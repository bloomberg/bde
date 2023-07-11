// bdlma_heapbypassallocator.t.cpp                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlma_heapbypassallocator.h>

#include <bslim_testutil.h>

#include <bdlf_bind.h>
#include <bslmt_barrier.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>

#include <bsls_review.h>

#include <bsl_iostream.h>

#include <bsl_cstdlib.h>    // 'atoi'
#include <bsl_cstring.h>    // 'memcmp'
#include <bsl_set.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cin;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
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

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

//=============================================================================
//                      GLOBAL HELPER #DEFINES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER TYPES & CLASSES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
static void concurrent(bdlma::HeapBypassAllocator *bypassAllocator,
                       bslmt::Barrier             *barrier)
    // Wait for the specified 'barrier', then use the specified
    // 'bypassAllocator' to perform a series of small allocations.  Verify that
    // the allocated addresses are unique and that their memory has not been
    // corrupted by another thread.
{
    // Allocate a bunch of individual bytes
    bsl::vector<void *> addrs(1024096);
    barrier->wait();
    for (size_t i = 0; i != addrs.size(); ++i) {
        addrs[i] = bypassAllocator->allocate(1);
        memset(addrs[i], static_cast<int>(i & 0xff), 1);
    }

    // Check that none of our byte values have changed since being set
    for (size_t i = 0; i != addrs.size(); ++i) {
        unsigned char v;
        memcpy(&v, addrs[i], 1);
        ASSERT(static_cast<int>(i & 0xff) == v);
    }

    // Ensure all returned addresses are unique
    bsl::set<void *> unique(addrs.begin(), addrs.end());
    ASSERT(unique.size() == addrs.size());
}

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST
        //
        // Concerns:
        //: 1 Concurrent allocation is safe.
        //
        // Plan:
        //: 1 Create a heap bypass allocator, do some allocations from multiple
        //    threads at once, and verify the addresses are unique and non-
        //    overlapping.  (C-1)
        //
        // Testing:
        //   CONCERN: Concurrent allocation is safe.
        // --------------------------------------------------------------------

        if (verbose) cout << "CONCURRENCY TEST\n"
                             "================\n";

        enum { k_NUM_THREADS = 8 };

        bdlma::HeapBypassAllocator bypassAllocator(4096);
        bslmt::Barrier             barrier(k_NUM_THREADS);
        bslmt::ThreadGroup         tg;
        tg.addThreads(
                 bdlf::BindUtil::bind(&concurrent, &bypassAllocator, &barrier),
                 k_NUM_THREADS);
        tg.joinAll();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "bdlma::HeapBypassAllocator usage example\n"
                             "========================================\n";

///Usage
///-----
// Here we allocate some memory using a heap bypass allocator, then write to
// that memory, then read from it and verify the values written are preserved.
//..
    {
        enum {
            k_LENGTH       = 10 * 1000,
            k_NUM_SEGMENTS = 60
        };

        bdlma::HeapBypassAllocator hbpa;
//..
// First, we allocate some segments:
//..
        char *segments[k_NUM_SEGMENTS];
        for (int i = 0; i < k_NUM_SEGMENTS; ++i) {
            segments[i] = static_cast<char *>(hbpa.allocate(k_LENGTH));
            BSLS_ASSERT(segments[i]);
        }
//..
// Next, we write to the segments:
//..
        char c = 'a';
        for (int i = 0; i < k_NUM_SEGMENTS; ++i) {
            char *segment = segments[i];
            for (int j = 0; j < k_LENGTH; ++j) {
                c = (c + 1) & 0x7f;
                segment[j] = c;
            }
        }
//..
// Finally, we read from the segments and verify the written data is still
// there:
//..
        c = 'a';
        for (int i = 0; i < k_NUM_SEGMENTS; ++i) {
            char *segment = segments[i];
            for (int j = 0; j < k_LENGTH; ++j) {
                c = (c + 1) & 0x7f;
                BSLS_ASSERT(segment[j] == c);  (void)segment;
            }
        }
//..
// Memory is released upon destruction of object 'hbpa' when it goes out of
// scope.
//..
    }
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 Exercise heap bypass allocator basic functionality.
        //
        // Plan:
        //: 1 Create a heap bypass allocator, do some allocations, then destroy
        //:   the object.  (C-1)
        //:
        //: 2 Verify that allocating 0 bytes results in null.  (C-1)
        //:
        //: 3 Verify that 'replenishHint' is not ignored.  (C-1)
        //:
        //: 4 Verify that allocating just beyond the initial chunk size works.
        //:   (C-1)
        //:
        //: 5 Verify a single call to 'allocate' that exceeds the suggested
        //:   chunk size is able to be satisfied.  (C-1)
        //
        // Testing
        //   CONCERN: The object is able to perform basic allocations.
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        enum { k_LENGTH = 10 * 1000, k_NUM_SEGMENTS = 20 };

        bdlma::HeapBypassAllocator hbpa;

        char *segments[k_NUM_SEGMENTS];
        for (int i = 0; i < k_NUM_SEGMENTS; ++i) {
            segments[i] = static_cast<char *>(hbpa.allocate(k_LENGTH));
            char *pcB = segments[i];

            const char C = static_cast<char>('a' + i);
            for (char *pcE = pcB + k_LENGTH, *pc = pcB; pc < pcE; ++pc) {
                *pc = C;
            }
        }

        for (int i = 0; i < k_NUM_SEGMENTS; ++i) {
            const char *pcB = segments[i];

            const char C = static_cast<char>('a' + i);
            for (const char *pcE = pcB + k_LENGTH, *pc = pcB; pc < pcE; ++pc) {
                ASSERT(C == *pc);
            }
        }

        {
            // Allocate no bytes
            bdlma::HeapBypassAllocator allocator(4096);
            ASSERT(!allocator.allocate(0));
        }

        {
            // Allocate one page plus one byte
            bdlma::HeapBypassAllocator  allocator(4096);
            void                       *buf0 = allocator.allocate(4096);
            void                       *buf1 = allocator.allocate(1);
            memset(buf0, 0xcc, 4096);
            memset(buf1, 0xdd, 1);

            char control[4096];
            memset(control, 0xcc, sizeof(control));
            ASSERT(0 == memcmp(buf0, control, sizeof(control)));
        }

        {
            // Allocate one page plus one byte accounting for bookkeeping
            bdlma::HeapBypassAllocator  allocator(4096);
            void                       *buf0 = allocator.allocate(4096 - 16);
            void                       *buf1 = allocator.allocate(1);
            memset(buf0, 0xcc, 4096 - 16);
            memset(buf1, 0xdd, 1);

            char control[4096 - 16];
            memset(control, 0xcc, sizeof(control));
            ASSERT(0 == memcmp(buf0, control, sizeof(control)));
        }

        {
            // Ensure 'replenishHint' is not ignored
            bdlma::HeapBypassAllocator allocator0(64000);
            bdlma::HeapBypassAllocator allocator1(64000);

            char *buf0 = static_cast<char *>(allocator0.allocate(32000));
            allocator1.allocate(32000);
            char *buf1 = static_cast<char *>(allocator0.allocate(32000));
            char *buf2 = static_cast<char *>(allocator0.allocate(32000));
            char *buf3 = static_cast<char *>(allocator0.allocate(32000));
            ASSERT(buf0 + 32000 == buf1);
            ASSERT(buf1 + 32000 != buf2);  // non-contiguous
            ASSERT(buf2 + 32000 == buf3);
        }

        {
            // Ensure we can allocate more than the suggested chunk size
            bdlma::HeapBypassAllocator  allocator(4096);
            void                       *buf = allocator.allocate(4 * 4096);
            ASSERT(buf);

            bsl::vector<unsigned char> control(4 * 4096, 0xaa);
            memset(buf, 0xaa, 4 * 4096);
            ASSERT(0 == memcmp(buf, control.data(), control.size()));
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
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
