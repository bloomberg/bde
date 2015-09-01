// bdlma_multipool.t.cpp                                              -*-C++-*-
#include <bdlma_multipool.h>

#include <bdlma_bufferedsequentialallocator.h>   // for testing only

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// A 'bdlma::Multipool' is a mechanism (i.e., having state but no value) that
// is used as a memory manager to manage an array of 'bdlma::Pool' objects.
// Each of the internal pools of a 'bdlma::Multipool' has a distinct block
// size, and each pool may also be configured to have a distinct growth
// strategy and/or a maximum blocks per chunk.
//
// Primary testing concerns are: 1) that the constructors configure the
// individual internal pools as expected, and 2) that the manipulators and
// overloaded 'operator new' operate on the correct internal pool (or all of
// the pools in the case of the 'release' method).  Since this component is a
// memory manager, the 'bslma_testallocator' component is used extensively to
// verify expected behavior.  Note that memory allocation must be tested for
// exception neutrality (also via the 'bslma_testallocator' component).
// Several small helper functions are also used to facilitate testing.
//-----------------------------------------------------------------------------
// [ 7] bdlma::Multipool(Allocator *ba = 0);
// [ 2] bdlma::Multipool(numPools, Allocator *ba = 0);
// [ 7] bdlma::Multipool(gs, Allocator *ba = 0);
// [ 7] bdlma::Multipool(numPools, gs, Allocator *ba = 0);
// [ 7] bdlma::Multipool(numPools, gs, mbpc, Allocator *ba = 0);
// [ 7] bdlma::Multipool(numPools, *gs, Allocator *ba = 0);
// [ 7] bdlma::Multipool(numPools, *gs, mbpc, Allocator *ba = 0);
// [ 7] bdlma::Multipool(numPools, gs, *mbpc, Allocator *ba = 0);
// [ 7] bdlma::Multipool(numPools, *gs, *mbpc, Allocator *ba = 0);
// [ 2] ~bdlma::Multipool();
// [ 3] void *allocate(int size);
// [ 4] void deallocate(void *address);
// [ 8] template <class TYPE> void deleteObject(const TYPE *object);
// [ 8] template <class TYPE> void deleteObjectRaw(const TYPE *object);
// [ 5] void release();
// [ 6] void reserveCapacity(int size, int numBlocks);
// [ 9] int numPools() const;
// [ 9] int maxPooledBlockSize() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE
// [ *] CONCERN: Precondition violations are detected when enabled.

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

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                       GLOBAL TYPES AND CONSTANTS
//-----------------------------------------------------------------------------

typedef bdlma::Multipool            Obj;

typedef bsls::BlockGrowth::Strategy Strategy;

const int MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

// Warning: keep this in sync with bdlma_multipool.h!
struct Header {
    // Stores pool number of this item.
    union {
        int                                 d_pool;   // pool for this item
        bsls::AlignmentUtil::MaxAlignedType d_dummy;  // force max. alignment
    } d_header;
};

int numLeftChildren  = 0;
int numRightChildren = 0;
int numMostDerived   = 0;

struct LeftChild {
    int d_li;

    LeftChild()          { ++numLeftChildren; }
    virtual ~LeftChild() { --numLeftChildren; }
};

struct RightChild {
    int d_ri;

    RightChild()          { ++numRightChildren; }
    virtual ~RightChild() { --numRightChildren; }
};

struct MostDerived : LeftChild, RightChild {
    int d_md;

    MostDerived()  { ++numMostDerived; }
    ~MostDerived() { --numMostDerived; }
};

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static
int calcPool(int numPools, int objSize)
    // Return the index of the pool that should allocate objects that are of
    // the specified 'objSize' bytes in size from a multipool managing the
    // specified 'numPools' memory pools, or -1 if 'objSize' exceeds the size
    // of the blocks managed by all of the pools.  The behavior is undefined
    // unless '0 < numPools' and '0 < objSize'.
{
    ASSERT(0 < numPools);
    ASSERT(0 < objSize);

    int pool     = 0;
    int poolSize = 8;

    while (objSize > poolSize) {
        poolSize *= 2;
        ++pool;
    }

    if (pool >= numPools) {
        pool = -1;
    }

    return pool;
}

static inline
int recPool(char *address)
    // Return the index of the pool that allocated the memory at the specified
    // 'address', or -1 if the memory was allocated directly from the
    // underlying allocator.  The behavior is undefined unless 'address' is
    // non-null.
{
    ASSERT(address);

    Header *h = (Header *)address - 1;

    return h->d_header.d_pool;
}

inline
int delta(char *address1, char *address2)
    // Return the number of bytes between the specified 'address1' and the
    // specified 'address2'.
{
    return address1 < address2
         ? static_cast<int>(address2 - address1)
         : static_cast<int>(address1 - address2);
}

static inline
void scribble(char *address, int size)
    // Assign a non-zero value to each of the specified 'size' bytes starting
    // at the specified 'address'.
{
    memset(address, 0xff, size);
}

void stretchRemoveAll(Obj *object, int numElements, int objSize)
    // Using only primary manipulators, extend the capacity of the specified
    // 'object' to (at least) the specified 'numElements', each of the
    // specified 'objSize' bytes, then remove all elements leaving 'object'
    // empty.  The behavior is undefined unless '0 <= numElements' and
    // '0 <= objSize'.
{
    ASSERT(object);
    ASSERT(0 <= numElements);
    ASSERT(0 <= objSize);

    for (int i = 0; i < numElements; ++i) {
        object->allocate(objSize);
    }
    object->release();
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using a 'bdlma::Multipool' Directly
///- - - - - - - - - - - - - - - - - - - - - - -
// A 'bdlma::Multipool' can be used by containers that hold different types of
// elements, each of uniform size, for efficient memory allocation of new
// elements.  Suppose we have a factory class, 'my_MessageFactory', that
// creates messages based on user requests.  Each message is created with the
// most efficient memory storage possible, using predefined 8-byte, 16-byte,
// and 32-byte buffers.  If the message size exceeds the three predefined
// values, a generic message is used.  For efficient memory allocation of
// messages, we use a 'bdlma::Multipool'.
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

      private:
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

      private:
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

      private:
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

      private:
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
// Then, we define our factory class, 'my_MessageFactory', as follows:
//..
    class my_MessageFactory {
        // This class implements an efficient message factory that builds and
        // returns messages.  The life-time of the messages created by this
        // factory is the same as this factory.

        // DATA
        bdlma::Multipool d_multipool;  // multipool used to supply memory

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
            // unless 'message' was created by this factory, and has not
            // already been disposed.
    };
//..
// Next, we define the 'inline' methods of 'my_MessageFactory'.
//
// In calling the multipool's 'release' method, 'disposeAllMessages' quickly
// deallocates all memory blocks that were used to create messages currently
// outstanding from the factory.  Following the call to 'release', *all* memory
// that had been allocated from the multipool is available for reuse:
//..
    // MANIPULATORS
    inline
    void my_MessageFactory::disposeAllMessages()
    {
        d_multipool.release();
    }
//..
// Similarly, the call to the multipool's 'deleteObject' method in
// 'disposeMessage' first destroys the message, then releases the memory that
// had been allocated for it back to the multipool for use in creating another
// message having the same size:
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
// value.  If users instead want to specify the number of pools, the value can
// be calculated as the smallest value, 'N', such that the following
// relationship holds:
//..
//  N > log2(sizeof(Object Type)) - 2
//..
// Next, we define the creators of 'my_MessageFactory':
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
// Finally, we define the 'createMessage' factory method that actually creates
// the messages using memory provided by the multipool.  A 'bdlma::Multipool'
// is ideal for allocating the different sized messages since repeated
// deallocations might be necessary, which renders a 'bdlma::SequentialPool'
// unsuitable:
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
///Example 2: Implementing an Allocator Using 'bdlma::Multipool'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to create a multipool allocator (i.e., that implements
// the 'bslma::Allocator' interface) that allocates memory from multiple
// 'bdlma::Pool' objects in a similar fashion to 'bdlma::Multipool'.  In this
// example, we create just such a multipool allocator, 'my_MultipoolAllocator',
// that uses a 'bdlma::Multipool' to manage the multiple pools.
//
// First, we define the interface of 'my_MultipoolAllocator':
//..
    class my_MultipoolAllocator : public bslma::Allocator {
        // This class implements the 'bslma::Allocator' protocol to provide an
        // allocator that manages a set of memory pools, each dispensing memory
        // blocks of a unique size, with each successive pool's block size
        // being twice that of the previous one.

        // DATA
        bdlma::Multipool d_multiPool;  // memory manager for allocated memory
                                       // blocks

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
        virtual void *allocate(size_type size);
            // Return the address of a contiguous block of maximally-aligned
            // memory of (at least) the specified 'size' (in bytes).  If 'size'
            // is 0, no memory is allocated and 0 is returned.

        virtual void deallocate(void *address);
            // Relinquish the memory block at the specified 'address' back to
            // this multipool allocator for reuse.  The behavior is undefined
            // unless 'address' is non-zero, was allocated by this multipool
            // allocator, and has not already been deallocated.
    };
//..
// Note that the interface and documentation for this class is simplified for
// this usage example.  Please see 'bdlma_multipoolallocator' for a similar
// class meant for production use.
//
// Finally, we provide the trivial implementation of 'my_MultipoolAllocator':
//..
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
    void *my_MultipoolAllocator::allocate(size_type size)
    {
        if (0 == size) {
            return 0;                                                 // RETURN
        }
        else {
            return d_multiPool.allocate(static_cast<int>(size));      // RETURN
        }
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

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator(veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    bslma::Allocator     *Z = &testAllocator;

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        {
            bslma::TestAllocator ta;
            my_MessageFactory factory(&ta);

            my_Message *msg = factory.createMessage("Hello");       // len < 8
            factory.disposeMessage(msg);

            msg = factory.createMessage("Hello, again");            // len > 8
            factory.disposeMessage(msg);

            msg = factory.createMessage("Hello, again and again");  // len > 16
            factory.disposeMessage(msg);
        }

        {
            bslma::TestAllocator ta;
            my_MultipoolAllocator multipoolAllocator(&ta);
            bslma::Allocator *allocator = &multipoolAllocator;

            void *address = allocator->allocate(16);
            allocator->deallocate(address);

            address = allocator->allocate(731);
            allocator->deallocate(address);
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'numPools' and 'maxPooledBlockSize'
        //
        // Concerns:
        //   1) That 'numPools' returns the number of pools managed by the
        //      multipool.
        //
        //   2) That 'maxPooledBlockSize' returns the correct maximum block
        //      size managed by the multipool given the specified 'numPools'.
        //
        // Plan:
        //   Since the constructors are thoroughly tested at this point, simply
        //   construct a multipool passing in different 'numPools' arguments
        //   specified in a test array, and verify that 'numPools' and
        //   'maxPooledBlockSize' return the expected values.
        //
        // Testing:
        //   int numPools() const;
        //   int maxPooledBlockSize() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                      << "TESTING 'numPools' and 'maxPooledBlockSize'" << endl
                      << "===========================================" << endl;

        static const struct {
            int d_lineNum;       // line number
            int d_numPools;      // number of pools
            int d_maxBlockSize;  // maximum block size
        } DATA[] = {
            //LINE  # POOLS     MAXIMUM BLOCK SIZE
            //----  --------    ------------------
            { L_,   1,          8                  },
            { L_,   2,          16                 },
            { L_,   3,          32                 },
            { L_,   4,          64                 },
            { L_,   5,          128                },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE         = DATA[i].d_lineNum;
            const int NUMPOOLS     = DATA[i].d_numPools;
            const int MAXBLOCKSIZE = DATA[i].d_maxBlockSize;

            if (veryVerbose) {
                P_(LINE) P_(NUMPOOLS) P(MAXBLOCKSIZE)
            }

            Obj mX(NUMPOOLS, &testAllocator);  const Obj& X = mX;

            LOOP2_ASSERT(NUMPOOLS, X.numPools(),
                         NUMPOOLS == X.numPools());

            LOOP2_ASSERT(MAXBLOCKSIZE, X.maxPooledBlockSize(),
                         MAXBLOCKSIZE == X.maxPooledBlockSize());
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'deleteObject' AND 'deleteObjectRaw'
        //
        // Concerns:
        //   That 'deleteObject' and 'deleteObjectRaw' properly destroy and
        //   deallocate managed objects.
        //
        // Plan:
        //   Iterate, where at the beginning of the loop we create an object
        //   of type 'MostDerived' that multiply inherits from two types with
        //   virtual destructors.  Then in the middle of the loop we switch on
        //   the various ways to destroy and deallocate the object with various
        //   forms of 'deleteObjectRaw' and 'deleteObject', after which we
        //   verify that the destructors have been run.  In each iteration we
        //   verify that the memory we got was the same as the previous
        //   iteration, showing that memory is being deallocated and recovered
        //   by the pool.
        //
        // Testing:
        //   template <class TYPE> void deleteObject(const TYPE *object);
        //   template <class TYPE> void deleteObjectRaw(const TYPE *object);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                     << "TESTING 'deleteObject' and 'deleteObjectRaw'" << endl
                     << "============================================" << endl;

        bool finished = false;
        const MostDerived *lastAddress = 0;  // Used to verify that we are
                                             // re-using the same memory each
                                             // time through the loop.
        Obj mX(6, Z);
        for (int di = 0; !finished; ++di) {
            MostDerived *pMD = (MostDerived *)mX.allocate(sizeof(*pMD));
            const MostDerived *pMDC = pMD;
            new (pMD) MostDerived();

            if (!lastAddress) {
                lastAddress = pMDC;
            }
            else {
                // Verify that we are freeing the memory in each iteration.
                // The same address should be returned each time we allocate.
                LOOP_ASSERT(di, lastAddress == pMDC);
            }

            ASSERT(1 == numLeftChildren);
            ASSERT(1 == numRightChildren);
            ASSERT(1 == numMostDerived);

            switch (di) {
              case 0: {
                mX.deleteObjectRaw(pMDC);
              } break;
              case 1: {
                const LeftChild *pLCC = pMDC;
                ASSERT((const void*) pLCC == (const void*) pMDC);
                mX.deleteObjectRaw(pLCC);
              } break;
              case 2: {
                mX.deleteObject(pMDC);
              } break;
              case 3: {
                const LeftChild *pLCC = pMDC;
                ASSERT((const void*) pLCC == (const void*) pMDC);
                mX.deleteObject(pLCC);
              } break;
              case 4: {
                const RightChild *pRCC = pMDC;
                ASSERT((const void*) pRCC != (const void*) pMDC);
                mX.deleteObject(pRCC);
              } break;
              case 5: {
                mX.deleteObjectRaw(pMDC);  // 2nd time we do this
                finished = true;
              } break;
              default: {
                ASSERT(0);
              }
            }

            LOOP_ASSERT(di, 0 == numLeftChildren);
            LOOP_ASSERT(di, 0 == numRightChildren);
            LOOP_ASSERT(di, 0 == numMostDerived);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ALL CTORS
        //
        // Concerns:
        //   1. That all arguments passed to the multipool constructors have
        //      the expected effect.
        //
        //   2. That the default growth strategy is geometric growth.
        //
        //   3. That the default maximum blocks per chunk is 32.
        //
        //   QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //   First, create different arrays of test data for growth strategy,
        //   max blocks per chunk, and allocation sizes.  Then, create an array
        //   of 'bdlma::Pool', each initialized with the corresponding growth
        //   strategy, max blocks per chunk, and managing memory blocks
        //   doubling in size - just as what is managed within
        //   'bdlma::Multipool'.  Then create a multipool.  Allocate different
        //   object sizes from the multipool, and test that the growth of the
        //   internal chunks within the multipool is the same as the
        //   appropriate 'bdlma::Pool', determined by the 'calcPool' method.
        //
        //   To test the default arguments (the different constructors), we
        //   repeat the test above, but initialize the 'bdlma::Pool' with the
        //   expected default argument.
        //
        // Testing:
        //   bdlma::Multipool(Allocator *ba = 0);
        //   bdlma::Multipool(gs, Allocator *ba = 0);
        //   bdlma::Multipool(numPools, gs, Allocator *ba = 0);
        //   bdlma::Multipool(numPools, gs, mbpc, Allocator *ba = 0);
        //   bdlma::Multipool(numPools, *gs, Allocator *ba = 0);
        //   bdlma::Multipool(numPools, *gs, mbpc, Allocator *ba = 0);
        //   bdlma::Multipool(numPools, gs, *mbpc, Allocator *ba = 0);
        //   bdlma::Multipool(numPools, *gs, *mbpc, Allocator *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING ALL CTORS" << endl
                                  << "=================" << endl;

        enum {
            k_INITIALCHUNK_SIZE     =  1,
            k_DEFAULT_MAX_CHUNK_SIZE = 32,
            k_DEFAULT_NUM_POOLS      = 10
        };

        // For pool allocation.
        char buffer[1024];
        bdlma::BufferedSequentialAllocator bsa(buffer, 1024);

        const int NUM_POOLS           =  5;
        const int TEST_MAX_CHUNK_SIZE = 30;

        // Object Data
        const int ODATA[]   = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };
        const int NUM_ODATA = sizeof ODATA / sizeof *ODATA;

        const Strategy GEO = bsls::BlockGrowth::BSLS_GEOMETRIC;
        const Strategy CON = bsls::BlockGrowth::BSLS_CONSTANT;

        // Strategy Data
        const Strategy SDATA[][5] = {
            { GEO, GEO, GEO, GEO, GEO },
            { CON, CON, CON, CON, CON },
            { GEO, CON, GEO, CON, GEO },
            { CON, GEO, CON, GEO, CON }
        };
        const int NUM_SDATA = sizeof SDATA / sizeof *SDATA;

        // MaxBlocksPerChunk Data
        const int MDATA[][5] = {
            { 32, 32, 32, 32, 32 },
            { 30, 30, 30, 30, 30 },
            { 30, 32, 30, 32, 30 },
            { 32, 30, 32, 30, 32 }
        };
        const int NUM_MDATA = sizeof MDATA / sizeof *MDATA;

        if (verbose) {
            cout << "'bdlma::Multipool(numPools, *gs, *mbpc, *ba)'" << endl;
        }
        for (int si = 0; si < NUM_SDATA; ++si) {
            for (int mi = 0; mi < NUM_MDATA; ++mi) {

                bslma::TestAllocator pta("pool",   veryVeryVerbose);
                bslma::TestAllocator  oa("object", veryVeryVerbose);

                // Initialize the pools.
                bdlma::Pool *pool[NUM_POOLS];
                int INIT = 8;
                for (int j = 0; j < NUM_POOLS; ++j) {
                    pool[j] = new(bsa)bdlma::Pool(INIT,
                                                  SDATA[si][j],
                                                  MDATA[mi][j],
                                                  &pta);
                    INIT <<= 1;
                }

                // Create the multipool.
                Obj mX(NUM_POOLS, SDATA[si], MDATA[mi], &oa);

                bsls::Types::Int64 poolAllocations      = pta.numAllocations();
                bsls::Types::Int64 multipoolAllocations =  oa.numAllocations();

                // The multipool should have an extra allocation for the array
                // of pools.
                LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                             poolAllocations + 1 == multipoolAllocations);

                for (int oi = 0; oi < NUM_ODATA; ++oi) {
                    const int OBJ_SIZE    = ODATA[oi];
                    const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                    if (-1 == calcPoolNum) {
                        char *p = (char *) mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                        continue;
                    }

                    LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                    // Testing geometric growth.
                    if (GEO == SDATA[si][calcPoolNum]) {
                        int ri = k_INITIALCHUNK_SIZE;
                        while (ri < MDATA[mi][calcPoolNum]) {
                            poolAllocations      = pta.numAllocations();
                            multipoolAllocations =  oa.numAllocations();

                            for (int j = 0; j < ri; ++j) {
                                char *p = (char *)mX.allocate(OBJ_SIZE);
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
                        multipoolAllocations =  oa.numAllocations();

                        for (int j = 0; j < MDATA[mi][calcPoolNum]; ++j) {
                            char *p = (char *)mX.allocate(OBJ_SIZE);
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
            cout << "'bdlma::Multipool(*ba)'" << endl;
        }

        {
            bslma::TestAllocator pta("pool",   veryVeryVerbose);
            bslma::TestAllocator  oa("object", veryVeryVerbose);

            // Initialize the pools.
            bdlma::Pool *pool[k_DEFAULT_NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < k_DEFAULT_NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::Pool(INIT,
                                              GEO,
                                              k_DEFAULT_MAX_CHUNK_SIZE,
                                              &pta);
                INIT <<= 1;
            }

            // Create the multipool.
            Obj mX(&oa);

            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations =  oa.numAllocations();

            // The multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(k_DEFAULT_NUM_POOLS,
                                                 OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < k_DEFAULT_NUM_POOLS);

                // Testing geometric growth.
                int ri = k_INITIALCHUNK_SIZE;
                while (ri < k_DEFAULT_MAX_CHUNK_SIZE) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations =  oa.numAllocations();

                    for (int j = 0; j < ri; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1 == multipoolAllocations);

                    ri <<= 1;
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations =  oa.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1 == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < k_DEFAULT_NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "'bdlma::Multipool(numPools, *ba)'" << endl;
        }
        {
            bslma::TestAllocator pta("pool",   veryVeryVerbose);
            bslma::TestAllocator  oa("object", veryVeryVerbose);

            // Initialize the pools.
            bdlma::Pool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::Pool(INIT,
                                              GEO,
                                              k_DEFAULT_MAX_CHUNK_SIZE,
                                              &pta);
                INIT <<= 1;
            }

            // Create the multipool.
            Obj mX(NUM_POOLS, &oa);

            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations =  oa.numAllocations();

            // The multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing geometric growth.
                int ri = k_INITIALCHUNK_SIZE;
                while (ri < k_DEFAULT_MAX_CHUNK_SIZE) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations =  oa.numAllocations();

                    for (int j = 0; j < ri; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1 == multipoolAllocations);

                    ri <<= 1;
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations =  oa.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1 == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "'bdlma::Multipool(gs, *ba)'" << endl;
        }
        {
            bslma::TestAllocator pta("pool",   veryVeryVerbose);
            bslma::TestAllocator  oa("object", veryVeryVerbose);

            // Initialize the pools.
            bdlma::Pool *pool[k_DEFAULT_NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < k_DEFAULT_NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::Pool(INIT,
                                              CON,
                                              k_DEFAULT_MAX_CHUNK_SIZE,
                                              &pta);
                INIT <<= 1;
            }

            // Create the multipool.
            Obj mX(CON, &oa);

            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations =  oa.numAllocations();

            // The multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(k_DEFAULT_NUM_POOLS,
                                                 OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < k_DEFAULT_NUM_POOLS);

                // Testing constant growth.
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations =  oa.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1 == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < k_DEFAULT_NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "'bdlma::Multipool(numPools, gs, *ba)'" << endl;
        }
        {
            bslma::TestAllocator pta("pool",   veryVeryVerbose);
            bslma::TestAllocator  oa("object", veryVeryVerbose);

            // Initialize the pools.
            bdlma::Pool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::Pool(INIT,
                                              CON,
                                              k_DEFAULT_MAX_CHUNK_SIZE,
                                              &pta);
                INIT <<= 1;
            }

            // Create the multipool.
            Obj mX(NUM_POOLS, CON, &oa);

            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations =  oa.numAllocations();

            // The multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing constant growth.
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations =  oa.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1 == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "'bdlma::Multipool(numPools, *gs, *ba)'" << endl;
        }
        for (int si = 0; si < NUM_SDATA; ++si) {
            bslma::TestAllocator pta("pool",   veryVeryVerbose);
            bslma::TestAllocator  oa("object", veryVeryVerbose);

            // Initialize the pools.
            bdlma::Pool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::Pool(INIT,
                                              SDATA[si][j],
                                              k_DEFAULT_MAX_CHUNK_SIZE,
                                              &pta);
                INIT <<= 1;
            }

            // Create the multipool.
            Obj mX(NUM_POOLS, SDATA[si], &oa);

            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations =  oa.numAllocations();

            // The multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing geometric growth.
                if (GEO == SDATA[si][calcPoolNum]) {
                    int ri = k_INITIALCHUNK_SIZE;
                    while (ri < k_DEFAULT_MAX_CHUNK_SIZE) {
                        poolAllocations      = pta.numAllocations();
                        multipoolAllocations =  oa.numAllocations();

                        for (int j = 0; j < ri; ++j) {
                            char *p = (char *)mX.allocate(OBJ_SIZE);
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
                    multipoolAllocations =  oa.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1 == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "'bdlma::Multipool(numPools, gs, mbpc, *ba)'" << endl;
        }
        {
            bslma::TestAllocator pta("pool",   veryVeryVerbose);
            bslma::TestAllocator  oa("object", veryVeryVerbose);

            // Initialize the pools.
            bdlma::Pool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::Pool(INIT,
                                              GEO,
                                              TEST_MAX_CHUNK_SIZE,
                                              &pta);
                INIT <<= 1;
            }

            // Create the multipool.
            Obj mX(NUM_POOLS, GEO, TEST_MAX_CHUNK_SIZE, &oa);

            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations =  oa.numAllocations();

            // The multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing geometric growth.
                int ri = k_INITIALCHUNK_SIZE;
                while (ri < TEST_MAX_CHUNK_SIZE) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations =  oa.numAllocations();

                    for (int j = 0; j < ri; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1 == multipoolAllocations);

                    ri <<= 1;
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations =  oa.numAllocations();

                    for (int j = 0; j < TEST_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1 == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "'bdlma::Multipool(numPools, gs, *mbpc, *ba)'" << endl;
        }
        for (int mi = 0; mi < NUM_MDATA; ++mi) {

            bslma::TestAllocator pta("pool",   veryVeryVerbose);
            bslma::TestAllocator  oa("object", veryVeryVerbose);

            // Initialize the pools.
            bdlma::Pool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::Pool(INIT,
                                              GEO,
                                              MDATA[mi][j],
                                              &pta);
                INIT <<= 1;
            }

            // Create the multipool.
            Obj mX(NUM_POOLS, GEO, MDATA[mi], &oa);

            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations =  oa.numAllocations();

            // The multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing geometric growth.
                int ri = k_INITIALCHUNK_SIZE;
                while (ri < MDATA[mi][calcPoolNum]) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations =  oa.numAllocations();

                    for (int j = 0; j < ri; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1 == multipoolAllocations);

                    ri <<= 1;
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations =  oa.numAllocations();

                    for (int j = 0; j < MDATA[mi][calcPoolNum]; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1 == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "'bdlma::Multipool(numPools, gs, mbpc, *ba)'" << endl;
        }
        {
            bslma::TestAllocator pta("pool",   veryVeryVerbose);
            bslma::TestAllocator  oa("object", veryVeryVerbose);

            // Initialize the pools.
            bdlma::Pool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::Pool(INIT,
                                              CON,
                                              TEST_MAX_CHUNK_SIZE,
                                              &pta);
                INIT <<= 1;
            }

            // Create the multipool.
            Obj mX(NUM_POOLS, CON, TEST_MAX_CHUNK_SIZE, &oa);

            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations =  oa.numAllocations();

            // The multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing constant growth.
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations =  oa.numAllocations();

                    for (int j = 0; j < TEST_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1 == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "'bdlma::Multipool(numPools, *gs, mbpc, *ba)'" << endl;
        }
        for (int si = 0; si < NUM_SDATA; ++si) {
            bslma::TestAllocator pta("pool",   veryVeryVerbose);
            bslma::TestAllocator  oa("object", veryVeryVerbose);

            // Initialize the pools.
            bdlma::Pool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::Pool(INIT,
                                              SDATA[si][j],
                                              TEST_MAX_CHUNK_SIZE,
                                              &pta);
                INIT <<= 1;
            }

            // Create the multipool.
            Obj mX(NUM_POOLS, SDATA[si], TEST_MAX_CHUNK_SIZE, &oa);

            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations =  oa.numAllocations();

            // The multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing geometric growth.
                if (GEO == SDATA[si][calcPoolNum]) {
                    int ri = k_INITIALCHUNK_SIZE;
                    while (ri < TEST_MAX_CHUNK_SIZE) {
                        poolAllocations      = pta.numAllocations();
                        multipoolAllocations =  oa.numAllocations();

                        for (int j = 0; j < ri; ++j) {
                            char *p = (char *)mX.allocate(OBJ_SIZE);
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
                    multipoolAllocations =  oa.numAllocations();

                    for (int j = 0; j < TEST_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1 == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "'bdlma::Multipool(numPools, gs, *mbpc, *ba)'" << endl;
        }
        for (int mi = 0; mi < NUM_MDATA; ++mi) {

            bslma::TestAllocator pta("pool",   veryVeryVerbose);
            bslma::TestAllocator  oa("object", veryVeryVerbose);

            // Initialize the pools.
            bdlma::Pool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdlma::Pool(INIT,
                                              CON,
                                              MDATA[mi][j],
                                              &pta);
                INIT <<= 1;
            }

            // Create the multipool.
            Obj mX(NUM_POOLS, CON, MDATA[mi], &oa);

            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations =  oa.numAllocations();

            // The multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
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
                    multipoolAllocations =  oa.numAllocations();

                    for (int j = 0; j < MDATA[mi][calcPoolNum]; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool[calcPoolNum]->allocate();
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations + 1 == multipoolAllocations);
                }
            }

            // Release all pooled memory.
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'Multipool(numPools, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1));

                ASSERT_SAFE_FAIL(Obj( 0));
                ASSERT_SAFE_FAIL(Obj(-1));
            }

            if (veryVerbose) cout << "\t'Multipool(numPools, gs, *ba)'"
                                  << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1, CON));

                ASSERT_SAFE_FAIL(Obj( 0, CON));
                ASSERT_SAFE_FAIL(Obj(-1, CON));
            }

            if (veryVerbose) cout << "\t'Multipool(numPools, gs, mbpc, *ba)'"
                                  << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1, CON,  1));

                ASSERT_SAFE_FAIL(Obj( 0, CON,  1));
                ASSERT_SAFE_FAIL(Obj(-1, CON,  1));

                ASSERT_SAFE_FAIL(Obj( 1, CON,  0));
                ASSERT_SAFE_FAIL(Obj( 1, CON, -1));
            }

            if (veryVerbose) cout << "\t'Multipool(numPools, *gs, *ba)'"
                                  << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1, SDATA[0]));

                ASSERT_SAFE_FAIL(Obj( 0, SDATA[0]));
                ASSERT_SAFE_FAIL(Obj(-1, SDATA[0]));

                ASSERT_SAFE_FAIL(Obj( 1, (Strategy *)0));
            }

            if (veryVerbose) cout << "\t'Multipool(numPools, *gs, mbpc, *ba)'"
                                  << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1, SDATA[0],       1));

                ASSERT_SAFE_FAIL(Obj( 0, SDATA[0],       1));
                ASSERT_SAFE_FAIL(Obj(-1, SDATA[0],       1));

                ASSERT_SAFE_FAIL(Obj( 1, (Strategy *)0,  1));

                ASSERT_SAFE_FAIL(Obj( 1, SDATA[0],       0));
                ASSERT_SAFE_FAIL(Obj( 1, SDATA[0],      -1));
            }

            if (veryVerbose) cout << "\t'Multipool(numPools, gs, *mbpc, *ba)'"
                                  << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1, CON, MDATA[0]));

                ASSERT_SAFE_FAIL(Obj( 0, CON, MDATA[0]));
                ASSERT_SAFE_FAIL(Obj(-1, CON, MDATA[0]));

                ASSERT_SAFE_FAIL(Obj( 1, CON, (int *)0));
            }

            if (veryVerbose) cout << "\t'Multipool(numPools, *gs, *mbpc, *ba)'"
                                  << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1, SDATA[0],      MDATA[0]));

                ASSERT_SAFE_FAIL(Obj( 0, SDATA[0],      MDATA[0]));
                ASSERT_SAFE_FAIL(Obj(-1, SDATA[0],      MDATA[0]));

                ASSERT_SAFE_FAIL(Obj( 1, (Strategy *)0, MDATA[0]));

                ASSERT_SAFE_FAIL(Obj( 1, SDATA[0],      (int *)0));
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING RESERVECAPACITY
        //
        // Concerns:
        //   Our primary concern is that 'reserveCapacity(sz, n)' reserves
        //   sufficient memory to satisfy 'n' allocation requests from the
        //   pool managing objects of size 'sz'.
        //
        //   QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //   To test 'reserveCapacity', specify a table of capacities to
        //   reserve.  Construct an object managing three pools and call
        //   'reserveCapacity' for each of the three pools with the tabulated
        //   number of elements.  Allocate as many objects as required to
        //   bring the size of the pool under test to the specified number of
        //   elements and use 'bslma::TestAllocator' to verify that no
        //   additional allocations have occurred.  Perform each test in the
        //   standard 'bslma' exception-testing macro block.
        //
        // Testing:
        //   void reserveCapacity(int size, int numBlocks);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing RESERVECAPACITY"
                          << endl << "=======================" << endl;

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
                        LOOP3_ASSERT(ti, ei, zi, NUM_BLOCKS ==
                                               testAllocator.numBlocksTotal());
                        LOOP3_ASSERT(ti, ei, zi, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
                      } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX(2);

            {
                ASSERT_SAFE_PASS(mX.reserveCapacity( 1,  1));
                ASSERT_SAFE_PASS(mX.reserveCapacity(16,  0));

                ASSERT_SAFE_FAIL(mX.reserveCapacity( 0,  1));
                ASSERT_SAFE_FAIL(mX.reserveCapacity(-1,  1));
                ASSERT_SAFE_FAIL(mX.reserveCapacity(17,  1));

                ASSERT_SAFE_FAIL(mX.reserveCapacity(16, -1));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING RELEASE
        //
        // Concerns:
        //   Our primary concern is that 'release' relinquishes all
        //   outstanding memory to the allocator that is in use by the
        //   multipool.
        //
        // Plan:
        //   Create multipools that manage a varying number of pools and
        //   make many allocation requests from each of the pools, as well as
        //   from the "overflow" block list.  As a simple sanity test,
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

        if (verbose) cout << endl << "TESTING RELEASE"
                          << endl << "===============" << endl;

        const int MAX_POOLS     = 3;
        const int POOL_QUANTA[] = { MAX_ALIGN, MAX_ALIGN * 2, MAX_ALIGN * 4 };
        const int OVERFLOW_SIZE = MAX_ALIGN * 5;
        const int NITERS        = MAX_ALIGN * 256;

        for (int i = 1; i <= MAX_POOLS; ++i) {
            if (veryVerbose) { T_ cout << "# pools: "; P(i); }
            Obj mX(i, Z);
            const bsls::Types::Int64 NUM_BLOCKS =
                                                testAllocator.numBlocksInUse();
            const bsls::Types::Int64 NUM_BYTES  =
                                                 testAllocator.numBytesInUse();
            int its              = NITERS;
            while (its-- > 0) {  // exercise each pool, as well as "overflow"
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
        // TESTING DEALLOCATE
        //
        // Concerns:
        //   Our primary concern is that 'deallocate' returns the block of
        //   memory to the underlying pool making it available for future
        //   allocation.
        //
        //   QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //   Create multipools that manage a varying number of pools and
        //   make many allocation requests from each of the pools, as well as
        //   from the "overflow" block list.  As a simple sanity test,
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

        if (verbose) cout << endl << "TESTING DEALLOCATE"
                          << endl << "==================" << endl;

        const int MAX_POOLS     = 3;
        const int POOL_QUANTA[] = { MAX_ALIGN, MAX_ALIGN * 2, MAX_ALIGN * 4 };
        const int OVERFLOW_SIZE = MAX_ALIGN * 5;
        const int NITERS        = MAX_ALIGN * 256;

        bsls::Types::Int64 numBlocks;
        bsls::Types::Int64 numBytes;

        for (int i = 1; i <= MAX_POOLS; ++i) {
            if (veryVerbose) { T_ cout << "# pools: "; P(i); }
            Obj mX(i + 1, Z);
            for (int j = 0; j < i; ++j) {
                if (veryVerbose) { T_ T_ cout << "pool: "; P(j); }
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

            if (veryVerbose) { T_ T_ cout << "overflow\n"; }
            int its       = NITERS;
            int firstTime = 1;
            while (its-- > 0) {  // exercise "overflow" block list
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

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX(2);
            char *p = (char *)mX.allocate(8);

            if (veryVerbose) cout << "\t'deallocate(0)'" << endl;
            {
                ASSERT_SAFE_PASS(mX.deallocate(p));

                ASSERT_SAFE_FAIL(mX.deallocate(0));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATE
        //
        // Concerns:
        //   We have the following concerns:
        //    1) The memory returned by 'allocate' is properly aligned.
        //    2) The memory returned by 'allocate' is of sufficient size.
        //    3) The allocation request is distributed to the proper underlying
        //       pool, or to the "overflow" block list.
        //
        //    QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //   Create multipools that manage a varying number of pools and
        //   allocate objects of varying sizes from each of the pools, as well
        //   as from the "overflow" block list.  As a simple sanity test,
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
        //   void *allocate(int size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING ALLOCATE"
                          << endl << "================" << endl;

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

                Obj mX(NUM_POOLS, Z);

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

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX(2);

            if (veryVerbose) cout << "\t'allocate(1 <= size)'" << endl;
            {
                ASSERT_SAFE_PASS(mX.allocate( 1));

                ASSERT_SAFE_FAIL(mX.allocate( 0));
                ASSERT_SAFE_FAIL(mX.allocate(-1));
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'NUMPOOLS' CTOR AND DTOR
        //
        // Concerns:
        //   We have the following concerns:
        //    1) The 'bdlma::Multipool' 'numPools' constructor works properly:
        //       a. The constructor is exception neutral w.r.t. memory
        //          allocation.
        //       b. The internal memory management system is hooked up properly
        //          so that *all* internally allocated memory draws from a
        //          user-supplied allocator whenever one is specified.
        //       c. Objects may be allocated from the pools managed by the
        //          multipool.
        //    2) The destructor works properly as implicitly tested in the
        //       various scopes of this test and in the presence of exceptions.
        //
        //   Note that this test case only tests one of the constructors.
        //
        // Plan:
        //   Create a test object using the constructor: 1) without exceptions
        //   and 2) in the presence of exceptions during memory allocations
        //   using a 'bslma::TestAllocator' and varying its *allocation*
        //   *limit*.  When the object goes out of scope, verify that the
        //   destructor properly deallocates all memory that had been allocated
        //   to it.
        //
        //   Concern 2 is addressed by making use of the 'allocate' method
        //   (which is thoroughly tested in case 3).
        //
        //   At the end, include a constructor test specifying a static buffer
        //   allocator.  This tests for rudimentary correct object behavior
        //   via the destructor and Purify.
        //
        // Testing:
        //   bdlma::Multipool(numPools, Allocator *ba = 0);
        //   ~bdlma::Multipool();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING NUMPOOLS CTOR AND DTOR"
                          << endl << "=============================="
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
                    char *p = (char *) mX.allocate(2048);  // "overflow" block
                                                           // list
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
                    char *p = (char *) mX.allocate(2048);  // "overflow" block
                                                           // list
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
                  char *p = (char *) mX.allocate(2048);  // "overflow" block
                                                         // list
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
            char *p = (char *) doNotDelete->allocate(2048);  // "overflow"
                                                             // block list
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
        //   That the basic functionality of 'bdlma::Multipool' works properly.
        //
        // Plan:
        //   Create a multipool that manages three pools.  Allocate memory from
        //   the first two pools, as well as from the "overflow" block list.
        //   Then 'deallocate' or 'release' the allocated blocks.  Finally, let
        //   the multipool go out of scope to exercise the destructor.
        //
        // Testing:
        //   This "test" exercises basic functionality, but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST"
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

            p = (char *)mX.allocate(8);                   ASSERT(p);

            // 3.
            if (verbose) cout << "3. Deallocate the small object." << endl;

            mX.deallocate(p);

            // Address 'p' is no longer valid.

            // 4.
            if (verbose)
                cout << "4. Reserve capacity in the medium object pool for "
                        "at least two objects."
                     << endl;

            mX.reserveCapacity(8 * 2, 2);

            // 5.
            if (verbose)
                cout << "5. Allocate two objects from the medium object pool."
                     << endl;

            p = (char *)mX.allocate(8 * 2);               ASSERT(p);
            q = (char *)mX.allocate(8 * 2 - 1);           ASSERT(q);

            // 6.
            if (verbose)
                cout << "6. Allocate an object from the \"overflow\" block "
                        "list."
                     << endl;

            r = (char *)mX.allocate(1024);                ASSERT(r);

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

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

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
