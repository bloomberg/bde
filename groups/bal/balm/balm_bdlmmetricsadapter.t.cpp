// balm_bdlmmetricsadapter.t.cpp                                      -*-C++-*-
#include <balm_bdlmmetricsadapter.h>

#include <balm_metricsmanager.h>

#include <bdlm_metricdescriptor.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstddef.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::endl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// ----------------------------------------------------------------------------
// CREATORS
// [ 1] BdlmMetricsAdapter(*mM, mN, oIP, *bA = 0);
//
// MANIPULATORS
// [ 2] CH registerCollectionCallback(metricDescriptor, cb);
// [ 2] int removeCollectionCallback(const CH& handle);
//
// ACCESSORS
// [ 1] const bsl::string& defaultMetricNamespace();
// [ 1] const bsl::string& defaultObjectIdentifierPrefix();
// [ 1] bslma::Allocator *allocator() const;
// ----------------------------------------------------------------------------
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ *] CONCERN: In no case does memory come from the default allocator.
// [ 3] USAGE EXAMPLE

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
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef balm::BdlmMetricsAdapter Obj;

// ============================================================================
//                   GLOBAL FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static int s_testMetricCount = 0;

void testMetric(BloombergLP::bdlm::Metric *value)
{
    (void)value;
    ++s_testMetricCount;
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'balm::BdlmMetricsAdapter'
///- - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the initialization and usage of the
// 'balm::BdlmMetricsAdapter' object, allowing for registering metric callback
// functions with the 'balm' monitoring system.
//
// First, we provide a metric function to be used during callback registration
// with the 'balm' monitoring system:
//..
    void elidedMetric(BloombergLP::bdlm::Metric *value)
    {
        (void)value;
        // ...
    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
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
      case 3: {
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

        expectDefaultAllocation = true;

// Then, we construct a 'balm::MetricsManager' object and use it to construct a
// 'balm::BdlmMetricsAdapter' that will use "bdlm" as its default metric
// namespace, "svc" as its default object indentifier prefix, and will not
// attempt to set itself as the default metrics adapter:
//..
    balm::MetricsManager     manager;
    balm::BdlmMetricsAdapter adapter(&manager, "bdlm", "svc");
//..
// Next, we construct a 'bdlm::MetricsDescriptor' object to be used when
// registering the callback function, using constants from
// 'bdlm::MetricDescriptor' for the namespace and identifier to indicate the
// implementation of the 'bdlm::MetricsAdapter' protocol should supply values:
//..
    bdlm::MetricDescriptor descriptor(
            bdlm::MetricDescriptor::k_USE_METRICS_ADAPTER_NAMESPACE_SELECTION,
            "example",
            1,
            "balm.bdlmmetricsadapter",
            "bmr",
            bdlm::MetricDescriptor::k_USE_METRICS_ADAPTER_OBJECT_ID_SELECTION);
//..
// Now, we register the collection callback:
//..
    bdlm::MetricsAdapter::CallbackHandle handle =
                              adapter.registerCollectionCallback(descriptor,
                                                                 elidedMetric);
//..
// Finally, presumably during shutdown of the application, we remove the
// callback from the monitoring system, and verify the callback was
// successfully removed:
//..
    ASSERT(0 == adapter.removeCollectionCallback(handle));
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CALLBACK REGISTRATON
        //   Ensure the callback registration and removal manipulators function
        //   as expected.
        //
        // Concerns:
        //: 1 'registerCollectionCallback' registers a callback with the
        //:   supplied 'balm::MetricsManager'.
        //:
        //: 2 'removeCollectionCallback' removes a callback with the
        //:   supplied 'balm::MetricsManager' and provides the correct return
        //:   value.
        //
        // Plan:
        //: 1 Register a collection callback, invoke the 'publishAll' method on
        //:   the metrics manager, and verify the callback was invoked.  (C-1)
        //:
        //: 2 Remove the callback, verify the return value indicates the
        //:   removal of a callback, invoke 'publishAll', and verify the
        //:   callback was not invoked.
        //:
        //: 3 Attempt to remove the callback again, and verify the return code
        //:   indicates no callback was removed.  (C-2)
        //
        // Testing:
        //   CH registerCollectionCallback(metricDescriptor, cb);
        //   int removeCollectionCallback(const CH& handle);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CALLBACK REGISTRATION" << endl
                                  << "=====================" << endl;

        expectDefaultAllocation = true;

        balm::MetricsManager manager;

        Obj mX(&manager, "namespace", "prefix");

        bdlm::MetricDescriptor descriptor("ns", "n", 1, "otn", "oa", "oid");

        bdlm::MetricsAdapter::CallbackHandle handle =
                                     mX.registerCollectionCallback(descriptor,
                                                                   testMetric);
        ASSERT(0 == s_testMetricCount);

        manager.publishAll();

        ASSERT(1 == s_testMetricCount);

        ASSERT(0 == mX.removeCollectionCallback(handle));

        manager.publishAll();

        ASSERT(1 == s_testMetricCount);

        ASSERT(0 != mX.removeCollectionCallback(handle));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR & BASIC ACCESSORS
        //   Ensure the constructor appropriatly stores the specified values
        //   and that the basic accessors return these values.
        //
        // Concerns:
        //: 1 The constructor correctly stores the default metric namespace,
        //:   default object identifier prefix, and the allocator.
        //:
        //: 2 The basic accessors return these values.
        //:
        //: 3 The constructor has the internal memory management system hooked
        //:   up properly so that *all* internally allocated memory draws from
        //:   the same user-supplied allocator whenever one is specified.
        //:
        //: 4 Memory is not leaked by any method and the destructor properly
        //:   deallocates the residual allocated memory.
        //
        // Plan:
        //: 1 Create objects and verify the values returned by the basic
        //:   accessors.  (C-1,2)
        //:
        //: 2 Use a supplied 'bslma::TestAllocator' that goes out-of-scope
        //:   at the conclusion of each test to ensure all memory is taken from
        //:   and returned to the allocator.  (C-3,4)
        //
        // Testing:
        //   BdlmMetricsAdapter(*mM, mN, oIP, *bA = 0);
        //   const bsl::string& defaultMetricNamespace();
        //   const bsl::string& defaultObjectIdentifierPrefix();
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CONSTRUCTOR & BASIC ACCESSORS" << endl
                                  << "=============================" << endl;

        bslma::TestAllocator managerTestAllocator;

        balm::MetricsManager manager(&managerTestAllocator);

        {
            Obj mX(&manager, "namespace", "prefix");
            const Obj& X = mX;

            ASSERT("namespace"       == X.defaultMetricNamespace());
            ASSERT("prefix"          == X.defaultObjectIdentifierPrefix());
            ASSERT(&defaultAllocator == X.allocator());
        }
        {
            bslma::TestAllocator stringTestAllocator;

            bsl::string metricNamespace(
                                "namespace long enough to cause an allocation",
                                &stringTestAllocator);

            bsl::string prefix("prefix long enough to cause an allocation",
                               &stringTestAllocator);

            ASSERT(2 == stringTestAllocator.numAllocations());

            bslma::TestAllocator testAllocator;

            Obj mX(&manager, metricNamespace, prefix, &testAllocator);
            const Obj& X = mX;

            ASSERT(metricNamespace == X.defaultMetricNamespace());
            ASSERT(prefix          == X.defaultObjectIdentifierPrefix());
            ASSERT(&testAllocator  == X.allocator());

            ASSERT(2 == testAllocator.numAllocations());
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
