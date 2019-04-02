// bsla_nonnullarg.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLA_NONNULLARG
#define INCLUDED_BSLA_NONNULLARG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macros to hint at null arguments to functions.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_NONNULLARGS: warn if any arguments are NULL
//  BSLA_NONNULLARG(...): warn if indexed arguments are NULL
//  BSLA_NONNULLARGS_IS_ACTIVE: 1 if 'BSLA_NONNULLARGS' is active, 0 otherwise
//  BSLA_NONNULLARG_IS_ACTIVE:  1 if 'BSLA_NONNULLARG' is active, 0 otherwise
//
//@SEE ALSO: bsla_annotations
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides preprocessor macros that define
// compiler-specific compile-time annotations.  These macros instruct the
// compiler to warn if null values are passed to certain arguments to a
// function, or, on platforms where the feature is not supported, expand to
// nothing.
//
///Macros
///------
//: o BSLA_NONNULLARGS
//:
//: o This annotation indicates that a compiler warning is to be issued if
//:   any of the pointer arguments to this function are passed null.
//
//: o BSLA_NONNULLARG(...)
//:
//: o This annotation, passed a variable-length list of positive integers,
//:   indicates that a compiler warning is to be issued if null is passed to
//:   a pointer argument at any of the specified indices, where the first
//:   argument has an index of 1.
//
//: o BSLA_NONNULLARG_IS_ACTIVE
//: o BSLS_NONNULLARGS_IS_ACTIVE
//:
//: o In these two cases, 'X_IS_ACTIVE' is defined to 0 if 'X' expands to
//:   nothing and 1 otherwise.
//
///Usage
///-----
//
///Example 1: Passing Null to Arguments Annotated as Non-Null:
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we define a function 'usagePrint1' annotated such that a compiler
// warning will occur if the first argument is null:
//..
//  void usagePrint1(const char *string, int repitition) BSLA_NONNULLARG(1);
//      // Print the specified 'string' the specified 'repitition' times.
///
//  void usagePrint1(const char *string, int repitition)
//  {
//      for (int ii = 0; ii < repitition; ++ii) {
//          printf("%s\n", string);
//      }
//  }
//..
// Then, we define a similar function annotaged with 'BSLA_NONNULLARGS'.  Note
// that only the pointer argument is affected by the annotation.
//..
//  void usagePrint2(const char *string, int repitition) BSLA_NONNULLARGS;
//      // Print the specified 'string' the specified 'repitition' times.
//
//  void usagePrint2(const char *string, int repitition)
//  {
//      for (int ii = 0; ii < repitition; ++ii) {
//          printf("%s\n", string);
//      }
//  }
//..
// Next, in 'main', we call both functions with a non-null first argument, and
// observe that no warning occurs.  Note that even though 0 is passed to the
// integer argument to 'usagePrint2' and the 'BSLA_NONNULLARGS' annotation was
// used, non-pointer arguments are not affected by that annotation.
//..
//  usagePrint1("woof", 0);
//  usagePrint2("meow", 0);
//..
// Now, we call both functions passing a null pointer to the first argument,
// and observe that we get compiler warnings on both calls.
//..
//  usagePrint1(NULL, -10);
//  usagePrint2(NULL, -10);
//..
// Finally, we observe that the above two calls result in the following
// warnings on clang:
//..
//  .../bsla_nonnullarg.t.cpp:359:25: warning: null passed to a callee that req
//  uires a non-null argument [-Wnonnull]
//      usagePrint1(NULL, -10);
//                  ~~~~     ^
//  .../bsla_nonnullarg.t.cpp:360:25: warning: null passed to a callee that req
//  uires a non-null argument [-Wnonnull]
//      usagePrint2(NULL, -10);
//                  ~~~~     ^
//..

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_NONNULLARG(...)   __attribute__((__nonnull__(__VA_ARGS__)))
    #define BSLA_NONNULLARGS       __attribute__((__nonnull__))

    #define BSLA_NONNULLARG_IS_ACTIVE  1
    #define BSLA_NONNULLARGS_IS_ACTIVE 1
#else
    #define BSLA_NONNULLARG(...)
    #define BSLA_NONNULLARGS

    #define BSLA_NONNULLARG_IS_ACTIVE  0
    #define BSLA_NONNULLARGS_IS_ACTIVE 0
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
