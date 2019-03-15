// bsla_null.h                                                        -*-C++-*-
#ifndef INCLUDED_BSLA_NULL
#define INCLUDED_BSLA_NULL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macros for use with null-terminated variadic fucntions.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_NULL_TERMINATED: warn if last argument is non-NULL
//  BSLA_NULL_TERMINATED_AT(x): warn if argument at 'x' is non-NULL
//
//  BSLA_NULL_TERMINATED_IS_ACTIVE
//  BSLA_NULL_TERMINATED_AT_IS_ACTIVE
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macro that indicates
// that a variadic function's arguments are terminated by 0 value, or, in the
// case 'BSLA_NULL_TERMINATED_AT', by a 0 value at a certain index, where the
// index starts with 0 as the last argument of the function.
//
// The '*_IS_ACTIVE' macros are defined to 0 if their corresponding macros
// expand to nothing and 1 otherwise.
//
///Macros
///------
//..
//  BSLA_NULL_TERMINATED
//  BSLA_NULL_TERMINATED_AT(x)
//..
// This annotation ensures that a parameter in a function call is an explicit
// 'NULL'.  The annotation is valid only on variadic functions.  By default,
// the sentinel is located at position 0, the last parameter of the function
// call.  If an optional position is specified, the sentinel must be located at
// that index, counting backwards from the end of the argument list.
//
///Usage
///-----

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_NULL_TERMINATED         __attribute__((__sentinel__))
    #define BSLA_NULL_TERMINATED_AT(x)   __attribute__((__sentinel__(x)))

    #define BSLA_NULL_TERMINATED_IS_ACTIVE    1
    #define BSLA_NULL_TERMINATED_AT_IS_ACTIVE 1
#else
    #define BSLA_NULL_TERMINATED
    #define BSLA_NULL_TERMINATED_AT(x)

    #define BSLA_NULL_TERMINATED_IS_ACTIVE    0
    #define BSLA_NULL_TERMINATED_AT_IS_ACTIVE 0
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
