// bcemt_mutexassertislocked.t.cpp                                    -*-C++-*-

#include <bcemt_mutexassertislocked.h>

#include <bcemt_mutex.h>
#include <bcemt_threadutil.h>

#include <bsls_atomic.h>

#include <bsl_iostream.h>
#include <bsl_map.h>

#include <bces_atomictypes.h>

using namespace BloombergLP;
using namespace bsl;

//-----------------------------------------------------------------------------
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";   \
                aSsErT(1, #X, __LINE__); } }
#define LOOP3_ASSERT(I,J,K,X) {                                               \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J <<         \
                        "\t" << #K << ": " << K << "\n";                      \
                aSsErT(1, #X, __LINE__); } }
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o line feed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int verbose;
int veryVerbose;

                                // ------
                                // case 3
                                // ------

namespace TEST_CASE_3 {

bcemt_Mutex lockedBySubThread;
bcemt_Mutex lockedByMainThread;

bsls::AtomicInt state;

}  // close namespace TEST_CASE_3

extern "C"
void *testCase3SubThread(void *)
{
    using namespace TEST_CASE_3;

    lockedBySubThread.lock();
    ++state;
    lockedByMainThread.lock();

    // Clean up the locks on the mutexes

    lockedByMainThread.unlock();
    lockedBySubThread.unlock();

    return 0;
}

                                // ------
                                // case 2
                                // ------

namespace TEST_CASE_2 {

enum AssortMode {
    SAFE_MODE,
    NORMAL_MODE,
    OPT_MODE
} mode;

int expectedLine;

void myHandler(const char *text, const char *file, int line)
{
    switch (mode) {
      case SAFE_MODE: {
        ASSERT(!bsl::strcmp("BCEMT_MUTEX_ASSERT_SAFE_IS_LOCKED(&mutex)",text));
      } break;
      case NORMAL_MODE: {
        ASSERT(!bsl::strcmp("BCEMT_MUTEX_ASSERT_IS_LOCKED(&mutex)",     text));
      } break;
      case OPT_MODE: {
        ASSERT(!bsl::strcmp("BCEMT_MUTEX_ASSERT_OPT_IS_LOCKED(&mutex)", text));
      } break;
      default: {
        ASSERT(0);
      }
    }

    LOOP2_ASSERT(line, expectedLine, expectedLine == line);

    ASSERT(!bsl::strcmp(__FILE__, file));

#ifdef BDE_BUILD_TARGET_EXC
    throw 5;
#else
    // We can't return to 'bsls::Assert::invokeHandler'.  Make sure this test
    // fails.

    ASSERT(0 &&
              "BCEMT_MUTEX_ASSERT_*IS_LOCKED failed wtih exceptions disabled");
    abort();
#endif
}

}  // close namespace TEST_CASE_2

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ON LOCK HELD BY ANOTHER THREAD
        //
        // Concerns:
        //: 1 That BCEMT_MUTEX_ASSERT_*IS_LOCKED is never calliing
        //:   'bsls::Assert::invokeHandler' if the mutex is locked by anohter
        //:   thread.
        //
        // Plan:
        //: o Spawn a subthread that will lock a mutex, then, once it has,
        //:   call the macros to assert that it is locked and observe that
        //:   no failures occur.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING LOCK HELD BY OTHER THREAD\n"
                             "=================================\n";

        TEST_CASE_3::state = 0;
        TEST_CASE_3::lockedByMainThread.lock();

        bcemt_ThreadUtil::Handle handle;
        int sts = bcemt_ThreadUtil::create(&handle,
                                           &testCase3SubThread,
                                           0);
        ASSERT(0 == sts);

        bcemt_ThreadUtil::microSleep(10 * 1000);

        while (0 == TEST_CASE_3::state) {
            ; // do nothing
        }

        // The subthread has locked the mutex.  Now observe that none of these
        // macros blow up.

        BCEMT_MUTEX_ASSERT_SAFE_IS_LOCKED(&TEST_CASE_3::lockedBySubThread);
        BCEMT_MUTEX_ASSERT_IS_LOCKED(     &TEST_CASE_3::lockedBySubThread);
        BCEMT_MUTEX_ASSERT_OPT_IS_LOCKED( &TEST_CASE_3::lockedBySubThread);

        TEST_CASE_3::lockedByMainThread.unlock();    // let subthread finish

        sts = bcemt_ThreadUtil::join(handle);
        ASSERT(0 == sts);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BCEMT_MUTEX_ASSERT_*IS_LOCKED
        //
        // Concerns:
        //: 1 That BCEMT_MUTEX_ASSERT_*IS_LOCKED is never calliing
        //:   'bsls::Assert::invokeHandler' if the mutex is locked.
        //: 2 That, in appropriate build modes, 'invokeHandler' is in fact
        //:   called.  This test is only run when exceptions are enabled.
        //
        //: Plan:
        //: 1 C-1 with the mutex locked and the assert handler set to
        //:   'bsls::failAbort' (the default), call all three '*_IS_LOCKED'
        //:   asserts and verify that they don't fail.
        //: 2 Only if exceptions are enabled, unlock the mutex and set the
        //:   assert handler to 'TEST_CASE_2::myHandler' then call all 3
        //:   macros in try-catch blocks.  Expect throws depending on the
        //:   build mode.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING BCEMT_MUTEX_ASSERT_*IS_LOCKED\n"
                             "=====================================\n";

        bcemt_Mutex mutex;

        mutex.lock();

        BCEMT_MUTEX_ASSERT_SAFE_IS_LOCKED(&mutex);
        BCEMT_MUTEX_ASSERT_IS_LOCKED(&mutex);
        BCEMT_MUTEX_ASSERT_OPT_IS_LOCKED(&mutex);

        mutex.unlock();

#ifdef BDE_BUILD_TARGET_EXC

        bsls::Assert::setFailureHandler(&TEST_CASE_2::myHandler);

        bool expectThrow;

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
        expectThrow = true;
#else
        expectThrow = false;
#endif

        try {
            TEST_CASE_2::mode = TEST_CASE_2::SAFE_MODE;
            TEST_CASE_2::expectedLine = __LINE__ + 1;
            BCEMT_MUTEX_ASSERT_SAFE_IS_LOCKED(&mutex);
            ASSERT(!expectThrow);

            if (veryVerbose) cout << "Didn't throw SAFE\n";
        } catch (int thrown) {
            ASSERT(5 == thrown);
            ASSERT(expectThrow);

            if (veryVerbose) cout << "Threw SAFE\n";
        }

#ifdef BSLS_ASSERT_IS_ACTIVE
        expectThrow = true;
#else
        expectThrow = false;
#endif

        try {
            TEST_CASE_2::mode = TEST_CASE_2::NORMAL_MODE;
            TEST_CASE_2::expectedLine = __LINE__ + 1;
            BCEMT_MUTEX_ASSERT_IS_LOCKED(&mutex);
            ASSERT(!expectThrow);

            if (veryVerbose) cout << "Didn't throw\n";
        } catch (int thrown) {
            ASSERT(5 == thrown);
            ASSERT(expectThrow);

            if (veryVerbose) cout << "Threw\n";
        }

#ifdef BSLS_ASSERT_OPT_IS_ACTIVE
        expectThrow = true;
#else
        expectThrow = false;
#endif

        try {
            TEST_CASE_2::mode = TEST_CASE_2::OPT_MODE;
            TEST_CASE_2::expectedLine = __LINE__ + 1;
            BCEMT_MUTEX_ASSERT_OPT_IS_LOCKED(&mutex);
            ASSERT(!expectThrow);

            if (veryVerbose) cout << "Didn't throw OPT\n";
        } catch (int thrown) {
            ASSERT(5 == thrown);
            ASSERT(expectThrow);

            if (veryVerbose) cout << "Threw OPT\n";
        }

        bsls::Assert::setFailureHandler(&bsls::Assert::failAbort);
#endif
      } break;
      case 1: {
        // ------------------------------------------------------------------
        // Breathing test
        //
        // Create and destroy a mutex.  Lock and verify that tryLock fails;
        // unlock and verify that tryLock succeeds.
        // ------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        bcemt_Mutex mutex;
        mutex.lock();

        // All of these asserts should pass.

        BCEMT_MUTEX_ASSERT_SAFE_IS_LOCKED(&mutex);
        BCEMT_MUTEX_ASSERT_IS_LOCKED(&mutex);
        BCEMT_MUTEX_ASSERT_OPT_IS_LOCKED(&mutex);

        mutex.unlock();
      } break;
      case -1: {
        // ------------------------------------------------------------------
        // TESTING 'BCEMT_MUTEX_ASSERT_OPT_IS_LOCKED'
        // ------------------------------------------------------------------

        if (verbose) cout << "WATCH ASSERT BLOW UP\n"
                             "====================\n";

        bcemt_Mutex mutex;

        cout << "Expect opt assert fail now, line number is: " <<
                                                          __LINE__ + 2 << endl;

        BCEMT_MUTEX_ASSERT_OPT_IS_LOCKED(&mutex);

        BSLS_ASSERT_OPT(0);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
