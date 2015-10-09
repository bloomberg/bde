// bslmt_meteredmutex.t.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_meteredmutex.h>

#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>
#include <bslmt_barrier.h>     // for testing only

#include <bslim_testutil.h>

#include <bsls_timeutil.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// Testing bslmt::MeteredMutex is divided into 3 parts (apart from usage and
// breathing test).
//   (1) Testing mutex behavior, it is tested in [ 2].
//   (2) Testing 'holdTime' and 'waitTime' (specially in the presence
//       of multiple threads), this is tested in [ 3].
//   (3) Testing 'lastResetTime' and 'resetMetrics' (specially the in
//       presence of multiple threads), this is tested in [ 4].
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] bslmt::MeteredMutex();
// [ 1] ~bslmt::MeteredMutex();
//
// MANIPULATORS
// [ 2] void lock();
// [ 4] void resetMetrics();
// [ 2] int tryLock();
// [ 2] void unlock();
//
// ACCESSORS
// [ 3] bsls::Types::Int64 holdTime() const;
// [ 4] bsls::Types::Int64 lastResetTime() const;
// [ 3] bsls::Types::Int64 waitTime() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
//              GLOBAL TYPEDEFS/CONSTANTS/VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

static int verbose;
static int veryVerbose;
// static int veryVeryVerbose;  // not used
typedef bslmt::MeteredMutex Obj;
const bsls::Types::Int64 NANOSECONDS_IN_ONE_MICRO_SECOND = 1000LL;
bslmt::Mutex printLock; // lock needed for non thread-safe macro (P, P_ etc)

// ============================================================================
//                          CASE 5 RELATED ENTITIES
// ----------------------------------------------------------------------------

///Usage
///-----
// In the following example, we have 'NUM_THREADS' threads (that are
// sequentially numbered from '0' to 'NUM_THREADS-1') and two counters
// 'evenCount' and 'oddCount'.  'evenCount' is incremented by the even numbered
// threads and 'oddCount' is incremented by the odd ones.  We considers two
// strategies to increment these counters.  In the first strategy (strategy1),
// we use two mutexes (one for each counter) and in the second strategy
// (strategy2), we use a single mutex for both counters.
//..
    int oddCount = 0;
    int evenCount = 0;

    typedef bslmt::MeteredMutex Obj;
    Obj oddMutex;
    Obj evenMutex;
    Obj globalMutex;

    enum { k_USAGE_NUM_THREADS = 4, k_USAGE_SLEEP_TIME = 100000 };
    bslmt::Barrier usageBarrier(k_USAGE_NUM_THREADS);

    void executeInParallel(int                               numThreads,
                           bslmt::ThreadUtil::ThreadFunction function)
        // Create the specified 'numThreads', each executing the specified
        // 'function'.  Number each thread (sequentially from 0 to
        // 'numThreads - 1') by passing i to i'th thread.  Finally join all the
        // threads.
    {
        bslmt::ThreadUtil::Handle *threads =
                                     new bslmt::ThreadUtil::Handle[numThreads];
        ASSERT(threads);

        for (int i = 0; i < numThreads; ++i) {
            bslmt::ThreadUtil::create(&threads[i], function, (void*)i);
        }
        for (int i = 0; i < numThreads; ++i) {
            bslmt::ThreadUtil::join(threads[i]);
        }

        delete [] threads;
    }

    extern "C" {
        void *strategy1(void *arg)
        {
            usageBarrier.wait();
            int remainder = (int)(bsls::Types::IntPtr)arg % 2;
            if (remainder == 1) {
                oddMutex.lock();
                ++oddCount;
                bslmt::ThreadUtil::microSleep(k_USAGE_SLEEP_TIME);
                oddMutex.unlock();
            }
            else {
                evenMutex.lock();
                ++evenCount;
                bslmt::ThreadUtil::microSleep(k_USAGE_SLEEP_TIME);
                evenMutex.unlock();
            }
            return NULL;
        }
    } // extern "C"

    extern "C" {
        void *strategy2(void *arg)
        {
            usageBarrier.wait();
            int remainder = (int)(bsls::Types::IntPtr)arg % 2;
            if (remainder == 1) {
                globalMutex.lock();
                ++oddCount;
                bslmt::ThreadUtil::microSleep(k_USAGE_SLEEP_TIME);
                globalMutex.unlock();
            }
            else {
                globalMutex.lock();
                ++evenCount;
                bslmt::ThreadUtil::microSleep(k_USAGE_SLEEP_TIME);
                globalMutex.unlock();
            }
            return NULL;
        }
    } // extern "C"
//..

// ============================================================================
//                          CASE 4 RELATED ENTITIES
// ----------------------------------------------------------------------------

enum { k_NUM_THREADS4 = 4, k_NUM_ITERATION = 10000000 };

bslmt::Barrier barrier4(k_NUM_THREADS4);
Obj mutex4;
extern "C" {
  void *resetTest(void *arg)
    {
        bsls::Types::Int64 previous, current;

        barrier4.wait();

        previous = mutex4.lastResetTime();
        for (int i = 0; i < k_NUM_ITERATION; ++i) {
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

// ============================================================================
//                          CASE 3 RELATED ENTITIES
// ----------------------------------------------------------------------------

enum {
    k_NUM_THREADS3 = 2,   // number of threads to be created
    k_SLEEP_TIME3  = 100000,
    k_NUM_ACQUIRE  = 2    // number of times, each thread acquire the lock
};
Obj mutex3;
bslmt::Barrier barrier3(k_NUM_THREADS3);

extern "C" {
    void *timesTest(void *arg)
    {
        for (int i = 0; i < k_NUM_ACQUIRE; ++i) {
            barrier3.wait();

            mutex3.lock();
            bslmt::ThreadUtil::microSleep(k_SLEEP_TIME3);
            mutex3.unlock();
        }
        return NULL;
    }
} // extern "C"

// ============================================================================
//                          CASE 2 RELATED ENTITIES
// ----------------------------------------------------------------------------

enum { k_NUM_THREADS2 = 4, k_SLEEP_TIME2 = 5 };
enum { e_VALID, e_INVALID };
int state = e_VALID;
Obj mutex2;

extern "C" {
    void *mutexTest(void *arg)
    {
        int remainder = (int)(bsls::Types::IntPtr)arg % 2;
        if (remainder == 1) {
            mutex2.lock();
        }
        else {
            while (mutex2.tryLock() != 0) {
            }
        }
        ASSERT(state == e_VALID);
        state = e_INVALID;
        bslmt::ThreadUtil::microSleep(k_SLEEP_TIME2);
        state = e_VALID;
        mutex2.unlock();
        return NULL;
    }
} // extern "C"

// ============================================================================
//                          CASE-1 RELATED ENTITIES
// ----------------------------------------------------------------------------

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;

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

// Then in the application 'main':
//..
    executeInParallel(k_USAGE_NUM_THREADS, strategy1);
    bsls::Types::Int64 waitTimeForStrategy1 =
                                    oddMutex.waitTime() + evenMutex.waitTime();

    executeInParallel(k_USAGE_NUM_THREADS, strategy2);
    bsls::Types::Int64 waitTimeForStrategy2 = globalMutex.waitTime();

    ASSERT(waitTimeForStrategy2 > waitTimeForStrategy1);
    if (veryVerbose) {
        P(waitTimeForStrategy1);
        P(waitTimeForStrategy2);
    }
//..
// We measured the wait times for each strategy.  Intuitively, the wait time
// for the second strategy should be greater than that of the first.  The
// output was consistent with our expectation.
//..
// waitTimeForStrategy1 = 400787000
// waitTimeForStrategy2 = 880765000
//..
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
        //   main thread spans 'k_NUM_THREADS4' threads, each of which calls
        //   'resetMetrics' and 'lastResetTime' in a loop.  Verify that for
        //   each thread, values returned by 'lastResetTime' are in
        //   increasing order.
        //
        // Tactics:
        //
        // Testing:
        //   bsls::Types::Int64 lastResetTime() const;
        //   void resetMetrics();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'lastResetTime', 'resetMetrics'" << endl
                          << "=======================================" << endl;

        // TBD: Since 'bsls::TimeUtil::getTimer' is not monotonic on hp, this
        // test case will not work on hp.

        // executeInParallel(k_NUM_THREADS4, resetTest);
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
        //   main thread spans k_NUM_THREADS3, each of which (in a loop, that
        //   runs 'k_NUM_ACQUIRE3' times) acquires the lock, sleeps for a while
        //   releases the lock.  Finally main thread joins all these threads
        //   and verifies that the 'holdTime' and 'waitTime' give the
        //   expected results.
        //
        // Tactics:
        //
        // Testing:
        //   bsls::Types::Int64 holdTime() const;
        //   bsls::Types::Int64 waitTime() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'holdTime' and 'waitTime'" << endl
                          << "=================================" << endl;

        executeInParallel(k_NUM_THREADS3, timesTest);

        // we have 'k_NUM_THREADS3' threads, each of which holds the lock for
        // 'k_SLEEP_TIME3' time in one iteration of loop (and we have
        // 'k_NUM_ACQUIRE3' iterations in the loop).
        bsls::Types::Int64 holdTime =  k_NUM_ACQUIRE
                                       *  k_NUM_THREADS3
                                       *  k_SLEEP_TIME3
                                       *  NANOSECONDS_IN_ONE_MICRO_SECOND;
        ASSERT(mutex3.holdTime() >= (bsls::Types::Int64)(holdTime * 50.0/100));
                                                         // error margin = 50%
        // In an iteration, after the barrier, the first thread to acquire the
        //  lock waits for no time, the second thread to acquire the lock waits
        // for 'k_SLEEP_TIME3' time, the third thread to acquire the lock waits
        // for '2 * k_SLEEP_TIME3' and so on.  Thus the wait time accumulated
        // during one iteration = 0*k_SLEEP_TIME3 + 1*k_SLEEP_TIME3 +
        // 2*k_SLEEP_TIME3 ..........+ (k_NUM_THREADS3-1)*k_SLEEP_TIME3 =
        // ((k_NUM_THREADS3-1)*k_NUM_THREADS3)/2.0 * k_SLEEP_TIME3.  We have
        // k_NUM_ACQUIRE such iteration.

        bsls::Types::Int64 waitTime =
              (bsls::Types::Int64) (k_NUM_ACQUIRE
                                    * ((k_NUM_THREADS3-1) * k_NUM_THREADS3)/2.0
                                    * k_SLEEP_TIME3
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
        //   Initialize a global variable 'state' with 'e_VALID'.  Create
        //   'k_NUM_THREADS' (sequentially numbered from 0 to k_NUM_THREADS)
        //   threads.  Each thread attempts to acquire the lock (such that
        //   odd numbered threads acquires the lock using 'lock' and even
        //   numbered threads acquires the lock using repetitive calls to
        //   'tryLock', until they succeed), verifies
        //   that the invariant 'state == e_VALID' is true, makes the invariant
        //   false by setting the 'state' to 'e_INVALID', sleeps for a while
        //   (to allow other threads to run), sets the 'state' to 'e_VALID',
        //   unlocks the mutex and returns.  Finally, join all the threads and
        //   verity that the invariant 'state == e_VALID' is still true.
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

        executeInParallel(k_NUM_THREADS2, mutexTest);
        ASSERT(state == e_VALID);

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

        bsls::Types::Int64 t1 = bsls::TimeUtil::getTimer();
        Obj mutex;
        bsls::Types::Int64 t2 = bsls::TimeUtil::getTimer();
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
            k_SLEEP_TIME   = 100000 // in microseconds
        };

        const float ERROR_MARGIN = .6;      // 40 % error margin
        mutex.lock();
        bslmt::ThreadUtil::microSleep(k_SLEEP_TIME);
        mutex.unlock();
        ASSERT(mutex.holdTime() >= (bsls::Types::Int64)(
                                          k_SLEEP_TIME * 1000 * ERROR_MARGIN));
        // 'holdTime()' reports in nanoseconds
        if (veryVerbose) {
            P(mutex.holdTime());
            P(mutex.waitTime());
        }

        bsls::Types::Int64 t3 = bsls::TimeUtil::getTimer();
        mutex.resetMetrics();
        bsls::Types::Int64 t4 = bsls::TimeUtil::getTimer();
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
        bslmt::ThreadUtil::microSleep(k_SLEEP_TIME);
        mutex.unlock();
        ASSERT(mutex.holdTime() >= (bsls::Types::Int64)(
                                         k_SLEEP_TIME * 1000  * ERROR_MARGIN));
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
