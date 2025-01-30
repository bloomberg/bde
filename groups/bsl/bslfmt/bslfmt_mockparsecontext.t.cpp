// bslfmt_mockparsecontext.t.cpp                                      -*-C++-*-
#include <bslfmt_mockparsecontext.h>

#include <bsls_bsltestutil.h>
#include <bsls_util.h>  // Testing only

#include <bslstl_string.h>

#include <stdio.h>

using namespace BloombergLP;


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

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int  test    = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 Use mock parse context to parse a format specification.
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
///Example: Simulating format specification parsing
/// - - - - - - - - - - - - - - - - - - - - - - - -
// `MockParseContext` is designed to imitate the behavior of the
// `basic_format_parse_context`. Therefore, let's look at the examples of
// real-life context usage (`FormatSpecificationParser::parse()`), repeat
// these steps and see the results.
//
// First, create a `MockParseContext` based on the defined specification:
// ```
    typedef bslfmt::MockParseContext<char> ParseContext;
    typedef ParseContext::const_iterator   ContextIterator;

    const char   *formatSpecification       = "*<5x";
    const int     formatSpecificationLength = 4;
    const int     numArgs                   = 1;
    ParseContext  mpc(formatSpecification, numArgs);
// ```
// Next call basic accessors:
// ```
    ContextIterator current = mpc.begin();
    ContextIterator end     = mpc.end();

    ASSERT(formatSpecification == BSLS_UTIL_ADDRESSOF(*current));
    ASSERT(current + formatSpecificationLength == end);
    // ```
// Now check the work with arguments:
// ```
    mpc.check_arg_id(0);
    ASSERT(ParseContext::e_MANUAL == mpc.indexingMode());
// ```
// Finally, advance our mock context:
// ```
    ++current;
    ASSERT(current != mpc.begin());
    mpc.advance_to(current);
    ASSERT(current == mpc.begin());
// ```
//
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
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        if (verbose) printf("\tMockParseContext\n");
        {
            typedef  bslfmt::MockParseContext<char>       CharMockParseContext;
            typedef  CharMockParseContext::const_iterator ContextConstIterator;

            bsl::string_view format = "{:}";
            size_t           numArgs = 2;

            bslfmt::MockParseContext<char> mockParseContext(format, numArgs);
            ASSERTV(mockParseContext.indexingMode(),
                    CharMockParseContext::e_UNKNOWN ==
                        mockParseContext.indexingMode());
            ContextConstIterator contextIt = mockParseContext.begin();

            ASSERTV(format.begin() == contextIt);

            contextIt = mockParseContext.end();

            ASSERTV(format.end() == contextIt);

            ContextConstIterator formatIt = format.begin();
            ++formatIt;
            mockParseContext.advance_to(formatIt);

            ASSERTV(formatIt == mockParseContext.begin());

#ifdef BDE_BUILD_TARGET_EXC
            try {
                bslfmt::MockParseContext<char> mockParseContext1(format, 0);
                ASSERTV(0 == mockParseContext1.next_arg_id());
                ASSERTV("Exception has not been thrown", false);
            }
            catch(const bsl::format_error& err) {
            }

            try {
                bslfmt::MockParseContext<char> mockParseContext2(format, 1);
                ASSERTV(0 == mockParseContext2.next_arg_id());
                ASSERTV(mockParseContext2.indexingMode(),
                        CharMockParseContext::e_AUTOMATIC ==
                        mockParseContext2.indexingMode());
                mockParseContext2.check_arg_id(1);
                ASSERTV("Exception has not been thrown", false);
            }
            catch(const bsl::format_error& err) {
            }

            try {
                bslfmt::MockParseContext<char> mockParseContext3(format, 1);
                mockParseContext3.check_arg_id(1);
                ASSERTV(mockParseContext3.indexingMode(),
                        CharMockParseContext::e_MANUAL ==
                        mockParseContext3.indexingMode());
                ASSERTV(0 == mockParseContext3.next_arg_id());
                ASSERTV("Exception has not been thrown", false);
            }
            catch(const bsl::format_error& err) {
            }

            try {
                bslfmt::MockParseContext<char> mockParseContext1(format, 1);
                ASSERTV(0 == mockParseContext1.next_arg_id());
                mockParseContext1.check_arg_id(1);
                ASSERTV("Exception has not been thrown", false);
            }
            catch(const bsl::format_error& err) {
            }
#endif
        }
      } break;
      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        printf("Error, non-zero test status = %d .\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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

