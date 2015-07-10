// bslalg_dequeimputil.t.cpp                                          -*-C++-*-

#include <bslalg_dequeimputil.h>

#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides an extremely simple 'struct' with only types and
// integral 'enum' values.  The concerns for testing are merely that the
// computation of 'BLOCK_LENGTH' be correct and that the names are as expected.
//-----------------------------------------------------------------------------
// [ 2] 'bslalg::DequeImp::BLOCK_LENGTH'
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'bslstl::DequeImpUtil' TEMPLATE
        //
        // Concerns:  That all the names are defined.
        //
        // Plan:  Since the component under test is a template, we cannot use
        //   the table data selection method, but instead use a test macro,
        //   with two parameters containing the value of the template
        //   arguments.  We then instantiate the macro for various parameters
        //   that exercise the various branches and boundary conditions.
        //
        // Testing:
        //   class template 'bslstl::DequeImp'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bslalg::DequeImp"
                            "\n=========================\n");

#define TEST_DEQUE_IMP(LINE_NUM, VALUE_TYPE, BLOCK_LENGTH)  {                \
                const int LINE               = LINE_NUM;                     \
                                                                             \
                typedef bslalg::DequeImpUtil<VALUE_TYPE,                     \
                                            BLOCK_LENGTH> Obj;               \
                                                                             \
                LOOP_ASSERT(LINE, (bsl::is_same<VALUE_TYPE,                 \
                                                Obj::ValueType>::value));    \
                                                                             \
                LOOP_ASSERT(LINE, Obj::BLOCK_BYTES == sizeof(Obj::Block));   \
                LOOP_ASSERT(LINE, (bsl::is_same<Obj::BlockPtr,              \
                                               Obj::Block *>::value));       \
            }

        TEST_DEQUE_IMP(L_, char, 1);
        TEST_DEQUE_IMP(L_, char, 4096);

        TEST_DEQUE_IMP(L_, void *, 1);
        TEST_DEQUE_IMP(L_, void *, sizeof(void *));
        TEST_DEQUE_IMP(L_, void *, 200);
        TEST_DEQUE_IMP(L_, void *, 4095);
        TEST_DEQUE_IMP(L_, void *, 4096);

        typedef char XXL[4096];
        TEST_DEQUE_IMP(L_, XXL, 1);
        TEST_DEQUE_IMP(L_, XXL, 200);
        TEST_DEQUE_IMP(L_, XXL, 4095);
        TEST_DEQUE_IMP(L_, XXL, sizeof(XXL));
        TEST_DEQUE_IMP(L_, XXL, 8192);
        TEST_DEQUE_IMP(L_, XXL, 16384);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //   That the basic usage is functional and correct.
        //
        // Plan:  Exercise basic usage of this component.
        //
        // Testing:
        //   This test exercises basic usage but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============");

        enum { BLOCK_LENGTH = 200 };
        typedef bslalg::DequeImpUtil<char, BLOCK_LENGTH> Obj;

        Obj::Block block[4];
        memset(block[0].d_data, 'A', BLOCK_LENGTH);
        memset(block[1].d_data, 'B', BLOCK_LENGTH);
        memset(block[2].d_data, 'C', BLOCK_LENGTH);
        memset(block[3].d_data, 'D', BLOCK_LENGTH);

        ASSERT(200 == Obj::BLOCK_BYTES);

        Obj::BlockPtr blockPtr = &block[0];
        ASSERT('A' == blockPtr->d_data[0]);
        ++blockPtr;
        ASSERT('B' == blockPtr->d_data[1]);
        ++blockPtr;
        ASSERT('C' == blockPtr->d_data[2]);
        ++blockPtr;
        ASSERT('D' == blockPtr->d_data[3]);

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
