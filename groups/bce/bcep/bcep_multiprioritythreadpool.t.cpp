// bcep_multiprioritythreadpool.t.cpp       -*-C++-*-
#include <bcep_multiprioritythreadpool.h>

#include <bcec_queue.h>
#include <bcema_testallocator.h>       // for testing only
#include <bcemt_barrier.h>
#include <bcemt_lockguard.h>
#include <bcemt_qlock.h>
#include <bcemt_thread.h>
#include <bces_atomictypes.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bdesu_pathutil.h>
#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_list.h>
#include <bsl_string.h>

#include <bsl_c_ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <bsl_c_string.h>
#include <bsl_c_stdlib.h>            // atoi()
#include <bsl_c_stdio.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] // transitions between non-processing states
// [ 6] // enqueuing fails on disabled queue
// [ 7] // queue sorting
// [10] stress test
// [11] ignoring 'joinable' trait of attributes passed
// [12] usage example 2
// [13] usage example 1
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define PP(X) (cout << #X " = " << (X) << endl, 0) // Print name and
                                                   // value, then return false.

//=============================================================================
//              GLOBAL TYPEDEFS, CONSTANTS, ROUTINES & MACROS FOR TESTING
//-----------------------------------------------------------------------------

typedef bcep_MultipriorityThreadPool Obj;

#define LO_ARRAY_LENGTH(array) (sizeof(array) / sizeof((array)[0]))

namespace {

// Have the default allocator be of different type than the allocator usually
// used -- then we can put breakpoints in bslma_TestAllocator code to find
// unintentional uses of the default allocator.

bslma_NewDeleteAllocator taDefault;
bcema_TestAllocator ta;

int verbose;
int veryVerbose;
int veryVeryVerbose;
int veryVeryVeryVerbose;

} // close unnamed namespace

//=============================================================================
//               Classes for test case 13 -- usage example 1
//=============================================================================

namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_13 {

// The idea here is we have a large number of jobs submitted in too little
// time for all of them to be completed.  All jobs take the same amount of
// time to complete, but there are two different priorities of jobs.  There
// are 100 times more jobs of less urgent priority than of the more urgent
// priority, and there is more than enough time for the jobs of more
// urgent priority to be completed.  Verify that all the jobs of more
// urgent priority get completed while most of the jobs of less urgent
// priority do not.  This demonstrates that we can construct an arrangement
// where traffic of low priority, while massively more numerous, does not
// impede the progress of higher priority jobs.

bces_AtomicInt urgentJobsDone = 0;
bces_AtomicInt lessUrgentJobsDone = 0;

extern "C" void *urgentJob(void *) {
    bcemt_ThreadUtil::microSleep(10000);          // 10 mSec

    ++urgentJobsDone;

    return 0;
}

extern "C" void *lessUrgentJob(void *) {
    bcemt_ThreadUtil::microSleep(10000);          // 10 mSec

    ++lessUrgentJobsDone;

    return 0;
}

}  // close namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_13

//=============================================================================
//               Classes for test case 12 -- usage example 2
//=============================================================================

// The idea here is to have a multithreaded algorithm for calculating prime
// prime numbers.  This is just to serve as an illustration, although it works,
// it is not really any faster than doing it with a single thread.
//
// For every prime number P, we have to mark all multiples of it up in two
// ranges P .. P**2, and P**2 to TOP_NUMBER as non-prime.  For any P**2,
// if we can determine that all primes below P have marked all their multiples
// up to P**2, then we can scan that range and any unmarked values in it will
// be a new prime.  The we can start out with our first prime, 2, and mark
// all primes between it and 2**2 == 4, thus discovering 3 is prime.  Once
// we have marked all multiples of 2 and 3 below 3*3 == 9, we can then scan
// that range and discover 5 and 7 are primes, and repeat the process to
// discover bigger and bigger primes until we have covered an entire range, in
// this example all ints below TOP_NUMBER == 2000.

namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_12 {

enum {
    TOP_NUMBER = 2000,
    NUM_PRIORITIES = 32
};

bool isStillPrime[TOP_NUMBER];
bces_AtomicInt scannedTo[TOP_NUMBER];   // is P is a prime, what is the
                                        // highest multiple of P that
                                        // we have marked
                                        // isStillPrime[P] = false;

bces_AtomicInt maxPrimeFound;       // maximum prime we have identified
                                    // so far
int primeNumbers[TOP_NUMBER];       // elements in the range
                                    // '0 .. numPrimeNumbers - 1' are
                                    // the prime numbers we have found
                                    // so far
bces_AtomicInt numPrimeNumbers;

bcep_MultipriorityThreadPool *threadPool;

bool          doneFlag;                 // set this flag to signal
                                        // other jobs that we're done
bcemt_Barrier doneBarrier(2);           // we wait on this barrier
                                        // to signal the main thread
                                        // that we're done

struct Functor {
    static bcemt_Mutex s_mutex;
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
            return;
        }

        int numToScanI;
        for (numToScanI = numPrimeNumbers - 1; 0 < numToScanI;
                                                        --numToScanI) {
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
                // Not all multiples of all prime numbers below
                // us have been adequately marked as nonPrime.  We
                // cannot yet draw any new conclusions about what
                // is and what is not prime in this range.

                // Resubmit ourselves to back of the priority queue
                // so that we'll get reevaluated when previous prime
                // numbers are done scanning.  Note we could get
                // reenqueued several times.

                // Note that jobs marking the isStillPrime array are
                // at priority 0, while later incarnations that can
                // only set new primes are at priority 1 and keep
                // getting resubmitted at less and less urgent
                // priorities until all their prerequisites (which
                // are at priority 0) are done.

                d_priority = bsl::min(NUM_PRIORITIES - 2,
                                                    d_priority + 1);
                threadPool->enqueueJob(*this, d_priority);

                return;
            }
        }

        // everything up to d_limit that has not been marked
        // nonPrime is prime

        bcemt_LockGuard<bcemt_Mutex> guard(&s_mutex);

        for (int i = maxPrimeFound + 1; d_limit > i; ++i) {
            if (isStillPrime[i]) {
                setNewPrime(i);
            }
        }

        if (TOP_NUMBER == d_limit && !doneFlag) {
            // We have successfully listed all primes below TOP_NUMBER.
            // Touch the done barrier and our caller will then know that
            // we are done and shut down the queue.

            doneFlag = true;
            doneBarrier.wait();
        }
    }

    void operator()() {
        if (0 == d_priority) {
            bces_AtomicInt& rScannedTo = scannedTo[d_numToScan];

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
bcemt_Mutex Functor::s_mutex;

}  // close namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_12

//=============================================================================
//                       Classes for test case 11
//=============================================================================

namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_11 {

struct Functor {
    bcemt_Barrier  *d_barrier;
    bces_AtomicInt *d_jobsCompleted;
    void operator()() {
        d_barrier->wait();

        bcemt_ThreadUtil::microSleep(50 * 1000);       // 0.05 sec
        bcemt_ThreadUtil::yield();
        bcemt_ThreadUtil::microSleep(50 * 1000);       // 0.05 sec

        ++*d_jobsCompleted;
    }
};

}  // close namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_11

//=============================================================================
//                      Classes for test case 10
//=============================================================================

namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_10 {

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

    static bcep_MultipriorityThreadPool *s_pool;
    static bcec_Queue<Worker>           *s_doneQueue;
    static bces_AtomicInt                s_time;

    Worker(int priority) {
        d_priority = priority;
    }

    int submitMe() {
        d_submittedTime = ++s_time;
        if (s_pool->enqueueJob(*this, d_priority)) {
            return -1;
        }

        return 0;
    }

    void operator()() {
        d_doneTime = ++s_time;
        s_doneQueue->pushBack(*this);
    }
};
bcep_MultipriorityThreadPool *Worker::s_pool = 0;
bcec_Queue<Worker>           *Worker::s_doneQueue = 0;
bces_AtomicInt Worker::s_time = 0;

struct ProducerThread {
    int                   d_workersPerProducer;

    static bcemt_Barrier *s_barrier;

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
            static bcemt_Mutex mutex;
            bcemt_LockGuard<bcemt_Mutex> lock(&mutex);

            cout << "Aborted with " << d_workersPerProducer - i <<
                                                    " submissions to go\n";
        }
    }
};
bcemt_Barrier *ProducerThread::s_barrier = 0;

}  // close namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_10

//=============================================================================
//                           Classes for test case 8
//=============================================================================

namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_8 {

bool veryVerboseCase8;

bcemt_Barrier barrier2(2);
bcemt_Barrier barrier8(8);

struct BlockFunctor {
    void operator()() {
        barrier2.wait();
        barrier8.wait();

#if 0
        static bcemt_Mutex *mutex_p;
        BCEMT_ONCE_DO {
            static bcemt_Mutex mutex;
            mutex_p = &mutex;
        }
        bcemt_LockGuard<bcemt_Mutex> lock(mutex_p);
#endif

        static bcemt_QLock mutex = BCEMT_QLOCK_INITIALIZER;
        bcemt_QLockGuard guard(&mutex);

        if (veryVerboseCase8) cout << "Thread finishing\n";
    }
};

}  // close namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_8

//=============================================================================
//                           Classes for test case 5
//                   also partially used in cases 4, 6, and 7
//=============================================================================

namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_5 {

long resultsVec[100];
bces_AtomicInt resultsVecIdx;

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

// Generally check out that a threadpool is healthy, given the state it
// thinks it's in.
void checkOutPool(bcep_MultipriorityThreadPool *pool) {
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
        bcemt_ThreadUtil::microSleep(10 * 1000);
        LOOP_ASSERT(pool->numPendingJobs(), 10 == pool->numPendingJobs());
        pool->removeJobs();
    }

    ASSERT(0 == pool->numPendingJobs());
}

}  // close namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_5

//=============================================================================
//                           Classes for test case 3
//=============================================================================

namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_3 {

long counter;

extern "C" void *sleepAndAmassCounterBy(void *arg)
{
    bcemt_ThreadUtil::microSleep(50 * 1000);    // 50 mSeconds

    counter += (char *) arg - (char *) 0;
    counter *= counter;

    return 0;
}

}  // close namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_3

//=============================================================================
//                           Classes for test case 2
//=============================================================================

namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_2 {

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
    static_cast<bcemt_Barrier *>(arg)->wait();

    return 0;
}

}  // close namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_2

//=============================================================================
//                    Classes for test case 1 - breathing test
//=============================================================================

namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_1 {

bces_AtomicInt counter;

extern "C" void *incCounter(void *)
{
    ++counter;

    return 0;
}

}  // close namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_1

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    bslma_DefaultAllocatorGuard guard(&taDefault);
    ASSERT(&taDefault == bslma_Default::defaultAllocator());

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

        using namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_13;

        bcep_MultipriorityThreadPool pool(20,    // threads
                                          2,     // priorities
                                          &ta);

        bdet_TimeInterval finishTime = bdetu_SystemTime::now() + 0.5;
        pool.startThreads();

        for (int i = 0; 100 > i; ++i) {
            for (int j = 0; 100 > j; ++j) {
                pool.enqueueJob(&lessUrgentJob, (void *) 0, 1); // less urgent
                                                                // priority
            }
            pool.enqueueJob(&urgentJob, (void *) 0, 0); // urgent priority
        }

        bcemt_ThreadUtil::sleep(finishTime - bdetu_SystemTime::now());
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

        using namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_12;

        double startTime = bdetu_SystemTime::now().totalSecondsAsDouble();

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

        threadPool = new (ta) bcep_MultipriorityThreadPool(20,
                                                           NUM_PRIORITIES,
                                                          &ta);
        threadPool->startThreads();

        double startJobs = bdetu_SystemTime::now().totalSecondsAsDouble();

        Functor f(2);
        threadPool->enqueueJob(f, 0);

        doneBarrier.wait();

        double finish = bdetu_SystemTime::now().totalSecondsAsDouble();

        threadPool->shutdown();
        ta.deleteObjectRaw(threadPool);

        if (verbose) {
            double now = bdetu_SystemTime::now().totalSecondsAsDouble();
            printf("Runtime: %g seconds, %g seconds w/o init & cleanup\n",
                                          now - startTime, finish - startJobs);

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

        using namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_11;

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

        bcemt_Barrier barrier(NUM_THREADS + 1);
        bces_AtomicInt jobsCompleted;

        Functor functor;

        functor.d_barrier = &barrier;
        functor.d_jobsCompleted = &jobsCompleted;

        for (int attrState = ATTRIB_JOINABLE; attrState >= ATTRIB_DEFAULT;
                                                                 --attrState) {
            bcema_TestAllocator localTa;

            bcemt_Attribute attrib;

            if (ATTRIB_DETACHED == attrState) {
                attrib.setDetachedState(
                                       bcemt_Attribute::BCEMT_CREATE_DETACHED);
            }
            else if (ATTRIB_JOINABLE == attrState) {
                attrib.setDetachedState(
                                       bcemt_Attribute::BCEMT_CREATE_JOINABLE);
            }

            jobsCompleted = 0;

            bcep_MultipriorityThreadPool pool(NUM_THREADS,
                                              1,            // # priorities
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

        using namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_10;

        bslma_TestAllocator taDefaultLocal;
        bslma_DefaultAllocatorGuard guard(&taDefaultLocal);

        bcec_Queue<Worker> doneQueue(&ta);
        Worker::s_doneQueue = &doneQueue;

        enum {
            NUM_WORKERS_PER_PRODUCER = 1000
        };

        bcep_MultipriorityThreadPool pool(NUM_POOL_THREADS, NUM_PRIORITIES,
                                                                          &ta);
        Worker::s_pool = &pool;

        bcemt_Barrier barrier(NUM_PRODUCER_THREADS + 1);
        ProducerThread::s_barrier = &barrier;

        bcemt_ThreadUtil::Handle handles[NUM_PRODUCER_THREADS];

        for (int i = 0; NUM_PRODUCER_THREADS > i; ++i) {
            ProducerThread producer(NUM_WORKERS_PER_PRODUCER);

            bcemt_ThreadUtil::create(&handles[i], producer);
        }

        pool.startThreads();

        LOOP_ASSERT(taDefaultLocal.numBytesMax(),
                                            0 == taDefaultLocal.numBytesMax());
        ASSERT(0 == Worker::s_time);

        barrier.wait();         // set all producer threads loose
        double startTime = bdetu_SystemTime::now().totalSecondsAsDouble();

        for (int i = 0; NUM_PRODUCER_THREADS > i; ++i) {
            bcemt_ThreadUtil::join(handles[i]);
        }
        pool.drainJobs();
        pool.stopThreads();

        LOOP_ASSERT(taDefaultLocal.numBytesInUse(),
                                          0 == taDefaultLocal.numBytesInUse());

        if (verbose) {
            cout << doneQueue.queue().length() << " mini-jobs processed in " <<
                bdetu_SystemTime::now().totalSecondsAsDouble() - startTime <<
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
            T_(); P(averages[0]); T_(); P(averages[1]);
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

        using namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_5;

        bcep_MultipriorityThreadPool pool(1, 1, &ta);

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

        using namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_8;

        enum {
            NUM_THREADS = 7
        };

        veryVerboseCase8 = veryVerbose;

        bcep_MultipriorityThreadPool pool(NUM_THREADS,
                                          1,    // single priority
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

        using namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_5;

        static const long scramble[] = { 5, 8, 3, 1, 7, 9, 0, 4, 6, 2 };
            // ints 0-9 in scrambled order
        const int scrambleLen = sizeof scramble / sizeof scramble[0];
        const char garbageVal = 0x8f;

        bcep_MultipriorityThreadPool pool(1,            // single thread
                                          scrambleLen,  // priorities
                                          &ta);

        memset(resultsVec, garbageVal, sizeof resultsVec);
        resultsVecIdx = 0;

        for (int i = 0; scrambleLen > i; ++i) {
            pool.enqueueJob(&pushInt, (void *) (scramble[i] * scramble[i]),
                                                            (int) scramble[i]);
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

        using namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_5;

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
            bcep_MultipriorityThreadPool pool(NUM_THREADS, NUM_PRIORITIES,&ta);

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
                        P_(L_) P_(ii) P(j);
                    }
                    break;
                }

                bcemt_ThreadUtil::yield();
                bcemt_ThreadUtil::microSleep(10 * 1000);

                ASSERT(pool.isStarted());
                ASSERT(pool.isSuspended());
                ASSERT(NUM_THREADS == pool.numStartedThreads());
                ASSERT(0 == resultsVecIdx);
                ASSERT(NUM_JOBS == pool.numPendingJobs());

                pool.stopThreads();

                bcemt_ThreadUtil::yield();
                bcemt_ThreadUtil::microSleep(10 * 1000);

                ASSERT(!pool.isStarted());
                ASSERT(pool.isSuspended());
                ASSERT(0 == pool.numStartedThreads());
                ASSERT(0 == resultsVecIdx);
                ASSERT(NUM_JOBS == pool.numPendingJobs());

                pool.resumeProcessing();

                bcemt_ThreadUtil::yield();
                bcemt_ThreadUtil::microSleep(10 * 1000);

                ASSERT(!pool.isStarted());
                ASSERT(!pool.isSuspended());
                ASSERT(0 == pool.numStartedThreads());
                ASSERT(0 == resultsVecIdx);
                ASSERT(NUM_JOBS == pool.numPendingJobs());

                pool.suspendProcessing();

                bcemt_ThreadUtil::yield();
                bcemt_ThreadUtil::microSleep(10 * 1000);

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
                        P_(L_) P_(ii) P(j);
                    }
                    break;
                }
                ASSERT(0 == pool.startThreads());

                bcemt_ThreadUtil::yield();
                bcemt_ThreadUtil::microSleep(10 * 1000);

                ASSERT(pool.isStarted());
                ASSERT(pool.isSuspended());
                ASSERT(NUM_THREADS == pool.numStartedThreads());
                ASSERT(0 == resultsVecIdx);
                ASSERT(NUM_JOBS == pool.numPendingJobs());

                pool.stopThreads();
                pool.stopThreads();

                bcemt_ThreadUtil::yield();
                bcemt_ThreadUtil::microSleep(10 * 1000);

                ASSERT(!pool.isStarted());
                ASSERT(pool.isSuspended());
                ASSERT(0 == pool.numStartedThreads());
                ASSERT(0 == resultsVecIdx);
                ASSERT(NUM_JOBS == pool.numPendingJobs());

                pool.resumeProcessing();
                pool.resumeProcessing();

                bcemt_ThreadUtil::yield();
                bcemt_ThreadUtil::microSleep(10 * 1000);

                ASSERT(!pool.isStarted());
                ASSERT(!pool.isSuspended());
                ASSERT(0 == pool.numStartedThreads());
                ASSERT(0 == resultsVecIdx);
                ASSERT(10 == pool.numPendingJobs());

                pool.suspendProcessing();
                pool.suspendProcessing();

                bcemt_ThreadUtil::yield();
                bcemt_ThreadUtil::microSleep(10 * 1000);

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
                    P_(L_) P(ii);
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
        if (verbose) { P_(L_) P(ii); }
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

        using namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_5;

        bcep_MultipriorityThreadPool pool(16,   // num threads
                                          4,    // num priorities
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
        //   bcep_MultipriorityThreadPool(numThreads, numPriorities, alloc)
        //   bcep_MultipriorityThreadPool(numThreads, numPriorities,
        //                                                   attributes, alloc)
        //   bcep_MultipriorityThreadPool(numThreads, numPriorities)
        //   bcep_MultipriorityThreadPool(numThreads, numPriorities,
        //                                                          attributes)
        //   ~bcep_MultipriorityThreadPool()
        //   numThreads()
        //   numPriorities()
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "===========================================\n"
                    "Testing all constructors and 'numThreads()'\n"
                    "===========================================\n";
        }

        using namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_5;

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

        bcema_TestAllocator taDefault;

        bslma_DefaultAllocatorGuard guard(&taDefault);
        ASSERT(&taDefault == bslma_Default::defaultAllocator());
        ASSERT(taDefault.numBytesMax() == 0);

        for (int allocS = DEBUG_ALLOC; DEFAULT_ALLOC >= allocS; ++allocS) {
            for (int aS = ATTRIBS_NONE; ATTRIBS_YES >= aS; ++aS) {
                for (int i = 0; numArrayLen > i; ++i) {
                    for (int j = 0; numArrayLen > j; ++j) {
                        bcep_MultipriorityThreadPool *pool = 0;

                        const int numThreads = numArray[i];
                        const int numPri     = numArray[j];

                        if (DEBUG_ALLOC == allocS) {
                            if (ATTRIBS_NONE == aS) {
                                pool = new (ta)
                                                bcep_MultipriorityThreadPool(
                                                      numThreads, numPri, &ta);
                            }
                            else {
                                bcemt_Attribute attrib;

                                pool = new (ta)
                                                bcep_MultipriorityThreadPool(
                                              numThreads, numPri, attrib, &ta);
                            }
                        }
                        else { // default allocator
                            if (ATTRIBS_NONE == aS) {
                                pool = new (taDefault)
                                                bcep_MultipriorityThreadPool(
                                                   numThreads, numPri);
                            }
                            else {
                                bcemt_Attribute attrib;

                                pool = new (taDefault)
                                                bcep_MultipriorityThreadPool(
                                           numThreads, numPri, attrib);
                            }
                        }

                        ASSERT(numThreads == pool->numThreads());
                        ASSERT(numPri     == pool->numPriorities());

                        memset(resultsVec, 0, sizeof(resultsVec));
                        resultsVecIdx = 0;

                        pool->startThreads();

                        for (long i = 0; 10 > i; ++i) {
                            int sts = pool->enqueueJob(&pushInt,
                                                 (void *) (i * i), numPri - 1);
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

        using namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_3;

        static long incBy[] = { 473, 9384, 273, 132, 182, 191, 282, 934 };
        const int incByLength = sizeof incBy / sizeof incBy[0];

        bcemt_Barrier barrier(2);
        bcep_MultipriorityThreadPool pool(1 /* threads */, 1 /* priorities */,
                                                                          &ta);

        counter = 0;
        long otherCounter = 0;

        for (int i = 0; incByLength > i; ++i) {
            int sts = pool.enqueueJob(&sleepAndAmassCounterBy,
                                                        (void *) incBy[i], 0);
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

        using namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_2;

        static long incBy[] = { 473, 9384, 273, 132, 182, 191, 282, 934 };
        const int incByLength = sizeof incBy / sizeof incBy[0];

        bcemt_Barrier barrier(2);
        bcep_MultipriorityThreadPool pool(1 /* threads */, 1 /* priorities */,
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

        using namespace BCEP_MULTIPRIORITYTHREADPOOL_CASE_1;

        {
            bcema_TestAllocator taDefault;
            bslma_DefaultAllocatorGuard guard(&taDefault);
            bcema_TestAllocator ta;
            bcep_MultipriorityThreadPool *pool = new (ta)
            bcep_MultipriorityThreadPool(1 /* threads */,
                                         1 /* priorities */,
                                         &ta);

            pool->startThreads();

            counter = 0;

            ASSERT(!pool->enqueueJob(&incCounter, 0, 0));
            ASSERT(!pool->enqueueJob(&incCounter, 0, 0));
            ASSERT(!pool->enqueueJob(&incCounter, 0, 0));
            ASSERT(!pool->enqueueJob(&incCounter, 0, 0));
            ASSERT(!pool->enqueueJob(&incCounter, 0, 0));

            bcemt_ThreadUtil::yield();
            bcemt_ThreadUtil::microSleep(100 * 1000);       // 0.1 seconds

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
