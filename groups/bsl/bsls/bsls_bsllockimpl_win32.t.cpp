// bsls_bsllockimpl_win32.t.cpp                                       -*-C++-*-
#include <bsls_bsllockimpl_win32.h>

#include <bsls_atomic.h>         // for testing only
#include <bsls_bsltestutil.h>    // for testing only

#ifdef BSLS_PLATFORM_OS_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif  // BSLS_PLATFORM_OS_WINDOWS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a mutually exclusive locking primitive ("mutex") that wraps a
// suitable platform-specific mechanism.  The operations on the mutex type,
// 'bsls::BslLockImpl_win32', are 'lock' and 'unlock'.
//
// In each test we use two concurrent threads, verifying that the 'lock' and
// 'unlock' methods invoked by the respective threads occur in the expected
// order.
// ----------------------------------------------------------------------------
// [ 1] BslLockImpl_win32::BslLockImpl_win32();
// [ 1] BslLockImpl_win32::~BslLockImpl_win32();
// [ 1] void BslLockImpl_win32::lock();
// [ 1] void BslLockImpl_win32::unlock();
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

#ifdef BSLS_PLATFORM_OS_WINDOWS
// ============================================================================
//                   GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsls::BslLockImpl_win32      Obj;

typedef HANDLE    ThreadId;

extern "C" {
    typedef void *(*ThreadFunction)(void *arg);
}

// ============================================================================
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static
ThreadId createThread(ThreadFunction func, void *arg)
    // Create a thread and return its thread id.
{
    return CreateThread(0, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, 0);
}

static
void joinThread(ThreadId id)
    // Join the thread with specified 'id'.
{
    WaitForSingleObject(id, INFINITE);
    CloseHandle(id);
}

static
void sleepSeconds(int seconds)
    // Sleep the specified number of 'seconds'.
{
    Sleep(seconds * 1000);
}

enum { MAX_SLEEP_CYCLES = 2 };

static
void pause(bsls::AtomicInt *value)
    // Pause the current thread until the specified '*value' is non-zero, or a
    // sufficient number of sleep cycles have elapsed.
{
    for (int i = 0; 0 == *value && i < MAX_SLEEP_CYCLES; ++i) {
        sleepSeconds(1);
    }
}

                                // -----------
                                // cases 1 & 2
                                // -----------

enum { NO_THREAD = 0, MAIN_THREAD = 1, CHILD_THREAD = 2 };

struct ThreadInfo {
    Obj             *d_lock_p;
    bsls::AtomicInt  d_firstIn;
    bsls::AtomicInt  d_threadDone;
};

extern "C" void *threadFunction(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;

    info->d_lock_p->lock();
    if (NO_THREAD == info->d_firstIn) {
        info->d_firstIn = CHILD_THREAD;
    }
    info->d_lock_p->unlock();
    info->d_threadDone = 1;

    return arg;
}

#endif  // BSLS_PLATFORM_OS_WINDOWS

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int    test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TEST
        //   Ensure that 'BslLockImpl_win32' works as expected.
        //
        // Concerns:
        //: 1 Following construction, the lock is in the unlocked state.
        //:
        //: 2 'lock' acquires the lock for exclusive access by the calling
        //:    thread.
        //:
        //: 3 'unlock' releases the lock making it available to other threads.
        //:
        // Plan:
        //: 1 Create a lock, 'mX', in the main thread, then create a child
        //:   thread that has access to 'mX'.  Each of the threads, upon
        //:   acquiring the lock on 'mX', atomically update a shared "first in"
        //:   variable if and only if it still has its initial value (0); both
        //:   threads use the 'lock' method.  Allow the child thread to run for
        //:   a brief time before locking in the main thread.  After joining
        //:   with the child thread, verify the "first in" variable has the
        //:   expected result.  Repeat, except this time lock 'mX' in the main
        //:   thread *before* creating the child thread.  (C-1..3)
        //
        // Testing:
        //   BslLockImpl_win32::BslLockImpl_win32();
        //   BslLockImpl_win32::~BslLockImpl_win32();
        //   void BslLockImpl_win32::lock();
        //   void BslLockImpl_win32::unlock();
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC TEST"
                            "\n==========\n");

#ifdef BSLS_PLATFORM_OS_WINDOWS
        // C-1..3
        {
            Obj mX;

            ThreadInfo info;
            info.d_lock_p     = &mX;
            info.d_firstIn    = NO_THREAD;
            info.d_threadDone = 0;

            ThreadId id = createThread(&threadFunction, &info);

            pause(&info.d_threadDone);

            mX.lock();                                                // LOCK
            if (NO_THREAD == info.d_firstIn) {
                info.d_firstIn = MAIN_THREAD;
            }
            mX.unlock();                                              // UNLOCK

            joinThread(id);

            ASSERT(CHILD_THREAD == info.d_firstIn);
        }

        {
            Obj mX;
            mX.lock();                                                // LOCK

            ThreadInfo info;
            info.d_lock_p     = &mX;
            info.d_firstIn    = NO_THREAD;
            info.d_threadDone = 0;

            ThreadId id = createThread(&threadFunction, &info);

            pause(&info.d_threadDone);

            if (NO_THREAD == info.d_firstIn) {
                info.d_firstIn = MAIN_THREAD;
            }
            mX.unlock();                                              // UNLOCK

            joinThread(id);

            ASSERT(MAIN_THREAD == info.d_firstIn);
        }
#endif  // BSLS_PLATFORM_OS_WINDOWS
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
