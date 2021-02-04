// bslh_hashvariant.t.cpp                                             -*-C++-*-
#include <bslh_hashvariant.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>
#include <bsls_types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace bslh;


//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test is a standards-conformant hashing algorithm
// functor.  The component will be tested for conformance to the interface
// requirements on 'std::hash', outlined in the C++ Standard.  The output of
// the component will also be tested to check that it matches the expected
// output of the underlying hashing algorithms.  This component also contains
// 'hashAppend' free functions written for fundamental types.  These free
// functions will be tested to ensure they properly pass data into the hashing
// algorithms they are given.
//-----------------------------------------------------------------------------
//
// FREE FUNCTIONS
// [ 2] void hashAppend(HASHALG& algorithm, const std::variant<TYPE...>&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
//          GLOBAL TYPEDEFS, HELPER FUNCTIONS, AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class MockAccumulatingHashingAlgorithm {
    // This class implements a mock hashing algorithm that provides a way to
    // accumulate and then examine data that is being passed into hashing
    // algorithms by 'hashAppend'.

    void   *d_data_p;  // Data we were asked to hash
    size_t  d_length;  // Length of the data we were asked to hash

  public:
    MockAccumulatingHashingAlgorithm()
    : d_data_p(0)
    , d_length(0)
        // Create an object of this type.
    {
    }

    ~MockAccumulatingHashingAlgorithm()
        // Destroy this object
    {
        free(d_data_p);
    }

    void operator()(const void *voidPtr, size_t length)
        // Append the data of the specified 'length' at 'voidPtr' for later
        // inspection.
    {
        d_data_p = realloc(d_data_p, d_length += length);
        memcpy(getData() + d_length - length, voidPtr, length);
    }

    char *getData()
        // Return a pointer to the stored data.
    {
        return static_cast<char *>(d_data_p);
    }

    size_t getLength()
        // Return the length of the stored data.
    {
        return d_length;
    }
};

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2; (void)verbose;
    bool         veryVerbose = argc > 3; (void)veryVerbose;
    bool     veryVeryVerbose = argc > 4; (void)veryVeryVerbose;
    bool veryVeryVeryVerbose = argc > 5; (void)veryVeryVeryVerbose;

    ASSERT(true);  // Quiet warnings for non C++-17 compilations.

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Run the usage example (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

///Example 1:  Hashing an integer or floating-point value
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to maintain a value as either an integer or a floating-point
// number, and have it fit within the BDE hash framework.  We can use
// 'std::variant<int, double>' for this, and demonstrate that such a value can
// be correctly hashed.
//
// First, we set up a pair of such optional values to represent the two
// potential states we wish to represent.
//..
    typedef std::variant<int, double> id;
    id variantInt    = 42;
    id variantDouble = 3.475;
//..
// Then, we create a hashing object.
//..
    bslh::Hash<> hasher;
//..
// Next, we hash each of our values.
//..
    size_t variantIntHash    = hasher(variantInt);
    size_t variantDoubleHash = hasher(variantDouble);
//..
// Then we hash the underlying values.  A variant hashes as a pair of values,
// first the 'size_t' index of the held type, then the contained value, so we
// need to accumulate the hashes.
//..
    bslh::Hash<>::HashAlgorithm haInt;
    hashAppend(haInt, size_t(0));
    hashAppend(haInt, 42);
    size_t expectedIntHash = size_t(haInt.computeHash());

    bslh::Hash<>::HashAlgorithm haDouble;
    hashAppend(haDouble, size_t(1));
    hashAppend(haDouble, 3.475);
    size_t expectedDoubleHash = size_t(haDouble.computeHash());
//..
// Finally, we verify that the 'std::variant' hasher produces the same results
// as the underlying hashers.
//..
    ASSERT(expectedIntHash    == variantIntHash);
    ASSERT(expectedDoubleHash == variantDoubleHash);
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'hashAppend'
        //   Verify that the 'hashAppend' free functions have been implemented
        //   for all of the fundamental types and don't truncate or pass extra
        //   data into the algorithms.
        //
        // Concerns:
        //: 1 'hashAppend' has been implemented for std::variant.
        //
        // Plan:
        //: 1 Use a mock hashing algorithm to test 'hashAppend'.
        //
        // Testing:
        //   void hashAppend(HASHALG& algorithm, const std::variant<TYPE...>&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'hashAppend'"
                            "\n====================\n");

        if (verbose) printf("Use a mock hashing algorithm to test that"
                            " 'hashAppend' inputs the variant index"
                            " followed by the variant value.\n");
        {
            std::variant<int, double>        o;
            MockAccumulatingHashingAlgorithm for0;
            MockAccumulatingHashingAlgorithm exp0;
            MockAccumulatingHashingAlgorithm for1;
            MockAccumulatingHashingAlgorithm exp1;

            if (veryVerbose) printf("\tVariant holding integer\n");
            int i = 42;
            o = i;
            hashAppend(for0, o);
            hashAppend(exp0, size_t(0));
            hashAppend(exp0, i);
            ASSERT(for0.getLength() == exp0.getLength());
            ASSERT(0 == memcmp(for0.getData(),
                               exp0.getData(),
                               exp0.getLength()));

            if (veryVerbose) printf("\tVariant holding double\n");
            double d = 3.475;
            o = d;
            hashAppend(for1, o);
            hashAppend(exp1, size_t(1));
            hashAppend(exp1, d);
            ASSERT(for1.getLength() == exp1.getLength());
            ASSERT(0 == memcmp(for1.getData(),
                               exp1.getData(),
                               exp1.getLength()));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create an instance of 'bslh::Hash<>'. (C-1)
        //:
        //: 2 Verify different hashes are produced for different variants.
        //:   (C-1)
        //:
        //: 3 Verify the same hashes are produced for the same variants in the
        //:   same positions. (C-1)
        //:
        //: 4 Verify different hashes are produced for the same variants in
        //:   different positions. (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef std::variant<int, char> Vic;
        typedef std::variant<char, int> Vci;

        bslh::Hash<> hasher;

        ASSERT(hasher(Vic(312)) == hasher(Vic(312)));
        ASSERT(hasher(Vic(312)) != hasher(Vic(999)));
        ASSERT(hasher(Vic('a')) == hasher(Vic('a')));
        ASSERT(hasher(Vic('a')) != hasher(Vic('A')));

        ASSERT(hasher(Vic(312)) != hasher(Vci(312)));
        ASSERT(hasher(Vic('a')) != hasher(Vci('a')));

      } break;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
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
// Copyright 2017 Bloomberg Finance L.P.
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
