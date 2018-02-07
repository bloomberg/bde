// bslmt_testutil.h                                      -*-C++-*-
#ifndef INCLUDED_BSLMT_TESTUTIL
#define INCLUDED_BSLMT_TESTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide test utilities for components above 'bsl'.
//
//@CLASSES:
//
//@MACROS:
//  BSLMT_TESTUTIL_LOOP0_ASSERT(X)
//  BSLMT_TESTUTIL_LOOP1_ASSERT(I, X)
//  BSLMT_TESTUTIL_LOOP2_ASSERT(I, J, X)
//  BSLMT_TESTUTIL_LOOP3_ASSERT(I, J, K, X)
//  BSLMT_TESTUTIL_LOOP4_ASSERT(I, J, K, L, X)
//  BSLMT_TESTUTIL_LOOP5_ASSERT(I, J, K, L, M, X)
//  BSLMT_TESTUTIL_LOOP6_ASSERT(I, J, K, L, M, N, X)
//  BSLMT_TESTUTIL_ASSERTV(..., X)
//  BSLMT_TESTUTIL_Q(X)
//  BSLMT_TESTUTIL_P(X)
//  BSLMT_TESTUTIL_P_(X)
//  BSLMT_TESTUTIL_L_
//  BSLMT_TESTUTIL_T_
//
//@SEE_ALSO: bslmt_testutil, bsls_bsltestutil
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
// This component also defines a pair of methods, 'setFunc' and 'callFunc',
// that allow a test driver to set and call a function by going through another
// compilation unit to preclude the optimizer from inlining the function call.

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
//  #define ASSERT       BSLMT_TESTUTIL_LOOP0_ASSERT
//  #define ASSERTV      BSLMT_TESTUTIL_ASSERTV
//  #define Q            BSLMT_TESTUTIL_Q
//  #define P            BSLMT_TESTUTIL_P
//  #define P_           BSLMT_TESTUTIL_P_
//  #define T_           BSLMT_TESTUTIL_T_
//  #define L_           BSLMT_TESTUTIL_L_
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSL_IOSTREAM
#include <bsl_iostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif


                           // =================
                           // Macro Definitions
                           // =================

#define BSLMT_TESTUTIL_GUARD                                                  \
    BloombergLP::bslmt::TestUtil::GuardObject bslmt_testutil_guard_object
    // Create a guard that locks the singleton output mutex.

#define BSLMT_TESTUTIL_LOOP0_ASSERT(X)                                        \
    if (X) ; else do { BSLMT_TESTUTIL_GUARD;                                  \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_LOOP1_ASSERT(I,X)                                      \
    if (X) ; else do { BSLMT_TESTUTIL_GUARD;                                  \
                       bsl::cout << #I << ": " << I << "\n";                  \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_LOOP2_ASSERT(I,J,X)                                    \
    if (X) ; else do { BSLMT_TESTUTIL_GUARD;                                  \
                       bsl::cout << #I << ": " << I << "\t"                   \
                                 << #J << ": " << J << "\n";                  \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_LOOP3_ASSERT(I,J,K,X)                                  \
    if (X) ; else do { BSLMT_TESTUTIL_GUARD;                                  \
                       bsl::cout << #I << ": " << I << "\t"                   \
                                 << #J << ": " << J << "\t"                   \
                                 << #K << ": " << K << "\n";                  \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_LOOP4_ASSERT(I,J,K,L,X)                                \
    if (X) ; else do { BSLMT_TESTUTIL_GUARD;                                  \
                       bsl::cout << #I << ": " << I << "\t"                   \
                                 << #J << ": " << J << "\t"                   \
                                 << #K << ": " << K << "\t"                   \
                                 << #L << ": " << L << "\n";                  \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)                              \
    if (X) ; else do { BSLMT_TESTUTIL_GUARD;                                  \
                       bsl::cout << #I << ": " << I << "\t"                   \
                                 << #J << ": " << J << "\t"                   \
                                 << #K << ": " << K << "\t"                   \
                                 << #L << ": " << L << "\t"                   \
                                 << #M << ": " << M << "\n";                  \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)                            \
    if (X) ; else do { BSLMT_TESTUTIL_GUARD;                                  \
                       bsl::cout << #I << ": " << I << "\t"                   \
                                 << #J << ": " << J << "\t"                   \
                                 << #K << ": " << K << "\t"                   \
                                 << #L << ": " << L << "\t"                   \
                                 << #M << ": " << M << "\t"                   \
                                 << #N << ": " << N << "\n";                  \
                       aSsErT(1, #X, __LINE__); } while (false)

// The 'BSLMT_TESTUTIL_EXPAND' macro is required to work around a
// preprocessor issue on Windows that prevents '__VA_ARGS__' from being
// expanded in the definition of 'BSLMT_TESTUTIL_NUM_ARGS'.

#define BSLMT_TESTUTIL_EXPAND(X)                                              \
    X

#define BSLMT_TESTUTIL_NUM_ARGS_IMPL(X6, X5, X4, X3, X2, X1, X0, N, ...)      \
    N

#define BSLMT_TESTUTIL_NUM_ARGS(...)                                          \
    BSLMT_TESTUTIL_EXPAND(BSLMT_TESTUTIL_NUM_ARGS_IMPL(                       \
                                         __VA_ARGS__, 6, 5, 4, 3, 2, 1, 0, ""))

#define BSLMT_TESTUTIL_LOOPN_ASSERT_IMPL(N, ...)                              \
    BSLMT_TESTUTIL_EXPAND(BSLMT_TESTUTIL_LOOP ## N ## _ASSERT(__VA_ARGS__))

#define BSLMT_TESTUTIL_LOOPN_ASSERT(N, ...)                                   \
    BSLMT_TESTUTIL_LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define BSLMT_TESTUTIL_ASSERTV(...) BSLMT_TESTUTIL_LOOPN_ASSERT(              \
                             BSLMT_TESTUTIL_NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

extern char bslmt_testutil_guard_object;
    // Create a global variable, outside of any namespace, with this name to
    // match that sought by the 'NestedGuard' in the following macro if no
    // 'GuardObject' with that name is in a tighter scope.

#define BSLMT_TESTUTIL_NESTED_GUARD                                           \
    BloombergLP::bslmt::TestUtil::NestedGuard                                 \
               bslmt_testutil_nested_guard_object(&bslmt_testutil_guard_object)
    // If a regular 'GuardObject' named
    // 'bslmt_testutil_guard_object' created by
    // 'BSLMT_TESTUTIL_GUARD' is in scope, create a guard that
    // does nothing, because the singleton output mutex is already locked.
    // Otherwise, 'bslmt_testutil_guard_object' will match the
    // global scoped 'char' defined above and this macro will create a guard
    // that locks the singleton output mutex.

// The following 4 macros can either be used within a block guarded by a
// 'BSLMT_TESTUTIL_GUARD', in which case they don't acquire the
// mutex, or outside such a block, in which case they do acquire it.

#define BSLMT_TESTUTIL_Q(X)                                                   \
    do { BSLMT_TESTUTIL_NESTED_GUARD;                                         \
         bsl::cout << "<| " #X " |>" << bsl::endl; } while (false)
    // Quote identifier literally.

#define BSLMT_TESTUTIL_P(X)                                                   \
    do { BSLMT_TESTUTIL_NESTED_GUARD;                                         \
         bsl::cout << #X " = " << (X) << bsl::endl; } while (false)
    // Print identifier and its value.

#define BSLMT_TESTUTIL_P_(X)                                                  \
    do { BSLMT_TESTUTIL_NESTED_GUARD;                                         \
         bsl::cout << #X " = " << (X) << ", " << bsl::flush; } while (false)
    // 'P(X)' without '\n'

#define BSLMT_TESTUTIL_T_                                                     \
    do { BSLMT_TESTUTIL_NESTED_GUARD;                                         \
         bsl::cout << "\t" << bsl::flush; } while (false)
    // Print tab (w/o newline).

#define BSLMT_TESTUTIL_L_                                                     \
    __LINE__
    // current Line number

namespace BloombergLP {
namespace bslmt {

                              // ===============
                              // struct TestUtil
                              // ===============

struct TestUtil {
    // This 'struct' provides a namespace for a suite of utility functions that
    // facilitate the creation of BDE-style test drivers.

    // PUBLIC TYPES
    typedef void *(*Func)(void *);
        // 'Func' is the type of a user-supplied callback functor that can be
        // called by external code and will not be inlined by a compiler.

    class GuardObject;
    class NestedGuard;

  private:
    // CLASS DATA
    static Func s_func;   // user-supplied functor

  public:
    // CLASS METHODS
    static void *callFunc(void *arg);
        // Call the function whose address was passed to the most recent call
        // to 'setFunc', passing it the specified 'arg', and return its
        // returned value.  The behavior is undefined unless 'setFunc' has been
        // called, and the most recent call was passed a non-null function
        // pointer.

    static bool compareText(bslstl::StringRef a,
                            bslstl::StringRef b,
                            bsl::ostream&     errorStream = bsl::cout);
        // Return 'true' if the specified 'a' has the same value as the
        // specified' b', and 'false' otherwise.  Optionally specify an
        // 'errorStream' on which, if 'a' and 'b' are not the same', a
        // description of how the two strings differ will be written.  If
        // 'errorStream' is not supplied, 'stdout' is used.

    static bslmt::Mutex& outputMutexSingleton();
        // Return a reference to the constructed output mutex, initializing
        // it if necessary.

    static void setFunc(Func func);
        // Set the function to be called by 'callFunc' to the specified 'func'.
};

                          // ===========================
                          // class TestUtil::GuardObject
                          // ===========================

class TestUtil::GuardObject {
    // DATA
    bslmt::Mutex *d_mutex;

  private:
    // NOT IMPLEMENTED
    GuardObject(const GuardObject&);
    GuardObject& operator=(const GuardObject&);

  public:
    // CREATORS
    GuardObject();
        // Acquire and lock the output mutex that is delivered by
        // 'MultiThreadedTestUtil::outputMutexSingleton()'.

    ~GuardObject();
        // Release the mutex acquired at construction.
};

                          // ===========================
                          // class TestUtil::NestedGuard
                          // ===========================

class TestUtil::NestedGuard {
    // DATA
    bslmt::Mutex *d_mutex;

  private:
    // NOT IMPLEMENTED
    NestedGuard(const NestedGuard&);
    NestedGuard& operator=(const NestedGuard&);

  public:
    // CREATORS
    explicit
    NestedGuard(char *);
        // Acquire and lock the output mutex that is delivered by
        // 'MultiThreadedTestUtil::outputMutexSingleton()'.

    explicit
    NestedGuard(GuardObject *);
        // Initialize 'd_mutex' to 0.

    ~NestedGuard();
        // If the constructor locked a mutex, unlock that mutex, otherwise do
        // nothing.
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
