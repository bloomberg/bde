// bsla_printf.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLA_PRINTF
#define INCLUDED_BSLA_PRINTF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macro to indicate 'printf'-style arguments.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_PRINTF(s, n): validate 'printf' format and arguments
//
//  BSLA_PRINTF_IS_ACTIVE
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides apreprocessor macro that allows the
// designation of a given function argument as a 'printf'-style format string,
// and arguments starting at a certain index as the argument list to be
// formatted according to that string.
//
// The macro 'BSLA_PRINTF_IS_ACTIVE' is defined to 0 on platforms or compilers
// where 'BSLA_PRINTF' expands to nothing, and 1 otherwise.
// 'BSLS_PLATFORM_CMP_GNU' or 'BSLS_PLATFORM_CMP_CLANG' is defined.
//
///Macros
///------
//..
//  BSLA_PRINTF(stringIndex, firstToCheck)
//..
// These annotations perform additional compile-time checks on so-annotated
// functions that take 'printf'-style arguments, which should be type-checked
// against a format string.
//
// The 'stringIndex' parameter is the one-based index to the 'const' format
// string.  The 'firstToCheck' parameter is the one-based index to the first
// variable argument to type-check against that format string.  For example:
//..
//  extern int my_printf(void *obj, const char *format, ...) BSLA_PRINTF(2, 3);
//..
//
///Usage
///-----

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_GNU)   ||                                      \
    defined(BSLS_PLATFORM_CMP_CLANG) ||                                      \
    defined(BSLS_PLATFORM_CMP_HP)    ||                                      \
    defined(BSLS_PLATFORM_CMP_IBM)
    #define BSLA_PRINTF(fmt, arg) __attribute__((format(printf, fmt, arg)))

    #define BSLA_PRINTF_IS_ACTIVE 1
#else
    #define BSLA_PRINTF(fmt, arg)

    #define BSLA_PRINTF_IS_ACTIVE 0
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
