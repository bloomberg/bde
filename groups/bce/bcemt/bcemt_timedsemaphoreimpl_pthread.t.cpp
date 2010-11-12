// bcemt_timedsemaphoreimpl_pthread.t.cpp                             -*-C++-*-
#include <bcemt_timedsemaphoreimpl_pthread.h>

#include <bcemt_lockguard.h>   // for testing only
#include <bcemt_mutex.h>       // for testing only
#include <bcemt_threadutil.h>  // for testing only

#include <bces_atomictypes.h>

#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bsls_platform.h>

#include <bsl_deque.h>
#include <bsl_iostream.h>

#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

#if defined (BSLS_PLATFORM__OS_UNIX) && \
    !(defined(BSLS_PLATFORM__OS_SUNOS)   ||     \
      defined(BSLS_PLATFORM__OS_SOLARIS) ||     \
      defined(BSLS_PLATFORM__OS_LINUX))

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [1] Breathing test
// [2] wait(int *signalInterrupted = 0)
// [2] post()
// [3] timedWait(bdet_TimeInterval timeout, int *signalInterrupted = 0)
// [4] post(int number)
// [5] tryWait()
// [6] USAGE Example
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define NL()  cout << endl;                   // Print newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bcemt_TimedSemaphoreImpl<bces_Platform::PthreadTimedSemaphore> Obj;

//=============================================================================
//                         HELPER CLASSES AND FUNCTIONS  FOR TESTING
//-----------------------------------------------------------------------------

class MyCondition {
    // This class defines a platform-independent condition variable.  Using
    // bcemt Condition would create a dependency cycle.
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
    int wait(bcemt_Mutex *mutex)
    {
        return pthread_cond_wait(&d_cond, &mutex->nativeMutex());
    }

    void broadcast()
    {
        pthread_cond_broadcast(&d_cond);
    }
};

class MyBarrier {
    // This class defines a thread barrier.  This is a cut-and-paste of bcemt
    // Barrier, but depending on bcemt Barrier itself here would cause a
    // dependency cycle.

    bcemt_Mutex     d_mutex;      // mutex used to control access to this
                                  // barrier.
    MyCondition d_cond;       // condition variable used for signaling
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

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

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
            bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
            if (0 == d_numPending) break;
        }

        bcemt_ThreadUtil::yield();
    }

    BSLS_ASSERT( 0 == d_numWaiting );
}

void MyBarrier::wait()
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
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

struct ThreadInfo5 {
    MyBarrier        *d_barrier;
    Obj *d_sem;
    int                   d_numIterations;
};

extern "C" void *thread5Post(void *arg)
{
    const ThreadInfo5& t = *(ThreadInfo5 *) arg;

    t.d_barrier->wait();
    for (int i = 0; i < t.d_numIterations; ++i) {
        t.d_sem->post();
    }
    t.d_barrier->wait();
    t.d_barrier->wait();
    for (int i = 0; i < t.d_numIterations; ++i) {
        t.d_sem->post();
        if (i % 5 == 0) {
            bcemt_ThreadUtil::microSleep(10);
        }
    }
    return 0;
}

extern "C" void *thread5Wait(void * arg)
{
    ThreadInfo5 *t = (ThreadInfo5 *) arg;

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
struct ThreadInfo4 {
    MyBarrier        *d_barrier;
    Obj *d_sem;
    int                   d_numIterations;
    int                   d_numWaitThreads;
};

extern "C" void *thread4Post(void *arg)
{
    const ThreadInfo4& t = *(ThreadInfo4 *) arg;

    t.d_barrier->wait();
    for (int i = 0; i <  (t.d_numWaitThreads * t.d_numIterations / 4); ++i) {
        t.d_sem->post(4);
        if (i % 5 == 0) {
            bcemt_ThreadUtil::microSleep(10);
        }
    }
    return 0;
}

extern "C" void *thread4Wait(void * arg)
{
    ThreadInfo4 *t = (ThreadInfo4 *) arg;

    t->d_barrier->wait();
    for (int i = 0; i < t->d_numIterations; ++i) {
        t->d_sem->wait();
    }
    return 0;
}

struct ThreadInfo3 {
    MyBarrier        *d_barrier;
    Obj *d_sem;
    int                   d_numIterations;
};

extern "C" void *thread3Post(void *arg) {
    const ThreadInfo3& t = *(ThreadInfo3 *) arg;

    t.d_barrier->wait();
    for (int i = 0; i < t.d_numIterations; ++i) {
        t.d_sem->post();
        if (i % 5 == 0) {
            bcemt_ThreadUtil::microSleep(10);
        }
    }
    t.d_barrier->wait();
    for (int i = 0; i < t.d_numIterations; ++i) {
        t.d_sem->post();
        if (i % 5 == 0) {
            bcemt_ThreadUtil::microSleep(10);
        }
    }
    return 0;
}

extern "C" void *thread3Wait(void * arg) {
    ThreadInfo3 *t = (ThreadInfo3 *) arg;

    ASSERT(0 != t->d_sem->timedWait(bdetu_SystemTime::now() +
                                    bdet_TimeInterval(0, 1000 * 100)));

    t->d_barrier->wait();
    for (int i = 0; i < t->d_numIterations; ++i) {
        ASSERT(0 == t->d_sem->timedWait(bdetu_SystemTime::now() +
                                        bdet_TimeInterval(5)));
    }
    t->d_barrier->wait();
    for (int i = 0; i < t->d_numIterations; ++i) {
        if (0 != t->d_sem->timedWait(bdetu_SystemTime::now() +
                                     bdet_TimeInterval(0, 1000 * 20))) {
            --i;
        }
    }
    return 0;
}

struct ThreadInfo2 {
    MyBarrier        *d_barrier;
    Obj *d_sem;
    int                   d_numIterations;
    bces_AtomicInt       *d_past;
};

extern "C" void *thread2Post(void *arg) {
    const ThreadInfo2& t = *(ThreadInfo2 *) arg;

    t.d_barrier->wait();
    for (int i = 0; i < t.d_numIterations; ++i) {
        t.d_sem->post();
        if (i % 5 == 0) {
            bcemt_ThreadUtil::microSleep(10);
        }
    }
    return 0;
}

extern "C" void *thread2Wait(void * arg) {
    ThreadInfo2 *t = (ThreadInfo2 *) arg;

    for (int i = 0; i < t->d_numIterations; ++i) {
        t->d_sem->wait();
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
    explicit IntQueue(bslma_Allocator *basicAllocator = 0);
        // Create a new 'IntQueue' object.

    ~IntQueue();
        // Destroy this object.

    // MANIPULATORS
    int getInt();
        // Get an integer from the queue.

    void pushInt(int number);
        // Push the specified 'number' to the queue.
};

IntQueue::IntQueue(bslma_Allocator *basicAllocator)
: d_queue(basicAllocator)
{
    d_mutexSem.post(); // Initialized to 1 to enforce exclusive access to
                       // to the queue.
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

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // USAGE EXAMPLE
        IntQueue testQueue;

        testQueue.pushInt(1);
        ASSERT(1 == testQueue.getInt());
        testQueue.pushInt(2);
        ASSERT(2 == testQueue.getInt());

      } break;
      case 5: {
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

        bcemt_ThreadUtil::Handle threads[10];
        MyBarrier barrier(10);
        Obj sem;

        struct ThreadInfo5 info;
        info.d_numIterations = 5000; // number of ops per thread / 3
        info.d_barrier = &barrier;
        info.d_sem = &sem;

        for (int i = 0; i < 5; ++i) {
            ASSERT(0 == bcemt_ThreadUtil::create(&threads[i * 2],
                                                 thread5Post,
                                                 &info));

            ASSERT(0 == bcemt_ThreadUtil::create(&threads[i * 2 + 1],
                                                   thread5Wait,
                                                   &info));
        }
        for (int i = 0; i < 10; ++i) {
            ASSERT(0 == bcemt_ThreadUtil::join(threads[i]));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'post(int)'
        //
        // Concerns:
        //   1. post(int) increments the count by the expected number
        //
        // Plan:
        // Create a set of threads calling 'wait' and use a thread to post
        // a number smaller than the set of threads.
        //
        // Testing:
        //   void post(int number);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'post(int number)'" << endl
                          << "==========================" << endl;

        bcemt_ThreadUtil::Handle threads[6];
        MyBarrier barrier(6);
        Obj sem;

        struct ThreadInfo4 info;
        info.d_numIterations = 10000; // number of ops per thread
        info.d_numWaitThreads = 5;
        info.d_barrier = &barrier;
        info.d_sem = &sem;

        for (int i = 0; i < 5; ++i) {
            ASSERT(0 == bcemt_ThreadUtil::create(&threads[i],
                                                 thread4Wait,
                                                 &info));
        }
        ASSERT(0 == bcemt_ThreadUtil::create(&threads[5],
                                             thread4Post,
                                             &info));
        for (int i = 0; i < 6; ++i) {
            ASSERT(0 == bcemt_ThreadUtil::join(threads[i]));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'timedWait'
        //
        // Concerns:
        //   1. timedWait() blocks the thread until a resource is available
        //      or the timeout expires.
        //
        // Plan:
        //  Create two groups of threads one will call 'post' and the other
        //  will call 'timedWait'.  First, we will make sure that the
        //  'timedWait' will timeout properly if no resource available by
        //  calling the function with a reasonable timeout before any calls to
        //  'post'.  Then, both groups of threads will enter a loop to simulate
        //  the steady state.  The 'post' loop will be perturbed to exercise
        //  different portions of code.  The specified timeout will be pretty
        //  important and we will make sure we do not timeout.  At the end
        //  of this first run, we will make a second run with a much lower
        //  timeout which will force *some* waits to timeout.
        //
        // Testing:
        //   void timedWait(bdet_TimeInterval timeout,
        //                  int              *signalInterrupted = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'timedWait'" << endl
                          << "===================" << endl;

        bcemt_ThreadUtil::Handle threads[10];
        MyBarrier barrier(10);
        Obj sem;

        struct ThreadInfo3 info;
        info.d_numIterations = 2000; // half the number of ops per thread
        info.d_barrier = &barrier;
        info.d_sem = &sem;

        for (int i = 0; i < 5; ++i) {
            ASSERT(0 == bcemt_ThreadUtil::create(&threads[i * 2],
                                                 thread3Post,
                                                 &info));

            ASSERT(0 == bcemt_ThreadUtil::create(&threads[i * 2 + 1],
                                                 thread3Wait,
                                                 &info));
        }
        for (int i = 0; i < 10; ++i) {
            ASSERT(0 == bcemt_ThreadUtil::join(threads[i]));
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
        //  Create two groups of threads one will call 'post' and the other
        //  will call 'wait'.  To address concern 1, we will use a barrier and
        //  a counter to make sure that waiting threads are blocked into wait
        //  state before any calls to 'post'.  After that, we will try to reach
        //  a steady state by calling the two functions in a loop and perturb
        //  the 'post' operation by adding small delays to exercise different
        //  parts of the code.
        //
        // Testing:
        //   void post();
        //   void wait(int *signalInterrupted = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'wait' and 'post'" << endl
                          << "=========================" << endl;

        bcemt_ThreadUtil::Handle threads[10];
        MyBarrier barrier(6);
        bces_AtomicInt past = 0;
        Obj sem;

        struct ThreadInfo2 info;
        info.d_numIterations = 10000; // number of ops per thread
        info.d_barrier = &barrier;
        info.d_sem = &sem;
        info.d_past = &past;

        for (int i = 0; i < 5; ++i) {
            ASSERT(0 == bcemt_ThreadUtil::create(&threads[i * 2],
                                                 thread2Post,
                                                 &info));

            ASSERT(0 == bcemt_ThreadUtil::create(&threads[i * 2 + 1],
                                                 thread2Wait,
                                                 &info));
        }
        bcemt_ThreadUtil::microSleep(1000 * 100);
        ASSERT(0 == past);
        barrier.wait();
        bcemt_ThreadUtil::microSleep(1000 * 200);
        ASSERT(0 != past);

        for (int i = 0; i < 10; ++i) {
            ASSERT(0 == bcemt_ThreadUtil::join(threads[i]));
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Breathing Test" << endl
                          << "==============" << endl;
        {
            Obj X;
            X.post();
            X.post(2);
            X.wait();
            ASSERT(0 == X.timedWait(bdetu_SystemTime::now() +
                                    bdet_TimeInterval(60)));
            ASSERT(0 == X.tryWait());
            ASSERT(0 != X.tryWait());
            ASSERT(0 != X.timedWait(bdetu_SystemTime::now() +
                                    bdet_TimeInterval(1)));
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

#else  // not pthread

int main()
{
    return -1;
}

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
