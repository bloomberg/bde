// bsla_warning.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLA_WARNING
#define INCLUDED_BSLA_WARNING

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide support for compiler annotations for compile-time safety.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_WARNING("msg"): emit warning message during compilation
//
//  BSLA_WARNING_IS_ACTIVE
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides amacros that indicates that a compiler
// warning should be emitted when a given function is called.
//
// The macro 'BSLA_WARNING_IS_ACTIVE' is defined to 0 when 'BSLA_WARNING'
// expands ot nothing and 1 otherwise.
//
///Macro
///-----
//..
//  BSLA_WARNING("message")
//..
// This annotation, when used, will cause a compile-time ewarning when a call
// to the so-annotated function is not removed through dead-code elimination or
// other optimizations.  While it is possible to leave the function undefined,
// thus incurring a link-time failure, with the use of this macro the invalid
// call will be diagnosed earlier (i.e., at compile time), and the diagnostic
// will include the exact location of the function call.
//
///Usage
///-----

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_GNU)
    // '__error__' and '__warning__' attributes are not supported by clang as
    // of version 7.0.

    #define BSLA_WARNING(x) __attribute__((__warning__(x)))

    #define BSLA_WARNING_IS_ACTIVE 1
#else
    #define BSLA_WARNING(x)

    #define BSLA_WARNING_IS_ACTIVE 0
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
