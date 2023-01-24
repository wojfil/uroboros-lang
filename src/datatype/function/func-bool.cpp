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

#include "func-bool.h"
#include "..\..\var\var-runtime.h"
#include "..\..\os.h"
#include <cwctype>


namespace uro::func
{

_bool F_AnyDef::getValue()
{
   const _bool any = definition->hasNext();

   if (any) {
      definition->reset();
   }

   return any;
}


_bool F_AnyInside::getValue()
{
   const _str v = os_trim(this->value->getValue());
   if (os_isInvaild(v)) {
      return false;
   }

   const _str path = os_join(this->inner.location.value, v);
   if (!os_directoryExists(path)) {
      return false;
   }

   const _str prevLoc = this->inner.location.value;
   this->inner.location.value = path;

   const _bool any = this->definition->hasNext();
   if (any) {
      this->definition->reset();
   }

   this->inner.location.value = prevLoc;
   return any;
}


_bool F_ContainsStr::getValue()
{
   const _str phrase = this->arg2->getValue();
   if (phrase.empty()) {
      return true;
   }

   const _str base =  this->arg1->getValue();
   if (base.empty()) {
      return false;
   }

   return base.find(phrase) != _str::npos;
}


_bool F_ContainsDef::getValue()
{
   const _str v = this->value->getValue();

   while (this->definition->hasNext()) {
      if (!this->uroboros.state == State::s_Running) {
         this->definition->reset();
         break;
      }

      if (v == this->definition->getValue()) {
         this->definition->reset();
         return true;
      }
   }

   return false;
}


_bool F_IsLower::getValue()
{
   const _str value = this->arg1->getValue();
   if (value.empty()) {
      return false;
   }

   _bool anyLetter = false;
   const _size len = value.size();

   for (_size i = 0; i < len; i++) {
      const _char& ch = value[i];

      if (std::iswalpha(ch)) {
         if (std::iswupper(ch)) {
            return false;
         }
         anyLetter = true;
      }
   }

   return anyLetter;
}


_bool F_IsUpper::getValue()
{
   const _str value = this->arg1->getValue();
   if (value.empty()) {
      return false;
   }

   _bool anyLetter = false;
   const _size len = value.size();

   for (_size i = 0; i < len; i++) {
      const _char& ch = value[i];

      if (std::iswalpha(ch)) {
         if (std::iswlower(ch)) {
            return false;
         }
         anyLetter = true;
      }
   }

   return anyLetter;
}


_bool F_IsNumber::getValue()
{
   return isNumber(this->arg1->getValue());
}


_bool isNumber(const _str& value)
{
   if (value.empty()) {
      return false;
   }

   const _size len = value.size();
   _bool hasDot = false;

   for (_size i = 0; i < len; i++) {
      const _char& ch = value[i];
      if (ch == CHAR_DOT) {
         if (hasDot) {
            return false;
         }
         else {
            hasDot = true;
         }
      }
      else if (ch == CHAR_MINUS) {
         if (i != 0) {
            return false;
         }
      }
      else if (!std::iswdigit(ch)) {
         return false;
      }
   }

   return true;
}


_bool F_StartsWith::getValue()
{
   const _str phrase = this->arg2->getValue();
   if (phrase.empty()) {
      return true;
   }

   const _str base =  this->arg1->getValue();
   if (base.empty()) {
      return false;
   }

   const _size blen = base.size();
   const _size plen = phrase.size();

   if (plen > blen) {
      return false;
   }

   for (_size i = 0; i < plen; i++) {
      if (phrase[i] != base[i]) {
         return false;
      }
   }

   return true;
}


_bool F_EndsWith::getValue()
{
   const _str phrase = this->arg2->getValue();
   if (phrase.empty()) {
      return true;
   }

   const _str base =  this->arg1->getValue();
   if (base.empty()) {
      return false;
   }

   const _size blen = base.size();
   const _size plen = phrase.size();

   if (plen > blen) {
      return false;
   }

   for (_size i = 0; i < plen; i++) {
      if (phrase[plen - 1 - i] != base[blen - 1 - i]) {
         return false;
      }
   }

   return true;
}


_bool F_ExistsInside::getValue()
{
   const _str p = os_trim(this->arg2->getValue());
   if (os_isInvaild(p)) {
      return false;
   }

   const _str base = os_join(this->inner.location.value, p);
   if (!os_directoryExists(base)) {
      return false;
   }

   const _str v = os_trim(this->arg1->getValue());

   if (os_isInvaild(v)) {
      return false;
   }

   const _str path = os_join(base, v);
   return os_exists(path);
}


_bool F_ExistInside::getValue()
{
   const _str p = os_trim(this->arg2->getValue());
   if (os_isInvaild(p)) {
      return false;
   }

   const _str base = os_join(this->inner.location.value, p);
   if (!os_directoryExists(base)) {
      return false;
   }

   const _list vs = this->arg1->getValue();
   const _size len = vs.size();

   if (len == 0) {
      return true;
   }

   if (len == 1) {
      const _str& v = vs[0];
      return !os_isInvaild(v) && os_exists(os_join(base, v));
   }

   for (_size i = 0; i < len; i++) {
      const _str& v = os_trim(vs[i]);
      if (os_isInvaild(v)) {
          return false;
      }

      const _str path = os_join(base, v);
      if (!os_exists(path)) {
         return false;
      }
   }

   return true;
}


_bool F_StartsWithChar::getValue()
{
   const _str v = value->getValue();
   const _size len = v.size();

   return len == 0
      ? false
      : (v[0] == character);
}


_bool F_EndsWithChar::getValue()
{
   const _str v = value->getValue();
   const _size len = v.size();

   return len == 0
      ? false
      : (v[len - 1] == character);
}


_bool F_Find::getValue()
{
   const _str v = os_trim(this->arg1->getValue());

   if (os_isInvaild(v)) {
      return false;
   }

   const _str path = os_join(this->inner.location.value, v);
   return os_find(path, this->arg2->getValue());
}


_bool F_Find_InThis::getValue()
{
   if (!this->inner.exists.value || this->inner.isdirectory.value) {
      return false;
   }

   return os_find(this->inner.path.value, this->arg1->getValue());
}


_bool F_StartsWithConst::getValue()
{
   const _str v = value->getValue();
   const _size len = v.size();
   if (len < constantLength) {
      return false;
   }

   for (_size i = 0; i < constantLength; i++) {
      if (v[i] != constant[i]) {
         return false;
      }
   }

   return true;
}


_bool F_EndsWithConst::getValue()
{
   const _str v = value->getValue();
   const _size len = v.size();
   if (len < constantLength) {
      return false;
   }

   const _size b = len - constantLength;

   for (_size i = 0; i < constantLength; i++) {
      if (v[b + i] != constant[i]) {
         return false;
      }
   }

   return true;
}


_bool F_IsLetter::getValue()
{
   const _str value = this->arg1->getValue();
   if (value.empty()) {
      return false;
   }

   const _size len = value.size();
   for (_size i = 0; i < len; i++) {
      if (!std::iswalpha(value[i])) {
         return false;
      }
   }

   return true;
}


_bool F_IsDigit::getValue()
{
   const _str value = this->arg1->getValue();
   if (value.empty()) {
      return false;
   }

   const _size len = value.size();
   for (_size i = 0; i < len; i++) {
      if (!std::iswdigit(value[i])) {
         return false;
      }
   }

   return true;
}


_bool F_IsBinary::getValue()
{
   const _str value = this->arg1->getValue();
   if (value.empty()) {
      return false;
   }

   const _size len = value.size();
   for (_size i = 0; i < len; i++) {
      switch (value[i]) {
         case DIGIT_0:
         case DIGIT_1:{
            break;
         }
         default: {
            return false;
         }
      }
   }

   return true;
}


_bool F_IsHex::getValue()
{
   const _str value = this->arg1->getValue();
   if (value.empty()) {
      return false;
   }

   const _size len = value.size();
   for (_size i = 0; i < len; i++) {
      switch (value[i]) {
         case DIGIT_0: case DIGIT_1: case DIGIT_2: case DIGIT_3: case DIGIT_4:
         case DIGIT_5: case DIGIT_6: case DIGIT_7: case DIGIT_8: case DIGIT_9:
         case LETTER_a: case LETTER_b: case LETTER_c: case LETTER_d: case LETTER_e: case LETTER_f:
         case LETTER_A: case LETTER_B: case LETTER_C: case LETTER_D: case LETTER_E: case LETTER_F: {
            break;
         }
         default: {
            return false;
         }
      }
   }

   return true;
}

}
