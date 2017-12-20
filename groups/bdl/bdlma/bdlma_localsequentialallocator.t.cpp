// bdlma_localsequentialallocator.t.cpp                               -*-C++-*-
#include <bdlma_localsequentialallocator.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_alignedbuffer.h>
#include <bsls_alignmentutil.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_cstdio.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// A 'bdlma::LocalSequentialAllocator' provides an encapsulation of a common
// use case of 'bdlma::BufferedSequentialAllocator', which is to provide it
// with a 'bsls::AlignedBuffer' as the local buffer.  This class adds no extra
// methods to the inherited 'bdlma::BufferedSequentialAllocator', so the
// primary concern is that the local buffer is correctly passed to the
// 'bdlma::BufferedSequentialAllocator' base.
//
// We also need to verify that the 'deallocate' method has no effect.  Again,
// we make use of the test allocator to ensure that no memory is deallocated
// when the 'deallocate' method of a buffered sequential allocator is invoked
// on previously allocated memory (both from the buffer and from the fall back
// allocator).
//
// Finally, the destructor of 'bdlma::LocalSequentialAllocator' is tested
// throughout the test driver.  At destruction, the allocator should reclaim
// all outstanding allocated memory.  By setting the global allocator, default
// allocator, and object allocator to different test allocators, we can
// determine whether all memory had been released by the destructor of the
// local sequential allocator.
//-----------------------------------------------------------------------------
// // CREATORS
// [ 2] bdlma::LocalSequentialAllocator(*a = 0);
//
// [ 5] ~bdlma::LocalSequentialAllocator();
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

static const int k_SIZE = 256;
typedef bdlma::LocalSequentialAllocator<k_SIZE> Obj;

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
///Example 1: Recommended Usage
/// - - - - - - - - - - - - - -
// Suppose we have a function which takes a map of items to update in some
// database:
//..
    typedef bsl::string DatabaseKey;
    typedef bsl::string DatabaseValue;

    void updateRecords_1(const bsl::map<DatabaseKey, DatabaseValue>& values)
    {
        for (bsl::map<DatabaseKey, DatabaseValue>::const_iterator
                 it = values.begin(), end = values.end();
             it != end;
             ++it) {
            bsl::stringbuf stringBuf;
            bsl::ostream   ostr(&stringBuf);

            ostr << "UPDATE myTable SET myValue = '" << it->first << "' WHERE "
                    "myKey = '" << it->second << "'";

            // execute query using 'stringBuf.str()'
        }
    }
//..
// We call this method a lot, and after profiling, we notice that it's
// contributing a significant proportion of time, due to the allocations it is
// making.  We decide to see whether a LocalSequentialAllocator would help.
//
// First, use a 'bslma::TestAllocator' to track the typical memory usage:
//..
    void updateRecords_2(const bsl::map<DatabaseKey, DatabaseValue>& values)
    {
        bslma::TestAllocator ta;

        for (bsl::map<DatabaseKey, DatabaseValue>::const_iterator
                 it = values.begin(), end = values.end();
             it != end;
             ++it) {
            bsl::stringbuf stringBuf(&ta);
            bsl::ostream   ostr(&stringBuf);

            ostr << "UPDATE myTable SET myValue = '" << it->first << "' WHERE "
                    "myKey = '" << it->second << "'";

            // execute query using 'stringBuf.str()'

            bsl::cout << "In use: " << ta.numBytesInUse() << '\n';
        }

        bsl::cout << "Max: " << ta.numBytesMax() << '\n';
    }
//..
// Then we run our program again, and observe the following output:
//..
//  In use: 77
//  In use: 77
//  In use: 77
//  In use: 77
//  In use: 77
//  Max: 129
//..
// It looks like 129 is a good choice for the size of our allocator, so we go
// with that:
//..
    void updateRecords_3(const bsl::map<DatabaseKey, DatabaseValue>& values)
    {
        bdlma::LocalSequentialAllocator<129> lsa;

        for (bsl::map<DatabaseKey, DatabaseValue>::const_iterator
                 it = values.begin(), end = values.end();
             it != end;
             ++it) {
            lsa.release();

            bsl::stringbuf stringBuf(&lsa);
            bsl::ostream   ostr(&stringBuf);

            ostr << "UPDATE myTable SET myValue = '" << it->first << "' WHERE "
                    "myKey = '" << it->second << "'";

            // execute query using 'stringBuf.str()'
        }
    }
//..
// Note that we release at the end of every iteration, as the deallocate method
// is a no-op, so without this, subsequent memory would be allocated from the
// default allocator (or the allocator passed to 'bsa' at construction).
//
// Finally, we re-profile our code to determine whether the addition of a
// 'LocalSequentialAllocator' helped.

//=============================================================================
//           Additional Functionality Needed to Complete Usage Test Case
//-----------------------------------------------------------------------------

void populateDummyData(bsl::map<DatabaseKey, DatabaseValue> *values)
{
    BSLS_ASSERT(values);

    typedef bsl::map<DatabaseKey, DatabaseValue>::value_type ValueType;

    values->insert(ValueType("MyKeyNumber1", "MyValueNumber1"));
    values->insert(ValueType("MyKeyNumber2", "MyValueNumber2"));
    values->insert(ValueType("MyKeyNumber3", "MyValueNumber3"));
    values->insert(ValueType("MyKeyNumber4", "MyValueNumber4"));
    values->insert(ValueType("MyKeyNumber5", "MyValueNumber5"));
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
        //:   leading comment characters, and replace 'assert' with 'ASSERT'
        //:   (C-1).
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        if (verbose) cout << "\nTesting usage example 1." << endl;
        {
            bsl::map<DatabaseKey, DatabaseValue> values;
            populateDummyData(&values);

            updateRecords_1(values);
            updateRecords_2(values);
            updateRecords_3(values);
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // DTOR TEST
        //
        // Concerns:
        //: 1 That all memory allocated from the allocator supplied at
        //:   construction is deallocated after destruction of the local
        //:   sequential allocator.
        //
        // Plan:
        //: 1 Construct a local sequential allocator using a
        //:   'bslma::TestAllocator', then allocate sufficient memory such that
        //:   the buffer runs out and the allocator is used.  Finally, destroy
        //:   the local sequential allocator, and verify, using the test
        //:   allocator, that there is no outstanding memory allocated (C-1).
        //
        // Testing:
        //   ~bdlma::LocalSequentialAllocator();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DTOR TEST" << endl
                                  << "=========" << endl;

        if (verbose) cout << "\nTesting allocated memory is deallocated after"
                             " destruction." << endl;
        {
            ASSERT(0 == objectAllocator.numBlocksInUse());

            Obj mX(&objectAllocator);

            ASSERT(0 == objectAllocator.numBlocksInUse());

            // Allocate some memory such that memory is allocated from the test
            // allocator.
            mX.allocate(k_SIZE + 1);
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
        //: 1 That all memory allocated from the allocator supplied at
        //:   construction is deallocated after 'release'.
        //
        //: 2 That subsequent allocation requests after invocation of the
        //:   'release' method are satisfied by the local buffer.
        //
        // Plan:
        //: 1 Construct a local sequential allocator using a
        //: 'bslma::TestAllocator', then allocate sufficient memory such that
        //: the buffer runs out and the allocator is used.  Finally, invoke
        //: 'release' and verify, using the test allocator, that there is no
        //: outstanding memory allocated.  Then, allocate memory again and
        //: verify memory comes from the local buffer, rather than the external
        //: allocator (C-1, C-2).
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'release' TEST" << endl
                                  << "==============" << endl;

        if (verbose) cout << "\nTesting allocated memory is deallocated after"
                             " 'release'." << endl;
        {
            ASSERT(0 == objectAllocator.numBlocksInUse());

            Obj mX(&objectAllocator);

            ASSERT(0 == objectAllocator.numBlocksInUse());

            // Allocate some memory such that memory is allocated from the test
            // allocator.
            mX.allocate(k_SIZE + 1);
            mX.allocate(1);
            mX.allocate(16);

            ASSERT(0 != objectAllocator.numBlocksInUse());

            // Release all memory.
            mX.release();

            ASSERT(0 == objectAllocator.numBlocksInUse());

            if (verbose) cout << "\nTesting subsequent allocations come"
                                 " first from the initial buffer." << endl;
            void *addr = mX.allocate(16);
            ASSERT(0 != addr);

            ASSERT(0 == objectAllocator.numBlocksInUse());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'deallocate' TEST
        //
        // Concerns:
        //: 1 That 'deallocate' has no effect.
        //
        // Plan:
        //: 1 Create a local sequential allocator initialized with a test
        //:   allocator.  Request memory of varying sizes and then deallocate
        //:   each memory block.  Verify that the number of bytes in use
        //:   indicated by the test allocator does not decrease after each
        //:   'deallocate' method invocation (C-1).
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
        //: 1 That the 'bdlma::LocalSequentialAllocator' correctly provides a
        //:   local buffer for the 'bdlma::BufferedSequentialAllocator' it
        //:   adapts.
        //
        //: 2 That allocating 0 bytes returns 0.
        //
        // Plan:
        //: 1 Using the array driven approach, create an array of various
        //:   memory allocation request sizes.  Then, create both a local
        //:   sequential allocator and buffered sequential allocator using the
        //:   same buffer size and two different test allocators.  Finally,
        //:   verify that:
        //:     1) If the memory used by either test allocator is non-zero, the
        //:        number of bytes used by both test allocators is the same
        //:        (C-1).
        //
        //: 2 Create a local sequential allocator, and call 'allocate' with 0.
        //:   Verify that the returned address is 0.  (C-2)
        //
        // Testing:
        //   bdlma::LocalSequentialAllocator(*a = 0);
        //   void *allocate(size_type size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CTOR / ALLOCATE TEST" << endl
                                  << "====================" << endl;

        bsls::AlignedBuffer<k_SIZE> buffer;
        const int DATA[] = { 1, 5, 12, 24, 32, 64, 256, 257, 512, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator taA(veryVeryVeryVerbose);
        bslma::TestAllocator taV(veryVeryVeryVerbose);

        if (verbose) cout << "\nTesting single allocation." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            // local allocator
            Obj mV(&taV);

            // buffered allocator
            bdlma::BufferedSequentialAllocator mA(buffer.buffer(),
                                                  k_SIZE,
                                                  &taA);

            void *addrA = mA.allocate(DATA[i]);
            LOOP_ASSERT(i, 0 != addrA);

            void *addrV = mV.allocate(DATA[i]);
            LOOP_ASSERT(i, 0 != addrV);

            // If one allocates from the provided allocator, the other should
            // too.
            LOOP_ASSERT(i, taA.numBytesInUse() == taV.numBytesInUse());
        }
        ASSERT(0 == taA.numBytesInUse());
        ASSERT(0 == taV.numBytesInUse());

        if (verbose) cout << "\nTesting consecutive allocations." << endl;
        {
            // local allocator
            Obj mV(&taV);

            // buffered allocator
            bdlma::BufferedSequentialAllocator mA(buffer.buffer(),
                                                  k_SIZE,
                                                  &taA);

            for (int i = 0; i < NUM_DATA; ++i) {
                void *addrA = mA.allocate(DATA[i]);
                LOOP_ASSERT(i, 0 != addrA);

                void *addrV = mV.allocate(DATA[i]);
                LOOP_ASSERT(i, 0 != addrV);

                // If one allocates from the provided allocator, the other
                // should too.
                LOOP_ASSERT(i, taA.numBytesInUse() == taV.numBytesInUse());
            }
        }
        ASSERT(0 == taA.numBytesInUse());
        ASSERT(0 == taV.numBytesInUse());

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

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 That a 'bdlma::LocalSequentialAllocator' can be created and
        //:   destroyed.
        //
        //: 2 That 'allocate' returns a block of memory having the specified
        //:   size.
        //
        //: 3 That 'allocate' returns a block of memory having the correct
        //:   alignment.
        //
        //: 4 That 'allocate' returns a block of memory even when the
        //:   allocation request exceeds the remaining free space in the
        //:   internal buffer.
        //
        //: 5 Destruction of the allocator releases all managed memory,
        //:   including memory that comes from dynamic allocation.
        //
        // Plan:
        //: 1 First, create a 'bdlma::LocalSequentialAllocator' (C-1).  Next,
        //:   allocate a block of memory from the allocator and verify that it
        //:   comes from its local buffer (C-2).
        //
        //: 2 Using the array driven approach, allocate 1 byte, followed by
        //:   N bytes.  Ensure that the N bytes are correctly aligned for the
        //:   size of N (C-3).  This test is repeated, where an extra call to
        //:   ensure the local buffer is full.  This ensures that the alignment
        //:   is correct for memory both from the local buffer, and from the
        //:   supplied allocator.
        //
        //: 3 Initialize a 'bdlma::LocalSequentialAllocator' with a
        //:   'bslma::TestAllocator'.  Then allocate a block of memory that is
        //:   larger than its local buffer.  Verify that memory is allocated
        //:   from the test allocator (C-4).
        //
        //: 4 Let the allocator created with the test allocator go out of scope
        //:   (C-1), and verify, through the test allocator, that all allocated
        //:   memory is deallocated (C-5).
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        enum { k_ALLOC_SIZE1 = 4, k_ALLOC_SIZE2 = 8 };

        if (verbose) cout << "\nTesting constructor." << endl;
        {
            Obj mX(&objectAllocator);

            if (verbose) cout << "\nTesting allocate from buffer." << endl;
            void *addr1 = mX.allocate(k_ALLOC_SIZE1);
            ASSERT(0 != addr1);

            // Allocation comes from within the buffer.
            ASSERT(0 == objectAllocator.numBlocksTotal());

            void *addr2 = mX.allocate(k_ALLOC_SIZE2);
            ASSERT(0 != addr2);

            // Make sure no memory comes from the object, default, and global
            // allocators.
            ASSERT(0 == objectAllocator.numBlocksTotal());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());
        }

        if (verbose) cout << "\nTesting alignment." << endl;
        {
            Obj mY(&objectAllocator);

            const int DATA[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            if (veryVerbose) cout << "\nFrom the local buffer." << endl;
            for (int i = 0; i < NUM_DATA; ++i) {
                mY.release();

                char *addr1 = static_cast<char *>(mY.allocate(1));
                char *addr2 = static_cast<char *>(mY.allocate(DATA[i]));

                // We expect natural alignment.
                const int expectedAlignment
                    = bsls::AlignmentUtil::calculateAlignmentFromSize(DATA[i]);

                LOOP_ASSERT(i, expectedAlignment == (addr2 - addr1));
            }

            if (veryVerbose) cout << "\nFrom the supplied allocator." << endl;
            for (int i = 0; i < NUM_DATA; ++i) {
                mY.release();

                // Exhaust the local buffer.
                char *addr0 = static_cast<char *>(mY.allocate(k_SIZE));
                ASSERT(0 != addr0);

                char *addr1 = static_cast<char *>(mY.allocate(1));
                ASSERT(0 != addr1);

                // Ensure that we're allocating from the supplied allocator.
                LOOP_ASSERT(i, 0 != objectAllocator.numBlocksInUse());

                char *addr2 = static_cast<char *>(mY.allocate(DATA[i]));
                ASSERT(0 != addr2);

                // We expect natural alignment.
                const int expectedAlignment
                    = bsls::AlignmentUtil::calculateAlignmentFromSize(DATA[i]);

                LOOP_ASSERT(i, expectedAlignment == (addr2 - addr1));
            }
        }

        if (verbose) cout << "\nTesting allocate when buffer runs out."
                          << endl;
        {
            Obj mZ(&objectAllocator);
            void *addr1 = mZ.allocate(k_SIZE + 1);

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
