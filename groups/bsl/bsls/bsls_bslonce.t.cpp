// bsls_bslonce.t.cpp                                                 -*-C++-*-
#include <bsls_bslonce.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
typedef HANDLE my_thread_t;
#else
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
typedef pthread_t my_thread_t;
#endif

using namespace BloombergLP;
using namespace bsls;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//-----------------------------------------------------------------------------
//
///class BslOnce
///=============
// MANIPULATORS
// [ 2] bool enter();
// [ 2] void leave();
//
///class BslOnceGuard
///==================
// CREATORS
// [ 3] BslOnceGuard();
// [ 3] ~BslOnceGuard();
//
// MANIPULATORS
// [ 3] bool enter(BslOnce *);
// [ 3] void leave();
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] CONCERN: CONCURRENT CALLS TO 'enter'
// [ 5] USAGE EXAMPLE

// ============================================================================
//                    NON-STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

// Note that non-standard assertion macros are required to ensure thread-safe
// assertion output.

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i, bool locked = false);

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef BslOnce      Obj;
typedef BslOnceGuard Guard;

typedef bsls::AtomicOperations                   AtomicOp;
typedef bsls::AtomicOperations::AtomicTypes::Int AtomicOpInt;

// ============================================================================
//                      BASIC CONCURRENCY FRAMEWORK
// ----------------------------------------------------------------------------

extern "C" {
typedef void* (*THREAD_ENTRY)(void *arg);
}

class my_Mutex {
    // This class implements a cross-platform mutual exclusion primitive
    // similar to posix mutexes.
#ifdef BSLS_PLATFORM_OS_WINDOWS
    HANDLE d_mutex;
#else
    pthread_mutex_t d_mutex;
#endif

  public:
    my_Mutex();
        // Construct a 'my_Mutex' object.
    ~my_Mutex();
        // Destroy a 'my_Mutex' object.

    void lock();
        // Lock this mutex.

    void unlock();
        // Unlock this mutex;
};

class my_Conditional {
    // This class implements a cross-platform waitable state indicator used for
    // testing.  It has two states, signaled and non-signaled.  Once
    // signaled('signal'), the state will persist until explicitly 'reset'.
    // Calls to wait, when the state is signaled, will succeed immediately.
#ifdef BSLS_PLATFORM_OS_WINDOWS
    HANDLE d_cond;
#else
    pthread_mutex_t d_mutex;
    pthread_cond_t  d_cond;
    volatile int    d_signaled;
#endif

  public:
    my_Conditional();
        // Create a conditional object.

    ~my_Conditional();
        // Destroy this object/

    void reset();
        // Reset the state of this indicator to non-signaled.

    void signal();
        // Signal the state of the indicator and unblock any threads waiting
        // for the state to be signaled.

    void wait();
        // Wait until the state of this indicator becomes signaled.  If the
        // state is already signaled then return immediately.

    int  timedWait(int timeout);
        // Wait until the state of this indicator becomes signaled or until or
        // for the specified 'timeout'(in milliseconds).  Return 0 if the state
        // is signaled, non-zero if the timeout has expired.  If the state is
        // already signaled then return immediately.
};

class my_Barrier {
    // This class defines a barrier for synchronizing multiple threads.  A
    // barrier is initialized with the number of threads to expect.  Each
    // invocation of 'wait()' will block the active thread until the expected
    // number of threads have entered the barrier, at which point all threads
    // are released and return from 'wait()'.
    //
    // *NOTE*: The 'my_Conditional' operation 'wait' does *not* take a mutex,
    // so it cannot be atomically combined with another operation (e.g.,
    // incrementing the 'd_waiting' counter), it also does not provide a
    // 'broadcast' method, so each thread must be signaled individually.

    // DATA
    my_Conditional d_waitCondition;  // waiting threads wait on this condition
    my_Mutex       d_waitMutex;      // mutex for 'd_waiting' counter
    my_Mutex       d_awakeMutex;     // mutex for 'd_awake' counter
    int            d_waiting;        // # of threads blocked on d_waitCondition
    int            d_awake;          // # of threads exited 'wait'
    volatile int   d_genCounter;     // generation counter
    const int      d_expected;       // number of threads to expected

  public:
    // CREATORS
    explicit my_Barrier(int numThreads);
        // Create a barrier that will synchronize the specified 'numThreads'
        // number of threads.  The behavior is undefined unless
        // '0 < expectedThreads'.

    ~my_Barrier();
        // Destroy this barrier.

    // MANIPULATORS
    void wait();
        // Block the current thread until the number of expectedThreads,
        // supplied at construction, have entered this barrier's 'wait()'
        // method, then return (all blocked threads, including the current
        // thread).
};


template <class LOCK>
class LockGuard {
    // A scoped guard for calling 'lock' and 'unlock' on an object of
    // parameterized type 'LOCK' (presumably a mutex).

    // DATA
    LOCK *d_lock;

  public:
    // CREATORS
    explicit LockGuard(LOCK *lock) : d_lock(lock) { d_lock->lock(); }
        // Construct a lock guard for the specified 'lock', and call 'lock()'
        // on 'lock'.

    ~LockGuard() { d_lock->unlock(); }
        // Destroy this lock guard and call 'unlock()' on the object supplied
        // at construction.
};


// ============================================================================
//                         INLINE DEFINITIONS
// ============================================================================

inline
my_Mutex::my_Mutex()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    d_mutex = CreateMutex(0,FALSE,0);
#else
    pthread_mutex_init(&d_mutex,0);
#endif
}

inline
my_Mutex::~my_Mutex()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    CloseHandle(d_mutex);
#else
    pthread_mutex_destroy(&d_mutex);
#endif
}

inline
void my_Mutex::lock()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    WaitForSingleObject(d_mutex, INFINITE);
#else
    pthread_mutex_lock(&d_mutex);
#endif
}

inline
void my_Mutex::unlock()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    ReleaseMutex(d_mutex);
#else
    pthread_mutex_unlock(&d_mutex);
#endif
}


my_Conditional::my_Conditional()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    d_cond = CreateEvent(0,TRUE,FALSE,0);
#else
    pthread_mutex_init(&d_mutex,0);
    pthread_cond_init(&d_cond,0);
    d_signaled = 0;
#endif
}

my_Conditional::~my_Conditional()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    CloseHandle(d_cond);
#else
    pthread_cond_destroy(&d_cond);
    pthread_mutex_destroy(&d_mutex);
#endif
}

void my_Conditional::reset()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    ResetEvent(d_cond);
#else
    pthread_mutex_lock(&d_mutex);
    d_signaled = 0;
    pthread_mutex_unlock(&d_mutex);
#endif
}

void my_Conditional::signal()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    SetEvent(d_cond);
#else
    pthread_mutex_lock(&d_mutex);
    d_signaled = 1;
    pthread_cond_broadcast(&d_cond);
    pthread_mutex_unlock(&d_mutex);
#endif
}

int my_Conditional::timedWait(int timeout)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    DWORD res = WaitForSingleObject(d_cond,timeout);
    return res == WAIT_OBJECT_0 ? 0 : -1;
#else
    struct timeval now;
    struct timespec tspec;
    int res;

    gettimeofday(&now,0);
    tspec.tv_sec  = now.tv_sec + timeout/1000;
    tspec.tv_nsec = (now.tv_usec + (timeout%1000) * 1000) * 1000;
    pthread_mutex_lock(&d_mutex);
    while ((res = pthread_cond_timedwait(&d_cond,&d_mutex,&tspec)) == 0 &&
           !d_signaled) {
        ;
     }
    pthread_mutex_unlock(&d_mutex);
    return res;
#endif
}

void my_Conditional::wait()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    WaitForSingleObject(d_cond,INFINITE);
#else
    pthread_mutex_lock(&d_mutex);
    while (!d_signaled) pthread_cond_wait(&d_cond,&d_mutex);
    pthread_mutex_unlock(&d_mutex);
#endif
}

static int myCreateThread(my_thread_t  *aHandle,
                          THREAD_ENTRY  aEntry,
                          void         *arg )
    // Create a thread beginning execution at the specified 'aEntry' function
    // having the specified 'arg' function arguments, and load the specified
    // 'aHandle' to the thread so it can be joined later.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    *aHandle = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)aEntry,arg,0,0);
    return *aHandle ? 0 : -1;
#else
    return pthread_create(aHandle, 0, aEntry, arg);
#endif
}

static void  myJoinThread(my_thread_t aHandle)
    // Join the specified 'aHandle'.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    WaitForSingleObject(aHandle,INFINITE);
    CloseHandle(aHandle);
#else
    pthread_join(aHandle,0);
#endif
}

static void mySleep(int milliseconds)
    // Sleep for the specified 'milliseconds'.
{
#if defined(BSLS_PLATFORM_OS_WINDOWS)
    ::SleepEx(milliseconds, 0);
#else
    usleep(milliseconds * 1000);
#endif
}


// CREATORS
my_Barrier::my_Barrier(int numThreads)
: d_waiting(0)
, d_awake(0)
, d_genCounter(0)
, d_expected(numThreads)
{
    BSLS_ASSERT(numThreads > 0);
}

my_Barrier::~my_Barrier()
{
}

// MANIPULATORS
void my_Barrier::wait()
{
    // *NOTE*: The 'my_Conditional' operation 'wait' does *not* take a mutex,
    // so it cannot be atomically combined with another operation (e.g.,
    // incrementing the 'd_waiting' counter), it also does not provide
    // 'broadcast' method, so each thread must be signaled individually.

    // For the first 'd_expected - 1' threads, increment the 'd_waiting'
    // counter, wait on the condition variable, and then increment the
    // 'd_awake' counter.  For the last thread, loop signaling the condition
    // variable until all threads are awake (i.e., 'd_awake == d_expected - 1')
    d_waitMutex.lock();
    int generation = d_genCounter;
    if (d_waiting < d_expected - 1) {
        ++d_waiting;
        d_waitMutex.unlock();
        do {
            d_waitCondition.wait();
        } while (generation == d_genCounter);
        LockGuard<my_Mutex> awakeGuard(&d_awakeMutex);
        ++d_awake;
    }
    else {
        ++d_genCounter;
        int numAwake;
        do {
            d_waitCondition.signal();
            LockGuard<my_Mutex> awakeGuard(&d_awakeMutex);
            numAwake = d_awake;
        } while(numAwake < d_expected - 1);

        // Reset the counters so that the barrier can be used again.
        d_waitCondition.reset();
        d_awake   = 0;
        d_waiting = 0;
        d_waitMutex.unlock();
    }
}

//=============================================================================
//                    NON-STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static my_Mutex aSsErT_mutex;

static void aSsErT(int c, const char *s, int i, bool locked) {
    if (c) {
        if (!locked) {
            aSsErT_mutex.lock();
            locked = true;
        }
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
    if (locked) {
        aSsErT_mutex.unlock();
    }
}

// ============================================================================
//                      HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

enum { TEST_ITERATIONS = 3 };

struct ConcurrencyTest {
   my_Mutex    *d_mutex;
   my_Barrier  *d_barrier;
   AtomicOpInt  d_iteration;
   AtomicOpInt  d_entered;
   BslOnce      d_onces[TEST_ITERATIONS];


   ConcurrencyTest(my_Mutex *mutex, my_Barrier *barrier)
       // Initialize this test data with the specified 'mutex' and specified
       // 'barrier'.
   : d_mutex(mutex)
   , d_barrier(barrier)
   {
       AtomicOp::setInt(&d_iteration, 0);
       AtomicOp::setInt(&d_entered, 0);
       for (int i = 0; i < TEST_ITERATIONS; ++i) {
           AtomicOp::setInt(&d_onces[i].d_onceState, 0xdead);
       }
   }

};

static void* concurrencyTest(void* args)
    // Perform a concurrency test using the specified 'args'.  The behavior is
    // undefined unless 'args' points to a 'ConcurrencyTest' object.
{

    // Plan:
    //: 1 Iterate through a sequence of 'BslOnce' objects, using a barrier to
    //:   ensure the test threads are iterating through the array in
    //:   lock-stop.  On each iteration:
    //:
    //:   1 Call 'enter' on a 'BslOnce' object.
    //:
    //:   2 If the 'BslOnce' is successfully entered, set 'entered' to 'true'
    //:     and verify that 'entered' was previously set to 'false'.
    //:
    //:   3 If the 'BslOnce' is not successfully entered, verify that
    //:     'entered' had previously been set to 'true'.

    ConcurrencyTest *data      = static_cast<ConcurrencyTest *>(args);
    my_Barrier      *barrier   = data->d_barrier;
    AtomicOpInt     *iteration = &data->d_iteration;
    AtomicOpInt     *entered   = &data->d_entered;
    BslOnce         *onces     = data->d_onces;

    for (int i = 0; i < TEST_ITERATIONS; ++i) {

        // This barrier is required to reset 'entered' prior to beginning the
        // test (so the previous iteration does not see the updated value).

        barrier->wait();

        // Sanity check on the 'barrier'.
        int testIteration = AtomicOp::testAndSwapInt(iteration, i, i + 1);
        ASSERTV(i == testIteration || i == testIteration - 1);

        // Set the 'entered' flag to false.
        AtomicOp::setInt(entered, 0);

        barrier->wait();
        {
            if (onces[i].enter()) {
                int wasEntered = AtomicOp::swapInt(entered, true);
                ASSERT(0 == wasEntered);
                mySleep(3);
                onces[i].leave();
            }
            else {
                int wasEntered = AtomicOp::getIntRelaxed(entered);
                ASSERT(1 == wasEntered);
            }
            ASSERT(false == onces[i].enter());
        }
    }
    return 0;
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Example 1: Using 'bsls::BslOnce' to Perform a Singleton Initialization
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates using 'bsls::BslOnce' to initialize a
// singleton object.
//
// First we declare a 'struct', 'MySingleton', whose definition is elided:
//..
    struct MySingleton {
//
      // PUBLIC DATA
      int d_exampleData;
//
      // ...
    };
//..
// Notice that the data members are public because we want to avoid dynamic
// runtime initialize (i.e., initialization at run-time before the start of
// 'main') when an object of this type is declared in a static context.
//
// Now we implement a function 'getSingleton' that returns a singleton object.
// 'getSingleton' uses 'BslOnce' to ensure the singleton is initialized only
// once, and that the singleton is initialized before the function returns:
//..
    MySingleton *getSingleton()
        // Return a reference to a modifiable singleton object.
    {
       static MySingleton singleton = { 0 };
       static BslOnce     once      = BSLS_BSLONCE_INITIALIZER;
//
       BslOnceGuard onceGuard;
       if (onceGuard.enter(&once)) {
         // Initialize 'singleton'.  Note that this code is executed exactly
         // once.
//
       }
       return &singleton;
    }
//..
// Notice that 'BslOnce' must be initialized to 'BSLS_BSLONCE_INITIALIZER', and
// that 'singleton' is a function scoped static variable to avoid allocating
// it on the 'heap' (which might be reported as leaked memory).

// ============================================================================
//                          MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{

    int            test = argc > 1 ? atoi(argv[1]) : 0;
    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4; (void) veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        MySingleton *dummy = getSingleton();
        ASSERT(0 != dummy);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CONCERN: CONCURRENT CALLS TO 'enter'
        //   Test the concurrency of 'BslOnceGuard'.
        //
        // Concerns:
        //: 1 'enter' will only return 'true' once when called concurrently
        //:   from multiple threads.
        //:
        //: 2 'enter' will not return until the thread that first entered the
        //:   one-time execution block has called 'leave'.
        //
        // Plan:
        //: 1 Iterate through a sequence of 'BslOnce' objects, using a barrier
        //:   to ensure the test threads are iterating through the array in
        //:   lock-stop.  On each iteration:
        //:
        //:   1 Call 'enter' on a 'BslOnce' object.
        //:
        //:   2 If the 'BslOnce' is successfully entered, set 'entered' to
        //:     'true' and verify that 'entered' was previously set to
        //:     'false'.
        //:
        //:   3 If the 'BslOnce' is not successfully entered, verify that
        //:     'entered' had previously been set to 'true'.
        //
        // Testing:
        //   CONCERN: CONCURRENT CALLS TO 'enter'
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONCERN: CONCURRENT CALLS TO 'enter'"
                            "\n====================================\n");

        {
            enum { NUM_THREADS = 10 };

            my_Mutex    mutex;
            my_Barrier  barrier(NUM_THREADS);
            my_thread_t handles[NUM_THREADS];

            ConcurrencyTest args(&mutex, &barrier);

            // Execute the the first test.
            for (int i = 0; i < NUM_THREADS; ++i) {
                myCreateThread(&handles[i], concurrencyTest, &args);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                myJoinThread(handles[i]);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING: PRIMARY MANIPULATORS ('BslOnceGuard')
        //   Test the operations on 'BslOnceGuard'.
        //
        // Concerns:
        //: 1 'enter' will only return 'true' on the first invocation.
        //:
        //: 3 'BslOnceGuard' will leave the supplied 'BslOnce' on its
        //:    destruction.
        //:
        //: 2 'leave' can be called after 'enter'
        //:
        //: 4 Destroying a 'BslOnceGuard' after 'leave' has no effect.
        //:
        //: 5 'enter' returns 'false', and leaves the guard unmodified, if
        //:   the 'BslOnce' has already been 'enter'ed.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Call 'enter' on a newly constructed object and verify it returns
        //:   'true' (C-1)
        //:
        //: 2 Call 'enter', then 'leave' on a newly constructed object and
        //:   verify subsequent calls to 'enter' return 'false' (C-2)
        //:
        //: 3 Call 'enter' on an object that has been incorrectly initialized
        //:   and verify that it throws in an appropriate build mode.  (C-3)
        //
        // Testing:
        //   BslOnceGuard();
        //   ~BslOnceGuard();
        //   bool enter(BslOnce *);
        //   void leave();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: PRIMARY MANIPULATORS ('BslOnceGuard')"
                            "\n=============================================="
                            "\n");

        if (veryVerbose) printf("\nConstructing and destructing a guard.\n");
        {
            {
                BslOnceGuard guard;
            }
        }

        if (veryVerbose) printf("\nCalling 'enter' with a guard\n");
        {
            Obj x = BSLS_BSLONCE_INITIALIZER;
            {
                BslOnceGuard guard;
                ASSERT(true == guard.enter(&x));
            }
            {
                BslOnceGuard guard;
                ASSERT(false == guard.enter(&x));
                ASSERT(false == guard.enter(&x));
            }
        }

        if (veryVerbose) printf("\nCalling 'leave' with a guard\n");
        {
            Obj x = BSLS_BSLONCE_INITIALIZER;
            {
                BslOnceGuard guard;
                ASSERT(true == guard.enter(&x));
                guard.leave();
                ASSERT(false == guard.enter(&x));
                ASSERT(false == guard.enter(&x));
            }
            {
                BslOnceGuard guard;
                ASSERT(false == guard.enter(&x));
                ASSERT(false == guard.enter(&x));
            }
        }


        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            {
                Obj x = BSLS_BSLONCE_INITIALIZER;

                BslOnceGuard guard;
                ASSERT_SAFE_FAIL(guard.enter(0));
                ASSERT_SAFE_PASS(guard.enter(&x));
                ASSERT_SAFE_FAIL(guard.enter(&x));
            }

            {
                BslOnceGuard guard;
                ASSERT_SAFE_FAIL(guard.leave());
            }
            {
                Obj x = BSLS_BSLONCE_INITIALIZER;

                BslOnceGuard guard;
                ASSERT(true == guard.enter(&x));
                ASSERT_SAFE_PASS(guard.leave());
                ASSERT_SAFE_FAIL(guard.leave());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING: PRIMARY MANIPULATORS ('BslOnce')
        //   Test the operations on 'BslOnce'.  Note that one key concern not
        //   tested is that if 'enter' returns 'true', other threads calling
        //   'enter' will block until the original thread calls 'leave' (that
        //   is tested in 'CONCERN: CONCURRENT CALLS TO 'enter').
        //
        // Concerns:
        //: 1 'enter' will only return 'true' on the first invocation.
        //:
        //: 2 'leave' can be called after 'enter'
        //:
        //: 3 Calls to 'enter' after a call to 'leave' return 'false'.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Call 'enter' on a newly constructed object and verify it returns
        //:   'true' (C-1)
        //:
        //: 2 Call 'enter', then 'leave' on a newly constructed object and
        //:   verify subsequent calls to 'enter' return 'false' (C-2)
        //:
        //: 3 Call 'enter' on an object that has been incorrectly initialized
        //:   and verify that it throws in an appropriate build mode.  (C-3)
        //
        // Testing:
        //   bool enter();
        //   void leave();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING: PRIMARY MANIPULATORS ('BslOnce')"
                            "\n=========================================\n");

        if (veryVerbose) printf("\nCalling 'enter' the first time\n");
        {
            Obj x = BSLS_BSLONCE_INITIALIZER;

            ASSERT(true == x.enter());
        }

        if (veryVerbose) printf("\nCalling 'enter' after 'leave'\n");
        {
            Obj x = BSLS_BSLONCE_INITIALIZER;

            ASSERT(true == x.enter());
            x.leave();
            ASSERT(false == x.enter());
            ASSERT(false == x.enter());
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj x = { 0 };
            ASSERT_FAIL(x.enter());

        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Use 'BslOnce' in some simple contexts and and verify it allows
        //:   'enter' to be called once successfully.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (veryVerbose) printf("\tTest that a block can be entered once\n");
        {
            Obj x = BSLS_BSLONCE_INITIALIZER;

            ASSERT(true  == x.enter());
            x.leave();
            ASSERT(false == x.enter());
            ASSERT(false == x.enter());
        }

        if (veryVerbose) printf("\tTest guard without entering\n");
        {
            Guard guard;
        }

        if (veryVerbose) printf("\tTest guard w/ entering\n");
        {
            Obj x = BSLS_BSLONCE_INITIALIZER;
            {
                Guard guard;
                ASSERT(true  == guard.enter(&x));
            }
            ASSERT(false == x.enter());
            {
                Guard guard;
                ASSERT(false == guard.enter(&x));
            }
        }

        if (veryVerbose) printf("\tTest guard w/ leaving\n");
        {
            Obj x = BSLS_BSLONCE_INITIALIZER;
            {
                Guard guard;
                ASSERT(true  == guard.enter(&x));

                guard.leave();
                ASSERT(false == x.enter());
            }
            ASSERT(false == x.enter());
            {
                Guard guard;
                ASSERT(false == guard.enter(&x));
            }

        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
   return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
