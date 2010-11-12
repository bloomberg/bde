// bcema_protectablemultipoolallocator.t.cpp                         -*-C++-*-

#include <bcema_protectablemultipoolallocator.h>

#include <bslma_testallocatorexception.h>         // for testing only
#include <bdema_testprotectableblockdispenser.h>  // for testing only

#include <bsl_cstdlib.h>                         // atoi()
#include <bsl_cstring.h>                         // memcpy(), memset()

#include <bsl_iostream.h>
#include <bsl_c_signal.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The 'bcema_ProtectableMultipoolAllocator' class consists of one
// constructor, a destructor, and six manipulators.  The manipulators are used
// to allocate, deallocate, reserve, and protect memory.  Since this component
// is a memory manager, the 'bdema_testallocator' component is used
// extensively to verify expected behaviors.  Note that the copying of objects
// is explicitly disallowed since the copy constructor and assignment operator
// are declared 'private' and left unimplemented.  So we are primarily
// concerned that the internal memory management system functions as expected
// and that the manipulators operator correctly.  Note that memory allocation
// must be tested for exception neutrality (also via the 'bdema_testallocator'
// component).  Several small helper functions are also used to facilitate
// testing.
//-----------------------------------------------------------------------------
// [ 2] Test Helper Functions
// [ 3] bcema_ProtectableMultipoolAllocator(
//                            int                                    numPools,
//                            bcema_ProtectableBlockDispenser *ba);
// [ 3] ~bcema_ProtectableMultipoolAllocator();
// [ 4] void *allocate(int size);
// [ 6] void deallocate(void *address);
// [ 7] void release();
// [ 8] void reserveCapacity(int size, int numObjects);
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
#define TAB cout << '\t';

//=============================================================================
//                  STANDARD BDEMA EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BDEMA_EXCEPTION_TEST {                                      \
    {                                                                     \
        static int firstTime = 1;                                         \
        if (veryVerbose && firstTime) cout <<                             \
            "### BDEMA EXCEPTION TEST -- (ENABLED) --" << endl;           \
        firstTime = 0;                                                    \
    }                                                                     \
    if (veryVeryVerbose) cout <<                                          \
        "### Begin bdema exception test." << endl;                        \
    int bdemaExceptionCounter = 0;                                        \
    static int bdemaExceptionLimit = 100;                                 \
    testDispenser.setAllocationLimit(bdemaExceptionCounter);              \
    do {                                                                  \
        try {

#define END_BDEMA_EXCEPTION_TEST                                          \
        } catch (bslma_TestAllocatorException& e) {                       \
            if (veryVerbose && bdemaExceptionLimit || veryVeryVerbose) {  \
                --bdemaExceptionLimit;                                    \
                cout << "(*** " << bdemaExceptionCounter << ')';          \
                if (veryVeryVerbose) { cout << " BEDMA_EXCEPTION: "       \
                    << "alloc limit = " << bdemaExceptionCounter << ", "  \
                    << "last alloc size = " << e.numBytes();              \
                }                                                         \
                else if (0 == bdemaExceptionLimit) {                      \
                    cout << " [ Note: 'bdemaExceptionLimit' reached. ]";  \
                }                                                         \
                cout << endl;                                             \
            }                                                             \
            testDispenser.setAllocationLimit(++bdemaExceptionCounter);    \
            continue;                                                     \
        }                                                                 \
        testDispenser.setAllocationLimit(-1);                             \
        break;                                                            \
    } while (1);                                                          \
    if (veryVeryVerbose) cout <<                                          \
        "### End bdema exception test." << endl;                          \
}
#else
#define BEGIN_BDEMA_EXCEPTION_TEST                                        \
{                                                                         \
    static int firstTime = 1;                                             \
    if (verbose && firstTime) { cout <<                                   \
        "### BDEMA EXCEPTION TEST -- (NOT ENABLED) --" << endl;           \
        firstTime = 0;                                                    \
    }                                                                     \
}
#define END_BDEMA_EXCEPTION_TEST
#endif

//=============================================================================
//                       GLOBAL TYPES AND CONSTANTS
//-----------------------------------------------------------------------------

typedef bcema_ProtectableMultipoolAllocator Obj;
typedef bdema_ProtectableBlockList          PBList;
typedef bdema_ProtectableBlockDispenser     PBDisp;
typedef bdema_TestProtectableBlockDispenser TestDisp;

const int MAX_ALIGN = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;

// Warning: keep this in sync with bdema_Multipool.h!
struct Header {
    // Stores pool number of this item.
    union {
        int                                d_pool;   // pool for this item
        bsls_AlignmentUtil::MaxAlignedType d_dummy;  // force maximum alignment
    } d_header;
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

//---- Global variables used by the segmentation fault handler -------- //
bool   g_inTest = false;   // whether we are in a test
bool   g_fault  = false;   // whether a fault has occurred
Obj   *g_testingAlloc = 0; // A global variable that must refer to the
// ----------------------------------------------------------- //

extern "C" {

void segfaultHandler(int x)
// This is a segmentation fault signal handler.  It uses the global variables
// above to manage it's state.  Mark that a segmentation fault has occurred in
// g_fault and, if protection is under test, unprotect the memory pointed to by
// the allocator under test.
{
    g_fault = true;
    if (g_inTest) {
        g_testingAlloc->unprotect();

    }
}

}

int testProtectedSet(Obj *testAlloc, char *data, char val)
// Assign to the specified 'data' the specified 'val' which was allocated used
// the specified 'testAlloc'.  Return 0 and updated 'data' if protection on
// that memory worked correctly, return non-zero if memory protection did not
// work correct.  This function tests a single assignment of (theoretically)
// protected memory.
{

    // install the signal handler and initialize the global variables used by
    // the signal handler.
    g_fault        = false;
    g_inTest       = true;
    g_testingAlloc = testAlloc;

    signal(SIGSEGV, segfaultHandler);

    // protect the memory
    g_testingAlloc->protect();

    // attempt an assignment.  The segFaultHandler should be called, which will
    // in turn, unprotect the memory.
    *data = val;

    // verify that the segmentation fault handler was called and reset the
    // global segmentation fault variables
    g_testingAlloc = NULL;
    g_inTest       = false;
    signal(SIGSEGV, SIG_DFL);
    return (g_fault) ? 0 : 1;
}

struct PtrComparator
// A pointer comparator used for creating maps of addresses
{
  bool operator()(const void* a, const void* b) const
  {
      return a < b;
  }
};

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

    int ACTUAL_PG_SIZE =
                       bdema_NativeProtectableBlockDispenser::pageSize();
    int PG_SIZE        = TestDisp::BDEMA_DEFAULT_PAGE_SIZE;
    int HEADER_SIZE    = PBList::blockHeaderSize();

    TestDisp  testDispenser(PG_SIZE, veryVeryVerbose);

    switch (test) { case 0:
      case 9: {
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
        if (verbose) cout << endl << "USAGE EXAMPLE"
                          << endl << "============="
                          << endl;
///Usage
///-----
// The following snippets of code depict basic usage of 'bcema_Multipool'.
// The example structures are contrived to illustrate how allocation requests
// of a given size are distributed among the pools managed by a multipool.
// First define three objects of geometrically increasing size:
//..
//     #include <bsls_alignmentutil.h>

        const int MIN_SIZE = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;

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
//..
// Next create a 'bcema_ProtectableMultipoolAllocator' that manages three
// pools:
//..
        const int NUM_POOLS = 3;
        bcema_ProtectableMultipoolAllocator mp(NUM_POOLS);
//..
// Reserve capacity in the three pools so that each has sufficient storage
// to allocate *at* *least* eight objects before they *may* need to be
// replenished:
//..
        const int NUM_OBJS = 8;
        int objSize        = MIN_SIZE;
        for (int i = 0; i < NUM_POOLS - 1; ++i) {
            mp.reserveCapacity(objSize, NUM_OBJS);
            objSize *= 2;
        }
//..
// Allocate an object from each of the three pools:
//..
        Small  *pS = new(mp.allocate(sizeof(Small)))  Small;    ASSERT(pS);
        Medium *pM = new(mp.allocate(sizeof(Medium))) Medium;   ASSERT(pM);
        Large  *pL = new(mp.allocate(sizeof(Large)))  Large;    ASSERT(pL);
//..
// Allocate a buffer from the "overflow" pool.  All objects whose sizes exceed
// MIN_SIZE * 2 ^ (NUM_POOLS - 1) are allocated from this "overflow" pool:
//..
        char *pBuf = (char *) mp.allocate(5 * MIN_SIZE);        ASSERT(pBuf);
//..
// Protect the allocated memory from accidental writes using the 'protect'
// operation.  Once the memory is in the protected state, attempting to modify
// it will cause a memory fault.
//..
        mp.protect();

        // pBuf[0] = 0;  // SEGMENTATION FAULT!
//..
// Unprotect the allocated memory so that it can be modified.
//..
        mp.unprotect();

        pBuf[0] = 0;     // OK!

//..
// Explicitly deallocate two of the objects previously allocated:
//..
        mp.deallocate(pS);
        mp.deallocate(pL);
        // 'pS' and 'pL' are no longer valid addresses.
//..
// Deallocate all remaining outstanding objects:
//..
        mp.release();
        // Now 'pM' and 'pBuf' are also invalid addresses.
//..

      } break;
      case 8: {
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

            const int OVER_SIZE = MAX_ALIGN * 5; // Too big to reserve

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int NE = DATA[ti];
                if (veryVerbose) { cout << "\t\t"; P(NE); }

                for (int zi = 0; zi < NUM_OSIZE; ++zi) {
                    const int OBJ_SIZE = OSIZE[zi];
                    for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                      BEGIN_BDEMA_EXCEPTION_TEST {
                        Obj mX(NUM_POOLS, &testDispenser);
                        stretchRemoveAll(&mX, EXTEND[ei], OBJ_SIZE);
                        mX.reserveCapacity(OBJ_SIZE, 0);
                        mX.reserveCapacity(OBJ_SIZE, NE);
                        const int NUM_BYTES  = testDispenser.numBytesInUse();
                        for (int i = 0; i < NE; ++i) {
                            mX.allocate(OBJ_SIZE);
                        }
                        LOOP3_ASSERT(ti, ei, zi, NUM_BYTES  ==
                                                testDispenser.numBytesInUse());
                      } END_BDEMA_EXCEPTION_TEST
                    }
                }
            }
        }
      } break;
      case 7: {
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

        const int ODATA [] = { 1 , 2 , 4,  8, 16, 32, 64, 128, 256};
        const int NUM_ODATA = sizeof(ODATA)/sizeof(*ODATA);

        for (int i = 1; i <= MAX_POOLS; ++i) {
            if (veryVerbose) { TAB; cout << "# pools: "; P(i); }
            Obj mX(i, &testDispenser);
            const int NUM_BLOCKS = testDispenser.numBlocksInUse();
            const int NUM_BYTES  = testDispenser.numBytesInUse();
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
                                               testDispenser.numBlocksInUse());
                LOOP2_ASSERT(i, its, NUM_BYTES  ==
                                               testDispenser.numBytesInUse());
            }
            // Verify that the structure is still valid.
            for (int objI = 0; objI < NUM_ODATA; ++objI) {
                char *p = (char *)mX.allocate(ODATA[objI]);
                const int pCalculatedPool = calcPool(i, ODATA[objI]);
                const int pRecordedPool   = recPool(p);
                ASSERT(pCalculatedPool == pRecordedPool);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING DEALLOCATE METHOD
        //
        // Concerns:
        //   Our primary concerns are that 'deallocate' returns the block of
        //   memory to the underlying pool making it available for future
        //   allocation and that it behaves correctly on a protected allocator.
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
            if (veryVerbose) { TAB; cout << "# pools: "; P(i); }
            Obj mX(i, &testDispenser);
            for (int j = 0; j < i; ++j) {
                if (veryVerbose) { TAB; TAB; cout << "pool: "; P(j); }
                const int OBJ_SIZE = POOL_QUANTA[j];
                int its            = NITERS;
                int firstTime      = 1;
                while (its-- > 0) {  // exercise each pool
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    LOOP2_ASSERT(i, j, p);
                    if (firstTime) {
                        numBytes  = testDispenser.numBytesInUse();
                        firstTime = 0;
                    }
                    else {
                        LOOP2_ASSERT(i, j, numBytes  ==
                                                testDispenser.numBytesInUse());
                    }
                    scribble(p, OBJ_SIZE);
                    mX.deallocate(p);
                }
            }

            if (veryVerbose) { TAB; TAB; cout << "overflow\n"; }
            int its       = NITERS;
            int firstTime = 1;
            while (its-- > 0) {  // exercise "overflow" pool
                char *p = (char *) mX.allocate(OVERFLOW_SIZE);
                LOOP2_ASSERT(i, its, p);
                if (firstTime) {
                    numBlocks = testDispenser.numBlocksInUse();
                    numBytes  = testDispenser.numBytesInUse();
                    firstTime = 0;
                }
                else {
                    LOOP2_ASSERT(i, its, numBlocks ==
                                               testDispenser.numBlocksInUse());
                    LOOP2_ASSERT(i, its, numBytes ==
                                                testDispenser.numBytesInUse());
                }
                scribble(p, OVERFLOW_SIZE);
                mX.deallocate(p);
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING MODIFIERS: protect/unprotect
        //
        // Concerns:
        //   We have the following concerns:
        //    1) That protect and unprotect call the underlying allocators
        //       protect and unprotect on all the blocks they manage.
        //    2) That the underlying protect and unprotect are called with
        //       the correct sizes.
        //
        // Plan:
        //   Create multi-pools that manage a varying number of pools and
        //   allocate objects of varying sizes from each of the pools
        //   (including the "overflow" pool).  As a simple sanity test,
        //   scribble over the extent of all allocated objects.
        //
        //   Then protect and then unprotect and then protect again the memory
        //   under management.  The TestProtectedBlockAllocator can be used to
        //   verify that the protection operations were successful.
        //
        // Testing:
        //   int protect();
        //   int unprotect();
        //   bool isProtected() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'protect' and 'unprotect'"
                          << endl << "================================="
                          << endl;

        {
            if (veryVerbose)
                bsl::cout << "\t\tverify various protect/unprotect calls"
                          << bsl::endl;

            // number of pools to manage
            const int PDATA[]   = { 1, 2, 3, 4, 5 };
            const int NUM_PDATA = sizeof PDATA / sizeof *PDATA;

            // sizes of objects to allocate
            const int ODATA[]   = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };
            const int NUM_ODATA = sizeof ODATA / sizeof *ODATA;

            for (int i = 0; i < NUM_PDATA; ++i) {
                const int NUM_POOLS = PDATA[i];
                Obj mX(NUM_POOLS, &testDispenser);

                for (int j = 0; j < NUM_ODATA; ++j) {

                    for (int k = -1; k <= 1; ++k) {
                        const int OBJ_SIZE = ODATA[j] + k;
                        char *p = (char *) mX.allocate(OBJ_SIZE);
                    }
                }

                TestDisp &ta = testDispenser;
                ASSERT(0 < ta.numBytesInUse());
                ASSERT(0 < ta.numBlocksInUse());

                ASSERT(false == mX.isProtected());
                ASSERT(0 == ta.numBlocksProtected());

                mX.protect();
                ASSERT(true == mX.isProtected());
                ASSERT(ta.numBlocksInUse() == ta.numBlocksProtected());

                mX.protect();
                ASSERT(true == mX.isProtected());
                ASSERT(ta.numBlocksInUse() == ta.numBlocksProtected());

                mX.unprotect();
                ASSERT(false == mX.isProtected());
                ASSERT(0 == ta.numBlocksProtected());

                mX.unprotect();
                ASSERT(false == mX.isProtected());
                ASSERT(0 == ta.numBlocksProtected());

                mX.protect();
                ASSERT(true == mX.isProtected());
                ASSERT(ta.numBlocksInUse() == ta.numBlocksProtected());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATE METHOD
        //
        // Concerns:
        //   We have the following concerns:
        //    1) the memory returned by 'allocate' is properly aligned.
        //    2) the memory returned by 'allocate' is of sufficient size.
        //    3) the allocation request is distributed to the proper underlying
        //       pool (including the "overflow" pool).
        //    4) that allocating from a protected pool will not cause a memory
        //       error and will return protected memory.
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
        //   Concern 4 is addressed by allocating a test set from a protected
        //   multi-pool and then verifying that there was no memory errors and
        //   that all the allocated memory is protected.
        //
        // Testing:
        //   void *allocate(int size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'allocate'"
                          << endl << "==================" << endl;

        if (verbose) cout << "\tTesting 'allocate(0)'." << endl;
        {
            const int MAX_POOLS = 3;
            int i;

            for (i = 1; i <= MAX_POOLS; ++i) {
                if (veryVerbose) { TAB; cout << "# pools: "; P(i); }
                Obj mX(i, &testDispenser);
                char *p = (char *) mX.allocate(0);
                LOOP_ASSERT(i, 0 == p);
            }
        }

        // number of pools to manage
        const int PDATA[]   = { 1, 2, 3, 4, 5 };
        const int NUM_PDATA = sizeof PDATA / sizeof *PDATA;

        // sizes of objects to allocate
        const int ODATA[]   = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };
        const int NUM_ODATA = sizeof ODATA / sizeof *ODATA;

        {
            for (int i = 0; i < NUM_PDATA; ++i) {
                const int NUM_POOLS = PDATA[i];
                if (veryVerbose) { P(NUM_POOLS); }

                Obj mX(NUM_POOLS, &testDispenser);

                for (int j = 0; j < NUM_ODATA; ++j) {

                     for (int k = -1; k <= 1; ++k) {
                        const int OBJ_SIZE = ODATA[j] + k;
                        if (0 == OBJ_SIZE) {
                            continue;
                        }
                        if (veryVerbose) { TAB; P(OBJ_SIZE); }

                        char *p = (char *) mX.allocate(OBJ_SIZE);
                        LOOP3_ASSERT(i, j, k, p);
                        LOOP3_ASSERT(
                                i,
                                j,
                                k,
                                0 == bsls_PlatformUtil::IntPtr(p) % MAX_ALIGN);

                        scribble(p, OBJ_SIZE);
                        const int pCalculatedPool =
                                                 calcPool(NUM_POOLS, OBJ_SIZE);
                        const int pRecordedPool   = recPool(p);
                        if (veryVerbose) {
                            TAB; P_((void *)p);
                            P_(pCalculatedPool); P(pRecordedPool);
                        }
                        LOOP3_ASSERT(i, j, k, pCalculatedPool ==
                                                                pRecordedPool);

                        char *q = (char *) mX.allocate(OBJ_SIZE);
                        LOOP3_ASSERT(i, j, k, q);
                        LOOP3_ASSERT(
                                i,
                                j,
                                k,
                                0 == bsls_PlatformUtil::IntPtr(q) % MAX_ALIGN);

                        scribble(q, OBJ_SIZE);
                        const int qCalculatedPool =
                                                 calcPool(NUM_POOLS, OBJ_SIZE);
                        const int qRecordedPool   = recPool(q);
                        if (veryVerbose) {
                            TAB; P_((void *)q);
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
      case 3: {
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
        //   bcema_ProtectableMultipoolAllocator(int numPools,
        //                                      bslma_Allocator *ba = 0);
        //   ~bcema_ProtectableMultipoolAllocator();
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

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            for (int i = 0; i < NUM_PDATA; ++i) {
                const int NUM_POOLS = PDATA[i];

                if (veryVerbose) { P(NUM_POOLS); }
                const int NUM_BYTES  = testDispenser.numBytesInUse();
                for (int j = 0; j < NUM_ODATA; ++j) {
                    {
                        Obj mX(NUM_POOLS, &testDispenser);
                        const int OBJ_SIZE = ODATA[j];
                        if (veryVerbose) { TAB; P(OBJ_SIZE); }
                        char *p = (char *) mX.allocate(OBJ_SIZE);
                        LOOP2_ASSERT(i, j, p);
                        const int pCalculatedPool =
                                                 calcPool(NUM_POOLS, OBJ_SIZE);
                        const int pRecordedPool   = recPool(p);
                        if (veryVerbose) {
                            TAB; P_((void *)p);
                            P_(pCalculatedPool); P(pRecordedPool);
                        }
                        LOOP2_ASSERT(i, j, pCalculatedPool == pRecordedPool);
                    }
                    LOOP2_ASSERT(i, j, NUM_BYTES  ==
                                                testDispenser.numBytesInUse());
                }

                {
                    Obj mX(NUM_POOLS, &testDispenser);
                    char *p = (char *) mX.allocate(2048);  // "overflow" pool
                    LOOP_ASSERT(i, p);
                    const int pRecordedPool = recPool(p);
                    if (veryVerbose) { TAB; P_((void *)p); P(pRecordedPool); }
                    LOOP_ASSERT(i, -1 == pRecordedPool);
                }
                LOOP_ASSERT(i, NUM_BYTES  == testDispenser.numBytesInUse());
            }
        }
        ASSERT(0 == testDispenser.numBytesInUse());
        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            for (int i = 0; i < NUM_PDATA; ++i) {
                const int NUM_POOLS = PDATA[i];
                if (veryVerbose) { P(NUM_POOLS); }
                const int NUM_BYTES  = testDispenser.numBytesInUse();
                for (int j = 0; j < NUM_ODATA; ++j) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj mX(NUM_POOLS, &testDispenser);
                    const int OBJ_SIZE = ODATA[j];
                    if (veryVerbose) { TAB; P(OBJ_SIZE); }
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    LOOP2_ASSERT(i, j, p);
                    const int pCalculatedPool = calcPool(NUM_POOLS, OBJ_SIZE);
                    const int pRecordedPool   = recPool(p);
                    if (veryVerbose) {
                        TAB; P_((void *)p);
                        P_(pCalculatedPool); P(pRecordedPool);
                    }
                    LOOP2_ASSERT(i, j, pCalculatedPool == pRecordedPool);
                  } END_BDEMA_EXCEPTION_TEST
                  LOOP2_ASSERT(i, j, NUM_BYTES  ==
                                                testDispenser.numBytesInUse());
                }

                BEGIN_BDEMA_EXCEPTION_TEST {
                  Obj mX(NUM_POOLS, &testDispenser);
                  char *p = (char *) mX.allocate(2048);  // "overflow" pool
                  LOOP_ASSERT(i, p);
                  const int pRecordedPool = recPool(p);
                  if (veryVerbose) { TAB; P_((void *)p); P(pRecordedPool); }
                  LOOP_ASSERT(i, -1 == pRecordedPool);
                } END_BDEMA_EXCEPTION_TEST
                LOOP_ASSERT(i, NUM_BYTES  == testDispenser.numBytesInUse());
            }
        }
      } break;
      case 2: {
        // -------------------------------------------------------------------
        // VERIFY HELPER FUNCTIONS:
        // Test the helper functions and classes defined in this test driver
        //
        // testProtectedSet()
        // --------------------------------------------------------------------
        if (verbose) {
            bsl::cout << "\tVerify Helper Functions\n"
                      << "\t=======================" << bsl::endl;
        }
        {
            // testProtectedSet()
            Obj a(4);
            char  dummy = '0';
            int   size;
            char *memX = (char *)a.allocate(1);

            *memX = 'x';
            ASSERT('x' == memX[0]);

            ASSERT(0 != testProtectedSet(&a, &dummy, 'a'));
            ASSERT(0 == testProtectedSet(&a, memX, 'a'));

            ASSERT('a' == dummy);
            ASSERT('a' == memX[0]);
        }
      }; break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   That the basic functionality of
        //   'bcema_ProtectableMultipoolAllocator' works properly.
        //
        // Plan:
        //   Create a multi-pool allocator that manages two pools using the
        //   lone constructor.  Allocate memory from each of the pools as well
        //   as   from the "overflow" pool.  Protect the memory and and test
        //   writing to protected memory causes a memory fault.  Then
        //   'deallocate' or 'release' the allocated objects.  Finally let
        //   the multi-pool go out of scope to exercise the destructor.
        //
        // Testing:
        //   This "test" exercises basic functionality, but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Breathing Test"
                          << endl << "==============" << endl;

        {
            char *p, *q, *r;
            int   rc;

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
                cout << "7. Protect the memory under the allocator" << endl;
            mX.protect();

            // 8.
            if (verbose)
                cout << "8. Test that writing to the memory causes a memory"
                    " causes a memory fault." << endl;

            rc = testProtectedSet(&mX, &p[0], 'x');   ASSERT(rc   == 0);
                                                      ASSERT(p[0] == 'x');
            rc = testProtectedSet(&mX, &q[0], 'x');   ASSERT(rc   == 0);
                                                      ASSERT(q[0] == 'x');
            rc = testProtectedSet(&mX, &r[0], 'x');   ASSERT(rc   == 0);
                                                      ASSERT(r[0] == 'x');

            rc = testProtectedSet(&mX, &p[MAX_ALIGN * 2 - 1], 'y');
                               ASSERT(rc   == 0);
                               ASSERT(p[MAX_ALIGN * 2 - 1] == 'y');
            rc = testProtectedSet(&mX, &q[MAX_ALIGN * 2 - 2], 'y');
                               ASSERT(rc   == 0);
                               ASSERT(q[MAX_ALIGN * 2 - 2] == 'y');
            rc = testProtectedSet(&mX, &r[1024 - 1], 'y');
                                ASSERT(rc   == 0);
                                ASSERT(r[1024 - 1] == 'y');

            // 9.
            if (verbose)
                cout << "9. Unprotect the memory under the allocator" << endl;
            mX.unprotect();

            // 10.
            if (verbose)
                cout << "10. Test that writing to the memory doesn't cause "
                    " a memory causes a memory fault." << endl;

            p[0] = 'a'; ASSERT(rc   == 0); ASSERT(p[0] == 'a');
            q[0] = 'a'; ASSERT(rc   == 0); ASSERT(q[0] == 'a');
            r[0] = 'a'; ASSERT(rc   == 0); ASSERT(r[0] == 'a');

            p[MAX_ALIGN * 2 - 1] = 'a';
                                 ASSERT(rc   == 0);
                                 ASSERT(p[MAX_ALIGN * 2 - 1] == 'a');
            q[MAX_ALIGN * 2 - 2] = 'a';
                                 ASSERT(rc   == 0);
                                 ASSERT(q[MAX_ALIGN * 2 - 2] == 'a');

            r[1024 -1] = 'a';
                                 ASSERT(rc   == 0);
                                 ASSERT(r[1024 -1] == 'a');

            // 11.
            if (verbose)
                cout << "11. Deallocate all outstanding objects." << endl;
            mX.release();

            // Addresses 'p', 'q', 'r' are no longer valid.

            // 12.
            if (verbose)
                cout << "12. Let the multi-pool go out of scope." << endl;
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
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
