// bdlma_smallsequentialallocator.t.cpp                               -*-C++-*-
#include <bdlma_smallsequentialallocator.h>

#include <bdlma_managedallocator.h>
#include <bdlma_sequentialallocator.h>
#include <bdlma_smallsequentialpool.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_alignmentutil.h>
#include <bsls_asserttest.h>
#include <bsls_keyword.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>  // 'bsl::size_t'
#include <bsl_cstdlib.h>  // 'bsl::atoi'
#include <bsl_iostream.h>

#undef GS  // Solaris 2.10 x86 /usr/include/sys/regset.h

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// A 'bdlma::SmallSequentialAllocator' adapts the 'bdlma::SmallSequentialPool'
// mechanism to the 'bdlma::ManagedAllocator' protocol.  The primary concern is
// that the allocator correctly forwards the memory allocation requests to the
// sequential pool it adapts.
//
// To test that requests are correctly proxied, we create
// 'bsl::SmallSequentialAllocator' and 'bsl::SmallSequentalPool' objects each
// using a different different test allocator.  We then verify that either:
//
//: 1 The addresses returned by the allocator and the pool are the same (i.e.,
//:   the memory is allocated from the buffer).
//:
//: 2 If the memory used by either test allocator is non-zero, the number of
//:   bytes used by both test allocators is the same.
//
// We also need to verify that the 'deallocate' method has no effect.  Again,
// we make use of the test allocator to ensure that no memory is deallocated
// when the 'deallocate' method of a sequential allocator is invoked on
// previously allocated memory.
//
// Finally, the destructor of 'bdlma::SmallSequentialAllocator' is tested
// throughout the test driver.  At destruction, the allocator should reclaim
// all outstanding allocated memory.  By setting the global allocator, default
// allocator, and object allocator to different test allocators, we can
// determine whether all memory had been released by the destructor of the
// sequential allocator.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] SmallSequentialAllocator(Alloc *a = 0);
// [ 2] SmallSequentialAllocator(GS g, Alloc *a = 0);
// [ 2] SmallSequentialAllocator(AS a, Alloc *a = 0);
// [ 2] SmallSequentialAllocator(GS g, AS a, Alloc *a = 0);
//
// [ 2] SmallSequentialAllocator(int initSize);
// [ 2] SmallSequentialAllocator(int i, Alloc *a = 0);
// [ 2] SmallSequentialAllocator(int i, GS g, Alloc *a = 0);
// [ 2] SmallSequentialAllocator(int i, AS a, Alloc *a = 0);
// [ 2] SmallSequentialAllocator(int i, GS g, AS a, Alloc *a = 0);
//
// [ 2] SmallSequentialAllocator(int i, int m, Alloc *a = 0);
// [ 2] SmallSequentialAllocator(int i, int m, GS g, Alloc *a = 0);
// [ 2] SmallSequentialAllocator(int i, int m, AS a, Alloc *a = 0);
// [ 2] SmallSequentialAllocator(int i, int m, GS g, AS a, *a = 0);
//
// [ 2] ~SmallSequentialAllocator();
//
// MANIPULATORS
// [ 2] void *allocate(bsl::size_t size);
// [ 5] void *allocateAndExpand(bsl::size_t *size);
// [ 3] void deallocate(void *address);
// [ 4] void release();
// [ 4] void rewind();
// [ 7] void reserveCapacity(int numBytes);
// [ 6] int truncate(void *address, int originalSize, int newSize);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 9] USAGE EXAMPLE
// [ 8] CONCERN: The footprint is smaller than 'SequentialAllocator'.
// [ 8] CONCERN: The class inherits from 'bdlma::MangageAllocator'.

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

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

// ============================================================================
//                  GLOBAL VARIABLES / TYPEDEF FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlma::SmallSequentialAllocator Obj;
typedef bdlma::SmallSequentialPool      Pool;

typedef bsls::Alignment::Strategy       Strat;

enum { k_MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT };

enum { k_DEFAULT_SIZE = 256 };

// ============================================================================
//                                HELPER FUNCTIONS
// ----------------------------------------------------------------------------

void mirrorAllocatorAndPool(Obj                         *alloc,
                            Pool                        *pool,
                            const bslma::TestAllocator&  allocAllocator,
                            const bslma::TestAllocator&  poolAllocator,
                            bsl::size_t                  size,
                            bool                         isLargeAllocation)
    // Perform two identical allocations from the specified 'alloc' and 'pool'.
    // The first of the specified 'size' and the second of 2 bytes (the
    // smallest size for which alignment can be relevant).  If the specified
    // 'allocAllocator', 'poolAllocator', and 'isLargeAllocation' flag indicate
    // that the two allocator allocations and the two pool allocations were
    // serviced out of the allocator buffer, and the same pool buffer,
    // respectively, compare the relative offset between the two pairs of
    // allocations.  Otherwise, check that when the pool acquires a new buffer,
    // so does the allocator.  Deviations from the expected results are
    // reported as test failures.
{
    ASSERT(alloc);
    ASSERT(pool);

    char *addr1a = static_cast<char *>(alloc->allocate(size));
    char *addr1p = static_cast<char *>(pool ->allocate(size));

    ASSERT(addr1a);
    ASSERT(addr1p);

    ASSERT(poolAllocator.numBlocksInUse() == allocAllocator.numBlocksInUse());
    ASSERT(poolAllocator.numBytesInUse()  == allocAllocator.numBytesInUse());

    const bsls::Types::Int64 numBytesInUse = poolAllocator.numBytesInUse();

    bslma::TestAllocatorMonitor tama(&allocAllocator);
    bslma::TestAllocatorMonitor tamp(& poolAllocator);

    char *addr2a = static_cast<char *>(alloc->allocate(2));
    char *addr2p = static_cast<char *>(pool ->allocate(2));

    ASSERT(addr2a);
    ASSERT(addr2p);

    if (numBytesInUse == poolAllocator.numBytesInUse()) {
        ASSERT(tama.isInUseSame());
        ASSERT(tamp.isInUseSame());
        if (!isLargeAllocation) {
            ASSERT(addr2a - addr1a ==  addr2p - addr1p);
        }
    } else {
        ASSERT(1 == tama.numBlocksInUseChange());
        ASSERT(1 == tamp.numBlocksInUseChange());
    }
}

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

// BDE_VERIFY pragma: -NT01  // End of namespace should be marked with ...
// BDE_VERIFY pragma: -IND01 // Possibly mis-indented line
namespace UsageExample1 {

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using a 'bdlma::SmallSequentialAllocator'
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Allocators are often supplied, at construction, to objects requiring
// dynamically-allocated memory.  For example, consider the following
// 'xyza::DoubleStack' class whose constructor takes a 'bslma::Allocator *'.
//
// The header file (elided) is:
//..
    // xyaz_doublestack.h

    // ...

    namespace xyza {
                            // =================
                            // class DoubleStack
                            // =================

    class DoubleStack {
        // This class implements a stack that stores 'double' values.

        // DATA
        double           *d_stack_p;      // dynamically-allocated array
        int               d_capacity;     // physical capacity of stack
        int               d_length;       // next available index in stack
        bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)

      private:
        // PRIVATE MANIPULATORS
        void increaseCapacity();
            // Increase the capacity of this stack by at least one element.

        // NOT IMPLEMENTED
        DoubleStack(const DoubleStack&)            BSLS_KEYWORD_DELETED;
        DoubleStack& operator=(const DoubleStack&) BSLS_KEYWORD_DELETED;

      public:
        // CREATORS
        explicit DoubleStack(bslma::Allocator *basicAllocator = 0);
            // Create an empty stack that stores 'double' values.  Optionally
            // specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.

        ~DoubleStack();
            // Destroy this stack and all elements held by it.

        // ...

        // MANIPULATORS
        void push(double value);
            // Push the specified 'value' onto this stack.

        // ...
    };

    // ========================================================================
    //                             INLINE DEFINITIONS
    // ========================================================================

                            // -----------------
                            // class DoubleStack
                            // -----------------

    // MANIPULATORS
    inline
    void DoubleStack::push(double value)
    {
        if (d_capacity <= d_length) {
            increaseCapacity();
        }
        d_stack_p[d_length++] = value;
    }

    // ...

    }  // close package namespace
//..
// The implementation file (elided) is:
//..
    // my_doublestack.cpp

    namespace xyza {

                            // -----------------
                            // class DoubleStack
                            // -----------------

    // PRIVATE MANIPULATORS
    void DoubleStack::increaseCapacity()
    {
        // Implementation elided.
        // ...
    }

    // CREATORS
    DoubleStack::DoubleStack(bslma::Allocator *basicAllocator)
    : d_capacity(1)
    , d_length(0)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_stack_p = static_cast<double *>(
                      d_allocator_p->allocate(d_capacity * sizeof *d_stack_p));
    }

    // ...
//..
// Note that this implementation set the initial capacity to 1 and
// pre-allocates space for one data value in the constructor.
//..
    DoubleStack::~DoubleStack()
    {
        // CLASS INVARIANTS
        ASSERT(d_allocator_p);
        ASSERT(d_stack_p);
        ASSERT(0        <= d_length);
        ASSERT(d_length <= d_capacity);

        d_allocator_p->deallocate(d_stack_p);
    }
// Note that, when the allocator passed in is a
// 'bdlma::SmallSequentialAllocator', the 'deallocate' method is a no-op, and
// the memory is not actually reclaimed until the destruction of the allocator
// object.
//..

    // ...

    }  // close package namespace
//..
// In 'main', users can create a 'bdlma::SmallSequentialAllocator' and pass it
// to the constructor of 'xyza::DoubleStack':
//..
    int main()
        // Run a program that uses a 'bdlma::SmallSequentialAllocator' object.
    {
        bdlma::SmallSequentialAllocator smallSequentialAlloc;
        xyza::DoubleStack               dstack(&smallSequentialAlloc);

        // ....

        return 0;
    }
//..

}  // close namespace UsageExample1
// BDE_VERIFY pragma: +NT01  // End of namespace should be marked with ...
// BDE_VERIFY pragma: +IND01 // Possibly mis-indented line

// ============================================================================
//                                MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;  (void) veryVeryVerbose;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // As part of our overall allocator testing strategy, we will create three
    // test allocators.

    // Object Test Allocator.
    bslma::TestAllocator objectAllocator("Object Allocator",
                                         veryVeryVeryVerbose);

    // Default Test Allocator.
    bslma::TestAllocator         defaultAllocator("Default Allocator",
                                                  veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    // Global Test Allocator.
    bslma::TestAllocator globalAllocator("Global Allocator",
                                         veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 9: {
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

        UsageExample1::main();

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // CLASS PROPERTIES
        //
        // Concerns:
        //: 1 The footprint size of 'bdlma::SmallSequentialAllocator'
        //:   is sufficiently less than that of 'bdlma::SequentialAllocator'
        //:   to justify the appellation "small".
        //:
        //: 2 The class 'bdlma::SmallSequentialAllocator' inherits from the
        //:   'bdlma::ManagedAllocator' protocol class.
        //
        // Plan:
        //: 1 Explicit comparison using the 'sizeof' operator.
        //:
        //: 2 Use the address of a 'bdlma::SmallSequentialAllocator' object to
        //:   initialize without casting a pointer to
        //:   'bdlma::ManagedAllocator'.  Note that in the absence of the
        //:   expected class relation, the statement fails to compile.
        //
        // Testing:
        //   CONCERN: The footprint is smaller than 'SequentialAllocator'.
        //   CONCERN: The class inherits from 'bdlma::MangageAllocator'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CLASS PROPERTIES" << endl
                                  << "================" << endl;

        ASSERTV(sizeof(bdlma::SequentialAllocator),  sizeof(Obj),
                static_cast<double>(sizeof(bdlma::SequentialAllocator))
              / static_cast<double>(sizeof(Obj)) > 3.0);

        Obj                      mX;
        bdlma::ManagedAllocator *map = &mX;
        ASSERT(map);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // 'reserveCapacity' TEST
        //
        // Concerns:
        //: 1 If there is sufficient memory within the internal buffer,
        //:   'reserveCapacity' does not trigger dynamic allocation.
        //:
        //: 2 One can allocate at least the amount of bytes specified in
        //:   'reserveCapacity' before triggering another dynamic allocation.
        //:
        //: 3 The 'reserveCapacity' method can override the maximum buffer size
        //:   parameter supplied to the pool at construction.
        //:
        //: 4 Invoking 'reserveCapacity' on a default constructed pool
        //:   succeeds.
        //:
        //: 5 Invoking 'reserveCapacity' with 0 bytes succeeds with no dynamic
        //:   memory allocation.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a 'bdlma::SmallSequentialAllocator' using a test allocator
        //:   and specify an initial size and maximum buffer size.
        //:
        //: 2 Invoke 'reserveCapacity' with a size less than the initial size.
        //:   Allocate the same amount of memory, and verify, using the test
        //:   allocator, that no new dynamic allocation is triggered.  (C-1)
        //:
        //: 3 Invoke 'reserveCapacity' with a size larger than the initial
        //:   size.  Verify that the call triggers dynamic allocation, and that
        //:   we can allocate the same amount of memory without triggering
        //:   further dynamic allocation.  (C-2)
        //:
        //: 4 Invoke 'reserveCapacity' with a size larger than the maximum
        //:   buffer size.  Repeat verification for C-2.  (C-3)
        //:
        //: 5 Repeat the tests for C-1 and C-2 with a default constructed
        //:   sequential allocator.  (C-4)
        //:
        //: 6 Invoke 'reserveCapacity' with a 0 size, and verify that no memory
        //:   is allocated.  (C-5)
        //:
        //: 7 QoI: Verify that, in appropriate build modes, defensive checks
        //:   are triggered.  (C-6)
        //
        // Testing:
        //   void reserveCapacity(int numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'reserveCapacity' TEST" << endl
                                  << "======================" << endl;

        enum { k_INITIAL_SIZE = 64, k_MAX_BUFFER = k_INITIAL_SIZE * 4 };

        if (verbose) cout << "Testing that 'reserveCapacity' does not "
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
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());
        }

        if (verbose) cout << "Testing 'reserveCapacity' on a default "
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

        if (verbose) cout << "Testing 'reserveCapacity' with 0 bytes"
                          << endl;
        {
            Obj mX(&objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());

            mX.reserveCapacity(0);
            ASSERT(0 == objectAllocator.numBytesInUse());
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // 'truncate' TEST
        //
        // Concerns:
        //: 1 The 'truncate' method reduces the amount of memory allocated to
        //:   the specified 'newSize'.
        //:
        //: 2 When 'truncate' fails, 'originalSize' is returned.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, create test vectors having the
        //:   alignment strategy, initial allocation size, the new size, and
        //:   expected offset.  First allocate memory of initial allocation
        //:   size, then truncate to the new size and allocate memory (1 byte)
        //:   again.  Verify that the latest allocation matches the expected
        //:   offset.  (C-1)
        //:
        //: 2 Truncate the memory returned by the initial allocation, and
        //:   verify that the return value is 'originalSize'.  (C-2)
        //:
        //: 3 QoI: Verify that, in appropriate build modes, defensive checks
        //:   are triggered.  (C-3)
        //
        // Testing:
        //   int truncate(void *address, int originalSize, int newSize);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'truncate' TEST" << endl
                                  << "===============" << endl;

#define NAT bsls::Alignment::BSLS_NATURAL
#define MAX bsls::Alignment::BSLS_MAXIMUM
#define BYT bsls::Alignment::BSLS_BYTEALIGNED

        if (verbose) cout << "Testing 'truncate'." << endl;

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
                             static_cast<char *>(addr1) + EXPOFFSET
                          == static_cast<char *>(addr2));
            }
            else {
                ASSERT(used < objectAllocator.numBytesInUse());
            }

            // Truncating previously allocated address should fail.
            if (EXPOFFSET >= 0
             && (static_cast<char *>(addr1) + INITIALSIZE)
             != (static_cast<char *>(addr2) + 1)) {
                int ret = static_cast<int>(mX.truncate(addr1,
                                                       INITIALSIZE,
                                                       NEWSIZE));
                LOOP2_ASSERT(INITIALSIZE, ret, INITIALSIZE == ret);
            }
        }

        if (verbose) cout << "Negative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

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
        //: 1 The 'allocateAndExpand' method returns the maximum amount of
        //:   memory available for use without triggering another allocation.
        //:
        //: 2 The 'allocateAndExpand' method returns the updated size of memory
        //:   used.
        //:
        //: 3 The 'allocateAndExpand' method has no effect when passed a size
        //:   of 0.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, create test vectors having the
        //:   alignment strategy, initial memory offset, and expected memory
        //:   used.  First allocate memory necessary for the initial memory
        //:   offset, then allocate 1 byte using 'allocateAndExpand'.  Verify
        //:   the updated size is the same as the expected memory used.
        //:   Finally, invoke 'allocate' again and verify it triggers new
        //:   dynamic memory allocation -- meaning 'allocateAndExpand' did use
        //:   up all available memory in the internal buffer.  (C-1..3)
        //:
        //: 2 QoI: Verify that, in appropriate build modes, defensive checks
        //:   are triggered.  (C-4)
        //
        // Testing:
        //   void *allocateAndExpand(bsl::size_t *size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'allocateAndExpand' TEST" << endl
                                  << "========================" << endl;

#define NAT bsls::Alignment::BSLS_NATURAL
#define MAX bsls::Alignment::BSLS_MAXIMUM
#define BYT bsls::Alignment::BSLS_BYTEALIGNED

        if (verbose) cout << "Testing 'expand'." << endl;

        static const struct {
            int   d_line;         // line number
            Strat d_strategy;     // alignment strategy
            int   d_initialSize;  // size of initial allocation request
            int   d_expused;      // expected memory used after 'expand'
        } DATA[] = {
            // LINE     STRAT    INITIALSIZE   EXPUSED
            // ----     -----    -----------   -------

            // NATURAL ALIGNMENT
            {  L_,      NAT,           1,   k_DEFAULT_SIZE - 1           },
            {  L_,      NAT,           2,   k_DEFAULT_SIZE - 2           },
            {  L_,      NAT,           3,   k_DEFAULT_SIZE - 3           },
            {  L_,      NAT,           4,   k_DEFAULT_SIZE - 4           },
            {  L_,      NAT,           7,   k_DEFAULT_SIZE - 7           },
            {  L_,      NAT,           8,   k_DEFAULT_SIZE - 8           },
            {  L_,      NAT,          15,   k_DEFAULT_SIZE - 15          },
            {  L_,      NAT,          16,   k_DEFAULT_SIZE - 16          },
            {  L_,      NAT,         100,   k_DEFAULT_SIZE - 100         },

            // MAXIMUM ALIGNMENT
            {  L_,      MAX,           1,   k_DEFAULT_SIZE - k_MAX_ALIGN },
            {  L_,      MAX,           2,   k_DEFAULT_SIZE - k_MAX_ALIGN },
            {  L_,      MAX,           3,   k_DEFAULT_SIZE - k_MAX_ALIGN },
            {  L_,      MAX,           4,   k_DEFAULT_SIZE - k_MAX_ALIGN },
            {  L_,      MAX,           7,   k_DEFAULT_SIZE - k_MAX_ALIGN },
            {  L_,      MAX,           8,   k_DEFAULT_SIZE - k_MAX_ALIGN },
            {  L_,      MAX,          15,   k_DEFAULT_SIZE - 16          },
            {  L_,      MAX,          16,   k_DEFAULT_SIZE - 16          },
            {  L_,      MAX,         108,   k_DEFAULT_SIZE - 112         },

            // 1-BYTE ALIGNMENT
            {  L_,      BYT,           1,   k_DEFAULT_SIZE - 1           },
            {  L_,      BYT,           2,   k_DEFAULT_SIZE - 2           },
            {  L_,      BYT,           3,   k_DEFAULT_SIZE - 3           },
            {  L_,      BYT,           4,   k_DEFAULT_SIZE - 4           },
            {  L_,      BYT,           7,   k_DEFAULT_SIZE - 7           },
            {  L_,      BYT,           8,   k_DEFAULT_SIZE - 8           },
            {  L_,      BYT,          15,   k_DEFAULT_SIZE - 15          },
            {  L_,      BYT,          16,   k_DEFAULT_SIZE - 16          },
            {  L_,      BYT,         100,   k_DEFAULT_SIZE - 100         },
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

            void               *addr1 = mX.allocate(INITIALSIZE);
            bsls::Types::Int64  numBytesUsed = objectAllocator.numBytesInUse();
            ASSERT(1 == objectAllocator.numBlocksInUse());

            bsl::size_t  size = 1;
            void        *addr2 = mX.allocateAndExpand(&size);

            // Check for correct memory address.
            if (NAT == STRAT || BYT == STRAT) {
                ASSERT(static_cast<char *>(addr1) + INITIALSIZE
                    == static_cast<char *>(addr2));
            }
            else {
                int offset = bsls::AlignmentUtil::calculateAlignmentOffset(
                                      static_cast<char *>(addr1) + INITIALSIZE,
                                      k_MAX_ALIGN);
                ASSERT(static_cast<char *>(addr1) + INITIALSIZE + offset
                    == static_cast<char *>(addr2));
            }

            // Check 'size' is updated correctly.
            ASSERT(EXPUSED == static_cast<int>(size));

            // Check for no new allocations.
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());
            ASSERT(1            == objectAllocator.numBlocksInUse());

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
            bsl::size_t size = 0;
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

        if (verbose) cout << "Negative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout << "\t'0 != size'" << endl;
            {
                Obj mX;

                bsl::size_t size = 1;

                ASSERT_SAFE_PASS_RAW(mX.allocateAndExpand(&size));

                ASSERT_SAFE_FAIL_RAW(mX.allocateAndExpand(    0));
            }
        }

#undef NAT
#undef MAX
#undef BYT

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'release' AND 'rewind' TEST
        //
        // Concerns:
        //: 1 All memory allocated from the allocator supplied at construction
        //:   is deallocated after 'release'.
        //:
        //: 2 Allocation requests after invocation of the 'release' method
        //:   follow the specified growth and alignment strategies.
        //:
        //: 3 All memory allocated from the allocator supplied at construction
        //:   is reclaimed by 'rewind' except for the latest, non-"large"
        //:   allocation.
        //
        // Plan:
        //: 1 Construct a small sequential allocator using a
        //:   'bslma::TestAllocator', and allocate several memory blocks such
        //:   that there are multiple dynamic allocations.  Then invoke
        //:   'release' and verify, using the test allocator, that there is no
        //:   outstanding memory allocated.  Finally, allocate memory again and
        //:   verify the alignment and growth strategies.  (C-1..2)
        //:
        //: 2 Construct a small sequential allocator using a
        //:   'bslma::TestAllocator' (so memory usage is easily measured)  and
        //:    configured with a 'maxBufferSize' (so we can create "large"
        //:    blocks that should *not* be retained past calls the 'rewind').
        //:    Trigger a "large" block allocation and then 'rewind'.  Confirm
        //:    that the initial allocation (on construction) is retained but
        //:    the "large" block is not.
        //
        // Testing:
        //   void release();
        //   void rewind();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'release' AND 'rewind' TEST" << endl
                                  << "===========================" << endl;

#define CON bsls::BlockGrowth::BSLS_CONSTANT
#define MAX bsls::Alignment::BSLS_MAXIMUM

        if (verbose) cout << "Testing allocated memory is deallocated after"
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
            ASSERT(2 == objectAllocator.numBlocksInUse());

            // Release all memory.
            mX.release();

            ASSERT(0 == objectAllocator.numBlocksInUse());

            if (verbose) cout << "Testing alignment and growth strategies."
                              << endl;

            // Testing alignment.
            void *addr1 = mX.allocate(1);
            void *addr2 = mX.allocate(2);

            ASSERT(static_cast<char *>(addr1) + k_MAX_ALIGN
                == static_cast<char *>(addr2));

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

        if (verbose) cout << "Testing allocated memory is deallocated after"
                             " 'rewind'." << endl;
        {
            ASSERT(0 == objectAllocator.numBlocksInUse());

            Obj mX(k_DEFAULT_SIZE, k_DEFAULT_SIZE, CON, &objectAllocator);
                // Create an allocator using constant growth strategy and
                // having a maximum buffer size that suppresses the fallback to
                // geometric growth.  Thus, all allocations in excess of
                // 'k_DEFAULT_SIZE' are "large" blocks.
            ASSERT(1 == objectAllocator.numBlocksInUse());

            bsls::Types::Int64 numBytesInUse = objectAllocator.numBytesInUse();

            mX.allocate(k_DEFAULT_SIZE + 1); // Allocate a "large" block
            ASSERT(2             == objectAllocator.numBlocksInUse());
            ASSERT(numBytesInUse <  objectAllocator.numBytesInUse());

            mX.rewind();  // Reclaim all but the last non-"large" block.
            ASSERT(1             == objectAllocator.numBlocksInUse());
            ASSERT(numBytesInUse == objectAllocator.numBytesInUse());

            mX.release();
            ASSERT(0 == objectAllocator.numBlocksInUse());
        }

#undef CON
#undef MAX

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'deallocate' TEST
        //
        // Concerns:
        //: 1  The 'deallocate' method has no effect.
        //
        // Plan:
        //: 1 Create a sequential allocator initialized with a test allocator.
        //:   Request memory of varying sizes and then deallocate each memory
        //:   block.  Verify that the number of bytes in use indicated by the
        //:   test allocator does not decrease after each 'deallocate' method
        //:   invocation.  (C-1)
        //
        // Testing:
        //   void deallocate(void *address);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'deallocate' TEST" << endl
                                  << "=================" << endl;

        if (verbose) cout << "Testing 'deallocate'." << endl;

        const int DATA[] = { 0, 1, 5, 12, 24, 32, 64, 256, 257, 512, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        Obj mX(&objectAllocator);

        bsls::Types::Int64 lastNumBytesInUse = objectAllocator.numBytesInUse();

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                 SIZE          = DATA[i];
            void                     *p             = mX.allocate(SIZE);
            const bsls::Types::Int64  numBytesInUse =
                                               objectAllocator.numBytesInUse();
            if (veryVerbose) { P(SIZE) }

            mX.deallocate(p);
            LOOP_ASSERT(i, numBytesInUse == objectAllocator.numBytesInUse());
            LOOP_ASSERT(i, lastNumBytesInUse <=
                                              objectAllocator.numBytesInUse());
            lastNumBytesInUse = objectAllocator.numBytesInUse();
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR / ALLOCATE TEST
        //
        // Concerns:
        //: 1 The 'bdlma::SmallSequentialAllocator' class correctly proxies its
        //:   constructor arguments and allocation requests to the
        //:   'bdlma::SmallSequentialPool' (thoroughly tested) it adapts.
        //:
        //: 2 Allocating 0 bytes returns 0.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a series of pairs of mechanisms: 1) a small sequential
        //:   allocator, and 2) a small sequential pool (for reference).  In
        //:   each case the small sequential pool is created using the same
        //:   constructor arguments as the allocator.
        //:
        //:   1 The series of mechanism pairs uses each constructor of the
        //:     small sequential allocator, and covers a representative sample
        //:     of the argument space for each constructor parameter.
        //:
        //:   2 For a series of different sizes, use the
        //:     'mirrorAllocatorAndPool' helper function. to perform identical
        //:     allocations from the allocator and pool mechanisms check check
        //:     for difference in the observable state.
        //:
        //:   3 The absence of difference provides strong evidence that the
        //:     implementation of the small sequential allocator correctly
        //:     wraps a small sequential pool.
        //:
        //: 2 Test the return value of a 0 sized allocation is 0.
        //:
        //: 3 Using the facilities for negative testing, verify that, in
        //:   appropriate build modes, defensive checks are triggered.  (C-3)
        //
        // Testing:
        //   SmallSequentialAllocator(Alloc *a = 0);
        //   SmallSequentialAllocator(GS g, Alloc *a = 0);
        //   SmallSequentialAllocator(AS a, Alloc *a = 0);
        //   SmallSequentialAllocator(GS g, AS a, Alloc *a = 0);
        //
        //   SmallSequentialAllocator(int initSize);
        //   SmallSequentialAllocator(int i, Alloc *a = 0);
        //   SmallSequentialAllocator(int i, GS g, Alloc *a = 0);
        //   SmallSequentialAllocator(int i, AS a, Alloc *a = 0);
        //   SmallSequentialAllocator(int i, GS g, AS a, Alloc *a = 0);
        //
        //   SmallSequentialAllocator(int i, int m, Alloc *a = 0);
        //   SmallSequentialAllocator(int i, int m, GS g, Alloc *a = 0);
        //   SmallSequentialAllocator(int i, int m, AS a, Alloc *a = 0);
        //   SmallSequentialAllocator(int i, int m, GS g, AS a, *a = 0);
        //
        //   ~SmallSequentialAllocator();
        //
        //   void *allocate(bsl::size_t size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CONSTRUCTOR / ALLOCATE TEST" << endl
                                  << "===========================" << endl;

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
        const bsls::BlockGrowth::Strategy GS[]   = { GEO, CON };
        const int                         NUM_GS = sizeof GS / sizeof *GS;

        // alignment strategy
        const Strat AS[]   = { NAT, MAX, BYT };
        const int   NUM_AS = sizeof AS / sizeof *AS;

        if (verbose) cout << "Testing 'Obj(Alloc *a = 0)'." << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tp(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                Obj    mX(&ta);
                Pool pool(&tp);

                mirrorAllocatorAndPool(&mX, &pool, ta, tp, SIZE, false);
            }
        }

        if (verbose) cout << "Testing 'Obj(GS g, Alloc *a = 0)'." << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tp(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_GS; ++j) {
                    if (veryVerbose) { T_ T_ P(GS[j]) }

                    Obj    mX(GS[j], &ta);
                    Pool pool(GS[j], &tp);

                    mirrorAllocatorAndPool(&mX, &pool, ta, tp, SIZE, false);
                }
            }
        }

        if (verbose) cout << "Testing 'Obj(AS a, Alloc *a = 0)'." << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tp(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {

                    if (veryVerbose) { T_ T_ P(AS[j]) }

                    Obj    mX(AS[j], &ta);
                    Pool pool(AS[j], &tp);

                    mirrorAllocatorAndPool(&mX, &pool, ta, tp, SIZE, false);
                }
            }
        }

        if (verbose) cout << "Testing 'Obj(GS g, AS a, Alloc *a = 0)'."
                          << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tp(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {      // alignment strategy

                    if (veryVerbose) { T_ T_ P(AS[j]) }

                    for (int k = 0; k < NUM_GS; ++k) {  // growth strategy

                        if (veryVerbose) { T_ T_ T_ P(GS[k]) }

                        Obj    mX(GS[k], AS[j], &ta);
                        Pool pool(GS[k], AS[j], &tp);

                        mirrorAllocatorAndPool(&mX,
                                               &pool,
                                               ta,
                                               tp,
                                               SIZE,
                                               false);
                    }
                }
            }
        }

        if (verbose) cout << "Testing 'Obj(int initSize)'." << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tp(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                bsls::Types::Int64 numBytesInUse1a = 0;
                bsls::Types::Int64 numBytesInUse1p = 0;

                {
                    bslma::DefaultAllocatorGuard guard(&ta);

                    Obj mX(k_INITIAL_SIZE);

                    void *addr1a = mX.allocate(SIZE);
                    ASSERT(0 != addr1a);

                    numBytesInUse1a = ta.numBytesInUse();
                }

                {
                    bslma::DefaultAllocatorGuard guard(&tp);

                    Pool pool(k_INITIAL_SIZE);

                    void *addr1p = pool.allocate(SIZE);
                    ASSERT(0 != addr1p);

                    numBytesInUse1p = tp.numBytesInUse();
                }

                ASSERT(   numBytesInUse1p ==    numBytesInUse1a);
                ASSERT(tp.numBytesInUse() == ta.numBytesInUse());
            }
        }

        if (verbose) cout << "Testing 'Obj(initSize, Alloc *a = 0)'."
                          << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tp(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                Obj    mX(k_INITIAL_SIZE, &ta);
                Pool pool(k_INITIAL_SIZE, &tp);

                mirrorAllocatorAndPool(&mX, &pool, ta, tp, SIZE, false);
            }
        }

        if (verbose) cout << "Testing 'Obj(initSize, GS g, Alloc *a = 0)'."
                          << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tp(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_GS; ++j) {

                    if (veryVerbose) { T_ T_ P(GS[j]) }

                    Obj    mX(k_INITIAL_SIZE, GS[j], &ta);
                    Pool pool(k_INITIAL_SIZE, GS[j], &tp);

                    mirrorAllocatorAndPool(&mX, &pool, ta, tp, SIZE, false);
                }
            }
        }

        if (verbose) cout << "Testing 'Obj(initSize, AS a, Alloc *a = 0)'."
                          << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tp(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {

                    if (veryVerbose) { T_ T_ P(AS[j]) }

                    Obj    mX(k_INITIAL_SIZE, AS[j], &ta);
                    Pool pool(k_INITIAL_SIZE, AS[j], &tp);

                    mirrorAllocatorAndPool(&mX, &pool, ta, tp, SIZE, false);
                }
            }
        }

        if (verbose) cout
                        << "Testing 'Obj(initSize, GS g, AS a, Alloc *a = 0)'."
                        << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tp(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {      // alignment strategy

                    if (veryVerbose) { T_ T_ P(j) }

                    for (int k = 0; k < NUM_GS; ++k) {  // growth strategy

                        if (veryVerbose) { T_ T_ T_ P(k) }

                        Obj    mX(k_INITIAL_SIZE, GS[k], AS[j], &ta);
                        Pool pool(k_INITIAL_SIZE, GS[k], AS[j], &tp);

                        mirrorAllocatorAndPool(&mX,
                                               &pool,
                                               ta,
                                               tp,
                                               SIZE,
                                               false);
                    }
                }
            }
        }

        if (verbose) cout << "Testing 'Obj(initSize, int m, Alloc *a = 0)'."
                          << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tp(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                Obj    mX(k_INITIAL_SIZE, k_MAX_BUFFER, &ta);
                Pool pool(k_INITIAL_SIZE, k_MAX_BUFFER, &tp);

                mirrorAllocatorAndPool(&mX,
                                        &pool,
                                        ta,
                                        tp,
                                        SIZE,
                                        k_MAX_BUFFER < SIZE);
            }
        }

        if (verbose) cout
                       << "Testing 'Obj(initSize, int m, GS g, Alloc *a = 0)'."
                       << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tp(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_GS; ++j) {

                    if (veryVerbose) { T_ T_ P(GS[j]) }

                    Obj    mX(k_INITIAL_SIZE, k_MAX_BUFFER, GS[j], &ta);
                    Pool pool(k_INITIAL_SIZE, k_MAX_BUFFER, GS[j], &tp);

                    mirrorAllocatorAndPool(&mX,
                                           &pool,
                                           ta,
                                           tp,
                                           SIZE,
                                           k_MAX_BUFFER < SIZE);
                }
            }
        }

        if (verbose) cout
                       << "Testing 'Obj(initSize, int m, AS a, Alloc *a = 0)'."
                       << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tp(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {

                    if (veryVerbose) { T_ T_ P(AS[j]) }

                    Obj    mX(k_INITIAL_SIZE, k_MAX_BUFFER, AS[j], &ta);
                    Pool pool(k_INITIAL_SIZE, k_MAX_BUFFER, AS[j], &tp);

                    mirrorAllocatorAndPool(&mX,
                                            &pool,
                                            ta,
                                            tp,
                                            SIZE,
                                            k_MAX_BUFFER < SIZE);
                }
            }
        }

        if (verbose) cout
                << "Testing 'Obj(initSize, int m, GS g, AS a, Alloc *a = 0)'."
                << endl;
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            bslma::TestAllocator tp(veryVeryVeryVerbose);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) { T_ P(SIZE) }

                for (int j = 0; j < NUM_AS; ++j) {      // alignment strategy

                    if (veryVerbose) { T_ T_ P(AS[j]) }

                    for (int k = 0; k < NUM_GS; ++k) {  // growth strategy

                        if (veryVerbose) { T_ T_ T_ P(GS[k]) }

                        Obj mX(k_INITIAL_SIZE,
                               k_MAX_BUFFER,
                               GS[k],
                               AS[j],
                               &ta);

                        Pool pool(k_INITIAL_SIZE,
                                  k_MAX_BUFFER,
                                  GS[k],
                                  AS[j],
                                  &tp);

                        mirrorAllocatorAndPool(&mX,
                                               &pool,
                                               ta,
                                               tp,
                                               SIZE,
                                               k_MAX_BUFFER < SIZE);
                    }
                }
            }
        }

        if (verbose) cout << "Testing allocation size of 0 bytes." << endl;
        {
            Obj   mX(&objectAllocator);
            void *addr = mX.allocate(0);
            ASSERT(0 == addr);
            ASSERT(0 == objectAllocator.numBytesInUse());
        }
        ASSERT(0 ==  objectAllocator.numBlocksTotal());
        ASSERT(0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 ==  globalAllocator.numBlocksTotal());

        if (verbose) cout << "Negative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout << "\t'Obj(i, *ba)'" << endl;
            {
                ASSERT_PASS_RAW(Obj( 1));
                ASSERT_FAIL_RAW(Obj( 0));
            }

            if (veryVerbose) cout << "\t'Obj(i, GS, *ba)'" << endl;
            {
                ASSERT_PASS_RAW(Obj( 1, CON));
                ASSERT_FAIL_RAW(Obj( 0, CON));
            }

            if (veryVerbose) cout << "\t'Obj(i, AS, *ba)'" << endl;
            {
                ASSERT_PASS_RAW(Obj( 1, MAX));
                ASSERT_FAIL_RAW(Obj( 0, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(i, GS, AS, *ba)'" << endl;
            {
                ASSERT_PASS_RAW(Obj( 1, CON, MAX));
                ASSERT_FAIL_RAW(Obj( 0, CON, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(i, m, *ba)'" << endl;
            {
                ASSERT_PASS_RAW(Obj( 1,  8));
                ASSERT_FAIL_RAW(Obj( 0,  8));

                ASSERT_PASS_RAW(Obj( 2,  2));
                ASSERT_FAIL_RAW(Obj( 2,  1));
            }

            if (veryVerbose) cout << "\t'Obj(i, m, GS, *ba)'" << endl;
            {
                ASSERT_PASS_RAW(Obj( 1,  8, CON));
                ASSERT_FAIL_RAW(Obj( 0,  8, CON));

                ASSERT_PASS_RAW(Obj( 2,  2, CON));
                ASSERT_FAIL_RAW(Obj( 2,  1, CON));
            }

            if (veryVerbose) cout << "\t'Obj(i, m, AS, *ba)'" << endl;
            {
                ASSERT_PASS_RAW(Obj( 1,  8, MAX));
                ASSERT_FAIL_RAW(Obj( 0,  8, MAX));

                ASSERT_PASS_RAW(Obj( 2,  2, MAX));
                ASSERT_FAIL_RAW(Obj( 2,  1, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(i, m, GS, AS, *ba)'" << endl;
            {
                ASSERT_PASS_RAW(Obj( 1,  8, CON, MAX));
                ASSERT_FAIL_RAW(Obj( 0,  8, CON, MAX));

                ASSERT_PASS_RAW(Obj( 2,  2, CON, MAX));
                ASSERT_FAIL_RAW(Obj( 2,  1, CON, MAX));
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
        //: 1 A 'bdlma::SmallSequentialAllocator' object can be created and
        //:   destroyed.
        //:
        //: 2 The 'allocate' method returns a block of memory having the
        //:   specified size and expected alignment.
        //:
        //: 3 The 'allocate' method does not always cause dynamic allocation
        //:   (i.e., the allocator manages an internal buffer of memory).
        //:
        //: 4 The 'allocate' returns a "large" block of memory even when the
        //:   the allocation request exceeds the initial size of the internal
        //:   buffer.
        //:
        //: 5 The 'rewind' method releases the "large" block but retains the
        //:   other buffer.
        //:
        //: 5 Destruction of the allocator releases all managed memory.
        //
        // Plan:
        //: 1 Create a 'bdlma::SmallSequentialAllocator' with a
        //:   'bslma::TestAllocator' (C-1).
        //:
        //: 2 Allocate a block of memory, and verify that it comes from the
        //:   test allocator.  Allocate another block of memory, and verify
        //:   that no dynamic allocation is triggered (C-3).
        //:
        //: 3 Verify the alignment of each allocation using
        //:   'bsls::AlignmentUtil' (C-2).
        //:
        //: 3 Allocate a large block of memory and verify the memory comes from
        //:   the test allocator (C-4).
        //:
        //: 4 Invoke 'rewind' and confirm that test allocator shows that
        //:   exactly one block was returned.  (C-5)
        //:
        //: 5 Destroy the allocator and confirm that all allocated memory is
        //:   deallocated.  (C-6)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        enum { k_ALLOC_SIZE1 = 4, k_ALLOC_SIZE2 = 8, k_ALLOC_SIZE3 = 1024 };

        ASSERT(k_ALLOC_SIZE1 + k_ALLOC_SIZE2    <= k_DEFAULT_SIZE);
        ASSERT(static_cast<int>(k_DEFAULT_SIZE) <  k_ALLOC_SIZE3);

        {
            if (verbose) cout << endl
                              << "Testing construction of allocator." << endl;

            Obj mX(&objectAllocator);

            // Confirm that no memory allocated from the object, default, or
            // global allocators.

            ASSERT(0 ==  objectAllocator.numBlocksTotal());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 ==  globalAllocator.numBlocksTotal());

            if (verbose) cout << endl << "Testing allocation." << endl;
            void *addr1 = mX.allocate(k_ALLOC_SIZE1);

            ASSERT(0 !=  objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 ==  globalAllocator.numBlocksTotal());

            bsls::Types::Int64 oldNumBytesInUse =
                                               objectAllocator.numBytesInUse();

            if (verbose) cout << endl << "Testing internal buffering." << endl;
            void *addr2 = mX.allocate(k_ALLOC_SIZE2);

            ASSERT(oldNumBytesInUse ==  objectAllocator.numBytesInUse());
            ASSERT(0                == defaultAllocator.numBlocksTotal());
            ASSERT(0                ==  globalAllocator.numBlocksTotal());

            if (verbose) cout << endl
                              << "Check values of returned addresses."
                              << endl;
            ASSERT(bsls::AlignmentUtil::is4ByteAligned(addr1));
            ASSERT(bsls::AlignmentUtil::is8ByteAligned(addr2));
            ASSERT(k_ALLOC_SIZE1 <= static_cast<char *>(addr2)
                                  - static_cast<char *>(addr1));

            if (verbose) cout << endl << "Testing large allocation." << endl;
            void *addr3 = mX.allocate(k_ALLOC_SIZE3);

            ASSERT(oldNumBytesInUse < objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 ==  globalAllocator.numBlocksTotal());
            ASSERT(0 == bsls::AlignmentUtil::calculateAlignmentOffset(
                                                                 addr3,
                                                                 k_MAX_ALIGN));

            if (verbose) cout << endl << "Testing rewind." << endl;
            mX.rewind();
            ASSERT(0 != objectAllocator.numBytesInUse());
            ASSERT(1 == objectAllocator.numBlocksInUse());
            ASSERT(2 == objectAllocator.numBlocksTotal());
        }

        if (verbose) cout << endl << "Testing destruction." << endl;
        ASSERT(0 ==  objectAllocator.numBytesInUse());
        ASSERT(0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 ==  globalAllocator.numBlocksTotal());

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
// Copyright 2020 Bloomberg Finance L.P.
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
