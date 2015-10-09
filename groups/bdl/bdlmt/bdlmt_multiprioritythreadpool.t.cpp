// bdlmt_multiprioritythreadpool.t.cpp                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlmt_multiprioritythreadpool.h>

#include <bslim_testutil.h>

#include <bdlcc_queue.h>

#include <bdlma_concurrentpool.h>

#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_qlock.h>

#include <bdlt_currenttime.h>

#include <bsl_algorithm.h>
#include <bsl_cctype.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>            // atoi()
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_list.h>
#include <bsl_string.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocator.h>       // for testing only

#include <bsls_atomic.h>
#include <bsls_timeinterval.h>

#include <sys/stat.h>
#include <sys/types.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
// CREATORS
// [ 4] all constructors, destructor
//
// MANIPULATORS
// [ 2] enqueueJob(void (*)(), void *)
// [ 6] enqueueJob(ThreadFunctor)
// [ 3] drainJobs()
// [ 5] enableQueue(), disableQueue(), isEnabled()
// [ 5] startThreads(), stopThreads(), isStarted(), numStartedThreads()
// [ 5] suspendProcessing(), resumeProcessing(), isSuspended()
// [ 9] removeJobs()
// [ 9] shutdown()
//
// ACCESSORS
// [ 4] numThreads()
// [ 4] numPriorities()
// [ 6] numPendingJobs()
// [ 8] numActiveThreads()
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] // transitions between non-processing states
// [ 6] // enqueuing fails on disabled queue
// [ 7] // queue sorting
// [10] stress test
// [11] ignoring 'joinable' trait of attributes passed
// [12] usage example 2
// [13] usage example 1

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
//         GLOBAL TYPEDEFS, CONSTANTS, ROUTINES & MACROS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlmt::MultipriorityThreadPool Obj;

#define LO_ARRAY_LENGTH(array) (sizeof(array) / sizeof((array)[0]))

namespace {

// Have the default allocator be of different type than the allocator usually
// used -- then we can put breakpoints in bslma::TestAllocator code to find
// unintentional uses of the default allocator.

bslma::NewDeleteAllocator taDefault;
bslma::TestAllocator ta;

int verbose;
int veryVerbose;
int veryVeryVerbose;
int veryVeryVeryVerbose;

}  // close unnamed namespace

// ============================================================================
//                Classes for test case 13 -- usage example 1
// ============================================================================

namespace MULTIPRIORITYTHREADPOOL_CASE_13 {

// The idea here is we have a large number of jobs submitted in too little time
// for all of them to be completed.  All jobs take the same amount of time to
// complete, but there are two different priorities of jobs.  There are 100
// times more jobs of less urgent priority than of the more urgent priority,
// and there is more than enough time for the jobs of more urgent priority to
// be completed.  Verify that all the jobs of more urgent priority get
// completed while most of the jobs of less urgent priority do not.  This
// demonstrates that we can construct an arrangement where traffic of low
// priority, while massively more numerous, does not impede the progress of
// higher priority jobs.

bsls::AtomicInt     urgentJobsDone;
bsls::AtomicInt lessUrgentJobsDone;

extern "C" void *urgentJob(void *) {
    bslmt::ThreadUtil::microSleep(10000);          // 10 mSec

    ++urgentJobsDone;

    return 0;
}

extern "C" void *lessUrgentJob(void *) {
    bslmt::ThreadUtil::microSleep(10000);          // 10 mSec

    ++lessUrgentJobsDone;

    return 0;
}

}  // close namespace MULTIPRIORITYTHREADPOOL_CASE_13

// ============================================================================
//                Classes for test case 12 -- usage example 2
// ============================================================================

// The idea here is to have a multithreaded algorithm for calculating prime
// prime numbers.  This is just to serve as an illustration, although it works,
// it is not really any faster than doing it with a single thread.
//
// For every prime number P, we have to mark all multiples of it up in two
// ranges '[P .. P**2]', and '[P**2 .. TOP_NUMBER]' as non-prime.  For any
// P**2, if we can determine that all primes below P have marked all their
// multiples up to P**2, then we can scan that range and any unmarked values in
// it will be a new prime.  The we can start out with our first prime, 2, and
// mark all primes between it and 2**2 == 4, thus discovering 3 is prime.  Once
// we have marked all multiples of 2 and 3 below 3*3 == 9, we can then scan
// that range and discover 5 and 7 are primes, and repeat the process to
// discover bigger and bigger primes until we have covered an entire range, in
// this example all ints below TOP_NUMBER == 2000.

namespace MULTIPRIORITYTHREADPOOL_CASE_12 {

enum {
    TOP_NUMBER = 2000,
    NUM_PRIORITIES = 32
};

bool isStillPrime[TOP_NUMBER];
bsls::AtomicInt scannedTo[TOP_NUMBER];  // is P is a prime, what is the highest
                                        // multiple of P that we have marked
                                        // isStillPrime[P] = false;

bsls::AtomicInt maxPrimeFound;      // maximum prime we have identified so far
int primeNumbers[TOP_NUMBER];       // elements in the range
                                    // '0 .. numPrimeNumbers - 1' are the prime
                                    // numbers we have found so far
bsls::AtomicInt numPrimeNumbers;

bdlmt::MultipriorityThreadPool *threadPool;

bool          doneFlag;                 // set this flag to signal other jobs
                                        // that we're done
bslmt::Barrier doneBarrier(2);          // we wait on this barrier to signal
                                        // the main thread that we're done

struct Functor {
    static bslmt::Mutex s_mutex;
    int                d_numToScan;
    int                d_priority;
    int                d_limit;

    Functor(int numToScan)
    : d_numToScan(numToScan)
    , d_priority(0)
    {
        d_limit = (int) bsl::min((double) numToScan * numToScan,
                                                        (double) TOP_NUMBER);
    }

    void setNewPrime(int newPrime) {
        maxPrimeFound = newPrime;
        primeNumbers[numPrimeNumbers] = newPrime;
        ++numPrimeNumbers;

        if (2 * newPrime < TOP_NUMBER) {
            Functor f(newPrime);

            threadPool->enqueueJob(f, 0);
        }
    }

    void evaluateCandidatesForPrime() {
        if (maxPrimeFound > d_limit) {
            return;                                                   // RETURN
        }

        int numToScanI;
        for (numToScanI = numPrimeNumbers - 1; 0 < numToScanI; --numToScanI) {
            if (primeNumbers[numToScanI] == d_numToScan) {
                break;
            }
        }
        for (int i = numToScanI - 1; 0 < i; --i) {
            if (TOP_NUMBER < scannedTo[primeNumbers[i]]) {
                for (int j = i + 1; numPrimeNumbers > j; ++j) {
                    if (TOP_NUMBER == scannedTo[primeNumbers[j]]) {
                        scannedTo[primeNumbers[j]] = TOP_NUMBER + 1;
                    }
                    else {
                        break;
                    }
                }
                break;
            }

            if (scannedTo[primeNumbers[i]] < d_limit) {
                // Not all multiples of all prime numbers below us have been
                // adequately marked as nonPrime.  We cannot yet draw any new
                // conclusions about what is and what is not prime in this
                // range.

                // Resubmit ourselves to back of the priority queue so that
                // we'll get reevaluated when previous prime numbers are done
                // scanning.  Note we could get reenqueued several times.

                // Note that jobs marking the isStillPrime array are at
                // priority 0, while later incarnations that can only set new
                // primes are at priority 1 and keep getting resubmitted at
                // less and less urgent priorities until all their
                // prerequisites (which are at priority 0) are done.

                d_priority = bsl::min(NUM_PRIORITIES - 2, d_priority + 1);
                threadPool->enqueueJob(*this, d_priority);

                return;                                               // RETURN
            }
        }

        // everything up to d_limit that has not been marked nonPrime is prime

        bslmt::LockGuard<bslmt::Mutex> guard(&s_mutex);

        for (int i = maxPrimeFound + 1; d_limit > i; ++i) {
            if (isStillPrime[i]) {
                setNewPrime(i);
            }
        }

        if (TOP_NUMBER == d_limit && !doneFlag) {
            // We have successfully listed all primes below TOP_NUMBER.  Touch
            // the done barrier and our caller will then know that we are done
            // and shut down the queue.

            doneFlag = true;
            doneBarrier.wait();
        }
    }

    void operator()() {
        if (0 == d_priority) {
            bsls::AtomicInt& rScannedTo = scannedTo[d_numToScan];
#if 0
            bsls::AtomicInt& rScannedTo(scannedTo[d_numToScan]);
#endif

            for (int i = d_numToScan; d_limit > i; i += d_numToScan) {
                isStillPrime[i] = false;
                rScannedTo = i;
            }

            d_priority = 1;
            threadPool->enqueueJob(*this, d_priority);

            for (int i = d_limit; TOP_NUMBER > i; i += d_numToScan) {
                isStillPrime[i] = false;
                rScannedTo = i;
            }
            rScannedTo = TOP_NUMBER;
        }
        else {
            evaluateCandidatesForPrime();
        }
    }
};
bslmt::Mutex Functor::s_mutex;

}  // close namespace MULTIPRIORITYTHREADPOOL_CASE_12

// ============================================================================
//                         Classes for test case 11
// ============================================================================

namespace MULTIPRIORITYTHREADPOOL_CASE_11 {

struct Functor {
    bslmt::Barrier  *d_barrier;
    bsls::AtomicInt *d_jobsCompleted;
    void operator()() {
        d_barrier->wait();

        bslmt::ThreadUtil::microSleep(50 * 1000);       // 0.05 sec
        bslmt::ThreadUtil::yield();
        bslmt::ThreadUtil::microSleep(50 * 1000);       // 0.05 sec

        ++*d_jobsCompleted;
    }
};

}  // close namespace MULTIPRIORITYTHREADPOOL_CASE_11

// ============================================================================
//                         Classes for test case 10
// ============================================================================

namespace MULTIPRIORITYTHREADPOOL_CASE_10 {

enum {
    NUM_PRIORITIES = 2,
    PRIORITY_MASK = 1,
    NUM_POOL_THREADS = 2,
    NUM_PRODUCER_THREADS = 25
};

struct Worker {
    int                                  d_priority;
    int                                  d_submittedTime;
    int                                  d_doneTime;

    static bdlmt::MultipriorityThreadPool *s_pool;
    static bdlcc::Queue<Worker>           *s_doneQueue;
    static bsls::AtomicInt                s_time;

    Worker(int priority) {
        d_priority = priority;
    }

    int submitMe() {
        d_submittedTime = ++s_time;
        if (s_pool->enqueueJob(*this, d_priority)) {
            return -1;                                                // RETURN
        }

        return 0;
    }

    void operator()() {
        d_doneTime = ++s_time;
        s_doneQueue->pushBack(*this);
    }
};
bdlmt::MultipriorityThreadPool *Worker::s_pool = 0;
bdlcc::Queue<Worker>           *Worker::s_doneQueue = 0;
bsls::AtomicInt Worker::s_time;

struct ProducerThread {
    int                   d_workersPerProducer;

    static bslmt::Barrier *s_barrier;

    ProducerThread(int workersPerProducer)
    : d_workersPerProducer(workersPerProducer) {}

    void operator()() {
        s_barrier->wait();

        int i;
        for (i = 0; d_workersPerProducer > i; ++i) {
            Worker w(i & PRIORITY_MASK);

            if (w.submitMe()) {
                break;
            }
        }

        if (veryVerbose && d_workersPerProducer - i) {
            static bslmt::Mutex mutex;
            bslmt::LockGuard<bslmt::Mutex> lock(&mutex);

            cout << "Aborted with " << d_workersPerProducer - i <<
                                                    " submissions to go\n";
        }
    }
};
bslmt::Barrier *ProducerThread::s_barrier = 0;

}  // close namespace MULTIPRIORITYTHREADPOOL_CASE_10

// ============================================================================
//                          Classes for test case 8
// ============================================================================

namespace MULTIPRIORITYTHREADPOOL_CASE_8 {

bool veryVerboseCase8;

bslmt::Barrier barrier2(2);
bslmt::Barrier barrier8(8);

struct BlockFunctor {
    void operator()() {
        barrier2.wait();
        barrier8.wait();

#if 0
        static bslmt::Mutex *mutex_p;
        BSLMT_ONCE_DO {
            static bslmt::Mutex mutex;
            mutex_p = &mutex;
        }
        bslmt::LockGuard<bslmt::Mutex> lock(mutex_p);
#endif

        static bslmt::QLock mutex = BSLMT_QLOCK_INITIALIZER;
        bslmt::QLockGuard guard(&mutex);

        if (veryVerboseCase8) cout << "Thread finishing\n";
    }
};

}  // close namespace MULTIPRIORITYTHREADPOOL_CASE_8

// ============================================================================
//                           Classes for test case 5
//                   also partially used in cases 4, 6, and 7
// ============================================================================

namespace MULTIPRIORITYTHREADPOOL_CASE_5 {

long resultsVec[100];
bsls::AtomicInt resultsVecIdx;

extern "C" void *pushInt(void *arg)
{
    resultsVec[resultsVecIdx++] = (char *) arg - (char *) 0;

    return 0;
}

struct PushIntFunctor {
    int d_arg;

    void operator()() {
        resultsVec[resultsVecIdx++] = d_arg;
    }
};

// Generally check out that a threadpool is healthy, given the state it thinks
// it's in.
void checkOutPool(bdlmt::MultipriorityThreadPool *pool) {
    ASSERT(pool->isEnabled());
    ASSERT(0 == pool->numActiveThreads());
    LOOP_ASSERT(pool->numPendingJobs(), 0 == pool->numPendingJobs());

    if (pool->isStarted()) {
        ASSERT(16 == pool->numStartedThreads());
    }
    else {
        ASSERT(0  == pool->numStartedThreads());
    }

    if (pool->isStarted() && !pool->isSuspended()) {
        memset(resultsVec, 0x3f, sizeof(resultsVec));
        resultsVecIdx = 0;

        for (long i = 0; 10 > i; ++i) {
            pool->enqueueJob(&pushInt, (void *) (i * i), 1);
        }
        pool->drainJobs();

        ASSERT(10 == resultsVecIdx);
        sort(resultsVec, resultsVec + resultsVecIdx);
        for (int i = 0; 10 > i; ++i) {
            ASSERT(resultsVec[i] == i * i);
        }
    }
    else {
        for (long i = 0; 10 > i; ++i) {
            pool->enqueueJob(&pushInt, (void *) (i * i), 1);
        }
        ASSERT(0 == pool->numActiveThreads());
        bslmt::ThreadUtil::microSleep(10 * 1000);
        LOOP_ASSERT(pool->numPendingJobs(), 10 == pool->numPendingJobs());
        pool->removeJobs();
    }

    ASSERT(0 == pool->numPendingJobs());
}

}  // close namespace MULTIPRIORITYTHREADPOOL_CASE_5

// ============================================================================
//                          Classes for test case 3
// ============================================================================

namespace MULTIPRIORITYTHREADPOOL_CASE_3 {

long counter;

extern "C" void *sleepAndAmassCounterBy(void *arg)
{
    bslmt::ThreadUtil::microSleep(50 * 1000);    // 50 mSeconds

    counter += (char *) arg - (char *) 0;
    counter *= counter;

    return 0;
}

}  // close namespace MULTIPRIORITYTHREADPOOL_CASE_3

// ============================================================================
//                          Classes for test case 2
// ============================================================================

namespace MULTIPRIORITYTHREADPOOL_CASE_2 {

int callCount;
long counter;

extern "C" void *amassCounterBy(void *arg)
{
    ++callCount;

    counter += (char *) arg - (char *) 0;
    counter *= counter;

    return 0;
}

extern "C" void *waiter(void *arg) {
    static_cast<bslmt::Barrier *>(arg)->wait();

    return 0;
}

}  // close namespace MULTIPRIORITYTHREADPOOL_CASE_2

// ============================================================================
//                 Classes for test case 1 - breathing test
// ============================================================================

namespace MULTIPRIORITYTHREADPOOL_CASE_1 {

bsls::AtomicInt counter;

extern "C" void *incCounter(void *)
{
    ++counter;

    return 0;
}

}  // close namespace MULTIPRIORITYTHREADPOOL_CASE_1

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    bslma::DefaultAllocatorGuard guard(&taDefault);
    ASSERT(&taDefault == bslma::Default::defaultAllocator());

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 13: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        //
        // Concerns:
        //   That usage example 1 compiles and links.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "===============\n"
                    "Usage example 2\n"
                    "===============\n";
        }

        using namespace MULTIPRIORITYTHREADPOOL_CASE_13;

        bdlmt::MultipriorityThreadPool pool(20,  // threads
                                            2,   // priorities
                                            &ta);

        bsls::TimeInterval finishTime = bdlt::CurrentTime::now() + 0.5;
        pool.startThreads();

        for (int i = 0; 100 > i; ++i) {
            for (int j = 0; 100 > j; ++j) {
                pool.enqueueJob(&lessUrgentJob, (void *) 0, 1); // less urgent
                                                                // priority
            }
            pool.enqueueJob(&urgentJob, (void *) 0, 0); // urgent priority
        }

        bslmt::ThreadUtil::sleep(finishTime - bdlt::CurrentTime::now());
        pool.shutdown();

        if (verbose) {
            bsl::cout << "Jobs done: urgent: " << urgentJobsDone <<
                          ", less urgent: " << lessUrgentJobsDone << bsl::endl;
        }
      }  break;
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2
        //
        // Concerns:
        //   That usage example 2 compiles and links.
        // --------------------------------------------------------------------

        using namespace MULTIPRIORITYTHREADPOOL_CASE_12;

        double startTime = bdlt::CurrentTime::now().totalSecondsAsDouble();

        for (int i = 0; TOP_NUMBER > i; ++i) {
            isStillPrime[i] = true;
            scannedTo[i] = 0;
        }

        scannedTo[0] = TOP_NUMBER + 1;
        scannedTo[1] = TOP_NUMBER + 1;

        maxPrimeFound = 2;
        primeNumbers[0] = 2;
        numPrimeNumbers = 1;
        doneFlag = false;

        threadPool =
            new (ta) bdlmt::MultipriorityThreadPool(20, NUM_PRIORITIES, &ta);
        threadPool->startThreads();

        double startJobs = bdlt::CurrentTime::now().totalSecondsAsDouble();

        Functor f(2);
        threadPool->enqueueJob(f, 0);

        doneBarrier.wait();

        double finish = bdlt::CurrentTime::now().totalSecondsAsDouble();

        threadPool->shutdown();
        ta.deleteObjectRaw(threadPool);

        if (verbose) {
            double now = bdlt::CurrentTime::now().totalSecondsAsDouble();
            printf("Runtime: %g seconds, %g seconds w/o init & cleanup\n",
                   now - startTime,
                   finish - startJobs);

            printf("%d prime numbers below %d:", (int) numPrimeNumbers,
                                                              TOP_NUMBER);

            for (int i = 0; numPrimeNumbers > i; ++i) {
                printf("%s%4d", 0 == i % 10 ? "\n    " : ", ",
                       primeNumbers[i]);
            }
            printf("\n");
        }
      }  break;
      case 11: {
        // --------------------------------------------------------------------
        // JOINABLE ATTRIBUTE IGNORED TEST
        //
        // Concerns:
        //   That the joinable/detached characteristic of attributes passed
        //   to the threadpool is ignored.
        //
        // Plan:
        //   Create threadpools with attributes specified, both with joinable
        //   and detached state, and in both cases verify that 'stopThreads()'
        //   blocks until all started jobs have been completed.
        // --------------------------------------------------------------------

        using namespace MULTIPRIORITYTHREADPOOL_CASE_11;

        if (verbose) {
            cout << "===================================\n"
                    "Testing attribute qualities ignored\n"
                    "===================================\n";
        }

        enum {
            NUM_THREADS = 10
        };

        enum {
            ATTRIB_DEFAULT,
            ATTRIB_DETACHED,
            ATTRIB_JOINABLE
        };

        bslmt::Barrier barrier(NUM_THREADS + 1);
        bsls::AtomicInt jobsCompleted;

        Functor functor;

        functor.d_barrier = &barrier;
        functor.d_jobsCompleted = &jobsCompleted;

        for (int attrState = ATTRIB_JOINABLE; attrState >= ATTRIB_DEFAULT;
                                                                 --attrState) {
            bslma::TestAllocator localTa;

            bslmt::ThreadAttributes attrib;

            if (ATTRIB_DETACHED == attrState) {
                attrib.setDetachedState(
                                   bslmt::ThreadAttributes::e_CREATE_DETACHED);
            }
            else if (ATTRIB_JOINABLE == attrState) {
                attrib.setDetachedState(
                                   bslmt::ThreadAttributes::e_CREATE_JOINABLE);
            }

            jobsCompleted = 0;

            bdlmt::MultipriorityThreadPool pool(NUM_THREADS,
                                                1,  // # priorities
                                                attrib,
                                                &localTa);

            pool.startThreads();
            for (int i = 0; NUM_THREADS > i; ++i) {
                pool.enqueueJob(functor, 0);
            }

            barrier.wait();     // we know all threads have started jobs when
                                // we pass this point

            pool.stopThreads();

            ASSERT(NUM_THREADS == jobsCompleted); // verifies that the stop
                                                  // blocked until all jobs
                                                  // were completed

            if (veryVerbose) {
                cout << "Pass " << attrState << " completed\n";
            }
        }
      }  break;
      case 10: {
        // --------------------------------------------------------------------
        // STRESS TEST
        //
        // Concerns:
        //   Run many jobs on a threadpool, verify that more urgent jobs
        //   get done faster than less urgent ones.
        //
        // Plan:
        //   Using an atomicInt as a 'timer', have a lightweight job that is
        //   a functor that records two times -- when it is submitted to the
        //   threadpool, and when the functor is executed.  Submit otherwise
        //   identical jobs of varying priorities, and observe at the end
        //   that jobs of more urgent priority got executed faster than
        //   jobs of lower priority.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "===========\n"
                    "Stress Test\n"
                    "===========\n";
        }

        using namespace MULTIPRIORITYTHREADPOOL_CASE_10;

        bslma::TestAllocator taDefaultLocal;
        bslma::DefaultAllocatorGuard guard(&taDefaultLocal);

        bdlcc::Queue<Worker> doneQueue(&ta);
        Worker::s_doneQueue = &doneQueue;

        enum {
            NUM_WORKERS_PER_PRODUCER = 1000
        };

        bdlmt::MultipriorityThreadPool pool(NUM_POOL_THREADS,
                                            NUM_PRIORITIES,
                                            &ta);
        Worker::s_pool = &pool;

        bslmt::Barrier barrier(NUM_PRODUCER_THREADS + 1);
        ProducerThread::s_barrier = &barrier;

        bslmt::ThreadUtil::Handle handles[NUM_PRODUCER_THREADS];

        for (int i = 0; NUM_PRODUCER_THREADS > i; ++i) {
            ProducerThread producer(NUM_WORKERS_PER_PRODUCER);

            bslmt::ThreadUtil::create(&handles[i], producer);
        }

        pool.startThreads();

        LOOP_ASSERT(taDefaultLocal.numBytesMax(),
                                            0 == taDefaultLocal.numBytesMax());
        ASSERT(0 == Worker::s_time);

        barrier.wait();         // set all producer threads loose
        double startTime = bdlt::CurrentTime::now().totalSecondsAsDouble();

        for (int i = 0; NUM_PRODUCER_THREADS > i; ++i) {
            bslmt::ThreadUtil::join(handles[i]);
        }
        pool.drainJobs();
        pool.stopThreads();

        LOOP_ASSERT(taDefaultLocal.numBytesInUse(),
                                          0 == taDefaultLocal.numBytesInUse());

        if (verbose) {
            cout << doneQueue.queue().length() << " mini-jobs processed in " <<
                bdlt::CurrentTime::now().totalSecondsAsDouble() - startTime <<
                                                                " seconds\n";
            cout << "Atomictime = " << Worker::s_time << endl;
        }

        ASSERT(doneQueue.queue().length() == NUM_WORKERS_PER_PRODUCER *
                                                        NUM_PRODUCER_THREADS);

        int dataPoints[ NUM_PRIORITIES];
        double averages[NUM_PRIORITIES];        // starts out sums

        memset(dataPoints, 0, sizeof(dataPoints));
        memset(averages,   0, sizeof(averages));

        while (0 < doneQueue.queue().length()) {
            Worker w = doneQueue.popFront();

            ++dataPoints[w.d_priority];
            averages[    w.d_priority] += w.d_doneTime - w.d_submittedTime;
        }
        for (int i = 0; NUM_PRIORITIES > i; ++i) {
            if (dataPoints[i]) {
                averages[i] /= dataPoints[i];
            }
        }

        if (verbose) {
            T_ P(averages[0]) T_ P(averages[1])
        }

        LOOP2_ASSERT(averages[0], averages[1], averages[0] < averages[1]);

        LOOP_ASSERT(taDefaultLocal.numBytesInUse(),
                                          0 == taDefaultLocal.numBytesInUse());
      }  break;
      case 9: {
        // --------------------------------------------------------------------
        // REMOVEJOBS AND SHUTDOWN
        //
        // Concerns:
        //   That removeJobs and shutdown can successfully cancel enqueued
        //   jobs before they execute.
        //
        // Plan:
        //   Create a threadpool.  Before starting it, enqueue a bunch of
        //   jobs.  Then call 'removeJobs()'.  Then start and drain the
        //   threadpool, and verify that none of the jobs have run.
        //
        //   Repeat the experiment, except using 'shutdown()' instead of
        //   'removeJobs()'.
        //
        // Testing:
        //   removeJobs()
        //   shutdown()
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "================================\n"
                    "removeJobs() and shutdown() test\n"
                    "================================\n";
        }

        using namespace MULTIPRIORITYTHREADPOOL_CASE_5;

        bdlmt::MultipriorityThreadPool pool(1, 1, &ta);

        memset(resultsVec, 0x8f, sizeof resultsVec);
        resultsVecIdx = 0;

        pool.enqueueJob(&pushInt, (void *) 0, 0);
        pool.enqueueJob(&pushInt, (void *) 0, 0);
        pool.enqueueJob(&pushInt, (void *) 0, 0);
        pool.enqueueJob(&pushInt, (void *) 0, 0);

        ASSERT(0 == resultsVecIdx);
        ASSERT(4 == pool.numPendingJobs());

        pool.removeJobs();

        if (verbose) {
            cout << "After removeJobs(): " << pool.numPendingJobs() <<
                                                                     " jobs\n";
        }

        ASSERT(0 == resultsVecIdx);
        ASSERT(0 == pool.numPendingJobs());

        pool.startThreads();
        pool.drainJobs();

        ASSERT(0 == resultsVecIdx);
        ASSERT(0 == pool.numPendingJobs());

        pool.suspendProcessing();
        pool.removeJobs();

        pool.enqueueJob(&pushInt, (void *) 0, 0);
        pool.enqueueJob(&pushInt, (void *) 0, 0);
        pool.enqueueJob(&pushInt, (void *) 0, 0);
        pool.enqueueJob(&pushInt, (void *) 0, 0);

        ASSERT(0 == resultsVecIdx);
        ASSERT(4 == pool.numPendingJobs());

        pool.shutdown();

        if (verbose) {
            cout << "After shutdown(): " << pool.numPendingJobs() << " jobs\n";
        }

        ASSERT(0 == resultsVecIdx);
        ASSERT(0 == pool.numPendingJobs());

        pool.startThreads();
        pool.resumeProcessing();
        pool.enableQueue();

        pool.drainJobs();

        ASSERT(0 == resultsVecIdx);
        ASSERT(0 == pool.numPendingJobs());

        pool.shutdown();
      }  break;
      case 8: {
        // --------------------------------------------------------------------
        // NUMACTIVETHREADS ACCESSOR
        //
        // Concerns:
        //   That numActiveThreads() properly reflects the number of active
        //   threads.
        //
        // Plan:
        //   Create two barriers, one with a threshold of 2 and one with
        //   a much high threshold.  Create a threadpool with a large number
        //   of threads.  Create a functor that will block first on the first
        //   barrier then on the second.  Repeatedly submit the functor and
        //   block on the first barrier, thus ensuring the job has started.
        //   Observe then that numActiveThreads() is equal to the number
        //   of executing jobs.  Eventually reach the higher threshold and
        //   allow all jobs to finish.
        //
        // Testing:
        //   numActiveThreads()
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "=====================\n"
                    "numActiveThreads test\n"
                    "=====================\n";
        }

        using namespace MULTIPRIORITYTHREADPOOL_CASE_8;

        enum {
            NUM_THREADS = 7
        };

        veryVerboseCase8 = veryVerbose;

        bdlmt::MultipriorityThreadPool pool(NUM_THREADS,
                                            1,  // single priority
                                            &ta);

        pool.startThreads();

        for (int i = 0; 7 > i; ++i) {
            BlockFunctor bfInner;

            pool.enqueueJob(bfInner, 0);

            barrier2.wait();

            ASSERT(i + 1 == pool.numActiveThreads());
        }
        barrier8.wait();

        pool.stopThreads();
        ASSERT(0 == pool.numActiveThreads());
      }  break;
      case 7: {
        // --------------------------------------------------------------------
        // QUEUE SORTING
        //
        // Concerns:
        //   That jobs in the queue are sorted by priority.
        //
        // Plan:
        //   Create a single thread threadpool.  Before starting it, enqueue
        //   a bunch of distinctly identifiable jobs with different
        //   priorities.  Then start and drain the threadpool, then verify
        //   that all the jobs have been executed in exactly the right order.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "==================\n"
                    "queue sorting test\n"
                    "==================\n";
        }

        using namespace MULTIPRIORITYTHREADPOOL_CASE_5;

        static const long scramble[] = { 5, 8, 3, 1, 7, 9, 0, 4, 6, 2 };
            // ints 0-9 in scrambled order
        const int scrambleLen = sizeof scramble / sizeof scramble[0];
        const char garbageVal = 0x8f;

        bdlmt::MultipriorityThreadPool pool(1,            // single thread
                                            scrambleLen,  // priorities
                                            &ta);

        memset(resultsVec, garbageVal, sizeof resultsVec);
        resultsVecIdx = 0;

        for (int i = 0; scrambleLen > i; ++i) {
            pool.enqueueJob(&pushInt,
                            (void *)(scramble[i] * scramble[i]),
                            (int)scramble[i]);
        }

        ASSERT(scrambleLen == pool.numPendingJobs());
        pool.startThreads();
        pool.drainJobs();

        ASSERT(scrambleLen == resultsVecIdx);

        for (int i = 0; scrambleLen > i; ++i) {
            LOOP2_ASSERT(i, resultsVec[i], i * i == resultsVec[i]);
        }

        if (verbose) {
            cout << "Results: ";
            for (int i = 0; resultsVecIdx > i; ++i) {
                cout << (i ? ", " : "") << resultsVec[i];
            }
            cout << endl;
        }

        pool.stopThreads();
      }  break;
      case 6: {
        // --------------------------------------------------------------------
        // FURTHER TESTING OF FUNDAMENTAL THREADPOOL STATES
        //
        // Concerns:
        //   That transitions between states properly affect enqueued jobs.
        //
        // Plan:
        //   Attempt to enqueue a job into a disabled queue, verify that it
        //   fails, the job does not run, and is not in the queue.
        //   Then, successfully enqueue several jobs while the queue is
        //   stopped and suspended, then start the queue, then stop the
        //   queue, then resume, then suspend, verifying the entire time
        //   that the job never disappears
        //   or gets processed.  Eventually start and resume the queue, and
        //   verify the jobs get executed.
        //
        // Testing:
        //   numPendingJobs()
        //   enqueueJob(ThreadFunctor)
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "====================\n"
                    "Intensive state test\n"
                    "====================\n";
        }

        using namespace MULTIPRIORITYTHREADPOOL_CASE_5;

        enum {
            NUM_THREADS = 8,
            NUM_PRIORITIES = 4,
            GARBAGE_VAL = 0x8f,
            NUM_JOBS = 10,
            MAX_LOOP = 4
        };

        int ii;
        for (ii = 0; ii <= MAX_LOOP; ++ii) {
            bool startOverFromScratch = false;
            bdlmt::MultipriorityThreadPool pool(NUM_THREADS,
                                                NUM_PRIORITIES,
                                                &ta);

            ASSERT(pool.isEnabled());
            ASSERT(!pool.isStarted());
            ASSERT(!pool.isSuspended());
            checkOutPool(&pool);

            pool.disableQueue();

            ASSERT(!pool.isEnabled());

            PushIntFunctor pif;
            pif.d_arg = 0;

            int sts = pool.enqueueJob(pif, 0);
            ASSERT(sts);    // should fail

            ASSERT(0 == pool.numPendingJobs());

            pool.enableQueue();

            ASSERT(pool.isEnabled());
            ASSERT(!pool.isStarted());
            ASSERT(!pool.isSuspended());
            ASSERT(0 == pool.numStartedThreads());

            pool.suspendProcessing();
            pool.removeJobs();

            ASSERT(!pool.isStarted());
            ASSERT(pool.isSuspended());
            ASSERT(0 == pool.numStartedThreads());

            memset(resultsVec, GARBAGE_VAL, sizeof resultsVec);
            resultsVecIdx = 0;

            // push jobs.  The queue is not started, so they won't run
            for (int i = 0; NUM_JOBS > i; ++i) {
                pif.d_arg = i * i;
                sts = pool.enqueueJob(pif, 0);
                ASSERT(!sts);

                ASSERT(1 + i == pool.numPendingJobs());
            }

            // now go back and forth between states of the queue that don't
            // execute jobs and verify the jobs have not begun executing

            for (int j = 0; 10 > j; ++j) {
                if (pool.startThreads()) {
                    startOverFromScratch = true;
                    if (verbose) {
                        P_(L_) P_(ii) P(j)
                    }
                    break;
                }

                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(10 * 1000);

                ASSERT(pool.isStarted());
                ASSERT(pool.isSuspended());
                ASSERT(NUM_THREADS == pool.numStartedThreads());
                ASSERT(0 == resultsVecIdx);
                ASSERT(NUM_JOBS == pool.numPendingJobs());

                pool.stopThreads();

                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(10 * 1000);

                ASSERT(!pool.isStarted());
                ASSERT(pool.isSuspended());
                ASSERT(0 == pool.numStartedThreads());
                ASSERT(0 == resultsVecIdx);
                ASSERT(NUM_JOBS == pool.numPendingJobs());

                pool.resumeProcessing();

                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(10 * 1000);

                ASSERT(!pool.isStarted());
                ASSERT(!pool.isSuspended());
                ASSERT(0 == pool.numStartedThreads());
                ASSERT(0 == resultsVecIdx);
                ASSERT(NUM_JOBS == pool.numPendingJobs());

                pool.suspendProcessing();

                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(10 * 1000);

                ASSERT(!pool.isStarted());
                ASSERT(pool.isSuspended());
                ASSERT(0 == pool.numStartedThreads());
                ASSERT(0 == resultsVecIdx);
                ASSERT(NUM_JOBS == pool.numPendingJobs());
            }
            if (startOverFromScratch) {
                continue;
            }

            // repeat that last experiment, not as many times, redundantly
            // calling the state changes.

            for (int j = 0; 5 > j; ++j) {
                if (pool.startThreads()) {
                    startOverFromScratch = true;
                    if (verbose) {
                        P_(L_) P_(ii) P(j)
                    }
                    break;
                }
                ASSERT(0 == pool.startThreads());

                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(10 * 1000);

                ASSERT(pool.isStarted());
                ASSERT(pool.isSuspended());
                ASSERT(NUM_THREADS == pool.numStartedThreads());
                ASSERT(0 == resultsVecIdx);
                ASSERT(NUM_JOBS == pool.numPendingJobs());

                pool.stopThreads();
                pool.stopThreads();

                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(10 * 1000);

                ASSERT(!pool.isStarted());
                ASSERT(pool.isSuspended());
                ASSERT(0 == pool.numStartedThreads());
                ASSERT(0 == resultsVecIdx);
                ASSERT(NUM_JOBS == pool.numPendingJobs());

                pool.resumeProcessing();
                pool.resumeProcessing();

                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(10 * 1000);

                ASSERT(!pool.isStarted());
                ASSERT(!pool.isSuspended());
                ASSERT(0 == pool.numStartedThreads());
                ASSERT(0 == resultsVecIdx);
                ASSERT(10 == pool.numPendingJobs());

                pool.suspendProcessing();
                pool.suspendProcessing();

                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(10 * 1000);

                ASSERT(!pool.isStarted());
                ASSERT(pool.isSuspended());
                ASSERT(0 == pool.numStartedThreads());
                ASSERT(0 == resultsVecIdx);
                ASSERT(NUM_JOBS == pool.numPendingJobs());
            }
            if (startOverFromScratch) {
                continue;
            }

            if (pool.startThreads()) {
                if (verbose) {
                    P_(L_) P(ii)
                }
                continue;
            }
            pool.resumeProcessing();
            pool.drainJobs();

            ASSERT(0 == pool.numPendingJobs());
            ASSERT(NUM_JOBS == resultsVecIdx);

            sort(resultsVec, resultsVec + resultsVecIdx);
            for (int i = 0; resultsVecIdx > i; ++i) {
                ASSERT(resultsVec[i] == i * i);
            }

            if (verbose) {
                cout << "First pass: ";
                for (int i = 0; resultsVecIdx > i; ++i) {
                    cout << (i ? ", " : "") << resultsVec[i];
                }
                cout << endl;
            }

            // redundant state transitions, then repeat the jobs
            ASSERT(pool.isStarted());
            ASSERT(0 == pool.startThreads());
            pool.resumeProcessing();

            // push jobs.  The queue is ready to go, so they'll start right
            // away
            memset(resultsVec, GARBAGE_VAL, sizeof resultsVec);
            resultsVecIdx = 0;

            for (int i = 0; NUM_JOBS > i; ++i) {
                pif.d_arg = i * i;
                sts = pool.enqueueJob(pif, 0);
                ASSERT(!sts);
            }
            pool.drainJobs();

            ASSERT(0 == pool.numPendingJobs());
            ASSERT(10 == resultsVecIdx);

            sort(resultsVec, resultsVec + resultsVecIdx);
            for (int i = 0; resultsVecIdx > i; ++i) {
                ASSERT(resultsVec[i] == i * i);
            }

            if (verbose) {
                cout << "Second pass: ";
                for (int i = 0; resultsVecIdx > i; ++i) {
                    cout << (i ? ", " : "") << resultsVec[i];
                }
                cout << endl;
            }

            pool.stopThreads();

            break;
        }
        ASSERT(ii <= MAX_LOOP);
        if (verbose) { P_(L_) P(ii) }
      }  break;
      case 5: {
        // --------------------------------------------------------------------
        // FIRST TESTING OF BASIC ACCESSORS THROUGH FUNDAMENTAL THREADPOOL
        // STATES
        //
        // Concerns:
        //   The threadpool has 3 orthogonal properties, therefore 8 states
        //   it can be in.  The enable/disable axis is trivially independent
        //   of the other two and can be tested separately.  The
        //   started/stopped axis and the suspended/resumed axis interact in
        //   complicated ways, so it will be necessary to move through all
        //   possible transitions between the four states created by those
        //   two properties, verifying that it does not hang, and that the
        //   accessors properly reflect the state at any time.
        //
        // Testing:
        //   enableQueue()
        //   disableQueue()
        //   isEnabled()
        //   startThreads()
        //   stopThreads()
        //   isStarted()
        //   numStartedThreads()
        //   suspendProcessing()
        //   resumeProcessing()
        //   isSuspended()
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "==========\n"
                    "State test\n"
                    "==========\n";
        }

        using namespace MULTIPRIORITYTHREADPOOL_CASE_5;

        bdlmt::MultipriorityThreadPool pool(16,  // num threads
                                            4,   // num priorities
                                            &ta);

        ASSERT(pool.isEnabled());
        ASSERT(!pool.isStarted());
        ASSERT(!pool.isSuspended());
        checkOutPool(&pool);

        pool.disableQueue();

        ASSERT(!pool.isEnabled());

        pool.enableQueue();

        ASSERT(pool.isEnabled());

        ASSERT(!pool.isStarted());
        ASSERT(!pool.isSuspended());
        checkOutPool(&pool);

        pool.startThreads();

        ASSERT(pool.isStarted());
        ASSERT(!pool.isSuspended());
        checkOutPool(&pool);

        pool.suspendProcessing();
        pool.removeJobs();

        ASSERT(pool.isStarted());
        ASSERT(pool.isSuspended());
        checkOutPool(&pool);

        pool.stopThreads();
        pool.removeJobs();

        ASSERT(!pool.isStarted());
        ASSERT(pool.isSuspended());
        checkOutPool(&pool);

        pool.resumeProcessing();

        ASSERT(!pool.isStarted());
        ASSERT(!pool.isSuspended());
        checkOutPool(&pool);

        // now go around the same 4 states in the opposite direction

        pool.suspendProcessing();
        pool.removeJobs();

        ASSERT(!pool.isStarted());
        ASSERT(pool.isSuspended());
        checkOutPool(&pool);

        pool.startThreads();

        ASSERT(pool.isStarted());
        ASSERT(pool.isSuspended());
        checkOutPool(&pool);

        pool.resumeProcessing();

        ASSERT(pool.isStarted());
        ASSERT(!pool.isSuspended());
        checkOutPool(&pool);

        pool.stopThreads();
        pool.removeJobs();

        ASSERT(!pool.isStarted());
        ASSERT(!pool.isSuspended());
        checkOutPool(&pool);

        pool.startThreads();

        ASSERT(pool.isStarted());
        ASSERT(!pool.isSuspended());
        checkOutPool(&pool);

        pool.stopThreads();
      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ALL POSSIBLE CONSTRUCTORS AND NUMTHREADS
        //
        // Concerns:
        //   Test all combinations of args in the constructors with a range
        //   of values of numThreads, and that numThreads() accurately reflects
        //   the constructed number of threads.
        //
        // Plan:
        //   Go in a 2 loops nested loops, the outer loop selecting all
        //   possible prototypes of the constructor, the inner loop selecting
        //   multiple possible values of numThreads and multiple possible
        //   values of priorities.  Verify that numThreads() and
        //   numPriorities() reflect the constructed values.  Then do a
        //   simple test on the created queue and verify it works.  Test
        //   both an attempt to set the attributes to attached and detached
        //   and that the queue works reasonably well in either case.
        //
        // Testing:
        //   bdlmt::MultipriorityThreadPool(numThreads, numPriorities, alloc)
        //   bdlmt::MultipriorityThreadPool(numThreads, numPriorities,
        //                                                   attributes, alloc)
        //   bdlmt::MultipriorityThreadPool(numThreads, numPriorities)
        //   bdlmt::MultipriorityThreadPool(numThreads, numPriorities,
        //                                                          attributes)
        //   ~bdlmt::MultipriorityThreadPool()
        //   numThreads()
        //   numPriorities()
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "===========================================\n"
                    "Testing all constructors and 'numThreads()'\n"
                    "===========================================\n";
        }

        using namespace MULTIPRIORITYTHREADPOOL_CASE_5;

        enum {
            DEBUG_ALLOC,
            DEFAULT_ALLOC
        };

        enum {
            ATTRIBS_NONE,
            ATTRIBS_YES
        };

        static const int numArray[] = { 1, 2, 3, 7, 10, 15, 32 };
            // numbers to use as priorities as well as numThreads
        const int numArrayLen = sizeof numArray / sizeof numArray[0];

        bslma::TestAllocator taDefault;

        bslma::DefaultAllocatorGuard guard(&taDefault);
        ASSERT(&taDefault == bslma::Default::defaultAllocator());
        ASSERT(taDefault.numBytesMax() == 0);

        for (int allocS = DEBUG_ALLOC; DEFAULT_ALLOC >= allocS; ++allocS) {
            for (int aS = ATTRIBS_NONE; ATTRIBS_YES >= aS; ++aS) {
                for (int i = 0; numArrayLen > i; ++i) {
                    for (int j = 0; numArrayLen > j; ++j) {
                        bdlmt::MultipriorityThreadPool *pool = 0;

                        const int numThreads = numArray[i];
                        const int numPri     = numArray[j];

                        if (DEBUG_ALLOC == allocS) {
                            if (ATTRIBS_NONE == aS) {
                                pool = new (ta)
                                    bdlmt::MultipriorityThreadPool(numThreads,
                                                                   numPri,
                                                                   &ta);
                            }
                            else {
                                bslmt::ThreadAttributes attrib;

                                pool = new (ta) bdlmt::MultipriorityThreadPool(
                                                                    numThreads,
                                                                    numPri,
                                                                    attrib,
                                                                    &ta);
                            }
                        }
                        else { // default allocator
                            if (ATTRIBS_NONE == aS) {
                                pool = new (taDefault)
                                    bdlmt::MultipriorityThreadPool(numThreads,
                                                                   numPri);
                            }
                            else {
                                bslmt::ThreadAttributes attrib;

                                pool = new (taDefault)
                                    bdlmt::MultipriorityThreadPool(numThreads,
                                                                   numPri,
                                                                   attrib);
                            }
                        }

                        ASSERT(numThreads == pool->numThreads());
                        ASSERT(numPri     == pool->numPriorities());

                        memset(resultsVec, 0, sizeof(resultsVec));
                        resultsVecIdx = 0;

                        pool->startThreads();

                        for (long i = 0; 10 > i; ++i) {
                            int sts = pool->enqueueJob(&pushInt,
                                                       (void *)(i * i),
                                                        numPri - 1);
                            ASSERT(!sts);
                        }

                        pool->drainJobs();

                        if (1 < numThreads) {
                            sort(resultsVec, resultsVec + resultsVecIdx);
                        }

                        bool ok = true;
                        ok &= (10 == resultsVecIdx);
                        for (int i = 0; 10 > i; ++i) {
                            ok &= (resultsVec[i] == i * i);
                        }

                        LOOP5_ASSERT(allocS, aS, numThreads, numPri,
                                                            resultsVecIdx, ok);
                        if (!ok || veryVeryVeryVerbose) {
                            cout << (ok ? "Vector: " : "Bad vector: ");
                            for (int i = 0; resultsVecIdx > i; ++i) {
                                cout << (i ? ", " : "") << resultsVec[i];
                            }
                            cout << endl;
                        }

                        ASSERT(numThreads == pool->numThreads());
                        ASSERT(numPri     == pool->numPriorities());

                        pool->stopThreads();

                        ASSERT(numThreads == pool->numThreads());
                        ASSERT(numPri     == pool->numPriorities());

                        if (DEBUG_ALLOC == allocS) {
                            ta.deleteObjectRaw(pool);
                        }
                        else {
                            taDefault.deleteObjectRaw(pool);
                        }

                        ASSERT(ta.numBytesInUse() == 0);
                        ASSERT(taDefault.numBytesInUse() == 0);

                        if (veryVeryVerbose) {
                            cout << "Finished loop: allocS: " << allocS <<
                                    ", aS: " << aS <<
                                    ", numThreads: " << numThreads <<
                                    ", numPri: " << numPri << endl;
                        }
                    }  // for j
                }  // for i
            }  // for attachedState
        }  // for allocS
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // DRAINJOBS
        //
        // Concerns:
        //   That 'drainJobs()' properly blocks until a queue is emptied.
        //
        // Plan:
        //   Repeat test case 2, except without the barriers but with quite
        //   slow jobs, call drainJobs() and verify that it blocks until all
        //   the jobs are finished.
        //
        // Testing:
        //   drainJobs()
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "===================\n"
                    "Testing drainJobs()\n"
                    "===================\n";
        }

        using namespace MULTIPRIORITYTHREADPOOL_CASE_3;

        static long incBy[] = { 473, 9384, 273, 132, 182, 191, 282, 934 };
        const int incByLength = sizeof incBy / sizeof incBy[0];

        bslmt::Barrier barrier(2);
        bdlmt::MultipriorityThreadPool pool(1 /* threads */,
                                            1 /* priorities */,
                                            &ta);

        counter = 0;
        long otherCounter = 0;

        for (int i = 0; incByLength > i; ++i) {
            int sts =
                pool.enqueueJob(&sleepAndAmassCounterBy, (void *)incBy[i], 0);
            ASSERT(!sts);
        }

        ASSERT(!counter && !otherCounter);

        for (int i = 0; incByLength > i; ++i) {
            otherCounter += incBy[i];
            otherCounter *= otherCounter;
        }

        pool.startThreads();
        pool.drainJobs();

        LOOP2_ASSERT(counter, otherCounter, counter == otherCounter);
        if (verbose) {
            cout << "Total sum: " << counter << endl;
        }

        pool.stopThreads();
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC OPERATION (BOOTSTRAP, SINGLE THREAD POOL)
        //
        // Concerns:
        //   That a threadpool executes jobs in the order in which they were
        //   received.
        //
        // Plan:
        //   Create a threadpool with one thread, enqueue several jobs, all of
        //   the same priority, then enqueue a barrier job and then wait on
        //   the barrier, verify all jobs before the barrier job got executed
        //   in the order enqueued.
        //
        // Testing:
        //   enqueueJob(void (*)(), void *)
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "===============================================\n"
                    "Testing basic operation, order of job execution\n"
                    "===============================================\n";
        }

        using namespace MULTIPRIORITYTHREADPOOL_CASE_2;

        static long incBy[] = { 473, 9384, 273, 132, 182, 191, 282, 934 };
        const int incByLength = sizeof incBy / sizeof incBy[0];

        bslmt::Barrier barrier(2);
        bdlmt::MultipriorityThreadPool pool(1 /* threads */,
                                            1 /* priorities */,
                                            &ta);

        for (int j = 0; 100 > j; ++j) {
            counter = 0;
            long otherCounter = 0;

            for (int i = 0; incByLength > i; ++i) {
                ASSERT(!pool.enqueueJob(&amassCounterBy, (void *) incBy[i],0));
                ASSERT(!pool.enqueueJob(&waiter, &barrier, 0));
                ASSERT(!pool.enqueueJob(&waiter, &barrier, 0));
            }

            ASSERT(!counter && !otherCounter);

            pool.startThreads();

            for (int i = 0; incByLength > i; ++i) {
                barrier.wait();

                otherCounter += incBy[i];
                otherCounter *= otherCounter;
                LOOP2_ASSERT(counter, otherCounter, counter == otherCounter);
                if (veryVerbose) {
                    cout << "Sum[" << i << "] = " << counter << endl;
                }

                barrier.wait();
            }

            pool.stopThreads();
        }  // for j
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise some basic functionality.
        //
        // Plan:
        //   Create a threadpool, enqueue a job, see it execute.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "==============\n"
                    "Breathing test\n"
                    "==============\n";
        }

        using namespace MULTIPRIORITYTHREADPOOL_CASE_1;

        {
            bslma::TestAllocator taDefault;
            bslma::DefaultAllocatorGuard guard(&taDefault);
            bslma::TestAllocator ta;
            bdlmt::MultipriorityThreadPool *pool =
                new (ta) bdlmt::MultipriorityThreadPool(1 /* threads */,
                                                        1 /* priorities */,
                                                        &ta);

            pool->startThreads();

            counter = 0;

            ASSERT(!pool->enqueueJob(&incCounter, 0, 0));
            ASSERT(!pool->enqueueJob(&incCounter, 0, 0));
            ASSERT(!pool->enqueueJob(&incCounter, 0, 0));
            ASSERT(!pool->enqueueJob(&incCounter, 0, 0));
            ASSERT(!pool->enqueueJob(&incCounter, 0, 0));

            bslmt::ThreadUtil::yield();
            bslmt::ThreadUtil::microSleep(100 * 1000);       // 0.1 seconds

            LOOP_ASSERT(counter, 5 == counter);

            if (verbose) {
                cout << "counter = " << counter << endl;
            }

            pool->drainJobs();
            pool->stopThreads();
            ASSERT(0 == taDefault.numBytesInUse());
            ta.deleteObjectRaw(pool);
        }
      }  break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }  // switch (test)
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
