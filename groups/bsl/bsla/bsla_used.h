// bsla_used.h                                                        -*-C++-*-
#ifndef INCLUDED_BSLA_USED
#define INCLUDED_BSLA_USED

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro to prevent elision of unused entities.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_USED: emit annotated entity even if not referenced
//  BSLA_USED_IS_ACTIVE: 0 of 'BSLA_USED' expands to nothing and 1 otherwise
//
//@SEE ALSO: bsla_annotations
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macro that will
// guarantee the emisssion of a local function, type, or variable whether it is
// used or not.
//
///Macro Reference
///---------------
//: o BSLA_USED
//:
//: o This annotation indicates that the so-annotated function, variable, or
//:   type must be emitted even if it appears that the variable is not
//:   referenced.
//
//: o BSLA_USED_IS_ACTIVE
//:
//: o The macro 'BSLA_USED_IS_ACTIVE' is defined to 0 if 'BSLA_UNUSED' expands
//:   to nothing and 1 otherwise.
//
///Usage
///-----
//
///Example 1: Unused variables:
///- - - - - - - - - - - - - -
// First, we declare two unused static variables, one marked 'BSLA_UNUSED'
// and the other marked 'BSLA_USED'.
//..
//  static
//  int usage_UNUSED_variable_no_warning BSLA_UNUSED;
//
//  static
//  int usage_USED_variable_no_warning BSLA_USED;
//..
// Finally, if we compile with clang and go into the debugger and stop in
// 'main' which is in the same file and from which both variables are visible,
// we observe that the variable marked 'BSLA_UNUSED' cannot be accessed, but
// the the variable marked 'BSLA_USED' can.
//..
//
///Example 2: Unused functions:
///- - - - - - - - - - - - - -
// First declare two unused static functions, one marked 'BSLA_UNUSED' and one
// marked 'BSLA_USED':
//..
//  static
//  void usage_UNUSED_function_no_warning(int woof) BSLA_UNUSED;
//      // Print the specified 'woof'.
//  static
//  void usage_UNUSED_function_no_warning(int woof)
//  {
//      printf("%d\n", woof);
//  }
//
//  static
//  void usage_USED_function_no_warning(int woof) BSLA_USED;
//      // Print the specified 'woof'.
//  static
//  void usage_USED_function_no_warning(int woof)
//  {
//      printf("%d\n", woof);
//  }
//..
// Finally, if we compile with clang and go into the debugger, we find that
// we can put a breakpoint in the function marked 'BSLA_USED', but not in
// the function marked 'BSLA_UNUSED'.

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
