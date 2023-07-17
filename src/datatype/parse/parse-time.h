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

#ifndef PARSE_TIME_H
#define PARSE_TIME_H

#include "../../tokens.h"
#include "../../perun2.h"


namespace perun2::parse
{

p_bool parseTime(_genptr<p_tim>& result, const Tokens& tks, pp_perun2& p2);

p_bool parseTimeConst(_genptr<p_tim>& result, const Tokens& tks, pp_perun2& p2);
static _tnum tokenToTimeNumber(const Token& tk);
static void checkDayCorrectness(const _tnum day, const _tnum month, const _tnum year, const Token& tk);
static p_bool parseTimeExp(_genptr<p_tim>& result, const Tokens& tks, pp_perun2& p2);
static p_bool timeExpUnit(p_int& sublen, const p_bool subtract, p_bool& prevSubtract,
   _genptr<p_tim>& prevTim, _genptr<p_tim>& time, const Tokens& tks, p_int& numReserve, pp_perun2& p2);

}

#endif /* PARSE_TIME_H */
