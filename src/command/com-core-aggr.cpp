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

#include "com-core-aggr.h"
#include "../print.h"
#include "../uroboros.h"
#include "../os.h"


namespace uro::comm
{

void C_AggrCopy_String::run()
{
   const _str n = os_trim(value->getValue());
   if (n.empty()) {
      aggregate->failedCopy++;
   }
   else if (os_isInvaild(n)) {
      aggregate->invalidCopy.insert(os_fullname(n));
   }
   else {
      aggregate->copyPaths.insert(os_join(this->inner->location.value, n));
   }
}

void C_AggrCopy_List::run()
{
   const _list list = value->getValue();
   const _size len = list.size();

   for (_size i = 0; i < len; i++) {
      const _str n = os_trim(list[i]);
      if (n.empty()) {
         aggregate->failedCopy++;
      }
      else if (os_isInvaild(n)) {
         aggregate->invalidCopy.insert(os_fullname(n));
      }
      else {
         aggregate->copyPaths.insert(os_join(this->inner->location.value, n));
      }
   }
}

void C_AggrSelect_String::run()
{
   const _str n = os_trim(value->getValue());

   if (n.empty()) {
      aggregate->failedSelect++;
   }
   else if (os_isInvaild(n)) {
      aggregate->invalidSelect.insert(os_fullname(n));
   }
   else {
      const _str path = os_join(this->inner->location.value, n);

      if (os_hasParentDirectory(path)) {
         const _str parent = os_parent(path);
         auto it = aggregate->selectPaths.find(parent);

         if (it == aggregate->selectPaths.end()) {
            std::set<_str> newSet;
            newSet.insert(path);
            aggregate->selectPaths.insert(
               std::pair<_str,std::set<_str>>(parent, newSet));
         }
         else {
            it->second.insert(path);
         }
      }
      else {
         aggregate->invalidSelect.insert(os_fullname(n));
      }
   }
}

void C_AggrSelect_List::run()
{
   const _list elements = value->getValue();
   const _size length = elements.size();
   _str prevParent;
   std::set<_str>* prevSet;

   for (_size i = 0; i < length; i++) {
      const _str n = os_trim(elements[i]);
      if (n.empty()) {
         aggregate->failedSelect++;
      }
      else if (os_isInvaild(n)) {
         aggregate->invalidSelect.insert(os_fullname(n));
      }
      else {
         const _str path = os_join(this->inner->location.value, n);

         if (os_hasParentDirectory(path)) {
            const _str parent = os_parent(path);

            if (i > 0 && parent == prevParent) {
               prevSet->insert(path);
            }
            else {
               auto it = aggregate->selectPaths.find(parent);
               if (it == aggregate->selectPaths.end()) {
                  std::set<_str> newSet;
                  newSet.insert(path);
                  aggregate->selectPaths.insert(
                     std::pair<_str, std::set<_str>>(parent, newSet));

                  prevSet = &(aggregate->selectPaths.find(parent)->second);
               }
               else {
                  it->second.insert(path);
                  prevSet = &(it->second);
               }
               prevParent = parent;
            }
         }
         else {
            aggregate->invalidSelect.insert(os_fullname(n));
         }
      }
   }
}



void logCopyError(Uroboros* uro, const _str& name)
{
   commandLog(uro, L"Failed to copy ", getCCNameShort(name));
}

void logCopySuccess(Uroboros* uro, const _str& name)
{
   commandLog(uro, L"Copy ", getCCNameShort(name));
}

void logSelectError(Uroboros* uro, const _str& name)
{
   commandLog(uro, L"Failed to select ", getCCNameShort(name));
}

void logSelectSuccess(Uroboros* uro, const _str& name)
{
   commandLog(uro, L"Select ", getCCNameShort(name));
}



void C_Copy_String::run()
{
   const _str n = os_trim(value->getValue());
   if (os_isInvaild(n)) {
      logCopyError(this->uroboros, n);
      this->inner->success.value = false;
      return;
   }

   const _str path = os_join(this->inner->location.value, n);
   if (os_exists(path)) {
      std::set<_str> set;
      set.insert(path);
      const _bool s = os_copy(set);
      if (s) {
         logCopySuccess(this->uroboros, path);
      }
      else {
         logCopyError(this->uroboros, path);
      }
      this->inner->success.value = s;

   }
   else {
      logCopyError(this->uroboros, path);
      this->inner->success.value = false;
   }
}

void C_Copy_List::run()
{
   const _list elements = value->getValue();
   const _size length = elements.size();

   if (length == 0) {
      this->inner->success.value = true;
      return;
   }

   std::set<_str> set;
   _bool anyFailure = false;

   for (_size i = 0; i < length; i++) {
      const _str n = os_trim(elements[i]);
      if (os_isInvaild(n)) {
         logCopyError(this->uroboros, n);
         anyFailure = true;
      }
      else {
         const _str path = os_join(this->inner->location.value, n);
         if (os_exists(path)) {
            set.insert(path);
         }
         else {
            logCopyError(this->uroboros, path);
            anyFailure = true;
         }
      }
   }

   if (!set.empty()) {
      const _bool s = os_copy(set);
      const auto end = set.end();

      for (auto it = set.begin(); it != end; ++it) {
         if (s) {
            logCopySuccess(this->uroboros, *it);
         }
         else {
            logCopyError(this->uroboros, *it);
         }
      }
   }

   this->inner->success.value = !anyFailure;
}

void C_Select_String::run()
{
   const _str n = os_trim(value->getValue());
   if (os_isInvaild(n)) {
      logSelectError(this->uroboros, n);
      this->inner->success.value = false;
      return;
   }

   const _str path = os_join(this->inner->location.value, n);
   _bool success = false;

   if (os_exists(path) && os_hasParentDirectory(path)) {
      const _str parent = os_parent(path);

      if (os_directoryExists(parent)) {
         std::set<_str> set;
         set.insert(path);
         success = os_select(parent, set);

         if (success) {
            logSelectSuccess(this->uroboros, path);
         }
         else {
            logSelectError(this->uroboros, path);
         }
      }
      else {
         logSelectError(this->uroboros, path);
      }
   }
   else {
      logSelectError(this->uroboros, path);
   }

   this->inner->success.value = success;
}

void C_Select_List::run()
{
   const _list elements = value->getValue();
   const _size length = elements.size();

   if (length == 0) {
      this->inner->success.value = true;
      return;
   }

   std::map<_str, std::set<_str>> selectPaths;
   _str prevParent;
   std::set<_str>* prevSet;
   bool isFirst = true;

   for (_size i = 0; i < length; i++) {
      const _str n = os_trim(elements[i]);

      if (os_isInvaild(n)) {
         logSelectError(this->uroboros, n);
         continue;
      }

      const _str path = os_join(this->inner->location.value, n);

      if (!os_exists(path) || !os_hasParentDirectory(path)) {
         logSelectError(this->uroboros, path);
         continue;
      }

      const _str parent = os_parent(path);
      if (!os_directoryExists(parent)) {
         logSelectError(this->uroboros, path);
         continue;
      }

      if (!isFirst && parent == prevParent) {
         prevSet->insert(path);
      }
      else {
         auto it = selectPaths.find(parent);
         if (it == selectPaths.end())
         {
            std::set<_str> newSet;
            newSet.insert(path);
            selectPaths.insert(std::pair<_str, std::set<_str>>(parent, newSet));
            prevSet = &(selectPaths.find(parent)->second);
         }
         else
         {
            it->second.insert(path);
            prevSet = &(it->second);
         }
         prevParent = parent;
      }

      isFirst = false;
   }

   if (selectPaths.empty()) {
      this->inner->success.value = false;
   }
   else {
      _bool anyFailed = false;

      for (auto it = selectPaths.begin(); it != selectPaths.end(); it++)
      {
         if (this->uroboros->state != State::s_Running) {
            break;
         }

         const _bool success = os_select(it->first, it->second);
         if (!anyFailed && !success) {
            anyFailed = true;
         }

         const auto end = it->second.end();
         for (auto it2 = it->second.begin(); it2 != end; ++it2) {
            if (success) {
               logSelectSuccess(this->uroboros, *it2);
            }
            else {
               logSelectError(this->uroboros, *it2);
            }
         }
      }

      this->inner->success.value = !anyFailed;
   }
}

}
