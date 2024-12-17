// bslfmt_formatterspecificationstandard.t.cpp                        -*-C++-*-
#include <bslfmt_formatterspecificationstandard.h>

#include <bslfmt_format_string.h> // Testing only
#include <bslfmt_formattertestutil.h> // Testing only

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <bslstl_string.h>

#include <bslfmt_format_arg.h> // Testing only
#include <bslfmt_format_args.h> // Testing only
#include <bslfmt_format_parsecontext.h> // Testing only
#include <bslfmt_format_string.h> // Testing only
#include <bslfmt_formattertestutil.h> // Testing only

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
#define BSLFMT_FORMAT_STRING_PARAMETER bslfmt::format_string<>
#define BSLFMT_FORMAT_WSTRING_PARAMETER bslfmt::wformat_string<>
#else
// We cannot define format_string<t_ARGS...> in a C++03 compliant manner, so
// have to use non-template versions instead.
#define BSLFMT_FORMAT_STRING_PARAMETER bslfmt::format_string
#define BSLFMT_FORMAT_WSTRING_PARAMETER bslfmt::wformat_string
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY


typedef FormatterSpecificationStandard<char>     FSSC;
typedef FormatterSpecificationStandard<wchar_t>  FSSW;

BSLFMT_FORMATTER_TEST_CONSTEVAL FSSC parseStandard(
                             BSLFMT_FORMAT_STRING_PARAMETER inputSpecification,
                             FSSC::Category                 category)
{
    FSSC splitter;

    bsl::basic_string_view<char> inputStringView(inputSpecification.get());

    bslfmt::MockParseContext<char> mpc(inputStringView, 4);

    FSSC::parse(&splitter, &mpc, category);

    return splitter;
}

BSLFMT_FORMATTER_TEST_CONSTEVAL FSSW parseStandard(
                            BSLFMT_FORMAT_WSTRING_PARAMETER inputSpecification,
                            FSSW::Category                  category)
{
    FSSW splitter;

    bsl::basic_string_view<wchar_t> inputStringView(inputSpecification.get());

    bslfmt::MockParseContext<wchar_t> mpc(inputStringView, 4);

    FSSW::parse(&splitter, &mpc, category);

    return splitter;
}

void checkStandard(
       int                                              line,
       const FSSC&                                      splitter,
       bsl::basic_string_view<char>                     filler,
       FormatterSpecificationStandard<char>::Alignment  alignment,
       FormatterSpecificationStandard<char>::Sign       sign,
       bool                                             alternativeFlag,
       bool                                             zeroPaddingFlag,
       FormatterSpecificationNumericValue               postprocessedWidth,
       FormatterSpecificationNumericValue               postprocessedPrecision,
       bool                                             localeSpecificFlag,
       FormatterSpecificationStandard<char>::FormatType formatType)
{
    FSSC fs = splitter;

    bslfmt::MockFormatContext<char> mfc(99, 98, 97, 96);
    FormatterSpecificationStandard<char>::postprocess(&fs, mfc);

    ASSERTV(line, filler == bsl::basic_string_view<char>(fs.filler(),
                                                  fs.fillerCharacters()));
    ASSERTV(line, alignment == fs.alignment());
    ASSERTV(line, sign == fs.sign());
    ASSERTV(line, alternativeFlag == fs.alternativeFlag());
    ASSERTV(line, zeroPaddingFlag == fs.zeroPaddingFlag());
    ASSERTV(line, postprocessedWidth == fs.postprocessedWidth());
    ASSERTV(line, postprocessedPrecision == fs.postprocessedPrecision());
    ASSERTV(line, localeSpecificFlag == fs.localeSpecificFlag());
    ASSERTV(line, formatType == fs.formatType());
}

void checkStandard(
    int                                                 line,
    const FSSW&                                         splitter,
    bsl::basic_string_view<wchar_t>                     filler,
    FormatterSpecificationStandard<wchar_t>::Alignment  alignment,
    FormatterSpecificationStandard<wchar_t>::Sign       sign,
    bool                                                alternativeFlag,
    bool                                                zeroPaddingFlag,
    FormatterSpecificationNumericValue                  postprocessedWidth,
    FormatterSpecificationNumericValue                  postprocessedPrecision,
    bool                                                localeSpecificFlag,
    FormatterSpecificationStandard<wchar_t>::FormatType formatType)
{
    FSSW fs = splitter;

    bslfmt::MockFormatContext<wchar_t> mfc(99, 98, 97, 96);
    FormatterSpecificationStandard<wchar_t>::postprocess(&fs, mfc);

    ASSERTV(line,
            filler == bsl::basic_string_view<wchar_t>(fs.filler(),
                                                      fs.fillerCharacters()));
    ASSERTV(line, alignment == fs.alignment());
    ASSERTV(line, sign == fs.sign());
    ASSERTV(line, alternativeFlag == fs.alternativeFlag());
    ASSERTV(line, zeroPaddingFlag == fs.zeroPaddingFlag());
    ASSERTV(line, postprocessedWidth == fs.postprocessedWidth());
    ASSERTV(line, postprocessedPrecision == fs.postprocessedPrecision());
    ASSERTV(line, localeSpecificFlag == fs.localeSpecificFlag());
    ASSERTV(line, formatType == fs.formatType());
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int  test    = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;
    // const bool veryVerbose = argc > 3;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 1: {
        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");
        typedef FormatterSpecificationStandard<char>    FSC;
        typedef FormatterSpecificationStandard<wchar_t> FSW;
        typedef FormatterSpecificationNumericValue      FSValue;

        checkStandard(L_,
                      parseStandard("", FSC::e_CATEGORY_STRING),
                      " ",
                      FSC::e_ALIGN_DEFAULT,
                      FSC::e_SIGN_DEFAULT,
                      false,
                      false,
                      FSValue(0, FSValue::e_DEFAULT),
                      FSValue(0, FSValue::e_DEFAULT),
                      false,
                      FSC::e_STRING_DEFAULT);

        checkStandard(L_,
                      parseStandard("2.3s", FSC::e_CATEGORY_STRING),
                      " ",
                      FSC::e_ALIGN_DEFAULT,
                      FSC::e_SIGN_DEFAULT,
                      false,
                      false,
                      FSValue(2, FSValue::e_VALUE),
                      FSValue(3, FSValue::e_VALUE),
                      false,
                      FSC::e_STRING_DEFAULT);

        checkStandard(L_,
                      parseStandard("{3}.{2}", FSC::e_CATEGORY_STRING),
                      " ",
                      FSC::e_ALIGN_DEFAULT,
                      FSC::e_SIGN_DEFAULT,
                      false,
                      false,
                      FSValue(96, FSValue::e_VALUE),
                      FSValue(97, FSValue::e_VALUE),
                      false,
                      FSC::e_STRING_DEFAULT);

        checkStandard(L_,
                      parseStandard("*<{1}.{3}F",
                      FSC::e_CATEGORY_FLOATING),
                      "*",
                      FSC::e_ALIGN_LEFT,
                      FSC::e_SIGN_DEFAULT,
                      false,
                      false,
                      FSValue(98, FSValue::e_VALUE),
                      FSValue(96, FSValue::e_VALUE),
                      false,
                      FSC::e_FLOATING_FIXED_UC);

        checkStandard(L_,
                      parseStandard(L"",
                      FSW::e_CATEGORY_STRING),
                      L" ",
                      FSW::e_ALIGN_DEFAULT,
                      FSW::e_SIGN_DEFAULT,
                      false,
                      false,
                      FSValue(0, FSValue::e_DEFAULT),
                      FSValue(0, FSValue::e_DEFAULT),
                      false,
                      FSW::e_STRING_DEFAULT);

        checkStandard(L_,
                      parseStandard(L"2.3s", FSC::e_CATEGORY_STRING),
                      L" ",
                      FSC::e_ALIGN_DEFAULT,
                      FSC::e_SIGN_DEFAULT,
                      false,
                      false,
                      FSValue(2, FSValue::e_VALUE),
                      FSValue(3, FSValue::e_VALUE),
                      false,
                      FSC::e_STRING_DEFAULT);

        checkStandard(L_,
                      parseStandard(L"{2}.{1}", FSC::e_CATEGORY_STRING),
                      L" ",
                      FSC::e_ALIGN_DEFAULT,
                      FSC::e_SIGN_DEFAULT,
                      false,
                      false,
                      FSValue(97, FSValue::e_VALUE),
                      FSValue(98, FSValue::e_VALUE),
                      false,
                      FSC::e_STRING_DEFAULT);

        checkStandard(L_,
                      parseStandard(L"*<{0}.{3}f",
                      FSW::e_CATEGORY_FLOATING),
                      L"*",
                      FSW::e_ALIGN_LEFT,
                      FSW::e_SIGN_DEFAULT,
                      false,
                      false,
                      FSValue(99, FSValue::e_VALUE),
                      FSValue(96, FSValue::e_VALUE),
                      false,
                      FSW::e_FLOATING_FIXED);

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
