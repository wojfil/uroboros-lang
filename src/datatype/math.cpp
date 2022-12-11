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

#include "math.h"
#include "numbers.h"


namespace uro
{

Math::Math() {
   anyRandomDouble = false;
}

void Math::init() {
   std::random_device rd;
   generator = std::mt19937(rd());

   if (anyRandomDouble) {
      doubleDist = std::uniform_real_distribution<>(NDOUBLE_ZERO, NDOUBLE_ONE);
   }
}

_ndouble Math::randomDouble() {
   return doubleDist(generator);
}

_nint Math::randomInt(const _nint& limit) {
   std::uniform_int_distribution<_nint> distribution(NINT_ZERO, limit);
   return distribution(generator);
}

void Math::setAnyRandomDouble() {
   anyRandomDouble = true;
}

}
