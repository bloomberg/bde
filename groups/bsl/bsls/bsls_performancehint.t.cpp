// bsls_performancehint.t.cpp                                         -*-C++-*-
#include <bsls_performancehint.h>
#include <bsls_stopwatch.h>

#include <stdlib.h>
#include <iostream>

// Warning: the following 'using' declarations interfere with the testing of
// the macros defined in this component.  Please do not uncomment them.
// using namespace BloombergLP;
// using namespace std;

using std::cout;
using std::cerr;
using std::endl;
using std::flush;

//=============================================================================
//                                 TEST  PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This component provides MACROs to facilitate more intelligent code
// generation by the compiler with regards to branch prediction and
// prefetching.  It is difficult to ensure the compiler behaves exactly as
// *hinted*.  This is because the hint is only taken under optimized build and
// various other optimizations are performed simultaneously by the compiler.
// Therefore, only the small usage example will be verified.
//
// In addition, macro safety is tested in all test cases.  The common
// 'using namespace BloombergLP' statement is intentionally commented out to
// test that these macros function outside namespace 'BloombergLP'.
// ----------------------------------------------------------------------------
// [ 1] Usage Example: Using 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY' and
//                     'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY'
// [ 2] Usage Example: Using 'BSLS_PERFORMANCEHINT_PREDICT_EXPECT'
// [ 3] Usage Example: Using 'prefetchForReading' and 'prefetchForWriting'
//-----------------------------------------------------------------------------
// [-1] Performance Test: Verifies the performance of test 1, 2, 3
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace TestCase1 {

const int TESTSIZE = 10000000;  // test size used for timing
int global;                     // uninitialized on purpose to prevent compiler
                                // optimization

}  // close namespace TestCase1

namespace TestCase3 {

const int SIZE = 10 * 1024 * 1024;  // big enough so not all data sits in cache

#if defined(BSLS_PLATFORM_CMP_SUN)
    // For some reason the sun machine is A LOT slower than the other
    // platforms, even in optimized mode.
const int TESTSIZE = 10;
#else
const int TESTSIZE = 100;
#endif

volatile int array1[SIZE]; // for 'addWithPrefetch'
volatile int array2[SIZE]; // for 'addWithPrefetch'

volatile int array3[SIZE]; // for 'addWithoutPrefetch
volatile int array4[SIZE]; // for 'addWithoutPrefetch

}  // close namespace TestCase3

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace TestCase1 {

volatile int count1 = 0;
volatile int count2 = 0;

void foo()
    // Dummy function that sets the global variable.  Used to prevent the
    // compiler from optimizing the code too much.
{
    global = 1;
    count1++;
    count1++;
    count1++;
    count1++;

    count1++;
    count1++;
    count1++;
    count1++;
}

void bar()
    // Dummy function that sets the global variable.  Used to prevent the
    // compiler from optimizing the code too much.
{
    global = 2;
    count2++;
    count2++;
    count2++;
    count2++;

    count2++;
    count2++;
    count2++;
    count2++;
}

void testCase1(int argc, bool assert)
{
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    BloombergLP::bsls::Stopwatch timer;

    timer.reset();

    if (veryVerbose) {
        cout << "BSLS_PERFORMANCEHINT_PREDICT_LIKELY" << endl;
    }

    timer.start();

    for (int x = 0; x < TESTSIZE; ++x) {
        int y = rand() % 10;

        // Incorrect usage of 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY' since there
        // is only a one in ten chance that this branch is taken.

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(y == 8)) {
            foo();
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            bar();
        }
    }

    timer.stop();
    double likelyTime = timer.elapsedTime();

    if (veryVerbose) {
        cout << "\ttime = " << likelyTime << endl;
    }


    if (veryVerbose) {
        cout << "BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY" << endl;
    }

    timer.reset();
    timer.start();

    for (int x = 0; x < TESTSIZE; ++x) {
        int y = rand() % 10;

        // Correct usage of 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY' since there
        // is only a one in ten chance that this branch is taken.

        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(y == 8)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            foo();
        }
        else {
            bar();
        }
    }
    timer.stop();
    double unlikelyTime = timer.elapsedTime();

    if (veryVerbose) {
        cout << "\ttime = " << unlikelyTime << endl;
    }

#if defined(BDE_BUILD_TARGET_OPT)
    // Only check under optimized build.

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_SUN) ||     \
   (defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VER_MAJOR >= 0x0900)\
    // Only check when 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY' and
    // 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY' expands into something
    // meaningful.

    double tolerance = 0.02;

    if (assert) {
        LOOP2_ASSERT(likelyTime, unlikelyTime,
                     likelyTime + tolerance > unlikelyTime);
    }

#endif

#endif
}

}  // close namespace TestCase1

namespace TestCase3 {

void init(volatile int *arrayA, volatile int *arrayB)
{
#if defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VER_MAJOR >= 0x0900
    // Only available under xlc 10.

    for (int i = 0; i < SIZE; ++i){
        __dcbf((const void *)(arrayA++));
        __dcbf((const void *)(arrayB++));
    }
#endif
}

void addWithoutPrefetch(volatile int *arrayA, volatile int *arrayB)
    // Performs some form of addition on the specified 'arrayA' and 'arrayB'
    // without using prefetch.
{
    for (int i = 0; i < SIZE/8; ++i){
        *(arrayA++) = *arrayA + *(arrayB++);
        *(arrayA++) = *arrayA + *(arrayB++);
        *(arrayA++) = *arrayA + *(arrayB++);
        *(arrayA++) = *arrayA + *(arrayB++);

        *(arrayA++) = *arrayA + *(arrayB++);
        *(arrayA++) = *arrayA + *(arrayB++);
        *(arrayA++) = *arrayA + *(arrayB++);
        *(arrayA++) = *arrayA + *(arrayB++);
    }
}

void addWithPrefetch(volatile int *arrayA, volatile int *arrayB)
    // Performs some form of addition on the specified 'arrayA' and 'arrayB'
    // using prefetch.
{
    for (int i = 0; i < SIZE/8; ++i){
        BloombergLP::bsls::PerformanceHint::prefetchForWriting(
                                                          (int *) arrayA + 16);
        BloombergLP::bsls::PerformanceHint::prefetchForReading(
                                                          (int *) arrayB + 16);

        *(arrayA++) = *arrayA + *(arrayB++);
        *(arrayA++) = *arrayA + *(arrayB++);
        *(arrayA++) = *arrayA + *(arrayB++);
        *(arrayA++) = *arrayA + *(arrayB++);

        *(arrayA++) = *arrayA + *(arrayB++);
        *(arrayA++) = *arrayA + *(arrayB++);
        *(arrayA++) = *arrayA + *(arrayB++);
        *(arrayA++) = *arrayA + *(arrayB++);
    }
}

void testCase3(int argc, bool assert)
{
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    if (veryVerbose) {
        cout << "Adding with prefetch" << endl;
    }

    TestCase3::init(TestCase3::array1, TestCase3::array2);

    BloombergLP::bsls::Stopwatch timer;
    timer.start();

    for(int i = 0; i < TESTSIZE; ++i) {
        TestCase3::addWithPrefetch(TestCase3::array1, TestCase3::array2);
    }

    timer.stop();
    double withPrefetch = timer.elapsedTime();

    if (veryVerbose) {
        cout << "\ttime = " << withPrefetch << endl;
    }

    if (veryVerbose) {
        cout << "Adding without prefetch" << endl;
    }

    TestCase3::init(TestCase3::array3, TestCase3::array4);

    timer.reset();
    timer.start();

    for(int i = 0; i < TestCase3::TESTSIZE; ++i) {
        addWithoutPrefetch(array3, array4);
    }

    timer.stop();
    double withoutPrefetch = timer.elapsedTime();

    if (veryVerbose) {
        cout << "\ttime = " << withoutPrefetch << endl;
    }

#if defined(BDE_BUILD_TARGET_OPT)
    // Only check under optimized build.

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_IBM) ||     \
    defined(BSLS_PLATFORM_CMP_SUN) || defined(BSLS_PLATFORM_OS_WINDOWS)
    // Only check when 'prefetchForReading' or 'prefetchForWriting' expands
    // expands into something meaningful.

    double tolerance = 0.02;

    // Note that when compiling using 'bde_build.pl -t opt_exc_mt', the
    // optimization flag is set to '-O'.  Whereas, the optimization flag used
    // by 'IS_OPTIMIZED=1 pcomp' is set to '-xO2'.  Therefore, the improvement
    // in efficiency is much less than what's described in the usage example
    // when compiled using bde_build.
    if (assert) {
        // Only assert in performance test case.
        LOOP2_ASSERT(withoutPrefetch, withPrefetch,
                     withoutPrefetch + tolerance > withPrefetch);
    }

#endif

#endif

}

}  // close namespace TestCase3

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   This will test the usage example 3 provided in the component
        //   header file for 'prefetchForReading' and 'prefetchForWriting'.
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example using 'prefetchForReading' and
        //   'prefetchForWriting'.
        //
        // Testing:
        //   prefetchForReading
        //   prefetchForWriting
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example 3"
                          << "\n=======================" << endl;

        TestCase3::testCase3(argc, false);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   This will test the usage example 2 provided in the component
        //   header file for 'BSLS_PERFORMANCEHINT_PREDICT_EXPECT'.
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Ensure the usage example compiles.
        //
        // Testing:
        //   BSLS_PERFORMANCEHINT_PREDICT_EXPECT
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example 2"
                          << "\n=======================" << endl;

        int x = rand() % 4;

        // Incorrect usage of 'BSLS_PERFORMANCEHINT_PREDICT_EXPECT', since the
        // probability of getting a 3 is equivalent to other numbers (0, 1, 2).
        // However, this is sufficient to illustrate the intent of this macro.

        switch(BSLS_PERFORMANCEHINT_PREDICT_EXPECT(x, 3)) {
          case 1: //..
                  break;
          case 2: //..
                  break;
          case 3: //..
                  break;
          default: break;
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   This will test the usage example 1 provided in the component
        //   header file for 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY' and
        //   'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY'.
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example using 'BSLS_PERFORMANCEHINT_PREDICT_LIKELY'
        //   and 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY'.
        //
        // Testing:
        //   BSLS_PERFORMANCEHINT_PREDICT_LIKELY,
        //   BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example 1"
                          << "\n=======================" << endl;

        TestCase1::testCase1(argc, false);

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //   Test the improvement of performance in using various performance
        //   hints.
        //
        // Concerns:
        //   The performance of using the performance hints in the various
        //   usage examples must be faster than not using them.
        //
        // Plan:
        //   Using 'bsls::Stopwatch', compare the time it takes to run the test
        //   using the performance hints and not using the hints.  Then compare
        //   and assert the time difference.  The test driver must observe an
        //   improvement in performance.  To minimize the effect of caching
        //   biasing the result, run the version that uses the proper hint
        //   first.
        //
        // Testing:
        //   Performance
        // --------------------------------------------------------------------

        if (verbose) cout << "\nPerformance Test"
                          << "\n================" << endl;

        if (veryVerbose) {
            cout << "Usage Example 1:" << endl;
        }

        TestCase1::testCase1(argc, true);

        if (veryVerbose) {
            cout << "Usage Example 3:" << endl;
        }

        TestCase3::testCase3(argc, true);

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
