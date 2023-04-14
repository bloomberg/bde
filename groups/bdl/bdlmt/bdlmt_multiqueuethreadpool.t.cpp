// bdlmt_multiqueuethreadpool.t.cpp                                   -*-C++-*-

#include <bdlmt_multiqueuethreadpool.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bslmt_barrier.h>
#include <bslmt_latch.h>
#include <bslmt_mutex.h>
#include <bslmt_semaphore.h>
#include <bslmt_testutil.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>
#include <bsls_systemtime.h>

#include <bsls_timeinterval.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_rawdeleterproctor.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_systemtime.h>
#include <bsls_platform.h>
#include <bsls_timeutil.h>  // For CachePerformance
#include <bsls_types.h>     // For 'BloombergLP::bsls::Types::Int64'

#include <bdlf_bind.h>

#include <bsl_algorithm.h>
#include <bsl_climits.h>
#include <bsl_fstream.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_iterator.h>
#include <bsl_limits.h>
#include <bsl_map.h>
#include <bsl_memory.h>
#include <bsl_set.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>
#include <bsl_cmath.h>   // 'sqrt'
#include <bsl_cstdlib.h>

using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::ends;
using bsl::flush;

using namespace BloombergLP;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] bdlmt::MultiQueueThreadPool(
//                         const bslmt::ThreadAttributes&  threadAttributes,
//                         int                             minThreads,
//                         int                             maxThreads,
//                         int                             maxIdleTime,
//                         bslma::Allocator               *basicAllocator = 0);
// [ 7] bdlmt::MultiQueueThreadPool(bdlmt::ThreadPool *threadPool,
//                                  bslma::Allocator  *basicAllocator = 0);
// [ 2] ~bdlmt::MultiQueueThreadPool();
//
// MANIPULATORS
// [33] void setBatchSize(int id, int batchSize);
// [ 2] int createQueue();
// [ 2] int deleteQueue(int id, const bsl::function<void()>& cleanupFunc);
// [ 2] int enqueueJob(int id, const bsl::function<void()>& functor);
// [ 6] int enableQueue(int id);
// [ 6] int disableQueue(int id);
// [ 2] void start();
// [ 2] void stop();
// [ 2] void shutdown();
// [13] void numProcessedReset(int *, int *, int * = 0);
//
// ACCESSORS
// [33] int batchSize(int id) const;
// [13] void numProcessed(int *, int *, int * = 0) const;
// [ 4] int numQueues() const;
// [13] int numElements() const;
// [ 4] int numElements(int id) const;
// [ 6] bool isEnabled(int id);
// [ 2] const bdlmt::ThreadPool& threadPool() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] PRIMARY TEST APPARATUS
// [ 5] OUTPUT (<<) OPERATOR
// [ 8] CONCERN: 'deleteQueueCb' is the last callback processed
// [ 9] CONCERN: Multi-threaded safety of public API and processing
// [10] CONCERN: One 'bdlmt::ThreadPool' can be shared by two MQTPs
// [11] CONCERN: 'deleteQueue' blocks the caller
// [12] CONCERN: Cleanup callback does not deadlock
// [20] DRQS 99979290
// [21] DRQS 104502699
// [22] PAUSE/DELETE INTERACTION
// [23] DRQS 107865762: more than one task per queue to thread pool
// [24] DRQS 107733386: pause queue can be indefinitely deferred
// [25] DRQS 112259433: 'drain' and 'deleteQueue' can deadlock
// [26] DRQS 113734461: 'deleteQueue' copies cleanupFunctor
// [27] DRQS 118269630: 'drain' may not drain underlying threadpool
// [28] DRQS 139148629: deadlock when job pauses another queue
// [29] DRQS 138890062: 'deleteQueueCb' running after destructor
// [30] DRQS 140150365: resume fails immediately after pause
// [31] DRQS 140403279: pause can deadlock with delete and create
// [32] DRQS 143578129: 'numElements' stress test
// [-2] PERFORMANCE TEST
// ----------------------------------------------------------------------------

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

#define ASSERT                   BSLMT_TESTUTIL_ASSERT
#define ASSERTV                  BSLMT_TESTUTIL_ASSERTV

#define GUARD                    BSLMT_TESTUTIL_GUARD

#define Q                        BSLMT_TESTUTIL_Q
#define P                        BSLMT_TESTUTIL_P
#define P_                       BSLMT_TESTUTIL_P_
#define T_                       BSLMT_TESTUTIL_T_
#define L_                       BSLMT_TESTUTIL_L_

#define GUARDED_STREAM(STREAM)   BSLMT_TESTUTIL_GUARDED_STREAM(STREAM)
#define COUT                     BSLMT_TESTUTIL_COUT
#define CERR                     BSLMT_TESTUTIL_CERR

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//            GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
// ----------------------------------------------------------------------------
typedef bdlmt::MultiQueueThreadPool Obj;

typedef bsl::function<void()> Func;

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

static bslmt::Mutex coutMutex;

// windows tends to sleep slightly less time than requested
#ifdef BSLS_PLATFORM_OS_WINDOWS
    static const double jumpTheGun = 0.0002;
#elif defined(BSLS_PLATFORM_OS_DARWIN)
// and mac os appears to be even more aggressive
    static const double jumpTheGun = 0.001;
#else
    static const double jumpTheGun = 0.0;
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_AIX)
// On Windows, the thread name will only be set if we're running on Windows
// 10, version 1607 or later, otherwise it will be empty. AIX does not
// support thread naming.
static const bool k_threadNameCanBeEmpty = true;
#else
static const bool k_threadNameCanBeEmpty = false;
#endif

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------
void makeFunc(Func *f, void (*fptr)()) {
    *f = fptr;
}

template <class A1>
void makeFunc(Func *f, void (*fptr)(A1), A1 a1)
{
    *f = bdlf::BindUtil::bind(fptr, a1);
}

template <class A1, class A2>
void makeFunc(Func *f, void (*fptr)(A1, A2), A1 a1, A2 a2)
{
    *f = bdlf::BindUtil::bind(fptr, a1, a2);
}

template <class A1, class A2, class A3>
void makeFunc(Func *f, void (*fptr)(A1, A2, A3), A1 a1, A2 a2, A3 a3)
{
    *f = bdlf::BindUtil::bind(fptr, a1, a2, a3);
}

template <class A1, class A2, class A3, class A4>
void makeFunc(Func  *f,
              void (*fptr)(A1, A2, A3, A4),
              A1     a1,
              A2     a2,
              A3     a3,
              A4     a4)
{
    *f = bdlf::BindUtil::bind(fptr, a1, a2, a3, a4);
}

void noop() {
}

void checkThreadName()
    // Check that the name of the thread is one of the acceptable values.    
{
    bsl::string threadName;
    bslmt::ThreadUtil::getThreadName(&threadName);
    ASSERTV(threadName,
            (k_threadNameCanBeEmpty && threadName.empty()) ||
                threadName == "bdl.MultiQuePl" || threadName == "OtherName");
}

static
void incrementCounter(bsls::AtomicInt *counter)
    // Increment the value at the address specified by 'counter'.
{
    ASSERT(counter);
    ++*counter;
}

static void timedWaitOnBarrier(bslmt::Barrier  *barrier,
                               bsls::AtomicInt *timedOut)
    // Timed wait on the specified 'barrier' for 0.1 seconds and load into the
    // specified 'timedOut' a non-zero value if the 'timedWait' timeouts.
{
    ASSERT(barrier);
    ASSERT(timedOut);

    *timedOut = barrier->timedWait(
                    bsls::SystemTime::nowRealtimeClock().addMilliseconds(100));
}

static void waitTwiceAndIncrement(bslmt::Barrier  *barrier,
                                  bsls::AtomicInt *counter,
                                  int              incrementBy)
{
    // Wait two times on the specified 'barrier' and increment the specified
    // 'counter' by the specified 'incrementBy' value.
    barrier->wait();
    barrier->wait();
    counter->add(incrementBy);
}

static void resumeAndIncrement(Obj             *pool,
                               int              queueId,
                               bsls::AtomicInt *counter)
{
    // Resume the queue with the specified 'queueId' in the specified 'pool'.
    // On success, increment the specified 'counter'.

    if (0 == pool->resumeQueue(queueId)) {
        (*counter)++;
    }
}

static void waitPauseAndIncrement(bslmt::Barrier  *barrier,
                                  Obj             *pool,
                                  int              queueId,
                                  bsls::AtomicInt *counter)
{
    // Wait on the specified 'barrier', pause the queue with the specified
    // 'queueId' in the specified 'pool' and then increment the specified
    // 'counter'.  Finally, wait on the barrier again.

    barrier->wait();
    int rc = pool->pauseQueue(queueId);
    ASSERT(0 == rc);
    (*counter)++;
    barrier->wait();
}

static void waitThenAppend(bslmt::Semaphore *semaphore,
                           bsl::string      *value,
                           char              letter)
{
    // Wait on the specified 'semaphore', then append the specified 'letter' to
    // the specified 'value'.
    semaphore->wait();
    value->push_back(letter);
}

static void waitPauseWait(bslmt::Barrier *barrier, Obj *pool, int queueId)
{
    // Wait on the specified 'barrier', resume the queue with the specified
    // 'queueId' in the specified 'pool', and then wait on the 'barrier'.

    barrier->wait();
    int rc = pool->pauseQueue(queueId);
    ASSERT(0 == rc);
    barrier->wait();
}

static void waitWait(bslmt::Barrier *barrier)
{
    // Wait on the specified 'barrier' twice.

    barrier->wait();
    barrier->wait();
}

static void waitWaitCreate(bslmt::Barrier *barrier, Obj *pool)
{
    // Wait on the specified 'barrier' twice, then create a queue in the
    // specified 'pool'.

    barrier->wait();
    barrier->wait();
    pool->createQueue();
}

static void waitWaitDelete(bslmt::Barrier *barrier, Obj *pool, int queueId)
{
    // Wait on the specified 'barrier' twice, then delete the queue with the
    // specified 'queueId' in the specified 'pool'.

    barrier->wait();
    barrier->wait();
    int rc = pool->deleteQueue(queueId);
    ASSERT(0 == rc);
}

static void addAppendJobAtFront(Obj         *pool,
                                int          queue,
                                bsl::string *value,
                                char         letter)
{
    pool->addJobAtFront(queue,
                        bdlf::BindUtil::bind(&bsl::string::push_back,
                                             value, letter));
}

static void waitOnBarrier(bslmt::Barrier *barrier, int numIterations)
{
    // Wait on the specified 'barrier' for 'numIterations' iterations.

    ASSERT(barrier);

    do {
        barrier->wait();
    } while (--numIterations);
}

struct Sleeper {
    int                    d_sleepMicroSeconds;
    static bsls::AtomicInt s_finished;

    Sleeper(double sleepSeconds)
    {
        d_sleepMicroSeconds = static_cast<int>(sleepSeconds * 1e6 + .5);
    }

    void operator()() {
        bslmt::ThreadUtil::microSleep(d_sleepMicroSeconds);
        ++s_finished;
    }
};
bsls::AtomicInt Sleeper::s_finished;

struct Reproducer {
    Obj                    *d_threadPool;
    const bsl::vector<int> *d_handles;
    int                     d_handleIdx;
    int                     d_handleIdxIncrement;
    static bsls::AtomicInt  s_counter;        // submit until counter == 0

    Reproducer(Obj                    *threadPool,
               const bsl::vector<int> *handles,
               int                     handleIdx,
               int                     handleIdxIncrement)
    : d_threadPool(threadPool)
    , d_handles(handles)
    , d_handleIdx(handleIdx)
    , d_handleIdxIncrement(handleIdxIncrement) {}

    void operator()()
    {
        enum { k_SLEEP_HARDLY_TIME = 10 * 1000 };         // 0.01 sec

        bslmt::ThreadUtil::microSleep(k_SLEEP_HARDLY_TIME);
        d_handleIdx += d_handleIdxIncrement;
        d_handleIdx %= static_cast<int>(d_handles->size());
        if (s_counter > 0) {
            --s_counter;
            d_threadPool->enqueueJob((*d_handles)[d_handleIdx], *this);
        }
    }
};
bsls::AtomicInt Reproducer::s_counter;

class UniqueGuard {
    static bsls::AtomicInt s_count;

  public:
    UniqueGuard() {  ASSERT(0 == s_count++);  }
    ~UniqueGuard() {  ASSERT(0 == --s_count);  }
};

bsls::AtomicInt UniqueGuard::s_count(0);

static int s_daisyChainCount = 0;

void daisyChain(Obj *pObj, int id)
{
    if (++s_daisyChainCount < 100) {
        bslmt::ThreadUtil::microSleep(100000);
        pObj->addJobAtFront(id, bdlf::BindUtil::bind(&daisyChain, pObj, id));
    }
}

void startDaisyChain(Obj *pObj, int id, bslmt::Latch *waitLatch)
{
    waitLatch->arrive();
    bslmt::ThreadUtil::microSleep(100000);
    pObj->addJobAtFront(id, bdlf::BindUtil::bind(&daisyChain, pObj, id));
}

void pauseQueueWithUniqueGuard(Obj          *pObj,
                               int           id,
                               bslmt::Latch *pauseLatch,
                               bslmt::Latch *doneLatch)
{
    UniqueGuard guard;
    pObj->pauseQueue(id);
    pauseLatch->arrive();
    doneLatch->wait();
}

void noopWithUniqueGuard()
{
    UniqueGuard guard;
}

void deferredDeleteQueue(Obj *pObj, int id, bslmt::Latch *waitLatch)
{
    waitLatch->wait();
    bslmt::ThreadUtil::microSleep(100000);
    pObj->deleteQueue(id);
}

double now() {
    return bsls::SystemTime::nowRealtimeClock().totalSecondsAsDouble();
}

// ============================================================================
//      CASE-SPECIFIC TYPES, HELPER FUNCTIONS, AND CLASSES FOR TESTING
// ----------------------------------------------------------------------------
static
void case9Callback(bsls::AtomicInt *counter, bsl::vector<int> *results)
{
    ASSERT(counter);
    ASSERT(results);

    results->push_back(++*counter);
}

static
void case11CleanUp(bsls::AtomicInt *counter, bslmt::Barrier *barrier) {
    BSLS_ASSERT(barrier);
    BSLS_ASSERT(counter);
    barrier->wait();
    *counter = 1;
    barrier->wait();
}

static
void case12EnqueueJob(bdlmt::MultiQueueThreadPool *mqtp,
                      int                          id,
                      const Func&                  job,
                      bslmt::Barrier              *barrier)
{
    // Enqueue the specified 'job' to the queue with the specified 'id' managed
    // by the 'bdlmt::MultiQueueThreadPool' pointed to by 'mqtp'.

    ASSERT(mqtp);
    ASSERT(barrier);

    if (veryVerbose) {
        cout << "case12EnqueueJob: Enqueuing into queue # " << id << endl;
    }
    ASSERT(0 == mqtp->enqueueJob(id, job));

    barrier->wait();
}

static
void case12DeleteQueue(bdlmt::MultiQueueThreadPool *mqtp,
                       int                          id,
                       const Func&                  cleanupCb,
                       bslmt::Barrier              *barrier)
{
    // Delete the queue identified by 'id' from the
    // 'bdlmt::MultiQueueThreadPool' pointed to by 'mqtp' with the specified
    // 'cleanupCb' callback.

    ASSERT(mqtp);
    ASSERT(barrier);
    if (veryVerbose) {
        cout << "case12DeleteQueue: Deleting queue # " << id << endl;
    }
    ASSERT(0 == mqtp->deleteQueue(id, cleanupCb));
    barrier->wait();
}

void case26CleanupFunctor(bool *called)
    // Set the specified 'called' to 'true'.  Note that this function in
    // intended to be bound as a cleanup functor for 'deleteQueue'
{
    if (veryVerbose) {
        bsl::cout << "Case26 Cleanup Functor" << bsl::endl;
    }
    *called = true;
}

void case26WaitJob(bslmt::Latch *latch)
    // Call 'wait' on the specified 'latch'.
{
    latch->wait();
}

void case26DeleteQueue(Obj *queue, int queueId, bool *cleanupDone)
    // Call 'deleteQueue' on the specified 'queue', using the specified
    // 'queueId', and supply a cleanup functor that will set 'cleanupDone' to
    // 'true', and will be *destroyed* when this function exits (i.e., the
    // functor will be called after the temporary functor in this function call
    // is destroyed).  Note that this is factored into a separate function to
    // ensure the temporary functor is destroyed.
{
    queue->deleteQueue(
        queueId, bdlf::BindUtil::bind(&case26CleanupFunctor, cleanupDone));
}

class Case27DrainThread {
    Obj *d_obj_p;

  public:
    Case27DrainThread(Obj *obj)
    : d_obj_p(obj)
    {
        BSLS_ASSERT(obj);
    }

    void operator()() {
        for (int i = 0; i < 10000; ++i) {
            d_obj_p->drain();
            bslmt::ThreadUtil::microSleep(10);
        }
    }
};

static bsls::AtomicInt s_case27Count(0);

void case27Counter()
{
    --s_case27Count;
}

static bsls::AtomicInt s_case28Count(0);

Obj *s_case28Obj_p = 0;

void case28Job()
{
    int queueId = s_case28Obj_p->createQueue();

    ASSERT(0 == s_case28Obj_p->enqueueJob(queueId, noop));

    s_case28Obj_p->pauseQueue(queueId);

    ++s_case28Count;
}

static bslmt::Barrier *s_case29Barrier_p;

void case29Job()
{
    s_case29Barrier_p->wait();

    // If the destructor waits for the 'deleteQueueCb' to complete, the
    // following will time-out.

    ASSERT(0 != s_case29Barrier_p->timedWait(
                   bsls::SystemTime::nowRealtimeClock().addMilliseconds(100)));
}

void case32Job()
{
    bslmt::ThreadUtil::microSleep(10000);
}

// ============================================================================
//          CLASSES AND HELPER FUNCTIONS FOR TESTING USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Word Search Application
/// - - - - - - - - - - - - - - - - - -
// This example illustrates the use of a 'bdlmt::MultiQueueThreadPool' in a
// word search application called 'fastSearch'.  'fastSearch' searches a list
// of files for a list of words, and returns the set of files which contain all
// of the specified words.  'bdlmt::MultiQueueThreadPool' is used to provide
// concurrent processing of files, and to simplify the collection of results by
// serializing access to result sets which are maintained for each word.
//
// First, we present a class used to manage a word, and the set of files which
// contain that word:
//..
    class my_SearchProfile {
        // This class defines a search profile consisting of a word and a set
        // of files (given by name) that contain the word.  Here, "word" is
        // defined as any string of characters.

        bsl::string           d_word;     // word to search for
        bsl::set<bsl::string> d_fileSet;  // set of matching files

      private:
        // not implemented
        my_SearchProfile(const my_SearchProfile&);
        my_SearchProfile& operator=(const my_SearchProfile&);

      public:
        // CREATORS
        my_SearchProfile(const char       *word,
                         bslma::Allocator *basicAllocator = 0);
            // Create a 'my_SearchProfile' with the specified 'word'.
            // Optionally specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the default memory allocator is used.

        ~my_SearchProfile();
            // Destroy this search profile.

        // MANIPULATORS
        void insert(const char *file);
            // Insert the specified 'file' into the file set maintained by this
            // search profile.

        // ACCESSORS
        bool isMatch(const char *file) const;
            // Return 'true' if the specified 'file' matches this search
            // profile.

        const bsl::set<bsl::string>& fileSet() const;
            // Return a reference to the non-modifiable file set maintained by
            // this search profile.

        const bsl::string& word() const;
            // Return a reference to the non-modifiable word maintained by this
            // search profile.
    };
//..
// And the implementation:
//..
    // CREATORS
    my_SearchProfile::my_SearchProfile(const char       *word,
                                       bslma::Allocator *basicAllocator)
    : d_word(basicAllocator)
    , d_fileSet(bsl::less<bsl::string>(), basicAllocator)
    {
        ASSERT(word);

        d_word.assign(word);
    }

    my_SearchProfile::~my_SearchProfile()
    {
    }

    // MANIPULATORS
    inline
    void my_SearchProfile::insert(const char *file)
    {
        ASSERT(file);

        d_fileSet.insert(file);
    }

    // ACCESSORS
    bool my_SearchProfile::isMatch(const char *file) const
    {
        ASSERT(file);

        bool          found = false;
        bsl::ifstream ifs(file);
        bsl::string   line;
        while (bsl::getline(ifs, line)) {
            if (bsl::string::npos != line.find(d_word)) {
                found = true;
                break;
            }
        }
        ifs.close();
        return found;
    }

    inline
    const bsl::set<bsl::string>& my_SearchProfile::fileSet() const
    {
        return d_fileSet;
    }

    inline
    const bsl::string& my_SearchProfile::word() const
    {
        return d_word;
    }
//..
// Next, we define a helper function to perform a search of a word in a
// particular file.  The function is parameterized by a search profile and a
// file name.  If the specified file name matches the profile, it is inserted
// into the profile's file list.
//..
    void my_SearchCb(my_SearchProfile* profile, const char *file)
    {
        // Insert the specified 'file' to the file set of the specified search
        // 'profile' if 'file' matches the 'profile'.

        ASSERT(profile);
        ASSERT(file);

        if (profile->isMatch(file)) {
            profile->insert(file);
        }
    }
//..
// Lastly, we present the front end to the search application: 'fastSearch'.
// 'fastSearch' is parameterized by a list of words to search for, a list of
// files to search in, and a set which is populated with the search results.
// 'fastSearch' instantiates a 'bdlmt::MultiQueueThreadPool', and creates a
// queue for each word.  It then associates each queue with a search profile
// based on a word in the word list.  Then, it enqueues a job to each queue for
// each file in the file list that tries to match the file to each search
// profile.  Lastly, 'fastSearch' collects the results, which is the set
// intersection of each file set maintained by the individual search profiles.
//..
    void fastSearch(const bsl::vector<bsl::string>&  wordList,
                    const bsl::vector<bsl::string>&  fileList,
                    bsl::set<bsl::string>&           resultSet,
                    int                              repetitions = 1,
                    bslma::Allocator                *basicAllocator = 0)
    {
        // Return the set of files, specified by 'fileList', containing every
        // word in the specified 'wordList', in the specified 'resultSet'.
        // Optionally specify 'repetitions', the number of repetitions to run
        // the search jobs (it is used to increase the load for performance
        // testing).  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the default memory allocator is
        // used.

        typedef bsl::vector<bsl::string> ListType;
            // This type is defined for notational convenience when iterating
            // over 'wordList' or 'fileList'.

        typedef bsl::pair<int, my_SearchProfile*> RegistryValue;
            // This type is defined for notational convenience.  The first
            // parameter specifies a queue ID.  The second parameter specifies
            // an associated search profile.

        typedef bsl::map<bsl::string, RegistryValue> RegistryType;
            // This type is defined for notational convenience.  The first
            // parameter specifies a word.  The second parameter specifies a
            // tuple containing a queue ID, and an associated search profile
            // containing the specified word.

        enum {
            // thread pool configuration
            k_MIN_THREADS = 4,
            k_MAX_THREADS = 20,
            k_MAX_IDLE    = 100  // use a very short idle time since new jobs
                                 // arrive only at startup
        };
        bslmt::ThreadAttributes     defaultAttrs;
        bdlmt::MultiQueueThreadPool pool(defaultAttrs,
                                         k_MIN_THREADS,
                                         k_MAX_THREADS,
                                         k_MAX_IDLE,
                                         basicAllocator);
        RegistryType profileRegistry(bsl::less<bsl::string>(), basicAllocator);

        // Create a queue and a search profile associated with each word in
        // 'wordList'.

        for (ListType::const_iterator it = wordList.begin();
             it != wordList.end();
             ++it) {
            bslma::Allocator *allocator =
                                     bslma::Default::allocator(basicAllocator);

            const bsl::string& word = *it;
            int                id = pool.createQueue();
            ASSERTV(word, 0 != id);
            my_SearchProfile *profile = new (*allocator)
                                                 my_SearchProfile(word.c_str(),
                                                                  allocator);

            bslma::RawDeleterProctor<my_SearchProfile, bslma::Allocator>
                                                   deleter(profile, allocator);

            profileRegistry[word] = bsl::make_pair(id, profile);
            deleter.release();
        }

        // Start the pool, enabling enqueuing and queue processing.
        pool.start();

        // Enqueue a job which tries to match each file in 'fileList' with each
        // search profile.

        for (ListType::const_iterator it = fileList.begin();
             it != fileList.end();
             ++it) {
            for (ListType::const_iterator jt = wordList.begin();
                 jt != wordList.end();
                 ++jt) {
                const bsl::string& file = *it;
                const bsl::string& word = *jt;
                RegistryValue&     rv   = profileRegistry[word];
                Func               job;
                makeFunc(&job, my_SearchCb, rv.second, file.c_str());
                for (int i = 0; i < repetitions; ++i) {
                    int rc = pool.enqueueJob(rv.first, job);
                    ASSERTV(word, 0 == rc);
                }
            }
        }

        // Stop the pool, and wait while enqueued jobs are processed.
        pool.stop();

        // Construct the 'resultSet' as the intersection of file sets collected
        // in each search profile.

        resultSet.insert(fileList.begin(), fileList.end());
        for (RegistryType::iterator it = profileRegistry.begin();
             it != profileRegistry.end();
             ++it) {
            my_SearchProfile *profile = it->second.second;
            const bsl::set<bsl::string>& fileSet = profile->fileSet();
            bsl::set<bsl::string> tmpSet;
            bsl::set_intersection(fileSet.begin(),
                                  fileSet.end(),
                                  resultSet.begin(),
                                  resultSet.end(),
                                  bsl::inserter(tmpSet, tmpSet.begin()));
            resultSet = tmpSet;
            bslma::Default::allocator(basicAllocator)->deleteObjectRaw(
                                                                      profile);
        }
    }
//..

struct StressJob {
    int        d_x;
    int        d_y;
    static int s_count;
    bslma::Allocator *d_allocator_p;    // held, not owned

    void operator()() {
        int prod = d_x ^ d_y;
        ASSERT((prod ^ d_x) == d_y);
        ASSERT((prod ^ d_y) == d_x);

        int allocSize = bsl::rand() & (1024 - 1);
        if (0 == allocSize) {
            allocSize = 100;
        }
        void *p = d_allocator_p->allocate(allocSize);
        d_allocator_p->deallocate(p);

        ++s_count;
    }
};
int StressJob::s_count = 0;

namespace mqpoolperf {

class MQPoolPerformance {
    // This class performs the various performance tests

  public:
    typedef bsls::Types::Int64              TimeType;
    typedef bsl::vector<bsls::Types::Int64> VecTimeType;
    typedef bsl::vector<int> VecIntType;
    typedef int (*InitFunc)(MQPoolPerformance*, VecIntType&);
        // Initialization function takes a vector of ints, and returns 0 for
        // success, <0 for failure.
    typedef int (*RunFunc)(MQPoolPerformance*, VecIntType&);
        // Run function takes a pointer to MQPoolPerformance, and a vector of
        // ints.  It returns a return code of 0 for success, <0 for failure.
        // The last entry in the vector of ints is the thread number.  Writer
        // threads are first, writers after.

    typedef struct WorkData {
        RunFunc            d_func;
        MQPoolPerformance *d_poolperf_p;
        VecIntType         d_data;
    } WorkData;

  private:
    // DATA
    bslma::Allocator *d_allocator_p;  // memory allocator

    bsl::string d_title;  // Title for printing purposes

    int d_numRThreads;    // # of reader threads
    int d_numWThreads;    // # of writer threads
    int d_numThreads;     // d_numRThreads + d_numWThreads
    int d_numCalcs;       // # of calculations, say 1,000,000
    int d_numRepeats;     // # of repetitions of the calculation, say 10.
                          // Repetitions are used to find variance

    VecTimeType d_vecWTime;
    VecTimeType d_vecUTime;
    VecTimeType d_vecSTime;
        // Wall time, User time and system time in nanos, collected from the
        // various threads, and the various repeats.  If we have 6 threads, and
        // 100 repeats, we will have 60 entries.

    TimeType d_avgWTime;
    TimeType d_avgUTime;
    TimeType d_avgSTime;
        // Averages of Wall time, User time and system time in nanos,
        // calculated from d_vecWTime, d_vecUTime, d_vecCTime respectively.
    TimeType d_seWTime;
    TimeType d_seUTime;
    TimeType d_seSTime;
        // Standard errors of Wall time, User time and system time in nanos,
        // calculated from d_vecWTime, d_vecUTime, d_vecCTime respectively.  It
        // is set to 0 if d_numRepeat = 1.
    VecTimeType runTest(VecIntType& args, RunFunc func);
        // run a single repetition of the calculation.  The calculation is
        // defined in the specified 'func' function, and takes as input the
        // specified 'args' vector.  Return for each thread a triad of elapsed
        // wall time, user time, system time.

    // NOT IMPLEMENTED
    MQPoolPerformance(const MQPoolPerformance&);
    MQPoolPerformance& operator=(const MQPoolPerformance&);

  public:
    MQPoolPerformance(const char       *title,
                      int               numRThreads,
                      int               numWThreads,
                      int               numCalcs,
                      int               numRepeats,
                      bslma::Allocator *basicAllocator = 0);

    int initialize(VecIntType& args, InitFunc func);
        // run the initialization function.  The initialization is defined in
        // the specified 'func' function, and takes as input the specified
        // 'args' vector.  Returns the return code from 'func'.
    VecTimeType runTests(VecIntType& args, RunFunc func);
        // run the tests by running 'runTest' numRepeats times.  The test being
        // run is the defined in the specified 'func' function, and takes as
        // input the specified 'args' vector.  Return a triad of elapsed wall
        // time, user time, and system time.
    void printResult();
        // print the output from the calculation

    // ACCESSORS
    bslma::Allocator *allocator() const;
        //  Return the allocator used by this object.

    int numCalcs() const;
        // Return the number of calculations

    int numRepeats() const;
        // Return the number of test repetitions

    int numRThreads() const;
        // Return the number of reader threads

    int numWThreads() const;
        // Return the number of writer threads

    int numThreads() const;
        // Return the total number of threads.  It is equal to numRThreads() +
        // numWThreads().

    const char* title() const;
        // Return the test title.

    // TEST FUNCTIONS
    static int testFastSearch(MQPoolPerformance *poolperf_p, VecIntType& args);
        // Run the fastSearch test functionInsert with the specified
        // 'poolperf_p'.  The specified 'args' vector is empty.

};  // END class MQPoolPerformance

MQPoolPerformance::MQPoolPerformance(
                              const char                       *title,
                              int                               numRThreads,
                              int                               numWThreads,
                              int                               numCalcs,
                              int                               numRepeats,
                              bslma::Allocator                 *basicAllocator)
: d_allocator_p(basicAllocator)
, d_title(title, basicAllocator)
, d_numRThreads(numRThreads)
, d_numWThreads(numWThreads)
, d_numThreads(numRThreads + numWThreads)
, d_numCalcs(numCalcs)
, d_numRepeats(numRepeats)
, d_vecWTime(basicAllocator)
, d_vecUTime(basicAllocator)
, d_vecSTime(basicAllocator)
{
}

int MQPoolPerformance::initialize(VecIntType&                 args,
                                  MQPoolPerformance::InitFunc func)
{
    // Call initializing function
    int rc = func(this, args);
    return rc;
}

MQPoolPerformance::VecTimeType MQPoolPerformance::runTests(
                                               VecIntType&                args,
                                               MQPoolPerformance::RunFunc func)
{
    d_seWTime = d_seUTime = d_seSTime = 0;
    d_avgWTime = d_avgUTime = d_avgSTime = 0;
//bsl::cout << "NR=" << d_numRepeats << ",NC=" << d_numCalcs << "\n";
    for (int j = 0; j < d_numRepeats; ++j) {
        VecTimeType times(d_allocator_p);
        times = runTest(args, func);
        TimeType curWTime = 0, curUTime = 0, curSTime = 0;
        for (int i = 0; i < d_numThreads; ++i) {
            d_vecWTime.push_back(times[i * 3    ]);
            d_vecUTime.push_back(times[i * 3 + 1]);
            d_vecSTime.push_back(times[i * 3 + 2]);
            curWTime += times[i * 3    ];
            curUTime += times[i * 3 + 1];
            curSTime += times[i * 3 + 2];
        }
        d_avgWTime += curWTime;
        d_avgUTime += curUTime;
        d_avgSTime += curSTime;
        d_seWTime += curWTime * curWTime;
        d_seUTime += curUTime * curUTime;
        d_seSTime += curSTime * curSTime;
    }
    d_avgWTime /= d_numRepeats;
    d_avgUTime /= d_numRepeats;
    d_avgSTime /= d_numRepeats;
    if (d_numRepeats == 1) {
        d_seWTime = d_seUTime = d_seSTime = 0;
    }
    else {
        // SD = SQRT(E(X^2) - E(X)^2)
        d_seWTime = static_cast<TimeType>(
                    bsl::sqrt(static_cast<double>(  d_seWTime / d_numRepeats
                                                  - d_avgWTime * d_avgWTime)));
        d_seUTime = static_cast<TimeType>(
                    bsl::sqrt(static_cast<double>(  d_seUTime / d_numRepeats
                                                  - d_avgUTime * d_avgUTime)));
        d_seSTime = static_cast<TimeType>(
                    bsl::sqrt(static_cast<double>(  d_seSTime / d_numRepeats
                                                  - d_avgSTime * d_avgSTime)));
    }
    VecTimeType ret(d_allocator_p);
    ret.push_back(d_avgWTime);
    ret.push_back(d_avgUTime);
    ret.push_back(d_avgSTime);
    return ret;
} // END runTests

extern "C" void *workFunc(void *arg)
{
    MQPoolPerformance::WorkData    *wdp
                         = reinterpret_cast<MQPoolPerformance::WorkData*>(arg);
    MQPoolPerformance::VecTimeType *pTimes
        = new MQPoolPerformance::VecTimeType(wdp->d_poolperf_p->allocator());

    MQPoolPerformance::TimeType startWTime = bsls::TimeUtil::getTimer();

    MQPoolPerformance::TimeType startUTime, startSTime;
    bsls::TimeUtil::getProcessTimers(&startSTime, &startUTime);

    wdp->d_func(wdp->d_poolperf_p, wdp->d_data);

    MQPoolPerformance::TimeType endWTime = bsls::TimeUtil::getTimer();

    MQPoolPerformance::TimeType endUTime, endSTime;
    bsls::TimeUtil::getProcessTimers(&endSTime, &endUTime);

    pTimes->push_back((endWTime - startWTime) / 1000);
    pTimes->push_back((endUTime - startUTime) / 1000);
    pTimes->push_back((endSTime - startSTime) / 1000);

    return pTimes;
}

MQPoolPerformance::VecTimeType MQPoolPerformance::runTest(
                                               VecIntType&                args,
                                               MQPoolPerformance::RunFunc func)
{
    if (d_numThreads == 1) {
        // For a single thread, run calculation on the caller
        TimeType startWTime = bsls::TimeUtil::getTimer();
        TimeType startUTime, startSTime;
        bsls::TimeUtil::getProcessTimers(&startSTime, &startUTime);
        args.push_back(-1);
        func(this, args);
        TimeType endWTime = bsls::TimeUtil::getTimer();
        TimeType endUTime, endSTime;
        bsls::TimeUtil::getProcessTimers(&endSTime, &endUTime);
        VecTimeType times(d_allocator_p);
        times.push_back((endWTime - startWTime) / 1000);
        times.push_back((endUTime - startUTime) / 1000);
        times.push_back((endSTime - startSTime) / 1000);
        return times;                                                 // RETURN
    }

    bsl::vector<bslmt::ThreadUtil::Handle> handles(d_numThreads);
    bsl::vector<WorkData>                  todos(d_numThreads);

    // Spawn work threads
    for (int i = 0; i < d_numThreads; ++i) {
        todos[i].d_func = func;
        todos[i].d_poolperf_p = this;
        todos[i].d_data = args;
        todos[i].d_data.push_back(i);

        bslmt::ThreadUtil::create(&handles[i], workFunc, &todos[i]);
    }
    // Collect results
    VecTimeType        times(d_allocator_p);
    bsl::vector<void*> results(d_numThreads);
    for (int i = 0; i < d_numThreads; ++i) {
        bslmt::ThreadUtil::join(handles[i], &results[i]);
        VecTimeType *pRes = reinterpret_cast<VecTimeType*>(results[i]);
        for (size_t j = 0; j < pRes->size(); ++j) times.push_back((*pRes)[j]);
        delete pRes;
    }

    return times;
} // END runTest

void MQPoolPerformance::printResult()
{
    // print the output from the calculation
    bsl::cout << "Title=" << d_title << "\n";
    bsl::cout << bsl::fixed << bsl::setprecision(1);
    bsl::cout << "Threads="   << d_numThreads
              << ",RThreads=" << d_numRThreads
              << ",WThreads=" << d_numWThreads << "\n";
    // Time is printed in milliseconds
    bsl::cout << "Wall Time="
              << d_avgWTime / 1000
              << "+/-"
              << static_cast<double>(d_seWTime)
                                      / static_cast<double>(d_avgWTime) * 100.0
              << "%\n";
    bsl::cout << "User Time="
              << d_avgUTime / 1000
              << "+/-"
              << static_cast<double>(d_seUTime)
                                      / static_cast<double>(d_avgUTime) * 100.0
              << "%\n";
    bsl::cout << "System Time="
              << d_avgSTime / 1000
              << "+/-"
              << static_cast<double>(d_seSTime)
                                      / static_cast<double>(d_avgSTime) * 100.0
              << "%\n";
}

// ACCESSORS
inline
bslma::Allocator *MQPoolPerformance::allocator() const
{
    return d_allocator_p;
}

inline
int MQPoolPerformance::numCalcs() const
{
    return d_numCalcs;
}

inline
int MQPoolPerformance::numRThreads() const
{
    return d_numRThreads;
}

inline
int MQPoolPerformance::numRepeats() const
{
    return d_numRepeats;
}

inline
int MQPoolPerformance::numThreads() const
{
    return d_numThreads;
}

inline
int MQPoolPerformance::numWThreads() const
{
    return d_numWThreads;
}

inline
const char* MQPoolPerformance::title() const
{
    return d_title.c_str();
}

// TEST FUNCTIONS
int MQPoolPerformance::testFastSearch(MQPoolPerformance *poolperf_p,
                                      VecIntType&)
{
    // Run the fastSearch test functionInsert with the specified 'poolperf_p'.
    // The specified 'args' vector is empty.
    bslma::TestAllocator ta(false);

    bsl::string WORDS[] = {
        "a", "b", "c", "d", "e", "f", "g", "h",
        "i", "j", "k", "l", "m", "n", "o", "p",
        "q", "r", "s", "t", "u", "v", "w", "x",
        "y", "z", "A", "B", "C", "D", "E", "F",
        "G", "H", "I", "J", "K", "L", "M", "N",
        "O", "P", "Q", "R", "S", "T", "U", "V",
        "W", "X", "Y", "Z", "1", "2", "3", "4",
        "5", "6", "7", "8", "9", "0",
    };
    const bsl::string PATH("/usr/include/");
    bsl::string FILES[] = {
        PATH + "err.h", PATH + "errno.h", PATH + "error.h",
        PATH + "zlib.h", PATH + "elf.h", PATH + "argp.h"
    };
    enum {
        k_NUM_WORDS = sizeof WORDS / sizeof *WORDS,
        k_NUM_FILES = sizeof FILES / sizeof *FILES
    };

    bsl::vector<bsl::string> wordList(WORDS, WORDS + k_NUM_WORDS, &ta);
    bsl::vector<bsl::string> fileList(FILES, FILES + k_NUM_FILES, &ta);
    bsl::set<bsl::string>    resultSet;
    int numCalcs = poolperf_p->numCalcs();
    fastSearch(wordList, fileList, resultSet, numCalcs);
    return 0;
}

}  // close namespace mqpoolperf

// ============================================================================
//                         For test cases 14 and 15
// ----------------------------------------------------------------------------

// This was originally done twice in a loop in case 14, but it is taking so
// long that it was broken up to be called separately in 2 cases with different
// values of 'concurrency'.

namespace MULTIQUEUETHREADPOOL_CASE_14 {

enum {
#ifndef BSLS_PLATFORM_OS_CYGWIN
    k_NUM_QUEUES = 9
#else
    k_NUM_QUEUES = 5
#endif
};

void testDrainQueueAndDrain(bslma::TestAllocator *ta, int concurrency)
{
    enum {
        k_MIN_THREADS = 1,
        k_MAX_THREADS = k_NUM_QUEUES + 1,
        k_MAX_IDLE    = 60000    // milliseconds
    };
    bslmt::ThreadAttributes defaultAttrs;

    int queueIds[k_NUM_QUEUES];

    const double SLEEP_HARDLY_TIME   = 0.01;
    const double SLEEP_A_LITTLE_TIME = 0.50;
    const double SLEEP_A_LOT_TIME    = 3.00;

    Sleeper sleepHardly( SLEEP_HARDLY_TIME);
    Sleeper sleepALittle(SLEEP_A_LITTLE_TIME);
    Sleeper sleepALot(   SLEEP_A_LOT_TIME);

    int ii;
    enum { k_MAX_LOOP = 4 };
    for (ii = 0; ii <= k_MAX_LOOP; ++ii) {
        Obj mX(defaultAttrs, k_MIN_THREADS, k_MAX_THREADS, k_MAX_IDLE, ta);
        const Obj& X = mX;
        ASSERT(0 == mX.start());

        Sleeper::s_finished = 0;
        for (int i = 0; i < k_NUM_QUEUES; ++i) {
            queueIds[i] = mX.createQueue();          ASSERT(queueIds[i]);
        }
        double startTime = now();
        for (int i = 0; i < k_NUM_QUEUES; ++i) {
            if (k_NUM_QUEUES - 1 == i) {
                mX.enqueueJob(queueIds[i], sleepALittle);
            }
            else {
                mX.enqueueJob(queueIds[i], sleepALot);
            }
        }
        double time = now() - startTime;
        ASSERT(ii < k_MAX_LOOP || time < SLEEP_A_LITTLE_TIME);
        if (time >= SLEEP_A_LITTLE_TIME) {
            if (verbose) { P_(L_); P_(concurrency); P(time); }
            continue;
        }

        mX.drainQueue(queueIds[k_NUM_QUEUES - 1]);
        ASSERT(1 == Sleeper::s_finished);
        {
            time = now() - startTime;
            ASSERTV(time, time >= SLEEP_A_LITTLE_TIME - jumpTheGun);
            ASSERT(ii < k_MAX_LOOP || time <  SLEEP_A_LOT_TIME * 0.90);
            if (time >= SLEEP_A_LOT_TIME * 0.90) {
                if (verbose) { P_(L_); P(time); }
                continue;
            }
        }

        int doneJobs, enqueuedJobs;
        X.numProcessed(&doneJobs, &enqueuedJobs);
        ASSERT(k_NUM_QUEUES == enqueuedJobs);

        mX.drain();
        time = now() - startTime;
        ASSERTV(time, time >= SLEEP_A_LOT_TIME - jumpTheGun);
        X.numProcessed(&doneJobs, &enqueuedJobs);
        ASSERT(k_NUM_QUEUES == doneJobs);
        ASSERT(k_NUM_QUEUES == enqueuedJobs);
        ASSERT(k_NUM_QUEUES == Sleeper::s_finished);

        // verify pool is still enabled
        startTime = now();
        for (int i = 0; i < k_NUM_QUEUES; ++i) {
            ASSERT(mX.isEnabled(queueIds[i]));
            ASSERT(0 == mX.enqueueJob(queueIds[i], sleepHardly));
        }

        mX.stop();
        ASSERT(now() - startTime >= SLEEP_HARDLY_TIME - jumpTheGun);
        ASSERT(2 * k_NUM_QUEUES == Sleeper::s_finished);

        ASSERT(0 == mX.start());
        Sleeper::s_finished = 0;
        ASSERT(mX.isEnabled(queueIds[2]));
        ASSERT(0 == mX.enqueueJob(queueIds[2], sleepALittle));
        ASSERT(0 == mX.disableQueue(queueIds[2]));
        ASSERT(!mX.isEnabled(queueIds[2]));

        ASSERT(mX.isEnabled(queueIds[4]));
        ASSERT(0 == mX.enqueueJob(queueIds[4], sleepALittle));
        ASSERT(0 == mX.disableQueue(queueIds[4]));
        ASSERT(!mX.isEnabled(queueIds[4]));
        mX.drainQueue(queueIds[2]);

        // verify queues are disabled as expected after 'drainQueue()'
        for (int i = 0; i < k_NUM_QUEUES; ++i) {
            if (2 == i || 4 == i) {
                ASSERT(!mX.isEnabled(queueIds[i]));
                ASSERT(0 != mX.enqueueJob(queueIds[i], sleepHardly));
            }
            else {
                ASSERT(mX.isEnabled(queueIds[i]));
                ASSERT(0 == mX.enqueueJob(queueIds[i], sleepHardly));
            }
        }
        mX.drain();
        ASSERT(k_NUM_QUEUES == Sleeper::s_finished);

        // verify queues are disabled as expected after 'drain()'
        for (int i = 0; i < k_NUM_QUEUES; ++i) {
            if (2 == i || 4 == i) {
                ASSERT(!mX.isEnabled(queueIds[i]));
                ASSERT(0 != mX.enqueueJob(queueIds[i], sleepHardly));
            }
            else {
                ASSERT(mX.isEnabled(queueIds[i]));
                ASSERT(0 == mX.enqueueJob(queueIds[i], sleepHardly));
            }
        }

        mX.stop();
        ASSERT(2 * k_NUM_QUEUES - 2 == Sleeper::s_finished);

        // make sure that 'drain' on a stopped pool won't segfault or abort,
        // and that it will return immediately
        startTime = now();
        mX.drain();
        time = now() - startTime;
        ASSERT(ii < k_MAX_LOOP || time < 0.010);
        if (time >= 0.010) {
            if (verbose) { P_(L_); P(time); }
            continue;
        }

        // verify global queue is still disabled
        for (int i = 0; i < k_NUM_QUEUES; ++i) {
            ASSERT(0 != mX.enqueueJob(queueIds[i], sleepHardly));
        }

        // make sure that 'drainQueue' on a stopped pool won't segfault or
        // abort, and that it will return immediately
        startTime = now();
        mX.drainQueue(queueIds[3]);
        time = now() - startTime;
        ASSERT(ii < k_MAX_LOOP || time < 0.010);
        if (time >= 0.010) {
            if (verbose) { P_(L_); P(time); }
            continue;
        }

        // verify global queue is still disabled
        for (int i = 0; i < k_NUM_QUEUES; ++i) {
            ASSERT(0 != mX.enqueueJob(queueIds[i], sleepHardly));
        }

        break;
    }
    if (verbose) { P_(L_); P(ii); }
    ASSERT(ii <= k_MAX_LOOP);

}
}  // close namespace MULTIQUEUETHREADPOOL_CASE_14

struct DoNothing {
    void operator()() const {}
        // NOP functor for cases 21, 22.
};

// ============================================================================
//                              MAIN PROGRAM

int main(int argc, char *argv[]) {

    int test = (argc > 1) ? bsl::atoi(argv[1]) : 1;

    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator ta("test"), da("default");
    bslma::DefaultAllocatorGuard dGuard(&da);

    bslma::NewDeleteAllocator  globalAllocator;
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 34: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver.  Make use of existing test apparatus by instantiating
        //   objects with a 'bslma::TestAllocator' object where applicable.
        //   Additionally, replace all calls to 'assert' in the usage example
        //   with calls to 'ASSERT'.  This now becomes the source, which is
        //   then "copied" back to the header file by reversing the above
        //   process.
        //
        // Testing:
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Usage Example 1" << endl
                 << "=======================" << endl;
        }

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            bsl::string WORDS[] = {
                "a", "b", "c", "d", "e", "f", "g", "h",
                "i", "j", "k", "l", "m", "n", "o", "p",
                "q", "r", "s", "t", "u", "v", "w", "x",
                "y", "z", "A", "B", "C", "D", "E", "F",
                "G", "H", "I", "J", "K", "L", "M", "N",
                "O", "P", "Q", "R", "S", "T", "U", "V",
                "W", "X", "Y", "Z", "1", "2", "3", "4",
                "5", "6", "7", "8", "9", "0",
            };
#ifdef BSLS_PLATFORM_OS_WINDOWS
            const bsl::string PATH("/usr/include/");
#else
            const bsl::string PATH("/usr/include/");
#endif
            bsl::string FILES[] = {
                PATH + "err.h", PATH + "errno.h", PATH + "error.h",
                PATH + "zlib.h", PATH + "elf.h", PATH + "argp.h"
            };
            enum {
                k_NUM_WORDS = sizeof WORDS / sizeof *WORDS,
                k_NUM_FILES = sizeof FILES / sizeof *FILES
            };

            bsl::vector<bsl::string> wordList(WORDS, WORDS + k_NUM_WORDS, &ta);
            bsl::vector<bsl::string> fileList(FILES, FILES + k_NUM_FILES, &ta);
            bsl::set<bsl::string>    resultSet;

            bsls::TimeInterval start = bsls::SystemTime::nowRealtimeClock();
            fastSearch(wordList, fileList, resultSet);
            bsls::TimeInterval stop = bsls::SystemTime::nowRealtimeClock();

//            ASSERT(3 == resultSet.size())

            if (verbose) {
              cout << "DIRECTORY = [ ";
              bsl::copy(fileList.begin(), fileList.end(),
                        bsl::ostream_iterator<bsl::string>(cout, " "));
              cout << ']' << endl;
              cout << "WORD_LIST = [ ";
              bsl::copy(wordList.begin(), wordList.end(),
                        bsl::ostream_iterator<bsl::string>(cout, " "));
              cout << ']' << endl;
              cout << "RESULTS   = [ ";
              bsl::copy(resultSet.begin(), resultSet.end(),
                        bsl::ostream_iterator<bsl::string>(cout, " "));
              cout << ']' << endl;
              cout << "TIME      = " << (stop - start) << endl;
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 33: {
        // --------------------------------------------------------------------
        // TESTING BATCH SIZE
        //
        // Concerns:
        //: 1 The value returned by 'batchSize' matches the value assigned by
        //:   'setBatchSize'.
        //:
        //: 2 The value assigned by 'setBatchSize' is the batching size.
        //:
        //: 3 The 'deleteQueue' method operates properly (since a job is
        //:   enqueued for handling deletion).
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use 'setBatchSize' to set the batching size and directly verify
        //:   the result of 'batchSize'.  (C-1)
        //:
        //: 2 From the main thread, use 'setBatchSize' to set the batching size
        //:   and submit at least this number of jobs to the queue with the
        //:   first job using a barrier to sync the thread-pool thread with the
        //:   main thread twice.  After the first synchronization, the main
        //:   thread will delete the queue containing the jobs.  After the
        //:   second synchronization, the main thread will verify the number
        //:   of executed jobs is frequently the batch size using
        //:   'numProcessed'.  Note that the number of executed jobs will not
        //:   always match the batch size since the timing of when the thread
        //:   from the thread pool will take jobs can not be guaranteed.  (C-2)
        //:
        //: 3 Verify the number of enqueued, processed, and deleted jobs in
        //:   P-2 to ensure no jobs were lost.  Use 'numQueues' and
        //:   'numElements' to verify the queue no longer exists.  (C-3)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   void setBatchSize(int id, int batchSize);
        //   int batchSize(int id) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING BATCH SIZE\n"
                          << "===================\n";

        if (verbose) cout << "\nTesting 'batchSize'." << endl;
        {
            Obj mX(bslmt::ThreadAttributes(), 1, 1, 30);  const Obj& X = mX;

            mX.start();

            ASSERT(-1 == X.batchSize(0));

            ASSERT( 0 != mX.setBatchSize(0, 2));

            int queueId = mX.createQueue();

            ASSERT( 1 == X.batchSize(queueId));
            ASSERT(-1 == X.batchSize(queueId + 1));

            ASSERT( 0 == mX.setBatchSize(queueId, 2));
            ASSERT( 0 != mX.setBatchSize(queueId + 1, 2));

            ASSERT( 2 == X.batchSize(queueId));

            ASSERT( 0 == mX.setBatchSize(queueId, 3));

            ASSERT( 3 == X.batchSize(queueId));

            ASSERT( 0 == mX.setBatchSize(queueId, 1));

            ASSERT( 1 == X.batchSize(queueId));
        }

        if (verbose) cout << "\nTesting 'setBatchSize'." << endl;
        {
            const int k_ENQUEUE = 5;

            int count[k_ENQUEUE + 1];

            for (int batchSize = 1; batchSize <= 4; ++batchSize) {
                for (int i = 0; i <= k_ENQUEUE; ++i) {
                    count[i] = 0;
                }

                for (int i = 0; i < 100; ++i) {
                    Obj        mX(bslmt::ThreadAttributes(), 1, 1, 30);
                    const Obj& X = mX;

                    mX.start();
                    int queueId = mX.createQueue();

                    mX.setBatchSize(queueId, batchSize);

                    bslmt::Barrier barrier(2);

                    Func job = bdlf::BindUtil::bind(&waitWait, &barrier);

                    mX.enqueueJob(queueId, job);
                    for (int j = 1; j < k_ENQUEUE; ++j) {
                        mX.enqueueJob(queueId, noop);
                    }

                    barrier.wait();
                    mX.deleteQueue(queueId, noop);  // must not wait
                    barrier.wait();

                    ASSERT( 0 == X.numQueues());
                    ASSERT(-1 == X.numElements(queueId));

                    int doneJobs;
                    int enqueuedJobs;
                    int deletedJobs;

                    X.numProcessed(&doneJobs, &enqueuedJobs, &deletedJobs);

                    ASSERT(k_ENQUEUE == enqueuedJobs);
                    ASSERT(k_ENQUEUE == doneJobs + deletedJobs);

                    ++count[doneJobs];
                }

                ASSERTV(batchSize, count[batchSize], 70 <= count[batchSize]);

                ASSERT(0 == count[0]);
                for (int i = batchSize + 1; i <= k_ENQUEUE; ++i) {
                    ASSERT(0 == count[i]);
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX(bslmt::ThreadAttributes(), 1, 1, 30);

            mX.start();
            int queueId = mX.createQueue();

            ASSERT_SAFE_PASS(mX.setBatchSize(queueId,  1));
            ASSERT_SAFE_PASS(mX.setBatchSize(queueId,  2));
            ASSERT_SAFE_FAIL(mX.setBatchSize(queueId,  0));
            ASSERT_SAFE_FAIL(mX.setBatchSize(queueId, -1));
        }
      }  break;
      case 32: {
        // --------------------------------------------------------------------
        // DRQS 143578129: 'numElements' stress test
        //
        // Concerns:
        //: 1 The value returned by 'numElements' is non-negative.
        //
        // Plan:
        //: 1 Stress test 'bdlmt::MultiQueueThreadPool' and frequently verify
        //:   'numElements() >= 0'.
        //
        // Testing:
        //   DRQS 143578129: 'numElements' stress test
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "DRQS 143578129: 'numElements' stress test\n"
                 << "=========================================\n";
        }

        Obj mX(bslmt::ThreadAttributes(), 4, 4, 1000);  const Obj& X = mX;

        mX.start();

        bsl::vector<int> queueId;
        {
            for (int i = 0; i < 5; ++i) {
                queueId.push_back(mX.createQueue());
            }
        }

        int max = 0;
        for (int iter = 0; iter < 1000; ++iter) {
            if (0 == iter % 9) {
                queueId.push_back(mX.createQueue());
            }
            if (0 == iter % 11) {
                mX.deleteQueue(queueId.front());
                queueId.erase(queueId.begin());
            }
            for (bsl::size_t i = 0; i < queueId.size(); ++i) {
                int n = X.numElements(queueId[i]);
                ASSERT(0 <= n);

                mX.enqueueJob(queueId[i], case32Job);
                ++max;
                if (0 == (n + iter + i) % 3) {
                    mX.enqueueJob(queueId[i], case32Job);
                    ++max;
                }
            }
            int n = X.numElements();
            ASSERT(0   <= n);
            ASSERT(max >= n);
            if (n < max) {
                max = n;
            }
        }

        for (bsl::size_t i = 0; i < queueId.size(); ++i) {
            mX.deleteQueue(queueId[i]);
            ASSERT(0 <= X.numElements());
        }

        mX.drain();
        ASSERT(0 == X.numElements());

        mX.stop();
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // DRQS 140403279: pause can deadlock with delete and create
        //
        // Concerns:
        //: 1 If the 'pauseQueue' method blocks, there will not be a deadlock
        //:   with other threads executing 'deleteQueue' and 'createQueue'.
        //
        // Plan:
        //: 1 Recreate the scenario and verify there is no deadlock.
        //
        // Testing:
        //   DRQS 140403279: pause can deadlock with delete and create
        // --------------------------------------------------------------------

        if (verbose) {
            cout
              << "DRQS 140403279: pause can deadlock with delete and create\n"
              << "=========================================================\n";
        }
        {
            // verifying with 'deleteQueue'

            Obj mX(bslmt::ThreadAttributes(), 4, 4, 30);

            mX.start();
            int queueId = mX.createQueue();

            bslmt::ThreadUtil::Handle pauseHandle;
            bslmt::Barrier            pauseBarrier(2);

            bslmt::ThreadUtil::create(&pauseHandle,
                                      bdlf::BindUtil::bind(&waitPauseWait,
                                                           &pauseBarrier,
                                                           &mX,
                                                           queueId));

            bslmt::Barrier deleteBarrier(2);

            Func job = bdlf::BindUtil::bind(&waitWaitDelete,
                                            &deleteBarrier,
                                            &mX,
                                            queueId);

            ASSERT(0 == mX.enqueueJob(queueId, job));

            int rv;

            rv = deleteBarrier.timedWait(
                    bsls::SystemTime::nowRealtimeClock().addMilliseconds(200));

            ASSERT(0 == rv && "delete 1");
            if (0 != rv) {
                exit(0);
            }

            rv = pauseBarrier.timedWait(
                    bsls::SystemTime::nowRealtimeClock().addMilliseconds(200));

            ASSERT(0 == rv && "pause 1");
            if (0 != rv) {
                exit(0);
            }

            // Wait for the pausing thread to release the mutex protecting the
            // queue and start waiting for a signal.  There is no good way to
            // do this so a "sleep" is used.

            bslmt::ThreadUtil::microSleep(20000);

            rv = deleteBarrier.timedWait(
                    bsls::SystemTime::nowRealtimeClock().addMilliseconds(200));

            ASSERT(0 == rv && "delete 2");
            if (0 != rv) {
                exit(0);
            }

            // If the pausing thread is still blocked, the following will
            // time-out.

            rv = pauseBarrier.timedWait(
                    bsls::SystemTime::nowRealtimeClock().addMilliseconds(200));

            ASSERT(0 == rv && "pause 2");
            if (0 != rv) {
                exit(0);
            }

            ASSERT(0 == bslmt::ThreadUtil::join(pauseHandle));

            mX.drain();
            mX.stop();
        }
        {
            // verifying with 'createQueue'

            Obj mX(bslmt::ThreadAttributes(), 4, 4, 30);

            mX.start();
            int queueId = mX.createQueue();

            bslmt::ThreadUtil::Handle pauseHandle;
            bslmt::Barrier            pauseBarrier(2);

            bslmt::ThreadUtil::create(&pauseHandle,
                                      bdlf::BindUtil::bind(&waitPauseWait,
                                                           &pauseBarrier,
                                                           &mX,
                                                           queueId));

            bslmt::Barrier createBarrier(2);

            Func job = bdlf::BindUtil::bind(&waitWaitCreate,
                                            &createBarrier,
                                            &mX);

            ASSERT(0 == mX.enqueueJob(queueId, job));

            int rv;

            rv = createBarrier.timedWait(
                    bsls::SystemTime::nowRealtimeClock().addMilliseconds(200));

            ASSERT(0 == rv && "create 1");
            if (0 != rv) {
                exit(0);
            }

            rv = pauseBarrier.timedWait(
                    bsls::SystemTime::nowRealtimeClock().addMilliseconds(200));

            ASSERT(0 == rv && "pause 1");
            if (0 != rv) {
                exit(0);
            }

            // Wait for the pausing thread to release the mutex protecting the
            // queue and start waiting for a signal.  There is no good way to
            // do this so a "sleep" is used.

            bslmt::ThreadUtil::microSleep(20000);

            rv = createBarrier.timedWait(
                    bsls::SystemTime::nowRealtimeClock().addMilliseconds(200));

            ASSERT(0 == rv && "create 2");
            if (0 != rv) {
                exit(0);
            }

            // If the pausing thread is still blocked, the following will
            // time-out.

            rv = pauseBarrier.timedWait(
                    bsls::SystemTime::nowRealtimeClock().addMilliseconds(200));

            ASSERT(0 == rv && "pause 2");
            if (0 != rv) {
                exit(0);
            }

            ASSERT(0 == bslmt::ThreadUtil::join(pauseHandle));

            mX.drain();
            mX.stop();
        }
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // DRQS 140150365: resume fails immediately after pause
        //
        // Concerns:
        //: 1 The method 'resumeQueue' does not return an error when called
        //:   quickly after 'pauseQueue'.
        //:
        //: 2 If 'pauseQueue' blocks, another thread that quickly runs
        //:   'resumeQueue' unblocks the blocked thread (while the "currently
        //:   executing job is completing").
        //
        // Plan:
        //: 1 Recreate the scenario and verify that 'resumeQueue' succeeds.
        //
        // Testing:
        //   DRQS 140150365: resume fails immediately after pause
        // --------------------------------------------------------------------

        if (verbose) {
            cout
               << "DRQS 140150365: 'resume' fails immediately after 'pause'\n"
               << "========================================================\n";
        }

        {
            // verify when 'pauseQueue' does not block

            // Note that in this test, 'pauseQueue' is expected -- but not
            // guaranteed -- to not block.  As such, the test will be repeated
            // until the desired code path is executed, or a large number of
            // iterations is performed (indicating failure).

            bslmt::Barrier controlBarrier(2);

            bsls::AtomicInt timedOut;

            Func job = bdlf::BindUtil::bind(&timedWaitOnBarrier,
                                            &controlBarrier,
                                            &timedOut);

            Obj mX(bslmt::ThreadAttributes(), 1, 1, 30);

            bool haveDesiredCodePath = false;

            for (int i = 0; i < 100 && !haveDesiredCodePath; ++i) {
                timedOut = 0;

                mX.start();
                int queueId = mX.createQueue();

                // The enqueued job will set 'timedOut' if the contained wait
                // on 'controlBarrier' times out.  We are trying to pause the
                // queue before the job executes, so if '0 != timedOut' occurs
                // during the iteration, the iteration is not useful (the
                // executed code path in 'pauseQueue' can not be determined).

                ASSERT(0 == mX.enqueueJob(queueId, job));

                // The following invocation of 'pauseQueue' may execute before,
                // or after, the enqueued job starts.

                ASSERT(0 == mX.pauseQueue(queueId));

                // Note that '0 != timedOut' indicates 'pauseQueue' executed
                // after the enqueued job started (but does not guarantee that
                // 'pauseQueue' blocks), and the iteration is not useful.

                if (0 == timedOut) {
                    int rv = mX.resumeQueue(queueId);
                    ASSERT(0 == rv);

                    // Note that there is a very unlikely race with the timeout
                    // in 'timedWaitInBarrier' if the thread that invoked
                    // 'resumeQueue' is descheduled for a long time so, to
                    // prevent a possible "hang" if 'wait' were used, the
                    // following must be a 'timedWait'.

                    rv = controlBarrier.timedWait(
                                           bsls::SystemTime::nowRealtimeClock()
                                                        .addMilliseconds(100));

                    // If this 'timeWait' times out, then the 'timedWait' in
                    // 'timedWaitOnBarrier' must have timed out (due to
                    // pathalogical scheduling), and the iteration should not
                    // be counted (i.e., '0 != timedOut').

                    if (0 == rv) {
                        haveDesiredCodePath = true;
                    }
                    else {
                        ASSERT(0 != timedOut);
                    }
                }

                mX.drain();
                mX.stop();
            }

            ASSERT(haveDesiredCodePath);
        }
        {
            // verify when 'pauseQueue' does block

            bslmt::Barrier controlBarrier(2);

            Func job = bdlf::BindUtil::bind(&waitOnBarrier,
                                            &controlBarrier,
                                            1);

            Obj mX(bslmt::ThreadAttributes(), 1, 1, 30);

            mX.start();
            int queueId = mX.createQueue();

            bslmt::ThreadUtil::Handle pauseHandle;
            bslmt::Barrier            pauseBarrier(2);

            bslmt::ThreadUtil::create(
                                   &pauseHandle,
                                   bdlf::BindUtil::bind(&waitPauseWait,
                                                        &pauseBarrier,
                                                        &mX,
                                                        queueId));

            ASSERT(0 == mX.enqueueJob(queueId, job));

            pauseBarrier.wait();

            // Wait for the pausing thread to release the mutex protecting the
            // queue and start waiting for a signal.  There is no good way to
            // do this so a "sleep" is used.

            bslmt::ThreadUtil::microSleep(20000);

            // Failing on the following 'ASSERT' implies the sleep was not
            // effective.

            ASSERT(0 == mX.resumeQueue(queueId));

            controlBarrier.wait();

            // If the pausing thread is still blocked, the following will
            // time-out.

            int rv = pauseBarrier.timedWait(
                    bsls::SystemTime::nowRealtimeClock().addMilliseconds(200));

            ASSERT(0 == rv);

            if (0 != rv) {
                exit(0);
            }

            ASSERT(0 == bslmt::ThreadUtil::join(pauseHandle));

            mX.drain();
            mX.stop();
        }
      }  break;
      case 29: {
        // --------------------------------------------------------------------
        // DRQS 138890062: 'deleteQueueCb' running after destructor
        //
        // Concerns:
        //: 1 The method 'deleteQueueCb' can complete after the destructor has
        //:   completed.
        //
        // Plan:
        //: 1 Recreate the scenario and verify that the destructor will not
        //:   complete before the 'deleteQueueCb' method.
        //
        // Testing:
        //   DRQS 138890062: 'deleteQueueCb' running after destructor
        // --------------------------------------------------------------------

        if (verbose) {
            cout
            << "DRQS 138890062: 'deleteQueueCb' running after destructor\n"
            << "========================================================\n";
        }

        bslmt::Barrier barrier(2);

        s_case29Barrier_p = &barrier;

        bdlmt::ThreadPool pool(bslmt::ThreadAttributes(), 4, 4, 30);

        pool.start();

        {
            Obj mX(&pool);

            mX.start();
            int queueId = mX.createQueue();

            mX.deleteQueue(queueId, case29Job);

            barrier.wait();
        }

        // If the destructor waited for the 'deleteQueueCb' to complete, the
        // following will time-out.

        ASSERT(0 != barrier.timedWait(
                   bsls::SystemTime::nowRealtimeClock().addMilliseconds(200)));

        pool.shutdown();
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // DRQS 139148629: deadlock when job pauses another queue
        //
        // Concerns:
        //: 1 That deadlock can occur in a 'bdlmt::MultiQueueThreadPool' with
        //:   one thread when a job from one queue pauses a different queue.
        //
        // Plan:
        //: 1 Recreate the scenario and verify the deadlock no longer occurs.
        //
        // Testing:
        //   DRQS 139148629: deadlock when job pauses another queue
        // --------------------------------------------------------------------

        if (verbose) {
            cout
            << "DRQS 139148629: deadlock when job pauses another queue\n"
            << "======================================================\n";
        }

        Obj mX(bslmt::ThreadAttributes(), 1, 1, 30);

        s_case28Obj_p = &mX;

        mX.start();
        int queueId = mX.createQueue();

        ASSERT(0 == mX.enqueueJob(queueId, case28Job));

        for (int i = 0; i < 10 && 0 == s_case28Count; ++i) {
            bslmt::ThreadUtil::microSleep(100000);
        }

        ASSERT(1 == s_case28Count);

        if (0 == s_case28Count) {
            exit(0);
        }

        mX.drain();
        mX.stop();
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // DRQS 118269630: 'drain' may not drain underlying threadpool
        //
        // Concerns:
        //: 1 That 'drain' appropriately reaches completion without causing
        //:   race conditions between 'drain' and 'enqueueJob'.
        //
        // Plan:
        //: 1 Recreate the scenario and verify the race no longer occurs.
        //
        // Testing:
        //   DRQS 118269630: 'drain' may not drain underlying threadpool
        // --------------------------------------------------------------------

        if (verbose) {
            cout
            << "DRQS 118269630: 'drain' may not drain underlying threadpool\n"
            << "===========================================================\n";
        }

        Obj mX(bslmt::ThreadAttributes(), 4, 4, 30);

        mX.start();
        int queueId = mX.createQueue();

        Case27DrainThread         drainThread(&mX);
        bslmt::ThreadUtil::Handle drainThreadHandle;

        ASSERT(0 == bslmt::ThreadUtil::create(&drainThreadHandle,
                                              drainThread));

        for (int i = 0; i < 1000; ++i) {
            ASSERT(0 == mX.enqueueJob(queueId, case27Counter));
            ++s_case27Count;
            bslmt::ThreadUtil::microSleep(100);
        }

        ASSERT(0 == bslmt::ThreadUtil::join(drainThreadHandle));

        bslmt::ThreadUtil::microSleep(100000);

        ASSERT(0 == s_case27Count);
      }  break;
      case 26: {
        // --------------------------------------------------------------------
        // DRQS 113734461: 'deleteQueue' copies cleanupFunctor
        //
        // Concerns:
        //: 1 That 'deleteQueue' copies the user supplied 'cleanupFunctor'
        //:   so that if the user supplied functor goes out of scope
        //:   and is destroyed, the task does not seg-fault.
        //
        // Plan:
        //: 1 Recreate the scenario and verify the deadlock no longer occurs.
        //
        // Testing:
        //   DRQS 113734461: 'deleteQueue' copies cleanupFunctor
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "DRQS 113734461: 'deleteQueue' copies cleanupFunctor\n"
                 << "===================================================\n";
        }

        Obj          mX(bslmt::ThreadAttributes(), 4, 4, 30);
        bool         cleanupDone = false;
        bslmt::Latch latch(1);

        mX.start();
        int queueId = mX.createQueue();
        mX.enqueueJob(queueId,
                          bdlf::BindUtil::bind(&case26WaitJob, &latch));

        // We call 'deleteQueue' within a separate function to ensure the
        // cleanup functor temporary object supplied to 'deleteQueue' is
        // destroyed.

        case26DeleteQueue(&mX, queueId, &cleanupDone);

        latch.arrive();
        mX.drain();
        mX.stop();
        ASSERT(true == cleanupDone);

      } break;
      case 25: {
        // --------------------------------------------------------------------
        // DRQS 112259433: 'drain' and 'deleteQueue' can deadlock
        //
        // Concerns:
        //: 1 The 'drain' and 'deleteQueue' methods should not allow a
        //:   deadlock.  This DRQS revealed a scenario where, if a thread is
        //:   waiting on 'drain' and 'deleteQueue' is called from an executing
        //:   job, a deadlock occurs.
        //
        // Plan:
        //: 1 Recreate the scenario and verify the deadlock no longer occurs.
        //
        // Testing:
        //   DRQS 112259433: 'drain' and 'deleteQueue' can deadlock
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "DRQS 112259433: 'drain' and 'deleteQueue' can deadlock"
                 << endl
                 << "======================================================"
                 << endl;
        }

        Obj mX(bslmt::ThreadAttributes(), 4, 4, 30);

        mX.start();

        bslmt::Latch waitLatch(1);

        int queueId = mX.createQueue();

        mX.enqueueJob(queueId,
                      bdlf::BindUtil::bind(&deferredDeleteQueue,
                                           &mX,
                                           queueId,
                                           &waitLatch));

        waitLatch.arrive();

        mX.drain();

        mX.stop();
      }  break;
      case 24: {
        // --------------------------------------------------------------------
        // DRQS 107733386: pause queue can be indefinitely deferred
        //
        // Concerns:
        //: 1 A call to 'pauseQueue' can not be deferred indefinitely by
        //:   submitting more jobs at the front of the queue.  This DRQS
        //:   revealed a scenario where an arbitrary number of tasks for a
        //:   given queue may be submitted, after a call to 'pauseQueue', that
        //:   prevent the pause from taking effect.
        //
        // Plan:
        //: 1 Recreate the scenario and verify the pause takes effect.
        //:   Specifically, a job will be submitted that will recursively
        //:   submit itself at the front of the queue before exiting; the job
        //:   will daisy-chain itself.  While this is occurring, the queue will
        //:   be paused in another thread.  The number of times the recursive
        //:   submission succeeds will be verified as small, proving the
        //:   'pauseQueue' can not be indefinitely deferred.
        //
        // Testing:
        //   DRQS 107733386: pause queue can be indefinitely deferred
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "DRQS 107733386: pause queue can be indefinitely deferred"
                 << endl
                 << "========================================================"
                 << endl;
        }

        Obj mX(bslmt::ThreadAttributes(), 4, 4, 30);

        mX.start();

        bslmt::Latch waitLatch(1);

        int queueId = mX.createQueue();

        mX.enqueueJob(queueId,
                      bdlf::BindUtil::bind(&startDaisyChain,
                                           &mX,
                                           queueId,
                                           &waitLatch));

        waitLatch.wait();
        mX.pauseQueue(queueId);

        mX.stop();

        // While no tasks should be scheduled before the 'pauseQueue'
        // completes, verify only a few tasks were schedulable before the
        // 'pauseQueue' completed.  Failure of this assert implies the
        // 'pauseQueue' can be indefinitely delayed.

        ASSERT(100 > s_daisyChainCount);
      }  break;
      case 23: {
        // --------------------------------------------------------------------
        // DRQS 107865762: more than one task per queue to thread pool
        //
        // Concerns:
        //: 1 A 'MultiQueueThreadPool' may allow at most one task to be
        //:   submitted to the thread poll for a given queue.  This DRQS
        //:   revealed a scenario where an arbitrary number of tasks for a
        //:   given queue may be submitted to the thread pool.
        //
        // Plan:
        //: 1 Recreate the scenario and verify only one task may be submitted
        //:   to the threadpool for a queue.  Specifically, the queue will be
        //:   paused from the thread currently executing a callback.  This
        //:   thread will not be allowed to return from the callback until the
        //:   queue is resumed from another thread.  By using 'UniqueGuard',
        //:   we verify than only one task is every submitted to the thread
        //:   pool.
        //
        // Testing:
        //   DRQS 107865762: more than one task per queue to thread pool
        // --------------------------------------------------------------------

        if (verbose) {
            cout <<
                  "DRQS 107865762: more than one task per queue to thread pool"
                 << endl
                 <<
                  "==========================================================="
                 << endl;
        }

        Obj mX(bslmt::ThreadAttributes(), 4, 4, 30);

        mX.start();

        bslmt::Latch pauseLatch(1);
        bslmt::Latch doneLatch(1);

        int queueId = mX.createQueue();

        mX.enqueueJob(queueId,
                      bdlf::BindUtil::bind(&pauseQueueWithUniqueGuard,
                                           &mX,
                                           queueId,
                                           &pauseLatch,
                                           &doneLatch));

        pauseLatch.wait();

        mX.enqueueJob(queueId, bdlf::BindUtil::bind(&noopWithUniqueGuard));

        mX.resumeQueue(queueId);

        bslmt::ThreadUtil::microSleep(100000);

        doneLatch.arrive();

        mX.stop();
      }  break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING PAUSE/DELETE INTERACTION
        //
        // Concerns:
        //: 1 The various loadRelaxed statements do not cause a race condition
        //    where 'deleteQueue' executed after 'pauseQueue' fails or hangs.
        //
        // Plan:
        //: 1 Run Pause/Delete a 1000 times, and check for success on delete.
        //
        // Testing:
        //   PAUSE/DELETE INTERACTION
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "TESTING PAUSE/DELETE INTERACTION" << endl
                 << "================================" << endl;
        }

        for (int i = 0; i < 1000; ++i) {
            if (veryVerbose && i % 100 == 0) {
                cout << "ITERATION " << i << endl;
            }
            bslmt::ThreadAttributes attr;
            bdlmt::MultiQueueThreadPool pool(attr, 1, 40, 10000);

            int rc = pool.start();
            BSLS_ASSERT_OPT(rc == 0);

            const int queueId = pool.createQueue();
            BSLS_ASSERT_OPT(queueId);

            rc = pool.enqueueJob(queueId, DoNothing());
            BSLS_ASSERT_OPT(rc == 0);

            rc = pool.pauseQueue(queueId);
            BSLS_ASSERT_OPT(rc == 0);

            rc = pool.deleteQueue(queueId);
            BSLS_ASSERT_OPT(rc == 0);  // delete after pause.
        }
      }  break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING DRQS 104502699
        //
        // Concerns:
        //: 1 DRQS 104502699 shows that if a multiqueue thread pool is
        //    constructed for a thread pool, and that threadpool is shutdown,
        //    operations on the multiqueue thread pool hang.
        //
        // Plan:
        //: 1 Incorporate the example from DRQS and show it no longer hangs.
        //
        // Testing:
        //   DRQS 104502699
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "TESTING DRQS 104502699" << endl
                 << "======================" << endl;
        }

        bslmt::ThreadAttributes attr;
        bdlmt::MultiQueueThreadPool pool(attr, 1, 40, 10000);

        int rc = pool.start();
        BSLS_ASSERT_OPT(rc == 0);

        const int queueId = pool.createQueue();
        BSLS_ASSERT_OPT(queueId);

        rc = pool.enqueueJob(queueId, DoNothing());
        BSLS_ASSERT_OPT(rc == 0);

        rc = pool.pauseQueue(queueId);
        BSLS_ASSERT_OPT(rc == 0);

        rc = pool.deleteQueue(queueId); // This used to block indefinitely
        BSLS_ASSERT_OPT(rc == 0);       // delete after pause now succeeds.
      }  break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING DRQS 99979290
        //
        // Concerns:
        //: 1 DRQS 99979290 shows that if a multiqueue thread pool is
        //    constructed for a thread pool, and that threadpool is shutdown,
        //    operations on the multiqueue thread pool hang.
        //
        // Plan:
        //: 1 Incorporate the example from DRQS and show it no longer hangs.
        //
        // Testing:
        //   DRQS 99979290
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "TESTING DRQS 99979290" << endl
                 << "=====================" << endl;
        }

        bslmt::ThreadAttributes attr;
        bdlmt::ThreadPool pool(attr, 5, 5, 300);
        bdlmt::MultiQueueThreadPool mq(&pool);
        mq.start();

        int queue = mq.createQueue();

        pool.shutdown();

        mq.deleteQueue(queue);
      }  break;
      case 19: {
        // --------------------------------------------------------------------
        // FRONT
        //
        // Concerns:
        //   * 'addAtFront' positions a job at the front of the queue.
        //   * 'addAtFront' fails if the queue is disabled.
        //   * 'addAtFront' succeeds if the queue is paused.
        //   * 'addAtFront' can be invoked from a worker thread.
        //
        // Plan:
        //   Enqueue a job that waits on a synchronization mechanism in order
        //   to ensure that jobs are queued (effectively "stuck") when
        //   addAtFront is invoked. Check the order of the jobs by inspecting
        //   a string the jobs modify.
        // --------------------------------------------------------------------
        if (verbose) {
            cout << "Testing Front\n"
                    "=============\n";
        }

        bslmt::ThreadAttributes   defaultAttrs;

        Obj mX(defaultAttrs, 1, 1, INT_MAX);
        int rc = mX.start();
        ASSERT(0 == rc);

        int id1;
        {
            bslmt::Semaphore sema;
            if (veryVerbose) {
                cout << "\taddAtFront positions at front of queue" << endl;
            }

            bsl::string value;

            id1 = mX.createQueue();
            // The first job we enqueue may or may not be dequeued before
            // 'addJobAtFront' executes.  Thus, the two possible values of
            // 'value' are "abc" and "bac".
            mX.enqueueJob(id1,
                          bdlf::BindUtil::bind(&waitThenAppend,
                                               &sema, &value, 'b'));
            mX.enqueueJob(id1,
                          bdlf::BindUtil::bind(&waitThenAppend,
                                               &sema, &value, 'c'));
            mX.addJobAtFront(id1,
                             bdlf::BindUtil::bind(&waitThenAppend,
                                                  &sema, &value, 'a'));
            sema.post(3);
            mX.drainQueue(id1);

            if (veryVeryVerbose) {
                cout << "checking result: " << value << endl;
            }

            ASSERT("abc" == value || "bac" == value);
        }
        {
            if (veryVerbose) {
                cout << "\taddAtFront fails when queue is disabled" << endl;
            }

            bsl::string value;

            mX.disableQueue(id1);
            int rc = mX.enqueueJob(id1,
                                   bdlf::BindUtil::bind(
                                                       &bsl::string::push_back,
                                                       &value, 'b'));
            ASSERT(0 != rc);

            mX.enableQueue(id1);
            mX.drainQueue(id1);
            ASSERT("" == value);
        }
        {
            bslmt::Semaphore sema;
            if (veryVerbose) {
                cout << "\taddAtFront succeeds if queue is paused" << endl;
            }

            bsl::string value;
            mX.pauseQueue(id1);

            mX.enqueueJob(id1,
                          bdlf::BindUtil::bind(&waitThenAppend,
                                               &sema, &value, 'c'));
            mX.addJobAtFront(id1,
                             bdlf::BindUtil::bind(&waitThenAppend,
                                                  &sema, &value, 'b'));
            mX.addJobAtFront(id1,
                             bdlf::BindUtil::bind(&waitThenAppend,
                                                  &sema, &value, 'a'));

            mX.resumeQueue(id1);

            sema.post(3);
            mX.drainQueue(id1);

            ASSERTV(value, "abc" == value);
        }
        {
            if (veryVerbose) {
                cout << "\taddAtFront can be invoked from worker thread"
                     << endl;
            }

            bsl::string value;
            bslmt::Semaphore sema;

            mX.enqueueJob(id1,
                          bdlf::BindUtil::bind(&waitThenAppend,
                                               &sema, &value, 'a'));
            mX.enqueueJob(id1,
                          bdlf::BindUtil::bind(&addAppendJobAtFront,
                                               &mX, id1, &value, 'b'));

            mX.enqueueJob(id1,
                          bdlf::BindUtil::bind(&waitThenAppend,
                                               &sema, &value, 'c'));

            sema.post(2);
            mX.drainQueue(id1);

            ASSERTV(value, "abc" == value);
        }
      } break;

      case 18: {
        // --------------------------------------------------------------------
        // PAUSE/RESUME
        //
        // Concerns:
        //   * Pausing a queue with jobs executing blocks until the current
        //     job finishes.
        //   * Jobs on other queues continue to execute.
        //   * No jobs execute on a paused queue, even if more jobs are
        //     enqueued to it.
        //   * Jobs execute after resume is invoked.
        //   * A paused queue may be disabled/enabled.
        //   * A disabled queue may be paused/resumed.
        //   * A paused queue may be deleted.
        //   * A pool having a paused queue may be drained.
        //   * A pool having a paused queue may be stopped/shutdown.
        //
        // Plan:
        //   Using an underlying threadpool with N threads, exercise the
        //   scenarios listed above.  Conditions that refer to a "paused
        //   queue" should always have a job on the queue during the
        //   test.
        //
        //   Run the tests with N=1, to verify that no deadlocks exist that
        //   involve waiting for another threadpool thread, and N=2, to verify
        //   no problems arise from putting two processing jobs into the
        //   threadpool for the same queue.
        // --------------------------------------------------------------------
        if (verbose) {
            cout << "Testing Pause/Resume\n"
                    "====================\n";
        }

        bslmt::ThreadAttributes   defaultAttrs;
        defaultAttrs.setThreadName("OtherName");

        bslmt::ThreadUtil::Handle handle;
        bslmt::ThreadAttributes   detached;
        detached.setDetachedState(bslmt::ThreadAttributes::e_CREATE_DETACHED);

        enum { k_SHORT_SLEEP = 20 * 1000 }; // 20 ms

        const struct Params {
            int d_numThreads;
            int d_numResumers;
        } PARAMS[] = {
            {1, 1},
            {3, 1},
            {1, 3},
            {3, 3}
        };

        int numParams = sizeof(PARAMS) / sizeof(Params);
        for (int i = 0; i < numParams; ++i) {
            const int NUM_THREADS = PARAMS[i].d_numThreads;
            const int NUM_RESUMERS = PARAMS[i].d_numResumers;
            if (veryVerbose) {
                cout << "Pool with " << NUM_THREADS << " thread" <<
                    (NUM_THREADS > 1 ? "s" : "") <<
                    " with resume() invoked by " << NUM_RESUMERS <<
                    " thread"  <<
                    (NUM_THREADS > 1 ? "s" : "") << endl;
            }

            Obj mX(defaultAttrs, NUM_THREADS, NUM_THREADS, INT_MAX);
            const Obj& X = mX;
            int rc = mX.start();
            ASSERT(0 == rc);

            bslmt::Barrier controlBarrier(2);
            bsls::AtomicInt count(0);
            int id1;
            {
                if (veryVerbose) {
                    cout << "\tTight pause/resume loop" << endl;
                }
                {
                    int id = mX.createQueue();
                    const int N = 100000;
                    for (int loop = 0; loop <= N; ++loop) {
                        if (veryVeryVerbose) {
                            if (loop % (N / 10) == 0) { P(loop); }
                        }
                        mX.pauseQueue(id);
                        mX.resumeQueue(id);
                    }
                }

                if (veryVerbose) {
                    cout << "\tPause blocks until job finishes" << endl;
                }
                Func job = bdlf::BindUtil::bind(&waitTwiceAndIncrement,
                                                &controlBarrier,
                                                &count, 1);
                id1 = mX.createQueue();
                mX.enqueueJob(id1, job);
                mX.enqueueJob(id1, job);

                // ensure the first job is running
                controlBarrier.wait();

                bsls::AtomicInt pauseCount(0);
                bslmt::Barrier pauseBarrier(2);
                bslmt::ThreadUtil::createWithAllocator(
                                   &handle,
                                   detached,
                                   bdlf::BindUtil::bind(&waitPauseAndIncrement,
                                                        &pauseBarrier,
                                                        &mX,
                                                        id1,
                                                        &pauseCount),
                                   &ta);
                pauseBarrier.wait();
                // Now the thread will invoke pauseQueue.  Wait a little bit
                // and ensure it hasn't finished (because the threadpool job is
                // still running)
                bslmt::ThreadUtil::microSleep(k_SHORT_SLEEP);
                ASSERT(0 == pauseCount);
                controlBarrier.wait();

                // Having unblocked the threadpool job, pause() should now be
                // able to complete
                pauseBarrier.wait();
                ASSERT(1 == pauseCount);
                ASSERT(X.isPaused(id1));

                // Pausing waited till the first job completed
                ASSERT(1 == count);

                // At this point there's still one blocked job sitting on the
                // queue.  Try to wait for it for a short time -- this will
                // fail, as the queue is paused.
                ASSERT(0 != controlBarrier.timedWait(
                                      bsls::SystemTime::nowRealtimeClock()
                                      .addMicroseconds(k_SHORT_SLEEP)));

            }
            {
                if (veryVerbose) {
                    cout << "\tJobs on other queues continue to execute"
                         << endl;
                }
                int id2 = mX.createQueue();
                bsls::AtomicInt count2(0);
                Func job =
                    bdlf::BindUtil::bind(&incrementCounter, &count2);

                mX.enqueueJob(id2, job);
                mX.enqueueJob(id2, job);
                mX.enqueueJob(id2, &checkThreadName);
                mX.drainQueue(id2);
                ASSERT(2 == count2);
            }
            {
                if (veryVerbose) {
                    cout << "\tJobs do not execute when added to paused queue"
                         << endl;
                }
                // Queue id1 is still paused
                Func job =
                    bdlf::BindUtil::bind(&waitTwiceAndIncrement,
                                         &controlBarrier,
                                         &count, 2);
                mX.enqueueJob(id1, job);
                // Try a timedWait; adding a job should not cause anything to
                // execute on the queue
                ASSERT(0 != controlBarrier.timedWait(
                                      bsls::SystemTime::nowRealtimeClock()
                                      .addMicroseconds(k_SHORT_SLEEP)));
                // count should be unchanged, with nothing executing
                ASSERT(1 == count);
            }
            {
                if (veryVerbose) {
                    cout << "\tJobs execute after resume is invoked"
                         << endl;
                }
                // Queue id1 is still paused and still has two jobs sitting on
                // it.  The first is to increment (from the existing value of
                // 1) by '1', and the second is to increment by '2'.  Ensure
                // that the jobs execute in the correct order.

                // Try to resume N times in parallel.  Only one resume() should
                // succeed, as the others should encounter either a resumed
                // queue or one in the process of being resumed.
                bsls::AtomicInt numSuccesses; // = 0
                bslmt::ThreadGroup tg;
                tg.addThreads(bdlf::BindUtil::bind(&resumeAndIncrement,
                                                   &mX, id1, &numSuccesses),
                              NUM_RESUMERS);
                tg.joinAll();

                ASSERT(!X.isPaused(id1));
                ASSERT(1 == numSuccesses);
                controlBarrier.wait();
                controlBarrier.wait();
                bslmt::ThreadUtil::microSleep(k_SHORT_SLEEP);
                ASSERT(2 == count);
                controlBarrier.wait();
                controlBarrier.wait();
                mX.drainQueue(id1);
                ASSERT(4 == count);
            }
            {
                if (veryVerbose) {
                    cout << "\tPool with paused queue: drain"
                         << endl;
                }
                // Reset the count
                count = 0;

                // Pause the queue again and submit another job
                mX.pauseQueue(id1);
                Func job = bdlf::BindUtil::bind(&incrementCounter, &count);
                mX.enqueueJob(id1, job);

                // Drain; should return with no effect
                mX.drain();
                ASSERT(0 == count);

                // Queue should still be paused
                mX.enqueueJob(id1, job);
                ASSERT(0 == count);
            }
            {
                if (veryVerbose) {
                    cout << "\tPool with paused queue: stop/shutdown"
                         << endl;
                }
                // Queue 1 is still paused with two jobs on it

                // Stop (jobs should not execute)
                mX.stop();
                ASSERT(0 == count);

                // Start
                mX.start();

                // Submit another job (queue should have been re-enabled even
                // though paused)
                Func job = bdlf::BindUtil::bind(&incrementCounter, &count);
                ASSERT(0 == mX.enqueueJob(id1, job));

                mX.shutdown();
                ASSERT(0 == count);
            }
        }
      } break;

      case 17: {
        // --------------------------------------------------------------------
        // TESTING UNDER STRESS
        //
        // Concerns:
        //   Clients were reporting jobs getting lost when many jobs were
        //   submitted to an MQTP with a single queue.
        //
        // Plan:
        //   Submit many jobs to a single queue MQTP, verifying that all jobs
        // get processed in the end.
        //
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Stress Test\n"
                    "===================\n";
        }

        enum {
            k_X_START = 0,
            k_X_END = 150,
            k_Y_START = -125,
            k_Y_END = 125,
            k_JOB_COUNT = (k_X_END - k_X_START) * (k_Y_END - k_Y_START)
        };

        bslma::TestAllocator ta(veryVeryVerbose);

        bslmt::ThreadAttributes threadAttrs;
        threadAttrs.setStackSize(1 << 20);      // one megabyte
        bdlmt::MultiQueueThreadPool tp(threadAttrs, 1, 1, 1000*1000, &ta);
        ASSERT(0 == tp.start());
        int queue = tp.createQueue();

        double startTime = now();

        StressJob s;
        s.s_count = 0;
        s.d_allocator_p = &ta;
        for (s.d_x = k_X_START; k_X_END > s.d_x; ++s.d_x) {
            for (s.d_y = k_Y_START; k_Y_END > s.d_y; ++s.d_y) {
                tp.enqueueJob(queue, s);
            }
        }

        tp.stop();

        double endTime = now();
        if (verbose) {
            cout << "Stress test, " << k_JOB_COUNT << " jobs, completed in " <<
                                        (endTime - startTime) << " seconds\n";
        }

        ASSERTV(s.s_count, k_JOB_COUNT, k_JOB_COUNT == s.s_count);
      }  break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING drainQueue AND drain: MORE JOBS SUBMITTED DURING DRAIN
        //
        // Concerns:
        //   That 'drainQueue()' and 'drain()' work properly when more jobs
        //   are enqueued during the drain.
        // --------------------------------------------------------------------

        if (verbose) cout << "Complex test of drainQueue and drain\n";

        bslma::TestAllocator ta(veryVeryVerbose);

#ifndef BSLS_PLATFORM_OS_CYGWIN
        const int k_NUM_QUEUES = 9;
#else
        const int k_NUM_QUEUES = 5;
#endif

        enum {
            k_MIN_THREADS = 1,
            k_MAX_THREADS = k_NUM_QUEUES + 1,
            k_MAX_IDLE    = 60000    // milliseconds
        };
        bslmt::ThreadAttributes defaultAttrs;

        // first do 'drainQueue()' case
        {
            Obj mX(defaultAttrs,
                   k_MIN_THREADS,
                   k_MAX_THREADS,
                   k_MAX_IDLE,
                   &ta);
            const Obj& X = mX;
            ASSERT(0 == mX.start());

            enum {
                k_REPRODUCE_COUNT = 10
            };

            bsl::vector<int> queueIds;

            const double SLEEP_A_LOT_TIME    = 1.0;
            Sleeper sleepALot(SLEEP_A_LOT_TIME);
            Sleeper::s_finished = 0;

            Reproducer reproducer(&mX, &queueIds, 2, 0);
            Reproducer::s_counter = k_REPRODUCE_COUNT;

            for (int i = 0; i < k_NUM_QUEUES; ++i) {
                int queueId = mX.createQueue();                ASSERT(queueId);
                queueIds.push_back(queueId);
            }
            for (int i = 0; i < k_NUM_QUEUES; ++i) {
                if (2 == i) {
                    mX.enqueueJob(queueIds[i], reproducer);
                }
                else {
                    mX.enqueueJob(queueIds[i], sleepALot);
                }
            }
            ASSERT(0 == Sleeper::s_finished);
            ASSERTV(Reproducer::s_counter, 0 <  Reproducer::s_counter);

            mX.drainQueue(queueIds[2]);
            ASSERTV(Reproducer::s_counter, 0 == Reproducer::s_counter);
            ASSERT(0 == Sleeper::s_finished);

            int numDequeued, numEnqueued;
            X.numProcessed(&numDequeued, &numEnqueued);

            ASSERT(k_NUM_QUEUES + k_REPRODUCE_COUNT == numEnqueued);

            mX.stop();
            ASSERT(k_NUM_QUEUES + k_REPRODUCE_COUNT == numDequeued);
            ASSERT(k_NUM_QUEUES - 1 == Sleeper::s_finished);
        }

        // next do 'drain' case
        {
            Obj mX(defaultAttrs,
                   k_MIN_THREADS,
                   k_MAX_THREADS,
                   k_MAX_IDLE,
                   &ta);
            ASSERT(0 == mX.start());

            enum {
                k_REPRODUCE_COUNT = 50
            };

            bsl::vector<int> queueIds;

            Reproducer reproducer(&mX, &queueIds, 1, 1);
            Reproducer::s_counter = k_REPRODUCE_COUNT;

            for (int i = 0; i < k_NUM_QUEUES; ++i) {
                int queueId = mX.createQueue();                ASSERT(queueId);
                queueIds.push_back(queueId);
            }
            mX.enqueueJob(queueIds[0], reproducer);
            ASSERTV(Reproducer::s_counter, 0 <  Reproducer::s_counter);

            mX.drain();
            ASSERTV(Reproducer::s_counter, 0 == Reproducer::s_counter);

            int numDequeued, numEnqueued;
            mX.numProcessed(&numDequeued, &numEnqueued);

            ASSERT(1 + k_REPRODUCE_COUNT == numEnqueued);
            ASSERT(1 + k_REPRODUCE_COUNT == numDequeued);
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING drainQueue AND drain: SIMPLE CASE
        //
        // Repeat of test case 14 with different value passed to 'concurrency'
        // --------------------------------------------------------------------

        using namespace MULTIQUEUETHREADPOOL_CASE_14;

        if (verbose) cout << "Simple test of drainQueue and drain 2\n";

        bslma::TestAllocator ta(veryVeryVerbose);

        testDrainQueueAndDrain(&ta, k_NUM_QUEUES + 1);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING drainQueue AND drain: SIMPLE CASE
        //
        // Concerns:
        //   That 'drainQueue()' and 'drain()' work properly.
        //
        // Plan:
        //   A functor 'Sleeper' is created, an object of which, when invoked,
        //   will sleep for a configured number of seconds, then increment
        //   a static atomic counter 's_finished'.
        //   First submit long sleeping sleepers to all queues but one, and
        //   submit a short sleeping sleeper to that queue.  Call 'drainQueue'
        //   on the queue with the short sleeper.  Verify that 'drainQueue'
        //   finishes before the long sleeping sleepers have finished.
        //   Now, to verify the queue was not permanently disabled by the
        //   calls to 'drainQueue' or 'drain', submit some very fast sleepers
        //   to all queues, stop, and verify that they all finished.
        //   Now, put a short lived job in a queue, disable that queue, then
        //   drain that queue.  Put a short lived job in another queue, disable
        //   that queue.  Then try to enqueue jobs to all queues, verifying
        //   that we only succeed in the queues that we expect to be enabled,
        //   to verify that 'drainQueue' left the enabled state as it found it.
        //   Then drain and verify that the expected number of jobs have
        //   finished.  Again try to submit jobs to all queues to verify that
        //   'drain' left the enabled state as it found it.
        //   Stop the threadpool, then call 'drain' and 'drainQueue' just to
        //   verify that they return immediate without segmentation faulting or
        //   aborting when called on a stopped queue.
        //   This test is called with one level of concurrency in this case,
        //   with another level in case 15.  The test was taking too long to
        //   both tests serially in one case.
        //
        // Testing:
        //   drainQueue()
        //   drain()
        // --------------------------------------------------------------------

        using namespace MULTIQUEUETHREADPOOL_CASE_14;

        if (verbose) cout << "Simple test of drainQueue and drain\n";

        bslma::TestAllocator ta(veryVeryVerbose);

        testDrainQueueAndDrain(&ta, 1);
      }  break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING numElements, numProcessed, AND numProcessedReset
        //
        // Concerns:
        //   The return values for 'numElements', 'numProcessed', and
        //   'numProcessedReset' are as expected.
        //
        // Plan:
        //   In a loop over the same object under test, start the pool,
        //   enqueue a number of quantities into different queues, then
        //   drain the pool.  Verify that numProcessed and numProcessedReset
        //   return values as expected.
        //
        // Testing:
        //   void numElements() const;
        //   void numProcessed(int *, int *, int * = 0) const;
        //   void numProcessedReset(int *, int *, int * = 0);
        // --------------------------------------------------------------------

        if (verbose)
            cout <<
               "TESTING 'numElements', 'numProcessed', AND 'numProcessedReset'"
                 << endl
                 <<
               "=============================================================="
                 << endl;

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            enum {
#ifndef BSLS_PLATFORM_OS_CYGWIN
                k_NUM_QUEUES = 10,
                k_NUM_ITERATIONS = 10,
                k_NUM_JOBS = 200
#else
                k_NUM_QUEUES = 5,
                k_NUM_ITERATIONS = 10,
                k_NUM_JOBS = 50
#endif
            };

            enum {
                k_MIN_THREADS = 1,
                k_MAX_THREADS = k_NUM_QUEUES + 1,
                k_MAX_IDLE    = 60000    // milliseconds
            };
            bslmt::ThreadAttributes defaultAttrs;

            Obj mX(defaultAttrs,
                   k_MIN_THREADS,
                   k_MAX_THREADS,
                   k_MAX_IDLE,
                   &ta);
            const Obj& X = mX;

            // verify methods without optional 'numDeleted'

            for (int i = 1; i <= k_NUM_ITERATIONS; ++i) {
                int numEnqueued = -1, numExecuted = -1;
                X.numProcessed(&numExecuted, &numEnqueued);
                ASSERT(0 == numExecuted);
                ASSERT(0 == numEnqueued);

                ASSERT(numEnqueued - numExecuted == X.numElements());

                if (veryVerbose)
                    cout << "Iteration " << i << "\n";
                ASSERT(0 == mX.start());
                int  QUEUE_IDS[k_NUM_QUEUES];
                Func QUEUE_NOOP[k_NUM_QUEUES];

                bslmt::Barrier barrier(1 + k_NUM_QUEUES);
                Func           block;  // blocks on barrier
                makeFunc(&block, waitOnBarrier, &barrier, 1);

                for (int j = 0; j < k_NUM_QUEUES; ++j) {
                    QUEUE_NOOP[j] = Func(&noop);
                    int id = QUEUE_IDS[j] = mX.createQueue();
                    ASSERTV(i, j, 0 == mX.enqueueJob(id, block));
                }
                ASSERT(k_NUM_QUEUES == X.numQueues());

                for (int j = 0; j < i * k_NUM_JOBS; ++j) {
                    for (int k = 0; k < k_NUM_QUEUES; ++k) {
                        ASSERTV(i, j, k,
                              0 == mX.enqueueJob(QUEUE_IDS[k], QUEUE_NOOP[k]));
                    }
                }

                if (veryVerbose)
                    cout << "   waiting for barrier\n";
                barrier.wait();
                if (veryVerbose)
                    cout << "   passed barrier\n";

                int numElements = X.numElements();
                X.numProcessed(&numExecuted, &numEnqueued);

                ASSERTV(i,
                        k_NUM_QUEUES * (1 + i * k_NUM_JOBS),
                        numEnqueued,
                        k_NUM_QUEUES * (1 + i * k_NUM_JOBS)
                                                               == numEnqueued);
                ASSERTV(i, 0 <  numExecuted);
                // on Linux, IBM, and Solaris 10, code is so optimized that
                // sometimes equality holds (the pool is already drained)
                ASSERTV(i, numExecuted <= numEnqueued);

                ASSERT(numEnqueued >= X.numElements());

                ASSERT(numEnqueued - numExecuted <= numElements);
                ASSERT(numEnqueued - numExecuted >= X.numElements());

                if (veryVerbose)
                    cout << "   waiting for threadpool stop\n";
                mX.stop();
                if (veryVerbose)
                    cout << "   threadpool stopped\n";
                X.numProcessed(&numExecuted, &numEnqueued);

                ASSERTV(i,
                             k_NUM_QUEUES * (1 + i * k_NUM_JOBS),
                             numEnqueued,
                             k_NUM_QUEUES * (1 + i * k_NUM_JOBS)
                                                               == numEnqueued);
                ASSERTV(i, numExecuted == numEnqueued);

                ASSERT(0 == X.numElements());

                int numEnqueued2 = -1, numExecuted2 = -1;
                mX.numProcessedReset(&numExecuted2, &numEnqueued2);
                ASSERTV(i, numEnqueued2 == numEnqueued);
                ASSERTV(i, numExecuted2 == numExecuted);

                X.numProcessed(&numExecuted, &numEnqueued);
                ASSERTV(i, 0 == numExecuted);
                ASSERTV(i, 0 == numEnqueued);

                ASSERT(0 == X.numElements());

                mX.start();

                for (int j = 0; j < k_NUM_QUEUES; ++j) {
                    int rc = mX.deleteQueue(QUEUE_IDS[j]);
                    ASSERT(0 == rc);
                }

                mX.stop();
                mX.numProcessed(&numExecuted, &numEnqueued);
                ASSERTV(i, numExecuted, 0 == numExecuted);
                ASSERTV(i, numEnqueued, 0 == numEnqueued);

                ASSERT(0 == X.numElements());
            }

            // verify methods with optional 'numDeleted'

            for (int i = 1; i <= k_NUM_ITERATIONS; ++i) {
                int numEnqueued = -1, numExecuted = -1, numDeleted = -1;
                X.numProcessed(&numExecuted, &numEnqueued, &numDeleted);
                ASSERT(0 == numExecuted);
                ASSERT(0 == numEnqueued);
                ASSERT(0 == numDeleted);

                ASSERT(numEnqueued - numExecuted - numDeleted ==
                                                              X.numElements());

                if (veryVerbose)
                    cout << "Iteration " << i << "\n";
                ASSERT(0 == mX.start());
                int  QUEUE_IDS[k_NUM_QUEUES];
                Func QUEUE_NOOP[k_NUM_QUEUES];

                bslmt::Barrier barrier(1 + k_NUM_QUEUES);
                Func           block;  // blocks on barrier
                makeFunc(&block, waitOnBarrier, &barrier, 1);

                for (int j = 0; j < k_NUM_QUEUES; ++j) {
                    QUEUE_NOOP[j] = Func(&noop);
                    int id = QUEUE_IDS[j] = mX.createQueue();
                    ASSERTV(i, j, 0 == mX.enqueueJob(id, block));
                }
                ASSERT(k_NUM_QUEUES == X.numQueues());

                for (int j = 0; j < i * k_NUM_JOBS; ++j) {
                    for (int k = 0; k < k_NUM_QUEUES; ++k) {
                        ASSERTV(i, j, k,
                              0 == mX.enqueueJob(QUEUE_IDS[k], QUEUE_NOOP[k]));
                    }
                }

                if (veryVerbose)
                    cout << "   waiting for barrier\n";
                barrier.wait();
                if (veryVerbose)
                    cout << "   passed barrier\n";

                int numElements = X.numElements();
                X.numProcessed(&numExecuted, &numEnqueued, &numDeleted);

                ASSERTV(i,
                        k_NUM_QUEUES * (1 + i * k_NUM_JOBS),
                        numEnqueued,
                        k_NUM_QUEUES * (1 + i * k_NUM_JOBS)
                                                               == numEnqueued);
                ASSERTV(i, 0 <  numExecuted);
                ASSERTV(i, 0 == numDeleted);
                // on Linux, IBM, and Solaris 10, code is so optimized that
                // sometimes equality holds (the pool is already drained)
                ASSERTV(i, numExecuted <= numEnqueued);

                ASSERT(numEnqueued >= X.numElements());

                ASSERT(numEnqueued - numExecuted - numDeleted <= numElements);
                ASSERT(numEnqueued - numExecuted - numDeleted >=
                                                              X.numElements());

                if (veryVerbose)
                    cout << "   waiting for threadpool stop\n";
                mX.stop();
                if (veryVerbose)
                    cout << "   threadpool stopped\n";
                X.numProcessed(&numExecuted, &numEnqueued, &numDeleted);

                ASSERTV(i,
                        k_NUM_QUEUES * (1 + i * k_NUM_JOBS),
                        numEnqueued,
                        k_NUM_QUEUES * (1 + i * k_NUM_JOBS)
                                                               == numEnqueued);
                ASSERTV(i, numExecuted == numEnqueued);
                ASSERTV(i, 0 == numDeleted);

                ASSERT(0 == X.numElements());

                int numEnqueued2 = -1, numExecuted2 = -1, numDeleted2 = -1;
                mX.numProcessedReset(&numExecuted2,
                                     &numEnqueued2,
                                     &numDeleted2);
                ASSERTV(i, numEnqueued2 == numEnqueued);
                ASSERTV(i, numExecuted2 == numExecuted);
                ASSERTV(i, numDeleted2  == numDeleted);

                X.numProcessed(&numExecuted, &numEnqueued, &numDeleted);
                ASSERTV(i, 0 == numExecuted);
                ASSERTV(i, 0 == numEnqueued);
                ASSERTV(i, 0 == numDeleted);

                ASSERT(0 == X.numElements());

                mX.start();

                for (int j = 0; j < k_NUM_QUEUES; ++j) {
                    int rc = mX.deleteQueue(QUEUE_IDS[j]);
                    ASSERT(0 == rc);
                }

                mX.stop();
                mX.numProcessed(&numExecuted, &numEnqueued, &numDeleted);
                ASSERTV(i, numExecuted, 0 == numExecuted);
                ASSERTV(i, numEnqueued, 0 == numEnqueued);
                ASSERTV(i, numDeleted,  0 == numDeleted);

                ASSERT(0 == X.numElements());
            }

            // verify 'deleteQueue' affects 'numDeleted' as expected

            for (int i = 1; i <= k_NUM_ITERATIONS; ++i) {
                bslmt::Barrier barrier(2);
                Func           block;  // blocks on barrier
                makeFunc(&block, waitOnBarrier, &barrier, 2);

                int id = mX.createQueue();

                mX.start();

                int numEnqueued = -1, numExecuted = -1, numDeleted = -1;
                mX.numProcessedReset(&numExecuted, &numEnqueued, &numDeleted);

                {
                    for (int j = 0; j < k_NUM_JOBS; ++j) {
                        ASSERT(0 == mX.enqueueJob(id, noop));
                    }
                    ASSERT(0 == mX.enqueueJob(id, block));
                    for (int j = 0; j < i; ++j) {
                        ASSERT(0 == mX.enqueueJob(id, noop));
                    }
                }

                int numElements = X.numElements();
                X.numProcessed(&numExecuted, &numEnqueued, &numDeleted);

                ASSERT(k_NUM_JOBS + 1 + i == numEnqueued);
                ASSERT(                 0 <= numExecuted);
                ASSERT(                 0 == numDeleted);

                ASSERT(numEnqueued               >= X.numElements());
                ASSERT(numEnqueued - numExecuted <= numElements);
                ASSERT(numEnqueued - numExecuted >= X.numElements());

                barrier.wait();

                X.numProcessed(&numExecuted, &numEnqueued, &numDeleted);

                ASSERT(k_NUM_JOBS + 1 + i == numEnqueued);
                ASSERT(k_NUM_JOBS + 1     == numExecuted);
                ASSERT(                 0 == numDeleted);

                ASSERT(numEnqueued - numExecuted == X.numElements());

                mX.deleteQueue(id, noop);

                barrier.wait();

                mX.drain();

                X.numProcessed(&numExecuted, &numEnqueued, &numDeleted);

                ASSERT(k_NUM_JOBS + 1 + i == numEnqueued);
                ASSERT(k_NUM_JOBS + 1     == numExecuted);
                ASSERT(                 i == numDeleted);

                ASSERT(numEnqueued - numExecuted - numDeleted ==
                                                              X.numElements());

                mX.stop();
            }
        }

        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: CLEANUP CALLBACK DOES NOT DEADLOCK
        //
        // Concerns:
        //   * That a cleanup callback which enqueues into a second queue
        //     managed by the MQTP does not deadlock.
        //
        //   * That a cleanup callback that deletes a second queue managed by
        //     the MQTP does not deadlock.
        //
        // Plan:
        //   Instantiate a modifiable 'bdlmt::MultiQueueThreadPool' object, and
        //   create two queues, identified as 'id1', and 'id2'.  Delete 'id1'
        //   using a cleanup callback which enqueues a job to 'id2'.  Create
        //   queue 'id3', and delete it with a callback that deletes 'id2'.
        //
        // Testing:
        //   Concern: cleanup callback does not deadlock.
        // --------------------------------------------------------------------

        if (verbose) {
           cout << "Concern: Cleanup Callback Does Not Deadlock" << endl
                << "===========================================" << endl;
        }

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            enum {
                k_MIN_THREADS = 2,
                k_MAX_THREADS = 2,
                k_MAX_IDLE    = 60000    // milliseconds
            };
            bslmt::ThreadAttributes defaultAttrs;

            Obj mX(defaultAttrs,
                   k_MIN_THREADS,
                   k_MAX_THREADS,
                   k_MAX_IDLE,
                   &ta);
            const Obj& X = mX;

            bsls::AtomicInt counter(0);
            Func            count;        // increment 'counter'
            makeFunc(&count, incrementCounter, &counter);

            ASSERT(0 == mX.start());

            int id1 = mX.createQueue();  ASSERT(0 != id1);
            int id2 = mX.createQueue();  ASSERT(0 != id2);  ASSERT(id1 != id2);
            ASSERT(2 == X.numQueues());

            if (veryVerbose) {
                cout << "\tCreated queues " << id1 << " and " << id2 << endl;
            }

            Func           cleanupCb;
            bslmt::Barrier barrier(2);
            makeFunc<bdlmt::MultiQueueThreadPool *,
                     int,
                     const Func&,
                     bslmt::Barrier *>(&cleanupCb,
                                       case12EnqueueJob,
                                       &mX,
                                       id2,
                                       count,
                                       &barrier);
            // The clean up callback for queue 'id1' will increment 'counter'.

            if (veryVerbose) {
                cout << "\tDeleting queue " << id1 << endl;
            }
            ASSERT(0 == mX.deleteQueue(id1, cleanupCb));
            barrier.wait();

            // The 'barrier.wait()' is insufficient to verify 'count' has
            // completed since 'deleteQueue' does not wait for the queue to be
            // actually deleted (and 'count' invoked).  Hence, if the increment
            // of 'counter' is not verified, the deletion of the queue
            // associated with 'id2' (below) may occur before the 'count'
            // callback executes and the counter will not be incremented as
            // expected.

            for (int i = 0; i < 10 && 1 != counter; ++i) {  // SPIN
                bslmt::ThreadUtil::microSleep(100000);
            }
            ASSERT(1 == counter);

            ASSERT(1 == X.numQueues());  // id2 is still active

            int id3 = mX.createQueue();  ASSERT(0 != id3);  ASSERT(id2 != id3);
            if (veryVerbose) {
                cout << "\tCreated queue " << id3 << endl;
            }
            ASSERT(2 == X.numQueues());  // id2 and id3
            makeFunc<bdlmt::MultiQueueThreadPool *,
                     int,
                     const Func&,
                     bslmt::Barrier *>(&cleanupCb,
                                       case12DeleteQueue,
                                       &mX,
                                       id2,
                                       count,
                                       &barrier);
            if (veryVerbose) {
                cout << "\tDeleting queue " << id3 << endl;
            }
            // The clean up callback for queue 'id3' will increment 'counter'.
            ASSERT(0 == mX.deleteQueue(id3, cleanupCb));
            barrier.wait();

            for (int i = 0; i < 10 && 2 != counter; ++i) {  // SPIN
                bslmt::ThreadUtil::microSleep(100000);
            }
            ASSERT(2 == counter);

            ASSERT(0 == mX.numQueues());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: 'deleteQueue' DOES NOT BLOCK THE CALLER
        //
        // Concerns:
        //   * That stopping one MQTP does not affect the other.
        //
        //   * That disabling one MQTP does not affect the other.
        //
        //   * That both MQTPs can process jobs concurrently.
        //
        //   * That destroying one MQTP does not affect the other
        //
        // Plan:
        //   TBD.
        //
        // Testing:
        //   Concern: 'deleteQueue' blocks the caller.
        // --------------------------------------------------------------------

        if (verbose) {
           cout << "Concern: 'deleteQueue' Does NOT Block the Caller." << endl
                << "=================================================" << endl;
        }

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            enum {
                k_MIN_THREADS = 1,
                k_MAX_THREADS = 1,
                k_MAX_IDLE    = 60000    // milliseconds
            };
            bslmt::ThreadAttributes defaultAttrs;

            Obj mX(defaultAttrs,
                   k_MIN_THREADS,
                   k_MAX_THREADS,
                   k_MAX_IDLE,
                   &ta);

            int id = 0;
            bsls::AtomicInt counter(0);
            Func            cleanupCb;
            bslmt::Barrier  barrier(2);
            makeFunc(&cleanupCb, case11CleanUp, &counter, &barrier);

            enum { k_NUM_ITERATIONS = 500 };
            ASSERT(0 == mX.start());
            for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
                id = mX.createQueue();
                ASSERTV(i, 0 != id);
                ASSERTV(i, 0 == mX.deleteQueue(id, cleanupCb));
                ASSERTV(i, 0 == counter);
                barrier.wait();
                barrier.wait();
                ASSERTV(i, counter == 1);
                counter = 0;
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: ONE 'bdlmt::ThreadPool' CAN BE SHARED BY TWO MQTPS
        //
        // Concerns:
        //   * That stopping one MQTP does not affect the other.
        //
        //   * That disabling one MQTP does not affect the other.
        //
        //   * That both MQTPs can process jobs concurrently.
        //
        //   * That destroying one MQTP does not affect the other
        //
        // Plan:
        //   TBD.
        //
        // Testing:
        //   Concern: One 'bcep::Threadpool' can be shared by two MQTPs.
        // --------------------------------------------------------------------

        if (verbose) {
           cout << "Concern: One 'bcep::Threadpool' Can Be Shared By Two MQTPs"
                << endl
                << "=========================================================="
                << endl;
        }

        bslma::TestAllocator ta(veryVeryVerbose);

        enum {
            k_MIN_THREADS = 1,
            k_NUM_QUEUES = 5,
            k_MAX_THREADS = k_NUM_QUEUES * 2,
            k_MAX_IDLE = 60000    // milliseconds
        };

        bdlmt::ThreadPool tp(bslmt::ThreadAttributes(),
                             k_MIN_THREADS,
                             k_MAX_THREADS,
                             k_MAX_IDLE,
                             &ta);
        Obj *pMX = new (ta) Obj(&tp, &ta);
        Obj& mX = *pMX;         const Obj& X = mX;
        Obj mY(&tp, &ta);       const Obj& Y = mY;

        int queueIds[k_NUM_QUEUES][2];
        enum { k_IX = 0, k_IY = 1 };

        const double SLEEP_A_LITTLE_TIME = 0.50;
        const double SLEEP_A_LOT_TIME    = 1.00;

        Sleeper sleepALittle(SLEEP_A_LITTLE_TIME);
        Sleeper sleepALot(   SLEEP_A_LOT_TIME);

        // less than k_MAX_THREADS + 1
        ASSERT(0 == tp.start());
        ASSERT(0 == mX.start());
        ASSERT(0 == mY.start());
        double startTime = now();

        for (int i = 0; i < k_NUM_QUEUES; ++i) {
            queueIds[i][k_IX] = mX.createQueue();
            ASSERT(0 != queueIds[i][k_IX]);
            queueIds[i][k_IY] = mY.createQueue();
            ASSERT(0 != queueIds[i][k_IY]);
        }

        for (int i = 0; i < k_NUM_QUEUES; ++i) {
            ASSERT(0 == mX.enqueueJob(queueIds[i][k_IX], sleepALittle));
            ASSERT(0 == mY.enqueueJob(queueIds[i][k_IY], sleepALittle));
        }
        ASSERTV(Sleeper::s_finished, 0 == Sleeper::s_finished);

        int numExecuted, numEnqueued;
        X.numProcessed(&numExecuted, &numEnqueued);
        ASSERTV(numEnqueued, k_NUM_QUEUES == numEnqueued);
        Y.numProcessed(&numExecuted, &numEnqueued);
        ASSERTV(numEnqueued, k_NUM_QUEUES == numEnqueued);

        mX.stop();
        mY.stop();
        ASSERTV(Sleeper::s_finished,
                2 * k_NUM_QUEUES == Sleeper::s_finished);
        double time = now() - startTime;
        ASSERTV(time, time >= SLEEP_A_LITTLE_TIME - jumpTheGun);

        X.numProcessed(&numExecuted, &numEnqueued);
        ASSERTV(numEnqueued, k_NUM_QUEUES == numEnqueued);
        ASSERTV(numExecuted, k_NUM_QUEUES == numExecuted);
        Y.numProcessed(&numExecuted, &numEnqueued);
        ASSERTV(numEnqueued, k_NUM_QUEUES == numEnqueued);
        ASSERTV(numExecuted, k_NUM_QUEUES == numExecuted);

        Sleeper::s_finished = 0;

        ASSERT(0 == mX.start());
        ASSERT(0 == mY.start());

        startTime = now();
        ASSERT(0 == mX.enqueueJob(queueIds[0][k_IX], sleepALittle));
        for (int i = 0; i < k_NUM_QUEUES; ++i) {
            ASSERT(0 == mX.disableQueue(queueIds[i][k_IX]));
        }
        for (int i = 0; i < k_NUM_QUEUES; ++i) {
            ASSERT(0 != mX.enqueueJob(queueIds[i][k_IX], sleepALittle));
            ASSERT(0 == mY.enqueueJob(queueIds[i][k_IY], sleepALot));
        }
        ASSERTV(Sleeper::s_finished, 0 == Sleeper::s_finished);

        mX.stop();
        ASSERTV(Sleeper::s_finished, 1 <= Sleeper::s_finished);
        time = now() - startTime;
        ASSERTV(time, time >= SLEEP_A_LITTLE_TIME - jumpTheGun);

        ta.deleteObjectRaw(pMX);

        for (int i = 0; i < k_NUM_QUEUES; ++i) {
            ASSERT(0 == mY.enqueueJob(queueIds[i][k_IY], sleepALittle));
        }

        mY.stop();
        ASSERTV(Sleeper::s_finished,
                2 * k_NUM_QUEUES + 1 == Sleeper::s_finished);
        ASSERTV(now() - startTime, now() - startTime >=
                          SLEEP_A_LOT_TIME + SLEEP_A_LITTLE_TIME - jumpTheGun);

        tp.stop();
      }  break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: MT-SAFETY OF PUBLIC API AND PROCESSING
        //
        // Concerns:
        //   * That 'enqueueJob', 'deleteQueue', and 'processQueueCb' are
        //     thread-safe.
        //
        //   * That queues are processed serially.
        //
        // Plan:
        //   Iterate over a number of test vectors varying in the minimum and
        //   maximum number of threads used to create a 'bdlmt::ThreadPool',
        //   and the number of queues to create.  For each test vector, create
        //   a modifiable 'bdlmt::MultiQueueThreadPool' object 'mX', and a
        //   non-modifiable reference to 'mX' named 'X'.  Start 'mX'.  Create
        //   the specified number of queues, and enqueue a fixed number of jobs
        //   to each queue.  Each job (represented as a functor) increments a
        //   counter, and appends it to a list.  (the address of the counter
        //   and the address of the list are bound to the functor.)  Verify
        //   the number of queues.  Then, stop 'mX', and verify the results.
        //
        // Testing:
        //   Multi-threaded safety of public API and processing
        // --------------------------------------------------------------------

        if (verbose) {
          cout
           << "Concern: Multi-Threaded Safety of Public API and Processing"
           << endl
           << "==========================================================="
           << endl;
        }

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            const struct {
                int d_line;          // source line number
                int d_minThreads;    // minimum number of threads in pool
                int d_maxThreads;    // maximum number of threads in pool
                int d_numQueues;     // number of queues to create
                int d_numJobs;       // number of jobs to enqueue
            } DATA[] = {
                //Line  Min Threads  Max Threads  Num Queues  Num Jobs
                //----  -----------  -----------  ----------  --------
                { L_,   1,           1,           1,          1,      },
                { L_,   1,           1,           1,          2,      },
                { L_,   1,           1,           1,          8,      },
                { L_,   1,           1,           1,          64,     },
                { L_,   1,           1,           1,          256,    },

                { L_,   1,           1,           2,          1,      },
                { L_,   1,           1,           2,          2,      },
                { L_,   1,           1,           2,          8,      },
                { L_,   1,           1,           2,          64,     },
                { L_,   1,           1,           2,          256,    },

                { L_,   1,           1,           8,          1,      },
                { L_,   1,           1,           8,          2,      },
                { L_,   1,           1,           8,          8,      },
                { L_,   1,           1,           8,          64,     },
                { L_,   1,           1,           8,          256,    },

                { L_,   1,           1,           128,        1,      },
                { L_,   1,           1,           128,        2,      },
                { L_,   1,           1,           128,        8,      },
                { L_,   1,           1,           128,        64,     },
                { L_,   1,           1,           128,        256,    },

#if !defined(BSLS_PLATFORM_OS_DARWIN) \
 || !defined(BSLS_PLATFORM_CMP_CLANG) || BSLS_PLATFORM_CMP_VERSION > 70300

                // Darwin had a problem with creating a lot of semaphores.

                { L_,   1,           1,           256,        1,      },
                { L_,   1,           1,           256,        2,      },
                { L_,   1,           1,           256,        8,      },
                { L_,   1,           1,           256,        64,     },
                { L_,   1,           1,           256,        256,    },
#endif

                { L_,   1,           2,           1,          1,      },
                { L_,   2,           2,           1,          2,      },
                { L_,   4,           4,           1,          8,      },
                { L_,   8,           8,           1,          64,     },
                { L_,   16,          16,          1,          256,    },

                { L_,   2,           2,           2,          1,      },
                { L_,   2,           2,           2,          2,      },
                { L_,   2,           2,           2,          8,      },
                { L_,   2,           2,           2,          64,     },
                { L_,   2,           2,           2,          256,    },

                { L_,   1,           8,           8,          1,      },
                { L_,   2,           8,           8,          2,      },
                { L_,   4,           8,           8,          8,      },
                { L_,   6,           8,           8,          64,     },
                { L_,   8,           8,           8,          256,    },

                { L_,   1,           16,          128,        1,      },
                { L_,   2,           16,          128,        2,      },
                { L_,   4,           16,          128,        8,      },
                { L_,   8,           16,          128,        64,     },
                { L_,   16,          16,          128,        256,    },

#if !defined(BSLS_PLATFORM_OS_DARWIN) \
 || !defined(BSLS_PLATFORM_CMP_CLANG) || BSLS_PLATFORM_CMP_VERSION > 70300

                // Darwin had a problem with creating a lot of semaphores.

                { L_,   1,           16,          256,        1,      },
                { L_,   2,           16,          256,        2,      },
                { L_,   4,           16,          256,        8,      },
                { L_,   8,           16,          256,        64,     },
                { L_,   16,          16,          256,        256,    },
#endif
            };
            enum { k_DATA_SIZE = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < k_DATA_SIZE; ++i) {
                const int LINE          = DATA[i].d_line;
                const int k_MIN_THREADS = DATA[i].d_minThreads;
                const int k_MAX_THREADS = DATA[i].d_maxThreads;
                const int k_NUM_QUEUES  = DATA[i].d_numQueues;
                const int k_NUM_JOBS    = DATA[i].d_numJobs;
                const int k_MAX_IDLE    = 1000;  // milliseconds

                bslmt::ThreadAttributes defaultAttrs;

                Obj mX(defaultAttrs,
                       k_MIN_THREADS,
                       k_MAX_THREADS,
                       k_MAX_IDLE,
                       &ta);
                const Obj& X = mX;
                ASSERTV(i, LINE, 0 == X.numQueues());
                ASSERT(0 == mX.start());

                if (verbose) {
                    P_(i); P_(LINE);
                    P_(k_MIN_THREADS); P_(k_MAX_THREADS);
                    P_(k_NUM_QUEUES);  P(k_NUM_JOBS);
                }

                bsl::vector<bsl::vector<int> > results(k_NUM_QUEUES);
                bsl::vector<bsls::AtomicInt>    counters(k_NUM_QUEUES);

                // Create queues and enqueue jobs.
                for (int j = 0; j < k_NUM_QUEUES; ++j) {
                    int id = mX.createQueue();
                    ASSERTV(i, LINE, j, 0 != id);
                    ASSERTV(i, LINE, j, true == results[j].empty());

                    for (int k = 0; k < k_NUM_JOBS; ++k) {
                        Func job;
                        makeFunc(&job,
                                 case9Callback,
                                 &counters[j],
                                 &results[j]);
                        ASSERTV(i, LINE, j, k,
                                0 == mX.enqueueJob(id, job));
                    }
                }
                ASSERTV(i, LINE, k_NUM_QUEUES == X.numQueues());

                mX.stop();
                ASSERTV(i, LINE, k_NUM_QUEUES == X.numQueues());
                ASSERTV(i, LINE, 0 <  X.threadPool().numWaitingThreads());
                ASSERTV(i, LINE, 0 == X.threadPool().numActiveThreads());

                mX.shutdown();
                ASSERTV(i, LINE, 0 == X.numQueues());
                ASSERTV(i, LINE, 0 == X.threadPool().numWaitingThreads());
                ASSERTV(i, LINE, 0 == X.threadPool().numActiveThreads());

                // Verify results.
                for (int j = 0; j < k_NUM_QUEUES; ++j) {
                    ASSERTV(i, LINE, j,
                            k_NUM_JOBS == (int)results[j].size());
                    for (int k = 0; k < k_NUM_JOBS; ++k) {
                        const int VALUE = k + 1;
                        ASSERTV(i, LINE, j, k,
                                VALUE == results[j].at(k));
                    }
                }
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: 'deleteQueueCb' IS THE LAST CALLBACK PROCESSED
        //
        // Concerns:
        //   * That no other enqueued jobs are processed after 'deleteQueue' is
        //     called.
        //
        //   * That after 'deleteQueue' is called, no other jobs can be
        //     enqueued.
        //
        // Plan:
        //   Create a modifiable 'bdlmt::MultiQueueThreadPool', 'mX', and a
        //   non-modifiable reference to 'mX' named 'X'.  Enqueue a job to 'mX'
        //   which blocks on a barrier.  Then, enqueue several jobs to 'mX'
        //   which increment a counter, initialized to 0.  Using 'X', verify
        //   that there are a non-zero number of queued jobs.  Then, call
        //   'deleteQueue' on 'mX'.  Verify that no other jobs can be enqueued
        //   to 'mX'.  Then, unblock the barrier, and verify that none of the
        //   jobs which increment the counter were executed.
        //
        // Testing:
        //   CONCERN: 'deleteQueueCb' is the last callback processed
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Concern: 'deleteQueueCb' Is the Last Callback Processed"
                 << endl
                 << "======================================================="
                 << endl;
        }

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            enum {
                k_MIN_THREADS = 2,
                k_MAX_THREADS = 4,
                k_MAX_IDLE    = 60000    // milliseconds
            };
            bslmt::ThreadAttributes defaultAttrs;
            bslmt::Barrier   barrier(2);
            bsls::AtomicInt  counter(0);
            Func             cleanupCb;  // empty callback
            Func             block;      // blocks on 'barrier'
            Func             count;      // increments 'counter'
            makeFunc(&cleanupCb, waitOnBarrier, &barrier, 1);
            makeFunc(&block, waitOnBarrier, &barrier, 1);
            makeFunc(&count, incrementCounter, &counter);
            Obj mX(defaultAttrs,
                   k_MIN_THREADS,
                   k_MAX_THREADS,
                   k_MAX_IDLE,
                   &ta);
            const Obj& X = mX;

            ASSERT(0 == mX.start());
            int id = mX.createQueue();
            ASSERT(0 != id);
            ASSERT(1 == X.numQueues());
            ASSERT(0 == X.numElements(id));
            ASSERT(0 == counter);
            ASSERT(0 == mX.enqueueJob(id, block));
            ASSERT(0 == mX.enqueueJob(id, count));
            ASSERT(0 == mX.enqueueJob(id, count));
            ASSERT(0 == mX.enqueueJob(id, count));
            ASSERT(0 == mX.enqueueJob(id, count));
            ASSERT(0 == mX.enqueueJob(id, count));
            while (5 != X.numElements(id)) {
                bslmt::ThreadUtil::microSleep(250000); // trigger thread switch
                bslmt::ThreadUtil::yield();
            }
            ASSERT(5 == X.numElements(id));
            ASSERT(0 == counter);
            ASSERT(0 == mX.deleteQueue(id, cleanupCb));
            barrier.wait();                            // unblock 'barrier'
            ASSERT(0 == counter);
            ASSERT(0 != mX.enqueueJob(id, count));
            ASSERT(0 != mX.enqueueJob(id, count));
            ASSERT(0 != mX.enqueueJob(id, count));
            barrier.wait();
            ASSERT(0 == X.numQueues());
            ASSERT(0 == counter);
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ALTERNATE CONSTRUCTORS
        //
        // Concerns: That a pool constructed with an external
        // 'bdlmt::ThreadPool' behaves identically to one created with an
        // internal thread pool.
        //
        // Plan:
        //   Create a modifiable 'bdlmt::ThreadPool', 'pool', with 1 <
        //   'minThreads' and 'minThreads' < 'maxThreads'.  Create a
        //   'bdlmt::MultiQueueThreadPool', 'mX', instantiated with 'pool', and
        //   a non-modifiable reference to 'mX' named 'X'.
        //
        //   Start the pool.  Create a queue and enqueue a large number of jobs
        //   to it.  Each job, represented by a functor, increments a counter,
        //   the address of which is bound to the functor.  Stop the pool, and
        //   verify that all jobs have been processed, by comparing the value
        //   of the counter to the number of enqueued jobs.  Verify that the
        //   queue was not deleted, and that the threads are still active.
        //   Destroy 'mX', and verify that 'pool' is valid, and that all but
        //   the last thread have been destroyed.
        //
        // Note:
        //   The last thread is not necessarily closed because the threadpool
        //   is external and thus not drained upon destruction of the
        //   multipool.  This accounts for infrequent failures on various
        //   platforms.  However, there can be only one thread left active in
        //   the threadpool (corresponding to the last job on the only queue).
        //
        // Testing:
        //    bdlmt::MultiQueueThreadPool(
        //                              bdlmt::ThreadPool *threadPool,
        //                              bslma::Allocator  *basicAllocator = 0);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Alternative Constructors" << endl
                 << "================================" << endl;
        }

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            enum {
                k_MIN_THREADS = 2,
                k_MAX_THREADS = 4,
                k_IDLE        = 1000,    // milliseconds
                k_MAX_IDLE    = 60000    // milliseconds
            };
            bslmt::ThreadAttributes defaultAttrs;
            bdlmt::ThreadPool       pool(defaultAttrs,
                                         k_MIN_THREADS,
                                         k_MAX_THREADS,
                                         k_MAX_IDLE,
                                         &ta);

            {
                Obj        mX(&pool, &ta);
                const Obj& X = mX;

                const bdlmt::ThreadPool& tp = X.threadPool();
                ASSERT(&tp == &pool);
                ASSERT(k_MIN_THREADS == tp.minThreads());
                ASSERT(k_MAX_THREADS == tp.maxThreads());
                ASSERT(k_MAX_IDLE == tp.maxIdleTime());
                ASSERT(0 == tp.numPendingJobs());
                ASSERT(0 == tp.numActiveThreads());
                ASSERT(0 == tp.numWaitingThreads());

                ASSERT(k_MIN_THREADS == pool.minThreads());
                ASSERT(k_MAX_THREADS == pool.maxThreads());
                ASSERT(k_MAX_IDLE == pool.maxIdleTime());
                ASSERT(0 == pool.numPendingJobs());
                ASSERT(0 == pool.numActiveThreads());
                ASSERT(0 == pool.numWaitingThreads());

                //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                // Start pool.  Enqueue many jobs, stop the pool, and verify
                // that all elements are processed, but neither the queue nor
                // the threads are destroyed.

                pool.start();

                ASSERT(0 == mX.start());
                ASSERT(0 == tp.numPendingJobs());
                ASSERT(0 == tp.numActiveThreads());
                ASSERT(k_MIN_THREADS == tp.numWaitingThreads());

                ASSERT(0 == pool.numPendingJobs());
                ASSERT(0 == pool.numActiveThreads());
                ASSERT(k_MIN_THREADS == pool.numWaitingThreads());

                bslmt::Barrier  barrier(2);
                bsls::AtomicInt counter(0);
                Func            block;      // blocks on 'barrier'
                Func            count;      // increments 'counter'
                makeFunc(&block, waitOnBarrier, &barrier, 2);
                makeFunc(&count, incrementCounter, &counter);

                int id = mX.createQueue();
                ASSERT(0 != id);
                ASSERT(1 == X.numQueues());
                ASSERT(0 == X.numElements(id));

                enum { k_NUM_JOBS = 1000 };
                ASSERT(0 == mX.enqueueJob(id, block));
                for (int i = 0; i < k_NUM_JOBS; ++i) {
                    ASSERTV(i, 0 == mX.enqueueJob(id, count));
                }
                barrier.wait();
                ASSERT(k_NUM_JOBS == X.numElements(id));  // 'block' is
                                                          // blocking
                ASSERT(0 == counter);
                barrier.wait();

                mX.stop();
                ASSERT(k_NUM_JOBS == counter);
                ASSERT(1 == X.numQueues());
                ASSERT(0 == X.numElements(id));

                // Also give a chance to the Threadpool to kill the idle
                // threads.  But given the problematic likelihood of failure of
                // this test, only assert '1 >= tp.numActiveThreads()' when all
                // else has been exhausted.

                bslmt::ThreadUtil::microSleep(k_IDLE);
                bslmt::ThreadUtil::yield();
                if (1 < tp.numActiveThreads()) {
                    cout << "WARNING: Long delay in case 7 (still "
                         << tp.numActiveThreads() << " threads active)"
                         << endl;
                    bslmt::ThreadUtil::microSleep(k_IDLE);
                    bslmt::ThreadUtil::yield();
                }

                ASSERTV(tp.numActiveThreads(),
                        1 >= tp.numActiveThreads());
                ASSERT(k_MIN_THREADS <= tp.numActiveThreads()
                                    + tp.numWaitingThreads());
                ASSERT(0 == tp.numPendingJobs());

                ASSERT(1 >= pool.numActiveThreads());
                ASSERT(k_MIN_THREADS <= pool.numActiveThreads()
                                    + pool.numWaitingThreads());
                ASSERT(0 == pool.numPendingJobs());
            }

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Destroy the pool, and verify that the external thread pool is
            // valid, and that all but perhaps the last thread have been
            // destroyed.

            ASSERT(k_MIN_THREADS == pool.minThreads());
            ASSERT(k_MAX_THREADS == pool.maxThreads());
            ASSERT(k_MAX_IDLE == pool.maxIdleTime());
            ASSERT(0 == pool.numPendingJobs());
            ASSERT(1 >= pool.numActiveThreads());
            ASSERT(k_MIN_THREADS <= pool.numActiveThreads()
                                + pool.numWaitingThreads());
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'enableQueue' AND 'disableQueue' FUNCTIONS
        //
        // Concerns:
        //   * That 'enableQueue' and 'disableQueue' return with failure when
        //     the pool is stopped.
        //
        //   * That no jobs can be enqueued to a queue after 'disableQueue' is
        //     called for that queue.
        //
        //   * That currently enqueued jobs are processed after 'disableQueue'
        //     is called for a queue.
        //
        //   * That a disabled queue can be enabled.
        //
        //   * That jobs can be enqueued to a queue after 'enableQueue' is
        //     called for that queue.
        //
        //   * That a disabled queue can be deleted.
        //
        // Plan:
        //   Create a modifiable 'bdlmt::MultiQueueThreadPool', 'mX', and a
        //   non-modifiable reference to 'mX' named 'X'.  Verify that
        //   'enableQueue' and 'disableQueue' fail when called on
        //   non-existent and invalid queue IDs.  Create a queue, and enqueue
        //   several jobs to it, starting with a job that blocks on a barrier,
        //   and followed by a number of jobs which increment a counter.
        //   Enqueue another blocking job, followed by more counting jobs.
        //   Unblock the first barrier.  Then, disable the queue, and verify
        //   that no new jobs can be enqueued.  Enable the queue, and verify
        //   that new (counting) jobs can be enqueued.  Unblock the second
        //   barrier, and verify that the remaining jobs are processed.
        //   Disable the queue, and verify that the queue can be deleted.
        //
        // Testing:
        //   int enableQueue(int id);
        //   int disableQueue(int id);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing 'enableQueue' and 'disableQueue' Functions"
                 << endl
                 << "=================================================="
                 << endl;
        }

        for (int k=0; k<100; ++k) {
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            enum {
                k_MIN_THREADS = 3,
                k_MAX_THREADS = 3,
                k_MAX_IDLE    = 60000    // milliseconds
            };
            bslmt::ThreadAttributes defaultAttrs;

            bslmt::Barrier   barrier(2);
            bsls::AtomicInt  counter(0);
            Func             block;      // blocks on 'barrier'
            Func             count;      // increments 'counter'
            makeFunc(&block, waitOnBarrier, &barrier, 1);
            makeFunc(&count, incrementCounter, &counter);
            Obj mX(defaultAttrs,
                   k_MIN_THREADS,
                   k_MAX_THREADS,
                   k_MAX_IDLE,
                   &ta);
            const Obj& X = mX;
            int numElements;

            int id = 0;
            ASSERT(0 != mX.enableQueue(id));
            ASSERT(0 != mX.disableQueue(id));

            ASSERT(0 == mX.start());
            ASSERT(0 == X.numQueues());
            id = mX.createQueue();
            ASSERT(0 != id);
            ASSERT(1 == X.numQueues());
            ASSERT(0 == X.numElements(id));
            ASSERT(0 == mX.deleteQueue(id));
            while (0 < X.numQueues()) {
                bslmt::ThreadUtil::yield();           // SPIN
            }
            ASSERT(0 == X.numQueues());
            ASSERT(0 != mX.disableQueue(id));
            ASSERT(0 != mX.disableQueue(id));

            id = mX.createQueue();
            ASSERT(0 != id);
            ASSERT(1 == X.numQueues());
            ASSERT(0 == X.numElements(id));
            ASSERT(0 == counter);
            ASSERT(0 == mX.enqueueJob(id, block));
            ASSERT(0 == mX.enqueueJob(id, count));
            ASSERT(0 == mX.enqueueJob(id, count));
            ASSERT(0 == mX.enqueueJob(id, count));
            ASSERT(0 == mX.enqueueJob(id, count));
            ASSERT(0 == mX.enqueueJob(id, count));
            ASSERT(0 == mX.enqueueJob(id, &checkThreadName));
            numElements = X.numElements(id);
            ASSERT(6 == numElements || 7 == numElements);
            ASSERT(0 == mX.disableQueue(id));

            ASSERT(0 != mX.enqueueJob(id, block));
            ASSERT(0 != mX.enqueueJob(id, count));
            ASSERT(0 != mX.enqueueJob(id, count));
            ASSERT(0 != mX.enqueueJob(id, count));
            numElements = X.numElements(id);
            ASSERT(6 == numElements || 7 == numElements);

            ASSERT(0 == mX.enableQueue(id));
            ASSERT(0 == mX.enqueueJob(id, count));
            ASSERT(0 == mX.enqueueJob(id, count));
            ASSERT(0 == mX.enqueueJob(id, count));
            numElements = X.numElements(id);

            ASSERT(9 == numElements || 10 == numElements);
            ASSERT(0 == mX.enqueueJob(id, block));
            barrier.wait();                          // 'block' is blocking
            barrier.wait();                          // 'block' is blocking

            ASSERT(0 == mX.disableQueue(id));
            while (0 < X.numElements(id)) {
                bslmt::ThreadUtil::yield();           // SPIN
            }
            ASSERT(0 == mX.deleteQueue(id));
            while (0 < X.numQueues()) {
                bslmt::ThreadUtil::yield();           // SPIN
            }
            ASSERT(0 == X.numQueues());
            ASSERT(8 == counter);
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
        }
      }  break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //
        // Concerns:
        //   Verify the correct behavior of the output operator.
        //
        //   Currently, there is no 'operator<<' defined for
        //   'bdlmt::MultiQueueThreadPool', so this test case remains empty.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Output (<<) Operator" << endl
                 << "============================" << endl;
        }
      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        // Concerns:
        //   * That 'numQueues' reports a snapshot of the number of queues
        //     managed by the pool.
        //
        //   * That 'numElements' reports a snapshot of the number of elements
        //     in a given queue managed by the pool.
        //
        //   * That 'numElements' returns -1 if called on a non-existent or
        //     invalid queue ID.
        //
        // Plan:
        //   Create a modifiable 'bdlmt::MultiQueueThreadPool', 'mX', and a
        //   non-modifiable reference to 'mX' named 'X'.  Create several
        //   queues, and enqueue a monotonically increasing number of jobs to
        //   each queue in succession, starting with a job that blocks on a
        //   barrier, and followed by a number of jobs which increment a
        //   counter.  After all jobs have been enqueued, use 'X' to verify
        //   the number of queues, and the number of elements in each queue.
        //
        // Testing:
        //   int numQueues() const;
        //   int numElements(int id) const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Basic Accessors" << endl
                 << "=======================" << endl;
        }

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            enum {
                k_MIN_THREADS = 16,
                k_MAX_THREADS = 16,
                k_MAX_IDLE    = 60000,    // milliseconds
                k_MAX_QUEUES  = 16        // total number of queues to create
            };
            bslmt::ThreadAttributes defaultAttrs;

            bslmt::Barrier   barrier(1 + k_MAX_QUEUES);
            Func             block;      // blocks on 'barrier'
            makeFunc(&block, waitOnBarrier, &barrier, 1);
            Obj mX(defaultAttrs,
                   k_MIN_THREADS,
                   k_MAX_THREADS,
                   k_MAX_IDLE,
                   &ta);
            const Obj& X = mX;

            ASSERT(0 == mX.start());
            ASSERT(0 == X.numQueues());

            bsls::AtomicInt counters[k_MAX_QUEUES];
            for (int i = 0; i < k_MAX_QUEUES; ++i) {
                int k_NUM_QUEUES = i + 1;
                int id = mX.createQueue();
                ASSERTV(i, 0 != id);
                ASSERTV(i, k_NUM_QUEUES == X.numQueues());
                ASSERTV(i, 0 == mX.enqueueJob(id, block));

                int k_NUM_JOBS = i + 1;
                for (int j = 0; j < k_NUM_JOBS; ++j) {
                    Func count;
                    makeFunc(&count, incrementCounter, &counters[i]);
                    ASSERTV(i, j, 0 == mX.enqueueJob(id, count));
                }
                int numJobs = X.numElements(id);
                ASSERTV(i, k_NUM_JOBS <= numJobs);
                if (verbose) {
                    P_(i); P_(k_NUM_JOBS); P(numJobs);
                }
            }

            barrier.wait();

            // Verify results.
            mX.shutdown();
            ASSERT(0 == X.numQueues());
            for (int i = 0; i < k_MAX_QUEUES; ++i) {
                int VALUE = i + 1;
                int value = counters[i];
                ASSERTV(i, VALUE == value);
                if (verbose) {
                    P_(i); P_(VALUE); P(value);
                }
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY TEST APPARATUS
        //
        // Concerns:
        //   Verify the correct behavior of the primary test apparatus.
        //
        //   Currently, there is no primary test apparatus to verify, so this
        //   test case remains empty.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Primary Test Apparatus" << endl
                 << "==============================" << endl;
        }
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //
        // Concerns:
        //   * That creating a queue while the pool is running yields a valid
        //     queue ID, and that deleting a valid queue while the pool is
        //     running is successful.  Also, that deleting an invalid queue
        //     (either identified by an imaginary queue ID or by an invalid
        //     queue ID) results in an error.
        //
        //   * That enqueuing jobs to a non-existent queue (either identified
        //     by an imaginary queue ID or by an invalid queue ID) results in
        //     an error.
        //
        //   * That enqueuing jobs to an existing queue while the pool is
        //     running is successful.
        //
        //   * That stopping a pool blocks until all queues are empty, but does
        //     not destroy the queues or the threads.
        //
        //   * That creating queues while the pool is stopped succeeds.
        //
        //   * That deleting queues while the pool is stopped fails.
        //
        //   * That enqueuing jobs to any queue while the pool is stopped
        //     results in an error.
        //
        //   * That shutting down the pool blocks until all queues are empty,
        //     and additionally destroys the queues and the threads.
        //
        //   * That re-starting the pool after it has been shut down re-starts
        //     the minimum number of threads, but does not create any queues
        //     (i.e., the number of queues is 0).
        //
        //   * That destroying the pool blocks until all queues are empty.
        //
        // Plan:
        //   Create a modifiable 'bdlmt::MultiQueueThreadPool', 'mX' with
        //   1 < 'minThreads' and 'minThreads' < 'maxThreads'.  Create a
        //   non-modifiable reference to 'mX' named 'X'.
        //
        //   Start the pool.  Verify that deleting a queue via an imaginary
        //   queue ID results in an error.  Create a queue, and delete it.
        //   Verify that deleting the same queue results in an error.
        //
        //   Verify that enqueuing a job to a queue via an imaginary queue ID
        //   results in an error.  Create a queue, and enqueue a job to it.
        //   Delete the queue, and verify that enqueuing a job to the same
        //   queue results in an error.
        //
        //   Create a queue and enqueue a large number of jobs to it.  Each
        //   job, represented by a functor, increments a counter, the address
        //   of which is bound to the functor.  Stop the pool, and verify that
        //   that all jobs have been processed by comparing the value of the
        //   counter to the number of enqueued jobs.  Verify that the queue was
        //   not deleted, and that the threads are still active.  Verify that
        //   deleting the queue and enqueuing jobs to the queue result in an
        //   an error.  Verify that creating a queue results in an error.
        //
        //   Start the pool, and verify that there are no enqueued jobs in any
        //   queue.  Verify that enqueuing a job to the queue is successful.
        //   delete the queue.
        //
        //   Create a queue, and enqueue a large number of jobs to the queue as
        //   above, and shut down the pool.  Verify that all jobs have been
        //   processed as above.  Verify that the queue was deleted, and that
        //   all threads have been destroyed.  Verify that creating a queue
        //   results in an error.
        //
        //   Restart the pool, and verify that no queues exist, but the minimum
        //   number of threads have been started.  Create a queue, and enqueue
        //   a job to it.  Delete the queue.
        //
        //   Create a queue, and enqueue many jobs to it as above.  Destroy
        //   the pool, and verify that the destructor blocks until all queues
        //   are empty.
        //
        // Testing:
        //   bdlmt::MultiQueueThreadPool(
        //                 const bslmt::ThreadAttributes&  threadAttributes,
        //                 int                             minThreads,
        //                 int                             maxThreads,
        //                 int                             maxIdleTime,
        //                 bslma::Allocator               *basicAllocator = 0);
        //   ~bdlmt::MultiQueueThreadPool();
        //   int createQueue();
        //   int deleteQueue(int id, const bsl::function<void()>& cleanupFunc);
        //   int enqueueJob(int id, const bsl::function<void()>& functor);
        //   void start();
        //   void stop();
        //   void shutdown();
        //   const bdlmt::ThreadPool& threadPool() const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Primary Manipulators (Bootstrap)" << endl
                 << "========================================" << endl;
        }

        enum { k_NUM_JOBS = 1000 };  // for testing 'stop' and 'shutdown'

        bsls::AtomicInt counter;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            enum {
                k_MIN_THREADS = 2,
                k_MAX_THREADS = 4,
                k_MAX_IDLE    = 60000    // milliseconds
            };
            bslmt::ThreadAttributes defaultAttrs;

            Obj mX(defaultAttrs,
                   k_MIN_THREADS,
                   k_MAX_THREADS,
                   k_MAX_IDLE,
                   &ta);
            const Obj& X = mX;

            const bdlmt::ThreadPool& tp = X.threadPool();
            ASSERT(k_MIN_THREADS == tp.minThreads());
            ASSERT(k_MAX_THREADS == tp.maxThreads());
            ASSERT(k_MAX_IDLE == tp.maxIdleTime());
            ASSERT(0 == tp.numPendingJobs());
            ASSERT(0 == tp.numActiveThreads());
            ASSERT(0 == tp.numWaitingThreads());
            ASSERT(0 == X.numQueues());

            int  id = 0, id2 = 0;
            Func cleanupCb;  // do nothing

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Start pool.  Create and delete queues.

            if (veryVerbose) {
                const int LINE = L_;
                P_(LINE);
                cout << "Start pool.  Create and delete queues." << endl;
            }
            {
                ASSERT(0 == mX.start());
                ASSERT(0 == tp.numPendingJobs());
                ASSERT(0 == tp.numActiveThreads());
                ASSERT(k_MIN_THREADS == tp.numWaitingThreads());
                ASSERT(0 == X.numQueues());

                id = 0;
                ASSERT(0 != mX.deleteQueue(id));

                id = mX.createQueue();
                ASSERT(0 != id);
                ASSERT(1 == X.numQueues());
                ASSERT(0 == X.numElements(id));
                ASSERT(0 == mX.deleteQueue(id));
                ASSERT(0 == X.numQueues());
                ASSERT(0 != mX.deleteQueue(id));
            }

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Enqueue a job to a valid queue in a running pool.

            if (veryVerbose) {
                const int LINE = L_;
                P_(LINE);
                cout << "Enqueue a job to a valid queue in a running pool."
                     << endl;
            }
            {
                bslmt::Barrier barrier(2);
                Func           block;
                makeFunc(&block, waitOnBarrier, &barrier, 2);

                id = 0;
                ASSERT(0 != mX.enqueueJob(id, block));

                id = mX.createQueue();
                ASSERT(0 != id);
                ASSERT(1 == X.numQueues());
                ASSERT(0 == X.numElements(id));
                ASSERT(0 == mX.enqueueJob(id, &checkThreadName));
                ASSERT(0 == mX.enqueueJob(id, block));
                ASSERT(0 == mX.enqueueJob(id, block));
                barrier.wait();                        // first job blocks
                ASSERT(1 == X.numElements(id));
                ASSERT(1 <= tp.numActiveThreads());    // should be 1 here
                ASSERT(0 <= tp.numWaitingThreads());   // should be 1 here
                ASSERT(0 == tp.numPendingJobs());
                barrier.wait();
                barrier.wait();                        // second job blocks
                ASSERT(0 == X.numElements(id));
                ASSERT(1 <= tp.numActiveThreads());    // might be 2 here
                ASSERT(0 <= tp.numWaitingThreads());   // might be 0 here
                ASSERT(0 == tp.numPendingJobs());
                barrier.wait();

                ASSERT(0 == mX.deleteQueue(id));
                ASSERT(0 != mX.enqueueJob(id, block));
            }

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Enqueue many jobs, stop the pool, and verify that all elements
            // are processed, but neither the queues nor the threads are
            // destroyed.

            if (veryVerbose) {
                const int LINE = L_;
                P_(LINE);
                cout << "Enqueue many elements, and call 'stop'."
                     << endl;
            }
            {
                counter = 0;
                bslmt::Barrier barrier(2);
                Func           block;
                Func           count;
                makeFunc(&block, waitOnBarrier, &barrier, 2);
                makeFunc(&count, incrementCounter, &counter);

                id = mX.createQueue();
                ASSERT(0 != id);
                ASSERT(1 == X.numQueues());
                ASSERT(0 == X.numElements(id));
                ASSERT(0 == mX.enqueueJob(id, block));
                for (int i = 0; i < k_NUM_JOBS; ++i) {
                    ASSERTV(i, 0 == mX.enqueueJob(id, count));
                }
                barrier.wait();
                ASSERT(k_NUM_JOBS == X.numElements(id));  // 'block' is
                                                          // blocking
                ASSERT(0 == counter);
                barrier.wait();

                mX.stop();
                ASSERT(k_NUM_JOBS == counter);
                ASSERT(1 == X.numQueues());
                ASSERT(0 == X.numElements(id));
                ASSERT(0 == tp.numActiveThreads());
                ASSERT(k_MIN_THREADS <= tp.numWaitingThreads());
                ASSERT(0 == tp.numPendingJobs());
                ASSERT(0 != mX.deleteQueue(id));
                ASSERT(0 != mX.enqueueJob(id, block));

                // Queues can be created (but not deleted) on a stopped pool
                id2 = mX.createQueue();
                ASSERT(0 != id2);
                ASSERT(2 == X.numQueues());
            }

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Re-start the pool; pause the queue.  Enqueue a job to the queue.
            // Stop the pool and ensure the job is not executed and stop() does
            // not deadlock.  Start, resume, drain, and stop the queue.
            if (veryVerbose) {
                const int LINE = L_;
                P_(LINE);
                cout << "Re-start; stop a paused queue." << endl;
            }
            {
                counter = 0;
                ASSERT(0 == mX.start());
                ASSERT(2 == X.numQueues());
                ASSERT(0 == X.numElements(id));

                // Now that we're restarted, let's remove the extra queue we
                // created above
                ASSERT(0 == mX.deleteQueue(id2));
                ASSERT(1 == X.numQueues());

                mX.pauseQueue(id);
                ASSERT(X.isPaused(id));
                mX.enqueueJob(id, bdlf::BindUtil::bind(&incrementCounter,
                                                       &counter));
                ASSERT(1 == X.numElements(id));
                mX.stop();
                ASSERT(1 == X.numElements(id));

                ASSERT(0 == mX.start());
                mX.resumeQueue(id);
                ASSERT(!X.isPaused(id));
                mX.drain();
                ASSERT(0 == X.numElements(id));
                ASSERT(1 == counter);
                mX.stop();
            }

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Re-start the pool.  Enqueue a job to the queue.  Then, delete
            // the queue.

            if (veryVerbose) {
                const int LINE = L_;
                P_(LINE);
                cout << "Re-start the pool." << endl;
            }
            {
                ASSERT(0 == mX.start());
                ASSERT(1 == X.numQueues());
                ASSERT(0 == X.numElements(id));

                bslmt::Barrier barrier(2);
                Func           block;
                makeFunc(&block, waitOnBarrier, &barrier, 1);

                ASSERT(0 == mX.enqueueJob(id, block));
                barrier.wait();
                ASSERT(0 == mX.deleteQueue(id));
            }

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Create a queue, and enqueue many jobs.  Shut down the pool.
            // Verify that all elements are processed, and both the queues and
            // the threads are destroyed.

            if (veryVerbose) {
                const int LINE = L_;
                P_(LINE);
                cout << "Enqueue many elements, and call 'shutdown'."
                     << endl;
            }
            {
                counter = 0;
                bslmt::Barrier barrier(2);
                Func           block;
                Func           count;
                makeFunc(&block, waitOnBarrier, &barrier, 2);
                makeFunc(&count, incrementCounter, &counter);

                id = mX.createQueue();
                ASSERT(0 != id);
                ASSERT(1 == X.numQueues());
                ASSERT(0 == X.numElements(id));
                ASSERT(0 == mX.enqueueJob(id, block));
                for (int i = 0; i < k_NUM_JOBS; ++i) {
                    ASSERTV(i, 0 == mX.enqueueJob(id, count));
                }
                barrier.wait();
                ASSERT(k_NUM_JOBS == X.numElements(id));  // 'block' is
                                                          // blocking
                ASSERT(0 == counter);
                barrier.wait();

                mX.shutdown();
                ASSERT(k_NUM_JOBS == counter);
                ASSERT(0 == X.numQueues());
                ASSERT(-1 == X.numElements(id));
                ASSERT(0 == tp.numActiveThreads());
                ASSERT(0 == tp.numWaitingThreads());
                ASSERT(0 == tp.numPendingJobs());
                ASSERT(0 != mX.deleteQueue(id));
                ASSERT(0 != mX.enqueueJob(id, block));
            }

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Re-start the pool.  Create a queue.  Enqueue a job to the queue.
            // Then, delete the queue.

            if (veryVerbose) {
                const int LINE = L_;
                P_(LINE);
                cout << "Re-start the pool." << endl;
            }
            {
                ASSERT(0 == mX.start());
                ASSERT(0 == X.numQueues());
                ASSERT(-1 == X.numElements(id));

                bslmt::Barrier barrier(2);
                Func           block;
                makeFunc(&block, waitOnBarrier, &barrier, 1);

                id = mX.createQueue();
                ASSERT(0 != id);
                ASSERT(1 == X.numQueues());
                ASSERT(0 == X.numElements(id));
                ASSERT(0 == mX.enqueueJob(id, block));
                barrier.wait();
                ASSERT(0 == mX.deleteQueue(id));
            }

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Create a queue, and enqueue many jobs.  Destroy the pool, and
            // verify that all elements have been processed.

            if (veryVerbose) {
                const int LINE = L_;
                P_(LINE);
                cout << "Enqueue many elements, and destroy the pool."
                     << endl;
            }
            {
                counter = 0;
                bslmt::Barrier barrier(2);
                Func           block;
                Func           count;
                makeFunc(&block, waitOnBarrier, &barrier, 2);
                makeFunc(&count, incrementCounter, &counter);

                id = mX.createQueue();
                ASSERT(0 != id);
                ASSERT(1 == X.numQueues());
                ASSERT(0 == X.numElements(id));
                ASSERT(0 == mX.enqueueJob(id, block));
                for (int i = 0; i < k_NUM_JOBS; ++i) {
                    ASSERTV(i, 0 == mX.enqueueJob(id, count));
                }
                barrier.wait();
                ASSERT(k_NUM_JOBS == X.numElements(id));  // 'block' is
                                                          // blocking
                ASSERT(0 == counter);
                barrier.wait();
            }
        }
        ASSERT(k_NUM_JOBS == counter);
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (verbose) cout << "\t'Value CTOR'" << endl;
            {
                bslmt::ThreadAttributes attr;
                ASSERT_PASS_RAW(Obj(attr,   0, 100, 1000));
                ASSERT_FAIL_RAW(Obj(attr,  -1, 100, 1000));
                ASSERT_FAIL_RAW(Obj(attr,  11,  10, 1000));
                ASSERT_PASS_RAW(Obj(attr,  10,  10, 1000));
                ASSERT_PASS_RAW(Obj(attr,   9,  10, 1000));
                ASSERT_FAIL_RAW(Obj(attr,  10,  10,   -1));
            }

         }
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise the basic functionality of the
        //   'bdlmt::MultiQueueThreadPool' class.  We want to ensure that
        //   multi-queue thread pool objects can be instantiated and destroyed.
        //   We also want to exercise the primary manipulators and accessors.
        //
        // Plan:
        //   Create a modifiable 'bdlmt::MultiQueueThreadPool' instance, 'mX',
        //   and a non-'const' reference to 'mX' named 'X'.  Start 'mX'.
        //   Create a queue, and enqueue a fixed number of jobs to it.  Each
        //   job (represented as a functor) increments a counter; the address
        //   the counter is bound to the functor.  Then, stop 'mX', and verify
        //   the results.
        //
        // Testing:
        //   Exercise basic functionality.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Breathing Test" << endl
                 << "==============" << endl;
        }

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            enum {
                k_MIN_THREADS = 2,
                k_MAX_THREADS = 4,
                k_MAX_IDLE    = 60000    // milliseconds
            };
            bslmt::ThreadAttributes defaultAttrs;

            Obj mX(defaultAttrs,
                   k_MIN_THREADS,
                   k_MAX_THREADS,
                   k_MAX_IDLE,
                   &ta);
            const Obj& X = mX;

            if (veryVerbose) {
                cout << "\tVerifying initial state" << endl;
            }
            const bdlmt::ThreadPool& tp = X.threadPool();
            ASSERT(k_MIN_THREADS == tp.minThreads());
            ASSERT(k_MAX_THREADS == tp.maxThreads());
            ASSERT(k_MAX_IDLE == tp.maxIdleTime());
            ASSERT(0 == tp.numPendingJobs());
            ASSERT(0 == tp.numActiveThreads());
            ASSERT(0 == tp.numWaitingThreads());

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Start pool.  Enqueue many jobs, stop the pool, and verify that
            // all elements are processed, but neither the queue nor the
            // threads are destroyed.
            if (veryVerbose) {
                cout << "\tCalling 'start'" << endl;
            }
            ASSERT(0 == mX.start());
            ASSERT(0 == mX.start());
            ASSERT(0 == tp.numPendingJobs());
            ASSERT(0 == tp.numActiveThreads());
            ASSERT(k_MIN_THREADS == tp.numWaitingThreads());

            bslmt::Barrier  barrier(2);
            bsls::AtomicInt counter(0);
            Func            block;      // blocks on 'barrier'
            Func            count;      // increments 'counter'
            makeFunc(&block, waitOnBarrier, &barrier, 2);
            makeFunc(&count, incrementCounter, &counter);

            int id = mX.createQueue();
            ASSERT(0 != id);
            ASSERT(1 == X.numQueues());
            ASSERT(0 == X.numElements(id));

            if (veryVerbose) {
                cout << "\tCreated queue with id = " << id << endl;
            }

            enum { k_NUM_JOBS = 1000 };
            if (veryVerbose) {
                cout << "\tEnqueuing " << k_NUM_JOBS << " jobs." << endl;
            }
            ASSERT(0 == mX.enqueueJob(id, block));
            for (int i = 0; i < k_NUM_JOBS; ++i) {
                ASSERTV(i, 0 == mX.enqueueJob(id, count));
            }

            if (veryVerbose) {
                cout << "\tWaiting for 'block' functor to finish." << endl;
            }
            barrier.wait();
            ASSERT(k_NUM_JOBS  == X.numElements(id));  // 'block' is blocking
            ASSERT(0 == counter);
            barrier.wait();

            if (veryVerbose) {
                cout << "\tCalling 'stop'." << endl;
            }
            mX.stop();
            ASSERT(k_NUM_JOBS == counter);
            ASSERT(1 == X.numQueues());
            ASSERT(0 == X.numElements(id));

            ASSERT(0 == tp.numActiveThreads());
            ASSERT(k_MIN_THREADS <= tp.numWaitingThreads());
            ASSERT(0 == tp.numPendingJobs());
            if (veryVerbose) {
                cout << "\tAbout to destroy the pool." << endl;
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING RETURN STATUS OF 'START'
        //
        // Concerns:
        //  That 'start' fails gracefully when an owned threadpool can't be
        //  started.
        //
        // Plan:
        //  Try to start enough threads to exhaust the address space, and
        //  observe the failure.  Note that this is a '-1' because allocating
        //  that much RAM in the nightly build would be a problem.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing 'start' return status\n"
                             "=============================\n";

#ifdef BSLS_PLATFORM_CPU_32_BIT
        // Only test on 32 bit, so we can easily exhaust the address space.

        bslmt::ThreadAttributes attr;
        attr.setStackSize(bsl::numeric_limits<int>::max());

        bslma::TestAllocator ta(veryVeryVerbose);

        Obj mX(attr, 2, 2, 1000, &ta);

        ASSERT(0 != mX.start());
#endif
      }  break;
      case -2: {
        // --------------------------------------------------------------------
        // TESTING PERFORMANCE OF USAGE EXAMPLE
        //   Tests performance of the usage example.  To provide control over
        //   the test, command line parameters are used.
        //   2nd parameter: multiply the number of jobs sumitted by this
        //   number.
        //   3rd parameter: number of repetitions to peform.
        //
        // Concerns:
        //: 1 Calculates wall time, user time, and system time for inserting
        //:   the given rows in bulk.
        //
        // Plan:
        //: 1 Create a MQPerformance object, and use testFastSearch.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE PERFORMANCE
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Performance of usage example\n"
                             "====================================\n";
        bslma::TestAllocator ta(veryVeryVerbose);

        int numCalcs   = argc > 2 ? atoi(argv[2]) : 1;
        int numRepeats = argc > 3 ? atoi(argv[3]) : 10;
        int numThreads = argc > 4 ? atoi(argv[4]) : 1;

        mqpoolperf::MQPoolPerformance cp("testFS1",
                                         0,
                                         numThreads,
                                         numCalcs,
                                         numRepeats,
                                         &ta);

        // Empty args vector
        mqpoolperf::MQPoolPerformance::VecIntType  args(&ta);
        mqpoolperf::MQPoolPerformance::VecTimeType times(&ta);

        times = cp.runTests(args,
                            mqpoolperf::MQPoolPerformance::testFastSearch);
        cp.printResult();
      }  break;
      default: {
          cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
          testStatus = -1;
      }
    }

//  ASSERT(0 == globalAllocator.numAllocations());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
