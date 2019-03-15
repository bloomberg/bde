// bsla_format.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLA_FORMAT
#define INCLUDED_BSLA_FORMAT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macro to designate value of function is format string.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_FORMAT(n): validate 'printf' format in 'n'th argument
//
//  BSLA_FORMAT_IS_ACTIVE
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macro that
// indicates that the a certain argument of function it designates is a
// 'printf'-style format string, and that the function will return a 'printf'
// style string with equivalent arguments.
//
// The macro 'BSLA_FORMAT_IS_ACTIVE' is defined to 0 when 'BSLA_FORMAT' expands
// to nothing and 1 otherwise.
//
///Macro
///-----
//..
//  BSLA_FORMAT(stringIndex)
//..
// This annotation specifies that the so-annotated function takes an argument
// parameter that is a valid format string for a 'printf'-style function and
// returns a format string that is consistent with that format.  This allows
// format strings manipulated by translation functions to be checked against
// arguments.  Without this annotation, attempting to manipulate the format
// string via this kind of function might generate warnings about non-literal
// formats.  For example:
//..
//  const char *translateFormat(const char *locale, const char *format)
//                                                   BSLA_FORMAT(2);
//..
// On a conforming compiler, this will validate the "Mike" argument against the
// 'format' specification passed to 'translateFormat'.
//..
//   printf(translateFormat("FR", "Name: %s"), "Mike");
//..
//
///Usage
///-----

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_GNU)   ||                                      \
    defined(BSLS_PLATFORM_CMP_CLANG) ||                                      \
    defined(BSLS_PLATFORM_CMP_IBM)
    #define BSLA_FORMAT(arg) __attribute__((format_arg(arg)))

    #define BSLA_FORMAT_IS_ACTIVE 1
#else
    #define BSLA_FORMAT(arg)

    #define BSLA_FORMAT_IS_ACTIVE 0
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
