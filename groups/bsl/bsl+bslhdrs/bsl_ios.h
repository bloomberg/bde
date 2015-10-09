// bsl_ios.h                                                          -*-C++-*-
#ifndef INCLUDED_BSL_IOS
#define INCLUDED_BSL_IOS

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

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#include <ios>

namespace bsl
{
    // Import selected symbols into bsl namespace

    using native_std::basic_ios;
    using native_std::fpos;
    using native_std::ios;
    using native_std::ios_base;
    using native_std::wios;
    using native_std::streamoff;
    using native_std::streamsize;
    using native_std::streampos;
    using native_std::wstreampos;


    // The following using declarations result in link time errors with the
    // gcc 4.1.2 compiler (due to a compiler bug).  See DRQS 31108406.
#if !(defined(BSLS_PLATFORM_CMP_GNU)               \
    && (BSLS_PLATFORM_CMP_VER_MAJOR >= 40000)      \
    && (BSLS_PLATFORM_CMP_VER_MAJOR  < 40300))

    using native_std::boolalpha;
    using native_std::dec;
    using native_std::fixed;
    using native_std::hex;
    using native_std::internal;
    using native_std::left;
    using native_std::oct;
    using native_std::right;
    using native_std::scientific;
    using native_std::showbase;
    using native_std::showpoint;
    using native_std::showpos;
    using native_std::skipws;
    using native_std::unitbuf;
    using native_std::uppercase;
    using native_std::noboolalpha;
    using native_std::noshowbase;
    using native_std::noshowpoint;
    using native_std::noshowpos;
    using native_std::noskipws;
    using native_std::nounitbuf;
    using native_std::nouppercase;
#else
    inline bsl::ios_base& boolalpha(ios_base& str) {
        return native_std::boolalpha(str);
    }

    inline bsl::ios_base& dec(ios_base& str) {
        return native_std::dec(str);
    }

    inline bsl::ios_base& fixed(ios_base& str) {
        return native_std::fixed(str);
    }

    inline bsl::ios_base& hex(ios_base& str) {
        return native_std::hex(str);
    }

    inline bsl::ios_base& internal(ios_base& str) {
        return native_std::internal(str);
    }

    inline bsl::ios_base& left(ios_base& str) {
        return native_std::left(str);
    }

    inline bsl::ios_base& oct(ios_base& str) {
        return native_std::oct(str);
    }

    inline bsl::ios_base& right(ios_base& str) {
        return native_std::right(str);
    }

    inline bsl::ios_base& scientific(ios_base& str) {
        return native_std::scientific(str);
    }

    inline bsl::ios_base& showbase(ios_base& str) {
        return native_std::showbase(str);
    }

    inline bsl::ios_base& showpoint(ios_base& str) {
        return native_std::showpoint(str);
    }

    inline bsl::ios_base& showpos(ios_base& str) {
        return native_std::showpos(str);
    }

    inline bsl::ios_base& skipws(ios_base& str) {
        return native_std::skipws(str);
    }

    inline bsl::ios_base& unitbuf(ios_base& str) {
        return native_std::unitbuf(str);
    }

    inline bsl::ios_base& uppercase(ios_base& str) {
        return native_std::uppercase(str);
    }

    inline bsl::ios_base& noboolalpha(ios_base& str) {
        return native_std::noboolalpha(str);
    }

    inline bsl::ios_base& noshowbase(ios_base& str) {
        return native_std::noshowbase(str);
    }

    inline bsl::ios_base& noshowpoint(ios_base& str) {
        return native_std::noshowpoint(str);
    }

    inline bsl::ios_base& noshowpos(ios_base& str) {
        return native_std::noshowpos(str);
    }

    inline bsl::ios_base& noskipws(ios_base& str) {
        return native_std::noskipws(str);
    }

    inline bsl::ios_base& nounitbuf(ios_base& str) {
        return native_std::nounitbuf(str);
    }

    inline bsl::ios_base& nouppercase(ios_base& str) {
        return native_std::nouppercase(str);
    }
#endif
}  // close package namespace

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
