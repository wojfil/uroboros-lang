/*
    This file is part of Perun2.
    Perun2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Peruns2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Perun2. If not, see <http://www.gnu.org/licenses/>.
*/

#include "parse-number.h"
#include "parse-generic.h"
#include "parse-var.h"
#include "../parse/parse-function.h"
#include "../generator/gen-number.h"
#include "../generator/gen-generic.h"
#include "../parse-gen.h"
#include "../../brackets.h"
#include "../../hash.h"


namespace perun2::parse
{

_constexpr _char CHAR_UNARY_MINUS = CHAR_TILDE;
// within a numerical expression, the minus sign means either subtraction operation (x-y) or unary negation (-x)
// the sign above is used internally by the interpreter to distinguish them


_bool parseNumber(_genptr<_num>& result, const Tokens& tks, _p2& p2)
{
   const _size len = tks.getLength();

   if (len == 1) {
      return parseOneToken(p2, tks, result);
   }

   if (tks.check(TI_HAS_FILTER_KEYWORD)) {
      return false;
   }

   if (tks.check(TI_IS_POSSIBLE_FUNCTION)) {
      return func::numberFunction(result, tks, p2);
   }
   else if (len >= 2 && !tks.check(TI_HAS_CHAR_COMMA)) {
      // build numeric expression (but only if the sequence has any operator)
      BracketsInfo bi;
      const _int end = tks.getEnd();
      const _int start = tks.getStart();
      _bool anyOperator = false;

      if (tks.first().isSymbol(CHAR_ASTERISK)) {
         return false;
      }

      for (_int i = start; i <= end; i++) {
         const Token& t = tks.listAt(i);
         if (t.type == Token::t_Symbol && isNumExpOperator(t.value.ch) && bi.isBracketFree()
             && !(i == start && t.value.ch == CHAR_MINUS))
         {
            if (parseNumExp(result, tks, p2)) {
               return true;
            }
            else if (!tks.check(TI_HAS_COMPARISON_CHAR)) {
               const std::vector<Tokens> elements = tks.splitBySymbol(CHAR_PLUS);

               if (elements.size() == 1) {
                  break;
               }

               for (const Tokens& tkse : elements) {
                  _genptr<_str> str;

                  if (!parse(p2, tkse, str)) {
                     throw SyntaxError::invalidExpression(tkse.first().line);
                  }
               }
            }
            anyOperator = true;
            break;
         }
         bi.refresh(t);
      }

      if (!anyOperator && tks.first().isSymbol(CHAR_MINUS)) {
         Tokens tks2(tks);
         tks2.trimLeft();
         _genptr<_num> num;

         if (parse(p2, tks2, num)) {
            result = std::make_unique<gen::Negation>(num);
            return true;
         }
         else {
            _genptr<_per> per;
            if (!parse(p2, tks2, per)) {
               throw SyntaxError(L"sign '-' is not followed by a valid number nor a valid period", tks.first().line);
            }
         }
      }
   }

   if (parseCollectionElement<_num>(result, tks, p2)) {
      return true;
   }

   if (tks.check(TI_IS_LIST_ELEM_MEMBER)) {
      const Tokens tksm(tks, tks.getStart(), tks.getLength() - 1);
      _genptr<_num> num;
      parseListElementIndex(num, tksm, p2);
      const Token& f = tks.first();
      _genptr<_tlist> tlist;
      if (makeVarRef(f, tlist, p2)) {
         const Token& last = tks.last();
         const _hash h = last.value.twoWords.h2;
         const Hashes& hs = p2.hashes;
         _genptr<_tim> tim = std::make_unique<gen::ListElement<_tim>>(tlist, num);

         if (h == hs.HASH_YEAR || h == hs.HASH_YEARS)
            result = std::make_unique<gen::TimeMember>(tim, Period::u_Years);
         else if (h == hs.HASH_MONTH || h == hs.HASH_MONTHS)
            result = std::make_unique<gen::TimeMember>(tim, Period::u_Months);
         else if (h == hs.HASH_WEEKDAY)
            result = std::make_unique<gen::TimeMember>(tim, Period::u_Weeks);
         else if (h == hs.HASH_DAY || h == hs.HASH_DAYS)
            result = std::make_unique<gen::TimeMember>(tim, Period::u_Days);
         else if (h == hs.HASH_HOUR || h == hs.HASH_HOURS)
            result = std::make_unique<gen::TimeMember>(tim, Period::u_Hours);
         else if (h == hs.HASH_MINUTE || h == hs.HASH_MINUTES)
            result = std::make_unique<gen::TimeMember>(tim, Period::u_Minutes);
         else if (h == hs.HASH_SECOND || h == hs.HASH_SECONDS)
            result = std::make_unique<gen::TimeMember>(tim, Period::u_Seconds);
         else if (h == hs.HASH_DATE)
            return false;
         else
            timeVariableMemberException(last, p2);

         return true;
      }
   }

   if (parseBinary<_num>(result, tks, p2) || parseTernary<_num>(result, tks, p2)) {
      return true;
   }

   return false;
}


// build numeric expression
// multiple numbers connected with signs +-*/% and brackets ()
static _bool parseNumExp(_genptr<_num>& result, const Tokens& tks, _p2& p2)
{
   std::vector<ExpElement<_num>> infList; // infix notation list
   const _int start = tks.getStart();
   const _int end = tks.getEnd();
   _int sublen = 0;
   BracketsInfo bi;
   _bool prev = false;

   for (_int i = start; i <= end; i++) {
      const Token& t = tks.listAt(i);
      if (t.type == Token::t_Symbol) {
         const _char ch = t.value.ch;

         if (isNumExpOperator(ch)) {
            if (sublen == 0) {
               // check if character '-' represents binary subtraction
               // or unary negation
               const _char ch2 = (ch == CHAR_MINUS && (prev || i == start)) ? CHAR_UNARY_MINUS : ch;
               infList.emplace_back(ch2, t.line);
            }
            else {
               if (bi.isBracketFree()) {
                  const Tokens tks2(tks, i - sublen, sublen);
                  const _int line = tks2.first().line;

                  if (tks2.getLength() == 1
                     && tks2.first().type == Token::t_Number) {

                     const _num num = tks2.first().value.num.n;
                     infList.emplace_back(num, line);
                     infList.emplace_back(ch, line);
                     sublen = 0;
                  }
                  else {
                     _genptr<_num> num;
                     if (parse(p2, tks2, num)) {
                        infList.emplace_back(num, line);
                        infList.emplace_back(ch, line);
                        sublen = 0;
                     }
                     else {
                        return false;
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

         if (ch == CHAR_OPENING_ROUND_BRACKET) {
            if (bi.isBracketFree()) {
               if (sublen == 1) {
                  const Token& pt = tks.listAt(i - 1);
                  if (pt.type != Token::t_Word) {
                     throw SyntaxError::invalidFunctionName(t.line);
                  }
               }
            }
         }

         bi.refresh(t);
      }

      prev = false;
      sublen++;
   }

   if (sublen != 0) {
      Tokens tks2(tks, 1 + end - sublen, sublen);

      if (tks2.getLength() == 1 && tks2.first().type == Token::t_Number) {
         infList.emplace_back(tks2.first().value.num.n, tks2.first().line);
      }
      else {
         _genptr<_num> num;
         if (parse(p2, tks2, num)) {
            infList.emplace_back(num, tks2.first().line);
         }
         else {
            return false;
         }
      }
   }

   if (!isNumExpComputable(infList)) {
      throw SyntaxError::invalidNumericalExpression(tks.first().line);
   }

   return numExpTree(result, infList);
}

static _bool numExpTree(_genptr<_num>& result, std::vector<ExpElement<_num>>& infList)
{
   std::vector<ExpElement<_num>> elements;
   std::vector<ExpElement<_num>> temp;
   const _size len = infList.size();
   _int brackets = 0;
   _bool anyUnary = false;

   for (_size i = 0; i < len; i++) {
      ExpElement<_num>& e = infList[i];
      if (e.type == ElementType::et_Operator) {
         const _char op = e.operator_;
         switch (op) {
            case CHAR_OPENING_ROUND_BRACKET: {
               brackets++;
               if (brackets > 1) {
                  temp.emplace_back(e);
               }
               break;
            }
            case CHAR_CLOSING_ROUND_BRACKET: {
               brackets--;
               if (brackets == 0) {
                  _genptr<_num> res;

                  if (!numExpTree(res, temp)) {
                     return false;
                  }

                  temp.clear();
                  elements.emplace_back(res, e.line);
               }
               else {
                  temp.emplace_back(e);
               }
               break;
            }
            default: {
               if (brackets == 0) {
                  elements.emplace_back(e);
               }
               else {
                  temp.emplace_back(e);
               }
               if (!anyUnary && op == CHAR_UNARY_MINUS) {
                  anyUnary = true;
               }
               break;
            }
         }
      }
      else
      {
         if (brackets == 0) {
            elements.emplace_back(e);
         }
         else {
            temp.emplace_back(e);
         }
      }
   }

   return anyUnary
      ? numExpIntegrateUnary(result, elements)
      : numExpTreeMerge(result, elements);
}

static _bool numExpIntegrateUnary(_genptr<_num>& result, std::vector<ExpElement<_num>>& elements)
{
   std::vector<ExpElement<_num>> newList;
   _bool minus = false;
   const _size len = elements.size();

   for (_size i = 0; i < len; i++) {
      ExpElement<_num>& e = elements[i];
      if (e.type == ElementType::et_Operator) {
         if (e.operator_ == CHAR_UNARY_MINUS) {
            minus = true;
         }
         else {
            newList.emplace_back(e);
         }
      }
      else {
         if (minus) {
            if (e.type == ElementType::et_Constant) {
               const _num value = -(e.constant);
               newList.emplace_back(value, e.line);
            }
            else {
               _genptr<_num> n = std::move(e.generator);
               _genptr<_num> neg = std::make_unique<gen::Negation>(n);
               newList.emplace_back(neg, e.line);
            }

            minus = false;
         }
         else {
            newList.emplace_back(e);
         }
      }
   }

   return numExpTreeMerge(result, newList);
}

static _bool numExpTreeMerge(_genptr<_num>& result, std::vector<ExpElement<_num>>& elements)
{
   const _size len = elements.size();
   if (len == 1) {
      result = std::move(elements[0].generator);
      return true;
   }

   std::vector<ExpElement<_num>> nextElements;
   ExpElement<_num> firstElement(elements[0]);
   _char oper;
   _int operLine;

   for (_size i = 1; i < len; i++) {
      ExpElement<_num>& secondElement = elements[i];
      const ElementType& type = secondElement.type;

      if (type == ElementType::et_Operator) {
         oper = secondElement.operator_;
         operLine = secondElement.line;
      }
      else {
         if (isNumExpHighPriority(oper)) {
            if (type == ElementType::et_Constant && firstElement.type == ElementType::et_Constant)
            {
               const _num& v1 = firstElement.constant;
               const _num& v2 = secondElement.constant;
               _num value;

               switch(oper) {
                  case CHAR_ASTERISK: {
                     value = v1 * v2;
                     break;
                  }
                  case CHAR_SLASH: {
                     if (v2.isZero()) {
                        throw SyntaxError::inevitableDivisionByZero(secondElement.line);
                     }
                     value = v1 / v2;
                     break;
                  }
                  case CHAR_PERCENT: {
                     if (v2.isZero()) {
                        throw SyntaxError::inevitableModuloByZero(secondElement.line);
                     }
                     value = v1 % v2;
                     break;
                  }
               }

               firstElement.reinit(value, firstElement.line);
            }
            else {
               _genptr<_num> first = std::move(firstElement.generator);
               _genptr<_num> second = std::move(secondElement.generator);
               _genptr<_num> bin;

               switch(oper) {
                  case CHAR_ASTERISK: {
                     bin = std::make_unique<gen::Multiplication>(first, second);
                     break;
                  }
                  case CHAR_SLASH: {
                     bin = std::make_unique<gen::Division>(first, second);
                     break;
                  }
                  case CHAR_PERCENT: {
                     bin = std::make_unique<gen::Modulo>(first, second);
                     break;
                  }
               }

               firstElement.reinit(bin, firstElement.line);
            }
         }
         else {
            nextElements.emplace_back(firstElement);
            nextElements.emplace_back(oper, operLine);
            firstElement.reinit(secondElement);
         }
      }
   }

   nextElements.emplace_back(firstElement);

   if (nextElements.size() == 1) {
      result = std::move(nextElements[0].generator);
      return true;
   }
   else {
      return numExpTreeMerge2(result, nextElements);
   }
}

static _bool numExpTreeMerge2(_genptr<_num>& result, std::vector<ExpElement<_num>>& elements)
{
   _genptr<_num> first(std::move(elements[0].generator));
   _bool firstIsConstant = first->isConstant();
   _char op;
   const _size len = elements.size();

   for (_size i = 1; i < len; i++) {
      ExpElement<_num>& e = elements[i];
      if (e.type == ElementType::et_Operator) {
         op = e.operator_;
      }
      else {
         _genptr<_num> second(std::move(e.generator));

         if (firstIsConstant && e.type == ElementType::et_Constant) {
            _num value;

            switch(op) {
               case CHAR_PLUS: {
                  value = first->getValue() + second->getValue();
                  break;
               }
               case CHAR_MINUS: {
                  value = first->getValue() - second->getValue();
                  break;
               }
            }

            first = std::make_unique<gen::Constant<_num>>(value);
         }
         else {
            _genptr<_num> prev = std::move(first);

            switch(op) {
               case CHAR_PLUS: {
                  first = std::make_unique<gen::Addition>(prev, second);
                  break;
               }
               case CHAR_MINUS: {
                  first = std::make_unique<gen::Subtraction>(prev, second);
                  break;
               }
            }

            firstIsConstant = false;
         }
      }
   }

   result = std::move(first);
   return true;
}

static _bool isNumExpComputable(const std::vector<ExpElement<_num>>& infList)
{
   const _size len = infList.size();
   if (len == 0) {
      return false;
   }

   // numeric expressions can start with only two symbols: - or (
   const ExpElement<_num>& first = infList[0];
   if (first.type == ElementType::et_Operator) {
      if (!(first.operator_ == CHAR_OPENING_ROUND_BRACKET || first.operator_ == CHAR_UNARY_MINUS)) {
         return false;
      }
   }

   // numeric expressions can end with only one symbol: )
   const ExpElement<_num>& last = infList[len - 1];
   if (last.type == ElementType::et_Operator) {
      if (last.operator_ != CHAR_CLOSING_ROUND_BRACKET) {
         return false;
      }
   }

   for (_size i = 1; i < len; i++) {
      const ExpElement<_num>& prev = infList[i - 1];
      const ExpElement<_num>& curr = infList[i];
      const _bool cop = curr.type == ElementType::et_Operator;

      if (prev.type == ElementType::et_Operator) {
         switch (prev.operator_) {
            case CHAR_UNARY_MINUS: {
               if (cop && curr.operator_ != CHAR_OPENING_ROUND_BRACKET) {
                  return false;
               }
               break;
            }
            case CHAR_OPENING_ROUND_BRACKET: {
               if (cop) {
                  const _char op = curr.operator_;
                  if (!(op == CHAR_OPENING_ROUND_BRACKET || op == CHAR_UNARY_MINUS)) {
                     return false;
                  }
               }
               break;
            }
            case CHAR_CLOSING_ROUND_BRACKET: {
               if (cop) {
                  const _char op = curr.operator_;
                  if (op == CHAR_UNARY_MINUS || op == CHAR_OPENING_ROUND_BRACKET) {
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
                  const _char op = curr.operator_;
                  if (!(op == CHAR_OPENING_ROUND_BRACKET || op == CHAR_UNARY_MINUS)) {
                     return false;
                  }
               }
               break;
            }
         }
      }
      else {
         if (cop) {
            const _char op = curr.operator_;
            if (op == CHAR_UNARY_MINUS || op == CHAR_OPENING_ROUND_BRACKET) {
               return false;
            }
         }
         else {
            return false;
         }
      }
   }

   return true;
}

static _bool isNumExpOperator(const _char ch)
{
   switch (ch) {
      case CHAR_PLUS:
      case CHAR_MINUS:
      case CHAR_ASTERISK:
      case CHAR_SLASH:
      case CHAR_PERCENT:
         return true;
      default:
         return false;
   }
}

static _bool isNumExpHighPriority(const _char ch)
{
   switch (ch)  {
      case CHAR_ASTERISK:
      case CHAR_SLASH:
      case CHAR_PERCENT:
         return true;
      default:
         return false;
   }
}

void timeVariableMemberException(const Token& tk, _p2& p2)
{
   throw SyntaxError(str(L"'", tk.getOriginString_2(p2),
      L"' is not a time variable member"), tk.line);
}

}
