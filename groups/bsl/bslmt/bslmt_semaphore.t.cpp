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
#include <bsls_platform.h>
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
    static bsls::Stopwatch sw;

    sw.reset();
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
            printf("%s%2d: %6.1f", jj ? ", " : "        //     ",
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
        //: 1 Benchmark the time, in nanoseconds, to create and destroy a
        //:   bslmt::Semaphore.
        //
        // Plan:
        //: 1 Write a loop that times a large number of semaphore create /
        //:   destroys, store 101 such trials in an array, sort it, then
        //:   display 11 of these results from min to max.
        //:
        //: 2 The clock on Windows seems to have coarser resolution, so aim for
        //    5 ms per sample on Unix and 20 ms per sample on Windows.
        //
        // Testing:
        //   CREATORS BENCHMARK
        //
        // Results:
        //     Times in nanoseconds per create-destroy: 0 is min, 100 is max.
        //   ------------------------------------------------------------------
        // Linux
        //     iterations: 4093791, mulFactor: 244.272
        //     SysTimes:- - - - - - - - - - - - - - - - - - - - - - - - - - - -
        //      0:    0.0, 10:    0.0, 20:    0.0, 30:    0.0
        //     40:    0.0, 50:    0.0, 60:    0.0, 70:    0.0
        //     80:    0.0, 90:    0.0, 100:    0.7
        //     UserTimes: - - - - - - - - - - - - - - - - - - - - - - - - - - -
        //      0:   10.6, 10:   10.8, 20:   11.0, 30:   11.2
        //     40:   11.4, 50:   11.4, 60:   11.6, 70:   11.8
        //     80:   12.2, 90:   12.7, 100:   16.7
        //     WallTimes: - - - - - - - - - - - - - - - - - - - - - - - - - - -
        //      0:   10.8, 10:   11.1, 20:   11.3, 30:   11.5
        //     40:   11.7, 50:   11.7, 60:   11.9, 70:   12.3
        //     80:   12.6, 90:   13.6, 100:   16.8
        //   ------------------------------------------------------------------
        // Solaris
        //     iterations: 893359, mulFactor: 1119.37
        //     SysTimes:- - - - - - - - - - - - - - - - - - - - - - - - - - - -
        //      0:    0.0, 10:    0.0, 20:    0.0, 30:    0.0
        //     40:    0.0, 50:    0.0, 60:    0.0, 70:    0.0
        //     80:    0.0, 90:    0.0, 100:    0.0
        //     UserTimes: - - - - - - - - - - - - - - - - - - - - - - - - - - -
        //      0:   55.7, 10:   56.2, 20:   56.4, 30:   56.5
        //     40:   56.6, 50:   56.8, 60:   57.0, 70:   57.3
        //     80:   58.1, 90:   59.1, 100:   64.3
        //     WallTimes: - - - - - - - - - - - - - - - - - - - - - - - - - - -
        //      0:   55.7, 10:   56.2, 20:   56.4, 30:   56.5
        //     40:   56.6, 50:   56.8, 60:   57.1, 70:   57.3
        //     80:   58.1, 90:   59.1, 100:   64.3
        //   ------------------------------------------------------------------
        // Ibm
        //     iterations: 15396, mulFactor: 64951.9
        //     SysTimes:- - - - - - - - - - - - - - - - - - - - - - - - - - - -
        //      0: 1127.3, 10: 1264.4, 20: 1326.8, 30: 1382.2
        //     40: 1425.6, 50: 1443.4, 60: 1460.7, 70: 1488.3
        //     80: 1523.6, 90: 1616.1, 100: 1729.2
        //     UserTimes: - - - - - - - - - - - - - - - - - - - - - - - - - - -
        //      0:  213.2, 10:  253.8, 20:  266.4, 30:  277.4
        //     40:  285.4, 50:  290.4, 60:  293.3, 70:  297.7
        //     80:  307.8, 90:  323.3, 100:  347.4
        //     WallTimes: - - - - - - - - - - - - - - - - - - - - - - - - - - -
        //      0: 3094.3, 10: 3162.3, 20: 3194.3, 30: 3207.8
        //     40: 3223.9, 50: 3237.3, 60: 3254.6, 70: 3298.9
        //     80: 3332.2, 90: 3361.3, 100: 3679.1
        //   ------------------------------------------------------------------
        // Windows
        //     iterations: 65050, mulFactor: 15372.8
        //     SysTimes:- - - - - - - - - - - - - - - - - - - - - - - - - - - -
        //      0:  720.6, 10:  960.8, 20:  960.8, 30: 1201.0
        //     40: 1201.0, 50: 1441.2, 60: 1441.2, 70: 1681.4
        //     80: 1681.4, 90: 1921.6, 100: 2882.4
        //     UserTimes: - - - - - - - - - - - - - - - - - - - - - - - - - - -
        //      0:  240.2, 10: 1201.0, 20: 1201.0, 30: 1441.2
        //     40: 1441.2, 50: 1681.4, 60: 1681.4, 70: 1921.6
        //     80: 1921.6, 90: 2161.8, 100: 2402.0
        //     WallTimes: - - - - - - - - - - - - - - - - - - - - - - - - - - -
        //      0: 2884.8, 10: 2937.3, 20: 2978.7, 30: 2995.1
        //     40: 3016.8, 50: 3044.7, 60: 3068.9, 70: 3088.8
        //     80: 3130.8, 90: 3204.5, 100: 3364.6
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

        for (; userTime < 0.040; iterations *= 10) {
            enum { k_BILLION = 1000 * 1000 * 1000 };

            if (veryVerbose) P(iterations);
            BSLS_ASSERT(iterations <= k_BILLION);
            u::benchmark(&sysTime, &userTime, &wallTime, iterations);
        }

        // Aim for 5 ms per call (20 ms on Windows) to 'u::benchmark' as
        // requested by the ticket.

#if defined(BSLS_PLATFORM_OS_UNIX)
        const double targetIterationTime = 0.005;
#else
        const double targetIterationTime = 0.020;
#endif
        iterations = static_cast<Uint64>(static_cast<double>(iterations) *
                                             (targetIterationTime / wallTime));

        // Amount to adjust the time by to get ns / iteration.

        const double mulFactor = 1e9 / static_cast<double>(iterations);

        bsl::printf("        //     iterations: %llu, mulFactor: %g\n",
                                                        iterations, mulFactor);

        for (int ii = 0; ii < u::k_NUM_SAMPLES; ++ii) {
            u::benchmark(&sysTime, &userTime, &wallTime, iterations);

            sysTimes [ii] = sysTime  * mulFactor;
            userTimes[ii] = userTime * mulFactor;
            wallTimes[ii] = wallTime * mulFactor;
        }

        bsl::sort(sysTimes  + 0, sysTimes  + u::k_NUM_SAMPLES);
        bsl::sort(userTimes + 0, userTimes + u::k_NUM_SAMPLES);
        bsl::sort(wallTimes + 0, wallTimes + u::k_NUM_SAMPLES);

        printf("        //     SysTimes:"
                  "- - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
        u::dumpTimes(sysTimes);
        printf("        //     UserTimes:"
                   " - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
        u::dumpTimes(userTimes);
        printf("        //     WallTimes:"
                   " - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
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
