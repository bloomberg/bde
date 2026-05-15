// bsl_iosfwd.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_IOSFWD
#define INCLUDED_BSL_IOSFWD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsls_libraryfeatures.h>
#include <bslstl_iosfwd.h>

#include <iosfwd>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {
    // Import selected symbols into bsl namespace


    // These are declared in bslstl_iosfwd.h (so cannot be using declarations):
    //..
    //  using std::allocator;
    //  using std::basic_istringstream;
    //  using std::basic_ostringstream;
    //  using std::basic_stringbuf;
    //  using std::basic_stringstream;
    //  using std::istringstream;
    //  using std::ostringstream;
    //  using std::stringbuf;
    //  using std::stringstream;
    //  using std::wistringstream;
    //  using std::wostringstream;
    //  using std::wstringbuf;
    //  using std::wstringstream;
    //
    //  using basic_osyncstream
    //  using basic_syncbuf
    //  using osyncstream
    //  using syncbuf
    //  using wosyncstream
    //  using wsyncbuf
    //..

    using std::basic_filebuf;
    using std::basic_fstream;
    using std::basic_ifstream;
    using std::basic_ios;
    using std::basic_iostream;
    using std::basic_istream;
    using std::basic_ofstream;
    using std::basic_ostream;
    using std::basic_streambuf;
    using std::char_traits;
    using std::filebuf;
    using std::fpos;
    using std::fstream;
    using std::ifstream;
    using std::ios;
    using std::iostream;
    using std::istream;
    using std::istreambuf_iterator;
    using std::ofstream;
    using std::ostream;
    using std::ostreambuf_iterator;
    using std::streambuf;
    using std::streamoff;
    using std::streampos;
    using std::wfilebuf;
    using std::wfstream;
    using std::wifstream;
    using std::wios;
    using std::wiostream;
    using std::wistream;
    using std::wofstream;
    using std::wostream;
    using std::wstreambuf;
    using std::wstreampos;

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
    using std::u16streampos;
    using std::u32streampos;
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY)
    using std::u8streampos;
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP23_SPANSTREAM)
    using std::basic_ispanstream;
    using std::basic_ospanstream;
    using std::basic_spanbuf;
    using std::basic_spanstream;
    using std::ispanstream;
    using std::ospanstream;
    using std::spanbuf;
    using std::spanstream;
    using std::wspanbuf;
    using std::wispanstream;
    using std::wospanstream;
    using std::wspanstream;
#endif

}  // close package namespace

// Include Bloomberg's implementation.
#include <bslstl_iosfwd.h>

#endif

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
