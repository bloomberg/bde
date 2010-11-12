// baem_stopwatchscopedguard.t.cpp  -*-C++-*-
#include <baem_stopwatchscopedguard.h>

#include <baem_metricsample.h>
#include <baem_publisher.h>

#include <bcema_testallocator.h>
#include <bcemt_barrier.h>
#include <bcep_fixedthreadpool.h>

#include <bdef_bind.h>
#include <bdetu_date.h>

#include <bslma_allocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bsls_assert.h>
#include <bsls_stopwatch.h>

#include <bsl_ostream.h>
#include <bsl_cstring.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_cstring.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The 'baem_StopwatchScopedGuard' provides a mechanism and macros for
// recording the elapsed time of a block of code.  The class provides several
// constructor variants, but no manipulator methods, and single accessor, so
// there are relatively few tests.  All the tests, except the
// 'ELAPSED TIME VALUE' test, ensure that the correct metric is updated,
// the 'ELAPSED TIME VALUE' test verifies that the class records a reasonable
// elapsed time to that metric.
//-----------------------------------------------------------------------------
// CREATORS
// [ 4]  explicit baem_StopwatchScopedGuard(baem_Metric *metric);
// [ 3]  explicit baem_StopwatchScopedGuard(baem_Collector *collector);
// [ 4]  baem_StopwatchScopedGuard(const baem_MetricId&  ,
//                                 baem_MetricsManager  * = 0);
// [ 4]  baem_StopwatchScopedGuard(const char * ,
//                                 const char *  ,
//                                 baem_MetricsManager    *);
// [ 3]  ~baem_StopwatchScopedGuard();
// ACCESSORS
// [ 3]  bool isActive() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] 'TestPublisher'                             (helper classes)
// [ 3] TESTING REPORTED TIME UNITS
// [ 6] ELAPSED TIME VALUE
// [ 8] USAGE

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

typedef baem_StopwatchScopedGuard       Obj;
typedef Obj::Units                      Units;
typedef baem_MetricsManager             MetricsManager;
typedef baem_DefaultMetricsManager      DefaultManager;
typedef baem_MetricRegistry             Registry;
typedef baem_CollectorRepository        Repository;
typedef baem_Collector                  Collector;
typedef baem_MetricId                   Id;
typedef baem_Category                   Category;
typedef baem_Metric                     Metric;
typedef bcema_SharedPtr<baem_Publisher> PublisherPtr;

//=============================================================================
//                      CLASSES FOR TESTING
//-----------------------------------------------------------------------------

bool within(double value, Units scale, double expectedS, double windowMs)
    // Return 'true' if the specified 'value' in the specified 'scale' is
    // within the specified 'windowMs' (milliseconds) of the specified
    // 'expectedS' (seconds).
{
    double expected = expectedS * scale;
    double window   = windowMs *1000 * scale;

    return ((expected - window) < value)
        && ((expected + window) > value);
}

bool recordLess(const baem_MetricRecord& lhs, const baem_MetricRecord& rhs)
    // Return 'true' if the specified 'lhs' is less than (ordered before) the
    // specified 'rhs', and 'false' otherwise.  A 'baem_MetricRecord' are
    // ordered by their the 'metricId' method.
{

    return lhs.metricId() < rhs.metricId();
}

baem_MetricRecord recordValue(baem_Collector *collector)
    // Return the current value of the metric record being collected by the
    // specified 'collector'.
{
    baem_MetricRecord record;
    collector->load(&record);
    return record;
}

                      // ===================
                      // class TestPublisher
                      // ===================

class TestPublisher : public baem_Publisher {
    // This class defines a test implementation of the 'baem_Publisher' that
    // protocol can be used to record information about invocations of the
    // 'publish' method.  Each 'TestPublisher' instance tracks the number of
    // times 'publish' has been called, and maintains 'lastElapsedTime()',
    // 'lastTimeStamp()', and 'lastRecords()' values holding the elapsed time,
    // time stamp, and record values (in sorted order) of the last
    // published 'baem_MetricSample' object.  The 'TestPublisher' also
    // provides a 'reset()' operation to reset the invocation count to 0 and
    // clear the 'lastRecords' information.  Note that the 'publish' method is
    // *not* thread-safe.

    // DATA
    bces_AtomicInt                 d_numInvocations;  // # of invocations

    bsl::vector<baem_MetricRecord> d_recordBuffer;    // last samples records

    bsl::vector<baem_MetricRecord> d_sortedRecords;   // last sample's records
                                                      // in sorted order

    baem_MetricSample              d_sample;          // reconstructed last
                                                      // sample (using
                                                      // 'd_recordsBuffer')

    bsl::set<bdet_TimeInterval>    d_elapsedTimes;    // last elapsed times

    // NOT IMPLEMENTED
    TestPublisher(const TestPublisher& );
    TestPublisher& operator=(const TestPublisher& );

  public:

    // CREATORS
    TestPublisher(bslma_Allocator *allocator);
        // Create a test publisher with 0 'invocations()' and the default
        // constructed 'lastSample()' using the specified 'allocator' to
        // supply memory.

    virtual ~TestPublisher();
        // Destroy this test publisher.

    // MANIPULATORS
    virtual void publish(const baem_MetricSample& sample);
        // Increment the number of 'invocations()', set the
        // 'lastElapsedTime()' and 'lastTimeStamp()' equal to the elapsed time
        // and time stamp of the specified 'sample', and set 'lastRecords()'
        // to be the list of sequence in 'sample' in *sorted* order.   Note
        // that this method is *not* thread-safe.

   void reset();
        // Set 'invocations()' to 0, clear the 'lastRecords()' sequence.

    // ACCESSORS
    int invocations() const;
        // Return the number of times the 'publish' method has been invoked
        // since this test publisher was constructed or the last call to
        // 'reset()'.

    const baem_MetricSample& lastSample() const;
        // Return a reference to the non-modifiable reconstruction of the last
        // sample passed to the 'publish' method.  The returned sample value
        // contains the same metric record values organized into the same
        // groups as the published sample, but the returned sample does not
        // refer to the the same addresses in memory: so the returned sample is
        // equivalent but *not* *equal* to the published sample.

    const bsl::vector<baem_MetricRecord>& lastRecords() const;
        // Return a reference to the non-modifiable sequence of records
        // containing the values of the records in the last sample passed to
        // the 'publish' method in *sorted* order, or an empty vector if the
        // 'publish' method has not been called since this object was created
        // or last reset.

    const bsl::set<bdet_TimeInterval>& lastElapsedTimes() const;
        // Return a reference to the non-modifiable elapsed time value of the
        // last sample passed to the 'publish' method.  The behavior is
        // undefined unless the 'publish' method has invoked since this object
        // was created or last reset.

    const bdet_DatetimeTz& lastTimeStamp() const;
        // Return a reference to the non-modifiable time stamp of the
        // last sample passed to the 'publish' method.  The behavior is
        // undefined unless the 'publish' method has invoked since this object
        // was created or last reset.

    int indexOf(const baem_MetricId& id) const;
        // Return the index into 'lastRecords()' of the specified 'id', or -1
        // if there is no record with 'id' in 'lastRecords()'.

    bool contains(const baem_MetricId& id) const;
        // Return 'true' if the 'lastSample' contains a 'baem_MetricRecord'
        // object whose 'metricId()' equals the specified 'id', and 'false'
        // otherwise.  Note that this operation is logical equivalent to
        // 'index(id) != -1'.

};

                      // -------------------
                      // class TestPublisher
                      // -------------------

// CREATORS
inline
TestPublisher::TestPublisher(bslma_Allocator *allocator)
: d_numInvocations(0)
, d_recordBuffer(allocator)
, d_sortedRecords(allocator)
, d_sample(allocator)
, d_elapsedTimes(allocator)
{
}

inline
TestPublisher::~TestPublisher()
{
}

// MANIPULATORS
void TestPublisher::publish(const baem_MetricSample& sample)
{
    d_numInvocations++;

    d_sample.removeAllRecords();
    d_recordBuffer.clear();
    d_sortedRecords.clear();
    d_elapsedTimes.clear();

    d_sample.setTimeStamp(sample.timeStamp());
    if (0 == sample.numRecords()) {
        return;
    }

    // We *must* reserve memory to avoid re-allocating data while building
    // the sample
    d_recordBuffer.reserve(sample.numRecords());
    baem_MetricSample::const_iterator sIt = sample.begin();
    for (; sIt != sample.end(); ++sIt) {
        baem_MetricSampleGroup::const_iterator gIt = sIt->begin();
        BSLS_ASSERT(gIt != sIt->end());
        d_recordBuffer.push_back(*gIt);
        baem_MetricRecord *head = &d_recordBuffer.back();
        for (++gIt; gIt != sIt->end(); ++gIt) {
            d_recordBuffer.push_back(*gIt);
        }
        d_sample.appendGroup(head, sIt->numRecords(), sIt->elapsedTime());
        d_elapsedTimes.insert(sIt->elapsedTime());
    }
    d_sortedRecords = d_recordBuffer;
    bsl::sort(d_sortedRecords.begin(), d_sortedRecords.end(), recordLess);
}

void TestPublisher::reset()
{
    d_numInvocations = 0;
    d_sample.removeAllRecords();
    d_recordBuffer.clear();
    d_sortedRecords.clear();
    d_elapsedTimes.clear();

}

// ACCESSORS
inline
int TestPublisher::invocations() const
{
    return d_numInvocations;
}

inline
const baem_MetricSample& TestPublisher::lastSample() const
{
    return d_sample;
}

inline
const bsl::set<bdet_TimeInterval>& TestPublisher::lastElapsedTimes() const
{
    return d_elapsedTimes;
}

inline
const bdet_DatetimeTz& TestPublisher::lastTimeStamp() const
{
    return d_sample.timeStamp();
}

inline
const bsl::vector<baem_MetricRecord>& TestPublisher::lastRecords() const
{
    return d_sortedRecords;
}

inline
int TestPublisher::indexOf(const baem_MetricId& id) const
{
    baem_MetricRecord searchRecord(id);
    bsl::vector<baem_MetricRecord>::const_iterator it =
                                   bsl::lower_bound(d_sortedRecords.begin(),
                                                    d_sortedRecords.end(),
                                                    searchRecord, recordLess);
    if (it == d_sortedRecords.end()) {
        return -1;
    }
    return (it->metricId() == id) ? it - d_sortedRecords.begin() : -1;
}

inline
bool TestPublisher::contains(const baem_MetricId& id) const
{
    return indexOf(id) != -1;
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

//..
///Example 2 - Metric collection with 'baem_StopwatchScopedGuard'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We can alternatively use the 'baem_StopwatchScopedGuard' to record metric
// values.  In the following example we implement a hypothetical request
// processor similar to example 3.  We use 'baem_Metric' and a
// 'baem_StopwatchScopedGuard' to record the elapsed time of the request
// processing function.
//..
    class RequestProcessor {

        // DATA
        baem_Metric d_elapsedTime;

      public:

        // CREATORS
        RequestProcessor()
        : d_elapsedTime("TestCategory", "RequestProcessor/elapsedTime")
        {}

        // MANIPULATORS
        int processRequest(const bsl::string& request)
            // Process the specified 'request'.  Return 0 on success, and a
            // non-zero value otherwise.
        {
           int returnCode = 0;

           baem_StopwatchScopedGuard guard(&d_elapsedTime);

    // ...

           return returnCode;
        }

    // ...
    };
//..

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

    bslma_TestAllocator testAlloc; bslma_TestAllocator *Z = &testAlloc;
    bslma_TestAllocator defaultAllocator;
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 9:{
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

///Example 1 - Create and configure the default 'baem_MetricsManager' instance
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create the default 'baem_MetricManager'
// instance and perform a trivial configuration.
//
// First we create a 'baem_DefaultMetricsManagerScopedGuard', which manages the
// lifetime of the default metrics manager instance.  At construction, we
// provide the scoped guard an output stream ('stdout') that it will publish
// metrics to.  Note that the default metrics manager is intended to be
// created and destroyed by the *owner* of 'main'.  An instance of the manager
// should be created during the initialization of an application (while the
// task has a single thread) and destroyed just prior to termination (when
// there is similarly a single thread).
//..
//  int main(int argc, char *argv[])
    {

    // ...

        baem_DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Once the default instance has been created, it can be accessed using the
// 'instance' operation.
//..
        baem_MetricsManager *manager  = baem_DefaultMetricsManager::instance();
                        ASSERT(0       != manager);
//..
// Note that the default metrics manager will be released when 'managerGuard'
// exits this scoped and is destroyed.  Clients that choose to explicitly call
// the 'baem_DefaultMetricsManager::create' method must also explicitly call
// the 'baem_DefaultMetricsManager::release' method.

        RequestProcessor processor;

        processor.processRequest("ab");
        processor.processRequest("abc");
        processor.processRequest("abc");
        processor.processRequest("abdef");

        manager->publishAll();

        processor.processRequest("ab");
        processor.processRequest("abc");
        processor.processRequest("abc");
        processor.processRequest("abdef");

        processor.processRequest("a");
        processor.processRequest("abc");
        processor.processRequest("abc");
        processor.processRequest("abdefg");

        manager->publishAll();

    }
        ASSERT(0 == baem_DefaultMetricsManager::instance());
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ELAPSED TIME VALUE:
        //
        // Concerns:
        //    That the value recorded by the guard is (roughly) the elapsed
        //    time between the objects construction and destruction.
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ELAPSED TIME VALUE\n"
                          << "==================\n";

        MetricsManager  manager(Z);
        Repository&     repository = manager.collectorRepository();
        baem_Collector *collector = repository.getDefaultCollector("A", "1");
        bsls_Stopwatch  stopwatch;

        enum { COUNT = 10 };

        double ms = 1.0 * .001;

        double expectedTotal = 0;
        double expectedMin   = 500;
        double expectedMax   = 0;

        for (int i = 0; i < COUNT; ++i) {
            stopwatch.start();

            Obj mX(collector);
            bcemt_ThreadUtil::sleep(bdet_TimeInterval(50 * ms));

            stopwatch.stop();
            expectedTotal += stopwatch.elapsedTime();
            if (stopwatch.elapsedTime() < expectedMin) {
                expectedMin = stopwatch.elapsedTime();
            }
            if (stopwatch.elapsedTime() > expectedMax) {
                expectedMax = stopwatch.elapsedTime();
            }
        }

        baem_MetricRecord record = recordValue(collector);
        ASSERT(COUNT == record.count());
        ASSERT(within(record.total(), Obj::BAEM_SECONDS, expectedTotal, 1.0))
        ASSERT(within(record.max(), Obj::BAEM_SECONDS, expectedMax, 1.0))
        ASSERT(within(record.min(), Obj::BAEM_SECONDS, expectedMin, 1.0))

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING REPORTED TIME UNITS:
        //
        // Concerns:
        //    That the value reported by the 'baem_StopwatchScopedGuard' is
        //    scaled into the appropriate time units.
        //
        // Plan:
        //
        // Testing:
        //   explicit baem_StopwatchScopedGuard(baem_Metric *, Units );
        //   baem_StopwatchScopedGuard(const baem_MetricId&  ,
        //                             Units ,
        //                             baem_MetricsManager  *);
        //   baem_StopwatchScopedGuard(const char *  ,
        //                             const char *  ,
        //                             Units  ,
        //                             baem_MetricsManager    * );
        //   baem_StopwatchScopedGuard(baem_Collector *, Units);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "ScopedGuard REPORTED TIME UNITS\n"
                          << "===============================\n";

        {
            baem_DefaultMetricsManagerScopedGuard guard(Z);
            MetricsManager& manager = *baem_DefaultMetricsManager::instance();
            Repository&    repository = manager.collectorRepository();

            // Create one version for every time unit, and one for the default.
            baem_Collector *a_D  = repository.getDefaultCollector("A", "D");
            baem_Collector *a_S  = repository.getDefaultCollector("A", "S");
            baem_Collector *a_Ms = repository.getDefaultCollector("A", "Ms");
            baem_Collector *a_Us = repository.getDefaultCollector("A", "Us");
            baem_Collector *a_Ns = repository.getDefaultCollector("A", "Ns");

            baem_Collector *b_D  = repository.getDefaultCollector("B", "D");
            baem_Collector *b_S  = repository.getDefaultCollector("B", "S");
            baem_Collector *b_Ms = repository.getDefaultCollector("B", "Ms");
            baem_Collector *b_Us = repository.getDefaultCollector("B", "Us");
            baem_Collector *b_Ns = repository.getDefaultCollector("B", "Ns");

            baem_Collector *c_D  = repository.getDefaultCollector("C", "D");
            baem_Collector *c_S  = repository.getDefaultCollector("C", "S");
            baem_Collector *c_Ms = repository.getDefaultCollector("C", "Ms");
            baem_Collector *c_Us = repository.getDefaultCollector("C", "Us");
            baem_Collector *c_Ns = repository.getDefaultCollector("C", "Ns");

            baem_Collector *d_D  = repository.getDefaultCollector("D", "D");
            baem_Collector *d_S  = repository.getDefaultCollector("D", "S");
            baem_Collector *d_Ms = repository.getDefaultCollector("D", "Ms");
            baem_Collector *d_Us = repository.getDefaultCollector("D", "Us");
            baem_Collector *d_Ns = repository.getDefaultCollector("D", "Ns");

            baem_MetricId idB_D  = manager.metricRegistry().getId("B", "D");
            baem_MetricId idB_S  = manager.metricRegistry().getId("B", "S");
            baem_MetricId idB_Ms = manager.metricRegistry().getId("B", "Ms");
            baem_MetricId idB_Us = manager.metricRegistry().getId("B", "Us");
            baem_MetricId idB_Ns = manager.metricRegistry().getId("B", "Ns");

            baem_Metric mC_D("C", "D");
            baem_Metric mC_S("C", "S");
            baem_Metric mC_Ms("C", "Ms");
            baem_Metric mC_Us("C", "Us");
            baem_Metric mC_Ns("C", "Ns");

            bsls_Stopwatch sw;
            sw.start();
            {
                baem_StopwatchScopedGuard gA_D(a_D);
                baem_StopwatchScopedGuard gA_S(a_S, Obj::BAEM_SECONDS);
                baem_StopwatchScopedGuard gA_Ms(a_Ms, Obj::BAEM_MILLISECONDS);
                baem_StopwatchScopedGuard gA_Us(a_Us, Obj::BAEM_MICROSECONDS);
                baem_StopwatchScopedGuard gA_Ns(a_Ns, Obj::BAEM_NANOSECONDS);

                baem_StopwatchScopedGuard gB_D(idB_D);
                baem_StopwatchScopedGuard gB_S(idB_S, Obj::BAEM_SECONDS);
                baem_StopwatchScopedGuard gB_Ms(idB_Ms,
                                                Obj::BAEM_MILLISECONDS);
                baem_StopwatchScopedGuard gB_Us(idB_Us,
                                                Obj::BAEM_MICROSECONDS);
                baem_StopwatchScopedGuard gB_Ns(idB_Ns, Obj::BAEM_NANOSECONDS);

                baem_StopwatchScopedGuard gC_D(&mC_D);
                baem_StopwatchScopedGuard gC_S(&mC_S, Obj::BAEM_SECONDS);
                baem_StopwatchScopedGuard gC_Ms(&mC_Ms,
                                                Obj::BAEM_MILLISECONDS);
                baem_StopwatchScopedGuard gC_Us(&mC_Us,
                                                Obj::BAEM_MICROSECONDS);
                baem_StopwatchScopedGuard gC_Ns(&mC_Ns, Obj::BAEM_NANOSECONDS);

                baem_StopwatchScopedGuard gD_D("D", "D");
                baem_StopwatchScopedGuard gD_S ("D", "S", Obj::BAEM_SECONDS);
                baem_StopwatchScopedGuard gD_Ms("D", "Ms",
                                                Obj::BAEM_MILLISECONDS);
                baem_StopwatchScopedGuard gD_Us("D", "Us",
                                                Obj::BAEM_MICROSECONDS);
                baem_StopwatchScopedGuard gD_Ns("D", "Ns",
                                                Obj::BAEM_NANOSECONDS);

                bcemt_ThreadUtil::sleep(bdet_TimeInterval(50 * .001));

                sw.stop();
            }
            double expected = sw.elapsedTime();
            baem_MetricRecord rA_D  = recordValue(a_D);
            baem_MetricRecord rA_S  = recordValue(a_S);
            baem_MetricRecord rA_Ms = recordValue(a_Ms);
            baem_MetricRecord rA_Us = recordValue(a_Us);
            baem_MetricRecord rA_Ns = recordValue(a_Ns);

            baem_MetricRecord rB_D  = recordValue(b_D);
            baem_MetricRecord rB_S  = recordValue(b_S);
            baem_MetricRecord rB_Ms = recordValue(b_Ms);
            baem_MetricRecord rB_Us = recordValue(b_Us);
            baem_MetricRecord rB_Ns = recordValue(b_Ns);

            baem_MetricRecord rC_D  = recordValue(c_D);
            baem_MetricRecord rC_S  = recordValue(c_S);
            baem_MetricRecord rC_Ms = recordValue(c_Ms);
            baem_MetricRecord rC_Us = recordValue(c_Us);
            baem_MetricRecord rC_Ns = recordValue(c_Ns);

            baem_MetricRecord rD_D  = recordValue(d_D);
            baem_MetricRecord rD_S  = recordValue(d_S);
            baem_MetricRecord rD_Ms = recordValue(d_Ms);
            baem_MetricRecord rD_Us = recordValue(d_Us);
            baem_MetricRecord rD_Ns = recordValue(d_Ns);

            ASSERT(within(rA_D.total(),  Obj::BAEM_SECONDS,
                                                               expected, 1.0));
            ASSERT(within(rA_S.total(),  Obj::BAEM_SECONDS,    expected, 1.0));
            ASSERT(within(rA_Ms.total(), Obj::BAEM_MILLISECONDS,
                                                               expected, 1.0));
            ASSERT(within(rA_Us.total(), Obj::BAEM_MICROSECONDS,
                                                               expected, 1.0));
            ASSERT(within(rA_Ns.total(), Obj::BAEM_NANOSECONDS,
                                                               expected, 1.0));

            ASSERT(within(rB_D.total(),  Obj::BAEM_SECONDS,    expected, 1.0));
            ASSERT(within(rB_S.total(),  Obj::BAEM_SECONDS,    expected, 1.0));
            ASSERT(within(rB_Ms.total(), Obj::BAEM_MILLISECONDS,
                                                               expected, 1.0));
            ASSERT(within(rB_Us.total(), Obj::BAEM_MICROSECONDS,
                                                               expected, 1.0));
            ASSERT(within(rB_Ns.total(), Obj::BAEM_NANOSECONDS,
                                                               expected, 1.0));

            ASSERT(within(rC_D.total(),  Obj::BAEM_SECONDS,    expected, 1.0));
            ASSERT(within(rC_S.total(),  Obj::BAEM_SECONDS,    expected, 1.0));
            ASSERT(within(rC_Ms.total(), Obj::BAEM_MILLISECONDS,
                                                               expected, 1.0));
            ASSERT(within(rC_Us.total(), Obj::BAEM_MICROSECONDS,
                                                               expected, 1.0));
            ASSERT(within(rC_Ns.total(), Obj::BAEM_NANOSECONDS,
                                                               expected, 1.0));

            ASSERT(within(rD_D.total(),  Obj::BAEM_SECONDS,    expected, 1.0));
            ASSERT(within(rD_S.total(),  Obj::BAEM_SECONDS,    expected, 1.0));
            ASSERT(within(rD_Ms.total(), Obj::BAEM_MILLISECONDS,
                                                               expected, 1.0));
            ASSERT(within(rD_Us.total(), Obj::BAEM_MICROSECONDS,
                                                               expected, 1.0));
            ASSERT(within(rD_Ns.total(), Obj::BAEM_NANOSECONDS,
                                                               expected, 1.0));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ALTERNATIVE CONSTRUCTORS:
        //
        // Concerns:
        //    That the alternative constructors record metrics to the correct
        //    collectors.  That the default metrics manager is used a metrics
        //    manager is not explicitly supplied.  That the guard will be
        //    inactive if a metrics manager is not provided and the default
        //    metrics manager is not available.
        //
        // Plan:
        //
        // Testing:
        //   explicit baem_StopwatchScopedGuard(baem_Metric *);
        //   baem_StopwatchScopedGuard(const baem_MetricId&  ,
        //                             baem_MetricsManager  *);
        //   baem_StopwatchScopedGuard(const char *,
        //                             const char *,
        //                             baem_MetricsManager    *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST ALTERNATIVE CONSTRUCTORS\n"
                          << "=============================\n";

        {
            if (veryVerbose) cout << "\tTest with valid collector" << endl;

            MetricsManager manager(Z);
            Repository&    repository = manager.collectorRepository();
            baem_Collector *cA = repository.getDefaultCollector("A", "1");
            baem_Collector *cB = repository.getDefaultCollector("B", "2");
            baem_Collector *cC = repository.getDefaultCollector("C", "3");

            baem_MetricId  idA = manager.metricRegistry().getId("A","1");
            baem_MetricId  idB = manager.metricRegistry().getId("B","2");
            baem_MetricId  idC = manager.metricRegistry().getId("C","3");

            baem_Metric    metric(idA, &manager);
            {
                Obj mX(&metric);           const Obj& MX = mX;
                Obj mY(idB, &manager);     const Obj& MY = mY;
                Obj mZ("C","3", &manager); const Obj& MZ = mZ;

                ASSERT(MX.isActive());
                ASSERT(MY.isActive());
                ASSERT(MZ.isActive());

                bcemt_ThreadUtil::microSleep(5, 0);
            }

            baem_MetricRecord recordA = recordValue(cA);
            ASSERT(1 == recordA.count());
            LOOP_ASSERT(recordA.total(),
                        0.0 < recordA.total() && 1.0 > recordA.total());
            ASSERT(1.0 > recordA.min());
            ASSERT(1.0 > recordA.max());

            baem_MetricRecord recordB = recordValue(cB);
            ASSERT(1 == recordB.count());
            LOOP_ASSERT(recordB.total(),
                        0.0 < recordB.total() && 1.0 > recordB.total());
            ASSERT(1.0 > recordB.min());
            ASSERT(1.0 > recordB.max());

            baem_MetricRecord recordC = recordValue(cC);
            ASSERT(1 == recordC.count());
            LOOP_ASSERT(recordC.total(),
                        0.0 < recordC.total() && 1.0 > recordC.total());
            ASSERT(1.0 > recordC.min());
            ASSERT(1.0 > recordC.max());
        }

        {
            if (veryVerbose) cout << "\tTest with default metricsmanager"
                                  << endl;

            baem_DefaultMetricsManagerScopedGuard guard(Z);
            MetricsManager& manager = *baem_DefaultMetricsManager::instance();

            Repository&    repository = manager.collectorRepository();
            baem_Collector *cA = repository.getDefaultCollector("A", "1");
            baem_Collector *cB = repository.getDefaultCollector("B", "2");

            baem_MetricId  idA = manager.metricRegistry().getId("A","1");
            baem_MetricId  idB = manager.metricRegistry().getId("B","2");
            {
                Obj mY(idA);     const Obj& MY = mY;
                Obj mZ("B","2"); const Obj& MZ = mZ;

                ASSERT(MY.isActive());
                ASSERT(MZ.isActive());

                bcemt_ThreadUtil::microSleep(5, 0);
            }
            baem_MetricRecord recordA = recordValue(cA);
            ASSERT(1 == recordA.count());
            LOOP_ASSERT(recordA.total(),
                        0.0 < recordA.total() && 1.0 > recordA.total());
            ASSERT(1.0 > recordA.min());
            ASSERT(1.0 > recordA.max());

            baem_MetricRecord recordB = recordValue(cB);
            ASSERT(1 == recordB.count());
            LOOP_ASSERT(recordB.total(),
                        0.0 < recordB.total() && 1.0 > recordB.total());
            ASSERT(1.0 > recordB.min());
            ASSERT(1.0 > recordB.max());

        }

        {
            if (veryVerbose) cout << "\tTest with a no metrics manager\n" ;

            MetricsManager manager(Z);
            baem_MetricId  id = manager.metricRegistry().getId("A", "1");
            baem_Metric    metric(id);

            {
                Obj mX(&metric); const Obj& MX = mX;
                Obj mY(id);      const Obj& MY = mY;
                Obj mZ("A","A"); const Obj& MZ = mZ;

                ASSERT(!MX.isActive());
                ASSERT(!MY.isActive());
                ASSERT(!MZ.isActive());
            }
            baem_Collector *collector =
                   manager.collectorRepository().getDefaultCollector("A", "1");
            ASSERT(baem_MetricRecord(id) == recordValue(collector));
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 == testAlloc.numBytesInUse());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTOR & DESTRUCTOR (bootstrap):
        //
        // Concerns:
        //   That the stop watch scoped guard records the elapsed time of a
        //   block of code to the supplied collector, if enabled, when
        //   the scoped guard is destroyed.
        //
        // Plan:
        //
        // Testing:
        //   baem_StopwatchScopedGuard(baem_Collector *);
        //   ~baem_StopwatchScopedGuard();
        //   bool isActive();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST CONSTRUCTOR & DESTRUCTOR (BOOTSTRAP)\n"
                          << "=========================================\n";

        {
            if (veryVerbose) cout << "\tTest with null collector" << endl;
            baem_Collector *collector = 0;
            Obj mX(collector); const Obj& MX = mX;

            ASSERT(!MX.isActive());
        }

        {
            if (veryVerbose) cout << "\tTest with valid colletcor" << endl;

            MetricsManager manager(Z);
            baem_Collector *collector =
                   manager.collectorRepository().getDefaultCollector("A", "A");

            {
                Obj mX(collector);  const Obj& MX = mX;
                ASSERT(MX.isActive());
                ASSERT(baem_MetricRecord(collector->metricId()) ==
                       recordValue(collector));

                bcemt_ThreadUtil::microSleep(5, 0);
            }

            baem_MetricRecord record = recordValue(collector);
            ASSERT(1 == record.count());
            LOOP_ASSERT(record.total(),
                        0.0 < record.total() && 1.0 > record.total());
            ASSERT(1.0 > record.min());
            ASSERT(1.0 > record.max());

        }
        {
            if (veryVerbose) cout << "\tTest with disabled category" << endl;

            MetricsManager manager(Z);
            baem_Collector *collector =
                   manager.collectorRepository().getDefaultCollector("A", "A");
            const Category *CATEGORY = collector->metricId().category();

            // Test category always disabled
            manager.setCategoryEnabled(CATEGORY, false);
            {
                Obj mX(collector);  const Obj& MX = mX;
                ASSERT(!MX.isActive());
                ASSERT(baem_MetricRecord(collector->metricId()) ==
                       recordValue(collector));
            }
            ASSERT(baem_MetricRecord(collector->metricId()) ==
                   recordValue(collector));

            // Test category always disabled only at construction
            manager.setCategoryEnabled(CATEGORY, false);
            {
                Obj mX(collector);  const Obj& MX = mX;
                ASSERT(!MX.isActive());
                ASSERT(baem_MetricRecord(collector->metricId()) ==
                       recordValue(collector));
                manager.setCategoryEnabled(CATEGORY, true);
                ASSERT(!MX.isActive());
            }
            ASSERT(baem_MetricRecord(collector->metricId()) ==
                   recordValue(collector));

            // Test category always disabled only at destruction
            manager.setCategoryEnabled(CATEGORY, true);
            {
                Obj mX(collector);  const Obj& MX = mX;
                ASSERT(MX.isActive());
                ASSERT(baem_MetricRecord(collector->metricId()) ==
                       recordValue(collector));
                manager.setCategoryEnabled(CATEGORY, false);
                ASSERT(!MX.isActive());
            }
            ASSERT(baem_MetricRecord(collector->metricId()) ==
                   recordValue(collector));
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 == testAlloc.numBytesInUse());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING HELPERS: TestPublisher
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST HELPERS: TestPublisher\n"
                          << "===========================\n";

        {
            if (veryVerbose)
                cout << "\tTesting TestPublisher\n";

            baem_MetricRegistry registry(Z);
            struct {
                int         d_elapsedTime;
                int         d_timeStamp;
                const char *d_records;
            } VALUES[] =  {
                { 1, 19991101, "12" },
                { 2, 20071205, "4321" },
                { 5, 20040101, "23417" },
                { 4, 19960606, "3259" },
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            TestPublisher tp1(Z), tp2(Z);

            ASSERT(0 == tp1.invocations());
            ASSERT(0 == tp2.invocations());
            ASSERT(0 == tp1.lastRecords().size());
            ASSERT(0 == tp2.lastRecords().size());

            bsl::vector<baem_MetricRecord>               allRecords(Z);
            bsl::vector<bsl::vector<baem_MetricRecord> > recordBuffer(Z);
            recordBuffer.resize(NUM_VALUES);
            baem_MetricSample sample(Z);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const char *RECORD_SPEC = VALUES[i].d_records;
                bsl::vector<baem_MetricRecord>& records = recordBuffer[i];

                for (int j = 0; j < bsl::strlen(RECORD_SPEC); ++j) {
                    bsl::string value(1, RECORD_SPEC[j], Z);
                    Id id = registry.getId(value.c_str(), value.c_str());
                    records.push_back(baem_MetricRecord(id));
                    allRecords.push_back(baem_MetricRecord(id));
                }

                bdet_TimeInterval ELAPSED_TIME(VALUES[i].d_elapsedTime, 0);
                bdet_Date dt = bdetu_Date::fromYyyyMmDd(VALUES[i].d_timeStamp);
                bdet_DatetimeTz TIME_STAMP(bdet_Datetime(dt), 0);

                sample.setTimeStamp(TIME_STAMP);
                sample.appendGroup(&records.front(),
                                   records.size(),
                                   ELAPSED_TIME);

                tp2.reset();
                ASSERT(0 == tp2.invocations());
                ASSERT(0 == tp2.lastRecords().size());

                baem_Publisher *p1_p = &tp1;
                baem_Publisher *p2_p = &tp2;

                p1_p->publish(sample);
                p2_p->publish(sample);

                ASSERT(i + 1 == tp1.invocations());
                ASSERT(1     == tp2.invocations());

                ASSERT(i + 1 == tp1.lastSample().numGroups());
                ASSERT(i + 1 == tp2.lastSample().numGroups());

                ASSERT(sample.numRecords() == tp1.lastSample().numRecords());
                ASSERT(sample.numRecords() == tp2.lastSample().numRecords());
                ASSERT(sample.numRecords() == tp1.lastRecords().size());
                ASSERT(sample.numRecords() == tp2.lastRecords().size());

                ASSERT(TIME_STAMP == tp1.lastTimeStamp());
                ASSERT(TIME_STAMP == tp2.lastTimeStamp());

                ASSERT(i + 1 == tp1.lastElapsedTimes().size());
                ASSERT(i + 1 == tp2.lastElapsedTimes().size());

                for (int j = 0; j < sample.numGroups(); ++j) {
                    ASSERT(sample.sampleGroup(j).elapsedTime() ==
                           tp1.lastSample().sampleGroup(j).elapsedTime());
                    ASSERT(sample.sampleGroup(j).elapsedTime() ==
                           tp2.lastSample().sampleGroup(j).elapsedTime());

                    ASSERT(sample.sampleGroup(j).numRecords() ==
                           tp1.lastSample().sampleGroup(j).numRecords());
                    ASSERT(sample.sampleGroup(j).numRecords() ==
                           tp2.lastSample().sampleGroup(j).numRecords());

                    ASSERT(tp1.lastElapsedTimes().end() !=
                           tp1.lastElapsedTimes().find(
                               sample.sampleGroup(j).elapsedTime()));
                    ASSERT(tp2.lastElapsedTimes().end() !=
                           tp2.lastElapsedTimes().find(
                               sample.sampleGroup(j).elapsedTime()));

                    for (int k=0; k<sample.sampleGroup(j).numRecords(); ++k) {
                        ASSERT(sample.sampleGroup(j).records()[k] ==
                               tp1.lastSample().sampleGroup(j).records()[k]);
                        ASSERT(sample.sampleGroup(j).records()[k] ==
                               tp1.lastSample().sampleGroup(j).records()[k]);
                    }
                }

                bsl::sort(allRecords.begin(), allRecords.end(), recordLess);

                ASSERT(allRecords == tp1.lastRecords());
                ASSERT(allRecords == tp2.lastRecords());

                bsl::vector<baem_MetricRecord>::const_iterator it =
                                                           allRecords.begin();
                for (; it != allRecords.end(); ++it) {
                    ASSERT(tp1.contains(it->metricId()));
                    ASSERT(tp2.contains(it->metricId()));
                }
                ASSERT(!tp1.contains(registry.getId("Bad", "Bad")));
                ASSERT(!tp2.contains(registry.getId("Bad", "Bad")));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Concerns:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //      - default and copy constructors (and also the destructor)
        //      - the assignment operator (including aliasing)
        //      - equality operators: 'operator==' and 'operator!='
        //      - the (test-driver supplied) output operator: 'operator<<'
        //      - primary manipulators: 'push_back' and 'clear' methods
        //      - basic accessors: 'size' and 'operator[]'
        //   In addition we would like to exercise objects with potentially
        //   different internal organizations representing the same value.
        //
        // Plan:
        //   Create four objects using the default, make and
        //   copy constructors.  Exercise these objects using primary
        //   manipulators, basic accessors, equality operators, and the
        //   assignment operator.  Invoke the primary (black box) manipulator
        //   [3&5], copy constructor [2&8], and assignment operator [10&9] in
        //   situations where the internal data (i) does *not* and (ii) *does*
        //   have to resize.  Try aliasing with assignment for a non-empty
        //   instance [11] and allow the result to leave scope, enabling the
        //   destructor to assert internal object invariants.  Display object
        //   values frequently in verbose mode:
        //    1. Create an object x1.                       x1:
        //    2. Create a second object x2 (copy from x1).  x1: x2:
        //    3. Append an element value A to x1.           x1:A x2:
        //    4. Append the same element value A to x2.     x1:A x2:A
        //    5. Append another element value B to x2.      x1:A x2:AB
        //    6. Remove all elements from x1.               x1: x2:AB
        //    7. Create a third object x3.                  x1: x2:AB x3:
        //    8. Create a fourth object x4 (copy of x2).    x1: x2:AB x3: x4:AB
        //    9. Assign x2 = x1 (non-empty becomes empty).  x1: x2: x3: x4:AB
        //   10. Assign x3 = x4 (empty becomes non-empty).  x1: x2: x3:AB x4:AB
        //   11. Assign x4 = x4 (aliasing).                 x1: x2: x3:AB x4:AB
        //
        // Tactics:
        //   - Ad Hoc Test Data Selection Method
        //   - Brute Force Implementation Technique
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        {
            if (veryVerbose) cout << "\tTest inactive metrics" << endl;

            TestPublisher  publisher(Z);
            PublisherPtr   pubPtr(&publisher, bcema_SharedPtrNilDeleter(), 0);
            MetricsManager manager(Z);

            manager.addGeneralPublisher(pubPtr);
            Metric metric(manager.metricRegistry().getId("A","A"));
            {
                Obj mX((baem_Collector *)0);
                Obj mY(manager.metricRegistry().getId("A","A"));
                Obj mZ(&metric);

                const Obj& MX = mX;
                const Obj& MY = mY;
                const Obj& MZ = mZ;

                ASSERT(!MX.isActive());
                ASSERT(!MY.isActive());
                ASSERT(!MZ.isActive());
            }
            manager.publishAll();
            ASSERT(0 == publisher.invocations());
        }
        {
            if (veryVerbose) cout << "\tTest active mtrics" << endl;

            TestPublisher  publisher(Z);
            PublisherPtr   pubPtr(&publisher, bcema_SharedPtrNilDeleter(), 0);
            MetricsManager manager(Z);
            Registry&      registry   = manager.metricRegistry();
            Repository&    repository = manager.collectorRepository();

            manager.addGeneralPublisher(pubPtr);
            Metric metric(registry.getId("A","A"), &manager);
            {
                Obj mX(repository.getDefaultCollector("B", "B"));
                Obj mY(manager.metricRegistry().getId("C","C"), &manager);
                Obj mZ(&metric);

                const Obj& MX = mX;
                const Obj& MY = mY;
                const Obj& MZ = mZ;

                ASSERT(MX.isActive());
                ASSERT(MY.isActive());
                ASSERT(MZ.isActive());
            }
            manager.publishAll();
            ASSERT(1 == publisher.invocations());
            ASSERT(3 == publisher.lastRecords().size());
            ASSERT(publisher.contains(registry.getId("A", "A")));
            ASSERT(publisher.contains(registry.getId("B", "B")));
            ASSERT(publisher.contains(registry.getId("C", "C")));
            for (int i = 0; i < publisher.lastRecords().size(); ++i) {
                ASSERT(1   == publisher.lastRecords()[i].count());
                ASSERT(1.0 > publisher.lastRecords()[i].total());
                ASSERT(1.0 > publisher.lastRecords()[i].min());
                ASSERT(1.0 > publisher.lastRecords()[i].max());
            }
        }
        {
            if (veryVerbose) cout << "\tTest default metrics manager" << endl;

            baem_DefaultMetricsManager::create(Z);
            TestPublisher   publisher(Z);
            PublisherPtr    pubPtr(&publisher, bcema_SharedPtrNilDeleter(), 0);
            MetricsManager& manager = *baem_DefaultMetricsManager::instance();
            Registry&       registry = manager.metricRegistry();

            manager.addGeneralPublisher(pubPtr);
            Metric metric(registry.getId("A","A"));
            {
                Obj mY(manager.metricRegistry().getId("B","B"), &manager);
                Obj mZ(&metric);

                const Obj& MY = mY;
                const Obj& MZ = mZ;

                ASSERT(MY.isActive());
                ASSERT(MZ.isActive());
            }
            manager.publishAll();
            ASSERT(1 == publisher.invocations());
            ASSERT(2 == publisher.lastRecords().size());
            ASSERT(publisher.contains(registry.getId("A", "A")));
            ASSERT(publisher.contains(registry.getId("B", "B")));
            for (int i = 0; i < publisher.lastRecords().size(); ++i) {
                ASSERT(1   == publisher.lastRecords()[i].count());
                ASSERT(1.0 > publisher.lastRecords()[i].total());
                ASSERT(1.0 > publisher.lastRecords()[i].min());
                ASSERT(1.0 > publisher.lastRecords()[i].max());
            }
            baem_DefaultMetricsManager::destroy();
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
