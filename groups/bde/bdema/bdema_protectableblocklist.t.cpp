// bdema_protectableblocklist.t.cpp  -*-C++-*-

#include <bdema_protectableblocklist.h>

#include <bdema_protectableblockdispenser.h>
#include <bdema_testprotectableblockdispenser.h>  // for testing only
#include <bslma_testallocatorexception.h>         // for testing only

#include <bsls_alignmentutil.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_c_signal.h>    // signal()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The protected block list is comprised of two class implementations,
// bdema_ProtectableBlockList and bdema_VallocProtectedAllocator.
//
// [ 2] Verify helper functions (testProtectedSet &
//                               TestProtectableBlockAllocator,
//                               BadProtectableBlockAllocator)
// [ 3] Basic constructors & accessors for bdema_ProtectableBlockList.
//      bdema_ProtectableBlockList(bdema_ProtectedMemoryAllocator *)
//      blockHeaderSize();
// [ 4] Manipulator: bdema_ProtectableBlockList::allocate
// [ 5] Manipulator: bdema_ProtectableBlockList::deallocate
// [ 6] Manipulator: bdema_ProtectableBlockList::protect
//                   bdema_ProtectableBlockList::unprotect
//      Accessor:    bdema_ProtectableBlockList::isProtected
// [ 7] Manipulator: bdema_ProtectableBlockList::release
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE

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
//                               GLOBAL TYPEDEF
//-----------------------------------------------------------------------------

typedef bdema_ProtectableBlockList            Obj;
typedef bdema_TestProtectableBlockDispenser   TestDisp;
typedef bdema_NativeProtectableBlockDispenser NativeDisp;
typedef bdema_MemoryBlockDescriptor           BlkDesc;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//---- Global variables used by the segmentation fault handler -------- //
bool   g_inTest = false;   // whether we are in a test
bool   g_fault  = false;   // whether a fault has occurred
Obj   *g_testingAlloc = 0; // A global variable that must refer to the
// -------------------------------------------------------------------- //

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
#ifndef BSLS_PLATFORM_OS_WINDOWS
    signal(SIGBUS, segfaultHandler);
#endif

    // protect the memory
    g_testingAlloc->protect();

    // attempt an assignment.  The segfaultHandler should be called, which will
    // in turn, unprotect the memory.
    *data = val;

    // verify that the segmentation fault handler was called and reset the
    // global segmentation fault variables
    g_testingAlloc = NULL;
    g_inTest       = false;
    signal(SIGSEGV, SIG_DFL);
#ifndef BSLS_PLATFORM_OS_WINDOWS
    signal(SIGBUS, SIG_DFL);
#endif
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

class myProtectedDataProvider {

    bdema_ProtectableBlockList d_blockList;

    // NOT IMPLEMENTED
    myProtectedDataProvider(const myProtectedDataProvider&);
    myProtectedDataProvider& operator=(const myProtectedDataProvider&);
//..
// We pass a 'bdema_ProtectableBlockDispenser' to the
// 'bdema_ProtectableBlockList' on construction.
//..
  public:
         // CREATORS
         myProtectedDataProvider(
                     bdema_ProtectableBlockDispenser *protectedDispener)
         : d_blockList(protectedDispener)
              // Create a 'myProtectedDataProvider' using the specified
              // 'protectedAllocator' to allocate memory.
         {
         }

// The 'bdema_ProtectableBlockList' is unprotected before it is used and
// then protected again after its use.  Note: memory will be wasted by not
// making use of the returned 'actualSize':
//..
         // MANIPULATORS
         void *protectedCopy(const void *data, int size)
             // Return a protected copy of the specified 'data' of specified
             // 'size'.  A call to 'protectedCopy' will allocate at least,
             // and probably significantly more than 'size' bytes of
             // data.  The behavior is undefined unless 'data' points to a
             // valid memory region of the specified 'size'.
         {
             d_blockList.unprotect();
             bdema_MemoryBlockDescriptor block = d_blockList.allocate(size);
             bsl::memcpy(block.address(), data, size);
             d_blockList.protect();
             return block.address();
         }

         void releaseProtectedCopy(void *data)
             // Deallocate the specified 'data'.  The behavior is undefined
             // unless 'data' was returned from a call to 'protectedCopy'.
         {
             d_blockList.unprotect();
             d_blockList.deallocate(data);
             d_blockList.protect();
         }
    };
//..

// Tests whether the specified 'block' returned from the protectable block
// list is protected by the specified 'dispenser'.  Behavior is undefined if
// the specified 'block' wasn't returned from a
// 'bdema_ProtectableBlockList' using the 'dispenser'.
bool isProtected(const TestDisp& dispenser, const BlkDesc& block)
{
    BlkDesc actualBlock(((char *)block.address()) - Obj::blockHeaderSize(),
                        block.size() + Obj::blockHeaderSize());
    return dispenser.isProtected(actualBlock);
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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    int ACTUAL_PG_SIZE    = NativeDisp::pageSize();
    int PG_SIZE           = TestDisp::BDEMA_DEFAULT_PAGE_SIZE;
    int BLOCK_HEADER_SIZE = Obj::blockHeaderSize();

    struct TestBlock {
        TestBlock                          *d_next_p;        // next block
        TestBlock                         **d_addrPrevNext;  // prev block
                                                             // next ptr
        int                                 d_size;          // size of
                                                             // allocation
        bsls_AlignmentUtil::MaxAlignedType  d_memory;        // force alignment
    };

    bdema_TestProtectableBlockDispenser testDispenser(PG_SIZE,
                                                            veryVeryVerbose);
    switch (test) { case 0:
      case 8: {
        // --------------------------------------------------------------------
        // USAGE TEST:
        //   Create a 'myProtectedDataProvider' initialized with a test block
        //   allocator.  Protected a few blocks and verify that they are
        //   protected.
        //
        // Testing:
        //   Ensure usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        bdema_TestProtectableBlockDispenser testDispenser(PG_SIZE);
        myProtectedDataProvider protector(&testDispenser);

        const char *valueA = "This is a string that needs protection.";
        const char *valueB = "This is also a string that needs protection.";

        char *protectedA = (char *)protector.protectedCopy(valueA,
                                                      bsl::strlen(valueA) + 1);
        char *protectedB = (char *)protector.protectedCopy(valueB,
                                                      bsl::strlen(valueB) + 1);

        ASSERT(0 == bsl::strcmp(protectedA, valueA));
        ASSERT(0 == bsl::strcmp(protectedB, valueB));

        TestBlock block;
        const int HEADER_SIZE = ((bsls_PlatformUtil::size_type)&block.d_memory)
                              - (bsls_PlatformUtil::size_type)&block;
        ASSERT(2 == testDispenser.numBlocksInUse());
        ASSERT(testDispenser.numBlocksProtected() ==
               testDispenser.numBlocksInUse());

    } break;
      case 7: {
        // --------------------------------------------------------------------
        // MANIPULATOR TEST: release
        //
        // Concerns:
        // That release frees all the memory under the
        // ProtectableBlockList and that it unprotects it's memory
        // before deallocating.
        //
        // Note: Most of the heavy lifting for this test is handled by the
        // TestProtectableBlockDispenser - which verifies that the
        // protection operations are correct and all the memory is successfully
        // released
        //
        // Plan:
        // Use a table of input data describing various allocation sizes to
        // initialize two allocators.  Set one to a protected state.  Call
        // release and verify the memory was release successfully.
        //
        // Testing:
        //     bdema_ProtectableBlockList::release()
        // --------------------------------------------------------------------
        if (verbose) {
            bsl::cout
                << "\nVerify:\n"
                << "\tbdema_ProtectableBlockList::release()\n"
                << "\t====================================" << bsl::endl;
        }

        int SIZES[] = {
            1,
            PG_SIZE - BLOCK_HEADER_SIZE,
            PG_SIZE,
            PG_SIZE + 1,
            2 * PG_SIZE - BLOCK_HEADER_SIZE,
            2 * PG_SIZE
        };
        int NUM_TESTS = sizeof(SIZES)/sizeof(*SIZES);

        ASSERT(0 == testDispenser.numBytesInUse());
        Obj a(&testDispenser); Obj b(&testDispenser);

        for (int i = 0; i < NUM_TESTS; ++i) {
            a.allocate(SIZES[i]);
            b.allocate(SIZES[i]);
        }
        b.protect();

        int bytes = testDispenser.numBytesInUse();
        ASSERT(0 < testDispenser.numBytesInUse());
        a.release();
        ASSERT(bytes/2 == testDispenser.numBytesInUse());
        b.release();
        ASSERT(0 == testDispenser.numBytesInUse());
    }; break;
      case 6: {
        // --------------------------------------------------------------------
        // MANIPULATOR TEST: protect/unprotect
        //
        // Note: Most of the heavy lifting for this test is handled by the
        // TestProtectableBlockAllocator - which verifies that the protection
        // operations are correct and all the memory is successfully
        // protected.
        //
        // Use a table of input data describing various allocation sizes to
        // verify that protect will protect all the allocated memory.
        //
        // Testing:
        //     bdema_ProtectableBlockList::protect()
        //     bdema_ProtectableBlockList::unprotect()
        //     bdema_ProtectableBlockList::isProtected() const
        // --------------------------------------------------------------------
        if (verbose) {
            bsl::cout
                << "\tVerify:\n"
                << "\tbdema_ProtectableBlockList::protect()\n"
                << "\tbdema_ProtectableBlockList::unprotect()\n"
                << "\t======================================" << bsl::endl;
        }
        {
            if (veryVerbose)
                bsl::cout << "\t\tverify various protect/unprotect calls"
                          << bsl::endl;

            int SIZES[] = {
                1,
                PG_SIZE - BLOCK_HEADER_SIZE,
                PG_SIZE,
                PG_SIZE + 1,
                2 * PG_SIZE - BLOCK_HEADER_SIZE,
                2 * PG_SIZE
            };
            int NUM_TESTS = sizeof(SIZES)/sizeof(*SIZES);

            for (int i = 0; i < NUM_TESTS; ++i) {
                Obj a(&testDispenser);
                // For each iteration select one more allocation from the
                // sample data to allocate.
                for (int j = 0; j <= i; ++j) {
                    a.allocate(SIZES[i]);
                }
                const int NUM_BLKS = testDispenser.numBlocksInUse();
                ASSERT(0 < NUM_BLKS);
                ASSERT(0 == testDispenser.numBlocksProtected());
                ASSERT(false == a.isProtected());

                a.protect();
                ASSERT(true == a.isProtected());
                ASSERT(NUM_BLKS == testDispenser.numBlocksProtected());

                a.protect();
                ASSERT(true == a.isProtected());
                ASSERT(NUM_BLKS == testDispenser.numBlocksProtected());

                a.unprotect();
                ASSERT(false == a.isProtected());
                ASSERT(0 == testDispenser.numBlocksProtected());

                a.unprotect();
                ASSERT(false == a.isProtected());
                ASSERT(0 == testDispenser.numBlocksProtected());

            }
        }
    }; break;
      case 5: {
        // --------------------------------------------------------------------
        // MANIPULATOR TEST:
        //                bdema_ProtectableBlockList::deallocate(void *)
        //
        // Use a table of input data describing various allocation sizes to
        // verify that deallocate does not effect the allocated memory.
        //
        // Testing:
        //    bdema_ProtectableBlockList::deallocate(void *)
        // --------------------------------------------------------------------
          if (verbose) {
              bsl::cout
                  << "\tVerify:\n"
                  << "\tbdema_ProtectableBlockList::deallocate()\n"
                  << "\t======================================="
                  << bsl::endl;
          }
          const int BHS = BLOCK_HEADER_SIZE;
          {
              struct {
                      int  d_size;
              } DATA []  = {
                  { 1              },
                  { PG_SIZE - BHS  },
                  { PG_SIZE        },
                  { PG_SIZE + 1    },
              };
              const int NUM_TESTS = sizeof(DATA)/sizeof(*DATA);
              bdema_MemoryBlockDescriptor BLOCKS[NUM_TESTS];

              for (int i = 0; i < NUM_TESTS; ++i) {
                  Obj                    a(&testDispenser);
                  for (int j = 0; j < NUM_TESTS; ++j) {
                      BLOCKS[j] = a.allocate(DATA[j].d_size);
                  }

                  for (int j = 0; j < NUM_TESTS; ++j) {

                      // Ensure the blocks are deallocated in different orders
                      // for each test.
                      int idx = (j + i) % NUM_TESTS;

                      int numBytes = testDispenser.numBytesInUse();
                      a.deallocate(BLOCKS[idx].address());

                      if (veryVeryVerbose) {
                          bsl::cout << "i: " << i << "  j:  " << j << " idx:"
                                    << idx << "  numBytes: "
                                    << bsl::setw(6) << numBytes
                                    << "  ACT SIZE+BHS: "
                                    << bsl::setw(6) << BLOCKS[idx].size() + BHS
                                    << "  alloc: "
                                    << bsl::setw(6)
                                    << testDispenser.numBytesInUse()
                                    << bsl::endl;
                      }

                      ASSERT( numBytes - BLOCKS[idx].size() - BHS ==
                              testDispenser.numBytesInUse());
                      numBytes = testDispenser.numBytesInUse();
                  }
                  ASSERT(0 == testDispenser.numBytesInUse());
              }
          }
      }; break;
      case 4: {
        // --------------------------------------------------------------------
        // MANIPULATOR TEST:
        //               bdema_ProtectableBlockList::allocate(int)
        //
        // Use a table of input data and verify that the returns from
        // allocate match the expected returns that were computed locally.
        // Ensure that memory can be allocated from a protected state.
        //
        // Testing:
        //   bdema_ProtectableBlockList::allocate(int)
        // --------------------------------------------------------------------

          if (verbose) {
              bsl::cout
                << "\tVerify\n"
                << "\tbdema_ProtectableBlockList::allocate(int)\n"
                << "\t==============================================="
                << bsl::endl;
          }

          const int BHS = BLOCK_HEADER_SIZE;
          // 1. Use a table of input data and verify that the returns from
          //    allocate match the expected returns that were computed
          //    locally.
          struct {
                  int d_allocSize;
                  int d_expectedResult;
                  int d_actualAlloc;
          } DATA[] = {
          { 0,                 0,                 0},
          { 1,                 PG_SIZE - BHS,     PG_SIZE},
          { PG_SIZE - BHS,     PG_SIZE - BHS,     PG_SIZE},
          { PG_SIZE,           2 * PG_SIZE - BHS, 2 * PG_SIZE},
          { PG_SIZE+1,         2 * PG_SIZE - BHS, 2 * PG_SIZE},
          { 2 * PG_SIZE - BHS, 2 * PG_SIZE - BHS, 2 * PG_SIZE},
          { 2 * PG_SIZE,       3 * PG_SIZE - BHS, 3 * PG_SIZE},
          { 2 * PG_SIZE + 1,   3 * PG_SIZE - BHS, 3 * PG_SIZE}
            };
          const int NUM_TESTS  = sizeof(DATA)/sizeof(*DATA);

          if (verbose) {
              bsl::cout << "PAGE SIZE: " << PG_SIZE << bsl::endl;
              bsl::cout << "BLOCK_HEADER_SIZE: "
                        << BLOCK_HEADER_SIZE << bsl::endl;
          }

          // Execute the test on an unprotected allocator
          Obj a(&testDispenser);
          for (int i = 0; i < NUM_TESTS; ++i) {
              BEGIN_BDEMA_EXCEPTION_TEST {

                  bdema_MemoryBlockDescriptor block =
                      a.allocate(DATA[i].d_allocSize);
                  ASSERT(0 == testDispenser.numBlocksProtected());
                  if (!block.isNull()) {
                      ASSERT(!isProtected(testDispenser, block));
                  }
                  int  actualSize = block.size();

                  // verify the actual size is rounded up to the nearest
                  // page size and accounts for the block header).  Use
                  // two methods to calculate the expected value.
                  int pages =DATA[i].d_actualAlloc/PG_SIZE;
                  if (veryVeryVerbose) {
                      bsl::cout << "Size: "
                                << bsl::setw(6) << DATA[i] .d_allocSize
                                << "  Actual: "  << bsl::setw(6)
                                << actualSize
                                << "  expectedAlloc: "
                                << bsl::setw(6) << DATA[i].d_expectedResult
                                << "  Actual w/Header: " <<  bsl::setw(6)
                                << actualSize + BLOCK_HEADER_SIZE
                                << bsl::setw(2)
                                << "  pages:" << pages << bsl::endl;
                  }
                  LOOP2_ASSERT( actualSize, DATA[i].d_expectedResult,
                                actualSize == DATA[i].d_expectedResult);
                  ASSERT( testDispenser.lastAllocateNumBytes() ==
                          DATA[i].d_actualAlloc);
              } END_BDEMA_EXCEPTION_TEST
          }
          a.release();

      } break;
      case 3: {
        // -------------------------------------------------------------------
        //  BASIC ACCESSORS test for  bdema_ProtectableBlockList.
        //
        // Testing:
        // 1.  bdema_ProtectableBlockList(
        //                             bdema_ProtectableBlockDispenser *)
        //    ~bdema_ProtectableBlockList()
        // 2.  bdema_ProtectableBlockList::BLOCK_HEADER_SIZE
        //
        // -------------------------------------------------------------------
          if (verbose) {
              bsl::cout << "\tVerify basic constructors and accessors for"
                        << " bdema_ProtectableBlockList\n"
                        << "\t=======================" << bsl::endl;
          }
          {
              if (veryVerbose) {
                  bsl::cout << "\tbdema_ProtectableBlockList()"
                            << bsl::endl;
              }
              int NUM_ALLOCS = 10;
              {
                  Obj  a(&testDispenser);
                  ASSERT(0 == testDispenser.numBytesInUse());
                  for (int i = 0; i < NUM_ALLOCS; ++i) {
                      a.allocate(1);
                      ASSERT(PG_SIZE * (i + 1) ==
                             testDispenser.numBytesInUse());
                  }
                  ASSERT( PG_SIZE * NUM_ALLOCS ==
                          testDispenser.numBytesInUse());
              }
              ASSERT( 0 == testDispenser.numBytesInUse());

              if (veryVerbose) {
                  bsl::cout << "\t~bdema_ProtectableBlockList() "
                            << "protected mem"
                            << bsl::endl;
              }
              {
                  Obj a(&testDispenser);
                  ASSERT(0 == testDispenser.numBytesInUse());
                  int size;
                  a.allocate(1);
                  a.protect();
                  ASSERT(PG_SIZE == testDispenser.numBytesInUse());
              }
          }
          {
              if (veryVerbose) {
                  bsl::cout
                    << "\tbdema_ProtectableBlockList::BLOCK_HEADER_SIZE"
                    << bsl::endl;
              }

              struct Test {
                  void                                *d_ptr;
                  void                               **d_backPtr;
                  int                                  d_size;
                  bsls_AlignmentUtil::MaxAlignedType   d_memory;
              };

              int expectedSize = sizeof(Test) -
                                 sizeof(bsls_AlignmentUtil::MaxAlignedType);
              ASSERT(expectedSize == Obj::blockHeaderSize());
          }
      } break;
      case 2: {
        // -------------------------------------------------------------------
        // VERIFY HELPER FUNCTIONS:
        // Test the helper functions and classes defined in the test driver
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
            bdema_MemoryBlockDescriptor block = a.allocate(1);
            char *memX = static_cast<char *>(block.address());

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
        // BREATHING TEST 1: Test that the allocated regions are protected by
        //                   this block list.
        // Testing:
        //   Protect/unprotect
        // --------------------------------------------------------------------

        Obj a;
        const int SIZES[]    = { 0,
                                 1,
                                 ACTUAL_PG_SIZE - BLOCK_HEADER_SIZE,
                                 ACTUAL_PG_SIZE,
                                 ACTUAL_PG_SIZE+1,
                                 2*ACTUAL_PG_SIZE - BLOCK_HEADER_SIZE,
                                 2*ACTUAL_PG_SIZE,
                                 2*ACTUAL_PG_SIZE+1
                               };
        const int NUM_TESTS  = sizeof(SIZES)/sizeof(int);
        int       ACTUAL_SIZES[NUM_TESTS];
        char     *DATA[NUM_TESTS];

        for (int i = 0; i < NUM_TESTS; ++i) {
            // Initialized DATA
            bdema_MemoryBlockDescriptor block= a.allocate(SIZES[i]);
            DATA[i]         = static_cast<char *>(block.address());
            ACTUAL_SIZES[i] = block.size();
            memset(DATA[i], 'x', ACTUAL_SIZES[i]);

            if (SIZES[i] == 0) {
                ASSERT(0 == ACTUAL_SIZES[i]);
                continue;
            }

            // Set test points
            int start = 0;
            int mid = ACTUAL_SIZES[i] / 2;
            int end = ACTUAL_SIZES[i] - 1;

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
