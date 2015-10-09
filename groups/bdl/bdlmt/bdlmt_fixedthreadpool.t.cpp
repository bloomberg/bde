// bdlmt_fixedthreadpool.t.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlmt_fixedthreadpool.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bdlt_currenttime.h>
#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>

#include <bslma_defaultallocatorguard.h>

#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>
#include <bsl_cstdio.h>             // For FILE in usage example
#include <bsl_cstdlib.h>            // for atoi
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_c_signal.h>

// for collecting CPU time
#ifdef BSLS_PLATFORM_OS_WINDOWS
#        include <windows.h>
#else
#        include <sys/resource.h>
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              OVERVIEW
// A fixed thread pool maintains a fixed-capacity queue of jobs and dispatches
// them onto a fixed number of threads.  We need to test that the pool can be
// started, stopped, and drained properly, that jobs are enqueued properly,
// blocking when the queue reaches full capacity or returning if only trying to
// enqueue, and that the pool is appropriately busy or idle if no jobs are
// enqueued.  Our testing apparatus consist mostly of jobs whose behavior can
// be controlled from the calling thread via an argument class shared by all
// the threads; for instance, a certain job can be enqueued and waits for a
// condition variable to be signaled, another one waits for a barrier, all of
// which are controlled by the test case.
//
// In addition to positive test cases (run in the nightly builds), a negative
// test case -1 can be run manually to measure performance of enqueuing jobs.
//
// [ 3] bdlmt::FixedThreadPool(const bslmt::Attributes&, int, int, int);
// [ 3] ~bdlmt::FixedThreadPool();
// [ 3] int enqueueJob(const bsl::function<void()>& );
// [ 3] int numThreads() const;
// [ 4] int enqueueJob(FixedThreadPoolJobFunc, void *);
// [ 4] void start();
// [ 4] void stop();
// [ 4] void drain();
// [ 4] void shutdown();
// [ 4] int queueCapacity() const;
// [ 4] int numThreadsStarted() const;
// [ 5] int tryenqueueJob(FixedThreadPoolJobFunc, void *);
// ----------------------------------------------------------------------------
// [ 2] TESTING HELPER FUNCTIONS
// [ 2] Breathing test
// [ 6] TESTING a job enqueuing other jobs
// [ 8] TESTING SYNCHRONOUS SIGNALS
// [ 9] TESTING CPU consumption of an idle pool.
// [11] Usage examples
// [12] Usage examples

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

typedef bdlmt::FixedThreadPool Obj;

// ============================================================================
//                             DEFAULT ALLOCATOR
// ----------------------------------------------------------------------------

bslma::TestAllocator taDefault;

// ============================================================================
//                 HELPER CLASSES AND FUNCTIONS  FOR TESTING
// ----------------------------------------------------------------------------

#define STARTPOOL(x) \
    if (0 != x.start()) { \
        cout << "Thread start() failed.  Thread quota exceeded?" \
             << bsl::endl; \
        ASSERT(false); \
        break; } // things are SNAFU

struct TestJobFunctionArgs {
    bslmt::Condition *d_startCond;
    bslmt::Condition *d_stopCond;
    bslmt::Mutex     *d_mutex;
    volatile int     d_count;
    volatile int     d_startSig;
    volatile int     d_stopSig;
};

struct TestJobFunctionArgs1 {
    bslmt::Barrier   *d_startBarrier_p;
    bslmt::Barrier   *d_stopBarrier_p;
    bsls::AtomicInt   d_count;
};

extern "C" {

#if defined(BSLS_PLATFORM_OS_UNIX)
void TestSynchronousSignals(void *ptr)
{
    sigset_t blockedSet;
    sigemptyset(&blockedSet);
    pthread_sigmask(SIG_BLOCK, NULL, &blockedSet);

    // copy this array from .cpp file
    static const int synchronousSignals[] = {
      SIGBUS,
      SIGFPE,
      SIGILL,
      SIGSEGV,
      SIGSYS,
      SIGABRT,
      SIGTRAP,
#ifdef SIGIOT
      SIGIOT
#endif
    };

    int SIZE = sizeof synchronousSignals / sizeof *synchronousSignals;

    for (int i=0; i < SIZE; ++i) {
        ASSERT(sigismember(&blockedSet, synchronousSignals[i]) == 0);
    }

    // Verify that thread pool does not, by mistake, leave ALL the signals
    // unblocked.  Testing for 1 signal should be enough.
#ifndef BSLS_PLATFORM_OS_CYGWIN
    ASSERT(sigismember(&blockedSet, SIGINT) == 1);
#endif

}
#endif

void testJobFunction1(void *ptr)
    // This function is used to simulate a thread pool job.  It accepts a
    // pointer to a pointer to a structure containing a mutex and a conditional
    // variable.  The function simply blocks until the conditional variable is
    // signaled.
{
    TestJobFunctionArgs *args = (TestJobFunctionArgs*)ptr;
    bslmt::LockGuard<bslmt::Mutex> lock(args->d_mutex);
    ++args->d_count;
    ++args->d_startSig;
    args->d_startCond->signal();
    while( !args->d_stopSig ) {
        args->d_stopCond->wait(args->d_mutex);
    }
}

static void testJobFunction2( void *ptr )
    // This function is used to simulate a thread pool job.  It accepts a
    // pointer to a pointer to a structure containing a mutex and a conditional
    // variable.  The function simply signals that it has started, increments
    // the supplied counter and returns.
{
    TestJobFunctionArgs *args = (TestJobFunctionArgs*)ptr;
    bslmt::LockGuard<bslmt::Mutex> lock(args->d_mutex);
    ++args->d_count;
    ++args->d_startSig;
    args->d_startCond->signal();
}

void testJobFunction3(void *ptr)
    // This function is used to simulate a thread pool job.  It accepts a
    // pointer to a pointer to a structure containing a mutex and a conditional
    // variable.  All the threads calling this function will wait until the
    // main thread invokes the 'wait' on a barrier signifying the start of the
    // function.  After that all the threads calling this function will wait
    // until the main thread invokes the 'wait' on a barrier signifying the end
    // of the function, if such a barrier is specified.
{
    TestJobFunctionArgs1 *args = (TestJobFunctionArgs1*)ptr;

    if (args->d_startBarrier_p) {
        args->d_startBarrier_p->wait();
    }
    ++args->d_count;
    if (args->d_stopBarrier_p) {
        args->d_stopBarrier_p->wait();
    }
}

}

int testJobFunction4(const int N)
    // This function is used to simulate a thread pool job.  It accepts an int
    // which controls the time taken by that job.  We let the job compute some
    // quantity (the actually job does not matter, only the time it takes).
    // Here we compute the golden ratio.
{
    double result = 1.0;
    for (int i = 0; i < N; ++i) {
        result = 1 / (1 + result);
    }
    return result;
}

static double getCurrentCpuTime()
    // Return the total CPU time (user and system) consumed by the current
    // process since creation; the CPU time unit is seconds.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    HANDLE me = GetCurrentProcess();
    FILETIME ct, et, kt, ut; // ct & et are unused.
    ULARGE_INTEGER stime, utime;
    ASSERT(GetProcessTimes(me, &ct, &et, &kt, &ut));
    memcpy(&stime, &kt, sizeof(stime)); // avoids memory alignment issues.
    memcpy(&utime, &ut, sizeof(utime));
    return (double)((long double)((__int64)utime.QuadPart +
                                  (__int64)stime.QuadPart)
                                 /1000000000.0L);
#else
    struct rusage ru;

    ASSERT(getrusage(RUSAGE_SELF, &ru) >= 0);
    return (double)ru.ru_utime.tv_sec +
               (double)ru.ru_utime.tv_usec/1000000.0 +
           (double)ru.ru_stime.tv_sec +
           (double)ru.ru_stime.tv_usec/1000000.0;
#endif
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------
#define SEARCH_THREADS     10
#define SEARCH_QUEUE_CAPACITY  50

struct myFastSearchJobInfo {
    const string        *d_word;    // word to search for
    const string        *d_path;    // path of the file to search
    bslmt::Mutex         *d_mutex;   // mutex to control access the result
                                    // file list.
    bsl::vector<string> *d_outList; // list of matching files
};

extern "C" {

static void myFastSearchJob(void *arg)
{
    FILE *file;
    myFastSearchJobInfo *job =  (myFastSearchJobInfo*)arg;

    file = fopen(job->d_path->c_str(), "r");

    if (file) {
     char  buffer[1024];
     size_t nread;
     int   wordLen = job->d_word->length();
     const char *word = job->d_word->c_str();

     nread = fread( buffer, 1, sizeof(buffer)-1, file );
     while( nread >= (size_t)wordLen ) {
         buffer[nread] = 0;
         if (strstr(buffer, word)) {
             bslmt::LockGuard<bslmt::Mutex> lock(job->d_mutex);
             job->d_outList->push_back(*job->d_path);
             break;

         }
         memcpy( buffer, &buffer[nread-wordLen-1], wordLen-1 );
         nread = fread(buffer+wordLen-1, 1, sizeof(buffer)-wordLen, file);
     }
     fclose(file);
    }
}

}

static void myFastSearch(const string&         word,
                         const vector<string>& fileList,
                         vector<string>&       outFileList)
{
    bslmt::Mutex            mutex;
    bslmt::ThreadAttributes defaultAttributes;
    bdlmt::FixedThreadPool  pool(defaultAttributes,
                                 SEARCH_THREADS,
                                 SEARCH_QUEUE_CAPACITY);

    if (0 != pool.start()) {
        cout << "Thread start() failed.  Thread quota exceeded?" << endl;
        ASSERT(false);
        return; // things are SNAFU                                   // RETURN
    }

    int count = fileList.size();
    myFastSearchJobInfo  *jobInfoArray = new myFastSearchJobInfo[count];

    for (int i=0; i<count; ++i) {
        myFastSearchJobInfo &job = jobInfoArray[i];
        job.d_word    = &word;
        job.d_path    = &fileList[i];
        job.d_mutex   = &mutex;
        job.d_outList = &outFileList;
        pool.enqueueJob(myFastSearchJob, &job);
    }
    pool.drain();
    delete[] jobInfoArray;
}
//
// Usage Example 2
//
static void myFastFunctorSearchJob(myFastSearchJobInfo *job)
{
    FILE *file;

    file = fopen(job->d_path->c_str(), "r");

    if (file) {
        char  buffer[1024];
        size_t nread;
        int   wordLen = job->d_word->length();
        const char *word = job->d_word->c_str();

        nread = fread( buffer, 1, sizeof(buffer)-1, file );
        while( nread >= (size_t)wordLen ) {
            buffer[nread] = 0;
            if (strstr(buffer, word)) {
                bslmt::LockGuard<bslmt::Mutex> lock(job->d_mutex);
                job->d_outList->push_back(*job->d_path);
                break;

            }
            memcpy( buffer, &buffer[nread-wordLen-1], wordLen-1 );
            nread = fread(buffer+wordLen-1, 1, sizeof(buffer)-wordLen, file);
        }
        fclose(file);
    }
}

static void myFastFunctorSearch(const string&         word,
                                const vector<string>& fileList,
                                vector<string>&       outFileList)
{
    bslmt::Mutex            mutex;
    bslmt::ThreadAttributes defaultAttributes;
    bdlmt::FixedThreadPool  pool(defaultAttributes,
                                 SEARCH_THREADS,
                                 SEARCH_QUEUE_CAPACITY);

    if (0 != pool.start()) {
        cout << "Thread start() failed.  Thread quota exceeded?"
             << bsl::endl;
        ASSERT(false);
        return; // things are SNAFU                                   // RETURN
    }

    int count = fileList.size();
    myFastSearchJobInfo  *jobInfoArray = new myFastSearchJobInfo[count];

    for (int i=0; i<count; ++i) {
        myFastSearchJobInfo &job = jobInfoArray[i];
        job.d_word    = &word;
        job.d_path    = &fileList[i];
        job.d_mutex   = &mutex;
        job.d_outList = &outFileList;

        bsl::function<void()> jobHandle =
                           bdlf::BindUtil::bind(&myFastFunctorSearchJob, &job);
        pool.enqueueJob(jobHandle);
    }
    pool.drain();
    delete[] jobInfoArray;
}

// ============================================================================
//                         CASE 14 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace FIXEDTHREADPOOL_CASE_14 {

class ConcurrencyTest {
    // Invoke a set of operations operations synchronously.

    // DATA
    bdlmt::FixedThreadPool   d_pool;
    bslmt::Barrier          d_barrier;
    bslma::Allocator      *d_allocator_p;

    // PRIVATE MANIPULATORS
    void execute();
        // Execute a single test.

    ConcurrencyTest(const ConcurrencyTest&);             // unimplemented
    ConcurrencyTest& operator=(const ConcurrencyTest&);  // unimplemented

  public:
    // CREATORS
    ConcurrencyTest(int               numThreads,
                    bslma::Allocator *basicAllocator)
    : d_pool(numThreads, 1000, basicAllocator)
    , d_barrier(numThreads)
    , d_allocator_p(basicAllocator)
    {
        d_pool.start();
    }

    ~ConcurrencyTest() {}

    //  MANIPULATORS
    void runTest();
        // Run the test.
};

void ConcurrencyTest::execute()
{
    d_barrier.wait();
}

void ConcurrencyTest::runTest()
{
    bsl::function<void()> job =
                         bdlf::BindUtil::bind(&ConcurrencyTest::execute, this);
    for (int i = 0; i < d_pool.numThreads(); ++i) {
        d_pool.enqueueJob(job);
    }
    d_pool.drain();
}

}  // close namespace FIXEDTHREADPOOL_CASE_14

// ============================================================================
//                         CASE 11 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace FIXEDTHREADPOOL_CASE_11 {

enum {
    NUM_THREADPOOLS = 8,
    THREADPOOL_QUEUE_SIZE = 20
};

bdlmt::FixedThreadPool *threadPools[NUM_THREADPOOLS];
bsls::AtomicInt64 numFertileRabbits(0);

struct EnqueueRabbit {
    bool d_fertile;
    int d_poolIdx;

    EnqueueRabbit(bool fertile, int poolIdx) {
        d_fertile = fertile;
        d_poolIdx = poolIdx;
    }

    void operator()() {
        if (d_fertile) {
            int poolIdx = d_poolIdx ^ 1;
            EnqueueRabbit inFertile(0, poolIdx);

            ++numFertileRabbits;

            for (int i = 4; i > 0; --i) {
                threadPools[poolIdx]->enqueueJob(inFertile);
            }
            threadPools[poolIdx]->enqueueJob(EnqueueRabbit(1, poolIdx));
        }
    }
};

struct TryEnqueueRabbit {
    bool d_fertile;
    int d_poolIdx;

    TryEnqueueRabbit(bool fertile, int poolIdx) {
        d_fertile = fertile;
        d_poolIdx = poolIdx;
    }

    void operator()() {
        if (d_fertile) {
            int poolIdx = d_poolIdx ^ 1;
            TryEnqueueRabbit inFertile(0, poolIdx);

            ++numFertileRabbits;

            for (int i = 4; i > 0; --i) {
                threadPools[poolIdx]->tryEnqueueJob(inFertile);
            }
            threadPools[poolIdx]->tryEnqueueJob(TryEnqueueRabbit(1, poolIdx));
        }
    }
};

}  // close namespace FIXEDTHREADPOOL_CASE_11

// ============================================================================
//                         CASE 10 RELATED ENTITIES
// ----------------------------------------------------------------------------

void testJobFunction9_2(bslmt::Barrier *barrier)
{
    barrier->wait();
}

class Test9Object {
    Obj *d_threadPool_p;
    bslmt::Barrier *d_barrier_p;

  private:
    Test9Object(const Test9Object&);
    Test9Object& operator=(const Test9Object&);

  public:
    Test9Object(bslmt::Barrier *barrier_p, Obj *threadPool_p);
    ~Test9Object();
};

Test9Object::Test9Object(bslmt::Barrier *barrier_p, Obj *threadPool_p)
: d_threadPool_p(threadPool_p)
, d_barrier_p(barrier_p)
{
}

Test9Object::~Test9Object()
{
    bsl::function<void()> job =
                        bdlf::BindUtil::bind(&testJobFunction9_2, d_barrier_p);
    ASSERT(0 == d_threadPool_p->enqueueJob(job));
}

extern "C" {
    void testJobFunction9(void *ptr)
    {
        Test9Object *p = (Test9Object*)ptr;
        delete p;
    }
}

// ============================================================================
//                          CASE 9 RELATED ENTITIES
// ----------------------------------------------------------------------------

static void counter(int *result, int num) {
    while(num > 0) {
        *result += 1;
        --num;
    }
}

// ============================================================================
//                          CASE 6 RELATED ENTITIES
// ----------------------------------------------------------------------------
enum {
    DEPTH_LIMIT = 10,
    THREADS     = 10,
    QUEUE_CAPACITY  = 100 // must be greater than or equal to DEPTH_LIMIT
};

Obj *xP;
bslmt::Barrier barrier(DEPTH_LIMIT + 1);  // to ensure that all the threads
                                          // have started

bsls::AtomicInt depthCounter;

extern "C" {

void testJobFunction5(void *ptr)
    // This function is used to simulate a thread pool job.  It enqueues itself
    // in the pool if the depth limit is not reached.
{
    ASSERT(depthCounter >= 0);
    ASSERT(depthCounter <= DEPTH_LIMIT);
    if (++depthCounter != DEPTH_LIMIT)
        xP->enqueueJob(&testJobFunction5, NULL);

    barrier.wait();
}

}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    // TBD: create additional tests as follows
    /*
       -------------------------
       The original implementation of 'shutdown' and 'stop' had a race
       condition where an inconsistent set of items is removed/processed.
       Reproduce this race condition in the old implementations and demonstrate
       that it has been fix by the new implementation.
       Test plan:
       Create a large enough set of functors that must be executed in
       in a contiguous block without skipping any.  Start pushing these
       functors into the thread pool.  At the same time 'shutdown'/'stop' the
       pool.  Verify that no functors have been skipped.
       -------------------------
    */
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2 ? (atoi(argv[2]) ? atoi(argv[2]) : 1) : 0;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bslma::DefaultAllocatorGuard guard(&taDefault);
    bslma::TestAllocator  testAllocator(veryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // case 0 is always the first case
      case 14: {
        // --------------------------------------------------------------------
        // TEST CASE FOR WINDOWS TEST FAILURE
        //   This test case was created for a windows failure that occurred on
        //   8/17.  This essentially recreates the use of
        //   'bdlmt_fixedthreadpool' in the 'baem' test drivers.
        //
        // Plan:
        if (verbose) cout << "Check windows test failure\n"
                          << "==========================" << endl;

        using namespace FIXEDTHREADPOOL_CASE_14;

        bslma::TestAllocator defaultAllocator;
        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        bslma::TestAllocator testAllocator;
        for (int numThreads = 1; numThreads < 12; ++numThreads) {
            ConcurrencyTest tester(numThreads, &defaultAllocator);
            tester.runTest();
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING RACE CONDITION BETWEEN SHUTDOWN AND ENQUEUE
        //   There was a race condition between shutdown and enqueue that
        //   resulted in a residue of jobs being left in the queue after
        //   shutdown.
        //
        // Plan:
        //   If the race condition exists, reproduce it.  Otherwise, verify
        //   that it has been eliminated.
        //
        //   Start a bunch of single-threaded thread pools, several pairs of
        //   them, actually.  Submit 'rabbit' jobs, that submit jobs like
        //   themselves to the adjacent thread pools.  Once the rabbits are
        //   reproducing amongst the thread pools, shut them all down, then
        //   test the thread pools to see if any residual jobs were left in
        //   any of the thread pools.  Repeat the process.

        if (verbose) cout << "Check for race condition in shutdown().\n"
                          << "=======================================" << endl;

        using namespace FIXEDTHREADPOOL_CASE_11;

        memset(threadPools, 0, sizeof(threadPools));

        for (int i = 0; i < NUM_THREADPOOLS; ++i) {
            threadPools[i] = new bdlmt::FixedThreadPool(1,
                                                        THREADPOOL_QUEUE_SIZE);
        }

#       ifdef BSLS_PLATFORM_OS_LINUX
            enum {
                NUM_ITERATIONS = 10,
                NUM_ITERATIONS_PER_LINE = 10,
                NUM_ITERATIONS_PER_DOT = 1
            };
#       else
#           ifdef BSLS_PLATFORM_OS_AIX
                enum {
                    NUM_ITERATIONS = 800,
                    NUM_ITERATIONS_PER_LINE = 200,
                    NUM_ITERATIONS_PER_DOT = 4
                };
#           else
                enum {
                    NUM_ITERATIONS = 400,
                    NUM_ITERATIONS_PER_LINE = 100,
                    NUM_ITERATIONS_PER_DOT = 2
                };
#           endif
#       endif

        if (verbose) cout << "Enqueue: " << NUM_ITERATIONS <<
                                                    " iterations." << flush;
        for (int jj = 0; jj < NUM_ITERATIONS; ++jj) {
            if (verbose && 0 == jj % NUM_ITERATIONS_PER_LINE)
                                             cout << "\n  " << flush;

            for (int i = 0; i < NUM_THREADPOOLS; ++i) {
                threadPools[i]->start();
            }

            bsls::Types::Int64 pauseUntil =
                                           numFertileRabbits + NUM_THREADPOOLS;

            for (int i = 0; i < NUM_THREADPOOLS; i += 2) {
                int sts = threadPools[i]->enqueueJob(EnqueueRabbit(true, i));
                ASSERT(0 == sts);
            }

            do {
                bslmt::ThreadUtil::yield();
            } while (numFertileRabbits < pauseUntil);

            for (int i = 0; i < NUM_THREADPOOLS; ++i) {
                threadPools[i]->shutdown();
                ASSERT(0 == threadPools[i]->numPendingJobs());
            }
            for (int i = 0; i < NUM_THREADPOOLS; ++i) {
                if (0 != threadPools[i]->numPendingJobs()) {
                    delete threadPools[i];
                    threadPools[i] = new bdlmt::FixedThreadPool(
                                                        1,
                                                        THREADPOOL_QUEUE_SIZE);
                }
            }

            if (verbose && 0 == (jj + 1) % NUM_ITERATIONS_PER_DOT)
                                                          cout << "." << flush;
        }

        if (verbose) cout << "\nTryEnqueue: " << NUM_ITERATIONS <<
                                                    " iterations." << flush;
        for (int jj = 0; jj < NUM_ITERATIONS; ++jj) {
            if (verbose && 0 == jj % NUM_ITERATIONS_PER_LINE)
                                             cout << "\n  " << flush;

            for (int i = 0; i < NUM_THREADPOOLS; ++i) {
                threadPools[i]->start();
            }

            bsls::Types::Int64 pauseUntil =
                                           numFertileRabbits + NUM_THREADPOOLS;

            for (int i = 0; i < NUM_THREADPOOLS; i += 2) {
                int sts = threadPools[i]->tryEnqueueJob(TryEnqueueRabbit(true,
                                                                         i));
                ASSERT(0 == sts);
            }

            do {
                bslmt::ThreadUtil::yield();
            } while (numFertileRabbits < pauseUntil);

            for (int i = 0; i < NUM_THREADPOOLS; ++i) {
                threadPools[i]->shutdown();
                ASSERT(0 == threadPools[i]->numPendingJobs());
            }
            for (int i = 0; i < NUM_THREADPOOLS; ++i) {
                if (0 != threadPools[i]->numPendingJobs()) {
                    delete threadPools[i];
                    threadPools[i] = new bdlmt::FixedThreadPool(
                                                        1,
                                                        THREADPOOL_QUEUE_SIZE);
                }
            }

            if (verbose && 0 == (jj + 1) % NUM_ITERATIONS_PER_DOT)
                                                          cout << "." << flush;
        }
        if (verbose) {
            cout << "\nFertile Rabbits: " << numFertileRabbits << endl;
        }

        for (int i = 0; i < NUM_THREADPOOLS; ++i) {
            delete threadPools[i];
            threadPools[i] = 0;
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE with Functor Interface
        //   This is a duplicate of test case 11, except that we use the
        //   functor version of enqueueJob() directly.  This is discussed
        //   in the Usage documentation.
        // Plan:
        //   Call the main routine with an empty list of files to process.
        //   Assert that the call succeeds and that the resulting file list
        //   is empty.
        //
        // Testing:
        //   USAGE EXAMPLE (Functor Interface)
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing: Usage Example" << endl
                          << "======================" << endl ;
        {
            vector<string> fileList;
            vector<string> outFileList;

            myFastFunctorSearch("bcep", fileList, outFileList);

            ASSERT(0 == outFileList.size());
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   Because of the nature of the example in the header file, we cannot
        //   verify the output, so we simply very that it compiles and that the
        //   main routine can be called.
        // Plan:
        //   Call the main routine with an empty list of files to process.
        //   Assert that the call succeeds and that the resulting file list
        //   is empty.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing: Usage Example\n"
                          << "====================================" << endl ;
        {
            vector<string> fileList;
            vector<string> outFileList;

            myFastSearch("hello", fileList, outFileList);
            ASSERT(0 == outFileList.size());
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // VERIFY that functors are destroyed when the thread pool is not
        // holding the lock.
        //
        // Concerns:
        //  That a functor can have bound state which might call back the
        //          the threadpool or acquire other locks.
        //
        // Plan:
        //   Create a functor which has an object bound which calls back
        //   the threadpool in its destructor.
        // --------------------------------------------------------------------

        if (verbose) cout << "Verifying that functors are destroyed.\n"
                          << "======================================" << endl ;

        enum { NTHREADS = 1,
               NQUEUE_CAPACITY = 100 };

        bslmt::ThreadAttributes attr;
        Obj mX(attr, NTHREADS, NQUEUE_CAPACITY, &testAllocator);

        STARTPOOL(mX);

        bslmt::Barrier barrier(2);
        {
            Test9Object* p = new Test9Object(&barrier, &mX);
            mX.enqueueJob(testJobFunction9, p);
        }

        barrier.wait();

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING CPU time consumption of an idle pool.
        //   Verify that a threadpool consumes negligible CPU time when
        //   not being used.
        //
        // Concerns: that an idle thread pool spins the CPU.
        //
        // Plan:
        //   First measure CPU usage for 10 seconds, doing nothing.
        //   Instantiate a threadpool with a given number of threads.
        //   Capture the current cumulative CPU time consumed.
        //   Wait 10 seconds.
        //   Capture the new cumulative CPU time consumed.
        //   Require the difference between the two CPU times to be negligible.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing: cpu time not consumed when idle \n"
                          << "====================================" << endl ;
        {
            bslmt::ThreadAttributes attr;

            enum {
                THREADS_THREADS = 25,
                QUEUE_CAPACITY_THREADS = THREADS_THREADS,
                SLEEP_TIME  = 10            // in seconds
            };

            double startIdleCpuTime = getCurrentCpuTime();
            if (veryVerbose) {
                T_ P(startIdleCpuTime)
            }
            bslmt::ThreadUtil::microSleep(0, SLEEP_TIME);
            double consumedIdleCpuTime = getCurrentCpuTime()
                                       - startIdleCpuTime;

            startIdleCpuTime = getCurrentCpuTime();
            if (veryVerbose) {
                T_ P_(consumedIdleCpuTime)
            }
            double additiveFudge = 0.01 // to allow for imprecisions in timing
                                 + getCurrentCpuTime() - startIdleCpuTime;
                                        // to allow for I/O operations!!!
            double fudgeFactor   = 10;  // to allow for imprecisions in timing
            double maxConsumedCpuTime = fudgeFactor *
                                (additiveFudge +
                                 consumedIdleCpuTime * QUEUE_CAPACITY_THREADS);
            if (veryVerbose) {
                P(maxConsumedCpuTime)
            }

            Obj x(attr,
                  THREADS_THREADS,
                  QUEUE_CAPACITY_THREADS,
                  &testAllocator);

            STARTPOOL(x);

            if (veryVerbose) {
                T_ P_(THREADS_THREADS) P_(QUEUE_CAPACITY_THREADS) P(SLEEP_TIME)
            }

            double startCpuTime = getCurrentCpuTime();
            if (veryVerbose) {
                 T_ P(startCpuTime)
            }
            bslmt::ThreadUtil::microSleep(0, SLEEP_TIME);
            double consumedCpuTime = getCurrentCpuTime() - startCpuTime;

            if (veryVerbose) {
                T_ P(consumedCpuTime)
            }
            x.stop();

            // Failure if more than 10 times idle CPU time consumed per thread
            // plus 50ms (to allow for imprecisions in timing)
            if (verbose && !(consumedCpuTime < maxConsumedCpuTime)) {
                T_ P(maxConsumedCpuTime)
            }
            LOOP2_ASSERT(consumedCpuTime,
                         maxConsumedCpuTime,
                         consumedCpuTime < maxConsumedCpuTime);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SYNCHRONOUS SIGNALS
        //   Verify that threads managed by the thread pool block all but the
        //   synchronous signals.
        //
        // Plan:
        //   Enqueue a function, that, when executed, verifies that all but
        //   the synchronous signals are blocked.
        //
        // Testing:
        // --------------------------------------------------------------------

#if defined(BSLS_PLATFORM_OS_UNIX)
        if (verbose) cout << "TESTING SYNCHRONOUS SIGNALS" << endl
                          << "===========================" << endl ;
        {
            bslmt::ThreadAttributes attr;
            enum {
                THREADS = 5,
                QUEUE_CAPACITY = 10
            };
            Obj x(attr, THREADS, QUEUE_CAPACITY, &testAllocator);
            STARTPOOL(x);
            x.enqueueJob(TestSynchronousSignals, NULL);
            x.stop();
        }
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'numActiveThreads'
        //
        // Concerns:
        //   (1) 'numActiveThreads' of a stopped threadPool is 0.
        //   (2) 'numActiveThreads' of a heavily loaded threadPool is
        //       numThreads.
        //   (3) 'numActiveThreads' of an idle threadPool 0.
        // --------------------------------------------------------------------
          if (verbose) cout << "TESTING numActiveThreads\n"
                            << "==========================" << endl ;

          enum {
              NUM_THREADS = 5
          };

          bdlmt::FixedThreadPool tp(NUM_THREADS, NUM_THREADS);

          ASSERT(0 == tp.numActiveThreads());

          tp.start();
          bslmt::Barrier barrier(NUM_THREADS+1);
          for (int i = 0; i < NUM_THREADS; ++i) {
              tp.enqueueJob(bdlf::BindUtil::bind(&bslmt::Barrier::wait,
                                                 &barrier));
          }

          for (int i = 0; i < 100; ++i) {
            if (NUM_THREADS == tp.numActiveThreads()) {
                break;
            }
              bslmt::ThreadUtil::microSleep(10000); // 10 ms
          }
          ASSERT(NUM_THREADS == tp.numActiveThreads());

          barrier.wait();

          for (int i = 0; i < 100; ++i) {
            if (0 == tp.numActiveThreads()) {
                break;
            }
              bslmt::ThreadUtil::microSleep(10000); // 10 ms
          }
          ASSERT(0 == tp.numActiveThreads());

          tp.shutdown();

          ASSERT(0 == tp.numActiveThreads());

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING a job enqueuing other jobs
        //   Verify that an enqueued job, when executed, can enqueue jobs.
        //
        // Plan:
        //   Enqueue a function, that, when executed, increments a global
        //   counter (initialized to zero) and enqueues itself if the
        //   counter has reached a limit.
        //
        // Testing:
        //   int enqueueJob(FixedThreadPoolJobFunc , void *);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING a job enqueuing other jobs\n"
                          << "====================================" << endl ;
        {
            bslmt::ThreadAttributes attr;
            Obj localX(attr, THREADS, QUEUE_CAPACITY, &testAllocator);
            xP = &localX;
            STARTPOOL(localX);
            localX.enqueueJob(&testJobFunction5, NULL);
            barrier.wait();
            ASSERT(DEPTH_LIMIT == depthCounter);
            if (veryVerbose) { T_ P(depthCounter) }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'tryEnqueueJob'
        //   Verify that tryEnqueueJob does not block,
        //
        // Plan:
        //   First, for a sequence of independent numbers N of threads and
        //   queue capacities, create a thread pool with N threads and capacity
        //   M, but before starting it try to enqueue a job and verify that it
        //   returns a negative value.  Then start the pool, and try enqueuing
        //   N jobs that block on a barrier.  Verify that the next M jobs are
        //   enqueued successfully, but that the next job fails to enqueue,
        //   returning a positive value.  Then free all the jobs to avoid a
        //   timeout.
        //
        // Testing:
        //   int tryEnqueueJob(FixedThreadPoolJobFunc , void *);
        // --------------------------------------------------------------------

        static const struct {
            int d_lineNum;
            int d_numThreads;
            int d_maxNumJobs;
        } VALUES[] = {
            //line min threads max threads
            //---- ----------- -----------
            { L_ ,         1 ,        1  },
            { L_ ,         2 ,        2 },
            { L_ ,         10,        10 },
            { L_ ,         10,        50 },
            { L_ ,         50,        75},
            { L_ ,         25,        80}
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        if (verbose) cout << "TESTING 'tryEnqueueJob'\n"
                          << "=======================" << endl ;

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int THREADS  = VALUES[i].d_numThreads;
            const int QUEUE_CAPACITY  = VALUES[i].d_maxNumJobs;

            bslmt::ThreadAttributes attr;
            Obj x(attr, THREADS, QUEUE_CAPACITY, &testAllocator);
            const Obj& X = x;

            bslmt::Barrier startBarrier(THREADS + 1);
            bslmt::Barrier stopBarrier(THREADS + 1);

            TestJobFunctionArgs1 args;
            args.d_startBarrier_p = &startBarrier;
            args.d_stopBarrier_p  = &stopBarrier;

            ASSERT(0 != x.tryEnqueueJob(testJobFunction3, &args));

            STARTPOOL(x);

            for (int j = 0; j < THREADS; ++j) {
                LOOP_ASSERT(i, 0 == x.tryEnqueueJob(testJobFunction3, &args));
            }
            startBarrier.wait(); // make sure that threads have indeed started
            LOOP_ASSERT(i, 0 == X.numPendingJobs()); // and that queue is empty

            TestJobFunctionArgs1 emptyArgs;
            emptyArgs.d_startBarrier_p = 0;
            emptyArgs.d_stopBarrier_p  = 0;

            for (int j = 0; j < QUEUE_CAPACITY; ++j) {
                LOOP_ASSERT(i, 0 == x.tryEnqueueJob(testJobFunction3,
                                                    &emptyArgs));
            }
            // queue must now be full
            LOOP_ASSERT(i, 0 != x.tryEnqueueJob(testJobFunction3, &emptyArgs));

            stopBarrier.wait(); // unblock threads
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'drain' and 'stop' and 'shutdown'
        //   Verify that the 'drain', 'stop' and 'shutdown' behave as expected.
        //
        // Plan:
        //   First, for a sequence of independent number of threads, create a
        //   thread pool and verify that those threads are started.  Then spool
        //   the same number of jobs and assert they all the jobs are actively
        //   being processed without any of them blocking the others.  Finally
        //   let them complete and verify that after all the jobs have
        //   completed, those threads are again waiting.
        //
        //   Next, for a sequence of independent number N of threads and queue
        //   capacity, create a thread pool and verify that N threads are
        //   started.  Then spool more jobs to fill the queue to capacity and
        //   assert that N of them are being processed while the others are
        //   enqueued.  Next let the jobs complete and assert that after
        //   shutting down the pool, all the threads are stopped.
        //
        // Testing:
        //   int enqueueJob(FixedThreadPoolJobFunc , void *);
        //   void stop();
        //   void drain();
        //   void shutdown();
        // --------------------------------------------------------------------

        static const struct {
            int d_lineNum;
            int d_numThreads;
            int d_maxNumJobs;
        } VALUES[] = {
            //line min threads max threads
            //---- ----------- -----------
            { L_ ,         1 ,        1  },
            { L_ ,         2 ,        2 },
            { L_ ,         10,        10 },
            { L_ ,         10,        50 },
            { L_ ,         50,        75},
            { L_ ,         25,        80}
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        if (verbose) cout << "Testing: 'drain', 'stop', and 'shutdown'\n"
                          << "=======================================" << endl;

        if (veryVerbose) cout << "\tTesting: 'drain'\n"
                              << "\t----------------" << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int THREADS  = VALUES[i].d_numThreads;
            const int QUEUE_CAPACITY  = VALUES[i].d_maxNumJobs;

            bslmt::Mutex mutex;
            bslmt::Condition startCond;
            bslmt::Condition stopCond;
            TestJobFunctionArgs args;

            args.d_mutex = &mutex;
            args.d_startCond = &startCond;
            args.d_stopCond = &stopCond;
            args.d_count = 0;

            bslmt::ThreadAttributes attr;
            Obj x(attr, THREADS, QUEUE_CAPACITY, &testAllocator);
            const Obj& X = x;

            LOOP_ASSERT(i, THREADS        == X.numThreads());
            LOOP_ASSERT(i, QUEUE_CAPACITY == X.queueCapacity());
            LOOP_ASSERT(i, 0              == X.numThreadsStarted());
            if (veryVerbose) {
                T_ P_(i) T_ P_(THREADS) P(QUEUE_CAPACITY)
            }

            ASSERT(0 != x.enqueueJob(testJobFunction1, &args));

            STARTPOOL(x);
            LOOP_ASSERT(i, 0 == x.numPendingJobs());

            mutex.lock();
            for (int j = 0; j < THREADS; j++) {
                args.d_startSig = 0;
                args.d_stopSig = 0;
                x.enqueueJob(testJobFunction1, &args);
                while( !args.d_startSig ) {
                    startCond.wait(&mutex);
                }
            }
            if (veryVeryVerbose) {
                T_ P_(X.numActiveThreads()) P(X.numPendingJobs())
            }
            args.d_stopSig++;
            mutex.unlock();
            stopCond.broadcast();
            x.drain();
            if (veryVeryVerbose) {
                T_ P_(X.numThreadsStarted()) P(X.queueCapacity())
            }
            LOOP_ASSERT(i, THREADS == X.numThreadsStarted());
            LOOP_ASSERT(i, 0       == X.numPendingJobs());

            // We can't assert necessarily that the number of active threads is
            // 0, since the threads that are running the barrier in drain() may
            // still be running, although none of our functors should be
            // running.
        }

        if (veryVerbose) cout << "\n\tTesting: 'stop'"
                              << "\n\t--------------" << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int THREADS  = VALUES[i].d_numThreads;
            const int QUEUE_CAPACITY  = VALUES[i].d_maxNumJobs;

            bslmt::Mutex mutex;
            bslmt::Condition startCond;
            bslmt::Condition stopCond;
            TestJobFunctionArgs args;

            args.d_mutex = &mutex;
            args.d_startCond = &startCond;
            args.d_stopCond = &stopCond;
            args.d_count = 0;

            bslmt::ThreadAttributes attr;
            Obj x(attr, THREADS, QUEUE_CAPACITY, &testAllocator);

            const Obj& X = x;

            if (veryVerbose) {
                T_ P_(i) T_ P_(THREADS) P(QUEUE_CAPACITY)
            }

            LOOP_ASSERT(i, 0              == X.numThreadsStarted());
            LOOP_ASSERT(i, QUEUE_CAPACITY == X.queueCapacity());

            STARTPOOL(x);
            LOOP_ASSERT(i, THREADS        == X.numThreadsStarted());

            mutex.lock();
            for (int j = 0; j < QUEUE_CAPACITY; j++) {
                args.d_startSig = 0;
                args.d_stopSig = 0;
                x.enqueueJob(testJobFunction1, &args);
                while(j < THREADS && !args.d_startSig ) {
                    startCond.wait(&mutex);
                }
            }
            args.d_stopSig++;
            mutex.unlock();
            stopCond.broadcast();
            x.stop();
            ASSERT(QUEUE_CAPACITY == args.d_count);
            ASSERT(0 == X.numThreadsStarted());
            ASSERT(0 == X.numActiveThreads());
            ASSERT(0 == X.numPendingJobs());
            if (veryVeryVerbose) {
                T_ P_(X.numActiveThreads()) P(X.numPendingJobs())
            }
        }

        if (veryVerbose) cout << "\n\tTesting: 'shutdown'"
                              << "\n\t------------------"
                              << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int THREADS  = VALUES[i].d_numThreads;
            const int QUEUE_CAPACITY  = VALUES[i].d_maxNumJobs;

            if (veryVerbose) {
                T_ P_(i) T_ P_(THREADS) P(QUEUE_CAPACITY)
            }

            bslmt::Mutex mutex;
            bslmt::Condition startCond;
            bslmt::Condition stopCond;
            TestJobFunctionArgs args;

            args.d_mutex = &mutex;
            args.d_startCond = &startCond;
            args.d_stopCond = &stopCond;
            args.d_count = 0;

            bslmt::ThreadAttributes attr;
            Obj x(attr, THREADS, QUEUE_CAPACITY, &testAllocator);

            const Obj& X = x;

            LOOP_ASSERT(i, THREADS        == X.numThreads());
            LOOP_ASSERT(i, 0              == X.numThreadsStarted());
            LOOP_ASSERT(i, QUEUE_CAPACITY == X.queueCapacity());

            if (veryVerbose) cout << "\tStarting pool.\n";
            STARTPOOL(x);
            mutex.lock();
            for (int j=0; j < QUEUE_CAPACITY; ++j) {
                args.d_startSig = 0;
                args.d_stopSig = 0;
                x.enqueueJob(testJobFunction1,&args);
                while(j < THREADS && !args.d_startSig ) {
                    startCond.wait(&mutex);
                }
            }
            ASSERT(THREADS                  == X.numThreadsStarted());
            ASSERT(QUEUE_CAPACITY - THREADS == X.numPendingJobs());
            if (veryVeryVerbose) {
                T_ P(X.queueCapacity())
                T_ P_(X.numActiveThreads()) P(x.numPendingJobs())
            }
            for(int j = 0; j < THREADS; ++j ) {
                x.enqueueJob(&testJobFunction2, &args);
                if (veryVerbose) cout << "\tEnqueuing job2.\n";
            }
            ASSERT(THREADS        == X.numThreadsStarted());
            ASSERT(QUEUE_CAPACITY == X.numPendingJobs());
            if (veryVeryVerbose) {
                T_ P(X.queueCapacity())
                T_ P_(X.numActiveThreads()) P(X.numPendingJobs())
            }
            args.d_stopSig++;
            stopCond.broadcast();
            mutex.unlock();
            x.shutdown();

            ASSERT(0 == X.numThreadsStarted());
            ASSERT(0 == X.numActiveThreads());
            ASSERT(0 == X.numPendingJobs());
            if (veryVeryVerbose) {
                T_ P_(X.numThreadsStarted()) P(X.queueCapacity())
                T_ P_(X.numActiveThreads()) P(X.numPendingJobs())
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING DIRECT ACCESSORS:
        //   Verify that the the threadpool correctly initializes with the
        //   specified max/min threads and idle time values.
        //
        // Plan:
        //   For each of a sequence of independent number of threads and queue
        //   capacities, construct a threadpool object and verify that the
        //   values are as expected.
        //
        // Testing:
        //   int numThreads() const;
        //   int queueCapacity() const;
        //   bdlmt::FixedThreadPool(const bslmt::Attributes&,int , int);
        //   ~bdlmt::FixedThreadPool();
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing: direct accessors\n"
                          << "=========================" << endl;
        {
            static const struct {
                int d_lineNum;
                int d_numThreads;
                int d_maxNumJobs;
            } VALUES[] = {
                //line num threads queue capacity
                //---- ----------- --------------
                { L_ ,         10,        50 },
                { L_ ,         1 ,        1  },
                { L_ ,         50,        100},
                { L_ ,         2 ,        22 },
                { L_ ,        100,        200}
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int THREADS  = VALUES[i].d_numThreads;
                const int QUEUE_CAPACITY  = VALUES[i].d_maxNumJobs;

                bslmt::ThreadAttributes attr;
                Obj x(attr, THREADS, QUEUE_CAPACITY, &testAllocator);

                const Obj& X = x;

                if (veryVerbose) {
                    T_ P_(i) T_ P_(THREADS) P(QUEUE_CAPACITY)
                }

                LOOP_ASSERT(i, THREADS        == X.numThreads());
                LOOP_ASSERT(i, 0              == X.numThreadsStarted());
                LOOP_ASSERT(i, QUEUE_CAPACITY == X.queueCapacity());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Make sure standard usage works as expected.
        //
        // Plan:
        //   Create a thread pool, assert that the accessors have the expected
        //   value, start the pool, enqueue an empty job, drain the pool, and
        //   declare victory.
        //
        // TESTING:
        //   This test case exercises basic functionality but tests *nothing*.
        // --------------------------------------------------------------------

        bslmt::ThreadAttributes attr;
        const int THREADS  = 10;
        const int QUEUE_CAPACITY  = 50;
        Obj x(attr, THREADS, QUEUE_CAPACITY, &testAllocator);

        ASSERT(0              == x.numThreadsStarted());
        ASSERT(QUEUE_CAPACITY == x.queueCapacity());
        ASSERT(0              == x.numPendingJobs());

        STARTPOOL(x);
        ASSERT(THREADS        == x.numThreadsStarted());
        ASSERT(QUEUE_CAPACITY == x.queueCapacity());
        ASSERT(0              == x.numPendingJobs());

        x.drain();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING HELPER FUNCTIONS
        //   Verify that the support functions used to test
        //   'bdlmt::FixedThreadPool' behave as expected.  The
        //   'testJobFunction1' function is expected to increment the supplied
        //   counter, signal a condition variable indicating that it has
        //   started, and wait for a stop condition to be signaled before
        //   exiting.  'testJobFunction2' is expected to simply increment the
        //   supplied counter, signal a condition variable, indicating that it
        //   has started, and exit.
        //
        // Plan:
        //   First create NITERATIONS threads that call 'testJobFunction1'.
        //   After each thread is started, verify that it increments the
        //   counter properly and that it waits for the stop condition before
        //   returning.  Then signal the stop the and verify that all the
        //   threads exit.
        //
        //   Next create NITERATIONS threads that call 'testJobFunction2'.
        //   After each thread is started, verify that it increments the
        //   counter properly.  Finally verify that each thread exits properly.
        //
        // Testing:
        //   testJobFunction1(TestJobFunctionArgs *d_args);
        //   testJobFunction2(TestJobFunctionArgs *d_args);
        //   testJobFunction3(TestJobFunctionArgs1 *d_args);
        // --------------------------------------------------------------------

        if (verbose) cout << "HELPER FUNCTION TEST" << endl
                          << "====================" << endl;

        {
            const int NITERATIONS=50;

            if (veryVerbose) cout << "\tTesting: 'testJobFunction1'" << endl
                                  << "\t===========================" << endl;

            bslmt::Mutex mutex;
            bslmt::Condition startCond;
            bslmt::Condition stopCond;
            TestJobFunctionArgs args;

            args.d_mutex = &mutex;
            args.d_startCond = &startCond;
            args.d_stopCond = &stopCond;
            args.d_count = 0;

            bslmt::ThreadUtil::Handle threadHandles[NITERATIONS];
            bslmt::ThreadAttributes attributes;

            mutex.lock();
            for (int i=0; i<NITERATIONS; ++i) {
                args.d_startSig = 0;
                args.d_stopSig = 0;
                bslmt::ThreadUtil::create(
                          &threadHandles[i],
                          attributes,
                          (bslmt::ThreadUtil::ThreadFunction)&testJobFunction1,
                          &args);
                while( !args.d_startSig ) {
                    startCond.wait(&mutex);
                }
                LOOP_ASSERT(i, (i+1) == args.d_count);
            }
            args.d_stopSig++;
            stopCond.broadcast();
            mutex.unlock();
            ASSERT(NITERATIONS == args.d_count);

            for (int i=0; i<NITERATIONS; ++i) {
                bslmt::ThreadUtil::join(threadHandles[i]);
            }
        }

        if (veryVerbose) cout << "\tTesting: 'testJobFunction2'\n"
                              << "\t===========================" << endl;
        {
            const int NITERATIONS=50;

            bslmt::Mutex mutex;
            bslmt::Condition startCond;
            bslmt::Condition stopCond;
            TestJobFunctionArgs args;

            args.d_mutex = &mutex;
            args.d_startCond = &startCond;
            args.d_stopCond = &stopCond;
            args.d_count = 0;

            bslmt::ThreadUtil::Handle threadHandles[NITERATIONS];
            bslmt::ThreadAttributes attributes;

            for (int i=0; i<NITERATIONS; ++i) {
                mutex.lock();
                args.d_startSig=0;
                args.d_stopSig=0;
                bslmt::ThreadUtil::create(
                          &threadHandles[i],
                          attributes,
                          (bslmt::ThreadUtil::ThreadFunction)&testJobFunction2,
                          &args);
                while( !args.d_startSig ) {
                    startCond.wait(&mutex);
                }
                LOOP_ASSERT(i, (i+1) == args.d_count);
                mutex.unlock();
            }
            ASSERT(NITERATIONS == args.d_count);

            for (int i=0; i<NITERATIONS; ++i) {
                bslmt::ThreadUtil::join(threadHandles[i]);
            }
        }

        if (veryVerbose) cout << "\tTesting: 'testJobFunction3'\n"
                              << "\t===========================" << endl;
        {
            const int NITERATIONS=50;

            bslmt::Barrier startBarrier(NITERATIONS + 1);
            bslmt::Barrier stopBarrier(NITERATIONS + 1);
            TestJobFunctionArgs1 args;

            args.d_startBarrier_p = &startBarrier;
            args.d_stopBarrier_p  = &stopBarrier;
            args.d_count = 0;

            bslmt::ThreadUtil::Handle threadHandles[NITERATIONS];
            bslmt::ThreadAttributes attributes;

            for (int i=0; i<NITERATIONS; ++i) {
                bslmt::ThreadUtil::create(
                          &threadHandles[i],
                          attributes,
                          (bslmt::ThreadUtil::ThreadFunction)&testJobFunction3,
                          &args);
            }
            ASSERT(0 == args.d_count);
            startBarrier.wait();
            // All threads increment args.d_count
            stopBarrier.wait();
            ASSERT(NITERATIONS == args.d_count);

            for (int i=0; i<NITERATIONS; ++i) {
                bslmt::ThreadUtil::join(threadHandles[i]);
            }
        }

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING PERFORMANCE SCALABILITY
        //   Verify that enqueuing on multiple threads scales linearly with the
        //   number of threads.
        //
        // Plan:
        //   Enqueue a large number of jobs that perform an loop long enough to
        //   ensure that the jobs don't get dequeued faster than they are
        //   enqueued.  This is necessary because as we add more threads, with
        //   only one thread enqueuing the jobs, the queue is going to get
        //   depleted at a faster rate, but we want to measure the effect of
        //   contention on the job queue and not the contention on the jobs
        //   (which would happen if the jobs were accessing some common
        //   resource).  We also want to see the plateau which is the number of
        //   threads needed to keep the queue small (i.e., to deplete as fast
        //   as it is enqueued), and that number will depend on how long the
        //   jobs take.  However, for that number to be meaningful, the jobs
        //   must all take the same amount of time, regardless of how many
        //   threads.  Display the elapsed time as a function of the number of
        //   threads.
        //
        // Testing:
        //   TESTING PERFORMANCE SCALABILITY
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING Scalability with several threads\n"
                 << "========================================" << endl ;

        if (verbose)
            cout << "\tUse the verbose parameter to control how long\n"
               << "\t  the jobs should take (10=fast, 100=medium, 500=slow)\n"
               << "\tUsing value " << verbose << "\n";

        const int MAX_NTHREADS = 10;
        for (int i = 1; i < MAX_NTHREADS; ++i)
        {
            cout << "\tUsing " << i << " threads.\n";
            bslmt::ThreadAttributes attr;
            const int NTHREADS = i;
            const int NQUEUE_CAPACITY = 10000;

            Obj localX(attr, NTHREADS, NQUEUE_CAPACITY, &testAllocator);
            const int ITERATIONS = 1000 * 1000;
            double queueSize(0); // to avoid overflows and casts

            bsls::Stopwatch timer;  timer.start();
            xP = &localX;
            STARTPOOL(localX);
            const Obj::Job job(bdlf::BindUtil::bind(&testJobFunction4,
                                                    verbose ? verbose : 100));
            for (int i = 0; i < ITERATIONS; ++i) {
                localX.enqueueJob(job);
                queueSize += localX.numPendingJobs();
            }
            localX.drain();
            double averageQueueSize = queueSize / ITERATIONS;
            P(averageQueueSize)
            P(timer.elapsedTime())
            localX.shutdown();
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

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
