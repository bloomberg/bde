// bdlmt_threadpool.t.cpp                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlmt_threadpool.h>

#include <bslim_testutil.h>

#include <bslmt_configuration.h>

#include <bslma_testallocator.h>

#include <bdlf_bind.h>

#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_timeinterval.h>
#include <bsls_timeutil.h>
#include <bsls_types.h>
#include <bsls_atomic.h>

#include <bdlt_currenttime.h> // For test only
#include <bslmt_barrier.h>    // For test only
#include <bslmt_lockguard.h>  // For test only
#include <bslmt_threadattributes.h>     // For test only
#include <bslmt_threadutil.h>     // For test only
#include <bsl_cstddef.h>
#include <bsl_cstdio.h>           // For FILE in usage example
#include <bsl_cstdlib.h>          // for atoi
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
//
// [3 ] bdlmt::ThreadPool(const bslmt::Attributes&,int , int , int );
// [3 ] ~bdlmt::ThreadPool();
// [  ] int enqueueJob(bsl::function<void()>);
// [4 ] int enqueueJob(ThreadPoolJobFunc , void *);
// [4 ] void start();
// [4 ] void stop();
// [4 ] void drain();
// [4 ] void shutdown();
// [4 ] int numActiveThreads() const;
// [4 ] int numWaitingThreads() const;
// [4 ] int numPendingJobs() const;
// [3 ] int minThreads() const;
// [3 ] int maxThreads() const;
// [3 ] int maxIdleTime() const;
// [3 ] int threadFailures() const;
// [8 ] double percentBusy() const
// [8 ] double resetPercentBusy()
// ----------------------------------------------------------------------------
// [1 ] Breathing test
// [6 ] Max idle time functionality
// [5 ] Min/max thread functionality
// [7 ] TESTING a job enqueuing other jobs
// [9 ] TESTING SYNCHRONOUS SIGNALS
// [10] USAGE EXAMPLE
// [11] USAGE EXAMPLE (Functor Interface)
// [12] TESTING CPU consumption of an idle pool.

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

typedef bdlmt::ThreadPool Obj;

// ============================================================================
//                 HELPER CLASSES AND FUNCTIONS  FOR TESTING
// ----------------------------------------------------------------------------

#define STARTPOOL(x) \
    if (0 != x.start()) { \
        cout << "Thread start() failed.  Thread quota exceeded?" \
             << bsl::endl; \
        ASSERT(false); \
        break; } // things are SNAFU

void noopFunc()
{
}

struct TestJobFunctionArgs {
    bslmt::Condition *d_startCond;
    bslmt::Condition *d_stopCond;
    bslmt::Mutex     *d_mutex;
    volatile int     d_count;
    volatile int     d_startSig;
    volatile int     d_stopSig;
};

struct TestJobFunctionArgs1 {
    bslmt::Barrier    *d_startBarrier_p;
    bslmt::Barrier    *d_stopBarrier_p;
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

    // verify that thread pool does not, by mistake, leave ALL the signals
    // unblocked.  Testing for 1 signal should be enough.
#ifndef BSLS_PLATFORM_OS_CYGWIN
    ASSERT(sigismember(&blockedSet, SIGINT) == 1);
#endif

}
#endif

void TestJobFunction1(void *ptr)
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
    while ( !args->d_stopSig ) {
        args->d_stopCond->wait(args->d_mutex);
    }
}

void TestJobFunction3(void *ptr)
    // This function is used to simulate a thread pool job.  It accepts a
    // pointer to a pointer to a structure containing a mutex and a conditional
    // variable.  All the threads calling this function will wait until the
    // main thread invokes the 'wait' on a barrier signifying the start of the
    // function.  After that all the threads calling this function will wait
    // until the main thread invokes the 'wait' on a barrier signifying the end
    // of the function
{
    TestJobFunctionArgs1 *args = (TestJobFunctionArgs1*)ptr;
    ++args->d_count;

    args->d_startBarrier_p->wait();
    args->d_stopBarrier_p->wait();
}

static void TestJobFunction2( void *ptr )
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
namespace THREADPOOL_USAGE_EXAMPLE {

///Usage
///-----
// This example demonstrates the use of a 'bdlmt::ThreadPool' to parallelize a
// segment of program logic.  The example implements a multi-threaded file
// search utility.  The utility searches multiple files for a string, similar
// to the Unix command 'fgrep'; the use of a 'bdlmt::ThreadPool' allows the
// utility to search multiple files concurrently.
//
// The example program will take as input a string and a list of files to
// search.  The program creates a 'bdlmt::ThreadPool', and then enqueues a
// single "job" for each file to be searched.  Each thread in the pool will
// take a job from the queue, open the file, and search for the string.  If a
// match is found, the job adds the filename to an array of matching filenames.
// Because this array of filenames is shared across multiple jobs and across
// multiple threads, access to the array is controlled via a 'bslmt::Mutex'.
//
///Setting ThreadPool Attributes
///- - - - - - - - - - - - - - -
// To get started, we declare thread attributes, to be used in constructing the
// thread pool.  In this example, our choices for minimum search threads and
// maximum idle time are arbitrary; we don't expect the thread pool to become
// idle, so the thread pool should not begin to delete unused threads before
// the program terminates.
//
// However, a maximum number of 50 threads is meaningful, and may affect
// overall performance.  The maximum should cover the expected peak, in this
// case, the maximum number of files to search.  However, if the maximum is too
// large for a given platform, it may cause a bottleneck as the operating
// system spends significant resources switching context among multiple
// threads.
//..
    #define MIN_SEARCH_THREADS     10
    #define MAX_SEARCH_THREADS     50
    #define MAX_SEARCH_THREAD_IDLE 100 // use a very short idle time since new
                                       // jobs will arrive only at startup
//..
// Below is the structure that will be used to pass arguments to the file
// search function.  Since each job will be searching a separate file, a
// distinct instance of the structure will be used for each job.
//..
    struct my_FastSearchJobInfo {
        const bsl::string        *d_word;    // word to search for
        const bsl::string        *d_path;    // path of the file to search
        bslmt::Mutex              *d_mutex;   // mutex to control access to the
                                             // result file list
        bsl::vector<bsl::string> *d_outList; // list of matching files
    };
//..
//
///The "void function/void pointer" Interface
///- - - - - - - - - - - - - - - - - - - - -
// 'myFastSearchJob' is the search function to be executed as a job by threads
// in the thread pool.  It is declared with extern "C" linkage to match the
// "void function/void pointer" interface.  The single 'void *' argument is
// received and cast to point to a 'struct my_FastSearchJobInfo', which then
// points to the search string and a single file to be searched.  Note that
// different 'my_FastSearchInfo' structures for the same search request will
// differ only in the attribute 'd_path', which points to a specific filename
// among the set of files to be searched; other fields will be identical across
// all structures for a given Fast Search.
//
// See the following section for an illustration of the functor interface.
//..
    extern "C" {
    static void myFastSearchJob(void *arg)
    {
        my_FastSearchJobInfo *job =  (my_FastSearchJobInfo*)arg;
        FILE *file;

        file = fopen(job->d_path->c_str(), "r");

        if (file) {
            char  buffer[1024];
            size_t nread;
            int wordLen = job->d_word->length();
            const char *word = job->d_word->c_str();

            nread = fread(buffer, 1, sizeof(buffer) - 1, file);
            while (nread >= wordLen) {
                buffer[nread] = 0;
                if (strstr(buffer, word)) {
//..
// If we find a match, we add the file to the result list and return.  Since
// the result list is shared among multiple processing threads, we use a mutex
// lock to regulate access to the list.  We use a 'bslmt::LockGuard' to manage
// access to the mutex lock.  This template object acquires a mutex lock on
// 'job->d_mutex' at construction, releases that lock on destruction.  Thus,
// the mutex will be locked within the scope of the 'if' block, and released
// when the program exits that scope.
//
// See 'bslmt_threadutil' for information about the 'bslmt::Mutex' class, and
// component 'bslmt_lockguard' for information about the 'bslmt::LockGuard'
// template class.
//..
                    bslmt::LockGuard<bslmt::Mutex> lock(job->d_mutex);
                    job->d_outList->push_back(*job->d_path);
                    break;  // bslmt::LockGuard destructor unlocks mutex.
                }
             }
             bsl::memcpy(buffer, &buffer[nread - wordLen - 1], wordLen - 1);
             nread = fread(buffer + wordLen - 1, 1, sizeof(buffer) - wordLen,
                           file);
        }
        fclose(file);
    }

    } // extern "C"
//..
// Routine 'myFastSearch' is the main driving routine, taking three
// arguments: a single string to search for ('word'), a list of files to
// search, and an output list of files.  When the function completes, the file
// list will contain the names of files where a match was found.
//..
    void  myFastSearch(const bsl::string&              word,
                       const bsl::vector<bsl::string>& fileList,
                       bsl::vector<bsl::string>&       outFileList)
    {
        bslmt::Mutex            mutex;
        bslmt::ThreadAttributes defaultAttributes;
//..
// We initialize the thread pool using default thread attributes.  We then
// start the pool so that the threads can begin while we prepare the jobs.
//..
        bdlmt::ThreadPool       pool(defaultAttributes,
                                     MIN_SEARCH_THREADS,
                                     MAX_SEARCH_THREADS,
                                     MAX_SEARCH_THREAD_IDLE);

        if (0 != pool.start()) {
            bsl::cerr << "Failed to start minimum number of threads.  "
                      << "Thread quota exceeded?\n";
            ASSERT(false);

            // things are SNAFU

            return;                                                   // RETURN
        }
//..
// For each file to be searched, we create the job info structure that will be
// passed to the search function and add the job to the pool.
//
// As noted above, all jobs will share a single mutex to guard the output file
// list.  Function 'myFastSearchJob' uses a 'bslmt::LockGuard' on this mutex to
// serialize access to the list.
//..
        int count = fileList.size();
        my_FastSearchJobInfo *jobInfoArray = new my_FastSearchJobInfo[count];

        for (int i = 0; i < count; ++i) {
            my_FastSearchJobInfo &job = jobInfoArray[i];
            job.d_word    = &word;
            job.d_path    = &fileList[i];
            job.d_mutex   = &mutex;
            job.d_outList = &outFileList;
            pool.enqueueJob(myFastSearchJob, &job);
        }
//..
// Now we simply wait for all the jobs in the queue to complete.  Any matched
// files should have been added to the output file list.
//..
        pool.drain();
        delete[] jobInfoArray;
    }
//..
//
///The Functor Interface
///- - - - - - - - - - -
// The "void function/void pointer" convention is idiomatic for C programs.
// The 'void' pointer argument provides a generic way of passing in user data,
// without regard to the data type.  Clients who prefer better or more explicit
// type safety may wish to use the Functor Interface instead.  This interface
// uses the 'bsl::function' component to provide type-safe wrappers that
// can match argument number and type for a C++ free function or member
// function.
//
// To illustrate the Functor Interface, we will make two small changes to the
// usage example above.  First, we change the signature of the function that
// executes a single job, so that it uses a 'my_FastSearchJobInfo' pointer
// rather than a 'void' pointer.  With this change, we can remove the first
// executable statement, which casts the 'void *' pointer to
// 'my_FastSearchJobInfo *'.
//..
    static void my_FastFunctorSearchJob(my_FastSearchJobInfo *job)
    {
        FILE *file;

        file = fopen(job->d_path->c_str(), "r");

        // The rest of the function is unchanged.
        if (file) {
            char  buffer[1024];
            size_t nread;
            int wordLen = job->d_word->length();
            const char *word = job->d_word->c_str();

            nread = fread(buffer, 1, sizeof(buffer) - 1, file);
            while (nread >= wordLen) {
                buffer[nread] = 0;
                if (strstr(buffer, word)) {
                    bslmt::LockGuard<bslmt::Mutex> lock(job->d_mutex);
                    job->d_outList->push_back(*job->d_path);
                    break;  // bslmt::LockGuard destructor unlocks mutex.
                }
            }
            bsl::memcpy(buffer, &buffer[nread - wordLen - 1], wordLen - 1);
            nread = fread(buffer + wordLen - 1, 1, sizeof(buffer) - wordLen,
                          file);
        }
        fclose(file);
    }
//..
// Next, we make a change to the loop that enqueues the jobs in 'myFastSearch'.
// The function starts exactly as in the previous example:
//..
    static void myFastFunctorSearch(const string&         word,
                                    const vector<string>& fileList,
                                    vector<string>&       outFileList)
    {
        bslmt::Mutex            mutex;
        bslmt::ThreadAttributes defaultAttributes;
        bdlmt::ThreadPool       pool(defaultAttributes,
                                     MIN_SEARCH_THREADS,
                                     MAX_SEARCH_THREADS,
                                     MAX_SEARCH_THREAD_IDLE);

        if (0 != pool.start()) {
            bsl::cerr << "Failed to start minimum number of threads.  "
                      << "Thread quota exceeded?\n";
            ASSERT(false);

            // things are SNAFU

            return;                                                   // RETURN
        }

        int count = fileList.size();
        my_FastSearchJobInfo  *jobInfoArray = new my_FastSearchJobInfo[count];
//..
// We create a functor - a C++ object that acts as a function.  The thread pool
// will "execute" this functor (by calling its 'operator()' member function) on
// a thread when one becomes available.
//..
        for (int i = 0; i < count; ++i) {
            my_FastSearchJobInfo &job = jobInfoArray[i];
            job.d_word    = &word;
            job.d_path    = &fileList[i];
            job.d_mutex   = &mutex;
            job.d_outList = &outFileList;

            bsl::function<void()> jobHandle =
                          bdlf::BindUtil::bind(&my_FastFunctorSearchJob, &job);
            pool.enqueueJob(jobHandle);
        }
//..
// Note that the functor is created locally and handed to the thread pool.
// The thread pool copies the functor onto its internal queue, and takes
// responsibility for the copied functor until execution is complete.
//
// The function is completed exactly as it was in the previous example.
//..
        pool.drain();
        delete[] jobInfoArray;
    }
//..

}  // close namespace THREADPOOL_USAGE_EXAMPLE

// ============================================================================
//                          CASE 8 RELATED ENTITIES
// ----------------------------------------------------------------------------

static void counter(int *result, int num)
{
    while (num > 0) {
        *result += 1;
        --num;
    }
}

// ============================================================================
//                          CASE 7 RELATED ENTITIES
// ----------------------------------------------------------------------------
enum {
    DEPTH_LIMIT = 10,
            MIN = 5,
            MAX = 10, // must be greater than or equal to DEPTH_LIMIT
           IDLE = 100 // in milliseconds
};

Obj *xP;
bslmt::Barrier barrier(DEPTH_LIMIT + 1); // to ensure that all the threads have
                                        // started

bsls::AtomicInt depthCounter;

extern "C" {

void TestJobFunction7(void *ptr)
    // This function is used to simulate a thread pool job.  It enqueues itself
    // in the pool if the depth limit is not reached.
{
    ASSERT(depthCounter >= 0);
    ASSERT(depthCounter <= DEPTH_LIMIT);
    if (++depthCounter != DEPTH_LIMIT)
        xP->enqueueJob(TestJobFunction7, NULL);

    barrier.wait();
}

}

// ============================================================================
//                         CASE 13 RELATED ENTITIES
// ----------------------------------------------------------------------------

void testJobFunction13_2(bslmt::Barrier *barrier)
{
    barrier->wait();
}

class Test13Object {
    Obj *d_threadPool_p;
    bslmt::Barrier *d_barrier_p;

  private:
    // NOT IMPLEMENTED
    Test13Object(const Test13Object&);
    Test13Object& operator=(const Test13Object&);

  public:
    Test13Object(bslmt::Barrier *barrier_p, Obj *threadPool_p);
    ~Test13Object();
};

Test13Object::Test13Object(bslmt::Barrier *barrier_p, Obj *threadPool_p)
: d_threadPool_p(threadPool_p)
, d_barrier_p(barrier_p)
{
}

Test13Object::~Test13Object()
{
    bsl::function<void()> job =
                       bdlf::BindUtil::bind(&testJobFunction13_2, d_barrier_p);
    ASSERT(0 == d_threadPool_p->enqueueJob(job));
}

extern "C" {
    void testJobFunction13(void *ptr)
    {
        Test13Object *p = (Test13Object*)ptr;
        delete p;
    }
}

// ============================================================================
//                         CASE -1 RELATED ENTITIES
// ----------------------------------------------------------------------------

int testTimingJobFunction(const int N)
    // This function is used to simulate a thread pool job.  It accepts an
    // 'int' which controls the time taken by that job.  We let the job compute
    // some quantity (the actually job does not matter, only the time it
    // takes).  Here we compute the golden ratio.
{
    double result = 1.0;
    for (int i = 0; i < N; ++i) {
        result = 1 / (1 + result);
    }
    return result;
}

// ============================================================================
//                         CASE -2 RELATED ENTITIES
// ----------------------------------------------------------------------------

struct MinusTwoJob {
    void operator()()
    {
        ; // Do nothing.  In a successful test, this job won't even run.
    }
};

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2 ? atoi(argv[2]) : 0;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bslmt::Configuration::setDefaultThreadStackSize(
                    bslmt::Configuration::recommendedDefaultThreadStackSize());

    bslma::TestAllocator  testAllocator(veryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0: // 0 is always the first test case
      case 13: {
        // --------------------------------------------------------------------
        // VERIFY that functor are destroyed when the thread pool is not
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

        enum { MIN_THREADS = 1,
               MAX_THREADS = 1,
               IDLE_TIME   = 0 };

        bslmt::ThreadAttributes attr;
        Obj mX(attr, MIN_THREADS, MAX_THREADS, IDLE_TIME);

        STARTPOOL(mX);

        bslmt::Barrier barrier(2);
        {
            Test13Object* p = new Test13Object(&barrier, &mX);
            mX.enqueueJob(testJobFunction13, p);
        }

        barrier.wait();

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CPU time consumption of an idle pool.
        //   Verify that a threadpool consumes negligible CPU time when
        //   not being used.
        //
        // Concerns: that an idle thread pool spins the CPU.
        //
        // Plan:
        //   First measure CPU usage for 10 seconds, doing nothing.
        //   Instantiate a threadpool with the minimum number of threads set
        //        to the maximum number of threads, and idle time of 0 seconds.
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
                MIN_THREADS = 25,
                MAX_THREADS = MIN_THREADS,
                IDLE_TIME   = 0,            //
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
                           (additiveFudge + consumedIdleCpuTime * MAX_THREADS);
            if (veryVerbose) {
                P(maxConsumedCpuTime)
            }

            Obj x(attr, MIN_THREADS, MAX_THREADS, IDLE_TIME);

            STARTPOOL(x);

            if (veryVerbose) {
                T_ P_(IDLE_TIME) P_(MIN_THREADS) P_(MAX_THREADS) P(SLEEP_TIME)
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
            LOOP2_ASSERT(consumedCpuTime, maxConsumedCpuTime,
                         consumedCpuTime < maxConsumedCpuTime);
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE with Functor Interface
        //   This is a duplicate of test case 10, except that we use the
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

        using namespace THREADPOOL_USAGE_EXAMPLE;
        {
            bsl::vector<bsl::string> fileList;
            bsl::vector<bsl::string> outFileList;

            myFastFunctorSearch("bcep", fileList, outFileList);
            ASSERT(0 == outFileList.size());
        }

      } break;
      case 10: {
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

        if (verbose) cout << "Testing: Usage Example" << endl
                          << "======================" << endl ;

        using namespace THREADPOOL_USAGE_EXAMPLE;
        {
            bsl::vector<bsl::string> fileList;
            bsl::vector<bsl::string> outFileList;

            myFastSearch("hello", fileList, outFileList);
            ASSERT(0 == outFileList.size());
        }

      } break;
      case 9: {
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

        if (verbose) cout << "TESTING SYNCHRONOUS SIGNALS" << endl
                          << "===========================" << endl ;
        {

            bslmt::ThreadAttributes attr;
            enum {
                MIN = 5,
                MAX = 10,
                IDLE = 100
            };
            Obj x(attr, MIN, MAX, IDLE);
            STARTPOOL(x);
#if defined(BSLS_PLATFORM_OS_UNIX)
            x.enqueueJob(TestSynchronousSignals, NULL);
#endif
            x.stop();
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'percentBusy' and 'resetPercentBusy' METHODS
        //   Verify that percent busy metric is reported as expected.
        //
        // Plan:
        //   When thread pool is not running, the percent busy is always 0.
        //   When only one thread is in the thread pool and another thread
        //   constantly enqueuing, the percent busy should be almost 100%.
        //
        // Testing:
        //   double percentBusy() const
        //   double resetPercentBusy()
        // --------------------------------------------------------------------

        if (verbose)
            cout << "TESTING percentBusy AND resetPercentBusy" << endl
                 << "========================================" << endl;

        if (veryVerbose)
            cout << "\tWhen thread pool is not running." << endl;
        {
            enum {
                NUM_ITERATIONS = 1000,
                MIN_THREADS    = 1,
                MAX_THREADS    = 10000, // should not matter
                IDLE_TIME      = 0

            };
            bslmt::ThreadAttributes attributes;
            Obj                     mX(attributes,
                                       MIN_THREADS,
                                       MAX_THREADS,
                                       IDLE_TIME,
                                       &testAllocator);
            const Obj& X = mX;

            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                LOOP_ASSERT(i, 0.0 == X.percentBusy());
            }

            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                LOOP_ASSERT(i, 0.0 == mX.resetPercentBusy());
            }
        }

        if (veryVerbose)
            cout << "\tWhen thread pool is running.  " << endl;
        {
            enum {
                NUM_JOBS       = 100,
                MIN_THREADS    = 1,
                MAX_THREADS    = 1,
                IDLE_TIME      = 0

            };
            bslmt::ThreadAttributes attributes;
            Obj mX(attributes, MIN_THREADS, MAX_THREADS, IDLE_TIME);

            const Obj& X = mX;

            bsl::function<void()> noop(&noopFunc);

            STARTPOOL(mX);
            ASSERT(0 == mX.enqueueJob(noop));
            mX.stop();

            double percentBusy1 = X.percentBusy();
            LOOP_ASSERT(percentBusy1, 0 <= percentBusy1);

            double percentBusy2 = mX.resetPercentBusy();
            LOOP2_ASSERT(percentBusy1,
                         percentBusy2,
                         percentBusy1 >= percentBusy2);

            if (veryVerbose) { T_ P_(percentBusy1) P(percentBusy2) }

            double percentX  = X.percentBusy();
            double percentMX = mX.resetPercentBusy();

            LOOP_ASSERT(percentX, 0 == percentX);
            LOOP_ASSERT(percentMX, 0 == percentMX);

            int result = 0;

            bsl::function<void()> count =
                              bdlf::BindUtil::bind(&counter, &result, 2000000);

            STARTPOOL(mX);
            ASSERT(0 == mX.resetPercentBusy());
            for (int i = 0; i < NUM_JOBS; ++i) {
                ASSERT(0 == mX.enqueueJob(count));
            }
            mX.drain();
            percentBusy1 = X.percentBusy();
            percentBusy2 = mX.resetPercentBusy();

            if (veryVerbose) {
                T_ P_(percentBusy1) P(percentBusy2) T_ P(result)
            }

            LOOP_ASSERT(percentBusy1, 0 <= percentBusy1);
            ASSERT(2000000 * NUM_JOBS == result);
            LOOP2_ASSERT(percentBusy1,
                         percentBusy2,
                         percentBusy1 >= percentBusy2);

#if !defined(BSLS_PLATFORM_CMP_IBM) && !defined(BDE_BUILD_TARGET_OPT)
            LOOP_ASSERT(percentBusy1, 50. < percentBusy1);
            // Should be around 100%, but former values of 99% and 95%
            // triggered occasional failures, so we relax it a bit.  Xlc (at
            // least 8.0) seems to be so aggressive in its optimization that
            // the percentBusy is only at about 35% on all builds, and this
            // happens also in optimized builds on other platforms.  MS VC
            // 2005: percentBusy can be 9% (on 4 CPU box)
#endif
        }

      } break;
      case 7: {
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
        //   int enqueueJob(ThreadPoolJobFunc , void *);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING a job enqueuing other jobs" << endl
                          << "==================================" << endl;
        {
            bslmt::ThreadAttributes attr;
            Obj localX(attr, MIN, MAX, IDLE);
            xP = &localX;
            STARTPOOL(localX);
            localX.enqueueJob(TestJobFunction7, NULL);
            barrier.wait();
            ASSERT(DEPTH_LIMIT == depthCounter);
            if (veryVerbose) { T_ P(depthCounter) }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING max idle time functionality
        //   Verify that the threadpool properly destroys threads beyond
        //   the minimum number of threads after the configured idle
        //   time.
        //
        // Plan:
        //   Using an arbitrary set of MIN,MAX, and IDLE time values, create
        //   MAX concurrent jobs to force the creation of addition threads.
        //   Next complete all the jobs and create a new burst of 'BURST =
        //   (MAX-MIN)/2' concurrent jobs to keep 'BURST' threads busy,
        //   meanwhile the rest of 'MAX - BURST' threads will remain idle
        //   for 'IDLE' time and thus should be destroyed.  Now complete
        //   the 'BURST' jobs and assert that after IDLE time, all threads
        //   beyond the minimum number have exited.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing: max idle time functionality" << endl
                          << "====================================" << endl ;
        {
            const int MIN  = 5;
            const int MAX  = 10;
#ifndef BSLS_PLATFORM_OS_CYGWIN
            const int IDLE = 100; // in milliseconds
#else
            // Windows is pretty bad at timing.

            const int IDLE = 10000; // in milliseconds
#endif
            const int BURST= MIN + ((MAX-MIN)/2);

            TestJobFunctionArgs1 args;
            bslmt::Barrier startBarrier(MAX + 1);
            bslmt::Barrier stopBarrier(MAX + 1);
            args.d_startBarrier_p = &startBarrier;
            args.d_stopBarrier_p = &stopBarrier;
            args.d_count = 0;

            bslmt::ThreadAttributes attr;
            Obj x(attr, MIN, MAX, IDLE);

            if (veryVerbose) {
                T_ P_(IDLE) P_(MIN) P_(MAX) P(BURST)
            }

            STARTPOOL(x);
            ASSERT(MIN == x.numWaitingThreads());
            for (int j=0; j < MAX; ++j) {
                x.enqueueJob(TestJobFunction3, &args);
            }
            args.d_startBarrier_p->wait();

            ASSERT(MAX == x.numActiveThreads());
            ASSERT( 0  == x.numWaitingThreads());

            args.d_stopBarrier_p->wait();

            while (0 != x.numActiveThreads())
                bslmt::ThreadUtil::yield();
            bsls::Types::Int64 t1 = bsls::TimeUtil::getTimer();

            bslmt::Barrier startBarrier1(BURST + 1);
            bslmt::Barrier stopBarrier1(BURST + 1);
            args.d_startBarrier_p = &startBarrier1;
            args.d_stopBarrier_p = &stopBarrier1;
            for (int j=0; j < BURST; ++j) {
                x.enqueueJob(TestJobFunction3, &args);
            }

            args.d_startBarrier_p->wait();
            ASSERT(BURST == x.numActiveThreads());

            // The other MAX-BURST threads have been idle since t1, compute
            // their remaining time to IDLE.  Note: one millisecond has
            // 1000000 nanoseconds.

            bsls::Types::Int64 t2 = bsls::TimeUtil::getTimer();
            bsls::Types::Int64 duration = (t2 - t1) / 1000000;
            duration = (duration < IDLE) ? IDLE - duration : 0;

            // (Allow 30% tolerance)

            duration += duration * 0.3;

            if (veryVerbose) {
                T_ P_(t1) P_(t2) P(duration)
            }

            // This suspends for AT LEAST duration... one millisecond has 1000
            // microseconds.

            bslmt::ThreadUtil::microSleep(duration * 1000);
            bslmt::ThreadUtil::yield();
            ASSERT(BURST == x.numActiveThreads());

            // Also give a chance to the Threadpool to kill the idle threads.
            // But given the problematic likelihood of failure of this test, do
            // not assert '0 == x.numWaitingThreads()'.

            bslmt::ThreadUtil::microSleep(IDLE * 300);
            bslmt::ThreadUtil::yield();
            ASSERT(0 <= x.numWaitingThreads());
            if (0 < x.numWaitingThreads()) {
                cout << "WARNING: Failure in case 6 (still "
                     << x.numWaitingThreads() << " threads waiting)"
                     << endl;
            }

            args.d_stopBarrier_p->wait();

            // One millisecond has 1000 microseconds.  Use 30 % tolerance.
            // Again, given the problematic likelihood of failure of the last
            // test, do not assert 'MIN == x.numWaitingThreads()'.

            bslmt::ThreadUtil::microSleep(IDLE * 1300);
            bslmt::ThreadUtil::yield();
            if (0 < x.numActiveThreads()) {
                cout << "WARNING: Failure in case 6 (still "
                     << x.numActiveThreads() << " threads active after IDLE)"
                     << endl;
                while (0 < x.numActiveThreads()) {
                    bslmt::ThreadUtil::microSleep(IDLE * 1000);
                    bslmt::ThreadUtil::yield();
                }
            }
            ASSERT(    0 == x.numActiveThreads());
            ASSERT(  MIN <= x.numWaitingThreads());
            if (MIN < x.numWaitingThreads()) {
                cout << "WARNING: Failure in case 6 (still "
                     << x.numWaitingThreads() << " threads waiting)"
                     << endl;
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING min/max thread functionality
        //   Verify that thread pool spawns at least the minimum number of
        //   threads and no more than the maximum number of threads regardless
        //   of the number of queued items.
        //
        // Plan:
        //   First, for a sequence of independent min/max test values, create
        //   a thread pool and verify that min threads are waiting.  Then spool
        //   min jobs and assert they all the jobs are actively being processed
        //   without the creation of any new threads.  Finally verify the after
        //   all the jobs have completed, min threads are again waiting.
        //
        //   Next, for a sequence of independent min/max test values, create a
        //   a thread pool and verify that min threads are waiting.  Then spool
        //   max jobs and assert that new threads were created to handle to
        //   additional jobs.  Next Create one more job and assert that no
        //   additional threads were created and one item remained in queue.
        //
        // Testing:
        // --------------------------------------------------------------------
        {
            static const struct {
                int d_lineNum;
                int d_minThreads;
                int d_maxThreads;
                int d_maxIdle;
            } VALUES[] = {
                //line min threads max threads max idle
                //---- ----------- ----------- -----------
                { L_ ,         10,        50 ,     30*1000},
                { L_ ,         1 ,        1  ,      1*1000},
                { L_ ,         50,        80,      60*1000},
                { L_ ,         2 ,        22 ,     72*1000},
                { L_ ,         20,        80,     15*1000}
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            const int TIMEOUT = 2;

            if (verbose) cout << "Testing: min/max thread functionality\n"
                              << "====================================="
                              << endl;

            if (veryVerbose) cout << "\tTesting: min threads functionality\n"
                                  << "\t----------------------------------"
                                  << endl;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int MIN  = VALUES[i].d_minThreads;
                const int MAX  = VALUES[i].d_maxThreads;
                const int IDLE = VALUES[i].d_maxIdle;

                bslmt::Mutex mutex;
                bslmt::Condition startCond;
                bslmt::Condition stopCond;
                TestJobFunctionArgs args;

                args.d_mutex = &mutex;
                args.d_startCond = &startCond;
                args.d_stopCond = &stopCond;
                args.d_count = 0;

                bslmt::ThreadAttributes attr;
                Obj x(attr, MIN, MAX, IDLE);

                const Obj& X = x;

                if (veryVerbose) {
                    T_ P_(i) P(IDLE) T_ P_(MIN) P(MAX)
                }

                LOOP_ASSERT(i, MIN == X.minThreads());
                LOOP_ASSERT(i, MAX == X.maxThreads());
                LOOP_ASSERT(i, IDLE== X.maxIdleTime());

                if (0 != x.start()) {
                    bsl::cout << "Case 5: Thread start failure.  Skipping:\n";
                    T_ P_(MIN) P(MAX)
                    continue;
                }
                ASSERT(x.numWaitingThreads() == MIN);
                mutex.lock();
                for (int j=0; j < MIN; ++j) {
                    args.d_startSig = 0;
                    args.d_stopSig = 0;
                    x.enqueueJob(TestJobFunction1, &args);
                    while ( !args.d_startSig ) {
                        bsls::TimeInterval timeout = bdlt::CurrentTime::now();
                        timeout.addSeconds(TIMEOUT);
                        if (0 != startCond.timedWait(&mutex, timeout)) {
                            bsl::cout << "Case 5: Thread start failure.  "
                                      << "Skipping at: ";
                            P(j)
                            ASSERT(false);
                            args.d_stopSig++;
                            stopCond.broadcast();
                            mutex.unlock();
                            x.shutdown();
                            goto endtest1;
                        }
                    }
                }

                if (veryVeryVerbose) {
                    T_ P_(MIN) P_(MAX) P(x.numActiveThreads())
                    P(x.numWaitingThreads())
                }
                ASSERT(MIN==x.numActiveThreads());
                ASSERT(0 == x.numWaitingThreads());
                args.d_stopSig++;
                stopCond.broadcast();
                mutex.unlock();
                ASSERT(MIN==args.d_count);
            endtest1:
                ;
            }

            if (veryVerbose) cout << "\tTesting: max threads functionality\n"
                                  << "\t----------------------------------"
                                  << endl;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int MIN  = VALUES[i].d_minThreads;
                const int MAX  = VALUES[i].d_maxThreads;
                const int IDLE = VALUES[i].d_maxIdle;

                bslmt::Mutex mutex;
                bslmt::Condition startCond;
                bslmt::Condition stopCond;
                TestJobFunctionArgs args;

                args.d_mutex = &mutex;
                args.d_startCond = &startCond;
                args.d_stopCond = &stopCond;
                args.d_count = 0;

                bslmt::ThreadAttributes attr;
                Obj x(attr, MIN, MAX, IDLE);

                const Obj& X = x;

                if (veryVerbose) {
                    T_ P_(i) P(IDLE) T_ P_(MIN) P(MAX)
                }

                LOOP_ASSERT(i, MIN == X.minThreads());
                LOOP_ASSERT(i, MAX == X.maxThreads());
                LOOP_ASSERT(i, IDLE== X.maxIdleTime());

                if (0 != x.start()) {
                    bsl::cout << "Case 5: Thread start failure.  Skipping:\n";
                    T_ P_(MIN) P(MAX)
                    ASSERT(false);
                    continue;
                }

                ASSERT(x.numWaitingThreads() == MIN);

                mutex.lock();
                for (int j=0; j < MAX; j++) {
                    args.d_startSig = 0;
                    args.d_stopSig = 0;
                    x.enqueueJob(TestJobFunction1, &args);
                    while ( !args.d_startSig ) {
                        bsls::TimeInterval timeout = bdlt::CurrentTime::now();
                        timeout.addSeconds(TIMEOUT);
                        if (0 != startCond.timedWait(&mutex, timeout)) {
                            bsl::cout << "Case 5: Thread start failure.  "
                                         "Skipping at: ";
                            P(j)
                            ASSERT(false);
                            args.d_stopSig++;
                            stopCond.broadcast();
                            mutex.unlock();
                            x.shutdown();
                            goto endtest2;
                        }
                    }
                }

                if (veryVeryVerbose) {
                    T_ P_(MIN) P_(MAX) P(X.numActiveThreads())
                    T_ P_(X.numWaitingThreads()) P(X.numPendingJobs())
                    cout << "\n";
                }

                mutex.unlock();

                ASSERT(MAX==x.numActiveThreads());
                ASSERT(0 == x.numWaitingThreads());
                ASSERT(0 == x.numPendingJobs());
                ASSERT(MAX==args.d_count);

                x.enqueueJob(TestJobFunction2, &args);
                if (veryVeryVerbose) {
                    T_ P_(MIN) P_(MAX) P(x.numActiveThreads())
                    T_ P_(X.numWaitingThreads()) P(X.numPendingJobs())
                    cout << "\n";
                }

                ASSERT(MAX==x.numActiveThreads());
                ASSERT(0 == x.numWaitingThreads());
                ASSERT(1 == x.numPendingJobs());
                ASSERT(MAX==args.d_count);
                args.d_stopSig++;
                stopCond.broadcast();
            endtest2:
                ;
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'drain' and 'stop' and 'shutdown'
        //   Verify that the 'drain', 'stop' and 'shutdown' behave as expected.
        //
        // Plan:
        //   First, for a sequence of independent min/max test values, create
        //   a thread pool and verify that min threads are waiting.  Then spool
        //   min jobs and assert they all the jobs are actively being processed
        //   without the creation of any new threads.  Finally verify the after
        //   all the jobs have completed, min threads are again waiting.
        //
        //   Next, for a sequence of independent min/max test values, create a
        //   a thread pool and verify that min threads are waiting.  Then spool
        //   max jobs and assert that new threads were created to handle to
        //   additional jobs.  Next Create one more job and assert that no
        //   additional threads were created and one item remained in queue.
        //
        // Testing:
        //   int enqueueJob(ThreadPoolJobFunc , void *);
        //   void stop();
        //   void drain();
        //   void shutdown();
        // --------------------------------------------------------------------

        static const struct {
            int d_lineNum;
            int d_minThreads;
            int d_maxThreads;
            int d_maxIdle;
        } VALUES[] = {
            //line min threads max threads max idle
            //---- ----------- ----------- -----------
            { L_ ,         10,        50 ,     30*1000},
            { L_ ,         1 ,        1  ,      1*1000},
            { L_ ,         50,         75,     60*1000},
            { L_ ,         2 ,        22 ,     72*1000},
            { L_ ,         25,         80,     15*1000}

        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        if (verbose) cout << "Testing: 'drain', 'stop', and 'shutdown'\n"
                          << "=======================================" << endl;

        if (veryVerbose) cout << "\tTesting: 'drain'\n"
                              << "\t----------------" << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int MIN  = VALUES[i].d_minThreads;
            const int MAX  = VALUES[i].d_maxThreads;
            const int IDLE = VALUES[i].d_maxIdle;

            bslmt::Mutex mutex;
            bslmt::Condition startCond;
            bslmt::Condition stopCond;
            TestJobFunctionArgs args;

            args.d_mutex = &mutex;
            args.d_startCond = &startCond;
            args.d_stopCond = &stopCond;
            args.d_count = 0;

            bslmt::ThreadAttributes attr;
            Obj x(attr, MIN, MAX, IDLE);

            const Obj& X = x;

            if (veryVerbose) {
                T_ P_(i) P(IDLE) T_ P_(MIN) P(MAX)
            }

            LOOP_ASSERT(i, MIN == X.minThreads());
            LOOP_ASSERT(i, MAX == X.maxThreads());
            LOOP_ASSERT(i, IDLE== X.maxIdleTime());

            STARTPOOL(x);
            mutex.lock();
            ASSERT(MIN == x.numWaitingThreads());
            for (int j=0; j < MIN; j++) {
                args.d_startSig = 0;
                args.d_stopSig = 0;
                x.enqueueJob(TestJobFunction1, &args);
                while ( !args.d_startSig ) {
                    startCond.wait(&mutex);
                }
            }
            if (veryVeryVerbose) {
                T_ P_(x.numActiveThreads()) P(x.numWaitingThreads())
            }
            args.d_stopSig++;
            mutex.unlock();
            stopCond.broadcast();
            x.drain();
            if (veryVeryVerbose) {
                T_ P_(x.numActiveThreads()) P(x.numWaitingThreads())
            }
            ASSERT(0   == x.numActiveThreads());
            ASSERT(MIN == x.numWaitingThreads());
        }

        if (veryVerbose) cout << "\n\tTesting: 'stop'"
                              << "\n\t--------------" << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int MIN  = VALUES[i].d_minThreads;
            const int MAX  = VALUES[i].d_maxThreads;
            const int IDLE = VALUES[i].d_maxIdle;

            bslmt::Mutex mutex;
            bslmt::Condition startCond;
            bslmt::Condition stopCond;
            TestJobFunctionArgs args;

            args.d_mutex = &mutex;
            args.d_startCond = &startCond;
            args.d_stopCond = &stopCond;
            args.d_count = 0;

            bslmt::ThreadAttributes attr;
            Obj x(attr, MIN, MAX, IDLE);

            const Obj& X = x;

            if (veryVerbose) {
                T_ P_(i) P(IDLE) T_ P_(MIN) P(MAX)
            }

            LOOP_ASSERT(i, MIN == X.minThreads());
            LOOP_ASSERT(i, MAX == X.maxThreads());
            LOOP_ASSERT(i, IDLE== X.maxIdleTime());

            STARTPOOL(x);
            mutex.lock();
            ASSERT(x.numWaitingThreads() == MIN);
            for (int j=0; j < MAX; j++) {
                args.d_startSig = 0;
                args.d_stopSig = 0;
                x.enqueueJob(TestJobFunction1, &args);
                while ( !args.d_startSig ) {
                    startCond.wait(&mutex);
                }
            }
            args.d_stopSig++;
            mutex.unlock();
            stopCond.broadcast();
            x.stop();
            ASSERT(MAX == args.d_count);
            ASSERT(0 == x.numActiveThreads());
            ASSERT(0 == x.numWaitingThreads());
            if (veryVeryVerbose) {
                T_ P_(x.numActiveThreads()) P(x.numWaitingThreads())
            }

        }

        if (veryVerbose) cout << "\n\tTesting: 'shutdown'"
                              << "\n\t------------------"
                              << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int MIN  = VALUES[i].d_minThreads;
            const int MAX  = VALUES[i].d_maxThreads;
            const int IDLE = VALUES[i].d_maxIdle;

            bslmt::Mutex mutex;
            bslmt::Condition startCond;
            bslmt::Condition stopCond;
            TestJobFunctionArgs args;

            args.d_mutex = &mutex;
            args.d_startCond = &startCond;
            args.d_stopCond = &stopCond;
            args.d_count = 0;

            bslmt::ThreadAttributes attr;
            Obj x(attr, MIN, MAX, IDLE);

            const Obj& X = x;

            if (veryVerbose) {
                T_ P_(i) P(IDLE) T_ P_(MIN) P(MAX)
            }

            LOOP_ASSERT(i, MIN == X.minThreads());
            LOOP_ASSERT(i, MAX == X.maxThreads());
            LOOP_ASSERT(i, IDLE== X.maxIdleTime());

            STARTPOOL(x);
            mutex.lock();
            ASSERT(MIN == x.numWaitingThreads());
            for (int j=0; j < MAX; ++j) {
                args.d_startSig = 0;
                args.d_stopSig = 0;
                x.enqueueJob(TestJobFunction1,&args);
                while ( !args.d_startSig ) {
                    startCond.wait(&mutex);
                }
            }
            for ( int j=0; j<MAX; ++j ) {
                x.enqueueJob(TestJobFunction2, &args);
            }
            if (veryVeryVerbose) {
                T_ P(x.numPendingJobs())
            }

            ASSERT(MAX == x.numPendingJobs());
            args.d_stopSig++;
            stopCond.broadcast();
            mutex.unlock();
            x.shutdown();

            ASSERT(0 == x.numActiveThreads());
            ASSERT(0 == x.numWaitingThreads());
            if (veryVeryVerbose) {
                T_ P_(x.numActiveThreads()) P(x.numWaitingThreads())
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
        //   For each of a sequence of independent min/max threads and idle
        //   time values, construct a threadpool object and verify that the
        //   values are as expected.
        //
        // Testing:
        //   int minThreads() const;
        //   int maxThreads() const;
        //   int maxIdleTime() const;
        //   bdlmt::ThreadPool(const bslmt::Attributes&,int , int , int );
        //   ~bdlmt::ThreadPool();
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing: direct accessors\n"
                          << "========================" << endl;
        {
            static const struct {
                int d_lineNum;
                int d_minThreads;
                int d_maxThreads;
                int d_maxIdle;
            } VALUES[] = {
                //line min threads max threads max idle
                //---- ----------- ----------- -----------
                { L_ ,         10,        50 ,     30*1000},
                { L_ ,         1 ,        1  ,      1*1000},
                { L_ ,         50,        100,     60*1000},
                { L_ ,         2 ,        22 ,     72*1000},
                { L_ ,        100,        200,     15*1000}
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int MIN  = VALUES[i].d_minThreads;
                const int MAX  = VALUES[i].d_maxThreads;
                const int IDLE = VALUES[i].d_maxIdle;

                bslmt::ThreadAttributes attr;
                Obj x(attr, MIN, MAX, IDLE);

                const Obj& X = x;

                if (veryVerbose) {
                    T_ P_(i) P(IDLE) T_ P_(MIN) P(MAX)
                }

                LOOP_ASSERT(i, MIN  == X.minThreads());
                LOOP_ASSERT(i, MAX  == X.maxThreads());
                LOOP_ASSERT(i, IDLE == X.maxIdleTime());
                LOOP_ASSERT(i, 0    == X.threadFailures());
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   Create four test objects by using the default, initializing, and
        //   copy constructors.  Exercise the basic value-semantic methods and
        //   the equality operators using these test objects.  Invoke the
        //   primary manipulator [3, 6], copy constructor [2, 8], and
        //   assignment operator without [9, 10] and with [11] aliasing.  Use
        //   the direct accessors to verify the expected results.  Display
        //   object values frequently in verbose mode.  Note that 'VA', 'VB',
        //   and 'VC' denote unique, but otherwise arbitrary, object values,
        //   while '0' denotes the default object value.
        //
        // 1.   Create a default object x1          { x1:0        }
        // 2.   Create an object x2 (copy from x1)  { x1:0  x2:0  }
        // 3.   Set x1 to VA                        { x1:VA x2:0  }
        // 4.   Set x2 to VA                        { x1:VA x2:VA }
        // 5.   Set x2 to VB                        { x1:VA x2:VB }
        // 6.   Set x1 to 0                         { x1:0  x2:VB }
        // 7.   Create an object x3 (with value VC) { x1:0  x2:VB x3:VC }
        // 8.   Create an object x4 (copy from x1)  { x1:0  x2:VB x3:VC x4:0 }
        // 9.   Assign x2 = x1                      { x1:0  x2:0  x3:VC x4:0 }
        // 10.  Assign x2 = x3                      { x1:0  x2:VC x3:VC x4:0 }
        // 11.  Assign x1 = x1 (aliasing)           { x1:0  x2:VC x3:VC x4:0 }
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        bslmt::ThreadAttributes attr;
        const int MIN  = 10;
        const int MAX  = 50;
        const int IDLE = 1000;
        Obj x(attr,MIN,MAX,IDLE);

        ASSERT(MIN == x.minThreads());
        ASSERT(MAX == x.maxThreads());
        ASSERT(IDLE== x.maxIdleTime());

        STARTPOOL(x);
        ASSERT(x.numWaitingThreads() == MIN);
        x.drain();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING HELPER FUNCTIONS
        //   Verify that the support functions used to test 'bdlmt::ThreadPool'
        //   behave as expected.  The 'TestJobFunction1' function is expected
        //   to increment the supplied counter, signal a condition variable
        //   indicating that it has started, and wait for a stop condition to
        //   be signaled before exiting.  'TestJobFunction2' is expected to
        //   simply increment the supplied counter, signal a condition
        //   variable, indicating that it has started, and exit.
        //
        // Plan:
        //   First create NITERATIONS threads that call 'TestJobFunction1'.
        //   After each thread is started, verify that it increments the
        //   counter properly and that it waits for the stop condition before
        //   returning.  Then signal the stop the and verify that all the
        //   threads exit.
        //
        //   Next create NITERATIONS threads that call 'TestJobFunction2'.
        //   After each thread is started, verify that it increments the
        //   counter properly.  Finally verify that each thread exits properly.
        //
        // Testing:
        //   TestJobFunction1(TestJobFunctionArgs *d_args);
        //   TestJobFunction2(TestJobFunctionArgs *d_args);
        // --------------------------------------------------------------------

        if (verbose) cout << "HELPER FUNCTION TEST" << endl
                          << "====================" << endl;

        {
            const int NITERATIONS=50;

            if (veryVerbose) cout << "\tTesting: 'TestJobFunction1'" << endl
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
                           (bslmt::ThreadUtil::ThreadFunction)TestJobFunction1,
                           &args);
                while ( !args.d_startSig ) {
                    startCond.wait(&mutex);
                }
                LOOP_ASSERT(i, (i+1) == args.d_count);
            }
            args.d_stopSig++;
            stopCond.broadcast();
            mutex.unlock();
            ASSERT(NITERATIONS == args.d_count);

            for (int i=0; i<NITERATIONS;++i) {
                bslmt::ThreadUtil::join(threadHandles[i]);
            }
            ASSERT(NITERATIONS == args.d_count);
        }

        if (veryVerbose) cout << "\tTesting: 'TestJobFunction2'\n"
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
                           &threadHandles[i], attributes,
                           (bslmt::ThreadUtil::ThreadFunction)TestJobFunction2,
                           &args);
                while ( !args.d_startSig ) {
                    startCond.wait(&mutex);
                }
                LOOP_ASSERT(i, (i+1) == args.d_count);
                mutex.unlock();
            }
            ASSERT(NITERATIONS == args.d_count);

            for (int i=0; i<NITERATIONS;++i) {
                bslmt::ThreadUtil::join(threadHandles[i]);
            }
            ASSERT(NITERATIONS == args.d_count);
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING PERFORMANCE OF 'enqueueJob'
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
        //    PERFORMANCE OF 'enqueueJob'
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING PERFORMANCE OF 'enqueueJob'\n"
                          << "===================================" << endl ;

        if (verbose)
            cout << "\tUse the verbose parameter to control how long\n"
               << "\t  the jobs should take (10=fast, 100=medium, 500=slow)\n"
               << "\tUsing value " << verbose << endl;

        const int MAX_NTHREADS = 10;
        const int SPEED = verbose ? verbose : 100;
        for (int i = (SPEED < 25 ? 1 : 2); i < MAX_NTHREADS; ++i)
        {
            cout << "\tUsing " << i << " threads." << endl;
            bslmt::ThreadAttributes attr;
            const int NTHREADS = i;
            const int IDLE_TIME = 10000;

            Obj localX(attr, NTHREADS, NTHREADS, IDLE_TIME);
            const int ITERATIONS = 1000 * 1000;
            double queueSize(0); // to avoid overflows and casts

            xP = &localX;
            STARTPOOL(localX);
            const Obj::Job job(bdlf::BindUtil::bind(&testTimingJobFunction,
                                                    verbose ? verbose : 100));

            bsls::Stopwatch timer;  timer.start();
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
      case -2: {
        // --------------------------------------------------------------------
        // TEST ENQUEUEJOB FAILS APPROPRIATELY
        //
        // Concern:
        //   That 'equeueJob' returns non-zero when it is unable to spawn the
        //   first thread.
        //
        // Plan:
        //   Before running the test, say
        //
        //   $ ulimit -v 50000    # note that 'ulimit -v' doesn't work on Aix
        //
        //   Limiting total memory of the task to about 50 meg.  Then run this
        //   test, which will try to spawn a 500 Mbyte thread, which will fail,
        //   which should cause 'enqueueJob' to return a non-zero value.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'enqueueJob' fails\n"
                               "==========================\n";

        bslmt::ThreadAttributes attr;
        attr.setStackSize(500 << 20);    // 500 MByte thread stack size

        bdlmt::ThreadPool tp(attr, 0, 10, 1000);

        int rc = tp.start();
        ASSERT(0 == rc);

        rc = tp.enqueueJob(MinusTwoJob());
        ASSERT(0 != rc);

        tp.shutdown();
      } break;
      default: {
          testStatus = -1;
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
