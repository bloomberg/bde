// bslh_hashpair.t.cpp                                                -*-C++-*-
#include <bslh_hashpair.h>

#include <bslh_defaulthashalgorithm.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_types.h>

#include <assert.h>
#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'realloc', 'free'
#include <string.h>  // 'memcmp', 'memcpy'

using namespace BloombergLP;
using namespace bslh;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
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
// ----------------------------------------------------------------------------
//
// FREE FUNCTIONS
// [ 2] void hashAppend(HASHALG& algorithm, const std::pair<T1, T2>&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE

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

// ============================================================================
//          GLOBAL TYPEDEFS, HELPER FUNCTIONS, AND CLASSES FOR TESTING
// ----------------------------------------------------------------------------

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

    size_t getLength() const
        // Return the length of the stored data.
    {
        return d_length;
    }
};

// FREE OPERATORS
bool operator==(MockAccumulatingHashingAlgorithm& lhs,
                MockAccumulatingHashingAlgorithm& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value and
    // 'false' otherwise.  Two objects have the same value if they both have
    // the same length and if they both point to data that compares equal.
    // Note that for simplicity of implementation the arguments are
    // (unconventionally) non-'const'.
{
    if (lhs.getLength() != rhs.getLength()) {
        return false;                                                 // RETURN
    }

    return 0 == memcmp(lhs.getData(), rhs.getData(), rhs.getLength());
}

namespace my {

class MyType {

    const char *d_value_p;
    size_t      d_length;

  public:
    // CREATORS
    explicit MyType(const char *value)
        // Create a 'MyType' object that holds the address of the specified
        // '\0'-terminate 'value'.
    : d_value_p(value)
    {
        assert(value);
        d_length = strlen(value);
    }

    // ACCESSORS
    size_t length() const
        // Return the length of the '\0'-terminated string supplied on
        // construction.
    {
        return d_length;
    }

    const char *value() const
        // Return the address of the '\0'-terminated string supplied on
        // construction.
    {
        return d_value_p;
    }

};

// FREE FUNCTIONS
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& algorithm, const MyType& input)
    // Pass the contents of (address and length) of the specified 'input',
    // which holds a '\0'-terminated string, to the specified 'algorithm'.
{
    algorithm(input.value(), input.length());
}

}  // close namespace my

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
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Hashing a Pair of Integer Values
/// - - - - - - - - - - - - - - - - - - - - - -
// Suppose one must compute has that combines the hashes of several integer
// values, each of a different type:
//..
    char  c = 'a';
    short s = static_cast<short>(1);
//..
// First, we can make that calculation by repeated invocations of a
// 'bslh::DefaultHashAlogorithm object:
//..
    bslh::DefaultHashAlgorithm hasherS;
    hasherS(&c, sizeof(char));
    hasherS(&s, sizeof(short));

    bslh::DefaultHashAlgorithm::result_type hashS = hasherS.computeHash();
//..
// Now, the same calculation can also be be made if those same values are
// contained in a single 'std::pair' object.
//..
    std::pair<char, short> t = std::make_pair(c, s);

    bslh::DefaultHashAlgorithm hasherT;
    bslh::hashAppend(hasherT, t);

    bslh::DefaultHashAlgorithm::result_type hashT = hasherT.computeHash();
//..
// Finally, we confirm that we computed the same result.
//..
    ASSERT(hashS == hashT);
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'hashAppend'
        //   Verify that the 'hashAppend' free functions have been implemented
        //   for all of the fundamental types and do not truncate or pass extra
        //   data into the algorithms.
        //
        // Concerns:
        //: 1 The 'hashAppend' function ("the function") uses the specified
        //:   algorithm.
        //:
        //: 2 The function visits each element of the pair once and in the
        //:   order of their placement in the pair ('first' then 'second').
        //:
        //: 3 The function works for pairs consisting of heterogeneous types,
        //:   including a user-defined type that defines as custom 'hashAppend'
        //:   function in its namespace.
        //
        // Plan:
        //: 1 Use a locally defined algorithm class,
        //:   'MockAccumulatingHashingAlgorithm', to show that 'hashAppend'
        //:   uses the supplied algorithm object.  (C-1)
        //:
        //: 2 The 'MockAccumulatingHashingAlgorithm' functor stores a
        //:   concatenation of each of the input supplied.  This allows one to
        //:   compare the results of separate invocations of the algorithm on
        //:   the individual pair elements with the result of single invocation
        //:   of the algorithm on the analogous pair.
        //:
        //: 4 In P-2 use elements of different values.  Any omission,
        //:   duplication, or mis-ordering of element visits would become
        //:   manifest when comparing the concatenations of the two
        //:   computations.  (C-2)
        //:
        //: 5 Use the function to compute the hash of a pair an integer type
        //:   and the locally defined class 'my::MyType'.  As before, compare
        //:   the concatenated results of hashing the individual elements with
        //:   that of hashing the pair.  Additionally, check that the
        //:   concatenation shows the expected result of the locally defined
        //:   'hashAppend' defined for 'my::MyType'.  (C-3)
        //
        // Testing:
        //   void hashAppend(HASHALG& algorithm, const std::pair<T1, T2>&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'hashAppend'"
                            "\n====================\n");

        if (veryVerbose) printf("\tpair of integers\n");
        {
            typedef std::pair<int, int> PairInts;

            PairInts pairInts = std::make_pair(0, 1);

            MockAccumulatingHashingAlgorithm expd;

            int elem0 = pairInts.first;
            int elem1 = pairInts.second;

            hashAppend(expd, elem0);
            hashAppend(expd, elem1);

            MockAccumulatingHashingAlgorithm calc;

            hashAppend(calc, pairInts);

            ASSERT(expd == calc);
        }

        if (veryVerbose) printf("\theterogeneous pair\n");
        {
            typedef std::pair<long, my::MyType> PairMixed;

            PairMixed pairMixed = std::make_pair(1L, my::MyType("Two"));

            MockAccumulatingHashingAlgorithm expd;

            long       elem0 = pairMixed.first;
            my::MyType elem1 = pairMixed.second;

            hashAppend(expd, elem0);
            hashAppend(expd, elem1);

            MockAccumulatingHashingAlgorithm calc;

            hashAppend(calc, pairMixed);

            ASSERT(expd == calc);

            size_t offset = sizeof(long);

            ASSERT(0 == memcmp("Two",
                               expd.getData() + offset, strlen("Two")));
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
        //: 1 Create an instance of 'bslh::Hash<>'.  (C-1)
        //:
        //: 2 Verify different hashes are produced for different pairs.  (C-1)
        //:
        //: 3 Verify that a change in any element of a pair changes the hash
        //:   of the pair.  (C-1)
        //:
        //: 4 Verify that the position of a value in the pair changes the hash
        //:   of the pair.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslh::Hash<> hasher;

        ASSERT(hasher(std::make_pair(0, 1))
            == hasher(std::make_pair(0, 1)));  // Baseline value

        ASSERT(hasher(std::make_pair(0, 1))
            != hasher(std::make_pair(9, 1)));  // Change in 'first'

        ASSERT(hasher(std::make_pair(0, 1))
            != hasher(std::make_pair(0, 9)));  // Change in 'second'

        ASSERT(hasher(std::make_pair(0, 1))
            != hasher(std::make_pair(1, 0)));  // Order matters.

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
// Copyright 2021 Bloomberg Finance L.P.
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
