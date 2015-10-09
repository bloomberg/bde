// bslmt_sluice.t.cpp                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_sluice.h>

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_semaphore.h>    // for testing only
#include <bslmt_threadgroup.h>  // for testing only
#include <bslmt_threadutil.h>   // for testing only

#include <bslim_testutil.h>

#include <bsls_atomic.h>
#include <bsls_timeinterval.h>

/* TBD -- bind
#include <bdlf_bind.h>
*/
#include <bsls_systemtime.h>

#include <bslma_testallocator.h>
#include <bsls_stopwatch.h>
#include <bsl_iostream.h>
#include <bsl_cstdlib.h>

#include <bsl_c_time.h>
#include <bsl_c_stdio.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//
//
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

static bslmt::Mutex coutMutex;

#define MTCOUT   { coutMutex.lock(); cout << bslmt::ThreadUtil::selfIdAsInt() \
                                          << ": "
#define MTENDL   endl;  coutMutex.unlock(); }
#define MTFLUSH  bsl::flush; } coutMutex.unlock()

///Usage
///-----
// 'bslmt::Sluice' is intended to be used to implement other synchronization
// mechanisms.  In particular, the functionality provided by 'bslmt::Sluice' is
// useful for implementing a condition variable:
//..
    class MyCondition {
        // This class implements a condition variable based on 'bslmt::Sluice'.

        // DATA
        bslmt::Sluice d_waitSluice;  // sluice object

      public:
        // MANIPULATORS
        void wait(bslmt::Mutex *mutex)
        {
            const void *token = d_waitSluice.enter();
            mutex->unlock();
            d_waitSluice.wait(token);
            mutex->lock();
        }

        void signal()
        {
            d_waitSluice.signalOne();
        }

        void broadcast()
        {
            d_waitSluice.signalAll();
        }
    };
//..

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslmt::Sluice Obj;

class My_TestAllocator : public bslma::Allocator{

    // DATA
    bslma::TestAllocator d_allocator;
    bslmt::Mutex          d_lock;

  public:
    explicit
    My_TestAllocator(bool verbose)
        : d_allocator(verbose)
    {}

    virtual ~My_TestAllocator()
    {}

    virtual void *allocate(size_type size)
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);
        return d_allocator.allocate(size);
    }

    virtual void deallocate(void *address)
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_lock);
        d_allocator.deallocate(address);
    }

    int numAllocations() {
        return d_allocator.numAllocations();
    }

    int numBytesInUse() {
        return d_allocator.numBytesInUse();
    }
};

// ============================================================================
//                 HELPER CLASSES AND FUNCTIONS  FOR TESTING
// ----------------------------------------------------------------------------

void enterAndWaitUntilDone(Obj             *sluice,
                           int             *done,
                           bslmt::Mutex    *lock,
                           bsls::AtomicInt *iterations)
{
    while (1) {
        lock->lock();
        if (*done) {
            lock->unlock();
            break;
        }
        const void *token = sluice->enter();
        lock->unlock();
        sluice->wait(token);
        ++(*iterations);
    }
}

void enterPostSleepAndWait(Obj *sluice, bslmt::Semaphore *semaphore)
{
    const void *token = sluice->enter();

    semaphore->post();

    bslmt::ThreadUtil::sleep(bsls::TimeInterval(2));
    sluice->wait(token);
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    My_TestAllocator ta(veryVeryVerbose);

    switch (test) {  case 0:
      case 3: {
        // --------------------------------------------------------------------
        // STRESS AND ALLOCATOR TEST
        //
        // From many threads, enter and wait.  From the main thread,
        // continuously signal.  After several seconds, set a 'done' flag,
        // signal once, and join all threads.
        //
        // Execute this test with a TestAllocator to look for leaks.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Stress / Allocator Test" << endl
                          << "=======================" << endl;

        /* TBD -- bind
        enum {
            k_NUM_WAITING_THREADS = 20,
            k_NUM_TEST_SECONDS    =  3
        };

        Obj mX(&ta);
        int done = 0;
        bslmt::Mutex lock;
        bsls::AtomicInt iterations(0);

        bslmt::ThreadGroup threadGroup;

        ASSERT(k_NUM_WAITING_THREADS ==
            threadGroup.addThreads(bdlf::BindUtil::bind(&enterAndWaitUntilDone,
                                                        &mX,
                                                        &done,
                                                        &lock,
                                                        &iterations),
                                   k_NUM_WAITING_THREADS));
        bsls::Stopwatch timer;
        timer.start();
        while (timer.elapsedTime() < k_NUM_TEST_SECONDS) {
            mX.signalOne();
            mX.signalOne();
            mX.signalOne();
            mX.signalOne();
            mX.signalOne();
        }
        lock.lock();
        done = 1;
        lock.unlock();

        mX.signalAll();
        threadGroup.joinAll();

        if (verbose) {
            P(iterations);
        }
        // we should ALWAYS be able to make WAY MORE THAN 100 iterations per
        // thread per second
        LOOP_ASSERT(iterations,
                100 * k_NUM_WAITING_THREADS * k_NUM_TEST_SECONDS < iterations);
      }
      if (verbose) {
          P(ta.numAllocations());
      }
      ASSERT(0 < ta.numAllocations());
      ASSERT(0 == ta.numBytesInUse());
        */

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DELAYED WAIT TEST
        //
        // Concerns:
        //   * When a thread enters the gate, and another thread signals
        //     the condition, and THEN the first thread invokes wait(), it
        //     does not block.
        //   * As above, but with three waiting threads and signalAll() -
        //     using the same sluice as before.
        //   * Repeat the entire test using the same sluice.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Delayed Wait Test" << endl
                          << "=================" << endl;

        /* TBD -- bind
        enum {
            k_NUM_SIGNALED_THREADS = 3,
            k_NUM_ITERATIONS       = 2
        };

        Obj mX;

        for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
            bslmt::Semaphore readySem;

            bslmt::ThreadUtil::Handle h;
            int rc = bslmt::ThreadUtil::create(&h,
                                   bdlf::BindUtil::bind(&enterPostSleepAndWait,
                                                       &mX, &readySem));
            BSLS_ASSERT(0 == rc); // test invariant

            readySem.wait();
            mX.signalOne();
            bslmt::ThreadUtil::join(h);

            bslmt::ThreadGroup threadGroup;
            rc = threadGroup.addThreads(
                                   bdlf::BindUtil::bind(&enterPostSleepAndWait,
                                                        &mX,
                                                        &readySem),
                                   k_NUM_SIGNALED_THREADS);
            BSLS_ASSERT(k_NUM_SIGNALED_THREADS == rc); // test invariant

            for (int i = 0; i < k_NUM_SIGNALED_THREADS; ++i) {
                readySem.wait();
            }
            mX.signalAll();
            threadGroup.joinAll();
        }
        */
      } break; // success if we can reach the end of the test
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Exercises basic functionality.
        // --------------------------------------------------------------------

        bsls::TimeInterval future = bsls::SystemTime::nowRealtimeClock();
        future.addSeconds(100);
        if (verbose) cout << endl
                          << "Breathing Test" << endl
                          << "==============" << endl;
        {
            Obj x;
            const void *t1 = x.enter();
            x.signalOne();
            x.wait(t1);
        }
        {
            Obj x;
            const void *t1 = x.enter();
            const void *t2 = x.enter();
            x.signalOne();
            x.signalOne();
            x.wait(t1);
            ASSERT(0 == x.timedWait(t2, future));
        }
        {
            Obj x;
            x.signalOne();
        }
        {
            Obj x;
            const void *t1 = x.enter();
            const void *t2 = x.enter();
            x.signalAll();
            x.wait(t1);
            ASSERT(0 == x.timedWait(t2, future));
        }
      } break;
      default: {
          testStatus = -1;
      }
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
