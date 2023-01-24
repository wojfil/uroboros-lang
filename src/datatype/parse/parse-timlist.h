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

#ifndef PARSE_TIMLIST_H
#define PARSE_TIMLIST_H

#include "../parse-gen.h"


namespace uro::parse
{

_bool parseTimList(_genptr<_tlist>& result, const Tokens& tks, _uro& uro);

static _bool parseTimListed(_genptr<_tlist>& result, const Tokens& tks, _uro& uro);
static _bool parseListedTimes(_genptr<_tlist>& res, const std::vector<Tokens>& elements, _uro& uro);
static _bool timeFromTwoSeqs(_genptr<_tim>& result, const Tokens& prev, const Tokens& curr, _uro& uro);
static _bool parseListedTimLists(_genptr<_tlist>& res, const std::vector<Tokens>& elements, _uro& uro);

}

#endif /* PARSE_TIMLIST_H */
