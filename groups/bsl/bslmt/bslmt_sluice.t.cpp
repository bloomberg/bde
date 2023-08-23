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
#include <bslmt_threadutil.h>   // for testing only

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsls_atomic.h>
#include <bsla_maybeunused.h>
#include <bsls_timeinterval.h>
#include <bsls_systemtime.h>
#include <bsls_stopwatch.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>

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

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

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

// ============================================================================
//                 HELPER CLASSES AND FUNCTIONS  FOR TESTING
// ----------------------------------------------------------------------------

class EnterAndWaitUntilDoneJob {
    Obj             *d_sluice;
    int             *d_done;
    bslmt::Mutex    *d_lock;
    bsls::AtomicInt *d_iterations;

  public:
    EnterAndWaitUntilDoneJob(Obj             *sluice,
                             int             *done,
                             bslmt::Mutex    *lock,
                             bsls::AtomicInt *iterations)
    : d_sluice(sluice)
    , d_done(done)
    , d_lock(lock)
    , d_iterations(iterations)
    {
    }

    void operator()()
    {
        while (1) {
            d_lock->lock();
            if (*d_done) {
                d_lock->unlock();
                break;
            }
            const void *token = d_sluice->enter();
            d_lock->unlock();
            d_sluice->wait(token);
            ++(*d_iterations);
        }
    }
};

class EnterPostSleepAndWaitJob {
    Obj              *d_sluice;
    bslmt::Semaphore *d_semaphore;

    enum {
        SLEEP_SECONDS = 2
    };

  public:
    EnterPostSleepAndWaitJob(Obj *sluice, bslmt::Semaphore *semaphore)
    : d_sluice(sluice)
    , d_semaphore(semaphore)
    {
    }

    void operator()()
    {
        const void *token = d_sluice->enter();

        d_semaphore->post();

        bslmt::ThreadUtil::sleep(bsls::TimeInterval(SLEEP_SECONDS));
        d_sluice->wait(token);
    }
};

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -MN03

            // ==================
            // class AnotherClock
            // ==================

class AnotherClock {
    // 'AnotherClock' is a C++11-compatible clock that is very similar to
    // 'bsl::chrono::steady_clock'.  The only difference is that it uses a
    // different epoch; it begins 10000 "ticks" after the beginning of
    // 'steady_clock's epoch.

  private:
    typedef bsl::chrono::steady_clock base_clock;

  public:
    typedef base_clock::duration                  duration;
    typedef base_clock::rep                       rep;
    typedef base_clock::period                    period;
    typedef bsl::chrono::time_point<AnotherClock> time_point;

    static const bool is_steady = base_clock::is_steady;

    // CLASS METHODS
    static time_point now();
        // Return a time point representing the time since the beginning of the
        // epoch.
};

// CLASS METHODS
AnotherClock::time_point AnotherClock::now()
{
    base_clock::duration ret = base_clock::now().time_since_epoch();
    return AnotherClock::time_point(ret - duration(10000));
}

            // ===============
            // class HalfClock
            // ===============

class HalfClock {
    // 'HalfClock' is a C++11-compatible clock that is very similar to
    // 'bsl::chrono::steady_clock'.  The difference is that it runs "half as
    // fast" as 'steady_clock'.

  private:
    typedef bsl::chrono::steady_clock base_clock;

  public:
    typedef base_clock::duration               duration;
    typedef base_clock::rep                    rep;
    typedef base_clock::period                 period;
    typedef bsl::chrono::time_point<HalfClock> time_point;

    static const bool is_steady = base_clock::is_steady;

    // CLASS METHODS
    static time_point now();
        // Return a time point representing the time since the beginning of the
        // epoch.
};

// CLASS METHODS
HalfClock::time_point HalfClock::now()
{
    base_clock::duration ret = base_clock::now().time_since_epoch();
    return HalfClock::time_point(ret/2);
}

// BDE_VERIFY pragma: pop

template <class CLOCK>
int WaitForTimeout(bslmt::Sluice& mX, int secondsToWait)
    // Wait on the specified 'Sluice' 'mX' for the specified 'secondsToWait'
    // seconds based on the specified 'CLOCK'.  If the call to 'timedWait'
    // returns 'e_TIMED_OUT', indicating that a timeout has occurred, verify
    // that at least that much time has elapsed (measured by the clock).
{
    typename CLOCK::time_point tp = CLOCK::now() +
                                           bsl::chrono::seconds(secondsToWait);
    const void                *token = mX.enter();
    int                        ret = mX.timedWait(token, tp);
    if (bslmt::Sluice::e_TIMED_OUT == ret) {
        ASSERT(CLOCK::now() >= tp);
    }
    return ret;
}
#endif

namespace {
namespace u {

class LocalThreadGroup {
    // This 'class' is a mechanism for managing the creation and joining of a
    // group of threads.  It mimics a subset of 'bslmt::ThreadGroup's
    // functionality for use in this test driver.  Note that it is implemented
    // here to avoid a dependency on 'bslmt_threadgroup', which would introduce
    // a test driver cycle.

    // DATA
    bsl::vector<bslmt::ThreadUtil::Handle>    d_handles;

  public:
    // CREATORS
    explicit
    LocalThreadGroup(bslma::Allocator *basicAllocator)
    : d_handles(basicAllocator)
    {}

    ~LocalThreadGroup()
    {
        BSLS_ASSERT_OPT(d_handles.empty());
    }

    // MANIPULATORS
    template <class INVOKABLE>
    void addThreads(const INVOKABLE& func, unsigned numThreads)
    {
        BSLS_ASSERT(d_handles.empty());

        d_handles.resize(numThreads);

        for (unsigned uu = 0; uu < numThreads; ++uu) {
            int rc = bslmt::ThreadUtil::create(&d_handles[uu], func);
            BSLS_ASSERT_OPT(0 == rc);    (void) rc;
        }
    }

    void joinAll()
    {
        BSLS_ASSERT(!d_handles.empty());

        for (; !d_handles.empty(); d_handles.pop_back()) {
            int rc = bslmt::ThreadUtil::join(d_handles.back());
            BSLS_ASSERT_OPT(0 == rc);    (void) rc;
        }
    }
};

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)        veryVerbose;  // unused variable warning
    (void)    veryVeryVerbose;  // unused variable warning
    (void)veryVeryVeryVerbose;  // unused variable warning


    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator ta(veryVeryVerbose);

    switch (test) {  case 0:
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'clockType'
        //
        // Concerns:
        //: 1 'clockType' returns the clock type passed to the constructor.
        //:
        //: 2 'clockType' is declared 'const'.
        //
        // Plan:
        //: 1 Create a 'const' object, and then query it to make sure that the
        //:   correct clock type is returned.
        //
        // Testing:
        //   bsls::SystemClockType::Enum clockType() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'clockType'" << endl
                          << "===================" << endl;

        const Obj def;
        ASSERT(bsls::SystemClockType::e_REALTIME == def.clockType());

        const Obj rt(bsls::SystemClockType::e_REALTIME);
        ASSERT(bsls::SystemClockType::e_REALTIME == rt.clockType());

        const Obj mt(bsls::SystemClockType::e_MONOTONIC);
        ASSERT(bsls::SystemClockType::e_MONOTONIC == mt.clockType());

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        const Obj rtC((bsl::chrono::system_clock()));
        ASSERT(bsls::SystemClockType::e_REALTIME == rtC.clockType());

        const Obj mtC((bsl::chrono::steady_clock()));
        ASSERT(bsls::SystemClockType::e_MONOTONIC == mtC.clockType());
#endif
      } break;
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

        {
            enum {
                k_NUM_WAITING_THREADS = 20,
                k_NUM_TEST_SECONDS    =  3
            };

            Obj mX(&ta);
            int done = 0;
            bslmt::Mutex lock;
            bsls::AtomicInt iterations(0);

            u::LocalThreadGroup threadGroup(&ta);

            EnterAndWaitUntilDoneJob job(&mX,
                                         &done,
                                         &lock,
                                         &iterations);

            threadGroup.addThreads(job, k_NUM_WAITING_THREADS);

            bsls::Stopwatch timer;
            timer.start();
            while (timer.elapsedTime() < double(k_NUM_TEST_SECONDS)) {
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
            // we should ALWAYS be able to make WAY MORE THAN 100 iterations
            // per thread per second

            LOOP_ASSERT(iterations,
                        100 * k_NUM_WAITING_THREADS * k_NUM_TEST_SECONDS <
                        iterations);
        }
        if (verbose) {
            P(ta.numAllocations());
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
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

        enum {
            k_NUM_SIGNALED_THREADS = 3,
            k_NUM_ITERATIONS       = 2
        };

        Obj mX;

        for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
            bslmt::Semaphore readySem;

            bslmt::ThreadUtil::Handle h;

            EnterPostSleepAndWaitJob job(&mX, &readySem);
            BSLA_MAYBE_UNUSED int rc = bslmt::ThreadUtil::create(&h, job);
            BSLS_ASSERT(0 == rc); // test invariant

            readySem.wait();
            mX.signalOne();
            bslmt::ThreadUtil::join(h);

            u::LocalThreadGroup threadGroup(&ta);
            threadGroup.addThreads(job, k_NUM_SIGNALED_THREADS);

            for (int i = 0; i < k_NUM_SIGNALED_THREADS; ++i) {
                readySem.wait();
            }
            mX.signalAll();
            threadGroup.joinAll();
        }
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
        {
            Obj                x;
            bsls::TimeInterval plus2seconds =
               bsls::SystemTime::nowRealtimeClock().addSeconds(2);

            const void *t2 = x.enter();
            ASSERT(Obj::e_TIMED_OUT == x.timedWait(t2, plus2seconds));
        }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        {
            Obj         x;
            const void *t1 = x.enter();
            const void *t2 = x.enter();
            x.signalAll();
            x.wait(t1);
            ASSERT(0 == x.timedWait(t2, HalfClock::now()));
        }
        {
            using namespace bsl::chrono;

            Obj         x;

            ASSERT(Obj::e_TIMED_OUT == WaitForTimeout<steady_clock>(x, 2));
            ASSERT(Obj::e_TIMED_OUT == WaitForTimeout<system_clock>(x, 2));
            ASSERT(Obj::e_TIMED_OUT == WaitForTimeout<AnotherClock>(x, 2));
            ASSERT(Obj::e_TIMED_OUT == WaitForTimeout<HalfClock>(x, 2));
        }
#endif
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
