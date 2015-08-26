// bdlma_sequentialallocator.t.cpp                                    -*-C++-*-
#include <bdlma_sequentialallocator.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

#undef GS  // Solaris 2.10 x86 /usr/include/sys/regset.h

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// A 'bdlma::SequentialAllocator' adapts the 'bdlma::SequentialPool' mechanism
// to the 'bdlma::ManagedAllocator' protocol.  The primary concern is that the
// allocator correctly proxies the memory allocation requests to the sequential
// pool it adapts.
//
// To test that requests are correctly proxied, we create a sequential
// allocator and a sequential pool using the same external buffer and two
// different test allocators.  We then verify that either:
//
// 1) The addresses returned by the allocator and the pool are the same (i.e.,
//    the memory is allocated from the buffer).
//
// 2) If the memory used by either test allocator is non-zero, the number of
//    bytes used by both test allocators is the same.
//
// We also need to verify that the 'deallocate' method has no effect.  Again,
// we make use of the test allocator to ensure that no memory is deallocated
// when the 'deallocate' method of a sequential allocator is invoked on
// previously allocated memory.
//
// Finally, the destructor of 'bdlma::SequentialAllocator' is tested throughout
// the test driver.  At destruction, the allocator should reclaim all
// outstanding allocated memory.  By setting the global allocator, default
// allocator, and object allocator to different test allocators, we can
// determine whether all memory had been released by the destructor of the
// sequential allocator.
//-----------------------------------------------------------------------------
// // CREATORS
// [ 2] bdlma::SequentialAllocator(Alloc *a = 0);
// [ 2] bdlma::SequentialAllocator(GS g, Alloc *a = 0);
// [ 2] bdlma::SequentialAllocator(AS a, Alloc *a = 0);
// [ 2] bdlma::SequentialAllocator(GS g, AS a, Alloc *a = 0);
//
// [ 2] bdlma::SequentialAllocator(int i, Alloc *a = 0);
// [ 2] bdlma::SequentialAllocator(int i, GS g, Alloc *a = 0);
// [ 2] bdlma::SequentialAllocator(int i, AS a, Alloc *a = 0);
// [ 2] bdlma::SequentialAllocator(int i, GS g, AS a, Alloc *a = 0);
//
// [ 2] bdlma::SequentialAllocator(int i, int m, Alloc *a = 0);
// [ 2] bdlma::SequentialAllocator(int i, int m, GS g, Alloc *a = 0);
// [ 2] bdlma::SequentialAllocator(int i, int m, AS a, Alloc *a = 0);
// [ 2] bdlma::SequentialAllocator(int i, int m, GS g, AS a, Alloc *a = 0);
//
// [  ] ~bdlma::SequentialAllocator();
//
// // MANIPULATORS
// [ 2] void *allocate(size_type size);
// [ 5] void *allocateAndExpand(size_type *size);
// [ 3] void deallocate(void *address);
// [ 4] void release();
// [ 7] void reserveCapacity(int numBytes);
// [ 6] int truncate(void *address, int originalSize, int newSize);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE TEST

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

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL VARIABLES / TYPEDEF FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlma::SequentialAllocator Obj;

typedef bsls::Alignment::Strategy  Strat;

enum { k_MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT };

enum { k_DEFAULT_SIZE = 256 };

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// Allocators are often supplied, at construction, to objects requiring
// dynamically-allocated memory.  For example, consider the following
// 'my_DoubleStack' class whose constructor takes a 'bslma::Allocator *':
//..
    // my_doublestack.h
    // ...

    class my_DoubleStack {
        // This class implements a stack that stores 'double' values.

        // DATA
        double           *d_stack_p;      // dynamically-allocated array
        int               d_size;         // physical capacity of stack
        int               d_length;       // next available index in stack
        bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)

      private:
        // PRIVATE MANIPULATORS
        void increaseCapacity();
            // Increase the capacity of this stack by at least one element.

        // Not implemented:
        my_DoubleStack(const my_DoubleStack&);

      public:
        // CREATORS
        explicit my_DoubleStack(bslma::Allocator *basicAllocator = 0);
            // Create a stack that stores 'double' values.  Optionally specify
            // a 'basicAllocator' used to supply memory.  If 'basicAllocator'
            // is 0, the currently installed default allocator is used.

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
            increaseCapacity();
        }
        d_stack_p[d_length++] = value;
    }

    // ...

    // my_doublestack.cpp

    // PRIVATE MANIPULATORS
    void my_DoubleStack::increaseCapacity()
    {
        // Implementation elided.
        // ...
    }

    // CREATORS
    my_DoubleStack::my_DoubleStack(bslma::Allocator *basicAllocator)
    : d_size(1)
    , d_length(0)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_stack_p = static_cast<double *>(
                          d_allocator_p->allocate(d_size * sizeof *d_stack_p));
    }
//..
// Note that, when the allocator passed in is a 'bdlma::SequentialAllocator',
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

    // ...

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // As part of our overall allocator testing strategy, we will create three
    // test allocators.

    // Object Test Allocator.
    bslma::TestAllocator objectAllocator("Object Allocator",
                                         veryVeryVeryVerbose);

    // Default Test Allocator.
    bslma::TestAllocator defaultAllocator("Default Allocator",
                                          veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    // Global Test Allocator.
    bslma::TestAllocator globalAllocator("Global Allocator",
                                         veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 8: {
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

//..
// In 'main', users can create a 'bdlma::SequentialAllocator' and pass it to
// the constructor of 'my_DoubleStack':
//..
        bdlma::SequentialAllocator sequentialAlloc;
        my_DoubleStack dstack(&sequentialAlloc);
//..

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // 'reserveCapacity' TEST
        //
        // Concerns:
        //   1) That if there is sufficient memory within the internal buffer,
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
        //   5) That invoking 'reserveCapacity' with 0 bytes succeeds with no
        //      dynamic memory allocation.
        //
        //   6) QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   Create a 'bdlma::SequentialAllocator' using a test allocator and
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
        //   For concern 6, verify that, in appropriate build modes, defensive
        //   checks are triggered.
        //
        // Testing:
        //   void reserveCapacity(int numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'reserveCapacity' TEST" << endl
                                  << "======================" << endl;

        enum { k_INITIAL_SIZE = 64, k_MAX_BUFFER = k_INITIAL_SIZE * 4 };

        if (verbose) cout << "\nTesting that 'reserveCapacity' does not "
                             "trigger dynamic memory allocation." << endl;

        {
            Obj mX(k_INITIAL_SIZE, k_MAX_BUFFER, &objectAllocator);
            bsls::Types::Int64 numBytesUsed = objectAllocator.numBytesInUse();

            mX.reserveCapacity(k_INITIAL_SIZE / 2);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            mX.allocate(k_INITIAL_SIZE / 2);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            mX.reserveCapacity(k_INITIAL_SIZE * 2);
            ASSERT(numBytesUsed < objectAllocator.numBytesInUse());
            numBytesUsed = objectAllocator.numBytesInUse();

            mX.allocate(k_INITIAL_SIZE * 2);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            mX.reserveCapacity(k_MAX_BUFFER * 2);
            ASSERT(numBytesUsed < objectAllocator.numBytesInUse());
            numBytesUsed = objectAllocator.numBytesInUse();

            mX.allocate(k_MAX_BUFFER * 2);
            ASSERT(numBytesUsed = objectAllocator.numBytesInUse());
        }

        if (verbose) cout << "\nTesting 'reserveCapacity' on a default "
                             "constructed allocator." << endl;
        {
            Obj mX(&objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());

            mX.reserveCapacity(k_DEFAULT_SIZE);
            bsls::Types::Int64 numBytesUsed = objectAllocator.numBytesInUse();

            mX.allocate(k_DEFAULT_SIZE);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            mX.reserveCapacity(k_DEFAULT_SIZE * 4);
            ASSERT(numBytesUsed < objectAllocator.numBytesInUse());
            numBytesUsed = objectAllocator.numBytesInUse();

            mX.allocate(k_DEFAULT_SIZE * 4);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());
        }

        if (verbose) cout << "\nTesting 'reserveCapacity' with 0 bytes"
                          << endl;
        {
            Obj mX(&objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());

            mX.reserveCapacity(0);
            ASSERT(0 == objectAllocator.numBytesInUse());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;

            {
                ASSERT_SAFE_PASS(mX.reserveCapacity( 0));

                ASSERT_SAFE_FAIL(mX.reserveCapacity(-1));
            }
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
        //   3) QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   For concern 1, using the table-driven technique, create test
        //   vectors having the alignment strategy, initial allocation size,
        //   the new size, and expected offset.  First allocate memory of
        //   initial allocation size, then truncate to the new size and
        //   allocate memory (1 byte) again.  Verify that the latest allocation
        //   matches the expected offset.
        //
        //   For concern 2, truncate the memory returned by the initial
        //   allocation, and verify that the return value is 'originalSize'.
        //
        //   For concern 3, verify that, in appropriate build modes, defensive
        //   checks are triggered.
        //
        // Testing:
        //   int truncate(void *address, int originalSize, int newSize);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'truncate' TEST" << endl
                                  << "===============" << endl;

#define NAT bsls::Alignment::BSLS_NATURAL
#define MAX bsls::Alignment::BSLS_MAXIMUM
#define BYT bsls::Alignment::BSLS_BYTEALIGNED

        if (verbose) cout << "\nTesting 'truncate'." << endl;

        static const struct {
            int   d_line;         // line number
            Strat d_strategy;     // alignment strategy
            int   d_initialSize;  // size of initial allocation request
            int   d_newSize;      // truncate size
            int   d_expOffset;    // expected memory offset
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
            {  L_,      MAX,           1,           1, k_MAX_ALIGN },
            {  L_,      MAX,           2,           0,           0 },
            {  L_,      MAX,           2,           1, k_MAX_ALIGN },
            {  L_,      MAX,           2,           2, k_MAX_ALIGN },
            {  L_,      MAX,           3,           0,           0 },
            {  L_,      MAX,           3,           1, k_MAX_ALIGN },
            {  L_,      MAX,           3,           2, k_MAX_ALIGN },
            {  L_,      MAX,           3,           3, k_MAX_ALIGN },
            {  L_,      MAX,           8,           4, k_MAX_ALIGN },
            {  L_,      MAX,         511,         511,          -1 },  // *
            {  L_,      MAX,         512,           0,           0 },
            {  L_,      MAX,         512,           1, k_MAX_ALIGN },
            {  L_,      MAX,         512,         511,          -1 },  // *
            {  L_,      MAX,         512,         512,          -1 },  // *

            // 1-BYTE ALIGNMENT
            {  L_,      BYT,           1,           0,           0 },
            {  L_,      BYT,           1,           1,           1 },
            {  L_,      BYT,           2,           0,           0 },
            {  L_,      BYT,           2,           1,           1 },
            {  L_,      BYT,           2,           2,           2 },
            {  L_,      BYT,           3,           0,           0 },
            {  L_,      BYT,           3,           1,           1 },
            {  L_,      BYT,           3,           2,           2 },
            {  L_,      BYT,           3,           3,           3 },
            {  L_,      BYT,           4,           0,           0 },
            {  L_,      BYT,           4,           1,           1 },
            {  L_,      BYT,           4,           2,           2 },
            {  L_,      BYT,           4,           3,           3 },
            {  L_,      BYT,           8,           4,           4 },
            {  L_,      BYT,         511,         511,         511 },
            {  L_,      BYT,         512,           0,           0 },
            {  L_,      BYT,         512,         511,         511 },
            {  L_,      BYT,         512,         512,          -1 },  // *
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE        = DATA[ti].d_line;         // line number
            const Strat STRAT       = DATA[ti].d_strategy;     // align. strat.
            const int   INITIALSIZE = DATA[ti].d_initialSize;  // initial size
            const int   NEWSIZE     = DATA[ti].d_newSize;      // new size
            const int   EXPOFFSET   = DATA[ti].d_expOffset;    // exp. offset

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL ALIGNMENT, ";
                }
                else if (STRAT == MAX) {
                    cout << "STRAT = MAXIMUM ALIGNMENT, ";
                }
                else {  // STRAT == BYTE
                    cout << "STRAT = 1-BYTE ALIGNMENT, ";
                }
                P_(INITIALSIZE) P_(NEWSIZE) P(EXPOFFSET)
            }

            Obj mX(STRAT, &objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());

            void *addr1 = mX.allocate(INITIALSIZE);
            ASSERT(0 != objectAllocator.numBytesInUse());

            bsls::Types::Int64 used = objectAllocator.numBytesInUse();

            mX.truncate(addr1, INITIALSIZE, NEWSIZE);
            ASSERT(used == objectAllocator.numBytesInUse());

            void *addr2 = mX.allocate(1);
            if (EXPOFFSET >= 0) {
                ASSERT(used == objectAllocator.numBytesInUse());
                LOOP4_ASSERT(LINE, EXPOFFSET, addr1, addr2,
                             (char *)addr1 + EXPOFFSET == (char *)addr2);
            }
            else {
                ASSERT(used < objectAllocator.numBytesInUse());
            }

            // Truncating previously allocated address should fail.
            if (EXPOFFSET >= 0
             && ((char *)addr1 + INITIALSIZE) != ((char *)addr2 + 1)) {
                int ret = mX.truncate(addr1, INITIALSIZE, NEWSIZE);
                LOOP2_ASSERT(INITIALSIZE, ret, INITIALSIZE == ret);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'0 != address'" << endl;
            {
                Obj mX;

                void *addr = mX.allocate(2);

                ASSERT_SAFE_PASS_RAW(mX.truncate(addr, 2, 1));

                ASSERT_SAFE_FAIL_RAW(mX.truncate(   0, 1, 0));
            }

            if (veryVerbose) cout << "\t'0 <= newSize'" << endl;
            {
                Obj mX;

                void *addr = mX.allocate(2);

                ASSERT_SAFE_PASS_RAW(mX.truncate(addr, 2,  1));
                ASSERT_SAFE_PASS_RAW(mX.truncate(addr, 1,  0));

                ASSERT_SAFE_FAIL_RAW(mX.truncate(addr, 0, -1));
            }

            if (veryVerbose) cout << "\t'newSize <= originalSize'" << endl;
            {
                Obj mX;

                void *addr = mX.allocate(2);

                ASSERT_SAFE_PASS_RAW(mX.truncate(addr, 2, 2));

                ASSERT_SAFE_FAIL_RAW(mX.truncate(addr, 2, 3));
            }
        }

#undef NAT
#undef MAX
#undef BYT

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
        //   4) QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   For concerns 1-3, using the table-driven technique, create test
        //   vectors having the alignment strategy, initial memory offset, and
        //   expected memory used.  First allocate memory necessary for the
        //   initial memory offset, then allocate 1 byte using
        //   'allocateAndExpand'.  Verify the updated size is the same as the
        //   expected memory used.  Finally, invoke 'allocate' again and verify
        //   it triggers new dynamic memory allocation -- meaning
        //   'allocateAndExpand' did use up all available memory in the
        //   internal buffer.
        //
        //   For concern 4, verify that, in appropriate build modes, defensive
        //   checks are triggered.
        //
        // Testing:
        //   void *allocateAndExpand(size_type *size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'allocateAndExpand' TEST" << endl
                                  << "========================" << endl;

#define NAT bsls::Alignment::BSLS_NATURAL
#define MAX bsls::Alignment::BSLS_MAXIMUM
#define BYT bsls::Alignment::BSLS_BYTEALIGNED

        if (verbose) cout << "\nTesting 'expand'." << endl;

        static const struct {
            int   d_line;         // line number
            Strat d_strategy;     // alignment strategy
            int   d_initialSize;  // size of initial allocation request
            int   d_expused;      // expected memory used after 'expand'
        } DATA[] = {
            // LINE     STRAT    INITIALSIZE   EXPUSED
            // ----     -----    -----------   -------

            // NATURAL ALIGNMENT
            {  L_,      NAT,           1,   k_DEFAULT_SIZE * 2 - 1           },
            {  L_,      NAT,           2,   k_DEFAULT_SIZE * 2 - 2           },
            {  L_,      NAT,           3,   k_DEFAULT_SIZE * 2 - 3           },
            {  L_,      NAT,           4,   k_DEFAULT_SIZE * 2 - 4           },
            {  L_,      NAT,           7,   k_DEFAULT_SIZE * 2 - 7           },
            {  L_,      NAT,           8,   k_DEFAULT_SIZE * 2 - 8           },
            {  L_,      NAT,          15,   k_DEFAULT_SIZE * 2 - 15          },
            {  L_,      NAT,          16,   k_DEFAULT_SIZE * 2 - 16          },
            {  L_,      NAT,         100,   k_DEFAULT_SIZE * 2 - 100         },
            {  L_,      NAT,         510,   k_DEFAULT_SIZE * 2 - 510         },
            {  L_,      NAT,         511,   k_DEFAULT_SIZE * 2 - 511         },

            // MAXIMUM ALIGNMENT
            {  L_,      MAX,           1,   k_DEFAULT_SIZE * 2 - k_MAX_ALIGN },
            {  L_,      MAX,           2,   k_DEFAULT_SIZE * 2 - k_MAX_ALIGN },
            {  L_,      MAX,           3,   k_DEFAULT_SIZE * 2 - k_MAX_ALIGN },
            {  L_,      MAX,           4,   k_DEFAULT_SIZE * 2 - k_MAX_ALIGN },
            {  L_,      MAX,           7,   k_DEFAULT_SIZE * 2 - k_MAX_ALIGN },
            {  L_,      MAX,           8,   k_DEFAULT_SIZE * 2 - k_MAX_ALIGN },
            {  L_,      MAX,          15,   k_DEFAULT_SIZE * 2 - 16          },
            {  L_,      MAX,          16,   k_DEFAULT_SIZE * 2 - 16          },
            {  L_,      MAX,         108,   k_DEFAULT_SIZE * 2 - 112         },

            // 1-BYTE ALIGNMENT
            {  L_,      BYT,           1,   k_DEFAULT_SIZE * 2 - 1           },
            {  L_,      BYT,           2,   k_DEFAULT_SIZE * 2 - 2           },
            {  L_,      BYT,           3,   k_DEFAULT_SIZE * 2 - 3           },
            {  L_,      BYT,           4,   k_DEFAULT_SIZE * 2 - 4           },
            {  L_,      BYT,           7,   k_DEFAULT_SIZE * 2 - 7           },
            {  L_,      BYT,           8,   k_DEFAULT_SIZE * 2 - 8           },
            {  L_,      BYT,          15,   k_DEFAULT_SIZE * 2 - 15          },
            {  L_,      BYT,          16,   k_DEFAULT_SIZE * 2 - 16          },
            {  L_,      BYT,         100,   k_DEFAULT_SIZE * 2 - 100         },
            {  L_,      BYT,         510,   k_DEFAULT_SIZE * 2 - 510         },
            {  L_,      BYT,         511,   k_DEFAULT_SIZE * 2 - 511         },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE        = DATA[ti].d_line;
            const Strat STRAT       = DATA[ti].d_strategy;
            const int   INITIALSIZE = DATA[ti].d_initialSize;
            const int   EXPUSED     = DATA[ti].d_expused;

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL ALIGNMENT, ";
                }
                else if (STRAT == MAX) {
                    cout << "STRAT = MAXIMUM ALIGNMENT, ";
                }
                else {  // STRAT == BYTE
                    cout << "STRAT = 1-BYTE ALIGNMENT, ";
                }
                P_(INITIALSIZE) P(EXPUSED)
            }

            Obj mX(STRAT, &objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());
            ASSERT(0 == objectAllocator.numBlocksInUse());

            void *addr1 = mX.allocate(INITIALSIZE);
            bsls::Types::Int64 numBytesUsed = objectAllocator.numBytesInUse();
            ASSERT(1 == objectAllocator.numBlocksInUse());

            bsls::Types::size_type size = 1;
            void *addr2 = mX.allocateAndExpand(&size);

            // Check for correct memory address.
            if (NAT == STRAT || BYT == STRAT) {
                ASSERT((char *)addr1 + INITIALSIZE == (char *)addr2);
            }
            else {
                int offset = bsls::AlignmentUtil::calculateAlignmentOffset(
                                                   (char *)addr1 + INITIALSIZE,
                                                   k_MAX_ALIGN);
                ASSERT((char *)addr1 + INITIALSIZE + offset == (char *)addr2);
            }

            // Check 'size' is updated correctly.
            ASSERT(EXPUSED == (int)size);

            // Check for no new allocations.
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());
            ASSERT(1 == objectAllocator.numBlocksInUse());

            // Check for new allocations.
            mX.allocate(1);
            ASSERT(2 == objectAllocator.numBlocksInUse());
        }
        ASSERT(0 == objectAllocator.numBytesInUse());

        {
            Obj mX(&objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());
            ASSERT(0 == objectAllocator.numBlocksInUse());

            // No effect when 0 is passed in.
            bsls::Types::size_type size = 0;
            mX.allocateAndExpand(&size);
            ASSERT(0 == objectAllocator.numBlocksInUse());

            // No initial allocation, just allocate and expand directly.
            size = 1;
            mX.allocateAndExpand(&size);
            ASSERT(1 == objectAllocator.numBlocksInUse());

            mX.allocate(1);
            ASSERT(2 == objectAllocator.numBlocksInUse());
        }
        ASSERT(0 == objectAllocator.numBytesInUse());

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'0 != size'" << endl;
            {
                Obj mX;

                bsls::Types::size_type size = 1;

                ASSERT_SAFE_PASS(mX.allocateAndExpand(&size));

                ASSERT_SAFE_FAIL(mX.allocateAndExpand(    0));
            }
        }

#undef NAT
#undef MAX
#undef BYT

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'release' TEST
        //
        // Concerns:
        //   1) That all memory allocated from the allocator supplied at
        //      construction is deallocated after 'release'.
        //
        //   2) That subsequent allocation requests after invocation of the
        //      'release' method follow the specified growth and alignment
        //      strategies.
        //
        // Plan:
        //   For concerns 1 and 2, construct a sequential allocator using a
        //   'bslma::TestAllocator', and allocate several memory blocks such
        //   that there are multiple dynamic allocations.  Then invoke
        //   'release' and verify, using the test allocator, that there
        //   is no outstanding memory allocated.  Finally, allocate memory
        //   again and verify the alignment and growth strategies.
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'release' TEST" << endl
                                  << "==============" << endl;

#define CON bsls::BlockGrowth::BSLS_CONSTANT
#define MAX bsls::Alignment::BSLS_MAXIMUM

        if (verbose) cout << "\nTesting allocated memory is deallocated after"
                             " 'release'." << endl;
        {
            Obj mX(CON, MAX, &objectAllocator);

            ASSERT(0 == objectAllocator.numBlocksInUse());

            // Allocate some memory such that memory is allocated from the test
            // allocator.
            mX.allocate(1000);
            ASSERT(1 == objectAllocator.numBlocksInUse());

            mX.allocate(1000);
            ASSERT(2 == objectAllocator.numBlocksInUse());

            mX.allocate(1000);
            ASSERT(3 == objectAllocator.numBlocksInUse());

            // Release all memory.
            mX.release();

            ASSERT(0 == objectAllocator.numBlocksInUse());

            if (verbose) cout << "\nTesting alignment and growth strategies."
                              << endl;

            // Testing alignment.
            void *addr1 = mX.allocate(1);
            void *addr2 = mX.allocate(2);

            ASSERT((char *)addr1 + k_MAX_ALIGN == (char *)addr2);

            // Testing growth strategy.
            bsls::Types::Int64 numBytesUsed = objectAllocator.numBytesInUse();
            mX.allocate(k_DEFAULT_SIZE / 2);
            ASSERT(objectAllocator.numBytesInUse() == numBytesUsed);

            // Because of alignment concerns, we just test that the number of
            // bytes used is within a small range.  If the memory growth is
            // outside of this range, the allocator is using geometric growth.

            mX.allocate(k_DEFAULT_SIZE / 2);
            ASSERT(objectAllocator.numBytesInUse()
                                            >= numBytesUsed + k_DEFAULT_SIZE);
            ASSERT(objectAllocator.numBytesInUse() <=
                                  numBytesUsed + k_DEFAULT_SIZE + k_MAX_ALIGN);
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
        //   Request memory of varying sizes and then deallocate each memory
        //   block.  Verify that the number of bytes in use indicated by the
        //   test allocator does not decrease after each 'deallocate' method
        //   invocation.
        //
        // Testing:
        //   void deallocate(void *address);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'deallocate' TEST" << endl
                                  << "=================" << endl;

        if (verbose) cout << "\nTesting 'deallocate'." << endl;

        const int DATA[] = { 0, 1, 5, 12, 24, 32, 64, 256, 257, 512, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        Obj mX(&objectAllocator);

        bsls::Types::Int64 lastNumBytesInUse = objectAllocator.numBytesInUse();

        for (int i = 0; i < NUM_DATA; ++i) {
            const int SIZE = DATA[i];
            void *p = mX.allocate(SIZE);
            const bsls::Types::Int64 numBytesInUse =
                                               objectAllocator.numBytesInUse();
            mX.deallocate(p);
            LOOP_ASSERT(i, numBytesInUse == objectAllocator.numBytesInUse());
            LOOP_ASSERT(i, lastNumBytesInUse <=
                                              objectAllocator.numBytesInUse());
            lastNumBytesInUse = objectAllocator.numBytesInUse();
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CTOR / ALLOCATE TEST
        //
        // Concerns:
        //   1) That the 'bdlma::SequentialAllocator' correctly proxies its
        //      constructor arguments and allocation requests to the
        //      'bdlma::SequentialPool' it adapts.
        //
        //   2) That allocating 0 bytes returns 0.
        //
        //   3) QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   Using the array driven approach, create an array of various memory
        //   allocation request sizes.  Then, create both a sequential
        //   allocator and a sequential pool using two different test
        //   allocators.  Finally, verify that the memory usage of both test
        //   allocators is the same.
        //
        //   In addition, verify that, in appropriate build modes, defensive
        //   checks are triggered.
        //
        // Testing:
        //   bdlma::SequentialAllocator(Alloc *a = 0);
        //   bdlma::SequentialAllocator(GS g, Alloc *a = 0);
        //   bdlma::SequentialAllocator(AS a, Alloc *a = 0);
        //   bdlma::SequentialAllocator(GS g, AS a, Alloc *a = 0);
        //
        //   bdlma::SequentialAllocator(int i, Alloc *a = 0);
        //   bdlma::SequentialAllocator(int i, GS g, Alloc *a = 0);
        //   bdlma::SequentialAllocator(int i, AS a, Alloc *a = 0);
        //   bdlma::SequentialAllocator(int i, GS g, AS a, Alloc *a = 0);
        //
        //   bdlma::SequentialAllocator(int i, int m, Alloc *a = 0);
        //   bdlma::SequentialAllocator(int i, int m, GS g, Alloc *a = 0);
        //   bdlma::SequentialAllocator(int i, int m, AS a, Alloc *a = 0);
        //   bdlma::SequentialAllocator(int i, int m, GS g, AS a, Alloc *a= 0);
        //
        //   void *allocate(size_type size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CTOR / ALLOCATE TEST" << endl
                                  << "====================" << endl;

        enum { k_INITIAL_SIZE = 64, k_MAX_BUFFER = 256 };

        const int DATA[]   = { 2, 5, 7, 8, 15, 16, 24, 31, 32, 33, 48,
                               63, 64, 65, 66, 127, 128, 129, 255, 256,
                               511, 512, 1023, 1024, 1025 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

#define GEO bsls::BlockGrowth::BSLS_GEOMETRIC
#define CON bsls::BlockGrowth::BSLS_CONSTANT
#define NAT bsls::Alignment::BSLS_NATURAL
#define MAX bsls::Alignment::BSLS_MAXIMUM
#define BYT bsls::Alignment::BSLS_BYTEALIGNED

        // block growth strategy
        const bsls::BlockGrowth::Strategy GS[2] = { GEO, CON };
        const int NUM_GS = sizeof GS / sizeof *GS;

        // alignment strategy
        const Strat AS[3] = { NAT, MAX, BYT };
        const int NUM_AS = sizeof AS / sizeof *AS;

        if (verbose) cout << "\nTesting 'Obj(Alloc *a = 0)'." << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                Obj                   mX(&ta);
                bdlma::SequentialPool pool(&tb);

                void *addr1 = mX.allocate(SIZE);
                void *addr2 = pool.allocate(SIZE);

                ASSERT(0 != addr1);
                ASSERT(0 != addr2);

                ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
            }
        }

        if (verbose) cout << "\nTesting 'Obj(GS g, Alloc *a = 0)'." << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_GS; ++j) {
                    Obj                   mX(GS[j], &ta);
                    bdlma::SequentialPool pool(GS[j], &tb);

                    void *addr1 = mX.allocate(SIZE);
                    void *addr2 = pool.allocate(SIZE);

                    ASSERT(0 != addr1);
                    ASSERT(0 != addr2);

                    ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
                }
            }
        }

        if (verbose) cout << "\nTesting 'Obj(AS a, Alloc *a = 0)'." << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {
                    Obj                   mX(AS[j], &ta);
                    bdlma::SequentialPool pool(AS[j], &tb);

                    void *addr1 = mX.allocate(SIZE);
                    void *addr2 = pool.allocate(SIZE);

                    ASSERT(0 != addr1);
                    ASSERT(0 != addr2);

                    ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
                    bsls::Types::Int64 numBytesUsed = tb.numBytesInUse();

                    void *addr1b = mX.allocate(2);
                    void *addr2b = pool.allocate(2);

                    // Verify that the alignment strategy between the pool and
                    // the allocator is the same by adding the alignment offset
                    // of the pool to the allocator.  The alignment offset is
                    // only sensible if memory comes from the same internal
                    // buffer.

                    if (numBytesUsed == tb.numBytesInUse()) {
                        bsl::size_t alignmentOffset =
                                                (char *)addr2b - (char *)addr2;
                        ASSERT((char *)addr1 + alignmentOffset
                                                            == (char *)addr1b);
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting 'Obj(GS g, AS a, Alloc *a = 0)'."
                          << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {      // alignment strategy

                    for (int k = 0; k < NUM_GS; ++k) {  // growth strategy

                        Obj                   mX(GS[k], AS[j], &ta);
                        bdlma::SequentialPool pool(GS[k], AS[j], &tb);

                        void *addr1 = mX.allocate(SIZE);
                        void *addr2 = pool.allocate(SIZE);

                        ASSERT(0 != addr1);
                        ASSERT(0 != addr2);

                        ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
                        bsls::Types::Int64 numBytesUsed = tb.numBytesInUse();

                        void *addr1b = mX.allocate(2);
                        void *addr2b = pool.allocate(2);

                        if (numBytesUsed == tb.numBytesInUse()
                         && GS[k] != CON) {
                            bsl::size_t alignmentOffset =
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
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                Obj                   mX(k_INITIAL_SIZE, &ta);
                bdlma::SequentialPool pool(k_INITIAL_SIZE, &tb);

                void *addr1 = mX.allocate(SIZE);
                void *addr2 = pool.allocate(SIZE);

                ASSERT(0 != addr1);
                ASSERT(0 != addr2);

                ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
            }
        }

        if (verbose) cout << "\nTesting 'Obj(int i, GS g, Alloc *a = 0)'."
                          << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_GS; ++j) {
                    Obj                   mX(k_INITIAL_SIZE, GS[j], &ta);
                    bdlma::SequentialPool pool(k_INITIAL_SIZE, GS[j], &tb);

                    void *addr1 = mX.allocate(SIZE);
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
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {
                    Obj                   mX(k_INITIAL_SIZE, AS[j], &ta);
                    bdlma::SequentialPool pool(k_INITIAL_SIZE, AS[j], &tb);

                    void *addr1 = mX.allocate(SIZE);
                    void *addr2 = pool.allocate(SIZE);

                    ASSERT(0 != addr1);
                    ASSERT(0 != addr2);

                    ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
                    bsls::Types::Int64 numBytesUsed = tb.numBytesInUse();

                    void *addr1b = mX.allocate(2);
                    void *addr2b = pool.allocate(2);

                    // Verify that the alignment strategy between the pool and
                    // the allocator is the same by adding the alignment offset
                    // of the pool to the allocator.  The alignment offset is
                    // only sensible if memory comes from the same internal
                    // buffer.

                    if (numBytesUsed == tb.numBytesInUse()) {
                        bsl::size_t alignmentOffset =
                                                (char *)addr2b - (char *)addr2;
                        ASSERT((char *)addr1 + alignmentOffset
                                                            == (char *)addr1b);
                    }
                }
            }
        }

        if (verbose) cout <<"\nTesting 'Obj(int i, GS g, AS a, Alloc *a = 0)'."
                          << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {      // alignment strategy

                    if (veryVerbose) { T_ T_ P(j) }

                    for (int k = 0; k < NUM_GS; ++k) {  // growth strategy

                        if (veryVerbose) { T_ T_ T_ P(k) }

                        Obj mX(k_INITIAL_SIZE, GS[k], AS[j], &ta);

                        bdlma::SequentialPool pool(k_INITIAL_SIZE,
                                                   GS[k],
                                                   AS[j],
                                                   &tb);

                        void *addr1 = mX.allocate(SIZE);
                        void *addr2 = pool.allocate(SIZE);

                        ASSERT(0 != addr1);
                        ASSERT(0 != addr2);

                        ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
                        bsls::Types::Int64 numBytesUsed = tb.numBytesInUse();

                        void *addr1b = mX.allocate(2);
                        void *addr2b = pool.allocate(2);

                        if (numBytesUsed == tb.numBytesInUse()
                         && GS[k] != CON) {
                            bsl::size_t alignmentOffset =
                                                (char *)addr2b - (char *)addr2;

                            LOOP3_ASSERT(addr1, addr1b, alignmentOffset,
                            (char *)addr1 + alignmentOffset == (char *)addr1b);
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting 'Obj(int i, int m, Alloc *a = 0)'."
                          << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                Obj                   mX(k_INITIAL_SIZE, k_MAX_BUFFER, &ta);
                bdlma::SequentialPool pool(k_INITIAL_SIZE, k_MAX_BUFFER, &tb);

                void *addr1 = mX.allocate(SIZE);
                void *addr2 = pool.allocate(SIZE);

                ASSERT(0 != addr1);
                ASSERT(0 != addr2);

                ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
            }
        }

        if (verbose) cout <<
            "\nTesting 'Obj(int i, int m, GS g, Alloc *a = 0)'." << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_GS; ++j) {
                    Obj mX(k_INITIAL_SIZE, k_MAX_BUFFER, GS[j], &ta);

                    bdlma::SequentialPool pool(k_INITIAL_SIZE,
                                               k_MAX_BUFFER,
                                               GS[j],
                                               &tb);

                    void *addr1 = mX.allocate(SIZE);
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
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {
                    Obj mX(k_INITIAL_SIZE, k_MAX_BUFFER, AS[j], &ta);

                    bdlma::SequentialPool pool(k_INITIAL_SIZE,
                                               k_MAX_BUFFER,
                                               AS[j],
                                               &tb);

                    void *addr1 = mX.allocate(SIZE);
                    void *addr2 = pool.allocate(SIZE);

                    ASSERT(0 != addr1);
                    ASSERT(0 != addr2);

                    ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
                    bsls::Types::Int64 numBytesUsed = tb.numBytesInUse();

                    void *addr1b = mX.allocate(2);
                    void *addr2b = pool.allocate(2);

                    // Verify that the alignment strategy between the pool and
                    // the allocator is the same by adding the alignment offset
                    // of the pool to the allocator.  The alignment offset is
                    // only sensible if memory comes from the same internal
                    // buffer.

                    if (numBytesUsed == tb.numBytesInUse()
                     && SIZE < k_MAX_BUFFER) {
                        bsl::size_t alignmentOffset =
                                                (char *)addr2b - (char *)addr2;
                        ASSERT((char *)addr1 + alignmentOffset
                                                            == (char *)addr1b);
                    }
                }
            }
        }

        if (verbose) cout <<
            "\nTesting 'Obj(int i, int m, GS g, AS a, Alloc *a = 0)'." << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tb(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {      // alignment strategy

                    for (int k = 0; k < NUM_GS; ++k) {  // growth strategy

                        Obj mX(k_INITIAL_SIZE,
                               k_MAX_BUFFER,
                               GS[k],
                               AS[j],
                               &ta);

                        bdlma::SequentialPool pool(k_INITIAL_SIZE,
                                                   k_MAX_BUFFER,
                                                   GS[k],
                                                   AS[j],
                                                   &tb);

                        void *addr1 = mX.allocate(SIZE);
                        void *addr2 = pool.allocate(SIZE);

                        ASSERT(0 != addr1);
                        ASSERT(0 != addr2);

                        ASSERT(tb.numBytesInUse() == ta.numBytesInUse());
                        bsls::Types::Int64 numBytesUsed = tb.numBytesInUse();

                        void *addr1b = mX.allocate(2);
                        void *addr2b = pool.allocate(2);

                        if (numBytesUsed == tb.numBytesInUse()
                         && GS[k] != CON && SIZE < k_MAX_BUFFER) {
                            bsl::size_t alignmentOffset =
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
            Obj mX(&objectAllocator);
            void *addr = mX.allocate(0);
            ASSERT(0 == addr);
            ASSERT(0 == objectAllocator.numBytesInUse());
        }
        ASSERT(0 == objectAllocator.numBlocksTotal());
        ASSERT(0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'Obj(i, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj( 1));

                ASSERT_SAFE_FAIL_RAW(Obj( 0));
                ASSERT_SAFE_FAIL_RAW(Obj(-1));
            }

            if (veryVerbose) cout << "\t'Obj(i, GS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj( 1, CON));

                ASSERT_SAFE_FAIL_RAW(Obj( 0, CON));
                ASSERT_SAFE_FAIL_RAW(Obj(-1, CON));
            }

            if (veryVerbose) cout << "\t'Obj(i, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj( 1, MAX));

                ASSERT_SAFE_FAIL_RAW(Obj( 0, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(-1, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(i, GS, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj( 1, CON, MAX));

                ASSERT_SAFE_FAIL_RAW(Obj( 0, CON, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(-1, CON, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(i, m, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj( 1,  8));

                ASSERT_SAFE_FAIL_RAW(Obj( 0,  8));
                ASSERT_SAFE_FAIL_RAW(Obj(-1,  8));

                ASSERT_SAFE_PASS_RAW(Obj( 2,  2));

                ASSERT_SAFE_FAIL_RAW(Obj( 2,  1));
                ASSERT_SAFE_FAIL_RAW(Obj( 2, -2));
            }

            if (veryVerbose) cout << "\t'Obj(i, m, GS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj( 1,  8, CON));

                ASSERT_SAFE_FAIL_RAW(Obj( 0,  8, CON));
                ASSERT_SAFE_FAIL_RAW(Obj(-1,  8, CON));

                ASSERT_SAFE_PASS_RAW(Obj( 2,  2, CON));

                ASSERT_SAFE_FAIL_RAW(Obj( 2,  1, CON));
                ASSERT_SAFE_FAIL_RAW(Obj( 2, -2, CON));
            }

            if (veryVerbose) cout << "\t'Obj(i, m, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj( 1,  8, MAX));

                ASSERT_SAFE_FAIL_RAW(Obj( 0,  8, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(-1,  8, MAX));

                ASSERT_SAFE_PASS_RAW(Obj( 2,  2, MAX));

                ASSERT_SAFE_FAIL_RAW(Obj( 2,  1, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj( 2, -2, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(i, m, GS, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj( 1,  8, CON, MAX));

                ASSERT_SAFE_FAIL_RAW(Obj( 0,  8, CON, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(-1,  8, CON, MAX));

                ASSERT_SAFE_PASS_RAW(Obj( 2,  2, CON, MAX));

                ASSERT_SAFE_FAIL_RAW(Obj( 2,  1, CON, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj( 2, -2, CON, MAX));
            }
        }

#undef GEO
#undef CON
#undef NAT
#undef MAX
#undef BYT

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1) That a 'bdlma::SequentialAllocator' can be created and
        //      destroyed.
        //
        //   2) That 'allocate' returns a block of memory having the specified
        //      size and expected alignment.
        //
        //   3) That 'allocate' does not always cause dynamic allocation
        //      (i.e., the allocator manages an internal buffer of memory).
        //
        //   4) That 'allocate' returns a block of memory even when the
        //      the allocation request exceeds the initial size of the internal
        //      buffer.
        //
        //   5) Destruction of the allocator releases all managed memory.
        //
        // Plan:
        //   First, initialize a 'bdlma::SequentialAllocator' with a
        //   'bslma::TestAllocator' (concern 1).  Then, allocate a block of
        //   memory, and verify that it comes from the test allocator.
        //   Allocate another block of memory, and verify that no dynamic
        //   allocation is triggered (concern 3).  Verify the alignment and
        //   size of the first allocation by checking the address of the
        //   second allocation (concern 2).
        //
        //   Then, allocate a large block of memory and verify the memory comes
        //   from the test allocator (concern 4).  Finally, destroy the
        //   allocator and check that all allocated memory is deallocated
        //   (concern 5).
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        enum { k_ALLOC_SIZE1 = 4, k_ALLOC_SIZE2 = 8, k_ALLOC_SIZE3 = 1024 };

        {
            if (verbose) cout << "\nTesting construction of allocator."
                              << endl;
            Obj mX(&objectAllocator);

            // Make sure no memory comes from the object, default, and global
            // allocators.
            ASSERT(0 == objectAllocator.numBlocksTotal());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            if (verbose) cout << "\nTesting allocation." << endl;
            void *addr1 = mX.allocate(k_ALLOC_SIZE1);

            ASSERT(0 != objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            bsls::Types::Int64 oldNumBytesInUse =
                                               objectAllocator.numBytesInUse();

            if (verbose) cout << "\nTesting internal buffering." << endl;
            void *addr2 = mX.allocate(k_ALLOC_SIZE2);

            ASSERT(oldNumBytesInUse == objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            if (verbose) cout << "\nTesting alignment strategy." << endl;

            // Check for alignment and size of first allocation.
            ASSERT((char *)addr1 + 8 == (char *)addr2);

            if (verbose) cout << "\nTesting large allocation." << endl;
            void *addr3 = mX.allocate(k_ALLOC_SIZE3);
            (void)addr3;

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
