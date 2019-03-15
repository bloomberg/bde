// bsla_scanf.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLA_SCANF
#define INCLUDED_BSLA_SCANF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macro for checking 'scanf'-style format strings.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_SCANF(s, n): validate 'scanf' format and arguments
//
//  BSLA_SCANF_IS_ACTIVE
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component a preprocessor macro that indicates that one of
// the arguments to a function is a 'scanf'-style format string, and that the
// arguments starting at a certain index are to be checked for compatibility
// with that format string.
//
// The macro 'BSLA_SCANF_IS_ACTIVE' is defined to 0 if 'BSLA_SCANF' expands to
// nothing and 1 otherwise.
//
///Macro
///-----
//..
//  BSLA_SCANF(stringIndex, firstToCheck)
//..
// These annotations perform additional compile-time checks on so-annotated
// functions that take 'scantf'-style arguments, which should be type-checked
// against a format string.
//
// The 'stringIndex' parameter is the one-based index to the 'const' format
// string.  The 'firstToCheck' parameter is the one-based index to the first
// variable argument to type-check against that format string.  For example:
//..
//  extern int my_scantf(void *obj, const char *format, ...) BSLA_SCANF(2, 3);
//..
//
///Usage
///-----

#include <bsls_platform.h>
#include <bsls_compilerfeatures.h>

#if defined(BSLS_PLATFORM_CMP_GNU)   ||                                      \
    defined(BSLS_PLATFORM_CMP_CLANG) ||                                      \
    defined(BSLS_PLATFORM_CMP_HP)    ||                                      \
    defined(BSLS_PLATFORM_CMP_IBM)
    #define BSLA_SCANF(fmt, arg) __attribute__((format(scanf,  fmt, arg)))

    #define BSLA_SCANF_IS_ACTIVE 1
#else
    #define BSLA_SCANF(fmt, arg)

    #define BSLA_SCANF_IS_ACTIVE 0
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
