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

#include "gen-bool-compare.h"
#include "../../uroboros.h"


DefComparison::~DefComparison()
{
   delete value1;
   delete value2;
};

_boo DefinitionsEqual::getValue()
{
   while (true) {
      if (!g_running) {
         value1->reset();
         value2->reset();
         break;
      }

      if (value1->hasNext()) {
         if (value2->hasNext()) {
            if (value1->getValue() != value2->getValue()) {
               value1->reset();
               value2->reset();
               return false;
            }
         }
         else {
            value1->reset();
            return false;
         }
      }
      else {
         if (value2->hasNext()) {
            value2->reset();
            return false;
         }
         else {
            return true;
         }
      }
   }

   return false;
};

_boo DefinitionsNotEqual::getValue()
{
   while (true) {
      if (!g_running) {
         value1->reset();
         value2->reset();
         break;
      }

      if (value1->hasNext()) {
         if (value2->hasNext()) {
            if (value1->getValue() != value2->getValue()) {
               value1->reset();
               value2->reset();
               return true;
            }
         }
         else {
            value1->reset();
            return true;
         }
      }
      else {
         if (value2->hasNext()) {
            value2->reset();
            return true;
         }
         else {
            return false;
         }
      }
   }

   return false;
};

_boo DefinitionsSmaller::getValue()
{
   while (true) {
      if (!g_running) {
         value1->reset();
         value2->reset();
         break;
      }

      if (value1->hasNext()) {
         if (!value2->hasNext()) {
            value1->reset();
            return false; // no more on the right
         }
      }
      else {
         if (value2->hasNext()) {
            value2->reset();
            return true; // no more more on the left
         }
         else {
            return false; // no more in both
         }
      }
   }

   return false;
};

_boo DefinitionsSmallerEquals::getValue()
{
   while (true) {
      if (!g_running) {
         value1->reset();
         value2->reset();
         break;
      }

      if (value1->hasNext()) {
         if (!value2->hasNext()) {
            value1->reset();
            return false; // no more on the right
         }
      }
      else {
         if (value2->hasNext()) {
            value2->reset();
            return true; // no more more on the left
         }
         else {
            return true; // no more in both
         }
      }
   }

   return false;
};

_boo DefinitionsBigger::getValue()
{
   while (true) {
      if (!g_running) {
         value1->reset();
         value2->reset();
         break;
      }

      if (value1->hasNext()) {
         if (!value2->hasNext()) {
            value1->reset();
            return true; // no more on the right
         }
      }
      else {
         if (value2->hasNext()) {
            value2->reset();
            return false; // no more more on the left
         }
         else {
            return false; // no more in both
         }
      }
   }

   return false;
};

_boo DefinitionsBiggerEquals::getValue()
{
   while (true) {
      if (!g_running) {
         value1->reset();
         value2->reset();
         break;
      }

      if (value1->hasNext()) {
         if (!value2->hasNext()) {
            value1->reset();
            return true; // no more on the right
         }
      }
      else {
         if (value2->hasNext()) {
            value2->reset();
            return false; // no more more on the left
         }
         else {
            return true; // no more in both
         }
      }
   }

   return false;
};


DefListComparison::~DefListComparison()
{
   delete definition;
   delete list;
};

_boo DefinitionListEqual::getValue()
{
   const _list v = list->getValue();
   const _size len = v.size();

   if (len == 0) {
      if (definition->hasNext()) {
         definition->reset();
         return false;
      }
      else {
         return true;
      }
   }

   _size i = 0;

   while (definition->hasNext()) {
      if (!g_running) {
         definition->reset();
         break;
      }

      if (definition->getValue() != v[i]) {
         definition->reset();
         return false;
      }

      i++;
      if (i == len) {
         if (definition->hasNext()) {
            definition->reset();
            return false;
         }
         else {
            return true;
         }
      }
   }

   return false;
}

_boo DefinitionListNotEqual::getValue()
{
   const _list v = list->getValue();
   const _size len = v.size();

   if (len == 0) {
      if (definition->hasNext()) {
         definition->reset();
         return true;
      }
      else {
         return false;
      }
   }

   _size i = 0;

   while (definition->hasNext()) {
      if (!g_running) {
         definition->reset();
         break;
      }

      if (definition->getValue() != v[i]) {
         definition->reset();
         return true;
      }

      i++;
      if (i == len) {
         if (definition->hasNext()) {
            definition->reset();
            return true;
         }
         else {
            return false;
         }
      }
   }

   return false;
}

_boo DefinitionListSmaller::getValue()
{
   const _list v = list->getValue();
   const _size len = v.size();

   if (len == 0) {
      return false;
   }

   _size i = 0;

   while (definition->hasNext()) {
      if (!g_running) {
         definition->reset();
         return false;
      }

      i++;
      if (i == len) {
         definition->reset();
         return false;
      }
   }

   return true;
}

_boo DefinitionListSmallerEquals::getValue()
{
   const _list v = list->getValue();
   const _size len = v.size();

   if (len == 0) {
      if (definition->hasNext()) {
         definition->reset();
         return false;
      }
      else {
         return true;
      }
   }

   _size i = 0;

   while (definition->hasNext()) {
      if (!g_running) {
         definition->reset();
         return false;
      }

      i++;
      if (i == len) {
         if (definition->hasNext()) {
            definition->reset();
            return false;
         }
         else {
            return true;
         }
      }
   }

   return true;
}

_boo DefinitionListBigger::getValue()
{
   const _list v = list->getValue();
   const _size len = v.size();

   if (len == 0) {
      if (definition->hasNext()) {
         definition->reset();
         return true;
      }
      else {
         return false;
      }
   }

   _size i = 0;

   while (definition->hasNext()) {
      if (!g_running) {
         definition->reset();
         return false;
      }

      i++;
      if (i == len) {
         if (definition->hasNext()) {
            definition->reset();
            return true;
         }
         else {
            return false;
         }
      }
   }

   return false;
}

_boo DefinitionListBiggerEquals::getValue()
{
   const _list v = list->getValue();
   const _size len = v.size();

   if (len == 0) {
      return true;
   }

   _size i = 0;

   while (definition->hasNext()) {
      if (!g_running) {
         definition->reset();
         return false;
      }

      i++;
      if (i == len) {
         definition->reset();
         return true;
      }
   }

   return false;
}




