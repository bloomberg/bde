// bslstl_formatterunicodeutils.t.cpp                                 -*-C++-*-
#include <bslfmt_formatterunicodeutils.h>

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
        ASSERT(true);  // placeholder

        unsigned char bytes[4];
        bytes[0] = (unsigned char)0xff;
        bytes[1] = (unsigned char)0xfe;
        bytes[2] = (unsigned char)0x00;
        bytes[2] = (unsigned char)0x00;

        Formatter_UnicodeUtils::CodePointExtractionResult cpresult =
                                      Formatter_UnicodeUtils::extractCodePoint(
                                          Formatter_UnicodeUtils::e_UTF8,
                                          bytes,
                                          4);

        ASSERT(!cpresult.isValid);

        cpresult = Formatter_UnicodeUtils::extractCodePoint(
                        sizeof(wchar_t) == 2 ? Formatter_UnicodeUtils::e_UTF16
                                             : Formatter_UnicodeUtils::e_UTF32,
                        bytes,
                        4);

        ASSERT(!cpresult.isValid);

        const char *fmt1 = (const char *) u8"\U0001F600";
        int len1 = (int) strlen(fmt1);

        cpresult = Formatter_UnicodeUtils::extractCodePoint(
                                                Formatter_UnicodeUtils::e_UTF8,
                                                (const void *)fmt1,
                                                len1);

        ASSERT(cpresult.codePointValue == 0x1f600);
        ASSERT(cpresult.codePointWidth == 2);
        ASSERT(cpresult.isValid == true);
        ASSERT(cpresult.numSourceBytes == 4);
        ASSERT(cpresult.sourceEncoding == Formatter_UnicodeUtils::e_UTF8);

        if (sizeof(wchar_t) == 2) {
            const wchar_t *fmt2 = (const wchar_t *)u"\U0001F600";
            int         len2 = (int) wcslen(fmt2);

            cpresult = Formatter_UnicodeUtils::extractCodePoint(
                                                Formatter_UnicodeUtils::e_UTF16,
                                                (const void *)fmt2,
                                                len2 * sizeof(wchar_t));
            
            ASSERT(cpresult.sourceEncoding == Formatter_UnicodeUtils::e_UTF16);
            ASSERT(cpresult.numSourceBytes == 4);
        }
        else {
            const wchar_t *fmt2 = (const wchar_t *)U"\U0001F600";
            int         len2 = (int) wcslen(fmt2);

            cpresult = Formatter_UnicodeUtils::extractCodePoint(
                                                Formatter_UnicodeUtils::e_UTF32,
                                                (const void *)fmt2,
                                                len2 * sizeof(wchar_t));

            ASSERT(cpresult.sourceEncoding == Formatter_UnicodeUtils::e_UTF32);
            ASSERT(cpresult.numSourceBytes == 4);
        }

        ASSERT(cpresult.codePointValue == 0x1f600);
        ASSERT(cpresult.codePointWidth == 2);
        ASSERT(cpresult.isValid == true);

        const char *fmt3 = (const char *) u8"\U00000067\U00000308";
        int len3 = (int) strlen(fmt3);

        Formatter_UnicodeUtils::GraphemeClusterExtractionResult
        gcresult = Formatter_UnicodeUtils::extractGraphemeCluster(
                                                Formatter_UnicodeUtils::e_UTF8,
                                                (const void *)fmt3,
                                                len3);

        ASSERT(gcresult.firstCodePointValue == 0x67);
        ASSERT(gcresult.firstCodePointWidth == 1);
        ASSERT(gcresult.isValid == true);
        ASSERT(gcresult.numCodePoints == 2);
        ASSERT(gcresult.numSourceBytes == 3);
        ASSERT(gcresult.sourceEncoding == Formatter_UnicodeUtils::e_UTF8);

        const char *fmt4 = (const char *) u8"\U0001F408\U0000200D\U0001F7E7hello";
        int len4 = (int) strlen(fmt4);

        gcresult = Formatter_UnicodeUtils::extractGraphemeCluster(
                                                Formatter_UnicodeUtils::e_UTF8,
                                                (const void *)fmt4,
                                                len4);

        ASSERT(gcresult.firstCodePointValue == 0x1f408);
        ASSERT(gcresult.firstCodePointWidth == 2);
        ASSERT(gcresult.isValid == true);
        ASSERT(gcresult.numCodePoints == 3);
        ASSERT(gcresult.numSourceBytes == 11);
        ASSERT(gcresult.sourceEncoding == Formatter_UnicodeUtils::e_UTF8);

        if (sizeof(wchar_t) == 2) {
            const wchar_t *fmt5 = (const wchar_t *)
                u"\U0001F407\U0000200D\U0001F7E7hello";
            int         len5 = (int) wcslen(fmt5);

            gcresult = Formatter_UnicodeUtils::extractGraphemeCluster(
                                                Formatter_UnicodeUtils::e_UTF16,
                                                (const void *)fmt5,
                                                len5 * sizeof(wchar_t));
            
            ASSERT(gcresult.sourceEncoding == Formatter_UnicodeUtils::e_UTF16);
            ASSERT(gcresult.numSourceBytes == 12);
        }
        else {
            const wchar_t *fmt5 = (const wchar_t *)
                U"\U0001F407\U0000200D\U0001F7E7hello";
            int         len5 = (int) wcslen(fmt5);

            gcresult = Formatter_UnicodeUtils::extractGraphemeCluster(
                                                Formatter_UnicodeUtils::e_UTF32,
                                                (const void *)fmt5,
                                                len5 * sizeof(wchar_t));

            ASSERT(gcresult.sourceEncoding == Formatter_UnicodeUtils::e_UTF32);
            ASSERT(gcresult.numSourceBytes == 12);
        }

        ASSERT(gcresult.firstCodePointValue == 0x1f407);
        ASSERT(gcresult.firstCodePointWidth == 2);
        ASSERT(gcresult.isValid == true);
        ASSERT(gcresult.numCodePoints == 3);
        ASSERT(gcresult.numSourceBytes == 12);

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
