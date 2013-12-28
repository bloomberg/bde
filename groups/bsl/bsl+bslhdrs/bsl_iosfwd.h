// bsl_iosfwd.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_IOSFWD
#define INCLUDED_BSL_IOSFWD

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#include <iosfwd>

namespace bsl
{
    // Import selected symbols into bsl namespace
#ifdef BDE_OMIT_INTERNAL_DEPRECATED
    using native_std::basic_filebuf;
    using native_std::basic_fstream;
    using native_std::basic_ifstream;
    using native_std::basic_ios;
    using native_std::basic_iostream;
    using native_std::basic_istream;
    using native_std::basic_ofstream;
    using native_std::basic_ostream;
    using native_std::basic_streambuf;
    using native_std::char_traits;
    using native_std::filebuf;
    using native_std::fpos;
    using native_std::fstream;
    using native_std::ifstream;
    using native_std::ios;
    using native_std::iostream;
    using native_std::istream;
    using native_std::istreambuf_iterator;
    using native_std::ofstream;
    using native_std::ostream;
    using native_std::ostreambuf_iterator;
    using native_std::streambuf;
    using native_std::streamoff;
    using native_std::streampos;
    using native_std::wfilebuf;
    using native_std::wfstream;
    using native_std::wifstream;
    using native_std::wios;
    using native_std::wiostream;
    using native_std::wistream;
    using native_std::wofstream;
    using native_std::wostream;
    using native_std::wstreambuf;
    using native_std::wstreampos;
#else
    using native_std::basic_filebuf;
    using native_std::basic_fstream;
    using native_std::basic_ifstream;
    using native_std::basic_ios;
    using native_std::basic_iostream;
    using native_std::basic_istream;
    using native_std::basic_ofstream;
    using native_std::basic_ostream;
    using native_std::basic_streambuf;
    using native_std::char_traits;
    using native_std::filebuf;
    using native_std::fpos;
    using native_std::fstream;
    using native_std::ifstream;
    using native_std::ios;
    using native_std::iostream;
    using native_std::istream;
    using native_std::istreambuf_iterator;
    using native_std::ofstream;
    using native_std::ostream;
    using native_std::ostreambuf_iterator;
    using native_std::streambuf;
    using native_std::streamoff;
    using native_std::streampos;
    using native_std::wfilebuf;
    using native_std::wfstream;
    using native_std::wifstream;
    using native_std::wios;
    using native_std::wiostream;
    using native_std::wistream;
    using native_std::wofstream;
    using native_std::wostream;
    using native_std::wstreambuf;
    using native_std::wstreampos;
#endif
}

// Include Bloomberg's implementation, unless compilation is configured to
// override native types in the 'std' namespace with Bloomberg's
// implementation, in which case the implementation file will be included by
// the Bloomberg supplied standard header file.

#ifndef BSL_OVERRIDES_STD
#include <bslstl_iosfwd.h>
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
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
