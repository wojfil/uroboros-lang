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

#include <vector>
#include <cstdlib>
#include "uroboros.h"
#include "exception.h"
#include "command/com-parse.h"
#include "tokens.h"
#include "util.h"
#include "brackets.h"
#include "lexer.h"
#include "os.h"
#include "hash.h"
#include "print.h"
#include "datatype/math.h"
#include "var/var-runtime.h"


Uroboros::Uroboros(const Arguments& args) : arguments(args), hashes(Hashes()), vars(Variables(this)),
   vc(VariablesContext(&this->hashes, &this->vars)), math(Math()), flags(args.getFlags()), running(true),
   break_(false), continue_(false), exitCode(EXITCODE_OK), sideProcess(SideProcess()),
   terminator(Terminator(this)), keywordsData(KeywordsData()), literals(Literals()),
   commands(nullptr), tokens(std::vector<Token>()), ifContext(IfContext()),
   patternParser(PatternParser(this)) { };


_boo Uroboros::run()
{
   return this->uro_tokenize()
       && this->uro_parse()
       && this->uro_postParse()
       && this->uro_runCommands();
};

_boo Uroboros::uro_tokenize()
{
   this->exitCode = EXITCODE_OK;

   try {
      this->tokens = tokenize(this->arguments.code, this);
   }
   catch (const SyntaxException& ex) {
      rawPrint(ex.getMessage());
      this->exitCode = EXITCODE_SYNTAX_ERROR;
      return false;
   }
   catch (...) {
      SyntaxException ex2(L"wrong syntax. No command can be formed of this code", 1);
      rawPrint(ex2.getMessage());
      this->exitCode = EXITCODE_SYNTAX_ERROR;
      return false;
   }

   return true;
};


_boo Uroboros::uro_parse()
{
   try {
      Tokens tks(&this->tokens);
      checkBrackets(tks);
      this->commands = parseCommands(tks, this);
   }
   catch (const SyntaxException& ex) {
      rawPrint(ex.getMessage());
      this->exitCode = EXITCODE_SYNTAX_ERROR;
      return false;
   }
   catch (...) {
      SyntaxException ex2(L"wrong syntax. No command can be formed of this code", 1);
      rawPrint(ex2.getMessage());
      this->exitCode = EXITCODE_SYNTAX_ERROR;
      return false;
   }

   return true;
};

_boo Uroboros::uro_postParse()
{
   this->math.init();

   // this is potential direction of optimizations
   // next iteration of syntax analysis after successful parsing of commands
   // 1) variables, that appear only once are incorporated into optimized expressions as constants
   // 2) If statements, that do not contain any Else/Else If structure are transformed into simpler forms
   // 3) loops, that do not feature any attribute are also simplified
   // how to do that? potential solution:
   // every Command has a method: Command* postParse();
   // it returns nullptr by default
   // we can override this method, so the command can return a simplified form of itself if possible

   return true;
};

_boo Uroboros::uro_runCommands()
{
   try {
      this->commands->run();
   }
   catch (const UroRuntimeException& re) {
      rawPrint(re.getMessage());
      delete this->commands;
      this->exitCode = EXITCODE_RUNTIME_ERROR;
      return false;
   }

   delete this->commands;
   return true;
};
