// bslstl_iosfwd.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLSTL_IOSFWD
#define INCLUDED_BSLSTL_IOSFWD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide forward declarations for Standard stream classes.
//
//@CLASSES:
//  bsl::stringbuf: specialization for bsl::basic_stringbuf<char>
//  bsl::istringstream: specialization for bsl::basic_istringstream<char>
//  bsl::ostringstream: specialization for bsl::basic_ostringstream<char>
//  bsl::stringstream: specialization for bsl::basic_stringstream<char>
//  bsl::wstringbuf: specialization for bsl::basic_stringbuf<wchar_t>
//  bsl::wistringstream: specialization for bsl::basic_istringstream<wchar_t>
//  bsl::wostringstream: specialization for bsl::basic_ostringstream<wchar_t>
//  bsl::wstringstream: specialization for bsl::basic_stringstream<wchar_t>
//
//@CANONICAL_HEADER: bsl_iosfwd.h
//
//@SEE_ALSO: bslstl_stringbuf, bslstl_istringstream, bslstl_ostringstream,
//           bslstl_stringstream
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_iosfwd.h>' instead.

#include <bslscm_version.h>

#include <iosfwd>

namespace bsl {

using std::char_traits;

template <class TYPE> class allocator;

template <class CHAR_TYPE, class CHAR_TRAITS = char_traits<CHAR_TYPE>,
          class ALLOCATOR = allocator<CHAR_TYPE> >
class basic_stringbuf;

template <class CHAR_TYPE, class CHAR_TRAITS = char_traits<CHAR_TYPE>,
          class ALLOCATOR = allocator<CHAR_TYPE> >
class basic_istringstream;

template <class CHAR_TYPE, class CHAR_TRAITS = char_traits<CHAR_TYPE>,
          class ALLOCATOR = allocator<CHAR_TYPE> >
class basic_ostringstream;

template <class CHAR_TYPE, class CHAR_TRAITS = char_traits<CHAR_TYPE>,
          class ALLOCATOR = allocator<CHAR_TYPE> >
class basic_stringstream;

typedef basic_stringbuf    <char, char_traits<char>,
                                               allocator<char> >     stringbuf;
typedef basic_istringstream<char, char_traits<char>,
                                               allocator<char> > istringstream;
typedef basic_ostringstream<char, char_traits<char>,
                                               allocator<char> > ostringstream;
typedef basic_stringstream <char, char_traits<char>,
                                               allocator<char> >  stringstream;

typedef basic_stringbuf    <wchar_t, char_traits<wchar_t>,
                                           allocator<wchar_t> >     wstringbuf;
typedef basic_istringstream<wchar_t, char_traits<wchar_t>,
                                           allocator<wchar_t> > wistringstream;
typedef basic_ostringstream<wchar_t, char_traits<wchar_t>,
                                           allocator<wchar_t> > wostringstream;
typedef basic_stringstream <wchar_t, char_traits<wchar_t>,
                                           allocator<wchar_t> >  wstringstream;

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
