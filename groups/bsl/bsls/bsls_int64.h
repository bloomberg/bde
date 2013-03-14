// bsls_int64.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLS_INT64
#define INCLUDED_BSLS_INT64

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide namespace for platform-dependent 64-bit integer types.
//
//@INTERNAL_DEPRECATED: Use component 'bsls_types' instead.
//
//@CLASSES:
//  bsls_Int64: namespace for platform-dependent 64-bit integer types
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@DESCRIPTION: This component provides platform-independent type names for
// both signed and unsigned 64 bit integers.  For healthy platforms, the
// component simply provides a BDE-standard 'typedef' for the non-standard
// native type name.  For defective platforms or compilers, a bde-relevant
// subset of the needed functionality can be implemented.
//
// The types (and platform-specific 'typedef's) exposed here are intended for
// use by 'bsls_platformutil' only (and not directly by any other client).
// This component exists to factor out various pathologies, present and future,
// with 64-bit integers.
//
// !WARNING! If a given platform has a defective native 64-bit integer type,
// then there is no guarantee that the types supplied here will be suitable
// *except* as they are explicitly used by other BDE components.  If a user
// encounters problems with the types, please contact the author or a BDE
// library maintenance engineer.
//
///Usage
///-----
// Do not use this component; use 'bsls_types' instead.

#ifdef BDE_OMIT_TRANSITIONAL // DEPRECATED
#error "bsls_int64 is deprecated"
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

                          // =================
                          // struct bsls_Int64
                          // =================

struct bsls_Int64 {
    // Provide a namespace for the implementations of platform-neutral 64-bit
    // integers.  Namely, this 'struct' defines 'Int64' as a 64-bit signed
    // integer and 'Uint64' as a 64-bit unsigned integer for all supported
    // platforms.

    // TYPES
    typedef bsls::Types::Int64  Int64;
    typedef bsls::Types::Uint64 Uint64;
        // DEPRECATED: Use 'bsls::Types::Int64' and 'bsls::Types::Uint64',
        // respectively, instead.
};

}  // close enterprise namespace

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
