// bslfmt_formatterspecificationsplitter.t.cpp                        -*-C++-*-
#include <bslfmt_formatterspecificationsplitter.h>

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
//                  ASSISTANCE FUNCTIONS
// ----------------------------------------------------------------------------

void checkStandard(
    const char                   *inputSpecification,
    bsl::basic_string_view<char>  filler,
    Formatter_SpecificationSplitter<char, const char *>::Alignment alignment,
    Formatter_SpecificationSplitter<char, const char *>::Sign      sign,
    bool                                                       alternativeFlag,
    bool                                                       zeroPaddingFlag,
    Formatter_SpecificationSplitter<char, const char *>::Value width,
    Formatter_SpecificationSplitter<char, const char *>::Value precision,
    bool                         localeSpecificFlag,
    bsl::basic_string_view<char> finalSpec)
{
    const Formatter_SpecificationSplitter<char, const char *>::Sections sect =
          static_cast<
              Formatter_SpecificationSplitter<char, const char *>::Sections>(
              Formatter_SpecificationSplitter<char, const char *>::
                  e_SECTIONS_FILL_ALIGN |
              Formatter_SpecificationSplitter<char, const char *>::
                  e_SECTIONS_SIGN_FLAG |
              Formatter_SpecificationSplitter<char, const char *>::
                  e_SECTIONS_ALTERNATE_FLAG |
              Formatter_SpecificationSplitter<char, const char *>::
                  e_SECTIONS_ZERO_PAD_FLAG |
              Formatter_SpecificationSplitter<char, const char *>::
                  e_SECTIONS_WIDTH |
              Formatter_SpecificationSplitter<char, const char *>::
                  e_SECTIONS_PRECISION |
              Formatter_SpecificationSplitter<char, const char *>::
                  e_SECTIONS_LOCALE_FLAG |
              Formatter_SpecificationSplitter<char, const char *>::
                  e_SECTIONS_FINAL_SPECIFICATION);

    Formatter_SpecificationSplitter<char, const char *> fs;

    const char *start = inputSpecification;
    const char *end   = start + strlen(start);
    int         rv    = fs.parse(&start, end, sect);
    ASSERT(0 == rv);
    ASSERT(filler == bsl::basic_string_view<char>(fs.filler(),
                                                  fs.fillerCharacters()));
    ASSERT(alignment == fs.alignment());
    ASSERT(sign == fs.sign());
    ASSERT(alternativeFlag == fs.alternativeFlag());
    ASSERT(zeroPaddingFlag == fs.zeroPaddingFlag());
    ASSERT(width == fs.width());
    ASSERT(precision == fs.precision());
    ASSERT(localeSpecificFlag == fs.localcSpecificFlag());
    ASSERT(finalSpec == fs.finalSpec());
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
        typedef Formatter_SpecificationSplitter<char, const char *>       FSC;
        typedef Formatter_SpecificationSplitter<wchar_t, const wchar_t *> FSW;

        checkStandard("",
                      "",
                      FSC::e_ALIGN_DEFAULT,
                      FSC::e_SIGN_DEFAULT,
                      false,
                      false,
                      FSC::Value(0, FSC::Value::e_DEFAULT),
                      FSC::Value(0, FSC::Value::e_DEFAULT),
                      false,
                      "");

        checkStandard("*<06.3XYZ",
                      "*",
                      FSC::e_ALIGN_LEFT,
                      FSC::e_SIGN_DEFAULT,
                      false,
                      true,
                      FSC::Value(6, FSC::Value::e_VALUE),
                      FSC::Value(3, FSC::Value::e_VALUE),
                      false,
                      "XYZ");

        checkStandard("*<{}.{3}XYZ",
                      "*",
                      FSC::e_ALIGN_LEFT,
                      FSC::e_SIGN_DEFAULT,
                      false,
                      false,
                      FSC::Value(0, FSC::Value::e_NEXT_ARG),
                      FSC::Value(3, FSC::Value::e_ARG_ID),
                      false,
                      "XYZ");

        FSC  fs;
        const char                                          *spec = "{}";
        const char                                          *start = spec + 1;
        const char *end = start + strlen(start);
        int         rv  = fs.parse(
                         &start,
                         end,
                         Formatter_SpecificationSplitter<char,
                                                const char *>::e_SECTIONS_ALL);
        ASSERT(0 == rv);  // placeholder
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
