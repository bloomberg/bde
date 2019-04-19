// bsla_unreachable.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLA_UNREACHABLE
#define INCLUDED_BSLA_UNREACHABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compiler-hint macro to indicate unreachable code.
//
//@MACROS:
//  BSLA_UNREACHABLE: indicate that a statement is intended to be not reached
//  BSLA_UNREACHABLE_IS_ACTIVE: 1 if 'BSLA_UNREACHABLE' is active, 0 otherwise
//
//@SEE_ALSO: bsla_annotations
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macro that hints to the
// compile that a statement in the code is intended to be unreachable.  Note
// that the instance of 'BSLA_UNREACHABLE' must be followed by a ';' and is a
// statement in its own right.
//
///Macro Reference
///---------------
//: 'BSLA_UNREACHABLE'
//:     This macro will, when used and followed by a semicolon, create a
//:     statement that emits no code, but that is indicated to be unreachable,
//:     causing compilers, where supported, to issue warnings if there is
//:     actually a way that the code can be reached.  Note that the behavior is
//:     undefined if control actually reaches a 'BSLA_UNREACHABLE' statement.
//
//: 'BSLA_UNREACHABLE_IS_ACTIVE'
//:     The macro 'BSLA_UNREACHABLE_IS_ACTIVE' is defined to 0 if
//:     'BSLA_UNREACHABLE' expands to nothing and 1 otherwise.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Indicating That a Statement is Intended to be Unreachable
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we define a function' 'killSelf', that dumps core.  The function is
// intended never to return, and we indicate that by annotating the function
// with 'BSLA_NORETURN'.  The function dumps core by calling 'BSLS_ASSERT_OPT'
// on a 'false' value, which the compiler doesn't understand will never return:
//..
//  BSLA_NORETURN void killSelf()
//  {
//      BSLS_ASSERT_OPT(false && "dump core");
//  }
//..
// Then, we observe the compiler warning because the compiler expects the
// 'BSLA_ASSERT_OPT' to return and the function, marked 'BSLA_NORETURN', to
// return:
//..
//  .../bsla_unreachable.t.cpp:128:5: warning: function declared 'noreturn'
//  should not return [-Winvalid-noreturn]
//  }
//  ^
//..
// Now, we put a 'BSLA_UNREACHABLE' statement after the 'BSLS_ASSERT_OPT',
// which tells the compiler that that point in the code is unreachable:
//..
//  BSLA_NORETURN void killSelf()
//  {
//      BSLS_ASSERT_OPT(false && "dump core");
//
//      BSLA_UNREACHABLE;
//  }
//..
// Finally, we observe that the compiler warning is silenced.

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_UNREACHABLE __builtin_unreachable()

    #define BSLA_UNREACHABLE_IS_ACTIVE 1
#else
    #define BSLA_UNREACHABLE

    #define BSLA_UNREACHABLE_IS_ACTIVE 0
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
