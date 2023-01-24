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

#include "gen-list.h"
#include "../../uroboros.h"
#include "../../util.h"


namespace uro::gen
{

_list Join_StrStr::getValue()
{
   _list v;
   v.reserve(2);
   v.emplace_back(left->getValue());
   v.emplace_back(right->getValue());
   return v;
}

_list Join_StrList::getValue()
{
   _list v = right->getValue();
   v.insert(v.begin(), left->getValue());
   return v;
}

_list Join_ListStr::getValue()
{
   _list v = left->getValue();
   v.emplace_back(right->getValue());
   return v;
}

_list Join_ListList::getValue()
{
   _list v = left->getValue();
   langutil::appendVector(v, right->getValue());
   return v;
}

}
