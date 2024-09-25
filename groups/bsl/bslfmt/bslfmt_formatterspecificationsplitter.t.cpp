// bslfmt_formatterspecificationsplitter.t.cpp                        -*-C++-*-
#include <bslfmt_formatterspecificationsplitter.h>

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <stdio.h>
#include <string.h>

#include <bslfmt_formatarg.h> // Testing only
#include <bslfmt_formatargs.h> // Testing only
#include <bslfmt_formatparsecontext.h> // Testing only
#include <bslfmt_formatstring.h> // Testing only
#include <bslfmt_formattertestutil.h> // Testing only

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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES) &&                 \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
#define BSLFMT_FORMAT_STRING_PARAMETER bslfmt::format_string<>
#define BSLFMT_FORMAT_WSTRING_PARAMETER bslfmt::wformat_string<>
#define BSLFMT_FORMATTER_TEST_CONSTEVAL consteval
#else
// We cannot define format_string<t_ARGS...> in a C++03 compliant manner, so
// have to use non-template versions instead.
#define BSLFMT_FORMAT_STRING_PARAMETER bslfmt::format_string
#define BSLFMT_FORMAT_WSTRING_PARAMETER bslfmt::wformat_string
#define BSLFMT_FORMATTER_TEST_CONSTEVAL
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

namespace {

#if 0
template <class t_CHAR>
struct MockParseContext : public bslfmt::basic_format_parse_context<t_CHAR> {
    MockParseContext(const bsl::basic_string_view<t_CHAR>& sv)
    : bslfmt::basic_format_parse_context<t_CHAR>(sv, 3)
    {
    }
};

template <class t_CHAR>
struct MockFormatContext {
  public:
    // TYPES
    typedef basic_format_arg<basic_format_context<t_CHAR*, t_CHAR> > Arg;

  private:
    // DATA
    Arg d_arg_0;
    Arg d_arg_1;
    Arg d_arg_2;

  public:
    // CREATORS
    template <class t_ARG0>
    MockFormatContext(const t_ARG0 &arg_0) {
        bsl::array<Arg, 1> arr;
        Format_FormatArg_ImpUtil::makeFormatArgArray(&arr, arg_0);
        d_arg_0 = Arg(arr[0]);
    }

    template <class t_ARG0, class t_ARG1>
    MockFormatContext(const t_ARG0 &arg_0, const t_ARG1 &arg_1) {
        bsl::array<Arg, 2> arr;
        Format_FormatArg_ImpUtil::makeFormatArgArray(&arr, arg_0, arg_1);
        d_arg_0 = Arg(arr[0]);
        d_arg_1 = Arg(arr[1]);
    }

    template <class t_ARG0, class t_ARG1, class t_ARG2>
    MockFormatContext(const t_ARG0& arg_0,
                      const t_ARG1& arg_1,
                      const t_ARG2& arg_2)
    {
        bsl::array<Arg, 3> arr;
        Format_FormatArg_ImpUtil::makeFormatArgArray(&arr,
                                                      arg_0,
                                                      arg_1,
                                                      arg_2);
        d_arg_0 = Arg(arr[0]);
        d_arg_1 = Arg(arr[1]);
        d_arg_2 = Arg(arr[2]);
    }

    // ACCESSORS
    Arg arg(size_t id) const BSLS_KEYWORD_NOEXCEPT {
        if (id == 0) return d_arg_0;
        if (id == 1) return d_arg_1;
        if (id == 2) return d_arg_2;
        return Arg();
    }
};

#endif

typedef FormatterSpecification_Splitter<char>    FSC;
typedef FormatterSpecification_Splitter<wchar_t> FSW;

BSLFMT_FORMATTER_TEST_CONSTEVAL FSC parseStandard(
       BSLFMT_FORMAT_STRING_PARAMETER                   inputSpecification)
{
    const FSC::Sections sect = static_cast<FSC::Sections>(
            FSC::e_SECTIONS_FILL_ALIGN | FSC::e_SECTIONS_SIGN_FLAG |
            FSC::e_SECTIONS_ALTERNATE_FLAG | FSC::e_SECTIONS_ZERO_PAD_FLAG |
            FSC::e_SECTIONS_WIDTH | FSC::e_SECTIONS_PRECISION |
            FSC::e_SECTIONS_LOCALE_FLAG | FSC::e_SECTIONS_FINAL_SPECIFICATION);

    FSC splitter;

    bsl::basic_string_view<char> inputStringView(inputSpecification.get());

    bslfmt::Formatter_MockParseContext<char> mpc(inputStringView, 3);

    FSC::parse(&splitter, &mpc, sect);

    return splitter;
}

BSLFMT_FORMATTER_TEST_CONSTEVAL FSW parseStandard(
                             BSLFMT_FORMAT_WSTRING_PARAMETER inputSpecification)
{
    const FSC::Sections sect = static_cast<FSC::Sections>(
            FSC::e_SECTIONS_FILL_ALIGN | FSC::e_SECTIONS_SIGN_FLAG |
            FSC::e_SECTIONS_ALTERNATE_FLAG | FSC::e_SECTIONS_ZERO_PAD_FLAG |
            FSC::e_SECTIONS_WIDTH | FSC::e_SECTIONS_PRECISION |
            FSC::e_SECTIONS_LOCALE_FLAG | FSC::e_SECTIONS_FINAL_SPECIFICATION);

    FSW splitter;

    bsl::basic_string_view<wchar_t> inputStringView(inputSpecification.get());

    bslfmt::Formatter_MockParseContext<wchar_t> mpc(inputStringView, 3);

    FSW::parse(&splitter, &mpc, sect);

    return splitter;
}

void checkStandard(
          const FSC                                        &splitter,
          bsl::basic_string_view<char>                      filler,
          FormatterSpecification_Splitter<char>::Alignment  alignment,
          FormatterSpecification_Splitter<char>::Sign       sign,
          bool                                              alternativeFlag,
          bool                                              zeroPaddingFlag,
          FormatterSpecification_NumericValue               rawWidth,
          FormatterSpecification_NumericValue               postprocessedWidth,
          FormatterSpecification_NumericValue               rawPrecision,
          FormatterSpecification_NumericValue               postprocessedPrecision,
          bool                                              localeSpecificFlag,
          bsl::basic_string_view<char>                      finalSpec)
{
    FSC fs = splitter;

    bslfmt::Formatter_MockFormatContext<char> mfc(99, 98, 97);
    FSC::postprocess(&fs, mfc);

    ASSERT(filler == bsl::basic_string_view<char>(fs.filler(),
                                                  fs.fillerCharacters()));
    ASSERT(alignment == fs.alignment());
    ASSERT(sign == fs.sign());
    ASSERT(alternativeFlag == fs.alternativeFlag());
    ASSERT(zeroPaddingFlag == fs.zeroPaddingFlag());
    ASSERT(rawWidth == fs.rawWidth());
    ASSERT(rawPrecision == fs.rawPrecision());
    ASSERT(postprocessedWidth == fs.postprocessedWidth());
    ASSERT(postprocessedPrecision == fs.postprocessedPrecision());
    ASSERT(localeSpecificFlag == fs.localeSpecificFlag());
    ASSERT(finalSpec == fs.finalSpec());
}

void checkStandard(
       const FSW&                                           splitter,
       bsl::basic_string_view<wchar_t>                      filler,
       FormatterSpecification_Splitter<wchar_t>::Alignment  alignment,
       FormatterSpecification_Splitter<wchar_t>::Sign       sign,
       bool                                                 alternativeFlag,
       bool                                                 zeroPaddingFlag,
       FormatterSpecification_NumericValue                  rawWidth,
       FormatterSpecification_NumericValue                  postprocessedWidth,
       FormatterSpecification_NumericValue                  rawPrecision,
       FormatterSpecification_NumericValue                  postprocessedPrecision,
       bool                                                 localeSpecificFlag,
       bsl::basic_string_view<wchar_t>                      finalSpec)
{
    FSW fs = splitter;

    bslfmt::Formatter_MockFormatContext<wchar_t> mfc(99, 98, 97);
    FSW::postprocess(&fs, mfc);

    ASSERT(filler == bsl::basic_string_view<wchar_t>(fs.filler(),
                                                     fs.fillerCharacters()));
    ASSERT(alignment == fs.alignment());
    ASSERT(sign == fs.sign());
    ASSERT(alternativeFlag == fs.alternativeFlag());
    ASSERT(zeroPaddingFlag == fs.zeroPaddingFlag());
    ASSERT(rawWidth == fs.rawWidth());
    ASSERT(rawPrecision == fs.rawPrecision());
    ASSERT(postprocessedWidth == fs.postprocessedWidth());
    ASSERT(postprocessedPrecision == fs.postprocessedPrecision());
    ASSERT(localeSpecificFlag == fs.localeSpecificFlag());
    ASSERT(finalSpec == fs.finalSpec());
}

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
        typedef FormatterSpecification_Splitter<char>    FSC;
        typedef FormatterSpecification_Splitter<wchar_t> FSW;
        typedef FormatterSpecification_NumericValue      FSValue;

        checkStandard(parseStandard(""),
                      " ",
                      FSC::e_ALIGN_DEFAULT,
                      FSC::e_SIGN_DEFAULT,
                      false,
                      false,
                      FSValue(0, FSValue::e_DEFAULT),
                      FSValue(0, FSValue::e_DEFAULT),
                      FSValue(0, FSValue::e_DEFAULT),
                      FSValue(0, FSValue::e_DEFAULT),
                      false,
                      "");

        checkStandard(parseStandard("*<06.3XYZ"),
                      "*",
                      FSC::e_ALIGN_LEFT,
                      FSC::e_SIGN_DEFAULT,
                      false,
                      true,
                      FSValue(6, FSValue::e_VALUE),
                      FSValue(6, FSValue::e_VALUE),
                      FSValue(3, FSValue::e_VALUE),
                      FSValue(3, FSValue::e_VALUE),
                      false,
                      "XYZ");

        checkStandard(parseStandard("*<{}.{}XYZ"),
                      "*",
                      FSC::e_ALIGN_LEFT,
                      FSC::e_SIGN_DEFAULT,
                      false,
                      false,
                      FSValue(0, FSValue::e_ARG_ID),
                      FSValue(99, FSValue::e_VALUE),
                      FSValue(1, FSValue::e_ARG_ID),
                      FSValue(98, FSValue::e_VALUE),
                      false,
                      "XYZ");

        checkStandard(parseStandard("*<{1}.{2}XYZ"),
                      "*",
                      FSC::e_ALIGN_LEFT,
                      FSC::e_SIGN_DEFAULT,
                      false,
                      false,
                      FSValue(1, FSValue::e_ARG_ID),
                      FSValue(98, FSValue::e_VALUE),
                      FSValue(2, FSValue::e_ARG_ID),
                      FSValue(97, FSValue::e_VALUE),
                      false,
                      "XYZ");

        checkStandard(parseStandard(L""),
                      L" ",
                      FSW::e_ALIGN_DEFAULT,
                      FSW::e_SIGN_DEFAULT,
                      false,
                      false,
                      FSValue(0, FSValue::e_DEFAULT),
                      FSValue(0, FSValue::e_DEFAULT),
                      FSValue(0, FSValue::e_DEFAULT),
                      FSValue(0, FSValue::e_DEFAULT),
                      false,
                      L"");

        checkStandard(parseStandard(L"*<06.3XYZ"),
                      L"*",
                      FSW::e_ALIGN_LEFT,
                      FSW::e_SIGN_DEFAULT,
                      false,
                      true,
                      FSValue(6, FSValue::e_VALUE),
                      FSValue(6, FSValue::e_VALUE),
                      FSValue(3, FSValue::e_VALUE),
                      FSValue(3, FSValue::e_VALUE),
                      false,
                      L"XYZ");

        checkStandard(parseStandard(L"*<{}.{}XYZ"),
                      L"*",
                      FSW::e_ALIGN_LEFT,
                      FSW::e_SIGN_DEFAULT,
                      false,
                      false,
                      FSValue(0, FSValue::e_ARG_ID),
                      FSValue(99, FSValue::e_VALUE),
                      FSValue(1, FSValue::e_ARG_ID),
                      FSValue(98, FSValue::e_VALUE),
                      false,
                      L"XYZ");

        checkStandard(parseStandard(L"*<{1}.{2}XYZ"),
                      L"*",
                      FSW::e_ALIGN_LEFT,
                      FSW::e_SIGN_DEFAULT,
                      false,
                      false,
                      FSValue(1, FSValue::e_ARG_ID),
                      FSValue(98, FSValue::e_VALUE),
                      FSValue(2, FSValue::e_ARG_ID),
                      FSValue(97, FSValue::e_VALUE),
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
