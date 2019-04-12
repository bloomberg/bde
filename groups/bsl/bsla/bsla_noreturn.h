// bsla_noreturn.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLA_NORETURN
#define INCLUDED_BSLA_NORETURN

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro to issue a compiler warning if a function returns.
//
//@MACROS:
//  BSLA_NORETURN: issue a compiler warning if function returns normally
//  BSLA_NORETURN_IS_ACTIVE: 1 if 'BSLA_NORETURN' is active and 0 otherwise
//
//@SEE_ALSO: bsla_annotations
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macro that annotates a
// function as never returning, resulting in a compiler warning if a path of
// control exists such that the function does return.
//
///Macro Reference
///---------------
//: 'BSLA_NORETURN'
//:     This annotation is used to tell the compiler that a specified function
//:     will not return in a normal fashion.  The function can still exit via
//:     other means such as throwing an exception or aborting the process.
//
//: 'BSLA_NORETURN_IS_ACTIVE'
//:     The macro 'BSLA_NORETURN_IS_ACTIVE' is defined to 0 if 'BSLA_NORETURN'
//:     expands to nothing and 1 otherwise.
//
///Usage
///-----
//
///Example 1: Assertion Handler
/// - - - - - - - - - - - - - -
// First, we create an assertion handler, 'myHandlerA', which never returns,
// and annotate it with 'BSLA_NORETURN' so that the compiler will warn if it
// does return:
//..
//  BSLA_NORETURN void myHandlerA(const bsls::AssertViolation& assertViolation)
//  {
//      printf("%s:%d %s", assertViolation.fileName(),
//                         assertViolation.lineNumber(),
//                         assertViolation.comment());
//
//      if      (::getenv("MY_HANDLER_THROW")) {
//          throw -1;
//      }
//      else if (::getenv("MY_HANDLER_EXIT")) {
//          ::exit(1);
//      }
//
//      ::abort();
//  }
//..
// Now, a new hire copies 'myHandlerA' and creates a new handler, 'myHandlerB',
// that doesn't abort unless instructed to via an environment variable, not
// realizing that this opens up the possibility of the handler returning:
//..
//  BSLA_NORETURN void myHandlerB(const bsls::AssertViolation& assertViolation)
//  {
//      printf("%s:%d %s", assertViolation.fileName(),
//                         assertViolation.lineNumber(),
//                         assertViolation.comment());
//
//      if      (::getenv("MY_HANDLER_THROW")) {
//          throw -1;
//      }
//      else if (::getenv("MY_HANDLER_EXIT")) {
//          ::exit(1);
//      }
//      else if (::getenv("MY_HANDLER_ABORT")) {
//          ::abort();
//      }
//  }
//..
// Finally, we observe the compiler warning that occurs to point out the
// possibility of 'myHandlerB' returning:
//..
//  .../bsla_noreturn.t.cpp:157:5: warning: function declared 'noreturn' should
//  not return [-Winvalid-noreturn]
//  }
//  ^
//..

#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN)
    #define BSLA_NORETURN [[ noreturn ]]

    #define BSLA_NORETURN_IS_ACTIVE 1
#elif defined(BSLS_PLATFORM_CMP_MSVC)
    #define BSLA_NORETURN __declspec(noreturn)

    #define BSLA_NORETURN_IS_ACTIVE 1
#else
    #define BSLA_NORETURN

    #define BSLA_NORETURN_IS_ACTIVE 0
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
