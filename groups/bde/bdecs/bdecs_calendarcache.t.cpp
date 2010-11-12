// bdecs_calendarcache.t.cpp          -*-C++-*-

#include <bdecs_calendarcache.h>

#include <bdex_testoutstream.h>                 // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only

#include <bsls_platform.h>                      // for testing only
#include <bsls_platformutil.h>                  // for testing only
#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bsl_cstdio.h>       // printf()
#include <bsl_cstdlib.h>      // atoi()
#include <bsl_cstring.h>      // strcmp()
#include <bsl_iostream.h>

#include <bdecs_calendarloader.h>
#include <bdecs_packedcalendar.h>

#ifdef BSLS_PLATFORM__CMP_MSVC
#include <windows.h>
#include <crtdbg.h>  // '_CrtSetReportMode', to suppress popups
#else
#include <unistd.h>
#endif

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//==========================================================================
//                             TEST PLAN
//--------------------------------------------------------------------------
//                              Overview
//                              --------
//--------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] THE ITERATORS
// [ 3] const bdecs_Calendar *bdecs_CalendarCache::calendar()
// [ 4] invalidate() and invalidateAll()
// [ 5] EXCEPTION NEUTRALITY
//
//==========================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
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

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
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


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number


//=============================================================================
// ITERATOR TEST UTILITY FUNCTIONS
// ----------------------------------------------------------------------------

template <typename U, typename V>
struct IsSame
{
    enum { VALUE = 0 };
};

template <typename U>
struct IsSame<U, U>
{
    enum { VALUE = 1 };
};


//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdecs_CalendarCache Obj;
typedef bdecs_CalendarCache::ConstIterator Iterator;

static int holidayOffset = 38;

class testLoader : public bdecs_CalendarLoader {
  private:
    testLoader(const testLoader&);
    testLoader& operator=(const testLoader&);

    bdet_Date d_firstDate;

 public:
    testLoader(bslma_Allocator *basicAllocator = 0);
    ~testLoader();

    int load(bdecs_PackedCalendar *result, const char *calendarName);
    void getFirstDate(bdet_Date *date);
};

inline
testLoader::testLoader(bslma_Allocator *basicAllocator)
{
}

inline
testLoader::~testLoader()
{
}

inline
int testLoader::load(bdecs_PackedCalendar *result, const char *calendarName)
{
    if (!calendarName || bsl::strcmp("ERROR", calendarName) == 0) {
        return -1;
    }
    if (bsl::strcmp("NOTFOUND", calendarName) == 0) {
        return 1;
    }
    result->removeAll();
    result->setValidRange(d_firstDate, bdet_Date(9999,12,31));
    result->addHoliday(d_firstDate + holidayOffset);
    d_firstDate++;

    return 0;
}

inline
void testLoader::getFirstDate(bdet_Date *date)
{
    *date = d_firstDate;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
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

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
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

            testLoader loader;
            Obj X(&loader, &testAllocator);
            const bdecs_Calendar *cal;

            BEGIN_BSLMA_EXCEPTION_TEST {
                cal = X.calendar("ERROR");
                ASSERT(0 == cal);
                cal = X.calendar("VALID");
                ASSERT(0 != cal);
                cal = X.calendar("VALID1");
                ASSERT(0 != cal);
                cal = X.calendar("VALID");
                ASSERT(0 != cal);

                X.invalidate("VALID1");
                cal = X.calendar("VALID1");
                ASSERT(0 != cal);

                X.invalidateAll();
                cal = X.calendar("VALID");
                ASSERT(0 != cal);
                cal = X.calendar("VALID1");
                ASSERT(0 != cal);
            } END_BSLMA_EXCEPTION_TEST
        }
        {
            // Testing a calendar cache with a timeout value.

            testLoader loader;
            Obj X(&loader, bdet_TimeInterval(1), &testAllocator);
            bdet_Date date1, date2;
            const bdecs_Calendar *cal;

            BEGIN_BSLMA_EXCEPTION_TEST {
                cal = X.calendar("VALID");
                ASSERT(0 != cal);
                cal = X.calendar("VALID1");
                ASSERT(0 != cal);

                #ifdef BSLS_PLATFORM__CMP_MSVC
                Sleep(2 * 1000);
                #else
                sleep(2);
                #endif

                loader.getFirstDate(&date1);
                cal = X.calendar("VALID");
                loader.getFirstDate(&date2);
                ASSERT(0 != cal);
                ASSERT(date1 != date2);

                loader.getFirstDate(&date1);
                cal = X.calendar("VALID1");
                loader.getFirstDate(&date2);
                ASSERT(0 != cal);
                ASSERT(date1 != date2);
            } END_BSLMA_EXCEPTION_TEST
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING THE 'invalidate()' and 'invalidateAll()' METHODS
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
                          << " TESTING 'invalidate()' and 'invalidateAll()"
                          << endl
                          << "============================================"
                          << endl;

        testLoader loader;
        Obj X(&loader, &testAllocator);
        Iterator it;
        bdet_Date date1, date2;
        const bdecs_Calendar *cal, *calValid, *calValid1;

        // try 'invalidate()' on an empty cache

        X.invalidate("VALID");
        ASSERT(X.begin() == X.end());

        // try 'invalidateAll()' on an empty cache

        X.invalidateAll();
        ASSERT(X.begin() == X.end());

        // load two entries into the cache

        cal = X.calendar("VALID");
        ASSERT(0 != cal);
        calValid = cal;
        cal = X.calendar("VALID1");
        ASSERT(0 != cal);
        calValid1 = cal;

        // invalidate a non-exist entry

        X.invalidate("NONEXIST");
        loader.getFirstDate(&date1);
        cal = X.calendar("VALID");
        ASSERT(0 != cal);
        ASSERT(calValid == cal);
        cal = X.calendar("VALID1");
        ASSERT(0 != cal);
        ASSERT(calValid1 == cal);
        loader.getFirstDate(&date2);
        ASSERT(date1 == date2);       // verify both are in the cache

        // invalidate a valid entry

        X.invalidate("VALID");
        loader.getFirstDate(&date1);
        cal = X.calendar("VALID");
        ASSERT(0 != cal);
        ASSERT(calValid == cal);
        loader.getFirstDate(&date2);
        ASSERT(date1 != date2);       // verify "VALID" is reloaded
        loader.getFirstDate(&date1);
        cal = X.calendar("VALID1");
        ASSERT(0 != cal);
        ASSERT(calValid1 == cal);
        loader.getFirstDate(&date2);
        ASSERT(date1 == date2);       // verify "VALID1" is still in cache

        // invalidate another valid entry

        X.invalidate("VALID1");
        loader.getFirstDate(&date1);
        cal = X.calendar("VALID1");
        ASSERT(0 != cal);
        ASSERT(calValid1 == cal);
        loader.getFirstDate(&date2);
        ASSERT(date1 != date2);       // verify "VALID1" is reloaded

        // try 'invalidateAll()' on a non-empty cache

        cal = X.calendar("VALID");
        ASSERT(0 != cal);
        ASSERT(calValid == cal);
        cal = X.calendar("VALID1");
        ASSERT(0 != cal);
        ASSERT(calValid1 == cal);
        X.invalidateAll();
        loader.getFirstDate(&date1);
        cal = X.calendar("VALID");
        ASSERT(0 != cal);
        ASSERT(calValid == cal);
        loader.getFirstDate(&date2);
        ASSERT(date1 != date2);       // verify "VALID" is reloaded
        loader.getFirstDate(&date1);
        cal = X.calendar("VALID1");
        ASSERT(0 != cal);
        ASSERT(calValid1 == cal);
        loader.getFirstDate(&date2);
        ASSERT(date1 != date2);       // verify "VALID1" is reloaded
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING THE 'calendar' METHOD
        //
        // We want to demonstrate that the 'calendar()' method returns a
        // 'bdecs_Calendar' object from the cache when a valid entry with
        // matching 'calendarName' is found; and that it loads the
        // 'bdecs_Calendar' object using the loader and properly inserts it
        // into the cache when a matching 'calendarName' is not found.  We also
        // want to demonstrate that if the calendar cache objects has a timeout
        // value, the expired entries will get reloaded using the loader when
        // they are accessed.
        //
        // Concerns:
        //  1.  The 'calendar()' method returns 0 if an invalid 'calendarName'
        //      is given.
        //  2   The 'calendar()' method returns the properly-loaded
        //      'bdecs_Calendar' object and inserts it into the cache if a
        //      valid 'calendarName' is given but is not found in the cache.
        //  3.  The 'calendar()' method returns the properly-loaded
        //      'bdecs_Calendar' object from the cache if a matching
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
        //  the 'calendar()' method returns the correct 'bdecs_Calendar" object
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
        //  it returns the correct 'bdecs_Calendar' object by checking its
        //  'firstDate()' and holidays.  Then we verify that this
        //  'bdecs_Calendar' object is retrieved from the cache by making sure
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
        //      const bdecs_Calendar *calendar(const char *calendarName);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << " TESTING THE 'calendar()' METHOD" << endl
                          << "================================" << endl;
        testLoader loader;
        Obj X(&loader, &testAllocator);
        Iterator it;
        bdet_Date date1, date2;
        const bdet_Date FirstDate(1,1,1), FirstDate1(1,1,2);
        const bdecs_Calendar *cal, *cal1, *cal0;

        // Concern 1

        if (verbose) cout << endl
                          << "   Testing concern 1" << endl;
        ASSERT(0 == X.calendar(0));
        ASSERT(X.end() == X.begin());
        ASSERT(0 == X.calendar("ERROR"));
        ASSERT(X.end() == X.begin());
        ASSERT(0 == X.calendar("NOTFOUND"));
        ASSERT(X.end() == X.begin());

        // Concern 2 for empty cache

        if (verbose) cout << endl
                          << "  Testing concern 2 (empty cache)" << endl;
        loader.getFirstDate(&date1);
        cal = X.calendar("VALID");
        ASSERT(0 != cal);
        loader.getFirstDate(&date2);
        ASSERT(date1 != date2); // Verify 'load' method is called.
        ASSERT(date1 == cal->firstDate());
        ASSERT(1 == cal->isHoliday(cal->firstDate()+holidayOffset));
        it = X.begin();
        ASSERT(X.end() != it);
        ASSERT("VALID" == it->first);
        ASSERT(cal == &it->second);
        ASSERT(X.end() == ++it);

        // Concern 2 for non-empty cache

        if (verbose) cout << endl
                          << "   Testing concern 2 (non-empty cache)" << endl;
        loader.getFirstDate(&date1);
        cal1 = X.calendar("VALID1");
        ASSERT(0 != cal1);
        loader.getFirstDate(&date2);
        ASSERT(date1 != date2); // Verify 'load' method is called.
        ASSERT(date1 == cal1->firstDate());
        ASSERT(1 == cal1->isHoliday(cal1->firstDate()+holidayOffset));
        it = X.begin();
        ASSERT(X.end() != it);
        ASSERT("VALID" == it->first);
        ASSERT(cal == &it->second);
        ++it;
        ASSERT(X.end() != it);
        ASSERT("VALID1" == it->first);
        ASSERT(cal1 == &it->second);
        ASSERT(X.end() == ++it);

        cal0 = X.calendar("VALID0"); // Insert an entry between them.
        ASSERT(0 != cal0);
        it = X.begin();
        ASSERT(X.end() != it);
        ASSERT("VALID" == it->first);
        ASSERT(cal == &it->second);
        ++it;
        ASSERT(X.end() != it);
        ASSERT("VALID0" == it->first);
        ASSERT(cal0 == &it->second);
        ++it;
        ASSERT(X.end() != it);
        ASSERT("VALID1" == it->first);
        ASSERT(cal1 == &it->second);
        ASSERT(X.end() == ++it);

        // Concern 3

        if (verbose) cout << endl
                          << "   Testing concern 3" << endl;
        loader.getFirstDate(&date1);
        cal = X.calendar("VALID");
        ASSERT(0 != cal);
        loader.getFirstDate(&date2);
        ASSERT(date1 == date2); // Verify 'load' method is not called.
        ASSERT(FirstDate == cal->firstDate());
        ASSERT(1 == cal->isHoliday(cal->firstDate()+holidayOffset));

        cal = X.calendar("VALID1");
        ASSERT(0 != cal);
        loader.getFirstDate(&date2);
        ASSERT(date1 == date2); // Verify 'load' method is not called.
        ASSERT(FirstDate1 == cal->firstDate());
        ASSERT(1 == cal->isHoliday(cal->firstDate()+holidayOffset));

        // Concern 4 and 5

        testLoader loaderY;
        Obj Y(&loaderY, bdet_TimeInterval(4), &testAllocator);
        const bdecs_Calendar *calValid, *calValid1;

        // load first entry into Y

        cal = Y.calendar("VALID");
        ASSERT(0 != cal);
        calValid = cal;

        #ifdef BSLS_PLATFORM__CMP_MSVC
        Sleep(2 * 1000);
        #else
        sleep(2);
        #endif

        // load second entry into Y

        cal = Y.calendar("VALID1");
        ASSERT(0 != cal);
        calValid1 = cal;

        #ifdef BSLS_PLATFORM__CMP_MSVC
        Sleep(1 * 1000);
        #else
        sleep(1);
        #endif

        // now access both entries from cache to verify they still there

        loaderY.getFirstDate(&date1);
        cal = Y.calendar("VALID");
        ASSERT(0 != cal);
        ASSERT(calValid == cal);
        loaderY.getFirstDate(&date2);
        ASSERT(date1 == date2); // Verify 'load' method is not called.
        loaderY.getFirstDate(&date1);
        cal = Y.calendar("VALID1");
        ASSERT(0 != cal);
        ASSERT(calValid1 == cal);
        loaderY.getFirstDate(&date2);
        ASSERT(date1 == date2); // Verify 'load' method is not called.

        #ifdef BSLS_PLATFORM__CMP_MSVC
        Sleep(2 * 1000);
        #else
        sleep(2);
        #endif

        // now only the second entry should be in the cache

        loaderY.getFirstDate(&date1);
        cal = Y.calendar("VALID1");
        ASSERT(0 != cal);
        ASSERT(calValid1 == cal);
        loaderY.getFirstDate(&date2);
        ASSERT(date1 == date2); // Verify 'load' method is not called.
        loaderY.getFirstDate(&date1);
        cal = Y.calendar("VALID");
        ASSERT(0 != cal);
        ASSERT(calValid == cal);
        loaderY.getFirstDate(&date2);
        ASSERT(date1 != date2); // Verify 'load' method is called.

        #ifdef BSLS_PLATFORM__CMP_MSVC
        Sleep(2 * 1000);
        #else
        sleep(2);
        #endif

        // now the second entry should expire


        loaderY.getFirstDate(&date1);
        cal = Y.calendar("VALID1");
        ASSERT(0 != cal);
        ASSERT(calValid1 == cal);
        loaderY.getFirstDate(&date2);
        ASSERT(date1 != date2); // Verify 'load' method is called.

        // verify that after reload, the reload flag is cleared

        loaderY.getFirstDate(&date1);
        cal = Y.calendar("VALID1");
        ASSERT(0 != cal);
        ASSERT(calValid1 == cal);
        loaderY.getFirstDate(&date2);
        ASSERT(date1 == date2); // Verify 'load' method is not called.

        // the entries in X should still be there

        loader.getFirstDate(&date1);
        cal = X.calendar("VALID");
        ASSERT(0 != cal);
        cal = X.calendar("VALID1");
        ASSERT(0 != cal);
        loader.getFirstDate(&date2);
        ASSERT(date1 == date2); // Verify 'load' method is not called.
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING THE ITERATORS
        //
        // Concerns:
        //  1. The class 'bdecs_CalendarCacheIter_PairProxy' properly proxies a
        //     'bdecs_CalendarCache_Pair' object.
        //  2. 'begin()' returns the first element if it exists and
        //     'end()' returns one element past the last one.  If the
        //     container is empty, 'begin()' must be equal to 'end()'.
        //  3. 'operator++()' and 'operator--()' move the iterator
        //     forward/backward by one element.  'operator=()' assigns an
        //     iterator to another one of the same type and returns a reference
        //     to the iterator.
        //  4. 'operator*()' and 'operator->()' return the actual value
        //     associated with the iterator.  If the entry has expired or is
        //     marked for reload, it is reloaded using its loader.
        //  5. 'operator==()' and 'operator!=()' compare the values of
        //     the two iterators and return the expected result.
        //  6. The reverse iterators are working properly.
        //
        //  To address concern 1, we will construct a
        //  'bdecs_CalendarCacheIter_PairProxy' object based on a pre-selected
        //  'bdecs_CalendarCache_Pair' in order to test the constructor
        //  'bdecs_CalendarCacheIter_PairProxy(const bdecs_CalendarCache_Pair&
        //  pair);'.  Then we will create another pair proxy object based on
        //  the first object in order to test the copy constructor.  We will
        //  then compare the pre-selected 'bdecs_CalendarCache_Pair' object
        //  with the 'bdecs_CalendarCache_Pair' objects returned by
        //  'operator->()' of these two objects and make sure they all match.
        //  We will also use 'operator->()' to access one of the data members
        //  of the 'bdecs_CalendarCache_Pair' class and verify that the return
        //  value matches the value obtained by accessing the data member
        //  through 'bdecs_CalendarCache_Pair'.
        //
        //  To address concern 2, we first start with an empty calendar cache
        //  and verify that 'begin' equals 'end'.  We then insert a set of
        //  calendars in alphabetical order and make sure 'begin' always
        //  references the first calendar and 'end' always references the new
        //  calendar being loaded.  Finally we insert a set of calendars into
        //  another calendar cache object in reverse-alphabetical order and
        //  make sure 'begin' always references the new calendar being loaded
        //  and 'end' always references the element pass the last calendar.
        //
        //  To address concerns 3, and 4, we load calendars into the cache, use
        //  'begin', 'end', 'operator++', and 'operator--' to move back and
        //  forth, and use 'operator*' and 'operator->' to verify the content
        //  of the entry.  Finally we invalidate all entries in the cache and
        //  use an iterator and 'operator*' to access them again and verify all
        //  the entries are reloaded via the test loader.
        //
        //  To address concern 5, we will first compare an iterator to itself.
        //  Then we will compare it with another iterator that points to the
        //  same entry and make sure they are equal.  Finally we will compare
        //  it with an iterator which points to a different entry and make sure
        //  they are not equal.
        //
        //  To address concern 6, we will compare the results returned by the
        //  reverse iterators with the results returned by the forward
        //  iterators moving backwards and make sure they are identical.  It is
        //  not necessary to apply all the tests for the forward iterators to
        //  these reverse iterators because they are implemented as the
        //  'bsl::reverse_iterator<>' version of the forward iterators.
        //
        //  Tactics:
        //      - Ad Hoc test data selection method
        //      - Brute force/loop based test case implementation technique
        //
        //  Testing:
        //      bdecs_CalendarCache::ConstIterator
        //      bdecs_CalendarCache::ConstReverseIterator
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING THE ITERATOR" << endl
                          << "====================" << endl;

        {
            // Testing the 'bdecs_CalendarCacheIter_PairProxy' class

            if (verbose) cout << endl
                              << "Testing 'bdecs_CalendarCacheIter_PairProxy'"
                              << endl
                              << "==========================================="
                              << endl;

            testLoader loader;
            const bdecs_Calendar *cal;
            Obj X(&loader, &testAllocator);
            bsl::string name("VALID");
            cal = X.calendar(name.c_str());
            ASSERT(0 != cal);
            bdecs_CalendarCache_Pair pair(name, *cal);
            bdecs_CalendarCacheIter_PairProxy proxy1(pair);
            bdecs_CalendarCacheIter_PairProxy proxy2(proxy1);
            ASSERT(pair == *(proxy1.operator->()));
            ASSERT(pair == *(proxy2.operator->()));

            ASSERT(pair.first == proxy1->first);
            ASSERT(pair.first == proxy2->first);
        }
        {
            typedef bdecs_CalendarCache_Pair          IteratorValue;
            typedef int                               IteratorDifference;
            typedef bdecs_CalendarCacheIter_PairProxy IteratorPointer;
            typedef bdecs_CalendarCache_PairRef       IteratorReference;

            typedef bsl::bidirectional_iterator_tag   IteratorCategory;
            // *** TYPEDEF TESTS ***

            if (veryVerbose) cout << "\ttypedef test" << endl;
            {
                typedef bsl::iterator_traits<Iterator> IterTraits;
                ASSERT((IsSame<IterTraits::value_type, IteratorValue>::VALUE));
                ASSERT((IsSame<IterTraits::difference_type,
                                                  IteratorDifference>::VALUE));
                ASSERT((IsSame<IterTraits::pointer, IteratorPointer>::VALUE));
                ASSERT((IsSame<IterTraits::reference,
                                                   IteratorReference>::VALUE));
                ASSERT((IsSame<IterTraits::iterator_category,
                                                    IteratorCategory>::VALUE));
            }
        }
        {
            // Testing 'begin' and 'end'

            if (verbose) cout << endl
                              << "Testing 'begin' and 'end'" << endl
                              << "=========================" << endl;

            testLoader loader;
            const bdecs_Calendar *cal;
            const char *DATA[] = {
                                 "AAAA", "CH", "EN", "FR", "IS", "US", "ZZZZ"};
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            {
                Obj X(&loader, &testAllocator);

                ASSERT(X.begin() == X.end());
                ASSERT(X.begin() == X.begin());
                ASSERT(X.end()   == X.end());

                for (int i = 0; i < NUM_DATA; ++i) {
                    cal = X.calendar(DATA[i]);
                    LOOP_ASSERT(i, 0 != cal);
                    LOOP2_ASSERT(i, X.begin()->first, DATA[0] ==
                                                             X.begin()->first);
                    Iterator it = X.end();
                    --it;
                    LOOP3_ASSERT(i, DATA[i], it->first, DATA[i] == it->first);
                }
            }
            {
                Obj X(&loader, &testAllocator);

                for (int i = NUM_DATA - 1; i >= 0; --i) {
                    cal = X.calendar(DATA[i]);
                    LOOP_ASSERT(i, 0 != cal);
                    LOOP3_ASSERT(i, DATA[i], X.begin()->first, DATA[i] ==
                                                             X.begin()->first);
                    Iterator it = X.end();
                    --it;
                    LOOP2_ASSERT(i, it->first, DATA[NUM_DATA - 1] ==
                                                                    it->first);
                }
            }
        }
        {
            // Testing 'operator++()', 'operator--()', 'operator*()', and
            // 'operator->()'

            if (verbose) cout << endl
                              << "Testing 'begin()', 'end()',        " << endl
                              << "  'operator++()', 'operator--()',  " << endl
                              << "  'operator*()', and 'operator->()'" << endl
                              << "===================================" << endl;

            const char *DATA[] = {
                                 "AAAA", "CH", "EN", "FR", "IS", "US", "ZZZZ"};
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            testLoader loader;
            Obj X(&loader, &testAllocator);
            bdet_Date date1, date2;
            const bdecs_Calendar *cal1, *cal2;

            // *** NON-MUTATING FORWARD ITERATOR TESTS ***

            if (veryVerbose) cout << "\tnon-mutating forward iterator "
                                  << "tests" << endl;

            // Breathing test

            ASSERT(X.begin() == X.end());
            ASSERT(X.begin() == X.begin());
            ASSERT(X.end()   == X.end());

            for (int nb = 0; nb < NUM_DATA; ++nb) {
                for (int j = 0; j < nb; ++j) {
                    cal1 = X.calendar(DATA[j]);
                    ASSERT(0 != cal1);
                }
                LOOP_ASSERT(nb, X.begin() == X.begin());
                LOOP_ASSERT(nb, X.end() == X.end());
                LOOP_ASSERT(nb, !nb || X.begin() != X.end());
                LOOP_ASSERT(nb,  nb || X.begin() == X.end());

                // Primary manipulators and accessors test
                // pre-increment, deference, copy constructor, 'operator==()'
                // test

                {
                    Iterator i = X.begin();
                    for (int c = 0; c < nb; ++c) {
                        LOOP2_ASSERT(nb, c, !(i == X.end()));
                        LOOP4_ASSERT(nb, c, DATA[c], i->first,
                                                          DATA[c] == i->first);

                        // copy construction

                        Iterator j(i);
                        LOOP2_ASSERT(nb, c, j != X.end());
                        LOOP4_ASSERT(nb, c, DATA[c], j->first,
                                                          DATA[c] == j->first);

                        // 'operator++()'

                        Iterator k(++i);
                        if (c < (nb - 1)) {
                            LOOP2_ASSERT(nb, c, *i == *k);
                            LOOP2_ASSERT(nb, c, *i != *j);
                        }
                        else {
                            LOOP2_ASSERT(nb, c, i == X.end());
                            LOOP2_ASSERT(nb, c, k == X.end());
                        }
                    }
                    LOOP_ASSERT(nb, i == X.end());
                }

                // 'operator==()' and 'operator!=()'

                {
                    // Note that in the double loop below, each loop runs 'nb'
                    // + 1 times because we compare the 'nb' iterators
                    // referencing the 'nb' holidays plus the end iterator.

                    Iterator i = X.begin();
                    for (int c = 0; c <= nb; ++c) {
                        Iterator j = X.begin();
                        for (int d = 0; d <= nb; ++d) {

                            // The two tests are necessary because
                            // 'operator!=()' is NOT implemented as "return
                            // !(lhs == rhs);".

                            if (c == d) {
                                LOOP3_ASSERT(nb, c, d, i == j);
                                LOOP3_ASSERT(nb, c, d, !(i != j));
                            }
                            else {
                                LOOP3_ASSERT(nb, c, d, i != j);
                                LOOP3_ASSERT(nb, c, d, !(i == j));
                            }
                            if (d < nb)
                                ++j;
                        }
                        if (c < nb)
                            ++i;
                    }
                }

                // assignment operator

                {
                    Iterator i = X.begin();
                    for (int c = 0; c < nb; ++c) {
                        Iterator j = X.begin();
                        for (int d = 0; d < nb; ++d) {
                            Iterator k(i);

                            i = i;
                            LOOP3_ASSERT(nb, c, d, k == i);

                            k = j;
                            LOOP3_ASSERT(nb, c, d, k == j);
                            ++j;
                        }
                        ++i;
                    }
                }

                // 'operator->()'

                Iterator i;
                for (i = X.begin(); i != X.end(); ++i) {
                    LOOP_ASSERT(nb, (*i).first == i->first);
                }

                // post-increment operator test

                i = X.begin();
                for (int c = 0; c < nb; ++c) {
                    LOOP2_ASSERT(nb, c, i != X.end());
                    LOOP4_ASSERT(nb, c, DATA[c], i->first, DATA[c] ==
                                                                 (i++)->first);
                }
                LOOP_ASSERT(nb, i == X.end());

                // *** NON-MUTATING BIDIRECTIONAL ITERATOR TESTS ***

                if (veryVerbose) cout << "\tnon-mutating bidirectional "
                                      << "iterator tests" << endl;

                // pre-decrement operator test

                i = X.end();
                for (int c = (nb - 1); c >= 0; --c) {
                    Iterator j = --i;
                    LOOP4_ASSERT(nb, c, DATA[c], i->first,
                                                          DATA[c] == i->first);
                    LOOP2_ASSERT(nb, c, j == i);
                }

                // post-decrement operator test

                i = X.end();
                if (i != X.begin())
                    --i;
                for (int c = (nb - 1); c > 0; --c) {
                    LOOP4_ASSERT(nb, c, DATA[c], i->first, DATA[c] ==
                                                                 (i--)->first);
                }

                // Test last value, if size > 0

                LOOP_ASSERT(nb, !nb || i->first == DATA[0]);

                // *** REVERSE ITERATOR TESTS ***
                // The following tests are very simple because
                // 'HolidayReverseIterator' is implemented using a TESTED
                // bsl::reserse_iterator<Iterator>-like template.

                if (veryVerbose) cout << "\treverse iterator tests" << endl;

                typedef Obj::ConstReverseIterator ReverseIterator;

                if (nb == 0) {
                    LOOP_ASSERT(nb, X.rbegin() == X.rend());
                }
                else {
                    ReverseIterator ri;

                    for (i = X.end(), ri = X.rbegin(); ri != X.rend(); ++ri) {
                        --i;
                        LOOP_ASSERT(nb, *i == *ri);
                        LOOP_ASSERT(nb, i->first == ri->first);
                    }
                    LOOP_ASSERT(nb, X.begin() == i);

                    for (ri = X.rend(), i = X.begin(); i != X.end(); ++i) {
                        --ri;
                        LOOP_ASSERT(nb, *i == *ri);
                        LOOP_ASSERT(nb, i->first == ri->first);
                    }
                    LOOP_ASSERT(nb, X.rbegin() == ri);
                }
            }
            {
                // Verify 'operator*' causes a reload.

                X.invalidateAll();
                int index = 0;
                for (Iterator it = X.begin(); it != X.end(); ++it, ++index) {
                    loader.getFirstDate(&date1);

                    //'operator*' below will cause a reload

                    ASSERT(DATA[index] == (*it).first);

                    loader.getFirstDate(&date2);
                    ASSERT(date1 != date2);
                }
            }
        }
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
        testLoader loader;
        Obj X(&loader);

        ASSERT(0 == X.calendar(0));
        ASSERT(0 == X.calendar("ERROR"));
        ASSERT(0 == X.calendar("NOTFOUND"));
        ASSERT(0 != X.calendar("VALID"));

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
