// bslfmt_formatspecificationparser.t.cpp                             -*-C++-*-
#include <bslfmt_formatspecificationparser.h>

#include <bslfmt_format_arg.h>          // Testing only
#include <bslfmt_format_args.h>         // Testing only
#include <bslfmt_format_string.h>       // Testing only
#include <bslfmt_formatparsecontext.h>  // Testing only
#include <bslfmt_formatterspecificationnumericvalue.h>
#include <bslfmt_formattertestutil.h>   // Testing only

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <stdio.h>
#include <string.h>

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
//                  ASSISTANCE TYPES AND FUNCTIONS
// ----------------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP20
#define BSLFMT_FORMATTER_TEST_CONSTEVAL consteval
#else
#define BSLFMT_FORMATTER_TEST_CONSTEVAL
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&                 \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
#define BSLFMT_FORMAT_STRING_PARAMETER bslfmt::format_string<>
#define BSLFMT_FORMAT_WSTRING_PARAMETER bslfmt::wformat_string<>
#else
// We cannot define format_string<t_ARGS...> in a C++03 compliant manner, so
// have to use non-template versions instead.
#define BSLFMT_FORMAT_STRING_PARAMETER bslfmt::format_string
#define BSLFMT_FORMAT_WSTRING_PARAMETER bslfmt::wformat_string
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

namespace {

typedef FormatSpecificationParser<char>    CharParser;
typedef FormatSpecificationParser<wchar_t> WcharParser;
typedef FormatterSpecificationNumericValue NumericValue;

BSLFMT_FORMATTER_TEST_CONSTEVAL CharParser parseStandard(
                             BSLFMT_FORMAT_STRING_PARAMETER inputSpecification)
{
    const CharParser::Sections sect = static_cast<CharParser::Sections>(
                                   CharParser::e_SECTIONS_FILL_ALIGN |
                                   CharParser::e_SECTIONS_SIGN_FLAG |
                                   CharParser::e_SECTIONS_ALTERNATE_FLAG |
                                   CharParser::e_SECTIONS_ZERO_PAD_FLAG |
                                   CharParser::e_SECTIONS_WIDTH |
                                   CharParser::e_SECTIONS_PRECISION |
                                   CharParser::e_SECTIONS_LOCALE_FLAG |
                                   CharParser::e_SECTIONS_FINAL_SPECIFICATION);

    CharParser                     parser;
    bsl::basic_string_view<char>   input(inputSpecification.get());
    bslfmt::MockParseContext<char> context(input, 3);

    parser.parse(&context, sect);

    return parser;
}

BSLFMT_FORMATTER_TEST_CONSTEVAL WcharParser parseStandard(
                            BSLFMT_FORMAT_WSTRING_PARAMETER inputSpecification)
{
    const WcharParser::Sections sect = static_cast<WcharParser::Sections>(
                                  WcharParser::e_SECTIONS_FILL_ALIGN |
                                  WcharParser::e_SECTIONS_SIGN_FLAG |
                                  WcharParser::e_SECTIONS_ALTERNATE_FLAG |
                                  WcharParser::e_SECTIONS_ZERO_PAD_FLAG |
                                  WcharParser::e_SECTIONS_WIDTH |
                                  WcharParser::e_SECTIONS_PRECISION |
                                  WcharParser::e_SECTIONS_LOCALE_FLAG |
                                  WcharParser::e_SECTIONS_FINAL_SPECIFICATION);

    WcharParser                       parser;
    bsl::basic_string_view<wchar_t>   input(inputSpecification.get());
    bslfmt::MockParseContext<wchar_t> context(input, 3);

    parser.parse(&context, sect);

    return parser;
}

void checkStandard(const CharParser&     originalParser,
                   bsl::string_view      filler,
                   CharParser::Alignment alignment,
                   CharParser::Sign      sign,
                   bool                  alternativeFlag,
                   bool                  zeroPaddingFlag,
                   NumericValue          rawWidth,
                   NumericValue          postprocessedWidth,
                   NumericValue          rawPrecision,
                   NumericValue          postprocessedPrecision,
                   bool                  localeSpecificFlag,
                   bsl::string_view      finalSpec)
{
    CharParser parser = originalParser;

    bslfmt::MockFormatContext<char> context(99, 98, 97);
    parser.postprocess(context);

    ASSERT(filler                 == bsl::string_view(
                                                   parser.filler(),
                                                   parser.fillerCharacters()));
    ASSERT(alignment              == parser.alignment());
    ASSERT(sign                   == parser.sign());
    ASSERT(alternativeFlag        == parser.alternativeFlag());
    ASSERT(zeroPaddingFlag        == parser.zeroPaddingFlag());
    ASSERT(rawWidth               == parser.rawWidth());
    ASSERT(rawPrecision           == parser.rawPrecision());
    ASSERT(postprocessedWidth     == parser.postprocessedWidth());
    ASSERT(postprocessedPrecision == parser.postprocessedPrecision());
    ASSERT(localeSpecificFlag     == parser.localeSpecificFlag());
    ASSERT(finalSpec              == parser.finalSpec());
}

void checkStandard(const WcharParser&     originalParser,
                   bsl::wstring_view      filler,
                   WcharParser::Alignment alignment,
                   WcharParser::Sign      sign,
                   bool                   alternativeFlag,
                   bool                   zeroPaddingFlag,
                   NumericValue           rawWidth,
                   NumericValue           postprocessedWidth,
                   NumericValue           rawPrecision,
                   NumericValue           postprocessedPrecision,
                   bool                   localeSpecificFlag,
                   bsl::wstring_view      finalSpec)
{
    WcharParser parser = originalParser;

    bslfmt::MockFormatContext<wchar_t> context(99, 98, 97);
    parser.postprocess(context);

    ASSERT(filler                 == bsl::wstring_view(
                                                   parser.filler(),
                                                   parser.fillerCharacters()));
    ASSERT(alignment              == parser.alignment());
    ASSERT(sign                   == parser.sign());
    ASSERT(alternativeFlag        == parser.alternativeFlag());
    ASSERT(zeroPaddingFlag        == parser.zeroPaddingFlag());
    ASSERT(rawWidth               == parser.rawWidth());
    ASSERT(rawPrecision           == parser.rawPrecision());
    ASSERT(postprocessedWidth     == parser.postprocessedWidth());
    ASSERT(postprocessedPrecision == parser.postprocessedPrecision());
    ASSERT(localeSpecificFlag     == parser.localeSpecificFlag());
    ASSERT(finalSpec              == parser.finalSpec());
}

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
        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");
        checkStandard(parseStandard(""),
                      " ",
                      CharParser::e_ALIGN_DEFAULT,
                      CharParser::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(),
                      NumericValue(),
                      NumericValue(),
                      NumericValue(),
                      false,
                      "");

        checkStandard(parseStandard("*<06.3XYZ"),
                      "*",
                      CharParser::e_ALIGN_LEFT,
                      CharParser::e_SIGN_DEFAULT,
                      false,
                      true,
                      NumericValue(NumericValue::e_VALUE, 6),
                      NumericValue(NumericValue::e_VALUE, 6),
                      NumericValue(NumericValue::e_VALUE, 3),
                      NumericValue(NumericValue::e_VALUE, 3),
                      false,
                      "XYZ");

        checkStandard(parseStandard("*<{}.{}XYZ"),
                      "*",
                      CharParser::e_ALIGN_LEFT,
                      CharParser::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_ARG_ID, 0 ),
                      NumericValue(NumericValue::e_VALUE,  99),
                      NumericValue(NumericValue::e_ARG_ID, 1 ),
                      NumericValue(NumericValue::e_VALUE,  98),
                      false,
                      "XYZ");

        checkStandard(parseStandard("*<{1}.{2}XYZ"),
                      "*",
                      CharParser::e_ALIGN_LEFT,
                      CharParser::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_ARG_ID, 1 ),
                      NumericValue(NumericValue::e_VALUE,  98),
                      NumericValue(NumericValue::e_ARG_ID, 2 ),
                      NumericValue(NumericValue::e_VALUE,  97),
                      false,
                      "XYZ");

        checkStandard(parseStandard(L""),
                      L" ",
                      WcharParser::e_ALIGN_DEFAULT,
                      WcharParser::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(),
                      NumericValue(),
                      NumericValue(),
                      NumericValue(),
                      false,
                      L"");

        checkStandard(parseStandard(L"*<06.3XYZ"),
                      L"*",
                      WcharParser::e_ALIGN_LEFT,
                      WcharParser::e_SIGN_DEFAULT,
                      false,
                      true,
                      NumericValue(NumericValue::e_VALUE, 6),
                      NumericValue(NumericValue::e_VALUE, 6),
                      NumericValue(NumericValue::e_VALUE, 3),
                      NumericValue(NumericValue::e_VALUE, 3),
                      false,
                      L"XYZ");

        checkStandard(parseStandard(L"*<{}.{}XYZ"),
                      L"*",
                      WcharParser::e_ALIGN_LEFT,
                      WcharParser::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_ARG_ID, 0 ),
                      NumericValue(NumericValue::e_VALUE,  99),
                      NumericValue(NumericValue::e_ARG_ID, 1 ),
                      NumericValue(NumericValue::e_VALUE,  98),
                      false,
                      L"XYZ");

        checkStandard(parseStandard(L"*<{1}.{2}XYZ"),
                      L"*",
                      WcharParser::e_ALIGN_LEFT,
                      WcharParser::e_SIGN_DEFAULT,
                      false,
                      false,
                      NumericValue(NumericValue::e_ARG_ID, 1 ),
                      NumericValue(NumericValue::e_VALUE,  98),
                      NumericValue(NumericValue::e_ARG_ID, 2 ),
                      NumericValue(NumericValue::e_VALUE,  97),
                      false,
                      L"XYZ");

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
// Copyright 2023 Bloomberg Finance L.P.
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
