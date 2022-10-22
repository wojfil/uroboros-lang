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

#include "gen-definition.h"
#include "../../os.h"
#include "../../uroboros.h"


namespace uro::gen
{

DefFilter::DefFilter(_def* def, Uroboros& uro)
   : first(true), definition(def), uroboros(uro) { };

DefFilter::~DefFilter()
{
   delete definition;
}

_fdata* DefFilter::getDataPtr()
{
   return this->definition->getDataPtr();
}

void DefFilter::reset() {
   if (!first) {
      first = true;
      definition->reset();
   }
}


void Filter_WhereDef::reset() {
   if (!first) {
      first = true;
      if (!finished) {
         definition->reset();
      }

      if (hasMemory) {
         attrMemory.restore();
      }
   }
}


_bool Filter_WhereDef::hasNext()
{
   if (first) {
      finished = false;
      first = false;
      if (hasMemory) {
         attrMemory.load();
      }
      index.setToZero();
   }

   while (definition->hasNext()) {
      if (this->uroboros.state != State::s_Running) {
         this->reset();
         break;
      }

      value = definition->getValue();

      if (this->hasAttribute) {
         this->attribute->run();
      }

      const _bool con = condition->getValue();

      if (con) {
         this->inner.index.value = index;
         index++;
         return true;
      }
   }

   finished = true;
   reset();
   return false;
}


DefinitionChain::DefinitionChain(_def* def, Uroboros& uro)
   : definition(def), inner(uro.vars.inner) { };


DefinitionChain::~DefinitionChain()
{
   delete this->definition;
};


void DefinitionChain::reset()
{
   if (!this->finished) {
      this->finished = true;
      this->index = 0LL;
      this->definition->reset();
   }
};


_bool DefinitionChain::hasNext()
{
   if (this->definition->hasNext()) {
      this->finished = false;
      this->value = this->definition->getValue();
      this->inner.index.value.value.i = this->index;
      this->index++;
      return true;
   }

   this->finished = true;
   this->index = 0LL;
   return false;
};


LocationVessel::LocationVessel(const _bool& abs, Uroboros& uro)
   : isAbsolute(abs), inner(uro.vars.inner) { };


_str LocationVessel::getValue()
{
   return this->isAbsolute
      ? this->value
      : str(this->inner.location.value, OS_SEPARATOR_STRING, this->value);
};

const _str& LocationVessel::getRawValue() const
{
   return this->value;
};


void LocationVessel::setValue(const _str& val)
{
   this->value = val;
};


NestedDefiniton::NestedDefiniton(LocationVessel* ves, _def* def, _def* locs, const _bool& abs)
   : vessel(ves), definition(def), locations(locs), isAbsolute(abs) { };


NestedDefiniton::~NestedDefiniton()
{
   delete this->definition;
   delete this->locations;
};


void NestedDefiniton::reset()
{
   if (this->defOpened) {
      this->defOpened = false;
      this->definition->reset();
   }
   if (this->locsOpened) {
      this->locsOpened = false;
      this->locations->reset();
   }
};


_bool NestedDefiniton::hasNext()
{
   if (!this->locsOpened) {
      if (this->locations->hasNext()) {
         this->locsOpened = true;
         this->vessel->setValue(this->locations->getValue());
      }
      else {
         return false;
      }
   }

   while (true) {
      if (this->definition->hasNext()) {
         this->defOpened = true;
         this->value = this->isAbsolute
            ? this->definition->getValue()
            : str(this->vessel->getRawValue(), OS_SEPARATOR_STRING, this->definition->getValue());
         return true;
      }
      else {
         this->defOpened = false;
         if (this->locations->hasNext()) {
            this->locsOpened = true;
            this->vessel->setValue(this->locations->getValue());
         }
         else {
            this->locsOpened = false;
            return false;
         }
      }
   }
   
   return false;
};


_bool Filter_LimitDef::hasNext()
{
   if (first) {
      limit = number->getValue().toInt();
      if (limit <= 0LL) {
         return false;
      }

      counter = 0LL;
      first = false;
   }

   while (definition->hasNext()) {
      if (this->uroboros.state != State::s_Running || counter >= limit) {
         definition->reset();
         break;
      }
      value = definition->getValue();
      counter++;
      return true;
   }

   first = true;
   return false;
};


_bool Filter_SkipDef::hasNext()
{
   if (first) {
      limit = number->getValue().toInt();
      if (limit < 0LL) {
         limit = 0LL;
      }

      counter = 0LL;
      first = false;
   }

   while (definition->hasNext()) {
      if (this->uroboros.state != State::s_Running) {
         definition->reset();
      }

      if (counter == limit) {
         this->inner.index.value -= limit;
         value = definition->getValue();
         return true;
      }
      else {
         counter++;
      }
   }

   first = true;
   return false;
};


_bool Filter_EveryDef::hasNext()
{
   if (first) {
      limit = number->getValue().toInt();
      if (limit < 1LL) {
         limit = 1LL;
      }

      counter = limit;
      first = false;
      index.setToZero();
   }

   while (definition->hasNext()) {
      if (this->uroboros.state != State::s_Running) {
         definition->reset();
      }

      if (counter == limit) {
         counter = 1LL;
         value = definition->getValue();
         this->inner.index.value = index;
         index++;
         return true;
      }
      else {
         counter++;
      }
   }

   first = true;
   return false;
};


_bool Filter_FinalDef::hasNext()
{
   if (first) {
      const _nint limit = number->getValue().toInt();
      if (limit < 1LL) {
         return false;
      }

      values.clear();
      length = 0LL;
      
      while (definition->hasNext()) {
         if (this->uroboros.state != State::s_Running) {
            definition->reset();
            return false;
         }

         if (length == limit) {
            values.pop_front();
         }
         else {
            length++;
         }
         values.emplace_back(definition->getValue());
      }

      index = 0LL;
      first = false;
   }

   if (this->uroboros.state != State::s_Running) {
      first = true;
      return false;
   }

   if (index < length) {
      value = values[static_cast<_size>(index)];
      this->inner.index.value.value.i = index;
      index++;
      return true;
   }

   first = true;
   return false;
};



Join_DefStr::~Join_DefStr()
{
   delete left;
   delete right;
}

void Join_DefStr::reset()
{
   if (taken) {
      taken = false;
   }
   else {
      left->reset();
   }
}

_bool Join_DefStr::hasNext()
{
   if (this->uroboros.state != State::s_Running) {
      reset();
      return false;
   }

   if (taken) {
      taken = false;
      return false;
   }

   if (left->hasNext()) {
      value = left->getValue();
      return true;
   }

   value = right->getValue();
   taken = true;
   return true;
}


Join_StrDef::~Join_StrDef()
{
   delete left;
   delete right;
}


void Join_StrDef::reset()
{
   if (!first) {
      first = true;
      right->reset();
   }
}


_bool Join_StrDef::hasNext()
{
   if (this->uroboros.state != State::s_Running) {
      reset();
      return false;
   }

   if (first) {
      value = left->getValue();
      first = false;
      return true;
   }

   if (right->hasNext()) {
      value = right->getValue();
      return true;
   }

   first = true;
   return false;
}


Join_DefList::~Join_DefList()
{
   delete left;
   delete right;
}


void Join_DefList::reset()
{
   if (taken) {
      taken = false;
   }
   else {
      left->reset();
   }
}


_bool Join_DefList::hasNext()
{
   if (this->uroboros.state != State::s_Running) {
      reset();
      return false;
   }

   if (!taken) {
      if (left->hasNext()) {
         value = left->getValue();
         return true;
      }
      else {
         values = right->getValue();
         length = values.size();
         index = 0;
         taken = true;
      }
   }

   if (index != length) {
      value = values[index];
      index++;
      return true;
   }
   else {
      taken = false;
      return false;
   }
}


Join_ListDef::~Join_ListDef()
{
   delete left;
   delete right;
}


void Join_ListDef::reset()
{
   if (taken) {
      right->reset();
      taken = false;
   }
   first = true;
}


_bool Join_ListDef::hasNext()
{
   if (this->uroboros.state != State::s_Running) {
      reset();
      return false;
   }

   if (first) {
      values = left->getValue();
      length = values.size();
      index = 0;
      first = false;
   }

   if (!taken) {
      if (index == length) {
         taken = true;
      }
      else {
         value = values[index];
         index++;
         return true;
      }
   }

   if (right->hasNext()) {
      value = right->getValue();
      return true;
   }
   else {
      first = true;
      taken = false;
      return false;
   }
}


Join_DefDef::~Join_DefDef()
{
   delete left;
   delete right;
}


void Join_DefDef::reset()
{
   if (!first) {
      if (taken) {
         right->reset();
         taken = false;
      }
      else {
         left->reset();
      }
      first = true;
   }
}


_bool Join_DefDef::hasNext()
{
   if (this->uroboros.state != State::s_Running) {
      reset();
      return false;
   }

   if (first) {
      first = false;
   }

   if (!taken) {
      if (left->hasNext()) {
         value = left->getValue();
         return true;
      }
      else {
         taken = true;
      }
   }

   if (taken) {
      if (right->hasNext()) {
         value = right->getValue();
         return true;
      }
      else {
         first = true;
         taken = false;
         return false;
      }
   }
}


DefinitionSuffix::DefinitionSuffix(_def* def, Uroboros& uro, const _str& suf, const _bool& abs, const _bool& dir)
   : uroboros(uro), inner(uro.vars.inner), definition(def), suffix(suf), absoluteBase(abs), isDirectory(dir) { };


DefinitionSuffix::~DefinitionSuffix()
{
   delete this->definition;
}


void DefinitionSuffix::reset()
{
   if (!this->first) {
      this->definition->reset();
      this->first = true;
   }
}


_bool DefinitionSuffix::hasNext()
{
   if (this->first) {
      this->first = false;
      this->index.setToZero();
   }

   while (definition->hasNext()) {
      if (this->uroboros.state != State::s_Running) {
         this->definition->reset();
         this->first = true;
         break;
      }

      this->value = str(this->definition->getValue(), this->suffix);
      const _str path = this->absoluteBase 
         ? this->value 
         : str(this->inner.location.value, OS_SEPARATOR_STRING, this->value);

      if (this->isDirectory ? os_directoryExists(path): os_exists(path)) {
         this->inner.index.value = index;
         index++;
         return true;
      }
   }

   this->first = true;
   return false;
}


void DefTernary::reset()
{
   if (!first) {
      first = true;
      if (isLeft) {
         left->reset();
      }
      else {
         right->reset();
      }
   }
}


_bool DefTernary::hasNext()
{
   if (first) {
      isLeft = condition->getValue();
   }

   if (isLeft) {
      if (left->hasNext()) {
         value = left->getValue();
         return true;
      }
   }
   else {
      if (right->hasNext()) {
         value = right->getValue();
         return true;
      }
   }

   first = true;
   return false;
}


void DefBinary::reset()
{
   if (!first) {
      first = true;
      left->reset();
   }
}


_bool DefBinary::hasNext()
{
   if (first) {
      if (!condition->getValue()) {
         return false;
      }
      first = false;
   }

   if (left->hasNext()) {
      value = left->getValue();
      return true;
   }

   first = true;
   return false;
}

}
