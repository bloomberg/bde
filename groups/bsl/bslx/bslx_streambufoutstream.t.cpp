// bslx_streambufoutstream.t.cpp                                      -*-C++-*-

#include <bslx_streambufoutstream.h>
#include <bslx_genericoutstream.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsl_cctype.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bslx;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The primary concerns are that the 'typedef' is correct and that the usage
// example works correctly.
// ----------------------------------------------------------------------------
// [ 1] TYPEDEF
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

const int VERSION_SELECTOR = 20131127;

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Externalization
///- - - - - - - - - - - - - - - -
// A 'bslx::StreambufOutStream' can be used to externalize values in a
// platform-neutral way.  Writing out fundamental C++ types and 'bsl::string'
// requires no additional work on the part of the client; the client can simply
// use the stream directly.  The following code serializes a few representative
// values using a 'bslx::StreambufOutStream', compares the contents of this
// stream to the expected value, and then writes the contents of this stream's
// buffer to 'stdout'.
//
// First, we create a 'bslx::StreambufOutStream' with an arbitrary value for
// its 'versionSelector' and externalize some values:
//..
    bsl::stringbuf           buffer;
    bslx::StreambufOutStream outStream(&buffer, 20131127);
    outStream.putInt32(1);
    outStream.putInt32(2);
    outStream.putInt8('c');
    outStream.putString(bsl::string("hello"));
//..
// Then, we compare the contents of the stream to the expected value:
//..
    bsl::string  theChars = buffer.str();
    ASSERT(15 == theChars.size());
    ASSERT( 0 == bsl::memcmp(theChars.data(),
                             "\x00\x00\x00\x01\x00\x00\x00\x02""c\x05""hello",
                             15));
//..
// Finally, we print the stream's contents to 'bsl::cout'.
//..
    if (veryVerbose)
    for (bsl::size_t i = 0; i < theChars.size(); ++i) {
        if (bsl::isalnum(static_cast<unsigned char>(theChars[i]))) {
            bsl::cout << "nextByte (char): " << theChars[i] << bsl::endl;
        }
        else {
            bsl::cout << "nextByte (int): "
                      << static_cast<int>(theChars[i])
                      << bsl::endl;
        }
    }
//..
// Executing the above code results in the following output:
//..
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 1
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 0
//  nextByte (int): 2
//  nextByte (char): c
//  nextByte (int): 5
//  nextByte (char): h
//  nextByte (char): e
//  nextByte (char): l
//  nextByte (char): l
//  nextByte (char): o
//..
// See the 'bslx_byteinstream' component usage example for a more practical
// example of using 'bslx' streams.

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TYPEDEF
        //   Verify the 'typedef' is correct.
        //
        // Concerns:
        //: 1 The 'typedef' is correct.
        //
        // Plan:
        //: 1 Externalize a few items with 'bslx::StreambufOutStream' (using a
        //:   'bsl::stringbuf') and 'bslx::GenericOutStream', and verify the
        //:   results are the same.
        //
        // Testing:
        //   TYPEDEF
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TYPEDEF" << endl
                          << "=======" << endl;

        bsl::stringbuf                         expected;
        bslx::GenericOutStream<bsl::streambuf> mX(&expected, 20131127);

        bsl::stringbuf           buffer;
        bslx::StreambufOutStream mY(&buffer, 20131127);

        mX.putInt32(3);
        mY.putInt32(3);

        ASSERT(expected.str() == buffer.str());

        mX.putFloat64(3.14);
        mY.putFloat64(3.14);

        ASSERT(expected.str() == buffer.str());

        mX.putString(bsl::string("alpha"));
        mY.putString(bsl::string("alpha"));

        ASSERT(expected.str() == buffer.str());

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
