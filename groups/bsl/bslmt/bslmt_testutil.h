// bslmt_testutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMT_TESTUTIL
#define INCLUDED_BSLMT_TESTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide thread-safe test utilities for components above 'bsl'.
//
//@CLASSES:
//
//@MACROS:
//  BSLMT_TESTUTIL_LOOP0_ASSERT(X)
//  BSLMT_TESTUTIL_ASSERTV(..., X)
//  BSLMT_TESTUTIL_Q(X)
//  BSLMT_TESTUTIL_P(X)
//  BSLMT_TESTUTIL_P_(X)
//  BSLMT_TESTUTIL_L_
//  BSLMT_TESTUTIL_T_
//  BSLMT_TESTUTIL_OUTPUT_GUARD
//
//@SEE_ALSO: bslim_testutil, bsls_bsltestutil
//
//@DESCRIPTION: This component provides the mutex-controlled versions of the
// standard print macros used in BDE-style test drivers ('ASSERT', 'ASSERTV',
// 'P', 'Q', 'L', and 'T') for multithreaded components at or above the 'bslmt'
// package.  A new macro is introduced, 'BSLMT_TESTUTIL_OUTPUT_GUARD', which
// creates a guard object that locks the mutex.
//
// This component defines a pair of methods, 'setFunc' and 'callFunc', that
// allow a test driver to set and call a function by going through another
// compilation unit to preclude the optimizer from inlining the function call.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Writing a Test Driver
/// - - - - - - - - - - - - - - - -
// First, we write an elided component to test, which provides a utility class:
//..
//  namespace bslabc {
//
//  struct SumUtil {
//      // This utility class provides sample functionality to demonstrate how
//      // a multi threaded test driver might be written.
//
//      // CLASS METHODS
//      static double sumOfSquares(double a,
//                                 double b = 0,
//                                 double c = 0,
//                                 double d = 0);
//          // Return the sum of the squares of one to 4 arguments, the
//          // specified 'a' and the optionally specified 'b', 'c', and 'd'.
//  };
//
//  // CLASS METHODS
//  inline
//  double SumUtil::sumOfSquares(double a,
//                               double b,
//                               double c,
//                               double d)
//  {
//      // Note that there is a bug here in that we have added the cube, rather
//      // than the square, of 'd'.
//
//      double ret = a*a;
//      ret += b*b;
//      ret += c*c;
//      ret += d*d*d;
//      return ret;
//  }
//
//  }  // close namespace bslabc
//..
// Then, we can write an elided test driver for this component.  We start by
// providing the standard BDE assert test macro:
//..
//  //=========================================================================
//  //                       STANDARD BDE ASSERT TEST MACRO
//  //-------------------------------------------------------------------------
//  int testStatus = 0;
//
//  void aSsErT(int c, const char *s, int i)
//  {
//      if (c) {
//          bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
//                    << "    (failed)" << bsl::endl;
//          if (testStatus >= 0 && testStatus <= 100) ++testStatus;
//      }
//  }
//..
// Next, we define the standard print and 'ASSERT*' macros, as aliases to the
// macros defined by this component:
//..
//  //=========================================================================
//  //                       STANDARD BDE TEST DRIVER MACROS
//  //-------------------------------------------------------------------------
//  #define ASSERT   BSLMT_TESTUTIL_ASSERT
//  #define ASSERTV  BSLMT_TESTUTIL_ASSERTV
//
//  #define Q        BSLMT_TESTUTIL_Q   // Quote identifier literally.
//  #define P        BSLMT_TESTUTIL_P   // Print identifier and value.
//  #define P_       BSLMT_TESTUTIL_P_  // P(X) without '\n'.
//  #define T_       BSLMT_TESTUTIL_T_  // Print a tab (w/o newline).
//  #define L_       BSLMT_TESTUTIL_L_  // current Line number
//..
// Then, we define global verbosity flags to be used for controlling debug
// traces.  The flags will be set by elided code at the beginning of 'main' to
// determine the level of output verbosity the client wants.  We also define
// six global double variables that we will use for testing, and a barrier to
// coordinate threads
//..
//  //=========================================================================
//  //                     GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//  //-------------------------------------------------------------------------
//
//  bool  verbose;
//  bool  veryVerbose;
//  bool  veryVeryVerbose;
//  bool  veryVeryVeryVerbose;
//..
// Next we define some global typedefs, variables, and constants used by this
// test case:
//..
//                              // ---------------
//                              // Usage Test Case
//                              // ---------------
//
//  typedef  bslabc::SumUtil SU;
//
//  bsls::AtomicInt threadIdx(0);
//  bsls::AtomicInt lastRand(0);
//
//  enum { k_NUM_ITERATIONS     = 5,
//         k_NUM_THREADS        = 10,
//         k_NUM_RAND_VARIABLES = k_NUM_ITERATIONS * k_NUM_THREADS * 4 };
//
//  double *randNumbers;
//..
// Then, using out test macros, we write five test functors that can be run
// concurrently to test the five static functions.
//..
//  struct TestSums {
//      void operator()()
//          // Test 'TestUtil::sumOfSquares' with a variety of randomly
//          // generated arguments to be found in the array 'randNumbers'
//      {
//          const int idx     = threadIdx++;
//          int       randIdx = idx * k_NUM_ITERATIONS * 4;
//
//          if (veryVerbose) {
//              BSLMT_TESTUTIL_OUTPUT_GUARD;
//
//              // Instantiating the output guard above locks the output
//              // mutex.  The following two macros would lock that mutex
//              // themselves, except that they detect the output guard and
//              // refrain.  Having the two macros guarded by one output guard
//              // ensures that the output from both will appear on the same
//              // line, uninterrupted by output from the 'BSLMT_TESTUTIL_*'
//              // macros being called from any other thread.
//
//              P_(idx);    P(randIdx);
//          }
//
//          for (int ii = 0; ii < k_NUM_ITERATIONS; ++ii) {
//              double x[4];
//              for (int jj = 0; jj < 4; ++jj, ++randIdx) {
//                  x[jj] = randNumbers[randIdx];
//              }
//              if (randIdx >= k_NUM_RAND_VARIABLES) {
//                  BSLMT_TESTUTIL_OUTPUT_GUARD;
//
//                  // We expect with the following two asserts, if either one
//                  // fails, both will fail.  We create the output guard above
//                  // to ensure that if they both output, their output will
//                  // appear adjacent to each other, uninterrupted by any
//                  // output from 'bslmt_TESTUTIL_*' macros being called by
//                  // any other thread.
//
//                  ASSERTV(randIdx, k_NUM_RAND_VARIABLES, !lastRand);
//                  ASSERTV(randIdx, k_NUM_RAND_VARIABLES,
//                                            randIdx == k_NUM_RAND_VARIABLES);
//                  lastRand = true;
//
//                  if (veryVerbose) {
//                      // This output calling 'cout' could become illegible if
//                      // 'BSLMT_TESTUTIL_*' macros in the other threads
//                      // output at the same time.  We are safe here because
//                      // we are within scope of an output guard.
//
//                      bsl::cout << "Thread number " << idx <<
//                           " reached the end of the random number buffer." <<
//                                                                   bsl::endl;
//                  }
//              }
//
//              // If any of the 'ASSERTV's following here fail, they will
//              // detect that no output guard object is in scope and lock the
//              // output mutex before doing any output, so the entire trace
//              // from any one failing 'ASSERTV' will be in one contiguous
//              // block.
//
//              double exp = x[0] * x[0];
//              ASSERTV(x[0], exp, SU::sumOfSquares(x[0]),
//                                              exp == SU::sumOfSquares(x[0]));
//
//              exp += x[1] * x[1];
//              ASSERTV(x[0], x[1], exp, SU::sumOfSquares(x[0], x[1]),
//                                        exp == SU::sumOfSquares(x[0], x[1]));
//
//              exp += x[2] * x[2];
//              ASSERTV(x[0], x[1], x[2], exp,
//                                          SU::sumOfSquares(x[0], x[1], x[2]),
//                                  exp == SU::sumOfSquares(x[0], x[1], x[2]));
//
//              exp += x[3] * x[3];
//              ASSERTV(x[0], x[1], x[2], x[3], exp,
//                                    SU::sumOfSquares(x[0], x[1], x[2], x[3]),
//                            exp == SU::sumOfSquares(x[0], x[1], x[2], x[3]));
//          }
//
//          if (veryVerbose) {
//              BSLMT_TESTUTIL_OUTPUT_GUARD;
//
//              bsl::cout << "Thread number " << idx << " finishing." <<
//                                                                   bsl::endl;
//          }
//      }
//  };
//..
// Next, in 'main', we allocate and populate our array of random numbers with
// 'bsl::rand', which is single-threaded:
//..
//  bslma::TestAllocator testAllocator("usage");
//  randNumbers = static_cast<double *>(testAllocator.allocate(
//                                     sizeof(double) * k_NUM_RAND_VARIABLES));
//
//  for (int ii = 0; ii < k_NUM_RAND_VARIABLES; ++ii) {
//      randNumbers[ii] = static_cast<double>(bsl::rand()) / RAND_MAX *
//                                                                 bsl::rand();
//  }
//..
// Then, we spawn our threads and let them run:
//..
//  bslmt::ThreadGroup tg;
//  tg.addThreads(TestSums(), k_NUM_THREADS);
//..
// Now, we join the threads, clean up, and at the end of 'main' examine
// 'testStatus'.  If it's greater than 0, report that the test failed:
//..
//  tg.joinAll();
//
//  testAllocator.deallocate(randNumbers);
//
//  if (testStatus > 0) {
//      // Note that since there is a bug in 'SU::sumOfSquares' with 4 args, we
//      // expect the last assert in 'TestSums::operator()' to fail 5
//      // iterations times 10 threads == 50 times, so the following message
//      // will report 'test status = 50'.
//
//      bsl::cerr << "Error, non-zero test status = " << testStatus << "."
//                << bsl::endl;
//  }
//
//  return testStatus;
//..
// Finally, after the program has run, we see 50 assertion failures in the
// output with differring values of the 'x[*]' variables, but each assert's
// output is intact (rather than being interleaved with output from asserts in
// other threads).

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
                           // Global Definition
                           // =================

extern char bloomberglp_bslmt_testutil_guard_object;
    // Create a global variable, outside of any namespace, with this name to
    // match that sought by the 'NestedGuard' in the
    // 'BSLMT_TESTUTIL_NESTED_GUARD' macro if it is instantiated in a context
    // not guarded by 'BSLMT_TESTUTIL_GUARD'.  This variable is not intended to
    // be accessed directly by clients of this component, rather, only by
    // macros within this component.  Note that the state of this variable is
    // never accessed.

                           // =================
                           // Macro Definitions
                           // =================

#define BSLMT_TESTUTIL_OUTPUT_GUARD                                           \
    BloombergLP::bslmt::TestUtil::GuardObject                                 \
                                        bloomberglp_bslmt_testutil_guard_object
    // Create a guard that unconditionally locks the singleton output mutex.

#define BSLMT_TESTUTIL_NESTED_OUTPUT_GUARD                                    \
    BloombergLP::bslmt::TestUtil::NestedGuard                                 \
                      bslmt_testutil_nested_guard_object(                     \
                                      &bloomberglp_bslmt_testutil_guard_object)
    // If a regular 'GuardObject' named
    // 'bloomberglp_bslmt_testutil_guard_object' created by
    // 'BSLMT_TESTUTIL_GUARD' is in scope, create a guard that does nothing,
    // because the singleton output mutex is already locked in the current
    // thread.  Otherwise, 'bloomberglp_bslmt_testutil_guard_object' will match
    // the global scoped 'char' defined above and this macro will create a
    // guard that locks the singleton output mutex.

#define BSLMT_TESTUTIL_LOOP0_ASSERT(X)                                        \
    if (X) ; else do { BSLMT_TESTUTIL_NESTED_OUTPUT_GUARD;                    \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_ASSERT(X) BSLMT_TESTUTIL_LOOP0_ASSERT(X)

#define BSLMT_TESTUTIL_LOOP1_ASSERT(I,X)                                      \
    if (X) ; else do { BSLMT_TESTUTIL_NESTED_OUTPUT_GUARD;                    \
                       bsl::cout << #I << ": " << (I) << "\n";                \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_LOOP2_ASSERT(I,J,X)                                    \
    if (X) ; else do { BSLMT_TESTUTIL_NESTED_OUTPUT_GUARD;                    \
                       bsl::cout << #I << ": " << (I) << "\t"                 \
                                 << #J << ": " << (J) << "\n";                \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_LOOP3_ASSERT(I,J,K,X)                                  \
    if (X) ; else do { BSLMT_TESTUTIL_NESTED_OUTPUT_GUARD;                    \
                       bsl::cout << #I << ": " << (I) << "\t"                 \
                                 << #J << ": " << (J) << "\t"                 \
                                 << #K << ": " << (K) << "\n";                \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_LOOP4_ASSERT(I,J,K,L,X)                                \
    if (X) ; else do { BSLMT_TESTUTIL_NESTED_OUTPUT_GUARD;                    \
                       bsl::cout << #I << ": " << (I) << "\t"                 \
                                 << #J << ": " << (J) << "\t"                 \
                                 << #K << ": " << (K) << "\t"                 \
                                 << #L << ": " << (L) << "\n";                \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)                              \
    if (X) ; else do { BSLMT_TESTUTIL_NESTED_OUTPUT_GUARD;                    \
                       bsl::cout << #I << ": " << (I) << "\t"                 \
                                 << #J << ": " << (J) << "\t"                 \
                                 << #K << ": " << (K) << "\t"                 \
                                 << #L << ": " << (L) << "\t"                 \
                                 << #M << ": " << (M) << "\n";                \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)                            \
    if (X) ; else do { BSLMT_TESTUTIL_NESTED_OUTPUT_GUARD;                    \
                       bsl::cout << #I << ": " << (I) << "\t"                 \
                                 << #J << ": " << (J) << "\t"                 \
                                 << #K << ": " << (K) << "\t"                 \
                                 << #L << ": " << (L) << "\t"                 \
                                 << #M << ": " << (M) << "\t"                 \
                                 << #N << ": " << (N) << "\n";                \
                       aSsErT(1, #X, __LINE__); } while (false)

// The 'BSLMT_TESTUTIL_EXPAND' macro is required to work around a preprocessor
// issue on Windows that prevents '__VA_ARGS__' from being expanded in the
// definition of 'BSLMT_TESTUTIL_NUM_ARGS'.

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

// The following 4 macros can either be used within a block guarded by a
// 'BSLMT_TESTUTIL_GUARD', in which case they don't acquire the mutex, or
// outside such a block, in which case they do acquire it.

#define BSLMT_TESTUTIL_Q(X)                                                   \
    do { BSLMT_TESTUTIL_NESTED_OUTPUT_GUARD;                                  \
         bsl::cout << "<| " #X " |>" << bsl::endl; } while (false)
    // Quote identifier literally.

#define BSLMT_TESTUTIL_P(X)                                                   \
    do { BSLMT_TESTUTIL_NESTED_OUTPUT_GUARD;                                  \
         bsl::cout << #X " = " << (X) << bsl::endl; } while (false)
    // Print identifier and its value.

#define BSLMT_TESTUTIL_P_(X)                                                  \
    do { BSLMT_TESTUTIL_NESTED_OUTPUT_GUARD;                                  \
         bsl::cout << #X " = " << (X) << ", " << bsl::flush; } while (false)
    // 'P(X)' without '\n'

#define BSLMT_TESTUTIL_T_                                                     \
    { BSLMT_TESTUTIL_NESTED_OUTPUT_GUARD;                                     \
      bsl::cout << "\t" << bsl::flush; }
    // Print tab (w/o newline).  Do not put in a 'do {} while(false)', as this
    // macro is intended to be called without a terminating ';'.

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

    static bslmt::Mutex& outputMutexSingleton_impl();
        // Return a reference to the constructed output mutex, initializing it
        // if necessary.  This function is not intended to be called by clients
        // of this component, rather, only by macros defined in this component.

    static void setFunc(Func func);
        // Set the function to be called by 'callFunc' to the specified 'func'.
};

                          // ===========================
                          // class TestUtil::GuardObject
                          // ===========================

class TestUtil::GuardObject {
    // DATA
    bslmt::Mutex *d_mutex_p;

  private:
    // NOT IMPLEMENTED
    GuardObject(const GuardObject&);
    GuardObject& operator=(const GuardObject&);

  public:
    // CREATORS
    GuardObject();
        // Acquire and lock the output mutex that is delivered by
        // 'TestUtil::outputMutexSingleton_impl()'.

    ~GuardObject();
        // Release the mutex acquired at construction.
};

                          // ===========================
                          // class TestUtil::NestedGuard
                          // ===========================

class TestUtil::NestedGuard {
    // DATA
    bslmt::Mutex *d_mutex_p;

  private:
    // NOT IMPLEMENTED
    NestedGuard(const NestedGuard&);
    NestedGuard& operator=(const NestedGuard&);

  public:
    // CREATORS
    explicit
    NestedGuard(char *);
        // Acquire and lock the output mutex that is delivered by
        // 'TestUtil::outputMutexSingleton_impl()'.

    explicit
    NestedGuard(GuardObject *);
        // Initialize 'd_mutex_p' to 0.

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
