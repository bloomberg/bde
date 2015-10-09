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


    // These are declared in bslstl_iosfwd.h (so cannot be using declarations):
    //..
    //  using native_std::allocator;
    //  using native_std::basic_istringstream;
    //  using native_std::basic_ostringstream;
    //  using native_std::basic_stringbuf
    //  using native_std::basic_stringstream
    //  using native_std::istringstream;
    //  using native_std::ostringstream;
    //  using native_std::stringbuf;
    //  using native_std::stringstream;
    //  using native_std::wistringstream;
    //  using native_std::wostringstream;
    //  using native_std::wstringbuf;
    //  using native_std::wstringstream;
    //..

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

}  // close package namespace

// Include Bloomberg's implementation, unless compilation is configured to
// override native types in the 'std' namespace with Bloomberg's
// implementation, in which case the implementation file will be included by
// the Bloomberg supplied standard header file.

#ifndef BSL_OVERRIDES_STD
#include <bslstl_iosfwd.h>
#endif

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
