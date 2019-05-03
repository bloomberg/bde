// bsla_deprecated.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLA_DEPRECATED
#define INCLUDED_BSLA_DEPRECATED

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compiler-hint macro to indicate deprecated entities.
//
//@MACROS:
//  BSLA_DEPRECATED: warn if annotated (deprecated) entity is used
//  BSLA_DEPRECATED_IS_ACTIVE: 1 if 'BSLA_DEPRECATED' is active, 0 otherwise
//
//@SEE_ALSO: bsla_annotations
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a preprocessor macro that hints to the
// compile that a function, variable, or type is deprecated.
//
///Macro Reference
///---------------
//: 'BSLA_DEPRECATED'
//:     This annotation will, when used, cause a compile-time warning if the
//:     so-annotated function, variable, or type is used anywhere within the
//:     source file.  This is useful, for example, when identifying functions
//:     that are expected to be removed in a future version of a library.  The
//:     warning includes the location of the declaration of the deprecated
//:     entity to enable users to find further information about the
//:     deprecation, or what they should use instead.
//
//: 'BSLA_DEPRECATED_IS_ACTIVE'
//:     The macro 'BSLA_DEPRECATED_IS_ACTIVE' is defined to 0 if
//:     'BSLA_DEPRECATED' expands to nothing and 1 otherwise.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Deprecating a Type, a Function, and a Variable
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we define a deprecated type 'UsageType':
//..
//  struct UsageType {
//      int d_int;
//  } BSLA_DEPRECATED;
//..
// Then, we define a deprecated function 'usageFunc':
//..
//  void usageFunc() BSLA_DEPRECATED;
//  void usageFunc()
//  {
//      printf("Don't call me.\n");
//  }
//..
// Next, we define a deprecated variable 'usageVar':
//..
//  extern int usageVar BSLA_DEPRECATED;
//  int usageVar = 5;
//..
// Then, as long as we don't use them, no warnings will be issued.
//
// Next, we use 'UsageType':
//..
//  UsageType ut;
//  ut.d_int = 5;
//  (void) ut.d_int;
//..
// which results in the following warnings:
//..
//  .../bsla_deprecated.t.cpp:287:5: warning: 'UsageType' is deprecated
//  [-Wdeprecated-declarations]
//      UsageType ut;
//      ^
//  .../bsla/bsla_deprecated.t.cpp:113:7: note: 'UsageType' has been explicitly
//   marked deprecated here
//      } BSLA_DEPRECATED;
//        ^
//..
// Now, we call 'usageFunc':
//..
//  usageFunc();
//..
// which results in the following warnings:
//..
//  .../bsla_deprecated.t.cpp:309:5: warning: 'usageFunc' is deprecated
//  [-Wdeprecated-declarations]
//      usageFunc();
//      ^
//  .../bsla_deprecated.t.cpp:117:22: note: 'usageFunc' has been explicitly
//  marked deprecated here
//      void usageFunc() BSLA_DEPRECATED;
//                       ^
//..
// Finally, we access 'usageVar':
//..
//  printf("%d\n", usageVar);
//..
// which results in the following warnings:
//..
//  .../bsla_deprecated.t.cpp:329:20: warning: 'usageVar' is deprecated
//  [-Wdeprecated-declarations]
//      printf("%d\n", usageVar);
//                     ^
//  .../bsla_deprecated.t.cpp:125:25: note: 'usageVar' has been explicitly
//  marked deprecated here
//      extern int usageVar BSLA_DEPRECATED;
//                          ^
//..

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_DEPRECATED __attribute__((__deprecated__))

    #define BSLA_DEPRECATED_IS_ACTIVE 1
#else
    #define BSLA_DEPRECATED

    #define BSLA_DEPRECATED_IS_ACTIVE 0
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
