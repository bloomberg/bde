// bcema_poolallocator.t.cpp       -*-C++-*-

#include <bcema_poolallocator.h>

#include <bcema_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bsls_alignmentutil.h>

#include <bsl_cstring.h>  // strcmp()
#include <bsl_cstdlib.h>  // atoi()
#include <bsl_iostream.h>
#include <bsl_list.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
//-----------------------------------------------------------------------------
// CREATORS
// [ 4] bcema_PoolAllocator(size_type size, bslma_Allocator *allocator = 0);
// [ 4] bcema_PoolAllocator(bslma_Allocator *allocator = 0);
// [ 4] ~bcema_PoolAllocator();
//
// MANIPULATORS
// [ 4] void *allocate(size_type size);
// [ 4] void deallocate(void *address);
//
// ACCESSORS
// [ 4] int blockSize() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] CONCERN: 0-size allocation/deallocating 0 pointer
// [ 3] CONCERN: thread-safety of the first allocation
// [ 5] Usage example 1
// [ 6] Usage example 2

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100)  ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\t" \
                    << #M << ": " << M << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number.
#define T_ cout << '\t' << flush;             // Print tab w/o newline.

// The following macros facilitate thread-safe streaming to standard output.

#define MTCOUT \
coutMutex.lock(); \
{ bsl::cout << bcemt_ThreadUtil::selfIdAsInt() << ": "

#define MTCOUT_UNLOCK \
} \
coutMutex.unlock()

#define MTENDL   bsl::endl;  MTCOUT_UNLOCK
#define MTFLUSH  bsl::flush; MTCOUT_UNLOCK

//=============================================================================
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

static bcemt_Mutex coutMutex;

typedef bcema_PoolAllocator Obj;

//=============================================================================
//                        HELPERS FOR TEST CASE 4
//-----------------------------------------------------------------------------
// The following structure and function is copied from the implementation
// directly.

union Header {
    // Leading header on each allocated memory block.  If the memory block
    // was allocated from the pool, the 'd_magicNumber' will be set to
    // 'MAGIC_NUMBER'.  Otherwise, memory was allocated by the external
    // allocator supplied during construction, and 'd_magicNumber' will be
    // zero.

    int                                d_magicNumber;  // allocation source
                                                       // and sanity check

    bsls_AlignmentUtil::MaxAlignedType d_dummy;        // dummy padding
};

static inline
int calculateMaxAlignedSize(int totalSize)
{
    using namespace BloombergLP;

    const int objectAlignmentMin1 = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1;
    const int ret = (totalSize + objectAlignmentMin1) & ~(objectAlignmentMin1);
    BSLS_ASSERT(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT ==
                          bsls_AlignmentUtil::calculateAlignmentFromSize(ret));
    return ret;
}

//=============================================================================
//              HELPERS FOR TESTING FIRST ALLOCATION (case 3)
//-----------------------------------------------------------------------------
static const int NUM_ALLOCATORS = 200;

extern "C" void *my3_up(void *arg)
{
    Obj *arr = (Obj *)arg;

    for (int i = 0; i < NUM_ALLOCATORS; ++i) {
        arr[i].allocate(13);
    }

    return 0;
}

extern "C" void *my3_down(void *arg)
{
    Obj *arr = (Obj *)arg;

    for (int i = NUM_ALLOCATORS-1; i >= 0; --i) {
        arr[i].allocate(13);
    }

    return 0;
}
//=============================================================================
//              HELPERS FOR TESTING USAGE EXAMPLE 2 (case 6)
//-----------------------------------------------------------------------------
struct my2_WorkItem {
    char *d_item;  // represents work to perform
};

struct my2_WorkQueue {
    list<my2_WorkItem>        d_queue;   // queue of work requests
    bcemt_Mutex               d_mx;      // protects the shared queue
    bcemt_Condition           d_cv;      // signals existence of new work
    bslma_Allocator          *d_alloc_p; // pooled allocator

    my2_WorkQueue(bslma_Allocator *a) : d_queue(a), d_alloc_p(a) {}
};

extern "C" void *my2_producer(void *arg)
{
    my2_WorkQueue *queue = (my2_WorkQueue *)arg;

    for (int i = 0; i < 50; ++i) {

        char b[100];
        sprintf(b, "%d", i);
        int len = strlen(b);

        my2_WorkItem request;
        request.d_item = (char *)queue->d_alloc_p->allocate(len+1);
        memcpy(request.d_item, b, len+1);

        if (veryVerbose) {
            MTCOUT << "Enqueueing " << request.d_item << MTENDL;
        }

        queue->d_mx.lock();
        queue->d_queue.push_back(request);
        queue->d_mx.unlock();
        queue->d_cv.signal();
    }

    my2_WorkItem request;
    request.d_item = 0;

    queue->d_mx.lock();
    queue->d_queue.push_back(request);
    queue->d_mx.unlock();
    queue->d_cv.signal();

    return queue;
}

extern "C" void *my2_consumer(void *arg)
{
    my2_WorkQueue *queue = (my2_WorkQueue *)arg;

    for (int i = 0; ; ++i) {

        queue->d_mx.lock();
        while (0 == queue->d_queue.size()) {
            queue->d_cv.wait(&queue->d_mx);
        }

        my2_WorkItem item = queue->d_queue.front();
        queue->d_queue.pop_front();
        queue->d_mx.unlock();

        if (0 == item.d_item) {
            break;
        }

        // Process the work requests.
        if (veryVerbose) {
            MTCOUT << "Processing " << item.d_item << MTENDL;
        }

        char b[100];
        sprintf(b, "%d", i);
        ASSERT(strcmp(b, item.d_item) == 0);

        queue->d_alloc_p->deallocate(item.d_item);
    }

    return 0;
}
//=============================================================================
//              HELPERS FOR TESTING USAGE EXAMPLE 1 (case 5)
//-----------------------------------------------------------------------------
struct my1_WorkItem {
    char *d_item;  // represents work to perform
};

struct my1_WorkQueue {
    list<my1_WorkItem>        d_queue;   // queue of work requests
    bcemt_Mutex               d_mx;      // protects the shared queue
    bcemt_Condition           d_cv;      // signals existence of new work
    bslma_Allocator          *d_alloc_p; // pooled allocator

    my1_WorkQueue(bslma_Allocator *a) : d_alloc_p(a) {}
};

extern "C" void *my1_producer(void *arg)
{
    my1_WorkQueue *queue = (my1_WorkQueue *)arg;

    for (int i = 0; i < 50; ++i) {

        char b[100];
        sprintf(b, "%d", i);
        int len = strlen(b);

        my1_WorkItem request;
        request.d_item = (char *)queue->d_alloc_p->allocate(100);
        memcpy(request.d_item, b, len+1);

        if (veryVerbose) {
            MTCOUT << "Enqueueing " << request.d_item << MTENDL;
        }

        queue->d_mx.lock();
        queue->d_queue.push_back(request);
        queue->d_mx.unlock();
        queue->d_cv.signal();
    }

    my1_WorkItem request;
    request.d_item = 0;

    queue->d_mx.lock();
    queue->d_queue.push_back(request);
    queue->d_mx.unlock();
    queue->d_cv.signal();

    return queue;
}

extern "C" void *my1_consumer(void *arg)
{
    my1_WorkQueue *queue = (my1_WorkQueue *)arg;

    for (int i = 0; ; ++i) {

        queue->d_mx.lock();
        while (0 == queue->d_queue.size()) {
            queue->d_cv.wait(&queue->d_mx);
        }

        my1_WorkItem item = queue->d_queue.front();
        queue->d_queue.pop_front();
        queue->d_mx.unlock();

        if (0 == item.d_item) {
            break;
        }

        // Process the work requests.
        if (veryVerbose) {
            MTCOUT << "Processing " << item.d_item << MTENDL;
        }

        char b[100];
        sprintf(b, "%d", i);
        ASSERT(strcmp(b, item.d_item) == 0);

        queue->d_alloc_p->deallocate(item.d_item);
    }

    return 0;
}
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 2
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

        if (verbose) cout << "\nUsage example 2" << endl;

        bcema_PoolAllocator poolAlloc(100);
        my2_WorkQueue queue(&poolAlloc);

        bcemt_Attribute attributes;

        bcemt_ThreadUtil::Handle producerHandle;
        int status = bcemt_ThreadUtil::create(&producerHandle,
                                               attributes,
                                              &my2_producer,
                                              &queue);
        ASSERT(0 == status);

        bcemt_ThreadUtil::Handle consumerHandle;
        status = bcemt_ThreadUtil::create(&consumerHandle,
                                           attributes,
                                          &my2_consumer,
                                          &queue);
        ASSERT(0 == status);
        status = bcemt_ThreadUtil::join(consumerHandle);
        ASSERT(0 == status);
        status = bcemt_ThreadUtil::join(producerHandle);
        ASSERT(0 == status);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
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

        if (verbose) cout << "\nUsage example 1" << endl;

        bcema_PoolAllocator poolAlloc;
        my1_WorkQueue queue(&poolAlloc);

        bcemt_Attribute attributes;

        bcemt_ThreadUtil::Handle producerHandle;
        int status = bcemt_ThreadUtil::create(&producerHandle,
                                               attributes,
                                              &my1_producer,
                                              &queue);
        ASSERT(0 == status);

        bcemt_ThreadUtil::Handle consumerHandle;
        status = bcemt_ThreadUtil::create(&consumerHandle,
                                           attributes,
                                          &my1_consumer,
                                          &queue);
        ASSERT(0 == status);
        status = bcemt_ThreadUtil::join(consumerHandle);
        ASSERT(0 == status);
        status = bcemt_ThreadUtil::join(producerHandle);
        ASSERT(0 == status);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // ACCEPTANCE TEST
        //
        // Concerns:
        //   The each constructor create validly functioning objects.
        //   The constructors include on default, and one with an
        //   optional argument.
        //
        // Plan:
        //   Test basic operations on object constructed by default
        //     constructor.
        //   Test basic operations on object constructed with   0 pooled size.
        //   Test basic operations on object constructed with 100 pooled size.
        //   Test basic operations on object constructed with   0 pooled size
        //     and an explicit allocator.
        //   Test basic operations on object constructed with 100 pooled size
        //     and an explicit allocator.
        //
        // Testing:
        //    constructors
        //    allocate(int size);
        //    deallocate(void *address);
        //    blockSize();
        // --------------------------------------------------------------------
        const int SA = 42;
        const int SB = 43;
        const int SC = SA + bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;

        if (verbose)
            cout << "bcema_PoolAllocator()" << endl;
        {
            bcema_TestAllocator ta;
            bslma_DefaultAllocatorGuard g(&ta);
            Obj x; Obj const &X = x;
            ASSERT(0 == X.blockSize());

            void *p2 = x.allocate(SB);
            ASSERT(SB == X.blockSize());
            int bu1 = ta.numBytesInUse();
            x.deallocate(p2);
            ASSERT(bu1 == ta.numBytesInUse());
            void *p1 = x.allocate(SA);
            ASSERT(bu1 == ta.numBytesInUse());
            x.deallocate(p1);
            ASSERT(bu1 == ta.numBytesInUse());
            void *p3 = x.allocate(SC);
            ASSERT(bu1 < ta.numBytesInUse());
            x.deallocate(p3);
            ASSERT(bu1 == ta.numBytesInUse());
        }

        if (verbose)
            cout << "bcema_PoolAllocator(0)" << endl;
        {
            bcema_TestAllocator ta;
            bslma_DefaultAllocatorGuard g(&ta);
            Obj x((bsls_PlatformUtil::size_type)0); Obj const &X = x;
            ASSERT(0 == X.blockSize());

            void *p2 = x.allocate(SB);
            ASSERT(SB == X.blockSize());
            int bu1 = ta.numBytesInUse();
            x.deallocate(p2);
            ASSERT(bu1 == ta.numBytesInUse());
            void *p1 = x.allocate(SA);
            ASSERT(bu1 == ta.numBytesInUse());
            x.deallocate(p1);
            ASSERT(bu1 == ta.numBytesInUse());
            void *p3 = x.allocate(SC);
            ASSERT(bu1 < ta.numBytesInUse());
            x.deallocate(p3);
            ASSERT(bu1 == ta.numBytesInUse());
        }

        if (verbose)
            cout << "bcema_PoolAllocator(100)" << endl;
        {
            bcema_TestAllocator ta;
            bslma_DefaultAllocatorGuard g(&ta);
            Obj x(100); Obj const &X = x;
            ASSERT(100 == X.blockSize());

            void *p2 = x.allocate(SB);
            ASSERT(100 == X.blockSize());
            int bu1 = ta.numBytesInUse();
            x.deallocate(p2);
            ASSERT(bu1 == ta.numBytesInUse());
            void *p1 = x.allocate(SA);
            ASSERT(bu1 == ta.numBytesInUse());
            x.deallocate(p1);
            ASSERT(bu1 == ta.numBytesInUse());
            void *p3 = x.allocate(SC);
            ASSERT(bu1 == ta.numBytesInUse());
            x.deallocate(p3);
            ASSERT(bu1 == ta.numBytesInUse());
        }

        if (verbose)
            cout << "bcema_PoolAllocator(allocator)" << endl;
        {
            bcema_TestAllocator ta;
            Obj x(&ta); Obj const &X = x;
            ASSERT(0 == X.blockSize());
            ASSERT(0 == ta.numBytesInUse());

            void *p2 = x.allocate(SB);
            ASSERT(SB == X.blockSize());
            int bu1 = ta.numBytesInUse();
            x.deallocate(p2);
            ASSERT(bu1 == ta.numBytesInUse());
            void *p1 = x.allocate(SA);
            ASSERT(bu1 == ta.numBytesInUse());
            x.deallocate(p1);
            ASSERT(bu1 == ta.numBytesInUse());
            void *p3 = x.allocate(SC);
            ASSERT(bu1 < ta.numBytesInUse());
            x.deallocate(p3);
            ASSERT(bu1 == ta.numBytesInUse());
        }

        if (verbose)
            cout << "bcema_PoolAllocator(0, allocator)" << endl;
        {
            bcema_TestAllocator ta;
            Obj x(0, &ta); Obj const &X = x;
            ASSERT(0 == X.blockSize());
            ASSERT(0 == ta.numBytesInUse());

            void *p2 = x.allocate(SB);
            ASSERT(SB == X.blockSize());
            int bu1 = ta.numBytesInUse();
            x.deallocate(p2);
            ASSERT(bu1 == ta.numBytesInUse());
            void *p1 = x.allocate(SA);
            ASSERT(bu1 == ta.numBytesInUse());
            x.deallocate(p1);
            ASSERT(bu1 == ta.numBytesInUse());
            void *p3 = x.allocate(SC);
            ASSERT(bu1 < ta.numBytesInUse());
            x.deallocate(p3);
            ASSERT(bu1 == ta.numBytesInUse());
        }

        if (verbose)
            cout << "bcema_PoolAllocator(100, allocator)" << endl;
        {
            bcema_TestAllocator ta;
            Obj x(100, &ta); Obj const &X = x;
            ASSERT(100 == X.blockSize());

            void *p2 = x.allocate(SB);
            ASSERT(100 == X.blockSize());
            int bu1 = ta.numBytesInUse();
            x.deallocate(p2);
            ASSERT(bu1 == ta.numBytesInUse());
            void *p1 = x.allocate(SA);
            ASSERT(bu1 == ta.numBytesInUse());
            x.deallocate(p1);
            ASSERT(bu1 == ta.numBytesInUse());
            void *p3 = x.allocate(SC);
            ASSERT(bu1 == ta.numBytesInUse());
            x.deallocate(p3);
            ASSERT(bu1 == ta.numBytesInUse());
        }

        if (verbose)
            cout << "bcema_PoolAllocator(growthStrategy, allocator)" << endl;
        {
            bsls_BlockGrowth::Strategy STRATEGY[] = {
                                            bsls_BlockGrowth::BSLS_GEOMETRIC,
                                            bsls_BlockGrowth::BSLS_CONSTANT };

            const int NUM_STRATEGY = sizeof STRATEGY / sizeof *STRATEGY;

            for (int i = 0; i < NUM_STRATEGY; ++i) {
                const bsls_BlockGrowth::Strategy STRAT = STRATEGY[i];

                if (veryVerbose) {
                    if (bsls_BlockGrowth::BSLS_GEOMETRIC == STRAT) {
                        cout << "\tBSLS_GEOMETRIC" << endl;
                    }
                    else {
                        cout << "\tBSLS_CONSTANT" << endl;
                    }
                }

                bcema_TestAllocator ta;
                bcema_TestAllocator tb;

                bcema_Pool pool(calculateMaxAlignedSize(SB + sizeof(Header)),
                                STRAT,
                                &ta);
                Obj x(STRAT, &tb);
                const Obj& X = x;

                void *a = pool.allocate();
                void *b = x.allocate(SB);

                ASSERT(SB == X.blockSize());
                ASSERT(ta.numBytesInUse() == tb.numBytesInUse());

                pool.deallocate(a);
                x.deallocate(b);

                // Make sure growth strategy is passed into 'bcema_Pool'
                // correctly.
                void *a1 = pool.allocate();
                void *a2 = pool.allocate();

                void *b1 = x.allocate(SB);
                void *b2 = x.allocate(SB);

                ASSERT(ta.numBytesInUse() == tb.numBytesInUse());
                pool.deallocate(a1);
                pool.deallocate(a2);
                x.deallocate(b1);
                x.deallocate(b2);
            }
        }

        if (verbose)
            cout << "bcema_PoolAllocator(0,growthStrategy,allocator)" << endl;
        {
            bsls_BlockGrowth::Strategy STRATEGY[] = {
                                            bsls_BlockGrowth::BSLS_GEOMETRIC,
                                            bsls_BlockGrowth::BSLS_CONSTANT };

            const int NUM_STRATEGY = sizeof STRATEGY / sizeof *STRATEGY;

            for (int i = 0; i < NUM_STRATEGY; ++i) {
                const bsls_BlockGrowth::Strategy STRAT = STRATEGY[i];

                if (veryVerbose) {
                    if (bsls_BlockGrowth::BSLS_GEOMETRIC == STRAT) {
                        cout << "\tBSLS_GEOMETRIC" << endl;
                    }
                    else {
                        cout << "\tBSLS_CONSTANT" << endl;
                    }
                }
                bcema_TestAllocator ta;
                bcema_TestAllocator tb;

                bcema_Pool pool(calculateMaxAlignedSize(SB + sizeof(Header)),
                                STRAT,
                                &ta);
                Obj x(0, STRAT, &tb);
                const Obj& X = x;

                void *a = pool.allocate();
                void *b = x.allocate(SB);

                ASSERT(SB == X.blockSize());
                ASSERT(ta.numBytesInUse() == tb.numBytesInUse());

                pool.deallocate(a);
                x.deallocate(b);

                // Make sure growth strategy is passed into 'bcema_Pool'
                // correctly.
                void *a1 = pool.allocate();
                void *a2 = pool.allocate();

                void *b1 = x.allocate(SB);
                void *b2 = x.allocate(SB);

                ASSERT(ta.numBytesInUse() == tb.numBytesInUse());
                pool.deallocate(a1);
                pool.deallocate(a2);
                x.deallocate(b1);
                x.deallocate(b2);
            }
        }

        if (verbose)
            cout<< "bcema_PoolAllocator(100,growthStrategy,allocator)" << endl;
        {
            bsls_BlockGrowth::Strategy STRATEGY[] = {
                                            bsls_BlockGrowth::BSLS_GEOMETRIC,
                                            bsls_BlockGrowth::BSLS_CONSTANT };

            const int NUM_STRATEGY = sizeof STRATEGY / sizeof *STRATEGY;

            for (int i = 0; i < NUM_STRATEGY; ++i) {
                const bsls_BlockGrowth::Strategy STRAT = STRATEGY[i];

                if (veryVerbose) {
                    if (bsls_BlockGrowth::BSLS_GEOMETRIC == STRAT) {
                        cout << "\tBSLS_GEOMETRIC" << endl;
                    }
                    else {
                        cout << "\tBSLS_CONSTANT" << endl;
                    }
                }
                bcema_TestAllocator ta;
                bcema_TestAllocator tb;

                bcema_Pool pool(calculateMaxAlignedSize(100 + sizeof(Header)),
                                STRAT,
                                &ta);
                Obj x(100, STRAT, &tb);
                const Obj& X = x;

                void *a = pool.allocate();
                void *b = x.allocate(SB);

                ASSERT(100 == X.blockSize());
                ASSERT(ta.numBytesInUse() == tb.numBytesInUse());

                pool.deallocate(a);
                x.deallocate(b);

                // Make sure growth strategy is passed into 'bcema_Pool'
                // correctly.
                void *a1 = pool.allocate();
                void *a2 = pool.allocate();

                void *b1 = x.allocate(SB);
                void *b2 = x.allocate(SB);

                ASSERT(ta.numBytesInUse() == tb.numBytesInUse());
                pool.deallocate(a1);
                pool.deallocate(a2);
                x.deallocate(b1);
                x.deallocate(b2);
            }
        }

        if (verbose)
            cout<< "bcema_PoolAllocator(growthStrategy, "
                                       "maxBlocksPerChunk, "
                                       "allocator)" << endl;
        {
            const bsls_BlockGrowth::Strategy STRATEGY[] = {
                                            bsls_BlockGrowth::BSLS_GEOMETRIC,
                                            bsls_BlockGrowth::BSLS_CONSTANT };
            const int MBPC[] = { 1, 2, 3, 4, 5, 15, 16, 17, 31, 32, 33 };

            const int NUM_STRATEGY = sizeof STRATEGY / sizeof *STRATEGY;
            const int NUM_MBPC     = sizeof MBPC / sizeof *MBPC;

            for (int j = 0; j < NUM_STRATEGY; ++j) {
                const bsls_BlockGrowth::Strategy STRAT = STRATEGY[j];

                if (veryVerbose) {
                    if (bsls_BlockGrowth::BSLS_GEOMETRIC == STRAT) {
                        cout << "\tBSLS_GEOMETRIC" << endl;
                    }
                    else {
                        cout << "\tBSLS_CONSTANT" << endl;
                    }
                }

                for (int i = 0; i < NUM_MBPC; ++i) {
                    const int MAXBLOCKSPERCHUNK = MBPC[i];

                    if (veryVerbose) { T_ T_ P(MAXBLOCKSPERCHUNK) }

                    bcema_TestAllocator ta;
                    bcema_TestAllocator tb;

                    bcema_Pool pool(
                                 calculateMaxAlignedSize(SB + sizeof(Header)),
                                 STRAT,
                                 MAXBLOCKSPERCHUNK,
                                 &ta);
                    Obj x(STRAT, MAXBLOCKSPERCHUNK, &tb);
                    const Obj& X = x;

                    void *a = pool.allocate();
                    void *b = x.allocate(SB);

                    ASSERT(SB == X.blockSize());
                    ASSERT(ta.numBytesInUse() == tb.numBytesInUse());

                    pool.deallocate(a);
                    x.deallocate(b);

                    // Make sure growth strategy is passed into 'bcema_Pool'
                    // correctly.
                    void *a1 = pool.allocate();
                    void *a2 = pool.allocate();

                    void *b1 = x.allocate(SB);
                    void *b2 = x.allocate(SB);

                    ASSERT(ta.numBytesInUse() == tb.numBytesInUse());
                    pool.deallocate(a1);
                    pool.deallocate(a2);
                    x.deallocate(b1);
                    x.deallocate(b2);
                }
            }
        }

        if (verbose)
            cout<< "bcema_PoolAllocator(0, "
                                       "growthStrategy, "
                                       "maxBlocksPerChunk, "
                                       "allocator)" << endl;
        {
            const bsls_BlockGrowth::Strategy STRATEGY[] = {
                                            bsls_BlockGrowth::BSLS_GEOMETRIC,
                                            bsls_BlockGrowth::BSLS_CONSTANT };
            const int MBPC[] = { 1, 2, 3, 4, 5, 15, 16, 17, 31, 32, 33 };

            const int NUM_STRATEGY = sizeof STRATEGY / sizeof *STRATEGY;
            const int NUM_MBPC     = sizeof MBPC / sizeof *MBPC;

            for (int j = 0; j < NUM_STRATEGY; ++j) {
                const bsls_BlockGrowth::Strategy STRAT = STRATEGY[j];

                if (veryVerbose) {
                    if (bsls_BlockGrowth::BSLS_GEOMETRIC == STRAT) {
                        cout << "\tBSLS_GEOMETRIC" << endl;
                    }
                    else {
                        cout << "\tBSLS_CONSTANT" << endl;
                    }
                }

                for (int i = 0; i < NUM_MBPC; ++i) {
                    const int MAXBLOCKSPERCHUNK = MBPC[i];

                    if (veryVerbose) { T_ T_ P(MAXBLOCKSPERCHUNK) }

                    bcema_TestAllocator ta;
                    bcema_TestAllocator tb;

                    bcema_Pool pool(
                                 calculateMaxAlignedSize(SB + sizeof(Header)),
                                 STRAT,
                                 MAXBLOCKSPERCHUNK,
                                 &ta);
                    Obj x(0, STRAT, MAXBLOCKSPERCHUNK, &tb);
                    const Obj& X = x;

                    void *a = pool.allocate();
                    void *b = x.allocate(SB);

                    ASSERT(SB == X.blockSize());
                    ASSERT(ta.numBytesInUse() == tb.numBytesInUse());

                    pool.deallocate(a);
                    x.deallocate(b);

                    // Make sure growth strategy is passed into 'bcema_Pool'
                    // correctly.
                    void *a1 = pool.allocate();
                    void *a2 = pool.allocate();

                    void *b1 = x.allocate(SB);
                    void *b2 = x.allocate(SB);

                    ASSERT(ta.numBytesInUse() == tb.numBytesInUse());
                    pool.deallocate(a1);
                    pool.deallocate(a2);
                    x.deallocate(b1);
                    x.deallocate(b2);
                }
            }
        }

        if (verbose)
            cout<< "bcema_PoolAllocator(100, "
                                       "growthStrategy, "
                                       "maxBlocksPerChunk, "
                                       "allocator)" << endl;
        {
            const bsls_BlockGrowth::Strategy STRATEGY[] = {
                                            bsls_BlockGrowth::BSLS_GEOMETRIC,
                                            bsls_BlockGrowth::BSLS_CONSTANT };
            const int MBPC[] = { 1, 2, 3, 4, 5, 15, 16, 17, 31, 32, 33 };

            const int NUM_STRATEGY = sizeof STRATEGY / sizeof *STRATEGY;
            const int NUM_MBPC     = sizeof MBPC / sizeof *MBPC;

            for (int j = 0; j < NUM_STRATEGY; ++j) {
                const bsls_BlockGrowth::Strategy STRAT = STRATEGY[j];

                if (veryVerbose) {
                    if (bsls_BlockGrowth::BSLS_GEOMETRIC == STRAT) {
                        cout << "\tBSLS_GEOMETRIC" << endl;
                    }
                    else {
                        cout << "\tBSLS_CONSTANT" << endl;
                    }
                }

                for (int i = 0; i < NUM_MBPC; ++i) {
                    const int MAXBLOCKSPERCHUNK = MBPC[i];

                    if (veryVerbose) { T_ T_ P(MAXBLOCKSPERCHUNK) }

                    bcema_TestAllocator ta;
                    bcema_TestAllocator tb;

                    bcema_Pool pool(
                                 calculateMaxAlignedSize(100 + sizeof(Header)),
                                 STRAT,
                                 MAXBLOCKSPERCHUNK,
                                 &ta);
                    Obj x(100, STRAT, MAXBLOCKSPERCHUNK, &tb);
                    const Obj& X = x;

                    void *a = pool.allocate();
                    void *b = x.allocate(SB);

                    ASSERT(100 == X.blockSize());
                    ASSERT(ta.numBytesInUse() == tb.numBytesInUse());

                    pool.deallocate(a);
                    x.deallocate(b);

                    // Make sure growth strategy is passed into 'bcema_Pool'
                    // correctly.
                    void *a1 = pool.allocate();
                    void *a2 = pool.allocate();

                    void *b1 = x.allocate(SB);
                    void *b2 = x.allocate(SB);

                    ASSERT(ta.numBytesInUse() == tb.numBytesInUse());
                    pool.deallocate(a1);
                    pool.deallocate(a2);
                    x.deallocate(b1);
                    x.deallocate(b2);
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // THREAD-SAFETY OF THE FIRST ALLOCATION
        //
        // Concerns:
        //   Multiple, concurrent threads must be able to use the object
        //   for the 'allocate' method (and others).
        //
        // Plan:
        //   Create 'NUM_ALLOCATOR' (200) objects and two threads.
        //   Each thread will invoke the 'allocator' method of each object.
        //
        // Testing:
        //   allocate(int size);
        // --------------------------------------------------------------------

        for (int i=0; i<1000; i++) {
            bcema_TestAllocator ta;
            bslma_DefaultAllocatorGuard g(&ta);
            Obj a[NUM_ALLOCATORS];

            bcemt_Attribute attributes;

            bcemt_ThreadUtil::Handle upHandle;
            int status = bcemt_ThreadUtil::create(&upHandle,
                                                  attributes,
                                                  &my3_up,
                                                  a);
            ASSERT(0 == status);

            bcemt_ThreadUtil::Handle downHandle;
            status = bcemt_ThreadUtil::create(&downHandle,
                                              attributes,
                                              &my3_down,
                                              a);
            ASSERT(0 == status);
            status = bcemt_ThreadUtil::join(upHandle);
            ASSERT(0 == status);
            status = bcemt_ThreadUtil::join(downHandle);
            ASSERT(0 == status);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST WITH ZERO ARGUMENTS
        //
        // Concerns:
        //   Do allocations of zero bytes or deallocations of zero point
        //   work correctly or invalidate the object for subsequent
        //   non-zero operations?
        //
        // Plan:
        //   Create an object under test and perform basic operations with
        //   zero arguments then non-zero arguments then zero arguments.
        //
        // Testing:
        //   blockSize();
        //   allocate(int size);
        //   deallocate(void *address);
        // --------------------------------------------------------------------
        bcema_TestAllocator ta;
        bslma_DefaultAllocatorGuard g(&ta);
        Obj x; Obj const &X = x;
        ASSERT(0 == X.blockSize());

        ASSERT(0 == x.allocate(0));
        ASSERT(0 == X.blockSize());
        x.deallocate(0);
        ASSERT(0 == X.blockSize());

        void *p2 = x.allocate(33);
        ASSERT(33 == X.blockSize());
        int bu1 = ta.numBytesInUse();
        x.deallocate(p2);
        ASSERT(bu1 == ta.numBytesInUse());

        ASSERT(0 == x.allocate(0));
        ASSERT(bu1 == ta.numBytesInUse());
        ASSERT(33 == X.blockSize());
        x.deallocate(0);
        ASSERT(bu1 == ta.numBytesInUse());
        ASSERT(33 == X.blockSize());

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Concerns:
        //   Can an object be created and basic operations performed?
        //
        // Plan:
        //   Create an object under test and perform basic operations.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        const int SA = 44;
        const int SB = 42;
        const int SC = SA + bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;

        Obj x; Obj const &X = x;
        ASSERT(0 == X.blockSize());
        void *p1 = x.allocate(SA);

        ASSERT(0 != p1);
        ASSERT(SA == x.blockSize());

        void *p2 = x.allocate(SB);
        ASSERT(SA == x.blockSize());

        void *p3 = x.allocate(SC);
        ASSERT(SA == x.blockSize());

        x.deallocate(p3);

        x.deallocate(p2);

        void *p4 = x.allocate(SC);
        ASSERT (p2 != p4);

        void *p5 = x.allocate(SB);
        ASSERT (p2 == p5);

        x.deallocate(p1);
        void *p6 = x.allocate(SB);
        ASSERT (p1 == p6);
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
