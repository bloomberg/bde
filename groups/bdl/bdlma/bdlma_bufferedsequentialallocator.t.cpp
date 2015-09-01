// bdlma_bufferedsequentialallocator.t.cpp                            -*-C++-*-
#include <bdlma_bufferedsequentialallocator.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_alignedbuffer.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_cstdio.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_string.h>
#include <bsl_vector.h>
#include <bsl_utility.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// A 'bdlma::BufferedSequentialAllocator' adapts the
// 'bdlma::BufferedSequentialPool' mechanism to the 'bdlma::ManagedAllocator'
// protocol.  The primary concern is that the allocator correctly proxies the
// memory allocation requests to the buffered sequential pool it adapts.
//
// To test that requests are correctly proxied, we create a buffered sequential
// allocator and a buffered sequential pool using the same external buffer and
// two different test allocators.  We then verify that either:
//
// 1) The addresses returned by the allocator and the pool are the same (i.e.,
//    the memory is allocated from the buffer).
//
// 2) If the memory used by either test allocator is non-zero, the number of
//    bytes used by both test allocators is the same.
//
// We also need to verify that the 'deallocate' method has no effect.  Again,
// we make use of the test allocator to ensure that no memory is deallocated
// when the 'deallocate' method of a buffered sequential allocator is invoked
// on previously allocated memory (both from the buffer and from the fall back
// allocator).
//
// Finally, the destructor of 'bdlma::BufferedSequentialAllocator' is tested
// throughout the test driver.  At destruction, the allocator should reclaim
// all outstanding allocated memory.  By setting the global allocator, default
// allocator, and object allocator to different test allocators, we can
// determine whether all memory had been released by the destructor of the
// buffered sequential allocator.
//-----------------------------------------------------------------------------
// // CREATORS
// [ 2] bdlma::BufferedSequentialAllocator(*buf, sz, *a = 0);
// [ 2] bdlma::BufferedSequentialAllocator(*buf, sz, GS, *a = 0);
// [ 2] bdlma::BufferedSequentialAllocator(*buf, sz, AS, *a = 0);
// [ 2] bdlma::BufferedSequentialAllocator(*buf, sz, GS, AS, *a = 0);
//
// [ 2] bdlma::BufferedSequentialAllocator(*buf, sz, max, *a = 0);
// [ 2] bdlma::BufferedSequentialAllocator(*buf, sz, max, GS, *a = 0);
// [ 2] bdlma::BufferedSequentialAllocator(*buf, sz, max, AS, *a = 0);
// [ 2] bdlma::BufferedSequentialAllocator(*buf, sz, max, GS, AS, *a = 0);
//
// [ 5] ~bdlma::BufferedSequentialAllocator();
//
// // MANIPULATORS
// [ 2] void *allocate(size_type size);
// [ 3] void deallocate(void *address);
// [ 4] void release();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE TEST

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

typedef bdlma::BufferedSequentialAllocator Obj;

typedef bsls::Alignment::Strategy          Strat;

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
///Example 1: Using 'bdlma::BufferedSequentialAllocator' with Exact Calculation
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need to implement a method, 'calculate', that performs
// calculations (where the specifics are not important to illustrate the use of
// this component), which require three vectors of 'double' values.
// Furthermore, suppose we know that we need to store at most 100 values for
// each vector:
//..
    double calculate(const bsl::vector<double>& data)
    {
//..
// Since the amount of memory needed is known in advance, we can optimize the
// memory allocation by using a 'bdlma::BufferedSequentialAllocator' to supply
// memory for the vectors.  We can also prevent the vectors from resizing
// (which triggers more allocations) by reserving for the specific capacity we
// need:
//..
        enum { k_SIZE = 3 * 100 * sizeof(double) };
//..
// In the above calculation, we assume that the only memory allocation
// requested by the vector is the allocation for the array that stores the
// 'double' values.  Furthermore, we assume that the 'reserve' method allocates
// the exact amount of memory for the number of items specified (in this case,
// of type 'double').  Note that both of these assumptions are true for BDE's
// implementation of 'bsl::vector'.
//
// To avoid alignment issues described in the "Warning" section (above), we
// create a 'bsls::AlignedBuffer':
//..
        bsls::AlignedBuffer<k_SIZE> bufferStorage;

        bdlma::BufferedSequentialAllocator alloc(bufferStorage.buffer(),
                                                 k_SIZE);

        bsl::vector<double> v1(&alloc);     v1.reserve(100);
        bsl::vector<double> v2(&alloc);     v2.reserve(100);
        bsl::vector<double> v3(&alloc);     v3.reserve(100);

        return data.empty() ? 0.0 : data.front();
    }
//..
// By making use of a 'bdlma::BufferedSequentialAllocator', *all* dynamic
// memory allocation is eliminated in the above example.
//

//=============================================================================
//           Additional Functionality Needed to Complete Usage Test Case
//-----------------------------------------------------------------------------

void receivePriceQuotes(bsl::map<bsl::string, double> *updateMap)
{
    enum { k_NUM_SECURITIES = 100 };

    for (int i = 0; i < k_NUM_SECURITIES; ++i) {
        char buffer[256];
        int n = bsl::sprintf(buffer, "sec%d", i);
        bsl::string security(buffer, n);
        updateMap->insert(bsl::make_pair(security, 1.0));
    }
}

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

    // Object Test Allocator
    bslma::TestAllocator objectAllocator("Object Allocator",
                                         veryVeryVeryVerbose);

    // Default Test Allocator
    bslma::TestAllocator defaultAllocator("Default Allocator",
                                          veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    // Global Test Allocator
    bslma::TestAllocator globalAllocator("Global Allocator",
                                         veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 6: {
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

        if (verbose) cout << "\nTesting usage example 1." << endl;
        {
            bsl::vector<double> v;
            calculate(v);

            ASSERT(0 == defaultAllocator.numBytesTotal());
            ASSERT(0 == globalAllocator.numBytesTotal());
            ASSERT(0 == objectAllocator.numBytesTotal());
        }

        if (verbose) cout << "\nTesting usage example 2." << endl;
        {

///Example 2: Using 'bdlma::BufferedSequentialAllocator' with Fallback
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are receiving updates for price quotes for a list of securities
// through the following function:
//..
//  void receivePriceQuotes(bsl::map<bsl::string, double> *updateMap);
//      // Load into the specified 'updateMap' updates for price quotes for a
//      // list of securities.
//..
// Furthermore, suppose the number of securities we are interested in is
// limited.  We can then use a 'bdlma::BufferedSequentialAllocator' to optimize
// memory allocation for the 'bsl::map'.  We first create a buffer on the
// stack:
//..
    enum {
        k_NUM_SECURITIES = 100,

        k_TREE_NODE_SIZE = sizeof(bsl::map<bsl::string, double>::value_type)
                         + sizeof(void *) * 4,

        k_AVERAGE_SECURITY_LENGTH = 5,

        k_TOTAL_SIZE = k_NUM_SECURITIES *
                                 (k_TREE_NODE_SIZE + k_AVERAGE_SECURITY_LENGTH)
    };

    bsls::AlignedBuffer<k_TOTAL_SIZE> bufferStorage;
//..
// The calculation of the amount of memory needed is just an estimate, as we
// used the average security size instead of the maximum security size.  We
// also assume that a 'bsl::map's node size is roughly the size of 4 pointers.
//..
    bdlma::BufferedSequentialAllocator bsa(bufferStorage.buffer(),
                                           k_TOTAL_SIZE,
                                           &objectAllocator);
    bsl::map<bsl::string, double> updateMap(&bsa);

    receivePriceQuotes(&updateMap);
//..
// With the use of a 'bdlma::BufferedSequentialAllocator', we can be reasonably
// assured that the memory allocation performance is optimized (i.e., minimal
// use of dynamic allocation).

            ASSERT(0 == globalAllocator.numBytesTotal());
            if (verbose) P(objectAllocator.numBytesTotal())
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // DTOR TEST
        //
        // Concerns:
        //   1) That the previously managed buffer is not changed in any way
        //      after destruction of the allocator.
        //
        //   2) That all memory allocated from the allocator supplied at
        //      construction is deallocated after destruction of the
        //      buffered sequential allocator.
        //
        // Plan:
        //   For concern 1, dynamically allocate a buffer using a test
        //   allocator.  Then, set all bytes in the buffer to '0xA', and
        //   initialize a buffered sequential allocator using the buffer and
        //   the same test allocator.  Finally, destroy the allocator, and
        //   verify that the bytes in the first buffer remain '0xA' and the
        //   buffer is not deallocated.
        //
        //   For concern 2, construct a buffered sequential allocator using a
        //   'bslma::TestAllocator', then allocate sufficient memory such that
        //   the buffer runs out and the allocator is used.  Finally, destroy
        //   the buffered sequential allocator, and verify, using the test
        //   allocator, that there is no outstanding memory allocated.
        //
        // Testing:
        //   ~bdlma::BufferedSequentialAllocator();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DTOR TEST" << endl
                                  << "=========" << endl;

        if (verbose) cout << "\nTesting managed buffer is not affected after "
                             "destruction." << endl;
        {
            char bufferRef[k_BUFFER_SIZE];
            bsls::Types::Int64 total = 0;

            ASSERT(0 == objectAllocator.numBlocksInUse());
            char *buffer = (char *)objectAllocator.allocate(k_BUFFER_SIZE);
            total = objectAllocator.numBlocksInUse();

            {
                memset(buffer,    0xA, k_BUFFER_SIZE);
                memset(bufferRef, 0xA, k_BUFFER_SIZE);

                Obj mX(buffer, k_BUFFER_SIZE, &objectAllocator);

                ASSERT(total == objectAllocator.numBlocksInUse());

                // Allocate some memory.
                mX.allocate(1);
                mX.allocate(16);
                ASSERT(total == objectAllocator.numBlocksInUse());
            }
            ASSERT(total == objectAllocator.numBlocksInUse());
            ASSERT(0 == memcmp(buffer, bufferRef, k_BUFFER_SIZE));

            objectAllocator.deallocate(buffer);
            ASSERT(0 == objectAllocator.numBlocksInUse());
        }

        if (verbose) cout << "\nTesting allocated memory is deallocated after"
                             " destruction." << endl;
        {
            char buffer[k_BUFFER_SIZE];

            ASSERT(0 == objectAllocator.numBlocksInUse());

            Obj mX(buffer, k_BUFFER_SIZE, &objectAllocator);

            ASSERT(0 == objectAllocator.numBlocksInUse());

            // Allocate some memory such that memory is allocated from the test
            // allocator.
            mX.allocate(k_BUFFER_SIZE + 1);
            mX.allocate(1);
            mX.allocate(16);
            ASSERT(0 != objectAllocator.numBlocksInUse());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'release' TEST
        //
        // Concerns:
        //   1) That 'release' has no effect on the previously managed buffer.
        //
        //   2) That all memory allocated from the allocator supplied at
        //      construction is deallocated after 'release'.
        //
        //   3) That subsequent allocation requests after invocation of the
        //      'release' method are satisfied by the buffer supplied at
        //      construction.
        //
        // Plan:
        //   For concern 1, dynamically allocate a buffer using a test
        //   allocator.  Then, set all bytes in the buffer to '0xA', and
        //   initialize a buffered sequential allocator using the buffer and
        //   the same test allocator.  Finally, invoke 'release' and verify
        //   that the bytes in the first buffer remain '0xA' and the buffer is
        //   not deallocated.
        //
        //   For concerns 2 and 3, construct a buffered sequential allocator
        //   using a 'bslma::TestAllocator', then allocate sufficient memory
        //   such that the buffer runs out and the allocator is used.  Finally,
        //   invoke 'release' and verify, using the test allocator, that there
        //   is no outstanding memory allocated.  Then, allocate memory again
        //   and verify memory comes from the buffer.
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'release' TEST" << endl
                                  << "==============" << endl;

        if (verbose) cout << "\nTesting managed buffer is not affected after "
                             "'release'." << endl;

        {
            ASSERT(0 == objectAllocator.numBlocksInUse());

            char *buffer = (char *)objectAllocator.allocate(k_BUFFER_SIZE);
            char bufferRef[k_BUFFER_SIZE];

            bsls::Types::Int64 total = objectAllocator.numBlocksInUse();

            memset(buffer,    0xA, k_BUFFER_SIZE);
            memset(bufferRef, 0xA, k_BUFFER_SIZE);

            Obj mX(buffer, k_BUFFER_SIZE, &objectAllocator);

            ASSERT(total == objectAllocator.numBlocksInUse());

            // Allocate some memory.
            mX.allocate(1);
            mX.allocate(16);
            ASSERT(total == objectAllocator.numBlocksInUse());

            // Release all memory.
            mX.release();
            ASSERT(total == objectAllocator.numBlocksInUse());

            ASSERT(0 == memcmp(buffer, bufferRef, k_BUFFER_SIZE));

            objectAllocator.deallocate(buffer);
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());

        if (verbose) cout << "\nTesting allocated memory is deallocated after"
                             " 'release'." << endl;
        {
            char buffer[k_BUFFER_SIZE];

            ASSERT(0 == objectAllocator.numBlocksInUse());

            Obj mX(buffer, k_BUFFER_SIZE, &objectAllocator);

            ASSERT(0 == objectAllocator.numBlocksInUse());

            // Allocate some memory such that memory is allocated from the test
            // allocator.
            mX.allocate(k_BUFFER_SIZE + 1);
            mX.allocate(1);
            mX.allocate(16);

            ASSERT(0 != objectAllocator.numBlocksInUse());

            // Release all memory.
            mX.release();

            ASSERT(0 == objectAllocator.numBlocksInUse());

            if (verbose) cout << "\nTesting subsequent allocations come"
                                 " first from the initial buffer." << endl;
            void *addr = mX.allocate(16);

            ASSERT(&buffer[0] <= addr);
            ASSERT(&buffer[0] + k_BUFFER_SIZE > addr);
            ASSERT(0 == objectAllocator.numBlocksInUse());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'deallocate' TEST
        //
        // Concerns:
        //   That 'deallocate' has no effect.
        //
        // Plan:
        //   Create a buffered sequential allocator initialized with a test
        //   allocator.  Request memory of varying sizes and then deallocate
        //   each memory block.  Verify that the number of bytes in use
        //   indicated by the test allocator does not decrease after each
        //   'deallocate' method invocation.
        //
        // Testing:
        //   void deallocate(void *address);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'deallocate' TEST" << endl
                                  << "=================" << endl;

        char *buffer = bufferStorage.buffer();

        if (verbose) cout << "\nTesting 'deallocate'." << endl;

        const int DATA[] = { 0, 1, 5, 12, 24, 32, 64, 256, 257, 512, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        Obj mX(buffer, k_BUFFER_SIZE, &objectAllocator);

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
        //   1) That the 'bdlma::BufferedSequentialAllocator' correctly proxies
        //      its constructor arguments and allocation requests to the
        //      'bdlma::BufferedSequentialPool' it adapts.
        //
        //   2) That allocating 0 bytes returns 0.
        //
        //   3) QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   Using the array driven approach, create an array of various memory
        //   allocation request sizes.  Then, create both a buffered sequential
        //   allocator and buffered sequential pool using the same buffer and
        //   two different test allocators.  Finally, verify that:
        //
        //   1) The addresses returned by the allocator and the pool are the
        //      same (i.e., the memory is allocated from the buffer).
        //
        //   2) If the memory used by either test allocator is non-zero, the
        //      number of bytes used by both test allocators is the same.
        //
        //   In addition, verify that in appropriate build modes, defensive
        //   checks are triggered.
        //
        // Testing:
        //   bdlma::BufferedSequentialAllocator(*buf, sz, *a = 0);
        //   bdlma::BufferedSequentialAllocator(*buf, sz, GS, *a = 0);
        //   bdlma::BufferedSequentialAllocator(*buf, sz, AS, *a = 0);
        //   bdlma::BufferedSequentialAllocator(*buf, sz, GS, AS, *a = 0);
        //   bdlma::BufferedSequentialAllocator(*buf, sz, max, *a = 0);
        //   bdlma::BufferedSequentialAllocator(*buf, sz, max, GS, *a = 0);
        //   bdlma::BufferedSequentialAllocator(*buf, sz, max, AS, *a = 0);
        //   bdlma::BufferedSequentialAllocator(*buf, sz, max, GS, AS, *a = 0);
        //   void *allocate(size_type size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CTOR / ALLOCATE TEST" << endl
                                  << "====================" << endl;

        char *buffer = bufferStorage.buffer();

        Strat MAX = bsls::Alignment::BSLS_MAXIMUM;
        Strat NAT = bsls::Alignment::BSLS_NATURAL;
        Strat BYT = bsls::Alignment::BSLS_BYTEALIGNED;

        const int DATA[] = { 1, 5, 12, 24, 32, 64, 256, 257, 512, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator taA(veryVeryVeryVerbose);
        bslma::TestAllocator taB(veryVeryVeryVerbose);
        bslma::TestAllocator taC(veryVeryVeryVerbose);
        bslma::TestAllocator taD(veryVeryVeryVerbose);
        bslma::TestAllocator taV(veryVeryVeryVerbose);
        bslma::TestAllocator taW(veryVeryVeryVerbose);
        bslma::TestAllocator taX(veryVeryVeryVerbose);
        bslma::TestAllocator taY(veryVeryVeryVerbose);

        if (verbose) cout << "\nTesting single allocation." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            // allocators
            Obj mV(buffer, k_BUFFER_SIZE,      &taV);
            Obj mW(buffer, k_BUFFER_SIZE, MAX, &taW);
            Obj mX(buffer, k_BUFFER_SIZE, NAT, &taX);
            Obj mY(buffer, k_BUFFER_SIZE, BYT, &taY);

            // pools
            bdlma::BufferedSequentialPool mA(buffer, k_BUFFER_SIZE,      &taA);
            bdlma::BufferedSequentialPool mB(buffer, k_BUFFER_SIZE, MAX, &taB);
            bdlma::BufferedSequentialPool mC(buffer, k_BUFFER_SIZE, NAT, &taC);
            bdlma::BufferedSequentialPool mD(buffer, k_BUFFER_SIZE, BYT, &taD);

            void *addrA = mA.allocate(DATA[i]);
            void *addrV = mV.allocate(DATA[i]);

            void *addrB = mB.allocate(DATA[i]);
            void *addrW = mW.allocate(DATA[i]);

            void *addrC = mC.allocate(DATA[i]);
            void *addrX = mX.allocate(DATA[i]);

            void *addrD = mD.allocate(DATA[i]);
            void *addrY = mY.allocate(DATA[i]);

            // Allocating from the test allocator.
            if (taA.numBytesInUse()) {
                ASSERT(taA.numBytesInUse() == taV.numBytesInUse());
            }
            else {
                ASSERT(addrA == addrV);
            }

            if (taB.numBytesInUse()) {
                ASSERT(taB.numBytesInUse() == taW.numBytesInUse());
            }
            else {
                ASSERT(addrB == addrW);
            }

            if (taC.numBytesInUse()) {
                ASSERT(taC.numBytesInUse() == taX.numBytesInUse());
            }
            else {
                ASSERT(addrC == addrX);
            }

            if (taD.numBytesInUse()) {
                ASSERT(taD.numBytesInUse() == taY.numBytesInUse());
            }
            else {
                ASSERT(addrD == addrY);
            }
        }
        ASSERT(0 == taA.numBytesInUse());
        ASSERT(0 == taB.numBytesInUse());
        ASSERT(0 == taC.numBytesInUse());
        ASSERT(0 == taD.numBytesInUse());
        ASSERT(0 == taV.numBytesInUse());
        ASSERT(0 == taW.numBytesInUse());
        ASSERT(0 == taX.numBytesInUse());
        ASSERT(0 == taY.numBytesInUse());

        if (verbose) cout << "\nTesting consecutive allocations." << endl;
        {
            // allocators
            Obj mV(buffer, k_BUFFER_SIZE,      &taV);
            Obj mW(buffer, k_BUFFER_SIZE, MAX, &taW);
            Obj mX(buffer, k_BUFFER_SIZE, NAT, &taX);
            Obj mY(buffer, k_BUFFER_SIZE, BYT, &taY);

            // pools
            bdlma::BufferedSequentialPool mA(buffer, k_BUFFER_SIZE,      &taA);
            bdlma::BufferedSequentialPool mB(buffer, k_BUFFER_SIZE, MAX, &taB);
            bdlma::BufferedSequentialPool mC(buffer, k_BUFFER_SIZE, NAT, &taC);
            bdlma::BufferedSequentialPool mD(buffer, k_BUFFER_SIZE, BYT, &taD);

            for (int i = 0; i < NUM_DATA; ++i) {
                void *addrA = mA.allocate(DATA[i]);
                void *addrV = mV.allocate(DATA[i]);

                void *addrB = mB.allocate(DATA[i]);
                void *addrW = mW.allocate(DATA[i]);

                void *addrC = mC.allocate(DATA[i]);
                void *addrX = mX.allocate(DATA[i]);

                void *addrD = mD.allocate(DATA[i]);
                void *addrY = mY.allocate(DATA[i]);

                // Allocating from the test allocator.
                if (taA.numBytesInUse()) {
                    ASSERT(taA.numBytesInUse() == taV.numBytesInUse());
                }
                else {
                    ASSERT(addrA == addrV);
                }

                if (taB.numBytesInUse()) {
                    ASSERT(taB.numBytesInUse() == taW.numBytesInUse());
                }
                else {
                    ASSERT(addrB == addrW);
                }

                if (taC.numBytesInUse()) {
                    ASSERT(taC.numBytesInUse() == taX.numBytesInUse());
                }
                else {
                    ASSERT(addrC == addrX);
                }

                if (taD.numBytesInUse()) {
                    ASSERT(taD.numBytesInUse() == taY.numBytesInUse());
                }
                else {
                    ASSERT(addrD == addrY);
                }
            }
        }
        ASSERT(0 == taA.numBytesInUse());
        ASSERT(0 == taB.numBytesInUse());
        ASSERT(0 == taC.numBytesInUse());
        ASSERT(0 == taD.numBytesInUse());
        ASSERT(0 == taV.numBytesInUse());
        ASSERT(0 == taW.numBytesInUse());
        ASSERT(0 == taX.numBytesInUse());
        ASSERT(0 == taY.numBytesInUse());

        if (verbose) cout << "\nTesting allocation size of 0 bytes." << endl;
        {
            Obj mX(buffer, k_BUFFER_SIZE, &objectAllocator);
            void *addr = mX.allocate(0);
            ASSERT(0 == addr);
            ASSERT(0 == objectAllocator.numBytesInUse());
        }
        ASSERT(0 == objectAllocator.numBlocksTotal());
        ASSERT(0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

#define GEO bsls::BlockGrowth::BSLS_GEOMETRIC
#define CON bsls::BlockGrowth::BSLS_CONSTANT

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'Obj(buf, sz, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2));

                ASSERT_SAFE_FAIL_RAW(Obj(0,       2));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  0));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer, -1));
            }

            if (veryVerbose) cout << "\t'Obj(buf, sz, GS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2, CON));

                ASSERT_SAFE_FAIL_RAW(Obj(0,       2, CON));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  0, CON));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer, -1, CON));
            }

            if (veryVerbose) cout << "\t'Obj(buf, sz, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2, MAX));

                ASSERT_SAFE_FAIL_RAW(Obj(0,       2, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  0, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer, -1, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(buf, sz, GS, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2, CON, MAX));

                ASSERT_SAFE_FAIL_RAW(Obj(0,       2, CON, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  0, CON, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer, -1, CON, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(buf, sz, max, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2,  8));

                ASSERT_SAFE_FAIL_RAW(Obj(0,       2,  8));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  0,  8));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer, -1,  8));

                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2,  2));

                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  2,  1));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  2, -2));
            }

            if (veryVerbose) cout << "\t'Obj(buf, sz, max, GS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2,  8, CON));

                ASSERT_SAFE_FAIL_RAW(Obj(0,       2,  8, CON));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  0,  8, CON));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer, -1,  8, CON));

                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2,  2, CON));

                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  2,  1, CON));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  2, -2, CON));
            }

            if (veryVerbose) cout << "\t'Obj(buf, sz, max, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2,  8, MAX));

                ASSERT_SAFE_FAIL_RAW(Obj(0,       2,  8, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  0,  8, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer, -1,  8, MAX));

                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2,  2, MAX));

                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  2,  1, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  2, -2, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(buf, sz, max, GS, AS, *ba)'"
                                  << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2,  8, CON, MAX));

                ASSERT_SAFE_FAIL_RAW(Obj(0,       2,  8, CON, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  0,  8, CON, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer, -1,  8, CON, MAX));

                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2,  2, CON, MAX));

                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  2,  1, CON, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  2, -2, CON, MAX));
            }
        }

#undef GEO
#undef CON

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1) That a 'bdlma::BufferedSequentialAllocator' can be created and
        //      destroyed.
        //
        //   2) That 'allocate' returns a block of memory having the specified
        //      size and expected alignment.
        //
        //   3) That 'allocate' returns a block of memory from the external
        //      buffer supplied at construction.
        //
        //   4) That 'allocate' returns a block of memory even when the
        //      the allocation request exceeds the remaining free space in the
        //      external buffer.
        //
        //   5) Destruction of the allocator releases all managed memory,
        //      including memory that comes from dynamic allocation.
        //
        // Plan:
        //   For concerns 1, 2, and 3, first, create a
        //   'bdlma::BufferedSequentialAllocator' with an aligned static
        //   buffer.  Next, allocate a block of memory from the allocator and
        //   verify that it comes from the external buffer.  Then, allocate
        //   another block of memory from the allocator, and verify that the
        //   first allocation returns a block of memory of sufficient size by
        //   checking that 'addr2 >= addr1 + ALLOC_SIZE1'.  Also verify that
        //   the alignment strategy indicated at construction is followed by
        //   checking the address of the second allocation.
        //
        //   For concern 4, initialize a 'bdlma::BufferedSequentialAllocator'
        //   with a 'bslma::TestAllocator'.  Then allocate a block of memory
        //   that is larger than the buffer supplied at construction of the
        //   buffered sequential allocator.  Verify that memory is allocated
        //   from the test allocator.
        //
        //   For concern 5, let the allocator created with the test allocator
        //   go out of scope, and verify, through the test allocator, that all
        //   allocated memory is deallocated.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        enum { k_ALLOC_SIZE1 = 4, k_ALLOC_SIZE2 = 8 };
        char *buffer = bufferStorage.buffer();

        if (verbose) cout << "\nTesting constructor." << endl;
        Obj mX(buffer, k_BUFFER_SIZE, &objectAllocator);

        if (verbose) cout << "\nTesting allocate from buffer." << endl;
        void *addr1 = mX.allocate(k_ALLOC_SIZE1);

        // Allocation starts at the beginning of the aligned buffer.
        ASSERT(&buffer[0] == addr1);

        // Allocation comes from within the buffer.
        LOOP2_ASSERT((void *)&buffer[k_BUFFER_SIZE - 1],
                     addr1,
                     &buffer[k_BUFFER_SIZE - 1] >= addr1);

        void *addr2 = mX.allocate(k_ALLOC_SIZE2);

        // Allocation comes from within the buffer.
        ASSERT(&buffer[0]               <  addr2);
        ASSERT(&buffer[k_BUFFER_SIZE - 1] >= addr2);

        // Allocation respects the alignment strategy.
        LOOP2_ASSERT((void *)&buffer[8],
                     addr2,
                     &buffer[8] == addr2);

        // First allocation is of sufficient size.
        ASSERT((char *)addr2 >= (char *)addr1 + k_ALLOC_SIZE1);

        // Make sure no memory comes from the object, default, and global
        // allocators.
        ASSERT(0 == objectAllocator.numBlocksTotal());
        ASSERT(0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        if (verbose) cout << "\nTesting allocate when buffer runs out."
                          << endl;
        {
            Obj mY(buffer, k_BUFFER_SIZE, &objectAllocator);
            addr1 = mY.allocate(k_BUFFER_SIZE + 1);

            // Allocation request is satisfied even when larger than the
            // supplied buffer.
            LOOP_ASSERT(addr1, 0 != addr1);

            // Allocation comes from the objectAllocator.
            ASSERT(0 != objectAllocator.numBlocksInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());
        }

        // All dynamically allocated memory is released after the allocator's
        // destruction.
        ASSERT(0 == objectAllocator.numBlocksInUse());

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
