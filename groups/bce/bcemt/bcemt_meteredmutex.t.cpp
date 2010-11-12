// bcemt_meteredmutex.t.cpp      -*-C++-*-

#include <bcemt_meteredmutex.h>

#include <bcemt_thread.h>      // for testing only
#include <bcemt_barrier.h>     // for testing only

#include <bsls_platformutil.h>
#include <bsls_timeutil.h>

#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// Testing bcemt_MeteredMutex is divided into 3 parts (apart from usage and
// breathing test).
//   (1) Testing mutex behavior, it is tested in [ 2].
//   (2) Testing 'holdTime' and 'waitTime' (specially in the presence
//       of multiple threads), this is tested in [ 3].
//   (3) Testing 'lastResetTime' and 'resetMetrics' (specially the in
//       presence of multiple threads), this is tested in [ 4].
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] bcemt_MeteredMutex();
// [ 1] ~bcemt_MeteredMutex();
//
// MANIPULATORS
// [ 2] void lock();
// [ 4] void resetMetrics();
// [ 2] int tryLock();
// [ 2] void unlock();
//
// ACCESSORS
// [ 3] bsls_PlatformUtil::Int64 holdTime() const;
// [ 4] bsls_PlatformUtil::Int64 lastResetTime() const;
// [ 3] bsls_PlatformUtil::Int64 waitTime() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS/VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

static int verbose;
static int veryVerbose;
// static int veryVeryVerbose;  // not used
typedef bcemt_MeteredMutex Obj;
const bsls_PlatformUtil::Int64 NANOSECONDS_IN_ONE_MICRO_SECOND = 1000LL;
bcemt_Mutex printLock; // lock needed for non thread-safe macro (P, P_ etc)

//=============================================================================
//                         HELPER CLASSES AND FUNCTIONS  FOR TESTING

void executeInParallel(int numThreads, bcemt_ThreadUtil::ThreadFunction func)
   // Create the specified 'numThreads', each executing the specified 'func'.
   // Number each thread (sequentially from 0 to 'numThreads-1') by passing i
   // to i'th thread.  Finally join all the threads.
{
    bcemt_ThreadUtil::Handle *threads =
                               new bcemt_ThreadUtil::Handle[numThreads];
    ASSERT(threads);

    for (int i = 0; i < numThreads; ++i) {
        bcemt_ThreadUtil::create(&threads[i], func, (void*)i);
    }
    for (int i = 0; i < numThreads; ++i) {
        bcemt_ThreadUtil::join(threads[i]);
    }

    delete [] threads;
}

//=============================================================================
//                         CASE 5 RELATED ENTITIES
//-----------------------------------------------------------------------------
int oddCount = 0;
int evenCount = 0;

Obj oddMutex;
Obj evenMutex;
Obj globalMutex;

enum { NUM_THREADS5 = 4, SLEEP_TIME5 = 1 };
bcemt_Barrier barrier5(NUM_THREADS5);

extern "C" {
  void *strategy1(void *arg)
    {
        barrier5.wait();
        int remainder = (int)(bsls_PlatformUtil::IntPtr)arg % 2;
        if (remainder == 1) {
            oddMutex.lock();
            ++oddCount;
            bcemt_ThreadUtil::microSleep(SLEEP_TIME5);
            oddMutex.unlock();
        }
        else {
            evenMutex.lock();
            ++evenCount;
            bcemt_ThreadUtil::microSleep(SLEEP_TIME5);
            evenMutex.unlock();
        }
        return NULL;
    }
} //extern "C"

extern "C" {
    void *strategy2(void *arg)
    {
        barrier5.wait();
        int remainder = (int)(bsls_PlatformUtil::IntPtr)arg % 2;
        if (remainder == 1) {
            globalMutex.lock();
            ++oddCount;
            bcemt_ThreadUtil::microSleep(SLEEP_TIME5);
            globalMutex.unlock();
        }
        else {
            globalMutex.lock();
            ++evenCount;
            bcemt_ThreadUtil::microSleep(SLEEP_TIME5);
            globalMutex.unlock();
        }
        return NULL;
    }
} //extern "C"

//=============================================================================
//                         CASE 4 RELATED ENTITIES
//-----------------------------------------------------------------------------

enum { NUM_THREADS4 = 4, NUM_ITERATION = 10000000 };
bcemt_Barrier barrier4(NUM_THREADS4);
Obj mutex4;
extern "C" {
  void *resetTest(void *arg)
    {
        bsls_PlatformUtil::Int64 previous, current;

        barrier4.wait();

        previous = mutex4.lastResetTime();
        for(int i = 0; i < NUM_ITERATION; ++i) {
            mutex4.resetMetrics();
            current = mutex4.lastResetTime();
            ASSERT(current >= previous);
            if (veryVerbose) {
                printLock.lock();
                P_(previous); P(current);
                printLock.unlock();
            }
            previous = current;
        }

        return NULL;
    }
} //extern "C"

//=============================================================================
//                         CASE 3 RELATED ENTITIES
//-----------------------------------------------------------------------------

enum {
    NUM_THREADS3 = 2,   // number of threads to be created
    SLEEP_TIME3  = 100000,
    NUM_ACQUIRE  = 2    // number of times, each thread acquire the lock
};
Obj mutex3;
bcemt_Barrier barrier3(NUM_THREADS3);

extern "C" {
    void *timesTest(void *arg)
    {
        for (int i = 0; i < NUM_ACQUIRE; ++i) {
            barrier3.wait();

            mutex3.lock();
            bcemt_ThreadUtil::microSleep(SLEEP_TIME3);
            mutex3.unlock();
        }
        return NULL;
    }
} // extern "C"

//=============================================================================
//                         CASE 2 RELATED ENTITIES
//-----------------------------------------------------------------------------

enum { NUM_THREADS2 = 4, SLEEP_TIME2 = 5 };
enum { VALID, INVALID };
int state = VALID;
Obj mutex2;

extern "C" {
    void *mutexTest(void *arg)
    {
        int remainder = (int)(bsls_PlatformUtil::IntPtr)arg % 2;
        if (remainder == 1) {
            mutex2.lock();
        }
        else {
            while(mutex2.tryLock() != 0) {
            }
        }
        ASSERT(state == VALID);
        state = INVALID;
        bcemt_ThreadUtil::microSleep(SLEEP_TIME2);
        state = VALID;
        mutex2.unlock();
        return NULL;
    }
} // extern "C"

//=============================================================================
//                         CASE-1 RELATED ENTITIES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    // veryVeryVerbose = argc > 4; // not used

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver.
        //
        // Tactics:
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING USAGE EXAMPLE" << endl
                          << "=====================" << endl;

        executeInParallel(NUM_THREADS5, strategy1);
        bsls_PlatformUtil::Int64 waitTimeForStrategy1 = oddMutex.waitTime()
                                                      + evenMutex.waitTime();

        executeInParallel(NUM_THREADS5, strategy2);
        bsls_PlatformUtil::Int64 waitTimeForStrategy2 =
                                 globalMutex.waitTime();

        if (veryVerbose) {
            P(waitTimeForStrategy1);
            P(waitTimeForStrategy2);
        }

      break;  }
      case 4: {
        // --------------------------------------------------------------------
        // TESTING LAST_RESET_TIME AND RESET_METRICS:
        //   Testing that 'lastResetTime' and 'resetMetrics' work correctly.
        //
        // Concerns:
        //   That the 'lastResetTime' and 'resetMetrics' work correctly in
        //   presence of multiple threads.
        //
        // Plan:
        //   main thread spans 'NUM_THREADS4' threads, each of which calls
        //   'resetMetrics' and 'lastResetTime' in a loop.  Verify that for
        //   each thread, values returned by 'lastResetTime' are in
        //   increasing order.
        //
        // Tactics:
        //
        // Testing:
        //   bsls_PlatformUtil::Int64 lastResetTime() const;
        //   void resetMetrics();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'lastResetTime', 'resetMetrics'" << endl
                          << "=======================================" << endl;

        // TBD:
        // Since 'bsls_TimeUtil::getTimer' is not monotonic on hp, this test
        // case will not work on hp.
        // executeInParallel(NUM_THREADS4, resetTest);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING HOLDTIME AND WAITTIME:
        //   Testing that the 'holdTime' and 'waitTime' work correctly.
        //
        // Concerns:
        //   That the 'holdTime' and 'waitTime' work correctly, in presence
        //   of multiple threads.
        //
        //   That the hold and wait time are accumulated correctly, when
        //   a single thread holds the lock over multiple time intervals.
        //
        // Plan:
        //   main thread spans NUM_THREADS3, each of which (in a loop, that
        //   runs 'NUM_ACQUIRE3' times) acquires the lock, sleeps for a while
        //   releases the lock.  Finally main thread joins all these threads
        //   and verifies that the 'holdTime' and 'waitTime' give the
        //   expected results.
        //
        // Tactics:
        //
        // Testing:
        //   bsls_PlatformUtil::Int64 holdTime() const;
        //   bsls_PlatformUtil::Int64 waitTime() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'holdTime' and 'waitTime'" << endl
                          << "=================================" << endl;

        executeInParallel(NUM_THREADS3, timesTest);

        // we have 'NUM_THREADS3' threads, each of which holds the lock for
        // 'SLEEP_TIME3' time in one iteration of loop (and we have
        // 'NUM_ACQUIRE3' iterations in the loop).
        bsls_PlatformUtil::Int64 holdTime =  NUM_ACQUIRE
                                          *  NUM_THREADS3
                                          *  SLEEP_TIME3
                                          *  NANOSECONDS_IN_ONE_MICRO_SECOND;
        ASSERT(mutex3.holdTime() >= (bsls_PlatformUtil::Int64)(
                                                         holdTime * 50.0/100));
                                                         // error margin = 50%
        // In an iteration, after the barrier, the first thread to acquire the
        //  lock waits for no time, the second thread to acquire the lock waits
        // for 'SLEEP_TIME3' time, the third thread to acquire the lock waits
        // for '2 * SLEEP_TIME3' and so on.  Thus the wait time accumulated
        // during one iteration = 0*SLEEP_TIME3 + 1*SLEEP_TIME3 + 2*SLEEP_TIME3
        //  ..........+ (NUM_THREADS3-1)*SLEEP_TIME3
        // =  ((NUM_THREADS3-1)*NUM_THREADS3)/2.0 * SLEEP_TIME3.
        // We have NUM_ACQUIRE such iteration.

        bsls_PlatformUtil::Int64 waitTime =
              (bsls_PlatformUtil::Int64) (NUM_ACQUIRE
                                        * ((NUM_THREADS3-1) * NUM_THREADS3)/2.0
                                        * SLEEP_TIME3
                                        * NANOSECONDS_IN_ONE_MICRO_SECOND
                                        * 50.0/100);      // error margin = 50%
        ASSERT(mutex3.waitTime() > waitTime);
        if (veryVerbose) {
            P(mutex3.holdTime()) ;
            P(mutex3.waitTime());
        }

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING MUTEX PROPERTIES:
        //   Verify the *mutex* part of the metered mutex.
        //
        // Concerns:
        //   That the lock can be acquired after the mutex has been
        //   constructed.
        //
        //   That only one thread can acquire the lock at a time (either
        //   through 'lock' or 'tryLock').
        //
        //   That after the lock is released, it should be possible to
        //   acquire the lock.
        //
        // Plan:
        //   Initialize a global variable 'state' with 'VALID'.  Create
        //   'NUM_THREADS' (sequentially numbered from 0 to NUM_THREADS)
        //   threads.  Each thread attempts to acquire the lock (such that
        //   odd numbered threads acquires the lock using 'lock' and even
        //   numbered threads acquires the lock using repetitive calls to
        //   'tryLock', until they succeed), verifies
        //   that the invariant 'state == VALID' is true, makes the invariant
        //   false by setting the 'state' to 'INVALID', sleeps for a while (to
        //   allow other threads to run), sets the 'state' to 'VALID', unlocks
        //   the mutex and returns.  Finally, join all the threads and verity
        //   that the invariant 'state == VALID' is still true.
        //
        // Tactics:
        //
        // Testing:
        //   void lock();
        //   int tryLock();
        //   void unlock();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing mutex properties" << endl
                          << "========================" << endl;

        executeInParallel(NUM_THREADS2, mutexTest);
        ASSERT(state == VALID);

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Concerns:
        //   That basic essential functionality is operational
        //   for one thread.
        //
        // Plan:
        //   Create an object and verify that all accessors works as
        //   expected.
        //
        //   Lock the mutex using 'lock', sleep for a while, unlock and
        //   then verify that the the 'holdTime' works correctly.
        //
        //   Call 'resetMetrics' and verify that all accessors works
        //   correctly.
        //
        //   Lock the mutex using 'tryLock', sleep for half second, unlock
        //   and then verify that the the 'holdTime' works correctly.
        //
        // Tactics:
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bsls_PlatformUtil::Int64 t1 = bsls_TimeUtil::getTimer();
        Obj mutex;
        bsls_PlatformUtil::Int64 t2 = bsls_TimeUtil::getTimer();
        ASSERT(mutex.waitTime() == 0);
        ASSERT(mutex.holdTime() == 0);
        ASSERT(t2 >= mutex.lastResetTime());
        ASSERT(mutex.lastResetTime() >= t1);
        if (veryVerbose) {
            P(mutex.holdTime());
            P(mutex.waitTime());
            P_(t1); P_(mutex.lastResetTime()); P(t2);
        }

        enum {
            SLEEP_TIME   = 100000 // in microseconds
        };

        const float ERROR_MARGIN = .6;      // 40 % error margin
        mutex.lock();
        bcemt_ThreadUtil::microSleep(SLEEP_TIME);
        mutex.unlock();
        ASSERT(mutex.holdTime() >= (bsls_PlatformUtil::Int64)(
                                            SLEEP_TIME * 1000 * ERROR_MARGIN));
        // 'holdTime()' reports in nanoseconds
        if (veryVerbose) {
            P(mutex.holdTime());
            P(mutex.waitTime());
        }

        bsls_PlatformUtil::Int64 t3 = bsls_TimeUtil::getTimer();
        mutex.resetMetrics();
        bsls_PlatformUtil::Int64 t4 = bsls_TimeUtil::getTimer();
        ASSERT(mutex.waitTime() == 0);
        ASSERT(mutex.holdTime() == 0);
        ASSERT(t4 >= mutex.lastResetTime());
        ASSERT(mutex.lastResetTime() >= t3);
        if (veryVerbose) {
            P(mutex.holdTime());
            P(mutex.waitTime());
            P_(t3); P_(mutex.lastResetTime()); P(t4);
        }

        ASSERT(mutex.tryLock() == 0);
        bcemt_ThreadUtil::microSleep(SLEEP_TIME);
        mutex.unlock();
        ASSERT(mutex.holdTime() >= (bsls_PlatformUtil::Int64)(
                                           SLEEP_TIME * 1000  * ERROR_MARGIN));
        // 'holdTime()' reports in nanoseconds
        if (veryVerbose) {
            P(mutex.holdTime());
            P(mutex.waitTime());
        }

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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
