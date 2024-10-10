// bslstl_formatterfloating.t.cpp                                     -*-C++-*-
#include <bslfmt_formatterfloating.h>

#include <bslfmt_formatargs.h>
#include <bslfmt_formattertestutil.h> // Testing only

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <limits>

#include <stdio.h>
#include <string.h>

using namespace BloombergLP;
using namespace bsl;

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
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

/// Check whether the `bslfmt::formatter<t_TYPE, char>::parse` function works
/// as expected for the specified `format` string created at runtime.  The
/// specified `line` is used to identify the function call location.
template <class t_TYPE>
bool testRuntimeCharParse(int line, const char *format)
{
    bsl::string message;

    bool rv = bslfmt::Formatter_TestUtil<char>::testParseVFormat<t_TYPE>(
                                                                      &message,
                                                                      false,
                                                                      format);
    ASSERTV(line, format, message.c_str(), rv);

    return rv;
}

/// Check whether the `bslfmt::formatter<t_TYPE, wchar_t>::parse` function
/// works as expected for the specified `format` string created at runtime.
/// The specified `line` is used to identify the function call location.
template <class t_TYPE>
bool testRuntimeWcharParse(int line, const wchar_t *format)
{
    bsl::string message;

    bool rv = bslfmt::Formatter_TestUtil<wchar_t>::testParseVFormat<t_TYPE>(
                                                                      &message,
                                                                      false,
                                                                      format);
    ASSERTV(line, format, message.c_str(), rv);

    return rv;
}


/// Check whether the `bslfmt::formatter<t_TYPE, t_CHAR>::format` function
/// produces the specified `expected` result for the specified `format` and
/// `value`.  The specified `line` is used to identify the function call
/// location.
template <class t_CHAR, class t_TYPE>
bool testRuntimeFormat(int           line,
                       const t_CHAR *expected,
                       const t_CHAR *format,
                       t_TYPE        value)
{
    bsl::string message;
    int         dummyArg = 0;

    bool rv = bslfmt::Formatter_TestUtil<t_CHAR>::testEvaluateVFormat(
                                                                     &message,
                                                                     expected,
                                                                     true,
                                                                     format,
                                                                     value,
                                                                     dummyArg,
                                                                     dummyArg);
    ASSERTV(line, format, message.c_str(), rv);

    return rv;
}
}  // close unnamed namespace


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int  test            = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose         = argc > 2;
    const bool veryVerbose     = argc > 3;
    const bool veryVeryVerbose = argc > 4;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 1: {
        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");

        {
            bsl::formatter<double, char>    dummy;   (void)dummy;
            bsl::formatter<double, wchar_t> wdummy;  (void)wdummy;
        }

        {
            bsl::formatter<float, char>    dummy;   (void)dummy;
            bsl::formatter<float, wchar_t> wdummy;  (void)wdummy;
        }

        {
            bsl::formatter<long double, char>    dummy;   (void)dummy;
            bsl::formatter<long double, wchar_t> wdummy;  (void)wdummy;
        }

        typedef std::numeric_limits<double> DblLim;

        const double k_DBL_INF     = DblLim::infinity();
        const double k_DBL_NEG_INF = -k_DBL_INF;

        const double k_DBL_NAN     = DblLim::quiet_NaN();
        const double k_DBL_NEG_NAN = -k_DBL_NAN;

        const double k_DBL_SNAN     = DblLim::signaling_NaN();
        const double k_DBL_NEG_SNAN = -DblLim::signaling_NaN();
            // This probably won't be a signaling NaN as changing the sign may
            // actually count as a read, which (optionally signals and) turns
            // it into a quiet NaN.

        static struct {
            int            d_line;
            const char    *d_expected;
            const char    *d_format;
            const wchar_t *d_wexpected;
            const wchar_t *d_wformat;
            double         d_value;
        }  DOUBLE_TEST[] = {
#define ROW(expected, format, value)                      \
    { L_, expected, format, L##expected, L##format, value }

            // NO PRECISION

            // no format specifier
            ROW("0",   "{}",    0.),
            ROW("0",   "{:}",   0.),
            ROW("0",   "{:1}",  0.),
            ROW(" 0",  "{:2}",  0.),
            ROW("0 ",  "{:<2}", 0.),
            ROW("0  ", "{:<3}", 0.),
            ROW(" 0 ", "{:^3}", 0.),

            // Zero padding
            ROW("00",  "{:02}",  0.),
            ROW(" 0",  "{:>02}", 0.),
            ROW("0 ",  "{:<02}", 0.),
            ROW("0  ", "{:<03}", 0.),
            ROW(" 0 ", "{:^03}", 0.),

            // Zero padding for a negative value
            ROW("-01", "{:03}", -1.),
            // Zero padding for a positive value w/o sign
            ROW("01", "{:02}", 1.),
            // Zero padding for a positive value with sign
            ROW("+01", "{:+03}", 1.),
            // Zero padding for a positive value with space for sign
            ROW(" 01", "{: 03}", 1.),

            ROW("inf",  "{}", k_DBL_INF),
            ROW("-inf", "{}", k_DBL_NEG_INF),
            ROW("nan",  "{}", k_DBL_NAN),
            ROW("-nan", "{}", k_DBL_NEG_NAN),
            ROW("nan",  "{}", k_DBL_SNAN),
            ROW("-nan", "{}", k_DBL_NEG_SNAN),

            ROW("inf",  "{:3}", k_DBL_INF),
            ROW("-inf", "{:4}", k_DBL_NEG_INF),
            ROW("nan",  "{:3}", k_DBL_NAN),
            ROW("-nan", "{:4}", k_DBL_NEG_NAN),

            ROW(" inf",  "{:4}", k_DBL_INF),
            ROW(" -inf", "{:5}", k_DBL_NEG_INF),
            ROW(" nan",  "{:4}", k_DBL_NAN),
            ROW(" -nan", "{:5}", k_DBL_NEG_NAN),

            ROW(" inf",  "{:04}", k_DBL_INF),
            ROW(" -inf", "{:05}", k_DBL_NEG_INF),
            ROW(" nan",  "{:04}", k_DBL_NAN),
            ROW(" -nan", "{:05}", k_DBL_NEG_NAN),

            ROW("+inf", "{:+}", k_DBL_INF),
            ROW("-inf", "{:+}", k_DBL_NEG_INF),
            ROW("+nan", "{:+}", k_DBL_NAN),
            ROW("-nan", "{:+}", k_DBL_NEG_NAN),

            ROW(" inf", "{: }", k_DBL_INF),
            ROW("-inf", "{: }", k_DBL_NEG_INF),
            ROW(" nan", "{: }", k_DBL_NAN),
            ROW("-nan", "{: }", k_DBL_NEG_NAN),

            // hexfloat
            ROW("0p+0",   "{:a}",   0.),
            ROW("0p+0",   "{:4a}",  0.),
            ROW(" 0p+0",  "{:5a}",  0.),
            ROW("0p+0 ",  "{:<5a}", 0.),
            ROW("0p+0  ", "{:<6a}", 0.),
            ROW(" 0p+0 ", "{:^6a}", 0.),
            // uppercase hexfloat
            ROW("0P+0",   "{:A}",   0.),
            ROW("0P+0",   "{:4A}",  0.),
            ROW(" 0P+0",  "{:5A}",  0.),
            ROW("0P+0 ",  "{:<5A}", 0.),
            ROW("0P+0  ", "{:<6A}", 0.),
            ROW(" 0P+0 ", "{:^6A}", 0.),

            // hexfloat ALTERNATE
            ROW("0.p+0",   "{:#a}",   0.),
            ROW("0.p+0",   "{:#5a}",  0.),
            ROW(" 0.p+0",  "{:#6a}",  0.),
            ROW("0.p+0 ",  "{:<#6a}", 0.),
            ROW("0.p+0  ", "{:<#7a}", 0.),
            ROW(" 0.p+0 ", "{:^#7a}", 0.),
            // uppercase hexfloat ALTERNATE
            ROW("0.P+0",   "{:#A}",   0.),
            ROW("0.P+0",   "{:#5A}",  0.),
            ROW(" 0.P+0",  "{:#6A}",  0.),
            ROW("0.P+0 ",  "{:<#6A}", 0.),
            ROW("0.P+0  ", "{:<#7A}", 0.),
            ROW(" 0.P+0 ", "{:^#7A}", 0.),

            // scientific
            ROW("0.000000e+00",   "{:e}",    0.),
            ROW("0.000000e+00",   "{:12e}",  0.),
            ROW(" 0.000000e+00",  "{:13e}",  0.),
            ROW("0.000000e+00 ",  "{:<13e}", 0.),
            ROW("0.000000e+00  ", "{:<14e}", 0.),
            ROW(" 0.000000e+00 ", "{:^14e}", 0.),
            // uppercase scientific
            ROW("0.000000E+00",   "{:E}",    0.),
            ROW("0.000000E+00",   "{:12E}",  0.),
            ROW(" 0.000000E+00",  "{:13E}",  0.),
            ROW("0.000000E+00 ",  "{:<13E}", 0.),
            ROW("0.000000E+00  ", "{:<14E}", 0.),
            ROW(" 0.000000E+00 ", "{:^14E}", 0.),

            // scientific ALTERNATE
            ROW("0.000000e+00",   "{:#e}",    0.),
            ROW("0.000000e+00",   "{:#12e}",  0.),
            ROW(" 0.000000e+00",  "{:#13e}",  0.),
            ROW("0.000000e+00 ",  "{:<#13e}", 0.),
            ROW("0.000000e+00  ", "{:<#14e}", 0.),
            ROW(" 0.000000e+00 ", "{:^#14e}", 0.),
            // uppercase scientific ALTERNATE
            ROW("0.000000E+00",   "{:#E}",    0.),
            ROW("0.000000E+00",   "{:#12E}",  0.),
            ROW(" 0.000000E+00",  "{:#13E}",  0.),
            ROW("0.000000E+00 ",  "{:<#13E}", 0.),
            ROW("0.000000E+00  ", "{:<#14E}", 0.),
            ROW(" 0.000000E+00 ", "{:^#14E}", 0.),

            // fixed
            ROW("0.000000",   "{:f}",    0.),
            ROW("0.000000",   "{:8f}",   0.),
            ROW(" 0.000000",  "{:9f}",   0.),
            ROW("0.000000 ",  "{:<9f}",  0.),
            ROW("0.000000  ", "{:<10f}", 0.),
            ROW(" 0.000000 ", "{:^10f}", 0.),
            // uppercase fixed (same thing as fixed)
            ROW("0.000000",   "{:F}",    0.),
            ROW("0.000000",   "{:8F}",   0.),
            ROW(" 0.000000",  "{:9F}",   0.),
            ROW("0.000000 ",  "{:<9F}",  0.),
            ROW("0.000000  ", "{:<10F}", 0.),
            ROW(" 0.000000 ", "{:^10F}", 0.),
            // fixed ALTERNATE
            ROW("0.000000",   "{:#f}",    0.),
            ROW("0.000000",   "{: <#8f}", 0.),
            ROW(" 0.000000",  "{: #9f}",  0.),
            ROW("0.000000 ",  "{:<#9f}",  0.),
            ROW("0.000000  ", "{:<#10f}", 0.),
            ROW(" 0.000000 ", "{:^#10f}", 0.),
            // uppercase fixed (same thing as fixed)
            ROW("0.000000",   "{:#F}",    0.),
            ROW("0.000000",   "{: <#8F}", 0.),
            ROW(" 0.000000",  "{: #9F}",  0.),
            ROW("0.000000 ",  "{:<#9F}",  0.),
            ROW("0.000000  ", "{:<#10F}", 0.),
            ROW(" 0.000000 ", "{:^#10F}", 0.),

            // general
            ROW("0",   "{:g}",   0.),
            ROW("0",   "{:1g}",  0.),
            ROW(" 0",  "{:2g}",  0.),
            ROW("0 ",  "{:<2g}", 0.),
            ROW("0  ", "{:<3g}", 0.),
            ROW(" 0 ", "{:^3g}", 0.),
            // uppercase general
            ROW("0",   "{:G}",   0.),
            ROW("0",   "{:1G}",  0.),
            ROW(" 0",  "{:2G}",  0.),
            ROW("0 ",  "{:<2G}", 0.),
            ROW("0  ", "{:<3G}", 0.),
            ROW(" 0 ", "{:^3G}", 0.),

            // general ALTERNATE
            ROW("0.00000",   "{:#g}",   0.),
            ROW("0.00000",   "{:#7g}",  0.),
            ROW(" 0.00000",  "{:#8g}",  0.),
            ROW("0.00000 ",  "{:<#8g}", 0.),
            ROW("0.00000  ", "{:<#9g}", 0.),
            ROW(" 0.00000 ", "{:^#9g}", 0.),
            // uppercase general
            ROW("0.00000",   "{:#G}",   0.),
            ROW("0.00000",   "{:#7G}",  0.),
            ROW(" 0.00000",  "{:#8G}",  0.),
            ROW("0.00000 ",  "{:<#8G}", 0.),
            ROW("0.00000  ", "{:<#9G}", 0.),
            ROW(" 0.00000 ", "{:^#9G}", 0.),

            // general
            ROW("1.234e-37",   "{:g}",    1.234e-37),
            ROW("1.234e-37",   "{:9g}",   1.234e-37),
            ROW(" 1.234e-37",  "{:10g}",  1.234e-37),
            ROW("1.234e-37 ",  "{:<10g}", 1.234e-37),
            ROW("1.234e-37  ", "{:<11g}", 1.234e-37),
            ROW(" 1.234e-37 ", "{:^11g}", 1.234e-37),
            // uppercase general
            ROW("1.234E-37",   "{:G}",    1.234e-37),
            ROW("1.234E-37",   "{:9G}",   1.234e-37),
            ROW(" 1.234E-37",  "{:10G}",  1.234e-37),
            ROW("1.234E-37 ",  "{:<10G}", 1.234e-37),
            ROW("1.234E-37  ", "{:<11G}", 1.234e-37),
            ROW(" 1.234E-37 ", "{:^11G}", 1.234e-37),

            ROW("1.23457e-37", "{:g}", 1.2345678e-37),

            // general ALTERNATE
            ROW("1.23400e-37",   "{:#g}",    1.234e-37),
            ROW("1.23400e-37",   "{:#11g}",  1.234e-37),
            ROW(" 1.23400e-37",  "{:#12g}",  1.234e-37),
            ROW("1.23400e-37 ",  "{:<#12g}", 1.234e-37),
            ROW("1.23400e-37  ", "{:<#13g}", 1.234e-37),
            ROW(" 1.23400e-37 ", "{:^#13g}", 1.234e-37),
            // uppercase general
            ROW("1.23400E-37",   "{:#G}",    1.234e-37),
            ROW("1.23400E-37",   "{:#11G}",  1.234e-37),
            ROW(" 1.23400E-37",  "{:#12G}",  1.234e-37),
            ROW("1.23400E-37 ",  "{:<#12G}", 1.234e-37),
            ROW("1.23400E-37  ", "{:<#13G}", 1.234e-37),
            ROW(" 1.23400E-37 ", "{:^#13G}", 1.234e-37),

            ROW("1.23457e-37", "{:#g}", 1.2345678e-37),

            // WITH PRECISION
            // --------------

            // hexfloat .0
            ROW("0p+0",   "{:.0a}",   0.),
            ROW("0p+0",   "{:4.0a}",  0.),
            ROW(" 0p+0",  "{:5.0a}",  0.),
            ROW("0p+0 ",  "{:<5.0a}", 0.),
            ROW("0p+0  ", "{:<6.0a}", 0.),
            ROW(" 0p+0 ", "{:^6.0a}", 0.),
            // uppercase hexfloat .0
            ROW("0P+0",   "{:.0A}",   0.),
            ROW("0P+0",   "{:4.0A}",  0.),
            ROW(" 0P+0",  "{:5.0A}",  0.),
            ROW("0P+0 ",  "{:<5.0A}", 0.),
            ROW("0P+0  ", "{:<6.0A}", 0.),
            ROW(" 0P+0 ", "{:^6.0A}", 0.),

            // hexfloat .0 ALTERNATE
            ROW("0.p+0",   "{:#.0a}",   0.),
            ROW("0.p+0",   "{:#5.0a}",  0.),
            ROW(" 0.p+0",  "{:#6.0a}",  0.),
            ROW("0.p+0 ",  "{:<#6.0a}", 0.),
            ROW("0.p+0  ", "{:<#7.0a}", 0.),
            ROW(" 0.p+0 ", "{:^#7.0a}", 0.),
            // uppercase hexfloat .0 ALTERNATE
            ROW("0.P+0",   "{:#.0A}",   0.),
            ROW("0.P+0",   "{:#5.0A}",  0.),
            ROW(" 0.P+0",  "{:#6.0A}",  0.),
            ROW("0.P+0 ",  "{:<#6.0A}", 0.),
            ROW("0.P+0  ", "{:<#7.0A}", 0.),
            ROW(" 0.P+0 ", "{:^#7.0A}", 0.),

            // hexfloat .1
            ROW("0.0p+0",   "{:.1a}",   0.),
            ROW("0.0p+0",   "{:6.1a}",  0.),
            ROW(" 0.0p+0",  "{:7.1a}",  0.),
            ROW("0.0p+0 ",  "{:<7.1a}", 0.),
            ROW("0.0p+0  ", "{:<8.1a}", 0.),
            ROW(" 0.0p+0 ", "{:^8.1a}", 0.),
            // uppercase hexfloat .1
            ROW("0.0P+0",   "{:.1A}",   0.),
            ROW("0.0P+0",   "{:6.1A}",  0.),
            ROW(" 0.0P+0",  "{:7.1A}",  0.),
            ROW("0.0P+0 ",  "{:<7.1A}", 0.),
            ROW("0.0P+0  ", "{:<8.1A}", 0.),
            ROW(" 0.0P+0 ", "{:^8.1A}", 0.),

            // hexfloat .3
            ROW("0.000p+0",   "{:.3a}",    0.),
            ROW("0.000p+0",   "{:8.3a}",   0.),
            ROW(" 0.000p+0",  "{:9.3a}",   0.),
            ROW("0.000p+0 ",  "{:<9.3a}",  0.),
            ROW("0.000p+0  ", "{:<10.3a}", 0.),
            ROW(" 0.000p+0 ", "{:^10.3a}", 0.),
            // uppercase hexfloat .3
            ROW("0.000P+0",   "{:.3A}",    0.),
            ROW("0.000P+0",   "{:8.3A}",   0.),
            ROW(" 0.000P+0",  "{:9.3A}",   0.),
            ROW("0.000P+0 ",  "{:<9.3A}",  0.),
            ROW("0.000P+0  ", "{:<10.3A}", 0.),
            ROW(" 0.000P+0 ", "{:^10.3A}", 0.),

            // hexfloat .51
            ROW("0.000000000000000000000000000000000000000000000000000p+0",
                                                             "{:.51a}",    0.),
            ROW("0.000000000000000000000000000000000000000000000000000p+0",
                                                             "{:56.51a}",  0.),
            ROW(" 0.000000000000000000000000000000000000000000000000000p+0",
                                                             "{:57.51a}",  0.),
            ROW("0.000000000000000000000000000000000000000000000000000p+0 ",
                                                             "{:<57.51a}", 0.),
            ROW("0.000000000000000000000000000000000000000000000000000p+0  ",
                                                             "{:<58.51a}", 0.),
            ROW(" 0.000000000000000000000000000000000000000000000000000p+0 ",
                                                             "{:^58.51a}", 0.),
            // uppercase hexfloat .51
            ROW("0.000000000000000000000000000000000000000000000000000P+0",
                                                             "{:.51A}",    0.),
            ROW("0.000000000000000000000000000000000000000000000000000P+0",
                                                             "{:56.51A}",  0.),
            ROW(" 0.000000000000000000000000000000000000000000000000000P+0",
                                                             "{:57.51A}",  0.),
            ROW("0.000000000000000000000000000000000000000000000000000P+0 ",
                                                             "{:<57.51A}", 0.),
            ROW("0.000000000000000000000000000000000000000000000000000P+0  ",
                                                             "{:<58.51A}", 0.),
            ROW(" 0.000000000000000000000000000000000000000000000000000P+0 ",
                                                             "{:^58.51A}", 0.),

            // scientific .0
            ROW("0e+00",   "{:.0e}",   0.),
            ROW("0e+00",   "{:5.0e}",  0.),
            ROW(" 0e+00",  "{:6.0e}",  0.),
            ROW("0e+00 ",  "{:<6.0e}", 0.),
            ROW("0e+00  ", "{:<7.0e}", 0.),
            ROW(" 0e+00 ", "{:^7.0e}", 0.),
            // uppercase scientific .0
            ROW("0E+00",   "{:.0E}",   0.),
            ROW("0E+00",   "{:5.0E}",  0.),
            ROW(" 0E+00",  "{:6.0E}",  0.),
            ROW("0E+00 ",  "{:<6.0E}", 0.),
            ROW("0E+00  ", "{:<7.0E}", 0.),
            ROW(" 0E+00 ", "{:^7.0E}", 0.),

            // scientific .0 ALTERNATE
            ROW("0.e+00",   "{:#.0e}",   0.),
            ROW("0.e+00",   "{:#6.0e}",  0.),
            ROW(" 0.e+00",  "{:#7.0e}",  0.),
            ROW("0.e+00 ",  "{:<#7.0e}", 0.),
            ROW("0.e+00  ", "{:<#8.0e}", 0.),
            ROW(" 0.e+00 ", "{:^#8.0e}", 0.),
            // uppercase scientific .0
            ROW("0.E+00",   "{:#.0E}",   0.),
            ROW("0.E+00",   "{:#6.0E}",  0.),
            ROW(" 0.E+00",  "{:#7.0E}",  0.),
            ROW("0.E+00 ",  "{:<#7.0E}", 0.),
            ROW("0.E+00  ", "{:<#8.0E}", 0.),
            ROW(" 0.E+00 ", "{:^#8.0E}", 0.),

            // scientific .1
            ROW("0.0e+00",   "{:.1e}",   0.),
            ROW("0.0e+00",   "{:7.1e}",  0.),
            ROW(" 0.0e+00",  "{:8.1e}",  0.),
            ROW("0.0e+00 ",  "{:<8.1e}", 0.),
            ROW("0.0e+00  ", "{:<9.1e}", 0.),
            ROW(" 0.0e+00 ", "{:^9.1e}", 0.),
            // uppercase scientific .1
            ROW("0.0E+00",   "{:.1E}",   0.),
            ROW("0.0E+00",   "{:7.1E}",  0.),
            ROW(" 0.0E+00",  "{:8.1E}",  0.),
            ROW("0.0E+00 ",  "{:<8.1E}", 0.),
            ROW("0.0E+00  ", "{:<9.1E}", 0.),
            ROW(" 0.0E+00 ", "{:^9.1E}", 0.),

            // scientific .1 ALTERNATE
            ROW("0.0e+00",   "{:#.1e}",   0.),
            ROW("0.0e+00",   "{:#7.1e}",  0.),
            ROW(" 0.0e+00",  "{:#8.1e}",  0.),
            ROW("0.0e+00 ",  "{:<#8.1e}", 0.),
            ROW("0.0e+00  ", "{:<#9.1e}", 0.),
            ROW(" 0.0e+00 ", "{:^#9.1e}", 0.),
            // uppercase scientific .1 ALTERNATE
            ROW("0.0E+00",   "{:#.1E}",   0.),
            ROW("0.0E+00",   "{:#7.1E}",  0.),
            ROW(" 0.0E+00",  "{:#8.1E}",  0.),
            ROW("0.0E+00 ",  "{:<#8.1E}", 0.),
            ROW("0.0E+00  ", "{:<#9.1E}", 0.),
            ROW(" 0.0E+00 ", "{:^#9.1E}", 0.),

            // scientific .17
            ROW("0.00000000000000000e+00",   "{:.17e}" ,   0.),
            ROW("0.00000000000000000e+00",   "{:23.17e}",  0.),
            ROW(" 0.00000000000000000e+00",  "{:24.17e}",  0.),
            ROW("0.00000000000000000e+00 ",  "{:<24.17e}", 0.),
            ROW("0.00000000000000000e+00  ", "{:<25.17e}", 0.),
            ROW(" 0.00000000000000000e+00 ", "{:^25.17e}", 0.),
            // uppercase scientific .17
            ROW("0.00000000000000000E+00",   "{:.17E}" ,   0.),
            ROW("0.00000000000000000E+00",   "{:23.17E}",  0.),
            ROW(" 0.00000000000000000E+00",  "{:24.17E}",  0.),
            ROW("0.00000000000000000E+00 ",  "{:<24.17E}", 0.),
            ROW("0.00000000000000000E+00  ", "{:<25.17E}", 0.),
            ROW(" 0.00000000000000000E+00 ", "{:^25.17E}", 0.),

            // scientific .50
            ROW("0.00000000000000000000000000000000000000000000000000e+00",
                                                             "{:.50e}" ,   0.),
            ROW("0.00000000000000000000000000000000000000000000000000e+00",
                                                             "{:56.50e}",  0.),
            ROW(" 0.00000000000000000000000000000000000000000000000000e+00",
                                                             "{:57.50e}",  0.),
            ROW("0.00000000000000000000000000000000000000000000000000e+00 ",
                                                             "{:<57.50e}", 0.),
            ROW("0.00000000000000000000000000000000000000000000000000e+00  ",
                                                             "{:<58.50e}", 0.),
            ROW(" 0.00000000000000000000000000000000000000000000000000e+00 ",
                                                             "{:^58.50e}", 0.),
            // uppercase scientific .50
            ROW("0.00000000000000000000000000000000000000000000000000E+00",
                                                             "{:.50E}" ,   0.),
            ROW("0.00000000000000000000000000000000000000000000000000E+00",
                                                             "{:56.50E}",  0.),
            ROW(" 0.00000000000000000000000000000000000000000000000000E+00",
                                                             "{:57.50E}",  0.),
            ROW("0.00000000000000000000000000000000000000000000000000E+00 ",
                                                             "{:<57.50E}", 0.),
            ROW("0.00000000000000000000000000000000000000000000000000E+00  ",
                                                             "{:<58.50E}", 0.),
            ROW(" 0.00000000000000000000000000000000000000000000000000E+00 ",
                                                             "{:^58.50E}", 0.),

            // general .6
            ROW("1.234e-37",   "{:.6g}",    1.234e-37),
            ROW("1.234e-37",   "{:9.6g}",   1.234e-37),
            ROW(" 1.234e-37",  "{:10.6g}",  1.234e-37),
            ROW("1.234e-37 ",  "{:<10.6g}", 1.234e-37),
            ROW("1.234e-37  ", "{:<11.6g}", 1.234e-37),
            ROW(" 1.234e-37 ", "{:^11.6g}", 1.234e-37),
            // uppercase general .6
            ROW("1.234E-37",   "{:.6G}",    1.234e-37),
            ROW("1.234E-37",   "{:9.6G}",   1.234e-37),
            ROW(" 1.234E-37",  "{:10.6G}",  1.234e-37),
            ROW("1.234E-37 ",  "{:<10.6G}", 1.234e-37),
            ROW("1.234E-37  ", "{:<11.6G}", 1.234e-37),
            ROW(" 1.234E-37 ", "{:^11.6G}", 1.234e-37),

            ROW("1.23457e-37", "{:.6g}", 1.2345678e-37),

            // general .6 ALTERNATE
            ROW("1.23400e-37",   "{:#.6g}",    1.234e-37),
            ROW("1.23400e-37",   "{:#11.6g}",  1.234e-37),
            ROW(" 1.23400e-37",  "{:#12.6g}",  1.234e-37),
            ROW("1.23400e-37 ",  "{:<#12.6g}", 1.234e-37),
            ROW("1.23400e-37  ", "{:<#13.6g}", 1.234e-37),
            ROW(" 1.23400e-37 ", "{:^#13.6g}", 1.234e-37),
            // uppercase general .6 ALTERNATE
            ROW("1.23400E-37",   "{:#.6G}",    1.234e-37),
            ROW("1.23400E-37",   "{:#11.6G}",  1.234e-37),
            ROW(" 1.23400E-37",  "{:#12.6G}",  1.234e-37),
            ROW("1.23400E-37 ",  "{:<#12.6G}", 1.234e-37),
            ROW("1.23400E-37  ", "{:<#13.6G}", 1.234e-37),
            ROW(" 1.23400E-37 ", "{:^#13.6G}", 1.234e-37),

            ROW("1.23457e-37", "{:#.6g}", 1.2345678e-37),

            // general .0
            ROW("1e-37",   "{:.0g}",   1.234e-37),
            ROW("1e-37",   "{:5.0g}",  1.234e-37),
            ROW(" 1e-37",  "{:6.0g}",  1.234e-37),
            ROW("1e-37 ",  "{:<6.0g}", 1.234e-37),
            ROW("1e-37  ", "{:<7.0g}", 1.234e-37),
            ROW(" 1e-37 ", "{:^7.0g}", 1.234e-37),
            // uppercase general .0
            ROW("1E-37",   "{:.0G}",   1.234E-37),
            ROW("1E-37",   "{:5.0G}",  1.234E-37),
            ROW(" 1E-37",  "{:6.0G}",  1.234E-37),
            ROW("1E-37 ",  "{:<6.0G}", 1.234E-37),
            ROW("1E-37  ", "{:<7.0G}", 1.234E-37),
            ROW(" 1E-37 ", "{:^7.0G}", 1.234E-37),

            // general .0 ALTERNATE
            ROW("1.e-37",   "{:#.0g}",   1.234e-37),
#if defined(_MSC_FULL_VER) && _MSC_FULL_VER > 194134123
    // MSVC as Oracle has a bug with width when precision is 0
            ROW("1.e-37",   "{:#6.0g}",  1.234e-37),
            ROW(" 1.e-37",  "{:#7.0g}",  1.234e-37),
            ROW("1.e-37 ",  "{:<#7.0g}", 1.234e-37),
            ROW("1.e-37  ", "{:<#8.0g}", 1.234e-37),
            ROW(" 1.e-37 ", "{:^#8.0g}", 1.234e-37),
#endif
            // uppercase general .0 ALTERNATE
            ROW("1.E-37",   "{:#.0G}",   1.234e-37),
#if defined(_MSC_FULL_VER) && _MSC_FULL_VER > 194134123
    // MSVC as Oracle has a bug with width when precision is 0
            ROW("1.E-37",   "{:#6.0G}",  1.234e-37),
            ROW(" 1.E-37",  "{:#7.0G}",  1.234e-37),
            ROW("1.E-37 ",  "{:<#7.0G}", 1.234e-37),
            ROW("1.E-37  ", "{:<#8.0G}", 1.234e-37),
            ROW(" 1.E-37 ", "{:^#8.0G}", 1.234e-37),
#endif

            // general .1
            ROW("1e-37",   "{:.1g}",   1.234e-37),
            ROW("1e-37",   "{:5.1g}",  1.234e-37),
            ROW(" 1e-37",  "{:6.1g}",  1.234e-37),
            ROW("1e-37 ",  "{:<6.1g}", 1.234e-37),
            ROW("1e-37  ", "{:<7.1g}", 1.234e-37),
            ROW(" 1e-37 ", "{:^7.1g}", 1.234e-37),
            // uppercase general .1
            ROW("1E-37",   "{:.1G}",   1.234E-37),
            ROW("1E-37",   "{:5.1G}",  1.234E-37),
            ROW(" 1E-37",  "{:6.1G}",  1.234E-37),
            ROW("1E-37 ",  "{:<6.1G}", 1.234E-37),
            ROW("1E-37  ", "{:<7.1G}", 1.234E-37),
            ROW(" 1E-37 ", "{:^7.1G}", 1.234E-37),

            // general .1 ALTERNATE
            ROW("1.e-37",   "{:#.1g}",   1.234e-37),
            ROW("1.e-37",   "{:#6.1g}",  1.234e-37),
            ROW(" 1.e-37",  "{:#7.1g}",  1.234e-37),
            ROW("1.e-37 ",  "{:<#7.1g}", 1.234e-37),
            ROW("1.e-37  ", "{:<#8.1g}", 1.234e-37),
            ROW(" 1.e-37 ", "{:^#8.1g}", 1.234e-37),
            // uppercase general .1
            ROW("1.E-37",   "{:#.1G}",   1.234E-37),
            ROW("1.E-37",   "{:#6.1G}",  1.234E-37),
            ROW(" 1.E-37",  "{:#7.1G}",  1.234E-37),
            ROW("1.E-37 ",  "{:<#7.1G}", 1.234E-37),
            ROW("1.E-37  ", "{:<#8.1G}", 1.234E-37),
            ROW(" 1.E-37 ", "{:^#8.1G}", 1.234E-37),

            // general .2
            ROW("1.2e-37",   "{:.2g}",   1.234e-37),
            ROW("1.2e-37",   "{:7.2g}",  1.234e-37),
            ROW(" 1.2e-37",  "{:8.2g}",  1.234e-37),
            ROW("1.2e-37 ",  "{:<8.2g}", 1.234e-37),
            ROW("1.2e-37  ", "{:<9.2g}", 1.234e-37),
            ROW(" 1.2e-37 ", "{:^9.2g}", 1.234e-37),
            // uppercase general .2
            ROW("1.2E-37",   "{:.2G}",   1.234E-37),
            ROW("1.2E-37",   "{:7.2G}",  1.234E-37),
            ROW(" 1.2E-37",  "{:8.2G}",  1.234E-37),
            ROW("1.2E-37 ",  "{:<8.2G}", 1.234E-37),
            ROW("1.2E-37  ", "{:<9.2G}", 1.234E-37),
            ROW(" 1.2E-37 ", "{:^9.2G}", 1.234E-37),

            // general .2 ALTERNATE
            ROW("1.2e-37",   "{:#.2g}",   1.234e-37),
            ROW("1.2e-37",   "{:#7.2g}",  1.234e-37),
            ROW(" 1.2e-37",  "{:#8.2g}",  1.234e-37),
            ROW("1.2e-37 ",  "{:<#8.2g}", 1.234e-37),
            ROW("1.2e-37  ", "{:<#9.2g}", 1.234e-37),
            ROW(" 1.2e-37 ", "{:^#9.2g}", 1.234e-37),
            // uppercase general .2 ALTERNATE
            ROW("1.2E-37",   "{:#.2G}",   1.234E-37),
            ROW("1.2E-37",   "{:#7.2G}",  1.234E-37),
            ROW(" 1.2E-37",  "{:#8.2G}",  1.234E-37),
            ROW("1.2E-37 ",  "{:<#8.2G}", 1.234E-37),
            ROW("1.2E-37  ", "{:<#9.2G}", 1.234E-37),
            ROW(" 1.2E-37 ", "{:^#9.2G}", 1.234E-37),

            // general .50
            ROW("1.2340000000000000187836107844281224919182953523414e-37",
                                                      "{:.50g}",    1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414e-37",
                                                      "{:55.50g}",  1.234e-37),
            ROW(" 1.2340000000000000187836107844281224919182953523414e-37",
                                                      "{:56.50g}",  1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414e-37 ",
                                                      "{:<56.50g}", 1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414e-37  ",
                                                      "{:<57.50g}", 1.234e-37),
            ROW(" 1.2340000000000000187836107844281224919182953523414e-37 ",
                                                      "{:^57.50g}", 1.234e-37),
            // uppercase general .50
            ROW("1.2340000000000000187836107844281224919182953523414E-37",
                                                      "{:.50G}",    1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414E-37",
                                                      "{:55.50G}",  1.234e-37),
            ROW(" 1.2340000000000000187836107844281224919182953523414E-37",
                                                      "{:56.50G}",  1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414E-37 ",
                                                      "{:<56.50G}", 1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414E-37  ",
                                                      "{:<57.50G}", 1.234e-37),
            ROW(" 1.2340000000000000187836107844281224919182953523414E-37 ",
                                                      "{:^57.50G}", 1.234e-37),

            // general .50 ALTERNATE
            ROW("1.2340000000000000187836107844281224919182953523414e-37",
                                                     "{:#.50g}",    1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414e-37",
                                                     "{:#55.50g}",  1.234e-37),
            ROW(" 1.2340000000000000187836107844281224919182953523414e-37",
                                                     "{:#56.50g}",  1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414e-37 ",
                                                     "{:<#56.50g}", 1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414e-37  ",
                                                     "{:<#57.50g}", 1.234e-37),
            ROW(" 1.2340000000000000187836107844281224919182953523414e-37 ",
                                                     "{:^#57.50g}", 1.234e-37),
            // uppercase general .50 ALTERNATE
            ROW("1.2340000000000000187836107844281224919182953523414E-37",
                                                     "{:#.50G}",    1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414E-37",
                                                     "{:#55.50G}",  1.234e-37),
            ROW(" 1.2340000000000000187836107844281224919182953523414E-37",
                                                    "{:#56.50G}",  1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414E-37 ",
                                                     "{:<#56.50G}", 1.234e-37),
            ROW("1.2340000000000000187836107844281224919182953523414E-37  ",
                                                     "{:<#57.50G}", 1.234e-37),
            ROW(" 1.2340000000000000187836107844281224919182953523414E-37 ",
                                                     "{:^#57.50G}", 1.234e-37),

            // fixed .6
            ROW("0.000000",   "{:.6f}",    0.),
            ROW("0.000000",   "{:8.6f}",   0.),
            ROW(" 0.000000",  "{:9.6f}",   0.),
            ROW("0.000000 ",  "{:<9.6f}",  0.),
            ROW("0.000000  ", "{:<10.6f}", 0.),
            ROW(" 0.000000 ", "{:^10.6f}", 0.),
            // uppercase fixed .6 (same as fixed)
            ROW("0.000000",   "{:.6F}",    0.),
            ROW("0.000000",   "{:8.6F}",   0.),
            ROW(" 0.000000",  "{:9.6F}",   0.),
            ROW("0.000000 ",  "{:<9.6F}",  0.),
            ROW("0.000000  ", "{:<10.6F}", 0.),
            ROW(" 0.000000 ", "{:^10.6F}", 0.),

            // fixed .6 ALTERNATE
            ROW("0.000000",   "{:#.6f}",    0.),
            ROW("0.000000",   "{:#8.6f}",   0.),
            ROW(" 0.000000",  "{:#9.6f}",   0.),
            ROW("0.000000 ",  "{:<#9.6f}",  0.),
            ROW("0.000000  ", "{:<#10.6f}", 0.),
            ROW(" 0.000000 ", "{:^#10.6f}", 0.),
            // uppercase fixed .6 ALTERNATE (same as fixed)
            ROW("0.000000",   "{:#.6F}",    0.),
            ROW("0.000000",   "{:#8.6F}",   0.),
            ROW(" 0.000000",  "{:#9.6F}",   0.),
            ROW("0.000000 ",  "{:<#9.6F}",  0.),
            ROW("0.000000  ", "{:<#10.6F}", 0.),
            ROW(" 0.000000 ", "{:^#10.6F}", 0.),

            // fixed .0
            ROW("0",   "{:.0f}",   0.),
            ROW("0",   "{:1.0f}",  0.),
            ROW(" 0",  "{:2.0f}",  0.),
            ROW("0 ",  "{:<2.0f}", 0.),
            ROW("0  ", "{:<3.0f}", 0.),
            ROW(" 0 ", "{:^3.0f}", 0.),
            // uppercase fixed .6 (same as fixed)
            ROW("0",   "{:.0F}",   0.),
            ROW("0",   "{:1.0F}",  0.),
            ROW(" 0",  "{:2.0F}",  0.),
            ROW("0 ",  "{:<2.0F}", 0.),
            ROW("0  ", "{:<3.0F}", 0.),
            ROW(" 0 ", "{:^3.0F}", 0.),
            // fixed .0 ALTERNATE
            ROW("0.",   "{:#.0f}",   0.),
            ROW("0.",   "{:#2.0f}",  0.),
            ROW(" 0.",  "{:#3.0f}",  0.),
            ROW("0. ",  "{:<#3.0f}", 0.),
            ROW("0.  ", "{:<#4.0f}", 0.),
            ROW(" 0. ", "{:^#4.0f}", 0.),
            // uppercase fixed .6 (same as fixed)
            ROW("0.",   "{:#.0F}",   0.),
            ROW("0.",   "{:#2.0F}",  0.),
            ROW(" 0.",  "{:#3.0F}",  0.),
            ROW("0. ",  "{:<#3.0F}", 0.),
            ROW("0.  ", "{:<#4.0F}", 0.),
            ROW(" 0. ", "{:^#4.0F}", 0.),

            // fixed .54
            ROW("0.000000000000000000000000000000000000000000000000000000",
                                                             "{:.54f}",    0.),
            ROW("0.000000000000000000000000000000000000000000000000000000",
                                                             "{:56.54f}",  0.),
            ROW(" 0.000000000000000000000000000000000000000000000000000000",
                                                             "{:57.54f}",  0.),
            ROW("0.000000000000000000000000000000000000000000000000000000 ",
                                                             "{:<57.54f}", 0.),
            ROW("0.000000000000000000000000000000000000000000000000000000  ",
                                                             "{:<58.54f}", 0.),
            ROW(" 0.000000000000000000000000000000000000000000000000000000 ",
                                                             "{:^58.54f}", 0.),
            // uppercase fixed .6 (same as fixed)
            ROW("0.000000000000000000000000000000000000000000000000000000",
                                                             "{:.54F}",    0.),
            ROW("0.000000000000000000000000000000000000000000000000000000",
                                                             "{:56.54F}",  0.),
            ROW(" 0.000000000000000000000000000000000000000000000000000000",
                                                             "{:57.54F}",  0.),
            ROW("0.000000000000000000000000000000000000000000000000000000 ",
                                                             "{:<57.54F}", 0.),
            ROW("0.000000000000000000000000000000000000000000000000000000  ",
                                                             "{:<58.54F}", 0.),
            ROW(" 0.000000000000000000000000000000000000000000000000000000 ",
                                                             "{:^58.54F}", 0.),
        };
        const size_t NUM_DOUBLE_TESTS =
                                      sizeof DOUBLE_TEST / sizeof *DOUBLE_TEST;

        if (veryVerbose) puts("\tTesting `double` to `char`");
        for (size_t ti = 0; ti < NUM_DOUBLE_TESTS; ++ti) {
            const int          LINE     = DOUBLE_TEST[ti].d_line;
            const char * const EXPECTED = DOUBLE_TEST[ti].d_expected;
            const char * const FORMAT   = DOUBLE_TEST[ti].d_format;
            const double       VALUE    = DOUBLE_TEST[ti].d_value;

            if (veryVeryVerbose) {
                P_(LINE) P_(EXPECTED) P_(FORMAT) P(VALUE);
            }

            testRuntimeCharParse<double>(LINE, FORMAT);
            testRuntimeFormat(LINE, EXPECTED, FORMAT, VALUE);
        }

        if (veryVerbose) puts("\tTesting `double` to `wchar_t`");
        for (size_t ti = 0; ti < NUM_DOUBLE_TESTS; ++ti) {
            const int             LINE      = DOUBLE_TEST[ti].d_line;
            const wchar_t * const WEXPECTED = DOUBLE_TEST[ti].d_wexpected;
            const wchar_t * const WFORMAT   = DOUBLE_TEST[ti].d_wformat;
            const double          VALUE     = DOUBLE_TEST[ti].d_value;

            if (veryVeryVerbose) {
                P_(LINE) P(VALUE);
            }

            testRuntimeWcharParse<double>(LINE, WFORMAT);
            testRuntimeFormat(LINE, WEXPECTED, WFORMAT, VALUE);
        }

        // =========================== `float` ================================

        typedef std::numeric_limits<float> FltLim;

        const float k_FLT_INF     = FltLim::infinity();
        const float k_FLT_NEG_INF = -k_DBL_INF;

        const float k_FLT_NAN     = FltLim::quiet_NaN();
        const float k_FLT_NEG_NAN = -k_DBL_NAN;

        const float k_FLT_SNAN     = FltLim::signaling_NaN();
        const float k_FLT_NEG_SNAN = -FltLim::signaling_NaN();
            // This probably won't be a signaling NaN as changing the sign may
            // actually count as a read, which (optionally signals and) turns
            // it into a quiet NaN.

        static struct {
            int            d_line;
            const char    *d_expected;
            const char    *d_format;
            const wchar_t *d_wexpected;
            const wchar_t *d_wformat;
            float          d_value;
        }  FLOAT_TEST[] = {
#define ROW(expected, format, value)                      \
    { L_, expected, format, L##expected, L##format, value }

            // NO PRECISION

            // no format specifier
            ROW("0",   "{:}",   0.f),
            ROW("0",   "{:1}",  0.f),
            ROW(" 0",  "{:2}",  0.f),
            ROW("0 ",  "{:<2}", 0.f),
            ROW("0  ", "{:<3}", 0.f),
            ROW(" 0 ", "{:^3}", 0.f),

            ROW("inf",  "{}", k_FLT_INF),
            ROW("-inf", "{}", k_FLT_NEG_INF),
            ROW("nan",  "{}", k_FLT_NAN),
            ROW("-nan", "{}", k_FLT_NEG_NAN),
            ROW("nan",  "{}", k_FLT_SNAN),
            ROW("-nan", "{}", k_FLT_NEG_SNAN),

            ROW("inf",  "{:3}", k_FLT_INF),
            ROW("-inf", "{:4}", k_FLT_NEG_INF),
            ROW("nan",  "{:3}", k_FLT_NAN),
            ROW("-nan", "{:4}", k_FLT_NEG_NAN),

            ROW("+inf", "{:+}", k_FLT_INF),
            ROW("-inf", "{:+}", k_FLT_NEG_INF),
            ROW("+nan", "{:+}", k_FLT_NAN),
            ROW("-nan", "{:+}", k_FLT_NEG_NAN),

            ROW(" inf", "{: }", k_FLT_INF),
            ROW("-inf", "{: }", k_FLT_NEG_INF),
            ROW(" nan", "{: }", k_FLT_NAN),
            ROW("-nan", "{: }", k_FLT_NEG_NAN),

            // hexfloat
            ROW("0p+0",   "{:a}",   0.f),
            ROW("0p+0",   "{:4a}",  0.f),
            ROW(" 0p+0",  "{:5a}",  0.f),
            ROW("0p+0 ",  "{:<5a}", 0.f),
            ROW("0p+0  ", "{:<6a}", 0.f),
            ROW(" 0p+0 ", "{:^6a}", 0.f),
            // uppercase hexfloat
            ROW("0P+0",   "{:A}",   0.f),
            ROW("0P+0",   "{:4A}",  0.f),
            ROW(" 0P+0",  "{:5A}",  0.f),
            ROW("0P+0 ",  "{:<5A}", 0.f),
            ROW("0P+0  ", "{:<6A}", 0.f),
            ROW(" 0P+0 ", "{:^6A}", 0.f),

            // hexfloat ALTERNATE
            ROW("0.p+0",   "{:#a}",   0.f),
            ROW("0.p+0",   "{:#5a}",  0.f),
            ROW(" 0.p+0",  "{:#6a}",  0.f),
            ROW("0.p+0 ",  "{:<#6a}", 0.f),
            ROW("0.p+0  ", "{:<#7a}", 0.f),
            ROW(" 0.p+0 ", "{:^#7a}", 0.f),
            // uppercase hexfloat
            ROW("0.P+0",   "{:#A}",   0.f),
            ROW("0.P+0",   "{:#5A}",  0.f),
            ROW(" 0.P+0",  "{:#6A}",  0.f),
            ROW("0.P+0 ",  "{:<#6A}", 0.f),
            ROW("0.P+0  ", "{:<#7A}", 0.f),
            ROW(" 0.P+0 ", "{:^#7A}", 0.f),

            // scientific
            ROW("0.000000e+00",   "{:e}",    0.f),
            ROW("0.000000e+00",   "{:12e}",  0.f),
            ROW(" 0.000000e+00",  "{:13e}",  0.f),
            ROW("0.000000e+00 ",  "{:<13e}", 0.f),
            ROW("0.000000e+00  ", "{:<14e}", 0.f),
            ROW(" 0.000000e+00 ", "{:^14e}", 0.f),
            // uppercase scientific
            ROW("0.000000E+00",   "{:E}",    0.f),
            ROW("0.000000E+00",   "{:12E}",  0.f),
            ROW(" 0.000000E+00",  "{:13E}",  0.f),
            ROW("0.000000E+00 ",  "{:<13E}", 0.f),
            ROW("0.000000E+00  ", "{:<14E}", 0.f),
            ROW(" 0.000000E+00 ", "{:^14E}", 0.f),

            // scientific ALTERNATE
            ROW("0.000000e+00",   "{:#e}",    0.f),
            ROW("0.000000e+00",   "{:#12e}",  0.f),
            ROW(" 0.000000e+00",  "{:#13e}",  0.f),
            ROW("0.000000e+00 ",  "{:<#13e}", 0.f),
            ROW("0.000000e+00  ", "{:<#14e}", 0.f),
            ROW(" 0.000000e+00 ", "{:^#14e}", 0.f),
            // uppercase scientific ALTERNATE
            ROW("0.000000E+00",   "{:#E}",    0.f),
            ROW("0.000000E+00",   "{:#12E}",  0.f),
            ROW(" 0.000000E+00",  "{:#13E}",  0.f),
            ROW("0.000000E+00 ",  "{:<#13E}", 0.f),
            ROW("0.000000E+00  ", "{:<#14E}", 0.f),
            ROW(" 0.000000E+00 ", "{:^#14E}", 0.f),

            // fixed
            ROW("0.000000",   "{:f}",    0.f),
            ROW("0.000000",   "{:8f}",   0.f),
            ROW(" 0.000000",  "{:9f}",   0.f),
            ROW("0.000000 ",  "{:<9f}",  0.f),
            ROW("0.000000  ", "{:<10f}", 0.f),
            ROW(" 0.000000 ", "{:^10f}", 0.f),
            // uppercase fixed (same thing as fixed)
            ROW("0.000000",   "{:F}",    0.f),
            ROW("0.000000",   "{:8F}",   0.f),
            ROW(" 0.000000",  "{:9F}",   0.f),
            ROW("0.000000 ",  "{:<9F}",  0.f),
            ROW("0.000000  ", "{:<10F}", 0.f),
            ROW(" 0.000000 ", "{:^10F}", 0.f),
            // fixed ALTERNATE
            ROW("0.000000",   "{:#f}",    0.f),
            ROW("0.000000",   "{: <#8f}", 0.f),
            ROW(" 0.000000",  "{: #9f}",  0.f),
            ROW("0.000000 ",  "{:<#9f}",  0.f),
            ROW("0.000000  ", "{:<#10f}", 0.f),
            ROW(" 0.000000 ", "{:^#10f}", 0.f),
            // uppercase fixed (same thing as fixed)
            ROW("0.000000",   "{:#F}",    0.f),
            ROW("0.000000",   "{: <#8F}", 0.f),
            ROW(" 0.000000",  "{: #9F}",  0.f),
            ROW("0.000000 ",  "{:<#9F}",  0.f),
            ROW("0.000000  ", "{:<#10F}", 0.f),
            ROW(" 0.000000 ", "{:^#10F}", 0.f),

            // general
            ROW("0",   "{:g}",   0.f),
            ROW("0",   "{:1g}",  0.f),
            ROW(" 0",  "{:2g}",  0.f),
            ROW("0 ",  "{:<2g}", 0.f),
            ROW("0  ", "{:<3g}", 0.f),
            ROW(" 0 ", "{:^3g}", 0.f),
            // uppercase general
            ROW("0",   "{:G}",   0.f),
            ROW("0",   "{:1G}",  0.f),
            ROW(" 0",  "{:2G}",  0.f),
            ROW("0 ",  "{:<2G}", 0.f),
            ROW("0  ", "{:<3G}", 0.f),
            ROW(" 0 ", "{:^3G}", 0.f),

            // general ALTERNATE
            ROW("0.00000",   "{:#g}",   0.f),
            ROW("0.00000",   "{:#7g}",  0.f),
            ROW(" 0.00000",  "{:#8g}",  0.f),
            ROW("0.00000 ",  "{:<#8g}", 0.f),
            ROW("0.00000  ", "{:<#9g}", 0.f),
            ROW(" 0.00000 ", "{:^#9g}", 0.f),
            // uppercase general
            ROW("0.00000",   "{:#G}",   0.f),
            ROW("0.00000",   "{:#7G}",  0.f),
            ROW(" 0.00000",  "{:#8G}",  0.f),
            ROW("0.00000 ",  "{:<#8G}", 0.f),
            ROW("0.00000  ", "{:<#9G}", 0.f),
            ROW(" 0.00000 ", "{:^#9G}", 0.f),

            // general
            ROW("1.234e-37",   "{:g}",    1.234e-37f),
            ROW("1.234e-37",   "{:9g}",   1.234e-37f),
            ROW(" 1.234e-37",  "{:10g}",  1.234e-37f),
            ROW("1.234e-37 ",  "{:<10g}", 1.234e-37f),
            ROW("1.234e-37  ", "{:<11g}", 1.234e-37f),
            ROW(" 1.234e-37 ", "{:^11g}", 1.234e-37f),
            // uppercase general
            ROW("1.234E-37",   "{:G}",    1.234e-37f),
            ROW("1.234E-37",   "{:9G}",   1.234e-37f),
            ROW(" 1.234E-37",  "{:10G}",  1.234e-37f),
            ROW("1.234E-37 ",  "{:<10G}", 1.234e-37f),
            ROW("1.234E-37  ", "{:<11G}", 1.234e-37f),
            ROW(" 1.234E-37 ", "{:^11G}", 1.234e-37f),

            ROW("1.23457e-37", "{:g}", 1.2345678e-37f),

            // general ALTERNATE
            ROW("1.23400e-37",   "{:#g}",    1.234e-37f),
            ROW("1.23400e-37",   "{:#11g}",  1.234e-37f),
            ROW(" 1.23400e-37",  "{:#12g}",  1.234e-37f),
            ROW("1.23400e-37 ",  "{:<#12g}", 1.234e-37f),
            ROW("1.23400e-37  ", "{:<#13g}", 1.234e-37f),
            ROW(" 1.23400e-37 ", "{:^#13g}", 1.234e-37f),
            // uppercase general
            ROW("1.23400E-37",   "{:#G}",    1.234e-37f),
            ROW("1.23400E-37",   "{:#11G}",  1.234e-37f),
            ROW(" 1.23400E-37",  "{:#12G}",  1.234e-37f),
            ROW("1.23400E-37 ",  "{:<#12G}", 1.234e-37f),
            ROW("1.23400E-37  ", "{:<#13G}", 1.234e-37f),
            ROW(" 1.23400E-37 ", "{:^#13G}", 1.234e-37f),

            ROW("1.23457e-37", "{:#g}", 1.2345678e-37f),

            // WITH PRECISION
            // --------------

            // hexfloat .0
            ROW("0p+0",   "{:.0a}",   0.f),
            ROW("0p+0",   "{:4.0a}",  0.f),
            ROW(" 0p+0",  "{:5.0a}",  0.f),
            ROW("0p+0 ",  "{:<5.0a}", 0.f),
            ROW("0p+0  ", "{:<6.0a}", 0.f),
            ROW(" 0p+0 ", "{:^6.0a}", 0.f),
            // uppercase hexfloat .0
            ROW("0P+0",   "{:.0A}",   0.f),
            ROW("0P+0",   "{:4.0A}",  0.f),
            ROW(" 0P+0",  "{:5.0A}",  0.f),
            ROW("0P+0 ",  "{:<5.0A}", 0.f),
            ROW("0P+0  ", "{:<6.0A}", 0.f),
            ROW(" 0P+0 ", "{:^6.0A}", 0.f),

            // hexfloat .0 ALTERNATE
            ROW("0.p+0",   "{:#.0a}",   0.f),
            ROW("0.p+0",   "{:#5.0a}",  0.f),
            ROW(" 0.p+0",  "{:#6.0a}",  0.f),
            ROW("0.p+0 ",  "{:<#6.0a}", 0.f),
            ROW("0.p+0  ", "{:<#7.0a}", 0.f),
            ROW(" 0.p+0 ", "{:^#7.0a}", 0.f),
            // uppercase hexfloat .0 ALTERNATE
            ROW("0.P+0",   "{:#.0A}",   0.f),
            ROW("0.P+0",   "{:#5.0A}",  0.f),
            ROW(" 0.P+0",  "{:#6.0A}",  0.f),
            ROW("0.P+0 ",  "{:<#6.0A}", 0.f),
            ROW("0.P+0  ", "{:<#7.0A}", 0.f),
            ROW(" 0.P+0 ", "{:^#7.0A}", 0.f),

            // hexfloat .1
            ROW("0.0p+0",   "{:.1a}",   0.f),
            ROW("0.0p+0",   "{:6.1a}",  0.f),
            ROW(" 0.0p+0",  "{:7.1a}",  0.f),
            ROW("0.0p+0 ",  "{:<7.1a}", 0.f),
            ROW("0.0p+0  ", "{:<8.1a}", 0.f),
            ROW(" 0.0p+0 ", "{:^8.1a}", 0.f),
            // uppercase hexfloat .1
            ROW("0.0P+0",   "{:.1A}",   0.f),
            ROW("0.0P+0",   "{:6.1A}",  0.f),
            ROW(" 0.0P+0",  "{:7.1A}",  0.f),
            ROW("0.0P+0 ",  "{:<7.1A}", 0.f),
            ROW("0.0P+0  ", "{:<8.1A}", 0.f),
            ROW(" 0.0P+0 ", "{:^8.1A}", 0.f),

            // hexfloat .3
            ROW("0.000p+0",   "{:.3a}",    0.f),
            ROW("0.000p+0",   "{:8.3a}",   0.f),
            ROW(" 0.000p+0",  "{:9.3a}",   0.f),
            ROW("0.000p+0 ",  "{:<9.3a}",  0.f),
            ROW("0.000p+0  ", "{:<10.3a}", 0.f),
            ROW(" 0.000p+0 ", "{:^10.3a}", 0.f),
            // uppercase hexfloat .3
            ROW("0.000P+0",   "{:.3A}",    0.f),
            ROW("0.000P+0",   "{:8.3A}",   0.f),
            ROW(" 0.000P+0",  "{:9.3A}",   0.f),
            ROW("0.000P+0 ",  "{:<9.3A}",  0.f),
            ROW("0.000P+0  ", "{:<10.3A}", 0.f),
            ROW(" 0.000P+0 ", "{:^10.3A}", 0.f),

            // hexfloat .51
            ROW("0.000000000000000000000000000000000000000000000000000p+0",
                                                            "{:.51a}",    0.f),
            ROW("0.000000000000000000000000000000000000000000000000000p+0",
                                                            "{:56.51a}",  0.f),
            ROW(" 0.000000000000000000000000000000000000000000000000000p+0",
                                                            "{:57.51a}",  0.f),
            ROW("0.000000000000000000000000000000000000000000000000000p+0 ",
                                                            "{:<57.51a}", 0.f),
            ROW("0.000000000000000000000000000000000000000000000000000p+0  ",
                                                            "{:<58.51a}", 0.f),
            ROW(" 0.000000000000000000000000000000000000000000000000000p+0 ",
                                                            "{:^58.51a}", 0.f),
            // uppercase hexfloat .51
            ROW("0.000000000000000000000000000000000000000000000000000P+0",
                                                            "{:.51A}",    0.f),
            ROW("0.000000000000000000000000000000000000000000000000000P+0",
                                                            "{:56.51A}",  0.f),
            ROW(" 0.000000000000000000000000000000000000000000000000000P+0",
                                                            "{:57.51A}",  0.f),
            ROW("0.000000000000000000000000000000000000000000000000000P+0 ",
                                                            "{:<57.51A}", 0.f),
            ROW("0.000000000000000000000000000000000000000000000000000P+0  ",
                                                            "{:<58.51A}", 0.f),
            ROW(" 0.000000000000000000000000000000000000000000000000000P+0 ",
                                                            "{:^58.51A}", 0.f),

            // scientific .0
            ROW("0e+00",   "{:.0e}",   0.f),
            ROW("0e+00",   "{:5.0e}",  0.f),
            ROW(" 0e+00",  "{:6.0e}",  0.f),
            ROW("0e+00 ",  "{:<6.0e}", 0.f),
            ROW("0e+00  ", "{:<7.0e}", 0.f),
            ROW(" 0e+00 ", "{:^7.0e}", 0.f),
            // uppercase scientific .0
            ROW("0E+00",   "{:.0E}",   0.f),
            ROW("0E+00",   "{:5.0E}",  0.f),
            ROW(" 0E+00",  "{:6.0E}",  0.f),
            ROW("0E+00 ",  "{:<6.0E}", 0.f),
            ROW("0E+00  ", "{:<7.0E}", 0.f),
            ROW(" 0E+00 ", "{:^7.0E}", 0.f),

            // scientific .0 ALTERNATE
            ROW("0.e+00",   "{:#.0e}",   0.f),
            ROW("0.e+00",   "{:#6.0e}",  0.f),
            ROW(" 0.e+00",  "{:#7.0e}",  0.f),
            ROW("0.e+00 ",  "{:<#7.0e}", 0.f),
            ROW("0.e+00  ", "{:<#8.0e}", 0.f),
            ROW(" 0.e+00 ", "{:^#8.0e}", 0.f),
            // uppercase scientific .0
            ROW("0.E+00",   "{:#.0E}",   0.f),
            ROW("0.E+00",   "{:#6.0E}",  0.f),
            ROW(" 0.E+00",  "{:#7.0E}",  0.f),
            ROW("0.E+00 ",  "{:<#7.0E}", 0.f),
            ROW("0.E+00  ", "{:<#8.0E}", 0.f),
            ROW(" 0.E+00 ", "{:^#8.0E}", 0.f),

            // scientific .1
            ROW("0.0e+00",   "{:.1e}",   0.f),
            ROW("0.0e+00",   "{:7.1e}",  0.f),
            ROW(" 0.0e+00",  "{:8.1e}",  0.f),
            ROW("0.0e+00 ",  "{:<8.1e}", 0.f),
            ROW("0.0e+00  ", "{:<9.1e}", 0.f),
            ROW(" 0.0e+00 ", "{:^9.1e}", 0.f),
            // uppercase scientific .1
            ROW("0.0E+00",   "{:.1E}",   0.f),
            ROW("0.0E+00",   "{:7.1E}",  0.f),
            ROW(" 0.0E+00",  "{:8.1E}",  0.f),
            ROW("0.0E+00 ",  "{:<8.1E}", 0.f),
            ROW("0.0E+00  ", "{:<9.1E}", 0.f),
            ROW(" 0.0E+00 ", "{:^9.1E}", 0.f),

            // scientific .1 ALTERNATE
            ROW("0.0e+00",   "{:#.1e}",   0.f),
            ROW("0.0e+00",   "{:#7.1e}",  0.f),
            ROW(" 0.0e+00",  "{:#8.1e}",  0.f),
            ROW("0.0e+00 ",  "{:<#8.1e}", 0.f),
            ROW("0.0e+00  ", "{:<#9.1e}", 0.f),
            ROW(" 0.0e+00 ", "{:^#9.1e}", 0.f),
            // uppercase scientific .1 ALTERNATE
            ROW("0.0E+00",   "{:#.1E}",   0.f),
            ROW("0.0E+00",   "{:#7.1E}",  0.f),
            ROW(" 0.0E+00",  "{:#8.1E}",  0.f),
            ROW("0.0E+00 ",  "{:<#8.1E}", 0.f),
            ROW("0.0E+00  ", "{:<#9.1E}", 0.f),
            ROW(" 0.0E+00 ", "{:^#9.1E}", 0.f),

            // scientific .17
            ROW("0.00000000000000000e+00",   "{:.17e}" ,   0.f),
            ROW("0.00000000000000000e+00",   "{:23.17e}",  0.f),
            ROW(" 0.00000000000000000e+00",  "{:24.17e}",  0.f),
            ROW("0.00000000000000000e+00 ",  "{:<24.17e}", 0.f),
            ROW("0.00000000000000000e+00  ", "{:<25.17e}", 0.f),
            ROW(" 0.00000000000000000e+00 ", "{:^25.17e}", 0.f),
            // uppercase scientific .17
            ROW("0.00000000000000000E+00",   "{:.17E}" ,   0.f),
            ROW("0.00000000000000000E+00",   "{:23.17E}",  0.f),
            ROW(" 0.00000000000000000E+00",  "{:24.17E}",  0.f),
            ROW("0.00000000000000000E+00 ",  "{:<24.17E}", 0.f),
            ROW("0.00000000000000000E+00  ", "{:<25.17E}", 0.f),
            ROW(" 0.00000000000000000E+00 ", "{:^25.17E}", 0.f),

            // scientific .50
            ROW("0.00000000000000000000000000000000000000000000000000e+00",
                                                            "{:.50e}" ,   0.f),
            ROW("0.00000000000000000000000000000000000000000000000000e+00",
                                                            "{:56.50e}",  0.f),
            ROW(" 0.00000000000000000000000000000000000000000000000000e+00",
                                                            "{:57.50e}",  0.f),
            ROW("0.00000000000000000000000000000000000000000000000000e+00 ",
                                                            "{:<57.50e}", 0.f),
            ROW("0.00000000000000000000000000000000000000000000000000e+00  ",
                                                            "{:<58.50e}", 0.f),
            ROW(" 0.00000000000000000000000000000000000000000000000000e+00 ",
                                                            "{:^58.50e}", 0.f),
            // uppercase scientific .50
            ROW("0.00000000000000000000000000000000000000000000000000E+00",
                                                            "{:.50E}" ,   0.f),
            ROW("0.00000000000000000000000000000000000000000000000000E+00",
                                                            "{:56.50E}",  0.f),
            ROW(" 0.00000000000000000000000000000000000000000000000000E+00",
                                                            "{:57.50E}",  0.f),
            ROW("0.00000000000000000000000000000000000000000000000000E+00 ",
                                                            "{:<57.50E}", 0.f),
            ROW("0.00000000000000000000000000000000000000000000000000E+00  ",
                                                            "{:<58.50E}", 0.f),
            ROW(" 0.00000000000000000000000000000000000000000000000000E+00 ",
                                                            "{:^58.50E}", 0.f),

            // general .6
            ROW("1.234e-37",   "{:.6g}",    1.234e-37f),
            ROW("1.234e-37",   "{:9.6g}",   1.234e-37f),
            ROW(" 1.234e-37",  "{:10.6g}",  1.234e-37f),
            ROW("1.234e-37 ",  "{:<10.6g}", 1.234e-37f),
            ROW("1.234e-37  ", "{:<11.6g}", 1.234e-37f),
            ROW(" 1.234e-37 ", "{:^11.6g}", 1.234e-37f),
            // uppercase general .6
            ROW("1.234E-37",   "{:.6G}",    1.234e-37f),
            ROW("1.234E-37",   "{:9.6G}",   1.234e-37f),
            ROW(" 1.234E-37",  "{:10.6G}",  1.234e-37f),
            ROW("1.234E-37 ",  "{:<10.6G}", 1.234e-37f),
            ROW("1.234E-37  ", "{:<11.6G}", 1.234e-37f),
            ROW(" 1.234E-37 ", "{:^11.6G}", 1.234e-37f),

            ROW("1.23457e-37", "{:.6g}", 1.2345678e-37f),

            // general .6 ALTERNATE
            ROW("1.23400e-37",   "{:#.6g}",    1.234e-37f),
            ROW("1.23400e-37",   "{:#11.6g}",  1.234e-37f),
            ROW(" 1.23400e-37",  "{:#12.6g}",  1.234e-37f),
            ROW("1.23400e-37 ",  "{:<#12.6g}", 1.234e-37f),
            ROW("1.23400e-37  ", "{:<#13.6g}", 1.234e-37f),
            ROW(" 1.23400e-37 ", "{:^#13.6g}", 1.234e-37f),
            // uppercase general .6 ALTERNATE
            ROW("1.23400E-37",   "{:#.6G}",    1.234e-37f),
            ROW("1.23400E-37",   "{:#11.6G}",  1.234e-37f),
            ROW(" 1.23400E-37",  "{:#12.6G}",  1.234e-37f),
            ROW("1.23400E-37 ",  "{:<#12.6G}", 1.234e-37f),
            ROW("1.23400E-37  ", "{:<#13.6G}", 1.234e-37f),
            ROW(" 1.23400E-37 ", "{:^#13.6G}", 1.234e-37f),

            ROW("1.23457e-37", "{:#.6g}", 1.2345678e-37f),

            // general .0
            ROW("1e-37",   "{:.0g}",   1.234e-37f),
            ROW("1e-37",   "{:5.0g}",  1.234e-37f),
            ROW(" 1e-37",  "{:6.0g}",  1.234e-37f),
            ROW("1e-37 ",  "{:<6.0g}", 1.234e-37f),
            ROW("1e-37  ", "{:<7.0g}", 1.234e-37f),
            ROW(" 1e-37 ", "{:^7.0g}", 1.234e-37f),
            // uppercase general .0
            ROW("1E-37",   "{:.0G}",   1.234E-37f),
            ROW("1E-37",   "{:5.0G}",  1.234E-37f),
            ROW(" 1E-37",  "{:6.0G}",  1.234E-37f),
            ROW("1E-37 ",  "{:<6.0G}", 1.234E-37f),
            ROW("1E-37  ", "{:<7.0G}", 1.234E-37f),
            ROW(" 1E-37 ", "{:^7.0G}", 1.234E-37f),

            // general .0 ALTERNATE
            ROW("1.e-37",   "{:#.0g}",   1.234e-37f),
#if defined(_MSC_FULL_VER) && _MSC_FULL_VER > 194134123
    // MSVC as Oracle has a bug with width when precision is 0
            ROW("1.e-37",   "{:#6.0g}",  1.234e-37f),
            ROW(" 1.e-37",  "{:#7.0g}",  1.234e-37f),
            ROW("1.e-37 ",  "{:<#7.0g}", 1.234e-37f),
            ROW("1.e-37  ", "{:<#8.0g}", 1.234e-37f),
            ROW(" 1.e-37 ", "{:^#8.0g}", 1.234e-37f),
#endif
            // uppercase general .0 ALTERNATE
            ROW("1.E-37",   "{:#.0G}",   1.234e-37f),
#if defined(_MSC_FULL_VER) && _MSC_FULL_VER > 194134123
    // MSVC as Oracle has a bug with width when precision is 0
            ROW("1.E-37",   "{:#6.0G}",  1.234e-37f),
            ROW(" 1.E-37",  "{:#7.0G}",  1.234e-37f),
            ROW("1.E-37 ",  "{:<#7.0G}", 1.234e-37f),
            ROW("1.E-37  ", "{:<#8.0G}", 1.234e-37f),
            ROW(" 1.E-37 ", "{:^#8.0G}", 1.234e-37f),
#endif

            // general .1
            ROW("1e-37",   "{:.1g}",   1.234e-37f),
            ROW("1e-37",   "{:5.1g}",  1.234e-37f),
            ROW(" 1e-37",  "{:6.1g}",  1.234e-37f),
            ROW("1e-37 ",  "{:<6.1g}", 1.234e-37f),
            ROW("1e-37  ", "{:<7.1g}", 1.234e-37f),
            ROW(" 1e-37 ", "{:^7.1g}", 1.234e-37f),
            // uppercase general .1
            ROW("1E-37",   "{:.1G}",   1.234E-37f),
            ROW("1E-37",   "{:5.1G}",  1.234E-37f),
            ROW(" 1E-37",  "{:6.1G}",  1.234E-37f),
            ROW("1E-37 ",  "{:<6.1G}", 1.234E-37f),
            ROW("1E-37  ", "{:<7.1G}", 1.234E-37f),
            ROW(" 1E-37 ", "{:^7.1G}", 1.234E-37f),

            // general .1 ALTERNATE
            ROW("1.e-37",   "{:#.1g}",   1.234e-37f),
            ROW("1.e-37",   "{:#6.1g}",  1.234e-37f),
            ROW(" 1.e-37",  "{:#7.1g}",  1.234e-37f),
            ROW("1.e-37 ",  "{:<#7.1g}", 1.234e-37f),
            ROW("1.e-37  ", "{:<#8.1g}", 1.234e-37f),
            ROW(" 1.e-37 ", "{:^#8.1g}", 1.234e-37f),
            // uppercase general .1
            ROW("1.E-37",   "{:#.1G}",   1.234E-37f),
            ROW("1.E-37",   "{:#6.1G}",  1.234E-37f),
            ROW(" 1.E-37",  "{:#7.1G}",  1.234E-37f),
            ROW("1.E-37 ",  "{:<#7.1G}", 1.234E-37f),
            ROW("1.E-37  ", "{:<#8.1G}", 1.234E-37f),
            ROW(" 1.E-37 ", "{:^#8.1G}", 1.234E-37f),

            // general .2
            ROW("1.2e-37",   "{:.2g}",   1.234e-37f),
            ROW("1.2e-37",   "{:7.2g}",  1.234e-37f),
            ROW(" 1.2e-37",  "{:8.2g}",  1.234e-37f),
            ROW("1.2e-37 ",  "{:<8.2g}", 1.234e-37f),
            ROW("1.2e-37  ", "{:<9.2g}", 1.234e-37f),
            ROW(" 1.2e-37 ", "{:^9.2g}", 1.234e-37f),
            // uppercase general .2
            ROW("1.2E-37",   "{:.2G}",   1.234E-37f),
            ROW("1.2E-37",   "{:7.2G}",  1.234E-37f),
            ROW(" 1.2E-37",  "{:8.2G}",  1.234E-37f),
            ROW("1.2E-37 ",  "{:<8.2G}", 1.234E-37f),
            ROW("1.2E-37  ", "{:<9.2G}", 1.234E-37f),
            ROW(" 1.2E-37 ", "{:^9.2G}", 1.234E-37f),

            // general .2 ALTERNATE
            ROW("1.2e-37",   "{:#.2g}",   1.234e-37f),
            ROW("1.2e-37",   "{:#7.2g}",  1.234e-37f),
            ROW(" 1.2e-37",  "{:#8.2g}",  1.234e-37f),
            ROW("1.2e-37 ",  "{:<#8.2g}", 1.234e-37f),
            ROW("1.2e-37  ", "{:<#9.2g}", 1.234e-37f),
            ROW(" 1.2e-37 ", "{:^#9.2g}", 1.234e-37f),
            // uppercase general .2 ALTERNATE
            ROW("1.2E-37",   "{:#.2G}",   1.234E-37f),
            ROW("1.2E-37",   "{:#7.2G}",  1.234E-37f),
            ROW(" 1.2E-37",  "{:#8.2G}",  1.234E-37f),
            ROW("1.2E-37 ",  "{:<#8.2G}", 1.234E-37f),
            ROW("1.2E-37  ", "{:<#9.2G}", 1.234E-37f),
            ROW(" 1.2E-37 ", "{:^#9.2G}", 1.234E-37f),

            // general .50
            ROW("1.2340000190582515184895560080033662978123930243179e-37",
                                                     "{:.50g}",    1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179e-37",
                                                     "{:55.50g}",  1.234e-37f),
            ROW(" 1.2340000190582515184895560080033662978123930243179e-37",
                                                     "{:56.50g}",  1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179e-37 ",
                                                     "{:<56.50g}", 1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179e-37  ",
                                                     "{:<57.50g}", 1.234e-37f),
            ROW(" 1.2340000190582515184895560080033662978123930243179e-37 ",
                                                     "{:^57.50g}", 1.234e-37f),
            // uppercase general .50
            ROW("1.2340000190582515184895560080033662978123930243179E-37",
                                                     "{:.50G}",    1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179E-37",
                                                     "{:55.50G}",  1.234e-37f),
            ROW(" 1.2340000190582515184895560080033662978123930243179E-37",
                                                     "{:56.50G}",  1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179E-37 ",
                                                     "{:<56.50G}", 1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179E-37  ",
                                                     "{:<57.50G}", 1.234e-37f),
            ROW(" 1.2340000190582515184895560080033662978123930243179E-37 ",
                                                     "{:^57.50G}", 1.234e-37f),

            // general .50 ALTERNATE
            ROW("1.2340000190582515184895560080033662978123930243179e-37",
                                                    "{:#.50g}",    1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179e-37",
                                                    "{:#55.50g}",  1.234e-37f),
            ROW(" 1.2340000190582515184895560080033662978123930243179e-37",
                                                    "{:#56.50g}",  1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179e-37 ",
                                                    "{:<#56.50g}", 1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179e-37  ",
                                                    "{:<#57.50g}", 1.234e-37f),
            ROW(" 1.2340000190582515184895560080033662978123930243179e-37 ",
                                                    "{:^#57.50g}", 1.234e-37f),
            // uppercase general .50 ALTERNATE
            ROW("1.2340000190582515184895560080033662978123930243179E-37",
                                                    "{:#.50G}",    1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179E-37",
                                                    "{:#55.50G}",  1.234e-37f),
            ROW(" 1.2340000190582515184895560080033662978123930243179E-37",
                                                    "{:#56.50G}",  1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179E-37 ",
                                                    "{:<#56.50G}", 1.234e-37f),
            ROW("1.2340000190582515184895560080033662978123930243179E-37  ",
                                                    "{:<#57.50G}", 1.234e-37f),
            ROW(" 1.2340000190582515184895560080033662978123930243179E-37 ",
                                                    "{:^#57.50G}", 1.234e-37f),

            // fixed .6
            ROW("0.000000",   "{:.6f}",    0.f),
            ROW("0.000000",   "{:8.6f}",   0.f),
            ROW(" 0.000000",  "{:9.6f}",   0.f),
            ROW("0.000000 ",  "{:<9.6f}",  0.f),
            ROW("0.000000  ", "{:<10.6f}", 0.f),
            ROW(" 0.000000 ", "{:^10.6f}", 0.f),
            // uppercase fixed .6 (same as fixed)
            ROW("0.000000",   "{:.6F}",    0.f),
            ROW("0.000000",   "{:8.6F}",   0.f),
            ROW(" 0.000000",  "{:9.6F}",   0.f),
            ROW("0.000000 ",  "{:<9.6F}",  0.f),
            ROW("0.000000  ", "{:<10.6F}", 0.f),
            ROW(" 0.000000 ", "{:^10.6F}", 0.f),

            // fixed .6 ALTERNATE
            ROW("0.000000",   "{:#.6f}",    0.f),
            ROW("0.000000",   "{:#8.6f}",   0.f),
            ROW(" 0.000000",  "{:#9.6f}",   0.f),
            ROW("0.000000 ",  "{:<#9.6f}",  0.f),
            ROW("0.000000  ", "{:<#10.6f}", 0.f),
            ROW(" 0.000000 ", "{:^#10.6f}", 0.f),
            // uppercase fixed .6 ALTERNATE (same as fixed)
            ROW("0.000000",   "{:#.6F}",    0.f),
            ROW("0.000000",   "{:#8.6F}",   0.f),
            ROW(" 0.000000",  "{:#9.6F}",   0.f),
            ROW("0.000000 ",  "{:<#9.6F}",  0.f),
            ROW("0.000000  ", "{:<#10.6F}", 0.f),
            ROW(" 0.000000 ", "{:^#10.6F}", 0.f),

            // fixed .0
            ROW("0",   "{:.0f}",   0.f),
            ROW("0",   "{:1.0f}",  0.f),
            ROW(" 0",  "{:2.0f}",  0.f),
            ROW("0 ",  "{:<2.0f}", 0.f),
            ROW("0  ", "{:<3.0f}", 0.f),
            ROW(" 0 ", "{:^3.0f}", 0.f),
            // uppercase fixed .6 (same as fixed)
            ROW("0",   "{:.0F}",   0.f),
            ROW("0",   "{:1.0F}",  0.f),
            ROW(" 0",  "{:2.0F}",  0.f),
            ROW("0 ",  "{:<2.0F}", 0.f),
            ROW("0  ", "{:<3.0F}", 0.f),
            ROW(" 0 ", "{:^3.0F}", 0.f),
            // fixed .0 ALTERNATE
            ROW("0.",   "{:#.0f}",   0.f),
            ROW("0.",   "{:#2.0f}",  0.f),
            ROW(" 0.",  "{:#3.0f}",  0.f),
            ROW("0. ",  "{:<#3.0f}", 0.f),
            ROW("0.  ", "{:<#4.0f}", 0.f),
            ROW(" 0. ", "{:^#4.0f}", 0.f),
            // uppercase fixed .6 (same as fixed)
            ROW("0.",   "{:#.0F}",   0.f),
            ROW("0.",   "{:#2.0F}",  0.f),
            ROW(" 0.",  "{:#3.0F}",  0.f),
            ROW("0. ",  "{:<#3.0F}", 0.f),
            ROW("0.  ", "{:<#4.0F}", 0.f),
            ROW(" 0. ", "{:^#4.0F}", 0.f),

            // fixed .54
            ROW("0.000000000000000000000000000000000000000000000000000000",
                                                            "{:.54f}",    0.f),
            ROW("0.000000000000000000000000000000000000000000000000000000",
                                                            "{:56.54f}",  0.f),
            ROW(" 0.000000000000000000000000000000000000000000000000000000",
                                                            "{:57.54f}",  0.f),
            ROW("0.000000000000000000000000000000000000000000000000000000 ",
                                                            "{:<57.54f}", 0.f),
            ROW("0.000000000000000000000000000000000000000000000000000000  ",
                                                            "{:<58.54f}", 0.f),
            ROW(" 0.000000000000000000000000000000000000000000000000000000 ",
                                                            "{:^58.54f}", 0.f),
            // uppercase fixed .6 (same as fixed)
            ROW("0.000000000000000000000000000000000000000000000000000000",
                                                            "{:.54F}",    0.f),
            ROW("0.000000000000000000000000000000000000000000000000000000",
                                                            "{:56.54F}",  0.f),
            ROW(" 0.000000000000000000000000000000000000000000000000000000",
                                                            "{:57.54F}",  0.f),
            ROW("0.000000000000000000000000000000000000000000000000000000 ",
                                                            "{:<57.54F}", 0.f),
            ROW("0.000000000000000000000000000000000000000000000000000000  ",
                                                            "{:<58.54F}", 0.f),
            ROW(" 0.000000000000000000000000000000000000000000000000000000 ",
                                                            "{:^58.54F}", 0.f),
        };
        const size_t NUM_FLOAT_TESTS = sizeof FLOAT_TEST / sizeof *FLOAT_TEST;

        if (veryVerbose) puts("\tTesting `float` to `char`");
        for (size_t ti = 0; ti < NUM_FLOAT_TESTS; ++ti) {
            const int          LINE     = FLOAT_TEST[ti].d_line;
            const char * const EXPECTED = FLOAT_TEST[ti].d_expected;
            const char * const FORMAT   = FLOAT_TEST[ti].d_format;
            const float        VALUE    = FLOAT_TEST[ti].d_value;

            if (veryVeryVerbose) {
                P_(LINE) P_(EXPECTED) P_(FORMAT) P(VALUE);
            }

            testRuntimeCharParse<float>(LINE, FORMAT);
            testRuntimeFormat(LINE, EXPECTED, FORMAT, VALUE);
        }

        if (veryVerbose) puts("\tTesting `float` to `wchar_t`");
        for (size_t ti = 0; ti < NUM_FLOAT_TESTS; ++ti) {
            const int             LINE      = FLOAT_TEST[ti].d_line;
            const wchar_t * const WEXPECTED = FLOAT_TEST[ti].d_wexpected;
            const wchar_t * const WFORMAT   = FLOAT_TEST[ti].d_wformat;
            const float           VALUE     = FLOAT_TEST[ti].d_value;

            if (veryVeryVerbose) {
                P_(LINE) P(VALUE);
            }

            testRuntimeWcharParse<float>(LINE, WFORMAT);
            testRuntimeFormat(LINE, WEXPECTED, WFORMAT, VALUE);
        }


        if (veryVerbose) puts("\tTesting compile-time processing.");
        {
            bsl::string message;

            // `parse`

            bool rv = bslfmt::Formatter_TestUtil<char>::testParseFormat<
                                               double>(&message, true, "{0:}");
            ASSERTV(message.c_str(), rv);

            rv = bslfmt::Formatter_TestUtil<char>::testParseFormat<
                                              double>(&message, true, "{0:g}");
            ASSERTV(message.c_str(), rv);

            rv = bslfmt::Formatter_TestUtil<char>::testParseFormat<
                                              double>(&message, true, "{0:E}");
            ASSERTV(message.c_str(), rv);

            rv = bslfmt::Formatter_TestUtil<char>::testParseFormat<
                                              double>(&message, true, "{0:A}");
            ASSERTV(message.c_str(), rv);

            rv = bslfmt::Formatter_TestUtil<wchar_t>::testParseFormat<
                                               float>(&message, true, L"{0:}");
            ASSERTV(message.c_str(), rv);

            rv = bslfmt::Formatter_TestUtil<char>::testParseFormat<
                                          double>(&message, true, "{:*<6.2a}");
            ASSERTV(message.c_str(), rv);

            rv = bslfmt::Formatter_TestUtil<wchar_t>::testParseFormat<
                                          float>(&message, true, L"{:*<6.2g}");
            ASSERTV(message.c_str(), rv);

            // `format`

            const double VALUE     = 5.;
            const int    DUMMY_ARG = 0;
            rv = bslfmt::Formatter_TestUtil<char>::testEvaluateFormat(
                                                                    &message,
                                                                    "5",
                                                                    true,
                                                                    "{:}",
                                                                    VALUE,
                                                                    DUMMY_ARG,
                                                                    DUMMY_ARG);
            ASSERTV(message.c_str(), rv);

            rv = bslfmt::Formatter_TestUtil<wchar_t>::testEvaluateFormat(
                                                                    &message,
                                                                    L"5",
                                                                    true,
                                                                    L"{:}",
                                                                    VALUE,
                                                                    DUMMY_ARG,
                                                                    DUMMY_ARG);
            ASSERTV(message.c_str(), rv);

            const float FLOAT_VALUE = 12.34f;
            rv = bslfmt::Formatter_TestUtil<char>::testEvaluateFormat(
                                                                   &message,
                                                                   "12.34",
                                                                   true,
                                                                   "{:}",
                                                                   FLOAT_VALUE,
                                                                   DUMMY_ARG,
                                                                   DUMMY_ARG);
            ASSERTV(message.c_str(), rv);

            rv = bslfmt::Formatter_TestUtil<wchar_t>::testEvaluateFormat(
                                                                   &message,
                                                                   L"12.34",
                                                                   true,
                                                                   L"{:}",
                                                                   FLOAT_VALUE,
                                                                   DUMMY_ARG,
                                                                   DUMMY_ARG);
            ASSERTV(message.c_str(), rv);
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (veryVerbose) puts("\tTesting locale prohibition.");
        {
            try {
                bsl::string message;
                bool        rv = bslfmt::Formatter_TestUtil<char>::
                                testParseVFormat<double>(&message,false,"{:}");
                ASSERTV(message.c_str(), rv);

                rv = bslfmt::Formatter_TestUtil<char>::
                               testParseVFormat<double>(&message,false, "{:L}");
                ASSERTV(message.c_str(), !rv);
            }
            catch(const bsl::format_error& err) {
                ASSERTV(err.what(),
                        "Exception should have been caught by the "
                        "`Formatter_TestUtil`",
                        false);
            }
            try {
                bsl::string message;
                bool        rv = bslfmt::Formatter_TestUtil<char>::
                                testParseVFormat<float>(&message,false,"{:}");
                ASSERTV(message.c_str(), rv);

                rv = bslfmt::Formatter_TestUtil<char>::
                               testParseVFormat<float>(&message,false, "{:L}");
                ASSERTV(message.c_str(), !rv);
            }
            catch(const bsl::format_error& err) {
                ASSERTV(err.what(),
                        "Exception should have been caught by the "
                        "`Formatter_TestUtil`",
                        false);
            }
        }
#endif  // BDE_BUILD_TARGET_EXC

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
