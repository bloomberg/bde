// bslmt_semaphore.t.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_semaphore.h>

#include <bslmt_threadattributes.h>     // for testing only
#include <bslmt_threadutil.h>           // for testing only

#include <bslim_testutil.h>

#include <bsls_atomic.h>
#include <bsls_stopwatch.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_deque.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//
//
//-----------------------------------------------------------------------------
// [ 1] Breathing test
// [-1] CREATORS BENCHMARK

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
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------
//
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
        bsl::deque<int> d_queue;        // underlying queue
        bslmt::Semaphore d_mutexSem;     // mutual-access semaphore
        bslmt::Semaphore d_resourceSem;  // resource-availability semaphore

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
        int getInt();
            // Retrieve an integer from this 'IntQueue' object.  Integer values
            // are obtained from the queue in FIFO order.

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
    {
        d_mutexSem.post();
    }

    IntQueue::~IntQueue()
    {
        d_mutexSem.wait();  // Wait for potential modifier.
    }

    // MANIPULATORS
    int IntQueue::getInt()
    {
        // Waiting for resources.
        d_resourceSem.wait();

        // 'd_mutexSem' is used for exclusive access.
        d_mutexSem.wait();        // lock
        const int ret = d_queue.back();
        d_queue.pop_back();
        d_mutexSem.post();        // unlock

        return ret;
    }

    void IntQueue::pushInt(int value)
    {
        d_mutexSem.wait();
        d_queue.push_front(value);
        d_mutexSem.post();

        d_resourceSem.post();  // Signal we have resources available.
    }
//..

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslmt::Semaphore    Obj;
typedef bsls::Types::Uint64 Uint64;

// ============================================================================
//                   HELPER CLASSES/FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {
namespace u {

class WaitAndSetJob {

    bslmt::Semaphore *d_obj;
    bsls::AtomicInt *d_val;

  public:
    WaitAndSetJob(bslmt::Semaphore *obj,
                  bsls::AtomicInt *val)
    : d_obj(obj)
    , d_val(val)
    {
    }

    void operator()() const {
        d_obj->wait();
        (*d_val) = 1;
    }
};

void benchmark(double *sysTime,
               double *userTime,
               double *wallTime,
               Uint64  iterations)
{
    bsls::Stopwatch sw;

    sw.start(true);

    while (0 < iterations--) {
        Obj mX;
        (void) mX;
    }

    sw.stop();

    sw.accumulatedTimes(sysTime, userTime, wallTime);
}

enum { k_NUM_SAMPLES = 101 };

void dumpTimes(double times[k_NUM_SAMPLES])
{
    for (int ii = 0; ii < 3; ++ii)  {
        for (int jj = 0; jj < 4; ++jj) {
            const int    index = 10 * (4 * ii + jj);
            if (k_NUM_SAMPLES <= index) {
                break;
            }
            printf("%st[%2d] = %4.1f", jj ? "  " : "        //     ",
                                                          index, times[index]);
        }
        printf("\n");
    }
}

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

    switch (test) {  case 0:
      case 3: {
        IntQueue testQueue;

        testQueue.pushInt(1);
        ASSERT(1 == testQueue.getInt());
        testQueue.pushInt(2);
        ASSERT(2 == testQueue.getInt());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BLOCKING TEST
        //
        // Test that a thread can block on the semaphore.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "Blocking Test" << endl
                 << "=============" << endl;
        }
        {
            Obj X;
            bslmt::ThreadUtil::Handle h;
            bsls::AtomicInt flag;
            int rc = bslmt::ThreadUtil::create(&h,
                                               u::WaitAndSetJob(&X, &flag));
            ASSERT(0 == rc);
            bslmt::ThreadUtil::sleep(bsls::TimeInterval(1));
            ASSERT(0 == flag);
            X.post();
            bslmt::ThreadUtil::join(h);
            ASSERT(1 == flag);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "Breathing Test" << endl
                 << "==============" << endl;
        }
        {
            Obj X;
            X.post();
            X.post(2);
            X.wait();
            X.wait();
            ASSERT(0 == X.tryWait());
            ASSERT(0 != X.tryWait());
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // CREATORS BENCHMARK
        //
        // Concern:
        //: 1 Benchmark the time, in microseconds, to create and destror a
        //:   bslmt::Semaphore.
        //
        // Plan:
        //: 1 Write a loop that times a large number of semaphore create /
        //:   destroys, store 100 such trials in an array, sort it, then
        //:   display 10 of these results from min to max.
        //
        // Testing:
        //   CREATORS BENCHMARK
        //
        // Results:
        //     Times in nanoseconds per create-destroy: 0 is min, 100 is max.
        //   ------------------------------------------------------------------
        // Linux
        //     iterations: 3309056, mulFactor: 302.201
        //     SysTimes:- - - - - - - - - - - - - - - - -
        //     t[ 0] =  0.0  t[10] =  0.0  t[20] =  0.0  t[30] =  0.0
        //     t[40] =  0.0  t[50] =  0.0  t[60] =  0.0  t[70] =  0.2
        //     t[80] =  0.3  t[90] =  0.4  t[100] =  1.8
        //     UserTimes: - - - - - - - - - - - - - - - -
        //     t[ 0] = 10.2  t[10] = 11.1  t[20] = 11.4  t[30] = 11.8
        //     t[40] = 12.1  t[50] = 12.9  t[60] = 13.2  t[70] = 13.8
        //     t[80] = 14.7  t[90] = 15.2  t[100] = 16.9
        //     WallTimes: - - - - - - - - - - - - - - - -
        //     t[ 0] = 11.2  t[10] = 11.6  t[20] = 12.5  t[30] = 12.9
        //     t[40] = 13.5  t[50] = 14.5  t[60] = 15.0  t[70] = 16.2
        //     t[80] = 17.4  t[90] = 18.2  t[100] = 20.3
        //   ------------------------------------------------------------------
        // Solaris
        //     iterations: 862544, mulFactor: 1159.36
        //     SysTimes:- - - - - - - - - - - - - - - - -
        //     t[ 0] =  0.0  t[10] =  0.0  t[20] =  0.0  t[30] =  0.0
        //     t[40] =  0.0  t[50] =  0.0  t[60] =  0.0  t[70] =  0.0
        //     t[80] =  0.0  t[90] =  0.0  t[100] =  0.0
        //     UserTimes: - - - - - - - - - - - - - - - -
        //     t[ 0] = 56.2  t[10] = 56.6  t[20] = 56.8  t[30] = 56.9
        //     t[40] = 57.1  t[50] = 57.2  t[60] = 57.4  t[70] = 57.9
        //     t[80] = 58.3  t[90] = 58.8  t[100] = 61.8
        //     WallTimes: - - - - - - - - - - - - - - - -
        //     t[ 0] = 56.2  t[10] = 56.7  t[20] = 56.8  t[30] = 56.9
        //     t[40] = 57.1  t[50] = 57.3  t[60] = 57.4  t[70] = 57.9
        //     t[80] = 58.3  t[90] = 58.8  t[100] = 61.9
        //   ------------------------------------------------------------------
        // Ibm
        //     iterations: 15123, mulFactor: 66124.4
        //     SysTimes:- - - - - - - - - - - - - - - - -
        //     t[ 0] = 1100.6  t[10] = 1221.8  t[20] = 1266.3  t[30] = 1304.2
        //     t[40] = 1346.0  t[50] = 1364.1  t[60] = 1383.7  t[70] = 1403.0
        //     t[80] = 1431.3  t[90] = 1504.6  t[100] = 1636.3
        //     UserTimes: - - - - - - - - - - - - - - - -
        //     t[ 0] = 204.9  t[10] = 242.5  t[20] = 249.3  t[30] = 256.6
        //     t[40] = 264.5  t[50] = 272.9  t[60] = 275.2  t[70] = 280.2
        //     t[80] = 289.1  t[90] = 302.1  t[100] = 326.2
        //     WallTimes: - - - - - - - - - - - - - - - -
        //     t[ 0] = 2963.4  t[10] = 3064.1  t[20] = 3090.3  t[30] = 3138.6
        //     t[40] = 3162.1  t[50] = 3182.8  t[60] = 3206.9  t[70] = 3231.2
        //     t[80] = 3274.4  t[90] = 3355.6  t[100] = 3999.5
        // --------------------------------------------------------------------

        if (verbose) printf("CREATORS BENCHMARK\n"
                            "==================\n");

        printf("        //     Times in nanoseconds per create-destroy:"
                                                  " 0 is min, 100 is max.\n");

        double sysTimes[ u::k_NUM_SAMPLES];
        double userTimes[u::k_NUM_SAMPLES];
        double wallTimes[u::k_NUM_SAMPLES];

        Uint64 iterations = 1;

        double sysTime, userTime = 0, wallTime;

        for (; userTime < 0.020; iterations *= 10) {
            if (veryVerbose) P(iterations);
            BSLS_ASSERT(iterations <= 1e9);
            u::benchmark(&sysTime, &userTime, &wallTime, iterations);
        }

        // Aim for 5 ms per call to 'u::benchmark' as requested by the ticket.

        iterations = static_cast<Uint64>(static_cast<double>(iterations) *
                                                           (0.005 / wallTime));

        // Amount to adjust the time by to get ns / iteration.

        const double mulFactor = 1e9 / static_cast<double>(iterations);

        printf("        //     iterations: %llu, mulFactor: %g\n", iterations,
                                                                    mulFactor);

        for (int ii = 0; ii < u::k_NUM_SAMPLES; ++ii) {
            u::benchmark(&sysTime, &userTime, &wallTime, iterations);

            sysTimes [ii] = sysTime  * mulFactor;
            userTimes[ii] = userTime * mulFactor;
            wallTimes[ii] = wallTime * mulFactor;
        }

        bsl::sort(sysTimes  + 0, sysTimes  + u::k_NUM_SAMPLES);
        bsl::sort(userTimes + 0, userTimes + u::k_NUM_SAMPLES);
        bsl::sort(wallTimes + 0, wallTimes + u::k_NUM_SAMPLES);

        printf("        //     SysTimes:- - - - - - - - - - - - - - - - -\n");
        u::dumpTimes(sysTimes);        
        printf("        //     UserTimes: - - - - - - - - - - - - - - - -\n");
        u::dumpTimes(userTimes);        
        printf("        //     WallTimes: - - - - - - - - - - - - - - - -\n");
        u::dumpTimes(wallTimes);        
      } break;


      default: {
          testStatus = -1; break;
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
