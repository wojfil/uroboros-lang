/*
    This file is part of Uroboros.
    Uroboros is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Uroboros is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Uroboros. If not, see <http://www.gnu.org/licenses/>.
*/

#include "function.h"
#include "../exception.h"
#include "function/func-bool.h"
#include "function/func-number.h"
#include "function/func-string.h"
#include "function/func-time.h"
#include "function/func-list.h"
#include "function/func-aggr.h"
#include "function/func-attr.h"
#include "parse-gen.h"
#include "../util.h"
#include "../hash.h"
#include "generator/gen-generic.h"
#include "generator/gen-number.h"
#include "generator/gen-string.h"
#include "generator/gen-definition.h"
#include "../brackets.h"
#include "../var/var-runtime.h"
#include <cwctype>

// function parsing could have been done in a more generalized way
// on the other hand, current approach
// enables variety of precise error messages



_boo isPossibleFunction(const Tokens& tks)
{
   if (tks.getLength() < 3) {
      return false;
   }

   const Token& f = tks.first();
   const Token& s = tks.second();
   const Token& l = tks.last();

   return f.type == Token::t_Word
       && s.type == Token::t_Symbol
       && l.type == Token::t_Symbol
       && s.value.c == L'('
       && l.value.c == L')'
       && !tks.hasIndependentBrackets();
}

static std::vector<Tokens> toFunctionArgs(const Tokens& tks)
{
   Tokens tks2(tks);
   tks2.trimFunction();

   std::vector<Tokens> result;
   tks2.splitBySymbol(L',', result);

   return result;
}

Generator<_boo>* boolFunction(const Tokens& tks)
{
   const Token& word = tks.first();
   const _size& name = word.value.h1;
   const std::vector<Tokens> args = toFunctionArgs(tks);
   const _size len = args.size();

   if (HASH_GROUP_FUNC_BOO_STR.find(name) != HASH_GROUP_FUNC_BOO_STR.end())
   {
      if (len != 1) {
         functionArgNumberException(len, word);
      }

      return simpleBoolFunction(args[0], word);
   }
   else if (name == HASH_FUNC_EXIST) {
      if (len == 0) {
         functionArgNumberException(len, word);
      }

      _def* def = definitionGenerator(args[0]);
      if (def != nullptr) {
         if (len > 1) {
            checkInOperatorCommaAmbiguity(word, args[0]);
            functionArgNumberException(len, word);
         }

         return new F_AnyDef(def);
      }

      Generator<_list>* list = listGenerator(args[0]);
      if (list == nullptr) {
         functionArgException(1, L"list", word);
      }
      else {
         if (len > 1) {
            checkInOperatorCommaAmbiguity(word, args[0]);
            functionArgNumberException(len, word);
         }

         return new F_Exist(list);
      }
   }
   else if (name == HASH_FUNC_ANY) {
      if (len == 0) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str = stringGenerator(args[0]);
      if (str != nullptr) {
         delete str;
         throw SyntaxException(L"the argument of function '" + word.originString
            + L"' cannot be resolved to a collection", word.line);
      }

      _def* def = definitionGenerator(args[0]);
      if (def != nullptr) {
         if (len > 1) {
            checkInOperatorCommaAmbiguity(word, args[0]);
            functionArgNumberException(len, word);
         }
         return new F_AnyDef(def);
      }

      Generator<_tlist>* tlist = timListGenerator(args[0]);
      if (tlist != nullptr) {
         if (len > 1) {
            checkInOperatorCommaAmbiguity(word, args[0]);
            functionArgNumberException(len, word);
         }
         return new F_Any<_tim>(tlist);
      }

      Generator<_nlist>* nlist = numListGenerator(args[0]);
      if (nlist != nullptr) {
         if (len > 1) {
            checkInOperatorCommaAmbiguity(word, args[0]);
            functionArgNumberException(len, word);
         }
         return new F_Any<_num>(nlist);
      }

      Generator<_list>* list = listGenerator(args[0]);
      if (list == nullptr) {
         functionArgException(1, L"list", word);
      }
      else {
         if (len > 1) {
            checkInOperatorCommaAmbiguity(word, args[0]);
            functionArgNumberException(len, word);
         }

         return new F_Any<_str>(list);
      }
   }
   else if (name == HASH_FUNC_ANYINSIDE) {
      if (len == 0) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str1 = stringGenerator(args[0]);
      if (str1 != nullptr) {
         delete str1;
         throw SyntaxException(L"first argument of function '" + word.originString
            + L"' cannot be resolved to a collection", word.line);
      }

      _def* def = definitionGenerator(args[0]);
      if (def == nullptr) {
         functionArgException(1, L"definition", word);
      }

      if (len == 1) {
         checkFunctionAttribute(word);
         return new F_AnyInside(def, new GeneratorRef<_str>(&g_this_s));
      }

      checkInOperatorCommaAmbiguity(word, args[0]);

      if (len > 2) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str = stringGenerator(args[1]);
      if (str == nullptr) {
         delete def;
         functionArgException(2, L"string", word);
      }
      else {
         return new F_AnyInside(def, str);
      }
   }
   else if (name == HASH_FUNC_CONTAINS) {
      if (len <= 1) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str = stringGenerator(args[0]);
      if (str != nullptr) {
         Generator<_str>* str2 = stringGenerator(args[1]);
         if (str2 == nullptr) {
            functionArgException(2, L"string", word);
         }
         else {
            return new F_ContainsStr(str, str2);
         }
      }

      _def* def = definitionGenerator(args[0]);
      if (def != nullptr) {
         checkInOperatorCommaAmbiguity(word, args[0]);
         Generator<_str>* str2 = stringGenerator(args[1]);
         if (str2 == nullptr) {
            delete def;
            functionArgException(2, L"string", word);
         }
         else {
            if (len != 2) {
               functionArgNumberException(len, word);
            }
            return new F_ContainsDef(def, str2);
         }
      }

      Generator<_tlist>* tlist = timListGenerator(args[0]);
      if (tlist != nullptr) {
         checkInOperatorCommaAmbiguity(word, args[0]);
         Generator<_tim>* tim2 = timeGenerator(args[1]);
         if (tim2 == nullptr) {
            delete tlist;
            functionArgException(2, L"time", word);
         }
         else {
            if (len != 2) {
               functionArgNumberException(len, word);
            }
            return new F_ContainsCol<_tim>(tlist, tim2);
         }
      }

      Generator<_nlist>* nlist = numListGenerator(args[0]);
      if (nlist != nullptr) {
         checkInOperatorCommaAmbiguity(word, args[0]);
         Generator<_num>* num2 = numberGenerator(args[1]);
         if (num2 == nullptr) {
            delete nlist;
            functionArgException(2, L"number", word);
         }
         else {
            if (len != 2) {
               functionArgNumberException(len, word);
            }
            return new F_ContainsCol<_num>(nlist, num2);
         }
      }

      Generator<_list>* list = listGenerator(args[0]);
      if (list == nullptr) {
         checkInOperatorCommaAmbiguity(word, args[0]);
         functionArgException(1, L"list", word);
         throw SyntaxException(L"first argument of function '" + word.originString + L"' cannot be resolved to a string nor any collection", word.line);
      }
      else {
         Generator<_str>* str2 = stringGenerator(args[1]);
         if (str2 == nullptr) {
            delete list;
            functionArgException(2, L"string", word);
         }
         else {
            if (len != 2) {
               functionArgNumberException(len, word);
            }
            return new F_ContainsCol<_str>(list, str2);
         }
      }
   }
   else if (name == HASH_FUNC_EXISTSINSIDE) {
      if (len == 0 || len > 2) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str  = stringGenerator(args[0]);
      if (str == nullptr) {
         functionArgException(1, L"string", word);
      }

      if (len == 1) {
         checkFunctionAttribute(word);
         return new F_ExistsInside(str, new GeneratorRef<_str>(&g_this_s));
      }

      Generator<_str>* str2 = stringGenerator(args[1]);
      if (str2 == nullptr) {
         delete str;
         functionArgException(2, L"string", word);
      }
      else {
         return new F_ExistsInside(str, str2);
      }
   }
   else if (name == HASH_FUNC_EXISTINSIDE) {
      if (len == 0) {
         functionArgNumberException(len, word);
      }

      _def* def = definitionGenerator(args[0]);
      if (def != nullptr) {
         if (len == 1) {
            checkFunctionAttribute(word);
            return new F_AnyInside(def, new GeneratorRef<_str>(&g_this_s));
         }
         else {
            checkInOperatorCommaAmbiguity(word, args[0]);
            if (len > 2) {
               functionArgNumberException(len, word);
            }
            Generator<_str>* str2 = stringGenerator(args[1]);
            if (str2 == nullptr) {
               functionArgException(2, L"string", word);
            }
            else {
               return new F_AnyInside(def, str2);
            }
         }
      }

      Generator<_list>* list  = listGenerator(args[0]);
      if (list == nullptr) {
         functionArgException(1, L"list", word);
      }

      if (len == 1) {
         checkFunctionAttribute(word);
         return new F_ExistInside(list, new GeneratorRef<_str>(&g_this_s));
      }

      checkInOperatorCommaAmbiguity(word, args[0]);
      if (len > 2) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str2 = stringGenerator(args[1]);
      if (str2 == nullptr) {
         delete list;
         functionArgException(2, L"string", word);
      }
      else {
         return new F_ExistInside(list, str2);
      }
   }
   else if (name == HASH_FUNC_STARTSWITH) {
      if (len != 2) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str = stringGenerator(args[0]);
      if (str == nullptr) {
         functionArgException(1, L"string", word);
      }

      if (args[1].getLength() == 1) {
         const Token& f = args[1].first();
         switch (f.type) {
            case Token::t_Quotation: {
               switch (f.value.sl.size()) {
                  case 0: {
                     return new Constant<_boo>(true);
                  }
                  case 1: {
                     const _char ch = (f.value.sl)[0];
                     return new F_StartsWithChar(str, ch);
                  }
                  default: {
                     return new F_StartsWithConst(str, f.value.sl);
                  }
               }
            }
            case Token::t_Number: {
               const _str conv = f.value.n.toString();
               switch (conv.size()) {
                  case 0: {
                     return new Constant<_boo>(true);
                  }
                  case 1: {
                     const _char ch = conv[0];
                     return new F_StartsWithChar(str, ch);
                  }
                  default: {
                     return new F_StartsWithConst(str, conv);
                  }
               }
            }
         }
      }

      Generator<_str>* str2 = stringGenerator(args[1]);
      if (str2 == nullptr) {
         functionArgException(2, L"string", word);
      }

      return new F_StartsWith(str, str2);
   }
   else if (name == HASH_FUNC_ENDSWITH) {
      if (len != 2) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str = stringGenerator(args[0]);
      if (str == nullptr) {
         functionArgException(1, L"string", word);
      }

      if (args[1].getLength() == 1) {
         const Token& f = args[1].first();
         switch (f.type) {
            case Token::t_Quotation: {
               switch (f.value.sl.size()) {
                  case 0: {
                     return new Constant<_boo>(true);
                  }
                  case 1: {
                     const _char ch = (f.value.sl)[0];
                     return new F_EndsWithChar(str, ch);
                  }
                  default: {
                     return new F_EndsWithConst(str, f.value.sl);
                  }
               }
            }
            case Token::t_Number: {
               const _str conv = f.value.n.toString();
               switch (conv.size()) {
                  case 0: {
                     return new Constant<_boo>(true);
                  }
                  case 1: {
                     const _char ch = conv[0];
                     return new F_EndsWithChar(str, ch);
                  }
                  default: {
                     return new F_EndsWithConst(str, conv);
                  }
               }
            }
         }
      }

      Generator<_str>* str2 = stringGenerator(args[1]);
      if (str2 == nullptr) {
         functionArgException(2, L"string", word);
      }

      return new F_EndsWith(str, str2);
   }

   return nullptr;
}

static Generator<_boo>* simpleBoolFunction(const Tokens& tks, const Token& word)
{
   Generator<_str>* arg1 = stringGenerator(tks);
   if (arg1 == nullptr)
      functionArgException(1, L"string", word);

   const _size& name = word.value.h1;

   if (name == HASH_FUNC_ISLOWER)
      return new F_IsLower(arg1);
   else if (name == HASH_FUNC_ISUPPER)
      return new F_IsUpper(arg1);
   else if (name == HASH_FUNC_ISNUMBER)
      return new F_IsNumber(arg1);
   else if (name == HASH_VAR_ARCHIVE)
      return new F_Archive(arg1);
   else if (name == HASH_VAR_COMPRESSED)
      return new F_Compressed(arg1);
   else if (name == HASH_VAR_EMPTY)
      return new F_Empty(arg1);
   else if (name == HASH_VAR_ENCRYPTED)
      return new F_Encrypted(arg1);
   else if (name == HASH_VAR_EXISTS)
      return new F_Exists(arg1);
   else if (name == HASH_VAR_HIDDEN)
      return new F_Hidden(arg1);
   else if (name == HASH_VAR_ISDIRECTORY)
      return new F_IsDirectory(arg1);
   else if (name == HASH_VAR_ISFILE)
      return new F_IsFile(arg1);
   else if (name == HASH_VAR_READONLY)
      return new F_Readonly(arg1);
   else if (name == HASH_FUNC_ISLETTER)
      return new F_IsLetter(arg1);
   else if (name == HASH_FUNC_ISDIGIT)
      return new F_IsDigit(arg1);
   else if (name == HASH_FUNC_ISBINARY)
      return new F_IsBinary(arg1);
   else if (name == HASH_FUNC_ISHEX)
      return new F_IsHex(arg1);
   return nullptr;
}

Generator<_num>* numberFunction(const Tokens& tks)
{
   const Token& word = tks.first();
   const _size& name = word.value.h1;
   const std::vector<Tokens> args = toFunctionArgs(tks);
   const _size len = args.size();

   if (HASH_GROUP_FUNC_NUM_NUM.find(name) != HASH_GROUP_FUNC_NUM_NUM.end()) {
      if (len != 1)
         functionArgNumberException(len, word);

      return simpleNumberFunction(args[0], word);
   }
   else if (name == HASH_FUNC_LENGTH) {
      if (len != 1)
         functionArgNumberException(len, word);

      Generator<_str>* arg1 = stringGenerator(args[0]);
      if (arg1 == nullptr) {
         throw SyntaxException(L"the argument of function '" + word.originString + L"' cannot be resolved to a string. "
            L"If you want to count elements in a collection, use function 'count' instead", word.line);
      }
      else
         return new F_Length(arg1);
   }
   else if (name == HASH_FUNC_FROMBINARY || name == HASH_FUNC_FROMHEX) {
      if (len != 1)
         functionArgNumberException(len, word);

      Generator<_str>* arg1 = stringGenerator(args[0]);

      if (arg1 == nullptr) {
         functionArgException(1, L"string", word);
      }
      else {
         if (name == HASH_FUNC_FROMBINARY)
            return new F_FromBinary(arg1);
         else
            return new F_FromHex(arg1);
      }
   }
   else if (name == HASH_VAR_SIZE) {
      if (len == 0) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str = stringGenerator(args[0]);
      if (str != nullptr) {
         if (len != 1) {
            functionArgNumberException(len, word);
         }
         return new F_Size(str);
      }

      _def* def = definitionGenerator(args[0]);
      if (def != nullptr) {
         if (len != 1) {
            checkInOperatorCommaAmbiguity(word, args[0]);
            functionArgNumberException(len, word);
         }
         return new F_SizeDefinition(def);
      }

      Generator<_list>* list = listGenerator(args[0]);
      if (list == nullptr) {
         throw SyntaxException(L"the argument of function '" + word.originString + L"' cannot be resolved to a string nor a list. ", word.line);
      }
      else {
         if (len != 1) {
            checkInOperatorCommaAmbiguity(word, args[0]);
            functionArgNumberException(len, word);
         }
         return new F_SizeList(list);
      }
   }
   else if (name == HASH_FUNC_NUMBER) {
      if (len != 1)
         functionArgNumberException(len, word);

      Generator<_str>* arg1 = stringGenerator(args[0]);
      if (arg1 == nullptr) {
         throw SyntaxException(L"the argument of function '" + word.originString + L"' cannot be resolved to a string. ", word.line);
      }
      else
         return new F_Number(arg1);
   }
   else if (name == HASH_FUNC_COUNT) {
      if (len == 0) {
         throw SyntaxException(L"function '" + word.originString + L"' needs at least one argument", word.line);
      }

      std::vector<Generator<_tlist>*>* tlists = new std::vector<Generator<_tlist>*>();
      std::vector<Generator<_nlist>*>* nlists = new std::vector<Generator<_nlist>*>();
      std::vector<Generator<_list>*>* lists = new std::vector<Generator<_list>*>();
      std::vector<_def*>* defs = new std::vector<_def*>();

      for (_size i = 0; i < len; i++) {
         const Tokens& tks = args[i];
         Generator<_str>* str = stringGenerator(tks);
         if (str != nullptr) {
            delete str;

            throw SyntaxException(ordinalNumber(i + 1)
               + L" argument of the function '" + word.originString + L"' "
               L"is not a collection", word.line);
         }

         Generator<_tlist>* tlist = timListGenerator(tks);
         if (tlist != nullptr) {
            if (i != len - 1) {
               checkInOperatorCommaAmbiguity(word, args[i]);
            }
            tlists->push_back(tlist);
            continue;
         }

         Generator<_nlist>* nlist = numListGenerator(tks);
         if (nlist != nullptr) {
            if (i != len - 1) {
               checkInOperatorCommaAmbiguity(word, args[i]);
            }
            nlists->push_back(nlist);
            continue;
         }

         _def* def = definitionGenerator(tks);
         if (def != nullptr) {
            if (i != len - 1) {
               checkInOperatorCommaAmbiguity(word, args[i]);
            }
            defs->push_back(def);
            continue;
         }

         Generator<_list>* list = listGenerator(tks);
         if (list != nullptr) {
            if (i != len - 1) {
               checkInOperatorCommaAmbiguity(word, args[i]);
            }
            lists->push_back(list);
            continue;
         }

         deleteVectorPtr(tlists);
         deleteVectorPtr(nlists);
         deleteVectorPtr(lists);
         deleteVectorPtr(defs);

         throw SyntaxException(ordinalNumber(i + 1)
            + L" argument of the function '" + word.originString + L"' "
              L"is not a collection", word.line);
      }

      const _size tc = tlists->size();
      const _size nc = nlists->size();
      const _size lc = lists->size();
      const _size dc = defs->size();
      const _size sum = tc + nc + lc + dc;

      if (sum == 1) {
         Generator<_num>* result;

         if (tc == 1) {
            Generator<_tlist>* v = (*tlists)[0];
            tlists->clear();
            result = new F_CountUnit<_tim>(v);
         }
         else if (nc == 1) {
            Generator<_nlist>* v = (*nlists)[0];
            nlists->clear();
            result = new F_CountUnit<_num>(v);
         }
         else if (lc == 1) {
            Generator<_list>* v = (*lists)[0];
            lists->clear();
            result = new F_CountUnit<_str>(v);
         }
         else if (dc == 1) {
            _def* v = (*defs)[0];
            defs->clear();
            result = new F_CountUnitDef(v);
         }

         deleteVectorPtr(tlists);
         deleteVectorPtr(nlists);
         deleteVectorPtr(lists);
         deleteVectorPtr(defs);
         return result;
      }

      return new F_Count(tlists, nlists, lists, defs);
   }
   else if (name == HASH_FUNC_COUNTINSIDE) {
      if (len == 0) {
         functionArgNumberException(len, word);
      }

      _def* def = definitionGenerator(args[0]);
      if (def == nullptr) {
         functionArgException(1, L"definition", word);
      }

      if (len == 1) {
         checkFunctionAttribute(word);
         return new F_CountInside(def, new GeneratorRef<_str>(&g_this_s));
      }

      checkInOperatorCommaAmbiguity(word, args[0]);
      if (len > 2) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str = stringGenerator(args[1]);
      if (str == nullptr) {
         delete def;
         functionArgException(2, L"string", word);
      }
      else {
         return new F_CountInside(def, str);
      }
   }
   else if (name == HASH_FUNC_POWER) {
      if (len != 2)
         functionArgNumberException(len, word);

      Generator<_num>* arg1 = numberGenerator(args[0]);
      if (arg1 == nullptr)
         functionArgException(1, L"number", word);

      Generator<_num>* arg2 = numberGenerator(args[1]);
      if (arg2 == nullptr) {
         delete arg1;
         functionArgException(2, L"number", word);
      }

      return new F_Power(arg1, arg2);
   }
   else if (HASH_GROUP_AGGRFUNC.find(name) != HASH_GROUP_AGGRFUNC.end()) {
      if (len == 0) {
         throw SyntaxException(L"aggregate function '" + word.originString
            + L"' needs at least one argument", word.line);
      }

      return aggrFunction(args, word);
   }
   else if (name == HASH_FUNC_FIRST || name == HASH_FUNC_LAST) {
      if (len == 0) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str = stringGenerator(args[0]);
      if (str != nullptr) {
         delete str;

         throw SyntaxException(L"function '" + word.originString
            + L"' can only take a collection of values as an argument", word.line);
      }

      Generator<_nlist>* nlist = numListGenerator(args[0]);
      if (nlist != nullptr) {
         if (len != 1) {
            checkInOperatorCommaAmbiguity(word, args[0]);
            functionArgNumberException(len, word);
         }

         if (name == HASH_FUNC_FIRST) {
            return new F_First<_num>(nlist);
         }
         else {
            return new F_Last<_num>(nlist);
         }
      }
   }
   else if (name == HASH_FUNC_RANDOM) {
      if (len > 1) {
         functionArgNumberException(len, word);
      }

      if (len == 0) {
         return new F_Random();
      }

      Generator<_num>* num = numberGenerator(args[0]);
      if (num != nullptr) {
         return new F_RandomNumber(num);
      }

      Generator<_nlist>* nlist = numListGenerator(args[0]);
      if (nlist != nullptr) {
         return new F_RandomElement<_num>(nlist);
      }
   }

   return nullptr;
}

static Generator<_num>* simpleNumberFunction(const Tokens& tks, const Token& word)
{
   Generator<_num>* arg1 = numberGenerator(tks);
   if (arg1 == nullptr)
      functionArgException(1, L"number", word);

   const _size& name = word.value.h1;

   if (name == HASH_FUNC_ABSOLUTE)
      return new F_Absolute(arg1);
   else if (name == HASH_FUNC_CEIL)
      return new F_Ceil(arg1);
   else if (name == HASH_FUNC_FLOOR)
      return new F_Floor(arg1);
   else if (name == HASH_FUNC_ROUND)
      return new F_Round(arg1);
   else if (name == HASH_FUNC_SIGN)
      return new F_Sign(arg1);
   else if (name == HASH_FUNC_SQRT)
      return new F_Sqrt(arg1);
   else if (name == HASH_FUNC_TRUNCATE)
      return new F_Truncate(arg1);
   else
      return nullptr;
}

static Generator<_num>* aggrFunction(const std::vector<Tokens>& args, const Token& word)
{
   const _size& name = word.value.h1;

   std::vector<Generator<_num>*>* singles =
      new std::vector<Generator<_num>*>();
   std::vector<Generator<_nlist>*>* multis =
      new std::vector<Generator<_nlist>*>();

   const _size len = args.size();

   for (_size i = 0; i < len; i++) {
      const Tokens& tks = args[i];

      Generator<_num>* num = numberGenerator(tks);
      if (num != nullptr) {
         singles->push_back(num);
      }
      else {
         Generator<_nlist>* nlist = numListGenerator(tks);
         if (nlist == nullptr) {
            deleteVectorPtr(singles);
            deleteVectorPtr(multis);

            throw SyntaxException(ordinalNumber(i + 1)
               + L" argument of aggregate function '" + word.originString
               + L"' cannot be resolved to a number nor a numeric list", word.line);
         }
         else {
            multis->push_back(nlist);
         }
      }
   }

   if (name == HASH_FUNC_AVERAGE)
      return new F_Average(singles, multis);
   else if (name == HASH_FUNC_MAX)
      return new F_Max(singles, multis);
   else if (name == HASH_FUNC_MEDIAN)
      return new F_Median(singles, multis);
   else if (name == HASH_FUNC_MIN)
      return new F_Min(singles, multis);
   else if (name == HASH_FUNC_SUM)
      return new F_Sum(singles, multis);
   else
      return nullptr;
}

Generator<_per>* periodFunction(const Tokens& tks)
{
   const Token& word = tks.first();
   const _size& name = word.value.h1;
   const std::vector<Tokens> args = toFunctionArgs(tks);
   const _size len = args.size();

   if (name == HASH_FUNC_LIFETIME) {
      if (len != 1)
         functionArgNumberException(len, word);

      Generator<_str>* str = stringGenerator(args[0]);
      if (str == nullptr) {
         functionArgException(1, L"string", word);
      }
      else {
         return new F_Lifetime(str);
      }
   }

   return nullptr;
}

Generator<_str>* stringFunction(const Tokens& tks)
{
   const Token& word = tks.first();
   const _size& name = word.value.h1;
   const std::vector<Tokens> args = toFunctionArgs(tks);
   const _size len = args.size();

   if (name == HASH_FUNC_AFTER || name == HASH_FUNC_BEFORE) {
      if (len != 2)
         functionArgNumberException(len, word);

      return stringTwoArgFunction(args, word);
   }
   else if (HASH_GROUP_FUNC_STR_STR.find(name) != HASH_GROUP_FUNC_STR_STR.end()) {

      if (len != 1)
         functionArgNumberException(len, word);

      return simpleStringFunction(args[0], word);
   }
   else if (name == HASH_FUNC_REVERSED) {
      const _str sub = word.originString.substr(0, 7);
      throw SyntaxException(L"the proper name for this function is '" + sub + L"'", word.line);
   }
   else if (HASH_GROUP_FUNC_STR_STR_NUM.find(name) != HASH_GROUP_FUNC_STR_STR_NUM.end()) {
      if (len != 2) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str = stringGenerator(args[0]);
      if (str == nullptr) {
         functionArgException(1, L"string", word);
      }

      Generator<_num>* num = numberGenerator(args[1]);
      if (num == nullptr) {
         delete str;
         functionArgException(2, L"number", word);
      }

      if (name == HASH_FUNC_REPEAT)
         return new F_Repeat(str, num);
      else if (name == HASH_FUNC_LEFT)
         return new F_Left(str, num);
      else if (name == HASH_FUNC_RIGHT)
         return new F_Right(str, num);
      else if (name == HASH_FUNC_FILL)
         return new F_Fill(str, num);
   }
   else if (name == HASH_FUNC_REPLACE) {
      if (len != 3) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str1 = stringGenerator(args[0]);
      if (str1 == nullptr) {
         functionArgException(1, L"string", word);
      }

      Generator<_str>* str2 = stringGenerator(args[1]);
      if (str2 == nullptr) {
         delete str1;
         functionArgException(2, L"string", word);
      }

      Generator<_str>* str3 = stringGenerator(args[2]);
      if (str3 == nullptr) {
         delete str1;
         delete str2;
         functionArgException(3, L"string", word);
      }

      return new F_Replace(str1, str2, str3);
   }
   else if (name == HASH_FUNC_SUBSTRING) {
      if (len < 2 || len > 3) {
         throw SyntaxException(L"function '" + word.originString + L"' can only take "
            L" two or three arguments", word.line);
      }

      Generator<_str>* str = stringGenerator(args[0]);
      if (str == nullptr) {
         functionArgException(1, L"string", word);
      }

      Generator<_num>* num = numberGenerator(args[1]);
      if (num == nullptr) {
         delete str;
         functionArgException(2, L"number", word);
      }

      if (len == 2) {
         return new F_Substring_2(str, num);
      }

      Generator<_num>* num2 = numberGenerator(args[2]);
      if (num2 == nullptr) {
         delete str;
         delete num;
         functionArgException(3, L"number", word);
      }

      return new F_Substring_3(str, num, num2);
   }
   else if (name == HASH_FUNC_CONCATENATE) {
      if (len < 1) {
         throw SyntaxException(L"function '" + word.originString + L"' needs at least one arguments", word.line);
      }

      std::vector<Generator<_str>*>* values = new std::vector<Generator<_str>*>();

      for (_size i = 0; i < len; i++) {
         Generator<_str>* str = stringGenerator(args[i]);
         if (str != nullptr) {
            values->push_back(str);
            continue;
         }

         Generator<_list>* list = listGenerator(args[i]);
         if (list == nullptr) {
            deleteVectorPtr(values);

            throw SyntaxException(ordinalNumber(i + 1)
               + L" argument of the function '" + word.originString + L"' cannot be resolved to any data type", word.line);
         }
         else {
            if (i != len - 1) {
               checkInOperatorCommaAmbiguity(word, args[i]);
            }
            values->push_back(new F_ConcatenateUnit(list));
         }
      }

      return new F_Concatenate(values);
   }
   else if (name == HASH_FUNC_FIRST || name == HASH_FUNC_LAST) {
      if (len == 0) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str = stringGenerator(args[0]);
      if (str != nullptr) {
         delete str;

         throw SyntaxException(L"function '" + word.originString
            + L"' can only take a collection of values as an argument", word.line);
      }

      _def* def = definitionGenerator(args[0]);
      if (def != nullptr) {
         if (len != 1) {
            checkInOperatorCommaAmbiguity(word, args[0]);
            functionArgNumberException(len, word);
         }

         if (name == HASH_FUNC_FIRST) {
            return new F_FirstDef(def);
         }
         else {
            return new F_LastDef(def);
         }
      }

      Generator<_list>* list = listGenerator(args[0]);
      if (list != nullptr) {
         if (len != 1) {
            checkInOperatorCommaAmbiguity(word, args[0]);
            functionArgNumberException(len, word);
         }

         if (name == HASH_FUNC_FIRST) {
            return new F_First<_str>(list);
         }
         else {
            return new F_Last<_str>(list);
         }
      }
      else {
         throw SyntaxException(L"the argument of function '" + word.originString
            + L"' cannot be resolved to any collection", word.line);
      }
   }
   else if (name == HASH_FUNC_PATH) {
      if (len == 0) {
         functionArgNumberException(len, word);
      }

      if (len > 4) {
         std::vector<Generator<_str>*>* values = new std::vector<Generator<_str>*>();

         for (_size i = 0; i < len; i++) {
            Generator<_str>* str = stringGenerator(args[i]);
            if (str == nullptr) {
               deleteVectorPtr(values);
               functionArgException(i + 1, L"string", word);
            }

            values->push_back(str);
         }

         return new F_Path_Multi(values);
      }
      else {
         Generator<_str>* str1 = stringGenerator(args[0]);
         if (str1 == nullptr) {
            functionArgException(1, L"string", word);
         }

         if (len == 1) {
            return new F_Path(str1);
         }

         Generator<_str>* str2 = stringGenerator(args[1]);
         if (str2 == nullptr) {
            delete str1;
            functionArgException(2, L"string", word);
         }

         if (len == 2) {
            return new F_Path_2(str1, str2);
         }

         Generator<_str>* str3 = stringGenerator(args[2]);
         if (str3 == nullptr) {
            delete str1;
            delete str2;
            functionArgException(3, L"string", word);
         }

         if (len == 3) {
            return new F_Path_3(str1, str2, str3);
         }

         Generator<_str>* str4 = stringGenerator(args[3]);
         if (str4 == nullptr) {
            delete str1;
            delete str2;
            delete str3;
            functionArgException(4, L"string", word);
         }

         return new F_Path_4(str1, str2, str3, str4);
      }
   }
   else if (name == HASH_FUNC_STRING) {
      if (len != 1) {
         functionArgNumberException(len, word);
      }

      Generator<_boo>* boo = boolGenerator(args[0]);
      if (boo != nullptr) {
         return new F_String_B(boo);
      }

      Generator<_num>* num = numberGenerator(args[0]);
      if (num != nullptr) {
         return new F_String_N(num);
      }

      Generator<_tim>* tim = timeGenerator(args[0]);
      if (tim != nullptr) {
         return new F_String_T(tim);
      }

      Generator<_per>* per = periodGenerator(args[0]);
      if (per != nullptr) {
         return new F_String_P(per);
      }

      throw SyntaxException(L"the argument of function '"
         + word.originString + L"' cannot be resolved to any singular data type. If you want to concatenate a collection, use function 'concatenate' instead", word.line);
   }
   else if (HASH_GROUP_FUNC_STR_NUM.find(name) != HASH_GROUP_FUNC_STR_NUM.end()) {
      if (len != 1) {
         functionArgNumberException(len, word);
      }

      Generator<_num>* num = numberGenerator(args[0]);
      if (num == nullptr) {
         functionArgException(1, L"number", word);
      }
      else {
         if (name == HASH_FUNC_ROMAN)
            return new F_Roman(num);
         else if (name == HASH_FUNC_BINARY)
            return new F_Binary(num);
         else if (name == HASH_FUNC_HEX)
            return new F_Hex(num);
      }
   }
   else if (name == HASH_FUNC_MONTHNAME) {
      if (len != 1) {
         functionArgNumberException(len, word);
      }

      Generator<_num>* num = numberGenerator(args[0]);
      if (num != nullptr) {
         return new F_MonthName(num);
      }
      else {
         functionArgException(1, L"number", word);
      }
   }
   else if (name == HASH_FUNC_WEEKDAYNAME) {
      if (len != 1) {
         functionArgNumberException(len, word);
      }

      Generator<_num>* num = numberGenerator(args[0]);
      if (num != nullptr) {
         return new F_WeekDayName(num);
      }
      else {
         functionArgException(1, L"number", word);
      }
   }
   else if (name == HASH_FUNC_RANDOM) {
      if (len == 0) {
         return nullptr;
      }

      Generator<_str>* str = stringGenerator(args[0]);
      if (str != nullptr) {
         if (len > 1) {
            functionArgNumberException(len, word);
         }
         return new F_RandomChar(str);
      }

      Generator<_list>* list = listGenerator(args[0]);
      if (list != nullptr) {
         if (len > 1) {
            checkInOperatorCommaAmbiguity(word, args[0]);
            functionArgNumberException(len, word);
         }

         return new F_RandomElement<_str>(list);
      }
      else {
         throw SyntaxException(L"wrong arguments of function '" + word.originString, word.line);
      }
   }
   else if (name == HASH_FUNC_JOIN) {
      if (len == 0) {
         functionArgNumberException(len, word);
      }

      Generator<_list>* list = listGenerator(args[0]);
      if (list == nullptr) {
         functionArgException(1, L"list", word);
      }

      if (len == 1) {
         throw SyntaxException(L"function '" + word.originString
            + L"' cannot be called with one argument. If you want to join multiple strings without a separator, use function 'concatenate' instead", word.line);
      }

      checkInOperatorCommaAmbiguity(word, args[0]);

      if (len != 2) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str = stringGenerator(args[1]);
      if (str == nullptr) {
         functionArgException(2, L"string", word);
      }

      return new F_Join(list, str);
   }

   return nullptr;
}

static Generator<_str>* stringTwoArgFunction(const std::vector<Tokens>& args, const Token& word)
{
   Generator<_str>* arg1 = stringGenerator(args[0]);
   if (arg1 == nullptr)
      functionArgException(1, L"string", word);

   Generator<_str>* arg2 = stringGenerator(args[1]);
   if (arg2 == nullptr) {
      delete arg1;
      functionArgException(2, L"string", word);
   }

   const _size& name = word.value.h1;

   if (name == HASH_FUNC_AFTER)
      return new F_After(arg1, arg2);
   else if (name == HASH_FUNC_BEFORE)
      return new F_Before(arg1, arg2);

   return nullptr;
}

static Generator<_str>* simpleStringFunction(const Tokens& tks, const Token& word)
{
   Generator<_str>* arg1 = stringGenerator(tks);
   if (arg1 == nullptr) {
      functionArgException(1, L"string", word);
   }

   const _size& name = word.value.h1;

   if (name == HASH_FUNC_DIGITS)
      return new F_Digits(arg1);
   else if (name == HASH_FUNC_LETTERS)
      return new F_Letters(arg1);
   else if (name == HASH_FUNC_LOWER)
      return new F_Lower(arg1);
   else if (name == HASH_FUNC_TRIM)
      return new F_Trim(arg1);
   else if (name == HASH_FUNC_UPPER)
      return new F_Upper(arg1);
   else if (name == HASH_FUNC_REVERSE)
      return new F_Reverse(arg1);
   else if (name == HASH_VAR_DRIVE)
      return new F_Drive(arg1);
   else if (name == HASH_VAR_EXTENSION)
      return new F_Extension(arg1);
   else if (name == HASH_VAR_FULLNAME)
      return new F_Fullname(arg1);
   else if (name == HASH_VAR_NAME)
      return new F_Name(arg1);
   else if (name == HASH_VAR_PARENT)
      return new F_Parent(arg1);
   else if (name == HASH_FUNC_AFTERDIGITS)
      return new F_AfterDigits(arg1);
   else if (name == HASH_FUNC_AFTERLETTERS)
      return new F_AfterLetters(arg1);
   else if (name == HASH_FUNC_BEFOREDIGITS)
      return new F_BeforeDigits(arg1);
   else if (name == HASH_FUNC_BEFORELETTERS)
      return new F_BeforeLetters(arg1);
   else if (name == HASH_FUNC_CAPITALIZE)
      return new F_Capitalize(arg1);
   else
      return nullptr;
}

Generator<_tim>* timeFunction(const Tokens& tks)
{
   const Token& word = tks.first();
   const _size& name = word.value.h1;
   const std::vector<Tokens> args = toFunctionArgs(tks);
   const _size len = args.size();

   if (HASH_GROUP_FUNC_TIM_NUM.find(name) != HASH_GROUP_FUNC_TIM_NUM.end()) {
      if (len != 1)
         functionArgNumberException(len, word);

      return simpleTimeFunction(args[0], word);
   }

   if (HASH_GROUP_TIME_ATTR.find(name) != HASH_GROUP_TIME_ATTR.end()) {

      if (len != 1)
         functionArgNumberException(len, word);

      Generator<_str>* arg1 = stringGenerator(args[0]);
      if (arg1 == nullptr)
         functionArgException(1, L"string", word);

      if (name == HASH_VAR_ACCESS)
         return new F_Access(arg1);
      else if (name == HASH_VAR_CHANGE)
         return new F_Change(arg1);
      else if (name == HASH_VAR_CREATION)
         return new F_Creation(arg1);
      else
         return new F_Modification(arg1);
   }

   else if (name == HASH_FUNC_DATE) {
      if (len != 3)
         functionArgNumberException(len, word);

      Generator<_num>* arg1 = numberGenerator(args[0]);
      if (arg1 == nullptr)
         functionArgException(1, L"number", word);

      Generator<_num>* arg2 = numberGenerator(args[1]);
      if (arg2 == nullptr) {
         delete arg1;
         functionArgException(2, L"number", word);
      }

      Generator<_num>* arg3 = numberGenerator(args[2]);
      if (arg3 == nullptr) {
         delete arg1;
         delete arg2;
         functionArgException(3, L"number", word);
      }

      return new F_Time_3(arg1, arg2, arg3);
   }
   else if (name == HASH_FUNC_TIME) {
      if (len < 2 || len == 4 || len > 6) {
         functionArgNumberException(len, word);
      }

      Generator<_num>* arg1 = numberGenerator(args[0]);
      if (arg1 == nullptr)
         functionArgException(1, L"number", word);

      Generator<_num>* arg2 = numberGenerator(args[1]);
      if (arg2 == nullptr) {
         delete arg1;
         functionArgException(2, L"number", word);
      }

      if (len == 2) {
         return new F_Time_2(arg1, arg2);
      }

      Generator<_num>* arg3 = numberGenerator(args[2]);
      if (arg3 == nullptr) {
         delete arg1;
         delete arg2;
         functionArgException(3, L"number", word);
      }

      if (len == 3) {
         return new F_Time_3(arg1, arg2, arg3);
      }

      Generator<_num>* arg4 = numberGenerator(args[3]);
      if (arg4 == nullptr) {
         delete arg1;
         delete arg2;
         delete arg3;
         functionArgException(4, L"number", word);
      }

      Generator<_num>* arg5 = numberGenerator(args[4]);
      if (arg5 == nullptr) {
         delete arg1;
         delete arg2;
         delete arg3;
         delete arg4;
         functionArgException(5, L"number", word);
      }

      if (len == 5) {
         return new F_Time_5(arg1, arg2, arg3, arg4, arg5);
      }

      Generator<_num>* arg6 = numberGenerator(args[5]);
      if (arg6 == nullptr) {
         delete arg1;
         delete arg2;
         delete arg3;
         delete arg4;
         delete arg5;
         functionArgException(6, L"number", word);
      }

      return new F_Time_6(arg1, arg2, arg3, arg4, arg5, arg6);
   }
   else if (name == HASH_FUNC_FIRST || name == HASH_FUNC_LAST) {
      if (len == 0) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str = stringGenerator(args[0]);
      if (str != nullptr) {
         delete str;

         throw SyntaxException(L"function '" + word.originString
            + L"' can only take a collection of values as an argument", word.line);
      }

      Generator<_tlist>* tlist = timListGenerator(args[0]);
      if (tlist != nullptr) {
         if (len != 1) {
            checkInOperatorCommaAmbiguity(word, args[0]);
            functionArgNumberException(len, word);
         }

         if (name == HASH_FUNC_FIRST) {
            return new F_First<_tim>(tlist);
         }
         else {
            return new F_Last<_tim>(tlist);
         }
      }
   }
   else if (name == HASH_FUNC_RANDOM) {
      if (len > 1) {
         functionArgNumberException(len, word);
      }

      if (len == 0) {
         return nullptr;
      }

      Generator<_tlist>* tlist = timListGenerator(args[0]);
      if (tlist != nullptr) {
         return new F_RandomElement<_tim>(tlist);
      }
   }

   return nullptr;
}

static Generator<_tim>* simpleTimeFunction(const Tokens& tks, const Token& word)
{
   Generator<_num>* arg1 = numberGenerator(tks);
   if (arg1 == nullptr) {
      functionArgException(1, L"number", word);
   }

   const _size& name = word.value.h1;

   if (name == HASH_FUNC_CHRISTMAS)
      return new F_Christmas(arg1);
   else if (name == HASH_FUNC_EASTER)
      return new F_Easter(arg1);
   else if (name == HASH_FUNC_NEWYEAR)
      return new F_NewYear(arg1);
   else
      return nullptr;
}





static void functionArgNumberException(const _int& argNumber, const Token& word)
{
   throw SyntaxException(L"function '" + word.originString + L"' cannot be called with "
      + toStr(argNumber) + L" argument" + (argNumber == 1 ? L"" : L"s"), word.line);
}

static void functionArgException(const _int& argNumber, const _str& typeName, const Token& word)
{
   throw SyntaxException(ordinalNumber(argNumber) + L" argument of function '"
      + word.originString + L"' cannot be resolved to a " + typeName, word.line);
}

static _str ordinalNumber(const _int& number)
{
   switch (number) {
      case 1:
         return L"first";
      case 2:
         return L"second";
      case 3:
         return L"third";
      case 4:
         return L"fourth";
      case 5:
         return L"fifth";
      default: {
         if (number >= 11 && number <= 19) {
            return toStr(number) + L"th";
         }

         switch (number % 10) {
            case 1:
               return toStr(number) + L"st";
            case 2:
               return toStr(number) + L"nd";
            case 3:
               return toStr(number) + L"rd";
            default:
               return toStr(number) + L"th";
         }
      }
   }
}

Generator<_list>* listFunction(const Tokens& tks)
{
   const Token& word = tks.first();
   const _size& name = word.value.h1;
   const std::vector<Tokens> args = toFunctionArgs(tks);
   const _size len = args.size();

   if (name == HASH_FUNC_CHARACTERS || name == HASH_FUNC_WORDS) {
      if (len != 1) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str = stringGenerator(args[0]);
      if (str == nullptr) {
         functionArgException(1, L"string", word);
      }

      if (name == HASH_FUNC_CHARACTERS)
         return new F_Characters(str);
      else
         return new F_Words(str);
   }
   if (name == HASH_FUNC_SPLIT) {
      if (len != 2) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str1 = stringGenerator(args[0]);
      if (str1 == nullptr) {
         functionArgException(1, L"string", word);
      }

      Generator<_str>* str2 = stringGenerator(args[1]);
      if (str2 == nullptr) {
         delete str1;
         functionArgException(2, L"string", word);
      }

      return new F_Split(str1, str2);
   }

   throw SyntaxException(L"function with name '" + word.originString + L"' does not exist", word.line);
}


Generator<_nlist>* numListFunction(const Tokens& tks)
{
   const Token& word = tks.first();
   const _size& name = word.value.h1;
   const std::vector<Tokens> args = toFunctionArgs(tks);
   const _size len = args.size();

   if (name == HASH_FUNC_NUMBERS) {
      if (len != 1) {
         functionArgNumberException(len, word);
      }

      Generator<_str>* str = stringGenerator(args[0]);
      if (str == nullptr) {
         functionArgException(1, L"string", word);
      }

      return new F_Numbers(str);
   }

   return nullptr;
}


static void checkFunctionAttribute(const Token& word)
{
   if (g_attrs.size() == 0 || g_thisstate != ThisState::ts_String) {
      throw SyntaxException(L"function '" + word.originString
         + L"' can be called only inside an iteration loop", word.line);
   }
}

static void checkInOperatorCommaAmbiguity(const Token& word, const Tokens& tks)
{
   BracketsInfo bi;
   _int end = tks.getEnd();
   _int start = tks.getStart();
   _int index = -1;

   for (_int i = end; i >= start; i--)
   {
      const Token& t = tks.listAt(i);

      if (t.isKeyword(Keyword::kw_In)) {
         if (bi.isBracketFree()) {
            index = i;
            break;
         }
      }
      else {
         bi.refresh(t);
      }
   }

   if (index == -1) {
      return;
   }

   Tokens right(tks);
   right.setRange(index + 1, tks.getLength() + start - index - 1);

   if (right.first().isSymbol(L'(')) {
      return;
   }

   const Token& inToken = tks.listAt(index);
   throw SyntaxException(L"the right side of the operator '" + inToken.originString
      + L"' used inside a function '" + word.originString
      + L"' should be embraced by brackets. Comma is a function argument separator and causes ambiguity here", word.line);
}
