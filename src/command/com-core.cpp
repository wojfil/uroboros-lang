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

#include "com-core.h"
#include "../perun2.h"
#include "../os.h"
#include "../datatype/patterns.h"


namespace perun2::comm
{



////
//   simple commands:
////

void C_Delete::run()
{
   P_CHECK_IF_PERUN2_IS_RUNNING;

   const p_bool s = this->context->v_exists->value && os_delete(this->context->v_path->value);
   this->perun2.contexts.success->value = s;

   if (s) {
      this->perun2.logger.log(L"Delete ", getCCName(this->context->v_path->value));
      this->context->reloadData();
   }
   else {
      this->perun2.logger.log(L"Failed to delete ", getCCName(this->context->v_path->value));
   }
}

void C_Drop::run()
{
   P_CHECK_IF_PERUN2_IS_RUNNING;

   const p_bool s = this->context->v_exists->value && os_drop(this->context->v_path->value, this->context->v_isfile->value, this->perun2);
   this->perun2.contexts.success->value = s;

   if (s) {
      this->perun2.logger.log(L"Drop ", getCCName(this->context->v_path->value));
      if (saveChanges) {
         this->context->reloadData();
      }
   }
   else {
      this->perun2.logger.log(L"Failed to drop ", getCCName(this->context->v_path->value));
   }
}

void C_Hide::run()
{
   P_CHECK_IF_PERUN2_IS_RUNNING;

   const p_bool s = this->context->v_exists->value && os_hide(this->context->v_path->value);
   this->perun2.contexts.success->value = s;

   if (s) {
      this->perun2.logger.log(L"Hide ", getCCName(this->context->v_path->value));
      if (saveChanges) {
         this->context->v_hidden->value = true;
      }
   }
   else {
      this->perun2.logger.log(L"Failed to hide ", getCCName(this->context->v_path->value));
   }
}

void C_Lock::run()
{
   P_CHECK_IF_PERUN2_IS_RUNNING;

   const p_bool s = this->context->v_exists->value && os_lock(this->context->v_path->value);
   this->perun2.contexts.success->value = s;

   if (s) {
      this->perun2.logger.log(L"Lock ", getCCName(this->context->v_path->value));
      if (saveChanges) {
         this->context->v_readonly->value = true;
      }
   }
   else {
      this->perun2.logger.log(L"Failed to lock ", getCCName(this->context->v_path->value));
   }
}

void C_Open::run()
{
   P_CHECK_IF_PERUN2_IS_RUNNING;

   const p_bool s = this->context->v_exists->value && os_open(this->context->v_path->value);
   this->perun2.contexts.success->value = s;

   if (s) {
      this->perun2.logger.log(L"Open ", getCCName(this->context->v_path->value));
   }
   else {
      this->perun2.logger.log(L"Failed to open ", getCCName(this->context->v_path->value));
   }
}

void C_Unlock::run()
{
   P_CHECK_IF_PERUN2_IS_RUNNING;

   const p_bool s = this->context->v_exists->value && os_unlock(this->context->v_path->value);
   this->perun2.contexts.success->value = s;

   if (s) {
      this->perun2.logger.log(L"Unlock ", getCCName(this->context->v_path->value));
      if (saveChanges) {
         this->context->v_readonly->value = false;
      }
   }
   else {
      this->perun2.logger.log(L"Failed to unlock ", getCCName(this->context->v_path->value));
   }
}

void C_Unhide::run()
{
   P_CHECK_IF_PERUN2_IS_RUNNING;

   const p_bool s = this->context->v_exists->value && os_unhide(this->context->v_path->value);
   this->perun2.contexts.success->value = s;

   if (s) {
      this->perun2.logger.log(L"Unhide ", getCCName(this->context->v_path->value));
      if (saveChanges) {
         this->context->v_hidden->value = false;
      }
   }
   else {
      this->perun2.logger.log(L"Failed to unhide ", getCCName(this->context->v_path->value));
   }
}

void C_OpenWith::run()
{
   P_CHECK_IF_PERUN2_IS_RUNNING;

   const p_str pro = os_trim(program->getValue());

   if (!this->context->v_exists->value || pro.empty()) {
      this->perun2.logger.log(L"Failed to open ", getCCName(this->context->v_path->value), L" with ", getCCNameShort(pro));
      this->perun2.contexts.success->value = false;
      return;
   }

   const p_str proPath = os_leftJoin(this->locationContext->location->value, pro);

   if (proPath.empty()) {
      this->perun2.logger.log(L"Failed to open ", getCCName(this->context->v_path->value), L" with ", getCCNameShort(pro));
      this->perun2.contexts.success->value = false;
      return;
   }

   if (os_exists(proPath)) {
      const p_bool s = os_openWith(proPath, this->context->v_path->value);
      this->perun2.contexts.success->value = s;

      if (s) {
         this->perun2.logger.log(L"Open ", getCCName(this->context->v_path->value), L" with ", getCCName(proPath));
      }
      else {
         this->perun2.logger.log(L"Failed to open ", getCCName(this->context->v_path->value), L" with ", getCCName(proPath));
      }
   }
   else {
      if (!os_hasParentDirectory(this->context->v_path->value)) {
         this->perun2.logger.log(L"Failed to open ", getCCName(this->context->v_path->value), L" with '", pro, L"'");
         this->perun2.contexts.success->value = false;
         return;
      }

      const p_str com = str(pro, CHAR_SPACE, os_quoteEmbraced(this->context->v_path->value));
      const p_bool s = os_openAsCommand(com, os_parent(this->context->v_path->value));
      this->perun2.contexts.success->value = s;

      if (s) {
         this->perun2.logger.log(L"Open ", getCCName(this->context->v_path->value), L" with '", pro, L"'");
      }
      else {
         this->perun2.logger.log(L"Failed to open ", getCCName(this->context->v_path->value), L" with '", pro, L"'");
      }
   }
};

void C_MoveTo::run()
{
   P_CHECK_IF_PERUN2_IS_RUNNING;

   p_str& oldPath = this->context->v_path->value;
   p_str n = os_trim(location->getValue());

   if (!this->context->v_exists->value || os_isInvalid(n)
         || !os_hasParentDirectory(oldPath)) {

      this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
      this->perun2.contexts.success->value = false;
      return;
   }

   const p_str newLoc = os_leftJoin(this->locationContext->location->value, n);

   if (newLoc.empty()) {
      this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
      this->perun2.contexts.success->value = false;
      return;
   }

   if (!os_directoryExists(newLoc)) {
      if (!(os_hasParentDirectory(newLoc) && os_createDirectory(newLoc))) {
         this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
         this->perun2.contexts.success->value = false;
         return;
      }
   }

   const p_str fulln = os_fullname(oldPath);
   const p_str newPath = str(newLoc, OS_SEPARATOR, fulln);

   if (os_exists(newPath)) {
      if (!(forced && !(this->context->v_isdirectory->value && os_isAncestor(oldPath, newPath)) 
            && os_drop(newPath, this->perun2))) 
      {
         this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
         this->perun2.contexts.success->value = false;
         return;
      }
   }

   const p_bool s = os_moveTo(oldPath, newPath);
   this->perun2.contexts.success->value = s;

   if (s) {
      this->perun2.logger.log(L"Move ", getCCName(oldPath), L" to ", getCCName(newLoc));

      if (this->saveChanges) {
         this->context->loadData(newPath);
      }
   }
   else {
      this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
   }
}

void C_MoveTo_Stack::run()
{
   P_CHECK_IF_PERUN2_IS_RUNNING;

   p_str& oldPath = this->context->v_path->value;
   p_str n = os_trim(location->getValue());

   if (!this->context->v_exists->value || os_isInvalid(n) || !os_hasParentDirectory(oldPath)) {
      this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
      this->perun2.contexts.success->value = false;
      return;
   }

   p_str newLoc = os_leftJoin(this->locationContext->location->value, n);
   
   if (newLoc.empty()) {
      this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
      this->perun2.contexts.success->value = false;
      return;
   }

   if (!os_directoryExists(newLoc)) {
      if (!(os_hasParentDirectory(newLoc) && os_createDirectory(newLoc))) {
         this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
         this->perun2.contexts.success->value = false;
         return;
      }
   }

   const p_str fulln = os_fullname(oldPath);
   p_str newPath = str(newLoc, OS_SEPARATOR, fulln);
   const p_bool hasExt = this->context->v_isfile->value && os_hasExtension(oldPath);

   if (os_exists(newPath)) {
      if (hasExt) {
         const p_str ext = os_extension(newPath);
         const p_str base = os_stackPathBase(newPath);
         newPath = os_stackPathExt(base, ext);
      }
      else {
         newPath = os_stackPath(newPath);
      }
   }

   const p_bool s = os_moveTo(oldPath, newPath);
   this->perun2.contexts.success->value = s;

   if (s) {
      this->perun2.logger.log(L"Move ", getCCName(oldPath), L" to ", getCCName(newLoc));

      if (this->saveChanges) {
         this->context->loadData(newPath);
      }
   }
   else {
      this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
   }
}

void C_MoveToAs::run()
{
   P_CHECK_IF_PERUN2_IS_RUNNING;

   p_str& oldPath = this->context->v_path->value;
   p_str fulln = os_fullname(os_trim(name->getValue()));
   const p_str loc = os_trim(location->getValue());

   if (!this->context->v_exists->value || os_isInvalid(fulln)
         || os_isInvalid(loc) || !os_hasParentDirectory(oldPath)) {

      this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
      this->perun2.contexts.success->value = false;
      return;
   }

   const p_bool hasExt = this->context->v_isfile->value && os_hasExtension(oldPath);

   if (!extensionless && hasExt && !os_hasExtension(fulln)) {
      fulln += str(CHAR_DOT, os_extension(oldPath));
   }

   const p_str newLoc = os_leftJoin(this->locationContext->location->value, loc);

   if (newLoc.empty()) {
      this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
      this->perun2.contexts.success->value = false;
      return;
   }

   if (!os_directoryExists(newLoc)) {
      if (!(os_hasParentDirectory(newLoc) && os_createDirectory(newLoc))) {
         this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
         this->perun2.contexts.success->value = false;
         return;
      }
   }

   p_str newPath = str(newLoc, OS_SEPARATOR, fulln);

   if (os_exists(newPath)) {
      if (!(forced && !(this->context->v_isdirectory->value && os_isAncestor(oldPath, newPath))
            && os_drop(newPath, this->perun2))) 
      {
         this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
         this->perun2.contexts.success->value = false;
         return;
      }
   }

   const p_bool s = os_moveTo(oldPath, newPath);
   this->perun2.contexts.success->value = s;

   if (s) {
      this->perun2.logger.log(L"Move ", getCCName(oldPath), L" to ", getCCName(newLoc), L" as '", fulln, L"'");

      if (this->saveChanges) {
         this->context->loadData(newPath);
      }
   }
   else {
      this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
   }
}

void C_MoveToAs_Stack::run()
{
   P_CHECK_IF_PERUN2_IS_RUNNING;

   const p_str& oldPath = this->context->v_path->value;
   p_str fulln = os_fullname(os_trim(name->getValue()));
   const p_str loc = os_trim(location->getValue());

   if (!this->context->v_exists->value || os_isInvalid(fulln)
         || os_isInvalid(loc) || !os_hasParentDirectory(oldPath)) {

      this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
      this->perun2.contexts.success->value = false;
      return;
   }

   const p_bool hasExt = this->context->v_isfile->value && os_hasExtension(oldPath);
   const p_str newLoc = os_leftJoin(this->locationContext->location->value, loc);

   if (newLoc.empty()) {
      this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
      this->perun2.contexts.success->value = false;
      return;
   }

   if (!os_directoryExists(newLoc)) {
      if (!(os_hasParentDirectory(newLoc) && os_createDirectory(newLoc))) {
         this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
         this->perun2.contexts.success->value = false;
         return;
      }
   }

   p_str newPath;
   p_bool s;

   if (hasExt) {
      p_str ex;
      if (os_hasExtension(fulln)) {
         ex = os_extension(fulln);
      }
      else {
         ex = os_extension(oldPath);
         if (!extensionless) {
            fulln += str(CHAR_DOT, ex);
         }
      }

      newPath = str(newLoc, OS_SEPARATOR, fulln);
      if (os_exists(newPath)) {
         const p_str base = os_stackPathBase(newPath);
         newPath = os_stackPathExt(base, ex);
         fulln = os_fullname(newPath);
      }
   }
   else {
      newPath = str(newLoc, OS_SEPARATOR, fulln);
      if (os_exists(newPath)) {
         newPath = os_stackPath(newPath);
         fulln = os_fullname(newPath);
      }
   }

   s = os_moveTo(oldPath, newPath);
   this->perun2.contexts.success->value = s;

   if (s) {
      this->perun2.logger.log(L"Move ", getCCName(oldPath), L" to ", getCCName(newLoc), L" as '", fulln, L"'");

      if (this->saveChanges) {
         this->context->loadData(newPath);
      }
   }
   else {
      this->perun2.logger.log(L"Failed to move ", getCCName(oldPath));
   }
}

p_str getCCName(const p_str& path)
{
   if (path.empty()) {
      return STRING_NOTHING;
   }

   return os_hasParentDirectory(path) || path.size() != 2
      ? str(CHAR_APOSTROPHE, os_fullname(path), CHAR_APOSTROPHE)
      : str(CHAR_APOSTROPHE, path, OS_SEPARATOR, CHAR_APOSTROPHE);
}

p_str getCCNameShort(const p_str& path)
{
   if (path.empty()) {
      return STRING_NOTHING;
   }

   const p_str f = os_fullname(path);
   if (f.empty()) {
      return STRING_NOTHING;
   }

   return f.size() == 2 && f[1] == CHAR_COLON && os_isDriveLetter(f[0])
      ? str(CHAR_APOSTROPHE, f, OS_SEPARATOR, CHAR_APOSTROPHE)
      : str(CHAR_APOSTROPHE, f, CHAR_APOSTROPHE);
}

}
