// balm_metricsmanager.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_metricsmanager.h>

#include <balm_metricsample.h>
#include <balm_publisher.h>

#include <ball_defaultobserver.h>
#include <ball_log.h>
#include <ball_loggermanager.h>
#include <ball_severity.h>

#include <bslma_testallocator.h>
#include <bslmt_barrier.h>
#include <bdlmt_fixedthreadpool.h>

#include <bdlf_bind.h>
#include <bdlt_dateutil.h>
#include <bdlt_currenttime.h>

#include <bslma_allocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_defaultallocatorguard.h>
#include <bsls_assert.h>

#include <bsl_c_stdio.h>
#include <bsl_c_stdlib.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf_s
#endif

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
// The component under test is a mechanism for organizing metrics to be
// recorded and published.  The 'balm::MetricsManager' provides to general
// classes of methods:
//
// 1) A 'balm::MetricsManager' serves as an organized container of callback
// functors and 'balm::Publisher' objects.   Methods are provided to add and
// remove these objects from the container ('addPublisher', 'removePublisher',
// 'registerCollectionCallback', removeCollectionCallback', etc).
//
// 2) The variants of the 'publish' operation.  The 'publish' operations
// coordinate the behavior of the contained collector-repository, publishers,
// and callbacks.  All the variants of 'publish' perform the same operation
// and differ only w.r.t. how the list of categories to publish is
// supplied.  In fact, all 'publish' operations are implemented using the same
// template function (defined in the .cpp).
//
// The test driver defines a 'TestCallback' class and a 'TestPublisher' class
// that are used to capture the behavior of the metrics manager's 'publish'
// methods.
//
// The "boot-strap" for this test driver spans several test cases:  First the
// the primary container operators are tested, using 'publish', when required,
// to verify internal state; then the test driver exhaustively tests the
// primary publish variant 'publish(const char *[], ...)'.
// ----------------------------------------------------------------------------
// CREATORS
// [ 5]  balm::MetricsManager(bslma::Allocator *basicAllocator = 0);
// [ 5]  ~balm::MetricsManager();
// MANIPULATORS
// [16]  CallbackHandle registerCollectionCallback(const char * ,
//                                           RecordsCollectionCallback );
// [ 6]  CallbackHandle registerCollectionCallback(const balm::Category *,
//                                           RecordsCollectionCallback  );
// [16]  int removeCollectionCallback(CallbackHandle );
// [ 7]  int addGeneralPublisher(bsl::shared_ptr<balm::Publisher>& );
// [18]  int addSpecificPublisher(const char *,
//                                bsl::shared_ptr<balm::Publisher>& );
// [ 7]  int addSpecificPublisher(const balm::Category *,
//                                bsl::shared_ptr<balm::Publisher>&  );
// [ 7]  int removePublisher(balm::Publisher *);
// [24]  int removePublisher(const bsl::shared_ptr<balm::Publisher>& );
// [ 5]  balm::CollectorRepository& collectorRepository();
// [ 5]  balm::MetricRegistry& metricRegistry();
// [22]  void collectSample(balm::MetricSample              *
//                          bsl::vector<balm::MetricRecord> *
//                          bool                            );
// [22]  void collectSample(balm::MetricSample              *,
//                          bsl::vector<balm::MetricRecord> *,
//                          const balm::Category            *[],
//                          int                             ,
//                          bool                            );
// [10]  void publish(const balm::Category  *, const bsls::TimeInterval& );
// [ 8]  void publish(const balm::Category *[], int, const TimeInterval& );
// [ 9]  void publish(const bsl::set<const balm::Category *>& ,
//                    const bsls::TimeInterval&               );
// [11]  void publishAll(const bsls::TimeInterval& );
// [12]  void publishAll(const bsl::set<const balm::Category *>& ,
//                       const bsls::TimeInterval& );
// [15]  void setCategoryEnabled(const balm::Category *, bool );
// [15]  void setCategoryEnabled(const char *, bool );
// [15]  void setAllCategoriesEnabled(bool );
// ACCESSORS
// [ 7]  int findGeneralPublishers(bsl::vector<balm::Publisher *> *) const;
// [17]  int findSpecificPublishers(bsl::vector<balm::Publisher *> *,
//                                  const char *        ) const;
// [ 7] int findSpecificPublishers(bsl::vector<Publisher *> *,
//                                 const Category           *) const;
// [ 5]  const balm::CollectorRepository& collectorRepository() const;
// [ 5]  const balm::MetricRegistry& metricRegistry() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST: general behavior
// [ 2] BREATHING TEST: publisher registration
// [ 3] BREATHING TEST: enabling categories
// [ 4] HELPERS TEST: CombinationIterator, TestPublisher, TestCallback
// [13] AUXILIARY TEST: 'publish' publishes the correct elapsed time
// [14] AUXILIARY TEST: 'publish' publishes only enabled categories
// [19] BSLMA ALLOCATION EXCEPTION TEST: add publishers
// [20] BSLMA ALLOCATION EXCEPTION TEST: add callbacks
// [21] BSLMA ALLOCATION EXCEPTION TEST: publish
// [23] TESTING: 'publish' with 'resetFlag'
// [25] CONCURRENCY TEST
// [26] USAGE EXAMPLE

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

typedef balm::Collector                Collector;
typedef balm::Category                 Category;
typedef balm::MetricId                 Id;
typedef balm::Publisher                Pub;
typedef bsl::shared_ptr<Pub>           PubPtr;
typedef balm::MetricsManager           Obj;
typedef balm::MetricRegistry           Registry;
typedef balm::CollectorRepository      Repository;
typedef Obj::CallbackHandle            CbHandle;

enum {
    MILLISECS_PER_SEC     = 1000,        // one thousand
    MICROSECS_PER_SEC     = 1000000,     // one million
    NANOSECS_PER_MICROSEC = 1000,        // one thousand
    NANOSECS_PER_MILLISEC = 1000000,     // one million
    NANOSECS_PER_SEC      = 1000000000   // one billion
};

// ============================================================================
//                            CLASSES FOR TESTING
// ----------------------------------------------------------------------------

inline
bool recordLess(const balm::MetricRecord& lhs, const balm::MetricRecord& rhs)
    // Return 'true' if the specified 'lhs' is less than (ordered before) the
    // specified 'rhs', and 'false' otherwise.  A 'balm::MetricRecord' are
    // ordered by their 'metricId' return value.
{
    return lhs.metricId() < rhs.metricId();
}

bool withinWindow(const bdlt::DatetimeTz&   localTime,
                  const bdlt::Datetime&     utcExpectedTime,
                  int                      windowMs)
    // Return 'true' if the specified 'localTime' is within the specified
    // 'windowMs' (milliseconds) of the specified 'utcExpectedTime'.
{
    bdlt::Datetime gmtTime = localTime.utcDatetime();
    bdlt::Datetime begin   = utcExpectedTime;
    bdlt::Datetime end     = utcExpectedTime;
    begin.addMilliseconds(-windowMs);
    end.addMilliseconds(windowMs);

    bool withinWindow = begin < gmtTime && end > gmtTime;
    if (!withinWindow) {
        P_(localTime); P_(gmtTime); P(utcExpectedTime);
    }
    return withinWindow;
}

inline
const balm::Category *firstCategory(const balm::MetricSampleGroup& group)
    // Return the 'balm::Category' for the first record in 'group'.  'ASSERT'
    // if all the records in the group do not belong to the same category.  The
    // behavior is undefined in 'group.numRecords()' is 0.
{

    ASSERT(0 < group.numRecords());
    const balm::MetricRecord *record = group.records();
    const balm::Category *value = record->metricId().category();
    for (; record != group.records() + group.numRecords(); ++record) {
        ASSERT(value == record->metricId().category());
    }
    return value;
}

bool withinWindow(const bsls::TimeInterval& value,
                  const bsls::TimeInterval& expectedValue,
                  int                      windowMs)
    // Return 'true' if the specified 'value' is within the specified
    // 'windowMs' (milliseconds) of the specified 'expectedValue'.
{
    bsls::TimeInterval window(0, windowMs * NANOSECS_PER_MILLISEC);
    bool withinWindow = (expectedValue - window) < value
                     && (expectedValue + window) > value;

    if (!withinWindow) {
        P_(windowMs); P_(expectedValue); P(value);
    }
    return withinWindow;
}

                             // ==================
                             // class TestCallback
                             // ==================

class TestCallback {
    // This class is used to provide a callback matching
    // 'balm::MetricsManager::MetricsCollectionCallback'.  A 'TestCallback' is
    // supplied a 'balm::MetricId' at construction that determines the id of
    // the metric returned by the 'recordMetrics' operation.  A 'TestCallback'
    // object keeps track of the number of times 'recordMetrics' is invoked.  A
    // callback object also provides a 'function' method that returns a
    // 'bsl::function' object matching the
    // 'balm::MetricsManager::MetricsCollectionCallback' signature that, on
    // invocation, calls 'recordMetrics' on the object.

    // DATA
    bsls::AtomicInt     d_numInvocations; // number of invocations

    balm::MetricRecord  d_record;         // record to append on
                                          // 'recordMetrics'

    bool                d_reset;          // last invocation's resetFlag

    bslma::Allocator   *d_allocator_p;    // allocator (held, not owned)

    // NOT IMPLEMENTED
    TestCallback(const TestCallback& );
    TestCallback& operator=(const TestCallback& );
  public:
    // CREATORS
    TestCallback(balm::MetricId metricId, bslma::Allocator *allocator);
        // Create a 'TestCallback' that will populate a metric record with the
        // specified 'metricId'.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    TestCallback(const balm::MetricRecord& record,
                 bslma::Allocator *allocator);
        // Create a 'TestCallback' that will populate a metric with the
        // specified 'record'.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    // MANIPULATORS
    void recordMetrics(bsl::vector<balm::MetricRecord> *records,
                       bool                             resetFlag);
        // Increment 'invocations()' and append to the specified 'records' a
        // 'balm::MetricRecord' containing the 'balm::MetricId' or matching the
        // 'balm::MetricRecord' supplied at construction.

    balm::MetricRecord& record();
        // Return a reference to the modifiable 'balm::MetricRecord' object
        // that will be appended on a call to 'recordMetrics'.

    balm::MetricsManager::RecordsCollectionCallback function();
        // Return a 'bsl::function' of type
        // 'balm::MetricsManager::RecordsCollectionCallback' that will invoke
        // the 'recordMetrics' method on this object.

    void reset();
        // Set the number of invocations to 0.

    // ACCESSORS
    const balm::MetricRecord& record() const;
        // Return a reference to the non-modifiable 'balm::MetricRecord' object
        // that will be appended on a call to 'recordMetrics'.

    balm::MetricId metricId() const;
        // Return a 'balm::MetricId' object identifying the
        // 'balm::MetricRecord' that will be appended on a call to
        // 'recordMetrics'.

    int invocations() const;
        // Return the number of invocations of 'recordMetrics' since this
        // object was constructed or the last invocation of 'reset()'.

    bool resetFlag() const;
        // Return 'true' if the last callback invocation requested a reset,
        // and 'false' otherwise.
};

                             // ------------------
                             // class TestCallback
                             // ------------------

// CREATORS
inline
TestCallback::TestCallback(Id metricId, bslma::Allocator *allocator)
: d_numInvocations(0)
, d_record(metricId)
, d_reset(false)
, d_allocator_p(allocator)
{
}

inline
TestCallback::TestCallback(const balm::MetricRecord&  record,
                           bslma::Allocator          *allocator)
: d_numInvocations(0)
, d_record(record)
, d_reset(false)
, d_allocator_p(allocator)
{
}

// MANIPULATORS
void TestCallback::recordMetrics(bsl::vector<balm::MetricRecord> *records,
                                 bool                             resetFlag)
{
    d_reset = resetFlag;
    ++d_numInvocations;
    records->push_back(d_record);
}

inline
balm::MetricRecord& TestCallback::record()
{
    return d_record;
}

inline
void TestCallback::reset()
{
    d_numInvocations = 0;
}

// ACCESSORS
inline
const balm::MetricRecord& TestCallback::record() const
{
    return d_record;
}

inline
balm::MetricId TestCallback::metricId() const
{
    return d_record.metricId();
}

inline
balm::MetricsManager::RecordsCollectionCallback TestCallback::function()
{
    balm::MetricsManager::RecordsCollectionCallback cb =
        bdlf::BindUtil::bind(&TestCallback::recordMetrics,
                             this,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2);
    return cb;
}

inline
int TestCallback::invocations() const
{
    return d_numInvocations;
}

inline
bool TestCallback::resetFlag() const
{
    return d_reset;
}

                         // =========================
                         // class LockAndModifyWorker
                         // =========================

class LockAndModifyWorker {
    // This class owns a thread in which manipulators of a
    // 'balm::MetricsManager' are invoked under the scope of another lock,
    // repeatedly.

    bslmt::Mutex                *d_mutex_p;
    balm::MetricsManager        *d_obj_p;
    bslmt::ThreadUtil::Handle    d_thread;
    bsls::AtomicInt              d_done;
    const balm::Category        *d_myCategory_p;

    void worker();
    static void dummyCallback(bsl::vector<balm::MetricRecord>*,
                              bool) {}

public:
    // CREATORS
    LockAndModifyWorker(bslmt::Mutex *mutex,
                        balm::MetricsManager *obj)
    : d_mutex_p(mutex)
    , d_obj_p(obj)
    , d_myCategory_p(obj->metricRegistry().addCategory("LOCKANDMODIFYWORKER"))
    {}

    int start() {
        d_done = 0;
        return bslmt::ThreadUtil::create(
                      &d_thread,
                      bdlf::MemFnUtil::memFn(&LockAndModifyWorker::worker,
                                             this));
    }

    void stop() {
        d_done = 1;
        bslmt::ThreadUtil::join(d_thread);
    }
};

void
LockAndModifyWorker::worker() {
    while (!d_done) {
        bslmt::LockGuard<bslmt::Mutex> guard(d_mutex_p);

        balm::MetricsManager::CallbackHandle handle =
            d_obj_p->registerCollectionCallback(
                                          d_myCategory_p,
                                          &LockAndModifyWorker::dummyCallback);
        d_obj_p->removeCollectionCallback(handle);
    }
}

                           // ======================
                           // class LockingPublisher
                           // ======================

class LockingPublisher : public balm::Publisher {
    // This class defines a test implementation of 'balm::Publisher' that
    // locks and unlocks a specified mutex when publish() is invoked.

    bslmt::Mutex *d_mutex_p;

public:

    // CREATORS
    LockingPublisher(bslmt::Mutex *mutex)
    : d_mutex_p(mutex)
    {}

    // MANIPULATORS
    virtual void publish(const balm::MetricSample&);
       // Lock and unlock the mutex specified at construction.
};

void
LockingPublisher::publish(const balm::MetricSample&) {
    bslmt::LockGuard<bslmt::Mutex> guard(d_mutex_p);
}

                            // ===================
                            // class TestPublisher
                            // ===================

class TestPublisher : public balm::Publisher {
    // This class defines a test implementation of the 'balm::Publisher' that
    // protocol can be used to record information about invocations of the
    // 'publish' methods.  Each 'TestPublisher' instance tracks the number of
    // times 'publish' has been called, and maintains 'lastTimeStamp()', and
    // 'lastRecords()' values holding the time stamp, and record values (in
    // sorted order) of the last published 'balm::MetricSample' object.  A
    // 'TestPublisher' also maintains a reconstruction of the most recently
    // published sample, accessed using the 'lastSample()'.  The returned
    // sample value contains the same metric record values organized into the
    // same groups as the published sample, but the returned sample does not
    // refer to the the same addresses in memory: so the returned sample is
    // equivalent but *not* *equal* to the published sample.  The
    // 'TestPublisher' also provides a 'reset()' operation to reset the
    // invocation count to 0 and clear the 'lastRecords' and 'lastSample'
    // information.  Note that the 'publish' method is *not* thread-safe.

    // DATA
    bsls::AtomicInt                 d_numInvocations;  // # of invocations

    bsl::vector<balm::MetricRecord> d_recordBuffer;    // last samples records

    bsl::vector<balm::MetricRecord> d_sortedRecords;   // last sample's records
                                                       // in sorted order

    balm::MetricSample              d_sample;          // reconstructed last
                                                       // sample (using
                                                       // 'd_recordsBuffer')

    bsl::set<bsls::TimeInterval>    d_elapsedTimes;    // last elapsed times

    // NOT IMPLEMENTED
    TestPublisher(const TestPublisher& );
    TestPublisher& operator=(const TestPublisher& );

  public:

    // CREATORS
    TestPublisher(bslma::Allocator *allocator);
        // Create a test publisher with 0 'invocations()' and the default
        // constructed 'lastSample()' using the specified 'allocator' to
        // supply memory.

    virtual ~TestPublisher();
        // Destroy this test publisher.

    // MANIPULATORS
    virtual void publish(const balm::MetricSample& sample);
        // Increment the number of 'invocations()', set the 'lastTimeStamp()'
        // to 'sampleTimeStamp()', set 'lastRecords()' to be the records
        // referred to by 'sample' in *sorted* order, set 'lastElapsedTimes()'
        // to be the set of elapsed time values contained in 'sample', and
        // finally set 'lastSample' to a congruent (but *not* equal) sample
        // value.  The 'lastSample' value contains the same metric record
        // values organized into the same groups as the published sample, but
        // the returned sample does not refer to the the same addresses in
        // memory: so the returned sample is equivalent but not equal to
        // the published sample.

   void reset();
        // Set 'invocations()' to 0, clear the 'lastRecords()' sequence.

    // ACCESSORS
    int invocations() const;
        // Return the number of times the 'publish' methods have been invoked
        // since this test publisher was constructed or the last call to
        // 'reset()'.

    const balm::MetricSample& lastSample() const;
        // Return a reference to the non-modifiable reconstruction of the last
        // sample passed to a 'publish' method.  The returned sample value
        // contains the same metric record values organized into the same
        // groups as the published sample, but the returned sample does not
        // refer to the the same addresses in memory: so the returned sample is
        // congruent but *not* *equal* to the published sample.

    const bsl::vector<balm::MetricRecord>& lastRecords() const;
        // Return a reference to the non-modifiable sequence of records
        // containing the values of the records in the last sample passed to
        // the 'publish' methods in *sorted* order, or an empty vector if none
        // of the 'publish' methods have been called since this object was
        // created or last reset.

    const bsl::set<bsls::TimeInterval>& lastElapsedTimes() const;
        // Return a reference to the non-modifiable set of elapsed time values
        // contained in the last sample passed to the 'publish' methods.  The
        // behavior is undefined unless a 'publish' method has been invoked
        // since this object was created or last reset.

    const bdlt::DatetimeTz& lastTimeStamp() const;
        // Return a reference to the non-modifiable time stamp of the
        // last sample passed to a 'publish' method.  The behavior is undefined
        // unless a 'publish' method has invoked since this object was created
        // or last reset.

    int indexOf(const balm::MetricId& id) const;
        // Return the index into 'lastRecords()' of the specified 'id', or -1
        // if there is no record with 'id' in 'lastRecords()'.

    bool contains(const balm::MetricId& id) const;
        // Return 'true' if the 'lastSample' contains a 'balm::MetricRecord'
        // object whose 'metricId()' equals the specified 'id', and 'false'
        // otherwise.  Note that this operation is logically equivalent to
        // 'index(id) != -1'.

};

                            // -------------------
                            // class TestPublisher
                            // -------------------

// CREATORS
inline
TestPublisher::TestPublisher(bslma::Allocator *allocator)
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
void TestPublisher::publish(const balm::MetricSample& sample)
{
    d_numInvocations++;

    d_sample.removeAllRecords();
    d_recordBuffer.clear();
    d_sortedRecords.clear();
    d_elapsedTimes.clear();

    d_sample.setTimeStamp(sample.timeStamp());
    if (0 == sample.numRecords()) {
        return;                                                       // RETURN
    }

    // We *must* reserve memory to avoid re-allocating data while building
    // the sample
    d_recordBuffer.reserve(sample.numRecords());
    balm::MetricSample::const_iterator sIt = sample.begin();
    for (; sIt != sample.end(); ++sIt) {
        balm::MetricSampleGroup::const_iterator gIt = sIt->begin();
        BSLS_ASSERT(gIt != sIt->end());
        d_recordBuffer.push_back(*gIt);
        balm::MetricRecord *head = &d_recordBuffer.back();
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
const balm::MetricSample& TestPublisher::lastSample() const
{
    return d_sample;
}

inline
const bsl::set<bsls::TimeInterval>& TestPublisher::lastElapsedTimes() const
{
    return d_elapsedTimes;
}

inline
const bdlt::DatetimeTz& TestPublisher::lastTimeStamp() const
{
    return d_sample.timeStamp();
}

inline
const bsl::vector<balm::MetricRecord>& TestPublisher::lastRecords() const
{
    return d_sortedRecords;
}

inline
int TestPublisher::indexOf(const balm::MetricId& id) const
{
    balm::MetricRecord searchRecord(id);
    bsl::vector<balm::MetricRecord>::const_iterator it =
                                   bsl::lower_bound(d_sortedRecords.begin(),
                                                    d_sortedRecords.end(),
                                                    searchRecord, recordLess);
    if (it == d_sortedRecords.end()) {
        return -1;                                                    // RETURN
    }
    return (it->metricId() == id) ? it - d_sortedRecords.begin() : -1;
}

inline
bool TestPublisher::contains(const balm::MetricId& id) const
{
    return indexOf(id) != -1;
}

                         // =========================
                         // class CombinationIterator
                         // =========================

template <class T>
class CombinationIterator {
    // This class provides an iterator over the set of possible combinations
    // of elements.  A 'CombinationIterator' object is supplied a vector of
    // values of parameterized type 'T' at construction.  The behavior is
    // undefined unless the supplied vector contains unique elements and has a
    // size between 1 and 31 (inclusive).  The 'current' method returns a
    // reference to a vector containing the current combination of elements.
    // The 'next' method increments the iterator to the next possible
    // combination of elements.  Note that the sequence of combinations always
    // begins with the empty set and ends with the set containing all values.

    // DATA
    bsl::vector<T> d_values;               // sequence of values to combine
    bsl::vector<T> d_currentCombination;   // current combination
    int            d_bits;                 // current bit mask of 'd_values'
    int            d_maxBits;              // max bit mask of 'd_values'

    // PRIVATE MANIPULATORS
    void createCurrentCombination();
        // Populate 'd_currentCombination' with those elements of 'd_values'
        // whose corresponding bit in the 'd_bits' bit-mask is is 1.

  public:

    // CREATORS
    CombinationIterator(const bsl::vector<T>&  values,
                        bslma::Allocator      *allocator);
        // Create this combination iterator and initialize it with the first
        // combination of values (the empty set of values), use the specified
        // 'allocator' to supply memory.  The behavior is undefined unless '
        // '0 < values.size() < 32' and each element of 'values' appears only
        // once.

    bool next();
        // If this iterator is not at the end of the sequence of combinations,
        // iterator to the next combination of value and return 'true',
        // otherwise return 'false'.

     // ACCESSORS
     const bsl::vector<T>& current() const;
        // Return a reference to the non-modifiable combination of
        // values that the iterator currently is positioned at.

     bool includesElement(int index) const;
        // Return 'true' if the 'current()' combination contains the value at
        // the specified 'index' in the sequence supplied at construction.

};

                         // -------------------------
                         // class CombinationIterator
                         // -------------------------

// PRIVATE MANIPULATORS
template <class T>
void CombinationIterator<T>::createCurrentCombination()
{
    d_currentCombination.clear();
    for (int i = 0; i < d_values.size(); ++i) {
        if (d_bits & (1 << i)) {
            d_currentCombination.push_back(d_values[i]);
        }
    }
}

// CREATORS
template <class T>
CombinationIterator<T>::CombinationIterator(const bsl::vector<T>&  values,
                                            bslma::Allocator      *allocator)
: d_values(values, allocator)
, d_currentCombination(allocator)
, d_maxBits( (1 << values.size()) - 1 )
, d_bits(0)
{
    BSLS_ASSERT(values.size() > 0);
    BSLS_ASSERT(values.size() < 32);
    d_currentCombination.reserve(d_values.size());
    createCurrentCombination();
}

// MANIPULATORS
template <class T>
inline
bool CombinationIterator<T>::next()
{
    if (d_bits >= d_maxBits) {
        return false;                                                 // RETURN
    }
    ++d_bits;
    createCurrentCombination();
    return true;
}

// ACCESSORS
template <class T>
inline
const typename bsl::vector<T>& CombinationIterator<T>::current() const
{
    return d_currentCombination;
}

template <class T>
inline
bool CombinationIterator<T>::includesElement(int index) const
{
    return d_bits & (1 << index);
}

                           // =====================
                           // class ConcurrencyTest
                           // =====================

void stringId(bsl::string *resultId, const char *heading, int value)
    // Populate the specified 'resultId' with a null-terminated string
    // identifier containing the specified 'heading' concatenated with the
    // specified 'value'.  The behavior is undefined if the resulting
    // identifier would be larger than 100 (including the null terminating
    // character).
{
    char buffer[100];
    int rc = snprintf(buffer, 100, "%s-%d", heading, value);
    BSLS_ASSERT(rc < 100);
    BSLS_ASSERT(rc > 0);
    *resultId = buffer;
}

void stringId(bsl::string *resultId,
              const char  *heading,
              int          value1,
              int          value2)
    // Populate the specified 'resultId' with a null-terminated string
    // identifier containing the specified 'heading' concatenated with the
    // specified 'value1' and 'value2'.  The behavior is undefined if the
    // resulting identifier would be larger than 100 (including the null
    // terminating character).
{
    char buffer[100];
    int rc = snprintf(buffer, 100, "%s-%d-%d", heading, value1, value2);
    BSLS_ASSERT(rc < 100);
    BSLS_ASSERT(rc > 0);
    *resultId = buffer;
}

class ConcurrencyTest {
    // Invoke a set of operations operations synchronously.

    // DATA
    bdlmt::FixedThreadPool     d_pool;
    bslmt::Barrier             d_barrier;
    balm::MetricsManager      *d_manager_p;
    bslma::Allocator          *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

  public:

    // CREATORS
    ConcurrencyTest(int                  numThreads,
                    balm::MetricsManager *manager,
                    bslma::Allocator    *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_manager_p(manager)
    , d_allocator_p(basicAllocator)
    {
        d_pool.start();
    }

    ~ConcurrencyTest() {}

    //  MANIPULATORS
    void runTest();
        // Run the test.
};

void ConcurrencyTest::execute()
{
    // On each iteration of this test, a group of identifiers unique to the
    // iteration are created.  4 of those identifiers are shared by all threads
    // (but are unique to the test iteration), while 2 are unique across all
    // threads and iterations.

    typedef bsl::shared_ptr<TestCallback> CbPtr;
    typedef bsl::pair<CbHandle, CbPtr>    CallbackInfo;
    typedef bsl::map<Id, CallbackInfo>    CallbackMap;

    const int NUM_THREADS = d_barrier.numThreads();

    bslma::Allocator *Z = d_allocator_p;
    Obj *mX = d_manager_p; const Obj *MX = mX;
    Registry& registry = mX->metricRegistry();
    for (int i = 0; i < 10; ++i) {

        // Create 2 strings unique for this iteration.
        bsl::string iterStringA, iterStringB;
        stringId(&iterStringA, "A", i); stringId(&iterStringB, "B", i);

        const char *A_VAL = iterStringA.c_str();
        const char *B_VAL = iterStringB.c_str();

        // Create 2 strings unique across all threads & iterations.
        bsl::string uniqueString1, uniqueString2;
        stringId(&uniqueString1, "U1", bslmt::ThreadUtil::selfIdAsInt(), i);
        stringId(&uniqueString2, "U2", bslmt::ThreadUtil::selfIdAsInt(), i);
        const char *S1 = uniqueString1.c_str();
        const char *S2 = uniqueString2.c_str();

        const char *CATEGORIES[] = { A_VAL, B_VAL, S1, S2 };
        const int NUM_CATEGORIES = sizeof CATEGORIES / sizeof *CATEGORIES;

        bsl::vector<const balm::Category *> allCategories(Z);
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            allCategories.push_back(registry.getCategory(CATEGORIES[i]));
        }

        TestPublisher gPub(Z), aPub(Z), bPub(Z), s1Pub(Z), s2Pub(Z);
        PubPtr gPub_p(&gPub, bslstl::SharedPtrNilDeleter(), Z);
        PubPtr aPub_p(&aPub, bslstl::SharedPtrNilDeleter(), Z);
        PubPtr bPub_p(&bPub, bslstl::SharedPtrNilDeleter(), Z);
        PubPtr s1Pub_p(&s1Pub, bslstl::SharedPtrNilDeleter(), Z);
        PubPtr s2Pub_p(&s2Pub, bslstl::SharedPtrNilDeleter(), Z);

        balm::MetricId idA  = registry.getId(A_VAL, S1);
        balm::MetricId idB  = registry.getId(B_VAL, S1);
        balm::MetricId idS1 = registry.getId(S1, S1);
        balm::MetricId idS2 = registry.getId(S2, S2);
        TestCallback aCb(idA, Z), bCb(idB, Z), s1Cb(idS1, Z), s2Cb(idS2, Z);

        ASSERT(0 == gPub.invocations());
        ASSERT(0 == aPub.invocations());
        ASSERT(0 == bPub.invocations());
        ASSERT(0 == s1Pub.invocations());
        ASSERT(0 == s2Pub.invocations());
        ASSERT(0 == aCb.invocations());
        ASSERT(0 == bCb.invocations());
        ASSERT(0 == s1Cb.invocations());
        ASSERT(0 == s2Cb.invocations());

        // Begin the test iteration
        d_barrier.wait();

        ASSERT(0 == mX->addGeneralPublisher(gPub_p));
        ASSERT(0 == mX->addSpecificPublisher(A_VAL, aPub_p));
        ASSERT(0 == mX->addSpecificPublisher(B_VAL, bPub_p));
        ASSERT(0 == mX->addSpecificPublisher(S1, s1Pub_p));
        ASSERT(0 == mX->addSpecificPublisher(S2, s2Pub_p));

        bsl::vector<balm::Publisher *> pubs(Z);
        ASSERT(1 <= MX->findGeneralPublishers(&pubs));
        ASSERT(1 <= pubs.size());

        pubs.clear();
        ASSERT(1 <= MX->findSpecificPublishers(&pubs, A_VAL));
        ASSERT(1 <= pubs.size());

        pubs.clear();
        ASSERT(1 <= MX->findSpecificPublishers(&pubs, B_VAL));
        ASSERT(1 <= pubs.size());

        pubs.clear();
        ASSERT(1 == MX->findSpecificPublishers(&pubs, S1));
        ASSERT(1 == pubs.size());
        ASSERT(&s1Pub == pubs[0]);

        pubs.clear();
        ASSERT(1 == MX->findSpecificPublishers(&pubs, S2));
        ASSERT(1 == pubs.size());
        ASSERT(&s2Pub == pubs[0]);

        CbHandle kA  = mX->registerCollectionCallback(A_VAL, aCb.function());
        CbHandle kB  = mX->registerCollectionCallback(B_VAL, bCb.function());
        CbHandle kS1 = mX->registerCollectionCallback(S1, s1Cb.function());
        CbHandle kS2 = mX->registerCollectionCallback(S2, s2Cb.function());
        ASSERT(Obj::e_INVALID_HANDLE != kA);
        ASSERT(Obj::e_INVALID_HANDLE != kB);
        ASSERT(Obj::e_INVALID_HANDLE != kS1);
        ASSERT(Obj::e_INVALID_HANDLE != kS2);

        // Test 'collectSample'
        bsl::vector<balm::MetricRecord> records;
        balm::MetricSample              sample;
        mX->collectSample(&sample,
                          &records,
                          allCategories.data(),
                          allCategories.size());

        bdlt::Datetime now = bdlt::CurrentTime::utc();

        ASSERT(!s1Cb.resetFlag());
        ASSERT(!s2Cb.resetFlag());
        ASSERT(1 <= aCb.invocations());
        ASSERT(1 <= bCb.invocations());
        ASSERT(1 == s1Cb.invocations());
        ASSERT(1 == s2Cb.invocations());
        ASSERT(withinWindow(sample.timeStamp(), now, 100));

        // Test 'publish'.
        mX->publish(allCategories.data(), allCategories.size());

        // Verify the callbacks were invoked.
        ASSERT(2 <= aCb.invocations());
        ASSERT(2 <= bCb.invocations());
        LOOP2_ASSERT( bslmt::ThreadUtil::selfIdAsInt(),
                      s1Cb.invocations(),
                      2 == s1Cb.invocations());
        LOOP2_ASSERT( bslmt::ThreadUtil::selfIdAsInt(),
                      s2Cb.invocations(),
                      2 == s2Cb.invocations());

        // Verify the publishers were invoked.
        ASSERT(1 <= gPub.invocations());
        ASSERT(1 <= aPub.invocations());
        ASSERT(1 <= bPub.invocations());
        ASSERT(1 == s1Pub.invocations());
        ASSERT(1 == s1Pub.lastRecords().size());
        ASSERT(s1Pub.contains(idS1));

        ASSERT(1 == s2Pub.invocations());
        ASSERT(1 == s2Pub.lastRecords().size());
        ASSERT(s2Pub.contains(idS2));

        mX->publish(allCategories.data(), allCategories.size());

        // Verify the callbacks were invoked.
        ASSERT(3 <= aCb.invocations());
        ASSERT(3 <= bCb.invocations());
        LOOP2_ASSERT(bslmt::ThreadUtil::selfIdAsInt(),
                     s1Cb.invocations(),
                     3 == s1Cb.invocations());
        LOOP2_ASSERT(bslmt::ThreadUtil::selfIdAsInt(),
                     s2Cb.invocations(),
                     3 == s2Cb.invocations());

        // Verify the publishers were invoked.
        ASSERT(2 <= gPub.invocations());
        ASSERT(2 <= aPub.invocations());
        ASSERT(2 <= bPub.invocations());

        ASSERT(2 == s1Pub.invocations());
        ASSERT(1 == s1Pub.lastRecords().size());
        ASSERT(s1Pub.contains(idS1));

        ASSERT(2 == s2Pub.invocations());
        ASSERT(1 == s2Pub.lastRecords().size());
        ASSERT(s2Pub.contains(idS2));

        // Remove some publishers and callbacks
        ASSERT(0 == mX->removePublisher(aPub_p));
        ASSERT(0 != mX->removePublisher(aPub_p));
        ASSERT(0 == mX->removeCollectionCallback(kA));
        ASSERT(0 != mX->removeCollectionCallback(kA));
        ASSERT(0 == mX->removeCollectionCallback(kS1));
        ASSERT(0 != mX->removeCollectionCallback(kS1));

        d_barrier.wait();

        const int BCB_INV  =  bCb.invocations();
        const int S2CB_INV = s2Cb.invocations();
        const int GPUB_INV = gPub.invocations();
        const int BPUB_INV = bPub.invocations();

        d_barrier.wait();

        mX->publishAll();

        d_barrier.wait();

        // Verify that the 'B' callback and the s2Cb were both invoked
        // 'NUM_THREADS' additional times.
        ASSERT(BCB_INV  + NUM_THREADS == bCb.invocations());
        ASSERT(S2CB_INV + NUM_THREADS == s2Cb.invocations());

        // Verify the publishers were invoked.
        ASSERT(GPUB_INV + NUM_THREADS == gPub.invocations());
        ASSERT(!gPub.contains(idA));
        ASSERT( gPub.contains(idB));
        ASSERT(!gPub.contains(idS1));
        ASSERT( gPub.contains(idS2));
        ASSERT(BPUB_INV + NUM_THREADS == bPub.invocations());
        ASSERT(bPub.contains(idB));

        ASSERT(2 + NUM_THREADS == s2Pub.invocations());
        ASSERT(s2Pub.contains(idS2));

        ASSERT(0 == mX->removePublisher(gPub_p));
        ASSERT(0 != mX->removePublisher(gPub_p));
        ASSERT(0 == mX->removePublisher(bPub_p));
        ASSERT(0 != mX->removePublisher(bPub_p));
        ASSERT(0 == mX->removeCollectionCallback(kB));
        ASSERT(0 != mX->removeCollectionCallback(kB));
        ASSERT(0 == mX->removeCollectionCallback(kS2));
        ASSERT(0 != mX->removeCollectionCallback(kS2));

        d_barrier.wait();
    }

}

void ConcurrencyTest::runTest()
{
    bsl::function<void()> job = bdlf::BindUtil::bind(&ConcurrencyTest::execute,
                                                     this);
    for (int i = 0; i < d_pool.numThreads(); ++i) {
        d_pool.enqueueJob(job);
    }
    d_pool.drain();
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// SimpleStreamPublisher was defined in 'balm_publisher.h'.

    // simplestreampublisher.h
    class SimpleStreamPublisher : public balm::Publisher {
        // A simple implementation of the 'balm::Publisher' protocol that
        // writes metric records to a stream.

        // PRIVATE DATA
        bsl::ostream& d_stream; // output stream (held, not owned)

        // NOT IMPLEMENTED
        SimpleStreamPublisher(const SimpleStreamPublisher& );
        SimpleStreamPublisher& operator=(const SimpleStreamPublisher& );

    public:
        // CREATORS
        SimpleStreamPublisher(bsl::ostream& stream);
            // Create this publisher that will public metrics to the specified
            // 'stream' using the specified 'registry' to identify published
            // metrics.

        virtual ~SimpleStreamPublisher();
             // Destroy this publisher.

        // MANIPULATORS
        virtual void publish(const balm::MetricSample& metricValues);
            // Publish the specified 'metricValues'.  This implementation will
            // write the 'metricValues' to the output stream specified on
            // construction.
    };

    // simplestreampublisher.cpp

    // CREATORS
    SimpleStreamPublisher::SimpleStreamPublisher(bsl::ostream& stream)
    : d_stream(stream)
    {
    }

    SimpleStreamPublisher::~SimpleStreamPublisher()
    {
    }

    // MANIPULATORS
    void SimpleStreamPublisher::publish(const balm::MetricSample& metricValues)
    {
        if (metricValues.numRecords() > 0) {
            d_stream << metricValues.timeStamp() << " "
                     << metricValues.numRecords() << " Records" << bsl::endl;

            balm::MetricSample::const_iterator sIt = metricValues.begin();
            for (; sIt != metricValues.end(); ++sIt) {
                d_stream << "\tElapsed Time: "
                         << sIt->elapsedTime().totalSecondsAsDouble()
                         << "s" << bsl::endl;
                balm::MetricSampleGroup::const_iterator gIt = sIt->begin();
                for (; gIt != sIt->end(); ++gIt) {
                    d_stream << "\t" << gIt->metricId()
                             << " [count = " << gIt->count()
                             << ", total = " << gIt->total()
                             << ", min = "   << gIt->min()
                             << ", max = "   << gIt->max() << "]" << bsl::endl;
                }
            }
        }
    }

///Example 2 - Recording metric values with 'balm::Collector'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This second example demonstrates using 'balm::Collector' objects (obtained
// from a metrics manager's collector repository) to collect metrics related to
// a hypothetical 'EventHandler' class.  On construction, the event handler
// obtains references to 'balm::Collector' objects from the metrics
// manager's collector repository.  On each handled event, the 'EventHandler',
// updates its collectors with the appropriate metric values.
//
// Note that the 'balm_metric' component provides both classes and macros to
// reduce the code required for collecting metric values.
//..
    class EventHandler {
        // Provide an example event handling mechanism that records metrics
        // for (1) the size of the processed event messages and (2) the number
        // of failures using 'balm::Collector' objects provided by a
        // 'balm::MetricsManager'.

        // PRIVATE DATA
        balm::Collector *d_eventMessageSizes_p;  // collect the message sizes

        balm::Collector *d_eventFailures_p;      // collect the number of
                                                 // failures

    // ...

      public:
        // CREATORS
//..
// We obtain the addresses of the respective 'balm::Collector' objects that we
// will use to collect metrics values, from the metrics managers' collector
// repository.  Note that in practice, clients can use the
// 'balm::DefaultMetricManager' (see 'balm_defaultmetricsmanager' and
// 'balm_metric') rather than explicitly pass the address of 'manager'.
//..
        EventHandler(balm::MetricsManager *manager)
        : d_eventMessageSizes_p(
               manager->collectorRepository().getDefaultCollector(
                                                "MyCategory", "messageSizes"))
        , d_eventFailures_p(
               manager->collectorRepository().getDefaultCollector(
                                                "MyCategory", "eventFailures"))
        {}

        // MANIPULATORS
//..
// Then, when processing an "event", we update the 'balm::Collector' objects
// with the appropriate metric values for the event.
//..
        int handleEvent(int eventId, const bsl::string& eventMessage)
            // Process the event described by the specified 'eventId' and
            // 'eventMessage' .  Return 0 on success, and a non-zero value
            // if there was an error handling the event.
        {
           int returnCode = 0;
           d_eventMessageSizes_p->update(eventMessage.size());

    // ...    (Process the event)

           if (0 != returnCode) {
               d_eventFailures_p->update(1);
           }
           return returnCode;
        }

    // ...

    };
//..
///Example 3 - Recording metric values with a callback
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// The metrics manager provides a facility to register a callback that will
// report metric values.  A callback should be used if clients want to
// customize how a metric, or group of metrics, are recorded.  In the following
// example, the 'EventHandlerWithCallback' class maintains a metric for the
// average number of events per second that it reports through a
// 'balm::MetricsManager::MetricsCollectionCallback'.
//..
    // eventhandlerwithcallback.h

    class EventHandlerWithCallback {
        // Provide an example event handling mechanism that records a
        // metric for the number of requests per second and reports the metric
        // using a 'balm::MetricsManager::RecordsCollectionCallback'.

        // PRIVATE DATA
        bsls::AtomicInt       d_numEvents;         // number of requests

        bsls::TimeInterval    d_periodStart;       // start of the current
                                                   // period

        balm::MetricId        d_eventsPerSecId;    // identifies the events-
                                                   // per-second metric
        balm::MetricsManager::CallbackHandle
                             d_callbackHandle;     // identifies the callback

        balm::MetricsManager *d_metricsManager_p;  // metrics manager (held,
                                                   // but not owned)
     // ...

        // PRIVATE MANIPULATORS
        void collectMetricsCb(bsl::vector<balm::MetricRecord> *records,
                              bool                             resetFlag);
            // Append to the specified 'records' the aggregated values of the
            // metrics recorded by this event handler and, if 'resetFlag' is
            // 'true', reset those metric values to their default state.  Note
            // that this method is intended to be used as a callback, and is
            // consistent with the
            // 'balm::MetricsManager::RecordsCollectionCallback' function
            // prototype.

      public:
        // CREATORS
        EventHandlerWithCallback(balm::MetricsManager *manager,
                                 bslma::Allocator    *basicAllocator = 0);
            // Initialize this object to use the specified 'manager' to record
            // and publish metrics.  Optionally specify a 'basicAllocator'
            // used to supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.

        ~EventHandlerWithCallback();
            // Destroy this request processor.

        // MANIPULATORS
        int handleEvent(int eventId, const bsl::string& eventMessage);
            // Process the event described by the specified 'eventId' and
            // 'eventMessage'.  Return 0 on success, and a non-zero value if
            // there was an error processing the event.

    // ...

    };
//..
// In the implementation of 'EventHandlerWithCallback' below, we ensure that
// the callback is registered on construction and removed before the object is
// destroyed.
//..
    // eventhandlerwithcallback.cpp

    namespace {

    const char *METRIC_CATEGORY = "MyCategory";

    }  // close unnamed namespace

//..
// The callback creates metric records and populates them with data collected
// by the request processor.
//..
    // PRIVATE MANIPULATORS
    void EventHandlerWithCallback::collectMetricsCb(
                                    bsl::vector<balm::MetricRecord> *records,
                                    bool                             resetFlag)
    {
        int numEvents = resetFlag ?
                        (int)d_numEvents.swap(0) :
                        (int)d_numEvents;
        bsls::TimeInterval now         = bdlt::CurrentTime::now();
        bsls::TimeInterval elapsedTime = now - d_periodStart;
        d_periodStart = now;

        balm::MetricRecord record(d_eventsPerSecId);
        record.count() = 1;
        record.total() = numEvents / elapsedTime.totalSecondsAsDouble();

        records->push_back(record);
    }

//..
// In the constructor, we initialize a metric id from the specified 'manager'
// object's metric registry.  We will also register the collection callback
// ('collectMetricsCb') with the supplied 'manager'.
//..
    // CREATORS
    EventHandlerWithCallback::EventHandlerWithCallback(
                                          balm::MetricsManager *manager,
                                          bslma::Allocator     *basicAllocator)
    : d_numEvents(0)
    , d_periodStart(bdlt::CurrentTime::now())
    , d_eventsPerSecId()
    , d_callbackHandle(balm::MetricsManager::e_INVALID_HANDLE)
    , d_metricsManager_p(manager)
    {
        d_eventsPerSecId = d_metricsManager_p->metricRegistry().getId(
                                           METRIC_CATEGORY, "EventsPerSecond");
//..
// We now register the callback function 'collectMetricsCb' with the metrics
// manager.  We use 'bdlf::BindUtil' to bind the member function to a
// 'bsl::function' matching the
// 'balm::MetricsManager::RecordsCollectionCallback' function prototype.  The
// private data member 'd_callbackHandle' is used to store the
// 'balm::MetricsManager::CallbackHandle' returned for the registered callback;
// we will use this handle later to remove the callback from the metrics
// manager.
//..
        d_callbackHandle =
           d_metricsManager_p->registerCollectionCallback(
              METRIC_CATEGORY,
              bdlf::BindUtil::bind(&EventHandlerWithCallback::collectMetricsCb,
                                    this,
                                    bdlf::PlaceHolders::_1,
                                    bdlf::PlaceHolders::_2));
    }

//..
// In the destructor we used the 'balm::MetricsManager::CallbackHandle', stored
// in 'd_callbackHandle' to remove the callback from the metrics manager.
// This prevents the metrics manager from invoking the callback method on an
// object that has already been destroyed.
//..
    EventHandlerWithCallback::~EventHandlerWithCallback()
    {
        int rc =
               d_metricsManager_p->removeCollectionCallback(d_callbackHandle);
        ASSERT(0 == rc);
    }

    // MANIPULATORS
    int EventHandlerWithCallback::handleEvent(int                eventId,
                                              const bsl::string& eventMessage)
    {
//..
// We increment the atomic integer 'd_numEvents' to keep track of the number
// events handled by the 'handleEvent' method.  If collecting a metric is
// expensive (e.g., metrics requiring a system call to collect), clients should
// test whether the metric is enabled before performing the collection
// operation.
//..
        // We don't test 'd_eventsPerSecId.category()->enabled())' before
        // incrementing 'd_numEvents' because, in this instance, it will not
        // improve performance.
        ++d_numEvents;

    // ...    (Process the request)

        return 0;
     }

    // ...
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    ball::DefaultObserver observer(&bsl::cout);
    ball::LoggerManagerConfiguration configuration;
    ball::LoggerManager& manager =
            ball::LoggerManager::initSingleton(&observer, configuration);

    ball::Severity::Level defaultPassthrough = ball::Severity::e_OFF;
    if (verbose)
        defaultPassthrough = ball::Severity::e_FATAL;
    if (veryVerbose)
        defaultPassthrough = ball::Severity::e_ERROR;
    if (veryVeryVerbose)
        defaultPassthrough = ball::Severity::e_TRACE;

    manager.setDefaultThresholdLevels(ball::Severity::e_OFF,
                                      defaultPassthrough,
                                      ball::Severity::e_OFF,
                                      ball::Severity::e_OFF);

    bslma::TestAllocator testAlloc("Test", veryVeryVeryVerbose);
    bslma::TestAllocator *Z = &testAlloc;
    bslma::TestAllocator defaultAllocator("Default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    bdlt::CurrentTime::now();

    switch (test) { case 0:  // Zero is always the leading case.
      case 26: {
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
// The following examples demonstrate how to configure, collect, and publish
// metrics.
//
///Example 1 - Initialize a 'balm::MetricsManager'
///- - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create and configure a
// 'balm::MetricsManager' that we will use to record and publish metric values.
// We first create a 'balm::MetricsManager' object and a
// 'SimpleStreamPublisher' object.  Note that,  'SimpleStreamPublisher' is an
// example implementation of the 'balm::Publisher' protocol defined in the
// 'balm_publisher' component, in practice clients can use a standard
// publisher class (e.g., 'balm::StreamPublisher').
//..
//  int main(int argc, char *argv[]) {
//  {

        // ...

        bslma::Allocator   *allocator = bslma::Default::allocator(0);
        balm::MetricsManager manager(allocator);

        bsl::shared_ptr<balm::Publisher> publisher(
                            new (*allocator) SimpleStreamPublisher(bsl::cout),
                            allocator);
        manager.addSpecificPublisher("MyCategory", publisher);
        // ...

//..

///Example 4 - Publishing a metric
///- - - - - - - - - - - - - - - -
// The metrics manager provides a 'publish' operation to publish metrics for a
// category or set of categories.  In this example we will use the metrics
// manager's 'publish' operation to publish metrics for "MyCategory".  We will
// record metrics for "MyCategory" using instances of the 'EventHandler'
// and 'EventHandlerWithCallback' classes (defined above).  This example
// assumes that an instance, 'manager', of the 'balm::MetricsManager' class has
// been initialized as in example 1.  Note that in practice the publish
// operation is normally tied to a scheduling mechanism (e.g., see the
// 'balm::PublicationScheduler').
//..
    EventHandler             eventHandler(&manager);
    EventHandlerWithCallback eventHandlerWithCallback(&manager);

    eventHandler.handleEvent(0, "A 20 character event message");
    eventHandlerWithCallback.handleEvent(1, "A different event message");
    manager.publishAll();

    eventHandler.handleEvent(0, "A 20 character event message");
    eventHandler.handleEvent(0, "A 20 character event message");
    eventHandlerWithCallback.handleEvent(1, "A different event message");
    manager.publishAll();
//..
// Executing the example should result in two samples being published to
// standard output.  Each sample should contain 3 metrics belonging to
// "MyCategory": "requestsPerSecond" (collected by the
// 'CustomRequestProcessor'), "requestSize", and "failedRequests" (both
// collected by 'RequestProcessor').
//..
// 09FEB2009_18:52:51.093+0000 3 Records
//         Elapsed Time: 0.001358s
//         MyCategory.EventsPerSecond [count = 1, total = 2267.57, ... ]
//         MyCategory.messageSize [count = 1, total = 28, min = 28, max = 28]
//         MyCategory.eventFailure [count = 0, total = 0, ... ]
// 09FEB2009_18:52:51.096+0000 3 Records
//         Elapsed Time: 0.002217s
//         MyCategory.EventsPerSecond [count = 1, total = 453.721, ... ]
//         MyCategory.messageSize [count = 2, total = 56, min = 28, max = 28]
//         MyCategory.eventFailure [count = 0, total = 0, ... ]
//..

      } break;
      case 25: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST
        //
        // Testing:
        //: o Thread-safety of 'balm::MetricsManager' operations.
        //: o publish() is invoked outside the scope of the
        //    'balm::MetricsManager' lock.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST CONCURRENCY" << endl
                                  << "================" << endl;

        bslma::TestAllocator defaultAllocator;
        bslma::DefaultAllocatorGuard dag(&defaultAllocator);

        bslma::TestAllocator testAllocator;
        {
            balm::MetricsManager manager(&testAllocator);
            {
                ConcurrencyTest tester(10, &manager, &defaultAllocator);
                tester.runTest();
            }
        }

        {
            bslmt::Mutex lock;
            bsl::shared_ptr<balm::Publisher> publisher
                (new (testAllocator) LockingPublisher(&lock),
                 &testAllocator);
            balm::MetricsManager manager(&testAllocator);
            manager.addGeneralPublisher(publisher);

            LockAndModifyWorker worker(&lock, &manager);
            worker.start();
            {
                ConcurrencyTest tester(2, &manager, &defaultAllocator);
                tester.runTest();
            }
            worker.stop();
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING: 'removePublisher'
        //
        // Concerns:
        //     That the alternative 'removePublisher' method behaves correctly.
        //
        // Plan:
        //
        // Testing:
        //   int removePublisher(const bsl::shared_ptr<balm::Publisher> );
        // --------------------------------------------------------------------
        const char *CATEGORIES[] = { "A", "B", "C", "D" };
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);

        TestPublisher gtp(Z), tp1(Z), tp2(Z);
        TestPublisher *TEST_PUBS[] = { &tp1, &tp2 };
        const int NUM_PUBS = sizeof(TEST_PUBS) / sizeof(*TEST_PUBS);

        PubPtr gtpSPtr(&gtp, bslstl::SharedPtrNilDeleter(), 0);
        PubPtr tp1SPtr(&tp1, bslstl::SharedPtrNilDeleter(), 0);
        PubPtr tp2SPtr(&tp2, bslstl::SharedPtrNilDeleter(), 0);

        Obj mX(Z); const Obj& MX = mX;

        bsl::vector<balm::Publisher *> publisherVec(Z);

        ASSERT(0 == MX.findGeneralPublishers(&publisherVec));
        mX.addGeneralPublisher(gtpSPtr);
        ASSERT(1 == MX.findGeneralPublishers(&publisherVec));
        ASSERT(&gtp == publisherVec[0]);

        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const char *CATEGORY = CATEGORIES[i];
            publisherVec.clear();
            ASSERT(0 == mX.addSpecificPublisher(CATEGORY, tp1SPtr));
            ASSERT(0 == mX.addSpecificPublisher(CATEGORY, tp2SPtr));
            ASSERT(2 == MX.findSpecificPublishers(&publisherVec, CATEGORY));
        }
        publisherVec.clear();
        ASSERT(0 != mX.removePublisher(bsl::shared_ptr<balm::Publisher>()));
        ASSERT(0 == mX.removePublisher(gtpSPtr));
        ASSERT(0 != mX.removePublisher(gtpSPtr));
        ASSERT(0 == MX.findGeneralPublishers(&publisherVec));

        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const char *CATEGORY = CATEGORIES[i];
            ASSERT(2 == MX.findSpecificPublishers(&publisherVec, CATEGORY));
        }

        ASSERT(0 == mX.removePublisher(tp1SPtr));
        ASSERT(0 != mX.removePublisher(tp1SPtr));

        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            publisherVec.clear();
            const char *CATEGORY = CATEGORIES[i];
            ASSERT(1 == MX.findSpecificPublishers(&publisherVec, CATEGORY));
        }

        ASSERT(0 == mX.removePublisher(tp2SPtr));
        ASSERT(0 != mX.removePublisher(tp2SPtr));

        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            publisherVec.clear();
            const char *CATEGORY = CATEGORIES[i];
            ASSERT(0 == MX.findSpecificPublishers(&publisherVec, CATEGORY));
        }
      };
      case 23: {
        // --------------------------------------------------------------------
        // TESTING: 'publish' with 'resetFlag'
        //
        // Concerns:
        //   That 'publish' resets collectors and callback functions when
        //   'resetFlag' is 'true'.
        //
        // Plan:
        //    Register a set of callbacks and collectors and invoke
        //    publishAll.  Verify that the callbacks and collectors were not
        //    reset (previous tests verified if they were reset).
        //
        // Testing:
        //   void publishAll(false);
        // --------------------------------------------------------------------

        const char *CATEGORIES[]   = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);

        const char *METRICS[]   = { "A", "B", "C", "MyMetric", "90123metric" };
        const int   NUM_METRICS = sizeof (METRICS) / sizeof (*METRICS);

        if (veryVerbose) cout << "\tTest 'publish' with 'resetFlag' 'false'"
                              << endl;

        Obj mX(Z); const Obj& MX = mX;
        Repository& rep = mX.collectorRepository();
        Registry&   reg = mX.metricRegistry();

        bsl::vector<bsl::shared_ptr<TestCallback> > callbacks(Z);
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            bsl::shared_ptr<TestCallback> cb;
            cb.createInplace(Z, reg.getId(CATEGORIES[i], "CB"), Z);
            callbacks.push_back(cb);
            mX.registerCollectionCallback(CATEGORIES[i], cb->function());
            for (int j = 0; j < NUM_METRICS; ++j) {
                rep.getDefaultCollector(CATEGORIES[i], METRICS[j])->update(1);
            }
        }
        mX.publishAll(false);
        for (int i = 0; i < callbacks.size(); ++i) {
            ASSERT(!callbacks[i]->resetFlag());
        }
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            for (int j = 0; j < NUM_METRICS; ++j) {
                balm::MetricRecord rec;
                rep.getDefaultCollector(CATEGORIES[i], METRICS[j])->load(&rec);
                ASSERT(1 == rec.count());
                ASSERT(1 == rec.total());
                ASSERT(1 == rec.min());
                ASSERT(1 == rec.max());
            }
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING: 'collectSample'
        //
        // Concerns:
        //   That 'collectSample' collects metric records and build a metric
        //   sample from the correct categories and respects the supplied
        //   'resetFlag'.
        //
        // Plan:
        //   Initialize a series of collectors and invoke 'collectSample' for
        //   all-categories and verify that the correct sample is collected
        //   and none of the collectors are set.  Then call 'collectSample'
        //   with 'resetFlag' as 'true' and verify the collectors or reset.
        //   Then, for each potential combination of categories, perform a
        //   similar test collecting a sample 'resetFlag' as 'false' and
        //   'true'.
        //
        // Testing:
        //   void collectSample(balm::MetricSample              *,
        //                      bsl::vector<balm::MetricRecord> *,
        //                      bool                            );
        //   void collectSample(balm::MetricSample              *,
        //                      bsl::vector<balm::MetricRecord> *,
        //                      const balm::Category            **,
        //                      int                             ,
        //                      bool                            );
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING: collectSample" << endl
                          << "======================" << endl;

        const char *CATEGORIES[]   = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);

        const char *METRICS[]   = { "A", "B", "C", "MyMetric", "90123metric" };
        const int   NUM_METRICS = sizeof (METRICS) / sizeof (*METRICS);

        if (veryVerbose) cout << "\tTest collectSample() for all categories"
                              << endl;
        {
            Obj mX(Z); const Obj& MX = mX;
            Repository& rep = mX.collectorRepository();
            Registry&   reg = mX.metricRegistry();

            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    rep.getDefaultCollector(CATEGORIES[i],
                                            METRICS[j])->update(1);
                }
            }

            bsls::TimeInterval start = bdlt::CurrentTime::now();
            bslmt::ThreadUtil::microSleep(100000, 0);

            bsl::vector<balm::MetricRecord> records(Z);
            balm::MetricSample sample;

            if (veryVerbose) cout << "\t\twithout reset" << endl;
            mX.collectSample(&sample, &records, false);

            bsls::TimeInterval window = bdlt::CurrentTime::now() - start;
            bdlt::Datetime     now    = bdlt::CurrentTime::utc();
            ASSERT(NUM_CATEGORIES * NUM_METRICS == records.size());
            ASSERT(NUM_CATEGORIES * NUM_METRICS == sample.numRecords());
            ASSERT(NUM_CATEGORIES               == sample.numGroups());
            ASSERT(withinWindow(sample.timeStamp(), now, 10));
            for (int i = 0; i < sample.numGroups(); ++i) {
                const balm::MetricSampleGroup& group = sample.sampleGroup(i);
                ASSERT(withinWindow(group.elapsedTime(), window, 10));
                ASSERT(NUM_METRICS == group.numRecords());

                const balm::MetricRecord *record = group.records();
                const char *name = record->metricId().categoryName();
                for (; record < group.records()+group.numRecords(); ++record) {
                    ASSERT(0 == bsl::strcmp(record->metricId().categoryName(),
                                            name));
                    ASSERT(1 == record->count());
                    ASSERT(1 == record->total());
                    ASSERT(1 == record->min());
                    ASSERT(1 == record->max());
                }
            }
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    balm::MetricRecord record;
                    Collector *col = rep.getDefaultCollector(CATEGORIES[i],
                                                             METRICS[j]);
                    col->load(&record);
                    ASSERT(1 == record.count());
                    ASSERT(1 == record.total());
                    ASSERT(1 == record.min());
                    ASSERT(1 == record.max());
                }
            }

            if (veryVerbose) cout << "\t\twith reset" << endl;
            sample.removeAllRecords();
            bsl::vector<balm::MetricRecord> records2(Z);

            mX.collectSample(&sample, &records2, true);
            ASSERT(NUM_CATEGORIES * NUM_METRICS == records2.size());
            ASSERT(NUM_CATEGORIES * NUM_METRICS == sample.numRecords());
            ASSERT(NUM_CATEGORIES               == sample.numGroups());

            ASSERT(records == records2);
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    balm::MetricRecord record;
                    Collector *col = rep.getDefaultCollector(CATEGORIES[i],
                                                             METRICS[j]);
                    col->load(&record);
                    ASSERT(balm::MetricRecord(record.metricId()) == record);
                }
            }
        }

        Obj mX(Z); const Obj& MX = mX;
        bsl::vector<const Category *> allCategories(Z);

        Repository& rep = mX.collectorRepository();
        Registry&   reg = mX.metricRegistry();
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            allCategories.push_back(reg.getCategory(CATEGORIES[i]));
        }

        if (veryVerbose) cout << "\tTest collectSample() for some categories"
                              << endl;
        CombinationIterator<const Category *> combIt(allCategories, Z);
        do {
            // Set all the 'balm::Collector' objects to a known state and set
            // the 'enabled' state of each category.
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    Collector *col = rep.getDefaultCollector(CATEGORIES[i],
                                                             METRICS[j]);
                    col->reset(); col->update(1);
                }
            }

            bsl::vector<balm::MetricRecord> records(Z);
            balm::MetricSample              sample;

            // Test without a reset.
            const bsl::vector<const Category *>& cats = combIt.current();
            mX.collectSample(&sample,
                             &records,
                             cats.data(),
                             cats.size(),
                             false);
            ASSERT(NUM_METRICS * cats.size() == sample.numRecords());
            ASSERT(cats.size()               == sample.numGroups());
            for (int i = 0; i < NUM_CATEGORIES; ++i ) {
                // Verify the correct categories are in the sample (once)
                const Category *CATEGORY = allCategories[i];
                int   found = 0;
                for (int j = 0; j < sample.numGroups(); ++j) {
                    if (CATEGORY == firstCategory(sample.sampleGroup(j))) {
                        ++found;
                    }
                }
                ASSERT(found < 2);
                ASSERT(static_cast<bool>(found)== combIt.includesElement(i));
            }
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    balm::MetricRecord record;
                    Collector *col = rep.getDefaultCollector(CATEGORIES[i],
                                                             METRICS[j]);
                    col->load(&record);
                    ASSERT(1 == record.count());
                    ASSERT(1 == record.total());
                    ASSERT(1 == record.min());
                    ASSERT(1 == record.max());

                }
            }
            bsl::vector<balm::MetricRecord> records2(Z);
            sample.removeAllRecords();

            // Test with a reset.
            mX.collectSample(&sample,
                             &records2,
                             cats.data(),
                             cats.size(),
                             true);
            ASSERT(NUM_METRICS * cats.size() == sample.numRecords());
            ASSERT(cats.size()               == sample.numGroups());
            ASSERT(records                   == records2);
            for (int i = 0; i < NUM_CATEGORIES; ++i ) {
                // Verify the correct categories are in the sample (once)
                const Category *CATEGORY = allCategories[i];
                int   found = 0;
                for (int j = 0; j < sample.numGroups(); ++j) {
                    if (CATEGORY == firstCategory(sample.sampleGroup(j))) {
                        ++found;
                    }
                }
                ASSERT(found < 2);
                ASSERT(static_cast<bool>(found) == combIt.includesElement(i));
            }
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    balm::MetricRecord record;
                    Collector *col = rep.getDefaultCollector(CATEGORIES[i],
                                                             METRICS[j]);
                    col->load(&record);
                    if (combIt.includesElement(i)) {
                        ASSERT(
                              balm::MetricRecord(record.metricId()) == record);
                    }
                    else {
                        ASSERT(1 == record.count());
                        ASSERT(1 == record.total());
                        ASSERT(1 == record.min());
                        ASSERT(1 == record.max());
                    }
                }
            }

        } while (combIt.next());

    } break;
    case 21: {
        // --------------------------------------------------------------------
        // BSLMA ALLOCATION EXCEPTION TEST: 'publish'
        //
        // Concerns:
        //   That 'publish' is exception safe with respect to allocation.
        //
        // Plan:
        //   Use the 'BSLMA_EXCEPTION_TEST' to verify the manipulator methods
        //   of this object are exception neutral.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BSLMA EXCEPTION TEST: publish" << endl
                          << "=============================" << endl;

        const char *CATEGORIES[] = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);
        const char *METRICS[] = { "A", "B", "C", "MyMetric" "90123metric" };
        const int   NUM_METRICS = sizeof (METRICS) / sizeof (*METRICS);

        typedef bsl::shared_ptr<TestCallback> CbPtr;
        typedef bsl::pair<CbHandle, CbPtr>    CallbackInfo;
        typedef bsl::map<Id, CallbackInfo>    CallbackMap;

        {
            bslma::TestAllocator testAllocator;
            Obj mX(&testAllocator); const Obj& MX = mX;
            TestPublisher tp(Z), tp1(Z), tp2(Z);
            PubPtr pub_p(&tp, bslstl::SharedPtrNilDeleter(), Z);
            PubPtr spub_p1(&tp1, bslstl::SharedPtrNilDeleter(), Z);
            PubPtr spub_p2(&tp2, bslstl::SharedPtrNilDeleter(), Z);
            mX.addGeneralPublisher(pub_p);
            mX.addSpecificPublisher(CATEGORIES[0], spub_p1);
            mX.addSpecificPublisher(CATEGORIES[0], spub_p2);

            // Add 'TestCallback' functors to the metrics manager and store the
            // returned 'CallbackHandle'.
            CallbackMap callbacks(Z);
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                const char *CATEGORY = CATEGORIES[i];
                for (int j = 0; j < NUM_METRICS; ++j) {
                    Id id = mX.metricRegistry().getId(CATEGORY, METRICS[j]);
                    CbPtr cb; cb.createInplace(Z, id, Z);
                    CbHandle handle =
                       mX.registerCollectionCallback(CATEGORY, cb->function());
                    callbacks[id] = bsl::make_pair(handle, cb);
                    ASSERT(handle != Obj::e_INVALID_HANDLE);
                    mX.collectorRepository().getDefaultCollector(CATEGORY,
                                                                 METRICS[j]);
                }
            }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                bslma::DefaultAllocatorGuard guard(&testAllocator);
                tp.reset();
                mX.publishAll();
                ASSERT(1 == tp.invocations());
                mX.publishAll();
                ASSERT(2 == tp.invocations());
                mX.publishAll();
                ASSERT(3 == tp.invocations());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

    } break;
    case 20: {
        // --------------------------------------------------------------------
        // BSLMA ALLOCATION EXCEPTION TEST: 'registerCollectionCallback'
        //
        // Concerns:
        //   That 'registerCollectionCallback' is exception safe with respect
        //   to allocation.
        //
        // Plan:
        //   Use the 'BSLMA_EXCEPTION_TEST' to verify the manipulator methods
        //   of this object are exception neutral.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BSLMA EXCEPTION TEST: register callback" << endl
                          << "=======================================" << endl;

        const char *CATEGORIES[] = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);
        const char *METRICS[] = { "A", "B", "C", "MyMetric" "90123metric" };
        const int NUM_METRICS = sizeof (METRICS) / sizeof (*METRICS);

        // Create a map, from metric id to 'TestCallback' functor.
        typedef bsl::shared_ptr<TestCallback>      CbPtr;

        {
            bslma::TestAllocator testAllocator;
            Obj mX(&testAllocator); const Obj& MX = mX;
            TestPublisher tp(Z);
            PubPtr pub_p(&tp, bslstl::SharedPtrNilDeleter(), Z);
            mX.addGeneralPublisher(pub_p);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                for (int i = 0; i < NUM_CATEGORIES; ++i) {
                    for (int j = 0; j < NUM_METRICS; ++j) {
                        const char *CAT = CATEGORIES[i];
                        Id id = mX.metricRegistry().getId(CAT, METRICS[j]);
                        CbPtr cb; cb.createInplace(Z, id, Z);
                        CbHandle handle =
                            mX.registerCollectionCallback(CAT, cb->function());
                        ASSERT(handle != Obj::e_INVALID_HANDLE);
                        ASSERT(0 == mX.removeCollectionCallback(handle));
                    }
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            mX.publishAll();
            ASSERT(0 == tp.invocations());
        }
    } break;
    case 19: {
        // --------------------------------------------------------------------
        // BSLMA ALLOCATION EXCEPTION TEST: add publisher
        //
        // Concerns:
        //   That 'addGeneralPublisher' and 'addSpecificPublisher' are
        //   exception safe with respect to allocation.
        //
        // Plan:
        //   Use the 'BSLMA_EXCEPTION_TEST' to verify the manipulator methods
        //   of this object are exception neutral.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BSLMA EXCEPTION TEST: publishers" << endl
                          << "================================" << endl;

        const char *CATEGORIES[] = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);

        TestPublisher tp1(Z), tp2(Z), tp3(Z), tp4(Z);
        TestPublisher *TEST_PUBS[] = { &tp1, &tp2, &tp3, &tp4 };
        const int NUM_PUBS = sizeof(TEST_PUBS)/sizeof(*TEST_PUBS);
        bsl::set<balm::Publisher *> generalPublishers;
        {
            bslma::TestAllocator testAllocator;
            Obj mX(&testAllocator); const Obj& MX = mX;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                for (int i = 0; i < NUM_PUBS; ++i) {
                    // Verify that the set of general publishers is correct
                    bsl::vector<balm::Publisher *> publishers;
                    ASSERT(generalPublishers.size() ==
                           MX.findGeneralPublishers(&publishers));
                    for (int j = 0; j < publishers.size(); ++j) {
                        ASSERT(0 != publishers[j]);
                    }

                    // Verify that the publisher can be removed.
                    PubPtr pub_p(TEST_PUBS[i],
                                 bslstl::SharedPtrNilDeleter(),
                                 Z);
                    mX.removePublisher(pub_p);

                    for (int j = 0; j < NUM_CATEGORIES; ++j) {
                        const char *CATEGORY = CATEGORIES[j];
                        ASSERT(0 == mX.addSpecificPublisher(CATEGORY, pub_p));
                        ASSERT(0 != mX.addGeneralPublisher(pub_p));
                    }
                    mX.removePublisher(pub_p);
                    mX.addGeneralPublisher(pub_p);
                    generalPublishers.insert(pub_p.get());
                }

                for (int i = 0; i < NUM_CATEGORIES; ++i) {
                    bsl::vector<balm::Publisher *> publishers;
                    ASSERT(0 == MX.findSpecificPublishers(&publishers,
                                                          CATEGORIES[i]));
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

    } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING: 'addSpecificPublishers(const bslstl::StringRef&, ...)'
        //
        // Concerns:
        //   That the alternative 'addSpecificPublishers' variant (taking a
        //   string) is logically equivalent to the previously tested
        //   primary 'addSpecificPublishers' variant (taking a
        //   'balm::Category *').
        //
        // Plan:
        //   For each category in a set of unique categories, add a series of
        //   'balm::Publisher' objects.  Then, for each category, invoke
        //   'findSpecificPublishers' and verify the publishers are found.
        //
        // Testing:
        //   int addSpecificPublishers(bsl::vector<balm::Publisher *> *,
        //                              const char *        ) const;
        // --------------------------------------------------------------------
        if (verbose) cout
            << endl
            << "TEST: addSpecificPublishers(const bslstl::StringRef&, ...)\n"
            << "========================================================\n";

        const char *CATEGORIES[] = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);
        const int NUM_PUBLISHERS = 4;

        bsl::multimap<const char *, PubPtr> publishers(Z);

        Obj mX(Z); const Obj& MX = mX;
        Registry& registry = mX.metricRegistry();
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const char *CATEGORY = CATEGORIES[i];
            for (int j = 0; j < NUM_PUBLISHERS; ++j) {
                PubPtr pub_p(new (*Z) TestPublisher(Z), Z);
                mX.addSpecificPublisher(CATEGORIES[i], pub_p);
                publishers.insert(bsl::make_pair(CATEGORIES[i], pub_p));
            }
        }

        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const char *CATEGORY = CATEGORIES[i];
            const Category *CAT = mX.metricRegistry().getCategory(CATEGORY);
            bsl::vector<balm::Publisher *> results(Z);
            ASSERT(NUM_PUBLISHERS == MX.findSpecificPublishers(&results, CAT));

            bsl::vector<balm::Publisher *>::const_iterator expIt =
                                              results.begin();
            bsl::multimap<const char *, PubPtr>::const_iterator it =
                                             publishers.lower_bound(CATEGORY);
            bsl::multimap<const char *, PubPtr>::const_iterator endIt =
                                             publishers.upper_bound(CATEGORY);

            for (; it != endIt; ++it, ++expIt) {
                ASSERT(*expIt == it->second.get());
            }
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING: 'findSpecificPublishers(..., const bslstl::StringRef&)'
        //
        // Concerns:
        //   That the alternative 'findSpecificPublishers' variant (taking a
        //   string) is logically equivalent to the previously tested
        //   primary 'findSpecificPublishers' variant (taking a
        //   'balm::Category *').
        //
        // Plan:
        //   For each category in a set of unique categories, add a series of
        //   'balm::Publisher' objects.  Then, for each category, invoke the
        //   two 'findSpecificPublishers' variants and verify the results are
        //   equivalent.
        //
        // Testing:
        //   int findSpecificPublishers(bsl::vector<balm::Publisher *> *,
        //                              const char *                  ) const;
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "TEST: findSpecificPublishers(...,const bslstl::StringRef&)\n"
            << "==========================================================\n";

        const char *CATEGORIES[] = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);

        const int NUM_PUBLISHERS = 4;
        Obj mX(Z); const Obj& MX = mX;
        Registry& registry = mX.metricRegistry();

        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const char *CATEGORY = CATEGORIES[i];
            const Category *CAT = mX.metricRegistry().getCategory(CATEGORY);
            for (int j = 0; j < NUM_PUBLISHERS; ++j) {
                PubPtr pub_p(new (*Z) TestPublisher(Z), Z);
                mX.addSpecificPublisher(CAT, pub_p);
            }
        }

        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const char *CATEGORY = CATEGORIES[i];
            const Category *CAT = mX.metricRegistry().getCategory(CATEGORY);
            bsl::vector<balm::Publisher *> exp_results(Z);
            bsl::vector<balm::Publisher *> results(Z);

            ASSERT(NUM_PUBLISHERS == MX.findSpecificPublishers(&exp_results,
                                                               CAT));
            ASSERT(NUM_PUBLISHERS == MX.findSpecificPublishers(&results,
                                                               CATEGORY));
            ASSERT(exp_results == results);
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING: 'registerCollectionCallback(const bslstl::StringRef&, ...)'
        //
        // Concerns:
        //   That the alternative 'registerCollectionCallback' variant (taking
        //   a string) is logically equivalent to the primary variant (taking a
        //   'balm::Category *').
        //
        // Plan:
        //   Duplicate the original 'registerCollectionCallback' test (case 6),
        //   but invoke 'registerCollectionCallback' with a string.
        //
        // Testing:
        //    CallbackHandle registerCollectionCallback(
        //                                        const char *
        //                                        RecordsCollectionCallback);
        //    int removeCollectionCallback(CallbackHandle handle);
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            <<   "TEST: void registerCollectionCallback(const char *, ...)"
            << "\n========================================================\n";

        const char *CATEGORIES[] = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);
        const char *METRICS[] = { "A", "B", "C", "MyMetric" "90123metric" };
        const int NUM_METRICS = sizeof (METRICS) / sizeof (*METRICS);

        // Create a map, from metric id to 'TestCallback' functor.
        typedef bsl::shared_ptr<TestCallback> CbPtr;
        typedef bsl::pair<CbHandle, CbPtr>    CallbackInfo;
        typedef bsl::map<Id, CallbackInfo>    CallbackMap;

        // Add 'TestCallback' functors to the metrics manager and store the
        // returned 'CallbackHandle'.
        CallbackMap callbacks(Z);
        Obj mX(Z); const Obj& MX = mX;
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            for (int j = 0; j < NUM_METRICS; ++j) {
                Id id = mX.metricRegistry().getId(CATEGORIES[i], METRICS[j]);

                CbPtr cb; cb.createInplace(Z, id, Z);
                CbHandle handle =
                  mX.registerCollectionCallback(CATEGORIES[i], cb->function());
                callbacks[id] = bsl::make_pair(handle, cb);
                ASSERT(handle != Obj::e_INVALID_HANDLE);
            }
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());

        // Invoke 'publish' and verify the 'TestCallbacks' in the published
        // category (and only those callbacks) are invoked once.
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const char *CATEGORY = CATEGORIES[i];
            const Category *CAT = mX.metricRegistry().getCategory(CATEGORY);

            mX.publish(CAT);

            CallbackMap::iterator it = callbacks.begin();
            for (; it != callbacks.end(); ++it) {
                TestCallback& cb = *it->second.second;
                const int EXP = (CAT == cb.metricId().category()) ? 1 : 0;
                ASSERT(EXP == cb.invocations());
                cb.reset();
            }
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());

        // Remove each 'TestCallback' functor and invoke publish to verify it
        // is no longer invoked when the category is published'.
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const char *CATEGORY = CATEGORIES[i];
            for (int j = 0; j < NUM_METRICS; ++j) {
                const char *METRIC = METRICS[j];

                Id id = mX.metricRegistry().getId(CATEGORY, METRIC);
                CallbackInfo& cbInfo = callbacks[id];
                ASSERT(id == cbInfo.second->metricId());

                ASSERT(0 == mX.removeCollectionCallback(cbInfo.first));
                ASSERT(0 != mX.removeCollectionCallback(cbInfo.first));

                cbInfo.first = Obj::e_INVALID_HANDLE;

                mX.publishAll();

                CallbackMap::iterator it = callbacks.begin();
                for (; it != callbacks.end(); ++it) {
                    CallbackInfo& info = it->second;
                    const int EXP = (info.first == Obj::e_INVALID_HANDLE)
                                    ? 0 : 1;
                    ASSERT(EXP == info.second->invocations());
                    info.second->reset();
                }
            }
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING: 'setCategoryEnabled(bool )' , 'publish'
        //
        // Concerns:
        //   That 'setCategoryEnabled' method sets the enabled status of the
        //   supplied category.
        //
        // Plan:
        // Testing:
        //   void setCategoryEnabled(const balm::Category *, bool );
        //   void setCategoryEnabled(const char *, bool );
        //   void setAllCategoriesEnabled(bool );
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "TEST: void setCategoryEnabled(bool );\n"
            << "=====================================\n";

        const char *CATEGORIES[] = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);

        Obj mX(Z); const Obj& MX = mX;
        Registry& registry = mX.metricRegistry();

        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const Category *CAT = registry.getCategory(CATEGORIES[i]);
            ASSERT(CAT->enabled());
            mX.setCategoryEnabled(CAT, false);
            ASSERT(!CAT->enabled());
            mX.setCategoryEnabled(CAT, true);
            ASSERT(CAT->enabled());

            mX.setCategoryEnabled(CATEGORIES[i], false);
            ASSERT(!CAT->enabled());
            mX.setCategoryEnabled(CATEGORIES[i], true);
            ASSERT(CAT->enabled());
        }

        mX.setAllCategoriesEnabled(false);
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            ASSERT(!registry.getCategory(CATEGORIES[i])->enabled());
        }

        mX.setAllCategoriesEnabled(true);
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            ASSERT(registry.getCategory(CATEGORIES[i])->enabled());
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // AUXILIARY TESTING: 'publish' publishes only enabled categories
        //
        // Concerns:
        //   That the publish methods do not publish categories which are not
        //   enabled.
        //
        // Plan:
        //   Create a set of category and metric names.
        //
        //   Add a "general" publisher, and a "specific" publisher for each
        //   category.
        //
        //   Perform a test for each possible combination of categories.  On
        //   each iteration:
        //   1. Set all the 'balm::Collector' objects to a known state, and set
        //      those categories in the test set to be enabled, and set all
        //      other categories to be disabled.
        //   2. Invoke 'publishAll'.
        //   3. Verify that the general publisher has been invoked, and the
        //      expected "specific" publishers have been invoked.
        //
        // Testing:
        //   void publishAll(const bsl::set<const balm::Category *>& ,
        //                   const bsls::TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "TEST: 'publish' publishes only enabled categories\n"
            << "=================================================\n";

        const char *CATEGORIES[] = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);

        const char *METRICS[] = { "A", "B", "C", "MyMetric", "903metric" };
        const int   NUM_METRICS = sizeof (METRICS) / sizeof (*METRICS);

        Obj mX(Z); const Obj& MX = mX;
        Registry& registry     = mX.metricRegistry();
        Repository& repository = mX.collectorRepository();

        // Create a "general" publisher.
        TestPublisher gPub(Z);
        PubPtr gPub_p(&gPub, bslstl::SharedPtrNilDeleter(), Z);
        mX.addGeneralPublisher(gPub_p);

        // Create a "specific" publisher for each category.
        bsl::vector<const Category *> allCategories(Z);
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const Category *CAT = registry.getCategory(CATEGORIES[i]);
            PubPtr pub_p(new (*Z) TestPublisher(Z), Z);
            mX.addSpecificPublisher(CAT, pub_p);
            allCategories.push_back(CAT);
        }

        // Perform a test iteration for each *combination* of categories from
        // the set of test categories.  On each iteration set those
        // categories (an only those categories) not in the set of test
        // categories to be disabled.
        CombinationIterator<const Category *> combIt(allCategories, Z);
        do {
            // Set all the 'balm::Collector' objects to a known state and set
            // the 'enabled' state of each category.
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    Collector *col =
                                 repository.getDefaultCollector(CATEGORIES[i],
                                                                METRICS[j]);
                    col->reset(); col->update(1);
                }
                registry.setCategoryEnabled(allCategories[i],
                                            combIt.includesElement(i));
            }

            // Publish the records.
            mX.publishAll();

            // Verify the "general" publishers has been invoked.
            const int EXP_INV = combIt.current().empty() ? 0 : 1;
            ASSERT(EXP_INV == gPub.invocations())

            // Verify the correct "specific" publishers have been invoked.
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    Id id  = registry.getId(CATEGORIES[i], METRICS[j]);
                    ASSERT(combIt.includesElement(i) == gPub.contains(id));
                }

                const int EXP_INV = combIt.includesElement(i) ? 1 : 0;
                bsl::vector<balm::Publisher *> sPublishers(Z);
                mX.findSpecificPublishers(&sPublishers, allCategories[i]);
                TestPublisher *sPub_p = (TestPublisher *)sPublishers.front();
                ASSERT(EXP_INV == sPub_p->invocations());
                sPub_p->reset();

            }
            gPub.reset();

        } while (combIt.next());
        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // AUXILIARY TESTING: 'publish' publishes the correct elapsed time
        //
        // Concerns:
        //   That the 'publish' operations publish a 'balm::MetricSample' that
        //   is grouped correctly with the correct elapsed time values.
        // Plan:
        //   Create a set of category and metric names.
        //
        //   Add a "general" publisher, and create a "oracle" mapping of
        //   publication times from which to generated the expected elapsed
        //   times.
        //
        //   Perform a test for each possible combination of categories.  On
        //   each iteration:
        //   1. Set all the 'balm::Collector' objects to a known state, and set
        //      those categories in the test set to be enabled, and set all
        //      other categories to be disabled.
        //   2. Invoke 'publishAll'.
        //   3. Verify that the general publisher has been invoked, and the
        //      expected "specific" publishers have been invoked.
        //
        // Testing:
        //   void publish*(...);
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "TEST: 'publish' publishes the correct elapsed tim\n"
            << "=================================================\n";

        const char *CATEGORIES[] = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);

        const char *METRICS[] = { "A", "B", "C", "MyMetric", "903metric" };
        const int   NUM_METRICS = sizeof (METRICS) / sizeof (*METRICS);

        const int TIME_UNIT = 50 * NANOSECS_PER_MILLISEC; // 50ms

        bsls::TimeInterval creationTime = bdlt::CurrentTime::now();

        Obj mX(Z); const Obj& MX = mX;
        Registry& registry     = mX.metricRegistry();
        Repository& repository = mX.collectorRepository();

        // Create a "general" publisher.
        TestPublisher gPub(Z);
        PubPtr gPub_p(&gPub, bslstl::SharedPtrNilDeleter(), Z);
        mX.addGeneralPublisher(gPub_p);

        bsl::map<const Category *, bsls::TimeInterval> lastPublicationTimes(Z);
        bsl::vector<const Category *> allCategories(Z);
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const Category *CAT = registry.getCategory(CATEGORIES[i]);
            allCategories.push_back(CAT);
            lastPublicationTimes[CAT] = creationTime;
        }

        // Perform a test iteration for each *combination* of categories from
        // the set of test categories.  On each iteration set those
        // categories (an only those categories) not in the set of test
        // categories to be disabled.
        CombinationIterator<const Category *> combIt(allCategories, Z);
        do {
            // Set all the 'balm::Collector' objects to a known state and set
            // the 'enabled' state of each category.
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    Collector *col =
                                 repository.getDefaultCollector(CATEGORIES[i],
                                                                METRICS[j]);
                    col->reset(); col->update(1);
                }
                registry.setCategoryEnabled(allCategories[i],
                                            combIt.includesElement(i));
            }

            // Publish the records.
            bslmt::ThreadUtil::sleep(bsls::TimeInterval(0, TIME_UNIT));
            bsls::TimeInterval publicationTime = bdlt::CurrentTime::now();
            mX.publishAll();

            // Verify the "general" publishers has been invoked.
            const int EXP_INV = combIt.current().empty() ? 0 : 1;
            ASSERT(EXP_INV == gPub.invocations());

            const balm::MetricSample& sample = gPub.lastSample();
            ASSERT(combIt.current().size() == sample.numGroups());
            ASSERT(NUM_METRICS * combIt.current().size() ==
                   sample.numRecords());

            // Verify the correct metrics were published.
            for (int i = 0; i < combIt.current().size(); ++i) {
                const Category *CATEGORY = combIt.current()[i];
                bsls::TimeInterval elapsedTime =
                              publicationTime - lastPublicationTimes[CATEGORY];
                lastPublicationTimes[CATEGORY] = publicationTime;

                int  groupIndex = -1;
                for (int j = 0; j < sample.numGroups(); ++j) {
                    const balm::MetricSampleGroup& group =
                                                         sample.sampleGroup(j);

                    // If 'j' is the sample group for 'CATEGORY' test the
                    // elapsed time.
                    if (CATEGORY == group.records()->metricId().category()) {
                        ASSERT(-1 == groupIndex);
                        ASSERT(withinWindow(group.elapsedTime(),
                                            elapsedTime,
                                            10));
                        groupIndex = j;

                    }
                    for (int k = 0; k < group.numRecords(); ++k) {
                        ASSERT((groupIndex == j) ==
                               (CATEGORY ==
                                group.records()[k].metricId().category()));
                    }
                }
                ASSERT(-1 != groupIndex);
           }

            // Verify the elapsed time
            gPub.reset();

        } while (combIt.next());
        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING: 'publishAll(const bsl::set<const balm::Category *>&, ...)'
        //
        // Concerns:
        //   That 'publishAll' method calls the expected callbacks and invokes
        //   the expected publishers
        //
        // Plan:
        //   Create a set of category and metric names.
        //
        //   Add a "general" publisher, and a "specific" publisher for each
        //   category.
        //
        //   Perform a test for each possible combination of categories.  On
        //   each iteration:
        //   1. Set all the 'balm::Collector' objects to a known state.
        //   2. Create an exclusion set (the inverse of the included
        //      categories) and a unique time interval and the invoke
        //      'publishAll'.
        //   3. Verify that the general publisher has been invoked, and the
        //      expected "specific" publishers have been invoked.
        //
        // Testing:
        //   void publishAll(const bsl::set<const balm::Category *>& ,
        //                   const bsls::TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "TEST: void publishAll(set<const balm::Category *>&,...);\n"
            << "========================================================\n";

        const char *CATEGORIES[] = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);

        const char *METRICS[] = { "A", "B", "C", "MyMetric", "903metric" };
        const int   NUM_METRICS = sizeof (METRICS) / sizeof (*METRICS);

        Obj mX(Z); const Obj& MX = mX;
        Registry& registry     = mX.metricRegistry();
        Repository& repository = mX.collectorRepository();

        // Create a "general" publisher.
        TestPublisher gPub(Z);
        PubPtr gPub_p(&gPub, bslstl::SharedPtrNilDeleter(), Z);
        mX.addGeneralPublisher(gPub_p);

        // Create a "specific" publisher for each category.
        bsl::vector<const Category *> allCategories(Z);
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const Category *CAT = registry.getCategory(CATEGORIES[i]);
            PubPtr pub_p(new (*Z) TestPublisher(Z), Z);
            mX.addSpecificPublisher(CAT, pub_p);
            allCategories.push_back(CAT);
        }

        // Perform a test iteration for each *combination* of categories from
        // the set of test categories.  On each iteration invoke 'publish'
        // with the combination of categories and then verify the callbacks,
        // collectors, publishers were updated as expected.
        CombinationIterator<const Category *> combIt(allCategories, Z);
        do {
            // Set all the 'balm::Collector' objects to a known state.
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    Collector *col =
                        repository.getDefaultCollector(CATEGORIES[i],
                                                       METRICS[j]);
                    col->reset(); col->update(1);
                }
            }

            // Create the exclusion set.
            bsl::set<const balm::Category *> excludedSet(Z);
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                if (!combIt.includesElement(i)) {
                    excludedSet.insert(allCategories[i]);
                }
            }
            ASSERT(allCategories.size() ==
                   excludedSet.size() + combIt.current().size());

            // Publish the records.
            bdlt::Datetime tmStamp = bdlt::CurrentTime::utc();
            mX.publishAll(excludedSet);

            // Verify the "general" publishers has been invoked.
            if (combIt.current().empty()) {
                ASSERT(0 == gPub.invocations())
            }
            else {
                ASSERT(1 == gPub.invocations());
                ASSERT(withinWindow(gPub.lastTimeStamp(), tmStamp, 10));
                ASSERT(combIt.current().size() ==
                       gPub.lastSample().numGroups());
            }

            // Verify the correct "specific" publishers have been invoked.
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    Id id  = registry.getId(CATEGORIES[i], METRICS[j]);
                    ASSERT(combIt.includesElement(i) == gPub.contains(id));
                }

                const int EXP_INV = combIt.includesElement(i) ? 1 : 0;
                bsl::vector<balm::Publisher *> sPublishers(Z);
                mX.findSpecificPublishers(&sPublishers, allCategories[i]);
                TestPublisher *sPub_p = (TestPublisher *)sPublishers.front();
                ASSERT(EXP_INV == sPub_p->invocations());
                sPub_p->reset();

            }
            gPub.reset();
        } while (combIt.next());
        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING: 'publishAll(const bsls::TimeInterval& )'
        //
        // Concerns:
        //   That 'publishAll' method calls the expected callbacks and invokes
        //   the expected publishers
        //
        // Plan:
        //   Create a set of category and metric names.
        //
        //   Add a "general" publisher, and a "specific" publisher for each
        //   category.
        //
        //   Then set the 'balm::Collector' objects for each metric to a known
        //   state and invoke 'publishAll'.
        //
        //   Verify that the "general" publisher has been invoked, and the
        //   expected "specific" publishers have been invoked.
        //
        //
        // Testing:
        //   void publishAll(const bsls::TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "TEST: void publishAll(const bsls::TimeInterval& );\n"
            << "=================================================\n";

        const char *CATEGORIES[] = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);

        const char *METRICS[] = { "A", "B", "C", "MyMetric", "903metric" };
        const int   NUM_METRICS = sizeof (METRICS) / sizeof (*METRICS);

        Obj mX(Z); const Obj& MX = mX;
        Registry& registry     = mX.metricRegistry();
        Repository& repository = mX.collectorRepository();

        // Create a "general" publisher.
        TestPublisher gPub(Z);
        PubPtr gPub_p(&gPub, bslstl::SharedPtrNilDeleter(), Z);
        mX.addGeneralPublisher(gPub_p);

        // Create a "specific" publisher for each category.
        bsl::vector<const Category *> allCategories(Z);
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const Category *CAT = registry.getCategory(CATEGORIES[i]);
            PubPtr pub_p(new (*Z) TestPublisher(Z), Z);
            mX.addSpecificPublisher(CAT, pub_p);
            allCategories.push_back(CAT);
        }

        // Set all the 'balm::Collector' objects to a known state.
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            for (int j = 0; j < NUM_METRICS; ++j) {
                Collector *col =
                    repository.getDefaultCollector(CATEGORIES[i],
                                                   METRICS[j]);
                col->reset(); col->update(1);
            }
        }

        // Invoke 'publishAll'.
        bslmt::ThreadUtil::microSleep(100000, 0);
        bdlt::Datetime tmStamp = bdlt::CurrentTime::utc();
        mX.publishAll();

        // Verify the "general" publisher has been invoked.
        ASSERT(1 == gPub.invocations());
        ASSERT(withinWindow(gPub.lastTimeStamp(), tmStamp, 10));
        ASSERT(gPub.lastElapsedTimes().size() == 1);
        ASSERT(bsls::TimeInterval(0, 0) < *gPub.lastElapsedTimes().begin());

        // Verity the correct "specific" publishers have been invoked.
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            for (int j = 0; j < NUM_METRICS; ++j) {
                Id id  = registry.getId(CATEGORIES[i], METRICS[j]);
                ASSERT(gPub.contains(id));
            }

            bsl::vector<balm::Publisher *> sPublishers(Z);
            mX.findSpecificPublishers(&sPublishers, allCategories[i]);
            TestPublisher *sPub_p = (TestPublisher *)sPublishers.front();
            ASSERT(1 == sPub_p->invocations());
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING: 'publish(const balm::Category *, ...)'
        //
        // Concerns:
        //   That 'publish' method calls the expected callbacks and invokes the
        //   expected publishers
        //
        // Plan:
        //   Create a set of category and metric names.
        //
        //   Add a "general" publisher, and a "specific" publisher for each
        //   category.
        //
        //   For each category:
        //   1. Set all the 'balm::Collector' objects to a known state.
        //   2. Create a unique time interval and invoke 'publish'.
        //   3. Verify that the "general" publisher has been invoked, and the
        //      expected "specific" publishers have been invoked.
        //
        // Testing:
        //   void publish(const balm::Category *, const bsls::TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "TEST: void publish(const balm::Category *, ...);\n"
            << "===============================================\n";

        const char *CATEGORIES[] = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);

        const char *METRICS[] = { "A", "B", "C", "MyMetric", "903metric" };
        const int   NUM_METRICS = sizeof (METRICS) / sizeof (*METRICS);

        Obj mX(Z); const Obj& MX = mX;
        Registry& registry     = mX.metricRegistry();
        Repository& repository = mX.collectorRepository();

        // Create a "general" publisher.
        TestPublisher gPub(Z);
        PubPtr gPub_p(&gPub, bslstl::SharedPtrNilDeleter(), Z);
        mX.addGeneralPublisher(gPub_p);

        // Create a "specific" publisher for each category.
        bsl::vector<const Category *> allCategories(Z);
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const Category *CAT = registry.getCategory(CATEGORIES[i]);
            PubPtr pub_p(new (*Z) TestPublisher(Z), Z);
            mX.addSpecificPublisher(CAT, pub_p);
            allCategories.push_back(CAT);
        }

        // Perform a test iteration for each *combination* of categories from
        // the set of test categories.  On each iteration invoke 'publish'
        // with the combination of categories and then verify the callbacks,
        // collectors, publishers were updated as expected.
        for (int catIdx = 0; catIdx < NUM_CATEGORIES; ++catIdx) {
            const Category *CAT = registry.getCategory(CATEGORIES[catIdx]);

            // Set all the 'balm::Collector' objects to a known state.
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    Collector *col =
                        repository.getDefaultCollector(CATEGORIES[i],
                                                       METRICS[j]);
                    col->reset(); col->update(1);
                }
            }

            // Publish the records.
            bslmt::ThreadUtil::microSleep(100000, 0);
            bdlt::Datetime tmStamp = bdlt::CurrentTime::utc();
            mX.publish(CAT);

            ASSERT(1 == gPub.invocations());
            ASSERT(withinWindow(gPub.lastTimeStamp(), tmStamp, 10));
            ASSERT(gPub.lastElapsedTimes().size() == 1);
            ASSERT(
                  bsls::TimeInterval(0, 0) < *gPub.lastElapsedTimes().begin());

            // Verify the correct "specific" publishers have been invoked.
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    Id id  = registry.getId(CATEGORIES[i], METRICS[j]);
                    ASSERT((i == catIdx) == gPub.contains(id));
                }

                const int EXP_INV = i == catIdx;
                bsl::vector<balm::Publisher *> sPublishers(Z);
                mX.findSpecificPublishers(&sPublishers, allCategories[i]);
                TestPublisher *sPub_p = (TestPublisher *)sPublishers.front();
                ASSERT(EXP_INV == sPub_p->invocations());
                sPub_p->reset();
            }
            gPub.reset();
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING: 'publish(const bsl::set<const balm::Category *>&, ...)'
        //
        // Concerns:
        //   That 'publish' method calls the expected callbacks and invokes the
        //   expected publishers
        //
        // Plan:
        //   Create a set of category and metric names.
        //
        //   Add a "general" publisher, and a "specific" publisher for each
        //   category.
        //
        //   Perform a test for each possible combination of categories.  On
        //   each iteration:
        //   1. Set all the 'balm::Collector' objects to a known state.
        //   2. Create a unique time interval and invoke 'publish'.
        //   3. Verify that the "general" publisher has been invoked, and the
        //      expected "specific" publishers have been invoked.
        //
        // Testing:
        //   void publish(const bsl::set<const balm::Category *>& ,
        //                 const bsls::TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "TEST: void publish(bsl::set<const balm::Category *>&, ...);\n"
            << "===========================================================\n";

        const char *CATEGORIES[] = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);

        const char *METRICS[] = { "A", "B", "C", "MyMetric", "903metric" };
        const int   NUM_METRICS = sizeof (METRICS) / sizeof (*METRICS);

        Obj mX(Z); const Obj& MX = mX;
        Registry& registry     = mX.metricRegistry();
        Repository& repository = mX.collectorRepository();

        // Create a "general" publisher.
        TestPublisher gPub(Z);
        PubPtr gPub_p(&gPub, bslstl::SharedPtrNilDeleter(), Z);
        mX.addGeneralPublisher(gPub_p);

        // Create a "specific" publisher for each category.
        bsl::vector<const Category *> allCategories(Z);
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const Category *CAT = registry.getCategory(CATEGORIES[i]);
            PubPtr pub_p(new (*Z) TestPublisher(Z), Z);
            mX.addSpecificPublisher(CAT, pub_p);
            allCategories.push_back(CAT);
        }

        // Perform a test iteration for each *combination* of categories from
        // the set of test categories.  On each iteration invoke 'publish'
        // with the combination of categories and then verify the callbacks,
        // collectors, publishers were updated as expected.
        CombinationIterator<const Category *> combIt(allCategories, Z);
        do {
            // Set all the 'balm::Collector' objects to a known state.
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    Collector *col =
                        repository.getDefaultCollector(CATEGORIES[i],
                                                       METRICS[j]);
                    col->reset(); col->update(1);
                }
            }
            const bsl::vector<const Category *>& categories = combIt.current();
            bsl::set<const balm::Category *> categorySet(
                                       categories.begin(), categories.end(),
                                       bsl::less<const balm::Category *>(), Z);

            // Publish the records.
            bdlt::Datetime tmStamp = bdlt::CurrentTime::utc();
            mX.publish(categorySet);

            // Verify the "general" publishers has been invoked.
            if (categorySet.empty()) {
                ASSERT(0 == gPub.invocations())
            }
            else {
                ASSERT(1 == gPub.invocations());
                ASSERT(withinWindow(gPub.lastTimeStamp(), tmStamp, 10));
                ASSERT(categorySet.size() == gPub.lastSample().numGroups());
            }

            // Verify the correct "specific" publishers have been invoked.
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    Id id  = registry.getId(CATEGORIES[i], METRICS[j]);
                    ASSERT(combIt.includesElement(i) == gPub.contains(id));
                }

                const int EXP_INV = combIt.includesElement(i) ? 1 : 0;
                bsl::vector<balm::Publisher *> sPublishers(Z);
                mX.findSpecificPublishers(&sPublishers, allCategories[i]);
                TestPublisher *sPub_p = (TestPublisher *)sPublishers.front();
                ASSERT(EXP_INV == sPub_p->invocations());
                sPub_p->reset();

            }
            gPub.reset();

        } while (combIt.next());
        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS/ACCESSORS (Bootstrap IV):
        //                            publish(balm::Category **, int, ...)
        //
        // Concerns:
        //   That the 'publish' method collects metric records from the
        //   callbacks and collectors belonging to the specified set of
        //   categories.
        //
        //   That the 'publish' method delivers a 'balm::MetricSample',
        //   containing an accurate time stamp, and the correct sub-set of
        //   collected records,  to the registered publishers for the
        //   specified 'categories'.
        //
        // Plan:
        //
        //   Create a set of unique category names, metric names to be
        //   collected by 'balm::Collector' objects, and an equal number of
        //   metric names to be collected by
        //   'balm::MetricsManager::RecordCollectionCallback' functors.
        //
        //   Add to the 'balm::MetricManager' object under test: a number
        //   ('NUM_PUBLISHER') of general publishers, a number
        //   ('NUM_PUBLISHER') of specific publishers for each category, and a
        //   'TestCallback' functor for each metric to be collected by
        //   callback.
        //
        //   Perform a test for each possible combination of categories.  On
        //   each iteration:
        //   1. Set all the 'balm::Collector' objects to a known state.
        //   2. Create a unique time interval and invoke 'publish'.
        //   3. For each combination of (category, metric name) to collected by
        //        callback: verify that, that the callback was invoked if
        //        and only if the category was published.
        //   4. For each category: lookup the "specific" publishers for that
        //        category; verify those publishers were invoked if and only
        //        if the category was published; if records were published,
        //        verify the elapsed time, time stamp, and records published
        //        were correct.
        //   5. For each "general" publisher; verify the publisher was invoked
        //        if any categories were published, and that the elapsed time,
        //        time stamp, and records published were correct.
        //
        // Testing:
        //   void publish(const balm::Category *[], int, const TimeInterval& );
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "TEST: void publish(const balm::Category **, int, ...)\n"
            << "=====================================================\n";

        const char *CATEGORIES[] = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);

        // One set of names for "collector" metrics and a distinct set of
        // names for "callback" metrics.
        const char *COL_METRICS[] = { "A", "B", "C", "MyMetric", "903metric" };
        const char *CB_METRICS[]  = { "D", "E", "jnkMetric", "my2", "other" };
        const int   NUM_METRICS = sizeof (COL_METRICS) / sizeof (*COL_METRICS);
        ASSERT(NUM_METRICS == sizeof(CB_METRICS) / sizeof(*CB_METRICS));
        const int NUM_PUBLISHERS = 3;

        typedef bsl::shared_ptr<TestCallback>       CbPtr;
        typedef bsl::map<Id, CbPtr>                 CallbackMap;

        Obj mX(Z); const Obj& MX = mX;
        Registry& registry     = mX.metricRegistry();
        Repository& repository = mX.collectorRepository();

        CallbackMap  callbacks(Z);
        bsl::vector<const Category *> allCategories(Z);

        // Populate the 'balm::MetricsManager' object under test with
        // callbacks, "general" publishers, and "specific" publishers.
        for (int j = 0; j < NUM_PUBLISHERS; ++j) {
            PubPtr pub_p(new (*Z) TestPublisher(Z), Z);
            mX.addGeneralPublisher(pub_p);
        }
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const Category *CAT = registry.getCategory(CATEGORIES[i]);
            allCategories.push_back(CAT);

            for (int j = 0; j < NUM_PUBLISHERS; ++j) {
                PubPtr pub_p(new (*Z) TestPublisher(Z), Z);
                mX.addSpecificPublisher(CAT, pub_p);
            }
            for (int j = 0; j < NUM_METRICS; ++j) {
                Id id = registry.getId(CATEGORIES[i],CB_METRICS[j]);
                CbPtr cb_p; cb_p.createInplace(Z, id, Z);
                mX.registerCollectionCallback(CAT, cb_p->function());
                callbacks[id] = cb_p;
            }
        }

        // Perform a test iteration for each *combination* of categories from
        // the set of test categories.  On each iteration invoke 'publish'
        // with the combination of categories and then verify the callbacks,
        // collectors, publishers were updated as expected.
        CombinationIterator<const Category *> combIt(allCategories, Z);
        do {
            // 1. Set all the 'balm::Collector' objects to a known state.
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    Collector *col =
                        repository.getDefaultCollector(CATEGORIES[i],
                                                       COL_METRICS[j]);
                    col->reset(); col->update(1);
                }
            }

            //   2. Create a unique time interval and invoke 'publish'.
            const bsl::vector<const Category *>& categories = combIt.current();
            bdlt::Datetime tmStamp = bdlt::CurrentTime::utc();
            if (categories.size() > 0) {
                mX.publish(categories.data(), categories.size());
            }

            //   3. For each combination of (category, metric name) to be
            //      collected by callback: verify that, that the callback was
            //      invoked if and only if the category was published.
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                for (int j = 0; j < NUM_METRICS; ++j) {
                    Id colId = registry.getId(CATEGORIES[i], COL_METRICS[j]);
                    Id cbId  = registry.getId(CATEGORIES[i], CB_METRICS[j]);
                    Collector *collector =
                                        repository.getDefaultCollector(colId);

                    // Verify the callback has been invoked (or not)
                    const int EXP_INV = combIt.includesElement(i) ? 1 : 0;
                    ASSERT(EXP_INV == callbacks[cbId]->invocations());
                    callbacks[cbId]->reset();

                    // Verify the collector has been reset (or not)
                    const int EXP_CNT = combIt.includesElement(i) ? 0 : 1;
                    balm::MetricRecord rec;
                    collector->load(&rec);
                    ASSERT(colId   == rec.metricId());
                    ASSERT(EXP_CNT == rec.count());
                }
            }

            //   4. For each category: lookup the "specific" publishers for
            //      that category; verify those publishers were invoked if and
            //      only if the category was published; if records were
            //      published, verify the elapsed time, time stamp, and
            //      records published were correct.
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                const Category *CAT = registry.getCategory(CATEGORIES[i]);
                bsl::vector<balm::Publisher *> specificPublishers(Z);
                ASSERT(NUM_PUBLISHERS ==
                       MX.findSpecificPublishers(&specificPublishers, CAT));
                for (int j = 0; j < specificPublishers.size(); ++j) {
                    TestPublisher *pub = (TestPublisher*)specificPublishers[j];

                    const int EXP_INV = combIt.includesElement(i) ? 1 : 0;
                    ASSERT(EXP_INV == pub->invocations());

                    // If the publisher was invoked verify the time stamp,
                    // elapsed time, and records published.
                    if (combIt.includesElement(i)) {
                        ASSERT(withinWindow(pub->lastTimeStamp(), tmStamp,10));
                        ASSERT(1 == pub->lastSample().numGroups());
                        ASSERT(2 * NUM_METRICS == pub->lastRecords().size());
                        for (int k = 0; k < NUM_METRICS; ++k) {
                            Id colId = registry.getId(CATEGORIES[i],
                                                      COL_METRICS[k]);
                            Id cbId = registry.getId(CATEGORIES[i],
                                                     CB_METRICS[k]);
                            ASSERT(pub->contains(colId));
                            ASSERT(pub->contains(cbId));

                        }
                    }
                    pub->reset();
                }
            }

            //   5. For each "general" publisher; verify the publisher was
            //      invoked if any categories were published, and that the
            //      elapsed time, time stamp, and records published were
            //      correct.
            bsl::vector<balm::Publisher *> generalPublishers(Z);
            ASSERT(NUM_PUBLISHERS ==
                   MX.findGeneralPublishers(&generalPublishers));
            for (int i = 0; i < NUM_PUBLISHERS; ++i) {
                TestPublisher *pub = (TestPublisher *)generalPublishers[i];
                if (categories.size() == 0) {
                    ASSERT(0 == pub->invocations());
                    continue;
                }
                ASSERT(1 == pub->invocations());
                ASSERT(withinWindow(pub->lastTimeStamp(), tmStamp, 10));
                ASSERT(pub->lastSample().numGroups() ==
                       combIt.current().size());
                ASSERT(2 * NUM_METRICS * categories.size() ==
                       pub->lastRecords().size());
                for (int j = 0; j < NUM_CATEGORIES; ++j) {
                    for (int k = 0; k < NUM_METRICS; ++k) {
                        Id colId = registry.getId(CATEGORIES[j],
                                                  COL_METRICS[k]);
                        Id cbId = registry.getId(CATEGORIES[j],
                                                 CB_METRICS[k]);
                        ASSERT(combIt.includesElement(j) ==
                               pub->contains(colId));
                        ASSERT(combIt.includesElement(j) ==
                               pub->contains(cbId));
                    }
                }
                pub->reset();
            }
        } while (combIt.next());
        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS/ACCESSORS (Bootstrap III):
        //                         register "general" and "specific" publishers
        //
        // Concerns:
        //   That the 'addGeneralPublisher' and 'addSpecificPublisher'
        //   properly register the publisher and that 'removePublishers'
        //   removes the publisher.
        //
        //   That the publications are passed to the appropriate publisher.
        //
        // Plan:
        //   Using a representative set of category names and 'TestPublisher'
        //   instances:
        //
        //   Test general publishers: Add each 'TestPublisher' as a general
        //   publisher.  Verify the publisher cannot be added again (as either
        //   a general or specific publisher) and that it can be found using
        //   'findGeneralPublishers'.  Test that 'publish' operations properly
        //   invoke the publishers 'publication' method.  Finally, for each
        //   registered 'TestPublisher', verify that 'removePublisher' removes
        //   the publisher for publications.
        //
        //   Test specific publishers: For each category,  add each
        //   'TestPublisher' as a specific publisher for that category.  Verify
        //   the publisher cannot be added again (as either a general or
        //   specific publisher for that category), that it can be found
        //   using  'findGeneralPublishers', and that publications for that
        //   category are properly reported to the 'TestPublisher'.  Finally,
        //   for each registered publisher, verify that 'removePublisher'
        //   removes the publisher for publications.
        //
        // Testing:
        //   int addGeneralPublisher(bsl::shared_ptr<balm::Publisher>& );
        //   int addSpecificPublisher(const balm::Category *category,
        //                            bsl::shared_ptr<balm::Publisher>&  );
        //   int removePublisher(const bsl::shared_ptr<balm::Publisher>& )
        //   void publish(const balm::Category *, const bsls::TimeInterval&  );
        //   void publishAll(const bsls::TimeInterval&  );
        //   int findGeneralPublishers(bsl::vector<balm::Publisher *> *) const;
        //   int findSpecificPublishers(bsl::vector<Publisher *> *,
        //                              const Category           *) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST: add and remove publishers" << endl
                          << "===============================" << endl;

        const char *CATEGORIES[] = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);

        TestPublisher tp1(Z), tp2(Z), tp3(Z), tp4(Z);
        TestPublisher *TEST_PUBS[] = { &tp1, &tp2, &tp3, &tp4 };
        const int NUM_PUBS = sizeof(TEST_PUBS)/sizeof(*TEST_PUBS);

        // ------------------- Verify 'general' publishers --------------------
        {
            Obj mX(Z); const Obj& MX = mX;
            Registry& registry = mX.metricRegistry();

            // Add general publishers
            if (veryVerbose) cout << "Test general publishers" << bsl::endl;

            for (int i = 0; i < NUM_PUBS; ++i) {
                PubPtr pub_p(TEST_PUBS[i], bslstl::SharedPtrNilDeleter(), Z);
                ASSERT(0 == mX.addGeneralPublisher(pub_p));

                // Verify that the publisher can't be added again, as either a
                // general publisher or a specific publisher.
                ASSERT(0 != mX.addGeneralPublisher(pub_p));
                for (int j = 0; j < NUM_CATEGORIES; ++j) {
                    const Category *CAT = registry.getCategory(CATEGORIES[j]);
                    bsl::vector<balm::Publisher *> pubsFound(Z);
                    ASSERT(0 != mX.addSpecificPublisher(CAT, pub_p));
                    ASSERT(0 == mX.findSpecificPublishers(&pubsFound, CAT));
                    ASSERT(0 == pubsFound.size());
                }

                // Verify 'findGeneralPublishers'
                bsl::vector<balm::Publisher *> pubsFound(Z);
                ASSERT(i + 1 == MX.findGeneralPublishers(&pubsFound));

                bsl::set<balm::Publisher *> pubSet(Z);
                pubSet.insert(pubsFound.begin(), pubsFound.end());
                ASSERT(i + 1 == pubSet.size());
                for (int j = 0; j < NUM_PUBS; ++j) {
                    const int EXP = i < j ? 0 : 1;
                    ASSERT(EXP == pubSet.count(TEST_PUBS[j]));
                }
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify publications go to the general publisher
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                const Category *CAT = registry.getCategory(CATEGORIES[i]);
                mX.collectorRepository().getDefaultCollector(CATEGORIES[i],
                                                             "A");
                mX.publish(CAT);

                for (int j = 0; j < NUM_PUBS; ++j) {
                    ASSERT(1 == TEST_PUBS[j]->invocations());
                    TEST_PUBS[j]->reset();
                }
            }
            mX.publishAll();
            for (int i = 0; i < NUM_PUBS; ++i) {
                ASSERT(1 == TEST_PUBS[i]->invocations());
                TEST_PUBS[i]->reset();
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            // Verify removal
            ASSERT(0 != mX.removePublisher(0));
            for (int i = 0; i < NUM_PUBS; ++i) {
                PubPtr pub_p(TEST_PUBS[i], bslstl::SharedPtrNilDeleter(), Z);
                ASSERT(0 == mX.removePublisher(pub_p.get()));
                ASSERT(0 != mX.removePublisher(pub_p.get()));
                mX.publishAll();
                for (int j = 0; j < NUM_PUBS; ++j) {
                    const int EXP = (j <= i) ? 0 : 1;
                    ASSERT(EXP == TEST_PUBS[j]->invocations());
                    TEST_PUBS[j]->reset();
                }
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        // ------------------- Verify 'specific' publishers -------------------
        {
            Obj mX(Z); const Obj& MX = mX;
            Registry& registry = mX.metricRegistry();

            // Add general publishers
            if (veryVerbose) cout << "Test specific publishers" << bsl::endl;

            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                const Category *CAT = registry.getCategory(CATEGORIES[i]);
                mX.collectorRepository().getDefaultCollector(CATEGORIES[i],
                                                             "A");
                for (int j = 0; j < NUM_PUBS; ++j) {
                    PubPtr pub_p(TEST_PUBS[j],
                                 bslstl::SharedPtrNilDeleter(),
                                 Z);
                    ASSERT(0 == mX.addSpecificPublisher(CAT, pub_p));

                    // Verify the publisher can't be added again.
                    ASSERT(0 != mX.addSpecificPublisher(CAT, pub_p));
                    ASSERT(0 != mX.addGeneralPublisher(pub_p));

                    // Verify 'find' operations.
                    bsl::vector<balm::Publisher *> pubsFound(Z);
                    ASSERT(0 == MX.findGeneralPublishers(&pubsFound));
                    ASSERT(0 == pubsFound.size());

                    ASSERT(j+1 == MX.findSpecificPublishers(&pubsFound, CAT));
                    bsl::set<balm::Publisher *> pubSet(Z);
                    pubSet.insert(pubsFound.begin(), pubsFound.end());
                    ASSERT(j+1 == pubsFound.size());
                    for (int k = 0; k < NUM_PUBS; ++k) {
                        const int EXP = j < k ? 0 : 1;
                        ASSERT(EXP == pubSet.count(TEST_PUBS[k]));
                    }

                    // Verify publish.
                    mX.publish(CAT);
                    for (int k = 0; k < NUM_PUBS; ++k) {
                        const int EXP = j < k ? 0 : 1;
                        ASSERT(EXP == TEST_PUBS[k]->invocations());
                        if (EXP > 0) {
                            const balm::MetricRecord& first =
                                      *TEST_PUBS[k]->lastRecords().begin();
                            ASSERT(CAT == first.metricId().category());
                        }
                        TEST_PUBS[k]->reset();
                    }
                }
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            mX.publishAll();
            for (int i = 0; i < NUM_PUBS; ++i) {
                ASSERT(1 == TEST_PUBS[i]->invocations());
                TEST_PUBS[i]->reset();
            }

            // Verify removal
            for (int i = 0; i < NUM_PUBS; ++i) {
                PubPtr pub_p(TEST_PUBS[i], bslstl::SharedPtrNilDeleter(), Z);
                ASSERT(0 == mX.removePublisher(pub_p.get()));
                ASSERT(0 != mX.removePublisher(pub_p.get()));

                mX.publishAll();
                for (int j = 0; j < NUM_PUBS; ++j) {
                    const int EXP = (j <= i) ? 0 : 1;
                    ASSERT(EXP == TEST_PUBS[j]->invocations());
                    TEST_PUBS[j]->reset();
                }
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS/ACCESSORS (Bootstrap II):
        //                                                  register callbacks
        //
        // Concerns:
        //   That 'registerCollectionCallback' registers the callback function
        //   for a category such that it is invoked when the category is
        //   published.
        //
        // Plan:
        //   Create a map from 'balm::MetricId' to a
        //   '(CallbackHandle, TestCallback)' pair.  Using a representative set
        //   of category and metric identifiers:
        //
        //   For each identified metric, create a 'TestCallback' and register
        //   it for associated category.  Verify the returned 'CallbackHandle'
        //   is valid and add the 'CallbackHandle' and 'TestCallback' to the
        //   map of callbacks.
        //
        //   Next, for each category, invoke 'publish' and verify that the
        //   callbacks (in the map of callbacks) belonging to that category are
        //   invoked.
        //
        //   Finally, for each 'TestCallback' in the map of callbacks, invoke
        //   'removeCollectionCallback'.  Verify the callbackHandle cannot be
        //   removed twice.  Then invoke 'publishAll' and verify that the
        //   removed callbacks are not invoked.
        //
        // Testing:
        //   CallbackHandle registerCollectionCallback(
        //                                          const balm::Category     *,
        //                                          RecordsCollectionCallback);
        //   void publish(const balm::Category *, const bsls::TimeInterval&  );
        //   void publishAll(const bsls::TimeInterval&  );
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "TEST: registerCollectionCallback, publish" << endl
            << "======================================" << endl;

        const char *CATEGORIES[] = {"A", "B", "C", "Test", "12312category"};
        const int   NUM_CATEGORIES = sizeof (CATEGORIES)/sizeof (*CATEGORIES);
        const char *METRICS[] = { "A", "B", "C", "MyMetric" "90123metric" };
        const int NUM_METRICS = sizeof (METRICS) / sizeof (*METRICS);

        // Create a map, from metric id to 'TestCallback' functor.
        typedef bsl::shared_ptr<TestCallback> CbPtr;
        typedef bsl::pair<CbHandle, CbPtr>    CallbackInfo;
        typedef bsl::map<Id, CallbackInfo>    CallbackMap;
        CallbackMap                           callbacks(Z);

        // Add 'TestCallback' functors to the metrics manager and store the
        // returned 'CallbackHandle'.
        Obj mX(Z); const Obj& MX = mX;
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const char *CATEGORY = CATEGORIES[i];
            for (int j = 0; j < NUM_METRICS; ++j) {
                const char *METRIC = METRICS[j];

                Id id = mX.metricRegistry().getId(CATEGORY, METRIC);

                CbPtr cb; cb.createInplace(Z, id, Z);
                CbHandle handle =
                  mX.registerCollectionCallback(id.category(), cb->function());

                callbacks[id] = bsl::make_pair(handle, cb);
                ASSERT(handle != Obj::e_INVALID_HANDLE);
            }
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());

        // Invoke 'publish' and verify the 'TestCallbacks' in the published
        // category (and only those callbacks) are invoked once.
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const char *CATEGORY = CATEGORIES[i];
            const Category *CAT = mX.metricRegistry().getCategory(CATEGORY);

            mX.publish(CAT);

            CallbackMap::iterator it = callbacks.begin();
            for (; it != callbacks.end(); ++it) {
                TestCallback& cb = *it->second.second;
                const int EXP = (CAT == cb.metricId().category()) ? 1 : 0;
                ASSERT(EXP == cb.invocations());
                cb.reset();
            }
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());

        // Remove each 'TestCallback' functor and invoke publish to verify it
        // is no longer invoked when the category is published'.
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            const char *CATEGORY = CATEGORIES[i];
            for (int j = 0; j < NUM_METRICS; ++j) {
                const char *METRIC = METRICS[j];

                Id id = mX.metricRegistry().getId(CATEGORY, METRIC);
                CallbackInfo& cbInfo = callbacks[id];
                ASSERT(id == cbInfo.second->metricId());

                ASSERT(0 == mX.removeCollectionCallback(cbInfo.first));
                ASSERT(0 != mX.removeCollectionCallback(cbInfo.first));

                cbInfo.first = Obj::e_INVALID_HANDLE;

                mX.publishAll();

                CallbackMap::iterator it = callbacks.begin();
                for (; it != callbacks.end(); ++it) {
                    CallbackInfo& info = it->second;
                    const int EXP = (info.first == Obj::e_INVALID_HANDLE)
                                    ? 0 : 1;
                    ASSERT(EXP == info.second->invocations());
                    info.second->reset();
                }
            }
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS/ACCESSORS (BOOTSTRAP I):
        //                              metricRegistry(), collectorRepository()
        //
        // Concerns:
        //   'metricRegistry()' and 'collectorRepository()' correctly return
        //   valid (non-modifiable) references.
        //
        // Plan:
        //
        // Testing:
        //   balm::MetricsManager(bslma::Allocator *);
        //   ~balm::MetricsManager();
        //   balm::CollectorRepository& collectorRepository();
        //   balm::MetricRegistry& metricRegistry();
        //   const balm::CollectorRepository& collectorRepository() const;
        //   const balm::MetricRegistry& metricRegistry() const;
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "TEST: metricRegistry, collectorRepository" << endl
            << "=========================================" << endl;

        {
            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 == testAlloc.numBytesInUse());
            Obj mX(Z); const Obj& MX = mX;

            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 <  testAlloc.numBytesInUse());

            Registry& registry = mX.metricRegistry();
            const Registry& REGISTRY = MX.metricRegistry();
            ASSERT(&registry == &REGISTRY);

            Repository& repository = mX.collectorRepository();
            const Repository& REPOSITORY = MX.collectorRepository();
            ASSERT(&repository == &REPOSITORY);

            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 <  testAlloc.numBytesInUse());
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 == testAlloc.numBytesInUse());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING HELPERS: Combinations, TestPublisher, TestCallback,
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "TEST HELPERS: Combinations, TestPublisher, TestCallback\n"
            << "=======================================================\n";

        {
            if (veryVerbose)
                cout << "\tTesting TestCallback\n";

            balm::MetricRegistry registry(Z);
            const char *IDS [] = {"A", "B", "AB", "ABC", "DEF"};
            const int NUM_IDS = sizeof IDS / sizeof *IDS;

            for (int i = 0; i < NUM_IDS; ++i) {
                Id id = registry.getId(IDS[i], IDS[i]);
                const balm::MetricRecord VAL(id, 2, 2, 2, 2);
                TestCallback tcb1(id, Z), tcb2(id, Z), tcb3(VAL, Z);
                const TestCallback& TCB1 = tcb1;
                const TestCallback& TCB2 = tcb2;
                const TestCallback& TCB3 = tcb3;

                ASSERT(id == TCB1.metricId());
                ASSERT(id == TCB2.metricId());
                ASSERT(id == TCB3.metricId());
                ASSERT(balm::MetricRecord(id) == TCB1.record());
                ASSERT(balm::MetricRecord(id) == TCB2.record());
                ASSERT(VAL                   == TCB3.record());
                ASSERT(balm::MetricRecord(id) == tcb1.record());
                ASSERT(balm::MetricRecord(id) == tcb2.record());
                ASSERT(VAL                   == tcb3.record());

                tcb2.record() = VAL;
                ASSERT(balm::MetricRecord(id) == TCB1.record());
                ASSERT(VAL                   == TCB2.record());
                ASSERT(VAL                   == TCB3.record());
                ASSERT(balm::MetricRecord(id) == tcb1.record());
                ASSERT(VAL                   == tcb2.record());
                ASSERT(VAL                   == tcb3.record());

                Obj::RecordsCollectionCallback cb1 = tcb1.function();
                Obj::RecordsCollectionCallback cb2 = tcb2.function();
                Obj::RecordsCollectionCallback cb3 = tcb3.function();

                ASSERT(0 == TCB1.invocations());
                ASSERT(0 == TCB2.invocations());
                ASSERT(0 == TCB3.invocations());

                for (int j = 0; j < 10; ++j) {
                    bsl::vector<balm::MetricRecord> rec1(Z), rec2(Z), rec3(Z);
                    cb1(&rec1, true);
                    cb2(&rec2, true);
                    cb3(&rec3, true);

                    ASSERT(j + 1 == TCB1.invocations());
                    ASSERT(1     == TCB2.invocations());
                    ASSERT(1     == TCB3.invocations());

                    ASSERT(1 == rec1.size());
                    ASSERT(1 == rec2.size());
                    ASSERT(1 == rec3.size());

                    ASSERT(balm::MetricRecord(id) == rec1.front());
                    ASSERT(VAL                   == rec2.front());
                    ASSERT(VAL                   == rec3.front());

                    tcb2.reset();
                    tcb3.reset();

                    ASSERT(0     == TCB2.invocations());
                    ASSERT(0     == TCB3.invocations());

                }
            }
        }
        {
            if (veryVerbose)
                cout << "\tTesting TestPublisher\n";

            balm::MetricRegistry registry(Z);
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

            bsl::vector<balm::MetricRecord>               allRecords(Z);
            bsl::vector<bsl::vector<balm::MetricRecord> > recordBuffer(Z);
            recordBuffer.resize(NUM_VALUES);
            balm::MetricSample sample(Z);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const char *RECORD_SPEC = VALUES[i].d_records;
                bsl::vector<balm::MetricRecord>& records = recordBuffer[i];

                for (int j = 0; j < bsl::strlen(RECORD_SPEC); ++j) {
                    bsl::string value(1, RECORD_SPEC[j], Z);
                    Id id = registry.getId(value.c_str(), value.c_str());
                    records.push_back(balm::MetricRecord(id));
                    allRecords.push_back(balm::MetricRecord(id));
                }

                bsls::TimeInterval ELAPSED_TIME(VALUES[i].d_elapsedTime, 0);
                bdlt::Date dt = bdlt::DateUtil::convertFromYYYYMMDDRaw(
                                                        VALUES[i].d_timeStamp);
                bdlt::DatetimeTz TIME_STAMP(bdlt::Datetime(dt), 0);

                sample.setTimeStamp(TIME_STAMP);
                sample.appendGroup(records.data(),
                                   records.size(),
                                   ELAPSED_TIME);

                tp2.reset();
                ASSERT(0 == tp2.invocations());
                ASSERT(0 == tp2.lastRecords().size());

                balm::Publisher *p1_p = &tp1;
                balm::Publisher *p2_p = &tp2;

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

                bsl::vector<balm::MetricRecord>::const_iterator it =
                                                           allRecords.begin();
                for (; it != allRecords.end(); ++it) {
                    ASSERT(tp1.contains(it->metricId()));
                    ASSERT(tp2.contains(it->metricId()));
                }
                ASSERT(!tp1.contains(registry.getId("Bad", "Bad")));
                ASSERT(!tp2.contains(registry.getId("Bad", "Bad")));
            }
        }
        {
            if (veryVerbose)
                cout << "\tTesting CombinationIterator\n";

            const char *VALUES ="1234";
            const char *COM_0  = "";
            const char *COM_1  = "1";
            const char *COM_2  = "2";
            const char *COM_3  = "12";
            const char *COM_4  = "3";
            const char *COM_5  = "13";
            const char *COM_6  = "23";
            const char *COM_7  = "123";
            const char *COM_8  = "4";
            const char *COM_9  = "14";
            const char *COM_10 = "24";
            const char *COM_11 = "124";
            const char *COM_12 = "34";
            const char *COM_13 = "134";
            const char *COM_14 = "234";
            const char *COM_15 = "1234";
            const char *COMBINATIONS[] = {
                COM_0, COM_1, COM_2, COM_3, COM_4, COM_5, COM_6, COM_7, COM_8,
                COM_9, COM_10, COM_11, COM_12, COM_13, COM_14, COM_15 };

            bsl::vector<char> values(VALUES, VALUES + bsl::strlen(VALUES), Z);
            CombinationIterator<char> iter(values, Z);
            ASSERT(0 == defaultAllocator.numBytesInUse());
            int i = 0;
            do {
                bsl::string result1(Z), result2(Z);

                const bsl::vector<char>& combination = iter.current();
                for (int j = 0; j < combination.size();++j) {
                    result1 += combination[j];
                }

                for (int j = 0; j < bsl::strlen(VALUES); ++j) {
                    if (iter.includesElement(j)) {
                        result2 += VALUES[j];
                    }
                }
                if (veryVeryVerbose) {
                    P_(COMBINATIONS[i]); P_(result1); P(result2);
                }
                ASSERT(result1 == COMBINATIONS[i]);
                ASSERT(result2 == COMBINATIONS[i]);
                ++i;
            } while (iter.next());
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BREATHING TEST: enableCategory
        //   Developers' Sandbox.
        //
        // Plan:
        //   Perform and ad-hoc test of the primary modifiers and accessors.
        //
        //   Test MetricsManager
        //      Create a metrics manager register data
        //      Verify Add/find GeneralPublishers
        //      Verify add/find specificPublishers
        //      Verify remove publishers
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST: enable category" << endl
                          << "===============================" << endl;

        Obj mX(Z); const Obj& MX = mX;

        mX.collectorRepository().getDefaultCollector("A", "A");
        mX.collectorRepository().getDefaultCollector("B", "B");
        mX.collectorRepository().getDefaultCollector("C", "C");

        balm::MetricId METRIC_A = mX.metricRegistry().getId("A", "A");
        balm::MetricId METRIC_B = mX.metricRegistry().getId("B", "B");
        balm::MetricId METRIC_C = mX.metricRegistry().getId("C", "C");

        const balm::Category *CAT_A = mX.metricRegistry().getCategory("A");
        const balm::Category *CAT_B = mX.metricRegistry().getCategory("B");
        const balm::Category *CAT_C = mX.metricRegistry().getCategory("C");

        TestCallback CB_A(METRIC_A, Z), CB_B(METRIC_B, Z), CB_C(METRIC_C, Z);
        TestPublisher PUB_A(Z), PUB_B(Z), PUB_ALL(Z);
        PubPtr PUBPTR_A(&PUB_A, bslstl::SharedPtrNilDeleter(), Z);
        PubPtr PUBPTR_B(&PUB_B, bslstl::SharedPtrNilDeleter(), Z);
        PubPtr PUBPTR_ALL(&PUB_ALL, bslstl::SharedPtrNilDeleter(), Z);

        ASSERT(0 == mX.addSpecificPublisher(CAT_A, PUBPTR_A));
        ASSERT(0 == mX.addSpecificPublisher(CAT_B, PUBPTR_B));
        ASSERT(0 == mX.addGeneralPublisher(PUBPTR_ALL));

        int handle0 = mX.registerCollectionCallback(CAT_A, CB_A.function());
        int handle1 = mX.registerCollectionCallback(CAT_B, CB_B.function());
        int handle2 = mX.registerCollectionCallback(CAT_C, CB_C.function());

        ASSERT(CAT_A->enabled());
        ASSERT(CAT_B->enabled());
        ASSERT(CAT_C->enabled());

        mX.publish(CAT_A);

        ASSERT(1 == CB_A.invocations());
        ASSERT(0 == CB_B.invocations());
        ASSERT(0 == CB_C.invocations());
        ASSERT(1 == PUB_A.invocations());
        ASSERT(0 == PUB_B.invocations());
        ASSERT(1 == PUB_ALL.invocations());

        CB_A.reset(); CB_B.reset(); CB_C.reset();
        PUB_A.reset(); PUB_B.reset(); PUB_ALL.reset();

        mX.setCategoryEnabled(CAT_A, false);

        ASSERT(!CAT_A->enabled());
        ASSERT( CAT_B->enabled());
        ASSERT( CAT_C->enabled());

        mX.publish(CAT_A);

        ASSERT(0 == CB_A.invocations());
        ASSERT(0 == CB_B.invocations());
        ASSERT(0 == CB_C.invocations());
        ASSERT(0 == PUB_A.invocations());
        ASSERT(0 == PUB_B.invocations());
        ASSERT(0 == PUB_ALL.invocations());

        CB_A.reset(); CB_B.reset(); CB_C.reset();
        PUB_A.reset(); PUB_B.reset(); PUB_ALL.reset();

        mX.publishAll();

        ASSERT(0 == CB_A.invocations());
        ASSERT(1 == CB_B.invocations());
        ASSERT(1 == CB_C.invocations());
        ASSERT(0 == PUB_A.invocations());
        ASSERT(1 == PUB_B.invocations());
        ASSERT(1 == PUB_ALL.invocations());

        CB_A.reset(); CB_B.reset(); CB_C.reset();
        PUB_A.reset(); PUB_B.reset(); PUB_ALL.reset();

        mX.setCategoryEnabled(CAT_A, true);

        ASSERT(CAT_A->enabled());
        ASSERT(CAT_B->enabled());
        ASSERT(CAT_C->enabled());

        bsl::set<const balm::Category *> exclude(Z);
        exclude.insert(CAT_B);
        mX.publishAll(exclude);

        ASSERT(1 == CB_A.invocations());
        ASSERT(0 == CB_B.invocations());
        ASSERT(1 == CB_C.invocations());
        ASSERT(1 == PUB_A.invocations());
        ASSERT(0 == PUB_B.invocations());
        ASSERT(1 == PUB_ALL.invocations());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BREATHING TEST: publisher registry
        //   Developers' Sandbox.
        //
        // Plan:
        //   Perform and ad-hoc test of the primary modifiers and accessors.
        //
        //   Test MetricsManager
        //      Create a metrics manager register data
        //      Verify Add/find GeneralPublishers
        //      Verify add/find specificPublishers
        //      Verify remove publishers
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST: publisher registry" << endl
                          << "==================================" << endl;

        Obj mX(Z); const Obj& MX = mX;
        mX.collectorRepository().getDefaultCollector("A", "A");
        mX.collectorRepository().getDefaultCollector("B", "B");
        mX.collectorRepository().getDefaultCollector("C", "C");
        TestPublisher tp1(Z), tp2(Z), tp3(Z), tp4(Z);
        PubPtr p1(&tp1, bslstl::SharedPtrNilDeleter(), Z);
        PubPtr p2(&tp2, bslstl::SharedPtrNilDeleter(), Z);
        PubPtr p3(&tp3, bslstl::SharedPtrNilDeleter(), Z);
        PubPtr p4(&tp4, bslstl::SharedPtrNilDeleter(), Z);

        // Add general publishers.
        bsl::vector<balm::Publisher *> pubV(Z);
        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 != mX.removePublisher(p1));
        ASSERT(0 == mX.addGeneralPublisher(p1));
        ASSERT(0 == mX.addGeneralPublisher(p2));
        ASSERT(0 != mX.addGeneralPublisher(p1));
        ASSERT(0 != mX.addGeneralPublisher(p2));
        ASSERT(0 != mX.addSpecificPublisher("A", p1));
        ASSERT(0 != mX.addSpecificPublisher("A", p2));
        ASSERT(0 == mX.addSpecificPublisher("A", p3));
        ASSERT(0 != mX.addSpecificPublisher("A", p3));

        ASSERT(1 == MX.findSpecificPublishers(&pubV, "A"));
        ASSERT(2 == MX.findGeneralPublishers(&pubV));

        mX.publish(mX.metricRegistry().getCategory("A"));
        ASSERT(1 == tp1.lastRecords().size());
        ASSERT(1 == tp2.lastRecords().size());
        ASSERT(1 == tp3.lastRecords().size());

        // Remove publishers
        ASSERT(0 == mX.removePublisher(p3));
        ASSERT(0 == mX.removePublisher(p1));
        ASSERT(0 == MX.findSpecificPublishers(&pubV, "A"));
        ASSERT(1 == MX.findGeneralPublishers(&pubV));

        ASSERT(0 == mX.removePublisher(p2));
        ASSERT(0 == MX.findSpecificPublishers(&pubV, "A"));
        ASSERT(0 == MX.findGeneralPublishers(&pubV));

        ASSERT(0 == mX.addSpecificPublisher("A", p1));
        ASSERT(0 != mX.addSpecificPublisher("A", p1));
        ASSERT(0 != mX.addGeneralPublisher(p1));
        ASSERT(0 == mX.addSpecificPublisher("B", p1));
        ASSERT(0 != mX.addSpecificPublisher("B", p1));
        ASSERT(0 != mX.addGeneralPublisher(p1));

        ASSERT(0 == mX.addSpecificPublisher("A", p2));
        ASSERT(0 != mX.addSpecificPublisher("A", p2));
        ASSERT(0 != mX.addGeneralPublisher(p2));
        ASSERT(0 == mX.addSpecificPublisher("B", p2));
        ASSERT(0 != mX.addSpecificPublisher("B", p2));
        ASSERT(0 != mX.addGeneralPublisher(p2));

        ASSERT(2 == MX.findSpecificPublishers(&pubV, "A"));
        ASSERT(2 == MX.findSpecificPublishers(&pubV, "B"));
        ASSERT(0 == MX.findGeneralPublishers(&pubV));
        ASSERT(0 == defaultAllocator.numBytesInUse());

        ASSERT(0 == mX.removePublisher(p1));
        ASSERT(1 == MX.findSpecificPublishers(&pubV, "A"));
        ASSERT(1 == MX.findSpecificPublishers(&pubV, "B"));
        ASSERT(0 == MX.findGeneralPublishers(&pubV));

        ASSERT(0 == mX.removePublisher(p2));
        ASSERT(0 == MX.findSpecificPublishers(&pubV, "A"));
        ASSERT(0 == MX.findSpecificPublishers(&pubV, "B"));
        ASSERT(0 == MX.findGeneralPublishers(&pubV));

        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Plan:
        //   Perform and ad-hoc test of the primary modifiers and accessors.
        //
        //   Test MetricsManager
        //      Create a metrics manager register data
        //      Verify publishers can be found
        //      Verify publish
        //      Verify publish(const char *categories[],...)
        //      Verify publish(bsl::set<bsl::string>&,...)
        //      Verify remove callbacks
        //      Verify remove publishers
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        TestPublisher tpa_1(Z), tpa_2(Z),
                      tpb_1(Z), tpb_2(Z),
                      tpc_1(Z), tpc_2(Z);

        PubPtr pA_1(&tpa_1, bslstl::SharedPtrNilDeleter(), Z);
        PubPtr pA_2(&tpa_2, bslstl::SharedPtrNilDeleter(), Z);
        PubPtr pB_1(&tpb_1, bslstl::SharedPtrNilDeleter(), Z);
        PubPtr pB_2(&tpb_2, bslstl::SharedPtrNilDeleter(), Z);
        PubPtr pC_1(&tpc_1, bslstl::SharedPtrNilDeleter(), Z);
        PubPtr pC_2(&tpc_2, bslstl::SharedPtrNilDeleter(), Z);

        if (verbose) {
            cout << "Test MetricsManager\n";
            cout << "\tCreate a metrics manager register data"
                 << endl;
        }

        Obj mX(Z); const Obj& MX = mX;
        const balm::Category *CAT_A = mX.metricRegistry().getCategory("A");
        const balm::Category *CAT_B = mX.metricRegistry().getCategory("B");
        const balm::Category *CAT_C = mX.metricRegistry().getCategory("C");

        balm::MetricId METRIC_A = mX.metricRegistry().getId("A", "A");
        balm::MetricId METRIC_B = mX.metricRegistry().getId("B", "B");
        balm::MetricId METRIC_C = mX.metricRegistry().getId("C", "C");

        TestCallback tcba_1(METRIC_A, Z), tcba_2(METRIC_A, Z),
                     tcbb_1(METRIC_B, Z), tcbb_2(METRIC_B, Z),
                     tcbc_1(METRIC_C, Z), tcbc_2(METRIC_C, Z);

        ASSERT(0 == tcba_1.invocations());
        ASSERT(0 == tcbb_1.invocations());
        ASSERT(0 == tcbc_1.invocations());
        ASSERT(0 == tcba_2.invocations());
        ASSERT(0 == tcbb_2.invocations());
        ASSERT(0 == tcbc_2.invocations());

        // Use a 20 minute window to verify timestamps.  If the test
        // takes longer than 10 minutes to run, there are other issues.
        bdlt::Datetime now    = bdlt::CurrentTime::utc();
        const int     WINDOW = 10 * 60 * MILLISECS_PER_SEC;

        // Reset dummy callbacks and dummy publishers
        mX.publishAll();  // Reset the prev publication time of all categories
        tcba_1.reset(); tcba_2.reset();
        tcbb_1.reset(); tcbb_2.reset();
        tcbc_1.reset(); tcbc_2.reset();
        tpa_1.reset(); tpa_2.reset();
        tpb_1.reset(); tpb_2.reset();
        tpc_1.reset(); tpc_2.reset();

        ASSERT(0 == tcba_1.invocations());
        ASSERT(0 == tcbb_1.invocations());
        ASSERT(0 == tcbc_1.invocations());
        ASSERT(0 == tcba_2.invocations());
        ASSERT(0 == tcbb_2.invocations());
        ASSERT(0 == tcbc_2.invocations());

        ASSERT(0 == mX.addSpecificPublisher(CAT_A, pA_1));
        ASSERT(0 != mX.addSpecificPublisher(CAT_A, pA_1));
        ASSERT(0 == mX.addSpecificPublisher(CAT_A, pA_2));
        ASSERT(0 != mX.addSpecificPublisher(CAT_A, pA_2));
        ASSERT(0 == mX.addSpecificPublisher(CAT_B, pB_1));
        ASSERT(0 != mX.addSpecificPublisher(CAT_B, pB_1));
        ASSERT(0 == mX.addSpecificPublisher(CAT_B, pB_2));
        ASSERT(0 != mX.addSpecificPublisher(CAT_B, pB_2));
        ASSERT(0 == mX.addSpecificPublisher(CAT_C, pC_1));
        ASSERT(0 != mX.addSpecificPublisher(CAT_C, pC_1));
        ASSERT(0 == mX.addSpecificPublisher(CAT_C, pC_2));
        ASSERT(0 != mX.addSpecificPublisher(CAT_C, pC_2));

        int handle0 = mX.registerCollectionCallback(CAT_A, tcba_1.function());
        int handle1 = mX.registerCollectionCallback(CAT_A, tcba_2.function());
        int handle2 = mX.registerCollectionCallback(CAT_B, tcbb_1.function());
        int handle3 = mX.registerCollectionCallback(CAT_B, tcbb_2.function());
        int handle4 = mX.registerCollectionCallback(CAT_C, tcbc_1.function());
        int handle5 = mX.registerCollectionCallback(CAT_C, tcbc_2.function());

        if (verbose) {
            cout << "\tVerify publishers can be found\n";
        }

        bsl::vector<balm::Publisher *> pVec(Z);
        ASSERT(2 == MX.findSpecificPublishers(&pVec, CAT_A));
        ASSERT(2 == pVec.size());
        ASSERT(pA_1.get() == pVec[0] || pA_2.get() == pVec[0]);
        ASSERT(pA_1.get() == pVec[1] || pA_2.get() == pVec[1]);

        pVec.clear();
        ASSERT(2 == MX.findSpecificPublishers(&pVec, CAT_B));
        ASSERT(2 == pVec.size());
        ASSERT(pB_1.get() == pVec[0] || pB_2.get() == pVec[0]);
        ASSERT(pB_1.get() == pVec[1] || pB_2.get() == pVec[1]);

        pVec.clear();
        ASSERT(2 == MX.findSpecificPublishers(&pVec, CAT_C));
        ASSERT(2 == pVec.size());
        ASSERT(pC_1.get() == pVec[0] || pC_2.get() == pVec[0]);
        ASSERT(pC_1.get() == pVec[1] || pC_2.get() == pVec[1]);

        if (verbose) {
            cout << "\tVerify publish\n";
        }

        bslmt::ThreadUtil::microSleep(100000, 0);
        mX.publish(CAT_A);
        ASSERT(1 == tcba_1.invocations());
        ASSERT(1 == tcba_2.invocations());
        ASSERT(0 == tcbb_1.invocations());
        ASSERT(0 == tcbb_2.invocations());
        ASSERT(0 == tcbc_1.invocations());
        ASSERT(0 == tcbc_2.invocations());

        ASSERT(1 == tpa_1.invocations());
        ASSERT(1 == tpa_2.invocations());
        ASSERT(0 == tpb_1.invocations());
        ASSERT(0 == tpb_2.invocations());
        ASSERT(0 == tpc_1.invocations());
        ASSERT(0 == tpc_2.invocations());

        ASSERT(1 == tpa_1.lastElapsedTimes().size());
        LOOP_ASSERT(*tpa_1.lastElapsedTimes().begin(),
            bsls::TimeInterval(0, 0) < *tpa_1.lastElapsedTimes().begin());
        withinWindow(tpa_1.lastTimeStamp(), now, WINDOW);
        ASSERT(2 == tpa_1.lastRecords().size());
        ASSERT(tpa_1.contains(tcba_1.metricId()));

        ASSERT(1 == tpa_2.lastElapsedTimes().size());
        LOOP_ASSERT(*tpa_2.lastElapsedTimes().begin(),
            bsls::TimeInterval(0, 0) < *tpa_2.lastElapsedTimes().begin());
        withinWindow(tpa_2.lastTimeStamp(), now, WINDOW);
        ASSERT(2 == tpa_2.lastRecords().size());
        ASSERT(tpa_2.contains(tcba_2.metricId()));

        // Reset dummy callbacks and dummy publishers
        mX.publishAll();  // Reset the prev publication time of all categories
        tcba_1.reset(); tcba_2.reset();
        tcbb_1.reset(); tcbb_2.reset();
        tcbc_1.reset(); tcbc_2.reset();
        tpa_1.reset(); tpa_2.reset();
        tpb_1.reset(); tpb_2.reset();
        tpc_1.reset(); tpc_2.reset();

        bslmt::ThreadUtil::microSleep(100000, 0);
        mX.publish(CAT_B);
        ASSERT(0 == tcba_1.invocations());
        ASSERT(0 == tcba_2.invocations());
        ASSERT(1 == tcbb_1.invocations());
        ASSERT(1 == tcbb_2.invocations());
        ASSERT(0 == tcbc_1.invocations());
        ASSERT(0 == tcbc_2.invocations());

        ASSERT(0 == tpa_1.invocations());
        ASSERT(0 == tpa_2.invocations());
        ASSERT(1 == tpb_1.invocations());
        ASSERT(1 == tpb_2.invocations());
        ASSERT(0 == tpc_1.invocations());
        ASSERT(0 == tpc_2.invocations());

        // Reset dummy callbacks and dummy publishers
        mX.publishAll();  // Reset the prev publication time of all categories
        tcba_1.reset(); tcba_2.reset();
        tcbb_1.reset(); tcbb_2.reset();
        tcbc_1.reset(); tcbc_2.reset();
        tpa_1.reset(); tpa_2.reset();
        tpb_1.reset(); tpb_2.reset();
        tpc_1.reset(); tpc_2.reset();

        bslmt::ThreadUtil::microSleep(100000, 0);
        mX.publish(CAT_C);
        ASSERT(0 == tcba_1.invocations());
        ASSERT(0 == tcba_2.invocations());
        ASSERT(0 == tcbb_1.invocations());
        ASSERT(0 == tcbb_2.invocations());
        ASSERT(1 == tcbc_1.invocations());
        ASSERT(1 == tcbc_2.invocations());

        ASSERT(0 == tpa_1.invocations());
        ASSERT(0 == tpa_2.invocations());
        ASSERT(0 == tpb_1.invocations());
        ASSERT(0 == tpb_2.invocations());
        ASSERT(1 == tpc_1.invocations());
        ASSERT(1 == tpc_2.invocations());

        if (verbose) {
            cout << "\tVerify publish(const Category **categories,...)\n";
        }

        // Reset dummy callbacks and dummy publishers
        mX.publishAll();  // Reset the prev publication time of all categories
        tcba_1.reset(); tcba_2.reset();
        tcbb_1.reset(); tcbb_2.reset();
        tcbc_1.reset(); tcbc_2.reset();
        tpa_1.reset(); tpa_2.reset();
        tpb_1.reset(); tpb_2.reset();
        tpc_1.reset(); tpc_2.reset();

        const balm::Category *CATEGORIES[] = {CAT_A, CAT_B, CAT_C};
        const int  NUM_CATEGORIES = sizeof(CATEGORIES)/sizeof(*CATEGORIES);
        bslmt::ThreadUtil::microSleep(100000, 0);
        mX.publish(CATEGORIES, NUM_CATEGORIES);

        ASSERT(1 == tcba_1.invocations());
        ASSERT(1 == tcba_2.invocations());
        ASSERT(1 == tcbb_1.invocations());
        ASSERT(1 == tcbb_2.invocations());
        ASSERT(1 == tcbc_1.invocations());
        ASSERT(1 == tcbc_2.invocations());

        ASSERT(1 == tpa_1.invocations());
        ASSERT(1 == tpa_2.invocations());
        ASSERT(1 == tpb_1.invocations());
        ASSERT(1 == tpb_2.invocations());
        ASSERT(1 == tpc_1.invocations());
        ASSERT(1 == tpc_2.invocations());

        ASSERT(1 == tpa_1.lastElapsedTimes().size());
        LOOP_ASSERT(*tpa_1.lastElapsedTimes().begin(),
            bsls::TimeInterval(0, 0) < *tpa_1.lastElapsedTimes().begin());
        withinWindow(tpa_1.lastTimeStamp(), now, WINDOW);

        ASSERT(2 == tpa_1.lastRecords().size());
        ASSERT(2 == tpa_2.lastRecords().size());
        ASSERT( tpa_1.contains(METRIC_A));
        ASSERT( tpa_2.contains(METRIC_A));
        ASSERT(!tpa_1.contains(METRIC_B));
        ASSERT(!tpa_2.contains(METRIC_B));
        ASSERT(!tpa_1.contains(METRIC_C));
        ASSERT(!tpa_2.contains(METRIC_C));

        ASSERT(2 == tpb_1.lastRecords().size());
        ASSERT(2 == tpb_2.lastRecords().size());
        ASSERT(!tpb_1.contains(METRIC_A));
        ASSERT(!tpb_2.contains(METRIC_A));
        ASSERT( tpb_1.contains(METRIC_B));
        ASSERT( tpb_2.contains(METRIC_B));
        ASSERT(!tpb_1.contains(METRIC_C));
        ASSERT(!tpb_2.contains(METRIC_C));

        ASSERT(2 == tpc_1.lastRecords().size());
        ASSERT(2 == tpc_2.lastRecords().size());
        ASSERT(!tpc_1.contains(METRIC_A));
        ASSERT(!tpc_2.contains(METRIC_A));
        ASSERT(!tpc_1.contains(METRIC_B));
        ASSERT(!tpc_2.contains(METRIC_B));
        ASSERT( tpc_1.contains(METRIC_C));
        ASSERT( tpc_2.contains(METRIC_C));

        ASSERT(tpa_1.lastTimeStamp() == tpa_2.lastTimeStamp());
        ASSERT(tpa_1.lastTimeStamp() == tpb_1.lastTimeStamp());
        ASSERT(tpa_1.lastTimeStamp() == tpc_1.lastTimeStamp());
        ASSERT(tpb_1.lastTimeStamp() == tpb_2.lastTimeStamp());
        ASSERT(tpc_1.lastTimeStamp() == tpc_2.lastTimeStamp());

        ASSERT(*tpa_1.lastElapsedTimes().begin() ==
               *tpa_2.lastElapsedTimes().begin());
        ASSERT(*tpa_1.lastElapsedTimes().begin() ==
               *tpb_1.lastElapsedTimes().begin());
        ASSERT(*tpa_1.lastElapsedTimes().begin() ==
               *tpc_1.lastElapsedTimes().begin());
        ASSERT(*tpb_1.lastElapsedTimes().begin() ==
               *tpb_2.lastElapsedTimes().begin());
        ASSERT(*tpc_1.lastElapsedTimes().begin() ==
               *tpc_2.lastElapsedTimes().begin());

        if (verbose) {
            cout << "\tVerify publishAll(set<const balm::Category *>&,)\n";
        }

        // Reset dummy callbacks and dummy publishers
        mX.publishAll();  // Reset the prev publication time of all categories
        tcba_1.reset(); tcba_2.reset();
        tcbb_1.reset(); tcbb_2.reset();
        tcbc_1.reset(); tcbc_2.reset();
        tpa_1.reset(); tpa_2.reset();
        tpb_1.reset(); tpb_2.reset();
        tpc_1.reset(); tpc_2.reset();

        bsl::set<const balm::Category *> catSet;
        catSet.insert(CAT_C);

        bslmt::ThreadUtil::microSleep(100000, 0);
        mX.publishAll(catSet);

        ASSERT(1 == tcba_1.invocations());
        ASSERT(1 == tcba_2.invocations());
        ASSERT(1 == tcbb_1.invocations());
        ASSERT(1 == tcbb_2.invocations());
        ASSERT(0 == tcbc_1.invocations());
        ASSERT(0 == tcbc_2.invocations());

        ASSERT(1 == tpa_1.invocations());
        ASSERT(1 == tpa_2.invocations());
        ASSERT(1 == tpb_1.invocations());
        ASSERT(1 == tpb_2.invocations());
        ASSERT(0 == tpc_1.invocations());
        ASSERT(0 == tpc_2.invocations());

        ASSERT(2 == tpa_1.lastRecords().size());
        ASSERT(2 == tpa_2.lastRecords().size());
        ASSERT( tpa_1.contains(METRIC_A));
        ASSERT( tpa_2.contains(METRIC_A));
        ASSERT(!tpa_1.contains(METRIC_B));
        ASSERT(!tpa_2.contains(METRIC_B));
        ASSERT(!tpa_1.contains(METRIC_C));
        ASSERT(!tpa_2.contains(METRIC_C));

        ASSERT(2 == tpb_1.lastRecords().size());
        ASSERT(2 == tpb_2.lastRecords().size());
        ASSERT(!tpb_1.contains(METRIC_A));
        ASSERT(!tpb_2.contains(METRIC_A));
        ASSERT( tpb_1.contains(METRIC_B));
        ASSERT( tpb_2.contains(METRIC_B));
        ASSERT(!tpb_1.contains(METRIC_C));
        ASSERT(!tpb_2.contains(METRIC_C));

        if (verbose) {
            cout << "\tVerify published elapsed time values\n";
        }

        // Reset dummy callbacks and dummy publishers
        bsls::TimeInterval startTime = bdlt::CurrentTime::now();
        mX.publishAll();  // Reset the prev publication time of all categories
        tcba_1.reset(); tcba_2.reset();
        tcbb_1.reset(); tcbb_2.reset();
        tcbc_1.reset(); tcbc_2.reset();
        tpa_1.reset(); tpa_2.reset();
        tpb_1.reset(); tpb_2.reset();
        tpc_1.reset(); tpc_2.reset();

        const int TIME_UNIT = 50 * NANOSECS_PER_MILLISEC;

        bslmt::ThreadUtil::sleep(bsls::TimeInterval(0, TIME_UNIT));
        bsls::TimeInterval catATime  = bdlt::CurrentTime::now();
        bsls::TimeInterval catAIntvl = catATime - startTime;
        mX.publish(CAT_A);

        bslmt::ThreadUtil::sleep(bsls::TimeInterval(0, TIME_UNIT));
        bsls::TimeInterval catBTime  = bdlt::CurrentTime::now();
        bsls::TimeInterval catBIntvl = catBTime - startTime;
        mX.publish(CAT_B);

        ASSERT(1 == tcba_1.invocations());
        ASSERT(1 == tcba_2.invocations());
        ASSERT(1 == tcbb_1.invocations());
        ASSERT(1 == tcbb_2.invocations());
        ASSERT(0 == tcbc_1.invocations());
        ASSERT(0 == tcbc_2.invocations());

        ASSERT(1 == tpa_1.lastElapsedTimes().size());
        ASSERT(1 == tpb_1.lastElapsedTimes().size());

        ASSERT(withinWindow(*tpa_1.lastElapsedTimes().begin(), catAIntvl, 2));
        ASSERT(withinWindow(*tpb_1.lastElapsedTimes().begin(), catBIntvl, 2));

        bslmt::ThreadUtil::sleep(bsls::TimeInterval(0, TIME_UNIT));
        bsls::TimeInterval allTime  = bdlt::CurrentTime::now();
        mX.publishAll();

        catAIntvl = allTime - catATime;
        catBIntvl = allTime - catBTime;
        bsls::TimeInterval catCIntvl = allTime - startTime;

        ASSERT(2 == tcba_1.invocations());
        ASSERT(2 == tcba_2.invocations());
        ASSERT(2 == tcbb_1.invocations());
        ASSERT(2 == tcbb_2.invocations());
        ASSERT(1 == tcbc_1.invocations());
        ASSERT(1 == tcbc_2.invocations());

        ASSERT(1 == tpa_1.lastElapsedTimes().size());
        ASSERT(1 == tpb_1.lastElapsedTimes().size());
        ASSERT(1 == tpc_1.lastElapsedTimes().size());

        ASSERT(withinWindow(*tpa_1.lastElapsedTimes().begin(), catAIntvl, 2));
        ASSERT(withinWindow(*tpb_1.lastElapsedTimes().begin(), catBIntvl, 2));
        ASSERT(withinWindow(*tpc_1.lastElapsedTimes().begin(), catCIntvl, 2));

        if (verbose) {
            cout << "\tVerify collectSample()\n";
        }

        mX.publishAll();  // Reset the prev publication time of all categories
        tcba_1.reset(); tcba_2.reset();
        tcbb_1.reset(); tcbb_2.reset();
        tcbc_1.reset(); tcbc_2.reset();
        tpa_1.reset(); tpa_2.reset();
        tpb_1.reset(); tpb_2.reset();
        tpc_1.reset(); tpc_2.reset();

        balm::MetricSample sample;
        bsl::vector<balm::MetricRecord> records;

        mX.collectSample(&sample, &records);

        ASSERT(1 == tcba_1.invocations());
        ASSERT(1 == tcba_2.invocations());
        ASSERT(1 == tcbb_1.invocations());
        ASSERT(1 == tcbb_2.invocations());
        ASSERT(1 == tcbc_1.invocations());
        ASSERT(1 == tcbc_2.invocations());

        ASSERT(6 == records.size());
        ASSERT(6 == sample.numRecords());
        ASSERT(3 == sample.numGroups());

        if (verbose) {
            sample.print(bsl::cout, 1, 3);
        }

        ASSERT(false == tcba_1.resetFlag());
        ASSERT(false == tcbb_1.resetFlag());
        ASSERT(false == tcbc_1.resetFlag());

        if (verbose) {
            cout << "\tVerify remove callbacks\n";
        }

        // Reset dummy callbacks and dummy publishers
        mX.publishAll();  // Reset the prev publication time of all categories
        tcba_1.reset(); tcba_2.reset();
        tcbb_1.reset(); tcbb_2.reset();
        tcbc_1.reset(); tcbc_2.reset();
        tpa_1.reset(); tpa_2.reset();
        tpb_1.reset(); tpb_2.reset();
        tpc_1.reset(); tpc_2.reset();

        ASSERT(0 == mX.removeCollectionCallback(handle0));
        ASSERT(0 != mX.removeCollectionCallback(handle0));

        mX.publish(CAT_A);

        ASSERT(0 == tcba_1.invocations());
        ASSERT(1 == tcba_2.invocations());

        ASSERT(1 == tpa_1.invocations());
        ASSERT(1 == tpa_2.invocations());

        ASSERT(0 == mX.removeCollectionCallback(handle1));
        ASSERT(0 != mX.removeCollectionCallback(handle1));

        mX.publish(CAT_A);

        ASSERT(0 == tcba_1.invocations());
        ASSERT(1 == tcba_2.invocations());

        ASSERT(1 == tpa_1.invocations());
        ASSERT(1 == tpa_2.invocations());

        if (verbose) {
            cout << "\tVerify remove publishers\n";
        }
        pVec.clear();
        ASSERT(0 == mX.removePublisher(pA_1));
        ASSERT(0 != mX.removePublisher(pA_1));
        ASSERT(1 == MX.findSpecificPublishers(&pVec, CAT_A));
        ASSERT(1 == pVec.size());
        ASSERT(pA_2.get() == pVec[0]);

        pVec.clear();
        ASSERT(0 == mX.removePublisher(pA_2));
        ASSERT(0 == MX.findSpecificPublishers(&pVec, CAT_A));
        ASSERT(0 == pVec.size());

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
