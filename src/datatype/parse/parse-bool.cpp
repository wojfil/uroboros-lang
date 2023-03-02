/*
    This file is part of Uroboros2.
    Uroboros2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Uroboros2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Uroboros2. If not, see <http://www.gnu.org/licenses/>.
*/

#include "parse-bool.h"
#include "parse-list.h"
#include "parse-generic.h"
#include "../parse/parse-function.h"
#include "../generator/gen-bool.h"
#include "../../brackets.h"
#include "../../lexer.h"
#include "../generator/gen-like.h"
#include "../parse-gen.h"


namespace uro::parse
{

_bool parseBool(_genptr<_bool>& result, const Tokens& tks, _uro& uro)
{
   const _size len = tks.getLength();

   if (len == 1) {
      return parseOneToken(uro, tks, result);
   }

   if (tks.check(TI_HAS_FILTER_KEYWORD)) {
      return false;
   }

   const _bool possibleBinary = tks.check(TI_HAS_CHAR_QUESTION_MARK);

   if (tks.check(TI_IS_POSSIBLE_FUNCTION)) {
      return func::boolFunction(result, tks, uro);
   }
   else if (len >= 2 && !possibleBinary) {
      // build numeric expression (but only if sequence has any operator)
      BracketsInfo bi;
      const _int end = tks.getEnd();
      const _int start = tks.getStart();

      for (_int i = start; i <= end; i++) {
         const Token& t = tks.listAt(i);
         if (t.type == Token::t_Keyword && isBoolExpOperator(t) && bi.isBracketFree())
         {
            if (!(t.isKeyword(Keyword::kw_Not) && i != end && tks.listAt(i + 1).isNegatableKeywordOperator()))
            {
               if (!parseBoolExp(result, tks, uro)) {
                  throw SyntaxError(L"syntax of a boolean expression is not valid", tks.first().line);
               }
               return true;
            }
         }
         bi.refresh(t);
      }
   }

   if (parseComparisons(result, tks, uro)) {
      return true;
   }

   if (tks.check(TI_HAS_KEYWORD_IN)) {
      if (parseIn(result, tks, uro)) {
         return true;
      }
   }

   if (tks.check(TI_HAS_KEYWORD_LIKE)) {
      if (parseLike(result, tks, uro)) {
         return true;
      }
   }

   if (parseTernary<_bool>(result, tks, uro)) {
      return true;
   }

   return false;
}


// build boolean expression
// multiple logic statements
// connected with keywords not, and, or, xor and brackets ()
static _bool parseBoolExp(_genptr<_bool>& result, const Tokens& tks, _uro& uro)
{
   std::vector<ExpElement<_bool>> infList; // infix notation list
   const _int start = tks.getStart();
   const _int end = tks.getEnd();
   _int sublen = 0, lv1 = 0, lv2 = 0;

   for (_int i = start; i <= end; i++) {
      const Token& t = tks.listAt(i);
      if (t.type == Token::t_Keyword) {
         const _bool free = (lv1 == 0) && (lv2 == 0);
         if (isBoolExpOperator(t)) {
            if (t.isKeyword(Keyword::kw_Not) && i != end && tks.listAt(i + 1).isNegatableKeywordOperator())
            {
               sublen++;
            }
            else {
               const _char ch = toBoolExpOperator(t);
               if (sublen == 0) {
                  infList.emplace_back(ch, t.line);
               }
               else {
                  if (free) {
                     const Tokens tks2(tks, i - sublen, sublen);
                     const _int line = tks2.first().line;

                     if (tks2.getLength() == 1 && tks2.first().isLogicConstant()) {
                        const _bool boo = tks2.first().value.keyword.k == Keyword::kw_True;
                        infList.emplace_back(boo, line);
                        infList.emplace_back(ch, line);
                        sublen = 0;
                     }
                     else {
                        _genptr<_bool> boo;
                        if (parse(uro, tks2, boo)) {
                           infList.emplace_back(boo, line);
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
            }
         }
         else {
            sublen++;
         }
      }
      else if (t.type == Token::t_Symbol) {
         sublen++;

         switch (t.value.ch) {
            case CHAR_OPENING_ROUND_BRACKET: {
               lv1++;
               break;
            }
            case CHAR_CLOSING_ROUND_BRACKET: {
               lv1--;
               break;
            }
            case CHAR_OPENING_SQUARE_BRACKET: {
               lv2++;
               break;
            }
            case CHAR_CLOSING_SQUARE_BRACKET: {
               lv2--;
               break;
            }
            default: {
               break;
            }
         }
      }
      else {
         sublen++;
      }
   }

   if (sublen != 0) {
      const Tokens tks2(tks, 1 + end - sublen, sublen);

      if (tks2.getLength() == 1 && tks2.first().isLogicConstant()) {
         const _bool boo = (tks2.first().value.keyword.k == Keyword::kw_True);
         infList.emplace_back(boo, tks2.first().line);
      }
      else {
         _genptr<_bool> boo;
         if (parse(uro, tks2, boo)) {
            infList.emplace_back(boo, tks2.first().line);
         }
         else {
            return false;
         }
      }
   }

   if (!isBoolExpComputable(infList)) {
      throw SyntaxError(L"syntax of a boolean expression is not valid",
         tks.first().line);
   }

   return boolExpTree(result, infList);
}

static _bool boolExpTree(_genptr<_bool>& result, std::vector<ExpElement<_bool>>& infList)
{
   std::vector<ExpElement<_bool>> elements;
   std::vector<ExpElement<_bool>> temp;
   const _size len = infList.size();
   _int brackets = 0;
   _bool anyNot = false;

   for (_size i = 0; i < len; i++) {
      ExpElement<_bool>& e = infList[i];
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
                  _genptr<_bool> res;

                  if (!boolExpTree(res, temp)) {
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
               if (!anyNot && op == CHAR_EXCLAMATION_MARK) {
                  anyNot = true;
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

   return anyNot
      ? boolExpIntegrateNegations(result, elements)
      : boolExpTreeMerge(result, elements);
}

static _bool boolExpIntegrateNegations(_genptr<_bool>& result,
   std::vector<ExpElement<_bool>>& elements)
{
   std::vector<ExpElement<_bool>> newList;
   _bool negate = false;
   const _size len = elements.size();

   for (_size i = 0; i < len; i++) {
      ExpElement<_bool>& e = elements[i];
      if (e.type == ElementType::et_Operator) {
         if (e.operator_ == CHAR_EXCLAMATION_MARK) {
            negate = true;
         }
         else {
            newList.emplace_back(e);
         }
      }
      else {
         if (negate) {
            if (e.type == ElementType::et_Constant) {
               const _bool value = !(e.constant);
               newList.emplace_back(value, e.line);
            }
            else {
               _genptr<_bool> n = std::move(e.generator);
               _genptr<_bool> no = std::make_unique<gen::Not>(n);
               newList.emplace_back(no, e.line);
            }

            negate = false;
         }
         else {
            newList.emplace_back(e);
         }
      }
   }

   return boolExpTreeMerge(result, newList);
}

static _bool boolExpTreeMerge(_genptr<_bool>& result,
   std::vector<ExpElement<_bool>>& elements)
{
   const _size len = elements.size();
   if (len == 1) {
      result = std::move(elements[0].generator);
      return true;
   }

   _genptr<_bool> first = std::move(elements[0].generator);
   _bool firstIsConstant = first->isConstant();
   _char op;

   for (_size i = 1; i < len; i++) {
      ExpElement<_bool>& e = elements[i];

      if (e.type == ElementType::et_Operator) {
         op = e.operator_;
      }
      else {
         _genptr<_bool> second(std::move(e.generator));

         if (firstIsConstant && e.type == ElementType::et_Constant) {
            _bool value;

            switch(op) {
               case CHAR_AMPERSAND: {
                  value = first->getValue() && second->getValue();
                  break;
               }
               case CHAR_VERTICAL_BAR: {
                  value = first->getValue() || second->getValue();;
                  break;
               }
               case CHAR_CARET: {
                  value = first->getValue() ^ second->getValue();;
                  break;
               }
            }

            first = std::make_unique<gen::Constant<_bool>>(value);
         }
         else {
            _genptr<_bool> prev = std::move(first);

            switch(op) {
               case CHAR_AMPERSAND: {
                  first = std::make_unique<gen::And>(prev, second);
                  break;
               }
               case CHAR_VERTICAL_BAR: {
                  first = std::make_unique<gen::Or>(prev, second);
                  break;
               }
               case CHAR_CARET: {
                  first = std::make_unique<gen::Xor>(prev, second);
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

static _bool isBoolExpComputable(const std::vector<ExpElement<_bool>>& infList)
{
   const _size len = infList.size();
   if (len == 0) {
      return false;
   }

   // bool expressions can start with only two symbols: not or (
   const ExpElement<_bool>& first = infList[0];
   if (first.type == ElementType::et_Operator) {
      const _char& op = first.operator_;
      if (!(op == CHAR_OPENING_ROUND_BRACKET || op == CHAR_EXCLAMATION_MARK)) {
         return false;
      }
   }

   // bool expressions can end with only one symbol: )
   const ExpElement<_bool>& last = infList[len - 1];
   if (last.type == ElementType::et_Operator) {
      if (last.operator_ != CHAR_CLOSING_ROUND_BRACKET) {
         return false;
      }
   }

   for (_size i = 1; i < len; i++) {
      const ExpElement<_bool>& prev = infList[i - 1];
      const ExpElement<_bool>& curr = infList[i];
      const _bool cop = curr.type == ElementType::et_Operator;

      if (prev.type == ElementType::et_Operator) {
         switch (prev.operator_) {
            case CHAR_EXCLAMATION_MARK: {
               if (cop && curr.operator_ != CHAR_OPENING_ROUND_BRACKET) {
                  return false;
               }
               break;
            }
            case CHAR_OPENING_ROUND_BRACKET: {
               if (cop) {
                  const _char& op = curr.operator_;
                  if (!(op == CHAR_OPENING_ROUND_BRACKET || op == CHAR_EXCLAMATION_MARK)) {
                     return false;
                  }
               }
               break;
            }
            case CHAR_CLOSING_ROUND_BRACKET: {
               if (cop) {
                  const _char& op = curr.operator_;
                  if (op == CHAR_OPENING_ROUND_BRACKET || op == CHAR_EXCLAMATION_MARK) {
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
                  const _char& op = curr.operator_;
                  if (!(op == CHAR_OPENING_ROUND_BRACKET || op == CHAR_EXCLAMATION_MARK)) {
                     return false;
                  }
               }
               break;
            }
         }
      }
      else
      {
         if (cop) {
            const _char& op = curr.operator_;
            if (op == CHAR_EXCLAMATION_MARK || op == CHAR_OPENING_ROUND_BRACKET) {
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

_bool isBoolExpOperator(const Token& tk)
{
   switch (tk.value.keyword.k) {
      case Keyword::kw_And:
      case Keyword::kw_Or:
      case Keyword::kw_Xor:
      case Keyword::kw_Not:
         return true;
      default:
         return false;
   }
}

static _char toBoolExpOperator(const Token& tk)
{
   switch (tk.value.keyword.k) {
      case Keyword::kw_And:
         return CHAR_AMPERSAND;
      case Keyword::kw_Or:
         return CHAR_VERTICAL_BAR;
      case Keyword::kw_Xor:
         return CHAR_CARET;
      case Keyword::kw_Not:
         return CHAR_EXCLAMATION_MARK;
      default:
         return CHAR_SPACE;
   }
}

template <typename T>
static _bool parseIn_Unit(_genptr<_bool>& result, const _bool negated,
   const std::pair<Tokens, Tokens>& pair, _uro& uro)
{
   _genptr<T> valLeft;
   if (!parse(uro, pair.first, valLeft)) {
      return false;
   }

   // check if the right side is a single value
   // in this special case, the whole structure is reduced to
   // a simple comparison "left == right" or "left != right"
   _genptr<T> num2;
   if (parse(uro, pair.second, num2)) {
      if (negated) {
         result = std::make_unique<gen::NotEquals<T>>(valLeft, num2);
      }
      else {
         result = std::make_unique<gen::Equals<T>>(valLeft, num2);
      }
      return true;
   }

   _genptr<std::vector<T>> valRight;

   if (parse(uro, pair.second, valRight)) {
      if (valRight->isConstant()) {
         const std::vector<T> vs = valRight->getValue();
         _genptr<_bool> in = std::make_unique<gen::InConstList<T>>(valLeft, vs);
         result = negated
            ? std::make_unique<gen::Not>(in)
            : std::move(in);
      }
      else {
         _genptr<_bool> in = std::make_unique<gen::InList<T>>(valLeft, valRight);
         result = negated
            ? std::make_unique<gen::Not>(in)
            : std::move(in);
      }

      return true;
   }
   else {
      return false;
   }
}

static _bool parseIn(_genptr<_bool>& result, const Tokens& tks, _uro& uro)
{
   std::pair<Tokens, Tokens> pair = tks.divideByKeyword(Keyword::kw_In);

   if (pair.first.isEmpty()) {
      emptyOperSideException(tks.first(), true, uro);
   }
   if (pair.second.isEmpty()) {
      emptyOperSideException(tks.last(), false, uro);
   }

   _bool neg = pair.first.last().isKeyword(Keyword::kw_Not);
   if (neg) {
      pair.first.trimRight();
      if (pair.first.isEmpty()) {
         emptyOperSideException(tks.first(), true, uro);
      }
   }

   // first: try to build "Number IN NumList"
   _genptr<_bool> list;

   if (parseIn_Unit<_num>(list, neg, pair, uro)) {
      result = std::move(list);
      return true;
   }

   const Token& lf = pair.first.first();

   if (pair.first.getLength() == 1 && lf.type == Token::t_Word &&
      uro.hashes.HASH_GROUP_TIME_ATTR.find(lf.value.word.h) != uro.hashes.HASH_GROUP_TIME_ATTR.end())
   {
      if (pair.second.check(TI_HAS_CHAR_COMMA)) {
         const std::vector<Tokens> elements = pair.second.splitBySymbol(CHAR_COMMA);
         const _size elen = elements.size();
         for (_size i = 0; i < elen; i++) {
            const Tokens& t = elements[i];
            if (t.getLength() == 1) {
               const Token& tf = t.first();
               if (tf.isWeekDay()) {
                  timeInNumberException(pair.first.first(), tf, L"weekDay", neg, pair.first, uro);
               }
               else if (tf.isMonth()) {
                  timeInNumberException(pair.first.first(), tf, L"month", neg, pair.first, uro);
               }

               const _bool isInteger = (tf.type == Token::t_Number) && !tf.value.num.n.isDouble;
               if (isInteger) {
                  timeInNumberException(pair.first.first(), tf, L"year", neg, pair.first, uro);
               }
            }
         }
      }
      else if (pair.second.getLength() == 1) {
         const Token& rf = pair.second.first();
         const _bool isWeek = rf.isWeekDay();
         const _bool isMonth = rf.isMonth();

         if (isWeek || isMonth) {
            timeInNumberException(pair.first.first(), pair.second.first(),
               isWeek ? L"weekDay" : L"month", neg, pair.first, uro);
         }

         const _bool isInteger = (rf.type == Token::t_Number) && !rf.value.num.n.isDouble;
         if (isInteger) {
            timeInNumberException(pair.first.first(), pair.second.first(), L"year", neg, pair.first, uro);
         }
      }
   }

   // secondary: try to build "Time IN TimList"
   _genptr<_bool> list2;
   if (parseInTimList(list2, neg, pair, uro)) {
      result = std::move(list2);
      return true;
   }

   // finally: try to build "string IN list"
   return parseIn_Unit<_str>(result, neg, pair, uro);;
}


static _bool parseInTimList(_genptr<_bool>& result, const bool& negated,
   const std::pair<Tokens, Tokens>& pair, _uro& uro)
{
   _genptr<_tim> tim;
   if (!parse(uro, pair.first, tim)) {
      return false;
   }

   _genptr<_tim> tim2;
   if (parse(uro, pair.second, tim2)) {
      if (negated) {
         result = std::make_unique<gen::NotEquals<_tim>>(tim, tim2);
      }
      else {
         result = std::make_unique<gen::Equals<_tim>>(tim, tim2);
      }

      return true;
   }

   _genptr<_tlist> tlist;
   if (parse(uro, pair.second, tlist)) {
      if (tlist->isConstant()) {
         const _tlist vs = tlist->getValue();
         _genptr<_bool> in = std::make_unique<gen::InConstTimeList>(tim, vs);
         result = negated
            ? std::make_unique<gen::Not>(in)
            : std::move(in);
      }
      else {
         _genptr<_bool> in = std::make_unique<gen::InList<_tim>>(tim, tlist);
         result = negated
            ? std::make_unique<gen::Not>(in)
            : std::move(in);
      }

      return true;
   }
   else {
      return false;
   }
}

static void emptyOperSideException(const Token& oper, const bool& isLeft, _uro& uro)
{
   const _str side = isLeft ? L"left" : L"right";

   throw SyntaxError(str(side, L" side of operator '", oper.getOriginString(uro), L"' is empty"),
      oper.line);
}

static void timeInNumberException(const Token& timeVar, const Token& numVar,
   const _str& timeMember, const _bool negated, const Tokens& tks, _uro& uro)
{
   if (timeMember == L"year") {
      if (negated) {
         throw SyntaxError(str(L"instead of '", timeVar.getOriginString(uro), L" not in ", toStr(numVar.value.num.n.value.i),
            L"', write '", timeVar.getOriginString(uro), L".year != ",
            toStr(numVar.value.num.n.value.i), L"'"), tks.first().line);
      }
      else {
         throw SyntaxError(str(L"instead of '", timeVar.getOriginString(uro), L" in ", toStr(numVar.value.num.n.value.i),
            L"', write '", timeVar.getOriginString(uro), L".year = ",
            toStr(numVar.value.num.n.value.i), L"'"), tks.first().line);
      }
   }
   else {
      if (negated) {
         throw SyntaxError(str(L"instead of '", timeVar.getOriginString(uro), L" not in ", numVar.getOriginString(uro),
            L"', write '", timeVar.getOriginString(uro), L".", timeMember,
            L" != ", numVar.getOriginString(uro), L"'"), tks.first().line);
      }
      else {
         throw SyntaxError(str(L"instead of '", timeVar.getOriginString(uro), L" in ", numVar.getOriginString(uro),
            L"', write '", timeVar.getOriginString(uro), L".", timeMember,
            L" = ", numVar.getOriginString(uro), L"'"), tks.first().line);
      }
   }
}

static _bool parseLike(_genptr<_bool>& result, const Tokens& tks, _uro& uro)
{
   std::pair<Tokens, Tokens> pair = tks.divideByKeyword(Keyword::kw_Like);

   if (pair.first.isEmpty()) {
      emptyOperSideException(tks.first(), true, uro);
   }
   if (pair.second.isEmpty()) {
      emptyOperSideException(tks.last(), false, uro);
   }

   const _bool neg = pair.first.last().isKeyword(Keyword::kw_Not);
   if (neg) {
      pair.first.trimRight();
      if (pair.first.isEmpty()) {
         emptyOperSideException(tks.first(), true, uro);
      }
   }

   _genptr<_str> value;
   if (!parse(uro, pair.first, value)) {
      return false;
   }

   _genptr<_str> pattern;
   if (parse(uro, pair.second, pattern)) {
      if (pattern->isConstant()) {
         const _str cnst = pattern->getValue();

         if (neg) {
            _genptr<_bool> b = std::make_unique<gen::LikeConst>(value, cnst);
            result = std::make_unique<gen::Not>(b);
         }
         else {
            result = std::make_unique<gen::LikeConst>(value, cnst);
         }

         return true;
      }

      if (neg) {
         _genptr<_bool> b = std::make_unique<gen::Like>(value, pattern);
         result = std::make_unique<gen::Not>(b);
      }
      else {
         result = std::make_unique<gen::Like>(value, pattern);
      }

      return true;
   }
   else {
      return false;
   }
}

static _bool parseComparisons(_genptr<_bool>& result, const Tokens& tks, _uro& uro)
{
   BracketsInfo bi;
   const _int end = tks.getEnd();

   for (_int i = tks.getStart(); i <= end; i++) {
      const Token& t = tks.listAt(i);
      if (t.type == Token::t_Symbol && bi.isBracketFree()) {
         const _char& ch = t.value.ch;
         switch (ch) {
            case CHAR_SMALLER:
            case CHAR_GREATER:
            case CHAR_EXCLAMATION_MARK:
            case CHAR_EQUAL_SIGN: {
               return parseComparison(result, tks, ch, uro);
            }
         }
      }
      bi.refresh(t);
   }

   return false;
}

template <typename T>
static _bool comparison(_genptr<_bool>& result, _genptr<T>& val1,
   _genptr<T>& val2, const gen::CompType& ct)
{
   switch (ct) {
      case gen::ct_Equals: {
         result = std::make_unique<gen::Equals<T>>(val1, val2);
         break;
      }
      case gen::ct_NotEquals: {
         result = std::make_unique<gen::NotEquals<T>>(val1, val2);
         break;
      }
      case gen::ct_Smaller: {
         result = std::make_unique<gen::Smaller<T>>(val1, val2);
         break;
      }
      case gen::ct_SmallerEquals: {
         result = std::make_unique<gen::SmallerEquals<T>>(val1, val2);
         break;
      }
      case gen::ct_Bigger: {
         result = std::make_unique<gen::Bigger<T>>(val1, val2);
         break;
      }
      case gen::ct_BiggerEquals: {
         result = std::make_unique<gen::BiggerEquals<T>>(val1, val2);
         break;
      }
      default:
         return false;
   };

   return true;
}


template <typename T>
_bool parseComparisonUnit(_genptr<_bool>& result, const Tokens& left,
   const Tokens& right, const gen::CompType& ct, _uro& uro)
{
   _genptr<T> v1;
   _genptr<T> v2;
   if (parse(uro, left, v1) && parse(uro, right, v2)) {
      return comparison<T>(result, v1, v2, ct);
   }

   return false;
}

static _bool parseComparison(_genptr<_bool>& result, const Tokens& tks, const _char sign, _uro& uro)
{
   gen::CompType ct;
   const std::pair<Tokens, Tokens> pair = prepareComparison(tks, sign, ct);
   const Tokens& left = pair.first;
   const Tokens& right = pair.second;

   // look for some common errors
   // and throw precise messages to the user
   if (left.getLength() == 1 && right.getLength() == 1) {
      const Token& t1 = left.first();
      const Token& t2 = right.first();

      const _bool isWeek1 = t1.isWeekDay();
      const _bool isWeek2 = t2.isWeekDay();
      const _bool isMonth1 = t1.isMonth();
      const _bool isMonth2 = t2.isMonth();
      const _bool isVar1 = t1.type == Token::t_Word &&
         uro.hashes.HASH_GROUP_TIME_ATTR.find(t1.value.word.h) != uro.hashes.HASH_GROUP_TIME_ATTR.end();
      const _bool isVar2 = t2.type == Token::t_Word &&
         uro.hashes.HASH_GROUP_TIME_ATTR.find(t2.value.word.h) != uro.hashes.HASH_GROUP_TIME_ATTR.end();

      //const _str& v1 = t1.originString;
      //const _str& v2 = t2.originString;
      const _str s = _str(1, sign);

      if (isVar1 && (isWeek2 || isMonth2)) {
         throw SyntaxError(str(L"instead of '", t1.getOriginString(uro), L" ", s, L" ", t2.getOriginString(uro),
            L"', write '", t1.getOriginString(uro), L".", (isWeek2 ? L"weekDay" : L"month"),
            L" ", s, L" ", t2.getOriginString(uro), L"'"), tks.first().line);
      }
      else if ((isWeek1 || isMonth1) && isVar2) {
         throw SyntaxError(str(L"instead of '", t1.getOriginString(uro), L" ", s, L" ", t2.getOriginString(uro),
            L"', write '", t1.getOriginString(uro), L" ", s, L" ", t2.getOriginString(uro), L".",
            (isWeek1 ? L"weekDay" : L"month"), L"'"), tks.first().line);
      }

      const _bool isInteger1 = (t1.type == Token::t_Number) && !t1.value.num.n.isDouble;
      const _bool isInteger2 = (t2.type == Token::t_Number) && !t2.value.num.n.isDouble;

      if (isVar1 && isInteger2) {
         const _nint nm = t2.value.num.n.value.i;
         if (nm >= 1950LL && nm <= 2100LL) {
            throw SyntaxError(str(L"instead of '", t1.getOriginString(uro), L" ", s, L" ", toStr(nm),
               L"', write '", t1.getOriginString(uro), L".year ", s, L" ", toStr(nm), L"'"), tks.first().line);
         }
         else {
            throw SyntaxError(str(L"time variable '", t1.getOriginString(uro),
               L"' cannot be compared with a number"), tks.first().line);
         }
      }
      else if (isInteger1 && isVar2) {
         const _nint nm = t1.value.num.n.value.i;
         if (nm >= 1950LL && nm <= 2100LL) {
            throw SyntaxError(str(L"instead of '", toStr(nm), L" ", s, L" ", t2.getOriginString(uro),
               L"', write '", toStr(nm), L" ", s, L" ", t2.getOriginString(uro), L".year'"), tks.first().line);
         }
         else {
            throw SyntaxError(str(L"time variable '", t2.getOriginString(uro),
               L"' cannot be compared with a number"), tks.first().line);
         }
      }
   }

   // try to parse comparison for every singular data type
   if (parseComparisonUnit<_bool>(result, left, right, ct, uro)
    || parseComparisonUnit<_num>(result, left, right, ct, uro)
    || parseComparisonUnit<_per>(result, left, right, ct, uro)
    || parseComparisonUnit<_tim>(result, left, right, ct, uro)
    || parseComparisonUnit<_str>(result, left, right, ct, uro)) {
      return true;
   }

   // comparisons between singular values have failed, so try comparisons of collections
   return parseCollectionComparisons(result, left, right, ct, uro);
}

_bool comparisonDefList(_genptr<_bool>& result, _defptr& def, _genptr<_list>& list, const gen::CompType& ct,
   const _bool reversed, _uro& uro)
{
   switch (ct) {
      case gen::ct_Equals: {
         result = std::make_unique<gen::DefinitionListEqual>(def, list, uro);
         break;
      }
      case gen::ct_NotEquals: {
         result = std::make_unique<gen::DefinitionListNotEqual>(def, list, uro);
         break;
    }
      default: {
         if (reversed) {
            switch(ct) {
               case gen::ct_Smaller: {
                  result = std::make_unique<gen::DefinitionListBigger>(def, list, uro);
                  break;
               }
               case gen::ct_SmallerEquals: {
                  result = std::make_unique<gen::DefinitionListBiggerEquals>(def, list, uro);
                  break;
               }
               case gen::ct_Bigger: {
                  result = std::make_unique<gen::DefinitionListSmaller>(def, list, uro);
                  break;
               }
               case gen::ct_BiggerEquals: {
                  result = std::make_unique<gen::DefinitionListSmallerEquals>(def, list, uro);
                  break;
               }
               default: {
                  return false;
               }
            }
         }
         else {
            switch(ct) {
               case gen::ct_Smaller: {
                  result = std::make_unique<gen::DefinitionListSmaller>(def, list, uro);
                  break;
               }
               case gen::ct_SmallerEquals: {
                  result = std::make_unique<gen::DefinitionListSmallerEquals>(def, list, uro);
                  break;
               }
               case gen::ct_Bigger: {
                  result = std::make_unique<gen::DefinitionListBigger>(def, list, uro);
                  break;
               }
               case gen::ct_BiggerEquals: {
                  result = std::make_unique<gen::DefinitionListBiggerEquals>(def, list, uro);
                  break;
               }
               default: {
                  return false;
               }
            }
         }
         break;
      }
   }

   return true;
}


template <typename T>
_bool comparisonCollections(_genptr<_bool>& result, const Tokens& left,
   const Tokens& right, const gen::CompType& ct, _uro& uro)
{
   _genptr<std::vector<T>> leftValue;
   if (parse(uro, left, leftValue)) {
      _genptr<std::vector<T>> rightValue;

      if (parse(uro, right, rightValue)) {
         switch(ct) {
            case gen::ct_Equals: {
               result = std::make_unique<gen::CollectionsEqual<T>>(leftValue, rightValue);
               break;
            }
            case gen::ct_NotEquals: {
               result = std::make_unique<gen::CollectionsNotEqual<T>>(leftValue, rightValue);
               break;
            }
            case gen::ct_Smaller: {
               result = std::make_unique<gen::CollectionsSmaller<T>>(leftValue, rightValue);
               break;
            }
            case gen::ct_SmallerEquals: {
               result = std::make_unique<gen::CollectionsSmallerEquals<T>>(leftValue, rightValue);
               break;
            }
            case gen::ct_Bigger: {
               result = std::make_unique<gen::CollectionsBigger<T>>(leftValue, rightValue);
               break;
            }
            case gen::ct_BiggerEquals: {
               result = std::make_unique<gen::CollectionsBiggerEquals<T>>(leftValue, rightValue);
               break;
            }
         }
         return true;
      }
   }

   return false;
}


template <typename T>
_bool comparisonCollectionValue(_genptr<_bool>& result, const Tokens& left, const Tokens& right,
   const gen::CompType& ct, _uro& uro)
{
   _genptr<T> leftValue;
   if (parse(uro, left, leftValue)) {
      _genptr<std::vector<T>> rightCollection;

      if (parse(uro, right, rightCollection)) {
         switch(ct) {
            case gen::ct_Equals: {
               result = std::make_unique<gen::CollectionValueEquals<T>>(rightCollection, leftValue);
               break;
            }
            case gen::ct_NotEquals: {
               result = std::make_unique<gen::CollectionValueNotEquals<T>>(rightCollection, leftValue);
               break;
            }
            case gen::ct_Smaller: {
               result = std::make_unique<gen::CollectionValueBigger<T>>(rightCollection);
               break;
            }
            case gen::ct_SmallerEquals: {
               result = std::make_unique<gen::CollectionValueBiggerEquals<T>>(rightCollection);
               break;
            }
            case gen::ct_Bigger: {
               result = std::make_unique<gen::CollectionValueSmaller<T>>(rightCollection);
               break;
            }
            case gen::ct_BiggerEquals: {
               result = std::make_unique<gen::CollectionValueSmallerEquals<T>>(rightCollection);
               break;
            }
         }
         return true;
      }
      else {
         return false;
      }
   }

   _genptr<T> rightValue;
   if (parse(uro, right, rightValue)) {
      _genptr<std::vector<T>> leftCollection;

      if (parse(uro, left, leftCollection)) {
         switch(ct) {
            case gen::ct_Equals: {
               result = std::make_unique<gen::CollectionValueEquals<T>>(leftCollection, rightValue);
               break;
            }
            case gen::ct_NotEquals: {
               result = std::make_unique<gen::CollectionValueNotEquals<T>>(leftCollection, rightValue);
               break;
            }
            case gen::ct_Smaller: {
               result = std::make_unique<gen::CollectionValueSmaller<T>>(leftCollection);
               break;
            }
            case gen::ct_SmallerEquals: {
               result = std::make_unique<gen::CollectionValueSmallerEquals<T>>(leftCollection);
               break;
            }
            case gen::ct_Bigger: {
               result = std::make_unique<gen::CollectionValueBigger<T>>(leftCollection);
               break;
            }
            case gen::ct_BiggerEquals: {
               result = std::make_unique<gen::CollectionValueBiggerEquals<T>>(leftCollection);
               break;
            }
         }

         return true;
      }
      else {
         return false;
      }
   }

   return false;
}

static _bool parseCollectionComparisons(_genptr<_bool>& result, const Tokens& left,
   const Tokens& right, const gen::CompType& ct, _uro& uro)
{
   _defptr leftDef;
   _defptr rightDef;
   const _bool hasLeftDef = parse(uro, left, leftDef);
   const _bool hasRightDef = parse(uro, right, rightDef);

   // special case situations
   // when definition is compared with other data types or with another definition
   // need optimized solutions
   if (hasLeftDef || hasRightDef) {
      if (hasLeftDef && hasRightDef) {
         switch(ct) {
            case gen::ct_Equals: {
               result = std::make_unique<gen::DefinitionsEqual>(leftDef, rightDef, uro);
               break;
            }
            case gen::ct_NotEquals: {
               result = std::make_unique<gen::DefinitionsNotEqual>(leftDef, rightDef, uro);
               break;
            }
            case gen::ct_Smaller: {
               result = std::make_unique<gen::DefinitionsSmaller>(leftDef, rightDef, uro);
               break;
            }
            case gen::ct_SmallerEquals: {
               result = std::make_unique<gen::DefinitionsSmallerEquals>(leftDef, rightDef, uro);
               break;
            }
            case gen::ct_Bigger: {
               result = std::make_unique<gen::DefinitionsBigger>(leftDef, rightDef, uro);
               break;
            }
            case gen::ct_BiggerEquals: {
               result = std::make_unique<gen::DefinitionsBiggerEquals>(leftDef, rightDef, uro);
               break;
            }
         }

         return true;
      }

      if (hasLeftDef) {
         _genptr<_list> rightList;
         return parse(uro, right, rightList)
            ? comparisonDefList(result, leftDef, rightList, ct, false, uro)
            : false;
      }
      else {
         _genptr<_list> leftList;
         return parse(uro, left, leftList)
            ? comparisonDefList(result, rightDef, leftList, ct, true, uro)
            : false;
      }
   }

   return comparisonCollectionValue<_tim>(result, left, right, ct, uro)
       || comparisonCollectionValue<_num>(result, left, right, ct, uro)
       || comparisonCollectionValue<_str>(result, left, right, ct, uro)
       || comparisonCollections<_tim>(result, left, right, ct, uro)
       || comparisonCollections<_num>(result, left, right, ct, uro)
       || comparisonCollections<_str>(result, left, right, ct, uro);
}

static std::pair<Tokens, Tokens> prepareComparison(const Tokens& tks, const _char sign, gen::CompType& ctype)
{
   std::pair<Tokens, Tokens> result = tks.divideBySymbol(sign);
   _bool eq = false;

   if (result.first.isEmpty()) {
      if (result.second.isEmpty()) {
         throw SyntaxError(str(L"both sides of ", toStr(sign),
            L" comparison are empty"), tks.first().line);
      }
      else {
         throw SyntaxError(str(L"left side of ", toStr(sign),
            L" comparison is empty"), tks.first().line);
      }
   }

   if (result.second.isEmpty()) {
      throw SyntaxError(str(L"right side of ", toStr(sign),
         L" comparison is empty"), tks.last().line);
   }

   if (result.second.first().isSymbol(CHAR_EQUAL_SIGN)) {
      if (result.second.getLength() == 1) {
         throw SyntaxError(str(L"right side of ", toStr(sign),
            L"= comparison is empty"), result.second.first().line);
      }

      result.second.trimLeft();
      eq = true;
   }
   else if (sign == CHAR_EXCLAMATION_MARK) {
      throw SyntaxError(L"expected = after exclamation mark. "
         L"For a simple negation, use keyword 'not' instead",
         result.second.first().line);
   }

   switch (sign) {
      case CHAR_SMALLER:
         ctype = eq ? gen::CompType::ct_SmallerEquals : gen::CompType::ct_Smaller;
         break;
      case CHAR_GREATER:
         ctype = eq ? gen::CompType::ct_BiggerEquals : gen::CompType::ct_Bigger;
         break;
      case CHAR_EQUAL_SIGN:
         ctype = gen::CompType::ct_Equals;
         break;
      case CHAR_EXCLAMATION_MARK: {
         ctype = gen::CompType::ct_NotEquals;
         break;
      default:
         ctype = gen::CompType::ct_Equals;
         break;
      }
   }

   return result;
}

}
