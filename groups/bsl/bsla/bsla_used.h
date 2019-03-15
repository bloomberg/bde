// bsla_used.h                                                        -*-C++-*-
#ifndef INCLUDED_BSLA_USED
#define INCLUDED_BSLA_USED

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macro to prevent elision of unused entities.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_USED: emit annotated entity even if not referenced
//
//  BSLA_USED_IS_ACTIVE
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macro that will
// guarantee the elision of a local function, type, or variable whether it is
// used or not.
//
// The macro 'BSLA_USED_IS_ACTIVE' is defined to 0 if 'BSLA_USED' expands to
// nothing and 1 otherwise.
//
///Macro
///-----
//..
//  BSLA_USED
//..
// This annotation indicates that the so-annotated function, variable, or type
// must be emitted even if it appears that the variable is not referenced.
//
///Usage
///-----

#include <bsls_platform.h>

// Note that we could conceivably migrate this to use '[[maybe_unused]]' when
// available, but that has more specific constraints over where it can be
// syntactically placed than the older vendor annotations.

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_USED       __attribute__((__used__))

    #define BSLA_USED_IS_ACTIVE 1
#else
    #define BSLA_USED

    #define BSLA_USED_IS_ACTIVE 0
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
