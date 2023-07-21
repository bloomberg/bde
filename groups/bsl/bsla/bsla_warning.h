// bsla_warning.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLA_WARNING
#define INCLUDED_BSLA_WARNING

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro to emit a warning when a function is called.
//
//@MACROS:
//  BSLA_WARNING(QUOTED_MESSAGE): emit warning message during compilation
//  BSLA_WARNING_IS_ACTIVE:       defined if 'BSLA_WARNING' is active
//
//@SEE_ALSO: bsla_annotations
//
//@DESCRIPTION: This component provides a macro that indicates that a compiler
// warning should be emitted when a given function is called.
//
///Macro Reference
///---------------
//: 'BSLA_WARNING(QUOTED_MESSAGE)':
//:     This annotation, when used, will cause a compile-time warning
//:     containing the specified 'QUOTED_MESSAGE', which must be a string
//:     contained in double quotes, when a call to the so-annotated function is
//:     not removed through dead-code elimination or other optimizations.
//:     While it is possible to leave the function undefined, thus incurring a
//:     link-time failure, with the use of this macro the invalid call will be
//:     diagnosed earlier (i.e., at compile time), and the diagnostic will
//:     include the location of the function call.
//:
//: 'BSLA_WARNING_IS_ACTIVE':
//:     The macro 'BSLA_WARNING_IS_ACTIVE' is defined if 'BSLA_WARNING' expands
//:     to something with the desired effect; otherwise
//:     'BSLA_WARNING_IS_ACTIVE' is not defined and 'BSLA_WARNING' expands to
//:     nothing.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Function Annotated with 'BSLA_WARNING'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// First, we declare and define a function annotated with 'BSLA_WARNING'.  Note
// that the argument to 'BSLA_WARNING' must be a quoted string:
//..
//  void usageFunc() BSLA_WARNING("Don't call 'usageFunc'");
//      // Do nothing.
//
//  void usageFunc()
//  {
//  }
//..
// Now, in 'main', we call 'usageFunc':
//..
//      usageFunc();
//..
// Finally, observe the following warning issued by the compiler:
//..
//  .../bsla_warning.t.cpp: In function 'int main(int, char**)':
//  .../bsla_warning.t.cpp:246:16: warning: call to 'usageFunc' declared with
//  attribute warning: Don't call 'usageFunc'
//       usageFunc();
//                  ^
//..

#include <bsls_platform.h>

                       // =============================
                       // Checks for Pre-Defined macros
                       // =============================

#if defined(BSLA_WARNING)
#error BSLA_WARNING is already defined!
#endif

#if defined(BSLA_WARNING_IS_ACTIVE)
#error BSLA_WARNING_IS_ACTIVE is already defined!
#endif

                       // =========================
                       // Set macros as appropriate
                       // =========================

#if defined(BSLS_PLATFORM_CMP_GNU)
    // The '__warning__' attribute is not supported by clang as of version
    // 8.0.0.

    #define BSLA_WARNING(x) __attribute__((__warning__(x)))

    #define BSLA_WARNING_IS_ACTIVE 1
#else
    #define BSLA_WARNING(x)
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
