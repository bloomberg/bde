// bslmt_throughputbenchmarkresult.t.cpp                              -*-C++-*-

#include <bslmt_throughputbenchmarkresult.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_ctime.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_vector.h>

#include <math.h>
#include <stddef.h>
#include <stdio.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a mechanism,
// 'bslmt::ThroughputBenchmarkResult', that provides result repository for
// throughput performance testing for multi-threaded components done by
// 'bslmt::ThroughputBenchmark'.  The results represent counts of the work done
// by each thread, thread group, and sample, divided by the number of actual
// seconds that sample has executed.  The primary manipulator are the methods
// for initializing the repository ('initialize') and setting values in it
// ('setThroughput').  The provided basic accessors are object state related
// methods for obtaining the allocator ('allocator'), number of samples
// ('numSamples'), number of thread groups ('numThreadGroups'), the number of
// threads in each thread group ('numThreads'), and a convenience function with
// the total number of threads ('totalNumThreads').  There are also the
// accessors for retrieving results: a specific throughput ('getValue'), median
// ('getMedian'), percentile ('getPercentile'), vector of percentiles
// ('getPercentiles'), and percentiles for each thread
// ('getThreadPercentiles').
//
// The validation is purely single threaded, as the only multi-threaded access
// is with 'setThroughput', but the memory is pre-allocated, and the access is
// only to different locations.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Any allocated memory is always from the object allocator.
//: o An object's value is independent of the allocator used to supply memory.
//: o Injected exceptions are safely propagated during memory allocation.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//: o ACCESSOR methods are 'const' thread-safe.
// ----------------------------------------------------------------------------
// [ 2] ThroughputBenchmarkResult(bslma::Allocator *basicAllocator = 0);
// [ 5] ThroughputBenchmarkResult(numSamples, threadGroupSizes, ba = 0);
// [ 6] ThroughputBenchmarkResult(original, ba = 0);
// [ 7] ThroughputBenchmarkResult(MRef<TBenchmarkResult> orig);
// [ 7] ThroughputBenchmarkResult(MRef<TBenchmarkResult> orig, ba);
// [ 8] ThroughputBenchmarkResult& operator=(const TBenchmarkResult& rhs);
// [ 9] ThroughputBenchmarkResult& operator=(MRef<TBenchmarkResult> rhs);
// [ 3] void initialize(numSamples, threadGroupSizes);
// [ 3] void setThroughput(tgIndex, threadIndex, sampleIndex, value);
// [ 4] int numSamples() const;
// [ 4] int numThreadGroups() const;
// [ 4] int numThreads(int threadGroupIndex) const;
// [ 4] int totalNumThreads() const;
// [ 3] double getValue(threadGroupIndex, threadIndex, sampleIndex) const;
// [10] void getMedian(double *median, int threadGroupIndex) const;
// [10] void getPercentile(*percentile, percentage, tGroupIndex) const;
// [10] void getPercentiles(*percentiles, threadGroupIndex) const;
// [10] void getThreadPercentiles(*percentiles, threadGroupIndex) const;
// [ 4] bslma::Allocator *allocator() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslmt::ThroughputBenchmarkResult          Obj;
typedef bslmt::ThroughputBenchmarkResult_TestUtil TestUtil;

// ============================================================================
//                    EXCEPTION TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define EXCEPTION_COUNT bslmaExceptionCounter

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

struct DefaultDataRow {
    int d_line;
    int d_numSamples;
    int d_numTGroups;
    int d_tgSizes[4];
};

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE NS NTG TG0 TG1 TG2 TG3
    //---- -- --- --- --- --- ---
    { L_,   1,  1,  5, -1, -1, -1},
    { L_,   2,  2,  3,  2, -1, -1},
    { L_,  10,  1,  5, -1, -1, -1},
    { L_, 100,  1,  5, -1, -1, -1},
    { L_,   5,  1,  6, -1, -1, -1},
    { L_,   1,  3,  7,  4,  5, -1},
    { L_,   4,  1, 10, -1, -1, -1},
};
enum { k_DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA };

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocatorRaw(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Calculate Median and Percentiles
///- - - - - - - - - - - - - - - - - - - - - -
// In the following example we populate a 'bslmt::ThroughputBenchmarkResult'
// object and calculate median and percentiles.
//
// First, we define a vector with thread group sizes:
//..
    bsl::vector<int> threadGroupSizes;
    threadGroupSizes.resize(2);
    threadGroupSizes[0] = 3;
    threadGroupSizes[1] = 2;
//..
// Next, we define a 'bslmt::ThroughputBenchmarkResult' with 10 samples and the
// previously defined thread group sizes:
//..
    bslmt::ThroughputBenchmarkResult myResult(10, threadGroupSizes);
//..
// Then, we populate the object with throughputs:
//..
    for (int tgId = 0; tgId < 2; ++tgId) {
        for (int tId = 0; tId < myResult.numThreads(tgId); ++tId) {
            for (int sId = 0; sId < 10; ++sId) {
                double throughput = static_cast<double>(rand());
                myResult.setThroughput(tgId, tId, sId, throughput);
            }
        }
    }

//..
// Now, we calculate median of the first thread group and print it out:
//..
    double median;
    myResult.getMedian(&median, 0);
    bsl::cout << "Median of first thread group:" << median << "\n";
//..
// Finally, we calculate percentiles 0, 0.25, 0.5, 0.75, and 1.0 of the first
// thread group and print it out:
//..
    bsl::vector<double> percentiles(5);
    myResult.getPercentiles(&percentiles, 0);
    for (int i = 0; i < 5; ++i) {
        bsl::cout << "Percentile " << 25 * i << "% is:"
                  << percentiles[i] << "\n";
    }
//..
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TEST PERCENTILE FUNCTIONS
        //
        // Concerns:
        //: 1 The 'get*' methods produce the correct values.
        //:
        //: 2 The method 'getPercentile' with an input of 0.5 produces the same
        //:   value as 'getMedian'.
        //:
        //: 3 The method 'getPercentiles' results are monotonically increasing.
        //:   If the size of 'percentiles' vector is odd, the middle value is
        //:   the same as the one produced by 'getMedian'.
        //:
        //: 4 Each of the inner vectors of the double 'percentiles' vector
        //:   given to 'getThreadPercentiles' is monotonically increasing.
        //:
        //: 5 The 'get*' method allocate temporary memory on the default
        //:   allocator.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create an object using a supplied test allocator, initialize it
        //:   to the desired number of samples and thread groups, and populate
        //:   it with a known set of values.
        //:
        //: 2 Verify that:
        //:   1 The dimensions of the vectors provided to 'getPercentiles' and
        //:     'getThreadPercentiles' did not change.
        //:   2 The median and other percentiles are what is expected.
        //:   3 Memory was allocated and released on the default allocator
        //:     for each of the 'get*' methods.
        //:
        //: 3 Create an object using a supplied test allocator, initialize it
        //:   to the desired number of samples and thread groups, and
        //:   repeatedly populate it with a random set of values.
        //:
        //: 4 Verify that:
        //:   1 The method 'getPercentile' with an input of 0.5 produces the
        //:     same value as 'getMedian'.
        //:   2 The method 'getPercentiles' results are monotonically
        //:     increasing.
        //:   3 If the size of 'percentiles' vector is odd, the middle value is
        //:     the same as the one produced by 'getMedian'.
        //:   4 Each of the inner vectors of the double 'percentiles' vector
        //:     given to 'getThreadPercentiles' is monotonically increasing.
        //:   5 Memory was allocated and released on the default allocator
        //:     for each of the 'get*' methods.
        //:
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid indexes, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //
        // Testing:
        //   void getMedian(double *median, int threadGroupIndex) const;
        //   void getPercentile(*percentile, percentage, tGroupIndex) const;
        //   void getPercentiles(*percentiles, threadGroupIndex) const;
        //   void getThreadPercentiles(*percentiles, threadGroupIndex) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST PERCENTILE FUNCTIONS" << endl
                          << "=========================" << endl;

        const double k_EPS = 1e-6; (void)k_EPS;

        bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&defaultAllocator);

        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        bslma::TestAllocatorMonitor dam(&defaultAllocator);
        bslma::TestAllocatorMonitor sam(&supplied);

        if (verbose) cout << "Full test case" << endl;
        {
            Obj      mX(&supplied);  const Obj& X = mX;

            bsl::vector<int> threadGroupSizes(2, &scratch);
            threadGroupSizes[0] = 3;
            threadGroupSizes[1] = 2;
            mX.initialize(5, threadGroupSizes);

            // Populate thread group 1.
            mX.setThroughput(1, 0, 0, 0.1);
            mX.setThroughput(1, 0, 1, 0.3);
            mX.setThroughput(1, 0, 2, 0.1);
            mX.setThroughput(1, 0, 3, 0.3);
            mX.setThroughput(1, 0, 4, 0.2);
            mX.setThroughput(1, 1, 0, 1.1);
            mX.setThroughput(1, 1, 1, 1.3);
            mX.setThroughput(1, 1, 2, 1.1);
            mX.setThroughput(1, 1, 3, 1.3);
            mX.setThroughput(1, 1, 4, 1.2);

            sam.reset();
            dam.reset();

            // Check 'getMedian', 'getPercentile'
            double median, percentile;
            X.getMedian(&median, 1);
            BSLS_ASSERT(1.4 == median);
            X.getPercentile(&percentile, 0.0 , 1);
            BSLS_ASSERT(fabs(1.2 - percentile) < k_EPS);
            X.getPercentile(&percentile, 0.25, 1);
            BSLS_ASSERT(fabs(1.2 - percentile) < k_EPS);
            X.getPercentile(&percentile, 0.5 , 1);
            BSLS_ASSERT(fabs(1.4 - percentile) < k_EPS);
            X.getPercentile(&percentile, 0.75, 1);
            BSLS_ASSERT(fabs(1.6 - percentile) < k_EPS);
            X.getPercentile(&percentile, 1.0 , 1);
            BSLS_ASSERT(fabs(1.6 - percentile) < k_EPS);

            // Check 'getPercentiles'
            bsl::vector<double> percentiles(5, &scratch);
            X.getPercentiles(&percentiles, 1);
            BSLS_ASSERT(fabs(1.2 - percentiles[0]) < k_EPS);
            BSLS_ASSERT(fabs(1.2 - percentiles[1]) < k_EPS);
            BSLS_ASSERT(fabs(1.4 - percentiles[2]) < k_EPS);
            BSLS_ASSERT(fabs(1.6 - percentiles[3]) < k_EPS);
            BSLS_ASSERT(fabs(1.6 - percentiles[4]) < k_EPS);

            // Check 'getThreadPercentiles'
            bsl::vector<bsl::vector<double> > tPercentiles(3, &scratch);
            tPercentiles[0].resize(2);
            tPercentiles[1].resize(2);
            tPercentiles[2].resize(2);
            X.getThreadPercentiles(&tPercentiles, 1);
            BSLS_ASSERT(fabs(0.1 - tPercentiles[0][0]) < k_EPS);
            BSLS_ASSERT(fabs(1.1 - tPercentiles[0][1]) < k_EPS);
            BSLS_ASSERT(fabs(0.2 - tPercentiles[1][0]) < k_EPS);
            BSLS_ASSERT(fabs(1.2 - tPercentiles[1][1]) < k_EPS);
            BSLS_ASSERT(fabs(0.3 - tPercentiles[2][0]) < k_EPS);
            BSLS_ASSERT(fabs(1.3 - tPercentiles[2][1]) < k_EPS);

            BSLS_ASSERT(sam.isTotalSame());
            BSLS_ASSERT(dam.isTotalUp());
            BSLS_ASSERT(dam.isInUseSame());
        }

        if (verbose) cout << "Random values" << endl;
        for (int i = 0; i < 10; ++i) {
            Obj      mX(&supplied);  const Obj& X = mX;

            bsl::vector<int> threadGroupSizes(2, &scratch);
            threadGroupSizes[0] = 3;
            threadGroupSizes[1] = 2;
            mX.initialize(5, threadGroupSizes);

            srand(static_cast<unsigned int>(time(0)));

            // Populate both thread groups.
            for (int tgId = 0; tgId < 2; ++tgId) {
                for (int tId = 0; tId < X.numThreads(tgId); ++tId) {
                    for (int sId = 0; sId < 5; ++sId) {
                        double throughput = static_cast<double>(rand());
                        mX.setThroughput(tgId, tId, sId, throughput);
                        if (veryVerbose) {
                            P_(tgId) P_(tId) P(sId)
                        }
                    }
                }
            }

            for (int tgId = 0; tgId < 2; ++tgId) {
                sam.reset();
                dam.reset();

                // Check 'getMedian', 'getPercentile'
                double median, percentile;
                X.getMedian(&median, tgId);
                X.getPercentile(&percentile, 0.5 , tgId);
                BSLS_ASSERT(fabs(median - percentile) < k_EPS);

                // Check 'getPercentiles'
                bsl::vector<double> percentiles(5, &scratch);
                X.getPercentiles(&percentiles, tgId);
                BSLS_ASSERT(median == percentiles[2]);
                for (int i = 0; i < 4; ++i) {
                    BSLS_ASSERT(percentiles[i] <= percentiles[i+1]);
                }

                // Check 'getThreadPercentiles'
                bsl::vector<bsl::vector<double> > tPercentiles(3, &scratch);
                tPercentiles[0].resize(X.numThreads(tgId));
                tPercentiles[1].resize(X.numThreads(tgId));
                tPercentiles[2].resize(X.numThreads(tgId));
                X.getThreadPercentiles(&tPercentiles, tgId);
                for (int i = 0; i < 2; ++i) {
                    for (int tId = 0; tId < X.numThreads(tgId); ++tId) {
                        BSLS_ASSERT(tPercentiles[i  ][tId] <=
                                    tPercentiles[i+1][tId]);
                        if (veryVerbose) {
                            P_(i) P(tId)
                        }
                    }
                }

                BSLS_ASSERT(sam.isTotalSame());
                BSLS_ASSERT(dam.isTotalUp());
                BSLS_ASSERT(dam.isInUseSame());
            }

        }

        if (verbose) cout << "Negative Testing" << endl;
        {
            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mX(&supplied); const Obj& X = mX;

            bsl::vector<int> threadGroupSizes(2, &scratch);
            threadGroupSizes[0] = 3;
            threadGroupSizes[1] = 2;
            mX.initialize(10, threadGroupSizes);

            bsls::AssertTestHandlerGuard hG;

            // getMedian
            double median;
            ASSERT_PASS(X.getMedian(&median, 1));
            ASSERT_PASS(X.getMedian(&median, 0));
            ASSERT_FAIL(X.getMedian(NULL   , 1));
            ASSERT_FAIL(X.getMedian(&median,-1));
            ASSERT_FAIL(X.getMedian(&median, 2));

            // getPercentile
            double percentile;
            ASSERT_PASS(X.getPercentile(&percentile, 0.25, 1));
            ASSERT_PASS(X.getPercentile(&percentile, 0.25, 0));
            ASSERT_FAIL(X.getPercentile(NULL   , 0.25, 1));
            ASSERT_FAIL(X.getPercentile(&percentile, 0.25,-1));
            ASSERT_FAIL(X.getPercentile(&percentile, 0.25, 2));
            ASSERT_FAIL(X.getPercentile(&percentile,-0.1 , 1));
            ASSERT_FAIL(X.getPercentile(&percentile, 1.1 , 1));

            // getPercentiles
            bsl::vector<double> percentiles(5, &scratch);
            bsl::vector<double> notEnough(1, &scratch);
            ASSERT_PASS(X.getPercentiles(&percentiles, 1));
            ASSERT_PASS(X.getPercentiles(&percentiles, 0));
            ASSERT_FAIL(X.getPercentiles(NULL        , 1));
            ASSERT_FAIL(X.getPercentiles(&percentiles,-1));
            ASSERT_FAIL(X.getPercentiles(&percentiles, 2));
            ASSERT_FAIL(X.getPercentiles(&notEnough  , 1));

            // getThreadPercentiles
            bsl::vector<bsl::vector<double> > threadPercentiles(3, &scratch);
            bsl::vector<bsl::vector<double> > notEnoughThread(1, &scratch);
            threadPercentiles[0].resize(2);
            threadPercentiles[1].resize(2);
            threadPercentiles[2].resize(2);
            ASSERT_PASS(X.getThreadPercentiles(&threadPercentiles, 1));
            ASSERT_FAIL(X.getThreadPercentiles(&threadPercentiles, 0));
            ASSERT_FAIL(X.getThreadPercentiles(NULL              , 1));
            ASSERT_FAIL(X.getThreadPercentiles(&threadPercentiles,-1));
            ASSERT_FAIL(X.getThreadPercentiles(&threadPercentiles, 2));
            ASSERT_FAIL(X.getThreadPercentiles(&notEnoughThread  , 1));
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // MOVE-ASSIGNMENT OPERATOR
        //   Ensure that we can move the value of any object of the class to
        //   any object of the class, such that the target object subsequently
        //   has the source value, and there are no additional allocations if
        //   only one allocator is being used, and the source object is
        //   unchanged if allocators are different.
        //
        // Concerns:
        //: 1 The move assignment operator can change the value of any
        //:   modifiable target object to that of any source object.
        //:
        //: 2 The allocator address held by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 If the allocators are different, the value of the source object
        //:   is not modified.
        //:
        //: 7 If the allocators are the same, no new allocations happen when
        //:   the move assignment happens.
        //:
        //: 8 The allocator address held by the source object is unchanged.
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:11 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..3, 5-6,8-11)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' objects 's1'.
        //:
        //:     2 Use the value constructor and 's1' to create a modifiable
        //:       'Obj', 'mF', having the value 'V'.
        //:
        //:     3 Use the value constructor and 's1' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     3 Move-assign 'mX' from 'bslmf::MovableRefUtil::move(mF)'.
        //:
        //:     4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     5 Use the equality-comparison operator to verify that the
        //:       target object, 'mX', now has the same value as that of 'Z'.
        //:
        //:     6 Use the 'allocator' accessor of both 'mX' and 'mF' to verify
        //:       that the respective allocator addresses held by the target
        //:       and source objects are unchanged.  (C-2, 7)
        //:
        //:     7 Use the appropriate test allocators to verify that no new
        //:       allocations were made by the move assignment operation.
        //:
        //:   4 For each of the iterations (P-4.2):  (C-1..2, 5, 7-9, 11)
        //:
        //:     1 Create two 'bslma::TestAllocator' objects 's1' and 's2'.
        //:
        //:     2 Use the value constructor and 's1' to create a modifiable
        //:       'Obj', 'mF', having the value 'V'.
        //:
        //:     3 Use the value constructor and 's2' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     3 Move-assign 'mX' from 'bslmf::MovableRefUtil::move(mF)'.
        //:
        //:     4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     5 Use the equality-comparison operator to verify that the
        //:       target object, 'mX', now has the same value as that of 'Z'.
        //:
        //:     6 Use the equality-comparison operator to verify that the
        //:       source object, 'mF', now has the same value as that of 'Z'.
        //:
        //:     6 Use the 'allocator' accessor of both 'mX' and 'mF' to verify
        //:       that the respective allocator addresses held by the target
        //:       and source objects are unchanged.  (C-2, 7)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-10)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a  reference to 'mX'.
        //:
        //:   4 Assign 'mX' from 'bslmf::MovableRefUtil::move(Z)'.
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-10)
        //:
        //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.
        //:
        //:   8 Use the appropriate test allocators to verify that:
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   ThroughputBenchmarkResult& operator=(MRef<TBenchmarkResult> rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MOVE-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(bslmf::MovableRef<Obj>);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int              NUM_DATA        = k_DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE1    = DATA[ti].d_line;
            const int  NS1      = DATA[ti].d_numSamples;
            const int  NTG1     = DATA[ti].d_numTGroups;
            const int *TGSIZES1 = DATA[ti].d_tgSizes;

            if (veryVerbose) {
                P_(LINE1) P_(NS1) P(NTG1)
                for (int i = 0; i < NTG1; ++i) {
                    P_(i) P(TGSIZES1[i])
                }
            }

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            bsl::vector<int> threadGroupSizes1(NTG1, &scratch);
            for (int i = 0; i < NTG1; ++i) {
                threadGroupSizes1[i] = TGSIZES1[i];
            }

            const Obj Z (NS1, threadGroupSizes1, &scratch);
            const Obj ZZ(NS1, threadGroupSizes1, &scratch);

            // move assignment with the same allocator

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int  LINE2    = DATA[tj].d_line;
                const int  NS2      = DATA[tj].d_numSamples;
                const int  NTG2     = DATA[tj].d_numTGroups;
                const int *TGSIZES2 = DATA[tj].d_tgSizes;

                if (veryVerbose) {
                    P_(LINE2) P_(NS2) P(NTG2)
                    for (int i = 0; i < NTG2; ++i) {
                        P_(i) P(TGSIZES2[i])
                    }
                }

                bslma::TestAllocator s1("scratch1", veryVeryVeryVerbose);
                bslma::TestAllocator s3("scratch3", veryVeryVeryVerbose);

                bsl::vector<int> threadGroupSizes2(NTG2, &s3);
                for (int i = 0; i < NTG2; ++i) {
                    threadGroupSizes2[i] = TGSIZES2[i];
                }

                {
                    // Test move assignment with same allocator.

                    Obj mF(NS1, threadGroupSizes1, &s1); const Obj& F=mF;
                    Obj mX(NS2, threadGroupSizes2, &s1); const Obj& X=mX;

                    LOOP6_ASSERT(LINE1,
                                 LINE2,
                                 F.numSamples(),
                                 X.numSamples(),
                                 F.numThreadGroups(),
                                 X.numThreadGroups(),
                                 (LINE1 == LINE2) ==
                                 (F.numSamples()      == X.numSamples() &&
                                  F.numThreadGroups() == X.numThreadGroups()));

                    bslma::TestAllocatorMonitor s1m(&s1);

                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mF));

                    LOOP4_ASSERT(LINE1,
                                 LINE2,
                                 Z.numSamples(),
                                 X.numSamples(),
                                 Z.numSamples() == X.numSamples());
                    LOOP4_ASSERT(LINE1,
                                 LINE2,
                                 Z.numThreadGroups(),
                                 X.numThreadGroups(),
                                 Z.numThreadGroups() == X.numThreadGroups());

                    for (int i = 0; i < Z.numThreadGroups(); ++i) {
                        LOOP5_ASSERT(LINE1,
                                     LINE2,
                                     i,
                                     Z.numThreads(i),
                                     X.numThreads(i),
                                     Z.numThreads(i) == X.numThreads(i));
                        if (veryVerbose) {
                            P_(i) P(Z.numThreads(i))
                        }
                    }

                    LOOP4_ASSERT(LINE1,
                                 LINE2,
                                 mR,
                                 &mX,
                                 mR == &mX);

                    LOOP2_ASSERT(LINE1,
                                 LINE2,
                                 s1m.isTotalSame());

                    LOOP4_ASSERT(LINE1,
                                 LINE2,
                                 &s1,
                                 X.allocator(),
                                 &s1 == X.allocator());
                    LOOP4_ASSERT(LINE1,
                                 LINE2,
                                 &s1,
                                 F.allocator(),
                                 &s1 == F.allocator());
                }

                // Verify all memory is released on object destruction.

                LOOP3_ASSERT(LINE1,
                             LINE2,
                             s1.numBlocksInUse(),
                             0 == s1.numBlocksInUse());
            }

            // move assignment with different allocators

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int  LINE2    = DATA[tj].d_line;
                const int  NS2      = DATA[tj].d_numSamples;
                const int  NTG2     = DATA[tj].d_numTGroups;
                const int *TGSIZES2 = DATA[tj].d_tgSizes;

                if (veryVerbose) {
                    P_(LINE2) P_(NS2) P(NTG2)
                    for (int i = 0; i < NTG2; ++i) {
                        P_(i) P(TGSIZES2[i])
                    }
                }

                bslma::TestAllocator s1("scratch1", veryVeryVeryVerbose);
                bslma::TestAllocator s2("scratch2", veryVeryVeryVerbose);
                bslma::TestAllocator s3("scratch3", veryVeryVeryVerbose);

                bsl::vector<int> threadGroupSizes2(NTG2, &s3);
                for (int i = 0; i < NTG2; ++i) {
                    threadGroupSizes2[i] = TGSIZES2[i];
                }

                {
                    // Test move assignment with different allocator

                    Obj mF(NS1, threadGroupSizes1, &s1); const Obj& F=mF;
                    Obj mX(NS2, threadGroupSizes2, &s2); const Obj& X=mX;

                    LOOP6_ASSERT(LINE1,
                                 LINE2,
                                 F.numSamples(),
                                 X.numSamples(),
                                 F.numThreadGroups(),
                                 X.numThreadGroups(),
                                 (LINE1 == LINE2) ==
                                 (F.numSamples()      == X.numSamples() &&
                                  F.numThreadGroups() == X.numThreadGroups()));

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(s2) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = bslmf::MovableRefUtil::move(mF));

                        LOOP4_ASSERT(LINE1,
                                     LINE2,
                                     Z.numSamples(),
                                     X.numSamples(),
                                     Z.numSamples() == X.numSamples());
                        LOOP4_ASSERT(
                                   LINE1,
                                   LINE2,
                                   Z.numThreadGroups(),
                                   X.numThreadGroups(),
                                   Z.numThreadGroups() == X.numThreadGroups());

                        for (int i = 0; i < Z.numThreadGroups(); ++i) {
                            LOOP5_ASSERT(LINE1,
                                         LINE2,
                                         i,
                                         Z.numThreads(i),
                                         X.numThreads(i),
                                         Z.numThreads(i) == X.numThreads(i));
                            if (veryVerbose) {
                                P_(i) P(Z.numThreads(i))
                            }
                        }

                        LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);

                        LOOP3_ASSERT(LINE1,
                                     F.numSamples(),
                                     Z.numSamples(),
                                     F.numSamples() == Z.numSamples());
                        LOOP3_ASSERT(
                                   LINE1,
                                   F.numThreadGroups(),
                                   Z.numThreadGroups(),
                                   F.numThreadGroups() == Z.numThreadGroups());

                        for (int i = 0; i < Z.numThreadGroups(); ++i) {
                            LOOP4_ASSERT(LINE1,
                                         i,
                                         F.numThreads(i),
                                         Z.numThreads(i),
                                         F.numThreads(i) == Z.numThreads(i));
                            if (veryVerbose) {
                                P_(i) P(F.numThreads(i))
                            }
                        }

                        LOOP4_ASSERT(LINE1, LINE2, &s2, X.allocator(),
                                     &s2 == X.allocator());
                        LOOP4_ASSERT(LINE1, LINE2, &s1, F.allocator(),
                                     &s1 == F.allocator());

                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                // Verify all memory is released on object destruction.

                LOOP3_ASSERT(LINE1, LINE2, s1.numBlocksInUse(),
                             0 == s1.numBlocksInUse());
                LOOP3_ASSERT(LINE1, LINE2, s2.numBlocksInUse(),
                             0 == s2.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj       mX(NS1, threadGroupSizes1, &oa);
                const Obj ZZ(NS1, threadGroupSizes1, &scratch);

                Obj& Z = mX;

                LOOP3_ASSERT(LINE1, NS1, Z.numSamples(),
                                                        NS1 == Z.numSamples());
                LOOP3_ASSERT(LINE1, NTG1, Z.numThreadGroups(),
                                                  NTG1 == Z.numThreadGroups());

                for (int i = 0; i < NTG1; ++i) {
                    LOOP4_ASSERT(LINE1, i, TGSIZES1[i], Z.numThreads(i),
                            TGSIZES1[i] == Z.numThreads(i));
                    if (veryVerbose) {
                        P_(i) P(TGSIZES1[i])
                    }
                }

                bslma::TestAllocatorMonitor oam(&oa);

                Obj *mR = &(mX = bslmf::MovableRefUtil::move(Z));

                LOOP3_ASSERT(LINE1,
                             NS1,
                             Z.numSamples(),
                             NS1 == Z.numSamples());
                LOOP3_ASSERT(LINE1,
                             NTG1,
                             Z.numThreadGroups(),
                             NTG1 == Z.numThreadGroups());

                for (int i = 0; i < NTG1; ++i) {
                    LOOP4_ASSERT(LINE1, i, TGSIZES1[i], Z.numThreads(i),
                            TGSIZES1[i] == Z.numThreads(i));
                    if (veryVerbose) {
                        P_(i) P(TGSIZES1[i])
                    }
                }

                LOOP3_ASSERT(LINE1, mR, &mX, mR == &mX);

                LOOP3_ASSERT(LINE1, &oa, Z.allocator(), &oa == Z.allocator());

                LOOP_ASSERT(LINE1, oam.isTotalSame());

                LOOP_ASSERT(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            LOOP2_ASSERT(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The allocator address held by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The value of the source object is not modified.
        //:
        //: 7 The allocator address held by the source object is unchanged.
        //:
        //: 8 QoI: Assigning a source object having the default-constructed
        //:   value allocates no memory.
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:11 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..2, 5..8, 11)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     3 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     5 Use the equality-comparison operator to verify that: (C-1, 6)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
        //:
        //:     6 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
        //:       that the respective allocator addresses held by the target
        //:       and source objects are unchanged.  (C-2, 7)
        //:
        //:     7 Use the appropriate test allocators to verify that:
        //:       (C-8, 11)
        //:
        //:       1 For an object that (a) is initialized with a value that did
        //:         NOT require memory allocation, and (b) is then assigned a
        //:         value that DID require memory allocation, the target object
        //:         DOES allocate memory from its object allocator only
        //:         (irrespective of the specific number of allocations or the
        //:         total amount of memory allocated); also cross check with
        //:         what is expected for 'mX' and 'Z'.
        //:
        //:       2 An object that is assigned a value that did NOT require
        //:         memory allocation, does NOT allocate memory from its object
        //:         allocator; also cross check with what is expected for 'Z'.
        //:
        //:       3 No additional memory is allocated by the source object.
        //:         (C-8)
        //:
        //:       4 All object memory is released when the object is destroyed.
        //:         (C-11)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-9..10)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a 'const' reference to 'mX'.
        //:
        //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-9)
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-10)
        //:
        //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.
        //:
        //:   8 Use the appropriate test allocators to verify that:
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   ThroughputBenchmarkResult& operator=(const TBenchmarkResult& rhs);
        //   CONCERN: All memory allocation is from the object's allocator.
        //   CONCERN: All memory allocation is exception neutral.
        //   CONCERN: Object value is independent of the object allocator.
        //   CONCERN: There is no temporary allocation from any allocator.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int              NUM_DATA        = k_DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE1    = DATA[ti].d_line;
            const int  NS1      = DATA[ti].d_numSamples;
            const int  NTG1     = DATA[ti].d_numTGroups;
            const int *TGSIZES1 = DATA[ti].d_tgSizes;

            if (veryVerbose) {
                P_(LINE1) P_(NS1) P(NTG1)
                for (int i = 0; i < NTG1; ++i) {
                    P_(i) P(TGSIZES1[i])
                }
            }

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            bsl::vector<int> threadGroupSizes1(NTG1, &scratch);
            for (int i = 0; i < NTG1; ++i) {
                threadGroupSizes1[i] = TGSIZES1[i];
            }

            const Obj Z (NS1, threadGroupSizes1, &scratch);
            const Obj ZZ(NS1, threadGroupSizes1, &scratch);

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int  LINE2    = DATA[tj].d_line;
                const int  NS2      = DATA[tj].d_numSamples;
                const int  NTG2     = DATA[tj].d_numTGroups;
                const int *TGSIZES2 = DATA[tj].d_tgSizes;

                if (veryVerbose) {
                    P_(LINE2) P_(NS2) P(NTG2)
                    for (int i = 0; i < NTG2; ++i) {
                        P_(i) P(TGSIZES2[i])
                    }
                }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                bsl::vector<int> threadGroupSizes2(NTG2, &scratch);
                for (int i = 0; i < NTG2; ++i) {
                    threadGroupSizes2[i] = TGSIZES2[i];
                }

                {
                    Obj mX(NS2, threadGroupSizes2, &oa);  const Obj& X = mX;

                    if (veryVerbose) { T_ P_(LINE2) P_(NS2) P(NTG2) }

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        LOOP4_ASSERT(LINE1,
                                     LINE2,
                                     NS1,
                                     NS2,
                                     Z.numSamples() == X.numSamples());
                        LOOP4_ASSERT(
                                   LINE1,
                                   LINE2,
                                   NTG1,
                                   NTG2,
                                   Z.numThreadGroups() == X.numThreadGroups());
                        LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);

                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    LOOP4_ASSERT(LINE1,
                                 LINE2,
                                 NS1,
                                 Z.numSamples(),
                                 NS1 == Z.numSamples());
                    LOOP4_ASSERT(LINE1,
                                 LINE2,
                                 NTG1,
                                 Z.numThreadGroups(),
                                 NTG1 == Z.numThreadGroups());

                    LOOP4_ASSERT(LINE1,
                                 LINE2,
                                 &oa,
                                 X.allocator(),
                                 &oa == X.allocator());
                    LOOP4_ASSERT(LINE1,
                                 LINE2,
                                 &scratch,
                                 Z.allocator(),
                                 &scratch == Z.allocator());

                    LOOP2_ASSERT(LINE1, LINE2, sam.isInUseSame());

                    LOOP2_ASSERT(LINE1, LINE2, 0 == da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                LOOP3_ASSERT(LINE1,
                             LINE2,
                             oa.numBlocksInUse(),
                             0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj       mX(NS1, threadGroupSizes1, &oa);
                const Obj ZZ(NS1, threadGroupSizes1, &scratch);

                const Obj& Z = mX;

                LOOP3_ASSERT(LINE1,
                             NS1,
                             Z.numSamples(),
                             NS1 == Z.numSamples());
                LOOP3_ASSERT(LINE1,
                             NTG1,
                             Z.numThreadGroups(),
                             NTG1 == Z.numThreadGroups());

                bslma::TestAllocatorMonitor oam(&oa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    LOOP3_ASSERT(LINE1,
                                 NS1,
                                 Z.numSamples(),
                                 NS1 == Z.numSamples());
                    LOOP3_ASSERT(LINE1,
                                 NTG1,
                                 Z.numThreadGroups(),
                                 NTG1 == Z.numThreadGroups());
                    LOOP3_ASSERT(LINE1, mR, &mX, mR == &mX);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP3_ASSERT(LINE1, &oa, Z.allocator(), &oa == Z.allocator());

                LOOP_ASSERT(LINE1, !oam.isInUseUp());

                LOOP_ASSERT(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            LOOP2_ASSERT(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the new object has the original value.
        //   Verify that if the same allocator is used there have been no new
        //   allocations, and if a different allocator is used the source
        //   object has the original value.
        //
        // Concerns:
        //: 1 The move constructor (with or without a supplied allocator)
        //:   creates an object having the same value as the original object
        //:   started with.
        //:
        //: 2 If an allocator is NOT supplied, the allocator of the new object
        //:   is the same as the original object, and no new allocations occur.
        //:
        //: 3 If an allocator is supplied that is the same as the original
        //:   object, then no new allocations occur.
        //:
        //: 4 If an allocator is supplied that is different from the original
        //:   object, then the original object's value remains unchanged.
        //:
        //: 5 Supplying a null allocator explicitly is the same as supplying
        //:   the default allocator.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The allocator address held by the original object is unchanged.
        //:
        //:10 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..9)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop creating four distinct objects in turn,
        //:     each using the move constructor on a newly creating object with
        //:     value V, but moving differently: (a) using the standard single
        //:     argument move constructor, (b) using the move constructor with
        //:     a 0 allocator argument (to use the default allocator), (c) the
        //:     same constructor with the same allocator as the moved-from
        //:     object, and (d) the same constructor with a different allocator
        //:     than the moved-from object.
        //:
        //: 3 For each of these iterations (P-2.2):
        //:
        //:   1 Create four 'bslma::TestAllocator' objects, and install one as
        //:     the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Dynamically allocate another object 'F" using the 's1'
        //:     allocator having the same value V, using a distinct allocator
        //:     for the object's footprint.
        //:
        //:   3 Dynamically allocate an object 'X' using the appropriate move
        //:     constructor to move from 'F', passing as a second argument
        //:     (a) nothing, (b) 0, (c) '&s1', or (d) '&s2'.
        //:
        //:   4 Record the allocator expected to be used by the new object and
        //:     how much memory it used before the move constructor.
        //:
        //:   5 Verify that space for 2 objects is used in the footprint
        //:     allocator
        //:
        //:   6 Verify that the moved-to object has the expected value 'V' by
        //:     comparing to 'Z'.
        //:
        //:   7 If the allocators of 'F' and 'X' are different, verify that the
        //:     value of 'F' is still 'V', and that the amount of memory
        //:     used in the allocator for 'X' is the same as the amount of
        //:     that was used by 'F'.
        //:
        //:   8 If the allocators of 'F' and 'X' are the same, verify that no
        //:     extra memory was used by the move constructor.
        //:
        //:   9 Verify that no memory was used by the move constructor as
        //:     temporary memory, and no unused allocators have had any memory
        //:     used.
        //:
        //:  10 Delete both dynamically allocated objects and verify that all
        //:     temporary allocators have had all memory returned to them.
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  Do
        //:   this by creating one object with one test allocator ('s1') and
        //:   then using the move constructor with a separate test allocator
        //:   that is injecting exceptions ('s2').
        //:   (C-10)
        //
        // Testing:
        //   ThroughputBenchmarkResult(MRef<TBenchmarkResult> orig);
        //   ThroughputBenchmarkResult(MRef<TBenchmarkResult> orig, ba);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MOVE CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int              NUM_DATA        = k_DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE    = DATA[ti].d_line;
                const int  NS      = DATA[ti].d_numSamples;
                const int  NTG     = DATA[ti].d_numTGroups;
                const int *TGSIZES = DATA[ti].d_tgSizes;

                if (veryVerbose) {
                    P_(LINE) P_(NS) P(NTG)
                    for (int i = 0; i < NTG; ++i) {
                        P_(i) P(TGSIZES[i])
                    }
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                bsl::vector<int> threadGroupSizes(NTG, &scratch);
                for (int i = 0; i < NTG; ++i) {
                    threadGroupSizes[i] = TGSIZES[i];
                }

                const Obj Z (NS, threadGroupSizes, &scratch);
                const Obj ZZ(NS, threadGroupSizes, &scratch);

                for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator s1("supplied",  veryVeryVeryVerbose);
                    bslma::TestAllocator s2("supplied2", veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj *fromPtr = new (fa) Obj(NS, threadGroupSizes, &s1);

                    Obj                &mF = *fromPtr; const Obj& F = mF;
                    bsls::Types::Int64  s1Alloc = s1.numBytesInUse();

                    Obj                  *objPtr;
                    bsls::Types::Int64    objAlloc;
                    bslma::TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        // normal move constructor
                        objAllocatorPtr = &s1;
                        objAlloc        = objAllocatorPtr->numBytesInUse();
                        objPtr          = new (fa)
                                          Obj(bslmf::MovableRefUtil::move(mF));
                      } break;
                      case 'b': {
                        // allocator move constructor, default allocator
                        objAllocatorPtr = &da;
                        objAlloc        = objAllocatorPtr->numBytesInUse();
                        objPtr          = new (fa)
                                       Obj(bslmf::MovableRefUtil::move(mF), 0);
                      } break;
                      case 'c': {
                        // allocator move constructor, same allocator
                        objAllocatorPtr = &s1;
                        objAlloc        = objAllocatorPtr->numBytesInUse();
                        objPtr          = new (fa)
                                     Obj(bslmf::MovableRefUtil::move(mF), &s1);
                      } break;
                      case 'd': {
                        // allocator move constructor, different allocator
                        objAllocatorPtr = &s2;
                        objAlloc        = objAllocatorPtr->numBytesInUse();
                        objPtr          = new (fa)
                                     Obj(bslmf::MovableRefUtil::move(mF), &s2);
                      } break;
                      default: {
                        LOOP_ASSERT(CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    LOOP2_ASSERT(LINE,
                                 CONFIG,
                                 2*sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P(CONFIG) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;

                    // Verify the value of the object.

                    LOOP4_ASSERT(LINE,
                                 CONFIG,
                                 NS,
                                 X.numSamples(),
                                 NS == X.numSamples());

                    LOOP4_ASSERT(LINE,
                                 CONFIG,
                                 NTG,
                                 X.numThreadGroups(),
                                 NTG == X.numThreadGroups());

                    for (int i = 0; i < NTG; ++i) {
                        LOOP5_ASSERT(LINE,
                                     CONFIG,
                                     i,
                                     TGSIZES[i],
                                     X.numThreads(i),
                                     TGSIZES[i] == X.numThreads(i));
                        if (veryVerbose) {
                            P_(i) P(TGSIZES[i])
                        }
                    }

                    if (objAllocatorPtr != F.allocator()) {
                        // If the allocators are different, verify that the
                        // value of 'fX' has not changed.

                        LOOP4_ASSERT(LINE,
                                     CONFIG,
                                     NS,
                                     F.numSamples(),
                                     NS == F.numSamples());

                        LOOP4_ASSERT(LINE,
                                     CONFIG,
                                     NTG,
                                     F.numThreadGroups(),
                                     NTG == F.numThreadGroups());

                        for (int i = 0; i < NTG; ++i) {
                            LOOP5_ASSERT(LINE,
                                         CONFIG,
                                         i,
                                         TGSIZES[i],
                                         F.numThreads(i),
                                         TGSIZES[i] == F.numThreads(i));
                            if (veryVerbose) {
                                P_(i) P(F.numThreads(i))
                            }
                        }

                        // If memory was used, verify that the same amount was
                        // used by the moved-to object.  This does not hold, as
                        // vector resize uses a power of 2 ceiling to the size,
                        // while copy does not.

                        bsls::Types::Int64 moveBytesUsed =
                            objAllocatorPtr->numBytesInUse() - objAlloc;
                        LOOP4_ASSERT(LINE,
                                     CONFIG,
                                     s1.numBytesInUse(),
                                     moveBytesUsed,
                                     s1.numBytesInUse() >= moveBytesUsed);
                    }
                    else {
                        // If the allocators are the same, verify that no new
                        // bytes were allocated by moving.

                        LOOP4_ASSERT(LINE,
                                     CONFIG,
                                     s1Alloc,
                                     s1.numBytesInUse(),
                                     s1Alloc == s1.numBytesInUse());
                    }

                    // Also invoke the object's 'allocator' accessor, as well
                    // as that of 'Z'.

                    LOOP4_ASSERT(LINE,
                                 CONFIG,
                                 &oa,
                                 X.allocator(),
                                 &oa == X.allocator());

                    LOOP4_ASSERT(LINE,
                                 CONFIG,
                                 &scratch,
                                 Z.allocator(),
                                 &scratch == Z.allocator());

                    // Verify no allocation from the non-object allocators.
                    if (objAllocatorPtr != &da) {
                        LOOP3_ASSERT(LINE,
                                     CONFIG,
                                     da.numBlocksTotal(),
                                     0 == da.numBlocksTotal());
                    }

                    if (objAllocatorPtr != &s2) {
                        LOOP3_ASSERT(LINE,
                                     CONFIG,
                                     s2.numBlocksTotal(),
                                     0 == s2.numBlocksTotal());
                    }

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    LOOP4_ASSERT(LINE,
                                 CONFIG,
                                 oa.numBlocksTotal(),
                                 oa.numBlocksInUse(),
                                 oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Reclaim dynamically allocated objects under test.

                    fa.deleteObject(fromPtr);
                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    LOOP3_ASSERT(LINE,
                                 CONFIG,
                                 da.numBlocksInUse(),
                                 0 == da.numBlocksInUse());
                    LOOP3_ASSERT(LINE,
                                 CONFIG,
                                 fa.numBlocksInUse(),
                                 0 == fa.numBlocksInUse());
                    LOOP3_ASSERT(LINE,
                                 CONFIG,
                                 s1.numBlocksInUse(),
                                 0 == s1.numBlocksInUse());
                    LOOP3_ASSERT(LINE,
                                 CONFIG,
                                 s2.numBlocksInUse(),
                                 0 == s2.numBlocksInUse());
                }  // end foreach configuration

            }  // end foreach row

        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE    = DATA[ti].d_line;
                const int  NS      = DATA[ti].d_numSamples;
                const int  NTG     = DATA[ti].d_numTGroups;
                const int *TGSIZES = DATA[ti].d_tgSizes;

                if (veryVerbose) {
                    P_(LINE) P_(NS) P(NTG)
                    for (int i = 0; i < NTG; ++i) {
                        P_(i) P(TGSIZES[i])
                    }
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator s1("supplied1", veryVeryVeryVerbose);
                bslma::TestAllocator s2("supplied2", veryVeryVeryVerbose);

                bsl::vector<int> threadGroupSizes(NTG, &scratch);
                for (int i = 0; i < NTG; ++i) {
                    threadGroupSizes[i] = TGSIZES[i];
                }

                const Obj Z (NS, threadGroupSizes, &scratch);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(s2) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj from(NS, threadGroupSizes, &s1);

                    Obj obj(bslmf::MovableRefUtil::move(from), &s2);
                    LOOP3_ASSERT(LINE,
                                 NS,
                                 obj.numSamples(),
                                 NS == obj.numSamples());
                    LOOP3_ASSERT(LINE,
                                 NTG,
                                 obj.numThreadGroups(),
                                 NTG == obj.numThreadGroups());

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP3_ASSERT(LINE,
                             &scratch,
                             Z.allocator(),
                             &scratch == Z.allocator());
                LOOP2_ASSERT(LINE,
                             da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                LOOP2_ASSERT(LINE,
                             s1.numBlocksInUse(),
                             0 == s1.numBlocksInUse());
                LOOP2_ASSERT(LINE,
                             s2.numBlocksInUse(),
                             0 == s2.numBlocksInUse());
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor (with or without a supplied allocator)
        //:   creates an object having the same value as that of the supplied
        //:   original object.
        //:
        //: 2 If an allocator is NOT supplied to the copy constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object (i.e., the
        //:   allocator of the original object is never copied).
        //:
        //: 3 If an allocator IS supplied to the copy constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the copy constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The original object is passed as a 'const' reference.
        //:
        //:10 The value of the original object is unchanged.
        //:
        //:11 The allocator address held by the original object is unchanged.
        //:
        //:12 QoI: Copying an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:13 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..12)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop creating three distinct objects in turn,
        //:     each using the copy constructor on 'Z' from P-2.1, but
        //:     configured differently: (a) without passing an allocator,
        //:     (b) passing a null allocator address explicitly, and (c)
        //:     passing the address of a test allocator distinct from the
        //:     default.
        //:
        //:   3 For each of these three iterations (P-2.2):  (C-1..12)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object 'X',
        //:       with its object allocator configured appropriately (see
        //:       P-2.2), supplying it the 'const' object 'Z' (see P-2.1); use
        //:       a distinct test allocator for the object's footprint.  (C-9)
        //:
        //:     3 Use the equality-comparison operator to verify that:
        //:       (C-1, 5, 10)
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         that of 'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-10)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also use the 'allocator'
        //:       accessor of 'X' to verify that its object allocator is
        //:       properly installed, and use the 'allocator' accessor of 'Z'
        //:       to verify that the allocator address that it holds is
        //:       unchanged.  (C-6, 11)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-2..4,
        //:       7..8, 12)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-2, 4)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-12)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-8)
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   ThroughputBenchmarkResult(original, ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int              NUM_DATA        = k_DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE    = DATA[ti].d_line;
                const int  NS      = DATA[ti].d_numSamples;
                const int  NTG     = DATA[ti].d_numTGroups;
                const int *TGSIZES = DATA[ti].d_tgSizes;

                if (veryVerbose) {
                    P_(LINE) P_(NS) P(NTG)
                    for (int i = 0; i < NTG; ++i) {
                        P_(i) P(TGSIZES[i])
                    }
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                bsl::vector<int> threadGroupSizes(NTG, &scratch);
                for (int i = 0; i < NTG; ++i) {
                    threadGroupSizes[i] = TGSIZES[i];
                }

                const Obj Z (NS, threadGroupSizes, &scratch);
                const Obj ZZ(NS, threadGroupSizes, &scratch);

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr;
                    bslma::TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        objPtr          = new (fa) Obj(Z);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr          = new (fa) Obj(Z, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr          = new (fa) Obj(Z, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        LOOP_ASSERT(CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    LOOP2_ASSERT(LINE,
                                 CONFIG,
                                 sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P(CONFIG) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // Verify the value of the object.

                    LOOP4_ASSERT(LINE,
                                 CONFIG,
                                 NS,
                                 X.numSamples(),
                                 NS == X.numSamples());

                    LOOP4_ASSERT(LINE,
                                 CONFIG,
                                 NTG,
                                 X.numThreadGroups(),
                                 NTG == X.numThreadGroups());

                    for (int i = 0; i < NTG; ++i) {
                        LOOP5_ASSERT(LINE,
                                     CONFIG,
                                     i,
                                     TGSIZES[i],
                                     X.numThreads(i),
                                     TGSIZES[i] == X.numThreads(i));
                        if (veryVerbose) {
                            P_(i) P(TGSIZES[i])
                        }
                    }

                    // Also invoke the object's 'allocator' accessor, as well
                    // as that of 'Z'.

                    LOOP4_ASSERT(LINE,
                                 CONFIG,
                                 &oa,
                                 X.allocator(),
                                 &oa == X.allocator());

                    LOOP4_ASSERT(LINE,
                                 CONFIG,
                                 &scratch,
                                 Z.allocator(),
                                 &scratch == Z.allocator());

                    // Verify no allocation from the non-object allocator.

                    LOOP3_ASSERT(LINE,
                                 CONFIG,
                                 noa.numBlocksTotal(),
                                 0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    LOOP4_ASSERT(LINE,
                                 CONFIG,
                                 oa.numBlocksTotal(),
                                 oa.numBlocksInUse(),
                                 oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    LOOP3_ASSERT(LINE,
                                 CONFIG,
                                 da.numBlocksInUse(),
                                 0 == da.numBlocksInUse());
                    LOOP3_ASSERT(LINE,
                                 CONFIG,
                                 fa.numBlocksInUse(),
                                 0 == fa.numBlocksInUse());
                    LOOP3_ASSERT(LINE,
                                 CONFIG,
                                 sa.numBlocksInUse(),
                                 0 == sa.numBlocksInUse());
                }  // end foreach configuration

            }  // end foreach row

        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE    = DATA[ti].d_line;
                const int  NS      = DATA[ti].d_numSamples;
                const int  NTG     = DATA[ti].d_numTGroups;
                const int *TGSIZES = DATA[ti].d_tgSizes;

                if (veryVerbose) {
                    P_(LINE) P_(NS) P(NTG)
                    for (int i = 0; i < NTG; ++i) {
                        P_(i) P(TGSIZES[i])
                    }
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                bsl::vector<int> threadGroupSizes(NTG, &scratch);
                for (int i = 0; i < NTG; ++i) {
                    threadGroupSizes[i] = TGSIZES[i];
                }

                const Obj Z(NS, threadGroupSizes, &scratch);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    Obj obj(Z, &sa);
                    LOOP3_ASSERT(LINE,
                                 NS,
                                 obj.numSamples(),
                                 NS == obj.numSamples());
                    LOOP3_ASSERT(LINE,
                                 NTG,
                                 obj.numThreadGroups(),
                                 NTG == obj.numThreadGroups());

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP3_ASSERT(LINE, NS, Z.numSamples(), NS == Z.numSamples());
                LOOP3_ASSERT(LINE,
                             NTG,
                             Z.numThreadGroups(),
                             NTG == Z.numThreadGroups());

                LOOP3_ASSERT(LINE,
                             &scratch,
                             Z.allocator(),
                             &scratch == Z.allocator());
                LOOP2_ASSERT(LINE,
                             da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                LOOP2_ASSERT(LINE,
                             sa.numBlocksInUse(),
                             0 == sa.numBlocksInUse());
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // VALUE CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor (with or without a supplied allocator) can
        //:   create an object having any value that does not violate the
        //:   constructor's documented preconditions.
        //:
        //: 3 Any argument can be 'const'.
        //:
        //: 4 If an allocator is NOT supplied to the value constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 5 If an allocator IS supplied to the value constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 6 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 7 Supplying an allocator to the value constructor has no effect
        //:   on subsequent object values.
        //:
        //: 8 Any memory allocation is from the object allocator.
        //:
        //: 9 There is no temporary memory allocation from any allocator.
        //:
        //:10 Every object releases any allocated memory at destruction.
        //:
        //:11 QoI: Creating an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:12 Any memory allocation is exception neutral.
        //:
        //:13 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1, 3..11)
        //:
        //:   1 Execute an inner loop creating three distinct objects, in turn,
        //:     each object having the same value, 'V', but configured
        //:     differently: (a) without passing an allocator, (b) passing a
        //:     null allocator address explicitly, and (c) passing the address
        //:     of a test allocator distinct from the default allocator.
        //:
        //:   2 For each of the three iterations in P-2.1:  (C-1, 4..11)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the value constructor to dynamically create an object
        //:       having the value 'V', with its object allocator configured
        //:       appropriately (see P-2.1), supplying all the arguments as
        //:       'const' and representing any string arguments as 'char *';
        //:       use a distinct test allocator for the object's footprint.
        //:
        //:     3 Use the (as yet unproven) salient attribute accessors to
        //:       verify that all of the attributes of each object have their
        //:       expected values.  (C-1, 7)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also invoke the (as yet
        //:       unproven) 'allocator' accessor of the object under test.
        //:       (C-8)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-4..6,
        //:       9..11)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-4, 6)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-11)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         default allocator doesn't allocate any memory.  (C-5)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-9)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-10)
        //:
        //: 3 Repeat the steps in P-2 for the supplied allocator configuration
        //:   (P-2.1c) on the data of P-1, but this time create the object as
        //:   an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros);
        //:   represent any string arguments in terms of 'string' using a
        //:   "scratch" allocator.  (C-2, 12)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   ThroughputBenchmarkResult(numSamples, threadGroupSizes, ba = 0);
        //   CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CTOR" << endl
                          << "==========" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int              NUM_DATA        = k_DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE    = DATA[ti].d_line;
                const int  NS      = DATA[ti].d_numSamples;
                const int  NTG     = DATA[ti].d_numTGroups;
                const int *TGSIZES = DATA[ti].d_tgSizes;

                if (veryVerbose) {
                    P_(LINE) P_(NS) P(NTG)
                    for (int i = 0; i < NTG; ++i) {
                        P_(i) P(TGSIZES[i])
                    }
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                bsl::vector<int> threadGroupSizes(NTG, &scratch);
                for (int i = 0; i < NTG; ++i) {
                    threadGroupSizes[i] = TGSIZES[i];
                }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    if (veryVerbose) { T_ T_ P(CONFIG) }

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr;
                    bslma::TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        objPtr          = new (fa) Obj(NS, threadGroupSizes);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr          = new (fa)
                                                  Obj(NS, threadGroupSizes, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr          = new (fa)
                                                Obj(NS, threadGroupSizes, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        LOOP2_ASSERT(LINE, CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    LOOP2_ASSERT(LINE,
                                 CONFIG,
                                 sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                  // -------------------------------------
                  // Verify the object's attribute values.
                  // -------------------------------------

                    LOOP4_ASSERT(LINE,
                                 CONFIG,
                                 NS,
                                 X.numSamples(),
                                 NS == X.numSamples());

                    LOOP4_ASSERT(LINE,
                                 CONFIG,
                                 NTG,
                                 X.numThreadGroups(),
                                 NTG == X.numThreadGroups());

                    for (int i = 0; i < NTG; ++i) {
                        LOOP5_ASSERT(LINE,
                                     CONFIG,
                                     i,
                                     TGSIZES[i],
                                     X.numThreads(i),
                                     TGSIZES[i] == X.numThreads(i));
                        if (veryVerbose) {
                            P_(i) P(TGSIZES[i])
                        }
                    }

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    LOOP4_ASSERT(LINE,
                                 CONFIG,
                                 &oa,
                                 X.allocator(),
                                 &oa == X.allocator());

                    // Verify no allocation from the non-object allocator.

                    LOOP3_ASSERT(LINE,
                                 CONFIG,
                                 noa.numBlocksTotal(),
                                 0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    LOOP4_ASSERT(LINE,
                                 CONFIG,
                                 oa.numBlocksTotal(),
                                 oa.numBlocksInUse(),
                                 oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    LOOP3_ASSERT(LINE,
                                 CONFIG,
                                 da.numBlocksInUse(),
                                 0 == da.numBlocksInUse());
                    LOOP3_ASSERT(LINE,
                                 CONFIG,
                                 fa.numBlocksInUse(),
                                 0 == fa.numBlocksInUse());
                    LOOP3_ASSERT(LINE,
                                 CONFIG,
                                 sa.numBlocksInUse(),
                                 0 == sa.numBlocksInUse());

                }  // end foreach configuration

            }  // end foreach row

        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            // Note that any string arguments are now of type 'string', which
            // require their own "scratch" allocator.

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE    = DATA[ti].d_line;
                const int  NS      = DATA[ti].d_numSamples;
                const int  NTG     = DATA[ti].d_numTGroups;
                const int *TGSIZES = DATA[ti].d_tgSizes;

                if (veryVerbose) {
                    P_(LINE) P_(NS) P(NTG)
                    for (int i = 0; i < NTG; ++i) {
                        P_(i) P(TGSIZES[i])
                    }
                }

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                bsl::vector<int> threadGroupSizes(NTG, &scratch);
                for (int i = 0; i < NTG; ++i) {
                    threadGroupSizes[i] = TGSIZES[i];
                }

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(NS, threadGroupSizes, &sa);
                    LOOP3_ASSERT(LINE,
                                 NS,
                                 obj.numSamples(),
                                 NS == obj.numSamples());
                    LOOP3_ASSERT(LINE,
                                 NTG,
                                 obj.numThreadGroups(),
                                 NTG == obj.numThreadGroups());

                    LOOP_ASSERT(LINE, 0 < EXCEPTION_COUNT);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP2_ASSERT(LINE,
                             da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                LOOP2_ASSERT(LINE,
                             sa.numBlocksInUse(),
                             0 == sa.numBlocksInUse());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const int        NUMSAMPLES = 10;
            bsl::vector<int> threadGroupSizes(2);
            threadGroupSizes[0] = 3;
            threadGroupSizes[1] = 2;
            const bsl::vector<int>& THREADGROUPSIZES = threadGroupSizes;

            if (verbose) cout << "\t'numSamples'" << endl;
            {
                ASSERT_FAIL(Obj(0         , THREADGROUPSIZES));
                ASSERT_PASS(Obj(NUMSAMPLES, THREADGROUPSIZES));
            }

            bsl::vector<int> emptyThreadGroupSizes;
            threadGroupSizes[0] = 0;
            if (verbose) cout << "\t'threadGroupSizes'" << endl;
            {
                ASSERT_FAIL(Obj(NUMSAMPLES, emptyThreadGroupSizes));
                ASSERT_FAIL(Obj(NUMSAMPLES, threadGroupSizes));
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute of
        //:   the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates any memory.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 To test 'allocator', create object with various allocators and
        //:   ensure the returned value matches the supplied allocator.
        //:
        //: 2 Use 'addThreadGroup' to populate an object, and verify the
        //:   return value of the accessor against expected values.
        //:   (C-1)
        //:
        //: 3 The accessors will only be accessed from a 'const' reference to
        //:   the created object.  (C-2)
        //:
        //: 4 A supplied allocator will be used for all created objects
        //:   (excluding those used to test 'allocator') and the number of
        //:   allocation will be verified to ensure that no memory was
        //:   allocated during use of the accessors.  (C-3)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid indexes, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-4)
        //
        // Testing:
        //   bslma::Allocator *allocator() const;
        //   int numSamples() const;
        //   int numThreadGroups() const;
        //   int numThreads(int threadGroupIndex) const;
        //   int totalNumThreads() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nTesting 'allocator'." << endl;
        {
            Obj mX;  const Obj& X = mX; (void)X;
            BSLS_ASSERT(&defaultAllocator == X.allocator());
        }
        {
            Obj        mX(reinterpret_cast<bslma::TestAllocator *>(0));
            const Obj& X = mX; (void)X;
            BSLS_ASSERT(&defaultAllocator == X.allocator());
        }
        {
            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

            Obj mX(&supplied);  const Obj& X = mX; (void)X;
            BSLS_ASSERT(&supplied == X.allocator());
        }

        if (verbose) cout << "\nTesting other basic accessors." << endl;
        {
            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj      mX(&supplied);  const Obj& X = mX; (void)X;
            TestUtil test(mX);

            BSLS_ASSERT(0 == X.numSamples());
            BSLS_ASSERT(0 == X.numThreadGroups());
            BSLS_ASSERT(0 == X.totalNumThreads());

            bsl::vector<int> threadGroupSizes(2, &scratch);
            threadGroupSizes[0] = 3;
            threadGroupSizes[1] = 2;
            mX.initialize(10, threadGroupSizes);

            // Check with the accessors
            BSLS_ASSERT(10 == X.numSamples());
            BSLS_ASSERT( 2 == X.numThreadGroups());
            BSLS_ASSERT( 5 == X.totalNumThreads());
            BSLS_ASSERT( 3 == X.numThreads(0));
            BSLS_ASSERT( 2 == X.numThreads(1));
        }

        if (verbose) cout << "Negative Testing" << endl;
        {
            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mX(&supplied); const Obj& X = mX;

            bsl::vector<int> threadGroupSizes(2, &scratch);
            threadGroupSizes[0] = 3;
            threadGroupSizes[1] = 2;
            mX.initialize(10, threadGroupSizes);

            bsls::AssertTestHandlerGuard hG;

            ASSERT_FAIL(X.numThreads(-1));
            ASSERT_FAIL(X.numThreads(2));
            ASSERT_PASS(X.numThreads(0));
            ASSERT_PASS(X.numThreads(1));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS AND 'getValue'
        //
        // Concerns:
        //: 1 The method 'initialize' sets the correct number of samples, the
        //:   correct number of thread groups in each sample, and the correct
        //    number of threads in each thread groups.
        //:
        //: 2 The method 'initialize' allocates only memory from the allocator
        //:   specified with the constructor, and is exception neutral with
        //:   respect to memory allocation.
        //:
        //: 3 The method 'setThroughput' sets the correct value in the correct
        //:   location, and does not change other values.
        //:
        //: 4 The method 'getValue' gets the correct value.
        //:
        //: 5 The method 'setThroughput' does not allocate memory.
        //:
        //: 6 Memory is not leaked by any method and the destructor properly
        //:   deallocates the residual allocated memory.
        //:
        //: 7 QoI: There is no temporary memory allocation from any allocator.
        //:
        //: 8 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create an object using a supplied test allocator.
        //:
        //: 2 Call method 'initialize' with a certain number of samples and
        //:   thread group sizes. Verify:
        //:   1 Number of samples is correct.
        //:   2 Number of thread groups in each sample is correct.
        //:   3 The number of threads in each thread group is correct.
        //:   4 All throughputs are initialized to 0.0.
        //:   5 No memory is allocated on the default constructor.
        //:   6 Memory is allocated on the supplied allocator.
        //:
        //: 3 Call method 'setThroughput' in a loop and verify:
        //:   1 The value is the specified location is changed.
        //:   2 No values other have changed.
        //:   3 No memory was allocated or released on the supplied or default
        //:     allocators.
        //:
        //: 4 Repeat the verification using 'getValue', thus proving it has the
        //:   correct value.
        //:
        //: 5 When the object goes out of scope, there is no memory leak on the
        //:   supplied allocator.
        //:
        //: 6 There is no memory allocated on the default or global allocators.
        //:
        //: 7 Call the 'initialize' method wrapped with
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macros.
        //:
        //: 8 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid indexes, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //
        // Testing:
        //   void initialize(numSamples, threadGroupSizes);
        //   void setThroughput(tgIndex, threadIndex, sampleIndex, value);
        //   double getValue(threadGroupIndex, threadIndex, sampleIndex) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS AND 'getValue'" << endl
                          << "===================================" << endl;

        {
            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj      mX(&supplied);  const Obj& X = mX;
            TestUtil test(mX);

            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();
            bsls::Types::Int64 sAllocations = supplied.numAllocations();
            (void)allocations;
            (void)sAllocations;

            bsl::vector<int> threadGroupSizes(2, &scratch);
            threadGroupSizes[0] = 3;
            threadGroupSizes[1] = 2;
            mX.initialize(10, threadGroupSizes);
            // Check with the accessors
            BSLS_ASSERT(10 == X.numSamples());
            BSLS_ASSERT( 2 == X.numThreadGroups());
            BSLS_ASSERT( 5 == X.totalNumThreads());
            BSLS_ASSERT( 3 == X.numThreads(0));
            BSLS_ASSERT( 2 == X.numThreads(1));
            // Check through testUtil
            BSLS_ASSERT(10 == test.throughputs().size());
            for (bsl::size_t i = 0; i < test.throughputs().size(); ++i) {
                BSLS_ASSERT(X.numThreadGroups() ==
                               static_cast<int>(test.throughputs()[i].size()));
                BSLS_ASSERT(3 == test.throughputs()[i][0].size());
                BSLS_ASSERT(2 == test.throughputs()[i][1].size());
                for (int j = 0; j < X.numThreadGroups(); ++j) {
                    for (int k = 0; k < X.numThreads(j); ++k) {
                        BSLS_ASSERT(0.0 == test.throughputs()[i][j][k]);
                        if (veryVerbose) {
                            P_(i) P_(j) P(k)
                        }
                    }
                }
            }

            // Check memory allocation on default and supplied allocators.
            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());
            BSLS_ASSERT(sAllocations + 31 == supplied.numAllocations());

            sAllocations = supplied.numAllocations();

            struct ThroughputRow {
                int    d_line;       // source line number
                int    d_sampleIdx;  // sample index
                int    d_tgIdx;      // thread group index
                int    d_threadIdx;  // thread index
                double d_throughput; // Sample index
            };

            static const ThroughputRow ThroughputData[] =
            {
              //LN  sampleIdx tgIdx tIdx val
              //--- --------- ----- ---- -----
              {  L_, 0        , 0   , 0  , 1.0},
              {  L_, 5        , 1   , 1  , 2.5},
              {  L_, 9        , 0   , 2  , 0.4}
            };

            const int ThroughputDataSize =
              static_cast<int>(sizeof ThroughputData / sizeof *ThroughputData);

            // Set throughput values, and test that they are set correctly, and
            // that everything else is a zero.  Test 'getValue' at the same
            // time.
            for (int ti = 0; ti < ThroughputDataSize; ++ti) {
                const int    LINE       = ThroughputData[ti].d_line;
                const int    SAMPLEIDX  = ThroughputData[ti].d_sampleIdx;
                const int    TGIDX      = ThroughputData[ti].d_tgIdx;
                const int    THREADIDX  = ThroughputData[ti].d_threadIdx;
                const double THROUGHPUT = ThroughputData[ti].d_throughput;

                if (veryVerbose) {
                    T_
                    P_(LINE)
                    P_(SAMPLEIDX)
                    P_(TGIDX)
                    P_(THREADIDX)
                    P(THROUGHPUT)
                }

                // Set the value
                mX.setThroughput(TGIDX, THREADIDX, SAMPLEIDX, THROUGHPUT);

                BSLS_ASSERT(THROUGHPUT ==
                              test.throughputs()[SAMPLEIDX][TGIDX][THREADIDX]);
                BSLS_ASSERT(THROUGHPUT ==
                                      X.getValue(TGIDX, THREADIDX, SAMPLEIDX));

                for (int i = 0;
                        i < static_cast<int>(test.throughputs().size()); ++i) {
                    BSLS_ASSERT(X.numThreadGroups() ==
                               static_cast<int>(test.throughputs()[i].size()));
                    BSLS_ASSERT(3 == test.throughputs()[i][0].size());
                    BSLS_ASSERT(2 == test.throughputs()[i][1].size());
                    for (int j = 0; j < X.numThreadGroups(); ++j) {
                        for (int k = 0; k < X.numThreads(j); ++k) {
                            if (i != SAMPLEIDX ||
                                j != TGIDX ||
                                k != THREADIDX) {
                               BSLS_ASSERT(0.0 == test.throughputs()[i][j][k]);
                               if (veryVerbose) {
                                   P_(i) P_(j) P(k)
                               }
                            }
                        }
                    }
                }

                // Unset the value
                mX.setThroughput(TGIDX, THREADIDX, SAMPLEIDX, 0.0);

                BSLS_ASSERT(0.0 ==
                              test.throughputs()[SAMPLEIDX][TGIDX][THREADIDX]);
                BSLS_ASSERT(0.0 == X.getValue(TGIDX, THREADIDX, SAMPLEIDX));

                // Confirm that memory did not change
                BSLS_ASSERT(allocations == defaultAllocator.numAllocations());
                BSLS_ASSERT(sAllocations == supplied.numAllocations());
            }
        }

        if (verbose) cout << "\n'initialize' exception test." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();
            (void)allocations; (void)allocations;

            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {
                Obj mX(&supplied);  const Obj& X = mX; (void)X;
                BSLS_ASSERT(0 == X.numThreadGroups());

                bsl::vector<int> threadGroupSizes(2, &scratch);
                threadGroupSizes[0] = 3;
                threadGroupSizes[1] = 2;
                mX.initialize(10, threadGroupSizes);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());
        }

        if (verbose) cout << "Negative Testing" << endl;
        {
            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mX(&supplied);  const Obj& X = mX;

            bsls::AssertTestHandlerGuard hG;

            // 'initialize'
            bsl::vector<int> threadGroupSizes(&scratch);
            ASSERT_FAIL(mX.initialize(10, threadGroupSizes)); // 0 group size
            threadGroupSizes.resize(2);
            threadGroupSizes[0] = 3;
            threadGroupSizes[1] = 0;
            ASSERT_FAIL(mX.initialize(10, threadGroupSizes)); // 0 numThreads
            threadGroupSizes[1] = 2;
            ASSERT_FAIL(mX.initialize(0, threadGroupSizes)); // 0 numSamples
            ASSERT_PASS(mX.initialize(10, threadGroupSizes));

            // 'setThroughput'
            ASSERT_FAIL(mX.setThroughput(-1, 0, 0, 1.0)); // tgId < 0
            ASSERT_FAIL(mX.setThroughput( 2, 0, 0, 1.0)); // tgId >= numTG
            ASSERT_FAIL(mX.setThroughput( 0,-1, 0, 1.0)); // tId < 0
            ASSERT_FAIL(mX.setThroughput( 0, 3, 0, 1.0)); // tId >= numT
            ASSERT_FAIL(mX.setThroughput( 0, 0,-1, 1.0)); // sId < 0
            ASSERT_FAIL(mX.setThroughput( 0, 0,10, 1.0)); // sId >= numS
            ASSERT_FAIL(mX.setThroughput( 0, 0, 0,-1.0)); // val < 0
            ASSERT_PASS(mX.setThroughput( 0, 0, 0, 1.0));

            // 'getValue'
            ASSERT_FAIL(X.getValue(-1, 0, 0)); // tgId < 0
            ASSERT_FAIL(X.getValue( 2, 0, 0)); // tgId >= numTG
            ASSERT_FAIL(X.getValue( 0,-1, 0)); // tId < 0
            ASSERT_FAIL(X.getValue( 0, 3, 0)); // tId >= numT
            ASSERT_FAIL(X.getValue( 0, 0,-1)); // sId < 0
            ASSERT_FAIL(X.getValue( 0, 0,10)); // sId >= numS
            ASSERT_PASS(X.getValue( 0, 0, 0));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CONSTRUCTOR
        //
        // Concerns:
        //: 1 The default constructor creates the correct initial value and has
        //:   the internal memory management system hooked up properly so that
        //:   *all* internally allocated memory draws from the same
        //:   user-supplied allocator whenever one is specified.
        //:
        //: 2 The method 'initialize' uses the allocator specified with the
        //:   constructor.  Note that 'initalize' is not tested here, and
        //:   is used merely to test memory allocation.
        //:
        //: 3 Memory is not leaked by any method and the (default) destructor
        //:   properly deallocates the residual allocated memory.
        //
        // Plan:
        //: 1 Create an object using the default constructor with and without
        //:   passing in an allocator, verify the allocator is stored using the
        //:   (untested) 'allocator' accessor, and verifying all allocations
        //:   are done from the allocator in future tests.
        //:
        //: 2 Create objects using the
        //:   'bslma::TestAllocator', use the 'initialize' method with
        //:   various values, and the (untested) accessors to verify the value
        //:   of the object and that allocation occurred when expected.
        //:
        //: 5 Use a supplied 'bslma::TestAllocator' that goes out-of-scope
        //:   at the conclusion of each test to ensure all memory is returned
        //:   to the allocator.  (C-3)
        //
        // Testing:
        //   ThroughputBenchmarkResult(bslma::Allocator *basicAllocator = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DEFAULT CONSTRUCTOR" << endl
                          << "===================" << endl;

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();
            (void)allocations;

            Obj      mX;  const Obj& X = mX; (void)X;
            TestUtil test(mX);

            BSLS_ASSERT(&defaultAllocator == X.allocator());
            BSLS_ASSERT(0 == X.numSamples());
            BSLS_ASSERT(0 == X.numThreadGroups());
            BSLS_ASSERT(0 == X.totalNumThreads());
            BSLS_ASSERT(0 == test.throughputs().size());
            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());

            bsl::vector<int> threadGroupSizes(2, &scratch);
            threadGroupSizes[0] = 3;
            threadGroupSizes[1] = 2;
            mX.initialize(10, threadGroupSizes);
            BSLS_ASSERT(10 == X.numSamples());
            BSLS_ASSERT( 2 == X.numThreadGroups());
            BSLS_ASSERT( 5 == X.totalNumThreads());
            BSLS_ASSERT( 3 == X.numThreads(0));
            BSLS_ASSERT( 2 == X.numThreads(1));
            BSLS_ASSERT(10 == test.throughputs().size());

            BSLS_ASSERT(allocations + 31 == defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();
            (void)allocations;

            Obj        mX(reinterpret_cast<bslma::TestAllocator *>(0));
            const Obj& X = mX; (void)X;
            TestUtil   test(mX);

            BSLS_ASSERT(0           == X.numSamples());
            BSLS_ASSERT(0           == X.numThreadGroups());
            BSLS_ASSERT(0           == X.totalNumThreads());
            BSLS_ASSERT(0           == test.throughputs().size());
            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());

            bsl::vector<int> threadGroupSizes(2, &scratch);
            threadGroupSizes[0] = 3;
            threadGroupSizes[1] = 2;
            mX.initialize(10, threadGroupSizes);
            BSLS_ASSERT(10 == X.numSamples());
            BSLS_ASSERT( 2 == X.numThreadGroups());
            BSLS_ASSERT( 5 == X.totalNumThreads());
            BSLS_ASSERT( 3 == X.numThreads(0));
            BSLS_ASSERT( 2 == X.numThreads(1));
            BSLS_ASSERT(10 == test.throughputs().size());

            BSLS_ASSERT(allocations + 31 == defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();
            (void)allocations;

            bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

            Obj      mX(&supplied);  const Obj& X = mX; (void)X;
            TestUtil test(mX);

            BSLS_ASSERT(&supplied   == X.allocator());
            BSLS_ASSERT(0           == X.numSamples());
            BSLS_ASSERT(0           == X.numThreadGroups());
            BSLS_ASSERT(0           == X.totalNumThreads());
            BSLS_ASSERT(0           == test.throughputs().size());
            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());

            bsl::vector<int> threadGroupSizes(2, &scratch);
            threadGroupSizes[0] = 3;
            threadGroupSizes[1] = 2;
            mX.initialize(10, threadGroupSizes);
            BSLS_ASSERT(10 == X.numSamples());
            BSLS_ASSERT( 2 == X.numThreadGroups());
            BSLS_ASSERT( 5 == X.totalNumThreads());
            BSLS_ASSERT( 3 == X.numThreads(0));
            BSLS_ASSERT( 2 == X.numThreads(1));
            BSLS_ASSERT(10 == test.throughputs().size());

            BSLS_ASSERT(allocations == defaultAllocator.numAllocations());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Instantiate an object and verify basic functionality.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj      mX(&supplied);  const Obj& X = mX; (void)X;

        bsl::vector<int> threadGroupSizes(2, &scratch);
        threadGroupSizes[0] = 3;
        threadGroupSizes[1] = 2;
        mX.initialize(10, threadGroupSizes);
        BSLS_ASSERT(10 == X.numSamples());
        BSLS_ASSERT( 2 == X.numThreadGroups());
        BSLS_ASSERT( 5 == X.totalNumThreads());
        BSLS_ASSERT( 3 == X.numThreads(0));
        BSLS_ASSERT( 2 == X.numThreads(1));
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

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
