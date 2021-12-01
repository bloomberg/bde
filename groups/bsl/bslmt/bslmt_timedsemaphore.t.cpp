// bslmt_timedsemaphore.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_timedsemaphore.h>

#include <bslim_testutil.h>

#include <bsls_systemtime.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>
#include <bsl_deque.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [1] Breathing test
// [2] bsls::SystemClockType::Enum clockType() const;

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

///Usage
///-----
// This example illustrates a very simple queue where potential clients can
// push integers to a queue, and later retrieve the integer values from the
// queue in FIFO order.  It illustrates two potential uses of semaphores: to
// enforce exclusive access, and to allow resource sharing.
//..
    class IntQueue {
        // FIFO queue of integer values.

        // DATA
        bsl::deque<int>       d_queue;       // underlying queue
        bslmt::TimedSemaphore d_resourceSem; // resource-availability semaphore
        bslmt::TimedSemaphore d_mutexSem;    // mutual-access semaphore

        // NOT IMPLEMENTED
        IntQueue(const IntQueue&);
        IntQueue& operator=(const IntQueue&);

      public:
        // CREATORS
        explicit IntQueue(bslma::Allocator *basicAllocator = 0);
            // Create an 'IntQueue' object.  Optionally specified a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        ~IntQueue();
            // Destroy this 'IntQueue' object.

        // MANIPULATORS
        int getInt(int *result, int maxWaitSeconds = 0);
            // Load the first integer in this queue into the specified 'result'
            // and return 0 unless the operation takes more than the optionally
            // specified 'maxWaitSeconds', in which case return a nonzero value
            // and leave 'result' unmodified.

        void pushInt(int value);
            // Push the specified 'value' to this 'IntQueue' object.
    };
//..
// Note that the 'IntQueue' constructor increments the count of the semaphore
// to 1 so that values can be pushed into the queue immediately following
// construction:
//..
    // CREATORS
    IntQueue::IntQueue(bslma::Allocator *basicAllocator)
    : d_queue(basicAllocator)
    , d_resourceSem(bsls::SystemClockType::e_MONOTONIC)
    {
        d_mutexSem.post();
    }

    IntQueue::~IntQueue()
    {
        d_mutexSem.wait();  // Wait for potential modifier.
    }

    // MANIPULATORS
    int IntQueue::getInt(int *result, int maxWaitSeconds)
    {
        // Waiting for resources.
        if (0 == maxWaitSeconds) {
            d_resourceSem.wait();
        } else {
            bsls::TimeInterval absTime = bsls::SystemTime::nowMonotonicClock()
                .addSeconds(maxWaitSeconds);
            int rc = d_resourceSem.timedWait(absTime);
            if (0 != rc) {
               return rc;
            }
        }

        // 'd_mutexSem' is used for exclusive access.
        d_mutexSem.wait();       // lock
        *result = d_queue.back();
        d_queue.pop_back();
        d_mutexSem.post();       // unlock

        return 0;
    }

    void IntQueue::pushInt(int value)
    {
        d_mutexSem.wait();
        d_queue.push_front(value);
        d_mutexSem.post();

        d_resourceSem.post();  // Signal that we have resources available.
    }
//..

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
int WaitForTimeout(bslmt::TimedSemaphore& mX, int secondsToWait)
    // Wait on the specified 'TimedSemaphore' 'mX' for the specified
    // 'secondsToWait' seconds based on the specified 'CLOCK'.  If the call to
    // 'timedWait' returns 'e_TIMED_OUT', indicating that a timeout has
    // occurred, verify that at least that much time has elapsed (measured by
    // the clock).
{
    typename CLOCK::time_point tp = CLOCK::now() +
                                           bsl::chrono::seconds(secondsToWait);
    int                        ret = mX.timedWait(tp);
    if (bslmt::TimedSemaphore::e_TIMED_OUT == ret) {
#ifdef BSLS_PLATFORM_OS_WINDOWS
        // On Windows, 'timedWait' may return early due to rounding of the time
        // to "ticks".

        ASSERT(CLOCK::now() + bsl::chrono::microseconds(500) >= tp);
#else
        ASSERT(CLOCK::now() >= tp);
#endif
    }
    return ret;
}
#endif

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslmt::TimedSemaphore Obj;

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
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
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Breathing Test" << endl
                          << "==============" << endl;
        {
            Obj mX;
            mX.post();
            mX.post(2);
            mX.wait();
            ASSERT(0 == mX.timedWait(bsls::SystemTime::nowRealtimeClock() +
                                    bsls::TimeInterval(60)));
            ASSERT(0 == mX.tryWait());
            ASSERT(0 != mX.tryWait());

            bsls::TimeInterval ti = bsls::SystemTime::nowRealtimeClock() +
                                    bsls::TimeInterval(1);
            ASSERT(0 != mX.timedWait(ti));

#ifdef BSLS_PLATFORM_OS_WINDOWS
            // On Windows, 'timedWait' may return early due to rounding of the
            // time to "ticks".

            ASSERT(bsls::SystemTime::nowRealtimeClock()
                                        + bsls::TimeInterval(0, 500000) >= ti);
#else
            ASSERT(bsls::SystemTime::nowRealtimeClock() >= ti);
#endif
        }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        {
            using namespace bsl::chrono;

            Obj mX(steady_clock{});

            mX.post();
            mX.post(5);
            mX.wait();
            ASSERT(0 == WaitForTimeout<steady_clock>(mX, 60));
            ASSERT(0 == WaitForTimeout<system_clock>(mX, 60));
            ASSERT(0 == WaitForTimeout<AnotherClock>(mX, 60));
            ASSERT(0 == WaitForTimeout<HalfClock>(mX, 60));

            ASSERT(0 == mX.tryWait());
            ASSERT(0 != mX.tryWait());

            ASSERT(Obj::e_TIMED_OUT == WaitForTimeout<steady_clock>(mX, 1));
            ASSERT(Obj::e_TIMED_OUT == WaitForTimeout<system_clock>(mX, 1));
            ASSERT(Obj::e_TIMED_OUT == WaitForTimeout<AnotherClock>(mX, 1));
            ASSERT(Obj::e_TIMED_OUT == WaitForTimeout<HalfClock>(mX, 1));
        }
        {
            using namespace bsl::chrono;

            Obj mX(system_clock{});

            mX.post();
            mX.post(5);
            mX.wait();
            ASSERT(0 == WaitForTimeout<steady_clock>(mX, 60));
            ASSERT(0 == WaitForTimeout<system_clock>(mX, 60));
            ASSERT(0 == WaitForTimeout<AnotherClock>(mX, 60));
            ASSERT(0 == WaitForTimeout<HalfClock>(mX, 60));

            ASSERT(0 == mX.tryWait());
            ASSERT(0 != mX.tryWait());

            ASSERT(Obj::e_TIMED_OUT == WaitForTimeout<steady_clock>(mX, 1));
            ASSERT(Obj::e_TIMED_OUT == WaitForTimeout<system_clock>(mX, 1));
            ASSERT(Obj::e_TIMED_OUT == WaitForTimeout<AnotherClock>(mX, 1));
            ASSERT(Obj::e_TIMED_OUT == WaitForTimeout<HalfClock>(mX, 1));
        }
#endif
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
