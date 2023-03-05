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

#ifndef FUNC_NUMBER_H
#define FUNC_NUMBER_H

#include "func-generic.h"
#include "../../uroboros.h"


namespace uro::func
{

struct F_Absolute : Func_1<_num>, Generator<_num>
{
public:
   F_Absolute(_genptr<_num>& a1) : Func_1(a1) { };
   _num getValue() override;
};


struct F_Ceil : Func_1<_num>, Generator<_num>
{
public:
   F_Ceil(_genptr<_num>& a1) : Func_1(a1) { };
   _num getValue() override;
};


struct F_CountInside : Generator<_num>
{
public:
   F_CountInside(_defptr& def, _lcptr& lctx, FileContext* fctx, _uro& uro)
      : definition(std::move(def)), locContext(std::move(lctx)), fileContext(fctx), uroboros(uro) { };

   _num getValue() override;

private:
   _lcptr locContext;
   FileContext* fileContext;
   _defptr definition;
   _uro& uroboros;
};


struct F_Floor : Func_1<_num>, Generator<_num>
{
public:
   F_Floor(_genptr<_num>& a1) : Func_1(a1) { };
   _num getValue() override;
};


struct F_Length : Func_1<_str>, Generator<_num>
{
public:
   F_Length(_genptr<_str>& a1) : Func_1(a1) { };
   _num getValue() override;
};


struct F_Number : Func_1<_str>, Generator<_num>
{
public:
   F_Number(_genptr<_str>& a1) : Func_1(a1) { };
   _num getValue() override;
};


struct F_Power : Func_2<_num, _num>, Generator<_num>
{
public:
   F_Power(_genptr<_num>& a1, _genptr<_num>& a2) : Func_2(a1, a2) { };
   _num getValue() override;

private:
   _num doublePower(const _ndouble base, const _ndouble exp);
};

struct F_Round : Func_1<_num>, Generator<_num>
{
public:
   F_Round(_genptr<_num>& a1) : Func_1(a1) { };
   _num getValue() override;
};


struct F_Sign : Func_1<_num>, Generator<_num>
{
public:
   F_Sign(_genptr<_num>& a1) : Func_1(a1) { };
   _num getValue() override;
};


struct F_Sqrt : Func_1<_num>, Generator<_num>
{
public:
   F_Sqrt(_genptr<_num>& a1) : Func_1(a1) { };
   _num getValue() override;
};


struct F_Truncate : Func_1<_num>, Generator<_num>
{
public:
   F_Truncate(_genptr<_num>& a1) : Func_1(a1) { };
   _num getValue() override;
};


struct F_Random : Generator<_num>
{
public:
   F_Random(_uro& uro)
      : math(uro.math) { };

   _num getValue() override;

private:
   Math& math;
};


struct F_RandomNumber : Func_1<_num>, Generator<_num>
{
public:
   F_RandomNumber(_genptr<_num>& a1, _uro& uro)
      : Func_1(a1), math(uro.math) { };

   _num getValue() override;

private:
   Math& math;
};


struct F_FromBinary : Func_1<_str>, Generator<_num>
{
public:
   F_FromBinary(_genptr<_str>& a1) : Func_1(a1) { };
   _num getValue() override;
};


struct F_FromHex : Func_1<_str>, Generator<_num>
{
public:
   F_FromHex(_genptr<_str>& a1) : Func_1(a1) { };
   _num getValue() override;
};

}

#endif /* FUNC_NUMBER_H */

