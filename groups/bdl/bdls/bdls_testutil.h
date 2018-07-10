// bdls_testutil.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLS_TESTUTIL
#define INCLUDED_BDLS_TESTUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide test utilities for components in 'bdl' and above.
//
//@DEPRECATED: Use 'bslim_testutil' instead.
//
//@CLASSES:
//
//@MACROS:
//  BDLS_TESTUTIL_ASSERT(X): record and print error if '!X'
//  BDLS_TESTUTIL_LOOP_ASSERT(I, X): print args if '!X'
//  BDLS_TESTUTIL_LOOP2_ASSERT(I, J, X): print args if '!X'
//  BDLS_TESTUTIL_LOOP3_ASSERT(I, J, K, X): print args if '!X'
//  BDLS_TESTUTIL_LOOP4_ASSERT(I, J, K, L, X): print args if '!X'
//  BDLS_TESTUTIL_LOOP5_ASSERT(I, J, K, L, M, X): print args if '!X'
//  BDLS_TESTUTIL_LOOP6_ASSERT(I, J, K, L, M, N, X): print args if '!X'
//  BDLS_TESTUTIL_ASSERTV(..., X): generic print args if '!X'
//  BDLS_TESTUTIL_Q(X): quote identifier literally
//  BDLS_TESTUTIL_P(X): print identifier and value
//  BDLS_TESTUTIL_P_(X): print identifier and value without '\n'
//  BDLS_TESTUTIL_L_: current line number
//  BDLS_TESTUTIL_T_: print tab without '\n'
//
//@SEE_ALSO: bslim_testutil
//
//@DESCRIPTION: This component provides the standard print macros used in
// BDE-style test drivers ('ASSERT', 'LOOP_ASSERT', 'ASSERTV', 'P', 'Q', 'L',
// and 'T') for components in the 'bdl' package group and above.
//
// This component also define a set of overloads for the insertion operator
// ('<<') to support the streaming of test types defined in the 'bsltf'
// package.  This is required for test drivers in the 'bdl' package group and
// above to print the objects of these types to 'bsl::cout'.
//
// Note that the 'bsltf' package resides below 'bsl+bslhdrs', in which
// 'bsl::cout' is defined; therefore, the components in 'bsltf' cannot
// directly define the overloads of the insertion operator to support printing
// the test types.  Instead, an alternate method supplied in 'bsls_bsltestutil'
// is used for test drivers in the 'bsl' package group.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Writing a Test Driver
/// - - - - - - - - - - - - - - - -
// First, we write a component to test, which provides a utility class:
//..
//  namespace bdlabc {
//
//  struct BdlExampleUtil {
//      // This utility class provides sample functionality to demonstrate how
//      // a test driver might be written validating its only method.
//
//      static int fortyTwo();
//          // Return the integer value '42'.
//  };
//
//  inline
//  int BdlExampleUtil::fortyTwo()
//  {
//      return 42;
//  }
//
//  }  // close package namespace
//..
// Then, we can write a test driver for this component.  We start by providing
// the standard BDE assert test macro:
//..
//  //=========================================================================
//  //                       STANDARD BDE ASSERT TEST MACRO
//  //-------------------------------------------------------------------------
//  static int testStatus = 0;
//
//  static void aSsErT(bool b, const char *s, int i)
//  {
//      if (b) {
//          printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
//          if (testStatus >= 0 && testStatus <= 100) ++testStatus;
//      }
//  }
//..
// Next, we define the standard print and 'LOOP_ASSERT' macros, as aliases to
// the macros defined by this component:
//..
//  //=========================================================================
//  //                       STANDARD BDE TEST DRIVER MACROS
//  //-------------------------------------------------------------------------
//
//  #define ASSERT       BDLS_TESTUTIL_ASSERT
//  #define LOOP_ASSERT  BDLS_TESTUTIL_LOOP_ASSERT
//  #define LOOP0_ASSERT BDLS_TESTUTIL_LOOP0_ASSERT
//  #define LOOP1_ASSERT BDLS_TESTUTIL_LOOP1_ASSERT
//  #define LOOP2_ASSERT BDLS_TESTUTIL_LOOP2_ASSERT
//  #define LOOP3_ASSERT BDLS_TESTUTIL_LOOP3_ASSERT
//  #define LOOP4_ASSERT BDLS_TESTUTIL_LOOP4_ASSERT
//  #define LOOP5_ASSERT BDLS_TESTUTIL_LOOP5_ASSERT
//  #define LOOP6_ASSERT BDLS_TESTUTIL_LOOP6_ASSERT
//  #define ASSERTV      BDLS_TESTUTIL_ASSERTV
//
//  #define Q   BDLS_TESTUTIL_Q   // Quote identifier literally.
//  #define P   BDLS_TESTUTIL_P   // Print identifier and value.
//  #define P_  BDLS_TESTUTIL_P_  // P(X) without '\n'.
//  #define T_  BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
//  #define L_  BDLS_TESTUTIL_L_  // current Line number
//..
// Now, using the (standard) abbreviated macro names we have just defined, we
// write a test function for the 'static' 'fortyTwo' method, to be called from
// a test case in a test driver.
//..
//  void testFortyTwo(bool verbose)
//  {
//      const int value = bdlabc::BdlExampleUtil::fortyTwo();
//      if (verbose) P(value);
//      LOOP_ASSERT(value, 42 == value);
//  }
//..
// Finally, when 'testFortyTwo' is called from a test case in verbose mode we
// observe the console output:
//..
//  value = 42
//..
//
///Example 2: Print the Value of a Test Type
///- - - - - - - - - - - - - - - - - - - - -
// Suppose we want to print the value of an object of a test type defined the
// 'bsltf' package using 'bsl::cout'.  This component supplies the necessary
// overloads of the insertion operator for this to be done directly.
//
// First, include the header of this component:
//..
//  #include <bdls_testutil.h>
//..
// Now, we construct a 'SimpleTestType' object and stream its value to
// 'bsl::cout' using the '<<' operator:
//..
//  bsltf::SimpleTestType a(10);
//  bsl::cout << a;
//..
// Finally, we observe the following console output:
//..
//  10
//..

#include <bdlscm_version.h>

#include <bslim_testutil.h>

                       // =================
                       // Macro Definitions
                       // =================

#define BDLS_TESTUTIL_ASSERT(X)                                               \
    BSLIM_TESTUTIL_ASSERT(X)

#define BDLS_TESTUTIL_LOOP0_ASSERT                                            \
    BSLIM_TESTUTIL_ASSERT

#define BDLS_TESTUTIL_LOOP_ASSERT(I,X)                                        \
    BSLIM_TESTUTIL_LOOP_ASSERT(I,X)

#define BDLS_TESTUTIL_LOOP1_ASSERT                                            \
    BSLIM_TESTUTIL_LOOP_ASSERT

#define BDLS_TESTUTIL_LOOP2_ASSERT(I,J,X)                                     \
    BSLIM_TESTUTIL_LOOP2_ASSERT(I,J,X)

#define BDLS_TESTUTIL_LOOP3_ASSERT(I,J,K,X)                                   \
    BSLIM_TESTUTIL_LOOP3_ASSERT(I,J,K,X)

#define BDLS_TESTUTIL_LOOP4_ASSERT(I,J,K,L,X)                                 \
    BSLIM_TESTUTIL_LOOP4_ASSERT(I,J,K,L,X)

#define BDLS_TESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)                               \
    BSLIM_TESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)

#define BDLS_TESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)                             \
    BSLIM_TESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)

#define BDLS_TESTUTIL_ASSERTV                                                 \
    BSLIM_TESTUTIL_ASSERTV

#define BDLS_TESTUTIL_Q(X)                                                    \
    BSLIM_TESTUTIL_Q(X)

#define BDLS_TESTUTIL_P(X)                                                    \
    BSLIM_TESTUTIL_P(X)

#define BDLS_TESTUTIL_P_(X)                                                   \
    BSLIM_TESTUTIL_P_(X)

#define BDLS_TESTUTIL_L_                                                      \
    BSLIM_TESTUTIL_L_

#define BDLS_TESTUTIL_T_                                                      \
    BSLIM_TESTUTIL_T_

#endif

// ----------------------------------------------------------------------------
// Copyright 2012 Bloomberg Finance L.P.
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
