// bcec_sharedobjectpool.t.cpp      -*-C++-*-

#include <bcec_sharedobjectpool.h>

#include <bcema_testallocator.h>
#include <bcema_blob.h>
#include <bcema_poolallocator.h>
#include <bcemt_threadgroup.h>
#include <bcec_objectpool.h>
#include <bdef_bind.h>
#include <bdef_placeholder.h>
#include <bsls_stopwatch.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bcema_pooledblobbufferfactory.h>

#include <bsl_numeric.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

static int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

namespace {
   //Unnamed namespace scopes private classes and methods for testing

bces_SpinLock coutLock;

template <class POOL>
class TestRun
{
   bslma_Allocator *d_allocator_p; // held not owned
   POOL     d_pool;   //held
   vector<double> d_partialRates; //note: count / (time * d_numThread)
   int      d_numThreads;
   int      d_numBlobsPerIteration;
   int      d_secondsToRun;
   bool     d_run;  //has test been run yet

   // Results:
   double   d_ratePerThread;  //blobs acq/rel per sec per thread

   void threadProc(int id);
public:

   TestRun(int numThreads, int numBlobsPerIteration,
           int secondsToRun, bslma_Allocator *basicAllocator = 0)
      : d_allocator_p(bslma_Default::allocator(basicAllocator)),
        d_pool(bslma_Default::allocator(basicAllocator)),
        d_partialRates(bslma_Default::allocator(basicAllocator)),
        d_numThreads(numThreads),
        d_numBlobsPerIteration(numBlobsPerIteration),
        d_secondsToRun(secondsToRun),
        d_run(false)
   {
      d_partialRates.resize(d_numThreads, 0.0);
   }

   void run();

   double getRatePerThread()
   {
      if (d_run) {
         return d_ratePerThread;
      }
      else {
         return -1;
      }
   }
};

template <class POOL>
void TestRun<POOL>::threadProc(int id)
{
   bsls_Stopwatch timer;
   timer.start();

   for (int count = 0; true; ++count) {
      double elapsed = timer.elapsedTime();
      if (elapsed < d_secondsToRun) {
         vector<bcema_SharedPtr<bcema_Blob> > blobs(d_allocator_p);
         blobs.resize(d_numBlobsPerIteration);
         for (int i = 0; i < d_numBlobsPerIteration; ++i) {
            d_pool.getBlob(&blobs[i]);
         }
      }
      else {
         d_partialRates[id] = (double)(count * d_numBlobsPerIteration) /
            (elapsed * d_numThreads);
         if (veryVerbose) {
            coutLock.lock();
            cout << "INFO: thread " << id << " count = " << count << endl;
            coutLock.unlock();
         }
         break;
      }
   }
}

template <class POOL>
void TestRun<POOL>::run()
{
   bcemt_ThreadGroup tg;

   for (int i = 0; i < d_numThreads; ++i) {
      tg.addThread(bdef_BindUtil::bind(&TestRun::threadProc,
                                       this, i));
   }
   tg.joinAll();

   d_ratePerThread = accumulate(d_partialRates.begin(),
                                d_partialRates.end(), 0.0) / d_numThreads;
   d_run = true;
}

class SlowerBlobPool {
    bcema_PooledBlobBufferFactory d_blobFactory;  // supply blob buffers
    bcec_ObjectPool<bcema_Blob>   d_blobPool;     // supply blobs
    bslma_Allocator              *d_allocator_p;  // allocator (held)

    enum {BUFFER_SIZE=65536};

    static void createBlob(void* address, bcema_BlobBufferFactory *factory,
                    bslma_Allocator *allocator) {
        new (address) bcema_Blob(factory, allocator);
    }

  public:

    SlowerBlobPool(bslma_Allocator *basicAllocator = 0)
      : d_blobFactory(BUFFER_SIZE, bslma_Default::allocator(basicAllocator))
      , d_blobPool(bdef_BindUtil::bind(
                                    &SlowerBlobPool::createBlob,
                                    bdef_PlaceHolders::_1,
                                    &d_blobFactory,
                                    bslma_Default::allocator(basicAllocator)),
                   -1, basicAllocator)
      , d_allocator_p(bslma_Default::allocator(basicAllocator))

   {}

   void getBlob(bcema_SharedPtr<bcema_Blob> *blob_sp) {
       blob_sp->load(d_blobPool.getObject(),
                     bdef_MemFnUtil::memFn(
                              &bcec_ObjectPool<bcema_Blob>::releaseObject,
                              &d_blobPool),
                     d_allocator_p);
   }
};

class SlowBlobPool {
    bcema_PoolAllocator           d_spAllocator;  // allocate shared pointer
    bcema_PooledBlobBufferFactory d_blobFactory;  // supply blob buffers
    bcec_ObjectPool<bcema_Blob>   d_blobPool;     // supply blobs

    enum {BUFFER_SIZE=65536};

    static void createBlob(void* address, bcema_BlobBufferFactory *factory,
                    bslma_Allocator *allocator) {
        new (address) bcema_Blob(factory, allocator);
    }


    static void resetAndReturnBlob(bcema_Blob *blob,
                                   bcec_ObjectPool<bcema_Blob> *pool) {
        blob->removeAll();
        pool->releaseObject(blob);
    }

  public:

    SlowBlobPool(bslma_Allocator *basicAllocator = 0)
      : d_spAllocator(basicAllocator)
      , d_blobFactory(BUFFER_SIZE, basicAllocator)
      , d_blobPool(bdef_BindUtil::bind(
                                    &SlowBlobPool::createBlob,
                                    bdef_PlaceHolders::_1,
                                    &d_blobFactory,
                                    basicAllocator), -1,
                   basicAllocator)
   {}

   void getBlob(bcema_SharedPtr<bcema_Blob> *blob_sp) {
       blob_sp->load(d_blobPool.getObject(),
                     bdef_BindUtil::bind(
                              &SlowBlobPool::resetAndReturnBlob,
                              bdef_PlaceHolders::_1,
                              &d_blobPool),
                     &d_spAllocator);
   }
};

class FastBlobPool {
    typedef bcec_SharedObjectPool<
                     bcema_Blob,
                     bcec_ObjectPoolFunctors::DefaultCreator,
                     bcec_ObjectPoolFunctors::RemoveAll<bcema_Blob> > BlobPool;

    bcema_PooledBlobBufferFactory d_blobFactory;  // supply blob buffers
    BlobPool                      d_blobPool;     // supply blobs

    enum {BUFFER_SIZE=65536};

    static void createBlob(void* address, bcema_BlobBufferFactory *factory,
                    bslma_Allocator *allocator) {
        new (address) bcema_Blob(factory, allocator);
    }

  public:

   FastBlobPool(bslma_Allocator *basicAllocator = 0)
      : d_blobFactory(BUFFER_SIZE, basicAllocator)
      , d_blobPool(bdef_BindUtil::bind(
                                    &FastBlobPool::createBlob,
                                    bdef_PlaceHolders::_1,
                                    &d_blobFactory,
                                    bdef_PlaceHolders::_2),
                   -1, basicAllocator)
   {}

   void getBlob(bcema_SharedPtr<bcema_Blob> *blob_sp) {
       *blob_sp = d_blobPool.getObject();
   }
};

struct SpLink
{
    bcema_SharedPtr<SpLink> d_next;

    void reset()
    {
        ASSERT(false == d_next);
    }
};


template <class POOL>
class LinkTestRun
{
   bslma_Allocator *d_allocator_p; // held not owned
   POOL     d_pool;   //held
   vector<double> d_partialRates; //note: count / (time * d_numThread)
   int      d_numThreads;
   int      d_numLinksPerIteration;
   int      d_secondsToRun;
   bool     d_run;  //has test been run yet

   // Results:
   double   d_ratePerThread;  //links acq/rel per sec per thread

   void threadProc(int id);
public:

   LinkTestRun(int numThreads, int numLinksPerIteration,
           int secondsToRun, bslma_Allocator *basicAllocator = 0)
      : d_allocator_p(bslma_Default::allocator(basicAllocator)),
        d_pool(bslma_Default::allocator(basicAllocator)),
        d_partialRates(bslma_Default::allocator(basicAllocator)),
        d_numThreads(numThreads),
        d_numLinksPerIteration(numLinksPerIteration),
        d_secondsToRun(secondsToRun),
        d_run(false)
   {
      d_partialRates.resize(d_numThreads, 0.0);
   }

   void run();

   double getRatePerThread()
   {
      if (d_run) {
         return d_ratePerThread;
      }
      else {
         return -1;
      }
   }
};

template <class POOL>
void LinkTestRun<POOL>::threadProc(int id)
{
   bsls_Stopwatch timer;
   timer.start();

   for (int count = 0; true; ++count) {
      double elapsed = timer.elapsedTime();
      if (elapsed < d_secondsToRun) {
         bcema_SharedPtr<SpLink> head, node;
         d_pool.getLink(&head);
         node = head;
         for (int i = 0; i < d_numLinksPerIteration-1; ++i) {
            bcema_SharedPtr<SpLink> thisNode;
            d_pool.getLink(&thisNode);
            node->d_next = thisNode;
            node = thisNode;
         }
         for (node = head; node; ) {
            bcema_SharedPtr<SpLink> next = node->d_next;
            node->d_next.clear();
            node = next;
         }

      }
      else {
         d_partialRates[id] = (double)(count * d_numLinksPerIteration) /
            (elapsed * d_numThreads);
         if (veryVerbose) {
            coutLock.lock();
            cout << "INFO: thread " << id << " count = " << count << endl;
            coutLock.unlock();
         }
         break;
      }
   }
}

template <class POOL>
void LinkTestRun<POOL>::run()
{
   bcemt_ThreadGroup tg;

   for (int i = 0; i < d_numThreads; ++i) {
      tg.addThread(bdef_BindUtil::bind(&LinkTestRun::threadProc,
                                       this, i));
   }
   tg.joinAll();

   d_ratePerThread = accumulate(d_partialRates.begin(),
                                d_partialRates.end(), 0.0) / d_numThreads;
   d_run = true;
}


} // close unnamed namespace

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
//-----------------------------------------------------------------------------
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
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

static bcemt_Mutex assertMutex; // mutex to protect assert macros

#define LOOP_ASSERTT(I,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\n"; \
       aSsErT(1, #X, __LINE__); } assertMutex.unlock(); }

#define LOOP2_ASSERTT(I,J,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << "\n"; aSsErT(1, #X, __LINE__); } \
       assertMutex.unlock(); }

#define LOOP3_ASSERTT(I,J,K,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << "\t" << #K << ": " << K << "\n"; \
       aSsErT(1, #X, __LINE__); } assertMutex.unlock(); }

#define LOOP4_ASSERTT(I,J,K,L,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << "\t" << #K << ": " << K << "\t" << #L \
       << ": " << L << "\n"; aSsErT(1, #X, __LINE__); } assertMutex.unlock(); }

#define LOOP5_ASSERTT(I,J,K,L,M,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << "\t" << #K << ": " << K << "\t" << #L \
       << ": " << L << "\t" << #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } assertMutex.unlock(); }

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

    void reset() {
        ++d_resetCount;
    }
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

//=============================================================================
//          GLOBAL TYPEDEFS/CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------

class StringCreator
{
  public:
    void operator() (void *mem, bslma_Allocator* allocator) {
        new (mem) bsl::string(allocator);
    }
};

class StringReseter
{
    bool d_reset;

  public:
    explicit StringReseter(bool r=true) {
        d_reset = r;
    }

    void operator() (bsl::string *s) {
        if (d_reset) s->clear();
    }
};

class SlowLinkPool {
   bcema_PoolAllocator     d_spAllocator;  // allocate shared pointer
   bcec_ObjectPool<SpLink>   d_linkPool;

   static void createLink(void* address) {
      new (address) SpLink;
   }

public:

   SlowLinkPool(bslma_Allocator *basicAllocator = 0)
      : d_spAllocator(bslma_Default::allocator(basicAllocator))
      , d_linkPool(bdef_BindUtil::bind(&createLink,
                                       bdef_PlaceHolders::_1), -1,
                   bslma_Default::allocator(basicAllocator))
   {}

   void getLink(bcema_SharedPtr<SpLink> *link_sp) {
      link_sp->load(d_linkPool.getObject(),
                    bdef_MemFnUtil::memFn(
                              &bcec_ObjectPool<SpLink>::releaseObject,
                              &d_linkPool),
                     &d_spAllocator);
   }
};

class FastLinkPool {
    typedef bcec_SharedObjectPool<SpLink,
                                  bcec_ObjectPoolFunctors::DefaultCreator,
                                  bcec_ObjectPoolFunctors::Reset<SpLink> >
                                                                    LinkPool;

    LinkPool                      d_linkPool;

  public:

    FastLinkPool(bslma_Allocator *basicAllocator = 0)
      : d_linkPool(-1, basicAllocator)
   {}

   void getLink(bcema_SharedPtr<SpLink> *link_sp) {
       *link_sp = d_linkPool.getObject();
   }
};

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

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
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

         if (veryVerbose) {
            cout <<
          "Constructor form test:\n"
          "\tbcec_SharedObjectPool(int                     numObjects = -1,\n"
          "\t                      bslma_Allocator        *basicAllocator = 0)"
                 << endl;
         }
         bcema_TestAllocator ta;

         bcec_SharedObjectPool<ConstructorTestHelp1a,
                   bcec_ObjectPoolFunctors::DefaultCreator,
                   bcec_ObjectPoolFunctors::Reset<ConstructorTestHelp1a> >
                                                              pool1a(1, &ta);
         ConstructorTestHelp1a* ptr1a;
         {
            bcema_SharedPtr<ConstructorTestHelp1a> sp1a = pool1a.getObject();
            ptr1a = sp1a.ptr();
         }

         ASSERT(&ta == ptr1a->d_allocator_p);
         ASSERT(1 == ptr1a->d_resetCount);

         bcec_SharedObjectPool<
             ConstructorTestHelp1a,
             ConstructorTestHelp1aCreator,
             bcec_ObjectPoolFunctors::Reset<ConstructorTestHelp1a> >
                   pool1a2(1, &ta);
         {
            bcema_SharedPtr<ConstructorTestHelp1a> sp1a = pool1a2.getObject();
            ptr1a = sp1a.ptr();
         }

         ASSERT(&ta == ptr1a->d_allocator_p);
         LOOP_ASSERT(ptr1a->d_resetCount,
                     -1000 == ptr1a->d_resetCount);

         bcec_SharedObjectPool<
             ConstructorTestHelp1a,
             ConstructorTestHelp1aCreator2,
             bcec_ObjectPoolFunctors::Reset<ConstructorTestHelp1a> >
                 pool1a3(1, &ta);
         {
            bcema_SharedPtr<ConstructorTestHelp1a> sp1a = pool1a3.getObject();
            ptr1a = sp1a.ptr();
         }

         ASSERT(0 == ptr1a->d_allocator_p);
         LOOP_ASSERT(ptr1a->d_resetCount,
                     -2000 == ptr1a->d_resetCount);

         bcec_SharedObjectPool<ConstructorTestHelp1b,
             bcec_ObjectPoolFunctors::DefaultCreator,
             bcec_ObjectPoolFunctors::Reset<ConstructorTestHelp1b> >
                  pool1b(1, &ta);
         ConstructorTestHelp1b* ptr1b;
         {
            bcema_SharedPtr<ConstructorTestHelp1b> sp1b = pool1b.getObject();
            ptr1b = sp1b.ptr();
         }
         ASSERT(1 == ptr1b->d_resetCount);

         if (veryVerbose) {
            cout <<
          "Constructor form test:\n"
          "\tbcec_SharedObjectPool(const CREATOR&         objectCreator,\n"
          "\t                      int                     numObjects = -1,\n"
          "\t                      bslma_Allocator        *basicAllocator = 0)"
                 << endl;
         }

         bcec_SharedObjectPool<ConstructorTestHelp3,
                               ConstructorTestHelp3Creator,
             bcec_ObjectPoolFunctors::Reset<ConstructorTestHelp3> > pool2(
                     ConstructorTestHelp3Creator(400),
                     1, &ta);

         ConstructorTestHelp3* ptr2;
         {
            bcema_SharedPtr<ConstructorTestHelp3> sp2 = pool2.getObject();
            ptr2 = sp2.ptr();
         }

         ASSERT(&ta == ptr2->d_allocator_p);
         ASSERT(1 == ptr2->d_resetCount);
         ASSERT(400 == ptr2->d_startCount);

         if (veryVerbose) {
            cout <<
          "Constructor form test:\n"
          "\tbcec_SharedObjectPool(const CREATOR&          objectCreator,\n"
          "\t                      const RESETTER&         objectResetter,\n"
          "\t                      int                     numObjects = -1,\n"
          "\t                      bslma_Allocator        *basicAllocator = 0)"
                 << endl;
         }

         bcec_SharedObjectPool<ConstructorTestHelp3,
            ConstructorTestHelp3Creator,
            bdef_Function<void(*)(ConstructorTestHelp3*)> >
               pool3(ConstructorTestHelp3Creator(400),
                     bdef_BindUtil::bind(
                              &ConstructorTestHelp3::resetWithCount, _1, 300),
                     1, &ta);

         ConstructorTestHelp3* ptr3;
         {
            bcema_SharedPtr<ConstructorTestHelp3> sp3 = pool3.getObject();
            ptr3 = sp3.ptr();
         }

         ASSERT(&ta == ptr3->d_allocator_p);
         ASSERT(300 == ptr3->d_resetCount);
         ASSERT(400 == ptr3->d_startCount);

         if (veryVerbose) {
            cout <<
 "Constructor form test: (IMPLICIT) \n"
 "\t"
 "bcec_SharedObjectPool(\n"
 "\t"
 "                    const bdef_Function<void(*)(void *)>& objectCreator\n"
 "\t"
 "                    const RESETTER&                       objectResetter,\n"
 "\t"
 "                    int                                   numObjects = -1,\n"
 "\t"
 "                    bslma_Allocator                      *basicAllocator=0)"
                 << endl;
         }

         typedef bcec_SharedObjectPool<ConstructorTestHelp3,
             bcec_ObjectPoolFunctors::DefaultCreator,
            bdef_Function<void(*)(ConstructorTestHelp3*)> > Pool4;
         Pool4 pool4
            (bdef_BindUtil::bind(&constructor4, 600, _1),
             bdef_BindUtil::bind(&ConstructorTestHelp3::resetWithCount,
                                 _1, 500),
             1, &ta);

         ConstructorTestHelp3* ptr4;
         {
            bcema_SharedPtr<ConstructorTestHelp3> sp4 = pool4.getObject();
            ptr4 = sp4.ptr();
         }

         ASSERT(0 == ptr4->d_allocator_p);
         ASSERT(500 == ptr4->d_resetCount);
         ASSERT(600 == ptr4->d_startCount);

      } break;
      case 7: {
           //////////////////////////////////////////////////////
           // Thread safety test
           //
           // Run the performance test under high contention using a
           // test allocator.
           //////////////////////////////////////////////////////
         if (verbose) {
            cout << "Thread safety test" << endl;
         }

         enum {
            NUM_THREADS=20,
            NUM_BLOBS_PER_ITER=10000,
            SECONDS_TO_RUN=6
         };

         bcema_TestAllocator alloc;
         {
            TestRun<FastBlobPool> test(NUM_THREADS, NUM_BLOBS_PER_ITER,
                                       SECONDS_TO_RUN, &alloc);
            test.run();

            //sanity check, *not* performance check:
            LOOP_ASSERT(test.getRatePerThread(), 1 < test.getRatePerThread());
         }
         ASSERT(0 < alloc.numAllocations());
         ASSERT(0 == alloc.numBytesInUse());
      } break;

      case 6: {
         //////////////////////////////////////////////////////
         // Usage Example Test
         //
         // Concerns: that the component's usage example compiles and
         // runs properly.
         //////////////////////////////////////////////////////
         if (verbose) {
            cout << "Usage example test" << endl;
         }

         bslma_TestAllocator slowAllocator, fastAllocator;
         {
           SlowBlobPool slowPool(&slowAllocator);
           FastBlobPool fastPool(&fastAllocator);

           bcema_SharedPtr<bcema_Blob> blob_sp;

           fastPool.getBlob(&blob_sp);
           slowPool.getBlob(&blob_sp);
         }

         LOOP_ASSERT(slowAllocator.numAllocations(),
                     2 == slowAllocator.numAllocations());
         LOOP_ASSERT(fastAllocator.numAllocations(),
                     1 == fastAllocator.numAllocations());
         ASSERT(0 == slowAllocator.numBytesInUse());
         ASSERT(0 == fastAllocator.numBytesInUse());
      } break;

      case 5: { // resetter test 2
        if (verbose) {
           cout << "Resetter test 2" << endl;
        }

        typedef
        bcec_SharedObjectPool<bsl::string, StringCreator, StringReseter> Pool;

        Pool pool1(StringCreator(),StringReseter(),1);
        bcema_SharedPtr<bsl::string> sharedStr;

        {
            sharedStr = pool1.getObject();
            *sharedStr = "abcdef";
            ASSERT(pool1.numObjects()==1);
            ASSERT(*sharedStr=="abcdef");
            sharedStr.reset();
        }
        {
            sharedStr = pool1.getObject();
            ASSERT(pool1.numObjects()==1);
            ASSERT(*sharedStr=="");
        }
      } break;

      case 4: { // resetter test 1
        if (verbose) {
           cout << "Resetter test 1" << endl;
        }
        typedef
        bcec_SharedObjectPool<bsl::string, StringCreator, StringReseter> Pool;

        Pool pool1(StringCreator(),StringReseter(false),1);
        bcema_SharedPtr<bsl::string> sharedStr;

        {
            sharedStr = pool1.getObject();
            *sharedStr = "abcdef";
            ASSERT(pool1.numObjects()==1);
            ASSERT(*sharedStr=="abcdef");
            sharedStr.reset();
        }
        {
            sharedStr = pool1.getObject();
            ASSERT(pool1.numObjects()==1);
            ASSERT(*sharedStr=="abcdef");
        }
      } break;

      case 3: { // numObjects test
        if (verbose) {
           cout << "numObjects test" << endl;
        }

        typedef
        bcec_SharedObjectPool<bsl::string, StringCreator, StringReseter> Pool;

        Pool pool(StringCreator(),StringReseter(),20);
        ASSERT(pool.numObjects()==0);
        bcema_SharedPtr<bsl::string> sharedStr = pool.getObject();
        ASSERT(pool.numObjects()==20);
        ASSERT(pool.numAvailableObjects()==19);
      } break;

      case 2: { // allocator test
        if (verbose) {
           cout << "Allocator test" << endl;
        }

        typedef
        bcec_SharedObjectPool<bsl::string, StringCreator, StringReseter> Pool;


        bslma_TestAllocator ta1(veryVeryVerbose);
        bslma_TestAllocator ta2(veryVeryVerbose);

        {
            bslma_DefaultAllocatorGuard allocGuard(&ta1);
            Pool pool(StringCreator(),StringReseter(),-1, &ta2);
            pool.reserveCapacity(10);
            ASSERT(ta1.numBytesInUse()==0);
            bcema_SharedPtr<bsl::string> sharedStr = pool.getObject();
            ASSERT(ta1.numBytesInUse()==0);
            *sharedStr = "abcdefg";
            ASSERT(ta1.numBytesInUse()==0);
        }
      } break;

      case 1: { // breathing test
        if (verbose) {
           cout << "Breathing test" << endl;
        }

        typedef
        bcec_SharedObjectPool<bsl::string, StringCreator, StringReseter> Pool;

        Pool pool((StringCreator()),(StringReseter()));
        ASSERT(pool.numObjects()==0);
        pool.reserveCapacity(10);
        ASSERT(pool.numObjects()==10);
        bcema_SharedPtr<bsl::string> sharedStr = pool.getObject();
        ASSERT(pool.numObjects()==10);
      } break;

      case -1: {
         ///////////////////////////////////////////////////
         // Cache-stressing performance test
         //
         // Similar to the basic performance test, except that
         // the pooled objects are links in a long list.  Following
         // the list from beginning to end seeks to highlight any
         // caching inefficiencies.
         ////////////////////////////////////////////////////
         cout << "Cache-stressing Performance Test" << endl;

         enum { SECONDS_TO_RUN = 5 };

         struct Parameters {
            int d_numThreads;
            int d_numLinksPerIteration;
            int d_scalingFactor;
         } parameters[] = {
            {1, 1,          725000},
            {1, 500000,     504000},
            {4, 5,          20000},
            {4, 120000,     21000},
            {10, 5,         2700},
            {10, 120000,    2800},
         };

         const int numParameters = sizeof(parameters) / sizeof(Parameters);

         double slowScore = 0, fastScore = 0;
         for (int i = 0; i < numParameters; ++i) {
            const Parameters& p = parameters[i];
            if (verbose) {
               cout << "==========================\n"
                    << "Test step: \n" << endl;
               P(p.d_numThreads);
               P(p.d_numLinksPerIteration);
            }

            LinkTestRun<FastLinkPool> fastTest(p.d_numThreads,
                                               p.d_numLinksPerIteration,
                                               SECONDS_TO_RUN);

            fastTest.run();
            double fastResult = fastTest.getRatePerThread() /
               p.d_scalingFactor;
            if (verbose) {
               P(fastResult);
            }

            LinkTestRun<SlowLinkPool> slowTest(p.d_numThreads,
                                               p.d_numLinksPerIteration,
                                               SECONDS_TO_RUN);

            slowTest.run();
            double slowResult = slowTest.getRatePerThread() /
               p.d_scalingFactor;
            if (verbose) {
               P(slowResult);
            }
            slowScore += slowResult;
            fastScore += fastResult;
         }
         cout
            << "\nOVERALL SCORES:"
            << "\nOriginal link pool: " << slowScore / numParameters
            << "\nSharedObjectPool: " << fastScore / numParameters
            << endl;
      } break;

      case -2: {
         ///////////////////////////////////////////////////
         // Basic Performance Test
         cout << "Basic Performance Test" << endl;

         bool runBaseTest = true;
         if (veryVeryVerbose) {
            cout << "WARNING: 3 command-line args detected.  Suppressing "
                 << "baseline \"slow\" test." << endl;
            runBaseTest = false;
         }

         enum { SECONDS_TO_RUN = 5 };

         struct Parameters {
            int d_numThreads;
            int d_numBlobsPerIteration;
            int d_scalingFactor;
         } parameters[] = {
            {1, 1, 250000},
            {1, 25000, 358000},
            {4, 5, 24000},
            {4, 10000, 21000},
            {10, 5, 2700},
            {10, 10000, 2400},
         };

         const int numParameters = sizeof(parameters) / sizeof(Parameters);

         double slowScore = 0, fastScore = 0, slowerScore = 0;
         for (int i = 0; i < numParameters; ++i) {
            const Parameters& p = parameters[i];
            if (verbose) {
               cout << "==========================\n"
                    << "Test step: \n" << endl;
               P(p.d_numThreads);
               P(p.d_numBlobsPerIteration);
            }

            TestRun<FastBlobPool> fastTest(p.d_numThreads,
                                           p.d_numBlobsPerIteration,
                                           SECONDS_TO_RUN);

            fastTest.run();
            double fastResult = fastTest.getRatePerThread() /
               p.d_scalingFactor;
            if (verbose) {
               P(fastResult);
            }

            if (runBaseTest) {
               TestRun<SlowBlobPool> slowTest(p.d_numThreads,
                                              p.d_numBlobsPerIteration,
                                              SECONDS_TO_RUN);

               slowTest.run();
               double slowResult = slowTest.getRatePerThread() /
                  p.d_scalingFactor;
               if (verbose) {
                  P(slowResult);
               }
               slowScore += slowResult;

               TestRun<SlowerBlobPool> slowerTest(p.d_numThreads,
                                                  p.d_numBlobsPerIteration,
                                                  SECONDS_TO_RUN);

               slowerTest.run();
               double slowerResult = slowerTest.getRatePerThread() /
                  p.d_scalingFactor;
               if (verbose) {
                  P(slowerResult);
               }
               slowerScore += slowerResult;
            }

            fastScore += fastResult;
         }
         if (runBaseTest) {
            cout
               << "\nOVERALL SCORES:"
               << "\nOriginal blob pool: " << slowScore / numParameters
               << "\nNon-Sp-Pooling blob pool: " << slowerScore / numParameters
               << "\nSharedObjectPool: " << fastScore / numParameters
               << endl;
         }
         else {
            cout << "Overall score: " << fastScore << endl;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
