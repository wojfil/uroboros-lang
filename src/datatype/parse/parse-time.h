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

#ifndef PARSE_TIME_H
#define PARSE_TIME_H

#include "../../tokens.h"


Generator<_tim>* parseTime(const Tokens& tks);

Generator<_tim>* parseTimeConst(const Tokens& tks);
static _tnum monthFromToken(const Token& tk);
static _tnum tokenToTimeNumber(const Token& tk);
static void checkDayCorrectness(const _tnum day, const _tnum month,
   const _tnum year, const Token& tk);
static void clockUnitException(const _str& unit, const _tnum& value,
   const Token& tk);
static Generator<_tim>* parseTimeExp(const Tokens& tks);
static _boo timeExpUnit(_int& sublen, const _boo& subtract, _boo& prevSubtract,
   Generator<_tim>*& prevTim, Generator<_tim>*& time, const Tokens& tks, _int& numReserve);

#endif /* PARSE_TIME_H */
