// bslfmt_standardformatspecification.t.cpp                           -*-C++-*-
#include <bslfmt_standardformatspecification.h>

#include <bslfmt_format_string.h>
#include <bslfmt_mockformatcontext.h>
#include <bslfmt_mockparsecontext.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <bslstl_stringview.h>

#include <stdio.h>

using namespace BloombergLP;
using namespace bslfmt;

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------
namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

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

// ============================================================================
//                  ADDITIONAL MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

// Due to a member initialization bug for types returned from consteval
// functions in clang 15, we do the testing on a non-consteval basis for clang
// 15 and earlier.
#if defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP20) &&                 \
    (!defined(BSLS_PLATFORM_CMP_CLANG) || BSLS_PLATFORM_CMP_VERSION >=        \
                                              (16 * 10000))
    #define BSLFMT_FORMATTER_TEST_CONSTEVAL consteval
#else
    #define BSLFMT_FORMATTER_TEST_CONSTEVAL
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&                 \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
    #define BSLFMT_FORMAT_STRING_PARAMETER  bslfmt::format_string<>
    #define BSLFMT_FORMAT_WSTRING_PARAMETER bslfmt::wformat_string<>
#else
    // We cannot define format_string<t_ARGS...> in a C++03 compliant manner,
    // so have to use non-template versions instead.
    #define BSLFMT_FORMAT_STRING_PARAMETER  bslfmt::format_string
    #define BSLFMT_FORMAT_WSTRING_PARAMETER bslfmt::wformat_string
#endif

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef StandardFormatSpecification<char>    CharSpec;
typedef StandardFormatSpecification<wchar_t> WcharSpec;

// ============================================================================
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

BSLFMT_FORMATTER_TEST_CONSTEVAL CharSpec parseStandard(
                             BSLFMT_FORMAT_STRING_PARAMETER inputSpecification,
                             CharSpec::Category             category)
{
    CharSpec                       spec;
    bsl::string_view               input(inputSpecification.get());
    bslfmt::MockParseContext<char> context(input, 4);

    spec.parse(&context, category);

    return spec;
}

BSLFMT_FORMATTER_TEST_CONSTEVAL WcharSpec parseStandard(
                            BSLFMT_FORMAT_WSTRING_PARAMETER inputSpecification,
                            WcharSpec::Category             category)
{
    WcharSpec                         spec;
    bsl::wstring_view                 input(inputSpecification.get());
    bslfmt::MockParseContext<wchar_t> context(input, 4);

    spec.parse(&context, category);

    return spec;
}

void checkStandard(int                                line,
                   const CharSpec&                    originalSpec,
                   bsl::basic_string_view<char>       filler,
                   CharSpec::Alignment                alignment,
                   CharSpec::Sign                     sign,
                   bool                               alternativeFlag,
                   bool                               zeroPaddingFlag,
                   FormatterSpecificationNumericValue postprocessedWidth,
                   FormatterSpecificationNumericValue postprocessedPrecision,
                   bool                               localeSpecificFlag,
                   CharSpec::FormatType               formatType)
{
    CharSpec                        spec = originalSpec;
    bslfmt::MockFormatContext<char> mfc(99, 98, 97, 96);
    spec.postprocess(mfc);

    ASSERTV(line, filler                 == bsl::string_view(
                                                     spec.filler(),
                                                     spec.fillerCharacters()));
    ASSERTV(line, alignment              == spec.alignment());
    ASSERTV(line, sign                   == spec.sign());
    ASSERTV(line, alternativeFlag        == spec.alternativeFlag());
    ASSERTV(line, zeroPaddingFlag        == spec.zeroPaddingFlag());
    ASSERTV(line, postprocessedWidth     == spec.postprocessedWidth());
    ASSERTV(line, postprocessedPrecision == spec.postprocessedPrecision());
    ASSERTV(line, localeSpecificFlag     == spec.localeSpecificFlag());
    ASSERTV(line, formatType             == spec.formatType());
}

void checkStandard(int                                line,
                   const WcharSpec&                   originalSpec,
                   bsl::basic_string_view<wchar_t>    filler,
                   WcharSpec::Alignment               alignment,
                   WcharSpec::Sign                    sign,
                   bool                               alternativeFlag,
                   bool                               zeroPaddingFlag,
                   FormatterSpecificationNumericValue postprocessedWidth,
                   FormatterSpecificationNumericValue postprocessedPrecision,
                   bool                               localeSpecificFlag,
                   WcharSpec::FormatType              formatType)
{
    WcharSpec                          spec = originalSpec;
    bslfmt::MockFormatContext<wchar_t> mfc(99, 98, 97, 96);
    spec.postprocess(mfc);

    ASSERTV(line, filler                 == bsl::wstring_view(
                                                     spec.filler(),
                                                     spec.fillerCharacters()));
    ASSERTV(line, alignment              == spec.alignment());
    ASSERTV(line, sign                   == spec.sign());
    ASSERTV(line, alternativeFlag        == spec.alternativeFlag());
    ASSERTV(line, zeroPaddingFlag        == spec.zeroPaddingFlag());
    ASSERTV(line, postprocessedWidth     == spec.postprocessedWidth());
    ASSERTV(line, postprocessedPrecision == spec.postprocessedPrecision());
    ASSERTV(line, localeSpecificFlag     == spec.localeSpecificFlag());
    ASSERTV(line, formatType             == spec.formatType());
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
        //: 1 Invoke public methods of class being tested and verify the
        //:   results.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef StandardFormatSpecification<char>    CharSpec;
        typedef StandardFormatSpecification<wchar_t> WcharSpec;
        typedef FormatterSpecificationNumericValue   NumericValue;

        checkStandard(L_,
                      parseStandard("", CharSpec::e_CATEGORY_STRING),
                      " ",
                      CharSpec::e_ALIGN_DEFAULT,
                      CharSpec::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(),
                      NumericValue(),
                      false,
                      CharSpec::e_STRING_DEFAULT);

        checkStandard(L_,
                      parseStandard("2.3s", CharSpec::e_CATEGORY_STRING),
                      " ",
                      CharSpec::e_ALIGN_DEFAULT,
                      CharSpec::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_VALUE, 2),
                      NumericValue(NumericValue::e_VALUE, 3),
                      false,
                      CharSpec::e_STRING_DEFAULT);

        checkStandard(L_,
                      parseStandard("{3}.{2}", CharSpec::e_CATEGORY_STRING),
                      " ",
                      CharSpec::e_ALIGN_DEFAULT,
                      CharSpec::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_VALUE, 96),
                      NumericValue(NumericValue::e_VALUE, 97),
                      false,
                      CharSpec::e_STRING_DEFAULT);

        checkStandard(L_,
                      parseStandard("*<{1}.{3}F",
                                    CharSpec::e_CATEGORY_FLOATING),
                      "*",
                      CharSpec::e_ALIGN_LEFT,
                      CharSpec::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_VALUE, 98),
                      NumericValue(NumericValue::e_VALUE, 96),
                      false,
                      CharSpec::e_FLOATING_FIXED_UC);

        checkStandard(L_,
                      parseStandard(L"", WcharSpec::e_CATEGORY_STRING),
                      L" ",
                      WcharSpec::e_ALIGN_DEFAULT,
                      WcharSpec::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(),
                      NumericValue(),
                      false,
                      WcharSpec::e_STRING_DEFAULT);

        checkStandard(L_,
                      parseStandard(L"2.3s", WcharSpec::e_CATEGORY_STRING),
                      L" ",
                      WcharSpec::e_ALIGN_DEFAULT,
                      WcharSpec::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_VALUE, 2),
                      NumericValue(NumericValue::e_VALUE, 3),
                      false,
                      WcharSpec::e_STRING_DEFAULT);

        checkStandard(L_,
                      parseStandard(L"{2}.{1}", WcharSpec::e_CATEGORY_STRING),
                      L" ",
                      WcharSpec::e_ALIGN_DEFAULT,
                      WcharSpec::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_VALUE, 97),
                      NumericValue(NumericValue::e_VALUE, 98),
                      false,
                      WcharSpec::e_STRING_DEFAULT);

        checkStandard(L_,
                      parseStandard(L"*<{0}.{3}f",
                                    WcharSpec::e_CATEGORY_FLOATING),
                      L"*",
                      WcharSpec::e_ALIGN_LEFT,
                      WcharSpec::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_VALUE, 99),
                      NumericValue(NumericValue::e_VALUE, 96),
                      false,
                      WcharSpec::e_FLOATING_FIXED);

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
// Copyright 2025 Bloomberg Finance L.P.
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
