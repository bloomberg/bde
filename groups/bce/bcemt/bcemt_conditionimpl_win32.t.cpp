// bcemt_conditionimpl_win32.t.cpp                                    -*-C++-*-
#include <bcemt_conditionimpl_win32.h>

#ifdef BCES_PLATFORM__WIN32_THREADS

#include <bcemt_lockguard.h>
#include <bcemt_mutex.h>
#include <bces_atomictypes.h>
#include <bdetu_systemtime.h>

#include <bsl_c_errno.h>
#include <bsl_cstdlib.h>
#include <bsl_deque.h>
#include <bsl_iostream.h>

#include <windows.h>

using namespace BloombergLP;
using namespace bsl;

//-----------------------------------------------------------------------------
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int verbose;
int veryVerbose;

typedef bcemt_ConditionImpl<bces_Platform::Win32Threads> Obj;

extern "C" {
   typedef void *(*ThreadFunction)(void *);
}

#include <process.h>

struct StartArgs {
    ThreadFunction d_function;
    void* d_arg;
};

static unsigned _stdcall ThreadEntry (void* arg)
{
    StartArgs* args = (StartArgs*)arg;

    Sleep(100); // 100 ms - for our testing, let's just not deal with the
                // "invalid handle from too-quickly-executing thread" issue

    args->d_function(args->d_arg);
    free(arg);

    return 1;
}

struct PingPongArgs
{
    Obj* d_cond;
    bcemt_Mutex *d_lock;
    bces_AtomicInt d_count;

    PingPongArgs(Obj* cond, bcemt_Mutex *lock)
        : d_cond(cond), d_lock(lock), d_count(0)
    {}
};

enum {
    PINGPONG_ITER = 200
};

extern "C" {

void* pingPong(void* argp) {
    PingPongArgs *arg = (PingPongArgs*)argp;

    while (arg->d_count < PINGPONG_ITER) {
        arg->d_lock->lock();
        arg->d_cond->signal();
        ASSERT(0 == arg->d_cond->wait(arg->d_lock));
        arg->d_lock->unlock();

        ++arg->d_count;
    }
    return argp;
}

} // extern "C"

void My_CreateDetachedThread(ThreadFunction function,
                             void* userData) {
    StartArgs *args = (StartArgs*)malloc(sizeof(StartArgs));
    args->d_function = function;
    args->d_arg = userData;

    unsigned dummy;
    HANDLE h = (HANDLE)_beginthreadex(0, 1000000, ThreadEntry,
                                      args, 0, &dummy);
    CloseHandle(h);
}

// ----------------------------------------------------------------------------
// Case 1. Producer-consumer example

struct my_WorkItem {
    int d_item;  // represents work to perform
};

struct my_WorkQueue {
    deque<my_WorkItem> d_queue;  // queue of work requests
    bcemt_Mutex        d_mx;     // protects the shared queue
    Obj                d_cv;     // signals the existence of new work
};

struct Case1 {
    my_WorkQueue   *d_queue;
    bces_AtomicInt  d_done;

    Case1(my_WorkQueue* queue)
        : d_queue(queue), d_done(0)
    {}
};

enum {
    NUM_ITEMS = 1000
};

extern "C" void *producer(void *arg_p)
    // Receive commands from an external source, place them on
    // the shared 'my_WorkQueue' (passed as the specified void
    // pointer 'arg'), and return 'arg'.
{
    Case1* arg = (Case1*)arg_p;
    my_WorkQueue *queue = arg->d_queue;

    // For simplicity, the external source is modeled by a
    // for loop that generates NUM_ITEMS random numbers.

    for (int i = 0; i < NUM_ITEMS; ++i) {
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
    Sleep(ms);
}

extern "C" void *consumer(void *arg_p)
    // Consume the commands from the shared 'my_WorkQueue' (passed as
    // the specified void pointer 'arg') and return 0.  Note that this
    // function will continue processing commands until there are no
    // commands for at least one tenth of a second.
{
    Case1* arg = (Case1*)arg_p;
    my_WorkQueue *queue = arg->d_queue;

    int finished = 0, received = 0;
    while (!finished) {
        // Set the timeout to be one second from now.

        bdet_TimeInterval timeout = bdetu_SystemTime::now();
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
            // The condition variable was either signaled or timed out
            // and there are work requests in the queue.

            my_WorkItem item = queue->d_queue.front();
            queue->d_queue.pop_front();
            queue->d_mx.unlock();

            // Process the work requests.
            ++received;
        }
        else {
            // The wait timed out and the queue was empty.  Unlock the
            // mutex and return.

            queue->d_mx.unlock();
            finished = 1;
        }
    }
    ASSERT(NUM_ITEMS == received);
    if (verbose) {
        P(received);
    }

    arg->d_done = 1;
    return arg_p;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

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
        bcemt_Mutex lock;
        PingPongArgs args1(&x, &lock);
        PingPongArgs args2(&x, &lock);

        My_CreateDetachedThread(&pingPong, &args1);
        My_CreateDetachedThread(&pingPong, &args2);

        while (args1.d_count < PINGPONG_ITER-1 ||
               args2.d_count < PINGPONG_ITER-1) {
            My_Sleep(250);
        }

        // one of them is still waiting on the last 'pong'
        if (args1.d_count == PINGPONG_ITER) {
            ASSERT(args2.d_count == PINGPONG_ITER - 1);
            x.signal();
            My_Sleep(100);
            ASSERT(args2.d_count == PINGPONG_ITER);
        }
        else {
            ASSERT(args1.d_count == PINGPONG_ITER-1);
            ASSERT(args2.d_count == PINGPONG_ITER);
            x.signal();
            My_Sleep(100);
            ASSERT(args1.d_count == PINGPONG_ITER);
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

        const bdet_TimeInterval SLEEP_SECONDS(0.3);
        const double OVERSHOOT_MIN = -1e-5;
#if defined(BSLS_PLATFORM__OS_SOLARIS)
        const double OVERSHOOT_MAX = 0.10;
#else
        const double OVERSHOOT_MAX = 0.05;
#endif

        Obj condition;
        bcemt_Mutex     mutex;
        bcemt_LockGuard<bcemt_Mutex> guard(&mutex);

        for (int i = 0; i < 8; ++i) {
            const bdet_TimeInterval start = bdetu_SystemTime::now();
            const bdet_TimeInterval timeout = start + SLEEP_SECONDS;
            const bdet_TimeInterval minTimeout = timeout + OVERSHOOT_MIN;

            int sts;
            bdet_TimeInterval finish;

            int j;
            for (j = 0; j < 4; ++j) {
                sts = condition.timedWait(&mutex, timeout);
                finish = bdetu_SystemTime::now();
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
        //   int timedWait(bcemt_Mutex*, const bdet_TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "TimedWait() timeout test\n";
        }

        Obj cond;
        bcemt_Mutex mutex;
        bcemt_LockGuard<bcemt_Mutex> guard(&mutex);

        bdet_TimeInterval startT = bdetu_SystemTime::now();
        double start = startT.totalSecondsAsDouble();
        bdet_TimeInterval endT  = startT + bdet_TimeInterval(0.1);
        bdet_TimeInterval endT2 = startT + bdet_TimeInterval(0.11);
        int i;
        int sts;
        for (i = 1; 10 >= i; ++i) {
            sts = cond.timedWait(&mutex, endT);
            if (-1 == sts) {
                break;
            }
            if (bdetu_SystemTime::now() > endT2) {
                ASSERT(0 && "should have timed out by now");
                break;
            }
        }
        ASSERT(0 != sts);
        double finish = bdetu_SystemTime::now().totalSecondsAsDouble() - start;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
