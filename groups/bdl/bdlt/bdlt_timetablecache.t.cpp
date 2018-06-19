// bdlt_timetablecache.t.cpp                                          -*-C++-*-
#include <bdlt_timetablecache.h>

#include <bdlt_timetableloader.h>
#include <bdlt_currenttime.h>
#include <bdlt_date.h>            // for testing only
#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>
#include <bdlt_timetable.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_climits.h>    // 'INT_MAX'
#include <bsl_cstdlib.h>    // 'atoi'
#include <bsl_cstring.h>    // 'strcmp'
#include <bsl_iostream.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a thread-safe mechanism, 'bdlt::TimetableCache', that is a
// cache of 'bdlt::Timetable' objects.  We make extensive use of a concrete
// instance of 'bdlt::TimetableLoader', 'TestLoader', that has been crafted for
// our purposes, and the 'bslma' test allocator.
//
// Two issues of note are taken into account in testing this component.  One is
// that 'bsl::shared_ptr' objects can take ownership of timetables that have
// been removed from the cache (either via 'invalidate' or 'invalidateAll', or
// due to a timeout).  We need to be mindful of this when, for example,
// checking test allocators for expected memory use.  To that end, shared
// pointers are scoped in such a way that takes this into account.
//
// The second issue is that of the timeout with which a cache object may be
// optionally configured.  Thorough testing of timeouts is problematic in that
// it entails use of 'sleep', which increases the running time of test cases
// and can lead to intermittent failures in regression testing.  Consequently,
// we make very judicious use of 'sleep' in positively-numbered test cases; in
// particular, we generally use a timeout of 0 where we are only interested in
// whether or not a cache has been constructed with a timeout.  We relegate the
// few lengthy sleeps in the test driver to a negative test case (-1).
//
// Separate test cases are included for ensuring exception-neutrality and
// thread-safety.  Finally, negative testing is applied, in applicable test
// cases, to ensure that precondition violations are detected in appropriate
// build modes.
// ----------------------------------------------------------------------------
// 'TimetableCache' class:
// [ 2] TimetableCache(Loader *loader,          Allocator *ba = 0);
// [ 2] TimetableCache(Loader *loader, timeout, Allocator *ba = 0);
// [ 2] ~TimetableCache();
// [ 3] shared_ptr<const Timetable> getTimetable(const char *name);
// [ 4] int invalidate(const char *name);
// [ 4] int invalidateAll();
// [ 3] shared_ptr<const Timetable> lookupTimetable(const char *n) const;
// [ 3] Datetime lookupLoadTime(const char *name) const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ *] CONCERN: Precondition violations are detected when enabled.
// [ 5] CONCERN: All memory allocation is exception neutral.
// [ 6] CONCERN: All manipulators and accessors are thread-safe.
// [-1] CONCERN: A non-trivial timeout is processed correctly.

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlt::TimetableCache                   Obj;
typedef bsl::shared_ptr<const bdlt::Timetable> Entry;

typedef bsls::TimeInterval                     Interval;
typedef bdlt::Datetime                         Datetime;

#ifdef BSLS_PLATFORM_OS_WINDOWS
typedef HANDLE    ThreadId;
#else
typedef pthread_t ThreadId;
#endif

extern "C" {
    typedef void *(*ThreadFunction)(void *arg);
}

bdlt::DatetimeInterval                         OneSecond(0, 0, 0, 1);

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT((bslma::UsesBslmaAllocator<Obj>::VALUE));

// ============================================================================
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static const bdlt::Date gFirstDate1(2000, 1, 1);   // "CAL-1" timetable
static const bdlt::Date gFirstDate2(2005, 5, 15);  // "CAL-2"    "
static const bdlt::Date gFirstDate3(2010, 9, 30);  // "CAL-3"    "

static const bdlt::Date gLastDate(2020, 12, 31);

class TestLoader : public bdlt::TimetableLoader {
    // This concrete timetable loader successfully loads timetables named
    // "CAL-1", "CAL-2", and "CAL-3" only.  An attempt to load a timetable by
    // any other name results in a non-zero result status being returned from
    // the 'load' method.

  private:
    // NOT IMPLEMENTED
    TestLoader(const TestLoader&);             // = delete
    TestLoader& operator=(const TestLoader&);  // = delete

  public:
    // CREATORS
    TestLoader();
        // Create a test loader.

    ~TestLoader();
        // Destroy this object.

    // MANIPULATORS
    int load(bdlt::Timetable *result, const char *timetableName);
        // Load, into the specified 'result', the timetable identified by the
        // specified 'timetableName'.  Return 0 on success, and a non-zero
        // value otherwise.
};

// CREATORS
inline
TestLoader::TestLoader()
{
}

TestLoader::~TestLoader()
{
}

int TestLoader::load(bdlt::Timetable *result, const char *timetableName)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timetableName);

    if (0 == bsl::strcmp("CAL-1", timetableName)
     || 0 == bsl::strcmp("CAL-2", timetableName)
     || 0 == bsl::strcmp("CAL-3", timetableName)) {

        // Three timetables are recognized by this loader.

        bdlt::Date firstDate;

        switch (timetableName[4]) {
          case '1': {
            firstDate = gFirstDate1;
          } break;
          case '2': {
            firstDate = gFirstDate2;
          } break;
          case '3': {
            firstDate = gFirstDate3;
          } break;
          default: {
            ASSERT(!"Internal error.");
          }
        }

        result->reset();
        result->setValidRange(firstDate, gLastDate);

        // The following is required to trigger an allocation in 'result'.

        result->addTransition(bdlt::Datetime(firstDate), 1);

        return 0;                                                     // RETURN
    }

    if (0 == bsl::strcmp("ERROR", timetableName)) {
        return -1;                                                    // RETURN
    }

    return 1;
}

static
bool providesNonModifiableAccessOnly(bdlt::Timetable *)
    // Return 'false'.
{
    return false;
}

static
bool providesNonModifiableAccessOnly(const bdlt::Timetable *)
    // Return 'true'.
{
    return true;
}

static
ThreadId createThread(ThreadFunction function, void *arg)
    // Create a thread whose entry point is the specified 'function' that is
    // supplied with the specified 'arg' list, and return the id of the new
    // thread.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return CreateThread(0, 0, (LPTHREAD_START_ROUTINE)function, arg, 0, 0);
#else
    ThreadId id;
    pthread_create(&id, 0, function, arg);
    return id;
#endif
}

static
void joinThread(ThreadId id)
    // Join with the thread having the specified 'id'.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    WaitForSingleObject(id, INFINITE);
    CloseHandle(id);
#else
    pthread_join(id, 0);
#endif
}

static
void sleepSeconds(int seconds)
    // Sleep for the specified number of 'seconds'.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    Sleep(seconds * 1000);
#else
    sleep(seconds);
#endif
}

namespace TestCase6 {

struct ThreadInfo {
    int  d_numIterations;
    Obj *d_cache_p;
    Obj *d_cacheWithTimeout_p;
};

extern "C" void *threadFunction1(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    Obj& mX = *info->d_cache_p;             const Obj& X = mX;
    Obj& mY = *info->d_cacheWithTimeout_p;  const Obj& Y = mY;

    for (int i = 0; i < info->d_numIterations; ++i) {
        {
            Entry e = mX.getTimetable("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = Y.lookupTimetable("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = mX.getTimetable("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = Y.lookupTimetable("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = mX.getTimetable("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = Y.lookupTimetable("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        if (0 == i % 3) {
            mX.invalidate("CAL-1");
        }

        if (0 == i % 10) {
            mX.invalidateAll();
        }

        {
            Entry e = mY.getTimetable("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = X.lookupTimetable("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = mY.getTimetable("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = X.lookupTimetable("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = mY.getTimetable("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = X.lookupTimetable("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        if (0 == i % 3) {
            mY.invalidate("CAL-1");
        }
    }

    return arg;
}

extern "C" void *threadFunction2(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    Obj& mX = *info->d_cache_p;             const Obj& X = mX;
    Obj& mY = *info->d_cacheWithTimeout_p;  const Obj& Y = mY;

    for (int i = 0; i < info->d_numIterations; ++i) {
        {
            Entry e = mX.getTimetable("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = Y.lookupTimetable("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = mX.getTimetable("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = Y.lookupTimetable("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = mX.getTimetable("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = Y.lookupTimetable("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        if (0 == i % 5) {
            mX.invalidate("CAL-2");
        }

        if (0 == i % 10) {
            mY.invalidateAll();
        }

        {
            Entry e = mY.getTimetable("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = X.lookupTimetable("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = mY.getTimetable("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = X.lookupTimetable("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = mY.getTimetable("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = X.lookupTimetable("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        if (0 == i % 5) {
            mY.invalidate("CAL-2");
        }
    }

    return arg;
}

extern "C" void *threadFunction3(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    Obj& mX = *info->d_cache_p;             const Obj& X = mX;
    Obj& mY = *info->d_cacheWithTimeout_p;  const Obj& Y = mY;

    for (int i = 0; i < info->d_numIterations; ++i) {
        {
            Entry e = mX.getTimetable("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = Y.lookupTimetable("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = mX.getTimetable("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = Y.lookupTimetable("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = mX.getTimetable("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = Y.lookupTimetable("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        if (0 == i % 7) {
            mX.invalidate("CAL-3");
        }

        {
            Entry e = mY.getTimetable("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = X.lookupTimetable("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = mY.getTimetable("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = X.lookupTimetable("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = mY.getTimetable("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = X.lookupTimetable("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        if (0 == i % 7) {
            mY.invalidate("CAL-3");
        }
    }

    return arg;
}

}  // close namespace TestCase6

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

// Define the 'MyTimetableLoader' class that is used in the Usage example.

class MyTimetableLoader : public bdlt::TimetableLoader {

  private:
    // NOT IMPLEMENTED
    MyTimetableLoader(const MyTimetableLoader&);             // = delete
    MyTimetableLoader& operator=(const MyTimetableLoader&);  // = delete

  public:
    // CREATORS
    MyTimetableLoader();
        // Create a 'MyTimetableLoader' object.

    ~MyTimetableLoader();
        // Destroy this object.

    // MANIPULATORS
    int load(bdlt::Timetable *result, const char *timetableName);
        // Load, into the specified 'result', the timetable identified by the
        // specified 'timetableName'.  Return 0 on success, and a non-zero
        // value otherwise.
};

// CREATORS
inline
MyTimetableLoader::MyTimetableLoader()
{
}

MyTimetableLoader::~MyTimetableLoader()
{
}

int MyTimetableLoader::load(bdlt::Timetable *result, const char *timetableName)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timetableName);

    result->reset();
    result->setValidRange(bdlt::Date(2000, 1, 1), bdlt::Date(2020, 12, 31));

    if (     0 == bsl::strcmp("ZERO", timetableName)) {
        result->setInitialTransitionCode(0);

        return 0;                                                     // RETURN
    }
    else if (0 == bsl::strcmp("ONE", timetableName)) {
        result->setInitialTransitionCode(1);

        return 0;                                                     // RETURN
    }
    else if (0 == bsl::strcmp("TWO", timetableName)) {
        result->setInitialTransitionCode(2);

        return 0;                                                     // RETURN
    }
    else {                                            // others not supported
        return -1;                                                    // RETURN
    }
}

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        if (verbose) cout << "Example 1: Using a 'bdlt::TimetableCache'"
                          << endl;
        {
///Usage
///-----
// The following example illustrates how to use a 'bdlt::TimetableCache'.
//
///Example 1: Using a 'bdlt::TimetableCache'
/// - - - - - - - - - - - - - - - - - - - -
// This example shows basic use of a 'bdlt::TimetableCache' object.
//
// In this example, we assume a hypothetical timetable loader,
// 'MyTimetableLoader', the details of which are not important other than that
// it supports timetables identified by "ZERO", "ONE", and "TWO".  Furthermore,
// the value of the initial transition code for each of these timetables is
// given by the timetable's name (e.g., if 'Z' has the value of the timetable
// identified as "ZERO", then '0 == Z.initialTransitionCode()').
//
// First, we create a timetable loader, an instance of 'MyTimetableLoader', and
// use it, in turn, to create a cache.  For the purposes of this example, it is
// sufficient to let the cache use the default allocator:
//..
    MyTimetableLoader    loader;
    bdlt::TimetableCache cache(&loader);
//..
// Next, we retrieve the timetable 'twoA', identified by "TWO", verify that the
// loading of that timetable into the cache was successful ('twoA.get()' is
// non-null), and verify that 2 is the value of the initial transition code for
// timetable "TWO":
//..
    bsl::shared_ptr<const bdlt::Timetable> twoA = cache.getTimetable("TWO");

    ASSERT(twoA.get());
    ASSERT(2 == twoA->initialTransitionCode());
//..
// Then, we fetch the timetable identified by "ONE", this time verifying that 1
// is the value of the initial transition code for the "ONE" timetable:
//..
    bsl::shared_ptr<const bdlt::Timetable> oneA = cache.getTimetable("ONE");

    ASSERT(oneA.get());
    ASSERT(1 == oneA->initialTransitionCode());
//..
// Next, we retrieve the "ONE" timetable again, this time via the
// 'lookupTimetable' accessor, and note that the request is satisfied by the
// timetable that is already in the cache:
//..
    const bdlt::TimetableCache& readonlyCache = cache;

    bsl::shared_ptr<const bdlt::Timetable> oneB =
                                          readonlyCache.lookupTimetable("ONE");

    ASSERT(oneA.get() == oneB.get());
//..
// Then, we invalidate the "TWO" timetable in the cache and immediately fetch
// it again.  The call to 'invalidate' removed the "TWO" timetable from the
// cache, so it had to be reloaded into the cache to satisfy the request:
//..
    int numInvalidated = cache.invalidate("TWO");
    ASSERT(1 == numInvalidated);

    bsl::shared_ptr<const bdlt::Timetable> twoB = cache.getTimetable("TWO");

    ASSERT(twoB.get() != twoA.get());
    ASSERT(twoB.get());
    ASSERT(2 == twoB->initialTransitionCode());
//..
// Next, all timetables in the cache are invalidated, then reloaded:
//..
    numInvalidated = cache.invalidateAll();
    ASSERT(2 == numInvalidated);

    bsl::shared_ptr<const bdlt::Timetable> twoC = cache.getTimetable("TWO");

    ASSERT(twoC.get() != twoA.get());
    ASSERT(twoC.get() != twoB.get());
    ASSERT(twoC.get());
    ASSERT(2 == twoC->initialTransitionCode());

    bsl::shared_ptr<const bdlt::Timetable> oneC = cache.getTimetable("ONE");

    ASSERT(oneC.get() != oneA.get());
    ASSERT(oneC.get() != oneB.get());
    ASSERT(oneC.get());
    ASSERT(1 == oneC->initialTransitionCode());
//..
// Now, verify that references to timetables that were invalidated in the cache
// are still valid for clients that obtained references to them before they
// were made invalid:
//..
    ASSERT(1 == oneA->initialTransitionCode());
    ASSERT(1 == oneB->initialTransitionCode());

    ASSERT(2 == twoA->initialTransitionCode());
    ASSERT(2 == twoB->initialTransitionCode());
//..
// When 'twoA', 'twoB', 'oneA', and 'oneB' go out of scope, the resources used
// by the timetables to which they refer are automatically reclaimed.
//
// Finally, using the 'lookupTimetable' accessor, we attempt to retrieve a
// timetable that has not yet been loaded into the cache, but that we *know* to
// be supported by the timetable loader.  Since the 'lookupTimetable' accessor
// does not load timetables into the cache as a side-effect, the request fails:
//..
    bsl::shared_ptr<const bdlt::Timetable> zero =
                                         readonlyCache.lookupTimetable("ZERO");

    ASSERT(!zero.get());
//..
        }

        if (verbose) cout << "Example 2: A Timetable Cache with a Timeout"
                          << endl;
        {
//
///Example 2: A Timetable Cache with a Timeout
/// - - - - - - - - - - - - - - - - - - - - -
// This second example shows the effects on a 'bdlt::TimetableCache' object
// that is constructed to have a timeout value.  Note that the following
// snippets of code assume a platform-independent 'sleepSeconds' method that
// sleeps for the specified number of seconds.
//
// First, we create a timetable loader and a timetable cache.  The cache is
// constructed to have a timeout of 3 seconds.  Of course, such a short timeout
// is inappropriate for production use, but it is necessary for illustrating
// the effects of a timeout in this example.  As in example 1 (above), we again
// let the cache use the default allocator:
//..
    MyTimetableLoader           loader;
    bdlt::TimetableCache        cache(&loader, bsls::TimeInterval(3, 0));
    const bdlt::TimetableCache& readonlyCache = cache;
//..
// Next, we retrieve the timetable identified by "ZERO" from the cache:
//..
    bsl::shared_ptr<const bdlt::Timetable> zeroA = cache.getTimetable("ZERO");

    ASSERT(zeroA.get());
//..
// Next, we sleep for 2 seconds before retrieving the "ONE" timetable:
//..
    sleepSeconds(2);

    bsl::shared_ptr<const bdlt::Timetable> oneA = cache.getTimetable("ONE");

    ASSERT(oneA.get());
//..
// Next, we sleep for 2 more seconds before attempting to retrieve the "ZERO"
// timetable again, this time using the 'lookupTimetable' accessor.  Since the
// cumulative sleep time exceeds the timeout value established for the cache
// when it was constructed, the "ZERO" timetable has expired; hence, it has
// been removed from the cache:
//..
    sleepSeconds(2);

    bsl::shared_ptr<const bdlt::Timetable> zeroB =
                                         readonlyCache.lookupTimetable("ZERO");

    ASSERT(!zeroB.get());
//..
// Next, we verify that the "ONE" timetable is still available in the cache:
//..
    bsl::shared_ptr<const bdlt::Timetable> oneB =
                                          readonlyCache.lookupTimetable("ONE");

    ASSERT(oneA.get() == oneB.get());
//..
// Finally, we sleep for an additional 2 seconds and verify that the "ONE"
// timetable has also expired:
//..
    sleepSeconds(2);

    bsl::shared_ptr<const bdlt::Timetable> oneC =
                                          readonlyCache.lookupTimetable("ONE");

    ASSERT(!oneC.get());
//..
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // CONCURRENCY
        //   Ensure that all manipulators and accessors are thread-safe.
        //
        // Concerns:
        //: 1 That all manipulators and accessors are thread-safe.
        //
        // Plan:
        //: 1 Create two 'bslma::TestAllocator' objects; install one as the
        //:   current default allocator and supply the other to two caches,
        //:   one that does not have a timeout and one that does.
        //:
        //: 2 Within a loop, create three threads that iterate a specified
        //:   number of times and that perform a different ("random") sequence
        //:   of operations on the two caches from P-1.  (C-1)
        //
        // Testing:
        //   CONCERN: All manipulators and accessors are thread-safe.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONCURRENCY" << endl
                          << "===========" << endl;

        using namespace TestCase6;

        TestLoader loader;

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX(&loader,                 &sa);
        Obj mY(&loader, Interval(0, 0), &sa);

        const int NUM_TEST_ITERATIONS   =  10;
        const int NUM_THREAD_ITERATIONS = 200;

        ThreadInfo info = { NUM_THREAD_ITERATIONS, &mX, &mY };

        for (int ti = 0; ti < NUM_TEST_ITERATIONS; ++ti) {

            if (veryVerbose) P(ti);

            ThreadId id1 = createThread(&threadFunction1, &info);
            ThreadId id2 = createThread(&threadFunction2, &info);
            ThreadId id3 = createThread(&threadFunction3, &info);

            joinThread(id1);
            joinThread(id2);
            joinThread(id3);
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // EXCEPTION NEUTRALITY
        //   Ensure that all memory allocation is exception neutral.
        //
        // Concerns:
        //: 1 That 'getTimetable', the only method that allocates memory, is
        //:   exception neutral.
        //
        // Plan:
        //: 1 We use the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macros to generate
        //:   exceptions in order to verify that there is no memory leak when
        //:   we invoke 'getTimetable'.  (C-1)
        //
        // Testing:
        //   CONCERN: All memory allocation is exception neutral.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EXCEPTION NEUTRALITY" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nTesting a cache without a timeout." << endl;
        {
            TestLoader loader;

            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&loader, &sa);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
              Entry e;
              int   rc;

              e  = mX.getTimetable("ERROR");
                                         ASSERT(!e.get());

              e  = mX.getTimetable("CAL-1");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate1);

              e  = mX.getTimetable("CAL-2");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate2);

              e  = mX.getTimetable("CAL-1");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate1);

              rc = mX.invalidate("CAL-2");
                                         ASSERT(1 == rc);

              e  = mX.getTimetable("CAL-2");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate2);

              rc = mX.invalidateAll();   ASSERT(2 == rc);

              e  = mX.getTimetable("CAL-1");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate1);

              e  = mX.getTimetable("CAL-2");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate2);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            LOOP_ASSERT(da.numBlocksInUse(), 0 == da.numBlocksInUse());
        }

        if (verbose) cout << "\nTesting a cache with a timeout." << endl;
        {
            TestLoader loader;

            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&loader, Interval(0, 0), &sa);  const Obj& X = mX;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
              Entry e;

              e = mX.getTimetable("CAL-1");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate1);

              e = mX.getTimetable("CAL-2");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate2);

              // "CAL-1" is invalidated in cache by 'getTimetable' before being
              // reloaded.

              e = mX.getTimetable("CAL-1");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate1);

              // "CAL-2" is invalidated in cache by 'lookupTimetable'.

              e = X.lookupTimetable("CAL-2");
                                         ASSERT(!e.get());

              e = mX.getTimetable("CAL-2");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate2);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            LOOP_ASSERT(da.numBlocksInUse(), 0 == da.numBlocksInUse());
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'invalidate' AND 'invalidateAll'
        //   Ensure that 'invalidate' and 'invalidateAll' behave as expected.
        //
        // Concerns:
        //: 1 That 'invalidate', when supplied with a string identifying a
        //:   timetable in the cache, invalidates the named timetable and
        //:   leaves other timetables in the cache (if any) unaffected.
        //:
        //: 2 That 'invalidate' has no effect if the string supplied to it does
        //:   not identify any timetable in the cache.
        //:
        //: 3 That 'invalidate' returns the correct status value.
        //:
        //: 4 That 'invalidate' can be called on an empty cache.
        //:
        //: 5 That timetables that have been invalidated by 'invalidate' can be
        //:   reloaded successfully.
        //:
        //: 6 That 'invalidate' is not affected by a timeout.
        //:
        //: 7 That 'invalidateAll' invalidates all timetables in the timetable.
        //:
        //: 8 That 'invalidateAll' returns the correct status value.
        //:
        //: 9 That timetables that have been invalidated by 'invalidateAll' can
        //:   be reloaded successfully.
        //:
        //:10 That 'invalidateAll' can be called on an empty cache.
        //:
        //:11 That 'invalidateAll' is not affected by a timeout.
        //:
        //:12 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 'invalidate' is tested ad hoc as follows: (C-1..6)
        //:
        //:   1 'invalidate' is called on an empty cache that has no timeout.
        //:     (C-4)
        //:
        //:   2 "CAL-1" is loaded into the cache, then 'invalidate' is called
        //:     with strings slightly different from "CAL-1".  (C-2)
        //:
        //:   3 'invalidate' is called with "CAL-1".
        //:
        //:   4 "CAL-1" and "CAL-2" are loaded into the cache (reloaded in the
        //:     case of "CAL-1") and each is invalidated in turn.  (C-1, C-3,
        //:     C-5)
        //:
        //:   5 P-1.1..4 is repeated on a cache that has a timeout.  (C-6)
        //:
        //: 2 'invalidateAll' is tested ad hoc as follows: (C-7..11)
        //:
        //:   1 'invalidateAll' is called on an empty cache that has no
        //:     timeout.  (C-10)
        //:
        //:   2 'invalidateAll' is successively called on the cache after it
        //:     has been loaded with one, two, and three timetables.  (C-7..9)
        //:
        //:   3 P-2.1..2 is repeated on a cache that has a timeout.  (C-11)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered (using the 'BSLS_ASSERTTEST_*' macros).  (C-12)
        //
        // Testing:
        //   int invalidate(const char *name);
        //   int invalidateAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'invalidate' AND 'invalidateAll'" << endl
                          << "================================" << endl;

        if (verbose) cout << "\nTesting 'invalidate' (w/o timeout)." << endl;
        {
            TestLoader loader;

            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&loader, &sa);  const Obj& X = mX;

            Entry e;
            int   rc;

            // Call 'invalidate' on an empty cache.

            rc = mX.invalidate("x");         ASSERT(0 == rc);

            // Load "CAL-1" into the cache.

            e  = mX.getTimetable("CAL-1");    ASSERT( e.get());

            rc = mX.invalidate("aCAL-1");    ASSERT(0 == rc);
            rc = mX.invalidate("CAL-1z");    ASSERT(0 == rc);

            e  = X.lookupTimetable("CAL-1");  ASSERT( e.get());

            rc = mX.invalidate("CAL-1");     ASSERT(1 == rc);

            e  = X.lookupTimetable("CAL-1");  ASSERT(!e.get());

            // Load "CAL-1" and "CAL-2" into the cache.

            e  = mX.getTimetable("CAL-1");    ASSERT( e.get());
            e  = mX.getTimetable("CAL-2");    ASSERT( e.get());

            rc = mX.invalidate("CAL-1");     ASSERT(1 == rc);

            e  = X.lookupTimetable("CAL-1");  ASSERT(!e.get());
            e  = X.lookupTimetable("CAL-2");  ASSERT( e.get());

            rc = mX.invalidate("CAL-2");     ASSERT(1 == rc);

            e  = X.lookupTimetable("CAL-1");  ASSERT(!e.get());
            e  = X.lookupTimetable("CAL-2");  ASSERT(!e.get());

            rc = mX.invalidate("CAL-2");     ASSERT(0 == rc);
        }

        if (verbose) cout << "\nTesting 'invalidate' (with timeout)." << endl;
        {
            TestLoader loader;

            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&loader, Interval(60, 0), &sa);  const Obj& X = mX;

            Entry e;
            int   rc;

            // Call 'invalidate' on an empty cache.

            rc = mX.invalidate("x");         ASSERT(0 == rc);

            // Load "CAL-1" into the cache.

            e  = mX.getTimetable("CAL-1");    ASSERT( e.get());

            rc = mX.invalidate("aCAL-1");    ASSERT(0 == rc);
            rc = mX.invalidate("CAL-1z");    ASSERT(0 == rc);

            e  = X.lookupTimetable("CAL-1");  ASSERT( e.get());

            rc = mX.invalidate("CAL-1");     ASSERT(1 == rc);

            e  = X.lookupTimetable("CAL-1");  ASSERT(!e.get());

            // Load "CAL-1" and "CAL-2" into the cache.

            e  = mX.getTimetable("CAL-1");    ASSERT( e.get());
            e  = mX.getTimetable("CAL-2");    ASSERT( e.get());

            rc = mX.invalidate("CAL-1");     ASSERT(1 == rc);

            e  = X.lookupTimetable("CAL-1");  ASSERT(!e.get());
            e  = X.lookupTimetable("CAL-2");  ASSERT( e.get());

            rc = mX.invalidate("CAL-2");     ASSERT(1 == rc);

            e  = X.lookupTimetable("CAL-1");  ASSERT(!e.get());
            e  = X.lookupTimetable("CAL-2");  ASSERT(!e.get());

            rc = mX.invalidate("CAL-2");     ASSERT(0 == rc);
        }

        if (verbose) cout << "\nTesting 'invalidateAll' (w/o timeout)."
                          << endl;
        {
            TestLoader loader;

            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&loader, &sa);  const Obj& X = mX;

            Entry e;
            int   rc;

            // Call 'invalidateAll' on an empty cache.

            rc = mX.invalidateAll();         ASSERT(0 == rc);

            // Load "CAL-1" into the cache.

            e  = mX.getTimetable("CAL-1");    ASSERT( e.get());

            rc = mX.invalidateAll();         ASSERT(1 == rc);

            e  = X.lookupTimetable("CAL-1");  ASSERT(!e.get());

            // Load "CAL-1" and "CAL-2" into the cache.

            e  = mX.getTimetable("CAL-1");    ASSERT( e.get());
            e  = mX.getTimetable("CAL-2");    ASSERT( e.get());

            rc = mX.invalidateAll();         ASSERT(2 == rc);

            e  = X.lookupTimetable("CAL-1");  ASSERT(!e.get());
            e  = X.lookupTimetable("CAL-2");  ASSERT(!e.get());

            // Load "CAL-1", "CAL-2", and "CAL-3" into the cache.

            e  = mX.getTimetable("CAL-1");    ASSERT( e.get());
            e  = mX.getTimetable("CAL-2");    ASSERT( e.get());
            e  = mX.getTimetable("CAL-3");    ASSERT( e.get());

            rc = mX.invalidateAll();         ASSERT(3 == rc);

            e  = X.lookupTimetable("CAL-1");  ASSERT(!e.get());
            e  = X.lookupTimetable("CAL-2");  ASSERT(!e.get());
            e  = X.lookupTimetable("CAL-3");  ASSERT(!e.get());
        }

        if (verbose) cout << "\nTesting 'invalidateAll' (with timeout)."
                          << endl;
        {
            TestLoader loader;

            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&loader, Interval(60, 0), &sa);  const Obj& X = mX;

            Entry e;
            int   rc;

            // Call 'invalidateAll' on an empty cache.

            rc = mX.invalidateAll();         ASSERT(0 == rc);

            // Load "CAL-1" into the cache.

            e  = mX.getTimetable("CAL-1");    ASSERT( e.get());

            rc = mX.invalidateAll();         ASSERT(1 == rc);

            e  = X.lookupTimetable("CAL-1");  ASSERT(!e.get());

            // Load "CAL-1" and "CAL-2" into the cache.

            e  = mX.getTimetable("CAL-1");    ASSERT( e.get());
            e  = mX.getTimetable("CAL-2");    ASSERT( e.get());

            rc = mX.invalidateAll();         ASSERT(2 == rc);

            e  = X.lookupTimetable("CAL-1");  ASSERT(!e.get());
            e  = X.lookupTimetable("CAL-2");  ASSERT(!e.get());

            // Load "CAL-1", "CAL-2", and "CAL-3" into the cache.

            e  = mX.getTimetable("CAL-1");    ASSERT( e.get());
            e  = mX.getTimetable("CAL-2");    ASSERT( e.get());
            e  = mX.getTimetable("CAL-3");    ASSERT( e.get());

            rc = mX.invalidateAll();         ASSERT(3 == rc);

            e  = X.lookupTimetable("CAL-1");  ASSERT(!e.get());
            e  = X.lookupTimetable("CAL-2");  ASSERT(!e.get());
            e  = X.lookupTimetable("CAL-3");  ASSERT(!e.get());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            TestLoader           loader;
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&loader, &sa);

            {
                ASSERT_SAFE_PASS(mX.invalidate(""));

                ASSERT_SAFE_FAIL(mX.invalidate(0));
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'getTimetable', 'lookupTimetable', AND 'lookupLoadTime'
        //   Ensure that 'getTimetable', 'lookupTimetable', and
        //   'lookupLoadTime' behave as expected in the absence of exceptions
        //   (exception neutrality is tested in a a later test case).
        //
        // Concerns:
        //: 1 That 'getTimetable', when supplied with a string not recognized
        //:   by the loader, returns null.
        //:
        //: 2 That 'getTimetable', when supplied with a string identifying a
        //:   timetable recognized by the loader that is NOT present in the
        //:   cache, loads the specified timetable into the cache and returns a
        //:   reference to that timetable.
        //:
        //: 3 That 'getTimetable', when supplied with a string identifying a
        //:   timetable recognized by the loader that IS present in the cache
        //:   and has NOT expired, returns a reference to that timetable.
        //:
        //: 4 That 'getTimetable', when supplied with a string identifying a
        //:   timetable recognized by the loader that IS present in the cache
        //:   but HAS expired, reloads the specified timetable into the cache
        //:   and returns a reference to that (newly-loaded) timetable.
        //:
        //: 5 That 'getTimetable' allocates memory if and only if it loads a
        //:   timetable into the cache.
        //:
        //: 6 That 'lookupTimetable', when supplied with a string not
        //:   identifying a timetable present in the cache, returns null.
        //:
        //: 7 That 'lookupTimetable', when supplied with a string identifying a
        //:   timetable present in the cache that has NOT expired, returns a
        //:   reference to that timetable.
        //:
        //: 8 That 'lookupTimetable', when supplied with a string identifying a
        //:   timetable present in the cache that HAS expired, returns null.
        //:
        //: 9 That 'lookupTimetable' allocates no memory from any allocator.
        //:
        //:10 That both 'getTimetable' and 'lookupTimetable' return pointers
        //:   providing non-modifiable access (only).
        //:
        //:11 That 'lookupLoadTime', when supplied with a string not
        //:   identifying a timetable present in the cache, returns
        //:   'Datetime()'.
        //:
        //:12 That 'lookupLoadTime', when supplied with a string identifying a
        //:   timetable present in the cache that has NOT expired, returns the
        //:   the time at which that timetable was loaded.
        //:
        //:13 That 'lookupLoadTime', when supplied with a string identifying a
        //:   timetable present in the cache that HAS expired, returns
        //:   'Datetime()'.
        //:
        //:14 That 'lookupLoadTime' allocates no memory from any allocator.
        //:
        //:15 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 'getTimetable', 'lookupTimetable', and 'lookupLoadTime' are
        //:   tested ad hoc, using a cache with no timeout, as follows:
        //:
        //:   1 'getTimetable', 'lookupTimetable', and 'lookupLoadTime' are
        //:     called with various strings on the initially empty cache.
        //:     (C-1, C-6, C-11)
        //:
        //:   2 Using 'getTimetable', the "CAL-1" timetable is loaded into the
        //:     cache, then 'getTimetable', 'lookupTimetable', and
        //:     'lookupLoadTime' are called with various strings.
        //:
        //:   3 A second timetable, "CAL-2", is loaded into the cache, and
        //:     'getTimetable', 'lookupTimetable', and 'lookupLoadTime' are
        //:     again called with various strings.  (C-2)
        //:
        //:   4 Verify that 'getTimetable' and 'lookupTimetable' return
        //:     pointers providing non-modifiable access (only).  (C-10)
        //:
        //: 2 'getTimetable', 'lookupTimetable', and 'lookupLoadTime' are
        //:    further tested ad hoc, using a cache with a 1-second timeout,
        //:    as follows:
        //:
        //:   1 Timetables "CAL-1" and "CAL-2" are loaded into the cache, then
        //:     'getTimetable', 'lookupTimetable', and 'lookupLoadTime' are
        //:     used to verify that the timetables have not yet expired.  (C-3,
        //:     C-7, C-12).
        //:
        //:   2 Sleep for 2 seconds.
        //:
        //:   3 'lookupTimetable' and 'lookupLoadTime' are used to verify that
        //:     "CAL-1" has expired (null/'Datetime()' is returned);
        //:     'getTimetable' is used to verify that "CAL-2" has expired
        //:     ("CAL-2" is reloaded into the cache).  (C-4, C-5, C-8, C-9,
        //:     C-13, C-14).
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered (using the 'BSLS_ASSERTTEST_*' macros).  (C-15)
        //
        // Testing:
        //   shared_ptr<const Timetable> getTimetable(const char *name);
        //   shared_ptr<const Timetable> lookupTimetable(const char *) const;
        //   Datetime lookupLoadTime(const char *name) const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "'getTimetable', 'lookupTimetable', AND 'lookupLoadTime'"
                 << endl
                 << "====================================================="
                 << endl;
        }

        if (verbose) cout << "\nTesting without a timeout." << endl;
        {
            TestLoader loader;

            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&loader, &sa);  const Obj& X = mX;

            // Empty cache.
            {
                Entry    e;
                Datetime d;

                e = mX.getTimetable("ERROR");      ASSERT(!e.get());

                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());

                e = mX.getTimetable("CAL-Z");      ASSERT(!e.get());

                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());

                e = X.lookupTimetable("CAL-Z");    ASSERT(!e.get());

                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());

                d = X.lookupLoadTime("CAL-Z");    ASSERT(d == Datetime());

                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());
            }

            bsls::Types::Int64 daLastNumBlocksTotal, saLastNumBlocksTotal;

            // Cache with one entry.
            {
                Entry    e1;
                Entry    ex;
                Datetime dn = bdlt::CurrentTime::utc();
                Datetime dx;

                e1 = mX.getTimetable("CAL-1");     ASSERT(e1.get());
                                                  ASSERT(e1->firstDate() ==
                                                                  gFirstDate1);

                ASSERT(true == providesNonModifiableAccessOnly(e1.get()));

                LOOP_ASSERT(da.numBlocksInUse(), 0 == da.numBlocksInUse());
                LOOP_ASSERT(sa.numBlocksInUse(), 0 <  sa.numBlocksInUse());

                daLastNumBlocksTotal = da.numBlocksTotal();
                saLastNumBlocksTotal = sa.numBlocksTotal();

                ex = X.lookupTimetable("CAL-1");   ASSERT(ex.get());
                                                  ASSERT(ex.get() == e1.get());

                ASSERT(true == providesNonModifiableAccessOnly(ex.get()));

                LOOP2_ASSERT(daLastNumBlocksTotal, da.numBlocksTotal(),
                             daLastNumBlocksTotal == da.numBlocksTotal());
                LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                             saLastNumBlocksTotal == sa.numBlocksTotal());

                dx = X.lookupLoadTime("CAL-1");
                ASSERT(dx - dn < OneSecond);

                LOOP2_ASSERT(daLastNumBlocksTotal, da.numBlocksTotal(),
                             daLastNumBlocksTotal == da.numBlocksTotal());
                LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                             saLastNumBlocksTotal == sa.numBlocksTotal());

                ex = mX.getTimetable("CAL-1");     ASSERT(ex.get());
                                                  ASSERT(ex.get() == e1.get());

                LOOP2_ASSERT(daLastNumBlocksTotal, da.numBlocksTotal(),
                             daLastNumBlocksTotal == da.numBlocksTotal());
                LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                             saLastNumBlocksTotal == sa.numBlocksTotal());

                ex = mX.getTimetable("aCAL-1");    ASSERT(!ex.get());

                ex = mX.getTimetable("CAL-1z");    ASSERT(!ex.get());

                ex = X.lookupTimetable("aCAL-1");  ASSERT(!ex.get());

                dx = X.lookupLoadTime("aCAL-1");  ASSERT(dx == Datetime());

                ex = X.lookupTimetable("CAL-1z");  ASSERT(!ex.get());

                dx = X.lookupLoadTime("CAL-1z");  ASSERT(dx == Datetime());

                LOOP2_ASSERT(daLastNumBlocksTotal, da.numBlocksTotal(),
                             daLastNumBlocksTotal == da.numBlocksTotal());
                LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                             saLastNumBlocksTotal == sa.numBlocksTotal());
            }

            // Cache with two entries.
            {
                Entry    e1;
                Entry    e2;
                Entry    ex;
                Entry    ey;
                Datetime dn = bdlt::CurrentTime::utc();
                Datetime dx;
                Datetime dy;

                const bsls::Types::Int64 lastNumBlocksInUse =
                                                           sa.numBlocksInUse();

                e2 = mX.getTimetable("CAL-2");     ASSERT(e2.get());
                                                  ASSERT(e2->firstDate() ==
                                                                  gFirstDate2);

                LOOP_ASSERT(da.numBlocksInUse(), 0 == da.numBlocksInUse());
                LOOP2_ASSERT(lastNumBlocksInUse, sa.numBlocksInUse(),
                             lastNumBlocksInUse < sa.numBlocksInUse());

                daLastNumBlocksTotal = da.numBlocksTotal();
                saLastNumBlocksTotal = sa.numBlocksTotal();

                ey = X.lookupTimetable("CAL-2");   ASSERT(ey.get());
                                                  ASSERT(ey.get() == e2.get());

                dy = X.lookupLoadTime("CAL-2");
                ASSERT(dy - dn < OneSecond);

                e1 = mX.getTimetable("CAL-1");     ASSERT(e1.get());
                                                  ASSERT(e1->firstDate() ==
                                                                  gFirstDate1);

                ex = X.lookupTimetable("CAL-1");   ASSERT(ex.get());
                                                  ASSERT(ex.get() == e1.get());

                dx = X.lookupLoadTime("CAL-1");
                ASSERT(dx - dn < OneSecond);

                ex = X.lookupTimetable("CAL-Z");   ASSERT(!ex.get());

                dx = X.lookupLoadTime("CAL-Z");   ASSERT(dx == Datetime());

                LOOP2_ASSERT(daLastNumBlocksTotal, da.numBlocksTotal(),
                             daLastNumBlocksTotal == da.numBlocksTotal());
                LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                             saLastNumBlocksTotal == sa.numBlocksTotal());
            }
        }

        if (verbose) cout << "\nTesting with a timeout." << endl;
        {
            TestLoader loader;

            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&loader, Interval(1, 0), &sa);  const Obj& X = mX;

            Entry    e1;
            Entry    e2;
            Entry    eo;
            Datetime dn = bdlt::CurrentTime::utc();
            Datetime d0;

            e1 = mX.getTimetable("CAL-1");    ASSERT( e1.get());
            e2 = mX.getTimetable("CAL-2");    ASSERT( e2.get());

            eo = X.lookupTimetable("CAL-1");  ASSERT( eo.get());
                                             ASSERT( eo.get() == e1.get());

            d0 = X.lookupLoadTime("CAL-1");  ASSERT(d0 - dn < OneSecond);

            eo = X.lookupTimetable("CAL-2");  ASSERT( eo.get());
                                             ASSERT( eo.get() == e2.get());

            d0 = X.lookupLoadTime("CAL-2");  ASSERT(d0 - dn < OneSecond);

            eo = mX.getTimetable("CAL-1");    ASSERT( eo.get());
                                             ASSERT( eo.get() == e1.get());

            eo = mX.getTimetable("CAL-2");    ASSERT( eo.get());
                                             ASSERT( eo.get() == e2.get());

            const bsls::Types::Int64 saLastNumBlocksTotal =
                                                           sa.numBlocksTotal();

            sleepSeconds(2);

            eo = X.lookupTimetable("CAL-1");  ASSERT(!eo.get());

            d0 = X.lookupLoadTime("CAL-1");  ASSERT(d0 == Datetime());

            eo = mX.getTimetable("CAL-2");    ASSERT( eo.get());
                                             ASSERT( eo.get() != e2.get());

            LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                         saLastNumBlocksTotal <  sa.numBlocksTotal());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            TestLoader loader;

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            {
                Obj mX(&loader, &sa);

                ASSERT_SAFE_PASS(mX.getTimetable(""));

                ASSERT_SAFE_FAIL(mX.getTimetable(0));
            }

            {
                const Obj X(&loader, &sa);

                ASSERT_SAFE_PASS(X.lookupTimetable(""));

                ASSERT_SAFE_FAIL(X.lookupTimetable(0));
            }

            {
                const Obj X(&loader, &sa);

                ASSERT_SAFE_PASS(X.lookupLoadTime(""));

                ASSERT_SAFE_FAIL(X.lookupLoadTime(0));
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CTORS AND DTOR
        //   Ensure that the constructors configure the cache with the expected
        //   loader, timeout, and allocator; and that the destructor releases
        //   all memory.
        //
        // Concerns:
        //: 1 The cache uses the loader supplied at construction.
        //:
        //: 2 If a timeout is NOT supplied at construction, timetables do not
        //:   expire from the cache.
        //:
        //: 3 If a timeout IS supplied at construction, timetables expire from
        //:   the cache per the specified timeout.
        //:
        //: 4 If an allocator is NOT supplied at construction, the default
        //:   allocator in effect at the time of construction becomes the
        //:   object allocator for the resulting cache.
        //:
        //: 5 If an allocator IS supplied at construction, that allocator
        //:   becomes the object allocator for the resulting cache.
        //:
        //: 6 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 7 No memory is allocated from any allocator during construction.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using a loop-based approach, construct three distinct objects
        //:   without a timeout, in turn, but configured differently: (a)
        //:   without passing an allocator, (b) passing a null allocator
        //:   address explicitly, and (c) passing the address of a test
        //:   allocator distinct from the default.  For each of these three
        //:   iterations: (C-1..2, C-4..8)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the two-argument constructor to dynamically create an
        //:     object, 'mX', with its object allocator configured
        //:     appropriately (see P-1); use a distinct test allocator for the
        //:     object's footprint.
        //:
        //:   3 Use the appropriate test allocators to verify that no memory
        //:     is allocated by the constructor.  (C-7)
        //:
        //:   4 Use the (as yet unproven) 'getTimetable' manipulator to load a
        //:     a timetable into 'mX'; verify that the timetable loader
        //:     supplied at construction is used and that use of the object
        //:     and default allocators are as expected.  (C-1, C4..6)
        //:
        //:   5 Use the (as yet unproven) 'lookupTimetable' accessor to verify
        //:     that a timeout is not in effect (see the TEST PLAN regarding
        //:     timeout testing).  (C-2)
        //:
        //:   6 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //:
        //: 2 Repeat P-1, but this time using the three-argument constructor
        //:   to supply a timeout (P-1.2), and using 'getTimetable' instead of
        //:   'lookupTimetable' to reload the timetable (P-1.5).  (C-1, C-3..8)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered (using the 'BSLS_ASSERTTEST_*' macros).  (C-9)
        //
        // Testing:
        //   TimetableCache(Loader *loader,          Allocator *ba = 0);
        //   TimetableCache(Loader *loader, timeout, Allocator *ba = 0);
        //   ~TimetableCache();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CTORS AND DTOR" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nTesting a cache with no timeout." << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            if (veryVerbose) { T_  P(CONFIG) }

            TestLoader loader;

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj(&loader);
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) Obj(&loader, 0);
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(&loader, &sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                LOOP_ASSERT(CONFIG, !"Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;

            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify no allocation from the object/non-object allocators.

            LOOP2_ASSERT(CONFIG,  oa.numBlocksTotal(),
                         0 ==  oa.numBlocksTotal());
            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            {
                Entry e1 = mX.getTimetable("CAL-1");

                LOOP_ASSERT(CONFIG, e1.get());
                LOOP_ASSERT(CONFIG, e1->firstDate() == gFirstDate1);

                // Check that some object memory is now in use.

                LOOP_ASSERT(CONFIG, 1 <= oa.numBlocksInUse());

                if ('c' == CONFIG) {
                    LOOP_ASSERT(CONFIG, 0 == da.numBlocksInUse());
                }

                const bsls::Types::Int64 lastNumBlocksInUse =
                                                           oa.numBlocksInUse();

                Entry e2 = X.lookupTimetable("CAL-1");

                LOOP_ASSERT(CONFIG, e2.get());
                LOOP_ASSERT(CONFIG, e2.get() == e1.get());
                LOOP_ASSERT(CONFIG, e2->firstDate() == gFirstDate1);

                // Check that no additional object memory is in use.

                LOOP_ASSERT(CONFIG, lastNumBlocksInUse == oa.numBlocksInUse());

                if ('c' == CONFIG) {
                    LOOP_ASSERT(CONFIG, 0 == da.numBlocksInUse());
                }
            }
            LOOP_ASSERT(CONFIG, 1 <= oa.numBlocksInUse());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            LOOP_ASSERT( fa.numBlocksInUse(), 0 ==  fa.numBlocksInUse());
            LOOP_ASSERT( oa.numBlocksInUse(), 0 ==  oa.numBlocksInUse());
            LOOP_ASSERT(noa.numBlocksInUse(), 0 == noa.numBlocksInUse());
        }

        if (verbose) cout << "\nTesting a cache with a timeout." << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            if (veryVerbose) { T_  P(CONFIG) }

            TestLoader loader;

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj(&loader, Interval(0, 0));
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) Obj(&loader, Interval(0, 0), 0);
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(&loader, Interval(0, 0), &sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                LOOP_ASSERT(CONFIG, !"Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify no allocation from the object/non-object allocators.

            LOOP2_ASSERT(CONFIG,  oa.numBlocksTotal(),
                         0 ==  oa.numBlocksTotal());
            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            {
                Entry e1 = mX.getTimetable("CAL-1");

                LOOP_ASSERT(CONFIG, e1.get());
                LOOP_ASSERT(CONFIG, e1->firstDate() == gFirstDate1);

                // Check that some object memory is now in use.

                LOOP_ASSERT(CONFIG, 1 <= oa.numBlocksInUse());

                if ('c' == CONFIG) {
                    LOOP_ASSERT(CONFIG, 0 == da.numBlocksInUse());
                }

                const bsls::Types::Int64 lastNumBlocksInUse =
                                                           oa.numBlocksInUse();

                Entry e2 = mX.getTimetable("CAL-1");

                LOOP_ASSERT(CONFIG, e2.get());
                LOOP_ASSERT(CONFIG, e2.get() != e1.get());
                LOOP_ASSERT(CONFIG, e2->firstDate() == gFirstDate1);

                // Check that additional object memory is in use.

                LOOP_ASSERT(CONFIG, lastNumBlocksInUse < oa.numBlocksInUse());

                if ('c' == CONFIG) {
                    LOOP_ASSERT(CONFIG, 0 == da.numBlocksInUse());
                }
            }
            LOOP_ASSERT(CONFIG, 1 <= oa.numBlocksInUse());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            LOOP_ASSERT( fa.numBlocksInUse(), 0 ==  fa.numBlocksInUse());
            LOOP_ASSERT( oa.numBlocksInUse(), 0 ==  oa.numBlocksInUse());
            LOOP_ASSERT(noa.numBlocksInUse(), 0 == noa.numBlocksInUse());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            TestLoader loader;

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            {
                ASSERT_SAFE_PASS(Obj(&loader, &sa));

                ASSERT_SAFE_FAIL(Obj(      0, &sa));
            }

            {
                ASSERT_SAFE_PASS(Obj(&loader, Interval(1,       0), &sa));
                ASSERT_SAFE_FAIL(Obj(      0, Interval(1,       0), &sa));

                ASSERT_SAFE_PASS(Obj(&loader, Interval(0,       0), &sa));
                ASSERT_SAFE_FAIL(Obj(&loader, Interval(0,      -1), &sa));

                ASSERT_SAFE_PASS(Obj(&loader, Interval(INT_MAX, 0), &sa));
                ASSERT_SAFE_FAIL(Obj(&loader, Interval(INT_MAX, 1), &sa));
            }
        }

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
        //: 1 Create a timetable cache 'mX'.
        //:
        //: 2 Load "CAL-1" into 'mX' with 'Entry' 'e1' referring to "CAL-1".
        //:
        //: 3 Create 'Entry' 'e2' as a copy of 'e1'.
        //:
        //: 4 Let 'e2' go out of scope.
        //:
        //: 5 Let 'e1' go out of scope.
        //:
        //: 6 Look up "CAL-1" in the cache.
        //:
        //: 7 Invalidate "CAL-1" in the cache.
        //:
        //: 8 Verify "CAL-1" is no longer present in the cache.
        //:
        //: 9 Reload "CAL-1" into the cache.
        //:
        //:10 Let 'mX' go out of scope.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        TestLoader loader;

        {
            bdlt::Timetable mX;  const bdlt::Timetable& X = mX;

            ASSERT(false == providesNonModifiableAccessOnly(&mX));
            ASSERT(true  == providesNonModifiableAccessOnly(&X));
        }

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        bsls::Types::Int64 daLastNumBlocksTotal;
        bsls::Types::Int64 saLastNumBlocksTotal, saLastNumBlocksInUse;

        {
            // 1 Create a timetable cache 'mX'.

            Obj mX(&loader, &sa);  const Obj& X = mX;

            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
            LOOP_ASSERT(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());

            {
                // 2 Load "CAL-1" into 'mX' with 'Entry' 'e1' referring to
                //   "CAL-1".

                Entry e1 = mX.getTimetable("CAL-1");

                ASSERT(e1.get());
                ASSERT(e1->firstDate() == gFirstDate1);

                LOOP_ASSERT(da.numBlocksInUse(), 0 == da.numBlocksInUse());
                LOOP_ASSERT(sa.numBlocksInUse(), 0 <  sa.numBlocksInUse());

                daLastNumBlocksTotal = da.numBlocksTotal();
                saLastNumBlocksTotal = sa.numBlocksTotal();
                saLastNumBlocksInUse = sa.numBlocksInUse();

                {
                    // 3 Create 'Entry' 'e2' as a copy of 'e1'.

                    Entry e2(e1);

                    ASSERT(e1.get()        == e2.get());
                    ASSERT(e2->firstDate() == gFirstDate1);

                    LOOP2_ASSERT(daLastNumBlocksTotal, da.numBlocksTotal(),
                                 daLastNumBlocksTotal == da.numBlocksTotal());
                    LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                                 saLastNumBlocksTotal == sa.numBlocksTotal());
                }
                // 4 Let 'e2' go out of scope.

                ASSERT(e1->firstDate() == gFirstDate1);

                LOOP2_ASSERT(daLastNumBlocksTotal, da.numBlocksTotal(),
                             daLastNumBlocksTotal == da.numBlocksTotal());
                LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                             saLastNumBlocksTotal == sa.numBlocksTotal());
                LOOP2_ASSERT(saLastNumBlocksInUse, sa.numBlocksInUse(),
                             saLastNumBlocksInUse == sa.numBlocksInUse());
            }
            // 5 Let 'e1' go out of scope.

            LOOP2_ASSERT(daLastNumBlocksTotal, da.numBlocksTotal(),
                         daLastNumBlocksTotal == da.numBlocksTotal());
            LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                         saLastNumBlocksTotal == sa.numBlocksTotal());
            LOOP2_ASSERT(saLastNumBlocksInUse, sa.numBlocksInUse(),
                         saLastNumBlocksInUse == sa.numBlocksInUse());

            {
                // 6 Look up "CAL-1" in the cache.

                Entry e = X.lookupTimetable("CAL-1");

                ASSERT(e.get());
                ASSERT(e->firstDate() == gFirstDate1);

                LOOP2_ASSERT(daLastNumBlocksTotal, da.numBlocksTotal(),
                             daLastNumBlocksTotal == da.numBlocksTotal());
                LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                             saLastNumBlocksTotal == sa.numBlocksTotal());
            }

            // 7 Invalidate "CAL-1" in the cache.

            int rc = mX.invalidate("CAL-1");

            ASSERT(1 == rc);

            LOOP2_ASSERT(daLastNumBlocksTotal, da.numBlocksTotal(),
                         daLastNumBlocksTotal == da.numBlocksTotal());
            LOOP2_ASSERT(saLastNumBlocksInUse, sa.numBlocksInUse(),
                         saLastNumBlocksInUse >  sa.numBlocksInUse());

            saLastNumBlocksInUse = sa.numBlocksInUse();

            // 8 Verify "CAL-1" is no longer present in the cache.

            Entry e = X.lookupTimetable("CAL-1");

            ASSERT(!e.get());

            LOOP2_ASSERT(daLastNumBlocksTotal, da.numBlocksTotal(),
                         daLastNumBlocksTotal == da.numBlocksTotal());
            LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                         saLastNumBlocksTotal == sa.numBlocksTotal());

            // 9 Reload "CAL-1" into the cache.

            e = mX.getTimetable("CAL-1");

            ASSERT(e.get());
            ASSERT(e->firstDate() == gFirstDate1);

            LOOP_ASSERT(da.numBlocksInUse(), 0 == da.numBlocksInUse());
            LOOP2_ASSERT(saLastNumBlocksInUse, sa.numBlocksInUse(),
                         saLastNumBlocksInUse < sa.numBlocksInUse());
        }
        // 10 Let 'mX' go out of scope.

        LOOP_ASSERT(da.numBlocksInUse(), 0 == da.numBlocksInUse());
        LOOP_ASSERT(sa.numBlocksInUse(), 0 == sa.numBlocksInUse());

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TIMEOUT GREATER THAN 20 SECONDS
        //   Ensure that a non-trivial timeout is processed correctly.
        //
        // Concerns:
        //: 1 A cache having a timeout greater than 20 seconds correctly
        //:   expires its entries.
        //
        // Plan:
        //: 1 Create a cache with a timeout of greater than 20 seconds.
        //:
        //: 2 Use the 'getTimetable' method to load a timetable into the cache.
        //:
        //: 3 Wait a period significantly less than the timeout and use the
        //:   'lookupTimetable' method to retrieve the timetable loaded in P-1.
        //:   Verify that the timetable was successfully retrieved.
        //:
        //: 4 Wait another period so that the cumulative waiting time for P-2
        //:   and P-3 is greater than the timeout of the cache.  Again use the
        //:   'lookupTimetable' method to retrieve the timetable loaded in P-1,
        //:   and verify that the timetable is no longer in the cache.
        //:
        //: 5 Repeat P-1..4, but this time use the 'getTimetable' method to
        //:   observe the effects of a timeout.  (C-1)
        //
        // Testing:
        //   CONCERN: A non-trivial timeout is processed correctly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TIMEOUT GREATER THAN 20 SECONDS" << endl
                          << "===============================" << endl;

        TestLoader loader;

        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        Obj mX(&loader, Interval(30, 0), &sa);  const Obj& X = mX;

        // Observe effects of timeout via 'lookupTimetable' method.
        {
            Entry e;

            e = mX.getTimetable("CAL-1");         ASSERT( e.get());

            sleepSeconds( 5);

            e = X.lookupTimetable("CAL-1");       ASSERT( e.get());

            sleepSeconds(30);

            e = X.lookupTimetable("CAL-1");       ASSERT(!e.get());
        }

        // Observe effects of timeout via 'getTimetable' method.
        {
            Entry e1 = mX.getTimetable("CAL-1");  ASSERT(e1.get());

            const bsls::Types::Int64 saLastNumBlocksTotal =
                                                           sa.numBlocksTotal();

            sleepSeconds( 5);

            Entry e2 = mX.getTimetable("CAL-1");  ASSERT(e2.get());
                                                 ASSERT(e2.get() == e1.get());

            LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                         saLastNumBlocksTotal == sa.numBlocksTotal());

            sleepSeconds(30);

            Entry e3 = mX.getTimetable("CAL-1");  ASSERT(e3.get());
                                                 ASSERT(e3.get() != e2.get());

            LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                         saLastNumBlocksTotal < sa.numBlocksTotal());
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
