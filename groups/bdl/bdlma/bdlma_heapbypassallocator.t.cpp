// bdlma_heapbypassallocator.t.cpp                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlma_heapbypassallocator.h>

#include <bslim_testutil.h>

#include <bsl_iostream.h>

#include <bsl_cstdlib.h>    // 'atoi'

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

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "bdlma::HeapBypassAllocator usage example\n"
                             "=======================================\n";

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
                BSLS_ASSERT(segment[j] == c);
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
        // bdlma::HeapBypassAllocator BREATHING TEST
        //
        // Concerns:
        //   Exercise heap bypass allocator basic functionality.
        //
        // Plan:
        //   Create a heap bypass allocator, do some allocations, then destroy
        //   the object.
        // --------------------------------------------------------------------

        if (verbose) cout << "bdlma::HeapBypassAllocator breathing test\n"
                             "========================================\n";

        enum {
            k_LENGTH = 10 * 1000,
            k_NUM_SEGMENTS = 20
        };

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
