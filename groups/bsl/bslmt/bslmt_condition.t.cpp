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
      case 2: {
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
      bsls::TimeInterval timeout = bsls::SystemTime::nowRealtimeClock();

      // Advance 'timeout' to some delta into the future here.

      mutex.lock();
      while (false == predicate()) {
          const int status = condition.timedWait(&mutex, timeout);
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
      case 1: {
        // --------------------------------------------------------------------
        // TEST THAT THIS IS A CONDITION
        //
        // Without relying on anything in, e.g., bslmt_threadutil, test that
        // this object likely forwards to an appropriate implementation.  We'll
        // test that timedWait on a default-constructed Condition object
        // returns in roughly the right amount of time.
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
              x.timedWait(&lock, bsls::SystemTime::nowRealtimeClock() + 2);
              double elapsed = timer.elapsedTime();
              ASSERT(1.8 <= elapsed && elapsed <= 2.2);
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
              x.timedWait(&lock, bsls::SystemTime::nowMonotonicClock() + 2);
              double elapsed = timer.elapsedTime();
              ASSERT(1.8 <= elapsed && elapsed <= 2.2);
              lock.unlock();
          }
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
