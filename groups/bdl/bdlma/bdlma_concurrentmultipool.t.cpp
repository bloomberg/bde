// bdlma_concurrentmultipool.t.cpp                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlma_concurrentmultipool.h>
#include <bdlma_concurrentpool.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>                // for testing only

#include <bdlma_bufferedsequentialallocator.h>

#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bslmt_barrier.h>                      // for testing only
#include <bslmt_threadutil.h>

#include <bsls_alignmentutil.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsl_iostream.h>
#include <bsl_vector.h>
#include <bsl_cstdlib.h>                         // 'atoi'
#include <bsl_cstring.h>                         // 'memcpy', 'memset'
#include <bsl_cstdint.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The 'bdlma::ConcurrentMultipool' class consists of one constructor, a
// destructor, and four manipulators.  The manipulators are used to allocate,
// deallocate, and reserve memory.  Since this component is a memory manager,
// the 'bslma_testallocator' component is used extensively to verify expected
// behaviors.  Note that the copying of objects is explicitly disallowed since
// the copy constructor and assignment operator are declared 'private' and left
// unimplemented.  So we are primarily concerned that the internal memory
// management system functions as expected and that the manipulators operator
// correctly.  Note that memory allocation must be tested for exception
// neutrality (also via the 'bslma_testallocator' component).  Several small
// helper functions are also used to facilitate testing.
//-----------------------------------------------------------------------------
// [ 2] bdlma::ConcurrentMultipool(int numPools, bslma::Allocator *ba = 0);
// [ 8] bdlmca::MultipoolAllocator(numPools, minSize);
// [ 8] bdlmca::MultipoolAllocator(numPools, poolNumObjects);
// [ 8] bdlmca::MultipoolAllocator(numPools, minSize, poolNumObjects);
// [ 8] bdlmca::MultipoolAllocator(numPools, minSize, Z);
// [ 8] bdlmca::MultipoolAllocator(numPools, poolNumObjects, Z);
// [ 8] bdlmca::MultipoolAllocator(numPools, minSize, poolNumObjects, Z);
// [ 2] ~bdlma::ConcurrentMultipool();
// [ 3] void *allocate(bsls::Types::size_type size);
// [ 4] void deallocate(void *address);
// [ 9] void deleteObject(const TYPE *object);
// [ 9] void deleteObjectRaw(const TYPE *object);
// [ 5] void release();
// [ 6] void reserveCapacity(bsls::Types::size_type size, int numObjects);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] CONCURRENCY TEST
// [11] OLD USAGE EXAMPLE
// [12] USAGE EXAMPLE

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
//                       GLOBAL TYPES, CONSTANTS, AND VARIABLES
//-----------------------------------------------------------------------------

typedef bdlma::ConcurrentMultipool Obj;

const int MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

// Warning: keep this in sync with bdlma_multipool.h!
struct Header {
    // Stores pool number of this item.
    union {
        int                                 d_pool;   // pool for this item
        bsls::AlignmentUtil::MaxAlignedType d_dummy;  // force max. alignment
    } d_header;
};

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
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static int calcPool(int numPools, int objSize)
    // Calculate the index of the pool that should allocate objects that are of
    // the specified 'objSize' (in bytes) from a multipool managing the
    // specified 'numPools' number of memory pools.
{
    ASSERT(0 < numPools);
    ASSERT(0 < objSize);

    int poolIndex        = 0;
    int pooledObjectSize = 8;

    while (objSize > pooledObjectSize) {
        pooledObjectSize *= 2;
        ++poolIndex;
    }

    if (poolIndex >= numPools) {
        poolIndex = -1;
    }

    return poolIndex;
}

static int recPool(char *address)
    // Return the index of the pool that allocated the memory at the specified
    // 'address'.
{
    ASSERT(address);

    Header *h = (Header *)address - 1;

    return h->d_header.d_pool;
}

static bsl::intptr_t delta(char *address1, char *address2)
    // Return the number of bytes between the specified 'address1' and the
    // specified 'address2'.
{
    return address1 < address2
        ? static_cast<bsl::intptr_t>(address2 - address1)
        : static_cast<bsl::intptr_t>(address1 - address2);
}

static void scribble(char *address, int size)
    // Assign a non-zero value to each of the specified 'size' bytes starting
    // at the specified 'address'.
{
    memset(address, 0xff, size);
}

void stretchRemoveAll(Obj *object, int numElements, int objSize)
    // Using only primary manipulators, extend the capacity of the specified
    // 'object' to (at least) the specified 'numElements' each of the specified
    // 'objSize' bytes, then remove all elements leaving 'object' empty.  The
    // behavior is undefined unless '0 <= numElements' and '0 <= objSize'.
{
    ASSERT(object);
    ASSERT(0 <= numElements);
    ASSERT(0 <= objSize);

    for (int i = 0; i < numElements; ++i) {
        object->allocate(objSize);
    }
    object->release();
}

enum {
    k_NUM_THREADS = 10
};

struct WorkerArgs {
    Obj       *d_allocator; // allocator to perform allocations
    const int *d_sizes;     // array of allocations sizes
    int        d_numSizes;  // number of allocations

};

bslmt::Barrier g_barrier(k_NUM_THREADS);
extern "C" void *workerThread(void *arg) {
    // Perform a series of allocate, protect, un-protect, and deallocate
    // operations on the 'bcema::TestProtectableMemoryBlockDispenser' and
    // verify their results.  This is operation is intended to be a thread
    // entry point.  Cast the specified 'args' to a 'WorkerArgs', and perform a
    // series of '(WorkerArgs *)args->d_numSizes' allocations using the
    // corresponding allocations sizes specified by
    // '(WorkerARgs *)args->d_sizes'.  Protect, un-protect, and finally delete
    // the allocated memory.  Use the barrier 'g_barrier' to ensure tests are
    // performed while the allocator is in the correct state.

    WorkerArgs *args = (WorkerArgs *) arg;
    ASSERT(0 != args);
    ASSERT(0 != args->d_sizes);

    Obj       *allocator  = args->d_allocator;
    const int *allocSizes = args->d_sizes;
    const int  numAllocs  = args->d_numSizes;

    bsl::vector<char *> blocks(bslma::Default::allocator(0));
    blocks.resize(numAllocs);

    g_barrier.wait();

    // Perform allocations
    for (int i = 0; i < numAllocs; ++i) {
        blocks[i] = (char *)allocator->allocate(allocSizes[i]);
    }

    // deallocate all the blocks
    for (int i = 0; i < numAllocs; ++i) {
        allocator->deallocate(blocks[i]);
    }

    // perform a second set of allocations
    unsigned char threadId =
                  static_cast<unsigned char>(bslmt::ThreadUtil::selfIdAsInt());
    for (int i = 0; i < numAllocs; ++i) {
        blocks[i] = (char *)allocator->allocate(allocSizes[i]);
        memset(blocks[i], threadId, allocSizes[i]);
    }

    g_barrier.wait();

    for (int i = 0; i< numAllocs; ++i) {
        unsigned char *data = (unsigned char *)blocks[i];
        for (int j = 0; j < allocSizes[i]; ++j) {
            ASSERT(threadId == data[j]);
        }
    }

    g_barrier.wait();

    return arg;
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using a 'bdlma::ConcurrentMultipool' Directly
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'bdlma::ConcurrentMultipool' can be used by containers that hold different
// types of elements, each of uniform size, for efficient memory allocation of
// new elements.  Suppose we have a factory class, 'my_MessageFactory', that
// creates messages based on user requests.  Each message is created with the
// most efficient memory storage possible - using predefined 8-byte, 16-byte
// and 32-byte buffers.  If the message size exceeds the three predefined
// values, a generic message is used.  For efficient memory allocation of
// messages, we use a 'bdlma::ConcurrentMultipool'.
//
// First, we define our message types as follows:
//..
    class my_MessageFactory;

    class my_Message {
        // This class represents a general message interface that provides a
        // 'getMessage' method for clients to retrieve the underlying message.

      public:
        // ACCESSORS
        virtual const char *getMessage() = 0;
            // Return the null-terminated message string.
    };

    class my_SmallMessage : public my_Message {
        // This class represents an 8-byte message (including null terminator).

        // DATA
        char d_buffer[8];

        // FRIEND
        friend class my_MessageFactory;

        // NOT IMPLEMENTED
        my_SmallMessage(const my_SmallMessage&);
        my_SmallMessage& operator=(const my_SmallMessage&);

        // PRIVATE CREATORS
        my_SmallMessage(const char *msg, int length)
        {
            ASSERT(length <= 7);

            bsl::memcpy(d_buffer, msg, length);
            d_buffer[length] = '\0';
        }

        // PRIVATE ACCESSORS
        virtual const char *getMessage()
        {
            return d_buffer;
        }
    };

    class my_MediumMessage : public my_Message {
        // This class represents a 16-byte message (including null
        // terminator).

        // DATA
        char d_buffer[16];

        // FRIEND
        friend class my_MessageFactory;

        // NOT IMPLEMENTED
        my_MediumMessage(const my_MediumMessage&);
        my_MediumMessage& operator=(const my_MediumMessage&);

        // PRIVATE CREATORS
        my_MediumMessage(const char *msg, int length)
        {
            ASSERT(length <= 15);

            bsl::memcpy(d_buffer, msg, length);
            d_buffer[length] = '\0';
        }

        // PRIVATE ACCESSORS
        virtual const char *getMessage()
        {
            return d_buffer;
        }
    };

    class my_LargeMessage : public my_Message {
        // This class represents a 32-byte message (including null
        // terminator).

        // DATA
        char d_buffer[32];

        // FRIEND
        friend class my_MessageFactory;

        // NOT IMPLEMENTED
        my_LargeMessage(const my_LargeMessage&);
        my_LargeMessage& operator=(const my_LargeMessage&);

        // PRIVATE CREATORS
        my_LargeMessage(const char *msg, int length)
        {
            ASSERT(length <= 31);

            bsl::memcpy(d_buffer, msg, length);
            d_buffer[length] = '\0';
        }

        // PRIVATE ACCESSORS
        virtual const char *getMessage()
        {
            return d_buffer;
        }
    };

    class my_GenericMessage : public my_Message {
        // This class represents a generic message.

        // DATA
        char *d_buffer;

        // FRIEND
        friend class my_MessageFactory;

        // NOT IMPLEMENTED
        my_GenericMessage(const my_GenericMessage&);
        my_GenericMessage& operator=(const my_GenericMessage&);

        // PRIVATE CREATORS
        my_GenericMessage(char *msg) : d_buffer(msg)
        {
        }

        // PRIVATE ACCESSORS
        virtual const char *getMessage()
        {
            return d_buffer;
        }
    };
//..
// Then we define our factory class, 'my_MessageFactory', as follows:
//..
    class my_MessageFactory {
        // This class implements an efficient message factory that builds and
        // returns messages.  The life-time of the messages created by this
        // factory is the same as this factory.

        // DATA
        bdlma::ConcurrentMultipool d_multipool;  // multipool used to supply
                                                 // memory

      private:
        // Not implemented:
        my_MessageFactory(const my_MessageFactory&);

      public:
        // CREATORS
        my_MessageFactory(bslma::Allocator *basicAllocator = 0);
            // Create a message factory.  Optionally specify a 'basicAllocator'
            // used to supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.

        ~my_MessageFactory();
            // Destroy this factory and reclaim all messages created by it.

        // MANIPULATORS
        my_Message *createMessage(const char *data);
            // Create a message storing the specified 'data'.  The behavior is
            // undefined unless 'data' is null-terminated.

        void disposeAllMessages();
            // Dispose of all created messages.

        void disposeMessage(my_Message *message);
            // Dispose of the specified 'message'.  The behavior is undefined
            // unless 'message' was created by this factory.
    };
//..
// The use of a multipool and the 'release' method enables the
// 'disposeAllMessages' method to quickly deallocate all memory blocks used to
// create messages:
//..
    // MANIPULATORS
    inline
    void my_MessageFactory::disposeAllMessages()
    {
        d_multipool.release();
    }
//..
// The multipool can also reuse deallocated memory.  Once a message is
// destroyed by the 'disposeMessage' method, memory allocated for that message
// is reclaimed by the multipool and can be used to create the next message
// having the same size:
//..
    inline
    void my_MessageFactory::disposeMessage(my_Message *message)
    {
        d_multipool.deleteObject(message);
    }
//..
// A multipool optimizes the allocation of memory by using
// dynamically-allocated buffers (also known as chunks) to supply memory.  As
// each chunk can satisfy multiple memory block requests before requiring
// additional dynamic memory allocation, the number of dynamic allocation
// requests needed is greatly reduced.
//
// For the number of pools managed by the multipool, we chose to use the
// implementation-defined default value instead of calculating and specifying a
// value.  Note that if users want to specify the number of pools, the value
// can be calculated as the smallest 'N' such that the following relationship
// holds:
//..
//  N > log2(sizeof(Object Type)) - 2
//..
// Continuing on with the usage example:
//..
    // CREATORS
    my_MessageFactory::my_MessageFactory(bslma::Allocator *basicAllocator)
    : d_multipool(basicAllocator)
    {
    }
//..
// Note that in the destructor, all outstanding messages are reclaimed
// automatically when 'd_multipool' is destroyed:
//..
    my_MessageFactory::~my_MessageFactory()
    {
    }
//..
// A 'bdlma::ConcurrentMultipool' is ideal for allocating the different sized
// messages since repeated deallocations might be necessary (which renders a
// 'bdlma::SequentialPool' unsuitable) and the sizes of these types are all
// different:
//..
    // MANIPULATORS
    my_Message *my_MessageFactory::createMessage(const char *data)
    {
        enum { k_SMALL = 8, k_MEDIUM = 16, k_LARGE = 32 };

        const int length = static_cast<int>(bsl::strlen(data));

        if (length < k_SMALL) {
            return new(d_multipool.allocate(sizeof(my_SmallMessage)))
                                      my_SmallMessage(data, length);  // RETURN
        }

        if (length < k_MEDIUM) {
            return new(d_multipool.allocate(sizeof(my_MediumMessage)))
                                     my_MediumMessage(data, length);  // RETURN
        }

        if (length < k_LARGE) {
            return new(d_multipool.allocate(sizeof(my_LargeMessage)))
                                      my_LargeMessage(data, length);  // RETURN
        }

        char *buffer = (char *)d_multipool.allocate(length + 1);
        bsl::memcpy(buffer, data, length + 1);

        return new(d_multipool.allocate(sizeof(my_GenericMessage)))
                                                     my_GenericMessage(buffer);
    }
//..
//
///Example 2: Implementing an Allocator Using 'bdlma::ConcurrentMultipool'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bslma::Allocator' is used throughout the interfaces of BDE components.
// Suppose we would like to create a multipool allocator,
// 'my_MultipoolAllocator', that allocates memory from multiple
// 'bdlma::ConcurrentPool' objects in a similar fashion to
// 'bdlma::ConcurrentMultipool'.  This class can be used directly to implement
// such an allocator.
//
// Note that the documentation for this class is simplified for this usage
// example.  Please see 'bdlmca_multipoolallocator' for full documentation of a
// similar class.
//..
    class my_MultipoolAllocator : public bslma::Allocator{
        // This class implements the 'bslma::Allocator' protocol to provide an
        // allocator that manages a set of memory pools, each dispensing memory
        // blocks of a unique size, with each successive pool's block size
        // being twice that of the previous one.

        // DATA
        bdlma::ConcurrentMultipool d_multiPool;  // memory manager for
                                                 // allocated memory blocks

      public:
        // CREATORS
        my_MultipoolAllocator(bslma::Allocator *basicAllocator = 0);
            // Create a multipool allocator.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        // ...

        virtual ~my_MultipoolAllocator();
            // Destroy this multipool allocator.  All memory allocated from
            // this memory pool is released.

        // MANIPULATORS
        virtual void *allocate(bsls::Types::size_type size);
            // Return the address of a contiguous block of maximally-aligned
            // memory of (at least) the specified 'size' (in bytes).  If 'size'
            // is 0, no memory is allocated and 0 is returned.

        virtual void deallocate(void *address);
            // Relinquish the memory block at the specified 'address' back to
            // this multipool allocator for reuse.  The behavior is undefined
            // unless 'address' is non-zero, was allocated by this multipool
            // allocator, and has not already been deallocated.
    };

    // CREATORS
    inline
    my_MultipoolAllocator::my_MultipoolAllocator(
                                              bslma::Allocator *basicAllocator)
    : d_multiPool(basicAllocator)
    {
    }

    my_MultipoolAllocator::~my_MultipoolAllocator()
    {
    }

    // MANIPULATORS
    inline
    void *my_MultipoolAllocator::allocate(bsls::Types::size_type size)
    {
        return d_multiPool.allocate(size);
    }

    inline
    void my_MultipoolAllocator::deallocate(void *address)
    {
        d_multiPool.deallocate(address);
    }
//..

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    bslma::Allocator    *Z = &testAllocator;

    bslma::TestAllocator defaultAllocator("default", veryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    switch (test) { case 0:
      case 12: {
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

        if (verbose) cout << endl << "Testing Usage Example"
                          << endl << "=====================" << endl;
        {
            bslma::TestAllocator ta;
            my_MessageFactory factory(&ta);

            my_Message *msg = factory.createMessage("Hello");
            ASSERT(0 != msg);
        }

        if (verbose) cout << endl << "Testing Old Usage Example"
                          << endl << "=========================" << endl;

        {
            const int MIN_SIZE = 12;

            struct Small {
                char x[MIN_SIZE];
            };

            struct Medium {
                char x[MIN_SIZE * 2];
            };

            struct Large {  // sizeof(Large) == MIN_SIZE * 4
                Small  s1, s2;
                Medium m;
            };

            const int NUM_POOLS = 5;
            bdlma::ConcurrentMultipool mp(NUM_POOLS);

            const int NUM_OBJS = 8;
            int objSize        = 8;
            for (int i = 0; i < NUM_POOLS; ++i) {
                mp.reserveCapacity(objSize, NUM_OBJS);
                objSize *= 2;
            }

            Small  *pS = new(mp.allocate(sizeof(Small)))  Small;    ASSERT(pS);
            Medium *pM = new(mp.allocate(sizeof(Medium))) Medium;   ASSERT(pM);
            Large  *pL = new(mp.allocate(sizeof(Large)))  Large;    ASSERT(pL);

            char *pBuf = (char *) mp.allocate(1024);              ASSERT(pBuf);

            mp.deallocate(pS);
            mp.deallocate(pL);
            // 'pS' and 'pL' are no longer valid addresses.

            mp.release();
            // Now 'pM' and 'pBuf' are also invalid addresses.
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING OLD USAGE EXAMPLE
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

        if (verbose) cout << endl << "Testing OLD Usage Example"
                          << endl << "=========================" << endl;

        {
            const int MIN_SIZE = 12;

            struct Small {
                char x[MIN_SIZE];
            };

            struct Medium {
                char x[MIN_SIZE * 2];
            };

            struct Large {  // sizeof(Large) == MIN_SIZE * 4
                Small  s1, s2;
                Medium m;
            };

            const int NUM_POOLS = 4;
            bdlma::ConcurrentMultipool mp(NUM_POOLS);

            const int NUM_OBJS = 8;
            int objSize        = MIN_SIZE;
            for (int i = 0; i < NUM_POOLS - 1; ++i) {
                mp.reserveCapacity(objSize, NUM_OBJS);
                objSize *= 2;
            }

            Small  *pS = new(mp.allocate(sizeof(Small)))  Small;    ASSERT(pS);
            Medium *pM = new(mp.allocate(sizeof(Medium))) Medium;   ASSERT(pM);
            Large  *pL = new(mp.allocate(sizeof(Large)))  Large;    ASSERT(pL);

            char *pBuf = (char *) mp.allocate(1024);              ASSERT(pBuf);

            mp.deallocate(pS);
            mp.deallocate(pL);
            // 'pS' and 'pL' are no longer valid addresses.

            mp.release();
            // Now 'pM' and 'pBuf' are also invalid addresses.
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // ALLOCATOR ACCESSOR TEST
        //
        // Concerns:
        //: 1. 'allocator()' accessor returns the expected value.
        //:
        //: 2. 'allocator()' accessor is declared const.
        //
        // Plan:
        //: 1 To test 'allocator', create object with various allocators and
        //:   ensure the returned value matches the supplied allocator.  (C-1)
        //:
        //: 2 Directly test that 'allocator()', invoked on a 'const' object,
        //:   returns the expected value.  (C-1..2)
        //
        // Testing:
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "ALLOCATOR ACCESSOR TEST" << endl
                                  << "=======================" << endl;

        if (verbose) cout << "\nTesting 'allocator'." << endl;
        {
            Obj mX;  const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
        }
        {
            Obj mX(reinterpret_cast<bslma::TestAllocator *>(0));

            const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
        }
        {
            bslma::TestAllocator sa("supplied", veryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            ASSERT(&sa == X.allocator());
        }
      } break;
      case 9: {
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

        bool finished = false;
        const MostDerived *repeater = 0;    // verify we're re-using the memory
                                            // each time
        Obj mp(6, Z);
        for (int di = 0; !finished; ++di) {
            MostDerived *pMD = (MostDerived *) mp.allocate(sizeof(*pMD));
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
                mp.deleteObjectRaw(pMDC);
              } break;
              case 1: {
                const LeftChild *pLCC = pMDC;
                ASSERT((const void*) pLCC == (const void*) pMDC);
                mp.deleteObjectRaw(pLCC);
              } break;
              case 2: {
                mp.deleteObject(pMDC);
              } break;
              case 3: {
                const LeftChild *pLCC = pMDC;
                ASSERT((const void*) pLCC == (const void*) pMDC);
                mp.deleteObject(pLCC);
              } break;
              case 4: {
                const RightChild *pRCC = pMDC;
                ASSERT((const void*) pRCC != (const void*) pMDC);
                mp.deleteObject(pRCC);
              } break;
              case 5: {
                mp.deleteObjectRaw(pMDC);    // 2nd time we do this

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
      case 8: {
        // --------------------------------------------------------------------
        // TESTING ALL C'TORS
        //
        // Concerns:
        //   1. That all arguments passed to the multipool constructors has
        //      their proper effect.
        //
        //   2. That default growth strategy is geometric growth.
        //
        //   3. That default max blocks per chunk is 32.
        //
        // Plan:
        //   First, create different arrays of test data for growth strategy,
        //   max blocks per chunk and allocation sizes.  Then, create an array
        //   of 'bdlma::Pool', each initialized with the corresponding growth
        //   strategy, max blocks per chunk and managing memory blocks doubling
        //   in size - just as what is managed within 'bdema::Multipool'.  Then
        //   create a multipool.  Allocate different object sizes from the
        //   multipool, and test that the growth of the internal chunks within
        //   the multipool is the same as the appropriate 'bdlma::Pool',
        //   determined by the 'calcPool' method.
        //
        //   To test the default arguments (the different constructors), the
        //   we repeat the test above, but initialize the 'bdlma::Pool' with
        //   the expected default argument.
        //
        // Testing:
        //   bdema::ConcurrentMultipool(bslma::Allocator *ba = 0);
        //   bdema::ConcurrentMultipool(int numPools, ba = 0);
        //   bdema::ConcurrentMultipool(Strategy gs, bslma::Allocator *ba = 0);
        //   bdema::ConcurrentMultipool(int n, Strategy gs, ba = 0);
        //   bdema::ConcurrentMultipool(int n, const Strategy *gsa, ba = 0);
        //   bdema::ConcurrentMultipool(int n, int mbpc, ba= 0);
        //   bdema::ConcurrentMultipool(int n, const int *mbpc, ba = 0);
        //   bdema::ConcurrentMultipool(int n, gs, int mbpc, ba = 0);
        //   bdema::ConcurrentMultipool(int n, gsa, int mbpc, ba = 0);
        //   bdema::ConcurrentMultipool(int n, gs, const int *ma, ba = 0);
        //   bdema::ConcurrentMultipool(int n, gsa, const int *ma, ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'constructor' and 'destructor'"
                          << endl << "======================================"
                          << endl;

        enum { k_INITIAL_CHUNK_SIZE = 1,
               k_DEFAULT_MAX_CHUNK_SIZE = 32,
               k_DEFAULT_NUM_POOLS = 10 };

        // For pool allocation.
        char buffer[1024];
        bdlma::BufferedSequentialAllocator bsa(buffer, 1024);

        const int NUM_POOLS = 5;
        const int TEST_MAX_CHUNK_SIZE = 30;

        // Object Data
        const int ODATA[]   = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };
        const int NUM_ODATA = sizeof ODATA / sizeof *ODATA;

        typedef bsls::BlockGrowth::Strategy St;
        const bsls::BlockGrowth::Strategy GEO =
                                             bsls::BlockGrowth::BSLS_GEOMETRIC;
        const bsls::BlockGrowth::Strategy CON =
                                             bsls::BlockGrowth::BSLS_CONSTANT;

        // Strategy Data
        const St SDATA[][5]  = { {GEO, GEO, GEO, GEO, GEO},
                                 {CON, CON, CON, CON, CON},
                                 {GEO, CON, GEO, CON, GEO},
                                 {CON, GEO, CON, GEO, CON} };
        const int NUM_SDATA = sizeof SDATA / sizeof *SDATA;

        // MaxBlocksPerChunk Data
        const int MDATA[][5] = { { 32, 32, 32, 32, 32 },
                                 { 30, 30, 30, 30, 30 },
                                 { 30, 32, 30, 32, 30 },
                                 { 32, 30, 32, 30, 32 } };
        const int NUM_MDATA = sizeof MDATA / sizeof *MDATA;

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int n, const S *gsa, "
                    "const int *ma, A *ba = 0)" << endl;
        }
        for (int si = 0; si < NUM_SDATA; ++si) {
            for (int mi = 0; mi < NUM_MDATA; ++mi) {

                bslma::TestAllocator pta("pool test allocator",
                                         veryVeryVerbose);
                bslma::TestAllocator mpta("multipool test allocator",
                                          veryVeryVerbose);

                // Initialize the pools
                bdlma::ConcurrentPool *pool[NUM_POOLS];
                int INIT = 8;
                for (int j = 0; j < NUM_POOLS; ++j) {
                    pool[j] = new(bsa)bdlma::ConcurrentPool(INIT,
                                                 SDATA[si][j],
                                                 MDATA[mi][j],
                                                 &pta);
                    INIT <<= 1;
                }

                // Create the multipool
                Obj mp(NUM_POOLS, SDATA[si], MDATA[mi], &mpta);

                // Allocate until we depleted the pool
                bsls::Types::Int64 poolAllocations      = pta.numAllocations();
                bsls::Types::Int64 multipoolAllocations =
                                                         mpta.numAllocations();

                // multipool should have an extra allocation for the array of
                // pools.
                LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                             poolAllocations + 1 == multipoolAllocations);

                for (int oi = 0; oi < NUM_ODATA; ++oi) {
                    const int OBJ_SIZE = ODATA[oi];
                    const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                    if (-1 == calcPoolNum) {
                        char *p = (char *) mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                        continue;
                    }

                    LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                    // Testing geometric growth
                    if (GEO == SDATA[si][calcPoolNum]) {
                        int ri = k_INITIAL_CHUNK_SIZE;
                        while (ri < MDATA[mi][calcPoolNum]) {
                            poolAllocations      = pta.numAllocations();
                            multipoolAllocations = mpta.numAllocations();

                            for (int j = 0; j < ri; ++j) {
                                char *p = (char *)mp.allocate(OBJ_SIZE);
                                const int recordPoolNum = recPool(p);

                                LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                             recordPoolNum == calcPoolNum);

                                pool[calcPoolNum]->allocate();
                            }

                            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                         poolAllocations + 1
                                                      == multipoolAllocations);

                            ri <<= 1;
                        }
                    }

                    // Testing constant growth (also applies to capped
                    // geometric growth).
                    const int NUM_REPLENISH = 3;
                    for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                        poolAllocations      = pta.numAllocations();
                        multipoolAllocations = mpta.numAllocations();

                        for (int j = 0; j < MDATA[mi][calcPoolNum]; ++j) {
                            char *p = (char *)mp.allocate(OBJ_SIZE);
                            const int recordPoolNum = recPool(p);

                            LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                             recordPoolNum == calcPoolNum);

                            pool[calcPoolNum]->allocate();
                        }

                        LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                     poolAllocations + 1
                                                      == multipoolAllocations);
                    }
                }

                // Release all pooled memory.
                for (int j = 0; j < NUM_POOLS; ++j) {
                    pool[j]->release();
                }
            }
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(bslma::Allocator *ba = 0)"
                 << endl;
        }

        {
            bslma::TestAllocator pta("pool test allocator",
                                     veryVeryVerbose);
            bslma::TestAllocator mpta("multipool test allocator",
                                      veryVeryVerbose);

            // Initialize the pools
            bdlma::ConcurrentPool *pool[k_DEFAULT_NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < k_DEFAULT_NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::ConcurrentPool(INIT,
                                             GEO,
                                             k_DEFAULT_MAX_CHUNK_SIZE,
                                             &pta);
                INIT <<= 1;
            }

            // Create the multipool
            Obj mp(&mpta);

            // Allocate until we depleted the pool
            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(k_DEFAULT_NUM_POOLS,
                                                 OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < k_DEFAULT_NUM_POOLS);

                // Testing geometric growth
                int ri = k_INITIAL_CHUNK_SIZE;
                while (ri < k_DEFAULT_MAX_CHUNK_SIZE) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < ri; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1
                                              == multipoolAllocations);

                    ri <<= 1;
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1
                                                  == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < k_DEFAULT_NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int numPools,"
                 << " bslma::Allocator *ba = 0)"
                 << endl;
        }
        {
            bslma::TestAllocator pta("pool test allocator",
                                     veryVeryVerbose);
            bslma::TestAllocator mpta("multipool test allocator",
                                      veryVeryVerbose);

            // Initialize the pools
            bdlma::ConcurrentPool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::ConcurrentPool(INIT,
                                             GEO,
                                             k_DEFAULT_MAX_CHUNK_SIZE,
                                             &pta);
                INIT <<= 1;
            }

            // Create the multipool
            Obj mp(NUM_POOLS, &mpta);

            // Allocate until we depleted the pool
            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS,
                                                 OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing geometric growth
                int ri = k_INITIAL_CHUNK_SIZE;
                while (ri < k_DEFAULT_MAX_CHUNK_SIZE) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < ri; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1
                                              == multipoolAllocations);

                    ri <<= 1;
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1
                                                  == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(Strategy gs,"
                 << " bslma::Allocator *ba = 0)"
                 << endl;
        }
        {
            bslma::TestAllocator pta("pool test allocator",
                                     veryVeryVerbose);
            bslma::TestAllocator mpta("multipool test allocator",
                                      veryVeryVerbose);

            // Initialize the pools
            bdlma::ConcurrentPool *pool[k_DEFAULT_NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < k_DEFAULT_NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::ConcurrentPool(INIT,
                                             CON,
                                             k_DEFAULT_MAX_CHUNK_SIZE,
                                             &pta);
                INIT <<= 1;
            }

            // Create the multipool
            Obj mp(CON, &mpta);

            // Allocate until we depleted the pool
            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(k_DEFAULT_NUM_POOLS,
                                                 OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < k_DEFAULT_NUM_POOLS);

                // Testing constant growth
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1
                                                  == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < k_DEFAULT_NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int n, Strategy gs,"
                 << " bslma::Allocator *ba = 0)"
                 << endl;
        }
        {
            bslma::TestAllocator pta("pool test allocator",
                                     veryVeryVerbose);
            bslma::TestAllocator mpta("multipool test allocator",
                                      veryVeryVerbose);

            // Initialize the pools
            bdlma::ConcurrentPool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::ConcurrentPool(INIT,
                                             CON,
                                             k_DEFAULT_MAX_CHUNK_SIZE,
                                             &pta);
                INIT <<= 1;
            }

            // Create the multipool
            Obj mp(NUM_POOLS, CON, &mpta);

            // Allocate until we depleted the pool
            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS,
                                                 OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing constant growth
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1
                                                  == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int n, const Strategy *gsa,"
                 << " Allocator *ba = 0)"
                 << endl;
        }
        for (int si = 0; si < NUM_SDATA; ++si) {
            bslma::TestAllocator pta("pool test allocator",
                                     veryVeryVerbose);
            bslma::TestAllocator mpta("multipool test allocator",
                                      veryVeryVerbose);

            // Initialize the pools
            bdlma::ConcurrentPool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::ConcurrentPool(INIT,
                                             SDATA[si][j],
                                             k_DEFAULT_MAX_CHUNK_SIZE,
                                             &pta);
                INIT <<= 1;
            }

            // Create the multipool
            Obj mp(NUM_POOLS, SDATA[si], &mpta);

            // Allocate until we depleted the pool
            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing geometric growth
                if (GEO == SDATA[si][calcPoolNum]) {
                    int ri = k_INITIAL_CHUNK_SIZE;
                    while (ri < k_DEFAULT_MAX_CHUNK_SIZE) {
                        poolAllocations      = pta.numAllocations();
                        multipoolAllocations = mpta.numAllocations();

                        for (int j = 0; j < ri; ++j) {
                            char *p = (char *)mp.allocate(OBJ_SIZE);
                            const int recordPoolNum = recPool(p);

                            LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                            pool[calcPoolNum]->allocate();
                        }

                        LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                     poolAllocations + 1
                                                  == multipoolAllocations);

                        ri <<= 1;
                    }
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1
                                                  == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int n, int mbpc,"
                 << " bslma::Allocator *ba= 0)"
                 << endl;
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int n, const int *mbpc,"
                 << " bslma::Allocator *ba = 0)"
                 << endl;
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int n, Strategy gs,"
                 << " int mbpc, Allocator *ba = 0)"
                 << endl;
        }
        {
            bslma::TestAllocator pta("pool test allocator",
                                     veryVeryVerbose);
            bslma::TestAllocator mpta("multipool test allocator",
                                      veryVeryVerbose);

            // Initialize the pools
            bdlma::ConcurrentPool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::ConcurrentPool(INIT,
                                             CON,
                                             TEST_MAX_CHUNK_SIZE,
                                             &pta);
                INIT <<= 1;
            }

            // Create the multipool
            Obj mp(NUM_POOLS, CON, TEST_MAX_CHUNK_SIZE, &mpta);

            // Allocate until we depleted the pool
            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS,
                                                 OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing constant growth
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < TEST_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1
                                                  == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int n, const S *gsa,"
                 << " int mbpc, Alloc *ba = 0)"
                 << endl;
        }
        for (int si = 0; si < NUM_SDATA; ++si) {
            bslma::TestAllocator pta("pool test allocator",
                                     veryVeryVerbose);
            bslma::TestAllocator mpta("multipool test allocator",
                                      veryVeryVerbose);

            // Initialize the pools
            bdlma::ConcurrentPool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::ConcurrentPool(INIT,
                                             SDATA[si][j],
                                             TEST_MAX_CHUNK_SIZE,
                                             &pta);
                INIT <<= 1;
            }

            // Create the multipool
            Obj mp(NUM_POOLS, SDATA[si], TEST_MAX_CHUNK_SIZE, &mpta);

            // Allocate until we depleted the pool
            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing geometric growth
                if (GEO == SDATA[si][calcPoolNum]) {
                    int ri = k_INITIAL_CHUNK_SIZE;
                    while (ri < TEST_MAX_CHUNK_SIZE) {
                        poolAllocations      = pta.numAllocations();
                        multipoolAllocations = mpta.numAllocations();

                        for (int j = 0; j < ri; ++j) {
                            char *p = (char *)mp.allocate(OBJ_SIZE);
                            const int recordPoolNum = recPool(p);

                            LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                            pool[calcPoolNum]->allocate();
                        }

                        LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                     poolAllocations + 1
                                                  == multipoolAllocations);

                        ri <<= 1;
                    }
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < TEST_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1
                                                  == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int n, Strat gs,"
                 << " const int *ma, Alloc "
                 << " *ba = 0)"
                 << endl;
        }
        for (int mi = 0; mi < NUM_MDATA; ++mi) {

            bslma::TestAllocator pta("pool test allocator",
                                     veryVeryVerbose);
            bslma::TestAllocator mpta("multipool test allocator",
                                      veryVeryVerbose);

            // Initialize the pools
            bdlma::ConcurrentPool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::ConcurrentPool(INIT,
                                             CON,
                                             MDATA[mi][j],
                                             &pta);
                INIT <<= 1;
            }

            // Create the multipool
            Obj mp(NUM_POOLS, CON, MDATA[mi], &mpta);

            // Allocate until we depleted the pool
            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < MDATA[mi][calcPoolNum]; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1
                                                  == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST
        //
        // Testing:
        //     Thread-safety of allocate/deallocate methods.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST CONCURRENCY" << endl
                                  << "================" << endl;
        bslmt::ThreadUtil::Handle threads[k_NUM_THREADS];

        bslma::TestAllocator ta;
        Obj                  mX(4, &ta);

        const int SIZES [] = { 1 , 2 , 4,  8, 16, 32, 64, 128, 256, 512,
                               1 , 2 , 4,  8, 16, 32, 64, 128, 256, 512};

        const int NUM_SIZES = sizeof (SIZES) / sizeof(*SIZES);

        WorkerArgs args;
        args.d_allocator = &mX;
        args.d_sizes     = (const int *)&SIZES;
        args.d_numSizes  = NUM_SIZES;

        for (int i = 0; i < k_NUM_THREADS; ++i) {
            int rc =
                bslmt::ThreadUtil::create(&threads[i], workerThread, &args);
            LOOP_ASSERT(i, 0 == rc);
        }
        for (int i = 0; i < k_NUM_THREADS; ++i) {
            int rc =
                bslmt::ThreadUtil::join(threads[i]);
            LOOP_ASSERT(i, 0 == rc);
        }
        mX.release();
        ASSERT(1 == ta.numBlocksInUse());
        ASSERT(1 < ta.numBlocksMax());
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING RESERVECAPACITY METHOD
        //
        // Concerns:
        //   Our primary concern is that 'reserveCapacity(sz, n)' reserves
        //   sufficient memory to satisfy 'n' allocation requests from the
        //   pool managing objects of size 'sz'.
        //
        // Plan:
        //   To test 'reserveCapacity', specify a table of capacities to
        //   reserve.  Construct an object managing three pools and call
        //   'reserveCapacity' for each of the three pools with the tabulated
        //   number of elements.  Allocate as many objects as required to
        //   bring the size of the pool under test to the specified number of
        //   elements and use 'bslma::TestAllocator' to verify that no
        //   additional allocations have occurred.  Perform each test in the
        //   standard 'bdema' exception-testing macro block.
        //
        // Testing:
        //   void reserveCapacity(bsls::Types::size_type size, int numObjects);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'reserveCapacity'"
                          << endl << "=========================" << endl;

        {
            const int DATA[] = {
                0, 1, 2, 3, 4, 5, 15, 16, 17
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            static const int EXTEND[] = {
                0, 1, 4, 5, 7, 17, 23, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            static const int OSIZE[] = {  // enough for 3 pools
                MAX_ALIGN, MAX_ALIGN * 2, MAX_ALIGN * 4
            };
            const int NUM_OSIZE = sizeof OSIZE / sizeof *OSIZE;

            const int NUM_POOLS = 4;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int NE = DATA[ti];
                if (veryVerbose) { cout << "\t\t"; P(NE); }

                for (int zi = 0; zi < NUM_OSIZE; ++zi) {
                    const int OBJ_SIZE = OSIZE[zi];
                    for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                      BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        Obj mX(NUM_POOLS, Z);
                        stretchRemoveAll(&mX, EXTEND[ei], OBJ_SIZE);
                        mX.reserveCapacity(OBJ_SIZE, 0);
                        mX.reserveCapacity(OBJ_SIZE, NE);
                        const bsls::Types::Int64 NUM_BLOCKS =
                                                testAllocator.numBlocksTotal();
                        const bsls::Types::Int64 NUM_BYTES  =
                                                 testAllocator.numBytesInUse();
                        for (int i = 0; i < NE; ++i) {
                            mX.allocate(OBJ_SIZE);
                        }
                        LOOP3_ASSERT(ti, ti, zi, NUM_BLOCKS ==
                                               testAllocator.numBlocksTotal());
                        LOOP3_ASSERT(ti, ti, zi, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                  }
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING RELEASE METHOD
        //
        // Concerns:
        //   Our primary concern is that 'release' relinquishes all
        //   outstanding memory to the allocator that is in use by the
        //   multipool.
        //
        // Plan:
        //   Create multipools that manage a varying number of pools and
        //   make many allocation requests from each of the pools
        //   (including the "overflow" pool).  As a simple sanity test,
        //   scribble over the extent of all allocated objects.  This
        //   ensures that no portion of the object is used by the multipool
        //   for bookkeeping.  Make use of the facilities available in
        //   'bslma::TestAllocator' to monitor memory usage.  Verify with
        //   appropriate assertions that all memory is indeed relinquished
        //   to the memory allocator following each 'release'.
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'release'"
                          << endl << "=================" << endl;

        const int MAX_POOLS     = 3;
        const int POOL_QUANTA[] = { MAX_ALIGN, MAX_ALIGN * 2, MAX_ALIGN * 4 };
        const int OVERFLOW_SIZE = MAX_ALIGN * 5;
        const int NITERS        = MAX_ALIGN * 256;

        for (int i = 1; i <= MAX_POOLS; ++i) {
            if (veryVerbose) { T_; cout << "# pools: "; P(i); }
            Obj mX(i, Z);
            const bsls::Types::Int64 NUM_BLOCKS =
                                                testAllocator.numBlocksInUse();
            const bsls::Types::Int64 NUM_BYTES  =
                                                 testAllocator.numBytesInUse();
            int its              = NITERS;
            while (its-- > 0) {  // exercise each pool (including "overflow")
                char *p;
                for (int j = 0; j < i; ++j) {
                    const int OBJ_SIZE = POOL_QUANTA[j];
                    p = (char *) mX.allocate(OBJ_SIZE);  // garbage-collected
                                                         //  by 'release'
                    LOOP3_ASSERT(i, j, its, p);
                    scribble(p, OBJ_SIZE);
                }

                p = (char *) mX.allocate(OVERFLOW_SIZE);
                LOOP2_ASSERT(i, its, p);
                scribble(p, OVERFLOW_SIZE);

                mX.release();
                LOOP2_ASSERT(i, its, NUM_BLOCKS ==
                                               testAllocator.numBlocksInUse());
                LOOP2_ASSERT(i, its, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING DEALLOCATE METHOD
        //
        // Concerns:
        //   Our primary concern is that 'deallocate' returns the block of
        //   memory to the underlying pool making it available for future
        //   allocation.
        //
        // Plan:
        //   Create multipools that manage a varying number of pools and
        //   make many allocation requests from each of the pools
        //   (including the "overflow" pool).  As a simple sanity test,
        //   scribble over the extent of all allocated objects.  This
        //   ensures that no portion of the object is used by the multipool
        //   for bookkeeping.  Make use of the facilities available in
        //   'bslma::TestAllocator' to monitor memory usage.  Verify with
        //   appropriate assertions that no demands are put on the memory
        //   allocation beyond those attributable to start-up.
        //
        // Testing:
        //   void deallocate(void *address);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'deallocate'"
                          << endl << "====================" << endl;

        const int MAX_POOLS     = 3;
        const int POOL_QUANTA[] = { MAX_ALIGN, MAX_ALIGN * 2, MAX_ALIGN * 4 };
        const int OVERFLOW_SIZE = MAX_ALIGN * 5;
        const int NITERS        = MAX_ALIGN * 256;

        bsls::Types::Int64 numBlocks;
        bsls::Types::Int64 numBytes;

        for (int i = 1; i <= MAX_POOLS; ++i) {
            if (veryVerbose) { T_; cout << "# pools: "; P(i); }
            Obj mX(i + 1, Z);
            for (int j = 0; j < i; ++j) {
                if (veryVerbose) { T_; T_; cout << "pool: "; P(j); }
                const int OBJ_SIZE = POOL_QUANTA[j];
                int its            = NITERS;
                int firstTime      = 1;
                while (its-- > 0) {  // exercise each pool
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    LOOP2_ASSERT(i, j, p);
                    if (firstTime) {
                        numBlocks = testAllocator.numBlocksTotal();
                        numBytes  = testAllocator.numBytesInUse();
                        firstTime = 0;
                    }
                    else {
                        LOOP2_ASSERT(i, j, numBlocks ==
                                               testAllocator.numBlocksTotal());
                        LOOP2_ASSERT(i, j, numBytes  ==
                                                testAllocator.numBytesInUse());
                    }
                    scribble(p, OBJ_SIZE);
                    mX.deallocate(p);
                }
            }

            if (veryVerbose) { T_; T_; cout << "overflow\n"; }
            int its       = NITERS;
            int firstTime = 1;
            while (its-- > 0) {  // exercise "overflow" pool
                char *p = (char *) mX.allocate(OVERFLOW_SIZE);
                LOOP2_ASSERT(i, its, p);
                if (firstTime) {
                    numBlocks = testAllocator.numBlocksInUse();
                    numBytes  = testAllocator.numBytesInUse();
                    firstTime = 0;
                }
                else {
                    LOOP2_ASSERT(i, its, numBlocks ==
                                               testAllocator.numBlocksInUse());
                    LOOP2_ASSERT(i, its, numBytes ==
                                                testAllocator.numBytesInUse());
                }
                scribble(p, OVERFLOW_SIZE);
                mX.deallocate(p);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATE METHOD
        //
        // Concerns:
        //   We have the following concerns:
        //    1) the memory returned by 'allocate' is properly aligned.
        //    2) the memory returned by 'allocate' is of sufficient size.
        //    3) the allocation request is distributed to the proper underlying
        //       pool (including the "overflow" pool).
        //
        // Plan:
        //   Create multipools that manage a varying number of pools and
        //   allocate objects of varying sizes from each of the pools
        //   (including the "overflow" pool).  As a simple sanity test,
        //   scribble over the extent of all allocated objects.
        //
        //   Concern 1 is addressed with a simple "%" calculation on the value
        //   returned by 'allocate'.  Concern 2 cannot be verified with 100%
        //   certainty, but a necessary condition can be tested, namely that
        //   the difference between the memory addresses returned by two
        //   successive allocation requests is at least as great as an object's
        //   size.  Scribbling over the object also has bearing on concern 2.
        //   Concern 3 is addressed by comparing the calculated pool index of
        //   an object with the pool index that is recorded by the multipool
        //   when the object is allocated.  Note that concerns 2 and 3 entail
        //   some white-box testing.
        //
        // Testing:
        //   void *allocate(bsls::Types::size_type size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'allocate'"
                          << endl << "==================" << endl;

        // number of pools to manage
        const int PDATA[]   = { 1, 2, 3, 4, 5 };
        const int NUM_PDATA = sizeof PDATA / sizeof *PDATA;

        // sizes of objects to allocate
        const int ODATA[]   = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };
        const int NUM_ODATA = sizeof ODATA / sizeof *ODATA;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            for (int i = 0; i < NUM_PDATA; ++i) {
                const int NUM_POOLS = PDATA[i];
                if (veryVerbose) { P(NUM_POOLS); }

                Obj mX(NUM_POOLS);

                for (int j = 0; j < NUM_ODATA; ++j) {

                     for (int k = -1; k <= 1; ++k) {

                        const int OBJ_SIZE = ODATA[j] + k;
                        if (0 == OBJ_SIZE) {
                            continue;
                        }
                        if (veryVerbose) { T_ P(OBJ_SIZE); }

                        char *p = (char *) mX.allocate(OBJ_SIZE);

                        LOOP3_ASSERT(i, j, k, p);
                        LOOP3_ASSERT(i, j, k,
                               0 == bsls::Types::UintPtr(p) % MAX_ALIGN);

                        scribble(p, OBJ_SIZE);
                        const int pCalculatedPool =
                                                 calcPool(NUM_POOLS, OBJ_SIZE);
                        const int pRecordedPool   = recPool(p);
                        if (veryVerbose) {
                            T_ P_((void *)p);
                            P_(pCalculatedPool); P(pRecordedPool);
                        }
                        LOOP3_ASSERT(i, j, k, pCalculatedPool ==
                                                                pRecordedPool);

                        char *q = (char *) mX.allocate(OBJ_SIZE);
                        LOOP3_ASSERT(i, j, k, q);
                        LOOP3_ASSERT(i, j, k,
                               0 == bsls::Types::UintPtr(q) % MAX_ALIGN);

                        scribble(q, OBJ_SIZE);
                        const int qCalculatedPool =
                                                 calcPool(NUM_POOLS, OBJ_SIZE);
                        const int qRecordedPool   = recPool(q);
                        if (veryVerbose) {
                            T_ P_((void *)q);
                            P_(qCalculatedPool); P(qRecordedPool);
                        }
                        LOOP3_ASSERT(i, j, k, qCalculatedPool ==
                                                                qRecordedPool);

                        LOOP3_ASSERT(i, j, k, pRecordedPool   ==
                                                                qRecordedPool);
                        LOOP3_ASSERT(i, j, k, OBJ_SIZE + (int)sizeof(Header) <=
                                                                  delta(p, q));

                        mX.deallocate(p);
                        mX.deallocate(q);
                    }
                }
            }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;
        {
            for (int i = 0; i < NUM_PDATA; ++i) {
                const int NUM_POOLS = PDATA[i];
                if (veryVerbose) { P(NUM_POOLS); }

                bslma::TestAllocator ta;
                Obj mX(NUM_POOLS, &ta);

                for (int j = 0; j < NUM_ODATA; ++j) {

                     for (int k = -1; k <= 1; ++k) {
                        const int OBJ_SIZE = ODATA[j] + k;
                        if (0 == OBJ_SIZE) {
                            continue;
                        }
                        if (veryVerbose) { T_ P(OBJ_SIZE); }

                        char *p = (char *) mX.allocate(OBJ_SIZE);

                        LOOP3_ASSERT(i, j, k, p);
                        LOOP3_ASSERT(i, j, k,
                               0 == bsls::Types::UintPtr(p) % MAX_ALIGN);

                        scribble(p, OBJ_SIZE);
                        const int pCalculatedPool =
                                                 calcPool(NUM_POOLS, OBJ_SIZE);
                        const int pRecordedPool   = recPool(p);
                        if (veryVerbose) {
                            T_ P_((void *)p);
                            P_(pCalculatedPool); P(pRecordedPool);
                        }
                        LOOP3_ASSERT(i, j, k, pCalculatedPool ==
                                                                pRecordedPool);

                        char *q = (char *) mX.allocate(OBJ_SIZE);
                        LOOP3_ASSERT(i, j, k, q);
                        LOOP3_ASSERT(i, j, k,
                               0 == bsls::Types::UintPtr(q) % MAX_ALIGN);

                        scribble(q, OBJ_SIZE);
                        const int qCalculatedPool =
                                                 calcPool(NUM_POOLS, OBJ_SIZE);
                        const int qRecordedPool   = recPool(q);
                        if (veryVerbose) {
                            T_ P_((void *)q);
                            P_(qCalculatedPool); P(qRecordedPool);
                        }
                        LOOP3_ASSERT(i, j, k, qCalculatedPool ==
                                                                qRecordedPool);

                        LOOP3_ASSERT(i, j, k, pRecordedPool   ==
                                                                qRecordedPool);
                        LOOP3_ASSERT(i, j, k, OBJ_SIZE + (int)sizeof(Header) <=
                                                                  delta(p, q));

                        mX.deallocate(p);
                        mX.deallocate(q);

                    }
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING C'TOR AND D'TOR
        //
        // Concerns:
        //   We have the following concerns:
        //    1) The 'bdlma::ConcurrentMultipool' constructor works properly:
        //       a. The constructor is exception neutral w.r.t. memory
        //          allocation.
        //       b. The internal memory management system is hooked up properly
        //          so that *all* internally allocated memory draws from a
        //          user-supplied allocator whenever one is specified.
        //       c. Objects may be allocated from the pools managed by the
        //          multipool.
        //    2) The destructor works properly as implicitly tested in the
        //       various scopes of this test and in the presence of exceptions.
        //   Note this test only tests one of the c'tors.
        //
        // Plan:
        //   Create a test object using the constructor: 1) without
        //   exceptions and 2) in the presence of exceptions during memory
        //   allocations using a 'bslma::TestAllocator' and varying its
        //   *allocation* *limit*.  When the object goes out of scope, verify
        //   that the destructor properly deallocates all memory that had been
        //   allocated to it.
        //
        //   Concern 2 is addressed by making use of the 'allocate' method
        //   (which is thoroughly tested in case 3).
        //
        //   At the end, include a constructor test specifying a static buffer
        //   allocator.  This tests for rudimentary correct object behavior
        //   via the destructor and Purify.
        //
        // Testing:
        //   bdlma::ConcurrentMultipool(int numPools, ba = 0);
        //   ~bdlma::ConcurrentMultipool();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'constructor' and 'destructor'"
                          << endl << "======================================"
                          << endl;

        // number of pools to manage
        const int PDATA[]   = { 1, 2, 3, 4, 5 };
        const int NUM_PDATA = sizeof PDATA / sizeof *PDATA;

        // sizes of objects to allocate
        const int ODATA[]   = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };
        const int NUM_ODATA = sizeof ODATA / sizeof *ODATA;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            for (int i = 0; i < NUM_PDATA; ++i) {
                const int NUM_POOLS = PDATA[i];
                if (veryVerbose) { P(NUM_POOLS); }
                for (int j = 0; j < NUM_ODATA; ++j) {
                    Obj mX(NUM_POOLS);
                    const int OBJ_SIZE = ODATA[j];
                    if (veryVerbose) { T_ P(OBJ_SIZE); }
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    LOOP2_ASSERT(i, j, p);
                    const int pCalculatedPool = calcPool(NUM_POOLS, OBJ_SIZE);
                    const int pRecordedPool   = recPool(p);
                    if (veryVerbose) {
                        T_ P_((void *)p);
                        P_(pCalculatedPool); P(pRecordedPool);
                    }
                    LOOP2_ASSERT(i, j, pCalculatedPool == pRecordedPool);
                }

                {
                    Obj mX(NUM_POOLS);
                    char *p = (char *) mX.allocate(2048);  // "overflow" pool
                    LOOP_ASSERT(i, p);
                    const int pRecordedPool = recPool(p);
                    if (veryVerbose) { T_ P_((void *)p); P(pRecordedPool); }
                    LOOP_ASSERT(i, -1 == pRecordedPool);
                }
            }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;
        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            for (int i = 0; i < NUM_PDATA; ++i) {
                const int NUM_POOLS = PDATA[i];
                if (veryVerbose) { P(NUM_POOLS); }
                const bsls::Types::Int64 NUM_BLOCKS =
                                                testAllocator.numBlocksInUse();
                const bsls::Types::Int64 NUM_BYTES  =
                                                 testAllocator.numBytesInUse();
                for (int j = 0; j < NUM_ODATA; ++j) {
                    {
                        Obj mX(NUM_POOLS, Z);
                        const int OBJ_SIZE = ODATA[j];
                        if (veryVerbose) { T_ P(OBJ_SIZE); }
                        char *p = (char *) mX.allocate(OBJ_SIZE);
                        LOOP2_ASSERT(i, j, p);
                        const int pCalculatedPool =
                                                 calcPool(NUM_POOLS, OBJ_SIZE);
                        const int pRecordedPool   = recPool(p);
                        if (veryVerbose) {
                            T_ P_((void *)p);
                            P_(pCalculatedPool); P(pRecordedPool);
                        }
                        LOOP2_ASSERT(i, j, pCalculatedPool == pRecordedPool);
                    }
                    LOOP2_ASSERT(i, j, NUM_BLOCKS ==
                                               testAllocator.numBlocksInUse());
                    LOOP2_ASSERT(i, j, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
                }

                {
                    Obj mX(NUM_POOLS, Z);
                    char *p = (char *) mX.allocate(2048);  // "overflow" pool
                    LOOP_ASSERT(i, p);
                    const int pRecordedPool = recPool(p);
                    if (veryVerbose) { T_ P_((void *)p); P(pRecordedPool); }
                    LOOP_ASSERT(i, -1 == pRecordedPool);
                }
                LOOP_ASSERT(i, NUM_BLOCKS == testAllocator.numBlocksInUse());
                LOOP_ASSERT(i, NUM_BYTES  == testAllocator.numBytesInUse());
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            for (int i = 0; i < NUM_PDATA; ++i) {
                const int NUM_POOLS = PDATA[i];
                if (veryVerbose) { P(NUM_POOLS); }
                const bsls::Types::Int64 NUM_BLOCKS =
                                                testAllocator.numBlocksInUse();
                const bsls::Types::Int64 NUM_BYTES  =
                                                 testAllocator.numBytesInUse();
                for (int j = 0; j < NUM_ODATA; ++j) {
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mX(NUM_POOLS, Z);
                    const int OBJ_SIZE = ODATA[j];
                    if (veryVerbose) { T_ P(OBJ_SIZE); }
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    LOOP2_ASSERT(i, j, p);
                    const int pCalculatedPool = calcPool(NUM_POOLS, OBJ_SIZE);
                    const int pRecordedPool   = recPool(p);
                    if (veryVerbose) {
                        T_ P_((void *)p);
                        P_(pCalculatedPool); P(pRecordedPool);
                    }
                    LOOP2_ASSERT(i, j, pCalculatedPool == pRecordedPool);
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                  LOOP2_ASSERT(i, j, NUM_BLOCKS ==
                                               testAllocator.numBlocksInUse());
                  LOOP2_ASSERT(i, j, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                  Obj mX(NUM_POOLS, Z);
                  char *p = (char *) mX.allocate(2048);  // "overflow" pool
                  LOOP_ASSERT(i, p);
                  const int pRecordedPool = recPool(p);
                  if (veryVerbose) { T_ P_((void *)p); P(pRecordedPool); }
                  LOOP_ASSERT(i, -1 == pRecordedPool);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                LOOP_ASSERT(i, NUM_BLOCKS == testAllocator.numBlocksInUse());
                LOOP_ASSERT(i, NUM_BYTES  == testAllocator.numBytesInUse());
            }
        }

        if (verbose)
            cout << "\tWith a buffer allocator (exercise only)." << endl;
        {
            char memory[4096];
            bdlma::BufferedSequentialAllocator a(memory, sizeof memory);
            Obj *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(1, &a);
            char *p = (char *) doNotDelete->allocate(2048);  // "overflow" pool
            ASSERT(p);
            // No destructor is called; will produce memory leak in purify if
            // internal allocators are not hooked up properly.
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We are concerned that the basic functionality of
        //   'bdlma::ConcurrentMultipool' works properly.
        //
        // Plan:
        //   Create a multipool that manages two pools using the lone
        //   constructor.  Allocate memory from each of the pools as well as
        //   from the "overflow" pool.  Then 'deallocate' or 'release' the
        //   allocated objects.  Finally let the multipool go out of scope
        //   to exercise the destructor.
        //
        // Testing:
        //   This "test" exercises basic functionality, but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Breathing Test"
                          << endl << "==============" << endl;

        {
            char *p, *q, *r;

            // 1.
            if (verbose)
                cout << "1. Create a multipool that manages three pools."
                     << endl;

            Obj mX(3);

            // 2.
            if (verbose)
                cout << "2. Allocate an object from the small object pool."
                     << endl;

            p = (char *) mX.allocate(MAX_ALIGN);          ASSERT(p);

            // 3.
            if (verbose) cout << "3. Deallocate the small object." << endl;

            mX.deallocate(p);

            // Address 'p' is no longer valid.

             // 4.
            if (verbose)
                cout << "4. Reserve capacity in the medium object pool for "
                        "at least two objects." << endl;

            mX.reserveCapacity(MAX_ALIGN * 2, 2);

             // 5.
            if (verbose)
                cout << "5. Allocate two objects from the medium object pool."
                     << endl;

            p = (char *) mX.allocate(MAX_ALIGN * 2);      ASSERT(p);
            q = (char *) mX.allocate(MAX_ALIGN * 2 - 1);  ASSERT(q);

             // 6.
            if (verbose)
                cout << "6. Allocate an object from the \"overflow\" (large) "
                        "object pool." << endl;

            r = (char *) mX.allocate(1024);               ASSERT(r);

             // 7.
            if (verbose)
                cout << "7. Deallocate all outstanding objects." << endl;

            mX.release();

            // Addresses 'p', 'q', 'r' are no longer valid.

             // 8.
            if (verbose)
                cout << "8. Let the multipool go out of scope." << endl;
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

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
