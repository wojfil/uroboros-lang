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

#ifndef COM_AGGREGATE_H_INCLUDED
#define COM_AGGREGATE_H_INCLUDED

#include "../datatype/datatype.h"
#include <unordered_map>
#include <memory>


namespace uro
{
   struct _uro;
   struct InnerVariables;
}

namespace uro::comm
{

inline constexpr _uint32 AGGR_NULL =      0b000;
inline constexpr _uint32 AGGR_COPY =      0b001;
inline constexpr _uint32 AGGR_SELECT =    0b010;

// commands Select and Copy are different than others
// they have to be performed on multiple files and directories at once
// what happens when we call them in a loop?
// if we select files one by one, then the result will be selection of the one last file
// this is what Aggregate is for - it stores files and directories
// and performs the command once at the loop exit

struct Aggregate
{
public:
   Aggregate(_uro& uro);
   void set (const _uint32& v);
   void run();

   _set copyPaths;
   std::unordered_map<_str, _set> selectPaths;
   _uint32 failedCopy;
   _uint32 failedSelect;
   _set invalidCopy;
   _set invalidSelect;

private:
   _uro& uroboros;
   InnerVariables& inner;
   _uint32 value;
   inline _bool has(const _uint32& v) const;
};

typedef std::unique_ptr<Aggregate> _aggrptr;

}

#endif /* COM_AGGREGATE_H_INCLUDED */
