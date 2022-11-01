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

#ifndef EXP_ELEMENT_H
#define EXP_ELEMENT_H

#include "generator.h"
#include "generator\gen-generic.h"
#include "primitives.h"


namespace uro
{

enum ElementType
{
   et_Operator = 0,
   et_Constant = 1,
   et_Generator = 2
};


// element of an expression:  logic (&|^) or numeric (+-*/%)
// this element is either an operator, a constant value or a pointer to a generator
template <typename T>
struct ExpElement
{
public:
   ExpElement<T>() = delete;

   ExpElement<T>(const T& cnst, const _int& li)
      : generator(std::make_unique<gen::Constant<T>>(cnst)),
      constant(cnst), type(ElementType::et_Constant), operator_(0), line(li) { };

   ExpElement<T>(const _char& op, const _int& li)
      : generator(), operator_(op), type(ElementType::et_Operator), constant(T()), line(li) { };

   ExpElement<T>(_genptr<T>& gen, const _int& li)
      : generator(std::move(gen)), type(et_Generator), constant(T()), operator_(0), line(li) { };

   ExpElement<T>(ExpElement<T>& element)
      : generator(element.type == ElementType::et_Operator ? _genptr<T>() : std::move(element.generator)),
      constant(element.constant), type(element.type), operator_(element.operator_), line(element.line) { };

   ExpElement<T>& operator=(ExpElement<T>&&) noexcept { };

   ExpElement<T>(ExpElement<T>&&) noexcept { };

   void reinit(const T& cnst, const _int& li)
   {
      this->type = ElementType::et_Constant;
      this->constant = cnst;
      this->generator = std::make_unique<gen::Constant<T>>(cnst);
   }

   void reinit(_genptr<T>& gen, const _int& li)
   {
      this->type = ElementType::et_Generator;
      this->generator = std::move(gen);
   }

   void reinit(ExpElement<T>& element)
   {
      this->line = element.line;
      this->type = element.type;
      this->constant = element.constant;
      this->operator_ = element.operator_;

      if (element.type != et_Operator) {
         this->generator = std::move(element.generator);
      }
   }

   _char operator_;
   T constant;
   _genptr<T> generator;

   ElementType type;
   _int line;
};

}

#endif /* EXP_ELEMENT_H */

