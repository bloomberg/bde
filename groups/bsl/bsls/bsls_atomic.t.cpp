// bsls_atomic.t.cpp                                                  -*-C++-*-
#include <bsls_atomic.h>

#include <bsls_types.h>
#include <bsls_platform.h>

#include <stdlib.h>               // atoi(), rand()
#include <iostream>

// For thread support
#ifdef BSLS_PLATFORM_OS_WINDOWS
#   include <windows.h>

typedef HANDLE thread_t;

#else
#   include <pthread.h>
#   include <unistd.h>
#   include <sched.h>           // sched_yield

typedef pthread_t thread_t;

#endif

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// This component provides a set of wrapper classes for their respective
// atomic types and operations.  Since the provided classes are only proxies to
// the respective atomic operations, this test driver only verifies that each
// class and operation, is properly "hooked up" to its respective atomic type
// and operation.
//
//-----------------------------------------------------------------------------
// bsls::AtomicInt
// ---------------
// [ 2] bsls::AtomicInt();
// [ 3] bsls::AtomicInt(const bsls::AtomicInt& rhs);
// [ 3] bsls::AtomicInt(int value);
// [ 2] ~bsls::AtomicInt();
// [ 5] int swap(int swapValue);
// [ 5] int testAndSwap(int compareValue,int swapValue);
// [ 4] int add(int value);
// [ 6] int operator ++();
// [ 6] int operator ++(int);
// [ 6] int operator --();
// [ 6] int operator --(int);
// [ 3] bsls::AtomicInt& operator= (const bsls::AtomicInt& rhs);
// [ 2] bsls::AtomicInt& operator= (int value);
// [ 4] void operator +=(int value);
// [ 4] void operator -=(int value);
// [ 2] operator int() const;
//
// bsls::AtomicInt64
// -----------------
// [ 2] bsls::AtomicInt64();
// [ 3] bsls::AtomicInt64(const bsls::AtomicInt64& original);
// [ 3] bsls::AtomicInt64(bsls::Types::Int64 value);
// [ 2] ~bsls::AtomicInt64();
// [ 4] bsls::Types::Int64 add(bsls::Types::Int64 value);
// [ 5] bsls::Types::Int64 swap(bsls::Types::Int64 swapValue);
// [ 5] bsls::Types::Int64 testAndSwap(bsls::Types::Int64 ...
// [ 6] bsls::Types::Int64 operator ++();
// [ 6] bsls::Types::Int64 operator ++(int);
// [ 6] bsls::Types::Int64 operator --();
// [ 6] bsls::Types::Int64 operator --(int);
// [ 3] bsls::AtomicInt64& operator= (const bsls::AtomicInt64& rhs);
// [ 2] bsls::AtomicInt64& operator= (bsls::Types::Int64 value);
// [ 4] void operator +=(bsls::Types::Int64 value);
// [ 4] void operator -=(bsls::Types::Int64 value);
// [ 2] operator bsls::Types::Int64() const;
//
// bsls::AtomicPointer
// -------------------
// [ 2] bsls::AtomicPointer();
// [ 3] bsls::AtomicPointer(const bsls::AtomicPointer<T>& original);
// [ 3] bsls::AtomicPointer(const T* value);
// [ 2] ~bsls::AtomicPointer();
// [ 5] T* swap(const T* swapValue);
// [ 5] T* testAndSwap(const T* compareValue, const T* swapValue);
// [ 3] bsls::AtomicPointer<T>& operator= (const bsls::AtomicPointer<T>& rhs);
// [ 2] bsls::AtomicPointer<T>& operator= (const T *value);
// [ 2] T& operator*() const;
// [ 3] T* operator->() const;
// [ 2] operator T*() const;
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] SEQUENTIAL CONSISTENCY MEMORY ORDERING GUARANTEE TEST
// [ 8] ACQUIRE/RELEASE MEMORY ORDERING GUARANTEE TEST
// [ 9] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int verbose = 0;
int testStatus = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

# define ASSERT(X) do { aSsErT(!(X), #X, __LINE__); } while (0)

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
#define P64(X) printf(#X " = %lld\n", (X));   // Print 64-bit integer id & val
#define P64_(X) printf(#X " = %lld,  ", (X)); // Print 64-bit integer w/o '\n'

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

struct APTestObj
    // This structure is used to test Atomic Pointers the self method
    // returns the 'this' pointer of this object.  The method is used
    // to test 'operator ->'.
{
    const APTestObj* self() const { return this;}
        // Return a pointer to this object.
};

typedef bsls::AtomicInt                      AI;
typedef bsls::AtomicInt64                    AI64;
typedef bsls::AtomicPointer<APTestObj>       AP;
typedef bsls::AtomicPointer<const APTestObj> CAP;

typedef bsls::Types::Int64            Int64;

//=============================================================================
//                  HELPER CLASSES AND FUNCTIONS  FOR TESTING
//-----------------------------------------------------------------------------

namespace {

const int g_spinCount = 100;

void yield()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    SwitchToThread();
#else
    sched_yield();
#endif
}

template <class INT>
struct LockData
{
    INT flags[2];
    INT turn;     // 0 or 1 - index into d_flags
};

template <class INT>
class PetersonsLock
    // PetersonsLock class implements the Peterson's locking algorithms for two
    // concurrently executing threads, using atomic operations on integers with
    // acquire/release/relaxed memory ordering semantics.
    //
    // For the analysis of this algorithm see:
    // http://www.justsoftwaresolutions.co.uk/threading/
    //                                   petersons_lock_with_C++0x_atomics.html
{
public:
    PetersonsLock(int id, LockData<INT>& lockData)
        : d_id(id)
        , d_data(lockData)
    {
        ASSERT(id == 0 || id == 1);
    }

    void lock()
    {
        d().flags[d_id].storeRelaxed(1);
        d().turn.swapAcqRel(1 - d_id);

        int spin = g_spinCount;

        while (d().flags[1 - d_id].loadAcquire()
            && d().turn.loadAcquire() == 1 - d_id)
            // Contrary to justsoftwaresolutions 'turn' load needs to be
            // 'loadAcquire'.
        {
            if (--spin == 0) {
                yield();
                spin = g_spinCount;
            }
        }
    }

    void unlock()
    {
        d().flags[d_id].storeRelease(0);
    }

private:
    LockData<INT>& d() const
    {
        return d_data;
    }

private:
    int             d_id;   // 0 or 1 - id of the thread that owns this object
    LockData<INT>&  d_data;
};

template <class INT>
class PetersonsLockSeqCst
    // PetersonsLock class implements the Peterson's locking algorithms for two
    // concurrently executing threads, using atomic operations on integers with
    // sequential consistency memory ordering semantics.
{
public:
    PetersonsLockSeqCst(int id, LockData<INT>& lockData)
        : d_id(id)
        , d_data(lockData)
    {
        ASSERT(id == 0 || id == 1);
    }

    void lock()
    {
        d().flags[d_id] = 1;
        d().turn.swap(1 - d_id);

        int spin = g_spinCount;

        while (d().flags[1 - d_id] && d().turn == 1 - d_id)
        {
            if (--spin == 0) {
                yield();
                spin = g_spinCount;
            }
        }
    }

    void unlock()
    {
        d().flags[d_id] = 0;
    }

private:
    LockData<INT>& d() const
    {
        return d_data;
    }

private:
    int             d_id;   // 0 or 1 - id of the thread that owns this object
    LockData<INT>&  d_data;
};

template <class LOCK>
struct Guard
{
    Guard(LOCK& lock)
        : d_lock(lock)
    {
        d_lock.lock();
    }

    ~Guard()
    {
        d_lock.unlock();
    }

    LOCK& d_lock;
};

bsls::AtomicInt s_data1(0);
bsls::AtomicInt s_data2(0);
bsls::AtomicInt s_data3(1);

template <class LOCK>
void testAtomicLocking(LOCK& lock, int iterations)
    // Test 'lock' implemented using atomic operations by using it to protect
    // some shared data, which is both read and written to.
{
    for (int i = 0; i < iterations; ++i) {
        if (rand() & 1) {
            // read shared data
            Guard<LOCK> guard(lock);

            int data1 = s_data1.loadRelaxed();
            int data2 = s_data2.loadRelaxed();
            int data3 = s_data3.loadRelaxed();

            ASSERT(data1 == -data2 && data1 + 1 == data3);
        }
        else {
            // write to shared data
            Guard<LOCK> guard(lock);

            int data = rand();
            s_data1.storeRelaxed(data);
            s_data2.storeRelaxed(-data);
            s_data3.storeRelaxed(data + 1);
        }
    }
}

template <class LOCK>
struct AtomicLockingThreadParam
{
    AtomicLockingThreadParam(LOCK& lock, int iterations)
        : d_lock(lock)
        , d_iterations(iterations)
    {}

    LOCK&   d_lock;
    int     d_iterations;
};

template <class LOCK>
void *testAtomicLockingThreadFunc(void *arg)
{
    AtomicLockingThreadParam<LOCK> *param
        = reinterpret_cast<AtomicLockingThreadParam<LOCK> *>(arg);

    testAtomicLocking(param->d_lock, param->d_iterations);

    return 0;
}


typedef void *(*thread_func)(void *arg);

thread_t createThread(thread_func func, void *arg)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return CreateThread(0, 0, (LPTHREAD_START_ROUTINE) func, arg, 0, 0);
#else
    thread_t thr;
    pthread_create(&thr, 0, func, arg);
    return thr;
#endif
}

void joinThread(thread_t thr)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    WaitForSingleObject(thr, INFINITE);
    CloseHandle(thr);
#else
    pthread_join(thr, 0);
#endif
}

void sleepSeconds(int sec)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    Sleep(sec * 1000);
#else
    sleep(sec);
#endif
}


struct TestLoopParameters
{
    typedef void (*TestFunc)(TestLoopParameters *);

    bsls::AtomicInt d_cancel;
    bsls::AtomicInt d_iterations;
    TestFunc        d_testFunc;
};

void petersonsLockLoopTest(TestLoopParameters *params)
{
    LockData<bsls::AtomicInt64> ld;
    PetersonsLockSeqCst<bsls::AtomicInt64> lock(0, ld);

    for (; !params->d_cancel; ++params->d_iterations) {
        Guard<PetersonsLockSeqCst<bsls::AtomicInt64> > guard(lock);
    }
}

void sharedCountLoopTest(TestLoopParameters *params)
{
    bsls::AtomicInt count;

    for (; !params->d_cancel; ++params->d_iterations) {
        count.storeRelease(10);

        while (count.loadAcquire() != 0) {
            count.addAcqRel(-1);
        }
    }
}

void *runTestingLoop(void *arg)
    // Run a simulation of the memory ordering test case to determine the
    // number of iterations for the real test.
{
    TestLoopParameters *params = static_cast<TestLoopParameters *>(arg);
    params->d_testFunc(params);

    return 0;
}

void *runObserverLoop(void *arg)
    // Observe changes in shared state for more accurate memory ordering test
    // simulation.
{
    TestLoopParameters *params = static_cast<TestLoopParameters *>(arg);

    while (!params->d_cancel && params->d_iterations >= 0)
    {
    }

    return 0;
}

int getTestCaseIterations(TestLoopParameters::TestFunc testFunc)
    // Return a reasonable experimentally determined number of iterations for
    // the memory ordering test case.
{
    TestLoopParameters params;
    params.d_testFunc = testFunc;
    thread_t loopThr = createThread(&runTestingLoop, &params);
    thread_t obsvThr = createThread(&runObserverLoop, &params);

    sleepSeconds(5);  // this makes the real test run for a couple of minutes

    params.d_cancel = 1;
    joinThread(loopThr);
    joinThread(obsvThr);

    return params.d_iterations;
}


template <template <class> class LOCK, class INT>
void testCaseMemOrder(int iterations)
{
    LockData<INT> lockData;
    LOCK<INT> lock0(0, lockData);
    LOCK<INT> lock1(1, lockData);

    AtomicLockingThreadParam<LOCK<INT> > param0(lock0, iterations);
    AtomicLockingThreadParam<LOCK<INT> > param1(lock1, iterations);

    thread_t thr0 =
               createThread(&testAtomicLockingThreadFunc<LOCK<INT> >, &param0);
    thread_t thr1 =
               createThread(&testAtomicLockingThreadFunc<LOCK<INT> >, &param1);

    joinThread(thr0);
    joinThread(thr1);
}


void testSharedCountWrite(int& data,
                          bsls::AtomicInt& shared,
                          bsls::AtomicInt& done,
                          int)
{
    int spin = g_spinCount;

    while (!done.loadRelaxed()) {
        while (shared.loadRelaxed() > 1) {
            const_cast<int volatile &>(data) = shared.loadRelaxed();
            shared.addAcqRel(-1);           // plays the role of store release
        }

        if (--spin == 0) {
            yield();
            spin = g_spinCount;
        }
    }
}

void testSharedCountRead(int& data,
                         bsls::AtomicInt& shared,
                         bsls::AtomicInt& done,
                         int iterations)
{
    for (int i = 0; i < iterations; ++i) {
        shared.storeRelease(10);

        int spin = g_spinCount;

        while (shared.loadRelaxed() != 1) {
            if (--spin == 0) {
                yield();
                spin = g_spinCount;
            }
        }

        if (shared.addAcqRel(-1) == 0) {    // plays the role of load acquire
            ASSERT(const_cast<int volatile &>(data) == 2);
        }
        else {
            ASSERT(false && "logic error in test");
        }
    }

    done.storeRelaxed(1);
}

struct SharedCountThreadParam
{
    typedef void (*ThreadFunc)(int&, bsls::AtomicInt&, bsls::AtomicInt&, int);

    SharedCountThreadParam(int& data,
                           bsls::AtomicInt& shared,
                           bsls::AtomicInt& done,
                           ThreadFunc func,
                           int iterations)
        : d_data(data)
        , d_shared(shared)
        , d_done(done)
        , d_func(func)
        , d_iterations(iterations)
    {}

    int&             d_data;
    bsls::AtomicInt& d_shared;
    bsls::AtomicInt& d_done;
    ThreadFunc       d_func;
    int              d_iterations;
};

void *testSharedCountThreadFunc(void *arg)
{
    SharedCountThreadParam *param
        = reinterpret_cast<SharedCountThreadParam *>(arg);

    param->d_func(param->d_data,
                  param->d_shared,
                  param->d_done,
                  param->d_iterations);
    return 0;
}

void testCaseSharedPtr(int iterations)
{
    bsls::AtomicInt shared;
    bsls::AtomicInt done;
    int data = 0;

    SharedCountThreadParam paramReader(data, shared, done,
                                       &testSharedCountRead,
                                       iterations);
    thread_t thrReader = createThread(&testSharedCountThreadFunc,
                                      &paramReader);

    SharedCountThreadParam paramWriter(data, shared, done,
                                       &testSharedCountWrite,
                                       iterations);
    thread_t thrWriter = createThread(&testSharedCountThreadFunc,
                                      &paramWriter);

    joinThread(thrReader);
    joinThread(thrWriter);
}

}  // close unnamed namespace

//=============================================================================
//                       USAGE EXAMPLES FROM HEADER
//-----------------------------------------------------------------------------

// EXAMPLE 1

int processNextTransaction()
    // Dummy implementation.
{
    return 0;
}

void createWorkerThread()
    // Dummy implementation.
{
}

void waitAllThreads()
    // Dummy implementation.
{
}

static bsls::AtomicInt64 transactionCount;
static bsls::AtomicInt64 successCount;
static bsls::AtomicInt64 failureCount;

void serverMain()
{
    const int num_threads = 10;
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
/// -  -  -  -  -  -  -  -  -
// First, we define class 'my_CountedHandleRep'.  This class manages a single
// 'INSTANCE' object on behalf of multiple "handle" objects; since different
// "handle" objects may be active in different threads, class
// 'my_CountedHandleRep' must be (fully) thread-safe.  Specifically, methods
// 'increment' and 'decrement' must work atomically.
//
// The class declaration for 'my_CountedHandleRep' is identical to the same
// class in component 'bsls_atomicoperations', with a single exception: member
// 'd_count' is of type 'bsls::AtomicInt', rather than
// 'bsls::AtomicOperations::Int'.  Whereas 'bsls::AtomicOperations::Int' is
// merely a 'typedef' for a platform-specific data type to be used in atomic
// integer operations, 'bsls::AtomicInt' encapsulates those atomic operations
// as member functions and operator overloads.  Class 'my_CountedHandleRep'
// will benefit from this encapsulation: Its method implementations will be
// able to operate on 'd_count' as if it were a standard integer.
//
// Note that, as in the example in component 'bsls_atomicoperations', this rep
// class is intended to be used only by class 'my_CountedHandle', and thus all
// methods of class 'my_CountedHandleRep' are declared private, and 'friend'
// status is granted to class 'my_CountedHandle':
//..
                        // =========================
                        // class my_CountedHandleRep
                        // =========================

template <class INSTANCE>
class my_CountedHandle;

template <class INSTANCE>
class my_CountedHandleRep {

    // DATA
    INSTANCE        *d_instance_p;   // address of managed instance
    bsls::AtomicInt  d_count;        // number of active references

    // FRIENDS
    friend class my_CountedHandle<INSTANCE>;

    // NOT IMPLEMENTED
    my_CountedHandleRep(const my_CountedHandleRep&);
    my_CountedHandleRep& operator=(const my_CountedHandleRep&);

  private:
    // PRIVATE CLASS METHODS
    static void
    deleteObject(my_CountedHandleRep<INSTANCE> *object);

    // PRIVATE CREATORS
    my_CountedHandleRep(INSTANCE *instance);
    ~my_CountedHandleRep();

    // PRIVATE MANIPULATORS
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
// "rep" object when its reference count is reduced to zero.
//
// Similar to 'my_CountedHandleRep', the class declaration for
// 'my_CountedHandle' is identical to that in 'bsls_atomicoperations':
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
    my_CountedHandle();
    my_CountedHandle(INSTANCE        *instance);

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
// Then, we define the constructor for the 'my_CountedHandleRep<INSTANCE>'
// class.  Member 'd_count' is initialized to 1, reflecting the fact that this
// constructor will be called by a new instance of 'my_CountedHandle', which
// instance is our first and only handle when this constructor is called:
// notice that 'd_count' (of type 'bsls::AtomicInt') is initialized as if it
// were a simple integer; its constructor guarantees that the initialization is
// done atomically.
//..
template <class INSTANCE>
inline
my_CountedHandleRep<INSTANCE>:: my_CountedHandleRep(INSTANCE *instance)
: d_instance_p(instance)
, d_count(1)
{
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
// Next, we define method 'increment', which is called by 'my_CountedHandle'
// to add a new reference to the current "rep" object, which simply increments
// 'd_count', using the prefix 'operator++':
//..
// MANIPULATORS
template <class INSTANCE>
inline
void my_CountedHandleRep<INSTANCE>::increment()
{
    ++d_count;
}
//..
// The above operation must be done atomically in a multi-threaded context;
// class 'bsls::AtomicInt' provides this guarantee for all its overloaded
// operators, and 'my_CountedHandleRep' relies upon this guarantee.
//
// Then, we implement method 'decrement', which is called by 'my_CountedHandle'
// when a reference to the current "rep" object is being deleted:
//..
template <class INSTANCE>
inline
int my_CountedHandleRep<INSTANCE>::decrement()
{
    return --d_count;
}
//..
// This method atomically decrements the number of references to this
// 'my_CountedHandleRep' and, once again, atomicity is guaranteed by the
// underlying type of 'd_count'.
//
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

// CREATORS
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
// ACCESSORS
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
    return d_rep_p ? d_rep_p->d_count : 0;
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
// This example parallels the third usage example given for component
// 'bsls_atomicoperations', presenting a different implementation of
// 'my_PtrStack<T>', with an identical public interface.  Note that, where the
// 'bsls_atomicoperations' example uses the basic data type
// 'bsls::AtomicOperations::AtomicTypes::Pointer' for members 'd_list' and
// 'd_freeList', this implementation uses instead the higher-level type
// 'bsls::AtomicPointer<T>'.
//
// First, we create class template, 'my_PtrStack', parameterized by 'TYPE'.
// Instances of this template maintain a list of nodes and a free-node list.
// Each node has a pointer to a data item, 'd_item_p', a link to the next node
// in the list, 'd_next_p' and an atomic flag, 'd_inUseFlag', intended for
// lock-free list manipulation.  The definition of the 'my_PtrStack' class is
// provided below:
//..
template <class TYPE>
class my_PtrStack {
    // TYPES
    struct Node {
        TYPE                 *d_item_p;
        Node                 *d_next_p;
        bsls::AtomicInt       d_inUseFlag; // used to lock this node
    };

    // DATA
    bsls::AtomicPointer<Node>  d_list;
    bsls::AtomicPointer<Node>  d_freeList;

    // PRIVATE MANIPULATORS
    Node *allocateNode();
    void freeNode(Node *node);
    void deleteNodes(Node *node);

  public:
    // CREATORS
    my_PtrStack();
   ~my_PtrStack();

    // MANIPULATORS
    void push(TYPE *item);
    TYPE *pop();
};
//..
// Then, we write a constructor that default-initializes the stack.  In the
// corresponding example in 'bsls_atomicoperations', the constructor must also
// initialize the atomic pointer 'd_freeList'.  Since this example uses the
// encapsulated type 'bsls::AtomicPointer', initialization of these member
// variables is done in their default constructors.  Hence, no explicit code is
// required in this constructor:
//..
// CREATORS
template <class TYPE>
inline my_PtrStack<TYPE>::my_PtrStack()
{
}
//..
// Next, we define the 'deleteNodes' and the destructor function to delete
// nodes that the 'my_PtrStack' object owns.  Note that we don't need to worry
// about the concurrent access to node lists in the destructor, as destructor
// can be executed in only a single thread:
//..
template <class TYPE>
inline void my_PtrStack<TYPE>::deleteNodes(Node *node)
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
    deleteNodes(d_list);
    deleteNodes(d_freeList);
}
//..
// Then, we define method 'allocateNode' to get a node from the free list in
// the thread-safe manner by leveraging atomic operations to ensure proper
// thread synchronization:
//..
// PRIVATE MANIPULATORS
template <class TYPE>
typename my_PtrStack<TYPE>::Node *my_PtrStack<TYPE>::allocateNode()
{
    Node *node;
    while (1) {
        node = d_freeList; // get the current head
        if (!node) {
            break;
        }
//..
// Next, we try locking the node, and start over if locking fails:
//..
        if (node->d_inUseFlag.swapInt(1)) {
            continue;
        }
//..
// Then, we atomically modify the head if it has not changed.  'testAndSwap'
// compares 'd_freeList' to 'node', replacing 'node' with 'node->d_next_p' only
// if it matches 'd_freeList'.  If 'd_freeList' did not match 'node', then the
// free list has been changed on another thread, between its assignment to the
// 'node' and the call to 'testAndSwap'.  If the list head has changed, then
// try again:
//..
        if (d_freeList.testAndSwap(node, node->d_next_p) == node) {
            break;
        }

        // Unlock the node.
        node->d_inUseFlag = 0;
    }
//..
// Next, we allocate a new node if there were no nodes in the free node list:
//..
    if (!node) {
        node = new Node();  // should allocate with 'd_allocator_p', but
                            // here we use 'new' directly for simplicity
        node->d_inUseFlag = 1;
    }

    return node;
}
//..
// Note that the 'node' is returned in the locked state and remained
// locked until it is added to the free list.
//
// Then, we define the 'freeNode' method to add a given 'node' to the free
// list; 'freeNode' also needs to be synchronized using atomic operations:
//..
template <class TYPE>
inline void my_PtrStack<TYPE>::freeNode(Node *node)
{
    if (!node) {
        return;                                                       // RETURN
    }

    while (1) {
        node->d_next_p = d_freeList;
        // Atomically test and swap the head of the list with the
        // new node.  If the list head has been changed (by another
        // thread), try again.
        if (d_freeList.testAndSwap(node->d_next_p, node) == node->d_next_p)
        {
            break;
        }
    }

    // unlock the 'node'
    node->d_inUseFlag = 0;
}
//..
// Now, we begin to define the public "stack-like" interface for 'my_PtrStack'.
// Note that the 'push' method is similar to 'freeNode', except that it assigns
// an item value and operates on 'd_list', which maintains the list of active
// nodes:
//..
// MANIPULATORS
template <class TYPE>
void my_PtrStack<TYPE>::push(TYPE *item)
{
    Node *node = allocateNode();
    node->d_item_p = item;
    while (1) {
        node->d_next_p = d_list;
        if (d_list.testAndSwap(node->d_next_p, node) == node->d_next_p) {
            break;
        }
    }

    node->d_inUseFlag = 0;
}
//..
// Finally, we define the 'pop' method which removes the node from the top
// of active node list, 'd_list', adds it to the free-node list, and returns
// the data item contained in the node to the caller:
//..
template <class TYPE>
TYPE *my_PtrStack<TYPE>::pop()
{
    Node *node;
    while (1) {
        node = d_list;
        if (!node) {
            break;
        }

        if (node->d_inUseFlag.swapInt(1)) {
            continue;  // node is locked
        }

        if (d_list.testAndSwap(node, node->d_next_p) == node) {
            break;  // node list is being modified in another thread
        }

        node->d_inUseFlag = 0;
    }

    TYPE *item = node ? node->d_item_p : 0;
    if (node) {
        freeNode(node);
    }
    return item;
}
//..
// Notice that if the stack was empty, a NULL pointer is returned.

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 10: {
        // TESTING USAGE Examples
        //
        // Plan:
        //
        // Testing:
        {
            my_PtrStack<int> stack;
        }
        {
            my_CountedHandle<double> handle(NULL);
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING MEMORY ORDERING OF ATOMIC OPERATIONS USED IN SHARED POINTER
        //
        // Concerns:
        //   Atomic operations commonly used to implement a shared pointer
        //   class implement the guaranteed memory orderings.
        //
        // Plan:
        //  1. Emulate the operations on shared count typically used in a
        //     shared pointer class:
        //    a. increment the shared count,
        //    b. decrement the shared count,
        //    c. access the shared data after the count is decremented.
        //  2. Create and start two threads that perform the above operations
        //     on the same shared count, and also manipulate the shared data.
        //  3. What the shared count is decremented to 0, verify that the
        //     shared data is in expected state.
        // --------------------------------------------------------------------
        if (verbose)
            cout << "\nTesting atomic operations used in shared pointer"
                 << "\n================================================"
                 << endl;

        int iterations = getTestCaseIterations(sharedCountLoopTest);
        if (veryVerbose) cout << "\tRunning the test loop for "
                              << iterations << " iterations" << endl;

        testCaseSharedPtr(iterations);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING ACQUIRE/RELEASE MEMORY ORDERING GUARANTEE
        //
        // Concerns:
        //   Atomic operations with acquire/release memory ordering guarantee
        //   do indeed exhibit that memory ordering.
        //
        // Plan:
        //  1. Implement a classical lock free algorithm, the Peterson's lock,
        //     which has been studied well enough to know the exact atomic
        //     operations it needs.
        //  2. Construct a test function which will read and write to some
        //     shared data protected by the Peterson's lock, and verify the
        //     consistency of the shared data.
        //  3. Start two threads, execute the test function in a loop in both
        //     threads and verify that the consistency of the shared data is
        //     not violated.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting acquire/release guarantee"
                          << "\n================================="
                          << endl;

        int iterations = getTestCaseIterations(petersonsLockLoopTest);
        if (veryVerbose) cout << "\tRunning the test loop for "
                              << iterations << " iterations" << endl;

        if (verbose) cout << "\t\twith bsls::AtomicInt" << endl;
        testCaseMemOrder<PetersonsLock, bsls::AtomicInt>(iterations);

        if (verbose) cout << "\t\twith bsls::AtomicInt64" << endl;
        testCaseMemOrder<PetersonsLock, bsls::AtomicInt64>(iterations);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING SEQUENTIAL CONSISTENCY MEMORY ORDERING GUARANTEE
        //
        // Concerns:
        //   Atomic operations with sequential consistency memory ordering
        //   guarantee do indeed exhibit that memory ordering.
        //
        // Plan:
        //  1. Implement a classical lock free algorithm, the Peterson's lock,
        //     which has been studied well enough to know the exact atomic
        //     operations it needs.
        //  2. Construct a test function which will read and write to some
        //     shared data protected by the Peterson's lock, and verify the
        //     consistency of the shared data.
        //  3. Start two threads, execute the test function in a loop in both
        //     threads and verify that the consistency of the shared data is
        //     not violated.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting sequential consistency guarantee"
                          << "\n========================================"
                          << endl;

        int iterations = getTestCaseIterations(petersonsLockLoopTest);
        if (veryVerbose) cout << "\tRunning the test loop for "
                              << iterations << " iterations" << endl;

        if (verbose) cout << "\t\twith bsls::AtomicInt" << endl;
        testCaseMemOrder<PetersonsLockSeqCst, bsls::AtomicInt>(iterations);

        if (verbose) cout << "\t\twith bsls::AtomicInt64" << endl;
        testCaseMemOrder<PetersonsLockSeqCst, bsls::AtomicInt64>(iterations);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING INCREMENT/DECREMENT MANIPULATORS
        //   Test the atomic increment and decrement operators for the
        //   AtomicInt and  AtomicInt64 types.
        //
        // Plan:
        //   For each atomic type(AtomicInt,AtomicInt64), using a sequence of
        //   independent test values, begin by initializing each object to a
        //   base value.  Then increment the value by 1 using to prefix
        //   'operator ++'.  Verify that both the return value and the
        //   resulting object value are equal to the expected result.  Repeat
        //   using the "postfix" 'operator ++' and verify that the return
        //   is equal to the base value and that the resulting object value is
        //   the expected value.  Finally perform similar tests for the
        //   prefix and postfix 'operator --' and verify that the results are
        //   correct.
        //
        // Testing:
        //  int operator ++();
        //  int operator ++(int);
        //  int operator --();
        //  int operator --(int);
        //  bsls::Types::Int64 operator ++();
        //  bsls::Types::Int64 operator ++(int);
        //  bsls::Types::Int64 operator --();
        //  bsls::Types::Int64 operator --(int);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Increment/Decrement Manipulators"
                          << "\n========================================="
                          << endl;

        if (verbose) cout << "\nTesting 'AtomicInt' Increment Manipulators"
                          << endl;
        {
            static const struct {
                int d_lineNum;   // source line number
                int d_value;     // input value
                int d_expected;  // expected resulting value
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
                int       result;

                AI x;  const AI& X = x;

                x = VAL;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL);P(EXP);
                }
                LOOP_ASSERT(i, VAL == X);
                result = ++x;
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == X);
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                const int EXP  = VALUES[i].d_expected;
                int       result;

                AI x;  const AI& X = x;

                x =VAL;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == X);
                result = x++;
                LOOP_ASSERT(i, VAL == result);
                LOOP_ASSERT(i, EXP == X);
            }
        }

        if (verbose) cout << "\nTesting 'AtomicInt' Decrement Manipulators"
                          << endl;
        {
            static const struct {
                int d_lineNum;   // source line number
                int d_expected;  // expected result of decrement
                int d_value;     // base value to be decremented
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
                int       result;

                AI x;  const AI& X = x;

                x = VAL;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL);P(EXP);
                }
                LOOP_ASSERT(i, VAL == X);
                result = --x;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL);P_(EXP); P(result);
                }
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == X);
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                const int EXP  = VALUES[i].d_expected;
                int       result;

                AI x;  const AI& X = x;

                x = VAL;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL);P(EXP);
                }
                LOOP_ASSERT(i, VAL == X);
                result = x--;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL);P_(EXP); P(result);
                }
                LOOP_ASSERT(i, VAL == result);
                LOOP_ASSERT(i, EXP == X);
            }
        }

        if (verbose) cout << "\nTesting 'AtomicInt64' increment Manipulators"
                          << endl;
        {
            static const struct {
                int   d_lineNum;   // source line number
                Int64 d_value;     // expected resulting value of decrement
                Int64 d_expected;  // input value

            } VALUES[] = {
                //line value                        expected
                //---- -------------------          ---------------------
                { L_,   0                          , 1                    },
                { L_,   1                          , 2                    },
                { L_,  -1LL                        , 0                    },
                { L_,   0xFFFFFFFFLL               , 0x100000000LL        },
                { L_,  (Int64) 0xFFFFFFFFFFFFFFFFLL , 0                   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Int64 VAL = VALUES[i].d_value;
                const Int64 EXP = VALUES[i].d_expected;
                Int64       result;

                AI64 x;  const AI64& X = x;

                x = VAL;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL);P_(EXP); NL();
                }
                LOOP_ASSERT(i, VAL == X);
                result = ++x;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL);P_(EXP); P(result);
                }
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == X);
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Int64 VAL = VALUES[i].d_value;
                const Int64 EXP = VALUES[i].d_expected;
                Int64       result;

                AI64 x;  const AI64& X = x;

                x = VAL;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == X);
                result = x++;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL);P_(EXP); P(result);
                }
                LOOP_ASSERT(i, VAL == result);
                LOOP_ASSERT(i, EXP == X);
            }
        }

        if (verbose) cout << "\nTesting 'AtomicInt64' decrement Manipulators"
                          << endl;
        {
            static const struct {
                int   d_lineNum;   // source line number
                Int64 d_expected;  // expected resulting value of decrement
                Int64 d_value;     // input value

            } VALUES[] = {
                //line expected                     value
                //---- -------------------          ---------------------
                { L_,   0                          , 1                    },
                { L_,   1                          , 2                    },
                { L_,  -1LL                        , 0                    },
                { L_,   0xFFFFFFFFLL               , 0x100000000LL        },
                { L_,  (Int64) 0xFFFFFFFFFFFFFFFFLL , 0                   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Int64 VAL = VALUES[i].d_value;
                const Int64 EXP = VALUES[i].d_expected;
                Int64       result;

                AI64 x;  const AI64& X = x;

                x = VAL;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL);P_(EXP); NL();
                }
                LOOP_ASSERT(i, VAL == X);
                result = --x;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL);P_(EXP); P(result);
                }
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == X);
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Int64 VAL = VALUES[i].d_value;
                const Int64 EXP = VALUES[i].d_expected;
                Int64       result;

                AI64 x;  const AI64& X = x;

                x = VAL;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL);P_(EXP);NL();
                }
                LOOP_ASSERT(i, VAL == X);
                result = x--;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL);P_(EXP); P(result);
                }
                LOOP_ASSERT(i, VAL == result);
                LOOP_ASSERT(i, EXP == X);
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING SWAP MANIPULATORS:
        //   Test the "swap" and "test and swap" functions for the Int,Int64,
        //   and Pointer atomic types.
        //
        // Plan:
        //   For each atomic type("AtomicInt", "AtomicInt64", "AtomicPointer"),
        //   perform the following tests to verify the swap, and testAndSwap
        //   manipulators.
        //
        // 1 Using an independent sequence of values, initialize an object and
        //   set its value to a base value.  Next 'swap' it with a second test
        //   value and assert that the new value is the swapped value and that
        //   the return value is the base value.
        //
        // 2 Using an independent sequence of values, initialize an object and
        //   set its value to a base value.  Next 'testAndSwap' it with a
        //   second test value and assert that the new value is the expected
        //   value and that the return value is the expected return value.
        //
        // Testing:
        //   int swap(int swapValue);
        //   int testAndSwap(int compareValue,int swapValue);
        //   bsls::Types::Int64 swap(bsls::Types::Int64 swapValue);
        //   bsls::Types::Int64 testAndSwap(bsls::Types::Int64 ...
        //   T* swap(const T* swapValue);
        //   T* testAndSwap(const T* compareValue, const T* swapValue);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting SWAP Manipulators"
                          << "\n========================================="
                          << endl;

        if (verbose) cout << "\nTesting 'AtomicInt' SWAP Manipulators" << endl;
        {
            static const struct {
                int  d_lineNum;   // source line number
                int d_value;      // initial value
                int d_swapValue;  // swap value
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

                AI x; const AI& X = x;

                x = VAL;
                LOOP_ASSERT(i, VAL  == X);

                result = x.swap(SWPVAL);

                if (veryVerbose) {
                    T_(); P_(X); P_(VAL); P(SWPVAL);
                }
                LOOP_ASSERT(i, SWPVAL == X);
                LOOP_ASSERT(i, VAL    == result );
            }
        }

        if (verbose) cout << endl
                          << "\tTesting 'testAndSwapInt'" << endl
                          << "\t------------------------" << endl;
        {
            static const struct {
                int d_lineNum;       // source line number
                int d_value;         // initial value
                int d_swapValue;     // swap value
                int d_compareValue;  // compare value
                int d_expValue;      // expected value after the operations
                int d_expResult;     // expected result
            } VALUES[] = {
            //ln  val swapValue          cmpValue expValue            expResult
            //--- --- ------------------ -------- ------------------- ---------
            { L_,  0, 11               , 33     ,  0                ,  0     },
            { L_,  1, 19               , 1      , 19                ,  1     },
            { L_, -1, 4                , 1      , -1                , -1     },
            { L_,  2, (int) 0xFFFFFFFF , 2      , (int) 0xFFFFFFFF  ,  2     },
            { L_, -2, 16               , 0      , -2                , -2     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL    = VALUES[i].d_value;
                const int CMPVAL = VALUES[i].d_compareValue;
                const int SWPVAL = VALUES[i].d_swapValue;
                const int EXPVAL = VALUES[i].d_expValue;
                const int EXPRES = VALUES[i].d_expResult;
                int       result = 0;

                AI x; const AI& X = x;

                x = VAL;
                LOOP_ASSERT(i, VAL == X);
                result = x.testAndSwap(CMPVAL,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(X);
                    P_(VAL);P_(CMPVAL);P_(SWPVAL); P_(result);
                    P_(EXPVAL);P_(EXPRES); NL();
                }
                LOOP_ASSERT(i, EXPVAL == X);
                LOOP_ASSERT(i, EXPRES == result );
            }
        }

        if (verbose) cout << "\nTesting 'AtomicInt64' SWAP Manipulators"
                          << endl;
        {
            static const struct {
                int   d_lineNum;    // source line number
                Int64 d_value;      // initial value
                Int64 d_swapValue;  // swap value
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
                const Int64 VAL    = VALUES[i].d_value;
                const Int64 SWPVAL = VALUES[i].d_swapValue;
                Int64       result = 0;

                AI64 x; const AI64& X = x;

                x = VAL;
                LOOP_ASSERT(i, VAL == X);
                result = x.swap(SWPVAL);

                if (veryVerbose) {
                    T_(); P_(X); P_(VAL);
                    P_(SWPVAL); NL();
                }
                LOOP_ASSERT(i, SWPVAL == X);
                LOOP_ASSERT(i, VAL    == result );
            }
        }

        {
            static const struct {
                int   d_lineNum;       // source line number
                Int64 d_value;         // initial value
                Int64 d_swapValue;     // swap value
                Int64 d_compareValue;  // compare value
                Int64 d_expValue;      // expected value after the operation
                Int64  d_expResult;    // expected result
            } VALUES[] = {
                //line value swapValue    cmpValue  expValue      expResult
                //---- ----- ------------ --------- ------------- ---------
                { L_,   0LL  , 11         , 33     , 0          , 0       },
                { L_,   1LL  , 19         , 1      , 19         , 1       },
                { L_,  -1LL  , 4          , 1      , -1LL        , -1LL     },
                { L_,   2LL  , 0xFFFFFFFFFLL, 2    , 0xFFFFFFFFFLL, 2       },
                { L_,  -2LL  , 16         , 0      , -2LL        , -2LL     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Int64 VAL    = VALUES[i].d_value;
                const Int64 CMPVAL = VALUES[i].d_compareValue;
                const Int64 SWPVAL = VALUES[i].d_swapValue;
                const Int64 EXPVAL = VALUES[i].d_expValue;
                const Int64 EXPRES = VALUES[i].d_expResult;
                Int64       result = 0;

                AI64 x; const AI64& X = x;

                x = VAL;
                LOOP_ASSERT(i, VAL == X);

                result = x.testAndSwap(CMPVAL,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(X);
                    P_(VAL);P_(CMPVAL);P_(SWPVAL); P_(result);
                    P_(EXPVAL);P_(EXPRES);NL();
                }
                LOOP_ASSERT(i, EXPVAL == X);
                LOOP_ASSERT(i, EXPRES == result );
            }
        }
        if (verbose) cout << "\nTesting 'Pointer' SWAP Manipulators" << endl;
        {
            static const struct {
                int        d_lineNum;    // source line number
                APTestObj *d_value;      // initial value
                APTestObj *d_swapValue;  // swap value
            } VALUES[] = {
                //line value             swap
                //---- ----------------- --------------------
                { L_,  (APTestObj*)0    , (APTestObj*)11     },
                { L_,  (APTestObj*)1    , (APTestObj*)19     },
                { L_,  (APTestObj*)-1   , (APTestObj*)4      },
                { L_,  (APTestObj*) 2   , (APTestObj*)-4     },
                { L_,  (APTestObj*)-2   , (APTestObj*)16     }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                APTestObj *VAL    = VALUES[i].d_value;
                APTestObj *SWPVAL = VALUES[i].d_swapValue;
                APTestObj *result = 0;

                AP x; const AP& X = x;

                x = VAL;
                LOOP_ASSERT(i, VAL == X);
                result = x.swap(SWPVAL);

                if (veryVerbose) {
                    T_(); P_(X); P_(VAL);P_(SWPVAL);NL();
                }
                LOOP_ASSERT(i, SWPVAL == X);
                LOOP_ASSERT(i, VAL    == result );
            }
        }

        {
            static const struct {
                int        d_lineNum;       // source line number
                APTestObj *d_value;         // initial value
                APTestObj *d_swapValue;     // swap value
                APTestObj *d_compareValue;  // compare value
                APTestObj *d_expValue;      // expected value after operations
                APTestObj *d_expResult;     // expected result
            } VALUES[] = {
                //line value        swapValue           cmpValue
                //---- ------------ ------------------- --------------
                //    expValue             expResult
                //    -------------------- ----------------
                { L_, (APTestObj*) 0 , (APTestObj*)11         , (APTestObj*)33,
                      (APTestObj*)0        , (APTestObj*)0       },
                { L_, (APTestObj*) 1 , (APTestObj*)19         , (APTestObj*)1,
                      (APTestObj*)19       , (APTestObj*)1       },
                { L_, (APTestObj*)-1 , (APTestObj*)4          , (APTestObj*)1,
                      (APTestObj*)-1       , (APTestObj*)-1      },
                { L_, (APTestObj*) 2 , (APTestObj*)0xFFFFFFFF , (APTestObj*)2,
                      (APTestObj*)0xFFFFFFFF , (APTestObj*)2       },
                { L_, (APTestObj*)-2 , (APTestObj*)16         , (APTestObj*)0,
                      (APTestObj*)-2         , (APTestObj*)-2      }
            };
            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const APTestObj *VAL    = VALUES[i].d_value;
                const APTestObj *CMPVAL = VALUES[i].d_compareValue;
                const APTestObj *SWPVAL = VALUES[i].d_swapValue;
                const APTestObj *EXPVAL = VALUES[i].d_expValue;
                const APTestObj *EXPRES = VALUES[i].d_expResult;
                const APTestObj *result = 0;

                CAP x; const CAP& X = x;

                x = VAL;
                LOOP_ASSERT(i, VAL == X);
                result = x.testAndSwap(CMPVAL,SWPVAL);

                if (veryVerbose) {
                    T_(); P_(X);
                    P_(VAL);P_(CMPVAL);P_(SWPVAL);
                    P_(EXPVAL);P_(EXPRES);NL();
                }
                LOOP_ASSERT(i, EXPVAL == X);
                LOOP_ASSERT(i, EXPRES == result );
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ARITHMETIC MANIPULATORS
        //   Test that the 32/64 bit integer arithmetic functions work as
        //   expected.
        //
        // Plan:
        //   For each atomic type('AtomicInt', and 'AtomicInt64') using a
        //   sequence of independent values, begin by initializing the
        //   value to 0 and adding the test value using the 'operator+='.
        //   Assert that the resulting value is the expected value.  Then
        //   subtract the test value using the 'operator-=' and verify that
        //   The value returns to 0.  Repeat the operation using the
        //   'add' method and assert the both the resulting value of the
        //   object and the return value from the operation are both the
        //   expected value.  Next, initialize each object to a base value
        //   add a delta value using 'operator+='.  Assert that the result
        //   are still correct.  Then subtract the delta value using the
        //   'operator -=' and verify that the value returns to the base value.
        //   Repeat this step for the 'add' method and assert that the
        //   return value and resulting object values are correct.
        //
        // Testing:
        //   int add(int value);
        //   void operator +=(int value);
        //   void operator -=(int value);
        //   bsls::Types::Int64 add(bsls::Types::Int64 value);
        //   void operator +=(bsls::Types::Int64 value);
        //   void operator -=(bsls::Types::Int64 value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Arithmetic Manipulators" << endl
                          << "========================================="
                          << endl;

        if (verbose) cout << endl
                          << "Testing 'AtoicInt' Arithmetic Manipulators"
                          << endl;
        {
            static const struct {
                int d_lineNum;  // source line number
                int d_value;    // input value
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

                AI x;  const AI& X = x;
                ASSERT(0 == X);

                x += VAL;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == X);

                x -= VAL;

                if (veryVerbose) {
                    T_(); P_(X); P_(VAL); NL();
                }
                LOOP_ASSERT(i, 0 == X);
            }

            if (verbose) cout << endl
               << "\tTesting 'AtomicInt' Arithmetic(and values) Manipulators"
               << endl
               << "\t-------------------------------------------------"
               << endl;
            for (int i = 0; i < NUM_VALUES; ++i) {
                const int VAL  = VALUES[i].d_value;
                int       result;

                AI x;  const AI& X = x;

                ASSERT(0 == X);

                result = x.add(VAL);
                if (veryVerbose) {
                    T_(); P_(X); P_(result); P(VAL);
                }
                LOOP_ASSERT(i, VAL == X);
                LOOP_ASSERT(i, VAL == result);
            }

        }

        if (verbose) cout << endl
                          << "\tTesting 'AtomicInt' Arith(with base) Manip"
                          << endl;
        {
            static const struct {
                int d_lineNum;   // source line number
                int d_base;      // base value
                int d_amount;    // amount to add
                int d_expected;  // expected value
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

                AI x;  const AI& X = x;

                ASSERT(0 == X);

                x = BASE;
                ASSERT(BASE == X);

                x += AMT;
                if (veryVerbose) {
                    T_(); P_(X);
                    P_(BASE); P_(AMT); P_(EXP); NL();
                }
                LOOP_ASSERT(i, EXP == X);

                x -= AMT;
                if (veryVerbose) {
                    T_(); P_(X); P_(BASE); P_(AMT); P(EXP);
                }
                LOOP_ASSERT(i, BASE == X);

            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int BASE = VALUES[i].d_base;
                const int AMT  = VALUES[i].d_amount;
                const int EXP  = VALUES[i].d_expected;
                int       result;

                AI x;  const AI& X = x;

                ASSERT(0 == X);

                x = BASE;
                ASSERT(BASE == X);

                result = x.add(AMT);
                if (veryVerbose) {
                    T_(); P_(X);
                    P_(BASE); P_(AMT); P_(EXP); P_(result); NL();
                }
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == X);
            }

        }

        if (verbose) cout << "\nTesting 'AtomicInt64' Arithmetic Manipulators"
                          << endl;
        {
            static const struct {
                int   d_lineNum;  // source line number
                Int64 d_value;    // input value
            } VALUES[] = {
                //line d_x
                //---- ----
                { L_,   0     },
                { L_,   1     },
                { L_,  -1LL   },
                { L_,   2     },
                { L_,  -2LL   }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Int64 VAL = VALUES[i].d_value;

                AI64 x;  const AI64& X = x;
                ASSERT(0 == X);
                x += VAL;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == X);
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Int64 VAL = VALUES[i].d_value;
                Int64       result;

                AI64 x;  const AI64& X = x;

                ASSERT(0 == X);
                result = x.add(VAL);
                if (veryVerbose) {
                    T_(); P_(X);
                    P_(VAL); P_(result); NL();
                }
                LOOP_ASSERT(i, VAL == result);
                LOOP_ASSERT(i, VAL == X);
            }

        }
        {
            static const struct {
                int   d_lineNum;   // source line number
                Int64 d_base;      // base value
                Int64 d_amount;    // amount to add
                Int64 d_expected;  // expected value
            } VALUES[] = {
                //line d_base        d_amount d_expected
                //---- ------------- -------- ----------
                { L_,  -1LL         , 10      , 9                    },
                { L_,  1            , -2LL    , -1LL                 },
                { L_,  -1LL         , 2LL     , 1LL                  },
                { L_,  0xFFFFFFFFLL , 1LL     , 0x100000000LL        },
                { L_,  0x100000000LL, -2LL    , 0xFFFFFFFELL         }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Int64 BASE = VALUES[i].d_base;
                const Int64 AMT  = VALUES[i].d_amount;
                const Int64 EXP  = VALUES[i].d_expected;

                AI64 x;  const AI64& X = x;
                ASSERT(0 == X);

                x = BASE;
                ASSERT(BASE == X);

                x += AMT;
                if (veryVerbose) {
                    T_(); P_(X); P(BASE);
                    T_(); P_(AMT); P(EXP);
                }
                LOOP_ASSERT(i, EXP == X);
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Int64 BASE = VALUES[i].d_base;
                const Int64 AMT  = VALUES[i].d_amount;
                const Int64 EXP  = VALUES[i].d_expected;
                Int64       result;

                AI64 x;  const AI64& X = x;
                ASSERT(0 == X);

                x = BASE;
                ASSERT(BASE == X);

                result = x.add(AMT);
                if (veryVerbose) {
                    T_(); P_(X); P(BASE);
                    T_(); P_(AMT); P(EXP); NL();
                }
                LOOP_ASSERT(i, EXP == result);
                LOOP_ASSERT(i, EXP == X);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZATION, COPY CONSTRUCTORS, AND ASSIGNMENT OPERATOR
        //
        // Plan:
        //   First, verify the initialization functions by initializing each
        //   atomic type and testing the resulting value.
        //
        //   Next, for the AtomicInt, AtomicInt64, and AtomicPointer types,
        //   for a sequence of independent test values, use the
        //   initialization constructor to construct an object 'x' of each
        //   type.  Then using the copy constructor, construct an object 'y'
        //   from 'x'.  Next construct a third object 'z'.  Using the
        //   assignment operator, assign the value of 'x' to 'z'.  Finally
        //   Verify that the value of 'x', 'y', and 'z' are correct.
        //
        // Testing:
        //   bsls::AtomicInt(const bsls::AtomicInt& rhs);
        //   bsls::AtomicInt(int value);
        //   bsls::AtomicInt& operator= (const bsls::AtomicInt& rhs);
        //   bsls::AtomicInt64(const bsls::AtomicInt64& original);
        //   bsls::AtomicInt64(bsls::Types::Int64 value);
        //   bsls::AtomicInt64& operator= (const bsls::AtomicInt64& rhs);
        //   bsls::AtomicPointer(const bsls::AtomicPointer<T>& original);
        //   bsls::AtomicPointer(const T* value);
        //   bsls::AtomicPointer<T>& operator=(const bsls::AtomicPointer<T>&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulators"
                          << "\n============================" << endl;

        if (verbose) cout << "\nTesting 'AtomicInt' Primary Manipulators"
                          << endl;
        {
            static const struct {
                int d_lineNum;  // source line number
                int d_value;    // input value
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

                AI x(VAL);              const AI& X = x;
                AI y(X.loadRelaxed());  const AI& Y = y;
                AI z;                   const AI& Z = z;

                z = X.loadRelaxed();
                if (veryVerbose) {
                    T_(); P_(X); P_(Y); P_(Z); P(VAL);
                }
                LOOP_ASSERT(i, VAL == X);
                LOOP_ASSERT(i, VAL == Y);
                LOOP_ASSERT(i, VAL == Z);
            }
        }

        if (verbose) cout <<"\nTesting 'AtomicInt64' Primary Manipulators"
                          << endl;
        {
            static const struct {
                int   d_lineNum;  // source line number
                Int64 d_value;    // input value
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
                const Int64 VAL = VALUES[i].d_value;

                AI64 x(VAL);                const AI64& X = x;
                AI64 y(X.loadRelaxed());    const AI64& Y = y;
                AI64 z;                     const AI64& Z = z;

                z = X.loadRelaxed();
                if (veryVerbose) {
                    T_(); P_(X); P_(Y); P_(Z); P(VAL);
                }
                LOOP_ASSERT(i, VAL == X);
                LOOP_ASSERT(i, VAL == Y);
                LOOP_ASSERT(i, VAL == Z);
            }
        }

        if (verbose)
            cout << "\nTesting 'bsls::AtomicPointer' Primary Manipulators"
                 << endl;
        {
            static const struct {
                int        d_lineNum;  // source line number
                APTestObj *d_value;    // input value
            } VALUES[] = {
                //line value
                //---- -----------------------
                { L_,  (APTestObj*)0x0        },
                { L_,  (APTestObj*)0x1        },
                { L_,  (APTestObj*)0xffff     },
                { L_,  (APTestObj*)0xffff8888 },
                { L_,  (APTestObj*)0xffffffff }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                APTestObj* VAL  = VALUES[i].d_value;

                AP x(VAL);              const AP& X = x;
                AP y(X.loadRelaxed());  const AP& Y = y;
                AP z;                   const AP& Z = z;

                z = X.loadRelaxed();
                if (veryVerbose) {
                    T_(); P_(X); P_(Y); P_(Z); P(VAL);
                }
                LOOP_ASSERT(i, VAL == X);
                LOOP_ASSERT(i, VAL == Y);
                LOOP_ASSERT(i, VAL == Z);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   Verify that the default construct properly initializes each
        //   atomic type, and that basic manipulators and accessors
        //   work correctly.
        //
        // Plan:
        //   For each atomic type(AtomicInt, AtomicInt64, AtomicPointer),
        //   Begin by constructing an object using the default constructor
        //   and verify that it is the expected default value.  Then for a
        //   sequence independent test values, set the value using the basic
        //   manipulator('operator=').  Verify that the value is correct using
        //   the respective direct accessor('operator int',
        //   'operator bsls::Types::Int64', 'operator T*').
        //
        // Testing:
        //   bsls::AtomicInt();
        //   ~bsls::AtomicInt()
        //   bsls::AtomicInt& operator= (int value);
        //   operator int() const;
        //   bsls::AtomicInt64();
        //   ~bsls::AtomicInt64();
        //   bsls::AtomicInt64& operator= (bsls::Types::Int64 value);
        //   operator bsls::Types::Int64() const;
        //   bsls::AtomicPointer();
        //   bsls::AtomicPointer<T>& operator= (const T *value);
        //   ~bsls::AtomicPointer();
        //   T& operator*() const;
        //   T* operator->() const;
        //   operator T*() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulators"
                          << "\n============================" << endl;

        if (verbose) cout << "\nTesting 'bsls::AtomicInt' Primary Manipulators"
                          << endl;
        {
            static const struct {
                int d_lineNum;  // source line number
                int d_value;    // input value
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

                AI x;  const AI& X = x;
                ASSERT(0 == X);

                x = VAL;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == X);
            }
        }

        if (verbose)
            cout << "\nTesting 'bsls::AtomicInt64' Primary Manipulators"
                 << endl;
        {
            static const struct {
                int   d_lineNum;  // source line number
                Int64 d_value;    // input value
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
                const Int64 VAL = VALUES[i].d_value;

                AI64 x;  const AI64& X = x;
                ASSERT(0 == X);

                x = VAL;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL); NL();
                }
                LOOP_ASSERT(i, VAL == X);
            }
        }

        if (verbose)
            cout << "\nTesting 'bsls::AtomicPointer' Primary Manipulators"
                 << endl;
        {
            static const struct {
                int        d_lineNum;  // source line number
                APTestObj *d_value;    // input value
            } VALUES[] = {
                //line value
                //---- -----------------------
                { L_,  (APTestObj*)0x0        },
                { L_,  (APTestObj*)0x1        },
                { L_,  (APTestObj*)0xffff     },
                { L_,  (APTestObj*)0xffff8888 },
                { L_,  (APTestObj*)0xffffffff }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                APTestObj *VAL = VALUES[i].d_value;

                AP x; const AP& X = x;

                LOOP_ASSERT(i, (APTestObj*)0 == X);

                x = VAL;
                if (veryVerbose) {
                    T_(); P_(X); P_(VAL); NL();
                }

                LOOP_ASSERT(i, VAL == X);
                LOOP_ASSERT(i, VAL == &(*X));
                LOOP_ASSERT(i, VAL == X->self());
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

        int XVA = -1;
        int XVB =  2;
        int XVC = -2;

        APTestObj *PVA = (APTestObj*)0xffff8888;
        APTestObj *PVB = (APTestObj*)0xffffffff;
        APTestObj *PVC = (APTestObj*)0x78888888;

        int lresult;
        void *pResult;

        AI mX1;

        ASSERT(0 == mX1);

        mX1 = XVA;
        ASSERT(XVA == mX1);

        mX1.storeRelaxed(XVB);
        ASSERT(mX1.loadRelaxed() == XVB);

        mX1.storeRelease(XVC);
        ASSERT(mX1.loadAcquire() == XVC);

        mX1 = 0;
        ASSERT(0 == mX1);

        ++mX1;
        ASSERT(1 == mX1);

        --mX1;
        ASSERT(0 == mX1);

        lresult = ++mX1;
        ASSERT(1 == lresult);
        ASSERT(1 == mX1);
        if ( veryVeryVerbose) {
            T_(); P(lresult); NL();
        }

        lresult = --mX1;
        ASSERT(0 == lresult);
        ASSERT(0 == mX1);

        mX1 = 0;
        ASSERT(0 == mX1);

        mX1 += XVB;
        ASSERT(XVB == mX1);

        mX1.addRelaxed(XVC);
        ASSERT(mX1 == XVB + XVC);

        mX1.addAcqRel(XVA);
        ASSERT(mX1 == XVA + XVB + XVC);

        mX1 = 0;
        ASSERT(0 == mX1);

        lresult = mX1.swap(XVA);
        ASSERT(0 == lresult);
        ASSERT(mX1 == XVA);

        lresult = mX1.swapAcqRel(XVC);
        ASSERT(lresult == XVA);
        ASSERT(mX1 == XVC);

        lresult = mX1.testAndSwap(XVA, XVB);
        ASSERT(XVC == lresult);
        ASSERT(XVC == mX1);

        lresult = mX1.testAndSwap(XVC, 0);
        ASSERT(XVC == lresult);
        ASSERT(0 == mX1);

        lresult = mX1.testAndSwapAcqRel(XVC, XVA);
        ASSERT(lresult == 0);
        ASSERT(mX1 == 0);

        lresult = mX1.testAndSwapAcqRel(0, XVA);
        ASSERT(lresult == 0);
        ASSERT(mX1 == XVA);

        // Pointers
        // --------

        if (veryVerbose) cout << endl
                              << "\tPointers" << endl
                              << "\t--------" << endl;
        AP mP1;

        ASSERT(((APTestObj*)0) == mP1);

        pResult = mP1.swap(PVC);
        ASSERT(((APTestObj*)0) == pResult);
        ASSERT(PVC == mP1);

        pResult = mP1.testAndSwap(PVA,PVB);
        ASSERT(PVC == pResult);
        ASSERT(PVC == mP1);

        pResult = mP1.testAndSwap(PVC,0);
        ASSERT(PVC == pResult);
        ASSERT(((APTestObj*)0) == mP1);

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
