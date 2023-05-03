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

#include "func-string.h"
#include "../../lexer.h"
#include "../../util.h"
#include "../math.h"
#include <cwctype>
#include <algorithm>
#include <sstream>
#include <cwctype>
#include <cmath>
#include <bitset>


namespace perun2::func
{

_str F_After::getValue()
{
   const _str s1 = arg1->getValue();
   if (s1.empty()) {
      return s1;
   }

   const _str s2 = arg2->getValue();
   if (s2.empty()) {
      return s2;
   }

   if (s2.size() == 1) {
      for (_size i = 0; i < s1.size(); i++) {
         if (s1[i] == s2[0]) {
            return i == s1.size() - 1
               ? _str()
               : s1.substr(i + 1);
         }
      }

      return _str();
   }
   else {
      auto pos = s1.find(s2);
      return pos == _str::npos
         ? _str()
         : s1.substr(pos + s2.size());
   }
}


_str F_Before::getValue()
{
   const _str s1 = arg1->getValue();
   if (s1.empty()) {
      return s1;
   }

   const _str s2 = arg2->getValue();
   if (s2.empty()) {
      return s2;
   }

   if (s2.size() == 1) {
      for (_size i = 0; i < s1.size(); i++) {
         if (s1[i] == s2[0]) {
            return i == 0
               ? _str()
               : s1.substr(0, i);
         }
      }

      return _str();
   }
   else {
      auto pos = s1.find(s2);
      return pos == _str::npos
         ? _str()
         : s1.substr(0, pos);
   }
}


_str F_Digits::getValue()
{
   const _str s1 = arg1->getValue();
   _size len2 = 0;
   _size it = 0;

   for (const _char ch : s1) {
      if (std::iswdigit(ch)) {
         len2++;
      }
   }

   _str s2(len2, CHAR_SPACE);

   for (const _char ch : s1) {
      if (std::iswdigit(ch)) {
         s2[it] = ch;
         it++;
      }
   }

   return s2;
}


_str F_Fill::getValue()
{
   const _str base = arg1->getValue();
   const _size len = base.size();
   const _nint v = arg2->getValue().toInt();

   if (v <= NINT_ZERO) {
      return base;
   }

   const _size min = static_cast<_size>(v);

   return len >= min
      ? base
      : str(_str(min - len, CHAR_0), base);
}


_str F_Letters::getValue()
{
   const _str s1 = arg1->getValue();
   _size len2 = 0;
   _size it = 0;

   for (const _char ch : s1) {
      if (std::iswalpha(ch)) {
         len2++;
      }
   }

   _str s2(len2, CHAR_SPACE);
   for (const _char ch : s1) {
      if (std::iswalpha(ch)) {
         s2[it] = ch;
         it++;
      }
   }

   return s2;
}


_str F_Lower::getValue()
{
   _str value = arg1->getValue();
   if (!value.empty()) {
      toLower(value);
   }
   return value;
}


_str F_Trim::getValue()
{
   const _str value = arg1->getValue();
   const _int length = value.size();

   if (length == 0) {
      return value;
   }

   _int left;
   for (left = 0; left < length; left++) {
      if (!(std::iswspace(value[left]))) {
         break;
      }
   }

   if (left == length) {
      return _str();
   }

   _int right;
   for (right = length - 1; right >= 0; --right) {
      if (!(std::iswspace(value[right]))) {
         break;
      }
   }

   return right == length - 1
      ? value.substr(left)
      : value.substr(left, right - left + 1);
}


_str F_Upper::getValue()
{
   _str value = arg1->getValue();
   if (!value.empty()) {
      toUpper(value);
   }
   return value;
}


_str F_Repeat::getValue()
{
   const _nint repeats = arg2->getValue().toInt();
   if (repeats <= NINT_ZERO) {
      return _str();
   }

   const _str base = arg1->getValue();
   const _size len = base.size();

   switch (len) {
      case 0: {
         return _str();
      }
      case 1: {
         return _str(repeats, base[0]);
      }
   }

   switch (repeats) {
      case 1: {
         return base;
      }
      case 2: {
         return str(base, base);
      }
      case 3: {
         return str(base, base, base);
      }
   }

   _str result;
   result.reserve(len * repeats);

   for (_nint i = NINT_ZERO; i < repeats; i++) {
      result += base;
   }

   return result;
}


_str F_Reverse::getValue()
{
   _str value = arg1->getValue();

   if (value.size() <= 1) {
      return value;
   }

   std::reverse(value.begin(), value.end());
   return value;
}


_str F_Left::getValue()
{
   const _nint left = arg2->getValue().toInt();
   if (left <= NINT_ZERO) {
      return _str();
   }

   const _str value = arg1->getValue();
   const _nint length = static_cast<_nint>(value.size());

   return left >= length
      ? value
      : value.substr(0, left);
}


_str F_Right::getValue()
{
   const _nint right = arg2->getValue().toInt();
   if (right <= NINT_ZERO) {
      return _str();
   }

   const _str value = arg1->getValue();
   const _nint length = static_cast<_nint>(value.size());

   return right >= length
      ? value
      : value.substr(length - right);
}


_str F_Substring_2::getValue()
{
   _nint index = arg2->getValue().toInt();
   const _str value = arg1->getValue();

   if (index == NINT_ZERO) {
      return value;
   }

   const _nint length = static_cast<_nint>(value.size());

   if (index < NINT_ZERO) {
      index *= NINT_MINUS_ONE;

      return index >= length
         ? value
         : value.substr(length - index);
   }
   else {
      return index >= length
         ? _str()
         : value.substr(index);
   }
}


_str F_Substring_3::getValue()
{
   const _str value = arg1->getValue();
   _nint index = arg2->getValue().toInt();
   _nint index2 = arg3->getValue().toInt();
   const _nint length = static_cast<_nint>(value.size());

   if (index2 <= NINT_ZERO) {
      return _str();
   }

   if (index < NINT_ZERO) {
      index *= NINT_MINUS_ONE;

      if (index >= length) {
         const _nint lets = length - index + index2;
         return lets <= NINT_ZERO
            ? _str()
            : value.substr(0, lets);
      }
      else {
         const _nint start = length - index;
         return start + index2 >= length
            ? value.substr(start)
            : value.substr(start, index2);
      }
   }
   else {
      if (index >= length) {
         return _str();
      }

      return index + index2 >= length
         ? value.substr(index)
         : value.substr(index, index2);
   }
}


_str F_ConcatenateUnit::getValue()
{
   const _list values = this->arg1->getValue();

   switch (values.size()) {
      case 0: {
         return _str();
      }
      case 1: {
         return values[0];
      }
      case 2: {
         return str(values[0], values[1]);
      }
   }

   _stream ss;

   for (const _str& v : values) {
      ss << v;
   }

   return ss.str();
}


_str F_Concatenate::getValue()
{
   _stream ss;

   for (_genptr<_str>& gen : values) {
      ss << gen->getValue();
   }

   return ss.str();
}


_str F_FirstDef::getValue()
{
   if (definition->hasNext()) {
      const _str v = definition->getValue();
      definition->reset();
      return v;
   }
   else {
      return _str();
   }
}


_str F_LastDef::getValue()
{
   if (!definition->hasNext()) {
      return _str();
   }

   while (definition->hasNext()) { }
   return definition->getValue();
}


_str F_Replace::getValue()
{
   _str base = arg1->getValue();
   _size len = base.size();

   if (len == 0) {
      return base;
   }

   const _str v1 = arg2->getValue();
   const _str v2 = arg3->getValue();

   switch (v1.size()) {
      case 0: {
         break;
      }
      case 1: {
         switch (v2.size()) {
            case 0: {
               base.erase(std::remove(base.begin(), base.end(), v1[0]), base.end());
               break;
            }
            case 1: {
               std::replace(base.begin(), base.end(), v1[0], v2[0]);
               break;
            }
            default: {
               for (_size i = 0; i < len; i++) {
                  if (base[i] == v1[0]) {
                     if (i == 0) {
                        base = str(v2, base.substr(1));
                     }
                     else if (i == len - 1) {
                        return str(base.substr(0, len - 1), v2);
                     }
                     else {
                        base = str(base.substr(0, i), v2, base.substr(i + 1));
                     }

                     i += v2.size() - 1;
                     len += v2.size() - 1;
                  }
               }
               break;
            }
         }
         break;
      }
      default: {
         size_t i = 0;

         switch (v2.size()) {
            case 0: {
               while ((i = base.find(v1)) != _str::npos) {
                  base.erase(i, v1.size());
               }
               break;
            }
            default: {
               while ((i = base.find(v1, i)) != _str::npos) {
                  base.replace(i, v1.size(), v2);
                  i += v2.size();
               }
               break;
            }
         }
         break;
      }
   }

   return base;
}


_str F_String_B::getValue()
{
   return arg1->getValue() ? toStr(CHAR_1) : toStr(CHAR_0);
}


_str F_String_N::getValue()
{
   return arg1->getValue().toString();
}


_str F_String_T::getValue()
{
   return arg1->getValue().toString();
}


_str F_String_P::getValue()
{
   return arg1->getValue().toString();
}


_str F_MonthName::getValue()
{
   return monthToString(static_cast<_tnum>(arg1->getValue().toInt()));
}


_str F_MonthNameFromTime::getValue()
{
   const _tim t = arg1->getValue();
   return t.type == Time::tt_YearMonth
      ? _str()
      : monthToString(t.month);
}


_str F_WeekDayName::getValue()
{
   return weekdayToString(static_cast<_tnum>(arg1->getValue().toInt()));
}


_str F_WeekDayNameFromTime::getValue()
{
   const _tim t = arg1->getValue();
   return t.type == Time::tt_YearMonth
      ? _str()
      : weekdayToString(t.getWeekDay());
}


_str F_AfterDigits::getValue()
{
   const _str value = arg1->getValue();
   _bool after = false;

   for (_size i = 0; i < value.size(); i++) {
      if (after) {
         if (!std::iswdigit(value[i])) {
            return value.substr(i);
         }
      }
      else {
         if (std::iswdigit(value[i])) {
            after = true;
         }
      }
   }

   return _str();
}


_str F_AfterLetters::getValue()
{
   const _str value = arg1->getValue();
   _bool after = false;

   for (_size i = 0; i < value.size(); i++) {
      if (after) {
         if (!std::iswalpha(value[i])) {
            return value.substr(i);
         }
      }
      else {
         if (std::iswalpha(value[i])) {
            after = true;
         }
      }
   }

   return _str();
}


_str F_BeforeDigits::getValue()
{
   const _str value = arg1->getValue();

   for (_size i = 0; i < value.size(); i++) {
      if (std::iswdigit(value[i])) {
         return i == 0
            ? _str()
            : value.substr(0, i);
      }
   }

   return _str();
}


_str F_BeforeLetters::getValue()
{
   const _str value = arg1->getValue();

   for (_size i = 0; i < value.size(); i++) {
      if (std::iswalpha(value[i])) {
         return i == 0
            ? _str()
            : value.substr(0, i);
      }
   }

   return _str();
}


_str F_RandomChar::getValue()
{
   const _str value = arg1->getValue();

   switch (value.size()) {
      case 0:
      case 1: {
         return value;
      }
      default: {
         return toStr(value[math.randomInt(value.size() - 1)]);
      }
   }
}


_str F_Capitalize::getValue()
{
   _str value = arg1->getValue();

   if (value.empty()) {
      return value;
   }

   _bool prevLetter = false;

   for (_char& ch : value) {
      const _bool isLetter = std::iswalpha(ch);

      if (isLetter) {
         if (prevLetter) {
            toLower(ch);
         }
         else {
            prevLetter = true;
            toUpper(ch);
         }
      }
      else {
         prevLetter = false;
      }
   }

   return value;
}


_str F_Join::getValue()
{
   _list values = arg1->getValue();

   switch (values.size()) {
      case 0: {
         return _str();
      }
      case 1: {
         return values[0];
      }
      case 2: {
         return str(values[0], arg2->getValue(), values[1]);
      }
      default: {
         const _str separator = arg2->getValue();

         if (separator.empty()) {
            _stream ss;

            for (const _str& val : values) {
               ss << val;
            }

            return ss.str();
         }
         else {
            _stream ss;
            ss << values[0];

            for (_size i = 1; i < values.size(); i++) {
               ss << separator;
               ss << values[i];
            }

            return ss.str();
         }
      }
   }
}


_str F_Roman::getValue()
{
   const _num base = arg1->getValue();
   _nint number = base.toInt();

   if (number == NINT_ZERO) {
      if (base.isDouble) {
         _stream ss;
         appendFraction(base, ss);
         _str result = ss.str();

         if (result.empty()) {
            result = CHAR_N;
         }
         else if (base.value.d < NDOUBLE_ZERO) {
            result = str(CHAR_MINUS, result);
         }

         return result;
      }
      else {
         return toStr(CHAR_N);
      }
   }
   else if (number >= ROMAN_MAXIMUM || number <= -ROMAN_MAXIMUM) {
      return base.toString();
   }

   _stream ss;
   if (number < NINT_ZERO) {
      ss << CHAR_MINUS;
      number *= NINT_MINUS_ONE;
   }

   const _bool isBig = number >= ROMAN_VINCULUM_THRESHOLD;
   _int i = 24;

   while (number > 0) {
      _nint div = number / ROMAN_NUMBER_LITERALS[i];
      number = number % ROMAN_NUMBER_LITERALS[i];
      while (div--) {
         ss << ((isBig && i == 12) 
            ? ROMAN_VINCULUM_THOUSAND 
            : ROMAN_STRING_LITERALS[i]);
      }
      i--;
   }

   if (base.isDouble) {
      appendFraction(base, ss);
   }

   return ss.str();
}


inline void F_Roman::appendFraction(const _num& base, _stream& ss) const
{
   _int oc = static_cast<_int>(std::fmod(base.value.d, NDOUBLE_ONE) * NDOUBLE_TWELVE);
   if (oc < 0) {
      oc *= -1;
   }
   if (oc >= 6) {
      oc -= 6;
      ss << CHAR_S;
   }

   if (oc > 0) {
      ss << _str(oc,  CHAR_INTERPUNCT);
   }
}


_str F_Binary::getValue()
{
   _nint v = arg1->getValue().toInt();
   _stream ss;
   _bool negative = false;

   if (v < 0) {
      v *= -1;
      negative = true;
   }

   ss << std::bitset<BITS_IN_NINT>(v);
   const _str val = ss.str();

   for (_size i = 0; i < val.size(); i++) {
      if (val[i] != CHAR_0) {
         return negative
            ? str(CHAR_MINUS, val.substr(i))
            : val.substr(i);
      }
   }

   return toStr(CHAR_0);
}

_str F_Hex::getValue()
{
   _nint v = arg1->getValue().toInt();

   if (v < NINT_ZERO) {
      v *= NINT_MINUS_ONE;
      _stream oss;
      oss << std::hex << v;
      return str(CHAR_MINUS, oss.str());
   }
   else {
      _stream oss;
      oss << std::hex << v;
      return oss.str();
   }
}

}
