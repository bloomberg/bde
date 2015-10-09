// bsls_atomicoperations.t.cpp                                        -*-C++-*-

#include <bsls_atomicoperations.h>
#include <bsls_types.h>

#include <stdlib.h> // atoi
#include <iostream>

#if defined(BSLS_PLATFORM_OS_SOLARIS)
#include <thread.h>  // thr_setconcurrency
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
typedef HANDLE my_thread_t;
#else
#include <pthread.h>
#include <sys/time.h>
typedef pthread_t my_thread_t;
#endif

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//
//
//-----------------------------------------------------------------------------
//
// [2 ] initInt(Int *aInt);
// [2 ] initInt64(Int64 *aInt);
// [2 ] initPointer(Pointer *aPointer);
// [3 ] addInt(Int *aInt, int value);
// [5 ] incrementInt(Int *aInt);
// [5 ] decrementInt(Int *aInt);
// [4 ] swapInt64(Int *aInt, int value);
// [4 ] testAndSwapInt(Int *, int, int);
// [2 ] setInt(Int *aInt, int value);
// [2 ] getInt(const Int &aInt);
// [3 ] addIntNv(Int *aInt, int val);
// [5 ] incrementIntNv(Int *aInt);
// [5 ] decrementIntNv(Int *aInt);
// [3 ] addInt64(Int64 *, bsls::Types::Int64);
// [5 ] incrementInt64(Int64 *aInt);
// [5 ] decrementInt64(Int64 *aInt);
// [4 ] swapInt64(Int64 *, bsls::Types::Int64);
// [4 ] testAndSwapInt64(Int64 *, bsls::Types::Int64,
//                       bsls::Types::Int64);
// [3 ] addInt64Nv(Int64 *, bsls::Types::Int64);
// [5 ] incrementInt64Nv(Int64 *);
// [5 ] decrementInt64Nv(Int64 *);
// [2 ] setInt64(Int64 *, bsls::Types::Int64);
// [2 ] getInt64(const Int64 &aInt);
// [2 ] getPtr(const Pointer &aPointer);
// [2 ] setPtr(Pointer *aPointer, void *value);
// [4 ] swapPtr(Pointer *aPointer, void *value);
// [4 ] testAndSwapPtr(Pointer *, void *, void *);
//-----------------------------------------------------------------------------
// [1 ] Breathing test
// [7 ] Usage examples
//-----------------------------------------------------------------------------
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { \
        aSsErT_mutex.lock(); \
        cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__, true);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { \
        aSsErT_mutex.lock(); \
        cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__, true); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) {  \
        aSsErT_mutex.lock(); \
        cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__, true); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define NL()  cout << endl;                   // Print newline
#define P64(X) printf(#X " = %lld\n", (X));   // Print 64-bit integer id & val
#define P64_(X) printf(#X " = %lld,  ", (X)); // Print 64-bit integer w/o '\n'

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls::AtomicOperations              Obj;
typedef bsls::AtomicOperations::AtomicTypes Types;

const int INT_SWAPTEST_VALUE1 = 0x33ff33ff;
const int INT_SWAPTEST_VALUE2 = 0xff33ff33;

const bsls::Types::Int64 INT64_SWAPTEST_VALUE1 = 0x33ff33ff33ff33ffLL;
const bsls::Types::Int64 INT64_SWAPTEST_VALUE2 = 0xff33ff33ff33ff33LL;

void* POINTER_SWAPTEST_VALUE1 = (void*)0x33ff33ff;
void* POINTER_SWAPTEST_VALUE2 = (void*)0xff33ff33;

extern "C" {
typedef void* (*THREAD_ENTRY)(void *arg);
}


const bsls::Types::Int64 OFFSET_64 = 0xA00000000LL;

//=============================================================================
//                  HELPER CLASSES AND FUNCTIONS  FOR TESTING
//-----------------------------------------------------------------------------


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
#ifdef BSLS_PLATFORM_OS_WINDOWS
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

static my_Mutex aSsErT_mutex;

static void aSsErT(int c, const char *s, int i, bool locked = false) {
    if (c) {
        if (!locked) {
            aSsErT_mutex.lock();
            locked = true;
        }
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
    if (locked) {
        aSsErT_mutex.unlock();
    }
}


struct IntTestThreadArgs {
    my_Conditional d_barrier;
    my_Conditional d_startSig;
    my_Mutex       d_mutex;
    volatile int   d_countStarted;
    int            d_iterations;
    int            d_addVal;
    Types::Int    *d_int_p;
};

struct Int64TestThreadArgs {
    my_Conditional d_barrier;
    my_Conditional d_startSig;
    my_Mutex       d_mutex;
    volatile int   d_countStarted;
    int            d_iterations;
    bsls::Types::Int64 d_addVal;
    Types::Int64    *d_int_p;
};

struct IntSwapTestThreadArgs {
    my_Conditional d_barrier;
    my_Conditional d_startSig;
    my_Mutex       d_mutex;
    volatile int   d_countStarted;
    int            d_iterations;
    volatile int   d_value1Count;
    volatile int   d_value2Count;
    volatile int   d_errorCount;
    Types::Int    *d_int_p;
};

struct Int64SwapTestThreadArgs {
    my_Conditional d_barrier;
    my_Conditional d_startSig;
    my_Mutex       d_mutex;
    volatile int   d_countStarted;
    int            d_iterations;
    volatile int   d_value1Count;
    volatile int   d_value2Count;
    volatile int   d_errorCount;
    Types::Int64  *d_int_p;
};

struct PointerTestThreadArgs {
    my_Conditional d_barrier;
    my_Conditional d_startSig;
    my_Mutex       d_mutex;
    volatile int   d_countStarted;
    int            d_iterations;
    volatile int   d_value1Count;
    volatile int   d_value2Count;
    volatile int   d_errorCount;
    Types::Pointer *d_ptr_p;
};

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

static int myCreateThread( my_thread_t *aHandle, THREAD_ENTRY aEntry,
                           void *arg )
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    *aHandle = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)aEntry,arg,0,0);
    return *aHandle ? 0 : -1;
#else
    return pthread_create(aHandle, 0, aEntry, arg);
#endif
}

static void  myJoinThread(my_thread_t aHandle)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    WaitForSingleObject(aHandle,INFINITE);
    CloseHandle(aHandle);
#else
    pthread_join(aHandle,0);
#endif
}


struct Case9
{
    Types::Int *d_value_p;
    int         d_n;
    int         d_m;
};

struct Case9_64
{
    Types::Int64  *d_value_p;
    Types::Pointer d_termPtr;
    int            d_n;
    int            d_m;
};

static void* case9Thread(void* ptr)
{
    Case9 *args = (Case9*) ptr;

    for (int i = 0; i < args->d_m; ++i) {
        int value = Obj::getIntAcquire(args->d_value_p);
        int newValue = Obj::addIntNvAcqRel(args->d_value_p, 1);
        LOOP2_ASSERT(value, newValue,
                    newValue > value && newValue <= args->d_m * args->d_n);
    }
    return ptr;
}

static void* case9Thread64(void* ptr)
{
    Case9_64 *args = (Case9_64*) ptr;

    while (args == Obj::getPtrAcquire(&args->d_termPtr))
        ;

    bsls::Types::Int64 limit = args->d_m * args->d_n + OFFSET_64;

    for (int i = 0; i < args->d_m; ++i) {
        bsls::Types::Int64 value =
            Obj::getInt64Acquire(args->d_value_p);
        bsls::Types::Int64 newValue =
            Obj::addInt64NvAcqRel(args->d_value_p, 1);
        LOOP3_ASSERT(value, newValue, limit,
                     newValue > value && value <= limit);
    }

    return ptr;
}


struct Case8
{
    Types::Int *d_value_p;
    int         d_n;
    int         d_m;
};

struct Case8_64
{
    Types::Int64   *d_value_p;
    Types::Pointer  d_termPtr;
    int             d_n;
    int             d_m;
};

extern "C" {

static void* case8Thread(void* ptr)
{
    Case8 *args = (Case8*) ptr;

    for (int i = 0; i < args->d_m; ++i) {
        int value = Obj::getIntRelaxed(args->d_value_p);
        int newValue = Obj::addIntNvRelaxed(args->d_value_p, 1);
        LOOP2_ASSERT(value, newValue,
                    newValue > value && newValue <= args->d_m * args->d_n);
    }
    return ptr;
}

static void* case8Thread64(void* ptr)
{
    Case8_64 *args = (Case8_64*) ptr;

    while (args == Obj::getPtrRelaxed(&args->d_termPtr))
        ;

    bsls::Types::Int64 limit = args->d_m * args->d_n + OFFSET_64;

    for (int i = 0; i < args->d_m; ++i) {
        bsls::Types::Int64 value =
            Obj::getInt64Relaxed(args->d_value_p);
        bsls::Types::Int64 newValue =
            Obj::addInt64NvRelaxed(args->d_value_p, 1);
        LOOP3_ASSERT(value, newValue, limit,
                     newValue > value && value <= limit);
    }

    return ptr;
}


static void* incrementIntTestThread(void *ptr)
    // This function is used to test the 'incrementInt' and 'incrementIntNv'
    // functions.  It atomically increments the specified atomic integer object
    // for the specified number of iterations.
{
    IntTestThreadArgs *args=(IntTestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::incrementInt(args->d_int_p);
        Obj::incrementIntNv(args->d_int_p);
    }
    return ptr;
}

static void* decrementIntTestThread(void *ptr)
    // This function is used to test the 'decrementInt' and 'decrementIntNv'
    // functions.  It atomically decrements the specified atomic integer object
    // for the specified number of iterations.
{
    IntTestThreadArgs *args=(IntTestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::decrementInt(args->d_int_p);
        Obj::decrementIntNv(args->d_int_p);
    }
    return ptr;
}

static void* incrementInt64TestThread(void *ptr)
    // This function is used to test the 'incrementInt64' function.  It
    // atomically increments the specified 64bit atomic integer object for the
    // specified number of iterations.  When executed by multiple threads
    // concurrently, final value of the specified int should have been
    // incremented by exactly NTHREADS * NITERATIONS.
{
    Int64TestThreadArgs *args=(Int64TestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::incrementInt64(args->d_int_p);
        Obj::incrementInt64Nv(args->d_int_p);
    }
    return ptr;
}

static void* decrementInt64TestThread(void *ptr)
    // This function is used to test the 'decrementInt64' function.  It
    // atomically decrements the specified 64bit atomic integer object for the
    // specified number of iterations.  When executed by multiple threads
    // concurrently, final value of the specified int should have been
    // decremented by exactly NTHREADS * NITERATIONS.
{
    Int64TestThreadArgs *args=(Int64TestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::decrementInt64(args->d_int_p);
        Obj::decrementInt64Nv(args->d_int_p);
    }
    return ptr;
}


static void* incrementIntAcqRelTestThread(void *ptr)
    // This function is used to test the 'incrementIntAcqRel' and
    // 'incrementIntNvAcqRel' functions.  It atomically increments the
    // specified atomic integer object for the specified number of iterations.
{
    IntTestThreadArgs *args=(IntTestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::incrementIntAcqRel(args->d_int_p);
        Obj::incrementIntNvAcqRel(args->d_int_p);
    }
    return ptr;
}

static void* decrementIntAcqRelTestThread(void *ptr)
    // This function is used to test the 'decrementIntAcqRel' and
    // 'decrementIntNvAcqRel' functions.  It atomically decrements the
    // specified atomic integer object for the specified number of iterations.
{
    IntTestThreadArgs *args=(IntTestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::decrementIntAcqRel(args->d_int_p);
        Obj::decrementIntNvAcqRel(args->d_int_p);
    }
    return ptr;
}

static void* incrementInt64AcqRelTestThread(void *ptr)
    // This function is used to test the 'incrementInt64AcqRel' function.  It
    // atomically increments the specified 64bit atomic integer object for the
    // specified number of iterations.  When executed by multiple threads
    // concurrently, final value of the specified int should have been
    // incremented by exactly NTHREADS * NITERATIONS.
{
    Int64TestThreadArgs *args=(Int64TestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::incrementInt64AcqRel(args->d_int_p);
        Obj::incrementInt64NvAcqRel(args->d_int_p);
    }
    return ptr;
}

static void* decrementInt64AcqRelTestThread(void *ptr)
    // This function is used to test the 'decrementInt64AcqRel' function.  It
    // atomically decrements the specified 64bit atomic integer object for the
    // specified number of iterations.  When executed by multiple threads
    // concurrently, final value of the specified int should have been
    // decremented by exactly NTHREADS * NITERATIONS.
{
    Int64TestThreadArgs *args=(Int64TestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::decrementInt64AcqRel(args->d_int_p);
        Obj::decrementInt64NvAcqRel(args->d_int_p);
    }
    return ptr;
}


static void* addIntTestThread(void *ptr)
    // This function is used to test the 'addInt' function.  It atomically
    // adds the specified 'd_addVal' to the specified atomic integer object
    // for the specified number of iterations.
{
    IntTestThreadArgs *args=(IntTestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::addInt(args->d_int_p,args->d_addVal);
    }

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::addIntNv(args->d_int_p,args->d_addVal);
    }
    return ptr;
}

static void* addInt64TestThread(void *ptr)
    // This function is used to test the 'addInt64' and 'addInt64Nv' functions.
    // It atomically adds the specified 'd_addVal' to the specified 64 bit
    // atomic integer object.  for the specified number of iterations.
{
    Int64TestThreadArgs *args=(Int64TestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::addInt64(args->d_int_p,args->d_addVal);
    }

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::addInt64Nv(args->d_int_p,args->d_addVal);
    }
    return ptr;
}


static void* addIntAcqRelTestThread(void *ptr)
    // This function is used to test the 'addInt' function.  It atomically
    // adds the specified 'd_addVal' to the specified atomic integer object
    // for the specified number of iterations.
{
    IntTestThreadArgs *args=(IntTestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::addIntAcqRel(args->d_int_p,args->d_addVal);
    }

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::addIntNvAcqRel(args->d_int_p,args->d_addVal);
    }
    return ptr;
}

static void* addInt64AcqRelTestThread(void *ptr)
    // This function is used to test the 'addInt64' and 'addInt64Nv' functions.
    // It atomically adds the specified 'd_addVal' to the specified 64 bit
    // atomic integer object.  for the specified number of iterations.
{
    Int64TestThreadArgs *args=(Int64TestThreadArgs*)ptr;

    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::addInt64AcqRel(args->d_int_p,args->d_addVal);
    }

    for(int i=0; i < args->d_iterations; ++i) {
        Obj::addInt64NvAcqRel(args->d_int_p,args->d_addVal);
    }
    return ptr;
}


static void* swapIntTestThread(void *ptr)
    // This function is used to test the 'swapInt' function.  It atomically
{
    IntSwapTestThreadArgs *args=(IntSwapTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        int oldValue =
            Obj::swapInt(args->d_int_p,INT_SWAPTEST_VALUE2);
        ASSERT(oldValue == INT_SWAPTEST_VALUE1 ||
               oldValue == INT_SWAPTEST_VALUE2);
        if (oldValue == INT_SWAPTEST_VALUE1) value1Count++;
        else if(oldValue == INT_SWAPTEST_VALUE2) value2Count++;
        else errorCount++;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}

static void* swapInt64TestThread(void *ptr)
    // This function is used to test the 'swapInt' function.  It atomically
{
    Int64SwapTestThreadArgs *args=(Int64SwapTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        bsls::Types::Int64 oldValue=
            Obj::swapInt64(args->d_int_p,INT64_SWAPTEST_VALUE2);
        if (oldValue == INT64_SWAPTEST_VALUE1) ++value1Count;
        else if (oldValue == INT64_SWAPTEST_VALUE2) ++value2Count;
        else ++errorCount;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}


static void* testAndSwapIntTestThread(void *ptr)
    // This function is used to test the 'swapInt' function.
{
    IntSwapTestThreadArgs *args=(IntSwapTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        int oldValue =
            Obj::testAndSwapInt(args->d_int_p,INT_SWAPTEST_VALUE1,
                                INT_SWAPTEST_VALUE2);
        ASSERT(oldValue == INT_SWAPTEST_VALUE1 ||
               oldValue == INT_SWAPTEST_VALUE2);
        if (oldValue == INT_SWAPTEST_VALUE1) value1Count++;
        else if(oldValue == INT_SWAPTEST_VALUE2) value2Count++;
        else errorCount++;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}

static void* testAndSwapInt64TestThread(void *ptr)
    // This function is used to test the 'testAndSwapInt64' function.
{
    Int64SwapTestThreadArgs *args=(Int64SwapTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        bsls::Types::Int64 oldValue=
            Obj::testAndSwapInt64(args->d_int_p,INT64_SWAPTEST_VALUE1,
                                  INT64_SWAPTEST_VALUE2);
        if (oldValue == INT64_SWAPTEST_VALUE1) ++value1Count;
        else if (oldValue == INT64_SWAPTEST_VALUE2) ++value2Count;
        else ++errorCount;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}


static void* swapPtrTestThread(void *ptr)
    // This function is used to test the 'swapPtr' function.
{
    PointerTestThreadArgs *args=(PointerTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        void *oldValue=
            Obj::swapPtr(args->d_ptr_p,POINTER_SWAPTEST_VALUE2);
        if (oldValue == POINTER_SWAPTEST_VALUE1) ++value1Count;
        else if (oldValue == POINTER_SWAPTEST_VALUE2) ++value2Count;
        else ++errorCount;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}

static void* testAndSwapPtrTestThread(void *ptr)
    // This function is used to test the 'testAndSwap' function.
{
    PointerTestThreadArgs *args=(PointerTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        void *oldValue=
            Obj::testAndSwapPtr(args->d_ptr_p,POINTER_SWAPTEST_VALUE1,
                                  POINTER_SWAPTEST_VALUE2);
        if (oldValue == POINTER_SWAPTEST_VALUE1) ++value1Count;
        else if (oldValue == POINTER_SWAPTEST_VALUE2) ++value2Count;
        else ++errorCount;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}


static void* swapIntAcqRelTestThread(void *ptr)
    // This function is used to test the 'swapInt' function.  It atomically
{
    IntSwapTestThreadArgs *args=(IntSwapTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        int oldValue =
            Obj::swapIntAcqRel(args->d_int_p,INT_SWAPTEST_VALUE2);
        ASSERT(oldValue == INT_SWAPTEST_VALUE1 ||
               oldValue == INT_SWAPTEST_VALUE2);
        if (oldValue == INT_SWAPTEST_VALUE1) value1Count++;
        else if(oldValue == INT_SWAPTEST_VALUE2) value2Count++;
        else errorCount++;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}

static void* swapInt64AcqRelTestThread(void *ptr)
    // This function is used to test the 'swapInt' function.  It atomically
{
    Int64SwapTestThreadArgs *args=(Int64SwapTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        bsls::Types::Int64 oldValue=
            Obj::swapInt64AcqRel(args->d_int_p,INT64_SWAPTEST_VALUE2);
        if (oldValue == INT64_SWAPTEST_VALUE1) ++value1Count;
        else if (oldValue == INT64_SWAPTEST_VALUE2) ++value2Count;
        else ++errorCount;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}


static void* testAndSwapIntAcqRelTestThread(void *ptr)
    // This function is used to test the 'swapInt' function.
{
    IntSwapTestThreadArgs *args=(IntSwapTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        int oldValue =
            Obj::testAndSwapIntAcqRel(args->d_int_p,INT_SWAPTEST_VALUE1,
                                INT_SWAPTEST_VALUE2);
        ASSERT(oldValue == INT_SWAPTEST_VALUE1 ||
               oldValue == INT_SWAPTEST_VALUE2);
        if (oldValue == INT_SWAPTEST_VALUE1) value1Count++;
        else if(oldValue == INT_SWAPTEST_VALUE2) value2Count++;
        else errorCount++;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}

static void* testAndSwapInt64AcqRelTestThread(void *ptr)
    // This function is used to test the 'testAndSwapInt64' function.
{
    Int64SwapTestThreadArgs *args=(Int64SwapTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        bsls::Types::Int64 oldValue=
            Obj::testAndSwapInt64AcqRel(args->d_int_p,INT64_SWAPTEST_VALUE1,
                                  INT64_SWAPTEST_VALUE2);
        if (oldValue == INT64_SWAPTEST_VALUE1) ++value1Count;
        else if (oldValue == INT64_SWAPTEST_VALUE2) ++value2Count;
        else ++errorCount;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}


static void* swapPtrAcqRelTestThread(void *ptr)
    // This function is used to test the 'swapPtr' function.
{
    PointerTestThreadArgs *args=(PointerTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        void *oldValue=
            Obj::swapPtrAcqRel(args->d_ptr_p,POINTER_SWAPTEST_VALUE2);
        if (oldValue == POINTER_SWAPTEST_VALUE1) ++value1Count;
        else if (oldValue == POINTER_SWAPTEST_VALUE2) ++value2Count;
        else ++errorCount;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}

static void* testAndSwapPtrAcqRelTestThread(void *ptr)
    // This function is used to test the 'testAndSwap' function.
{
    PointerTestThreadArgs *args=(PointerTestThreadArgs*)ptr;
    int value1Count=0;
    int value2Count=0;
    int errorCount=0;
    args->d_mutex.lock();
    args->d_countStarted++;
    args->d_startSig.signal();
    args->d_mutex.unlock();

    args->d_barrier.wait();

    for(int i=0; i < args->d_iterations; ++i) {
        void *oldValue=
            Obj::testAndSwapPtrAcqRel(args->d_ptr_p,POINTER_SWAPTEST_VALUE1,
                                  POINTER_SWAPTEST_VALUE2);
        if (oldValue == POINTER_SWAPTEST_VALUE1) ++value1Count;
        else if (oldValue == POINTER_SWAPTEST_VALUE2) ++value2Count;
        else ++errorCount;
    }
    args->d_mutex.lock();
    args->d_value1Count += value1Count;
    args->d_value2Count += value2Count;
    args->d_errorCount  += errorCount;
    args->d_mutex.unlock();
    return ptr;
}

}

//=============================================================================
//                       USAGE EXAMPLE FROM HEADER
//-----------------------------------------------------------------------------
// EXAMPLE 1

int processNextTransaction()
    // Dummy implementation
{
    return 0;
}

void createWorkerThread()
    // Dummy implementation
{
}

void waitAllThreads()
    // Dummy implementation
{
}

static Types::Int64 transactionCount;
static Types::Int64 successCount;
static Types::Int64 failureCount;

void serverMain()
{
    const int num_threads = 10;
    Obj::initInt64(&transactionCount, 0);
    Obj::initInt64(&successCount, 0);
    Obj::initInt64(&failureCount, 0);
    for (int i=0; i<num_threads; ++i) {
        createWorkerThread();
    }
    waitAllThreads();
}

///Example 2: Thread-safe Counted Handle
///- - - - - - - - - - - - - - - - - - -
// The following example demonstrates the use of atomic integer operations to
// implement a thread-safe ref-counted handle similar to a shared pointer.
// Each handle (of type 'my_CountedHandle') maintains a pointer to a
// representation object, 'my_CountedHandleRep', which in turn, stores both a
// pointer to the managed object and a reference counter.
//
// Both the handle class and the representation class are template classes with
// two template parameters.  The template parameter, 'INSTANCE', represents the
// type of the "instance", or managed object.
//
// A representation object can be shared by several handle objects.  When a
// handle object is assigned to a second handle object, the address of the
// representation is copied to the second handle, and the reference count on
// the representation is atomically incremented.  When a handle releases its
// reference to the representation, it atomically decrements the reference
// count.  If the resulting reference count becomes 0 (and there are no more
// references to the object), the handle deletes the representation object and
// the representation object, in turn, deletes the managed object ('INSTANCE').
//
///Class 'my_CountedHandleRep'
///-  -  -  -  -  -  -  -  -
// First, we define class 'my_CountedHandleRep'.  This class manages a single
// 'INSTANCE' object on behalf of multiple "handle" objects; since different
// "handle" objects may be active in different threads, class
// 'my_CountedHandleRep' must be (fully) thread-safe.  Specifically, methods
// 'increment' and 'decrement' must work atomically.
//
// Note that, this rep class is intended to be used only by class
// 'my_CountedHandle', and thus all methods of class 'my_CountedHandleRep' are
// declared private, and 'friend' status is granted to class
// 'my_CountedHandle':
//..
                        // =========================
                        // class my_CountedHandleRep
                        // =========================

template <class INSTANCE>
class my_CountedHandle;

template <class INSTANCE>
class my_CountedHandleRep {

    // DATA
    bsls::AtomicOperations::AtomicTypes::Int
                         d_count;        // number of active references
    INSTANCE            *d_instance_p;   // address of managed instance

    // FRIENDS
    friend class my_CountedHandle<INSTANCE>;

  private: // not implemented
    my_CountedHandleRep(const my_CountedHandleRep&);
    my_CountedHandleRep& operator=(const my_CountedHandleRep&);

  public:
    // CLASS METHODS
    static void
    deleteObject(my_CountedHandleRep<INSTANCE> *object);

    // CREATORS
    my_CountedHandleRep(INSTANCE *instance);
    ~my_CountedHandleRep();

    // MANIPULATORS
    void increment();
    int decrement();
};
//..
///Class 'my_CountedHandle'
///-  -  -  -  -  -  -  - -
// Then, we create class 'my_CountedHandle' that provides an individual handle
// to the shared, reference-counted object.  Each 'my_CountedHandle' object
// acts as a smart pointer, supplying an overloaded 'operator->' that provides
// access to the underlying 'INSTANCE' object via pointer semantics.
//
// 'my_CountedHandle' can also be copied freely; the copy constructor will use
// the 'increment' method from 'my_CountedHandleRep' to note the extra copy.
// Similarly, the destructor will call 'my_CountedHandleRep::decrement' to note
// that there is one fewer handle the underlying 'INSTANCE' has, and delete the
// "rep" object when its reference count is reduced to zero:
//..
                        // ======================
                        // class my_CountedHandle
                        // ======================

template <class INSTANCE>
class my_CountedHandle {

    // DATA
    my_CountedHandleRep<INSTANCE> *d_rep_p;  // shared rep.

  public:
    // CREATORS
    my_CountedHandle(INSTANCE *instance);

    my_CountedHandle(const my_CountedHandle<INSTANCE>& other);

    ~my_CountedHandle();

    // ACCESSORS
    INSTANCE *operator->() const;
    int numReferences() const;
};
//..
///Function Definitions for 'my_CountedHandleRep'
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// Next, we provide a definition for the 'static' 'deleteObject' method, which
// is called by the destructor for class 'my_CountedHandle' for the last
// instance of 'my_CountedHandle' using the given "rep" object:
//..
template <class INSTANCE>
inline
void my_CountedHandleRep<INSTANCE>::deleteObject(
                             my_CountedHandleRep<INSTANCE> *object)
{
    delete object;
}
//..
// Then, we write the constructor for the 'my_CountedHandleRep<INSTANCE>'
// class.  We initialize the atomic reference counter to one reference using
// 'bsls::AtomicOperations::initInt'.  This reflects the fact that this
// constructor will be called by a new instance of 'my_CountedHandle'.  That
// instance is our first and only handle when this constructor is called:
//..
template <class INSTANCE>
inline
my_CountedHandleRep<INSTANCE>::
                        my_CountedHandleRep(INSTANCE        *instance)
: d_instance_p(instance)
{
    bsls::AtomicOperations::initInt(&d_count, 1);
}
//..
// Then, we define the destructor, which just deletes 'my_CountedHandle'
// 'd_instance_p':
//..
template <class INSTANCE>
inline
my_CountedHandleRep<INSTANCE>::~my_CountedHandleRep()
{
    delete d_instance_p;
}
//..
// Next, we define method 'increment', which atomically increments the number
// of references to this 'my_CountedHandleRep'.  Since our caller is not
// interested in the result (and our return type is thus 'void'), we use
// 'incrementInt' instead of 'incrementIntNv'.
//..
// MANIPULATORS
template <class INSTANCE>
inline
void my_CountedHandleRep<INSTANCE>::increment()
{
    bsls::AtomicOperations::incrementInt(&d_count);
}
//..
// Then, we implement method 'decrement', which atomically decrements the
// reference count; since our caller will need to check the resulting value to
// determine whether the 'INSTANCE' should be deleted, we use 'decrementIntNv'
// rather than 'decrementInt', and return the new number of references:
//..
template <class INSTANCE>
inline
int my_CountedHandleRep<INSTANCE>::decrement()
{
    return bsls::AtomicOperations::decrementIntNv(&d_count);
}
//..
///Function Definitions for 'my_CountedHandle'
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// Next, we define the first constructor for 'my_CountedHandle', which is used
// when creating a handle for a new 'INSTANCE'; note that the 'INSTANCE' is
// constructed separately, and a pointer to that object is passed as the first
// argument ('object'):
//..
                        // ----------------------
                        // class my_CountedHandle
                        // ----------------------

template <class INSTANCE>
inline
my_CountedHandle<INSTANCE>::my_CountedHandle(INSTANCE *instance)
{
    d_rep_p = new my_CountedHandleRep<INSTANCE>(instance);
}
//..
// Then, we define the copy constructor; the new object copies the underlying
// 'my_CountedHandleRep' and then increments its counter:
//..
template <class INSTANCE>
inline
my_CountedHandle<INSTANCE>::my_CountedHandle(
                                   const my_CountedHandle<INSTANCE>& other)
: d_rep_p(other.d_rep_p)
{
    if (d_rep_p) {
        d_rep_p->increment();
    }
}
//..
// Next, we define the destructor which decrements the "rep" object's reference
// count using the 'decrement' method.  The 'decrement' method returns the
// object's reference count after the decrement is completed, and
// 'my_CountedHandle' uses this value to determine whether the "rep" object
// should be deleted:
//..
template <class INSTANCE>
inline
my_CountedHandle<INSTANCE>::~my_CountedHandle()
{
    if (d_rep_p && 0 == d_rep_p->decrement()) {
        my_CountedHandleRep<INSTANCE>::deleteObject(d_rep_p);
    }
}
//..
// Now, we define member 'operator->()', which provides basic pointer semantics
// for 'my_CountedHandle':
//..
template <class INSTANCE>
inline
INSTANCE *my_CountedHandle<INSTANCE>::operator->() const
{
    return d_rep_p->d_instance_p;
}
//..
// Finally, we define method 'numReferences', which returns the value of the
// reference counter:
//..
template <class INSTANCE>
inline
int my_CountedHandle<INSTANCE>::numReferences() const
{
    return d_rep_p ? bsls::AtomicOperations::getInt(d_rep_p->d_count) : 0;
}
//..
// Note that, while class 'my_CountedHandleRep' is itself fully thread-safe, it
// does not guarantee thread safety for the 'INSTANCE' object.  In order to
// provide thread safety for the 'INSTANCE' in the general case, the "rep"
// would need to use a more general concurrency mechanism such as a mutex.
//
///Example 3: Thread-Safe Lock-Free Singly-Linked List
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the use of atomic pointers to implement a fast and
// thread-aware, yet fast single-linked list.  The example class,
// 'my_PtrStack', is a templatized pointer stack, supporting 'push' and 'pop'
// methods.  The class is implemented using a single-linked list.  Nodes in the
// list are linked together using atomic operations.  Instance of this
// structure are allocated using the provided allocator.  When nodes are freed,
// they are cached on a free list.  This free list is also implemented as a
// single-linked list, using atomic pointer operations.
//
// First, we create class template, 'my_PtrStack', parameterized by 'TYPE'.
// Instances of this template maintain a list of nodes and a free-node list.
// Each node has a pointer to a data item, 'd_item_p', a link to the next node
// in the list, 'd_next_p'.  The definition of the 'my_PtrStack' class is
// provided below:
//..
template <class TYPE>
class my_PtrStack {
    // TYPES
    struct Node {
        TYPE *d_item_p;
        Node *d_next_p;
    };

    // DATA
    bsls::AtomicOperations::AtomicTypes::Pointer  d_list_p;
    bsls::AtomicOperations::AtomicTypes::Pointer  d_freeList_p;

    // PRIVATE MANIPULATORS
    Node *allocateNode();
    void freeNode(Node *node);
    void deleteNodes(const Node *node);

  public:
    // CREATORS
    my_PtrStack();
   ~my_PtrStack();

    // MANIPULATORS
    void push(TYPE *item);
    TYPE *pop();
};
//..
// Then, we write the constructor that initializes the pointers for the node
// list and the free list:
//..
// CREATORS
template <class TYPE>
inline my_PtrStack<TYPE>::my_PtrStack()
{
    bsls::AtomicOperations::initPointer(&d_freeList_p, 0);
    bsls::AtomicOperations::initPointer(&d_list_p, 0);
}
//..
// Next, we define the 'deleteNodes' and the destructor function to delete
// nodes that the 'my_PtrStack' object owns.  Note that we don't need to worry
// about the concurrent access to node lists in the destructor, as destructor
// can be executed in only a single thread:
//..
template <class TYPE>
inline void my_PtrStack<TYPE>::deleteNodes(const Node *node)
{
    while (node) {
        Node *next = node->d_next_p;
        delete node;
        node = next;
    }
}

template <class TYPE>
inline my_PtrStack<TYPE>::~my_PtrStack()
{
    deleteNodes(
       (const Node *) bsls::AtomicOperations::getPtrRelaxed(&d_list_p));
    deleteNodes(
       (const Node *) bsls::AtomicOperations::getPtrRelaxed(&d_freeList_p));
}
//..
// Then, we define method 'allocateNode' to get a node from the free list in
// the thread-safe manner by leveraging atomic operations to ensure proper
// thread synchronization:
//..
// PRIVATE MANIPULATORS
template <class TYPE>
inline typename my_PtrStack<TYPE>::Node *my_PtrStack<TYPE>::allocateNode()
{
    Node *node;
//..
// To remove an item from this list, get the current list head using 'getPtr'.
// Then, test and swap it with the next node.  'testAndSwapPtr' compares
// 'd_freeList_p' to 'node', replacing it with 'node->d_next_p' only if it
// matches.  If 'd_freeList_p' did not match 'node', then the free list has
// been changed on another thread, between the calls to 'getPtr' and
// 'testAndSwapPtr'.  If the list head has changed, then try again:
//..
    do {
        node = (Node*) bsls::AtomicOperations::getPtr(&d_freeList_p);
        if (!node) break;
    } while (bsls::AtomicOperations::testAndSwapPtr(&d_freeList_p,
                                                    node,
                                                    node->d_next_p) != node);
//..
// Next, we allocate a new node if there are no nodes in the free node list:
//..
    if (!node) {
        node = new Node();
    }
    return node;
}
//..
// Then, we provide the 'freeNode' method to add a given 'node' to the free
// list.  To add the node to the list, we set the next pointer of the new node
// to the current value of the list head, and atomically test and swap the head
// of the list with the new node.  If the list head has been changed (by
// another thread), we try again:
//..
template <class TYPE>
inline void my_PtrStack<TYPE>::freeNode(Node *node)
{
    do {
       node->d_next_p = (Node*) bsls::AtomicOperations::getPtr(&d_freeList_p);
    } while (bsls::AtomicOperations::testAndSwapPtr(&d_freeList_p,
                                                    node->d_next_p,
                                                    node)!= node->d_next_p);
}
//..
// Now, we begin to define the public "stack-like" interface for 'my_PtrStack'.
// Note that the 'push' method is similar to 'freeNode', except that it assigns
// an item value and operates on 'd_list_p', which maintains the list of active
// nodes:
//..
template <class TYPE>
inline void my_PtrStack<TYPE>::push(TYPE *item)
{
    Node *node = allocateNode();
    node->d_item_p = item;
    do {
        node->d_next_p = (Node*) bsls::AtomicOperations::getPtr(&d_list_p);
    } while (bsls::AtomicOperations::testAndSwapPtr(&d_list_p,
                                                    node->d_next_p,
                                                    node)!= node->d_next_p);
}
//..
// Finally, we define the 'pop' method which removes the node from the top
// of active node list, 'd_list_p', adds it to the free-node list, and returns
// the data item contained in the node to the caller:
//..
template <class TYPE>
inline TYPE *my_PtrStack<TYPE>::pop()
{
    Node *node;
    do {
        node = (Node*) bsls::AtomicOperations::getPtr(&d_list_p);
        if (!node) break;
    } while (bsls::AtomicOperations::testAndSwapPtr(&d_freeList_p,
                                                    node,
                                                    node->d_next_p)!= node);
    TYPE *item = node ? node->d_item_p : 0;
    if (node)
        freeNode(node);
    return item;
}
//..
// Notice that if the stack was empty, a NULL pointer is returned.


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------


int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

#if defined(BSLS_PLATFORM_OS_AIX)
    pthread_setconcurrency(20);
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
    pthread_setconcurrency(20);
    thr_setconcurrency(20);
#endif

    switch (test) { case 0:
      case 12: {
        // --------------------------------------------------------------------
        // TESTING GET/SET ACQUIRE/RELEASE MANIPULATORS:
        //
        // Plan:
        //   For the Int,Int64, and Pointer atomic types, for a sequence of
        //   independent test values, use the initialization functions to
        //   initialize each type and use the primary manipulator(i.e.,
        //   'setInt', 'setInt64', 'setPtr') to set its value.  Verify the
        //   value using the respective direct accessor(i.e.,'getInt',
        //   'getInt64', 'getPtr').
        //
        // Testing:
        //   setIntRelease(Obj::Int *aInt, int value);
        //   getIntAcquire(const Obj::Int &aInt);
        //   setInt64Release(Obj::Int64 *, bsls::Types::Int64);
        //   getInt64Acquire(const Obj::Int64 &aInt);
        //   getPtrAcquire(const Obj::Pointer &aPointer);
        //   setPtrRelease(Obj::Pointer *aPointer, void *value);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Get/Set Acquire/Relese Manipulators"
                          << "\n==========================================="
                          << endl;

        if (verbose) cout << "\nTesting 'Int' Primary Manipulators" << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                int d_value;       // Input value
            } VALUES[] = {
                //line value
                //---- ----
                { L_,   0   },
                { L_,   1   },
                { L_,  -1   },
                { L_,   2   },
                { L_,  -2   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getIntAcquire(&X));

                Obj::setIntRelease(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getIntAcquire(&X)); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getIntAcquire(&X));
            }
        }

        if (verbose) cout << "\nTesting 'Int64' Primary Manipulators" << endl;
        {
            static const struct {
                int       d_lineNum;     // Source line number
                bsls::Types::Int64 d_value;       // Input value
            } VALUES[] = {
                //line value
                //---- -----
                { L_,   0              },
                { L_,   1              },
                { L_,  -1              },
                { L_,  0xFFFFFFFFLL    },
                { L_,  0x100000000LL   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 VAL  = VALUES[i].d_value;

                Types::Int64 x;  const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64Acquire(&X));

                Obj::setInt64Release(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64Acquire(&X)); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt64Acquire(&X));
            }
        }


        if (verbose) cout << "\nTesting 'Pointer' Primary Manipulators"
                           << endl;
        {
            static const struct {
                int    d_lineNum;     // Source line number
                void  *d_value;       // Input value
            } VALUES[] = {
                //line value
                //---- ----
                { L_,  (void*)0x0        },
                { L_,  (void*)0x1        },
                { L_,  (void*)0xffff     },
                { L_,  (void*)0xffff8888 },
                { L_,  (void*)0xffffffff }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                void* VAL  = VALUES[i].d_value;

                Types::Pointer x;
                const Types::Pointer& X = x;

                Obj::initPointer(&x,0);
                ASSERT(0 == Obj::getPtrAcquire(&X));

                Obj::setPtrRelease(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getPtrAcquire(&X)); P_(VAL);
                }
                LOOP_ASSERT(i, VAL == Obj::getPtrAcquire(&X));
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING ARITHMETIC ACQUIRE/RELEASE MANIPULATORS
        //   Test that the 32/64 bit integer add functions work as expected.
        // Plan:
        //   For each atomic type('Int', and 'Int64') using a sequence of
        //   independent values, begin by initializing the value to 0
        //   and adding the test value.  Assert the resulting
        //   value is the expected value.  Repeat the operation using
        //   the "Nv" operation and assert the both the resulting
        //   value of the object and the return value from the
        //   operation are both the expected value.  Next, initialize
        //   each object to a base value add the delta value.  Assert
        //   that the result are still correct.  Repeat this step for
        //   the "Nv" operation and assert that the value return value
        //   and resulting object values are correct.  Finally create
        //   a series of threads each of which will concurrently add a
        //   specific value to a common instance.  If the final value
        //   is the expected value, then the operations are assumed to
        //   behave correctly when concurrently accessed by multiple
        //   threads.
        // Testing:
        //   addIntAcqRel(Obj::Int *aInt, int value);
        //   addInt64AcqRel(Obj::Int64 *, bsls::Types::Int64);
        //   addIntNvAcqRel(Obj::Int *aInt, int value);
        //   addInt64NvAcqRel(Obj::Int64 *, bsls::Types::Int64);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting Arithmetic Acquire/Release Manipulators"
                 << "\n==============================================="
                 << endl;

        if (verbose) cout << "\nTesting 'Int' Arithmetic Manipulators" << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                int d_value;       // Input value
            } VALUES[] = {
                //line d_x
                //---- ----
                { L_,   0   },
                { L_,   1   },
                { L_,  -1   },
                { L_,   2   },
                { L_,  -2   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof VALUES[0];

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                Obj::addIntAcqRel(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X)); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(&X));
            }

            if (verbose) cout <<
                "\n\tTesting 'Int' Arithmetic(and values) Manipulators\n" <<
                "\n\t-------------------------------------------------"
                              << endl;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                int       result;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                result = Obj::addIntNvAcqRel(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X)); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(&X));
                LOOP_ASSERT(i, VAL == result);
            }

        }

        if (verbose) cout << "\n\tTesting 'Int' Arith(with base) Manip"
                          << endl;

        {
            static const struct {
                int  d_lineNum;    // Source line number
                int d_base;        // Base value
                int d_amount;      // Amount to add
                int d_expected;    // Expected value
            } VALUES[] = {
                //line d_base          d_amount d_expected
                //---- --------        -------- ----------
                { L_,   0             , -9    , -9         },
                { L_,   1             , 0     , 1          },
                { L_,        -1       , 1     , 0          },
                { L_, (int) 0xFFFFFFFF, 1     , 0          },
                { L_,        -2       , -2    , -4         }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int BASE = VALUES[i].d_base;
                const int AMT  = VALUES[i].d_amount;
                const int EXP  = VALUES[i].d_expected;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                Obj::setInt(&x,BASE);
                ASSERT(BASE == Obj::getInt(&X));

                Obj::addIntAcqRel(&x,AMT);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X));
                    P_(BASE); P_(AMT); P_(EXP); NL();
                }
                LOOP_ASSERT(i, EXP == Obj::getInt(&X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int BASE = VALUES[i].d_base;
                const int AMT  = VALUES[i].d_amount;
                const int EXP  = VALUES[i].d_expected;
                int       result;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                Obj::setInt(&x,BASE);
                ASSERT(BASE == Obj::getInt(&X));

                result = Obj::addIntNvAcqRel(&x,AMT);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X));
                    P_(BASE); P_(AMT); P_(EXP); P_(result); NL();
                }
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == Obj::getInt(&X));
            }

        }

        if (verbose) cout << "\nTesting 'Int64' Arithmetic Manipulators"
                          << endl;
        {
            static const struct {
                int       d_lineNum;     // Source line number
                bsls::Types::Int64 d_value;       // Input value
            } VALUES[] = {
                //line d_x
                //---- ----
                { L_,   0   },
                { L_,   1   },
                { L_,  -1LL   },
                { L_,   2   },
                { L_,  -2LL   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 VAL  = VALUES[i].d_value;

                Types::Int64 x;  const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                Obj::addInt64AcqRel(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X)); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt64(&X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 VAL  = VALUES[i].d_value;
                bsls::Types::Int64       result;

                Types::Int64 x;  const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                result = Obj::addInt64NvAcqRel(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X));
                    P_(VAL); P_(result); NL();
                }
                LOOP_ASSERT(i, VAL == result);
                LOOP_ASSERT(i, VAL == Obj::getInt64(&X));
            }

        }
        {
            static const struct {
                int  d_lineNum;      // Source line number
                bsls::Types::Int64 d_base;    // Base value
                bsls::Types::Int64 d_amount;  // Amount to add
                bsls::Types::Int64 d_expected; // Expected value
            } VALUES[] = {
                //line d_base        d_amount d_expected
                //---- ------------- -------- ----------
                { L_,  -1LL         , 10      , 9                    },
                { L_,  1            , -2LL    , -1LL                 },
                { L_,  -1LL         , 2LL     , 1LL                  },
                { L_,  0xFFFFFFFFLL , 1LL    , 0x100000000LL        },
                { L_,  0x100000000LL, -2LL    , 0xFFFFFFFELL         }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 BASE = VALUES[i].d_base;
                const bsls::Types::Int64 AMT  = VALUES[i].d_amount;
                const bsls::Types::Int64 EXP  = VALUES[i].d_expected;

                Types::Int64 x;  const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                Obj::setInt64(&x,BASE);
                ASSERT(BASE == Obj::getInt64(&X));

                Obj::addInt64AcqRel(&x,AMT);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X)); P(BASE);
                    T_(); P_(AMT); P(EXP);
                }
                LOOP_ASSERT(i, EXP == Obj::getInt64(&X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 BASE = VALUES[i].d_base;
                const bsls::Types::Int64 AMT  = VALUES[i].d_amount;
                const bsls::Types::Int64 EXP  = VALUES[i].d_expected;
                bsls::Types::Int64       result;

                Types::Int64 x;  const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                Obj::setInt64(&x,BASE);
                ASSERT(BASE == Obj::getInt64(&X));

                result = Obj::addInt64NvAcqRel(&x,AMT);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X)); P(BASE);
                    T_(); P_(AMT); P(EXP); NL();
                }
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == Obj::getInt64(&X));
            }

        }
        if (verbose) cout << "\nTesting 'Int' add Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int ADDVAL = 3;
            const int EXPTOTAL=NTHREADS*NITERATIONS*ADDVAL*2;
            const int STARTVALUE=0;

            Types::Int mInt;

            Obj::initInt(&mInt,STARTVALUE);
            IntTestThreadArgs args;
            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = ADDVAL;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i],
                               addIntAcqRelTestThread,
                               &args);
                args.d_startSig.wait();
            }

            ASSERT(STARTVALUE == Obj::getInt(&mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt(&mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt(&mInt)); P(EXPTOTAL);
                T_(); P(STARTVALUE); NL();
            }
        }
        if (verbose) cout << "\nTesting 'Int64' add Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int ADDVAL = 33;
            const bsls::Types::Int64 STARTVALUE=0xfffff000;
            const bsls::Types::Int64 EXPTOTAL=(NTHREADS*NITERATIONS*
                                                     ADDVAL * 2) + STARTVALUE;
            Types::Int64 mInt;

            Int64TestThreadArgs args;
            Obj::initInt64(&mInt,STARTVALUE);

            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = ADDVAL;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i], addInt64AcqRelTestThread,
                                &args);
                args.d_startSig.wait();
            }
            ASSERT(STARTVALUE == Obj::getInt64(&mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt64(&mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt64(&mInt)); P(EXPTOTAL);
                T_(); P(STARTVALUE);
            }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING SWAP ACQUIRE/RELEASE MANIPULATORS:
        //   Test the swap and test and swap acquire/release functions for the
        //   Int,Int64, and Pointer atomic types.
        //
        // Plan:
        //   For each atomic type("Int", "Int64", "Pointer"), perform the
        //   following tests to test the swap, and testAndSwap manipulators.
        //
        // 1 Using an independent sequence of values, initialize an object and
        //   set its value to a base value.  Next 'swap' it with a second test
        //   value and assert that the new value is the swapped value and that
        //   the return value is the base value.  Then create a series of
        //   threads, each of which will concurrently swap the same object
        //   with a predefined test value and increment counters indicating the
        //   number of times a particular value was seen.  A third thread will
        //   concurrently swap the object to second test value and also
        //   increment counters indicating the number of times a particular
        //   value was seen.  Finally assert that final counts equal the total
        //   number of times each value should have been seen.
        //
        // 2 Using an independent sequence of values, initialize an object and
        //   set its value to a base value.  Next 'testAndSwap' it with a
        //   second test value and assert that the new value is the expected
        //   value and that the return value is the expected return value.
        //   Then create a series of threads, each of which will concurrently
        //   test and swap the same object with a predefined test value and
        //   increment counters indicating the number of times a particular
        //   value was seen.  A third thread will concurrently swap the object
        //   to second test value and also increment counters indicating the
        //   number of times a particular value was seen.  Finally assert that
        //   final counts equal the total number of times each value should
        //   have been seen.
        //
        // Testing:
        //   swapInt64AcqRel(Obj::Int *aInt, int value);
        //   testAndSwapIntAcqRel(Obj::Int *, int, int);
        //   swapInt64AcqRel(Obj::Int64 *, bsls::Types::Int64);
        //   testAndSwapInt64AcqRel(Obj::Int64 *, bsls::Types::Int6
        //                          bsls::Types::Int64 );
        //   swapPtrAcqRel(Obj::Pointer *aPointer, void *value);
        //   testAndSwapPtrAcqRel(Obj::Pointer *, void *, void *);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting SWAP Acquire/Release Manipulators"
                          << "\n========================================="
                          << endl;

        if (verbose) cout << "\nTesting 'Int' SWAP Manipulators" << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                int d_value;       // Initial value
                int d_swapValue;   // Swap value
            } VALUES[] = {
                //line value swap
                //---- ----- -------
                { L_,   0   , 11     },
                { L_,   1   , 19     },
                { L_,  -1   , 4      },
                { L_,   2   , -4     },
                { L_,  -2   , 16     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL    = VALUES[i].d_value;
                const int SWPVAL = VALUES[i].d_swapValue;
                int       result = 0;

                Types::Int x; const Types::Int& X = x;
                Obj::initInt(&x);
                ASSERT(0 == Obj::getInt(&x));

                Obj::setInt(&x,VAL);
                result = Obj::swapIntAcqRel(&x,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&x)); P_(VAL);P_(SWPVAL);NL();
                }
                LOOP_ASSERT(i, SWPVAL == Obj::getInt(&X));
                LOOP_ASSERT(i, VAL    == result );
            }
        }

        if (verbose) cout << endl
                          << "\tTesting 'testAndSwapInt'" << endl
                          << "\t------------------------" << endl;
        {
            static const struct {
                int  d_lineNum;    // Source line number
                int d_value;       // Initial value
                int d_swapValue;   // Swap value
                int d_cmpValue;    // Compare value
                int d_expValue;    // Expected value after the operations
                int d_expResult;   // Expected result
            } VALUES[] = {
                //line value swapVal      cmpVal  expValue      expResult
                //---- ----- ------------ ------- ------------- ---------
                { L_,   0   , 11         , 33     , 0          , 0       },
                { L_,   1   , 19         , 1      , 19         , 1       },
                { L_,  -1   , 4          , 1      , -1         , -1      },
                { L_,
                  2,
                  (int) 0xFFFFFFFF,
                  2,
                  (int) 0xFFFFFFFF,
                  2
                },
                { L_,  -2   , 16         , 0      , -2         , -2      }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL    = VALUES[i].d_value;
                const int CMPVAL = VALUES[i].d_cmpValue;
                const int SWPVAL = VALUES[i].d_swapValue;
                const int EXPVAL = VALUES[i].d_expValue;
                const int EXPRES = VALUES[i].d_expResult;
                int       result = 0;

                Types::Int x; const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&x));

                Obj::setInt(&x,VAL);
                result = Obj::testAndSwapIntAcqRel(&x,CMPVAL,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X));
                    P_(VAL);P_(CMPVAL);P_(SWPVAL); P_(result);
                    P_(EXPVAL);P_(EXPRES); NL();
                }
                LOOP_ASSERT(i, EXPVAL == Obj::getInt(&X));
                LOOP_ASSERT(i, EXPRES == result );
            }
        }

        if (verbose) cout << "\nTesting 'Int64' SWAP Manipulators" << endl;
        {
            static const struct {
                int       d_lineNum;     // Source line number
                bsls::Types::Int64 d_value;       // Initial value
                bsls::Types::Int64 d_swapValue;   // Swap value
            } VALUES[] = {
                //line value swap
                //---- ----- -------
                { L_,   0LL , 11LL     },
                { L_,   1LL , 19LL     },
                { L_,  -1LL ,  4LL     },
                { L_,   2LL , -4LL     },
                { L_,  -2LL , 16LL     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 VAL    = VALUES[i].d_value;
                const bsls::Types::Int64 SWPVAL = VALUES[i].d_swapValue;
                bsls::Types::Int64       result = 0;

                Types::Int64 x; const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                Obj::setInt64(&x,VAL);
                result = Obj::swapInt64AcqRel(&x,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X)); P_(VAL);
                    P_(SWPVAL); NL();
                }
                LOOP_ASSERT(i, SWPVAL == Obj::getInt64(&X));
                LOOP_ASSERT(i, VAL    == result );
            }
        }

        {
            static const struct {
                int       d_lineNum;     // Source line number
                bsls::Types::Int64 d_value;       // Initial value
                bsls::Types::Int64 d_swapValue;   // Swap value
                bsls::Types::Int64 d_cmpValue;    // Compare value
                bsls::Types::Int64 d_expValue;    // Expected value
                                                        // after the operation
                bsls::Types::Int64 d_expResult;   // Expected result
            } VALUES[] = {
                //line value swapVal      cmpVal  expValue      expResult
                //---- ----- ------------ ------- ------------- ---------
                { L_,   0LL  , 11         , 33     , 0          , 0       },
                { L_,   1LL  , 19         , 1      , 19         , 1       },
                { L_,  -1LL  , 4          , 1      , -1LL        , -1LL     },
                { L_,   2LL  , 0xFFFFFFFFFLL, 2    , 0xFFFFFFFFFLL, 2       },
                { L_,  -2LL  , 16         , 0      , -2LL        , -2LL     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 VAL    = VALUES[i].d_value;
                const bsls::Types::Int64 CMPVAL = VALUES[i].d_cmpValue;
                const bsls::Types::Int64 SWPVAL = VALUES[i].d_swapValue;
                const bsls::Types::Int64 EXPVAL = VALUES[i].d_expValue;
                const bsls::Types::Int64 EXPRES = VALUES[i].d_expResult;
                bsls::Types::Int64       result = 0;

                Types::Int64 x; const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                Obj::setInt64(&x,VAL);
                result = Obj::testAndSwapInt64AcqRel(&x,CMPVAL,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X));
                    P_(VAL);P_(CMPVAL);P_(SWPVAL); P_(result);
                    P_(EXPVAL);P_(EXPRES);NL();
                }
                LOOP_ASSERT(i, EXPVAL == Obj::getInt64(&X));
                LOOP_ASSERT(i, EXPRES == result );
            }
        }
        if (verbose) cout << "\nTesting 'Pointer' SWAP Manipulators" << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                void *d_value;       // Initial value
                void *d_swapValue;   // Swap value
            } VALUES[] = {
                //line value swap
                //---- ----- -------
                { L_,   (void*)0   , (void*)11     },
                { L_,   (void*)1   , (void*)19     },
                { L_,  (void*)-1   , (void*)4      },
                { L_,  (void*) 2   , (void*)-4     },
                { L_,  (void*)-2   , (void*)16     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                void *VAL    = VALUES[i].d_value;
                void *SWPVAL = VALUES[i].d_swapValue;
                void *result = 0;

                Types::Pointer x;
                const Types::Pointer& X = x;

                Obj::initPointer(&x,0);
                ASSERT(0 == Obj::getPtr(&X));

                Obj::setPtr(&x,VAL);
                result = Obj::swapPtrAcqRel(&x,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getPtr(&X)); P_(VAL);P_(SWPVAL);NL();
                }
                LOOP3_ASSERT(i, SWPVAL, Obj::getPtr(&X),
                             SWPVAL == Obj::getPtr(&X));
                LOOP_ASSERT(i, VAL    == result );
            }
        }

        {
            static const struct {
                int  d_lineNum;      // Source line number
                void *d_value;       // Initial value
                void *d_swapValue;   // Swap value
                void *d_cmpValue;    // Compare value
                void *d_expValue;    // Expected value after the operations
                void *d_expResult;   // Expected result
            } VALUES[] = {
                //line value        swapVal             cmpVal
                //---- ------------ ------------------- --------------
                //    expValue             expResult
                //    -------------------- ----------------
                { L_, (void*) 0   , (void*)11         , (void*)33     ,
                      (void*)0          , (void*)0       },
                { L_, (void*) 1   , (void*)19         , (void*)1      ,
                      (void*)19         , (void*)1       },
                { L_, (void*)-1   , (void*)4          , (void*)1      ,
                      (void*)-1         , (void*)-1      },
                { L_, (void*) 2   , (void*)0xFFFFFFFF , (void*)2      ,
                      (void*)0xFFFFFFFF , (void*)2       },
                { L_, (void*)-2   , (void*)16         , (void*)0      ,
                      (void*)-2         , (void*)-2      }
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                void *VAL    = VALUES[i].d_value;
                void *CMPVAL = VALUES[i].d_cmpValue;
                void *SWPVAL = VALUES[i].d_swapValue;
                void *EXPVAL = VALUES[i].d_expValue;
                void *EXPRES = VALUES[i].d_expResult;
                void *result = 0;

                Types::Pointer x;
                const Types::Pointer& X = x;
                Obj::initPointer(&x,0);
                ASSERT(0 == Obj::getPtr(&X));

                Obj::setPtr(&x,VAL);
                result = Obj::testAndSwapPtrAcqRel(&x,CMPVAL,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getPtr(&X));
                    P_(VAL);P_(CMPVAL);P_(SWPVAL);
                    P_(EXPVAL);P_(EXPRES);NL();
                }
                LOOP_ASSERT(i, EXPVAL == Obj::getPtr(&X));
                LOOP_ASSERT(i, EXPRES == result );
            }
        }

        if (verbose) cout << "\nTesting 'Int' swap Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=NTHREADS*NITERATIONS + NITERATIONS;

            Types::Int mInt;

            IntSwapTestThreadArgs args;
            Obj::initInt(&mInt,INT_SWAPTEST_VALUE1);

            args.d_int_p        = &mInt;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i], swapIntAcqRelTestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;

            for (int i=0; i < NITERATIONS; ++i) {
                int oldValue = Obj::swapIntAcqRel( &mInt,
                                                         INT_SWAPTEST_VALUE1);
                if (oldValue == INT_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == INT_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }

            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);

            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total); P(args.d_value1Count);
                T_(); P_(args.d_value2Count); P(args.d_errorCount);
            }
        }

        if (verbose) cout << "\nTesting 'Int64' swap Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=NTHREADS*NITERATIONS+NITERATIONS;
            Types::Int64 mInt;

            Int64SwapTestThreadArgs args;
            Obj::initInt64(&mInt,INT64_SWAPTEST_VALUE1);

            args.d_int_p        = &mInt;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i], swapInt64AcqRelTestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;
            for (int i=0; i < NITERATIONS; ++i) {
                bsls::Types::Int64 oldValue =
                    Obj::swapInt64AcqRel( &mInt,
                                                INT64_SWAPTEST_VALUE1);
                if (oldValue == INT64_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == INT64_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);

            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total);
                T_(); P_(args.d_value1Count); P_(args.d_value2Count);
                P(args.d_errorCount);
            }
        }

        if (verbose) cout << "\nTesting 'Int' testAndSwap Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=NTHREADS*NITERATIONS+NITERATIONS;
            Types::Int mInt;

            IntSwapTestThreadArgs args;
            Obj::initInt(&mInt,INT_SWAPTEST_VALUE1);

            args.d_int_p        = &mInt;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i],
                               testAndSwapIntAcqRelTestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;
            for (int i=0; i < NITERATIONS; ++i) {
                int oldValue = Obj::swapIntAcqRel(&mInt,
                                                        INT_SWAPTEST_VALUE1);
                if (oldValue == INT_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == INT_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);


            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total); P(args.d_value1Count);
                T_(); P_(args.d_value2Count); P(args.d_errorCount);
            }
        }

        if (verbose) cout << "\nTesting 'Int64' testAndSwap Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=NTHREADS*NITERATIONS+NITERATIONS;
            Types::Int64 mInt;

            Int64SwapTestThreadArgs args;
            Obj::initInt64(&mInt,INT64_SWAPTEST_VALUE1);

            args.d_int_p        = &mInt;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i],
                               testAndSwapInt64AcqRelTestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;
            for (int i=0; i < NITERATIONS; ++i) {
                bsls::Types::Int64 oldValue =
                    Obj::swapInt64AcqRel( &mInt,
                                                INT64_SWAPTEST_VALUE1);
                if (oldValue == INT64_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == INT64_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);

            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total);
                T_(); P_(args.d_value1Count); P_(args.d_value2Count);
                P(args.d_errorCount);
            }
        }

        if (verbose) cout << "\nTesting 'Pointer' swap Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=NTHREADS*NITERATIONS+NITERATIONS;
            Types::Pointer mPtr;

            PointerTestThreadArgs args;
            Obj::initPointer(&mPtr,POINTER_SWAPTEST_VALUE1);

            args.d_ptr_p        = &mPtr;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i], swapPtrAcqRelTestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;
            for (int i=0; i < NITERATIONS; ++i) {
                void* oldValue =
                    Obj::swapPtrAcqRel(&mPtr, POINTER_SWAPTEST_VALUE1);
                if (oldValue == POINTER_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == POINTER_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);

            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total);
                T_(); P_(args.d_value1Count); P_(args.d_value2Count);
                P(args.d_errorCount);
            }
        }

        if (verbose) cout << "\nTesting 'Pointer' testAndSwap Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=NTHREADS*NITERATIONS+NITERATIONS;
            Types::Pointer mPtr;

            PointerTestThreadArgs args;
            Obj::initPointer(&mPtr,POINTER_SWAPTEST_VALUE1);

            args.d_ptr_p        = &mPtr;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i],
                               testAndSwapPtrAcqRelTestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;
            for (int i=0; i < NITERATIONS; ++i) {
                void* oldValue =
                    Obj::swapPtrAcqRel(&mPtr, POINTER_SWAPTEST_VALUE1);
                if (oldValue == POINTER_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == POINTER_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);

            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total);
                T_(); P_(args.d_value1Count); P_(args.d_value2Count);
                P(args.d_errorCount);
            }
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING acquire/release methods
        //
        // Create N threads each of which invokes incrementIntNvAcqRel() M
        // times.  Verify that the final value is NxM, and that each
        // return value is in the range (x, NxM] where x is the prior value.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Acquire/Release Methods"
                          << "\n===============================" << endl;

        enum {
            N = 10,
            M = 80000
        };

        Types::Int value;
        Obj::initInt(&value, 0);

        Case9 args;
        args.d_value_p = &value;
        args.d_n = N;
        args.d_m = M;

        my_thread_t threadHandles[N];
        for (int i = 0; i < N; ++i) {
            myCreateThread(&threadHandles[i],
                           case9Thread,
                           &args);
        }

        for (int i = 0; i < N; ++i) {
            myJoinThread(threadHandles[i]);
        }

        ASSERT(N * M == Obj::getInt(&value));

        Case9_64 args64;

        Types::Int64 value64;
        Obj::setInt64Release(&value64, OFFSET_64);
        args64.d_value_p = &value64;

        Obj::setPtrRelease(&args64.d_termPtr, &args64);

        args64.d_n = N;
        args64.d_m = M;

        for (int i = 0; i < N; ++i) {
            myCreateThread(&threadHandles[i],
                           case9Thread64,
                           &args64);
        }

        Obj::setPtrRelease(&args64.d_termPtr, &args);

        for (int i = 0; i < N; ++i) {
            myJoinThread(threadHandles[i]);
        }

        bsls::Types::Int64 expected = OFFSET_64 + N * M;

        LOOP2_ASSERT(expected, Obj::getInt64(&value64),
                     expected == Obj::getInt64(&value64));

        LOOP2_ASSERT(expected, Obj::getInt64Acquire(&value64),
                     expected == Obj::getInt64Acquire(&value64));

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING relaxed methods
        //
        // Create N threads each of which invokes incrementIntNvRelaxed() M
        // times.  Verify that the final value is NxM, and that each
        // return value is in the range (x, NxM] where x is the prior value.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Relaxed Methods"
                          << "\n=======================" << endl;

        enum {
            N = 10,
            M = 80000
        };

        Types::Int value;
        Obj::initInt(&value, 0);

        Case8 args;
        args.d_value_p = &value;
        args.d_n = N;
        args.d_m = M;

        my_thread_t threadHandles[N];
        for (int i = 0; i < N; ++i) {
            myCreateThread(&threadHandles[i],
                           case8Thread,
                           &args);
        }

        for (int i = 0; i < N; ++i) {
            myJoinThread(threadHandles[i]);
        }

        ASSERT(N * M == Obj::getInt(&value));

        Case8_64 args64;

        Types::Int64 value64;
        Obj::setInt64Relaxed(&value64, OFFSET_64);
        args64.d_value_p = &value64;

        Obj::setPtrRelaxed(&args64.d_termPtr, &args64);

        args64.d_n = N;
        args64.d_m = M;

        for (int i = 0; i < N; ++i) {
            myCreateThread(&threadHandles[i],
                           case8Thread64,
                           &args64);
        }

        Obj::setPtrRelaxed(&args64.d_termPtr, &args);

        for (int i = 0; i < N; ++i) {
            myJoinThread(threadHandles[i]);
        }

        bsls::Types::Int64 expected = OFFSET_64 + N * M;

        LOOP2_ASSERT(expected, Obj::getInt64(&value64),
                     expected == Obj::getInt64(&value64));

        LOOP2_ASSERT(expected, Obj::getInt64Relaxed(&value64),
                     expected == Obj::getInt64Relaxed(&value64));

      } break;

      case 7: {
        // TESTING USAGE Examples
        //
        // Plan:
        //
        // Testing:
        {
            my_CountedHandle<double> handle(NULL);
        }
        {
            my_PtrStack<int> stack;
        }

      }break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING INCREMENT/DECREMENT ACQUIRE/RELEASE MANIPULATORS
        //   Test the atomic increment and decrement functions for the Int and
        //   Int64 atomic types.
        // Plan:
        //   For each atomic type(Int,Int64), using a sequence of
        //   independent values, begin by initializing the atomic Int/Int64 to
        //   a base value and increment the value by 1.  Next assert
        //   that the value is the expected value.  Then repeat the
        //   same operation using the "Nv" operation.  Assert that the
        //   value is the expected value, and that the returned value
        //   is also the expected value.  Finally create a series of
        //   threads to concurrently increment/decrement the same
        //   atomic Int/Int64 value for a specific number of
        //   iterations.  If the final value is the expected value,
        //   then it is assumed that the operations behave correctly
        //   in a multi-threaded environment.
        //
        // Testing:
        //   incrementIntAcqRel(Obj::Int *aInt);
        //   decrementIntAcqRel(Obj::Int *aInt);
        //   incrementIntNvAcqRel(Obj::Int *aInt);
        //   decrementIntNvAcqRel(Obj::Int *aInt);
        //   incrementInt64AcqRel(Obj::Int64 *aInt);
        //   decrementInt64AcqRel(Obj::Int64 *aInt);
        //   incrementInt64NvAcqRel(Obj::Int64 *);
        //   decrementInt64NvAcqRel(Obj::Int64 *);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTesting Increment/Decrement Acq/Rel Manipulators"
                 << "\n================================================"
                 << endl;

        if (verbose) cout << "\nTesting 'Int' Increment Manipulators"
                          << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                int d_value;       // Input value
                int d_expected;    // Expected resulting value
            } VALUES[] = {
                //line value expected
                //---- ----- --------
                { L_,   0   , 1       },
                { L_,   1   , 2       },
                { L_,  -1   , 0       },
                { L_,   2   , 3       },
                { L_,  -2   , -1      }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                const int EXP  = VALUES[i].d_expected;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                Obj::setInt(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X)); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(&X));
                Obj::incrementIntAcqRel(&x);
                LOOP_ASSERT(i, EXP == Obj::getInt(&X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                const int EXP  = VALUES[i].d_expected;
                int       result;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                Obj::setInt(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X)); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(&X));
                result = Obj::incrementIntNvAcqRel(&x);
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == Obj::getInt(&X));
            }
        }

        if (verbose) cout << "\nTesting 'Int' Increment Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=NTHREADS*NITERATIONS*2;
            const int STARTVALUE=0;

            Types::Int mInt;

            Obj::initInt(&mInt,STARTVALUE);
            IntTestThreadArgs args;
            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i],incrementIntAcqRelTestThread,
                                &args);
                args.d_startSig.wait();
            }

            ASSERT(STARTVALUE == Obj::getInt(&mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt(&mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt(&mInt)); P_(EXPTOTAL); P(STARTVALUE);
            }
        }

        if (verbose) cout << "\nTesting 'Int' Decrement Manipulators"
                          << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                int d_expected;     // Expected result of decrement
                int d_value;        // Base value to be decremented
            } VALUES[] = {
                //line expected value
                //---- -------- --------
                { L_,   0     , 1       },
                { L_,   1     , 2       },
                { L_,  -1     , 0       },
                { L_,   2     , 3       },
                { L_,  -2     , -1      }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                const int EXP  = VALUES[i].d_expected;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                Obj::setInt(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X)); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(&X));
                Obj::decrementIntAcqRel(&x);
                LOOP_ASSERT(i, EXP == Obj::getInt(&X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                const int EXP  = VALUES[i].d_expected;
                int       result;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                Obj::setInt(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X)); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(&X));
                result = Obj::decrementIntNvAcqRel(&x);
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == Obj::getInt(&X));
            }
        }

        if (verbose) cout << "\nTesting 'Int' Increment Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=NTHREADS*NITERATIONS*2;
            const int STARTVALUE=0;

            Types::Int mInt;

            Obj::initInt(&mInt,STARTVALUE);
            IntTestThreadArgs args;
            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i],incrementIntAcqRelTestThread,
                                &args);
                args.d_startSig.wait();
            }

            ASSERT(STARTVALUE == Obj::getInt(&mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt(&mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt(&mInt)); P_(EXPTOTAL); P(STARTVALUE);
            }
        }

        if (verbose) cout << "\nTesting 'Int' decrement Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=33;
            const int STARTVALUE=(NTHREADS*NITERATIONS*2)+EXPTOTAL;
            Types::Int mInt;

            IntTestThreadArgs args;
            Obj::initInt(&mInt,STARTVALUE);

            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i],
                                decrementIntAcqRelTestThread,
                                &args);
                args.d_startSig.wait();
            }
            ASSERT(STARTVALUE == Obj::getInt(&mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt(&mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt(&mInt)); P_(EXPTOTAL); P(STARTVALUE);
            }
        }


        if (verbose) cout << "\nTesting 'Int64' Increment Manipulators"
                          << endl;
        {
            static const struct {
                int       d_lineNum;     // Source line number
                bsls::Types::Int64 d_expected; // Expected resulting
                                                     // value of decrement
                bsls::Types::Int64 d_value; // Input value

            } VALUES[] = {
                //line expected                   value
                //---- -------------------        ---------------------
                { L_,   0                        , 1                    },
                { L_,   1                        , 2                    },
                { L_,  -1LL                      , 0                    },
                { L_,   0xFFFFFFFFLL             , 0x100000000LL        },
                { L_,  (int) 0xFFFFFFFFFFFFFFFFLL , 0                   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 VAL  = VALUES[i].d_value;
                const bsls::Types::Int64 EXP  = VALUES[i].d_expected;

                Types::Int64 x;  const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                Obj::setInt64(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X)); P_(VAL);P_(EXP); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt64(&X));
                Obj::decrementInt64AcqRel(&x);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X)); P_(VAL);P_(EXP); NL();
                }
                LOOP_ASSERT(i, EXP == Obj::getInt64(&X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 VAL  = VALUES[i].d_value;
                const bsls::Types::Int64 EXP  = VALUES[i].d_expected;
                bsls::Types::Int64       result;

                Types::Int64 x;  const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                Obj::setInt64(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&x)); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt64(&X));
                result = Obj::decrementInt64NvAcqRel(&x);
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == Obj::getInt64(&X));
            }
        }

        if (verbose) cout << "\nTesting 'Int64' Increment Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const bsls::Types::Int64 STARTVALUE=0xfffffff0LL;
            const bsls::Types::Int64 EXPTOTAL=NTHREADS*NITERATIONS*2+
                                           STARTVALUE;
            Types::Int64 mInt;

            Obj::initInt64(&mInt,STARTVALUE);
            Int64TestThreadArgs args;
            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i],
                                incrementInt64AcqRelTestThread,
                                &args);
                args.d_startSig.wait();
            }
            ASSERT(STARTVALUE == Obj::getInt64(&mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt64(&mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt64(&mInt)); P_(EXPTOTAL); P(STARTVALUE);
            }
        }

        if (verbose) cout << "\nTesting 'Int64' decrement Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const bsls::Types::Int64 EXPTOTAL=0xfffffff0;
            const bsls::Types::Int64 STARTVALUE=(NTHREADS*NITERATIONS*2)+
                                           EXPTOTAL;
            Types::Int64 mInt;

            Int64TestThreadArgs args;
            Obj::initInt64(&mInt,STARTVALUE);

            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i],
                                decrementInt64AcqRelTestThread,
                                &args);
                args.d_startSig.wait();
            }
            ASSERT(STARTVALUE == Obj::getInt64(&mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt64(&mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt64(&mInt)); P_(EXPTOTAL); P(STARTVALUE);
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING INCREMENT/DECREMENT MANIPULATORS
        //   Test the atomic increment and decrement functions for the Int and
        //   Int64 atomic types.
        // Plan:
        //   For each atomic type(Int,Int64), using a sequence of
        //   independent values, begin by initializing the atomic Int/Int64 to
        //   a base value and increment the value by 1.  Next assert
        //   that the value is the expected value.  Then repeat the
        //   same operation using the "Nv" operation.  Assert that the
        //   value is the expected value, and that the returned value
        //   is also the expected value.  Finally create a series of
        //   threads to concurrently increment/decrement the same
        //   atomic Int/Int64 value for a specific number of
        //   iterations.  If the final value is the expected value,
        //   then it is assumed that the operations behave correctly
        //   in a multi-threaded environment.
        //
        // Testing:
        //   incrementInt(Obj::Int *aInt);
        //   decrementInt(Obj::Int *aInt);
        //   incrementIntNv(Obj::Int *aInt);
        //   decrementIntNv(Obj::Int *aInt);
        //   incrementInt64(Obj::Int64 *aInt);
        //   decrementInt64(Obj::Int64 *aInt);
        //   incrementInt64Nv(Obj::Int64 *);
        //   decrementInt64Nv(Obj::Int64 *);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Increment/Decrement Manipulators"
                          << "\n========================================="
                          << endl;

        if (verbose) cout << "\nTesting 'Int' Increment Manipulators"
                          << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                int d_value;       // Input value
                int d_expected;    // Expected resulting value
            } VALUES[] = {
                //line value expected
                //---- ----- --------
                { L_,   0   , 1       },
                { L_,   1   , 2       },
                { L_,  -1   , 0       },
                { L_,   2   , 3       },
                { L_,  -2   , -1      }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                const int EXP  = VALUES[i].d_expected;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                Obj::setInt(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X)); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(&X));
                Obj::incrementInt(&x);
                LOOP_ASSERT(i, EXP == Obj::getInt(&X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                const int EXP  = VALUES[i].d_expected;
                int       result;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                Obj::setInt(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X)); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(&X));
                result = Obj::incrementIntNv(&x);
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == Obj::getInt(&X));
            }
        }

        if (verbose) cout << "\nTesting 'Int' Increment Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=NTHREADS*NITERATIONS*2;
            const int STARTVALUE=0;

            Types::Int mInt;

            Obj::initInt(&mInt,STARTVALUE);
            IntTestThreadArgs args;
            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i],incrementIntTestThread,
                                &args);
                args.d_startSig.wait();
            }

            ASSERT(STARTVALUE == Obj::getInt(&mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt(&mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt(&mInt)); P_(EXPTOTAL); P(STARTVALUE);
            }
        }

        if (verbose) cout << "\nTesting 'Int' Decrement Manipulators"
                          << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                int d_expected;     // Expected result of decrement
                int d_value;        // Base value to be decremented
            } VALUES[] = {
                //line expected value
                //---- -------- --------
                { L_,   0     , 1       },
                { L_,   1     , 2       },
                { L_,  -1     , 0       },
                { L_,   2     , 3       },
                { L_,  -2     , -1      }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                const int EXP  = VALUES[i].d_expected;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                Obj::setInt(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X)); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(&X));
                Obj::decrementInt(&x);
                LOOP_ASSERT(i, EXP == Obj::getInt(&X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                const int EXP  = VALUES[i].d_expected;
                int       result;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                Obj::setInt(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X)); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(&X));
                result = Obj::decrementIntNv(&x);
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == Obj::getInt(&X));
            }
        }

        if (verbose) cout << "\nTesting 'Int' Increment Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=NTHREADS*NITERATIONS*2;
            const int STARTVALUE=0;

            Types::Int mInt;

            Obj::initInt(&mInt,STARTVALUE);
            IntTestThreadArgs args;
            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i],incrementIntTestThread,
                                &args);
                args.d_startSig.wait();
            }

            ASSERT(STARTVALUE == Obj::getInt(&mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt(&mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt(&mInt)); P_(EXPTOTAL); P(STARTVALUE);
            }
        }

        if (verbose) cout << "\nTesting 'Int' decrement Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=33;
            const int STARTVALUE=(NTHREADS*NITERATIONS*2)+EXPTOTAL;
            Types::Int mInt;

            IntTestThreadArgs args;
            Obj::initInt(&mInt,STARTVALUE);

            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i], decrementIntTestThread,
                                &args);
                args.d_startSig.wait();
            }
            ASSERT(STARTVALUE == Obj::getInt(&mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt(&mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt(&mInt)); P_(EXPTOTAL); P(STARTVALUE);
            }
        }


        if (verbose) cout << "\nTesting 'Int64' Increment Manipulators"
                          << endl;
        {
            static const struct {
                int       d_lineNum;     // Source line number
                bsls::Types::Int64 d_expected; // Expected resulting
                                                     // value of decrement
                bsls::Types::Int64 d_value; // Input value

            } VALUES[] = {
                //line expected                   value
                //---- -------------------        ---------------------
                { L_,         0                  , 1                    },
                { L_,         1                  , 2                    },
                { L_,        -1LL                , 0                    },
                { L_,         0xFFFFFFFFLL       , 0x100000000LL        },
                { L_,  (int) 0xFFFFFFFFFFFFFFFFLL , 0                   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 VAL  = VALUES[i].d_value;
                const bsls::Types::Int64 EXP  = VALUES[i].d_expected;

                Types::Int64 x;  const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                Obj::setInt64(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X)); P_(VAL);P_(EXP); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt64(&X));
                Obj::decrementInt64(&x);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X)); P_(VAL);P_(EXP); NL();
                }
                LOOP_ASSERT(i, EXP == Obj::getInt64(&X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 VAL  = VALUES[i].d_value;
                const bsls::Types::Int64 EXP  = VALUES[i].d_expected;
                bsls::Types::Int64       result;

                Types::Int64 x;  const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                Obj::setInt64(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&x)); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt64(&X));
                result = Obj::decrementInt64Nv(&x);
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == Obj::getInt64(&X));
            }
        }

        if (verbose) cout << "\nTesting 'Int64' Increment Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const bsls::Types::Int64 STARTVALUE=0xfffffff0LL;
            const bsls::Types::Int64 EXPTOTAL=NTHREADS*NITERATIONS*2+
                                           STARTVALUE;
            Types::Int64 mInt;

            Obj::initInt64(&mInt,STARTVALUE);
            Int64TestThreadArgs args;
            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i], incrementInt64TestThread,
                                &args);
                args.d_startSig.wait();
            }
            ASSERT(STARTVALUE == Obj::getInt64(&mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt64(&mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt64(&mInt)); P_(EXPTOTAL); P(STARTVALUE);
            }
        }

        if (verbose) cout << "\nTesting 'Int64' decrement Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const bsls::Types::Int64 EXPTOTAL=0xfffffff0;
            const bsls::Types::Int64 STARTVALUE=(NTHREADS*NITERATIONS*2)+
                                           EXPTOTAL;
            Types::Int64 mInt;

            Int64TestThreadArgs args;
            Obj::initInt64(&mInt,STARTVALUE);

            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i], decrementInt64TestThread,
                                &args);
                args.d_startSig.wait();
            }
            ASSERT(STARTVALUE == Obj::getInt64(&mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt64(&mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt64(&mInt)); P_(EXPTOTAL); P(STARTVALUE);
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING SWAP MANIPULATORS:
        //   Test the swap and test and swap functions for the Int,Int64, and
        //   Pointer atomic types.
        //
        // Plan:
        //   For each atomic type("Int", "Int64", "Pointer"), perform the
        //   following tests to test the swap, and testAndSwap manipulators.
        //
        // 1 Using an independent sequence of values, initialize an object and
        //   set its value to a base value.  Next 'swap' it with a second test
        //   value and assert that the new value is the swapped value and that
        //   the return value is the base value.  Then create a series of
        //   threads, each of which will concurrently swap the same object
        //   with a predefined test value and increment counters indicating the
        //   number of times a particular value was seen.  A third thread will
        //   concurrently swap the object to second test value and also
        //   increment counters indicating the number of times a particular
        //   value was seen.  Finally assert that final counts equal the total
        //   number of times each value should have been seen.
        //
        // 2 Using an independent sequence of values, initialize an object and
        //   set its value to a base value.  Next 'testAndSwap' it with a
        //   second test value and assert that the new value is the expected
        //   value and that the return value is the expected return value.
        //   Then create a series of threads, each of which will concurrently
        //   test and swap the same object with a predefined test value and
        //   increment counters indicating the number of times a particular
        //   value was seen.  A third thread will concurrently swap the object
        //   to second test value and also increment counters indicating the
        //   number of times a particular value was seen.  Finally assert that
        //   final counts equal the total number of times each value should
        //   have been seen.
        //
        // Testing:
        //   swapInt64(Obj::Int *aInt, int value);
        //   testAndSwapInt(Obj::Int *, int, int);
        //   swapInt64(Obj::Int64 *, bsls::Types::Int64);
        //   testAndSwapInt64(Obj::Int64 *, bsls::Types::Int6
        //                    bsls::Types::Int64 );
        //   swapPtr(Obj::Pointer *aPointer, void *value);
        //   testAndSwapPtr(Obj::Pointer *, void *, void *);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting SWAP Manipulators"
                          << "\n========================================="
                          << endl;

        if (verbose) cout << "\nTesting 'Int' SWAP Manipulators" << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                int d_value;       // Initial value
                int d_swapValue;   // Swap value
            } VALUES[] = {
                //line value swap
                //---- ----- -------
                { L_,   0   , 11     },
                { L_,   1   , 19     },
                { L_,  -1   , 4      },
                { L_,   2   , -4     },
                { L_,  -2   , 16     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL    = VALUES[i].d_value;
                const int SWPVAL = VALUES[i].d_swapValue;
                int       result = 0;

                Types::Int x; const Types::Int& X = x;
                Obj::initInt(&x);
                ASSERT(0 == Obj::getInt(&x));

                Obj::setInt(&x,VAL);
                result = Obj::swapInt(&x,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&x)); P_(VAL);P_(SWPVAL);NL();
                }
                LOOP_ASSERT(i, SWPVAL == Obj::getInt(&X));
                LOOP_ASSERT(i, VAL    == result );
            }
        }

        if (verbose) cout << endl
                          << "\tTesting 'testAndSwapInt'" << endl
                          << "\t------------------------" << endl;
        {
            static const struct {
                int  d_lineNum;    // Source line number
                int d_value;       // Initial value
                int d_swapValue;   // Swap value
                int d_cmpValue;    // Compare value
                int d_expValue;    // Expected value after the operations
                int d_expResult;   // Expected result
            } VALUES[] = {
                //line value swapVal      cmpVal  expValue      expResult
                //---- ----- ------------ ------- ------------- ---------
                { L_,   0   , 11         , 33     , 0          , 0       },
                { L_,   1   , 19         , 1      , 19         , 1       },
                { L_,  -1   , 4          , 1      , -1         , -1      },
                { L_,
                  2,
                  (int) 0xFFFFFFFF,
                  2,
                  (int) 0xFFFFFFFF,
                  2
                },
                { L_,  -2   , 16         , 0      , -2         , -2      }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL    = VALUES[i].d_value;
                const int CMPVAL = VALUES[i].d_cmpValue;
                const int SWPVAL = VALUES[i].d_swapValue;
                const int EXPVAL = VALUES[i].d_expValue;
                const int EXPRES = VALUES[i].d_expResult;
                int       result = 0;

                Types::Int x; const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&x));

                Obj::setInt(&x,VAL);
                result = Obj::testAndSwapInt(&x,CMPVAL,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X));
                    P_(VAL);P_(CMPVAL);P_(SWPVAL); P_(result);
                    P_(EXPVAL);P_(EXPRES); NL();
                }
                LOOP_ASSERT(i, EXPVAL == Obj::getInt(&X));
                LOOP_ASSERT(i, EXPRES == result );
            }
        }

        if (verbose) cout << "\nTesting 'Int64' SWAP Manipulators" << endl;
        {
            static const struct {
                int       d_lineNum;     // Source line number
                bsls::Types::Int64 d_value;       // Initial value
                bsls::Types::Int64 d_swapValue;   // Swap value
            } VALUES[] = {
                //line value swap
                //---- ----- -------
                { L_,   0LL , 11LL     },
                { L_,   1LL , 19LL     },
                { L_,  -1LL ,  4LL     },
                { L_,   2LL , -4LL     },
                { L_,  -2LL , 16LL     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 VAL    = VALUES[i].d_value;
                const bsls::Types::Int64 SWPVAL = VALUES[i].d_swapValue;
                bsls::Types::Int64       result = 0;

                Types::Int64 x; const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                Obj::setInt64(&x,VAL);
                result = Obj::swapInt64(&x,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X)); P_(VAL);
                    P_(SWPVAL); NL();
                }
                LOOP_ASSERT(i, SWPVAL == Obj::getInt64(&X));
                LOOP_ASSERT(i, VAL    == result );
            }
        }

        {
            static const struct {
                int       d_lineNum;     // Source line number
                bsls::Types::Int64 d_value;       // Initial value
                bsls::Types::Int64 d_swapValue;   // Swap value
                bsls::Types::Int64 d_cmpValue;    // Compare value
                bsls::Types::Int64 d_expValue;    // Expected value
                                                        // after the operation
                bsls::Types::Int64 d_expResult;   // Expected result
            } VALUES[] = {
                //line value swapVal      cmpVal  expValue      expResult
                //---- ----- ------------ ------- ------------- ---------
                { L_,   0LL  , 11         , 33     , 0          , 0       },
                { L_,   1LL  , 19         , 1      , 19         , 1       },
                { L_,  -1LL  , 4          , 1      , -1LL        , -1LL     },
                { L_,   2LL  , 0xFFFFFFFFFLL, 2    , 0xFFFFFFFFFLL, 2       },
                { L_,  -2LL  , 16         , 0      , -2LL        , -2LL     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 VAL    = VALUES[i].d_value;
                const bsls::Types::Int64 CMPVAL = VALUES[i].d_cmpValue;
                const bsls::Types::Int64 SWPVAL = VALUES[i].d_swapValue;
                const bsls::Types::Int64 EXPVAL = VALUES[i].d_expValue;
                const bsls::Types::Int64 EXPRES = VALUES[i].d_expResult;
                bsls::Types::Int64       result = 0;

                Types::Int64 x; const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                Obj::setInt64(&x,VAL);
                result = Obj::testAndSwapInt64(&x,CMPVAL,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X));
                    P_(VAL);P_(CMPVAL);P_(SWPVAL); P_(result);
                    P_(EXPVAL);P_(EXPRES);NL();
                }
                LOOP_ASSERT(i, EXPVAL == Obj::getInt64(&X));
                LOOP_ASSERT(i, EXPRES == result );
            }
        }
        if (verbose) cout << "\nTesting 'Pointer' SWAP Manipulators" << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                void *d_value;       // Initial value
                void *d_swapValue;   // Swap value
            } VALUES[] = {
                //line value swap
                //---- ----- -------
                { L_,   (void*)0   , (void*)11     },
                { L_,   (void*)1   , (void*)19     },
                { L_,  (void*)-1   , (void*)4      },
                { L_,  (void*) 2   , (void*)-4     },
                { L_,  (void*)-2   , (void*)16     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                void *VAL    = VALUES[i].d_value;
                void *SWPVAL = VALUES[i].d_swapValue;
                void *result = 0;

                Types::Pointer x;
                const Types::Pointer& X = x;

                Obj::initPointer(&x,0);
                ASSERT(0 == Obj::getPtr(&X));

                Obj::setPtr(&x,VAL);
                result = Obj::swapPtr(&x,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getPtr(&X)); P_(VAL);P_(SWPVAL);NL();
                }
                LOOP3_ASSERT(i, SWPVAL, Obj::getPtr(&X),
                             SWPVAL == Obj::getPtr(&X));
                LOOP_ASSERT(i, VAL    == result );
            }
        }

        {
            static const struct {
                int  d_lineNum;      // Source line number
                void *d_value;       // Initial value
                void *d_swapValue;   // Swap value
                void *d_cmpValue;    // Compare value
                void *d_expValue;    // Expected value after the operations
                void *d_expResult;   // Expected result
            } VALUES[] = {
                //line value        swapVal             cmpVal
                //---- ------------ ------------------- --------------
                //    expValue             expResult
                //    -------------------- ----------------
                { L_, (void*) 0   , (void*)11         , (void*)33     ,
                      (void*)0          , (void*)0       },
                { L_, (void*) 1   , (void*)19         , (void*)1      ,
                      (void*)19         , (void*)1       },
                { L_, (void*)-1   , (void*)4          , (void*)1      ,
                      (void*)-1         , (void*)-1      },
                { L_, (void*) 2   , (void*)0xFFFFFFFF , (void*)2      ,
                      (void*)0xFFFFFFFF , (void*)2       },
                { L_, (void*)-2   , (void*)16         , (void*)0      ,
                      (void*)-2         , (void*)-2      }
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                void *VAL    = VALUES[i].d_value;
                void *CMPVAL = VALUES[i].d_cmpValue;
                void *SWPVAL = VALUES[i].d_swapValue;
                void *EXPVAL = VALUES[i].d_expValue;
                void *EXPRES = VALUES[i].d_expResult;
                void *result = 0;

                Types::Pointer x;
                const Types::Pointer& X = x;
                Obj::initPointer(&x,0);
                ASSERT(0 == Obj::getPtr(&X));

                Obj::setPtr(&x,VAL);
                result = Obj::testAndSwapPtr(&x,CMPVAL,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(Obj::getPtr(&X));
                    P_(VAL);P_(CMPVAL);P_(SWPVAL);
                    P_(EXPVAL);P_(EXPRES);NL();
                }
                LOOP_ASSERT(i, EXPVAL == Obj::getPtr(&X));
                LOOP_ASSERT(i, EXPRES == result );
            }
        }

        if (verbose) cout << "\nTesting 'Int' swap Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=NTHREADS*NITERATIONS + NITERATIONS;

            Types::Int mInt;

            IntSwapTestThreadArgs args;
            Obj::initInt(&mInt,INT_SWAPTEST_VALUE1);

            args.d_int_p        = &mInt;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i], swapIntTestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;

            for (int i=0; i < NITERATIONS; ++i) {
                int oldValue = Obj::swapInt( &mInt,
                                                         INT_SWAPTEST_VALUE1);
                if (oldValue == INT_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == INT_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }

            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);

            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total); P(args.d_value1Count);
                T_(); P_(args.d_value2Count); P(args.d_errorCount);
            }
        }

        if (verbose) cout << "\nTesting 'Int64' swap Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=NTHREADS*NITERATIONS+NITERATIONS;
            Types::Int64 mInt;

            Int64SwapTestThreadArgs args;
            Obj::initInt64(&mInt,INT64_SWAPTEST_VALUE1);

            args.d_int_p        = &mInt;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i], swapInt64TestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;
            for (int i=0; i < NITERATIONS; ++i) {
                bsls::Types::Int64 oldValue =
                    Obj::swapInt64( &mInt,
                                                INT64_SWAPTEST_VALUE1);
                if (oldValue == INT64_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == INT64_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);

            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total);
                T_(); P_(args.d_value1Count); P_(args.d_value2Count);
                P(args.d_errorCount);
            }
        }

        if (verbose) cout << "\nTesting 'Int' testAndSwap Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=NTHREADS*NITERATIONS+NITERATIONS;
            Types::Int mInt;

            IntSwapTestThreadArgs args;
            Obj::initInt(&mInt,INT_SWAPTEST_VALUE1);

            args.d_int_p        = &mInt;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i], testAndSwapIntTestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;
            for (int i=0; i < NITERATIONS; ++i) {
                int oldValue = Obj::swapInt(&mInt,
                                                        INT_SWAPTEST_VALUE1);
                if (oldValue == INT_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == INT_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);


            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total); P(args.d_value1Count);
                T_(); P_(args.d_value2Count); P(args.d_errorCount);
            }
        }

        if (verbose) cout << "\nTesting 'Int64' testAndSwap Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=NTHREADS*NITERATIONS+NITERATIONS;
            Types::Int64 mInt;

            Int64SwapTestThreadArgs args;
            Obj::initInt64(&mInt,INT64_SWAPTEST_VALUE1);

            args.d_int_p        = &mInt;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i], testAndSwapInt64TestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;
            for (int i=0; i < NITERATIONS; ++i) {
                bsls::Types::Int64 oldValue =
                    Obj::swapInt64( &mInt,
                                                INT64_SWAPTEST_VALUE1);
                if (oldValue == INT64_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == INT64_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);

            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total);
                T_(); P_(args.d_value1Count); P_(args.d_value2Count);
                P(args.d_errorCount);
            }
        }

        if (verbose) cout << "\nTesting 'Pointer' swap Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=NTHREADS*NITERATIONS+NITERATIONS;
            Types::Pointer mPtr;

            PointerTestThreadArgs args;
            Obj::initPointer(&mPtr,POINTER_SWAPTEST_VALUE1);

            args.d_ptr_p        = &mPtr;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i], swapPtrTestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;
            for (int i=0; i < NITERATIONS; ++i) {
                void* oldValue =
                    Obj::swapPtr(&mPtr, POINTER_SWAPTEST_VALUE1);
                if (oldValue == POINTER_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == POINTER_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);

            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total);
                T_(); P_(args.d_value1Count); P_(args.d_value2Count);
                P(args.d_errorCount);
            }
        }

        if (verbose) cout << "\nTesting 'Pointer' testAndSwap Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int EXPTOTAL=NTHREADS*NITERATIONS+NITERATIONS;
            Types::Pointer mPtr;

            PointerTestThreadArgs args;
            Obj::initPointer(&mPtr,POINTER_SWAPTEST_VALUE1);

            args.d_ptr_p        = &mPtr;
            args.d_iterations   = NITERATIONS;
            args.d_countStarted = 0;
            args.d_value1Count  = 0;
            args.d_value2Count  = 0;
            args.d_errorCount   = 0;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread(&threadHandles[i], testAndSwapPtrTestThread,
                               &args);
                args.d_startSig.wait();
            }
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            int errorCount=0;
            int value1Count=0;
            int value2Count=0;
            for (int i=0; i < NITERATIONS; ++i) {
                void* oldValue =
                    Obj::swapPtr(&mPtr, POINTER_SWAPTEST_VALUE1);
                if (oldValue == POINTER_SWAPTEST_VALUE1) ++value1Count;
                else if(oldValue == POINTER_SWAPTEST_VALUE2) ++value2Count;
                else ++errorCount;

            }
            for (int i=0; i < NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            args.d_errorCount += errorCount;
            args.d_value1Count += value1Count;
            args.d_value2Count += value2Count;

            ASSERT(0 == args.d_errorCount);
            int total = args.d_value1Count + args.d_value2Count;
            ASSERT(EXPTOTAL == total);

            if (veryVerbose) {
                T_(); P_(EXPTOTAL); P(total);
                T_(); P_(args.d_value1Count); P_(args.d_value2Count);
                P(args.d_errorCount);
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ARITHMETIC MANIPULATORS
        //   Test that the 32/64 bit integer add functions work as expected.
        // Plan:
        //   For each atomic type('Int', and 'Int64') using a sequence of
        //   independent values, begin by initializing the value to 0
        //   and adding the test value.  Assert the resulting
        //   value is the expected value.  Repeat the operation using
        //   the "Nv" operation and assert the both the resulting
        //   value of the object and the return value from the
        //   operation are both the expected value.  Next, initialize
        //   each object to a base value add the delta value.  Assert
        //   that the result are still correct.  Repeat this step for
        //   the "Nv" operation and assert that the value return value
        //   and resulting object values are correct.  Finally create
        //   a series of threads each of which will concurrently add a
        //   specific value to a common instance.  If the final value
        //   is the expected value, then the operations are assumed to
        //   behave correctly when concurrently accessed by multiple
        //   threads.
        // Testing:
        //   addInt(Obj::Int *aInt, int value);
        //   addInt64(Obj::Int64 *, bsls::Types::Int64);
        //   addIntNv(Obj::Int *aInt, int value);
        //   addInt64Nv(Obj::Int64 *, bsls::Types::Int64);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Arithmetic Manipulators"
                          << "\n========================================="
                          << endl;

        if (verbose) cout << "\nTesting 'Int' Arithmetic Manipulators" << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                int d_value;       // Input value
            } VALUES[] = {
                //line d_x
                //---- ----
                { L_,   0   },
                { L_,   1   },
                { L_,  -1   },
                { L_,   2   },
                { L_,  -2   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof VALUES[0];

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                Obj::addInt(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X)); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(&X));
            }

            if (verbose) cout <<
                "\n\tTesting 'Int' Arithmetic(and values) Manipulators\n" <<
                "\n\t-------------------------------------------------"
                              << endl;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                int       result;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                result = Obj::addIntNv(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X)); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(&X));
                LOOP_ASSERT(i, VAL == result);
            }

        }

        if (verbose) cout << "\n\tTesting 'Int' Arith(with base) Manip"
                          << endl;

        {
            static const struct {
                int  d_lineNum;    // Source line number
                int d_base;        // Base value
                int d_amount;      // Amount to add
                int d_expected;    // Expected value
            } VALUES[] = {
                //line d_base          d_amount d_expected
                //---- --------        -------- ----------
                { L_,   0             , -9    , -9         },
                { L_,   1             , 0     , 1          },
                { L_,  -1             , 1     , 0          },
                { L_, (int) 0xFFFFFFFF, 1     , 0          },
                { L_,  -2             , -2    , -4         }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int BASE = VALUES[i].d_base;
                const int AMT  = VALUES[i].d_amount;
                const int EXP  = VALUES[i].d_expected;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                Obj::setInt(&x,BASE);
                ASSERT(BASE == Obj::getInt(&X));

                Obj::addInt(&x,AMT);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X));
                    P_(BASE); P_(AMT); P_(EXP); NL();
                }
                LOOP_ASSERT(i, EXP == Obj::getInt(&X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int BASE = VALUES[i].d_base;
                const int AMT  = VALUES[i].d_amount;
                const int EXP  = VALUES[i].d_expected;
                int       result;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                Obj::setInt(&x,BASE);
                ASSERT(BASE == Obj::getInt(&X));

                result = Obj::addIntNv(&x,AMT);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X));
                    P_(BASE); P_(AMT); P_(EXP); P_(result); NL();
                }
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == Obj::getInt(&X));
            }

        }

        if (verbose) cout << "\nTesting 'Int64' Arithmetic Manipulators"
                          << endl;
        {
            static const struct {
                int       d_lineNum;     // Source line number
                bsls::Types::Int64 d_value;       // Input value
            } VALUES[] = {
                //line d_x
                //---- ----
                { L_,   0   },
                { L_,   1   },
                { L_,  -1LL   },
                { L_,   2   },
                { L_,  -2LL   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 VAL  = VALUES[i].d_value;

                Types::Int64 x;  const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                Obj::addInt64(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X)); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt64(&X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 VAL  = VALUES[i].d_value;
                bsls::Types::Int64       result;

                Types::Int64 x;  const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                result = Obj::addInt64Nv(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X));
                    P_(VAL); P_(result); NL();
                }
                LOOP_ASSERT(i, VAL == result);
                LOOP_ASSERT(i, VAL == Obj::getInt64(&X));
            }

        }
        {
            static const struct {
                int  d_lineNum;      // Source line number
                bsls::Types::Int64 d_base;    // Base value
                bsls::Types::Int64 d_amount;  // Amount to add
                bsls::Types::Int64 d_expected; // Expected value
            } VALUES[] = {
                //line d_base        d_amount d_expected
                //---- ------------- -------- ----------
                { L_,  -1LL         , 10      , 9                    },
                { L_,  1            , -2LL    , -1LL                 },
                { L_,  -1LL         , 2LL     , 1LL                  },
                { L_,  0xFFFFFFFFLL , 1LL    , 0x100000000LL        },
                { L_,  0x100000000LL, -2LL    , 0xFFFFFFFELL         }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 BASE = VALUES[i].d_base;
                const bsls::Types::Int64 AMT  = VALUES[i].d_amount;
                const bsls::Types::Int64 EXP  = VALUES[i].d_expected;

                Types::Int64 x;  const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                Obj::setInt64(&x,BASE);
                ASSERT(BASE == Obj::getInt64(&X));

                Obj::addInt64(&x,AMT);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X)); P(BASE);
                    T_(); P_(AMT); P(EXP);
                }
                LOOP_ASSERT(i, EXP == Obj::getInt64(&X));
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 BASE = VALUES[i].d_base;
                const bsls::Types::Int64 AMT  = VALUES[i].d_amount;
                const bsls::Types::Int64 EXP  = VALUES[i].d_expected;
                bsls::Types::Int64       result;

                Types::Int64 x;  const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                Obj::setInt64(&x,BASE);
                ASSERT(BASE == Obj::getInt64(&X));

                result = Obj::addInt64Nv(&x,AMT);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X)); P(BASE);
                    T_(); P_(AMT); P(EXP); NL();
                }
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == Obj::getInt64(&X));
            }

        }
        if (verbose) cout << "\nTesting 'Int' add Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int ADDVAL = 3;
            const int EXPTOTAL=NTHREADS*NITERATIONS*ADDVAL*2;
            const int STARTVALUE=0;

            Types::Int mInt;

            Obj::initInt(&mInt,STARTVALUE);
            IntTestThreadArgs args;
            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = ADDVAL;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i],addIntTestThread, &args);
                args.d_startSig.wait();
            }

            ASSERT(STARTVALUE == Obj::getInt(&mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt(&mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt(&mInt)); P(EXPTOTAL);
                T_(); P(STARTVALUE); NL();
            }
        }
        if (verbose) cout << "\nTesting 'Int64' add Thread Safeness"
                          << endl;
        {
            const int NTHREADS=4;
            const int NITERATIONS=10000;
            const int ADDVAL = 33;
            const bsls::Types::Int64 STARTVALUE=0xfffff000;
            const bsls::Types::Int64 EXPTOTAL=(NTHREADS*NITERATIONS*
                                                     ADDVAL * 2) + STARTVALUE;
            Types::Int64 mInt;

            Int64TestThreadArgs args;
            Obj::initInt64(&mInt,STARTVALUE);

            args.d_int_p = &mInt;
            args.d_iterations = NITERATIONS;
            args.d_countStarted = 0;
            args.d_addVal = ADDVAL;

            my_thread_t threadHandles[NTHREADS];

            args.d_barrier.reset();
            for (int i=0; i < NTHREADS; ++i) {
                args.d_startSig.reset();
                myCreateThread( &threadHandles[i], addInt64TestThread,
                                &args);
                args.d_startSig.wait();
            }
            ASSERT(STARTVALUE == Obj::getInt64(&mInt));
            ASSERT(NTHREADS == args.d_countStarted);
            args.d_barrier.signal();

            for (int i=0; i< NTHREADS; ++i) {
                myJoinThread(threadHandles[i]);
            }
            ASSERT(EXPTOTAL == Obj::getInt64(&mInt));
            if (veryVerbose) {
                T_(); P_(Obj::getInt64(&mInt)); P(EXPTOTAL);
                T_(); P(STARTVALUE);
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //
        // Plan:
        //   First, verify the initialization functions by initializing each
        //   atomic type and testing the resulting value.
        //
        //   Next, for the Int,Int64, and Pointer atomic types, for a sequence
        //   of independent test values, use the initialization functions to
        //   initialize each type and use the primary manipulator(i.e.,
        //   'setInt', 'setInt64', 'setPtr') to set its value.  Verify the
        //   value using the respective direct accessor(i.e.,'getInt',
        //   'getInt64', 'getPtr').
        //
        // Testing:
        //   initInt(Obj::Int *aInt, int initialValue);
        //   initInt64(Obj::Int64 *aInt);
        //   initPointer(Obj::Pointer *aPointer);
        //   setInt(Obj::Int *aInt, int value);
        //   getInt(const Obj::Int &aInt);
        //   setInt64(Obj::Int64 *, bsls::Types::Int64);
        //   getInt64(const Obj::Int64 &aInt);
        //   getPtr(const Obj::Pointer &aPointer);
        //   setPtr(Obj::Pointer *aPointer, void *value);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulators"
                          << "\n============================" << endl;

        if (verbose) cout << "\nTesting 'Int' Primary Manipulators" << endl;
        {
            static const struct {
                int  d_lineNum;     // Source line number
                int d_value;       // Input value
            } VALUES[] = {
                //line value
                //---- ----
                { L_,   0   },
                { L_,   1   },
                { L_,  -1   },
                { L_,   2   },
                { L_,  -2   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;

                Types::Int x;  const Types::Int& X = x;
                Obj::initInt(&x,0);
                ASSERT(0 == Obj::getInt(&X));

                Obj::setInt(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt(&X)); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt(&X));
            }
        }

        if (verbose) cout << "\nTesting 'Int64' Primary Manipulators" << endl;
        {
            static const struct {
                int       d_lineNum;     // Source line number
                bsls::Types::Int64 d_value;       // Input value
            } VALUES[] = {
                //line value
                //---- -----
                { L_,   0              },
                { L_,   1              },
                { L_,  -1              },
                { L_,  0xFFFFFFFFLL    },
                { L_,  0x100000000LL   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls::Types::Int64 VAL  = VALUES[i].d_value;

                Types::Int64 x;  const Types::Int64& X = x;
                Obj::initInt64(&x,0);
                ASSERT(0 == Obj::getInt64(&X));

                Obj::setInt64(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getInt64(&X)); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == Obj::getInt64(&X));
            }
        }


        if (verbose) cout << "\nTesting 'Pointer' Primary Manipulators"
                           << endl;
        {
            static const struct {
                int    d_lineNum;     // Source line number
                void  *d_value;       // Input value
            } VALUES[] = {
                //line value
                //---- ----
                { L_,  (void*)0x0        },
                { L_,  (void*)0x1        },
                { L_,  (void*)0xffff     },
                { L_,  (void*)0xffff8888 },
                { L_,  (void*)0xffffffff }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                void* VAL  = VALUES[i].d_value;

                Types::Pointer x;
                const Types::Pointer& X = x;

                Obj::initPointer(&x,0);
                ASSERT(0 == Obj::getPtr(&X));

                Obj::setPtr(&x,VAL);
                if (veryVerbose) {
                    T_(); P_(Obj::getPtr(&X)); P_(VAL);
                }
                LOOP_ASSERT(i, VAL == Obj::getPtr(&X));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

          if (verbose) cout << "\nBREATHING TEST"
                            << "\n==============" << endl;

        const int XVA=-1;
        const int XVB=2;
        const int XVC=-2;

        const bsls::Types::Int64 YVA=1LL;
        const bsls::Types::Int64 YVB=-1LL;
        const bsls::Types::Int64 YVC=0xFFFFFFFFLL;

        void *PVA=(void*)0xffff8888;
        void *PVB=(void*)0xffffffff;
        void *PVC=(void*)0x78888888;

        int lresult;
        bsls::Types::Int64 llresult;
        void *presult;

        if (veryVerbose) cout << endl
                              << "\tInt" << endl
                              << "\t---" << endl;

        Types::Int mX1;

        Obj::initInt(&mX1,7654);
        ASSERT(7654 == Obj::getInt(&mX1));

        Obj::setInt(&mX1,XVA);
        ASSERT(XVA == Obj::getInt(&mX1));

        Obj::setInt(&mX1,0);
        ASSERT(0 == Obj::getInt(&mX1));

        Obj::incrementInt(&mX1);
        ASSERT(1 == Obj::getInt(&mX1));

        Obj::decrementInt(&mX1);
        ASSERT(0 == Obj::getInt(&mX1));

        lresult = Obj::incrementIntNv(&mX1);
        ASSERT(1 == lresult);
        ASSERT(1 == Obj::getInt(&mX1));
        if ( veryVeryVerbose) {
            T_(); P(lresult); NL();
        }

        Obj::setIntRelaxed(&mX1,2);
        ASSERT(2 == Obj::getIntRelaxed(&mX1));

        ASSERT(-4 == Obj::addIntNvRelaxed(&mX1, -6));
        ASSERT(-4 == Obj::getIntRelaxed(&mX1));

        Obj::decrementIntNv(&mX1); Obj::decrementIntNv(&mX1);
        lresult = Obj::decrementIntNv(&mX1);
        ASSERT(-7 == lresult);
        ASSERT(-7 == Obj::getInt(&mX1));

        Obj::setInt(&mX1,0);
        ASSERT(0 == Obj::getInt(&mX1));

        lresult = Obj::addIntNv(&mX1,XVB);
        ASSERT(XVB == Obj::getInt(&mX1));

        Obj::setInt(&mX1,0);
        ASSERT(0 == Obj::getInt(&mX1));

        lresult = Obj::swapInt(&mX1, XVC);
        ASSERT(0 == lresult);
        ASSERT(XVC == Obj::getInt(&mX1));

        lresult = Obj::testAndSwapInt(&mX1,XVA,XVB);
        ASSERT(XVC == lresult);
        ASSERT(XVC == Obj::getInt(&mX1));

        lresult = Obj::testAndSwapInt(&mX1,XVC,0);
        ASSERT(XVC == lresult);
        ASSERT(0 == Obj::getInt(&mX1));


        if (veryVerbose) cout << endl
                              << "\tInt64" << endl
                              << "\t-----" << endl;

        Types::Int64 mY1;

        Obj::initInt64(&mY1,0);
        ASSERT(0 == Obj::getInt64(&mY1));

        Obj::setInt64(&mY1,YVA);
        ASSERT(YVA == Obj::getInt64(&mY1));

        Obj::setInt64(&mY1,0);
        ASSERT(0 == Obj::getInt64(&mY1));

        Obj::incrementInt64(&mY1);
        ASSERT(1 == Obj::getInt64(&mY1));

        Obj::decrementInt64(&mY1);
        ASSERT(0 == Obj::getInt64(&mY1));

        llresult = Obj::incrementInt64Nv(&mY1);
        ASSERT(1 == llresult);
        ASSERT(1 == Obj::getInt64(&mY1));
        if ( veryVeryVerbose) {
            T_(); P(llresult); NL();
        }
        llresult = Obj::decrementInt64Nv(&mY1);
        ASSERT(0 == llresult);
        ASSERT(0 == Obj::getInt64(&mY1));

        Obj::setInt64(&mY1,0);
        ASSERT(0 == Obj::getInt64(&mY1));

        llresult = Obj::addInt64Nv(&mY1,YVB);
        ASSERT(YVB == Obj::getInt64(&mY1));

        Obj::setInt64(&mY1,0);
        ASSERT(0 == Obj::getInt64(&mY1));

        llresult = Obj::swapInt64(&mY1, YVC);
        ASSERT(0 == llresult);
        ASSERT(YVC == Obj::getInt64(&mY1));

        llresult = Obj::testAndSwapInt64(&mY1,YVA,YVB);
        ASSERT(YVC == llresult);
        ASSERT(YVC == Obj::getInt64(&mY1));

        llresult = Obj::testAndSwapInt64(&mY1,YVC,0);
        ASSERT(YVC == llresult);
        ASSERT(0 == Obj::getInt64(&mY1));


        // Pointers
        // --------

        if (veryVerbose) cout << endl
                              << "\tPointers" << endl
                              << "\t--------" << endl;
        Types::Pointer mP1;

        Obj::initPointer(&mP1,0);
        ASSERT(0 == Obj::getPtr(&mP1));

        presult = Obj::swapPtr(&mP1, PVC);
        ASSERT(0 == presult);
        LOOP2_ASSERT(PVC, Obj::getPtr(&mP1),
                     PVC == Obj::getPtr(&mP1));

        presult = Obj::testAndSwapPtr(&mP1,PVA,PVB);
        ASSERT(PVC == presult);
        ASSERT(PVC == Obj::getPtr(&mP1));

        presult = Obj::testAndSwapPtr(&mP1,PVC,0);
        ASSERT(PVC == presult);
        ASSERT(0 == Obj::getPtr(&mP1));

      } break;
    default:
        testStatus = -1;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
