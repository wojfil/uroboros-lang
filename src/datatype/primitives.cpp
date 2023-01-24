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

#include "primitives.h"


namespace uro
{

_str toStr(const _char& ch) 
{
   return _str(1, ch);
}

_ndouble stringToDouble(const _str& value)
{
   _stream ss(value);
   _ndouble n;
   ss >> n;
   return n;
}

void toLower(_str& value)
{
   const _size len = value.size();

   for (_size i = 0; i < len; i++) {
      value[i] = std::tolower(value[i], std::locale(""));
   }
}

void toUpper(_str& value)
{
   const _size len = value.size();

   for (_size i = 0; i < len; i++) {
      value[i] = std::toupper(value[i], std::locale(""));
   }
}

}
