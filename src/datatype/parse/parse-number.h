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

#ifndef EXP_PARSE_NUMBER_H
#define EXP_PARSE_NUMBER_H

#include "../../tokens.h"
#include "../../uroboros.h"
#include "../exp-element.h"


namespace uro::parse
{

Generator<_num>* parseNumber(const Tokens& tks, uro::Uroboros* uro);

static Generator<_num>* parseNumExp(const Tokens& tks, uro::Uroboros* uro);
static Generator<_num>* numExpTree(const std::vector<ExpElement<_num>*>& infList,
   std::vector<ExpElement<_num>*>& pntList);
static Generator<_num>* numExpIntegrateUnary(
   const std::vector<ExpElement<_num>*>& elements,
   std::vector<ExpElement<_num>*>& pntList);
static Generator<_num>* numExpTreeMerge(
   const std::vector<ExpElement<_num>*>& elements,
   std::vector<ExpElement<_num>*>& pntList);
static Generator<_num>* numExpTreeMerge2(
   const std::vector<ExpElement<_num>*>& elements);
static _bool isNumExpComputable(const std::vector<ExpElement<_num>*>& infList);
static _bool isNumExpOperator(const _char& ch);
static _bool isNumExpHighPriority(const _char& ch);
void timeVariableMemberException(const Token& tk, uro::Uroboros* uro);

}

#endif /* EXP_PARSE_NUMBER_H */
