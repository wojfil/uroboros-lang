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

#ifndef PARSE_ORDER_H_INCLUDED
#define PARSE_ORDER_H_INCLUDED


#include "../../var/var-inner.h"
#include "../../uroboros.h"
#include "../../tokens.h"
#include "../generator/gen-definition.h"
#include "../parse-gen.h"


namespace uro::parse
{

void resetOrderParseSettings(const ThisState& state, const ThisState& prevState, Uroboros& uro);

template <typename T>
void cleanAfterOrderParseFailure(T& result, Attribute* attr, const ThisState& state)
{
   if (state == ThisState::ts_String) {
      delete attr;
   }

   delete result;
}


template <typename T>
void orderUnitFailure(const Token& tk, T& result, Uroboros& uro)
{
   throw SyntaxException(str(L"keyword '", tk.getOriginString(uro),
      L"' is not preceded by a value used for order"), tk.line);
}


template <typename T>
void prepareOrderUnit(Tokens& tks, _bool& desc, T& result, Attribute* attr,
   const ThisState& state, gen::Order* order, gen::OrderIndices* indices, Uroboros& uro)
{
   desc = false;
   const Token& last = tks.last();

   if (last.type == Token::t_Keyword) {
      const Keyword& kw = last.value.keyword.k;
      if (kw == Keyword::kw_Asc) {
         tks.trimRight();
         if (tks.isEmpty()) {
            delete indices;
            if (order != nullptr) {
               delete order;
            }
            cleanAfterOrderParseFailure(result, attr, state);
            orderUnitFailure(last, result, uro);
         }
      }
      else if (kw == Keyword::kw_Desc) {
         desc = true;
         tks.trimRight();
         if (tks.isEmpty()) {
            delete indices;
            if (order != nullptr) {
               delete order;
            }
            cleanAfterOrderParseFailure(result, attr, state);
            orderUnitFailure(last, result, uro);
         }
      }
   }
}


template <typename T>
void setOrderUnit(gen::Order*& order, Generator<T>* value, const _bool& desc, gen::OrderIndices* indices)
{
   if (order == nullptr) {
      order = new gen::OrderUnit_Final<T>(value, desc, indices);
   }
   else {
      order = new gen::OrderUnit_Middle<T>(value, desc, order, indices);
   }
}

void setSingleOrderFilter(Attribute* attr, const _bool& hasMemory, _def*& result,
   gen::OrderIndices* indices, gen::Order* order, Uroboros& uro);

template <typename T>
void setSingleOrderFilter(Attribute* attr, const _bool& hasMemory,
   Generator<std::vector<T>>*& result, gen::OrderIndices* indices, gen::Order* order, Uroboros& uro)
{
   result = new gen::OrderBy_List<T>(result, attr, indices, order, uro);
}


template <typename T, typename T2>
void addOrderByFilter(T& result, const ThisState& state, const Token& orderKeyword,
   Tokens& ts2, _fdata* fdata, Uroboros& uro)
{
   const ThisState prevThisState = uro.vars.inner.thisState;
   uro.vars.inner.thisState = state;
   const _bool hasMemory = uro.vc.anyAttribute();
   Attribute* attr = nullptr;

   if (state == ThisState::ts_String) {
      if (fdata == nullptr) {
         attr = new Attribute(uro);
      }
      else {
         attr = new BridgeAttribute(uro, fdata);
      }

      uro.vc.addAttribute(attr);
   }

   const Token& first = ts2.first();

   if (ts2.getLength() == 1 && first.type == Token::t_Keyword) {
      const Keyword& kw = first.value.keyword.k;
      if (kw == Keyword::kw_Asc || kw == Keyword::kw_Desc) {
         const _bool desc = kw == Keyword::kw_Desc;
         gen::OrderIndices* indices = new gen::OrderIndices();

         switch (state) {
            case ThisState::ts_String: {
               Generator<_str>* str;
               uro.vars.inner.createThisRef(str);
               setSingleOrderFilter(attr, hasMemory, result, indices,
                  new gen::OrderUnit_Final<_str>(str, desc, indices), uro);
               break;
            }
            case ThisState::ts_Number: {
               Generator<_num>* num;
               uro.vars.inner.createThisRef(num);
               setSingleOrderFilter(attr, hasMemory, result, indices,
                  new gen::OrderUnit_Final<_num>(num, desc, indices), uro);
               break;
            }
            case ThisState::ts_Time: {
               Generator<_tim>* tim;
               uro.vars.inner.createThisRef(tim);
               setSingleOrderFilter(attr, hasMemory, result, indices,
                  new gen::OrderUnit_Final<_tim>(tim, desc, indices), uro);
               break;
            }
         }

         resetOrderParseSettings(state, prevThisState, uro);
         return;
      }
   }

   if (!first.isKeyword(Keyword::kw_By)) {
      cleanAfterOrderParseFailure(result, attr, state);
      throw SyntaxException(str(L"keyword '", orderKeyword.getOriginString(uro),
         L"' should be followed by a keyword 'by'"), first.line);
   }

   ts2.trimLeft();
   if (ts2.isEmpty()) {
      cleanAfterOrderParseFailure(result, attr, state);
      throw SyntaxException(str(L"declaration of '", orderKeyword.getOriginString(uro),
         L" ", first.getOriginString(uro), L"' filter is empty"), first.line);
   }

   std::vector<Tokens> tokensList;
   if (ts2.check(TI_HAS_CHAR_COMMA)) {
      tokensList = ts2.splitBySymbol(L',');
   }
   else {
      tokensList.emplace_back(ts2);
   }

   const _int length = tokensList.size();
   gen::Order* order = nullptr;
   gen::OrderIndices* indices = new gen::OrderIndices();

   for (_int i = length - 1; i >= 0; i--) {
      Tokens& tk = tokensList[i];
      _bool desc;
      prepareOrderUnit(tk, desc, result, attr, state, order, indices, uro);

      Generator<_bool>* uboo;
      if (parse(uro, tk, uboo)) {
         setOrderUnit(order, uboo, desc, indices);
         continue;
      }

      Generator<_num>* unum;
      if (parse(uro, tk, unum)) {
         setOrderUnit(order, unum, desc, indices);
         continue;
      }

      Generator<_per>* uper;
      if (parse(uro, tk, uper)) {
         setOrderUnit(order, uper, desc, indices);
         continue;
      }

      Generator<_tim>* utim;
      if (parse(uro, tk, utim)) {
         setOrderUnit(order, utim, desc, indices);
         continue;
      }

      Generator<_str>* ustr;
      if (parse(uro, tk, ustr)) {
         setOrderUnit(order, ustr, desc, indices);
         continue;
      }
      else {
         if (order != nullptr) {
            delete order;
         }
         delete indices;
         cleanAfterOrderParseFailure(result, attr, state);

         throw SyntaxException(L"value of this order unit cannot be resolved to any valid data type. "
            L"Hint: if you use multiple variables for order, separate them by commas",
            tk.first().line);
      }
   }

   setSingleOrderFilter(attr, hasMemory, result, indices, order, uro);
   resetOrderParseSettings(state, prevThisState, uro);
}

}

#endif // PARSE_ORDER_H_INCLUDED
