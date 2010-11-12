// bdema_sequentialallocator.t.cpp                                    -*-C++-*-
#include <bdema_sequentialallocator.h>

#include <bdema_sequentialpool.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_allocator.h>

#include <bsls_alignment.h>
#include <bsls_blockgrowth.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// A 'bdema_SequentialAllocator' adopts the 'bdema_SequentialPool' mechanism to
// a 'bslma_ManagedAllocator' protocol.  The primary concern is that the
// allocator correctly proxies the memory allocation requests to the sequential
// pool it adopts.
//
// To test requests are correctly proxied, we create a sequential allocator and
// a sequential pool using the same external buffer and two different test
// allocators.  We then verify that either:
//
// 1) The address returned by the allocator and the pool is the same (i.e., the
//    memory is allocated from the buffer).
//
// 2) If the memory used by either test allocator is non-zero, the number of
//    bytes used by both test allocators is the same.
//
// We also need to verify that 'deallocate' method has no effect.  Again, we
// make use of the test allocator to ensure no memory is deallocated when the
// 'deallocate' method of a sequential allocator is invoked on previously
// allocated memory.
//
// Finally, the destructor of 'bdema_SequentialAllocator' is tested throughout
// the test driver.  At destruction, the allocator should reclaim all
// outstanding memory allocation.  By setting the global allocator, default
// allocator and object allocator to different test allocators, we can
// determine whether all memory had been released by the destructor of the
// allocator.
//-----------------------------------------------------------------------------
// // CREATORS
// [ 2] bdema_SequentialAllocator(Alloc *a = 0)
// [ 2] bdema_SequentialAllocator(GS g, Alloc *a = 0)
// [ 2] bdema_SequentialAllocator(AS a, Alloc *a = 0)
// [ 2] bdema_SequentialAllocator(GS g, AS a, Alloc *a = 0)
//
// [ 2] bdema_SequentialAllocator(int i, Alloc *a = 0)
// [ 2] bdema_SequentialAllocator(int i, GS g, Alloc *a = 0)
// [ 2] bdema_SequentialAllocator(int i, AS a, Alloc *a = 0)
// [ 2] bdema_SequentialAllocator(int i, GS g, AS a, Alloc *a = 0)
//
// [ 2] bdema_SequentialAllocator(int i, int m, Alloc *a = 0)
// [ 2] bdema_SequentialAllocator(int i, int m, GS g, Alloc *a = 0)
// [ 2] bdema_SequentialAllocator(int i, int m, AS a, Alloc *a = 0)
// [ 2] bdema_SequentialAllocator(int i, int m, GS g, AS a, Alloc *a = 0)
//
// [  ] ~bdema_SequentialAllocator()
//
// // MANIPULATORS
// [ 2] void *allocate(int size)
// [ 5] void *allocateAndExpand(int *size)
// [ 3] void deallocate(void *address)
// [ 4] void release()
// [ 7] void reserveCapacity(int numBytes)
// [ 6] int truncate(void *address, int originalSize, int newSize)
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE TEST
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
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

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
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
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL VARIABLES / TYPEDEF FOR TESTING
//-----------------------------------------------------------------------------
typedef bdema_SequentialAllocator Obj;

enum { DEFAULT_SIZE = 256 };

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// Allocators are often supplied, at construction, to objects requiring
// dynamically-allocated memory.  For example, consider the following
// 'my_DoubleStack' class whose constructor takes a 'bslma_Allocator':
//..
//  // my_doublestack.h
//  // ...
//
//  class bslma_Allocator;

    class my_DoubleStack {
        // This class implements a stack that stores 'double' values.

        // DATA
        double          *d_stack_p;      // dynamically-allocated array
        int              d_size;         // physical capacity of stack
        int              d_length;       // next available index in stack
        bslma_Allocator *d_allocator_p;  // memory allocator (held, not owned)

      private:
        // PRIVATE MANIPULATORS
        void increaseSize();
            // Increase the capacity of this stack by at least one element.

      public:
        // CREATORS
        my_DoubleStack(bslma_Allocator *basicAllocator = 0);
            // Create a stack that stores 'double'.  Optionally specify
            // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
            // the currently installed default allocator is used.

        ~my_DoubleStack();
            // Destroy this stack and all elements held by it.

        // ...

        // MANIPULATORS
        void push(double value);
            // Push the specified 'value' onto this stack.

        // ...
    };

    // ...

    // MANIPULATORS
    inline
    void my_DoubleStack::push(double value)
    {
        if (d_length >= d_size) {
            increaseSize();
        }
        d_stack_p[d_length++] = value;
    }

    // ...

//  // my_doublestack.cpp
//  #include <my_doublestack.h>
//
//  #include <bslma_allocator.h>
//  #include <bslma_default.h>

    // PRIVATE MANIPULATORS
    void my_DoubleStack::increaseSize()
    {
        // Implementation elided.
        // ...
    }

    // CREATORS
    my_DoubleStack::my_DoubleStack(bslma_Allocator *basicAllocator)
    : d_size(1)
    , d_length(0)
    , d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
        d_stack_p = static_cast<double *>(
                          d_allocator_p->allocate(d_size * sizeof *d_stack_p));
    }
//..
// Note that, when the allocator passed in is a 'bdema_SequentialAllocator',
// the 'deallocate' method is a no-op, and all memory is reclaimed during the
// destruction of the allocator:
//..
    my_DoubleStack::~my_DoubleStack()
    {
        // CLASS INVARIANTS
        ASSERT(d_allocator_p);
        ASSERT(d_stack_p);
        ASSERT(0 <= d_length);
        ASSERT(0 <= d_size);
        ASSERT(d_length <= d_size);

        d_allocator_p->deallocate(d_stack_p);
    }
//
//  // ...
//..
// In 'main', users can create a 'bdema_SequentialAllocator' and pass it to the
// constructor of 'my_DoubleStack':
//..
//  int main()
//  {
//      bdema_SequentialAllocator sequentialAlloc;
//      my_DoubleStack dstack(&sequentialAlloc);
//      // ...
//  }
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
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // As part of our overall allocator testing strategy, we will create
    // three test allocators.

    // Object Test Allocator.
    bslma_TestAllocator objectAllocator("Object Allocator",
                                        veryVeryVeryVerbose);

    // Default Test Allocator.
    bslma_TestAllocator defaultAllocator("Default Allocator",
                                         veryVeryVeryVerbose);
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);

    // Global Test Allocator.
    bslma_TestAllocator  globalAllocator("Global Allocator",
                                         veryVeryVeryVerbose);
    bslma_Allocator *originalGlobalAllocator =
                           bslma_Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Copy usage example from the header file, uncomment the code and
        //   change all 'assert' to 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

        bdema_SequentialAllocator sequentialAlloc;
        my_DoubleStack dstack(&sequentialAlloc);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // 'reserveCapacity' TEST
        //
        // Concerns:
        //   1) That if there are sufficient memory within the internal buffer,
        //      'reserveCapacity' should not trigger dynamic allocation.
        //
        //   2) That we can allocate at least the amount of bytes specified in
        //      'reserveCapacity' before triggering another dynamic allocation.
        //
        //   3) That 'reserveCapacity' can override the maximum buffer size
        //      parameter supplied to the pool at construction.
        //
        //   4) That invoking 'reserveCapacity' on a default constructed pool
        //      succeeds.
        //
        //   5) That invoking 'reserveCapacity' with 0 bytes can succeed with
        //      no dynamic memory allocation.
        //
        // Plan:
        //   Create a 'bdema_SequentialAllocator' using a test allocator and
        //   specify an initial size and maximum buffer size.
        //
        //   First, for concern 1, invoke 'reserveCapacity' with a size less
        //   than the initial size.  Allocate the same amount of memory, and
        //   verify, using the test allocator, that no new dynamic allocation
        //   is triggered.
        //
        //   Then, for concern 2, invoke 'reserveCapacity' with a size larger
        //   than the initial size.  Verify that the call triggers dynamic
        //   allocation, and that we can allocate the same amount of memory
        //   without triggering further dynamic allocation.
        //
        //   For concern 3, invoke 'reserveCapacity' with a size larger than
        //   the maximum buffer size.  Repeat verification for concern 2.
        //
        //   For concern 4, repeat the tests for concerns 1 and 2 with a
        //   default constructed sequential allocator.
        //
        //   For concern 5, invoke 'reserveCapacity' with a 0 size, and verify
        //   that no memory is allocated.
        //
        // Testing:
        //   void reserveCapacity(int numBytes)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'reserveCapacity' TEST" << endl
                                  << "======================" << endl;

        enum { INITIAL_SIZE = 64, MAX_BUFFER = INITIAL_SIZE * 4 };

        if (verbose) cout << "\nTesting that 'reserveCapacity' does not "
                             "trigger dynamic memory allocation." << endl;

        {
            Obj sa(INITIAL_SIZE, MAX_BUFFER, &objectAllocator);
            int numBytesUsed = objectAllocator.numBytesInUse();

            sa.reserveCapacity(INITIAL_SIZE / 2);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            sa.allocate(INITIAL_SIZE / 2);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            sa.reserveCapacity(INITIAL_SIZE * 2);
            ASSERT(numBytesUsed < objectAllocator.numBytesInUse());
            numBytesUsed = objectAllocator.numBytesInUse();

            sa.allocate(INITIAL_SIZE * 2);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            sa.reserveCapacity(MAX_BUFFER * 2);
            ASSERT(numBytesUsed < objectAllocator.numBytesInUse());
            numBytesUsed = objectAllocator.numBytesInUse();

            sa.allocate(MAX_BUFFER * 2);
            ASSERT(numBytesUsed = objectAllocator.numBytesInUse());
        }

        if (verbose) cout << "\nTesting 'reserveCapacity' on a default "
                             "constructed allocator." << endl;
        {
            Obj sa(&objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());

            sa.reserveCapacity(DEFAULT_SIZE);
            int numBytesUsed = objectAllocator.numBytesInUse();

            sa.allocate(DEFAULT_SIZE);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            sa.reserveCapacity(DEFAULT_SIZE * 4);
            ASSERT(numBytesUsed < objectAllocator.numBytesInUse());
            numBytesUsed = objectAllocator.numBytesInUse();

            sa.allocate(DEFAULT_SIZE * 4);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());
        }

        if (verbose) cout << "\nTesting 'reserveCapacity' with 0 bytes"
                          << endl;
        {
            Obj sa(&objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());

            sa.reserveCapacity(0);
            ASSERT(0 == objectAllocator.numBytesInUse());
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // 'truncate' TEST
        //
        // Concerns:
        //   1. That 'truncate' reduces the amount of memory allocated to the
        //      specified 'newSize'.
        //
        //   2. That when 'truncate' fails, 'originalSize' is returned.
        //
        // Plan:
        //   For concern 1, using the table-driven technique, create test
        //   vectors having the alignment strategy, initial allocation size,
        //   the new size, and expected offset.  First allocate memory of
        //   initial allocation size, then truncate to the new size, and
        //   allocate memory (1 byte) again.  Verify that the latest allocation
        //   matches the expected offset.
        //
        //   For concern 2, truncate the memory returned by the initial
        //   allocation, and verify that the return value is 'originalSize'.
        //
        // Testing:
        //   int truncate(void *address, int originalSize, int newSize)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TRUNCATE TEST" << endl
                                  << "=============" << endl;

        typedef bsls_Alignment::Strategy St;

        enum { MAX_ALIGN = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT };

#define NAT bsls_Alignment::BSLS_NATURAL
#define MAX bsls_Alignment::BSLS_MAXIMUM

        if (verbose) cout << "\nTesting 'truncate'." << endl;

        static const struct {
            int d_line;         // line number
            St  d_strategy;     // alignment strategy
            int d_initialSize;  // size of initial allocation request
            int d_newSize;      // truncate size
            int d_expOffset;    // expected memory offset
        } DATA[] = {
            // LINE     STRAT       INITIALSIZE   NEWSIZE   EXPOFFSET
            // ----     -----       -----------   -------   ---------

            // * - 'allocate' triggers dynamic memory allocation

            // NATURAL ALIGNMENT
            {  L_,      NAT,           1,           0,           0 },
            {  L_,      NAT,           1,           1,           1 },
            {  L_,      NAT,           2,           0,           0 },
            {  L_,      NAT,           2,           1,           1 },
            {  L_,      NAT,           2,           2,           2 },
            {  L_,      NAT,           3,           0,           0 },
            {  L_,      NAT,           3,           1,           1 },
            {  L_,      NAT,           3,           2,           2 },
            {  L_,      NAT,           3,           3,           3 },
            {  L_,      NAT,           8,           4,           4 },
            {  L_,      NAT,         511,         511,         511 },
            {  L_,      NAT,         512,           0,           0 },
            {  L_,      NAT,         512,         511,         511 },
            {  L_,      NAT,         512,         512,          -1 },  // *

            // MAXIMUM ALIGNMENT
            {  L_,      MAX,           1,           0,           0 },
            {  L_,      MAX,           1,           1,   MAX_ALIGN },
            {  L_,      MAX,           2,           0,           0 },
            {  L_,      MAX,           2,           1,   MAX_ALIGN },
            {  L_,      MAX,           2,           2,   MAX_ALIGN },
            {  L_,      MAX,           3,           0,           0 },
            {  L_,      MAX,           3,           1,   MAX_ALIGN },
            {  L_,      MAX,           3,           2,   MAX_ALIGN },
            {  L_,      MAX,           3,           3,   MAX_ALIGN },
            {  L_,      MAX,           8,           4,   MAX_ALIGN },
            {  L_,      MAX,         511,         511,          -1 },  // *
            {  L_,      MAX,         512,           0,           0 },
            {  L_,      MAX,         512,           1,   MAX_ALIGN },
            {  L_,      MAX,         512,         511,          -1 },  // *
            {  L_,      MAX,         512,         512,          -1 },  // *
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE        = DATA[ti].d_line;         // line number
            const St  STRAT       = DATA[ti].d_strategy;     // alignment strat
            const int INITIALSIZE = DATA[ti].d_initialSize;  // initial size
            const int NEWSIZE     = DATA[ti].d_newSize;      // new size
            const int EXPOFFSET   = DATA[ti].d_expOffset;    // expected offset

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL_ALIGNMENT, ";
                }
                else {
                    cout << "STRAT = MAXIMUM_ALIGNMENT, ";
                }
                P_(INITIALSIZE) P_(NEWSIZE) P(EXPOFFSET)
            }

            Obj sa(STRAT, &objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());

            void *addr1 = sa.allocate(INITIALSIZE);
            ASSERT(0 != objectAllocator.numBytesInUse());

            int used = objectAllocator.numBytesInUse();

            sa.truncate(addr1, INITIALSIZE, NEWSIZE);
            ASSERT(used == objectAllocator.numBytesInUse());

            void *addr2 = sa.allocate(1);
            if (EXPOFFSET >= 0) {
                ASSERT(used == objectAllocator.numBytesInUse());
                LOOP4_ASSERT(LINE, EXPOFFSET, addr1, addr2,
                             (char *)addr1 + EXPOFFSET == (char *)addr2);
            }
            else {
                ASSERT(used < objectAllocator.numBytesInUse());
            }

            // Truncating previously allocated address should fail.
            if (EXPOFFSET >= 0 &&
                        ((char *)addr1 + INITIALSIZE) != ((char *)addr2 + 1)) {
                int ret = sa.truncate(addr1, INITIALSIZE, NEWSIZE);
                LOOP2_ASSERT(INITIALSIZE, ret, INITIALSIZE == ret);
            }
        }

#undef MAX
#undef NAT

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'allocateAndExpand' TEST
        //
        // Concerns:
        //   1) That 'allocateAndExpand' returns the maximum amount of memory
        //      available for use without triggering another allocation.
        //
        //   2) That 'allocateAndExpand' returns the updated size of memory
        //      used.
        //
        //   3) That 'allocateAndExpand' has no effect when passed a size of
        //      0.
        //
        // Plan:
        //   For both concern, using the table-driven technique, create test
        //   vectors having the alignment strategy, initial memory offset and
        //   expected memory used.  First allocate memory necessary for the
        //   initial memory offset, then allocate 1 byte using
        //   'allocateAndExpand'.  Verify the updated size is the same as the
        //   expected memory used.  Finally, invoke 'allocate' again and verify
        //   it triggers new dynamic memory allocation - meaning
        //   'allocateAndExpand' did use up all available memory in the
        //   internal buffer.
        //
        // Testing:
        //   void *allocateAndExpand(int *size)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'allocateAndExpand' TEST" << endl
                                  << "========================" << endl;

        typedef bsls_Alignment::Strategy St;

        enum { MAX_ALIGN = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT };

#define NAT bsls_Alignment::BSLS_NATURAL
#define MAX bsls_Alignment::BSLS_MAXIMUM

        if (verbose) cout << "\nTesting 'expand'." << endl;

        static const struct {
            int d_line;         // line number
            St  d_strategy;     // alignment strategy
            int d_initialSize;  // size of initial allocation request
            int d_expused;      // expected memory used after 'expand'
        } DATA[] = {
            // LINE     STRAT       INITIALSIZE       EXPUSED
            // ----     -----       -----------       -------

            // NATURAL ALIGNMENT
            {  L_,      NAT,           1,     DEFAULT_SIZE * 2 - 1           },
            {  L_,      NAT,           2,     DEFAULT_SIZE * 2 - 2           },
            {  L_,      NAT,           3,     DEFAULT_SIZE * 2 - 3           },
            {  L_,      NAT,           4,     DEFAULT_SIZE * 2 - 4           },
            {  L_,      NAT,           7,     DEFAULT_SIZE * 2 - 7           },
            {  L_,      NAT,           8,     DEFAULT_SIZE * 2 - 8           },
            {  L_,      NAT,          15,     DEFAULT_SIZE * 2 - 15          },
            {  L_,      NAT,          16,     DEFAULT_SIZE * 2 - 16          },
            {  L_,      NAT,         100,     DEFAULT_SIZE * 2 - 100         },
            {  L_,      NAT,         510,     DEFAULT_SIZE * 2 - 510         },
            {  L_,      NAT,         511,     DEFAULT_SIZE * 2 - 511         },

            // MAXIMUM ALIGNMENT
            {  L_,      MAX,           1,     DEFAULT_SIZE * 2 - MAX_ALIGN   },
            {  L_,      MAX,           2,     DEFAULT_SIZE * 2 - MAX_ALIGN   },
            {  L_,      MAX,           3,     DEFAULT_SIZE * 2 - MAX_ALIGN   },
            {  L_,      MAX,           4,     DEFAULT_SIZE * 2 - MAX_ALIGN   },
            {  L_,      MAX,           7,     DEFAULT_SIZE * 2 - MAX_ALIGN   },
            {  L_,      MAX,           8,     DEFAULT_SIZE * 2 - MAX_ALIGN   },
            {  L_,      MAX,          15,     DEFAULT_SIZE * 2 - 16          },
            {  L_,      MAX,          16,     DEFAULT_SIZE * 2 - 16          },
            {  L_,      MAX,         108,     DEFAULT_SIZE * 2 - 112         },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE        = DATA[ti].d_line;
            const St  STRAT       = DATA[ti].d_strategy;
            const int INITIALSIZE = DATA[ti].d_initialSize;
            const int EXPUSED     = DATA[ti].d_expused;

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL_ALIGNMENT, ";
                }
                else {
                    cout << "STRAT = MAXIMUM_ALIGNMENT, ";
                }
                P_(INITIALSIZE) P(EXPUSED)
            }

            Obj sa(STRAT, &objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());
            ASSERT(0 == objectAllocator.numBlocksInUse());

            void *addr1 = sa.allocate(INITIALSIZE);
            int numBytesUsed = objectAllocator.numBytesInUse();
            ASSERT(1 == objectAllocator.numBlocksInUse());

            bsls_PlatformUtil::size_type size = 1;
            void *addr2 = sa.allocateAndExpand(&size);

            // Check for correct memory address.
            if (bsls_Alignment::BSLS_NATURAL == STRAT) {
                ASSERT((char *)addr1 + INITIALSIZE == (char *)addr2);
            }
            else {
                int offset = bsls_AlignmentUtil::calculateAlignmentOffset(
                                       (char *)addr1 + INITIALSIZE,
                                       bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);
                ASSERT((char *)addr1 + INITIALSIZE + offset == (char *)addr2);
            }

            // Check 'size' is updated correctly.
            ASSERT(EXPUSED == size);

            // Check for no new allocations.
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());
            ASSERT(1 == objectAllocator.numBlocksInUse());

            // Check for new allocations.
            sa.allocate(1);
            ASSERT(2 == objectAllocator.numBlocksInUse());
        }
        ASSERT(0 == objectAllocator.numBytesInUse());

        {
            Obj sa(&objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());
            ASSERT(0 == objectAllocator.numBlocksInUse());

            // No effect when 0 is passed in.
            bsls_PlatformUtil::size_type size = 0;
            sa.allocateAndExpand(&size);
            ASSERT(0 == objectAllocator.numBlocksInUse());

            // No initial allocation, just allocate and expand directly.
            size = 1;
            sa.allocateAndExpand(&size);
            ASSERT(1 == objectAllocator.numBlocksInUse());

            sa.allocate(1);
            ASSERT(2 == objectAllocator.numBlocksInUse());
        }
        ASSERT(0 == objectAllocator.numBytesInUse());

#undef MAX
#undef NAT

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'release' TEST
        //
        // Concerns:
        //   1) That all memory allocated from the allocator supplied at
        //      construction are deallocated after 'release'.
        //
        //   2) That subsequent allocation requests after invocation of the
        //      'release' method retains the specified growth and alignment
        //      strategy.
        //
        // Plan:
        //   For concern 1 and 2, construct a sequential allocator using a
        //   'bslma_TestAllocator', and allocate several memory blocks such
        //   that there are multiple dynamic allocations.  Finally, invoke
        //   'release' and verify, using the test allocator, that there
        //   is no outstanding memory allocated.  Then, allocate memory again
        //   and verify the alignment and growth strategy.
        //
        // Testing:
        //   void release()
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'release' TEST" << endl
                                  << "==============" << endl;

#define CON bsls_BlockGrowth::BSLS_CONSTANT
#define MAX bsls_Alignment::BSLS_MAXIMUM

        if (verbose) cout << "\nTesting allocated memory are deallocated after"
                             " 'release'." << endl;
        {
            Obj sa(CON, MAX, &objectAllocator);

            ASSERT(0 == objectAllocator.numBlocksInUse());

            // Allocate some memory such that memory is allocated from the test
            // allocator.
            sa.allocate(1000);
            ASSERT(1 == objectAllocator.numBlocksInUse());

            sa.allocate(1000);
            ASSERT(2 == objectAllocator.numBlocksInUse());

            sa.allocate(1000);
            ASSERT(3 == objectAllocator.numBlocksInUse());

            // Release all memory.
            sa.release();

            ASSERT(0 == objectAllocator.numBlocksInUse());

            if (verbose) cout << "\nTesting alignment and growth strategies."
                              << endl;

            // Testing alignment.
            void *addr1 = sa.allocate(1);
            void *addr2 = sa.allocate(2);

            ASSERT((char *)addr1 + bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT
                                                             == (char *)addr2);

            // Testing growth strategy.
            int numBytesUsed = objectAllocator.numBytesInUse();
            sa.allocate(DEFAULT_SIZE / 2);
            ASSERT(objectAllocator.numBytesInUse() == numBytesUsed);

            // Because of alignment concerns, we just test that the number of
            // bytes used are within the small range.  If the memory growth
            // is outside of this range, the allocator is using geometric
            // growth.

            sa.allocate(DEFAULT_SIZE / 2);
            ASSERT(objectAllocator.numBytesInUse()
                                              >= numBytesUsed + DEFAULT_SIZE);
            ASSERT(objectAllocator.numBytesInUse() <= numBytesUsed +
                        DEFAULT_SIZE + bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);
        }

#undef CON
#undef MAX

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'deallocate' TEST
        //
        // Concerns:
        //   That 'deallocate' has no effect.
        //
        // Plan:
        //   Create a sequential allocator initialized with a test allocator.
        //   Request memory of varying sizes and then deallocate each memory.
        //   Verify that the number of bytes in use indicated by the test
        //   allocator does not decrease after each 'deallocate' method
        //   invocation.
        //
        // Testing:
        //   void deallocate(void *address)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'deallocate' TEST" << endl
                                  << "=================" << endl;

        if (verbose) cout << "\nTesting 'deallocate'." << endl;

        const int DATA[] = { 0, 1, 5, 12, 24, 32, 64, 256, 257, 512, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        Obj sa(&objectAllocator);

        int lastNumBytesInUse = objectAllocator.numBytesInUse();

        for (int i = 0; i < NUM_DATA; ++i) {
            const int SIZE = DATA[i];
            void *p = sa.allocate(SIZE);
            const int numBytesInUse = objectAllocator.numBytesInUse();
            sa.deallocate(p);
            LOOP_ASSERT(i, numBytesInUse == objectAllocator.numBytesInUse());
            LOOP_ASSERT(i, lastNumBytesInUse <=
                                              objectAllocator.numBytesInUse());
            lastNumBytesInUse = objectAllocator.numBytesInUse();
        }

#undef MAX
#undef NAT
#undef CON
#undef GEO

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CTOR / ALLOCATE TEST
        //
        // Concerns:
        //   1) That the 'bdema_SequentialAllocator' correctly proxies its
        //      constructor argument and allocation request to the
        //      'bdema_SequentialPool' it adopts.
        //
        //   2) That allocating 0 bytes should return 0.
        //
        // Plan:
        //   Using the array driven approach, create an array of various memory
        //   allocation request sizes.  Then, create both a sequential
        //   allocator and sequential pool using two different test allocators.
        //   Finally, verify that memory usage of both test allocators are the
        //   same.
        //
        // Testing:
        //   bdema_SequentialAllocator(Alloc *a = 0)
        //   bdema_SequentialAllocator(GS g, Alloc *a = 0)
        //   bdema_SequentialAllocator(AS a, Alloc *a = 0)
        //   bdema_SequentialAllocator(GS g, AS a, Alloc *a = 0)
        //
        //   bdema_SequentialAllocator(int i, Alloc *a = 0)
        //   bdema_SequentialAllocator(int i, GS g, Alloc *a = 0)
        //   bdema_SequentialAllocator(int i, AS a, Alloc *a = 0)
        //   bdema_SequentialAllocator(int i, GS g, AS a, Alloc *a = 0)
        //
        //   bdema_SequentialAllocator(int i, int m, Alloc *a = 0)
        //   bdema_SequentialAllocator(int i, int m, GS g, Alloc *a = 0)
        //   bdema_SequentialAllocator(int i, int m, AS a, Alloc *a = 0)
        //   bdema_SequentialAllocator(int i, int m, GS g, AS a, Alloc *a = 0)
        //
        //   void *allocate(int size)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CTOR / ALLOCATE TEST" << endl
                                  << "====================" << endl;

        enum { INITIAL_SIZE = 64, MAX_BUFFER = 256 };

        const int DATA[]   = { 2, 5, 7, 8, 15, 16, 24, 31, 32, 33, 48,
                               63, 64, 65, 66, 127, 128, 129, 255, 256,
                               511, 512, 1023, 1024, 1025 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

#define GEO bsls_BlockGrowth::BSLS_GEOMETRIC
#define CON bsls_BlockGrowth::BSLS_CONSTANT
#define NAT bsls_Alignment::BSLS_NATURAL
#define MAX bsls_Alignment::BSLS_MAXIMUM

        // block growth strategy
        const bsls_BlockGrowth::Strategy GS[2] = { GEO, CON };
        const int NUM_GS = sizeof GS / sizeof *GS;

        // alignment strategy
        const bsls_Alignment::Strategy   AS[2] = { NAT, MAX };
        const int NUM_AS = sizeof AS / sizeof *AS;

        if (verbose) cout << "\nTesting 'Obj(Alloc *a = 0)'." << endl;
        {
            bslma_TestAllocator ta(veryVeryVeryVerbose);
            bslma_TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                Obj                  sa(&ta);
                bdema_SequentialPool pool(&tb);

                void *addr1 = sa.allocate(SIZE);
                void *addr2 = pool.allocate(SIZE);

                ASSERT(0 != addr1);
                ASSERT(0 != addr2);

                ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
            }
        }

        if (verbose) cout << "\nTesting 'Obj(GS g, Alloc *a = 0)'." << endl;
        {
            bslma_TestAllocator ta(veryVeryVeryVerbose);
            bslma_TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_GS; ++j) {
                    Obj                  sa(GS[j], &ta);
                    bdema_SequentialPool pool(GS[j], &tb);

                    void *addr1 = sa.allocate(SIZE);
                    void *addr2 = pool.allocate(SIZE);

                    ASSERT(0 != addr1);
                    ASSERT(0 != addr2);

                    ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
                }
            }
        }

        if (verbose) cout << "\nTesting 'Obj(AS a, Alloc *a = 0)'." << endl;
        {
            bslma_TestAllocator ta(veryVeryVeryVerbose);
            bslma_TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {
                    Obj                  sa(AS[j], &ta);
                    bdema_SequentialPool pool(AS[j], &tb);

                    void *addr1 = sa.allocate(SIZE);
                    void *addr2 = pool.allocate(SIZE);

                    ASSERT(0 != addr1);
                    ASSERT(0 != addr2);

                    ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
                    int numBytesUsed = tb.numBytesInUse();

                    void *addr1b = sa.allocate(2);
                    void *addr2b = pool.allocate(2);

                    // Verify that the alignment strategy between the pool and
                    // the allocator is the same by adding the alignment offset
                    // of the pool to the allocator.  The alignment offset is
                    // only sensible if memory comes from the same internal
                    // buffer.

                    if (numBytesUsed == tb.numBytesInUse()) {
                        int alignmentOffset = (char *)addr2b - (char *)addr2;
                        ASSERT((char *)addr1 + alignmentOffset
                                                            == (char *)addr1b);
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting 'Obj(GS g, AS a, Alloc *a = 0)'."
                          << endl;
        {
            bslma_TestAllocator ta(veryVeryVeryVerbose);
            bslma_TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {  // alignment strategy

                    for (int k = 0; k < NUM_GS; ++k) {  // growth strategy

                        Obj                  sa(GS[k], AS[j], &ta);
                        bdema_SequentialPool pool(GS[k], AS[j], &tb);

                        void *addr1 = sa.allocate(SIZE);
                        void *addr2 = pool.allocate(SIZE);

                        ASSERT(0 != addr1);
                        ASSERT(0 != addr2);

                        ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
                        int numBytesUsed = tb.numBytesInUse();

                        void *addr1b = sa.allocate(2);
                        void *addr2b = pool.allocate(2);

                        if (numBytesUsed == tb.numBytesInUse()
                         && GS[k] != CON) {
                            int alignmentOffset =
                                                (char *)addr2b - (char *)addr2;

                            ASSERT((char *)addr1 + alignmentOffset
                                                            == (char *)addr1b);
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting 'Obj(int i, Alloc *a = 0)'." << endl;
        {
            bslma_TestAllocator ta(veryVeryVeryVerbose);
            bslma_TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                Obj                  sa(INITIAL_SIZE, &ta);
                bdema_SequentialPool pool(INITIAL_SIZE, &tb);

                void *addr1 = sa.allocate(SIZE);
                void *addr2 = pool.allocate(SIZE);

                ASSERT(0 != addr1);
                ASSERT(0 != addr2);

                ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
            }
        }

        if (verbose) cout << "\nTesting 'Obj(int i, GS g, Alloc *a = 0)'."
                          << endl;
        {
            bslma_TestAllocator ta(veryVeryVeryVerbose);
            bslma_TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_GS; ++j) {
                    Obj                  sa(INITIAL_SIZE, GS[j], &ta);
                    bdema_SequentialPool pool(INITIAL_SIZE, GS[j], &tb);

                    void *addr1 = sa.allocate(SIZE);
                    void *addr2 = pool.allocate(SIZE);

                    ASSERT(0 != addr1);
                    ASSERT(0 != addr2);

                    ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
                }
            }
        }

        if (verbose) cout << "\nTesting 'Obj(int i, AS a, Alloc *a = 0)'."
                          << endl;
        {
            bslma_TestAllocator ta(veryVeryVeryVerbose);
            bslma_TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {
                    Obj                  sa(INITIAL_SIZE, AS[j], &ta);
                    bdema_SequentialPool pool(INITIAL_SIZE, AS[j], &tb);

                    void *addr1 = sa.allocate(SIZE);
                    void *addr2 = pool.allocate(SIZE);

                    ASSERT(0 != addr1);
                    ASSERT(0 != addr2);

                    ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
                    int numBytesUsed = tb.numBytesInUse();

                    void *addr1b = sa.allocate(2);
                    void *addr2b = pool.allocate(2);

                    // Verify that the alignment strategy between the pool and
                    // the allocator is the same by adding the alignment offset
                    // of the pool to the allocator.  The alignment offset is
                    // only sensible if memory comes from the same internal
                    // buffer.

                    if (numBytesUsed == tb.numBytesInUse()) {
                        int alignmentOffset = (char *)addr2b - (char *)addr2;
                        ASSERT((char *)addr1 + alignmentOffset
                                                            == (char *)addr1b);
                    }
                }
            }
        }

        if (verbose) cout <<"\nTesting 'Obj(int i, GS g, AS a, Alloc *a = 0)'."
                          << endl;
        {
            bslma_TestAllocator ta(veryVeryVeryVerbose);
            bslma_TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {  // alignment strategy

                    if (veryVerbose) { T_ T_ P(j) }

                    for (int k = 0; k < NUM_GS; ++k) {  // growth strategy

                        if (veryVerbose) { T_ T_ T_ P(k) }

                        Obj sa(INITIAL_SIZE, GS[k], AS[j], &ta);

                        bdema_SequentialPool pool(INITIAL_SIZE, GS[k], AS[j],
                                                                          &tb);

                        void *addr1 = sa.allocate(SIZE);
                        void *addr2 = pool.allocate(SIZE);

                        ASSERT(0 != addr1);
                        ASSERT(0 != addr2);

                        ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
                        int numBytesUsed = tb.numBytesInUse();

                        void *addr1b = sa.allocate(2);
                        void *addr2b = pool.allocate(2);

                        if (numBytesUsed == tb.numBytesInUse()
                         && GS[k] != CON) {
                            int alignmentOffset =
                                                (char *)addr2b - (char *)addr2;

                            LOOP3_ASSERT(addr1, addr1b, alignmentOffset, (char *)addr1 + alignmentOffset
                                                            == (char *)addr1b);
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting 'Obj(int i, int m, Alloc *a = 0)'."
                          << endl;
        {
            bslma_TestAllocator ta(veryVeryVeryVerbose);
            bslma_TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                Obj                  sa(INITIAL_SIZE, MAX_BUFFER, &ta);
                bdema_SequentialPool pool(INITIAL_SIZE, MAX_BUFFER, &tb);

                void *addr1 = sa.allocate(SIZE);
                void *addr2 = pool.allocate(SIZE);

                ASSERT(0 != addr1);
                ASSERT(0 != addr2);

                ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
            }
        }

        if (verbose) cout <<
            "\nTesting 'Obj(int i, int m, GS g, Alloc *a = 0)'." << endl;
        {
            bslma_TestAllocator ta(veryVeryVeryVerbose);
            bslma_TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_GS; ++j) {
                    Obj sa(INITIAL_SIZE, MAX_BUFFER, GS[j], &ta);

                    bdema_SequentialPool pool(INITIAL_SIZE, MAX_BUFFER, GS[j],
                                                                          &tb);

                    void *addr1 = sa.allocate(SIZE);
                    void *addr2 = pool.allocate(SIZE);

                    ASSERT(0 != addr1);
                    ASSERT(0 != addr2);

                    ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
                }
            }
        }

        if (verbose) cout <<
                  "\nTesting 'Obj(int i, int m, AS a, Alloc *a = 0)'." << endl;
        {
            bslma_TestAllocator ta(veryVeryVeryVerbose);
            bslma_TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {
                    Obj sa(INITIAL_SIZE, MAX_BUFFER, AS[j], &ta);

                    bdema_SequentialPool pool(INITIAL_SIZE, MAX_BUFFER, AS[j],
                                                                          &tb);

                    void *addr1 = sa.allocate(SIZE);
                    void *addr2 = pool.allocate(SIZE);

                    ASSERT(0 != addr1);
                    ASSERT(0 != addr2);

                    ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
                    int numBytesUsed = tb.numBytesInUse();

                    void *addr1b = sa.allocate(2);
                    void *addr2b = pool.allocate(2);

                    // Verify that the alignment strategy between the pool and
                    // the allocator is the same by adding the alignment offset
                    // of the pool to the allocator.  The alignment offset is
                    // only sensible if memory comes from the same internal
                    // buffer.

                    if (numBytesUsed == tb.numBytesInUse()
                     && SIZE < MAX_BUFFER) {
                        int alignmentOffset = (char *)addr2b - (char *)addr2;
                        ASSERT((char *)addr1 + alignmentOffset
                                                            == (char *)addr1b);
                    }
                }
            }
        }

        if (verbose) cout <<
            "\nTesting 'Obj(int i, int m, GS g, AS a, Alloc *a = 0)'." << endl;
        {
            bslma_TestAllocator ta(veryVeryVeryVerbose);
            bslma_TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {  // alignment strategy

                    for (int k = 0; k < NUM_GS; ++k) {  // growth strategy

                        Obj sa(INITIAL_SIZE, MAX_BUFFER, GS[k], AS[j], &ta);

                        bdema_SequentialPool pool(INITIAL_SIZE, MAX_BUFFER,
                                                            GS[k], AS[j], &tb);

                        void *addr1 = sa.allocate(SIZE);
                        void *addr2 = pool.allocate(SIZE);

                        ASSERT(0 != addr1);
                        ASSERT(0 != addr2);

                        ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
                        int numBytesUsed = tb.numBytesInUse();

                        void *addr1b = sa.allocate(2);
                        void *addr2b = pool.allocate(2);

                        if (numBytesUsed == tb.numBytesInUse()
                         && GS[k] != CON && SIZE < MAX_BUFFER) {
                            int alignmentOffset =
                                                (char *)addr2b - (char *)addr2;

                            ASSERT((char *)addr1 + alignmentOffset
                                                            == (char *)addr1b);
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting allocation size of 0 bytes." << endl;
        {
            Obj sa(&objectAllocator);
            void *addr = sa.allocate(0);
            ASSERT(0 == addr);
            ASSERT(0 == objectAllocator.numBytesInUse());
        }
        ASSERT(0 == objectAllocator.numBlocksTotal());
        ASSERT(0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1) That a 'bdema_SequentialAllocator' can be created and
        //      destroyed.
        //
        //   2) That 'allocate' returns a block of memory having the specified
        //      size and alignment strategy.
        //
        //   2) That 'allocate' does not always causes dynamic allocation
        //      (i.e., the allocator manages an internal buffer of memory)
        //
        //   3) That 'allocate' returns a block of memory even when the
        //      the allocation request exceeds the initial size of the internal
        //      buffer.
        //
        //   4) Destruction of the allocator releases all managed memory.
        //
        // Plan:
        //   First, initialize a 'bdema_SequentialAllocator' with a
        //   'bslma_TestAllocator' (concern 1).  Then, allocate a block of
        //   memory, and verify that it comes from the test allocator.
        //   Allocate another block of memory, and verify that no dynamic
        //   allocation is triggered (concern 3).  Verify the alignment and
        //   size of the first allocation by checking the address of the
        //   second allocation (concern 2).
        //
        //   Then, allocate a large block of memory and verify the memory comes
        //   from the test allocator (concern 4).  Finally, destroy the
        //   allocator and check that all allocated memory are deallocated
        //   (concern 5).
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATING TEST" << endl
                                  << "=============" << endl;

        enum { ALLOC_SIZE1 = 4, ALLOC_SIZE2 = 8, ALLOC_SIZE3 = 1024 };

        {
            if (verbose) cout << "\nTesting construction of allocator."
                              << endl;
            Obj sa(&objectAllocator);

            // Make sure no memory comes from the object, default and global
            // allocators.
            ASSERT(0 == objectAllocator.numBlocksTotal());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            if (verbose) cout << "\nTesting allocation." << endl;
            void *addr1 = sa.allocate(ALLOC_SIZE1);

            ASSERT(0 != objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            int oldNumBytesInUse = objectAllocator.numBytesInUse();

            if (verbose) cout << "\nTesting internal buffering." << endl;
            void *addr2 = sa.allocate(ALLOC_SIZE2);

            ASSERT(oldNumBytesInUse == objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            if (verbose) cout << "\nTesting alignment strategy." << endl;

            // Check for alignment and size of first allocation)
            ASSERT((char *)addr1 + 8 == (char *)addr2);

            if (verbose) cout << "\nTesting large allocation." << endl;
            void *addr3 = sa.allocate(ALLOC_SIZE3);

            ASSERT(oldNumBytesInUse < objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());
        }

        if (verbose) cout << "\nTesting destruction." << endl;
        ASSERT(0 == objectAllocator.numBytesInUse());
        ASSERT(0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

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
