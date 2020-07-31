// bslmt_testutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMT_TESTUTIL
#define INCLUDED_BSLMT_TESTUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide thread-safe test utilities for multithreaded components.
//
//@CLASSES:
//  bslmt::TestUtil: namespace class for multi-threaded utility functions.
//
//@MACROS:
//  BSLMT_TESTUTIL_ASSERT(X): record and print message if '!X'
//  BSLMT_TESTUTIL_ASSERTV(..., X): record and print args and message if '!X'
//  BSLMT_TESTUTIL_Q(X): quote identifier literally
//  BSLMT_TESTUTIL_P(X): print identifier and value
//  BSLMT_TESTUTIL_P_(X): print identifier and value without '\n'
//  BSLMT_TESTUTIL_L_: current line number
//  BSLMT_TESTUTIL_T_: print tab without '\n'
//  BSLMT_TESTUTIL_GUARD: create a lock guard on the singleton mutex
//  BSLMT_TESTUTIL_GUARDED_STREAM(STREAM): guarded access to 'STREAM'
//  BSLMT_TESTUTIL_COUT: thread-safe access to 'bsl::cout'
//  BSLMT_TESTUTIL_CERR: thread-safe access to 'bsl::cerr'
//
//  BSLMT_TESTUTIL_LOOP_ASSERT
//  BSLMT_TESTUTIL_LOOP2_ASSERT
//  BSLMT_TESTUTIL_LOOP3_ASSERT
//  BSLMT_TESTUTIL_LOOP4_ASSERT
//  BSLMT_TESTUTIL_LOOP5_ASSERT
//  BSLMT_TESTUTIL_LOOP6_ASSERT
//      // Discouraged: use 'ASSERTV'.  Provided for the sake of ease of
//      // migration of test drivers from using 'bsls_bsltestutil' or
//      // 'bslim_testutil'.
//
//  BSLMT_TESTUTIL_OUTPUT_GUARD
//      // !DEPRECATED!: use BSLMT_TESTUTIL_GUARD
//
//  BSLMT_TESTUTIL_NESTED_OUTPUT_GUARD
//      // !DEPRECATED!: use BSLMT_TESTUTIL_GUARD
//
//@SEE_ALSO: bslim_testutil, bsls_bsltestutil
//
//@DESCRIPTION: This component provides a set of macros for the standard BDE
// test driver assert and print facilities (see the macros defined in
// {'bsls_bsltestutil'} and {'bslim_testutil'}) that are suitable for use in
// multi-threaded test drivers.  This component also provides a utility
// 'struct', 'bslmt::TestUtil', that defines a namespace for types and
// functions that are helpful in multi-threaded test drivers.
//
///Simple Output Macros
///--------------------
//: o 'BSLMT_TESTUTIL_Q(X)'
//: o 'BSLMT_TESTUTIL_P(X)'
//: o 'BSLMT_TESTUTIL_P_(X)'
//: o 'BSLMT_TESTUTIL_L_'
//: o 'BSLMT_TESTUTIL_T_'
//
// These macros guarantee:
//
//: 1 The entire output message is not interleaved with those of other threads
//:   using these macros.
//:
//: 2 The output stream (e.g., 'bsl::cout') is not corrupted.  (Since
//:   'bsl::cout' itself was not made thread-safe until C++11 this is a concern
//:   on some of our supported platforms.)
//
///Guarded Stream Output
///---------------------
// This component also defines several additions to the set of standard test
// macros:
//: o 'BSLMT_TESTUTIL_GUARDED_STREAM(STREAM)'
//: o 'BSLMT_TESTUTIL_COUT'
//: o 'BSLMT_TESTUTIL_CERR'
//
// These macros guarantee atomicity of the output for the entire statement up
// to the terminating semi-colon.  For example, if one thread executes:
//..
//  int value = 42;
//  BSLMT_TESTUTIL_COUT << "The value is " << value << "." << bsl::endl;
//..
// and another thread executes;
//..
//  bsl::string name("Barbara");
//  BSLMT_TESTUTIL_COUT << "Hello " << name << "."  << bsl::endl;
//..
// The output will be either:
//..
//  Hello Barbara.
//  The value is 42.
//..
// or
//..
//  The value is 42.
//  Hello Barbara.
//..
// but, despite six separate operations on 'bsl::cout', the lines will not be
// interleaved.
//
// As with the "ASSERT" macros above, these macros do not guarantee thread-safe
// evaluation of any expressions in the output statement.  Thus, if one thread
// is executing:
//..
//  BSLMT_TESTUTIL_COUT << "The value is " << f() << "." << bsl::endl;
//..
// and some other thread is executing:
//..
//  BSLMT_TESTUTIL_COUT << "There are " << g() << " cases." << bsl::endl;
//..
// the two output statements are evaluated serially, but the evaluation of the
// 'f' and 'g' functions may require their own synchronization (if they are
// accessing values that may be in the process of unguarded modification by
// other threads).
//
///Assertion Macros
///----------------
// The macros:
//
//: o BSLMT_TESTUTIL_ASSERT
//: o BSLMT_TESTUTIL_ASSERTV, and
//: o BSLMT_TESTUTIL_LOOP*_ASSERT
//
// work like their counterparts in 'bsls_bsltestutil' and 'bslim_testutil',
// except that all output by a given assert is guaranteed not to be interleaved
// with output from other macros in this component.
//
// These macros provide no synchronization until *after* failure is detected.
// If the boolean expression being evaluated is not thread-safe, the caller of
// the macro must provide synchronization around the macro call.
//
// The 'LOOP*_ASSERT' macros are considered obsolete and are provided to
// facilitate the multi-threading of test drivers written using macros from
// 'bsls_bsltestutil' and 'bslim_testutil'.  Use 'BSLMT_TESTUTIL_ASSERTV'
// instead.
//
///Infrastructure for the Assertion Macros
///- - - - - - - - - - - - - - - - - - - -
// Similarly to the macros defined in {'bsls_bsltestutil'} and
// {'bslim_testutil'}, the assert macros defined here assume that the user has
// defined a global integer variable named 'testStatus' and has defined a
// function:
//..
//  void aSsErT(int c, const char *assertionAsString, int lineNumber);
//..
// that outputs the conventional error message and update 'testStatus' when 'c'
// is non-zero.  If all access to 'testStatus' is through the "ASSERT" macros,
// 'testStatus' can be an ordinary (non-atomic) 'int' and 'aSsErT' need not be
// thread-safe.
//
///Guarding Critical Sections
///--------------------------
// This component also provides a (macro-wrapped) guard object:
//
//: o 'BSLMT_TESTUTIL_GUARD'
//
// which does no output, but rather prevents the interleaving of output from
// the other macros until the guard goes out of scope.  This feature is useful
// in the common situation in which one wants to prevent interleaving of
// several separate uses of the macros.
//
// For example, assuming the conventional shorthand for the standard test
// macros:
//..
//  #define P                        BSLMT_TESTUTIL_P
//  #define P_                       BSLMT_TESTUTIL_P_
//  #define T_                       BSLMT_TESTUTIL_T_
//
//  #define COUT                     BSLMT_TESTUTIL_COUT
//..
// and
//..
//  #define GUARD                    BSLMT_TESTUTIL_GUARD
//..
// consider two threads, one of which executes:
//..
//  int a = 4 * 5, b = 7 / 2, c = 5;
//
//  T_    P_(a);    P_(b);    P(c * 2);
//..
// while the other thread executes:
//..
//  int x = 3 * 7, y = 45, z = 103;
//
//  T_    P_(x);    P_(y);    P(z * 2);
//..
// In total, there are eight output operations on 'bsl::cout'.  Though the
// order of outputs within each thread is guaranteed, there is no guarantee
// that the entire line of output of either thread will be finished before the
// other begins.  One possible result (among other myriad possibilities) is:
//..
//          x = 21,     a = 20, b = 3, y = 45, z = 206
//  c * 2 = 10
//
//..
// Clearly, such output is difficult to interpret.  Fortunately, we have a
// remedy.
//
// The two sets of output operation can be serialized by requiring the creation
// of a 'GUARD' object before each set of output operations, and destroying
// that object afterwards.  The existence of a guard object on the stack of one
// thread blocks the construction of guard objects by *other* threads until the
// existing guard object is destroyed.
//
// The behavior is undefined if guard objects are created anywhere but on the
// stack (i.e., automatic variables).
//
// Our revised (guarded) code can be written this way in one thread:
//..
//  int a = 4 * 5, b = 7 / 2, c = 5;
//  {
//      GUARD;
//
//      T_    P_(a);    P_(b);    P(c * 2);
//  }
//..
// and this way in the other thread:
//..
//  int x = 3 * 7, y = 45, z = 103;
//  {
//      GUARD;
//
//      T_    P_(x);    P_(y);    P(z * 2);
//  }
//..
// Now, possibility of the two threads interleaving this output has been
// eliminated and the output will appear in either this order:
//..
//      a = 20, b = 3, c * 2 = 10
//      x = 21, y = 45, z * 2 = 206
//..
// or this order:
//..
//      x = 21, y = 45, z * 2 = 206
//      a = 20, b = 3, c * 2 = 10
//..
// Note that each *individual* thread can own several guard objects at a given
// time.  (Recall the constructor blocks if guards exist on *other* threads).
// Thus, guarded sections can call, without fear of deadlock, functions that
// may create their own guard objects.
//
// If a guard exists in a thread, more guards can be created in the same thread
// with no effect.  Guards cannot be created in another thread until all of the
// guards have been destroyed.  So the above example could have been:
//..
//  int a = 4 * 5, b = 7 / 2, c = 5;
//  {
//      GUARD;
//      GUARD;
//      GUARD;
//      GUARD;
//
//      T_    P_(a);    P_(b);    P(c * 2);
//  }
//..
// and in the other thread:
//..
//  int x = 3 * 7, y = 45, z = 103;
//  {
//      GUARD;
//
//      T_    P_(x);
//
//      {
//          GUARD;
//
//          P_(y);    P(z * 2);
//      }
//  }
//..
// and the result would have been EXACTLY the same.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Use of Thread-Safe Asserts and Guard in a Test Driver
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we write a function, 'sumOfSquares', to test:
//..
//  namespace xyzde {
//
//  struct SumUtil {
//      // This utility class provides sample functionality to demonstrate how
//      // a multi-threaded test driver might be written.
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
//      double ret = a * a;
//      ret += b * b;
//      ret += c * c;
//      ret += d * d * d;
//      return ret;
//  }
//
//  }  // close namespace xyzde
//..
// Then, we can write a test driver for this component.  We start by providing
// the standard BDE 'ASSERT' test macro, which is not thread-safe, and is the
// same as it is for a test driver using 'bslim_testutil'.  The macros in
// 'bslmt_testutil' ensure that any time this function is called, the global
// mutex has been acquired.
//..
//  // ========================================================================
//  //                       STANDARD BDE ASSERT TEST FUNCTION
//  // ------------------------------------------------------------------------
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
// Next, we define the standard output and 'ASSERT*' macros, as aliases to the
// macros defined by this component:
//..
//  // ========================================================================
//  //                       STANDARD BDE TEST DRIVER MACROS
//  // ------------------------------------------------------------------------
//
//  #define ASSERT                   BSLMT_TESTUTIL_ASSERT
//  #define ASSERTV                  BSLMT_TESTUTIL_ASSERTV
//
//  #define LOOP_ASSERT              BSLMT_TESTUTIL_LOOP_ASSERT
//  #define LOOP2_ASSERT             BSLMT_TESTUTIL_LOOP2_ASSERT
//  #define LOOP3_ASSERT             BSLMT_TESTUTIL_LOOP3_ASSERT
//  #define LOOP4_ASSERT             BSLMT_TESTUTIL_LOOP4_ASSERT
//  #define LOOP5_ASSERT             BSLMT_TESTUTIL_LOOP5_ASSERT
//  #define LOOP6_ASSERT             BSLMT_TESTUTIL_LOOP6_ASSERT
//
//  #define GUARD                    BSLMT_TESTUTIL_GUARD
//
//  #define Q                        BSLMT_TESTUTIL_Q
//  #define P                        BSLMT_TESTUTIL_P
//  #define P_                       BSLMT_TESTUTIL_P_
//  #define T_                       BSLMT_TESTUTIL_T_
//  #define L_                       BSLMT_TESTUTIL_L_
//
//  #define GUARDED_STREAM(STREAM)   BSLMT_TESTUTIL_GUARDED_STREAM(STREAM)
//  #define COUT                     BSLMT_TESTUTIL_COUT
//  #define CERR                     BSLMT_TESTUTIL_CERR
//..
// Then, we define global verbosity flags to be used for controlling debug
// traces.  The flags will be set by elided code at the beginning of 'main' to
// determine the level of output verbosity the client wants:
//..
//  // ========================================================================
//  //                     GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//  // ------------------------------------------------------------------------
//
//  bool  verbose;
//  bool  veryVerbose;
//  bool  veryVeryVerbose;
//  bool  veryVeryVeryVerbose;
//..
// Next begin the usage test case, defining a 'typedef' and some 'enum's used
// by this test case:
//..
//                              // ---------------
//                              // Usage Test Case
//                              // ---------------
//
//  typedef  xyzde::SumUtil SU;
//
//  enum { k_NUM_THREADS =    5,
//         k_HI_LIMIT_X  =  100,
//         k_LO_LIMIT_X  = -100 };
//..
// Then, using our test macros, we write our test functor that can be run
// concurrently to test the static function:
//..
//  struct SumUtilTest {
//      void operator()()
//          // Test 'TestUtil::sumOfSquares' with a variety of randomly
//          // generated arguments.
//      {
//          int    threadIdx;
//          double x[4];                // randomly-generated test values
//..
// Next, we use the 'GUARD' macro to serialize the initialization of
// 'threadIdx' and the 'x' array.  We call 'bsl::srand' and 'bsl::rand', which
// are not thread-safe, so the calls to them must be mutex-guarded.  Because
// all access to 'masterThreadIdx' is guarded by the 'GUARD' call, it does not
// need to be an atomic variable.
//..
//          {
//              GUARD;
//
//              static int masterThreadIdx = 0;
//              threadIdx = masterThreadIdx++;
//
//              unsigned randSeed = (1234567891 + threadIdx) * 3333333333U;
//              bsl::srand(randSeed);
//
//              for (int ii = 0; ii < 4; ++ii) {
//                  // Note that 'bsl::rand' always returns a non-negative
//                  // value.
//
//                  const double characteristic =  bsl::rand() % k_HI_LIMIT_X;
//                  const double mantissa       =
//                             static_cast<double>(bsl::rand() % 1024) / 1024;
//                  const int    sign           = (bsl::rand() & 1) ? +1 : -1;
//
//                  // Note that it is safe to use 'ASSERTV', which redundantly
//                  // locks the mutex, even though the mutex has already been
//                  // acquired by the 'GUARD' call above.
//
//                  ASSERTV(threadIdx,ii, characteristic, 0 <= characteristic);
//                  ASSERTV(threadIdx,ii, characteristic,
//                                              characteristic < k_HI_LIMIT_X);
//                  ASSERTV(threadIdx,ii, mantissa,       0 <= mantissa);
//                  ASSERTV(threadIdx,ii, mantissa,       mantissa < 1);
//
//                  x[ii] = sign * (characteristic + mantissa / 1000);
//              }
//..
// Then we close the block, allowing other threads to do output with the
// 'BSLMT_TESTUTIL_*' macros or enter sections guarded by 'GUARD's.  Now, if we
// want to do output, we have to acquire the critical section again, which we
// can do by using the 'COUT' (aliased to 'BSLMT_TESTUTIL_COUT') macro:
//..
//          }
//
//          if (veryVerbose) COUT << "threadIdx: " << threadIdx <<
//                     ", x[] = { " << x[0] << ", " << x[1] << ", " << x[2] <<
//                                                       ", " << x[3] <<" }\n";
//..
// Next, if any of the 'ASSERTV's following this point fail with no 'GUARD'
// call in scope, they will lock the mutex before doing output.  Note that the
// 'ASSERTV's do not lock the mutex while checking to see if the predicate
// passed to them is 'false'.
//..
//          for (int ii = 0; ii < 4; ++ii) {
//              ASSERTV(threadIdx, ii, x[ii], x[ii] < k_HI_LIMIT_X);
//              ASSERTV(threadIdx, ii, x[ii], k_LO_LIMIT_X < x[ii]);
//          }
//
//          double exp = x[0] * x[0];
//          ASSERTV(x[0], exp, SU::sumOfSquares(x[0]),
//                                              exp == SU::sumOfSquares(x[0]));
//
//          exp += x[1] * x[1];
//          ASSERTV(x[0], x[1], exp, SU::sumOfSquares(x[0], x[1]),
//                                        exp == SU::sumOfSquares(x[0], x[1]));
//
//          exp += x[2] * x[2];
//          ASSERTV(x[0], x[1], x[2], exp, SU::sumOfSquares(x[0], x[1], x[2]),
//                                  exp == SU::sumOfSquares(x[0], x[1], x[2]));
//
//          exp += x[3] * x[3];
//          ASSERTV(x[0], x[1], x[2], x[3], exp,
//                                    SU::sumOfSquares(x[0], x[1], x[2], x[3]),
//                            exp == SU::sumOfSquares(x[0], x[1], x[2], x[3]));
//..
// Then, if we want to do any more output, since the mutex has not been
// acquired at this point, we have to re-acquire it.  We have a choice between
// using 'COUT' again, as we did above, or by using 'GUARD' and 'bsl::cout':
//..
//          if (veryVerbose) {
//              GUARD;
//
//              bsl::cout << "Thread number " << threadIdx << " finishing.\n";
//          }
//      }
//  };
//..
// Next, in 'main', we spawn our threads and let them run:
//..
//  int main()
//  {
//      // ..
//
//      using namespace BloombergLP;
//
//      bslmt::ThreadGroup tg;
//      tg.addThreads(SumUtilTest(), k_NUM_THREADS);
//..
// Then, we join the threads:
//..
//      tg.joinAll();
//..
// Now, we observe output something like this (tabs eliminated, long lines
// wrapped).  Note that each of the five test threads reported a failure:
//..
//  x[0]: 24.0005  x[1]: 80.0001  x[2]: 14.0009  x[3]: 3.00029  exp: 7181.07
//  SU::sumOfSquares(x[0], x[1], x[2], x[3]): 7199.08
//  Error ../../bde/groups/bsl/bslmt/bslmt_testutil.t.cpp(380):
//                  exp == SU::sumOfSquares(x[0], x[1], x[2], x[3])    (failed)
//  x[0]: -81.0006  x[1]: -82.0009  x[2]: 36.0009  x[3]: -59.0002
//  exp: 18062.3  SU::sumOfSquares(x[0], x[1], x[2], x[3]): -190799
//  Error ../../bde/groups/bsl/bslmt/bslmt_testutil.t.cpp(380):
//                  exp == SU::sumOfSquares(x[0], x[1], x[2], x[3])    (failed)
//  x[0]: 46.0001  x[1]: -62.0004  x[2]: 75.0006  x[3]: -66.0008  exp: 15941.3
//  SU::sumOfSquares(x[0], x[1], x[2], x[3]): -275921
//  Error ../../bde/groups/bsl/bslmt/bslmt_testutil.t.cpp(380):
//                  exp == SU::sumOfSquares(x[0], x[1], x[2], x[3])    (failed)
//  x[0]: -18.0003  x[1]: -84.0006  x[2]: 79.0004  x[3]: 76.0007  exp: 19397.3
//  SU::sumOfSquares(x[0], x[1], x[2], x[3]): 452609
//  Error ../../bde/groups/bsl/bslmt/bslmt_testutil.t.cpp(380):
//                  exp == SU::sumOfSquares(x[0], x[1], x[2], x[3])    (failed)
//  x[0]: -55.0006  x[1]: 35.0004  x[2]: 54.0009  x[3]: -45.0002  exp: 9191.21
//  SU::sumOfSquares(x[0], x[1], x[2], x[3]): -83960.1
//  Error ../../bde/groups/bsl/bslmt/bslmt_testutil.t.cpp(380):
//                  exp == SU::sumOfSquares(x[0], x[1], x[2], x[3])    (failed)
//..
// Finally, at the end of 'main' examine 'testStatus'.  If it's greater than 0,
// report that the test failed.  Note that since there is a bug in
// 'SU::sumOfSquares' with 4 args, we expect the last assert in
// 'SumUtil::operator()' to fail 5 times, so the following message will report
// 'test status = 5'.
//..
//
//      if (testStatus > 0) {
//          bsl::cerr << "Error, non-zero test status = " << testStatus << "."
//                    << bsl::endl;
//      }
//
//      return testStatus;
//  }
//..

#include <bslscm_version.h>

#include <bslmt_recursivemutex.h>
#include <bsls_keyword.h>
#include <bsls_unspecifiedbool.h>

#include <bsl_iostream.h>
#include <bsl_string.h>

                           // =================
                           // Macro Definitions
                           // =================

#define BSLMT_TESTUTIL_GUARD_NAME_IMPL                                        \
                                  bloomberglp_bslmt_tEsTuTiL_GuArD_ ## __LINE__

#define BSLMT_TESTUTIL_GUARD                                                  \
        BloombergLP::bslmt::TestUtil_Guard BSLMT_TESTUTIL_GUARD_NAME_IMPL
    // Acquire the critical section, and release it when the macro call goes
    // out of scope.  If the critical section is already held by the current
    // thread, this can still be used with no additional effect.

#define BSLMT_TESTUTIL_OUTPUT_GUARD           BSLMT_TESTUTIL_GUARD
#define BSLMT_TESTUTIL_NESTED_OUTPUT_GUARD    BSLMT_TESTUTIL_GUARD

#define BSLMT_TESTUTIL_GUARDED_STREAM(STREAM)                                 \
    for (BSLMT_TESTUTIL_GUARD; BSLMT_TESTUTIL_GUARD_NAME_IMPL;                \
                               BSLMT_TESTUTIL_GUARD_NAME_IMPL = false)        \
        (STREAM)
    // Enable output to the specified 'STREAM' that will not be interleaved
    // with output from 'bslmt_testutil' macros done by other threads.
    // 'STREAM' is expected to be a 'bsl::ostream' object, and this macro call
    // can be output to with the C++ '<<' operator with all output until the
    // terminating ';' occurring as one non-interleaved block.

#define BSLMT_TESTUTIL_COUT    BSLMT_TESTUTIL_GUARDED_STREAM(bsl::cout)
    // Equivalent to call to 'bsl::cout' guarded by a 'BSLMT_TESTUTIL_GUARD'
    // whose scope ends at the end of the statement.

#define BSLMT_TESTUTIL_CERR    BSLMT_TESTUTIL_GUARDED_STREAM(bsl::cerr)
    // Equivalent to call to 'bsl::cerr' guarded by a 'BSLMT_TESTUTIL_GUARD'
    // whose scope ends at the end of the statement.

#define BSLMT_TESTUTIL_LOOP0_ASSERT(X)                                        \
    if (X) ; else do { BSLMT_TESTUTIL_GUARD;                                  \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_ASSERT(X) BSLMT_TESTUTIL_LOOP0_ASSERT(X)

#define BSLMT_TESTUTIL_LOOP1_ASSERT(I,X)                                      \
    if (X) ; else do { BSLMT_TESTUTIL_GUARD;                                  \
                       bsl::cout << #I << ": " << (I) << "\n";                \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_LOOP_ASSERT(X) BSLMT_TESTUTIL_LOOP1_ASSERT(X)

#define BSLMT_TESTUTIL_LOOP2_ASSERT(I,J,X)                                    \
    if (X) ; else do { BSLMT_TESTUTIL_GUARD;                                  \
                       bsl::cout << #I << ": " << (I) << "\t"                 \
                                 << #J << ": " << (J) << "\n";                \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_LOOP3_ASSERT(I,J,K,X)                                  \
    if (X) ; else do { BSLMT_TESTUTIL_GUARD;                                  \
                       bsl::cout << #I << ": " << (I) << "\t"                 \
                                 << #J << ": " << (J) << "\t"                 \
                                 << #K << ": " << (K) << "\n";                \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_LOOP4_ASSERT(I,J,K,L,X)                                \
    if (X) ; else do { BSLMT_TESTUTIL_GUARD;                                  \
                       bsl::cout << #I << ": " << (I) << "\t"                 \
                                 << #J << ": " << (J) << "\t"                 \
                                 << #K << ": " << (K) << "\t"                 \
                                 << #L << ": " << (L) << "\n";                \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)                              \
    if (X) ; else do { BSLMT_TESTUTIL_GUARD;                                  \
                       bsl::cout << #I << ": " << (I) << "\t"                 \
                                 << #J << ": " << (J) << "\t"                 \
                                 << #K << ": " << (K) << "\t"                 \
                                 << #L << ": " << (L) << "\t"                 \
                                 << #M << ": " << (M) << "\n";                \
                       aSsErT(1, #X, __LINE__); } while (false)

#define BSLMT_TESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)                            \
    if (X) ; else do { BSLMT_TESTUTIL_GUARD;                                  \
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

// The following four macros guarantee atomicity of their output by implicitly
// creating a GUARD objects.  As with the other macros, these macros can be
// used within the scope of another GUARD object.

#define BSLMT_TESTUTIL_Q(X)                                                   \
    BSLMT_TESTUTIL_COUT << "<| " #X " |>" << bsl::endl
    // Quote identifier literally.

#define BSLMT_TESTUTIL_P(X)                                                   \
    BSLMT_TESTUTIL_COUT << #X " = " << (X) << bsl::endl
    // Print identifier and its value.

#define BSLMT_TESTUTIL_P_(X)                                                  \
    BSLMT_TESTUTIL_COUT << #X " = " << (X) << ", " << bsl::flush
    // 'P(X)' without '\n'

#define BSLMT_TESTUTIL_T_                                                     \
    BSLMT_TESTUTIL_COUT << "\t" << bsl::flush;
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
    //
    // The 'callFunc' function, because it's out-of-line implementation is in
    // the '.cpp' file, can be used to prevent unwanted inlining in test
    // drivers, particularly useful when testing the stack trace functionality.

    // PUBLIC TYPES
    typedef void *(*Func)(void *);
        // 'Func' is the type of a user-supplied callback functor that can be

    // CLASS METHODS
    static void *callFunc(Func func, void *arg);
        // Call the specified 'func', passing it the specified 'arg', and
        // return its returned value.
};

                              // ====================
                              // class TestUtil_Guard
                              // ====================

class TestUtil_Guard {
    // This 'class' defines an object that provides exclusive access to the
    // critical sections defined by this component's macros.  Multiple 'Guard'
    // objects can exist at once, but only in the same thread -- if an attempt
    // is made to construct a 'Guard' while any 'Guard's exist in other
    // threads, the constructor will block until all 'Guard' objects in other
    // threads are destroyed.  The behavior is undefined if a 'Guard' object is
    // anywhere but on the stack.

    // PRIVATE TYPE
    typedef bsls::UnspecifiedBool<TestUtil_Guard> UnspecifiedBool;

  public:
    // PUBLIC TYPE
    typedef UnspecifiedBool::BoolType             BoolType;

  private:
    // DATA
    RecursiveMutex *d_mutex_p;
    bool            d_boolState;    // boolean state assignable by 'operator='
                                    // and observable by casting the object to
                                    // 'bool'.

    // PRIVATE CLASS METHOD
    static RecursiveMutex& singletonMutex();
        // Return a non-'const' reference to the recursive mutex created by the
        // singleton.

  private:
    // NOT IMPLEMENTED
    TestUtil_Guard(const TestUtil_Guard&)            BSLS_KEYWORD_DELETED;
    TestUtil_Guard& operator=(const TestUtil_Guard&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    TestUtil_Guard();
        // Create a guard that locks the mutex in the singleton and saves the
        // address of the singleton mutex.

    ~TestUtil_Guard();
        // Unlock the recursive mutex that was locked by the constructor.

    // MANIPULATOR
    void operator=(bool rhs);
        // Set the boolean state of this object to the specified 'rhs'.

    // ACCESSOR
    operator BoolType() const;
        // Return the boolean state of this 'TestUtil_Guard' object.
};

// ============================================================================
//                           INLINE FUNCTION DEFINITIONS
// ============================================================================

                             // --------------------
                             // class TestUtil_Guard
                             // --------------------

inline
TestUtil_Guard::TestUtil_Guard()
: d_boolState(true)
{
    (d_mutex_p = &singletonMutex())->lock();
}

inline
TestUtil_Guard::~TestUtil_Guard()
{
    d_mutex_p->unlock();
}

inline
void TestUtil_Guard::operator=(bool rhs)
{
    d_boolState = rhs;
}

inline
TestUtil_Guard::operator TestUtil_Guard::BoolType() const
{
    return UnspecifiedBool::makeValue(d_boolState);
}

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
