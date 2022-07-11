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

#include "tokens.h"
#include "brackets.h"
#include "exception.h"
#include "lexer.h"


Tokens::Tokens(const std::vector<Token>* li)
{
   list = li;
   start = 0;
   length = li->size();
   end = length - 1;
};

Tokens::Tokens(const std::vector<Token>* li, _int ln)
{
   list = li;
   length = ln;
   start = 0;
   end = length - 1;
};

Tokens::Tokens(const std::vector<Token>* li, _int st, _int ln)
{
   list = li;
   start = st;
   length = ln;
   end = start + length - 1;
};

_int Tokens::getStart() const
{
   return start;
};

_int Tokens::getLength() const
{
   return length;
};

_int Tokens::getEnd() const
{
   return end;
};

_boo Tokens::isEmpty() const
{
   return length == 0;
};

const Token& Tokens::first() const
{
   return (*list)[start];
};

const Token& Tokens::second() const
{
   return (*list)[start + 1];
};

const Token& Tokens::penultimate() const
{
   return (*list)[end - 1];
};

const Token& Tokens::last() const
{
   return (*list)[end];
};

const Token& Tokens::at(const _int index) const
{
   return (*list)[start + index];
};

const Token& Tokens::listAt(const _int index) const
{
   return (*list)[index];
};

void Tokens::trimLeft()
{
   if (length != 0) {
      start++;
      length--;
   }
};

void Tokens::expandLeft()
{
   start--;
   length++;
};

void Tokens::trimRight()
{
   if (length != 0) {
      end--;
      length--;
   }
};

void Tokens::trimBoth()
{
   if (length >= 2) {
      start++;
      end--;
      length -= 2;
   }
};

void Tokens::trimFunction()
{
   if (length >= 3) {
      start += 2;
      end--;
      length -= 3;
   }
};

void Tokens::setRange(_int st, _int ln)
{
   start = st;
   length = ln;
   end = start + length - 1;
}

_boo Tokens::containsSymbol(const _char& ch) const
{
   BracketsInfo bi;

   for (_int i = start; i <= end; i++){
      const Token& t = listAt(i);
      if (t.type != Token::t_Symbol) {
         continue;
      }
      if (t.value.ch == ch && bi.isBracketFree()) {
         return true;
      }
      bi.refresh(t);
   }

   return false;
}

_boo Tokens::containsComparisonSymbol() const
{
   BracketsInfo bi;

   for (_int i = start; i <= end; i++){
      const Token& t = listAt(i);

      if (t.type == Token::t_Symbol && bi.isBracketFree()) {
         switch (t.value.ch) {
            case L'<':
            case L'>':
            case L'!':
            case L'=': {
               return true;
            }
         }
      }

      bi.refresh(t);
   }

   return false;
}

_boo Tokens::containsKeyword(const Keyword& kw) const
{
   BracketsInfo bi;

   for (_int i = start; i <= end; i++){
      const Token& t = listAt(i);
      if (t.type == Token::t_Keyword && t.value.keyword.k == kw && bi.isBracketFree()) {
         return true;
      }
      bi.refresh(t);
   }

   return false;
}

_boo Tokens::containsFilterKeyword() const
{
   BracketsInfo bi;

   for (_int i = start; i <= end; i++){
      const Token& t = listAt(i);
      if (t.isFiltherKeyword() && bi.isBracketFree()) {
         return true;
      }
      bi.refresh(t);
   }

   return false;
}

// independent are:  () () ()
// not independent:  ( () () )
_boo Tokens::hasIndependentBrackets() const
{
   _int lvl = 0;

   for (_int i = start; i <= end; i++) {
      const Token& t = listAt(i);
      if (t.type == Token::t_Symbol){
         switch(t.value.ch) {
            case L'(': {
               lvl++;
               break;
            }
            case L')': {
               lvl--;
               if (lvl == 0 && i != end) {
                  return true;
               }
               break;
            }
         }
      }
   }

   return false;
}

// independent are:  [] [] []
// not independent:  [ [] [] ]
_boo Tokens::hasIndependentSquareBrackets() const
{
   _boo first = false;
   _int lvl = 0;

   for (_int i = start; i <= end; i++) {
      const Token& t = listAt(i);
      if (t.type == Token::t_Symbol){
         switch(t.value.ch) {
            case L'[': {
               lvl++;
               if (first && lvl == 1) {
                  return true;
               }
               break;
            }
            case L']': {
               lvl--;
               if (!first && lvl == 0) {
                  first = true;
               }
               break;
            }
         }
      }
   }

   return false;
}


void Tokens::divideByKeyword(const Keyword& kw, Tokens& left, Tokens& right) const
{
   BracketsInfo bi;

   for (_int i = start; i <= end; i++) {
      const Token& t = listAt(i);

      if (t.isKeyword(kw)) {
         if (bi.isBracketFree()) {
            left.setRange(start, i - start);
            right.setRange(i + 1, length + start - i - 1);
            return;
         }
      }
      else {
         bi.refresh(t);
      }
   }
}

void Tokens::divideBySymbol(const _char& symbol, Tokens& left, Tokens& right) const
{
   BracketsInfo bi;

   for (_int i = start; i <= end; i++) {
      const Token& t = listAt(i);

      if (t.isSymbol(symbol)) {
         if (bi.isBracketFree()) {
            left.setRange(start, i - start);
            right.setRange(i + 1, length + start - i - 1);
            return;
         }
      }
      else {
         bi.refresh(t);
      }
   }
}

void Tokens::splitBySymbol(const _char& symbol, std::vector<Tokens>& result) const
{
   BracketsInfo bi;
   _int sublen = 0;

   for (_int i = start; i <= end; i++) {
      const Token& t = listAt(i);

      if (t.isSymbol(symbol) && bi.isBracketFree()) {
         if (sublen == 0) {
            if (result.empty()) {
               throw SyntaxException(str(L"expression cannot start with a ", charStr(symbol), L" symbol"), t.line);
            }
            else {
               throw SyntaxException(str(L"adjacent ", charStr(symbol), L" symbols"), t.line);
            }
         }

         result.push_back(Tokens(list, i - sublen, sublen));
         sublen = 0;
      }
      else {
         bi.refresh(t);
         sublen++;
      }
   }

   if (sublen == 0) {
      if (!isEmpty()) {
         throw SyntaxException(str(L"expression cannot end with a ", charStr(symbol), L" symbol"), last().line);
      }
   }
   else {
      result.push_back(Tokens(list, 1 + end - sublen, sublen));
   }
}

_int Tokens::countSymbols(const _char& symbol) const
{
   BracketsInfo bi;
   _int count = 0;

   for (_int i = start; i <= end; i++) {
      const Token& t = listAt(i);

      if (t.isSymbol(symbol) && bi.isBracketFree()) {
         count++;
      }
      else {
         bi.refresh(t);
      }
   }

   return count;
}

void Tokens::splitByFiltherKeywords(std::vector<Tokens>& result) const
{
   BracketsInfo bi;
   _int sublen = 0;

   for (_int i = start; i <= end; i++) {
      const Token& t = listAt(i);

      if (t.isFiltherKeyword() && bi.isBracketFree()) {
         if (sublen == 0) {
            const Token& prev = listAt(i - 1);

            throw SyntaxException(str(L"adjacent filter keywords '", *prev.value.keyword.os,
               L"' and '", *t.value.keyword.os, L"'"), t.line);
         }

         result.push_back(Tokens(list, i - sublen - 1, sublen + 1));
         sublen = 0;
      }
      else {
         bi.refresh(t);
         sublen++;
      }
   }

   if (sublen == 0) {
      throw SyntaxException(str(L"expression cannot end with a filter keyword '",
         *last().value.keyword.os, L"'"), last().line);
   }
   else {
      result.push_back(Tokens(list, end - sublen, sublen + 1));
   }
}

void Tokens::divideForTernary(Tokens& condition, Tokens& left, Tokens& right) const
{
   BracketsInfo bi;
   _boo hasPercent = false;
   _boo loop = true;
   _int percentId = -1;
   _int colonId = -1;

   for (_int i = start; loop && i <= end; i++) {
      const Token& t = listAt(i);
      if (t.type == Token::t_Symbol) {
         if (bi.isBracketFree()) {
            if (hasPercent) {
               if (t.value.ch == L':') {
                  loop = false;
                  colonId = i;
               }
            }
            else {
               if (t.value.ch == L'?') {
                  hasPercent = true;
                  percentId = i;
               }
            }
         }
         bi.refresh(t);
      }
   }

   condition.setRange(start, percentId - start);
   left.setRange(percentId + 1, colonId - percentId - 1);
   right.setRange(colonId + 1, length + start - colonId - 1);
}
