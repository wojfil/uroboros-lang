/*
    This file is part of Perun2.
    Perun2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Perun2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Perun2. If not, see <http://www.gnu.org/licenses/>.
*/

#include <cwctype>
#include <clocale>
#include <locale>
#include "chars.h"


namespace perun2
{

void toLower(p_char& ch)
{
   ch = std::tolower(ch, std::locale(""));
}

void toUpper(p_char& ch)
{
   ch = std::toupper(ch, std::locale(""));
}

p_bool charsEqualInsensitive(p_char ch1, p_char ch2)
{
   toLower(ch1);
   toLower(ch2);
   return ch1 == ch2;
}

}
