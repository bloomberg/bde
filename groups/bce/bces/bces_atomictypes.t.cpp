// bces_atomictypes.t.cpp           -*-C++-*-

#include <bces_atomictypes.h>

#include <bslma_allocator.h>
#include <bslma_default.h>       // for usage example(s)
#include <bsls_objectbuffer.h>
#include <bsls_platformutil.h>

#include <bsl_cstdlib.h>               // atoi()
#include <bsl_iostream.h>

// For thread support
#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
typedef HANDLE thread_t;
#else
#include <pthread.h>
typedef pthread_t thread_t;
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// This component provides a set of wrapper classes for their respective
// 'bces_AtomicUtil' types and operations.  Since the provided classes are
// only proxies to the respective atomic operations, this test driver
// only verifies that each class and operation, is properly "hooked up" to its
// respective 'bces_AtomicUtil' type and operation.
//
//-----------------------------------------------------------------------------
// bces_AtomicInt
// --------------
// [ 2] bces_AtomicInt();
// [ 3] bces_AtomicInt(const bces_AtomicInt& rhs);
// [ 3] bces_AtomicInt(int value);
// [ 2] ~bces_AtomicInt();
// [ 5] int swap(int swapValue);
// [ 5] int testAndSwap(int compareValue,int swapValue);
// [ 4] int add(int value);
// [ 6] int operator ++();
// [ 6] int operator ++(int);
// [ 6] int operator --();
// [ 6] int operator --(int);
// [ 3] bces_AtomicInt& operator= (const bces_AtomicInt& rhs);
// [ 2] bces_AtomicInt& operator= (int value);
// [ 4] void operator +=(int value);
// [ 4] void operator -=(int value);
// [ 2] operator int() const;
//
// bces_AtomicInt64
// ----------------
// [ 2] bces_AtomicInt64();
// [ 3] bces_AtomicInt64(const bces_AtomicInt64& original);
// [ 3] bces_AtomicInt64(bsls_PlatformUtil::Int64 value);
// [ 2] ~bces_AtomicInt64();
// [ 4] bsls_PlatformUtil::Int64 add(bsls_PlatformUtil::Int64 value);
// [ 5] bsls_PlatformUtil::Int64 swap(bsls_PlatformUtil::Int64 swapValue);
// [ 5] bsls_PlatformUtil::Int64 testAndSwap(bsls_PlatformUtil::Int64 ...
// [ 6] bsls_PlatformUtil::Int64 operator ++();
// [ 6] bsls_PlatformUtil::Int64 operator ++(int);
// [ 6] bsls_PlatformUtil::Int64 operator --();
// [ 6] bsls_PlatformUtil::Int64 operator --(int);
// [ 3] bces_AtomicInt64& operator= (const bces_AtomicInt64& rhs);
// [ 2] bces_AtomicInt64& operator= (bsls_PlatformUtil::Int64 value);
// [ 4] void operator +=(bsls_PlatformUtil::Int64 value);
// [ 4] void operator -=(bsls_PlatformUtil::Int64 value);
// [ 2] operator bsls_PlatformUtil::Int64() const;
//
// bces_AtomicPointer
// ------------------
// [ 2] bces_AtomicPointer();
// [ 3] bces_AtomicPointer(const bces_AtomicPointer<T>& original);
// [ 3] bces_AtomicPointer(const T* value);
// [ 2] ~bces_AtomicPointer();
// [ 5] T* swap(const T* swapValue);
// [ 5] T* testAndSwap(const T* compareValue, const T* swapValue);
// [ 3] bces_AtomicPointer<T>& operator= (const bces_AtomicPointer<T>& rhs);
// [ 2] bces_AtomicPointer<T>& operator= (const T *value);
// [ 2] T& operator*() const;
// [ 3] T* operator->() const;
// [ 2] operator T*() const;
//
// bces_SpinLock
// -------------
// [ 7] bces_SpinLock();
// [ 7] ~bces_SpinLock();
// [ 7] void lock();
// [ 7] int tryLock(int retries=100);
// [ 7] void unlock();
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
#define P64(X) printf(#X " = %lld\n", (X));   // Print 64-bit integer id & val
#define P64_(X) printf(#X " = %lld,  ", (X)); // Print 64-bit integer w/o '\n'

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

struct APTestObj;

typedef bces_AtomicInt                      AI;
typedef bces_AtomicInt64                    AI64;
typedef bces_AtomicPointer<APTestObj>       AP;
typedef bces_AtomicPointer<const APTestObj> CAP;
typedef bces_SpinLock                       SL;

typedef bsls_PlatformUtil::Int64            Int64;

//=============================================================================
//                         HELPER CLASSES AND FUNCTIONS  FOR TESTING
//-----------------------------------------------------------------------------

struct APTestObj
    // This structure is used to test Atomic Pointers the self method
    // returns the 'this' pointer of this object.  The method is used
    // to test 'operator ->'.
{
    const APTestObj* self() const { return this;}
        // Return a pointer to this object.
};

namespace {

class PetersonsLockSeqCst
    // PetersonsLock class implements the Peterson's locking algorithms for two
    // concurrently executing threads, using atomic operations on integers with
    // sequencial consistency memory ordering semantics.
{
public:
    struct Data
    {
        bces_AtomicInt  flags[2];
        bces_AtomicInt  turn;     // 0 or 1 - index into d_flags
    };

public:
    PetersonsLockSeqCst(int id, Data& lockData)
        : d_id(id)
        , d_data(lockData)
    {
        ASSERT(id == 0 || id == 1);
    }

    void lock()
    {
        d().flags[d_id] = 1;
        d().turn.swap(1 - d_id);

        while (d().flags[1 - d_id] && d().turn == 1 - d_id)
        {
            continue;
        }
    }

    void unlock()
    {
        d().flags[d_id] = 0;
    }

private:
    Data& d() const
    {
        return d_data;
    }

private:
    int     d_id;       // 0 or 1 - id of the thread that owns this object
    Data&   d_data;
};

template <typename LOCK>
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

bces_AtomicInt s_data1(0);
bces_AtomicInt s_data2(0);
bces_AtomicInt s_data3(1);

template <typename LOCK>
void testAtomicLocking(LOCK& lock, int iterations)
    // Test 'lock' implemented using atomic operations by using it to protect
    // some shared data, which is both read and written to.
{
    for (int i = 0; i < iterations; ++i) {
        if (rand() & 1) {
            // read shared data
            Guard<LOCK> guard(lock);

            int data1 = s_data1.relaxedLoad();
            int data2 = s_data2.relaxedLoad();
            int data3 = s_data3.relaxedLoad();

            ASSERT(data1 == -data2 && data1 + 1 == data3);
        }
        else {
            // write to shared data
            Guard<LOCK> guard(lock);

            int data = rand();
            s_data1.relaxedStore(data);
            s_data2.relaxedStore(-data);
            s_data3.relaxedStore(data + 1);
        }
    }
}

template <typename LOCK>
struct AtomicLockingThreadParam
{
    AtomicLockingThreadParam(LOCK& lock, int iterations)
        : d_lock(lock)
        , d_iterations(iterations)
    {}

    LOCK&   d_lock;
    int     d_iterations;
};

template <typename LOCK>
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


template <typename LOCK>
void testCaseMemOrder()
{
    int iterations = 10000000;

    typename LOCK::Data lockData;
    LOCK lock0(0, lockData);
    LOCK lock1(1, lockData);

    AtomicLockingThreadParam<LOCK> param0(lock0, iterations);
    AtomicLockingThreadParam<LOCK> param1(lock1, iterations);

    thread_t thr0 = createThread(&testAtomicLockingThreadFunc<LOCK>, &param0);
    thread_t thr1 = createThread(&testAtomicLockingThreadFunc<LOCK>, &param1);

    joinThread(thr0);
    joinThread(thr1);
}

}

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

static bces_AtomicInt64 transactionCount;
static bces_AtomicInt64 successCount;
static bces_AtomicInt64 failureCount;

void serverMain()
{
    const int num_threads = 10;
    for (int i=0; i<num_threads; ++i) {
        createWorkerThread();
    }
    waitAllThreads();
}

// EXAMPLE 2

template <class INSTANCE, class FACTORY>
class my_CountedHandle;

template <class INSTANCE, class FACTORY>
class my_CountedHandleRep {
    bces_AtomicInt  d_count;       // number of active references
    INSTANCE        *d_instance_p;  // address of managed instance
    FACTORY         *d_factory_p;   // held but not owned
    bslma_Allocator *d_allocator_p; // held but not owned
    friend class my_CountedHandle<INSTANCE, FACTORY>;
  private: // not implemented
    my_CountedHandleRep(const my_CountedHandleRep&);
    my_CountedHandleRep& operator=(const my_CountedHandleRep&);
  private:
    // CLASS METHODS
    static void
    deleteObject(my_CountedHandleRep<INSTANCE, FACTORY> *object);
    // CREATORS
    my_CountedHandleRep(INSTANCE        *instance,
                           FACTORY         *factory,
                           bslma_Allocator *basicAllocator);
    ~my_CountedHandleRep();
    // MANIPULATORS
    void increment();
    int decrement();
};

                        // ======================
                        // class my_CountedHandle
                        // ======================

template <class INSTANCE, class FACTORY>
class my_CountedHandle {
    my_CountedHandleRep<INSTANCE, FACTORY> *d_rep_p;  // shared rep.
  public:
    // CREATORS
    my_CountedHandle();
    my_CountedHandle(INSTANCE        *instance,
                        FACTORY         *factory,
                        bslma_Allocator *basicAllocator = 0);
    my_CountedHandle(const my_CountedHandle<INSTANCE, FACTORY>& other);
    ~my_CountedHandle();
     // ACCESSORS
    INSTANCE *operator->() const;
    int numReferences() const;
};

template <class INSTANCE, class FACTORY>
inline
void my_CountedHandleRep<INSTANCE, FACTORY>::deleteObject(
                             my_CountedHandleRep<INSTANCE, FACTORY> *object)
{
    object->~my_CountedHandleRep();
    object->d_allocator_p->deallocate(object);
}

template <class INSTANCE, class FACTORY>
inline
my_CountedHandleRep<INSTANCE, FACTORY>::
                        my_CountedHandleRep(INSTANCE        *instance,
                                            FACTORY         *factory,
                                            bslma_Allocator *basicAllocator)
: d_instance_p(instance)
, d_factory_p(factory)
, d_allocator_p(basicAllocator)
, d_count(1)
{
}

template <class INSTANCE, class FACTORY>
inline
my_CountedHandleRep<INSTANCE, FACTORY>::~my_CountedHandleRep()
{
    d_factory_p->deallocate(d_instance_p);
}
// MANIPULATORS
template <class INSTANCE, class FACTORY>
inline
void my_CountedHandleRep<INSTANCE, FACTORY>::increment()
{
    ++d_count;
}

template <class INSTANCE, class FACTORY>
inline
int my_CountedHandleRep<INSTANCE, FACTORY>::decrement()
{
    return --d_count;
}
                        // ----------------------
                        // class my_CountedHandle
                        // ----------------------

template <class INSTANCE, class FACTORY>
inline
my_CountedHandle<INSTANCE, FACTORY>::my_CountedHandle()
: d_rep_p(0)
{
}

template <class INSTANCE, class FACTORY>
inline
my_CountedHandle<INSTANCE, FACTORY>::my_CountedHandle(
                                             INSTANCE        *object,
                                             FACTORY         *factory,
                                             bslma_Allocator *basicAllocator)
{
    bslma_Allocator *ba = bslma_Default::allocator(basicAllocator);
    d_rep_p = new(ba->allocate(sizeof *d_rep_p))
                my_CountedHandleRep<INSTANCE, FACTORY>(object, factory, ba);
}

template <class INSTANCE, class FACTORY>
inline
my_CountedHandle<INSTANCE, FACTORY>::my_CountedHandle(
                           const my_CountedHandle<INSTANCE, FACTORY>& other)
: d_rep_p(other.d_rep_p)
{
    if (d_rep_p) {
        d_rep_p->increment();
    }
}

template <class INSTANCE, class FACTORY>
inline
my_CountedHandle<INSTANCE, FACTORY>::~my_CountedHandle()
{
    if (d_rep_p && 0 == d_rep_p->decrement()) {
        my_CountedHandleRep<INSTANCE, FACTORY>::deleteObject(d_rep_p);
    }
}

template <class INSTANCE, class FACTORY>
inline
INSTANCE *my_CountedHandle<INSTANCE, FACTORY>::operator->() const
{
    return d_rep_p->d_instance_p;
}

template <class INSTANCE, class FACTORY>
inline
int my_CountedHandle<INSTANCE, FACTORY>::numReferences() const
{
    return d_rep_p ? bces_AtomicUtil::getInt(d_rep_p->d_count) : 0;
}

// EXAMPLE 3
template <class TYPE>
class my_PtrStack {
    struct Node {
        TYPE                 *d_item;
        Node                 *d_next;
        bces_AtomicUtil::Int d_dirtyBit;
    };
    bces_AtomicPointer<Node>  d_list_p;
    bces_AtomicPointer<Node>  d_freeList_p;
    bslma_Allocator *d_allocator_p;
    Node *allocateNode();
    void freeNode(Node *node);
  public:
    my_PtrStack(bslma_Allocator *allocator=0);
   ~my_PtrStack();
    void push(TYPE* item);
    TYPE *pop();
};

template <class TYPE>
inline my_PtrStack<TYPE>::my_PtrStack(bslma_Allocator *allocator)
: d_allocator_p(allocator)
{
}

template <class TYPE>
inline my_PtrStack<TYPE>::~my_PtrStack()
{
}

template <class TYPE>
inline
typename my_PtrStack<TYPE>::Node* my_PtrStack<TYPE>::allocateNode()
{
    Node *node;
    do {
        node = d_freeList_p;
        if (!node) {
            break;
        }
        if (bces_AtomicUtil::swapInt(&node->d_dirtyBit, 1)) {
            continue;
        }
        if (d_freeList_p.testAndSwap(node, node->d_next) == node) {
            break;
        }
        bces_AtomicUtil::setInt(&node->d_dirtyBit, 0);
    } while (1);
    if (!node) {
        bslma_Allocator *ba = bslma_Default::allocator(d_allocator_p);
        node = new(*ba) Node();
        bces_AtomicUtil::setInt(&node->d_dirtyBit, 1);
    }
    return node;
}

template <class TYPE>
inline void my_PtrStack<TYPE>::freeNode(Node *node)
{
    if (!node) {
        return;
    }

    do {
        node->d_next = d_freeList_p;
        if (d_freeList_p.testAndSwap(node->d_next, node) == node->d_next) {
            break;
        }
    } while(1);
    bces_AtomicUtil::setInt(&node->d_dirtyBit, 0);
}

template <class TYPE>
inline void my_PtrStack<TYPE>::push( TYPE* item )
{
    Node *node = allocateNode();
    node->d_item = item;
    do {
        node->d_next = d_list_p;
        if (d_list_p.testAndSwap(node->d_next, node) == node->d_next) {
            break;
        }
    } while(1);
    bces_AtomicUtil::setInt(&node->d_dirtyBit, 0);
}

template <class TYPE>
inline TYPE *my_PtrStack<TYPE>::pop()
{
    Node *node;
    do {
        node = d_list_p;
        if (!node) {
            break;
        }

        if (bces_AtomicUtil::swapInt(&node->d_dirtyBit, 1)) {
            continue;
        }

        if (d_list_p.testAndSwap(node, node->d_next) == node) {
            break;
        }

        bces_AtomicUtil::setInt(&node->d_dirtyBit, 0);
    } while (1);

    TYPE *item = node ? node->d_item : 0;
    if (node)
        freeNode(node);
    return item;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 9: {
        // --------------------------------------------------------------------
        // TESTING MEMORY ORDERING GUARANTEES OF ATOMIC OPERATIONS
        //
        // Concerns:
        //   Atomic operations with specific memory ordering guarantees do
        //   indeed implement those memory orderings.
        //
        // Plan:
        //  1. Implement a classical lock free algorithm, the Peterson's lock,
        //     which has been studied well enough to know the exact atomic
        //     operations it needs.
        //  2. Construct a test function which will read and write to some
        //     shared data protected by the Peterson's lock, and verified the
        //     consistency of the shared data.
        //  3. Start two threads, execute the test function in a loop in both
        //     threads and verify that the consistency of the shared data is
        //     not violated.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting memory ordering guarantees"
                          << "\n=================================="
                          << endl;

        if (verbose) cout << "\nTesting sequencial consistency" << endl;

        testCaseMemOrder<PetersonsLockSeqCst>();

      } break;
      case 8: {
        // TESTING USAGE Examples
        //
        // Plan:
        //
        // Testing:
        {
            my_PtrStack<int> stack;
        }
        {
            my_CountedHandle<double, bslma_Allocator> handle;
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING Spinlocks
        //   Test basic spinlock behavior.
        //
        // Plan:
        //   Performing basic non-threaded tests to assert that the lock,
        //   unlock, and trylock operations are working correctly.
        //
        // Testing:
        //   initSpinLock(bces_AtomicUtil::SpinLock *aSpin);
        //   spinLock(bces_AtomicUtil::SpinLock *aSpin);
        //   spinTryLock(bces_AtomicUtil::SpinLock *aSpin, int retries);
        //   spinUnlock(bces_AtomicUtil::SpinLock *aSpin);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Spinlocks"
                          << "\n================="
                          << endl;
        {
            int result;
            SL s;

            s.lock();

            result = s.tryLock(100);
            ASSERT(0 != result);

            s.unlock();

            result = s.tryLock(1);
            ASSERT(0 == result);
            result = s.tryLock(1);
            ASSERT(0 != result);

            s.unlock();
        }
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
        //  bsls_PlatformUtil::Int64 operator ++();
        //  bsls_PlatformUtil::Int64 operator ++(int);
        //  bsls_PlatformUtil::Int64 operator --();
        //  bsls_PlatformUtil::Int64 operator --(int);
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
                //line value                expected
                //---- -------------------  ---------------------
                { L_,   0                  , 1                    },
                { L_,   1                  , 2                    },
                { L_,  -1LL                , 0                    },
                { L_,   0xFFFFFFFFLL       , 0x100000000LL        },
                { L_,  0xFFFFFFFFFFFFFFFFLL , 0                   }
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
                //line expected             value
                //---- -------------------  ---------------------
                { L_,   0                  , 1                    },
                { L_,   1                  , 2                    },
                { L_,  -1LL                , 0                    },
                { L_,   0xFFFFFFFFLL       , 0x100000000LL        },
                { L_,  0xFFFFFFFFFFFFFFFFLL , 0                   }
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
        //   bsls_PlatformUtil::Int64 swap(bsls_PlatformUtil::Int64 swapValue);
        //   bsls_PlatformUtil::Int64 testAndSwap(bsls_PlatformUtil::Int64 ...
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
                //line value swapValue    cmpValue expValue      expResult
                //---- ----- ------------ -------- ------------- ---------
                { L_,   0   , 11         , 33     , 0          , 0       },
                { L_,   1   , 19         , 1      , 19         , 1       },
                { L_,  -1   , 4          , 1      , -1         , -1      },
                { L_,   2   , 0xFFFFFFFF , 2      , 0xFFFFFFFF , 2       },
                { L_,  -2   , 16         , 0      , -2         , -2      }
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
        //   bsls_PlatformUtil::Int64 add(bsls_PlatformUtil::Int64 value);
        //   void operator +=(bsls_PlatformUtil::Int64 value);
        //   void operator -=(bsls_PlatformUtil::Int64 value);
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
                //line d_base    d_amount d_expected
                //---- --------  -------- ----------
                { L_,   0       , -9    , -9         },
                { L_,   1       , 0     , 1          },
                { L_,  -1       , 1     , 0          },
                { L_, 0xFFFFFFFF, 1     , 0          },
                { L_,  -2       , -2    , -4         }
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
        //   bces_AtomicInt(const bces_AtomicInt& rhs);
        //   bces_AtomicInt(int value);
        //   bces_AtomicInt& operator= (const bces_AtomicInt& rhs);
        //   bces_AtomicInt64(const bces_AtomicInt64& original);
        //   bces_AtomicInt64(bsls_PlatformUtil::Int64 value);
        //   bces_AtomicInt64& operator= (const bces_AtomicInt64& rhs);
        //   bces_AtomicPointer(const bces_AtomicPointer<T>& original);
        //   bces_AtomicPointer(const T* value);
        //   bces_AtomicPointer<T>& operator=(const bces_AtomicPointer<T>&);
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

                AI x(VAL);  const AI& X = x;
                AI y(X);    const AI& Y = y;
                AI z;       const AI& Z = z;

                z = X;
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

                AI64 x(VAL); const AI64& X = x;
                AI64 y(X);   const AI64& Y = y;
                AI64 z;      const AI64& Z = z;

                z = X;
                if (veryVerbose) {
                    T_(); P_(X); P_(Y); P_(Z); P(VAL);
                }
                LOOP_ASSERT(i, VAL == X);
                LOOP_ASSERT(i, VAL == Y);
                LOOP_ASSERT(i, VAL == Z);
            }
        }

        if (verbose) cout <<
                          "\nTesting 'bces_AtomicPointer' Primary Manipulators"
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

                AP x(VAL); const AP& X = x;
                AP y(X);   const AP& Y = y;
                AP z;      const AP& Z = z;

                z = X;
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
        //   'operator bsls_PlatformUtil::Int64', 'operator T*').
        //
        // Testing:
        //   bces_AtomicInt();
        //   ~bces_AtomicInt()
        //   bces_AtomicInt& operator= (int value);
        //   operator int() const;;
        //   bces_AtomicInt64();
        //   ~bces_AtomicInt64();
        //   bces_AtomicInt64& operator= (bsls_PlatformUtil::Int64 value);
        //   operator bsls_PlatformUtil::Int64() const;
        //   bces_AtomicPointer();
        //   bces_AtomicPointer<T>& operator= (const T *value);
        //   ~bces_AtomicPointer();
        //   T& operator*() const;
        //   T* operator->() const;
        //   operator T*() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulators"
                          << "\n============================" << endl;

        if (verbose) cout << "\nTesting 'bces_AtomicInt' Primary Manipulators"
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
            cout << "\nTesting 'bces_AtomicInt64' Primary Manipulators"
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

        if (verbose) cout <<
                          "\nTesting 'bces_AtomicPointer' Primary Manipulators"
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

        mX1 = 0;
        ASSERT(0 == mX1);

        lresult = mX1.swap(XVC);
        ASSERT(0 == lresult);
        ASSERT(XVC == mX1);

        lresult = mX1.testAndSwap(XVA,XVB);
        ASSERT(XVC == lresult);
        ASSERT(XVC == mX1);

        lresult = mX1.testAndSwap(XVC,0);
        ASSERT(XVC == lresult);
        ASSERT(0 == mX1);

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

        // Spin Locks
        // ----------

        if (veryVerbose) cout << endl
                              << "\tSpin Locks" << endl
                              << "\t----------" << endl;

        SL mS1;

        mS1.lock();

        lresult = mS1.tryLock(100);
        ASSERT(0 != lresult);

        mS1.unlock();

        lresult = mS1.tryLock(1);
        ASSERT(0 == lresult);

        mS1.unlock();
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
