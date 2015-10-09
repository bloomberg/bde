// bslmt_semaphoreimpl_darwin.t.cpp                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_semaphoreimpl_darwin.h>

#if defined(BSLS_PLATFORM_OS_DARWIN)

#include <bslmt_lockguard.h>   // for testing only
#include <bslmt_mutex.h>       // for testing only
#include <bslmt_threadutil.h>  // for testing only

#include <bslmt_semaphoreimpl_counted.h>

#include <bslim_testutil.h>

#include <bsls_atomic.h>
#include <bslmt_platform.h>

#include <bsls_timeinterval.h>

#include <bsls_timeutil.h>
#include <bsls_types.h>

#include <bsl_vector.h>
#include <bsl_deque.h>
#include <bsl_iostream.h>

#include <bsl_cstdlib.h>

#include <bsl_c_time.h>
#include <bsl_c_stdio.h>

#include <pthread.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//
//
//-----------------------------------------------------------------------------
// [1] Breathing test
// [2] wait(int *signalInterrupted = 0)
// [2] post()
// [3] post(int number)
// [4] tryWait()
// [5] USAGE Example

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

static bslmt::Mutex coutMutex;

#define MTCOUT   { coutMutex.lock(); cout << bslmt::ThreadUtil::selfIdAsInt() \
                                          << ": "
#define MTENDL   endl;  coutMutex.unlock(); }
#define MTFLUSH  bsl::flush; } coutMutex.unlock()

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslmt::SemaphoreImpl<bslmt::Platform::SemaphorePolicy> Obj;

class MyCondition {
    // This class defines a platform-independent condition variable.  Using
    // bslmt Condition would create a dependency cycle.
    // DATA
    pthread_cond_t d_cond;

    // NOT IMPLEMENTED
    MyCondition(const MyCondition&);
    MyCondition& operator=(const MyCondition&);

  public:
    // CREATORS
    MyCondition()
    {
        pthread_cond_init(&d_cond, 0);
    }

    ~MyCondition()
    {
        pthread_cond_destroy(&d_cond);
    }

    // MANIPULATORS
    int wait(bslmt::Mutex *mutex)
    {
        return pthread_cond_wait(&d_cond, &mutex->nativeMutex());
    }

    void broadcast()
    {
        pthread_cond_broadcast(&d_cond);
    }
};

class MyBarrier {
    // This class defines a thread barrier.  This is a cut-and-paste of bslmt
    // Barrier, but depending on bslmt Barrier itself here would cause a
    // dependency cycle.

    bslmt::Mutex    d_mutex;      // mutex used to control access to this
                                  // barrier.
    MyCondition     d_cond;       // condition variable used for signaling
                                  // blocked threads.
    const int       d_numThreads; // number of threads required to be waiting
                                  // before this barrier can be signaled.
    int             d_numWaiting; // number of threads currently waiting
                                  // for this barrier to be signaled.
    int             d_sigCount;   // counted of number of times this barrier
                                  // has been signaled.
    int             d_numPending; // Number of threads that have been signaled
                                  // but have not yet awakened.

    // NOT IMPLEMENTED
    MyBarrier(const MyBarrier&);
    MyBarrier& operator=(const MyBarrier&);

  public:
    // CREATORS
    explicit MyBarrier(int numThreads);
        // Construct a barrier that requires 'numThreads' to unblock.  Note
        // that the behavior is undefined unless '0 < numThreads'.

    ~MyBarrier();
        // Wait for all *signaled* threads to unblock and destroy this barrier.
        // (See 'wait' and 'timedWait' below for the meaning of *signaled*.)
        // Note that the behavior is undefined if a barrier is destroyed while
        // one or more threads are waiting on it.

    // MANIPULATORS
    void wait();
        // Block until the required number of threads have called either 'wait'
        // or 'timedWait' on this barrier.  Then *signal* all the threads that
        // are currently waiting on this barrier to unblock and reset the state
        // of this barrier to its initial state.  Note that generally 'wait'
        // and 'timedWait' should not be used together, for reasons explained
        // in the documentation of 'timedWait'.

    // ACCESSORS
    int numThreads() const;
        // Return the number of threads that are required to call 'wait' before
        // all waiting threads will unblock.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

// CREATORS
inline
MyBarrier::MyBarrier(int numThreads)
: d_numThreads(numThreads)
, d_numWaiting(0)
, d_sigCount(0)
, d_numPending(0)
{
}

// ACCESSORS
inline
int MyBarrier::numThreads() const
{
    return d_numThreads;
}

MyBarrier::~MyBarrier()
{
    while (1) {

        {
            bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
            if (0 == d_numPending) break;
        }

        bslmt::ThreadUtil::yield();
    }

    BSLS_ASSERT( 0 == d_numWaiting );
}

void MyBarrier::wait()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    int sigCount = d_sigCount;
    if (++d_numWaiting == d_numThreads) {
        ++d_sigCount;
        d_numPending += d_numThreads - 1;
        d_numWaiting = 0;
        d_cond.broadcast();
    }
    else {
        while (d_sigCount == sigCount) {
            d_cond.wait(&d_mutex);
        }
        --d_numPending;
    }
}

// ============================================================================
//                 HELPER CLASSES AND FUNCTIONS  FOR TESTING
// ----------------------------------------------------------------------------

struct ThreadInfo4 {
    MyBarrier   *d_barrier;
    Obj             *d_sem;
    int              d_numIterations;
};

extern "C" void *thread6wait(void *arg)
{
    const ThreadInfo4& t = *(ThreadInfo4 *) arg;

    t.d_barrier->wait();
    for (int i = 0; i < t.d_numIterations; ++i) {
        t.d_sem->wait();
    }

    return arg;
}

extern "C" void *thread4Post(void *arg)
{
    const ThreadInfo4& t = *(ThreadInfo4 *) arg;

    t.d_barrier->wait();
    for (int i = 0; i < t.d_numIterations; ++i) {
        t.d_sem->post();
    }
    t.d_barrier->wait();
    t.d_barrier->wait();
    for (int i = 0; i < t.d_numIterations; ++i) {
        t.d_sem->post();
        if (i % 5 == 0) {
            bslmt::ThreadUtil::microSleep(10);
        }
    }
    return 0;
}

extern "C" void *thread4Wait(void * arg)
{
    ThreadInfo4 *t = (ThreadInfo4 *) arg;

    for (int i = 0; i < t->d_numIterations; ++i) {
        ASSERT(0 != t->d_sem->tryWait());
    }
    t->d_barrier->wait();
    t->d_barrier->wait();
    for (int i = 0; i < t->d_numIterations; ++i) {
        ASSERT(0 == t->d_sem->tryWait());
    }
    t->d_barrier->wait();
    for (int i = 0; i < t->d_numIterations; ++i) {
        if (t->d_sem->tryWait() != 0) {
            --i;
        }
    }
    return 0;
}
struct ThreadInfo3 {
    MyBarrier   *d_barrier;
    Obj             *d_sem;
    int             d_numIterations;
    int             d_numWaitThreads;
};

extern "C" void *thread3Post(void *arg)
{
    const ThreadInfo3& t = *(ThreadInfo3 *) arg;

    t.d_barrier->wait();
    for (int i = 0; i <  (t.d_numWaitThreads * t.d_numIterations / 4); ++i) {
        t.d_sem->post(4);
        if (i % 5 == 0) {
            bslmt::ThreadUtil::microSleep(10);
        }
    }
    return 0;
}

extern "C" void *thread3Wait(void * arg)
{
    ThreadInfo3 *t = (ThreadInfo3 *) arg;

    t->d_barrier->wait();
    for (int i = 0; i < t->d_numIterations; ++i) {
        t->d_sem->wait();
    }
    return 0;
}

struct ThreadInfo2 {
    MyBarrier   *d_barrier;
    Obj             *d_sem;
    int              d_numIterations;
    bsls::AtomicInt  *d_numInitialPosts;
    bsls::AtomicInt  *d_past;
    int              d_verbose;
};

extern "C" void *thread2Post(void *arg) {
    const ThreadInfo2 *t = (ThreadInfo2 *) arg;

    t->d_barrier->wait();
    int n = *t->d_numInitialPosts;
    while (0 < n) {
        if (n == t->d_numInitialPosts->testAndSwap(n, n-1)) {
            if (t->d_verbose) MTCOUT << "\t\tPost from thread "
                                     << bslmt::ThreadUtil::selfIdAsInt()
                                     << MTENDL;
            t->d_sem->post();
        }
        n = *t->d_numInitialPosts;
    }
    if (t->d_verbose) MTCOUT << "\t\tWaiting on barrier." << MTENDL;
    t->d_barrier->wait();
    for (int i = 0; i < t->d_numIterations; ++i) {
        if (t->d_verbose) MTCOUT << "\t\tPost from thread "
                                 << bslmt::ThreadUtil::selfIdAsInt() << MTENDL;
        t->d_sem->post();
        if (i % 5 == 0) {
            bslmt::ThreadUtil::microSleep(10);
        }
    }
    return 0;
}

extern "C" void *thread2Wait(void * arg) {
    ThreadInfo2 *t = (ThreadInfo2 *) arg;

    for (int i = 0; i < t->d_numIterations; ++i) {
        if (t->d_verbose) MTCOUT << "\t\tWait initiated from thread "
                                 << bslmt::ThreadUtil::selfIdAsInt() << MTENDL;
        t->d_sem->wait();
        if (t->d_verbose) MTCOUT << "\t\tWait completed from thread "
                                 << bslmt::ThreadUtil::selfIdAsInt() << MTENDL;
        ++*t->d_past;
    }
    return 0;
}

class IntQueue {
    // FIFO integer queue.

    bsl::deque<int> d_queue;
    Obj             d_mutexSem;
    Obj             d_resourceSem;

    private:
    // not implemented
    IntQueue(const IntQueue&);
    IntQueue& operator=(const IntQueue&);

    public:
    // CREATORS
    explicit IntQueue(bslma::Allocator *basicAllocator = 0);
        // Create a new 'IntQueue' object.

    ~IntQueue();
        // Destroy this object.

    // MANIPULATORS
    int getInt();
        // Get an integer from the queue.

    void pushInt(int number);
        // Push the specified 'number' to the queue.
};

IntQueue::IntQueue(bslma::Allocator *basicAllocator)
: d_queue(basicAllocator)
, d_mutexSem(0)
, d_resourceSem(0)
{
    d_mutexSem.post(); // Initialized to 1 to enforce exclusive access to to
                       // the queue.
}

IntQueue::~IntQueue()
{
    d_mutexSem.wait();
}

int IntQueue::getInt()
{
    // Waiting for resources.
    d_resourceSem.wait();

    // 'd_mutexSem' is used for exclusive access.
    d_mutexSem.wait();
    int ret = d_queue.back();
    d_queue.pop_back();
    d_mutexSem.post();

    return ret;
}

void IntQueue::pushInt(int n)
{
    d_mutexSem.wait();
    d_queue.push_front(n);
    d_mutexSem.post();
    d_resourceSem.post();
}

struct BenchData {
    bslmt::ThreadUtil::Handle    handle;
    Obj                        *resource;
    Obj                        *queue;
    int                         count;
    bool                        stop;
};

extern "C" void *benchConsumer(void* arg) {
    BenchData *data = (BenchData*)arg;
    while(true) {
        data->resource->wait();
        data->queue->post();
//        cout << "-" << flush;
        if (data->stop) return 0;
        ++data->count;
    }
}

extern "C" void *benchProducer(void* arg) {
    BenchData *data = (BenchData*)arg;
    while(true) {
        data->queue->wait();
        data->resource->post();
//        cout << "+" << flush;
        if (data->stop) return 0;
        ++data->count;
    }
    return 0;
}

static const char* fmt(int n) {
    int f; char c=0;
    if (n>=1000) { f=n/10; n/=1000; f-=n*100; c='K'; }
    if (n>=1000) { f=n/10; n/=1000; f-=n*100; c='M'; }
    if (n>=1000) { f=n/10; n/=1000; f-=n*100; c='G'; }
    static char buf[0x100];
    if (c) {
        sprintf(buf, "%d.%02d%c", n, f, c);
    } else {
        sprintf(buf, "%d", n);
    }
    return buf;
}

void *createSemaphoresWorker(void *arg)
{
    bsl::vector<bsls::ObjectBuffer<Obj> > semaphores(10);

    for (int i = 0; i != 1000; ++i) {
        // create a bunch of semaphores
        for (int j = 0; j != semaphores.size(); ++j) {
            new (semaphores[j].buffer()) Obj(i);
        }

        // use semaphores
        for (int j = 0; j != semaphores.size(); ++j) {
            int result = semaphores[j].object().tryWait();
            ASSERT((i == 0) == (result != 0)); // lock fails on initial count 0
        }

        // delete semaphores
        for (int j = 0; j != semaphores.size(); ++j) {
            semaphores[j].object().~Obj();
        }
    }

    return 0;
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
    case 7: {
        // --------------------------------------------------------------------
        // TESTING CONCURRENT SEMAPHORE CREATION
        //
        // Concerns:
        // 1. On Darwin the creation of the semaphore object is synchronized
        //    because it's implemented via named semaphores.  Concurrent
        //    creation of multiple semaphores should not lead to invalid
        //    semaphore objects or deadlock.
        //
        // Plan:
        // 1. In multiple threads, create a number of semaphore objects and
        //    verify that they are valid.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing concurrent creation\n"
                          << "===========================\n";

        vector<bslmt::ThreadUtil::Handle> threads(16);

        for (int i = 0; i != threads.size(); ++i) {
            int rc = bslmt::ThreadUtil::create(&threads[i],
                                              &createSemaphoresWorker,
                                              NULL);
            ASSERT(rc == 0);
        }

        for (int i = 0; i != threads.size(); ++i) {
            bslmt::ThreadUtil::join(threads[i]);
        }

    } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING MULTIPLE POST
        //
        // Concern: that 'post(n)' for large n works properly.  Two test modes:
        // when threads are not waiting, and when they are concurrently
        // waiting.
        // --------------------------------------------------------------------
        enum {
            k_NUM_POST = 1048704,
            k_NUM_WAIT_THREADS = 8
        };

        BSLMF_ASSERT(0 == k_NUM_POST % k_NUM_WAIT_THREADS);

        Obj sem(0);

        bslmt::ThreadUtil::Handle threads[k_NUM_WAIT_THREADS];
        MyBarrier barrier(k_NUM_WAIT_THREADS+1);

        struct ThreadInfo4 info;
        info.d_numIterations = k_NUM_POST / k_NUM_WAIT_THREADS;
        info.d_barrier = &barrier;
        info.d_sem = &sem;
        for (int i = 0; i < k_NUM_WAIT_THREADS; ++i) {
            int rc = bslmt::ThreadUtil::create(&threads[i],
                                              thread6wait,
                                              &info);
            ASSERT(0 == rc);
        }

        if (verbose) {
            bsl::cout << "case 6 mode 1: concurrent waiting"
                      << bsl::endl;
        }

        // MODE 1: THREADS WAITING

        barrier.wait();
        bslmt::ThreadUtil::microSleep(10000); // 10 ms
        sem.post(k_NUM_POST);
        for (int i = 0; i < k_NUM_WAIT_THREADS; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::join(threads[i]));
        }

        // if we reach here, we woke up all the threads the correct number of
        // times

        for (int i = 0; i < k_NUM_WAIT_THREADS; ++i) {
            int rc = bslmt::ThreadUtil::create(&threads[i],
                                              thread6wait,
                                              &info);
            ASSERT(0 == rc);
        }

        if (verbose) {
            bsl::cout << "case 6 mode 2: no concurrent waiting"
                      << bsl::endl;
        }

        // MODE 2: NO THREADS WAITING

        sem.post(k_NUM_POST);
        barrier.wait();
        for (int i = 0; i < k_NUM_WAIT_THREADS; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::join(threads[i]));
        }

        // if we reach here, we woke up all the threads the correct number of
        // times

      } break;

      case 5: {
///Usage
///-----
// This component is an implementation detail of 'bslmt' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

        // USAGE EXAMPLE
        IntQueue testQueue;

        testQueue.pushInt(1);
        ASSERT(1 == testQueue.getInt());
        testQueue.pushInt(2);
        ASSERT(2 == testQueue.getInt());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'tryWait'
        //
        // Concerns:
        //   1. 'tryWait' decrements the count if resources are available,
        //      or return an error otherwise.
        //
        // Plan:
        // We create two groups of threads.  One will call 'post', the other
        // 'tryWait'.  First, we make sure that 'tryWait' fails if no resources
        // is available.  Then we will make sure it succeeds if resources are.
        // We will also test 'tryWait' in the steady state works fine.
        //
        // Testing:
        //   void tryWait();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'trywait'" << endl
                          << "=================" << endl;

        bslmt::ThreadUtil::Handle threads[10];
        MyBarrier barrier(10);
        Obj sem(0);

        struct ThreadInfo4 info;
        info.d_numIterations = 5000; // number of ops per thread / 3
        info.d_barrier = &barrier;
        info.d_sem = &sem;

        for (int i = 0; i < 5; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::create(&threads[i * 2],
                                                 thread4Post,
                                                 &info));

            ASSERT(0 == bslmt::ThreadUtil::create(&threads[i * 2 + 1],
                                                   thread4Wait,
                                                   &info));
        }
        for (int i = 0; i < 10; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::join(threads[i]));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'post(int)'
        //
        // Concerns:
        //   1. post(int) increments the count by the expected number
        //
        // Plan:
        // Create a set of threads calling 'wait' and use a thread to post a
        // number smaller than the set of threads.
        //
        // Testing:
        //   void post(int number);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'post(int number)'" << endl
                          << "==========================" << endl;

        bslmt::ThreadUtil::Handle threads[6];
        MyBarrier barrier(6);
        Obj sem(0);

        struct ThreadInfo3 info;
        info.d_numIterations = 10000; // number of ops per thread
        info.d_numWaitThreads = 5;
        info.d_barrier = &barrier;
        info.d_sem = &sem;

        for (int i = 0; i < 5; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::create(&threads[i],
                                                 thread3Wait,
                                                 &info));
        }
        ASSERT(0 == bslmt::ThreadUtil::create(&threads[5],
                                             thread3Post,
                                             &info));
        for (int i = 0; i < 6; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::join(threads[i]));
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'wait' and 'post'
        //
        // Concerns:
        //   1. wait() blocks the thread when no resource is available,
        //      and then decrements the count
        //   2. post() increments the count
        //
        // Plan:
        //  Create two groups of threads: one will call 'post' and the other
        //  will call 'wait'.  To address concern 1, we will use a barrier and
        //  a counter to make sure that waiting threads are blocked into wait
        //  state before any calls to 'post'.  After that, we will post a small
        //  limited number of times (between 0 and 5), and check that the
        //  semaphore can indeed satisfy that number of waiters.  Then we try
        //  to reach a steady state by calling the two functions 'post' and
        //  'wait' in a number of threads each, perturbing the 'post' operation
        //  by adding small delays to exercise different parts of the code.
        //
        // Testing:
        //   void post();
        //   void wait(int *signalInterrupted = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'wait' and 'post'" << endl
                          << "=========================" << endl;

        enum {
            k_NUM_POSTERS = 5,
            k_NUM_WAITERS = 5
        };

        for (int n = 0; n < 5; ++n) {
            if (veryVerbose) cout << "\tPosting " << n << " first." << endl;

            bslmt::ThreadUtil::Handle threads[k_NUM_POSTERS + k_NUM_WAITERS];
            MyBarrier barrier(k_NUM_POSTERS+ 1);
            bsls::AtomicInt posts(n);
            bsls::AtomicInt past (0);
            Obj sem(0);

            struct ThreadInfo2 info;
            info.d_numIterations = 1000; // number of ops per thread
            info.d_barrier = &barrier;
            info.d_sem = &sem;
            info.d_past = &past;
            info.d_numInitialPosts = &posts;
            info.d_verbose = veryVeryVerbose;

            for (int i = 0; i < k_NUM_POSTERS; ++i) {
                ASSERT(0 == bslmt::ThreadUtil::create(&threads[i],
                                                     thread2Post,
                                                     &info));
            }

            for (int i = 0; i < k_NUM_WAITERS; ++i) {
                ASSERT(0 == bslmt::ThreadUtil::create(
                             &threads[i + k_NUM_POSTERS], thread2Wait, &info));
            }
            bslmt::ThreadUtil::microSleep(1000 * 100);
            ASSERT(0 == past);
            ASSERT(0 == past);
            barrier.wait();
            // Wait until the initial posters complete.
            if (veryVerbose) cout << "\t\tFirst barrier passed." << endl;
            while (n != past) {
                // Wait for some waiters to grab the posts.
                if (veryVeryVerbose)
                    MTCOUT << "\t\tWaiters still blocking, "
                           << posts << "." << MTENDL;
                bslmt::ThreadUtil::microSleep(1000 * 100);
            }
            barrier.wait();
            // Unleash the remaining posters.
            if (veryVerbose) cout << "\t\tSecond barrier passed." << endl;

            // The testing will complete once all the threads join, meaning
            // that all the waiters were satisfied.

            for (int i = 0; i < 10; ++i) {
                ASSERT(0 == bslmt::ThreadUtil::join(threads[i]));
            }
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
#if defined(BSLS_PLATFORM_OS_AIX) || defined(BSLS_PLATFORM_OS_LINUX)
            cout << "INFO: SEM_VALUE_MAX=" << SEM_VALUE_MAX
                 << endl;
#endif
        }
        {
            Obj X(0);
            X.post();
            X.post(2);
            X.wait();
            X.wait();
            ASSERT(0 == X.tryWait());
            ASSERT(0 != X.tryWait());
        }
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // A SIMPLE BENCHMARK
        //
        // imitates a producer-consumer system with a fixed size queue using
        // two semaphores
        //

        int numProducers = atoi(argv[2]);
        int numConsumers = atoi(argv[3]);
        int queueSize = atoi(argv[4]);
        int seconds = 5;
        int samples = 5;
        if (verbose) cout << endl
                          << "Benchmarking....." << endl
                          << "=================" << endl
                          << "producers=" << numProducers << endl
                          << "consumers=" << numConsumers << endl
                          << "queue size=" << queueSize << endl;
        BenchData* producerData = new BenchData[numProducers];
        BenchData* consumerData = new BenchData[numConsumers];
        Obj resource(0);
        Obj queue(0);
        queue.post(queueSize);
        for(int i=0; i<numConsumers; i++) {
            consumerData[i].resource = &resource;
            consumerData[i].queue = &queue;
            consumerData[i].count = 0;
            consumerData[i].stop = false;
            bslmt::ThreadUtil::create(&consumerData[i].handle,
                                     benchConsumer,
                                     (void*)(consumerData+i));
        }
        for(int i=0; i<numProducers; i++) {
            producerData[i].resource = &resource;
            producerData[i].queue = &queue;
            producerData[i].stop = false;
            bslmt::ThreadUtil::create(&producerData[i].handle,
                                     benchProducer,
                                     (void*)(producerData+i));
        }
        for(int j=0; j<samples; j++) {
            bsls::Types::Int64 timeStart = bsls::TimeUtil::getTimer();
            bsls::Types::Int64 timeStartCPU = ::clock();
            int* consumerCount = new int[numConsumers];
            for(int i=0; i<numConsumers; i++) {
                consumerCount[i] = consumerData[i].count;
            }
            bsls::Types::Int64 throughput;
            bsls::Types::Int64 throughputCPU;
            for(int i=0; i<seconds; i++) {
                bslmt::ThreadUtil::microSleep(1000000);
                bsls::Types::Int64 totalMessages = 0;
                for(int i=0; i<numConsumers;i++) {
                    totalMessages += (consumerData[i].count-consumerCount[i]);
                }
                bsls::Types::Int64 elapsed_us =
                                   (bsls::TimeUtil::getTimer()-timeStart)/1000;
                bsls::Types::Int64 elapsed_usCPU = ::clock()-timeStartCPU;
                throughput = (totalMessages*1000000/elapsed_us);
                throughputCPU = (totalMessages*1000000/elapsed_usCPU);
                cout << "testing: "
                     << elapsed_us/1000
                     << " ms, "
                     << elapsed_usCPU*100/elapsed_us
                     << " CPU%, "
                     << totalMessages
                     << " msg, "
                     << fmt(throughput)
                     << " msg/s, "
                     << fmt(throughputCPU)
                     << " msg/CPUs"
                     << endl;
            }
            cout << "====== final:"
                 << fmt(throughput)
                 << " msg/s, "
                 << fmt(throughputCPU)
                 << " msg/CPUs\n"
                 << endl;
        }
        cout << "stopping: " << flush;
        for(int i=0; i<numProducers; i++) {
            producerData[i].stop = true;
        }
        for(int i=0; i<numProducers; i++) {
            bslmt::ThreadUtil::join(producerData[i].handle);
            cout << 'p' << flush;
        }
        for(int i=0; i<numConsumers; i++) {
            consumerData[i].stop = true;
        }
        resource.post(numConsumers);
        for(int i=0; i<numConsumers;i++) {
            bslmt::ThreadUtil::join(consumerData[i].handle);
            cout << 'c' << flush;
        }
        cout << endl;
        delete[] producerData;
        delete[] consumerData;
      } break;

      default: {
          testStatus = -1; break;
      }

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
