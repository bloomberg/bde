// bsls_linkcoercion.h                                                -*-C++-*-
#ifndef INCLUDED_BSLS_LINKCOERCION
#define INCLUDED_BSLS_LINKCOERCION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a way to force a link-time reference into an object.
//
//@MACROS:
// BSLS_LINKCOERCION_INCLUDE_REF
//
//@DESCRIPTION: This component provides a way to force a link-time reference
// to a symbol of type 'const char *' into the object being compiled.
//
///Usage
///-----
// A component that wants to make sure clients link in the correct version
// of the corresponding library could declare a coercion symbol in its header.
// First, declare a 'const char *' variable as follows:
//..
//  extern const char *s_coerce;
//..
// Next, use BSLS_LINKCOERCION_INCLUDE_REF to force a reference to this symbol
// into any object which includes the header:
//..
// BSLS_LINKCOERCION_INCLUDE_REF(bsls_coerceexample_coerce, s_coerce)
//..
// Finally, in the corresponding '.cpp' file, the 's_coerce' symbol needs to be
// defined:
//..
//  const char *s_coerce = "Link me in!";
//..

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {

#if defined(BSLS_PLATFORM_OS_LINUX) && defined(BSLS_PLATFORM_CMP_GNU)
#define BSLS_LINKCOERCION_INCLUDE_REF(refName, referredSymbol)   \
    static const char **refName __attribute__((used)) = &referredSymbol;
#elif defined(BSLS_PLATFORM_CMP_IBM)
#define BSLS_LINKCOERCION_INCLUDE_REF(refName, referredSymbol) \
    static const char **refName = &referredSymbol;
#else
#define BSLS_LINKCOERCION_INCLUDE_REF(refName, referredSymbol) \
    namespace {                                                \
        extern const char **const refName = &referredSymbol;   \
    }
#endif

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
