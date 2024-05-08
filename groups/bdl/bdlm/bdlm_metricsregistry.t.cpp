// bdlm_metricsregistry.t.cpp                                         -*-C++-*-
#include <bdlm_metricsregistry.h>

#include <bdlm_metricdescriptor.h>

#include <bdlf_bind.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_string.h>
#include <bsl_functional.h>

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
//
// MANIPULATORS
//
// ACCESSORS
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE
// [ 3] DRQS 174793420

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

typedef bdlm::MetricsRegistry                   Obj;
typedef bdlm::MetricsRegistryRegistrationHandle ObjHandle;

const char longName[] = "abcdefghijklmnopqrstuvwxyz01234567890123456789012345";
BSLMF_ASSERT(sizeof(longName) > sizeof(bsl::string));

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
//                       GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------

                           // ===================
                           // class LargeCallback
                           // ===================

class LargeCallback {
    // This class implements the callback type 'MetricsAdapter::Callback' and
    // is very large so as to trigger allocation.

    // DATA
    char d_large[1024];

    public:
    // MANIPULATORS
    void operator()(BloombergLP::bdlm::Metric*);
        // Do nothing.
};

                           // ===================
                           // class LargeCallback
                           // ===================

void LargeCallback::operator()(BloombergLP::bdlm::Metric*)
{
}

                         // ========================
                         // class TestMetricsAdapter
                         // ========================

class TestMetricsAdapter : public bdlm::MetricsAdapter {
    // This class implements a pure abstract interface for clients and
    // suppliers of metrics adapters.  The implemtation does not register
    // callbacks with any monitoring system, but does track registrations to
    // enable testing of thread-enabled objects metric registration.

    // DATA
    bsl::map<int, bdlm::MetricDescriptor> d_descriptors;
    int                                   d_next;

  public:
    // CREATORS
    TestMetricsAdapter(bslma::Allocator *basicAllocator = 0);
        // Create a 'TestMetricsAdapter'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~TestMetricsAdapter();
        // Destroy this object.

    // MANIPULATORS
    CallbackHandle registerCollectionCallback(
                                const bdlm::MetricDescriptor& metricDescriptor,
                                const Callback&               callback);
        // Do nothing with the specified 'metricsDescriptor' and 'callback'.
        // Return a callback handle that will be verified in
        // 'removeCollectionCallback'.

    int removeCollectionCallback(const CallbackHandle& handle);
        // Do nothing with the specified 'handle'.  Assert the supplied
        // 'handle' matches what was provided by 'registerCollectionCallback'.
        // Return 0.

    // ACCESSORS
    int size() const;
        // Return the number of registered metrics.

    bool verify(const bdlm::MetricDescriptor& descriptor) const;
        // Return 'true' if there is one registered metric and its supplied
        // descriptor has the same value as the supplied 'descriptor', and
        // 'false' otherwise.
};

                         // ------------------------
                         // class TestMetricsAdapter
                         // ------------------------

// CREATORS
TestMetricsAdapter::TestMetricsAdapter(bslma::Allocator *basicAllocator)
: d_descriptors(basicAllocator)
, d_next(1000)
{
}

TestMetricsAdapter::~TestMetricsAdapter()
{
}

// MANIPULATORS
bdlm::MetricsAdapter::CallbackHandle
                                TestMetricsAdapter::registerCollectionCallback(
                                const bdlm::MetricDescriptor& metricDescriptor,
                                const Callback&               /* callback */)
{
    d_descriptors[d_next] = metricDescriptor;

    return d_next++;
}

int TestMetricsAdapter::removeCollectionCallback(const CallbackHandle& handle)
{
    d_descriptors.erase(handle);
    return 0;
}

// ACCESSORS
int TestMetricsAdapter::size() const
{
    return static_cast<int>(d_descriptors.size());
}

bool TestMetricsAdapter::verify(const bdlm::MetricDescriptor& descriptor) const
{
    return                             1 == d_descriptors.size()
        && d_descriptors.begin()->second == descriptor;
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bdlm::MetricsRegistry'
///- - - - - - - - - - - - - - - - - - - -
// This example demonstrates the initialization and usage of the
// 'bdlm::MetricsRegistry' object, allowing for registering metric callback
// functions with the 'bdlm' monitoring system.
//
// First, we declare a class that provides some metric for the 'bdlm'
// monitoring system:
//..
    class LowLevelFacility {
        // PRIVATE DATA
        bdlm::MetricsRegistryRegistrationHandle d_metricHandle;
      public:
        // CREATORS
        explicit LowLevelFacility(bdlm::MetricsRegistry& metricsRegistry =
                                     bdlm::MetricsRegistry::defaultInstance());

        // ACCESSORS
        int someMetric() const
        {
            return 0; // just a stub
        }
    };
//..
// Next, we provide a metric function to be used during callback registration:
//..
    void metricCallback(bdlm::Metric *value, const LowLevelFacility *object)
    {
        *value = bdlm::Metric::Gauge(object->someMetric());
    }
//..
// Here is the constructor definition that registers the collection callback:
//..
    LowLevelFacility::LowLevelFacility(bdlm::MetricsRegistry& metricsRegistry)
    {
        // Construct a 'bdlm::MetricsDescriptor' object to be used when
        // registering the callback function:
        bdlm::MetricDescriptor descriptor("bdlm",
                                          "example",
                                          1,
                                          "bdlmmetricsregistry",
                                          "bmr",
                                          "identifier");

        // Register the collection callback:
        metricsRegistry.registerCollectionCallback(
                                   &d_metricHandle,
                                   descriptor,
                                   bdlf::BindUtil::bind(&metricCallback,
                                                        bdlf::PlaceHolders::_1,
                                                        this));
        ASSERT(d_metricHandle.isRegistered());
    }
//..
// Notice that the compiler-supplied destructor is sufficient because the
// 'd_metricHandle' will deregister the metric on destruction.

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
        //   DRQS 174793420
        //
        // Concerns:
        //: 1 All allocations are from the specified allocator.
        //
        // Plan:
        //: 1 Provide an allocator to an object, then exercise it while
        //:   monitoring the default and global allocators.
        //
        // Testing:
        //   DRQS 174793420
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DRQS 174793420" << endl
                          << "==============" << endl;

        bslma::TestAllocator         oa("object", veryVeryVeryVerbose);
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX(&oa);

        bdlm::MetricDescriptor md(longName,
                                  longName,
                                  1,
                                  longName,
                                  longName,
                                  longName,
                                  &oa);

        typedef bsl::function<void(BloombergLP::bdlm::Metric*)> Callback;
        Callback fn(bsl::allocator_arg, &oa, LargeCallback());
        BloombergLP::bdlm::MetricsRegistryRegistrationHandle handle;
        mX.registerCollectionCallback(&handle, md, fn);

        ASSERTV(da.numBytesInUse(),
                da.numBlocksInUse(),
                0 == da.numBlocksInUse());
      } break;
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

// Now, we construct a 'bdlm::MetricsRegistry' object with a test allocator:
//..
    bslma::TestAllocator  ta;
    bdlm::MetricsRegistry registry(&ta);
    ASSERT(registry.numRegisteredCollectionCallbacks() == 0);
//..
// Then, we create the object and pass the constructed 'bdlm::MetricsRegistry'
// object there:
//..
    {
        LowLevelFacility facility(registry);
        ASSERT(registry.numRegisteredCollectionCallbacks() == 1);
//..
// If we don't provide a 'bdlm::MetricsRegistry' object explicitly, the default
// global instance will be used.
//
// Finally, the callback is removed the monitoring system by the destructor of
// 'facility' object:
//..
    } // 'facility.d_metricHandle.unregister()' is called here
    ASSERT(registry.numRegisteredCollectionCallbacks() == 0);
//..
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
        //: 1 Exercise the class.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

        bdlm::MetricDescriptor descriptor("mns", "mn", 1, "otn", "ota", "oi");
        TestMetricsAdapter adapter(&ta);

        Obj mX(&ta);

        ASSERT(0 == adapter.size());

        ObjHandle handle;
        mX.registerCollectionCallback(&handle, descriptor, &testMetric);

        ASSERT(0 == adapter.size());

        mX.setMetricsAdapter(&adapter);

        ASSERT(adapter.verify(descriptor));

        mX.removeMetricsAdapter(&adapter);

        ASSERT(0 == adapter.size());

        mX.setMetricsAdapter(&adapter);

        ASSERT(adapter.verify(descriptor));

        handle.unregister();

        ASSERT(0 == adapter.size());
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
