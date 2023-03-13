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

#ifndef KEYWORD_H_INCLUDED
#define KEYWORD_H_INCLUDED

#include <unordered_map>
#include "hash.h"


namespace perun2
{

struct _p2;

enum Keyword
{
   kw_null = -1,
      // core commands:
   kw_Copy = 0,
   kw_Create,
   kw_CreateFile,
   kw_CreateDirectory,
   kw_CreateFiles,
   kw_CreateDirectories,
   kw_Delete,
   kw_Drop,
   kw_Hide,
   kw_Lock,
   kw_Move,
   kw_Open,
   kw_Reaccess,
   kw_Rechange,
   kw_Recreate,
   kw_Remodify,
   kw_Rename,
   kw_Select,
   kw_Unhide,
   kw_Unlock,
      // core command flags:
   kw_Force,
   kw_Stack,
      // logic:
   kw_True,
   kw_False,
   kw_And,
   kw_Or,
   kw_Xor,
   kw_Not,
      // others commands:
   kw_Print,
   kw_Run,
   kw_Sleep,
      // expression elements:
   kw_In,
   kw_Like,
      // command structs:
   kw_Else,
   kw_If,
   kw_Inside,
   kw_Times,
   kw_While,
      // filthers:
   kw_Every,
   kw_Final,
   kw_Limit,
   kw_Order,
   kw_Skip,
   kw_Where,
      // rest:
   kw_As,
   kw_By,
   kw_To,
   kw_Extensionless,
   kw_With,
      // order:
   kw_Asc,
   kw_Desc,
   // one-word command:
   kw_Break,
   kw_Continue,
   kw_Exit,
   kw_Error
};


struct KeywordsData
{
public:
   KeywordsData() = delete;
   KeywordsData(_p2& p2);

   const std::unordered_map<_hash, Keyword> KEYWORDS;
};

}

#endif // KEYWORD_H_INCLUDED
