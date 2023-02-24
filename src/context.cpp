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

#include "context.h"
#include "uroboros.h"
#include "datatype/generator/gen-time.h"
#include "os.h"

namespace uro
{

   AggregateContext::AggregateContext(_uro& uro)
      : aggregate(uro) { };

   LocationContext::LocationContext()
      : location(std::make_unique<Variable<_str>>(VarType::vt_Special)),
        prevLocation(nullptr) { };

   LocationContext::LocationContext(LocationContext* prev)
      : location(std::make_unique<Variable<_str>>(VarType::vt_Special)),
        prevLocation(prev) { };

   void LocationContext::loadData(const _str& newThis)
   {
      this->location->value = this->prevLocation == nullptr
         ? newThis
         : os_join(this->prevLocation->location->value, newThis);
   }

   IndexContext::IndexContext(_uro& uro)
      : AggregateContext(uro),
        index(std::make_unique<Variable<_num>>(VarType::vt_Special)) { };

   FileContext::FileContext(_uro& uro)
      : IndexContext(uro), attribute(std::make_unique<Attribute>(uro)),
      this_(std::make_unique<Variable<_str>>(VarType::vt_Special)),
      locContext(uro.contexts.getLocationContext())
   {
      this->initVars(uro);
   };

   FileContext::FileContext(_attrptr& attr, _uro& uro)
      : IndexContext(uro), attribute(std::move(attr)),
      this_(std::make_unique<Variable<_str>>(VarType::vt_Special)),
      locContext(uro.contexts.getLocationContext())
   {
      this->initVars(uro);
   };

   void IndexContext::resetIndex()
   {
      this->index->value.value.i = NINT_ZERO;
   }

   void IndexContext::incrementIndex()
   {
      this->index->value.value.i++;
   }

   void FileContext::initVars(_uro& uro)
   {
      this->v_archive = this->insertVar<_bool>(uro.hashes.HASH_VAR_ARCHIVE);
      this->v_compressed = this->insertVar<_bool>(uro.hashes.HASH_VAR_COMPRESSED);
      this->v_empty = this->insertVar<_bool>(uro.hashes.HASH_VAR_EMPTY);
      this->v_encrypted = this->insertVar<_bool>(uro.hashes.HASH_VAR_ENCRYPTED);
      this->v_exists = this->insertVar<_bool>(uro.hashes.HASH_VAR_EXISTS);
      this->v_hidden = this->insertVar<_bool>(uro.hashes.HASH_VAR_HIDDEN);
      this->v_isdirectory = this->insertVar<_bool>(uro.hashes.HASH_VAR_ISDIRECTORY);
      this->v_isfile = this->insertVar<_bool>(uro.hashes.HASH_VAR_ISFILE);
      this->v_readonly = this->insertVar<_bool>(uro.hashes.HASH_VAR_READONLY);
      this->v_access = this->insertVar<_tim>(uro.hashes.HASH_VAR_ACCESS);
      this->v_change = this->insertVar<_tim>(uro.hashes.HASH_VAR_CHANGE);
      this->v_creation = this->insertVar<_tim>(uro.hashes.HASH_VAR_CREATION);
      this->v_modification = this->insertVar<_tim>(uro.hashes.HASH_VAR_MODIFICATION);
      this->v_lifetime = this->insertVar<_per>(uro.hashes.HASH_VAR_LIFETIME);
      this->v_size = this->insertVar<_num>(uro.hashes.HASH_VAR_SIZE);
      this->v_depth = this->insertVar<_num>(uro.hashes.HASH_VAR_DEPTH);
      this->v_drive = this->insertVar<_str>(uro.hashes.HASH_VAR_DRIVE);
      this->v_extension = this->insertVar<_str>(uro.hashes.HASH_VAR_EXTENSION);
      this->v_fullname = this->insertVar<_str>(uro.hashes.HASH_VAR_FULLNAME);
      this->v_name = this->insertVar<_str>(uro.hashes.HASH_VAR_NAME);
      this->v_parent = this->insertVar<_str>(uro.hashes.HASH_VAR_PARENT);
      this->v_path = this->insertVar<_str>(uro.hashes.HASH_VAR_PATH);
   }


   void FileContext::loadData(const _str& newThis)
   {
      this->this_->value = newThis;
      this->loadAttributes();
   }

   void FileContext::reloadData()
   {
      this->loadAttributes();
   }

   void FileContext::loadAttributes()
   {
      if (this->attribute->hasAny()) {
         os_loadAttributes(*this);
      }
   }

   void FileContext::resetDepth()
   {
      this->v_depth->value.setToZero();
   }

   void FileContext::resetIndexAndDepth()
   {
      this->resetIndex();
      this->v_depth->value.setToZero();
   }

   GlobalContext::GlobalContext(_uro& uro)
   {
      this->globalVars.times.insert(std::make_pair(uro.hashes.HASH_VAR_NOW, std::make_unique<gen::v_Now>()));
      this->globalVars.times.insert(std::make_pair(uro.hashes.HASH_VAR_TODAY, std::make_unique<gen::v_Today>()));
      this->globalVars.times.insert(std::make_pair(uro.hashes.HASH_VAR_YESTERDAY, std::make_unique<gen::v_Yesterday>()));
      this->globalVars.times.insert(std::make_pair(uro.hashes.HASH_VAR_TOMORROW, std::make_unique<gen::v_Tomorrow>()));

      this->globalVars.strings.insert(std::make_pair(uro.hashes.HASH_VAR_DESKTOP,
         std::make_unique<Variable<_str>>(VarType::vt_Special, os_desktopPath())));
      this->globalVars.strings.insert(std::make_pair(uro.hashes.HASH_VAR_UROBOROS2,
         std::make_unique<Variable<_str>>(VarType::vt_Special, os_uroborosPath())));
      this->globalVars.strings.insert(std::make_pair(uro.hashes.HASH_VAR_ROOT,
         std::make_unique<Variable<_str>>(VarType::vt_Special, uro.arguments.getLocation())));

      this->globalVars.lists.insert(std::make_pair(uro.hashes.HASH_VAR_ALPHABET,
         std::make_unique<Variable<_list>>(VarType::vt_Special, this->getAlphabet())));
      this->globalVars.lists.insert(std::make_pair(uro.hashes.HASH_VAR_ASCII,
         std::make_unique<Variable<_list>>(VarType::vt_Special, STRINGS_ASCII)));
      this->globalVars.lists.insert(std::make_pair(uro.hashes.HASH_VAR_ARGUMENTS,
         std::make_unique<Variable<_list>>(VarType::vt_Special, uro.arguments.getArgs())));
   };

   _list GlobalContext::getAlphabet() const
   {
      _list a(LETTERS_IN_ENGLISH_ALPHABET);
      for (_size i = 0; i < LETTERS_IN_ENGLISH_ALPHABET; i++) {
         a[i] = CHAR_a + i;
      }
      return a;
   }

   Contexts::Contexts(_uro& uro)
      : GlobalContext(uro), hashes(uro.hashes)
   {
      this->locationContexts.push_back(&this->rootLocation);
      this->locationContexts.back()->location->value = uro.arguments.getLocation();

      this->addOsGen(hashes.HASH_VAR_FILES, gen::OsElement::oe_Files, uro);
      this->addOsGen(hashes.HASH_VAR_RECURSIVEFILES, gen::OsElement::oe_RecursiveFiles, uro);
      this->addOsGen(hashes.HASH_VAR_DIRECTORIES, gen::OsElement::oe_Directories, uro);
      this->addOsGen(hashes.HASH_VAR_RECURSIVEDIRECTORIES, gen::OsElement::oe_RecursiveDirectories, uro);
   };

   _bool Contexts::getVar(const Token& tk, Variable<_bool>*& result, _uro& uro)
   {
      const _hash& var = tk.value.word.h;

      if (var == this->hashes.HASH_VAR_SUCCESS) {
         result = this->success.get();
         return true;
      }

      return findVar(tk, result, uro);
   }

   _bool Contexts::getVar(const Token& tk, Variable<_num>*& result, _uro& uro)
   {
      const _hash& var = tk.value.word.h;

      if (var == this->hashes.HASH_VAR_INDEX) {
         if (this->indexContexts.empty()) {
            throw SyntaxError::undefinedVarValue(tk.getOriginString(uro), tk.line);
            return false;
         }

         IndexContext* last = this->indexContexts.back();
         if (last != nullptr) {
            result = last->index.get();
            return true;
         }
      }

      return findVar(tk, result, uro);
   }

   _bool Contexts::getVar(const Token& tk, Variable<_str>*& result, _uro& uro)
   {
      const _hash& var = tk.value.word.h;

      if (var == this->hashes.HASH_VAR_THIS) {
         if (this->fileContexts.empty()) {
            throw SyntaxError::undefinedVarValue(tk.getOriginString(uro), tk.line);
            return false;
         }

         FileContext* last = this->fileContexts.back();
         if (last != nullptr) {
            result = last->this_.get();
            return true;
         }
      }
      else if (var == this->hashes.HASH_VAR_LOCATION) {
         if (this->locationContexts.empty()) {
            throw SyntaxError::undefinedVarValue(tk.getOriginString(uro), tk.line);
            return false;
         }

         LocationContext* last = this->locationContexts.back();
         if (last != nullptr) {
            result = last->location.get();
            return true;
         }
      }

      return findVar(tk, result, uro);
   }

   _bool Contexts::makeVarRef(const Token& tk, _defptr& result, _uro& uro)
   {
      const _hash& var = tk.value.word.h;
      auto v = this->osGenerators.find(var);

      if (v != this->osGenerators.end()) {
         return v->second.generate(result);
      }

      return false;
   }

   void Contexts::addUserVarsContext(UserVarsContext* ctx)
   {
      this->userVarsContexts.push_back(ctx);
   }

   void Contexts::retreatUserVarsContext()
   {
      this->userVarsContexts.pop_back();
   }

   void Contexts::addAggregateContext(AggregateContext* ctx)
   {
      this->aggregateContexts.push_back(ctx);
   }

   void Contexts::retreatAggregateContext()
   {
      this->aggregateContexts.pop_back();
   }

   void Contexts::addIndexContext(IndexContext* ctx)
   {
      this->aggregateContexts.push_back(ctx);
      this->indexContexts.push_back(ctx);
   }

   void Contexts::retreatIndexContext()
   {
      this->aggregateContexts.pop_back();
      this->indexContexts.pop_back();
   }

   void Contexts::addFileContext(FileContext* ctx)
   {
      this->aggregateContexts.push_back(ctx);
      this->indexContexts.push_back(ctx);
      this->fileContexts.push_back(ctx);
   }

   void Contexts::retreatFileContext()
   {
      this->aggregateContexts.pop_back();
      this->indexContexts.pop_back();
      this->fileContexts.pop_back();
   }

   void Contexts::addLocationContext(LocationContext* ctx)
   {
      this->locationContexts.push_back(ctx);
   }

   void Contexts::retreatLocationContext()
   {
      this->locationContexts.pop_back();
   }



   void Contexts::markAllAttributesToRun()
   {
      for (FileContext*& fc : this->fileContexts) {
         if (fc != nullptr) {
            fc->attribute->markToRun();
         }
      }
   }


   _bool Contexts::nextContextWillBeAggregate()
   {
      return this->aggregateContexts.size() == 1;
   }

   _bool Contexts::hasAggregate()
   {
      return this->aggregateContexts.size() > 1;
   }

   comm::Aggregate* Contexts::getAggregate()
   {
      return &this->aggregateContexts[1]->aggregate;
   }

   LocationContext* Contexts::getLocationContext()
   {
      return this->locationContexts.back();
   }

   AggregateContext* Contexts::getAggregateContext()
   {
      return this->aggregateContexts.back();
   }

   _bool Contexts::hasIterationContext() const
   {
      return !this->fileContexts.empty();
   }

   _bool Contexts::hasFileContext() const
   {
      if (this->fileContexts.empty()) {
         return false;
      }

      return this->fileContexts.back() != nullptr;
   }

   FileContext* Contexts::getFileContext()
   {
      return this->fileContexts.back();
   }

   std::vector<FileContext*>& Contexts::getFileContexts()
   {
      return this->fileContexts;
   }

   _bool Contexts::hasIndexContext() const
   {
      return !this->indexContexts.empty();
   }

   void Contexts::addOsGen(const _hash& hash, const gen::OsElement& element, _uro& uro)
   {
      osGenerators.insert(std::make_pair(hash, gen::DefinitionGenerator(element, uro)));
   }

   void Contexts::makeLocationContext(_lcptr& result)
   {
      result = std::make_unique<LocationContext>(this->locationContexts.back());
   }

   UserVarsContext* Contexts::getUserVarsContext()
   {
      return this->userVarsContexts.back();
   }

   _bool Contexts::varExists(const Token& tk, _uro& uro)
   {
      Variable<_bool>* b;
      Variable<_tim>* t;
      Variable<_per>* p;
      Variable<_str>* s;
      Variable<_num>* n;
      Variable<_tlist>* tl;
      Variable<_nlist>* nl;
      Variable<_list>* l;

      return this->getVar(tk, b, uro)
          || this->getVar(tk, t, uro)
          || this->getVar(tk, p, uro)
          || this->getVar(tk, s, uro)
          || this->getVar(tk, n, uro)
          || this->getVar(tk, tl, uro)
          || this->getVar(tk, nl, uro)
          || this->getVar(tk, l, uro);
   }
}
