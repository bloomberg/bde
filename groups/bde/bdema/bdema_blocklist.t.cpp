// bdema_blocklist.t.cpp                                              -*-C++-*-

#include <bdema_blocklist.h>
#include <bsls_alignmentutil.h>
#include <bslma_testallocator.h>      // for testing purpose only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The goals of this 'bdema_BlockList' test suite are to verify that 1) the
// 'allocate' method allocates memory blocks of the correct size; 2) the
// returned memory address is properly offset to reserve room for internal
// data; 3) the returned memory is maximally aligned; 4) the allocated memory
// blocks are chained together into a list; and 5) both the 'release' method
// and the destructor deallocate all associated memory blocks.
//
// To achieve Goals 1 and 2, varying memory request sizes supplied to the
// 'allocate' method are compared with the corresponding sizes recorded by the
// test allocator.  Each recorded size is verified to equal the least integral
// value that can satisfy the requested memory plus the internal data used by
// the memory manager.  Goal 3 is achieved by using the method
// 'calculateAlignmentOffset' to verify that memory addresses returned by the
// 'allocate' method yield 0 offset.  Goals 4 and 5 are achieved by ensuring
// that, when the the block list is destroyed or its 'release' method is
// invoked, the test allocator indicates that all allocated memory is
// deallocated.
//-----------------------------------------------------------------------------
// [1] bdema_BlockList(basicAllocator);
// [2] ~bdema_BlockList();
// [1] void *allocate(numBytes);
// [2] void release();
// [3] void deallocate(address);
//-----------------------------------------------------------------------------
// [4] USAGE EXAMPLE
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

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\t" << #K << ": " << K << "\n"; \
              aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                      SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define TAB cout << '\t';

//=============================================================================
//                               GLOBAL TYPEDEF
//-----------------------------------------------------------------------------

typedef bdema_BlockList Obj;

// These type are copied from the 'bdema_blocklist.h' for testing purposes.

struct Block {
    Block                              *d_prev_p;
    Block                              *d_next_p;
    bsls_AlignmentUtil::MaxAlignedType  d_memory;  // force alignment
};

inline static int roundUp(int x, int y)
    // Round up the specified 'x' to the nearest whole integer multiple of the
    // specified 'y'.  The behavior is undefined unless 0 <= x and 0 < y;
{
    return (x + y - 1) / y * y;
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// A 'bdema_BlockList' object is commonly used to supply memory to more
// elaborate memory managers that distribute parts of each (larger) allocated
// memory block supplied by the 'bdema_BlockList' object.  The 'my_StrPool'
// memory pool manager shown below requests relatively large blocks of memory
// from its 'bdema_BlockList' member object and distributes memory chunks of
// varying sizes from each block on demand:
//..
    // my_strpool.h

    class my_StrPool {
        int              d_blockSize; // size of current memory block
        char            *d_block_p;   // current free memory block
        int              d_cursor;    // offset to next available byte in block
        bdema_BlockList  d_blockList; // supplies managed memory blocks

      private:
        void *allocateBlock(int numBytes);
            // Request a new memory block of at least the specified 'numBytes'
            // size and allocate the initial 'numBytes' from this block.
            // Return the address of the allocated memory.

      private:
        // NOT IMPLEMENTED
        my_StrPool(const my_StrPool&);
        my_StrPool& operator=(const my_StrPool&);

      public:
        my_StrPool(bslma_Allocator *basicAllocator = 0);
            // Create a memory manager using the specified 'basicAllocator' to
            // supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.

        ~my_StrPool();
            // Destroy this object and release all associated memory.

        void *allocate(int numBytes);
            // Allocate the specified 'numBytes' of memory and return its
            // address.

        void release();
            // Release all memory currently allocated through this object.
    };

    inline
    void *my_StrPool::allocate(int numBytes)
    {
        if (numBytes <= 0) {
            return 0;
        }

        if (d_block_p && numBytes + d_cursor <= d_blockSize) {
            char *p = d_block_p + d_cursor;
            d_cursor += numBytes;
            return p;
        }
        else {
            return allocateBlock(numBytes);
        }
    }

    inline
    void my_StrPool::release()
    {
        d_blockList.release();
        d_block_p = 0;
    }

//  // ...

    // my_strpool.cpp

    enum {
        INITIAL_SIZE  = 128,  // initial block size

        GROWTH_FACTOR = 2,    // multiplicative factor by which to grow block

        THRESHOLD     = 128   // size, beyond which, an individual block may be
                              // allocated if it doesn't fit in current block
    };

    void *my_StrPool::allocateBlock(int numBytes)
    {
        BSLS_ASSERT(0 < numBytes);

        if (THRESHOLD < numBytes) { // Alloc separate block if above threshold.
            return (char *) d_blockList.allocate(numBytes);
        }
        else {
            if (d_block_p) { // Don't increase block size if no current block.
                d_blockSize *= GROWTH_FACTOR;
            }
            d_block_p = (char *) d_blockList.allocate(d_blockSize);
            d_cursor = numBytes;
            return d_block_p;
        }
    }

    my_StrPool::my_StrPool(bslma_Allocator *basicAllocator)
    : d_blockSize(INITIAL_SIZE)
    , d_block_p(0)
    , d_blockList(basicAllocator)  // the blocklist knows about 'bslma_default'
    {
    }

    my_StrPool::~my_StrPool()
    {
        BSLS_ASSERT(INITIAL_SIZE <= d_blockSize);
        BSLS_ASSERT(d_block_p || 0 <= d_cursor && d_cursor <= d_blockSize);
    }
//..
// In the code shown above, the 'my_StrPool' memory manager allocates from its
// 'bdema_BlockList' member object an initial memory block of size
// 'INITIAL_SIZE'.  This size is multiplied by 'GROWTH_FACTOR' each time a
// depleted memory block is replaced by a newly allocated block.  The
// 'allocate' method distributes memory from the current memory block
// piecemeal, except when the requested size (1) is not available in the
// current block, or (2) exceeds the 'THRESHOLD_SIZE', in which case a separate
// memory block is allocated and returned.  When the 'my_StrPool' memory
// manager is destroyed, its 'bdema_BlockList' member object is also destroyed,
// which in turn automatically deallocates all of its managed memory blocks.

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

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE TEST:
        //   Create a 'my_StrPool' initialized with a test allocator.  Allocate
        //   memory of varying sizes and verify that memory addresses are
        //   returned (except when memory size is 0).  Also verify that when
        //   the memory manager is destroyed, the allocator indicates that all
        //   memory has been deallocated.
        //
        // Testing:
        //   Ensure usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        bslma_TestAllocator a(veryVeryVerbose);

        if (verbose) cout << "Testing 'my_StrPool'." << endl;
        {
            my_StrPool strPool(&a);

            const int SIZE[] = { 0, 10, 127, 128, 129, 1000 };
            const int NUM_DATA = sizeof SIZE / sizeof *SIZE;
            for (int di = 0; di < NUM_DATA; ++di) {
                void *p = strPool.allocate(SIZE[di]);
                if (veryVerbose) { TAB; P_(SIZE[di]); P(p); }
                if (SIZE[di]) {
                    LOOP_ASSERT(di, p);
                } else {
                    LOOP_ASSERT(di, !p);
                }
            }
        }
        ASSERT(0 == a.numBytesInUse());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // DEALLOCATE TEST
        //   Create a block list allocator initialized with a test allocator.
        //   Request memory of varying sizes and then deallocate each memory.
        //   Verify that the number of bytes in use indicated by the test
        //   allocator does decrease after each 'deallocate' method invocation.
        //   Also ensure 'deallocate(0)' has no effect.
        //
        // Testing:
        //   void deallocate(address);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DEALLOCATE TEST" << endl
                                  << "===============" << endl;

        if (verbose) cout << "Testing 'deallocate'." << endl;

        const int DATA[] = { 0, 5, 12, 24, 32, 64, 256, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma_TestAllocator ta(veryVeryVerbose);
        bdema_BlockList sa(&ta);

        int lastNumBytesInUse = ta.numBytesInUse();

        for (int i = 0; i < NUM_DATA; ++i) {
            const int SIZE = DATA[i];
            void *p = sa.allocate(SIZE);
            const int numBytesInUse = ta.numBytesInUse();
            sa.deallocate(p);
            LOOP_ASSERT(i, numBytesInUse >= ta.numBytesInUse());
            LOOP_ASSERT(i, lastNumBytesInUse <= ta.numBytesInUse());
            const int numBytesInUse2 = ta.numBytesInUse();
            sa.deallocate(0);
            LOOP_ASSERT(i, numBytesInUse2 == ta.numBytesInUse());
            lastNumBytesInUse = ta.numBytesInUse();
        }

        void *p[NUM_DATA];
        for (int i = 0; i < NUM_DATA; ++i) {
            p[i] = sa.allocate(DATA[i]);
        }

        for (int i = NUM_DATA - 1; i >= 0; --i) {
            const int numBytesInUse = ta.numBytesInUse();
            sa.deallocate(p[i]);
            LOOP_ASSERT(i, numBytesInUse >= ta.numBytesInUse());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // RELEASE TEST:
        //   Create a 'bdema_BlockList' memory manager initialized with a
        //   'TestAllocator' allocator.  Allocate memory of varying sizes from
        //   the memory manager and then invoke its 'release' method.  Verify
        //   that the allocator indicates all memory has been deallocated.  To
        //   test the destructor, perform the same test again, but let the
        //   memory manager go out of scope without calling the 'release'
        //   method.
        //
        // Testing:
        //   ~bdema_BlockList();
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RELEASE TEST" << endl
                                  << "============" << endl;

        const int DATA[] = { 0, 1, 10, 100, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma_TestAllocator a(veryVeryVerbose);

        if (verbose) cout << "Testing 'release'." << endl;
        {
            Obj mX(&a);
            for (int di = 0; di < NUM_DATA; ++di) {
                const int SIZE = DATA[di];
                void *p = mX.allocate(SIZE);
                if (veryVerbose) { TAB; P_(SIZE); P(p); }
            }
            mX.release();

            ASSERT(0 == a.numBytesInUse());
        }

        if (verbose) cout << "Testing destructor." << endl;
        {
            {
                Obj mX(&a);
                for (int di = 0; di < NUM_DATA; ++di) {
                    const int SIZE = DATA[di];
                    void *p = mX.allocate(SIZE);
                    if (veryVerbose) { TAB; P_(SIZE); P(p); }
                }
            }
            ASSERT(0 == a.numBytesInUse());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // ALLOCATE TEST:
        //   Iterate over a set of tabulated vectors of varying allocation
        //   request sizes and perform independent tests.  For each test,
        //   create a 'bdema_BlockList' memory manager supplied with a
        //   'TestAllocator' allocator.  Invoke the memory manager's 'allocate'
        //   method with the allocation size specified in the test vector, and
        //   verify that the actual size recorded by the allocator is equal to
        //   the expected size.  Also verify that the returned memory address
        //   is properly offset to 1) reserve room for the memory block header,
        //   and 2) produce a 0 offset when supplied to the
        //   'bsls_AlignmentUtil's 'calculateAlignmentOffset' method.  Note
        //   that the expected size varies depending on the platform, and it
        //   is chosen based on the memory block header size.
        //
        // Testing:
        //   bdema_BlockList(basicAllocator);
        //   void *allocate(numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "ALLOCATE TEST" << endl
                                  << "=============" << endl;

        typedef bsls_AlignmentUtil U;
        const int BLKSZ = sizeof(Block) -
                                        bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;

        if (veryVerbose) { TAB; P_(BLKSZ); P(U::BSLS_MAX_ALIGNMENT); }

        struct {
            int d_line;      // line number
            int d_size;      // memory request size
        } DATA[] = {
           // Line   Allocation
           // No.    Size
           //----    --------------
            { L_,    0               },
            { L_,    5 * BLKSZ - 0   },
            { L_,    5 * BLKSZ - 1   },
            { L_,    5 * BLKSZ - 2   },
            { L_,    5 * BLKSZ - 3   },
            { L_,    5 * BLKSZ - 4   },
            { L_,    5 * BLKSZ - 5   },
            { L_,    5 * BLKSZ - 6   },
            { L_,    5 * BLKSZ - 7   },
            { L_,    5 * BLKSZ - 8   },
            { L_,    5 * BLKSZ - 9   },
            { L_,    5 * BLKSZ - 10  },
            { L_,    5 * BLKSZ - 11  },
            { L_,    5 * BLKSZ - 12  },
            { L_,    5 * BLKSZ - 13  },
            { L_,    5 * BLKSZ - 14  },
            { L_,    5 * BLKSZ - 15  },
            { L_,    5 * BLKSZ - 16  }
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting 'allocate'." << endl;

        bslma_TestAllocator a(veryVeryVerbose);

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE = DATA[di].d_line;
            const int SIZE = DATA[di].d_size;

            int expSize;
            /* TBD
            switch (U::BSLS_MAX_ALIGNMENT) {
              case 1:  ??? // If misaligned compiler option is used
              case 4:  expSize = DATA[di].d_expSize4;  break;
              case 8:  expSize = DATA[di].d_expSize8;  break;
              case 16: expSize = DATA[di].d_expSize16; break;
            }
            */
            expSize = roundUp(SIZE + BLKSZ * (SIZE > 0),
                              U::BSLS_MAX_ALIGNMENT);

            const int EXP_SZ = expSize;

            Obj mX(&a);
            void *p = mX.allocate(SIZE);
            int offset = U::calculateAlignmentOffset(p, U::BSLS_MAX_ALIGNMENT);
            const void *EXP_P = p ? (char *) a.lastAllocatedAddress() + BLKSZ
                                  : 0;

            int numBytes = a.lastAllocatedNumBytes();
            if (veryVerbose) {
                TAB; P_(SIZE); P_(numBytes); P_(EXP_SZ);
                P_(p); P_(EXP_P); P(offset);
            }

            LOOP2_ASSERT(LINE, di,  EXP_P == p);
            LOOP2_ASSERT(LINE, di, EXP_SZ == numBytes);
            LOOP2_ASSERT(LINE, di,      0 == offset);
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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
