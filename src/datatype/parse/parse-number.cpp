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

#include "parse-number.h"
#include "parse-generic.h"
#include "../function.h"
#include "../generator/gen-number.h"
#include "../generator/gen-generic.h"
#include "../parse-gen.h"
#include "../../brackets.h"
#include "../../hash.h"


// within an expression, minus sign can either mean subtraction operation (x-y) or unary negation (-x)
// this sign is used to distinguish them
const _char UNARY_MINUS = L'~';


Generator<_num>* parseNumber(const Tokens& tks)
{
   const _size len = tks.getLength();
   if (len == 1) {
      const Token& f = tks.first();
      switch (f.type) {
         case Token::t_Number: {
            return new Constant<_num>(f.value.n);
         }
         case Token::t_Word: {
            Generator<_num>* var;
            return getVarValue(f, var) ? var : nullptr;
         }
         case Token::t_TwoWords: {
            if (f.value.h1 == HASH_NOTHING) {
               throw SyntaxException(L"dot . should be preceded by a time variable name", f.line);
            }

            Generator<_tim>* var;
            if (!getVarValue(f, var)) {
               throw SyntaxException(str(L"time variable from expression '", f.originString,
                  L"' does not exist or is unreachable here"), f.line);
            }

            const _size& h = f.value.h2;

            if (h == HASH_PER_YEAR || h == HASH_PER_YEARS)
               return new TimeYears(var);
            else if (h == HASH_PER_MONTH || h == HASH_PER_MONTHS)
               return new TimeMonths(var);
            else if (h == HASH_PER_WEEKDAY)
               return new TimeWeekDay(var);
            else if (h == HASH_PER_DAY || h == HASH_PER_DAYS)
               return new TimeDays(var);
            else if (h == HASH_PER_HOUR || h == HASH_PER_HOURS)
               return new TimeHours(var);
            else if (h == HASH_PER_MINUTE || h == HASH_PER_MINUTES)
               return new TimeMinutes(var);
            else if (h == HASH_PER_SECOND || h == HASH_PER_SECOND)
               return new TimeSeconds(var);
            else if (h == HASH_PER_DATE)
               return nullptr;
            else {
               timeVariableMemberException(f);
            }
         }
         default: {
            return nullptr;
         }
      }
   }

   if (isPossibleFunction(tks)) {
      Generator<_num>* func = numberFunction(tks);
      if (func != nullptr) {
         return func;
      }
   }
   else if (len >= 2 && !tks.containsSymbol(L',')) {
      // build numeric expression (but only if the sequence has any operator)
      BracketsInfo bi;
      const _int end = tks.getEnd();
      const _int start = tks.getStart();
      _boo anyOperator = false;

      for (_int i = start; i <= end; i++) {
         const Token& t = tks.listAt(i);
         if (t.type == Token::t_Symbol && isNumExpOperator(t.value.c) && bi.isBracketFree()
             && !(i == start && t.value.c == L'-'))
         {
            Generator<_num>* num = parseNumExp(tks);

            if (num != nullptr) {
               return num;
            }
            else {
               std::vector<Tokens> elements;
               tks.splitBySymbol(L'+', elements);
               const _size elen = elements.size();

               if (elen == 1) {
                  break;
               }

               for (_size i = 0; i < elen; i++) {
                  const Tokens& tkse = elements[i];
                  Generator<_str>* str;

                  if (parse(tkse, str)) {
                     delete str;
                  }
                  else {
                     throw SyntaxException(L"syntax of an expression is not valid", tkse.first().line);
                  }
               }
            }
            anyOperator = true;
            break;
         }
         bi.refresh(t);
      }

      if (!anyOperator && tks.first().isSymbol(L'-')) {
         Tokens tks2(tks);
         tks2.trimLeft();
         Generator<_num>* num;

         if (parse(tks2, num)) {
            return new Negation(num);
         }
         else {
            Generator<_per>* per;
            if (parse(tks2, per)) {
               delete per;
            }
            else {
               throw SyntaxException(L"sign '-' is not followed by a valid number nor a valid period", tks.first().line);
            }
         }
      }
   }

   Generator<_num>* el = parseCollectionElement<_num>(tks);
   if (el != nullptr) {
      return el;
   }

   if (isPossibleListElementMember(tks)) {
      Tokens tksm(tks);
      tksm.trimRight();

      Generator<_num>* num = parseListElementIndex(tksm);
      const Token& f = tks.first();
      Generator<_tlist>* tlist;
      if (getVarValue(f, tlist)) {
         const Token& last = tks.last();
         const _size& h = last.value.h2;

         if (h == HASH_PER_YEAR || h == HASH_PER_YEARS)
            return new TimeYearsAtIndex(tlist, num);
         else if (h == HASH_PER_MONTH || h == HASH_PER_MONTHS)
            return new TimeMonthsAtIndex(tlist, num);
         else if (h == HASH_PER_WEEKDAY)
            return new TimeWeekDayAtIndex(tlist, num);
         else if (h == HASH_PER_DAY || h == HASH_PER_DAYS)
            return new TimeDaysAtIndex(tlist, num);
         else if (h == HASH_PER_HOUR || h == HASH_PER_HOURS)
            return new TimeHoursAtIndex(tlist, num);
         else if (h == HASH_PER_MINUTE || h == HASH_PER_MINUTES)
            return new TimeMinutesAtIndex(tlist, num);
         else if (h == HASH_PER_SECOND || h == HASH_PER_SECONDS)
            return new TimeSecondsAtIndex(tlist, num);
         else if (h == HASH_PER_DATE)
            return nullptr;
         else
            timeVariableMemberException(last);
      }
      else {
         delete num;
      }
   }

   Generator<_num>* bin = parseBinary<_num>(tks);
   if (bin != nullptr) {
      return bin;
   }

   Generator<_num>* tern = parseTernary<_num>(tks);
   if (tern != nullptr) {
      return tern;
   }

   return nullptr;
}


// build numeric expression
// multiple numbers connected with signs +-*/% and brackets ()
static Generator<_num>* parseNumExp(const Tokens& tks)
{
   std::vector<ExpElement<_num>*> infList; // infix notation list
   const _int start = tks.getStart();
   const _int end = tks.getEnd();
   _int sublen = 0, lv1 = 0, lv2 = 0;
   _boo prev = false;

   for (_int i = start; i <= end; i++) {
      const Token& t = tks.listAt(i);
      if (t.type == Token::t_Symbol) {
         const _char& ch = t.value.c;
         const _boo free = (lv1 == 0) && (lv2 == 0);
         if (isNumExpOperator(ch)) {
            if (sublen == 0) {
               // _character '-' can represent either binary subtraction
               // or unary negation
               const _char ch2 = (ch == L'-' &&
                  (prev || i == start)) ? UNARY_MINUS : ch;
               infList.push_back(new ExpElement<_num>(ch2));
            }
            else {
               if (free) {
                  const Tokens tks2(tks.list, i - sublen, sublen);

                  if (tks2.getLength() == 1
                     && tks2.first().type == Token::t_Number) {

                     const _num num = tks2.first().value.n;
                     infList.push_back(new ExpElement<_num>(num));
                     infList.push_back(new ExpElement<_num>(ch));
                     sublen = 0;
                  }
                  else {
                     Generator<_num>* num;
                     if (parse(tks2, num)) {
                        infList.push_back(new ExpElement<_num>(num));
                        infList.push_back(new ExpElement<_num>(ch));
                        sublen = 0;
                     }
                     else {
                        deleteVector(infList);
                        return nullptr;
                     }
                  }
               }
               else {
                  sublen++;
               }
            }
            prev = true;
            continue;
         }

         switch (ch) {
            case L'(': {
               if (free) {
                  if (sublen == 1) {
                     const Token& pt = tks.listAt(i - 1);
                     if (pt.type != Token::t_Word) {
                        deleteVector(infList);
                        throw SyntaxException(L"function name is not valid", t.line);
                     }
                  }
               }
               lv1++;
               sublen++;
               prev = false;
               break;
            }
            case L')': {
               lv1--;
               sublen++;
               prev = false;
               break;
            }
            case L'[': {
               lv2++;
               sublen++;
               prev = false;
               break;
            }
            case L']': {
               lv2--;
               sublen++;
               prev = false;
               break;
            }
            default: {
               sublen++;
               prev = false;
               break;
            }
         }
      }
      else {
         prev = false;
         sublen++;
      }
   }

   if (sublen != 0) {
      Tokens tks2(tks.list, 1 + end - sublen, sublen);

      if (tks2.getLength() == 1 && tks2.first().type == Token::t_Number) {
         infList.push_back(new ExpElement<_num>(tks2.first().value.n));
      }
      else {
         Generator<_num>* num;
         if (parse(tks2, num)) {
            infList.push_back(new ExpElement<_num>(num));
         }
         else {
            deleteVector(infList);
            return nullptr;
         }
      }
   }

   if (!isNumExpComputable(infList)) {
      deleteVector(infList);
      throw SyntaxException(L"syntax of a numeric expression is not valid",
         tks.first().line);
   }

   std::vector<ExpElement<_num>*> pntList;
   Generator<_num>* num = numExpTree(infList, pntList);

   deleteVector(infList);
   deleteVector(pntList);

   return num;
}

static Generator<_num>* numExpTree(const std::vector<ExpElement<_num>*>& infList,
   std::vector<ExpElement<_num>*>& pntList)
{
   std::vector<ExpElement<_num>*> elements;
   std::vector<ExpElement<_num>*> temp;
   const _size len = infList.size();
   _int brackets = 0;
   _boo anyUnary = false;

   for (_size i = 0; i < len; i++) {
      ExpElement<_num>* e = infList[i];
      if (e->type == ElementType::et_Operator) {
         const _char op = e->_operator;
         switch (op) {
            case L'(': {
               brackets++;
               if (brackets > 1) {
                  temp.push_back(e);
               }
               break;
            }
            case L')': {
               brackets--;
               if (brackets == 0) {
                  Generator<_num>* result = numExpTree(temp, pntList);
                  ExpElement<_num>* ee = new ExpElement<_num>(result);
                  pntList.push_back(ee);
                  temp.resize(0);
                  elements.push_back(ee);
               }
               else {
                  temp.push_back(e);
               }
               break;
            }
            default: {
               if (brackets == 0) {
                  elements.push_back(e);
               }
               else {
                  temp.push_back(e);
               }
               if (!anyUnary && op == UNARY_MINUS) {
                  anyUnary = true;
               }
               break;
            }
         }
      }
      else
      {
         if (brackets == 0) {
            elements.push_back(e);
         }
         else {
            temp.push_back(e);
         }
      }
   }

   return anyUnary
      ? numExpIntegrateUnary(elements, pntList)
      : numExpTreeMerge(elements, pntList);
}

static Generator<_num>* numExpIntegrateUnary(
   const std::vector<ExpElement<_num>*>& elements,
   std::vector<ExpElement<_num>*>& pntList)
{
   std::vector<ExpElement<_num>*> newList;
   _boo minus = false;
   const _size len = elements.size();

   for (_size i = 0; i < len; i++) {
      ExpElement<_num>* e = elements[i];
      if (e->type == ElementType::et_Operator) {
         if (e->_operator == UNARY_MINUS) {
            minus = true;
         }
         else {
            newList.push_back(e);
         }
      }
      else {
         if (minus) {
            ExpElement<_num>* newElement;

            if (e->type == ElementType::et_Constant) {
               const _num value = -(e->constant);
               // delete e;
               newElement = new ExpElement<_num>(value);
            }
            else {
               Generator<_num>* n = e->takeValue();
               Generator<_num>* neg = new Negation(n);
               newElement = new ExpElement<_num>(neg);
            }

            pntList.push_back(newElement);
            newList.push_back(newElement);
            minus = false;
         }
         else {
            newList.push_back(e);
         }
      }
   }

   return numExpTreeMerge(newList, pntList);
}

static Generator<_num>* numExpTreeMerge(
   const std::vector<ExpElement<_num>*>& elements,
   std::vector<ExpElement<_num>*>& pntList)
{
   const _size len = elements.size();
   if (len == 1) {
      return elements[0]->takeValue();
   }

   std::vector<ExpElement<_num>*> nextElements;
   ExpElement<_num>* firstElement = elements[0];
   ExpElement<_num>* oper;

   for (_size i = 1; i < len; i++) {
      ExpElement<_num>* secondElement = elements[i];
      const ElementType& type = secondElement->type;

      if (type == ElementType::et_Operator) {
         oper = secondElement;
      }
      else {
         const _char& op = oper->_operator;
         if (isNumExpHighPriority(op)) {
            ExpElement<_num>* newElement;

            if (type == ElementType::et_Constant
               && firstElement->type == ElementType::et_Constant)
            {
               const _num v1 = firstElement->constant;
               const _num v2 = secondElement->constant;
               _num value;

               switch(op) {
                  case L'*': {
                     value = v1 * v2;
                     break;
                  }
                  case L'/': {
                     value = v1 / v2;
                     break;
                  }
                  case L'%': {
                     value = v1 % v2;
                     break;
                  }
               }

               //delete firstElement;
               //delete secondElement;
               newElement = new ExpElement<_num>(value);
            }
            else {
               Generator<_num>* first = firstElement->takeValue();
               Generator<_num>* second = secondElement->takeValue();
               BinaryOperation<_num>* bin;

               switch(op) {
                  case L'*': {
                     bin = new Multiplication(first, second);
                     break;
                  }
                  case L'/': {
                     bin = new Division(first, second);
                     break;
                  }
                  case L'%': {
                     bin = new Modulo(first, second);
                     break;
                  }
               }

               newElement = new ExpElement<_num>(bin);
            }

            pntList.push_back(newElement);
            firstElement = newElement;
         }
         else {
            nextElements.push_back(firstElement);
            nextElements.push_back(oper);
            firstElement = secondElement;
         }
      }
   }

   nextElements.push_back(firstElement);

   if (nextElements.size() == 1) {
      return nextElements[0]->takeValue();
   }
   else {
      return numExpTreeMerge2(nextElements);
   }
}

static Generator<_num>* numExpTreeMerge2(
   const std::vector<ExpElement<_num>*>& elements)
{
   Generator<_num>* first = elements[0]->takeValue();
   _boo firstIsConstant = elements[0]->type == ElementType::et_Constant;
   _char op;
   const _size len = elements.size();

   for (_size i = 1; i < len; i++) {
      ExpElement<_num>* e = elements[i];
      if (e->type == ElementType::et_Operator) {
         op = e->_operator;
      }
      else {
         Generator<_num>* second = e->takeValue();

         if (firstIsConstant && e->type == ElementType::et_Constant) {
            _num value;

            switch(op) {
               case L'+': {
                  value = first->getValue() + second->getValue();
                  break;
               }
               case L'-': {
                  value = first->getValue() - second->getValue();
                  break;
               }
            }

            //delete first;
            //delete second;
            first = new Constant<_num>(value);
         }
         else {
            switch(op) {
               case L'+': {
                  Addition* add = new Addition(first, second);
                  first = add;
                  break;
               }
               case L'-': {
                  Subtraction* sub = new Subtraction(first, second);
                  first = sub;
                  break;
               }
            }

            firstIsConstant = false;
         }
      }
   }

   return first;
}

static _boo isNumExpComputable(const std::vector<ExpElement<_num>*>& infList)
{
   const _size len = infList.size();
   if (len == 0) {
      return false;
   }

   // numeric expressions can start with only two symbols: - or (
   ExpElement<_num>* prev = infList[0];
   if (prev->type == ElementType::et_Operator) {
      const _char op = prev->_operator;
      if (!(op == L'(' || op == UNARY_MINUS)) {
         return false;
      }
   }

   // numeric expressions can end with only one symbol: )
   ExpElement<_num>* last = infList[len - 1];
   if (last->type == ElementType::et_Operator) {
      if (last->_operator != L')') {
         return false;
      }
   }

   for (_size i = 1; i < len; i++) {
      ExpElement<_num>* curr = infList[i];
      const _boo cop = curr->type == ElementType::et_Operator;

      if (prev->type == ElementType::et_Operator) {
         switch (prev->_operator) {
            case UNARY_MINUS: {
               if (cop && curr->_operator != L'(') {
                  return false;
               }
               break;
            }
            case L'(': {
               if (cop) {
                  const _char op = curr->_operator;
                  if (!(op == L'(' || op == UNARY_MINUS)) {
                     return false;
                  }
               }
               break;
            }
            case L')': {
               if (cop) {
                  const _char op = curr->_operator;
                  if (op == UNARY_MINUS || op == L'(') {
                     return false;
                  }
               }
               else {
                  return false;
               }
               break;
            }
            default: {
               if (cop) {
                  const _char op = curr->_operator;
                  if (!(op == L'(' || op == UNARY_MINUS)) {
                     return false;
                  }
               }
               break;
            }
         }
      }
      else {
         if (cop) {
            const _char op = curr->_operator;
            if (op == UNARY_MINUS || op == L'(') {
               return false;
            }
         }
         else {
            return false;
         }
      }
      prev = curr;
   }

   return true;
}

static _boo isNumExpOperator(const _char& ch)
{
   switch (ch) {
      case L'+':
      case L'-':
      case L'*':
      case L'/':
      case L'%':
         return true;
      default:
         return false;
   }
}

static _boo isNumExpHighPriority(const _char& ch)
{
   switch (ch)  {
      case L'*':
      case L'/':
      case L'%':
         return true;
      default:
         return false;
   }
}

void timeVariableMemberException(const Token& tk)
{
   throw SyntaxException(str(L"'", tk.originString2, 
      L"' is not a time variable member"), tk.line);
}
