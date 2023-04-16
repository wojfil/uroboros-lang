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

#include "concat.h"
#include <wchar.h>


namespace perun2
{

_size unitLen(const _char value)
{
   return 1;
}

_size unitLen(_char (&value)[])
{
   return wcslen(value);
}

_size unitLen(const _str& value)
{
   return value.size();
}

_size charsLen()
{
   return 0;
}

void insertStr(_str& result)
{
  // do nothing as expected
}

}
