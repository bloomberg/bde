// bdlb_doublecompareutil.t.cpp                                       -*-C++-*-
#include <bdlb_doublecompareutil.h>

#include <bslim_testutil.h>

#include <bsls_asserttest.h>
#include <bsls_buildtarget.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
// [ 2] static int compare(double, double)
// [ 3] static int compare(double, double, double)
// [ 4] static int compare(double, double, double, double)
// [ 2] static bool eq(double a, double b);
// [ 3] static bool eq(double a, double b, double relTol);
// [ 4] static bool eq(double a, double b, double relTol, double absTol);
// [ 2] static bool ne(double a, double b);
// [ 3] static bool ne(double a, double b, double relTol);
// [ 4] static bool ne(double a, double b, double relTol, double absTol);
// [ 2] static bool lt(double a, double b);
// [ 3] static bool lt(double a, double b, double relTol);
// [ 4] static bool lt(double a, double b, double relTol, double absTol);
// [ 2] static bool le(double a, double b);
// [ 3] static bool le(double a, double b, double relTol);
// [ 4] static bool le(double a, double b, double relTol, double absTol);
// [ 2] static bool ge(double a, double b);
// [ 3] static bool ge(double a, double b, double relTol);
// [ 4] static bool ge(double a, double b, double relTol, double absTol);
// [ 2] static bool gt(double a, double b);
// [ 3] static bool gt(double a, double b, double relTol);
// [ 4] static bool gt(double a, double b, double relTol, double absTol);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] TABULAR EXAMPLE FROM COMPONENT-LEVEL DOCUMENTATION
// [ 6] NAN TESTING
// [ 7] NEGATIVE TESTING
// [ 8] USAGE EXAMPLE

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
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

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test        = argc > 1 ? atoi(argv[1]) : 0;
    int verbose     = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace `assert` with `ASSERT`.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============\n";

        {
            double a = 1.0;
            double b = 1.0 + 1.0e-25;

// The `bdlb::DoubleCompareUtil` utility functions are well suited for
// comparing both final or intermediate values such as prices, volumes,
// interest rates, and the products and quotients thereof.   We'll now
// illustrate the use of some of the various `bdlb::DoubleCompareUtil`
// comparison methods on two `double` values `a` and `b`.  First we'll
// determine whether the two values are "CLOSE ENOUGH" using the
// implementation-defined (default) tolerances:
//```
            if (bdlb::DoubleCompareUtil::fuzzyEq(a, b)) {
                if (verbose) {
                    bsl::cout << "Values 'a' and 'b' are CLOSE ENOUGH.\n";
                }
            }
//```
// Next, we'll determine whether the same two values are "NOT RELATIVELY
// CLOSE" using our own (unusually large) criteria of 1.0 for relative
// tolerance, but continuing to rely on the default value for absolute
// tolerance:
//```
            if (bdlb::DoubleCompareUtil::fuzzyNe(a, b, 1.0)) {
                if (verbose) {
                    bsl::cout << "Values 'a' and 'b' are "
                              << "NOT RELATIVELY CLOSE.\n";
                }
            }
//```
// Finally, we'll determine if the value `a` is "SIGNIFICANTLY GREATER THAN"
// `b` by supplying our own rather larger values of 1e-1 and 1e-3 for the
// relative and absolute tolerances, respectively:
//```
            if (bdlb::DoubleCompareUtil::fuzzyGt(a, b, 1e-1, 1e-3)) {
                if (verbose) {
                    bsl::cout << "Value 'a' is SIGNIFICANTLY "
                              << "GREATER THAN 'b'.\n";
                }
            }
//```
// Which will print to bsl::cout if and only if
//```
//  bdlb::DoubleCompareUtil::fuzzyNe(a, b, 1e-1, 1e-3) && a > b
//```
// is `true`.
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // NEGATIVE TESTING
        //
        // Concerns:
        //   Unsupported relative and absolute tolerance arguments trigger
        //   `BSLS_ASSERT`s, while supported arguments do not.
        //
        // Plan:
        //   In build modes that support exceptions use `bsls_asserttest` to
        //   verify that unsupported arguments trigger asserts, while supported
        //   arguments do not.
        //
        // Testing:
        //   NEGATIVE TESTING
        // --------------------------------------------------------------------

        if (verbose) cout << "\nNEGATIVE TESTING"
                          << "\n================\n";

#ifdef BDE_BUILD_TARGET_EXC
        typedef bsl::numeric_limits<double> Limits;
        const double k_QNAN = Limits::quiet_NaN();
        const double k_SNAN = Limits::signaling_NaN();
        const double k_INF  = Limits::infinity();

        struct {
            int    d_line;
            double d_relTol;
            double d_absTol;
            bool   d_relAssert;
            bool   d_absAssert;
        } TEST_DATA[] = {
            { L_, 42., 42., false, false },

            { L_, -0., 42., true,  false },
            { L_, 42., -0., false, true  },
            { L_, -0., -0., true,  true  },

            { L_, -1., 42., true,  false },
            { L_, 42., -1., false, true  },
            { L_, -1., -1., true,  true  },

            { L_, k_SNAN, 42.,    true,  false },
            { L_, 42.,    k_SNAN, false, true  },
            { L_, k_SNAN, k_SNAN, true,  true  },

            { L_, k_QNAN, 42.,    true,  false },
            { L_, 42.,    k_QNAN, false, true  },
            { L_, k_QNAN, k_QNAN, true,  true  },

            { L_, k_INF, 42.,   true,  false },
            { L_, 42.,   k_INF, false, true  },
            { L_, k_INF, k_INF, true,  true  },

            { L_, -k_INF, 42. ,   true,  false },
            { L_, 42.,    -k_INF, false, true  },
            { L_, -k_INF, -k_INF, true,  true  },
        };
        const size_t COUNT_TEST_DATA = sizeof TEST_DATA / sizeof *TEST_DATA;

        for (size_t ti = 0; ti < COUNT_TEST_DATA; ++ti) {
            const int    LINE       = TEST_DATA[ti].d_line;
            const double REL_TOL    = TEST_DATA[ti].d_relTol;
            const double ABS_TOL    = TEST_DATA[ti].d_absTol;
            const bool   REL_ASSERT = TEST_DATA[ti].d_relAssert;
            const bool   ABS_ASSERT = TEST_DATA[ti].d_absAssert;

            if (veryVerbose) {
                T_
                P_(LINE) P_(REL_TOL) P_(ABS_TOL) P_(REL_ASSERT) P(ABS_ASSERT);
            }

            bsls::AssertTestHandlerGuard g; (void)g;

            typedef bdlb::DoubleCompareUtil DCU;

            if (REL_ASSERT && !ABS_ASSERT) {
                ASSERT_FAIL(DCU::fuzzyCompare(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_PASS(DCU::fuzzyCompare(42.,42.,42.,     ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyCompare(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyCompare(42.,42.,42.,     42.    ));
                ASSERT_FAIL(DCU::fuzzyCompare(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyCompare(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyEq(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_PASS(DCU::fuzzyEq(42.,42.,42.,     ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyEq(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyEq(42.,42.,42.,     42.    ));
                ASSERT_FAIL(DCU::fuzzyEq(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyEq(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyNe(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_PASS(DCU::fuzzyNe(42.,42.,42.,     ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyNe(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyNe(42.,42.,42.,     42.    ));
                ASSERT_FAIL(DCU::fuzzyNe(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyNe(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyLt(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_PASS(DCU::fuzzyLt(42.,42.,42.,     ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyLt(42.,42.,REL_TOL, 42.    ));
                ASSERT_FAIL(DCU::fuzzyLt(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyLt(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyGt(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_PASS(DCU::fuzzyGt(42.,42.,42.,     ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyGt(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyGt(42.,42.,42.,     42.    ));
                ASSERT_FAIL(DCU::fuzzyGt(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyGt(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyLe(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_PASS(DCU::fuzzyLe(42.,42.,42.,     ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyLe(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyLe(42.,42.,42.,     42.    ));
                ASSERT_FAIL(DCU::fuzzyLe(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyLe(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyGe(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_PASS(DCU::fuzzyGe(42.,42.,42.,     ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyGe(42.,42.,REL_TOL, 42.    ));
                ASSERT_FAIL(DCU::fuzzyGe(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyGe(42.,42.,42));
            }
            else if (!REL_ASSERT && ABS_ASSERT) {
                ASSERT_FAIL(DCU::fuzzyCompare(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyCompare(42.,42.,42.,     ABS_TOL));
                ASSERT_PASS(DCU::fuzzyCompare(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyCompare(42.,42.,42.,     42.    ));
                ASSERT_PASS(DCU::fuzzyCompare(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyCompare(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyEq(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyEq(42.,42.,42.,     ABS_TOL));
                ASSERT_PASS(DCU::fuzzyEq(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyEq(42.,42.,42.,     42.    ));
                ASSERT_PASS(DCU::fuzzyEq(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyEq(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyNe(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyNe(42.,42.,42.,     ABS_TOL));
                ASSERT_PASS(DCU::fuzzyNe(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyNe(42.,42.,42.,     42.    ));
                ASSERT_PASS(DCU::fuzzyNe(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyNe(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyLt(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyLt(42.,42.,42.,     ABS_TOL));
                ASSERT_PASS(DCU::fuzzyLt(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyLt(42.,42.,42.,     42.    ));
                ASSERT_PASS(DCU::fuzzyLt(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyLt(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyGt(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyGt(42.,42.,42.,     ABS_TOL));
                ASSERT_PASS(DCU::fuzzyGt(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyGt(42.,42.,42.,     42.    ));
                ASSERT_PASS(DCU::fuzzyGt(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyGt(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyLe(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyLe(42.,42.,42.,     ABS_TOL));
                ASSERT_PASS(DCU::fuzzyLe(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyLe(42.,42.,42.,     42.    ));
                ASSERT_PASS(DCU::fuzzyLe(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyLe(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyGe(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyGe(42.,42.,42.,     ABS_TOL));
                ASSERT_PASS(DCU::fuzzyGe(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyGe(42.,42.,42.,     42.    ));
                ASSERT_PASS(DCU::fuzzyGe(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyGe(42.,42.,42));
            }
            else if (REL_ASSERT && ABS_ASSERT) {
                ASSERT_FAIL(DCU::fuzzyCompare(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyCompare(42.,42.,42.,     ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyCompare(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyCompare(42.,42.,42.,     42.    ));
                ASSERT_FAIL(DCU::fuzzyCompare(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyCompare(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyEq(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyEq(42.,42.,42.,ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyEq(42.,42.,REL_TOL, 42));
                ASSERT_PASS(DCU::fuzzyEq(42.,42.,42.,42));
                ASSERT_FAIL(DCU::fuzzyEq(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyEq(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyNe(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyNe(42.,42.,42.,ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyNe(42.,42.,REL_TOL, 42));
                ASSERT_PASS(DCU::fuzzyNe(42.,42.,42.,42));
                ASSERT_FAIL(DCU::fuzzyNe(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyNe(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyLt(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyLt(42.,42.,42.,     ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyLt(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyLt(42.,42.,42.,     42.    ));
                ASSERT_FAIL(DCU::fuzzyLt(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyLt(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyGt(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyGt(42.,42.,42.,     ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyGt(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyGt(42.,42.,42.,     42.    ));
                ASSERT_FAIL(DCU::fuzzyGt(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyGt(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyLe(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyLe(42.,42.,42.,     ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyLe(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyLe(42.,42.,42.,     42.    ));
                ASSERT_FAIL(DCU::fuzzyLe(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyLe(42.,42.,42));

                ASSERT_FAIL(DCU::fuzzyGe(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyGe(42.,42.,42.,     ABS_TOL));
                ASSERT_FAIL(DCU::fuzzyGe(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyGe(42.,42.,42.,     42.    ));
                ASSERT_FAIL(DCU::fuzzyGe(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyGe(42.,42.,42));
            }
            else {  // All arguments are OK, everything must PASS
                ASSERT_PASS(DCU::fuzzyCompare(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_PASS(DCU::fuzzyCompare(42.,42.,42.,     ABS_TOL));
                ASSERT_PASS(DCU::fuzzyCompare(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyCompare(42.,42.,42.,     42.    ));
                ASSERT_PASS(DCU::fuzzyCompare(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyCompare(42.,42.,42));

                ASSERT_PASS(DCU::fuzzyEq(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_PASS(DCU::fuzzyEq(42.,42.,42.,ABS_TOL));
                ASSERT_PASS(DCU::fuzzyEq(42.,42.,REL_TOL, 42));
                ASSERT_PASS(DCU::fuzzyEq(42.,42.,42.,42));
                ASSERT_PASS(DCU::fuzzyEq(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyEq(42.,42.,42));

                ASSERT_PASS(DCU::fuzzyNe(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_PASS(DCU::fuzzyNe(42.,42.,42.,ABS_TOL));
                ASSERT_PASS(DCU::fuzzyNe(42.,42.,REL_TOL, 42));
                ASSERT_PASS(DCU::fuzzyNe(42.,42.,42.,42));
                ASSERT_PASS(DCU::fuzzyNe(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyNe(42.,42.,42));

                ASSERT_PASS(DCU::fuzzyLt(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_PASS(DCU::fuzzyLt(42.,42.,42.,     ABS_TOL));
                ASSERT_PASS(DCU::fuzzyLt(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyLt(42.,42.,42.,     42.    ));
                ASSERT_PASS(DCU::fuzzyLt(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyLt(42.,42.,42));

                ASSERT_PASS(DCU::fuzzyGt(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_PASS(DCU::fuzzyGt(42.,42.,42.,     ABS_TOL));
                ASSERT_PASS(DCU::fuzzyGt(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyGt(42.,42.,42.,     42.    ));
                ASSERT_PASS(DCU::fuzzyGt(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyGt(42.,42.,42));

                ASSERT_PASS(DCU::fuzzyLe(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_PASS(DCU::fuzzyLe(42.,42.,42.,     ABS_TOL));
                ASSERT_PASS(DCU::fuzzyLe(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyLe(42.,42.,42.,     42.    ));
                ASSERT_PASS(DCU::fuzzyLe(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyLe(42.,42.,42));

                ASSERT_PASS(DCU::fuzzyGe(42.,42.,REL_TOL, ABS_TOL));
                ASSERT_PASS(DCU::fuzzyGe(42.,42.,42.,     ABS_TOL));
                ASSERT_PASS(DCU::fuzzyGe(42.,42.,REL_TOL, 42.    ));
                ASSERT_PASS(DCU::fuzzyGe(42.,42.,42.,     42.    ));
                ASSERT_PASS(DCU::fuzzyGe(42.,42.,REL_TOL));
                ASSERT_PASS(DCU::fuzzyGe(42.,42.,42));
            }
        }
#else
        if (verbose) cout << "\tSkipped due to no exception support.\n";
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // NAN TESTING
        //
        // Concerns:
        //   When a NaN value of any kind is specified as either or both of
        //   the compared arguments `fuzzyCompare` shall return
        //   `e_NON_COMPARABLE` while the rest of the `bool` functions should
        //   all return `false` except `fuzzyNe` that returns `true`.
        //
        // Plan:
        //   In a table based test either and both arguments as either quiet or
        //   signaling NaN and verify that C-1 requirements are satisfied.
        //
        // Testing:
        //   NAN TESTING
        // --------------------------------------------------------------------

        if (verbose) cout << "\nNAN TESTING"
                          << "\n===========\n";

        typedef bsl::numeric_limits<double> Limits;
        const double k_QNAN = Limits::quiet_NaN();
        const double k_SNAN = Limits::signaling_NaN();

        struct {
            int    d_line;
            double d_aVal;
            double d_bVal;
        } TEST_DATA[] = {
            { L_, k_SNAN, 42.    },
            { L_, 42.,    k_SNAN },
            { L_, k_SNAN, k_SNAN },

            { L_, k_QNAN, 42.    },
            { L_, 42.,    k_QNAN },
            { L_, k_QNAN, k_QNAN },
        };
        const size_t COUNT_TEST_DATA = sizeof TEST_DATA / sizeof *TEST_DATA;

        for (size_t ti = 0; ti < COUNT_TEST_DATA; ++ti) {
            const int    LINE  = TEST_DATA[ti].d_line;
            const double A_VAL = TEST_DATA[ti].d_aVal;
            const double B_VAL = TEST_DATA[ti].d_bVal;

            if (veryVerbose) { T_ P_(LINE) P_(A_VAL) P(B_VAL); }

            typedef bdlb::DoubleCompareUtil DCU;

            ASSERT(DCU::fuzzyCompare(A_VAL, B_VAL) == DCU::e_NON_COMPARABLE);
            ASSERT(DCU::fuzzyCompare(A_VAL, B_VAL, 42)
                                                     == DCU::e_NON_COMPARABLE);
            ASSERT(DCU::fuzzyCompare(A_VAL, B_VAL, 42, 42)
                                                     == DCU::e_NON_COMPARABLE);

            ASSERT(false == DCU::fuzzyEq(A_VAL, B_VAL));
            ASSERT(false == DCU::fuzzyEq(A_VAL, B_VAL, 42));
            ASSERT(false == DCU::fuzzyEq(A_VAL, B_VAL, 42, 42));

            ASSERT(true == DCU::fuzzyNe(A_VAL, B_VAL));
            ASSERT(true == DCU::fuzzyNe(A_VAL, B_VAL, 42));
            ASSERT(true == DCU::fuzzyNe(A_VAL, B_VAL, 42, 42));

            ASSERT(false == DCU::fuzzyLt(A_VAL, B_VAL));
            ASSERT(false == DCU::fuzzyLt(A_VAL, B_VAL, 42));
            ASSERT(false == DCU::fuzzyLt(A_VAL, B_VAL, 42, 42));

            ASSERT(false == DCU::fuzzyGt(A_VAL, B_VAL));
            ASSERT(false == DCU::fuzzyGt(A_VAL, B_VAL, 42));
            ASSERT(false == DCU::fuzzyGt(A_VAL, B_VAL, 42, 42));

            ASSERT(false == DCU::fuzzyLe(A_VAL, B_VAL));
            ASSERT(false == DCU::fuzzyLe(A_VAL, B_VAL, 42));
            ASSERT(false == DCU::fuzzyLe(A_VAL, B_VAL, 42, 42));

            ASSERT(false == DCU::fuzzyGe(A_VAL, B_VAL));
            ASSERT(false == DCU::fuzzyGe(A_VAL, B_VAL, 42));
            ASSERT(false == DCU::fuzzyGe(A_VAL, B_VAL, 42, 42));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING TABULAR EXAMPLE
        //
        // Concerns:
        //   The tabular example provided in the component header correctly
        //   represents the behavior of the utility methods.
        //
        // Plan:
        //   Incorporate the tabular example from header as a conventional set
        //   of table-generated test vectors and `ASSERT` the expected results.
        //
        // Testing:
        //   The numerical example from `bdlb_DoubleCompareUtil.h`.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING TABULAR EXAMPLE"
                             "\n=======================\n";

        if (verbose) cout <<
             "\tTesting numerical EXAMPLE from the component documentation:\n";

        static const struct {        // Test input/expected output struct:
            int    d_lineNum;        //   source line number
            double d_a;              //   rhs operand to test
            double d_b;              //   lhs operand to test
            double d_relTol;         //   relative tolerance
            double d_absTol;         //   absolute tolerance
            int    d_eq;             //   return for eq(d_a, d_b)
            int    d_ne;             //   return for ne(d_a, d_b)
            int    d_lt;             //   return for lt(d_a, d_b)
            int    d_le;             //   return for le(d_a, d_b)
            int    d_ge;             //   return for ge(d_a, d_b)
            int    d_gt;             //   return for gt(d_a, d_b)
        } DATA[] = {
            //                                                  Expected
            //                                              -----------------
            // line    d_a       d_b      relTol   absTol   EQ NE LT LE GE GT
            // ----  --------  --------   ------   ------   -- -- -- -- -- --
            {  L_,    99.0 ,    100.0 ,   0.010 ,  0.001 ,  0, 1, 1, 1, 0, 0 },
            {  L_,   100.0 ,     99.0 ,   0.010 ,  0.001 ,  0, 1, 0, 0, 1, 1 },
            {  L_,    99.0 ,    100.0 ,   0.011 ,  0.001 ,  1, 0, 0, 1, 1, 0 },
            {  L_,    99.0 ,    100.0 ,   0.010 ,  0.990 ,  0, 1, 1, 1, 0, 0 },
            {  L_,    99.0 ,    100.0 ,   0.010 ,  1.000 ,  1, 0, 0, 1, 1, 0 },
            {  L_,   100.0 ,    101.0 ,   0.009 ,  0.001 ,  0, 1, 1, 1, 0, 0 },
            {  L_,   101.0 ,    100.0 ,   0.009 ,  0.001 ,  0, 1, 0, 0, 1, 1 },
            {  L_,   100.0 ,    101.0 ,   0.010 ,  0.001 ,  1, 0, 0, 1, 1, 0 },
            {  L_,   100.0 ,    101.0 ,   0.009 ,  0.990 ,  0, 1, 1, 1, 0, 0 },
            {  L_,   100.0 ,    101.0 ,   0.009 ,  1.000 ,  1, 0, 0, 1, 1, 0 }
        };

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    LINE   = DATA[i].d_lineNum;
            const double a      = DATA[i].d_a;
            const double b      = DATA[i].d_b;
            const double relTol = DATA[i].d_relTol;
            const double absTol = DATA[i].d_absTol;

            if (veryVerbose) { T_; T_; P_(a); P_(b); P_(relTol); P(absTol); }

            typedef bdlb::DoubleCompareUtil DCU;
#define LASSERT(...) LOOP_ASSERT(LINE, __VA_ARGS__)
            LASSERT(DCU::fuzzyEq(a, b, relTol, absTol) == !!DATA[i].d_eq);
            LASSERT(DCU::fuzzyNe(a, b, relTol, absTol) == !!DATA[i].d_ne);
            LASSERT(DCU::fuzzyLt(a, b, relTol, absTol) == !!DATA[i].d_lt);
            LASSERT(DCU::fuzzyLe(a, b, relTol, absTol) == !!DATA[i].d_le);
            LASSERT(DCU::fuzzyGe(a, b, relTol, absTol) == !!DATA[i].d_ge);
            LASSERT(DCU::fuzzyGt(a, b, relTol, absTol) == !!DATA[i].d_gt);
#undef LASSERT
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // EQUALITY AND RELATIONAL FUNCTIONS, BOTH TOLERANCES
        //
        // Concerns:
        //   Verification with user-specified absolute and relative tolerances
        //   where `eq` and `ne` should be symmetric in the first two
        //   arguments.
        //
        // Plan:
        //   Specify a set of pairs of values `(a, b)` along with conveniently
        //   chosen absolute and relative tolerances, and verify that each of
        //   the six relational functions returns the expected value for the
        //   tabulated input.  Test the symmetry of `eq` and `ne` in the first
        //   two arguments explicitly.
        //
        // Testing:
        //   static int compare(double, double, double, double);
        //   static bool eq(double a, double b, double relTol, double absTol);
        //   static bool ne(double a, double b, double relTol, double absTol);
        //   static bool lt(double a, double b, double relTol, double absTol);
        //   static bool le(double a, double b, double relTol, double absTol);
        //   static bool gt(double a, double b, double relTol, double absTol);
        //   static bool ge(double a, double b, double relTol, double absTol);
        // --------------------------------------------------------------------
        if (verbose)
            cout << "\nEQUALITY AND RELATIONAL FUNCTIONS, BOTH TOLERANCES"
                    "\n==================================================\n";

        if (verbose) cout << "\tTesting using user-supplied tolerances:\n";

        static const struct {        // Test input/expected output struct:
            int    d_lineNum;        //   source line number
            double d_a;              //   rhs operand to test
            double d_b;              //   lhs operand to test
            double d_relTol;         //   relative tolerance
            double d_absTol;         //   absolute tolerance
            int    d_eq;             //   return for eq(d_a, d_bMinusA + d_a)
            int    d_ne;             //   return for ne(d_a, d_bMinusA + d_a)
            int    d_lt;             //   return for lt(d_a, d_bMinusA + d_a)
            int    d_le;             //   return for le(d_a, d_bMinusA + d_a)
            int    d_ge;             //   return for ge(d_a, d_bMinusA + d_a)
            int    d_gt;             //   return for gt(d_a, d_bMinusA + d_a)
        } DATA[] = {
            //                                                Expected
            //                                            -----------------
            //line    d_a       d_b      relTol  absTol   EQ NE LT LE GE GT
            //----  --------  --------   ------  ------   -- -- -- -- -- --
            { L_,   1.0    ,  1.0     ,   1e-6,   1e-3,    1, 0, 0, 1, 1, 0 },
            { L_,   1.0    ,  0.9999  ,   1e-6,   1e-3,    1, 0, 0, 1, 1, 0 },
            { L_,   1.0    ,  0.9995  ,   1e-6,   1e-3,    1, 0, 0, 1, 1, 0 },
            { L_,   1.0    ,  0.999   ,   1e-6,   1e-3,    0, 1, 0, 0, 1, 1 },
            { L_,   1.0    ,  0.99    ,   1e-6,   1e-3,    0, 1, 0, 0, 1, 1 },
            { L_,   1.0    ,  0.9     ,   1e-6,   1e-3,    0, 1, 0, 0, 1, 1 },
            { L_,   1.0    ,  1.1     ,   1e-6,   1e-3,    0, 1, 1, 1, 0, 0 },
            { L_,   1.0    ,  1.01    ,   1e-6,   1e-3,    0, 1, 1, 1, 0, 0 },
            { L_,   1.0    ,  1.00099 ,   1e-6,   1e-3,    1, 0, 0, 1, 1, 0 },
            { L_,   1.0    ,  1.001   ,   1e-6,   1e-3,    1, 0, 0, 1, 1, 0 },
            { L_,   1.0    ,  1.0001  ,   1e-6,   1e-3,    1, 0, 0, 1, 1, 0 },
            { L_,   0.9999 ,  1.0     ,   1e-6,   1e-3,    1, 0, 0, 1, 1, 0 },
            { L_,   0.9995 ,  1.0     ,   1e-6,   1e-3,    1, 0, 0, 1, 1, 0 },
            { L_,   0.999  ,  1.0     ,   1e-6,   1e-3,    0, 1, 1, 1, 0, 0 },
            { L_,   0.99   ,  1.0     ,   1e-6,   1e-3,    0, 1, 1, 1, 0, 0 },
            { L_,   0.9    ,  1.0     ,   1e-6,   1e-3,    0, 1, 1, 1, 0, 0 }
        };

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    LINE   = DATA[i].d_lineNum;
            const double a      = DATA[i].d_a;
            const double b      = DATA[i].d_b;
            const double relTol = DATA[i].d_relTol;
            const double absTol = DATA[i].d_absTol;

            if (veryVerbose) { T_; T_; P_(a); P_(b); P_(relTol); P(absTol); }

            typedef bdlb::DoubleCompareUtil DCU;
#define LASSERT(...) LOOP_ASSERT(LINE, __VA_ARGS__)
            LASSERT(DCU::fuzzyEq(a, b, relTol, absTol) == !!DATA[i].d_eq);
            LASSERT(DCU::fuzzyEq(b, a, relTol, absTol) == !!DATA[i].d_eq);
            LASSERT(DCU::fuzzyNe(a, b, relTol, absTol) == !!DATA[i].d_ne);
            LASSERT(DCU::fuzzyNe(b, a, relTol, absTol) == !!DATA[i].d_ne);
            LASSERT(DCU::fuzzyLt(a, b, relTol, absTol) == !!DATA[i].d_lt);
            LASSERT(DCU::fuzzyLe(a, b, relTol, absTol) == !!DATA[i].d_le);
            LASSERT(DCU::fuzzyGe(a, b, relTol, absTol) == !!DATA[i].d_ge);
            LASSERT(DCU::fuzzyGt(a, b, relTol, absTol) == !!DATA[i].d_gt);
#undef LASSERT
            const DCU::CompareResult RESULT = DCU::fuzzyCompare(a,
                                                                b,
                                                                relTol,
                                                                absTol);
            if (DATA[i].d_eq) {
                LOOP_ASSERT(LINE, DCU::e_EQUAL == RESULT);
            }
            else if (DATA[i].d_lt) {
                LOOP_ASSERT(LINE, DCU::e_LESS_THAN == RESULT);
            }
            else {
                LOOP_ASSERT(LINE, DATA[i].d_gt);
                LOOP_ASSERT(LINE, DCU::e_GREATER_THAN == RESULT);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // EQUALITY AND RELATIONAL METHODS, RELATIVE TOLERANCE
        //
        // Concerns:
        //   Verification with default absolute tolerance and specified
        //   relative tolerance where `eq` and `ne` should be symmetric in the
        //   first two arguments.
        //
        // Plan:
        //   Specify a set of values `(a, b)`.  For convenience of notation,
        //   tabulate `a` and `b - a` along with a conveniently chosen relative
        //   tolerance, and verify that each of the six relational functions
        //   returns the expected value for the tabulated input.  Test the
        //   symmetry of `eq` and `ne` in the first two arguments explicitly.
        //
        // Testing:
        //   static int compare(double a, double b, double relTol);
        //   static bool eq(double a, double b, double relTol);
        //   static bool ne(double a, double b, double relTol);
        //   static bool lt(double a, double b, double relTol);
        //   static bool le(double a, double b, double relTol);
        //   static bool gt(double a, double b, double relTol);
        //   static bool ge(double a, double b, double relTol);
        // --------------------------------------------------------------------
        if (verbose)
            cout << "\nEQUALITY AND RELATIONAL METHODS, RELATIVE TOLERANCE"
                    "\n===================================================\n";

        if (verbose)
                cout << "\tTesting using user-supplied relative tolerances:\n";

        static const struct {        // Test input/expected output struct:
            int    d_lineNum;        //   source line number
            double d_a;              //   rhs operand to test
            double d_bMinusA;        //   lhs operand = d_bMinusA + d_a
            double d_relTol;         //   relative tolerance
            int    d_eq;             //   return for eq(d_a, d_bMinusA + d_a)
            int    d_ne;             //   return for ne(d_a, d_bMinusA + d_a)
            int    d_lt;             //   return for lt(d_a, d_bMinusA + d_a)
            int    d_le;             //   return for le(d_a, d_bMinusA + d_a)
            int    d_ge;             //   return for ge(d_a, d_bMinusA + d_a)
            int    d_gt;             //   return for gt(d_a, d_bMinusA + d_a)
        } DATA[] = {
            //                                            Expected
            //                                        -----------------
            //line   d_a       d_bMinusA  relTol      EQ NE LT LE GE GT
            //----  --------   ---------  ------      -- -- -- -- -- --
            { L_,    1.0     ,   0.0    ,   1e-6,      1, 0, 0, 1, 1, 0 },
            { L_,   -1.0     ,   0.0    ,   1e-6,      1, 0, 0, 1, 1, 0 },
            { L_,    1.0     ,   1.0e-7 ,   1e-6,      1, 0, 0, 1, 1, 0 },
            { L_,    1.0     ,  -1.0e-7 ,   1e-6,      1, 0, 0, 1, 1, 0 },
            { L_,   -1.0     ,   1.0e-7 ,   1e-6,      1, 0, 0, 1, 1, 0 },
            { L_,   -1.0     ,  -1.0e-7 ,   1e-6,      1, 0, 0, 1, 1, 0 },
            { L_,    1.0     ,   1.1e-6 ,   1e-6,      0, 1, 1, 1, 0, 0 },
            { L_,    1.0     ,  -1.1e-6 ,   1e-6,      0, 1, 0, 0, 1, 1 },
            { L_,   -1.0     ,   1.1e-6 ,   1e-6,      0, 1, 1, 1, 0, 0 },
            { L_,   -1.0     ,  -1.1e-6 ,   1e-6,      0, 1, 0, 0, 1, 1 },
            { L_,    1.0     ,   1.0e-7 ,   0.0 ,      0, 1, 1, 1, 0, 0 },
            { L_,    1.0     ,  -1.0e-7 ,   0.0 ,      0, 1, 0, 0, 1, 1 },
            { L_,   -1.0     ,   1.0e-7 ,   0.0 ,      0, 1, 1, 1, 0, 0 },
            { L_,   -1.0     ,  -1.0e-7 ,   0.0 ,      0, 1, 0, 0, 1, 1 },
            { L_,    1.0     ,   1.0e-25,   0.0 ,      1, 0, 0, 1, 1, 0 },
            { L_,    1.0     ,  -1.0e-25,   0.0 ,      1, 0, 0, 1, 1, 0 },
            { L_,   -1.0     ,   1.0e-25,   0.0 ,      1, 0, 0, 1, 1, 0 },
            { L_,   -1.0     ,  -1.0e-25,   0.0 ,      1, 0, 0, 1, 1, 0 }
        };
        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    LINE   = DATA[i].d_lineNum;
            const double a      = DATA[i].d_a;
            const double b      = DATA[i].d_bMinusA + DATA[i].d_a;
            const double relTol = DATA[i].d_relTol;

            if (veryVerbose) { T_;  T_;  P_(a);  P_(b);  P(relTol); }

            typedef bdlb::DoubleCompareUtil DCU;
#define LASSERT(...) LOOP_ASSERT(LINE, __VA_ARGS__)
            LASSERT(DCU::fuzzyEq(a, b, relTol) == !!DATA[i].d_eq);
            LASSERT(DCU::fuzzyEq(b, a, relTol) == !!DATA[i].d_eq);
            LASSERT(DCU::fuzzyNe(a, b, relTol) == !!DATA[i].d_ne);
            LASSERT(DCU::fuzzyNe(b, a, relTol) == !!DATA[i].d_ne);
            LASSERT(DCU::fuzzyLt(a, b, relTol) == !!DATA[i].d_lt);
            LASSERT(DCU::fuzzyLe(a, b, relTol) == !!DATA[i].d_le);
            LASSERT(DCU::fuzzyGe(a, b, relTol) == !!DATA[i].d_ge);
            LASSERT(DCU::fuzzyGt(a, b, relTol) == !!DATA[i].d_gt);
#undef LASSERT
            const DCU::CompareResult RESULT = DCU::fuzzyCompare(a, b, relTol);
            if (DATA[i].d_eq) {
                LOOP_ASSERT(LINE, DCU::e_EQUAL == RESULT);
            }
            else if (DATA[i].d_lt) {
                LOOP_ASSERT(LINE, DCU::e_LESS_THAN == RESULT);
            }
            else {
                LOOP_ASSERT(LINE, DATA[i].d_gt);
                LOOP_ASSERT(LINE, DCU::e_GREATER_THAN == RESULT);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // EQUALITY AND RELATIONAL FUNCTIONS, DEFAULT TOLERANCES
        //
        // Concerns:
        //   Verification with default tolerances, where `eq` and `ne` should
        //   be symmetric in the first two arguments.
        //
        // Plan:
        //   Specify a set of values `(a, b)`.  For convenience of notation,
        //   tabulate a and b - a and verify that each of the six relational
        //   functions returns the expected value for the tabulated input.
        //   Test the symmetry of `eq` and `ne` in the first two arguments
        //   explicitly.
        //
        // Testing:
        //   static int compare(double a, double b);
        //   static bool eq(double a, double b);
        //   static bool ne(double a, double b);
        //   static bool lt(double a, double b);
        //   static bool le(double a, double b);
        //   static bool gt(double a, double b);
        //   static bool ge(double a, double b);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                   "\nEQUALITY AND RELATIONAL FUNCTIONS, DEFAULT TOLERANCES"
                   "\n=====================================================\n";

        //  1) Test eq(), etc., with default tolerances

        if (verbose) cout << "\tTesting using default tolerances:\n";

        static const struct {        // Test input/expected output struct:
            int    d_lineNum;        //   source line number
            double d_a;              //   rhs operand to test
            double d_bMinusA;        //   lhs operand = d_bMinusA + d_a
            int    d_eq;             //   return for eq(d_a, d_bMinusA + d_a)
            int    d_ne;             //   return for ne(d_a, d_bMinusA + d_a)
            int    d_lt;             //   return for lt(d_a, d_bMinusA + d_a)
            int    d_le;             //   return for le(d_a, d_bMinusA + d_a)
            int    d_ge;             //   return for ge(d_a, d_bMinusA + d_a)
            int    d_gt;             //   return for gt(d_a, d_bMinusA + d_a)
        } DATA[] = {
            //                                         Expected
            //                                     -----------------
            //line     d_a            d_bMinusA        EQ NE LT LE GE GT
            //---- --------------   --------------     -- -- -- -- -- --
            { L_,      1.0         ,    0.0          ,   1, 0, 0, 1, 1, 0 },
            { L_,     -1.0         ,    0.0          ,   1, 0, 0, 1, 1, 0 },
            { L_,      1.0         ,    0.9e-12      ,   1, 0, 0, 1, 1, 0 },
            { L_,      1.0         ,   -0.9e-12      ,   1, 0, 0, 1, 1, 0 },
            { L_,     -1.0         ,    0.9e-12      ,   1, 0, 0, 1, 1, 0 },
            { L_,     -1.0         ,   -0.9e-12      ,   1, 0, 0, 1, 1, 0 },
            { L_,      1.0         ,    1.1e-12      ,   0, 1, 1, 1, 0, 0 },
            { L_,      1.0         ,   -1.1e-12      ,   0, 1, 0, 0, 1, 1 },
            { L_,     -1.0         ,    1.1e-12      ,   0, 1, 1, 1, 0, 0 },
            { L_,     -1.0         ,   -1.1e-12      ,   0, 1, 0, 0, 1, 1 },
            { L_,      1.0e-20     ,    0.9e-24      ,   1, 0, 0, 1, 1, 0 },
            { L_,      1.0e-20     ,   -0.9e-24      ,   1, 0, 0, 1, 1, 0 },
            { L_,     -1.0e-20     ,    0.9e-24      ,   1, 0, 0, 1, 1, 0 },
            { L_,     -1.0e-20     ,   -0.9e-24      ,   1, 0, 0, 1, 1, 0 },
            { L_,      1.0e-20     ,    1.1e-24      ,   0, 1, 1, 1, 0, 0 },
            { L_,      1.0e-20     ,   -1.1e-24      ,   0, 1, 0, 0, 1, 1 },
            { L_,     -1.0e-20     ,    1.1e-24      ,   0, 1, 1, 1, 0, 0 },
            { L_,     -1.0e-20     ,   -1.1e-24      ,   0, 1, 0, 0, 1, 1 }
        };

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            const int    LINE = DATA[i].d_lineNum;
            const double a    = DATA[i].d_a;
            const double b    = DATA[i].d_bMinusA + DATA[i].d_a;

            if (veryVerbose) { T_;  T_;  P_(a);  P(b); }

            typedef bdlb::DoubleCompareUtil DCU;
#define LASSERT(...) LOOP_ASSERT(LINE, __VA_ARGS__)
            LASSERT(DCU::fuzzyEq(a, b) == !!DATA[i].d_eq);
            LASSERT(DCU::fuzzyEq(b, a) == !!DATA[i].d_eq);
            LASSERT(DCU::fuzzyNe(a, b) == !!DATA[i].d_ne);
            LASSERT(DCU::fuzzyNe(b, a) == !!DATA[i].d_ne);
            LASSERT(DCU::fuzzyLt(a, b) == !!DATA[i].d_lt);
            LASSERT(DCU::fuzzyLe(a, b) == !!DATA[i].d_le);
            LASSERT(DCU::fuzzyGe(a, b) == !!DATA[i].d_ge);
            LASSERT(DCU::fuzzyGt(a, b) == !!DATA[i].d_gt);
#undef LASSERT
            const DCU::CompareResult RESULT = DCU::fuzzyCompare(a, b);
            if (DATA[i].d_eq) {
                LOOP_ASSERT(LINE, DCU::e_EQUAL == RESULT);
            }
            else if (DATA[i].d_lt) {
                LOOP_ASSERT(LINE, DCU::e_LESS_THAN == RESULT);
            }
            else {
                LOOP_ASSERT(LINE, DATA[i].d_gt);
                LOOP_ASSERT(LINE, DCU::e_GREATER_THAN == RESULT);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // COMPARE TESTS
        //
        // Concerns:
        //   Test basic use of compare().
        //
        // Plan:
        //   Use table based testing to verify compare with an absolute, then
        //   with a relative tolerance.
        //
        // Testing:
        //   int compare(double a, double b, double relTol, double absTol)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nCOMPARE TESTS"
                             "\n=============\n";

        if (verbose) cout << "\tTesting `compare()` on absolute tolerances:\n";

        {
            double relTol = 0.0;         // relative tolerance
            double absTol = 1.0e-3;      // absolute tolerance

            typedef bdlb::DoubleCompareUtil DCU;

            static const struct {
                int                d_lineNum;  // source line number
                double             d_a;        // rhs operand to test
                double             d_b;        // lhs operand to test
                DCU::CompareResult d_spec;     // return for compare(d_a, d_b)
            } DATA[] = {
                //line     d_a            d_b           expected d_spec
                //---- --------------   -----------     -------------------
                { L_,      1.0         ,   1.0       ,  DCU::e_EQUAL        },
                { L_,      1.0         ,   0.9999    ,  DCU::e_EQUAL        },
                { L_,      1.0         ,   0.9995    ,  DCU::e_EQUAL        },
                { L_,      1.0         ,   0.999     ,  DCU::e_GREATER_THAN },
                { L_,      1.0         ,   0.99      ,  DCU::e_GREATER_THAN },
                { L_,      1.0         ,   0.9       ,  DCU::e_GREATER_THAN },
                { L_,      1.0         ,   1.1       ,  DCU::e_LESS_THAN    },
                { L_,      1.0         ,   1.01      ,  DCU::e_LESS_THAN    },
                { L_,      1.0         ,   1.00099   ,  DCU::e_EQUAL        },
                { L_,      1.0         ,   1.001     ,  DCU::e_EQUAL        },
                { L_,      1.0         ,   1.0001    ,  DCU::e_EQUAL        },
                { L_,      0.9999      ,   1.0       ,  DCU::e_EQUAL        },
                { L_,      0.9995      ,   1.0       ,  DCU::e_EQUAL        },
                { L_,      0.999       ,   1.0       ,  DCU::e_LESS_THAN    },
                { L_,      0.99        ,   1.0       ,  DCU::e_LESS_THAN    },
                { L_,      0.9         ,   1.0       ,  DCU::e_LESS_THAN    },
                { L_,     -1.0         ,   1.0       ,  DCU::e_LESS_THAN    },
                { L_,     -0.1         ,   0.1       ,  DCU::e_LESS_THAN    },
                { L_,     -0.01        ,   0.01      ,  DCU::e_LESS_THAN    },
                { L_,     -0.001       ,   0.001     ,  DCU::e_LESS_THAN    },
                { L_,     -0.0001      ,   0.0001    ,  DCU::e_EQUAL        },
                { L_,     -0.00001     ,   0.00001   ,  DCU::e_EQUAL        },
                { L_,     -0.000001    ,   0.000001  ,  DCU::e_EQUAL        },
                { L_,     -1.e-19      ,   1.e-19    ,  DCU::e_EQUAL        },
                { L_,      1.0         ,  -1.0       ,  DCU::e_GREATER_THAN },
                { L_,      0.1         ,  -0.1       ,  DCU::e_GREATER_THAN },
                { L_,      0.01        ,  -0.01      ,  DCU::e_GREATER_THAN },
                { L_,      0.001       ,  -0.001     ,  DCU::e_GREATER_THAN },
                { L_,      0.0001      ,  -0.0001    ,  DCU::e_EQUAL        },
                { L_,      0.00001     ,  -0.00001   ,  DCU::e_EQUAL        },
                { L_,      0.000001    ,  -0.000001  ,  DCU::e_EQUAL        },
                { L_,      1.e-19      ,  -1.e-19    ,  DCU::e_EQUAL        }
            };

            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE = DATA[i].d_lineNum;
                const double a    = DATA[i].d_a;
                const double b    = DATA[i].d_b;

                const DCU::CompareResult retVal = DCU::fuzzyCompare(a,
                                                                    b,
                                                                    relTol,
                                                                    absTol);

                if (veryVerbose) { T_;  T_;  P_(a);  P_(b);  P(retVal); }

                LOOP_ASSERT(LINE, DATA[i].d_spec == retVal);
            }
        }

        if (verbose) cout << "\tTesting compare() on relative tolerances:\n";

        {
            double relTol = 1.0e-3;             // relative tolerance
            double absTol = 0.0;                // absolute tolerance

            typedef bdlb::DoubleCompareUtil DCU;

            static const struct {
                int                d_lineNum;  // source line number
                double             d_a;        // rhs operand to test
                double             d_b;        // lhs operand to test
                DCU::CompareResult d_spec;     // return for compare(d_a, d_b)
            } DATA[] = {
                //line     d_a             d_b          expected d_spec
                //----  --------------   -----------    -------------------
                { L_,      1.0         ,   1.0       ,  DCU::e_EQUAL        },
                { L_,      1.0         ,   0.9999    ,  DCU::e_EQUAL        },
                { L_,      1.0         ,   0.9995    ,  DCU::e_EQUAL        },
                { L_,      1.0         ,   0.999     ,  DCU::e_GREATER_THAN },
                { L_,      1.0         ,   0.99      ,  DCU::e_GREATER_THAN },
                { L_,      1.0         ,   0.9       ,  DCU::e_GREATER_THAN },
                { L_,      1.0         ,   1.1       ,  DCU::e_LESS_THAN    },
                { L_,      1.0         ,   1.01      ,  DCU::e_LESS_THAN    },
                { L_,      1.0         ,   1.00099   ,  DCU::e_EQUAL        },
                { L_,      1.0         ,   1.001     ,  DCU::e_EQUAL        },
                { L_,      1.0         ,   1.0001    ,  DCU::e_EQUAL        },
                { L_,      0.9999      ,   1.0       ,  DCU::e_EQUAL        },
                { L_,      0.9995      ,   1.0       ,  DCU::e_EQUAL        },
                { L_,      0.999       ,   1.0       ,  DCU::e_LESS_THAN    },
                { L_,      0.99        ,   1.0       ,  DCU::e_LESS_THAN    },
                { L_,      0.9         ,   1.0       ,  DCU::e_LESS_THAN    },
                { L_,     -1.0         ,   1.0       ,  DCU::e_LESS_THAN    },
                { L_,     -0.1         ,   0.1       ,  DCU::e_LESS_THAN    },
                { L_,     -0.01        ,   0.01      ,  DCU::e_LESS_THAN    },
                { L_,     -0.001       ,   0.001     ,  DCU::e_LESS_THAN    },
                { L_,     -0.0001      ,   0.0001    ,  DCU::e_LESS_THAN    },
                { L_,     -0.00001     ,   0.00001   ,  DCU::e_LESS_THAN    },
                { L_,     -0.000001    ,   0.000001  ,  DCU::e_LESS_THAN    },
                { L_,     -1.e-19      ,   1.e-19    ,  DCU::e_LESS_THAN    },
                { L_,      1.0         ,  -1.0       ,  DCU::e_GREATER_THAN },
                { L_,      0.1         ,  -0.1       ,  DCU::e_GREATER_THAN },
                { L_,      0.01        ,  -0.01      ,  DCU::e_GREATER_THAN },
                { L_,      0.001       ,  -0.001     ,  DCU::e_GREATER_THAN },
                { L_,      0.0001      ,  -0.0001    ,  DCU::e_GREATER_THAN },
                { L_,      0.00001     ,  -0.00001   ,  DCU::e_GREATER_THAN },
                { L_,      0.000001    ,  -0.000001  ,  DCU::e_GREATER_THAN },
                { L_,      1.e-19      ,  -1.e-19    ,  DCU::e_GREATER_THAN }
            };

            const int NUM_DATA = sizeof DATA / sizeof DATA[0];

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    LINE = DATA[i].d_lineNum;
                const double a    = DATA[i].d_a;
                const double b    = DATA[i].d_b;

                const DCU::CompareResult retVal = DCU::fuzzyCompare(a,
                                                                    b,
                                                                    relTol,
                                                                    absTol);

                if (veryVerbose) { T_;  T_;  P_(a);  P_(b);  P(retVal); }

                LOOP_ASSERT(LINE, DATA[i].d_spec == retVal);
            }
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error: non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
