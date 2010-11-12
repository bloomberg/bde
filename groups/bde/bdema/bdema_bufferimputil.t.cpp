// bdema_bufferimputil.t.cpp                                          -*-C++-*-

#include <bdema_bufferimputil.h>

#include <bslma_allocator.h>

#include <bsls_alignedbuffer.h>
#include <bsls_alignment.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// 'bdema_BufferImpUtil' provides two static methods for allocating a
// contiguous block of memory from an external buffer.  The address of the
// allocated memory block depends on several parameters: the current cursor
// position, the size of the allocation, the alignment strategy, and also the
// buffer size (necessary to detect overflow with 'allocateFromBuffer').  Due
// to the large amount of possible combinations, only the corner cases are
// selected as test data.  These data are then organized first by allocation
// size then current cursor position (both of which affects the address of the
// allocated memory block depending on the alignment strategy used) in a table
// format.
//
// Negative testing for undefined behavior is also part of the test driver.
// To test for undefined behavior, an assert handler is installed prior to
// executing the test.  The assert handler sets a global variable to indicate
// whether it had been ran.  If the test case triggers undefined behavior, the
// assert handler should be invoked and we can 'assert' that the global
// variable is modified accordingly.
//-----------------------------------------------------------------------------
// [1] void *allocateFromBuffer(int*, char*, int, int, AlignmentStrategy)
// [1] void *allocateFromBufferRaw(int *, char *, int, AlignmentStrategy)
//-----------------------------------------------------------------------------
// [2] USAGE EXAMPLE
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
typedef bdema_BufferImpUtil Obj;

// On windows, when a 8-byte aligned object is created on the stack, it
// actually get aligned on 4-byte boundary.  To fix this, create a static
// buffer instead.
enum { BUFFER_SIZE = 256 };
static bsls_AlignedBuffer<BUFFER_SIZE> bufferStorage;

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// This component is typically used by a class that manages a memory buffer.
// First, suppose we have a class that maintains a linked list of memory
// blocks:
//..
    class BlockList {
        // ...
    };
//..
// We can then create our memory manager:
//..
    class my_SequentialPool {
        // This class allocates memory from an internal pool of memory buffers
        // using natural alignment.  All allocated memory is managed internally
        // by the pool and released when the pool is destroyed.

        // DATA
        char      *d_buffer_p;    // pointer to current buffer
        int        d_cursor;      // byte offset to unused memory in buffer
        int        d_bufferSize;  // size of the current buffer
        BlockList  d_blockList;   // used to replenishing memory

      private:
        // PRIVATE MANIPULATORS
        void replenishBuffer(int size);
            // Replenish the current buffer with new memory that satisfies
            // a memory request having at least the specified 'size' (in
            // bytes).

      public:
        // CREATORS
        my_SequentialPool(bslma_Allocator *basicAllocator = 0);
            // Create a memory pool that dispenses user-specified-sized blocks
            // of memory.  Optionally specify 'basicAllocator' to supply
            // memory.  If 'basicAllocator' is 0, the currently installed
            // default allocator is used.

        ~my_SequentialPool();
            // Destroy this pool and release all associated memory.

        void *allocate(int size);
            // Return the address of a contiguous block of memory of the
            // specified 'size' (in bytes).
    };
//..
// The implementations of the constructor and destructor are elided since
// 'allocate' alone is sufficient to illustrate the use of
// 'bdema_BufferImpUtil':
//..
    void *my_SequentialPool::allocate(int size)
    {
        void *address = bdema_BufferImpUtil::allocateFromBuffer(
                                                 &d_cursor,
                                                 d_buffer_p,
                                                 d_bufferSize,
                                                 size,
                                                 bsls_Alignment::BSLS_NATURAL);
//..
// Note that if there is insufficient space in 'd_buffer_p',
// 'allocateFromBuffer' returns 0.
//..
        if (address) {
            return address;                                           // RETURN
        }

        replenishBuffer(size);

        return bdema_BufferImpUtil::allocateFromBufferRaw(
                                                 &d_cursor,
                                                 d_buffer_p,
                                                 size,
                                                 bsls_Alignment::BSLS_NATURAL);
    }
//..
// Note that the *raw* version is used because the contract of
// 'replenishBuffer' guarantees that the buffer will have sufficient space to
// satisfy the memory request of the specified 'size'.

//=============================================================================
//           Additional Functionality Needed to Complete Usage Test Case
//-----------------------------------------------------------------------------

void my_SequentialPool::replenishBuffer(int size)
{
    // ...
}

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

    switch (test) { case 0:
      case 2: {
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

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // CLASS METHODS TEST
        //
        // Concerns:
        //   1) That the allocated memory address is correctly aligned
        //      according to the specified alignment strategy with at least the
        //      specified size and allocated from the specified buffer at the
        //      specified cursor position.
        //
        //   2) That the cursor is updated with the first byte position
        //      immediately after the allocated memory.
        //
        //   3) That when 'allocateFromBuffer' is used and the allocation
        //      request causes the buffer to overflow, 0 is returned, and
        //      the cursor is not changed.
        //
        // Plan:
        //   Using the table-driven technique, create a set of test vectors
        //   having different allocation size, alignment strategy, cursor
        //   position, expected cursor position and expected memory address.
        //   Verify the result of the class method with the expected values.
        //
        // Testing:
        //   void *allocateFromBuffer(int*, char*, int, int, AlignmentStrategy)
        //   void *allocateFromBufferRaw(int *, char *, int, AlignmentStrategy)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CLASS METHODS TEST" << endl
                                  << "==================" << endl;

        typedef bsls_Alignment::Strategy St;

        enum { MA = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT };

#define NAT bsls_Alignment::BSLS_NATURAL
#define MAX bsls_Alignment::BSLS_MAXIMUM

        if (verbose) cout << "\nTesting expected cursor and offset" << endl;
        {

        static const struct {
            int d_line;       // line number
            int d_cursor;     // initial cursor position
            int d_bufSize;    // buffer size
            int d_allocSize;  // allocation request size
            St  d_strategy;   // alignment strategy
            int d_expOffset;  // expected memory offset
            int d_expCursor;  // expected cursor position after request
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
   {  L_,    1,     64,      16,          NAT,    MA,  MA + 16 },
   {  L_,    2,     64,      16,          NAT,    MA,  MA + 16 },
   {  L_,    3,     64,      16,          NAT,    MA,  MA + 16 },
   {  L_,    7,     64,      16,          NAT,    MA,  MA + 16 },
   {  L_,    8,     64,      16,          NAT,    MA,  MA + 16 },
   {  L_,   15,     64,      16,          NAT,    16,       32 },
   {  L_,   16,     64,      16,          NAT,    16,       32 },
   {  L_,   31,     64,      16,          NAT,    32,       48 },
   {  L_,   32,     64,      16,          NAT,    32,       48 },

   // MAXIMUM ALIGNMENT
   {  L_,   0,      64,       1,          MAX,     0,        1 },// ALLOCSIZE=1
   {  L_,   1,      64,       1,          MAX,    MA,  MA +  1 },
   {  L_,   2,      64,       1,          MAX,    MA,  MA +  1 },
   {  L_,   3,      64,       1,          MAX,    MA,  MA +  1 },
   {  L_,   7,      64,       1,          MAX,    MA,  MA +  1 },
   {  L_,   8,      64,       1,          MAX,    MA,  MA +  1 },
   {  L_,  15,      64,       1,          MAX,    16,       17 },
   {  L_,  16,      64,       1,          MAX,    16,       17 },
   {  L_,  31,      64,       1,          MAX,    32,       33 },
   {  L_,  32,      64,       1,          MAX,    32,       33 },

   {  L_,   0,      64,       2,          MAX,     0,        2 },// ALLOCSIZE=2
   {  L_,   1,      64,       2,          MAX,    MA,  MA +  2 },
   {  L_,   2,      64,       2,          MAX,    MA,  MA +  2 },
   {  L_,   3,      64,       2,          MAX,    MA,  MA +  2 },
   {  L_,   7,      64,       2,          MAX,    MA,  MA +  2 },
   {  L_,   8,      64,       2,          MAX,    MA,  MA +  2 },
   {  L_,  15,      64,       2,          MAX,    16,       18 },
   {  L_,  16,      64,       2,          MAX,    16,       18 },
   {  L_,  31,      64,       2,          MAX,    32,       34 },
   {  L_,  32,      64,       2,          MAX,    32,       34 },

   {  L_,   0,      64,       4,          MAX,     0,        4 },// ALLOCSIZE=4
   {  L_,   1,      64,       4,          MAX,    MA,  MA +  4 },
   {  L_,   2,      64,       4,          MAX,    MA,  MA +  4 },
   {  L_,   3,      64,       4,          MAX,    MA,  MA +  4 },
   {  L_,   7,      64,       4,          MAX,    MA,  MA +  4 },
   {  L_,   8,      64,       4,          MAX,    MA,  MA +  4 },
   {  L_,  15,      64,       4,          MAX,    16,       20 },
   {  L_,  16,      64,       4,          MAX,    16,       20 },
   {  L_,  31,      64,       4,          MAX,    32,       36 },
   {  L_,  32,      64,       4,          MAX,    32,       36 },

   {  L_,   0,      64,       8,          MAX,     0,        8 },// ALLOCSIZE=8
   {  L_,   1,      64,       8,          MAX,    MA,  MA +  8 },
   {  L_,   2,      64,       8,          MAX,    MA,  MA +  8 },
   {  L_,   3,      64,       8,          MAX,    MA,  MA +  8 },
   {  L_,   7,      64,       8,          MAX,    MA,  MA +  8 },
   {  L_,   8,      64,       8,          MAX,    MA,  MA +  8 },
   {  L_,  15,      64,       8,          MAX,    16,       24 },
   {  L_,  16,      64,       8,          MAX,    16,       24 },
   {  L_,  31,      64,       8,          MAX,    32,       40 },
   {  L_,  32,      64,       8,          MAX,    32,       40 },

   {  L_,    0,     64,      16,          NAT,     0,       16 },//ALLOCSIZE=16
   {  L_,    1,     64,      16,          NAT,    MA,  MA + 16 },
   {  L_,    2,     64,      16,          NAT,    MA,  MA + 16 },
   {  L_,    3,     64,      16,          NAT,    MA,  MA + 16 },
   {  L_,    7,     64,      16,          NAT,    MA,  MA + 16 },
   {  L_,    8,     64,      16,          NAT,    MA,  MA + 16 },
   {  L_,   15,     64,      16,          NAT,    16,       32 },
   {  L_,   16,     64,      16,          NAT,    16,       32 },
   {  L_,   31,     64,      16,          NAT,    32,       48 },
   {  L_,   32,     64,      16,          NAT,    32,       48 },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        char *buffer = bufferStorage.buffer();

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE      = DATA[ti].d_line;
            const int CURSOR    = DATA[ti].d_cursor;
            const int BUFSIZE   = DATA[ti].d_bufSize;
            const int ALLOCSIZE = DATA[ti].d_allocSize;
            const St  STRAT     = DATA[ti].d_strategy;
            const int EXPOFFSET = DATA[ti].d_expOffset;
            const int EXPCURSOR = DATA[ti].d_expCursor;

            if (veryVerbose) {
                T_ P_(LINE) P_(CURSOR) P_(BUFSIZE) P(ALLOCSIZE)
                T_ T_
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL_ALIGNMENT, ";
                }
                else {
                    cout << "STRAT = MAXIMUM_ALIGNMENT, ";
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

            if (veryVerbose) cout << "\tTesting 'allocateFromBufferRaw'"
                                  << endl;
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
        }

        }

        if (verbose) cout << "\nTesting buffer overflow." << endl;
        {

        static const struct {
            int d_line;       // line number
            int d_cursor;     // initial cursor position
            int d_bufSize;    // buffer size
            int d_allocSize;  // allocation request size
            St  d_strategy;   // alignment strategy
            int d_expCursor;  // expected cursor position after request
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
       {  L_,   63,     64,       1,          MAX,    63 }, // ALLOCSIZE=1

       {  L_,   64,     64,       2,          MAX,    64 }, // ALLOCSIZE=2
       {  L_,   63,     64,       2,          MAX,    63 },
       {  L_,   62,     64,       2,          MAX,    62 },

       {  L_,   64,     64,       4,          MAX,    64 }, // ALLOCSIZE=4
       {  L_,   63,     64,       4,          MAX,    63 },
       {  L_,   62,     64,       4,          MAX,    62 },
       {  L_,   61,     64,       4,          MAX,    61 },
       {  L_,   60,     64,       8,          MAX,    60 },

       {  L_,   64,     64,       8,          MAX,    64 }, // ALLOCSIZE=8
       {  L_,   63,     64,       8,          MAX,    63 },
       {  L_,   62,     64,       8,          MAX,    62 },
       {  L_,   61,     64,       8,          MAX,    61 },
       {  L_,   60,     64,       8,          MAX,    60 },
       {  L_,   59,     64,       8,          MAX,    59 },
       {  L_,   58,     64,       8,          MAX,    58 },
       {  L_,   57,     64,       8,          MAX,    57 },
       {  L_,   56,     64,      16,          MAX,    56 },

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

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        char *buffer = bufferStorage.buffer();

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE      = DATA[ti].d_line;
            const int CURSOR    = DATA[ti].d_cursor;
            const int BUFSIZE   = DATA[ti].d_bufSize;
            const int ALLOCSIZE = DATA[ti].d_allocSize;
            const St  STRAT     = DATA[ti].d_strategy;
            const int EXPCURSOR = DATA[ti].d_expCursor;

            if (veryVerbose) {
                T_ P_(LINE) P_(CURSOR) P_(BUFSIZE) P(ALLOCSIZE)
                T_ T_
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL_ALIGNMENT, ";
                }
                else {
                    cout << "STRAT = MAXIMUM_ALIGNMENT, ";
                }
                P(EXPCURSOR)
            }

            int tmpCursor = CURSOR;


            void *address = Obj::allocateFromBuffer(&tmpCursor,
                                                    buffer,
                                                    BUFSIZE,
                                                    ALLOCSIZE,
                                                    STRAT);

            LOOP2_ASSERT(EXPCURSOR, tmpCursor, EXPCURSOR == tmpCursor);
            LOOP_ASSERT(address, 0 == address);
        }

        }

#undef NAT
#undef MAX

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
