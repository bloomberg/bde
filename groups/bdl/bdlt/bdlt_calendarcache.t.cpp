// bdlt_calendarcache.t.cpp                                           -*-C++-*-
#include <bdlt_calendarcache.h>

#include <bdlt_calendarloader.h>
#include <bdlt_date.h>            // for testing only
#include <bdlt_packedcalendar.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
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
// We are testing a thread-safe mechanism, 'bdlt::CalendarCache', that is a
// cache of 'bdlt::Calendar' objects.  We make extensive use of a concrete
// instance of 'bdlt::CalendarLoader', 'TestLoader', that has been crafted for
// our purposes, and the 'bslma' test allocator.
//
// Two issues of note are taken into account in testing this component.  One is
// that 'bsl::shared_ptr' objects can take ownership of calendars that have
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
// 'CalendarCache' class:
// [ 2] CalendarCache(Loader *loader,          Allocator *ba = 0);
// [ 2] CalendarCache(Loader *loader, timeout, Allocator *ba = 0);
// [ 2] ~CalendarCache();
// [ 3] shared_ptr<const Calendar> getCalendar(const char *name);
// [ 4] int invalidate(const char *name);
// [ 4] int invalidateAll();
// [ 3] shared_ptr<const Calendar> lookupCalendar(const char *name) const;
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

typedef bdlt::CalendarCache                   Obj;
typedef bsl::shared_ptr<const bdlt::Calendar> Entry;

typedef bsls::TimeInterval                    Interval;

#ifdef BSLS_PLATFORM_OS_WINDOWS
typedef HANDLE    ThreadId;
#else
typedef pthread_t ThreadId;
#endif

typedef void *(*ThreadFunction)(void *arg);

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT((bslalg::HasTrait<Obj,
                               bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

// ============================================================================
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static const bdlt::Date gFirstDate1(2000, 1,  1);  // "CAL-1" calendar
static const bdlt::Date gFirstDate2(2005, 5, 15);  // "CAL-2"    "
static const bdlt::Date gFirstDate3(2010, 9, 30);  // "CAL-3"    "

static const bdlt::Date gLastDate(2020, 12, 31);

static const int        gHolidayOffset = 38;

class TestLoader : public bdlt::CalendarLoader {
    // This concrete calendar loader successfully loads calendars named
    // "CAL-1", "CAL-2", and "CAL-3" only.  An attempt to load a calendar by
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
    int load(bdlt::PackedCalendar *result, const char *calendarName);
        // Load, into the specified 'result', the calendar identified by the
        // specified 'calendarName'.  Return 0 on success, and a non-zero value
        // otherwise.
};

// CREATORS
inline
TestLoader::TestLoader()
{
}

TestLoader::~TestLoader()
{
}

int TestLoader::load(bdlt::PackedCalendar *result, const char *calendarName)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(calendarName);

    if (0 == bsl::strcmp("CAL-1", calendarName)
     || 0 == bsl::strcmp("CAL-2", calendarName)
     || 0 == bsl::strcmp("CAL-3", calendarName)) {

        // Three calendars are recognized by this loader.

        bdlt::Date firstDate;

        switch (calendarName[4]) {
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

        result->removeAll();
        result->setValidRange(firstDate, gLastDate);

        // The following is required to trigger an allocation in 'result'.

        result->addHoliday(firstDate + gHolidayOffset);

        return 0;                                                     // RETURN
    }

    if (0 == bsl::strcmp("ERROR", calendarName)) {
        return -1;                                                    // RETURN
    }

    return 1;
}

static
bool providesNonModifiableAccessOnly(bdlt::Calendar *)
    // Return 'false'.
{
    return false;
}

static
bool providesNonModifiableAccessOnly(const bdlt::Calendar *)
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
            Entry e = mX.getCalendar("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = Y.lookupCalendar("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = mX.getCalendar("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = Y.lookupCalendar("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = mX.getCalendar("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = Y.lookupCalendar("CAL-3");
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
            Entry e = mY.getCalendar("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = X.lookupCalendar("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = mY.getCalendar("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = X.lookupCalendar("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = mY.getCalendar("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = X.lookupCalendar("CAL-3");
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
            Entry e = mX.getCalendar("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = Y.lookupCalendar("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = mX.getCalendar("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = Y.lookupCalendar("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = mX.getCalendar("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = Y.lookupCalendar("CAL-1");
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
            Entry e = mY.getCalendar("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = X.lookupCalendar("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = mY.getCalendar("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = X.lookupCalendar("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = mY.getCalendar("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = X.lookupCalendar("CAL-1");
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
            Entry e = mX.getCalendar("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = Y.lookupCalendar("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = mX.getCalendar("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = Y.lookupCalendar("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = mX.getCalendar("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = Y.lookupCalendar("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        if (0 == i % 7) {
            mX.invalidate("CAL-3");
        }

        {
            Entry e = mY.getCalendar("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = X.lookupCalendar("CAL-3");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate3);
            }
        }

        {
            Entry e = mY.getCalendar("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = X.lookupCalendar("CAL-1");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate1);
            }
        }

        {
            Entry e = mY.getCalendar("CAL-2");
            if (e.get()) {
                ASSERT(e->firstDate() == gFirstDate2);
            }
        }

        {
            Entry e = X.lookupCalendar("CAL-2");
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

// Define the 'MyCalendarLoader' class that is used in the Usage example.

class MyCalendarLoader : public bdlt::CalendarLoader {

  private:
    // NOT IMPLEMENTED
    MyCalendarLoader(const MyCalendarLoader&);             // = delete
    MyCalendarLoader& operator=(const MyCalendarLoader&);  // = delete

  public:
    // CREATORS
    MyCalendarLoader();
        // Create a 'MyCalendarLoader' object.

    ~MyCalendarLoader();
        // Destroy this object.

    // MANIPULATORS
    int load(bdlt::PackedCalendar *result, const char *calendarName);
        // Load, into the specified 'result', the calendar identified by the
        // specified 'calendarName'.  Return 0 on success, and a non-zero value
        // otherwise.
};

// CREATORS
inline
MyCalendarLoader::MyCalendarLoader()
{
}

MyCalendarLoader::~MyCalendarLoader()
{
}

int MyCalendarLoader::load(bdlt::PackedCalendar *result,
                           const char           *calendarName)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(calendarName);

    result->removeAll();
    result->setValidRange(bdlt::Date(2000, 1, 1), bdlt::Date(2020, 12, 31));

    if (     0 == bsl::strcmp("DE", calendarName)) {  // Germany
        return 0;                                                     // RETURN
    }
    else if (0 == bsl::strcmp("FR", calendarName)) {  // France
        result->addHoliday(bdlt::Date(2011, 7, 14));

        return 0;                                                     // RETURN
    }
    else if (0 == bsl::strcmp("US", calendarName)) {  // USA
        result->addHoliday(bdlt::Date(2011, 7,  4));

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

        if (verbose) cout << "Example 1: Using a 'bdlt::CalendarCache'"
                          << endl;
        {
///Usage
///-----
// The following example illustrates how to use a 'bdlt::CalendarCache'.
//
///Example 1: Using a 'bdlt::CalendarCache'
/// - - - - - - - - - - - - - - - - - - - -
// This example shows basic use of a 'bdlt::CalendarCache' object.
//
// In this example, we assume a hypothetical calendar loader,
// 'MyCalendarLoader', the details of which are not important other than that
// it supports calendars identified by "DE", "FR", and "US", which nominally
// identify the major holidays in Germany, France, and the United States,
// respectively.  Furthermore, we cite two specific dates of interest:
// 2011/07/04, which was a holiday in the US (Independence Day), but not in
// France, and 2011/07/14, which was a holiday in France (Bastille Day), but
// not in the US.  Note that neither of these dates were holidays in Germany.
//
// First, we create a calendar loader, an instance of 'MyCalendarLoader', and
// use it, in turn, to create a cache.  For the purposes of this example, it is
// sufficient to let the cache use the default allocator:
//..
    MyCalendarLoader    loader;
    bdlt::CalendarCache cache(&loader);
//..
// Next, we retrieve the calendar 'usA', identified by "US", verify that the
// loading of that calendar into the cache was successful ('usA.get()' is
// non-null), and verify that 2011/07/04 is recognized as a holiday in the "US"
// calendar, whereas 2011/07/14 is not:
//..
    bsl::shared_ptr<const bdlt::Calendar> usA = cache.getCalendar("US");

                              ASSERT( usA.get());
                              ASSERT( usA->isHoliday(bdlt::Date(2011, 7,  4)));
                              ASSERT(!usA->isHoliday(bdlt::Date(2011, 7, 14)));
//..
// Then, we fetch the calendar identified by "FR", this time verifying that
// 2011/07/14 is recognized as a holiday in the "FR" calendar, but 2011/07/04
// is not:
//..
    bsl::shared_ptr<const bdlt::Calendar> frA = cache.getCalendar("FR");

                              ASSERT( frA.get());
                              ASSERT(!frA->isHoliday(bdlt::Date(2011, 7,  4)));
                              ASSERT( frA->isHoliday(bdlt::Date(2011, 7, 14)));
//..
// Next, we retrieve the "FR" calendar again, this time via the
// 'lookupCalendar' accessor, and note that the request is satisfied by the
// calendar that is already in the cache:
//..
    const bdlt::CalendarCache& readonlyCache = cache;

    bsl::shared_ptr<const bdlt::Calendar> frB =
                                            readonlyCache.lookupCalendar("FR");

                              ASSERT( frA.get() == frB.get());
//..
// Then, we invalidate the "US" calendar in the cache and immediately fetch it
// again.  The call to 'invalidate' removed the "US" calendar from the cache,
// so it had to be reloaded into the cache to satisfy the request:
//..
    int numInvalidated = cache.invalidate("US");
                              ASSERT(1 == numInvalidated);

    bsl::shared_ptr<const bdlt::Calendar> usB = cache.getCalendar("US");

                              ASSERT( usB.get() != usA.get());
                              ASSERT( usB.get());
                              ASSERT( usB->isHoliday(bdlt::Date(2011, 7,  4)));
                              ASSERT(!usB->isHoliday(bdlt::Date(2011, 7, 14)));
//..
// Next, all calendars in the cache are invalidated, then reloaded:
//..
    numInvalidated = cache.invalidateAll();
                              ASSERT(2 == numInvalidated);

    bsl::shared_ptr<const bdlt::Calendar> usC = cache.getCalendar("US");

                              ASSERT( usC.get() != usA.get());
                              ASSERT( usC.get() != usB.get());
                              ASSERT( usC.get());
                              ASSERT( usC->isHoliday(bdlt::Date(2011, 7,  4)));
                              ASSERT(!usC->isHoliday(bdlt::Date(2011, 7, 14)));

    bsl::shared_ptr<const bdlt::Calendar> frC = cache.getCalendar("FR");

                              ASSERT( frC.get() != frA.get());
                              ASSERT( frC.get() != frB.get());
                              ASSERT( frC.get());
                              ASSERT(!frC->isHoliday(bdlt::Date(2011, 7,  4)));
                              ASSERT( frC->isHoliday(bdlt::Date(2011, 7, 14)));
//..
// Now, verify that references to calendars that were invalidated in the cache
// are still valid for clients that obtained references to them before they
// were made invalid:
//..
                              ASSERT( usA->isHoliday(bdlt::Date(2011, 7,  4)));
                              ASSERT(!usA->isHoliday(bdlt::Date(2011, 7, 14)));

                              ASSERT( usB->isHoliday(bdlt::Date(2011, 7,  4)));
                              ASSERT(!usB->isHoliday(bdlt::Date(2011, 7, 14)));

                              ASSERT(!frA->isHoliday(bdlt::Date(2011, 7,  4)));
                              ASSERT( frA->isHoliday(bdlt::Date(2011, 7, 14)));

                              ASSERT(!frB->isHoliday(bdlt::Date(2011, 7,  4)));
                              ASSERT( frB->isHoliday(bdlt::Date(2011, 7, 14)));
//..
// When 'usA', 'usB', 'frA', and 'frB' go out of scope, the resources used by
// the calendars to which they refer are automatically reclaimed.
//
// Finally, using the 'lookupCalendar' accessor, we attempt to retrieve a
// calendar that has not yet been loaded into the cache, but that we *know* to
// be supported by the calendar loader.  Since the 'lookupCalendar' accessor
// does not load calendars into the cache as a side-effect, the request fails:
//..
    bsl::shared_ptr<const bdlt::Calendar> de =
                                            readonlyCache.lookupCalendar("DE");

                              ASSERT(!de.get());
//..
        }

        if (verbose) cout << "Example 2: A Calendar Cache with a Timeout"
                          << endl;
        {
//
///Example 2: A Calendar Cache with a Timeout
/// - - - - - - - - - - - - - - - - - - - - -
// This second example shows the affects on a 'bdlt::CalendarCache' object that
// is constructed to have a timeout value.  Note that the following snippets of
// code assume a platform-independent 'sleepSeconds' method that sleeps for the
// specified number of seconds.
//
// First, we create a calendar loader and a calendar cache.  The cache is
// constructed to have a timeout of 3 seconds.  Of course, such a short timeout
// is inappropriate for production use, but it is necessary for illustrating
// the affects of a timeout in this example.  As in example 1 (above), we again
// let the cache use the default allocator:
//..
    MyCalendarLoader           loader;
    bdlt::CalendarCache        cache(&loader, bsls::TimeInterval(3));
    const bdlt::CalendarCache& readonlyCache = cache;
//..
// Next, we retrieve the calendar identified by "DE" from the cache:
//..
    bsl::shared_ptr<const bdlt::Calendar> deA = cache.getCalendar("DE");

                              ASSERT( deA.get());
//..
// Next, we sleep for 2 seconds before retrieving the "FR" calendar:
//..
    sleepSeconds(2);

    bsl::shared_ptr<const bdlt::Calendar> frA = cache.getCalendar("FR");

                              ASSERT( frA.get());
//..
// Next, we sleep for 2 more seconds before attempting to retrieve the "DE"
// calendar again, this time using the 'lookupCalendar' accessor.  Since the
// cumulative sleep time exceeds the timeout value established for the cache
// when it was constructed, the "DE" calendar has expired; hence, it has been
// removed from the cache:
//..
    sleepSeconds(2);

    bsl::shared_ptr<const bdlt::Calendar> deB =
                                            readonlyCache.lookupCalendar("DE");

                              ASSERT(!deB.get());
//..
// Next, we verify that the "FR" calendar is still available in the cache:
//..
    bsl::shared_ptr<const bdlt::Calendar> frB =
                                            readonlyCache.lookupCalendar("FR");

                              ASSERT( frA.get() == frB.get());
//..
// Finally, we sleep for an additional 2 seconds and verify that the "FR"
// calendar has also expired:
//..
    sleepSeconds(2);

    bsl::shared_ptr<const bdlt::Calendar> frC =
                                            readonlyCache.lookupCalendar("FR");

                              ASSERT(!frC.get());
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

        Obj mX(&loader,              &sa);
        Obj mY(&loader, Interval(0), &sa);

        const int NUM_TEST_ITERATIONS   =   10;
        const int NUM_THREAD_ITERATIONS = 1000;

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
        //: 1 That 'getCalendar', the only method that allocates memory, is
        //:   exception neutral.
        //
        // Plan:
        //: 1 We use the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macros to generate
        //:   exceptions in order to verify that there is no memory leak when
        //:   we invoke 'getCalendar'.  (C-1)
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

              e  = mX.getCalendar("ERROR");
                                         ASSERT(!e.get());

              e  = mX.getCalendar("CAL-1");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate1);

              e  = mX.getCalendar("CAL-2");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate2);

              e  = mX.getCalendar("CAL-1");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate1);

              rc = mX.invalidate("CAL-2");
                                         ASSERT(1 == rc);

              e  = mX.getCalendar("CAL-2");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate2);

              rc = mX.invalidateAll();   ASSERT(2 == rc);

              e  = mX.getCalendar("CAL-1");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate1);

              e  = mX.getCalendar("CAL-2");
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

            Obj mX(&loader, Interval(0), &sa);  const Obj& X = mX;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
              Entry e;

              e = mX.getCalendar("CAL-1");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate1);

              e = mX.getCalendar("CAL-2");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate2);

              // "CAL-1" is invalidated in cache by 'getCalendar' before being
              // reloaded.

              e = mX.getCalendar("CAL-1");
                                         ASSERT(e.get());
                                         ASSERT(e->firstDate() == gFirstDate1);

              // "CAL-2" is invalidated in cache by 'lookupCalendar'.

              e = X.lookupCalendar("CAL-2");
                                         ASSERT(!e.get());

              e = mX.getCalendar("CAL-2");
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
        //:   calendar in the cache, invalidates the named calendar and leaves
        //:   other calendars in the cache (if any) unaffected.
        //:
        //: 2 That 'invalidate' has no effect if the string supplied to it does
        //:   not identify any calendar in the cache.
        //:
        //: 3 That 'invalidate' returns the correct status value.
        //:
        //: 4 That 'invalidate' can be called on an empty cache.
        //:
        //: 5 That calendars that have been invalidated by 'invalidate' can be
        //:   reloaded successfully.
        //:
        //: 6 That 'invalidate' is not affected by a timeout.
        //:
        //: 7 That 'invalidateAll' invalidates all calendars in the calendar.
        //:
        //: 8 That 'invalidateAll' returns the correct status value.
        //:
        //: 9 That calendars that have been invalidated by 'invalidateAll' can
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
        //:     has been loaded with one, two, and three calendars.  (C-7..9)
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

            e  = mX.getCalendar("CAL-1");    ASSERT( e.get());

            rc = mX.invalidate("aCAL-1");    ASSERT(0 == rc);
            rc = mX.invalidate("CAL-1z");    ASSERT(0 == rc);

            e  = X.lookupCalendar("CAL-1");  ASSERT( e.get());

            rc = mX.invalidate("CAL-1");     ASSERT(1 == rc);

            e  = X.lookupCalendar("CAL-1");  ASSERT(!e.get());

            // Load "CAL-1" and "CAL-2" into the cache.

            e  = mX.getCalendar("CAL-1");    ASSERT( e.get());
            e  = mX.getCalendar("CAL-2");    ASSERT( e.get());

            rc = mX.invalidate("CAL-1");     ASSERT(1 == rc);

            e  = X.lookupCalendar("CAL-1");  ASSERT(!e.get());
            e  = X.lookupCalendar("CAL-2");  ASSERT( e.get());

            rc = mX.invalidate("CAL-2");     ASSERT(1 == rc);

            e  = X.lookupCalendar("CAL-1");  ASSERT(!e.get());
            e  = X.lookupCalendar("CAL-2");  ASSERT(!e.get());

            rc = mX.invalidate("CAL-2");     ASSERT(0 == rc);
        }

        if (verbose) cout << "\nTesting 'invalidate' (with timeout)." << endl;
        {
            TestLoader loader;

            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&loader, Interval(60), &sa);  const Obj& X = mX;

            Entry e;
            int   rc;

            // Call 'invalidate' on an empty cache.

            rc = mX.invalidate("x");         ASSERT(0 == rc);

            // Load "CAL-1" into the cache.

            e  = mX.getCalendar("CAL-1");    ASSERT( e.get());

            rc = mX.invalidate("aCAL-1");    ASSERT(0 == rc);
            rc = mX.invalidate("CAL-1z");    ASSERT(0 == rc);

            e  = X.lookupCalendar("CAL-1");  ASSERT( e.get());

            rc = mX.invalidate("CAL-1");     ASSERT(1 == rc);

            e  = X.lookupCalendar("CAL-1");  ASSERT(!e.get());

            // Load "CAL-1" and "CAL-2" into the cache.

            e  = mX.getCalendar("CAL-1");    ASSERT( e.get());
            e  = mX.getCalendar("CAL-2");    ASSERT( e.get());

            rc = mX.invalidate("CAL-1");     ASSERT(1 == rc);

            e  = X.lookupCalendar("CAL-1");  ASSERT(!e.get());
            e  = X.lookupCalendar("CAL-2");  ASSERT( e.get());

            rc = mX.invalidate("CAL-2");     ASSERT(1 == rc);

            e  = X.lookupCalendar("CAL-1");  ASSERT(!e.get());
            e  = X.lookupCalendar("CAL-2");  ASSERT(!e.get());

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

            e  = mX.getCalendar("CAL-1");    ASSERT( e.get());

            rc = mX.invalidateAll();         ASSERT(1 == rc);

            e  = X.lookupCalendar("CAL-1");  ASSERT(!e.get());

            // Load "CAL-1" and "CAL-2" into the cache.

            e  = mX.getCalendar("CAL-1");    ASSERT( e.get());
            e  = mX.getCalendar("CAL-2");    ASSERT( e.get());

            rc = mX.invalidateAll();         ASSERT(2 == rc);

            e  = X.lookupCalendar("CAL-1");  ASSERT(!e.get());
            e  = X.lookupCalendar("CAL-2");  ASSERT(!e.get());

            // Load "CAL-1", "CAL-2", and "CAL-3" into the cache.

            e  = mX.getCalendar("CAL-1");    ASSERT( e.get());
            e  = mX.getCalendar("CAL-2");    ASSERT( e.get());
            e  = mX.getCalendar("CAL-3");    ASSERT( e.get());

            rc = mX.invalidateAll();         ASSERT(3 == rc);

            e  = X.lookupCalendar("CAL-1");  ASSERT(!e.get());
            e  = X.lookupCalendar("CAL-2");  ASSERT(!e.get());
            e  = X.lookupCalendar("CAL-3");  ASSERT(!e.get());
        }

        if (verbose) cout << "\nTesting 'invalidateAll' (with timeout)."
                          << endl;
        {
            TestLoader loader;

            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&loader, Interval(60), &sa);  const Obj& X = mX;

            Entry e;
            int   rc;

            // Call 'invalidateAll' on an empty cache.

            rc = mX.invalidateAll();         ASSERT(0 == rc);

            // Load "CAL-1" into the cache.

            e  = mX.getCalendar("CAL-1");    ASSERT( e.get());

            rc = mX.invalidateAll();         ASSERT(1 == rc);

            e  = X.lookupCalendar("CAL-1");  ASSERT(!e.get());

            // Load "CAL-1" and "CAL-2" into the cache.

            e  = mX.getCalendar("CAL-1");    ASSERT( e.get());
            e  = mX.getCalendar("CAL-2");    ASSERT( e.get());

            rc = mX.invalidateAll();         ASSERT(2 == rc);

            e  = X.lookupCalendar("CAL-1");  ASSERT(!e.get());
            e  = X.lookupCalendar("CAL-2");  ASSERT(!e.get());

            // Load "CAL-1", "CAL-2", and "CAL-3" into the cache.

            e  = mX.getCalendar("CAL-1");    ASSERT( e.get());
            e  = mX.getCalendar("CAL-2");    ASSERT( e.get());
            e  = mX.getCalendar("CAL-3");    ASSERT( e.get());

            rc = mX.invalidateAll();         ASSERT(3 == rc);

            e  = X.lookupCalendar("CAL-1");  ASSERT(!e.get());
            e  = X.lookupCalendar("CAL-2");  ASSERT(!e.get());
            e  = X.lookupCalendar("CAL-3");  ASSERT(!e.get());
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
        // 'getCalendar' AND 'lookupCalendar'
        //   Ensure that 'getCalendar' and 'lookupCalendar' behave as expected
        //   in the absence of exceptions (exception neutrality is tested in a
        //   a later test case).
        //
        // Concerns:
        //: 1 That 'getCalendar', when supplied with a string not recognized
        //:   by the loader, returns null.
        //:
        //: 2 That 'getCalendar', when supplied with a string identifying a
        //:   calendar recognized by the loader that is NOT present in the
        //:   cache, loads the specified calendar into the cache and returns a
        //:   reference to that calendar.
        //:
        //: 3 That 'getCalendar', when supplied with a string identifying a
        //:   calendar recognized by the loader that IS present in the cache
        //:   and has NOT expired, returns a reference to that calendar.
        //:
        //: 4 That 'getCalendar', when supplied with a string identifying a
        //:   calendar recognized by the loader that IS present in the cache
        //:   but HAS expired, reloads the specified calendar into the cache
        //:   and returns a reference to that (newly-loaded) calendar.
        //:
        //: 5 That 'getCalendar' allocates memory if and only if it loads a
        //:   calendar into the cache.
        //:
        //: 6 That 'lookupCalendar', when supplied with a string not
        //:   identifying a calendar present in the cache, returns null.
        //:
        //: 7 That 'lookupCalendar', when supplied with a string identifying a
        //:   calendar present in the cache that has NOT expired, returns a
        //:   reference to that calendar.
        //:
        //: 8 That 'lookupCalendar', when supplied with a string identifying a
        //:   calendar present in the cache that HAS expired, returns null.
        //:
        //: 9 That 'lookupCalendar' allocates no memory from any allocator.
        //:
        //:10 That both 'getCalendar' and 'lookupCalendar' return pointers
        //:   providing non-modifiable access (only).
        //:
        //:11 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 'getCalendar' and 'lookupCalendar' are tested ad hoc, using a
        //:   cache with no timeout, as follows: (C-1..2, C-5..6, C-9)
        //:
        //:   1 'getCalendar' and 'lookupCalendar' are called with various
        //:     strings on the initially empty cache.  (C-1)
        //:
        //:   2 Using 'getCalendar', the "CAL-1" calendar is loaded into the
        //:     cache, then 'lookupCalendar' and 'getCalendar' are called with
        //:     various strings.
        //:
        //:   3 A second calendar, "CAL-2", is loaded into the cache, and
        //:     'lookupCalendar' and 'getCalendar' are again called with
        //:     various strings.  (C-2, C-5..6, C-9)
        //:
        //:   4 Verify that 'getCalendar' and 'lookupCalendar' return pointers
        //:     providing non-modifiable access (only).  (C-10)
        //:
        //: 2 'getCalendar' and 'lookupCalendar' are further tested ad hoc,
        //:   using a cache with a 1-second timeout, as follows: (C-3..4,
        //:   C-7..8)
        //:
        //:   1 Calendars "CAL-1" and "CAL-2" are loaded into the cache, then
        //:     'lookupCalendar' and 'getCalendar' are used to verify that the
        //:     calendars have not yet expired.  (C-3, C-7).
        //:
        //:   2 Sleep for 2 seconds.
        //:
        //:   3 'lookupCalendar' is used to verify that "CAL-1" has expired
        //:     (null is returned); 'getCalendar' is used to verify that
        //:     "CAL-2" has expired ("CAL-2" is reloaded into the cache).
        //:     (C-4, C-8).
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered (using the 'BSLS_ASSERTTEST_*' macros).  (C-11)
        //
        // Testing:
        //   shared_ptr<const Calendar> getCalendar(const char *name);
        //   shared_ptr<const Calendar> lookupCalendar(const char *name) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'getCalendar' AND 'lookupCalendar'" << endl
                          << "==================================" << endl;

        if (verbose) cout << "\nTesting without a timeout." << endl;
        {
            TestLoader loader;

            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&loader, &sa);  const Obj& X = mX;

            // Empty cache.
            {
                Entry e;

                e = mX.getCalendar("ERROR");      ASSERT(!e.get());

                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());

                e = mX.getCalendar("CAL-Z");      ASSERT(!e.get());

                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());

                e = X.lookupCalendar("CAL-Z");    ASSERT(!e.get());

                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());
            }

            bsls::Types::Int64 daLastNumBlocksTotal, saLastNumBlocksTotal;

            // Cache with one entry.
            {
                Entry e1, ex;

                e1 = mX.getCalendar("CAL-1");     ASSERT(e1.get());
                                                  ASSERT(e1->firstDate() ==
                                                                  gFirstDate1);

                ASSERT(true == providesNonModifiableAccessOnly(e1.get()));

                LOOP_ASSERT(da.numBlocksInUse(), 0 == da.numBlocksInUse());
                LOOP_ASSERT(sa.numBlocksInUse(), 0 <  sa.numBlocksInUse());

                daLastNumBlocksTotal = da.numBlocksTotal();
                saLastNumBlocksTotal = sa.numBlocksTotal();

                ex = X.lookupCalendar("CAL-1");   ASSERT(ex.get());
                                                  ASSERT(ex.get() == e1.get());

                ASSERT(true == providesNonModifiableAccessOnly(ex.get()));

                LOOP2_ASSERT(daLastNumBlocksTotal, da.numBlocksTotal(),
                             daLastNumBlocksTotal == da.numBlocksTotal());
                LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                             saLastNumBlocksTotal == sa.numBlocksTotal());

                ex = mX.getCalendar("CAL-1");     ASSERT(ex.get());
                                                  ASSERT(ex.get() == e1.get());

                LOOP2_ASSERT(daLastNumBlocksTotal, da.numBlocksTotal(),
                             daLastNumBlocksTotal == da.numBlocksTotal());
                LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                             saLastNumBlocksTotal == sa.numBlocksTotal());

                ex = mX.getCalendar("aCAL-1");    ASSERT(!ex.get());

                ex = mX.getCalendar("CAL-1z");    ASSERT(!ex.get());

                ex = X.lookupCalendar("aCAL-1");  ASSERT(!ex.get());

                ex = X.lookupCalendar("CAL-1z");  ASSERT(!ex.get());

                LOOP2_ASSERT(daLastNumBlocksTotal, da.numBlocksTotal(),
                             daLastNumBlocksTotal == da.numBlocksTotal());
                LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                             saLastNumBlocksTotal == sa.numBlocksTotal());
            }

            // Cache with two entries.
            {
                Entry e1, e2, ex, ey;

                const bsls::Types::Int64 lastNumBlocksInUse =
                                                           sa.numBlocksInUse();

                e2 = mX.getCalendar("CAL-2");     ASSERT(e2.get());
                                                  ASSERT(e2->firstDate() ==
                                                                  gFirstDate2);

                LOOP_ASSERT(da.numBlocksInUse(), 0 == da.numBlocksInUse());
                LOOP2_ASSERT(lastNumBlocksInUse, sa.numBlocksInUse(),
                             lastNumBlocksInUse < sa.numBlocksInUse());

                daLastNumBlocksTotal = da.numBlocksTotal();
                saLastNumBlocksTotal = sa.numBlocksTotal();

                ey = X.lookupCalendar("CAL-2");   ASSERT(ey.get());
                                                  ASSERT(ey.get() == e2.get());

                e1 = mX.getCalendar("CAL-1");     ASSERT(e1.get());
                                                  ASSERT(e1->firstDate() ==
                                                                  gFirstDate1);

                ex = X.lookupCalendar("CAL-1");   ASSERT(ex.get());
                                                  ASSERT(ex.get() == e1.get());

                ex = X.lookupCalendar("CAL-Z");   ASSERT(!ex.get());

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

            Obj mX(&loader, Interval(1), &sa);  const Obj& X = mX;

            Entry e1, e2, eo;

            e1 = mX.getCalendar("CAL-1");    ASSERT( e1.get());
            e2 = mX.getCalendar("CAL-2");    ASSERT( e2.get());

            eo = X.lookupCalendar("CAL-1");  ASSERT( eo.get());
                                             ASSERT( eo.get() == e1.get());

            eo = X.lookupCalendar("CAL-2");  ASSERT( eo.get());
                                             ASSERT( eo.get() == e2.get());

            eo = mX.getCalendar("CAL-1");    ASSERT( eo.get());
                                             ASSERT( eo.get() == e1.get());

            eo = mX.getCalendar("CAL-2");    ASSERT( eo.get());
                                             ASSERT( eo.get() == e2.get());

            const bsls::Types::Int64 saLastNumBlocksTotal =
                                                           sa.numBlocksTotal();

            sleepSeconds(2);

            eo = X.lookupCalendar("CAL-1");  ASSERT(!eo.get());

            eo = mX.getCalendar("CAL-2");    ASSERT( eo.get());
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

                ASSERT_SAFE_PASS(mX.getCalendar(""));

                ASSERT_SAFE_FAIL(mX.getCalendar(0));
            }

            {
                const Obj X(&loader, &sa);

                ASSERT_SAFE_PASS(X.lookupCalendar(""));

                ASSERT_SAFE_FAIL(X.lookupCalendar(0));
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
        //: 2 If a timeout is NOT supplied at construction, calendars do not
        //:   expire from the cache.
        //:
        //: 3 If a timeout IS supplied at construction, calendars expire from
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
        //:   4 Use the (as yet unproven) 'getCalendar' manipulator to load a
        //:     a calendar into 'mX'; verify that the calendar loader supplied
        //:     at construction is used and that use of the object and default
        //:     allocators are as expected.  (C-1, C4..6)
        //:
        //:   5 Use the (as yet unproven) 'lookupCalendar' accessor to verify
        //:     that a timeout is not in effect (see the TEST PLAN regarding
        //:     timeout testing).  (C-2)
        //:
        //:   6 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //:
        //: 2 Repeat P-1, but this time using the three-argument constructor
        //:   to supply a timeout (P-1.2), and using 'getCalendar' instead of
        //:   'lookupCalendar' to reload the calendar (P-1.5).  (C-1, C-3..8)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered (using the 'BSLS_ASSERTTEST_*' macros).  (C-9)
        //
        // Testing:
        //   CalendarCache(Loader *loader,          Allocator *ba = 0);
        //   CalendarCache(Loader *loader, timeout, Allocator *ba = 0);
        //   ~CalendarCache();
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
                Entry e1 = mX.getCalendar("CAL-1");

                LOOP_ASSERT(CONFIG, e1.get());
                LOOP_ASSERT(CONFIG, e1->firstDate() == gFirstDate1);

                // Check that some object memory is now in use.

                LOOP_ASSERT(CONFIG, 1 <= oa.numBlocksInUse());

                if ('c' == CONFIG) {
                    LOOP_ASSERT(CONFIG, 0 == da.numBlocksInUse());
                }

                const bsls::Types::Int64 lastNumBlocksInUse =
                                                           oa.numBlocksInUse();

                Entry e2 = X.lookupCalendar("CAL-1");

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
                objPtr = new (fa) Obj(&loader, Interval(0));
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) Obj(&loader, Interval(0), 0);
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(&loader, Interval(0), &sa);
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
                Entry e1 = mX.getCalendar("CAL-1");

                LOOP_ASSERT(CONFIG, e1.get());
                LOOP_ASSERT(CONFIG, e1->firstDate() == gFirstDate1);

                // Check that some object memory is now in use.

                LOOP_ASSERT(CONFIG, 1 <= oa.numBlocksInUse());

                if ('c' == CONFIG) {
                    LOOP_ASSERT(CONFIG, 0 == da.numBlocksInUse());
                }

                const bsls::Types::Int64 lastNumBlocksInUse =
                                                           oa.numBlocksInUse();

                Entry e2 = mX.getCalendar("CAL-1");

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
                ASSERT_SAFE_PASS(Obj(&loader, Interval(1), &sa));
                ASSERT_SAFE_FAIL(Obj(      0, Interval(1), &sa));

                ASSERT_SAFE_PASS(Obj(&loader, Interval(0,  0), &sa));
                ASSERT_SAFE_FAIL(Obj(&loader, Interval(0, -1), &sa));

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
        //: 1 Create a calendar cache 'mX'.
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

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        bsls::Types::Int64 daLastNumBlocksTotal;
        bsls::Types::Int64 saLastNumBlocksTotal, saLastNumBlocksInUse;

        {
            // 1 Create a calendar cache 'mX'.

            Obj mX(&loader, &sa);  const Obj& X = mX;

            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
            LOOP_ASSERT(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());

            {
                // 2 Load "CAL-1" into 'mX' with 'Entry' 'e1' referring to
                //   "CAL-1".

                Entry e1 = mX.getCalendar("CAL-1");

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

                Entry e = X.lookupCalendar("CAL-1");

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

            Entry e = X.lookupCalendar("CAL-1");

            ASSERT(!e.get());

            LOOP2_ASSERT(daLastNumBlocksTotal, da.numBlocksTotal(),
                         daLastNumBlocksTotal == da.numBlocksTotal());
            LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                         saLastNumBlocksTotal == sa.numBlocksTotal());

            // 9 Reload "CAL-1" into the cache.

            e = mX.getCalendar("CAL-1");

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
        //: 2 Use the 'getCalendar' method to load a calendar into the cache.
        //:
        //: 3 Wait a period significantly less than the timeout and use the
        //:   'lookupCalendar' method to retrieve the calendar loaded in P-1.
        //:   Verify that the calendar was successfully retrieved.
        //:
        //: 4 Wait another period so that the cumulative waiting time for P-2
        //:   and P-3 is greater than the timeout of the cache.  Again use the
        //:   'lookupCalendar' method to retrieve the calendar loaded in P-1,
        //:   and verify that the calendar is no longer in the cache.
        //:
        //: 5 Repeat P-1..4, but this time use the 'getCalendar' method to
        //:   observe the affects of a timeout.  (C-1)
        //
        // Testing:
        //   CONCERN: A non-trivial timeout is processed correctly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TIMEOUT GREATER THAN 20 SECONDS" << endl
                          << "===============================" << endl;

        TestLoader loader;

        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        Obj mX(&loader, Interval(30), &sa);  const Obj& X = mX;

        // Observe affects of timeout via 'lookupCalendar' method.
        {
            Entry e;

            e = mX.getCalendar("CAL-1");         ASSERT( e.get());

            sleepSeconds( 5);

            e = X.lookupCalendar("CAL-1");       ASSERT( e.get());

            sleepSeconds(30);

            e = X.lookupCalendar("CAL-1");       ASSERT(!e.get());
        }

        // Observe affects of timeout via 'getCalendar' method.
        {
            Entry e1 = mX.getCalendar("CAL-1");  ASSERT(e1.get());

            const bsls::Types::Int64 saLastNumBlocksTotal =
                                                           sa.numBlocksTotal();

            sleepSeconds( 5);

            Entry e2 = mX.getCalendar("CAL-1");  ASSERT(e2.get());
                                                 ASSERT(e2.get() == e1.get());

            LOOP2_ASSERT(saLastNumBlocksTotal, sa.numBlocksTotal(),
                         saLastNumBlocksTotal == sa.numBlocksTotal());

            sleepSeconds(30);

            Entry e3 = mX.getCalendar("CAL-1");  ASSERT(e3.get());
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
