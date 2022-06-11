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

#ifndef PARSE_GENERIC_H
#define PARSE_GENERIC_H

#include "../parse-gen.h"

_boo isPossibleListElement(const Tokens& tks);
Generator<_num>* parseListElementIndex(const Tokens& tks);
_boo isPossibleBinary(const Tokens& tks);
_boo isPossibleTernary(const Tokens& tks);
void checkLimitBySize(const Tokens& tks);
_boo isPossibleListElementMember(const Tokens& tks);

#endif /* PARSE_GENERIC_H */
