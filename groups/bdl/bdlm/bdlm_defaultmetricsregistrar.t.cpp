// bdlm_defaultmetricsregistrar.t.cpp                                 -*-C++-*-
#include <bdlm_defaultmetricsregistrar.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_cstddef.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::endl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 1] MetricsRegistrar *defaultMetricsRegistrar();
// [ 1] MetricsRegistrar *metricsRegistrar(MR* bMR = 0);
// [ 1] MetricsRegistrar *notMonitoredMetricsRegistrar();
// [ 1] int setDefaultMetricsRegistrar(MR* bMR);
// ----------------------------------------------------------------------------
// [ 1] BOOTSTRAP TEST
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
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlm::DefaultMetricsRegistrar          Obj;
typedef bdlm::DefaultMetricsRegistrar_TestUtil ObjTestUtil;

// ============================================================================
//                       GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------

                        // ==========================
                        // class TestMetricsRegistrar
                        // ==========================

class TestMetricsRegistrar : public bdlm::MetricsRegistrar {
    // This class implements a pure abstract interface for clients and
    // suppliers of metrics registrars.  The implemtation does not register
    // callbacks with any monitoring system.

  public:
    // CREATORS
    TestMetricsRegistrar();
        // Create a 'TestMetricsRegistrar'.

    ~TestMetricsRegistrar();
        // Destroy this object.

    // MANIPULATORS
    int incrementInstanceCount(const bdlm::MetricDescriptor& metricDescriptor);
        // Do nothing with the specified 'metricDescriptor'.  Return 0.

    CallbackHandle registerCollectionCallback(
                                const bdlm::MetricDescriptor& metricDescriptor,
                                const Callback&               callback);
        // Do nothing with the specified 'metricsDescriptor' and 'callback'.
        // Return an aribitrary callback handle.

    int removeCollectionCallback(const CallbackHandle& handle);
        // Do nothing with the specified 'handle'.  Return 0.

    // ACCESSORS
    bsl::string defaultNamespace();
        // Return an empty string.

    bsl::string defaultObjectIdentifierPrefix();
        // Return an empty string.
};

                        // --------------------------
                        // class TestMetricsRegistrar
                        // --------------------------

// CREATORS
TestMetricsRegistrar::TestMetricsRegistrar()
{
}

TestMetricsRegistrar::~TestMetricsRegistrar()
{
}

// MANIPULATORS
int TestMetricsRegistrar::incrementInstanceCount(
                          const bdlm::MetricDescriptor& /* metricDescriptor */)
{
    return 0;
}

bdlm::MetricsRegistrar::CallbackHandle
                              TestMetricsRegistrar::registerCollectionCallback(
                          const bdlm::MetricDescriptor& /* metricDescriptor */,
                          const Callback&               /* callback */)
{
    return 0;
}

int TestMetricsRegistrar::removeCollectionCallback(
                                            const CallbackHandle& /* handle */)
{
    return 0;
}

// ACCESSORS
bsl::string TestMetricsRegistrar::defaultNamespace()
{
    return bsl::string();
}

bsl::string TestMetricsRegistrar::defaultObjectIdentifierPrefix()
{
    return bsl::string();
}

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Default Metrics Registrar Use
/// - - - - - - - - - - - - - - - - - - - - - - -
// This usage example illustrates the basics of class design that relate to
// proper use of the default metrics registrar, and introduces the standard
// pattern to apply when setting (and *locking*) the default metrics registrar.
// First we define a trivial class, 'my_ThreadPool', that uses a metrics
// registrar.  Note that the constructor is declared to take an *optional*
// 'bdlm::MetricsRegistrar *' as its last argument.  Also note that the
// expression
// 'bdlm::DefaultMetricsRegistrar::metricsRegistrar(basicMetricsRegistrar)' is
// used in the applicable member initializer to propagate the constructor's
// metrics registrar argument to the data members that require it.  If
// 'basicMetricsRegistrar' is 0, the object is created using the default
// metrics registrar.  Otherwise, the explicitly supplied metrics registrar is
// used:
//..
    class my_ThreadPool {
        // This is a trivial class solely intended to illustrate proper use
        // of the default metrics registrar.

        // DATA
        bdlm::MetricsRegistrar *d_metricsRegistrar_p;  // held, not owned

        // NOT IMPLEMENTED
        my_ThreadPool(const my_ThreadPool&);
        my_ThreadPool& operator=(const my_ThreadPool&);

      public:
        // CREATORS
        explicit my_ThreadPool(
                            bdlm::MetricsRegistrar *basicMetricsRegistrar = 0);
            // Create an ThreadPool object.  Optionally specify a
            // 'basicMetricsRegistrar' to be used for reporting metrics.  If
            // 'basicMetricsRegistrar' is 0, the currently installed default
            // metrics registrar is used.

        ~my_ThreadPool();
            // Destroy this thread pool object.
    };

    // CREATORS
    inline
    my_ThreadPool::my_ThreadPool(bdlm::MetricsRegistrar *basicMetricsRegistrar)
    : d_metricsRegistrar_p(bdlm::DefaultMetricsRegistrar::metricsRegistrar(
                                                        basicMetricsRegistrar))
    {
    }

    inline
    my_ThreadPool::~my_ThreadPool()
    {
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

// Now, we set the default metrics registrar to the unmonitored metrics
// registrar singleton provided in this component:
//..
    int status = bdlm::DefaultMetricsRegistrar::setDefaultMetricsRegistrar(
                bdlm::DefaultMetricsRegistrar::notMonitoredMetricsRegistrar());
    ASSERT(0 == status);
//..
// Finally, we instantiate an object of type 'my_ThreadPool', without supplying
// a metrics registrar.  Note that the creation of this object will lock the
// default metrics registrar, and subsequent invocations of
// 'setDefaultMetricsRegistrar' will fail:
//..
    my_ThreadPool pool;

    status = bdlm::DefaultMetricsRegistrar::setDefaultMetricsRegistrar(
                bdlm::DefaultMetricsRegistrar::notMonitoredMetricsRegistrar());
    ASSERT(1 == status);
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BOOTSTRAP TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 Initially, the default metrics registrar is the address of the
        //:   'notMonitoredMetricsRegistrar' singleton.
        //:
        //: 2 'defaultMetricsRegistrar' always returns the default metrics
        //:   registrar that is in effect at the point of the call.
        //:
        //: 3 'setDefaultMetricsRegistrar' succeeds until the default metrics
        //:   registrar is locked, and fails with no effect on the default
        //:   metrics registrar otherwise.
        //:
        //: 4 'metricsRegistrar', called with no argument, always returns the
        //:   default metrics registrar that is in effect at the point of the
        //:   call.
        //:
        //: 5 'metricsRegistrar', called with a non-zero argument, always
        //:   returns the argument and has no effect on the default metrics
        //:   registrar.
        //
        // Plan:
        //: 1 Verify the initial default metrics registrar is the
        //:   'notMonitoredMetricsRegistrar' singleton.  (C-1)
        //:
        //: 2 Verify 'defaultMetricsRegistrar' returns the expected value
        //:   repeatedly throughout this test case.  (C-2)
        //:
        //: 3 Verify 'setDefaultMetricsRegistrar' succeeds until the default
        //:   metric registrar is locked by 'defaultMetricsRegistrar' or
        //:   'metricsRegistrar' with argument 0, and then fails without effect
        //:   on the default metrics registrar.  (C-3)
        //:
        //: 4 Verify 'metricsRegistrar', called with no argument, returns the
        //:   expected value repeatedly throughout this test case.  (C-4)
        //:
        //: 5 Verify 'metricsRegistrar', called with a non-zero argument,
        //:   returns the expected value.  (C-5)
        //
        // Testing:
        //   MetricsRegistrar *defaultMetricsRegistrar();
        //   MetricsRegistrar *metricsRegistrar(MR* bMR = 0);
        //   MetricsRegistrar *notMonitoredMetricsRegistrar();
        //   int setDefaultMetricsRegistrar(MR* bMR);
        //   BOOTSTRAP TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BOOTSTRAP TEST" << endl
                          << "==============" << endl;

        TestMetricsRegistrar a;
        TestMetricsRegistrar b;
        TestMetricsRegistrar c;

        bdlm::MetricsRegistrar *notMonitored =
                                           Obj::notMonitoredMetricsRegistrar();
        ASSERT(0 != notMonitored);

        // initial default metrics registrar is the not monitored singleton

        ASSERT(notMonitored == Obj::defaultMetricsRegistrar());
        ASSERT(notMonitored == Obj::metricsRegistrar());
        ASSERT(           1 == Obj::setDefaultMetricsRegistrar(&a));
        ASSERT(notMonitored == Obj::defaultMetricsRegistrar());
        ASSERT(notMonitored == Obj::metricsRegistrar());

        ObjTestUtil::reset();

        // 'setDefaultMetricsRegistrar', 'defaultMetricsRegistrar', and
        // 'metricsRegistrar' with no argument

        ASSERT( 0 == Obj::setDefaultMetricsRegistrar(&a));
        ASSERT(&a == Obj::defaultMetricsRegistrar());
        ASSERT( 1 == Obj::setDefaultMetricsRegistrar(&b));
        ASSERT(&a == Obj::defaultMetricsRegistrar());
        ASSERT(&a == Obj::metricsRegistrar());

        ObjTestUtil::reset();

        ASSERT( 0 == Obj::setDefaultMetricsRegistrar(&a));
        ASSERT( 0 == Obj::setDefaultMetricsRegistrar(&b));
        ASSERT(&b == Obj::metricsRegistrar());
        ASSERT( 1 == Obj::setDefaultMetricsRegistrar(&a));
        ASSERT(&b == Obj::defaultMetricsRegistrar());
        ASSERT(&b == Obj::metricsRegistrar());

        ObjTestUtil::reset();

        ASSERT( 0 == Obj::setDefaultMetricsRegistrar(&a));
        ASSERT( 0 == Obj::setDefaultMetricsRegistrar(&b));
        ASSERT(&b == Obj::defaultMetricsRegistrar());
        ASSERT( 1 == Obj::setDefaultMetricsRegistrar(&a));
        ASSERT(&b == Obj::defaultMetricsRegistrar());

        ObjTestUtil::reset();

        // 'metricsRegistrar' with non-zero argument

        ASSERT( 0 == Obj::setDefaultMetricsRegistrar(&a));
        ASSERT(&b == Obj::metricsRegistrar(&b));
        ASSERT( 0 == Obj::setDefaultMetricsRegistrar(&c));
        ASSERT(&b == Obj::metricsRegistrar(&b));
        ASSERT(&c == Obj::metricsRegistrar());
        ASSERT( 1 == Obj::setDefaultMetricsRegistrar(&b));
        ASSERT(&a == Obj::metricsRegistrar(&a));
        ASSERT(&b == Obj::metricsRegistrar(&b));
        ASSERT(&c == Obj::metricsRegistrar());
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
