// bdlm_instancecount.t.cpp                                           -*-C++-*-
#include <bdlm_instancecount.h>

using namespace BloombergLP;

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmt_barrier.h>
#include <bslmt_threadutil.h>

#include <bsls_assert.h>
#include <bsls_review.h>

#include <bsl_iostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::endl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test defines a mechanism the purpose of which is to
// provide type specific instance counts.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o At no time is memory allocated from the global allocator except examples.
//: o At no time is memory allocated from the default alloc. except examples.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 1] InstanceCount::Value nextInstanceNumber<OBJECT_TYPE>();
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

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
//                         GLOBAL OBJECTS FOR TESTING
// ----------------------------------------------------------------------------

struct WorkerData {
    bsl::vector<bdlm::InstanceCount::Value>  d_results;
    bslmt::Barrier                          *d_barrier_p;
};

// ============================================================================
//                         GLOBAL METHODS FOR TESTING
// ----------------------------------------------------------------------------

extern "C" void *worker(void *arg)
    // Populate the specified 'arg', which is a pointer to a 'WorkerData'
    // instance, with the results of invoking
    // 'bslm::InstanceCount::nextInstanceNumber<short>'.
{
    WorkerData& wd = *static_cast<WorkerData *>(arg);

    wd.d_barrier_p->wait();

    for (unsigned i = 0; i < wd.d_results.size(); ++i) {
        wd.d_results[i] = bdlm::InstanceCount::nextInstanceNumber<short>();
    }

    return 0;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    using namespace BloombergLP;

    const int test = argc > 1 ? atoi(argv[1]) : 0;

    BSLA_MAYBE_UNUSED const bool             verbose = argc > 2;
    BSLA_MAYBE_UNUSED const bool         veryVerbose = argc > 3;
    BSLA_MAYBE_UNUSED const bool     veryVeryVerbose = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);
    bool expectGlobalAllocation = false;

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);
    bool expectDefaultAllocation = false;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
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
///Example 1: Using 'bdlm::InstanceCount'
///- - - - - - - - - - - - - - - - - - -
// This example demonstrates the usage of 'bdlm::InstanceCount' to obtain type
// specific instance counts.
//
// First, we obtain and verify the instance counts for a few types:
//..
    ASSERT(1 == bdlm::InstanceCount::nextInstanceNumber<char>());
    ASSERT(1 == bdlm::InstanceCount::nextInstanceNumber<int>());
    ASSERT(1 == bdlm::InstanceCount::nextInstanceNumber<double>());
//..
// Then, we obtain and verify the instance counts for the previous types and
// some new types:
//..
    ASSERT(2 == bdlm::InstanceCount::nextInstanceNumber<char>());
    ASSERT(2 == bdlm::InstanceCount::nextInstanceNumber<int>());
    ASSERT(1 == bdlm::InstanceCount::nextInstanceNumber<unsigned>());
    ASSERT(1 == bdlm::InstanceCount::nextInstanceNumber<float>());
    ASSERT(2 == bdlm::InstanceCount::nextInstanceNumber<double>());
//..
// Finally, we obtain and verify the next instance counts for these types:
//..
    ASSERT(3 == bdlm::InstanceCount::nextInstanceNumber<char>());
    ASSERT(3 == bdlm::InstanceCount::nextInstanceNumber<int>());
    ASSERT(2 == bdlm::InstanceCount::nextInstanceNumber<unsigned>());
    ASSERT(2 == bdlm::InstanceCount::nextInstanceNumber<float>());
    ASSERT(3 == bdlm::InstanceCount::nextInstanceNumber<double>());
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'nextInstanceNumber<TYPE>()'
        //   Ensure the class method functions as expected.
        //
        // Concerns:
        //: 1 The method 'nextInstanceNumber' produces the expected value.
        //:
        //: 2 The method 'nextInstanceNumber' is thread-safe.
        //
        // Plan:
        //: 1 Invoke 'nextInstanceNumber' with a variety of types and directly
        //:   verify the returned value.  (C-1)
        //:
        //: 2 Stress test 'nextInstanceNumber' by creating a number of threads,
        //:   have each thread invoke 'nextInstanceNumber' a number of times
        //:   and store the returned values, and finally ensure the returned
        //:   values have no duplicates.  (C-2)
        //
        // Testing:
        //   InstanceCount::Value nextInstanceNumber<OBJECT_TYPE>();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'nextInstanceNumber'" << endl
                          << "============================" << endl;

        expectDefaultAllocation = true;

        if (verbose) cout << "\nDirect test." << endl;
        {
            ASSERT(1 == bdlm::InstanceCount::nextInstanceNumber<char>());
            ASSERT(1 == bdlm::InstanceCount::nextInstanceNumber<int>());
            ASSERT(1 == bdlm::InstanceCount::nextInstanceNumber<double>());
            ASSERT(2 == bdlm::InstanceCount::nextInstanceNumber<char>());
            ASSERT(2 == bdlm::InstanceCount::nextInstanceNumber<int>());
            ASSERT(1 == bdlm::InstanceCount::nextInstanceNumber<unsigned>());
            ASSERT(1 == bdlm::InstanceCount::nextInstanceNumber<float>());
            ASSERT(2 == bdlm::InstanceCount::nextInstanceNumber<double>());
            ASSERT(3 == bdlm::InstanceCount::nextInstanceNumber<char>());
            ASSERT(3 == bdlm::InstanceCount::nextInstanceNumber<int>());
            ASSERT(2 == bdlm::InstanceCount::nextInstanceNumber<unsigned>());
            ASSERT(2 == bdlm::InstanceCount::nextInstanceNumber<float>());
            ASSERT(3 == bdlm::InstanceCount::nextInstanceNumber<double>());
        }

        if (verbose) cout << "\nStress test." << endl;
        {
            const int k_NUM_THREADS =   5;
            const int k_NUM_SAMPLES = 500;

            bslmt::ThreadUtil::Handle handles[k_NUM_THREADS];
            WorkerData                data[k_NUM_THREADS];
            bslmt::Barrier            barrier(k_NUM_THREADS + 1);

            // create worker threads

            for (int i = 0; i < k_NUM_THREADS; ++i) {
                data[i].d_results.resize(k_NUM_SAMPLES);
                data[i].d_barrier_p = &barrier;
                bslmt::ThreadUtil::create(&handles[i], worker, &data[i]);
            }

            // start population of results

            barrier.wait();

            // wait for completion

            for (int i = 0; i < k_NUM_THREADS; ++i) {
                bslmt::ThreadUtil::join(handles[i]);
            }

            // verify each instance number is unique

            int count[k_NUM_THREADS * k_NUM_SAMPLES];
            for (int i = 0; i < k_NUM_THREADS * k_NUM_SAMPLES; ++i) {
                count[i] = 0;
            }
            for (int i = 0; i < k_NUM_THREADS; ++i) {
                for (int j = 0; j < k_NUM_SAMPLES; ++j) {
                    ASSERT(1 <= data[i].d_results[j]);

                    ASSERT(k_NUM_THREADS * k_NUM_SAMPLES
                                                      >= data[i].d_results[j]);

                    ++count[data[i].d_results[j] - 1];
                }
            }
            for (int i = 0; i < k_NUM_THREADS * k_NUM_SAMPLES; ++i) {
                ASSERT(1 == count[i]);
            }
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      } break;
    }

    // CONCERN: In no case does memory come from the global allocator.

    if (!expectGlobalAllocation) {
        ASSERTV(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());
    }

    // CONCERN: In no case does memory come from the default allocator.

    if (!expectDefaultAllocation) {
        ASSERTV(defaultAllocator.numBlocksTotal(),
                0 == defaultAllocator.numBlocksTotal());
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = "
                  << testStatus
                  << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
