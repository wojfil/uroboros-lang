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

#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <cstdlib>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>


namespace uro
{

#define _COUT                       std::wcout
#define _CIN                        std::wcin

typedef int                         _exitint;
typedef bool                        _bool;
typedef int32_t                     _int;
typedef uint32_t                    _uint32;
typedef uint64_t                    _uint64;
typedef long unsigned int           _ulong;
typedef std::size_t                 _size;
typedef wchar_t                     _char;
typedef std::wstring                _str;
typedef std::vector<_str>           _list;

typedef int64_t                     _nint;
typedef long double                 _ndouble;
inline constexpr _size BITS_IN_NINT = static_cast<_size>(sizeof(_nint) * 8);

typedef WIN32_FILE_ATTRIBUTE_DATA   _adata;
typedef WIN32_FIND_DATAW            _fdata;
typedef FILETIME                    _ftim;
typedef std::wstringstream          _stream;
typedef std::wostringstream         _ostream;


}

#endif /* PRIMITIVES_H */
