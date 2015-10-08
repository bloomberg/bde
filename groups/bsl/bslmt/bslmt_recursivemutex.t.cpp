// bslmt_recursivemutex.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_recursivemutex.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>

#include <bslim_testutil.h>

#include <bsls_atomic.h>

#include <bsls_stopwatch.h>
#include <bsl_cstdlib.h>     // 'atoi'
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

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

int verbose;
int veryVerbose;

typedef bslmt::RecursiveMutex Obj;

extern "C" {
   typedef void *(*ThreadFunction)(void *);
}

void My_Sleep(int ms)
{
    bslmt::ThreadUtil::microSleep(1000 * ms);
}

void My_CreateDetachedThread(ThreadFunction function, void* userData) {
    bslmt::ThreadAttributes attr;
    attr.setDetachedState(bslmt::ThreadAttributes::e_CREATE_DETACHED);
    bslmt::ThreadUtil::Handle dum;
    bslmt::ThreadUtil::create(&dum, attr, function, userData);
}

struct Case1 {
    bslmt::RecursiveMutex *d_lock;
    bsls::AtomicInt d_retval;
    bsls::AtomicInt d_retvalSet;
};

extern "C" void* Case1Thread(void* arg_p) {
    Case1* arg = (Case1*)arg_p;

    arg->d_retval = arg->d_lock->tryLock();
    if (0 == arg->d_retval) {
        arg->d_lock->unlock();
    }
    arg->d_retvalSet = 1;
    return arg_p;
}

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
      case 2: {

///Usage
///-----
// As the name implies, 'bslmt::RecursiveMutex' supports multiple calls to
// 'lock', which *must* be balanced by a corresponding number of calls to
// 'unlock'.  Suppose that we are using a 'bslmt::RecursiveMutex' object to
// guarantee exclusive access to some object.  The following sketches the
// "recursive" nature of 'bslmt::RecursiveMutex':
//..
    bslmt::RecursiveMutex recMutex;
//..
// Assume that we do not have exclusive access to the object.
//..
    recMutex.lock();    // first level of locking
//..
// We have exclusive access here.
//..
    recMutex.lock();    // second level of locking
//..
// We still have exclusive access.
//..
    recMutex.unlock();  // release second level lock -- mutex stays locked
//..
// We *still* have exclusive access.
//..
    recMutex.unlock();  // release first level lock -- mutex is unlocked
//..
// The two calls to 'unlock' have balanced the two earlier calls to 'lock'.
// Consequently, we no longer have exclusive access.
//
// Note that 'bslmt::RecursiveMutex' has substantially more overhead than does
// 'bslmt::Mutex'.  Consequently, the latter should be used unless recursive
// locking is truly warranted.

      } break;
      case 1: {
        // ------------------------------------------------------------------
        // Breathing test
        //
        // Create and destroy a RecursiveMutex.  Lock; verify that tryLock
        // succeeds; from another thread, verify that tryLock fails; unlock and
        // verify that tryLock fails; unlock again and verify that tryLock
        // succeeds.
        // ------------------------------------------------------------------
        if (verbose) {
            cout << "BREATHING TEST" << endl;
            cout << "==============" << endl;
        }
        {
            enum {
                k_MAX_SLEEP_CYCLES = 1000
            };

            bslmt::RecursiveMutex lock;
            lock.lock();
            ASSERT(0 == lock.tryLock());

            Case1 args;
            args.d_lock = &lock;
            args.d_retval = 0;
            args.d_retvalSet = 0;
            ASSERT(0 == args.d_retvalSet);
            My_CreateDetachedThread(Case1Thread, &args);

            bsls::Stopwatch sleeptime;
            sleeptime.start();
            for (int i = 0; 0 == args.d_retvalSet && i < k_MAX_SLEEP_CYCLES;
                 ++i) {
                My_Sleep(50);
            }
            if (verbose) {
                cout << "Slept for " << sleeptime.elapsedTime() << endl;
            }
            ASSERT(args.d_retvalSet);
            ASSERT(0 != args.d_retval); // should fail
            if (verbose) {
                P(args.d_retval);
            }

            lock.unlock();
            args.d_retval = 0;
            args.d_retvalSet = 0;
            My_CreateDetachedThread(Case1Thread, &args);

            for (int i = 0; 0 == args.d_retvalSet && i < k_MAX_SLEEP_CYCLES;
                 ++i) {
                My_Sleep(50);
            }
            ASSERT(args.d_retvalSet);
            ASSERT(0 != args.d_retval); // should fail again, still 1 lock
            if (verbose) {
                P(args.d_retval);
            }

            lock.unlock();
            args.d_retval = -1;
            args.d_retvalSet = 0;
            My_CreateDetachedThread(Case1Thread, &args);

            for (int i = 0; 0 == args.d_retvalSet && i < k_MAX_SLEEP_CYCLES;
                 ++i) {
                My_Sleep(50);
            }
            ASSERT(args.d_retvalSet);
            ASSERT(0 == args.d_retval); // now should succeed
            if (verbose) {
                P(args.d_retval);
            }
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
