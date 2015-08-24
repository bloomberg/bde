// bdlqq_threadargument.t.cpp
#include <bdlqq_threadargument.h>

#include <bsltf_alloctesttype.h>  
#include <bsltf_simpletesttype.h>  

#include <bsls_asserttest.h>   
#include <bsls_bsltestutil.h>  
#include <bslma_testallocator.h> 
#include <bslma_default.h>

#include <stdio.h>   // printf

using namespace BloombergLP;
using namespace std;

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
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int             test = argc > 1 ? atoi(argv[1]) : 0;
    const bool         verbose = argc > 2;
    const bool     veryVerbose = argc > 3;
    const bool veryVeryVerbose = argc > 4;

    bslma::TestAllocator globalAllocator("global", veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // ALLOCATOR-USING TYPE
        //
        // Concerns: (all for parameterized TYPEs using 'bslma::Allocator')
        //: 1 The original value is copied
        //: 2 The provided allocator is stored
        //: 3 Accessors are const
        //: 4 Memory is allocated from the specified allocator and not leaked
        //:
        //
        // Testing:
        //   Parameterized types that do use bslma::Allocator
        // --------------------------------------------------------------------
        bslma::TestAllocator testAllocator("test", veryVeryVerbose);

        enum {
            TEST_VALUE = 999
        };
        {
            bsltf::AllocTestType data(TEST_VALUE, &testAllocator);
            bdlqq::ThreadArgument<bsltf::AllocTestType> mX(data,
                                                          &testAllocator);
            const bdlqq::ThreadArgument<bsltf::AllocTestType>& X = mX;
            
            ASSERTV(X.object()->data(),
                    TEST_VALUE == X.object()->data());
            ASSERTV(X.object()->allocator(),
                    &testAllocator == X.allocator());
        }
        
        ASSERTV(testAllocator.numBlocksTotal(),
                0 != testAllocator.numBlocksTotal());
        ASSERTV(testAllocator.numBytesInUse(),
                0 == testAllocator.numBytesInUse());
      } break;

    case 1: {
        // --------------------------------------------------------------------
        // NON-ALLOCATOR-USING TYPE
        //
        // Concerns: (all for parameterized TYPEs not using 'bslma::Allocator')
        //: 1 The original value is copied
        //: 2 The provided allocator is stored
        //: 3 Accessors are const
        //: 4 No memory should be used or leaked
        //:
        //
        // Testing:
        //   Parameterized types that do not use bslma::Allocator
        // --------------------------------------------------------------------
        bslma::TestAllocator testAllocator("test", veryVeryVerbose);

        enum {
            TEST_VALUE = 999
        };
        bsltf::SimpleTestType data(TEST_VALUE);
        
        bdlqq::ThreadArgument<bsltf::SimpleTestType> mX(data, &testAllocator);
        const bdlqq::ThreadArgument<bsltf::SimpleTestType>& X = mX;

        ASSERTV(X.object()->data(),
                TEST_VALUE == X.object()->data());
        ASSERTV(X.allocator(),
                &testAllocator == X.allocator());
        
        ASSERTV(testAllocator.numBlocksTotal(),
                0 == testAllocator.numBlocksTotal());
      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // No memory should be ever used by the global or default allocators in
    // this test driver.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());
    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
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
