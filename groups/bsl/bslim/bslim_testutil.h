// bslim_testutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLIM_TESTUTIL
#define INCLUDED_BSLIM_TESTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide test utilities for components above 'bsl'.
//
//@CLASSES:
//
//@MACROS:
//  BSLIM_TESTUTIL_LOOP_ASSERT(I, X): print arguments if '!X'
//  BSLIM_TESTUTIL_LOOP2_ASSERT(I, J, X): print arguments if '!X'
//  BSLIM_TESTUTIL_LOOP3_ASSERT(I, J, K, X): print arguments if '!X'
//  BSLIM_TESTUTIL_LOOP4_ASSERT(I, J, K, L, X): print arguments if '!X'
//  BSLIM_TESTUTIL_LOOP5_ASSERT(I, J, K, L, M, X): print arguments if '!X'
//  BSLIM_TESTUTIL_LOOP6_ASSERT(I, J, K, L, M, N, X): print arguments if '!X'
//  BSLIM_TESTUTIL_ASSERTV(..., X): print generic arguments if '!X'
//  BSLIM_TESTUTIL_Q(X): quote identifier literally
//  BSLIM_TESTUTIL_P(X): print identifier and value
//  BSLIM_TESTUTIL_P_(X): print identifier and value without '\n'
//  BSLIM_TESTUTIL_L_: current line number
//  BSLIM_TESTUTIL_T_: print tab without '\n'
//
//@SEE_ALSO: bsls_bsltestutil
//
//@DESCRIPTION: This component provides the standard print macros used in
// BDE-style test drivers ('ASSERT', 'LOOP_ASSERT', 'ASSERTV', 'P', 'Q', 'L',
// and 'T') for components above the 'bsl' package group.
//
// This component also defines a set of overloads for the insertion operator
// ('<<') to support the streaming of test types defined in the 'bsltf'
// package.  These overloads are required for test drivers above the 'bsl'
// package group in order to print objects of the 'bsltf' types to 'bsl::cout'.
//
// Note that the 'bsltf' package resides below 'bsl+bslhdrs', in which
// 'bsl::cout' is defined; therefore, the components in 'bsltf' cannot directly
// define the overloads of the insertion operator to support printing the test
// types.  Instead, an alternate method supplied in 'bsls_bsltestutil' is used
// for test drivers in the 'bsl' package group.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Writing a Test Driver
/// - - - - - - - - - - - - - - - -
// First, we write an elided component to test, which provides a utility class:
//..
//  namespace bdlabc {
//
//  struct ExampleUtil {
//      // This utility class provides sample functionality to demonstrate how
//      // a test driver might be written validating its only method.
//
//      // CLASS METHODS
//      static int fortyTwo();
//          // Return the integer value 42.
//  };
//
//  // CLASS METHODS
//  inline
//  int ExampleUtil::fortyTwo()
//  {
//      return 42;
//  }
//
//  }  // close package namespace
//..
// Then, we can write an elided test driver for this component.  We start by
// providing the standard BDE assert test macro:
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
//  #define ASSERT       BSLIM_TESTUTIL_ASSERT
//  #define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
//  #define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
//  #define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
//  #define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
//  #define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
//  #define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
//  #define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
//  #define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
//  #define ASSERTV      BSLIM_TESTUTIL_ASSERTV
//
//  #define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
//  #define P            BSLIM_TESTUTIL_P   // Print identifier and value.
//  #define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
//  #define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
//  #define L_           BSLIM_TESTUTIL_L_  // current Line number
//..
// Now, using the (standard) abbreviated macro names we have just defined, we
// write a test function for the 'static' 'fortyTwo' method, to be called from
// a test case in the test driver:
//..
//  void testFortyTwo(bool verbose)
//      // Test 'bdlabc::ExampleUtil::fortyTwo' in the specified 'verbose'
//      // verbosity level.
//  {
//      const int value = bdlabc::ExampleUtil::fortyTwo();
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
// Suppose we want to print the value of an object of a test type defined in
// the 'bsltf' package using 'bsl::cout'.  This component supplies the
// necessary overloads of the insertion operator for this to be done directly.
//
// First, include the header of this component:
//..
//  #include <bslim_testutil.h>
//..
// Now, we construct a 'bsltf::SimpleTestType' object and stream its value to
// 'bsl::cout' using the '<<' operator:
//..
//  bsltf::SimpleTestType a(10);
//  bsl::cout << a;
//..
// Finally, we observe the following console output:
//..
//  10
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLTF_TEMPLATETESTFACILITY
#include <bsltf_templatetestfacility.h>
#endif

#ifndef INCLUDED_BSL_IOSTREAM
#include <bsl_iostream.h>
#endif

                       // =================
                       // Macro Definitions
                       // =================

#define BSLIM_TESTUTIL_ASSERT(X)                                              \
    aSsErT(!(X), #X, __LINE__);

#define BSLIM_TESTUTIL_LOOP0_ASSERT                                           \
    BSLIM_TESTUTIL_ASSERT

#define BSLIM_TESTUTIL_LOOP_ASSERT(I,X)                                       \
    if (!(X)) { cout << #I << ": " << I << "\n";                              \
                aSsErT(1, #X, __LINE__); }

#define BSLIM_TESTUTIL_LOOP1_ASSERT                                           \
    BSLIM_TESTUTIL_LOOP_ASSERT

#define BSLIM_TESTUTIL_LOOP2_ASSERT(I,J,X)                                    \
    if (!(X)) { cout << #I << ": " << I << "\t"                               \
                     << #J << ": " << J << "\n";                              \
                aSsErT(1, #X, __LINE__); }

#define BSLIM_TESTUTIL_LOOP3_ASSERT(I,J,K,X)                                  \
    if (!(X)) { cout << #I << ": " << I << "\t"                               \
                     << #J << ": " << J << "\t"                               \
                     << #K << ": " << K << "\n";                              \
                aSsErT(1, #X, __LINE__); }

#define BSLIM_TESTUTIL_LOOP4_ASSERT(I,J,K,L,X)                                \
    if (!(X)) { cout << #I << ": " << I << "\t"                               \
                     << #J << ": " << J << "\t"                               \
                     << #K << ": " << K << "\t"                               \
                     << #L << ": " << L << "\n";                              \
                aSsErT(1, #X, __LINE__); }

#define BSLIM_TESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)                              \
    if (!(X)) { cout << #I << ": " << I << "\t"                               \
                     << #J << ": " << J << "\t"                               \
                     << #K << ": " << K << "\t"                               \
                     << #L << ": " << L << "\t"                               \
                     << #M << ": " << M << "\n";                              \
               aSsErT(1, #X, __LINE__); }

#define BSLIM_TESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)                            \
    if (!(X)) { cout << #I << ": " << I << "\t"                               \
                     << #J << ": " << J << "\t"                               \
                     << #K << ": " << K << "\t"                               \
                     << #L << ": " << L << "\t"                               \
                     << #M << ": " << M << "\t"                               \
                     << #N << ": " << N << "\n";                              \
               aSsErT(1, #X, __LINE__); }

// The 'BSLIM_TESTUTIL_EXPAND' macro is required to work around a preprocessor
// issue on Windows that prevents '__VA_ARGS__' from being expanded in the
// definition of 'BSLIM_TESTUTIL_NUM_ARGS'.

#define BSLIM_TESTUTIL_EXPAND(X)                                              \
    X

#define BSLIM_TESTUTIL_NUM_ARGS_IMPL(X6, X5, X4, X3, X2, X1, X0, N, ...)      \
    N

#define BSLIM_TESTUTIL_NUM_ARGS(...)                                          \
    BSLIM_TESTUTIL_EXPAND(BSLIM_TESTUTIL_NUM_ARGS_IMPL(                       \
                                         __VA_ARGS__, 6, 5, 4, 3, 2, 1, 0, ""))

#define BSLIM_TESTUTIL_LOOPN_ASSERT_IMPL(N, ...)                              \
    BSLIM_TESTUTIL_EXPAND(BSLIM_TESTUTIL_LOOP ## N ## _ASSERT(__VA_ARGS__))

#define BSLIM_TESTUTIL_LOOPN_ASSERT(N, ...)                                   \
    BSLIM_TESTUTIL_LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define BSLIM_TESTUTIL_ASSERTV(...)                                           \
    BSLIM_TESTUTIL_LOOPN_ASSERT(                                              \
                             BSLIM_TESTUTIL_NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

#define BSLIM_TESTUTIL_Q(X)                                                   \
    cout << "<| " #X " |>" << endl;
    // Quote identifier literally.

#define BSLIM_TESTUTIL_P(X)                                                   \
    cout << #X " = " << (X) << endl;
    // Print identifier and its value.

#define BSLIM_TESTUTIL_P_(X)                                                  \
    cout << #X " = " << (X) << ", " << flush;
    // 'P(X)' without '\n'

#define BSLIM_TESTUTIL_L_                                                     \
    __LINE__
    // current Line number

#define BSLIM_TESTUTIL_T_                                                     \
    cout << "\t" << flush;
    // Print tab (w/o newline).

namespace BloombergLP {
namespace bsltf {

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const EnumeratedTestType::Enum&        object);
bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const UnionTestType&                   object);
bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const SimpleTestType&                  object);
bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const AllocTestType&                   object);
bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const BitwiseMoveableTestType&         object);
bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const AllocBitwiseMoveableTestType&    object);
bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const NonTypicalOverloadsTestType&     object);
bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const NonAssignableTestType&           object);
bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const NonDefaultConstructibleTestType& object);
bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const NonEqualComparableTestType&      object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const EnumeratedTestType::Enum& object)
{
    return stream << bsltf::TemplateTestFacility::getIdentifier(object);
}

inline
bsl::ostream& operator<<(bsl::ostream&        stream,
                         const UnionTestType& object)
{
    return stream << bsltf::TemplateTestFacility::getIdentifier(object);
}

inline
bsl::ostream& operator<<(bsl::ostream&         stream,
                         const SimpleTestType& object)
{
    return stream << bsltf::TemplateTestFacility::getIdentifier(object);
}

inline
bsl::ostream& operator<<(bsl::ostream&        stream,
                         const AllocTestType& object)
{
    return stream << bsltf::TemplateTestFacility::getIdentifier(object);
}

inline
bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const BitwiseMoveableTestType& object)
{
    return stream << bsltf::TemplateTestFacility::getIdentifier(object);
}

inline
bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const AllocBitwiseMoveableTestType& object)
{
    return stream << bsltf::TemplateTestFacility::getIdentifier(object);
}

inline
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const NonTypicalOverloadsTestType& object)
{
    return stream << bsltf::TemplateTestFacility::getIdentifier(object);
}

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const NonAssignableTestType& object)
{
    return stream << bsltf::TemplateTestFacility::getIdentifier(object);
}

inline
bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const NonDefaultConstructibleTestType& object)
{
    return stream << bsltf::TemplateTestFacility::getIdentifier(object);
}

inline
bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const NonEqualComparableTestType& object)
{
    return stream << bsltf::TemplateTestFacility::getIdentifier(object);
}

}  // close namespace bsltf
}  // close enterprise namespace

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
