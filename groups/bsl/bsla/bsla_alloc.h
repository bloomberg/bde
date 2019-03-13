// bsla_alloc.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLA_ALLOC
#define INCLUDED_BSLA_ALLOC

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macro hints for size of allocated returned objects.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_ALLOC_SIZE(x): optimize when returning memory
//  BSLA_ALLOC_SIZE_MUL(x, y): optimize when returning memory
//
//  BSLA_ALLOC_SIZE_IS_ACTIVE
//  BSLA_ALLOC_SIZE_MUL_IS_ACTIVE
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a pair of preprocessor macros that can
// be used to hint to the compiler the size of dynamically allocated objects
// returned from a function through a pointer return value.
//
///Macros
///------
//..
//  BSLA_ALLOC_SIZE(argIndex)
//  BSLA_ALLOC_SIZE_MUL(argIndex1, argIndex2)
//..
// This annotation is used to inform the compiler that the return value of the
// so-annotated function is the address of an allocated block of memory whose
// size (in bytes) is given by one or two of the function parameters.  Certain
// compilers use this information to improve the correctness of built-in
// object-size functions (e.g., '__builtin_object_size' with
// 'BSLS_PLATFORM_CMP_GNU' or 'BSLS_PLATFORM_CMP_CLANG').
//
// The function parameter(s) denoting the size of the block are specified by
// one or two integer argument indeces supplied to the macro, where the first
// argument is denoted by '1'.  The allocated size (in bytes) is either the
// value of the single function argument or the product of the two arguments.
// Argument numbering starts at one.  For example:
//..
//  void *my_calloc(size_t a, size_t b) BSLA_ALLOC_SIZE_MUL(1, 2);
//      // The attribute indicates that the size of the allocated memory
//      // returned is 'a * b'.
//  void my_realloc(void * a, size_t b) BSLA_ALLOC_SIZE(2);
//      // The attribute indicates that the size of the allocated memory
//      // returned is 'b'.
//..
// The '*_IS_ACTIVE' macros are defined to 0 if their corresponding macros
// expand to nothing, or to 1 otherwise.
//
///Usage
///-----

#include <bsls_platform.h>

#if (defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 40300) || \
    defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_ALLOC_SIZE(argIndex)                                         \
                          __attribute__((__alloc_size__(argIndex)))
    #define BSLA_ALLOC_SIZE_MUL(argIndex1, argIndex2)                         \
                          __attribute__((__alloc_size__(argIndex1, argIndex2)))

    #define BSLA_ALLOC_SIZE_IS_ACTIVE     1
    #define BSLA_ALLOC_SIZE_MUL_IS_ACTIVE 1
#else
    #define BSLA_ALLOC_SIZE(x)
    #define BSLA_ALLOC_SIZE_MUL(x, y)

    #define BSLA_ALLOC_SIZE_IS_ACTIVE     0
    #define BSLA_ALLOC_SIZE_MUL_IS_ACTIVE 0
#endif

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
