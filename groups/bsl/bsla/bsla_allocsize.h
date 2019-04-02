// bsla_allocsize.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLAALLOCSIZE
#define INCLUDED_BSLAALLOCSIZE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macro hints for the size of allocated returned objects.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_ALLOCSIZE(ARGINDEX):                indiciate size of returned memory
//  BSLA_ALLOCSIZEMUL(ARGINDEX1, ARGINDEX2): indiciate size of returned memory
//  BSLA_ALLOCSIZEIS_ACTIVE:     '1' if 'BSLA_ALLOCSIZE' is in effect
//  BSLA_ALLOCSIZEMUL_IS_ACTIVE: '1' if 'BSLA_ALLOCSIZEMUL' is in effect
//
//@SEE ALSO: bsla_annotations
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides preprocessor macros that can be used to
// hint to the compiler the size of dynamically allocated objects returned from
// a function through a pointer return value.
//
///Macro Reference
///---------------
//: o BSLA_ALLOCSIZE(ARGINDEX)
//: o BSLA_ALLOCSIZEMUL(ARGINDEX1, ARGINDEX2)
//
//: o These annotations are used to inform the compiler that the return value
//:   of the so-annotated function is the address of an allocated block of
//:   memory whose size (in bytes) is given by one or two of the function
//:   parameters.  Certain compilers use this information to improve the
//:   correctness of built-in object-size functions (e.g.,
//:   '__builtin_object_size' with 'BSLS_PLATFORM_CMP_GNU' or
//:   'BSLS_PLATFORM_CMP_CLANG').
//:
//: o The function parameter(s) denoting the size of the block are specified by
//:   one or two integer argument indices supplied to the macro, where the
//:   first argument of the annotated function is denoted by '1'.  The
//:   allocated size (in bytes) is either the value of the single function
//:   argument or the product of the two arguments.  For example:
//..
//  void *my_calloc(size_t a, size_t b) BSLA_ALLOCSIZEMUL(1, 2);
//      // The annotation indicates that the size of the allocated memory
//      // returned is 'a * b'.
//  void my_realloc(void *a, size_t b) BSLA_ALLOCSIZE(2);
//      // The annotation indicates that the size of the allocated memory
//      // returned is 'b'.
//..
//: o BSLA_ALLOCSIZEIS_ACTIVE
//: o BSLA_ALLOCSIZEMUL_IS_ACTIVE
//
//: o The '*_IS_ACTIVE' macros are defined to 0 if their corresponding macros
//:   expand to nothing, or to 1 otherwise.
//
///Usage
///-----
//
///Example 1: Communicating to the Compiler via Annotations
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include <bsls_platform.h>

#if BDE_BUILD_TARGET_OPT &&                                                   \
   ((defined(BSLS_PLATFORM_CMP_GNU) &&                                        \
      ((BSLS_PLATFORM_CMP_VERSION >= 40300 &&                                 \
                                          BSLS_PLATFORM_CMP_VERSION < 70000)  \
     || BSLS_PLATFORM_CMP_VERSION >= 90000)) ||                               \
    (defined(BSLS_PLATFORM_CMP_CLANG) && BSLS_PLATFORM_CMP_VERSION >= 80000))

    #define BSLA_ALLOCSIZE(ARGINDEX)                                          \
                      __attribute__((__alloc_size__(ARGINDEX)))
    #define BSLA_ALLOCSIZEMUL(ARGINDEX1, ARGINDEX2)                           \
                      __attribute__((__alloc_size__(ARGINDEX1, ARGINDEX2)))

    #define BSLA_ALLOCSIZE_IS_ACTIVE     1
    #define BSLA_ALLOCSIZEMUL_IS_ACTIVE  1
#else
    #define BSLA_ALLOCSIZE(ARGINDEX)
    #define BSLA_ALLOCSIZEMUL(ARGINDEX1, ARGINDEX2)

    #define BSLA_ALLOCSIZE_IS_ACTIVE     0
    #define BSLA_ALLOCSIZEMUL_IS_ACTIVE  0
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
