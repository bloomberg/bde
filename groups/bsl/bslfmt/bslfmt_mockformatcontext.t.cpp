// bslfmt_mockformatcontext.t.cpp                                     -*-C++-*-
#include <bslfmt_mockformatcontext.h>

#include <bsls_bsltestutil.h>

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
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example: Testing an integer formatter
///- - - - - - - - - - - - - - - - - - -
// Suppose we need to test some formatter meeting `BasicFormatter`
// requirements.
//
// First we define our `IntegerFormatter` template class.  Actually the
// implementation features are not of particular importance.  In this case,
// what is important to us is that the class contains `parse` and `format`
// methods with the expected interfaces. In place of this class, we can
// always substitute one of the existing `bsl::formatter` specializations, such
// as `bsl::formatter<long, char>` or `bsl::formatter<bool, wchar_t>`.
// ```
    template <class t_VALUE>
    class IntegerFormatter {
        // DATA
        bsl::string_view d_resultString;  // pre-defined result string

      public:
        // CREATORS

        /// Creates the formatter having the specified 'resultString'.
        IntegerFormatter(const char* resultString);

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
// Next we define our `ParseContext` template class.  As with the formatter, in
// this example we are interested in the expected interface, not the actual
// implementation:
// ```
    template <class t_CHAR>
    struct ParseContext {
      public:
        // TYPES
        typedef typename bsl::basic_string_view<t_CHAR>::const_iterator
                                                                const_iterator;
        typedef const_iterator                                  iterator;

      private:
        // DATA
        iterator d_begin;  // beginning of the unparsed part of the format spec
        iterator d_end;    // end of the unparsed part of the format spec

        // NOT IMPLEMENTED
        ParseContext(const ParseContext&) BSLS_KEYWORD_DELETED;
        ParseContext& operator=(const ParseContext&) BSLS_KEYWORD_DELETED;

      public:
        // CREATORS
        /// Create an object having the specified `fmt` as a format
        /// specification and the specified `numArgs`.
        explicit ParseContext(bsl::basic_string_view<t_CHAR> fmt,
                              size_t                         numArgs = 0);

        // MANIPULATORS
        /// Update the held iterator to the unparsed portion of the format
        /// string to be the specified `it`. Subsequent calls to `begin` will
        /// return this value.
        BSLS_KEYWORD_CONSTEXPR_CPP20 void advance_to(const_iterator it);

        // ACCESSORS
        /// Return an iterator to the end of the format specification.
        const_iterator end() const;
    };
// ```

                         // ----------------------
                         // class IntegerFormatter
                         // ----------------------

// CREATORS
template <class t_VALUE>
IntegerFormatter<t_VALUE>::IntegerFormatter(const char* resultString)
 : d_resultString(resultString)
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

    outIterator = bsl::copy(d_resultString.data(),
                            d_resultString.data() + d_resultString.size(),
                            outIterator);
    return outIterator;
}

                         // ------------------
                         // class ParseContext
                         // ------------------

// CREATORS
template <class t_CHAR>
ParseContext<t_CHAR>::ParseContext(bsl::basic_string_view<t_CHAR> fmt,
                                   size_t                         numArgs)
: d_begin(fmt.begin())
, d_end(fmt.end())
{
    (void)numArgs;
}

// MANIPULATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 void ParseContext<t_CHAR>::advance_to(
                                                             const_iterator it)
{
    d_begin = it;
}

// ACCESSORS
template <class t_CHAR>
typename ParseContext<t_CHAR>::const_iterator ParseContext<t_CHAR>::end() const
{
    return d_end;
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

// Then, define format specification, value to output and the expected
// operation result:
// ```
    const char *formatSpecification = "*<5x";
    const char *expectedResult      = "2a***";
    const int   value               = 42;
// ```
// Next, create a `ParseContext` object based on the defined specification and
// parse it:
// ```
    ParseContext<char> pc(formatSpecification, 1);

    IntegerFormatter<int> formatter(expectedResult);
    pc.advance_to(formatter.parse(pc));
// ```
// Now create a `MockFormatContext` and format previously specified value using
// our formatter:
// ```
    bslfmt::MockFormatContext<char> mfc(value, 0, 0);

    mfc.advance_to(bsl::as_const(formatter).format(value, mfc));
// ```
// Finally check the resulting string:
// ```
    ASSERT(expectedResult == mfc.finalString());
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

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        if (verbose) printf("\tMockFormatContext_Iterator\n");
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

        if (verbose) printf("\tMockFormatContext\n");
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

