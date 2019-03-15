// bsla_unused.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLA_UNUSED
#define INCLUDED_BSLA_UNUSED

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro to suppress 'unused' warnings.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_UNUSED: do not warn if annotated entity is unused
//
//  BSLA_UNUSED_IS_ACTIVE
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macro that
// will suppress 'unused' warnings on a locally defined function, type, or
// variable that is unused.
//
// The macro 'BSLA_UNUSED_IS_ACTIVE' is defined to 0 if 'BSLA_UNUSED' expands
// to nothing and 1 otherwise.
//
///Annotations
///-----------
//..
//  BSLA_UNUSED
//..
// This annotation indicates that the so-annotated function, variable, or type
// is possibly unused and the compiler should not generate a warning for the
// unused identifier.
//
///Usage
///-----

#include <bsls_platform.h>

// Note that we could conceivably migrate this to use '[[maybe_unused]]' when
// available, but that has more specific constraints over where it can be
// syntactically placed than the older vendor annotations.

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_UNUSED     __attribute__((__unused__))

    #define BSLA_UNUSED_IS_ACTIVE 1
#else
    #define BSLA_UNUSED

    #define BSLA_UNUSED_IS_ACTIVE 0
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
