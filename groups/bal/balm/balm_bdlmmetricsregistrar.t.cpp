// balm_bdlmmetricsregistrar.t.cpp                                    -*-C++-*-
#include <balm_bdlmmetricsregistrar.h>

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
// [ 2] BdlmMetricsRegistrar(*mM, mN, oIP, iAD, *bA = 0);
//
// MANIPULATORS
// [ 3] int incrementInstanceCount(metricDescriptor);
// [ 4] CH registerCollectionCallback(metricDescriptor, cb);
// [ 4] int removeCollectionCallback(const CH& handle);
//
// ACCESSORS
// [ 2] const bsl::string& defaultMetricNamespace();
// [ 2] const bsl::string& defaultObjectIdentifierPrefix();
// [ 2] bslma::Allocator *allocator() const;
// ----------------------------------------------------------------------------
// [ 1] CONCERN: 'false == installAsDefault' works correctly
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ *] CONCERN: In no case does memory come from the default allocator.
// [ 5] USAGE EXAMPLE

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

typedef balm::BdlmMetricsRegistrar Obj;

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
///Example 1: Using 'balm::BdlmMetricsRegistrar'
///- - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the initialization and usage of the
// 'balm::BdlmMetricsRegistrar' object, allowing for registering metric
// callback functions with the 'balm' monitoring system.
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
      case 5: {
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
// 'balm::BdlmMetricsRegistrar' that will use "bdlm" as its default metric
// namespace, "svc" as its default object indentifier prefix, and will not
// attempt to set itself as the default metrics registrar:
//..
    balm::MetricsManager manager;
    balm::BdlmMetricsRegistrar registrar(&manager, "bdlm", "svc", false);
//..
// Next, we construct a 'bdlm::MetricsDescriptor' object to be used when
// registering the callback function, making use of the helper methods in the
// 'bdlm::MetricsRegistrar' protocol:
//..
    bdlm::MetricDescriptor descriptor(registrar.defaultMetricNamespace(),
                                      "example",
                                      "balm.bdlmmetricsregistrar",
                                      "");
    {
        bsl::stringstream identifier;
        identifier << registrar.defaultObjectIdentifierPrefix()
                   << ".bmr."
                   << registrar.incrementInstanceCount(descriptor);
        descriptor.setObjectIdentifier(identifier.str());
    }
//..
// Now, we register the collection callback:
//..
    bdlm::MetricsRegistrar::CallbackHandle handle =
                            registrar.registerCollectionCallback(descriptor,
                                                                 elidedMetric);
//..
// Finally, presumably during shutdown of the application, we remove the
// callback from the monitoring system, and verify the callback was
// successfully removed:
//..
    ASSERT(0 == registrar.removeCollectionCallback(handle));
//..

      } break;
      case 4: {
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
        //   int incrementInstanceCount(metricDescriptor);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'incrementInstanceCount'" << endl
                                  << "========================" << endl;

        expectDefaultAllocation = true;

        balm::MetricsManager manager;

        Obj mX(&manager, "namespace", "prefix", false);

        bdlm::MetricDescriptor descriptor("ns", "n", "otn", "oid");

        bdlm::MetricsRegistrar::CallbackHandle handle =
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
      case 3: {
        // --------------------------------------------------------------------
        // 'incrementInstanceCount'
        //   Ensure the manipulator functions as expected.
        //
        // Concerns:
        //: 1 The method correctly provides the instances counts.
        //:
        //: 2 The method ignores the object identifier attribute.
        //:
        //: 3 The constructor has the internal memory management system hooked
        //:   up properly so that *all* internally allocated memory draws from
        //:   the same user-supplied allocator whenever one is specified.
        //:
        //: 4 Memory is not leaked by any method and the destructor properly
        //:   deallocates the residual allocated memory.
        //
        // Plan:
        //: 1 Create a sequence of 'bdlm::MetricDescriptor', invoke
        //:   'incrementInstanceCount', and verify the resultant count.
        //:   (C-1,2)
        //:
        //: 2 Use a supplied 'bslma::TestAllocator' that goes out-of-scope
        //:   at the conclusion of each test to ensure all memory is taken from
        //:   and returned to the allocator.  (C-3,4)
        //
        // Testing:
        //   int incrementInstanceCount(metricDescriptor);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'incrementInstanceCount'" << endl
                                  << "========================" << endl;

        static const struct {
            const char *d_metricNamespace;
            const char *d_metricName;
            const char *d_objectTypeName;
            const char *d_objectIdentifier;
            const int   d_exp;
        } VALUES[] = {
            {  "a", "a", "a", "a", 1 },
            {  "a", "a", "a", "b", 2 },
            {  "b", "a", "a", "a", 1 },
            {  "b", "a", "a", "b", 2 },
            {  "a", "b", "a", "a", 1 },
            {  "a", "b", "a", "b", 2 },
            {  "a", "a", "b", "a", 1 },
            {  "a", "a", "b", "b", 2 },
            {  "a", "a", "a", "a", 3 },
            {  "b", "a", "a", "a", 3 },
            {  "a", "b", "a", "a", 3 },
            {  "a", "a", "b", "a", 3 },
            {  "a", "a", "a", "b", 4 },
            {  "b", "a", "a", "b", 4 },
            {  "a", "b", "a", "b", 4 },
            {  "a", "a", "b", "b", 4 },
        };
        const int NUM_VALUES =
                              static_cast<int>(sizeof VALUES / sizeof *VALUES);

        bslma::TestAllocator testAllocator;
        balm::MetricsManager manager(&testAllocator);

        Obj mX(&manager, "namespace", "prefix", false, &testAllocator);

        for (int i = 0; i < NUM_VALUES; ++i) {
            const char *METRIC_NAMESPACE  = VALUES[i].d_metricNamespace;
            const char *METRIC_NAME       = VALUES[i].d_metricName;
            const char *OBJECT_TYPE_NAME  = VALUES[i].d_objectTypeName;
            const char *OBJECT_IDENTIFIER = VALUES[i].d_objectIdentifier;
            const int   EXP               = VALUES[i].d_exp;

            bdlm::MetricDescriptor descriptor(METRIC_NAMESPACE,
                                              METRIC_NAME,
                                              OBJECT_TYPE_NAME,
                                              OBJECT_IDENTIFIER,
                                              &testAllocator);

            ASSERT(EXP == mX.incrementInstanceCount(descriptor));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR & BASIC ACCESSORS
        //   Ensure the constructor appropriatly stores the specified values
        //   and that the basic accessors return these values.
        //
        // Concerns:
        //: 1 Construct an object with 'true == installAsDefault' and verofy
        //:   the registrar is installed as the default.
        //:
        //: 2 The constructor correctly stores the default metric namespace,
        //:   default object identifier prefix, and the allocator.
        //:
        //: 3 The basic accessors return these values.
        //:
        //: 4 The constructor has the internal memory management system hooked
        //:   up properly so that *all* internally allocated memory draws from
        //:   the same user-supplied allocator whenever one is specified.
        //:
        //: 5 Memory is not leaked by any method and the destructor properly
        //:   deallocates the residual allocated memory.
        //
        // Plan:
        //: 1 Create an object with 'true == installAsDefault' and verify the
        //:   resultant default metrics registrar.  (C-1)
        //:
        //: 2 Create objects and verify the values returned by the basic
        //:   accessors.  (C-2,3)
        //:
        //: 3 Use a supplied 'bslma::TestAllocator' that goes out-of-scope
        //:   at the conclusion of each test to ensure all memory is taken from
        //:   and returned to the allocator.  (C-4,5)
        //
        // Testing:
        //   BdlmMetricsRegistrar(*mM, mN, oIP, iAD, *bA = 0);
        //   const bsl::string& defaultMetricNamespace();
        //   const bsl::string& defaultObjectIdentifierPrefix();
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CONSTRUCTOR & BASIC ACCESSORS" << endl
                                  << "=============================" << endl;

        bslma::TestAllocator managerTestAllocator;

        balm::MetricsManager manager(&managerTestAllocator);

        {
            Obj mX(&manager, "namespace", "prefix", true);
            const Obj& X = mX;

            ASSERT(bdlm::DefaultMetricsRegistrar::defaultMetricsRegistrar() ==
                                                                          &mX);

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

            Obj mX(&manager, metricNamespace, prefix, false, &testAllocator);
            const Obj& X = mX;

            ASSERT(metricNamespace == X.defaultMetricNamespace());
            ASSERT(prefix          == X.defaultObjectIdentifierPrefix());
            ASSERT(&testAllocator  == X.allocator());

            ASSERT(2 == testAllocator.numAllocations());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'false == installAsDefault'
        //   Ensure when the constructor 'installAsDefault' is 'false', the
        //   default metrics registrar is not affectd.
        //
        // Concerns:
        //: 1 When 'false == installAsDefault', the constructor does not
        //:   attempt to install the default metrics registrar.
        //
        // Plan:
        //: 1 Create an object with 'false == installAsDefault'  and verify
        //:   default metrics registrar is unset.  (C-1)
        //
        // Testing:
        //   CONCERN: 'false == installAsDefault' works correctly
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'false == installAsDefault'" << endl
                                  << "===========================" << endl;

        bslma::TestAllocator testAllocator;

        balm::MetricsManager manager(&testAllocator);

        Obj mX(&manager, "namespace", "prefix", false, &testAllocator);

        ASSERT(bdlm::DefaultMetricsRegistrar::defaultMetricsRegistrar() ==
                bdlm::DefaultMetricsRegistrar::notMonitoredMetricsRegistrar());
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
// Copyright 2023 Bloomberg Finance L.P.
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
