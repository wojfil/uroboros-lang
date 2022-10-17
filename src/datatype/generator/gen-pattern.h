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

#ifndef GEN_PATTERN_H_INCLUDED
#define GEN_PATTERN_H_INCLUDED

#include "gen-os.h"


namespace uro::gen
{

inline constexpr _uint32 PATTERN_INFO_NULL =             0b0000000;
inline constexpr _uint32 PATTERN_INFO_VALID =            0b0000001;
inline constexpr _uint32 PATTERN_INFO_ONE_ASTERISK =     0b0000010;
inline constexpr _uint32 PATTERN_INFO_DOUBLE_ASTERISK =  0b0000100;
inline constexpr _uint32 PATTERN_INFO_IS_ABSOLUTE =      0b0001000;


struct PatternUnit
{
public:
   PatternUnit(const _str& ast, const _str& suf) : asteriskPart(ast), suffixPart(suf) { };
   PatternUnit(const _str& ast) : asteriskPart(ast), suffixPart(_str()) { };

   const _str asteriskPart;
   const _str suffixPart;
};


struct PatternParser
{
public:
   PatternParser(uro::Uroboros* uro)
      : uroboros(uro), defGenerator(OsElement::oe_None, uro) { };

   _bool parse(const _str& originPattern, _def*& result, const _int& line) const;

private:

   void addUnit(_str& asteriskPart, _str& suffixPart, const _str& part, 
      const _bool& hasAsterisk, std::vector<PatternUnit>& units) const;

   uro::Uroboros* uroboros;
   const DefinitionGenerator defGenerator;
};

}

#endif // GEN_PATTERN_H_INCLUDED
