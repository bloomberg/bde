// bslalg_numericformatterutil.t.cpp                                  -*-C++-*-
#include <bslalg_numericformatterutil.h>

#include <bslmf_assert.h>
#include <bslmf_enableif.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_buildtarget.h>
#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_stopwatch.h>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV)
# include <charconv>
#endif

#include <algorithm>
#include <limits>
#include <string>
#include <sstream>
#include <streambuf>

#include <climits>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cfloat>

#include <stdint.h>  // ... we use the C header due to C++03 support
#include <stdio.h>   // printf, fprintf, puts, setbuf
#include <stdlib.h>  // atoi
#include <math.h>    // nextafterf

#include <ryu/blp_ryu.h>
#include <ryu/ryu.h>

extern "C" {
#include <ryu/blp_common64.h>
#include <ryu/blp_common32.h>
}

#if defined(DELETE)
# undef DELETE  // Awkward Microsoft macro
#endif

using namespace BloombergLP;
using std::numeric_limits;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
//
// In the following cases, 'TYPE' means every integral fundamental type, signed
// or unsigned, up to 64 bits long.
// ----------------------------------------------------------------------------
// [ 2] char *toChars(char *, char *, signed long long int,   int);
// [ 2] char *toChars(char *, char *, unsigned long long int, int);
// [ 3] char *toChars(char *, char *, char,                   int);
// [ 3] char *toChars(char *, char *, signed char,            int);
// [ 3] char *toChars(char *, char *, unsigned char,          int);
// [ 3] char *toChars(char *, char *, short,                  int);
// [ 3] char *toChars(char *, char *, unsigned short,         int);
// [ 3] char *toChars(char *, char *, int,                    int);
// [ 3] char *toChars(char *, char *, unsigned,               int);
// [ 3] char *toChars(char *, char *, long,                   int);
// [ 3] char *toChars(char *, char *, unsigned long,          int);
// [ 3] char *toChars(char *, char *, signed long long int,   int);
// [ 3] char *toChars(char *, char *, unsigned long long int, int);
// [ 4] char *toChars(char *, char *, char,                   int);
// [ 4] char *toChars(char *, char *, signed char,            int);
// [ 4] char *toChars(char *, char *, short,                  int);
// [ 4] char *toChars(char *, char *, int,                    int);
// [ 4] char *toChars(char *, char *, long,                   int);
// [ 4] char *toChars(char *, char *, signed long long int,   int);
// [ 5] char *toChars(char *, char *, char,                   int);
// [ 5] char *toChars(char *, char *, unsigned char,          int);
// [ 5] char *toChars(char *, char *, unsigned short,         int);
// [ 5] char *toChars(char *, char *, unsigned int,           int);
// [ 5] char *toChars(char *, char *, unsigned long,          int);
// [ 5] char *toChars(char *, char *, unsigned long long int, int);
// [ 7] int blp_d2s_buffered_n(double, char *);
// [ 8] int blp_f2s_buffered_n(float, char *);
// [ 9] int blp_d2d_buffered_n(double, char *);
// [10] int blp_f2d_buffered_n(float, char *);
// [11] int blp_d2m_buffered_n(double, char *);
// [12] int blp_f2m_buffered_n(float, char *);
// [13] char *toChars(char *first, char *last, double value);
// [14] char *toChars(char *first, char *last, float value);
// [15] char *toChars(char *first, char *last, double value, format);
// [16] char *toChars(char *first, char *last, float value, format);
// [17] ToCharsMaxLength<T, A>::k_VALUE
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] BLOOMBERG RYU INTERNALS
// [18] USAGE EXAMPLE

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

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

#define T2_          printf("  ");

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//               GLOBAL TYPEDEFS, CONSTANTS, AND VARIABLES
// ----------------------------------------------------------------------------

typedef bslalg::NumericFormatterUtil Util;
typedef unsigned long long int       Uint64;
typedef signed long long int         Int64;

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

namespace {

static const char k_PREFILL_CHAR = '\02';
    // A character we do not expect any 'toChar' function to write into its
    // output.

// ----------------------------------------------------------------------------
//                       'double' shared test data
// ----------------------------------------------------------------------------

static const double dblNegZero = -1 / std::numeric_limits<double>::infinity();

// ----------------------------------------------------------------------------
//                  'double' test data builder functions

static
double makeSubnormalDouble(int64_t mantissa)
    // Return a 'double' that is a (positive) subnormal and has the specified
    // 'mantissa'.  The behavior is undefined unless '0 < mantissa < 2^53'.
{
    double d;
    BSLMF_ASSERT(sizeof mantissa == sizeof d);
    memcpy(&d, &mantissa, sizeof d);
    return d;
}

                            // ==============
                            // struct DblData
                            // ==============

struct DblData {
    // General test data line for 'double' values.

    int         d_line;
    double      d_value;
    const char* d_expected;
};

                          // =================
                          // struct DblIntData
                          // =================

struct DblIntData {
    // Special 'double' test data line for precisely printed integer values.

    int         d_line;
    Int64       d_integer;
    unsigned    d_exponent;  // Use when value is too large for 52 bits
    const char *d_expected;
};

// ----------------------------------------------------------------------------
// Verify 'double' assumptions.

BSLMF_ASSERT(std::numeric_limits<double>::has_infinity);
BSLMF_ASSERT(std::numeric_limits<double>::has_quiet_NaN);
BSLMF_ASSERT(std::numeric_limits<double>::has_signaling_NaN);
BSLMF_ASSERT(std::numeric_limits<double>::has_denorm);

// ----------------------------------------------------------------------------
// 'double' "minimal" format test data.  Macros for "minimal" data are named
// with '_CPP' in their name because '_MIN' would be very confusing, and this
// is the format fully defined by C++ (as opposed to C).
static const DblData k_DBL_CPP[] = {

#define D(value, expected) { L_, value, expected }

           // Verifying C++ compatible scientific format exponent

    D(1.2e12,  "1.2e+12" ), // Exponent '+' is sign displayed
    D(1.2e7,   "1.2e+07" ), // Exponent leading 0 is added for 0-9
    D(1.2e100, "1.2e+100"), // 3 digit exponents work
    D(1.2e-7,  "1.2e-07" ), // Negative exponents work

                    // Verifying negative/positive numbers

    D(-1,      "-1"      ),  // Minus sign written for integers
    D(-12,     "-12"     ),
    D(-120,    "-120"    ),

    D(-1.2,    "-1.2"    ), // Minus sign written in decimal
    D(-0.12,   "-0.12"   ), // format for negative numbers
    D(-0.012,  "-0.012"  ),
    D(-1.2e7,  "-1.2e+07"),

    D(0,           "0"),  // Zero (no significant digits)
    D(dblNegZero, "-0"),  // Negative zero is written as negative

    // Extremes
    D(std::numeric_limits<double>::max(), "1.7976931348623157e+308"),
    D(std::numeric_limits<double>::min(), "2.2250738585072014e-308"),

    // Subnormal/denormal numbers
    D(std::numeric_limits<double>::denorm_min(), "5e-324"),

    D(makeSubnormalDouble(0x0000000000001ull), "5"                  "e-324"),
    D(makeSubnormalDouble(0x0000000000002ull), "1"                  "e-323"),
    D(makeSubnormalDouble(0x0000000000004ull), "2"                  "e-323"),
    D(makeSubnormalDouble(0x0000000000008ull), "4"                  "e-323"),
    D(makeSubnormalDouble(0x0000000000010ull), "8"                  "e-323"),
    D(makeSubnormalDouble(0x0000000000020ull), "1.6"                "e-322"),
    D(makeSubnormalDouble(0x0000000000040ull), "3.16"               "e-322"),
    D(makeSubnormalDouble(0x0000000000080ull), "6.3"                "e-322"),
    D(makeSubnormalDouble(0x0000000000100ull), "1.265"              "e-321"),
    D(makeSubnormalDouble(0x0000000000200ull), "2.53"               "e-321"),
    D(makeSubnormalDouble(0x0000000000400ull), "5.06"               "e-321"),
    D(makeSubnormalDouble(0x0000000000800ull), "1.012"              "e-320"),
    D(makeSubnormalDouble(0x0000000001000ull), "2.0237"             "e-320"),
    D(makeSubnormalDouble(0x0000000002000ull), "4.0474"             "e-320"),
    D(makeSubnormalDouble(0x0000000004000ull), "8.095"              "e-320"),
    D(makeSubnormalDouble(0x0000000008000ull), "1.61895"            "e-319"),
    D(makeSubnormalDouble(0x0000000010000ull), "3.2379"             "e-319"),
    D(makeSubnormalDouble(0x0000000020000ull), "6.4758"             "e-319"),
    D(makeSubnormalDouble(0x0000000040000ull), "1.295163"           "e-318"),
    D(makeSubnormalDouble(0x0000000080000ull), "2.590327"           "e-318"),
    D(makeSubnormalDouble(0x0000000100000ull), "5.180654"           "e-318"),
    D(makeSubnormalDouble(0x0000000200000ull), "1.036131"           "e-317"),
    D(makeSubnormalDouble(0x0000000400000ull), "2.0722615"          "e-317"),
    D(makeSubnormalDouble(0x0000000800000ull), "4.144523"           "e-317"),
    D(makeSubnormalDouble(0x0000001000000ull), "8.289046"           "e-317"),
    D(makeSubnormalDouble(0x0000002000000ull), "1.6578092"          "e-316"),
    D(makeSubnormalDouble(0x0000004000000ull), "3.3156184"          "e-316"),
    D(makeSubnormalDouble(0x0000008000000ull), "6.63123685"         "e-316"),
    D(makeSubnormalDouble(0x0000010000000ull), "1.32624737"         "e-315"),
    D(makeSubnormalDouble(0x0000020000000ull), "2.65249474"         "e-315"),
    D(makeSubnormalDouble(0x0000040000000ull), "5.304989477"        "e-315"),
    D(makeSubnormalDouble(0x0000080000000ull), "1.0609978955"       "e-314"),
    D(makeSubnormalDouble(0x0000100000000ull), "2.121995791"        "e-314"),
    D(makeSubnormalDouble(0x0000200000000ull), "4.243991582"        "e-314"),
    D(makeSubnormalDouble(0x0000400000000ull), "8.487983164"        "e-314"),
    D(makeSubnormalDouble(0x0000800000000ull), "1.69759663277"      "e-313"),
    D(makeSubnormalDouble(0x0001000000000ull), "3.39519326554"      "e-313"),
    D(makeSubnormalDouble(0x0002000000000ull), "6.7903865311"       "e-313"),
    D(makeSubnormalDouble(0x0004000000000ull), "1.35807730622"      "e-312"),
    D(makeSubnormalDouble(0x0008000000000ull), "2.716154612436"     "e-312"),
    D(makeSubnormalDouble(0x0010000000000ull), "5.43230922487"      "e-312"),
    D(makeSubnormalDouble(0x0020000000000ull), "1.086461844974"     "e-311"),
    D(makeSubnormalDouble(0x0040000000000ull), "2.1729236899484"    "e-311"),
    D(makeSubnormalDouble(0x0080000000000ull), "4.345847379897"     "e-311"),
    D(makeSubnormalDouble(0x0100000000000ull), "8.691694759794"     "e-311"),
    D(makeSubnormalDouble(0x0200000000000ull), "1.73833895195875"   "e-310"),
    D(makeSubnormalDouble(0x0400000000000ull), "3.4766779039175"    "e-310"),
    D(makeSubnormalDouble(0x0800000000000ull), "6.953355807835"     "e-310"),
    D(makeSubnormalDouble(0x1000000000000ull), "1.390671161567"     "e-309"),
    D(makeSubnormalDouble(0x2000000000000ull), "2.781342323134"     "e-309"),
    D(makeSubnormalDouble(0x4000000000000ull), "5.562684646268003"  "e-309"),
    D(makeSubnormalDouble(0x8000000000000ull), "1.1125369292536007" "e-308"),

    D(makeSubnormalDouble(0x0000000000003ull), "1.5"                "e-323"),
    D(makeSubnormalDouble(0x0000000000007ull), "3.5"                "e-323"),
    D(makeSubnormalDouble(0x000000000000Full), "7.4"                "e-323"),
    D(makeSubnormalDouble(0x000000000001Full), "1.53"               "e-322"),
    D(makeSubnormalDouble(0x000000000003Full), "3.1"                "e-322"),
    D(makeSubnormalDouble(0x000000000007Full), "6.27"               "e-322"),
    D(makeSubnormalDouble(0x00000000000FFull), "1.26"               "e-321"),
    D(makeSubnormalDouble(0x00000000001FFull), "2.525"              "e-321"),
    D(makeSubnormalDouble(0x00000000003FFull), "5.054"              "e-321"),
    D(makeSubnormalDouble(0x00000000007FFull), "1.0114"             "e-320"),
    D(makeSubnormalDouble(0x0000000000FFFull), "2.023"              "e-320"),
    D(makeSubnormalDouble(0x0000000001FFFull), "4.047"              "e-320"),
    D(makeSubnormalDouble(0x0000000003FFFull), "8.0943"             "e-320"),
    D(makeSubnormalDouble(0x0000000007FFFull), "1.6189"             "e-319"),
    D(makeSubnormalDouble(0x000000000FFFFull), "3.23786"            "e-319"),
    D(makeSubnormalDouble(0x000000001FFFFull), "6.47577"            "e-319"),
    D(makeSubnormalDouble(0x000000003FFFFull), "1.29516"            "e-318"),
    D(makeSubnormalDouble(0x000000007FFFFull), "2.59032"            "e-318"),
    D(makeSubnormalDouble(0x00000000FFFFFull), "5.18065"            "e-318"),
    D(makeSubnormalDouble(0x00000001FFFFFull), "1.0361303"          "e-317"),
    D(makeSubnormalDouble(0x00000003FFFFFull), "2.072261"           "e-317"),
    D(makeSubnormalDouble(0x00000007FFFFFull), "4.1445225"          "e-317"),
    D(makeSubnormalDouble(0x0000000FFFFFFull), "8.2890456"          "e-317"),
    D(makeSubnormalDouble(0x0000001FFFFFFull), "1.65780916"         "e-316"),
    D(makeSubnormalDouble(0x0000003FFFFFFull), "3.31561837"         "e-316"),
    D(makeSubnormalDouble(0x0000007FFFFFFull), "6.6312368"          "e-316"),
    D(makeSubnormalDouble(0x000000FFFFFFFull), "1.326247364"        "e-315"),
    D(makeSubnormalDouble(0x000001FFFFFFFull), "2.652494734"        "e-315"),
    D(makeSubnormalDouble(0x000003FFFFFFFull), "5.304989472"        "e-315"),
    D(makeSubnormalDouble(0x000007FFFFFFFull), "1.060997895"        "e-314"),
    D(makeSubnormalDouble(0x00000FFFFFFFFull), "2.1219957905"       "e-314"),
    D(makeSubnormalDouble(0x00001FFFFFFFFull), "4.2439915814"       "e-314"),
    D(makeSubnormalDouble(0x00003FFFFFFFFull), "8.4879831634"       "e-314"),
    D(makeSubnormalDouble(0x00007FFFFFFFFull), "1.6975966327"       "e-313"),
    D(makeSubnormalDouble(0x0000FFFFFFFFFull), "3.3951932655"       "e-313"),
    D(makeSubnormalDouble(0x0001FFFFFFFFFull), "6.79038653104"      "e-313"),
    D(makeSubnormalDouble(0x0003FFFFFFFFFull), "1.358077306213"     "e-312"),
    D(makeSubnormalDouble(0x0007FFFFFFFFFull), "2.71615461243"      "e-312"),
    D(makeSubnormalDouble(0x000FFFFFFFFFFull), "5.432309224866"     "e-312"),
    D(makeSubnormalDouble(0x001FFFFFFFFFFull), "1.0864618449737"    "e-311"),
    D(makeSubnormalDouble(0x003FFFFFFFFFFull), "2.172923689948"     "e-311"),
    D(makeSubnormalDouble(0x007FFFFFFFFFFull), "4.3458473798964"    "e-311"),
    D(makeSubnormalDouble(0x00FFFFFFFFFFFull), "8.6916947597933"    "e-311"),
    D(makeSubnormalDouble(0x01FFFFFFFFFFFull), "1.7383389519587"    "e-310"),
    D(makeSubnormalDouble(0x03FFFFFFFFFFFull), "3.47667790391745"   "e-310"),
    D(makeSubnormalDouble(0x07FFFFFFFFFFFull), "6.95335580783495"   "e-310"),
    D(makeSubnormalDouble(0x0FFFFFFFFFFFFull), "1.390671161566996"  "e-309"),
    D(makeSubnormalDouble(0x1FFFFFFFFFFFFull), "2.781342323133997"  "e-309"),
    D(makeSubnormalDouble(0x3FFFFFFFFFFFFull), "5.562684646268"     "e-309"),
    D(makeSubnormalDouble(0x7FFFFFFFFFFFFull), "1.1125369292536"    "e-308"),
    D(makeSubnormalDouble(0xFFFFFFFFFFFFFull), "2.225073858507201"  "e-308"),

    D(makeSubnormalDouble(0x0000000000005ull), "2.5"                "e-323"),
    D(makeSubnormalDouble(0x000000000000Aull), "5"                  "e-323"),
    D(makeSubnormalDouble(0x0000000000015ull), "1.04"               "e-322"),
    D(makeSubnormalDouble(0x000000000002Aull), "2.08"               "e-322"),
    D(makeSubnormalDouble(0x0000000000055ull), "4.2"                "e-322"),
    D(makeSubnormalDouble(0x00000000000AAull), "8.4"                "e-322"),
    D(makeSubnormalDouble(0x0000000000155ull), "1.685"              "e-321"),
    D(makeSubnormalDouble(0x00000000002AAull), "3.37"               "e-321"),
    D(makeSubnormalDouble(0x0000000000555ull), "6.744"              "e-321"),
    D(makeSubnormalDouble(0x0000000000AAAull), "1.349"              "e-320"),
    D(makeSubnormalDouble(0x0000000001555ull), "2.698"              "e-320"),
    D(makeSubnormalDouble(0x0000000002AAAull), "5.396"              "e-320"),
    D(makeSubnormalDouble(0x0000000005555ull), "1.0793"             "e-319"),
    D(makeSubnormalDouble(0x000000000AAAAull), "2.15857"            "e-319"),
    D(makeSubnormalDouble(0x0000000015555ull), "4.3172"             "e-319"),
    D(makeSubnormalDouble(0x000000002AAAAull), "8.6344"             "e-319"),
    D(makeSubnormalDouble(0x0000000055555ull), "1.726883"           "e-318"),
    D(makeSubnormalDouble(0x00000000AAAAAull), "3.453766"           "e-318"),
    D(makeSubnormalDouble(0x0000000155555ull), "6.907537"           "e-318"),
    D(makeSubnormalDouble(0x00000002AAAAAull), "1.3815073"          "e-317"),
    D(makeSubnormalDouble(0x0000000555555ull), "2.763015"           "e-317"),
    D(makeSubnormalDouble(0x0000000AAAAAAull), "5.5260304"          "e-317"),
    D(makeSubnormalDouble(0x0000001555555ull), "1.1052061"          "e-316"),
    D(makeSubnormalDouble(0x0000002AAAAAAull), "2.21041225"         "e-316"),
    D(makeSubnormalDouble(0x0000005555555ull), "4.42082455"         "e-316"),
    D(makeSubnormalDouble(0x000000AAAAAAAull), "8.8416491"          "e-316"),
    D(makeSubnormalDouble(0x0000015555555ull), "1.768329824"        "e-315"),
    D(makeSubnormalDouble(0x000002AAAAAAAull), "3.53665965"         "e-315"),
    D(makeSubnormalDouble(0x0000055555555ull), "7.0733193"          "e-315"),
    D(makeSubnormalDouble(0x00000AAAAAAAAull), "1.4146638603"       "e-314"),
    D(makeSubnormalDouble(0x0000155555555ull), "2.829327721"        "e-314"),
    D(makeSubnormalDouble(0x00001AAAAAAAAull), "3.5366596513"       "e-314"),
    D(makeSubnormalDouble(0x0000555555555ull), "1.1317310885"       "e-313"),
    D(makeSubnormalDouble(0x0000AAAAAAAAAull), "2.263462177"        "e-313"),
    D(makeSubnormalDouble(0x0001555555555ull), "4.52692435404"      "e-313"),
    D(makeSubnormalDouble(0x0002AAAAAAAAAull), "9.0538487081"       "e-313"),
    D(makeSubnormalDouble(0x0005555555555ull), "1.81076974162"      "e-312"),
    D(makeSubnormalDouble(0x000AAAAAAAAAAull), "3.621539483244"     "e-312"),
    D(makeSubnormalDouble(0x0015555555555ull), "7.243078966493"     "e-312"),
    D(makeSubnormalDouble(0x002AAAAAAAAAAull), "1.4486157932986"    "e-311"),
    D(makeSubnormalDouble(0x0055555555555ull), "2.897231586598"     "e-311"),
    D(makeSubnormalDouble(0x00AAAAAAAAAAAull), "5.7944631731955"    "e-311"),
    D(makeSubnormalDouble(0x0155555555555ull), "1.15889263463915"   "e-310"),
    D(makeSubnormalDouble(0x02AAAAAAAAAAAull), "2.3177852692783"    "e-310"),
    D(makeSubnormalDouble(0x0555555555555ull), "4.63557053855665"   "e-310"),
    D(makeSubnormalDouble(0x0AAAAAAAAAAAAull), "9.2711410771133"    "e-310"),
    D(makeSubnormalDouble(0x1555555555555ull), "1.854228215422666"  "e-309"),
    D(makeSubnormalDouble(0x2AAAAAAAAAAAAull), "3.70845643084533"   "e-309"),
    D(makeSubnormalDouble(0x5555555555555ull), "7.41691286169067"   "e-309"),
    D(makeSubnormalDouble(0xAAAAAAAAAAAAAull), "1.483382572338134"  "e-308"),

    D(+1,      "1"  ), // Plus sign not written for integers
    D(+12,     "12" ),
    D(+120,    "120"),

    D(+1.2,    "1.2"    ), // Plus sign not written in decimal
    D(+0.12,   "0.12"   ), // format
    D(+0.012,  "0.012"  ),

    D(+1.2e7,  "1.2e+07"), // Plus is not written in scientific

            // Verifying decimal/scientific format selection

    D(1.,                 "1"                ), // Random integers
    D(1234567890.,        "1234567890"       ), // that are written in
    D(12345678901234566., "12345678901234566"), // decimal format

    D(12345678901234566000000., "1.2345678901234566e+22"),
        // Integer that just switches to scientific format due to length

    // Single significant digit
    D(2e-5, "2e-05"),
    D(2e-4, "2e-04"),
    D(2e-3, "0.002"),
    D(2e-2, "0.02" ),
    D(2e-1, "0.2"  ),
    D(2e0,  "2"    ),
    D(2e1,  "20"   ),
    D(2e2,  "200"  ),
    D(2e3,  "2000" ),
    D(2e4,  "20000"),
    D(2e5,  "2e+05"),
    D(2e6,  "2e+06"),

    // Two significant digits
    D(1.2e-6, "1.2e-06"),
    D(1.2e-5, "1.2e-05"),
    D(1.2e-4, "0.00012"),
    D(1.2e-3, "0.0012" ),
    D(1.2e-2, "0.012"  ),
    D(1.2e-1, "0.12"   ),
    D(1.2e0,  "1.2"    ),
    D(1.2e1,  "12"     ),
    D(1.2e2,  "120"    ),
    D(1.2e3,  "1200"   ),
    D(1.2e4,  "12000"  ),
    D(1.2e5,  "120000" ),
    D(1.2e6,  "1200000"),
    D(1.2e7,  "1.2e+07"),
    D(1.2e8,  "1.2e+08"),

    // Three significant digits
    D(1.23e-6, "1.23e-06"),
    D(1.23e-5, "1.23e-05"),
    D(1.23e-4, "0.000123"),
    D(1.23e-3, "0.00123" ),
    D(1.23e-2, "0.0123"  ),
    D(1.23e-1, "0.123"   ),
    D(1.23e0,  "1.23"    ),
    D(1.23e1,  "12.3"    ),
    D(1.23e2,  "123"     ),
    D(1.23e3,  "1230"    ),
    D(1.23e4,  "12300"   ),
    D(1.23e5,  "123000"  ),
    D(1.23e6,  "1230000" ),
    D(1.23e7,  "12300000"),
    D(1.23e8,  "1.23e+08"),
    D(1.23e9,  "1.23e+09"),

    // Four significant digits
    D(1.234e-6, "1.234e-06"),
    D(1.234e-5, "1.234e-05"),
    D(1.234e-4, "0.0001234"),
    D(1.234e-3, "0.001234" ),
    D(1.234e-2, "0.01234"  ),
    D(1.234e-1, "0.1234"   ),
    D(1.234e0,  "1.234"    ),
    D(1.234e1,  "12.34"    ),
    D(1.234e2,  "123.4"    ),
    D(1.234e3,  "1234"     ),
    D(1.234e4,  "12340"    ),
    D(1.234e5,  "123400"   ),
    D(1.234e6,  "1234000"  ),
    D(1.234e7,  "12340000" ),
    D(1.234e8,  "123400000"),
    D(1.234e9,  "1.234e+09"),
    D(1.234e10, "1.234e+10"),

    // Five significant digits
    D(1.2345e-6, "1.2345e-06"),
    D(1.2345e-5, "1.2345e-05"),
    D(1.2345e-4, "0.00012345"),
    D(1.2345e-3, "0.0012345" ),
    D(1.2345e-2, "0.012345"  ),
    D(1.2345e-1, "0.12345"   ),
    D(1.2345e0,  "1.2345"    ),
    D(1.2345e1,  "12.345"    ),
    D(1.2345e2,  "123.45"    ),
    D(1.2345e3,  "1234.5"    ),
    D(1.2345e4,  "12345"     ),
    D(1.2345e5,  "123450"    ),
    D(1.2345e6,  "1234500"   ),
    D(1.2345e7,  "12345000"  ),
    D(1.2345e8,  "123450000" ),
    D(1.2345e9,  "1234500000"),
    D(1.2345e10, "1.2345e+10"),
    D(1.2345e11, "1.2345e+11"),

    // Six significant digits
    D(1.23456e-6, "1.23456e-06"),
    D(1.23456e-5, "1.23456e-05"),
    D(1.23456e-4, "0.000123456"),
    D(1.23456e-3, "0.00123456" ),
    D(1.23456e-2, "0.0123456"  ),
    D(1.23456e-1, "0.123456"   ),
    D(1.23456e0,  "1.23456"    ),
    D(1.23456e1,  "12.3456"    ),
    D(1.23456e2,  "123.456"    ),
    D(1.23456e3,  "1234.56"    ),
    D(1.23456e4,  "12345.6"    ),
    D(1.23456e5,  "123456"     ),
    D(1.23456e6,  "1234560"    ),
    D(1.23456e7,  "12345600"   ),
    D(1.23456e8,  "123456000"  ),
    D(1.23456e9,  "1234560000" ),
    D(1.23456e10, "12345600000"),
    D(1.23456e11, "1.23456e+11"),
    D(1.23456e12, "1.23456e+12"),

    // Seven significant digits
    D(1.234567e-6, "1.234567e-06"),
    D(1.234567e-5, "1.234567e-05"),
    D(1.234567e-4, "0.0001234567"),
    D(1.234567e-3, "0.001234567" ),
    D(1.234567e-2, "0.01234567"  ),
    D(1.234567e-1, "0.1234567"   ),
    D(1.234567e0,  "1.234567"    ),
    D(1.234567e1,  "12.34567"    ),
    D(1.234567e2,  "123.4567"    ),
    D(1.234567e3,  "1234.567"    ),
    D(1.234567e4,  "12345.67"    ),
    D(1.234567e5,  "123456.7"    ),
    D(1.234567e6,  "1234567"     ),
    D(1.234567e7,  "12345670"    ),
    D(1.234567e8,  "123456700"   ),
    D(1.234567e9,  "1234567000"  ),
    D(1.234567e10, "12345670000" ),
    D(1.234567e11, "123456700000"),
    D(1.234567e12, "1.234567e+12"),
    D(1.234567e13, "1.234567e+13"),

    // Eight significant digits
    D(1.2345678e-6, "1.2345678e-06"),
    D(1.2345678e-5, "1.2345678e-05"),
    D(1.2345678e-4, "0.00012345678"),
    D(1.2345678e-3, "0.0012345678" ),
    D(1.2345678e-2, "0.012345678"  ),
    D(1.2345678e-1, "0.12345678"   ),
    D(1.2345678e0,  "1.2345678"    ),
    D(1.2345678e1,  "12.345678"    ),
    D(1.2345678e2,  "123.45678"    ),
    D(1.2345678e3,  "1234.5678"    ),
    D(1.2345678e4,  "12345.678"    ),
    D(1.2345678e5,  "123456.78"    ),
    D(1.2345678e6,  "1234567.8"    ),
    D(1.2345678e7,  "12345678"     ),
    D(1.2345678e8,  "123456780"    ),
    D(1.2345678e9,  "1234567800"   ),
    D(1.2345678e10, "12345678000"  ),
    D(1.2345678e11, "123456780000" ),
    D(1.2345678e12, "1234567800000"),
    D(1.2345678e13, "1.2345678e+13"),
    D(1.2345678e14, "1.2345678e+14"),

    // Nine significant digits
    D(1.23456789e-6, "1.23456789e-06"),
    D(1.23456789e-5, "1.23456789e-05"),
    D(1.23456789e-4, "0.000123456789"),
    D(1.23456789e-3, "0.00123456789" ),
    D(1.23456789e-2, "0.0123456789"  ),
    D(1.23456789e-1, "0.123456789"   ),
    D(1.23456789e0,  "1.23456789"    ),
    D(1.23456789e1,  "12.3456789"    ),
    D(1.23456789e2,  "123.456789"    ),
    D(1.23456789e3,  "1234.56789"    ),
    D(1.23456789e4,  "12345.6789"    ),
    D(1.23456789e5,  "123456.789"    ),
    D(1.23456789e6,  "1234567.89"    ),
    D(1.23456789e7,  "12345678.9"    ),
    D(1.23456789e8,  "123456789"     ),
    D(1.23456789e9,  "1234567890"    ),
    D(1.23456789e10, "12345678900"   ),
    D(1.23456789e11, "123456789000"  ),
    D(1.23456789e12, "1234567890000" ),
    D(1.23456789e13, "12345678900000"),
    D(1.23456789e14, "1.23456789e+14"),
    D(1.23456789e15, "1.23456789e+15"),

    // Ten significant digits
    D(1.234567898e-6, "1.234567898e-06"),
    D(1.234567898e-5, "1.234567898e-05"),
    D(1.234567898e-4, "0.0001234567898"),
    D(1.234567898e-3, "0.001234567898" ),
    D(1.234567898e-2, "0.01234567898"  ),
    D(1.234567898e-1, "0.1234567898"   ),
    D(1.234567898e0,  "1.234567898"    ),
    D(1.234567898e1,  "12.34567898"    ),
    D(1.234567898e2,  "123.4567898"    ),
    D(1.234567898e3,  "1234.567898"    ),
    D(1.234567898e4,  "12345.67898"    ),
    D(1.234567898e5,  "123456.7898"    ),
    D(1.234567898e6,  "1234567.898"    ),
    D(1.234567898e7,  "12345678.98"    ),
    D(1.234567898e8,  "123456789.8"    ),
    D(1.234567898e9,  "1234567898"     ),
    D(1.234567898e10, "12345678980"    ),
    D(1.234567898e11, "123456789800"   ),
    D(1.234567898e12, "1234567898000"  ),
    D(1.234567898e13, "12345678980000" ),
    D(1.234567898e14, "123456789800000"),
    D(1.234567898e15, "1.234567898e+15"),
    D(1.234567898e16, "1.234567898e+16"),

    // Eleven significant digits
    D(1.2345678987e-6, "1.2345678987e-06"),
    D(1.2345678987e-5, "1.2345678987e-05"),
    D(1.2345678987e-4, "0.00012345678987"),
    D(1.2345678987e-3, "0.0012345678987" ),
    D(1.2345678987e-2, "0.012345678987"  ),
    D(1.2345678987e-1, "0.12345678987"   ),
    D(1.2345678987e0,  "1.2345678987"    ),
    D(1.2345678987e1,  "12.345678987"    ),
    D(1.2345678987e2,  "123.45678987"    ),
    D(1.2345678987e3,  "1234.5678987"    ),
    D(1.2345678987e4,  "12345.678987"    ),
    D(1.2345678987e5,  "123456.78987"    ),
    D(1.2345678987e6,  "1234567.8987"    ),
    D(1.2345678987e7,  "12345678.987"    ),
    D(1.2345678987e8,  "123456789.87"    ),
    D(1.2345678987e9,  "1234567898.7"    ),
    D(1.2345678987e10, "12345678987"     ),
    D(1.2345678987e11, "123456789870"    ),
    D(1.2345678987e12, "1234567898700"   ),
    D(1.2345678987e13, "12345678987000"  ),
    D(1.2345678987e14, "123456789870000" ),
    D(1.2345678987e15, "1234567898700000"),
    D(1.2345678987e16, "1.2345678987e+16"),
    D(1.2345678987e17, "1.2345678987e+17"),

    // Twelve significant digits
    D(1.23456789876e-6, "1.23456789876e-06"),
    D(1.23456789876e-5, "1.23456789876e-05"),
    D(1.23456789876e-4, "0.000123456789876"),
    D(1.23456789876e-3, "0.00123456789876" ),
    D(1.23456789876e-2, "0.0123456789876"  ),
    D(1.23456789876e-1, "0.123456789876"   ),
    D(1.23456789876e0,  "1.23456789876"    ),
    D(1.23456789876e1,  "12.3456789876"    ),
    D(1.23456789876e2,  "123.456789876"    ),
    D(1.23456789876e3,  "1234.56789876"    ),
    D(1.23456789876e4,  "12345.6789876"    ),
    D(1.23456789876e5,  "123456.789876"    ),
    D(1.23456789876e6,  "1234567.89876"    ),
    D(1.23456789876e7,  "12345678.9876"    ),
    D(1.23456789876e8,  "123456789.876"    ),
    D(1.23456789876e9,  "1234567898.76"    ),
    D(1.23456789876e10, "12345678987.6"    ),
    D(1.23456789876e11, "123456789876"     ),
    D(1.23456789876e12, "1234567898760"    ),
    D(1.23456789876e13, "12345678987600"   ),
    D(1.23456789876e14, "123456789876000"  ),
    D(1.23456789876e15, "1234567898760000" ),
    D(1.23456789876e16, "12345678987600000"),
    D(1.23456789876e17, "1.23456789876e+17"),
    D(1.23456789876e18, "1.23456789876e+18"),

    // Thirteen significant digits
    D(1.234567898765e-6, "1.234567898765e-06"),
    D(1.234567898765e-5, "1.234567898765e-05"),
    D(1.234567898765e-4, "0.0001234567898765"),
    D(1.234567898765e-3, "0.001234567898765" ),
    D(1.234567898765e-2, "0.01234567898765"  ),
    D(1.234567898765e-1, "0.1234567898765"   ),
    D(1.234567898765e0,  "1.234567898765"    ),
    D(1.234567898765e1,  "12.34567898765"    ),
    D(1.234567898765e2,  "123.4567898765"    ),
    D(1.234567898765e3,  "1234.567898765"    ),
    D(1.234567898765e4,  "12345.67898765"    ),
    D(1.234567898765e5,  "123456.7898765"    ),
    D(1.234567898765e6,  "1234567.898765"    ),
    D(1.234567898765e7,  "12345678.98765"    ),
    D(1.234567898765e8,  "123456789.8765"    ),
    D(1.234567898765e9,  "1234567898.765"    ),
    D(1.234567898765e10, "12345678987.65"    ),
    D(1.234567898765e11, "123456789876.5"    ),
    D(1.234567898765e12, "1234567898765"     ),
    D(1.234567898765e13, "12345678987650"    ),
    D(1.234567898765e14, "123456789876500"   ),
    D(1.234567898765e15, "1234567898765000"  ),
    D(1.234567898765e16, "12345678987650000" ),
    D(1.234567898765e17, "123456789876500000"),
    D(1.234567898765e18, "1.234567898765e+18"),
    D(1.234567898765e19, "1.234567898765e+19"),

    // Fourteen significant digits
    D(1.2345678987654e-6, "1.2345678987654e-06"),
    D(1.2345678987654e-5, "1.2345678987654e-05"),
    D(1.2345678987654e-4, "0.00012345678987654"),
    D(1.2345678987654e-3, "0.0012345678987654" ),
    D(1.2345678987654e-2, "0.012345678987654"  ),
    D(1.2345678987654e-1, "0.12345678987654"   ),
    D(1.2345678987654e0,  "1.2345678987654"    ),
    D(1.2345678987654e1,  "12.345678987654"    ),
    D(1.2345678987654e2,  "123.45678987654"    ),
    D(1.2345678987654e3,  "1234.5678987654"    ),
    D(1.2345678987654e4,  "12345.678987654"    ),
    D(1.2345678987654e5,  "123456.78987654"    ),
    D(1.2345678987654e6,  "1234567.8987654"    ),
    D(1.2345678987654e7,  "12345678.987654"    ),
    D(1.2345678987654e8,  "123456789.87654"    ),
    D(1.2345678987654e9,  "1234567898.7654"    ),
    D(1.2345678987654e10, "12345678987.654"    ),
    D(1.2345678987654e11, "123456789876.54"    ),
    D(1.2345678987654e12, "1234567898765.4"    ),
    D(1.2345678987654e13, "12345678987654"     ),
    D(1.2345678987654e14, "123456789876540"    ),
    D(1.2345678987654e15, "1234567898765400"   ),
    D(1.2345678987654e16, "12345678987654000"  ),
    D(1.2345678987654e17, "123456789876540000" ),
    D(1.2345678987654e18, "1234567898765400064"),
    D(1.2345678987654e19, "1.2345678987654e+19"),
    D(1.2345678987654e20, "1.2345678987654e+20"),

    // Fifteen significant digits
    D(1.23456789876543e-6, "1.23456789876543e-06"),
    D(1.23456789876543e-5, "1.23456789876543e-05"),
    D(1.23456789876543e-4, "0.000123456789876543"),
    D(1.23456789876543e-3, "0.00123456789876543" ),
    D(1.23456789876543e-2, "0.0123456789876543"  ),
    D(1.23456789876543e-1, "0.123456789876543"   ),
    D(1.23456789876543e0,  "1.23456789876543"    ),
    D(1.23456789876543e1,  "12.3456789876543"    ),
    D(1.23456789876543e2,  "123.456789876543"    ),
    D(1.23456789876543e3,  "1234.56789876543"    ),
    D(1.23456789876543e4,  "12345.6789876543"    ),
    D(1.23456789876543e5,  "123456.789876543"    ),
    D(1.23456789876543e6,  "1234567.89876543"    ),
    D(1.23456789876543e7,  "12345678.9876543"    ),
    D(1.23456789876543e8,  "123456789.876543"    ),
    D(1.23456789876543e9,  "1234567898.76543"    ),
    D(1.23456789876543e10, "12345678987.6543"    ),
    D(1.23456789876543e11, "123456789876.543"    ),
    D(1.23456789876543e12, "1234567898765.43"    ),
    D(1.23456789876543e13, "12345678987654.3"    ),
    D(1.23456789876543e14, "123456789876543"     ),
    D(1.23456789876543e15, "1234567898765430"    ),
    D(1.23456789876543e16, "12345678987654300"   ),
    D(1.23456789876543e17, "123456789876543008"  ),
    D(1.23456789876543e18, "1234567898765430016" ),
    D(1.23456789876543e19, "12345678987654299648"),
    D(1.23456789876543e20, "1.23456789876543e+20"),
    D(1.23456789876543e21, "1.23456789876543e+21"),

    // Sixteen significant digits
    D(1.234567898765432e-6, "1.234567898765432e-06"),
    D(1.234567898765432e-5, "1.234567898765432e-05"),
    D(1.234567898765432e-4, "0.0001234567898765432"),
    D(1.234567898765432e-3, "0.001234567898765432" ),
    D(1.234567898765432e-2, "0.01234567898765432"  ),
    D(1.234567898765432e-1, "0.1234567898765432"   ),
    D(1.234567898765432e0,  "1.234567898765432"    ),
    D(1.234567898765432e1,  "12.34567898765432"    ),
    D(1.234567898765432e2,  "123.4567898765432"    ),
    D(1.234567898765432e3,  "1234.567898765432"    ),
    D(1.234567898765432e4,  "12345.67898765432"    ),
    D(1.234567898765432e5,  "123456.7898765432"    ),
    D(1.234567898765432e6,  "1234567.898765432"    ),
    D(1.234567898765432e7,  "12345678.98765432"    ),
    D(1.234567898765432e8,  "123456789.8765432"    ),
    D(1.234567898765432e9,  "1234567898.765432"    ),
    D(1.234567898765432e10, "12345678987.65432"    ),
    D(1.234567898765432e11, "123456789876.5432"    ),
    D(1.234567898765432e12, "1234567898765.432"    ),
    D(1.234567898765432e13, "12345678987654.32"    ),
    D(1.234567898765432e14, "123456789876543.2"    ),
    D(1.234567898765432e15, "1234567898765432"     ),
    D(1.234567898765432e16, "12345678987654320"    ),
    D(1.234567898765432e17, "123456789876543200"   ),
    D(1.234567898765432e18, "1234567898765432064"  ),
    D(1.234567898765432e19, "12345678987654320128" ),
    D(1.234567898765432e20, "123456789876543193088"),
    D(1.234567898765432e21, "1.234567898765432e+21"),
    D(1.234567898765432e22, "1.234567898765432e+22"),

    // Seventeen significant digits
    D(1.2345678987654324e-6, "1.2345678987654324e-06"),
    D(1.2345678987654324e-5, "1.2345678987654324e-05"),
    D(1.2345678987654325e-4, "0.00012345678987654325"),
    D(1.2345678987654323e-3, "0.0012345678987654323" ),
    D(1.2345678987654323e-2, "0.012345678987654323"  ),
    D(1.2345678987654324e-1, "0.12345678987654324"   ),
    D(1.2345678987654325e0,  "1.2345678987654325"    ),
    D(1.2345678987654324e1,  "12.345678987654324"    ),
    D(1.2345678987654324e2,  "123.45678987654324"    ),
    D(1.2345678987654323e3,  "1234.5678987654323"    ),
    D(1.2345678987654324e4,  "12345.678987654324"    ),
    D(1.2345678987654323e5,  "123456.78987654323"    ),
    D(1.2345678987654324e6,  "1234567.8987654324"    ),
    D(1.2345678987654325e7,  "12345678.987654325"    ),
    D(1.2345678987654324e8,  "123456789.87654324"    ),
    D(1.2345678987654324e9,  "1234567898.7654324"    ),
    D(1.2345678987654324e10, "12345678987.654324"    ),
    D(1.2345678987654324e11, "123456789876.54324"    ),
    D(1.2345678987654324e12, "1234567898765.4324"    ),
    D(1.2345678987654324e13, "12345678987654.324"    ),
    D(1.2345678987654323e14, "123456789876543.23"    ),
    D(1.2345678987654325e15, "1234567898765432.5"    ),
    D(1.2345678987654324e16, "12345678987654324"     ),
    D(1.2345678987654323e17, "123456789876543232"    ),
    D(1.2345678987654323e18, "1234567898765432320"   ),
    D(1.2345678987654324e19, "12345678987654324224"  ),
    D(1.2345678987654324e20, "123456789876543242240" ),
    D(1.2345678987654325e21, "1234567898765432455168"),
    D(1.2345678987654324e22, "1.2345678987654324e+22"),
    D(1.2345678987654324e23, "1.2345678987654324e+23"),

                  // Leading decimal zero(s) regression

    // Three significant digits
    D(1.03e0,  "1.03"),

    // Four significant digits
    D(1.034e0,  "1.034"),
    D(1.004e0,  "1.004"),
    D(1.204e1,  "12.04"),

    // Five significant digits
    D(1.0345e0,  "1.0345"),
    D(1.0045e0,  "1.0045"),
    D(1.0005e0,  "1.0005"),
    D(1.2045e1,  "12.045"),
    D(1.2005e1,  "12.005"),
    D(1.2305e2,  "123.05"),

    // Six significant digits
    D(1.03456e0,  "1.03456"),
    D(1.00456e0,  "1.00456"),
    D(1.00056e0,  "1.00056"),
    D(1.00006e0,  "1.00006"),
    D(1.20456e1,  "12.0456"),
    D(1.20056e1,  "12.0056"),
    D(1.20006e1,  "12.0006"),
    D(1.23056e2,  "123.056"),
    D(1.23006e2,  "123.006"),
    D(1.23406e3,  "1234.06"),

    // Sixteen significant digits
    D(1.034567898765432e0,  "1.034567898765432"),
    D(1.004567898765432e0,  "1.004567898765432"),
    D(1.000567898765432e0,  "1.000567898765432"),
    D(1.000067898765432e0,  "1.000067898765432"),
    D(1.000007898765432e0,  "1.000007898765432"),
    D(1.000000898765432e0,  "1.000000898765432"),
    D(1.000000098765432e0,  "1.000000098765432"),
    D(1.000000008765432e0,  "1.000000008765432"),
    D(1.000000000765432e0,  "1.000000000765432"),
    D(1.000000000065432e0,  "1.000000000065432"),
    D(1.000000000005432e0,  "1.000000000005432"),
    D(1.000000000000432e0,  "1.000000000000432"),
    D(1.000000000000032e0,  "1.000000000000032"),
    D(1.000000000000002e0,  "1.000000000000002"),
    D(1.204567898765432e1,  "12.04567898765432"),
    D(1.200567898765432e1,  "12.00567898765432"),
    D(1.200067898765432e1,  "12.00067898765432"),
    D(1.200007898765432e1,  "12.00007898765432"),
    D(1.200000898765432e1,  "12.00000898765432"),
    D(1.200000098765432e1,  "12.00000098765432"),
    D(1.200000008765432e1,  "12.00000008765432"),
    D(1.200000000765432e1,  "12.00000000765432"),
    D(1.200000000065432e1,  "12.00000000065432"),
    D(1.200000000005432e1,  "12.00000000005432"),
    D(1.200000000000432e1,  "12.00000000000432"),
    D(1.200000000000032e1,  "12.00000000000032"),
    D(1.200000000000002e1,  "12.00000000000002"),
    D(1.230567898765432e2,  "123.0567898765432"),
    D(1.230067898765432e2,  "123.0067898765432"),
    D(1.230007898765432e2,  "123.0007898765432"),
    D(1.230000898765432e2,  "123.0000898765432"),
    D(1.230000098765432e2,  "123.0000098765432"),
    D(1.230000008765432e2,  "123.0000008765432"),
    D(1.230000000765432e2,  "123.0000000765432"),
    D(1.230000000065432e2,  "123.0000000065432"),
    D(1.230000000005432e2,  "123.0000000005432"),
    D(1.230000000000432e2,  "123.0000000000432"),
    D(1.230000000000032e2,  "123.0000000000032"),
    D(1.230000000000002e2,  "123.0000000000002"),
    D(1.234067898765432e3,  "1234.067898765432"),
    D(1.234007898765432e3,  "1234.007898765432"),
    D(1.234000898765432e3,  "1234.000898765432"),
    D(1.234000098765432e3,  "1234.000098765432"),
    D(1.234000008765432e3,  "1234.000008765432"),
    D(1.234000000765432e3,  "1234.000000765432"),
    D(1.234000000065432e3,  "1234.000000065432"),
    D(1.234000000005432e3,  "1234.000000005432"),
    D(1.234000000000432e3,  "1234.000000000432"),
    D(1.234000000000032e3,  "1234.000000000032"),
    D(1.234000000000002e3,  "1234.000000000002"),
    D(1.234507898765432e4,  "12345.07898765432"),
    D(1.234500898765432e4,  "12345.00898765432"),
    D(1.234500098765432e4,  "12345.00098765432"),
    D(1.234500008765432e4,  "12345.00008765432"),
    D(1.234500000765432e4,  "12345.00000765432"),
    D(1.234500000065432e4,  "12345.00000065432"),
    D(1.234500000005432e4,  "12345.00000005432"),
    D(1.234500000000432e4,  "12345.00000000432"),
    D(1.234500000000032e4,  "12345.00000000032"),
    D(1.234500000000002e4,  "12345.00000000002"),
    D(1.234560898765432e5,  "123456.0898765432"),
    D(1.234560098765432e5,  "123456.0098765432"),
    D(1.234560008765432e5,  "123456.0008765432"),
    D(1.234560000765432e5,  "123456.0000765432"),
    D(1.234560000065432e5,  "123456.0000065432"),
    D(1.234560000005432e5,  "123456.0000005432"),
    D(1.234560000000432e5,  "123456.0000000432"),
    D(1.234560000000032e5,  "123456.0000000032"),
    D(1.234560000000002e5,  "123456.0000000002"),
    D(1.234567098765432e6,  "1234567.098765432"),
    D(1.234567008765432e6,  "1234567.008765432"),
    D(1.234567000765432e6,  "1234567.000765432"),
    D(1.234567000065432e6,  "1234567.000065432"),
    D(1.234567000005432e6,  "1234567.000005432"),
    D(1.234567000000432e6,  "1234567.000000432"),
    D(1.234567000000032e6,  "1234567.000000032"),
    D(1.234567000000002e6,  "1234567.000000002"),
    D(1.234567808765432e7,  "12345678.08765432"),
    D(1.234567800765432e7,  "12345678.00765432"),
    D(1.234567800065432e7,  "12345678.00065432"),
    D(1.234567800005432e7,  "12345678.00005432"),
    D(1.234567800000432e7,  "12345678.00000432"),
    D(1.234567800000032e7,  "12345678.00000032"),
    D(1.234567800000002e7,  "12345678.00000002"),
    D(1.234567890765432e8,  "123456789.0765432"),
    D(1.234567890065432e8,  "123456789.0065432"),
    D(1.234567890005432e8,  "123456789.0005432"),
    D(1.234567890000432e8,  "123456789.0000432"),
    D(1.234567890000032e8,  "123456789.0000032"),
    D(1.234567890000002e8,  "123456789.0000002"),
    D(1.234567898065432e9,  "1234567898.065432"),
    D(1.234567898005432e9,  "1234567898.005432"),
    D(1.234567898000432e9,  "1234567898.000432"),
    D(1.234567898000032e9,  "1234567898.000032"),
    D(1.234567898000002e9,  "1234567898.000002"),
    D(1.234567898705432e10, "12345678987.05432"),
    D(1.234567898700432e10, "12345678987.00432"),
    D(1.234567898700032e10, "12345678987.00032"),
    D(1.234567898700002e10, "12345678987.00002"),
    D(1.234567898760432e11, "123456789876.0432"),
    D(1.234567898760032e11, "123456789876.0032"),
    D(1.234567898760002e11, "123456789876.0002"),
    D(1.234567898765032e12, "1234567898765.032"),
    D(1.234567898765002e12, "1234567898765.002"),
    D(1.234567898765402e13, "12345678987654.02"),
#undef D
};

// ----------------------------------------------------------------------------
// 'double' "minimal" format precisely written integers test data
static const DblIntData k_DBL_CPP_INT[] = {

#define D(integer, exponent, expected) { L_, integer##ll, exponent, expected }

                        // Small Integers

    D(0x01, 0,  "1"),
    D(0x02, 0,  "2"),
    D(0x03, 0,  "3"),
    D(0x04, 0,  "4"),
    D(0x05, 0,  "5"),
    D(0x06, 0,  "6"),
    D(0x07, 0,  "7"),
    D(0x08, 0,  "8"),
    D(0x09, 0,  "9"),
    D(0x0a, 0, "10"),
    D(0x0b, 0, "11"),
    D(0x0c, 0, "12"),
    D(0x0d, 0, "13"),
    D(0x0e, 0, "14"),
    D(0x0f, 0, "15"),
    D(0x10, 0, "16"),
    D(0x11, 0, "17"),
    D(0x12, 0, "18"),
    D(0x13, 0, "19"),
    D(0x14, 0, "20"),
    D(0x15, 0, "21"),
    D(0x16, 0, "22"),
    D(0x17, 0, "23"),
    D(0x18, 0, "24"),
    D(0x19, 0, "25"),
    D(0x1a, 0, "26"),
    D(0x1b, 0, "27"),
    D(0x1c, 0, "28"),
    D(0x1d, 0, "29"),
    D(0x1e, 0, "30"),
    D(0x1f, 0, "31"),

    // 1 bit integers represented precisely (decimal notation chosen)
    D(1,                70, "1180591620717411303424"),
    D(1,                71, "2361183241434822606848"),
    // Smallest 1 bit integer that switches to scientific
    D(1,                72,  "4.722366482869645e+21"),

    // Largest precise 1 bit integer is written in scientific mode
    D(1,              1023,  "8.98846567431158e+307"),

    // Largest integer that is written precisely (decimal notation chosen)
    D(0x252086244844A5, 18, "2739492525104616701952"),
    // Next integer is written in scientific mode
    D(0x252086244844A6, 18,  "2.739492525104617e+21"),

                 // Less than 52 Significant Digits Integers

    D(0x1FFFFFFFFF, 0, "137438953471"),
    D(0x1000000001, 0, "68719476737"),

                     // 52 Significant Digits Integers

    D(0x8000000000001, 0, "2251799813685249"),
    D(0x8555555555555, 0, "2345624805922133"),
    D(0x8999999999999, 0, "2420684799711641"),
    D(0xFFFFFFFFFFFFF, 0, "4503599627370495"),

                // 53 Significant Binary Digits Integers

    D(0x10000000000001, 0, "4503599627370497"),
    D(0x15555555555555, 0, "6004799503160661"),
    D(0x19999999999999, 0, "7205759403792793"),
    D(0x1FFFFFFFFFFFFF, 0, "9007199254740991"),

       // More than 53 Significant Bin Digits Decimal Notation Integers

    D(0x10000000000001,   1,       "9007199254740994"),
    D(0x15555555555555,   1,      "12009599006321322"),
    D(0x19999999999999,   1,      "14411518807585586"),
    D(0x1FFFFFFFFFFFFF,   1,      "18014398509481982"),

    D(0x10000000000001,   2,      "18014398509481988"),
    D(0x15555555555555,   2,      "24019198012642644"),
    D(0x19999999999999,   2,      "28823037615171172"),
    D(0x1FFFFFFFFFFFFF,   2,      "36028797018963964"),

    D(0x10000000000001,   3,      "36028797018963976"),
    D(0x15555555555555,   3,      "48038396025285288"),
    D(0x19999999999999,   3,      "57646075230342344"),
    D(0x1FFFFFFFFFFFFF,   3,      "72057594037927928"),

    D(0x10000000000001,   7,     "576460752303423616"),
    D(0x15555555555555,   7,     "768614336404564608"),
    D(0x19999999999999,   7,     "922337203685477504"),
    D(0x1FFFFFFFFFFFFF,   7,    "1152921504606846848"),

    D(0x10000000000001,  11,    "9223372036854777856"),
    D(0x15555555555555,  11,   "12297829382473033728"),
    D(0x19999999999999,  11,   "14757395258967640064"),
    D(0x1FFFFFFFFFFFFF,  11,   "18446744073709549568"),

    D(0x10000000000001,  14,   "73786976294838222848"),
    D(0x15555555555555,  14,   "98382635059784269824"),
    D(0x19999999999999,  14,  "118059162071741120512"),
    D(0x1FFFFFFFFFFFFF,  14,  "147573952589676396544"),

    D(0x10000000000001,  17,  "590295810358705782784"),
    D(0x15555555555555,  17,  "787061080478274158592"),
    D(0x19999999999999,  17,        "9.44473296573929e+20"), // sci is shorter!
    D(0x1FFFFFFFFFFFFF,  17, "1180591620717411172352"),

     // More than 52 Significant Binary Digits Scientific Notation Integers

    D(0x10000000000001,  21, "9.444732965739293e+21" ),
    D(0x15555555555555,  21, "1.2592977287652387e+22"),
    D(0x19999999999999,  21, "1.5111572745182863e+22"),
    D(0x1FFFFFFFFFFFFF,  21, "1.8889465931478579e+22"),

    D(0x10000000000001,  99, "2.8544953854119204e+45"),
    D(0x15555555555555,  99, "3.805993847215893e+45" ),
    D(0x19999999999999,  99, "4.567192616659071e+45" ),
    D(0x1FFFFFFFFFFFFF,  99, "5.708990770823839e+45" ),

    D(0x10000000000001, 100, "5.708990770823841e+45"),
    D(0x15555555555555, 100, "7.611987694431786e+45" ),
    D(0x19999999999999, 100, "9.134385233318142e+45" ),
    D(0x1FFFFFFFFFFFFF, 100, "1.1417981541647678e+46" ),

    D(0x10000000000001, 509, "7.547924849643084e+168"),
    D(0x15555555555555, 509, "1.006389979952411e+169" ),
    D(0x19999999999999, 509, "1.2076679759428931e+169" ),
    D(0x1FFFFFFFFFFFFF, 509, "1.5095849699286164e+169" ),

    D(0x10000000000001, 971, "8.988465674311582e+307"),
    D(0x15555555555555, 971, "1.1984620899082105e+308" ),
    D(0x19999999999999, 971, "1.4381545078898526e+308" ),
    D(0x1FFFFFFFFFFFFF, 971, "1.7976931348623157e+308" ),
#undef D
};

// ----------------------------------------------------------------------------
// 'double' scientific format test data
static const DblData k_DBL_SCI[] = {

#define D(value, expected) { L_, value, expected }

            // Verifying C++ compatible scientific format exponent

    D(1.2e12,  "1.2e+12"), // Exponent '+' is sign displayed
    D(1.2e7,   "1.2e+07"), // Exponent leading 0 is added for 0-9
    D(1.2e100, "1.2e+100"), // 3 digit exponents work
    D(1.2e-7,  "1.2e-07"), // Negative exponents work

                // Verifying negative/positive numbers

    D(-1,      "-1e+00"),  // Minus sign written for integers
    D(-12,     "-1.2e+01"),
    D(-120,    "-1.2e+02"),

    D(-1.2,    "-1.2e+00"), // Minus sign written in decimal
    D(-0.12,   "-1.2e-01"), // format for negative numbers
    D(-0.012,  "-1.2e-02"),
    D(-1.2e7,  "-1.2e+07"),

    D(0,           "0e+00"),  // Zero (no significant digits)
    D(dblNegZero, "-0e+00"),  // Negative zero is written as negative

    D(+1,      "1e+00"), // Plus sign not written for integers
    D(+12,     "1.2e+01"),
    D(+120,    "1.2e+02"),

    D(+1.2,    "1.2e+00"), // Plus sign not written in decimal
    D(+0.12,   "1.2e-01"), // format
    D(+0.012,  "1.2e-02"),

    // Extremes
    D(DBL_MAX, "1.7976931348623157e+308"),
    D(DBL_MIN, "2.2250738585072014e-308"),
#ifdef DBL_TRUE_MIN
    D(DBL_TRUE_MIN, "5e-324"),
#else
    D(DBL_MIN / (1ull << 52), "5e-324"),
#endif

    // Single significant digit
    D(0.00002,  "2e-05"),
    D(0.0002,   "2e-04"),
    D(0.002,    "2e-03"),
    D(0.02,     "2e-02"),
    D(0.2,      "2e-01"),
    D(2,        "2e+00"),
    D(20.,      "2e+01"),
    D(200.,     "2e+02"),
    D(2000.,    "2e+03"),
    D(20000.,   "2e+04"),
    D(200000.,  "2e+05"),
    D(2000000., "2e+06"),

    D(1e+308,   "1e+308"),

    // Two significant digits
    D(1.5e-323,   "1.5e-323"),

    D(0.0000012,  "1.2e-06"),
    D(0.000012,   "1.2e-05"),
    D(0.00012,    "1.2e-04"),
    D(0.0012,     "1.2e-03"),
    D(0.012,      "1.2e-02"),
    D(0.12,       "1.2e-01"),
    D(1.2,        "1.2e+00"),
    D(12.,        "1.2e+01"),
    D(120.,       "1.2e+02"),
    D(1200.,      "1.2e+03"),
    D(12000.,     "1.2e+04"),
    D(120000.,    "1.2e+05"),
    D(1200000.,   "1.2e+06"),
    D(12000000.,  "1.2e+07"),
    D(120000000., "1.2e+08"),

    D(1.2e+308,   "1.2e+308"),

    // Three significant digits
    D(1.23e-322, "1.24e-322"),

    D(0.0000123, "1.23e-05"),
    D(0.000123,  "1.23e-04"),
    D(0.00123,   "1.23e-03"),
    D(0.0123,    "1.23e-02"),
    D(0.123,     "1.23e-01"),
    D(1.23,      "1.23e+00"),
    D(12.3,      "1.23e+01"),
    D(123.,      "1.23e+02"),
    D(1230.,     "1.23e+03"),
    D(12300.,    "1.23e+04"),
    D(123000.,   "1.23e+05"),
    D(1230000.,  "1.23e+06"),
    D(12300000., "1.23e+07"),

    D(1.23e+308, "1.23e+308"),

    // Four significant digits
    D(1.234e-321, "1.235e-321"),

    D(0.00001234, "1.234e-05"),
    D(0.0001234,  "1.234e-04"),
    D(0.001234,   "1.234e-03"),
    D(0.01234,    "1.234e-02"),
    D(0.1234,     "1.234e-01"),
    D(1.234,      "1.234e+00"),
    D(12.34,      "1.234e+01"),
    D(123.4,      "1.234e+02"),
    D(1234.,      "1.234e+03"),
    D(12340.,     "1.234e+04"),
    D(123400.,    "1.234e+05"),
    D(1234000.,   "1.234e+06"),

    D(1.234e+308, "1.234e+308"),

    // Nine significant digits
    D(1.23456783e-316,  "1.23456783e-316"),

    D(0.0000123456789,  "1.23456789e-05"),
    D(0.000123456789,   "1.23456789e-04"),
    D(0.00123456789,    "1.23456789e-03"),
    D(0.0123456789,     "1.23456789e-02"),
    D(0.123456789,      "1.23456789e-01"),
    D(1.23456789,       "1.23456789e+00"),
    D(12.3456789,       "1.23456789e+01"),
    D(123.456789,       "1.23456789e+02"),
    D(1234.56789,       "1.23456789e+03"),
    D(12345.6789,       "1.23456789e+04"),
    D(123456.789,       "1.23456789e+05"),
    D(1234567.89,       "1.23456789e+06"),
    D(12345678.9,       "1.23456789e+07"),
    D(123456789.,       "1.23456789e+08"),
    D(1234567890.,      "1.23456789e+09"),
    D(12345678900.,     "1.23456789e+10"),
    D(123456789000.,    "1.23456789e+11"),
    D(1234567890000.,   "1.23456789e+12"),
    D(12345678900000.,  "1.23456789e+13"),
    D(123456789000000., "1.23456789e+14"),

    D(1.23456789e+308,  "1.23456789e+308"),

    // Fifteen significant digits
    D(1.2345678987654e-311,  "1.2345678987655e-311"),

    D(0.000012345678987654,  "1.2345678987654e-05"),
    D(0.00012345678987654,   "1.2345678987654e-04"),
    D(0.0012345678987654,    "1.2345678987654e-03"),
    D(0.012345678987654,     "1.2345678987654e-02"),
    D(0.12345678987654,      "1.2345678987654e-01"),
    D(1.2345678987654,       "1.2345678987654e+00"),
    D(12.345678987654,       "1.2345678987654e+01"),
    D(123.45678987654,       "1.2345678987654e+02"),
    D(1234.5678987654,       "1.2345678987654e+03"),
    D(12345.678987654,       "1.2345678987654e+04"),
    D(123456.78987654,       "1.2345678987654e+05"),
    D(1234567.8987654,       "1.2345678987654e+06"),
    D(12345678.987654,       "1.2345678987654e+07"),
    D(123456789.87654,       "1.2345678987654e+08"),
    D(1234567898.7654,       "1.2345678987654e+09"),
    D(12345678987.654,       "1.2345678987654e+10"),
    D(123456789876.54,       "1.2345678987654e+11"),
    D(1234567898765.4,       "1.2345678987654e+12"),
    D(12345678987654.,       "1.2345678987654e+13"),
    D(123456789876540.,      "1.2345678987654e+14"),
    D(1234567898765400.,     "1.2345678987654e+15"),
    D(12345678987654000.,    "1.2345678987654e+16"),
    D(123456789876540000.,   "1.2345678987654e+17"),
    D(1234567898765400000.,  "1.2345678987654e+18"),
    D(12345678987654000000., "1.2345678987654e+19"),

    D(1.2345678987654e+308, "1.2345678987654e+308"),

    // Seventeen significant digits
    D(1.234567898765435e-309,  "1.234567898765436e-309"),

    D(0.00001234567898765432,  "1.234567898765432e-05"),
    D(0.0001234567898765432,   "1.234567898765432e-04"),
    D(0.001234567898765432,    "1.234567898765432e-03"),
    D(0.01234567898765432,     "1.234567898765432e-02"),
    D(0.1234567898765432,      "1.234567898765432e-01"),
    D(1.234567898765432,       "1.234567898765432e+00"),
    D(12.34567898765432,       "1.234567898765432e+01"),
    D(123.4567898765432,       "1.234567898765432e+02"),
    D(1234.567898765432,       "1.234567898765432e+03"),
    D(12345.67898765432,       "1.234567898765432e+04"),
    D(123456.7898765432,       "1.234567898765432e+05"),
    D(1234567.898765432,       "1.234567898765432e+06"),
    D(12345678.98765432,       "1.234567898765432e+07"),
    D(123456789.8765432,       "1.234567898765432e+08"),
    D(1234567898.765432,       "1.234567898765432e+09"),
    D(12345678987.65432,       "1.234567898765432e+10"),
    D(123456789876.5432,       "1.234567898765432e+11"),
    D(1234567898765.432,       "1.234567898765432e+12"),
    D(12345678987654.32,       "1.234567898765432e+13"),
    D(123456789876543.2,       "1.234567898765432e+14"),
    D(1234567898765432.,       "1.234567898765432e+15"),
    D(12345678987654320.,      "1.234567898765432e+16"),
    D(123456789876543200.,     "1.234567898765432e+17"),
    D(1234567898765432000.,    "1.234567898765432e+18"),
    D(12345678987654320000.,   "1.234567898765432e+19"),
    D(123456789876543200000.,  "1.234567898765432e+20"),
    D(1234567898765432000000., "1.234567898765432e+21"),

    D(1.234567898765432e+308, "1.234567898765432e+308"),
#undef D
};

// ----------------------------------------------------------------------------
// 'double' decimal format test data
static const DblData k_DBL_DEC[] = {

#define D(value, expected) { L_, value, expected }

                 // Verifying negative/positive numbers

    D(-1,      "-1"  ),  // Minus sign written for integers
    D(-12,     "-12" ),
    D(-120,    "-120"),

    D(-1.2,    "-1.2"  ), // Minus sign written in decimal
    D(-0.12,   "-0.12" ), // format for negative numbers
    D(-0.012,  "-0.012"),
    D(-1.2e7,  "-12000000"),

    D(0,           "0"), // Zero (no significant digits)
    D(dblNegZero, "-0"), // Negative zero is written as negative

    D(+1,      "1"  ), // Plus sign not written for integers
    D(+12,     "12" ),
    D(+120,    "120"),

    D(+1.2,    "1.2"  ), // Plus sign not for non-integers
    D(+0.12,   "0.12" ),
    D(+0.012,  "0.012"),

    D(+1.2e7,  "12000000"), // Plus is not written for large integers

    // Extremes
    D(DBL_MAX,
      "179769313486231570814527423731704356798070567525844996598917"  //  60
      "476803157260780028538760589558632766878171540458953514382464"  // 120
      "234321326889464182768467546703537516986049910576551282076245"  // 180
      "490090389328944075868508455133942304583236903222948165808559"  // 240
      "332123348274797826204144723168738177180919299881250404026184"  // 300
      "124858368"),                                                   // 309

    D(DBL_MIN,
      "0.000000000000000000000000000000000000000000000000000000000000" //  60+2
      "000000000000000000000000000000000000000000000000000000000000"   // 120+2
      "000000000000000000000000000000000000000000000000000000000000"   // 180+2
      "000000000000000000000000000000000000000000000000000000000000"   // 240+2
      "000000000000000000000000000000000000000000000000000000000000"   // 300+2
      "000000022250738585072014"),                                     // 324+2
#ifdef DBL_TRUE_MIN
    D(DBL_TRUE_MIN,
      "0.000000000000000000000000000000000000000000000000000000000000" //  60+2
      "000000000000000000000000000000000000000000000000000000000000"   // 120+2
      "000000000000000000000000000000000000000000000000000000000000"   // 180+2
      "000000000000000000000000000000000000000000000000000000000000"   // 240+2
      "000000000000000000000000000000000000000000000000000000000000"   // 300+2
      "000000000000000000000005"),                                     // 324+2
#else
    D(DBL_MIN / (1ull << 52),
      "0.000000000000000000000000000000000000000000000000000000000000" //  60+2
      "000000000000000000000000000000000000000000000000000000000000"   // 120+2
      "000000000000000000000000000000000000000000000000000000000000"   // 180+2
      "000000000000000000000000000000000000000000000000000000000000"   // 240+2
      "000000000000000000000000000000000000000000000000000000000000"   // 300+2
      "000000000000000000000005"),                                     // 324+2
#endif
                  // Verifying Different Number of Significant Digits

    // Single significant digit
    D(2e-55, "0.0000000000000000000000000000000000000000000000000000002"),// 57
    D(2e-15, "0.000000000000002"),
    D(2e-5,  "0.00002"          ),
    D(2e-4,  "0.0002"           ),
    D(2e-3,  "0.002"            ),
    D(2e-2,  "0.02"             ),
    D(2e-1,  "0.2"              ),
    D(2e0,   "2"                ),
    D(2e1,   "20"               ),
    D(2e2,   "200"              ),
    D(2e3,   "2000"             ),
    D(2e4,   "20000"            ),
    D(2e5,   "200000"           ),
    D(2e6,   "2000000"          ),
    D(2e7,   "20000000"         ),
    D(2e8,   "200000000"        ),
    D(2e9,   "2000000000"       ),
    D(2e10,  "20000000000"      ),
    D(2e11,  "200000000000"     ),
    D(2e12,  "2000000000000"    ),
    D(2e13,  "20000000000000"   ),
    D(2e14,  "200000000000000"  ),
    D(2e15,  "2000000000000000" ),
    D(2e16,  "20000000000000000"),

    D(1e+308,
      "100000000000000001097906362944045541740492309677311846336810"  //  60
      "682903157585404911491537163328978494688899061249669721172515"  // 120
      "611590283743140088328307009198146046031271664502933027185697"  // 180
      "489699588559043338384466165001178426897626212945177628091195"  // 240
      "786707458122783970171784415105291802893207873272974885715430"  // 300
      "223118336"),                                                   // 309

            // Two significant digits
    D(1.5e-323,
      "0.000000000000000000000000000000000000000000000000000000000000" //  60+2
      "000000000000000000000000000000000000000000000000000000000000"   // 120+2
      "000000000000000000000000000000000000000000000000000000000000"   // 180+2
      "000000000000000000000000000000000000000000000000000000000000"   // 240+2
      "000000000000000000000000000000000000000000000000000000000000"   // 300+2
      "000000000000000000000015"),                                     // 324+2

    D(1.2e-55, "0.00000000000000000000000000000000000000000000000000000012"),
    D(1.2e-15, "0.0000000000000012"),                               // ^^^ 58
    D(1.2e-5,  "0.000012"          ),
    D(1.2e-4,  "0.00012"           ),
    D(1.2e-3,  "0.0012"            ),
    D(1.2e-2,  "0.012"             ),
    D(1.2e-1,  "0.12"              ),
    D(1.2e0,   "1.2"               ),
    D(1.2e1,   "12"                ),
    D(1.2e2,   "120"               ),
    D(1.2e3,   "1200"              ),
    D(1.2e4,   "12000"             ),
    D(1.2e5,   "120000"            ),
    D(1.2e6,   "1200000"           ),
    D(1.2e7,   "12000000"          ),
    D(1.2e8,   "120000000"         ),
    D(1.2e9,   "1200000000"        ),
    D(1.2e10,  "12000000000"       ),
    D(1.2e11,  "120000000000"      ),
    D(1.2e12,  "1200000000000"     ),
    D(1.2e13,  "12000000000000"    ),
    D(1.2e14,  "120000000000000"   ),
    D(1.2e15,  "1200000000000000"  ),
    D(1.2e16,  "12000000000000000" ),
    D(1.2e17,  "120000000000000000"),

    D(1.2e+308,
      "119999999999999993334126397393975403463099919465419951334367"  //  60
      "777741958936475704820095748427298337363539041637633122602661"  // 120
      "364120963721787130104216457463400612280838804264193951342814"  // 180
      "295835163564781855175581946052683498850173279838239830467614"  // 240
      "460567394191919690413964668097866535968825281358135881937140"  // 300
      "228030464"),                                                   // 309

    // Three significant digits
    D(1.23e-322,
      "0.000000000000000000000000000000000000000000000000000000000000" //  60+2
      "000000000000000000000000000000000000000000000000000000000000"   // 120+2
      "000000000000000000000000000000000000000000000000000000000000"   // 180+2
      "000000000000000000000000000000000000000000000000000000000000"   // 240+2
      "000000000000000000000000000000000000000000000000000000000000"   // 300+2
      "000000000000000000000124"),                                     // 324+2

    D(1.23e-55, "0.000000000000000000000000000000000000000000000000000000123"),
    D(1.23e-12, "0.00000000000123"),                                 // ^^^ 59
    D(1.23e-5,  "0.0000123"       ),
    D(1.23e-4,  "0.000123"        ),
    D(1.23e-3,  "0.00123"         ),
    D(1.23e-2,  "0.0123"          ),
    D(1.23e-1,  "0.123"           ),
    D(1.23e0,   "1.23"            ),
    D(1.23e1,   "12.3"            ),
    D(1.23e2,   "123"             ),
    D(1.23e3,   "1230"            ),
    D(1.23e4,   "12300"           ),
    D(1.23e5,   "123000"          ),
    D(1.23e6,   "1230000"         ),
    D(1.23e7,   "12300000"        ),
    D(1.23e8,   "123000000"       ),
    D(1.23e9,   "1230000000"      ),
    D(1.23e10,  "12300000000"     ),
    D(1.23e11,  "123000000000"    ),
    D(1.23e12,  "1230000000000"   ),
    D(1.23e13,  "12300000000000"  ),
    D(1.23e14,  "123000000000000" ),
    D(1.23e15,  "1230000000000000"),

    D(1.23e+308,
      "122999999999999994165399712096184694377863773970474733151452"  //  60
      "602403236680638871061816748083915277830519996661320268518272"  // 120
      "569447409911079430343040863096782457957445673513214510286387"  // 180
      "489706585492160170415693676197591912999799883796192491634532"  // 240
      "382516773491145316898335863553873652805923934213268526600740"  // 300
      "738695168"),                                                   // 309

    // Four significant digits
    D(1.234e-321,
      "0.000000000000000000000000000000000000000000000000000000000000" //  60+2
      "000000000000000000000000000000000000000000000000000000000000"   // 120+2
      "000000000000000000000000000000000000000000000000000000000000"   // 180+2
      "000000000000000000000000000000000000000000000000000000000000"   // 240+2
      "000000000000000000000000000000000000000000000000000000000000"   // 300+2
      "000000000000000000001235"),                                     // 324+2

    D(1.234e-55,
      "0.0000000000000000000000000000000000000000000000000000001234"), // 60

    D(1.234e-11, "0.00000000001234"),
    D(1.234e-5,  "0.00001234"      ),
    D(1.234e-4,  "0.0001234"       ),
    D(1.234e-3,  "0.001234"        ),
    D(1.234e-2,  "0.01234"         ),
    D(1.234e-1,  "0.1234"          ),
    D(1.234e0,   "1.234"           ),
    D(1.234e1,   "12.34"           ),
    D(1.234e2,   "123.4"           ),
    D(1.234e3,   "1234"            ),
    D(1.234e4,   "12340"           ),
    D(1.234e5,   "123400"          ),
    D(1.234e6,   "1234000"         ),
    D(1.234e7,   "12340000"        ),
    D(1.234e8,   "123400000"       ),
    D(1.234e9,   "1234000000"      ),
    D(1.234e10,  "12340000000"     ),
    D(1.234e11,  "123400000000"    ),
    D(1.234e12,  "1234000000000"   ),
    D(1.234e13,  "12340000000000"  ),
    D(1.234e14,  "123400000000000" ),
    D(1.234e15,  "1234000000000000"),

    D(1.234e+308,
      "123399999999999999598476979482398764250159522669384880240267"  //  60
      "273519293823867419207212113083114774068210678572458916509925"  // 120
      "643349187249639054301385419564149799352118051505967705665545"  // 180
      "710092336886524046371560208182733443837735548121235061951335"  // 240
      "094431061101322782957703109633663686719553198973575533170138"  // 300
      "166591488"),                                                   // 309

    // Five significant digits
    D(1.2345e-320,
      "0.000000000000000000000000000000000000000000000000000000000000" //  60+2
      "000000000000000000000000000000000000000000000000000000000000"   // 120+2
      "000000000000000000000000000000000000000000000000000000000000"   // 180+2
      "000000000000000000000000000000000000000000000000000000000000"   // 240+2
      "000000000000000000000000000000000000000000000000000000000000"   // 300+2
      "000000000000000000012347"),                                     // 324+2

    D(1.2345e-55,
      "0.00000000000000000000000000000000000000000000000000000012345"), // 61

    D(1.2345e-11, "0.000000000012345"),
    D(1.2345e-5,  "0.000012345"      ),
    D(1.2345e-4,  "0.00012345"       ),
    D(1.2345e-3,  "0.0012345"        ),
    D(1.2345e-2,  "0.012345"         ),
    D(1.2345e-1,  "0.12345"          ),
    D(1.2345e0,   "1.2345"           ),
    D(1.2345e1,   "12.345"           ),
    D(1.2345e2,   "123.45"           ),
    D(1.2345e3,   "1234.5"           ),
    D(1.2345e4,   "12345"            ),
    D(1.2345e5,   "123450"           ),
    D(1.2345e6,   "1234500"          ),
    D(1.2345e7,   "12345000"         ),
    D(1.2345e8,   "123450000"        ),
    D(1.2345e9,   "1234500000"       ),
    D(1.2345e10,  "12345000000"      ),
    D(1.2345e11,  "123450000000"     ),
    D(1.2345e12,  "1234500000000"    ),
    D(1.2345e13,  "12345000000000"   ),
    D(1.2345e14,  "123450000000000"  ),
    D(1.2345e15,  "1234500000000000" ),
    D(1.2345e16,  "12345000000000000"),

    D(1.2345e+308,
      "123449999999999995288010864068875993843264708664652233457740"  //  60
      "956320157113014619619293503978342300933459618897619658256158"  // 120
      "921469798935720897365083518138586565178522603042983304287926"  // 180
      "305262841619525686562432617212919501800616146351882056214797"  // 240
      "631244374830456795095013621625835173769366752962717670915452"  // 300
      "820258816"),                                                   // 309

    // Nine significant digits
    D(1.23456785e-316,
      "0.000000000000000000000000000000000000000000000000000000000000" //  60+2
      "000000000000000000000000000000000000000000000000000000000000"   // 120+2
      "000000000000000000000000000000000000000000000000000000000000"   // 180+2
      "000000000000000000000000000000000000000000000000000000000000"   // 240+2
      "000000000000000000000000000000000000000000000000000000000000"   // 300+2
      "000000000000000123456783"),                                     // 324+2

    D(1.23456789e-55,
      "0.000000000000000000000000000000000000000000000000000000123456789"),//65

    D(1.23456789e-11, "0.0000000000123456789"),
    D(1.23456789e-5,  "0.0000123456789"      ),
    D(1.23456789e-4,  "0.000123456789"       ),
    D(1.23456789e-3,  "0.00123456789"        ),
    D(1.23456789e-2,  "0.0123456789"         ),
    D(1.23456789e-1,  "0.123456789"          ),
    D(1.23456789e0,   "1.23456789"           ),
    D(1.23456789e1,   "12.3456789"           ),
    D(1.23456789e2,   "123.456789"           ),
    D(1.23456789e3,   "1234.56789"           ),
    D(1.23456789e4,   "12345.6789"           ),
    D(1.23456789e5,   "123456.789"           ),
    D(1.23456789e6,   "1234567.89"           ),
    D(1.23456789e7,   "12345678.9"           ),
    D(1.23456789e8,   "123456789"            ),
    D(1.23456789e9,   "1234567890"           ),
    D(1.23456789e10,  "12345678900"          ),
    D(1.23456789e11,  "123456789000"         ),
    D(1.23456789e12,  "1234567890000"        ),
    D(1.23456789e13,  "12345678900000"       ),
    D(1.23456789e14,  "123456789000000"      ),
    D(1.23456789e15,  "1234567890000000"     ),
    D(1.23456789e16,  "12345678900000000"    ),
    D(1.23456789e17,  "123456789000000000"   ),
    D(1.23456789e18,  "1234567890000000000"  ),
    D(1.23456789e19,  "12345678900000000000" ),
    D(1.23456789e20,  "123456788999999995904"),

    D(1.23456789e+308,
      "123456789000000004810070270463755942267619224180247500956396"  //  60
      "206068102412092226236396443696899102353171304143817789487919"  // 120
      "123992323950091504541492906785860605957745633924722561053445"  // 180
      "871433980835192763796487815950728557880041793319526330986197"  // 240
      "397604709979539461737602865391693914881194183408595281924019"  // 300
      "779010560"),                                                   // 309

    // Sixteen significant digits
    D(1.234567898765435e-309,
      "0.000000000000000000000000000000000000000000000000000000000000" //  60+2
      "000000000000000000000000000000000000000000000000000000000000"   // 120+2
      "000000000000000000000000000000000000000000000000000000000000"   // 180+2
      "000000000000000000000000000000000000000000000000000000000000"   // 240+2
      "000000000000000000000000000000000000000000000000000000000000"   // 300+2
      "000000001234567898765436"),                                     // 324+2

    D(1.234567898765435e-55,
      "0.00000000000000000000000000000000000000000000000000" //  50 + 2
      "00001234567898765435"),                               //  70 + 2

    D(1.234567898765435e-11, "0.00000000001234567898765435"),
    D(1.234567898765435e-5,  "0.00001234567898765435"      ),
    D(1.234567898765435e-4,  "0.0001234567898765435"       ),
    D(1.234567898765435e-3,  "0.001234567898765435"        ),
    D(1.234567898765435e-2,  "0.01234567898765435"         ),
    D(1.234567898765435e-1,  "0.1234567898765435"          ),
    D(1.234567898765435e0,   "1.234567898765435"           ),
    D(1.234567898765435e1,   "12.34567898765435"           ),
    D(1.234567898765435e2,   "123.4567898765435"           ),
    D(1.234567898765435e3,   "1234.567898765435"           ),
    D(1.234567898765435e4,   "12345.67898765435"           ),
    D(1.234567898765435e5,   "123456.7898765435"           ),
    D(1.234567898765435e6,   "1234567.898765435"           ),
    D(1.234567898765435e7,   "12345678.98765435"           ),
    D(1.234567898765435e8,   "123456789.8765435"           ),
    D(1.234567898765435e9,   "1234567898.765435"           ),
    D(1.234567898765435e10,  "12345678987.65435"           ),
    D(1.234567898765435e11,  "123456789876.5435"           ),
    D(1.234567898765435e12,  "1234567898765.435"           ),
    D(1.234567898765435e13,  "12345678987654.35"           ),
    D(1.234567898765435e14,  "123456789876543.5"           ),
    D(1.234567898765435e15,  "1234567898765435"            ),
    D(1.234567898765435e16,  "12345678987654350"           ),
    D(1.234567898765435e17,  "123456789876543504"          ),
    D(1.234567898765435e18,  "1234567898765434880"         ),
    D(1.234567898765435e19,  "12345678987654350848"        ),
    D(1.234567898765435e20,  "123456789876543504384"       ),
    D(1.234567898765435e21,  "1234567898765435076608"      ),
    D(1.234567898765435e22,  "12345678987654349193216"     ),
    D(1.234567898765435e23,  "123456789876543504515072"    ),
    D(1.234567898765435e24,  "1234567898765434877378560"   ),
    D(1.234567898765435e25,  "12345678987654350384398336"  ),
    D(1.234567898765435e26,  "123456789876543503843983360" ),
    D(1.234567898765435e27,  "1234567898765435038439833600"),

    D(1.234567898765435e+308,
      "123456789876543490404331907195626090389791516449522974911969"  //  60
      "504815256093220516779825055216544400958538621142875138546518"  // 120
      "031105302323270597512971249524452705238393012527619910129489"  // 180
      "565470757412542096214998967258088014617318710630315609127580"  // 240
      "898426611046842055700316112159686069665980894760863057386119"  // 300
      "651917824"),                                                   // 309

    // Seventeen significant digits
    D(1.2345678987654325e-308,
      "0.000000000000000000000000000000000000000000000000000000000000" //  60+2
      "000000000000000000000000000000000000000000000000000000000000"   // 120+2
      "000000000000000000000000000000000000000000000000000000000000"   // 180+2
      "000000000000000000000000000000000000000000000000000000000000"   // 240+2
      "000000000000000000000000000000000000000000000000000000000000"   // 300+2
      "000000012345678987654326"),                                     // 324+2

    D(1.2345678987654325e-55,
      "0.00000000000000000000000000000000000000000000000000" //  50 + 2
      "000012345678987654325"),                              //  71 + 2

    D(1.2345678987654325e-11, "0.000000000012345678987654324"),
    D(1.2345678987654325e-5,  "0.000012345678987654326"      ),
    D(1.2345678987654325e-4,  "0.00012345678987654325"       ),
    D(1.2345678987654325e-3,  "0.0012345678987654325"        ),
    D(1.2345678987654325e-2,  "0.012345678987654325"         ),
    D(1.2345678987654325e-1,  "0.12345678987654325"          ),
    D(1.2345678987654325e0,   "1.2345678987654325"           ),
    D(1.2345678987654325e1,   "12.345678987654326"           ),
    D(1.2345678987654325e2,   "123.45678987654325"           ),
    D(1.2345678987654325e3,   "1234.5678987654326"           ),
    D(1.2345678987654325e4,   "12345.678987654324"           ),
    D(1.2345678987654325e5,   "123456.78987654325"           ),
    D(1.2345678987654325e6,   "1234567.8987654324"           ),
    D(1.2345678987654325e7,   "12345678.987654325"           ),
    D(1.2345678987654325e8,   "123456789.87654325"           ),
    D(1.2345678987654325e9,   "1234567898.7654326"           ),
    D(1.2345678987654325e10,  "12345678987.654325"           ),
    D(1.2345678987654325e11,  "123456789876.54324"           ),
    D(1.2345678987654325e12,  "1234567898765.4326"           ),
    D(1.2345678987654325e13,  "12345678987654.324"           ),
    D(1.2345678987654325e14,  "123456789876543.25"           ),
    D(1.2345678987654325e15,  "1234567898765432.5"           ),
    D(1.2345678987654325e16,  "12345678987654324"            ),
    D(1.2345678987654325e17,  "123456789876543248"           ),
    D(1.2345678987654325e18,  "1234567898765432576"          ),
    D(1.2345678987654325e19,  "12345678987654324224"         ),
    D(1.2345678987654325e20,  "123456789876543242240"        ),
    D(1.2345678987654325e21,  "1234567898765432455168"       ),
    D(1.2345678987654325e22,  "12345678987654324027392"      ),
    D(1.2345678987654325e23,  "123456789876543252856832"     ),
    D(1.2345678987654325e24,  "1234567898765432461459456"    ),
    D(1.2345678987654325e25,  "12345678987654324614594560"   ),
    D(1.2345678987654325e26,  "123456789876543246145945600"  ),
    D(1.2345678987654325e27,  "1234567898765432564538671104" ),
    D(1.2345678987654325e28,  "12345678987654323996119269376"),

    D(1.2345678987654325e+308,
      "123456789876543250903494763029248691625065952028895046817818"  //  60
      "252560351112914847687359628192268713064343665283758854415796"  // 120
      "937483999223841320820412642293213416537777218347849471728808"  // 180
      "811340476230437569641675408796169611807973439751115911872966"  // 240
      "393979944384209841935017211305177244575255897677843629744838"  // 300
      "460571648"),                                                   // 309

                    // Leading decimal zero(s) regression

    // Three significant digits
    D(1.03e0,  "1.03"),

    // Four significant digits
    D(1.034e0,  "1.034"),
    D(1.004e0,  "1.004"),
    D(1.204e1,  "12.04"),

    // Five significant digits
    D(1.0345e0,  "1.0345"),
    D(1.0045e0,  "1.0045"),
    D(1.0005e0,  "1.0005"),
    D(1.2045e1,  "12.045"),
    D(1.2005e1,  "12.005"),
    D(1.2305e2,  "123.05"),

    // Six significant digits
    D(1.03456e0,  "1.03456"),
    D(1.00456e0,  "1.00456"),
    D(1.00056e0,  "1.00056"),
    D(1.00006e0,  "1.00006"),
    D(1.20456e1,  "12.0456"),
    D(1.20056e1,  "12.0056"),
    D(1.20006e1,  "12.0006"),
    D(1.23056e2,  "123.056"),
    D(1.23006e2,  "123.006"),
    D(1.23406e3,  "1234.06"),

    // Sixteen significant digits
    D(1.034567898765432e0,  "1.034567898765432"),
    D(1.004567898765432e0,  "1.004567898765432"),
    D(1.000567898765432e0,  "1.000567898765432"),
    D(1.000067898765432e0,  "1.000067898765432"),
    D(1.000007898765432e0,  "1.000007898765432"),
    D(1.000000898765432e0,  "1.000000898765432"),
    D(1.000000098765432e0,  "1.000000098765432"),
    D(1.000000008765432e0,  "1.000000008765432"),
    D(1.000000000765432e0,  "1.000000000765432"),
    D(1.000000000065432e0,  "1.000000000065432"),
    D(1.000000000005432e0,  "1.000000000005432"),
    D(1.000000000000432e0,  "1.000000000000432"),
    D(1.000000000000032e0,  "1.000000000000032"),
    D(1.000000000000002e0,  "1.000000000000002"),
    D(1.204567898765432e1,  "12.04567898765432"),
    D(1.200567898765432e1,  "12.00567898765432"),
    D(1.200067898765432e1,  "12.00067898765432"),
    D(1.200007898765432e1,  "12.00007898765432"),
    D(1.200000898765432e1,  "12.00000898765432"),
    D(1.200000098765432e1,  "12.00000098765432"),
    D(1.200000008765432e1,  "12.00000008765432"),
    D(1.200000000765432e1,  "12.00000000765432"),
    D(1.200000000065432e1,  "12.00000000065432"),
    D(1.200000000005432e1,  "12.00000000005432"),
    D(1.200000000000432e1,  "12.00000000000432"),
    D(1.200000000000032e1,  "12.00000000000032"),
    D(1.200000000000002e1,  "12.00000000000002"),
    D(1.230567898765432e2,  "123.0567898765432"),
    D(1.230067898765432e2,  "123.0067898765432"),
    D(1.230007898765432e2,  "123.0007898765432"),
    D(1.230000898765432e2,  "123.0000898765432"),
    D(1.230000098765432e2,  "123.0000098765432"),
    D(1.230000008765432e2,  "123.0000008765432"),
    D(1.230000000765432e2,  "123.0000000765432"),
    D(1.230000000065432e2,  "123.0000000065432"),
    D(1.230000000005432e2,  "123.0000000005432"),
    D(1.230000000000432e2,  "123.0000000000432"),
    D(1.230000000000032e2,  "123.0000000000032"),
    D(1.230000000000002e2,  "123.0000000000002"),
    D(1.234067898765432e3,  "1234.067898765432"),
    D(1.234007898765432e3,  "1234.007898765432"),
    D(1.234000898765432e3,  "1234.000898765432"),
    D(1.234000098765432e3,  "1234.000098765432"),
    D(1.234000008765432e3,  "1234.000008765432"),
    D(1.234000000765432e3,  "1234.000000765432"),
    D(1.234000000065432e3,  "1234.000000065432"),
    D(1.234000000005432e3,  "1234.000000005432"),
    D(1.234000000000432e3,  "1234.000000000432"),
    D(1.234000000000032e3,  "1234.000000000032"),
    D(1.234000000000002e3,  "1234.000000000002"),
    D(1.234507898765432e4,  "12345.07898765432"),
    D(1.234500898765432e4,  "12345.00898765432"),
    D(1.234500098765432e4,  "12345.00098765432"),
    D(1.234500008765432e4,  "12345.00008765432"),
    D(1.234500000765432e4,  "12345.00000765432"),
    D(1.234500000065432e4,  "12345.00000065432"),
    D(1.234500000005432e4,  "12345.00000005432"),
    D(1.234500000000432e4,  "12345.00000000432"),
    D(1.234500000000032e4,  "12345.00000000032"),
    D(1.234500000000002e4,  "12345.00000000002"),
    D(1.234560898765432e5,  "123456.0898765432"),
    D(1.234560098765432e5,  "123456.0098765432"),
    D(1.234560008765432e5,  "123456.0008765432"),
    D(1.234560000765432e5,  "123456.0000765432"),
    D(1.234560000065432e5,  "123456.0000065432"),
    D(1.234560000005432e5,  "123456.0000005432"),
    D(1.234560000000432e5,  "123456.0000000432"),
    D(1.234560000000032e5,  "123456.0000000032"),
    D(1.234560000000002e5,  "123456.0000000002"),
    D(1.234567098765432e6,  "1234567.098765432"),
    D(1.234567008765432e6,  "1234567.008765432"),
    D(1.234567000765432e6,  "1234567.000765432"),
    D(1.234567000065432e6,  "1234567.000065432"),
    D(1.234567000005432e6,  "1234567.000005432"),
    D(1.234567000000432e6,  "1234567.000000432"),
    D(1.234567000000032e6,  "1234567.000000032"),
    D(1.234567000000002e6,  "1234567.000000002"),
    D(1.234567808765432e7,  "12345678.08765432"),
    D(1.234567800765432e7,  "12345678.00765432"),
    D(1.234567800065432e7,  "12345678.00065432"),
    D(1.234567800005432e7,  "12345678.00005432"),
    D(1.234567800000432e7,  "12345678.00000432"),
    D(1.234567800000032e7,  "12345678.00000032"),
    D(1.234567800000002e7,  "12345678.00000002"),
    D(1.234567890765432e8,  "123456789.0765432"),
    D(1.234567890065432e8,  "123456789.0065432"),
    D(1.234567890005432e8,  "123456789.0005432"),
    D(1.234567890000432e8,  "123456789.0000432"),
    D(1.234567890000032e8,  "123456789.0000032"),
    D(1.234567890000002e8,  "123456789.0000002"),
    D(1.234567898065432e9,  "1234567898.065432"),
    D(1.234567898005432e9,  "1234567898.005432"),
    D(1.234567898000432e9,  "1234567898.000432"),
    D(1.234567898000032e9,  "1234567898.000032"),
    D(1.234567898000002e9,  "1234567898.000002"),
    D(1.234567898705432e10, "12345678987.05432"),
    D(1.234567898700432e10, "12345678987.00432"),
    D(1.234567898700032e10, "12345678987.00032"),
    D(1.234567898700002e10, "12345678987.00002"),
    D(1.234567898760432e11, "123456789876.0432"),
    D(1.234567898760032e11, "123456789876.0032"),
    D(1.234567898760002e11, "123456789876.0002"),
    D(1.234567898765032e12, "1234567898765.032"),
    D(1.234567898765002e12, "1234567898765.002"),
    D(1.234567898765402e13, "12345678987654.02"),
#undef D
};

// ----------------------------------------------------------------------------
// 'double' decimal format precisely written integers test data
static const DblIntData k_DBL_DEC_INT[] = {

#define D(integer, exponent, expected) { L_, integer##ll, exponent, expected }

                        // Small Integers
    D(0x01, 0,  "1"),
    D(0x02, 0,  "2"),
    D(0x03, 0,  "3"),
    D(0x04, 0,  "4"),
    D(0x05, 0,  "5"),
    D(0x06, 0,  "6"),
    D(0x07, 0,  "7"),
    D(0x08, 0,  "8"),
    D(0x09, 0,  "9"),
    D(0x0a, 0, "10"),
    D(0x0b, 0, "11"),
    D(0x0c, 0, "12"),
    D(0x0d, 0, "13"),
    D(0x0e, 0, "14"),
    D(0x0f, 0, "15"),
    D(0x10, 0, "16"),
    D(0x11, 0, "17"),
    D(0x12, 0, "18"),
    D(0x13, 0, "19"),
    D(0x14, 0, "20"),
    D(0x15, 0, "21"),
    D(0x16, 0, "22"),
    D(0x17, 0, "23"),
    D(0x18, 0, "24"),
    D(0x19, 0, "25"),
    D(0x1a, 0, "26"),
    D(0x1b, 0, "27"),
    D(0x1c, 0, "28"),
    D(0x1d, 0, "29"),
    D(0x1e, 0, "30"),
    D(0x1f, 0, "31"),

                  // Less than 52 Significant Digits Integers

    D(0x1FFFFFFFFF, 0, "137438953471"),
    D(0x1000000001, 0, "68719476737"),

                   // 52 Significant Digits Integers

    D(0x8000000000001, 0, "2251799813685249"),
    D(0x8555555555555, 0, "2345624805922133"),
    D(0x8999999999999, 0, "2420684799711641"),
    D(0xFFFFFFFFFFFFF, 0, "4503599627370495"),


               // 53 Significant Binary Digits Integers

    D(0x10000000000001, 0, "4503599627370497"),
    D(0x15555555555555, 0, "6004799503160661"),
    D(0x19999999999999, 0, "7205759403792793"),
    D(0x1FFFFFFFFFFFFF, 0, "9007199254740991"),

           // More than 53 Significant Binary Digits Integers

    D(0x10000000000001,   1,        "9007199254740994"),
    D(0x15555555555555,   1,       "12009599006321322"),
    D(0x19999999999999,   1,       "14411518807585586"),
    D(0x1FFFFFFFFFFFFF,   1,       "18014398509481982"),

    D(0x10000000000001,   2,       "18014398509481988"),
    D(0x15555555555555,   2,       "24019198012642644"),
    D(0x19999999999999,   2,       "28823037615171172"),
    D(0x1FFFFFFFFFFFFF,   2,       "36028797018963964"),

    D(0x10000000000001,   3,       "36028797018963976"),
    D(0x15555555555555,   3,       "48038396025285288"),
    D(0x19999999999999,   3,       "57646075230342344"),
    D(0x1FFFFFFFFFFFFF,   3,       "72057594037927928"),

    D(0x10000000000001,   7,      "576460752303423616"),
    D(0x15555555555555,   7,      "768614336404564608"),
    D(0x19999999999999,   7,      "922337203685477504"),
    D(0x1FFFFFFFFFFFFF,   7,     "1152921504606846848"),

    D(0x10000000000001,  11,     "9223372036854777856"),
    D(0x15555555555555,  11,    "12297829382473033728"),
    D(0x19999999999999,  11,    "14757395258967640064"),
    D(0x1FFFFFFFFFFFFF,  11,    "18446744073709549568"),

    D(0x10000000000001,  14,    "73786976294838222848"),
    D(0x15555555555555,  14,    "98382635059784269824"),
    D(0x19999999999999,  14,    "118059162071741120512"),
    D(0x1FFFFFFFFFFFFF,  14,   "147573952589676396544"),

    D(0x10000000000001,  17,   "590295810358705782784"),
    D(0x15555555555555,  17,   "787061080478274158592"),
    D(0x19999999999999,  17,   "944473296573928964096"),
    D(0x1FFFFFFFFFFFFF,  17,  "1180591620717411172352"),

    D(0x10000000000001,  21,  "9444732965739292524544"),
    D(0x15555555555555,  21, "12592977287652386537472"),
    D(0x19999999999999,  21, "15111572745182863425536"),
    D(0x1FFFFFFFFFFFFF,  21, "18889465931478578757632"),

    D(0x10000000000001,  99, "2854495385411920395941872053013691021117095936"),
    D(0x15555555555555,  99, "3805993847215892804880329213827086780903456768"),
    D(0x19999999999999,  99, "4567192616659071239091335033769563987413827584"),
    D(0x1FFFFFFFFFFFFF,  99, "5708990770823838890407843763683279797179383808"),

    D(0x10000000000001, 100, "5708990770823840791883744106027382042234191872"),
    D(0x15555555555555, 100, "7611987694431785609760658427654173561806913536"),
    D(0x19999999999999, 100, "9134385233318142478182670067539127974827655168"),
    D(0x1FFFFFFFFFFFFF, 100,
      "11417981541647677780815687527366559594358767616"),

    D(0x10000000000001, 509,
        "754792484964308438045910040480117522858696721656364679167314" //  60
        "017454042054022947816939275720954868963531722402092445615105" // 120
        "3523442570605941984358219996186635391504916414464"),          // 169

    D(0x15555555555555, 509,
        "100638997995241097139854818016938378935274149958669191640296" //  60
        "409426386070278973367838228366807426326800324865764610607659" // 120
        "81394134576213994780418707506046775014996345618432"),         // 170

    D(0x19999999999999, 509,
        "120766797594289313215873799134676779828822730398941498098514" //  60
        "236163564939903877680475433032650524847959921264375806872269" // 120
        "26876506829305522392655378708591881357154389590016"),         // 170

    D(0x1FFFFFFFFFFFFF, 509,
        "150958496992861637329902270811284381169145601059349957785840" //  60
        "976269333244341234149431240031415172629699315862292601269183" // 120
        "45100065208942813811010385512409540870391455547392"),         // 170


    D(0x10000000000001, 971,
        "898846567431158153448683548866493532446259793155009893112276" //  60
        "449334417529282229059384731573508444258601397118662927075722" // 120
        "538633263321607509708035992556736695806019295607469458382078" // 180
        "393114798122351037559196723747109526427866104713740450611064" // 240
        "791175074702279398227347241911826917268002745760026304682522" // 300
        "11347456"),                                                   // 308

    D(0x15555555555555, 971,
        "119846208990821053862485980936868943386622755140025217957449" //  60
        "185986963312195176500631099345318391471397553524277795258607" // 120
        "297703698567960268420438325781003880454605934667138922450849" // 180
        "236563878474354509650486846713237047577973081895276546573889" // 240
        "957650195146049445629577007136228474373133005395361920118602" // 300
        "782998528"),                                                  // 309

    D(0x19999999999999, 971,
        "143815450788985260643302558054803108751201880094353129414036" //  60
        "502313440891629117315882895430644141634107148298148082908150" // 120
        "072350749896561834159650014150017335067183525030903866301007" // 180
        "737974482816190336137695490081519150380078610426345194267757" // 240
        "707439456397548797859404093549232355496247523189717313681635" // 300
        "319742464"),                                                  // 309

    D(0x1FFFFFFFFFFFFF, 971,
        "179769313486231570814527423731704356798070567525844996598917" //  60
        "476803157260780028538760589558632766878171540458953514382464" // 120
        "234321326889464182768467546703537516986049910576551282076245" // 180
        "490090389328944075868508455133942304583236903222948165808559" // 240
        "332123348274797826204144723168738177180919299881250404026184" // 300
        "124858368"),                                                  // 309
#undef D
};

// ----------------------------------------------------------------------------
//                        'float' shared test data
// ----------------------------------------------------------------------------

static const float  fltNegZero = -1 / std::numeric_limits<float>::infinity();

// ----------------------------------------------------------------------------
//                  'float' test data builder functions

static
float makeSubnormalFloat(int32_t mantissa)
    // Return a 'float' that is a (positive) subnormal and has the specified
    // 'mantissa'.  The behavior is undefined unless '0 < mantissa < 2^23'.
{
    float f;
    memcpy(&f, &mantissa, sizeof f);
    BSLMF_ASSERT(sizeof mantissa == sizeof f);
    return f;
}

                            // ==============
                            // struct FltData
                            // ==============

struct FltData {
    // General test data line for 'double' values.

    int         d_line;
    float       d_value;
    const char* d_expected;
};

                          // =================
                          // struct FltIntData
                          // =================

struct FltIntData {
    // Special 'float' test data line for precisely printed integer values.

    int         d_line;
    int         d_integer;
    unsigned    d_exponent;  // Use when value is too large for 23 bits
    const char* d_expected;
};

// ----------------------------------------------------------------------------
// Verify 'float' assumptions.

BSLMF_ASSERT(std::numeric_limits<float>::has_infinity);
BSLMF_ASSERT(std::numeric_limits<float>::has_quiet_NaN);
BSLMF_ASSERT(std::numeric_limits<float>::has_signaling_NaN);
BSLMF_ASSERT(std::numeric_limits<float>::has_denorm);

// ----------------------------------------------------------------------------
// 'float' "minimal" format test data
static const FltData k_FLT_CPP[] = {

#define F(value, expected) { L_, value, expected }

            // Verifying C++ compatible scientific format exponent

    F(1.2e12f,  "1.2e+12" ), // Exponent '+' is sign displayed
    F(1.2e7f,   "1.2e+07" ), // Exponent leading 0 is added for 0-9
    F(1.2e38f,  "1.2e+38" ), // Maximum exponent works
    F(1.2e-38f, "1.2e-38" ), // Minimum exponent works
    F(1.2e-7f,  "1.2e-07" ), // Negative exponents work

                    // Verifying negative/positive numbers

    F(-1.f,     "-1"      ),  // Minus sign written for integers
    F(-12.f,    "-12"     ),
    F(-120.f,   "-120"    ),

    F(-1.2f,    "-1.2"    ), // Minus sign written in decimal
    F(-0.12f,   "-0.12"   ), // format for negative numbers
    F(-0.012f,  "-0.012"  ),
    F(-1.2e7f,  "-1.2e+07"),

    F(fltNegZero, "-0"), // Negative zero is written as negative

    // Extremes
    F(std::numeric_limits<float>::max(), "3.4028235e+38"),
    F(std::numeric_limits<float>::min(), "1.1754944e-38"),

    // Subnormal/denormal numbers
    F(std::numeric_limits<float>::denorm_min(), "1e-45"),

    F(makeSubnormalFloat(0x000001), "1"        "e-45"),
    F(makeSubnormalFloat(0x000002), "3"        "e-45"),
    F(makeSubnormalFloat(0x000004), "6"        "e-45"),
    F(makeSubnormalFloat(0x000008), "1.1"      "e-44"),
    F(makeSubnormalFloat(0x000010), "2.2"      "e-44"),
    F(makeSubnormalFloat(0x000020), "4.5"      "e-44"),
    F(makeSubnormalFloat(0x000040), "9"        "e-44"),
    F(makeSubnormalFloat(0x000080), "1.8"      "e-43"),
    F(makeSubnormalFloat(0x000100), "3.59"     "e-43"),
    F(makeSubnormalFloat(0x000200), "7.17"     "e-43"),
    F(makeSubnormalFloat(0x000400), "1.435"    "e-42"),
    F(makeSubnormalFloat(0x000800), "2.87"     "e-42"),
    F(makeSubnormalFloat(0x001000), "5.74"     "e-42"),
    F(makeSubnormalFloat(0x002000), "1.148"    "e-41"),
    F(makeSubnormalFloat(0x004000), "2.2959"   "e-41"),
    F(makeSubnormalFloat(0x008000), "4.5918"   "e-41"),
    F(makeSubnormalFloat(0x010000), "9.1835"   "e-41"),
    F(makeSubnormalFloat(0x020000), "1.83671"  "e-40"),
    F(makeSubnormalFloat(0x040000), "3.67342"  "e-40"),
    F(makeSubnormalFloat(0x080000), "7.34684"  "e-40"),
    F(makeSubnormalFloat(0x100000), "1.469368" "e-39"),

    F(makeSubnormalFloat(0x000003), "4"        "e-45"),
    F(makeSubnormalFloat(0x000007), "1"        "e-44"),
    F(makeSubnormalFloat(0x00000F), "2.1"      "e-44"),
    F(makeSubnormalFloat(0x00001F), "4.3"      "e-44"),
    F(makeSubnormalFloat(0x00003F), "8.8"      "e-44"),
    F(makeSubnormalFloat(0x00007F), "1.78"     "e-43"),
    F(makeSubnormalFloat(0x0000FF), "3.57"     "e-43"),
    F(makeSubnormalFloat(0x0001FF), "7.16"     "e-43"),
    F(makeSubnormalFloat(0x0003FF), "1.434"    "e-42"),
    F(makeSubnormalFloat(0x0007FF), "2.868"    "e-42"),
    F(makeSubnormalFloat(0x000FFF), "5.738"    "e-42"),
    F(makeSubnormalFloat(0x001FFF), "1.1478"   "e-41"),
    F(makeSubnormalFloat(0x003FFF), "2.2957"   "e-41"),
    F(makeSubnormalFloat(0x007FFF), "4.5916"   "e-41"),
    F(makeSubnormalFloat(0x00FFFF), "9.1834"   "e-41"),
    F(makeSubnormalFloat(0x01FFFF), "1.8367"   "e-40"),
    F(makeSubnormalFloat(0x03FFFF), "3.6734"   "e-40"),
    F(makeSubnormalFloat(0x07FFFF), "7.34683"  "e-40"),
    F(makeSubnormalFloat(0x0FFFFF), "1.469367" "e-39"),
    F(makeSubnormalFloat(0x1FFFFF), "2.938734" "e-39"),

    F(makeSubnormalFloat(0x000005), "7"        "e-45"),
    F(makeSubnormalFloat(0x00000A), "1.4"      "e-44"),
    F(makeSubnormalFloat(0x000015), "3"        "e-44"),
    F(makeSubnormalFloat(0x00002A), "5.9"      "e-44"),
    F(makeSubnormalFloat(0x000055), "1.19"     "e-43"),
    F(makeSubnormalFloat(0x0000AA), "2.38"     "e-43"),
    F(makeSubnormalFloat(0x000155), "4.78"     "e-43"),
    F(makeSubnormalFloat(0x0002AA), "9.56"     "e-43"),
    F(makeSubnormalFloat(0x000555), "1.913"    "e-42"),
    F(makeSubnormalFloat(0x000AAA), "3.826"    "e-42"),
    F(makeSubnormalFloat(0x001555), "7.652"    "e-42"),
    F(makeSubnormalFloat(0x002AAA), "1.5305"   "e-41"),
    F(makeSubnormalFloat(0x005555), "3.0611"   "e-41"),
    F(makeSubnormalFloat(0x00AAAA), "6.1223"   "e-41"),
    F(makeSubnormalFloat(0x015555), "1.22447"  "e-40"),
    F(makeSubnormalFloat(0x02AAAA), "2.44894"  "e-40"),
    F(makeSubnormalFloat(0x055555), "4.89789"  "e-40"),
    F(makeSubnormalFloat(0x0AAAAA), "9.79578"  "e-40"),
    F(makeSubnormalFloat(0x155555), "1.959157" "e-39"),

    F(+1.f,      "1"  ), // Plus sign not written for integers
    F(+12.f,     "12" ),
    F(+120.f,    "120"),

    F(+1.2f,    "1.2"    ), // Plus sign not written in decimal
    F(+0.12f,   "0.12"   ), // format
    F(+0.012f,  "0.012"  ),

    F(+1.2e7f,  "1.2e+07"), // Plus is not written in scientific

                // Verifying decimal/scientific format selection

    F(1.f,        "1"       ), // Ad-hoc integers that are written in
    F(12345678.f, "12345678"), // decimal format

    F(1234567800000.f, "1234567823360"),  // Decimal format
    F(12345678000000.f, "1.2345678e+13"), // Switch to scientific

    F(0.f, "0"),  // Zero (no significant digits)

    // Single significant digit
    F(2e-5f, "2e-05"),
    F(2e-4f, "2e-04"),
    F(2e-3f, "0.002"),
    F(2e-2f, "0.02" ),
    F(2e-1f, "0.2"  ),
    F(2e0f,  "2"    ),
    F(2e1f,  "20"   ),
    F(2e2f,  "200"  ),
    F(2e3f,  "2000" ),
    F(2e4f,  "20000"),
    F(2e5f,  "2e+05"),
    F(2e6f,  "2e+06"),

    // Two significant digits
    F(1.2e-6f, "1.2e-06"),
    F(1.2e-5f, "1.2e-05"),
    F(1.2e-4f, "0.00012"),
    F(1.2e-3f, "0.0012" ),
    F(1.2e-2f, "0.012"  ),
    F(1.2e-1f, "0.12"   ),
    F(1.2e0f,  "1.2"    ),
    F(1.2e1f,  "12"     ),
    F(1.2e2f,  "120"    ),
    F(1.2e3f,  "1200"   ),
    F(1.2e4f,  "12000"  ),
    F(1.2e5f,  "120000" ),
    F(1.2e6f,  "1200000"),
    F(1.2e7f,  "1.2e+07"),
    F(1.2e8f,  "1.2e+08"),

    // Three significant digits
    F(1.23e-6f, "1.23e-06"),
    F(1.23e-5f, "1.23e-05"),
    F(1.23e-4f, "0.000123"),
    F(1.23e-3f, "0.00123" ),
    F(1.23e-2f, "0.0123"  ),
    F(1.23e-1f, "0.123"   ),
    F(1.23e0f,  "1.23"    ),
    F(1.23e1f,  "12.3"    ),
    F(1.23e2f,  "123"     ),
    F(1.23e3f,  "1230"    ),
    F(1.23e4f,  "12300"   ),
    F(1.23e5f,  "123000"  ),
    F(1.23e6f,  "1230000" ),
    F(1.23e7f,  "12300000"),
    F(1.23e8f,  "1.23e+08"),
    F(1.23e9f,  "1.23e+09"),

    // Four significant digits
    F(1.234e-6f, "1.234e-06"),
    F(1.234e-5f, "1.234e-05"),
    F(1.234e-4f, "0.0001234"),
    F(1.234e-3f, "0.001234" ),
    F(1.234e-2f, "0.01234"  ),
    F(1.234e-1f, "0.1234"   ),
    F(1.234e0f,  "1.234"    ),
    F(1.234e1f,  "12.34"    ),
    F(1.234e2f,  "123.4"    ),
    F(1.234e3f,  "1234"     ),
    F(1.234e4f,  "12340"    ),
    F(1.234e5f,  "123400"   ),
    F(1.234e6f,  "1234000"  ),
    F(1.234e7f,  "12340000" ),
    F(1.234e8f,  "123400000"),
    F(1.234e9f,  "1.234e+09"),
    F(1.234e10f, "1.234e+10"),

    // Five significant digits
    F(1.2345e-6f, "1.2345e-06"),
    F(1.2345e-5f, "1.2345e-05"),
    F(1.2345e-4f, "0.00012345"),
    F(1.2345e-3f, "0.0012345" ),
    F(1.2345e-2f, "0.012345"  ),
    F(1.2345e-1f, "0.12345"   ),
    F(1.2345e0f,  "1.2345"    ),
    F(1.2345e1f,  "12.345"    ),
    F(1.2345e2f,  "123.45"    ),
    F(1.2345e3f,  "1234.5"    ),
    F(1.2345e4f,  "12345"     ),
    F(1.2345e5f,  "123450"    ),
    F(1.2345e6f,  "1234500"   ),
    F(1.2345e7f,  "12345000"  ),
    F(1.2345e8f,  "123450000" ),
    F(1.2345e9f,  "1234499968"),
    F(1.2345e10f, "1.2345e+10"),
    F(1.2345e11f, "1.2345e+11"),

    // Six significant digits
    F(1.23456e-6f, "1.23456e-06"),
    F(1.23456e-5f, "1.23456e-05"),
    F(1.23456e-4f, "0.000123456"),
    F(1.23456e-3f, "0.00123456" ),
    F(1.23456e-2f, "0.0123456"  ),
    F(1.23456e-1f, "0.123456"   ),
    F(1.23456e0f,  "1.23456"    ),
    F(1.23456e1f,  "12.3456"    ),
    F(1.23456e2f,  "123.456"    ),
    F(1.23456e3f,  "1234.56"    ),
    F(1.23456e4f,  "12345.6"    ),
    F(1.23456e5f,  "123456"     ),
    F(1.23456e6f,  "1234560"    ),
    F(1.23456e7f,  "12345600"   ),
    F(1.23456e8f,  "123456000"  ),
    F(1.23456e9f,  "1234560000" ),
    F(1.23456e10f, "12345600000"),
    F(1.23456e11f, "1.23456e+11"),
    F(1.23456e12f, "1.23456e+12"),

    // Seven significant digits
    F(1.234567e-6f, "1.234567e-06"),
    F(1.234567e-5f, "1.234567e-05"),
    F(1.234567e-4f, "0.0001234567"),
    F(1.234567e-3f, "0.001234567" ),
    F(1.234567e-2f, "0.01234567"  ),
    F(1.234567e-1f, "0.1234567"   ),
    F(1.234567e0f,  "1.234567"    ),
    F(1.234567e1f,  "12.34567"    ),
    F(1.234567e2f,  "123.4567"    ),
    F(1.234567e3f,  "1234.567"    ),
    F(1.234567e4f,  "12345.67"    ),
    F(1.234567e5f,  "123456.7"    ),
    F(1.234567e6f,  "1234567"     ),
    F(1.234567e7f,  "12345670"    ),
    F(1.234567e8f,  "123456704"   ),
    F(1.234567e9f,  "1234567040"  ),
    F(1.234567e10f, "12345669632" ),
    F(1.234567e11f, "123456700416"),
    F(1.234567e12f, "1.234567e+12"),
    F(1.234567e13f, "1.234567e+13"),

    // Eight significant digits
    F(1.2345678e-6f, "1.2345678e-06"),
    F(1.2345678e-5f, "1.2345678e-05"),
    F(1.2345678e-4f, "0.00012345678"),
    F(1.2345678e-3f, "0.0012345678" ),
    F(1.2345678e-2f, "0.012345678"  ),
    F(1.2345678e-1f, "0.12345678"   ),
    F(1.2345678e0f,  "1.2345678"    ),
    F(1.2345678e1f,  "12.345678"    ),
    F(1.2345678e2f,  "123.45678"    ),
    F(1.2345677e3f,  "1234.5677"    ),
    F(1.2345678e4f,  "12345.678"    ),
    F(1.2345678e5f,  "123456.78"    ),
    F(1.2345678e6f,  "1234567.8"    ),
    F(1.2345678e7f,  "12345678"     ),
    F(1.2345678e8f,  "123456784"    ),
    F(1.2345678e9f,  "1234567808"   ),
    F(1.2345678e10f, "12345677824"  ),
    F(1.2345678e11f, "123456782336" ),
    F(1.2345678e12f, "1234567823360"),
    F(1.2345678e13f, "1.2345678e+13"),
    F(1.2345678e14f, "1.2345678e+14"),

    // Nine significant digits

    // For certain exponents, there are no 'float' numbers that require 9
    // significant decimal digits when converted to a string to convert back to
    // the same number.

    //F(1.????????e-6f, "1.????????e-06"), // no such 9 digit number exists
    F(1.23456775e-5f, "1.23456775e-05"),
    F(1.21999976e-4f, "0.000121999976"),
    //F(1.????????e-3f, "0.00?????????" ), // no such 9 digit number exists
    F(1.23456735e-2f, "0.0123456735"  ),
    F(1.23456776e-1f, "0.123456776"   ),
    //F(1.????????e0f,  "1.????????"    ), // no such 9 digit number exists
    F(1.23456745e1f,  "12.3456745"    ),
    F(1.23456764e2f,  "123.456764"    ),
    F(1.02345685e3f,  "1023.45685"    ),
    F(1.23456875e4f,  "12345.6875"    ),
    F(1.23456766e5f,  "123456.766"    ),
    F(1.02345675e6f,  "1023456.75"    ),
    //F(1.????????e7f,  "1???????.?"    ), // no such 9 digit number exists
    F(1.23456776e8f,  "123456776"     ),
    F(1.02345683e9f,  "1023456832"    ),
    //F(1.????????e10f, "1????????00"   ), // no such 9 digit number exists
    F(1.23456774e11f, "123456774144"  ),
    F(1.02345684e12f, "1023456837632" ),
    //F(1.????????e13f, "1????????00000"), // no such 9 digit number exists
    F(1.23456855e14f, "1.23456855e+14"),
    //F(1.????????e15f, "1.????????e+15"), // no such 9 digit number exists

                // Leading decimal zero(s) regression

    // Three significant digits
    F(1.03e0f,  "1.03"),

    // Four significant digits
    F(1.034e0f,  "1.034"),
    F(1.004e0f,  "1.004"),
    F(1.204e1f,  "12.04"),

    // Five significant digits
    F(1.0345e0f,   "1.0345"),
    F(1.0045e0f,   "1.0045"),
    F(1.0005e0f,   "1.0005"),
    F(1.2045e1f,   "12.045"),
    F(1.2005e1f,   "12.005"),
    F(1.2305e2f,   "123.05"),

    // Six significant digits
    F(1.03456e0f,  "1.03456"),
    F(1.00456e0f,  "1.00456"),
    F(1.00056e0f,  "1.00056"),
    F(1.00006e0f,  "1.00006"),
    F(1.20456e1f,  "12.0456"),
    F(1.20056e1f,  "12.0056"),
    F(1.20006e1f,  "12.0006"),
    F(1.23056e2f,  "123.056"),
    F(1.23006e2f,  "123.006"),
    F(1.23406e3f,  "1234.06"),

    // Seven significant digits
    F(1.034567e0f,  "1.034567"),
    F(1.004567e0f,  "1.004567"),
    F(1.000567e0f,  "1.000567"),
    F(1.000067e0f,  "1.000067"),
    F(1.000007e0f,  "1.000007"),
    F(1.204567e1f,  "12.04567"),
    F(1.200567e1f,  "12.00567"),
    F(1.200067e1f,  "12.00067"),
    F(1.200007e1f,  "12.00007"),
    F(1.230567e2f,  "123.0567"),
    F(1.230067e2f,  "123.0067"),
    F(1.230007e2f,  "123.0007"),
    F(1.234067e3f,  "1234.067"),
    F(1.234007e3f,  "1234.007"),
    F(1.234507e4f,  "12345.07"),

    // Eight significant digits
    F(1.0345678e0f,  "1.0345678"),
    F(1.0045678e0f,  "1.0045677"),
    F(1.0005678e0f,  "1.0005678"),
    F(1.0000678e0f,  "1.0000678"),
    F(1.0000078e0f,  "1.0000077"),
    F(1.0000008e0f,  "1.0000008"),
    F(1.2045678e1f,  "12.045678"),
    F(1.2005678e1f,  "12.005678"),
    F(1.2000678e1f,  "12.000678"),
    F(1.2000078e1f,  "12.000078"),
    F(1.2000008e1f,  "12.000008"),
    F(1.2305678e2f,  "123.05678"),
    F(1.2300678e2f,  "123.00678"),
    F(1.2300078e2f,  "123.00078"),
    F(1.2300008e2f,  "123.00008"),
    F(1.2340677e3f,  "1234.0677"),
    F(1.2340077e3f,  "1234.0077"),
    F(1.2340007e3f,  "1234.0007"),
    F(1.2345078e4f,  "12345.078"),
    F(1.2345008e4f,  "12345.008"),
    F(1.2345608e5f,  "123456.08"),
#undef F
};

// ----------------------------------------------------------------------------
// 'float' "minimal" format precisely written integers test data
static const FltIntData k_FLT_CPP_INT[] = {

#define F(integer, exponent, expected) { L_, integer, exponent, expected }

                              // Small Integers

    F(0x01, 0,  "1"),
    F(0x02, 0,  "2"),
    F(0x03, 0,  "3"),
    F(0x04, 0,  "4"),
    F(0x05, 0,  "5"),
    F(0x06, 0,  "6"),
    F(0x07, 0,  "7"),
    F(0x08, 0,  "8"),
    F(0x09, 0,  "9"),
    F(0x0a, 0, "10"),
    F(0x0b, 0, "11"),
    F(0x0c, 0, "12"),
    F(0x0d, 0, "13"),
    F(0x0e, 0, "14"),
    F(0x0f, 0, "15"),
    F(0x10, 0, "16"),
    F(0x11, 0, "17"),
    F(0x12, 0, "18"),
    F(0x13, 0, "19"),
    F(0x14, 0, "20"),
    F(0x15, 0, "21"),
    F(0x16, 0, "22"),
    F(0x17, 0, "23"),
    F(0x18, 0, "24"),
    F(0x19, 0, "25"),
    F(0x1a, 0, "26"),
    F(0x1b, 0, "27"),
    F(0x1c, 0, "28"),
    F(0x1d, 0, "29"),
    F(0x1e, 0, "30"),
    F(0x1f, 0, "31"),

    // {DRQS 165162213<GO>}
    F(1, 24,      "16777216"),

    // 1 bit integers that will be represented precisely (decimal chosen)
    F(1, 41, "2199023255552"),
    F(1, 42, "4398046511104"),

    // Largest integer that is written precisely (decimal notation chosen)
    F(0x4A0129, 20, "5085552705536"),
    // Next integer is written in scientific mode
    F(0x4A012A, 20, "5.085554e+12"),

               // Less than 23 Significant Digits Integers

    F(0x200001, 0, "2097153"),
    F(0x3FFFFF, 0, "4194303"),

                     // 23 Significant Digits Integers

    F(0x400001, 0, "4194305"),
    F(0x555555, 0, "5592405"),
    F(0x499999, 0, "4823449"),
    F(0x7FFFFF, 0, "8388607"),


                // 24 Significant Binary Digits Integers

    F(0x800001, 0, "8388609"),
    F(0xD55555, 0, "13981013"),
    F(0x999999, 0, "10066329"),
    F(0xFFFFFF, 0, "16777215"),

       // More than 24 Significant Bin Digits Decimal Notation Integers

    F(0x800001,  1,    "16777218"),
    F(0xD55555,  1,    "27962026"),
    F(0x999999,  1,    "20132658"),
    F(0xFFFFFF,  1,    "33554430"),

    F(0x800001,  2,    "33554436"),
    F(0xD55555,  2,    "55924052"),
    F(0x999999,  2,    "40265316"),
    F(0xFFFFFF,  2,    "67108860"),

    F(0x800001,  3,    "67108872"),
    F(0xD55555,  3,   "111848104"),
    F(0x999999,  3,    "80530632"),
    F(0xFFFFFF,  3,   "134217720"),

    F(0x800001,  7,  "1073741952"),
    F(0xD55555,  7,  "1789569664"),
    F(0x999999,  7,  "1288490112"),
    F(0xFFFFFF,  7,  "2147483520"),

    F(0x800001, 11,  "17179871232"),
    F(0xD55555, 11,  "28633114624"),
    F(0x999999, 11,  "20615841792"),
    F(0xFFFFFF, 11,  "34359736320"),

    F(0x800001, 14,  "137438969856"),
    F(0xD55555, 14,  "229064916992"),
    F(0x999999, 14,  "164926734336"),
    F(0xFFFFFF, 14,  "274877890560"),

    F(0x800001, 17, "1099511758848"),
    F(0xD55555, 17, "1832519335936"),
    F(0x999999, 17, "1319413874688"),
    F(0xFFFFFF, 17, "2199023124480"),

    F(0x800001, 19,  "4.398047e+12"), // scientific notation is shorter!
    F(0xD55555, 19, "7330077343744"),
    F(0x999999, 19, "5277655498752"),
    F(0xFFFFFF, 19, "8796092497920"),

    // More than 24 Significant Binary Digits Scientific Notation Integers

    F(0x800001,  20, "8.796094e+12" ),
    F(0xD55555,  20, "1.4660155e+13"),
    F(0x999999,  20, "1.0555311e+13"),
    F(0xFFFFFF,  20, "1.7592185e+13"),

    F(0x800001,  21, "1.7592188e+13"),
    F(0xD55555,  21, "2.932031e+13" ),
    F(0x999999,  21, "2.1110622e+13"),
    F(0xFFFFFF,  21, "3.518437e+13" ),

    F(0x800001,  42, "3.6893493e+19"),
    F(0xD55555,  42, "6.1489145e+19" ),
    F(0x999999,  42, "4.4272183e+19"),
    F(0xFFFFFF,  42, "7.378697e+19" ),

    F(0x800001,  43, "7.3786985e+19" ),
    F(0xD55555,  43, "1.2297829e+20"),
    F(0x999999,  43, "8.854437e+19"),
    F(0xFFFFFF,  43, "1.4757394e+20"),

    F(0x800001,  99, "5.3169126e+36" ),
    F(0xD55555,  99, "8.86152e+36"),
    F(0x999999,  99, "6.380294e+36"),
    F(0xFFFFFF,  99, "1.06338233e+37"),

    F(0x800001, 104, "1.701412e+38" ),
    F(0xD55555, 104, "2.8356863e+38"),
    F(0x999999, 104, "2.041694e+38"),
    F(0xFFFFFF, 104, "3.4028235e+38"),
#undef F
};

// ----------------------------------------------------------------------------
// 'float' scientific format test data
static const FltData k_FLT_SCI[] = {

    #define F(value, expected) { L_, value, expected }

            // Verifying C++ compatible scientific format exponent

    F(1.2e12f,  "1.2e+12"), // Exponent '+' is sign displayed
    F(1.2e7f,   "1.2e+07"), // Exponent leading 0 is added for 0-9
    F(1.2e38f,  "1.2e+38"), // Maximum exponent works
    F(1.2e-38f, "1.2e-38"), // Minimum exponent works
    F(1.2e-7f,  "1.2e-07"), // Negative exponents work

                    // Verifying negative/positive numbers

    F(-1.f,     "-1e+00"  ),  // Minus sign written for integers
    F(-12.f,    "-1.2e+01"),
    F(-120.f,   "-1.2e+02"),

    F(-1.2f,    "-1.2e+00"), // Minus sign written in decimal
    F(-0.12f,   "-1.2e-01"), // format for negative numbers
    F(-0.012f,  "-1.2e-02"),
    F(-1.2e7f,  "-1.2e+07"),

    F(0.f,         "0e+00"), // Zero is written w/o sign
    F(fltNegZero, "-0e+00"), // Negative zero is written as negative

    F(+1.f,      "1e+00"  ), // Plus sign not written for integers
    F(+12.f,     "1.2e+01"),
    F(+120.f,    "1.2e+02"),

    F(+1.2f,    "1.2e+00"), // Plus sign not written in decimal
    F(+0.12f,   "1.2e-01"), // format
    F(+0.012f,  "1.2e-02"),
    F(+1.2e7f,  "1.2e+07"),

    // Extremes
    F(FLT_MAX, "3.4028235e+38"),
    F(FLT_MIN, "1.1754944e-38"),
#ifdef FLT_TRUE_MIN
    F(FLT_TRUE_MIN, "1e-45"),
#else
    F(FLT_MIN / (1ull << 23), "1e-45"),
#endif

    // Single significant digit
    F(0.00002f,  "2e-05"),
    F(0.0002f,   "2e-04"),
    F(0.002f,    "2e-03"),
    F(0.02f,     "2e-02"),
    F(0.2f,      "2e-01"),
    F(2.f,       "2e+00"),
    F(20.f,      "2e+01"),
    F(200.f,     "2e+02"),
    F(2000.f,    "2e+03"),
    F(20000.f,   "2e+04"),
    F(200000.f,  "2e+05"),
    F(2000000.f, "2e+06"),

    F(1e+38f,    "1e+38"),

    // Two significant digits
    F(1.5e-44f,    "1.5e-44"),

    F(0.0000012f,  "1.2e-06"),
    F(0.000012f,   "1.2e-05"),
    F(0.00012f,    "1.2e-04"),
    F(0.0012f,     "1.2e-03"),
    F(0.012f,      "1.2e-02"),
    F(0.12f,       "1.2e-01"),
    F(1.2f,        "1.2e+00"),
    F(12.f,        "1.2e+01"),
    F(120.f,       "1.2e+02"),
    F(1200.f,      "1.2e+03"),
    F(12000.f,     "1.2e+04"),
    F(120000.f,    "1.2e+05"),
    F(1200000.f,   "1.2e+06"),
    F(12000000.f,  "1.2e+07"),
    F(120000000.f, "1.2e+08"),

    F(1.2e+38f,    "1.2e+38"),

    // Three significant digits
    F(1.23e-43f,  "1.23e-43"),

    F(0.0000123f, "1.23e-05"),
    F(0.000123f,  "1.23e-04"),
    F(0.00123f,   "1.23e-03"),
    F(0.0123f,    "1.23e-02"),
    F(0.123f,     "1.23e-01"),
    F(1.23f,      "1.23e+00"),
    F(12.3f,      "1.23e+01"),
    F(123.f,      "1.23e+02"),
    F(1230.f,     "1.23e+03"),
    F(12300.f,    "1.23e+04"),
    F(123000.f,   "1.23e+05"),
    F(1230000.f,  "1.23e+06"),
    F(12300000.f, "1.23e+07"),

    F(1.23e+38f,  "1.23e+38"),

    // Four significant digits
    F(1.234e-42f,  "1.235e-42"),

    F(0.00001234f, "1.234e-05"),
    F(0.0001234f,  "1.234e-04"),
    F(0.001234f,   "1.234e-03"),
    F(0.01234f,    "1.234e-02"),
    F(0.1234f,     "1.234e-01"),
    F(1.234f,      "1.234e+00"),
    F(12.34f,      "1.234e+01"),
    F(123.4f,      "1.234e+02"),
    F(1234.f,      "1.234e+03"),
    F(12340.f,     "1.234e+04"),
    F(123400.f,    "1.234e+05"),
    F(1234000.f,   "1.234e+06"),

    F(1.234e+38f,  "1.234e+38"),

    // Nine significant digits

    // For certain exponents, there are no 'float' numbers that require
    // 9 significant decimal digits when converted to a string to
    // convert back to the same number.

    //F(1.????????e-6f,  "1.????????e-06"), // no 9 digit number exists
    F(1.23456775e-5f,    "1.23456775e-05"),
    F(0.000121999976f,   "1.21999976e-04"),
    //F(1.????????e-3f,  "1.????????e-03"), // no 9 digit number exists
    F(0.0123456735f,     "1.23456735e-02"),
    F(0.123456776f,      "1.23456776e-01"),
    //F(1.????????e0f,   "1.????????e+00"), // no 9 digit number exists
    F(12.3456745f,       "1.23456745e+01"),
    F(123.456764f,       "1.23456764e+02"),
    F(1023.45685f,       "1.02345685e+03"),
    F(12345.6875f,       "1.23456875e+04"),
    F(123456.766f,       "1.23456766e+05"),
    F(1023456.75f,       "1.02345675e+06"),
    //F(1.????????e7f,   "1.???????e+07" ), // no 9 digit number exists
    F(123456776.f,       "1.23456776e+08"),
    F(1023456830.f,      "1.02345683e+09"),
    //F(1.????????e10f,  "1.???????e+10" ), // no 9 digit number exists
    F(123456774000.f,    "1.23456774e+11"),
    F(1023456840000.f,   "1.02345684e+12"),
    //F(1.????????e13f,  "1.???????e+13" ), // no 9 digit number exists
    F(123456855000000.f, "1.23456855e+14"),
    //F(1.????????e15f,  "1.????????e+15"), // no 9 digit number exists
#undef F
};

// ----------------------------------------------------------------------------
// 'float' decimal/fixed format test data
static const FltData k_FLT_DEC[] = {

#define F(value, expected) { L_, value, expected }

                   // Verifying negative/positive numbers

    F(-1.f,     "-1"  ),  // Minus sign written for integers
    F(-12.f,    "-12" ),
    F(-120.f,   "-120"),

    F(-1.2f,    "-1.2"     ), // Minus sign written in decimal
    F(-0.12f,   "-0.12"    ), // format for negative numbers
    F(-0.012f,  "-0.012"   ),
    F(-1.2e7f,  "-12000000"),

    F(0.f,         "0"),  // Zero (no significant digits)
    F(fltNegZero, "-0"),  // Negative zero is written as negative

    F(+1.f,      "1"  ), // Plus sign not written for integers
    F(+12.f,     "12" ),
    F(+120.f,    "120"),

    F(+1.2f,    "1.2"    ), // Plus sign not written in decimal
    F(+0.12f,   "0.12"   ), // format
    F(+0.012f,  "0.012"  ),

    F(+1.2e7f,  "12000000"), // Plus is not written for large integers

    // Extremes
    F(FLT_MAX, "340282346638528859811704183484516925440"),
    F(FLT_MIN, "0.000000000000000000000000000000000000011754944"),
#ifdef FLT_TRUE_MIN
    F(FLT_TRUE_MIN, "0.000000000000000000000000000000000000000000001"),
#else
    F(FLT_MIN / (1ull << 23),
      "0.000000000000000000000000000000000000000000001"),
#endif
            // Verifying Different Number of Significant Digits

    // Single significant digit
    F(2e-38f,  "0.00000000000000000000000000000000000002"),
    F(2e-15f,  "0.000000000000002"),
    F(2e-5f,   "0.00002"          ),
    F(2e-4f,   "0.0002"           ),
    F(2e-3f,   "0.002"            ),
    F(2e-2f,   "0.02"             ),
    F(2e-1f,   "0.2"              ),
    F(2e0f,    "2"                ),
    F(2e1f,    "20"               ),
    F(2e2f,    "200"              ),
    F(2e3f,    "2000"             ),
    F(2e4f,    "20000"            ),
    F(2e5f,    "200000"           ),
    F(2e6f,    "2000000"          ),
    F(2e7f,    "20000000"         ),
    F(2e8f,    "200000000"        ),
    F(2e9f,    "2000000000"       ),
    F(2e10f,   "20000000000"      ),
    F(2e11f,   "199999995904"     ),
    F(2e12f,   "1999999991808"    ),
    F(2e13f,   "19999999655936"   ),
    F(2e14f,   "200000000753664"  ),
    F(2e15f,   "1999999973982208" ),
    F(2e16f,   "20000000545128448"),

    F(1e+38f, "99999996802856924650656260769173209088"),

    // Two significant digits
    F(1.3e-44f, "0.000000000000000000000000000000000000000000013"),

    F(1.2e-38f,  "0.000000000000000000000000000000000000012"),
    F(1.2e-15f,  "0.0000000000000012"),
    F(1.2e-5f,   "0.000012"          ),
    F(1.2e-4f,   "0.00012"           ),
    F(1.2e-3f,   "0.0012"            ),
    F(1.2e-2f,   "0.012"             ),
    F(1.2e-1f,   "0.12"              ),
    F(1.2e0f,    "1.2"               ),
    F(1.2e1f,    "12"                ),
    F(1.2e2f,    "120"               ),
    F(1.2e3f,    "1200"              ),
    F(1.2e4f,    "12000"             ),
    F(1.2e5f,    "120000"            ),
    F(1.2e6f,    "1200000"           ),
    F(1.2e7f,    "12000000"          ),
    F(1.2e8f,    "120000000"         ),
    F(1.2e9f,    "1200000000"        ),
    F(1.2e10f,   "12000000000"       ),
    F(1.2e11f,   "120000004096"      ),
    F(1.2e12f,   "1199999942656"     ),
    F(1.2e13f,   "12000000212992"    ),
    F(1.2e14f,   "120000002129920"   ),
    F(1.2e15f,   "1200000038076416"  ),
    F(1.2e16f,   "12000000112328704" ),
    F(1.2e17f,   "120000003270770688"),

    F(1.2e+38f, "120000004276392151041455682501908365312"),

    // Three significant digits
    F(1.23e-43f, "0.000000000000000000000000000000000000000000123"),

    F(1.23e-38f,  "0.0000000000000000000000000000000000000123"),
    F(1.23e-12f,  "0.00000000000123"),
    F(1.23e-5f,   "0.0000123"       ),
    F(1.23e-4f,   "0.000123"        ),
    F(1.23e-3f,   "0.00123"         ),
    F(1.23e-2f,   "0.0123"          ),
    F(1.23e-1f,   "0.123"           ),
    F(1.23e0f,    "1.23"            ),
    F(1.23e1f,    "12.3"            ),
    F(1.23e2f,    "123"             ),
    F(1.23e3f,    "1230"            ),
    F(1.23e4f,    "12300"           ),
    F(1.23e5f,    "123000"          ),
    F(1.23e6f,    "1230000"         ),
    F(1.23e7f,    "12300000"        ),
    F(1.23e8f,    "123000000"       ),
    F(1.23e9f,    "1230000000"      ),
    F(1.23e10f,   "12300000256"     ),
    F(1.23e11f,   "122999996416"    ),
    F(1.23e12f,   "1229999964160"   ),
    F(1.23e13f,   "12299999903744"  ),
    F(1.23e14f,   "123000003231744" ),
    F(1.23e15f,   "1229999981985792"),

    F(1.23e+38f, "122999995763277873265532144386874277888"),

    // Four significant digits
    F(1.235e-42f, "0.000000000000000000000000000000000000000001235"),

    F(1.234e-38f,  "0.00000000000000000000000000000000000001234"),
    F(1.234e-12f,  "0.000000000001234"),
    F(1.234e-5f,   "0.00001234"       ),
    F(1.234e-4f,   "0.0001234"        ),
    F(1.234e-3f,   "0.001234"         ),
    F(1.234e-2f,   "0.01234"          ),
    F(1.234e-1f,   "0.1234"           ),
    F(1.234e0f,    "1.234"            ),
    F(1.234e1f,    "12.34"            ),
    F(1.234e2f,    "123.4"            ),
    F(1.234e3f,    "1234"             ),
    F(1.234e4f,    "12340"            ),
    F(1.234e5f,    "123400"           ),
    F(1.234e6f,    "1234000"          ),
    F(1.234e7f,    "12340000"         ),
    F(1.234e8f,    "123400000"        ),
    F(1.234e9f,    "1234000000"       ),
    F(1.234e10f,   "12339999744"      ),
    F(1.234e11f,   "123400003584"     ),
    F(1.234e12f,   "1234000019456"    ),
    F(1.234e13f,   "12339999932416"   ),
    F(1.234e14f,   "123399997227008"  ),
    F(1.234e15f,   "1233999938715648" ),
    F(1.234e16f,   "12340000460898304"),

    F(1.234e+38f, "123399995304276289683798020103111442432"),

    // Eight significant digits
    F(1.2345678e-38f, "0.000000000000000000000000000000000000012345678"),

    F(1.2345678e-38f, "0.000000000000000000000000000000000000012345678"),
    F(1.2345678e-12f,  "0.0000000000012345678"),
    F(1.2345678e-5f,   "0.000012345678"       ),
    F(1.2345678e-4f,   "0.00012345678"        ),
    F(1.2345678e-3f,   "0.0012345678"         ),
    F(1.2345678e-2f,   "0.012345678"          ),
    F(1.2345678e-1f,   "0.12345678"           ),
    F(1.2345678e0f,    "1.2345678"            ),
    F(1.2345678e1f,    "12.345678"            ),
    F(1.2345678e2f,    "123.45678"            ),
    F(1.2345678e3f,    "1234.5677"            ),
    F(1.2345678e4f,    "12345.678"            ),
    F(1.2345678e5f,    "123456.78"            ),
    F(1.2345678e6f,    "1234567.8"            ),
    F(1.2345678e7f,    "12345678"             ),
    F(1.2345678e8f,    "123456784"            ),
    F(1.2345678e9f,    "1234567808"           ),
    F(1.2345678e10f,   "12345677824"          ),
    F(1.2345678e11f,   "123456782336"         ),
    F(1.2345678e12f,   "1234567823360"        ),
    F(1.2345678e13f,   "12345677971456"       ),
    F(1.2345678e14f,   "123456779714560"      ),
    F(1.2345678e15f,   "1234567813922816"     ),
    F(1.2345678e16f,   "12345678407663616"    ),
    F(1.2345678e17f,   "123456781929152512"   ),
    F(1.2345678e18f,   "1234567802111655936"  ),
    F(1.2345678e19f,   "12345678295994466304" ),
    F(1.2345678e20f,   "123456780760921407488"),

    F(1.2345678e+38f, "123456775909961712535149860433086644224"),

    // Nine significant digits

    // For certain exponents, there are no 'float' numbers that require 9
    // significant decimal digits when converted to a string to convert back to
    // the same number.

    //F(1.????????e-6f, "???"), // no such 9 digit number exists
    F(1.23456775e-5f, "0.0000123456775"),
    F(1.21999976e-4f, "0.000121999976" ),
    //F(1.????????e-3f, "???" ), // no such 9 digit number exists
    F(1.23456735e-2f, "0.0123456735"   ),
    F(1.23456776e-1f, "0.123456776"    ),
    //F(1.????????e0f,  "1.????????"), // no such 9 digit number exists
    F(1.23456745e1f,  "12.3456745"     ),
    F(1.23456764e2f,  "123.456764"     ),
    F(1.02345685e3f,  "1023.45685"     ),
    F(1.23456875e4f,  "12345.6875"     ),
    F(1.23456766e5f,  "123456.766"     ),
    F(1.02345675e6f,  "1023456.75"     ),
    //F(1.????????e7f,  "1???????.?"), // no such 9 digit number exists
    F(1.23456776e8f,  "123456776"      ),
    F(1.02345683e9f,  "1023456832"     ),
    //F(1.????????e10f, "1????????00"), // no such 9 digit number exists
    F(1.23456774e11f, "123456774144"   ),
    F(1.02345684e12f, "1023456837632"  ),
    //F(1.????????e13f, "1????????00000"), // no such 9 digit number exists
    F(1.23456855e14f, "123456855212032"),
    //F(1.????????e15f, "1.????????e+15"), // no such 9 digit number exists

                   // Leading decimal zero(s) regression

    // Three significant digits
    F(1.03e0f,  "1.03"),

    // Four significant digits
    F(1.034e0f,  "1.034"),
    F(1.004e0f,  "1.004"),
    F(1.204e1f,  "12.04"),

    // Five significant digits
    F(1.0345e0f,   "1.0345"),
    F(1.0045e0f,   "1.0045"),
    F(1.0005e0f,   "1.0005"),
    F(1.2045e1f,   "12.045"),
    F(1.2005e1f,   "12.005"),
    F(1.2305e2f,   "123.05"),

    // Six significant digits
    F(1.03456e0f,  "1.03456"),
    F(1.00456e0f,  "1.00456"),
    F(1.00056e0f,  "1.00056"),
    F(1.00006e0f,  "1.00006"),
    F(1.20456e1f,  "12.0456"),
    F(1.20056e1f,  "12.0056"),
    F(1.20006e1f,  "12.0006"),
    F(1.23056e2f,  "123.056"),
    F(1.23006e2f,  "123.006"),
    F(1.23406e3f,  "1234.06"),

    // Seven significant digits
    F(1.034567e0f,  "1.034567"),
    F(1.004567e0f,  "1.004567"),
    F(1.000567e0f,  "1.000567"),
    F(1.000067e0f,  "1.000067"),
    F(1.000007e0f,  "1.000007"),
    F(1.204567e1f,  "12.04567"),
    F(1.200567e1f,  "12.00567"),
    F(1.200067e1f,  "12.00067"),
    F(1.200007e1f,  "12.00007"),
    F(1.230567e2f,  "123.0567"),
    F(1.230067e2f,  "123.0067"),
    F(1.230007e2f,  "123.0007"),
    F(1.234067e3f,  "1234.067"),
    F(1.234007e3f,  "1234.007"),
    F(1.234507e4f,  "12345.07"),

    // Eight significant digits
    F(1.0345678e0f,  "1.0345678"),
    F(1.0045678e0f,  "1.0045677"),
    F(1.0005678e0f,  "1.0005678"),
    F(1.0000678e0f,  "1.0000678"),
    F(1.0000078e0f,  "1.0000077"),
    F(1.0000008e0f,  "1.0000008"),
    F(1.2045678e1f,  "12.045678"),
    F(1.2005678e1f,  "12.005678"),
    F(1.2000678e1f,  "12.000678"),
    F(1.2000078e1f,  "12.000078"),
    F(1.2000008e1f,  "12.000008"),
    F(1.2305678e2f,  "123.05678"),
    F(1.2300678e2f,  "123.00678"),
    F(1.2300078e2f,  "123.00078"),
    F(1.2300008e2f,  "123.00008"),
    F(1.2340677e3f,  "1234.0677"),
    F(1.2340077e3f,  "1234.0077"),
    F(1.2340007e3f,  "1234.0007"),
    F(1.2345078e4f,  "12345.078"),
    F(1.2345008e4f,  "12345.008"),
    F(1.2345608e5f,  "123456.08"),
#undef F
};

// ----------------------------------------------------------------------------
// 'float' decimal format precisely written integers test data
static const FltIntData k_FLT_DEC_INT[] = {

#define F(integer, exponent, expected) { L_, integer, exponent, expected }

                            // Small Integers

    F(0x01, 0,  "1"),
    F(0x02, 0,  "2"),
    F(0x03, 0,  "3"),
    F(0x04, 0,  "4"),
    F(0x05, 0,  "5"),
    F(0x06, 0,  "6"),
    F(0x07, 0,  "7"),
    F(0x08, 0,  "8"),
    F(0x09, 0,  "9"),
    F(0x0a, 0, "10"),
    F(0x0b, 0, "11"),
    F(0x0c, 0, "12"),
    F(0x0d, 0, "13"),
    F(0x0e, 0, "14"),
    F(0x0f, 0, "15"),
    F(0x10, 0, "16"),
    F(0x11, 0, "17"),
    F(0x12, 0, "18"),
    F(0x13, 0, "19"),
    F(0x14, 0, "20"),
    F(0x15, 0, "21"),
    F(0x16, 0, "22"),
    F(0x17, 0, "23"),
    F(0x18, 0, "24"),
    F(0x19, 0, "25"),
    F(0x1a, 0, "26"),
    F(0x1b, 0, "27"),
    F(0x1c, 0, "28"),
    F(0x1d, 0, "29"),
    F(0x1e, 0, "30"),
    F(0x1f, 0, "31"),

               // Less than 23 Significant Digits Integers

    F(0x200001, 0, "2097153"),
    F(0x3FFFFF, 0, "4194303"),

                // 23 Significant Binary Digits Integers

    F(0x400001, 0, "4194305"),
    F(0x555555, 0, "5592405"),
    F(0x499999, 0, "4823449"),
    F(0x7FFFFF, 0, "8388607"),


                // 24 Significant Binary Digits Integers

    F(0x800001, 0, "8388609"),
    F(0xD55555, 0, "13981013"),
    F(0x999999, 0, "10066329"),
    F(0xFFFFFF, 0, "16777215"),

            // More than 24 Significant Binary Digits Integers

    F(0x800001,   1,                               "16777218"),
    F(0xD55555,   1,                               "27962026"),
    F(0x999999,   1,                               "20132658"),
    F(0xFFFFFF,   1,                               "33554430"),

    F(0x800001,   2,                               "33554436"),
    F(0xD55555,   2,                               "55924052"),
    F(0x999999,   2,                               "40265316"),
    F(0xFFFFFF,   2,                               "67108860"),

    F(0x800001,   3,                               "67108872"),
    F(0xD55555,   3,                              "111848104"),
    F(0x999999,   3,                               "80530632"),
    F(0xFFFFFF,   3,                              "134217720"),

    F(0x800001,   7,                             "1073741952"),
    F(0xD55555,   7,                             "1789569664"),
    F(0x999999,   7,                             "1288490112"),
    F(0xFFFFFF,   7,                             "2147483520"),

    F(0x800001,  11,                            "17179871232"),
    F(0xD55555,  11,                            "28633114624"),
    F(0x999999,  11,                            "20615841792"),
    F(0xFFFFFF,  11,                            "34359736320"),

    F(0x800001,  14,                           "137438969856"),
    F(0xD55555,  14,                           "229064916992"),
    F(0x999999,  14,                           "164926734336"),
    F(0xFFFFFF,  14,                           "274877890560"),

    F(0x800001,  17,                          "1099511758848"),
    F(0xD55555,  17,                          "1832519335936"),
    F(0x999999,  17,                          "1319413874688"),
    F(0xFFFFFF,  17,                          "2199023124480"),

    F(0x800001,  19,                          "4398047035392"),
    F(0xD55555,  19,                          "7330077343744"),
    F(0x999999,  19,                          "5277655498752"),
    F(0xFFFFFF,  19,                          "8796092497920"),

    F(0x800001,  20,                          "8796094070784"),
    F(0xD55555,  20,                         "14660154687488"),
    F(0x999999,  20,                         "10555310997504"),
    F(0xFFFFFF,  20,                         "17592184995840"),

    F(0x800001,  21,                         "17592188141568"),
    F(0xD55555,  21,                         "29320309374976"),
    F(0x999999,  21,                         "21110621995008"),
    F(0xFFFFFF,  21,                         "35184369991680"),

    F(0x800001,  42,                   "36893492545465614336"),
    F(0xD55555,  42,                   "61489145446349668352"),
    F(0x999999,  42,                   "44272183138075017216"),
    F(0xFFFFFF,  42,                   "73786971896791695360"),

    F(0x800001,  43,                   "73786985090931228672"),
    F(0xD55555,  43,                  "122978290892699336704"),
    F(0x999999,  43,                   "88544366276150034432"),
    F(0xFFFFFF,  43,                  "147573943793583390720"),

    F(0x800001,  99,  "5316912616964963605729928989472980992"),
    F(0xD55555,  99,  "8861519760624339114653813485751762944"),
    F(0x999999,  99,  "6380293999472416121469453440334692352"),
    F(0xFFFFFF,  99, "10633823332454026869115755733891153920"),

    F(0x800001, 104, "170141203742878835383357727663135391744"),
    F(0xD55555, 104, "283568632339978851668922031544056414208"),
    F(0x999999, 104, "204169407983117315887022510090710155264"),
    F(0xFFFFFF, 104, "340282346638528859811704183484516925440"),
#undef F
};

}  // Close unnamed namespace

// ============================================================================
//                             TEST MACHINERY
// ----------------------------------------------------------------------------

namespace {
namespace u {

class IncompleteType;
    // Used in verifying compiler-error behavior, must not be defined.

template <class T, int BASE>
void verifyOneBaseMaxLenForSignedInType(const char *int_type_name)
    // Verify correctness of the sufficient length reporting for a signed
    // integer type 'T' for the specified 'BASE'.  The specified
    // 'int_type_name' is used only to make assert messages more useful.  We
    // have a separate function for signed and unsigned types because the
    // longest (in print) value differs between the two.
{
    typedef std::numeric_limits<T> Lim;

    const size_t k_HUGE_BUFFSIZE = 1024;
        // We use huge buffer size as we have to assume errors may be present.
    char buffer[k_HUGE_BUFFSIZE + 1];
        // Plus one for a closing null character so we can print the buffer in
        // an assert without tricks.

    char * const p = Util::toChars(buffer,
                                   buffer + k_HUGE_BUFFSIZE,
                                   Lim::min(),
                                   BASE);
    const size_t lenOfMin = (p - buffer);
    ASSERTV(int_type_name, BASE, lenOfMin, lenOfMin <= k_HUGE_BUFFSIZE);
    if (lenOfMin > k_HUGE_BUFFSIZE) {
        return;                                                       // RETURN
    }

    *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'
    ASSERTV(int_type_name,
            BASE,
            (bslalg::NumericFormatterUtil::ToCharsMaxLength<T, BASE>::k_VALUE),
            lenOfMin,
            buffer,
            (Util::ToCharsMaxLength<T, BASE>::k_VALUE == lenOfMin));

    // Paranoid  heck to make sure length needed to print 'max()' is one or two
    // *less* than the length needed for the minimum, as for two's complement
    // it should be.  Two less when the negative value prints as '-1' followed
    // by zeros, like so: '"-10000"'.

    const bool atEdge = lenOfMin > 1
                     && ('1' == buffer[1])
                     && (lenOfMin - 2 == strspn(buffer + 2, "0"));

    const size_t minMaxLenDiff = atEdge ? 2 : 1;

    char * const xp = Util::toChars(buffer,
                                    buffer + k_HUGE_BUFFSIZE,
                                    Lim::max(),
                                    BASE);
    const size_t lenOfMax = (xp - buffer);
    ASSERTV(int_type_name, BASE, lenOfMax, lenOfMax <= k_HUGE_BUFFSIZE);
    if (lenOfMax > k_HUGE_BUFFSIZE) {
        return;                                                       // RETURN
    }

    *xp = '\0';  // Enable 'ASSERTV' printing of 'buffer'
    ASSERTV(int_type_name, BASE, lenOfMax, lenOfMin, buffer, minMaxLenDiff,
            lenOfMax + minMaxLenDiff == lenOfMin);
}

template <class T, int BASE>
void verifyOneBaseMaxLenForUnsignedInType(const char *int_type_name)
    // Verify correctness of the sufficient length reporting for an unsigned
    // integer type 'T' for the specified 'BASE'.  The specified
    // 'int_type_name' is used only to make assert messages more useful.  We
    // have a separate function for signed and unsigned types because the
    // longest (in print) value differs between the two.
{
    typedef std::numeric_limits<T> Lim;

    const size_t k_HUGE_BUFFSIZE = 1024;
        // We use huge buffer size as we have to assume errors may be present.
    char buffer[k_HUGE_BUFFSIZE + 1];
        // Plus one for a closing null character so we can print the buffer in
        // an assert without tricks.

    char * const p = Util::toChars(buffer,
                                   buffer + k_HUGE_BUFFSIZE,
                                   Lim::max(),
                                   BASE);
    const size_t lenOfMin = (p - buffer);
    ASSERTV(int_type_name, BASE, lenOfMin, lenOfMin <= k_HUGE_BUFFSIZE);
    if (lenOfMin > k_HUGE_BUFFSIZE) {
        return;                                                       // RETURN
    }

    *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'
    ASSERTV(int_type_name,
            BASE,
            (bslalg::NumericFormatterUtil::ToCharsMaxLength<T, BASE>::k_VALUE),
            lenOfMin,
            buffer,
            (Util::ToCharsMaxLength<T, BASE>::k_VALUE == lenOfMin));

    // Paranoid check to make sure length needed to print 'min()' is length 1.

    char * const np = Util::toChars(buffer,
                                    buffer + k_HUGE_BUFFSIZE,
                                    Lim::min(),
                                    BASE);  // Should print 0
    const size_t lenOfMax = (np - buffer);
    ASSERTV(int_type_name, BASE, lenOfMax, lenOfMax <= k_HUGE_BUFFSIZE);
    if (lenOfMax > k_HUGE_BUFFSIZE) {
        return;                                                       // RETURN
    }

    *np = '\0';  // Enable 'ASSERTV' printing of 'buffer'
    ASSERTV(int_type_name, BASE, lenOfMin, buffer,
            1 == lenOfMax);
}

template <class T>
void verifyAllMaxLenForAnIntType(const char *int_type_name)
{
    typedef std::numeric_limits<T> Lim;
    if (veryVerbose) {
        fputs(int_type_name, stdout);
        fputs(Lim::is_signed ? "is signed" : "is unsigned", stdout);
        printf(", sizeof: %u\n", (unsigned)sizeof(T));
    }

    if (Lim::is_signed) {
        verifyOneBaseMaxLenForSignedInType<T,  2>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T,  3>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T,  4>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T,  5>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T,  6>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T,  7>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T,  8>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T,  9>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 10>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 11>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 12>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 13>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 14>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 15>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 16>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 17>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 18>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 19>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 20>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 21>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 22>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 23>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 24>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 25>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 26>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 27>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 28>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 29>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 30>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 31>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 32>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 33>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 34>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 35>(int_type_name);
        verifyOneBaseMaxLenForSignedInType<T, 36>(int_type_name);
    }
    else {
        verifyOneBaseMaxLenForUnsignedInType<T,  2>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T,  3>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T,  4>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T,  5>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T,  6>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T,  7>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T,  8>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T,  9>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 10>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 11>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 12>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 13>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 14>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 15>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 16>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 17>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 18>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 19>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 20>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 21>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 22>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 23>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 24>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 25>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 26>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 27>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 28>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 29>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 30>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 31>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 32>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 33>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 34>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 35>(int_type_name);
        verifyOneBaseMaxLenForUnsignedInType<T, 36>(int_type_name);
    }
}

template <class UINT64_T>
void dblMantissaToHex(char (&hexout)[16], UINT64_T m52)
    // Write the specified 'm52' 52 bit mantissa to the specified 'hexout' as a
    // null terminated C-string.  The behavior is undefined unless 'm52' has no
    // bit set above bit 52, or in other words '0 == (~0xFFFFFFFFFFFFF & m52)'.
{
    BSLS_ASSERT(0 == (~0xFFFFFFFFFFFFFull & m52));

    const size_t prefixSize = 2;
    memcpy(hexout, "0x", prefixSize);

    char * const bb  = hexout + prefixSize;
    char * const be  = bb + sizeof hexout - prefixSize - 1;

    char * const end = Util::toChars(bb, be, m52, 16);

    BSLS_ASSERT(0 != end);
    BSLS_ASSERT(end <= be);

    const size_t slack = be - end;
    if (slack > 0) {
        memmove(bb + slack, bb, end - bb);
        memset(bb, '0', slack);
    }
    *be = '\0';
}

double dblPowerMult(Int64 n, unsigned exponent)
    // Return the specified 'n' multiplied by two to the power of the specified
    // 'exponent' as a 'double', or positive or negative infinity if that value
    // cannot be represented as a 'double'.
{
    double d = static_cast<double>(n);

    while (exponent >= 63) {
        d *= 0x8000000000000000ull;
        exponent -= 63;
    }

    return d * static_cast<double>(1ull << exponent);
}

float fltPowerMult(int n, unsigned exponent)
    // Return the specified 'n' multiplied by two to the power of the specified
    // 'exponent' as a 'float', or positive or negative infinity if that value
    // cannot be represented as a 'float'.
{
    float f = static_cast<float>(n);

    while (exponent >= 63) {
        f *= 0x8000000000000000ull;
        exponent -= 63;
    }
    return f * static_cast<float>(1ull << exponent);
}

void adjustScientificToCppStd(char *buffer)
    // Adjust the number in scientific format, written by Ryu 'd2s_buffered'
    // or 'f2s_buffered' functions, in the specified 'buffer' to the scientific
    // format specified by the C++ ISO standard for 'std::to_chars': exponent
    // always has a sign, and it always has at least two digits.  The behavior
    // is undefined if 'buffer' is not large enough to contain the longest
    // possible string and a closing null character (24 + 1).  The behavior is
    // also undefined unless 'buffer' contains a 'double' or 'float' number
    // written by Ryu 'd2s_buffered' or 'f2s_buffered' functions of a
    // supported Ryu version (namely one that writes 'E' before the exponent).
{
    char *ePtr = strrchr(buffer, 'E');
    BSLS_ASSERT(0 != ePtr);
    ++ePtr;
    sprintf(ePtr, "%+03d", atoi(ePtr));
}

static
size_t calcDecimalFmtLength(const char *expPtr, const char *fullNumber)
    // Calculate and return the decimal format character length of the
    // specified scientific 'fullNumber' with the help of the specified
    // 'expPtr' that points to the sign character of the non-zero exponent
    // inside 'fullNumber'.  The behavior is undefined unless 'ePtr' points to
    // the sign character of the non-zero exponent inside 'fullNumber' and
    // 'fullNumber' points to a scientific format floating point number that
    // has no preceding sign character unless it is negative.
{
    const size_t    negative = (*fullNumber == '-');
    const ptrdiff_t exponent = atoi(expPtr);
    ASSERT(0 != exponent); // Zero exponent would be written in decimal format

    const char *const dotPtr = strchr(fullNumber, '.');

    const char* const significandEnd = expPtr - 1;
    const size_t significandLength =
                              static_cast<size_t>(significandEnd - fullNumber);
    const size_t significandDigits = significandLength - (dotPtr ? 1 : 0);

    if (exponent > 0) {
        const size_t posExp = static_cast<size_t>(exponent);
        if (significandDigits - 1 - negative > posExp) { // Has decimals
            return significandLength;                                 // RETURN
        }
        else if (significandDigits - 1 - negative == posExp) { // No decimals
            return significandLength - 1;                             // RETURN
        }
        else { // Added trailing zeros (integral)
            return posExp + 1 + negative;                             // RETURN
        }
    }

    // exponent < 0
    const size_t absExp = static_cast<size_t>(-exponent);
    return absExp + significandLength + negative + (dotPtr ? 0 : 1);
}

bool allBytesAre(char byte, const char *buffer, size_t length)
    // Return 'true' if all characters in the specified 'buffer' of the
    // specified 'length' are the specified 'byte', and return 'false'
    // otherwise, when any of the bytes in the 'buffer' is not 'byte'.
{
    for (; length; --length, ++buffer) {
        if (*buffer != byte) {
            return false;                                             // RETURN
        }
    }
    return true;
}

bool anyByteIs(char byte, const char *buffer, size_t length)
    // Return 'true' if any characters in the specified 'buffer' of the
    // specified 'length' is the specified 'byte', and return 'false'
    // otherwise, when none of the bytes in the 'buffer' are 'byte'.
{
    for (; length; --length, ++buffer) {
        if (*buffer == byte) {
            return true;                                              // RETURN
        }
    }
    return false;
}

template <size_t BUFFER_SIZE>
inline
void prepareOutputBuffer(char (&toCharsBuffer)[BUFFER_SIZE])
{
    memset(toCharsBuffer, k_PREFILL_CHAR, BUFFER_SIZE);
}

template <size_t BUFFER_SIZE>
inline
bool expectedBytesHaveChanged(const char (&toCharsBuffer)[BUFFER_SIZE],
                              const char  *result)
    // Return 'true' if all unused bytes of the specified 'toCharsBuffer' are
    // 'k_PREFILL_CHAR', and neither of the used bytes, the bytes preceding the
    // specified 'result' if it is not 0, are 'k_PREFILL_CHAR'.  Otherwise
    // return 'false'.  If 'result' is 0, all bytes of 'toCharsBuffer' are
    // considered unused.  The behavior is undefined unless 'result' points
    // into 'toCharsBuffer' or it is a null pointer.
{
    if (0 == result) {
        return allBytesAre(k_PREFILL_CHAR,
                           toCharsBuffer,
                           BUFFER_SIZE);                              // RETURN
    }

    const size_t textSize = result - toCharsBuffer;
    const size_t restSize = BUFFER_SIZE - 1 - textSize;

    if (0 == restSize) {
        return !anyByteIs(k_PREFILL_CHAR, toCharsBuffer, textSize);   // RETURN
    }

    // We allow a closing zero character in the first unused position of the
    // buffer.  The buffer is expected to be one character larger than
    // 'toChars' was allowed to write!
    return (k_PREFILL_CHAR == *result || 0 == *result)
        && allBytesAre(k_PREFILL_CHAR, result + 1, restSize - 1)
        && !anyByteIs(k_PREFILL_CHAR, toCharsBuffer, textSize);
}

template <size_t BUFFER_SIZE, class VALUE_TYPE>
inline
void verifyEmptyOutput(long        LINE,
                       VALUE_TYPE  value,
                       int         base,
                       char      (&toCharsBuffer)[BUFFER_SIZE])
    // Generic helper for verification of an empty output buffer resulting in
    // an error and not changing a single byte of the output.
{
    // null output
    char *result = Util::toChars(0, 0, value, base);
    ASSERTV(LINE, (void *)result, 0 == result);

    // empty output at the beginning of the buffer
    u::prepareOutputBuffer(toCharsBuffer);
    result = Util::toChars(toCharsBuffer, toCharsBuffer, value, base);
    ASSERTV(LINE, (void *)result, (void*)toCharsBuffer, 0 == result);
    ASSERTV(LINE, u::expectedBytesHaveChanged(toCharsBuffer, result));
}

template <size_t BUFFER_SIZE, class VALUE_TYPE>
inline
void verifyEmptyOutput(long        LINE,
                       VALUE_TYPE  value,
                       char      (&toCharsBuffer)[BUFFER_SIZE])
    // Helper for verification of an empty output buffer resulting in
    // an error and not changing a single byte of the output.
{
    // null output
    char *result = Util::toChars(0, 0, value);
    ASSERTV(LINE, (void *)result, 0 == result);

    // empty output at the beginning of the buffer
    u::prepareOutputBuffer(toCharsBuffer);
    result = Util::toChars(toCharsBuffer, toCharsBuffer, value);
    ASSERTV(LINE, (void *)result, (void*)toCharsBuffer, 0 == result);
    ASSERTV(LINE, u::expectedBytesHaveChanged(toCharsBuffer, result));
}

template <size_t BUFFER_SIZE, class VALUE_TYPE>
inline
void verifyEmptyOutputFloatingPoint(long        LINE,
                                    VALUE_TYPE  value,
                                    char      (&buffer)[BUFFER_SIZE])
    // Helper for verification of an empty output buffer resulting in
    // an error and not changing a single byte of the output.
{
    // null output
    char *result = Util::toChars(0, 0, value);
    ASSERTV(LINE, (void *)result, 0 == result);
    result = Util::toChars(0, 0, value, Util::e_FIXED);
    ASSERTV(LINE, (void*)result, 0 == result);
    result = Util::toChars(0, 0, value, Util::e_SCIENTIFIC);
    ASSERTV(LINE, (void*)result, 0 == result);

    // empty output at the beginning of the specified buffer
    u::prepareOutputBuffer(buffer);
    result = Util::toChars(buffer, buffer, value);
    ASSERTV(LINE, (void *)result, (void*)buffer, 0 == result);
    ASSERTV(LINE, u::expectedBytesHaveChanged(buffer, result));

    u::prepareOutputBuffer(buffer);
    result = Util::toChars(buffer, buffer, value, Util::e_FIXED);
    ASSERTV(LINE, (void*)result, (void*)buffer, 0 == result);
    ASSERTV(LINE, u::expectedBytesHaveChanged(buffer, result));

    u::prepareOutputBuffer(buffer);
    result = Util::toChars(buffer, buffer, value, Util::e_SCIENTIFIC);
    ASSERTV(LINE, (void*)result, (void*)buffer, 0 == result);
    ASSERTV(LINE, u::expectedBytesHaveChanged(buffer, result));
}

template <size_t BUFFER_SIZE>
inline
void verifyEmptyOutput(long    LINE,
                       float   value,
                       char  (&buffer)[BUFFER_SIZE])
    // Helper for verification of an empty output buffer resulting in
    // an error and not changing a single byte of the output.
{
    return verifyEmptyOutputFloatingPoint(LINE, value, buffer);
}

template <size_t BUFFER_SIZE>
inline
void verifyEmptyOutput(long     LINE,
                       double   value,
                       char   (&buffer)[BUFFER_SIZE])
    // Helper for verification of an empty output buffer resulting in
    // an error and not changing a single byte of the output.
{
    return verifyEmptyOutputFloatingPoint(LINE, value, buffer);
}

template <class TOCHARS_CALL, class FLOAT_TYPE, size_t k_BSIZE>
void verifytoCharsTooSmallBufferFails(
                                   const int            LINE,
                                   const FLOAT_TYPE     VALUE,
                                   const size_t         EXPECTED_LENGTH,
                                   char               (&outputBuffer)[k_BSIZE])
{

                         // zero output length

    verifyEmptyOutput(LINE, VALUE, outputBuffer);
        // Verifies every possible format trying to write to nothing

                   // just too short output length

    prepareOutputBuffer(outputBuffer);
    char *result = TOCHARS_CALL::bslToChars(outputBuffer,
                                            outputBuffer + EXPECTED_LENGTH - 1,
                                            VALUE);
    ASSERTV(LINE, result, 0 == result);
    ASSERTV(LINE, expectedBytesHaveChanged(outputBuffer, result));
}

template <class TOCHARS_CALL, class FLOAT_TYPE, size_t k_BSIZE>
char *verifyToCharsSuccess(const int            LINE,
                           const FLOAT_TYPE     VALUE,
                           const char * const   EXPECTED,
                           const size_t         EXPECTED_LENGTH,
                           const size_t         bufferLengthToTest,
                           char               (&outputBuffer)[k_BSIZE])
{
    prepareOutputBuffer(outputBuffer);
    char *result = TOCHARS_CALL::bslToChars(outputBuffer,
                                            outputBuffer + bufferLengthToTest,
                                            VALUE);
    ASSERTV(LINE, EXPECTED_LENGTH, bufferLengthToTest, EXPECTED,
            0 != result);
    if (0 == result) { // Leave, most further checks may crash
        return 0;                                                     // RETURN
    }
    const size_t resultLength = result - outputBuffer;
    ASSERTV(LINE, result, outputBuffer + EXPECTED_LENGTH == result);
    ASSERTV(LINE, resultLength, EXPECTED_LENGTH == resultLength);
    if (EXPECTED_LENGTH != resultLength) {
        if (resultLength > k_BSIZE - 1) {
            // Very wrong length, no sense to check anything else
            return 0;                                                 // RETURN
        }
    }

    // Display the whole buffer if there was an overrun
    outputBuffer[k_BSIZE - 1] = 0;
    ASSERTV(LINE, outputBuffer,
            expectedBytesHaveChanged(outputBuffer, result));

    // Set the length of the string in the buffer to just the result
    *result = 0;
    ASSERTV(LINE, outputBuffer, EXPECTED,
            EXPECTED_LENGTH == resultLength &&
                         0 == memcmp(outputBuffer, EXPECTED, EXPECTED_LENGTH));

    return result;
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV)
template <class ORACLE_CALL, class FLOAT_TYPE, size_t k_BSIZE>
void compareToOracle(const int            LINE,
                     const FLOAT_TYPE     VALUE,
                     const char * const   EXPECTED,
                     const size_t         EXPECTED_LENGTH,
                     char               (&outputBuffer)[k_BSIZE],
                     char * const         result)
    // When exists, use 'std::to_char' as an oracle.  We are also verifying our
    // 'EXPECTED' results here as well against the 'std' oracle to ensure they
    // agree what the output should be.
{
    char stdBuf[k_BSIZE + 1];
    const auto tcr = ORACLE_CALL::stdToChars(std::begin(stdBuf),
                                             std::end(stdBuf),
                                             VALUE);
    ASSERTV(LINE, static_cast<int>(tcr.ec), tcr.ec == std::errc{});
    if (tcr.ec != std::errc{}) return;                              // CONTINUE

    *tcr.ptr = 0;  // Null terminate the 'std' result

    const size_t stdLen = tcr.ptr - stdBuf;
    const size_t bslLen = result - outputBuffer;
    ASSERTV(LINE, stdLen, bslLen, stdBuf, outputBuffer,
            stdLen == bslLen && 0 == memcmp(stdBuf, outputBuffer, stdLen));

    // Verify that our expectations do not differ from standard library
    // implementations.  If they do, we want to know about it, even if we
    // decide to suppress such test failures.
    if (stdLen != EXPECTED_LENGTH || 0 != memcmp(stdBuf, EXPECTED, stdLen)) {

#define ASSERT_PX_(EXPR, POSTFIX) BloombergLP::bsls::BslTestUtil::            \
                                      callDebugprint(EXPR, #EXPR ": ", POSTFIX)

#define ASSERT_P_(EXPR) ASSERT_PX_(EXPR, "\t")
#define ASSERT_P(EXPR)  ASSERT_PX_(EXPR, "\n")
#define ASSERT_MSG(MSG) aSsErT(true, MSG, __LINE__)

        // Poor man's 'LOOP5_ASSERT' with custom error message
        ASSERT_P_(LINE);
        ASSERT_P_(stdLen);
        ASSERT_P_(EXPECTED_LENGTH);
        ASSERT_P_(stdBuf);
        ASSERT_P(EXPECTED);
        ASSERT_MSG("'std::to_char' oracle and 'EXPECTED' test data disagree!");

#undef ASSERT_MSG
#undef ASSERT_P
#undef ASSERT_P_
#undef ASSERT_PX_
    }
}
#endif

template <class TOCHARS_CALL, class FLOAT_TYPE, size_t k_BSIZE>
char *verifyToCharsLine(const int            LINE,
                        const FLOAT_TYPE     VALUE,
                        const char * const   EXPECTED,
                        const size_t         EXPECTED_LENGTH,
                        char               (&outputBuffer)[k_BSIZE])
{
    verifytoCharsTooSmallBufferFails<TOCHARS_CALL>(LINE,
                                                  VALUE,
                                                  EXPECTED_LENGTH,
                                                  outputBuffer);

    char *result = verifyToCharsSuccess<TOCHARS_CALL>(LINE,
                                                      VALUE,
                                                      EXPECTED,
                                                      EXPECTED_LENGTH,
                                                      EXPECTED_LENGTH,
                                                      outputBuffer);
    if (0 == result) return 0;                                        // RETURN

              // Oracle Test with 'std::to_chars' when available

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV)
    compareToOracle<TOCHARS_CALL>(LINE,
                                  VALUE,
                                  EXPECTED,
                                  EXPECTED_LENGTH,
                                  outputBuffer,
                                  result);
#endif

    return verifyToCharsSuccess<TOCHARS_CALL>(LINE,
                                              VALUE,
                                              EXPECTED,
                                              EXPECTED_LENGTH,
                                              k_BSIZE - 1,
                                              outputBuffer);
}

template <class FLOAT_TYPE,
          class TOCHARS_CALL,
          class TEST_DATA_TYPE,
          size_t k_NUM_DATA>
void verifyFloatingPointToChars(const TEST_DATA_TYPE (&k_DATA)[k_NUM_DATA])
{
    BSLMF_ASSERT(sizeof(FLOAT_TYPE) == 8 || sizeof(FLOAT_TYPE) == 4);

    for (size_t i = 0; i < k_NUM_DATA; ++i) {
        const int         LINE       = k_DATA[i].d_line;
        const FLOAT_TYPE  VALUE      = k_DATA[i].d_value;
        const char       *EXPECTED   = k_DATA[i].d_expected;
        const size_t EXPECTED_LENGTH = strlen(EXPECTED);

        if (veryVeryVerbose) {
            P_(LINE) P(EXPECTED);
        }

        const int k_MAX_BUF_SIZE = sizeof(FLOAT_TYPE) == 4
                                   ? TOCHARS_CALL::k_FLT_MAX_BUF_SIZE
                                   : TOCHARS_CALL::k_DBL_MAX_BUF_SIZE;
        char      outputBuffer[k_MAX_BUF_SIZE + 1];

        char *result = verifyToCharsLine<TOCHARS_CALL>(LINE,
                                                       VALUE,
                                                       EXPECTED,
                                                       EXPECTED_LENGTH,
                                                       outputBuffer);
        if (0 == result) continue;                                  // CONTINUE

        if ('-' != *EXPECTED) {
            // For non-negative numbers we automatically verify the
            // negative variation.

            char NEGATIVE_EXPECTED[k_MAX_BUF_SIZE + 1];
            NEGATIVE_EXPECTED[0] = '-';
            strcpy(NEGATIVE_EXPECTED + 1, EXPECTED);

            verifyToCharsLine<TOCHARS_CALL>(LINE,
                                            -VALUE,
                                            NEGATIVE_EXPECTED,
                                            EXPECTED_LENGTH + 1,
                                            outputBuffer);
        }
    }
}
                       // ===========================
                       // struct ScientificBufferSize
                       // ===========================

struct ScientificBufferSize {
    static const size_t k_FLT_MAX_BUF_SIZE = 15;
    static const size_t k_DBL_MAX_BUF_SIZE = 24;
};
                        // =========================
                        // struct OracleCallNoFormat
                        // =========================

struct OracleCallNoFormat {
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV)
    template <class VALUE_TYPE>
    static std::to_chars_result stdToChars(char* s, char* e, VALUE_TYPE v) {
        return std::to_chars(s, e, v);
    }
#endif
};
                        // ==========================
                        // struct ToCharsCallNoFormat
                        // ==========================

struct ToCharsCallNoFormat : OracleCallNoFormat, ScientificBufferSize {
    template <class VALUE_TYPE>
    static char *bslToChars(char* s, char* e, VALUE_TYPE v) {
        return Util::toChars(s, e, v);
    }
};
                       // ===========================
                       // struct OracleCallScientific
                       // ===========================

struct OracleCallScientific {
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV)
    template <class VALUE_TYPE>
    static std::to_chars_result stdToChars(char* s, char* e, VALUE_TYPE v) {
        return std::to_chars(s, e, v, std::chars_format::scientific);
    }
#endif
};
                       // ===========================
                       // struct ToCharsCallScientific
                       // ===========================

struct ToCharsCallScientific : OracleCallScientific, ScientificBufferSize {
    template <class VALUE_TYPE>
    static char* bslToChars(char* s, char* e, VALUE_TYPE v) {
        return Util::toChars(s, e, v, Util::e_SCIENTIFIC);
    }
};
                        // ========================
                        // struct DecimalBufferSize
                        // ========================

struct DecimalBufferSize {
    static const size_t k_FLT_MAX_BUF_SIZE = 48;
    static const size_t k_DBL_MAX_BUF_SIZE = 327;
};
                        // ========================
                        // struct OracleCallDecimal
                        // ========================

struct OracleCallDecimal {
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV)
    template <class VALUE_TYPE>
    static std::to_chars_result stdToChars(char* s, char* e, VALUE_TYPE v) {
        return std::to_chars(s, e, v, std::chars_format::fixed);
    }
#endif
};

                        // =========================
                        // struct ToCharsCallDecimal
                        // =========================

struct ToCharsCallDecimal : OracleCallDecimal, DecimalBufferSize {
    template <class VALUE_TYPE>
    static char* bslToChars(char* s, char* e, VALUE_TYPE v) {
        return Util::toChars(s, e, v, Util::e_FIXED);
    }
};

                        // Ryu Call Testing

template <class RYU_CALL, class FLOAT_TYPE, size_t k_BSIZE>
int verifyRyuCallLine(const int            LINE,
                      const FLOAT_TYPE     VALUE,
                      const char * const   EXPECTED,
                      const size_t         EXPECTED_LENGTH,
                      char               (&output)[k_BSIZE])
{
    prepareOutputBuffer(output);
    const int size = RYU_CALL::callRyu(VALUE, output);
    ASSERTV(LINE, size, size > 0);
    const size_t usize = static_cast<size_t>(size);

    ASSERTV(LINE, usize, EXPECTED, usize == EXPECTED_LENGTH);
    if (usize != EXPECTED_LENGTH) {
        if (usize < k_BSIZE - 1) {
            output[usize] = 0;
        }
        else {
            // Very wrong length, no sense to check anything else
            return 0;                                                 // RETURN
        }
    }
    output[usize] = 0;
    ASSERTV(LINE, expectedBytesHaveChanged(output, output + usize));
    ASSERTV(LINE, output, size, EXPECTED,
            usize == EXPECTED_LENGTH && 0 == memcmp(output, EXPECTED, usize));

    RYU_CALL::extraChecks(LINE,
                          VALUE,
                          EXPECTED,
                          EXPECTED_LENGTH,
                          output,
                          size);

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV)
    compareToOracle<RYU_CALL>(LINE,
                              VALUE,
                              EXPECTED,
                              EXPECTED_LENGTH,
                              output,
                              output + size);
#endif
    return size;
}

template <class FLOAT_TYPE,
          class RYU_CALL,
          class TEST_DATA_TYPE,
          size_t k_NUM_DATA>
void verifyRyuCall(const TEST_DATA_TYPE (&k_DATA)[k_NUM_DATA])
{
    BSLMF_ASSERT(sizeof(FLOAT_TYPE) == 8 || sizeof(FLOAT_TYPE) == 4);

    for (size_t i = 0; i < k_NUM_DATA; ++i) {
        const int         LINE            = k_DATA[i].d_line;
        const FLOAT_TYPE  VALUE           = k_DATA[i].d_value;
        const char       *EXPECTED        = k_DATA[i].d_expected;
        const size_t      EXPECTED_LENGTH = strlen(EXPECTED);

        if (veryVeryVerbose) {
            P_(LINE) P(EXPECTED);
        }

        const int k_MAX_BUF_SIZE = sizeof(FLOAT_TYPE) == 4
                                   ? RYU_CALL::k_FLT_MAX_BUF_SIZE
                                   : RYU_CALL::k_DBL_MAX_BUF_SIZE;
        char      output[k_MAX_BUF_SIZE + 1];

        const int size = verifyRyuCallLine<RYU_CALL>(LINE,
                                                     VALUE,
                                                     EXPECTED,
                                                     EXPECTED_LENGTH,
                                                     output);

        // Skip unrecoverable errors (way too large size reported).
        if (0 == size) continue;                                    // CONTINUE

        if ('-' != *EXPECTED) {
            // For non-negative numbers we automatically verify the
            // negative variation.

            char NEGATIVE_EXPECTED[k_MAX_BUF_SIZE + 1];
            NEGATIVE_EXPECTED[0] = '-';
            BSLS_ASSERT_OPT(strlen(EXPECTED) <= k_MAX_BUF_SIZE);
            strcpy(NEGATIVE_EXPECTED + 1, EXPECTED);

            verifyRyuCallLine<RYU_CALL>(LINE,
                                        -VALUE,
                                        NEGATIVE_EXPECTED,
                                        EXPECTED_LENGTH + 1,
                                        output);
        }
    }
}

                        // ===================
                        // template struct Flt
                        // ===================

template <class FLOAT_TYPE>
struct Flt;
    // Selector between 'float' and 'double' dependent types and functions.

template <>
struct Flt<double> {
    typedef Int64 IntType;
    static double powerMult(IntType n, unsigned exponent) {
        return dblPowerMult(n, exponent);
    }
};

template <>
struct Flt<float> {
    typedef int IntType;
    static float powerMult(IntType n, unsigned exponent) {
        return fltPowerMult(n, exponent);
    }
};

template <class FLOAT_TYPE,
          class RYU_CALL,
          class TEST_DATA_TYPE,
          size_t k_NUM_DATA_INT>
void verifyRyuCallInt(const TEST_DATA_TYPE (&k_DATA_INT)[k_NUM_DATA_INT])
{
    BSLMF_ASSERT(sizeof(FLOAT_TYPE) == 8 || sizeof(FLOAT_TYPE) == 4);

    for (size_t i = 0; i < k_NUM_DATA_INT; ++i) {
        typedef Flt<FLOAT_TYPE>       Flt;
        typedef typename  Flt::IntType IntType;

        const int         LINE            = k_DATA_INT[i].d_line;
        const IntType     INTEGER         = k_DATA_INT[i].d_integer;
        const unsigned    EXPONENT        = k_DATA_INT[i].d_exponent;
        const FLOAT_TYPE  VALUE           = Flt::powerMult(INTEGER, EXPONENT);
        const char       *EXPECTED        = k_DATA_INT[i].d_expected;
        const size_t      EXPECTED_LENGTH = strlen(EXPECTED);

        if (veryVeryVerbose) {
            P_(LINE) P_(INTEGER); P_(EXPONENT); P_(VALUE) P(EXPECTED);
        }

        const int k_MAX_BUF_SIZE = sizeof(FLOAT_TYPE) == 4
                                                ? RYU_CALL::k_FLT_MAX_BUF_SIZE
                                                : RYU_CALL::k_DBL_MAX_BUF_SIZE;
        char      output[k_MAX_BUF_SIZE + 1];

        const int size = verifyRyuCallLine<RYU_CALL>(LINE,
                                                     VALUE,
                                                     EXPECTED,
                                                     EXPECTED_LENGTH,
                                                     output);

        // Skip unrecoverable errors (way too large size reported).
        if (0 == size) continue;                                    // CONTINUE


        if ('-' != *EXPECTED) {
            // For non-negative numbers we automatically verify the
            // negative variation.

            char NEGATIVE_EXPECTED[k_MAX_BUF_SIZE + 1];
            NEGATIVE_EXPECTED[0] = '-';
            strcpy(NEGATIVE_EXPECTED + 1, EXPECTED);

            verifyRyuCallLine<RYU_CALL>(LINE,
                                        -VALUE,
                                        NEGATIVE_EXPECTED,
                                        EXPECTED_LENGTH + 1,
                                        output);
        }
    }
}

                        // =====================
                        // struct RyuCallMinimal
                        // =====================

struct RyuCallMinimal : OracleCallNoFormat, ScientificBufferSize {
    static int callRyu(float v, char *o) {
        return blp_f2m_buffered_n(v, o);
    }

    static int callRyu(double v, char* o) {
        return blp_d2m_buffered_n(v, o);
    }

    static void callRyu_OriginalScientific(float v, char* o) {
        ryu_f2s_buffered(v, o);
    }

    static void callRyu_OriginalScientific(double v, char* o) {
        ryu_d2s_buffered(v, o);
    }

    template <class FLOAT_TYPE, size_t k_BSIZE>
    static void extraChecks(const int            LINE,
                            const FLOAT_TYPE     VALUE,
                            const char * const   EXPECTED,
                            const size_t         EXPECTED_LENGTH,
                            char               (&output)[k_BSIZE],
                            const size_t         size)
    {
        (void)EXPECTED_LENGTH;
        (void)output;

        // Were we supposed to write scientific format?
        const char *ePtr = strrchr(EXPECTED, 'E');

        // Verifying that our format choice assumption is correct (testing
        // the test table, in a way).
        if (0 == ePtr) {
            // Verify that scientific output would be equal or longer in
            // length than the generic output
            char scientific[k_BSIZE];
            callRyu_OriginalScientific(VALUE, scientific);
            adjustScientificToCppStd(scientific);

            ASSERTV(LINE, scientific, size, size <= strlen(scientific));
        }
        else {
            // We do not have a decimal converter that writes the minimum
            // number of digits so we have to calculate the would-be
            // decimal length from the actual scientific output.

            const size_t decimalSize = calcDecimalFmtLength(++ePtr, EXPECTED);
            ASSERTV(LINE, size, decimalSize, size < decimalSize);
        }
    }
};

                        // ========================
                        // struct RyuCallScientific
                        // ========================

struct RyuCallScientific : OracleCallScientific, ScientificBufferSize {
    static int callRyu(float v, char* o) {
        return blp_f2s_buffered_n(v, o);
    }

    static int callRyu(double v, char* o) {
        return blp_d2s_buffered_n(v, o);
    }

    template <class FLOAT_TYPE, size_t k_BSIZE>
    static void extraChecks(const int            LINE,
                            const FLOAT_TYPE     VALUE,
                            const char * const   EXPECTED,
                            const size_t         EXPECTED_LENGTH,
                            char               (&output)[k_BSIZE],
                            const size_t         size)
    {
        (void)LINE;
        (void)VALUE;
        (void)EXPECTED;
        (void)EXPECTED_LENGTH;
        (void)output;
        (void)size;
    }
};

                            // =====================
                            // struct RyuCallDecimal
                            // =====================

struct RyuCallDecimal : OracleCallDecimal, DecimalBufferSize {
    static int callRyu(float v, char* o) {
        return blp_f2d_buffered_n(v, o);
    }

    static int callRyu(double v, char* o) {
        return blp_d2d_buffered_n(v, o);
    }

    template <class FLOAT_TYPE, size_t k_BSIZE>
    static void extraChecks(const int            LINE,
                            const FLOAT_TYPE     VALUE,
                            const char * const   EXPECTED,
                            const size_t         EXPECTED_LENGTH,
                            char               (&output)[k_BSIZE],
                            const size_t         size)
    {
        (void)LINE;
        (void)VALUE;
        (void)EXPECTED;
        (void)EXPECTED_LENGTH;
        (void)output;
        (void)size;
    }
};

template <class FLOAT_TYPE, class RYU_CALL>
void verifyRyuNonNumericFloatingPointValues()
{
    typedef std::numeric_limits<FLOAT_TYPE> lim;

#ifndef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD     // available
    // Yet another Sun "anomaly"
    BSLMF_ASSERT( true == lim::is_iec559);
#endif
    BSLMF_ASSERT(lim::has_infinity);
    BSLMF_ASSERT(lim::has_quiet_NaN);
    BSLMF_ASSERT(lim::has_signaling_NaN);
    BSLMF_ASSERT(8 == sizeof(double));
    BSLMF_ASSERT(4 == sizeof(float));

    static const struct {
        int         d_line;
        FLOAT_TYPE  d_value;
        const char* d_expected;
    } k_DATA[] = {

    #define TL(sign, func, expected) { L_, sign lim::func(), expected }

        TL(+, infinity,      "+INF"),
        TL(-, infinity,      "-INF"),
        TL(+, quiet_NaN,      "NaN"),
        TL(+, signaling_NaN,  "NaN"),
        TL(-, quiet_NaN,      "NaN"),
        TL(-, signaling_NaN,  "NaN")

    #undef TL
    };

    const size_t k_NUM_DATA = sizeof k_DATA / sizeof *k_DATA;

    for (size_t i = 0; i < k_NUM_DATA; ++i) {
        const int         LINE            = k_DATA[i].d_line;
        const FLOAT_TYPE  VALUE           = k_DATA[i].d_value;
        const char       *EXPECTED        = k_DATA[i].d_expected;
        const size_t      EXPECTED_LENGTH = strlen(EXPECTED);

        if (veryVeryVerbose) {
            P_(LINE) P_(VALUE); P(EXPECTED);
        }

        const int k_MAX_BUF_SIZE = 4;
        char      output[k_MAX_BUF_SIZE + 1];

        prepareOutputBuffer(output);
        const int size = RYU_CALL::callRyu(VALUE, output);
        ASSERTV(LINE, size, size > 0);
        const size_t usize = static_cast<size_t>(size);

        ASSERTV(LINE, usize, EXPECTED, usize == EXPECTED_LENGTH);
        if (usize != EXPECTED_LENGTH) {
            if (usize < k_MAX_BUF_SIZE) {
                output[usize] = 0;
            }
            else {
                // Very wrong length, no sense to check anything else
                continue;                                           // CONTINUE
            }
        }
        output[usize] = 0;
        ASSERTV(LINE, expectedBytesHaveChanged(output, output + usize));
        ASSERTV(LINE, output, size, EXPECTED,
                usize == EXPECTED_LENGTH &&
                                         0 == memcmp(output, EXPECTED, usize));

        RYU_CALL::extraChecks(LINE,
                              VALUE,
                              EXPECTED,
                              EXPECTED_LENGTH,
                              output,
                              size);
    }
}

template <class FLOAT_TYPE, class TOCHARS_CALL>
void verifyToCharsNonNumericFloatingPointValues()
{
    typedef std::numeric_limits<FLOAT_TYPE> lim;

#ifndef BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD     // available
    // Yet another Sun "anomaly"
    BSLMF_ASSERT( true == lim::is_iec559);
#endif
    BSLMF_ASSERT(lim::has_infinity);
    BSLMF_ASSERT(lim::has_quiet_NaN);
    BSLMF_ASSERT(lim::has_signaling_NaN);
    BSLMF_ASSERT(8 == sizeof(double));
    BSLMF_ASSERT(4 == sizeof(float));

    static const struct {
        int         d_line;
        FLOAT_TYPE  d_value;
        const char* d_expected;
    } k_DATA[] = {

    #define TL(sign, func, expected) { L_, sign lim::func(), expected }

        TL(+, infinity,      "+INF"),
        TL(-, infinity,      "-INF"),
        TL(+, quiet_NaN,      "NaN"),
        TL(+, signaling_NaN,  "NaN"),
        TL(-, quiet_NaN,      "NaN"),
        TL(-, signaling_NaN,  "NaN")

    #undef TL
    };

    const size_t k_NUM_DATA = sizeof k_DATA / sizeof *k_DATA;

    for (size_t i = 0; i < k_NUM_DATA; ++i) {
        const int         LINE            = k_DATA[i].d_line;
        const FLOAT_TYPE  VALUE           = k_DATA[i].d_value;
        const char       *EXPECTED        = k_DATA[i].d_expected;
        const size_t      EXPECTED_LENGTH = strlen(EXPECTED);

        if (veryVeryVerbose) {
            P_(LINE) P_(VALUE); P(EXPECTED);
        }

        const int k_MAX_BUF_SIZE = 4;
        char      outputBuffer[k_MAX_BUF_SIZE + 1];

        verifytoCharsTooSmallBufferFails<TOCHARS_CALL>(LINE,
                                                       VALUE,
                                                       EXPECTED_LENGTH,
                                                       outputBuffer);

        verifyToCharsSuccess<TOCHARS_CALL>(LINE,
                                           VALUE,
                                           EXPECTED,
                                           EXPECTED_LENGTH,
                                           EXPECTED_LENGTH,
                                           outputBuffer);

        // There is no Oracle comparison because in C++ the text for
        // non-numerical values is not standardized

        verifyToCharsSuccess<TOCHARS_CALL>(LINE,
                                           VALUE,
                                           EXPECTED,
                                           EXPECTED_LENGTH,
                                           k_MAX_BUF_SIZE,
                                           outputBuffer);
    }
}

char *generateIntStringRep(char *back, Uint64 value, bool sign, unsigned base)
    // Write a string representation of the specified integer 'value' using the
    // specified 'base', with the specified 'sign' if the 'value' is to be
    // negated, to the buffer ending at the specified 'back'.  Return a pointer
    // to the beginning of the written value.
{
    BSLS_ASSERT(value || !sign);

    do {
        unsigned digit = static_cast<unsigned>(value % base);
        *--back = static_cast<char>(digit < 10 ? '0' + digit
                                               : 'a' + (digit - 10));
        value /= base;
    } while (0 < value);

    if (sign) {
        *--back = '-';
    }

    return back;
}

                        // =======================
                        // struct NumDecimalDigits
                        // =======================

template <int NUMBER, class = void>
struct NumDecimalDigits {
    static const size_t value = 1 + NumDecimalDigits<NUMBER / 10>::value;
};

template <int NUMBER>
struct NumDecimalDigits<NUMBER, typename bslmf::EnableIf<NUMBER < 10>::type> {
    static const size_t value = 1;
};


int parseInt(Uint64     *result,
             const char *first,
             const char *last,
             unsigned    base)
    // Parse as a number the string specified by '[ first, last )' in the
    // specified 'base', setting '*result' to the value represented by the
    // string.  Negative numbers can be signified by '-' which must be the
    // value of '*first'.  Fail an 'ASSERT' and return a negative number on
    // error, and return +1 if the value represented can't be stored in an
    // 'Int64'.
{
    BSLS_ASSERT(2 <= base);
    BSLS_ASSERT(base <= 36);

    const Uint64 maxU = numeric_limits<Uint64>::max();
    const Uint64 maxI = numeric_limits<Int64 >::max();
    Uint64 tmpResult = 0;

    bool isMinI     = false;
    bool passesMinI = false;

    for (Uint64 radix = 1, prevRadix = 0; first < last--;
                                            prevRadix = radix, radix *= base) {
        const char c = *last;

        if (radix <= prevRadix && '-' != c) {
            ASSERT(0 && "number too long, radix wrapped");

            return -1;                                                // RETURN
        }

        unsigned digit;
        if      ('0' <= c && c <= '9') {
            digit = c - '0';
        }
        else if ('a' <= c && c <= 'z') {
            digit = 10 + c - 'a';
        }
        else if ('-' == c) {
            *result = ~tmpResult + 1;

            if (first != last) {
                ASSERTV(last - first, first == last);

                return -1;                                            // RETURN
            }

            if (passesMinI) {
                ASSERT(passesMinI);

                return -1;                                            // RETURN
            }

            return 0;                                                 // RETURN
        }
        else {
            ASSERTV(c, static_cast<int>(c), 0);

            return -1;                                                // RETURN
        }

        if (base <= digit) {
            ASSERTV(digit, base, digit < base);

            return -1;                                                // RETURN
        }

        if (maxU / radix < digit) {
            ASSERTV(digit, radix, digit <= maxU / radix);

            return -1;                                                // RETURN
        }
        if (maxU - tmpResult < digit * radix) {
            ASSERTV(digit, radix, maxU, tmpResult,
                                            digit * radix <= maxU - tmpResult);

            return -1;                                                // RETURN
        }

        if (maxI / radix < digit) {
            if ((maxI + 1) / radix == digit) {
                isMinI = true;
            }
            else {
                passesMinI = true;
            }
        }
        if (maxI - tmpResult < digit * radix) {
            if ((maxI + 1) - tmpResult == digit * radix) {
                isMinI = true;
            }
            else {
                passesMinI = true;
            }
        }

        tmpResult += digit * radix;
    }

    *result = tmpResult;

    return isMinI || passesMinI ? 1 : 0;
}

Uint64 mmixRand64(bool reset = false)
    // MMIX Linear Congruential Generator algorithm by Donald Knuth (modified).
    // Optionally specified 'reset' which, if 'true', indicates that the
    // accumulator is to be reset to zero.
{
    static Uint64 randAccum = 0;
    if (reset) {
        randAccum = 0;
    }

    enum { k_MASK = ~static_cast<unsigned>(0) };

    static const Uint64 a    = 6364136223846793005ULL;
    static const Uint64 c    = 1442695040888963407ULL;
    static const Uint64 mask = ((1ULL << 32) - 1) << 32;

    BSLMF_ASSERT(0 != mask);
    BSLMF_ASSERT(0 == (~static_cast<unsigned>(0) & mask));
    BSLMF_ASSERT(static_cast<Int64>(mask) < 0);

    randAccum = randAccum * a + c;
    Uint64 hi = randAccum & mask;
    randAccum = randAccum * a + c;

    return hi | (randAccum >> 32);
}

template <class TYPE>
void randIntTest(Uint64 iterationsForByte)
    // Test the function under test for a number of iterations specified by
    // 'iterationsForByte', passing randomly-generated values of the specified
    // 'TYPE' to the function, where the actual number of iterations will be
    // scaled by 2 to the power of 'sizeof(TYPE)'.  The random number generator
    // is a modified form of Knuth's 'MMIX' algorithm.  Errors are detected and
    // reported via 'ASSERT' and 'ASSERTV'.  Each randomly generated value is
    // tested with all bases in the range '[ 2 .. 36 ]'.
    //
    // Unlike the function 'testIntValue' above, we are not just doing corner
    // cases so more effort is made here to test as many values as quickly as
    // possible.
{
    const char *name = bsls::NameOf<TYPE>().name();

    static const char starBuffer[] = {
                        "**************************************************"
                        "**************************************************" };
    BSLMF_ASSERT(sizeof(starBuffer) == 101);

    const Uint64 iterations = iterationsForByte << sizeof(TYPE);

    mmixRand64(true);

    Uint64 mask = ~static_cast<Uint64>(0);
    if (sizeof(TYPE) < sizeof(Uint64)) {
        // Microsoft compilers freak out about shifts by 64 in here even though
        // they're impossible, so make them more impossible.

        unsigned shift = sizeof(TYPE) * 8;
        shift = sizeof(TYPE) < sizeof(Uint64) ? shift : 63;
        mask = (1ULL << shift) - 1;
    }
    const Uint64 hiMask = ~mask;

    if (veryVerbose) {
        printf("randIntTest: type: %s mask: 0x%llx\n", name, mask);
    }

    for (Uint64 ii = 0; ii < iterations; ++ii) {
        Uint64     bigNum = mmixRand64() & mask;
        const TYPE value  = static_cast<TYPE>(bigNum);

        const bool sign = value < 0;
        if (sign) {
            // make 'bigNum' the absolute value of 'value'

            // in the imp, we just assign 'value' to a 'Uint64' and believe
            // that it will sign extend, and we negate a 'Uint64' by compliment
            // and add 1.  Here we do it through a different maneuver to test
            // that we get the same result.

            bigNum |= hiMask;                        // sign extend
            ASSERT(static_cast<Int64>(bigNum) == static_cast<Int64>(value));

            bigNum = -static_cast<Int64>(bigNum);    // negate
        }

        for (unsigned base = 2; base <= 36; ++base) {
            char tBuffer[100], gBuffer[100];
            char *result;

            char *rear = gBuffer + 99;
            *rear = 0;
            char *front = u::generateIntStringRep(rear, bigNum, sign, base);
            const unsigned len = static_cast<unsigned>(rear - front);
            ASSERTV(len, len <= 64U + sign);

            Uint64 cmpResult;
            int rc = u::parseInt(&cmpResult, front, rear, base);
            ASSERT(0 <= rc);
            if (sign) {
                ASSERT(0 == rc);
                const Int64 iCmpResult = cmpResult;
                ASSERT(iCmpResult < 0);

                const bool test = iCmpResult == static_cast<Int64>(value);
                if (!test) { P_(name); P_(bigNum); P_(sign); P_(iCmpResult);
                             P_(front); P_(base);  P_(value); P(cmpResult); }
                ASSERT(test);
            }
            else {
                ASSERT(0 <= value);

                ASSERTV(value, bigNum, sign, cmpResult,
                                      cmpResult == static_cast<Uint64>(value));
            }

            // 'addLen' will be a random number in the range '[ 0 .. 2 ]'.  If
            // it's 0 (25% chance) there won't be enough room for the result.

            unsigned addLen = static_cast<unsigned>(mmixRand64()) & 3;
            addLen = std::min(addLen, 2U);

            char *tail = tBuffer + std::max<int>(len - 1 + addLen, 1);
            for (; tail <= tBuffer + len; ++tail) {
                memset(tBuffer, '*', tail - tBuffer);

                result = Util::toChars(tBuffer, tail, value, base);
                if (tail < tBuffer + len) {
                    if (veryVeryVeryVerbose) puts("too short");

                    ASSERTV(name, value, 0 == result);
                    ASSERTV(name, 0 == std::memcmp(tBuffer + sign,
                                                   starBuffer,
                                                   tail - sign - tBuffer));
                }
                else {
                    if (veryVeryVeryVerbose) puts("long enough");

                    ASSERTV(name, bigNum, tail - tBuffer, result);
                    ASSERTV(name, value, tBuffer + len == result);
                    ASSERTV(name, value, 0 == memcmp(front, tBuffer, len));
                    ASSERTV(name, value, 0 == memcmp(result,
                                                     starBuffer,
                                                     tail - (tBuffer + len)));

                    if (bigNum == 0) {
                        ASSERT(1 == len);
                        ASSERT('0' == *tBuffer);
                    }
                    else {
                        if (sign) {
                            ASSERTV(name, value, '-' == *tBuffer);
                            ASSERTV(name, value, '0' != tBuffer[1]);
                        }
                        else {
                            ASSERTV(name, value, '-' != *tBuffer);
                            ASSERTV(name, value, '0' != *tBuffer);
                        }
                    }
                }
            }
        }
    }
}

template <class TYPE>
void testIntValue(const Uint64 numArg, unsigned base)
    // Assign the specified 'numArg', and, in the case of signed types, the
    // negation of 'numArg', to a variable of type 'TYPE', and test the
    // function under test on it in the specified 'base'.  The testing is
    // performed by checks with the 'ASSERT' and 'ASSERTV' macros, which will
    // produce traces and cause the program to return a non-zero value
    // (indicating failure) if the expressions evaluated by the macros evaluate
    // 'false'.  This function returns without doing any checks if 'numArg' is
    // too large to be represented in a 'TYPE' variable.  This function uses
    // 'u::generateIntStringRep' as an oracle function whose result is compared
    // to the functions under test, and uses 'u::parseInt' as an inverse oracle
    // function.  The function under test is called only with 'base'.  For
    // bases 8, 10, and 16, 'sprintf' is also used as a redundant oracle.  If
    // 'std::to_chars' is available, it is also used as a redundant (but highly
    // reliable) oracle.
{
    const char *name = bsls::NameOf<TYPE>().name();

    const bool signedType = static_cast<TYPE>(-1) < 0;

    if (signedType) {
        Uint64 flipMin = static_cast<Uint64>(numeric_limits<TYPE>::min());
        flipMin = ~flipMin + 1;
        if (flipMin < numArg) {
            return;                                                   // RETURN
        }
    }
    else if (static_cast<Uint64>(numeric_limits<TYPE>::max()) < numArg) {
        return;                                                       // RETURN
    }

    for (int ii = 0; ii < 2; ++ii) {
        bool flip = signedType && !ii && numArg;

        TYPE num = static_cast<TYPE>(flip ? ~numArg + 1
                                          :  numArg);

        static const char starBuffer[] = {
                        "**************************************************"
                        "**************************************************" };

        char tBuffer[100], gBuffer[100];
        char *result;

        char *rear = gBuffer + 99;
        *rear = 0;
        char *front = u::generateIntStringRep(rear, numArg, flip, base);
        const size_t len = rear - front;
        ASSERTV(len, len <= 64U + flip);

        if (10 == base || (!flip && (8 == base || 16 == base))) {
            char sBuffer[100];

            if (flip) {
                Int64 signedArg = numArg;
                signedArg = -signedArg;

                sprintf(sBuffer, "%lld", signedArg);
            }
            else {
                const char *fmt = 10 == base
                                ? "%llu"
                                : 8 == base
                                ? "%llo"
                                : "%llx";
                sprintf(sBuffer, fmt, numArg);
            }

            ASSERT(std::strlen(sBuffer) == len);
            ASSERT(!std::strcmp(sBuffer, front));
            ASSERT(!std::memcmp(sBuffer, front, len));
        }

        Uint64 cmpResult;
        int rc = u::parseInt(&cmpResult, front, rear, base);
        ASSERT(0 <= rc);
        if (flip) {
            ASSERT(0 == rc);
            const Int64 iCmpResult = cmpResult;

            const bool test = iCmpResult == static_cast<Int64>(num);
            if (!test) { P_(name); P_(numArg); P_(flip); P_(iCmpResult);
                         P_(front); P_(base);  P_(num); P(cmpResult); }
            ASSERT(test);
        }
        else {
            ASSERT(0 <= num);

            ASSERTV(num, numArg, flip, cmpResult,
                                        cmpResult == static_cast<Uint64>(num));
        }

        u::verifyEmptyOutput(L_, num, tBuffer);

        for (char *tail = std::max(tBuffer + 1, tBuffer + len - 2);
                                            tail < tBuffer + len + 2; ++tail) {
            memset(tBuffer, '*', tail - tBuffer);

            result = Util::toChars(tBuffer, tail, num, base);
            if (tail < tBuffer + len) {
                ASSERTV(name, num, 0 == result);
                ASSERTV(name, 0 == std::memcmp(tBuffer + flip,
                                               starBuffer,
                                               tail - flip - tBuffer));
            }
            else {
                ASSERTV(name, numArg, flip, tail - tBuffer, 0 != result);
                if (0 == result) continue;                          // CONTINUE
                ASSERTV(name, num, len, base, result - tBuffer,
                                                      tBuffer + len == result);
                ASSERTV(name, num, 0 == memcmp(result,
                                               starBuffer,
                                               tail - (tBuffer + len)));
                *result = 0;
                ASSERTV(name, num, front, tBuffer,
                                             0 == memcmp(front, tBuffer, len));
                rc = u::parseInt(&cmpResult, tBuffer, result, base);
                if (signedType && flip) {
                    ASSERT(0 == rc);
                    Int64 iCmpResult = cmpResult;
                    ASSERT(iCmpResult ==  static_cast<Int64>(num));
                }
                else {
                    ASSERT(cmpResult == static_cast<Uint64>(num));
                }

                if (numArg != 0) {
                    if (flip) {
                        ASSERTV(name, num, '-' == *tBuffer);
                        ASSERTV(name, num, '0' != tBuffer[1]);
                    }
                    else {
                        ASSERTV(name, num, '0' != *tBuffer);
                    }
                }

#if             defined(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV)
                {
                    char nBuffer[100];
                    char *nTail = nBuffer + (tail - tBuffer);
                    const std::to_chars_result nResult =
                                      std::to_chars(nBuffer, nTail, num, base);
                    if (result) {
                        ASSERT(result - tBuffer == nResult.ptr - nBuffer);
                        ASSERT(0 == memcmp(tBuffer,
                                           nBuffer,
                                           result - tBuffer));
                        ASSERT(nResult.ec == std::errc());
                    }
                    else {
                        ASSERT(nResult.ec  == std::errc::value_too_large);
                        ASSERT(nResult.ptr == nTail);
                    }
                }
#endif
            }
        }

        if (!signedType || 0 == num ||
                   static_cast<Uint64>(numeric_limits<TYPE>::max()) < numArg) {
            return;                                                   // RETURN
        }
    }
}

void testIntValueMeta(Uint64 num, unsigned base)
    // Run 'u::testIntValue<TYPE>' on the specified 'num' and the specified
    // 'base' for every integral fundamental type, signed and unsigned, up to
    // 64 bits long.
{
#undef  TEST
#define TEST(type)    u::testIntValue<type>(num, base)

    TEST(char);
    TEST(signed char);
    TEST(unsigned char);

    TEST(short);
    TEST(unsigned short);

    TEST(int);
    TEST(unsigned int);

    TEST(Uint64);
    TEST(Int64);

#undef TEST
}

const Uint64 uint64Max = ~0ULL;    // max value a 'Uint64' can represent

}  // close namespace 'u'

// ----------------------------------------------------------------------------
// Fix 'isinf' and 'isnan' support for C++03 Solaris and AIX

#ifdef isinf
#undef isinf
#endif

#ifdef isnan
#undef isnan
#endif

struct DoubleWrapper {
    double d_value;

    BSLA_MAYBE_UNUSED DoubleWrapper(double d) : d_value(d) {}
};

BSLA_MAYBE_UNUSED
bool isinf(DoubleWrapper w)
{
    return w.d_value > DBL_MAX || w.d_value < -DBL_MAX;
}

BSLA_MAYBE_UNUSED
bool isnan(DoubleWrapper w)
{
    return w.d_value != w.d_value;
}

}  // close unnamed namespace


///Example 1: Writing an Integer to a 'streambuf'
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to define a function that writes an 'int' to a 'streambuf'.
// We can use 'bsl::to_chars' to write the 'int' to a buffer, then write the
// buffer to the 'streambuf'.
//
// First, we declare our function:
//..
    void writeJsonScalar(std::streambuf *result, int value)
        // Write the specified 'value', in decimal, to the specified 'result'.
    {
//..
// Then, we declare a buffer long enough to store any 'int' value in decimal.
//..
        char buffer[bslalg::NumericFormatterUtil::
                                               ToCharsMaxLength<int>::k_VALUE];
                                   // size large enough to write 'INT_MIN', the
                                   // worst-case value, in decimal.
//..
// Next, we call the function:
//..
        char *ret = bslalg::NumericFormatterUtil::toChars(
                                                        buffer,
                                                        buffer + sizeof buffer,
                                                        value);
//..
// Then, we check that the buffer was long enough, which should always be the
// case:
//..
        ASSERT(0 != ret);
//..
// Finally, we write our buffer to the 'streambuf':
//..
        result->sputn(buffer, ret - buffer);
    }
//..

///Example 2: Writing the Minimal Form of a 'double'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to store a floating point number using decimal text (such as
// JSON) for later retrieval, using the minimum number of digits that ensures
// we can later restore the same binary floating point value.
//
// First, we declare our writer function:
//..
    void writeJsonScalar(std::streambuf *result,
                         double          value,
                         bool            stringNonNumericValues = false)
        // Write the specified 'value' in the shortest round-trip decimal
        // format into the specified 'result'.  Write non-numeric values
        // according to the optionally specified 'stringNonNumericValues'
        // either as strings "NaN", "+Infinity", or "-Infinity" when
        // 'stringNonNumericValues' is 'true', or a null when it is 'false' or
        // not specified.
    {
//..
// Then, we handle non-numeric values ('toChars' would write them the XSD way):
//..
        if (isnan(value) || isinf(value)) {
            if (false == stringNonNumericValues) {  // JSON standard output
                result->sputn("null", 4);
            }
            else {                                  // Frequent JSON extension
                if (isnan(value)) {
                    result->sputn("\"NaN\"", 5);
                }
                else if (isinf(value)) {
                    result->sputn(value < 0 ? "\"-" : "\"+", 2);
                    result->sputn("Infinity\"", 9);
                }
            }
            return;                                                   // RETURN
        }
        //..
// Next, we declare a buffer long enough to store any 'double' value written in
// this minimal-length form:
//..
        char buffer[bslalg::NumericFormatterUtil::
                                            ToCharsMaxLength<double>::k_VALUE];
                                  // large enough to write the longest 'double'
                                  // without a null terminator character.
//..
// Then, we call the function:
//..
        char *ret = bslalg::NumericFormatterUtil::toChars(
                                                        buffer,
                                                        buffer + sizeof buffer,
                                                        value);
//..
// Finally, we can write our buffer to the 'streambuf':
//..
        result->sputn(buffer, ret - buffer);
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? std::atoi(argv[1]) : 0;
                verbose = argc > 2;    (void) verbose;
            veryVerbose = argc > 3;    (void) veryVerbose;
        veryVeryVerbose = argc > 4;    (void) veryVeryVerbose;
    veryVeryVeryVerbose = argc > 5;    (void) veryVeryVeryVerbose;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 18: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 Create the function 'writeJsonScalar' which is the usage example,
        //:   and call it a few times here to make sure it functions as
        //:   expected.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

        // These lines are not part of the usage examples, they just verify
        // that the functions defined by the usage examples work as intended.

///Example 1: Writing an Integer to a 'streambuf'
/// - - - - - - - - - - - - - - - - - - - - - - -
// See 'void writeJsonScalar(std::streambuf *result, int value)' definition
// above, before 'main'.
//
// Finally, we use an output string stream buffer to exercise the
// 'writeJsonScalar' function for 'int':
//..
    std::ostringstream  oss;
    std::streambuf* sb = oss.rdbuf();

    writeJsonScalar(sb, 0);
    ASSERT("0" == oss.str());

    oss.str("");
    writeJsonScalar(sb, 99);
    ASSERT("99" == oss.str());

    oss.str("");
    writeJsonScalar(sb, -1234567890);  // worst case: max string length
    ASSERT("-1234567890" == oss.str());
//..
//
///Example 2: Writing the Minimal Form of a 'double'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// See 'void writeJsonScalar(std::streambuf *, double, bool)' definition
// above, before 'main'.
//
// Finally, we use the output string stream buffer defined earlier to exercise
// the floating point 'writeJsonScalar' function:
//..
    oss.str("");
    writeJsonScalar(sb, 20211017.0);
    ASSERT("20211017" == oss.str());

    oss.str("");
    writeJsonScalar(sb, 3.1415926535897932);
    ASSERT("3.141592653589793" == oss.str());

    oss.str("");
    writeJsonScalar(sb, 2e5);
    ASSERT("2e+05" == oss.str());

    oss.str("");                  // Non-numeric are written as null by default
    writeJsonScalar(sb, std::numeric_limits<double>::quiet_NaN());
    ASSERT("null" == oss.str());  oss.str("");

    oss.str("");                  // Non-numeric can be printed as strings
    writeJsonScalar(sb, std::numeric_limits<double>::quiet_NaN(), true);
    ASSERT("\"NaN\"" == oss.str());  oss.str("");
//..
//
///Example 3: Determining The Necessary Minimum Buffer Size
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose you are writing code that uses 'bslalg::NumericFormatterUtil' to
// convert values to text.  Determining the necessary buffer sizes to ensure
// successful conversions, especially for floating point types, is non-trivial,
// and frankly usually strikes as a distraction in the flow of the work.  This
// component provides the 'ToCharsMaxLength' 'struct' "overloaded" template
// that parallels the overloaded 'toChars' function variants and provides the
// well-vetted and tested minimum sufficient buffer size values as compile time
// constants.
//
// Determining the sufficient buffer size for any conversion starts with
// determining "What type are we converting?" and "Do we use an argument to
// control the conversion, and is that argument a compile time time constant?
//
// First, because of the descriptive type names we may want to start by locally
// shortening them using a 'typedef':
//..
    typedef bslalg::NumericFormatterUtil NfUtil;
//..
//
// Next, we determine the sufficient buffer size for converting a 'long' to
// decimal.  'long' is a type that has different 'sizeof' on different 64 bit
// platforms, so it is especially convenient not to :
//..
    const size_t k_LONG_DEC_SIZE = NfUtil::ToCharsMaxLength<long>::k_VALUE;
        // Sufficient buffer size to convert any 'long' value to decimal text.
//..
//
// Then, we can write the longest possible 'long' successfully into a buffer:
//..
    char longDecimalBuffer[k_LONG_DEC_SIZE];
        // We can write any 'long' in decimal into this buffer using
        // 'NfUtil::toChars' safely.

    char *p = NfUtil::toChars(longDecimalBuffer,
                              longDecimalBuffer + sizeof longDecimalBuffer,
                              LONG_MIN);
    ASSERT(p != 0);
//..
//..
//
// Next, we can get the sufficient size for conversion of an 'unsigned int' to
// octal:
//..
    const size_t k_UINT_OCT_SIZE = NfUtil::ToCharsMaxLength<unsigned,
                                                            8>::k_VALUE;
//..
// Then, if we do not know what 'base' value 'toChars' will use we have to,
// assume the longest, which is always base 2:
//..
    const size_t k_SHRT_MAX_SIZE = NfUtil::ToCharsMaxLength<short, 2>::k_VALUE;
//..
//
// Now, floating point types have an optional 'format' argument instead of a
// 'base', with "default" format as the default, and "fixed" and "scientific"
// formats are selectable when a 'format' argument is specified:
//..
    const size_t k_DBL_DFL_SIZE = NfUtil::ToCharsMaxLength<double>::k_VALUE;

    const size_t k_FLT_DEC_SIZE = NfUtil::ToCharsMaxLength<
                                                     float,
                                                     NfUtil::e_FIXED>::k_VALUE;

    const size_t k_DBL_SCI_SIZE = NfUtil::ToCharsMaxLength<
                                                double,
                                                NfUtil::e_SCIENTIFIC>::k_VALUE;
//..
//
// Finally, the longest floating point format is 'e_FIXED', so if the 'format'
// argument is not known at compile time, 'e_FIXED' should be used:
//..
    const size_t k_DBL_MAX_SIZE = NfUtil::ToCharsMaxLength<
                                                     double,
                                                     NfUtil::e_FIXED>::k_VALUE;
//..
    (void)k_UINT_OCT_SIZE;
    (void)k_SHRT_MAX_SIZE;
    (void)k_SHRT_MAX_SIZE;
    (void)k_DBL_DFL_SIZE;
    (void)k_FLT_DEC_SIZE;
    (void)k_DBL_SCI_SIZE;
    (void)k_DBL_MAX_SIZE;
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // MAXIMUM NECESSARY BUFFER LENGTH TESTS
        //
        // Concerns:
        //:  1 A query using unsupported types results in comprehensible
        //:    compilation error that helps identify the issue.  Special
        //:    attention need be paid to the 'bool' type that is an integral
        //:    type by 'numeric_limits' definition, but is excluded from
        //:    'toChars' as it is not a number-type.
        //:
        //:  2 Same comprehensible compilation error occurs when the second,
        //:    non-type template argument is specified for an unsupported type.
        //:
        //:  3 A query using a supported integral type and an out-of-contract
        //:    'base' second (non-type) template argument value results in a
        //:    comprehensible compilation error that helps identify that the
        //:    value for 'base' is wrong.
        //:
        //:  4 A query using a supported floating type and a 'format' second
        //:    (non-type) template argument that is not one of the supported
        //:    enumerator values results in comprehensible a compilation error
        //:    that helps identify the wrong 'base' value.  'long double' is
        //:    not currently supported and must be tested as such.
        //:
        //:  5 Querying the supported integral types without providing a 'base'
        //:    argument results in 'k_VALUE' indicating the decimal, or
        //:    'base == 10' maximum necessary buffer length, with special care
        //:    on 'char' whose "signedness" is platform dependent.
        //:
        //:  6 Querying the supported integral types and providing a valid
        //:    'base' argument results in 'k_VALUE' indicating the maximum
        //:    necessary buffer length for that specified 'base'.  See 'char'
        //:    comment on C-7.
        //:
        //:  7 Querying 'float' without providing a 'format' argument results
        //:    in a 'k_VALUE' indicating the "minimal" or default format, which
        //:    in fact is the same as for scientific format.  Comprehensive
        //:    (brute force) verification of the value is possible but time
        //:    consuming, hence it is done in a (manual) negative test case.
        //:
        //:  8 Querying 'float' with an 'e_SCIENTIFIC' 'format' argument
        //:    results in a 'k_VALUE' of 15.  Comprehensive (brute force)
        //:    verification of the value is possible but time consuming, hence
        //:    it is provided in test case -2.  See the {Implementation Notes}
        //:    in the .cpp file for explanation for the value 15.
        //:
        //:  9 Querying 'float' with an 'e_FIXED' 'format' argument results in
        //:    a 'k_VALUE' of 48.  Comprehensive (brute force) verification of
        //:    the value is possible but time consuming, hence it is provided
        //:    in test case -2.  See the {Implementation Notes} in the .cpp
        //:    file for explanation for the value 48.
        //:
        //: 10 Querying 'double' without providing a 'format' argument results
        //:    in a 'k_VALUE' indicating the "minimal" or default format, which
        //:    in fact is the same as for scientific format.
        //:
        //: 11 Querying 'double' with an 'e_SCIENTIFIC' 'format' argument
        //:    results in a 'k_VALUE' of 24.  Comprehensive (brute force)
        //:    verification of the value is not possible as it would require
        //:    2^63 numbers tested.  See the {Implementation Notes} in the .cpp
        //:    file for explanation for the value 24.
        //:
        //: 12 Querying 'double' with an 'e_FIXED' 'format' argument results in
        //:    a 'k_VALUE' of 327.  Comprehensive (brute force) verification of
        //:    the value  of the value is not possible as it would strictly
        //:    require 2^63 numbers tested.  See the {Implementation Notes} in
        //:    the .cpp file for explanation for the value 24.
        //:
        //: 13 The correct maximum length in this test driver shall be
        //:    determined by means other than the ones used in the production
        //:    code that is tested.
        //
        // Plan:
        //: 1 Provide commented out lines to the instantiation that should not
        //:   compile and test them manually on all major platform flavors. C-1
        //:   to C-4.
        //:
        //: 2 Integer types with default and specified 'base' are verified by
        //:   converting 'bsl::numeric_limits::'max()' using that 'base' and
        //:   verifying the length is shorter than 'k_VALUE', as well as
        //:   converting 'bsl::numeric_limits::'min()' using that 'base' and
        //:   verifying that the length is exactly 'k_VALUE'. C-5, C-6, C-15.
        //:
        //: 3 Floating point 'k_VALUES' are verified against the constants we
        //:   expect to see.  Because brute-force testing would take too long
        //:   time for a unit test we verify that the following numbers fit
        //:   into buffers of 'k_VALUE' size in all 3 formats:
        //:
        //:   1 Negative boundary values from the corresponding
        //:     'std::numeric_limits<T>': '-max()', '-min()',
        //:     and '-denorm_min()'.
        //:
        //:   2 For 'double' we also verify many negative subnormal values that
        //:     they fit into the decimal 'k_VALUE' size buffer, mostly to
        //:     indicate that subnormals were considered and tested (see
        //:     {Implementation Notes}).  The subnormals are created from a
        //:     varying pattern binary significants of maximum 52 bits.
        //
        // Testing:
        //   ToCharsMaxLength<T, A>::k_VALUE
        // --------------------------------------------------------------------

        if (verbose) printf("MAXIMUM NECESSARY BUFFER LENGTH TESTS\n"
                            "=====================================\n");

#define U_VERIFY_UNSUPPORTED_TYPE_COMPILER_ERROR(type) do {                   \
        const size_t k_SIZE = Util::ToCharsMaxLength<type>::k_VALUE;          \
        ASSERTV(k_SIZE,                                                       \
                !"ToCharsMaxLength<" #type ">::k_VALUE should not compile");  \
    } while (false)

        // Uncomment a line below to verify the compiler error message

        //U_VERIFY_UNSUPPORTED_TYPE_COMPILER_ERROR(char *);
        //U_VERIFY_UNSUPPORTED_TYPE_COMPILER_ERROR(Util);
        //U_VERIFY_UNSUPPORTED_TYPE_COMPILER_ERROR(u::IncompleteType);
        //U_VERIFY_UNSUPPORTED_TYPE_COMPILER_ERROR(void);
        //U_VERIFY_UNSUPPORTED_TYPE_COMPILER_ERROR(bool);
        //U_VERIFY_UNSUPPORTED_TYPE_COMPILER_ERROR(long double);

#undef U_VERIFY_UNSUPPORTED_TYPE_COMPILER_ERROR

#define U_VERIFY_UNSUPPORTED_TYPE_COMPILER_ERROR2(type, arg) do {             \
        const size_t k_SIZE = Util::ToCharsMaxLength<type, arg>::k_VALUE;     \
        ASSERTV(k_SIZE,                                                       \
                !"ToCharsMaxLength<" #type ", " #arg ">::k_VALUE should not"  \
                                                                  "compile"); \
    } while (false)

        // Uncomment a line below to verify the compiler error message

        //U_VERIFY_UNSUPPORTED_TYPE_COMPILER_ERROR2(char *, 3);
        //U_VERIFY_UNSUPPORTED_TYPE_COMPILER_ERROR2(Util, 1);
        //U_VERIFY_UNSUPPORTED_TYPE_COMPILER_ERROR2(u::IncompleteType, 8);
        //U_VERIFY_UNSUPPORTED_TYPE_COMPILER_ERROR2(void, 4);
        //U_VERIFY_UNSUPPORTED_TYPE_COMPILER_ERROR2(bool, Util::e_SCIENTIFIC);
        //U_VERIFY_UNSUPPORTED_TYPE_COMPILER_ERROR2(long double, 16);

#undef U_VERIFY_UNSUPPORTED_TYPE_COMPILER_ERROR2

#define U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR(type, arg) do {               \
        const size_t k_SIZE = Util::ToCharsMaxLength<type, arg>::k_VALUE;     \
        ASSERTV(k_SIZE,                                                       \
                !"ToCharsMaxLength<" #type ", " #arg ">::k_VALUE should not"  \
                                                                  "compile"); \
    } while (false)

        // Uncomment a line below to verify the compiler error message

        //U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR(char, 1);
        //U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR(signed char, 42);
        //U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR(unsigned char, -8);

        //U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR(  signed short int, 1);
        //U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR(unsigned short int, 37);

        //U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR(  signed int, 1);
        //U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR(unsigned int, 37);

        //U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR(  signed long int, 1);
        //U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR(unsigned long int, 37);

        //U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR(  signed long long int, 1);
        //U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR(unsigned long long int, 37);

        //U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR(float, 2);
        //U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR(float, -33);

        //U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR(double, 1);
        //U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR(double, 37);

#undef U_VERIFY_UNSUPPORTED_ARG_COMPILER_ERROR

        using u::verifyAllMaxLenForAnIntType;

        verifyAllMaxLenForAnIntType<         char>(         "char");
        verifyAllMaxLenForAnIntType<  signed char>(  "signed char");
        verifyAllMaxLenForAnIntType<unsigned char>("unsigned char");

        verifyAllMaxLenForAnIntType<  signed short int>(  "signed short int");
        verifyAllMaxLenForAnIntType<unsigned short int>("unsigned short int");

        verifyAllMaxLenForAnIntType<  signed int>(  "signed int");
        verifyAllMaxLenForAnIntType<unsigned int>("unsigned int");

        verifyAllMaxLenForAnIntType<  signed long int>(  "signed long int");
        verifyAllMaxLenForAnIntType<unsigned long int>("unsigned long int");

        verifyAllMaxLenForAnIntType<  signed long long int>(
                                                       "signed long long int");
        verifyAllMaxLenForAnIntType<unsigned long long int>(
                                                     "unsigned long long int");

        // Verification of floating point sufficient length reporting

        if (veryVerbose) puts("Verifying 'double' 'ToCharsMaxLength'.");
        {
            typedef std::numeric_limits<double> Lim;
            using u::NumDecimalDigits;

            static const size_t MAX_SCIENTIFIC_DOUBLE_LENGTH =
                // See further documentation of this calculation in the
                // {Implementation Notes} in the '.cpp' file.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
                Lim::max_digits10 +
#else
                17 +
#endif                                                            //  17
                NumDecimalDigits<-Lim::min_exponent10>::value +   // + 3 ==> 20
                1 + // optional sign character                    // + 1 ==> 21
                1 + // optional radix mark (decimal point)        // + 1 ==> 22
                1 + // 'e' of the scientific format               // + 1 ==> 23
                1;  // sign for the scientific form exponent      // + 1 ==> 24

            const size_t MAX_DEFAULT_DOUBLE_LENGTH =
                                                  MAX_SCIENTIFIC_DOUBLE_LENGTH;
                // Default format falls back to scientific format when that is
                // shorter than the fixed format, so its maximum possible
                // length is the same as the scientific.

            static const std::ptrdiff_t MAX_FIXED_DOUBLE_LENGTH =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
                Lim::max_digits10 +
#else
                17 +
#endif                                                         //    17
                (-Lim::min_exponent10) +                       // + 307 ==> 324
                1 + // min() of double has -308 decimal exponent  +   1 ==> 325
                1 + // optional sign character                    +   1 ==> 326
                1;  // optional radix mark (decimal point)        +   1 ==> 327
                // See the detailed explanation in {Implementation Notes} in
                // the .cpp file, this is not the full picture!

            ASSERTV(Util::ToCharsMaxLength<double>::k_VALUE,
                    MAX_DEFAULT_DOUBLE_LENGTH,
                    MAX_DEFAULT_DOUBLE_LENGTH ==
                                      Util::ToCharsMaxLength<double>::k_VALUE);

            ASSERTV(
                 (Util::ToCharsMaxLength<double, Util::e_SCIENTIFIC>::k_VALUE),
                  MAX_SCIENTIFIC_DOUBLE_LENGTH,
                  MAX_SCIENTIFIC_DOUBLE_LENGTH ==
                        (Util::ToCharsMaxLength<double,
                                                Util::e_SCIENTIFIC>::k_VALUE));

            ASSERTV((Util::ToCharsMaxLength<double, Util::e_FIXED>::k_VALUE),
                    MAX_FIXED_DOUBLE_LENGTH,
                    MAX_FIXED_DOUBLE_LENGTH ==
                             (Util::ToCharsMaxLength<double,
                                                     Util::e_FIXED>::k_VALUE));

            const size_t k_HUGE_BUFFSIZE = 1024;
                // We use huge buffer size as we have to assume errors may be
                // present.

            BSLMF_ASSERT(k_HUGE_BUFFSIZE > 3 * MAX_FIXED_DOUBLE_LENGTH);
                // Let's make sure it *is* really huge.

            char buffer[k_HUGE_BUFFSIZE + 1];
                // Plus one for a closing null character so we can print the
                // buffer in an assert without tricks.

            if (veryVerbose) puts("Default format for 'double'.");
            do {
                // -std::numeric_limits<double>::min()
                char * p = Util::toChars(buffer,
                                         buffer + k_HUGE_BUFFSIZE,
                                         -Lim::min());

                size_t writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'
                ASSERTV(Util::ToCharsMaxLength<double>::k_VALUE,
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<double>::k_VALUE
                                                            >= writtenLength));

                // -std::numeric_limits<double>::max()
                p = Util::toChars(buffer,
                                  buffer + k_HUGE_BUFFSIZE,
                                  -Lim::max());

                writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV(Util::ToCharsMaxLength<double>::k_VALUE,
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<double>::k_VALUE
                                                            >= writtenLength));

                // smallest subnormal
                p = Util::toChars(buffer,
                                  buffer + k_HUGE_BUFFSIZE,
                                  -Lim::denorm_min());

                writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV(Util::ToCharsMaxLength<double>::k_VALUE,
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<double>::k_VALUE
                                                            >= writtenLength));
            } while (false);

            if (veryVerbose) puts("Scientific format for 'double'.");
            do {
                // -std::numeric_limits<double>::min()
                char * p = Util::toChars(buffer,
                                         buffer + k_HUGE_BUFFSIZE,
                                         -Lim::min(),
                                         Util::e_SCIENTIFIC);

                size_t writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV((Util::ToCharsMaxLength<double,
                                                Util::e_SCIENTIFIC>::k_VALUE),
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<double,
                                                Util::e_SCIENTIFIC>::k_VALUE
                                                            >= writtenLength));

                // -std::numeric_limits<double>::max()
                p = Util::toChars(buffer,
                                  buffer + k_HUGE_BUFFSIZE,
                                  -Lim::max(),
                                  Util::e_SCIENTIFIC);

                writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV((Util::ToCharsMaxLength<double,
                                                Util::e_SCIENTIFIC>::k_VALUE),
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<double,
                                                Util::e_SCIENTIFIC>::k_VALUE
                                                            >= writtenLength));

                // smallest subnormal
                p = Util::toChars(buffer,
                                  buffer + k_HUGE_BUFFSIZE,
                                  -Lim::denorm_min(),
                                  Util::e_SCIENTIFIC);

                writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV((Util::ToCharsMaxLength<double,
                                                Util::e_SCIENTIFIC>::k_VALUE),
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<double,
                                                Util::e_SCIENTIFIC>::k_VALUE
                                                            >= writtenLength));
            } while (false);

            if (veryVerbose) puts("Fixed format for 'double'.");
            do {
                // -std::numeric_limits<double>::min()
                char * p = Util::toChars(buffer,
                                         buffer + k_HUGE_BUFFSIZE,
                                         -Lim::min(),
                                         Util::e_FIXED);

                size_t writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV((Util::ToCharsMaxLength<double,
                                                Util::e_FIXED>::k_VALUE),
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<double,
                                                Util::e_FIXED>::k_VALUE
                                                            >= writtenLength));

                // -std::numeric_limits<double>::max()
                p = Util::toChars(buffer,
                                  buffer + k_HUGE_BUFFSIZE,
                                  -Lim::max(),
                                  Util::e_FIXED);

                writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV((Util::ToCharsMaxLength<double,
                                                Util::e_FIXED>::k_VALUE),
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<double,
                                                Util::e_FIXED>::k_VALUE
                                                            >= writtenLength));

                // smallest subnormal
                p = Util::toChars(buffer,
                                  buffer + k_HUGE_BUFFSIZE,
                                  -Lim::denorm_min(),
                                  Util::e_FIXED);

                writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV((Util::ToCharsMaxLength<double,
                                                Util::e_FIXED>::k_VALUE),
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<double,
                                                Util::e_FIXED>::k_VALUE
                                                            >= writtenLength));
            } while (false);

            // Longer Subnormals
            struct {
                int    d_line;
                Uint64 d_mantissa;
            } SUBNORMALS[] = {
                { L_, 0x0000000000001ull }, // one bit walks up
                { L_, 0x0000000000002ull },
                { L_, 0x0000000000004ull },
                { L_, 0x0000000000008ull },
                { L_, 0x0000000000010ull },
                { L_, 0x0000000000020ull },
                { L_, 0x0000000000040ull },
                { L_, 0x0000000000080ull },
                { L_, 0x0000000000100ull },
                { L_, 0x0000000000200ull },
                { L_, 0x0000000000400ull },
                { L_, 0x0000000000800ull },
                { L_, 0x0000000001000ull },
                { L_, 0x0000000002000ull },
                { L_, 0x0000000004000ull },
                { L_, 0x0000000008000ull },
                { L_, 0x0000000010000ull },
                { L_, 0x0000000020000ull },
                { L_, 0x0000000040000ull },
                { L_, 0x0000000080000ull },
                { L_, 0x0000000100000ull },
                { L_, 0x0000000200000ull },
                { L_, 0x0000000400000ull },
                { L_, 0x0000000800000ull },
                { L_, 0x0000001000000ull },
                { L_, 0x0000002000000ull },
                { L_, 0x0000004000000ull },
                { L_, 0x0000008000000ull },
                { L_, 0x0000010000000ull },
                { L_, 0x0000020000000ull },
                { L_, 0x0000040000000ull },
                { L_, 0x0000080000000ull },
                { L_, 0x0000100000000ull },
                { L_, 0x0000200000000ull },
                { L_, 0x0000400000000ull },
                { L_, 0x0000800000000ull },
                { L_, 0x0001000000000ull },
                { L_, 0x0002000000000ull },
                { L_, 0x0004000000000ull },
                { L_, 0x0008000000000ull },
                { L_, 0x0010000000000ull },
                { L_, 0x0020000000000ull },
                { L_, 0x0040000000000ull },
                { L_, 0x0080000000000ull },
                { L_, 0x0100000000000ull },
                { L_, 0x0200000000000ull },
                { L_, 0x0400000000000ull },
                { L_, 0x0800000000000ull },
                { L_, 0x1000000000000ull },
                { L_, 0x2000000000000ull },
                { L_, 0x4000000000000ull },
                { L_, 0x8000000000000ull },

                { L_, 0x0000000000003ull }, // filling with ones from the right
                { L_, 0x0000000000007ull },
                { L_, 0x000000000000Full },
                { L_, 0x000000000001Full },
                { L_, 0x000000000003Full },
                { L_, 0x000000000007Full },
                { L_, 0x00000000000FFull },
                { L_, 0x00000000001FFull },
                { L_, 0x00000000003FFull },
                { L_, 0x00000000007FFull },
                { L_, 0x0000000000FFFull },
                { L_, 0x0000000001FFFull },
                { L_, 0x0000000003FFFull },
                { L_, 0x0000000007FFFull },
                { L_, 0x000000000FFFFull },
                { L_, 0x000000001FFFFull },
                { L_, 0x000000003FFFFull },
                { L_, 0x000000007FFFFull },
                { L_, 0x00000000FFFFFull },
                { L_, 0x00000001FFFFFull },
                { L_, 0x00000003FFFFFull },
                { L_, 0x00000007FFFFFull },
                { L_, 0x0000000FFFFFFull },
                { L_, 0x0000001FFFFFFull },
                { L_, 0x0000003FFFFFFull },
                { L_, 0x0000007FFFFFFull },
                { L_, 0x000000FFFFFFFull },
                { L_, 0x000001FFFFFFFull },
                { L_, 0x000003FFFFFFFull },
                { L_, 0x000007FFFFFFFull },
                { L_, 0x00000FFFFFFFFull },
                { L_, 0x00001FFFFFFFFull },
                { L_, 0x00003FFFFFFFFull },
                { L_, 0x00007FFFFFFFFull },
                { L_, 0x0000FFFFFFFFFull },
                { L_, 0x0001FFFFFFFFFull },
                { L_, 0x0003FFFFFFFFFull },
                { L_, 0x0007FFFFFFFFFull },
                { L_, 0x000FFFFFFFFFFull },
                { L_, 0x001FFFFFFFFFFull },
                { L_, 0x003FFFFFFFFFFull },
                { L_, 0x007FFFFFFFFFFull },
                { L_, 0x00FFFFFFFFFFFull },
                { L_, 0x01FFFFFFFFFFFull },
                { L_, 0x03FFFFFFFFFFFull },
                { L_, 0x07FFFFFFFFFFFull },
                { L_, 0x0FFFFFFFFFFFFull },
                { L_, 0x1FFFFFFFFFFFFull },
                { L_, 0x3FFFFFFFFFFFFull },
                { L_, 0x7FFFFFFFFFFFFull },
                { L_, 0xFFFFFFFFFFFFFull },

                { L_, 0x0000000000002ull }, // 101010... pattern from the right
                { L_, 0x0000000000005ull },
                { L_, 0x000000000000Aull },
                { L_, 0x0000000000015ull },
                { L_, 0x000000000002Aull },
                { L_, 0x0000000000055ull },
                { L_, 0x00000000000AAull },
                { L_, 0x0000000000155ull },
                { L_, 0x00000000002AAull },
                { L_, 0x0000000000555ull },
                { L_, 0x0000000000AAAull },
                { L_, 0x0000000001555ull },
                { L_, 0x0000000002AAAull },
                { L_, 0x0000000005555ull },
                { L_, 0x000000000AAAAull },
                { L_, 0x0000000015555ull },
                { L_, 0x000000002AAAAull },
                { L_, 0x0000000055555ull },
                { L_, 0x00000000AAAAAull },
                { L_, 0x0000000155555ull },
                { L_, 0x00000002AAAAAull },
                { L_, 0x0000000555555ull },
                { L_, 0x0000000AAAAAAull },
                { L_, 0x0000001555555ull },
                { L_, 0x0000002AAAAAAull },
                { L_, 0x0000005555555ull },
                { L_, 0x000000AAAAAAAull },
                { L_, 0x0000015555555ull },
                { L_, 0x000002AAAAAAAull },
                { L_, 0x0000055555555ull },
                { L_, 0x00000AAAAAAAAull },
                { L_, 0x0000155555555ull },
                { L_, 0x00001AAAAAAAAull },
                { L_, 0x0000555555555ull },
                { L_, 0x0000AAAAAAAAAull },
                { L_, 0x0001555555555ull },
                { L_, 0x0002AAAAAAAAAull },
                { L_, 0x0005555555555ull },
                { L_, 0x000AAAAAAAAAAull },
                { L_, 0x0015555555555ull },
                { L_, 0x002AAAAAAAAAAull },
                { L_, 0x0055555555555ull },
                { L_, 0x00AAAAAAAAAAAull },
                { L_, 0x0155555555555ull },
                { L_, 0x02AAAAAAAAAAAull },
                { L_, 0x0555555555555ull },
                { L_, 0x0AAAAAAAAAAAAull },
                { L_, 0x1555555555555ull },
                { L_, 0x2AAAAAAAAAAAAull },
                { L_, 0x5555555555555ull },
                { L_, 0xAAAAAAAAAAAAAull },
            };
            const int NUM_SUBNORMALS = sizeof SUBNORMALS / sizeof * SUBNORMALS;

            for (int i = 0; i < NUM_SUBNORMALS; ++i) {
                const int    LINE  = SUBNORMALS[i].d_line;
                const double VALUE =
                                -makeSubnormalDouble(SUBNORMALS[i].d_mantissa);

                char * p = Util::toChars(buffer,
                                         buffer + k_HUGE_BUFFSIZE,
                                         VALUE,
                                         Util::e_FIXED);

                const size_t writtenLength = (p - buffer);
                ASSERTV(LINE, writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'
                ASSERTV(LINE,
                        (Util::ToCharsMaxLength<double,
                                                Util::e_FIXED>::k_VALUE),
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<double,
                                                Util::e_FIXED>::k_VALUE
                                                            >= writtenLength));
            }
        }

        if (veryVerbose) puts("Verifying 'float' 'ToCharsMaxLength'.");
        {
            typedef std::numeric_limits<float> Lim;
            using u::NumDecimalDigits;

            static const size_t MAX_SCIENTIFIC_FLOAT_LENGTH =
                // See further documentation of this calculation in the
                // {Implementation Notes} in the '.cpp' file.
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
                Lim::max_digits10 +
#else
                9 +
#endif                                                            //   9
                NumDecimalDigits<-Lim::min_exponent10>::value +   // + 2 ==> 11
                1 + // optional sign character                    // + 1 ==> 12
                1 + // optional radix mark (decimal point)        // + 1 ==> 13
                1 + // 'e' of the scientific format               // + 1 ==> 14
                1;  // sign for the scientific form exponent      // + 1 ==> 15

            const size_t MAX_DEFAULT_FLOAT_LENGTH =
                                                   MAX_SCIENTIFIC_FLOAT_LENGTH;
                // Default format falls back to scientific format when that is
                // shorter than the fixed format, so its maximum possible
                // length is the same as the scientific.

        static const std::ptrdiff_t MAX_FIXED_FLOAT_LENGTH =
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
                Lim::max_digits10 +
#else
                9 +
#endif                                                           //    9
                (-Lim::min_exponent10) +                         // + 37 ==> 46
                1 + // optional sign character                      +  1 ==> 47
                1;  // optional radix mark (decimal point)          +  1 ==> 48

            ASSERTV(Util::ToCharsMaxLength<float>::k_VALUE,
                    MAX_DEFAULT_FLOAT_LENGTH,
                    MAX_DEFAULT_FLOAT_LENGTH ==
                                       Util::ToCharsMaxLength<float>::k_VALUE);

            ASSERTV(
                 (Util::ToCharsMaxLength<float, Util::e_SCIENTIFIC>::k_VALUE),
                  MAX_SCIENTIFIC_FLOAT_LENGTH,
                  MAX_SCIENTIFIC_FLOAT_LENGTH ==
                        (Util::ToCharsMaxLength<float,
                                                Util::e_SCIENTIFIC>::k_VALUE));

            ASSERTV((Util::ToCharsMaxLength<float, Util::e_FIXED>::k_VALUE),
                    MAX_FIXED_FLOAT_LENGTH,
                    MAX_FIXED_FLOAT_LENGTH ==
                             (Util::ToCharsMaxLength<float,
                                                     Util::e_FIXED>::k_VALUE));

            const size_t k_HUGE_BUFFSIZE = 1024;
                // We use huge buffer size as we have to assume errors may be
                // present.
            char buffer[k_HUGE_BUFFSIZE + 1];
                // Plus one for a closing null character so we can print the
                // buffer in an assert without tricks.

            if (veryVerbose) puts("Default format for 'float'.");
            do {
                // -std::numeric_limits<double>::min()
                char * p = Util::toChars(buffer,
                                         buffer + k_HUGE_BUFFSIZE,
                                         -Lim::min());

                size_t writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV(Util::ToCharsMaxLength<float>::k_VALUE,
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<float>::k_VALUE
                                                            >= writtenLength));

                // -std::numeric_limits<double>::max()
                p = Util::toChars(buffer,
                                  buffer + k_HUGE_BUFFSIZE,
                                  -Lim::max());

                writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV(Util::ToCharsMaxLength<float>::k_VALUE,
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<float>::k_VALUE
                                                            >= writtenLength));

                // smallest subnormal
                p = Util::toChars(buffer,
                                  buffer + k_HUGE_BUFFSIZE,
                                  -Lim::denorm_min());

                writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV(Util::ToCharsMaxLength<float>::k_VALUE,
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<float>::k_VALUE
                                                            >= writtenLength));
            } while (false);

            if (veryVerbose) puts("Scientific format for 'float'.");
            do {
                // -std::numeric_limits<double>::min()
                char * p = Util::toChars(buffer,
                                         buffer + k_HUGE_BUFFSIZE,
                                         -Lim::min(),
                                         Util::e_SCIENTIFIC);

                size_t writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV((Util::ToCharsMaxLength<float,
                                                Util::e_SCIENTIFIC>::k_VALUE),
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<float,
                                                Util::e_SCIENTIFIC>::k_VALUE
                                                            >= writtenLength));

                // -std::numeric_limits<double>::max()
                p = Util::toChars(buffer,
                                  buffer + k_HUGE_BUFFSIZE,
                                  -Lim::max(),
                                  Util::e_SCIENTIFIC);

                writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV((Util::ToCharsMaxLength<float,
                                                Util::e_SCIENTIFIC>::k_VALUE),
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<float,
                                                Util::e_SCIENTIFIC>::k_VALUE
                                                            >= writtenLength));

                // smallest subnormal
                p = Util::toChars(buffer,
                                  buffer + k_HUGE_BUFFSIZE,
                                  -Lim::denorm_min(),
                                  Util::e_SCIENTIFIC);

                writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV((Util::ToCharsMaxLength<float,
                                                Util::e_SCIENTIFIC>::k_VALUE),
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<float,
                                                Util::e_SCIENTIFIC>::k_VALUE
                                                            >= writtenLength));
            } while (false);

            if (veryVerbose) puts("Fixed format for 'floats'.");
            do {
                // -std::numeric_limits<double>::min()
                char * p = Util::toChars(buffer,
                                         buffer + k_HUGE_BUFFSIZE,
                                         -Lim::min(),
                                         Util::e_FIXED);

                size_t writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV((Util::ToCharsMaxLength<float,
                                                Util::e_FIXED>::k_VALUE),
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<float,
                                                Util::e_FIXED>::k_VALUE
                                                            >= writtenLength));

                p = Util::toChars(buffer,
                                  buffer + k_HUGE_BUFFSIZE,
                                  -Lim::min(),
                                  Util::e_SCIENTIFIC);
                writtenLength = (p - buffer);
                *p = '\0';

                // -std::numeric_limits<double>::max()
                p = Util::toChars(buffer,
                                  buffer + k_HUGE_BUFFSIZE,
                                  -Lim::max(),
                                  Util::e_FIXED);

                writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV((Util::ToCharsMaxLength<float,
                                                Util::e_FIXED>::k_VALUE),
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<float,
                                                Util::e_FIXED>::k_VALUE
                                                            >= writtenLength));

                // smallest subnormal
                p = Util::toChars(buffer,
                                  buffer + k_HUGE_BUFFSIZE,
                                  -Lim::denorm_min(),
                                  Util::e_FIXED);

                writtenLength = (p - buffer);
                ASSERTV(writtenLength, writtenLength <= k_HUGE_BUFFSIZE);
                if (writtenLength > k_HUGE_BUFFSIZE) {
                    break;                                             // BREAK
                }
                *p = '\0';  // Enable 'ASSERTV' printing of 'buffer'

                ASSERTV((Util::ToCharsMaxLength<float,
                                                Util::e_FIXED>::k_VALUE),
                        writtenLength,
                        buffer,
                        (Util::ToCharsMaxLength<float,
                                                Util::e_FIXED>::k_VALUE
                                                            >= writtenLength));
            } while (false);
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // 'float' WITH FORMAT PARAMETER TEST
        //
        // Concerns:
        //: 1 The format of the format argument is used for the output.
        //:
        //: 2 No buffer overrun in writing to the output, regardless of the
        //:   provided output buffer size.
        //:
        //: 3 Zero is returned when the specified output buffer is too short,
        //:   and the output buffer is unchanged (i.e., no text written).
        //:
        //: 4 A pointer one past the last written character is returned when
        //:   the conversion to text is successful.
        //
        // Plan:
        //: 1 Run the tests below for both 'e_SCIENTIFIC', and 'e_FIXED'
        //:   formats (two test data tables).
        //:
        //: 2 Use tables of values with expected output.
        //:
        //: 3 Use a one-longer-than-max output buffer to have space to
        //:
        //:   1 Enable 'ASSERTV'-printing by null terminating the buffer.
        //:
        //:   2 Pre-fill the complete (oversized) buffer by a non-printable,
        //:     non-null character and verify after the call to 'toChars' what
        //:     characters have been written to, and if an immediate (as in
        //:     immediately-following-character) buffer overrun has occurred.
        //:
        //: 4 Attempt several conversions on every table row:
        //:
        //:   1 With 0 pointers for 'first' and 'last' arguments, must always
        //:     fail.
        //:
        //:   2 With the same non-0 pointer  for 'first' and 'last' arguments
        //:    (zero output length), must always fail.
        //:
        //:   3 With expected size-1 output length, must always fail.
        //:
        //:   4 With the exact expected size for output length, must always
        //:     succeed.
        //:
        //:   5 With the maximum necessary buffer length (48 for 'e_DECIMAL',
        //:     15 for 'e_SCIENTIFIC') for output length, must always succeed.
        //:
        //: 5 Verify the output of expected-to-be-successful conversions by
        //:   comparing them to the expected result from the table.
        //:
        //: 6 When the corresponding 'std::to_chars' is present use it as an
        //:   oracle, and verify the successful result against that as well.
        //:
        //: 7 Use preprocessor stringification to ensure accurate printing of
        //:   intended input in (test error) messages.
        //:
        //: 8 To avoid data duplication when seeing a positive value test its
        //:   negative counterpart automatically.
        //
        // Testing:
        //   char *toChars(char *first, char *last, float value, format);
        // --------------------------------------------------------------------

        if (verbose) printf("'float' WITH FORMAT PARAMETER TEST\n"
                            "==================================\n");

        using namespace u; // Everything in this case is delegated to templates

        if (veryVerbose) puts("Verifying 'e_SCIENTIFIC'");
        verifyFloatingPointToChars<float, ToCharsCallScientific>(k_FLT_SCI);

        if (veryVerbose) puts("Verifying 'e_FIXED'");
        verifyFloatingPointToChars<float, ToCharsCallDecimal>(k_FLT_DEC);

        if (veryVerbose) puts("Non-numerical value tests");
        verifyToCharsNonNumericFloatingPointValues<float,
                                                   ToCharsCallScientific>();
        verifyToCharsNonNumericFloatingPointValues<float,
                                                   ToCharsCallDecimal>();
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // 'double' WITH FORMAT PARAMETER TEST
        //
        // Concerns:
        //: 1 The format of the format argument is used for the output.
        //:
        //: 2 No buffer overrun in writing to the output, regardless of the
        //:   provided output buffer size.
        //:
        //: 3 Zero is returned when the specified output buffer is too short,
        //:   and the output buffer is unchanged (i.e., no text written).
        //:
        //: 4 A pointer one past the last written character is returned when
        //:   the conversion to text is successful.
        //
        // Plan:
        //: 1 Run the tests below for both 'e_SCIENTIFIC', and 'e_FIXED'
        //:   formats (two test data tables).
        //:
        //: 2 Use tables of values with expected output.
        //:
        //: 3 Use a one-longer-than-max output buffer to have space to
        //:
        //:   1 Enable 'ASSERTV'-printing by null terminating the buffer.
        //:
        //:   2 Pre-fill the complete (oversized) buffer by a non-printable,
        //:     non-null character and verify after the call to 'toChars' what
        //:     characters have been written to, and if an immediate (as in
        //:     immediately-following-character) buffer overrun has occurred.
        //:
        //: 4 Attempt several conversions on every table row:
        //:
        //:   1 With 0 pointers for 'first' and 'last' arguments, must always
        //:     fail.
        //:
        //:   2 With the same non-0 pointer  for 'first' and 'last' arguments
        //:    (zero output length), must always fail.
        //:
        //:   3 With expected size-1 output length, must always fail.
        //:
        //:   4 With the exact expected size for output length, must always
        //:     succeed.
        //:
        //:   5 With the maximum necessary buffer length (327 for 'e_DECIMAL',
        //:     24 for 'e_SCIENTIFIC') for output length, must always succeed.
        //:
        //: 5 Verify the output of expected-to-be-successful conversions by
        //:   comparing them to the expected result from the table.
        //:
        //: 6 When the corresponding 'std::to_chars' is present use it as an
        //:   oracle, and verify the successful result against that as well.
        //:
        //: 7 Use preprocessor stringification to ensure accurate printing of
        //:   intended input in (test error) messages.
        //:
        //: 8 To avoid data duplication when seeing a positive value test its
        //:   negative counterpart automatically.
        //
        // Testing:
        //   char *toChars(char *first, char *last, double value, format);
        // --------------------------------------------------------------------

        if (verbose) printf("'double' WITH FORMAT PARAMETER TEST\n"
                            "===================================\n");

        using namespace u; // Everything in this case is delegated to templates

        if (veryVerbose) puts("Verifying 'e_SCIENTIFIC'");
        verifyFloatingPointToChars<double, ToCharsCallScientific>(k_DBL_SCI);

        if (veryVerbose) puts("Verifying 'e_FIXED'");
        verifyFloatingPointToChars<double, u::ToCharsCallDecimal>(k_DBL_DEC);

        if (veryVerbose) puts("Non-numerical value tests");
        verifyToCharsNonNumericFloatingPointValues<double,
                                                   ToCharsCallScientific>();
        verifyToCharsNonNumericFloatingPointValues<double,
                                                   ToCharsCallDecimal>();
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // 'float' MINIMAL CONVERSION TEST
        //
        // Concerns:
        //: 1 Writing chooses the right format and writes the right number.
        //:
        //: 2 No buffer overrun in writing to the output, regardless of the
        //:   provided output buffer size.
        //:
        //: 3 Zero is returned when the specified output buffer is too short,
        //:   and the output buffer is unchanged (i.e., no text written).
        //:
        //: 4 A pointer one past the last written character is returned when
        //:   the conversion to text is successful.
        //
        // Plan:
        //: 1 Use a table of values with expected output.
        //:
        //: 2 Use a one-longer-than-max output buffer to have space to
        //:
        //:   1 Enable 'ASSERTV'-printing by null terminating the buffer.
        //:
        //:   2 Pre-fill the complete (oversized) buffer by a non-printable,
        //:     non-null character and verify after the call to 'toChars' what
        //:     characters have been written to, and if an immediate (as in
        //:     immediately-following-character) buffer overrun has occurred.
        //:
        //: 3 Attempt several conversions on every table row:
        //:
        //:   1 With 0 pointers for 'first' and 'last' arguments, must always
        //:     fail.
        //:
        //:   2 With the same non-0 pointer  for 'first' and 'last' arguments
        //:    (zero output length), must always fail.
        //:
        //:   3 With expected size-1 output length, must always fail.
        //:
        //:   4 With the exact expected size for output length, must always
        //:     succeed.
        //:
        //:   5 With the maximum necessary buffer length (15) for output
        //:     length, must always succeed.
        //:
        //: 4 Verify the output of expected-to-be-successful conversions by
        //:    comparing them to the expected result from the table.
        //:
        //: 5 When the corresponding 'std::to_chars' is present use it as an
        //:   oracle, and verify the successful result against that as well.
        //:
        //: 6 Use preprocessor stringification to ensure accurate printing of
        //:   intended input in (test error) messages.
        //:
        //: 7 To avoid data duplication when seeing a positive value test its
        //:   negative counterpart automatically.
        //
        // Testing:
        //   char *toChars(char *first, char *last, double value);
        // --------------------------------------------------------------------

        if (verbose) printf("'float' MINIMAL CONVERSION TEST\n"
                            "===============================\n");

        using namespace u; // Everything in this case is delegated to templates

        if (veryVerbose) puts("Testing numbers");
        verifyFloatingPointToChars<float, ToCharsCallNoFormat>(k_FLT_CPP);

        if (veryVerbose) puts("Non-numerical value tests");
        verifyToCharsNonNumericFloatingPointValues<float,
                                                   ToCharsCallNoFormat>();
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // 'double' MINIMAL CONVERSION TEST
        //
        // Concerns:
        //: 1 Writing chooses the right format and writes the right number.
        //:
        //: 2 No buffer overrun in writing to the output, regardless of the
        //:   provided output buffer size.
        //:
        //: 3 Zero is returned when the specified output buffer is too short,
        //:   and the output buffer is unchanged (i.e., no text written).
        //:
        //: 4 A pointer one past the last written character is returned when
        //:   the conversion to text is successful.
        //
        // Plan:
        //: 1 Use a table of values with expected output.
        //:
        //: 2 Use a one-longer-than-max output buffer to have space to
        //:
        //:   1 Enable 'ASSERTV'-printing by null terminating the buffer.
        //:
        //:   2 Pre-fill the complete (oversized) buffer by a non-printable,
        //:     non-null character and verify after the call to 'toChars' what
        //:     characters have been written to, and if an immediate (as in
        //:     immediately-following-character) buffer overrun has occurred.
        //:
        //: 3 Attempt several conversions on every table row:
        //:
        //:   1 With 0 pointers for 'first' and 'last' arguments, must always
        //:     fail.
        //:
        //:   2 With the same non-0 pointer  for 'first' and 'last' arguments
        //:    (zero output length), must always fail.
        //:
        //:   3 With expected size-1 output length, must always fail.
        //:
        //:   4 With the exact expected size for output length, must always
        //:     succeed.
        //:
        //:   5 With the maximum necessary buffer length (24) for output
        //:     length, must always succeed.
        //:
        //: 4 Verify the output of expected-to-be-successful conversions by
        //:    comparing them to the expected result from the table.
        //:
        //: 5 When the corresponding 'std::to_chars' is present use it as an
        //:   oracle, and verify the successful result against that as well.
        //:
        //: 6 Use preprocessor stringification to ensure accurate printing of
        //:   intended input in (test error) messages.
        //:
        //: 7 To avoid data duplication when seeing a positive value test its
        //:   negative counterpart automatically.
        //
        // Testing:
        //   char *toChars(char *first, char *last, double value);
        // --------------------------------------------------------------------

        if (verbose) printf("'double' MINIMAL CONVERSION TEST\n"
                            "================================\n");

        using namespace u; // Everything in this case is delegated to templates

        if (veryVerbose) puts("Testing numbers");
        verifyFloatingPointToChars<double, ToCharsCallNoFormat>(k_DBL_CPP);

        if (veryVerbose) puts("Non-numerical value tests");
        verifyToCharsNonNumericFloatingPointValues<double,
                                                   ToCharsCallNoFormat>();
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // RYU 'float' "MINIMAL" FORMAT TEST
        //  Ensure that our modifications and refactoring to the Ryu source
        //  code that perform the equivalent of the ISO C++ 'std::to_chars'
        //  decimal round tripping conversion (no precision, or format argument
        //  is used) are correct.
        //
        // Concerns:
        //: 1 Scientific format (when chosen) follows the C++ formatting rules
        //:   by writing a minimum of 2 exponent digits (with a leading 0 when
        //:   necessary), and by always writing the sign of the exponent
        //:   (even for positive exponents).
        //:
        //: 2 Leading negative sign is written for negative values, including
        //:   negative zero.
        //:
        //: 3 No leading positive sign is written for positive values, or zero.
        //:
        //: 4 Choice between writing decimal or scientific notation results in
        //:   the shorter of the two forms, decimal format preferred at a tie.
        //:
        //: 5 No more than 15 characters are written for any input value.
        //:
        //: 6 No characters are written beyond the reported length.
        //:
        //: 7 All characters within the reported length are (over)written.
        //:
        //: 8 The newly-developed decimal notation code is correct:
        //:
        //:   1 writes all necessary fraction digits for round tripping, but no
        //:     no more (no trailing zeros or fractional digits that do not
        //:     contribute to binary floating point value disambiguation)
        //:
        //:   2 writes all significant integer digits (no leading zeros)
        //:
        //:   3 writes no decimal separator (a.k.a., radix mark) for integral
        //:     values
        //:
        //:   4 trailing zeros are added for integers only when the exponent
        //:     requires more digit than the number of significant integer
        //:     decimal digits stored in the binary floating point type
        //:
        //:   5 writes the decimal separator for values with fractions, and in
        //:     the proper place
        //:
        //:   6 at least one integer digit is written before the decimal point,
        //:     or in other words if the absolute value is less than 1 a single
        //:     zero digit is written before the decimal point
        //:
        //:   7 leading zeros are added for the decimal fraction, after the
        //:     decimal separator, when the fraction value is less than 0.1
        //:     (or one tenth)
        //:
        //:   8 the fraction leading zeros (C8.7) are also added for numbers
        //:     larger than 1 in absolute value (REGRESSION)
        //
        // Plan:
        //: 1 Use a table of values with expected output.
        //:
        //: 2 Use a one-longer-than-max output buffer to have space to
        //:
        //:   1 Enable 'ASSERTV'-printing by null terminating the buffer.
        //:
        //:   2 Pre-fill the complete (oversized) buffer by a non-printable,
        //:     non-null character and verify after the call to 'toChars' what
        //:     characters have been written to, and if an immediate (as in
        //:     immediately-following-character) buffer overrun has occurred.
        //:
        //: 3 Verify the choice-of-format by determining the length of the
        //:   other format (not chosen), then verifying that the other format
        //:   is longer (when scientific notation was chosen) or not shorter
        //:   (in case decimal notation was chosen).
        //:
        //: 4 Verify output by comparing to the expected result from the table.
        //:
        //: 5 When the corresponding 'std::to_chars' is present use it as an
        //:   oracle, and verify the result against it as well.
        //:
        //: 6 Use preprocessor stringification to ensure accurate printing of
        //:   intended input in (test error) messages.
        //:
        //: 7 To avoid data duplication when seeing a positive value test its
        //:   negative counterpart automatically.
        //
        // Testing:
        //   int blp_f2m_buffered_n(float, char *);
        // --------------------------------------------------------------------

        if (verbose) printf("RYU 'float' \"MINIMAL\" TEST\n"
                            "==========================\n");

        using namespace u; // Everything in this case is delegated to templates

        if (veryVerbose) puts("Testing ordinary numbers");
        verifyRyuCall<float, RyuCallMinimal>(k_FLT_CPP);

        if (veryVerbose) puts("Precisely written integer tests");
        verifyRyuCallInt<float, RyuCallMinimal>(k_FLT_CPP_INT);

        if (veryVerbose) puts("Non-numerical value tests");
        verifyToCharsNonNumericFloatingPointValues<float,
                                                   ToCharsCallNoFormat>();
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // RYU 'double' "MINIMAL" FORMAT TEST
        //  Ensure that our modifications and refactoring to the Ryu source
        //  code that perform the equivalent of the ISO C++ 'std::to_chars'
        //  decimal round tripping conversion (no precision, or format argument
        //  is used) are correct.
        //
        // Concerns:
        //: 1 Scientific format (when chosen) follows the C++ formatting rules
        //:   by writing a minimum of 2 exponent digits (with a leading 0 when
        //:   necessary), and by always writing the sign of the exponent
        //:   (even for positive exponents).
        //:
        //: 2 Leading negative sign is written for negative values, including
        //:   negative zero.
        //:
        //: 3 No leading positive sign is written for positive values, or zero.
        //:
        //: 4 Choice between writing decimal or scientific notation results in
        //:   the shorter of the two forms, decimal format preferred at a tie.
        //:
        //: 5 No more than 24 characters written for any input value.
        //:
        //: 6 No characters are written beyond the reported length.
        //:
        //: 7 All characters within the reported length are (over)written.
        //:
        //: 8 The newly-developed decimal notation code is correct:
        //:
        //:   1 writes all necessary fraction digits for round tripping, but no
        //:     no more (no trailing zeros or fractional digits that do not
        //:     contribute to binary floating point value disambiguation)
        //:
        //:   2 writes all significant integer digits (no leading zeros)
        //:
        //:   3 writes no decimal separator (a.k.a., radix mark) for integral
        //:     values
        //:
        //:   4 trailing zeros are added for integers only when the exponent
        //:     requires more digit than the number of significant integer
        //:     decimal digits stored in the binary floating point type
        //:
        //:   5 writes the decimal separator for values with fractions, and in
        //:     the proper place
        //:
        //:   6 at least one integer digit is written before the decimal point,
        //:     or in other words if the absolute value is less than 1 a single
        //:     zero digit is written before the decimal point
        //:
        //:   7 leading zeros are added for the decimal fraction, after the
        //:     decimal separator, when the fraction value is less than 0.1
        //:     (or one tenth)
        //:
        //:   8 the fraction leading zeros (8.7) are also added for numbers
        //:     larger than 1 in absolute value (REGRESSION)
        //
        // Plan:
        //: 1 Use a table of values with expected output.
        //:
        //: 2 Use a one-longer-than-max output buffer to have space to
        //:
        //:   1 Enable 'ASSERTV'-printing by null terminating the buffer.
        //:
        //:   2 Pre-fill the complete (oversized) buffer by a non-printable,
        //:     non-null character and verify after the call to 'toChars' what
        //:     characters have been written to, and if an immediate (as in
        //:     immediately-following-character) buffer overrun has occurred.
        //:
        //: 3 Verify the choice-of-format by determining the length of the
        //:   other format (not chosen), then verifying that the other format
        //:   is longer (when scientific notation was chosen) or not shorter
        //:   (in case decimal notation was chosen).
        //:
        //: 4 Verify output by comparing to the expected result from the table.
        //:
        //: 5 When the corresponding 'std::to_chars' is present use it as an
        //:   oracle, and verify the result against it as well.
        //:
        //: 6 Use preprocessor stringification to ensure accurate printing of
        //:   intended input in (test error) messages.
        //:
        //: 7 To avoid data duplication when seeing a positive value test its
        //:   negative counterpart automatically.
        //
        // Testing:
        //   int blp_d2m_buffered_n(double, char *);
        // --------------------------------------------------------------------

        if (verbose) printf("RYU 'double' \"MINIMAL\" TEST\n"
                            "===========================\n");

        using namespace u; // Everything in this case is delegated to templates

        if (veryVerbose) puts("Testing ordinary numbers");
        verifyRyuCall<double, RyuCallMinimal>(k_DBL_CPP);

        if (veryVerbose) puts("Precisely written integer tests");
        verifyRyuCallInt<double, RyuCallMinimal>(k_DBL_CPP_INT);

        if (veryVerbose) puts("Non-numerical value tests");
        verifyToCharsNonNumericFloatingPointValues<double,
                                                   ToCharsCallNoFormat>();
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // RYU 'float' DECIMAL NOTATION TEST
        //  Ensure that our modifications and refactoring to the Ryu source
        //  code that perform the equivalent of the ISO C++ 'std::to_chars'
        //  decimal round tripping conversion (no precision, format argument is
        //  'std::chars_format::fixed') are correct.
        //
        // Concerns:
        //: 1 Leading negative sign is written for negative values, including
        //:   negative zero.
        //:
        //: 2 No leading positive sign is written for positive values, or zero.
        //:
        //: 3 No more than 48 characters are written for any input value.
        //:
        //: 4 No characters are written beyond the reported length.
        //:
        //: 5 All characters within the reported length are (over)written.
        //:
        //: 6 The newly-developed decimal notation code is correct:
        //:
        //:   1 writes all necessary fraction digits for round tripping, but no
        //:     no more (no trailing zeros or fractional digits that do not
        //:     contribute to binary floating point value disambiguation)
        //:
        //:   2 writes all significant integer digits (no leading zeros)
        //:
        //:   3 writes no decimal separator (a.k.a., radix mark) for integral
        //:     values
        //:
        //:   4 trailing zeros are added for integers only when the exponent
        //:     requires more digit than the number of significant integer
        //:     decimal digits stored in the binary floating point type
        //:
        //:   5 writes the decimal separator for values with fractions, and in
        //:     the proper place
        //:
        //:   6 at least one integer digit is written before the decimal point,
        //:     or in other words if the absolute value is less than 1 a single
        //:     zero digit is written before the decimal point
        //:
        //:   7 leading zeros are added for the decimal fraction, after the
        //:     decimal separator, when the fraction value is less than 0.1
        //:     (or one tenth)
        //:
        //:   8 the fraction leading zeros (C8.7) are also added for numbers
        //:     larger than 1 in absolute value (REGRESSION)
        //
        // Plan:
        //: 1 Use a table of values with expected output.
        //:
        //: 2 Use a one-longer-than-max output buffer to have space to
        //:
        //:   1 Enable 'ASSERTV'-printing by null terminating the buffer.
        //:
        //:   2 Pre-fill the complete (oversized) buffer by a non-printable,
        //:     non-null character and verify after the call to 'toChars' what
        //:     characters have been written to, and if an immediate (as in
        //:     immediately-following-character) buffer overrun has occurred.
        //:
        //: 3 Verify output by comparing to the expected result from the table.
        //:
        //: 4 When the corresponding 'std::to_chars' is present use it as an
        //:   oracle, and verify the result against it as well.
        //:
        //: 5 Use preprocessor stringification to ensure accurate printing of
        //:   intended input in (test error) messages.
        //:
        //: 6 To avoid data duplication when seeing a positive value test its
        //:   negative counterpart automatically.
        //
        // Testing:
        //   int blp_f2d_buffered_n(float, char *);
        // --------------------------------------------------------------------

        if (verbose) printf("RYU 'float' DECIMAL NOTATION TEST\n"
                            "=================================\n");

        using namespace u; // Everything in this case is delegated to templates

        if (veryVerbose) puts("Testing ordinary numbers");
        verifyRyuCall<float, RyuCallDecimal>(k_FLT_DEC);

        if (veryVerbose) puts("Precisely written integer tests");
        verifyRyuCallInt<float, RyuCallDecimal>(k_FLT_DEC_INT);

        if (veryVerbose) puts("Non-numerical value tests");
        verifyRyuNonNumericFloatingPointValues<float, RyuCallDecimal>();
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // RYU 'double' DECIMAL NOTATION TEST
        //  Ensure that our modifications and refactoring to the Ryu source
        //  code that perform the equivalent of the ISO C++ 'std::to_chars'
        //  decimal round tripping conversion (no precision, format argument is
        //  'std::chars_format::fixed') are correct.
        //
        // Concerns:
        //: 1 Leading negative sign is written for negative values, including
        //:   negative zero.
        //:
        //: 2 No leading positive sign is written for positive values, or zero.
        //:
        //: 3 No more than 327 characters are written for any input value.
        //:
        //: 4 No characters are written beyond the reported length.
        //:
        //: 5 All characters within the reported length are (over)written.
        //:
        //: 6 The newly-developed decimal notation code is correct:
        //:
        //:   1 writes all necessary fraction digits for round tripping, but no
        //:     no more (no trailing zeros or fractional digits that do not
        //:     contribute to binary floating point value disambiguation)
        //:
        //:   2 writes all significant integer digits (no leading zeros)
        //:
        //:   3 writes no decimal separator (a.k.a., radix mark) for integral
        //:     values
        //:
        //:   4 trailing zeros are added for integers only when the exponent
        //:     requires more digit than the number of significant integer
        //:     decimal digits stored in the binary floating point type
        //:
        //:   5 writes the decimal separator for values with fractions, and in
        //:     the proper place
        //:
        //:   6 at least one integer digit is written before the decimal point,
        //:     or in other words if the absolute value is less than 1 a single
        //:     zero digit is written before the decimal point
        //:
        //:   7 leading zeros are added for the decimal fraction, after the
        //:     decimal separator, when the fraction value is less than 0.1
        //:     (or one tenth)
        //:
        //:   8 the fraction leading zeros (C8.7) are also added for numbers
        //:     larger than 1 in absolute value (REGRESSION)
        //
        // Plan:
        //: 1 Use a table of values with expected output.
        //:
        //: 2 Use a one-longer-than-max output buffer to have space to
        //:
        //:   1 Enable 'ASSERTV'-printing by null terminating the buffer.
        //:
        //:   2 Pre-fill the complete (oversized) buffer by a non-printable,
        //:     non-null character and verify after the call to 'toChars' what
        //:     characters have been written to, and if an immediate (as in
        //:     immediately-following-character) buffer overrun has occurred.
        //:
        //: 3 Verify output by comparing to the expected result from the table.
        //:
        //: 4 When the corresponding 'std::to_chars' is present use it as an
        //:   oracle, and verify the result against it as well.
        //:
        //: 5 Use preprocessor stringification to ensure accurate printing of
        //:   intended input in (test error) messages.
        //:
        //: 6 To avoid data duplication when seeing a positive value test its
        //:   negative counterpart automatically.
        //
        // Testing:
        //   int blp_d2d_buffered_n(double, char *);
        // --------------------------------------------------------------------

        if (verbose) printf("RYU 'double' DECIMAL NOTATION TEST\n"
                            "==================================\n");

        using namespace u; // Everything in this case is delegated to templates

        if (veryVerbose) puts("Testing ordinary numbers");
        verifyRyuCall<double, RyuCallDecimal>(k_DBL_DEC);

        if (veryVerbose) puts("Precisely written integer tests");
        verifyRyuCallInt<double, RyuCallDecimal>(k_DBL_DEC_INT);

        if (veryVerbose) puts("Non-numerical value tests");
        verifyRyuNonNumericFloatingPointValues<double, RyuCallDecimal>();
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // RYU 'float' SCIENTIFIC NOTATION TEST
        //  Ensure that our modifications and refactoring to the Ryu source
        //  code that perform the equivalent of the ISO C++ 'std::to_chars'
        //  decimal round tripping conversion (no precision, format argument is
        //  'std::chars_format::scientific') are correct.
        //
        // Concerns:
        //: 1 Scientific format (when chosen) follows the C++ formatting rules
        //:   by writing a minimum of 2 exponent digits (with a leading 0 when
        //:   necessary), and by always writing the sign of the exponent
        //:   (even for positive exponents).
        //:
        //: 2 Leading negative sign is written for negative values, including
        //:   negative zero.
        //:
        //: 2 No leading positive sign is written for positive values, or zero.
        //:
        //: 3 No more than 15 characters are written for any input value.
        //:
        //: 4 No characters are written beyond the reported length.
        //:
        //: 5 All characters within the reported length are (over)written.
        //
        // Plan:
        //: 1 Use a table of values with expected output.
        //:
        //: 2 Use a one-longer-than-max output buffer to have space to
        //:
        //:   1 Enable 'ASSERTV'-printing by null terminating the buffer.
        //:
        //:   2 Pre-fill the complete (oversized) buffer by a non-printable,
        //:     non-null character and verify after the call to 'toChars' what
        //:     characters have been written to, and if an immediate (as in
        //:     immediately-following-character) buffer overrun has occurred.
        //:
        //: 3 Verify output by comparing to the expected result from the table.
        //:
        //: 4 When the corresponding 'std::to_chars' is present use it as an
        //:   oracle, and verify the result against it as well.
        //:
        //: 5 Use preprocessor stringification to ensure accurate printing of
        //:   intended input in (test error) messages.
        //:
        //: 6 To avoid data duplication when seeing a positive value test its
        //:   negative counterpart automatically.
        //
        // Testing:
        //   int blp_f2s_buffered_n(float, char *);
        // --------------------------------------------------------------------

        if (verbose) printf("RYU 'float' SCIENTIFIC NOTATION TEST\n"
                            "====================================\n");

        using namespace u; // Everything in this case is delegated to templates

        if (veryVerbose) puts("Testing numbers");
        verifyRyuCall<float, RyuCallScientific>(k_FLT_SCI);

        if (veryVerbose) puts("Non-numerical value tests");
        verifyRyuNonNumericFloatingPointValues<float, RyuCallScientific>();
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // RYU 'double' SCIENTIFIC NOTATION TEST
        //  Ensure that our modifications and refactoring to the Ryu source
        //  code that perform the equivalent of the ISO C++ 'std::to_chars'
        //  decimal round tripping conversion (no precision, format argument is
        //  'std::chars_format::scientific') are correct.
        //
        // Concerns:
        //: 1 Scientific format (when chosen) follows the C++ formatting rules
        //:   by writing a minimum of 2 exponent digits (with a leading 0 when
        //:   necessary), and by always writing the sign of the exponent
        //:   (even for positive exponents).
        //:
        //: 2 Leading negative sign is written for negative values, including
        //:   negative zero.
        //:
        //: 2 No leading positive sign is written for positive values, or zero.
        //:
        //: 3 No more than 24 characters are written for any input value.
        //:
        //: 4 No characters are written beyond the reported length.
        //:
        //: 5 All characters within the reported length are (over)written.
        //
        // Plan:
        //: 1 Use a table of values with expected output.
        //:
        //: 2 Use a one-longer-than-max output buffer to have space to
        //:
        //:   1 Enable 'ASSERTV'-printing by null terminating the buffer.
        //:
        //:   2 Pre-fill the complete (oversized) buffer by a non-printable,
        //:     non-null character and verify after the call to 'toChars' what
        //:     characters have been written to, and if an immediate (as in
        //:     immediately-following-character) buffer overrun has occurred.
        //:
        //: 3 Verify output by comparing to the expected result from the table.
        //:
        //: 4 When the corresponding 'std::to_chars' is present use it as an
        //:   oracle, and verify the result against it as well.
        //:
        //: 5 Use preprocessor stringification to ensure accurate printing of
        //:   intended input in (test error) messages.
        //:
        //: 6 To avoid data duplication when seeing a positive value test its
        //:   negative counterpart automatically.
        //
        // Testing:
        //   int blp_d2s_buffered_n(double, char *);
        // --------------------------------------------------------------------

        if (verbose) printf("RYU 'double' SCIENTIFIC NOTATION TEST\n"
                            "=====================================\n");

        using namespace u; // Everything in this case is delegated to templates

        if (veryVerbose) puts("Testing numbers");
        verifyRyuCall<double, RyuCallScientific>(k_DBL_SCI);

        if (veryVerbose) puts("Non-numerical value tests");
        verifyRyuNonNumericFloatingPointValues<double, RyuCallScientific>();
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // BLOOMBERG RYU INTERNALS
        //  Verify "generic" functions written for the Bloomberg extensions.
        //
        // Concerns:
        //: 1 'mantissa_countr_zero32' and 'mantissa_countr_zero64'
        //:   (counting trailing zeros in IEEE mantissa):
        //:   1 0 mantissa value results in number-of-mantissa-bits (23 for
        //:     'float', and 52 for 'double) plus one
        //:
        //:   2 non-zero values result in the number or trailing zeros (0-22
        //:     for 'float', and 0-51 for 'double)
        //:
        //:   3 non-trailing zeros do not matter
        //:
        //: 2 'is_integer32' and 'is_integer64':
        //:   1 Values with binary exponent larger or equal to IEEE
        //:     mantissa-bits (23 for 'float', 52 for 'double) are reported as
        //:     integers.  Binary exponent is the unbiased exponent
        //:     ('ieeeExponent - BIAS', where BIAS is 127 for 'float' and 1023
        //:     for 'double').
        //:
        //:   2 Values with positive effective exponent are reported as
        //:     integers.  Effective exponent is
        //:     'bin-exponent - MANTISSA_BITS + numof-mantissa-trailing-zeros'.
        //:
        //: 3 As the code is in C, we cannot do precondition tests
        //
        // Plan:
        //: 1 For C-1 use a loop:
        //:   1 start with  mantissa with all bits set to one and expected
        //:     number of trailing bits zero
        //:
        //:   2 run the loop until expected number of bits equals 53
        //:
        //:   3 In the loop:
        //:     1 verify that the counter function returns the expected number
        //:       1 for the number of trailing bits in the current mantissa
        //:
        //:       2 unless the current mantissa is zero:
        //:
        //:       3 the current mantissa modified to have only its least
        //:         significant bit set from the originally set bits
        //:
        //:       4 the current mantissa modified to have only its least
        //:         significant bit set from the originally set bits, and also
        //:         the most significant bit set
        //:
        //:     2 mask away the lowest set bit of the mantissa
        //:     3 increment the expected number of trailing bits
        //:
        //: 2 For C-2 use a similar loop as in C-1, but with an expected
        //:   exponent set next to the starter mantissa where the value becomes
        //:   an integer.  As we start with all bits set, this value is the
        //:   number of bits in the mantissa ('float' 23, 'double' 52).  Within
        //:   the loop run another loop for non-negative binary exponents and
        //:   some negative ones as well.  Verify that exponents less than the
        //:   expected exponent return 'false', while the expected exponent and
        //:   higher return 'true'.  To move the main loop unset the lowest set
        //:   bit of the mantissa and decrease the expected exponent.
        //:
        // Testing:
        //   BLOOMBERG RYU INTERNALS
        // --------------------------------------------------------------------

        if (verbose) printf("BLOOMBERG RYU INTERNALS\n"
                            "=======================\n");

        if (veryVerbose) puts("Verifying 'mantissa_countr_zero64'");
        {
            uint64_t mantissa = 0xFFFFFFFFFFFFFull;
            uint32_t expected = 0;
            while (expected < 52) {
                const uint64_t IEEE_MANTISSA = mantissa;
                const uint32_t EXPECTED      = expected;

                char MANTISSA_HEX[16];
                u::dblMantissaToHex(MANTISSA_HEX, IEEE_MANTISSA);

                if (veryVeryVerbose) {
                    P_(MANTISSA_HEX) P(EXPECTED);
                }

                uint32_t result = mantissa_countr_zero64(IEEE_MANTISSA);
                ASSERTV(EXPECTED, MANTISSA_HEX, result,
                        EXPECTED == result);

                if (IEEE_MANTISSA != 0) {
                    const uint64_t LSB_ONLY = IEEE_MANTISSA & -IEEE_MANTISSA;
                    u::dblMantissaToHex(MANTISSA_HEX, LSB_ONLY);
                    if (veryVeryVerbose) {
                        P_(MANTISSA_HEX) P(EXPECTED);
                    }
                    result = mantissa_countr_zero64(LSB_ONLY);
                    ASSERTV(MANTISSA_HEX, EXPECTED, result,
                            EXPECTED == result);

                    const uint64_t LSB_MSB = LSB_ONLY | 0x8000000000000ull;
                    u::dblMantissaToHex(MANTISSA_HEX, LSB_MSB);
                    result = mantissa_countr_zero64(LSB_MSB);
                    ASSERTV(MANTISSA_HEX, EXPECTED, result,
                            EXPECTED == result);
                }
                const uint64_t MASK = ~(IEEE_MANTISSA & -IEEE_MANTISSA);
                mantissa = MASK & mantissa;
                ++expected;
            }
        }

        if (veryVerbose) puts("Verifying 'mantissa_countr_zero32'");
        {
            uint32_t mantissa = 0x7FFFFF;
            uint32_t expected = 0;
            while (expected < 23) {
                const uint32_t IEEE_MANTISSA = mantissa;
                const uint32_t EXPECTED      = expected;

                // '%06X' may theoretically write 8 characters so compilers
                // will warn if the array below is less than 11: 0x + 8 + null.
                char IEEE_MANTISSA_STR[11];
                sprintf(IEEE_MANTISSA_STR, "0x%06X", IEEE_MANTISSA);

                if (veryVeryVerbose) {
                    P_(IEEE_MANTISSA_STR) P(EXPECTED);
                }

                uint32_t result = mantissa_countr_zero32(IEEE_MANTISSA);
                ASSERTV(EXPECTED, IEEE_MANTISSA_STR, result,
                        EXPECTED == result);

                if (IEEE_MANTISSA != 0) {
                    const uint32_t LSB_ONLY = IEEE_MANTISSA & -IEEE_MANTISSA;
                    sprintf(IEEE_MANTISSA_STR, "0x%06X", LSB_ONLY);
                    if (veryVeryVerbose) {
                        P_(IEEE_MANTISSA_STR) P(EXPECTED);
                    }
                    result = mantissa_countr_zero64(LSB_ONLY);
                    ASSERTV(IEEE_MANTISSA_STR, EXPECTED, result,
                            EXPECTED == result);

                    const uint32_t LSB_MSB = LSB_ONLY | 0x400000;
                    sprintf(IEEE_MANTISSA_STR, "0x%06X", LSB_MSB);
                    result = mantissa_countr_zero32(LSB_MSB);
                    ASSERTV(IEEE_MANTISSA_STR, EXPECTED, result,
                            EXPECTED == result);
                }
                const uint32_t MASK = ~(IEEE_MANTISSA & -IEEE_MANTISSA);
                mantissa = MASK & mantissa;
                ++expected;
            }
        }

        if (veryVerbose) puts("Verifying 'is_integer64'");
        {
            uint64_t mantissa            = 0xFFFFFFFFFFFFFull;
            int32_t  lowest_int_exponent = 52;
            while (lowest_int_exponent > 0) {
                const uint64_t IEEE_MANTISSA = mantissa;
                char MANTISSA_HEX[16];
                u::dblMantissaToHex(MANTISSA_HEX, IEEE_MANTISSA);

                if (veryVeryVerbose) {
                    P_(MANTISSA_HEX) P(lowest_int_exponent);
                }

                for (int32_t exponent = -1022; exponent < 1024; ++exponent) {
                    const bool EXPECTED = (exponent >= lowest_int_exponent);

                    if (veryVeryVerbose) {
                        T_ P_(exponent)  P(EXPECTED);
                    }

                    const bool result = is_integer64(IEEE_MANTISSA,
                                                     exponent + DOUBLE_BIAS);
                    ASSERTV(EXPECTED, MANTISSA_HEX, exponent,
                           EXPECTED == result);
                }

                const uint64_t MASK = ~(IEEE_MANTISSA & -IEEE_MANTISSA);
                mantissa = MASK & mantissa;
                --lowest_int_exponent;
            }
        }

        if (veryVerbose) puts("Verifying 'is_integer32'");
        {
            uint32_t mantissa            = 0x7FFFFF;
            int32_t  lowest_int_exponent = 23;
            while (lowest_int_exponent > 0) {
                const uint32_t IEEE_MANTISSA = mantissa;
                char IEEE_MANTISSA_STR[9];
                sprintf(IEEE_MANTISSA_STR, "0x%06X", IEEE_MANTISSA);

                if (veryVeryVerbose) {
                    P_(IEEE_MANTISSA_STR) P(lowest_int_exponent);
                }

                for (int32_t exponent = -126; exponent < 127; ++exponent) {
                    const bool EXPECTED = (exponent >= lowest_int_exponent);

                    if (veryVeryVerbose) {
                        T_ P_(exponent)  P(EXPECTED);
                    }

                    const bool result = is_integer32(IEEE_MANTISSA,
                                                     exponent + FLOAT_BIAS);
                    ASSERTV(EXPECTED, IEEE_MANTISSA_STR, exponent,
                           EXPECTED == result);
                }

                const uint32_t MASK = ~(IEEE_MANTISSA & -IEEE_MANTISSA);
                mantissa = MASK & mantissa;
                --lowest_int_exponent;
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // RANDOM VALUES TEST -- UNSIGNED TYPES
        //
        // Concerns:
        //: 1 That the function under test performs well when translating
        //:   random values.
        //
        // Plan:
        //: 1 Use an MMIX-style random number generator to generate random
        //:   values.
        //:
        //: 2 This test tests the function when called with all unsigned
        //:   fundamental types.
        //:
        //: 3 Each random value is tested with all bases in the range
        //:   '[ 2 .. 36 ]', which is all bases supported by the function under
        //:   test.
        //
        // Testing:
        //   char *toChars(char *, char *, char,                   int);
        //   char *toChars(char *, char *, unsigned char,          int);
        //   char *toChars(char *, char *, unsigned short,         int);
        //   char *toChars(char *, char *, unsigned int,           int);
        //   char *toChars(char *, char *, unsigned long,          int);
        //   char *toChars(char *, char *, unsigned long long int, int);
        // --------------------------------------------------------------------

        if (verbose) printf("RANDOM VALUES TEST -- UNSIGNED TYPES\n"
                            "====================================\n");

        Uint64 iterationsForByte = 128;
        if (verbose) {
            Uint64 it = std::atoi(argv[2]);
            if (it) {
                iterationsForByte = it;
                P(iterationsForByte);
            }
        }

        // do the unsigned types

#if CHAR_MIN == 0
        u::randIntTest<char>(iterationsForByte);
#endif
        u::randIntTest<unsigned char>( iterationsForByte);
        u::randIntTest<unsigned short>(iterationsForByte);
        u::randIntTest<unsigned int>(  iterationsForByte);
        u::randIntTest<Uint64>(        iterationsForByte);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // RANDOM VALUES TEST -- SIGNED TYPES
        //
        // Concerns:
        //: 1 That the function under test performs well when translating
        //:   random values.
        //
        // Plan:
        //: 1 Use an MMIX-style random number generator to generate random
        //:   values.
        //:
        //: 2 This test tests the function when called with all signed
        //:   fundamental types.
        //:
        //: 3 Each random value is tested with all bases in the range
        //:   '[ 2 .. 36 ]', which is all bases supported by the function under
        //:   test.
        //
        // Testing:
        //   char *toChars(char *, char *, char,                 int);
        //   char *toChars(char *, char *, signed char,          int);
        //   char *toChars(char *, char *, short,                int);
        //   char *toChars(char *, char *, int,                  int);
        //   char *toChars(char *, char *, long,                 int);
        //   char *toChars(char *, char *, signed long long int, int);
        // --------------------------------------------------------------------

        if (verbose) printf("RANDOM VALUES TEST -- SIGNED TYPES\n"
                            "==================================\n");

        Uint64 iterationsForByte = 128;
        if (verbose && argc >= 3) {
            Uint64 it = std::atoi(argv[2]);
            if (it) {
                iterationsForByte = it;
                P(iterationsForByte);
            }
        }

        // do the signed types

#if CHAR_MIN != 0
        u::randIntTest<char>(       iterationsForByte);
#endif
        u::randIntTest<signed char>(iterationsForByte);
        u::randIntTest<short>(      iterationsForByte);
        u::randIntTest<int>(        iterationsForByte);
        u::randIntTest<Int64>(      iterationsForByte);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // INTEGER CORNER CASES TEST
        //
        // Concerns:
        //: 1 The function works as expected for all "interesting" corner cases
        //:   of input value and base where "interesting" for a given base is:
        //:
        //:   o All input values that are a power of the given base:
        //:     '[ 1, base, base**2, base**3, ... ceiling ]' and 'ceiling' is
        //:     the largest value in the series representable by the 'Uint64'
        //:     type.
        //:
        //:   o A value of 0.
        //:
        //:   o The maximum and minimum values that can be represented by any
        //:     integral fundamental type.
        //:
        //:   o Also, 1 more and 1 less that each input value in the above
        //:     series.  Note the variable in question is unsigned, and in some
        //:     cases adding or subtracting 1 may overflow or underflow, but
        //:     this is not undefined behavior.
        //:
        //: 2 The function works as expected for the series (see C-1) for every
        //:   supported base (i.e., '[2 .. 36]').
        //:
        //: 3 The function works as expected for:
        //:
        //:   o Each of the supported integer types:
        //:     o 'char'
        //:     o 'signed char'
        //:     o 'unsigned char'
        //:     o 'short'
        //:     o 'unsigned short'
        //:     o 'int'
        //:     o 'unsigned int'
        //:     o 'Uint64'
        //:     o 'Int64'
        //:
        //:   o And, for signed types each of the input values in the test
        //:     series is also tested for its negated value
        //:
        //:   o And, the input values of each type are curtailed for the
        //:     maximum (and minimum) value of the type.
        //
        // Plan:
        //: 1 Have a function 'testIntValueMeta' that will assign the 'value'
        //:   passed to it to all integral types capable of holding it, and
        //:   also do the negative value, if the type is a signed type, and
        //:   then call 'toChar's to print out the variables to strings, and
        //:   check that the strings output are correct.
        //:
        //: 2 For good measure, for each base we print the strings with, we
        //:   test all powers of all supported bases.
        //:
        //: 3 For each base, test 0.
        //:
        //: 4 For each base, for the size of every integral type from 'char' to
        //:   'Uint64', test the maximum and minimum values the type can
        //:   represent.
        //:
        //: 5 For each value 'N' that is tested, also test 'N + 1' and 'N - 1'.
        //
        // Testing:
        //   char *toChars(char *, char *, char,                   int);
        //   char *toChars(char *, char *, signed char,            int);
        //   char *toChars(char *, char *, unsigned char,          int);
        //   char *toChars(char *, char *, short,                  int);
        //   char *toChars(char *, char *, unsigned short,         int);
        //   char *toChars(char *, char *, int,                    int);
        //   char *toChars(char *, char *, unsigned,               int);
        //   char *toChars(char *, char *, long,                   int);
        //   char *toChars(char *, char *, unsigned long,          int);
        //   char *toChars(char *, char *, signed long long int,   int);
        //   char *toChars(char *, char *, unsigned long long int, int);
        // --------------------------------------------------------------------

        if (verbose) printf("INTEGER CORNER CASES TEST\n"
                            "=========================\n");

#if defined(BSLALG_NUMERICFORMATTERUTIL_TEST_CALLING_BOOL_OVERLOAD)
        if (verbose) printf("The following should not compile:\n");
        {
            bool b = true;
            char *p = Util::toChars(0, 0, b);
            char* p = Util::toChars(0, 0, b, 16);
        }
#endif

        for (int base = 2; base <= 36; ++base) {
            if (veryVerbose) { T2_; P_(base); Q(zero:); }

            u::testIntValueMeta(       -1,               base);
            u::testIntValueMeta(        0,               base);
            u::testIntValueMeta(        1,               base);

            if (veryVerbose) { T2_; P_(base); Q(escalation loop:); }

            for (int escalation = 2; escalation <= 36; ++escalation) {
                Uint64 ceiling = ~0ULL / escalation;

                if (veryVeryVerbose) { T2_; T2_; P_(escalation); P(ceiling); }

                for (Uint64 ii = 1, prev = 0; prev <= ceiling;
                                                 prev = ii, ii *= escalation) {
                    if (veryVeryVeryVerbose) {
                        T2_; T2_; P(ii);
                    }

                    u::testIntValueMeta(ii - 1,          base);
                    u::testIntValueMeta(ii,              base);
                    u::testIntValueMeta(ii + 1,          base);
                }
            }

            if (veryVerbose) { T2_; P_(base); Q(max min loop:); }

            for (int bits = 8; bits <= 64; bits *= 2) {
                const Uint64 maxUnsigned = bits < 64
                                         ? (1ULL << bits) - 1
                                         : ~0ULL;
                const Uint64 minSigned   = 1ULL << (bits - 1);
                const Int64  sMinSigned  = minSigned;

                if (veryVeryVerbose) {
                    printf("    maxUnsigned: %llu = 0x%llx\n",
                                                     maxUnsigned, maxUnsigned);
                    printf("    minSigned:   %lld = 0x%llx\n",
                                                        sMinSigned, minSigned);
                }

                u::testIntValueMeta(    maxUnsigned - 1, base);
                u::testIntValueMeta(    maxUnsigned,     base);    // UINT_MAX
                u::testIntValueMeta(    maxUnsigned + 1, base);    // 0

                u::testIntValueMeta(    minSigned   - 1, base);    // INT_MAX
                u::testIntValueMeta(    minSigned,       base);    // INT_MIN
                u::testIntValueMeta(    minSigned   + 1, base);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // INT64 QUALITY & ACCURACY ON SOME BASES TEST
        //
        // Concern:
        //: 1 That, in all supported bases,
        //:   o negative numbers are always output beginning with '-'.
        //:
        //:   o positive numbers don't contain '-'.
        //:
        //:   o output other than an appropriate beginning '-' consists
        //:     entirely of digits in the range '[ '0' ..  <base> )', where
        //:     digits above '9' are represented by alphabetical characters.
        //:
        //:   o that a negative number is output as the same string as the
        //:     corresponding positive number, except with a leading '-' added.
        //:
        //:   o that the number of digits output is appropriate to the log of
        //:     the value in the given base.
        //:
        //:   o that an empty output range results in a null pointer returned
        //:
        //: 2 That the function under test can accurately output decimal, hex,
        //:   and octal strings of given values.
        //
        // Plan:
        //: 1 Have a table with positive constants in both string form and
        //:   parsed as integral types, as either decimal, hex, or octal
        //:   source, and a field indicating the base of the source number.
        //:
        //: 2 Iterate through the table of numbers.
        //:   o Iterate through all bases in the range '[ 2 .. 36 ]'.
        //
        //:   o Verify that a pair of null pointers for output results in
        //:     failure of conversion (null returned).  Verify that specifying
        //:     an empty range at the beginning of a buffer results in
        //:     conversion failure *and* the content of the buffer remains
        //:     unchanged.
        //:
        //:   o Output the numbers and observe that they contain only digits
        //:     in the range '[ 0 .. <base> )'.
        //:
        //:   o If the number is not 0, observe that the number of digits
        //:     output is as expected for the size of the number and the base
        //:     of the output.
        //:
        //:   o Assign the value to a signed type.  Output it again and observe
        //:     we get the same result.
        //:
        //:   o Negate the signed type and output it again, and observe that
        //:     we got the same value preceded by '-'.
        //:
        //:   o If we are on a platform that supports '<charconv>', apply
        //:     'std::to_chars' to the unsigned and signed values and observe
        //:     that the output strings match.
        //:
        //: 3 Iterate through the table of numbers.  For each number:
        //:   o write the number with both 'sprintf' and 'toChars' as unsigned
        //:     decimal, hex, and octal strings, and using 'sprintf' as an
        //:     "oracle" for comparing the resulting strings for perfect
        //:     accuracy.
        //:
        //:   o Assign it to a signed type, with it with both 'sprintf' and
        //:     'toChars', and observe they match to test for perfect
        //:     accuracy.  Then negate the signed type and repeat the
        //:     experiment.
        //:
        //: 4 Iterate through the table and for unsigned decimal, hex, or
        //:   octal, if the string in the table is of that radix, compare the
        //:   string in the table with the string generated by 'toChars' to
        //:   check that they match for perfect accuracy.
        //:
        //: 5 Note that there is some redundancy of testing 0 multiple times,
        //:   but this was maintained to preserve the symmetry of the test
        //:   pattern.
        //
        // Testing:
        //   char *toChars(char *, char *, signed long long int,   int);
        //   char *toChars(char *, char *, unsigned long long int, int);
        // --------------------------------------------------------------------

        if (verbose) printf("INT64 QUALITY & ACCURACY ON SOME BASES TEST\n"
                            "===========================================\n");

#undef  D
#undef  H
#undef  O

#define D(s)    #s,       s ## ULL, 10
#define H(s)    #s, 0x ## s ## ULL, 16
#define O(s)    #s,  0 ## s ## ULL,  8

        static const struct Data {
            int         d_line;
            const char *d_string_p;
            Uint64      d_value;
            unsigned    d_base;
        } DATA[] = {
            { L_, H(0) },
            { L_, H(1) },
            { L_, H(2) },
            { L_, H(4) },
            { L_, H(8) },
            { L_, H(10) },
            { L_, H(20) },
            { L_, H(40) },
            { L_, H(80) },
            { L_, H(100) },
            { L_, H(200) },
            { L_, H(400) },
            { L_, H(800) },
            { L_, H(1000) },
            { L_, H(2000) },
            { L_, H(4000) },
            { L_, H(8000) },
            { L_, H(10000) },
            { L_, H(20000) },
            { L_, H(40000) },
            { L_, H(80000) },
            { L_, H(100000) },
            { L_, H(200000) },
            { L_, H(400000) },
            { L_, H(800000) },
            { L_, H(1000000) },
            { L_, H(2000000) },
            { L_, H(4000000) },
            { L_, H(8000000) },
            { L_, H(10000000) },
            { L_, H(20000000) },
            { L_, H(40000000) },
            { L_, H(80000000) },
            { L_, H(100000000) },
            { L_, H(200000000) },
            { L_, H(400000000) },
            { L_, H(800000000) },
            { L_, H(1000000000) },
            { L_, H(2000000000) },
            { L_, H(4000000000) },
            { L_, H(8000000000) },
            { L_, H(10000000000) },
            { L_, H(20000000000) },
            { L_, H(40000000000) },
            { L_, H(80000000000) },
            { L_, H(100000000000) },
            { L_, H(200000000000) },
            { L_, H(400000000000) },
            { L_, H(800000000000) },
            { L_, H(1000000000000) },
            { L_, H(2000000000000) },
            { L_, H(4000000000000) },
            { L_, H(8000000000000) },
            { L_, H(10000000000000) },
            { L_, H(20000000000000) },
            { L_, H(40000000000000) },
            { L_, H(80000000000000) },
            { L_, H(100000000000000) },
            { L_, H(200000000000000) },
            { L_, H(400000000000000) },
            { L_, H(800000000000000) },
            { L_, H(1000000000000000) },
            { L_, H(2000000000000000) },
            { L_, H(4000000000000000) },
            { L_, H(8000000000000000) },

            { L_, O(0) },
            { L_, O(1) },
            { L_, O(2) },
            { L_, O(4) },
            { L_, O(10) },
            { L_, O(20) },
            { L_, O(40) },
            { L_, O(100) },
            { L_, O(200) },
            { L_, O(400) },
            { L_, O(1000) },
            { L_, O(2000) },
            { L_, O(4000) },
            { L_, O(10000) },
            { L_, O(20000) },
            { L_, O(40000) },
            { L_, O(100000) },
            { L_, O(200000) },
            { L_, O(400000) },
            { L_, O(1000000) },
            { L_, O(2000000) },
            { L_, O(4000000) },
            { L_, O(10000000) },
            { L_, O(20000000) },
            { L_, O(40000000) },
            { L_, O(100000000) },
            { L_, O(200000000) },
            { L_, O(400000000) },
            { L_, O(1000000000) },
            { L_, O(2000000000) },
            { L_, O(4000000000) },
            { L_, O(10000000000) },
            { L_, O(20000000000) },
            { L_, O(40000000000) },
            { L_, O(100000000000) },
            { L_, O(200000000000) },
            { L_, O(400000000000) },
            { L_, O(1000000000000) },
            { L_, O(2000000000000) },
            { L_, O(4000000000000) },
            { L_, O(10000000000000) },
            { L_, O(20000000000000) },
            { L_, O(40000000000000) },
            { L_, O(100000000000000) },
            { L_, O(200000000000000) },
            { L_, O(400000000000000) },
            { L_, O(1000000000000000) },
            { L_, O(2000000000000000) },
            { L_, O(4000000000000000) },
            { L_, O(10000000000000000) },
            { L_, O(20000000000000000) },
            { L_, O(40000000000000000) },
            { L_, O(100000000000000000) },
            { L_, O(200000000000000000) },
            { L_, O(400000000000000000) },
            { L_, O(1000000000000000000) },
            { L_, O(2000000000000000000) },
            { L_, O(4000000000000000000) },
            { L_, O(10000000000000000000) },
            { L_, O(20000000000000000000) },
            { L_, O(40000000000000000000) },
            { L_, O(100000000000000000000) },
            { L_, O(200000000000000000000) },
            { L_, O(400000000000000000000) },
            { L_, O(1000000000000000000000) },

            { L_, D(0) },
            { L_, D(1) },
            { L_, D(10) },
            { L_, D(100) },
            { L_, D(1000) },
            { L_, D(10000) },
            { L_, D(100000) },
            { L_, D(1000000) },
            { L_, D(10000000) },
            { L_, D(100000000) },
            { L_, D(1000000000) },
            { L_, D(10000000000) },
            { L_, D(100000000000) },
            { L_, D(1000000000000) },
            { L_, D(10000000000000) },
            { L_, D(100000000000000) },
            { L_, D(1000000000000000) },
            { L_, D(10000000000000000) },
            { L_, D(100000000000000000) },
            { L_, D(1000000000000000000) },

            { L_, H(1) },
            { L_, H(12) },
            { L_, H(123) },
            { L_, H(1234) },
            { L_, H(12345) },
            { L_, H(123456) },
            { L_, H(1234567) },
            { L_, H(12345678) },
            { L_, H(123456789) },
            { L_, H(1234567890) },
            { L_, H(12345678901) },
            { L_, H(123456789012) },
            { L_, H(1234567890123) },
            { L_, H(12345678901234) },
            { L_, H(123456789012345) },
            { L_, H(1234567890123456) },

            { L_, H(9) },
            { L_, H(98) },
            { L_, H(987) },
            { L_, H(9876) },
            { L_, H(98765) },
            { L_, H(987654) },
            { L_, H(9876543) },
            { L_, H(98765432) },
            { L_, H(987654321) },
            { L_, H(9876543219) },
            { L_, H(98765432198) },
            { L_, H(987654321987) },
            { L_, H(9876543219876) },
            { L_, H(98765432198765) },
            { L_, H(987654321987654) },
            { L_, H(9876543219876543) },

            { L_, H(f) },
            { L_, H(fe) },
            { L_, H(fed) },
            { L_, H(fedc) },
            { L_, H(fedcb) },
            { L_, H(fedcba) },
            { L_, H(fedcba9) },
            { L_, H(fedcba98) },
            { L_, H(fedcba987) },
            { L_, H(fedcba9876) },
            { L_, H(fedcba98765) },
            { L_, H(fedcba987654) },
            { L_, H(fedcba9876543) },
            { L_, H(fedcba98765432) },
            { L_, H(fedcba987654321) },
            { L_, H(fedcba9876543210) },

            { L_, D(1) },
            { L_, D(12) },
            { L_, D(123) },
            { L_, D(1234) },
            { L_, D(12345) },
            { L_, D(123456) },
            { L_, D(1234567) },
            { L_, D(12345678) },
            { L_, D(123456789) },
            { L_, D(1234567890) },
            { L_, D(12345678901) },
            { L_, D(123456789012) },
            { L_, D(1234567890123) },
            { L_, D(12345678901234) },
            { L_, D(123456789012345) },
            { L_, D(1234567890123456) },
            { L_, D(12345678901234567) },
            { L_, D(123456789012345678) },
            { L_, D(1234567890123456789) },
            { L_, D(12345678901234567890) },

            { L_, O(7) },
            { L_, O(76) },
            { L_, O(765) },
            { L_, O(7654) },
            { L_, O(76543) },
            { L_, O(765432) },
            { L_, O(7654321) },
            { L_, O(76543210) },
            { L_, O(765432107) },
            { L_, O(7654321076) },
            { L_, O(76543210765) },
            { L_, O(765432107654) },
            { L_, O(7654321076543) },
            { L_, O(76543210765432) },
            { L_, O(765432107654321) },
            { L_, O(7654321076543210) },
            { L_, O(76543210765432107) },
            { L_, O(765432107654321076) },
            { L_, O(7654321076543210765) },
            { L_, O(76543210765432107654) },
            { L_, O(765432107654321076543) },

            { L_, O(1) },
            { L_, O(12) },
            { L_, O(123) },
            { L_, O(1234) },
            { L_, O(12345) },
            { L_, O(123456) },
            { L_, O(1234567) },
            { L_, O(12345670) },
            { L_, O(123456701) },
            { L_, O(1234567012) },
            { L_, O(12345670123) },
            { L_, O(123456701234) },
            { L_, O(1234567012345) },
            { L_, O(12345670123456) },
            { L_, O(123456701234567) },
            { L_, O(1234567012345670) },
            { L_, O(12345670123456701) },
            { L_, O(123456701234567012) },
            { L_, O(1234567012345670123) },
            { L_, O(12345670123456701234) },
            { L_, O(123456701234567012345) },
            { L_, O(1234567012345670123456) },

            { L_, D(9) },
            { L_, D(98) },
            { L_, D(987) },
            { L_, D(9876) },
            { L_, D(98765) },
            { L_, D(987654) },
            { L_, D(9876543) },
            { L_, D(98765432) },
            { L_, D(987654321) },
            { L_, D(9876543219) },
            { L_, D(98765432198) },
            { L_, D(987654321987) },
            { L_, D(9876543219876) },
            { L_, D(98765432198765) },
            { L_, D(987654321987654) },
            { L_, D(9876543219876543) },
            { L_, D(98765432198765432) },
            { L_, D(987654321987654321) } };

        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

#undef D
#undef H
#undef O

        if (verbose) printf("quality tests (not testing for accuracy)\n");

        for (int di = 0; di < k_NUM_DATA; ++di) {
            const Data&     data   = DATA[di];
            const int       LINE   = data.d_line;
            Uint64          value  = data.d_value;

            for (unsigned base = 2; base <= 36; ++base) {
                const size_t k_BUFSIZE = 66;
                char         toCharsBuffer[k_BUFSIZE + 1];

                u::verifyEmptyOutput(LINE, value, base, toCharsBuffer);

                u::prepareOutputBuffer(toCharsBuffer);
                char *result = Util::toChars(toCharsBuffer,
                                             toCharsBuffer + k_BUFSIZE,
                                             value,
                                             base);
                ASSERT(result);
                ASSERT(toCharsBuffer < result);
                ASSERT(result < toCharsBuffer + k_BUFSIZE);
                ASSERT(u::expectedBytesHaveChanged(toCharsBuffer, result));

                for (const char *pc = toCharsBuffer; pc < result; ++pc) {
                    unsigned digit = '0' <= *pc && *pc <= '9'
                                   ? *pc - '0'
                                   : 'a' <= *pc && *pc <= 'z'
                                   ? 10 + *pc - 'a'
                                   : 100;
                    ASSERT(digit < base);
                }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV)
                char iBuffer[1000];
                std::to_chars_result iSts = std::to_chars(
                                                     iBuffer,
                                                     iBuffer + sizeof(iBuffer),
                                                     value,
                                                     base);
                ASSERT(std::errc() == iSts.ec);
                ASSERT(iBuffer < iSts.ptr);
                ASSERT(iSts.ptr < iBuffer + sizeof(iBuffer));
                ASSERT(iSts.ptr - iBuffer == result - toCharsBuffer);
                ASSERT(0 == std::memcmp(toCharsBuffer,
                                        iBuffer,
                                        iSts.ptr - iBuffer));
#endif

                if (value != 0) {
                    const Int64 len = result - toCharsBuffer;
                    Uint64 pow = 1;
                    for (int uu = 0; uu < len - 1; ++uu) {
                        pow *= base;
                    }
                    ASSERT(pow <= value);
                    const Uint64 nextPow = pow * base;
                    ASSERTV(LINE, nextPow, pow, base, value,
                                 value < nextPow || u::uint64Max / pow < base);
                }

                Int64 svalue = value;
                if (svalue < 0 && svalue != numeric_limits<Int64>::min()) {
                    // Don't do this block if 'svalue' is unable to store the
                    // positive value of itself.  Signed overflow is undefined
                    // behavior and leads to test failures on highly optimizing
                    // compilers.

                    value = -svalue;

                    u::verifyEmptyOutput(LINE, value, base, toCharsBuffer);

                    u::prepareOutputBuffer(toCharsBuffer);
                    result = Util::toChars(toCharsBuffer,
                                           toCharsBuffer + k_BUFSIZE,
                                           value,
                                           base);
                    ASSERT(result);
                    ASSERT(toCharsBuffer < result);
                    ASSERT(result < toCharsBuffer + k_BUFSIZE);
                    ASSERT(u::expectedBytesHaveChanged(toCharsBuffer, result));

                    svalue = -svalue;
                }

                char  toCharsBufferS[k_BUFSIZE + 1];
                char *resultS;

                if (0 <= svalue) {
                    u::verifyEmptyOutput(LINE, svalue, base, toCharsBufferS);

                    u::prepareOutputBuffer(toCharsBufferS);
                    resultS = Util::toChars(toCharsBufferS,
                                            toCharsBufferS + k_BUFSIZE,
                                            svalue,
                                            base);
                    ASSERT(resultS);
                    ASSERT(u::expectedBytesHaveChanged(toCharsBufferS,
                                                       resultS));
                    ASSERT(resultS < toCharsBufferS + k_BUFSIZE);
                    ASSERT(resultS - toCharsBufferS == result - toCharsBuffer);
                    ASSERT(0 == memcmp(toCharsBuffer,
                                       toCharsBufferS,
                                       result - toCharsBuffer));

                    if (0 == svalue) {
                        continue;                                   // CONTINUE
                    }

                    svalue = -svalue;
                }
                ASSERT(svalue < 0);

                u::verifyEmptyOutput(LINE, svalue, base, toCharsBufferS);

                u::prepareOutputBuffer(toCharsBufferS);
                resultS = Util::toChars(toCharsBufferS,
                                        toCharsBufferS + k_BUFSIZE,
                                        svalue,
                                        base);
                ASSERT(resultS);
                ASSERT(u::expectedBytesHaveChanged(toCharsBufferS, resultS));
                ASSERT(toCharsBufferS < resultS);
                ASSERT(resultS < toCharsBufferS + k_BUFSIZE);
                ASSERT('-' == toCharsBufferS[0]);
                ASSERT(resultS - toCharsBufferS == 1 + result - toCharsBuffer);
                ASSERT(0 == memcmp(toCharsBuffer,
                                   toCharsBufferS + 1,
                                   result - toCharsBuffer));

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV)
                iSts = std::to_chars(iBuffer,
                                     iBuffer + sizeof(iBuffer),
                                     svalue,
                                     base);
                ASSERT(std::errc() == iSts.ec);
                ASSERT(iSts.ptr - iBuffer == resultS - toCharsBufferS);
                ASSERT(0 == memcmp(toCharsBufferS,
                                   iBuffer,
                                   iSts.ptr - iBuffer));
#endif
            }
        }

        if (verbose) printf("Accuracy 1: sprintf comparisons\n");

        for (int di = 0; di < k_NUM_DATA; ++di) {
            const Data&     data   = DATA[di];
            const int       LINE   = data.d_line;
            const Uint64    VALUE  = data.d_value;

            char  sprintfBuffer[40];
            char  toCharsBuffer[40];
            char *result;

            sprintf(sprintfBuffer, "%llu", VALUE);
            result = Util::toChars(toCharsBuffer,
                                   toCharsBuffer + sizeof(toCharsBuffer) - 1,
                                   VALUE,
                                   10);
            ASSERT(result);
            *result = 0;
            ASSERTV(LINE, 0 == strcmp(sprintfBuffer, toCharsBuffer));

            for (Int64 sValue = VALUE, ii = 0; ii < 2; ++ii, sValue = -sValue){
                sprintf(sprintfBuffer, "%lld", sValue);
                result = Util::toChars(
                                     toCharsBuffer,
                                     toCharsBuffer + sizeof(toCharsBuffer) - 1,
                                     sValue,
                                     10);
                ASSERT(result);
                *result = 0;
                ASSERTV(LINE, 0 == strcmp(sprintfBuffer, toCharsBuffer));
                if (sValue == numeric_limits<Int64>::min()) {
                    // Signed overflow is undefined behavior.
                    break;                                             // BREAK
                }
            }

            sprintf(sprintfBuffer, "%llo", VALUE);
            result = Util::toChars(toCharsBuffer,
                                   toCharsBuffer + sizeof(toCharsBuffer) - 1,
                                   VALUE,
                                   8);
            ASSERT(result);
            *result = 0;
            ASSERTV(LINE, 0 == strcmp(sprintfBuffer, toCharsBuffer));

            sprintf(sprintfBuffer, "%llx", VALUE);
            result = Util::toChars(toCharsBuffer,
                                   toCharsBuffer + sizeof(toCharsBuffer) - 1,
                                   VALUE,
                                   16);
            ASSERT(result);
            *result = 0;
            ASSERTV(LINE, 0 == strcmp(sprintfBuffer, toCharsBuffer));
        }

        if (verbose) printf("Accuracy 2: Source string comparisons\n");

        for (int di = 0; di < k_NUM_DATA; ++di) {
            const Data&     data   = DATA[di];
            const int       LINE   = data.d_line;
            const Uint64    VALUE  = data.d_value;
            const char     *STRING = data.d_string_p;
            const unsigned  BASE   = data.d_base;

            char  toCharsBuffer[40];
            char *result;

            result = Util::toChars(toCharsBuffer,
                                   toCharsBuffer + sizeof(toCharsBuffer) - 1,
                                   VALUE,
                                   BASE);
            ASSERT(result);
            *result = 0;
            std::string s = STRING;
            ASSERTV(LINE, s == toCharsBuffer);
            if (0 < VALUE && VALUE <= (1ULL << 63)) {
                const Int64 sValue = -VALUE;
                result = Util::toChars(
                                     toCharsBuffer,
                                     toCharsBuffer + sizeof(toCharsBuffer) - 1,
                                     sValue,
                                     BASE);
                *result = 0;

                s.insert(0, "-");
                ASSERTV(LINE, s.c_str(), toCharsBuffer, s == toCharsBuffer);
            }
        }
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
        //: 1 Perform an ad-hoc test of the functions under test.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");

        if (veryVerbose) puts("Integers");
        {
            const size_t k_MAX_LEN = 65; // 64 binary digits plus null char

            char    buf[k_MAX_LEN + 1];
            char   *result;
            size_t  resultLen;

            for (int base = 2; base < 37; ++base) {
                // 0 and 1 look the same in any base
                result = Util::toChars(buf, buf + 1, 0, base);
                resultLen = result - buf;
                ASSERTV(resultLen, 1 == resultLen);
                ASSERTV(buf[0], '0' == buf[0]);

                result = Util::toChars(buf, buf + 1, 1, base);
                resultLen = result - buf;
                ASSERTV(resultLen, 1 == resultLen);
                ASSERTV(buf[0], '1' == buf[0]);
            }

            // Binary - all one bits
            result = Util::toChars(buf, buf + k_MAX_LEN, ~0ull, 2);
            resultLen = result - buf;
            ASSERTV(resultLen, 64 == resultLen);   // 64 significant bits set
            const char *sixtyfourOnes =
                                   "11111111" "11111111" "11111111" "11111111"
                                   "11111111" "11111111" "11111111" "11111111";
            *result = '\0';  // In case 'ASSERTV' wants to print it
            ASSERTV(buf, 0 == memcmp(buf, sixtyfourOnes, 64));

            // Binary - alternating pattern 01
            result = Util::toChars(buf, buf + k_MAX_LEN,
                                   0x5555555555555555ull, 2);
            resultLen = result - buf;
            ASSERTV(resultLen, 63 == resultLen);   // 63 significant bits set
            const char *thirtytwoZeroOnes =
                                    "1010101" "01010101" "01010101" "01010101"
                                   "01010101" "01010101" "01010101" "01010101";
            *result = '\0';  // In case 'ASSERTV' wants to print it
            ASSERTV(buf, 0 == memcmp(buf, thirtytwoZeroOnes, 63));

            // Binary - alternating pattern 10
            result = Util::toChars(buf, buf + k_MAX_LEN,
                                   0xAAAAAAAAAAAAAAAAull, 2);
            resultLen = result - buf;
            ASSERTV(resultLen, 64 == resultLen);   // 64 significant bits set
            const char *thirtytwoOneZeros =
                                   "10101010" "10101010" "10101010" "10101010"
                                   "10101010" "10101010" "10101010" "10101010";
            *result = '\0';  // In case 'ASSERTV' wants to print it
            ASSERTV(buf, 0 == memcmp(buf, thirtytwoOneZeros, 64));

            // Octal - all bits set
            result = Util::toChars(buf, buf + k_MAX_LEN, ~0ull, 8);
            resultLen = result - buf;
            ASSERTV(resultLen, 22 == resultLen);   // 64 bits set
            const char *octal64Bits = "17" "7777" "7777" "7777" "7777" "7777";
            *result = '\0';  // In case 'ASSERTV' wants to print it
            ASSERTV(buf, 0 == memcmp(buf, octal64Bits, 22));

            // Decimal -- boring
            result = Util::toChars(buf, buf + k_MAX_LEN, ~0ull);
            resultLen = result - buf;
            ASSERTV(resultLen, 20 == resultLen);   // 64 bits set
            const char *decim64Bits = "18" "446" "744" "073" "709" "551" "615";
            *result = '\0';  // In case 'ASSERTV' wants to print it
            ASSERTV(buf, 0 == memcmp(buf, decim64Bits, 20));

            // Hexadecimal -- predictable
            result = Util::toChars(buf, buf + k_MAX_LEN, ~0ull, 16);
            resultLen = result - buf;
            ASSERTV(resultLen, 16 == resultLen);   // 64 bits set
            const char* hex64Bits = "ffff" "ffff" "ffff" "ffff";
            *result = '\0';  // In case 'ASSERTV' wants to print it
            ASSERTV(buf, 0 == memcmp(buf, hex64Bits, 16));

            // ERROR -- not enough space
            result = Util::toChars(0, 0, 0, 2);
            ASSERTV((void*)result, 0 == result);   // Did not fit
        }

        if (veryVerbose) puts("'double' -- minimal notation");
        {
            const size_t k_MAX_LEN = 24;

            char    buf[k_MAX_LEN + 1];
            char   *result;
            size_t  resultLen;

            // Integers
            result = Util::toChars(buf, buf + k_MAX_LEN, 0.0);
            resultLen = result - buf;
            ASSERTV(resultLen, 1 == resultLen);
            ASSERTV(buf[0], '0' == buf[0]);

            result = Util::toChars(buf, buf + k_MAX_LEN, INT_MAX);
            resultLen = result - buf;
            ASSERTV(resultLen, 10 == resultLen);
            *result = '\0';  // In case 'ASSERTV' wants to print it
            ASSERTV(buf, 0 == memcmp(buf, "2147483647", 11));

            result = Util::toChars(buf, buf + k_MAX_LEN, -DBL_MAX);
            resultLen = result - buf;
            ASSERTV(resultLen, 24 == resultLen);
            *result = '\0';  // In case 'ASSERTV' wants to print it
            ASSERTV(buf, 0 == memcmp(buf, "-1.7976931348623157e+308", 24));

            result = Util::toChars(buf, buf + k_MAX_LEN, -DBL_MIN);
            resultLen = result - buf;
            ASSERTV(resultLen, 24 == resultLen);
            *result = '\0';  // In case 'ASSERTV' wants to print it
            ASSERTV(buf, 0 == memcmp(buf, "-2.2250738585072014e-308", 24));

            // ERROR -- not enough space
            result = Util::toChars(0, 0, 42.0);
            ASSERTV((void*)result, 0 == result);   // Did not fit
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // INTEGER BENCHMARKING
        //
        // Concern:
        //: 1 Measure how the performance of the integer 'toChars' function
        //:   under test compares to 'std::to_chars' and 'sprintf'.
        //
        // Plan:
        //: 1 Call the 3 functions many times and time them using
        //:   'bsls_stopwatch'.
        //
        //  Results on Linux host: bldlnx-ob-009
        //  Build: UFID: opt_exc_mt_cpp17 Compiler: /opt/bb/bin/g++-10
        //
        //  Digits:  1: Base:  8:       'toChars': 32 bit:  15.2477  nsec
        //  Digits:  1: Base:  8: 'std::to_chars': 32 bit:   5.2592  nsec
        //  Digits:  1: Base:  8:       'sprintf': 32 bit: 118.942   nsec
        //
        //  Digits:  2: Base:  8:       'toChars': 32 bit:  17.9973  nsec
        //  Digits:  2: Base:  8: 'std::to_chars': 32 bit:   5.62915 nsec
        //  Digits:  2: Base:  8:       'sprintf': 32 bit: 118.562   nsec
        //
        //  Digits:  3: Base:  8:       'toChars': 32 bit:  20.5269  nsec
        //  Digits:  3: Base:  8: 'std::to_chars': 32 bit:   6.14909 nsec
        //  Digits:  3: Base:  8:       'sprintf': 32 bit: 117.392   nsec
        //
        //  Digits:  4: Base:  8:       'toChars': 32 bit:  23.2965  nsec
        //  Digits:  4: Base:  8: 'std::to_chars': 32 bit:   6.33905 nsec
        //  Digits:  4: Base:  8:       'sprintf': 32 bit: 118.392   nsec
        //
        //  Digits:  5: Base:  8:       'toChars': 32 bit:  27.0926  nsec
        //  Digits:  5: Base:  8: 'std::to_chars': 32 bit:   7.2307  nsec
        //  Digits:  5: Base:  8:       'sprintf': 32 bit: 120.092   nsec
        //
        //  Digits:  6: Base:  8:       'toChars': 32 bit:  29.3617  nsec
        //  Digits:  6: Base:  8: 'std::to_chars': 32 bit:   7.5982  nsec
        //  Digits:  6: Base:  8:       'sprintf': 32 bit: 122.072   nsec
        //
        //  Digits:  7: Base:  8:       'toChars': 32 bit:  33.3025  nsec
        //  Digits:  7: Base:  8: 'std::to_chars': 32 bit:   8.034   nsec
        //  Digits:  7: Base:  8:       'sprintf': 32 bit: 122.194   nsec
        //
        //  Digits:  8: Base:  8:       'toChars': 32 bit:  36.1149  nsec
        //  Digits:  8: Base:  8: 'std::to_chars': 32 bit:   8.20035 nsec
        //  Digits:  8: Base:  8:       'sprintf': 32 bit: 122.159   nsec
        //
        //  Digits:  9: Base:  8:       'toChars': 32 bit:  39.5342  nsec
        //  Digits:  9: Base:  8: 'std::to_chars': 32 bit:   9.00638 nsec
        //  Digits:  9: Base:  8:       'sprintf': 32 bit: 124.823   nsec
        //
        //  Digits: 10: Base:  8:       'toChars': 32 bit:  42.6108  nsec
        //  Digits: 10: Base:  8: 'std::to_chars': 32 bit:   8.93934 nsec
        //  Digits: 10: Base:  8:       'sprintf': 32 bit: 122.99    nsec
        //
        //  Digits: 11: Base:  8:       'toChars': 32 bit:  44.9369  nsec
        //  Digits: 11: Base:  8: 'std::to_chars': 32 bit:   9.79696 nsec
        //  Digits: 11: Base:  8:       'sprintf': 32 bit: 125.297   nsec
        //
        //  Digits:  1: Base:  8:       'toChars': 64 bit:  19.0871  nsec
        //  Digits:  1: Base:  8: 'std::to_chars': 64 bit:   7.87881 nsec
        //  Digits:  1: Base:  8:       'sprintf': 64 bit: 128.27    nsec
        //
        //  Digits:  2: Base:  8:       'toChars': 64 bit:  21.3968  nsec
        //  Digits:  2: Base:  8: 'std::to_chars': 64 bit:   8.19875 nsec
        //  Digits:  2: Base:  8:       'sprintf': 64 bit: 128.43    nsec
        //
        //  Digits:  3: Base:  8:       'toChars': 64 bit:  24.9763  nsec
        //  Digits:  3: Base:  8: 'std::to_chars': 64 bit:   9.55857 nsec
        //  Digits:  3: Base:  8:       'sprintf': 64 bit: 127.291   nsec
        //
        //  Digits:  4: Base:  8:       'toChars': 64 bit:  26.896   nsec
        //  Digits:  4: Base:  8: 'std::to_chars': 64 bit:  10.0385  nsec
        //  Digits:  4: Base:  8:       'sprintf': 64 bit: 130.201   nsec
        //
        //  Digits:  5: Base:  8:       'toChars': 64 bit:  29.9229  nsec
        //  Digits:  5: Base:  8: 'std::to_chars': 64 bit:  11.2211  nsec
        //  Digits:  5: Base:  8:       'sprintf': 64 bit: 129.332   nsec
        //
        //  Digits:  6: Base:  8:       'toChars': 64 bit:  32.6152  nsec
        //  Digits:  6: Base:  8: 'std::to_chars': 64 bit:  12.2232  nsec
        //  Digits:  6: Base:  8:       'sprintf': 64 bit: 130.221   nsec
        //
        //  Digits:  7: Base:  8:       'toChars': 64 bit:  35.5443  nsec
        //  Digits:  7: Base:  8: 'std::to_chars': 64 bit:  13.3291  nsec
        //  Digits:  7: Base:  8:       'sprintf': 64 bit: 132.013   nsec
        //
        //  Digits:  8: Base:  8:       'toChars': 64 bit:  38.1054  nsec
        //  Digits:  8: Base:  8: 'std::to_chars': 64 bit:  13.6712  nsec
        //  Digits:  8: Base:  8:       'sprintf': 64 bit: 133.756   nsec
        //
        //  Digits:  9: Base:  8:       'toChars': 64 bit:  40.7261  nsec
        //  Digits:  9: Base:  8: 'std::to_chars': 64 bit:  15.1671  nsec
        //  Digits:  9: Base:  8:       'sprintf': 64 bit: 133.867   nsec
        //
        //  Digits: 10: Base:  8:       'toChars': 64 bit:  43.5886  nsec
        //  Digits: 10: Base:  8: 'std::to_chars': 64 bit:  14.9361  nsec
        //  Digits: 10: Base:  8:       'sprintf': 64 bit: 134.286   nsec
        //
        //  Digits: 11: Base:  8:       'toChars': 64 bit:  46.9955  nsec
        //  Digits: 11: Base:  8: 'std::to_chars': 64 bit:  15.8795  nsec
        //  Digits: 11: Base:  8:       'sprintf': 64 bit: 132.488   nsec
        //
        //  Digits: 12: Base:  8:       'toChars': 64 bit:  49.5784  nsec
        //  Digits: 12: Base:  8: 'std::to_chars': 64 bit:  16.4229  nsec
        //  Digits: 12: Base:  8:       'sprintf': 64 bit: 131.943   nsec
        //
        //  Digits: 13: Base:  8:       'toChars': 64 bit:  52.5967  nsec
        //  Digits: 13: Base:  8: 'std::to_chars': 64 bit:  17.3656  nsec
        //  Digits: 13: Base:  8:       'sprintf': 64 bit: 134.466   nsec
        //
        //  Digits: 14: Base:  8:       'toChars': 64 bit:  54.4012  nsec
        //  Digits: 14: Base:  8: 'std::to_chars': 64 bit:  17.7171  nsec
        //  Digits: 14: Base:  8:       'sprintf': 64 bit: 135.448   nsec
        //
        //  Digits: 15: Base:  8:       'toChars': 64 bit:  57.7411  nsec
        //  Digits: 15: Base:  8: 'std::to_chars': 64 bit:  18.8671  nsec
        //  Digits: 15: Base:  8:       'sprintf': 64 bit: 135.219   nsec
        //
        //  Digits: 16: Base:  8:       'toChars': 64 bit:  60.1708  nsec
        //  Digits: 16: Base:  8: 'std::to_chars': 64 bit:  19.437   nsec
        //  Digits: 16: Base:  8:       'sprintf': 64 bit: 134.05    nsec
        //
        //  Digits: 17: Base:  8:       'toChars': 64 bit:  64.1203  nsec
        //  Digits: 17: Base:  8: 'std::to_chars': 64 bit:  21.4467  nsec
        //  Digits: 17: Base:  8:       'sprintf': 64 bit: 138.619   nsec
        //
        //  Digits: 18: Base:  8:       'toChars': 64 bit:  66.05    nsec
        //  Digits: 18: Base:  8: 'std::to_chars': 64 bit:  20.8968  nsec
        //  Digits: 18: Base:  8:       'sprintf': 64 bit: 137.759   nsec
        //
        //  Digits: 19: Base:  8:       'toChars': 64 bit:  68.0696  nsec
        //  Digits: 19: Base:  8: 'std::to_chars': 64 bit:  22.3566  nsec
        //  Digits: 19: Base:  8:       'sprintf': 64 bit: 140.689   nsec
        //
        //  Digits: 20: Base:  8:       'toChars': 64 bit:  71.6691  nsec
        //  Digits: 20: Base:  8: 'std::to_chars': 64 bit:  22.4766  nsec
        //  Digits: 20: Base:  8:       'sprintf': 64 bit: 135.669   nsec
        //
        //  Digits: 21: Base:  8:       'toChars': 64 bit:  74.0487  nsec
        //  Digits: 21: Base:  8: 'std::to_chars': 64 bit:  23.5964  nsec
        //  Digits: 21: Base:  8:       'sprintf': 64 bit: 140.459   nsec
        //
        //  Digits:  1: Base: 10:       'toChars': 32 bit:   6.58899 nsec
        //  Digits:  1: Base: 10: 'std::to_chars': 32 bit:   6.01909 nsec
        //  Digits:  1: Base: 10:       'sprintf': 32 bit: 111.493   nsec
        //
        //  Digits:  2: Base: 10:       'toChars': 32 bit:   6.79897 nsec
        //  Digits:  2: Base: 10: 'std::to_chars': 32 bit:   6.16906 nsec
        //  Digits:  2: Base: 10:       'sprintf': 32 bit: 112.473   nsec
        //
        //  Digits:  3: Base: 10:       'toChars': 32 bit:  10.2984  nsec
        //  Digits:  3: Base: 10: 'std::to_chars': 32 bit:   6.36903 nsec
        //  Digits:  3: Base: 10:       'sprintf': 32 bit: 114.063   nsec
        //
        //  Digits:  4: Base: 10:       'toChars': 32 bit:  11.1483  nsec
        //  Digits:  4: Base: 10: 'std::to_chars': 32 bit:   6.639   nsec
        //  Digits:  4: Base: 10:       'sprintf': 32 bit: 115.252   nsec
        //
        //  Digits:  5: Base: 10:       'toChars': 32 bit:  12.7981  nsec
        //  Digits:  5: Base: 10: 'std::to_chars': 32 bit:   8.47871 nsec
        //  Digits:  5: Base: 10:       'sprintf': 32 bit: 120.042   nsec
        //
        //  Digits:  6: Base: 10:       'toChars': 32 bit:  13.7379  nsec
        //  Digits:  6: Base: 10: 'std::to_chars': 32 bit:   9.60854 nsec
        //  Digits:  6: Base: 10:       'sprintf': 32 bit: 119.782   nsec
        //
        //  Digits:  7: Base: 10:       'toChars': 32 bit:  14.7378  nsec
        //  Digits:  7: Base: 10: 'std::to_chars': 32 bit:  10.8583  nsec
        //  Digits:  7: Base: 10:       'sprintf': 32 bit: 122.801   nsec
        //
        //  Digits:  8: Base: 10:       'toChars': 32 bit:  15.1777  nsec
        //  Digits:  8: Base: 10: 'std::to_chars': 32 bit:  11.0783  nsec
        //  Digits:  8: Base: 10:       'sprintf': 32 bit: 124.901   nsec
        //
        //  Digits:  9: Base: 10:       'toChars': 32 bit:  17.9473  nsec
        //  Digits:  9: Base: 10: 'std::to_chars': 32 bit:  13.138   nsec
        //  Digits:  9: Base: 10:       'sprintf': 32 bit: 124.601   nsec
        //
        //  Digits: 10: Base: 10:       'toChars': 32 bit:  17.9383  nsec
        //  Digits: 10: Base: 10: 'std::to_chars': 32 bit:  13.3619  nsec
        //  Digits: 10: Base: 10:       'sprintf': 32 bit: 127.802   nsec
        //
        //  Digits: 11: Base: 10:       'toChars': 32 bit:  17.9709  nsec
        //  Digits: 11: Base: 10: 'std::to_chars': 32 bit:  13.7666  nsec
        //  Digits: 11: Base: 10:       'sprintf': 32 bit: 129.854   nsec
        //
        //  Digits:  1: Base: 10:       'toChars': 64 bit:  11.1483  nsec
        //  Digits:  1: Base: 10: 'std::to_chars': 64 bit:   7.38887 nsec
        //  Digits:  1: Base: 10:       'sprintf': 64 bit: 115.662   nsec
        //
        //  Digits:  2: Base: 10:       'toChars': 64 bit: 11.3683   nsec
        //  Digits:  2: Base: 10: 'std::to_chars': 64 bit: 7.59885   nsec
        //  Digits:  2: Base: 10:       'sprintf': 64 bit: 118.652   nsec
        //
        //  Digits:  3: Base: 10:       'toChars': 64 bit:  14.6678  nsec
        //  Digits:  3: Base: 10: 'std::to_chars': 64 bit:  19.797   nsec
        //  Digits:  3: Base: 10:       'sprintf': 64 bit: 123.631   nsec
        //
        //  Digits:  4: Base: 10:       'toChars': 64 bit:  15.5776  nsec
        //  Digits:  4: Base: 10: 'std::to_chars': 64 bit:  22.1566  nsec
        //  Digits:  4: Base: 10:       'sprintf': 64 bit: 129.91    nsec
        //
        //  Digits:  5: Base: 10:       'toChars': 64 bit:  17.1974  nsec
        //  Digits:  5: Base: 10: 'std::to_chars': 64 bit:  40.2039  nsec
        //  Digits:  5: Base: 10:       'sprintf': 64 bit: 137.109   nsec
        //
        //  Digits:  6: Base: 10:       'toChars': 64 bit:  18.0772  nsec
        //  Digits:  6: Base: 10: 'std::to_chars': 64 bit:  43.8233  nsec
        //  Digits:  6: Base: 10:       'sprintf': 64 bit: 146.688   nsec
        //
        //  Digits:  7: Base: 10:       'toChars': 64 bit:  19.897   nsec
        //  Digits:  7: Base: 10: 'std::to_chars': 64 bit:  56.7314  nsec
        //  Digits:  7: Base: 10:       'sprintf': 64 bit: 153.087   nsec
        //
        //  Digits:  8: Base: 10:       'toChars': 64 bit:  20.0469  nsec
        //  Digits:  8: Base: 10: 'std::to_chars': 64 bit:  61.0807  nsec
        //  Digits:  8: Base: 10:       'sprintf': 64 bit: 162.295   nsec
        //
        //  Digits:  9: Base: 10:       'toChars': 64 bit:  22.5066  nsec
        //  Digits:  9: Base: 10: 'std::to_chars': 64 bit:  77.3083  nsec
        //  Digits:  9: Base: 10:       'sprintf': 64 bit: 172.094   nsec
        //
        //  Digits: 10: Base: 10:       'toChars': 64 bit:  40.9038  nsec
        //  Digits: 10: Base: 10: 'std::to_chars': 64 bit:  79.6979  nsec
        //  Digits: 10: Base: 10:       'sprintf': 64 bit: 180.153   nsec
        //
        //  Digits: 11: Base: 10:       'toChars': 64 bit:  42.2536  nsec
        //  Digits: 11: Base: 10: 'std::to_chars': 64 bit:  93.1958  nsec
        //  Digits: 11: Base: 10:       'sprintf': 64 bit: 188.531   nsec
        //
        //  Digits: 12: Base: 10:       'toChars': 64 bit:  59.9409  nsec
        //  Digits: 12: Base: 10: 'std::to_chars': 64 bit: 102.054   nsec
        //  Digits: 12: Base: 10:       'sprintf': 64 bit: 192.701   nsec
        //
        //  Digits: 13: Base: 10:       'toChars': 64 bit:  68.3596  nsec
        //  Digits: 13: Base: 10: 'std::to_chars': 64 bit: 122.791   nsec
        //  Digits: 13: Base: 10:       'sprintf': 64 bit: 193.571   nsec
        //
        //  Digits: 14: Base: 10:       'toChars': 64 bit:  87.3667  nsec
        //  Digits: 14: Base: 10: 'std::to_chars': 64 bit: 132.98    nsec
        //  Digits: 14: Base: 10:       'sprintf': 64 bit: 198.24    nsec
        //
        //  Digits: 15: Base: 10:       'toChars': 64 bit:  95.9354  nsec
        //  Digits: 15: Base: 10: 'std::to_chars': 64 bit: 150.087   nsec
        //  Digits: 15: Base: 10:       'sprintf': 64 bit: 208.418   nsec
        //
        //  Digits: 16: Base: 10:       'toChars': 64 bit: 111.013   nsec
        //  Digits: 16: Base: 10: 'std::to_chars': 64 bit: 157.196   nsec
        //  Digits: 16: Base: 10:       'sprintf': 64 bit: 207.029   nsec
        //
        //  Digits: 17: Base: 10:       'toChars': 64 bit: 119.792   nsec
        //  Digits: 17: Base: 10: 'std::to_chars': 64 bit: 176.423   nsec
        //  Digits: 17: Base: 10:       'sprintf': 64 bit: 177.633   nsec
        //
        //  Digits: 18: Base: 10:       'toChars': 64 bit: 137.779   nsec
        //  Digits: 18: Base: 10: 'std::to_chars': 64 bit: 191.271   nsec
        //  Digits: 18: Base: 10:       'sprintf': 64 bit: 186.542   nsec
        //
        //  Digits: 19: Base: 10:       'toChars': 64 bit: 144.358   nsec
        //  Digits: 19: Base: 10: 'std::to_chars': 64 bit: 209.348   nsec
        //  Digits: 19: Base: 10:       'sprintf': 64 bit: 243.543   nsec
        //
        //  Digits:  1: Base: 16:       'toChars': 32 bit:  15.4276  nsec
        //  Digits:  1: Base: 16: 'std::to_chars': 32 bit:   5.66914 nsec
        //  Digits:  1: Base: 16:       'sprintf': 32 bit: 114.193   nsec
        //
        //  Digits:  2: Base: 16:       'toChars': 32 bit:  17.6973  nsec
        //  Digits:  2: Base: 16: 'std::to_chars': 32 bit:   5.18921 nsec
        //  Digits:  2: Base: 16:       'sprintf': 32 bit: 113.603   nsec
        //
        //  Digits:  3: Base: 16:       'toChars': 32 bit:  20.8469  nsec
        //  Digits:  3: Base: 16: 'std::to_chars': 32 bit:   5.71914 nsec
        //  Digits:  3: Base: 16:       'sprintf': 32 bit: 114.113   nsec
        //
        //  Digits:  4: Base: 16:       'toChars': 32 bit:   23.63   nsec
        //  Digits:  4: Base: 16: 'std::to_chars': 32 bit:   5.71242 nsec
        //  Digits:  4: Base: 16:       'sprintf': 32 bit: 115.769   nsec
        //
        //  Digits:  5: Base: 16:       'toChars': 32 bit:   26.739  nsec
        //  Digits:  5: Base: 16: 'std::to_chars': 32 bit:   6.66467 nsec
        //  Digits:  5: Base: 16:       'sprintf': 32 bit: 116.28    nsec
        //
        //  Digits:  6: Base: 16:       'toChars': 32 bit:  29.7024  nsec
        //  Digits:  6: Base: 16: 'std::to_chars': 32 bit:   6.85349 nsec
        //  Digits:  6: Base: 16:       'sprintf': 32 bit: 119.918   nsec
        //
        //  Digits:  7: Base: 16:       'toChars': 32 bit:  33.448   nsec
        //  Digits:  7: Base: 16: 'std::to_chars': 32 bit:   7.74743 nsec
        //  Digits:  7: Base: 16:       'sprintf': 32 bit: 117.098   nsec
        //
        //  Digits:  8: Base: 16:       'toChars': 32 bit:  36.3328  nsec
        //  Digits:  8: Base: 16: 'std::to_chars': 32 bit:   8.24234 nsec
        //  Digits:  8: Base: 16:       'sprintf': 32 bit: 120.409   nsec
        //
        //  Digits:  1: Base: 16:       'toChars': 64 bit:  19.437   nsec
        //  Digits:  1: Base: 16: 'std::to_chars': 64 bit:   8.95864 nsec
        //  Digits:  1: Base: 16:       'sprintf': 64 bit: 119.692   nsec
        //
        //  Digits:  2: Base: 16:       'toChars': 64 bit:  21.5367  nsec
        //  Digits:  2: Base: 16: 'std::to_chars': 64 bit:   7.85881 nsec
        //  Digits:  2: Base: 16:       'sprintf': 64 bit: 120.372   nsec
        //
        //  Digits:  3: Base: 16:       'toChars': 64 bit:  25.7162  nsec
        //  Digits:  3: Base: 16: 'std::to_chars': 64 bit:   9.3386  nsec
        //  Digits:  3: Base: 16:       'sprintf': 64 bit: 121.902   nsec
        //
        //  Digits:  4: Base: 16:       'toChars': 64 bit:  27.3316  nsec
        //  Digits:  4: Base: 16: 'std::to_chars': 64 bit:  10.1443  nsec
        //  Digits:  4: Base: 16:       'sprintf': 64 bit: 123.002   nsec
        //
        //  Digits:  5: Base: 16:       'toChars': 64 bit:  30.5431  nsec
        //  Digits:  5: Base: 16: 'std::to_chars': 64 bit:  11.0911  nsec
        //  Digits:  5: Base: 16:       'sprintf': 64 bit: 124.561   nsec
        //
        //  Digits:  6: Base: 16:       'toChars': 64 bit:  32.9444  nsec
        //  Digits:  6: Base: 16: 'std::to_chars': 64 bit:  12.0145  nsec
        //  Digits:  6: Base: 16:       'sprintf': 64 bit: 121.944   nsec
        //
        //  Digits:  7: Base: 16:       'toChars': 64 bit:  35.6233  nsec
        //  Digits:  7: Base: 16: 'std::to_chars': 64 bit:  13.707   nsec
        //  Digits:  7: Base: 16:       'sprintf': 64 bit: 123.877   nsec
        //
        //  Digits:  8: Base: 16:       'toChars': 64 bit:  38.2101  nsec
        //  Digits:  8: Base: 16: 'std::to_chars': 64 bit:  15.7807  nsec
        //  Digits:  8: Base: 16:       'sprintf': 64 bit: 124.369   nsec
        //
        //  Digits:  9: Base: 16:       'toChars': 64 bit:  41.1421  nsec
        //  Digits:  9: Base: 16: 'std::to_chars': 64 bit:  14.6536  nsec
        //  Digits:  9: Base: 16:       'sprintf': 64 bit: 122.936   nsec
        //
        //  Digits: 10: Base: 16:       'toChars': 64 bit:  44.1328  nsec
        //  Digits: 10: Base: 16: 'std::to_chars': 64 bit:  15.0476  nsec
        //  Digits: 10: Base: 16:       'sprintf': 64 bit: 125.02    nsec
        //
        //  Digits: 11: Base: 16:       'toChars': 64 bit:  47.7125  nsec
        //  Digits: 11: Base: 16: 'std::to_chars': 64 bit:  16.4474  nsec
        //  Digits: 11: Base: 16:       'sprintf': 64 bit: 125.71    nsec
        //
        //  Digits: 12: Base: 16:       'toChars': 64 bit:  49.6224  nsec
        //  Digits: 12: Base: 16: 'std::to_chars': 64 bit:  16.3875  nsec
        //  Digits: 12: Base: 16:       'sprintf': 64 bit: 124.301   nsec
        //
        //  Digits: 13: Base: 16:       'toChars': 64 bit:  52.0921  nsec
        //  Digits: 13: Base: 16: 'std::to_chars': 64 bit:  17.3773  nsec
        //  Digits: 13: Base: 16:       'sprintf': 64 bit: 126.331   nsec
        //
        //  Digits: 14: Base: 16:       'toChars': 64 bit:  54.4617  nsec
        //  Digits: 14: Base: 16: 'std::to_chars': 64 bit:  18.1372  nsec
        //  Digits: 14: Base: 16:       'sprintf': 64 bit: 127.161   nsec
        //
        //  Digits: 15: Base: 16:       'toChars': 64 bit:  58.1612  nsec
        //  Digits: 15: Base: 16: 'std::to_chars': 64 bit:  20.1769  nsec
        //  Digits: 15: Base: 16:       'sprintf': 64 bit: 129.96    nsec
        //
        //  Digits: 16: Base: 16:       'toChars': 64 bit:  60.5408  nsec
        //  Digits: 16: Base: 16: 'std::to_chars': 64 bit:  19.957   nsec
        //  Digits: 16: Base: 16:       'sprintf': 64 bit: 131.55    nsec
        //
        //  Thoughts: Before writing this component, we did a lot of
        //  examination of the implementation of 'std::to_chars' (being a
        //  template function, the whole thing is in the include files).
        //
        //  'std::to_chars' has special hard-coded implementations for octal
        //  and hex, whereas 'toChars' is using a general function that can
        //  handle all binary bases, so it's not entirely surprising that
        //  theirs is faster.  The performance is good enough that it's not
        //  worth doing custom octal and hex functions.
        //
        //  'toChars' has a fully-custom decimal implementation that is very
        //  similar to that of 'std::to_chars' with a couple of improvements.
        //
        //  Generally, the goal is to provide a replacement for 'to_chars' on
        //  compilers where the native imp isn't available.  What we have is
        //  performing well enough for that.
        //
        //  Note that this component's decimal implementation outperforms
        //  'std::to_chars', and that this performance difference widens as the
        //  number of digits grows.  We feel that decimal is by far the most
        //  important base supported by the function, and that is where most of
        //  the optimization effort was focused.
        // --------------------------------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV) && \
    defined(BDE_BUILD_TARGET_OPT)
        puts("INTEGER BENCHMARKING");

        enum { k_HUNDRED_MILLION = 100 * 1000 * 1000 };

        bsls::Stopwatch sw;

        unsigned bases[] = { 8, 10, 16 };
        const char *intFmts[] = { "%o", "%d", "%x" };
        const char *int64Fmts[] = { "%llo", "%lld", "%llx" };

        for (int baseIdx = 0; baseIdx < 3; ++baseIdx) {
            unsigned base = bases[baseIdx];

            bool quit = false;
            for (int digits = 1; !quit; ++digits) {
                int ceiling = 1;

                for (int ii = 0, prev = ceiling; ii < digits; ++ii) {
                    prev = ceiling;
                    ceiling *= base;
                    if (ceiling < prev) {
                        quit = true;
                        ceiling = std::numeric_limits<int>::max();
                    }
                }
                const int delta  = ceiling < k_HUNDRED_MILLION
                                 ? 1
                                 : ceiling / k_HUNDRED_MILLION;

                const int trials = std::max<unsigned>(
                                     1, k_HUNDRED_MILLION / (ceiling / delta));
                double actions = ceiling;
                actions /= delta;
                actions *= trials;

                const char *fmt = intFmts[baseIdx];

                for (int kk = 0; kk < 3; ++kk) {
                    printf((0 == kk
                         ? "\nDigits: %2d: Base: %2u:       toChars: 32 bit: "
                         :  1 == kk
                         ? "Digits: %2d: Base: %2u: std::to_chars: 32 bit: "
                         : "Digits: %2d: Base: %2u:       sprintf: 32 bit: "),
                                                                 digits, base);

                    sw.reset();
                    sw.start(true);

                    for (int ii = trials; 0 < ii--;) {
                        bool sign = false;
                        for (int jj = 0, prev = -1; prev < jj && jj < ceiling;
                                        prev = jj, jj += delta, sign = !sign) {
                            int num = sign ? -jj : jj;
                            char buf[100];
                            switch (kk) {
                              case 0: {
                                char *result = Util::toChars(buf,
                                                             buf + 100,
                                                             num,
                                                             base);
                                BSLS_ASSERT_OPT(result);
                              } break;
                              case 1: {
                                std::to_chars_result result =
                                                       std::to_chars(buf,
                                                                     buf + 100,
                                                                     num,
                                                                     base);
                                BSLS_ASSERT_OPT(result.ec == std::errc());
                              } break;
                              case 2: {
                                sprintf(buf, fmt, num);
                                BSLS_ASSERT_OPT(0 != *buf);
                              } break;
                            }
                        }
                    }

                    sw.stop();
                    printf("%g nsec\n",
                                      1e9 * sw.accumulatedUserTime()/ actions);
                }
            }

            quit = false;
            for (int digits = 1; !quit; ++digits) {
                Int64 ceiling = 1;
                for (Int64 ii = 0, prev = ceiling; ii < digits; ++ii) {
                    prev = ceiling;
                    ceiling *= base;
                    if (ceiling < prev) {
                        quit = true;
                        ceiling = std::numeric_limits<Int64>::max();
                    }
                }
                const Int64 delta = ceiling < k_HUNDRED_MILLION
                                  ? 1
                                  : ceiling / k_HUNDRED_MILLION;
                const Int64 trials = std::max<Int64>(
                                     1, k_HUNDRED_MILLION / (ceiling / delta));
                double actions = static_cast<double>(ceiling);
                actions /= static_cast<double>(delta);
                actions *= static_cast<double>(trials);

                const char *fmt = int64Fmts[baseIdx];

                for (int kk = 0; kk < 3; ++kk) {
                    printf((0 == kk
                         ? "\nDigits: %2d: Base: %2u:       toChars: 64 bit: "
                         :  1 == kk
                         ? "Digits: %2d: Base: %2u: std::to_chars: 64 bit: "
                         : "Digits: %2d: Base: %2u:       sprintf: 64 bit: "),
                                                                 digits, base);

                    sw.reset();
                    sw.start(true);

                    for (Int64 ii = trials; 0 < ii--;) {
                        bool sign = false;
                        for (Int64 jj = 0, prev = -1;
                                         prev < jj && jj < ceiling;
                                        prev = jj, jj += delta, sign = !sign) {
                            Int64 num = sign ? -jj : jj;
                            char buf[100];
                            switch (kk) {
                              case 0: {
                                char *result = Util::toChars(buf,
                                                             buf + 100,
                                                             num,
                                                             base);
                                BSLS_ASSERT_OPT(result);
                              } break;
                              case 1: {
                                std::to_chars_result result =
                                                       std::to_chars(buf,
                                                                     buf + 100,
                                                                     num,
                                                                     base);
                                BSLS_ASSERT_OPT(result.ec == std::errc());
                              } break;
                              case 2: {
                                sprintf(buf, fmt, num);
                                BSLS_ASSERT_OPT(0 != *buf);
                              } break;
                            }
                        }
                    }

                    sw.stop();
                    printf("%g nsec\n",
                                      1e9 * sw.accumulatedUserTime()/ actions);
                }
            }
        }
#else
#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV)
        puts("INTEGER BENCHMARKING IS NOT SUPPORTED ON THIS PLATFORM");
#elif !defined(BDE_BUILD_TARGET_OPT)
        puts("BUILD OPTIMIZED FOR INTEGER BENCHMARKING!");
#endif
#endif
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // 'float' MAX-LENGTH BRUTE-FORCE CHECK
        //
        // Concern:
        //: 1 The maximum buffer size for 'float both in scientific mode and
        //:   decimal (fixed) mode fits all possible 'float' numerical values,
        //:   including subnormals.
        //
        // Plan:
        //: 1 Loop all 'float' numbers between zero not-included and positive
        //:   infinity not-included.  (We test the length of zero and negative
        //:   zero elsewhere.)  Note that we verify positive values only as
        //:   they are the same as negative values, only 1 character shorter
        //:   and slightly faster to do.
        //:
        //: 2 Use the original, unchanged Ryu 'ryu_f2s_buffered' function to
        //:   get an oracle value.
        //:
        //: 3 Parse the original value into actual decimal significand number
        //:   of digits as well the decimal exponent value.
        //:
        //: 4 From the parsed values calculate the C++-conforming scientific,
        //:   as well as decimal length for negative values and verify that
        //:   they do not exceed the maximum values.
        //
        // Testing:
        //   ToCharsMaxLength<float>::k_VALUE
        //   ToCharsMaxLength<float, e_FIXED>::k_VALUE
        //   ToCharsMaxLength<float, e_SCIENTIFC>::k_VALUE
        // --------------------------------------------------------------------

        if (verbose) puts("\n'float' MAX-LENGTH BRUTE-FORCE CHECK"
                          "\n=====================================");

#if defined(BDE_BUILD_TARGET_OPT)
        const size_t k_F2S_BUFFSIZE = 15;
            // Ryu guarantees this is the maximum it will write.
        char ryuBuf[k_F2S_BUFFSIZE + 1];
            // Plus one for a closing null character.

        const float inf = std::numeric_limits<float>::infinity();
        bsls::Stopwatch sw;
        sw.start();
        for (float f = nextafterf(0, inf); f < inf; f = nextafterf(f, inf)) {
            ryu_f2s_buffered(f, ryuBuf);
            int nSignificandDigits = 0;
            const char* p = ryuBuf;
            for (; 'E' != *p; ++p) {
                if ('.' == *p) continue;                            // CONTINUE
                ++nSignificandDigits;
            }

            // 'p' is on the 'E' of the exponent
            const int exp = atoi(p + 1);

            // Calculate C++ scientific length for '-f'
            const bool significandHasDecimals = nSignificandDigits > 1;
            const size_t calculatedScientificLength =
                1 +                                  // negative sign
                nSignificandDigits +                 // significand
                (significandHasDecimals ? 1 : 0) +   // radix mark
                1 +                                  // 'e'
                1 +                                  // sign for exponent
                2;                                  // exponent itself 'float'
                                                   // 'exponents are written 2
                                                  // digits always in C and C++
            ASSERTV(ryuBuf,
                    calculatedScientificLength,
                    (Util::ToCharsMaxLength<float,
                                            Util::e_SCIENTIFIC>::k_VALUE),
                    (calculatedScientificLength <=
                         Util::ToCharsMaxLength<float,
                                                Util::e_SCIENTIFIC>::k_VALUE));

            // The code below verifies that our above calculation agrees with
            // our actual scientific-writing code.
            {
                const size_t k_SCI_FLT_MAX =
                           Util::ToCharsMaxLength<float,
                                                  Util::e_SCIENTIFIC>::k_VALUE;
                char blpSciBuf[k_SCI_FLT_MAX + 1];
                const size_t blpScientificLength =
                                             blp_f2s_buffered_n(-f, blpSciBuf);
                blpSciBuf[blpScientificLength] = '\0';

                ASSERTV(ryuBuf, blpSciBuf,
                        calculatedScientificLength, blpScientificLength,
                        calculatedScientificLength == blpScientificLength);
            }

            // Calculate C++ fixed length for '-f'
            const bool isInteger = exp >= 0 && (nSignificandDigits - 1 <= exp);
            const bool noIntPart = exp < 0;
            // const bool hasIntAndFraction = !isInteger && !noIntPart;
            const size_t calculatedDecimalLength =
                1 +                                      // negative sign
                (isInteger
                    ? (exp + 1)                          // when integer
                    : noIntPart
                    ? (1 + nSignificandDigits - exp)     // when less than 1
                    : (nSignificandDigits + 1));         // nnn.nnnn

            ASSERTV(ryuBuf,
                    calculatedDecimalLength,
                    (Util::ToCharsMaxLength<float,
                                            Util::e_FIXED>::k_VALUE),
                    (calculatedScientificLength <=
                              Util::ToCharsMaxLength<float,
                                                     Util::e_FIXED>::k_VALUE));

            // The code below verifies that our above calculation agrees with
            // our actual decimal-format-writing code.  (C++ calls it 'fixed'
            // format, but without a precision parameter there is nothing
            // "fixed" about it, so we call it decimal here.)
            {
                const size_t k_DEC_FLT_MAX =
                                Util::ToCharsMaxLength<float,
                                                       Util::e_FIXED>::k_VALUE;
                char blpDecBuf[k_DEC_FLT_MAX + 1];
                const size_t blpDecimalLength = blp_f2d_buffered_n(-f,
                                                                   blpDecBuf);
                blpDecBuf[blpDecimalLength] = '\0';

                if ((calculatedDecimalLength != blpDecimalLength) &&
                    (1 == nSignificandDigits) &&
                    (exp > 9))
                {
                    // Some integer values are printed exactly (according to
                    // ISO C++ requirements), reducing the calculated length.
                    // In such case we print the integer value using original
                    // Ryu functionality.

                    const size_t blpDecimalIntLength =
                                      ryu_d2fixed_buffered_n(-f, 0, blpDecBuf);
                    blpDecBuf[blpDecimalIntLength] = '\0';

                    ASSERTV(ryuBuf, blpDecBuf,
                           blpDecimalLength, blpDecimalIntLength,
                           blpDecimalLength == blpDecimalIntLength);

                }
                else {
                    ASSERTV(ryuBuf, blpDecBuf,
                           calculatedDecimalLength, blpDecimalLength,
                           calculatedDecimalLength == blpDecimalLength);
                }
            }

            // Calculate C++ default format length for '-f'
            const size_t calculatedDefaultFormatLength =
                (calculatedScientificLength < calculatedDecimalLength)
                ? calculatedScientificLength : calculatedDecimalLength;

            ASSERTV(ryuBuf,
                    calculatedDefaultFormatLength,
                    (Util::ToCharsMaxLength<float>::k_VALUE),
                    (calculatedDefaultFormatLength <=
                                      Util::ToCharsMaxLength<float>::k_VALUE));

            // The code below verifies that our above calculation agrees with
            // our actual default-format-writing code.
            {
                const size_t k_DFL_FLT_MAX =
                                        Util::ToCharsMaxLength<float>::k_VALUE;
                char blpDflBuf[k_DFL_FLT_MAX + 1];
                const size_t blpDefaultLength = blp_f2m_buffered_n(-f,
                                                                   blpDflBuf);
                blpDflBuf[blpDefaultLength] = '\0';

                ASSERTV(ryuBuf, blpDflBuf,
                        calculatedDefaultFormatLength,   blpDefaultLength,
                        calculatedDefaultFormatLength == blpDefaultLength);
            }
        }
        sw.stop();
        const double wallTime = sw.accumulatedWallTime();
        const int wallHrs = static_cast<int>(wallTime / 3600);
        const int wallMin = static_cast<int>(wallTime / 60 - wallHrs * 60.);
        const int wallSec = static_cast<int>(fmod(wallTime, 60));
        printf("%dh %02dm %02ds\n", wallHrs, wallMin, wallSec);
#else
          puts("BUILD OPTIMIZED FOR BRUTE-FORCE ANALYSIS!");
#endif

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
