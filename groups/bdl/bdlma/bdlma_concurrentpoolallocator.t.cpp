// bdlma_concurrentpoolallocator.t.cpp                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlma_concurrentpoolallocator.h>

#include <bslim_testutil.h>

#include <bslmt_condition.h>
#include <bslmt_threadutil.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bsls_alignmentutil.h>
#include <bsls_types.h>

#include <bsl_cstring.h>  // 'strcmp'
#include <bsl_cstdlib.h>  // 'atoi'
#include <bsl_cstdio.h>   // 'sprintf'
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
// [ 4] bdlma::ConcurrentPoolAllocator(size_type size, bslma::Allocator * = 0);
// [ 4] bdlma::ConcurrentPoolAllocator(bslma::Allocator *allocator = 0);
// [ 4] ~bdlma::ConcurrentPoolAllocator();
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
// [ 5] USAGE

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

// The following macros facilitate thread-safe streaming to standard output.

#define MTCOUT \
coutMutex.lock(); \
{ bsl::cout << bslmt::ThreadUtil::selfIdAsInt() << ": "

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

static bslmt::Mutex coutMutex;

typedef bdlma::ConcurrentPoolAllocator Obj;

//=============================================================================
//                        HELPERS FOR TEST CASE 4
//-----------------------------------------------------------------------------
// The following structure and function is copied from the implementation
// directly.

union Header {
    // Leading header on each allocated memory block.  If the memory block was
    // allocated from the pool, the 'd_magicNumber' will be set to
    // 'MAGIC_NUMBER'.  Otherwise, memory was allocated by the external
    // allocator supplied during construction, and 'd_magicNumber' will be
    // zero.

    int                                 d_magicNumber;  // allocation source
                                                        // and sanity check

    bsls::AlignmentUtil::MaxAlignedType d_dummy;        // dummy padding
};

static inline
int calculateMaxAlignedSize(int totalSize)
{
    using namespace BloombergLP;

    const int objectAlignmentMin = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1;
    const int ret = (totalSize + objectAlignmentMin) & ~(objectAlignmentMin);
    BSLS_ASSERT(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT ==
                         bsls::AlignmentUtil::calculateAlignmentFromSize(ret));
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
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------
///Usage
///-----
// The 'bdlma::ConcurrentPoolAllocator' is intended to be used in either of the
// following two cases.
//
// The first case is where frequent allocation and deallocation of memory
// occurs through the 'bslma::Allocator' protocol and all of the allocated
// blocks have the same size.  In this case, the size of blocks to pool is
// determined the first time 'allocate' is called and need not be specified at
// construction.
//
// The second case is where frequent allocation and deallocation of memory
// occurs through the 'bslma::Allocator' protocol, most of the allocations have
// similar sizes, and a likely maximum for the largest allocation is known at
// the time of construction.
//
///Example 1 - Uniform Sized Allocations
///- - - - - - - - - - - - - - - - - - -
// The following example illustrates the use of
// 'bdlma::ConcurrentPoolAllocator' when all allocations are of uniform size.
// A 'bdlma::ConcurrentPoolAllocator' is used in the implementation of a "work
// queue" where each "item" enqueued by a producer thread is of identical size.
// Concurrently, a consumer dequeues each work item when it becomes available,
// verifies the content (a sequence number in ASCII), and deallocates the work
// item.  The concurrent allocations and deallocations are valid because
// 'bdlma::ConcurrentPoolAllocator' is thread-safe.
//
// First, an abstract of the example will be given with focus and commentary on
// the relevant details of 'bdlma::ConcurrentPoolAllocator'.  Details
// pertaining to queue management, thread creation, thread synchronization,
// etc., can be seen in the full listing at the end of this example.
//
// The parent thread creates the 'bdlma::ConcurrentPoolAllocator' and work
// queue by the statements:
//..
//  bdlma::ConcurrentPoolAllocator poolAlloc;
//  my1_WorkQueue queue(&poolAlloc);
//..
// Note that since the default constructor is used to create 'poolAlloc', the
// pooled size has not yet been fixed.
//
// The work queue is defined by the following data structures.
//..
//  struct my1_WorkItem {
//      // DATA
//      char *d_item;  // represents work to perform
//  };
//
//  struct my1_WorkQueue {
//      // DATA
//      bsl::list<my1_WorkItem>  d_queue;    // queue of work requests
//      bslmt::Mutex             d_mx;       // protects the shared queue
//      bslmt::Condition         d_cv;       // signals existence of new work
//      bslma::Allocator        *d_alloc_p;  // pooled allocator
//
//      // CREATORS
//      explicit my1_WorkQueue(bslma::Allocator *basicAllocator = 0)
//      : d_alloc_p(bslma::Default::allocator(basicAllocator))
//      {
//      }
//  };
//..
// The producer and consumer threads are given the address of the work queue as
// their sole argument.  Here, the producer allocates a work item, initializes
// it with a sequence number in ASCII, enqueues it, and signals its presence to
// the consumer thread.  This action is done 50 times, and then a 51st, empty
// work item is added to inform the consumer of the end of the queue.  The
// first allocation of a work item (100 bytes) fixes the pooled size.  Each
// subsequent allocation is that same size (100 bytes).  The producer's actions
// are shown below:
//..
//  extern "C"
//  void *my1_producer(void *arg)
//  {
//      my1_WorkQueue *queue = (my1_WorkQueue *)arg;
//
//      for (int i = 0; i < 50; ++i) {
//          char b[100];
//          bsl::sprintf(b, "%d", i);
//          int len = static_cast<int>(bsl::strlen(b));
//
//          my1_WorkItem request;
//
//          // Fixed allocation size sufficient for content.
//
//          request.d_item = (char *)queue->d_alloc_p->allocate(100);
//
//          bsl::memcpy(request.d_item, b, len+1);
//
//          // Enqueue item and signal any waiting threads.
//          // ...
//      }
//
//      // Add empty item.
//      // ...
//
//      return queue;
//  }
//..
// When the consumer thread finds that the queue is not empty it dequeues the
// item, verifies its content (a sequence number in ASCII), returns the work
// item to the pool, and checks for the next item.  If the queue is empty, the
// consumer blocks until signaled by the producer.  An empty work item
// indicates that the producer will send no more items, so the consumer exits.
// The consumer's actions are shown below:
//..
//  extern "C"
//  void *my1_consumer(void *arg)
//  {
//      my1_WorkQueue *queue = (my1_WorkQueue *)arg;
//
//      for (int i = 0; ; ++i) {
//
//          // Block until work item on queue.
//          // ...
//
//          // Dequeue item.
//          // ...
//
//          // Break when end-of-work item received.
//          // ...
//
//          char b[100];
//          bsl::sprintf(b, "%d", i);
//          ASSERT(bsl::strcmp(b, item.d_item) == 0);   // check content
//
//          queue->d_alloc_p->deallocate(item.d_item);  // deallocate
//      }
//
//      return 0;
//  }
//..
// A complete listing of the example's structures and functions follows:
//..
    struct my1_WorkItem {
        // DATA
        char *d_item;  // represents work to perform
    };

    struct my1_WorkQueue {
        // DATA
        bsl::list<my1_WorkItem>  d_queue;    // queue of work requests
        bslmt::Mutex             d_mx;       // protects the shared queue
        bslmt::Condition         d_cv;       // signals existence of new work
        bslma::Allocator        *d_alloc_p;  // pooled allocator

      private:
        // Not implemented:
        my1_WorkQueue(const my1_WorkQueue&);

      public:
        // CREATORS
        explicit my1_WorkQueue(bslma::Allocator *basicAllocator = 0)
        : d_alloc_p(bslma::Default::allocator(basicAllocator))
        {
        }
    };

    extern "C" void *my1_producer(void *arg)
    {
        my1_WorkQueue *queue = (my1_WorkQueue *)arg;

        for (int i = 0; i < 50; ++i) {

            char b[100];
            bsl::sprintf(b, "%d", i);
            int len = static_cast<int>(bsl::strlen(b));

            my1_WorkItem request;
            request.d_item = (char *)queue->d_alloc_p->allocate(100);
            bsl::memcpy(request.d_item, b, len+1);

            if (veryVerbose) {
                // Assume thread-safe implementations of 'cout' and 'endl'
                // exist (named 'MTCOUT' and 'MTENDL', respectively).

                MTCOUT << "Enqueuing " << request.d_item << MTENDL;
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
                // Assume thread-safe implementations of 'cout' and 'endl'
                // exist (named 'MTCOUT' and 'MTENDL', respectively).

                MTCOUT << "Processing " << item.d_item << MTENDL;
            }

            char b[100];
            bsl::sprintf(b, "%d", i);
            ASSERT(bsl::strcmp(b, item.d_item) == 0);

            queue->d_alloc_p->deallocate(item.d_item);
        }

        return 0;
    }
//..

//
///Example 2 - Variable Allocation Size
/// - - - - - - - - - - - - - - - - - -
// The following example illustrates the use of
// 'bdlma::ConcurrentPoolAllocator' when allocations are of varying size.  A
// 'bdlma::ConcurrentPoolAllocator' is used in the implementation of a "work
// queue" where each "item" enqueued by a producer thread varies in size, but
// all items are smaller than a known maximum.  Concurrently, a consumer thread
// dequeues each work item when it is available, verifies its content (a
// sequence number in ASCII), and deallocates the work item.  The concurrent
// allocations and deallocations are valid because
// 'bdlma::ConcurrentPoolAllocator' is thread-safe.
//
// First, an abstract of the example will be given with focus and commentary on
// the relevant details of 'bdlma::ConcurrentPoolAllocator'.  Details
// pertaining to queue management, thread creation, thread synchronization,
// etc., can be seen in the full listing at the end of this example.
//
// The parent thread creates the 'bdlma::ConcurrentPoolAllocator' and work
// queue by the statements:
//..
//  bdlma::ConcurrentPoolAllocator poolAlloc(100);
//  my1_WorkQueue queue(&poolAlloc);
//..
// Note that the pooled size (100) is specified in the construction of
// 'poolAlloc'.  Any requests in excess of that size will be satisfied by
// implicit calls to the default allocator, not from the underlying pool.
//
// The work queue is defined by the following data structures.
//..
//  struct my2_WorkItem {
//      // DATA
//      char *d_item;  // represents work to perform
//  };
//
//  struct my2_WorkQueue {
//      // DATA
//      bsl::list<my2_WorkItem>  d_queue;    // queue of work requests
//      bslmt::Mutex             d_mx;       // protects the shared queue
//      bslmt::Condition         d_cv;       // signals existence of new work
//      bslma::Allocator        *d_alloc_p;  // pooled allocator
//
//      // CREATORS
//      explicit my2_WorkQueue(bslma::Allocator *basicAllocator = 0)
//      : d_queue(basic_Allocator)
//      , d_alloc_p(bslma::Default::allocator(basic_Allocator))
//      {
//      }
//  };
//..
// In this example (unlike Example 1), the given allocator is used not only for
// the work items, but is also passed to the constructor of 'd_queue' so that
// it also serves memory for the operations of 'bsl::list<my2_WorkItem>'.
//
// The producer and consumer threads are given the address of the work queue as
// their sole argument.  Here, the producer allocates a work item, initializes
// it with a sequence number in ASCII, enqueues it, and signals its presence to
// the consumer thread.  The action is done 50 times, and then a 51st, empty
// work item is added to inform the consumer of the end of the queue.  In this
// example, each work item is sized to match the length of its contents, the
// sequence number in ASCII.  The producer's actions are shown below:
//..
//  extern "C" void *my2_producer(void *arg)
//  {
//      my2_WorkQueue *queue = (my2_WorkQueue *)arg;
//
//      for (int i = 0; i < 50; ++i) {
//
//          char b[100];
//          bsl::sprintf(b, "%d", i);
//          int len = static_cast<int>(bsl::strlen(b));
//
//          my2_WorkItem request;
//
//          // Allocate item to exactly match space needed for content.
//
//          request.d_item = (char *)queue->d_alloc_p->allocate(len+1);
//
//          bsl::memcpy(request.d_item, b, len+1);
//
//          // Enqueue item and signal any waiting threads.
//          // ...
//      }
//
//      // Add empty item.
//      // ...
//
//      return queue;
//  }
//..
// The actions of this consumer thread are essentially the same as those of the
// consumer thread in Example 1.
//
// When the consumer thread finds that the queue is not empty, it dequeues the
// item, verifies its content (a sequence number in ASCII), returns the work
// item to the pool, and checks for the next item.  If the queue is empty, the
// consumer blocks until signaled by the producer.  An empty work item
// indicates that the producer will send no more items, so the consumer exits.
// The consumer's actions are shown below.
//..
//  extern "C" void *my2_consumer(void *arg)
//  {
//      my2_WorkQueue *queue = (my2_WorkQueue *)arg;
//
//      while (int i = 0; ; ++i) {
//
//          // Block until work item on queue.
//          // ...
//
//          // Deque item.
//          // ...
//
//          // Break when end-of-work item received.
//          // ...
//
//          char b[100];
//          bsl::sprintf(b, "%d", i);
//          ASSERT(bsl::strcmp(b, item.d_item) == 0);   // verify content
//
//          queue->d_alloc_p->deallocate(item.d_item);  // deallocate
//      }
//
//      return 0;
//  }
//..
// A complete listing of the example's structures and functions follows:
//..
    struct my2_WorkItem {
        // DATA
        char *d_item;  // represents work to perform
    };

    struct my2_WorkQueue {
        // DATA
        bsl::list<my2_WorkItem>  d_queue;    // queue of work requests
        bslmt::Mutex             d_mx;       // protects the shared queue
        bslmt::Condition         d_cv;       // signals existence of new work
        bslma::Allocator        *d_alloc_p;  // pooled allocator

      private:
        // Not implemented:
        my2_WorkQueue(const my2_WorkQueue&);

      public:
        // CREATORS
        explicit my2_WorkQueue(bslma::Allocator *basicAllocator = 0)
        : d_queue(basicAllocator)
        , d_alloc_p(basicAllocator)
        {
        }
    };

    extern "C" void *my2_producer(void *arg)
    {
        my2_WorkQueue *queue = (my2_WorkQueue *)arg;

        for (int i = 0; i < 50; ++i) {

            char b[100];
            bsl::sprintf(b, "%d", i);
            int len = static_cast<int>(bsl::strlen(b));

            my2_WorkItem request;
            request.d_item = (char *)queue->d_alloc_p->allocate(len+1);
            bsl::memcpy(request.d_item, b, len+1);

            if (veryVerbose) {
                // Assume thread-safe implementations of 'cout' and 'endl'
                // exist (named 'MTCOUT' and 'MTENDL', respectively).

                MTCOUT << "Enqueuing " << request.d_item << MTENDL;
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
                // Assume thread-safe implementations of 'cout' and 'endl'
                // exist (named 'MTCOUT' and 'MTENDL', respectively).

                MTCOUT << "Processing " << item.d_item << MTENDL;
            }

            char b[100];
            bsl::sprintf(b, "%d", i);
            ASSERT(bsl::strcmp(b, item.d_item) == 0);

            queue->d_alloc_p->deallocate(item.d_item);
        }

        return 0;
    }
//..

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
      case 5: {
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

        if (verbose) cout << "\nUsage example" << endl;

// In the application 'main':
//..
    {
        bdlma::ConcurrentPoolAllocator poolAlloc;
        my1_WorkQueue queue(&poolAlloc);

        bslmt::ThreadAttributes attributes;

        bslmt::ThreadUtil::Handle producerHandle;
        int status = bslmt::ThreadUtil::create(&producerHandle,
                                               attributes,
                                               &my1_producer,
                                               &queue);
        ASSERT(0 == status);

        bslmt::ThreadUtil::Handle consumerHandle;
        status = bslmt::ThreadUtil::create(&consumerHandle,
                                           attributes,
                                           &my1_consumer,
                                           &queue);
        ASSERT(0 == status);
        status = bslmt::ThreadUtil::join(consumerHandle);
        ASSERT(0 == status);
        status = bslmt::ThreadUtil::join(producerHandle);
        ASSERT(0 == status);
    }
//..

// In the application's 'main':
//..
    {
        bdlma::ConcurrentPoolAllocator poolAlloc(100);
        my2_WorkQueue queue(&poolAlloc);

        bslmt::ThreadAttributes attributes;

        bslmt::ThreadUtil::Handle producerHandle;
        int status = bslmt::ThreadUtil::create(&producerHandle,
                                              attributes,
                                              &my2_producer,
                                              &queue);
        ASSERT(0 == status);

        bslmt::ThreadUtil::Handle consumerHandle;
        status = bslmt::ThreadUtil::create(&consumerHandle,
                                          attributes,
                                          &my2_consumer,
                                          &queue);
        ASSERT(0 == status);
        status = bslmt::ThreadUtil::join(consumerHandle);
        ASSERT(0 == status);
        status = bslmt::ThreadUtil::join(producerHandle);
        ASSERT(0 == status);
    }
//..

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
        const int SC = SA + bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

        if (verbose)
            cout << "bdlma::ConcurrentPoolAllocator()" << endl;
        {
            bslma::TestAllocator ta;
            bslma::DefaultAllocatorGuard g(&ta);
            Obj x; Obj const &X = x;
            ASSERT(0 == X.blockSize());

            void *p2 = x.allocate(SB);
            ASSERT(SB == X.blockSize());
            bsls::Types::Int64 bu1 = ta.numBytesInUse();
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
            cout << "bdlma::ConcurrentPoolAllocator(0)" << endl;
        {
            bslma::TestAllocator ta;
            bslma::DefaultAllocatorGuard g(&ta);
            Obj x((bsls::Types::size_type)0); Obj const &X = x;
            ASSERT(0 == X.blockSize());

            void *p2 = x.allocate(SB);
            ASSERT(SB == X.blockSize());
            bsls::Types::Int64 bu1 = ta.numBytesInUse();
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
            cout << "bdlma::ConcurrentPoolAllocator(100)" << endl;
        {
            bslma::TestAllocator ta;
            bslma::DefaultAllocatorGuard g(&ta);
            Obj x(100); Obj const &X = x;
            ASSERT(100 == X.blockSize());

            void *p2 = x.allocate(SB);
            ASSERT(100 == X.blockSize());
            bsls::Types::Int64 bu1 = ta.numBytesInUse();
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
            cout << "bdlma::ConcurrentPoolAllocator(allocator)" << endl;
        {
            bslma::TestAllocator ta;
            Obj x(&ta); Obj const &X = x;
            ASSERT(0 == X.blockSize());
            ASSERT(0 == ta.numBytesInUse());

            void *p2 = x.allocate(SB);
            ASSERT(SB == X.blockSize());
            bsls::Types::Int64 bu1 = ta.numBytesInUse();
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
            cout << "bdlma::ConcurrentPoolAllocator(0, allocator)" << endl;
        {
            bslma::TestAllocator ta;
            Obj x(0, &ta); Obj const &X = x;
            ASSERT(0 == X.blockSize());
            ASSERT(0 == ta.numBytesInUse());

            void *p2 = x.allocate(SB);
            ASSERT(SB == X.blockSize());
            bsls::Types::Int64 bu1 = ta.numBytesInUse();
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
            cout << "bdlma::ConcurrentPoolAllocator(100, allocator)" << endl;
        {
            bslma::TestAllocator ta;
            Obj x(100, &ta); Obj const &X = x;
            ASSERT(100 == X.blockSize());

            void *p2 = x.allocate(SB);
            ASSERT(100 == X.blockSize());
            bsls::Types::Int64 bu1 = ta.numBytesInUse();
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
            cout << "bdlma::ConcurrentPoolAllocator(growthStrategy, allocator)"
                 << endl;
        {
            bsls::BlockGrowth::Strategy STRATEGY[] = {
                                            bsls::BlockGrowth::BSLS_GEOMETRIC,
                                            bsls::BlockGrowth::BSLS_CONSTANT };

            const int NUM_STRATEGY = sizeof STRATEGY / sizeof *STRATEGY;

            for (int i = 0; i < NUM_STRATEGY; ++i) {
                const bsls::BlockGrowth::Strategy STRAT = STRATEGY[i];

                if (veryVerbose) {
                    if (bsls::BlockGrowth::BSLS_GEOMETRIC == STRAT) {
                        cout << "\tBSLS_GEOMETRIC" << endl;
                    }
                    else {
                        cout << "\tBSLS_CONSTANT" << endl;
                    }
                }

                bslma::TestAllocator ta;
                bslma::TestAllocator tb;

                bdlma::ConcurrentPool pool(
                                  calculateMaxAlignedSize(SB + sizeof(Header)),
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

                // Make sure growth strategy is passed into
                // 'bdlma::ConcurrentPool' correctly.
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
            cout << "bdlma::ConcurrentPoolAllocator(0,growthStrategy,"
                 << "allocator)"
                 << endl;
        {
            bsls::BlockGrowth::Strategy STRATEGY[] = {
                                            bsls::BlockGrowth::BSLS_GEOMETRIC,
                                            bsls::BlockGrowth::BSLS_CONSTANT };

            const int NUM_STRATEGY = sizeof STRATEGY / sizeof *STRATEGY;

            for (int i = 0; i < NUM_STRATEGY; ++i) {
                const bsls::BlockGrowth::Strategy STRAT = STRATEGY[i];

                if (veryVerbose) {
                    if (bsls::BlockGrowth::BSLS_GEOMETRIC == STRAT) {
                        cout << "\tBSLS_GEOMETRIC" << endl;
                    }
                    else {
                        cout << "\tBSLS_CONSTANT" << endl;
                    }
                }
                bslma::TestAllocator ta;
                bslma::TestAllocator tb;

                bdlma::ConcurrentPool pool(
                                  calculateMaxAlignedSize(SB + sizeof(Header)),
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

                // Make sure growth strategy is passed into
                // 'bdlma::ConcurrentPool' correctly.
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
            cout << "bdlma::ConcurrentPoolAllocator(100,growthStrategy,"
                 << "allocator)"
                 << endl;
        {
            bsls::BlockGrowth::Strategy STRATEGY[] = {
                                            bsls::BlockGrowth::BSLS_GEOMETRIC,
                                            bsls::BlockGrowth::BSLS_CONSTANT };

            const int NUM_STRATEGY = sizeof STRATEGY / sizeof *STRATEGY;

            for (int i = 0; i < NUM_STRATEGY; ++i) {
                const bsls::BlockGrowth::Strategy STRAT = STRATEGY[i];

                if (veryVerbose) {
                    if (bsls::BlockGrowth::BSLS_GEOMETRIC == STRAT) {
                        cout << "\tBSLS_GEOMETRIC" << endl;
                    }
                    else {
                        cout << "\tBSLS_CONSTANT" << endl;
                    }
                }
                bslma::TestAllocator ta;
                bslma::TestAllocator tb;

                bdlma::ConcurrentPool pool(
                                 calculateMaxAlignedSize(100 + sizeof(Header)),
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

                // Make sure growth strategy is passed into
                // 'bdlma::ConcurrentPool' correctly.
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
            cout<< "bdlma::ConcurrentPoolAllocator(growthStrategy, "
                                       "maxBlocksPerChunk, "
                                       "allocator)" << endl;
        {
            const bsls::BlockGrowth::Strategy STRATEGY[] = {
                                            bsls::BlockGrowth::BSLS_GEOMETRIC,
                                            bsls::BlockGrowth::BSLS_CONSTANT };
            const int MBPC[] = { 1, 2, 3, 4, 5, 15, 16, 17, 31, 32, 33 };

            const int NUM_STRATEGY = sizeof STRATEGY / sizeof *STRATEGY;
            const int NUM_MBPC     = sizeof MBPC / sizeof *MBPC;

            for (int j = 0; j < NUM_STRATEGY; ++j) {
                const bsls::BlockGrowth::Strategy STRAT = STRATEGY[j];

                if (veryVerbose) {
                    if (bsls::BlockGrowth::BSLS_GEOMETRIC == STRAT) {
                        cout << "\tBSLS_GEOMETRIC" << endl;
                    }
                    else {
                        cout << "\tBSLS_CONSTANT" << endl;
                    }
                }

                for (int i = 0; i < NUM_MBPC; ++i) {
                    const int MAXBLOCKSPERCHUNK = MBPC[i];

                    if (veryVerbose) { T_ T_ P(MAXBLOCKSPERCHUNK) }

                    bslma::TestAllocator ta;
                    bslma::TestAllocator tb;

                    bdlma::ConcurrentPool pool(
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

                    // Make sure growth strategy is passed into
                    // 'bdlma::ConcurrentPool' correctly.
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
            cout<< "bdlma::ConcurrentPoolAllocator(0, "
                                       "growthStrategy, "
                                       "maxBlocksPerChunk, "
                                       "allocator)" << endl;
        {
            const bsls::BlockGrowth::Strategy STRATEGY[] = {
                                            bsls::BlockGrowth::BSLS_GEOMETRIC,
                                            bsls::BlockGrowth::BSLS_CONSTANT };
            const int MBPC[] = { 1, 2, 3, 4, 5, 15, 16, 17, 31, 32, 33 };

            const int NUM_STRATEGY = sizeof STRATEGY / sizeof *STRATEGY;
            const int NUM_MBPC     = sizeof MBPC / sizeof *MBPC;

            for (int j = 0; j < NUM_STRATEGY; ++j) {
                const bsls::BlockGrowth::Strategy STRAT = STRATEGY[j];

                if (veryVerbose) {
                    if (bsls::BlockGrowth::BSLS_GEOMETRIC == STRAT) {
                        cout << "\tBSLS_GEOMETRIC" << endl;
                    }
                    else {
                        cout << "\tBSLS_CONSTANT" << endl;
                    }
                }

                for (int i = 0; i < NUM_MBPC; ++i) {
                    const int MAXBLOCKSPERCHUNK = MBPC[i];

                    if (veryVerbose) { T_ T_ P(MAXBLOCKSPERCHUNK) }

                    bslma::TestAllocator ta;
                    bslma::TestAllocator tb;

                    bdlma::ConcurrentPool pool(
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

                    // Make sure growth strategy is passed into
                    // 'bdlma::ConcurrentPool' correctly.
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
            cout<< "bdlma::ConcurrentPoolAllocator(100, "
                                       "growthStrategy, "
                                       "maxBlocksPerChunk, "
                                       "allocator)" << endl;
        {
            const bsls::BlockGrowth::Strategy STRATEGY[] = {
                                            bsls::BlockGrowth::BSLS_GEOMETRIC,
                                            bsls::BlockGrowth::BSLS_CONSTANT };
            const int MBPC[] = { 1, 2, 3, 4, 5, 15, 16, 17, 31, 32, 33 };

            const int NUM_STRATEGY = sizeof STRATEGY / sizeof *STRATEGY;
            const int NUM_MBPC     = sizeof MBPC / sizeof *MBPC;

            for (int j = 0; j < NUM_STRATEGY; ++j) {
                const bsls::BlockGrowth::Strategy STRAT = STRATEGY[j];

                if (veryVerbose) {
                    if (bsls::BlockGrowth::BSLS_GEOMETRIC == STRAT) {
                        cout << "\tBSLS_GEOMETRIC" << endl;
                    }
                    else {
                        cout << "\tBSLS_CONSTANT" << endl;
                    }
                }

                for (int i = 0; i < NUM_MBPC; ++i) {
                    const int MAXBLOCKSPERCHUNK = MBPC[i];

                    if (veryVerbose) { T_ T_ P(MAXBLOCKSPERCHUNK) }

                    bslma::TestAllocator ta;
                    bslma::TestAllocator tb;

                    bdlma::ConcurrentPool pool(
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

                    // Make sure growth strategy is passed into
                    // 'bdlma::ConcurrentPool' correctly.
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
            bslma::TestAllocator ta;
            bslma::DefaultAllocatorGuard g(&ta);
            Obj a[NUM_ALLOCATORS];

            bslmt::ThreadAttributes attributes;

            bslmt::ThreadUtil::Handle upHandle;
            int status = bslmt::ThreadUtil::create(&upHandle,
                                                  attributes,
                                                  &my3_up,
                                                  static_cast<void *>(a));
            ASSERT(0 == status);

            bslmt::ThreadUtil::Handle downHandle;
            status = bslmt::ThreadUtil::create(&downHandle,
                                              attributes,
                                              &my3_down,
                                              static_cast<void *>(a));
            ASSERT(0 == status);
            status = bslmt::ThreadUtil::join(upHandle);
            ASSERT(0 == status);
            status = bslmt::ThreadUtil::join(downHandle);
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
        bslma::TestAllocator ta;
        bslma::DefaultAllocatorGuard g(&ta);
        Obj x; Obj const &X = x;
        ASSERT(0 == X.blockSize());

        ASSERT(0 == x.allocate(0));
        ASSERT(0 == X.blockSize());
        x.deallocate(0);
        ASSERT(0 == X.blockSize());

        void *p2 = x.allocate(33);
        ASSERT(33 == X.blockSize());
        bsls::Types::Int64 bu1 = ta.numBytesInUse();
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
        const int SC = SA + bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

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
