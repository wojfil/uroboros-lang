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

#include "parse-generic.h"
#include "../../brackets.h"
#include "../../lexer.h"
#include "../../hash.h"
#include <cwctype>



_boo isPossibleListElement(const Tokens& tks)
{
   const _size length = tks.getLength();

   if (length <= 2 || !tks.second().isSymbol(L'[')
      || !tks.last().isSymbol(L']')
      || tks.hasIndependentSquareBrackets()) {
      return false;
   }

   if (length == 3) {
      throw SyntaxException(L"empty space between square brackets []",
         tks.last().line);
   }

   if (tks.first().type != Token::t_Word) {
      throw SyntaxException(
         L"square brackets [] can be preceded only by a variable name",
         tks.first().line);
   }

   return true;
}

Generator<_num>* parseListElementIndex(const Tokens& tks)
{
   const _size start = tks.getStart() + 2;
   const _size length = tks.getLength() - 3;
   const Tokens tks2(tks.list, start, length);

   Generator<_num>* num = parseNumber(tks2);
   if (num == nullptr) {
      throw SyntaxException(
         L"content of square brackets [] cannot be resolved to a number",
         tks.first().line);
   }

   return num;
}

_boo isPossibleBinary(const Tokens& tks)
{
   if (!tks.containsSymbol(L'?')) {
      return false;
   }

   BracketsInfo bi;
   const _int start = tks.getStart();
   const _int end = tks.getEnd();
   _int i;

   for (i = start; i <= end; i++){
      const Token& t = tks.listAt(i);

      if (bi.isBracketFree() && t.type == Token::t_Symbol && t.value.c == L'?') {
         break;
      }

      bi.refresh(t);
   }

   if (i == start) {
      throw SyntaxException(L"sign ? is preceded by empty space",
         (*tks.list)[i].line);
   }
   else if (i == end) {
      throw SyntaxException(L"sign ? is followed by empty space",
         (*tks.list)[i].line);
   }

   return true;
}

_boo isPossibleTernary(const Tokens& tks)
{
   if (!tks.containsSymbol(L'?') || !tks.containsSymbol(L':')) {
      return false;
   }

   BracketsInfo bi;
   _boo loop = true;
   const _int start = tks.getStart();
   const _int end = tks.getEnd();
   _int percentId = -1;
   _int colonId = -1;

   for (_int i = start; loop && i <= end; i++){
      const Token& t = tks.listAt(i);

      if (bi.isBracketFree() && t.type == Token::t_Symbol) {
         switch (t.value.c) {
            case L'?': {
               if (percentId == -1) {
                  percentId = i;
                  if (colonId != -1) {
                     loop = false;
                  }
               }
               else {
                  return false;
               }
               break;
            }
            case L':': {
               if (colonId == -1) {
                  colonId = i;
                  if (percentId != -1) {
                     loop = false;
                  }
               }
               break;
            }
         }
      }

      bi.refresh(t);
   }

   if (percentId > colonId) {
      throw SyntaxException(L"signs ? and : appear in reverse order",
         (*tks.list)[percentId].line);
   }
   else if (percentId == start) {
      throw SyntaxException(L"sign ? is preceded by empty space",
         (*tks.list)[percentId].line);
   }
   else if (colonId == end) {
      throw SyntaxException(L"sign : is followed by empty space",
         (*tks.list)[colonId].line);
   }
   else if (percentId + 1 == colonId) {
      throw SyntaxException(L"empty space between signs ? and :",
         (*tks.list)[percentId].line);
   }

   return true;
}


void checkLimitBySize(const Tokens& tks)
{
   if (tks.getLength() == 1) {
      const Token& tk = tks.first();
      if (tk.type == Token::t_Number && tk.originString != L"") {
         throw SyntaxException(L"collection cannot be limited by file size '" + tk.originString +
            L"' in this way. You have to iterate over files in a loop, add their size to a helper variable and provide a loop break condition",
            tk.line);
      }
   }
}


_boo isPossibleListElementMember(const Tokens& tks)
{
   const _size length = tks.getLength();

   if (length <= 3 || !tks.second().isSymbol(L'[')
      || !tks.penultimate().isSymbol(L']')
      || tks.hasIndependentSquareBrackets()) {
      return false;
   }

   if (length == 3) {
      throw SyntaxException(L"empty space between square brackets []",
         tks.last().line);
   }

   const Token& last = tks.last();

   if (last.type != Token::t_TwoWords) {
      return false;
   }

   if (last.value.h1 != HASH_NOTHING) {
      throw SyntaxException(L"square brackets [] should be followed by a time variable member",
         tks.last().line);
   }

   if (tks.first().type != Token::t_Word) {
      throw SyntaxException(
         L"square brackets [] can be preceded only by a variable name",
         tks.first().line);
   }

   return true;
}
