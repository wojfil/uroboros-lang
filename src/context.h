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

#ifndef CONTEXT_H_INCLUDED
#define CONTEXT_H_INCLUDED

#include "datatype/datatype.h"
#include "command/com-aggregate.h"
#include "datatype/generator/gen-os-gen.h"
#include "var.h"
#include "attribute.h"
#include <unordered_map>
#include <memory>
#include <iostream>


namespace uro
{
   struct _uro;
   struct Hashes;

   template <typename T>
   using _varptr = std::unique_ptr<Variable<T>>;

   template <typename T>
   using _varptrs = std::unordered_map<_size, _varptr<T>>;


   struct VarsContext
   {
   public:

      void takeVarsPtr(_varptrs<_bool>*& result) { result = &this->bools; };
      void takeVarsPtr(_varptrs<_tim>*& result) { result = &this->times; };
      void takeVarsPtr(_varptrs<_per>*& result) { result = &this->periods; };
      void takeVarsPtr(_varptrs<_str>*& result) { result = &this->strings; };
      void takeVarsPtr(_varptrs<_num>*& result) { result = &this->numbers; };
      void takeVarsPtr(_varptrs<_tlist>*& result) { result = &this->timeLists; };
      void takeVarsPtr(_varptrs<_nlist>*& result) { result = &this->numLists; };
      void takeVarsPtr(_varptrs<_list>*& result) { result = &this->lists; };

      template <typename T>
      _bool takeVar(const _size& var, Variable<T>*& result)
      {
         _varptrs<T>* vars;
         this->takeVarsPtr(vars);
         auto v = vars->find(var);
         if (v != vars->end()) {
            result = v->second.get();
            return true;
         }

         return false;
      }

      template <typename T>
      Variable<T>* insertVar(const _size& var, const VarType& type)
      {
         _varptrs<T>* vars;
         this->takeVarsPtr(vars);
         auto a = vars->insert(std::make_pair(var, std::make_unique<Variable<T>>(type)));
         return a.first->second.get();
      }

      _varptrs<_bool> bools;
      _varptrs<_tim> times;
      _varptrs<_per> periods;
      _varptrs<_str> strings;
      _varptrs<_num> numbers;
      _varptrs<_tlist> timeLists;
      _varptrs<_nlist> numLists;
      _varptrs<_list> lists;
   };

   struct UserVarsContext
   {
      VarsContext userVars;
   };

   struct AggregateContext
   {
      AggregateContext() = delete;
      AggregateContext(_uro& uro);
      void runAggregate() { this->aggregate.run(); }

      comm::Aggregate aggregate;
   };

   struct IndexContext : AggregateContext
   {
   public:
      IndexContext() = delete;
      IndexContext(_uro& uro);
      void resetIndex();
      void incrementIndex();

      _varptr<_num> index;
   };

   struct FileContext : IndexContext
   {
   public:
      FileContext() = delete;
      FileContext(_uro& uro);
      FileContext(_attrptr& attr, _uro& uro);

      virtual void loadData(const _str& newThis);
      void reloadData();
      void loadAttributes();
      void resetDepth();
      void resetIndexAndDepth();

      _varptr<_str> this_;
      _attrptr attribute;
      VarsContext fileVars;
      LocationContext* const locContext;
      _str trimmed;

      Variable<_bool>* v_archive;
      Variable<_bool>* v_compressed;
      Variable<_bool>* v_empty;
      Variable<_bool>* v_encrypted;
      Variable<_bool>* v_exists;
      Variable<_bool>* v_hidden;
      Variable<_bool>* v_isdirectory;
      Variable<_bool>* v_isfile;
      Variable<_bool>* v_readonly;
      Variable<_tim>* v_access;
      Variable<_tim>* v_change;
      Variable<_tim>* v_creation;
      Variable<_tim>* v_modification;
      Variable<_per>* v_lifetime;
      Variable<_num>* v_size;
      Variable<_num>* v_depth;
      Variable<_str>* v_drive;
      Variable<_str>* v_extension;
      Variable<_str>* v_fullname;
      Variable<_str>* v_name;
      Variable<_str>* v_parent;
      Variable<_str>* v_path;

   private:
      void initVars(_uro& uro);

      template <typename T>
      Variable<T>* insertVar(const _size& hsh)
      {
         return this->fileVars.insertVar<T>(hsh, VarType::vt_Attribute);
      }
   };

   struct LocationContext
   {
   public:
      LocationContext();
      LocationContext(LocationContext* prev);
      void loadData(const _str& newThis);

      _varptr<_str> location;

   private:
      LocationContext* prevLocation;
   };

   struct GlobalContext
   {
   public:
      GlobalContext() = delete;
      GlobalContext(_uro& uro);

      VarsContext globalVars;

   private:
      template <typename T>
      Variable<T>* insertVar(const _size& hsh)
      {
         return this->globalVars.insertVar<T>(hsh, VarType::vt_Special);
      }

      _list getAlphabet() const;
   };

   typedef std::unique_ptr<UserVarsContext>        _ucptr;
   typedef std::unique_ptr<AggregateContext>       _acptr;
   typedef std::unique_ptr<IndexContext>           _icptr;
   typedef std::unique_ptr<FileContext>            _fcptr;
   typedef std::unique_ptr<LocationContext>        _lcptr;


   struct Contexts : GlobalContext
   {
   public:
      Contexts() = delete;
      Contexts(_uro& uro);

      _bool getVar(const Token& tk, Variable<_bool>*& result, _uro& uro);
      _bool getVar(const Token& tk, Variable<_num>*& result, _uro& uro);
      _bool getVar(const Token& tk, Variable<_str>*& result, _uro& uro);

      template <typename T>
      _bool getVar(const Token& tk, Variable<T>*& result, _uro& uro)
      {
         return findVar(tk, result, uro);
      };

      _bool makeVarRef(const Token& tk, _defptr& result, _uro& uro);

      template <typename T>
      _bool makeVarRef(const Token& tk, _genptr<T>& result, _uro& uro)
      {
         Variable<T>* var;
         if (getVar(tk, var, uro)) {
            result = std::make_unique<VariableReference<T>>(var);
            return true;
         }

         return false;
      };


      // context of user variables - every structure of { } brackets
      void addUserVarsContext(UserVarsContext* ctx);
      void retreatUserVarsContext();

      // context of a raw block of commands: { print 2+2 }
      void addAggregateContext(AggregateContext* ctx);
      void retreatAggregateContext();

      // context of a repeat loop: 10 times { print 2+2 }
      void addIndexContext(IndexContext* ctx);
      void retreatIndexContext();

      // context of iteration over file system elements or strings: files where extension = 'pdf'
      void addFileContext(FileContext* ctx);
      void retreatFileContext();

      // working location context
      void addLocationContext(LocationContext* ctx);
      void retreatLocationContext();

      void markAllAttributesToRun();

      // for aggregates
      _bool nextContextWillBeAggregate();
      _bool hasAggregate();
      comm::Aggregate* getAggregate();

      LocationContext* getLocationContext();
      AggregateContext* getAggregateContext();
      _bool hasIterationContext() const;
      _bool hasFileContext() const;
      FileContext* getFileContext();
      std::vector<FileContext*>& getFileContexts();
      _bool hasIndexContext() const;
      void makeLocationContext(_lcptr& result);
      UserVarsContext* getUserVarsContext();

      _varptr<_bool> success;

   private:
      const Hashes& hashes;

      template <typename T>
      _bool findVar(const Token& tk, Variable<T>*& result, _uro& uro)
      {
         const _size& var = tk.value.word.h;

         // look in global variables
         if (this->globalVars.takeVar(var, result)) {
            return true;
         }

         // look in user variables
         for (UserVarsContext* uvc : this->userVarsContexts) {
            if (uvc->userVars.takeVar(var, result)) {
               return true;
            }
         }

         // look in file attributes
         auto fw = this->hashes.HASH_GROUP_ATTR.find(var);

         if (fw != this->hashes.HASH_GROUP_ATTR.end()) {
            if (this->fileContexts.empty()) {
               throw SyntaxError::undefinedVarValue(tk.getOriginString(uro), tk.line);
            }

            FileContext* lastFileCtx = this->fileContexts.back();
            if (lastFileCtx->fileVars.takeVar(var, result)) {
               lastFileCtx->attribute->add(var);
               return true;
            }
         }

         return false;
      }

      void addOsGen(const _size& hash, const gen::OsElement& element, _uro& uro);

      LocationContext rootLocation;
      std::unordered_map<_size, gen::DefinitionGenerator> osGenerators;

      std::vector<UserVarsContext*> userVarsContexts;
      std::vector<AggregateContext*> aggregateContexts;
      std::vector<LocationContext*> locationContexts;
      std::vector<IndexContext*> indexContexts;
      std::vector<FileContext*> fileContexts;
   };
}

#endif // CONTEXT_H_INCLUDED
