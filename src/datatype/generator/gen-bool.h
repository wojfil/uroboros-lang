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

#ifndef GEN_BOOL_H
#define GEN_BOOL_H

#include "../generator.h"
#include "../datatype.h"
#include "gen-generic.h"
#include "gen-bool-compare.h"
#include <algorithm>


namespace uro::gen
{

struct Not : UnaryOperation<_bool>
{
public:
   Not(Generator<_bool>* val);
   _bool getValue() override;
};


struct And : BinaryOperation<_bool>
{
public:
   And(Generator<_bool>* val1, Generator<_bool>* val2);
   _bool getValue() override;
};


struct Or : BinaryOperation<_bool>
{
public:
   Or(Generator<_bool>* val1, Generator<_bool>* val2);
   _bool getValue() override;
};


struct Xor : BinaryOperation<_bool>
{
public:
   Xor(Generator<_bool>* val1, Generator<_bool>* val2);
   _bool getValue() override;
};


// IN operator straight outta SQL
// right side is variant, so is generated for every call
template <typename T>
struct InList : Generator<_bool>
{
public:
   InList<T> (Generator<T>* val, Generator<std::vector<T>>* li)
      : value(val), list(li) { };

   ~InList<T>()
   {
      delete value;
      delete list;
   };

   _bool getValue() override 
   {
      const std::vector<T> vs = list->getValue();
      const T v = value->getValue();
      const _size len = vs.size();

      for (_size i = 0; i < len; i++) {
         if (vs[i] == v) {
            return true;
         }
      }

      return false;
   };

private:
   Generator<T>* value;
   Generator<std::vector<T>>* list;
};


// an optimized variant of the IN operator
// is parsed if second argument is constant
template <typename T>
struct InConstList : Generator<_bool>
{
public:
   InConstList<T> (Generator<T>* val, const std::vector<T>& li)
      : value(val), list(li)
   {
      std::sort(list.begin(), list.end());
      list.erase(std::unique(list.begin(), list.end()), list.end());
   };

   ~InConstList<T>() 
   {
      delete value;
   };

   _bool getValue() override
   {
      return std::binary_search(list.begin(), list.end(), value->getValue());
   };

private:
   Generator<T>* value;
   std::vector<T> list;
};


// Time works quite differently than other data types
// for example '3 June 2005' equals 'June 2005'
// so let there be special a case struct
struct InConstTimeList : Generator<_bool>
{
public:
   InConstTimeList(Generator<_tim>* val, const _tlist& li);
   ~InConstTimeList();
   _bool getValue() override;

private:
   Generator<_tim>* value;
   const _tlist list;
};

}

#endif /* GEN_BOOL_H */
