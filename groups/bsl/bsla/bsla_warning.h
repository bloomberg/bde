// bsla_warning.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLA_WARNING
#define INCLUDED_BSLA_WARNING

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro to flag a warning when a function is called.
//
//@CLASSES:
//
//@MACROS:
//  BSLA_WARNING(QUOTED_MESSAGE): emit warning message during compilation
//  BSLA_WARNING_IS_ACTIVE: 0 if 'BSLA_WARNING' expands to nothing else 1
//
//@SEE ALSO: bsla_annotations
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a macro that indicates that a compiler
// warning should be emitted when a given function is called.
//
///Macro
///-----
//: o BSLA_WARNING(QUOTED_MESSAGE)
//:
//: o This annotation, when used, will cause a compile-time warning containing
//:   the specified 'QUOTED_MESSAGE', which must be a string contained in
//:   double quotes, when a call to the so-annotated function is not removed
//:   through dead-code elimination or other optimizations.  While it is
//:   possible to leave the function undefined, thus incurring a link-time
//:   failure, with the use of this macro the invalid call will be diagnosed
//:   earlier (i.e., at compile time), and the diagnostic will include the
//:   exact location of the function call.
//
//: o BSLA_WARNING_IS_ACTIVE
//:
//: o The macro 'BSLA_WARNING_IS_ACTIVE' is defined to 0 if 'BSLA_WARNING'
//:   expands to nothing and 1 otherwise.
//
///Usage
///-----
//
///Example 1: Flagged Function:
///- - - - - - - - - - - - - -
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
//  usageFunc();
//..
// Finally, observe the following compile error:
//..
//  .../bsla_warning.t.cpp: In function 'int main(int, char**)':
//  .../bsla_warning.t.cpp:246:16: warning: call to 'usageFunc' declared with a
//  ttribute warning: Don't call 'usageFunc'
//       usageFunc();
//                  ^
//..

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_GNU)
    // The '__warning__' attribute is not supported by clang as of version
    // 8.0.0.

    #define BSLA_WARNING(x) __attribute__((__warning__(x)))

    #define BSLA_WARNING_IS_ACTIVE 1
#else
    #define BSLA_WARNING(x)

    #define BSLA_WARNING_IS_ACTIVE 0
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
