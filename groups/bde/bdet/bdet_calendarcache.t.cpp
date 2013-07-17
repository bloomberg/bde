// bdet_calendarcache.t.cpp                                           -*-C++-*-
#include <bdet_calendarcache.h>

#include <bdet_calendarloader.h>
#include <bdet_packedcalendar.h>

#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_platform.h>

#include <bsl_cstdio.h>       // printf()
#include <bsl_cstdlib.h>      // atoi()
#include <bsl_cstring.h>      // strcmp()
#include <bsl_iostream.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#include <crtdbg.h>  // '_CrtSetReportMode', to suppress popups
#else
#include <pthread.h>
#include <unistd.h>
#endif

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD
// ----------------------------------------------------------------------------
// bdet_CalendarCacheEntryPtr class:
// [ ?] bdet_CalendarCacheEntryPtr();
// [ ?] bdet_CalendarCacheEntryPtr(const CacheEntryPtr& original);
// [ ?] ~bdet_CalendarCacheEntryPtr();
// [ ?] bdet_CalendarCacheEntryPtr& operator=(const CacheEntryPtr& rhs);
// [ ?] const bdet_Calendar *operator->() const;
// [ ?] const bdet_Calendar *ptr() const;
//
// bdet_CalendarCache class:
// [ ?] bdet_CalendarCache(Loader *loader, Alloc *ba=0);
// [ ?] bdet_CalendarCache(Loader *loader, const bdet_TI& to, Alloc *ba=0);
// [ ?] ~bdet_CalendarCache();
// [ ?] bdet_CalendarCacheEntryPtr calendar(const char *calendarName);
// [ ?] void invalidate(const char *calendarName);
// [ ?] void invalidateAll();
// [ ?] bdet_CalendarCacheEntryPtr calendar(const char *calendarName) const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
// [ 4] EXCEPTION NEUTRALITY

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdet_CalendarCache         Obj;
typedef bdet_CalendarCacheEntryPtr Entry;

#ifdef BSLS_PLATFORM_OS_WINDOWS
typedef HANDLE    ThreadId;
#else
typedef pthread_t ThreadId;
#endif

typedef void *(*ThreadFunction)(void *arg);

// ============================================================================
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static
ThreadId createThread(ThreadFunction func, void *arg)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return CreateThread(0, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, 0);
#else
    ThreadId id;
    pthread_create(&id, 0, func, arg);
    return id;
#endif
}

static
void joinThread(ThreadId id)
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
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    Sleep(seconds * 1000);
#else
    sleep(seconds);
#endif
}

// TBD test loader that loads three named calendars (or somesuch small number)

static int holidayOffset = 38;

class TestLoader : public bdet_CalendarLoader {

    // DATA
    bdet_Date d_firstDate;

  private:
    // NOT IMPLEMENTED
    TestLoader(const TestLoader&);
    TestLoader& operator=(const TestLoader&);

  public:
    // CREATORS
    TestLoader(bslma::Allocator *basicAllocator = 0);

    ~TestLoader();

    // MANIPULATORS
    int load(bdet_PackedCalendar *result, const char *calendarName);

    void getFirstDate(bdet_Date *date);
};

// CREATORS
inline
TestLoader::TestLoader(bslma::Allocator *)
{
}

inline
TestLoader::~TestLoader()
{
}

int TestLoader::load(bdet_PackedCalendar *result, const char *calendarName)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(calendarName);

    if (0 == bsl::strcmp("ERROR", calendarName)) {
        return -1;
    }

    if (0 == bsl::strcmp("NOT_FOUND", calendarName)) {
        return 1;
    }

    result->removeAll();
    result->setValidRange(d_firstDate, bdet_Date(9999,12,31));
    result->addHoliday(d_firstDate + holidayOffset);
    d_firstDate++;

    return 0;
}

void TestLoader::getFirstDate(bdet_Date *date)
{
    BSLS_ASSERT(date);

    *date = d_firstDate;
}

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

// Define the 'my_CalendarLoader' class that is used in the Usage example.

class my_CalendarLoader : public bdet_CalendarLoader {

  private:
    // NOT IMPLEMENTED
    my_CalendarLoader(const my_CalendarLoader&);
    my_CalendarLoader& operator=(const my_CalendarLoader&);

  public:
    // CREATORS
    my_CalendarLoader(bslma::Allocator *basicAllocator = 0);

    ~my_CalendarLoader();

    // MANIPULATORS
    int load(bdet_PackedCalendar *result, const char *calendarName);
};

// CREATORS
inline
my_CalendarLoader::my_CalendarLoader(bslma::Allocator *)
{
}

inline
my_CalendarLoader::~my_CalendarLoader()
{
}

int my_CalendarLoader::load(bdet_PackedCalendar *result,
                            const char          *calendarName)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(calendarName);

    result->removeAll();
    result->setValidRange(bdet_Date(2000, 1, 1), bdet_Date(2020, 12, 31));

    if (     0 == bsl::strcmp("DE", calendarName)) {  // Germany
        return 0;                                                     // RETURN
    }
    else if (0 == bsl::strcmp("FR", calendarName)) {  // France
        result->addHoliday(bdet_Date(2011, 7, 14));

        return 0;                                                     // RETURN
    }
    else if (0 == bsl::strcmp("US", calendarName)) {  // USA
        result->addHoliday(bdet_Date(2011, 7,  4));

        return 0;                                                     // RETURN
    }
    else {                                            // others not supported
        return -1;
    }
}

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    // Suppress all windows debugging popups
    _CrtSetReportMode(_CRT_ASSERT,0);
    _CrtSetReportMode(_CRT_ERROR, 0);
    _CrtSetReportMode(_CRT_WARN,  0);
#endif

    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
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

        if (verbose) cout << "Example 1: Using a 'bdet_CalendarCache'" << endl;
        {
///Usage
///-----
// The following examples illustrate how to use a 'bdet_CalendarCache'.
//
///Example 1: Using a 'bdet_CalendarCache'
///- - - - - - - - - - - - - - - - - - - -
// This first example shows basic use of a 'bdet_CalendarCache' object with no
// timeout value specified at construction.
//
// In this example and the next, we assume a hypothetical calendar loader,
// 'my_CalendarLoader', the details of which are not important other than that
// it supports calendars identified by "DE", "FR", and "US", which nominally
// identify the major holidays in Germany, France, and the United States,
// respectively.  Furthermore, we cite two specific dates of interest:
// 2011/07/04, which was a holiday in the US (Independence Day), but not in
// France, and 2011/07/14, which was a holiday in France (Bastille Day), but
// not in the US.
//
// First, we create a calendar loader, an instance of 'my_CalendarLoader', and
// use it, in turn, to create a cache.  For the purposes of this example, it is
// sufficient to let the cache use the default allocator:
//..
    my_CalendarLoader  loader;
    bdet_CalendarCache cache(&loader);
//..
// Next, we retrieve the calendar identified by "US", verify that the loading
// of that calendar into the cache was successful ('usA.ptr()' is non-null),
// and verify that 2011/07/04 is recognized as a holiday in the "US" calendar,
// whereas 2011/07/14 is not:
//..
    bdet_CalendarCacheEntryPtr usA = cache.calendar("US");

                               ASSERT( usA.ptr());
                               ASSERT( usA->isHoliday(bdet_Date(2011, 7,  4)));
                               ASSERT(!usA->isHoliday(bdet_Date(2011, 7, 14)));
//..
// Next, we fetch the calendar identified by "FR", this time verifying that
// 2011/07/14 is recognized as a holiday in the "FR" calendar, but 2011/07/04
// is not:
//..
    bdet_CalendarCacheEntryPtr frA = cache.calendar("FR");

                               ASSERT( frA.ptr());
                               ASSERT(!frA->isHoliday(bdet_Date(2011, 7,  4)));
                               ASSERT( frA->isHoliday(bdet_Date(2011, 7, 14)));
//..
// Next, we retrieve the "FR" calendar again, this time via the 'calendar'
// accessor, and note that the request is satisfied by the calendar that is
// already in the cache:
//..
    const bdet_CalendarCache& readonlyCache = cache;

    bdet_CalendarCacheEntryPtr frB = readonlyCache.calendar("FR");

                               ASSERT( frA.ptr() == frB.ptr());
//..
// Next, we invalidate the "US" calendar in the cache and immediately fetch it
// again.  The call to 'invalidate' removed the "US" calendar from the cache,
// so it had to be reloaded into the cache to satisfy the request:
//..
    cache.invalidate("US");

    bdet_CalendarCacheEntryPtr usB = cache.calendar("US");

                               ASSERT( usB.ptr() != usA.ptr());
                               ASSERT( usB.ptr());
                               ASSERT( usB->isHoliday(bdet_Date(2011, 7,  4)));
                               ASSERT(!usB->isHoliday(bdet_Date(2011, 7, 14)));
//..
// Next, all calendars in the cache are invalidated, then reloaded:
//..
    cache.invalidateAll();

    bdet_CalendarCacheEntryPtr usC = cache.calendar("US");

                               ASSERT( usC.ptr() != usA.ptr());
                               ASSERT( usC.ptr() != usB.ptr());
                               ASSERT( usC.ptr());
                               ASSERT( usC->isHoliday(bdet_Date(2011, 7,  4)));
                               ASSERT(!usC->isHoliday(bdet_Date(2011, 7, 14)));

    bdet_CalendarCacheEntryPtr frC = cache.calendar("FR");

                               ASSERT( frC.ptr() != frA.ptr());
                               ASSERT( frC.ptr() != frB.ptr());
                               ASSERT( frC.ptr());
                               ASSERT(!frC->isHoliday(bdet_Date(2011, 7,  4)));
                               ASSERT( frC->isHoliday(bdet_Date(2011, 7, 14)));
//..
// Now, verify that references to calendars that were invalidated in the cache
// are still valid for clients that obtained references to them before they
// were made invalid:
//..
                               ASSERT( usA->isHoliday(bdet_Date(2011, 7,  4)));
                               ASSERT(!usA->isHoliday(bdet_Date(2011, 7, 14)));

                               ASSERT( usB->isHoliday(bdet_Date(2011, 7,  4)));
                               ASSERT(!usB->isHoliday(bdet_Date(2011, 7, 14)));

                               ASSERT(!frA->isHoliday(bdet_Date(2011, 7,  4)));
                               ASSERT( frA->isHoliday(bdet_Date(2011, 7, 14)));

                               ASSERT(!frB->isHoliday(bdet_Date(2011, 7,  4)));
                               ASSERT( frB->isHoliday(bdet_Date(2011, 7, 14)));
//..
// When 'usA', 'usB', 'frA', and 'frB' go out of scope, the resources used by
// the calendars to which they refer are automatically reclaimed.
//
// Finally, using the 'calendar' accessor, we attempt to retrieve a calendar
// that has not yet been loaded into the cache, but which we *know* to be
// supported by the calendar loader.  Since the 'calendar' accessor does not
// load calendars into the cache as a side-effect, the request fails:
//..
    bdet_CalendarCacheEntryPtr de = readonlyCache.calendar("DE");

                               ASSERT(!de.ptr());
//..
        }

        if (verbose) cout << "Example 2: A Calendar Cache with a Timeout"
                          << endl;
        {
///Example 2: A Calendar Cache with a Timeout
///- - - - - - - - - - - - - - - - - - - - -
// This second example shows the affects on a 'bdet_CalendarCache' object that
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
    my_CalendarLoader  loader;
    bdet_CalendarCache cache(&loader, bdet_TimeInterval(3));
    const bdet_CalendarCache& readonlyCache = cache;
//..
// Next, we retrieve the calendar identified by "DE" from the cache:
//..
    bdet_CalendarCacheEntryPtr deA = cache.calendar("DE");

                               ASSERT( deA.ptr());
//..
// Next, we sleep for 2 seconds before retrieving the "FR" calendar:
//..
    sleepSeconds(2);

    bdet_CalendarCacheEntryPtr frA = cache.calendar("FR");

                               ASSERT( frA.ptr());
//..
// Next, we sleep for 2 more seconds before attempting to retrieve the "DE"
// calendar again, this time using the 'calendar' *accessor*.  Since the
// cumulative sleep time exceeds the timeout value established for the cache
// when it was constructed, the "DE" calendar has expired; hence, it has been
// removed from the cache:
//..
    sleepSeconds(2);

    bdet_CalendarCacheEntryPtr deB = readonlyCache.calendar("DE");

                               ASSERT(!deB.ptr());
//..
// Next, we verify that the "FR" calendar is still available in the cache:
//..
    bdet_CalendarCacheEntryPtr frB = readonlyCache.calendar("FR");

                               ASSERT( frA.ptr() == frB.ptr());
//..
// Finally, we sleep for an additional 2 seconds and verify that the "FR"
// calendar has also expired:
//..
    sleepSeconds(2);

    bdet_CalendarCacheEntryPtr frC = readonlyCache.calendar("FR");

                               ASSERT(!frC.ptr());
//..
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING EXCEPTION NEUTRALITY
        //
        // Concerns:
        //  1.  The 'calendar', 'invalidate' and 'invalidateAll' methods are
        //      exception neutral.
        //
        // Plan:
        //
        //  To address concern 1, we use the 'BEGIN_BSLMA_EXCEPTION_TEST' and
        //  'END_BSLMA_EXCEPTION_TEST' macros to generator exceptions in order
        //  to verify that there is no memory leak when we invoke the above
        //  methods and that this object can be destroyed.
        //
        //  Tactics:
        //      - Ad Hoc test data selection method
        //      - Brute force test case implementation technique
        //
        //  Testing:
        //      Exception neutrality
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << " TESTING EXCEPTION NEUTRALITY"
                          << endl
                          << "============================="
                          << endl;

        {
            // Testing a calendar cache without a timeout value.

            TestLoader loader;
            Obj X(&loader, &testAllocator);
            Entry e;

            BEGIN_BSLMA_EXCEPTION_TEST {
              e = X.calendar("ERROR");
              ASSERT(0 == e.ptr());
              e = X.calendar("VALID");
              ASSERT(0 != e.ptr());
              e = X.calendar("VALID1");
              ASSERT(0 != e.ptr());
              e = X.calendar("VALID");
              ASSERT(0 != e.ptr());

              X.invalidate("VALID1");
              e = X.calendar("VALID1");
              ASSERT(0 != e.ptr());

              X.invalidateAll();
              e = X.calendar("VALID");
              ASSERT(0 != e.ptr());
              e = X.calendar("VALID1");
              ASSERT(0 != e.ptr());
            } END_BSLMA_EXCEPTION_TEST
        }
        {
            // Testing a calendar cache with a timeout value.

            TestLoader loader;
            Obj X(&loader, bdet_TimeInterval(1), &testAllocator);
            bdet_Date date1, date2;
            Entry e;

            BEGIN_BSLMA_EXCEPTION_TEST {
              e = X.calendar("VALID");
              ASSERT(0 != e.ptr());
              e = X.calendar("VALID1");
              ASSERT(0 != e.ptr());

              sleepSeconds(2);

              loader.getFirstDate(&date1);
              e = X.calendar("VALID");
              loader.getFirstDate(&date2);
              ASSERT(0 != e.ptr());
              ASSERT(date1 != date2);

              loader.getFirstDate(&date1);
              e = X.calendar("VALID1");
              loader.getFirstDate(&date2);
              ASSERT(0 != e.ptr());
              ASSERT(date1 != date2);
            } END_BSLMA_EXCEPTION_TEST
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING THE 'invalidate' and 'invalidateAll' METHODS
        //
        // We want to demonstrate that 'invalidate()' will mark the specified
        // calendar entry in the cache for reload, and 'invalidateAll()' will
        // mark all entries in the cache for reload.
        //
        // Concerns:
        //  1.  The 'invalidate()' method can be called to mark a calendar for
        //      reload without affecting other entries, regardless of whether
        //      this calendar existed in the cache before the invalidation.
        //  2.  The 'invalidateAll()' method marks all entries in the cache for
        //      reload.
        //
        // Plan:
        //
        //  To address concern 1, we will call the 'invalidate()' method with
        //  both valid and invalid 'calendarName'.  Then we will load the valid
        //  calendars again and make sure they are loaded via the loader and
        //  not from the cache.  We'll also make sure no other entries are
        //  modified.
        //
        //  To address concern 2, we will call the 'invalidateAll()' method on
        //  both an empty cache and on a non-empty cache and make sure all the
        //  entries are reloaded from the loader the next time they are
        //  accessed.
        //
        //  Tactics:
        //      - Ad Hoc test data selection method
        //      - Brute force test case implementation technique
        //
        //  Testing:
        //      void invalidate(const char *calendarName);
        //      void invalidateAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'invalidate' and 'invalidateAll" << endl
                          << "=======================================" << endl;

        TestLoader loader;
        Obj mX(&loader, &testAllocator);

        bdet_Date date1, date2;
        Entry e, eV, eV2;

        // try 'invalidate' on an empty cache

        mX.invalidate("VALID");

        // try 'invalidateAll' on an empty cache

        mX.invalidateAll();

        // load two entries into the cache

        e = mX.calendar("VALID");
        ASSERT(0 != e.ptr());
        eV = e;

        e = mX.calendar("VALID1");
        ASSERT(0 != e.ptr());
        eV2 = e;

        // invalidate a non-exist entry

        mX.invalidate("NONEXIST");
        loader.getFirstDate(&date1);

        e = mX.calendar("VALID");
        ASSERT(       0 != e.ptr());
        ASSERT(eV.ptr() == e.ptr());

        e = mX.calendar("VALID1");
        ASSERT(        0 != e.ptr());
        ASSERT(eV2.ptr() == e.ptr());

        loader.getFirstDate(&date2);
        ASSERT(date1 == date2);       // verify both are in the cache

        // invalidate a valid entry

        mX.invalidate("VALID");
        loader.getFirstDate(&date1);

        e = mX.calendar("VALID");
        ASSERT(       0 != e.ptr());
        ASSERT(eV.ptr() != e.ptr());
        eV = e;

        loader.getFirstDate(&date2);
        ASSERT(date1 != date2);       // verify "VALID" is reloaded

        loader.getFirstDate(&date1);
        e = mX.calendar("VALID1");
        ASSERT(        0 != e.ptr());
        ASSERT(eV2.ptr() == e.ptr());

        loader.getFirstDate(&date2);
        ASSERT(date1 == date2);       // verify "VALID1" is still in cache

        // invalidate another valid entry

        mX.invalidate("VALID1");
        loader.getFirstDate(&date1);

        e = mX.calendar("VALID1");
        ASSERT(        0 != e.ptr());
        ASSERT(eV2.ptr() != e.ptr());
        eV2 = e;

        loader.getFirstDate(&date2);
        ASSERT(date1 != date2);       // verify "VALID1" is reloaded

        // try 'invalidateAll()' on a non-empty cache

        e = mX.calendar("VALID");
        ASSERT(       0 != e.ptr());
        ASSERT(eV.ptr() == e.ptr());

        e = mX.calendar("VALID1");
        ASSERT(        0 != e.ptr());
        ASSERT(eV2.ptr() == e.ptr());

        mX.invalidateAll();
        loader.getFirstDate(&date1);

        e = mX.calendar("VALID");
        ASSERT(       0 != e.ptr());
        ASSERT(eV.ptr() != e.ptr());

        loader.getFirstDate(&date2);
        ASSERT(date1 != date2);       // verify "VALID" is reloaded

        loader.getFirstDate(&date1);
        e = mX.calendar("VALID1");

        ASSERT(        0 != e.ptr());
        ASSERT(eV2.ptr() != e.ptr());

        loader.getFirstDate(&date2);
        ASSERT(date1 != date2);       // verify "VALID1" is reloaded

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING THE 'calendar' METHOD
        //
        // We want to demonstrate that the 'calendar()' method returns a
        // 'bdet_Calendar' object from the cache when a valid entry with
        // matching 'calendarName' is found; and that it loads the
        // 'bdet_Calendar' object using the loader and properly inserts it
        // into the cache when a matching 'calendarName' is not found.  We also
        // want to demonstrate that if the calendar cache objects has a timeout
        // value, the expired entries will get reloaded using the loader when
        // they are accessed.
        //
        // Concerns:
        //  1.  The 'calendar()' method returns 0 if an invalid 'calendarName'
        //      is given.
        //  2   The 'calendar()' method returns the properly-loaded
        //      'bdet_Calendar' object and inserts it into the cache if a
        //      valid 'calendarName' is given but is not found in the cache.
        //  3.  The 'calendar()' method returns the properly-loaded
        //      'bdet_Calendar' object from the cache if a matching
        //      'calendarName' is found, but does not modify the cache nor call
        //      the loader.
        //  4.  A calendar cache without a timeout value does not expire any of
        //      its caches entries.
        //  5.  A calendar cache with a timeout value expires an entry only
        //      after its timeout value has been reached.
        //
        // Plan:
        //
        //  To address concern 1, we will call the 'calendar()' method with
        //  some invalid 'calendarName' values and make sure it returns 0.
        //
        //  To address concern 2, we will call the 'calendar()' method with a
        //  valid 'calendarName' which is not in the cache.  We first make sure
        //  the 'calendar()' method returns the correct 'bdet_Calendar" object
        //  by checking its 'firstDate()' and holidays.  Then we verify the new
        //  object is properly loaded into the cache by iterating through all
        //  cache entries.  We also verify that the test loader's 'load'
        //  method was called by making sure that the 'd_firstDate' member has
        //  been modified.  We will do the above for the following 2 cases:
        //  when the cache is empty, and when the cache has one or more
        //  entries.
        //
        //  To address concern 3, we will call the 'calendar()' method with a
        //  'calendarName' that is already in the cache.  We first make sure
        //  it returns the correct 'bdet_Calendar' object by checking its
        //  'firstDate()' and holidays.  Then we verify that this
        //  'bdet_Calendar' object is retrieved from the cache by making sure
        //  the test loader's 'load' method was not called.
        //
        //  To address concern 4, we will construct a calendar cache object
        //  without a timeout value and verify that all its entries are still
        //  considered valid in the cache after a certain amount of time.
        //
        //  To address concern 5, we will construct a calendar cache object
        //  with a timeout.  Then we will load a calendar, wait for a while,
        //  then insert another one.  We will then test whether they are valid
        //  at different times.  During the test we will also try to "use"
        //  these entries by loading them from the cache and make sure that has
        //  no effect on the expiration of the cache entries.
        //
        //  Tactics:
        //      - Ad Hoc test data selection method
        //      - Brute force test case implementation technique
        //
        //  Testing:
        //      const bdet_Calendar *calendar(const char *calendarName);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << " TESTING THE 'calendar()' METHOD" << endl
                          << "================================" << endl;

        TestLoader loader;
        Obj mX(&loader, &testAllocator);

        bdet_Date date1, date2;
        const bdet_Date FirstDate(1,1,1), FirstDate1(1,1,2);
        Entry e, e1, e0;

        // Concern 1

        if (verbose) cout << endl
                          << "   Testing concern 1" << endl;

        e = mX.calendar("ERROR");
        ASSERT(0 == e.ptr());

        e = mX.calendar("NOT_FOUND");
        ASSERT(0 == e.ptr());


        // Concern 2 for empty cache

        if (verbose) cout << endl
                          << "  Testing concern 2 (empty cache)" << endl;
        loader.getFirstDate(&date1);
        e = mX.calendar("VALID");
        ASSERT(0 != e.ptr());
        loader.getFirstDate(&date2);
        ASSERT(date1 != date2); // Verify 'load' method is called.
        ASSERT(date1 == e->firstDate());
        ASSERT(    1 == e->isHoliday(e->firstDate()+holidayOffset));

        // Concern 2 for non-empty cache

        if (verbose) cout << endl
                          << "   Testing concern 2 (non-empty cache)" << endl;
        loader.getFirstDate(&date1);
        e1 = mX.calendar("VALID1");
        ASSERT(    0 != e1.ptr());

        loader.getFirstDate(&date2);
        ASSERT(date1 != date2); // Verify 'load' method is called.
        ASSERT(date1 == e1->firstDate());
        ASSERT(    1 == e1->isHoliday(e1->firstDate()+holidayOffset));

        e0 = mX.calendar("VALID0");  // Insert an entry between them.
        ASSERT(0 != e0.ptr());

        // Concern 3

        if (verbose) cout << endl
                          << "   Testing concern 3" << endl;
        loader.getFirstDate(&date1);
        e = mX.calendar("VALID");
        ASSERT(0 != e.ptr());

        loader.getFirstDate(&date2);
        ASSERT(    date1 == date2); // Verify 'load' method is not called.
        ASSERT(FirstDate == e->firstDate());
        ASSERT(        1 == e->isHoliday(e->firstDate()+holidayOffset));

        e = mX.calendar("VALID1");
        ASSERT(0 != e.ptr());

        loader.getFirstDate(&date2);
        ASSERT(     date1 == date2); // Verify 'load' method is not called.
        ASSERT(FirstDate1 == e->firstDate());
        ASSERT(         1 == e->isHoliday(e->firstDate()+holidayOffset));

        // Concern 4 and 5

        TestLoader loaderY;
        Obj Y(&loaderY, bdet_TimeInterval(4), &testAllocator);
        Entry eV, eV1;

        // load first entry into Y

        e = Y.calendar("VALID");
        ASSERT(0 != e.ptr());
        eV = e;

        sleepSeconds(2);

        // load second entry into Y

        e = Y.calendar("VALID1");
        ASSERT(0 != e.ptr());
        eV1 = e;

        sleepSeconds(1);

        // now access both entries from cache to verify they still there

        loaderY.getFirstDate(&date1);
        e = Y.calendar("VALID");
        ASSERT(       0 != e.ptr());
        ASSERT(eV.ptr() == e.ptr());

        loaderY.getFirstDate(&date2);
        ASSERT(date1 == date2); // Verify 'load' method is not called.

        loaderY.getFirstDate(&date1);

        e = Y.calendar("VALID1");
        ASSERT(        0 != e.ptr());
        ASSERT(eV1.ptr() == e.ptr());

        loaderY.getFirstDate(&date2);
        ASSERT(date1 == date2); // Verify 'load' method is not called.

        sleepSeconds(2);

        // now only the second entry should be in the cache

        loaderY.getFirstDate(&date1);
        e = Y.calendar("VALID1");
        ASSERT(        0 != e.ptr());
        ASSERT(eV1.ptr() == e.ptr());

        loaderY.getFirstDate(&date2);
        ASSERT(date1 == date2); // Verify 'load' method is not called.

        loaderY.getFirstDate(&date1);

        e = Y.calendar("VALID");
        ASSERT(       0 != e.ptr());
        ASSERT(eV.ptr() != e.ptr());

        loaderY.getFirstDate(&date2);
        ASSERT(date1 != date2); // Verify 'load' method is called.

        sleepSeconds(2);

        // now the second entry should expire

        loaderY.getFirstDate(&date1);

        e = Y.calendar("VALID1");
        ASSERT(        0 != e.ptr());
        ASSERT(eV1.ptr() != e.ptr());

        loaderY.getFirstDate(&date2);
        ASSERT(date1 != date2); // Verify 'load' method is called.

        // verify that after reload, the reload flag is cleared

        loaderY.getFirstDate(&date1);

        e = Y.calendar("VALID1");
        ASSERT(        0 != e.ptr());
        ASSERT(eV1.ptr() != e.ptr());

        loaderY.getFirstDate(&date2);
        ASSERT(date1 == date2); // Verify 'load' method is not called.

        // the entries in mX should still be there

        loader.getFirstDate(&date1);

        e = mX.calendar("VALID");
        ASSERT(0 != e.ptr());

        e = mX.calendar("VALID1");
        ASSERT(0 != e.ptr());

        loader.getFirstDate(&date2);
        ASSERT(date1 == date2); // Verify 'load' method is not called.

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Testing:
        //   This test *exercises* basic functionality, but "tests" nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        TestLoader loader;
        Obj mX(&loader);

        Entry e = mX.calendar("ERROR");       ASSERT(0 == e.ptr());

              e = mX.calendar("NOT_FOUND");   ASSERT(0 == e.ptr());

              e = mX.calendar("VALID");       ASSERT(0 != e.ptr());

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING TIMEOUT GREATER THAN 60 SECONDS
        //
        // Concerns:
        //: 1 An object having a timeout value greater than 20 seconds
        //    correctly expire any of its cache entries.
        //
        // Plan:
        //: 1 Create an object that has a timeout of greater than 20 seconds,
        //:   and use the 'calendar' method to load a calendar using the
        //:   object.
        //:
        //: 2 Wait a period less than the timeout and use the 'calendar' method
        //:   to retrieve the calendar loaded in P-1.  Verify that the calendar
        //:   was not reloaded.  (C-1)
        //:
        //: 3 Wait a period so that the cumulative waiting period for P-2 to
        //:   P-3 is greater than the timeout.  Use the 'calendar' method to
        //:   retrieve the calendar loaded in P-1.  Verify that the calendar
        //:   was reloaded.  (C-1)
        //
        //  Testing:
        //      const bdet_Calendar *calendar(const char *calendarName);
        // --------------------------------------------------------------------

        TestLoader loaderY;
        Obj Y(&loaderY, bdet_TimeInterval(30), &testAllocator);

        Entry e, eV;
        bdet_Date date1, date2;

        e = Y.calendar("VALID");
        ASSERT(0 != e.ptr());
        eV = e;
        loaderY.getFirstDate(&date1);

        sleepSeconds(2);

        e = Y.calendar("VALID");
        ASSERT(eV.ptr() == e.ptr());
        loaderY.getFirstDate(&date2);
        ASSERT(date1 == date2);

        sleepSeconds(30);

        e = Y.calendar("VALID");
        ASSERT(eV.ptr() != e.ptr());
        loaderY.getFirstDate(&date2);
        ASSERT(date1 != date2);

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
