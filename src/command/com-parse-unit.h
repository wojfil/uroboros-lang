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

#ifndef COM_PARSE_UNIT_H
#define COM_PARSE_UNIT_H

#include "../datatype/primitives.h"
#include "../datatype/generator.h"
#include "com.h"
#include <vector>


struct CS_If;

struct IfParseUnit
{
public:

   IfParseUnit(CS_If* pntr): pointer(pntr),
      closed(false), elseClosed(false), locked(true) { };

   CS_If* pointer;
   _boo closed;
   _boo elseClosed;
   _boo locked;
};


struct IfContext
{
public:
   IfContext() : units(std::vector<IfParseUnit>()) { };

   void addIfParseUnit(CS_If* pntr);
   void retreatIfParseUnit();
   void lockLastIf();
   void setElse(Command* com, const _int& line);
   void setEmptyElse(const _int& line);
   void addElseIf(Generator<_boo>* cond, Command* com, const _int& line);

private:
   std::vector<IfParseUnit> units;

};


#endif /* COM_PARSE_UNIT_H */
