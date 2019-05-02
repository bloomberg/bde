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
// First, we define a function 'directoriesInPath' that counts the number of
// directories in the '$PATH' environment variable.  If '$PATH' is not set, it
// dumps core by calling 'BSLS_ASSERT_OPT':
//..
//  int directoriesInPath()
//  {
//      const char *path = ::getenv("PATH");
//      if (path) {
//          int ret = 1;
//          for (; *path; ++path) {
//              ret += ':' == *path;
//          }
//          return ret;                                               // RETURN
//      }
//
//      BSLS_ASSERT_OPT(false && "dump core");
//  }
//..
// Then, we observe the compile error because the compiler expects the
// 'BSLA_ASSERT_OPT' to return and the function, which returns an 'int', to run
// off the end without returning anything, causing the following error message
// on Windows
//..
//  .../bsla_unreachable.t.cpp(141) : error C4715: 'directoriesInPath': not all
//  control paths return a value
//..
// Now, we put a 'BSLA_UNREACHABLE' statement after the 'BSLS_ASSERT_OPT',
// which tells the compiler that that point in the code is unreachable:
//..
//  int directoriesInPath()
//  {
//      const char *path = ::getenv("PATH");
//      if (path) {
//          int ret = 1;
//          for (; *path; ++path) {
//              ret += ':' == *path;
//          }
//          return ret;                                               // RETURN
//      }
//
//      BSLS_ASSERT_OPT(false && "dump core");
//
//      BSLA_UNREACHABLE;
//  }
//..
// Finally, we observe that the compiler error is silenced and the build is
// successful.

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_UNREACHABLE __builtin_unreachable()

    #define BSLA_UNREACHABLE_IS_ACTIVE 1
#elif defined(BSLS_PLATFORM_CMP_MSVC)
    #define BSLA_UNREACHABLE __assume(false)

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
