// bdema_multipool.t.cpp                                              -*-C++-*-

#include <bdema_multipool.h>

#include <bdema_bufferedsequentialallocator.h>   // for testing only
#include <bdes_bitutil.h>

#include <bslma_testallocator.h>                 // for testing only
#include <bslma_testallocatorexception.h>        // for testing only

#include <bsls_alignmentutil.h>

#include <bsl_cstdlib.h>                         // atoi()
#include <bsl_cstring.h>                         // memcpy(), memset()
#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>
#include <bsl_utility.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The 'bdema_Multipool' class consists of one constructor, a destructor,
// and four manipulators.  The manipulators are used to allocate, deallocate,
// and reserve memory.  Since this component is a memory manager, the
// 'bdema_testallocator' component is used extensively to verify expected
// behaviors.  Note that the copying of objects is explicitly disallowed
// since the copy constructor and assignment operator are declared 'private'
// and left unimplemented.  So we are primarily concerned that the internal
// memory management system functions as expected and that the manipulators
// operator correctly.  Note that memory allocation must be tested for
// exception neutrality (also via the 'bdema_testallocator' component).
// Several small helper functions are also used to facilitate testing.
//-----------------------------------------------------------------------------
// [ 2] bdema_Multipool(int numPools, bslma_Allocator *ba = 0);
// [ 2] ~bdema_Multipool();
// [ 3] void *allocate(int size);
// [ 4] void deallocate(void *address);
// [ 5] void release();
// [ 6] void reserveCapacity(int size, int numObjects);
// [ 7] bdema_MultipoolAllocator(numPools, minSize);
// [ 7] bdema_MultipoolAllocator(numPools, poolNumObjects);
// [ 7] bdema_MultipoolAllocator(numPools, minSize, poolNumObjects);
// [ 7] bdema_MultipoolAllocator(numPools, minSize, Z);
// [ 7] bdema_MultipoolAllocator(numPools, poolNumObjects, Z);
// [ 7] bdema_MultipoolAllocator(numPools, minSize, poolNumObjects, Z);
// [ 8] void deleteObjectRaw()
// [ 8] void deleteObject()
// [ 9] int numPools() const;
// [ 9] int maxPooledBlockSize() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE
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
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << '\t';

//=============================================================================
//                       GLOBAL TYPES AND CONSTANTS
//-----------------------------------------------------------------------------

typedef bdema_Multipool Obj;

const int MAX_ALIGN = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;

// Warning: keep this in sync with bdema_Multipool.h!
struct Header {
    // Stores pool number of this item.
    union {
        int                                d_pool;   // pool for this item
        bsls_AlignmentUtil::MaxAlignedType d_dummy;  // force maximum alignment
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

static int calcPool(int numPools, int objSize)
    // Calculate the index of the pool that should allocate objects that are
    // of the specified 'objSize' bytes in size from a multi-pool managing
    // the specified 'numPools' number of memory pools.
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

inline static int recPool(char *address)
    // Return the index of the pool that allocated the memory at the specified
    // 'address'.
{
    ASSERT(address);

    Header *h = (Header *)address - 1;

    return h->d_header.d_pool;
}

inline int delta(char *address1, char *address2)
    // Return the number of bytes between the specified 'address1' and
    // the specified 'address2'.
{
    return address1 < address2 ? address2 - address1 : address1 - address2;
}

inline static void scribble(char *address, int size)
    // Assign a non-zero value to each of the specified 'size' bytes starting
    // at the specified 'address'.
{
    memset(address, 0xff, size);
}

void stretchRemoveAll(Obj *object, int numElements, int objSize)
   // Using only primary manipulators, extend the capacity of the specified
   // 'object' to (at least) the specified 'numElements' each of the specified
   // 'objSize' bytes, then remove all elements leaving 'object' empty.  The
   // behavior is undefined unless 0 <= numElements and 0 <= objSize.
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
///Example 1: Using a 'bdema_Multipool' Directly
///- - - - - - - - - - - - - - - - - - - - - - -
// A 'bdema_Multipool' can be used by containers that hold different types of
// elements, each of uniform size, for efficient memory allocation of new
// elements.  Suppose we have a factory class, 'my_MessageFactory', that
// creates messages based on user requests.  Each message is created with the
// most efficient memory storage possible - using predefined 8-byte, 16-byte
// and 32-byte buffers.  If the message size exceeds the three predefined
// values, a generic message is used.  For efficient memory allocation of
// messages, we use a 'bdema_Multipool'.
//
// First, we define our message types as follows:
//..
    class my_Message {
        // This class represents a general message interface that provides
        // a 'getMessage' method for clients to retrieve the underlying
        // message.

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
        bdema_Multipool d_multipool;  // multipool used to supply memory

      public:
        // CREATORS
        my_MessageFactory(bslma_Allocator *basicAllocator = 0);
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
// implementation-defined default value instead of calculating and specifying
// a value.  Note that if users want to specify the number of pools, the value
// can be calculated as the smallest 'N' such that the following relationship
// holds:
//..
//  N > log2(sizeof(Object Type)) - 2
//..
// Continuing on with the usage example:
//..
    // CREATORS
    my_MessageFactory::my_MessageFactory(bslma_Allocator *basicAllocator)
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
// A 'bdema_Multipool' is ideal for allocating the different sized messages
// since repeated deallocations might be necessary (which renders a
// 'bdema_SequentialPool' unsuitable) and the sizes of these types are all
// different:
//..
    // MANIPULATORS
    my_Message *my_MessageFactory::createMessage(const char *data)
    {
        enum { SMALL = 8, MEDIUM = 16, LARGE = 32 };

        const int length = bsl::strlen(data);

        if (length < SMALL) {
            return new(d_multipool.allocate(sizeof(my_SmallMessage)))
                                                 my_SmallMessage(data, length);
        }

        if (length < MEDIUM) {
            return new(d_multipool.allocate(sizeof(my_MediumMessage)))
                                                my_MediumMessage(data, length);
        }

        if (length < LARGE) {
            return new(d_multipool.allocate(sizeof(my_LargeMessage)))
                                                 my_LargeMessage(data, length);
        }

        char *buffer = (char *)d_multipool.allocate(length + 1);
        bsl::memcpy(buffer, data, length + 1);

        return new(d_multipool.allocate(sizeof(my_GenericMessage)))
                                                     my_GenericMessage(buffer);
    }
//..
//
///Example 2: Implementing an Allocator Using 'bdema_Multipool'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bslma_Allocator' is used throughout the interfaces of BDE components.
// Suppose we would like to create a multipool allocator,
// 'my_MultipoolAllocator', that allocates memory from multiple 'bdema_Pool'
// objects in a similar fashion to 'bdema_Multipool'.  This class can be used
// directly to implement such an allocator.
//
// Note that the documentation for this class is simplified for this usage
// example.  Please see 'bdema_multipoolallocator' for full documentation of a
// similar class.
//..
    class my_MultipoolAllocator : public bslma_Allocator {
        // This class implements the 'bslma_Allocator' protocol to provide an
        // allocator that manages a set of memory pools, each dispensing memory
        // blocks of a unique size, with each successive pool's block size
        // being twice that of the previous one.

        // DATA
        bdema_Multipool d_multiPool;  // memory manager for allocated memory
                                      // blocks

      public:
        // CREATORS
        my_MultipoolAllocator(bslma_Allocator *basicAllocator = 0);
            // Create a multipool allocator.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        // ...

        virtual ~my_MultipoolAllocator();
            // Destroy this multipool allocator.  All memory allocated from
            // this memory pool is released.

        // MANIPULATORS
        virtual void *allocate(int size);
            // Return the address of a contiguous block of maximally-aligned
            // memory of (at least) the specified 'size' (in bytes).  The
            // behavior is undefined unless '1 <= size'.

        virtual void deallocate(void *address);
            // Relinquish the memory block at the specified 'address' back to
            // this multipool allocator for reuse.  The behavior is undefined
            // unless 'address' is non-zero, was allocated by this multipool
            // allocator, and has not already been deallocated.
    };

    // CREATORS
    inline
    my_MultipoolAllocator::my_MultipoolAllocator(
                                              bslma_Allocator *basicAllocator)
    : d_multiPool(basicAllocator)
    {
    }

    my_MultipoolAllocator::~my_MultipoolAllocator()
    {
    }

    // MANIPULATORS
    inline
    void *my_MultipoolAllocator::allocate(int size)
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

    bslma_TestAllocator  testAllocator(veryVeryVerbose);
    bslma_Allocator     *Z = &testAllocator;

    switch (test) { case 0:
      case 10: {
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
            bslma_TestAllocator ta;
            my_MessageFactory factory(&ta);

            my_Message *msg = factory.createMessage("Hello");
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'numPools' and 'maxPooledBlockSize'
        //
        // Concerns:
        //   1) That 'numPools' return the number of pools managed by the
        //      multipool.
        //
        //   2) That 'maxPooledBlockSize' correctly return the correct maximum
        //      block size managed by the multipool given the specified
        //      'numPools'.
        //
        // Plan:
        //   Since the constructors is thoroughly tested at this point, simply
        //   construct a multipool, pass in different 'numMemoryPools'
        //   arguments specified in a test array, and verify that the number of
        //   pools returned by 'numPools' is the same.
        //
        // Testing:
        //   int numPools() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                      << "TESTING 'numPools' and 'maxPooledBlockSize'" << endl
                      << "===========================================" << endl;

        const int MA = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;

        static const struct {
            int d_lineNum;       // line number
            int d_numPools;      // number of pools
            int d_maxBlockSize;  // maximum block size
        } DATA[] = {
            //LINE  NUMPOOLS    MAXBLOCKSIZE
            //----  --------    ------------
            { L_,   1,          8            },
            { L_,   2,          16           },
            { L_,   3,          32           },
            { L_,   4,          64           },
            { L_,   5,          128          },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE         = DATA[i].d_lineNum;
            const int NUMPOOLS     = DATA[i].d_numPools;
            const int MAXBLOCKSIZE = DATA[i].d_maxBlockSize;

            if (veryVerbose) {
                P_(LINE) P_(NUMPOOLS) P(MAXBLOCKSIZE)
            }

            Obj mp(NUMPOOLS, &testAllocator);

            LOOP2_ASSERT(NUMPOOLS, mp.numPools(),
                         NUMPOOLS == mp.numPools());

            LOOP2_ASSERT(MAXBLOCKSIZE, mp.maxPooledBlockSize(),
                         MAXBLOCKSIZE == mp.maxPooledBlockSize());
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
        //   Iterate where at the beginning of the loop, we create an object
        //   of type 'mostDerived' that multiply inherits from two types with
        //   virtual destructors.  Then in the middle of the loop we switch
        //   into several ways to destroying and deallocating the object with
        //   various forms of 'deleteObjectRaw' and 'deleteObject', after
        //   which we verify that the destructors have been run.  Each
        //   iteration we verify that the memory we got was the same as last
        //   time, showing that memory is being deallocated and recovered
        //   by the pool.
        //
        // Testing:
        //   void deleteObjectRaw()
        //   void deleteObject()
        // --------------------------------------------------------------------

        bool finished = false;
        const MostDerived *repeater = 0;    // verify we're re-using the memory
                                            // each time
        bdema_Multipool mp(6, Z);
        for (int di = 0; !finished; ++di) {
            MostDerived *pMD = (MostDerived *) mp.allocate(sizeof(*pMD));
            const MostDerived *pMDC = pMD;
            new (pMD) MostDerived();

            if (!repeater) {
                repeater = pMDC;
            }
            else {
                // this verifies that we are freeing the memory each iteration
                // because we get the same pointer every time we allocate, and
                // we allocate one extra time at the end
                LOOP_ASSERT(di, repeater == pMDC);
            }

            ASSERT(1 == numLeftChildren);
            ASSERT(1 == numRightChildren);
            ASSERT(1 == numMostDerived);

            switch (di) {
              case 0: {
                mp.deleteObjectRaw(pMDC);
              } break;
              case 1: {
                const LeftChild *pLCC = pMDC;
                ASSERT((void*) pLCC == (void*) pMDC);
                mp.deleteObjectRaw(pLCC);
              } break;
              case 2: {
                mp.deleteObject(pMDC);
              } break;
              case 3: {
                const LeftChild *pLCC = pMDC;
                ASSERT((void*) pLCC == (void*) pMDC);
                mp.deleteObject(pLCC);
              } break;
              case 4: {
                const RightChild *pRCC = pMDC;
                ASSERT((void*) pRCC != (void*) pMDC);
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
            LOOP_ASSERT(di, 0 == numRightChildren);
            LOOP_ASSERT(di, 0 == numMostDerived);
        }
      } break;
      case 7: {
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
        //   of 'bdema_Pool', each initialized with the corresponding growth
        //   strategy, max blocks per chunk and managing memory blocks doubling
        //   in size - just as what is managed within 'bdema_Multipool'.  Then
        //   create a multipool.  Allocate different object sizes from the
        //   multipool, and test that the growth of the internal chunks within
        //   the multipool is the same as the appropriate 'bdema_Pool',
        //   determined by the 'calcPool' method.
        //
        //   To test the default arguments (the different constructors), the
        //   we repeat the test above, but initialize the 'bdema_Pool' with the
        //   expected default argument.
        //
        // Testing:
        //   bdema_Multipool(bslma_Allocator *ba = 0);
        //   bdema_Multipool(int numPools, bslma_Allocator *ba = 0);
        //   bdema_Multipool(Strategy gs, bslma_Allocator *ba = 0);
        //   bdema_Multipool(int n, Strategy gs, bslma_Allocator *ba = 0);
        //   bdema_Multipool(int n, const Strategy *gsa, Allocator *ba = 0);
        //   bdema_Multipool(int n, int mbpc, bslma_Allocator *ba= 0);
        //   bdema_Multipool(int n, const int *mbpc, bslma_Allocator *ba = 0);
        //   bdema_Multipool(int n, Strategy gs, int mbpc, Allocator *ba = 0);
        //   bdema_Multipool(int n, const S *gsa, int mbpc, Alloc *ba = 0);
        //   bdema_Multipool(int n, Strat gs, const int *ma, Alloc *ba = 0);
        //   bdema_Multipool(int n, const S *gsa, const int *ma, A *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'constructor' and 'destructor'"
                          << endl << "======================================"
                          << endl;

        enum { INITIAL_CHUNK_SIZE = 1,
               DEFAULT_MAX_CHUNK_SIZE = 32,
               DEFAULT_NUM_POOLS = 10 };

        // For pool allocation.
        char buffer[1024];
        bdema_BufferedSequentialAllocator bsa(buffer, 1024);

        const int NUM_POOLS = 5;
        const int TEST_MAX_CHUNK_SIZE = 30;

        // Object Data
        const int ODATA[]   = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };
        const int NUM_ODATA = sizeof ODATA / sizeof *ODATA;

        typedef bsls_BlockGrowth::Strategy St;
        const bsls_BlockGrowth::Strategy GEO= bsls_BlockGrowth::BSLS_GEOMETRIC;
        const bsls_BlockGrowth::Strategy CON= bsls_BlockGrowth::BSLS_CONSTANT;

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
            cout << "bdema_Multipool(int n, const S *gsa, "
                    "const int *ma, A *ba = 0)" << endl;
        }
        for (int si = 0; si < NUM_SDATA; ++si) {
            for (int mi = 0; mi < NUM_MDATA; ++mi) {

                bslma_TestAllocator pta("pool test allocator",
                                        veryVeryVerbose);
                bslma_TestAllocator mpta("multipool test allocator",
                                         veryVeryVerbose);

                // Initialize the pools
                bdema_Pool *pool[NUM_POOLS];
                int INIT = 8;
                for (int j = 0; j < NUM_POOLS; ++j) {
                    pool[j] = new(bsa)bdema_Pool(INIT,
                                                 SDATA[si][j],
                                                 MDATA[mi][j],
                                                 &pta);
                    INIT <<= 1;
                }

                // Create the multipool
                Obj mp(NUM_POOLS, SDATA[si], MDATA[mi], &mpta);

                // Allocate until we depleted the pool
                int poolAllocations      = pta.numAllocations();
                int multipoolAllocations = mpta.numAllocations();

                // multipool should have an extra allocation for the array
                // of pools.
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
                        int ri = INITIAL_CHUNK_SIZE;
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
            cout << "bdema_Multipool(bslma_Allocator *ba = 0)" << endl;
        }

        {
            bslma_TestAllocator pta("pool test allocator",
                                    veryVeryVerbose);
            bslma_TestAllocator mpta("multipool test allocator",
                                     veryVeryVerbose);

            // Initialize the pools
            bdema_Pool *pool[DEFAULT_NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < DEFAULT_NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdema_Pool(INIT,
                                             GEO,
                                             DEFAULT_MAX_CHUNK_SIZE,
                                             &pta);
                INIT <<= 1;
            }

            // Create the multipool
            Obj mp(&mpta);

            // Allocate until we depleted the pool
            int poolAllocations      = pta.numAllocations();
            int multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array
            // of pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(DEFAULT_NUM_POOLS,
                                                 OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < DEFAULT_NUM_POOLS);

                // Testing geometric growth
                int ri = INITIAL_CHUNK_SIZE;
                while (ri < DEFAULT_MAX_CHUNK_SIZE) {
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

                // Testing constant growth (also applies to capped
                // geometric growth).
                const int NUM_REPLENISH = 3;
                for (ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < DEFAULT_MAX_CHUNK_SIZE; ++j) {
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
            for (int j = 0; j < DEFAULT_NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "bdema_Multipool(int numPools, bslma_Allocator *ba = 0)"
                 << endl;
        }
        {
            bslma_TestAllocator pta("pool test allocator",
                                    veryVeryVerbose);
            bslma_TestAllocator mpta("multipool test allocator",
                                     veryVeryVerbose);

            // Initialize the pools
            bdema_Pool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdema_Pool(INIT,
                                             GEO,
                                             DEFAULT_MAX_CHUNK_SIZE,
                                             &pta);
                INIT <<= 1;
            }

            // Create the multipool
            Obj mp(NUM_POOLS, &mpta);

            // Allocate until we depleted the pool
            int poolAllocations      = pta.numAllocations();
            int multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array
            // of pools.
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
                int ri = INITIAL_CHUNK_SIZE;
                while (ri < DEFAULT_MAX_CHUNK_SIZE) {
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

                // Testing constant growth (also applies to capped
                // geometric growth).
                const int NUM_REPLENISH = 3;
                for (ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < DEFAULT_MAX_CHUNK_SIZE; ++j) {
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
            cout << "bdema_Multipool(Strategy gs, bslma_Allocator *ba = 0)"
                 << endl;
        }
        {
            bslma_TestAllocator pta("pool test allocator",
                                    veryVeryVerbose);
            bslma_TestAllocator mpta("multipool test allocator",
                                     veryVeryVerbose);

            // Initialize the pools
            bdema_Pool *pool[DEFAULT_NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < DEFAULT_NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdema_Pool(INIT,
                                             CON,
                                             DEFAULT_MAX_CHUNK_SIZE,
                                             &pta);
                INIT <<= 1;
            }

            // Create the multipool
            Obj mp(CON, &mpta);

            // Allocate until we depleted the pool
            int poolAllocations      = pta.numAllocations();
            int multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array
            // of pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations + 1 == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(DEFAULT_NUM_POOLS,
                                                 OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < DEFAULT_NUM_POOLS);

                // Testing constant growth
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < DEFAULT_MAX_CHUNK_SIZE; ++j) {
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
            for (int j = 0; j < DEFAULT_NUM_POOLS; ++j) {
                pool[j]->release();
            }
        }

        if (verbose) {
            cout << "bdema_Multipool(int n, Strategy gs, bslma_Allocator "
                    "*ba = 0)" << endl;
        }
        {
            bslma_TestAllocator pta("pool test allocator",
                                    veryVeryVerbose);
            bslma_TestAllocator mpta("multipool test allocator",
                                     veryVeryVerbose);

            // Initialize the pools
            bdema_Pool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdema_Pool(INIT,
                                             CON,
                                             DEFAULT_MAX_CHUNK_SIZE,
                                             &pta);
                INIT <<= 1;
            }

            // Create the multipool
            Obj mp(NUM_POOLS, CON, &mpta);

            // Allocate until we depleted the pool
            int poolAllocations      = pta.numAllocations();
            int multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array
            // of pools.
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

                    for (int j = 0; j < DEFAULT_MAX_CHUNK_SIZE; ++j) {
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
            cout << "bdema_Multipool(int n, const Strategy *gsa, Allocator "
                    "*ba = 0)" << endl;
        }
        for (int si = 0; si < NUM_SDATA; ++si) {
            bslma_TestAllocator pta("pool test allocator",
                                    veryVeryVerbose);
            bslma_TestAllocator mpta("multipool test allocator",
                                     veryVeryVerbose);

            // Initialize the pools
            bdema_Pool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdema_Pool(INIT,
                                             SDATA[si][j],
                                             DEFAULT_MAX_CHUNK_SIZE,
                                             &pta);
                INIT <<= 1;
            }

            // Create the multipool
            Obj mp(NUM_POOLS, SDATA[si], &mpta);

            // Allocate until we depleted the pool
            int poolAllocations      = pta.numAllocations();
            int multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array
            // of pools.
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
                    int ri = INITIAL_CHUNK_SIZE;
                    while (ri < DEFAULT_MAX_CHUNK_SIZE) {
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

                    for (int j = 0; j < DEFAULT_MAX_CHUNK_SIZE; ++j) {
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
            cout << "bdema_Multipool(int n, int mbpc, bslma_Allocator *ba= 0)"
                 << endl;
        }
//        {
//            bslma_TestAllocator pta("pool test allocator",
//                                    veryVeryVerbose);
//            bslma_TestAllocator mpta("multipool test allocator",
//                                     veryVeryVerbose);
//
//            // Initialize the pools
//            bdema_Pool *pool[NUM_POOLS];
//            int INIT = 8;
//            for (int j = 0; j < NUM_POOLS; ++j) {
//                pool[j] = new(bsa)bdema_Pool(INIT,
//                                             GEO,
//                                             TEST_MAX_CHUNK_SIZE,
//                                             &pta);
//                INIT <<= 1;
//            }
//
//            // Create the multipool
//            Obj mp(NUM_POOLS, TEST_MAX_CHUNK_SIZE, &mpta);
//
//            // Allocate until we depleted the pool
//            int poolAllocations      = pta.numAllocations();
//            int multipoolAllocations = mpta.numAllocations();
//
//            // multipool should have an extra allocation for the array
//            // of pools.
//            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
//                         poolAllocations + 1 == multipoolAllocations);
//
//            for (int oi = 0; oi < NUM_ODATA; ++oi) {
//                const int OBJ_SIZE = ODATA[oi];
//                const int calcPoolNum = calcPool(NUM_POOLS,
//                                                 OBJ_SIZE);
//
//                if (-1 == calcPoolNum) {
//                    char *p = (char *) mp.allocate(OBJ_SIZE);
//                    const int recordPoolNum = recPool(p);
//
//                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
//                    continue;
//                }
//
//                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);
//
//                // Testing geometric growth
//                int ri = INITIAL_CHUNK_SIZE;
//                while (ri < TEST_MAX_CHUNK_SIZE) {
//                    poolAllocations      = pta.numAllocations();
//                    multipoolAllocations = mpta.numAllocations();
//
//                    for (int j = 0; j < ri; ++j) {
//                        char *p = (char *)mp.allocate(OBJ_SIZE);
//                        const int recordPoolNum = recPool(p);
//
//                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
//                                     recordPoolNum == calcPoolNum);
//
//                        pool[calcPoolNum]->allocate();
//                    }
//
//                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
//                                 poolAllocations + 1
//                                              == multipoolAllocations);
//
//                    ri <<= 1;
//                }
//
//                // Testing constant growth (also applies to capped
//                // geometric growth).
//                const int NUM_REPLENISH = 3;
//                for (ri = 0; ri < NUM_REPLENISH; ++ri) {
//                    poolAllocations      = pta.numAllocations();
//                    multipoolAllocations = mpta.numAllocations();
//
//                    for (int j = 0; j < TEST_MAX_CHUNK_SIZE; ++j) {
//                        char *p = (char *)mp.allocate(OBJ_SIZE);
//                        const int recordPoolNum = recPool(p);
//
//                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
//                                         recordPoolNum == calcPoolNum);
//
//                        pool[calcPoolNum]->allocate();
//                    }
//
//                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
//                                 poolAllocations + 1
//                                                  == multipoolAllocations);
//                }
//            }
//
//            // Release all pooled memory.
//            for (int j = 0; j < NUM_POOLS; ++j) {
//                pool[j]->release();
//            }
//        }

        if (verbose) {
            cout << "bdema_Multipool(int n, const int *mbpc, bslma_Allocator "
                    "*ba = 0)" << endl;
        }
//        for (int mi = 0; mi < NUM_MDATA; ++mi) {
//
//            bslma_TestAllocator pta("pool test allocator",
//                                    veryVeryVerbose);
//            bslma_TestAllocator mpta("multipool test allocator",
//                                     veryVeryVerbose);
//
//            // Initialize the pools
//            bdema_Pool *pool[NUM_POOLS];
//            int INIT = 8;
//            for (int j = 0; j < NUM_POOLS; ++j) {
//                pool[j] = new(bsa)bdema_Pool(INIT,
//                                             GEO,
//                                             MDATA[mi][j],
//                                             &pta);
//                INIT <<= 1;
//            }
//
//            // Create the multipool
//            Obj mp(NUM_POOLS, MDATA[mi], &mpta);
//
//            // Allocate until we depleted the pool
//            int poolAllocations      = pta.numAllocations();
//            int multipoolAllocations = mpta.numAllocations();
//
//            // multipool should have an extra allocation for the array
//            // of pools.
//            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
//                         poolAllocations + 1 == multipoolAllocations);
//
//            for (int oi = 0; oi < NUM_ODATA; ++oi) {
//                const int OBJ_SIZE = ODATA[oi];
//                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);
//
//                if (-1 == calcPoolNum) {
//                    char *p = (char *) mp.allocate(OBJ_SIZE);
//                    const int recordPoolNum = recPool(p);
//
//                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
//                    continue;
//                }
//
//                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);
//
//                // Testing geometric growth
//                int ri = INITIAL_CHUNK_SIZE;
//                while (ri < MDATA[mi][calcPoolNum]) {
//                    poolAllocations      = pta.numAllocations();
//                    multipoolAllocations = mpta.numAllocations();
//
//                    for (int j = 0; j < ri; ++j) {
//                        char *p = (char *)mp.allocate(OBJ_SIZE);
//                        const int recordPoolNum = recPool(p);
//
//                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
//                                     recordPoolNum == calcPoolNum);
//
//                        pool[calcPoolNum]->allocate();
//                    }
//
//                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
//                                 poolAllocations + 1
//                                              == multipoolAllocations);
//
//                    ri <<= 1;
//                }
//
//                // Testing constant growth (also applies to capped
//                // geometric growth).
//                const int NUM_REPLENISH = 3;
//                for (ri = 0; ri < NUM_REPLENISH; ++ri) {
//                    poolAllocations      = pta.numAllocations();
//                    multipoolAllocations = mpta.numAllocations();
//
//                    for (int j = 0; j < MDATA[mi][calcPoolNum]; ++j) {
//                        char *p = (char *)mp.allocate(OBJ_SIZE);
//                        const int recordPoolNum = recPool(p);
//
//                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
//                                         recordPoolNum == calcPoolNum);
//
//                        pool[calcPoolNum]->allocate();
//                    }
//
//                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
//                                 poolAllocations + 1
//                                                  == multipoolAllocations);
//                }
//            }
//
//            // Release all pooled memory.
//            for (int j = 0; j < NUM_POOLS; ++j) {
//                pool[j]->release();
//            }
//        }

        if (verbose) {
            cout << "bdema_Multipool(int n, Strategy gs, int mbpc, Allocator "
                    "*ba = 0)" << endl;
        }
        {
            bslma_TestAllocator pta("pool test allocator",
                                    veryVeryVerbose);
            bslma_TestAllocator mpta("multipool test allocator",
                                     veryVeryVerbose);

            // Initialize the pools
            bdema_Pool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdema_Pool(INIT,
                                             CON,
                                             TEST_MAX_CHUNK_SIZE,
                                             &pta);
                INIT <<= 1;
            }

            // Create the multipool
            Obj mp(NUM_POOLS, CON, TEST_MAX_CHUNK_SIZE, &mpta);

            // Allocate until we depleted the pool
            int poolAllocations      = pta.numAllocations();
            int multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array
            // of pools.
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
            cout << "bdema_Multipool(int n, const S *gsa, int mbpc, Alloc "
                    "*ba = 0)"
                 << endl;
        }
        for (int si = 0; si < NUM_SDATA; ++si) {
            bslma_TestAllocator pta("pool test allocator",
                                    veryVeryVerbose);
            bslma_TestAllocator mpta("multipool test allocator",
                                     veryVeryVerbose);

            // Initialize the pools
            bdema_Pool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdema_Pool(INIT,
                                             SDATA[si][j],
                                             TEST_MAX_CHUNK_SIZE,
                                             &pta);
                INIT <<= 1;
            }

            // Create the multipool
            Obj mp(NUM_POOLS, SDATA[si], TEST_MAX_CHUNK_SIZE, &mpta);

            // Allocate until we depleted the pool
            int poolAllocations      = pta.numAllocations();
            int multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array
            // of pools.
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
                    int ri = INITIAL_CHUNK_SIZE;
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

                // Testing constant growth (also applies to capped
                // geometric growth).
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
            cout << "bdema_Multipool(int n, Strat gs, const int *ma, Alloc "
                    " *ba = 0)" << endl;
        }
        for (int mi = 0; mi < NUM_MDATA; ++mi) {

            bslma_TestAllocator pta("pool test allocator",
                                    veryVeryVerbose);
            bslma_TestAllocator mpta("multipool test allocator",
                                     veryVeryVerbose);

            // Initialize the pools
            bdema_Pool *pool[NUM_POOLS];
            int INIT = 8;
            for (int j = 0; j < NUM_POOLS; ++j) {
                pool[j] = new(bsa)bdema_Pool(INIT,
                                             CON,
                                             MDATA[mi][j],
                                             &pta);
                INIT <<= 1;
            }

            // Create the multipool
            Obj mp(NUM_POOLS, CON, MDATA[mi], &mpta);

            // Allocate until we depleted the pool
            int poolAllocations      = pta.numAllocations();
            int multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array
            // of pools.
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
        //   elements and use 'bslma_TestAllocator' to verify that no
        //   additional allocations have occurred.  Perform each test in the
        //   standard 'bdema' exception-testing macro block.
        //
        // Testing:
        //   void reserveCapacity(int size, int numObjects);
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
                      BEGIN_BSLMA_EXCEPTION_TEST {
                        Obj mX(NUM_POOLS, Z);
                        stretchRemoveAll(&mX, EXTEND[ei], OBJ_SIZE);
                        mX.reserveCapacity(OBJ_SIZE, 0);
                        mX.reserveCapacity(OBJ_SIZE, NE);
                        const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                        const int NUM_BYTES  = testAllocator.numBytesInUse();
                        for (int i = 0; i < NE; ++i) {
                            mX.allocate(OBJ_SIZE);
                        }
                        LOOP3_ASSERT(ti, ei, zi, NUM_BLOCKS ==
                                               testAllocator.numBlocksTotal());
                        LOOP3_ASSERT(ti, ei, zi, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
                      } END_BSLMA_EXCEPTION_TEST
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
        //   multi-pool.
        //
        // Plan:
        //   Create multi-pools that manage a varying number of pools and
        //   make many allocation requests from each of the pools
        //   (including the "overflow" pool).  As a simple sanity test,
        //   scribble over the extent of all allocated objects.  This
        //   ensures that no portion of the object is used by the multi-pool
        //   for bookkeeping.  Make use of the facilities available in
        //   'bslma_TestAllocator' to monitor memory usage.  Verify with
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
            if (veryVerbose) { T_ cout << "# pools: "; P(i); }
            Obj mX(i, Z);
            const int NUM_BLOCKS = testAllocator.numBlocksInUse();
            const int NUM_BYTES  = testAllocator.numBytesInUse();
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
        //   Create multi-pools that manage a varying number of pools and
        //   make many allocation requests from each of the pools
        //   (including the "overflow" pool).  As a simple sanity test,
        //   scribble over the extent of all allocated objects.  This
        //   ensures that no portion of the object is used by the multi-pool
        //   for bookkeeping.  Make use of the facilities available in
        //   'bslma_TestAllocator' to monitor memory usage.  Verify with
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

        int numBlocks;
        int numBytes;

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
        //   Create multi-pools that manage a varying number of pools and
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
        //   an object with the pool index that is recorded by the multi-pool
        //   when the object is allocated.  Note that concerns 2 and 3 entail
        //   some white-box testing.
        //
        // Testing:
        //   void *allocate(int size);
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
                               0 == bsls_PlatformUtil::UintPtr(p) % MAX_ALIGN);

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
                               0 == bsls_PlatformUtil::UintPtr(q) % MAX_ALIGN);

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
                               0 == bsls_PlatformUtil::UintPtr(p) % MAX_ALIGN);

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
                               0 == bsls_PlatformUtil::UintPtr(q) % MAX_ALIGN);

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
        //    1) The 'bdema_Multipool' constructor works properly:
        //       a. The constructor is exception neutral w.r.t. memory
        //          allocation.
        //       b. The internal memory management system is hooked up properly
        //          so that *all* internally allocated memory draws from a
        //          user-supplied allocator whenever one is specified.
        //       c. Objects may be allocated from the pools managed by the
        //          multi-pool.
        //    2) The destructor works properly as implicitly tested in the
        //       various scopes of this test and in the presence of exceptions.
        //   Note this test only tests one of the c'tors.
        //
        // Plan:
        //   Create a test object using the constructor: 1) without
        //   exceptions and 2) in the presence of exceptions during memory
        //   allocations using a 'bslma_TestAllocator' and varying its
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
        //   bdema_Multipool(int numPools, bslma_Allocator *ba = 0);
        //   ~bdema_Multipool();
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
                const int NUM_BLOCKS = testAllocator.numBlocksInUse();
                const int NUM_BYTES  = testAllocator.numBytesInUse();
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
                const int NUM_BLOCKS = testAllocator.numBlocksInUse();
                const int NUM_BYTES  = testAllocator.numBytesInUse();
                for (int j = 0; j < NUM_ODATA; ++j) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
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
                  } END_BSLMA_EXCEPTION_TEST
                  LOOP2_ASSERT(i, j, NUM_BLOCKS ==
                                               testAllocator.numBlocksInUse());
                  LOOP2_ASSERT(i, j, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
                }

                BEGIN_BSLMA_EXCEPTION_TEST {
                  Obj mX(NUM_POOLS, Z);
                  char *p = (char *) mX.allocate(2048);  // "overflow" pool
                  LOOP_ASSERT(i, p);
                  const int pRecordedPool = recPool(p);
                  if (veryVerbose) { T_ P_((void *)p); P(pRecordedPool); }
                  LOOP_ASSERT(i, -1 == pRecordedPool);
                } END_BSLMA_EXCEPTION_TEST
                LOOP_ASSERT(i, NUM_BLOCKS == testAllocator.numBlocksInUse());
                LOOP_ASSERT(i, NUM_BYTES  == testAllocator.numBytesInUse());
            }
        }

        if (verbose)
            cout << "\tWith a buffer allocator (exercise only)." << endl;
        {
            char memory[4096];
            bdema_BufferedSequentialAllocator a(memory, sizeof memory);
            Obj *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(1, &a);
            char *p = (char *) doNotDelete->allocate(2048);  // "overflow" pool
            ASSERT(p);
            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We are concerned that the basic functionality of 'bdema_Multipool'
        //   works properly.
        //
        // Plan:
        //   Create a multi-pool that manages two pools using the lone
        //   constructor.  Allocate memory from each of the pools as well as
        //   from the "overflow" pool.  Then 'deallocate' or 'release' the
        //   allocated objects.  Finally let the multi-pool go out of scope
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
                cout << "1. Create a multi-pool that manages three pools."
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
                cout << "8. Let the multi-pool go out of scope." << endl;
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
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
