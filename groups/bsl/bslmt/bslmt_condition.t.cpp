// bslmt_condition.t.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bslmt_condition.h>

#include <bslmt_mutex.h>

#include <bslim_testutil.h>

#include <bsls_atomic.h>
#include <bsls_stopwatch.h>
#include <bsls_systemtime.h>

#include <bsl_cerrno.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

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
// Suppose we have a 'bslmt::Condition' object, 'condition', and a boolean
// predicate associated with 'condition' (represented here as a free function
// that returns a 'bool' value):
//..
    bool predicate()
        // Return 'true' if the invariant holds for 'condition', and 'false'
        // otherwise.
    {
        return true;
    }
//..

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

int verbose;
int veryVerbose;

typedef bslmt::Condition Obj;

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
int WaitForTimeout(bslmt::Condition& mX, bslmt::Mutex *m, int secondsToWait)
    // Wait on the specified 'Condition' 'mX' for the specified 'secondsToWait'
    // seconds based on the specified 'CLOCK'.  If the call to 'timedWait'
    // (using the specified mutex 'm') returns 'e_TIMED_OUT', indicating that
    // a timeout has occurred, verify that at least that much time has elapsed
    // (measured by the clock).
{
    typename CLOCK::time_point tp = CLOCK::now() +
                                           bsl::chrono::seconds(secondsToWait);
    int                        ret = mX.timedWait(m, tp);
    if (bslmt::Condition::e_TIMED_OUT == ret) {
        ASSERT(CLOCK::now() >= tp);
    }
    return ret;
}
#endif

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
// The following usage pattern should always be followed:
//..
      // ...

      bslmt::Condition condition;
      bslmt::Mutex     mutex;

      mutex.lock();
      while (false == predicate()) {
          condition.wait(&mutex);
      }

      // Modify shared resources and adjust the predicate here.

      mutex.unlock();

      // ...
//..
// The usage pattern for a timed wait is similar, but has extra branches to
// handle a timeout:
//..
      // ...

      enum { e_TIMED_OUT = -1 };
      bsls::TimeInterval absTime = bsls::SystemTime::nowRealtimeClock();

      // Advance 'absTime' to some delta into the future here.

      mutex.lock();
      while (false == predicate()) {
          const int status = condition.timedWait(&mutex, absTime);
          if (e_TIMED_OUT == status) {
              break;
          }
      }

      if (false == predicate()) {
          // The wait timed out and 'predicate' returned 'false'.  Perform
          // timeout logic here.

          // ...
      }
      else {
          // The condition variable was either signaled or timed out and
          // 'predicate' returned 'true'.  Modify shared resources and adjust
          // predicate here.

          // ...
      }
      mutex.unlock();

      // ...
//..
      } break;
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
        // TEST THAT THIS IS A CONDITION
        //
        // Without relying on anything in, e.g., bslmt_threadutil, test that
        // this object likely forwards to an appropriate implementation.  We'll
        // test that timedWait on a default-constructed Condition object
        // returns in roughly the right amount of time.  Finally, we'll verify
        // waiting with times in the past returns '-1'.
        // --------------------------------------------------------------------
          if (verbose) cout << "Basic forwarding test" << endl
                            << "=====================" << endl;

          {
              Obj x;
              bslmt::Mutex lock;

              lock.lock();
              bsls::Stopwatch timer;
              timer.start();
              x.timedWait(&lock, bsls::SystemTime::nowRealtimeClock() + 2);
              double elapsed = timer.elapsedTime();
              ASSERT(1.8 <= elapsed && elapsed <= 2.2);
              lock.unlock();
          }

          if (verbose) cout << "Test condition with realtime clock" << endl
                            << "==================================" << endl;

          {
              Obj x(bsls::SystemClockType::e_REALTIME);
              bslmt::Mutex lock;

              lock.lock();
              bsls::Stopwatch timer;

              timer.start();
              int    rv      = x.timedWait(&lock,
                                     bsls::SystemTime::nowRealtimeClock() + 2);
              double elapsed = timer.elapsedTime();
              ASSERT(Obj::e_TIMED_OUT == rv);
              ASSERT(1.8 <= elapsed && elapsed <= 2.2);

              timer.start();
              rv      = x.timedWait(&lock,
                                    bsls::SystemTime::nowRealtimeClock() - 1);
              elapsed = timer.elapsedTime();
              ASSERT(Obj::e_TIMED_OUT == rv);
              ASSERT(0.0 <= elapsed && elapsed <= 3.0);

              lock.unlock();
          }

          if (verbose) cout << "Test condition with monotonic clock" << endl
                            << "===================================" << endl;

          {
              Obj x(bsls::SystemClockType::e_MONOTONIC);
              bslmt::Mutex lock;

              lock.lock();
              bsls::Stopwatch timer;

              timer.start();
              int    rv      = x.timedWait(&lock,
                                    bsls::SystemTime::nowMonotonicClock() + 2);
              double elapsed = timer.elapsedTime();
              ASSERT(Obj::e_TIMED_OUT == rv);
              ASSERT(1.8 <= elapsed && elapsed <= 2.2);

              timer.start();
              rv      = x.timedWait(&lock,
                                    bsls::SystemTime::nowMonotonicClock() - 1);
              elapsed = timer.elapsedTime();
              ASSERT(Obj::e_TIMED_OUT == rv);
              ASSERT(0.0 <= elapsed && elapsed <= 3.0);

              lock.unlock();
          }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
          if (verbose) cout << "Test condition with chrono clocks" << endl
                            << "=================================" << endl;

          {
              using namespace bsl::chrono;

              Obj          x;
              bslmt::Mutex lock;

              lock.lock();

              ASSERT(Obj::e_TIMED_OUT ==
                                    WaitForTimeout<steady_clock>(x, &lock, 2));
              ASSERT(Obj::e_TIMED_OUT ==
                                    WaitForTimeout<system_clock>(x, &lock, 2));
              ASSERT(Obj::e_TIMED_OUT ==
                                    WaitForTimeout<AnotherClock>(x, &lock, 2));
              ASSERT(Obj::e_TIMED_OUT ==
                                       WaitForTimeout<HalfClock>(x, &lock, 2));

              lock.unlock();
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
