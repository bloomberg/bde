// bsla_deprecated.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLA_DEPRECATED
#define INCLUDED_BSLA_DEPRECATED

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide compiler-hint macros to indicate deprecated entities.
//
//@MACROS:
//  BSLA_DEPRECATED: warn if annotated (deprecated) entity is used
//  BSLA_DEPRECATED_MESSAGE: warn with message if annotated entity is used
//  BSLA_DEPRECATED_IS_ACTIVE: 1 if both macros are active, 0 if both inactive
//
//@SEE_ALSO: bsla_annotations
//
//@AUTHOR: Andrew Paprocki (apaprock), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides preprocessor macros that hint to the
// compiler that a function, variable, type, 'typedef', 'struct' member, 'enum'
// type, or template specialization is deprecated.  This is useful, for
// example, when identifying functions that are expected to be removed in a
// future version of a library.
//
///Macro Reference
///---------------
//: 'BSLA_DEPRECATED'
//:     This annotation will, when used, cause a compile-time warning if the
//:     so-annotated function, variable, type, 'typedef', 'struct' member,
//:     'enum' type, or template specialization is used anywhere within the
//:     source file.  The warning includes the location of the declaration of
//:     the deprecated entity to enable users to find further information about
//:     the deprecation, or what they should use instead.
//
//: 'BSLA_DEPRECATED_MESSAGE(QUOTED_MESSAGE)'
//:     This annotation will, when used, cause a compile-time warning if the
//:     so-annotated function, variable, type, 'typedef', 'struct' member,
//:     'enum' type, or template specialization is used anywhere within the
//:     source file.  The compiler warning will contain the contents of the
//:     specified 'QUOTED_MESSAGE', which must be a double-quoted string.  The
//:     warning includes the location of the declaration of the deprecated
//:     entity to enable users to find further information about the
//:     deprecation, and what they should use instead.  Note that on some
//:     compilers 'message' is ignored.
//
//: 'BSLA_DEPRECATED_IS_ACTIVE'
//:     The macro 'BSLA_DEPRECATED_IS_ACTIVE' is defined to 0 if
//:     'BSLA_DEPRECATED' and 'BSLA_DEPRECATED_MESSAGE' both expand to nothing
//:     and 1 if they are both enabled and have the desired effect.  Either
//:     both of them work or neither works.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Various Deprecations
///- - - - - - - - - - - - - - - -
// First, we define a deprecated type 'UsageType':
//..
//  struct BSLA_DEPRECATED UsageType {
//      int d_int;
//  };
//..
// Then, we define a function 'usageFunc' that is deprecated:
//..
//  BSLA_DEPRECATED
//  void usageFunc();
//  void usageFunc()
//  {
//      printf("Don't call me.\n");
//  }
//..
// Next, we define a variable 'usageVar' that is deprecated:
//..
//  BSLA_DEPRECATED extern int usageVar;
//  int usageVar = 5;
//..
// Then, we define a typedef 'UsageTypedef' that is deprecated:
//..
//  BSLA_DEPRECATED typedef int UsageTypedef;
//..
// Next, we define a 'struct' with a member 'd_y' that is deprecated:
//..
//  struct UsageStruct {
//      double                 d_x;
//      BSLA_DEPRECATED double d_y;
//  };
//..
// Then, we define an 'enum' 'UsageEnum' that is deprecated:
//..
//  enum BSLA_DEPRECATED UsageEnum { e_FALSE, e_TRUE };
//..
// Next, we define a template this is only deprecated in the case where it is
// specialized with the 'int' type as a template parameter:
//..
//  template <class TYPE>
//  TYPE usageAbs(TYPE x)
//  {
//      return x < 0 ? -x : x;
//  }
//
//  template <>
//  BSLA_DEPRECATED_MESSAGE("'int' specialization not allowed")
//  int usageAbs<int>(int x)
//  {
//      int ret = x < 0 ? -x : x;
//      return ret < 0 ? ~ret : ret;
//  }
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
// Then, we call 'usageFunc':
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
// Next, we access 'usageVar':
//..
//  printf("%d\n", usageVar);
//..
// which results in the following warnings:
//..
//  .../bsla_deprecated.t.cpp:326:20: warning: 'usageVar' is deprecated
//  [-Wdeprecated-declarations]
//      printf("%d\n", usageVar);
//                     ^
//  .../bsla_deprecated.t.cpp:134:25: note: 'usageVar' has been explicitly
//  marked deprecated here
//      extern int usageVar BSLA_DEPRECATED;
//                          ^
//  .../bsla_deprecated.h:119:32: note: expanded from macro 'BSLA_DEPRECATED'
//  #     define BSLA_DEPRECATED [[deprecated]]
//..
// Then, we use 'UsageTypedef':
//..
//  UsageTypedef jjj = 32;
//  (void) jjj;
//..
// which results in the following warnings:
//..
//  .../bsla_deprecated.t.cpp:379:5: warning: 'UsageTypedef' is deprecated
//  [-Wdeprecated-declarations]
//      UsageTypedef jjj = 32;
//      ^
//  .../bsla_deprecated.t.cpp:140:5: note: 'UsageTypedef' has been explicitly
//  marked deprecated here
//      BSLA_DEPRECATED typedef int UsageTypedef;
//      ^
//..
// Next, we access the deprecated member of 'UsageStruct':
//..
//  UsageStruct us;
//  ::memset(&us, 0, sizeof(us));
//  assert(0 == us.d_x);    // no warning
//  assert(0 == us.d_y);    // 'd_y' is deprecated -- issues warning.
//..
// which results in the following warnings:
//..
//  .../bsla_deprecated.t.cpp:387:20: warning: 'd_y' is deprecated
//  [-Wdeprecated-declarations]
//      assert(0 == us.d_y);    // 'd_y' is deprecated -- issues warning.
//                     ^
//  .../bsla_deprecated.t.cpp:146:9: note: 'd_y' has been explicitly marked
//  deprecated here
//          BSLA_DEPRECATED double d_y;
//          ^
//..
// Now, we use the deprecated 'UsageEnum':
//..
//  UsageEnum ue;
//  ue = e_TRUE;
//  (void) ue;
//..
// which results in the following warnings:
//..
//  .../bsla_deprecated.t.cpp:411:15: warning: 'UsageEnum' is deprecated
//  [-Wdeprecated-declarations]
//       UsageEnum ue;
//                 ^
//  .../bsla_deprecated.t.cpp:152:26: note: declared here
//       enum BSLA_DEPRECATED UsageEnum { e_FALSE, e_TRUE };
//                            ^
//..
// Finally, we access the deprecated specialization of 'usageAbs':
//..
//  assert(2.0 == usageAbs(-2.0));            // no warning, 'usageAbs<double>'
//                                            // not deprecated
//  assert(INT_MAX == usageAbs(INT_MIN));     // warning, 'usageAbs<int>' is
//                                            // deprecated
//..
// which results in the following warnings:
//..
//  .../bsla_deprecated.t.cpp:441:39: warning: 'TYPE usageAbs(TYPE) [with TYPE
//  = int]' is deprecated: 'int' specialization not allowed
//  [-Wdeprecated-declarations]
//       assert(INT_MAX == usageAbs(INT_MIN));     // warning, 'usageAbs<int>'
//                                         ^
//  .../bsla_deprecated.t.cpp:168:9: note: declared here
//       int usageAbs<int>(int x)
//           ^~~~~~~~~~~~~
//..

#include <bsls_platform.h>

#define BSLA_DEPRECATED_IS_ACTIVE 0

#if (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)) &&   \
                                                   defined(__has_cpp_attribute)
# if __has_attribute(deprecated)
#   if 201402L <= __cplusplus ||                                              \
                     (defined(BSLS_PLATFORM_CMP_GNU) && 201103L <= __cplusplus)
#     define BSLA_DEPRECATED                  [[ deprecated ]]
#     define BSLA_DEPRECATED_MESSAGE(message) [[ deprecated(message) ]]
#   else
#     define BSLA_DEPRECATED                  __attribute__((__deprecated__))
#     define BSLA_DEPRECATED_MESSAGE(message) __attribute__((__deprecated__))
#   endif

#   undef  BSLA_DEPRECATED_IS_ACTIVE
#   define BSLA_DEPRECATED_IS_ACTIVE 1
# endif
#endif

#if BSLA_DEPRECATED_IS_ACTIVE == 0
# define BSLA_DEPRECATED
# define BSLA_DEPRECATED_MESSAGE(message)
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
