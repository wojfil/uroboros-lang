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

#ifndef FUNC_BOOL_H_INCLUDED
#define FUNC_BOOL_H_INCLUDED

#include "func-generic.h"
#include <wctype.h>


namespace perun2::func
{


struct F_AnyInside : Generator<p_bool>
{
public:
   F_AnyInside() = delete;
   F_AnyInside(_defptr& def, _lcptr& lctx, FileContext* fctx)
      : definition(std::move(def)), locContext(std::move(lctx)), fileContext(fctx) { };

   p_bool getValue() override;

private:
   _lcptr locContext;
   FileContext* fileContext;
   _defptr definition;
};


template <typename T>
struct F_ContainsCol : Func_2<std::vector<T>, T>, Generator<p_bool>
{
public:
   F_ContainsCol<T>(_genptr<std::vector<T>>& a1, _genptr<T>& a2)
      : Func_2<std::vector<T>, T>(a1, a2) { };

   p_bool getValue() override {
      const std::vector<T> col = this->arg1->getValue();

      if (col.empty()) {
         return false;
      }

      const T v = this->arg2->getValue();

      switch (col.size()) {
         case 1: {
            return v == col[0];
         }
         case 2: {
            return v == col[0] || v == col[1];
         }
         case 3: {
            return v == col[0] || v == col[1] || v == col[2];
         }
      }

      for (const T& ce : col) {
         if (v == ce) {
            return true;
         }
      }

      return false;
   }
};


struct F_ContainsStr : Func_2<p_str, p_str>, Generator<p_bool>
{
public:
   F_ContainsStr(_genptr<p_str>& a1, _genptr<p_str>& a2)
      : Func_2<p_str, p_str>(a1, a2) { };

   p_bool getValue() override;
};


struct F_ContainsDef : Generator<p_bool>
{
public:
   F_ContainsDef(_defptr& def, _genptr<p_str>& val, p_perun2& p2)
      : definition(std::move(def)), value(std::move(val)), perun2(p2) { };

   p_bool getValue() override;

private:
   p_perun2& perun2;
   _defptr definition;
   _genptr<p_str> value;
};


struct F_IsLower : Func_1<p_str>, Generator<p_bool>
{
public:
   F_IsLower(_genptr<p_str>& a1) : Func_1(a1) { };
   p_bool getValue() override;
};


struct F_IsUpper : Func_1<p_str>, Generator<p_bool>
{
public:
   F_IsUpper(_genptr<p_str>& a1) : Func_1(a1) { };
   p_bool getValue() override;
};


struct F_IsNumber : Func_1<p_str>, Generator<p_bool>
{
public:
   F_IsNumber(_genptr<p_str>& a1) : Func_1(a1) { };
   p_bool getValue() override;
};


p_bool isNumber(const p_str& value);


struct F_StartsWith : Func_2<p_str, p_str>, Generator<p_bool>
{
public:
   F_StartsWith(_genptr<p_str>& a1, _genptr<p_str>& a2) : Func_2(a1, a2) { };
   p_bool getValue() override;
};


struct F_EndsWith : Func_2<p_str, p_str>, Generator<p_bool>
{
public:
   F_EndsWith(_genptr<p_str>& a1, _genptr<p_str>& a2) : Func_2(a1, a2) { };
   p_bool getValue() override;
};


struct F_ExistsInside : Func_1<p_str>, Generator<p_bool>
{
public:
   F_ExistsInside(_genptr<p_str>& a1, LocationContext* lctx, FileContext* fctx)
      : Func_1(a1), locContext(lctx), fileContext(fctx) { };
   p_bool getValue() override;

private:
   LocationContext* locContext;
   FileContext* fileContext;
};


struct F_ExistInside: Func_1<p_list>, Generator<p_bool>
{
public:
   F_ExistInside(_genptr<p_list>& a1, LocationContext* lctx, FileContext* fctx)
      : Func_1(a1), locContext(lctx), fileContext(fctx) { };
   p_bool getValue() override;

private:
   LocationContext* locContext;
   FileContext* fileContext;
};


struct F_StartsWithChar : Generator<p_bool>
{
public:
   F_StartsWithChar(_genptr<p_str>& val, const p_char ch)
      : value(std::move(val)), character(ch) { };

   p_bool getValue() override;

private:
   _genptr<p_str> value;
   const p_char character;
};


struct F_EndsWithChar : Generator<p_bool>
{
public:
   F_EndsWithChar(_genptr<p_str>& val, const p_char ch)
      : value(std::move(val)), character(ch) { };

   p_bool getValue() override;

private:
   _genptr<p_str> value;
   const p_char character;
};


struct F_StartsWithConst : Generator<p_bool>
{
public:
   F_StartsWithConst(_genptr<p_str>& val, const p_str& cnst)
      : value(std::move(val)), constant(cnst), constantLength(cnst.size()) { };

   p_bool getValue() override;

private:
   _genptr<p_str> value;
   const p_str constant;
   const p_size constantLength;
};


struct F_EndsWithConst : Generator<p_bool>
{
public:
   F_EndsWithConst(_genptr<p_str>& val, const p_str& cnst)
      : value(std::move(val)), constant(cnst), constantLength(cnst.size()) { };

   p_bool getValue() override;

private:
   _genptr<p_str> value;
   const p_str constant;
   const p_size constantLength;
};



struct F_FindText : Func_1<p_str>, Generator<p_bool>
{
public:
   F_FindText(_genptr<p_str>& a1,  FileContext* ctx)
      : Func_1(a1), context(ctx) { };
   p_bool getValue() override;

private:
   FileContext* context;
};



struct F_IsLetter : Func_1<p_str>, Generator<p_bool>
{
public:
   F_IsLetter(_genptr<p_str>& a1) : Func_1(a1) { };
   p_bool getValue() override;
};


struct F_IsDigit : Func_1<p_str>, Generator<p_bool>
{
public:
   F_IsDigit(_genptr<p_str>& a1) : Func_1(a1) { };
   p_bool getValue() override;
};


struct F_IsBinary : Func_1<p_str>, Generator<p_bool>
{
public:
   F_IsBinary(_genptr<p_str>& a1) : Func_1(a1) { };
   p_bool getValue() override;
};


struct F_IsHex : Func_1<p_str>, Generator<p_bool>
{
public:
   F_IsHex(_genptr<p_str>& a1) : Func_1(a1) { };
   p_bool getValue() override;
};


struct F_Any : Generator<p_bool>
{
public:
   F_Any() = delete;
   F_Any(_defptr& def) 
      : definition(std::move(def)) { };

   p_bool getValue() override;

private:
   _defptr definition;
};


template <typename T>
struct F_AnyList : Generator<p_bool>
{
public:
   F_AnyList() = delete;
   F_AnyList(_genptr<std::vector<T>>& val) 
      : value(std::move(val)) { };

   p_bool getValue() override
   {
      return this->value->getValue().size() > 0;
   }

private:
   _genptr<std::vector<T>> value;
};


struct F_Exists : Func_1<p_str>, Generator<p_bool>
{
public:
   F_Exists(_genptr<p_str>& a1, p_perun2& perun2);
   p_bool getValue() override;

private:
   LocationContext* context;
};


struct F_Exist: Func_1<p_list>, Generator<p_bool>
{
public:
   F_Exist(_genptr<p_list>& a1, p_perun2& perun2);
   p_bool getValue() override;

private:
   LocationContext* context;
};


}

#endif // FUNC_BOOL_H_INCLUDED
