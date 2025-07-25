// bslstl_formatterunicodedata.t.cpp                                  -*-C++-*-
#include <bslfmt_formatterunicodedata.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // `printf`
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;
using namespace bslfmt;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test is a set of structures containing data received
// from an external source.  Unfortunately, we cannot verify the correctness of
// this data, so we can only verify the discoverability of these structures for
// other `bslfmt` components.
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

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
        //   Since a component is a set of structures and static variables, we
        //   want to check that they are can be discovered and have correct
        //   values.
        //
        // Concerns:
        //: 1 Structures and static variables, defined in the component can be
        //:   discovered.
        //
        // Plan:
        //: 1 Create an object of each structure defined in the component.
        //:
        //: 2 Verify the values of static variables.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        typedef FormatterUnicodeData UnicodeData;

        // Testing the availability of entities.

        UnicodeData::GraphemeBreakCategory      category;
        UnicodeData::BooleanRange               booleanRange;
        UnicodeData::GraphemeBreakCategoryRange graphemeRange;

        (void) category;       // suppress compiler warning
        (void) booleanRange;   // suppress compiler warning
        (void) graphemeRange;  // suppress compiler warning

        // Testing class data.

        ASSERTV(UnicodeData::s_graphemeBreakCategoryRanges[0].d_start,
                0 == UnicodeData::s_graphemeBreakCategoryRanges[0].d_start);
        ASSERTV(UnicodeData::s_extendedPictographicRanges[0].d_start,
                0xa9 == UnicodeData::s_extendedPictographicRanges[0].d_start);
        ASSERTV(UnicodeData::s_printableRanges[0].d_start,
                0x20 == UnicodeData::s_printableRanges[0].d_start);
        ASSERTV(UnicodeData::s_graphemeExtendRanges[0].d_start,
                0x300 == UnicodeData::s_graphemeExtendRanges[0].d_start);
        ASSERTV(UnicodeData::s_doubleFieldWidthRanges[0].d_start,
                0x1100 == UnicodeData::s_doubleFieldWidthRanges[0].d_start);

        ASSERTV(UnicodeData::s_graphemeBreakCategoryRangeCount,
                1376 == UnicodeData::s_graphemeBreakCategoryRangeCount);
        ASSERTV(UnicodeData::s_extendedPictographicRangeCount,
                78   == UnicodeData::s_extendedPictographicRangeCount);
        ASSERTV(UnicodeData::s_printableRangeCount,
                736  == UnicodeData::s_printableRangeCount);
        ASSERTV(UnicodeData::s_graphemeExtendRangeCount,
                375  == UnicodeData::s_graphemeExtendRangeCount);
        ASSERTV(UnicodeData::s_doubleFieldWidthRangeCount,
                103  == UnicodeData::s_doubleFieldWidthRangeCount);

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
