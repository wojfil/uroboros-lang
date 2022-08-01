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

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include "gen-os.h"
#include "gen-string.h"
#include "../../uroboros.h"
#include "../../os.h"



_def* DefinitionGenerator::generateDefault() const
{
   return this->generatePattern(new LocationReference(this->uroboros),
      this->element_, OS_SEPARATOR_ASTERISK);
}

_def* DefinitionGenerator::generatePattern(Generator<_str>* location,
   const _uint32& element, const _str& pattern) const
{
   switch (element) {
      case ELEM_ALL: {
         return new Uro_All(location, this->uroboros, pattern);
      }
      case ELEM_DIRECTORIES: {
         return new Uro_Directories(location, this->uroboros, pattern);
      }
      case ELEM_FILES: {
         return new Uro_Files(location, this->uroboros, pattern);
      }
      case ELEM_RECURSIVE_FILES: {
         return new Uro_RecursiveFiles(location, this->uroboros, pattern);
      }
      case ELEM_RECURSIVE_DIRECTORIES: {
         return new Uro_RecursiveDirectories(location, this->uroboros, pattern);
      }
      case ELEM_RECURSIVE_ALL: {
         return new Uro_RecursiveAll(location, this->uroboros, pattern);
      }
      default: {
         return nullptr;
      }
   }
}

OsDefinition::OsDefinition(Generator<_str>* loc, Uroboros* uro, const _str& patt)
   : first(true), location(loc), uroboros(uro),
     inner(&uro->vars.inner), flags(uro->flags), pattern(patt) { };

void OsDefinitionPlain::reset()
{
   if (!first) {
      first = true;
      FindClose(handle);
      this->inner->this_s.value = prevThis;
      this->inner->index.value = prevIndex;
      this->inner->depth.value = prevDepth;
   }
}

void OsDefinitionRecursive::reset()
{
   if (!first) {
      first = true;
      paths.clear();
      bases.clear();
      const _size len = handles.size();
      if (len != 0) {
         for (_size i = 0; i < len; i++) {
            FindClose(handles[i]);
         }
         handles.clear();
      }
      this->inner->depth.value = prevDepth;
      this->inner->this_s.value = prevThis;
      this->inner->index.value = prevIndex;
   }
}

_boo Uro_All::hasNext()
{
   if (first) {
      _str path = os_trim(location->getValue());
      if (os_directoryExists(path)) {
         path = str(path, pattern);
         handle = FindFirstFile(path.c_str(), &data);
         if (handle == INVALID_HANDLE_VALUE) {
            return false;
         }

         first = false;
         value = data.cFileName;
         prevThis = this->inner->this_s.value;
         prevIndex = this->inner->index.value;
         prevDepth = this->inner->depth.value;
         this->inner->depth.value.setToZero();
         index.setToZero();
         this->inner->index.value = index;

         if (!os_isBrowsePath(value)) {
            const _boo isDir = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

            if ((this->flags & FLAG_NOOMIT) || (isDir && value != OS_GIT_DIRECTORY)
               || (!isDir && os_extension(value) != OS_UROEXT))
            {
               this->inner->index.value = index;
               index++;
               this->inner->depth.value.value.i = 0LL;
               this->inner->this_s.value = value;
               return true;
            }
         }
      }
      else {
         return false;
      }
   }

   while (FindNextFile(handle, &data)) {
      value = data.cFileName;

      if (!os_isBrowsePath(value)) {
         const _boo isDir = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

         if ((this->flags & FLAG_NOOMIT) || (isDir && value != OS_GIT_DIRECTORY)
            || (!isDir && os_extension(value) != OS_UROEXT))
         {
            this->inner->index.value = index;
            index++;
            this->inner->depth.value.value.i = 0LL;
            this->inner->this_s.value = value;
            return true;
         }
      }
   }

   first = true;
   FindClose(handle);
   this->inner->this_s.value = prevThis;
   this->inner->index.value = prevIndex;
   this->inner->depth.value = prevDepth;
   return false;
}

_boo Uro_Files::hasNext()
{
   if (first) {
      _str path = os_trim(location->getValue());
      if (os_directoryExists(path)) {
         path = str(path, pattern);
         handle = FindFirstFile(path.c_str(), &data);
         if (handle == INVALID_HANDLE_VALUE) {
            return false;
         }

         first = false;
         value = data.cFileName;
         prevThis = this->inner->this_s.value;
         prevIndex = this->inner->index.value;
         prevDepth = this->inner->depth.value;
         this->inner->depth.value.setToZero();
         index.setToZero();
         this->inner->index.value = index;

         if (!os_isBrowsePath(value)) {
            const _boo isDir = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

            if (!isDir && ((this->flags & FLAG_NOOMIT) || os_extension(value) != OS_UROEXT)) {
               this->inner->index.value = index;
               index++;
               this->inner->depth.value.value.i = 0LL;
               this->inner->this_s.value = value;
               return true;
            }
         }
      }
      else {
         return false;
      }
   }

   while (FindNextFile(handle, &data)) {
      value = data.cFileName;

      if (!os_isBrowsePath(value)) {
         const _boo isDir = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

         if (!isDir && ((this->flags & FLAG_NOOMIT) || os_extension(value) != OS_UROEXT)) {
            this->inner->index.value = index;
            index++;
            this->inner->depth.value.value.i = 0LL;
            this->inner->this_s.value = value;
            return true;
         }
      }
   }

   first = true;
   FindClose(handle);
   this->inner->this_s.value = prevThis;
   this->inner->index.value = prevIndex;
   this->inner->depth.value = prevDepth;
   return false;
}

_boo Uro_Directories::hasNext()
{
   if (first) {
      _str path = os_trim(location->getValue());
      if (os_directoryExists(path)) {
         path = str(path, pattern);
         handle = FindFirstFile(path.c_str(), &data);
         if (handle == INVALID_HANDLE_VALUE) {
            return false;
         }

         first = false;
         value = data.cFileName;
         prevThis = this->inner->this_s.value;
         prevIndex = this->inner->index.value;
         prevDepth = this->inner->depth.value;
         this->inner->depth.value.setToZero();
         index.setToZero();
         this->inner->index.value = index;

         if (!os_isBrowsePath(value)) {
            const _boo isDir = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

            if (isDir && ((this->flags & FLAG_NOOMIT) || value != OS_GIT_DIRECTORY)) {
               this->inner->index.value = index;
               index++;
               this->inner->depth.value.value.i = 0LL;
               this->inner->this_s.value = value;
               return true;
            }
         }
      }
      else {
         return false;
      }
   }

   while (FindNextFile(handle, &data)) {
      value = data.cFileName;

      if (!os_isBrowsePath(value)) {
         const _boo isDir = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

         if (isDir && ((this->flags & FLAG_NOOMIT) || value != OS_GIT_DIRECTORY)) {
            this->inner->index.value = index;
            index++;
            this->inner->depth.value.value.i = 0LL;
            this->inner->this_s.value = value;
            return true;
         }
      }
   }

   first = true;
   FindClose(handle);
   this->inner->this_s.value = prevThis;
   this->inner->index.value = prevIndex;
   this->inner->depth.value = prevDepth;
   return false;
}

_boo Uro_RecursiveFiles::hasNext()
{
   if (first) {
      this->paths.push_back(os_trim(location->getValue()));
      prevDepth = this->inner->depth.value;
      this->inner->depth.value.setToZero();
      goDeeper = true;
      first = false;
      prevThis = this->inner->this_s.value;
      prevIndex = this->inner->index.value;
      index.setToZero();
      this->inner->index.value = index;
   }

   while (this->uroboros->running) {
      if (goDeeper) {
         goDeeper = false;
         if (os_directoryExists(paths.back())) {
            const _str p = str(paths.back(), pattern);
            handles.push_back(FindFirstFile(p.c_str(), &data));

            if (handles.back() == INVALID_HANDLE_VALUE)
            {
               handles.pop_back();
               paths.pop_back();
               if (paths.empty()) {
                  break;
               }
               else {
                  bases.pop_back();
                  this->inner->depth.value--;
               }
            }
            else if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
               const _str& v = data.cFileName;

               if ((this->flags & FLAG_NOOMIT) || os_extension(v) != OS_UROEXT) {
                  value = v;
                  this->inner->index.value = index;
                  index++;
                  this->inner->this_s.value = value;
                  return true;
               }
            }
         }
         else {
            paths.pop_back();
            if (paths.empty()) {
               break;
            }
            else {
               bases.pop_back();
               this->inner->depth.value--;
            }
         }
      }
      else {
         if (FindNextFile(handles.back(), &data)) {
            const _str& v = data.cFileName;

            if (!os_isBrowsePath(v)) {
               if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                  if ((this->flags & FLAG_NOOMIT) || v != OS_GIT_DIRECTORY) {
                     paths.push_back(str(paths.back(), OS_SEPARATOR_STRING, v));

                     if (this->inner->depth.value.isZero()) {
                        bases.push_back(str(v, OS_SEPARATOR_STRING));
                     }
                     else {
                        bases.push_back(str(bases.back(), v, OS_SEPARATOR_STRING));
                     }

                     goDeeper = true;
                     this->inner->depth.value++;
                  }
               }
               else  if ((this->flags & FLAG_NOOMIT) || os_extension(v) != OS_UROEXT) {
                  value = this->inner->depth.value.isZero() ? v : str(bases.back(), v);
                  this->inner->index.value = index;
                  index++;
                  this->inner->this_s.value = value;
                  return true;
               }
            }
         }
         else {
            FindClose(handles.back());
            handles.pop_back();
            paths.pop_back();

            if (paths.empty()) {
               break;
            }
            else {
               bases.pop_back();
               this->inner->depth.value--;
            }
         }
      }
   }

   reset();
   return false;
}

_boo Uro_RecursiveDirectories::hasNext()
{
   if (first) {
      paths.push_back(os_trim(location->getValue()));
      prevDepth = this->inner->depth.value;
      this->inner->depth.value.value.i = -1LL;
      goDeeper = true;
      first = false;
      prevThis = this->inner->this_s.value;
      prevIndex = this->inner->index.value;
      index.setToZero();
      this->inner->index.value = index;
   }

   while (this->uroboros->running) {
      if (goDeeper) {
         goDeeper = false;
         if (os_directoryExists(paths.back())) {
            const _str p = str(paths.back(), pattern);

            handles.push_back(FindFirstFile(p.c_str(), &data));
            if (handles.back() == INVALID_HANDLE_VALUE)
            {
               handles.pop_back();
               paths.pop_back();
               if (paths.empty()) {
                  break;
               }
               else {
                  bases.pop_back();
                  this->inner->depth.value--;
               }
            }
         }
         else {
            paths.pop_back();
            if (paths.empty()) {
               break;
            }
            else {
               bases.pop_back();
               this->inner->depth.value--;
            }
         }
      }
      else {
         if (FindNextFile(handles.back(), &data)) {
            const _str& v = data.cFileName;

            if (!os_isBrowsePath(v) && (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                && ((this->flags & FLAG_NOOMIT) || v != OS_GIT_DIRECTORY))
            {
               value = this->inner->depth.value.isMinusOne() ? v : str(bases.back(), v);
               paths.push_back(str(paths.back(), OS_SEPARATOR_STRING, v));

               if (this->inner->depth.value.isMinusOne()) {
                  bases.push_back(str(v, OS_SEPARATOR_STRING));
               }
               else {
                  bases.push_back(str(bases.back(), v, OS_SEPARATOR_STRING));
               }

               goDeeper = true;
               this->inner->depth.value++;
               this->inner->index.value = index;
               index++;
               this->inner->this_s.value = value;
               return true;
            }
         }
         else {
            FindClose(handles.back());
            handles.pop_back();
            paths.pop_back();

            if (paths.empty()) {
               break;
            }
            else {
               bases.pop_back();
               this->inner->depth.value--;
            }
         }
      }
   }

   reset();
   return false;
}

_boo Uro_RecursiveAll::hasNext()
{
   if (first) {
      paths.push_back(os_trim(location->getValue()));
      prevDepth = this->inner->depth.value;
      this->inner->depth.value.value.i = -1LL;
      goDeeper = true;
      first = false;
      prevThis = this->inner->this_s.value;
      prevIndex = this->inner->index.value;
      index.setToZero();
      this->inner->index.value = index;
   }

   while (this->uroboros->running) {
      if (goDeeper) {
         goDeeper = false;
         if (os_directoryExists(paths.back())) {
            const _str p = str(paths.back(), pattern);

            handles.push_back(FindFirstFile(p.c_str(), &data));
            if (handles.back() == INVALID_HANDLE_VALUE)
            {
               handles.pop_back();
               paths.pop_back();
               if (paths.empty()) {
                  break;
               }
               else {
                  bases.pop_back();
                  this->inner->depth.value--;
               }
            }
         }
         else {
            paths.pop_back();
            if (paths.empty()) {
               break;
            }
            else {
               bases.pop_back();
               this->inner->depth.value--;
            }
         }
      }
      else {
         if (FindNextFile(handles.back(), &data)) {
            const _str& v = data.cFileName;

            if (!os_isBrowsePath(v)) {
               if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                  && ((this->flags & FLAG_NOOMIT) || v != OS_GIT_DIRECTORY))
               {
                  if (this->prevFile) {
                     this->prevFile = false;
                     this->inner->depth.value--;
                  }

                  value = this->inner->depth.value.isMinusOne() ? v : str(bases.back(), v);
                  paths.push_back(str(paths.back(), OS_SEPARATOR_STRING, v));

                  if (this->inner->depth.value.isMinusOne()) {
                     bases.push_back(str(v, OS_SEPARATOR_STRING));
                  }
                  else {
                     bases.push_back(str(bases.back(), v, OS_SEPARATOR_STRING));
                  }

                  goDeeper = true;
                  this->inner->depth.value++;
                  this->inner->index.value = index;
                  index++;
                  this->inner->this_s.value = value;
                  return true;
               }
               else if ((!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                  && (this->flags & FLAG_NOOMIT) || os_extension(v) != OS_UROEXT)
               {
                  if (!this->prevFile) {
                     this->prevFile = true;
                     this->inner->depth.value++;
                  }

                  value = this->inner->depth.value.isZero() ? v : str(bases.back(), v);
                  this->inner->index.value = index;
                  index++;
                  this->inner->this_s.value = value;
                  return true;
               }
            }
         }
         else {
            FindClose(handles.back());
            handles.pop_back();
            paths.pop_back();

            if (paths.empty()) {
               break;
            }
            else {
               bases.pop_back();
               this->inner->depth.value--;
            }
         }
      }
   }

   reset();
   return false;
}
