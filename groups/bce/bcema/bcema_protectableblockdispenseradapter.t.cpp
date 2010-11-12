 // bcema_protectableblockdispenseradapter.t.cpp  -*-C++-*-

#include <bcema_protectableblockdispenseradapter.h>

#include <bdema_protectableblocklist.h>
#include <bslma_deallocatorproctor.h>
#include <bslma_autodestructor.h>

#include <bcemt_thread.h>
#include <bcemt_lockguard.h>

#include <bcema_testallocator.h>                   // for testing only
#include <bcemt_barrier.h>                         // for testing only
#include <bslma_testallocator.h>                   // for testing only
#include <bslma_testallocatorexception.h>          // for testing only
#include <bdema_testprotectableblockdispenser.h>   // for testing only

#include <bsls_alignmentutil.h>
#include <bsls_stopwatch.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>

#include <bsl_cstdlib.h>                         // atoi()
#include <bsl_cstring.h>                         // memcpy(), memset()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The 'bcema_ProtectableblockdispenserAdapter' class consists of
// one constructor, a destructor, and four manipulators.  The manipulators are
// used to allocate, deallocate, and reserve memory.  Since this component is
// a memory manager, the 'bdema_testallocator' component is used extensively
// to verify expected behaviors.  Note that the copying of objects is
// explicitly disallowed since the copy constructor and assignment operator
// are declared 'private' and left unimplemented.  So we are primarily
// concerned that the internal memory management system functions as expected
// and that the manipulators operator correctly.  Note that memory allocation
// must be tested for exception neutrality (also via the 'bdema_testallocator'
// component).  Several small helper functions are also used to facilitate
// testing.
//-----------------------------------------------------------------------------
// [2] bcema_ProtectableblockdispenserAdapter(int              numPools,
//                                         bslma_Allocator *ba = 0);
// [2] ~bcema_ProtectableblockdispenserAdapter();
// [3] void *allocate(int size);
// [4] void deallocate(void *address);
// [5] void release();
// [6] void reserveCapacity(int size, int numObjects);
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [7] USAGE EXAMPLE
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
#define TAB cout << '\t';

//=============================================================================
//                       GLOBAL TYPES AND CONSTANTS
//-----------------------------------------------------------------------------

typedef bcema_ProtectableBlockDispenserAdapter Obj;
typedef bdema_TestProtectableBlockDispenser    TestDisp;

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

class NoopProtectableBlockDispenser :
                                   public bdema_ProtectableBlockDispenser {
        // The NoopProtectableBlockDispenser is an empty implementation of the
        // 'bdema_ProtectableBlockDispenser' interface that tracks the
        // methods called and verifies the 'bdema_ProtectedBlockDispener'
        // protocol.

  private:
    char const **d_lastMethod;  // the last method called on this instance

  public:

    // CREATORS
    NoopProtectableBlockDispenser(char const **lastMethod)
        // Create this 'NoopProtectableBlockDispenser' with the specified
        // 'lastMethod' to store the last method called.
    : d_lastMethod(lastMethod)
    {
        *d_lastMethod = "NoopProtectableBlockDispenser";
    }

    ~NoopProtectableBlockDispenser();
        // Destroy this 'NoopProtectableBlockDispenser'

    // MANIPULATORS
    virtual bdema_MemoryBlockDescriptor allocate(size_type size);
        // This method has no effect for this dispenser except that a
        // subsequent call to 'lastMethod()' will return 'allocate'.

    virtual void deallocate(const bdema_MemoryBlockDescriptor& block);
        // This method has no effect for this dispenser except that a
        // subsequent call to 'lastMethod()' will return 'deallocate'.

    virtual int protect(const bdema_MemoryBlockDescriptor& block);
        // Return '0'. This method has no effect for this dispenser except
        // that a subsequent call to 'lastMethod()' will return 'protect'.

    virtual int unprotect(const bdema_MemoryBlockDescriptor& block);
        // Return '0'. This method has no effect for this dispenser except
        // that a subsequent call to 'lastMethod()' will return 'unprotect'.

    // ACCESSORS
    int minimumBlockSize() const;
        // Return '0'. This method has no effect for this dispenser except
        // that a subsequent call to 'lastMethod()' will return 'blockSize'.

    const char *lastMethod() { return *d_lastMethod; }
        // Return the last method called on this
        // 'NoopProtectableBlockDispenser' instance.
};

NoopProtectableBlockDispenser::~NoopProtectableBlockDispenser()
{
    *d_lastMethod = "~NoopProtectableBlockDispenser";
}

bdema_MemoryBlockDescriptor
NoopProtectableBlockDispenser::allocate(size_type size)
{
    *d_lastMethod = "allocate";
    return bdema_MemoryBlockDescriptor();
}

void NoopProtectableBlockDispenser::deallocate(
                           const bdema_MemoryBlockDescriptor& block)
{
    *d_lastMethod = "deallocate";
}

int NoopProtectableBlockDispenser::protect(
                           const bdema_MemoryBlockDescriptor& block)
{
    *d_lastMethod = "protect";
    return 0;
}

int NoopProtectableBlockDispenser::unprotect(
                           const bdema_MemoryBlockDescriptor& block)
{
    *d_lastMethod = "unprotect";
    return 0;
}

int NoopProtectableBlockDispenser::minimumBlockSize() const
{
    *d_lastMethod = "minimumBlockSize";
    return 0;
}

enum {
    NUM_THREADS = 10
};

struct WorkerArgs {
    Obj       *d_dispenser; // dispenser to perform allocations
    const int *d_sizes;     // array of allocations sizes
    int        d_numSizes;  // number of allocations

};

bcemt_Barrier g_barrier(NUM_THREADS);
extern "C" void *workerThread(void *arg) {
    // Perform a series of allocate, protect, unprotect, and deallocate
    // operations on the 'bcema_ProtectableBlockDispenserAdapter' and verify
    // their results.  This is operation is intended to be a thread entry
    // point.  Cast the specified 'args' to a 'WorkerArgs', and perform a
    // series of '(WorkerArgs *)args->d_numSizes' allocations using the
    // corresponding allocations sizes specifed by
    // '(WorkerARgs *)args->d_sizes'. Protect, unprotect, and finally delete
    // the allocated memory.  Use the barrier 'g_barrier' to ensure tests are
    // performed while the allocator is in the correct state.

    WorkerArgs *args = (WorkerArgs *) arg;
    ASSERT(0 != args);
    ASSERT(0 != args->d_sizes);

    Obj       *dispenser  = args->d_dispenser;
    const int *allocSizes = args->d_sizes;
    const int  numAllocs  = args->d_numSizes;

    bsl::vector<bdema_MemoryBlockDescriptor>
                                           blocks(bslma_Default::allocator(0));

    blocks.resize(numAllocs);

    g_barrier.wait();

    // Perform allocations
    for (int i = 0; i < numAllocs; ++i) {
        blocks[i] = dispenser->allocate(allocSizes[i]);
    }

    // Perform protection operations
    for (int i = 0; i < numAllocs; ++i) {
        dispenser->protect(blocks[i]);
    }

    // Perform unprotect operations
    for (int i = 0; i < numAllocs; ++i) {
        dispenser->unprotect(blocks[i]);
    }

    // deallocate all the blocks
    for (int i = 0; i < numAllocs; ++i) {
        dispenser->deallocate(blocks[i]);
    }

    g_barrier.wait();

    return arg;
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// In the following elaborate usage example, we develop a simple
// 'ThreadEnabledProtectableMultipool' class containing a series of
// thread-enabled pools of memory.  We use a
// 'bcema_ProtectableBlockDispenserAdapter' to synchronize memory
// allocations *across* our thread-enabled pools.  For the purpose of this
// discussion, we first begin by defining a protectable pool:
//..
    // threadenabledprotectablepool.h

    class ThreadEnabledProtectablePool {
        // This class defines a thread-enabled protectable pool that dispenses
        // memory blocks of a fixed size, specified at construction.

        // PRIVATE TYPES
        struct Link {
            // This 'struct' holds a pointer to the next free memory
            // block in the free memory list.

            Link *d_next_p;
        };

        // DATA
        const int                   d_objectSize;  // size of each individual
                                                   // memory block

        int                         d_numPooledObjects;
                                                   // number of pooled objects
                                                   // both in use and on the
                                                   // free list

        Link                       *d_freeList_p;  // the list of free blocks

        bdema_ProtectableBlockList  d_blockList;   // supplies free memory

        bcemt_Mutex                 d_mutex;       // synchronize access to
                                                   // non-const data

      private:
        // PRIVATE MANIPULATORS
        void replenish();
            // Replenish the free memory list of this pool.

      private:
        // NOT IMPLEMENTED
        ThreadEnabledProtectablePool(const ThreadEnabledProtectablePool&);
        ThreadEnabledProtectablePool& operator=(
                                         const ThreadEnabledProtectablePool&);

      public:
        // CREATORS
        ThreadEnabledProtectablePool(
                               int                              objectSize,
                               bdema_ProtectableBlockDispenser *dispenser = 0);
            // Create a ThreadEnabledProtectablePool that will return memory
            // blocks of the specified 'objectSize' for each 'allocate' method
            // invocation.  Use the optionally specified 'dispenser' to
            // supply protectable memory.  If 'dispensper' is 0, the
            // 'bdema_NativeProtectableBlockDispenser::singleton()' will be
            // used.

        ~ThreadEnabledProtectablePool();
            // Destroy this protectable pool of memory and release its
            // contents.

        // MANIPULTORS
        void *allocate();
            // Allocate a memory block of 'objectSize' specified at
            // construction.

        void deallocate(void *address);
            // Deallocate a memory block back to this pool for reuse.

        void protect();
            // Set the access protection for the memory managed by this pool
            // to READ-ONLY.  After this call returns, attempts to
            // modify the memory managed by this object will result in a
            // memory fault.

        void unprotect();
            // Set the access protection for the memory managed by this pool
            // to READ-WRITE.  After this call returns, attempts to
            // write to the memory managed by this object will result in a
            // memory fault.

        // ACCESSORS
        int objectSize() const;
            // Return the fixed allocation size of this pool.  This value is
            // equal to the 'objectSize' supplied at construction rounded up
            // to the next multiple of
            // 'bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT' (in order to ensure
            // aligned allocations).
    };

    // threadenabledprotectablepool.cpp
    inline static int roundUp(int x, int y)
        // Round up the specified 'x' to the nearest whole integer multiple of
        // the specified 'y'.  The behavior is undefined unless
        // 0 <= x and 0 < y;
    {
        return (x + y - 1) / y * y;
    }

    void ThreadEnabledProtectablePool::replenish()
    {
        int numObjects = d_numPooledObjects ? d_numPooledObjects : 1;
        bdema_MemoryBlockDescriptor block =
                               d_blockList.allocate(d_objectSize * numObjects);

        // Compute the actual number of objects that will fit in the allocated
        // memory (the block list may return more memory than requested).
        int actualNumObjects = block.size() / d_objectSize;

        char *start = static_cast<char *>(block.address());
        char *last  = start + (actualNumObjects - 1) * d_objectSize;
        for (char *p = start; p < last; p += d_objectSize) {
            ((Link *) p)->d_next_p = (Link *) (p + d_objectSize);
        }
        ((Link *) last)->d_next_p = d_freeList_p;
        d_freeList_p = (Link *)start;

        d_numPooledObjects += actualNumObjects;
    }

    // CREATORS
    ThreadEnabledProtectablePool::ThreadEnabledProtectablePool(
                                 int                              objectSize,
                                 bdema_ProtectableBlockDispenser *dispenser)

    : d_objectSize(roundUp(objectSize, bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT))
    , d_numPooledObjects(0)
    , d_freeList_p(0)
    , d_blockList(dispenser)
    {
    }

    ThreadEnabledProtectablePool::~ThreadEnabledProtectablePool()
    {
    }

    // MANIPULTORS
    void *ThreadEnabledProtectablePool::allocate()
    {
        bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
        if (!d_freeList_p) {
            replenish();
        }
        Link *link   = d_freeList_p;
        d_freeList_p = link->d_next_p;
        return link;
    }

    void ThreadEnabledProtectablePool::deallocate(void *address)
    {
        bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
        Link *link     = static_cast<Link *>(address);
        link->d_next_p = d_freeList_p;
        d_freeList_p   = link;
    }

    void ThreadEnabledProtectablePool::protect()
    {
        d_blockList.protect();
    }

    void ThreadEnabledProtectablePool::unprotect()
    {
        d_blockList.unprotect();
    }

    // ACCESSORS
    int ThreadEnabledProtectablePool::objectSize() const
    {
        return d_objectSize;
    }

//..
// We use this thread-enabled protectable pool to create a thread-enabled
// protectable multipool; however, we use the
// 'bcema_ProtectableBlockDispenserAdapter' to prevent our (thread-enabled)
// pools from attempting simultaneous memory allocations from our, not
// necessarily thread-safe, 'bdema_ProtectableBlockDispenser'.  Note that, the
// 'ThreadEnabledProtectableMultipool' is optionally supplied both an allocator
// and a dispenser at construction.  The supplied dispenser provides the
// protectable memory dispensed to clients of the multipool.  The supplied
// allocator is used efficiently allocate the internal members 'd_pools_p' and
// 'd_poolSizes' (i.e. it avoids allocating an entire protectable page for the
// relatively small internal data members).
//..
    // threadenabledprotectablemultipool.h

    class ThreadEnabledProtectableMultipool {
        // This class implements a thread-enabled mechanism for managing a set
        // of memory pools, each dispensing fixed-sized memory blocks of
        // a size supplied at construction.  The 'bcema_Multipool' 'allocate'
        // ('deallocate') method allocates memory from (returns memory to) the
        // internal pool that manages memory of the smallest size that
        // satisfies the requested size.  The
        // 'ThreadEnabledProtectableMultipool'  is optionally
        // supplied both a 'bdema_ProtectableBlockDispenser' and a
        // 'bslma_Allocator' at construction.  The protectable block dispenser
        // provides protectable memory that is dispensed to clients of this
        // multipool on calls to the 'allocate' operation; the allocator is
        // used to provide (non-protectable) memory for internal use (the
        // 'd_poolsSizes' vector and 'd_pools_p' array are allocated with the
        // allocator).

        // PRIVATE TYPES
       union Header {
           // Stores the pool number of this item.

           int                                d_pool;   // pool index
           bsls_AlignmentUtil::MaxAlignedType d_dummy;  // force alignment
        };

        // DATA
        bcemt_Mutex               d_mutex;       // synchronize access to data

        bcema_ProtectableBlockDispenserAdapter
                                  d_dispenser;   // adapter for dispenser,
                                                 // allocates protected memory

        const int                 d_numPools;    // number of pools

        bsl::vector<int>          d_poolSizes;   // sorted array of pool
                                                 // object sizes
        ThreadEnabledProtectablePool
                                 *d_pools_p;     // array of pools
                                                 // corresponding to
                                                 // d_poolSizes

        bslma_Allocator          *d_allocator_p; // allocate 'd_pools_p' array
                                                 // and 'd_poolSizes'
      private:
        // PRIVATE MANIPULATORS
        void init(int *poolSizes);
            // Initialize the internal structures 'd_pools_p' and
            // 'd_poolSizes' to hold 'd_numPools' memory pools of the
            // specified 'poolSizes'.

      private:
        // NOT IMPLEMENTED
        ThreadEnabledProtectableMultipool(
                                     const ThreadEnabledProtectableMultipool&);
        ThreadEnabledProtectableMultipool& operator=(
                                     const ThreadEnabledProtectableMultipool&);

      public:
        // CREATORS
        ThreadEnabledProtectableMultipool(
                          int                              numMemoryPools,
                          int                             *poolSizes,
                          bslma_Allocator                 *basicAllocator = 0);
        ThreadEnabledProtectableMultipool(
                          int                              numMemoryPools,
                          int                             *poolSizes,
                          bdema_ProtectableBlockDispenser *dispenser,
                          bslma_Allocator                 *basicAllocator = 0);
            // Create a multipool having the specified 'numMemoryPools'
            // dispensing fixed size blocks of the specified 'poolSizes'.
            // Optionally specify a 'dispenser' used to supply protectable
            // memory to clients via the 'allocate' method.  If 'dispenser' is
            // not specified or 0, the
            // 'bdema_NativeProtectableMemoryBlockDispenser::singleton()' is
            // used.  Optionally specify a 'basicAllocator' used to supply
            // non-managed memory for internal use by this multipool.  If
            // 'basicAllocator' is 0, the currently installed
            // default allocator is used.  The 'i'th pool, for 'i' in
            // '[ 0 .. numMemoryPools - 1 ]' manages blocks of size
            // 'poolSizes[i]' rounded up to the nearest multiple of
            // 'bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT'.  The behavior is
            // undefined unless '1 <= numMemoryPools', and 'poolSizes' has
            // 'numMemoryPools' elements, none of which is 0.  Note that the
            // indicated 'basicAllocator' is used to supply memory for
            // internal data, where as the 'dispenser' is used to supply
            // managed memory to clients.

        ~ThreadEnabledProtectableMultipool();
            // Destroy this object and release all memory managed by it.

         // MANIPULATORS
         void *allocate(int numBytes);
            // Return a newly allocated block of memory of (at least) the
            // specified positive 'numBytes'.  If 'numBytes' is 0 or if
            // 'numBytes' is larger that the maximum pooled object size
            // supplied at construction, a null pointer is returned with no
            // affect.  The behavior is undefined unless  '0 <= size' and this
            // multipool object is in the unprotected state.  Note that the
            // alignment of the address returned is the maximum alignment for
            // any fundamental type defined for this platform.

         void deallocate(void *address);
            // Deallocate the memory block at the specified 'address' back to
            // this multipool for reuse.  The behavior is undefined unless
            // 'address' was allocated from this multipool object, has not
            // since been deallocated, and this multipool object is in the
            // unprotected state.

         void protect();
            // Set the access protection for the memory managed by this pool
            // to READ-ONLY.  After this call returns, attempts to
            // modify the memory managed by this object will result in a
            // memory fault.

         void unprotect();
            // Set the access protection for the memory managed by this pool
            // to READ-WRITE.  After this call returns, attempts to
            // write to the memory managed by this object will *not* result in
            // a memory fault.
    };

    // threadenabledprotectablemultipool.h

    // PRIVATE MANIPULATORS
    void ThreadEnabledProtectableMultipool::init(int *poolSizes)
    {
        for (int i = 0; i < d_numPools; ++i) {
            d_poolSizes.push_back(poolSizes[i]);
        }

        // Sort the pool sizes so that 'allocate()' can use a binary search to
        // find the appropriate pool.
        bsl::sort(d_poolSizes.begin(), d_poolSizes.end());

        d_pools_p = (ThreadEnabledProtectablePool *)
                     d_allocator_p->allocate(d_numPools * sizeof(*d_pools_p));

        bslma_DeallocatorProctor<bslma_Allocator> autoPoolsDeallocator(
                                                     d_pools_p, d_allocator_p);

        int size = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;
        bslma_AutoDestructor<ThreadEnabledProtectablePool>
                                                       autoDtor(d_pools_p, 0);
        for (int i = 0; i < d_numPools; ++i, ++autoDtor) {
            new (d_pools_p + i) ThreadEnabledProtectablePool(
                                                poolSizes[i] + sizeof(Header),
                                                &d_dispenser);
        }
        autoDtor.release();
        autoPoolsDeallocator.release();
    }

    // CREATORS
    ThreadEnabledProtectableMultipool::ThreadEnabledProtectableMultipool(
                          int                              numMemoryPools,
                          int                             *poolSizes,
                          bslma_Allocator                 *basicAllocator)
    : d_dispenser(&d_mutex, 0)
    , d_pools_p(0)
    , d_poolSizes(basicAllocator)
    , d_numPools(numMemoryPools)
    , d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
        init(poolSizes);
    }

    ThreadEnabledProtectableMultipool::ThreadEnabledProtectableMultipool(
                          int                              numMemoryPools,
                          int                             *poolSizes,
                          bdema_ProtectableBlockDispenser *dispenser,
                          bslma_Allocator                 *basicAllocator)
    : d_dispenser(&d_mutex, dispenser)
    , d_pools_p(0)
    , d_poolSizes(basicAllocator)
    , d_numPools(numMemoryPools)
    , d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
        init(poolSizes);
    }

    ThreadEnabledProtectableMultipool::~ThreadEnabledProtectableMultipool()
    {
        for (int i = 0; i < d_numPools; ++i) {
            d_pools_p[i].~ThreadEnabledProtectablePool();
        }
        d_allocator_p->deallocate(d_pools_p);
    }

    // MANIPULATORS
    void *ThreadEnabledProtectableMultipool::allocate(int numBytes)
    {
        if (0 == numBytes) {
            return 0;
        }

        // Perform a binary search to find the appropriately sized pool.
        const int *pool = bsl::lower_bound(d_poolSizes.begin(),
                                           d_poolSizes.end(),
                                           numBytes);
        if (pool >= d_poolSizes.end()) {

            // There is no pool that can supply 'numBytes'.
            return 0;
        }

        // The index in the 'd_poolSizes' vector corresponds to the index in
        // 'd_pools_p.

        int index = pool - d_poolSizes.begin();
        Header *block = (Header *)d_pools_p[index].allocate();
        block->d_pool = index;
        return block + 1;
    }

    void ThreadEnabledProtectableMultipool::deallocate(void *address)
    {
        Header *header = (Header *)address - 1;
        d_pools_p[header->d_pool].deallocate(header);
    }

    void ThreadEnabledProtectableMultipool::protect()
    {
        for (int i = 0; i < d_numPools; ++i) {
            d_pools_p[i].protect();
        }
    }

    void ThreadEnabledProtectableMultipool::unprotect()
    {
        for (int i = 0; i < d_numPools; ++i) {
            d_pools_p[i].unprotect();
        }
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
      case 3: {
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

        const int BLOCKSIZE = 2 * TestDisp::BDEMA_DEFAULT_PAGE_SIZE;
        const int ALIGN     = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;
        const int HEADER = bdema_ProtectableBlockList::blockHeaderSize();

        {
            TestDisp TD(BLOCKSIZE, veryVeryVerbose);

            ThreadEnabledProtectablePool pool(1, &TD);
            LOOP_ASSERT( pool.objectSize(),  ALIGN == pool.objectSize());

            char *lastMemory = 0;
            int numObjects = (BLOCKSIZE - HEADER) / ALIGN;
            for (int i = 0; i < numObjects ; ++i) {
                char *buffer = (char *)pool.allocate();
                if (lastMemory) {
                    ASSERT(buffer == lastMemory + ALIGN);
                }
                ASSERT(BLOCKSIZE == TD.numBytesInUse());
                ASSERT(1         == TD.numBlocksInUse());
                lastMemory = buffer;
            }
            pool.deallocate(lastMemory);
            lastMemory = (char *)pool.allocate();
            pool.deallocate(lastMemory);
            lastMemory = (char *)pool.allocate();
            ASSERT(BLOCKSIZE == TD.numBytesInUse());
            ASSERT(1         == TD.numBlocksInUse());

            pool.allocate();
            ASSERT(2 * BLOCKSIZE == TD.numBytesInUse());
            ASSERT(2             == TD.numBlocksInUse());
        }
        {
            TestDisp TD(2 * BLOCKSIZE, veryVeryVerbose);

            int sizes[] = {1, 2 * ALIGN, 4 * ALIGN};
            ThreadEnabledProtectableMultipool pool(3, sizes, &TD, Z);
            char *buff1 = (char *)pool.allocate(1);
            ASSERT(1 == TD.numBlocksInUse());

            char *buff2 = (char *)pool.allocate(2 * ALIGN);
            ASSERT(2 == TD.numBlocksInUse());

            char *buff3 = (char *)pool.allocate(2* ALIGN + 1);
            ASSERT(3 == TD.numBlocksInUse());

            char *buff4 = (char *)pool.allocate(4 * ALIGN - 1);
            ASSERT(3 == TD.numBlocksInUse());

            char *buff5 = (char *)pool.allocate(4 * ALIGN);
            ASSERT(3 == TD.numBlocksInUse());

            char *buff6 = (char *)pool.allocate(4 * ALIGN + 1);
            ASSERT(0 == buff6);

            for (int i = 0; i < 2 * BLOCKSIZE / ALIGN + 1; ++i) {
                pool.deallocate(buff1);
                buff1 = (char *)pool.allocate(1);
                ASSERT(3 == TD.numBlocksInUse());
            }

            for (int i = 0; i < 2 * BLOCKSIZE / ALIGN + 1; ++i) {
                pool.deallocate(buff2);
                buff2 = (char *)pool.allocate(2 * ALIGN);
                ASSERT(3 == TD.numBlocksInUse());
            }

            for (int i = 0; i < 2 * BLOCKSIZE / ALIGN + 1; ++i) {
                pool.deallocate(buff5);
                buff5 = (char *)pool.allocate(4 * ALIGN);
                ASSERT(3 == TD.numBlocksInUse());
            }
        }

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST
        //
        // Testing:
        //     Thread-safety of allocate/deallocate methods.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TEST CONCURRENCY" << endl
                                  << "================" << endl;
        bcemt_ThreadUtil::Handle threads[NUM_THREADS];

        const int   BLOCKSIZE = TestDisp::BDEMA_DEFAULT_PAGE_SIZE;
        const int   ALIGN     = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;
        TestDisp    dispenser(BLOCKSIZE, false);
        bcemt_Mutex mutex;
        Obj         mX(&mutex, &dispenser);

        const int SIZES [] = { 1 , 2 , 4,  8, 16, 32, 64, 128, 256, 512,
                               1 , 2 , 4,  8, 16, 32, 64, 128, 256, 512};

        const int NUM_SIZES = sizeof (SIZES) / sizeof(*SIZES);

        WorkerArgs args;
        args.d_dispenser = &mX;
        args.d_sizes     = (int *)&SIZES;
        args.d_numSizes  = NUM_SIZES;

        for (int i = 0; i < NUM_THREADS; ++i) {
            int rc =
                bcemt_ThreadUtil::create(&threads[i], workerThread, &args);
            LOOP_ASSERT(i, 0 == rc);
        }
        for (int i = 0; i < NUM_THREADS; ++i) {
            int rc =
                bcemt_ThreadUtil::join(threads[i]);
            LOOP_ASSERT(i, 0 == rc);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We are concerned that the basic functionality of
        //   'bcema_ProtectableblockdispenserAdapter' works
        //   properly.
        //
        // Plan:
        //   Create a NoopProtectableBlockDispenser and supply it to a
        //   'bcema_ProtectableBlockDispenser' under test.  Verify that
        //   operatiosn on the dispenser are delegated to the noop protectable
        //   block dispenser.
        //
        // Testing:
        //   This "test" exercises basic functionality, but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Breathing Test"
                          << endl << "==============" << endl;

        const char *lastMethod = 0;
        bcemt_Mutex mutex;
        NoopProtectableBlockDispenser noopDispenser =
                                    NoopProtectableBlockDispenser(&lastMethod);
        {
            bcema_ProtectableBlockDispenserAdapter mX(&mutex, &noopDispenser);

            ASSERT( 0 == bsl::strcmp("NoopProtectableBlockDispenser",
                                     lastMethod));

            mX.minimumBlockSize();
            ASSERT(0 == bsl::strcmp("minimumBlockSize", lastMethod));

            bdema_MemoryBlockDescriptor desc;

            desc = mX.allocate(1);
            ASSERT(0 == bsl::strcmp("allocate", lastMethod));

            mX.protect(desc);
            ASSERT(0 == bsl::strcmp("protect", lastMethod));

            mX.unprotect(desc);
            ASSERT(0 == bsl::strcmp("unprotect", lastMethod));

            mX.deallocate(desc);
            ASSERT(0 == bsl::strcmp("deallocate", lastMethod));
        }
        ASSERT(0 == bsl::strcmp("deallocate", lastMethod));
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
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
