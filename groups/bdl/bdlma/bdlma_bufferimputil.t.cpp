// bdlma_bufferimputil.t.cpp                                          -*-C++-*-
#include <bdlma_bufferimputil.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>

#include <bsls_alignedbuffer.h>
#include <bsls_alignment.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// 'bdlma::BufferImpUtil' provides several static methods for allocating a
// contiguous block of memory from an external buffer.  The address of the
// allocated memory block depends on several parameters: the current cursor
// position, the size of the allocation, the alignment strategy (which is
// possibly implied by the name on an alignment-specific method), and also the
// buffer size (necessary to detect overflow with the non-raw methods).  Due to
// the large amount of possible combinations, only the corner cases are
// selected as test data.  These data are then organized first by allocation
// size, then current cursor position (both of which affects the address of the
// allocated memory block depending on the indicated alignment strategy) in a
// table format.
//
// Negative testing (for precondition violations) is also part of the test
// driver.
//-----------------------------------------------------------------------------
// [ 1] void *allocateFromBuffer(cur, buf, bs, sz, Strat);
// [ 1] void *allocateMaximallyAlignedFromBuffer(cur, buf, bs, sz, Strat);
// [ 1] void *allocateNaturallyAlignedFromBuffer(cur, buf, bs, sz, Strat);
// [ 1] void *allocateOneByteAlignedFromBuffer(cur, buf, bs, sz, Strat);
// [ 1] void *allocateFromBufferRaw(cur, buf, sz, Strat);
// [ 1] void *allocateMaximallyAlignedFromBufferRaw(cur, buf, sz, Strat);
// [ 1] void *allocateNaturallyAlignedFromBufferRaw(cur, buf, sz, Strat);
// [ 1] void *allocateOneByteAlignedFromBufferRaw(cur, buf, sz, Strat);
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
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
//                  GLOBAL VARIABLES / TYPEDEF FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlma::BufferImpUtil Obj;

// On Windows, when an 8-byte aligned object is created on the stack, it
// actually gets aligned on a 4-byte boundary.  To work around this, create a
// static buffer instead.

enum { k_BUFFER_SIZE = 256 };
static bsls::AlignedBuffer<k_BUFFER_SIZE> bufferStorage;

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This component is typically used by a class that manages a memory buffer.
// First, suppose we have a class that maintains a linked list of memory
// blocks, details of which are elided:
//..
    class BlockList {
        // ...
    };
//..
// We can then create our memory manager using 'BlockList':
//..
    class my_SequentialPool {
        // This class allocates memory from an internal pool of memory buffers
        // using natural alignment.  All allocated memory is managed internally
        // by the pool and released when the pool is destroyed.

        // DATA
        char      *d_buffer_p;    // pointer to current buffer
        int        d_bufferSize;  // size (in bytes) of the current buffer
        int        d_cursor;      // byte offset to unused memory in buffer
        BlockList  d_blockList;   // used to replenish memory

      private:
        // PRIVATE MANIPULATORS
        void replenishBuffer(int size);
            // Replenish the current buffer with memory that satisfies an
            // allocation request having at least the specified 'size' (in
            // bytes).

      public:
        // CREATORS
        explicit my_SequentialPool(bslma::Allocator *basicAllocator = 0);
            // Create a memory pool that dispenses heterogeneous blocks of
            // memory (of varying, user-specified sizes).  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        ~my_SequentialPool();
            // Destroy this memory pool and release all associated memory.

        // MANIPULATORS
        void *allocate(int size);
            // Return the address of a contiguous block of naturally-aligned
            // memory of the specified 'size' (in bytes).  The behavior is
            // undefined unless '0 < size'.
    };
//..
// The implementations of the constructor and destructor are elided since
// 'allocate' alone is sufficient to illustrate the use of
// 'bdlma::BufferImpUtil':
//..
    void *my_SequentialPool::allocate(int size)
    {
        ASSERT(0 < size);

        void *address = bdlma::BufferImpUtil::allocateFromBuffer(
                                                &d_cursor,
                                                d_buffer_p,
                                                d_bufferSize,
                                                size,
                                                bsls::Alignment::BSLS_NATURAL);
//..
// Note that if there is insufficient space in 'd_buffer_p',
// 'allocateFromBuffer' returns 0:
//..
        if (address) {
            return address;                                           // RETURN
        }

        replenishBuffer(size);

        return bdlma::BufferImpUtil::allocateFromBufferRaw(
                                                &d_cursor,
                                                d_buffer_p,
                                                size,
                                                bsls::Alignment::BSLS_NATURAL);
    }
//..
// Note that the *raw* version is used because the contract of
// 'replenishBuffer' guarantees that the buffer will have sufficient space to
// satisfy the allocation request of the specified 'size'.

//=============================================================================
//           Additional Functionality Needed to Complete Usage Test Case
//-----------------------------------------------------------------------------

void my_SequentialPool::replenishBuffer(int size)
{
    // ...

    (void)size;
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
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

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // CLASS METHODS TEST
        //
        // Concerns:
        //   1) That the address of the allocated memory block is correctly
        //      aligned according to the indicated alignment strategy.
        //
        //   2) That the allocated memory block has the specified size.
        //
        //   3) That the block is allocated from the specified buffer at the
        //      expected offset from the specified cursor position.
        //
        //   4) That the cursor is updated to the position of the first byte
        //      immediately after the allocated memory.
        //
        //   5) That when a non-raw method is used and the allocation request
        //      would cause the capacity of the buffer to be exceeded, 0 is
        //      returned and the cursor is not effected.
        //
        //   6) QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   Using the table-driven technique, create a set of test vectors
        //   having different allocation sizes, alignment strategies, initial
        //   cursor positions, expected final cursor positions, and expected
        //   memory offsets.  Verify that invoking the class methods (where
        //   applicable) on the various test vectors produces the expected
        //   results.
        //
        //   In addition, verify that in appropriate build modes, defensive
        //   checks are triggered.
        //
        // Testing:
        //   void *allocateFromBuffer(cur, buf, bs, sz, Strat);
        //   void *allocateMaximallyAlignedFromBuffer(cur, buf, bs, sz, Strat);
        //   void *allocateNaturallyAlignedFromBuffer(cur, buf, bs, sz, Strat);
        //   void *allocateOneByteAlignedFromBuffer(cur, buf, bs, sz, Strat);
        //   void *allocateFromBufferRaw(cur, buf, sz, Strat);
        //   void *allocateMaximallyAlignedFromBufferRaw(cur, buf, sz, Strat);
        //   void *allocateNaturallyAlignedFromBufferRaw(cur, buf, sz, Strat);
        //   void *allocateOneByteAlignedFromBufferRaw(cur, buf, sz, Strat);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CLASS METHODS TEST" << endl
                                  << "==================" << endl;

        typedef bsls::Alignment::Strategy Strat;

        enum { k_MA = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT };

#define NAT bsls::Alignment::BSLS_NATURAL
#define MAX bsls::Alignment::BSLS_MAXIMUM
#define BYT bsls::Alignment::BSLS_BYTEALIGNED

        if (verbose) cout << "\nTesting expected cursor and offset." << endl;
        {

        static const struct {
            int   d_line;       // line number
            int   d_cursor;     // initial cursor position
            int   d_bufSize;    // buffer size
            int   d_allocSize;  // allocation request size
            Strat d_strategy;   // alignment strategy
            int   d_expOffset;  // expected memory offset
            int   d_expCursor;  // expected cursor position after request
        } DATA[] = {

   // LINE  CURSOR  BUFSIZE   ALLOCSIZE   STRAT   EXPOFFSET  EXPCURSOR
   // ----  ------  -------   ---------   -----   --------   ---------

   // NATURAL ALIGNMENT
   {  L_,    0,     64,       1,          NAT,     0,        1 },// ALLOCSIZE=1
   {  L_,    1,     64,       1,          NAT,     1,        2 },
   {  L_,    2,     64,       1,          NAT,     2,        3 },
   {  L_,    3,     64,       1,          NAT,     3,        4 },
   {  L_,    7,     64,       1,          NAT,     7,        8 },
   {  L_,    8,     64,       1,          NAT,     8,        9 },
   {  L_,   15,     64,       1,          NAT,    15,       16 },
   {  L_,   16,     64,       1,          NAT,    16,       17 },
   {  L_,   31,     64,       1,          NAT,    31,       32 },
   {  L_,   32,     64,       1,          NAT,    32,       33 },

   {  L_,    0,     64,       2,          NAT,     0,        2 },// ALLOCSIZE=2
   {  L_,    1,     64,       2,          NAT,     2,        4 },
   {  L_,    2,     64,       2,          NAT,     2,        4 },
   {  L_,    3,     64,       2,          NAT,     4,        6 },
   {  L_,    7,     64,       2,          NAT,     8,       10 },
   {  L_,    8,     64,       2,          NAT,     8,       10 },
   {  L_,   15,     64,       2,          NAT,    16,       18 },
   {  L_,   16,     64,       2,          NAT,    16,       18 },
   {  L_,   31,     64,       2,          NAT,    32,       34 },
   {  L_,   32,     64,       2,          NAT,    32,       34 },

   {  L_,    0,     64,       4,          NAT,     0,        4 },// ALLOCSIZE=4
   {  L_,    1,     64,       4,          NAT,     4,        8 },
   {  L_,    2,     64,       4,          NAT,     4,        8 },
   {  L_,    3,     64,       4,          NAT,     4,        8 },
   {  L_,    7,     64,       4,          NAT,     8,       12 },
   {  L_,    8,     64,       4,          NAT,     8,       12 },
   {  L_,   15,     64,       4,          NAT,    16,       20 },
   {  L_,   16,     64,       4,          NAT,    16,       20 },
   {  L_,   31,     64,       4,          NAT,    32,       36 },
   {  L_,   32,     64,       4,          NAT,    32,       36 },

   {  L_,    0,     64,       8,          NAT,     0,        8 },// ALLOCSIZE=8
   {  L_,    1,     64,       8,          NAT,     8,       16 },
   {  L_,    2,     64,       8,          NAT,     8,       16 },
   {  L_,    3,     64,       8,          NAT,     8,       16 },
   {  L_,    7,     64,       8,          NAT,     8,       16 },
   {  L_,    8,     64,       8,          NAT,     8,       16 },
   {  L_,   15,     64,       8,          NAT,    16,       24 },
   {  L_,   16,     64,       8,          NAT,    16,       24 },
   {  L_,   31,     64,       8,          NAT,    32,       40 },
   {  L_,   32,     64,       8,          NAT,    32,       40 },

   {  L_,    0,     64,      16,          NAT,     0,       16 },//ALLOCSIZE=16
   {  L_,    1,     64,      16,          NAT,  k_MA,k_MA + 16 },
   {  L_,    2,     64,      16,          NAT,  k_MA,k_MA + 16 },
   {  L_,    3,     64,      16,          NAT,  k_MA,k_MA + 16 },
   {  L_,    7,     64,      16,          NAT,  k_MA,k_MA + 16 },
   {  L_,    8,     64,      16,          NAT,  k_MA,k_MA + 16 },
   {  L_,   15,     64,      16,          NAT,    16,       32 },
   {  L_,   16,     64,      16,          NAT,    16,       32 },
   {  L_,   31,     64,      16,          NAT,    32,       48 },
   {  L_,   32,     64,      16,          NAT,    32,       48 },

   // MAXIMUM ALIGNMENT
   {  L_,   0,      64,       1,          MAX,     0,        1 },// ALLOCSIZE=1
   {  L_,   1,      64,       1,          MAX,  k_MA,k_MA +  1 },
   {  L_,   2,      64,       1,          MAX,  k_MA,k_MA +  1 },
   {  L_,   3,      64,       1,          MAX,  k_MA,k_MA +  1 },
   {  L_,   7,      64,       1,          MAX,  k_MA,k_MA +  1 },
   {  L_,   8,      64,       1,          MAX,  k_MA,k_MA +  1 },
   {  L_,  15,      64,       1,          MAX,    16,       17 },
   {  L_,  16,      64,       1,          MAX,    16,       17 },
   {  L_,  31,      64,       1,          MAX,    32,       33 },
   {  L_,  32,      64,       1,          MAX,    32,       33 },

   {  L_,   0,      64,       2,          MAX,     0,        2 },// ALLOCSIZE=2
   {  L_,   1,      64,       2,          MAX,  k_MA,k_MA +  2 },
   {  L_,   2,      64,       2,          MAX,  k_MA,k_MA +  2 },
   {  L_,   3,      64,       2,          MAX,  k_MA,k_MA +  2 },
   {  L_,   7,      64,       2,          MAX,  k_MA,k_MA +  2 },
   {  L_,   8,      64,       2,          MAX,  k_MA,k_MA +  2 },
   {  L_,  15,      64,       2,          MAX,    16,       18 },
   {  L_,  16,      64,       2,          MAX,    16,       18 },
   {  L_,  31,      64,       2,          MAX,    32,       34 },
   {  L_,  32,      64,       2,          MAX,    32,       34 },

   {  L_,   0,      64,       4,          MAX,     0,        4 },// ALLOCSIZE=4
   {  L_,   1,      64,       4,          MAX,  k_MA,k_MA +  4 },
   {  L_,   2,      64,       4,          MAX,  k_MA,k_MA +  4 },
   {  L_,   3,      64,       4,          MAX,  k_MA,k_MA +  4 },
   {  L_,   7,      64,       4,          MAX,  k_MA,k_MA +  4 },
   {  L_,   8,      64,       4,          MAX,  k_MA,k_MA +  4 },
   {  L_,  15,      64,       4,          MAX,    16,       20 },
   {  L_,  16,      64,       4,          MAX,    16,       20 },
   {  L_,  31,      64,       4,          MAX,    32,       36 },
   {  L_,  32,      64,       4,          MAX,    32,       36 },

   {  L_,   0,      64,       8,          MAX,     0,        8 },// ALLOCSIZE=8
   {  L_,   1,      64,       8,          MAX,  k_MA,k_MA +  8 },
   {  L_,   2,      64,       8,          MAX,  k_MA,k_MA +  8 },
   {  L_,   3,      64,       8,          MAX,  k_MA,k_MA +  8 },
   {  L_,   7,      64,       8,          MAX,  k_MA,k_MA +  8 },
   {  L_,   8,      64,       8,          MAX,  k_MA,k_MA +  8 },
   {  L_,  15,      64,       8,          MAX,    16,       24 },
   {  L_,  16,      64,       8,          MAX,    16,       24 },
   {  L_,  31,      64,       8,          MAX,    32,       40 },
   {  L_,  32,      64,       8,          MAX,    32,       40 },

   {  L_,    0,     64,      16,          MAX,     0,       16 },//ALLOCSIZE=16
   {  L_,    1,     64,      16,          MAX,  k_MA,k_MA + 16 },
   {  L_,    2,     64,      16,          MAX,  k_MA,k_MA + 16 },
   {  L_,    3,     64,      16,          MAX,  k_MA,k_MA + 16 },
   {  L_,    7,     64,      16,          MAX,  k_MA,k_MA + 16 },
   {  L_,    8,     64,      16,          MAX,  k_MA,k_MA + 16 },
   {  L_,   15,     64,      16,          MAX,    16,       32 },
   {  L_,   16,     64,      16,          MAX,    16,       32 },
   {  L_,   31,     64,      16,          MAX,    32,       48 },
   {  L_,   32,     64,      16,          MAX,    32,       48 },

   // 1-BYTE ALIGNMENT
   {  L_,   0,      64,       1,          BYT,     0,        1 },// ALLOCSIZE=1
   {  L_,   1,      64,       1,          BYT,     1,        2 },
   {  L_,   2,      64,       1,          BYT,     2,        3 },
   {  L_,   3,      64,       1,          BYT,     3,        4 },
   {  L_,   7,      64,       1,          BYT,     7,        8 },
   {  L_,   8,      64,       1,          BYT,     8,        9 },
   {  L_,  15,      64,       1,          BYT,    15,       16 },
   {  L_,  16,      64,       1,          BYT,    16,       17 },
   {  L_,  31,      64,       1,          BYT,    31,       32 },
   {  L_,  32,      64,       1,          BYT,    32,       33 },

   {  L_,   0,      64,       2,          BYT,     0,        2 },// ALLOCSIZE=2
   {  L_,   1,      64,       2,          BYT,     1,        3 },
   {  L_,   2,      64,       2,          BYT,     2,        4 },
   {  L_,   3,      64,       2,          BYT,     3,        5 },
   {  L_,   7,      64,       2,          BYT,     7,        9 },
   {  L_,   8,      64,       2,          BYT,     8,       10 },
   {  L_,  15,      64,       2,          BYT,    15,       17 },
   {  L_,  16,      64,       2,          BYT,    16,       18 },
   {  L_,  31,      64,       2,          BYT,    31,       33 },
   {  L_,  32,      64,       2,          BYT,    32,       34 },

   {  L_,   0,      64,       4,          BYT,     0,        4 },// ALLOCSIZE=4
   {  L_,   1,      64,       4,          BYT,     1,        5 },
   {  L_,   2,      64,       4,          BYT,     2,        6 },
   {  L_,   3,      64,       4,          BYT,     3,        7 },
   {  L_,   7,      64,       4,          BYT,     7,       11 },
   {  L_,   8,      64,       4,          BYT,     8,       12 },
   {  L_,  15,      64,       4,          BYT,    15,       19 },
   {  L_,  16,      64,       4,          BYT,    16,       20 },
   {  L_,  31,      64,       4,          BYT,    31,       35 },
   {  L_,  32,      64,       4,          BYT,    32,       36 },

   {  L_,   0,      64,       8,          BYT,     0,        8 },// ALLOCSIZE=8
   {  L_,   1,      64,       8,          BYT,     1,        9 },
   {  L_,   2,      64,       8,          BYT,     2,       10 },
   {  L_,   3,      64,       8,          BYT,     3,       11 },
   {  L_,   7,      64,       8,          BYT,     7,       15 },
   {  L_,   8,      64,       8,          BYT,     8,       16 },
   {  L_,  15,      64,       8,          BYT,    15,       23 },
   {  L_,  16,      64,       8,          BYT,    16,       24 },
   {  L_,  31,      64,       8,          BYT,    31,       39 },
   {  L_,  32,      64,       8,          BYT,    32,       40 },

   {  L_,    0,     64,      16,          BYT,     0,       16 },//ALLOCSIZE=16
   {  L_,    1,     64,      16,          BYT,     1,       17 },
   {  L_,    2,     64,      16,          BYT,     2,       18 },
   {  L_,    3,     64,      16,          BYT,     3,       19 },
   {  L_,    7,     64,      16,          BYT,     7,       23 },
   {  L_,    8,     64,      16,          BYT,     8,       24 },
   {  L_,   15,     64,      16,          BYT,    15,       31 },
   {  L_,   16,     64,      16,          BYT,    16,       32 },
   {  L_,   31,     64,      16,          BYT,    31,       47 },
   {  L_,   32,     64,      16,          BYT,    32,       48 },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        char *buffer = bufferStorage.buffer();

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE      = DATA[ti].d_line;
            const int   CURSOR    = DATA[ti].d_cursor;
            const int   BUFSIZE   = DATA[ti].d_bufSize;
            const int   ALLOCSIZE = DATA[ti].d_allocSize;
            const Strat STRAT     = DATA[ti].d_strategy;
            const int   EXPOFFSET = DATA[ti].d_expOffset;
            const int   EXPCURSOR = DATA[ti].d_expCursor;

            if (veryVerbose) {
                T_ P_(LINE) P_(CURSOR) P_(BUFSIZE) P(ALLOCSIZE)
                T_ T_
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL ALIGNMENT, ";
                }
                else if (STRAT == MAX) {
                    cout << "STRAT = MAXIMUM ALIGNMENT, ";
                }
                else {  // STRAT == BYT
                    cout << "STRAT = 1-BYTE ALIGNMENT, ";
                }
                P_(EXPOFFSET) P(EXPCURSOR)
            }

            if (veryVerbose) cout << "\tTesting 'allocateFromBuffer'" << endl;
            {
                int tmpCursor = CURSOR;

                void *address = Obj::allocateFromBuffer(&tmpCursor,
                                                        buffer,
                                                        BUFSIZE,
                                                        ALLOCSIZE,
                                                        STRAT);

                LOOP2_ASSERT(EXPCURSOR, tmpCursor, EXPCURSOR == tmpCursor);
                LOOP3_ASSERT(EXPOFFSET, &buffer[EXPOFFSET], address,
                                                &buffer[EXPOFFSET] == address);
            }

            {
                int tmpCursor = CURSOR;
                void *address;

                if (STRAT == NAT) {
                    address = Obj::allocateNaturallyAlignedFromBuffer(
                                                                    &tmpCursor,
                                                                    buffer,
                                                                    BUFSIZE,
                                                                    ALLOCSIZE);
                }
                else if (STRAT == MAX) {
                    address = Obj::allocateMaximallyAlignedFromBuffer(
                                                                    &tmpCursor,
                                                                    buffer,
                                                                    BUFSIZE,
                                                                    ALLOCSIZE);
                }
                else {  // STRAT == BYT
                    address = Obj::allocateOneByteAlignedFromBuffer(&tmpCursor,
                                                                    buffer,
                                                                    BUFSIZE,
                                                                    ALLOCSIZE);
                }

                LOOP2_ASSERT(EXPCURSOR, tmpCursor, EXPCURSOR == tmpCursor);
                LOOP3_ASSERT(EXPOFFSET, &buffer[EXPOFFSET], address,
                                                &buffer[EXPOFFSET] == address);
            }

            if (veryVerbose) cout << "\tTesting 'allocateFromBufferRaw'"
                                  << endl;
            {
                int tmpCursor = CURSOR;

                void *address = Obj::allocateFromBufferRaw(&tmpCursor,
                                                           buffer,
                                                           ALLOCSIZE,
                                                           STRAT);

                LOOP2_ASSERT(EXPCURSOR, tmpCursor, EXPCURSOR == tmpCursor);
                LOOP3_ASSERT(EXPOFFSET, &buffer[EXPOFFSET], address,
                                                &buffer[EXPOFFSET] == address);
            }

            {
                int tmpCursor = CURSOR;
                void *address;

                if (STRAT == NAT) {
                    address = Obj::allocateNaturallyAlignedFromBufferRaw(
                                                                    &tmpCursor,
                                                                    buffer,
                                                                    ALLOCSIZE);
                }
                else if (STRAT == MAX) {
                    address = Obj::allocateMaximallyAlignedFromBufferRaw(
                                                                    &tmpCursor,
                                                                    buffer,
                                                                    ALLOCSIZE);
                }
                else {  // STRAT == BYT
                    address = Obj::allocateOneByteAlignedFromBufferRaw(
                                                                    &tmpCursor,
                                                                    buffer,
                                                                    ALLOCSIZE);
                }

                LOOP2_ASSERT(EXPCURSOR, tmpCursor, EXPCURSOR == tmpCursor);
                LOOP3_ASSERT(EXPOFFSET, &buffer[EXPOFFSET], address,
                                                &buffer[EXPOFFSET] == address);
            }
        }

        }

        if (verbose) cout << "\nTesting buffer overflow." << endl;
        {

        static const struct {
            int   d_line;       // line number
            int   d_cursor;     // initial cursor position
            int   d_bufSize;    // buffer size
            int   d_allocSize;  // allocation request size
            Strat d_strategy;   // alignment strategy
            int   d_expCursor;  // expected cursor position after request
        } DATA[] = {

       // LINE  CURSOR  BUFSIZE   ALLOCSIZE   STRAT   EXPCURSOR
       // ----  ------  -------   ---------   -----   ---------

       // NATURAL ALIGNMENT
       {  L_,   64,     64,       1,          NAT,    64 }, // ALLOCSIZE=1

       {  L_,   64,     64,       2,          NAT,    64 }, // ALLOCSIZE=2
       {  L_,   63,     64,       2,          NAT,    63 },

       {  L_,   64,     64,       4,          NAT,    64 }, // ALLOCSIZE=4
       {  L_,   63,     64,       4,          NAT,    63 },
       {  L_,   62,     64,       4,          NAT,    62 },
       {  L_,   61,     64,       4,          NAT,    61 },

       {  L_,   64,     64,       8,          NAT,    64 }, // ALLOCSIZE=8
       {  L_,   63,     64,       8,          NAT,    63 },
       {  L_,   62,     64,       8,          NAT,    62 },
       {  L_,   61,     64,       8,          NAT,    61 },
       {  L_,   60,     64,       8,          NAT,    60 },
       {  L_,   59,     64,       8,          NAT,    59 },
       {  L_,   58,     64,       8,          NAT,    58 },
       {  L_,   57,     64,       8,          NAT,    57 },

       {  L_,   64,     64,      16,          NAT,    64 }, // ALLOCSIZE=16
       {  L_,   63,     64,      16,          NAT,    63 },
       {  L_,   62,     64,      16,          NAT,    62 },
       {  L_,   61,     64,      16,          NAT,    61 },
       {  L_,   60,     64,      16,          NAT,    60 },
       {  L_,   59,     64,      16,          NAT,    59 },
       {  L_,   58,     64,      16,          NAT,    58 },
       {  L_,   57,     64,      16,          NAT,    57 },
       {  L_,   56,     64,      16,          NAT,    56 },
       {  L_,   55,     64,      16,          NAT,    55 },
       {  L_,   54,     64,      16,          NAT,    54 },
       {  L_,   53,     64,      16,          NAT,    53 },
       {  L_,   52,     64,      16,          NAT,    52 },
       {  L_,   51,     64,      16,          NAT,    51 },
       {  L_,   50,     64,      16,          NAT,    50 },
       {  L_,   49,     64,      16,          NAT,    49 },

       // MAXIMUM ALIGNMENT
       {  L_,   64,     64,       1,          MAX,    64 }, // ALLOCSIZE=1
       {  L_,   63,     64,       1,          MAX,    63 },

       {  L_,   64,     64,       2,          MAX,    64 }, // ALLOCSIZE=2
       {  L_,   63,     64,       2,          MAX,    63 },
       {  L_,   62,     64,       2,          MAX,    62 },

       {  L_,   64,     64,       4,          MAX,    64 }, // ALLOCSIZE=4
       {  L_,   63,     64,       4,          MAX,    63 },
       {  L_,   62,     64,       4,          MAX,    62 },
       {  L_,   61,     64,       4,          MAX,    61 },
#if k_MA > 4
       {  L_,   60,     64,       4,          MAX,    60 },
#endif

       {  L_,   64,     64,       8,          MAX,    64 }, // ALLOCSIZE=8
       {  L_,   63,     64,       8,          MAX,    63 },
       {  L_,   62,     64,       8,          MAX,    62 },
       {  L_,   61,     64,       8,          MAX,    61 },
       {  L_,   60,     64,       8,          MAX,    60 },
       {  L_,   59,     64,       8,          MAX,    59 },
       {  L_,   58,     64,       8,          MAX,    58 },
       {  L_,   57,     64,       8,          MAX,    57 },
#if k_MA > 8
       {  L_,   56,     64,       8,          MAX,    56 },
#endif

       {  L_,   64,     64,      16,          MAX,    64 }, // ALLOCSIZE=16
       {  L_,   63,     64,      16,          MAX,    63 },
       {  L_,   62,     64,      16,          MAX,    62 },
       {  L_,   61,     64,      16,          MAX,    61 },
       {  L_,   60,     64,      16,          MAX,    60 },
       {  L_,   59,     64,      16,          MAX,    59 },
       {  L_,   58,     64,      16,          MAX,    58 },
       {  L_,   57,     64,      16,          MAX,    57 },
       {  L_,   56,     64,      16,          MAX,    56 },
       {  L_,   55,     64,      16,          MAX,    55 },
       {  L_,   54,     64,      16,          MAX,    54 },
       {  L_,   53,     64,      16,          MAX,    53 },
       {  L_,   52,     64,      16,          MAX,    52 },
       {  L_,   51,     64,      16,          MAX,    51 },
       {  L_,   50,     64,      16,          MAX,    50 },
       {  L_,   49,     64,      16,          MAX,    49 },

       // 1-BYTE ALIGNMENT
       {  L_,   64,     64,       1,          BYT,    64 }, // ALLOCSIZE=1

       {  L_,   64,     64,       2,          BYT,    64 }, // ALLOCSIZE=2
       {  L_,   63,     64,       2,          BYT,    63 },

       {  L_,   64,     64,       4,          BYT,    64 }, // ALLOCSIZE=4
       {  L_,   63,     64,       4,          BYT,    63 },
       {  L_,   62,     64,       4,          BYT,    62 },
       {  L_,   61,     64,       4,          BYT,    61 },

       {  L_,   64,     64,       8,          BYT,    64 }, // ALLOCSIZE=8
       {  L_,   63,     64,       8,          BYT,    63 },
       {  L_,   62,     64,       8,          BYT,    62 },
       {  L_,   61,     64,       8,          BYT,    61 },
       {  L_,   60,     64,       8,          BYT,    60 },
       {  L_,   59,     64,       8,          BYT,    59 },
       {  L_,   58,     64,       8,          BYT,    58 },
       {  L_,   57,     64,       8,          BYT,    57 },

       {  L_,   64,     64,      16,          BYT,    64 }, // ALLOCSIZE=16
       {  L_,   63,     64,      16,          BYT,    63 },
       {  L_,   62,     64,      16,          BYT,    62 },
       {  L_,   61,     64,      16,          BYT,    61 },
       {  L_,   60,     64,      16,          BYT,    60 },
       {  L_,   59,     64,      16,          BYT,    59 },
       {  L_,   58,     64,      16,          BYT,    58 },
       {  L_,   57,     64,      16,          BYT,    57 },
       {  L_,   56,     64,      16,          BYT,    56 },
       {  L_,   55,     64,      16,          BYT,    55 },
       {  L_,   54,     64,      16,          BYT,    54 },
       {  L_,   53,     64,      16,          BYT,    53 },
       {  L_,   52,     64,      16,          BYT,    52 },
       {  L_,   51,     64,      16,          BYT,    51 },
       {  L_,   50,     64,      16,          BYT,    50 },
       {  L_,   49,     64,      16,          BYT,    49 },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        char *buffer = bufferStorage.buffer();

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE      = DATA[ti].d_line;
            const int   CURSOR    = DATA[ti].d_cursor;
            const int   BUFSIZE   = DATA[ti].d_bufSize;
            const int   ALLOCSIZE = DATA[ti].d_allocSize;
            const Strat STRAT     = DATA[ti].d_strategy;
            const int   EXPCURSOR = DATA[ti].d_expCursor;

            if (veryVerbose) {
                T_ P_(LINE) P_(CURSOR) P_(BUFSIZE) P(ALLOCSIZE)
                T_ T_
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL ALIGNMENT, ";
                }
                else if (STRAT == MAX) {
                    cout << "STRAT = MAXIMUM ALIGNMENT, ";
                }
                else {  // STRAT == BYT
                    cout << "STRAT = 1-BYTE ALIGNMENT, ";
                }
                P(EXPCURSOR)
            }

            {
                int tmpCursor = CURSOR;

                void *address = Obj::allocateFromBuffer(&tmpCursor,
                                                        buffer,
                                                        BUFSIZE,
                                                        ALLOCSIZE,
                                                        STRAT);

                LOOP2_ASSERT(EXPCURSOR, tmpCursor, EXPCURSOR == tmpCursor);
                LOOP_ASSERT(address, 0 == address);
            }

            {
                int tmpCursor = CURSOR;
                void *address;

                if (STRAT == NAT) {
                    address = Obj::allocateNaturallyAlignedFromBuffer(
                                                                    &tmpCursor,
                                                                    buffer,
                                                                    BUFSIZE,
                                                                    ALLOCSIZE);
                }
                else if (STRAT == MAX) {
                    address = Obj::allocateMaximallyAlignedFromBuffer(
                                                                    &tmpCursor,
                                                                    buffer,
                                                                    BUFSIZE,
                                                                    ALLOCSIZE);
                }
                else {  // STRAT == BYT
                    address = Obj::allocateOneByteAlignedFromBuffer(&tmpCursor,
                                                                    buffer,
                                                                    BUFSIZE,
                                                                    ALLOCSIZE);
                }

                LOOP2_ASSERT(EXPCURSOR, tmpCursor, EXPCURSOR == tmpCursor);
                LOOP_ASSERT(address, 0 == address);
            }
        }

        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            char *buffer = bufferStorage.buffer();
            enum { k_BUFSIZE = 64, k_ALLOCSIZE = 4 };

            if (veryVerbose) cout << "\t'0 <= bufferSize'" << endl;
            {
                int cursor = 0;

                ASSERT_SAFE_PASS(Obj::allocateFromBuffer(&cursor,
                                                         buffer,
                                                         0,        // PASS
                                                         k_ALLOCSIZE,
                                                         NAT));
                ASSERT_SAFE_FAIL(Obj::allocateFromBuffer(&cursor,
                                                         buffer,
                                                         -1,       // FAIL
                                                         k_ALLOCSIZE,
                                                         NAT));

                ASSERT_SAFE_PASS(Obj::allocateMaximallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         0,        // PASS
                                                         k_ALLOCSIZE));
                ASSERT_SAFE_FAIL(Obj::allocateMaximallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         -1,       // FAIL
                                                         k_ALLOCSIZE));

                ASSERT_SAFE_PASS(Obj::allocateNaturallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         0,        // PASS
                                                         k_ALLOCSIZE));
                ASSERT_SAFE_FAIL(Obj::allocateNaturallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         -1,       // FAIL
                                                         k_ALLOCSIZE));

                ASSERT_SAFE_PASS(Obj::allocateOneByteAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         0,        // PASS
                                                         k_ALLOCSIZE));
                ASSERT_SAFE_FAIL(Obj::allocateOneByteAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         -1,       // FAIL
                                                         k_ALLOCSIZE));
            }

            if (veryVerbose) cout << "\t'0 < size'" << endl;
            {
                int cursor = 0;

                ASSERT_SAFE_PASS(Obj::allocateFromBuffer(&cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         1,        // PASS
                                                         NAT));
                ASSERT_SAFE_FAIL(Obj::allocateFromBuffer(&cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         0,        // FAIL
                                                         NAT));
                ASSERT_SAFE_FAIL(Obj::allocateFromBuffer(&cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         -1,       // FAIL
                                                         NAT));

                ASSERT_SAFE_PASS(Obj::allocateMaximallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         1));      // PASS
                ASSERT_SAFE_FAIL(Obj::allocateMaximallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         0));      // FAIL
                ASSERT_SAFE_FAIL(Obj::allocateMaximallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         -1));     // FAIL

                ASSERT_SAFE_PASS(Obj::allocateNaturallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         1));      // PASS
                ASSERT_SAFE_FAIL(Obj::allocateNaturallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         0));      // FAIL
                ASSERT_SAFE_FAIL(Obj::allocateNaturallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         -1));     // FAIL

                ASSERT_SAFE_PASS(Obj::allocateOneByteAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         1));      // PASS
                ASSERT_SAFE_FAIL(Obj::allocateOneByteAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         0));      // FAIL
                ASSERT_SAFE_FAIL(Obj::allocateOneByteAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         -1));     // FAIL

                ASSERT_SAFE_PASS(Obj::allocateFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         1,        // PASS
                                                         NAT));
                ASSERT_SAFE_FAIL(Obj::allocateFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         0,        // FAIL
                                                         NAT));
                ASSERT_SAFE_FAIL(Obj::allocateFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         -1,       // FAIL
                                                         NAT));

                ASSERT_SAFE_PASS(Obj::allocateMaximallyAlignedFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         1));      // PASS
                ASSERT_SAFE_FAIL(Obj::allocateMaximallyAlignedFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         0));      // FAIL
                ASSERT_SAFE_FAIL(Obj::allocateMaximallyAlignedFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         -1));     // FAIL

                ASSERT_SAFE_PASS(Obj::allocateNaturallyAlignedFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         1));      // PASS
                ASSERT_SAFE_FAIL(Obj::allocateNaturallyAlignedFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         0));      // FAIL
                ASSERT_SAFE_FAIL(Obj::allocateNaturallyAlignedFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         -1));     // FAIL

                ASSERT_SAFE_PASS(Obj::allocateOneByteAlignedFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         1));      // PASS
                ASSERT_SAFE_FAIL(Obj::allocateOneByteAlignedFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         0));      // FAIL
                ASSERT_SAFE_FAIL(Obj::allocateOneByteAlignedFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         -1));     // FAIL
            }

            if (veryVerbose) cout << "\t'0 <= *cursor'" << endl;
            {
                int cursor;

                cursor = 0;
                ASSERT_SAFE_PASS(Obj::allocateFromBuffer(&cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         k_ALLOCSIZE,
                                                         NAT));
                cursor = -1;
                ASSERT_SAFE_FAIL(Obj::allocateFromBuffer(&cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         k_ALLOCSIZE,
                                                         NAT));

                cursor = 0;
                ASSERT_SAFE_PASS(Obj::allocateMaximallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         k_ALLOCSIZE));
                cursor = -1;
                ASSERT_SAFE_FAIL(Obj::allocateMaximallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         k_ALLOCSIZE));

                cursor = 0;
                ASSERT_SAFE_PASS(Obj::allocateNaturallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         k_ALLOCSIZE));
                cursor = -1;
                ASSERT_SAFE_FAIL(Obj::allocateNaturallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         k_ALLOCSIZE));

                cursor = 0;
                ASSERT_SAFE_PASS(Obj::allocateOneByteAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         k_ALLOCSIZE));
                cursor = -1;
                ASSERT_SAFE_FAIL(Obj::allocateOneByteAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         k_ALLOCSIZE));

                cursor = 0;
                ASSERT_SAFE_PASS(Obj::allocateFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         k_ALLOCSIZE,
                                                         NAT));
                cursor = -1;
                ASSERT_SAFE_FAIL(Obj::allocateFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         k_ALLOCSIZE,
                                                         NAT));

                cursor = 0;
                ASSERT_SAFE_PASS(Obj::allocateMaximallyAlignedFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         k_ALLOCSIZE));
                cursor = -1;
                ASSERT_SAFE_FAIL(Obj::allocateMaximallyAlignedFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         k_ALLOCSIZE));

                cursor = 0;
                ASSERT_SAFE_PASS(Obj::allocateNaturallyAlignedFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         k_ALLOCSIZE));
                cursor = -1;
                ASSERT_SAFE_FAIL(Obj::allocateNaturallyAlignedFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         k_ALLOCSIZE));

                cursor = 0;
                ASSERT_SAFE_PASS(Obj::allocateOneByteAlignedFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         k_ALLOCSIZE));
                cursor = -1;
                ASSERT_SAFE_FAIL(Obj::allocateOneByteAlignedFromBufferRaw(
                                                         &cursor,
                                                         buffer,
                                                         k_ALLOCSIZE));
            }

            if (veryVerbose) cout << "\t'*cursor <= bufferSize'" << endl;
            {
                int cursor;

                cursor = k_BUFSIZE;
                ASSERT_SAFE_PASS(Obj::allocateFromBuffer(&cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         k_ALLOCSIZE,
                                                         NAT));
                cursor = k_BUFSIZE + 1;
                ASSERT_SAFE_FAIL(Obj::allocateFromBuffer(&cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         k_ALLOCSIZE,
                                                         NAT));

                cursor = k_BUFSIZE;
                ASSERT_SAFE_PASS(Obj::allocateMaximallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         k_ALLOCSIZE));
                cursor = k_BUFSIZE + 1;
                ASSERT_SAFE_FAIL(Obj::allocateMaximallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         k_ALLOCSIZE));

                cursor = k_BUFSIZE;
                ASSERT_SAFE_PASS(Obj::allocateNaturallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         k_ALLOCSIZE));
                cursor = k_BUFSIZE + 1;
                ASSERT_SAFE_FAIL(Obj::allocateNaturallyAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         k_ALLOCSIZE));

                cursor = k_BUFSIZE;
                ASSERT_SAFE_PASS(Obj::allocateOneByteAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         k_ALLOCSIZE));
                cursor = k_BUFSIZE + 1;
                ASSERT_SAFE_FAIL(Obj::allocateOneByteAlignedFromBuffer(
                                                         &cursor,
                                                         buffer,
                                                         k_BUFSIZE,
                                                         k_ALLOCSIZE));
            }
        }

#undef NAT
#undef MAX
#undef BYT

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
