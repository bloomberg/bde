// bslstp_exalgorithm.t.cpp                                           -*-C++-*-
#ifndef BDE_OPENSOURCE_PUBLICATION // STP

#include <bslstp_exalgorithm.h>

#endif  // BDE_OPENSOURCE_PUBLICATION -- STP

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>

#include <bslstl_array.h>
#include <bslstl_stringview.h>
#include <bslstl_vector.h>

#include <cstdio>
#include <cstring>  // memset, strncpy

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides backwards legacy features for backwards
// compatibility with STLPort.
// ----------------------------------------------------------------------------
// [ 1] pair<InputIter,OutputIter> bsl::copy_n(InputIter,Size,OutputIter);

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

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

// ============================================================================
//                         OTHER MACROS
// ----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
#ifndef BDE_OPENSOURCE_PUBLICATION // STP

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::copy_n'
        //
        // Concerns:
        //: 1 The 'copy_n' returns an object of a class that can be implicitly
        //:   converted either to a pair of iterators pointing past the last
        //:   copied element in the input range and past the last copied
        //:   element in the output range or to an output iterator pointing
        //:   past the last copied element in the output range.
        //:
        //: 2 Existing users of 'bsl::copy_n' can be relatively seamlessly
        //:   transferred to the new type of return value.
        //
        // Plan:
        //: 1 Create a couple of char arrays and copy part of the contents of
        //:   one array to another.  Assign returned value to the pair of
        //:   iterators and to the one iterator.  Verify the return value and
        //:   contents of arrays.  (C-1)
        //:
        //: 2 Reproduce our clients' existing code and verify that it supports
        //:   both the old and new return types.  (C-2)
        //
        // Testing:
        //   pair<InputIter,OutputIter> bsl::copy_n(InputIter,Size,OutputIter);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'bsl::copy_n'\n"
                   "\n=====================\n");

        if (verbose) printf("Testing basic behavior.\n");
        {
            const int  MAX_SYMBOLS     = 16;
            const int  NUM_SYMBOLS     = 8;
            const char in[MAX_SYMBOLS] = "hello world";

            // pair
            {
                char               out[MAX_SYMBOLS];
                char * const       EXPECTED_OUT = out + NUM_SYMBOLS;
                char const * const EXPECTED_IN  = in  + NUM_SYMBOLS;
                std::memset(out, 0, MAX_SYMBOLS);

                ASSERT(0 < std::strncmp(in, out, NUM_SYMBOLS));

                bsl::pair<const char *, char *> result = bsl::copy_n(
                                                                   in,
                                                                   NUM_SYMBOLS,
                                                                   out);

                const char *resultIn  = result.first;
                char       *resultOut = result.second;

                ASSERT(EXPECTED_IN  == resultIn);
                ASSERT(EXPECTED_OUT == resultOut);
                ASSERT(0            == std::strncmp(in, out, NUM_SYMBOLS));
                ASSERT(0            == out[NUM_SYMBOLS]);
            }
            // iterator
            {
                char               out[MAX_SYMBOLS];
                char * const       EXPECTED_OUT    = out + NUM_SYMBOLS;
                std::memset(out, 0, MAX_SYMBOLS);

                ASSERT(0 < std::strncmp(in, out, NUM_SYMBOLS));

                char *result = bsl::copy_n(in, NUM_SYMBOLS, out);

                ASSERT(EXPECTED_OUT == result);
                ASSERT(0            == std::strncmp(in, out, NUM_SYMBOLS));
                ASSERT(0            == out[NUM_SYMBOLS]);
            }
            // 'second' field
            {
                char               out[MAX_SYMBOLS];
                char * const       EXPECTED_OUT    = out + NUM_SYMBOLS;
                std::memset(out, 0, MAX_SYMBOLS);

                ASSERT(0 < std::strncmp(in, out, NUM_SYMBOLS));

                char *result = bsl::copy_n(in, NUM_SYMBOLS, out).second;

                ASSERT(EXPECTED_OUT == result);
                ASSERT(0            == std::strncmp(in, out, NUM_SYMBOLS));
                ASSERT(0            == out[NUM_SYMBOLS]);
            }
        }

        if (verbose) printf("Testing clients code.\n");
        {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
            // moaprsvc
            {
                const int CONTAINER_SIZE = 4;

                typedef bsl::array<char, CONTAINER_SIZE> Output;
                typedef Output::iterator                 OutputIterator;

                Output         outputOriginal;
                Output         outputModified;
                char           input[]          = {'a', 'b', 'c', 'd'};
                Output         expected         = {'a', 'b',   0,   0};
                const int      NUM_TO_COPY      = 2;
                OutputIterator expectedOriginal =
                                          outputOriginal.begin() + NUM_TO_COPY;
                OutputIterator expectedModified =
                                          outputModified.begin() + NUM_TO_COPY;

                const auto resultOriginal = bsl::copy_n(
                                                &input[0],
                                                NUM_TO_COPY,
                                                outputOriginal.begin()).second;

                ASSERT(expectedOriginal == resultOriginal);

                bsl::fill(resultOriginal, outputOriginal.end(), 0);
                ASSERT(expected == outputOriginal);


                // Note that we need to explicitly specify the type of the
                // returned value.  Otherwise 'auto' type is deduced as
                // 'CopyNRet' and 'bsl::fill' calling fails to compile.
                const OutputIterator resultModified = bsl::copy_n(
                                                       &input[0],
                                                       NUM_TO_COPY,
                                                       outputModified.begin());

                ASSERT(expectedModified == resultModified);

                bsl::fill(resultModified, outputModified.end(), 0);
                ASSERT(expected == outputOriginal);
            }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

            // datalayer/bbnodeapi
            {
                const int         CONTAINER_SIZE   = 4;
                bsl::string_view  input            = "test";
                char              outputOriginal[] = {'a', 'b', 'c', 'd'};
                char              outputModified[] = {'a', 'b', 'c', 'd'};
                char              expected[]       = {'t', 'e', '!', 'd'};
                const int         NUM_TO_COPY      = 2;
                char             *expectedOriginal =
                                                  outputOriginal + NUM_TO_COPY;
                char             *expectedModified =
                                                  outputModified + NUM_TO_COPY;

                char *resultOriginal = bsl::copy_n(input.cbegin(),
                                                   NUM_TO_COPY,
                                                   outputOriginal).second;

                ASSERT(expectedOriginal == resultOriginal);
                *resultOriginal = '!';

                ASSERT(0 == std::strncmp(expected,
                                         outputOriginal,
                                         CONTAINER_SIZE));

                char *resultModified = bsl::copy_n(input.cbegin(),
                                                   NUM_TO_COPY,
                                                   outputModified);

                ASSERT(expectedModified == resultModified);
                *resultModified = '!';

                ASSERT(0 == std::strncmp(expected,
                                         outputModified,
                                         CONTAINER_SIZE));
            }

            // datalayer/gdxzhs
            {
                typedef bsl::vector<int> Vector;
                typedef Vector::iterator Iterator;

                const int NUM_TO_COPY = 1;
                Vector    input;
                for (int i = 0; i< NUM_TO_COPY; ++i) {
                    input.push_back(i);
                }
                Iterator  inputIt = input.begin();

                Vector    outputOriginal(input.size());
                Vector    outputModified(input.size());
                Iterator  resultOriginal   = outputOriginal.begin();
                Iterator  resultModified   = outputModified.begin();
                Iterator  expectedOriginal =
                                          outputOriginal.begin() + NUM_TO_COPY;
                Iterator  expectedModified =
                                          outputModified.begin() + NUM_TO_COPY;

                for (;
                     inputIt != input.end();
                     inputIt          += NUM_TO_COPY,
                     expectedOriginal += NUM_TO_COPY,
                     expectedModified += NUM_TO_COPY) {
                    resultOriginal = bsl::copy_n(inputIt,
                                                   NUM_TO_COPY,
                                                   resultOriginal).second;
                    resultModified = bsl::copy_n(inputIt,
                                                   NUM_TO_COPY,
                                                   resultModified);
                    ASSERT(expectedOriginal == resultOriginal);
                    ASSERT(expectedModified == resultModified);
                }

                ASSERT(input == outputOriginal);
                ASSERT(input == outputModified);
            }
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-Zero test status = %d.\n", testStatus);
    }

    return testStatus;
}
#else
int main() { return -1; }  // empty test driver
#endif  // BDE_OPENSOURCE_PUBLICATION // STP

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
