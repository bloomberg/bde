// bdlt_defaulttimetablecache.t.cpp                                   -*-C++-*-
#include <bdlt_defaulttimetablecache.h>

#include <bdlt_timetablecache.h>
#include <bdlt_timetableloader.h>
#include <bdlt_date.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>      // 'atoi'
#include <bsl_cstring.h>      // 'strcmp'
#include <bsl_iostream.h>
#include <bsl_memory.h>       // 'bsl::shared_ptr'

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
// [ 2] static void destroy();
// [ 2] static int initialize(Loader *loader,          Allocator *alloc);
// [ 2] static int initialize(Loader *loader, timeout, Allocator *alloc);
// [ 2] static TimetableCache *instance();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE
// [ 3] CONCERN: 'initialize' and 'destroy' are thread-safe.

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

typedef bdlt::DefaultTimetableCache            Util;

typedef bdlt::TimetableCache                   Cache;
typedef bsl::shared_ptr<const bdlt::Timetable> Entry;

typedef bsls::TimeInterval                     Interval;

#ifdef BSLS_PLATFORM_OS_WINDOWS
typedef HANDLE    ThreadId;
#else
typedef pthread_t ThreadId;
#endif

extern "C" {
    typedef void *(*ThreadFunction)(void *arg);
}

// ============================================================================
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

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

namespace TestCase3 {

struct ThreadInfo {
    int                    d_numIterations;
    bdlt::TimetableLoader *d_loader_p;
    bslma::Allocator      *d_allocator_p;
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
        Util::initialize(info->d_loader_p, Interval(30), info->d_allocator_p);
    }

    return arg;
}

extern "C" void *threadFunctionC(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    for (int i = 0; i < info->d_numIterations; ++i) {
        Util::initialize(info->d_loader_p, info->d_allocator_p);
        Util::destroy();
        Util::initialize(info->d_loader_p, Interval(30), info->d_allocator_p);
    }

    return arg;
}

}  // close namespace TestCase3

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

// Define the 'MyTimetableLoader' class that is used in the Usage example.

class MyTimetableLoader : public bdlt::TimetableLoader {

  private:
    // NOT IMPLEMENTED
    MyTimetableLoader(const MyTimetableLoader&);
    MyTimetableLoader& operator=(const MyTimetableLoader&);

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

inline
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

// Use 'MyTimetableLoader' in other test cases.

typedef MyTimetableLoader TestLoader;

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

    (void)veryVeryVerbose;  // not used

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
// The following example illustrates how to use 'bdlt::DefaultTimetableCache'.
//
///Example 1: Using 'bdlt::DefaultTimetableCache'
///- - - - - - - - - - - - - - - - - - - - - - -
// 'bdlt::DefaultTimetableCache' has a particularly simple interface.  This
// example shows how to use each of its three methods.
//
// In this example, we assume a hypothetical timetable loader,
// 'MyTimetableLoader', the details of which are not important other than that
// it supports timetables identified by "ZERO", "ONE", and "TWO".  Furthermore,
// the value of the initial transition code for each of these timetables is
// given by the timetable's name (e.g., if 'Z' has the value of the timetable
// identified as "ZERO", then '0 == Z.initialTransitionCode()').
//
// First, we create a timetable loader, an instance of 'MyTimetableLoader', and
// use it, in turn, to initialize the default timetable cache.  A memory
// allocator must also be explicitly supplied to the 'initialize' method.  The
// global allocator is suitable in this case (see 'bslma_default'):
//..
    static MyTimetableLoader loader;

    int rc = bdlt::DefaultTimetableCache::initialize(
                                            &loader,
                                            bslma::Default::globalAllocator());
    ASSERT(!rc);
//..
// Note that declaring 'loader' to be 'static' ensures that it remains valid
// until the cache is destroyed.  Also note that initialization of the cache
// would typically be done in 'main' before other threads have been created.
//
// Next, we obtain the address of the default timetable cache using the
// 'instance' class method:
//..
    bdlt::TimetableCache *cachePtr = bdlt::DefaultTimetableCache::instance();
    ASSERT(cachePtr);
//..
// Then, we retrieve the timetable identified by "TWO" from the default cache
// and verify that 2 is the value of the initial transition code:
//..
    bsl::shared_ptr<const bdlt::Timetable> two = cachePtr->getTimetable("TWO");
    ASSERT(2 == two->initialTransitionCode());
//..
// Next, we fetch the timetable identified by "ONE", this time verifying that 1
// is the value of the initial transition code for the "ONE" timetable:
//..
    bsl::shared_ptr<const bdlt::Timetable> one = cachePtr->getTimetable("ONE");
    ASSERT(1 == one->initialTransitionCode());
//..
// Finally, we destroy the default timetable cache:
//..
    bdlt::DefaultTimetableCache::destroy();
    ASSERT(!bdlt::DefaultTimetableCache::instance());
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
        //:   to initialize the default timetable cache.
        //:
        //: 2 Create three threads, 'A', 'B', and 'C', each of which iterates
        //:   'N' times.  During each iteration, thread 'A' first calls
        //:   'initialize', then calls 'destroy'; thread 'B' first calls
        //:   'destroy', then calls 'initialize'; thread 'C' calls, in order,
        //:   'initialize', 'destroy', 'initialize'.
        //:
        //: 3 Join with threads 'A', 'B', and 'C'.
        //:
        //: 4 If the default timetable cache is in the initialized state after
        //:   the join, verify that a timetable can be successfully accessed
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

        const int NUM_TEST_ITERATIONS   =   10;
        const int NUM_THREAD_ITERATIONS = 1000;

        ThreadInfo info = { NUM_THREAD_ITERATIONS, &mL, &sa };

        for (int ti = 0; ti < NUM_TEST_ITERATIONS; ++ti) {
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

                Entry e = cachePtr->getTimetable("TWO");

                ASSERT(e.get());
                ASSERT(2 == e->initialTransitionCode());

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
        //   Ensure that the four interface methods work as expected in the
        //   absence of concurrency.
        //
        // Concerns:
        //: 1 That the allocator passed to 'initialize' is hooked up correctly.
        //:
        //: 2 That 'initialize' returns the expected status value.
        //:
        //: 3 That a cache timeout is in effect if any only if one is supplied
        //:   to 'initialize'.
        //:
        //: 4 That 'instance' returns non-zero if and only if the default
        //:   timetable cache is in the initialized state.
        //:
        //: 5 That the address returned by 'instance' provides modifiable
        //:   access to the default timetable cache.
        //:
        //: 6 That 'destroy' indeed destroys the default timetable cache, with
        //:   associated memory being reclaimed by the allocator supplied to
        //:   'initialize'.
        //:
        //: 7 That the default timetable cache can be initialized and destroyed
        //:   multiple times.
        //:
        //: 8 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a test timetable loader and three test allocators.  Set
        //:   one allocator to be the default allocator and another allocator
        //:   to be the global allocator.
        //:
        //: 2 Supply the loader and the third allocator, but no timeout, to the
        //:   'initialize' method.  Verify that there is no memory allocated
        //:   from either allocator, that 'initialize' returns the expected
        //:   value, and that the 'instance' method returns the expected result
        //:   before and after initialization.
        //:
        //: 3 Access a timetable from the default cache and verify that memory
        //:   use from the three allocators created in P-1 is as expected.
        //:
        //: 4 Call 'initialize' again and verify that it returns the expected
        //:   value.
        //:
        //: 5 Destroy the default cache.  Verify that memory is reclaimed by
        //:   the supplied allocator, and that 'instance' returns the expected
        //:   result after destruction.
        //:
        //: 6 Repeat P-2..5, this time supplying a timeout value to the
        //:   'initialize' method.  (C-1..2, C-4..7)
        //:
        //: 7 Provide a separate test for the timeout feature.  (C-3)
        //:
        //: 8 Verify that, in appropriate build modes, defensive checks are
        //:   triggered (using the 'BSLS_ASSERTTEST_*' macros).  (C-8)
        //
        // Testing:
        //   static void destroy();
        //   static int initialize(Loader *loader,          Allocator *alloc);
        //   static int initialize(Loader *loader, timeout, Allocator *alloc);
        //   static TimetableCache *instance();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "METHODS TEST" << endl
                          << "============" << endl;

        TestLoader mL;

        bslma::TestAllocator         da("default",  veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        bslma::TestAllocator         ga("global",   veryVeryVeryVerbose);
        bslma::Default::setGlobalAllocator(&ga);

        bslma::TestAllocator         sa("supplied", veryVeryVeryVerbose);

        int rc;  // for 'initialize' return status

        // "initialize (without timeout), access, destroy" for the first time
        {
                                     ASSERT(!Util::instance());

            rc = Util::initialize(&mL, &sa);

                                     ASSERT(!rc);
                                     ASSERT( Util::instance());
                                     ASSERT(0 == da.numBlocksTotal());
                                     ASSERT(0 == ga.numBlocksTotal());
                                     ASSERT(0 == sa.numBlocksTotal());

            {
                      Cache *cachePtr      = Util::instance();
                const Cache *constCachePtr = cachePtr;

                Entry e = cachePtr->getTimetable("TWO");

                ASSERT(e.get());
                ASSERT(2 == e->initialTransitionCode());

                                     ASSERT(0 == da.numBlocksInUse());
                                     ASSERT(0 == ga.numBlocksTotal());
                                     ASSERT(0 != sa.numBlocksInUse());

                e = constCachePtr->lookupTimetable("TWO");

                ASSERT(e.get());
            }
                                     ASSERT( Util::instance());

            rc = Util::initialize(&mL, &sa);

                                     ASSERT( rc);
                                     ASSERT( Util::instance());

            Util::destroy();
                                     ASSERT(!Util::instance());
                                     ASSERT(0 == da.numBlocksInUse());
                                     ASSERT(0 == ga.numBlocksTotal());
                                     ASSERT(0 == sa.numBlocksInUse());
        }

        // "initialize (*with* timeout), access, destroy" a second time
        {
                                     ASSERT(!Util::instance());

            rc = Util::initialize(&mL, Interval(30), &sa);

                                     ASSERT(!rc);
                                     ASSERT( Util::instance());
                                     ASSERT(0 == da.numBlocksInUse());
                                     ASSERT(0 == ga.numBlocksTotal());
                                     ASSERT(0 == sa.numBlocksInUse());

            {
                      Cache *cachePtr      = Util::instance();
                const Cache *constCachePtr = cachePtr;

                Entry e = cachePtr->getTimetable("TWO");

                ASSERT(e.get());
                ASSERT(2 == e->initialTransitionCode());

                                     ASSERT(0 == da.numBlocksInUse());
                                     ASSERT(0 == ga.numBlocksTotal());
                                     ASSERT(0 != sa.numBlocksInUse());

                e = constCachePtr->lookupTimetable("TWO");

                ASSERT( e.get());
            }
                                     ASSERT( Util::instance());

            rc = Util::initialize(&mL, Interval(30), &sa);

                                     ASSERT( rc);
                                     ASSERT( Util::instance());

            Util::destroy();
                                     ASSERT(!Util::instance());
                                     ASSERT(0 == da.numBlocksInUse());
                                     ASSERT(0 == ga.numBlocksTotal());
                                     ASSERT(0 == sa.numBlocksInUse());
        }

        // Ensure that the timeout parameter is passed to the underlying
        // 'bdlt::TimetableCache' correctly.

        // First 'initialize' without a timeout.
        {
            rc = Util::initialize(&mL, &sa);               ASSERT(!rc);

            {
                      Cache *cachePtr      = Util::instance();
                const Cache *constCachePtr = cachePtr;

                Entry e;

                e = cachePtr->getTimetable("TWO");           ASSERT( e.get());
                e = constCachePtr->lookupTimetable("TWO");   ASSERT( e.get());

                // Verify that a timeout doesn't occur.

                sleepSeconds(2);
                e = constCachePtr->lookupTimetable("TWO");   ASSERT( e.get());
            }

            Util::destroy();
        }

        // Then 'initialize' *with* a timeout.
        {
            rc = Util::initialize(&mL, Interval(2), &sa);  ASSERT(!rc);

            {
                      Cache *cachePtr      = Util::instance();
                const Cache *constCachePtr = cachePtr;

                Entry e;

                e = cachePtr->getTimetable("TWO");           ASSERT( e.get());
                e = constCachePtr->lookupTimetable("TWO");   ASSERT( e.get());

                // Verify that a timeout occurs after 2 seconds (by sleeping a
                // sufficiently long time).

                sleepSeconds(3);
                e = constCachePtr->lookupTimetable("TWO");   ASSERT(!e.get());
            }

            Util::destroy();
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            TestLoader           mL;
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            {
                ASSERT_SAFE_PASS(Util::initialize(&mL, &sa));
                                  ASSERT( Util::instance());
                Util::destroy();  ASSERT(!Util::instance());

                ASSERT_SAFE_FAIL(Util::initialize(  0, &sa));
                                  ASSERT(!Util::instance());

                ASSERT_SAFE_FAIL(Util::initialize(&mL,   0));
                                  ASSERT(!Util::instance());
            }

            {
                ASSERT_SAFE_PASS(Util::initialize(&mL, Interval(1), &sa));
                                  ASSERT( Util::instance());
                Util::destroy();  ASSERT(!Util::instance());

                ASSERT_SAFE_FAIL(Util::initialize(  0, Interval(1), &sa));
                                  ASSERT(!Util::instance());

                ASSERT_SAFE_FAIL(Util::initialize(&mL, Interval(1),   0));
                                  ASSERT(!Util::instance());
            }

            {
                ASSERT_SAFE_PASS(Util::initialize(&mL, Interval(0,  0), &sa));
                                  ASSERT( Util::instance());
                Util::destroy();  ASSERT(!Util::instance());

                ASSERT_SAFE_FAIL(Util::initialize(&mL, Interval(0, -1), &sa));
                                  ASSERT(!Util::instance());

                ASSERT_SAFE_PASS(Util::initialize(&mL,
                                                  Interval(INT_MAX, 0), &sa));
                                  ASSERT( Util::instance());
                Util::destroy();  ASSERT(!Util::instance());

                ASSERT_SAFE_FAIL(Util::initialize(&mL,
                                                  Interval(INT_MAX, 1), &sa));
                                  ASSERT(!Util::instance());
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
        //: 1 Create a test timetable loader and a test allocator, and supply
        //:   them to the 'initialize' method.  Verify the expected result of
        //:   the 'instance' method before and after initialization of the
        //:   default timetable cache.  Also verify that no memory is allocated
        //:   from the supplied allocator.
        //:
        //: 2 Obtain the address of the default cache via the 'instance' method
        //:   and verify that a timetable may be accessed from the cache.  Also
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

        int rc = Util::initialize(&mL, &sa);

                                          ASSERT(!rc);
                                          ASSERT( Util::instance());
                                          ASSERT(0 == sa.numBlocksTotal());

        // Access: P-2

        {
            Cache *cachePtr = Util::instance();

            Entry e = cachePtr->getTimetable("TWO");

            ASSERT(e.get());
            ASSERT(2 == e->initialTransitionCode());

                                          ASSERT(0 != sa.numBlocksInUse());
        }

        // Destroy: P-3

        Util::destroy();                  ASSERT(!Util::instance());
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
