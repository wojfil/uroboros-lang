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

#include "func-bool.h"
#include "..\..\os.h"
#include <cwctype>


namespace perun2::func
{



_bool F_AnyInside::getValue()
{
   if (!this->fileContext->v_exists->value || !this->fileContext->v_isdirectory->value) {
      return false;
   }

   this->locContext->loadData(this->fileContext->trimmed);
   const _bool any = this->definition->hasNext();

   if (any) {
      this->definition->reset();
   }

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
      if (this->perun2.isNotRunning()) {
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

   for (const _char ch : value) {
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

   for (const _char ch : value) {
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
      const _char ch = value[i];
      
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
   if (!this->fileContext->v_exists->value || !this->fileContext->v_isdirectory->value) {
      return false;
   }

   const _str base = os_leftJoin(this->locContext->location->value, this->fileContext->trimmed);
   const _str value = os_trim(this->arg1->getValue());

   if (os_isInvaild(value)) {
      return false;
   }

   return os_exists(os_leftJoin(base, value));
}


_bool F_ExistInside::getValue()
{
   if (!this->fileContext->v_exists->value || !this->fileContext->v_isdirectory->value) {
      return false;
   }

   const _str base = os_leftJoin(this->locContext->location->value, this->fileContext->trimmed);
   const _list values = this->arg1->getValue();
   const _size len = values.size();

   for (_size i = 0; i < len; i++) {
      const _str& v = os_trim(values[i]);

      if (os_isInvaild(v)) {
          return false;
      }

      const _str path = os_leftJoin(base, v);
      if (!os_exists(path)) {
         return false;
      }
   }

   return true;
}


_bool F_StartsWithChar::getValue()
{
   const _str v = value->getValue();

   return v.empty()
      ? false
      : (v[0] == character);
}


_bool F_EndsWithChar::getValue()
{
   const _str v = value->getValue();

   return v.empty()
      ? false
      : (v[v.size() - 1] == character);
}


_bool F_FindText::getValue()
{
   if (!this->context->v_exists->value || !this->context->v_isfile->value) {
      return false;
   }

   const _str value = this->arg1->getValue();
   if (value.empty()) {
      return true;
   }

   return os_findText(this->context->v_path->value, value);
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

   for (_size i = 0; i < constantLength; i++) {
      if (v[len - constantLength + i] != constant[i]) {
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

   for (const _char ch : value) {
      if (!std::iswalpha(ch)) {
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

   for (const _char ch : value) {
      if (!std::iswdigit(ch)) {
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

   for (const _char ch : value) {
      switch (ch) {
         case CHAR_0:
         case CHAR_1:{
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

   for (const _char ch : value) {
      switch (ch) {
         case CHAR_0: case CHAR_1: case CHAR_2: case CHAR_3: case CHAR_4:
         case CHAR_5: case CHAR_6: case CHAR_7: case CHAR_8: case CHAR_9:
         case CHAR_a: case CHAR_b: case CHAR_c: case CHAR_d: case CHAR_e: case CHAR_f:
         case CHAR_A: case CHAR_B: case CHAR_C: case CHAR_D: case CHAR_E: case CHAR_F: {
            break;
         }
         default: {
            return false;
         }
      }
   }

   return true;
}

_bool F_Any::getValue()
{
   const _bool result = this->definition->hasNext();

   if (result) {
      this->definition->reset();
   }

   return result;
}

F_Exists::F_Exists(_genptr<_str>& a1, p_perun2& perun2)
   : Func_1(a1), context(perun2.contexts.getLocationContext()) { };

_bool F_Exists::getValue()
{
   const _str value = os_trim(this->arg1->getValue());

   if (os_isInvaild(value)) {
      return false;
   }

   return os_exists(os_leftJoin(this->context->location->value, value));
}

F_Exist::F_Exist(_genptr<_list>& a1, p_perun2& perun2)
   : Func_1(a1), context(perun2.contexts.getLocationContext())  { };

_bool F_Exist::getValue()
{
   const _list values = this->arg1->getValue();

   for (const _str& val : values) {
      const _str& v = os_trim(val);

      if (os_isInvaild(v)) {
          return false;
      }

      const _str path = os_leftJoin(this->context->location->value, v);
      if (!os_exists(path)) {
         return false;
      }
   }

   return true;
}

}
