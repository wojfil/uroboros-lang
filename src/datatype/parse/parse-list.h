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

#ifndef PARSE_LIST_H
#define PARSE_LIST_H

#include "../parse-gen.h"

Generator<_list>* parseList(const Tokens& tks);

static Generator<_list>* parseListBinary(const Tokens& tks);
static Generator<_list>* parseListTernary(const Tokens& tks);
static Generator<_list>* parseListed(const Tokens& tks);
Generator<_list>* parseListConst(const Tokens& tks);
static Generator<_list>* parseListedStrings(const std::vector<Tokens>& elements);
static Generator<_list>* parseListedLists(const std::vector<Tokens>& elements);
static Generator<_list>* parseListFilter(const Tokens& tks);


#endif /* PARSE_LIST_H */

