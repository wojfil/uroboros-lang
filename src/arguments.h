#ifndef ARGUMENTS_H_INCLUDED
#define ARGUMENTS_H_INCLUDED

#include "datatype/datatype.h"

namespace uro 
{

enum ArgsParseState {
   aps_Ok = 0,
   aps_PrintInfo,
   aps_Failed
};


struct Arguments
{
public:

   // Uroboros arguments parsed from the command-line
   Arguments(_int* argc, _char** argv[]);

   // Uroboros arguments made from code and location
   Arguments(const _str& loc, const _str& cod);

   // Uroboros arguments made from code, location and flags
   Arguments(const _str& loc, const _str& cod, const _uint32& fls);

   _uint32 getFlags() const;
   _list getArgs() const;
   _str getLocation() const;
   _str getCode() const;
   ArgsParseState getParseState() const;

   // to avoid unnecessary duplication of potentially long code
   // code is public and passed as a reference to the lexer
   _str code;

private:

   _uint32 flags;
   _list args;
   _str location;
   ArgsParseState parseState;
};

}

#endif // ARGUMENTS_H_INCLUDED
