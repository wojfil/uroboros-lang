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

#ifndef PARSE_DEFINITION_H_INCLUDED
#define PARSE_DEFINITION_H_INCLUDED

#include "../../tokens.h"
#include "../../uroboros.h"


namespace uro::parse
{

_bool parseDefinition(_defptr& result, const Tokens& tks, _uro& uro);

static _bool isDefinitionChain(const Tokens& tks, _uro& uro);
static _bool parseDefinitionChain(_defptr& result, const Tokens& tks, _uro& uro);
static _bool parseDefTernary(_defptr& result, const Tokens& tks, _uro& uro);
static _bool parseDefBinary(_defptr& result, const Tokens& tks, _uro& uro);

}

#endif // PARSE_DEFINITION_H_INCLUDED
