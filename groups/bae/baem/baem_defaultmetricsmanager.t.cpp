// baem_defaultmetricsmanager.t.cpp  -*-C++-*-
#include <baem_defaultmetricsmanager.h>

#include <baem_metricsmanager.h>
#include <baem_metricsample.h>
#include <baem_publisher.h>
#include <baem_streampublisher.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsl_ostream.h>
#include <bsl_cstring.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The 'baem_DefaultMetricsManager' is a namespace for a related set of
// functions of managing an instance of the 'baem_MetricsManager' they are
// interrelated and must be tested together.
//
// The 'baem_DefaultMetricsManagerScopedGuard' is a simple scoped guard with
// only a single constructor and destructor and can be test with a single
// case.
//-----------------------------------------------------------------------------
// baem_DefaultMetricsManager
// CLASS METHODS
// [ 3] static baem_MetricsManager *manager(baem_MetricsManager *manager);
// [ 1] static baem_MetricsManager *create(bslma_Allocator *);
// [ 2] static baem_MetricsManager *create(bsl::ostream& , bslma_Allocator *);
// [ 1] static baem_MetricsManager *instance();
// [ 1] static void destroy();
//-----------------------------------------------------------------------------
// baem_DefaultMetricsManagerScopedGuard
// CREATORS
// [ 4] baem_DefaultMetricsManagerScopedGuard(bslma_Allocator *);
// [ 5] baem_DefaultMetricsManagerScopedGuard(bsl::ostream&,
//                                            bslma_Allocator *);
// [ 3] ~baem_DefaultMetricsManagerScopedGuard();
//-----------------------------------------------------------------------------
// [ 6] USAGE EXAMPLE

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"
#define T_() cout << '\t' << flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef baem_DefaultMetricsManagerScopedGuard ObjGuard;
typedef baem_DefaultMetricsManager            Obj;
typedef baem_MetricsManager                   Mgr;

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    bslma_TestAllocator testAllocator; bslma_TestAllocator *Z = &testAllocator;
    bslma_TestAllocator defaultAllocator, globalAllocator;
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);

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
// the default 'baem_MetricsManager' instance.
//
///Example 1 - Create and access the default 'baem_MetricsManager' instance
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create the default 'baem_MetricManager'
// instance and publishes a single metric to the console.  See the
// documentation of 'baem_Metric' and 'baem_MetricsManager' for information on
// how to record metrics.
//
// First we create a 'baem_DefaultMetricsManagerScopedGuard', which manages the
// lifetime of the default metrics manager instance.  Note that the default
// metrics manager is intended to be created and destroyed by the *owner* of
// 'main'.  The instance should be created during the initialization of an
// application (while the task has a single thread) and destroyed just prior to
// termination (when there is similarly a single thread).
//..
//  int main(int argc, char *argv[])
    {

        // ...

        baem_DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Once the default instance has been created, it can be accessed using the
// static 'instance' method.
//..
        baem_MetricsManager *manager  = baem_DefaultMetricsManager::instance();
                      ASSERT(0 != manager);
//..
// The default metrics manager, by default, is configured with a
// 'baem_StreamPublisher' object that will publish all recorded metrics to the
// consoled.  We use the default 'manager' instance to update the collector
// for a single metric, and then publish all metrics.
//..
        baem_Collector *myMetric =
              manager->collectorRepository().getDefaultCollector("MyCategory",
                                                                 "MyMetric");
        myMetric->update(10);
        manager->publishAll();
//..
// The output of this example would look similar to:
//..
//..
// Note that the default metrics manager will be destroyed when 'managerGuard'
// exits this scope and is destroyed.  Clients that choose to explicitly call
// 'baem_DefaultMetricsManager::create()' must also explicitly call
// 'baem_DefaultMetricsManager::destroy()'.
    }
        ASSERT(0 == baem_DefaultMetricsManager::instance());

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CLASS: baem_DefaultMetricsManagerScopedGuard
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
        //    baem_DefaultMetricsManagerScopedGuard(bsl::cout, bslma_Allocator)
        //
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "baen_DefaultMetricsManagerScopedGuard(bsl::cout, ...)" << endl
            << "=====================================================" << endl;

          bslma_Default::setGlobalAllocator(&globalAllocator);
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
              bsl::vector<baem_Publisher *> publishers;
              ASSERT(1 == Obj::instance()->findGeneralPublishers(&publishers));
              ASSERT(0 != dynamic_cast<baem_StreamPublisher *>(publishers[0]));
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

              bsl::vector<baem_Publisher *> publishers;
              ASSERT(1 == Obj::instance()->findGeneralPublishers(&publishers));
              ASSERT(0 != dynamic_cast<baem_StreamPublisher *>(publishers[0]));

          }
          ASSERT(0 == testAllocator.numBytesInUse());
          ASSERT(0 == globalAllocator.numBytesInUse());
          ASSERT(0 == defaultAllocator.numBytesInUse());

          ASSERT(0 == Obj::instance());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CLASS: baem_DefaultMetricsManagerScopedGuard
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
        //    baem_DefaultMetricsManagerScopedGuard(bslma_Allocator *)
        //    ~baem_DefaultMetricsManagerScopedGuard();
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "baen_DefaultMetricsManagerScopedGuard" << endl
                          << "=====================================" << endl;

          bslma_Default::setGlobalAllocator(&globalAllocator);
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

              bsl::vector<baem_Publisher *> publishers;
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

              bsl::vector<baem_Publisher *> publishers;
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
        //    static baem_MetricsManager *manager(baem_MetricsManager *);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "METHOD: manager" << endl
                          << "===============" << endl;

          bslma_Default::setGlobalAllocator(&globalAllocator);
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
        // CLASS METHOD TEST: create(bool, bslma_Allocator *);
        //
        // Concerns:
        //    That the 'create' method behaves as documented
        //
        // Plan:
        //
        //
        // Testing:
        //  static baem_MetricsManager *create(bsl::ostream& ,
        //                                     bslma_Allocator *);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "METHOD: create(bsl::ostream& , ...)" << endl
                          << "===================================" << endl;

        bslma_Default::setGlobalAllocator(&globalAllocator);
        ASSERT(0 == Obj::instance());
        {
            if (verbose) {
                bsl::cout
                    << "\tTest with an explicit allocator."
                    << endl;
            }

            bslma_TestAllocator streamAllocator;
            bdema_ManagedPtr<bsl::ostringstream> ostream(
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
                bsl::vector<baem_Publisher *> publishers;
                ASSERT(1 == x->findGeneralPublishers(&publishers));
                baem_StreamPublisher *streamPublisher =
                           dynamic_cast<baem_StreamPublisher *>(publishers[0]);
                ASSERT(0 != streamPublisher);


                baem_Category CA("A");
                baem_MetricDescription MA(&CA, "A");
                baem_MetricId id(&MA);
                baem_MetricRecord record(id);
                baem_MetricSample sample;
                sample.appendGroup(&record, 1, bdet_TimeInterval(1, 0));

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
                bsl::vector<baem_Publisher *> publishers;
                ASSERT(1 == x->findGeneralPublishers(&publishers));
                baem_StreamPublisher *streamPublisher =
                           dynamic_cast<baem_StreamPublisher *>(publishers[0]);
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
        //  static baem_MetricsManager *create(bslma_Allocator *);
        //  static baem_MetricsManager *instance();
        //  static void destroy();
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY CLASS METHOD TEST" << endl
                          << "=========================" << endl;


        bslma_Default::setGlobalAllocator(&globalAllocator);
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
                bsl::vector<baem_Publisher *> publishers;
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
        bslma_Default::setGlobalAllocator(0);

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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
