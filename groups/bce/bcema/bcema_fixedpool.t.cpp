// bcema_fixedpool.t.cpp  -*-C++-*-

#include <bcema_fixedpool.h>
#include <bcemt_barrier.h>
#include <bcemt_thread.h>
#include <bslma_testallocator.h>          // for testing only
#include <bslma_testallocatorexception.h> // for testing only
#include <bsls_alignment.h>
#include <bsls_platform.h>
#include <bcemt_threadgroup.h>
#include <bdef_bind.h>
#include <bcemt_qlock.h>
#include <bdeu_random.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_vector.h>
#include <bsl_iostream.h>
#include <bsl_cstring.h>     // memcpy()
#include <bsl_deque.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static bcemt_QLock coutMutex;

static void rEpOrT(const char *s, int i) {
    cout << "Error " << __FILE__ << "(" << i << "): " << s
         << "    (failed)" << endl;
    if (testStatus >= 0 && testStatus <= 100) ++testStatus;
}

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bcemt_QLockGuard guard(&coutMutex);
        rEpOrT(s, i);
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bcemt_QLockGuard guard(&coutMutex);        \
                cout << #I << ": " << I << "\n";           \
                rEpOrT(#X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bcemt_QLockGuard guard(&coutMutex);                          \
                cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";  \
                rEpOrT(#X, __LINE__); }}

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bcemt_QLockGuard guard(&coutMutex);                           \
               cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"    \
                    << #K << ": " << K << "\n";                              \
               rEpOrT(#X, __LINE__); }}

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define TAB cout << '\t';
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//=============================================================================
//                   GLOBAL TYPEDEFS, CONSTANTS, AND VARIABLES
//-----------------------------------------------------------------------------

typedef bcema_FixedPool Obj;

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

int numLeftChildren   = 0;
int numMiddleChildren = 0;
int numRightChildren  = 0;
int numMostDerived    = 0;

struct LeftChild {
    int d_li;
    LeftChild()           { ++numLeftChildren; }
    virtual ~LeftChild()  { --numLeftChildren; }
};

struct MiddleChild {    // non-polymorphic middle child
    int d_mi;
    MiddleChild()         { ++numMiddleChildren; }
    ~MiddleChild()        { --numMiddleChildren; }
};

struct RightChild {
    int d_ri;
    RightChild()          { ++numRightChildren; }
    virtual ~RightChild() { --numRightChildren; }
};

struct MostDerived : LeftChild, MiddleChild, RightChild {
    int d_md;
    MostDerived()         { ++numMostDerived; }
    ~MostDerived()        { --numMostDerived; }
};

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

class my_JobQueue {
 public:
 // PUBLIC TYPES

   typedef bdef_Function<void(*)(void)> Job;

 private:

   bcemt_Mutex       d_lock;
   bsl::deque<Job*>  d_queue;
   bcema_FixedPool   d_pool;
   bslma_Allocator  *d_allocator_p;

 public:
   my_JobQueue(int maxJobs,
               bslma_Allocator *basicAllocator=0);
  ~my_JobQueue();

   void enqueueJob(const Job& job);

   int tryExecuteJob();
};

my_JobQueue::my_JobQueue(int maxJobs,
                         bslma_Allocator* basicAllocator)
: d_queue(basicAllocator)
, d_pool(sizeof(Job), maxJobs, basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

my_JobQueue::~my_JobQueue()
{
   Job* job;
   while (!d_queue.empty()) {
      job = d_queue.front();
      job->~Job();
      d_queue.pop_front();
   }
}

void my_JobQueue::enqueueJob(const Job& job)
{
   Job *jobPtr = new (d_pool) Job(job, d_allocator_p);
   d_lock.lock();
   d_queue.push_back(jobPtr);
   d_lock.unlock();
}

int my_JobQueue::tryExecuteJob() {
  d_lock.lock();
  if (d_queue.empty()) {
     d_lock.unlock();
     return -1;
  }
  Job* job = d_queue.front();
  d_queue.pop_front();
  d_lock.unlock();
  (*job)();
  d_pool.deleteObject(job);
  return 0;
}

void sum5(double* result, double op1, double op2,
          double op3, double op4, double op5)
{
    *result = op1 + op2 + op3 + op4 + op5;
}

//=============================================================================
//                CONCRETE OBJECTS FOR TESTING 'deleteObject'
//-----------------------------------------------------------------------------

static int my_ClassCode = 0;

class my_Class1 {
  public:
    my_Class1()  { my_ClassCode = 1; }
    ~my_Class1() { my_ClassCode = 2; }
};

class my_Class2 {
  public:
    my_Class2()  { my_ClassCode = 3; }
    ~my_Class2() { my_ClassCode = 4; }
};

// The "dreaded diamond".

static int virtualBaseObjectCount = 0;
static int leftBaseObjectCount    = 0;
static int rightBaseObjectCount   = 0;
static int mostDerivedObjectCount = 0;

class my_VirtualBase {
    int x;
public:
    my_VirtualBase()          { virtualBaseObjectCount = 1; }
    virtual ~my_VirtualBase() { virtualBaseObjectCount = 0; }
};

class my_LeftBase : virtual public my_VirtualBase {
    int x;
public:
    my_LeftBase()             { leftBaseObjectCount = 1; }
    virtual ~my_LeftBase()    { leftBaseObjectCount = 0; }
};

class my_RightBase : virtual public my_VirtualBase {
    int x;
public:
    my_RightBase()            { rightBaseObjectCount = 1; }
    virtual ~my_RightBase()   { rightBaseObjectCount = 0; }
};

class my_MostDerived : public my_LeftBase, public my_RightBase {
    int x;
public:
    my_MostDerived()          { mostDerivedObjectCount = 1; }
    ~my_MostDerived()         { mostDerivedObjectCount = 0; }
};

//=============================================================================
//                      HELPER FUNCTION FOR CONCURRENCY TEST
//-----------------------------------------------------------------------------

enum {
    OBJECT_SIZE = 56,
    NUM_INTS = OBJECT_SIZE / sizeof(int),
    NUM_OBJECTS = 10000,
    NUM_THREADS = 4
};

bcemt_Barrier barrier(NUM_THREADS);
extern "C"
void *workerThread(void *arg) {
    Obj *mX = (Obj *) arg;
    ASSERT(OBJECT_SIZE == mX->objectSize());

    barrier.wait();
    for (int i = 0; i < NUM_OBJECTS; ++i) {
        volatile int *buffer = (int*)mX->allocate();
        if (veryVeryVerbose) {
            printf("Thread %d: Allocated %p\n", bcemt_ThreadUtil::self(),
                   (void *)buffer);
        }
        LOOP_ASSERT(i, (void*)buffer != (void*)0xAB);
        LOOP_ASSERT(i, buffer);
        *buffer = 0xAB;
        LOOP_ASSERT(i, (void*)buffer != (void*)0xAB);
        mX->deallocate((void*)buffer);
        LOOP_ASSERT(i, (void*)buffer != (void*)0xAB);
    }
    return arg;
}

//=============================================================================
//                     BENCHMARKS
//-----------------------------------------------------------------------------
namespace bench {

struct Item {
    int  d_threadId;
    int  d_work;
};

struct Control {
    bcemt_Barrier         *d_barrier;
    bcema_FixedPool       *d_fixedpool;
    int                    d_iterations;
    int                    d_numObjects;
    int                    d_load;
};

void hardwork(Item *item, int load)
{
    item->d_work = item->d_threadId;
    for (int i=0; i<load; ++i) {
        bdeu_Random::generate15(&item->d_work);
    }
}

void bench(Control *control)
{
    int threadId = bcemt_ThreadUtil::selfIdAsInt();

    bcema_FixedPool *pool = control->d_fixedpool;
    int numObjects = control->d_numObjects;
    int load = control->d_load;

    bsl::vector<Item *> objects(numObjects, (Item *)0);

    control->d_barrier->wait();

    for (int i=0; i<control->d_iterations; i++) {
        for(int j=0; j<numObjects; j++) {
            for(int t=0; t<=j; t++) {
                Item *item = (Item *)pool->allocate();
                ASSERT(item);
                item->d_threadId = threadId;
                objects[t] = item;
            }
            for(int t=0; t<=j; t++) {
                Item *item = objects[t];
                ASSERT(item->d_threadId == threadId);
                hardwork(item, load);
            }
            for(int t=0; t<=j; t++) {
                Item *item = objects[t];
                ASSERT(item->d_threadId == threadId);
                pool->deallocate(item);
            }
        }
    }
}

void runtest(int numIterations, int numObjects, int numThreads,
             int load, int backoff)
{
    bcema_FixedPool pool(sizeof(Item), numThreads * numObjects);
    pool.setBackoffLevel(backoff);

    ASSERT(backoff == pool.backoffLevel());

    bcemt_Barrier barrier(numThreads);

    Control control;

    control.d_barrier = &barrier;
    control.d_fixedpool = &pool;
    control.d_iterations = numIterations;
    control.d_numObjects = numObjects;
    control.d_load = load;

    bcemt_ThreadGroup tg;
    tg.addThreads(bdef_BindUtil::bind(&bench,&control), numThreads);

    tg.joinAll();
}
}

//=============================================================================
//                    fill and release concurrently
//-----------------------------------------------------------------------------
namespace fill_and_release {

struct Item {
    int  d_threadId;
};

struct Control {
    bcemt_Barrier         *d_barrier;
    bcema_FixedPool       *d_fixedpool;
    int                    d_iterations;
    bces_AtomicInt         d_allocationCount;
};

void bench(Control *control)
{
    bcema_FixedPool *pool = control->d_fixedpool;
    const int poolSize = pool->poolSize();
    const int iterations = control->d_iterations;
    bces_AtomicInt& allocationCount = control->d_allocationCount;


    for (int i = 0; i<iterations; i++) {

        int count = allocationCount++;
        while(count < poolSize) {
            void *p = pool->allocate();
            ASSERT(p);
            pool->deallocate(p);
            p = pool->allocate();
            ASSERT(p);
            count = allocationCount++;
        }

        // sync up with other threads
        control->d_barrier->wait();

        // To assert that allocate() returns 0, we need to make
        // sure that all threads have reached this point (so it has
        // to be done after the barrier.  But we need to make sure
        // that no thread is about to call release() so we have put
        // another barrier after the assert.

        ASSERT(pool->allocate() == 0);

        control->d_barrier->wait();
        if (count == poolSize) {
            // One thread will release all the objects.

            pool->release();
            allocationCount = 0;
        }
        bcemt_ThreadUtil::yield(); // exhaust time slice
        control->d_barrier->wait();
    }
}

void runtest(int numIterations, int numObjects, int numThreads)
{
    bcema_FixedPool pool(sizeof(Item), numObjects);

    bcemt_Barrier barrier(numThreads);

    Control control;

    control.d_barrier = &barrier;
    control.d_fixedpool = &pool;
    control.d_iterations = numIterations;
    control.d_allocationCount = 0;

    bcemt_ThreadGroup tg;
    tg.addThreads(bdef_BindUtil::bind(&bench,&control), numThreads);

    tg.joinAll();
}
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 11: {
        // --------------------------------------------------------------------
        // Testing dataOffset and nodeSize calculations
        // --------------------------------------------------------------------
        if (verbose) cout << endl
             << "Testing dataOffset and nodeSize calculations" << endl
             << "============================================" << endl;

        bslma_TestAllocator a;
        for (int i=1; i<=1000; ++i) {
            Obj mX(i, 1, &a);
            char *mem = (char *)mX.allocate();
            memset(mem, 0xFF, i);
            mX.deallocate(mem);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING deleteObject AND deleteObjectRaw
        //
        // Concerns:
        //   That 'deleteObject' and 'deleteObjectRaw' properly destroy and
        //   deallocate managed objects.
        //
        // Plan:
        //   Iterate where at the beginning of the loop, we create an object
        //   of type 'mostDerived' that multiply inherits from two types with
        //   virtual destructors.  Then in the middle of the loop we switch
        //   into several ways of destroying and deallocating the object with
        //   various forms of 'deleteObjectRaw' and 'deleteObject', after
        //   which we verify that the destructors have been run.  Each
        //   iteration we verify that the memory we got was the same as for
        //   the previous iteration, which shows that memory is being
        //   deallocated and recovered by the pool.
        //
        // Testing:
        //   deleteObjectRaw()
        //   deleteObject()
        // --------------------------------------------------------------------

        bslma_TestAllocator alloc, *Z = &alloc;

        bool finished = false;
        const MostDerived *repeater = 0;    // verify we're re-using the memory
                                            // each time
        Obj fp(sizeof(MostDerived), 100, Z);
        for (int di = 0; !finished; ++di) {
            MostDerived *pMD = (MostDerived *) fp.allocate();
            const MostDerived *pMDC = pMD;

            if (!repeater) {
                repeater = pMDC;
            }
            else {
                // this verifies that we are freeing the memory each iteration
                // because we get the same pointer every time we allocate, and
                // we allocate one extra time at the end
                LOOP_ASSERT(di, repeater == pMDC);
            }
            new (pMD) MostDerived();

            ASSERT(1 == numLeftChildren);
            ASSERT(1 == numMiddleChildren);
            ASSERT(1 == numRightChildren);
            ASSERT(1 == numMostDerived);

            switch (di) {
              case 0: {
                fp.deleteObjectRaw(pMDC);
              } break;
              case 1: {
                const LeftChild *pLCC = pMDC;
                ASSERT((void*) pLCC == (void*) pMDC);
                fp.deleteObjectRaw(pLCC);
              } break;
              case 2: {
                fp.deleteObject(pMDC);
              } break;
              case 3: {
                const LeftChild *pLCC = pMDC;
                ASSERT((void*) pLCC == (void*) pMDC);
                fp.deleteObject(pLCC);
              } break;
              case 4: {
                const RightChild *pRCC = pMDC;
                ASSERT((void*) pRCC != (void*) pMDC);
                fp.deleteObject(pRCC);
              } break;
              case 5: {
                fp.deleteObjectRaw(pMDC);    // 2nd time we do this

                finished = true;
              } break;
              default: {
                ASSERT(0);
              }
            }

            LOOP_ASSERT(di, 0 == numLeftChildren);
            LOOP_ASSERT(di, 0 == numMiddleChildren);
            LOOP_ASSERT(di, 0 == numRightChildren);
            LOOP_ASSERT(di, 0 == numMostDerived);
        }
      } break;
      case 9: {
        // ---------------------------------------------------------
        // Fill and release test
        // ---------------------------------------------------------
        if (verbose) cout << endl
                          << "Fill and release test" << endl
                          << "=====================" << endl;
        enum {
            NUM_THREADS = 4,
            NUM_ITERATIONS = 50,
            NUM_OBJECTS = 10
        };

        int numIterations = NUM_ITERATIONS;
        int numObjects = NUM_OBJECTS;

        for(int numThreads=1; numThreads<=NUM_THREADS; numThreads++) {

            fill_and_release::runtest(numIterations, numObjects, numThreads);
        }


      } break;
      case 8: {
        // ---------------------------------------------------------
        // BENCHMARK
        // ---------------------------------------------------------
        if (verbose) cout << endl
                          << "Benchmark" << endl
                          << "========================" << endl;
        enum {
            NUM_THREADS = 4,
            NUM_ITERATIONS = 50,
            NUM_OBJECTS = 10,
            LOAD = 16,
            BACKOFF = 0
        };

        int numIterations = NUM_ITERATIONS;
        int numObjects = NUM_OBJECTS;
        int load = LOAD;
        int backoff = BACKOFF;

        for(int numThreads=1; numThreads<=NUM_THREADS; numThreads++) {

            bench::runtest(numIterations, numObjects, numThreads,
                           load, backoff);
        }


      } break;
      case 7: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST
        //
        // Testing:
        //     Thread-safety of allocate/deallocate methods.
        // --------------------------------------------------------------------
        bcemt_ThreadUtil::Handle threads[NUM_THREADS];
        Obj mX(OBJECT_SIZE, NUM_THREADS);
        for (int i = 0; i < NUM_THREADS; ++i) {
            int rc =
                bcemt_ThreadUtil::create(&threads[i], workerThread, &mX);
            LOOP_ASSERT(i, 0 == rc);
        }
        for (int i = 0; i < NUM_THREADS; ++i) {
            int rc =
                bcemt_ThreadUtil::join(threads[i]);
            LOOP_ASSERT(i, 0 == rc);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // MEMBER TEMPLATE METHOD 'deleteObject' TEST:
        //   We want to make sure that when 'deleteObject' is used both
        //   destructor and 'deallocate' are invoked.
        //
        // Plan:
        //   Using a pool and placement new operator construct objects of
        //   two different classes.  Invoke 'deleteObject' to delete
        //   constructed objects and check that both destructor and
        //   'deallocate' have been called.
        //
        // Testing:
        //   template<typename TYPE> void deleteObject(TYPE *object)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'deleteObject' TEST" << endl
                                  << "===================" << endl;

        if (verbose) cout << "\nTesting 'deleteObject':" << endl;
        {
            bslma_TestAllocator a(veryVeryVerbose);
            const bslma_TestAllocator& A = a;

            const int OBJECT_SIZE = sizeof(my_Class1);
            ASSERT(sizeof(my_Class2) == OBJECT_SIZE);
            const int NUM_OBJECTS = 2;
            Obj mX(OBJECT_SIZE, NUM_OBJECTS, &a);

            if (verbose) cout << "\twith a my_Class1 object" << endl;

            my_ClassCode=0;

            my_Class1 *pC1 = (my_Class1 *) mX.allocate();
            new(pC1) my_Class1;
            if (verbose) { T_();  T_();  P(my_ClassCode); }
            ASSERT(1 == my_ClassCode);
            //ASSERT(A.numAllocations() == 1);

            mX.deleteObject(pC1);
            if (verbose) { T_();  T_();  P(my_ClassCode); }
            ASSERT(2 == my_ClassCode);
            //ASSERT(A.numAllocations() == 1);
            mX.allocate();
            //ASSERT(A.numAllocations() == 1);
                  // By observing that the number of allocations
                  // stays at one we confirm that the memory obtained
                  // from the pool has been returned by 'deleteObject'.
                  // Had it not been returned, the call to allocate would
                  // have required another allocation from the allocator.

            if (verbose) cout << "\twith a my_Class2 object" << endl;

            my_Class2 *pC2 = (my_Class2 *) mX.allocate();
            new(pC2) my_Class2;
            if (verbose) { T_();  T_();  P(my_ClassCode); }
            ASSERT(3 == my_ClassCode);
            //ASSERT(A.numAllocations() == 1);

            mX.deleteObject(pC2);
            if (verbose) { T_();  T_();  P(my_ClassCode); }
            ASSERT(4 == my_ClassCode);
            //ASSERT(A.numAllocations() == 1);
            mX.allocate();
            //ASSERT(A.numAllocations() == 1);
        }

        if (verbose) cout << "\nTesting 'deleteObject' on polymorphic types:"
                          << endl;
        {
            bslma_TestAllocator a(veryVeryVerbose);

            const int OBJECT_SIZE = sizeof(my_MostDerived);
            ASSERT(sizeof(my_MostDerived) == OBJECT_SIZE);
            const int NUM_OBJECTS = 1;
            Obj mX(OBJECT_SIZE, NUM_OBJECTS, &a);

            if (verbose) cout << "\tdeleteObject(my_MostDerived*)" << endl;

            my_MostDerived *pMost = (my_MostDerived *) mX.allocate();
            const my_MostDerived *pMostCONST = pMost;

            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
            new(pMost) my_MostDerived;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            mX.deleteObject(pMostCONST);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\tdeleteObject(my_LeftBase*)" << endl;

            pMost = (my_MostDerived *) mX.allocate();

            new(pMost) my_MostDerived;
            const my_LeftBase *pLeftCONST = pMost;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            mX.deleteObject(pLeftCONST);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\tdeleteObject(my_RightBase*)" << endl;

            pMost = (my_MostDerived *) mX.allocate();

            new(pMost) my_MostDerived;
            const my_RightBase *pRightCONST = pMost;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            mX.deleteObject(pRightCONST);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\tdeleteObject(my_VirtualBase*)" << endl;

            pMost = (my_MostDerived *) mX.allocate();

            new(pMost) my_MostDerived;
            const my_VirtualBase *pVirtualCONST = pMost;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            mX.deleteObject(pVirtualCONST);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\tWith a null pointer" << endl;

            pMost = 0;
            mX.deleteObject(pMost);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // RESERVECAPACITY TEST
        //
        // Testing:
        //   void reserveCapacity(numObjects);
        // --------------------------------------------------------------------
        if (verbose) cout << endl << "RESERVECAPACITY TEST" << endl
                                  << "====================" << endl;

        if (verbose) cout << "\nTesting 'reserveCapacity'." << endl;

        bslma_TestAllocator a;    const bslma_TestAllocator& A = a;
        {
            const int OBJECT_SIZE = 100;
            const int NUM_OBJECTS = 15;

            int baselineAllocations = 0;
            int ret = 0;
            bsl::vector<void *> v(NUM_OBJECTS, (void *)0);


            const int N1 = 5;
            Obj mX(OBJECT_SIZE, NUM_OBJECTS, &a);
            ret = mX.reserveCapacity(N1);
            ASSERT(ret == 0);
            baselineAllocations = a.numAllocations();

            for (int i = 0; i < N1; ++i) {
                v[i] = mX.allocate();
            }
            ASSERT(A.numAllocations() == baselineAllocations);

            for (int i = 0; i < N1; ++i) {
                mX.deallocate(v[i]);
            }

            ret = mX.reserveCapacity(N1);
            ASSERT(ret == 0);
            ASSERT(A.numAllocations() == baselineAllocations);

            for (int i = 0; i < N1; ++i) {
                v[i] = mX.allocate();
            }
            ASSERT(A.numAllocations() == baselineAllocations);

            mX.deallocate(v[N1-1]);
            ret = mX.reserveCapacity(1);
            ASSERT(ret == 0);
            ASSERT(A.numAllocations() == baselineAllocations);
            ret = mX.reserveCapacity(2);
            ASSERT(ret == 0);
            //ASSERT(A.numAllocations() > baselineAllocations);
            baselineAllocations = a.numAllocations();

            v[N1-1] = mX.allocate();
            ASSERT(A.numAllocations() == baselineAllocations);

            ret = mX.reserveCapacity(NUM_OBJECTS);
            ASSERT(ret == N1);

            for (int i = 0; i < N1; ++i) {
                mX.deallocate(v[i]);
            }
            ret = mX.reserveCapacity(NUM_OBJECTS);
            ASSERT(ret == 0);
        }
        ASSERT(0 == A.numBytesInUse());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE - Make sure main usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE TEST" << endl
                                  << "==================" << endl;

        if (verbose) cout << "\nTesting 'my_JobQueue'." << endl;

        bslma_TestAllocator a;
        {
            my_JobQueue q(10, &a);
            double result;

            bdef_Function<void(*)(void)> job = bdef_BindUtil::bind(&sum5,
                                                                   &result,
                                                                   1.0,
                                                                   2.0,
                                                                   3.0,
                                                                   4.0,
                                                                   5.0);

            q.enqueueJob(job);
            q.enqueueJob(job);
            ASSERT(0 == q.tryExecuteJob());
            ASSERT(15.0 == result);
        }
        ASSERT(0 < a.numAllocations());
        ASSERT(0 == a.numBytesInUse());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // RELEASE TEST
        //   Initialize two pools with varying object sizes and 'numObjects',
        //   and supply each with its own test allocator.  Invoke 'allocate'
        //   repeatedly.  Invoke 'release' on one pool, and allow the other
        //   pool to go out of scope.  Verify that both allocators indicate all
        //   memory has been released by the pools.
        //
        // Testing:
        //   ~bcema_FixedPool();
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RELEASE TEST" << endl
                                  << "============" << endl;

        if (verbose) cout << "\nTesting 'release' and destructor." << endl;

        struct {
            int d_line;
            int d_objectSize;
            int d_numObjects;
        } DATA[] = {
            //line    object
            //no.     size      numObjects
            //----    ------    ----------------
            { L_,       1,                    500 },
            { L_,       5,                   1000 }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int NUM_REQUESTS = 100;
        bslma_TestAllocator taX;    const bslma_TestAllocator& TAX = taX;
        bslma_TestAllocator taY;    const bslma_TestAllocator& TAY = taY;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_line;
            const int OBJECT_SIZE = DATA[di].d_objectSize;
            const int NUM_OBJECTS = DATA[di].d_numObjects;
            {
                Obj mX(OBJECT_SIZE, NUM_OBJECTS, &taX);
                Obj mY(OBJECT_SIZE, NUM_OBJECTS, &taY);

                for (int ai = 0; ai < NUM_REQUESTS; ++ai) {
                    mX.allocate();
                    mY.allocate();
                }

                if (veryVerbose) { TAB; P_(TAX.numBytesInUse()); }
                mX.release();
                if (veryVerbose) { TAB; P(TAX.numBytesInUse()); }

                if (veryVerbose) { TAB; P_(TAY.numBytesInUse()); }
                // Let 'mY' go out of scope.
            }
            if (veryVerbose) { TAB; P(TAY.numBytesInUse()); }

            LOOP2_ASSERT(LINE, di, 0 == TAX.numBytesInUse());
            LOOP2_ASSERT(LINE, di, 0 == TAY.numBytesInUse());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // RELEASE TEST
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RELEASE TEST" << endl
                                  << "============" << endl;

        if (verbose) cout << "\nTesting 'release' and destructor." << endl;

        bslma_TestAllocator a;    const bslma_TestAllocator& A = a;
        {
            const int OBJECT_SIZE = 100;
            const int NUM_OBJECTS = 15;

            int baselineAllocations = 0;
            bsl::vector<void *> v(NUM_OBJECTS, (void *)0);


            const int N1 = 5;
            Obj mX(OBJECT_SIZE, NUM_OBJECTS, &a);

            for (int i = 0; i < N1; ++i) {
                v[i] = mX.allocate();
            }

            for (int i = 0; i < N1; ++i) {
                mX.deallocate(v[i]);
            }

            void *p = mX.allocate();
            mX.deallocate(p);
            baselineAllocations = a.numAllocations();
            p = mX.allocate();
            mX.deallocate(p);
            ASSERT(A.numAllocations() == baselineAllocations);

            mX.release();
            p = mX.allocate();
            mX.deallocate(p);
            ASSERT(A.numAllocations() > baselineAllocations);
        }
        ASSERT(0 == A.numBytesInUse());
        {
            const int OBJECT_SIZE = 100;
            const int NUM_OBJECTS = 15;

            int baselineAllocations = 0;
            bsl::vector<void *> v(NUM_OBJECTS, (void *)0);


            const int N1 = NUM_OBJECTS;
            Obj mX(OBJECT_SIZE, NUM_OBJECTS, &a);

            for (int i = 0; i < N1; ++i) {
                v[i] = mX.allocate();
            }

            for (int i = 0; i < N1; ++i) {
                mX.deallocate(v[i]);
            }

            void *p = mX.allocate();
            mX.deallocate(p);
            baselineAllocations = a.numAllocations();
            p = mX.allocate();
            mX.deallocate(p);
            ASSERT(A.numAllocations() == baselineAllocations);

            mX.release();
            p = mX.allocate();
            mX.deallocate(p);
            ASSERT(A.numAllocations() > baselineAllocations);
        }
        ASSERT(0 == A.numBytesInUse());
        {
            const int OBJECT_SIZE = 100;
            const int NUM_OBJECTS = 15;

            int baselineAllocations = 0;
            Obj mX(OBJECT_SIZE, NUM_OBJECTS, &a);

            mX.release();
            baselineAllocations = a.numAllocations();
            void *p = mX.allocate();
            mX.deallocate(p);
            ASSERT(A.numAllocations() > baselineAllocations);
        }
        ASSERT(0 == A.numBytesInUse());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // POINTER/INDEX conversion test
        //
        // Testing:
        //   addressFromIndex()/indexFromAddress();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "POINTER/INDEX" << endl
                                  << "=============" << endl;

        if (verbose) cout <<
                  "\nTesting 'addressFromIndex()/indexFromAddress()'." << endl;

        bslma_TestAllocator a;    const bslma_TestAllocator& A = a;
        {
            const int OBJECT_SIZE = 100;
            const int NUM_OBJECTS = 15;

            bsl::vector<void *> v(NUM_OBJECTS, (void *)0);


            Obj mX(OBJECT_SIZE, NUM_OBJECTS, &a);
            const Obj& X = mX;

            for (int i = 0; i < NUM_OBJECTS; ++i) {
                v[i] = mX.allocate();
                ASSERT(X.addressFromIndex(i) == v[i]);
                ASSERT(X.indexFromAddress(v[i]) == i);
            }
        }
        ASSERT(0 == A.numBytesInUse());
        {
            const int OBJECT_SIZE = 1;
            const int NUM_OBJECTS = 15;

            bsl::vector<void *> v(NUM_OBJECTS, (void *)0);


            Obj mX(OBJECT_SIZE, NUM_OBJECTS, &a);
            const Obj& X = mX;

            for (int i = 0; i < NUM_OBJECTS; ++i) {
                v[i] = mX.allocate();
                ASSERT(X.addressFromIndex(i) == v[i]);
                ASSERT(X.indexFromAddress(v[i]) == i);
            }
        }
        ASSERT(0 == A.numBytesInUse());
      } break;
      case -1: {
        // ---------------------------------------------------------
        // Fill and release test
        // ---------------------------------------------------------
        if (verbose) cout << endl
                          << "Fill and release test" << endl
                          << "=====================" << endl;
        enum {
            NUM_THREADS = 4,
            NUM_ITERATIONS = 50,
            NUM_OBJECTS = 10
        };

        int numThreads = argc > 2 ? atoi(argv[2]) : NUM_THREADS;
        int numIterations = argc > 3 ? atoi(argv[3]) : NUM_ITERATIONS;
        int numObjects = argc > 4 ? atoi(argv[4]) : NUM_OBJECTS;

        if (verbose) cout << endl
                          << "NUM THREADS: " << numThreads << endl
                          << "NUM ITERATIONS: " << numIterations << endl
                          << "POOL SIZE: " << numObjects << endl;

        fill_and_release::runtest(numIterations, numObjects, numThreads);

      } break;
      case -2: {
        // ---------------------------------------------------------
        // BENCHMARK
        // ---------------------------------------------------------
        if (verbose) cout << endl
                          << "Benchmark" << endl
                          << "=========" << endl;
        enum {
            NUM_THREADS = 4,
            NUM_ITERATIONS = 50,
            NUM_OBJECTS = 10,
            LOAD = 16,
            BACKOFF = 0
        };

        int numThreads = argc > 2 ? atoi(argv[2]) : NUM_THREADS;
        int numIterations = argc > 3 ? atoi(argv[3]) : NUM_ITERATIONS;
        int numObjects = argc > 4 ? atoi(argv[4]) : NUM_OBJECTS;
        int load = argc > 5 ? atoi(argv[5]) : LOAD;
        int backoff = argc > 6 ? atoi(argv[6]) : BACKOFF;

        if (verbose) cout << endl
                          << "NUM THREADS: " << numThreads << endl
                          << "NUM ITERATIONS: " << numIterations << endl
                          << "POOL SIZE: " << numObjects * numThreads << endl
                          << "WORK LOAD: " << load << endl
                          << "BACKOFF: " << backoff << endl;

        bench::runtest(numIterations, numObjects, numThreads, load, backoff);

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
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
