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

#ifndef COM_STRUCT_H
#define COM_STRUCT_H

#include "../util.h"
#include "../attribute.h"
#include "../datatype/patterns.h"
#include "../datatype/datatype.h"
#include "../datatype/generator.h"
#include "com-arg.h"


namespace perun2::comm
{


struct CS_RawBlock : Command
{
public:
   CS_RawBlock(std::vector<_comptr>& coms, _ucptr& ctx, _p2& p2)
      : length(coms.size()), context(std::move(ctx)), perun2(p2)
   {
      langutil::transferUniquePtrs(coms, commands);
   };

   void run() override;

private:
   std::vector<_comptr> commands;
   const _size length;
   _p2& perun2;
   _ucptr context;
};


struct CS_Block : Command
{
public:
   CS_Block(std::vector<_comptr>& coms, _acptr& ctx, _p2& p2)
      : length(coms.size()), perun2(p2), context(std::move(ctx))
   {
      langutil::transferUniquePtrs(coms, commands);
   };

   void run() override;

private:
   std::vector<_comptr> commands;
   const _size length;
   _p2& perun2;
   _acptr context;
};


struct CS_Times : Command
{
public:
   CS_Times(_genptr<_num>& ts, _comptr& com, _icptr& ctx, _p2& p2)
      : context(std::move(ctx)), times(std::move(ts)), command(std::move(com)), perun2(p2) { };

   void run() override;

private:
   _genptr<_num> times;
   _comptr command;
   _p2& perun2;
   _icptr context;
};


struct CS_While : Command
{
public:
   CS_While(_genptr<_bool>& cond, _comptr& com, _icptr& ctx, _p2& p2)
      : context(std::move(ctx)), condition(std::move(cond)), command(std::move(com)), perun2(p2)  { };

   void run() override;

private:
   _genptr<_bool> condition;
   _comptr command;
   _p2& perun2;
   _icptr context;
};


struct CS_StringLoop : IterationLoop
{
public:
   CS_StringLoop(_genptr<_str>& str, _comptr& com, _fcptr& ctx, _p2& p2)
      : IterationLoop(com, ctx, p2), string(std::move(str)) { };

   void run() override;

private:
   _genptr<_str> string;
};


struct CS_DefinitionLoop : IterationLoop
{
public:
   CS_DefinitionLoop(_defptr& def, _comptr& com, _fcptr& ctx, _p2& p2)
      : IterationLoop(com, ctx, p2), definition(std::move(def)) { };

   void run() override;

private:
   _defptr definition;
};


// this is a definition loop
// but does not own its context
// the real context is somwhere else and this struct has a dangling pointer to it
struct CS_ContextlessLoop : Command
{
public:
   CS_ContextlessLoop(_defptr& def, _comptr& com, _p2& p2);
   void run() override;

private:
   _defptr definition;
   _comptr command;
   _p2& perun2;
   FileContext* const context;
};


struct CS_ListLoop : IterationLoop
{
public:
   CS_ListLoop(_genptr<_list>& li, _comptr& com, _fcptr& ctx, _p2& p2)
      : IterationLoop(com, ctx, p2), list(std::move(li)) { };

   void run() override;

private:
   _genptr<_list> list;
};


struct CS_Inside : IterationLoop
{
public:
   CS_Inside(_comptr& com, _lcptr& lctx, _fcptr& fctx, _p2& p2)
      : IterationLoop(com, fctx, p2), locContext(std::move(lctx)) { };

protected:
   _lcptr locContext;
};


struct CS_InsideThis : Command
{
public:
   CS_InsideThis(_comptr& com, _lcptr& lctx, FileContext* fctx, _p2& p2)
      : command(std::move(com)), locContext(std::move(lctx)), fileContext(fctx), perun2(p2) { };

   void run() override;

private:
   _comptr command;
   _lcptr locContext;
   FileContext* const fileContext;
   _p2& perun2;
};


struct CS_InsideString : CS_Inside
{
public:
   CS_InsideString(_genptr<_str>& str, _comptr& com, _lcptr& lctx, _fcptr& fctx, _p2& p2)
      : CS_Inside(com, lctx, fctx, p2), string(std::move(str)) { };

   void run() override;

private:
   _genptr<_str> string;
};


struct CS_InsideDefinition : CS_Inside
{
public:
   CS_InsideDefinition(_defptr& def, _comptr& com, _lcptr& lctx, _fcptr& fctx, _p2& p2)
      : CS_Inside(com, lctx, fctx, p2), definition(std::move(def)) { };

   void run() override;

private:
   _defptr definition;
};


// analogous to CS_ContextlessLoop
struct CS_InsideContextless : Command
{
public:
   CS_InsideContextless(_defptr& def, _comptr& com, _lcptr& lctx, _p2& p2);

   void run() override;

private:
   _defptr definition;
   _comptr command;
   _lcptr locContext;
   FileContext* const fileContext;
   _p2& perun2;
};


struct CS_InsideList : CS_Inside
{
public:
   CS_InsideList(_genptr<_list>& li, _comptr& com, _lcptr& lctx, _fcptr& fctx, _p2& p2)
      : CS_Inside(com, lctx, fctx, p2), list(std::move(li)) { };

   void run() override;

private:
   _genptr<_list> list;
};

}

#endif /* COM_STRUCT_H */

