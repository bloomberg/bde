// balm_configurationutil.t.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_configurationutil.h>

#include <balm_category.h>
#include <balm_defaultmetricsmanager.h>
#include <balm_metricdescription.h>
#include <balm_metricformat.h>
#include <balm_metrics.h>
#include <balm_metricsample.h>
#include <balm_publisher.h>

#include <bdlf_bind.h>
#include <bslma_allocator.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsl_ostream.h>
#include <bsl_cstring.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bslim_testutil.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 1] static int setPreferredPublicationType(
//                               const char                   *category,
//                               const char                   *metricName,
//                               balm::PublicationType::Value  publicationType,
//                               balm::MetricsManager         *manager = 0);
// [ 2] static int setFormat(const char                *category,
//                           const char                *metricName,
//                           const balm::MetricFormat&  format,
//                           balm::MetricsManager      *manager = 0);
// [ 3] static int setFormatSpec(
//                              const char                    *category,
//                              const char                    *metricName,
//                              balm::PublicationType::Value   publicationType,
//                              const balm::MetricFormatSpec&  formatSpec,
//                              balm::MetricsManager          *manager = 0);
// [ 4] createUserDataKey(balm::MetricsManager *);
// [ 5] static void setUserData(const char *,
//                              const char *,
//                              balm::MetricDescription::UserDataKey,
//                              const void *,
//                              balm::MetricsManager *);
// [ 6] static void setUserData(const char *,
//                              balm::MetricDescription::UserDataKey,
//                              const void *,
//                              balm::MetricsManager *);
// ----------------------------------------------------------------------------
// [ 7] USAGE EXAMPLE 1
// [ 8] USAGE EXAMPLE 2

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

typedef balm::ConfigurationUtil              Obj;
typedef balm::MetricFormatSpec               Spec;
typedef balm::MetricsManager                 MMgr;
typedef balm::PublicationType                Type;
typedef balm::MetricRegistry                 Registry;
typedef balm::MetricDescription::UserDataKey Key;
// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Using a Metric's User Data
/// - - - - - - - - - - - - -
// In the following example we configure, using 'balm::ConfigurationUtil',
// application specific thresholds for a series of metrics that will
// be used by a publisher to determine whether each metric should be written
// to the console.  For simplicity, the metric thresholds in this example will
// be a simple unsigned integer value that will be compared with the metric's
// total.
//
// We start by defining an application specific publisher implementation.
// This implementation is supplied a user data key on construction, which it
// uses to look up the threshold for a particular metric.  If a metric's total
// value is greater than its threshold, it will log the metric to the console.
//..
    // thresholdpublisher.h
    class ThresholdPublisher : public balm::Publisher {
        // A simple implementation of the 'balm::Publisher' protocol that
        // writes metric records to the console when their value is greater
        // than an application specific threshold.
//
        // DATA
        balm::MetricDescription::UserDataKey d_thresholdKey;  // key for a
                                                              // metric's
                                                              // threshold
//
        // NOT IMPLEMENTED
        ThresholdPublisher(const ThresholdPublisher& );
        ThresholdPublisher& operator=(const ThresholdPublisher& );
//
    public:
        // CREATORS
        ThresholdPublisher(balm::MetricDescription::UserDataKey thresholdKey);
            // Create this publisher that will publish metrics to the console
            // if their total value is greater than their associated
            // threshold, accessed via the specified 'thresholdKey'.
//
        virtual ~ThresholdPublisher();
             // Destroy this publisher.
//
        // MANIPULATORS
        virtual void publish(const balm::MetricSample& metricValues);
            // Publish the specified 'metricValues' to the console if they are
            // greater than their associated threshold.
    };
//
    // thresholdpublisher.cpp
//
    // CREATORS
    ThresholdPublisher::ThresholdPublisher(
                             balm::MetricDescription::UserDataKey thresholdKey)
    : d_thresholdKey(thresholdKey)
    {
    }
//
    ThresholdPublisher::~ThresholdPublisher()
    {
    }
//
    // MANIPULATORS
    void ThresholdPublisher::publish(const balm::MetricSample& metricValues)
    {
        if (0 >= metricValues.numRecords()) {
            return;                                                   // RETURN
        }
        balm::MetricSample::const_iterator sIt = metricValues.begin();
        for (; sIt != metricValues.end(); ++sIt) {
            balm::MetricSampleGroup::const_iterator gIt = sIt->begin();
            for (; gIt != sIt->end(); ++gIt) {
//..
// We now use the user data key to lookup the address of the threshold value.
// If this address is 0, no threshold is specified for the metric.
//..
                const balm::MetricDescription& description =
                                                *gIt->metricId().description();
                const unsigned int *thresholdPtr =
                    (const unsigned int *)description.userData(d_thresholdKey);
                if (thresholdPtr && gIt->total() > *thresholdPtr) {
                    bsl::cout << "WARNING: " << gIt->metricId()
                              << " = "       << gIt->total()
                              << bsl::endl;
                }
            }
        }
    }


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

    const Type::Value ALL_TYPES[] = {
        Type::e_UNSPECIFIED,
        Type::e_TOTAL,
        Type::e_COUNT,
        Type::e_MIN,
        Type::e_MAX,
        Type::e_AVG,
        Type::e_RATE,
        Type::e_RATE_COUNT
    };
    const int NUM_TYPES = sizeof ALL_TYPES / sizeof *ALL_TYPES;

    bslma::TestAllocator cAlloc;
    balm::MetricFormat formatA(&cAlloc);
    balm::MetricFormat formatB(&cAlloc);
    formatB.setFormatSpec(Type::e_TOTAL, Spec(1.0, "%f"));
    balm::MetricFormat formatC(&cAlloc);
    formatC.setFormatSpec(Type::e_TOTAL, Spec(1.0, "%f"));
    formatC.setFormatSpec(Type::e_AVG, Spec(1.0, "%d"));
    balm::MetricFormat formatD(&cAlloc);
    formatD.setFormatSpec(Type::e_MIN, Spec(2.0, "%f"));
    formatD.setFormatSpec(Type::e_MAX, Spec(2.0, "%d"));

    const balm::MetricFormat *FORMATS[] = {
        &formatA,
        &formatB,
        &formatC,
        &formatD,
    };
    const int NUM_FORMATS = sizeof FORMATS / sizeof *FORMATS;

    bslma::TestAllocator testAlloc; bslma::TestAllocator *Z = &testAlloc;
    bslma::TestAllocator defaultAllocator;
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 2
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

        if (verbose) cout << "\nTesting Usage Example 2"
                          << "\n=======================" << endl;

//..
// Next we define a couple threshold constants for our metrics:
//..
    static const unsigned int ELAPSED_TIME_THRESHOLD = 10;
    static const unsigned int NUM_REQUESTS_THRESHOLD = 100;
//..
// Now, we configure a default metrics manager and publish a couple
// example metrics.  We start by initializing a default metrics manager by
// creating a 'balm::DefaultMetricsManagerScopedGuard', which manages the
// lifetime of the default metrics manager instance:
//..
//  int main(int argc, char *argv[])
//  {
//
        // ...
        bslma::Allocator *allocator = bslma::Default::allocator(0);
        balm::DefaultMetricsManagerScopedGuard managerGuard;
//..
// Now we create a user data key for our threshold information:
//..
        balm::MetricDescription::UserDataKey thresholdKey =
                                  balm::ConfigurationUtil::createUserDataKey();
//..
// Now we create an instance of our application specific publisher type,
// 'ThresholdPublisher', and configure the default metrics manager to publish
// metrics to it:
//..
        bsl::shared_ptr<balm::Publisher> publisher(
                             new (*allocator) ThresholdPublisher(thresholdKey),
                             allocator);
        balm::DefaultMetricsManager::instance()->addGeneralPublisher(
                                                                    publisher);
//..
// Next we configure two metric thresholds:
//..
        balm::ConfigurationUtil::setUserData("myCategory",
                                            "elapsedTime",
                                            thresholdKey,
                                            &ELAPSED_TIME_THRESHOLD);
        balm::ConfigurationUtil::setUserData("myCategory",
                                            "numRequests",
                                            thresholdKey,
                                            &NUM_REQUESTS_THRESHOLD);
//..
// Now we update the value of a couple metrics.  Note that recorded number of
// requests is greater than the configured threshold:
//..
        BALM_METRICS_UPDATE("myCategory", "elapsedTime", 2);
        BALM_METRICS_UPDATE("myCategory", "numRequests", 150);
// Finally, we publish the metric.  Note that in practice, clients of
// the 'balm' package can use the 'balm::PublicationScheduler' to schedule the
// periodic publication of metrics:
//..
    balm::DefaultMetricsManager::instance()->publishAll();
//..
// The console ouput of the publication will look like:
//..
//  WARNING: myCategory.numRequests = 150
//..

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
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

        if (verbose) cout << "\nTesting Usage Example 1"
                          << "\n=======================" << endl;

///Usage
///-----
// The following examples demonstrate how to use 'balm::ConfigurationUtil'.
//
///Configuring the Output of a Metric
/// - - - - - - - - - - - - - - - - -
// This example uses 'balm::ConfigurationUtil' to configure the output for a
// metric.
//
// We start by initializing a default metrics manager by creating a
// 'balm::DefaultMetricsManagerScopedGuard', which manages the
// lifetime of the default metrics manager instance.  At construction, we
// provide the scoped guard an output stream ('stdout') to which the default
// metrics manager will publish metrics.  Note that the default metrics
// manager is intended to be created and destroyed by the *owner* of 'main'.
// An instance of the manager should be created during the initialization of
// an application (while the task has a single thread) and destroyed just
// prior to termination (when there is similarly a single thread).
//..
//  int main(int argc, char *argv[])
//  {
        // ...

        balm::DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Next we create a metric, "avgElapsedTimeMs", that will output the average
// time, in milliseconds, spent in a section of code.  We setting the
// preferred publication type for the metric to be average:
//..
        balm::ConfigurationUtil::setPreferredPublicationType(
                                                 "myCategory",
                                                 "avgElapsedTimeMs",
                                                 balm::PublicationType::e_AVG);
//..
// Next, because we will record the elapsed time in seconds, we configure a
// format to scale the elapsed time by 1000.0.
//..
       balm::ConfigurationUtil::setFormatSpec(
                                    "myCategory",
                                    "avgElapsedTimeMs",
                                    balm::PublicationType::e_AVG,
                                    balm::MetricFormatSpec(1000.0, "%.2f ms"));
//..
// We now collect some example an example value of .005:
//..
       BALM_METRICS_UPDATE("myCategory", "avgElapsedTimeMs", .005);
//..
// Finally, we publish the metric.  Note that in practice, clients of
// the 'balm' package can use the 'balm::PublicationScheduler' to schedule the
// periodic publication of metrics:
//..
    balm::DefaultMetricsManager::instance()->publishAll();
//..
// The output for the publication will look like:
//..
//  06AUG2009_20:27:51.982+0000 1 Records
//          Elapsed Time: 0.000816s
//                  myCategory.avgElapsedTimeMs[ avg (total/count) = 5.00 ms ]
//..
    } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING CLASS METHODS: setUserData (category)
        //
        // Concerns:
        //   That 'setUserData' ceates a sets a user data key using the
        //   correct metrics manager.
        //
        // Plan:
        //
        // Testing:
        //   static void setUserData(const char *,
        //                           balm::MetricDescription::UserDataKey  ,
        //                           const void *,
        //                           balm::MetricsManager *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "setUserDataKey (category)" << endl
                          << "=========================" << endl;

        if (veryVerbose) cout << "\tTesting primary bebavior" << endl;
        {
            MMgr mgr(Z); const MMgr& MGR = mgr;
            Key key0 = mgr.metricRegistry().createUserDataKey();
            Key key1 = mgr.metricRegistry().createUserDataKey();
            Key key2 = mgr.metricRegistry().createUserDataKey();
            Key key3 = mgr.metricRegistry().createUserDataKey();
            Key key4 = mgr.metricRegistry().createUserDataKey();


            balm::MetricRegistry& registry = mgr.metricRegistry();
            registry.addId("A",   "A");
            registry.addId("AA",  "A");
            registry.addId("AAA", "A");

            Obj::setUserData("A",   key0, (void *)1, &mgr);
            Obj::setUserData("AA",  key0, (void *)2, &mgr);
            Obj::setUserData("AAA", key0, (void *)3, &mgr);

            ASSERT((void *)1 ==
                   registry.getId("A", "A").description()->userData(key0))
            ASSERT((void *)2 ==
                   registry.getId("AA", "A").description()->userData(key0));
            ASSERT((void *)3 ==
                   registry.getId("AAA", "A").description()->userData(key0));

            // Test category prefixes.
            Obj::setUserData("*",    key1, (void *)1, &mgr);
            Obj::setUserData("A*",   key2, (void *)2, &mgr);
            Obj::setUserData("AA*",  key3, (void *)3, &mgr);
            Obj::setUserData("AAA*", key4, (void *)4, &mgr);

            ASSERT((void *)1 ==
                   registry.getId("A", "A").description()->userData(key1))
            ASSERT((void *)2 ==
                   registry.getId("A", "A").description()->userData(key2))
            ASSERT((void *)0 ==
                   registry.getId("A", "A").description()->userData(key3));
            ASSERT((void *)0 ==
                   registry.getId("A", "A").description()->userData(key4));

            ASSERT((void *)1 ==
                   registry.getId("AA", "A").description()->userData(key1))
            ASSERT((void *)2 ==
                   registry.getId("AA", "A").description()->userData(key2))
            ASSERT((void *)3 ==
                   registry.getId("AA", "A").description()->userData(key3));
            ASSERT((void *)0 ==
                   registry.getId("AA", "A").description()->userData(key4));

            ASSERT((void *)1 ==
                   registry.getId("AAA", "A").description()->userData(key1))
            ASSERT((void *)2 ==
                   registry.getId("AAA", "A").description()->userData(key2))
            ASSERT((void *)3 ==
                   registry.getId("AAA", "A").description()->userData(key3));
            ASSERT((void *)4 ==
                   registry.getId("AAA", "A").description()->userData(key4));


            // Test newly created categories matching the category prefixes.
            ASSERT((void *)1 ==
                   registry.getId("B", "B").description()->userData(key1))
            ASSERT((void *)0 ==
                   registry.getId("B", "B").description()->userData(key2))
            ASSERT((void *)0 ==
                   registry.getId("B", "B").description()->userData(key3));
            ASSERT((void *)0 ==
                   registry.getId("B", "B").description()->userData(key4));

            ASSERT((void *)1 ==
                   registry.getId("A", "B").description()->userData(key1))
            ASSERT((void *)2 ==
                   registry.getId("A", "B").description()->userData(key2))
            ASSERT((void *)0 ==
                   registry.getId("A", "B").description()->userData(key3));
            ASSERT((void *)0 ==
                   registry.getId("A", "B").description()->userData(key4));

            ASSERT((void *)1 ==
                   registry.getId("AA", "B").description()->userData(key1))
            ASSERT((void *)2 ==
                   registry.getId("AA", "B").description()->userData(key2))
            ASSERT((void *)3 ==
                   registry.getId("AA", "B").description()->userData(key3));
            ASSERT((void *)0 ==
                   registry.getId("AA", "B").description()->userData(key4));

            ASSERT((void *)1 ==
                   registry.getId("AAA", "B").description()->userData(key1))
            ASSERT((void *)2 ==
                   registry.getId("AAA", "B").description()->userData(key2))
            ASSERT((void *)3 ==
                   registry.getId("AAA", "B").description()->userData(key3));
            ASSERT((void *)4 ==
                   registry.getId("AAA", "B").description()->userData(key4));

        }
        if (veryVerbose) cout << "\tTesting w/o metrics manager" << endl;
        {
            ASSERT(0 == balm::DefaultMetricsManager::instance());
            Key key1 = Obj::createUserDataKey();
            Obj::setUserData("A", key1, (void *)1);
        }

        if (veryVerbose) cout << "\tTesting w/ default metrics manager"
                              << endl;
        {
            balm::DefaultMetricsManagerScopedGuard guard(Z);
            MMgr& mgr = *balm::DefaultMetricsManager::instance();

            Key key0 = mgr.metricRegistry().createUserDataKey();
            Key key1 = mgr.metricRegistry().createUserDataKey();
            Key key2 = mgr.metricRegistry().createUserDataKey();
            Key key3 = mgr.metricRegistry().createUserDataKey();
            Key key4 = mgr.metricRegistry().createUserDataKey();

            balm::MetricRegistry& registry = mgr.metricRegistry();
            registry.addId("A",   "A");
            registry.addId("AA",  "A");
            registry.addId("AAA", "A");

            Obj::setUserData("A",   key0, (void *)1, &mgr);
            Obj::setUserData("AA",  key0, (void *)2, &mgr);
            Obj::setUserData("AAA", key0, (void *)3, &mgr);

            ASSERT((void *)1 ==
                   registry.getId("A", "A").description()->userData(key0))
            ASSERT((void *)2 ==
                   registry.getId("AA", "A").description()->userData(key0));
            ASSERT((void *)3 ==
                   registry.getId("AAA", "A").description()->userData(key0));

            // Test category prefixes.
            Obj::setUserData("*",    key1, (void *)1, &mgr);
            Obj::setUserData("A*",   key2, (void *)2, &mgr);
            Obj::setUserData("AA*",  key3, (void *)3, &mgr);
            Obj::setUserData("AAA*", key4, (void *)4, &mgr);

            ASSERT((void *)1 ==
                   registry.getId("A", "A").description()->userData(key1))
            ASSERT((void *)2 ==
                   registry.getId("A", "A").description()->userData(key2))
            ASSERT((void *)0 ==
                   registry.getId("A", "A").description()->userData(key3));
            ASSERT((void *)0 ==
                   registry.getId("A", "A").description()->userData(key4));

            ASSERT((void *)1 ==
                   registry.getId("AA", "A").description()->userData(key1))
            ASSERT((void *)2 ==
                   registry.getId("AA", "A").description()->userData(key2))
            ASSERT((void *)3 ==
                   registry.getId("AA", "A").description()->userData(key3));
            ASSERT((void *)0 ==
                   registry.getId("AA", "A").description()->userData(key4));

            ASSERT((void *)1 ==
                   registry.getId("AAA", "A").description()->userData(key1))
            ASSERT((void *)2 ==
                   registry.getId("AAA", "A").description()->userData(key2))
            ASSERT((void *)3 ==
                   registry.getId("AAA", "A").description()->userData(key3));
            ASSERT((void *)4 ==
                   registry.getId("AAA", "A").description()->userData(key4));


            // Test newly created categories matching the category prefixes.
            ASSERT((void *)1 ==
                   registry.getId("B", "B").description()->userData(key1))
            ASSERT((void *)0 ==
                   registry.getId("B", "B").description()->userData(key2))
            ASSERT((void *)0 ==
                   registry.getId("B", "B").description()->userData(key3));
            ASSERT((void *)0 ==
                   registry.getId("B", "B").description()->userData(key4));

            ASSERT((void *)1 ==
                   registry.getId("A", "B").description()->userData(key1))
            ASSERT((void *)2 ==
                   registry.getId("A", "B").description()->userData(key2))
            ASSERT((void *)0 ==
                   registry.getId("A", "B").description()->userData(key3));
            ASSERT((void *)0 ==
                   registry.getId("A", "B").description()->userData(key4));

            ASSERT((void *)1 ==
                   registry.getId("AA", "B").description()->userData(key1))
            ASSERT((void *)2 ==
                   registry.getId("AA", "B").description()->userData(key2))
            ASSERT((void *)3 ==
                   registry.getId("AA", "B").description()->userData(key3));
            ASSERT((void *)0 ==
                   registry.getId("AA", "B").description()->userData(key4));

            ASSERT((void *)1 ==
                   registry.getId("AAA", "B").description()->userData(key1))
            ASSERT((void *)2 ==
                   registry.getId("AAA", "B").description()->userData(key2))
            ASSERT((void *)3 ==
                   registry.getId("AAA", "B").description()->userData(key3));
            ASSERT((void *)4 ==
                   registry.getId("AAA", "B").description()->userData(key4));

        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING CLASS METHODS: setUserData (metric)
        //
        // Concerns:
        //   That 'setUserData' sets a user data key using the
        //   correct metrics manager, and creates a new metric id if necessary.
        //
        // Plan:
        //
        // Testing:
        //   static void setUserData(const char *,
        //                           const char *,
        //                           balm::MetricDescription::UserDataKey  ,
        //                           const void *,
        //                           balm::MetricsManager *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "setUserDataKey (metric)" << endl
                          << "=======================" << endl;

        if (veryVerbose) cout << "\tTesting primary bebavior" << endl;
        {
            MMgr mgr(Z); const MMgr& MGR = mgr;
            Key key0 = mgr.metricRegistry().createUserDataKey();
            Key key1 = mgr.metricRegistry().createUserDataKey();

            balm::MetricRegistry& registry = mgr.metricRegistry();
            registry.addId("A", "A");

            Obj::setUserData("A", "A", key0, (void *)1, &mgr);

            ASSERT((void *)1 ==
                   registry.getId("A", "A").description()->userData(key0));

            // Create a new id.
            Obj::setUserData("A", "B", key0, (void *)1, &mgr);

            ASSERT(registry.findId("A","B").isValid());
            ASSERT((void *)1 ==
                   registry.getId("A", "B").description()->userData(key0));

        }

        if (veryVerbose) cout << "\tTesting w/o metrics manager" << endl;
        {
            ASSERT(0 == balm::DefaultMetricsManager::instance());
            Key key1 = Obj::createUserDataKey();
            Obj::setUserData("A", "A", key1, (void *)1);
        }

        if (veryVerbose) cout << "\tTesting w/ default metrics manager"
                              << endl;
        {
            balm::DefaultMetricsManagerScopedGuard guard(Z);
            MMgr& mgr = *balm::DefaultMetricsManager::instance();

            Key key0 = mgr.metricRegistry().createUserDataKey();
            Key key1 = mgr.metricRegistry().createUserDataKey();

            balm::MetricRegistry& registry = mgr.metricRegistry();
            registry.addId("A", "A");

            Obj::setUserData("A", "A", key0, (void *)1);

            ASSERT((void *)1 ==
                   registry.getId("A", "A").description()->userData(key0));

            // Create a new id.
            Obj::setUserData("A", "B", key0, (void *)1);

            ASSERT(registry.findId("A","B").isValid());
            ASSERT((void *)1 ==
                   registry.getId("A", "B").description()->userData(key0));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CLASS METHODS: createUserDataKey
        //
        // Concerns:
        //   That 'createUserDatayKey' creates a user data key using the
        //   correct metrics manager.
        //
        // Plan:
        //
        // Testing:
        //   createUserDataKey(balm::MetricsManager *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "createUserDataKey" << endl
                          << "=================" << endl;

        if (veryVerbose) cout << "\tTesting primary bebavior" << endl;
        {
            MMgr mgr(Z); const MMgr& MGR = mgr;
            Key key0 = mgr.metricRegistry().createUserDataKey();
            Key key1 = Obj::createUserDataKey(&mgr);
            Key key2 = mgr.metricRegistry().createUserDataKey();
            Key key3 = Obj::createUserDataKey(&mgr);

            ASSERT(key0 + 1 == key1);
            ASSERT(key1 + 1 == key2);
            ASSERT(key2 + 1 == key3);
        }

        if (veryVerbose) cout << "\tTesting w/o metrics manager" << endl;
        {
            ASSERT(0 == balm::DefaultMetricsManager::instance());
            Key key1 = Obj::createUserDataKey();
        }

        if (veryVerbose) cout << "\tTesting w/ default metrics manager"
                              << endl;
        {
            balm::DefaultMetricsManagerScopedGuard guard(Z);
            MMgr& mgr = *balm::DefaultMetricsManager::instance();

            Key key0 = mgr.metricRegistry().createUserDataKey();
            Key key1 = Obj::createUserDataKey();
            Key key2 = mgr.metricRegistry().createUserDataKey();
            Key key3 = Obj::createUserDataKey();

            ASSERT(key0 + 1 == key1);
            ASSERT(key1 + 1 == key2);
            ASSERT(key2 + 1 == key3);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CLASS METHODS: setFormat
        //
        // Concerns:
        //   That 'setFormatSpec' sets the format on the correct metrics
        //   manager.
        //
        // Plan:
        //
        // Testing:
        //    static int setFormatSpec(
        //                     const char                   *category,
        //                     const char                   *metricName,
        //                     balm::PublicationType::Value   publicationType,
        //                     const balm::MetricFormatSpec&  formatSpec,
        //                     balm::MetricsManager          *manager = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "setFormat" << endl
                          << "=========" << endl;

        const char *IDS[] = {"A", "B", "AB", "TEST", "Cx"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;

        Spec SPEC1(1, "%d");
        Spec SPEC2(2, "%d");
        Spec SPEC3(3, "%d");
        if (veryVerbose) cout << "\tTesting primary bebavior" << endl;
        {
            MMgr mgr(Z); const MMgr& MGR = mgr;
            for (int i = 0; i < NUM_IDS; ++i) {

                // Initialized 'e_UNSPECIFIED' publication type to SPEC1
                ASSERT(0 == Obj::setFormatSpec(IDS[i],
                                               IDS[i],
                                               (Type::Value)0,
                                               SPEC1,
                                               &mgr));

                // Verify the current format value.
                ASSERT(SPEC1 ==
                       *MGR.metricRegistry().findId(IDS[i], IDS[i]).
                       description()->format()->formatSpec(
                           (Type::Value)0));
                for (int j = 1; j < NUM_TYPES; ++j) {
                    ASSERT(0 == MGR.metricRegistry().findId(IDS[i], IDS[i]).
                                   description()->format()->formatSpec(
                                       (Type::Value)j));
                }

                // Initialize all publication types to SPEC2
                for (int j = 0; j < NUM_TYPES; ++j) {
                    ASSERT(0 == Obj::setFormatSpec(IDS[i],
                                                   IDS[i],
                                                   (Type::Value)j,
                                                   SPEC2,
                                                   &mgr));
                }
                // Verify the current format value.
                for (int j = 0; j < NUM_TYPES; ++j) {
                    ASSERT(SPEC2 ==
                           *MGR.metricRegistry().findId(IDS[i], IDS[i]).
                           description()->format()->formatSpec(
                               (Type::Value)j));
                }

                // Initialize all publication types to SPEC3.
                for (int j = 0; j < NUM_TYPES; ++j) {
                    ASSERT(0 == Obj::setFormatSpec(IDS[i],
                                                   IDS[i],
                                                   (Type::Value)j,
                                                   SPEC3,
                                                   &mgr));
                    // Verify the current format value.
                    for (int k = 0; k < NUM_TYPES; ++k) {
                        Spec EXPECTED = k <= j ? SPEC3 : SPEC2;
                        ASSERT(EXPECTED ==
                               *MGR.metricRegistry().findId(IDS[i], IDS[i]).
                               description()->format()->formatSpec(
                                   (Type::Value)k));
                    }
                }


            }
        }

        if (veryVerbose) cout << "\tTesting w/o metrics manager" << endl;
        {
            ASSERT(0 == balm::DefaultMetricsManager::instance());
            for (int i = 0; i < NUM_IDS; ++i) {
                ASSERT(0 != Obj::setFormatSpec(IDS[i],
                                               IDS[i],
                                               Type::e_MIN,
                                               SPEC1,
                                               0));
                ASSERT(0 != Obj::setFormatSpec(IDS[i],
                                               IDS[i],
                                               Type::e_MIN,
                                               SPEC1));
            }
        }

        if (veryVerbose) cout << "\tTesting w/ default metrics manager"
                              << endl;
        {
            balm::DefaultMetricsManagerScopedGuard guard(Z);
            const MMgr *MGR = balm::DefaultMetricsManager::instance();
            // Initialized 'e_UNSPECIFIED' publication type to SPEC1
            ASSERT(0 == Obj::setFormatSpec(IDS[0],
                                           IDS[0],
                                           (Type::Value)0,
                                           SPEC1));

            // Verify the current format value.
            ASSERT(SPEC1 ==
                   *MGR->metricRegistry().findId(IDS[0], IDS[0]).
                       description()->format()->formatSpec(
                           (Type::Value)0));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CLASS METHODS: setFormat
        //
        // Concerns:
        //   That 'setFormat' sets the format on the correct metrics manager.
        //
        // Plan:
        //
        // Testing:
        //   static int setFormat(const char               *category,
        //                        const char               *metricName,
        //                        const balm::MetricFormat&  format,
        //                        balm::MetricsManager      *manager = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "setFormat" << endl
                          << "=========" << endl;


        const char *IDS[] = {"A", "B", "AB", "TEST", "Cx"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;

        if (veryVerbose) cout << "\tTesting primary bebavior" << endl;
        {
            MMgr mgr(Z); const MMgr& MGR = mgr;
            for (int i = 0; i < NUM_FORMATS; ++i) {
                for (int j = 0; j < NUM_IDS; ++j) {
                    ASSERT(0 == Obj::setFormat(
                               IDS[j], IDS[j], *FORMATS[i], &mgr));

                    ASSERT(*FORMATS[i] ==
                           *MGR.metricRegistry().findId(IDS[j], IDS[j]).
                                    description()->format());
                }
            }
        }

        if (veryVerbose) cout << "\tTesting w/o metrics manager" << endl;
        {
            ASSERT(0 == balm::DefaultMetricsManager::instance());
            for (int i = 0; i < NUM_IDS; ++i) {
                ASSERT(0 != Obj::setFormat(IDS[i], IDS[i], *FORMATS[0], 0));
                ASSERT(0 != Obj::setFormat(IDS[i], IDS[i], *FORMATS[0]));
            }
        }

        if (veryVerbose) cout << "\tTesting w/ default metrics manager"
                              << endl;
        {
            balm::DefaultMetricsManagerScopedGuard guard(Z);
            const MMgr *MGR = balm::DefaultMetricsManager::instance();
            for (int i = 0; i < NUM_FORMATS; ++i) {
                for (int j = 0; j < NUM_IDS; ++j) {
                    ASSERT(0 == Obj::setFormat(
                               IDS[j], IDS[j], *FORMATS[i]));

                    ASSERT(*FORMATS[i] ==
                           *MGR->metricRegistry().findId(IDS[j], IDS[j]).
                                    description()->format());
                }
            }

        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING CLASS METHODS: setPreferredPublicationType
        //
        // Concerns:
        //   That 'setPreferredPublicationType' sets the preferred publication
        //   type on the correct metrics manager.
        //
        // Plan:
        //
        // Testing:
        // static int setPreferredPublicationType(
        //                     const char                  *,
        //                     const char                  *,
        //                     balm::PublicationType::Value  ,
        //                     balm::MetricsManager         *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "setPreferredPublicationType" << endl
                          << "===========================" << endl;


        const char *IDS[] = {"A", "B", "AB", "TEST", "Cx"};
        const int NUM_IDS = sizeof IDS / sizeof *IDS;


        if (veryVerbose) cout << "\tTesting primary bebavior" << endl;
        {
            MMgr mgr(Z); const MMgr& MGR = mgr;
            for (int i = 0; i < NUM_TYPES; ++i) {
                for (int j = 0; j < NUM_IDS; ++j) {
                    ASSERT(0 == Obj::setPreferredPublicationType(
                               IDS[j], IDS[j],(Type::Value)i, &mgr));

                    ASSERT((Type::Value)i ==
                           MGR.metricRegistry().findId(IDS[j], IDS[j]).
                                    description()->preferredPublicationType());
                }
            }
        }

        if (veryVerbose) cout << "\tTesting w/o metrics manager" << endl;
        {
            ASSERT(0 == balm::DefaultMetricsManager::instance());
            for (int i = 0; i < NUM_IDS; ++i) {
                ASSERT(0 != Obj::setPreferredPublicationType(
                           IDS[i], IDS[i],Type::e_MIN, 0));

                ASSERT(0 != Obj::setPreferredPublicationType(
                           IDS[i], IDS[i],Type::e_MIN));
            }
        }

        if (veryVerbose) cout << "\tTesting w/ default metrics manager"
                              << endl;
        {
            balm::DefaultMetricsManagerScopedGuard guard(Z);
            const MMgr *MGR = balm::DefaultMetricsManager::instance();
            for (int i = 0; i < NUM_TYPES; ++i) {
                for (int j = 0; j < NUM_IDS; ++j) {
                    ASSERT(0 == Obj::setPreferredPublicationType(
                               IDS[j], IDS[j],(Type::Value)i));

                    ASSERT((Type::Value)i ==
                           MGR->metricRegistry().findId(IDS[j], IDS[j]).
                                    description()->preferredPublicationType());
                }
            }

        }


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
