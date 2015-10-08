// bdlmt_multiqueuethreadpool.t.cpp                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlmt_multiqueuethreadpool.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslmt_barrier.h>
#include <bslmt_mutex.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>

#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_rawdeleterproctor.h>
#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bdlf_bind.h>
#include <bsl_algorithm.h>
#include <bsl_climits.h>
#include <bsl_fstream.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>
#include <bsl_limits.h>
#include <bsl_map.h>
#include <bsl_memory.h>
#include <bsl_set.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

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
// 'bdlmt_multiqueuethreadpool' private interface
// MANIPULATORS
// [ 2] void deleteQueueCb(int id, const bsl::function<void()>&
//                                                             cleanupFunctor);
// [ 2] void processQueueCb(int id);
//
// 'bdlmt_multiqueuethreadpool' public interface
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
// [ 2] int createQueue();
// [ 2] int deleteQueue(int id, const bsl::function<void()>&
//                                                             cleanupFunctor);
// [ 2] int enqueueJob(int id, const bsl::function<void()>& functor);
// [ 6] int enableQueue(int id);
// [ 6] int disableQueue(int id);
// [ 2] void start();
// [ 2] void stop();
// [ 2] void shutdown();
// [13] void numProcessedReset(int *, int *);
//
// ACCESSORS
// [13] void numProcessed(int *, int *) const;
// [ 4] int numQueues() const;
// [ 4] int numElements(int id) const;
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
// [14] USAGE EXAMPLE 1
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

// The following macros facilitate thread-safe streaming to standard output.

#define MTCOUT   coutMutex.lock(); { bsl::cout << bslmt::ThreadUtil::self() \
                                               << ": "
#define MTENDL   bsl::endl;  } coutMutex.unlock()
#define MTFLUSH  bsl::flush; } coutMutex.unlock()

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
              void (*fptr)(A1, A2, A3, A4), A1 a1, A2 a2, A3 a3, A4 a4)
{
    *f = bdlf::BindUtil::bind(fptr, a1, a2, a3, a4);
}

void noop() {
}

static
void incrementCounter(bsls::AtomicInt *counter)
{
    // Increment the value at the address specified by 'counter'.

    ASSERT(counter);
    ++*counter;
}

static
void waitOnBarrier(bslmt::Barrier *barrier, int numIterations)
{
    // Wait on the specified 'barrier' for 'numIterations' iterations.

    ASSERT(barrier);

    do {
        barrier->wait();
    } while (--numIterations);
}

struct Sleeper {
    int                   d_sleepMicroSeconds;
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
    static bsls::AtomicInt   s_counter;        // submit until counter == 0

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
        enum { SLEEP_HARDLY_TIME = 10 * 1000 };         // 0.01 sec

        bslmt::ThreadUtil::microSleep(SLEEP_HARDLY_TIME);
        d_handleIdx += d_handleIdxIncrement;
        d_handleIdx %= d_handles->size();
        if (s_counter > 0) {
            --s_counter;
            d_threadPool->enqueueJob((*d_handles)[d_handleIdx], *this);
        }
    }
};
bsls::AtomicInt Reproducer::s_counter;

double now() {
    return bdlt::CurrentTime::now().totalSecondsAsDouble();
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

// ============================================================================
//          CLASSES AND HELPER FUNCTIONS FOR TESTING USAGE EXAMPLES
// ----------------------------------------------------------------------------

                           // ======================
                           // class my_SearchProfile
                           // ======================

class my_SearchProfile {
    // This class defines a search profile consisting of a word and a set of
    // files (given by name) that contain the word.  Here, "word" is defined as
    // any string of characters.

    bsl::string           d_word;     // word to search for
    bsl::set<bsl::string> d_fileSet;  // set of matching files

  private:
    // not implemented
    my_SearchProfile(const my_SearchProfile&);
    my_SearchProfile& operator=(const my_SearchProfile&);

  public:
    // CREATORS
    my_SearchProfile(const char *word, bslma::Allocator *basicAllocator = 0);
        // Create a 'my_SearchProfile' with the specified 'word'.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the default memory allocator is used.

    ~my_SearchProfile();
        // Destroy this search profile.

    // MANIPULATORS
    void insert(const char *file);
        // Insert the specified 'file' into the file set maintained by this
        // search profile.

    // ACCESSORS
    bool isMatch(const char *file) const;
        // Return 'true' if the specified 'file' matches this search profile.

    const bsl::set<bsl::string>& fileSet() const;
        // Return a reference to the non-modifiable file set maintained by this
        // search profile.

    const bsl::string& word() const;
        // Return a reference to the non-modifiable word maintained by this
        // search profile.
};

// ----------------------------------------------------------------------------
//                              IMPLEMENTATION
// ----------------------------------------------------------------------------

// CREATORS
my_SearchProfile::my_SearchProfile(const char       *word,
                                   bslma::Allocator *basicAllocator)
: d_word(basicAllocator), d_fileSet(bsl::less<bsl::string>(), basicAllocator)
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
    BSLS_ASSERT(file);

    d_fileSet.insert(file);
}

// ACCESSORS
bool my_SearchProfile::isMatch(const char *file) const
{
    BSLS_ASSERT(file);

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

// ----------------------------------------------------------------------------
static
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

static
void fastSearch(const bsl::vector<bsl::string>&  wordList,
                const bsl::vector<bsl::string>&  fileList,
                bsl::set<bsl::string>&           resultSet,
                bslma::Allocator                *basicAllocator = 0)
{
    // Return the set of files, specified by 'fileList', containing every word
    // in the specified 'wordList', in the specified 'resultSet'.

    typedef bsl::vector<bsl::string> ListType;
        // This type is defined for notational convenience when iterating over
        // 'wordList' or 'fileList'.

    typedef bsl::pair<int, my_SearchProfile*> RegistryValue;
        // This type is defined for notational convenience.  The first
        // parameter specifies a queue ID.  The second parameter specifies an
        // associated search profile.

    typedef bsl::map<bsl::string, RegistryValue> RegistryType;
        // This type is defined for notational convenience.  The first
        // parameter specifies a word.  The second parameter specifies a tuple
        // containing a queue ID, and an associated search profile containing
        // the specified word.

    enum {
        // thread pool configuration
        MIN_THREADS = 4,
        MAX_THREADS = 20,
        MAX_IDLE    = 100   // use a very short idle time since new jobs
                            // arrive only at startup
    };
    bslmt::ThreadAttributes     defaultAttrs;
    bdlmt::MultiQueueThreadPool pool(defaultAttrs,
                                     MIN_THREADS,
                                     MAX_THREADS,
                                     MAX_IDLE,
                                     basicAllocator);
    RegistryType profileRegistry(bsl::less<bsl::string>(), basicAllocator);

    // Start the pool, enabling queue creation and processing.
    ASSERT(0 == pool.start());

    // Create a queue and a search profile associated with each word in
    // 'wordList'.

    for (ListType::const_iterator it = wordList.begin();
         it != wordList.end(); ++it)
    {
        bslma::Allocator *allocator =
                                  bslma::Default::allocator(basicAllocator);

        const bsl::string& word = *it;
        int                id = pool.createQueue();
        LOOP_ASSERT(word, 0 != id);
        my_SearchProfile *profile = new (*allocator)
                                             my_SearchProfile(word.c_str(),
                                                              allocator);

        bslma::RawDeleterProctor<my_SearchProfile, bslma::Allocator>
                                                   deleter(profile, allocator);

        profileRegistry[word] = bsl::make_pair(id, profile);
        deleter.release();
    }

    // Enqueue a job which tries to match each file in 'fileList' with each
    // search profile.

    for (ListType::const_iterator it = fileList.begin();
         it != fileList.end();
         ++it)
    {
        for (ListType::const_iterator jt = wordList.begin();
             jt != wordList.end(); ++jt)
        {
            const bsl::string& file = *it;
            const bsl::string& word = *jt;
            RegistryValue&     rv   = profileRegistry[word];
            Func               job;
            makeFunc(&job, my_SearchCb, rv.second, file.c_str());
            int rc = pool.enqueueJob(rv.first, job);
            LOOP_ASSERT(word, 0 == rc);
        }
    }

    // Stop the pool, and wait while enqueued jobs are processed.
    pool.stop();

    // Construct the 'resultSet' as the intersection of file sets collected in
    // each search profile.

    resultSet.insert(fileList.begin(), fileList.end());
    for (RegistryType::iterator it = profileRegistry.begin();
         it != profileRegistry.end();
         ++it)
    {
        my_SearchProfile *profile = it->second.second;
        const bsl::set<bsl::string>& fileSet = profile->fileSet();
        bsl::set<bsl::string> tmpSet;
        bsl::set_intersection(fileSet.begin(),   fileSet.end(),
                              resultSet.begin(), resultSet.end(),
                              bsl::inserter(tmpSet, tmpSet.begin()));
        resultSet = tmpSet;
        bslma::Default::allocator(basicAllocator)->deleteObjectRaw(profile);
    }
}

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

// ============================================================================
//                         For test cases 14 and 15
// ----------------------------------------------------------------------------

// This was originally done twice in a loop in case 14, but it is taking so
// long that it was broken up to be called separately in 2 cases with different
// values of 'concurrency'.

namespace MULTIQUEUETHREADPOOL_CASE_14 {

enum {
#ifndef BSLS_PLATFORM_OS_CYGWIN
    NUM_QUEUES = 9
#else
    NUM_QUEUES = 5
#endif
};

void testDrainQueueAndDrain(bslma::TestAllocator *ta, int concurrency)
{
    enum {
        MIN_THREADS = 1,
        MAX_THREADS = NUM_QUEUES + 1,
        MAX_IDLE    = 60000    // milliseconds
    };
    bslmt::ThreadAttributes defaultAttrs;

    int queueIds[NUM_QUEUES];

    const double SLEEP_HARDLY_TIME   = 0.01;
    const double SLEEP_A_LITTLE_TIME = 0.50;
    const double SLEEP_A_LOT_TIME    = 3.00;

    Sleeper sleepHardly( SLEEP_HARDLY_TIME);
    Sleeper sleepALittle(SLEEP_A_LITTLE_TIME);
    Sleeper sleepALot(   SLEEP_A_LOT_TIME);

    int ii;
    enum { MAX_LOOP = 4 };
    for (ii = 0; ii <= MAX_LOOP; ++ii) {
        Obj mX(defaultAttrs, MIN_THREADS, MAX_THREADS, MAX_IDLE, ta);
        const Obj& X = mX;
        ASSERT(0 == mX.start());

        Sleeper::s_finished = 0;
        for (int i = 0; i < NUM_QUEUES; ++i) {
            queueIds[i] = mX.createQueue();          ASSERT(queueIds[i]);
        }
        double startTime = now();
        for (int i = 0; i < NUM_QUEUES; ++i) {
            if (NUM_QUEUES - 1 == i) {
                mX.enqueueJob(queueIds[i], sleepALittle);
            }
            else {
                mX.enqueueJob(queueIds[i], sleepALot);
            }
        }
        double time = now() - startTime;
        ASSERT(ii < MAX_LOOP || time < SLEEP_A_LITTLE_TIME);
        if (time >= SLEEP_A_LITTLE_TIME) {
            if (verbose) { P_(L_) P_(concurrency) P(time) }
            continue;
        }

        mX.drainQueue(queueIds[NUM_QUEUES - 1]);
        ASSERT(1 == Sleeper::s_finished);
        {
            time = now() - startTime;
            LOOP_ASSERT(time, time >= SLEEP_A_LITTLE_TIME - jumpTheGun);
            ASSERT(ii < MAX_LOOP || time <  SLEEP_A_LOT_TIME * 0.90);
            if (time >= SLEEP_A_LOT_TIME * 0.90) {
                if (verbose) { P_(L_) P(time); }
                continue;
            }
        }

        int doneJobs, enqueuedJobs;
        X.numProcessed(&doneJobs, &enqueuedJobs);
        ASSERT(NUM_QUEUES == enqueuedJobs);

        mX.drain();
        time = now() - startTime;
        LOOP_ASSERT(time, time >= SLEEP_A_LOT_TIME - jumpTheGun);
        X.numProcessed(&doneJobs, &enqueuedJobs);
        ASSERT(NUM_QUEUES == doneJobs);
        ASSERT(NUM_QUEUES == enqueuedJobs);
        ASSERT(NUM_QUEUES == Sleeper::s_finished);

        // verify pool is still enabled
        startTime = now();
        for (int i = 0; i < NUM_QUEUES; ++i) {
            ASSERT(0 == mX.enqueueJob(queueIds[i], sleepHardly));
        }

        mX.stop();
        ASSERT(now() - startTime >= SLEEP_HARDLY_TIME - jumpTheGun);
        ASSERT(2 * NUM_QUEUES == Sleeper::s_finished);

        ASSERT(0 == mX.start());
        Sleeper::s_finished = 0;
        ASSERT(0 == mX.enqueueJob(queueIds[2], sleepALittle));
        ASSERT(0 == mX.disableQueue(queueIds[2]));
        ASSERT(0 == mX.enqueueJob(queueIds[4], sleepALittle));
        ASSERT(0 == mX.disableQueue(queueIds[4]));
        mX.drainQueue(queueIds[2]);

        // verify queues are disabled as expected after 'drainQueue()'
        for (int i = 0; i < NUM_QUEUES; ++i) {
            if (2 == i || 4 == i) {
                ASSERT(0 != mX.enqueueJob(queueIds[i], sleepHardly));
            }
            else {
                ASSERT(0 == mX.enqueueJob(queueIds[i], sleepHardly));
            }
        }
        mX.drain();
        ASSERT(NUM_QUEUES == Sleeper::s_finished);

        // verify queues are disabled as expected after 'drain()'
        for (int i = 0; i < NUM_QUEUES; ++i) {
            if (2 == i || 4 == i) {
                ASSERT(0 != mX.enqueueJob(queueIds[i], sleepHardly));
            }
            else {
                ASSERT(0 == mX.enqueueJob(queueIds[i], sleepHardly));
            }
        }

        mX.stop();
        ASSERT(2 * NUM_QUEUES - 2 == Sleeper::s_finished);

        // make sure that 'drain' on a stopped pool won't segfault or abort,
        // and that it will return immediately
        startTime = now();
        mX.drain();
        time = now() - startTime;
        ASSERT(ii < MAX_LOOP || time < 0.010);
        if (time >= 0.010) {
            if (verbose) { P_(L_) P(time); }
            continue;
        }

        // verify global queue is still disabled
        for (int i = 0; i < NUM_QUEUES; ++i) {
            ASSERT(0 != mX.enqueueJob(queueIds[i], sleepHardly));
        }

        // make sure that 'drainQueue' on a stopped pool won't segfault or
        // abort, and that it will return immediately
        startTime = now();
        mX.drainQueue(queueIds[3]);
        time = now() - startTime;
        ASSERT(ii < MAX_LOOP || time < 0.010);
        if (time >= 0.010) {
            if (verbose) { P_(L_) P(time); }
            continue;
        }

        // verify global queue is still disabled
        for (int i = 0; i < NUM_QUEUES; ++i) {
            ASSERT(0 != mX.enqueueJob(queueIds[i], sleepHardly));
        }

        break;
    }
    if (verbose) { P_(L_) P(ii); }
    ASSERT(ii <= MAX_LOOP);

}
}  // close namespace MULTIQUEUETHREADPOOL_CASE_14

// ============================================================================
//                              MAIN PROGRAM

int main(int argc, char *argv[]) {

    int test = (argc > 1) ? bsl::atoi(argv[1]) : 1;

    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 18: {
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
            const bsl::string PATH("/bb/infrastructure/groups/bde/include/");
#else
            const bsl::string PATH("/bbsrc/bde/releases/latest/include/bde/");
#endif
            bsl::string FILES[] = {
                PATH + "bdlmxxx_aggregate.h",
            };
            enum {
                NUM_WORDS = sizeof WORDS / sizeof *WORDS,
                NUM_FILES = sizeof FILES / sizeof *FILES
            };

            bsl::vector<bsl::string> wordList(WORDS, WORDS + NUM_WORDS, &ta);
            bsl::vector<bsl::string> fileList(FILES, FILES + NUM_FILES, &ta);
            bsl::set<bsl::string>    resultSet;

            bsls::TimeInterval start = bdlt::CurrentTime::now();
            fastSearch(wordList, fileList, resultSet);
            bsls::TimeInterval stop = bdlt::CurrentTime::now();

//            ASSERT(3 == resultSet.size())

            if (verbose) {
              cout << "DIRECTORY = /bbsrc/bde/releases/latest/include/bde/*.h"
                   << endl;
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
            X_START = 0,
            X_END = 150,
            Y_START = -125,
            Y_END = 125,
            JOB_COUNT = (X_END - X_START) * (Y_END - Y_START)
        };

        bslma::TestAllocator ta(veryVeryVerbose);

        bslmt::ThreadAttributes threadAttrs;
        threadAttrs.setStackSize(1 << 20);      // one megabyte
        bdlmt::MultiQueueThreadPool tp(threadAttrs, 1, 1, 1000*1000, &ta);
        ASSERT(0 == tp.start());
        int queue = tp.createQueue();

        double startTime = bdlt::CurrentTime::now().totalSecondsAsDouble();

        StressJob s;
        s.s_count = 0;
        s.d_allocator_p = &ta;
        for (s.d_x = X_START; X_END > s.d_x; ++s.d_x) {
            for (s.d_y = Y_START; Y_END > s.d_y; ++s.d_y) {
                tp.enqueueJob(queue, s);
            }
        }

        tp.stop();

        double endTime = bdlt::CurrentTime::now().totalSecondsAsDouble();
        if (verbose) {
            cout << "Stress test, " << JOB_COUNT << " jobs, completed in " <<
                                        (endTime - startTime) << " seconds\n";
        }

        LOOP2_ASSERT(s.s_count, JOB_COUNT, JOB_COUNT == s.s_count);
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

        enum {
#ifndef BSLS_PLATFORM_OS_CYGWIN
            NUM_QUEUES = 9
#else
            NUM_QUEUES = 5
#endif
        };

        enum {
            MIN_THREADS = 1,
            MAX_THREADS = NUM_QUEUES + 1,
            MAX_IDLE    = 60000    // milliseconds
        };
        bslmt::ThreadAttributes defaultAttrs;

        // first do 'drainQueue()' case
        {
            Obj mX(defaultAttrs, MIN_THREADS, MAX_THREADS, MAX_IDLE, &ta);
            const Obj& X = mX;
            ASSERT(0 == mX.start());

            enum {
                REPRODUCE_COUNT = 10
            };

            bsl::vector<int> queueIds;

            const double SLEEP_A_LOT_TIME    = 1.0;
            Sleeper sleepALot(SLEEP_A_LOT_TIME);
            Sleeper::s_finished = 0;

            Reproducer reproducer(&mX, &queueIds, 2, 0);
            Reproducer::s_counter = REPRODUCE_COUNT;

            for (int i = 0; i < NUM_QUEUES; ++i) {
                int queueId = mX.createQueue();                ASSERT(queueId);
                queueIds.push_back(queueId);
            }
            for (int i = 0; i < NUM_QUEUES; ++i) {
                if (2 == i) {
                    mX.enqueueJob(queueIds[i], reproducer);
                }
                else {
                    mX.enqueueJob(queueIds[i], sleepALot);
                }
            }
            ASSERT(0 == Sleeper::s_finished);
            LOOP_ASSERT(Reproducer::s_counter, 0 <  Reproducer::s_counter);

            mX.drainQueue(queueIds[2]);
            LOOP_ASSERT(Reproducer::s_counter, 0 == Reproducer::s_counter);
            ASSERT(0 == Sleeper::s_finished);

            int numDequeued, numEnqueued;
            X.numProcessed(&numDequeued, &numEnqueued);

            ASSERT(NUM_QUEUES + REPRODUCE_COUNT == numEnqueued);

            mX.stop();
            ASSERT(NUM_QUEUES + REPRODUCE_COUNT == numDequeued);
            ASSERT(NUM_QUEUES - 1 == Sleeper::s_finished);
        }

        // next do 'drain' case
        {
            Obj mX(defaultAttrs, MIN_THREADS, MAX_THREADS, MAX_IDLE, &ta);
            ASSERT(0 == mX.start());

            enum {
                REPRODUCE_COUNT = 50
            };

            bsl::vector<int> queueIds;

            Reproducer reproducer(&mX, &queueIds, 1, 1);
            Reproducer::s_counter = REPRODUCE_COUNT;

            for (int i = 0; i < NUM_QUEUES; ++i) {
                int queueId = mX.createQueue();                ASSERT(queueId);
                queueIds.push_back(queueId);
            }
            mX.enqueueJob(queueIds[0], reproducer);
            LOOP_ASSERT(Reproducer::s_counter, 0 <  Reproducer::s_counter);

            mX.drain();
            LOOP_ASSERT(Reproducer::s_counter, 0 == Reproducer::s_counter);

            int numDequeued, numEnqueued;
            mX.numProcessed(&numDequeued, &numEnqueued);

            ASSERT(1 + REPRODUCE_COUNT == numEnqueued);
            ASSERT(1 + REPRODUCE_COUNT == numDequeued);
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

        testDrainQueueAndDrain(&ta, NUM_QUEUES + 1);
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
        // TESTING numProcessed AND numProcessedReset
        //
        // Concerns:
        //   numProcessed and numProcessedReset return values as expected.
        //
        // Plan:
        //   In a loop over the same object under test, start the pool,
        //   enqueue a number of quantities into different queues, then
        //   drain the pool.  Verify that numProcessed and numProcessedReset
        //   return values as expected.
        //
        // Testing:
        //   void numProcessed(int *, int *)
        //   void numProcessedReset(int *, int *)
        // --------------------------------------------------------------------

        if (verbose)
            cout << "Testing numProcessed and numProcessedReset" << endl
                 << "==========================================" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            enum {
#ifndef BSLS_PLATFORM_OS_CYGWIN
                NUM_QUEUES = 10,
                NUM_ITERATIONS = 10,
                NUM_JOBS = 200
#else
                NUM_QUEUES = 5,
                NUM_ITERATIONS = 10,
                NUM_JOBS = 50
#endif
            };

            enum {
                MIN_THREADS = 1,
                MAX_THREADS = NUM_QUEUES + 1,
                MAX_IDLE    = 60000    // milliseconds
            };
            bslmt::ThreadAttributes defaultAttrs;

            Obj mX(defaultAttrs, MIN_THREADS, MAX_THREADS, MAX_IDLE, &ta);
            const Obj& X = mX;

            for (int i = 1; i <= NUM_ITERATIONS; ++i) {
                int numEnqueued = -1, numDequeued = -1;
                X.numProcessed(&numDequeued, &numEnqueued);
                ASSERT(0 == numDequeued);
                ASSERT(0 == numEnqueued);

                if (veryVerbose)
                    cout << "Iteration " << i << "\n";
                ASSERT(0 == mX.start());
                int  QUEUE_IDS[NUM_QUEUES];
                Func QUEUE_NOOP[NUM_QUEUES];

                bslmt::Barrier barrier(1+NUM_QUEUES);
                Func           block;  // blocks on barrier
                makeFunc(&block, waitOnBarrier, &barrier, 1);

                for (int j = 0; j < NUM_QUEUES; ++j) {
                    QUEUE_NOOP[j] = Func(&noop);
                    int id = QUEUE_IDS[j] = mX.createQueue();
                    LOOP2_ASSERT(i, j, 0 == mX.enqueueJob(id, block));
                }
                ASSERT(NUM_QUEUES == X.numQueues());

                for (int j = 0; j < i * NUM_JOBS; ++j) {
                    for (int k = 0; k < NUM_QUEUES; ++k) {
                        LOOP3_ASSERT(i, j, k,
                              0 == mX.enqueueJob(QUEUE_IDS[k], QUEUE_NOOP[k]));
                    }
                }
                if (veryVerbose)
                    cout << "   waiting for barrier\n";
                barrier.wait();
                if (veryVerbose)
                    cout << "   passed barrier\n";
                X.numProcessed(&numDequeued, &numEnqueued);

                LOOP3_ASSERT(i, NUM_QUEUES * (1 + i * NUM_JOBS), numEnqueued,
                               NUM_QUEUES * (1 + i * NUM_JOBS) == numEnqueued);
                LOOP_ASSERT(i, 0 < numDequeued);
                // on Linux, IBM, and Solaris 10, code is so optimized that
                // sometimes equality holds (the pool is already drained)
                LOOP_ASSERT(i, numDequeued <= numEnqueued);

                if (veryVerbose)
                    cout << "   waiting for threadpool stop\n";
                mX.stop();
                if (veryVerbose)
                    cout << "   threadpool stopped\n";
                X.numProcessed(&numDequeued, &numEnqueued);

                LOOP3_ASSERT(i, NUM_QUEUES * (1 + i * NUM_JOBS), numEnqueued,
                               NUM_QUEUES * (1 + i * NUM_JOBS) == numEnqueued);
                LOOP_ASSERT(i, numDequeued == numEnqueued);

                int numEnqueued2 = -1, numDequeued2 = -1;
                mX.numProcessedReset(&numDequeued2, &numEnqueued2);
                LOOP_ASSERT(i, numEnqueued2 == numEnqueued);
                LOOP_ASSERT(i, numDequeued2 == numDequeued);

                X.numProcessed(&numDequeued, &numEnqueued);
                LOOP_ASSERT(i, 0 == numDequeued);
                LOOP_ASSERT(i, 0 == numEnqueued);

                mX.start();

                for (int j = 0; j < NUM_QUEUES; ++j) {
                    int rc = mX.deleteQueue(QUEUE_IDS[j]);
                    ASSERT(0 == rc);
                }

                mX.stop();
                mX.numProcessed(&numDequeued, &numEnqueued);
                LOOP2_ASSERT(i, numDequeued, 0 == numDequeued);
                LOOP2_ASSERT(i, numEnqueued, 0 == numEnqueued);
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
        //   using a cleanup callback which enqueues a job to 'id2'.
        //   Re-create queue 'id1', and delete it with a callback that deletes
        //   'id2'.
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
                MIN_THREADS = 2,
                MAX_THREADS = 2,
                MAX_IDLE    = 60000    // milliseconds
            };
            bslmt::ThreadAttributes defaultAttrs;

            Obj mX(defaultAttrs, MIN_THREADS, MAX_THREADS, MAX_IDLE, &ta);
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

            while (2 != counter) {         // SPIN
                bslmt::ThreadUtil::microSleep(250000); // trigger thread switch
                bslmt::ThreadUtil::yield();
            }
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
                MIN_THREADS = 1,
                MAX_THREADS = 1,
                MAX_IDLE    = 60000    // milliseconds
            };
            bslmt::ThreadAttributes defaultAttrs;

            Obj mX(defaultAttrs, MIN_THREADS, MAX_THREADS, MAX_IDLE, &ta);

            int id = 0;
            bsls::AtomicInt counter(0);
            Func            cleanupCb;
            bslmt::Barrier  barrier(2);
            makeFunc(&cleanupCb, case11CleanUp, &counter, &barrier);

            enum { NUM_ITERATIONS = 500 };
            ASSERT(0 == mX.start());
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                id = mX.createQueue();
                LOOP_ASSERT(i, 0 != id);
                LOOP_ASSERT(i, 0 == mX.deleteQueue(id, cleanupCb));
                LOOP_ASSERT(i, 0 == counter);
                barrier.wait();
                barrier.wait();
                LOOP_ASSERT(i, counter == 1);
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
            MIN_THREADS = 1,
            NUM_QUEUES = 5,
            MAX_THREADS = NUM_QUEUES * 2,
            MAX_IDLE = 60000    // milliseconds
        };

        bdlmt::ThreadPool tp(bslmt::ThreadAttributes(),
                             MIN_THREADS,
                             MAX_THREADS,
                             MAX_IDLE,
                             &ta);
        Obj *pMX = new (ta) Obj(&tp, &ta);
        Obj& mX = *pMX;         const Obj& X = mX;
        Obj mY(&tp, &ta);       const Obj& Y = mY;

        int queueIds[NUM_QUEUES][2];
        enum { IX = 0, IY = 1 };

        const double SLEEP_A_LITTLE_TIME = 0.50;
        const double SLEEP_A_LOT_TIME    = 1.00;

        Sleeper sleepALittle(SLEEP_A_LITTLE_TIME);
        Sleeper sleepALot(   SLEEP_A_LOT_TIME);

        // less than MAX_THREADS + 1
        ASSERT(0 == tp.start());
        ASSERT(0 == mX.start());
        ASSERT(0 == mY.start());
        double startTime = now();

        for (int i = 0; i < NUM_QUEUES; ++i) {
            queueIds[i][IX] = mX.createQueue();
            ASSERT(0 != queueIds[i][IX]);
            queueIds[i][IY] = mY.createQueue();
            ASSERT(0 != queueIds[i][IY]);
        }

        for (int i = 0; i < NUM_QUEUES; ++i) {
            ASSERT(0 == mX.enqueueJob(queueIds[i][IX], sleepALittle));
            ASSERT(0 == mY.enqueueJob(queueIds[i][IY], sleepALittle));
        }
        LOOP_ASSERT(Sleeper::s_finished, 0 == Sleeper::s_finished);

        int numDequeued, numEnqueued;
        X.numProcessed(&numDequeued, &numEnqueued);
        LOOP_ASSERT(numEnqueued, NUM_QUEUES == numEnqueued);
        Y.numProcessed(&numDequeued, &numEnqueued);
        LOOP_ASSERT(numEnqueued, NUM_QUEUES == numEnqueued);

        mX.stop();
        mY.stop();
        LOOP_ASSERT(Sleeper::s_finished, 2*NUM_QUEUES == Sleeper::s_finished);
        double time = now() - startTime;
        LOOP_ASSERT(time, time >= SLEEP_A_LITTLE_TIME - jumpTheGun);

        X.numProcessed(&numDequeued, &numEnqueued);
        LOOP_ASSERT(numEnqueued, NUM_QUEUES == numEnqueued);
        LOOP_ASSERT(numDequeued, NUM_QUEUES == numDequeued);
        Y.numProcessed(&numDequeued, &numEnqueued);
        LOOP_ASSERT(numEnqueued, NUM_QUEUES == numEnqueued);
        LOOP_ASSERT(numDequeued, NUM_QUEUES == numDequeued);

        Sleeper::s_finished = 0;

        ASSERT(0 == mX.start());
        ASSERT(0 == mY.start());

        startTime = now();
        ASSERT(0 == mX.enqueueJob(queueIds[0][IX], sleepALittle));
        for (int i = 0; i < NUM_QUEUES; ++i) {
            ASSERT(0 == mX.disableQueue(queueIds[i][IX]));
        }
        for (int i = 0; i < NUM_QUEUES; ++i) {
            ASSERT(0 != mX.enqueueJob(queueIds[i][IX], sleepALittle));
            ASSERT(0 == mY.enqueueJob(queueIds[i][IY], sleepALot));
        }
        LOOP_ASSERT(Sleeper::s_finished, 0 == Sleeper::s_finished);

        mX.stop();
        LOOP_ASSERT(Sleeper::s_finished, 1 <= Sleeper::s_finished);
        time = now() - startTime;
        LOOP_ASSERT(time, time >= SLEEP_A_LITTLE_TIME - jumpTheGun);

        ta.deleteObjectRaw(pMX);

        for (int i = 0; i < NUM_QUEUES; ++i) {
            ASSERT(0 == mY.enqueueJob(queueIds[i][IY], sleepALittle));
        }

        mY.stop();
        LOOP_ASSERT(Sleeper::s_finished,
                                    2 * NUM_QUEUES + 1 == Sleeper::s_finished);
        LOOP_ASSERT(now() - startTime, now() - startTime >=
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

                { L_,   1,           1,           256,        1,      },
                { L_,   1,           1,           256,        2,      },
                { L_,   1,           1,           256,        8,      },
                { L_,   1,           1,           256,        64,     },
                { L_,   1,           1,           256,        256,    },

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

                { L_,   1,           16,          256,        1,      },
                { L_,   2,           16,          256,        2,      },
                { L_,   4,           16,          256,        8,      },
                { L_,   8,           16,          256,        64,     },
                { L_,   16,          16,          256,        256,    },
            };
            enum { DATA_SIZE = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < DATA_SIZE; ++i) {
                const int LINE        = DATA[i].d_line;
                const int MIN_THREADS = DATA[i].d_minThreads;
                const int MAX_THREADS = DATA[i].d_maxThreads;
                const int NUM_QUEUES  = DATA[i].d_numQueues;
                const int NUM_JOBS    = DATA[i].d_numJobs;
                const int MAX_IDLE    = 1000;  // milliseconds
                bslmt::ThreadAttributes defaultAttrs;

                Obj mX(defaultAttrs, MIN_THREADS, MAX_THREADS, MAX_IDLE, &ta);
                const Obj& X = mX;
                LOOP2_ASSERT(i, LINE, 0 == X.numQueues());
                ASSERT(0 == mX.start());

                if (verbose) {
                    P_(i); P_(LINE);
                    P_(MIN_THREADS); P_(MAX_THREADS);
                    P_(NUM_QUEUES);  P(NUM_JOBS);
                }

                bsl::vector<bsl::vector<int> > results(NUM_QUEUES);
                bsl::vector<bsls::AtomicInt>    counters(NUM_QUEUES);

                // Create queues and enqueue jobs.
                for (int j = 0; j < NUM_QUEUES; ++j) {
                    int id = mX.createQueue();
                    LOOP3_ASSERT(i, LINE, j, 0 != id);
                    LOOP3_ASSERT(i, LINE, j, true == results[j].empty());

                    for (int k = 0; k < NUM_JOBS; ++k) {
                        Func job;
                        makeFunc(&job,
                                 case9Callback,
                                 &counters[j],
                                 &results[j]);
                        LOOP4_ASSERT(i, LINE, j, k,
                                     0 == mX.enqueueJob(id, job));
                    }
                }
                LOOP2_ASSERT(i, LINE, NUM_QUEUES == X.numQueues());

                mX.stop();
                LOOP2_ASSERT(i, LINE, NUM_QUEUES == X.numQueues());
                LOOP2_ASSERT(i, LINE, 0 <  X.threadPool().numWaitingThreads());
                LOOP2_ASSERT(i, LINE, 0 == X.threadPool().numActiveThreads());

                mX.shutdown();
                LOOP2_ASSERT(i, LINE, 0 == X.numQueues());
                LOOP2_ASSERT(i, LINE, 0 == X.threadPool().numWaitingThreads());
                LOOP2_ASSERT(i, LINE, 0 == X.threadPool().numActiveThreads());

                // Verify results.
                for (int j = 0; j < NUM_QUEUES; ++j) {
                    LOOP3_ASSERT(i, LINE, j,
                                 NUM_JOBS == (int)results[j].size());
                    for (int k = 0; k < NUM_JOBS; ++k) {
                        const int VALUE = k + 1;
                        LOOP4_ASSERT(i, LINE, j, k,
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
                MIN_THREADS = 2,
                MAX_THREADS = 4,
                MAX_IDLE    = 60000    // milliseconds
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
            Obj mX(defaultAttrs, MIN_THREADS, MAX_THREADS, MAX_IDLE, &ta);
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
                MIN_THREADS = 2,
                MAX_THREADS = 4,
                IDLE        = 1000,    // milliseconds
                MAX_IDLE    = 60000    // milliseconds
            };
            bslmt::ThreadAttributes defaultAttrs;
            bdlmt::ThreadPool       pool(defaultAttrs,
                                         MIN_THREADS,
                                         MAX_THREADS,
                                         MAX_IDLE,
                                         &ta);

            {
                Obj        mX(&pool, &ta);
                const Obj& X = mX;

                const bdlmt::ThreadPool& tp = X.threadPool();
                ASSERT(&tp == &pool);
                ASSERT(MIN_THREADS == tp.minThreads());
                ASSERT(MAX_THREADS == tp.maxThreads());
                ASSERT(MAX_IDLE == tp.maxIdleTime());
                ASSERT(0 == tp.numPendingJobs());
                ASSERT(0 == tp.numActiveThreads());
                ASSERT(0 == tp.numWaitingThreads());

                ASSERT(MIN_THREADS == pool.minThreads());
                ASSERT(MAX_THREADS == pool.maxThreads());
                ASSERT(MAX_IDLE == pool.maxIdleTime());
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
                ASSERT(MIN_THREADS == tp.numWaitingThreads());

                ASSERT(0 == pool.numPendingJobs());
                ASSERT(0 == pool.numActiveThreads());
                ASSERT(MIN_THREADS == pool.numWaitingThreads());

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

                enum { NUM_JOBS = 1000 };
                ASSERT(0 == mX.enqueueJob(id, block));
                for (int i = 0; i < NUM_JOBS; ++i) {
                    LOOP_ASSERT(i, 0 == mX.enqueueJob(id, count));
                }
                barrier.wait();
                ASSERT(NUM_JOBS == X.numElements(id));  // 'block' is blocking
                ASSERT(0 == counter);
                barrier.wait();

                mX.stop();
                ASSERT(NUM_JOBS == counter);
                ASSERT(1 == X.numQueues());
                ASSERT(0 == X.numElements(id));

                // Also give a chance to the Threadpool to kill the idle
                // threads.  But given the problematic likelihood of failure of
                // this test, only assert '1 >= tp.numActiveThreads()' when all
                // else has been exhausted.

                bslmt::ThreadUtil::microSleep(IDLE);
                bslmt::ThreadUtil::yield();
                if (1 < tp.numActiveThreads()) {
                    cout << "WARNING: Long delay in case 7 (still "
                         << tp.numActiveThreads() << " threads active)"
                         << endl;
                    bslmt::ThreadUtil::microSleep(IDLE);
                    bslmt::ThreadUtil::yield();
                }

                LOOP_ASSERT(tp.numActiveThreads(),
                            1 >= tp.numActiveThreads());
                ASSERT(MIN_THREADS <= tp.numActiveThreads()
                                    + tp.numWaitingThreads());
                ASSERT(0 == tp.numPendingJobs());

                ASSERT(1 >= pool.numActiveThreads());
                ASSERT(MIN_THREADS <= pool.numActiveThreads()
                                    + pool.numWaitingThreads());
                ASSERT(0 == pool.numPendingJobs());
            }

            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            // Destroy the pool, and verify that the external thread pool is
            // valid, and that all but perhaps the last thread have been
            // destroyed.

            ASSERT(MIN_THREADS == pool.minThreads());
            ASSERT(MAX_THREADS == pool.maxThreads());
            ASSERT(MAX_IDLE == pool.maxIdleTime());
            ASSERT(0 == pool.numPendingJobs());
            ASSERT(1 >= pool.numActiveThreads());
            ASSERT(MIN_THREADS <= pool.numActiveThreads()
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

        for(int k=0; k<100; ++k){
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            enum {
                MIN_THREADS = 3,
                MAX_THREADS = 3,
                MAX_IDLE    = 60000    // milliseconds
            };
            bslmt::ThreadAttributes defaultAttrs;

            bslmt::Barrier   barrier(2);
            bsls::AtomicInt  counter(0);
            Func             block;      // blocks on 'barrier'
            Func             count;      // increments 'counter'
            makeFunc(&block, waitOnBarrier, &barrier, 1);
            makeFunc(&count, incrementCounter, &counter);
            Obj mX(defaultAttrs, MIN_THREADS, MAX_THREADS, MAX_IDLE, &ta);
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
            numElements = X.numElements(id);
            ASSERT(5 == numElements || 6 == numElements);
            ASSERT(0 == mX.disableQueue(id));

            ASSERT(0 != mX.enqueueJob(id, block));
            ASSERT(0 != mX.enqueueJob(id, count));
            ASSERT(0 != mX.enqueueJob(id, count));
            ASSERT(0 != mX.enqueueJob(id, count));
            numElements = X.numElements(id);
            ASSERT(5 == numElements || 6 == numElements);

            ASSERT(0 == mX.enableQueue(id));
            ASSERT(0 == mX.enqueueJob(id, count));
            ASSERT(0 == mX.enqueueJob(id, count));
            ASSERT(0 == mX.enqueueJob(id, count));
            numElements = X.numElements(id);
            ASSERT(8 == numElements || 9 == numElements);
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
                MIN_THREADS = 16,
                MAX_THREADS = 16,
                MAX_IDLE    = 60000,    // milliseconds
                MAX_QUEUES  = 16        // total number of queues to create
            };
            bslmt::ThreadAttributes defaultAttrs;

            bslmt::Barrier   barrier(1 + MAX_QUEUES);
            Func             block;      // blocks on 'barrier'
            makeFunc(&block, waitOnBarrier, &barrier, 1);
            Obj mX(defaultAttrs, MIN_THREADS, MAX_THREADS, MAX_IDLE, &ta);
            const Obj& X = mX;

            ASSERT(0 == mX.start());
            ASSERT(0 == X.numQueues());

            bsls::AtomicInt counters[MAX_QUEUES];
            for (int i = 0; i < MAX_QUEUES; ++i) {
                int NUM_QUEUES = i + 1;
                int id = mX.createQueue();
                LOOP_ASSERT(i, 0 != id);
                LOOP_ASSERT(i, NUM_QUEUES == X.numQueues());
                LOOP_ASSERT(i, 0 == mX.enqueueJob(id, block));

                int NUM_JOBS = i + 1;
                for (int j = 0; j < NUM_JOBS; ++j) {
                    Func count;
                    makeFunc(&count, incrementCounter, &counters[i]);
                    LOOP2_ASSERT(i, j, 0 == mX.enqueueJob(id, count));
                }
                int numJobs = X.numElements(id);
                LOOP_ASSERT(i, NUM_JOBS <= numJobs);
                if (verbose) {
                    P_(i); P_(NUM_JOBS); P(numJobs);
                }
            }

            barrier.wait();

            // Verify results.
            mX.shutdown();
            ASSERT(0 == X.numQueues());
            for (int i = 0; i < MAX_QUEUES; ++i) {
                int VALUE = i + 1;
                int value = counters[i];
                LOOP_ASSERT(i, VALUE == value);
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
        //   * That creating or deleting queues while the pool is stopped
        //     results in an error.
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
        //   int deleteQueue(int id, const bsl::function<void()>&
        //                                                     cleanupFunctor);
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

        enum { NUM_JOBS = 1000 };  // for testing 'stop' and 'shutdown'

        bsls::AtomicInt counter;
        bslma::TestAllocator ta(veryVeryVerbose);
        {
            enum {
                MIN_THREADS = 2,
                MAX_THREADS = 4,
                MAX_IDLE    = 60000    // milliseconds
            };
            bslmt::ThreadAttributes defaultAttrs;

            Obj mX(defaultAttrs, MIN_THREADS, MAX_THREADS, MAX_IDLE, &ta);
            const Obj& X = mX;

            const bdlmt::ThreadPool& tp = X.threadPool();
            ASSERT(MIN_THREADS == tp.minThreads());
            ASSERT(MAX_THREADS == tp.maxThreads());
            ASSERT(MAX_IDLE == tp.maxIdleTime());
            ASSERT(0 == tp.numPendingJobs());
            ASSERT(0 == tp.numActiveThreads());
            ASSERT(0 == tp.numWaitingThreads());
            ASSERT(0 == X.numQueues());

            int  id = 0;
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
                ASSERT(MIN_THREADS == tp.numWaitingThreads());
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
                for (int i = 0; i < NUM_JOBS; ++i) {
                    LOOP_ASSERT(i, 0 == mX.enqueueJob(id, count));
                }
                barrier.wait();
                ASSERT(NUM_JOBS == X.numElements(id));  // 'block' is blocking
                ASSERT(0 == counter);
                barrier.wait();

                mX.stop();
                ASSERT(NUM_JOBS == counter);
                ASSERT(1 == X.numQueues());
                ASSERT(0 == X.numElements(id));
                ASSERT(0 == tp.numActiveThreads());
                ASSERT(MIN_THREADS <= tp.numWaitingThreads());
                ASSERT(0 == tp.numPendingJobs());
                ASSERT(0 != mX.deleteQueue(id));
                ASSERT(0 != mX.enqueueJob(id, block));
                ASSERT(0 == mX.createQueue());
                ASSERT(1 == X.numQueues());
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
                for (int i = 0; i < NUM_JOBS; ++i) {
                    LOOP_ASSERT(i, 0 == mX.enqueueJob(id, count));
                }
                barrier.wait();
                ASSERT(NUM_JOBS == X.numElements(id));  // 'block' is blocking
                ASSERT(0 == counter);
                barrier.wait();

                mX.shutdown();
                ASSERT(NUM_JOBS == counter);
                ASSERT(0 == X.numQueues());
                ASSERT(-1 == X.numElements(id));
                ASSERT(0 == tp.numActiveThreads());
                ASSERT(0 == tp.numWaitingThreads());
                ASSERT(0 == tp.numPendingJobs());
                ASSERT(0 != mX.deleteQueue(id));
                ASSERT(0 != mX.enqueueJob(id, block));
                ASSERT(0 == mX.createQueue());
                ASSERT(0 == X.numQueues());
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
                for (int i = 0; i < NUM_JOBS; ++i) {
                    LOOP_ASSERT(i, 0 == mX.enqueueJob(id, count));
                }
                barrier.wait();
                ASSERT(NUM_JOBS == X.numElements(id));  // 'block' is blocking
                ASSERT(0 == counter);
                barrier.wait();
            }
        }
        ASSERT(NUM_JOBS == counter);
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
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
        //   and a non-const reference to 'mX' named 'X'.  Start 'mX'.  Create
        //   a queue, and enqueue a fixed number of jobs to it.  Each job
        //   (represented as a functor) increments a counter; the address
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
                MIN_THREADS = 2,
                MAX_THREADS = 4,
                MAX_IDLE    = 60000    // milliseconds
            };
            bslmt::ThreadAttributes defaultAttrs;

            Obj mX(defaultAttrs, MIN_THREADS, MAX_THREADS, MAX_IDLE, &ta);
            const Obj& X = mX;

            if (veryVerbose) {
                cout << "\tVerifying initial state" << endl;
            }
            const bdlmt::ThreadPool& tp = X.threadPool();
            ASSERT(MIN_THREADS == tp.minThreads());
            ASSERT(MAX_THREADS == tp.maxThreads());
            ASSERT(MAX_IDLE == tp.maxIdleTime());
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
            ASSERT(MIN_THREADS == tp.numWaitingThreads());

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

            enum { NUM_JOBS = 1000 };
            if (veryVerbose) {
                cout << "\tEnqueuing " << NUM_JOBS << " jobs." << endl;
            }
            ASSERT(0 == mX.enqueueJob(id, block));
            for (int i = 0; i < NUM_JOBS; ++i) {
                LOOP_ASSERT(i, 0 == mX.enqueueJob(id, count));
            }

            if (veryVerbose) {
                cout << "\tWaiting for 'block' functor to finish." << endl;
            }
            barrier.wait();
            ASSERT(NUM_JOBS  == X.numElements(id));  // 'block' is blocking
            ASSERT(0 == counter);
            barrier.wait();

            if (veryVerbose) {
                cout << "\tCalling 'stop'." << endl;
            }
            mX.stop();
            ASSERT(NUM_JOBS == counter);
            ASSERT(1 == X.numQueues());
            ASSERT(0 == X.numElements(id));

            ASSERT(0 == tp.numActiveThreads());
            ASSERT(MIN_THREADS <= tp.numWaitingThreads());
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
