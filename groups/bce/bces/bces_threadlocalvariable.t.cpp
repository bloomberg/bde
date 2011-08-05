// bces_threadlocalvariable.t.cpp  -*-C++-*-
#include <bces_threadlocalvariable.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_ostream.h>
#include <bsl_iostream.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The 'bces_threadlocalvariable' component defines a macro
// 'BCES_THREAD_LOCAL_VARIABLE' that declares a thread local variable.
// The tests verify that the variable is static (i.e., it has the same address
// on every invocation on the same thread), that it is thread-local (i.e., it
// has a different address for very thread), that it is initialized correctly
// with the supplied 'INITIAL_VALUE', and that it can be created for any
// pointer type.
//-----------------------------------------------------------------------------
// [ 5] BCES_DECLARE_THREAD_LOCAL_VARIABLE: data types test
// [ 4] BCES_DECLARE_THREAD_LOCAL_VARIABLE: initial value test
// [ 3] BCES_DECLARE_THREAD_LOCAL_VARIABLE: global scope test
// [ 2] BCES_DECLARE_THREAD_LOCAL_VARIABLE: function scope static test
//-----------------------------------------------------------------------------
// [ 1] Helper Test: 'my_Barrier'
// [ 6] Usage examples
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;


void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"
#define T_() cout << '\t' << flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                      THREAD CLASSES FOR TESTING
//-----------------------------------------------------------------------------

extern "C" {

typedef void* (*THREAD_ENTRY)(void *arg);

}

// Implementation Note: these classes were copied from 'bces_atomicutil.t.cpp'

#ifdef BSLS_PLATFORM__OS_WINDOWS
#include <windows.h>
typedef HANDLE my_thread_t;
#else
#include <pthread.h>
#include <bsl_c_sys_time.h>
typedef pthread_t my_thread_t;
#endif

class my_Mutex {
    // This class implements a cross-platform mutual exclusion primitive
    // similar to posix mutexes.
#ifdef BSLS_PLATFORM__OS_WINDOWS
    HANDLE d_mutex;
#else
    pthread_mutex_t d_mutex;
#endif

  public:
    my_Mutex();
        // Construct an 'my_Mutex' object.
    ~my_Mutex();
        // Destroy an 'my_Mutex' object.

    void lock();
        // Lock this mutex.

    void unlock();
        // Unlock this mutex;
};

class my_Conditional {
    // This class implements a cross-platform waitable state indicator used for
    // testing.  It has two states, signaled and non-signaled.  Once
    // signaled('signal'), the state will persist until explicitly 'reset'.
    // Calls to wait when the state is signaled, will succeed immediately.
#ifdef BSLS_PLATFORM__OS_WINDOWS
    HANDLE d_cond;
#else
    pthread_mutex_t d_mutex;
    pthread_cond_t  d_cond;
    volatile int   d_signaled;
#endif

  public:
    my_Conditional();
    ~my_Conditional();

    void reset();
        // Reset the state of this indicator to non-signaled.

    void signal();
        // Signal the state of the indicator and unblock any thread waiting
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

inline
my_Mutex::my_Mutex()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    d_mutex = CreateMutex(0,FALSE,0);
#else
    pthread_mutex_init(&d_mutex,0);
#endif
}

inline
my_Mutex::~my_Mutex()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    CloseHandle(d_mutex);
#else
    pthread_mutex_destroy(&d_mutex);
#endif
}

inline
void my_Mutex::lock()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    WaitForSingleObject(d_mutex, INFINITE);
#else
    pthread_mutex_lock(&d_mutex);
#endif
}

inline
void my_Mutex::unlock()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    ReleaseMutex(d_mutex);
#else
    pthread_mutex_unlock(&d_mutex);
#endif
}


my_Conditional::my_Conditional()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    d_cond = CreateEvent(0,TRUE,FALSE,0);
#else
    pthread_mutex_init(&d_mutex,0);
    pthread_cond_init(&d_cond,0);
    d_signaled = 0;
#endif
}

my_Conditional::~my_Conditional()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    CloseHandle(d_cond);
#else
    pthread_cond_destroy(&d_cond);
    pthread_mutex_destroy(&d_mutex);
#endif
}

void my_Conditional::reset()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    ResetEvent(d_cond);
#else
    pthread_mutex_lock(&d_mutex);
    d_signaled = 0;
    pthread_mutex_unlock(&d_mutex);
#endif
}

void my_Conditional::signal()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    SetEvent(d_cond);
#else
    pthread_mutex_lock(&d_mutex);
    d_signaled = 1;
    pthread_cond_broadcast(&d_cond);
    pthread_mutex_unlock(&d_mutex);
#endif
}


void my_Conditional::wait()
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    WaitForSingleObject(d_cond,INFINITE);
#else
    pthread_mutex_lock(&d_mutex);
    while (!d_signaled) pthread_cond_wait(&d_cond,&d_mutex);
    pthread_mutex_unlock(&d_mutex);
#endif
}

int my_Conditional::timedWait(int timeout)
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
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


static int myCreateThread( my_thread_t *aHandle, THREAD_ENTRY aEntry,
                           void *arg )
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    *aHandle = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)aEntry,arg,0,0);
    return *aHandle ? 0 : -1;
#else
    return pthread_create(aHandle, 0, aEntry, arg);
#endif
}

static void  myJoinThread(my_thread_t aHandle)
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    WaitForSingleObject(aHandle,INFINITE);
    CloseHandle(aHandle);
#else
    pthread_join(aHandle,0);
#endif
}

// --------------------------- Additional thread classes ----------------------

template <typename T>
class LockGuard {
    // A scoped guard for calling 'lock' and 'unlock' on an object
    // of parameterized type 'T' (presumably a mutex).

    // DATA
    T *d_lock;

  public:
    // CREATORS
    LockGuard(T *lock) : d_lock(lock) { d_lock->lock(); }
        // Construct a lock guard for the specified 'lock', and call 'lock()'
        // on 'lock'.

    ~LockGuard() { d_lock->unlock(); }
        // Destroy this lock guard and call 'unlock()' on the object supplied
        // at construction.
};

class my_Barrier {
    // This class defines a barrier for synchronizing multiple threads.  A
    // barrier is initialized with the number of threads to expect.  Each
    // invocation of 'wait()' will block the active thread until the expected
    // number of threads have entered the barrier, at which point all threads
    // are released and return from 'wait()'.
    //
    // *NOTE*: The 'my_Conditional' operation 'wait' does *not* take a mutex,
    // so it cannot be atomically combined with another operation
    // (e.g., incrementing the 'd_waiting' counter), it also does not provide
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
    my_Barrier(int numThreads);
        // Create a barrier that will synchronize the specified
        // 'numThreads' number of threads.  The behavior is undefined
        // unless '0 < expectedThreads'.

    ~my_Barrier();
        // Destroy this barrier.

    // MANIPULATORS
    void wait();
        // Block the current thread until the number of expectedThreads,
        // supplied at construction, have entered this barrier's 'wait()'
        // method, then return (all blocked threads, including the current
        // thread).
};

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
    // so it cannot be atomically combined with another operation
    // (e.g., incrementing the 'd_waiting' counter), it also does not provide
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
//                              TEST GUARD
//-----------------------------------------------------------------------------

// Verify that the macro is defined on supported platforms.
#if defined(BSLS_PLATFORM__CMP_SUN)  ||                                       \
    (defined(BSLS_PLATFORM__CMP_GNU) && !(defined(BSLS_PLATFORM__CPU_SPARC)))
#ifndef BCES_THREAD_LOCAL_VARIABLE
#error "'BCES_THREAD_LOCAL_VARIABLE' macro undefined for a supported platform"
#endif
#else  // unsupported platform
#ifdef BCES_THREAD_LOCAL_VARIABLE
#error "'BCES_THREAD_LOCAL_VARIABLE' macro defined for an unsupported platform"
#endif
#endif

// If the macro is not define (i.e., this is an unsupported platform) disable
// all the tests.
#ifndef BCES_THREAD_LOCAL_VARIABLE
#define DISABLE_TEST
#endif

//=============================================================================
//                              TEST CLASSES
//-----------------------------------------------------------------------------


#ifndef DISABLE_TEST

// ---------------------------  my_Barrier Test  ------------------------------

struct BarrierTestArgs {
    my_Barrier *d_barrier;
    my_Mutex    d_mutex;
    int         d_numThreads;
    int         d_count;

};

extern "C" void *barrierTest(void *voidArgs)
{
    BarrierTestArgs *args = (BarrierTestArgs *)voidArgs;

    my_Barrier *barrier = args->d_barrier;

    args->d_mutex.lock();
    args->d_count = 0;
    args->d_mutex.unlock();
    ASSERT(0 == args->d_count);

    barrier->wait();

    args->d_mutex.lock();
    ++args->d_count;
    args->d_mutex.unlock();

    barrier->wait();

    // The barrier should ensure the count is constant.
    ASSERT(args->d_numThreads == args->d_count);

    barrier->wait();

    args->d_mutex.lock();
    --args->d_count;
    args->d_mutex.unlock();

    barrier->wait();

    // The barrier should ensure the count is constant.
    ASSERT(0 == args->d_count);
    if (0 != args->d_count) {
        P(args->d_count);
    }

    barrier->wait();
    return voidArgs;
}


// --------------------  FUNCTION SCOPED ADDRESS TEST  ------------------------

struct ReturnAddressThreadArgs {
    int         d_threadId;
    my_Barrier *d_barrier;
    void       *d_address;
};

void functionScopeAddressTestDelegate(int    threadId,
                                      void **address,
                                      void  *expectedValue)
{
    BCES_THREAD_LOCAL_VARIABLE(void *, variable, 0);
    ASSERT(variable == expectedValue);

    *address = &variable;

    variable = (void *)threadId;
}

extern "C" void *functionScopeAddressTest(void *voidArgs)
{
    ReturnAddressThreadArgs *args = (ReturnAddressThreadArgs *)voidArgs;
    args->d_barrier->wait();

    int threadId = args->d_threadId;

    functionScopeAddressTestDelegate(threadId, &args->d_address, 0);

    args->d_barrier->wait();

    void *firstAddress = args->d_address;

    functionScopeAddressTestDelegate(threadId,
                                     &args->d_address,
                                     (void *)threadId);

    args->d_barrier->wait();

    ASSERT(firstAddress == args->d_address);

    return voidArgs;
}

// --------------------  GLOBAL SCOPED ADDRESS TEST  --------------------------


BCES_THREAD_LOCAL_VARIABLE(void *, g_testVariable, 0);

extern "C" void *globalScopeAddressTest(void *voidArgs)
{
    ReturnAddressThreadArgs *args = (ReturnAddressThreadArgs *)voidArgs;

    args->d_barrier->wait();

    ASSERT(0 == g_testVariable);

    args->d_barrier->wait();

    g_testVariable = (void *)args->d_threadId;

    args->d_barrier->wait();

    ASSERT((void *)args->d_threadId == g_testVariable)

    args->d_barrier->wait();

    args->d_address = &g_testVariable;
    return voidArgs;
}


// -----------------------  INITIAL VALUE TEST  -------------------------------


struct InitialValueThreadArgs {
    int         d_threadId;
    my_Barrier *d_barrier;
};

extern "C" void *initialValueTest(void *voidArgs)
{
    InitialValueThreadArgs *args = (InitialValueThreadArgs *)voidArgs;

    args->d_barrier->wait();

    BCES_THREAD_LOCAL_VARIABLE(void *, nullAddress, 0);
    BCES_THREAD_LOCAL_VARIABLE(void *, tenAddress,  (void *)10);
    BCES_THREAD_LOCAL_VARIABLE(void *, deadAddress, (void *)0xdead);

    ASSERT(0              == nullAddress);
    ASSERT((void *)10     == tenAddress);
    ASSERT((void *)0xdead == deadAddress);

    return 0;
}


// ---------------------------  TYPE TEST  ------------------------------------

struct TypesThreadArgs {
    int         d_threadId;
    my_Barrier *d_barrier;
};


#define VERIFY_BASIC_TYPE(TYPE, ID) {                                         \
    BCES_THREAD_LOCAL_VARIABLE(TYPE, testValue, (TYPE)0xaa);                  \
    ASSERT((TYPE)0xaa == testValue);                                          \
    TYPE value = testValue;                                                   \
    ASSERT(value == (TYPE)0xaa);                                              \
    testValue = (TYPE)(ID);                                                   \
    ASSERT((TYPE)(ID) == testValue);                                          \
}

#define VERIFY_STRUCT_TYPE(TYPE, VALUE) {                                     \
    BCES_THREAD_LOCAL_VARIABLE(TYPE, testValue, VALUE);                       \
    ASSERT(VALUE == testValue);                                               \
}

extern "C" void *typesTest(void *voidArgs)
{
    InitialValueThreadArgs *args = (InitialValueThreadArgs *)voidArgs;

    args->d_barrier->wait();

    VERIFY_BASIC_TYPE(int, args->d_threadId);
    VERIFY_BASIC_TYPE(char, args->d_threadId);
    VERIFY_BASIC_TYPE(double *, args->d_threadId);
    VERIFY_BASIC_TYPE(unsigned int, args->d_threadId);
    VERIFY_BASIC_TYPE(unsigned char, args->d_threadId);
    VERIFY_BASIC_TYPE(long long, args->d_threadId);
    VERIFY_BASIC_TYPE(unsigned long long, args->d_threadId);
    VERIFY_BASIC_TYPE(unsigned long long, args->d_threadId);

    VERIFY_BASIC_TYPE(void *, args->d_threadId);
    VERIFY_BASIC_TYPE(int *, args->d_threadId);
    VERIFY_BASIC_TYPE(char *, args->d_threadId);
    VERIFY_BASIC_TYPE(double *, args->d_threadId);
    VERIFY_BASIC_TYPE(bool *, args->d_threadId);
    VERIFY_BASIC_TYPE(my_Barrier *, args->d_threadId);
    VERIFY_BASIC_TYPE(const void *, args->d_threadId);
    VERIFY_BASIC_TYPE(const int *, args->d_threadId);
    VERIFY_BASIC_TYPE(const char *, args->d_threadId);
    VERIFY_BASIC_TYPE(const double *, args->d_threadId);
    VERIFY_BASIC_TYPE(const bool *, args->d_threadId);
    VERIFY_BASIC_TYPE(const my_Barrier *, args->d_threadId);

    VERIFY_BASIC_TYPE(void **, args->d_threadId);
    VERIFY_BASIC_TYPE(int **, args->d_threadId);

    return 0;
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended usage of this component. 
//
///Example 1: A Service Request Processor with Thread Local Context
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// In the following example we create a 'RequestProcessor' that places context
// information for the current request in a thread-local variable.
//
// First, we define a trivial structure for a request context.
//..
    // requestprocessor.h
//
    struct RequestContext {
//
        // DATA
        int d_userId;       // BB user id
        int d_workstation;  // BB LUW
    };
//..
// Next, we create a trivial 'RequestProcessor' that provides a 'static' class
// method that returns the 'RequestContext' for the current thread, or 0 if
// the current thread is not processing a request.
//..
    class RequestProcessor {
        // This class implements an "example" request processor.
//
        // NOT IMPLEMENTED
        RequestProcessor(const RequestProcessor& );
        RequestProcessor& operator=(const RequestProcessor& );
//
        // PRIVATE CLASS METHODS
        static const RequestContext *&contextReference();
            // Return a reference to a *modifiable* thread-local pointer to the
            // non-modifiable request context for this thread.  Note that this
            // method explicitly allows the pointer (but not the
            // 'RequestContext' object) to be modified by the caller to allow
            // other methods to assign the thread-local context pointer to a
            // new address.
//
      public:
//
        // CLASS METHODS
        static const RequestContext *requestContext();
            // Return the address of the non-modifiable, request context
            // for this thread, or 0 if none has been set.
//
        // CREATORS
        RequestProcessor() {}
            // Create a 'RequestProcessor'.
//
        ~RequestProcessor() {}
            // Destroy this request processor.
//
        // MANIPULATORS
        void processRequest(int         userId,
                            int         workstation,
                            const char *request);
            // Process (in the caller's thread) the specified 'request' for
            // the specified 'userId' and 'workstation'.
    };
//
    // requestprocessor.cpp
//
    // PRIVATE CLASS METHODS
//..
// Now, we define the 'contextReference' method, which defines a thread-local
// 'RequestContext' pointer, 'context', initialized to 0, and returns a
// reference providing modifiable access to that pointer.
//..
    const RequestContext *&RequestProcessor::contextReference()
    {
        BCES_THREAD_LOCAL_VARIABLE(const RequestContext *, context, 0);
        return context;
    }
//
    // CLASS METHODS
    const RequestContext *RequestProcessor::requestContext()
    {
        return contextReference();
    }
//
    // MANIPULATORS
//..
// Then, we define the 'processRequest' method, which first sets the
// thread-local pointer containing the request context, and then processes the
// 'request'.
//..
    void RequestProcessor::processRequest(int         userId,
                                          int         workstation,
                                          const char *request)
    {
        RequestContext currentContext = {userId, workstation};
//
        contextReference() = &currentContext;
//
        // Process the request.
//
        contextReference() = 0;
    }
//..
// Finally, we define a separate function 'myFunction' that uses the
// 'RequestProcessor' class to access the 'RequestContext' for the current
// thread.
//..
    void myFunction()
    {
        const RequestContext *context = RequestProcessor::requestContext();
//
        // Perform some task that makes use of this threads 'requestContext'.
        // ...
    }
//..

struct UsageTestArgs {
    my_Barrier       *d_barrier;
    RequestProcessor *d_processor;
};


extern "C" void *usageTest(void *voidArgs)
{
    UsageTestArgs *args = (UsageTestArgs *)voidArgs;

    args->d_barrier->wait();
    args->d_processor->processRequest(1, 2, "usageExample");
    return 0;
}

#endif // #ifndef DISABLE_TEST

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    bslma_TestAllocator allocator; bslma_TestAllocator *Z = &allocator;
    bslma_TestAllocator defaultAllocator;
    bslma_TestAllocator globalAllocator;
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
#ifdef DISABLE_TEST
// If testing is disable, we still require a fake test case.
      case 1: {} break;
#else // #ifndef DISABLE_TEST
      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        RequestProcessor processor;
        const int NUM_THREADS = 5;
        my_thread_t   handles[NUM_THREADS];
        my_Barrier    barrier(NUM_THREADS);
        UsageTestArgs args = { &barrier, &processor };

        // Execute the the first test.
        for (int i = 0; i < NUM_THREADS; ++i) {
            myCreateThread(&handles[i], usageTest, &args);
        }
        for (int i = 0; i < NUM_THREADS; ++i) {
            myJoinThread(handles[i]);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TEST TYPES
        //
        // Concerns:
        //    That 'BCES_THREAD_LOCAL_VARIABLE' creates variables of the
        //    correct type.
        //
        // Plan:
        //    Declares a thread local pointers of different types, assign the
        //    pointer to another variable of that type, then verify they have
        //    the same value.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DATA TYPES" << endl
                          << "==========" << endl;

        const int NUM_THREADS = 5;
        my_Barrier             barrier(NUM_THREADS);
        my_thread_t            handles[NUM_THREADS];
        InitialValueThreadArgs args[NUM_THREADS];

        // Execute the the first test.
        for (int i = 0; i < NUM_THREADS; ++i) {
            args[i].d_threadId = i;
            args[i].d_barrier  = &barrier;
            myCreateThread(&handles[i], typesTest, &args[i]);
        }
        for (int i = 0; i < NUM_THREADS; ++i) {
            myJoinThread(handles[i]);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TEST INITIAL VALUE
        //
        // Concerns:
        //    That a variable declared with
        //    'BCES_THREAD_LOCAL_VARIABLE'
        //    is initialized with the specified 'INITIAL_VALUE'
        //
        // Plan:
        //    Declares a thread local pointers with different initial values
        //    and verify that they are initialized correctly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Initial Value" << endl
                          << "=============" << endl;

        const int NUM_THREADS = 5;
        my_Barrier             barrier(NUM_THREADS);
        my_thread_t            handles[NUM_THREADS];
        InitialValueThreadArgs args[NUM_THREADS];

        // Execute the the first test.
        for (int i = 0; i < NUM_THREADS; ++i) {
            args[i].d_threadId = i;
            args[i].d_barrier  = &barrier;
            myCreateThread(&handles[i], initialValueTest, &args[i]);
        }
        for (int i = 0; i < NUM_THREADS; ++i) {
            myJoinThread(handles[i]);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST GLOBAL VARIABLE ADDRESS
        //
        // Concerns:
        //    That the 'BCES_THREAD_LOCAL_VARIABLE' macro declares a
        //    static global address that is different for each thread.
        //
        // Plan:
        //    Declares a thread local variable at global scope.  Create
        //    multiple threads and verify that the defined variable has a
        //    different address (and value) for each thread.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Global Scope Variable Address Test" << endl
                          << "==================================" << endl;

        const int NUM_THREADS = 5;
        my_Barrier              barrier(NUM_THREADS);
        my_thread_t             handles[NUM_THREADS];
        ReturnAddressThreadArgs args[NUM_THREADS];

        // Execute the the first test.
        for (int i = 0; i < NUM_THREADS; ++i) {
            args[i].d_threadId = i;
            args[i].d_address  = 0;
            args[i].d_barrier  = &barrier;
            myCreateThread(&handles[i], globalScopeAddressTest, &args[i]);
        }
        for (int i = 0; i < NUM_THREADS; ++i) {
            myJoinThread(handles[i]);
        }

        // Use the set to verify there are 'NUM_THREADS' unique addresses.
        bsl::set<void *> addresses(Z);
        for (int i = 0; i < NUM_THREADS; ++i) {
            ASSERT(0 != args[i].d_address);
            if (veryVerbose) {
                P_(i);  P(args[i].d_address);
            }
            addresses.insert(args[i].d_address);
        }
        ASSERT(NUM_THREADS == addresses.size());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST FUNCTION SCOPE VARIABLE ADDRESS
        //
        // Concerns:
        //    That the 'BCES_THREAD_LOCAL_VARIABLE' macro declares a
        //    static function scoped address that is different for each thread.
        //
        // Plan:
        //    Create a function that declares a thread local variable.  Invoke
        //    it on multiple threads and verify that the defined variable has a
        //    different value for each thread.  Within each thread, verify
        //    that the address (and value) of the variable is the same on each
        //    invocation.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Function Scope Variable Address Test" << endl
                          << "====================================" << endl;

        const int NUM_THREADS = 5;
        my_Barrier              barrier(NUM_THREADS);
        my_thread_t             handles[NUM_THREADS];
        ReturnAddressThreadArgs args[NUM_THREADS];

        // Execute the the first test.
        for (int i = 0; i < NUM_THREADS; ++i) {
            args[i].d_threadId = i;
            args[i].d_address  = 0;
            args[i].d_barrier  = &barrier;
            myCreateThread(&handles[i], functionScopeAddressTest, &args[i]);
        }
        for (int i = 0; i < NUM_THREADS; ++i) {
            myJoinThread(handles[i]);
        }

        // Use the set to verify there are 'NUM_THREADS' unique addresses.
        bsl::set<void *> addresses(Z);
        for (int i = 0; i < NUM_THREADS; ++i) {
            ASSERT(0 != args[i].d_address);
            if (veryVerbose) {
                P_(i);  P(args[i].d_address);
            }
            addresses.insert(args[i].d_address);
        }
        ASSERT(NUM_THREADS == addresses.size());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING HELPER CLASS: my_Barrier
        //
        // Concerns: That 'my_Barrier' waits for the correct number of threads
        // to enter, that it releases all threads, and can be used multiple
        // times with no side effects.
        //
        // Plan:
        //
        // Testing:
        //   my_Barrier(int );
        //   ~my_Barrier();
        //   wait();
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Helper: 'my_Barrier'"
                               << "\n============================"
                               << bsl::endl;


        for (int numThreads = 1; numThreads < 9; ++numThreads) {

            bsl::vector<my_thread_t> handles(Z);
            handles.resize(numThreads);
            my_Barrier barrier(numThreads);
            BarrierTestArgs args;
            args.d_numThreads = numThreads;
            args.d_count      = 0;
            args.d_barrier    = &barrier;

            for (int i = 0; i < numThreads; ++i) {
                myCreateThread(&handles[i], barrierTest, &args);
            }
            for (int i = 0; i < numThreads; ++i) {
                myJoinThread(handles[i]);
            }
        }
      } break;
#endif // #ifndef DISABLE_TEST
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }
    ASSERT(0 == defaultAllocator.numAllocations());
    ASSERT(0 == globalAllocator.numAllocations());

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
