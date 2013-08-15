// bdet_defaultcalendarcache.t.cpp                                    -*-C++-*-
#include <bdet_defaultcalendarcache.h>

#include <bdet_calendarcache.h>
#include <bdet_calendarloader.h>  // for testing only
#include <bdet_date.h>            // for testing only

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>     // 'atoi'
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
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The utility under test has an especially simple interface and a similarly
// straightforward implementation.  The primary concerns are that the allocator
// passed to 'initialize' is hooked up properly and that the 'initialize' and
// 'destroy' methods are thread-safe.
//
// Global Concerns:
//: o Precondition violations are detected in appropriate build modes.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] static int initialize(Loader *loader, Allocator *allocator);
// [ 2] static void destroy();
// [ 2] static bdet_CalendarCache *instance();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE
// [ 3] CONCERN: 'initialize' and 'destroy' are thread-safe.

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdet_DefaultCalendarCache  Util;

typedef bdet_CalendarCache         Cache;
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

namespace TestCase3 {

struct ThreadInfo {
    int                  d_numIterations;
    bdet_CalendarLoader *d_loader_p;
    bslma::Allocator    *d_allocator_p;
};

extern "C" void *threadFunctionA(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    for (int i = 0; i < info->d_numIterations; ++i) {
        Util::initialize(info->d_loader_p, info->d_allocator_p);
        Util::destroy();
    }

    return arg;
}

extern "C" void *threadFunctionB(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    for (int i = 0; i < info->d_numIterations; ++i) {
        Util::destroy();
        Util::initialize(info->d_loader_p, info->d_allocator_p);
    }

    return arg;
}

extern "C" void *threadFunctionC(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    for (int i = 0; i < info->d_numIterations; ++i) {
        Util::initialize(info->d_loader_p, info->d_allocator_p);
        Util::destroy();
        Util::initialize(info->d_loader_p, info->d_allocator_p);
    }

    return arg;
}

}  // close namspace TestCase3

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

// Use 'my_CalendarLoader' in other test cases.

typedef my_CalendarLoader TestLoader;

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

    // CONCERN: This test driver is reusable w/other, similar components.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 4: {
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

///Usage
///-----
// The following example illustrates how to use 'bdet_DefaultCalendarCache'.
//
///Example 1: Using 'bdet_DefaultCalendarCache'
///- - - - - - - - - - - - - - - - - - - - - -
// 'bdet_DefaultCalendarCache' has a particularly simple interface.  This
// example shows how to use each of its three methods.
//
// A hypothetical calendar loader is assumed, 'my_CalendarLoader', the details
// of which are not important other than that it supports calendars identified
// by "DE", "FR", and "US", which nominally identify the major holidays in
// Germany, France, and the United States, respectively.  Furthermore, we cite
// two specific dates of interest: 2011/07/04, which was a holiday in the US
// (Independence Day), but not in France, and 2011/07/14, which was a holiday
// in France (Bastille Day), but not in the US.
//
// First, we create a calendar loader, an instance of 'my_CalendarLoader', and
// use it, in turn, to initialize the default calendar cache.  A memory
// allocator must also be explicitly supplied to the 'initialize' method.  The
// global allocator is suitable in this case (see 'bslma_default'):
//..
    static my_CalendarLoader loader;

    bdet_DefaultCalendarCache::initialize(&loader,
                                          bslma::Default::globalAllocator());
//..
// Note that declaring 'loader' to be 'static' ensures that it remains valid
// until the cache is destroyed.  Also note that initialization of the cache
// would typically be done in 'main' before other threads have been created.
//
// Next, we obtain the address of the default calendar cache using the
// 'instance' class method:
//..
    bdet_CalendarCache *cachePtr = bdet_DefaultCalendarCache::instance();
    ASSERT(cachePtr);
//..
// Next, we retrieve the calendar identified by "US" from the default cache,
// and verify that 2011/07/04 is recognized as a holiday in the "US" calendar,
// whereas 2011/07/14 is not:
//..
    bdet_CalendarCacheEntryPtr us = cachePtr->getCalendar("US");
    ASSERT( us->isHoliday(bdet_Date(2011, 7,  4)));
    ASSERT(!us->isHoliday(bdet_Date(2011, 7, 14)));
//..
// Next, we fetch the calendar identified by "FR", this time verifying that
// 2011/07/14 is recognized as a holiday in the "FR" calendar, but 2011/07/04
// is not:
//..
    bdet_CalendarCacheEntryPtr fr = cachePtr->getCalendar("FR");
    ASSERT(!fr->isHoliday(bdet_Date(2011, 7,  4)));
    ASSERT( fr->isHoliday(bdet_Date(2011, 7, 14)));
//..
// Finally, we destroy the default calendar cache:
//..
    bdet_DefaultCalendarCache::destroy();
    ASSERT(!bdet_DefaultCalendarCache::instance());
//..
// Note that destruction of the default cache would typically be done in 'main'
// just prior to program termination.

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST
        //   Ensure that 'initialize' and 'destroy' can be executed
        //   concurrently.
        //
        // Concerns:
        //: 1 That 'initialize' and 'destroy' are thread-safe.
        //
        // Plan:
        //: 1 Create a test loader and a test allocator, both of which are used
        //:   to initialize the default calendar cache.
        //:
        //: 2 Create three threads, 'A', 'B', and 'C', each of which iterates
        //:   'N' times.  During each iteration, thread 'A' first calls
        //:   'initialize', then calls 'destroy'; thread 'B' first calls
        //:   'destroy', then calls 'initialize'; thread 'C' calls, in order,
        //:   'initialize', 'destroy', 'initialize'.
        //:
        //: 3 Join with threads 'A', 'B', and 'C'.
        //:
        //: 4 If the default calendar cache is in the initialized state after
        //:   the join, verify that a calendar can be successfully accessed
        //:   from the cache, then destroy the cache.
        //:
        //: 5 Repeat P-2..4 'N' times.  (C-1)
        //
        // Testing:
        //   CONCERN: 'initialize' and 'destroy' are thread-safe.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONCURRENCY TEST" << endl
                          << "================" << endl;

        using namespace TestCase3;

        TestLoader           mL;
        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        const int NUM_ITERATIONS = 10;

        ThreadInfo info = { NUM_ITERATIONS, &mL, &sa };

        for (int ti = 0; ti < NUM_ITERATIONS; ++ti) {
            ThreadId idA = createThread(&threadFunctionA, &info);
            ThreadId idB = createThread(&threadFunctionB, &info);
            ThreadId idC = createThread(&threadFunctionC, &info);

            joinThread(idA);
            joinThread(idB);
            joinThread(idC);

            Cache *cachePtr = Util::instance();

            if (cachePtr) {
                if (veryVerbose) {
                  P_(ti)
                  cout << "Cache  initialized after join." << endl;
                }

                Entry usA = cachePtr->getCalendar("US");

                ASSERT( usA.ptr());
                ASSERT( usA->isHoliday(bdet_Date(2011, 7,  4)));
                ASSERT(!usA->isHoliday(bdet_Date(2011, 7, 14)));

                Util::destroy();
            }
            else {
                if (veryVerbose) {
                  P_(ti)
                  cout << "Cache !initialized after join." << endl;
                }
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // METHODS TEST
        //   Ensure that the three interface methods work as expected in the
        //   absence of concurrency.
        //
        // Concerns:
        //: 1 That the allocator passed to 'initialize' is hooked up correctly.
        //:
        //: 2 That 'instance' returns non-zero if and only if the default
        //:   calendar cache is in the initialized state.
        //:
        //: 3 That 'destroy' indeed destroys the default calendar cache, with
        //:   associated memory being reclaimed by the allocator supplied to
        //:   'initialize'.
        //:
        //: 4 That the default calendar cache can be initialized and destroyed
        //:   multiple times.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a test calendar loader and three test allocators.  Set one
        //:   allocator to be the default allocator and another allocator to be
        //:   the global allocator.
        //:
        //: 2 Supply the loader and the third allocator to the 'initialize'
        //:   method.  Verify that there is no memory allocated from either
        //:   allocator, and that the 'instance' method returns the expected
        //:   result before and after initialization.
        //:
        //: 3 Access a calendar from the default cache and verify that memory
        //:   use from the three allocators created in P-1 is as expected.
        //:   (C-1)
        //:
        //: 4 Destroy the default cache.  Verify that memory is reclaimed by
        //:   the supplied allocator, and that 'instance' returns the expected
        //:   result before and after destruction.  (C-2..3)
        //:
        //: 5 Repeat P-2..4.  (C-4)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered (using the 'BSLS_ASSERTTEST_*' macros).  (C-5)
        //
        // Testing:
        //   static int initialize(Loader *loader, Allocator *allocator);
        //   static void destroy();
        //   static bdet_CalendarCache *instance();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "METHODS TEST" << endl
                          << "============" << endl;

        TestLoader           mL;

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        bslma::TestAllocator ga("global",   veryVeryVeryVerbose);
        bslma::Default::setGlobalAllocator(&ga);

        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        // "initialize, access, destroy" for the first time
        {
                                     ASSERT(!Util::instance());
            Util::initialize(&mL, &sa);
                                     ASSERT( Util::instance());
                                     ASSERT(0 == da.numBlocksTotal());
                                     ASSERT(0 == ga.numBlocksTotal());
                                     ASSERT(0 == sa.numBlocksTotal());

            {
                Cache *cachePtr = Util::instance();

                Entry usA = cachePtr->getCalendar("US");

                ASSERT( usA.ptr());
                ASSERT( usA->isHoliday(bdet_Date(2011, 7,  4)));
                ASSERT(!usA->isHoliday(bdet_Date(2011, 7, 14)));

                                     ASSERT(0 == da.numBlocksInUse());
                                     ASSERT(0 == ga.numBlocksTotal());
                                     ASSERT(0 != sa.numBlocksInUse());
            }

                                     ASSERT( Util::instance());
            Util::destroy();
                                     ASSERT(!Util::instance());
                                     ASSERT(0 == da.numBlocksInUse());
                                     ASSERT(0 == ga.numBlocksTotal());
                                     ASSERT(0 == sa.numBlocksInUse());
        }

        // "initialize, access, destroy" a second time
        {
                                     ASSERT(!Util::instance());
            Util::initialize(&mL, &sa);
                                     ASSERT( Util::instance());
                                     ASSERT(0 == da.numBlocksInUse());
                                     ASSERT(0 == ga.numBlocksTotal());
                                     ASSERT(0 == sa.numBlocksInUse());

            {
                Cache *cachePtr = Util::instance();

                Entry usA = cachePtr->getCalendar("US");

                ASSERT( usA.ptr());
                ASSERT( usA->isHoliday(bdet_Date(2011, 7,  4)));
                ASSERT(!usA->isHoliday(bdet_Date(2011, 7, 14)));

                                     ASSERT(0 == da.numBlocksInUse());
                                     ASSERT(0 == ga.numBlocksTotal());
                                     ASSERT(0 != sa.numBlocksInUse());
            }

                                     ASSERT( Util::instance());
            Util::destroy();
                                     ASSERT(!Util::instance());
                                     ASSERT(0 == da.numBlocksInUse());
                                     ASSERT(0 == ga.numBlocksTotal());
                                     ASSERT(0 == sa.numBlocksInUse());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            TestLoader           mL;
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            ASSERT_SAFE_FAIL(Util::initialize(&mL,   0));
            ASSERT_SAFE_FAIL(Util::initialize(  0, &sa));

            ASSERT(!Util::instance());
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
        //: 1 Create a test calendar loader and a test allocator, and supply
        //:   them to the 'initialize' method.  Verify the expected result of
        //:   the 'instance' method before and after initialization of the
        //:   default calendar cache.  Also verify that no memory is allocated
        //:   from the supplied allocator.
        //:
        //: 2 Obtain the address of the default cache via the 'instance' method
        //:   and verify that a calendar may be accessed from the cache.  Also
        //:   verify that the test allocator that was supplied to 'initialize'
        //:   in P-1 now has outstanding memory.
        //:
        //: 3 Invoke 'destroy'.  Verify that 'instance' now returns 0 and that
        //:   all memory has been reclaimed by the test allocator that was
        //:   supplied to 'initialize' in P-1.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // Initialize: P-1

        TestLoader           mL;
        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                                     ASSERT(!Util::instance());
        Util::initialize(&mL, &sa);  ASSERT( Util::instance());
                                     ASSERT(0 == sa.numBlocksTotal());

        // Access: P-2

        {
            Cache *cachePtr = Util::instance();

            Entry usA = cachePtr->getCalendar("US");

            ASSERT( usA.ptr());
            ASSERT( usA->isHoliday(bdet_Date(2011, 7,  4)));
            ASSERT(!usA->isHoliday(bdet_Date(2011, 7, 14)));

                                     ASSERT(0 != sa.numBlocksInUse());
        }

        // Destroy: P-3

        Util::destroy();             ASSERT(!Util::instance());
                                     ASSERT(0 == sa.numBlocksInUse());

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

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
