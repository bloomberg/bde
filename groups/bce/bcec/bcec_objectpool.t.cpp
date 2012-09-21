// bcec_objectpool.t.cpp      -*-C++-*-

#include <bcec_objectpool.h>

#include <bcec_fixedqueue.h>
#include <bcema_testallocator.h>
#include <bcema_sharedptr.h>
#include <bcemt_barrier.h>
#include <bcemt_lockguard.h>
#include <bcemt_threadgroup.h>
#include <bces_atomictypes.h>
#include <bdef_bind.h>
#include <bdef_function.h>
#include <bdef_placeholder.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bsls_alignmentfromtype.h>
#include <bsls_platform.h>
#include <bsls_timeutil.h>

#include <bsl_iostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// Testing bcec_ObjectPool is divided into 2 parts (apart from breathing test).
//
// (1) Testing functionality in presence of single thread.  Specifically
//     we make sure that:
//
//     o the pool is replenished with the correct number of objects
//       when it is depleted.
//
//     o enough memory is allocated, objects are correctly laid out in
//       the allocated memory and are properly constructed.
//
//     o in the presence of exception, the pool is always left in a valid
//       state.
//
//     o on the destruction of the pool, all objects are destroyed and
//       the memory is reclaimed.
//
//     All above are tested in [ 2] and [11].
//
// (2) Testing functionality in presence of multiple threads.  This ensures
//     that the pool remains consistent in the presence of multiple threads.
//     Specifically we make sure that:
//
//      o concurrent access to underlying free object list do not
//        introduce inconsistencies.  This is tested in [ 3], [ 4],
//        [ 5] and [ 6].
//
//      o concurrent access to 'd_numObjects' do not introduce
//        inconsistencies.  This is tested in [ 7].
//
//      o concurrent access to 'd_numAvailableObjects' do not introduce
//        inconsistencies.  This is tested in [ 8].
//
//      o once an object has been released, it can be used by the
//        pool to satisfy further object requests (potentially from other
//        threads).  This is tested in [ 9].
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] bcec_ObjectPool(objectCreator, bslma_Allocator);
// [ 2] bcec_ObjectPool(objectCreator, numObjects, bslma_Allocator);
// [ 2] ~bcec_ObjectPool();
//
// MANIPULATORS
// [ 2] TYPE *getObject();
// [ 8] void increaseCapacity(int numObjects);
// [ 9] void releaseObject(TYPE *objPtr);
// [ 1] void reserveCapacity(int numObjects);
//
// ACCESSORS
// [ 8] int numAvailableObjects() const;
// [ 7] int numObjects() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] Verify concurrent access to underlying free object list.
// [ 4] Verify concurrent access to underlying free object list.
// [ 5] Verify concurrent access to underlying free object list.
// [ 6] Verify concurrent access to underlying free object list.
// [10] USAGE EXAMPLE
//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
   static bcemt_Mutex *mutex = 0;
   if (0 == mutex) {
      mutex = new bcemt_Mutex;
   }

    if (c) {
        bcemt_LockGuard<bcemt_Mutex> guard(mutex);
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << #M << ": " <<  \
       M << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline
//=============================================================================
//                    THREAD-SAFE OUTPUT AND ASSERT MACROS
//-----------------------------------------------------------------------------

typedef bcemt_LockGuard<bcemt_Mutex> LockGuard;
static bcemt_Mutex printMutex;  // mutex to protect output macros

#define PT(X)  { LockGuard guard(&printMutex); P(X);  }
#define PT_(X) { LockGuard guard(&printMutex); P_(X); }

static bcemt_Mutex assertMutex; // mutex to protect assert macros

#define LOOP_ASSERTT(I,X) { \
   if (!(X)) {                                                                \
       LockGuard guard(&assertMutex);                                         \
       cout << #I << ": " << I << "\n";                                       \
       aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERTT(I,J,X) {                                                \
   if (!(X)) { LockGuard guard(&assertMutex);                                 \
       cout << #I << ": " << I << "\t"                                        \
       << #J << ": " << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERTT(I,J,K,X) { \
   if (!(X)) { LockGuard guard(&assertMutex); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << "\t" << #K << ": " << K << "\n";                 \
       aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERTT(I,J,K,L,X) { \
   if (!(X)) { LockGuard guard(&assertMutex); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << "\t" << #K << ": " << K << "\t" << #L            \
       << ": " << L << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERTT(I,J,K,L,M,X) { \
   if (!(X)) { LockGuard guard(&assertMutex); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << "\t" << #K << ": " << K << "\t" << #L            \
       << ": " << L << "\t" << #M << ": " << M << "\n";                       \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//          GLOBAL TYPEDEFS/CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

bcemt_Attribute attributes;
void executeInParallel(int numThreads, bcemt_ThreadUtil::ThreadFunction func)
   // Create the specified 'numThreads', each executing the specified 'func'.
   // Number each thread (sequentially from 0 to 'numThreads-1') by passing i
   // to i'th thread.  Finally join all the threads.
{
    bcemt_ThreadUtil::Handle *threads =
                               new bcemt_ThreadUtil::Handle[numThreads];
    ASSERT(threads);

    for (int i = 0; i < numThreads; ++i) {
        bcemt_ThreadUtil::create(&threads[i], attributes, func, (void*)i);
    }
    for (int i = 0; i < numThreads; ++i) {
        bcemt_ThreadUtil::join(threads[i]);
    }

    delete [] threads;
}
//=============================================================================
//                   HELPER CLASSES AND FUNCTIONS FOR TESTING
//=============================================================================

struct ConstructorTestHelp3
{
   // PUBLIC DATA
   bslma_Allocator     *d_allocator_p;
   int d_resetCount;
   int d_startCount;

   // CREATORS
   ConstructorTestHelp3(int startCount, bslma_Allocator *basicAllocator=0)
      : d_allocator_p(basicAllocator)
      , d_resetCount(0)
      , d_startCount(startCount)
   {}


   // ACCESSORS
   static void resetWithCount(ConstructorTestHelp3 *self, int count)
   { self->d_resetCount = count; }
};

struct ConstructorTestHelp3Creator
{
   int d_count;

   ConstructorTestHelp3Creator(int count = -1)
      : d_count(count)
   {}

   void operator()(void* arena, bslma_Allocator* alloc)
   {
      new(arena) ConstructorTestHelp3(d_count, alloc);
   }
};

void constructor4(int count, void* arena)
{
   new(arena) ConstructorTestHelp3(count);
}

void constructor5(int count, void* arena, bslma_Allocator* basicAllocator)
{
   new(arena) ConstructorTestHelp3(count, basicAllocator);
}

struct ConstructorTestHelp1a
{
   // PUBLIC DATA
   bslma_Allocator      *d_allocator_p; //held
   int                   d_resetCount;

   // CREATORS
   ConstructorTestHelp1a(bslma_Allocator *basicAllocator=0)
      : d_allocator_p(basicAllocator)
      , d_resetCount(0)
   {}

   // TRAITS
   BSLALG_DECLARE_NESTED_TRAITS(ConstructorTestHelp1a,
                                bslalg_TypeTraitUsesBslmaAllocator);

   // ACCESSORS
   void reset();
   static void resetWithCount(ConstructorTestHelp1a *self, int count);
};

struct ConstructorTestHelp1b
{
   // PUBLIC DATA
   int                   d_resetCount;

   ConstructorTestHelp1b()
      : d_resetCount(0)
   {}

   // ACCESSORS
   void reset();
   static void resetWithCount(ConstructorTestHelp1b *self, int count);
};

struct ConstructorTestHelp1aCreator
{
   int d_count;

   ConstructorTestHelp1aCreator(int count = -1001)
      : d_count(count)
   {}

   void operator()(void* arena, bslma_Allocator* alloc)
   {
      new(arena) ConstructorTestHelp1a(alloc);
      ((ConstructorTestHelp1a*)arena)->d_resetCount = d_count;
   }
};

void createConstructorTestHelp1a(void* arena, bslma_Allocator* alloc)
{
    new(arena) ConstructorTestHelp1a(alloc);
}

int creatorCount1a = 0;

void createConstructorTestHelp1b(void* arena, bslma_Allocator*)
{
    new(arena) ConstructorTestHelp1b;
}


struct ConstructorTestHelp1aCreator2
{
   int d_count;

   ConstructorTestHelp1aCreator2(int count = -2001)
      : d_count(count)
   {}

   void operator()(void* arena, bslma_Allocator*)
   {
      new(arena) ConstructorTestHelp1a;
      ((ConstructorTestHelp1a*)arena)->d_resetCount = d_count;
   }
};

void ConstructorTestHelp1a::reset()
{++d_resetCount;}

void ConstructorTestHelp1b::reset()
{++d_resetCount;}

void ConstructorTestHelp1a::resetWithCount(ConstructorTestHelp1a *self, int c)
{
   self->d_resetCount = c;
}

void ConstructorTestHelp1b::resetWithCount(ConstructorTestHelp1b *self, int c)
{
   self->d_resetCount = c;
}

   struct Address {
      int d_taskNum;
      int d_machNum;
   };

   struct UsesAllocatorType {
       bslma_Allocator      *d_allocator_p;

       // CREATORS
       UsesAllocatorType(bslma_Allocator *basicAllocator=0)
           : d_allocator_p(basicAllocator)
       {}

       // TRAITS
       BSLALG_DECLARE_NESTED_TRAITS(UsesAllocatorType,
                                    bslalg_TypeTraitUsesBslmaAllocator);
   };


   struct OtherType {
       int d_data;

       OtherType() : d_data(999) {}
   };

   class Case13Type {
      Address                      d_address;
      bdef_Function<void(*)(int*)> d_callback;
      bcema_SharedPtr<int>         d_sp1;
      int                          d_offset;
      bcema_SharedPtr<int>         d_sp2;
      int                          d_bytesLeft;
      bces_AtomicInt               d_state;
      int                          d_index;
      bces_AtomicInt               d_count;

   public:
      Case13Type()
         : d_offset(0), d_state(2), d_count(0), d_index(0) {}

      ~Case13Type() {
         ASSERT(2 == d_state.testAndSwap(2, -2));
      }

      bool valid() {
         return 2 == d_state;
      }

      void reset() {
         ASSERT(2 == d_state.testAndSwap(2, -2));
         d_offset = 0;
         d_index = 0;
         d_count = 0;
         ASSERT(-2 == d_state.testAndSwap(-2, 2));
      }


      void increment() {
         // Verify that no other thread is using this object right now
         // (otherwise the object pool allowed the same object to be
         // available to two threads).  This also serves to delay the object's
         // return to the pool.

         int val = d_count;

#ifdef BSLS_PLATFORM__OS_LINUX
         // when running test drivers on linux, this yield() tends to consume
         // a lot more time than we want.  So just microSleep(1) instead,
         // which is a much smaller delay (but quite a bit more than 1
         // microsecond).
         bcemt_ThreadUtil::microSleep(1);
#else
         bcemt_ThreadUtil::yield();
#endif

         ASSERT(val == d_count.testAndSwap(val, val+1));
      }

      int count() {
         return d_count;
      }
   };

   inline void createCase13(void *address, bslma_Allocator *,
                            bces_AtomicInt *created) {
      new (address) Case13Type;
      ++(*created);
   }

   void case13Thread(bcec_ObjectPool<Case13Type> *mX,
                     bcec_FixedQueue<Case13Type*> *queue,
                     int numIterations) {
      for (int i = 0; i < numIterations; ++i) {
         Case13Type *obj;
         while (1) {
            obj = mX->getObject();
            if (0 == queue->tryPushBack(obj)) {
                break;
            }
            else {
                if (veryVerbose) {
                    printMutex.lock();
                    cout << "Spinning: " << i << endl;
                    printMutex.unlock();
                }
                obj->reset();
                mX->releaseObject(obj);
            }
         }
      }
   }

void case13Processor(bcec_ObjectPool<Case13Type> *mX,
                     bcec_FixedQueue<Case13Type*> *queue,
                     bces_AtomicInt          *done)
{
   while (1) {
      Case13Type* obj;
      if (0 == queue->tryPopFront(&obj)) {
         obj->increment();
         mX->releaseObject(obj);
      }
      else if (*done) {
          break;
      }
      else {
          bcemt_ThreadUtil::microSleep(500); //0.5 ms
      }
   }
}

}


//                         CASE 12 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BCEC_OBJECTPOOL_TEST_CASE_12
{

const char *DEFAULT_STRING_INIT = "A default string, which is larger than what can fit "
                                  "into the string short buffer";

static void badCreateString(void *address)
    // Create a 'str::string' object at the specified 'address' in an initial
    // state equal to the 'DEFAULT_STRING_INIT' string.
{
    new(address) bsl::string(DEFAULT_STRING_INIT);
}

static void createString(void               *address,
                         const bsl::string&  initial,
                         bslma_Allocator    *allocator)
    // Create a 'str::string' object at the specified 'address' in an initial
    // state equal to the specified 'default' string.  Use the specified
    // 'allocator' to supply memory.  If 'allocator' is 0, the currently
    // installed default allocator is used.
{
    new(address) bsl::string(initial, allocator);
}

} // namespace BCEC_OBJECTPOOL_TEST_CASE_12
//                         CASE 11 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BCEC_OBJECTPOOL_TEST_CASE_11
{
struct Exception {};

class A
{
    int             *d_value_p;
    bslma_Allocator *d_alloc_p;

  public:
    static int constructorCount;
    static int destructorCount;

    A(bslma_Allocator *alloc = 0)
    : d_value_p(0)
    , d_alloc_p(bslma_Default::allocator(alloc))
    {
        d_value_p = (int *) d_alloc_p->allocate(sizeof(int));
        *d_value_p = 1;
        ++constructorCount;
    }

    ~A()
    {
        *d_value_p = 0;
        d_alloc_p->deallocate(d_value_p);
        ++destructorCount;
    }
};

int A::constructorCount = 0;
int A::destructorCount = 0;

static int createBThrow = 1;
    // Countdown for throwing an exception in B's constructor.  If 0, no
    // exception will be thrown.  Otherwise, this global variable will be
    // decremented in the destructor and an exception thrown when it reaches 0.

class B
{
    A                d_a;
    int             *d_value_p;
    bslma_Allocator *d_alloc_p;

public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(B, bslalg::TypeTraitUsesBslmaAllocator);

    static int constructorCount;
    static int destructorCount;

    B(bslma_Allocator *alloc = 0)
    : d_a(alloc)
    , d_value_p(0)
    , d_alloc_p(bslma_Default::allocator(alloc))
    {
        if (createBThrow && 0 == --createBThrow) {
            throw Exception();   // destroys 'd_a'
        }
        d_value_p = (int *) d_alloc_p->allocate(sizeof(int));
        *d_value_p = 1;
        ++constructorCount;
    }

    ~B()
    {
        *d_value_p = 0;
        d_alloc_p->deallocate(d_value_p);
        ++destructorCount;
    }
};

int B::constructorCount = 0;
int B::destructorCount = 0;

} // namespace BCEC_OBJECTPOOL_TEST_CASE_11

//=============================================================================
//                         CASE 10 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BCEC_OBJECTPOOL_TEST_CASE_10
{
class A
{
  public:
    static int constructorCount;
    static int destructorCount;

    A()
    {
        ++constructorCount;
    }

    ~A()
    {
        ++destructorCount;
    }
};

int A:: constructorCount = 0;
int A:: destructorCount = 0;

} // namespace BCEC_OBJECTPOOL_TEST_CASE_10
//=============================================================================
//                         CASE 9 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BCEC_OBJECTPOOL_TEST_CASE_9
{

enum {
    NUM_THREADS    = 2,
    NUM_OBJECTS    = NUM_THREADS,
    NUM_ITERATIONS = 100
};

class Counter
{
    int d_count;
public:
    Counter() : d_count(0)
    {
    }

    void increment()
    {
        ++d_count;
    }

    int count() const
    {
        return d_count;
    }
};

bcec_ObjectPool<Counter> *pool;

bcemt_Barrier barrier(NUM_THREADS);

#if !defined(BSLS_PLATFORM__CMP_SUN) \
    || BSLS_PLATFORM__CMP_VER_MAJOR >= 1360
extern "C"
    // This is a thread function and, thus, it must have extern "C" linkage.
    // Sun Workshop compilers, however, have a bug in that an extern "C"
    // function can't access template functions.
    // This was fixed in Sun Studio 8 compiler.
#endif

    void *workerThread9(void *arg)
    {
        barrier.wait();
        for(int i = 0; i < NUM_ITERATIONS; ++i){
            Counter *c = pool->getObject();
            c->increment();
            pool->releaseObject(c);
        }
        return NULL;
    }

} // namespace BCEC_OBJECTPOOL_TEST_CASE_9
//=============================================================================
//                         CASE 8 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BCEC_OBJECTPOOL_TEST_CASE_8
{

enum {
    NUM_THREADS    = 3,
    NUM_OBJECTS    = NUM_THREADS,
    NUM_ITERATIONS = 100
};

class my_Class
{
};

bcec_ObjectPool<my_Class> *pool;

bcemt_Barrier barrier(NUM_THREADS);

#if !defined(BSLS_PLATFORM__CMP_SUN) \
    || BSLS_PLATFORM__CMP_VER_MAJOR >= 1360
extern "C"
    // This is a thread function and, thus, it must have extern "C" linkage.
    // Sun Workshop compilers, however, have a bug in that an extern "C"
    // function can't access template functions.
    // This was fixed in Sun Studio 8 compiler.
#endif
    void *workerThread8(void *arg)
    {
        my_Class *p = pool->getObject();
        barrier.wait();
        for(int i = 0; i < NUM_ITERATIONS; ++i) {
            pool->increaseCapacity(1);
            pool->releaseObject(p);
            pool->getObject();
            p = pool->getObject();

            int nAvailable = pool->numAvailableObjects();
            LOOP2_ASSERTT(i, nAvailable, nAvailable <= 2 * (NUM_THREADS - 1));
            LOOP2_ASSERTT(i, nAvailable, nAvailable >= 0);
        }
        return NULL;
    }

} // namespace BCEC_OBJECTPOOL_TEST_CASE_8
//=============================================================================
//                          CASE 7 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BCEC_OBJECTPOOL_TEST_CASE_7
{

enum {
    NUM_THREADS    = 3,
    NUM_ITERATIONS = 100
};

class my_Class
{
};

bcec_ObjectPool<my_Class> *pool;

bcemt_Barrier barrier(NUM_THREADS);

#if !defined(BSLS_PLATFORM__CMP_SUN) \
    || BSLS_PLATFORM__CMP_VER_MAJOR >= 1360
extern "C"
    // This is a thread function and, thus, it must have extern "C" linkage.
    // Sun Workshop compilers, however, have a bug in that an extern "C"
    // function can't access template functions.
    // This was fixed in Sun Studio 8 compiler.
#endif
    void *workerThread7(void *arg)
    {
        int previous = 0, current;
        barrier.wait();
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            pool->increaseCapacity(1);
            current = pool->numObjects();
            LOOP3_ASSERTT(i, current, previous, current > previous);
            previous = current;
        }
        return NULL;
    }

} // namespace BCEC_OBJECTPOOL_TEST_CASE_7
//=============================================================================
//                         CASE 6 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BCEC_OBJECTPOOL_TEST_CASE_6
{

enum {
    NUM_THREADS = 8,     // must be multiple of four
    NUM_OBJECTS = 50
};

class my_Class
{
};

bcec_ObjectPool<my_Class> *pool;

bcemt_Barrier barrierAll(NUM_THREADS); // barrier for all threads

bcemt_Barrier barrier0(NUM_THREADS/4); // barrier for threads having
                                       // thread-number % 4 == 0

#if !defined(BSLS_PLATFORM__CMP_SUN) \
    || BSLS_PLATFORM__CMP_VER_MAJOR >= 1360
extern "C"
    // This is a thread function and, thus, it must have extern "C" linkage.
    // Sun Workshop compilers, however, have a bug in that an extern "C"
    // function can't access template functions.
    // This was fixed in Sun Studio 8 compiler.
#endif
    void *workerThread6(void *arg)
    {
        my_Class *arr[NUM_OBJECTS];
        int remainder = (bsls_PlatformUtil::IntPtr)arg % 4;

        // 0-order threads
        if (remainder == 0) {
            for(int i = 0; i < NUM_OBJECTS; ++i){
                arr[i] = pool->getObject();
            }

            barrierAll.wait();
            for(int i = 0; i < NUM_OBJECTS; ++i){
                pool->releaseObject(arr[i]);
            }
        }

        // 1-order threads
        else if (remainder == 1) {
            barrierAll.wait();
            for(int i = 0; i < NUM_OBJECTS; ++i){
                arr[i] = pool->getObject();
            }
        }

        // 2-order threads
        else if (remainder == 2) {
            barrierAll.wait();
            for(int i = 0; i < NUM_OBJECTS; ++i){
                pool->increaseCapacity(1);
            }
        }

        // 3-order threads
        else {
            barrierAll.wait();
            for(int i = 0; i < NUM_OBJECTS; ++i){
                pool->reserveCapacity(0);
            }
        }

        return NULL;
    }

void verifyPool(bcec_ObjectPool<my_Class> *pool)
    // Verify that 'numAvailableObjects' reports the actual number
    // of available objects.
{
    int numCreated = pool->numObjects();
    int numAvailable = pool->numAvailableObjects();
    my_Class **arr = new my_Class *[numAvailable];

    for (int i = 0; i < numAvailable; ++i) {
        arr[i] = pool->getObject();
    }
    int numCreated1 = pool->numObjects();
    int numAvailable1 = pool->numAvailableObjects();
    LOOP2_ASSERTT(numCreated, numCreated1, numCreated == numCreated1);
    LOOP_ASSERTT(numAvailable1, 0 == numAvailable1);


    for (int i = 0; i < numAvailable; ++i) {
        pool->releaseObject(arr[i]);
    }
    numCreated1 = pool->numObjects();
    numAvailable1 = pool->numAvailableObjects();
    LOOP2_ASSERTT(numCreated, numCreated1, numCreated == numCreated1);
    LOOP2_ASSERTT(numAvailable, numAvailable1, numAvailable == numAvailable1);

    delete []arr;
}

} // namespace BCEC_OBJECTPOOL_TEST_CASE_6
//=============================================================================
//                         CASE 5 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BCEC_OBJECTPOOL_TEST_CASE_5
{

enum {
    NUM_THREADS    = 3,
    NUM_OBJECTS    = NUM_THREADS,
    NUM_ITERATIONS = 100
};

class my_Class
{
};

bcec_ObjectPool<my_Class> *pool;

bcemt_Barrier barrier(NUM_THREADS);

#if !defined(BSLS_PLATFORM__CMP_SUN) \
    || BSLS_PLATFORM__CMP_VER_MAJOR >= 1360
extern "C"
    // This is a thread function and, thus, it must have extern "C" linkage.
    // Sun Workshop compilers, however, have a bug in that an extern "C"
    // function can't access template functions.
    // This was fixed in Sun Studio 8 compiler.
#endif

    void *workerThread5(void *arg)
    {
        barrier.wait();
        for(int i = 0; i < NUM_ITERATIONS; ++i){
            my_Class *p = pool->getObject();
            pool->releaseObject(p);
        }
        return NULL;
    }

} // namespace BCEC_OBJECTPOOL_TEST_CASE_5
//=============================================================================
//                         CASE 4 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BCEC_OBJECTPOOL_TEST_CASE_4
{

enum {
    NUM_THREADS = 4,      // must be even
    NUM_OBJECTS = 1000
};

class my_Class
{
};

bcec_ObjectPool<my_Class> *pool;

bcemt_Barrier barrierAll(NUM_THREADS);   // barrier for all threads
bcemt_Barrier barrier0(NUM_THREADS / 2); // barrier for even numbered threads

#if !defined(BSLS_PLATFORM__CMP_SUN) \
    || BSLS_PLATFORM__CMP_VER_MAJOR >= 1360
extern "C"
    // This is a thread function and, thus, it must have extern "C" linkage.
    // Sun Workshop compilers, however, have a bug in that an extern "C"
    // function can't access template functions.
    // This was fixed in Sun Studio 8 compiler.
#endif

    void *workerThread4(void *arg)
    {
        my_Class *arr[NUM_OBJECTS];
        int remainder = (bsls_PlatformUtil::IntPtr)arg % 2;

        // even numbered threads
        if (remainder == 0) {
            barrier0.wait();
            for(int i = 0; i < NUM_OBJECTS; ++i){
                arr[i] = pool->getObject();
            }

            barrier0.wait();
            if ((bsls_PlatformUtil::IntPtr)arg == 0) {
                int nC; // number of created objects
                int nA; // number of available objects
                nC = pool->numObjects();
                nA = pool->numAvailableObjects();
                LOOP_ASSERTT(nC, nC == NUM_THREADS * NUM_OBJECTS);
                LOOP_ASSERTT(nA, nA == NUM_THREADS * NUM_OBJECTS / 2);
            }

            barrierAll.wait();
            for(int i = 0; i < NUM_OBJECTS; ++i){
                pool->releaseObject(arr[i]);
            }
        }

        // odd numbered threads
        else {
            barrierAll.wait();
            for(int i = 0; i < NUM_OBJECTS; ++i){
                arr[i] = pool->getObject();
            }
        }
        return NULL;
    }

} // namespace BCEC_OBJECTPOOL_TEST_CASE_4
//=============================================================================
//                         CASE 3 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BCEC_OBJECTPOOL_TEST_CASE_3
{

enum {
    NUM_THREADS = 3,
    NUM_OBJECTS = 100
};

class my_Class
{
};

bcec_ObjectPool<my_Class> *pool;

bcemt_Barrier barrier(NUM_THREADS);

#if !defined(BSLS_PLATFORM__CMP_SUN) \
    || BSLS_PLATFORM__CMP_VER_MAJOR >= 1360
extern "C"
    // This is a thread function and, thus, it must have extern "C" linkage.
    // Sun Workshop compilers, however, have a bug in that an extern "C"
    // function can't access template functions.
    // This was fixed in Sun Studio 8 compiler.
#endif

    void *workerThread3(void *arg)
    {
      my_Class *arr[NUM_OBJECTS];

      barrier.wait();
      for(int i = 0; i < NUM_OBJECTS; ++i){
          arr[i] = pool->getObject();
      }

      barrier.wait();
      if ((bsls_PlatformUtil::IntPtr)arg == 0) {
        int nC; // number of created objects
        int nA; // number of available objects
        nC = pool->numObjects();
        nA = pool->numAvailableObjects();

        LOOP_ASSERTT(nC, nC == NUM_THREADS * NUM_OBJECTS);
        LOOP_ASSERTT(nA, nA == 0);
      }

      barrier.wait();
      for(int i = 0; i < NUM_OBJECTS; ++i){
          pool->releaseObject(arr[i]);
      }
      return NULL;
    }

} // namespace BCEC_OBJECTPOOL_TEST_CASE_3
//=============================================================================
//                         CASE 2 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BCEC_OBJECTPOOL_TEST_CASE_2
{

class my_CheckingClass
{
    int d_pattern;
  public:
    enum { PATTERN = 0x33333333 };
    static int objCount;

    // CREATORS
    my_CheckingClass()
    {
        d_pattern = PATTERN;
        objCount++;
    }

    ~my_CheckingClass()
    {
        objCount--;
    }

    // ACCESSORS
    int pattern() const
    {
        return d_pattern;
    }
};
int my_CheckingClass::objCount = 0;

union ObjectNode {
    struct {
        ObjectNode                *d_next_p;
        bces_AtomicUtil::Int       d_refCount;
    } d_inUse;
    bsls_AlignmentFromType<my_CheckingClass>::Type d_dummy;
};

union BlockNode {
    struct {
        BlockNode *d_next_p;
        int        d_numObjects;  // number of objects in this block
    } d_inUse;
    bsls_AlignmentFromType<ObjectNode>::Type d_dummy;
};

enum {
    ROUNDED_OBJECT_SIZE  =
        ((sizeof(my_CheckingClass) / sizeof(ObjectNode)       ) +
        ((sizeof(my_CheckingClass) % sizeof(ObjectNode)) > 0) ) *
        sizeof(ObjectNode),

    OBJECT_FRAME_SIZE    = sizeof(ObjectNode) + ROUNDED_OBJECT_SIZE
};

} // namespace BCEC_OBJECTPOOL_TEST_CASE_2
//=============================================================================
//                         CASE 1 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace BCEC_OBJECTPOOL_TEST_CASE_1
{

class my_Class
{
};

} // namespace BCEC_OBJECTPOOL_TEST_CASE_1
//=============================================================================
//                         USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace BCEC_OBJECTPOOL_TEST_USAGE_EXAMPLE
{

enum {
    CONNECTION_OPEN_TIME  = 100,    // (simulated) time to open
                                    //  a connection (in microseconds)

    CONNECTION_CLOSE_TIME = 8,      // (simulated) time to close
                                    //  a connection (in microseconds)

    QUERY_EXECUTION_TIME  = 4       // (simulated) time to execute
                                    //  a query (in microseconds)
};

class Query
    // Dummy query class.
{
};

class QueryFactory
    // Dummy query factory class.
{
  public:
    // MANIPULATORS
    Query *createQuery()
        // Return a dummy query.
    {
        return (Query *)NULL;
    }

    void destroyQuery(Query *query)
        // Simulate query destruction.
    {
    }
} *queryFactory;

class my_DatabaseConnection
    // This class simulates a database connection.
{
  public:
    my_DatabaseConnection()
    {
        bcemt_ThreadUtil::microSleep(CONNECTION_OPEN_TIME);
    }

    ~my_DatabaseConnection()
    {
        bcemt_ThreadUtil::microSleep(CONNECTION_CLOSE_TIME);
    }

    void executeQuery(Query *query)
    {
        bcemt_ThreadUtil::microSleep(QUERY_EXECUTION_TIME);
    }
};

bcec_ObjectPool<my_DatabaseConnection> *connectionPool;
bces_AtomicInt64 totalResponseTime1; // total response time when
                                     // we do not use object pool

bces_AtomicInt64 totalResponseTime2; // total response time when
                                     // we use object pool

#if !defined(BSLS_PLATFORM__CMP_SUN) \
    || BSLS_PLATFORM__CMP_VER_MAJOR >= 1360
extern "C"
    // This is a thread function and, thus, it must have extern "C" linkage.
    // Sun Workshop compilers, however, have a bug in that an extern "C"
    // function can't access template functions.
    // This was fixed in Sun Studio 8 compiler.
#endif

void serverThread(bces_AtomicInt* queries, int max,
                  void(*queryHandler)(Query*))
{
    while (++(*queries) <= max) {
        Query* query = queryFactory->createQuery();
        queryHandler(query);
    }
}

void queryHandler1(Query *query)
    // Handle the specified 'query' without using an objectpool.
{
    bsls_PlatformUtil::Int64 t1 = bsls_TimeUtil::getTimer();
    my_DatabaseConnection connection;
    connection.executeQuery(query);
    bsls_PlatformUtil::Int64 t2 = bsls_TimeUtil::getTimer();

    totalResponseTime1 += t2 - t1;

    queryFactory->destroyQuery(query);
}

#if !defined(BSLS_PLATFORM__CMP_SUN) \
    || BSLS_PLATFORM__CMP_VER_MAJOR >= 1360
extern "C"
    // This is a thread function and, thus, it must have extern "C" linkage.
    // Sun Workshop compilers, however, have a bug in that an extern "C"
    // function can't access template functions.
    // This was fixed in Sun Studio 8 compiler.
#endif
void queryHandler2(Query *query)
        // Handle the specified 'query' using an objectpool.
{
    bsls_PlatformUtil::Int64 t1 = bsls_TimeUtil::getTimer();
    my_DatabaseConnection *connection = connectionPool->getObject();
    connection->executeQuery(query);
    bsls_PlatformUtil::Int64 t2 = bsls_TimeUtil::getTimer();

    totalResponseTime2 += t2 - t1;

    connectionPool->releaseObject(connection);
    queryFactory->destroyQuery(query);
}

} // namespace BCEC_OBJECTPOOL_TEST_USAGE_EXAMPLE

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    using namespace bdef_PlaceHolders;

    switch (test) { case 0:  // Zero is always the leading case.
      case 17: {
        /////////////////////////////////////////////////////////
        // bcema_Factory test
        //
        // Concern: the pool works correctly as a bcema_Factory
        /////////////////////////////////////////////////////////
         if (verbose) {
            cout << "bcema_Factory test" << endl;
         }

         bcema_TestAllocator ta;

         bcec_ObjectPool<ConstructorTestHelp1a,
                   bcec_ObjectPoolFunctors::DefaultCreator,
                   bcec_ObjectPoolFunctors::Reset<ConstructorTestHelp1a> >
             concretePool1a(1, &ta);
         bcema_Factory<ConstructorTestHelp1a>* pool1a = &concretePool1a;

         ConstructorTestHelp1a* ptr1a = pool1a->createObject();
         pool1a->deleteObject(ptr1a);

         ASSERT(&ta == ptr1a->d_allocator_p);
         ASSERT(1 == ptr1a->d_resetCount);
      } break;

      case 16: {
           //////////////////////////////////////////////////////
           // Constructor overloads
           //
           // Concern: The various overloads of the constructor cause
           // the correct creator and resetter functions to be called.
           //
           //////////////////////////////////////////////////////

         using namespace bdef_PlaceHolders;
         if (verbose) {
            cout << "Constructor Overloads test" << endl;
         }

         bcema_TestAllocator ta;

         bcec_ObjectPool<ConstructorTestHelp1a,
                   bcec_ObjectPoolFunctors::DefaultCreator,
                   bcec_ObjectPoolFunctors::Reset<ConstructorTestHelp1a> >
                                                                pool1a(1, &ta);
         ConstructorTestHelp1a* ptr1a = pool1a.getObject();
         pool1a.releaseObject(ptr1a);

         ASSERT(&ta == ptr1a->d_allocator_p);
         ASSERT(1 == ptr1a->d_resetCount);

         bcec_ObjectPool<
             ConstructorTestHelp1a,
             ConstructorTestHelp1aCreator,
             bcec_ObjectPoolFunctors::Reset<ConstructorTestHelp1a> >
             pool1a2(1, &ta);

         ptr1a = pool1a2.getObject();
         pool1a2.releaseObject(ptr1a);

         ASSERT(&ta == ptr1a->d_allocator_p);
         LOOP_ASSERT(ptr1a->d_resetCount,
                     -1000 == ptr1a->d_resetCount);

         bcec_ObjectPool<
             ConstructorTestHelp1a,
             ConstructorTestHelp1aCreator,
             bcec_ObjectPoolFunctors::Reset<ConstructorTestHelp1a> >
             pool1a2b(ConstructorTestHelp1aCreator(499));

         ptr1a = pool1a2b.getObject();
         pool1a2b.releaseObject(ptr1a);

         ASSERT(bslma_Default::allocator() == ptr1a->d_allocator_p);
         LOOP_ASSERT(ptr1a->d_resetCount,
                     500 == ptr1a->d_resetCount);

         bcec_ObjectPool<
             ConstructorTestHelp1a,
             ConstructorTestHelp1aCreator,
             bcec_ObjectPoolFunctors::Reset<ConstructorTestHelp1a> >
             pool1a2a(1);

         ptr1a = pool1a2a.getObject();
         pool1a2a.releaseObject(ptr1a);

         ASSERT(bslma_Default::allocator() == ptr1a->d_allocator_p);
         LOOP_ASSERT(ptr1a->d_resetCount,
                     -1000 == ptr1a->d_resetCount);

         bcec_ObjectPool<
             ConstructorTestHelp1a,
             ConstructorTestHelp1aCreator2,
             bcec_ObjectPoolFunctors::Reset<ConstructorTestHelp1a> >
                pool1a3(1, &ta);

         ptr1a = pool1a3.getObject();
         pool1a3.releaseObject(ptr1a);

         ASSERT(0 == ptr1a->d_allocator_p);
         LOOP_ASSERT(ptr1a->d_resetCount,
                     -2000 == ptr1a->d_resetCount);

         bcec_ObjectPool<ConstructorTestHelp1b,
                         bcec_ObjectPoolFunctors::DefaultCreator,
                         bcec_ObjectPoolFunctors::
                                                 Reset<ConstructorTestHelp1b> >
                                                                pool1b(1, &ta);
         ConstructorTestHelp1b* ptr1b  = pool1b.getObject();
         pool1b.releaseObject(ptr1b);

         ASSERT(1 == ptr1b->d_resetCount);

         bcec_ObjectPool<ConstructorTestHelp1a,
             bcec_ObjectPoolFunctors::DefaultCreator,
             bcec_ObjectPoolFunctors::Reset<ConstructorTestHelp1a> > pool2a(
                    &createConstructorTestHelp1a, 1, &ta);
         ConstructorTestHelp1a* ptr2a = pool2a.getObject();
         pool2a.releaseObject(ptr2a);

         ASSERT(&ta == ptr2a->d_allocator_p);
         ASSERT(1 == ptr2a->d_resetCount);

         bcec_ObjectPool<ConstructorTestHelp1b,
                         bcec_ObjectPoolFunctors::DefaultCreator,
                         bdef_Function<void(*)(ConstructorTestHelp1b*)> >
                         pool2b(&createConstructorTestHelp1b,
                                bdef_BindUtil::bind(
                                        &ConstructorTestHelp1b::resetWithCount,
                                        _1,
                                        200),
                                1,
                                &ta);
         ConstructorTestHelp1b* ptr2b = pool2b.getObject();
         pool2b.releaseObject(ptr2b);

         ASSERT(200 == ptr2b->d_resetCount);

         bcec_ObjectPool<ConstructorTestHelp3,
                         ConstructorTestHelp3Creator,
                         bdef_Function<void(*)(ConstructorTestHelp3*)> >
                        pool3(ConstructorTestHelp3Creator(400),
                              bdef_BindUtil::bind(
                                         &ConstructorTestHelp3::resetWithCount,
                                         _1,
                                         300),
                              1,
                              &ta);

         ConstructorTestHelp3* ptr3 = pool3.getObject();
         pool3.releaseObject(ptr3);

         ASSERT(&ta == ptr3->d_allocator_p);
         ASSERT(300 == ptr3->d_resetCount);
         ASSERT(400 == ptr3->d_startCount);

         typedef bcec_ObjectPool<ConstructorTestHelp3,
             bcec_ObjectPoolFunctors::DefaultCreator,
             bdef_Function<void(*)(ConstructorTestHelp3*)> > Pool4;
         Pool4 pool4
            (bdef_BindUtil::bind(&constructor4, 600, _1),
             bdef_BindUtil::bind(&ConstructorTestHelp3::resetWithCount,
                                 _1, 500),
             1, &ta);

         ConstructorTestHelp3* ptr4 = pool4.getObject();
         pool4.releaseObject(ptr4);

         ASSERT(0 == ptr4->d_allocator_p);
         ASSERT(500 == ptr4->d_resetCount);
         ASSERT(600 == ptr4->d_startCount);
      } break;

      case 15: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATOR PROPAGATION:
        //
        // Concerns: Using the default creator, the pool's allocator is passed
        // to the constructor of types declaring UsesAllocator and not passed
        // to other types.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING ALLOCATOR PROPAGATION" << endl
                          << "=============================" << endl;

        bcema_TestAllocator ta(veryVeryVerbose);

        bcec_ObjectPool<UsesAllocatorType> pool1(-1, &ta);
        UsesAllocatorType *ptr1 = pool1.getObject();
        ASSERT(&ta == ptr1->d_allocator_p);

        bcec_ObjectPool<OtherType> pool2(-1, &ta);
        OtherType *ptr2 = pool2.getObject();
        ASSERT(999 == ptr2->d_data);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE:
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING USAGE EXAMPLE" << endl
                          << "=====================" << endl;

        using namespace BCEC_OBJECTPOOL_TEST_USAGE_EXAMPLE;


        QueryFactory *queryFactory = new QueryFactory;
        enum {
            NUM_THREADS = 8,
            NUM_QUERIES = 10000
        };

        bces_AtomicInt numQueries = 0;
        bcemt_ThreadGroup tg;

        tg.addThreads(bdef_BindUtil::bind(&serverThread, &numQueries,
                                          (int)NUM_QUERIES, &queryHandler1),
                      NUM_THREADS);
        tg.joinAll();

        if (verbose) {
            P(totalResponseTime1);
        }

        // server using object pool
        bcema_TestAllocator ta(veryVeryVerbose);
        bcec_ObjectPool<my_DatabaseConnection> pool(-1, &ta);
        connectionPool = &pool;

        numQueries = 0;

        tg.addThreads(bdef_BindUtil::bind(&serverThread, &numQueries,
                                          (int)NUM_QUERIES, &queryHandler2),
                      NUM_THREADS);
        tg.joinAll();

        if (verbose) {
            P(totalResponseTime2);
        }

        delete queryFactory;

      } break;
      case 13: {
        // -------------------------------------------------------------------
        // TESTING CONCERN: releaseObject thread-safety
        //
        // Concern: That several threads acquiring new objects from the pool
        // and then immediately releasing them in a tight loop do not cause
        // any violation of the component's internal invariants.
        // -------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING CONCERN: releaseObject thread safety"
                          << endl
                          << "============================================"
                          << endl;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
           struct Parameters {
              int d_numThreads;
              int d_maxCount;
              int d_numIterations;
           } parameters[] = {
              {1, 2, 10},
              {3, 500, 1000},
              {5, 500, 500}
           };

           for (int i = 0; i < sizeof(parameters)/sizeof(Parameters); ++i) {
              const Parameters& p = parameters[i];

              if (verbose) {
                 P(p.d_numThreads);
                 P(p.d_maxCount);
                 P(p.d_numIterations);
                 cout << "------------------------" << endl;
              }

              bces_AtomicInt created = 0;

              bcec_ObjectPool<Case13Type> mX(
                                  bdef_BindUtil::bind
                                   (&createCase13, _1, _2, &created), 1, &ta);

              bces_AtomicInt done = 0;

              bcec_FixedQueue<Case13Type*> queue(p.d_maxCount);
              bcemt_ThreadGroup procTg;
              for (int j = 0; j < 2; ++j) {
                 procTg.addThread(bdef_BindUtil::bind(&case13Processor,
                                                      &mX,
                                                      &queue,
                                                      &done));
              }

              bcemt_ThreadGroup tg;
              for (int j = 0; j < p.d_numThreads; ++j) {
                 ASSERT(0 == tg.addThread(bdef_BindUtil::bind(
                                                       &case13Thread,
                                                       &mX,
                                                       &queue,
                                                       p.d_numIterations)));
              }
              tg.joinAll();
              done = 1;
              procTg.joinAll();

              ASSERT(mX.numAvailableObjects() == mX.numObjects());
           }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: Allocator propagation
        //
        // Concerns:
        //
        // Plan:
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING CONCERN: Allocator Propagation" << endl
                          << "======================================" << endl;

        using namespace BCEC_OBJECTPOOL_TEST_CASE_12;

        bcema_TestAllocator defaultAlloc(veryVeryVerbose);
        bslma_DefaultAllocatorGuard taGuard(&defaultAlloc);

        bcema_TestAllocator ta(veryVeryVerbose);

        {
            bdef_Function<void(*)(void *, bslma_Allocator*)> objectCreator(
                                       bdef_BindUtil::bind(&createString,
                                                           _1,
                                                           DEFAULT_STRING_INIT,
                                                           _2));
            bcec_ObjectPool<bsl::string> pool(objectCreator, -1, &ta);

            bsl::string *myString = pool.getObject();
            ASSERT(0 == defaultAlloc.numBytesInUse())
        }

        // Expose the violation!
        {
            bcec_ObjectPool<bsl::string> pool
                (bdef_BindUtil::bind(&badCreateString,
                                     bdef_PlaceHolders::_1), -1, &ta);

            bsl::string *myString = pool.getObject();
            ASSERT(0 < defaultAlloc.numBytesInUse())
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: Exception safety
        //
        // Concerns:
        //   That if an object constructor throws in 'getObject',
        //   the objects are destroyed properly and the pool
        //   is left in a valid state (identical to its prior state, for the
        //   call to 'getObject').  We make the object have a side effect to
        //   ensure that the address of the object is correctly passed to the
        //   clean-up guard when the exception is thrown.
        //
        // Plan:
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING CONCERN: Exception safety" << endl
                          << "=================================" << endl;

        using namespace BCEC_OBJECTPOOL_TEST_CASE_11;

        bcema_TestAllocator ta(veryVeryVerbose);
        bslma_DefaultAllocatorGuard taGuard(&ta);

        if (verbose) cout << "\tWith 'getObject'" << endl
                          << "\t----------------" << endl;
        A::constructorCount = B::constructorCount = 0;
        A::destructorCount  = B::destructorCount  = 0;
        {
            bcec_ObjectPool<B> mX;

            createBThrow = 0;
            B *b1Ptr = mX.getObject();  // should not throw
            ++b1Ptr; // satisfy gcc that b1Ptr is used
            ASSERT(mX.numObjects() == 1);
            ASSERT(mX.numAvailableObjects() == 0);

            createBThrow = 1;  // first construction will throw
            try {
                B *b2Ptr = mX.getObject();  // throws
            }
            catch(const Exception& e) {
                if (verbose)
                    cout << "\t" << __LINE__
                         << ": exception caught in createB()." << endl;
            }
            ASSERT(A::constructorCount == 2);
            ASSERT(A::destructorCount  == 1);
            ASSERT(B::constructorCount == 1);
            ASSERT(B::destructorCount  == 0);
            ASSERT(mX.numObjects() == 1);
            ASSERT(mX.numAvailableObjects() == 0);
            if (verbose) {
                P_(L_); P_(A::constructorCount); P(A::destructorCount);
                P_(L_); P_(B::constructorCount); P(B::destructorCount);
                P_(L_); P_(mX.numObjects()); P(mX.numAvailableObjects());
            }
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(A::constructorCount == 2);
        ASSERT(A::destructorCount  == 2);
        ASSERT(B::constructorCount == 1);
        ASSERT(B::destructorCount  == 1);
        if (verbose) {
            P_(L_); P_(A::constructorCount); P(A::destructorCount);
            P_(L_); P_(B::constructorCount); P(B::destructorCount);
            cout << endl;
        }

        if (verbose) cout << "\tWith 'reserveCapacity'" << endl
                          << "\t----------------------" << endl;
        A::constructorCount = B::constructorCount = 0;
        A::destructorCount  = B::destructorCount  = 0;
        {
            bcec_ObjectPool<B> mX;

            createBThrow = 0;
            B *b1Ptr = mX.getObject();  // should not throw
            ++b1Ptr; // satisfy gcc that b1Ptr is used
            ASSERT(mX.numObjects() == 1);
            ASSERT(mX.numAvailableObjects() == 0);

            createBThrow = 3; // throws after the second creation
            try {
                mX.reserveCapacity(4);  // throws after creating two objects
            }
            catch(const Exception& e) {
                if (verbose)
                    cout << "\t" << __LINE__
                         << ": exception caught in createB()." << endl;
            }
            ASSERT(A::constructorCount == 4);
            ASSERT(A::destructorCount  == 3);
            ASSERT(B::constructorCount == 3);
            ASSERT(B::destructorCount  == 2);
            ASSERT(mX.numObjects() == 1);
            ASSERT(mX.numAvailableObjects() == 0);
            if (verbose) {
                P_(L_); P_(A::constructorCount); P(A::destructorCount);
                P_(L_); P_(B::constructorCount); P(B::destructorCount);
                P_(L_); P_(mX.numObjects()); P(mX.numAvailableObjects());
            }
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(A::constructorCount == 4);
        ASSERT(A::destructorCount  == 4);
        ASSERT(B::constructorCount == 3);
        ASSERT(B::destructorCount  == 3);
        if (verbose) {
            P_(L_); P_(A::constructorCount); P(A::destructorCount);
            P_(L_); P_(B::constructorCount); P(B::destructorCount);
            cout << endl;
        }

        if (verbose) cout << "\tWith 'increaseCapacity'" << endl
                          << "\t-----------------------" << endl;
        A::constructorCount = B::constructorCount = 0;
        A::destructorCount  = B::destructorCount  = 0;
        {
            bcec_ObjectPool<B> mX;

            createBThrow = 0;
            B *b1Ptr = mX.getObject();  // should not throw
            ++b1Ptr; // satisfy gcc that b1Ptr is used
            ASSERT(mX.numObjects() == 1);
            ASSERT(mX.numAvailableObjects() == 0);

            createBThrow = 3; // throws after the second creation
            try {
                mX.increaseCapacity(3);  // throws after creating two objects
            }
            catch(const Exception& e) {
                if (verbose)
                    cout << "\t" << __LINE__
                         << ": exception caught in createB()." << endl;
            }
            ASSERT(A::constructorCount == 4);
            ASSERT(A::destructorCount  == 3);
            ASSERT(B::constructorCount == 3);
            ASSERT(B::destructorCount  == 2);
            ASSERT(mX.numObjects() == 1);
            ASSERT(mX.numAvailableObjects() == 0);
            if (verbose) {
                P_(L_); P_(A::constructorCount); P(A::destructorCount);
                P_(L_); P_(B::constructorCount); P(B::destructorCount);
                P_(L_); P_(mX.numObjects()); P(mX.numAvailableObjects());
            }
        }
        ASSERT(0 == ta.numMismatches());
        ASSERT(0 == ta.numBytesInUse());
        ASSERT(A::constructorCount == 4);
        ASSERT(A::destructorCount  == 4);
        ASSERT(B::constructorCount == 3);
        ASSERT(B::destructorCount  == 3);
        if (verbose) {
            P_(L_); P_(A::constructorCount); P(A::destructorCount);
            P_(L_); P_(B::constructorCount); P(B::destructorCount);
            cout << endl;
        }

      } break;
      case 10:
         // Removed, but very inconvenient to shift the other cases down
         break;

      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'RELEASE_OBJECT':
        //   Verifying the 'releaseObject'.
        //
        // Concerns:
        //   That once an object has been released by 'releaseObject', it can
        //   be used by the pool to satisfy further object requests
        //   (potentially from other threads).
        //
        // Plan:
        //   Create a pool of objects of 'Counter' (a class that encapsulates
        //   an integer and has 'increment' method (to increment this
        //   integer) and 'count' method (to report it)).  Create
        //   'NUM_THREADS' threads and let each thread (in a loop) invoke
        //   'getObject' (getting a 'Counter' object 'c'), 'c.increment()'
        //   and finally 'releaseObject(c)'.  Join all the threads and then
        //   verify that the sum of all the counters is equal to the number of
        //   threads multiplied by the number of iterations in the loop.
        //
        // Testing:
        //   void releaseObject(TYPE *objPtr);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'RELEASE_OBJECT'" << endl
                          << "========================" << endl;

        using namespace BCEC_OBJECTPOOL_TEST_CASE_9;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcec_ObjectPool<Counter> p(-1, &ta);
        pool = &p;

        pool->reserveCapacity(NUM_OBJECTS);
        executeInParallel(NUM_THREADS, workerThread9);

        int totalCount = 0;
        for (int i = 0; i < NUM_OBJECTS; ++i) {
            totalCount += pool->getObject()->count();
        }
        LOOP_ASSERT(totalCount, totalCount == NUM_ITERATIONS * NUM_THREADS);

      } break;

      case 8: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST FOR 'd_numAvailableObjects':
        //   Verify the concurrent manipulation of 'd_numAvailableObjects'.
        //
        // Concerns:
        //   That the 'd_numAvailableObjects' remains consistent when multiple
        //   threads invoke 'increaseCapacity', 'getObject' and
        //   'releaseObject'.
        //
        // Plan:
        //   Create a pool, create 'NUM_THREADS' threads and let each thread
        //   run a loop.  In each iteration of the loop, call
        //   'increaseCapacity(1)' and 'releaseObject' (thus incrementing
        //   'd_numAvailableObjects' by 2), then call 'getObject' twice (thus
        //   decrementing 'd_numAvailableObjects' by 2) followed by state
        //   verification.
        //
        // Testing:
        //   int numAvailableObjects() const;
        // --------------------------------------------------------------------
        // TBD: This test case is failing on IBM machine for higher
        // values of 'NUM_ITERATIONS'
        if (verbose)
            cout << endl
                 << "CONCURRENCY TEST FOR 'd_numAvailableObjects'" << endl
                 << "================" << endl;

        using namespace BCEC_OBJECTPOOL_TEST_CASE_8;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcec_ObjectPool<my_Class> p(-1, &ta);
        pool = &p;

        pool->reserveCapacity(NUM_OBJECTS);
        int nC; // number of created objects
        int nA; // number of available objects
        nC = pool->numObjects();
        nA = pool->numAvailableObjects();
        LOOP_ASSERT(nC, NUM_OBJECTS == nC);
        LOOP_ASSERT(nA, NUM_OBJECTS == nA);

        executeInParallel(NUM_THREADS, workerThread8);

      } break;

      case 7: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST FOR 'd_numObjects':
        //   Verify the concurrent manipulation of 'd_numObjects'.
        //
        // Concerns:
        //   That the 'd_numObjects' remains consistent when multiple
        //   threads call 'increaseCapacity' and 'numObjects'.
        //
        // Plan:
        //   Create a pool, create 'NUM_THREADS' threads and let each thread
        //   call 'increaseCapacity', followed by 'numObjects' in a
        //   loop.  Verify that for each thread, the values returned by
        //   'numObjects' are in increasing order.
        //
        // Testing:
        //   void increaseCapacity(int numObjects);
        //   int numObjects() const;
        // --------------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "CONCURRENCY TEST FOR 'd_numObjects'" << endl
                 << "==========================================" << endl;

        using namespace BCEC_OBJECTPOOL_TEST_CASE_7;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcec_ObjectPool<my_Class> p(-1, &ta);
        pool = &p;

        executeInParallel(NUM_THREADS, workerThread7);

      } break;

      case 6: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST-4 FOR FREE OBJECT LIST:
        //   Verify the concurrent manipulation of free object list.
        //
        // Concerns:
        //   That the 'd_freeObjectsList' remains consistent in presence of
        //   multiple threads calling 'releaseObject', 'getObject',
        //   'increaseCapacity' and 'reserveCapacity'.
        //
        // Plan:
        //   Create a pool and reserve enough objects (so that calls to
        //   'getObject' can be quickly satisfied).  Create 'NUM_THREADS'
        //   (a multiple of 4) threads.  Define a *0-order* thread to be
        //   any i'th threads such that i%4 == 0.  Similarly define *1-order*
        //   thread (i%4 == 1), *2-order* thread (i%4 == 2) and *3-order*
        //   thread (i%4 == 3).  Let all 0-order threads call 'getObject' in
        //   a loop (thus collecting the objects that can later be released),
        //   meanwhile remaining threads wait.  After all the threads
        //   reach a common barrier, let each 0-order thread call
        //   'releaseObject', each 1-order thread call 'getObject', each
        //   2-order thread call 'increaseCapacity' and each 3-order thread
        //   call 'reserveCapacity'.  Finally verify the state.
        //
        // Testing:
        //   TYPE *getObject();
        //   void increaseCapacity(int numObjects);
        //   void releaseObject(TYPE *objPtr);
        //   void reserveCapacity(int numObjects);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "CONCURRENCY TEST-4 FOR FREE OBJECT LIST" << endl
                          << "=======================================" << endl;

        using namespace BCEC_OBJECTPOOL_TEST_CASE_6;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcec_ObjectPool<my_Class> p(-1, &ta);
        pool = &p;

        pool->reserveCapacity(NUM_THREADS * NUM_OBJECTS);
        int nC; // number of created objects
        int nA; // number of available objects
        nC = pool->numObjects();
        nA = pool->numAvailableObjects();
        LOOP_ASSERT(nC, nC == NUM_THREADS * NUM_OBJECTS);
        LOOP_ASSERT(nA, nA == NUM_THREADS * NUM_OBJECTS);

        attributes.setStackSize(sizeof(my_Class *) * NUM_OBJECTS + 1000000);
        executeInParallel(NUM_THREADS, workerThread6);

        nC = pool->numObjects();
        nA = pool->numAvailableObjects();
        LOOP_ASSERT(nC, nC ==  NUM_THREADS * NUM_OBJECTS * 5 / 4);
        LOOP_ASSERT(nA, nA == NUM_THREADS * NUM_OBJECTS);
        verifyPool(pool);

      } break;

      case 5: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST-3 FOR FREE OBJECT LIST:
        //   Verify the concurrent manipulation of free object list.
        //
        // Concerns:
        //   That the 'd_freeObjectsList' remains consistent in presence of
        //   multiple threads calling 'releaseObject' and 'getObject'.
        //
        // Plan:
        //   Create a pool and reserve enough objects so that calls to
        //   'getObject' can be quickly satisfied.  Create 'NUM_THREADS'
        //   threads and let each thread call 'getObject' followed by
        //   'releaseObject' in a loop.  Finally verify the state.
        //
        // Testing:
        //   TYPE *getObject();
        //   void releaseObject(TYPE *objPtr);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "CONCURRENCY TEST-3 FOR FREE OBJECT LIST" << endl
                          << "=======================================" << endl;

        using namespace BCEC_OBJECTPOOL_TEST_CASE_5;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcec_ObjectPool<my_Class> p(-1, &ta);
        pool = &p;

        pool->reserveCapacity(NUM_OBJECTS);
        int nC; // number of created objects
        int nA; // number of available objects
        nC = pool->numObjects();
        nA = pool->numAvailableObjects();
        LOOP_ASSERT(nC, nC == NUM_OBJECTS);
        LOOP_ASSERT(nA, nA == NUM_OBJECTS);

        executeInParallel(NUM_THREADS, workerThread5);
        nC = pool->numObjects();
        nA = pool->numAvailableObjects();
        LOOP_ASSERT(nC, nC == NUM_OBJECTS);
        LOOP_ASSERT(nA, nA == NUM_OBJECTS);

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST-2 FOR FREE OBJECT LIST:
        //   Verify the concurrent manipulation of free object list.
        //
        // Concerns:
        //   That the 'd_freeObjectsList' remains consistent in presence of
        //   multiple threads calling 'releaseObject' and 'getObject'.
        //
        // Plan:
        //   Create a pool and reserve enough objects so that calls to
        //   'getObject' can be quickly satisfied.  Create 'NUM_THREADS'
        //   threads.  Let even numbered threads call 'getObject' in a
        //   loop (thus collecting the objects that can later be released),
        //   meanwhile odd numbered threads wait.  After that when all the
        //   threads have reached a barrier,  let even numbered threads
        //   call 'releaseObject' (in a loop) and odd numbered threads
        //   call 'getObject' (in a loop).  Finally verify the state.
        //
        // Testing:
        //   TYPE *getObject();
        //   void releaseObject(TYPE *objPtr);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "CONCURRENCY TEST-2 FOR FREE OBJECT LIST" << endl
                          << "=======================================" << endl;

        using namespace BCEC_OBJECTPOOL_TEST_CASE_4;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcec_ObjectPool<my_Class> p(-1, &ta);
        pool = &p;

        pool->reserveCapacity(NUM_THREADS * NUM_OBJECTS);
        int nC; // number of created objects
        int nA; // number of available objects
        nC = pool->numObjects();
        nA = pool->numAvailableObjects();
        LOOP_ASSERT(nC, nC == NUM_THREADS * NUM_OBJECTS);
        LOOP_ASSERT(nA, nA == NUM_THREADS * NUM_OBJECTS);

        attributes.setStackSize(sizeof(my_Class *) * NUM_OBJECTS + 1000000);
        executeInParallel(NUM_THREADS, workerThread4);
        nC = pool->numObjects();
        nA = pool->numAvailableObjects();
        LOOP_ASSERT(nC, nC == NUM_THREADS * NUM_OBJECTS);
        LOOP_ASSERT(nA, nA == NUM_THREADS * NUM_OBJECTS / 2);

      } break;

      case 3: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST-1 FOR FREE OBJECT LIST:
        //   Verify the concurrent manipulation of free object list.
        //
        // Concerns:
        //   That the 'd_freeObjectsList' remains consistent when multiple
        //   threads call 'getObject'.
        //
        //   That the 'd_freeObjectsList' remains consistent when multiple
        //   threads call 'releaseObject'.
        //
        // Plan:
        //   Create a pool and reserve enough objects so that calls to
        //   'getObject' can be quickly satisfied.  Create 'NUM_THREADS'
        //   threads, each of which will call 'getObject' in a loop
        //   (thus ensuring the first part of the concerns is tested
        //   effectively).  Verify the state after the loop.  Now let each
        //   thread call 'releaseObject' in a loop (thus ensuring the
        //   second part of concerns is tested effectively) and verify the
        //   state.
        //
        // Testing:
        //   TYPE *getObject();
        //   void releaseObject(TYPE *objPtr);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "CONCURRENCY TEST-1 FOR FREE OBJECT LIST" << endl
                          << "=======================================" << endl;

        using namespace BCEC_OBJECTPOOL_TEST_CASE_3;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcec_ObjectPool<my_Class> p(-1, &ta);
        pool = &p;

        pool->reserveCapacity(NUM_THREADS * NUM_OBJECTS);
        int nC; // number of created objects
        int nA; // number of available objects
        nC = pool->numObjects();
        nA = pool->numAvailableObjects();
        LOOP_ASSERT(nC, nC == NUM_THREADS * NUM_OBJECTS);
        LOOP_ASSERT(nA, nA == NUM_THREADS * NUM_OBJECTS);

        attributes.setStackSize(sizeof(my_Class *) * NUM_OBJECTS + 1000000);
        executeInParallel(NUM_THREADS, workerThread3);
        LOOP_ASSERT(nC, nC == NUM_THREADS * NUM_OBJECTS);
        LOOP_ASSERT(nA, nA == NUM_THREADS * NUM_OBJECTS);

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // SINGLE THREAD FUNCTIONALITY TEST:
        //   Verify the functionality in presence of a single thread.
        //
        // Concerns:
        //   That the pool is replenished with the correct number of objects
        //   when it is depleted.
        //
        //   That enough memory is allocated for the objects and the
        //   associated block and object nodes.
        //
        //   That the objects and associated block and object nodes are
        //   properly laid out in the allocated memory.
        //
        //   That the objects are constructed properly.
        //
        //   That 'getObject' returns a properly constructed object.
        //
        //   That the destructor destroys all the objects created by the pool.
        //
        //   That the destructor deallocates all the memory allocated by the
        //   pool.
        //
        // Plan:
        //   Initialize pools for various values of replenishment policy.  Use
        //   'bslma_TestAllocator' as the allocator for each pool.  Do
        //   the following for each pool.
        //   Invoke 'getObject' repeatedly and verify that the correct
        //   number of objects are created.  Verify that all the objects (and
        //   associated block and object nodes) are properly laid out in the
        //   allocated memory (this is achieved by (1) using the test
        //   allocator (2) by having knowledge of sizes of the object, object
        //   node and block node and (3) by having knowledge of memory layout
        //   of these).  Verify that each object is properly aligned (this is
        //   achieved by having knowledge of each object's staring address and
        //   its alignment).  Verify that each object is properly constructed
        //   and 'getObject' returns a properly constructed object (this is
        //   achieved by having the object constructor initialize a class
        //   variable 'd_pattern' and a subsequent verification of the same).
        //   When the pool is destroyed, verify that all the objects are
        //   destroyed (this is achieved by maintaining object count in
        //   'objCount') and the memory is reclaimed (this is achieved by
        //   using test allocator).
        //
        // Testing:
        //   bcec_ObjectPool(objectCreator, bslma_Allocator);
        //   bcec_ObjectPool(objectCreator, numObjects, bslma_Allocator);
        //   ~bcec_ObjectPool();
        //   TYPE *getObject();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "SINGLE THREAD FUNCTIONALITY TEST" << endl
                          << "================================" << endl;

        using namespace BCEC_OBJECTPOOL_TEST_CASE_2;

        enum { MAX_SIZE = 100 };
        int DATA[][MAX_SIZE] = {

            // replenishment    // expected replenishments over
            // policy           // successive pool depletion
            {  -1,               1, 2, 4, 8, 16, 32, 32, 32  },
            {   1,               1, 1, 1                     },
            {   2,               2, 2, 2                     },
            { 100,               100, 100, 100               },
            {  -2,               2, 4, 8, 16, 32, 32, 32     },
            {  -3,               3, 6, 12, 24, 48, 48, 48    },
            { -31,               31, 62, 62, 62              },
            { -32,               32, 32, 32                  },
            { -33,               33, 33, 33                  },


            // this corresponds to default replenishment policy
            // Note that this MUST be the last row and MUST not be modified
            {   0,               1, 2, 4, 8, 16, 32, 32, 32  }
        };

        bslma_TestAllocator ta;
        bcec_ObjectPool<my_CheckingClass> *pool;

        int size = sizeof(DATA)/sizeof(DATA[0]);
        for (int i = 0; i < size; ++i) {

            if (i == (size-1)) { // last row represents default
                                 // replenishment policy
                pool = new bcec_ObjectPool<my_CheckingClass>(-1, &ta);
            }
            else {
                pool = new bcec_ObjectPool<my_CheckingClass>(DATA[i][0], &ta);
            }

            int numLastCreated = 0;
            for (int j = 1; DATA[i][j] != 0; ++j) {
                int nA; // number of available objects
                int nC; // number of created objects
                nA = pool->numAvailableObjects();
                LOOP3_ASSERT(i, j, nA, nA == 0);

                // trigger pool repletion
                my_CheckingClass *p = pool->getObject();
                pool->releaseObject(p);

                // verify that the correct number of objects are created
                nC = pool->numObjects();
                int diff1 = nC - numLastCreated;
                int diff2 = DATA[i][j];
                LOOP4_ASSERT(i, j, diff1, diff2, diff1 == diff2);
                int oC = my_CheckingClass::objCount; // object count
                LOOP4_ASSERT(i, j, nC, oC, nC == oC);

                // verify that the objects and block and object nodes are
                // allocated within the allocated memory
                char *lastAdr1 = (char *)p - sizeof(BlockNode);
                char *lastAdr2 = (char *)ta.lastAllocatedAddress();
                LOOP4_ASSERT(i, j, lastAdr1, lastAdr2, lastAdr1 >= lastAdr2);

                // verify that enough memory is allocated
                int size1 = sizeof(BlockNode) + DATA[i][j]*OBJECT_FRAME_SIZE;
                int size2 = ta.lastAllocatedNumBytes();
                LOOP4_ASSERT(i, j, size1, size2, size1 <= size2);


                numLastCreated = pool->numObjects();
                my_CheckingClass *lastP =
                    (my_CheckingClass*) ((char *)p - OBJECT_FRAME_SIZE);
                for (int k = 1; k <= DATA[i][j]; ++k) {
                    my_CheckingClass *currP = pool->getObject();

                    // verify the alignment
                    int al = bsls_AlignmentFromType<my_CheckingClass>::VALUE;
                    LOOP5_ASSERT(i,j,k,
                        (bsls_PlatformUtil::IntPtr)currP,al,
                        (bsls_PlatformUtil::IntPtr)currP % al == 0);

                    // verify that successive objects are separated by
                    // 'OBJECT_FRAME_SIZE' bytes.
                    my_CheckingClass *temp =
                     (my_CheckingClass *)((char *)lastP + OBJECT_FRAME_SIZE);
                    LOOP5_ASSERT(i, j, k, (char*)lastP, (char*)currP,
                                                                  temp==currP);

                    // Verify that the objects were properly constructed
                    int pat1 = currP->pattern();
                    int pat2 = my_CheckingClass::PATTERN;
                    LOOP5_ASSERT(i, j, k, pat1, pat2, pat1 == pat2);

                    lastP = currP;
                }
                nC = pool->numObjects();
                LOOP4_ASSERT(i, j, numLastCreated, nC, numLastCreated == nC);
            }

            delete pool;
            // verify that all the objects are destroyed
            int oC = my_CheckingClass::objCount; // object count
            LOOP_ASSERT(oC, oC == 0);

            // verify that all the memory is deallocated
            int nUse = ta.numBytesInUse();  // number of bytes in use
            LOOP_ASSERT(nUse, nUse == 0);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise the basic functionality.
        //
        // Concerns:
        //   That basic essential functionality is operational
        //   for one thread.
        //
        // Plan:
        //   Create a pool with default replenishment-policy, verify the
        //   initial state, call 'getObject' a couple of times (verifying
        //   states after each call), call 'releaseObject' and verify the
        //   state.
        //
        //   Create a pool with -2 as replenishment-policy, verify the
        //   initial state, call 'getObject' three times and verify the
        //   states after each call.
        //
        //   Create a pool with 1 as replenishment-policy, verify the
        //   initial state, call 'getObject' two times and verify the
        //   states after each call.
        //
        //   Create a pool, call 'reserveCapacity(2)', 'getObject' and
        //   'releaseObject' in order and verify the states after each
        //   call.
        //
        //   Create a pool, call 'reserveCapacity' five times (passing 0, 1,
        //   1, 2, 2 as its argument) and verify the states after each call.
        //
        // Testing:
        //   This Test Case exercises basic functionality.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        using namespace BCEC_OBJECTPOOL_TEST_CASE_1;
        int nC; // number of created objects
        int nA; // number of available objects

        {
            bcema_TestAllocator ta(veryVeryVerbose);
            bcec_ObjectPool<my_Class> pool(-1, &ta);

            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 0);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 0);

            pool.getObject();
            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 1);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 0);

            my_Class *p = pool.getObject();
            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 3);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 1);

            pool.releaseObject(p);
            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 3);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 2);
        } // let pool be destroyed

        {
            bcema_TestAllocator ta(veryVeryVerbose);
            bcec_ObjectPool<my_Class> pool(-2, &ta);

            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 0);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 0);

            pool.getObject();
            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 2);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 1);

            pool.getObject();
            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 2);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 0);

            pool.getObject();
            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 6);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 3);
        } // let pool be destroyed

        {
            bcema_TestAllocator ta(veryVeryVerbose);
            bcec_ObjectPool<my_Class> pool(1, &ta);

            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 0);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 0);

            pool.getObject();
            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 1);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 0);

            pool.getObject();
            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 2);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 0);
        } // let pool be destroyed

        {
            bcema_TestAllocator ta(veryVeryVerbose);
            bcec_ObjectPool<my_Class> pool(-1, &ta);

            pool.reserveCapacity(2);
            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 2);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 2);

            my_Class *p = pool.getObject();
            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 2);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 1);

            pool.releaseObject(p);
            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 2);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 2);
        } // let pool be destroyed

        {
            bcema_TestAllocator ta(veryVeryVerbose);
            bcec_ObjectPool<my_Class> pool(-1, &ta);

            pool.reserveCapacity(0);
            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 0);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 0);

            pool.reserveCapacity(1);
            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 1);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 1);

            pool.reserveCapacity(1);
            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 1);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 1);

            pool.reserveCapacity(2);
            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 2);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 2);

            pool.reserveCapacity(2);
            nC = pool.numObjects();
            LOOP_ASSERT(nC, nC == 2);
            nA = pool.numAvailableObjects();
            LOOP_ASSERT(nA, nA == 2);
        } // let pool be destroyed

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
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
