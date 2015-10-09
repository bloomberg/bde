// bdlma_concurrentfixedpool.t.cpp                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlma_concurrentfixedpool.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>          // for testing only
#include <bslma_testallocatorexception.h> // for testing only

#include <bsls_alignment.h>
#include <bsls_platform.h>

#include <bdlb_random.h>

#include <bdlf_bind.h>

#include <bslmt_barrier.h>
#include <bslmt_threadgroup.h>
#include <bslmt_qlock.h>

#include <bsls_types.h>

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'memcpy'
#include <bsl_deque.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

//=============================================================================
//                   GLOBAL TYPEDEFS, CONSTANTS, AND VARIABLES
//-----------------------------------------------------------------------------

typedef bdlma::ConcurrentFixedPool Obj;

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

///Usage
///-----
// 'bdlma::ConcurrentFixedPool' is intended to implement *out-of-place*
// container classes that hold up to a fixed number of elements, all of uniform
// size.  Suppose we wish to implement a simple thread pool.  We want the
// equivalent of a 'bsl::deque<bsl::function<void(void)> >'.  However, to
// minimize the time spent performing operations on this deque - which must be
// carried out under a lock - we instead store just pointers in the deque, and
// manage memory efficiently using 'bdlma::ConcurrentFixedPool'.
// 'bdlma::ConcurrentFixedPool' is fully thread-safe and does not require any
// additional synchronization.
//
// The example below is just for the container portion of our simple thread
// pool.  The implementation of the worker thread, and the requisite
// synchronization, are omitted for clarity.
//..
    class my_JobQueue {

      public:
        // PUBLIC TYPES
        typedef bsl::function<void(void)> Job;

      private:
        // DATA
        bslmt::Mutex                d_lock;
        bsl::deque<Job *>           d_queue;
        bdlma::ConcurrentFixedPool  d_pool;
        bslma::Allocator           *d_allocator_p;

        // Not implemented:
        my_JobQueue(const my_JobQueue&);

      public:
        // CREATORS
        my_JobQueue(int maxJobs, bslma::Allocator *basicAllocator = 0);
        ~my_JobQueue();

        // MANIPULATORS
        void enqueueJob(const Job& job);

        int tryExecuteJob();
    };

    my_JobQueue::my_JobQueue(int maxJobs, bslma::Allocator *basicAllocator)
    : d_queue(basicAllocator)
    , d_pool(sizeof(Job), maxJobs, basicAllocator)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    my_JobQueue::~my_JobQueue()
    {
        Job *jobPtr;
        while (!d_queue.empty()) {
            jobPtr = d_queue.front();
            jobPtr->~Job();
            d_queue.pop_front();
        }
    }

    void my_JobQueue::enqueueJob(const Job& job)
    {
        Job *jobPtr = new (d_pool) Job(bsl::allocator_arg_t(),
                                       bsl::allocator<Job>(d_allocator_p),
                                       job);
        d_lock.lock();
        d_queue.push_back(jobPtr);
        d_lock.unlock();
    }

    int my_JobQueue::tryExecuteJob()
    {
        d_lock.lock();
        if (d_queue.empty()) {
            d_lock.unlock();
            return -1;                                                // RETURN
        }
        Job *jobPtr = d_queue.front();
        d_queue.pop_front();
        d_lock.unlock();
        (*jobPtr)();
        d_pool.deleteObject(jobPtr);
        return 0;
    }
//..
// Note that in the destructor, there is no need to deallocate the individual
// job objects - the destructor of 'bdlma::ConcurrentFixedPool' will release
// any remaining allocated memory.  However, it *is* necessary to invoke the
// destructors of all these objects, as the destructor of
// 'bdlma::ConcurrentFixedPool' will not do so.

void sum5(double* result,
          double  op1,
          double  op2,
          double  op3,
          double  op4,
          double  op5)
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
    k_OBJECT_SIZE = 56,
    k_NUM_INTS = k_OBJECT_SIZE / sizeof(int),
    k_NUM_OBJECTS = 10000,
    k_NUM_THREADS = 4
};

bslmt::Barrier barrier(k_NUM_THREADS);
extern "C"
void *workerThread(void *arg) {
    Obj *mX = (Obj *) arg;
    ASSERT(k_OBJECT_SIZE == mX->objectSize());

    barrier.wait();
    for (int i = 0; i < k_NUM_OBJECTS; ++i) {
        int *buffer = (int*)mX->allocate();
        if (veryVeryVerbose) {
            printf("Thread %d: Allocated %p\n",
                   static_cast<int>(bslmt::ThreadUtil::selfIdAsUint64()),
                   buffer);
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
//                              BENCHMARKS
//-----------------------------------------------------------------------------

namespace bench {

struct Item {
    int  d_threadId;
    int  d_work;
};

struct Control {
    bslmt::Barrier         *d_barrier;
    bdlma::ConcurrentFixedPool       *d_fixedpool;
    int                    d_iterations;
    int                    d_numObjects;
    int                    d_load;
};

void hardwork(Item *item, int load)
{
    item->d_work = item->d_threadId;
    for (int i=0; i<load; ++i) {
        bdlb::Random::generate15(&item->d_work);
    }
}

void bench(Control *control)
{
    int threadId = static_cast<int>(bslmt::ThreadUtil::selfIdAsInt());

    bdlma::ConcurrentFixedPool *pool = control->d_fixedpool;
    int numObjects = control->d_numObjects;
    int load = control->d_load;

    bsl::vector<Item *> objects(numObjects, (Item *)0);

    control->d_barrier->wait();

    for (int i=0; i<control->d_iterations; i++) {
        for (int j=0; j<numObjects; j++) {
            for (int t=0; t<=j; t++) {
                Item *item = (Item *)pool->allocate();
                ASSERT(item);
                item->d_threadId = threadId;
                objects[t] = item;
            }
            for (int t=0; t<=j; t++) {
                Item *item = objects[t];
                ASSERT(item->d_threadId == threadId);
                hardwork(item, load);
            }
            for (int t=0; t<=j; t++) {
                Item *item = objects[t];
                ASSERT(item->d_threadId == threadId);
                pool->deallocate(item);
            }
        }
    }
}

void runtest(int numIterations,
             int numObjects,
             int numThreads,
             int load,
             int backoff)
{
    bdlma::ConcurrentFixedPool pool(sizeof(Item), numThreads * numObjects);
    pool.setBackoffLevel(backoff);

    ASSERT(backoff == pool.backoffLevel());

    bslmt::Barrier barrier(numThreads);

    Control control;

    control.d_barrier = &barrier;
    control.d_fixedpool = &pool;
    control.d_iterations = numIterations;
    control.d_numObjects = numObjects;
    control.d_load = load;

    bslmt::ThreadGroup tg;
    tg.addThreads(bdlf::BindUtil::bind(&bench,&control), numThreads);

    tg.joinAll();
}
}  // close namespace bench

//=============================================================================
//                    fill and release concurrently
//-----------------------------------------------------------------------------

namespace fill_and_release {

struct Item {
    int  d_threadId;
};

struct Control {
    bslmt::Barrier         *d_barrier;
    bdlma::ConcurrentFixedPool       *d_fixedpool;
    int                    d_iterations;
    bsls::AtomicInt         d_allocationCount;
};

void bench(Control *control)
{
    bdlma::ConcurrentFixedPool *pool = control->d_fixedpool;
    const int poolSize = pool->poolSize();
    const int iterations = control->d_iterations;
    bsls::AtomicInt& allocationCount(control->d_allocationCount);

    for (int i = 0; i<iterations; i++) {

        int count = allocationCount++;
        while (count < poolSize) {
            void *p = pool->allocate();
            ASSERT(p);
            pool->deallocate(p);
            p = pool->allocate();
            ASSERT(p);
            count = allocationCount++;
        }

        // sync up with other threads
        control->d_barrier->wait();

        // To assert that allocate() returns 0, we need to make sure that all
        // threads have reached this point (so it has to be done after the
        // barrier.  But we need to make sure that no thread is about to call
        // release() so we have put another barrier after the assert.

        ASSERT(pool->allocate() == 0);

        control->d_barrier->wait();
        if (count == poolSize) {
            // One thread will release all the objects.

            pool->release();
            allocationCount = 0;
        }
        bslmt::ThreadUtil::yield(); // exhaust time slice
        control->d_barrier->wait();
    }
}

void runtest(int numIterations, int numObjects, int numThreads)
{
    bdlma::ConcurrentFixedPool pool(sizeof(Item), numObjects);

    bslmt::Barrier barrier(numThreads);

    Control control;

    control.d_barrier = &barrier;
    control.d_fixedpool = &pool;
    control.d_iterations = numIterations;
    control.d_allocationCount = 0;

    bslmt::ThreadGroup tg;
    tg.addThreads(bdlf::BindUtil::bind(&bench,&control), numThreads);

    tg.joinAll();
}
}  // close namespace fill_and_release

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

        bslma::TestAllocator a;
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

        bslma::TestAllocator alloc, *Z = &alloc;

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
                ASSERT((const void*) pLCC == (const void*) pMDC);
                fp.deleteObjectRaw(pLCC);
              } break;
              case 2: {
                fp.deleteObject(pMDC);
              } break;
              case 3: {
                const LeftChild *pLCC = pMDC;
                ASSERT((const void*) pLCC == (const void*) pMDC);
                fp.deleteObject(pLCC);
              } break;
              case 4: {
                const RightChild *pRCC = pMDC;
                ASSERT((const void*) pRCC != (const void*) pMDC);
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
            k_NUM_THREADS = 4,
            k_NUM_ITERATIONS = 50,
            k_NUM_OBJECTS = 10
        };

        int numIterations = k_NUM_ITERATIONS;
        int numObjects = k_NUM_OBJECTS;

        for (int numThreads=1; numThreads<=k_NUM_THREADS; numThreads++) {

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
            k_NUM_THREADS = 4,
            k_NUM_ITERATIONS = 50,
            k_NUM_OBJECTS = 10,
            k_LOAD = 16,
            k_BACKOFF = 0
        };

        int numIterations = k_NUM_ITERATIONS;
        int numObjects = k_NUM_OBJECTS;
        int load = k_LOAD;
        int backoff = k_BACKOFF;

        for (int numThreads=1; numThreads<=k_NUM_THREADS; numThreads++) {
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
        bslmt::ThreadUtil::Handle threads[k_NUM_THREADS];
        Obj mX(k_OBJECT_SIZE, k_NUM_THREADS);
        for (int i = 0; i < k_NUM_THREADS; ++i) {
            int rc =
                bslmt::ThreadUtil::create(&threads[i], workerThread, &mX);
            LOOP_ASSERT(i, 0 == rc);
        }
        for (int i = 0; i < k_NUM_THREADS; ++i) {
            int rc =
                bslmt::ThreadUtil::join(threads[i]);
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
            bslma::TestAllocator a(veryVeryVerbose);
            const bslma::TestAllocator& A = a;

            const int OBJECT_SIZE = sizeof(my_Class1);
            ASSERT(sizeof(my_Class2) == OBJECT_SIZE);
            const int NUM_OBJECTS = 2;
            Obj mX(OBJECT_SIZE, NUM_OBJECTS, &a);

            if (verbose) cout << "\twith a my_Class1 object" << endl;

            my_ClassCode=0;

            my_Class1 *pC1 = (my_Class1 *) mX.allocate();
            new(pC1) my_Class1;
            if (verbose) { T_;  T_;  P(my_ClassCode); }
            ASSERT(1 == my_ClassCode);
            //ASSERT(A.numAllocations() == 1);

            mX.deleteObject(pC1);
            if (verbose) { T_;  T_;  P(my_ClassCode); }
            ASSERT(2 == my_ClassCode);
            //ASSERT(A.numAllocations() == 1);
            mX.allocate();
            //ASSERT(A.numAllocations() == 1);
                  // By observing that the number of allocations stays at one
                  // we confirm that the memory obtained from the pool has been
                  // returned by 'deleteObject'.  Had it not been returned, the
                  // call to allocate would have required another allocation
                  // from the allocator.

            if (verbose) cout << "\twith a my_Class2 object" << endl;

            my_Class2 *pC2 = (my_Class2 *) mX.allocate();
            new(pC2) my_Class2;
            if (verbose) { T_;  T_;  P(my_ClassCode); }
            ASSERT(3 == my_ClassCode);
            //ASSERT(A.numAllocations() == 1);

            mX.deleteObject(pC2);
            if (verbose) { T_;  T_;  P(my_ClassCode); }
            ASSERT(4 == my_ClassCode);
            //ASSERT(A.numAllocations() == 1);
            mX.allocate();
            //ASSERT(A.numAllocations() == 1);
        }

        if (verbose) cout << "\nTesting 'deleteObject' on polymorphic types:"
                          << endl;
        {
            bslma::TestAllocator a(veryVeryVerbose);

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

        bslma::TestAllocator a;    const bslma::TestAllocator& A = a;
        {
            const int OBJECT_SIZE = 100;
            const int NUM_OBJECTS = 15;

            bsls::Types::Int64 baselineAllocations = 0;
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

        bslma::TestAllocator a;
        {
            my_JobQueue q(10, &a);
            double result;

            bsl::function<void(void)> job = bdlf::BindUtil::bind(&sum5,
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
        //   ~bdlma::ConcurrentFixedPool();
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
        bslma::TestAllocator taX;    const bslma::TestAllocator& TAX = taX;
        bslma::TestAllocator taY;    const bslma::TestAllocator& TAY = taY;

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

                if (veryVerbose) { T_; P_(TAX.numBytesInUse()); }
                mX.release();
                if (veryVerbose) { T_; P(TAX.numBytesInUse()); }

                if (veryVerbose) { T_; P_(TAY.numBytesInUse()); }
                // Let 'mY' go out of scope.
            }
            if (veryVerbose) { T_; P(TAY.numBytesInUse()); }

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

        bslma::TestAllocator a;    const bslma::TestAllocator& A = a;
        {
            const int OBJECT_SIZE = 100;
            const int NUM_OBJECTS = 15;

            bsls::Types::Int64  baselineAllocations = 0;
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

            bsls::Types::Int64 baselineAllocations = 0;
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

            bsls::Types::Int64 baselineAllocations = 0;
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

        bslma::TestAllocator a;    const bslma::TestAllocator& A = a;
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
            k_NUM_THREADS = 4,
            k_NUM_ITERATIONS = 50,
            k_NUM_OBJECTS = 10
        };

        int numThreads = argc > 2 ? atoi(argv[2]) : k_NUM_THREADS;
        int numIterations = argc > 3 ? atoi(argv[3]) : k_NUM_ITERATIONS;
        int numObjects = argc > 4 ? atoi(argv[4]) : k_NUM_OBJECTS;

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
            k_NUM_THREADS = 4,
            k_NUM_ITERATIONS = 50,
            k_NUM_OBJECTS = 10,
            k_LOAD = 16,
            k_BACKOFF = 0
        };

        int numThreads = argc > 2 ? atoi(argv[2]) : k_NUM_THREADS;
        int numIterations = argc > 3 ? atoi(argv[3]) : k_NUM_ITERATIONS;
        int numObjects = argc > 4 ? atoi(argv[4]) : k_NUM_OBJECTS;
        int load = argc > 5 ? atoi(argv[5]) : k_LOAD;
        int backoff = argc > 6 ? atoi(argv[6]) : k_BACKOFF;

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
