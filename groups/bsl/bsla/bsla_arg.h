// bsla_arg.h                                                         -*-C++-*-
#ifndef INCLUDED_BSLA_ARG
#define INCLUDED_BSLA_ARG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macros to hint at null arguments to functions.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_ARG_NON_NULL(...): warn if listed arguments are NULL
//  BSLA_ARGS_NON_NULL: warn if any arguments are NULL
//
//  BSLA_ARG_NON_NULL_IS_ACTIVE
//  BSLA_ARGS_NON_NULL_IS_ACTIVE
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a pair of preprocessor macros that
// define compiler-specific compile-time annotations.  These macros instruct
// the compiler to warning if null values are passed to certain arguments to
// a function, or, on platforms where the feature is not supported, expand to
// nohting.  The '*_IS_ACTIVE' macros are defined to 0 if their corresponding
// macros expand to nothing and to 1 otherwise.
//
///Macros
///------
//..
//  BSLA_ARG_NON_NULL(...)
//  BSLA_ARGS_NON_NULL
//..
// These annotations are used to tell the compiler that a function argument
// must not be null.  If the compiler determines that a null pointer is passed
// to an argument slot marked by this annotation, a warning is issued.  If the
// 'BSLA_ARG_NON_NULL' annotation is used, it expects a variable list of
// argument indexes to be specified, where the first argument has an index of
// 1.  An argument slot is a one-based index of the argument in the function
// parameters.  The 'BSLA_ARGS_NON_NULL' annotation specifies that all pointer
// arguments must not be null.
//..
//  void my_function(void *a, void *b, void *c) BSLA_ARG_NON_NULL(2, 3);
//      // The attribute indicates that 'b' and 'c' must not be null.
//  void my_function(void *a, void *b, void *c) BSLA_ARGS_NON_NULL;
//      // The attribute indicates that 'a, 'b' and 'c' must not be null.
//..
//
///Usage
///-----

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_ARG_NON_NULL(...)   __attribute__((__nonnull__(__VA_ARGS__)))
    #define BSLA_ARGS_NON_NULL       __attribute__((__nonnull__))

    #define BSLA_ARG_NON_NULL_IS_ACTIVE  1
    #define BSLA_ARGS_NON_NULL_IS_ACTIVE 1
#else
    #define BSLA_ARG_NON_NULL(...)
    #define BSLA_ARGS_NON_NULL

    #define BSLA_ARG_NON_NULL_IS_ACTIVE  0
    #define BSLA_ARGS_NON_NULL_IS_ACTIVE 0
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
