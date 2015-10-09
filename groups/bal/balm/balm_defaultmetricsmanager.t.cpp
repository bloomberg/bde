// balm_defaultmetricsmanager.t.cpp                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_defaultmetricsmanager.h>

#include <balm_metricsmanager.h>
#include <balm_metricsample.h>
#include <balm_publisher.h>
#include <balm_streampublisher.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_managedptr.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>

#include <bslim_testutil.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The 'balm::DefaultMetricsManager' is a namespace for a related set of
// functions of managing an instance of the 'balm::MetricsManager' they are
// interrelated and must be tested together.
//
// The 'balm::DefaultMetricsManagerScopedGuard' is a simple scoped guard with
// only a single constructor and destructor and can be test with a single
// case.
// ----------------------------------------------------------------------------
// balm::DefaultMetricsManager
// CLASS METHODS
// [ 3] static balm::MetricsManager *manager(balm::MetricsManager *manager);
// [ 1] static balm::MetricsManager *create(bslma::Allocator *);
// [ 2] static balm::MetricsManager *create(ostream& , Allocator *);
// [ 1] static balm::MetricsManager *instance();
// [ 1] static void destroy();
// ----------------------------------------------------------------------------
// balm::DefaultMetricsManagerScopedGuard
// CREATORS
// [ 4] balm::DefaultMetricsManagerScopedGuard(bslma::Allocator *);
// [ 5] balm::DefaultMetricsManagerScopedGuard(bsl::cout, Allocator)
// [ 3] ~balm::DefaultMetricsManagerScopedGuard();
// ----------------------------------------------------------------------------
// [ 6] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef balm::DefaultMetricsManagerScopedGuard ObjGuard;
typedef balm::DefaultMetricsManager            Obj;
typedef balm::MetricsManager                   Mgr;

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------


// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    bslma::TestAllocator          testAllocator;
    bslma::TestAllocator         *Z = &testAllocator;
    bslma::TestAllocator          defaultAllocator, globalAllocator;
    bslma::DefaultAllocatorGuard  guard(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

///Usage
///-----
// The following examples demonstrate how to create, configure, and destroy
// the default 'balm::MetricsManager' instance.
//
///Example 1 - Create and Access the Default 'balm::MetricsManager' Instance
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create the default 'balm::MetricManager'
// instance and publish a single metric to the console.  See the documentation
// of 'balm_metric' and 'balm_metricsmanager' for information on how to record
// metrics.
//
// First we create a 'balm::DefaultMetricsManagerScopedGuard', which manages
// the lifetime of the default metrics manager instance.  At construction, we
// provide the 'balm::DefaultMetricsManagerScopedGuard' an output stream
// ('stdout') to which it will publish metrics.  Note that the default metrics
// manager is intended to be created and destroyed by the *owner* of 'main'.
// The instance should be created during the initialization of an application
// (while the task has a single thread) and destroyed just prior to termination
// (when there is similarly a single thread).
//..
//  int main(int argc, char *argv[])
    {

        // ...

        balm::DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Once the default instance has been created, it can be accessed using the
// static 'instance' method.
//..
       balm::MetricsManager *manager = balm::DefaultMetricsManager::instance();
       ASSERT(0 != manager);
//..
// The default metrics manager, by default, is configured with a
// 'balm::StreamPublisher' object that will publish all recorded metrics to the
// consoled.  We use the default 'manager' instance to update the collector
// for a single metric, and then publish all metrics.
//..
        balm::Collector *myMetric =
                        manager->collectorRepository().getDefaultCollector(
                                                     "MyCategory", "MyMetric");
        myMetric->update(10);
        manager->publishAll();

        // ... rest of program elided ...
    }
//..
// The output of this example would look similar to:
//..
// 05FEB2009_19:20:12.697+0000 1 Records
//    Elapsed Time: 0.009311s
//            MyCategory.MyMetric [ count = 1, total = 10, min = 10, max = 10 ]
//..
// Note that the default metrics manager will be destroyed when 'managerGuard'
// exits this scope and is destroyed.  Clients that choose to explicitly call
// 'balm::DefaultMetricsManager::create()' must also explicitly call
// 'balm::DefaultMetricsManager::destroy()'.

        ASSERT(0 == balm::DefaultMetricsManager::instance());

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CLASS: balm::DefaultMetricsManagerScopedGuard
        //
        // Concerns:
        //    That the scoped guard behaves as described
        //
        // Plan:
        //    Create a scoped guard with an allocator, verify memory was
        //    allocated from that allocator and that the default instance has
        //    been created, the destroy the guard and verify memory was
        //    reclaimed and the default instance is 0.  Repeat the test
        //    without passing the optional allocator, verify memory was
        //    allocated from the global allocator.
        //
        //
        // Testing:
        //   balm::DefaultMetricsManagerScopedGuard(bsl::cout, Allocator)
        //
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "balm::DefaultMetricsManagerScopedGuard(cout, ...)" << endl
            << "=================================================" << endl;

          bslma::Default::setGlobalAllocator(&globalAllocator);
          ASSERT(0 == testAllocator.numBytesInUse());
          ASSERT(0 == globalAllocator.numBytesInUse());
          ASSERT(0 == defaultAllocator.numBytesInUse());

          ASSERT(0 == Obj::instance());
          {
              // Test with passed allocator.
              ObjGuard guard(bsl::cout, Z);
              ASSERT(0 <  testAllocator.numBytesInUse());
              ASSERT(0 == globalAllocator.numBytesInUse());
              ASSERT(0 == defaultAllocator.numBytesInUse());

              ASSERT(0 != Obj::instance());
              bsl::vector<balm::Publisher *> publishers;
              ASSERT(1 == Obj::instance()->findGeneralPublishers(&publishers));
              ASSERT(
                    0 != dynamic_cast<balm::StreamPublisher *>(publishers[0]));
          }

          ASSERT(0 == testAllocator.numBytesInUse());
          ASSERT(0 == globalAllocator.numBytesInUse());
          ASSERT(0 == defaultAllocator.numBytesInUse());

          ASSERT(0 == Obj::instance());
          {
              // Test with global allocator.
              ObjGuard guard(bsl::cout);
              ASSERT(0 == testAllocator.numBytesInUse());
              ASSERT(0 <  globalAllocator.numBytesInUse());
              ASSERT(0 == defaultAllocator.numBytesInUse());

              ASSERT(0 != Obj::instance());

              bsl::vector<balm::Publisher *> publishers;
              ASSERT(1 == Obj::instance()->findGeneralPublishers(&publishers));
              ASSERT(
                    0 != dynamic_cast<balm::StreamPublisher *>(publishers[0]));

          }
          ASSERT(0 == testAllocator.numBytesInUse());
          ASSERT(0 == globalAllocator.numBytesInUse());
          ASSERT(0 == defaultAllocator.numBytesInUse());

          ASSERT(0 == Obj::instance());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CLASS: balm::DefaultMetricsManagerScopedGuard
        //
        // Concerns:
        //    That the scoped guard behaves as described
        //
        // Plan:
        //    Create a scoped guard with an allocator, verify memory was
        //    allocated from that allocator and that the default instance has
        //    been created, the destroy the guard and verify memory was
        //    reclaimed and the default instance is 0.  Repeat the test
        //    without passing the optional allocator, verify memory was
        //    allocated from the global allocator.
        //
        //
        // Testing:
        //    balm::DefaultMetricsManagerScopedGuard(bslma::Allocator *)
        //    ~balm::DefaultMetricsManagerScopedGuard();
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "balm::DefaultMetricsManagerScopedGuard" << endl
                          << "======================================" << endl;

          bslma::Default::setGlobalAllocator(&globalAllocator);
          ASSERT(0 == testAllocator.numBytesInUse());
          ASSERT(0 == globalAllocator.numBytesInUse());
          ASSERT(0 == defaultAllocator.numBytesInUse());

          ASSERT(0 == Obj::instance());
          {
              // Test with passed allocator.
              ObjGuard guard(Z);
              ASSERT(0 <  testAllocator.numBytesInUse());
              ASSERT(0 == globalAllocator.numBytesInUse());
              ASSERT(0 == defaultAllocator.numBytesInUse());

              ASSERT(0 != Obj::instance());
              ASSERT(Obj::instance() == guard.instance());

              bsl::vector<balm::Publisher *> publishers;
              ASSERT(0 == Obj::instance()->findGeneralPublishers(&publishers));
          }

          ASSERT(0 == testAllocator.numBytesInUse());
          ASSERT(0 == globalAllocator.numBytesInUse());
          ASSERT(0 == defaultAllocator.numBytesInUse());

          ASSERT(0 == Obj::instance());
          {
              // Test with global allocator.
              ObjGuard guard;
              ASSERT(0 == testAllocator.numBytesInUse());
              ASSERT(0 <  globalAllocator.numBytesInUse());
              ASSERT(0 == defaultAllocator.numBytesInUse());

              ASSERT(0 != Obj::instance());
              ASSERT(Obj::instance() == guard.instance());

              bsl::vector<balm::Publisher *> publishers;
              ASSERT(0 == Obj::instance()->findGeneralPublishers(&publishers));

          }
          ASSERT(0 == testAllocator.numBytesInUse());
          ASSERT(0 == globalAllocator.numBytesInUse());
          ASSERT(0 == defaultAllocator.numBytesInUse());

          ASSERT(0 == Obj::instance());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CLASS METHOD TEST: manager
        //
        // Concerns:
        //    That the manager function behaves as described.
        //
        // Plan:
        //    Invoke 'manager' with both a valid pointer and 0, ensure the
        //    values returned is correct.  Perform that test both with and
        //    without a default instance of the metrics manager created.
        //
        // Testing:
        //    static balm::MetricsManager *manager(balm::MetricsManager *);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "METHOD: manager" << endl
                          << "===============" << endl;

          bslma::Default::setGlobalAllocator(&globalAllocator);
          Mgr x(Z); const Mgr& X = x;

          ASSERT(0  == Obj::instance());
          ASSERT(0  == Obj::manager(0));
          ASSERT(&X == Obj::manager(&x));

          ASSERT(0 <  testAllocator.numBytesInUse());
          ASSERT(0 == globalAllocator.numBytesInUse());
          ASSERT(0 == defaultAllocator.numBytesInUse());

          Mgr *y = Obj::create();

          ASSERT(0 <  testAllocator.numBytesInUse());
          ASSERT(0 <  globalAllocator.numBytesInUse());
          ASSERT(0 == defaultAllocator.numBytesInUse());

          ASSERT(y  == Obj::instance());
          ASSERT(y  == Obj::manager(0));
          ASSERT(&X == Obj::manager(&x));

          Obj::destroy();
          ASSERT(0 <  testAllocator.numBytesInUse());
          ASSERT(0 == globalAllocator.numBytesInUse());
          ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CLASS METHOD TEST: create(bool, bslma::Allocator *);
        //
        // Concerns:
        //    That the 'create' method behaves as documented
        //
        // Plan:
        //
        //
        // Testing:
        //   static balm::MetricsManager *create(ostream& , Allocator *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "METHOD: create(bsl::ostream& , ...)" << endl
                          << "===================================" << endl;

        bslma::Default::setGlobalAllocator(&globalAllocator);
        ASSERT(0 == Obj::instance());
        {
            if (verbose) {
                bsl::cout
                    << "\tTest with an explicit allocator."
                    << endl;
            }

            bslma::TestAllocator streamAllocator;
            bslma::ManagedPtr<bsl::ostringstream> ostream(
                                 new (streamAllocator) bsl::ostringstream(),
                                 &streamAllocator);

            // Create an instance and verify it is initialized.
            Mgr *x = Obj::create(*ostream, Z);
            ASSERT(0 != x);
            ASSERT(x == Obj::instance());

            // Verify memory usage is from the correct allocator.
            ASSERT(0 <  testAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Sanity check that the object returned is a valid metrics
            // manager.
            ASSERT(0 == x->metricRegistry().numMetrics());
            ASSERT(&x->metricRegistry() ==
                   &x->collectorRepository().registry());

            // Verify that the metrics manager is initialized with a stream
            // publisher.
            {
                bsl::vector<balm::Publisher *> publishers;
                ASSERT(1 == x->findGeneralPublishers(&publishers));
                balm::StreamPublisher *streamPublisher =
                          dynamic_cast<balm::StreamPublisher *>(publishers[0]);
                ASSERT(0 != streamPublisher);


                balm::Category CA("A");
                balm::MetricDescription MA(&CA, "A");
                balm::MetricId id(&MA);
                balm::MetricRecord record(id);
                balm::MetricSample sample;
                sample.appendGroup(&record, 1, bsls::TimeInterval(1, 0));

                ASSERT("" == ostream->str());
                streamPublisher->publish(sample);
                ASSERT("" != ostream->str());
            }

            // Verify release releases the instance
            Obj::destroy();
            ASSERT(0 == Obj::instance());

            ostream.clear();

            ASSERT(0 == testAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        ASSERT(0 == Obj::instance());
        {
            if (verbose) {
                bsl::cout << "\tTest with the default allocator"
                          << endl;
            }

            // Create an instance and verify it is initialized.
            Mgr *x = Obj::create(bsl::cout);
            ASSERT(0 != x);
            ASSERT(x == Obj::instance());

            // Verify memory usage is from the correct allocator.
            ASSERT(0 == testAllocator.numBytesInUse());
            ASSERT(0  < globalAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Sanity check that the object returned is a valid metrics
            // manager.
            ASSERT(0 == x->metricRegistry().numMetrics());
            ASSERT(&x->metricRegistry() ==
                   &x->collectorRepository().registry());

            // Verify that the metrics manager is initialized with a stream
            // publisher.
            {
                bsl::vector<balm::Publisher *> publishers;
                ASSERT(1 == x->findGeneralPublishers(&publishers));
                balm::StreamPublisher *streamPublisher =
                          dynamic_cast<balm::StreamPublisher *>(publishers[0]);
                ASSERT(0 != streamPublisher);
            }

            // Verify release releases the instance
            Obj::destroy();
            ASSERT(0 == Obj::instance());

            ASSERT(0 == testAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PRIMARY CLASS METHOD TEST:
        //
        // Concerns:
        //    That the primary functions, 'create', 'instance', and 'release'
        //    all behave as documented.
        //
        // Plan:
        //    Using a supplied allocator, create a default instance for the
        //    metrics manager, validate the memory is allocated from the
        //    correct allocator, and that the default instance is valid,
        //    release the default instance and verify the memory is
        //    reclaimed.  Perform the same test without supplying an allocator
        //    (using the global allocator).
        //
        // Testing:
        //  static balm::MetricsManager *create(bslma::Allocator *);
        //  static balm::MetricsManager *instance();
        //  static void destroy();
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY CLASS METHOD TEST" << endl
                          << "=========================" << endl;


        bslma::Default::setGlobalAllocator(&globalAllocator);
        ASSERT(0 == Obj::instance());
        {
            if (verbose) {
                bsl::cout
                    << "\tCreate a default instance from supplied allocator."
                    << endl;
            }

            // Create an instance and verify it is initialized.
            Mgr *x = Obj::create(Z);
            ASSERT(0 != x);
            ASSERT(x == Obj::instance());

            // Verify memory usage is from the correct allocator.
            ASSERT(0 <  testAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify that the metrics manager is initialized with a stream
            // publisher.
            {
                bsl::vector<balm::Publisher *> publishers;
                ASSERT(0 == x->findGeneralPublishers(&publishers));
                ASSERT(0 == publishers.size());
            }

            // Sanity check that the object returned is a valid metrics
            // manager.
            ASSERT(0 == x->metricRegistry().numMetrics());
            ASSERT(&x->metricRegistry() ==
                   &x->collectorRepository().registry());

            // Verify release releases the instance
            Obj::destroy();
            ASSERT(0 == Obj::instance());

            ASSERT(0 == testAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());

        }
        {
            if (verbose) {
                bsl::cout
                    << "\tCreate a default instance from global allocator."
                    << endl;
            }

            // Create an instance and verify it is initialized.
            Mgr *x = Obj::create();
            ASSERT(0 != x);
            ASSERT(x == Obj::instance());

            // Verify memory usage is from the correct allocator.
            ASSERT(0 == testAllocator.numBytesInUse());
            ASSERT(0  < globalAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Sanity check that the object returned is a valid metrics
            // manager.
            ASSERT(0 == x->metricRegistry().numMetrics());
            ASSERT(&x->metricRegistry() ==
                   &x->collectorRepository().registry());

            // Verify release releases the instance
            Obj::destroy();
            ASSERT(0 == Obj::instance());

            ASSERT(0 == testAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());

        }
        bslma::Default::setGlobalAllocator(0);

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
