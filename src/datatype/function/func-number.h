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

#ifndef FUNC_NUMBER_H
#define FUNC_NUMBER_H

#include "func-generic.h"
#include "../../perun2.h"


namespace perun2::func
{

struct F_Absolute : Func_1<p_num>, Generator<p_num>
{
public:
   F_Absolute(_genptr<p_num>& a1) : Func_1(a1) { };
   p_num getValue() override;
};


struct F_Ceil : Func_1<p_num>, Generator<p_num>
{
public:
   F_Ceil(_genptr<p_num>& a1) : Func_1(a1) { };
   p_num getValue() override;
};


struct F_CountInside : Generator<p_num>
{
public:
   F_CountInside(p_defptr& def, _lcptr& lctx, FileContext* fctx, p_perun2& p2)
      : definition(std::move(def)), locContext(std::move(lctx)), fileContext(fctx), perun2(p2) { };

   p_num getValue() override;

private:
   _lcptr locContext;
   FileContext* fileContext;
   p_defptr definition;
   p_perun2& perun2;
};


struct F_Floor : Func_1<p_num>, Generator<p_num>
{
public:
   F_Floor(_genptr<p_num>& a1) : Func_1(a1) { };
   p_num getValue() override;
};


struct F_Length : Func_1<p_str>, Generator<p_num>
{
public:
   F_Length(_genptr<p_str>& a1) : Func_1(a1) { };
   p_num getValue() override;
};


struct F_Number : Func_1<p_str>, Generator<p_num>
{
public:
   F_Number(_genptr<p_str>& a1) : Func_1(a1) { };
   p_num getValue() override;
};


struct F_Power : Func_2<p_num, p_num>, Generator<p_num>
{
public:
   F_Power(_genptr<p_num>& a1, _genptr<p_num>& a2) : Func_2(a1, a2) { };
   p_num getValue() override;

private:
   p_num doublePower(const p_ndouble base, const p_ndouble exp);
};

struct F_Round : Func_1<p_num>, Generator<p_num>
{
public:
   F_Round(_genptr<p_num>& a1) : Func_1(a1) { };
   p_num getValue() override;
};


struct F_Sign : Func_1<p_num>, Generator<p_num>
{
public:
   F_Sign(_genptr<p_num>& a1) : Func_1(a1) { };
   p_num getValue() override;
};


struct F_Sqrt : Func_1<p_num>, Generator<p_num>
{
public:
   F_Sqrt(_genptr<p_num>& a1) : Func_1(a1) { };
   p_num getValue() override;
};


struct F_Truncate : Func_1<p_num>, Generator<p_num>
{
public:
   F_Truncate(_genptr<p_num>& a1) : Func_1(a1) { };
   p_num getValue() override;
};


struct F_Random : Generator<p_num>
{
public:
   F_Random(p_perun2& p2)
      : math(p2.math) { };

   p_num getValue() override;

private:
   Math& math;
};


struct F_RandomNumber : Func_1<p_num>, Generator<p_num>
{
public:
   F_RandomNumber(_genptr<p_num>& a1, p_perun2& p2)
      : Func_1(a1), math(p2.math) { };

   p_num getValue() override;

private:
   Math& math;
};


struct F_FromBinary : Func_1<p_str>, Generator<p_num>
{
public:
   F_FromBinary(_genptr<p_str>& a1) : Func_1(a1) { };
   p_num getValue() override;
};


struct F_FromHex : Func_1<p_str>, Generator<p_num>
{
public:
   F_FromHex(_genptr<p_str>& a1) : Func_1(a1) { };
   p_num getValue() override;
};


struct F_Count : Generator<p_num>
{
public:
   F_Count() = delete;
   F_Count(p_defptr& def, p_perun2& p2) 
      : definition(std::move(def)), perun2(p2) { };

   p_num getValue() override;

private:
   p_defptr definition;
   p_perun2& perun2;
};


template <typename T>
struct F_CountList : Generator<p_num>
{
public:
   F_CountList() = delete;
   F_CountList(_genptr<std::vector<T>>& val) 
      : value(std::move(val)) { };

   p_num getValue() override
   {
      return static_cast<p_nint>(this->value->getValue().size());
   }

private:
   _genptr<std::vector<T>> value;
};


struct F_ShiftMonth_Time : Func_2<p_tim, p_num>, Generator<p_num>
{
public:
   F_ShiftMonth_Time(_genptr<p_tim>& a1, _genptr<p_num>& a2) : Func_2(a1, a2) { };
   p_num getValue() override;
};


struct F_ShiftMonth_Number : Func_2<p_num, p_num>, Generator<p_num>
{
public:
   F_ShiftMonth_Number(_genptr<p_num>& a1, _genptr<p_num>& a2) : Func_2(a1, a2) { };
   p_num getValue() override;
};


struct F_ShiftWeekDay_Time : Func_2<p_tim, p_num>, Generator<p_num>
{
public:
   F_ShiftWeekDay_Time(_genptr<p_tim>& a1, _genptr<p_num>& a2) : Func_2(a1, a2) { };
   p_num getValue() override;
};


struct F_ShiftWeekDay_Number : Func_2<p_num, p_num>, Generator<p_num>
{
public:
   F_ShiftWeekDay_Number(_genptr<p_num>& a1, _genptr<p_num>& a2) : Func_2(a1, a2) { };
   p_num getValue() override;
};


}

#endif /* FUNC_NUMBER_H */
