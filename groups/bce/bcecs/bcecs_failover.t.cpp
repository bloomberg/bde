// bcecs_failover.t.cpp      -*-C++-*-
#include <bcecs_failover.h>

#include <bcema_testallocator.h>
#include <bcemt_lockguard.h>
#include <bcemt_thread.h>
#include <bcemt_barrier.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bsls_platform.h>
#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
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
static bcemt_Mutex printMutex;  // mutex to protect output macros
#define PT(X) { printMutex.lock(); P(X); printMutex.unlock(); }
#define PT_(X) { printMutex.lock(); P_(X); printMutex.unlock(); }

static bcemt_Mutex &assertMutex = printMutex; // mutex to protect assert macros

#define ASSERTT(X) {assertMutex.lock(); aSsErT(!(X), #X, __LINE__); \
                                          assertMutex.unlock();}

#define LOOP_ASSERTT(I,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << endl; \
       aSsErT(1, #X, __LINE__); } assertMutex.unlock(); }

#define LOOP2_ASSERTT(I,J,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << endl; aSsErT(1, #X, __LINE__); } \
       assertMutex.unlock(); }

#define LOOP3_ASSERTT(I,J,K,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << "\t" << #K << ": " << K << endl; \
       aSsErT(1, #X, __LINE__); } assertMutex.unlock(); }

#define LOOP4_ASSERTT(I,J,K,L,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << "\t" << #K << ": " << K << "\t" << #L \
       << ": " << L << endl; aSsErT(1, #X, __LINE__); } assertMutex.unlock(); }

#define LOOP5_ASSERTT(I,J,K,L,M,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << "\t" << #K << ": " << K << "\t" << #L \
       << ": " << L << "\t" << #M << ": " << M << endl; \
       aSsErT(1, #X, __LINE__); } assertMutex.unlock(); }

//=============================================================================
//          GLOBAL TYPEDEFS/CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
//static int verbose;
//static int veryVerbose;
// static int veryVeryVerbose;  // not used

class TestResourcePool : public bcecs_ResourcePool<int> {
    // Test implementation of the 'bcecs_ResourcePool' protocol.

    bool                      d_valid;              // valid state
    int                       d_allocated;          // number of allocated
                                                    // resources
    int                       d_maxResources;       // max number of
                                                    // resources that can be
                                                    // allocated (0 means no
                                                    // max)
    bsl::vector<int*>         d_allocatedPointers;  // list of allocated
                                                    // pointers
    bdef_Function<void (*)()> d_resCb;              // resource ready cb
    mutable bcemt_Mutex       d_lock;               // lock
    bslma_Allocator          *d_allocator_p;        // (held)

  private:
    // not implemented
    TestResourcePool(TestResourcePool&);
    TestResourcePool& operator=(const TestResourcePool&);

    // PRIVATE MANIPULATORS
    static void reInsertInt(int *ret, TestResourcePool* pool);
        // Return an int allocated by this pool.

  public:
    // CREATORS
    explicit TestResourcePool(bslma_Allocator *basicAllocator = 0);
        // Create a new 'TestResourcePool'.

    virtual ~TestResourcePool();
        // Destroy this object.

    // MANIPULATORS
    virtual int tryGetObject(bdema_ManagedPtr<int> *ptr, Handle *);
        // Transfer into the specified 'ptr' a resource from this pool.
        // Return 0 on success or if 'ptr' is null but this pool contains a
        // resource that could have been transferred, and a non-zero value
        // otherwise.  Note that the resource is removed from this pool on
        // successful transfer.

    virtual void invalidate(Handle);
        // Set the pool as invalid.  This pool will not deliver any more
        // resources until it is manually revalidated.

    void validate();
        // Revalidate pool.

    void setResourceReadyCb(const bdef_Function<void (*)()>& cb);
        // Set the callback that will be called when resources will be
        // available again to the specified 'cb'.

    void setMaxResources(int max = 0);
        // Set the maximum number of resource that can be allocated at the
        // same time from this pool to the specified 'max' if max is
        // non-zero positive value, and set this maximum to the infinity
        // otherwise.

    // ACCESSORS
    virtual bool isValid() const;
        // Return 'true' if this pool is valid and 'false' otherwise.

    int numAllocated() const;
        // Return the number of allocated resources from this pool.

    int maxResources() const;
        // Return the maximum number of resources that can be allocated from
        // this pool at the same time if any, and 0 otherwise.
};

// PRIVATE MANIPULATORS
void TestResourcePool::reInsertInt(int *ret, TestResourcePool *pool)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&pool->d_lock);
    bsl::vector<int*>::iterator it = bsl::lower_bound(
                                             pool->d_allocatedPointers.begin(),
                                             pool->d_allocatedPointers.end(),
                                             ret);
    ASSERT(pool->d_allocatedPointers.end() != it);
    ASSERT(*it == ret);
    pool->d_allocatedPointers.erase(it);
    pool->d_allocator_p->deleteObjectRaw(ret);

    if (pool->d_resCb && pool->d_valid && pool->d_maxResources != 0 &&
        pool->d_allocated == pool->d_maxResources) {
        pool->d_resCb();
    }
    --pool->d_allocated;
}

// CREATORS
TestResourcePool::TestResourcePool(bslma_Allocator* basicAllocator)
: d_valid(true)
, d_allocated(0)
, d_maxResources(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

TestResourcePool::~TestResourcePool()
{
    ASSERT(0 == d_allocated);
    ASSERT(d_allocatedPointers.empty());
}

// MANIPULATORS
int TestResourcePool::tryGetObject(bdema_ManagedPtr<int> *ptr, Handle *)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    if (d_valid && (d_maxResources == 0 || d_allocated < d_maxResources)) {
        if (ptr) {
            int *res = new (*d_allocator_p) int;
            bsl::vector<int*>::iterator it = bsl::lower_bound(
                                                d_allocatedPointers.begin(),
                                                d_allocatedPointers.end(),
                                                res);
            if (d_allocatedPointers.end() == it) {
                d_allocatedPointers.push_back(res);
            }
            else {
                d_allocatedPointers.insert(it, res);
            }
            ++d_allocated;
            bdema_ManagedPtr<int> tmp(res, this,
                                      &TestResourcePool::reInsertInt);
            *ptr = tmp;
        }
        return 0;
    }
    return 1;
}

void TestResourcePool::invalidate(Handle)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    d_valid = false;
}

void TestResourcePool::setResourceReadyCb(
                                         const bdef_Function<void(*)()>& cb)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    d_resCb = cb;
}

void TestResourcePool::setMaxResources(int max)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    if (max < 0) {
        max = 0;
    }
    if (d_resCb &&
        d_allocated >= d_maxResources && (max == 0 || max > d_allocated)) {
        d_resCb();
    }
    d_maxResources = max;
}

void TestResourcePool::validate()
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    if (d_resCb && !d_valid &&
        (d_maxResources == 0 || d_allocated < d_maxResources)) {
        d_resCb();
    }
    d_valid = true;
}

// ACCESSORS
bool TestResourcePool::isValid() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    return d_valid;
}

int TestResourcePool::numAllocated() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    return d_allocated;
}

int TestResourcePool::maxResources() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    return d_maxResources;
}

//=============================================================================
//                   HELPER CLASSES AND FUNCTIONS FOR TESTING
//=============================================================================
struct ThreadInfo5 {
    bcemt_Barrier          *d_barrier;
    bcecs_FailoverSet<int> *d_set;
    int                     d_numResources;
    bces_AtomicInt         *d_timeout;
};

//extern "C" XXX commented out to work around a bug in Forte 6
void *thread5(void * arg) {
    const ThreadInfo5& t = *(ThreadInfo5 *) arg;

    // invalid
    t.d_barrier->wait();
    {
        bcecs_FailoverResource<int> res;
        ASSERT(0 != t.d_set->getResourceTimed(&res,
                                              bdetu_SystemTime::now() +
                                            bdet_TimeInterval(0, 1000 * 100)));
    }
    t.d_barrier->wait();
    // revalidation
    t.d_barrier->wait();
    for (int i = 0; i < t.d_numResources; ++i) {
        bcecs_FailoverResource<int> res;
        ASSERT(0 == t.d_set->getResourceTimed(&res,
                                              bdetu_SystemTime::now() +
                                            bdet_TimeInterval(0, 1000 * 1)));
    }
    t.d_barrier->wait();
    // set max resources
    t.d_barrier->wait();
    for (int i = 0; i < t.d_numResources; ++i) {
        bcecs_FailoverResource<int> res;
        if (0 != t.d_set->getResourceTimed(&res,
                                              bdetu_SystemTime::now() +
                                            bdet_TimeInterval(0, 10 * 1000))) {
            --i;
            ++*t.d_timeout;
        }
    }
    return 0;
}

struct ThreadInfo4 {
    bcemt_Barrier          *d_barrier;
    bcecs_FailoverSet<int> *d_set;
    int                     d_numResources;
};

//extern "C" XXX commented out to work around a bug in Forte 6
void *thread4(void *arg) {
    const ThreadInfo4& t = *(ThreadInfo4 *) arg;

    t.d_barrier->wait();
    for (int i = 0; i < t.d_numResources; ++i) {
        bcecs_FailoverResource<int> res;
        t.d_set->getResource(&res);
    }
    t.d_barrier->wait();
    // invalidate during that time
    t.d_barrier->wait();
    for (int i = 0; i < t.d_numResources; ++i) {
        bcecs_FailoverResource<int> res;
        t.d_set->getResource(&res);
    }
    t.d_barrier->wait();
    // set max
    t.d_barrier->wait();
    for (int i = 0; i < t.d_numResources; ++i) {
        bcecs_FailoverResource<int> res;
        t.d_set->getResource(&res);
    }
    return 0;
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

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'getResourceTimed'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   getResourceTimed(bcecs_FailoverResource<TYPE> *resource,
        //                    bdet_TimeInterval             timeout)
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'getResourceTimed'" << endl
                          << "==========================" << endl;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            bcecs_FailoverSet<int> set(&ta);
            TestResourcePool *pool1 = new (ta) TestResourcePool(&ta);
            ASSERT(pool1);
            bdema_ManagedPtr<bcecs_ResourcePool<int> > pool1M(pool1, &ta);

            ASSERT(pool1->isValid());
            set.registerPool(pool1M, 0);

            typedef bcecs_FailoverResource<int> Res;
            // Test simple allocation.
            {
                Res r; ASSERT(!r.isValid()); ASSERT(!r.resource());
                ASSERT(0 == set.getResourceTimed(&r,
                                                 bdetu_SystemTime::now() +
                                                 bdet_TimeInterval(1000)));
                ASSERT(r.isValid());
                ASSERT(r.resource());
                *r.resource() = 123;
                ASSERT(1 == pool1->numAllocated());
            }
            ASSERT(0 == pool1->numAllocated());

            // Test multiple allocations.
            {
                Res r;
                ASSERT(0 == set.getResourceTimed(&r,
                                                 bdetu_SystemTime::now() +
                                                 bdet_TimeInterval(1000)));
                ASSERT(r.isValid());
                ASSERT(r.resource());
                ASSERT(1 == pool1->numAllocated());

                Res rr;
                ASSERT(0 == set.getResourceTimed(&rr,
                                                 bdetu_SystemTime::now() +
                                                 bdet_TimeInterval(1000)));
                ASSERT(rr.isValid());
                ASSERT(rr.resource());
                ASSERT(2 == pool1->numAllocated());

                Res rrr;
                ASSERT(0 == set.getResourceTimed(&rrr,
                                                 bdetu_SystemTime::now() +
                                                 bdet_TimeInterval(1000)));
                ASSERT(rrr.isValid());
                ASSERT(rrr.resource());
                ASSERT(3 == pool1->numAllocated());

                ASSERT(rrr.resource() != rr.resource());
                ASSERT(rrr.resource() !=  r.resource());
                *r.resource() = 123;
                *rr.resource() = 456;
                *rrr.resource() = 789;
            }
            ASSERT(0 == pool1->numAllocated());
        }
        {
            bcecs_FailoverSet<int> set(&ta);
            TestResourcePool *pool1 = new (ta) TestResourcePool(&ta);
            ASSERT(pool1);
            bdema_ManagedPtr<bcecs_ResourcePool<int> > pool1M(pool1, &ta);

            ASSERT(pool1->isValid());
            set.registerPool(pool1M, 0);

            enum {
                NUM_THREADS = 10
            };

            bcemt_ThreadUtil::Handle threads[NUM_THREADS];
            bcemt_Barrier barrier(NUM_THREADS + 1);
            bces_AtomicInt timeout = 0;

            struct ThreadInfo5 info;
            info.d_numResources = 200;
            info.d_barrier = &barrier;
            info.d_set     = &set;
            info.d_timeout = &timeout;

            for (int i = 0; i < NUM_THREADS; ++i) {
                ASSERT(0 == bcemt_ThreadUtil::create(&threads[i],
                                                     &thread5, &info));
            }
            pool1->invalidate(0);
            barrier.wait();
            barrier.wait();
            pool1->validate();
            barrier.wait();
            barrier.wait();
            pool1->setMaxResources(NUM_THREADS / 5);
            barrier.wait();

            for (int i = 0; i < NUM_THREADS; ++i) {
                ASSERT(0 == bcemt_ThreadUtil::join(threads[i]));
            }

            if (veryVerbose) {
                P(timeout);
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'getResource'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   getResource(bcecs_FailoverResource<TYPE> *resource)
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'getResource'" << endl
                          << "=====================" << endl;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            bcecs_FailoverSet<int> set(&ta);
            TestResourcePool *pool1 = new (ta) TestResourcePool(&ta);
            ASSERT(pool1);
            bdema_ManagedPtr<bcecs_ResourcePool<int> > pool1M(pool1, &ta);

            ASSERT(pool1->isValid());
            set.registerPool(pool1M, 0);

            typedef bcecs_FailoverResource<int> Res;
            // Test simple allocation.
            {
                Res r; ASSERT(!r.isValid()); ASSERT(!r.resource());
                set.getResource(&r);
                ASSERT(r.isValid());
                ASSERT(r.resource());
                *r.resource() = 123;
                ASSERT(1 == pool1->numAllocated());
            }
            ASSERT(0 == pool1->numAllocated());

            // Test multiple allocations.
            {
                Res r; set.getResource(&r);
                ASSERT(r.isValid());
                ASSERT(r.resource());
                ASSERT(1 == pool1->numAllocated());

                Res rr; set.getResource(&rr);
                ASSERT(rr.isValid());
                ASSERT(rr.resource());
                ASSERT(2 == pool1->numAllocated());

                Res rrr; set.getResource(&rrr);
                ASSERT(rrr.isValid());
                ASSERT(rrr.resource());
                ASSERT(3 == pool1->numAllocated());

                ASSERT(rrr.resource() != rr.resource());
                ASSERT(rrr.resource() !=  r.resource());
                *r.resource() = 123;
                *rr.resource() = 456;
                *rrr.resource() = 789;
            }
            ASSERT(0 == pool1->numAllocated());
        }
        {
            bcecs_FailoverSet<int> set(&ta);
            TestResourcePool *pool1 = new (ta) TestResourcePool(&ta);
            ASSERT(pool1);
            bdema_ManagedPtr<bcecs_ResourcePool<int> > pool1M(pool1, &ta);

            ASSERT(pool1->isValid());
            set.registerPool(pool1M, 0);

            enum {
                NUM_THREADS = 10
            };

            bcemt_ThreadUtil::Handle threads[NUM_THREADS];
            bcemt_Barrier barrier(NUM_THREADS + 1);

            struct ThreadInfo4 info;
            info.d_numResources = 200;
            info.d_barrier = &barrier;
            info.d_set     = &set;

            for (int i = 0; i < NUM_THREADS; ++i) {
                ASSERT(0 == bcemt_ThreadUtil::create(&threads[i],
                                                     &thread4, &info));
            }
            barrier.wait();
            barrier.wait();
            pool1->invalidate(0);
            barrier.wait();
            bcemt_ThreadUtil::microSleep(10 * 1000);
            pool1->validate();
            barrier.wait();
            pool1->setMaxResources(NUM_THREADS / 4);
            barrier.wait();

            for (int i = 0; i < NUM_THREADS; ++i) {
                ASSERT(0 == bcemt_ThreadUtil::join(threads[i]));
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING failover capacity
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING Failover capacity'" << endl
                          << "=========================" << endl;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            bcecs_FailoverSet<int> set(&ta);
            TestResourcePool *pool1 = new (ta) TestResourcePool(&ta);
            ASSERT(pool1);
            bdema_ManagedPtr<bcecs_ResourcePool<int> > pool1M(pool1, &ta);

            ASSERT(pool1->isValid());
            set.registerPool(pool1M, 0);

            TestResourcePool *pool2 = new (ta) TestResourcePool(&ta);
            ASSERT(pool2);

            // Make sure that nothing will be allocated from pool2 if pool1 is
            // valid.
            bdema_ManagedPtr<bcecs_ResourcePool<int> > pool2M(pool2, &ta);
            set.registerPool(pool2M, 1);
            ASSERT(!pool2M);

            typedef bcecs_FailoverResource<int> Res;
            {
                Res r; ASSERT(0 == set.tryGetResource(&r));
                ASSERT(r.isValid());
                ASSERT(r.resource());
                ASSERT(0 == r.poolPriority());
                ASSERT(0 == pool2->numAllocated());

                Res rr; ASSERT(0 == set.tryGetResource(&rr));
                ASSERT(rr.isValid());
                ASSERT(rr.resource());
                ASSERT(0 == rr.poolPriority());
                ASSERT(0 == pool2->numAllocated());

                Res rrr; ASSERT(0 == set.tryGetResource(&rrr));
                ASSERT(rrr.isValid());
                ASSERT(rrr.resource());
                ASSERT(0 == rrr.poolPriority());
                ASSERT(3 == pool1->numAllocated());
                ASSERT(0 == pool2->numAllocated());

                ASSERT(rrr.resource() != rr.resource());
                ASSERT(rrr.resource() !=  r.resource());
                *r.resource() = 123;
                *rr.resource() = 456;
                *rrr.resource() = 789;
                rrr.markAsInvalid();
            }
            ASSERT(0 == pool1->numAllocated());
            ASSERT(0 == pool2->numAllocated());

            // Only pool2 should provide resources since pool1 is invalid.
            {
                Res r; ASSERT(0 == set.tryGetResource(&r));
                ASSERT(r.isValid());
                ASSERT(r.resource());
                ASSERT(1 == r.poolPriority());
                ASSERT(0 == pool1->numAllocated());
                ASSERT(1 == pool2->numAllocated());

                Res rr; ASSERT(0 == set.tryGetResource(&rr));
                ASSERT(rr.isValid());
                ASSERT(rr.resource());
                ASSERT(1 == rr.poolPriority());
                ASSERT(0 == pool1->numAllocated());
                ASSERT(2 == pool2->numAllocated());

                Res rrr; ASSERT(0 == set.tryGetResource(&rrr));
                ASSERT(rrr.isValid());
                ASSERT(rrr.resource());
                ASSERT(1 == rrr.poolPriority());
                ASSERT(0 == pool1->numAllocated());
                ASSERT(3 == pool2->numAllocated());

                ASSERT(rrr.resource() != rr.resource());
                ASSERT(rrr.resource() !=  r.resource());
                *r.resource() = 123;
                *rr.resource() = 456;
                *rrr.resource() = 789;
            }
            ASSERT(0 == pool1->numAllocated());
            ASSERT(0 == pool2->numAllocated());
            pool1->validate();

            TestResourcePool *pool3 = new (ta) TestResourcePool(&ta);
            ASSERT(pool3);
            bdema_ManagedPtr<bcecs_ResourcePool<int> > pool3M(pool3, &ta);
            set.registerPool(pool3M, 0);
            ASSERT(!pool3M);
            ASSERT(0 == pool1->numAllocated());
            ASSERT(0 == pool2->numAllocated());
            ASSERT(0 == pool3->numAllocated());

            // Let's test round-robin, only pool1 and pool3 will be used.  We
            // will make sure that both pool1 and pool3 are used.
            {
                Res r; ASSERT(0 == set.tryGetResource(&r));
                ASSERT(r.isValid());
                ASSERT(r.resource());
                ASSERT(0 == r.poolPriority());
                ASSERT(1 == (pool1->numAllocated() + pool3->numAllocated()));
                ASSERT(0 == pool2->numAllocated());

                Res rr; ASSERT(0 == set.tryGetResource(&rr));
                ASSERT(rr.isValid());
                ASSERT(rr.resource());
                ASSERT(0 == rr.poolPriority());
                ASSERT(2 == (pool1->numAllocated() + pool3->numAllocated()));
                ASSERT(2 != pool1->numAllocated());
                ASSERT(2 != pool3->numAllocated());
                ASSERT(0 == pool2->numAllocated());

                Res rrr; ASSERT(0 == set.tryGetResource(&rrr));
                ASSERT(rrr.isValid());
                ASSERT(rrr.resource());
                ASSERT(0 == rrr.poolPriority());
                ASSERT(3 == (pool1->numAllocated() + pool3->numAllocated()));
                ASSERT(3 != pool1->numAllocated());
                ASSERT(3 != pool3->numAllocated());
                ASSERT(0 == pool2->numAllocated());

                ASSERT(rrr.resource() != rr.resource());
                ASSERT(rrr.resource() !=  r.resource());
                *r.resource() = 123;
                *rr.resource() = 456;
                *rrr.resource() = 789;
            }
            ASSERT(0 == pool1->numAllocated());
            ASSERT(0 == pool2->numAllocated());
            ASSERT(0 == pool3->numAllocated());

            // Invalidate pool1 and make sure everything is going to be
            // retrieved from pool3.
            pool1->invalidate(0);
            {
                Res r; ASSERT(0 == set.tryGetResource(&r));
                ASSERT(r.isValid());
                ASSERT(r.resource());
                ASSERT(0 == r.poolPriority());
                ASSERT(0 == pool1->numAllocated());
                ASSERT(0 == pool2->numAllocated());
                ASSERT(1 == pool3->numAllocated());

                Res rr; ASSERT(0 == set.tryGetResource(&rr));
                ASSERT(rr.isValid());
                ASSERT(rr.resource());
                ASSERT(0 == rr.poolPriority());
                ASSERT(0 == pool1->numAllocated());
                ASSERT(0 == pool2->numAllocated());
                ASSERT(2 == pool3->numAllocated());

                Res rrr; ASSERT(0 == set.tryGetResource(&rrr));
                ASSERT(rrr.isValid());
                ASSERT(rrr.resource());
                ASSERT(0 == rrr.poolPriority());
                ASSERT(0 == pool1->numAllocated());
                ASSERT(0 == pool2->numAllocated());
                ASSERT(3 == pool3->numAllocated());

                ASSERT(rrr.resource() != rr.resource());
                ASSERT(rrr.resource() !=  r.resource());
                *r.resource() = 123;
                *rr.resource() = 456;
                *rrr.resource() = 789;
            }
            ASSERT(0 == pool1->numAllocated());
            ASSERT(0 == pool2->numAllocated());
            ASSERT(0 == pool3->numAllocated());

            // Make sure everything is good when we revalidate pool1.
            pool1->validate();
            {
                Res r; ASSERT(0 == set.tryGetResource(&r));
                ASSERT(r.isValid());
                ASSERT(r.resource());
                ASSERT(0 == r.poolPriority());
                ASSERT(1 == (pool1->numAllocated() + pool3->numAllocated()));
                ASSERT(0 == pool2->numAllocated());

                Res rr; ASSERT(0 == set.tryGetResource(&rr));
                ASSERT(rr.isValid());
                ASSERT(rr.resource());
                ASSERT(0 == rr.poolPriority());
                ASSERT(2 == (pool1->numAllocated() + pool3->numAllocated()));
                ASSERT(2 != pool1->numAllocated());
                ASSERT(2 != pool3->numAllocated());
                ASSERT(0 == pool2->numAllocated());

                Res rrr; ASSERT(0 == set.tryGetResource(&rrr));
                ASSERT(rrr.isValid());
                ASSERT(rrr.resource());
                ASSERT(0 == rrr.poolPriority());
                ASSERT(3 == (pool1->numAllocated() + pool3->numAllocated()));
                ASSERT(3 != pool1->numAllocated());
                ASSERT(3 != pool3->numAllocated());
                ASSERT(0 == pool2->numAllocated());

                ASSERT(rrr.resource() != rr.resource());
                ASSERT(rrr.resource() !=  r.resource());
                *r.resource() = 123;
                *rr.resource() = 456;
                *rrr.resource() = 789;
            }
            ASSERT(0 == pool1->numAllocated());
            ASSERT(0 == pool2->numAllocated());
            ASSERT(0 == pool3->numAllocated());

            // Invalidate pool1 and pool3 and make sure everything is going to
            // be retrieved from pool2.
            pool1->invalidate(0);
            pool3->invalidate(0);
            {
                Res r; ASSERT(0 == set.tryGetResource(&r));
                ASSERT(r.isValid());
                ASSERT(r.resource());
                ASSERT(1 == r.poolPriority());
                ASSERT(0 == pool1->numAllocated());
                ASSERT(1 == pool2->numAllocated());
                ASSERT(0 == pool3->numAllocated());

                Res rr; ASSERT(0 == set.tryGetResource(&rr));
                ASSERT(rr.isValid());
                ASSERT(rr.resource());
                ASSERT(1 == rr.poolPriority());
                ASSERT(0 == pool1->numAllocated());
                ASSERT(2 == pool2->numAllocated());
                ASSERT(0 == pool3->numAllocated());

                Res rrr; ASSERT(0 == set.tryGetResource(&rrr));
                ASSERT(rrr.isValid());
                ASSERT(rrr.resource());
                ASSERT(1 == rrr.poolPriority());
                ASSERT(0 == pool1->numAllocated());
                ASSERT(3 == pool2->numAllocated());
                ASSERT(0 == pool3->numAllocated());

                ASSERT(rrr.resource() != rr.resource());
                ASSERT(rrr.resource() !=  r.resource());
                *r.resource() = 123;
                *rr.resource() = 456;
                *rrr.resource() = 789;
            }
            ASSERT(0 == pool1->numAllocated());
            ASSERT(0 == pool2->numAllocated());
            ASSERT(0 == pool3->numAllocated());

            pool2->invalidate(0);
            {
                Res r; ASSERT(0 != set.tryGetResource(&r));
                ASSERT(!r.isValid());
                ASSERT(!r.resource());
                ASSERT(-1 == r.poolPriority());
                ASSERT(0 == pool1->numAllocated());
                ASSERT(0 == pool2->numAllocated());
                ASSERT(0 == pool3->numAllocated());

                Res rr; ASSERT(0 != set.tryGetResource(&rr));
                ASSERT(!rr.isValid());
                ASSERT(!rr.resource());
                ASSERT(-1 == rr.poolPriority());
                ASSERT(0 == pool1->numAllocated());
                ASSERT(0 == pool2->numAllocated());
                ASSERT(0 == pool3->numAllocated());
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'tryGetResource'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   tryGetResource(bcecs_FailoverResource<TYPE> *resource)
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'tryGetResource'" << endl
                          << "========================" << endl;

        bcema_TestAllocator ta(veryVeryVerbose);
        {
            bcecs_FailoverSet<int> set(&ta);
            TestResourcePool *pool1 = new (ta) TestResourcePool(&ta);
            ASSERT(pool1);
            bdema_ManagedPtr<bcecs_ResourcePool<int> > pool1M(pool1, &ta);

            ASSERT(pool1->isValid());
            set.registerPool(pool1M, 0);

            typedef bcecs_FailoverResource<int> Res;

            // Test resource copy construction and resource lifetime of
            // copied resources.
            Res *ptr = 0;
            {
                Res r; ASSERT(0 == set.tryGetResource(&r));
                ASSERT(r.isValid());
                ASSERT(r.resource());
                ASSERT(0 == r.poolPriority());
                ASSERT(1 == pool1->numAllocated());
                *r.resource() = 250478;
                ptr = new (ta) Res(r, &ta);
            }
            ASSERT(1 == pool1->numAllocated());
            ASSERT(250478 == *ptr->resource());
            *ptr->resource() = 123;
            ta.deleteObjectRaw(ptr);
            ASSERT(0 == pool1->numAllocated());

            {
                Res r; ASSERT(0 == set.tryGetResource(&r));
                ASSERT(r.isValid());
                ASSERT(r.resource());
                ASSERT(0 == r.poolPriority());
                ASSERT(1 == pool1->numAllocated());
                *r.resource() = 250478;

                r.release();
                ASSERT(!r.isValid());
                ASSERT(!r.resource());
                ASSERT(0 == pool1->numAllocated());
            }

            // Test resource assignment.
            {
                Res r; ASSERT(0 == set.tryGetResource(&r));
                ASSERT(r.isValid());
                ASSERT(r.resource());
                ASSERT(0 == r.poolPriority());
                ASSERT(1 == pool1->numAllocated());

                Res rr; ASSERT(0 == set.tryGetResource(&rr));
                ASSERT(rr.isValid());
                ASSERT(rr.resource());
                ASSERT(0 == rr.poolPriority());
                ASSERT(2 == pool1->numAllocated());

                Res rrr; ASSERT(0 == set.tryGetResource(&rrr));
                ASSERT(rrr.isValid());
                ASSERT(rrr.resource());
                ASSERT(0 == rrr.poolPriority());
                ASSERT(3 == pool1->numAllocated());

                ASSERT(rrr.resource() != rr.resource());
                ASSERT(rrr.resource() !=  r.resource());
                *r.resource() = 123;
                *rr.resource() = 456;
                *rrr.resource() = 789;

                r = r;
                rr = rrr;
                ASSERT(2 == pool1->numAllocated());
                ASSERT(rrr.resource() == rr.resource());
                ASSERT(*rrr.resource() == *rr.resource());
                ASSERT(rrr.resource() !=  r.resource());
                *r.resource() = 123;
                *rr.resource() = 456;
                *rrr.resource() = 789;
            }
            ASSERT(0 == pool1->numAllocated());

            // Invalidation test.
            {
                Res r; ASSERT(0 == set.tryGetResource(&r));
                ASSERT(r.isValid());
                ASSERT(0 == r.poolPriority());
                ASSERT(r.resource());

                Res rr; ASSERT(0 == set.tryGetResource(&rr));
                ASSERT(rr.isValid());
                ASSERT(rr.resource());
                ASSERT(0 == rr.poolPriority());

                rr.markAsInvalid();
                ASSERT(!pool1->isValid());
                ASSERT(2 == pool1->numAllocated());
            }
            ASSERT(0 == pool1->numAllocated());

            // Make sure we cannot get any resources.
            {
                Res r; ASSERT(0 != set.tryGetResource(&r));
                ASSERT(!r.isValid());
                ASSERT(!r.resource());
                ASSERT(-1 == r.poolPriority());
                ASSERT(0 == pool1->numAllocated());

                Res rr; ASSERT(0 != set.tryGetResource(&rr));
                ASSERT(!rr.isValid());
                ASSERT(!rr.resource());
                ASSERT(-1 == rr.poolPriority());
                ASSERT(0 == pool1->numAllocated());
            }

            // Let's make sure that everything is all good after re-validation.
            pool1->validate();
            ASSERT(0 == pool1->numAllocated());
            {
                Res r; ASSERT(0 == set.tryGetResource(&r));
                ASSERT(r.isValid());
                ASSERT(r.resource());

                Res rr; ASSERT(0 == set.tryGetResource(&rr));
                ASSERT(rr.isValid());
                ASSERT(rr.resource());

                Res rrr; ASSERT(0 == set.tryGetResource(&rrr));
                ASSERT(rrr.isValid());
                ASSERT(rrr.resource());
                ASSERT(3 == pool1->numAllocated());

                ASSERT(rrr.resource() != rr.resource());
                ASSERT(rrr.resource() !=  r.resource());
                *r.resource() = 123;
                *rr.resource() = 456;
                *rrr.resource() = 789;
            }
        }
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

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
        //
        // Testing:
        //   This Test Case exercises basic functionality.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bcema_TestAllocator testAllocator(veryVeryVerbose);
        BEGIN_BSLMA_EXCEPTION_TEST
        {
            TestResourcePool testPool(&testAllocator);

            // Simple breathing test for TestResourcePool.
            typedef bdema_ManagedPtr<int> MPtr;
            TestResourcePool::Handle handle;
            {
                MPtr t;
                ASSERT(0 == testPool.tryGetObject(&t, &handle));
                ASSERT(0 != t.ptr());
            }
            {
                MPtr t;
                ASSERT(0 == testPool.tryGetObject(&t, &handle));
                ASSERT(t);
                ASSERT(1 == testPool.numAllocated());
                MPtr tt;
                ASSERT(0 == testPool.tryGetObject(&tt, &handle));
                ASSERT(tt);
                ASSERT(2 == testPool.numAllocated());
                MPtr ttt;
                ASSERT(0 == testPool.tryGetObject(&ttt, &handle));
                ASSERT(ttt);
                ASSERT(3 == testPool.numAllocated());
            }
            {
                MPtr t;
                ASSERT(0 == testPool.tryGetObject(&t, &handle));
                ASSERT(t);
                ASSERT(true == testPool.isValid());
                ASSERT(1 == testPool.numAllocated());

                testPool.invalidate(0);
                ASSERT(false == testPool.isValid());
                MPtr tt;
                ASSERT(0 != testPool.tryGetObject(&tt, &handle));
                ASSERT(!tt);
                ASSERT(1 == testPool.numAllocated());

                testPool.validate();
                ASSERT(true == testPool.isValid());
                MPtr ttt;
                ASSERT(0 == testPool.tryGetObject(&ttt, &handle));
                ASSERT(ttt);
                ASSERT(2 == testPool.numAllocated());
            }
        } END_BSLMA_EXCEPTION_TEST

        BEGIN_BSLMA_EXCEPTION_TEST
        {
            bcecs_FailoverSet<int> set(&testAllocator);
            TestResourcePool *pool1 =
                          new (testAllocator) TestResourcePool(&testAllocator);
            ASSERT(pool1);
            bdema_ManagedPtr<bcecs_ResourcePool<int> >
                                                 pool1M(pool1, &testAllocator);

            ASSERT(pool1->isValid());
            set.registerPool(pool1M, 0);

            typedef bcecs_FailoverResource<int> Res;

            // Test simple allocation.
            {
                Res r; ASSERT(!r.isValid()); ASSERT(!r.resource());
                ASSERT(0 == set.tryGetResource(&r));
                ASSERT(r.isValid());
                ASSERT(r.resource());
                ASSERT(0 == r.poolPriority());
                *r.resource() = 123;
                ASSERT(1 == pool1->numAllocated());
            }
            ASSERT(0 == pool1->numAllocated());

            // Test multiple allocations.
            {
                Res r; ASSERT(0 == set.tryGetResource(&r));
                ASSERT(r.isValid());
                ASSERT(r.resource());
                ASSERT(0 == r.poolPriority());
                ASSERT(1 == pool1->numAllocated());

                Res rr; ASSERT(0 == set.tryGetResource(&rr));
                ASSERT(rr.isValid());
                ASSERT(rr.resource());
                ASSERT(0 == rr.poolPriority());
                ASSERT(2 == pool1->numAllocated());

                Res rrr; ASSERT(0 == set.tryGetResource(&rrr));
                ASSERT(rrr.isValid());
                ASSERT(rrr.resource());
                ASSERT(0 == rrr.poolPriority());
                ASSERT(3 == pool1->numAllocated());

                ASSERT(rrr.resource() != rr.resource());
                ASSERT(rrr.resource() !=  r.resource());
                *r.resource() = 123;
                *rr.resource() = 456;
                *rrr.resource() = 789;
            }

            // Test max allocations.
            {
                pool1->setMaxResources(2);
                Res r; ASSERT(0 == set.tryGetResource(&r));
                ASSERT(r.isValid());
                ASSERT(r.resource());
                ASSERT(0 == r.poolPriority());
                ASSERT(1 == pool1->numAllocated());

                Res rr; ASSERT(0 == set.tryGetResource(&rr));
                ASSERT(rr.isValid());
                ASSERT(rr.resource());
                ASSERT(0 == rr.poolPriority());
                ASSERT(2 == pool1->numAllocated());

                Res rrr; ASSERT(0 != set.tryGetResource(&rrr));
                ASSERT(!rrr.isValid());
                ASSERT(!rrr.resource());
                ASSERT(-1 == rrr.poolPriority());
                ASSERT(2 == pool1->numAllocated());
            }
            ASSERT(0 == pool1->numAllocated());
        }
        END_BSLMA_EXCEPTION_TEST
        ASSERT(0 <  testAllocator.numAllocations());
        ASSERT(0 == testAllocator.numBytesInUse());
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
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
