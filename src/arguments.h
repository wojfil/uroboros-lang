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

#ifndef ARGUMENTS_H_INCLUDED
#define ARGUMENTS_H_INCLUDED

#include "datatype/datatype.h"

namespace uro
{

inline constexpr _uint32 FLAG_NULL =      0b000;
inline constexpr _uint32 FLAG_NOOMIT =    0b001;
inline constexpr _uint32 FLAG_SILENT =    0b010;
inline constexpr _uint32 FLAG_GUI =       0b100;

inline constexpr _char CHAR_FLAG_NOOMIT =           LETTER_n;
inline constexpr _char CHAR_FLAG_SILENT =           LETTER_s;
inline constexpr _char CHAR_FLAG_GUI =              LETTER_g;
inline constexpr _char CHAR_FLAG_NOOMIT_UPPER =     LETTER_N;
inline constexpr _char CHAR_FLAG_SILENT_UPPER =     LETTER_S;
inline constexpr _char CHAR_FLAG_GUI_UPPER =        LETTER_G;


enum ArgsParseState 
{
   aps_Ok = 0,
   aps_PrintInfo,
   aps_Failed
};

struct Arguments
{
public:

   // Uroboros arguments parsed from the command-line
   Arguments(const _int& argc, _char** const argv[]);

   // Uroboros arguments made from code and location
   Arguments(const _str& loc, const _str& cod);

   // Uroboros arguments made from code, location and flags
   Arguments(const _str& loc, const _str& cod, const _uint32& fls);

   _uint32 getFlags() const;
   _list getArgs() const;
   _str getLocation() const;
   const _str& getCode() const;
   ArgsParseState getParseState() const;

private:

   _str code;
   _uint32 flags = FLAG_NULL;
   _list args;
   _str location;
   ArgsParseState parseState = ArgsParseState::aps_Failed;
};

}

#endif // ARGUMENTS_H_INCLUDED
