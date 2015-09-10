// bdlqq_condition.t.cpp                                              -*-C++-*-

#include <bdlqq_condition.h>

#include <bdlqq_mutex.h>
#include <bsls_atomic.h>

#include <bsls_systemtime.h>

#include <bsls_stopwatch.h>
#include <bsls_systemtime.h>

#include <bsl_cerrno.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ----------------------------------------------------------------------------
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
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

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

int verbose;
int veryVerbose;

typedef bdlqq::Condition Obj;

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
        // --------------------------------------------------------------------
        // TEST THAT THIS IS A CONDITION
        //
        // Without relying on anything in, e.g., bdlqq_threadutil, test that
        // this object likely forwards to an appropriate implementation.  We'll
        // test that timedWait on a default-constructed Condition object
        // returns in roughly the right amount of time.
        // --------------------------------------------------------------------
      case 1: {
          if (verbose) cout << "Basic forwarding test" << endl
                            << "=====================" << endl;

          {
              Obj x;
              bdlqq::Mutex lock;

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
              bdlqq::Mutex lock;

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
              bdlqq::Mutex lock;

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
