// bslmt_conditionimpl_pthread.t.cpp                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_conditionimpl_pthread.h>

#ifdef BSLMT_PLATFORM_POSIX_THREADS

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_systemtime.h>

#include <bsl_cerrno.h>
#include <bsl_cstdlib.h>
#include <bsl_deque.h>
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
// This component is an implementation detail of 'bslmt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

int verbose;
int veryVerbose;

typedef bslmt::ConditionImpl<bslmt::Platform::PosixThreads> Obj;

extern "C" {
   typedef void *(*ThreadFunction)(void *);
}

struct PingPongArgs
{
    Obj* d_cond;
    bslmt::Mutex *d_lock;
    bsls::AtomicInt d_count;

    PingPongArgs(Obj *condition, bslmt::Mutex *lock)
        : d_cond(condition), d_lock(lock), d_count(0)
    {}
};

enum {
    k_PINGPONG_ITER = 200
};

extern "C" {

void *pingPong(void *argument) {
    PingPongArgs *arg = (PingPongArgs*)argument;

    while (arg->d_count < k_PINGPONG_ITER) {
        arg->d_lock->lock();
        arg->d_cond->signal();
        ASSERT(0 == arg->d_cond->wait(arg->d_lock));
        arg->d_lock->unlock();

        ++arg->d_count;
    }
    return argument;
}

}  // extern "C"

void My_CreateDetachedThread(ThreadFunction function, void *userData) {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,
                                PTHREAD_CREATE_DETACHED);
    pthread_t handle;
    int rc = pthread_create(&handle, &attr, function, userData);
    BSLS_ASSERT(0 == rc); // test invariant
}

// ----------------------------------------------------------------------------
//                     Case 1: Producer-consumer example
// ----------------------------------------------------------------------------

struct my_WorkItem {
    int d_item;  // represents work to perform
};

struct my_WorkQueue {
    deque<my_WorkItem> d_queue;  // queue of work requests
    bslmt::Mutex        d_mx;     // protects the shared queue
    Obj                d_cv;     // signals the existence of new work
};

struct Case1 {
    my_WorkQueue   *d_queue;
    bsls::AtomicInt  d_done;

    Case1(my_WorkQueue* queue)
        : d_queue(queue), d_done(0)
    {}
};

enum {
    k_NUM_ITEMS = 1000
};

extern "C" void *producer(void *arg_p)
    // Receive commands from an external source, place them on the shared
    // 'my_WorkQueue' (passed as the specified void pointer 'arg'), and return
    // 'arg'.
{
    Case1* arg = (Case1*)arg_p;
    my_WorkQueue *queue = arg->d_queue;

    // For simplicity, the external source is modeled by a for loop that
    // generates k_NUM_ITEMS random numbers.

    for (int i = 0; i < k_NUM_ITEMS; ++i) {
        my_WorkItem request;
        request.d_item = rand();
        queue->d_mx.lock();
        queue->d_queue.push_back(request);
        queue->d_mx.unlock();
        queue->d_cv.signal();
    }

    arg->d_done = 1;
    return arg_p;
}

void My_Sleep(int ms)
{
    timespec naptime;

    naptime.tv_sec = 0;
    naptime.tv_nsec = ms * 1000000;
    nanosleep(&naptime, 0);
}

extern "C" void *consumer(void *arg_p)
    // Consume the commands from the shared 'my_WorkQueue' (passed as the
    // specified void pointer 'arg') and return 0.  Note that this function
    // will continue processing commands until there are no commands for at
    // least one tenth of a second.
{
    Case1* arg = (Case1*)arg_p;
    my_WorkQueue *queue = arg->d_queue;

    int finished = 0, received = 0;
    while (!finished) {
        // Set the timeout to be one second from now.

        bsls::TimeInterval timeout = bsls::SystemTime::nowRealtimeClock();
        timeout.addMilliseconds(1000);

        // Wait for work requests to be added to the queue.

        queue->d_mx.lock();
        while (0 == queue->d_queue.size()) {
            int status = queue->d_cv.timedWait(&queue->d_mx, timeout);
            if (0 != status) {
                break;
            }
        }

        if (0 != queue->d_queue.size()) {
            // The condition variable was either signaled or timed out and
            // there are work requests in the queue.

            my_WorkItem item = queue->d_queue.front();
            queue->d_queue.pop_front();
            queue->d_mx.unlock();

            // Process the work requests.
            ++received;
        }
        else {
            // The wait timed out and the queue was empty.  Unlock the mutex
            // and return.

            queue->d_mx.unlock();
            finished = 1;
        }
    }
    ASSERT(k_NUM_ITEMS == received);
    if (verbose) {
        P(received);
    }

    arg->d_done = 1;
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
    case 4: {
        // --------------------------------------------------------------------
        // PING-PONG TEST
        //
        // Concerns: two threads can signal and wait (iteratively) on the same
        // condition variable.
        // --------------------------------------------------------------------
        if (verbose) cout << "\nPing-pong test" << endl;

        Obj x;
        bslmt::Mutex lock;
        PingPongArgs args1(&x, &lock);
        PingPongArgs args2(&x, &lock);

        My_CreateDetachedThread(&pingPong, &args1);
        My_CreateDetachedThread(&pingPong, &args2);

        while (args1.d_count < k_PINGPONG_ITER-1 ||
               args2.d_count < k_PINGPONG_ITER-1) {
            My_Sleep(250);
        }

        // one of them is still waiting on the last 'pong'
        if (args1.d_count == k_PINGPONG_ITER) {
            ASSERT(args2.d_count == k_PINGPONG_ITER - 1);
            x.signal();
            My_Sleep(100);
            ASSERT(args2.d_count == k_PINGPONG_ITER);
        }
        else {
            ASSERT(args1.d_count == k_PINGPONG_ITER-1);
            ASSERT(args2.d_count == k_PINGPONG_ITER);
            x.signal();
            My_Sleep(100);
            ASSERT(args1.d_count == k_PINGPONG_ITER);
        }
    } break;

    case 3: {
        // --------------------------------------------------------------------
        // TESTING CONDITION::TIMEDWAIT() ACCURACY
        //
        // Concerns:
        //   Test how accurate Condition::timedWait() is.
        //
        // Plan:
        //   Several times, timedWait for 0.3 seconds, then check how much
        //   time has passed and verify that it is within acceptable
        //   boundaries.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "TimedWait() accuracy test\n";
        }

        const bsls::TimeInterval SLEEP_SECONDS(0.3);
        const double OVERSHOOT_MIN = -1e-5;
#if defined(BSLS_PLATFORM_OS_SOLARIS)
        const double OVERSHOOT_MAX = 0.10;
#else
        const double OVERSHOOT_MAX = 0.05;
#endif

        Obj condition;
        bslmt::Mutex     mutex;
        bslmt::LockGuard<bslmt::Mutex> guard(&mutex);

        for (int i = 0; i < 8; ++i) {
            const bsls::TimeInterval start =
                                          bsls::SystemTime::nowRealtimeClock();
            const bsls::TimeInterval timeout = start + SLEEP_SECONDS;
            const bsls::TimeInterval minTimeout = timeout + OVERSHOOT_MIN;

            int sts;
            bsls::TimeInterval finish;

            int j;
            for (j = 0; j < 4; ++j) {
                sts = condition.timedWait(&mutex, timeout);
                finish = bsls::SystemTime::nowRealtimeClock();
                if (finish > minTimeout) {
                    break;
                }
            }
            ASSERT(j < 4);
            LOOP_ASSERT(sts, -1 == sts);

            double overshoot =
                       (finish - start - SLEEP_SECONDS).totalSecondsAsDouble();

            if (veryVerbose) P(overshoot);

            LOOP2_ASSERT(overshoot, j, overshoot >= OVERSHOOT_MIN);
            LOOP3_ASSERT(overshoot, j, OVERSHOOT_MAX,
                                                    overshoot < OVERSHOOT_MAX);
        }
    } break;

    case 2: {
        // --------------------------------------------------------------------
        // TESTING 'timedWait'
        //
        // Concerns:
        //   That timedWait, before timing out, waits at least the allotted
        //   time.
        //
        // Plan:
        //   Do a timedWait for a substantial time on a condition that never
        //   becomes true, and verify that the time has elapsed afterward.
        //
        // Testing:
        //   int timedWait(bslmt::Mutex*, const bsls::TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "TimedWait() timeout test\n";
        }

        Obj cond;
        bslmt::Mutex mutex;
        bslmt::LockGuard<bslmt::Mutex> guard(&mutex);

        bsls::TimeInterval startT = bsls::SystemTime::nowRealtimeClock();
        double start = startT.totalSecondsAsDouble();
        bsls::TimeInterval endT  = startT + bsls::TimeInterval(0.1);
        bsls::TimeInterval endT2 = startT + bsls::TimeInterval(0.11);
        int i;
        int sts;
        for (i = 1; 10 >= i; ++i) {
            sts = cond.timedWait(&mutex, endT);
            if (-1 == sts) {
                break;
            }
            if (bsls::SystemTime::nowRealtimeClock() > endT2) {
                ASSERT(0 && "should have timed out by now");
                break;
            }
        }
        ASSERT(0 != sts);
        double finish =
           bsls::SystemTime::nowRealtimeClock().totalSecondsAsDouble() - start;
        double end = endT.totalSecondsAsDouble() - start;
        LOOP2_ASSERT(finish, end, finish >= end);
        if (verbose) {
            cout << "Iterations: " << i << endl;
            cout << "Waited: " << finish << endl;
        }
    } break;

    case 1: {
        // ------------------------------------------------------------------
        // Producer-consumer example
        // ------------------------------------------------------------------

        if (verbose) cout << "\nProducer-consumer example" << endl;

        my_WorkQueue queue;

        Case1 producerArg(&queue), consumerArg(&queue);

        My_CreateDetachedThread(&producer,
                                &producerArg);

        My_CreateDetachedThread(&consumer,
                                &consumerArg);

        while (!consumerArg.d_done) {
            My_Sleep(200);
        }

        while (!producerArg.d_done) {
            My_Sleep(200);
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

#else

int main()
{
    return -1;
}

#endif

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
