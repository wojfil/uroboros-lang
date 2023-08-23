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

#ifndef ATTRIB_H
#define ATTRIB_H

#include "token.h"
#include <memory>


namespace perun2
{

typedef uint64_t     p_aunit;

p_constexpr p_aunit ATTR_NULL =         0b00000000000000000000000000000000;
p_constexpr p_aunit ATTR_ACCESS =       0b00000000000000000000000000000001;
p_constexpr p_aunit ATTR_ARCHIVE =      0b00000000000000000000000000000010;
p_constexpr p_aunit ATTR_CHANGE =       0b00000000000000000000000000000100;
p_constexpr p_aunit ATTR_COMPRESSED =   0b00000000000000000000000000001000;
p_constexpr p_aunit ATTR_CREATION =     0b00000000000000000000000000010000;
p_constexpr p_aunit ATTR_DEPTH =        0b00000000000000000000000000100000;
p_constexpr p_aunit ATTR_DRIVE =        0b00000000000000000000000001000000;
p_constexpr p_aunit ATTR_EMPTY =        0b00000000000000000000000010000000;
p_constexpr p_aunit ATTR_ENCRYPTED =    0b00000000000000000000000100000000;
p_constexpr p_aunit ATTR_EXISTS =       0b00000000000000000000001000000000;
p_constexpr p_aunit ATTR_EXTENSION =    0b00000000000000000000010000000000;
p_constexpr p_aunit ATTR_FULLNAME =     0b00000000000000000000100000000000;
p_constexpr p_aunit ATTR_HIDDEN =       0b00000000000000000001000000000000;
p_constexpr p_aunit ATTR_LIFETIME =     0b00000000000000000010000000000000;
p_constexpr p_aunit ATTR_MODIFICATION = 0b00000000000000000100000000000000;
p_constexpr p_aunit ATTR_NAME =         0b00000000000000001000000000000000;
p_constexpr p_aunit ATTR_PARENT =       0b00000000000000010000000000000000;
p_constexpr p_aunit ATTR_PATH =         0b00000000000000100000000000000000;
p_constexpr p_aunit ATTR_READONLY =     0b00000000000001000000000000000000;
p_constexpr p_aunit ATTR_SIZE =         0b00000000000010000000000000000000;
p_constexpr p_aunit ATTR_SIZE_FILE_ONLY=0b00000000000100000000000000000000;

// certain expression or syntax structure may require multiple file attributes:
// for example - creation time, modification time, size and extension
// they also may repeat
// instead of reading them one by one
// all attributes from an expression are joined together and read at once at the beginning of expression evaluation

struct p_perun2;

struct Attribute
{
public:
   Attribute() = delete;
   Attribute(p_perun2& p2);
   Attribute(const p_aunit val, p_perun2& p2);

   void add(const Token& tk);
   void set(const p_aunit v);
   void setCoreCommandBase();
   void setTimeCommandBase();
   p_bool has(const p_aunit v) const;
   p_bool hasAny() const;
   p_aunit getValue() const;
   
   p_perun2& perun2;

private:
   p_aunit value = ATTR_NULL;
};

typedef std::unique_ptr<Attribute> p_attrptr;

}

#endif /* ATTRIB_H */
