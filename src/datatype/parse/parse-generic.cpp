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

#include "parse-generic.h"
#include "../../brackets.h"
#include "../../lexer.h"
#include "../../hash.h"
#include "../generator/gen-definition.h"
#include "../generator/gen-time.h"
#include "../generator/gen-numlist.h"
#include "../generator/gen-timlist.h"
#include "../generator/gen-list.h"
#include <cwctype>



_boo isPossibleListElement(const Tokens& tks)
{
   const _size length = tks.getLength();

   if (length <= 2 || !tks.second().isSymbol(L'[')
      || !tks.last().isSymbol(L']')
      || tks.hasIndependentSquareBrackets()) {
      return false;
   }

   if (length == 3) {
      throw SyntaxException(L"empty space between square brackets []",
         tks.last().line);
   }

   if (tks.first().type != Token::t_Word) {
      throw SyntaxException(
         L"square brackets [] can be preceded only by a variable name",
         tks.first().line);
   }

   return true;
}

Generator<_num>* parseListElementIndex(const Tokens& tks)
{
   const _size start = tks.getStart() + 2;
   const _size length = tks.getLength() - 3;
   const Tokens tks2(tks.list, start, length);

   Generator<_num>* num = parseNumber(tks2);
   if (num == nullptr) {
      throw SyntaxException(
         L"content of square brackets [] cannot be resolved to a number",
         tks.first().line);
   }

   return num;
}

_boo isPossibleBinary(const Tokens& tks)
{
   if (!tks.containsSymbol(L'?')) {
      return false;
   }

   BracketsInfo bi;
   const _int start = tks.getStart();
   const _int end = tks.getEnd();
   _int i;

   for (i = start; i <= end; i++){
      const Token& t = tks.listAt(i);

      if (bi.isBracketFree() && t.type == Token::t_Symbol && t.value.c == L'?') {
         break;
      }

      bi.refresh(t);
   }

   if (i == start) {
      throw SyntaxException(L"sign ? is preceded by empty space",
         (*tks.list)[i].line);
   }
   else if (i == end) {
      throw SyntaxException(L"sign ? is followed by empty space",
         (*tks.list)[i].line);
   }

   return true;
}

_boo isPossibleTernary(const Tokens& tks)
{
   if (!tks.containsSymbol(L'?') || !tks.containsSymbol(L':')) {
      return false;
   }

   BracketsInfo bi;
   _boo loop = true;
   const _int start = tks.getStart();
   const _int end = tks.getEnd();
   _int percentId = -1;
   _int colonId = -1;

   for (_int i = start; loop && i <= end; i++){
      const Token& t = tks.listAt(i);

      if (bi.isBracketFree() && t.type == Token::t_Symbol) {
         switch (t.value.c) {
            case L'?': {
               if (percentId == -1) {
                  percentId = i;
                  if (colonId != -1) {
                     loop = false;
                  }
               }
               else {
                  return false;
               }
               break;
            }
            case L':': {
               if (colonId == -1) {
                  colonId = i;
                  if (percentId != -1) {
                     loop = false;
                  }
               }
               break;
            }
         }
      }

      bi.refresh(t);
   }

   if (percentId > colonId) {
      throw SyntaxException(L"signs ? and : appear in reverse order",
         (*tks.list)[percentId].line);
   }
   else if (percentId == start) {
      throw SyntaxException(L"sign ? is preceded by empty space",
         (*tks.list)[percentId].line);
   }
   else if (colonId == end) {
      throw SyntaxException(L"sign : is followed by empty space",
         (*tks.list)[colonId].line);
   }
   else if (percentId + 1 == colonId) {
      throw SyntaxException(L"empty space between signs ? and :",
         (*tks.list)[percentId].line);
   }

   return true;
}


void checkLimitBySize(const Tokens& tks)
{
   if (tks.getLength() == 1) {
      const Token& tk = tks.first();
      if (tk.type == Token::t_Number && tk.originString != L"") {
         throw SyntaxException(L"collection cannot be limited by file size '" + tk.originString +
            L"' in this way. You have to iterate over files in a loop, add their size to a helper variable and provide a loop break condition",
            tk.line);
      }
   }
}


_boo isPossibleListElementMember(const Tokens& tks)
{
   const _size length = tks.getLength();

   if (length <= 3 || !tks.second().isSymbol(L'[')
      || !tks.penultimate().isSymbol(L']')
      || tks.hasIndependentSquareBrackets()) {
      return false;
   }

   if (length == 3) {
      throw SyntaxException(L"empty space between square brackets []",
         tks.last().line);
   }

   const Token& last = tks.last();

   if (last.type != Token::t_TwoWords) {
      return false;
   }

   if (last.value.h1 != HASH_NOTHING) {
      throw SyntaxException(L"square brackets [] should be followed by a time variable member",
         tks.last().line);
   }

   if (tks.first().type != Token::t_Word) {
      throw SyntaxException(
         L"square brackets [] can be preceded only by a variable name",
         tks.first().line);
   }

   return true;
}


void setNumericFilter(const Keyword& kw, Generator<_num>*& num, _def*& result)
{
   switch (kw) {
      case Keyword::kw_Every: {
         result = new Filter_EveryDef(result, num);
         break;
      }
      case Keyword::kw_Limit: {
         result = new Filter_LimitDef(result, num);
         break;
      }
      case Keyword::kw_Skip: {
         result = new Filter_SkipDef(result, num);
         break;
      }
   }
}

void setNumericFilter(const Keyword& kw, Generator<_num>*& num, Generator<_nlist>*& result)
{
   switch (kw) {
      case Keyword::kw_Every: {
         result = new Filter_Every<_num>(result, num);
         break;
      }
      case Keyword::kw_Limit: {
         result = new Filter_Limit<_num>(result, num);
         break;
      }
      case Keyword::kw_Skip: {
         result = new Filter_Skip<_num>(result, num);
         break;
      }
   }
}

void setNumericFilter(const Keyword& kw, Generator<_num>*& num, Generator<_tlist>*& result)
{
   switch (kw) {
      case Keyword::kw_Every: {
         result = new Filter_Every<_tim>(result, num);
         break;
      }
      case Keyword::kw_Limit: {
         result = new Filter_Limit<_tim>(result, num);
         break;
      }
      case Keyword::kw_Skip: {
         result = new Filter_Skip<_tim>(result, num);
         break;
      }
   }
}

void setNumericFilter(const Keyword& kw, Generator<_num>*& num, Generator<_list>*& result)
{
   switch (kw) {
      case Keyword::kw_Every: {
         result = new Filter_Every<_str>(result, num);
         break;
      }
      case Keyword::kw_Limit: {
         result = new Filter_Limit<_str>(result, num);
         break;
      }
      case Keyword::kw_Skip: {
         result = new Filter_Skip<_str>(result, num);
         break;
      }
   }
}

void setWhereFilter(Generator<_boo>*& boo, Attribute*& attr, const _boo& hasMemory, Generator<_nlist>*& result)
{
   result = new Filter_WhereNumber(result, boo);
}

void setWhereFilter(Generator<_boo>*& boo, Attribute*& attr, const _boo& hasMemory, Generator<_tlist>*& result)
{
   result = new Filter_WhereTime(result, boo);
}

void setWhereFilter(Generator<_boo>*& boo, Attribute*& attr, const _boo& hasMemory,  Generator<_list>*& result)
{
   result = new Filter_WhereString(result, boo, *attr);
}

void setWhereFilter(Generator<_boo>*& boo, Attribute*& attr, const _boo& hasMemory, _def*& result)
{
   result = new Filter_WhereDef(result, boo, attr, hasMemory);
}

void setOrderFilter(Attribute*& attr, const _boo& hasMemory, OrderBy<_num>*& order, Generator<_nlist>*& result)
{
   order = new Filter_OrderByNumber(result);
   result = order;
}

void setOrderFilter(Attribute*& attr, const _boo& hasMemory, OrderBy<_tim>*& order, Generator<_tlist>*& result)
{
   order = new Filter_OrderByTime(result);
   result = order;
}

void setOrderFilter(Attribute*& attr, const _boo& hasMemory, OrderBy<_str>*& order, Generator<_list>*& result)
{
   order = new Filter_OrderByString(result, attr);
   result = order;
}

void setOrderFilter(Attribute*& attr, const _boo& hasMemory, OrderBy<_str>*& order, _def*& result)
{
   order = new Filter_OrderByString(new Cast_D_L(result), attr);
   result = new OrderByCast(order, attr, hasMemory);
}
