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

#include "gen-string.h"
#include "../../os.h"
#include "../../perun2.h"


namespace perun2::gen
{

_str ConcatString::getValue()
{
   _stream ss;
   for (_size i = 0; i < length; i++) {
      ss << value[i]->getValue();
   }
   return ss.str();
}

_str StringBinary::getValue()
{
   return condition->getValue()
      ? value->getValue()
      : _str();
}


LocationReference::LocationReference(_p2& p2)
   : context(*p2.contexts.getLocationContext()) { };

_str LocationReference::getValue()
{
   return this->context.location->value;
}

RelativeLocation::RelativeLocation(_genptr<_str>& val, _p2& p2)
   : value(std::move(val)), context(*p2.contexts.getLocationContext()) { };

_str RelativeLocation::getValue()
{
   return str(this->context.location->value, OS_SEPARATOR, this->value->getValue());
}

_str CharAtIndex::getValue()
{
   const _str v = value->getValue();
   const _size len = v.size();

   if (len == 0) {
      return v;
   }

   _nint n = index->getValue().toInt();

   if (n < NINT_ZERO) {
      n += len;
   }

   return (n >= NINT_ZERO && n < len)
      ? toStr(v[static_cast<_size>(n)])
      : _str();
}

_str DefinitionElement::getValue()
{
   _nint n = index->getValue().toInt();

   if (n < NINT_ZERO) {
      return _str();
   }

   while (this->definition->hasNext()) {
      if (this->perun2.isNotRunning()) {
         this->definition.reset();
         return _str();
      }

      if (n == NINT_ZERO) {
         const _str result = this->definition->getValue();
         this->definition.reset();
         return result;
      }

      n--;
   }

   return _str();
}

}
