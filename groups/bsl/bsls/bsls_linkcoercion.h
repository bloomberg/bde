// bsls_linkcoercion.h                                                -*-C++-*-
#ifndef INCLUDED_BSLS_LINKCOERCION
#define INCLUDED_BSLS_LINKCOERCION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a way to force a link-time dependency into an object.
//
//@MACROS:
//  BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY
//
//@DESCRIPTION: This component provides a facility to force a link-time
// dependency on a symbol of the name supplied to the macro into the
// translation unit being compiled. This macro can be useful for coercing the
// linker to incorporate version symbols for a library into resulting binary,
// and generating link-time failures if a library version mismatch occurs.
//
///Macro Summary
///-------------
// This section provides a brief description of the macros defined in this
// component.
//..
//   BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY
//       This macro forces a link-time dependency into an object.
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Force a Link-Time Dependency on 's_coerce'
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, declare a variable 's_coerce' of type 'const char *':
//..
//  extern const char *s_coerce;
//..
// Then, use 'BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY' to add a dependency on
// this symbol into any object which includes the header:
//..
//  BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(const char *,
//                                            bsls_coerceexample_coerce,
//                                            s_coerce)
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

#if  (defined(BSLS_PLATFORM_OS_LINUX)                                         \
      && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)))\
  || defined(BSLS_PLATFORM_OS_DARWIN)
#define BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(type,             \
                                                  refName,          \
                                                  referredSymbol)   \
    static type *refName __attribute__((used)) = &referredSymbol;
#elif defined(BSLS_PLATFORM_CMP_IBM)
#define BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(type,             \
                                                  refName,          \
                                                  referredSymbol)   \
    static type *refName = &referredSymbol;
#else
#define BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(type,             \
                                                  refName,          \
                                                  referredSymbol)   \
    namespace {                                                     \
        extern type *const refName = &referredSymbol;               \
    }
#endif

}  // close enterprise namespace

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
