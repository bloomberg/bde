// bcemt_semaphoreimpl_win32.t.cpp                                    -*-C++-*-
#include <bcemt_semaphoreimpl_win32.h>

#include <bcemt_lockguard.h>   // for testing only
#include <bcemt_mutex.h>       // for testing only
#include <bcemt_threadutil.h>  // for testing only

#include <bces_atomictypes.h>
#include <bces_platform.h>

#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bsls_timeutil.h>

#include <bsl_deque.h>
#include <bsl_iostream.h>

#include <bsl_cstdlib.h>

#ifdef BCES_PLATFORM_WIN32_THREADS

#include <windows.h>

#include <bsl_c_time.h>
#include <bsl_c_stdio.h>

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

static bcemt_Mutex coutMutex;

#define MTCOUT   { coutMutex.lock(); cout << bcemt_ThreadUtil::selfIdAsInt() \
                                          << ": "
#define MTENDL   endl;  coutMutex.unlock(); }
#define MTFLUSH  bsl::flush; } coutMutex.unlock()

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bcemt_SemaphoreImpl<bces_Platform::Win32Semaphore> Obj;

class MyCondition {
    // This class defines a platform-independent condition variable.  Using
    // bcemt Condition would create a dependency cycle.
private:
    HANDLE           d_semBlockLock;       // gate Semaphore

    HANDLE           d_semBlockQueue;      // main Semaphore
                                           // (handle to a semaphore used for
                                           // the actual signaling)

    CRITICAL_SECTION d_mtxUnblockLock;     //

    volatile LONG    d_waitersBlocked;     // count of how many threads are
                                           // currently waiting for this
                                           // condition

    volatile LONG    d_waitersToUnblock;   // count of how many threads we have
                                           // to unblock

    volatile LONG    d_waitersGone;        // count of how many threads are
                                           // left in waiting state due
                                           // timeouts or errors

  private:
    // NOT IMPLEMENTED
    MyCondition(const MyCondition&);
    MyCondition& operator=(const MyCondition&);

  public:
    // CREATORS
    MyCondition()
        // Create a condition variable.
        : d_semBlockLock(0)
        , d_semBlockQueue(0)
        , d_mtxUnblockLock()
        , d_waitersBlocked(0)
        , d_waitersToUnblock(0)
        , d_waitersGone(0)
    {
        InitializeCriticalSection(&d_mtxUnblockLock);

        d_semBlockLock  = CreateSemaphore(0,          // security attributes
                1,          // initial count
                              0x7FFFFFFF, // max count
                              0);         // name

        d_semBlockQueue = CreateSemaphore(0,          // security attributes
                                  0,          // initial count
                                  0x7FFFFFFF, // max count
                                  0);         // name
    }

    ~MyCondition()
        // Destroy this object.
    {
        EnterCriticalSection(&d_mtxUnblockLock);
        CloseHandle(d_semBlockLock);
        CloseHandle(d_semBlockQueue);
        LeaveCriticalSection(&d_mtxUnblockLock);
        DeleteCriticalSection(&d_mtxUnblockLock);
    }

    // MANIPULATORS
    int wait(bcemt_Mutex *mutex)
    {
        WaitForSingleObject(d_semBlockLock, INFINITE);
        ++d_waitersBlocked;
        ReleaseSemaphore(d_semBlockLock, 1, 0);

        mutex->unlock();

        DWORD rc = WaitForSingleObject(d_semBlockQueue, INFINITE);

        EnterCriticalSection(&d_mtxUnblockLock);

        LONG nSignalsWasLeft = d_waitersToUnblock;
        LONG nWaitersWasGone = 0;

        if (nSignalsWasLeft != 0) {

            if (rc != WAIT_OBJECT_0) {         // timeout or errors
                if (d_waitersBlocked != 0) {
                    --d_waitersBlocked;
                }
                else {
                    ++d_waitersGone;         // spurious wakeup pending!!
                }
            }

            if (0 == --d_waitersToUnblock) {
                if (0 != d_waitersBlocked) {
                    ReleaseSemaphore(d_semBlockLock,1 ,0 );  // open the gate
                    nSignalsWasLeft = 0;       // do not open the gate below
                    // again
                }
                else if ( 0 != (nWaitersWasGone = d_waitersGone) ) {
                    d_waitersGone = 0;
                }
            }
        }
        else if (INT_MAX/2 == ++d_waitersGone) {       // timeout/canceled or
            // spurious semaphore
            WaitForSingleObject(d_semBlockLock, INFINITE);

            d_waitersBlocked -= d_waitersGone;        // something is going on
            // here - test of timeouts?
            ReleaseSemaphore(d_semBlockLock, 1, 0);
            d_waitersGone = 0;
        }
        LeaveCriticalSection(&d_mtxUnblockLock);
        if (1 == nSignalsWasLeft) {
            while(nWaitersWasGone > 0) {
                --nWaitersWasGone;
                WaitForSingleObject(d_semBlockQueue, INFINITE);
                // better now than spurious later
            }
            ReleaseSemaphore(d_semBlockLock, 1, 0);           // open the gate
        }

        mutex->lock();

        switch (rc) {
            case WAIT_OBJECT_0: return 0;
            case WAIT_TIMEOUT:  return -1;
            default:            break;
        }
        return -2;
    }

    void broadcast()
    {
        LONG nSignalsToIssue = 0;
        EnterCriticalSection(&d_mtxUnblockLock);

        if (0 != d_waitersToUnblock) {    // the gate is closed!!!
            if (0 != d_waitersBlocked) {  // not NO-OP
                nSignalsToIssue = d_waitersBlocked;
                d_waitersToUnblock += nSignalsToIssue;
                d_waitersBlocked = 0;
            }
        }
        else if (d_waitersBlocked > d_waitersGone) { // HARMLESS RACE
                                                     // CONDITION!

            WaitForSingleObject(d_semBlockLock, INFINITE);   // close the gate
            if (0 != d_waitersGone ) {
                d_waitersBlocked -= d_waitersGone;
                d_waitersGone = 0;
            }

            nSignalsToIssue = d_waitersBlocked;
            d_waitersToUnblock = nSignalsToIssue;
            d_waitersBlocked = 0;
        }

        LeaveCriticalSection(&d_mtxUnblockLock);
        if (nSignalsToIssue != 0) {
            ReleaseSemaphore(d_semBlockQueue, nSignalsToIssue, 0);
        }
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

//=============================================================================
//                         HELPER CLASSES AND FUNCTIONS  FOR TESTING
//-----------------------------------------------------------------------------

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
            bcemt_ThreadUtil::microSleep(10);
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
            bcemt_ThreadUtil::microSleep(10);
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
    bces_AtomicInt  *d_numInitialPosts;
    bces_AtomicInt  *d_past;
    int              d_verbose;
};

extern "C" void *thread2Post(void *arg) {
    const ThreadInfo2 *t = (ThreadInfo2 *) arg;

    t->d_barrier->wait();
    int n = *t->d_numInitialPosts;
    while (0 < n) {
        if (n == t->d_numInitialPosts->testAndSwap(n, n-1)) {
            if (t->d_verbose) MTCOUT << "\t\tPost from thread "
                                     << bcemt_ThreadUtil::selfIdAsInt()
                                     << MTENDL;
            t->d_sem->post();
        }
        n = *t->d_numInitialPosts;
    }
    if (t->d_verbose) MTCOUT << "\t\tWaiting on barrier." << MTENDL;
    t->d_barrier->wait();
    for (int i = 0; i < t->d_numIterations; ++i) {
        if (t->d_verbose) MTCOUT << "\t\tPost from thread "
                                 << bcemt_ThreadUtil::selfIdAsInt() << MTENDL;
        t->d_sem->post();
        if (i % 5 == 0) {
            bcemt_ThreadUtil::microSleep(10);
        }
    }
    return 0;
}

extern "C" void *thread2Wait(void * arg) {
    ThreadInfo2 *t = (ThreadInfo2 *) arg;

    for (int i = 0; i < t->d_numIterations; ++i) {
        if (t->d_verbose) MTCOUT << "\t\tWait initiated from thread "
                                 << bcemt_ThreadUtil::selfIdAsInt() << MTENDL;
        t->d_sem->wait();
        if (t->d_verbose) MTCOUT << "\t\tWait completed from thread "
                                 << bcemt_ThreadUtil::selfIdAsInt() << MTENDL;
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
, d_mutexSem(0)
, d_resourceSem(0)
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

struct BenchData {
    bcemt_ThreadUtil::Handle    handle;
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

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) {
      case 6: {
        // --------------------------------------------------------------------
        // TESTING MULTIPLE POST
        //
        // Concern: that 'post(n)' for large n works properly.  Two test modes:
        // when threads are not waiting, and when they are concurrently
        // waiting.
        // --------------------------------------------------------------------
        enum {
            NUM_POST = 1048704,
            NUM_WAIT_THREADS = 8
        };

        BSLMF_ASSERT(0 == NUM_POST % NUM_WAIT_THREADS);

        Obj sem(0);

        bcemt_ThreadUtil::Handle threads[NUM_WAIT_THREADS];
        MyBarrier barrier(NUM_WAIT_THREADS+1);

        struct ThreadInfo4 info;
        info.d_numIterations = NUM_POST / NUM_WAIT_THREADS;
        info.d_barrier = &barrier;
        info.d_sem = &sem;
        for (int i = 0; i < NUM_WAIT_THREADS; ++i) {
            int rc = bcemt_ThreadUtil::create(&threads[i],
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
        bcemt_ThreadUtil::microSleep(10000); // 10 ms
        sem.post(NUM_POST);
        for (int i = 0; i < NUM_WAIT_THREADS; ++i) {
            ASSERT(0 == bcemt_ThreadUtil::join(threads[i]));
        }

        // if we reach here, we woke up all the threads the correct number of
        // times

        for (int i = 0; i < NUM_WAIT_THREADS; ++i) {
            int rc = bcemt_ThreadUtil::create(&threads[i],
                                              thread6wait,
                                              &info);
            ASSERT(0 == rc);
        }

        if (verbose) {
            bsl::cout << "case 6 mode 2: no concurrent waiting"
                      << bsl::endl;
        }

        // MODE 2: NO THREADS WAITING

        sem.post(NUM_POST);
        barrier.wait();
        for (int i = 0; i < NUM_WAIT_THREADS; ++i) {
            ASSERT(0 == bcemt_ThreadUtil::join(threads[i]));
        }

        // if we reach here, we woke up all the threads the correct number of
        // times

      } break;

      case 5: {
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

        bcemt_ThreadUtil::Handle threads[10];
        MyBarrier barrier(10);
        Obj sem(0);

        struct ThreadInfo4 info;
        info.d_numIterations = 5000; // number of ops per thread / 3
        info.d_barrier = &barrier;
        info.d_sem = &sem;

        for (int i = 0; i < 5; ++i) {
            ASSERT(0 == bcemt_ThreadUtil::create(&threads[i * 2],
                                                 thread4Post,
                                                 &info));

            ASSERT(0 == bcemt_ThreadUtil::create(&threads[i * 2 + 1],
                                                   thread4Wait,
                                                   &info));
        }
        for (int i = 0; i < 10; ++i) {
            ASSERT(0 == bcemt_ThreadUtil::join(threads[i]));
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
        Obj sem(0);

        struct ThreadInfo3 info;
        info.d_numIterations = 10000; // number of ops per thread
        info.d_numWaitThreads = 5;
        info.d_barrier = &barrier;
        info.d_sem = &sem;

        for (int i = 0; i < 5; ++i) {
            ASSERT(0 == bcemt_ThreadUtil::create(&threads[i],
                                                 thread3Wait,
                                                 &info));
        }
        ASSERT(0 == bcemt_ThreadUtil::create(&threads[5],
                                             thread3Post,
                                             &info));
        for (int i = 0; i < 6; ++i) {
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
            NUM_POSTERS = 5,
            NUM_WAITERS = 5
        };

        for (int n = 0; n < 5; ++n) {
            if (veryVerbose) cout << "\tPosting " << n << " first." << endl;

            bcemt_ThreadUtil::Handle threads[NUM_POSTERS + NUM_WAITERS];
            MyBarrier barrier(NUM_POSTERS+ 1);
            bces_AtomicInt posts = n;
            bces_AtomicInt past  = 0;
            Obj sem(0);

            struct ThreadInfo2 info;
            info.d_numIterations = 1000; // number of ops per thread
            info.d_barrier = &barrier;
            info.d_sem = &sem;
            info.d_past = &past;
            info.d_numInitialPosts = &posts;
            info.d_verbose = veryVeryVerbose;

            for (int i = 0; i < NUM_POSTERS; ++i) {
                ASSERT(0 == bcemt_ThreadUtil::create(&threads[i],
                                                     thread2Post,
                                                     &info));
            }

            for (int i = 0; i < NUM_WAITERS; ++i) {
                ASSERT(0 == bcemt_ThreadUtil::create(&threads[i + NUM_POSTERS],
                                                     thread2Wait,
                                                     &info));
            }
            bcemt_ThreadUtil::microSleep(1000 * 100);
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
                bcemt_ThreadUtil::microSleep(1000 * 100);
            }
            barrier.wait();
            // Unleash the remaining posters.
            if (veryVerbose) cout << "\t\tSecond barrier passed." << endl;

            // The testing will complete once all the threads join, meaning
            // that all the waiters were satisfied.

            for (int i = 0; i < 10; ++i) {
                ASSERT(0 == bcemt_ThreadUtil::join(threads[i]));
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
        // imitates a producer-consumer system with a fixed size
        // queue using two semaphores
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
            bcemt_ThreadUtil::create(&consumerData[i].handle,
                                     benchConsumer,
                                     (void*)(consumerData+i));
        }
        for(int i=0; i<numProducers; i++) {
            producerData[i].resource = &resource;
            producerData[i].queue = &queue;
            producerData[i].stop = false;
            bcemt_ThreadUtil::create(&producerData[i].handle,
                                     benchProducer,
                                     (void*)(producerData+i));
        }
        for(int j=0; j<samples; j++) {
            bsls_PlatformUtil::Int64 timeStart = bsls_TimeUtil::getTimer();
            bsls_PlatformUtil::Int64 timeStartCPU = ::clock();
            int* consumerCount = new int[numConsumers];
            for(int i=0; i<numConsumers; i++) {
                consumerCount[i] = consumerData[i].count;
            }
            bsls_PlatformUtil::Int64 throughput;
            bsls_PlatformUtil::Int64 throughputCPU;
            for(int i=0; i<seconds; i++) {
                bcemt_ThreadUtil::microSleep(1000000);
                bsls_PlatformUtil::Int64 totalMessages = 0;
                for(int i=0; i<numConsumers;i++) {
                    totalMessages += (consumerData[i].count-consumerCount[i]);
                }
                bsls_PlatformUtil::Int64 elapsed_us =
                                    (bsls_TimeUtil::getTimer()-timeStart)/1000;
                bsls_PlatformUtil::Int64 elapsed_usCPU =
                                                        ::clock()-timeStartCPU;
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
            bcemt_ThreadUtil::join(producerData[i].handle);
            cout << 'p' << flush;
        }
        for(int i=0; i<numConsumers; i++) {
            consumerData[i].stop = true;
        }
        resource.post(numConsumers);
        for(int i=0; i<numConsumers;i++) {
            bcemt_ThreadUtil::join(consumerData[i].handle);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
