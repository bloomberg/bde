// baem_publicationscheduler.t.cpp                                    -*-C++-*-
#include <baem_publicationscheduler.h>

#include <baem_publisher.h>
#include <baem_metricsmanager.h>
#include <baem_metricsample.h>

#include <bael_defaultobserver.h>
#include <bael_log.h>
#include <bael_loggermanager.h>
#include <bael_severity.h>
#include <bcep_timereventscheduler.h>
#include <bcemt_thread.h>
#include <bdef_bind.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_defaultallocatorguard.h>

#include <bcemt_barrier.h>
#include <bcep_fixedthreadpool.h>
#include <bcema_testallocator.h>

#include <bsl_ostream.h>
#include <bsl_cstring.h>
#include <bsl_cstdlib.h>
#include <bsl_c_stdio.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsls_assert.h>

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define snprintf _snprintf_s
#endif

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//  The component under test is a scheduling mechanism that uses a
//  'bcep_TimerEventScheduler' supplied at construction to providing
//  scheduling events.  This test driver defines a generator language 'gg' to
//  easily describe a sequence of scheduling operations.   The primary
//  manipulators and accessors are verified by comparing their results against
//  an "oracle" 'CategorySchedule' (a bsl::map of category to frequency), for
//  a set of test cases defined using the generator language.  The more
//  difficult test cases (4 & 5), verify that the correct clocks are
//  registered with the underlying timer event scheduler, and that the correct
//  set of categories are published for each timer event.  For those test
//  cases we define a helper class, 'TestPublisher', and use it to record the
//  invocations of the 'baem_MetricsManager' object's 'publish' method over a
//  period of time and compare those to their expected values.
//-----------------------------------------------------------------------------
// CREATORS
// [ 3] baem_PublicationScheduler(baem_MetricsManager *,
//                                bcep_TimerEventScheduler *,
//                                bslma_Allocator *);
// [ 3] ~baem_PublicationScheduler();
// MANIPULATORS
// [10] void scheduleCategory(const bdeut_StringRef&   ,
//                            const bdet_TimeInterval& );
// [ 3] void scheduleCategory(const baem_Category      *,
//                            const bdet_TimeInterval&  );
// [ 3] void setDefaultSchedule(const bdet_TimeInterval& );
// [10] int cancelCategorySchedule(const bdeut_StringRef& );
// [ 6] int cancelCategorySchedule(const baem_Category *);
// [ 6] int clearDefaultSchedule();
// [ 7] void cancelAll();
// [ 3] baem_MetricsManager *manager();
// ACCESSORS
// [10] bool findCategorySchedule(bdet_TimeInterval *,
//                                const bdeut_StringRef&  ) const;
// [ 3] bool findCategorySchedule(bdet_TimeInterval*,
//                                const baem_Category*) const;
// [ 3] bool getDefaultSchedule(bdet_TimeInterval *) const;
// [ 8] int getCategorySchedule(
//                        bsl::vector<bsl::pair<const baem_Category *,
//                                              bdet_TimeInterval> > * ) const;
// [ 3] const baem_MetricsManager *manager() const;
// [ 9] bsl::ostream& print(bsl::ostream&, int, spacesPerLevel ) const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] 'gg', 'TestPublisher'                       (generator, helper classes)
// [ 4] PRIVATE METHOD TEST: 'publish'              (private method)
// [ 5] AUXILIARY TEST: SCHEDULING FREQUENCY        (behavior matches schedule)
// [11] BAEM ALLOCATION EXCEPTION TEST
// [12] CONCURRENCY TEST
// [13] USAGE

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
//                *NON* STANDARD BAEM TEST MACROS
//-----------------------------------------------------------------------------

// Code defined between the 'BAEM_BEGIN_RETRY_TEST' and 'BAEM_END_RETRY_TEST'
// macros will executed once, and, if there is a test failure, executed again
// up to the specified number of 'ATTEMPTS'.  These macros are used to bracket
// code that may intermittently fail: tests that depend on operating system
// timers that, under heavy load, are unreliable.  If the bracketed test
// code succeeds in under 'ATTEMPTS' iterations, the test is successful and
// 'testStatus' will not be modified (though error messages may be reported to
// the console), otherwise the test will fail and 'testStatus' will not be 0.
#define BAEM_BEGIN_RETRY_TEST_IMP(ATTEMPTS, QUIET) {                          \
    {                                                                         \
        static int firstTime = 1;                                             \
        if (veryVerbose && firstTime) cout <<                                 \
            "### BAEM RETRY TEST" << endl;                                    \
        firstTime = 0;                                                        \
    }                                                                         \
    if (veryVeryVerbose) cout << "### Begin baem retry test." << endl;        \
    const int  __numAttempts = (ATTEMPTS);                                    \
    const int  __savedStatus = testStatus;                                    \
    int        __attempt     = 0;                                             \
    const bool __quietMode   = (QUIET);                                       \
    do {                                                                      \
        if (0 != __attempt && !__quietMode) {                                 \
            cout << "Test failure; retry.  Attempt: "                         \
                 << __attempt + 1 << " of " << __numAttempts                  \
                 << endl;                                                     \
        }                                                                     \
        testStatus = __savedStatus;

#define BAEM_BEGIN_RETRY_TEST(ATTEMPTS)  \
                                     BAEM_BEGIN_RETRY_TEST_IMP(ATTEMPTS, false)

#define BAEM_END_RETRY_TEST                                                   \
    } while (testStatus !=  __savedStatus && ++__attempt < __numAttempts);    \
    if (__savedStatus != testStatus) {                                        \
        if (!__quietMode) {                                                   \
            cout << "RETRY_TEST failure after " << __numAttempts              \
                  << " attempts" << endl;                                     \
        }                                                                     \
    }                                                                         \
}

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef baem_PublicationScheduler        Obj;
typedef bsl::pair<const baem_Category *,
                  bdet_TimeInterval>     ScheduleElement;
typedef bsl::vector<ScheduleElement>     Schedule;
typedef baem_Publisher                   Pub;
typedef bcema_SharedPtr<Pub>             PubPtr;
typedef baem_MetricId                    Id;
typedef baem_Category                    Category;
typedef bsl::map<const Category *,
                 bdet_TimeInterval>      CategoryScheduleOracle;
typedef bsl::map<bdet_TimeInterval,
                 bsl::set<const baem_Category *> >
                                         ScheduleOracle;

enum {
    MILLISECS_PER_SEC      = 1000,        // one thousand
    MICROSECS_PER_SEC      = 1000000,     // one million
    NANOSECS_PER_MICROSEC  = 1000,        // one thousand
    NANOSECS_PER_MILLISEC  = 1000000,     // one million
    NANOSECS_PER_SEC       = 1000000000,  // one billion
    MICROSECS_PER_MILLISEC = 1000         // one thousand
};

//=============================================================================
//                 NON-STANDARD BAEM EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

// Note that the following exception test macros are similar to the standard
// 'BSLMA_EXCEPTION_TEST' macros but, perform additional tests to verify the
// internal state of the publication scheduler is reset when an exception
// occurs when modifying the scheduler.  The macros require expect a
// 'bcep_TimerEventScheduler' object named 'timer' and
// 'baem_PublicationScheduler' named 'MX'.

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BAEM_EXCEPTION_TEST {                                       \
    {                                                                     \
        static int firstTime = 1;                                         \
        if (veryVerbose && firstTime) cout <<                             \
            "### BAEM EXCEPTION TEST -- (ENABLED) --" << endl;            \
        firstTime = 0;                                                    \
    }                                                                     \
    if (veryVeryVerbose) cout <<                                          \
        "### Begin bdema exception test." << endl;                        \
    int bdemaExceptionCounter = 0;                                        \
    static int bdemaExceptionLimit = 100;                                 \
    testAllocator.setAllocationLimit(bdemaExceptionCounter);              \
    do {                                                                  \
        try {

#define END_BAEM_EXCEPTION_TEST                                           \
        } catch (bslma_TestAllocatorException& e) {                       \
            Schedule schedule(Z);                                         \
            bdet_TimeInterval interval;                                   \
            ASSERT(0  == timer.numClocks());                              \
            ASSERT(0  == timer.numEvents());                              \
            ASSERT(0  == MX.getCategorySchedule(&schedule));              \
            ASSERT(!MX.getDefaultSchedule(&interval));                    \
            if (veryVerbose && bdemaExceptionLimit || veryVeryVerbose) {  \
                --bdemaExceptionLimit;                                    \
                cout << "(*** " << bdemaExceptionCounter << ')';          \
                if (veryVeryVerbose) { cout << " BEDMA_EXCEPTION: "       \
                    << "alloc limit = " << bdemaExceptionCounter << ", "  \
                    << "last alloc size = " << e.numBytes();              \
                }                                                         \
                else if (0 == bdemaExceptionLimit) {                      \
                    cout << " [ Note: 'bdemaExceptionLimit' reached. ]";  \
                }                                                         \
                cout << endl;                                             \
            }                                                             \
            testAllocator.setAllocationLimit(++bdemaExceptionCounter);    \
            continue;                                                     \
        }                                                                 \
        testAllocator.setAllocationLimit(-1);                             \
        break;                                                            \
    } while (1);                                                          \
    if (veryVeryVerbose) cout <<                                          \
        "### End bdema exception test." << endl;                          \
}
#else
#define BEGIN_BAEM_EXCEPTION_TEST                                         \
{                                                                         \
    static int firstTime = 1;                                             \
    if (verbose && firstTime) { cout <<                                   \
        "### BAEM EXCEPTION TEST -- (NOT ENABLED) --" << endl;            \
        firstTime = 0;                                                    \
    }                                                                     \
}
#define END_BAEM_EXCEPTION_TEST
#endif

//=============================================================================
//                      CLASSES FOR TESTING
//-----------------------------------------------------------------------------

// from 'bcep_timereventscheduler.t.cpp'.
void microSleep(int microSeconds, int seconds)
    // Sleep for *at* *least* the specified 'seconds' and 'microseconds'.  This
    // function is used for testing only.  It uses the function
    // 'bcemt_ThreadUtil::microSleep' but interleaves calls to 'yield' to give
    // a chance to the event scheduler to process its dispatching thread.
    // Without this, there have been a large number of unpredictable
    // intermittent failures by this test driver, especially on AIX with
    // xlc-8.0, in the nightly builds (i.e., when the load is higher than
    // running the test driver by hand).  It was noticed that calls to 'yield'
    // helped, and this routine centralizes this as a mechanism.
{
    bcemt_ThreadUtil::microSleep(microSeconds / 2, seconds / 2);
    bcemt_ThreadUtil::yield();
    bcemt_ThreadUtil::microSleep(microSeconds / 2, seconds / 2);
    bcemt_ThreadUtil::yield();
}

bool withinWindow(const bdet_TimeInterval& value,
                  const bdet_TimeInterval& expectedValue,
                  int                      windowMs)
    // Return 'true' if the specified 'value' is within the specified
    // 'windowMs' (milliseconds) of the specified 'expectedValue'.
{
    bdet_TimeInterval window(0, windowMs * NANOSECS_PER_MILLISEC);
    bool withinWindow = (expectedValue - window) < value
                     && (expectedValue + window) > value;

    if (!withinWindow) {
        P_(windowMs); P_(expectedValue); P(value);
    }
    return withinWindow;
}

                      // ===================
                      // class TestPublisher
                      // ===================

class TestPublisher : public baem_Publisher {
    // This class defines a test implementation of the 'baem_Publisher'
    // protocol that can be used to record information about invocations of a
    // 'baem_MetricsManager' object's 'publish' method.  Each 'TestPublisher'
    // instance tracks the number of times 'publish' has been called, and
    // maintains 'lastElapsedTime()' and 'lastTimeStamp()' values holding the
    // elapsed time and time stamp values of the last published
    // 'baem_MetricSample' object.  In addition, the 'TestPublisher' maintains
    // a map of invocation information.  Each 'baem_MetricSample' passed to the
    // 'publish' method is identified by the *set* of *categories* that appear
    // in the sequence of records held by that sample.  For each unique set of
    // categories passed to the 'publish' method (via a 'baem_MetricSample'
    // object) a 'TestPublisher' object will record: the number of times the
    // 'publish' method has been invoked with that set of categories, as well
    // as the time stamp and elapsed time of the last metric sample passed to
    // 'publish' with that identifying set of categories.  Note that the
    // invocation information is explicitly designed to test the
    // 'baem_PublicationScheduler::publish' method, which invokes
    // 'baem_MetricsManager::publish' with a unique set of categories for the
    // supplied time interval.

  public:

    typedef bsl::set<const Category *> InvocationId;
        // Invocations of 'TestPublisher::publish' are identified by the set
        // of categories contained in sequence of records of the published the
        // 'baem_MetricSample'.

    struct InvocationSummary {
        // This struct provides an value semantic type for holding summary
        // information about a "unique" invocation of a 'TestPublisher'.  Each
        // invocation of the 'TestPublisher::publish' method is identified by
        // the *set* of categories found in the records of the
        // 'baem_MetricSample' passed to the 'publish' method.  An
        // 'InvocationSummary' object holds: the number of times that the
        // unique set of categories was published, the time stamp of the most
        // recent metric sample with that set of categories, and the elapsed
        // time of the most recent metric sample with that set of categories.

        int                             d_numInvocations; // # of invocations

        bdet_DatetimeTz                 d_timeStamp;      // last time stamp

        bdet_TimeInterval               d_elapsedTime;    // last elapsed time

         // CREATORS
         InvocationSummary();
            // Create a 'InvocationSummary' object whose number of invocations
            // is 0.
    };

  private:
    // PRIVATE TYPES
    struct InvocationIdLess {
        // This structure provides an ordering on sets of category addresses
        // allowing them to be sorted and used in sorted containers such as
        // 'bsl::map'.

        bool operator()(const bsl::set<const baem_Category *>& lhs,
                        const bsl::set<const baem_Category *>& rhs) const;
            // Return 'true' if the specified 'lhs' is less than, i.e., ordered
            // before, the specified 'rhs' and 'false' otherwise.
    };

    typedef bsl::map<InvocationId,
                     InvocationSummary,
                     InvocationIdLess> InvocationMap;
        // An 'InvocationMap' maintains a map a set of categories that
        // identifies an invocation, to the summary of the invocation
        // information for that identifying set of categories.

    // DATA
    bces_AtomicInt              d_numInvocations; // # of invocations

    InvocationMap               d_invocations;    // map of invocation
                                                  // information

    bdet_DatetimeTz             d_timeStamp;      // last time stamp

    bsl::set<bdet_TimeInterval> d_elapsedTimes;   // last elapsed times

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
        // to be the list of sequence in 'sample' in *sorted* order.

    void reset();
        // Set 'invocations()' to 0, clear the invocation information.

    // ACCESSORS
    int invocations() const;
        // Return the number of times the 'publish' method has been invoked
        // since this test publisher was constructed or the most recent call to
        // 'reset()'.

    int uniqueInvocations() const;
        // Return the number of unique *sets* of categories found in the
        // 'baem_MetricSample' objects passed to 'publish' since this test
        // publisher was created or the most recent call to 'reset()'.

    const InvocationSummary *findInvocation(
                     const bsl::set<const baem_Category *>& categories) const;
        // Return the address of the non-modifiable 'Invocation' corresponding
        // to the specified set of 'categories', or 0 if 'publish' has not
        // been invoked with a 'baem_MetricSample' containing 'categories'.

    const bdet_TimeInterval& lastElapsedTime() const;
        // Return a reference to the non-modifiable elapsed time value of the
        // last sample passed to the 'publish' method.  This method will
        // 'ASSERT' if the last 'baem_MetricSample' passed to the 'publish'
        // method contained more than one unique time interval.  The behavior
        // is undefined unless the 'publish' method has invoked since this
        // object was created or last reset.

    const bdet_DatetimeTz& lastTimeStamp() const;
        // Return a reference to the non-modifiable time stamp of the last
        // sample passed to the 'publish' method.  The behavior is undefined
        // unless the 'publish' method has invoked since this object was
        // created or last reset.

    bsl::ostream& print(bsl::ostream&   stream,
                        int             level = 0,
                        int             spacesPerLevel = 4) const;
        // Print a formatted string describing the "unique" invocations on this
        // publisher to the specified 'stream' at the (absolute value of) the
        // optionally specified indentation 'level' and return a reference to
        // 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, suppress all indentation AND format the entire output on
        // one line.  If 'stream' is not valid on entry, this operation has no
        // effect.

};

                      // --------------------------------------
                      // class TestPublisher::InvocationSummary
                      // --------------------------------------

// CREATORS
inline
TestPublisher::InvocationSummary::InvocationSummary()
: d_numInvocations(0)
, d_timeStamp()
, d_elapsedTime()
{
}
                      // -------------------------------------
                      // class TestPublisher::InvocationIdLess
                      // -------------------------------------

// ACCESSORS
bool TestPublisher::InvocationIdLess::operator()(
                             const bsl::set<const baem_Category *>& lhs,
                             const bsl::set<const baem_Category *>& rhs) const
{
    if (lhs.size() < rhs.size()) {
        return true;
    }
    if (lhs.size() > rhs.size()) {
        return false;
    }
    bsl::set<const baem_Category *>::const_iterator lIt = lhs.begin();
    bsl::set<const baem_Category *>::const_iterator rIt = rhs.begin();
    for (; lIt != lhs.end(); ++lIt, ++rIt) {
        if (*lIt < *rIt) return true;
        if (*lIt > *rIt) return false;
    }
    return false;
}

                      // -------------------
                      // class TestPublisher
                      // -------------------

// CREATORS
inline
TestPublisher::TestPublisher(bslma_Allocator *allocator)
: d_numInvocations(0)
, d_invocations(allocator)
, d_timeStamp()
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
    BSLS_ASSERT(0 != sample.numRecords());

    d_elapsedTimes.clear();
    for (int i = 0; i < sample.numGroups(); ++i) {
        d_elapsedTimes.insert(sample.sampleGroup(i).elapsedTime());
    }
    d_timeStamp   = sample.timeStamp();
    d_numInvocations++;

    bsl::set<const baem_Category *> categories;
    baem_MetricSample::const_iterator sIt = sample.begin();
    for (; sIt != sample.end(); ++sIt) {
        baem_MetricSampleGroup::const_iterator gIt = sIt->begin();
        for (; gIt != sIt->end(); ++gIt) {
            categories.insert(gIt->metricId().category());
        }
    }
    InvocationSummary& invocation = d_invocations[categories];
    invocation.d_timeStamp   = sample.timeStamp();
    invocation.d_elapsedTime = *d_elapsedTimes.begin();
    invocation.d_numInvocations++;
}

inline
void TestPublisher::reset()
{
    d_numInvocations = 0;
    d_invocations.clear();
    d_elapsedTimes.clear();
}

// ACCESSORS
inline
int TestPublisher::invocations() const
{
    return d_numInvocations;
}

inline
int TestPublisher::uniqueInvocations() const
{
    return d_invocations.size();
}

inline
const TestPublisher::InvocationSummary *TestPublisher::findInvocation(
                     const bsl::set<const baem_Category *>& categories) const
{
    InvocationMap::const_iterator it = d_invocations.find(categories);
    if (it == d_invocations.end()) {
        return 0;
    }
    return &it->second;
}

inline
const bdet_TimeInterval& TestPublisher::lastElapsedTime() const
{
    ASSERT(1 == d_elapsedTimes.size());
    return *d_elapsedTimes.begin();
}

inline
const bdet_DatetimeTz& TestPublisher::lastTimeStamp() const
{
    return d_timeStamp;
}

bsl::ostream& TestPublisher::print(bsl::ostream&   stream,
                                   int             level,
                                   int             spacesPerLevel) const
{
    char SEP = (level <= 0) ? ' ' : '\n';
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "[" << SEP;
    InvocationMap::const_iterator it = d_invocations.begin();
    for (; it != d_invocations.end(); ++it) {
        bdeu_Print::indent(stream, level + 1, spacesPerLevel);
        stream << "[ "
               << it->second.d_numInvocations << " "
               << it->second.d_timeStamp << " "
               << it->second.d_elapsedTime << " ( ";
        bsl::set<const baem_Category *>::const_iterator cIt =
                                             it->first.begin();
        for (; cIt != it->first.end(); ++cIt) {
            stream << (*cIt)->name() << " ";
        }
        stream << ") ]" << SEP;
    }
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "]" << SEP;
    return stream;
}

                        // ============
                        // class Action
                        // ============

class Action {
     // This class defines a value semantic type used to describe a
     // scheduling action to be performed on a 'baem_PublicationScheduler'.
     // Each action is either a call to 'scheduleCategory' or
     // 'setDefaultSchedule' and contain the time interval to schedule for and,
     // optionally (if the type() is 'SCHEDULE_CATEGORY'), the address of the
     // 'baem_Category' object to schedule.  Note that this class is designed
     // to work with the 'gg' generator function defined below.
     //
     // IMPLEMENTATION NOTE: This class uses the default compiler supplied
     // copy and assignment operators.

  public:
    // PRIVATE TYPES
    enum Type {
        // Describes the type of action to perform on the
        // 'baem_PublicationScheduler'.  'SCHEDULE_CATEGORY' corresponds to
        // calling the 'scheduleCategory' method, while 'SCHEDULE_DEFAULT'
        // corresponds to calling 'setDefaultSchedule'.

        SCHEDULE_CATEGORY,
        SCHEDULE_DEFAULT
    };

  private:
    Type                 d_type;        // type of action
    const baem_Category *d_category_p;  // category to schedule (or 0)
    int                  d_interval;    // interval to schedule for

  public:
     // CLASS METHODS
     static const char *typeString(Type type);
        // Return a string describing the specified 'type'.

     // CREATORS
     Action(Type type, const baem_Category *category, int interval);
        // Create an 'Action' with the specified 'type', 'category', and
        // 'interval.

     // ACCESSORS
     Type type() const;
         //  Return the type of action to perform on a
         //  'baem_PublicationScheduler' object.  A value of
         //  'SCHEDULE_CATEGORY' indicates a call to the 'scheduleCategory'
         //  method, while the value 'SCHEDULE_DEFAULT' indicates a call to
         //  'setDefaultSchedule'.

     const baem_Category *category() const;
        // Return the address of the non modifiable category to schedule or 0
        // if 'type() == SCHEDULE_DEFAULT'.

     int interval() const;
        // Return the amount of time to schedule.  Note that the value is a
        // relative time value, and no specific unit of time is implied.
};

bsl::ostream& operator<<(bsl::ostream& stream, const Action& action)
    // Write the specified 'action' to the specified 'stream'.
{
    stream << "[ " << Action::typeString(action.type()) << " "
           << (action.category() ? action.category()->name() : "")
           << " " << action.interval() << " ]";
    return stream;
}

bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bsl::vector<Action>& actions)
    // Write the specified 'actions' to the specified 'stream'.
{
    bsl::vector<Action>::const_iterator it = actions.begin();
    stream << "[ ";
    for (; it != actions.end(); ++it) {
        stream << *it << " ";
    }
    stream << " ]";
    return stream;
}

// CLASS METHODS
const char *Action::typeString(Type type)
{
    switch (type) {
      case Action::SCHEDULE_CATEGORY: return "SCHEDULE_CATEGORY";
      case Action::SCHEDULE_DEFAULT: return "SCHEDULE_DEFAULT";
    }
    BSLS_ASSERT(false);
    return "Invalid Action Type";
}

// CREATORS
inline
Action::Action(Type type, const baem_Category *category, int interval)
: d_type(type)
, d_category_p(category)
, d_interval(interval)
{
}

// ACCESSORS
inline
Action::Type Action::type() const
{
    return d_type;
}

inline
const baem_Category *Action::category() const
{
    return d_category_p;
}

inline
int Action::interval() const
{
    return d_interval;
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
    bcep_FixedThreadPool         d_pool;
    bcemt_Barrier                d_barrier;
    baem_PublicationScheduler   *d_scheduler_p;
    bcep_TimerEventScheduler    *d_eventScheduler_p;
    bslma_Allocator             *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.  Note that this method contains the actual
        // test code.

  public:

    // CREATORS
    ConcurrencyTest(int                        numThreads,
                    baem_PublicationScheduler *scheduler,
                    bcep_TimerEventScheduler  *eventScheduler,
                    bslma_Allocator           *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_scheduler_p(scheduler)
    , d_eventScheduler_p(eventScheduler)
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
    bsl::string uniqueString1;
    stringId(&uniqueString1, "US",  bcemt_ThreadUtil::selfIdAsInt());
    const char *S1 = uniqueString1.c_str();

    Schedule schedule(d_allocator_p);

    Obj&  mX = *d_scheduler_p; const Obj &MX = mX;

    bdet_TimeInterval foundInterval;

    // Execute this test twice: once with the event scheduler stopped, the
    // other with it started.
    for (int testRun = 0; testRun < 2; ++testRun) {

        d_eventScheduler_p->stop();
        d_barrier.wait();

        ASSERT(0 == d_eventScheduler_p->numClocks());
        ASSERT(0 == d_eventScheduler_p->numEvents());

        if (testRun > 0) {
            d_eventScheduler_p->start();
        }
        d_barrier.wait();

        // TEST 1: Without cancelAll.
        for (int i = 0; i < 50; ++i) {
            const int INTERVAL_1 = (i % 10) + 1;
            const int INTERVAL_2 = 11 - INTERVAL_1;
            // schedule
            mX.scheduleCategory("A", bdet_TimeInterval(1));
            mX.scheduleCategory("B", bdet_TimeInterval(2));
            mX.scheduleCategory("C", bdet_TimeInterval(3));
            mX.scheduleCategory("D", bdet_TimeInterval(4));
            mX.scheduleCategory("E", bdet_TimeInterval(5));
            mX.scheduleCategory("F", bdet_TimeInterval(6));
            mX.scheduleCategory(S1,  bdet_TimeInterval(INTERVAL_1));
            mX.setDefaultSchedule(bdet_TimeInterval(5));

            ASSERT(MX.findCategorySchedule(&foundInterval, "A"));
            ASSERT(bdet_TimeInterval(1) == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "B"));
            ASSERT(bdet_TimeInterval(2) == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, S1));
            ASSERT(bdet_TimeInterval(INTERVAL_1) == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "C"));
            ASSERT(bdet_TimeInterval(3) == foundInterval ||
                   bdet_TimeInterval(1) == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "D"));
            ASSERT(bdet_TimeInterval(4) == foundInterval ||
                   bdet_TimeInterval(2) == foundInterval);
            MX.findCategorySchedule(&foundInterval, "E");
            MX.findCategorySchedule(&foundInterval, "F");

            bool defaultSet = MX.getDefaultSchedule(&foundInterval) ;
            ASSERT(!defaultSet ||
                   bdet_TimeInterval(5) == foundInterval ||
                   bdet_TimeInterval(7) == foundInterval);

            mX.scheduleCategory(S1,  bdet_TimeInterval(INTERVAL_2));
            mX.scheduleCategory("C", bdet_TimeInterval(1));
            mX.scheduleCategory("D", bdet_TimeInterval(2));
            ASSERT(MX.findCategorySchedule(&foundInterval, S1));
            ASSERT(bdet_TimeInterval(INTERVAL_2) == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "C"));
            ASSERT(bdet_TimeInterval(3) == foundInterval ||
                   bdet_TimeInterval(1) == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "D"));
            ASSERT(bdet_TimeInterval(4) == foundInterval ||
                   bdet_TimeInterval(2) == foundInterval);
            MX.findCategorySchedule(&foundInterval, "E");
            MX.findCategorySchedule(&foundInterval, "F");

            mX.cancelCategorySchedule("E");
            mX.cancelCategorySchedule("F");
            mX.cancelCategorySchedule(S1);
            mX.clearDefaultSchedule();

            ASSERT(!MX.findCategorySchedule(&foundInterval, S1));
            mX.setDefaultSchedule(bdet_TimeInterval(7));
        }
        d_barrier.wait();
        ASSERT(3 == d_eventScheduler_p->numClocks());
        ASSERT(0 == d_eventScheduler_p->numEvents());
        d_barrier.wait();
        // TEST 2: With cancelAll.
        for (int i = 0; i < 50; ++i) {
            bool found;
            const int INTERVAL_1 = (i % 10) + 1;
            const int INTERVAL_2 = 11 - INTERVAL_1;

            // schedule
            mX.scheduleCategory("A", bdet_TimeInterval(1));
            mX.scheduleCategory("B", bdet_TimeInterval(2));
            mX.scheduleCategory("C", bdet_TimeInterval(3));
            mX.scheduleCategory("D", bdet_TimeInterval(4));
            mX.scheduleCategory(S1,  bdet_TimeInterval(INTERVAL_1));
            mX.setDefaultSchedule(bdet_TimeInterval(5));

            // re-schedule
            mX.scheduleCategory("C", bdet_TimeInterval(1));
            mX.scheduleCategory("D", bdet_TimeInterval(1));

            // find
            found = MX.findCategorySchedule(&foundInterval, "A");
            ASSERT(!found || foundInterval == bdet_TimeInterval(1));
            found = MX.findCategorySchedule(&foundInterval, "B");
            ASSERT(!found || foundInterval == bdet_TimeInterval(2));
            found = MX.findCategorySchedule(&foundInterval, "C");
            ASSERT(!found ||
                   foundInterval == bdet_TimeInterval(3) ||
                   foundInterval == bdet_TimeInterval(1));
            found = MX.findCategorySchedule(&foundInterval, "D");
            ASSERT(!found ||
                   foundInterval == bdet_TimeInterval(4) ||
                   foundInterval == bdet_TimeInterval(1));
            found = MX.findCategorySchedule(&foundInterval, S1);
            ASSERT(!found ||
                   foundInterval == bdet_TimeInterval(INTERVAL_1));
            found = MX.getDefaultSchedule(&foundInterval);
            ASSERT(!found ||
                   foundInterval == bdet_TimeInterval(5) ||
                   foundInterval == bdet_TimeInterval(1));

            // cancel
            mX.cancelCategorySchedule("B");
            mX.cancelCategorySchedule("C");
            mX.cancelCategorySchedule("D");
            mX.cancelCategorySchedule(S1);
            mX.clearDefaultSchedule();

            // schedule
            mX.scheduleCategory("A", bdet_TimeInterval(1));
            mX.scheduleCategory("B", bdet_TimeInterval(2));
            mX.scheduleCategory("C", bdet_TimeInterval(3));
            mX.scheduleCategory("D", bdet_TimeInterval(4));
            mX.scheduleCategory(S1,  bdet_TimeInterval(INTERVAL_1));
            mX.setDefaultSchedule(bdet_TimeInterval(1));

            // cancelAll
            mX.cancelAll();

        }
        d_barrier.wait();
        ASSERT(!MX.findCategorySchedule(&foundInterval, "A"));
        ASSERT(!MX.findCategorySchedule(&foundInterval, "B"));
        ASSERT(!MX.findCategorySchedule(&foundInterval, "C"));
        ASSERT(!MX.findCategorySchedule(&foundInterval, "D"));
        ASSERT(!MX.getDefaultSchedule(&foundInterval));
        ASSERT(0 == d_eventScheduler_p->numClocks());
        ASSERT(0 == d_eventScheduler_p->numEvents());
        d_eventScheduler_p->stop();
    }
}

void ConcurrencyTest::runTest()
{
    bdef_Function<void(*)()> job = bdef_BindUtil::bindA(
                                              d_allocator_p,
                                              &ConcurrencyTest::execute,
                                              this);
    for (int i = 0; i < d_pool.numThreads(); ++i) {
        d_pool.enqueueJob(job);
    }
    d_pool.drain();
}

//=============================================================================
//                        Helper Functions
//=============================================================================

inline bdet_TimeInterval invalidInterval()
    // Return the value used to indicate an invalid schedule interval.
{
    return bdet_TimeInterval(-1, 0);
}

int countUniqueIntervals(const CategoryScheduleOracle& scheduleOracle,
                         const bdet_TimeInterval&      defaultInterval)
    // Return the number unique time intervals values in the set of time
    // intervals in the specified 'scheduleOracle' unioned with
    // 'defaultInterval' (if 'defaultInterval' is valid).
{
    bsl::set<bdet_TimeInterval> intervals;
    if (defaultInterval != invalidInterval()) {
        intervals.insert(defaultInterval);
    }
    CategoryScheduleOracle::const_iterator it = scheduleOracle.begin();
    for (; it != scheduleOracle.end(); ++it) {
        intervals.insert(it->second);
    }
    return intervals.size();
}

bool equalSchedule(const CategoryScheduleOracle& oracle,
                   const Schedule&               schedule)
    // Return 'true' if the specified 'oracle' schedule is equivalent to the
    // specified 'scheduler'.  The two values are considered equal if they
    // have the same number of elements and and that for each
    // (category address, interval) pair in 'schedule' there is an equivalent
    // pair in 'oracle'.
{
    if (oracle.size() != schedule.size()) {
        return false;
    }
    Schedule::const_iterator it = schedule.begin();
    for (; it != schedule.end(); ++it) {
        CategoryScheduleOracle::const_iterator fIt = oracle.find(it->first);
        if (fIt == oracle.end() || (it->second != fIt->second)) {
            return false;
        }
    }
    return true;
}

bool stringDiff(const bsl::string& expectedValue,
                const bsl::string& actualValue)
    // Return 'true' if the specified 'expectedValue' equals the specified
    // 'actualValue', otherwise return 'false' and output to the console
    // information about the lowest index where the two values differ.
{
    const char *exp = expectedValue.c_str();
    const char *act = actualValue.c_str();

    int index = 0;
    for (; *exp && (*exp == *act); ++exp, ++act, ++index) {
    }
    if (*exp == *act) {
        return true;
    }
    bsl::cout << "expcetedValue[" << index << "] = " << (int)*exp
              << " (" << *exp << ")" << bsl::endl
              << "actualValue[" << index << "] = " << (int)*act
              << " (" << *act << ")" << bsl::endl;

    bsl::cout << (int)*exp << "==" << (int)*act << "?\n";
    return false;
}

//=============================================================================
//                        GENERATOR LANGUAGE FOR gg
//=============================================================================
//
// The gg function interprets a given 'spec' in order from left to right to
// build up a schedule according to a custom language.
//
//  Spec      := (ScheduleElement)*
//
//  ScheduleElement := (DefaultId | CategoryId) TimeInterval
//
//  DefaultId := 'X'
//
//  CategoryId := 'A'|'B'|'C'|'D'         %% ["A".."D"] category strings
//
//  TimeInterval :=  '1'|'2'|'3'|'4'|'5'  %% # of time units (where the unit
//                  |'6'|'7'|'8'|'9'      %% of time is determined by the test)
//
// 'gg' syntax usage examples:
//  Here are some examples, and sequence of actions that would be created:
//
// Spec String      Result Actions              Description
// -----------      -------------               -----------
// "A1"            [                            'scheduleCategory()' "A"
//                   SCHEDULE_CATEGORY "A" 1     at 1 time unit.
//                 ]
//
// "X4"            [                            'setDefaultSchedule()' with
//                   SCHEDULE_DEFAULT  4         4 time units.
//                 ]
//
// "A4X4B1"        [                            'scheduleCategory()' "A" at
//                   SCHEDULE_CATEGORY "A" 4     4 time units, then
//                   SCHEDULE_DEFAULT  4        'setDefaultSchedule()' at
//                   SCHEDULE_CATEGORY "B" 1     at 4 time units, then
//                 ]                             'scheduleCategory()' "B" at
//                                               1 time unit.

void gg(bsl::vector<Action>  *actions,
        baem_MetricRegistry&  registry,
        const char           *specification)
    // Set the specified 'actions' to the sequence of actions indicated by the
    // specified 'specification' (as described by the 'gg' generator language
    // above), using the specified 'registry' to supply 'baem_Category' object
    // addresses.
{
    const char *c = specification;
    while (*c) {
        if (0 == *(c + 1)) {
            bsl::cout << "Unexepected end of specification" << bsl::endl;
        }
        Action::Type         type;
        const baem_Category *category = 0;
        int                  interval = 0;

        char elementChar = *c;
        char intervalChar = *(c + 1);
        if (elementChar == 'X') {
            type = Action::SCHEDULE_DEFAULT;
        }
        else if (elementChar >= 'A' && elementChar <= 'D') {
            type = Action::SCHEDULE_CATEGORY;
            bsl::string catString(1, elementChar);
            category = registry.getCategory(catString.c_str());
        }
        else {
            bsl::cout << "Unexpected element type in spec: " << elementChar
                      << "(" << (int)elementChar << ")" << bsl::endl;
            BSLS_ASSERT(false);
        }

        if (intervalChar > '0' && intervalChar <= '9') {
            interval = intervalChar - '0';
        }
        else {
            bsl::cout << "Unexpected interval in spec: " << intervalChar
                      << "(" << (int)intervalChar << ")" << bsl::endl;
            BSLS_ASSERT(false);
        }

        actions->push_back(Action(type, category, interval));
        c += 2;
    }
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

// SimpleStreamPublisher was defined in 'baem_publisher.h'.

    // simplestreampublisher.h
    class SimpleStreamPublisher : public baem_Publisher {
        // A simple implementation of the 'baem_Publisher' protocol that
        // writes metric records to a stream.

        // PRIVATE DATA
        bsl::ostream&              d_stream;     // output stream (held)

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
        virtual void publish(const baem_MetricSample& metricValues);
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
    void SimpleStreamPublisher::publish(const baem_MetricSample& metricValues)
    {
        if (metricValues.numRecords() > 0) {
            d_stream << metricValues.timeStamp() << " "
                     << metricValues.numRecords() << " Records" << bsl::endl;

            baem_MetricSample::const_iterator sIt = metricValues.begin();
            baem_MetricSample::const_iterator prev;
            for (; sIt != metricValues.end(); ++sIt) {
                if (sIt == metricValues.begin() ||
                    sIt->elapsedTime() != prev->elapsedTime()) {
                    d_stream << "\tElapsed Time: "
                             << sIt->elapsedTime().totalSecondsAsDouble()
                             << "s" << bsl::endl;
                }
                baem_MetricSampleGroup::const_iterator gIt = sIt->begin();
                for (; gIt != sIt->end(); ++gIt) {
                    d_stream << "\t" << gIt->metricId()
                             << " [count = " << gIt->count()
                             << ", total = " << gIt->total()
                             << ", min = "   << gIt->min()
                             << ", max = "   << gIt->max() << "]" << bsl::endl;
                }
                prev = sIt;
            }
        }
    }

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

    const bdet_TimeInterval INVALID = invalidInterval();

    bael_DefaultObserver observer(&bsl::cout);
    bael_LoggerManagerConfiguration configuration;
    bael_LoggerManager& manager =
            bael_LoggerManager::initSingleton(&observer, configuration);

    bael_Severity::Level defaultPassthrough = bael_Severity::BAEL_OFF;
    if (verbose)
        defaultPassthrough = bael_Severity::BAEL_FATAL;
    if (veryVerbose)
        defaultPassthrough = bael_Severity::BAEL_ERROR;
    if (veryVeryVerbose)
        defaultPassthrough = bael_Severity::BAEL_TRACE;

    manager.setDefaultThresholdLevels(bael_Severity::BAEL_OFF,
                                      defaultPassthrough,
                                      bael_Severity::BAEL_OFF,
                                      bael_Severity::BAEL_OFF);

    bslma_TestAllocator testAlloc; bslma_TestAllocator *Z = &testAlloc;
    bslma_TestAllocator defaultAllocator;
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 13: {
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
// The following example demonstrates how to use 'baem_PublicationScheduler'.
// Before instantiating the publication scheduler, we create a
// 'bcep_TimerEventScheduler' as well as a 'baem_MetricsManager'.  We obtain
// collectors for three different metric categories, "A", "B", and "C", that
// we will use to generate metric values for publication.
//..
    bslma_Allocator          *allocator = bslma_Default::allocator(0);
    bcep_TimerEventScheduler  timer(allocator);
    baem_MetricsManager       manager(allocator);

    baem_Collector *A = manager.collectorRepository().getDefaultCollector(
                                                                     "A", "1");
    baem_Collector *B = manager.collectorRepository().getDefaultCollector(
                                                                     "B", "1");
    baem_Collector *C = manager.collectorRepository().getDefaultCollector(
                                                                     "C", "1");
//..
// We now create an instance of 'SimpleStreamPublisher', which implements the
// 'baem_Publisher' protocol)  Note that, 'SimpleStreamPublisher' is an
// example implementation of the 'baem_Publisher' protocol, defined in the
// 'baem_publisher' component, in practice clients typically use a standard
// publisher class (e.g., 'baem_StreamPublisher').
//..
        bcema_SharedPtr<baem_Publisher> publisher(
                            new (*allocator) SimpleStreamPublisher(bsl::cout),
                            allocator);
//..
// We now register the 'publisher' we have created with the metrics 'manager'
// to publish our categories.  The, we 'start' the timer-event scheduler we
// will supply to the 'baem_PublicationScheduler'.
//..
    manager.addGeneralPublisher(publisher);
    timer.start();
//..
// Now we construct a 'baem_PublicationScheduler' and pass it the respective
// addresses of both the metrics manager and the timer-event scheduler.  We
// schedule the publication of category "A" and "B" every .05 seconds, then we
// set the default publication to every .10 seconds.  Note that those time
// intervals were chosen to ensure fast and consistent output for this
// example.  In normal usage the interval between publications should be large
// enough to ensure that metric publication does not negatively affect the
// performance of the application (a 30 second interval is typical).
//..
    baem_PublicationScheduler scheduler(&manager, &timer, allocator);
    scheduler.scheduleCategory("A", bdet_TimeInterval(.05));
    scheduler.scheduleCategory("B", bdet_TimeInterval(.05));
    scheduler.setDefaultSchedule(bdet_TimeInterval(.10));
//..
// We can use the accessor operations to verify the schedule that we have
// specified.
//..
    bdet_TimeInterval intervalA, intervalB, intervalC, defaultInterval;
        ASSERT( scheduler.findCategorySchedule(&intervalA, "A"));
        ASSERT( scheduler.findCategorySchedule(&intervalB, "B"));
        ASSERT(!scheduler.findCategorySchedule(&intervalC, "C"));
        ASSERT( scheduler.getDefaultSchedule(&defaultInterval));
        ASSERT(bdet_TimeInterval(.05) == intervalA);
        ASSERT(bdet_TimeInterval(.05) == intervalB);
        ASSERT(bdet_TimeInterval(.10) == defaultInterval);
//..
// Finally we add a couple metrics and wait just over .1 seconds.
//..
    A->update(1.0);
    B->update(2.0);
    C->update(3.0);
    bcemt_ThreadUtil::sleep(bdet_TimeInterval(.11));
//..
// The output of the publication should look similar to:
//..
// 19NOV2008_18:34:26.766+0000    2 Records   0.0517s Elapsed Time
//         A.1 [count = 1, total = 1, min = 1, max = 1]
//         B.1 [count = 1, total = 2, min = 2, max = 2]
// 19NOV2008_18:34:26.816+0000    2 Records   0.050183s Elapsed Time
//         A.1 [count = 0, total = 0, min = inf, max = -inf]
//         B.1 [count = 0, total = 0, min = inf, max = -inf]
// 19NOV2008_18:34:26.817+0000    1 Records   0.102473s Elapsed Time
//         C.1 [count = 1, total = 3, min = 3, max = 3]
//..
// Note that categories 'A' and 'B' are emitted as a single publication:
// the scheduler combines categories published at the same frequency into a
// single publication event to minimize the number of times
// 'baem_MetricsManager::publish' is invoked.  Also note that category 'C' is
// published as part of the scheduled default publication.

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST
        //
        // Testing:
        //     Thread-safety of manipulator methods
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST CONCURRENCY" << endl
                                  << "================" << endl;

        bcema_TestAllocator defaultAllocator;
        bslma_DefaultAllocatorGuard guard(&defaultAllocator);

        bcema_TestAllocator testAllocator;

        bcep_TimerEventScheduler  timer(&testAllocator);
        baem_MetricsManager       manager(&testAllocator);
        Obj mX(&manager, &timer, &testAllocator); const Obj& MX = mX;
        {
            ConcurrencyTest tester(6, &mX, &timer, &defaultAllocator);
            tester.runTest();
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // BAEM ALLOCATION EXCEPTION TEST
        //
        // Concerns:
        //   That 'scheduleCategory' and 'setDefaultSchedule' are exception
        //   safe.
        //
        // Plan:
        //   Use the 'BAEM_BEGIN_RETRY_TEST' to verify the manipulator methods
        //   of this object are exception neutral.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BDEMA EXCEPTION TEST" << endl
                          << "====================" << endl;

        const char *TEST_SPECS[] = {
            "A1",
            "A1B1",
            "A1B2",
            "A1B1C1",
            "A1B2C2",
            "A1B2C3",
            "A1B1X1",
            "A1B1X2",
            "A1B1C1D1X1",
            "A1B2C3D4X5",
            "A1B2C2D4X4",
            "A2B2C3D2X3",
            "X9A1B1C2D3"
            "X9A1B1C2D9"
            // With rescheduling
            "A5B6C7D8D1C1B1A1",
            "A5B6C7D8A1B1C1D1",
            "A5B6C7D8D1C1B1A1",
            "A1X2X1",
            "A1X2X3",
            "A1X2A2",
            "X1A1X2",
            "X4X6D6D4",
            "A1X2B2X1A2",
            "A2X4B5X5X6D6A6",
            "X1A1B1C1D1X1B2C3D4X3A2D4X4X7A5B1C1X1"
        };

        const int NUM_SPECS = sizeof TEST_SPECS / sizeof *TEST_SPECS;
        bslma_TestAllocator  testAllocator;
        baem_MetricsManager  manager(Z);
        baem_MetricRegistry& registry = manager.metricRegistry();
        bcep_TimerEventScheduler timer(Z);
        Obj mX(&manager, &timer, &testAllocator); const Obj& MX = mX;
        for (int i = 0; i < NUM_SPECS; ++i) {
            BEGIN_BAEM_EXCEPTION_TEST {
            bsl::vector<Action> actions(Z);
            gg(&actions, registry, TEST_SPECS[i]);

            if (veryVeryVerbose) {
                P_(i); P(TEST_SPECS[i]);
                P(actions);
            }

            //   1. Use the default constructor to create an object then
            //   use the basic manipulators configure the scheduler
            //   according to the configuration description.
            //   Simultaneously set the value of an "oracle"
            //   'CategoryScheduleOracle' (a map from category to scheduled
            //   frequency).
            bdet_TimeInterval      defaultInterval(INVALID);
            CategoryScheduleOracle intervalMap(Z);

            // Use 'scheduleCategory' and 'setDefaultSchedule' to set the
            // schedule for the publication scheduler.
            bsl::vector<Action>::const_iterator spIt = actions.begin();
            for (; spIt != actions.end(); ++spIt) {
                bdet_TimeInterval interval(spIt->interval(), 0);
                if (spIt->type() == Action::SCHEDULE_CATEGORY) {
                    intervalMap[spIt->category()] = interval;
                    mX.scheduleCategory(spIt->category(), interval);
                }
                else {
                    defaultInterval = interval;
                    mX.setDefaultSchedule(defaultInterval);
                }
            }

            bdet_TimeInterval interval;
            ASSERT((defaultInterval != INVALID) ==
                   MX.getDefaultSchedule(&interval));
            ASSERT((defaultInterval == INVALID) ||
                   (defaultInterval == interval));

            if (veryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }
            Schedule schedule(Z);
            ASSERT(intervalMap.size() == MX.getCategorySchedule(&schedule));
            ASSERT(equalSchedule(intervalMap, schedule));

            int uniqueIntervals = countUniqueIntervals(intervalMap,
                                                       defaultInterval);
            ASSERT(uniqueIntervals == timer.numClocks());
            ASSERT(0               == timer.numEvents());

            mX.cancelAll();
            } END_BAEM_EXCEPTION_TEST
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING AUXILIARY METHODS: scheduleCategory, cancelCategorySchedule,
        //                            findCategorySchedule
        //
        // Concerns:
        //   That the alternative variants of 'scheduleCategory',
        //   'cancelCategorySchedule', and 'findCategorySchedule' (taking a
        //   string for the category) and logically equivalent to the primary
        //   variant taking a 'baem_Category' address).
        //
        // Plan:
        //   Invoked 'scheduleCategory', 'cancelCategorySchedule', and
        //   'findCategory' and use the previously test 'findCategory' variant
        //   to verify the results.
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "TESTING ALTERNATIVE METHODS: " << endl
            << "scheduleCategory(const bdeut_StringReg&, ...)', \n"
            << "'findCategorySchedule(..., const bdeut_StringRef&)', and\n"
            << "'cancelCategorySchedule(constbdeut_StringRef&)\n"
            << "========================================================\n";

        const char *CATEGORIES[] = { "A", "B", "C", "dummy1", "testcategory" };
        const int NUM_CATEGORIES = sizeof CATEGORIES / sizeof *CATEGORIES;

        baem_MetricsManager manager(Z);
        baem_MetricRegistry&  reg = manager.metricRegistry();
        bcep_TimerEventScheduler timer(Z);
        Obj mX(&manager, &timer, Z); const Obj& MX = mX;
        for (int i = 0; i < 10; ++i) {
            bdet_TimeInterval interval(i+1, 0);
            bdet_TimeInterval foundInterval;
            for (int j = 0; j < NUM_CATEGORIES; ++j) {
                const Category *CATEGORY = reg.getCategory(CATEGORIES[j]);

                ASSERT(!MX.findCategorySchedule(&foundInterval,CATEGORY));
                ASSERT(!MX.findCategorySchedule(&foundInterval,CATEGORIES[j]));
                mX.scheduleCategory(CATEGORIES[j], interval);

                ASSERT(MX.findCategorySchedule(&foundInterval,CATEGORY));
                ASSERT(interval == foundInterval);

                foundInterval = bdet_TimeInterval(0,0);
                ASSERT(MX.findCategorySchedule(&foundInterval,CATEGORIES[j]));
                ASSERT(interval == foundInterval);

                mX.cancelCategorySchedule(CATEGORIES[j]);
                ASSERT(!MX.findCategorySchedule(&foundInterval,CATEGORY));
                ASSERT(!MX.findCategorySchedule(&foundInterval,CATEGORIES[j]));
                ASSERT(0 == defaultAllocator.numBytesInUse());
            }
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ACCESSOR: print
        //
        // Concerns:
        //   That print outputs a reasonable output for this class
        //
        // Plan:
        //   Generate a schedule and print the output to a stream.  Verify the
        //   expected value.
        // Testing:
        //
        //   bsl::ostream& print(bsl::ostream& , int, int) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING MANIPULATOR: print" << endl
                          << "==================================" << endl;

        bcep_TimerEventScheduler timer(Z);
        baem_MetricsManager      manager(Z);
        baem_MetricRegistry&     reg = manager.metricRegistry();
        Obj mX(&manager, &timer, Z); const Obj& MX = mX;
        mX.scheduleCategory(reg.getCategory("A"), bdet_TimeInterval(1, 0));
        mX.scheduleCategory(reg.getCategory("B"), bdet_TimeInterval(2, 0));
        mX.scheduleCategory(reg.getCategory("C"), bdet_TimeInterval(3, 0));
        mX.scheduleCategory(reg.getCategory("D"), bdet_TimeInterval(4, 0));
        mX.scheduleCategory(reg.getCategory("E"), bdet_TimeInterval(5, 0));
        mX.setDefaultSchedule(bdet_TimeInterval(6, 0));

        const char *EXP_1 =
            "[  default interval: (6, 0) "
            " scheduled categories: [   "
            " [ A -> (1, 0) ]   [ B -> (2, 0) ]   [ C -> (3, 0) ]  "
            " [ D -> (4, 0) ]   [ E -> (5, 0) ]  ] "
            " clock information: [   "
            " [    interval: (1, 0)   default: 0   handle: 8388608  "
            "  categories: [ A ]     excluded categories: [ ]    ]  "
            " [    interval: (2, 0)   default: 0   handle: 8388609  "
            "  categories: [ B ]     excluded categories: [ ]    ]  "
            " [    interval: (3, 0)   default: 0   handle: 8388610  "
            "  categories: [ C ]     excluded categories: [ ]    ]  "
            " [    interval: (4, 0)   default: 0   handle: 8388611  "
            "  categories: [ D ]     excluded categories: [ ]    ]  "
            " [    interval: (5, 0)   default: 0   handle: 8388612  "
            "  categories: [ E ]     excluded categories: [ ]    ]  "
            " [    interval: (6, 0)   default: 1   handle: 8388613  "
            "  categories: [ ]     excluded categories: [ A B C D E ]    ]  "
            "] ] ";

        const char *EXP_2 =
            "   [\n"
            "      default interval: (6, 0)\n"
            "      scheduled categories: [ \n"
            "         [ A -> (1, 0) ]\n"
            "         [ B -> (2, 0) ]\n"
            "         [ C -> (3, 0) ]\n"
            "         [ D -> (4, 0) ]\n"
            "         [ E -> (5, 0) ]\n"
            "      ]\n"
            "      clock information: [ \n"
            "         [\n"
            "            interval: (1, 0)   default: 0   handle: 8388608\n"
            "            categories: [ A ] \n"
            "            excluded categories: [ ] \n"
            "         ]\n"
            "         [\n"
            "            interval: (2, 0)   default: 0   handle: 8388609\n"
            "            categories: [ B ] \n"
            "            excluded categories: [ ] \n"
            "         ]\n"
            "         [\n"
            "            interval: (3, 0)   default: 0   handle: 8388610\n"
            "            categories: [ C ] \n"
            "            excluded categories: [ ] \n"
            "         ]\n"
            "         [\n"
            "            interval: (4, 0)   default: 0   handle: 8388611\n"
            "            categories: [ D ] \n"
            "            excluded categories: [ ] \n"
            "         ]\n"
            "         [\n"
            "            interval: (5, 0)   default: 0   handle: 8388612\n"
            "            categories: [ E ] \n"
            "            excluded categories: [ ] \n"
            "         ]\n"
            "         [\n"
            "            interval: (6, 0)   default: 1   handle: 8388613\n"
            "            categories: [ ] \n"
            "            excluded categories: [ A B C D E ] \n"
            "         ]\n"
            "      ]\n"
            "   ]\n";

        bsl::ostringstream buf1, buf2, buf3;
        mX.print(buf1, 0, -1);
        mX.print(buf2, 1, 3);

        bsl::string val1 = buf1.str();
        bsl::string val2 = buf2.str();

        if (veryVerbose) {
            P(bsl::strlen(val1.c_str()));
            P(bsl::strlen(EXP_1));
            P( val1);
            P(EXP_1);
            P( val2);
            P(EXP_2);
        }
        ASSERT(bsl::string(EXP_1) == val1);
        ASSERT(bsl::string(EXP_2) == val2);
        if (veryVerbose) {
            stringDiff(EXP_1, val1);
            stringDiff(EXP_2, val2);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING ACCESSOR: getCategorySchedule
        //
        // Concerns:
        //   That the 'getCategorySchedule' method returns the correct schedule
        //   information.
        //
        // Plan:
        //   For a sequence of independent test configuration values (defined
        //   using the 'gg' generator language), use the default constructor
        //   to create an object use the basic manipulators to its value.
        //   Invoke cancelAll() and verify the test object is returned to the
        //   default state.  Next, for a sequence of independent schedule
        //   configurations described using the generator language 'gg':
        //   1. Use the default constructor to create an object then use the
        //      basic manipulators configure the scheduler according to the
        //      configuration description.  Simultaneously set the value of an
        //      "oracle" 'CategoryScheduleOracle' (a map from category to
        //      scheduled frequency),
        //   2. Compare the results of the 'getCategorySchedule' method to the
        //      "oracle" 'CategoryScheduleOracle'
        //
        // Testing:
        //  int getCategorySchedule(
        //                bsl::vector<bsl::pair<const baem_Category *,
        //                                      bdet_TimeInterval> > * ) const;
        // --------------------------------------------------------------------

        const char *TEST_SPECS[] = {
            "A1",
            "A1B1",
            "A1B2",
            "A1B1C1",
            "A1B2C2",
            "A1B2C3",
            "A1B1X1",
            "A1B1X2",
            "A1B1C1D1X1",
            "A1B2C3D4X5",
            "A1B2C2D4X4",
            "A2B2C3D2X3",
            "X9A1B1C2D3"
            "X9A1B1C2D9"
            // With rescheduling
            "A5B6C7D8D1C1B1A1",
            "A5B6C7D8A1B1C1D1",
            "A5B6C7D8D1C1B1A1",
            "A1X2X1",
            "A1X2X3",
            "A1X2A2",
            "X1A1X2",
            "X4X6D6D4",
            "A1X2B2X1A2",
            "A2X4B5X5X6D6A6",
            "X1A1B1C1D1X1B2C3D4X3A2D4X4X7A5B1C1X1"
        };

        const int NUM_SPECS = sizeof TEST_SPECS / sizeof *TEST_SPECS;
        baem_MetricsManager  manager(Z);
        baem_MetricRegistry& registry = manager.metricRegistry();
        bcep_TimerEventScheduler timer(Z);
        for (int i = 0; i < NUM_SPECS; ++i) {

            Obj mX(&manager, &timer, Z); const Obj& MX = mX;
            bsl::vector<Action> actions(Z);
            gg(&actions, registry, TEST_SPECS[i]);

            if (veryVeryVerbose) {
                P_(i); P(TEST_SPECS[i]);
                P(actions);
            }

            // 1. Use the default constructor to create an object then
            //    use the basic manipulators configure the scheduler according
            //    to the configuration description.  Simultaneously set the
            //    value of an "oracle" 'CategoryScheduleOracle' (a map from
            //    category to scheduled frequency).
            bdet_TimeInterval      defaultInterval(INVALID);
            CategoryScheduleOracle intervalMap(Z);

            // Use 'scheduleCategory' and 'setDefaultSchedule' to set the
            // schedule for the publication scheduler.
            bsl::vector<Action>::const_iterator spIt = actions.begin();
            for (; spIt != actions.end(); ++spIt) {
                bdet_TimeInterval interval(spIt->interval(), 0);
                if (spIt->type() == Action::SCHEDULE_CATEGORY) {
                    intervalMap[spIt->category()] = interval;
                    mX.scheduleCategory(spIt->category(), interval);
                }
                else {
                    defaultInterval = interval;
                    mX.setDefaultSchedule(defaultInterval);
                }
            }

            if (veryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }

            //   2. Compare the results of the 'getCategorySchedule' method to
            //      the "oracle" 'CategoryScheduleOracle'

            Schedule schedule(Z);
            ASSERT(intervalMap.size() == MX.getCategorySchedule(&schedule));
            ASSERT(equalSchedule(intervalMap, schedule));
        }
      };
      case 7: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATOR: cancelAll
        //
        // Concerns:
        //   That the cancelAll manipulator correctly removes all the elements
        //   from the schedule.
        // Plan:
        //   For a sequence of independent test configuration values (defined
        //   using the 'gg' generator language), use the default constructor
        //   to create an object use the basic manipulators to its value.
        //   Invoke cancelAll() and verify the test object is returned to the
        //   default state.
        //
        // Testing:
        //
        //   void cancelAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING MANIPULATOR: cancelAll\n"
                          << "==========================================\n";

        bcep_TimerEventScheduler  timer(Z);
        baem_MetricsManager       manager(Z);
        baem_MetricRegistry&      registry = manager.metricRegistry();

        const char *CATEGORIES[] = {"A", "B", "C", "D", "dummy1", "dummy2"};
        const int NUM_CATEGORIES = sizeof CATEGORIES / sizeof *CATEGORIES;

        bsl::vector<const Category *> allCategories(Z);
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            registry.getCategory(CATEGORIES[i]);
        }
        registry.getAllCategories(&allCategories);
        ASSERT(NUM_CATEGORIES == allCategories.size());

        const char *TEST_SPECS[] = {
            "A1",
            "A1B1",
            "A1B2",
            "A1B1C1",
            "A1B2C2",
            "A1B2C3",
            "A1B1X1",
            "A1B1X2",
            "A1B1C1D1X1",
            "A1B2C3D4X5",
            "A1B2C2D4X4",
            "A2B2C3D2X3",
            "X9A1B1C2D3"
            "X9A1B1C2D9"
            // With rescheduling
            "A5B6C7D8D1C1B1A1",
            "A5B6C7D8A1B1C1D1",
            "A5B6C7D8D1C1B1A1",
            "A1X2X1",
            "A1X2X3",
            "A1X2A2",
            "X1A1X2",
            "X4X6D6D4",
            "A1X2B2X1A2",
            "A2X4B5X5X6D6A6",
            "X1A1B1C1D1X1B2C3D4X3A2D4X4X7A5B1C1X1"
        };
        const int NUM_SPECS = sizeof TEST_SPECS / sizeof *TEST_SPECS;

        for (int i = 0; i < NUM_SPECS; ++i) {
            Obj mX(&manager, &timer, Z); const Obj& MX = mX;
            bsl::vector<Action> actions(Z);
            gg(&actions, registry, TEST_SPECS[i]);

            if (veryVeryVerbose) {
                P_(i); P(TEST_SPECS[i]);
            }

            ASSERT(0 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            // Configure the scheduler according to the 'actions' list.
            bsl::vector<Action>::const_iterator aIt = actions.begin();
            for (; aIt != actions.end(); ++aIt) {
                bdet_TimeInterval interval(aIt->interval(), 0);
                if (aIt->type() == Action::SCHEDULE_CATEGORY) {
                    mX.scheduleCategory(aIt->category(), interval);
                }
                else {
                    mX.setDefaultSchedule(interval);
                }
            }
            ASSERT(0 <  timer.numClocks());
            ASSERT(0 == timer.numEvents());

            mX.cancelAll();

            bdet_TimeInterval interval;
            ASSERT(!MX.getDefaultSchedule(&interval));
            for (int j = 0; j < NUM_CATEGORIES; ++j) {
                ASSERT(!MX.findCategorySchedule(&interval,
                                                allCategories[j]));
            }
            ASSERT(0 == timer.numClocks());
            ASSERT(0 == timer.numEvents());
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS: cancelCategoryScheduleSchedule,
        //                       clearDefaultSchedule
        //
        // Concerns:
        //   That the cancelPublication manipulator correctly removes an
        //   element from the schedule.
        //
        // Plan:
        //   For a sequence of independent test configuration values defined
        //   using the 'gg' generator language:
        //      1. Create an object to test and set its to the test
        //         configuration, also create and configure an "oracle"
        //         'CategorySchedule' (a map from category to frequency).
        //      2. Perform a double iteration over each category added to the
        //         schedule, removing each element of the schedule one at a
        //         time, starting at a different element each time.  Then
        //         verify the schedule returned by the test objects accessors
        //         equals the "oracle" schedule.  Verify the number of clocks
        //         in the underlying timer is equal to the number of unique
        //         time intervals.
        //
        // Testing:
        //   int cancelCategorySchedule(const baem_Category *);
        //   int clearDefaultSchedule();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING MANIPULATOR: cancelCategorySchedule" << endl
                 << "===========================================" << endl;

        bcep_TimerEventScheduler  timer(Z);
        baem_MetricsManager       manager(Z);
        baem_MetricRegistry&      registry = manager.metricRegistry();

        const char *CATEGORIES[] = {"A", "B", "C", "D", "dummy1", "dummy2"};
        const int NUM_CATEGORIES = sizeof CATEGORIES / sizeof *CATEGORIES;

        bsl::vector<const Category *> allCategories(Z);
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            registry.getCategory(CATEGORIES[i]);
        }
        registry.getAllCategories(&allCategories);
        ASSERT(NUM_CATEGORIES == allCategories.size());

        const char *TEST_SPECS[] = {
            "A1",
            "A1B1",
            "A1B2",
            "A1B1C1",
            "A1B2C2",
            "A1B2C3",
            "A1B1X1",
            "A1B1X2",
            "A1B1C1D1X1",
            "A1B2C3D4X5",
            "A1B2C2D4X4",
            "A2B2C3D2X3",
            "X9A1B1C2D3"
            "X9A1B1C2D9"
            // With rescheduling
            "A5B6C7D8D1C1B1A1",
            "A5B6C7D8A1B1C1D1",
            "A5B6C7D8D1C1B1A1",
            "A1X2X1",
            "A1X2X3",
            "A1X2A2",
            "X1A1X2",
            "X4X6D6D4",
            "A1X2B2X1A2",
            "A2X4B5X5X6D6A6",
            "X1A1B1C1D1X1B2C3D4X3A2D4X4X7A5B1C1X1"
        };
        const int NUM_SPECS = sizeof TEST_SPECS / sizeof *TEST_SPECS;

        for (int i = 0; i < NUM_SPECS; ++i) {
            bsl::vector<Action> actions(Z);
            gg(&actions, registry, TEST_SPECS[i]);

            if (veryVeryVerbose) {
                P_(i); P(TEST_SPECS[i]);
            }

            bsl::vector<const Category *> elementsToRemove(allCategories, Z);
            elementsToRemove.push_back(0);

            // index j indicates the element in the list of elements to remove
            // to begin removing at.
            for (int j = 0; j < actions.size(); ++j) {
                ASSERT(0 == timer.numClocks());
                ASSERT(0 == timer.numEvents());

                Obj mX(&manager, &timer, Z); const Obj& MX = mX;
                bdet_TimeInterval      expDefault(INVALID);
                CategoryScheduleOracle expSchedule(Z);

                // Configure the scheduler according to the 'actions' list.
                bsl::vector<Action>::const_iterator aIt = actions.begin();
                for (; aIt != actions.end(); ++aIt) {
                    if (aIt->type() == Action::SCHEDULE_CATEGORY) {
                        bdet_TimeInterval intvl(aIt->interval(), 0);
                        expSchedule[aIt->category()] = intvl;
                        mX.scheduleCategory(aIt->category(), intvl);
                    }
                    else {
                        expDefault = bdet_TimeInterval(aIt->interval(), 0);
                        mX.setDefaultSchedule(expDefault);
                    }
                }

                for (int k = 0; k < elementsToRemove.size(); ++k) {
                    const int idx = (k + j) % elementsToRemove.size();
                    const Category *CATEGORY = elementsToRemove[idx];

                    // Remove the category (or default schedule) at this
                    // index.
                    if (CATEGORY) {
                        if (expSchedule.find(CATEGORY) != expSchedule.end()) {
                            ASSERT(0 == mX.cancelCategorySchedule(CATEGORY));
                            expSchedule.erase(expSchedule.find(CATEGORY));
                        }
                        ASSERT(0 != mX.cancelCategorySchedule(CATEGORY));
                    }
                    else {
                        if (expDefault != INVALID) {
                            ASSERT(0 == mX.clearDefaultSchedule());
                            expDefault = INVALID;
                        }
                        ASSERT(0 != mX.clearDefaultSchedule());
                    }

                    // Verify the scheduled elements
                    bdet_TimeInterval interval;
                    ASSERT((expDefault != INVALID) ==
                           MX.getDefaultSchedule(&interval));
                    ASSERT(expDefault == INVALID || expDefault == interval);
                    for (int l = 0; l < NUM_CATEGORIES; ++l) {
                        const Category *CAT = allCategories[l];
                        bool found = expSchedule.find(CAT)!=expSchedule.end();
                        ASSERT(found ==
                               MX.findCategorySchedule(&interval, CAT));
                        ASSERT((!found) || interval == expSchedule[CAT]);
                    }

                    // Verify the correct number of clocks in the underlying
                    // event scheduler.
                    int uniqueIntervals = countUniqueIntervals(expSchedule,
                                                               expDefault);
                    ASSERT(uniqueIntervals == timer.numClocks());
                    ASSERT(0               == timer.numEvents());
                }
                ASSERT(0 == timer.numClocks());
                ASSERT(0 == timer.numEvents());
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING PUBLICATION FREQUENCY
        //
        // Concerns:
        //   That Clocks are correctly registered with the underlying
        //   'bcep_TimerEventScheduler'.
        //
        // Plan:
        //   The clocks in a 'bcep_TimerEventSchedule' cannot be inspected
        //   directly.  Instead, we must wait a period of time and verify
        //   that the correct number of events were supplied by the
        //   'bcep_TimerEventScheduler'.  A 'TIME_UNIT' was selected as
        //   the basis for the scheduled periods, this value was chosen
        //   experimentally, as the lowest value that was high enough to
        //   produce consistent results.  Note that, since each test case
        //   takes on the order of 100 milliseconds to run, only a limited
        //   number of test cases are used.
        //
        //   Define a set of configurations where the number of expected
        //   publications in a particular time interval can be easily
        //   determined.  For each member of a that set of possible
        //   publication configurations (defined using the 'gg' generator
        //   language):
        //    1. Create a metrics manager and add a 'TestPublisher' to it.
        //    2. Configure a publication scheduler according to the
        //       configuration and create a 'CategorySchedule' "oracle"
        //       (mapping category to frequency) describing the configuration.
        //    3. Convert the 'CategorySchedule' "oracle" to a 'Schedule'
        //       "oracle", mapping a frequency to the set of categories at
        //       that frequency.
        //    4. Sleep for a multiple of 'TIME_UNIT'.
        //    5. Verify that the 'publish' method was invoked correctly by
        //       comparing the number of times that a set of categories should
        //       have been published (determined using the "oracle" 'Schedule')
        //       and then compare that to the actual number of invocations
        //       reported by the 'TestPublisher'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: invocation of 'publisher'" << endl
                          << "==================================" << endl;

        baem_MetricsManager  manager(Z);
        baem_MetricRegistry& reg = manager.metricRegistry();
        const char *CATEGORIES[] = {"A", "B", "C", "D", "dummy1", "dummy2"};
        const int NUM_CATEGORIES = sizeof CATEGORIES / sizeof *CATEGORIES;

        // Initialize a list of all 'baem_Category' addresses, and a collector
        // for each category (ensuring metrics to publish).
        bsl::set<const baem_Category *> allCategories(Z);
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            manager.collectorRepository().getDefaultCollector(CATEGORIES[i],
                                                              "metric");
            allCategories.insert(reg.getCategory(CATEGORIES[i]));
        }

        TestPublisher tp(Z);
        PubPtr pub_p(&tp, bcema_SharedPtrNilDeleter(), Z);
        manager.addGeneralPublisher(pub_p);

        // Use the time intervals '1' and '2' to make the number of
        // expected invocations easy to compute.  Minimize test cases to keep
        // the total running time low.
        const char *TEST_SPECS[] = {
            "A1",
            "A1B2",
            "A1X1",
            "A1X2A2X1",
            "A1A2X2X1",
            "A1B2A2B2",
            "A1B2B1A2"
        };
        const int NUM_SPECS = sizeof TEST_SPECS / sizeof *TEST_SPECS;

        // This time unit was chosen experimentally, as the lowest value that
        // was high enough to produce consistent results under heavy load.
        const int TIME_UNIT = 100 * NANOSECS_PER_MILLISEC; // 100 milliseconds

        for (int i = 0; i < NUM_SPECS; ++i) {
            BAEM_BEGIN_RETRY_TEST(3) {
            bsl::vector<Action> actions(Z);
            gg(&actions, manager.metricRegistry(), TEST_SPECS[i]);

            //    2. Configure a publication scheduler according to the
            //       configuration and create a 'CategoryScheduleOracle'
            //       "oracle" (mapping category to frequency) describing the
            //       configuration.
            bcep_TimerEventScheduler timer(Z);
            Obj mX(&manager, &timer, Z); const Obj& MX = mX;

            // Initialize the 'baem_PublicationScheduler' under test and the
            // 'CategorySchedule' object 'catSchedule'.
            bdet_TimeInterval       defaultInterval(INVALID);
            CategoryScheduleOracle  catSchedule(Z);
            {
                bsl::vector<Action>::const_iterator spIt = actions.begin();
                for (; spIt != actions.end(); ++spIt) {
                    bdet_TimeInterval interval(0, spIt->interval()*TIME_UNIT);
                    if (spIt->type() == Action::SCHEDULE_CATEGORY) {
                        catSchedule[spIt->category()] = interval;
                        mX.scheduleCategory(spIt->category(), interval);
                    }
                    else {
                        defaultInterval = interval;
                        mX.setDefaultSchedule(defaultInterval);
                    }
                }
            }

            //    3. Convert the 'CategoryScheduleOracle' "oracle" to a
            //       'Schedule' "oracle", mapping a frequency to the set of
            //       categories at that frequency.
            ScheduleOracle schedule(Z);
            {
                bsl::set<const Category *> nonDefaultCategories(Z);
                CategoryScheduleOracle::const_iterator csIt =
                                                         catSchedule.begin();
                for (; csIt != catSchedule.end(); ++csIt) {
                    nonDefaultCategories.insert(csIt->first);
                    schedule[csIt->second].insert(csIt->first);
                }
                if (defaultInterval != INVALID) {
                    bsl::insert_iterator<bsl::set<const Category *> >
                                        iIt(schedule[defaultInterval],
                                            schedule[defaultInterval].begin());
                    bsl::set_difference(allCategories.begin(),
                                        allCategories.end(),
                                        nonDefaultCategories.begin(),
                                        nonDefaultCategories.end(),
                                        iIt);
                }
            }

            microSleep(MICROSECS_PER_MILLISEC * 10, 0);
            manager.publishAll(); // reset all previous publication times.
            tp.reset();
            if (veryVeryVerbose) {
                P_(i); P(TEST_SPECS[i]);
                tp.print(bsl::cout, 1, 3);
            }

            //    4. Sleep for a multiple of 'TIME_UNIT'.
            timer.start();

            // Wait 8 1/3 TIME_UNITs
            const double TIME_UNIT_MICRO = TIME_UNIT / NANOSECS_PER_MICROSEC;
            const int WAIT_MICRO = (TIME_UNIT_MICRO*8) + (TIME_UNIT_MICRO/3);
            microSleep(WAIT_MICRO, 0);
            timer.stop();

            if (veryVeryVerbose) {
                tp.print(bsl::cout, 1, 3);
            }

            //    5. Verify that the 'publish' method was invoked correctly by
            //       comparing the number of times that a set of categories
            //       should have been published (determined using the "oracle"
            //       'Schedule') and then compare that to the actual number of
            //       invocations reported by the 'TestPublisher'.
            ASSERT(tp.uniqueInvocations() == schedule.size());
            ScheduleOracle::const_iterator it = schedule.begin();
            for (; it != schedule.end(); ++it) {
                int numTimeUnits = it->first.nanoseconds() / TIME_UNIT;
                int expInvocations = 8 / numTimeUnits;
                const TestPublisher::InvocationSummary *invocation =
                                                tp.findInvocation(it->second);

                ASSERT(0              != expInvocations);

                if (expInvocations!= invocation->d_numInvocations) {
                    // Extra logging on failure.
                    P_(i); P(WAIT_MICRO);
                    tp.print(bsl::cout, 1, 3);
                    mX.print(bsl::cout, 1, 3);

                }
                LOOP2_ASSERT(expInvocations,
                             invocation->d_numInvocations,
                             expInvocations == invocation->d_numInvocations);
            }
        } BAEM_END_RETRY_TEST
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PRIVATE METHOD TEST: 'publish'
        //
        // Concerns:
        //   That the private method 'publish' is invoked by the timer event
        //   scheduler and, when invoked, will provide the correct set of
        //   categories to the 'baem_MetricsManager' object's 'publish' or
        //   'publishAll' methods.
        //
        // Plan:
        //   For each member of a set of possible publication configurations,
        //   defined using the 'gg' generator language:
        //    1. Create a metrics manager and add a 'TestPublisher' to it.
        //    2. Configure a publication scheduler according the configuration
        //        and create a 'CategorySchedule' "oracle" (mapping category to
        //        frequency) describing the configuration.
        //    3. Convert the 'CategorySchedule' "oracle" to a 'Schedule'
        //       "oracle", mapping a frequency to the set of categories
        //       published at that frequency.
        //    4. Sleep until each scheduled category has been published
        //    5. Verify that the 'publish' method was invoked correctly by
        //       comparing the 'Schedule' "oracle" to the 'TestPublisher'
        //       objects reported invocations.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: invocation of 'publisher'" << endl
                          << "==================================" << endl;

        BAEM_BEGIN_RETRY_TEST(3) {
        // 1. Create a metrics manager and add a 'TestPublisher' to it.
        baem_MetricsManager   manager(Z);
        baem_MetricRegistry& reg = manager.metricRegistry();
        TestPublisher tp(Z);
        PubPtr pub_p(&tp, bcema_SharedPtrNilDeleter(), Z);
        manager.addGeneralPublisher(pub_p);

        const char *CATEGORIES[] = {"A", "B", "C", "D", "dummy1", "dummy2"};
        const int NUM_CATEGORIES = sizeof CATEGORIES / sizeof *CATEGORIES;

        // Initialize a list of all 'baem_Category' addresses, and a collector
        // for each category (ensuring metrics to publish).
        bsl::set<const baem_Category *> allCategories(Z);
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            manager.collectorRepository().getDefaultCollector(CATEGORIES[i],
                                                              "metric");
            allCategories.insert(reg.getCategory(CATEGORIES[i]));
        }

        const char *TEST_SPECS[] = {
            "X1", "A2", "B2", "C3", "D4", "X5", "A6",  // trivial
            "A1B2", "A2B3", "A4B5", "A1D4", "C3A2",    // 2 categories
            "A1B1", "A2B2", "B3C3", "B4D4", "C3D4",    // duplicate intervals
            "X1A1", "A1X1", "B3X2", "B4X9", "D1X4",    // default
            "A1B1X1", "X1A1B1", "A1X1B1", "C3X1D8",    // 2 cat + default
            "A1A1", "A1A2", "A1B1B2", "A1B4A1",        // rescheduling
            "C1C2A1", "C1C2A3", "D1D2D3D4",            // categories
            "A1D1A2D2", "A1B1C2A2B2", "D2C1B1C3"
            "A1B1C1D1A2B2C3D3A4B4C4D5"
            "A5B6C7D8A1B1C1D1",
            "A5B6C7D8D1C1B1A1",
            "X1X1", "X8X8", "X1X9", "X9X3X3",          // reschedule w/ default
            "A1X2X1", "A1X2X3", "A1X2A2", "X1A1X2",
            "X4X6D6D4", "A1X2B2X1A2", "A2X4B5X5X6D6A6",
            "X1A1B1C1D1X1B2C3D4X3A2D4X4X7A5B1C1X1",
            "A1B2C3D4X5",
        };
        const int NUM_SPECS = sizeof TEST_SPECS / sizeof *TEST_SPECS;
        const int TIME_UNIT = 10 * NANOSECS_PER_MICROSEC; // 10 microseconds

        //    2. Configure a publication scheduler according the configuration
        //        and create a 'CategoryScheduleOracle' "oracle" (mapping
        //        category to frequency) describing the configuration.
        for (int i = 0; i < NUM_SPECS; ++i) {
            bsl::vector<Action> actions(Z);
            gg(&actions, manager.metricRegistry(), TEST_SPECS[i]);

            if (veryVeryVerbose) {
                P_(i); P(TEST_SPECS[i]);
            }
            bcep_TimerEventScheduler timer(Z);
            Obj mX(&manager, &timer, Z); const Obj& MX = mX;

            // Initialize the 'baem_PublicationScheduler' under test and the
            // 'CategoryScheduleOracle' object 'catSchedule'.
            bdet_TimeInterval      defaultInterval(INVALID);
            CategoryScheduleOracle catSchedule(Z);
            {
                bsl::vector<Action>::const_iterator spIt = actions.begin();
                for (; spIt != actions.end(); ++spIt) {
                    bdet_TimeInterval interval(0, spIt->interval()*TIME_UNIT);
                    if (spIt->type() == Action::SCHEDULE_CATEGORY) {
                        catSchedule[spIt->category()] = interval;
                        mX.scheduleCategory(spIt->category(), interval);
                    }
                    else {
                        defaultInterval = interval;
                        mX.setDefaultSchedule(defaultInterval);
                    }
                }
            }

            //    3. Convert the 'CategoryScheduleOracle' to a 'ScheduleOracle'
            //       "oracle", mapping a frequency to the set of
            //       categories published at that frequency.
            ScheduleOracle schedule(Z);
            {
                bsl::set<const Category *> nonDefaultCategories(Z);
                CategoryScheduleOracle::const_iterator csIt =
                                                          catSchedule.begin();
                for (; csIt != catSchedule.end(); ++csIt) {
                    nonDefaultCategories.insert(csIt->first);
                    schedule[csIt->second].insert(csIt->first);
                }
                if (defaultInterval != INVALID) {
                    bsl::insert_iterator<bsl::set<const Category *> >
                                        iIt(schedule[defaultInterval],
                                            schedule[defaultInterval].begin());
                    bsl::set_difference(allCategories.begin(),
                                        allCategories.end(),
                                        nonDefaultCategories.begin(),
                                        nonDefaultCategories.end(),
                                        iIt);
                }
            }

            if (veryVeryVerbose) {
                bsl::cout << "baem_PublicationScheduler:" << bsl::endl;;
                MX.print(bsl::cout, 1, 3);
            }

            //    4. Sleep until each scheduled category has been published
            timer.start();
            microSleep(50 * 1000, 0);  // 10 ms
            timer.stop();

            if (veryVeryVerbose) {
                bsl::cout << "Published records:" << bsl::endl;
                tp.print(bsl::cout, 1, 3);
            }

            //    5. Verify that the 'publish' method was invoked correctly by
            //       comparing the 'Schedule' "oracle" to the 'TestPublisher'
            //       objects reported invocations.
            ASSERT(tp.uniqueInvocations() == schedule.size());
            ScheduleOracle::const_iterator it = schedule.begin();
            for (; it != schedule.end(); ++it) {
                ASSERT(0 != tp.findInvocation(it->second));
            }

            manager.publishAll(); // reset the previous publication time
            tp.reset();
        }
        } BAEM_END_RETRY_TEST
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS & ACCESSORS (BOOTSTRAP):
        //
        // Concerns:
        //   The primary fields must be correctly modifiable and accessible.
        //   In addition that the 'scheduleCategory' and 'setDefaultSchedule'
        //   methods properly reschedule publications, and that the number of
        //   timers configured is equal to the size of the set of unique time
        //   intervals that are scheduled.
        //
        //   Note that while this test verifies the publication scheduler
        //   maintains the correct schedule, and the correct *number* of
        //   underlying 'bcep_TimerEventScheduler::Clock' values, it does
        //   *not* verify that the schedule of events is correctly applied
        //   w.r.t the underlying timer-event scheduler.  (see case 4 & 5).
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent schedule configurations
        //   described using the generator language 'gg':
        //   1. Use the default constructor to create an object then use the
        //      basic manipulators configure the scheduler according to the
        //      configuration description.  Simultaneously set the value of an
        //       "oracle" 'CategorySchedule' (a map from category to scheduled
        //       frequency),
        //   2.  Verify that the value returned from the basic accessors is
        //       equal to the "oracle" value and that the number of clocks in
        //       the timer is equal to the number of unique scheduled
        //       intervals.
        // Note that the destructor is exercised on each configuration as the
        // object being tested leaves scope.
        //
        // Testing:
        //   baem_PublicationScheduler(baem_MetricsManager *,
        //                             bcep_TimerEventScheduler *,
        //                             bslma_Allocator *);
        //   ~baem_PublicationScheduler();
        //   baem_MetricsManager *manager();
        //   void scheduleCategory(const Category *, const bdet_TimeInterval&);
        //   void setDefaultSchedule(const bdet_TimeInterval& )
        //   const baem_MetricsManager *manager() const;
        //   bool findCategorySchedule(bdet_TimeInterval   *result,
        //                            const baem_Category *category) const;
        //   bool getDefaultSchedule(bdet_TimeInterval *result) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRIMARY METHODS (BOOTSTRAP)" << endl
                          << "===================================" << endl;

        bcep_TimerEventScheduler  timer(Z);
        baem_MetricsManager       manager(Z);
        baem_MetricRegistry&      registry = manager.metricRegistry();

        const char *CATEGORIES[] = {"A", "B", "C", "D", "dummy1", "dummy2"};
        const int NUM_CATEGORIES = sizeof CATEGORIES / sizeof *CATEGORIES;

        bsl::vector<const Category *> allCategories(Z);
        for (int i = 0; i < NUM_CATEGORIES; ++i) {
            registry.getCategory(CATEGORIES[i]);
        }
        registry.getAllCategories(&allCategories);
        ASSERT(NUM_CATEGORIES == allCategories.size());

        {
            if (veryVerbose) {
                cout << "\tTesting constructor\n";
            }

            Obj mX(&manager, &timer, Z); const Obj& MX = mX;
            baem_MetricsManager *modifiableManager = mX.manager();
            ASSERT(&manager == MX.manager());
            ASSERT(&manager == mX.manager());

            bdet_TimeInterval interval;
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                ASSERT(!MX.findCategorySchedule(&interval, allCategories[i]));
            }
            ASSERT(!MX.getDefaultSchedule(&interval));
        }

        if (veryVerbose) {
            cout << "\tTesting manipulators and accessors\n";
        }

        const char *TEST_SPECS[] = {
            "X1", "A2", "B2", "C3", "D4", "X5", "A6",  // trivial
            "A1B2", "A2B3", "A4B5", "A1D4", "C3A2",    // 2 categories
            "A1B1", "A2B2", "B3C3", "B4D4", "C3D4",    // duplicate intervals
            "X1A1", "A1X1", "B3X2", "B4X9", "D1X4",    // default
            "A1B1X1", "X1A1B1", "A1X1B1", "C3X1D8",    // 2 cat + default
            "A1A1", "A1A2", "A1B1B2", "A1B4A1",        // rescheduling
            "C1C2A1", "C1C2A3", "D1D2D3D4",            // categories
            "A1D1A2D2", "A1B1C2A2B2", "D2C1B1C3"
            "A1B1C1D1A2B2C3D3A4B4C4D5"
            "A5B6C7D8A1B1C1D1",
            "A5B6C7D8D1C1B1A1",
            "X1X1", "X8X8", "X1X9", "X9X3X3",          // reschedule w/ default
            "A1X2X1", "A1X2X3", "A1X2A2", "X1A1X2",
            "X4X6D6D4", "A1X2B2X1A2", "A2X4B5X5X6D6A6",
            "X1A1B1C1D1X1B2C3D4X3A2D4X4X7A5B1C1X1",
            "A1B2C3D4X5",

        };
        const int NUM_SPECS = sizeof TEST_SPECS / sizeof *TEST_SPECS;

        for (int i = 0; i < NUM_SPECS; ++i) {
            bsl::vector<Action> actions(Z);
            gg(&actions, registry, TEST_SPECS[i]);

            if (veryVeryVerbose) {
                P_(i); P(TEST_SPECS[i]);
                P(actions);
            }

            //   1. Use the default constructor to create an object then use
            //      the basic manipulators configure the scheduler according
            //      to the configuration description.  Simultaneously set the
            //      value of an "oracle" 'CategoryScheduleOracle' (a map from
            //      category to scheduled frequency),
            bdet_TimeInterval      defaultInterval(INVALID);
            CategoryScheduleOracle intervalMap(Z);
            Obj mX(&manager, &timer, Z); const Obj& MX = mX;

            // Use 'scheduleCategory' and 'setDefaultSchedule' to set the
            // schedule for the publication scheduler.
            bsl::vector<Action>::const_iterator spIt = actions.begin();
            for (; spIt != actions.end(); ++spIt) {
                bdet_TimeInterval interval(spIt->interval(), 0);
                if (spIt->type() == Action::SCHEDULE_CATEGORY) {
                    intervalMap[spIt->category()] = interval;
                    mX.scheduleCategory(spIt->category(), interval);
                }
                else {
                    defaultInterval = interval;
                    mX.setDefaultSchedule(defaultInterval);
                }
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            //   2.  Verify that the value returned from the basic accessors is
            //       equal to the "oracle" value and that the number of clocks
            //       in the timer is equal to the number of unique scheduled
            //       intervals.

            // Use 'findCategorySchedule' and 'getDefaultSchedule' to verify
            // the results against an "oracle" ('defaultInterval' and
            // 'intervalMap').
            bdet_TimeInterval interval;
            ASSERT((defaultInterval != INVALID) ==
                   MX.getDefaultSchedule(&interval));
            ASSERT((defaultInterval == INVALID) ||
                   (defaultInterval == interval));
            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                const Category *CATEGORY = allCategories[i];
                bool found = intervalMap.find(CATEGORY) != intervalMap.end();
                ASSERT(found == MX.findCategorySchedule(&interval, CATEGORY));
                ASSERT((!found) || (interval == intervalMap[CATEGORY]));
            }

            // Verify the correct number of clocks in the underlying event
            // scheduler.
            int uniqueIntervals = countUniqueIntervals(intervalMap,
                                                       defaultInterval);
            ASSERT(uniqueIntervals == timer.numClocks());
            ASSERT(0               == timer.numEvents());
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING HELPERS: TestPublisher, gg, BAEM_*_RETRY_TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "TEST HELPERS: TestPublisher, gg, BAEM_*_RETRY_TEST\n"
            << "==================================================\n";

        {
            if (veryVerbose) cout << "\tTesting 'gg'\n";
            baem_MetricRegistry reg(Z);

            const char *specification = "A1B2C3D4X5D6C7B8A9X1";
            bsl::vector<Action> result(Z);
            gg(&result, reg, specification);

            ASSERT(10 == result.size());

            ASSERT(Action::SCHEDULE_CATEGORY == result[0].type());
            ASSERT(reg.getCategory("A")      == result[0].category());
            ASSERT(1                         == result[0].interval());

            ASSERT(Action::SCHEDULE_CATEGORY == result[1].type());
            ASSERT(reg.getCategory("B")      == result[1].category());
            ASSERT(2                         == result[1].interval());

            ASSERT(Action::SCHEDULE_CATEGORY == result[2].type());
            ASSERT(reg.getCategory("C")      == result[2].category());
            ASSERT(3                         == result[2].interval());

            ASSERT(Action::SCHEDULE_CATEGORY == result[3].type());
            ASSERT(reg.getCategory("D")      == result[3].category());
            ASSERT(4                         == result[3].interval());

            ASSERT(Action::SCHEDULE_DEFAULT  == result[4].type());
            ASSERT(0                         == result[4].category());
            ASSERT(5                         == result[4].interval());

            ASSERT(Action::SCHEDULE_CATEGORY == result[5].type());
            ASSERT(reg.getCategory("D")      == result[5].category());
            ASSERT(6                         == result[5].interval());

            ASSERT(Action::SCHEDULE_CATEGORY == result[6].type());
            ASSERT(reg.getCategory("C")      == result[6].category());
            ASSERT(7                         == result[6].interval());

            ASSERT(Action::SCHEDULE_CATEGORY == result[7].type());
            ASSERT(reg.getCategory("B")      == result[7].category());
            ASSERT(8                         == result[7].interval());

            ASSERT(Action::SCHEDULE_CATEGORY == result[8].type());
            ASSERT(reg.getCategory("A")      == result[8].category());
            ASSERT(9                         == result[8].interval());

            ASSERT(Action::SCHEDULE_DEFAULT  == result[9].type());
            ASSERT(0                         == result[9].category());
            ASSERT(1                         == result[9].interval());

            P(result);
        }
        {
            if (veryVerbose) cout << "\tTesting 'TestPublisher'\n";
            baem_MetricRegistry reg(Z);

            const char *SETS[] = { "A", "B", "CD", "EF", "GHI" };
            const int NUM_SETS = sizeof SETS / sizeof *SETS;

            TestPublisher tp(Z);
            for (int i = 0; i < NUM_SETS; ++i) {
                const char *SET_SPEC = SETS[i];
                const char *c = SET_SPEC;

                bsl::vector<baem_MetricRecord> records(Z);
                int length = 0;
                for (; *c; ++c, ++length) {
                    bsl::string category(1, *c, Z);
                    records.push_back(
                         baem_MetricRecord(reg.getId(category.c_str(), "X")));
                }
                int key = SET_SPEC[0] - 'A' + 1;
                bdet_TimeInterval elapsedTime(key, 0);
                bdet_Datetime     datetime(1, 1, 1, 17);
                bdet_DatetimeTz   timeStamp(datetime, key);
                baem_MetricSample sample(Z);
                sample.setTimeStamp(timeStamp);
                sample.appendGroup(&records.front(),
                                   records.size(),
                                   elapsedTime);

                if (veryVeryVerbose) {
                    P_(i); P_(SET_SPEC); P(sample);
                }

                for (int j = 0; j < key; ++j) {
                    tp.publish(sample);
                }
            }

            // Verify the results.
            bdet_TimeInterval expElapsedTime;
            bdet_DatetimeTz   expTimeStamp;
            int               expTotalCount = 0;
            ASSERT(0 == defaultAllocator.numBytesInUse());
            for (int i = 0; i < NUM_SETS; ++i) {
                const char *SET_SPEC = SETS[i];
                const char *c = SET_SPEC;

                bsl::set<const Category *> categories(Z);
                int length = 0;
                for (; *c; ++c, ++length) {
                    bsl::string category(1, *c, Z);
                    categories.insert(reg.getCategory(category.c_str()));
                }
                int key = SET_SPEC[0] - 'A' + 1;
                int expCount = key;
                expTotalCount += expCount;
                expElapsedTime = bdet_TimeInterval(key, 0);
                bdet_Datetime datetime(1, 1, 1, 17);
                expTimeStamp   = bdet_DatetimeTz(datetime, key);

                const TestPublisher::InvocationSummary *invocation =
                                                tp.findInvocation(categories);

                ASSERT(0              != invocation);
                ASSERT(expCount       == invocation->d_numInvocations);
                ASSERT(expTimeStamp   == invocation->d_timeStamp);
                ASSERT(expElapsedTime == invocation->d_elapsedTime);
            }
            ASSERT(expTimeStamp   == tp.lastTimeStamp());
            ASSERT(expElapsedTime == tp.lastElapsedTime());
            ASSERT(expTotalCount  == tp.invocations());
            ASSERT(NUM_SETS       == tp.uniqueInvocations());
        }
        {
            if (veryVerbose)
                cout << "\tTesting BAEM_*_RETRY_TEST\n";

            {
                static int count = 0;
                BAEM_BEGIN_RETRY_TEST_IMP(5, !verbose) {
                    ++count;
                } BAEM_END_RETRY_TEST
                ASSERT(1 == count);
            }
            {
                static int count = 0;
                BAEM_BEGIN_RETRY_TEST_IMP(5, !verbose) {
                    if (count < 4) {
                        ++testStatus;
                    }
                    ++count;
                } BAEM_END_RETRY_TEST
                LOOP_ASSERT(count, 5 == count);
                ASSERT(0 == testStatus);
            }
            {
                int savedStatus = testStatus;
                static int count = 0;
                BAEM_BEGIN_RETRY_TEST_IMP(5, !verbose) {
                    ++testStatus;
                    ++count;
                } BAEM_END_RETRY_TEST
                int updatedStatus = testStatus;
                testStatus = savedStatus;
                ASSERT(updatedStatus == savedStatus + 1)
                LOOP_ASSERT(count, 5 == count);
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Plan:
        //   Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bcep_TimerEventScheduler  timer(Z);
        baem_MetricsManager       manager(Z);

        TestPublisher tp1(Z), tp2(Z), tp3(Z), tp4(Z);
        PubPtr p1(&tp1, bcema_SharedPtrNilDeleter(), Z);
        PubPtr p2(&tp2, bcema_SharedPtrNilDeleter(), Z);
        PubPtr p3(&tp3, bcema_SharedPtrNilDeleter(), Z);
        PubPtr p4(&tp4, bcema_SharedPtrNilDeleter(), Z);

        ASSERT(0 == manager.addSpecificPublisher("A", p1));
        ASSERT(0 == manager.addSpecificPublisher("B", p2));
        ASSERT(0 == manager.addSpecificPublisher("C", p3));
        ASSERT(0 == manager.addSpecificPublisher("D", p4));

        // Ensure metrics are collected for each category.
        manager.collectorRepository().getDefaultCollector("A", "A");
        manager.collectorRepository().getDefaultCollector("B", "B");
        manager.collectorRepository().getDefaultCollector("C", "C");
        manager.collectorRepository().getDefaultCollector("D", "D");
        {
            bdet_TimeInterval foundInterval;
            bdet_TimeInterval intvl_A(0, 1 * NANOSECS_PER_MICROSEC);
            bdet_TimeInterval intvl_B(0, 2 * NANOSECS_PER_MICROSEC);
            bdet_TimeInterval intvl_C(0, 3 * NANOSECS_PER_MICROSEC);
            bdet_TimeInterval intvl_D(0, 4 * NANOSECS_PER_MICROSEC);
            const bdet_TimeInterval& INTVL_A = intvl_A;
            const bdet_TimeInterval& INTVL_B = intvl_B;
            const bdet_TimeInterval& INTVL_C = intvl_C;
            const bdet_TimeInterval& INTVL_D = intvl_D;

            if (veryVerbose) {
                cout << "\tCreate schedule and test initial properties\n";
            }

            Obj mX(&manager, &timer, Z); const Obj& MX = mX;

            if (veryVeryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }

            ASSERT(0 == timer.numClocks());
            ASSERT(0 == timer.numEvents());
            ASSERT(&manager == mX.manager());
            ASSERT(&manager == MX.manager());

            ASSERT(!MX.findCategorySchedule(&foundInterval, "A"));
            ASSERT(!MX.findCategorySchedule(&foundInterval, "B"));
            ASSERT(!MX.findCategorySchedule(&foundInterval, "C"));
            ASSERT(!MX.findCategorySchedule(&foundInterval, "D"));
            ASSERT(!MX.getDefaultSchedule(&foundInterval));

            if (veryVerbose) {
                cout << "\tSchedule category A at interval A\n";
            }

            mX.scheduleCategory("A", INTVL_A);

            if (veryVeryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }

            ASSERT(1 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            ASSERT(MX.findCategorySchedule(&foundInterval, "A"));
            ASSERT(INTVL_A == foundInterval);
            ASSERT(!MX.findCategorySchedule(&foundInterval, "B"));
            ASSERT(!MX.findCategorySchedule(&foundInterval, "C"));
            ASSERT(!MX.findCategorySchedule(&foundInterval, "D"));
            ASSERT(!MX.getDefaultSchedule(&foundInterval));

            if (veryVerbose) {
                cout << "\tSchedule category B at interval B\n";
            }

            mX.scheduleCategory("B", INTVL_B);

            if (veryVeryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }

            ASSERT(2 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            ASSERT(MX.findCategorySchedule(&foundInterval, "A"));
            ASSERT(INTVL_A == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "B"));
            ASSERT(INTVL_B == foundInterval);
            ASSERT(!MX.findCategorySchedule(&foundInterval, "C"));
            ASSERT(!MX.findCategorySchedule(&foundInterval, "D"));
            ASSERT(!MX.getDefaultSchedule(&foundInterval));

            if (veryVerbose) {
                cout << "\tSchedule category C at interval C\n";
            }

            mX.scheduleCategory("C", INTVL_C);

            if (veryVeryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }

            ASSERT(3 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            ASSERT(MX.findCategorySchedule(&foundInterval, "A"));
            ASSERT(INTVL_A == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "B"));
            ASSERT(INTVL_B == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "C"));
            ASSERT(INTVL_C == foundInterval);
            ASSERT(!MX.findCategorySchedule(&foundInterval, "D"));
            ASSERT(!MX.getDefaultSchedule(&foundInterval));

            if (veryVerbose) {
                cout << "\tReschedule category C to interval B\n";
            }
            mX.scheduleCategory("C", INTVL_B);

            if (veryVeryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }

            ASSERT(2 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            ASSERT(MX.findCategorySchedule(&foundInterval, "A"));
            ASSERT(INTVL_A == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "B"));
            ASSERT(INTVL_B == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "C"));
            ASSERT(INTVL_B == foundInterval);
            ASSERT(!MX.findCategorySchedule(&foundInterval, "D"));
            ASSERT(!MX.getDefaultSchedule(&foundInterval));

            if (veryVerbose) {
                cout << "\tReschedule category C back to interval C\n";
            }
            mX.scheduleCategory("C", INTVL_C);

            if (veryVeryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }

            ASSERT(3 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            ASSERT(MX.findCategorySchedule(&foundInterval, "A"));
            ASSERT(INTVL_A == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "B"));
            ASSERT(INTVL_B == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "C"));
            ASSERT(INTVL_C == foundInterval);
            ASSERT(!MX.findCategorySchedule(&foundInterval, "D"));
            ASSERT(!MX.getDefaultSchedule(&foundInterval));

            if (veryVerbose) {
                cout << "\tReschedule category C back to interval A\n";
            }
            mX.scheduleCategory("C", INTVL_A);

            if (veryVeryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }

            ASSERT(2 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            ASSERT(MX.findCategorySchedule(&foundInterval, "A"));
            ASSERT(INTVL_A == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "B"));
            ASSERT(INTVL_B == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "C"));
            ASSERT(INTVL_A == foundInterval);
            ASSERT(!MX.findCategorySchedule(&foundInterval, "D"));
            ASSERT(!MX.getDefaultSchedule(&foundInterval));

            if (veryVerbose) {
                cout << "\tSet default schedule to interval D\n";
            }

            mX.setDefaultSchedule(INTVL_D);

            if (veryVeryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }

            ASSERT(3 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            ASSERT(MX.findCategorySchedule(&foundInterval, "A"));
            ASSERT(INTVL_A == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "B"));
            ASSERT(INTVL_B == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "C"));
            ASSERT(INTVL_A == foundInterval);
            ASSERT(!MX.findCategorySchedule(&foundInterval, "D"));
            ASSERT(MX.getDefaultSchedule(&foundInterval));
            ASSERT(INTVL_D == foundInterval);

            if (veryVerbose) {
                cout << "\nReset default schedule to interval A\n";
            }

            mX.setDefaultSchedule(INTVL_A);

            if (veryVeryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }

            ASSERT(2 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            ASSERT(MX.findCategorySchedule(&foundInterval, "A"));
            ASSERT(INTVL_A == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "B"));
            ASSERT(INTVL_B == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "C"));
            ASSERT(INTVL_A == foundInterval);
            ASSERT(!MX.findCategorySchedule(&foundInterval, "D"));
            ASSERT(MX.getDefaultSchedule(&foundInterval));
            ASSERT(INTVL_A == foundInterval);

            if (veryVerbose) {
                cout << "\tCancel category C\n";
            }

            ASSERT(0 == mX.cancelCategorySchedule("C"));
            if (veryVeryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }

            ASSERT(0 != mX.cancelCategorySchedule("C"));
            ASSERT(2 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            ASSERT(MX.findCategorySchedule(&foundInterval, "A"));
            ASSERT(INTVL_A == foundInterval);
            ASSERT(MX.findCategorySchedule(&foundInterval, "B"));
            ASSERT(INTVL_B == foundInterval);
            ASSERT(!MX.findCategorySchedule(&foundInterval, "C"));
            ASSERT(!MX.findCategorySchedule(&foundInterval, "D"));
            ASSERT(MX.getDefaultSchedule(&foundInterval));
            ASSERT(INTVL_A == foundInterval);

            if (veryVerbose) {
                cout << "\tCancel category B\n";
            }

            ASSERT(0 == mX.cancelCategorySchedule("B"));
            if (veryVeryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }
            ASSERT(0 != mX.cancelCategorySchedule("B"));
            ASSERT(1 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            ASSERT(MX.findCategorySchedule(&foundInterval, "A"));
            ASSERT(INTVL_A == foundInterval);
            ASSERT(! MX.findCategorySchedule(&foundInterval, "B"));
            ASSERT(! MX.findCategorySchedule(&foundInterval, "C"));
            ASSERT(! MX.findCategorySchedule(&foundInterval, "D"));
            ASSERT(MX.getDefaultSchedule(&foundInterval));
            ASSERT(INTVL_A == foundInterval);

            if (veryVerbose) {
                cout << "\tCancel category A\n";
            }

            ASSERT(0 == mX.cancelCategorySchedule("A"));
            if (veryVeryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }
            ASSERT(0 != mX.cancelCategorySchedule("A"));
            ASSERT(1 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            ASSERT(!MX.findCategorySchedule(&foundInterval, "A"));
            ASSERT(!MX.findCategorySchedule(&foundInterval, "B"));
            ASSERT(!MX.findCategorySchedule(&foundInterval, "C"));
            ASSERT(!MX.findCategorySchedule(&foundInterval, "D"));
            ASSERT(MX.getDefaultSchedule(&foundInterval));
            ASSERT(INTVL_A == foundInterval);

            if (veryVerbose) {
                cout << "\tClear default schedule\n";
            }

            ASSERT(0 == mX.clearDefaultSchedule());
            if (veryVeryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }

            ASSERT(0 != mX.clearDefaultSchedule());
            ASSERT(0 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            ASSERT(!MX.findCategorySchedule(&foundInterval, "A"));
            ASSERT(!MX.findCategorySchedule(&foundInterval, "B"));
            ASSERT(!MX.findCategorySchedule(&foundInterval, "C"));
            ASSERT(!MX.findCategorySchedule(&foundInterval, "D"));
            ASSERT(!MX.getDefaultSchedule(&foundInterval));

            if (veryVerbose) {
                cout << "\tAdd 3 categories and test cancelAll\n";
            }

            mX.scheduleCategory("A", INTVL_A);
            mX.scheduleCategory("B", INTVL_B);
            mX.scheduleCategory("C", INTVL_C);

            if (veryVeryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }
            LOOP_ASSERT(timer.numClocks(), 3 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            mX.cancelAll();
            if (veryVeryVerbose) {
                MX.print(bsl::cout, 1, 3);
            }

            ASSERT(0 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            ASSERT(!MX.findCategorySchedule(&foundInterval, "A"));
            ASSERT(!MX.findCategorySchedule(&foundInterval, "B"));
            ASSERT(!MX.findCategorySchedule(&foundInterval, "C"));
            ASSERT(!MX.findCategorySchedule(&foundInterval, "D"));

            if (veryVerbose) {
                cout << "\tAdd categories and destroy the scheduler\n";
            }
            mX.scheduleCategory("A", INTVL_A);
            mX.scheduleCategory("B", INTVL_B);
            mX.scheduleCategory("C", INTVL_C);

            ASSERT(3 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        ASSERT(0 == timer.numClocks());
        ASSERT(0 == timer.numEvents());

        {
            BAEM_BEGIN_RETRY_TEST(3) {
            if (veryVerbose) {
                cout << "\tVerify callbacks are invoked\n";
            }

            bdet_TimeInterval intvl_A(0, 250 * NANOSECS_PER_MILLISEC);
            bdet_TimeInterval intvl_B(0, 550 * NANOSECS_PER_MILLISEC);
            const bdet_TimeInterval& INTVL_A = intvl_A;
            const bdet_TimeInterval& INTVL_B = intvl_B;

            Obj mX(&manager, &timer, Z); const Obj& MX = mX;

            mX.scheduleCategory("A", INTVL_A);
            mX.scheduleCategory("B", INTVL_B);
            mX.scheduleCategory("C", INTVL_A);

            ASSERT(2 == timer.numClocks());
            ASSERT(0 == timer.numEvents());

            microSleep(MICROSECS_PER_MILLISEC * 10, 0);
            manager.publishAll();
            tp1.reset(); tp2.reset(); tp3.reset(); tp4.reset();

            timer.start();

            microSleep(600 * MICROSECS_PER_MILLISEC, 0);

            timer.stop();

            ASSERT(2 == tp1.invocations());
            ASSERT(1 == tp2.invocations());
            ASSERT(2 == tp3.invocations());
            ASSERT(0 == tp4.invocations());

            // Verify publisher 1 and 3 were invoked by the same call to
            // metrics manager publisher.
            ASSERT(tp1.lastTimeStamp() == tp3.lastTimeStamp());
            ASSERT(tp1.lastTimeStamp() != tp2.lastTimeStamp());

            ASSERT(tp1.lastElapsedTime() == tp3.lastElapsedTime());
            ASSERT(tp1.lastElapsedTime() != tp2.lastElapsedTime());

            if (veryVeryVerbose) {
                P(tp1.lastElapsedTime());
                P(tp2.lastElapsedTime());
                P(tp3.lastElapsedTime());

                P(tp1.lastTimeStamp());
                P(tp2.lastTimeStamp());
                P(tp3.lastTimeStamp());
            }

            ASSERT(withinWindow(tp1.lastElapsedTime(), INTVL_A, 40));
            ASSERT(withinWindow(tp2.lastElapsedTime(), INTVL_B, 40));
            ASSERT(withinWindow(tp3.lastElapsedTime(), INTVL_A, 40));

            ASSERT(0 == defaultAllocator.numBytesInUse());
            } BAEM_END_RETRY_TEST
        }

        ASSERT(0 == defaultAllocator.numBytesInUse());

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
