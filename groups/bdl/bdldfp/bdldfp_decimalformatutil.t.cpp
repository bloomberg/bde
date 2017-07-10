// bdldfp_decimalformatutil.t.cpp                                     -*-C++-*-
#include <bdldfp_decimalformatutil.h>
#include <bdldfp_decimal.h>
#include <bdldfp_decimalutil.h>
#include <bdldfp_decimalimputil.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>      // 'atoi'
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a utility class.  This test driver tests
// each implemented utility function independently.  The test driver verifies
// that all 'format' function overloads produce correct string representation
// of a decimal value.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] int format(char *, int, Decimal32,  int, Style, Sign, Letters, char);
// [ 2] int format(char *, int, Decimal64,  int, Style, Sign, Letters, char);
// [ 2] int format(char *, int, Decimal128, int, Style, Sign, Letters, char);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

#define ASSERT_SAFE_FAIL_RAW(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(expr)
#define ASSERT_SAFE_PASS_RAW(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(expr)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
typedef bdldfp::DecimalFormatConfig Config;
typedef bdldfp::DecimalFormatUtil   Util;
typedef bdldfp::DecimalUtil         DU;
typedef bdldfp::DecimalImpUtil      DIU;

//=============================================================================
//                      GLOBAL CONSTANTS
//-----------------------------------------------------------------------------

#define MAX_DECIMAL_32                                                    \
 "9999999000""0000000000""0000000000""0000000000""0000000000"             \
 "0000000000""0000000000""0000000000""0000000000""0000000"

#define MIN_DECIMAL_32                                                    \
 "0.00000000""0000000000""0000000000""0000000000""0000000000"             \
 "0000000000""0000000000""0000000000""0000000000""0000001"

#define MAX_DECIMAL_64                                                    \
 "9999999999""9999990000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""00000"

#define MIN_DECIMAL_64                                                    \
 "0.00000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""00001"

#define MAX_DECIMAL_128                                                   \
 "9999999999""9999999999""9999999999""9999000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""00000"

 #define MIN_DECIMAL_128                                                  \
 "0.00000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""0000000000""0000000000""0000000000""0000000000" \
 "0000000000""0000000000""00001"

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&da);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'format'
        //
        // Concerns:
        //: 1 That for input decimal value including minimum, maximum and
        //    special values 'format' functions output human readable string.
        //:
        //: 2 That 'format' returns the necessary size of a buffer with no
        //:   effect on 'buffer' which is sufficient for coping resultant
        //:   string if the string size exceeds the size of 'buffer'.
        //:
        //: 3 That 'format' outputs decimal value in fixed format if
        //:   'style == e_FIXED'.
        //:
        //: 4 That 'format' outputs decimal value in scientific format if
        //:   'style == e_SCIENTIFIC'.
        //:
        //: 5 That 'style' is default to 'e_SCIENTIFIC' if not defined.
        //:
        //: 6 That 'format' outputs decimal value in natural format if
        //:   'style == e_NATURAL'.
        //:
        //: 7 That 'format' precedes the resultant string with a minus sign if
        //:   decimal value is less that 0.
        //:
        //: 8 That 'sign' is default to 'e_NEGATIVE_ONLY' if not defined.
        //:
        //: 9 That 'format' precedes the resultant string with plus or minus
        //:   sign if 'sign == e_ALWAYS"
        //:
        //: 10 That 'format' returns the necessary size of a buffer sufficient
        //:    for storing the specified decimal 'value' formatted to human
        //:    readable string with no effect on buffer if the 'length' of the
        //:    'buffer' is negative.
        //:
        //: 11 That 'format' does not write terminating null character to the
        //:    buffer.
        //:
        //: 12 That length of string representing fraction part of the decimal
        //:    value equals the specified precision.
        //:
        //: 13 That if the number of digits after decimal point is less than
        //:    the specified precision then the fraction part is expanded by
        //:    by '0' on the right up to the precision length.
        //:
        //: 14 That if the number of digits after decimal pointis greater than
        //:    the specified precision then the fraction part is cut off to
        //:    fewer digits designated by the precision and rounded so that the
        //:    resultant value to be as close as possible to initial value.
        //:
        //: 15 That the resultant string for special values does not depend on
        //:    the style, precision and decimal point character.
        //:
        //: 16 That 'format' outputs decimal value in natural format if
        //:   'style == e_NATURAL'.
        //:
        //: 17 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //:
        //: 1 Using a table driven technique test that 'format' returns the
        //:   necessary size of a buffer with no effect on 'buffer' which is
        //:   sufficient for coping resultant string if the string size exceeds
        //:   the size of 'buffer'.  (C-2)
        //:
        //:  1 For each test element in the table specify decimal value,
        //:    configuration, pointer to the buffer, buffer size and expected
        //:    length.
        //:
        //:  2 For each test element create a 'Decimal32' object.
        //:
        //:  3 Fill in output 'buffer' with 'x' character.
        //:
        //:  4 Call 'format' function for the decimal object created in P-1.2.
        //:    Test that the 'buffer' was not modified and resultant value
        //:    equals to the expected value.
        //:
        //: 2 Using a loop-based technique test that 'format' function produces
        //:   a human readable string for different valid decimal numbers
        //:   designated by the significand value consisting of various number
        //:   of digits and the exponent in the range of minimum to maximum
        //:   exponent value defined by 'Decimal32' type.  For the test purpose
        //:   set precision value to 100 and use default 'sign' and
        //:   'decimalPoint' values.
        //:
        //:  1 Create a 'Decimal32' object.
        //:
        //:  2 Call 'format' function for the value created in P-2.1.
        //:
        //:  3 Parse the string obtained in P-2.2 and ensure that the resultant
        //:    value equals to the value created in P-2.1.
        //:
        //:  4 Repeat P-2.1..3 for 'e_FIXED', 'e_SCIENTIFIC' and 'e_NATURAL'
        //:    format styles.
        //:
        //: 3 Using a table driven technique test that 'format' function
        //:   produces a human readable string from a decimal number with the
        //:   specified 'precision', 'sign' and 'decimalPoint'.
        //:
        //:  1 For each test element in the table specify decimal value,
        //:    'precision', 'style', 'sign', 'decimalPoint' and expected
        //:    resultant string.
        //:
        //:  2 For each test element create a 'Decimal32' object.
        //:
        //:  3 Fill in output 'buffer' with 'x' character.
        //:
        //:  4 Call 'format' function for the decimal object created in P-3.2.
        //:    Test that the 'buffer' is filled in by a string that equals
        //:    to the expected string and the resultant value equals to the
        //:    length of the expected string.
        //:
        //:  5 Test that if rounding is required the 'format' function rounds
        //:    up the decimal value as "round-half-up".  (C-14)
        //:
        //:  6 Test that a character in the 'buffer' that follows by a
        //:    character representing the less significant digit equals to 'x'.
        //:    (C-11)
        //:
        //:  7 Test that for normal decimal values length of string
        //:    representing fraction part of equals to the specified precision.
        //:    (C-12)
        //:
        //: 4 Test that style' is default to 'e_SCIENTIFIC' if not defined.
        //:   (C-5)
        //:
        //:   1 Call 'format' function with default and explicitly set
        //:     'e_SCIENTIFIC' configuration.  Test that resultant values are
        //:     the same.
        //:
        //:   2 Call 'format' function with explicitly set 'e_FIXED'
        //:     configuration.  Test that resultant value does not equal to
        //:     default and 'e_SCIENTIFIC' formats.
        //:
        //: 5 Test that 'sign' is default to 'e_NEGATIVE_ONLY' if not defined.
        //:   (C-8)
        //:
        //:   1 Call 'format' function with default and explicitly set
        //:     'e_NEGATIVE_ONLY' configuration.  Test that resultant values
        //:     are the same.
        //:
        //:   2 Call 'format' function with explicitly set 'e_ALWAYS'
        //:     configuration.  Test that resultant value does not equal to
        //:     default and 'e_NEGATIVE_ONLY' formats.
        //:
        //: 6 Repeat P1-5 for Decimal64 and Decimal128 types. 
        //
        // Testing:
        //   int format(char *, int, Decimal32,  int, Style, Sign, Ltrs, char);
        //   int format(char *, int, Decimal64,  int, Style, Sign, Ltrs, char);
        //   int format(char *, int, Decimal128, int, Style, Sign, Ltrs, char);
        // --------------------------------------------------------------------

#define F(PRECISION) Config(PRECISION, Config::e_FIXED)
#define S(PRECISION) Config(PRECISION, Config::e_SCIENTIFIC)
#define N(PRECISION) Config(PRECISION, Config::e_NATURAL)

#define FS(PRECISION) Config(PRECISION, Config::e_FIXED,      Config::e_ALWAYS)
#define SS(PRECISION) Config(PRECISION, Config::e_SCIENTIFIC, Config::e_ALWAYS)
#define NS(PRECISION) Config(PRECISION, Config::e_NATURAL,    Config::e_ALWAYS)

#define FR(PRECISION, DECIMAL_POINT) Config(PRECISION,               \
                                            Config::e_FIXED,         \
                                            Config::e_NEGATIVE_ONLY, \
                                            "", "", "",              \
                                            DECIMAL_POINT)
#define SR(PRECISION, DECIMAL_POINT) Config(PRECISION,               \
                                            Config::e_FIXED,         \
                                            Config::e_NEGATIVE_ONLY, \
                                            "", "", "",              \
                                            DECIMAL_POINT)
#define NR(PRECISION, DECIMAL_POINT) Config(PRECISION,               \
                                            Config::e_FIXED,         \
                                            Config::e_NEGATIVE_ONLY, \
                                            "", "", "",              \
                                            DECIMAL_POINT)



#define FIXED(X)   Config(X, Config::e_FIXED)
#define SCIENT(X)  Config(X, Config::e_SCIENTIFIC)
#define NATURAL(X) Config(X, Config::e_NATURAL)

#define FIXED_SA   Config(2, Config::e_FIXED,      Config::e_ALWAYS)
#define SCIENT_SA  Config(2, Config::e_SCIENTIFIC, Config::e_ALWAYS)
#define NATURAL_SA Config(2, Config::e_NATURAL,    Config::e_ALWAYS)

        if (verbose) cout << endl
                          << "TEST FORMAT METHODS" << endl
                          << "===================" << endl;

        if (verbose) cout << endl
                          << "Test Decimal32" << endl
                          << "--------------" << endl;

#define DEC(X) BDLDFP_DECIMAL_DF(X)
        //-----------------------------------------------------------------
        // C-2,10
        {
            typedef bdldfp::Decimal32 Type;

            Type SUBN_P  =  bsl::numeric_limits<Type>::denorm_min();
            Type SUBN_N  = -bsl::numeric_limits<Type>::denorm_min();
            Type INF_P   =  bsl::numeric_limits<Type>::infinity();
            Type INF_N   = -bsl::numeric_limits<Type>::infinity();
            Type NAN_Q_P =  bsl::numeric_limits<Type>::quiet_NaN();
            Type NAN_Q_N = -bsl::numeric_limits<Type>::quiet_NaN();
            Type NAN_S_P =  bsl::numeric_limits<Type>::signaling_NaN();
            Type NAN_S_N = -bsl::numeric_limits<Type>::signaling_NaN();

            const int  k_BUFFER_SIZE = 256;
            char       BUFFER[k_BUFFER_SIZE];
            char      *B_PTR = &BUFFER[0];

            static const struct {
                const int     d_line;
                const Type    d_decimal;
                const Config  d_config;
                char         *d_buffer;
                const int     d_bufferSize;
                const int     d_expected;
            } DATA [] = {
            //------------------------------------------------------------
            // Line | Decimal      | Config | Buffer | Buffer | Expected
            //      |              |        |        | Size   | Length
            //------------------------------------------------------------
            {  L_,   DEC( 1234.865),    F(3),   B_PTR,       0,       8 },
            {  L_,   DEC(-1234.865),    F(3),   B_PTR,      -1,       9 },
            {  L_,   DEC( 1234.865),    F(3),       0,      -1,       8 },

            {  L_,   SUBN_P,            F(0),   B_PTR,       0,       4 },
            {  L_,   SUBN_N,            F(0),   B_PTR,       0,       5 },
            {  L_,   INF_P,             F(0),   B_PTR,       0,       3 },
            {  L_,   INF_N,             F(0),   B_PTR,       0,       4 },
            {  L_,   NAN_Q_P,           F(0),   B_PTR,       0,       3 },
            {  L_,   NAN_Q_N,           F(0),   B_PTR,       0,       4 },
            {  L_,   NAN_S_P,           F(0),   B_PTR,       0,       3 },
            {  L_,   NAN_S_N,           F(0),   B_PTR,       0,       4 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                bsl::fill_n(B_PTR, k_BUFFER_SIZE, 'x');

                const int     LINE        = DATA[ti].d_line;
                const Type    DECIMAL32   = DATA[ti].d_decimal;
                const Config  CONFIG      = DATA[ti].d_config;
                char         *BUFFER      = DATA[ti].d_buffer;
                const int     BUFFER_SIZE = DATA[ti].d_bufferSize;
                const int     EXPECTED    = DATA[ti].d_expected;

                int len = Util::format(BUFFER,
                                       BUFFER_SIZE,
                                       DECIMAL32,
                                       CONFIG);

                LOOP3_ASSERT(LINE, len, EXPECTED, len == EXPECTED);
                ASSERTV(L_, B_PTR == bsl::search_n(B_PTR,
                                                   B_PTR + k_BUFFER_SIZE,
                                                   k_BUFFER_SIZE,
                                                   'x'));
            }
        }

        {
            if (verbose) cout << "Loop-based test" << endl;

            typedef bdldfp::Decimal32 Type;

            const int SIGNIFICANDS[] = {       1,
                                              22,
                                             333,
                                            4444,
                                           55555,
                                          666666,
                                         7777777,
            };
            const int NUM_SIGNIFCANDS = static_cast<int>(sizeof SIGNIFICANDS
                                                       / sizeof *SIGNIFICANDS);

            const Config CONFIGURATIONS[] = { F(95),
                                              S(7),
                                              N(12),
            };

            const int NUM_CONFIGURATIONS = static_cast<int>(
                                                     sizeof CONFIGURATIONS
                                                     / sizeof *CONFIGURATIONS);

            const int BUFFER_SIZE = 256;
            char      BUFFER[BUFFER_SIZE];

            for (int ci = 0; ci < NUM_CONFIGURATIONS; ++ci) {

                const Config& CONFIG =  CONFIGURATIONS[ci];
                const int EXPONENT_MIN = -95;
                const int EXPONENT_MAX =  96;

                for (int si = 0; si < NUM_SIGNIFCANDS; ++si) {
                    const int SIGNIFICAND = SIGNIFICANDS[si];

                    for (int ei = EXPONENT_MIN; ei <= EXPONENT_MAX; ++ei) {
                        const int  EXPONENT = ei;
                        Type VALUE = static_cast<Type>(DU::makeDecimal64(
                                                                   SIGNIFICAND,
                                                                   EXPONENT));
                        const Type EXPECTED = VALUE;

                        int len = Util::format(BUFFER,
                                               BUFFER_SIZE,
                                               VALUE,
                                               CONFIG);

                        const bsl::string RESULT(BUFFER, len);

                        int  res = DU::parseDecimal32(&VALUE, RESULT);

                        ASSERTV(L_, res, 0 == res);
                        LOOP5_ASSERT(L_,
                                     SIGNIFICAND,
                                     EXPONENT,
                                     VALUE,
                                     EXPECTED,
                                     VALUE == EXPECTED);
                    }
                }
            }
        }

        {
            if (verbose) cout << "Table-driven test" << endl;

            typedef bdldfp::Decimal32 Type;

            Type SUBN_P  =  bsl::numeric_limits<Type>::denorm_min();
            Type SUBN_N  = -bsl::numeric_limits<Type>::denorm_min();
            Type INF_P   =  bsl::numeric_limits<Type>::infinity();
            Type INF_N   = -bsl::numeric_limits<Type>::infinity();
            Type NAN_Q_P =  bsl::numeric_limits<Type>::quiet_NaN();
            Type NAN_Q_N = -bsl::numeric_limits<Type>::quiet_NaN();
            Type NAN_S_P =  bsl::numeric_limits<Type>::signaling_NaN();
            Type NAN_S_N = -bsl::numeric_limits<Type>::signaling_NaN();
            Type MAX     =  bsl::numeric_limits<Type>::max();
            Type MIN     =  bsl::numeric_limits<Type>::min();

            static const struct {
                const int          d_line;
                const Type         d_decimal;
                const Config       d_config;
                const char        *d_expected;
            } DATA [] = {
                //-----------------------------------------------------
                // Line | Decimal   | Configuration |  Expected
                //-----------------------------------------------------

                // ----------------------------------------------------
                // FIXED format
                // C-1,3
                {  L_,    SUBN_P,            F(0),     "snan"          },
                {  L_,    SUBN_N,            F(0),    "-snan"          },
                {  L_,    INF_P,             F(0),     "inf"           },
                {  L_,    INF_N,             F(0),    "-inf"           },
                {  L_,    NAN_Q_P,           F(0),     "nan"           },
                {  L_,    NAN_Q_N,           F(0),    "-nan"           },
                {  L_,    NAN_S_P,           F(0),     "nan"           },
                {  L_,    NAN_S_N,           F(0),    "-nan"           },
                {  L_,    MAX,               F(0),      MAX_DECIMAL_32 },
                {  L_,   -MAX,               F(0),  "-" MAX_DECIMAL_32 },
                {  L_,    MIN,               F(95),     MIN_DECIMAL_32 },
                {  L_,   -MIN,               F(95), "-" MIN_DECIMAL_32 },
                {  L_,    DEC(-0.0),         F(0),    "-0"             },
                {  L_,    DEC(0.0),          F(0),     "0"             },

                // -----------------------------------------------------------
                // SCIENTIFIC format
                // C-1,4
                {  L_,    SUBN_P,            S(0),    "snan"         },
                {  L_,    SUBN_N,            S(0),   "-snan"         },
                {  L_,    INF_P,             S(0),    "inf"          },
                {  L_,    INF_N,             S(0),   "-inf"          },
                {  L_,    NAN_Q_P,           S(0),    "nan"          },
                {  L_,    NAN_Q_N,           S(0),   "-nan"          },
                {  L_,    NAN_S_P,           S(0),    "nan"          },
                {  L_,    NAN_S_N,           S(0),   "-nan"          },
                {  L_,    MAX,               S(6),    "9.999999E+96" },
                {  L_,   -MAX,               S(6),   "-9.999999E+96" },
                {  L_,    MIN,               S(0),    "1E-95"        },
                {  L_,   -MIN,               S(0),   "-1E-95"        },
                {  L_,    DEC(-0.0),         S(0),   "-0E-1"         },
                {  L_,    DEC(0.0),          S(0),    "0E-1"         },
                {  L_,    DEC(0.0),          S(1),    "0.0E-1"       },
                {  L_,    DEC(0.00),         S(2),    "0.00E-2"      },
                {  L_,    DEC(0.000),        S(3),    "0.000E-3"     },

                //-------------------------------------------------------------
                // C-7,8
                {  L_,    DEC( 123.0),       F(0),       "123"       },
                {  L_,    DEC(-123.0),       F(0),      "-123"       },
                {  L_,    DEC( 123.0),       S(2),       "1.23E+2"   },
                {  L_,    DEC(-123.0),       S(2),      "-1.23E+2"   },
                {  L_,    DEC( 123.0),       N(0),       "123"       },
                {  L_,    DEC(-123.0),       N(0),      "-123"       },
                {  L_,    NAN_Q_P,           F(0),       "nan"       },
                {  L_,    NAN_Q_N,           F(0),      "-nan"       },
                {  L_,    NAN_S_P,           F(0),       "nan"       },
                {  L_,    NAN_S_N,           F(0),      "-nan"       },
                {  L_,    SUBN_P,            F(0),       "snan"      },
                {  L_,    SUBN_N,            F(0),      "-snan"      },
                {  L_,    INF_P,             F(0),       "inf"       },
                {  L_,    INF_N,             F(0),      "-inf"       },

                //-------------------------------------------------------------
                // C-9
                {  L_,    DEC( 123.0),       FS(0),     "+123"       },
                {  L_,    DEC(-123.0),       FS(0),     "-123"       },
                {  L_,    DEC( 123.0),       SS(2),     "+1.23E+2"   },
                {  L_,    DEC(-123.0),       SS(2),     "-1.23E+2"   },
                {  L_,    DEC( 123.0),       NS(0),     "+123"       },
                {  L_,    DEC(-123.0),       NS(0),     "-123"       },
                {  L_,    NAN_Q_P,           FS(0),     "+nan"       },
                {  L_,    NAN_Q_N,           FS(0),     "-nan"       },
                {  L_,    NAN_S_P,           FS(0),     "+nan"       },
                {  L_,    NAN_S_N,           FS(0),     "-nan"       },
                {  L_,    SUBN_P,            FS(0),     "+snan"      },
                {  L_,    SUBN_N,            FS(0),     "-snan"      },
                {  L_,    INF_P,             FS(0),     "+inf"       },
                {  L_,    INF_N,             FS(0),     "-inf"       },

                //-------------------------------------------------------------
                // C-13
                {  L_,    DEC(1.11),         F(2),      "1.11"       },
                {  L_,    DEC(1.11),         F(3),      "1.110"      },
                {  L_,    DEC(1.11),         F(4),      "1.1100"     },
                {  L_,    DEC(1.11),         S(2),      "1.11E+0"    },
                {  L_,    DEC(1.11),         S(3),      "1.110E+0"   },
                {  L_,    DEC(1.11),         S(4),      "1.1100E+0"  },
                {  L_,    DEC(1.11),         N(2),      "1.11"       },
                {  L_,    DEC(1.11),         N(3),      "1.110"      },
                {  L_,    DEC(1.11),         N(4),      "1.1100"     },

                //-------------------------------------------------------------
                // C-14
                {  L_,    DEC(4.44),         F(2),      "4.44"       },
                {  L_,    DEC(4.44),         F(1),      "4.4"        },
                {  L_,    DEC(4.44),         F(0),      "4"          },
                {  L_,    DEC(5.55),         F(2),      "5.55"       },
                {  L_,    DEC(5.55),         F(1),      "5.6"        },
                {  L_,    DEC(5.55),         F(0),      "6"          },
                {  L_,    DEC(9.99),         F(2),      "9.99"       },
                {  L_,    DEC(9.99),         F(1),      "10.0"       },
                {  L_,    DEC(9.99),         F(0),      "10"         },

                {  L_,    DEC(4.44),         S(2),      "4.44E+0"    },
                {  L_,    DEC(4.44),         S(1),      "4.4E+0"     },
                {  L_,    DEC(4.44),         S(0),      "4E+0"       },
                {  L_,    DEC(5.55),         S(2),      "5.55E+0"    },
                {  L_,    DEC(5.55),         S(1),      "5.6E+0"     },
                {  L_,    DEC(5.55),         S(0),      "6E+0"       },
                {  L_,    DEC(9.99),         S(2),      "9.99E+0"    },
                {  L_,    DEC(9.99),         S(1),      "1.0E+1"     },
                {  L_,    DEC(9.99),         S(0),      "1E+1"       },

                {  L_,    DEC(4.44),         N(2),      "4.44"       },
                {  L_,    DEC(4.44),         N(1),      "4.4"        },
                {  L_,    DEC(4.44),         N(0),      "4"          },
                {  L_,    DEC(5.55),         N(2),      "5.55"       },
                {  L_,    DEC(5.55),         N(1),      "5.6"        },
                {  L_,    DEC(5.55),         N(0),      "6"          },
                {  L_,    DEC(9.99),         N(2),      "9.99"       },
                {  L_,    DEC(9.99),         N(1),      "10.0"       },
                {  L_,    DEC(9.99),         N(0),      "10"         },

                //-------------------------------------------------------------
                // C-15
                {  L_,    SUBN_P,            F(0),      "snan"       },
                {  L_,    SUBN_N,            F(1),     "-snan"       },
                {  L_,    INF_P,             F(2),      "inf"        },
                {  L_,    INF_N,             F(3),     "-inf"        },
                {  L_,    NAN_Q_P,           F(4),      "nan"        },
                {  L_,    NAN_Q_N,           F(5),     "-nan"        },
                {  L_,    NAN_S_P,           F(6),      "nan"        },
                {  L_,    NAN_S_N,           F(7),     "-nan"        },

                {  L_,    SUBN_P,            S(7),      "snan"       },
                {  L_,    SUBN_N,            S(6),     "-snan"       },
                {  L_,    INF_P,             S(5),      "inf"        },
                {  L_,    INF_N,             S(4),     "-inf"        },
                {  L_,    NAN_Q_P,           S(3),      "nan"        },
                {  L_,    NAN_Q_N,           S(2),     "-nan"        },
                {  L_,    NAN_S_P,           S(1),      "nan"        },
                {  L_,    NAN_S_N,           S(0),     "-nan"        },

                {  L_,    SUBN_P,            N(0),     "snan"        },
                {  L_,    SUBN_N,            N(1),    "-snan"        },
                {  L_,    INF_P,             N(2),     "inf"         },
                {  L_,    INF_N,             N(3),    "-inf"         },
                {  L_,    NAN_Q_P,           N(4),     "nan"         },
                {  L_,    NAN_Q_N,           N(5),    "-nan"         },
                {  L_,    NAN_S_P,           N(6),     "nan"         },
                {  L_,    NAN_S_N,           N(7),    "-nan"         },

                //-------------------------------------------------------------
                // C-16
                {  L_,    DEC(1E-7),         N(0),     "1E-7"           },
                {  L_,    DEC(1E-6),         N(6),     "0.000001"       },
                {  L_,    DEC(1.1E-6),       N(7),     "0.0000011"      },
                {  L_,    DEC(1.11E-6),      N(8),     "0.00000111"     },
                {  L_,    DEC(1.111E-6),     N(9),     "0.000001111"    },
                {  L_,    DEC(1.1111E-6),    N(10),    "0.0000011111"   },
                {  L_,    DEC(1.11111E-6),   N(11),    "0.00000111111"  },
                {  L_,    DEC(1.111111E-6),  N(12),    "0.000001111111" },

                {  L_,    DEC(1E+0),         N(0),     "1"              },
                {  L_,    DEC(1.1E+1),       N(0),     "11"             },
                {  L_,    DEC(1.11E+2),      N(0),     "111"            },
                {  L_,    DEC(1.111E+3),     N(0),     "1111"           },
                {  L_,    DEC(1.1111E+4),    N(0),     "11111"          },
                {  L_,    DEC(1.11111E+5),   N(0),     "111111"         },
                {  L_,    DEC(1.111111E+6),  N(0),     "1111111"        },
                {  L_,    DEC(1.111111E+7),  N(6),     "1.111111E+7"    },


            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            const int k_BUFFER_SIZE = 256;
            char      buffer[k_BUFFER_SIZE];

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                fill_n(&buffer[0], k_BUFFER_SIZE, 'x');

                const int      LINE        = DATA[ti].d_line;
                const Type     DECIMAL32   = DATA[ti].d_decimal;
                const Config   CONFIG      = DATA[ti].d_config;
                const string&  EXPECTED    = DATA[ti].d_expected;

                int len = Util::format(buffer,
                                       k_BUFFER_SIZE,
                                       DECIMAL32,
                                       CONFIG);

                const string RESULT(buffer, len);
                if (veryVerbose) P_(len) P(RESULT);

                ASSERTV(LINE, len, len > 0);
                LOOP3_ASSERT(LINE, RESULT, EXPECTED, RESULT == EXPECTED);
                LOOP3_ASSERT(LINE,
                             len,
                             EXPECTED.length(),
                             len == static_cast<int>(EXPECTED.length()));

                //-------------------------------------------------------------
                // C-11
                LOOP2_ASSERT(LINE, buffer[len], 'x' == buffer[len]);

                //-------------------------------------------------------------
                // C-12
                if (FP_NORMAL == DU::classify(DECIMAL32)) {

                    const char *BEGIN     = &buffer[0];
                    const char *END       = &buffer[len];
                    const char *POINT_POS = bsl::find(BEGIN,
                                                      END,
                                                      CONFIG.decimalPoint());
                    if (POINT_POS == END) {
                        LOOP2_ASSERT(LINE,
                                     CONFIG.precision(),
                                     0 == CONFIG.precision());
                    }
                    else {
                        const char *EXPONENT_POS = bsl::find(
                                                            POINT_POS,
                                                            END,
                                                            CONFIG.exponent());
                        LOOP3_ASSERT(LINE,
                                     CONFIG.precision(),
                                     bsl::distance(POINT_POS + 1,
                                                   EXPONENT_POS),
                                     bsl::distance(POINT_POS + 1,
                                                   EXPONENT_POS)
                                     == CONFIG.precision());
                    }
                }
            }

            //-----------------------------------------------------------------
            // C-5
            {
                const Config  DEFAULT_CONFIG(   4);
                const Config  SCIENTIFIC_CONFIG(4, Config::e_SCIENTIFIC);
                const Config  FIXED_CONFIG(     4, Config::e_FIXED);
                const Type    VALUE = DEC(376.12);
                const string  EXPECTED = "3.7612E+2";
                char          DEFAULT_BUFFER[k_BUFFER_SIZE];

                int           len = Util::format(DEFAULT_BUFFER,
                                                 k_BUFFER_SIZE,
                                                 VALUE,
                                                 DEFAULT_CONFIG);

                const string DEFAULT(DEFAULT_BUFFER, len);
                LOOP3_ASSERT(L_, DEFAULT, EXPECTED, DEFAULT == EXPECTED);

                // Test that 'format' produces the same resultant string when
                // invoked with default and scientific configuration.
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   SCIENTIFIC_CONFIG);

                const string SCIENTIFIC_FORMAT(buffer, len);
                LOOP3_ASSERT(L_,
                             SCIENTIFIC_FORMAT,
                             EXPECTED,
                             SCIENTIFIC_FORMAT == EXPECTED);
                LOOP3_ASSERT(L_,
                             SCIENTIFIC_FORMAT,
                             DEFAULT,
                             SCIENTIFIC_FORMAT == DEFAULT);

                // Test that 'format' produces the different resultant strings
                // when invoked with default and fixed configuration.
                const string FIXED_EXPECTED("376.1200");
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   FIXED_CONFIG);

                const bsl::string FIXED_FORMAT(buffer, len);
                LOOP3_ASSERT(L_,
                             FIXED_FORMAT,
                             FIXED_EXPECTED,
                             FIXED_FORMAT == FIXED_EXPECTED);
                LOOP3_ASSERT(L_,
                             FIXED_FORMAT,
                             DEFAULT,
                             FIXED_FORMAT != DEFAULT);
            }

            //-----------------------------------------------------------------
            // C-8
            {
                const Config    DEFAULT_CONFIG(      2);
                const Config    NEGATIVE_ONLY_CONFIG(2,
                                                     Config::e_SCIENTIFIC,
                                                     Config::e_NEGATIVE_ONLY);
                const Config    ALWAYS_CONFIG(       2,
                                                     Config::e_SCIENTIFIC,
                                                     Config::e_ALWAYS);
                const Type   VALUE = DEC(456.0);
                const string EXPECTED = "4.56E+2";
                char         DEFAULT_BUFFER[k_BUFFER_SIZE];

                int          len = Util::format(DEFAULT_BUFFER,
                                                k_BUFFER_SIZE,
                                                VALUE,
                                                DEFAULT_CONFIG);
                const string DEFAULT(DEFAULT_BUFFER, len);
                LOOP3_ASSERT(L_, DEFAULT, EXPECTED, DEFAULT == EXPECTED);

                // Test that 'format' produces the same resultant string when
                // invoked with default and 'negative only' sign configuration.
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   NEGATIVE_ONLY_CONFIG);

                const string    NEGATIVE_ONLY(buffer, len);
                LOOP3_ASSERT(L_,
                             NEGATIVE_ONLY,
                             EXPECTED,
                             NEGATIVE_ONLY == EXPECTED);
                LOOP3_ASSERT(L_,
                             NEGATIVE_ONLY,
                             DEFAULT,
                             NEGATIVE_ONLY == DEFAULT);

                // Test that 'format' produces the different resultant strings
                // when invoked with default and always sign configuration.
                const string  ALWAYS_EXPECTED("+4.56E+2");
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   ALWAYS_CONFIG);

                const string  ALWAYS(buffer, len);

                LOOP3_ASSERT(L_,
                             ALWAYS,
                             ALWAYS_EXPECTED,
                             ALWAYS == ALWAYS_EXPECTED);
                LOOP3_ASSERT(L_,
                             ALWAYS,
                             DEFAULT,
                             ALWAYS != DEFAULT);
            }

            if (verbose) cout << "\nNegative Testing." << endl;
            {
                bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

                if (verbose) cout << "\t'buffer == NULL'" << endl;
                {
                    const int  k_SIZE = 1000;
                    char       BUFFER[k_SIZE];
                    const Type V;

                    ASSERT_SAFE_PASS(Util::format(BUFFER, k_SIZE, V));
                    ASSERT_SAFE_FAIL(Util::format(     0, k_SIZE, V));
                }

                if (verbose) cout << "\t'Negative buffer size'" << endl;
                {
                    const int  k_SIZE = 1000;
                    char       BUFFER[k_SIZE];
                    const Type V;

                    ASSERT_SAFE_PASS(Util::format(BUFFER, k_SIZE, V));
                    ASSERT_SAFE_FAIL(Util::format(BUFFER,     -1, V));
                }

                if (verbose) cout << "\t'Negative precision'" << endl;
                {
                    const bdldfp::DecimalFormatConfig VALID(0);
                    const bdldfp::DecimalFormatConfig INVALID(-1);
                    const int                         k_SIZE = 1000;
                    char                              BUFFER[k_SIZE];
                    const Type                        V;

                    ASSERT_SAFE_PASS(Util::format(BUFFER, k_SIZE, V, VALID));
                    ASSERT_SAFE_FAIL(Util::format(BUFFER,     -1, V, INVALID));
                }
            }
        }

#undef DEC
        if (verbose) cout << endl
                          << "Test Decimal64" << endl
                          << "--------------" << endl;
#define DEC(X) BDLDFP_DECIMAL_DD(X)
        //-----------------------------------------------------------------
        // C-2,10
        {
            typedef bdldfp::Decimal64 Type;

            Type SUBN_P  =  bsl::numeric_limits<Type>::denorm_min();
            Type SUBN_N  = -bsl::numeric_limits<Type>::denorm_min();
            Type INF_P   =  bsl::numeric_limits<Type>::infinity();
            Type INF_N   = -bsl::numeric_limits<Type>::infinity();
            Type NAN_Q_P =  bsl::numeric_limits<Type>::quiet_NaN();
            Type NAN_Q_N = -bsl::numeric_limits<Type>::quiet_NaN();
            Type NAN_S_P =  bsl::numeric_limits<Type>::signaling_NaN();
            Type NAN_S_N = -bsl::numeric_limits<Type>::signaling_NaN();

            const int  k_BUFFER_SIZE = 256;
            char       BUFFER[k_BUFFER_SIZE];
            char      *B_PTR = &BUFFER[0];

            static const struct {
                const int     d_line;
                const Type    d_decimal;
                const Config  d_config;
                char         *d_buffer;
                const int     d_bufferSize;
                const int     d_expected;
            } DATA [] = {
            //------------------------------------------------------------
            // Line | Decimal      | Config | Buffer | Buffer | Expected
            //      |              |        |        | Size   | Length
            //------------------------------------------------------------
            {  L_,   DEC( 1234.865),    F(3),   B_PTR,       0,       8 },
            {  L_,   DEC(-1234.865),    F(3),   B_PTR,      -1,       9 },
            {  L_,   DEC( 1234.865),    F(3),       0,      -1,       8 },
            {  L_,   DEC( 1234.865),    S(6),   B_PTR,       0,      11 },
            {  L_,   DEC(-1234.865),    S(6),   B_PTR,      -1,      12 },
            {  L_,   DEC( 1234.865),    S(6),       0,      -1,      11 },

            {  L_,   SUBN_P,            F(0),   B_PTR,       0,       4 },
            {  L_,   SUBN_N,            F(0),   B_PTR,       0,       5 },
            {  L_,   INF_P,             F(0),   B_PTR,       0,       3 },
            {  L_,   INF_N,             F(0),   B_PTR,       0,       4 },
            {  L_,   NAN_Q_P,           F(0),   B_PTR,       0,       3 },
            {  L_,   NAN_Q_N,           F(0),   B_PTR,       0,       4 },
            {  L_,   NAN_S_P,           F(0),   B_PTR,       0,       3 },
            {  L_,   NAN_S_N,           F(0),   B_PTR,       0,       4 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                bsl::fill_n(B_PTR, k_BUFFER_SIZE, 'x');

                const int     LINE        = DATA[ti].d_line;
                const Type    DECIMAL     = DATA[ti].d_decimal;
                const Config  CONFIG      = DATA[ti].d_config;
                char         *BUFFER      = DATA[ti].d_buffer;
                const int     BUFFER_SIZE = DATA[ti].d_bufferSize;
                const int     EXPECTED    = DATA[ti].d_expected;

                int len = Util::format(BUFFER,
                                       BUFFER_SIZE,
                                       DECIMAL,
                                       CONFIG);

                LOOP3_ASSERT(LINE, len, EXPECTED, len == EXPECTED);
                ASSERTV(L_, B_PTR == bsl::search_n(B_PTR,
                                                   B_PTR + k_BUFFER_SIZE,
                                                   k_BUFFER_SIZE,
                                                   'x'));
            }
        }
        {
            if (verbose) cout << "Loop-based test" << endl;

            typedef bdldfp::Decimal64 Type;

            const unsigned long long SIGNIFICANDS[] = {       1,
                                                             22,
                                                            333,
                                                           4444,
                                                          55555,
                                                         666666,
                                                        7777777,
                                                       88888888,
                                                      999999999,
                                                     1111111111,
                                                    22222222222,
                                                   333333333333,
                                                  4444444444444,
                                                 55555555555555,
                                                666666666666666,
                                               7777777777777777,
            };
            const int NUM_SIGNIFCANDS = static_cast<int>(
                                                       sizeof SIGNIFICANDS
                                                       / sizeof *SIGNIFICANDS);

            const Config CONFIGURATIONS[] = { F(383),
                                              S(15),
                                              N(21),
            };

            const int NUM_CONFIGURATIONS = static_cast<int>(
                                                     sizeof CONFIGURATIONS
                                                     / sizeof *CONFIGURATIONS);

            const int BUFFER_SIZE = 1024;
            char      BUFFER[BUFFER_SIZE];

            for (int ci = 0; ci < NUM_CONFIGURATIONS; ++ci) {

                const Config&   CONFIG =  CONFIGURATIONS[ci];
                const int EXPONENT_MIN = -383;
                const int EXPONENT_MAX =  384;

                for (int si = 0; si < NUM_SIGNIFCANDS; ++si) {
                    const unsigned long long SIGNIFICAND = SIGNIFICANDS[si];

                    for (int ei = EXPONENT_MIN; ei <= EXPONENT_MAX; ++ei) {
                        const int EXPONENT = ei;
                        Type VALUE = static_cast<Type>(DU::makeDecimal64(
                                                                   SIGNIFICAND,
                                                                   EXPONENT));
                        const Type EXPECTED = VALUE;

                        int len = Util::format(BUFFER,
                                               BUFFER_SIZE,
                                               VALUE,
                                               CONFIG);

                        const bsl::string RESULT(BUFFER, len);

                        int  res = DU::parseDecimal64(&VALUE, RESULT);

                        ASSERTV(L_, res, EXPONENT, RESULT, VALUE, 0 == res);
                        LOOP5_ASSERT(L_,
                                     SIGNIFICAND,
                                     EXPONENT,
                                     VALUE,
                                     EXPECTED,
                                     VALUE == EXPECTED);
                    }
                }
            }
        }

        {
            if (verbose) cout << "Table-driven test" << endl;

            typedef bdldfp::Decimal64 Type;

            Type SUBN_P  =  bsl::numeric_limits<Type>::denorm_min();
            Type SUBN_N  = -bsl::numeric_limits<Type>::denorm_min();
            Type INF_P   =  bsl::numeric_limits<Type>::infinity();
            Type INF_N   = -bsl::numeric_limits<Type>::infinity();
            Type NAN_Q_P =  bsl::numeric_limits<Type>::quiet_NaN();
            Type NAN_Q_N = -bsl::numeric_limits<Type>::quiet_NaN();
            Type NAN_S_P =  bsl::numeric_limits<Type>::signaling_NaN();
            Type NAN_S_N = -bsl::numeric_limits<Type>::signaling_NaN();
            Type MAX     =  bsl::numeric_limits<Type>::max();
            Type MIN     =  bsl::numeric_limits<Type>::min();

            static const struct {
                const int          d_line;
                const Type         d_decimal;
                const Config       d_config;
                const char        *d_expected;
            } DATA [] = {
                //-----------------------------------------------------
                // Line | Decimal   | Configuration |  Expected
                //-----------------------------------------------------

                // ----------------------------------------------------
                // FIXED format
                // C-1,3
                {  L_,    SUBN_P,            F(0),      "snan"          },
                {  L_,    SUBN_N,            F(0),     "-snan"          },
                {  L_,    INF_P,             F(0),      "inf"           },
                {  L_,    INF_N,             F(0),     "-inf"           },
                {  L_,    NAN_Q_P,           F(0),      "nan"           },
                {  L_,    NAN_Q_N,           F(0),     "-nan"           },
                {  L_,    NAN_S_P,           F(0),      "nan"           },
                {  L_,    NAN_S_N,           F(0),     "-nan"           },
                {  L_,    MAX,               F(0),       MAX_DECIMAL_64 },
                {  L_,   -MAX,               F(0),   "-" MAX_DECIMAL_64 },
                {  L_,    MIN,               F(383),     MIN_DECIMAL_64 },
                {  L_,   -MIN,               F(383), "-" MIN_DECIMAL_64 },
                {  L_,    DEC(-0.0),         F(0),     "-0"             },
                {  L_,    DEC(0.0),          F(0),      "0"             },

                // -----------------------------------------------------------
                // SCIENTIFIC format
                // C-1,4
                {  L_,    SUBN_P,          S(0),    "snan"                   },
                {  L_,    SUBN_N,          S(0),   "-snan"                   },
                {  L_,    INF_P,           S(0),    "inf"                    },
                {  L_,    INF_N,           S(0),   "-inf"                    },
                {  L_,    NAN_Q_P,         S(0),    "nan"                    },
                {  L_,    NAN_Q_N,         S(0),   "-nan"                    },
                {  L_,    NAN_S_P,         S(0),    "nan"                    },
                {  L_,    NAN_S_N,         S(0),   "-nan"                    },
                {  L_,    MAX,             S(15),   "9.999999999999999E+384" },
                {  L_,   -MAX,             S(15),  "-9.999999999999999E+384" },
                {  L_,    MIN,             S(0),    "1E-383"                 },
                {  L_,   -MIN,             S(0),   "-1E-383"                 },
                {  L_,    DEC(-0.0),       S(0),   "-0E-1"                   },
                {  L_,    DEC(0.0),        S(0),    "0E-1"                   },
                {  L_,    DEC(0.0),        S(1),    "0.0E-1"                 },
                {  L_,    DEC(0.00),       S(2),    "0.00E-2"                },
                {  L_,    DEC(0.000),      S(3),    "0.000E-3"               },

                //-------------------------------------------------------------
                // C-7,8
                {  L_,    DEC( 123.0),       F(0),       "123"       },
                {  L_,    DEC(-123.0),       F(0),      "-123"       },
                {  L_,    DEC( 123.0),       S(2),       "1.23E+2"   },
                {  L_,    DEC(-123.0),       S(2),      "-1.23E+2"   },
                {  L_,    DEC( 123.0),       N(0),       "123"       },
                {  L_,    DEC(-123.0),       N(0),      "-123"       },
                {  L_,    NAN_Q_P,           F(0),       "nan"       },
                {  L_,    NAN_Q_N,           F(0),      "-nan"       },
                {  L_,    NAN_S_P,           F(0),       "nan"       },
                {  L_,    NAN_S_N,           F(0),      "-nan"       },
                {  L_,    SUBN_P,            F(0),       "snan"      },
                {  L_,    SUBN_N,            F(0),      "-snan"      },
                {  L_,    INF_P,             F(0),       "inf"       },
                {  L_,    INF_N,             F(0),      "-inf"       },

                //-------------------------------------------------------------
                // C-9
                {  L_,    DEC( 123.0),       FS(0),     "+123"       },
                {  L_,    DEC(-123.0),       FS(0),     "-123"       },
                {  L_,    DEC( 123.0),       SS(2),     "+1.23E+2"   },
                {  L_,    DEC(-123.0),       SS(2),     "-1.23E+2"   },
                {  L_,    DEC( 123.0),       NS(0),     "+123"       },
                {  L_,    DEC(-123.0),       NS(0),     "-123"       },
                {  L_,    NAN_Q_P,           FS(0),     "+nan"       },
                {  L_,    NAN_Q_N,           FS(0),     "-nan"       },
                {  L_,    NAN_S_P,           FS(0),     "+nan"       },
                {  L_,    NAN_S_N,           FS(0),     "-nan"       },
                {  L_,    SUBN_P,            FS(0),     "+snan"      },
                {  L_,    SUBN_N,            FS(0),     "-snan"      },
                {  L_,    INF_P,             FS(0),     "+inf"       },
                {  L_,    INF_N,             FS(0),     "-inf"       },

                //-------------------------------------------------------------
                // C-13
                {  L_,    DEC(1.11),         F(2),      "1.11"       },
                {  L_,    DEC(1.11),         F(3),      "1.110"      },
                {  L_,    DEC(1.11),         F(4),      "1.1100"     },
                {  L_,    DEC(1.11),         S(2),      "1.11E+0"    },
                {  L_,    DEC(1.11),         S(3),      "1.110E+0"   },
                {  L_,    DEC(1.11),         S(4),      "1.1100E+0"  },
                {  L_,    DEC(1.11),         N(2),      "1.11"       },
                {  L_,    DEC(1.11),         N(3),      "1.110"      },
                {  L_,    DEC(1.11),         N(4),      "1.1100"     },

                //-------------------------------------------------------------
                // C-14
                {  L_,    DEC(4.44),         F(2),      "4.44"       },
                {  L_,    DEC(4.44),         F(1),      "4.4"        },
                {  L_,    DEC(4.44),         F(0),      "4"          },
                {  L_,    DEC(5.55),         F(2),      "5.55"       },
                {  L_,    DEC(5.55),         F(1),      "5.6"        },
                {  L_,    DEC(5.55),         F(0),      "6"          },
                {  L_,    DEC(9.99),         F(2),      "9.99"       },
                {  L_,    DEC(9.99),         F(1),      "10.0"       },
                {  L_,    DEC(9.99),         F(0),      "10"         },

                {  L_,    DEC(4.44),         S(2),      "4.44E+0"    },
                {  L_,    DEC(4.44),         S(1),      "4.4E+0"     },
                {  L_,    DEC(4.44),         S(0),      "4E+0"       },
                {  L_,    DEC(5.55),         S(2),      "5.55E+0"    },
                {  L_,    DEC(5.55),         S(1),      "5.6E+0"     },
                {  L_,    DEC(5.55),         S(0),      "6E+0"       },
                {  L_,    DEC(9.99),         S(2),      "9.99E+0"    },
                {  L_,    DEC(9.99),         S(1),      "1.0E+1"     },
                {  L_,    DEC(9.99),         S(0),      "1E+1"       },

                {  L_,    DEC(4.44),         N(2),      "4.44"       },
                {  L_,    DEC(4.44),         N(1),      "4.4"        },
                {  L_,    DEC(4.44),         N(0),      "4"          },
                {  L_,    DEC(5.55),         N(2),      "5.55"       },
                {  L_,    DEC(5.55),         N(1),      "5.6"        },
                {  L_,    DEC(5.55),         N(0),      "6"          },
                {  L_,    DEC(9.99),         N(2),      "9.99"       },
                {  L_,    DEC(9.99),         N(1),      "10.0"       },
                {  L_,    DEC(9.99),         N(0),      "10"         },

                //-------------------------------------------------------------
                // C-15
                {  L_,    SUBN_P,            F(0),      "snan"       },
                {  L_,    SUBN_N,            F(1),     "-snan"       },
                {  L_,    INF_P,             F(2),      "inf"        },
                {  L_,    INF_N,             F(3),     "-inf"        },
                {  L_,    NAN_Q_P,           F(4),      "nan"        },
                {  L_,    NAN_Q_N,           F(5),     "-nan"        },
                {  L_,    NAN_S_P,           F(6),      "nan"        },
                {  L_,    NAN_S_N,           F(7),     "-nan"        },

                {  L_,    SUBN_P,            S(7),      "snan"       },
                {  L_,    SUBN_N,            S(6),     "-snan"       },
                {  L_,    INF_P,             S(5),      "inf"        },
                {  L_,    INF_N,             S(4),     "-inf"        },
                {  L_,    NAN_Q_P,           S(3),      "nan"        },
                {  L_,    NAN_Q_N,           S(2),     "-nan"        },
                {  L_,    NAN_S_P,           S(1),      "nan"        },
                {  L_,    NAN_S_N,           S(0),     "-nan"        },

                {  L_,    SUBN_P,            N(0),     "snan"        },
                {  L_,    SUBN_N,            N(1),    "-snan"        },
                {  L_,    INF_P,             N(2),     "inf"         },
                {  L_,    INF_N,             N(3),    "-inf"         },
                {  L_,    NAN_Q_P,           N(4),     "nan"         },
                {  L_,    NAN_Q_N,           N(5),    "-nan"         },
                {  L_,    NAN_S_P,           N(6),     "nan"         },
                {  L_,    NAN_S_N,           N(7),    "-nan"         },

                //-------------------------------------------------------------
                // C-16
                {  L_,    DEC(1E-7),         N(0),     "1E-7"           },
                {  L_,    DEC(1E-6),         N(6),     "0.000001"       },
                {  L_,    DEC(1.1E-6),       N(7),     "0.0000011"      },
                {  L_,    DEC(1.11E-6),      N(8),     "0.00000111"     },
                {  L_,    DEC(1.111E-6),     N(9),     "0.000001111"    },
                {  L_,    DEC(1.1111E-6),    N(10),    "0.0000011111"   },
                {  L_,    DEC(1.11111E-6),   N(11),    "0.00000111111"  },
                {  L_,    DEC(1.111111E-6),  N(12),    "0.000001111111" },

                {  L_,    DEC(1E+0),         N(0),     "1"              },
                {  L_,    DEC(1.1E+1),       N(0),     "11"             },
                {  L_,    DEC(1.11E+2),      N(0),     "111"            },
                {  L_,    DEC(1.111E+3),     N(0),     "1111"           },
                {  L_,    DEC(1.1111E+4),    N(0),     "11111"          },
                {  L_,    DEC(1.11111E+5),   N(0),     "111111"         },
                {  L_,    DEC(1.111111E+6),  N(0),     "1111111"        },
                {  L_,    DEC(1.111111E+7),  N(6),     "1.111111E+7"    },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            const int k_BUFFER_SIZE = 1024;
            char      buffer[k_BUFFER_SIZE];

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                fill_n(&buffer[0], k_BUFFER_SIZE, 'x');

                const int      LINE        = DATA[ti].d_line;
                const Type     DECIMAL32   = DATA[ti].d_decimal;
                const Config   CONFIG      = DATA[ti].d_config;
                const string&  EXPECTED    = DATA[ti].d_expected;

                int len = Util::format(buffer,
                                       k_BUFFER_SIZE,
                                       DECIMAL32,
                                       CONFIG);

                const string RESULT(buffer, len);
                if (veryVerbose) P_(len) P(RESULT);

                LOOP3_ASSERT(LINE, RESULT, EXPECTED, RESULT == EXPECTED);
                LOOP3_ASSERT(LINE,
                             len,
                             EXPECTED.length(),
                             len == static_cast<int>(EXPECTED.length()));
                //-------------------------------------------------------------
                // C-11
                LOOP2_ASSERT(LINE, buffer[len], 'x' == buffer[len]);

                //-------------------------------------------------------------
                // C-12
                if (FP_NORMAL == DU::classify(DECIMAL32)) {

                    const char *BEGIN     = &buffer[0];
                    const char *END       = &buffer[len];
                    const char *POINT_POS = bsl::find(BEGIN,
                                                      END,
                                                      CONFIG.decimalPoint());
                    if (POINT_POS == END) {
                        LOOP2_ASSERT(LINE,
                                     CONFIG.precision(),
                                     0 == CONFIG.precision());
                    }
                    else {
                        const char *EXPONENT_POS = bsl::find(
                                                            POINT_POS,
                                                            END,
                                                            CONFIG.exponent());
                        LOOP3_ASSERT(LINE,
                                     CONFIG.precision(),
                                     bsl::distance(POINT_POS + 1,
                                                   EXPONENT_POS),
                                     bsl::distance(POINT_POS + 1,
                                                   EXPONENT_POS)
                                     == CONFIG.precision());
                    }
                }
            }

            //-----------------------------------------------------------------
            // C-5
            {
                const Config  DEFAULT_CONFIG(   4);
                const Config  SCIENTIFIC_CONFIG(4, Config::e_SCIENTIFIC);
                const Config  FIXED_CONFIG(     4, Config::e_FIXED);
                const Type    VALUE = DEC(376.12);
                const string  EXPECTED = "3.7612E+2";
                char          DEFAULT_BUFFER[k_BUFFER_SIZE];

                int           len = Util::format(DEFAULT_BUFFER,
                                                 k_BUFFER_SIZE,
                                                 VALUE,
                                                 DEFAULT_CONFIG);

                const string DEFAULT(DEFAULT_BUFFER, len);
                LOOP3_ASSERT(L_, DEFAULT, EXPECTED, DEFAULT == EXPECTED);

                // Test that 'format' produces the same resultant string when
                // invoked with default and scientific configuration.
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   SCIENTIFIC_CONFIG);

                const string SCIENTIFIC_FORMAT(buffer, len);
                LOOP3_ASSERT(L_,
                             SCIENTIFIC_FORMAT,
                             EXPECTED,
                             SCIENTIFIC_FORMAT == EXPECTED);
                LOOP3_ASSERT(L_,
                             SCIENTIFIC_FORMAT,
                             DEFAULT,
                             SCIENTIFIC_FORMAT == DEFAULT);

                // Test that 'format' produces the different resultant strings
                // when invoked with default and fixed configuration.
                const string FIXED_EXPECTED("376.1200");
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   FIXED_CONFIG);

                const bsl::string FIXED_FORMAT(buffer, len);
                LOOP3_ASSERT(L_,
                             FIXED_FORMAT,
                             FIXED_EXPECTED,
                             FIXED_FORMAT == FIXED_EXPECTED);
                LOOP3_ASSERT(L_,
                             FIXED_FORMAT,
                             DEFAULT,
                             FIXED_FORMAT != DEFAULT);
            }

            //-----------------------------------------------------------------
            // C-8
            {
                const Config    DEFAULT_CONFIG(      2);
                const Config    NEGATIVE_ONLY_CONFIG(2,
                                                     Config::e_SCIENTIFIC,
                                                     Config::e_NEGATIVE_ONLY);
                const Config    ALWAYS_CONFIG(       2,
                                                     Config::e_SCIENTIFIC,
                                                     Config::e_ALWAYS);
                const Type   VALUE = DEC(456.0);
                const string EXPECTED = "4.56E+2";
                char         DEFAULT_BUFFER[k_BUFFER_SIZE];

                int          len = Util::format(DEFAULT_BUFFER,
                                                k_BUFFER_SIZE,
                                                VALUE,
                                                DEFAULT_CONFIG);
                const string DEFAULT(DEFAULT_BUFFER, len);
                LOOP3_ASSERT(L_, DEFAULT, EXPECTED, DEFAULT == EXPECTED);

                // Test that 'format' produces the same resultant string when
                // invoked with default and 'negative only' sign configuration.
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   NEGATIVE_ONLY_CONFIG);

                const string    NEGATIVE_ONLY(buffer, len);
                LOOP3_ASSERT(L_,
                             NEGATIVE_ONLY,
                             EXPECTED,
                             NEGATIVE_ONLY == EXPECTED);
                LOOP3_ASSERT(L_,
                             NEGATIVE_ONLY,
                             DEFAULT,
                             NEGATIVE_ONLY == DEFAULT);

                // Test that 'format' produces the different resultant strings
                // when invoked with default and always sign configuration.
                const string  ALWAYS_EXPECTED("+4.56E+2");
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   ALWAYS_CONFIG);

                const string  ALWAYS(buffer, len);

                LOOP3_ASSERT(L_,
                             ALWAYS,
                             ALWAYS_EXPECTED,
                             ALWAYS == ALWAYS_EXPECTED);
                LOOP3_ASSERT(L_,
                             ALWAYS,
                             DEFAULT,
                             ALWAYS != DEFAULT);
            }
            if (verbose) cout << "\nNegative Testing." << endl;
            {
                bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

                if (verbose) cout << "\t'buffer == NULL'" << endl;
                {
                    const int  k_SIZE = 1000;
                    char       BUFFER[k_SIZE];
                    const Type V;

                    ASSERT_SAFE_PASS(Util::format(BUFFER, k_SIZE, V));
                    ASSERT_SAFE_FAIL(Util::format(     0, k_SIZE, V));
                }

                if (verbose) cout << "\t'Negative buffer size'" << endl;
                {
                    const int  k_SIZE = 1000;
                    char       BUFFER[k_SIZE];
                    const Type V;

                    ASSERT_SAFE_PASS(Util::format(BUFFER, k_SIZE, V));
                    ASSERT_SAFE_FAIL(Util::format(BUFFER,     -1, V));
                }

                if (verbose) cout << "\t'Negative precision'" << endl;
                {
                    const bdldfp::DecimalFormatConfig VALID(0);
                    const bdldfp::DecimalFormatConfig INVALID(-1);
                    const int                         k_SIZE = 1000;
                    char                              BUFFER[k_SIZE];
                    const Type                        V;

                    ASSERT_SAFE_PASS(Util::format(BUFFER, k_SIZE, V, VALID));
                    ASSERT_SAFE_FAIL(Util::format(BUFFER,     -1, V, INVALID));
                }
            }
        }
#undef DEC

        if (verbose) cout << endl
                          << "Test Decimal128" << endl
                          << "---------------" << endl;

#define DEC(X) BDLDFP_DECIMAL_DL(X)

        //-----------------------------------------------------------------
        // C-2,10
        {
            typedef bdldfp::Decimal128 Type;

            Type SUBN_P  =  bsl::numeric_limits<Type>::denorm_min();
            Type SUBN_N  = -bsl::numeric_limits<Type>::denorm_min();
            Type INF_P   =  bsl::numeric_limits<Type>::infinity();
            Type INF_N   = -bsl::numeric_limits<Type>::infinity();
            Type NAN_Q_P =  bsl::numeric_limits<Type>::quiet_NaN();
            Type NAN_Q_N = -bsl::numeric_limits<Type>::quiet_NaN();
            Type NAN_S_P =  bsl::numeric_limits<Type>::signaling_NaN();
            Type NAN_S_N = -bsl::numeric_limits<Type>::signaling_NaN();

            const int  k_BUFFER_SIZE = 10000;
            char       BUFFER[k_BUFFER_SIZE];
            char      *B_PTR = &BUFFER[0];

            static const struct {
                const int     d_line;
                const Type    d_decimal;
                const Config  d_config;
                char         *d_buffer;
                const int     d_bufferSize;
                const int     d_expected;
            } DATA [] = {
            //------------------------------------------------------------
            // Line | Decimal      | Config | Buffer | Buffer | Expected
            //      |              |        |        | Size   | Length
            //------------------------------------------------------------
            {  L_,   DEC( 1234.865),    F(3),   B_PTR,       0,       8 },
            {  L_,   DEC(-1234.865),    F(3),   B_PTR,      -1,       9 },
            {  L_,   DEC( 1234.865),    F(3),       0,      -1,       8 },
            {  L_,   DEC( 1234.865),    S(6),   B_PTR,       0,      11 },
            {  L_,   DEC(-1234.865),    S(6),   B_PTR,      -1,      12 },
            {  L_,   DEC( 1234.865),    S(6),       0,      -1,      11 },

            {  L_,   SUBN_P,            F(0),   B_PTR,       0,       4 },
            {  L_,   SUBN_N,            F(0),   B_PTR,       0,       5 },
            {  L_,   INF_P,             F(0),   B_PTR,       0,       3 },
            {  L_,   INF_N,             F(0),   B_PTR,       0,       4 },
            {  L_,   NAN_Q_P,           F(0),   B_PTR,       0,       3 },
            {  L_,   NAN_Q_N,           F(0),   B_PTR,       0,       4 },
            {  L_,   NAN_S_P,           F(0),   B_PTR,       0,       3 },
            {  L_,   NAN_S_N,           F(0),   B_PTR,       0,       4 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                bsl::fill_n(B_PTR, k_BUFFER_SIZE, 'x');

                const int     LINE        = DATA[ti].d_line;
                const Type    DECIMAL     = DATA[ti].d_decimal;
                const Config  CONFIG      = DATA[ti].d_config;
                char         *BUFFER      = DATA[ti].d_buffer;
                const int     BUFFER_SIZE = DATA[ti].d_bufferSize;
                const int     EXPECTED    = DATA[ti].d_expected;

                int len = Util::format(BUFFER,
                                       BUFFER_SIZE,
                                       DECIMAL,
                                       CONFIG);

                LOOP3_ASSERT(LINE, len, EXPECTED, len == EXPECTED);
                ASSERTV(L_, B_PTR == bsl::search_n(B_PTR,
                                                   B_PTR + k_BUFFER_SIZE,
                                                   k_BUFFER_SIZE,
                                                   'x'));
            }
        }
        {
            if (verbose) cout << "Loop-based test" << endl;

            typedef bdldfp::Decimal128 Type;

            const string SIGNIFICANDS[] = {                                "1",
                                                                          "22",
                                                                         "333",
                                                                        "4444",
                                                                       "55555",
                                                                      "666666",
                                                                     "7777777",
                                                                    "88888888",
                                                                   "999999999",
                                                                  "1111111111",
                                                                 "22222222222",
                                                                "333333333333",
                                                               "4444444444444",
                                                              "55555555555555",
                                                             "666666666666666",
                                                            "7777777777777777",
                                                           "88888888888888888",
                                                          "999999999999999999",
                                                         "1111111111111111111",
                                                        "22222222222222222222",
                                                       "333333333333333333333",
                                                      "4444444444444444444444",
                                                     "55555555555555555555555",
                                                    "666666666666666666666666",
                                                   "7777777777777777777777777",
                                                  "88888888888888888888888888",
                                                 "999999999999999999999999999",
                                                "1111111111111111111111111111",
                                               "22222222222222222222222222222",
                                              "333333333333333333333333333333",
                                             "4444444444444444444444444444444",
                                            "55555555555555555555555555555555",
                                           "666666666666666666666666666666666",
                                          "7777777777777777777777777777777777",
            };
            const int NUM_SIGNIFCANDS = static_cast<int>(
                                                       sizeof SIGNIFICANDS
                                                       / sizeof *SIGNIFICANDS);

            const Config CONFIGURATIONS[] = { F(6143),
                                              S(33),
                                              N(33),
            };

            const int NUM_CONFIGURATIONS = static_cast<int>(
                                                     sizeof CONFIGURATIONS
                                                     / sizeof *CONFIGURATIONS);

            const int BUFFER_SIZE = 10000;
            char      BUFFER[BUFFER_SIZE];

            for (int ci = 0; ci < NUM_CONFIGURATIONS; ++ci) {

                const Config& CONFIG =  CONFIGURATIONS[ci];
                const int     EXPONENT_MIN = -6143;
                const int     EXPONENT_MAX =  6144;

                for (int si = 0; si < NUM_SIGNIFCANDS; ++si) {
                    const string SIGNIFICAND = SIGNIFICANDS[si];

                    Type VALUE;
                    DU::parseDecimal128(&VALUE, SIGNIFICAND);

                    for (int ei = EXPONENT_MIN; ei <= EXPONENT_MAX; ++ei) {
                        const int EXPONENT = ei;

                        VALUE = DU::multiplyByPowerOf10(VALUE, EXPONENT);

                        const Type EXPECTED = VALUE;

                        int len = Util::format(BUFFER,
                                               BUFFER_SIZE,
                                               VALUE,
                                               CONFIG);

                        const bsl::string RESULT(BUFFER, len);

                        int  res = DU::parseDecimal128(&VALUE, RESULT);

                        ASSERTV(L_, res, EXPONENT, RESULT, VALUE, 0 == res);
                        LOOP5_ASSERT(L_,
                                     SIGNIFICAND,
                                     EXPONENT,
                                     VALUE,
                                     EXPECTED,
                                     VALUE == EXPECTED);
                    }
                }
            }
        }

        {
            if (verbose) cout << "Table-driven test" << endl;

            typedef bdldfp::Decimal128 Type;

            Type SUBN_P  =  bsl::numeric_limits<Type>::denorm_min();
            Type SUBN_N  = -bsl::numeric_limits<Type>::denorm_min();
            Type INF_P   =  bsl::numeric_limits<Type>::infinity();
            Type INF_N   = -bsl::numeric_limits<Type>::infinity();
            Type NAN_Q_P =  bsl::numeric_limits<Type>::quiet_NaN();
            Type NAN_Q_N = -bsl::numeric_limits<Type>::quiet_NaN();
            Type NAN_S_P =  bsl::numeric_limits<Type>::signaling_NaN();
            Type NAN_S_N = -bsl::numeric_limits<Type>::signaling_NaN();
            Type MAX     =  bsl::numeric_limits<Type>::max();
            Type MIN     =  bsl::numeric_limits<Type>::min();

            static const struct {
                const int          d_line;
                const Type         d_decimal;
                const Config       d_config;
                const char        *d_expected;
            } DATA [] = {
                //-----------------------------------------------------
                // Line | Decimal   | Configuration |  Expected
                //-----------------------------------------------------

                // ----------------------------------------------------
                // FIXED format
                // C-1,3
                {  L_,    SUBN_P,            F(0),       "snan"           },
                {  L_,    SUBN_N,            F(0),      "-snan"           },
                {  L_,    INF_P,             F(0),       "inf"            },
                {  L_,    INF_N,             F(0),      "-inf"            },
                {  L_,    NAN_Q_P,           F(0),       "nan"            },
                {  L_,    NAN_Q_N,           F(0),      "-nan"            },
                {  L_,    NAN_S_P,           F(0),       "nan"            },
                {  L_,    NAN_S_N,           F(0),      "-nan"            },
                {  L_,    MAX,               F(0),        MAX_DECIMAL_128 },
                {  L_,   -MAX,               F(0),    "-" MAX_DECIMAL_128 },
                {  L_,    MIN,               F(6143),     MIN_DECIMAL_128 },
                {  L_,   -MIN,               F(6143), "-" MIN_DECIMAL_128 },
                {  L_,    DEC(-0.0),         F(0),      "-0"              },
                {  L_,    DEC(0.0),          F(0),       "0"              },

                // -----------------------------------------------------------
                // SCIENTIFIC format
                // C-1,4
                {  L_,    SUBN_P,            S(0),    "snan"                 },
                {  L_,    SUBN_N,            S(0),   "-snan"                 },
                {  L_,    INF_P,             S(0),    "inf"                  },
                {  L_,    INF_N,             S(0),   "-inf"                  },
                {  L_,    NAN_Q_P,           S(0),    "nan"                  },
                {  L_,    NAN_Q_N,           S(0),   "-nan"                  },
                {  L_,    NAN_S_P,           S(0),    "nan"                  },
                {  L_,    NAN_S_N,           S(0),   "-nan"                  },
                {  L_,    MAX,               S(33),
                                 "9.999999999999999999999999999999999E+6144" },
                {  L_,   -MAX,               S(33),
                                "-9.999999999999999999999999999999999E+6144" },
                {  L_,    MIN,               S(0),    "1E-6143"              },
                {  L_,   -MIN,               S(0),   "-1E-6143"              },
                {  L_,    DEC(-0.0),         S(0),   "-0E-1"                 },
                {  L_,    DEC(0.0),          S(0),    "0E-1"                 },
                {  L_,    DEC(0.0),          S(1),    "0.0E-1"               },
                {  L_,    DEC(0.00),         S(2),    "0.00E-2"              },
                {  L_,    DEC(0.000),        S(3),    "0.000E-3"             },

                //-------------------------------------------------------------
                // C-7,8
                {  L_,    DEC( 123.0),       F(0),       "123"       },
                {  L_,    DEC(-123.0),       F(0),      "-123"       },
                {  L_,    DEC( 123.0),       S(2),       "1.23E+2"   },
                {  L_,    DEC(-123.0),       S(2),      "-1.23E+2"   },
                {  L_,    DEC( 123.0),       N(0),       "123"       },
                {  L_,    DEC(-123.0),       N(0),      "-123"       },
                {  L_,    NAN_Q_P,           F(0),       "nan"       },
                {  L_,    NAN_Q_N,           F(0),      "-nan"       },
                {  L_,    NAN_S_P,           F(0),       "nan"       },
                {  L_,    NAN_S_N,           F(0),      "-nan"       },
                {  L_,    SUBN_P,            F(0),       "snan"      },
                {  L_,    SUBN_N,            F(0),      "-snan"      },
                {  L_,    INF_P,             F(0),       "inf"       },
                {  L_,    INF_N,             F(0),      "-inf"       },

                //-------------------------------------------------------------
                // C-9
                {  L_,    DEC( 123.0),       FS(0),     "+123"       },
                {  L_,    DEC(-123.0),       FS(0),     "-123"       },
                {  L_,    DEC( 123.0),       SS(2),     "+1.23E+2"   },
                {  L_,    DEC(-123.0),       SS(2),     "-1.23E+2"   },
                {  L_,    DEC( 123.0),       NS(0),     "+123"       },
                {  L_,    DEC(-123.0),       NS(0),     "-123"       },
                {  L_,    NAN_Q_P,           FS(0),     "+nan"       },
                {  L_,    NAN_Q_N,           FS(0),     "-nan"       },
                {  L_,    NAN_S_P,           FS(0),     "+nan"       },
                {  L_,    NAN_S_N,           FS(0),     "-nan"       },
                {  L_,    SUBN_P,            FS(0),     "+snan"      },
                {  L_,    SUBN_N,            FS(0),     "-snan"      },
                {  L_,    INF_P,             FS(0),     "+inf"       },
                {  L_,    INF_N,             FS(0),     "-inf"       },

                //-------------------------------------------------------------
                // C-13
                {  L_,    DEC(1.11),         F(2),      "1.11"       },
                {  L_,    DEC(1.11),         F(3),      "1.110"      },
                {  L_,    DEC(1.11),         F(4),      "1.1100"     },
                {  L_,    DEC(1.11),         S(2),      "1.11E+0"    },
                {  L_,    DEC(1.11),         S(3),      "1.110E+0"   },
                {  L_,    DEC(1.11),         S(4),      "1.1100E+0"  },
                {  L_,    DEC(1.11),         N(2),      "1.11"       },
                {  L_,    DEC(1.11),         N(3),      "1.110"      },
                {  L_,    DEC(1.11),         N(4),      "1.1100"     },

                //-------------------------------------------------------------
                // C-14
                {  L_,    DEC(4.44),         F(2),      "4.44"       },
                {  L_,    DEC(4.44),         F(1),      "4.4"        },
                {  L_,    DEC(4.44),         F(0),      "4"          },
                {  L_,    DEC(5.55),         F(2),      "5.55"       },
                {  L_,    DEC(5.55),         F(1),      "5.6"        },
                {  L_,    DEC(5.55),         F(0),      "6"          },
                {  L_,    DEC(9.99),         F(2),      "9.99"       },
                {  L_,    DEC(9.99),         F(1),      "10.0"       },
                {  L_,    DEC(9.99),         F(0),      "10"         },

                {  L_,    DEC(4.44),         S(2),      "4.44E+0"    },
                {  L_,    DEC(4.44),         S(1),      "4.4E+0"     },
                {  L_,    DEC(4.44),         S(0),      "4E+0"       },
                {  L_,    DEC(5.55),         S(2),      "5.55E+0"    },
                {  L_,    DEC(5.55),         S(1),      "5.6E+0"     },
                {  L_,    DEC(5.55),         S(0),      "6E+0"       },
                {  L_,    DEC(9.99),         S(2),      "9.99E+0"    },
                {  L_,    DEC(9.99),         S(1),      "1.0E+1"     },
                {  L_,    DEC(9.99),         S(0),      "1E+1"       },

                {  L_,    DEC(4.44),         N(2),      "4.44"       },
                {  L_,    DEC(4.44),         N(1),      "4.4"        },
                {  L_,    DEC(4.44),         N(0),      "4"          },
                {  L_,    DEC(5.55),         N(2),      "5.55"       },
                {  L_,    DEC(5.55),         N(1),      "5.6"        },
                {  L_,    DEC(5.55),         N(0),      "6"          },
                {  L_,    DEC(9.99),         N(2),      "9.99"       },
                {  L_,    DEC(9.99),         N(1),      "10.0"       },
                {  L_,    DEC(9.99),         N(0),      "10"         },

                //-------------------------------------------------------------
                // C-15
                {  L_,    SUBN_P,            F(0),      "snan"       },
                {  L_,    SUBN_N,            F(1),     "-snan"       },
                {  L_,    INF_P,             F(2),      "inf"        },
                {  L_,    INF_N,             F(3),     "-inf"        },
                {  L_,    NAN_Q_P,           F(4),      "nan"        },
                {  L_,    NAN_Q_N,           F(5),     "-nan"        },
                {  L_,    NAN_S_P,           F(6),      "nan"        },
                {  L_,    NAN_S_N,           F(7),     "-nan"        },

                {  L_,    SUBN_P,            S(7),      "snan"       },
                {  L_,    SUBN_N,            S(6),     "-snan"       },
                {  L_,    INF_P,             S(5),      "inf"        },
                {  L_,    INF_N,             S(4),     "-inf"        },
                {  L_,    NAN_Q_P,           S(3),      "nan"        },
                {  L_,    NAN_Q_N,           S(2),     "-nan"        },
                {  L_,    NAN_S_P,           S(1),      "nan"        },
                {  L_,    NAN_S_N,           S(0),     "-nan"        },

                {  L_,    SUBN_P,            N(0),     "snan"        },
                {  L_,    SUBN_N,            N(1),    "-snan"        },
                {  L_,    INF_P,             N(2),     "inf"         },
                {  L_,    INF_N,             N(3),    "-inf"         },
                {  L_,    NAN_Q_P,           N(4),     "nan"         },
                {  L_,    NAN_Q_N,           N(5),    "-nan"         },
                {  L_,    NAN_S_P,           N(6),     "nan"         },
                {  L_,    NAN_S_N,           N(7),    "-nan"         },

                //-------------------------------------------------------------
                // C-16
                {  L_,    DEC(1E-7),         N(0),     "1E-7"           },
                {  L_,    DEC(1E-6),         N(6),     "0.000001"       },
                {  L_,    DEC(1.1E-6),       N(7),     "0.0000011"      },
                {  L_,    DEC(1.11E-6),      N(8),     "0.00000111"     },
                {  L_,    DEC(1.111E-6),     N(9),     "0.000001111"    },
                {  L_,    DEC(1.1111E-6),    N(10),    "0.0000011111"   },
                {  L_,    DEC(1.11111E-6),   N(11),    "0.00000111111"  },
                {  L_,    DEC(1.111111E-6),  N(12),    "0.000001111111" },

                {  L_,    DEC(1E+0),         N(0),     "1"              },
                {  L_,    DEC(1.1E+1),       N(0),     "11"             },
                {  L_,    DEC(1.11E+2),      N(0),     "111"            },
                {  L_,    DEC(1.111E+3),     N(0),     "1111"           },
                {  L_,    DEC(1.1111E+4),    N(0),     "11111"          },
                {  L_,    DEC(1.11111E+5),   N(0),     "111111"         },
                {  L_,    DEC(1.111111E+6),  N(0),     "1111111"        },
                {  L_,    DEC(1.111111E+7),  N(6),     "1.111111E+7"    },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            const int k_BUFFER_SIZE = 10000;
            char      buffer[k_BUFFER_SIZE];

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                fill_n(&buffer[0], k_BUFFER_SIZE, 'x');

                const int      LINE        = DATA[ti].d_line;
                const Type     DECIMAL32   = DATA[ti].d_decimal;
                const Config   CONFIG      = DATA[ti].d_config;
                const string&  EXPECTED    = DATA[ti].d_expected;

                int len = Util::format(buffer,
                                       k_BUFFER_SIZE,
                                       DECIMAL32,
                                       CONFIG);

                const string RESULT(buffer, len);
                if (veryVerbose) P_(len) P(RESULT);

                LOOP3_ASSERT(LINE, RESULT, EXPECTED, RESULT == EXPECTED);
                LOOP3_ASSERT(LINE,
                             len,
                             EXPECTED.length(),
                             len == static_cast<int>(EXPECTED.length()));

                //-------------------------------------------------------------
                // C-11
                LOOP2_ASSERT(LINE, buffer[len], 'x' == buffer[len]);

                //-------------------------------------------------------------
                // C-12
                if (FP_NORMAL == DU::classify(DECIMAL32)) {

                    const char *BEGIN     = &buffer[0];
                    const char *END       = &buffer[len];
                    const char *POINT_POS = bsl::find(BEGIN,
                                                      END,
                                                      CONFIG.decimalPoint());
                    if (POINT_POS == END) {
                        LOOP2_ASSERT(LINE,
                                     CONFIG.precision(),
                                     0 == CONFIG.precision());
                    }
                    else {
                        const char *EXPONENT_POS = bsl::find(
                                                            POINT_POS,
                                                            END,
                                                            CONFIG.exponent());
                        LOOP3_ASSERT(LINE,
                                     CONFIG.precision(),
                                     bsl::distance(POINT_POS + 1,
                                                   EXPONENT_POS),
                                     bsl::distance(POINT_POS + 1,
                                                   EXPONENT_POS)
                                     == CONFIG.precision());
                    }
                }
            }

            //-----------------------------------------------------------------
            // C-5
            {
                const Config  DEFAULT_CONFIG(   4);
                const Config  SCIENTIFIC_CONFIG(4, Config::e_SCIENTIFIC);
                const Config  FIXED_CONFIG(     4, Config::e_FIXED);
                const Type    VALUE = DEC(376.12);
                const string  EXPECTED = "3.7612E+2";
                char          DEFAULT_BUFFER[k_BUFFER_SIZE];

                int           len = Util::format(DEFAULT_BUFFER,
                                                 k_BUFFER_SIZE,
                                                 VALUE,
                                                 DEFAULT_CONFIG);

                const string DEFAULT(DEFAULT_BUFFER, len);
                LOOP3_ASSERT(L_, DEFAULT, EXPECTED, DEFAULT == EXPECTED);

                // Test that 'format' produces the same resultant string when
                // invoked with default and scientific configuration.
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   SCIENTIFIC_CONFIG);

                const string SCIENTIFIC_FORMAT(buffer, len);
                LOOP3_ASSERT(L_,
                             SCIENTIFIC_FORMAT,
                             EXPECTED,
                             SCIENTIFIC_FORMAT == EXPECTED);
                LOOP3_ASSERT(L_,
                             SCIENTIFIC_FORMAT,
                             DEFAULT,
                             SCIENTIFIC_FORMAT == DEFAULT);

                // Test that 'format' produces the different resultant strings
                // when invoked with default and fixed configuration.
                const string FIXED_EXPECTED("376.1200");
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   FIXED_CONFIG);

                const bsl::string FIXED_FORMAT(buffer, len);
                LOOP3_ASSERT(L_,
                             FIXED_FORMAT,
                             FIXED_EXPECTED,
                             FIXED_FORMAT == FIXED_EXPECTED);
                LOOP3_ASSERT(L_,
                             FIXED_FORMAT,
                             DEFAULT,
                             FIXED_FORMAT != DEFAULT);
            }

            //-----------------------------------------------------------------
            // C-8
            {
                const Config    DEFAULT_CONFIG(      2);
                const Config    NEGATIVE_ONLY_CONFIG(2,
                                                     Config::e_SCIENTIFIC,
                                                     Config::e_NEGATIVE_ONLY);
                const Config    ALWAYS_CONFIG(       2,
                                                     Config::e_SCIENTIFIC,
                                                     Config::e_ALWAYS);
                const Type   VALUE = DEC(456.0);
                const string EXPECTED = "4.56E+2";
                char         DEFAULT_BUFFER[k_BUFFER_SIZE];

                int          len = Util::format(DEFAULT_BUFFER,
                                                k_BUFFER_SIZE,
                                                VALUE,
                                                DEFAULT_CONFIG);
                const string DEFAULT(DEFAULT_BUFFER, len);
                LOOP3_ASSERT(L_, DEFAULT, EXPECTED, DEFAULT == EXPECTED);

                // Test that 'format' produces the same resultant string when
                // invoked with default and 'negative only' sign configuration.
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   NEGATIVE_ONLY_CONFIG);

                const string    NEGATIVE_ONLY(buffer, len);
                LOOP3_ASSERT(L_,
                             NEGATIVE_ONLY,
                             EXPECTED,
                             NEGATIVE_ONLY == EXPECTED);
                LOOP3_ASSERT(L_,
                             NEGATIVE_ONLY,
                             DEFAULT,
                             NEGATIVE_ONLY == DEFAULT);

                // Test that 'format' produces the different resultant strings
                // when invoked with default and always sign configuration.
                const string  ALWAYS_EXPECTED("+4.56E+2");
                len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   VALUE,
                                   ALWAYS_CONFIG);

                const string  ALWAYS(buffer, len);

                LOOP3_ASSERT(L_,
                             ALWAYS,
                             ALWAYS_EXPECTED,
                             ALWAYS == ALWAYS_EXPECTED);
                LOOP3_ASSERT(L_,
                             ALWAYS,
                             DEFAULT,
                             ALWAYS != DEFAULT);
            }

            if (verbose) cout << "\nNegative Testing." << endl;
            {
                bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

                if (verbose) cout << "\t'buffer == NULL'" << endl;
                {
                    const int  k_SIZE = 1000;
                    char       BUFFER[k_SIZE];
                    const Type V;

                    ASSERT_SAFE_PASS(Util::format(BUFFER, k_SIZE, V));
                    ASSERT_SAFE_FAIL(Util::format(     0, k_SIZE, V));
                }

                if (verbose) cout << "\t'Negative buffer size'" << endl;
                {
                    const int  k_SIZE = 1000;
                    char       BUFFER[k_SIZE];
                    const Type V;

                    ASSERT_SAFE_PASS(Util::format(BUFFER, k_SIZE, V));
                    ASSERT_SAFE_FAIL(Util::format(BUFFER,     -1, V));
                }

                if (verbose) cout << "\t'Negative precision'" << endl;
                {
                    const bdldfp::DecimalFormatConfig VALID(0);
                    const bdldfp::DecimalFormatConfig INVALID(-1);
                    const int                         k_SIZE = 1000;
                    char                              BUFFER[k_SIZE];
                    const Type                        V;

                    ASSERT_SAFE_PASS(Util::format(BUFFER, k_SIZE, V, VALID));
                    ASSERT_SAFE_FAIL(Util::format(BUFFER,     -1, V, INVALID));
                }
            }
#undef DEC
#undef FIXED
#undef SCIENT
#undef NATURAL
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;


        const int          k_BUFFER_SIZE = 256;
        char               buffer[k_BUFFER_SIZE] = {0};
        {
            bdldfp::Decimal32  d32(BDLDFP_DECIMAL_DF(-51.234));
            int len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   d32,
                                   Config(2, Config::e_FIXED));

            string s(buffer, len);
            P_(len) P(s);
        }

        {
            bsl::fill_n(buffer, k_BUFFER_SIZE, 0);

            bdldfp::Decimal32  d32(BDLDFP_DECIMAL_DF(-0.0052345));

            int len = Util::format(buffer,
                                   k_BUFFER_SIZE,
                                   d32,
                                   Config(9, Config::e_FIXED));

            string s(buffer, len);
            P_(len) P(s);
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2017
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
