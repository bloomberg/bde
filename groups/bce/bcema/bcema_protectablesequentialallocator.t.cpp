// bcema_protectablesequentialallocator.t.cpp  -*-C++-*-

#include <bcema_protectablesequentialallocator.h>

#include <bdema_testprotectableblockdispenser.h>       // for testing only
#include <bcemt_barrier.h>                             // for testing only
#include <bcema_testallocator.h>                       // for testing only
#include <bslma_testallocatorexception.h>              // for testing only
#include <bslma_defaultallocatorguard.h>               // for testing only

#include <bdema_protectableblockdispenser.h>
#include <bdema_nativeprotectableblockdispenser.h>
#include <bsls_alignmentutil.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>
#include <bsl_c_signal.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//-----------------------------------------------------------------------------
// [ 2] Verify helper functions (testProtectedSet)
// [ 3] Primary creators
//           bcema_ProtectableSequentialAllocator();
//           ~bcema_ProtectableSequentialAllocator();
// [ 4] Primary modifier
//           void *allocate(int size);
// [ 5] Primary modifiers, accessor
//           int protect(), int unprotect(), bool isProtected()
// [ 6] Modifier: release()
// [ 7] Modifier: expand(void *, int, 0)
// [ 8] Modifier: expand(void *, int, int)
// [ 9] Modifier: reserveCapacity(int x)
// [10] Modifier: deallocate(void *)
// [11] Primary modifier (geometric expansion & limit)
//           void *allocate(int size);
//           bcema_ProtectableSequentialAllocator(int, strategy, dispenser);
// [12] Primary modifier (linear expansion)
//           void *allocate(int size);
//           bcema_ProtectableSequentialAllocator(int, strategy, dispenser);
// [13] Free operators: new, delete
// [14] Concurrency
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 0] USAGE EXAMPLE
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
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define TAB cout << '\t';

//=============================================================================
//                  STANDARD BCEMA EXCEPTION TEST MACROS
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
//                               GLOBAL TYPEDEF
//-----------------------------------------------------------------------------

typedef bcema_ProtectableSequentialAllocator Obj;
typedef bdema_TestProtectableBlockDispenser  TestDisp;
typedef bdema_MemoryBlockDescriptor          Block;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

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

enum {
    NUM_THREADS = 4
};

struct WorkerArgs {
    Obj       *d_allocator; // allocator to perform allocations
    const int *d_sizes;     // array of allocations sizes
    int        d_numSizes;  // number of allocations

};

bcemt_Barrier g_barrier(NUM_THREADS);
extern "C" void *workerThread(void *arg) {
    // Perform a series of allocate, protect, unprotect, and deallocate
    // operations on the 'bdema_TestProtectableBlockDispenser' and verify
    // their results.  This is operation is intended to be a thread entry
    // point.  Cast the specified 'args' to a 'WorkerArgs', and perform a
    // series of '(WorkerArgs *)args->d_numSizes' allocations using the
    // corresponding allocations sizes specified by
    // '(WorkerARgs *)args->d_sizes'.  Protect, unprotect, and finally delete
    // the allocated memory.  Use the barrier 'g_barrier' to ensure tests are
    // performed while the allocator is in the correct state.

    WorkerArgs *args = (WorkerArgs *) arg;
    ASSERT(0 != args);
    ASSERT(0 != args->d_sizes);

    Obj       *allocator  = args->d_allocator;
    const int *allocSizes = args->d_sizes;
    const int  numAllocs  = args->d_numSizes;

    bsl::vector<char *> blocks(bslma_Default::allocator(0));
    blocks.resize(numAllocs);

    g_barrier.wait();

    // Perform allocations
    for (int i = 0; i < numAllocs; ++i) {
        blocks[i] = (char *)allocator->allocate(allocSizes[i]);
    }

    g_barrier.wait();

    // Perform valid protect operation
    allocator->protect();

    // Perform valid un protect operation
    allocator->unprotect();

    // Perform a protect operations
    allocator->protect();

    g_barrier.wait();

    // unprotect
    allocator->unprotect();

    // deallocate all the blocks but the last
    for (int i = 0; i < numAllocs; ++i) {
        allocator->deallocate(blocks[i]);
    }

    for (int i = 0; i < numAllocs; ++i) {
        blocks[i] = (char *)allocator->allocate(allocSizes[i]);
    }

    for (int i = 0; i < numAllocs - 1; ++i) {
        allocator->deallocate(blocks[i]);
    }

    g_barrier.wait();

    // The final block still exists.  Perform a synchronized unprotect and
    // write.  Synchronize and verify the data.
    unsigned char threadId = bcemt_ThreadUtil::selfIdAsInt();
    memset(blocks[numAllocs - 1], threadId, allocSizes[numAllocs - 1]);

    g_barrier.wait();

    for (int i = 0; i < allocSizes[numAllocs - 1]; ++i) {
        unsigned char *data = (unsigned char *)blocks[numAllocs - 1];
        ASSERT(threadId == data[i]);
    }

    g_barrier.wait();

    allocator->release();

    return arg;
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

// The following example uses the 'bcema_ProtectableSequentialAllocator'
// to create a protected stack of integers.  Integers can be pushed onto and
// popped off of the stack, but the memory in the stack is protected so that a
// fault will occur if any of the data in the container is written to
// outside of the 'IntegerStack' container.  Since a sequential allocator
// will not deallocate memory, this container is not very efficient.
//..
    class IntegerStack {
        // This is a trivial implementation of a stack of ints whose data
        // has READ-ONLY access protection.  It does not perform bounds
        // checking.

        int                                        *d_data;      // stack
        int                                         d_stackSize; // top of
                                                                 // stack
        int                                         d_maxSize;   // max size
        bcema_ProtectableSequentialAllocator  d_allocator; // owned

        // NOT IMPLEMENTED
        IntegerStack(const IntegerStack& original);
        IntegerStack& operator=(const IntegerStack& rhs);

        enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };

      private:

//..
// Note that the increaseSize() method below will waste the previously
// allocated memory because a sequential allocator does not provide a means to
// deallocate it.
//..
        void increaseSize()
            // Increases the size of the stack memory by the growth factor.
            // Behavior is undefined unless the stacks allocator is in an
            // unprotected state.
        {
            int *oldData = d_data;
            int  oldSize = d_maxSize;
            d_maxSize *= GROW_FACTOR;
            d_data = (int *)d_allocator.allocate(d_maxSize * sizeof(int));
            memcpy(d_data, oldData, sizeof(int) * oldSize);
        }

      public:

        // CREATORS
        IntegerStack(
                bdema_ProtectableBlockDispenser *protectedDispenser = 0)
            // Create an 'IntegerStack' using the optionally specified
            // the specified 'protectedDispenser'.   If the
            // 'protectedDispenser' is not specified, use the native dispenser.
        : d_data()
        , d_stackSize(0)
        , d_maxSize(INITIAL_SIZE)
        , d_allocator(protectedDispenser)
        {
            d_data = (int *)d_allocator.allocate(d_maxSize * sizeof(int));
            d_allocator.protect();
        }

        ~IntegerStack()
            // Destroy this object and release its memory.
        {
        }

//..
// We must unprotect the dispenser before modifying or deallocating
// memory:
//..
        // MANIPULATORS
        void push(int value)
            // Push the specified 'value' onto the stack.
        {
            d_allocator.unprotect();
            if (d_stackSize >= d_maxSize) {
                increaseSize();
            }
            d_data[d_stackSize++] = value;
            d_allocator.protect();
        }

        int pop()
            // Remove the top value from the stack and return it.
        {
            // Memory is only being read so there is no need to unprotect it
            return d_data[--d_stackSize];
        }
    };

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
    int HEADER_SIZE    = bdema_ProtectableBlockList::blockHeaderSize();

    bdema_TestProtectableBlockDispenser testDispenser(PG_SIZE,
                                                      veryVeryVerbose);
    bcema_TestAllocator         talloc;
    bslma_DefaultAllocatorGuard guard(&talloc);
    switch (test) { case 0:
      case 15: {
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
        //
        // --------------------------------------------------------------------
        if (verbose) cout << endl << "USAGE EXAMPLE"
                          << endl << "============="
                          << endl;
        bdema_TestProtectableBlockDispenser testDispenser(PG_SIZE);
        IntegerStack stack(&testDispenser);

        stack.push(9);
        ASSERT( testDispenser.numBlocksProtected() ==
                testDispenser.numBlocksInUse());

        stack.push(5);
        ASSERT( testDispenser.numBlocksProtected() ==
                testDispenser.numBlocksInUse());

        stack.push(3);
        ASSERT( testDispenser.numBlocksProtected() ==
                testDispenser.numBlocksInUse());

        ASSERT(3 == stack.pop());
        ASSERT(5 == stack.pop());
        ASSERT(9 == stack.pop());
        ASSERT( testDispenser.numBlocksProtected() ==
                testDispenser.numBlocksInUse());

    } break;
      case 14: {
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
        {
            if (veryVerbose) cout << "\tTest a variety of small sizes" << endl;

            TestDisp disp(1024, veryVeryVerbose);
            Obj      mX(&disp);

            int SIZES[]   = {1, 2, 3, 4, 5};
            const int NUM_SIZES = sizeof (SIZES) / sizeof(*SIZES);

            WorkerArgs args;
            args.d_allocator = &mX;
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
            ASSERT(0 == disp.numBlocksInUse());
            ASSERT(1 == disp.numBlocksMax());
        }
        {
            if (veryVerbose) cout << "\tTest multiple page allocation" << endl;

            TestDisp disp(PG_SIZE, veryVeryVerbose);
            Obj      mX(&disp);

            // Using equal sizes ensures that we can deterministically compute
            // the expected maximum number of blocks allocated
            int SIZES[]   = {PG_SIZE - HEADER_SIZE,
                             PG_SIZE - HEADER_SIZE,
                             PG_SIZE - HEADER_SIZE,
                             PG_SIZE - HEADER_SIZE,
                             PG_SIZE - HEADER_SIZE};

            const int NUM_SIZES = sizeof (SIZES) / sizeof(*SIZES);

            WorkerArgs args;
            args.d_allocator = &mX;
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
            ASSERT(0                       == disp.numBlocksInUse());
            ASSERT(NUM_SIZES * NUM_THREADS >= disp.numBlocksMax());
            ASSERT(NUM_SIZES * NUM_THREADS * PG_SIZE
                                           <= disp.numBytesMax());
        }
      } break;
      case 13: {
        // -------------------------------------------------------------------
        // VERIFY FREE OPERATORS: new/delete
        //
        // Verify new calls allocate() and delete is a no-op.
        // Testing:
        //
        // -------------------------------------------------------------------

        if (verbose) {
            bsl::cout
                << "\nVerify free operators: new/delete"
                << "\n===========================" << bsl::endl;
        }

        Obj a(&testDispenser);
        ASSERT( 0 == testDispenser.numBytesInUse());

        struct DummyType {
                int d_dummy1;
                int d_dummy2;
        };
        DummyType *mem = new (a) DummyType;

        ASSERT( 0 < testDispenser.numBytesInUse() );
        int numBytes = testDispenser.numBytesInUse();
      } break;
      case 12: {
        // -------------------------------------------------------------------
        // VERIFY PRIMARY MODIFIER: allocate
        //
        // Concerns:
        // 1. Test that the pool expands linearly if a positive expansion
        //    limit is supplied.
        //
        // Third, create a sequential allocator with a negative expansion
        // limit (linear growth).  Allocate 1 byte, then expand, and
        // repeat.  Verify the memory expands linearly with increments of the
        // absolute value of the specified limit.  Allocate 3 * limit, verify
        // sufficient memory is allocated.
        //
        // Testing:
        //      void *allocate(int x);
        //      bcema_ProtectableSequentialAllocator(
        //                         size_type,
        //                         bsls_Alignment::Strategy,
        //                         bdema_ProtectableBlockDispenser *);
        // -------------------------------------------------------------------
        if (verbose) {
            bsl::cout << "\nVerify linear expansion"
                      << "\n======================="<< bsl::endl;
        }
        const int BHS       = HEADER_SIZE;
        const int MAX_ALLOCS = 12;
        int LIMITS[] = { 0,
                         1,
                         PG_SIZE - BHS,
                         2 * PG_SIZE - BHS,
                         5 * PG_SIZE - BHS,
                         32 * PG_SIZE - BHS};
        const int NUM_LIMITS = sizeof(LIMITS)/sizeof(*LIMITS);

        if (veryVerbose) {P_(HEADER_SIZE) P(PG_SIZE);}

        if (veryVerbose) {
            bsl::cout << "\tTest positive (linear) expansion size"
                      << bsl::endl;
        }
        for (int i = 0; i < NUM_LIMITS; ++i) {
            // TEST 2: Set negative max buffer size limit.  Allocate 1, expand,
            // repeat.  Verify linear growth increments of -limit size,
            // allocate 3 * limit, verify 3 * limit is allocated.

            const int LIMIT = LIMITS[i];
            if (0 == LIMIT) {
                continue;
            }

            if (veryVerbose) {P(LIMIT);}
            Obj a(bsls_Alignment::BSLS_NATURAL, LIMIT, &testDispenser);

            const int EXPECTED = (((LIMIT + PG_SIZE - 1)/ PG_SIZE) * PG_SIZE);
            for (int i = 0; i < MAX_ALLOCS; ++i) {
                void *ptr = a.allocate(1);
                ASSERT(EXPECTED == testDispenser.lastAllocateNumBytes());
                a.expand(ptr, 1);
            }

            const int EXPECTED2 = (((3*LIMIT + PG_SIZE - 1)/PG_SIZE)*PG_SIZE);
            a.allocate(3 * LIMIT);
            ASSERT(EXPECTED2 == testDispenser.lastAllocateNumBytes());
        }
      } break;
      case 11: {
        // -------------------------------------------------------------------
        // VERIFY PRIMARY MODIFIER: allocate
        //
        // Concerns:
        // 1. Test that the pool of allocated memory expands geometrically.
        // 2. Test that the the geometric expansion limit is observed.
        // 3. Test that the pool expands linearly after reach the limit.
        //
        // First create a sequential allocator with a expansion
        // limit.  Allocate 1 byte, then expand, and repeat.  Verify
        // the memory expands geometrically up to the specified limit, at
        // which point the limit is allocated.
        //
        // Second create a sequential allocator with an expansion limit.
        // Allocate the limit, allocate twice the limit, allocate the limit
        // again.  Verify that the memory allocated matches the size requested.
        //
        // Testing:
        //      void *allocate(int x);
        //      bcema_ProtectableSequentialAllocator(
        //                         size_type,
        //                         bsls_Alignment::Strategy,
        //                         bdema_ProtectableBlockDispenser *);
        // -------------------------------------------------------------------
        if (verbose) {
            bsl::cout << "\nVerify geometric expansion & Limit"
                      << "\n=================================="<< bsl::endl;
        }
        const int BHS       = HEADER_SIZE;
        const int MAX_ALLOCS = 12;
        int LIMITS[] = { 0,
                         1,
                         PG_SIZE - BHS,
                         2 * PG_SIZE - BHS,
                         5 * PG_SIZE - BHS,
                         32 * PG_SIZE - BHS};
        const int NUM_LIMITS = sizeof(LIMITS)/sizeof(*LIMITS);

        if (veryVerbose) {P_(HEADER_SIZE) P(PG_SIZE);}
        if (veryVerbose) {
            bsl::cout << "\tTest allocating 1 byte, expand, repeat"
                      << bsl::endl;
        }
        for (int i = 0; i < NUM_LIMITS; ++i) {
            // TEST 1: Set max buffer size limit.  Allocate 1 byte, expand,
            // repeat.  Verify geometric growth up to limit.

            const int LIMIT_PARAM = LIMITS[i];
            const int LIMIT = (0 == LIMITS[i]) ? INT_MAX : LIMITS[i];

            if (veryVerbose) { P(LIMIT_PARAM); }
            Obj a(bsls_Alignment::BSLS_NATURAL, -LIMIT_PARAM, &testDispenser);

            int size      = 0;
            int nextAlloc = PG_SIZE;
            for (int j = 0; j < MAX_ALLOCS; ++j) {
                void *ptr = a.allocate(1);

                if (veryVeryVerbose) {
                    P_(size); P_(nextAlloc);
                    P(testDispenser.lastAllocateNumBytes());
                }

                ASSERT(nextAlloc == testDispenser.lastAllocateNumBytes());
                a.expand(ptr, 1);

                size     += nextAlloc;
                nextAlloc = size;
                if (nextAlloc > LIMIT) {
                    nextAlloc = (((LIMIT + PG_SIZE - 1)/ PG_SIZE) * PG_SIZE);
                }
            }
        }
        if (veryVerbose) {
            bsl::cout << "\tTest allocating LIMIT, 2 * LIMIT, LIMIT"
                      << bsl::endl;
        }
        for (int i = 0; i < NUM_LIMITS; ++i) {
            // TEST 2: Set max buffer size limit.  Allocate limit, allocate
            // 2 * LIMIT, allocate 1. Observer allocations larger than the
            // limit are handled correctly.

            const int LIMIT = LIMITS[i];

            if (0 == LIMIT) {
                continue;
            }
            if (veryVerbose) {P(LIMIT);}
            Obj a(bsls_Alignment::BSLS_NATURAL, -LIMIT, &testDispenser);

            // Expected allocation is the the allocated amount + block header
            // size rounded to the nearest page size.
            const int EXP_ALLOC     = (( LIMIT + BHS + PG_SIZE - 1)/PG_SIZE)
                                   * PG_SIZE;
            const int EXP_DBL_ALLOC = ((2 * LIMIT + BHS + PG_SIZE -1)/PG_SIZE)
                                   * PG_SIZE;

            void *ptr = a.allocate(LIMIT);
            LOOP2_ASSERT(EXP_ALLOC,
                         testDispenser.lastAllocateNumBytes(),
                         EXP_ALLOC == testDispenser.lastAllocateNumBytes());

            ptr = a.allocate(2 * LIMIT);
            LOOP2_ASSERT(EXP_DBL_ALLOC,
                         testDispenser.lastAllocateNumBytes(),
                         EXP_DBL_ALLOC ==
                         testDispenser.lastAllocateNumBytes());

            a.expand(ptr, 2 * LIMIT);
            a.allocate(1);
            LOOP2_ASSERT(EXP_ALLOC,
                         testDispenser.lastAllocateNumBytes(),
                         EXP_ALLOC == testDispenser.lastAllocateNumBytes());

        }
      } break;
      case 10: {
        // -------------------------------------------------------------------
        // VERIFY MODIFIERS: deallocate()
        //
        // Test deallocate has no impact on allocated memory.
        //
        // Testing:
        //      void deallocate(void *);
        // -------------------------------------------------------------------

        if (verbose) {
            bsl::cout
                << "\nVerify Modifier: deallocate"
                << "\n===========================" << bsl::endl;
        }
        int SIZES[] = {
            1,
            PG_SIZE - HEADER_SIZE,
            PG_SIZE,
            PG_SIZE + 1,
            2 * PG_SIZE - HEADER_SIZE,
            2 * PG_SIZE
        };
        int NUM_TESTS = sizeof(SIZES)/sizeof(*SIZES);
        for (int i = 0; i < NUM_TESTS; ++i) {
            {
                Obj a(&testDispenser);
                ASSERT( 0 == testDispenser.numBytesInUse());
                void *mem = a.allocate(SIZES[i]);
                ASSERT(0 < testDispenser.numBytesInUse());
                int numBytes = testDispenser.numBytesInUse();
                a.deallocate(mem);
                ASSERT(numBytes == testDispenser.numBytesInUse());
            }
            {
                Obj a(&testDispenser);
                ASSERT( 0 == testDispenser.numBytesInUse());
                void *mem = a.allocate(SIZES[i]);
                ASSERT(0 < testDispenser.numBytesInUse());
                int numBytes = testDispenser.numBytesInUse();
                a.protect();
                a.deallocate(mem);
                ASSERT(numBytes == testDispenser.numBytesInUse());
            }

        }
      } break;
      case 9: {
        // -------------------------------------------------------------------
        // VERIFY MODIFIERS: reserveCapacity
        //
        // Use a table of input data describing different amount of memory to
        // reserve.  Verify that reserving capacity ensures data is not
        // allocated on a call to allocate().
        //
        // Testing:
        //      void reserveCapacity(int numBytes);
        // -------------------------------------------------------------------

        if (verbose) {
            bsl::cout
                << "\nVerify Modifier: reserveCapacity"
                << "\n================================" << bsl::endl;
        }

        const int BHS = HEADER_SIZE;
        const int ALIGN = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;
        struct {
            int d_firstAlloc;
            int d_secondAlloc;
            int d_actualAlloc;
        } DATA[] = {
          { 0,                         0,                         0},
          { 0,                         1,                         PG_SIZE},
          { 0,                         PG_SIZE - BHS - ALIGN,     PG_SIZE},
          { 1,                         PG_SIZE - BHS - ALIGN - 1, PG_SIZE},
          { PG_SIZE - BHS - ALIGN,     1,                         PG_SIZE},
          { PG_SIZE - BHS,             1,                         2 * PG_SIZE},
          { PG_SIZE,                   1,                         2 * PG_SIZE},
          { PG_SIZE - BHS - ALIGN,     PG_SIZE - BHS - ALIGN,     2 * PG_SIZE},
          { PG_SIZE - BHS,             1,                         2 * PG_SIZE},
          { PG_SIZE - BHS,             PG_SIZE - BHS,             2 * PG_SIZE},
          { PG_SIZE - BHS + 1,         PG_SIZE - BHS - ALIGN,     2 * PG_SIZE},
          { PG_SIZE - BHS,             PG_SIZE - BHS + 1,         3 * PG_SIZE},
          { 2 * PG_SIZE - BHS - ALIGN, 1,                         2 * PG_SIZE},
          { 2 * PG_SIZE - BHS,         1,                         4 * PG_SIZE},
          { 2 * PG_SIZE - BHS,         PG_SIZE - BHS,             4 * PG_SIZE},
          { 2 * PG_SIZE - BHS,         2 * PG_SIZE - BHS,         4 * PG_SIZE},
          { 2 * PG_SIZE - BHS,         2 * PG_SIZE - BHS + 1,     6 * PG_SIZE},
            };

        int NUM_TESTS = sizeof(DATA)/sizeof(*DATA);

        for (int i = 0; i < NUM_TESTS; ++i) {
            Obj a(&testDispenser);
            ASSERT( 0 == testDispenser.numBytesInUse());

            a.allocate(DATA[i].d_firstAlloc);
            a.reserveCapacity(DATA[i].d_secondAlloc);

            ASSERT(DATA[i].d_actualAlloc == testDispenser.numBytesInUse());
            a.allocate(DATA[i].d_secondAlloc);

            ASSERT(DATA[i].d_actualAlloc == testDispenser.numBytesInUse());
        }
      } break;
      case 8: {
        // -------------------------------------------------------------------
        // VERIFY MODIFIERS: expand
        //
        // Use a table of input data describing various allocation sizes to
        // verify that allocateAndExpand behaves as expected
        //
        // Testing:
        //      int expand(void *, int, int);
        // -------------------------------------------------------------------

        if (verbose) {
            bsl::cout
                << "\nVerify Modifier: expand"
                << "\n=======================" << bsl::endl;
        }
        const int BHS = HEADER_SIZE;
        struct {
                int d_allocSize;
                int d_maxSize;
                int d_actualAlloc;
        } DATA[] = {
            { 0,                 0,                    0},
            { 0,                 100,                  0},
            { 1,                 500,                  PG_SIZE},
            { 1,                 0,                    PG_SIZE},
            { 1,                 PG_SIZE - BHS,        PG_SIZE},
            { 1,                 PG_SIZE,              PG_SIZE},
            { PG_SIZE - BHS,     PG_SIZE,              PG_SIZE},
            { PG_SIZE,           PG_SIZE,              2 * PG_SIZE},
            { PG_SIZE,           PG_SIZE + 100,        2 * PG_SIZE},
            { PG_SIZE,           2* PG_SIZE - BHS,     2 * PG_SIZE},
            { PG_SIZE,           2* PG_SIZE,           2 * PG_SIZE},
        };
        int NUM_TESTS = sizeof(DATA)/sizeof(*DATA);

        for (int i = 0; i < NUM_TESTS; ++i) {
            Obj a(&testDispenser);

            ASSERT(0 == testDispenser.numBytesInUse());

            int   size = DATA[i].d_allocSize;
            void *mem  = a.allocate(size);
            ASSERT(DATA[i].d_actualAlloc == testDispenser.numBytesInUse());

            a.expand(mem, size, DATA[i].d_maxSize);

            ASSERT(DATA[i].d_actualAlloc == testDispenser.numBytesInUse());

            int maxSize = (DATA[i].d_maxSize == 0) ?
                          DATA[i].d_actualAlloc :
                          bsl::min(DATA[i].d_actualAlloc,
                                   DATA[i].d_maxSize + HEADER_SIZE);
            int expectedLastByte = (size == 0) ? 0 : maxSize;

            // We need to determine if we expanded in such a way that a new
            // allocation will actually return more memory
            const int ALIGN = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;
            bool expand = (expectedLastByte + ALIGN + PG_SIZE - 1) / PG_SIZE >
                           (expectedLastByte + PG_SIZE - 1) / PG_SIZE;

            int  newActualAlloc = 0;
            if (0 == DATA[i].d_actualAlloc) {
                newActualAlloc = PG_SIZE;
            } else {
                newActualAlloc =  (expand) ? DATA[i].d_actualAlloc * 2 :
                                             DATA[i].d_actualAlloc;
            }
            a.allocate(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);
            LOOP_ASSERT(i, newActualAlloc == testDispenser.numBytesInUse());
        }

      } break;
      case 7: {
        // -------------------------------------------------------------------
        // VERIFY MODIFIERS: expand
        //
        // Use a table of input data describing various allocation sizes to
        // verify that expand behaves as expected.  Allocate the test amount of
        // memory, then call expand() and then allocate() a second time.  The
        // call to expand should not effect the amount of actually allocated
        // memory, but should force the second call to allocate() to allocate
        // new memory.
        //
        // Testing:
        //      int expand(void *, int);
        // -------------------------------------------------------------------

        if (verbose) {
            bsl::cout
                << "\nVerify Modifier: expand"
                << "\n=======================" << bsl::endl;
        }
        const int BHS = HEADER_SIZE;
        struct {
                int d_allocSize;
                int d_actualAlloc;
        } DATA[] = {
            { 0,                     0},
            { 1,                     PG_SIZE},
            { PG_SIZE - BHS,         PG_SIZE},
            { PG_SIZE,               2 * PG_SIZE},
            { PG_SIZE+1,             2 * PG_SIZE},
            { 2 * PG_SIZE - BHS,     2 * PG_SIZE},
            { 2 * PG_SIZE,           4 * PG_SIZE},
            { 4 * PG_SIZE - BHS,     4 * PG_SIZE},
            { 4 * PG_SIZE - BHS + 1, 8 * PG_SIZE}
        };
        int NUM_TESTS = sizeof(DATA)/sizeof(*DATA);

        for (int i = 0; i < NUM_TESTS; ++i) {
            Obj a(&testDispenser);

            int size  = DATA[i].d_allocSize;
            void *mem = a.allocate(size);

            LOOP_ASSERT(i, DATA[i].d_actualAlloc ==
                           testDispenser.numBytesInUse());

            a.expand(mem, size);
            LOOP_ASSERT(i, DATA[i].d_actualAlloc ==
                           testDispenser.numBytesInUse());

            a.allocate(1);

            if (DATA[i].d_actualAlloc == 0) {
                LOOP_ASSERT(i, PG_SIZE == testDispenser.numBytesInUse());
            }
            else {
                LOOP_ASSERT(i, DATA[i].d_actualAlloc * 2 ==
                               testDispenser.numBytesInUse());
            }
        }
      } break;
      case 6: {
        // -------------------------------------------------------------------
        // VERIFY MODIFIERS: release
        //
        // Concerns:
        // That release frees all allocated memory and unprotects memory
        // before deallocating it.
        //
        // Plan:
        // Use a table of input data describing various allocation sizes to
        // populate an allocator, then release the memory.  Try again with
        // protected memory
        //
        // Testing:
        //      void release();
        // -------------------------------------------------------------------
        if (verbose) {
            bsl::cout
                << "\nVerify Modifiers: release"
                << "\n=========================" << bsl::endl;
        }

        const int BHS       = HEADER_SIZE;
        struct {
                int d_allocSize;
                int d_actualAlloc;
        } DATA[] = {
            { 0,                     0},
            { 1,                     PG_SIZE},
            { PG_SIZE - BHS,         PG_SIZE},
            { PG_SIZE,               2 * PG_SIZE},
            { PG_SIZE+1,             2 * PG_SIZE},
            { 2 * PG_SIZE - BHS,     2 * PG_SIZE},
            { 2 * PG_SIZE,           4 * PG_SIZE},
            { 4 * PG_SIZE - BHS,     4 * PG_SIZE},
            { 4 * PG_SIZE - BHS + 1, 8 * PG_SIZE}
        };
        const int NUM_TESTS  = sizeof(DATA)/sizeof(*DATA);
        for (int i = 0; i < NUM_TESTS; ++i) {
            Obj a(&testDispenser);

            ASSERT( 0 == testDispenser.numBytesInUse());

            a.allocate(DATA[i].d_allocSize);
            ASSERT(DATA[i].d_actualAlloc == testDispenser.numBytesInUse());

            a.release();
            ASSERT( 0 == testDispenser.numBytesInUse());
        }

        // Verify release on protected memory.
        for (int i = 0; i < NUM_TESTS; ++i) {
            Obj a(&testDispenser);

            ASSERT( 0 == testDispenser.numBytesInUse());

            a.allocate(DATA[i].d_allocSize);
            ASSERT(DATA[i].d_actualAlloc == testDispenser.numBytesInUse());

            a.protect();
            a.release();
            ASSERT( 0 == testDispenser.numBytesInUse());
        }

      } break;
      case 5: {
        // -------------------------------------------------------------------
        // VERIFY PRIMARY MODIFIERS: protect/unprotect
        //
        // Note: Most of the heavy lifting for this test is handled by the
        // bdema_TestProtectableBlockDispenser - which verifies that the
        // protection operations are correct and all the memory is successfully
        // protected.
        //
        // Use a table of input data describing various allocation sizes to
        // verify that protect will protect all the allocated memory.
        //
        // Testing:
        //      int protect();
        //      int unprotect();
        //      bool isProtected() const;
        // -------------------------------------------------------------------

        if (verbose) {
            bsl::cout
                << "\nVerify Primary Modifiers: protect, unprotect"
                << "\n=======================" << bsl::endl;
        }

        {
            if (veryVerbose)
                bsl::cout << "\t\tverify various protect/unprotect calls"
                          << bsl::endl;

            int SIZES[] = {
                1,
                PG_SIZE - HEADER_SIZE,
                PG_SIZE,
                PG_SIZE + 1,
                2 * PG_SIZE - HEADER_SIZE,
                2 * PG_SIZE
            };
            int NUM_TESTS = sizeof(SIZES)/sizeof(*SIZES);

            for (int i = 0; i < NUM_TESTS; ++i) {
                Obj a(&testDispenser);
                // For each iteration select one more allocation from the
                // sample data to allocate.
                for (int j = 0; j <= i; ++j) {
                    int size;
                    a.allocate(SIZES[i]);
                }

                ASSERT( 0 < testDispenser.numBlocksInUse());
                ASSERT( 0 == testDispenser.numBlocksProtected());
                ASSERT( false == a.isProtected());

                a.protect();
                ASSERT( testDispenser.numBlocksInUse() ==
                        testDispenser.numBlocksProtected());
                ASSERT( true == a.isProtected());

                a.protect();
                ASSERT( testDispenser.numBlocksInUse() ==
                        testDispenser.numBlocksProtected());
                ASSERT( true == a.isProtected());

                a.unprotect();
                ASSERT( 0 == testDispenser.numBlocksProtected());
                ASSERT( false == a.isProtected());

                a.unprotect();
                ASSERT( 0 == testDispenser.numBlocksProtected());
                ASSERT( false == a.isProtected());

            }
        }
      } break;
      case 4: {
        // -------------------------------------------------------------------
        // VERIFY PRIMARY MODIFIER: allocate
        //
        // 1. Test the results of a single allocation using a table of test
        //    allocation sizes and expected results.
        // 2. Test the results of a second allocation, to verify that the
        //    allocator makes use of a previously allocated but unused buffer
        //    space.  Use a table of test allocation sizes and expected
        //    results.
        //
        // Testing:
        //      void *allocate(int x);
        // -------------------------------------------------------------------
        if (verbose) {
            bsl::cout << "\nVerify Primary Modifier: allocate"
                      << "\n================================="<< bsl::endl;
        }
        const int BHS       = HEADER_SIZE;
        {
            struct {
                    int d_allocSize;
                    int d_actualAlloc;
            } DATA[] = {
                { 0,                 0},
                { 1,                 PG_SIZE},
                { PG_SIZE - BHS,     PG_SIZE},
                { PG_SIZE,           2 * PG_SIZE},
                { PG_SIZE+1,         2 * PG_SIZE},
                { 2 * PG_SIZE - BHS, 2 * PG_SIZE},
                { 2 * PG_SIZE,       4 * PG_SIZE},
                { 2 * PG_SIZE + 1,   4 * PG_SIZE}
            };
            const int NUM_TESTS  = sizeof(DATA)/sizeof(*DATA);
            for (int i = 0; i < NUM_TESTS; ++i) {
                // Test allocating with an unprotected allocator
                Obj a(&testDispenser);
                a.allocate(DATA[i].d_allocSize);
                LOOP_ASSERT(i, DATA[i].d_actualAlloc ==
                            testDispenser.lastAllocateNumBytes());
            }

        }
        {
            const int ALIGN = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;
            struct {
                    int d_firstAlloc;
                    int d_secondAlloc;
                    int d_actualAlloc;
            } DATA[] = {
          { 0,                         0,                         0},
          { 1,                         PG_SIZE - BHS - ALIGN - 1, PG_SIZE},
          { PG_SIZE - BHS - ALIGN,     1,                         PG_SIZE},
          { PG_SIZE - BHS,             1,                         2 * PG_SIZE},
          { PG_SIZE,                   1,                         2 * PG_SIZE},
          { PG_SIZE - BHS - ALIGN,     PG_SIZE - BHS - ALIGN,     2 * PG_SIZE},
          { PG_SIZE - BHS,             1,                         2 * PG_SIZE},
          { PG_SIZE - BHS,             PG_SIZE - BHS,             2 * PG_SIZE},
          { PG_SIZE - BHS + 1,         PG_SIZE - BHS - ALIGN,     2 * PG_SIZE},
          { PG_SIZE - BHS,             PG_SIZE - BHS + 1,         3 * PG_SIZE},
          { 2 * PG_SIZE - BHS - ALIGN, 1,                         2 * PG_SIZE},
          { 2 * PG_SIZE - BHS,         1,                         4 * PG_SIZE},
          { 2 * PG_SIZE - BHS,         PG_SIZE - BHS,             4 * PG_SIZE},
          { 2 * PG_SIZE - BHS,         2 * PG_SIZE - BHS,         4 * PG_SIZE},
          { 2 * PG_SIZE - BHS,         2 * PG_SIZE - BHS + 1,     6 * PG_SIZE},
            };

            const int NUM_TESTS  = sizeof(DATA)/sizeof(*DATA);
            for (int i = 0; i < NUM_TESTS; ++i) {
                // test allocating from a protected container
                BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj a(&testDispenser);

                    a.allocate(DATA[i].d_firstAlloc);
                    a.allocate(DATA[i].d_secondAlloc);
                    LOOP2_ASSERT(i,
                                 testDispenser.numBytesInUse(),
                                 DATA[i].d_actualAlloc ==
                                 testDispenser.numBytesInUse());
                } END_BDEMA_EXCEPTION_TEST
            }
        }

      } break;
      case 3: {
        // -------------------------------------------------------------------
        // VERIFY PRIMARY CONSTRUCTORS
        //
        // Testing:
        //     bcema_ProtectableSequentialAllocator(
        //                             bcema_ProtectedMemoryAllocator *);
        //    ~bcema_ProtectableSequentialAllocator(
        //                             bcema_ProtectedMemoryAllocator *);
        // -------------------------------------------------------------------
        if (verbose) {
            bsl::cout << "\nVerify Primary Creators"
                      << "\n=======================" << bsl::endl;
        }

        {
            Obj a(&testDispenser);
            ASSERT(0 == testDispenser.numBytesInUse());
            a.allocate(1);
            ASSERT(0 < testDispenser.numBytesInUse());
        }
        ASSERT(0 == testDispenser.numBytesInUse());
        {
            Obj a(&testDispenser);
            ASSERT(0 == testDispenser.numBytesInUse());
            a.allocate(1);
            a.protect();
            ASSERT(0 < testDispenser.numBytesInUse());
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
            Obj a;
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
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST: Test that the allocated regions are protected by
        //                   this allocator.
        //
        // Allocate memory of various sizes and then protect it, verify that
        // the memory was protected.
        // --------------------------------------------------------------------
        if (verbose) {
            bsl::cout << "\nBreathing Test"
                      << "\n==============" << bsl::endl;
        }

        Obj a;
        const int SIZES[]    = { 0,
                                 1,
                                 200,
                                 ACTUAL_PG_SIZE - 50,
                                 ACTUAL_PG_SIZE,
                                 ACTUAL_PG_SIZE + 1,
                                 2*ACTUAL_PG_SIZE - 50,
                                 2*ACTUAL_PG_SIZE,
                                 2*ACTUAL_PG_SIZE+1
                               };
        const int NUM_TESTS  = sizeof(SIZES)/sizeof(int);
        char     *DATA[NUM_TESTS];

        for (int i = 0; i < NUM_TESTS; ++i) {
            // Initialized DATA
            DATA[i] = (char *)a.allocate(SIZES[i]);

            memset(DATA[i], 'x', SIZES[i]);

            if (SIZES[i] == 0) {
                ASSERT(0 == SIZES[i]);
                continue;
            }

            // Set test points
            int start = 0;
            int mid = SIZES[i] / 2;
            int end = SIZES[i] - 1;

            ASSERT(0 == testProtectedSet(&a, &DATA[i][start], 'a'));
            ASSERT(0 == testProtectedSet(&a, &DATA[i][mid],   'a'));
            ASSERT(0 == testProtectedSet(&a, &DATA[i][end],   'a'));

            ASSERT('a' == DATA[i][start]);
            ASSERT('a' == DATA[i][mid]);
            ASSERT('a' == DATA[i][end]);
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
