// bslfmt_mockformatcontext.t.cpp                                     -*-C++-*-
#include <bslfmt_mockformatcontext.h>

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <stdio.h>
#include <string.h>  // `strlen`

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
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example: Testing an integer formatter
///- - - - - - - - - - - - - - - - - - -
// Suppose we want to test `format` function of some formatter that meets
// `BasicFormatter` requirements.  For example a formatter that formats integer
// values:
// ```
    template <class t_VALUE>
    class IntegerFormatter {
      public:
        // CREATORS

        /// Create a formatter object.
        IntegerFormatter();

        // MANIPULATORS

        /// Parse the specified `parseContext` and return an iterator, pointing
        /// to the end of the format string.
        template <class t_PARSE_CONTEXT>
        typename t_PARSE_CONTEXT::iterator parse(
                                                t_PARSE_CONTEXT& parseContext);

        // ACCESSORS

        /// Create string representation of the specified `value`, customized
        /// in accordance with the requested format and the specified
        /// `formatContext`, and copy it to the output that the output iterator
        /// of the `formatContext` points to.
        template <class t_FORMAT_CONTEXT>
        typename t_FORMAT_CONTEXT::iterator format(
                                        t_VALUE           value,
                                        t_FORMAT_CONTEXT& formatContext) const;
    };
// ```

                         // ----------------------
                         // class IntegerFormatter
                         // ----------------------

// CREATORS
template <class t_VALUE>
IntegerFormatter<t_VALUE>::IntegerFormatter()
{}

// MANIPULATORS
template <class t_VALUE>
template <class t_PARSE_CONTEXT>
typename t_PARSE_CONTEXT::iterator IntegerFormatter<t_VALUE>::parse(
                                                 t_PARSE_CONTEXT& parseContext)
{
    return parseContext.end();
};

// ACCESSORS
template <class t_VALUE>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator IntegerFormatter<t_VALUE>::format(
                                         t_VALUE           value,
                                         t_FORMAT_CONTEXT& formatContext) const
{
    (void)value;  // suppress compiler warning

    typename t_FORMAT_CONTEXT::iterator outIterator = formatContext.out();

    const char   *result       = "2a***";
    const size_t  resultLength = std::strlen(result);

    outIterator = bsl::copy(result, result + resultLength, outIterator);
    return outIterator;
}

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
        //: 1 Use test contexts to format a single string.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

// First, we create an object of our formatter:
// ```
    IntegerFormatter<int> formatter;
// ```
// Next, we specify a value to format and define expected result of formatting.
// In this example we will skip the spec parsing step, but let's say we want to
// format the number `42` with the following spec: "*<5x".
// ```
    const int     value                = 42;
    const char   *expectedResult       = "2a***";
    const size_t  expectedResultLength = std::strlen(expectedResult);
// ```
// Now create a `MockFormatContext` and format the value using our formatter:
// ```
    typedef bslfmt::MockFormatContext<char> FormatContext;

    FormatContext           mfc(value);
    FormatContext::iterator begin  = mfc.out();

    mfc.advance_to(bsl::as_const(formatter).format(value, mfc));
    FormatContext::iterator end  = mfc.out();
// ```
// Finally, verify that `format` function returns the correct past-the-end
// iterator and produces the expected result string:
// ```
    const size_t actualResultLength = static_cast<size_t>(end.rawPointer() -
                                                          begin.rawPointer());
    ASSERT(expectedResultLength == actualResultLength);
    ASSERT(expectedResult       == mfc.finalString());
// ```
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

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("\tTesting `MockFormatContext_Iterator`.\n");
        {
            char arr[] = {0, 1, 2, 3, 4};
            int  size  = static_cast<int>(sizeof arr / sizeof *arr);

            bslfmt::MockFormatContext_Iterator<char> preIt(arr,
                                                           arr + size);

            for (int i = 0; i < size - 1; ++i, ++preIt) {
                ASSERTV((int)arr[i], (int)*preIt,  arr[i] == *preIt);
                ASSERTV(arr + i, preIt.rawPointer(),
                        arr + i == preIt.rawPointer());
            }

            bslfmt::MockFormatContext_Iterator<char> itPost(arr,
                                                            arr + size);

            for (int i = 0; i < size - 1; ++i) {
                bslfmt::MockFormatContext_Iterator<char> copy(itPost);
                ASSERTV((int)arr[i], (int)*copy,  arr[i] == *(itPost++));
            }

#ifdef BDE_BUILD_TARGET_EXC
            try {
                ++preIt;
                ASSERTV("Exception has not been thrown", false);
            }
            catch(const bsl::format_error& err) {
            }
#endif
        }

        if (verbose) printf("\tTesting `MockFormatContext`.\n");
        {
            typedef bslfmt::MockFormatContext<char>     CharMockFormatContext;
            typedef bslfmt::MockFormatContext_Iterator<char>
                                                        Iterator;

            int                   arg0 = 1;
            CharMockFormatContext mockFormatContext(arg0);

            ASSERTV( mockFormatContext.arg(0));
            ASSERTV(!mockFormatContext.arg(1));
            ASSERTV(!mockFormatContext.arg(2));
            ASSERTV(!mockFormatContext.arg(3));
            ASSERTV(!mockFormatContext.arg(4));

            bsl::string_view finalString = mockFormatContext.finalString();
            ASSERTV(finalString.length(), 0 == finalString.length());
            Iterator it = mockFormatContext.out();
            ++it;
            mockFormatContext.advance_to(it);
            finalString = mockFormatContext.finalString();

            ASSERTV(finalString.length(), 1 == finalString.length());

            char                   arg1 = 1;
            CharMockFormatContext mockFormatContext1(arg0, arg1);

            ASSERTV( mockFormatContext1.arg(0));
            ASSERTV( mockFormatContext1.arg(1));
            ASSERTV(!mockFormatContext1.arg(2));
            ASSERTV(!mockFormatContext1.arg(3));
            ASSERTV(!mockFormatContext1.arg(4));

            float                 arg2 = 1;
            CharMockFormatContext mockFormatContext2(arg0, arg1, arg2);

            ASSERTV( mockFormatContext2.arg(0));
            ASSERTV( mockFormatContext2.arg(1));
            ASSERTV( mockFormatContext2.arg(2));
            ASSERTV(!mockFormatContext2.arg(3));
            ASSERTV(!mockFormatContext2.arg(4));


            bsl::string_view      arg3 = "";
            CharMockFormatContext mockFormatContext3(arg0, arg1, arg2, arg3);

            ASSERTV( mockFormatContext3.arg(0));
            ASSERTV( mockFormatContext3.arg(1));
            ASSERTV( mockFormatContext3.arg(2));
            ASSERTV( mockFormatContext3.arg(3));
            ASSERTV(!mockFormatContext3.arg(4));
        }

        if (verbose)
            printf("\tTesting `MockFormatContext::formatter_type`.\n");
        {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
            typedef bslfmt::MockFormatContext<char>     CharMockFormatContext;
            typedef bslfmt::MockFormatContext<wchar_t> WcharMockFormatContext;
            ASSERT(true ==
                   (bsl::is_same<
                       bsl::formatter<int, char>,
                       CharMockFormatContext::formatter_type<int> >::value));
            ASSERT(
                  true ==
                  (bsl::is_same<
                      bsl::formatter<float, wchar_t>,
                      WcharMockFormatContext::formatter_type<float> >::value));
#else
            if (verbose)
                printf("\t\t`MockFormatContext::formatter_type` is not "
                       "supported.\n");
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

