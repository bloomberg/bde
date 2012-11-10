// bslstl_iosfwd.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLSTL_IOSFWD
#define INCLUDED_BSLSTL_IOSFWD

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
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
//@SEE_ALSO: bslstl_stringbuf, bslstl_istringstream, bslstl_ostringstream,
//           bslstl_stringstream
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_iosfwd.h>' instead.

#if defined(std)
#define BSLSTL_IOSFWD_STD std
#undef std
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace std {

template <class CHAR_TYPE> struct char_traits;

}  // close namespace std

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

#if defined(BSLSTL_IOSFWD_STD)
#define std BSLSTL_IOSFWD_STD
#undef BSLSTL_IOSFWD_STD
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
