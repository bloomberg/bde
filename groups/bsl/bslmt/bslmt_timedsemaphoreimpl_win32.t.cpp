// bslmt_timedsemaphoreimpl_win32.t.cpp                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_timedsemaphoreimpl_win32.h>

#include <bslim_testutil.h>

#include <bslmt_lockguard.h>   // for testing only
#include <bslmt_mutex.h>       // for testing only
#include <bslmt_threadutil.h>  // for testing only

#include <bsls_atomic.h>

#include <bsls_timeinterval.h>
#include <bsls_systemtime.h>
#include <bsls_systemclocktype.h>
#include <bsls_systemtime.h>

#include <bsl_deque.h>
#include <bsl_iostream.h>

#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

#ifdef BSLMT_PLATFORM_WIN32_THREADS

#include <windows.h>

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
// [3] timedWait(bsls::TimeInterval timeout, int *signalInterrupted = 0)
// [4] post(int number)
// [5] tryWait()
// [6] USAGE Example

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

typedef bslmt::TimedSemaphoreImpl<bslmt::Platform::Win32TimedSemaphore> Obj;

// ============================================================================
//                 HELPER CLASSES AND FUNCTIONS  FOR TESTING
// ----------------------------------------------------------------------------

class MyCondition {
    // This class defines a platform-independent condition variable.  Using
    // bslmt Condition would create a dependency cycle.
private:
    HANDLE           d_semBlockLock;       // gate Semaphore

    HANDLE           d_semBlockQueue;      // main Semaphore (handle to a
                                           // semaphore used for the actual
                                           // signaling)

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
    int wait(bslmt::Mutex *mutex)
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
    // This class defines a thread barrier.  This is a cut-and-paste of bslmt
    // Barrier, but depending on bslmt Barrier itself here would cause a
    // dependency cycle.

    bslmt::Mutex     d_mutex;      // mutex used to control access to this
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
            bslmt::ThreadUtil::microSleep(10);
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
            bslmt::ThreadUtil::microSleep(10);
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
    MyBarrier                   *d_barrier;
    Obj                         *d_sem;
    int                          d_numIterations;
    bsls::SystemClockType::Enum  d_clockType;
};

extern "C" void *thread3Post(void *arg) {
    const ThreadInfo3& t = *(ThreadInfo3 *) arg;

    t.d_barrier->wait();
    for (int i = 0; i < t.d_numIterations; ++i) {
        t.d_sem->post();
        if (i % 5 == 0) {
            bslmt::ThreadUtil::microSleep(10);
        }
    }
    t.d_barrier->wait();
    for (int i = 0; i < t.d_numIterations; ++i) {
        t.d_sem->post();
        if (i % 5 == 0) {
            bslmt::ThreadUtil::microSleep(10);
        }
    }
    return 0;
}

extern "C" void *thread3Wait(void * arg) {
    ThreadInfo3 *t = (ThreadInfo3 *) arg;

    ASSERT(0 != t->d_sem->timedWait(bsls::SystemTime::now(t->d_clockType) +
                                    bsls::TimeInterval(0, 1000 * 100)));

    t->d_barrier->wait();
    for (int i = 0; i < t->d_numIterations; ++i) {
        ASSERT(0 ==
               t->d_sem->timedWait(bsls::SystemTime::now(t->d_clockType) +
                                   bsls::TimeInterval(5)));
    }
    t->d_barrier->wait();
    for (int i = 0; i < t->d_numIterations; ++i) {
        if (0 != t->d_sem->timedWait(bsls::SystemTime::now(t->d_clockType) +
                                     bsls::TimeInterval(0, 1000 * 20))) {
            --i;
        }
    }
    return 0;
}

void testCase3(bsls::SystemClockType::Enum clockType)
{
    bslmt::ThreadUtil::Handle threads[10];
    MyBarrier barrier(10);
    Obj sem(clockType);

    struct ThreadInfo3 info;
    info.d_numIterations = 2000; // half the number of ops per thread
    info.d_barrier = &barrier;
    info.d_sem = &sem;
    info.d_clockType = clockType;

    for (int i = 0; i < 5; ++i) {
        ASSERT(0 == bslmt::ThreadUtil::create(&threads[i * 2],
                                             thread3Post,
                                             &info));

        ASSERT(0 == bslmt::ThreadUtil::create(&threads[i * 2 + 1],
                                             thread3Wait,
                                             &info));
    }
    for (int i = 0; i < 10; ++i) {
        ASSERT(0 == bslmt::ThreadUtil::join(threads[i]));
    }
}

struct ThreadInfo2 {
    MyBarrier        *d_barrier;
    Obj *d_sem;
    int                   d_numIterations;
    bsls::AtomicInt       *d_past;
};

extern "C" void *thread2Post(void *arg) {
    const ThreadInfo2& t = *(ThreadInfo2 *) arg;

    t.d_barrier->wait();
    for (int i = 0; i < t.d_numIterations; ++i) {
        t.d_sem->post();
        if (i % 5 == 0) {
            bslmt::ThreadUtil::microSleep(10);
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

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3; int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
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

        bslmt::ThreadUtil::Handle threads[10];
        MyBarrier barrier(10);
        Obj sem;

        struct ThreadInfo5 info;
        info.d_numIterations = 5000; // number of ops per thread / 3
        info.d_barrier = &barrier;
        info.d_sem = &sem;

        for (int i = 0; i < 5; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::create(&threads[i * 2],
                                                 thread5Post,
                                                 &info));

            ASSERT(0 == bslmt::ThreadUtil::create(&threads[i * 2 + 1],
                                                   thread5Wait,
                                                   &info));
        }
        for (int i = 0; i < 10; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::join(threads[i]));
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
        Obj sem;

        struct ThreadInfo4 info;
        info.d_numIterations = 10000; // number of ops per thread
        info.d_numWaitThreads = 5;
        info.d_barrier = &barrier;
        info.d_sem = &sem;

        for (int i = 0; i < 5; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::create(&threads[i],
                                                 thread4Wait,
                                                 &info));
        }
        ASSERT(0 == bslmt::ThreadUtil::create(&threads[5],
                                             thread4Post,
                                             &info));
        for (int i = 0; i < 6; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::join(threads[i]));
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
        //   void timedWait(bsls::TimeInterval timeout,
        //                  int              *signalInterrupted = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'timedWait'" << endl
                          << "===================" << endl;

        testCase3(bsls::SystemClockType::e_REALTIME);
        testCase3(bsls::SystemClockType::e_MONOTONIC);

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

        bslmt::ThreadUtil::Handle threads[10];
        MyBarrier barrier(6);
        bsls::AtomicInt past(0);
        Obj sem;

        struct ThreadInfo2 info;
        info.d_numIterations = 10000; // number of ops per thread
        info.d_barrier = &barrier;
        info.d_sem = &sem;
        info.d_past = &past;

        for (int i = 0; i < 5; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::create(&threads[i * 2],
                                                 thread2Post,
                                                 &info));

            ASSERT(0 == bslmt::ThreadUtil::create(&threads[i * 2 + 1],
                                                 thread2Wait,
                                                 &info));
        }
        bslmt::ThreadUtil::microSleep(1000 * 100);
        ASSERT(0 == past);
        barrier.wait();
        bslmt::ThreadUtil::microSleep(1000 * 200);
        ASSERT(0 != past);

        for (int i = 0; i < 10; ++i) {
            ASSERT(0 == bslmt::ThreadUtil::join(threads[i]));
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
            ASSERT(0 == X.timedWait(bsls::SystemTime::nowRealtimeClock() +
                                    bsls::TimeInterval(60)));
            ASSERT(0 == X.tryWait());
            ASSERT(0 != X.tryWait());
            ASSERT(0 != X.timedWait(bsls::SystemTime::nowRealtimeClock() +
                                    bsls::TimeInterval(1)));
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

#else  // not Windows

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
