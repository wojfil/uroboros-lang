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

#include "parse-period.h"
#include "parse-generic.h"
#include "../generator/gen-generic.h"
#include "../generator/gen-period.h"
#include "../generator/gen-time.h"
#include "../../brackets.h"
#include "../../util.h"
#include "../function.h"
#include "../../hash.h"
#include "../parse-gen.h"


namespace uro::parse
{

Generator<_per>* parsePeriod(const Tokens& tks, uro::Uroboros* uro)
{
   const _size len = tks.getLength();

   if (len == 1) {
      Generator<_per>* unit = nullptr;
      parseOneToken(uro, tks, unit);
      return unit;
   }

   if (tks.check(TI_HAS_CHAR_COMMA) || tks.check(TI_HAS_FILTER_KEYWORD)) {
      return nullptr;
   }

   Tokens tks2(tks);
   tks2.trimLeft();

   const _boo startsWithMinus = tks.first().isSymbol(L'-');
   const _boo lastIsWord = tks.last().type == Token::t_Word;
   const _boo hasPluses =  tks.check(TI_HAS_CHAR_PLUS);
   const _boo hasMinuses = tks2.check(TI_HAS_CHAR_MINUS);

   if (len >= 3 && hasPluses || hasMinuses) {
      Generator<_per>* exp = parsePeriodExp(tks, uro);
      if (exp != nullptr) {
         return exp;
      }
   }

   if (len >= 2 && !hasPluses && !hasMinuses && lastIsWord) {
      if (len == 2) {
         Generator<_per>* cnst = parsePeriodConst(tks, false, uro);
         if (cnst != nullptr) {
            return cnst;
         }
      }
      else if (len == 3 && tks.first().isSymbol(L'-')) {
         Generator<_per>* ncnst = parsePeriodConst(tks2, true, uro);
         if (ncnst != nullptr) {
            return ncnst;
         }
      }

      if (!tks.check(TI_HAS_CHAR_QUESTION_MARK) && !tks.check(TI_HAS_CHAR_COLON)) {
         Generator<_per>* unt = parsePeriodUnit(tks, uro);
         if (unt != nullptr) {
            return unt;
         }
      }
   }

   if (len >= 2 && !hasPluses && !hasMinuses) {
      if (startsWithMinus) {
         Generator<_per>* per;
         if (parse(uro, tks2, per)) {
            return new gen::NegatedPeriod(per);
         }
      }
   }

   if (tks.check(TI_IS_POSSIBLE_FUNCTION)) {
      Generator<_per>* func = func::periodFunction(tks, uro);
      if (func != nullptr) {
         return func;
      }
   }

   Generator<_per>* bin = parseBinary<_per>(tks, uro);
   if (bin != nullptr) {
      return bin;
   }

   Generator<_per>* tern = parseTernary<_per>(tks, uro);
   if (tern != nullptr) {
      return tern;
   }

   return nullptr;
}

static Generator<_per>* parsePeriodConst(const Tokens& tks, const _boo& negated, uro::Uroboros* uro)
{
   const Token& last = tks.last();
   const Token& first = tks.first();

   if (first.type != Token::t_Number) {
      return nullptr;
   }

   const _size& h = last.value.word.h;
   const Number& num = first.value.num.n;

   if (uro->hashes.HASH_GROUP_PERIOD_SINGLE.find(h) !=
       uro->hashes.HASH_GROUP_PERIOD_SINGLE.end())
   {
      const Period::PeriodUnit unit = uro->hashes.HASH_MAP_PERIOD_UNITS.find(h)->second;

      if (num.isDouble) {
         if (num.value.d == 1L) {
            return new gen::Constant<_per>(Period(negated ? -1 : 1, unit));
         }
         else {
            unitNameException(last.getOriginString(uro), tks);
         }
      }
      else {
         if (num.value.i == 1LL) {
            return new gen::Constant<_per>(Period(negated ? -1 : 1, unit));
         }
         else {
            unitNameException(last.getOriginString(uro), tks);
         }
      }
   }

   if (uro->hashes.HASH_GROUP_PERIOD_MULTI.find(h) !=
      uro->hashes.HASH_GROUP_PERIOD_MULTI.end())
   {
      const Period::PeriodUnit unit = uro->hashes.HASH_MAP_PERIOD_UNITS.find(h)->second;

      _tnum v = num.isDouble
         ? static_cast<_tnum>(num.value.d)
         : static_cast<_tnum>(num.value.i);

      if (negated) {
         v *= -1;
      }

      return new gen::Constant<_per>(Period(v, unit));
   }

   return nullptr;
}

static Generator<_per>* parsePeriodUnit(const Tokens& tks, uro::Uroboros* uro)
{
   const _size& h = tks.last().value.word.h;
   Tokens tks2(tks);
   tks2.trimRight();

   Generator<_num>* num;
   if (!parse(uro, tks2, num)) {
      return nullptr;
   }

   if (uro->hashes.HASH_GROUP_PERIOD_SINGLE.find(h) !=
      uro->hashes.HASH_GROUP_PERIOD_SINGLE.end())
   {
      unitNameException(tks.last().getOriginString(uro), tks);
   }

   if (uro->hashes.HASH_GROUP_PERIOD_MULTI.find(h) !=
       uro->hashes.HASH_GROUP_PERIOD_MULTI.end())
   {
      const Period::PeriodUnit unit = uro->hashes.HASH_MAP_PERIOD_UNITS.find(h)->second;
      return new gen::PeriodUnit(num, unit);
   }

   return nullptr;
}

static void unitNameException(const _str& name, const Tokens& tks)
{
   throw SyntaxException(str(L"missing letter 's' at the end of the word '",
      name, L"'"), tks.last().line);
}

static Generator<_per>* parsePeriodExp(const Tokens& tks, uro::Uroboros* uro)
{
   const std::vector<Tokens> elements = tks.splitBySymbol(L'+');
   const _size len = elements.size();

   Generator<_per>* result = parsePeriodExpDiff(elements[0], uro);
   if (result == nullptr) {
      return nullptr;
   }
   if (len == 1) {
      return result;
   }

   for (_size i = 1; i < len; i++) {
      const Tokens& el = elements[i];

      Generator<_per>* per = parsePeriodExpDiff(el, uro);
      if (per == nullptr) {
         delete result;
         return nullptr;
      }
      else {
         result = new gen::PeriodAddition(result, per);
      }
   }

   return result;
}

static Generator<_per>* parsePeriodExpDiff(const Tokens& tks, uro::Uroboros* uro)
{
   const _int baseLen = tks.getLength();
   if (baseLen == 1) {
      Generator<_per>* per;
      return parse(uro, tks, per) ? per : nullptr;
   }

   _boo minusAwaits = tks.first().isSymbol(L'-');

   Tokens tks2(tks);
   if (minusAwaits) {
      tks2.trimLeft();
   }

   if (!tks2.check(TI_HAS_CHAR_MINUS)) {
      Generator<_per>* gp;
      if (parse(uro, tks2, gp)) {
         if (minusAwaits) {
            return new gen::NegatedPeriod(gp);
         }
         else {
            return gp;
         }
      }
      else {
         return nullptr;
      }
   }

   const std::vector<Tokens> elements = tks2.splitBySymbol(L'-');
   const _size len = elements.size();

   _boo isTime;
   _boo hasFirst = false;
   Generator<_tim>* time = nullptr;
   Generator<_per>* result;

   if (parse(uro, elements[0], result)) {
      hasFirst = true;
      isTime = false;
      if (minusAwaits) {
         result = new gen::NegatedPeriod(result);
         minusAwaits = false;
      }
   }
   else {
      if (parse(uro, elements[0], time)) {
         isTime = true;
      }
      else {
         return nullptr;
      }
   }

   for (_size i = 1; i < len; i++) {
      const Tokens& el = elements[i];

      if (isTime) {
         Generator<_tim>* time2;
         if (parse(uro, el, time2)) {
            Generator<_per>* per = new gen::TimeDifference(time, time2);

            if (hasFirst) {
               result = new gen::PeriodSubtraction(result, per);
            }
            else {
               if (minusAwaits) {
                  result = new gen::NegatedPeriod(per);
                  minusAwaits = false;
               }
               else {
                  result = per;
               }
               hasFirst = true;
            }
            time = nullptr;
            isTime = false;
         }
         else {
            Generator<_per>* per3;
            if (parse(uro, el, per3)) {
               Generator<_tim>* tim3 = new gen::DecreasedTime(time, per3);
               time = tim3;
            }
            else {
               delete time;
               return nullptr;
            }
         }
      }
      else {
         Generator<_per>* per;
         if (parse(uro, el, per)) {
            if (minusAwaits) {
               result = new gen::NegatedPeriod(new gen::PeriodSubtraction(result, per));
               minusAwaits = false;
            }
            else {
               result = new gen::PeriodSubtraction(result, per);
            }
         }
         else {
            if (parse(uro, el, time)) {
               isTime = true;
            }
            else {
               delete result;
               return nullptr;
            }
         }
      }
   }

   if (isTime) {
      delete time;
      if (hasFirst) {
         delete result;
         throw SyntaxException(L"time can be subtracted only from another time", tks.last().line);
      }
      return nullptr;
   }

   return result;
}

static Generator<_per>* parseTimeDifference(const Tokens& tks, uro::Uroboros* uro)
{
   std::pair<Tokens, Tokens> pair = tks.divideBySymbol(L'-');

   if (pair.second.isEmpty()) {
      throw SyntaxException(L"expression cannot end with -", tks.last().line);
   }

   if (pair.first.isEmpty()) {
      return nullptr;
   }

   Generator<_tim>* tim1;
   if (parse(uro, pair.first, tim1)) {
      Generator<_tim>* tim2;
      if (parse(uro, pair.second, tim2)) {
         return new gen::TimeDifference(tim1, tim2);
      }
      else {
         delete tim1;
      }
   }

   Generator<_per>* per1;
   if (!parse(uro, pair.first, per1)) {
      return nullptr;
   }

   Generator<_per>* per2;
   if (!parse(uro, pair.second, per2)) {
      delete per1;
      return nullptr;
   }

   return new gen::PeriodSubtraction(per1, per2);
}

}
