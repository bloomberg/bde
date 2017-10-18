// bdlsta_moment.t.cpp                                                -*-C++-*-
#include <bdlsta_moment.h>

#include <bdlb_float.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test consists of a templated object accumulating scalar
// values and supporting calculation of mean, variance, skew, and kurtosis.
// The general plan is that the methods are tested against a set of tabulated
// test vectors, and negative tests for preconditions are conducted.
// ----------------------------------------------------------------------------
// [ 2] Moment()
// [ 2] add(double value)
// [ 2] count()
// [ 2] kurtosis()
// [ 2] kurtosisIfValid()
// [ 2] mean()
// [ 2] meanIfValid()
// [ 2] skew()
// [ 2] skewIfValid()
// [ 2] variance()
// [ 2] varianceIfValid()
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] EDGE CASES
// [ 4] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

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
// NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//             NON-STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define DAY(X) bdlt::DayOfWeek::e_##X       // Shorten qualified name

// ============================================================================
//                      GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlsta::Moment<bdlsta::MomentLevel::e_M1> ObjM;
typedef bdlsta::Moment<bdlsta::MomentLevel::e_M2> ObjV;
typedef bdlsta::Moment<bdlsta::MomentLevel::e_M3> ObjS;
typedef bdlsta::Moment<bdlsta::MomentLevel::e_M4> ObjK;

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int         test = argc > 1 ? atoi(argv[1]) : 0;
    bool     verbose = argc > 2;
    bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Calculating skew, variance, and mean
///- - - - - - - - - - - - - - - -
// This example shows how to accumulate a set of value, and calculate the skew,
// variance and kurtosis.
//
// First, we create example input and instantiate the appropriate mechanism:
//..
    double input[] = { 1.0, 2.0, 4.0, 5.0 };

    bdlsta::Moment<bdlsta::MomentLevel::e_M3> m3;
//..
// Then, we invoke the 'add' routine to accumulate the data:
//..
    for(int i = 0; i < 4; ++i) {
        m3.add(input[i]);
    }
//..
// Finally, we assert that the mean, variance, and skew are what we expect:
//..
    ASSERT(4   == m3.count());
    ASSERT(3.0 == m3.mean());
    ASSERT(1e-5 > fabs(3.33333 - m3.variance()));
    ASSERT(1e-5 > fabs(0.0     - m3.skew()));
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING EDGE CASES
        //
        // Concerns:
        //: 1 'meanIfValid' returns '-1' when no data is fed.
        //:
        //: 2 'varianceIfValid' returns '-1' with less than 2 data values.
        //:
        //: 3 'skewIfValid' returns '-1' with less than 3 data values.
        //:
        //: 4 'kurtosisIfValid' returns '-1' with less than 4 data values.
        //
        // Plan:
        //: 1 Verify the 'meanIfValid' with no data returns '-1'.  (C-1)
        //:
        //: 2 Verify the 'varianceIfValid' with 1 data value returns '-1'.
        //:   (C-2)
        //:
        //: 3 Verify the 'skewIfValid' with 2 data values returns '-1'.  (C-3)
        //:
        //: 4 Verify the 'kurtosisIfValid' with 3 data values returns '-1'.
        //:   (C-4)
        //
        // Testing:
        //   EDGE CASES
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING EDGE CASES" << endl
                          << "==================" << endl;

        bsls::AssertFailureHandlerGuard hG(
                                         bsls::AssertTest::failTestDriver);

        {
            bdlsta::Moment<bdlsta::MomentLevel::e_M1> m1;
            ASSERT(0 == m1.count());
            double result;
            ASSERT(-1 == m1.meanIfValid(&result));
            ASSERT_SAFE_FAIL(m1.mean());
        }

        {
            bdlsta::Moment<bdlsta::MomentLevel::e_M2> m2;
            m2.add(1.0);
            ASSERT(1 == m2.count());
            double result;
            ASSERT(-1 == m2.varianceIfValid(&result));
            ASSERT_SAFE_FAIL(m2.variance());
        }

        {
            bdlsta::Moment<bdlsta::MomentLevel::e_M3> m3;
            m3.add(1.0);
            m3.add(2.0);
            ASSERT(2 == m3.count());
            double result;
            ASSERT(-1 == m3.skewIfValid(&result));
            ASSERT_SAFE_FAIL(m3.skew());
        }

        {
            bdlsta::Moment<bdlsta::MomentLevel::e_M4> m4;
            m4.add(1.0);
            m4.add(2.0);
            m4.add(4.0);
            ASSERT(3 == m4.count());
            double result;
            ASSERT(-1 == m4.kurtosisIfValid(&result));
            ASSERT_SAFE_FAIL(m4.kurtosis());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS, BASIC ACCESSORS, AND ACCESSORS
        //   This test will verify that the primary manipulators are working as
        //   expected.  As basic accessors should be tested exactly the same
        //   way, these two tests have been united.  So we test that the basic
        //   accessors are working as expected also.  The other accessors are
        //   added here as well.
        //
        // Concerns:
        //: 1 All accessors return the expected values after 'bdlsta::Moment'
        //:   loaded with a series of values.
        //:
        //: 2 Different specializations produce the same set of values.
        //
        // Plan:
        //: 1 Using the table method, create
        //    'bdlsta::Moment<MomentLevel::e_M4>', use
        //:   'bdlsta::Moment<MomentLevel::e_M4>::add' to load data, and check
        //:   the values of mean, variance, skew, and kurtosis.  (C-1)
        //:
        //: 2 Verify that we get the same values from the 'raw' methods.  (C-1)
        //:
        //: 3 Load the same set of data into
        //:   'bdlsta::Moment<MomentLevel::e_M3>',
        //:   'bdlsta::Moment<MomentLevel::e_M2>',
        //:   'bdlsta::Moment<MomentLevel::e_M1>', and verify that we get the
        //:   same values.  (C-2)
        //
        // Testing:
        //   Moment()
        //   add(double value)
        //   count()
        //   meanIfValid()
        //   mean()
        //   varianceIfValid()
        //   variance()
        //   skewIfValid()
        //   skew()
        //   kurtosisIfValid()
        //   kurtosis()
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "PRIMARY MANIPULATORS, BASIC ACCESSORS, AND ACCESSORS\n"
                 << "===================================================="
                 << endl;

        static const struct {
            int    d_line;
            double d_values[7];
            int    d_count;
            double d_mean;
            double d_variance;
            double d_skew;
            double d_kurtosis;
        } INPUT[] = {
            //LN  val1   val2   val3   val4   val5   val6   val7
            //--  -----  -----  -----  -----  -----  -----  -----
            //Cnt mean      variance  skew      kurtosis
            //--- --------  --------  --------  --------
            { L_, 1.0  , 2.0   , 0.0  ,  0.0 ,  0.0 , 0.0  ,  0.0,
              2 ,1.5      ,0.5    , 0.0      , 0.0                          },
            { L_, 1e3  , 2e3   , 0.0  , 0.0  ,  0.0 , 0.0  , 0.0 ,
              2 ,1500     ,5E5     , 0.0     , 0.0                          },
            { L_, 1.0  , 2.0   , 4.0  ,  0.0 ,  0.0 , 0.0  ,  0.0,
              3 ,2.33333  ,2.33333, 0.93522  , 0.0                          },
            { L_, 1e3  , 2e3   , 3e3  , 0.0  ,  0.0 , 0.0  , 0.0 ,
              3 ,2000     ,1E6     , 0.0     , 0.0                          },
            { L_, 1.0  , 2.0   , 4.0  , 12.0 ,  0.0 , 0.0  ,  0.0,
              4 ,4.75     ,24.9167, 1.64622  , 2.70427                      },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  ,  0.0 , 0.0  , 0.0 ,
              4 ,2500     ,1.6667E6, 0.0     , -1.2                         },
            { L_, 1.0  , 2.0   , 4.0  , 12.0 , 20.0 , 0.0  ,  0.0,
              5 ,7.8      ,65.2   , 1.04907  , -0.371627                    },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  , 1e-3 , 0.0  , 0.0 ,
              5 ,2000     ,2.5E6  , 6.3245E-7, -1.2                         },
            { L_, 1.0  , 2.0   , 4.0  , 12.0 , 20.0 , 25.0 ,  0.0,
              6 ,10.6667  ,101.467, 0.560491 , -1.75255                     },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  , 1e-3 , 2e-3 , 0.0 ,
              6 ,1666.7   ,2.6667E6, 0.382734  , -1.48125                   },
            { L_, 1.0  , 2.0   , 4.0  , 12.0 , 20.0 , 25.0 , 30.0,
              7 ,13.4286  ,137.952, 0.30677  , -1.86809                     },
            { L_,30.0  , 25.0  , 20.0 , 12.0 , 4.0  , 2.0  , 1.0,
              7 ,13.4286  ,137.952, 0.30677  , -1.86809                     },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  , 1.0  , 2.0  , 3.0,
              7 ,1429.43  ,2.616E6, 0.674989 , -1.14935                     },
            { L_, 1.0  , 2.0   , 3.0  , 4e3  , 1e3  , 2e3  , 3e3,
              7 ,1429.43  ,2.616E6, 0.674989 , -1.14935                     },
            { L_,-123.0,-55.0  , 4.0  , 0.0  , 100.0, 24.0 , 24.0,
              7 ,-3.71429 ,4870.9 ,-0.46229  , 1.01756                      },
            { L_, 1.0  ,  2.0  , 3.0  , 4.0  , 5.0  ,  6.0 , 7.0 ,
              7 ,  4.0 ,4.6667  ,0.0       , -1.2                           },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  , 1e-3 , 2e-3 , 3e-3,
              7 ,1428.57  ,2.619E6, 0.67409   , -1.15101                    },
            { L_, 1e-3 , 2e-3  , 3e-3 , 4e3  , 1e3  , 2e3  , 3e3 ,
              7 ,1428.57  ,2.619E6, 0.67409   , -1.15101                    },
        };

        const size_t NUM_DATA = sizeof INPUT / sizeof *INPUT;

        if (verbose) cout << "Testing M4\n";
        {
            for (size_t di = 0; di < NUM_DATA; ++di) {
                ObjK m4;

                const int     LINE   = INPUT[di].d_line;
                const double *VALUES = INPUT[di].d_values;
                const int     COUNT  = INPUT[di].d_count;
                if (COUNT < 4) continue;
                for(int i = 0; i < COUNT; ++i) {
                    m4.add(VALUES[i]);
                    if (veryVerbose) {
                        P_(LINE) P_(i) P(VALUES[i]);
                    }
                }
                const int    expectedCount    = INPUT[di].d_count;
                const double expectedMean     = INPUT[di].d_mean;
                const double expectedVariance = INPUT[di].d_variance;
                const double expectedSkew     = INPUT[di].d_skew;
                const double expectedKurtosis = INPUT[di].d_kurtosis;
                const int    count       = m4.count();
                double       mean = 0.0;
                const int    meanRet     = m4.meanIfValid(&mean);
                const double meanRaw     = m4.mean();
                double       variance = 0.0;
                const int    varianceRet = m4.varianceIfValid(&variance);
                const double varianceRaw = m4.variance();
                double       skew = 0.0;
                const int    skewRet     = m4.skewIfValid(&skew);
                const double skewRaw     = m4.skew();
                double       kurtosis = 0.0;
                const int    kurtosisRet = m4.kurtosisIfValid(&kurtosis);
                const double kurtosisRaw = m4.kurtosis();

                if (veryVerbose) {
                    T_ P_(LINE) P_(mean) P_(meanRet) P_(meanRaw)
                                                     P(expectedMean);
                    T_ P_(variance) P_(varianceRet) P_(varianceRaw)
                                                    P(expectedVariance);
                    T_ P_(skew) P_(skewRet) P_(skewRaw) P(expectedSkew);
                    T_ P_(kurtosis) P_(kurtosisRet) P_(kurtosisRaw)
                                                    P(expectedKurtosis);
                }

                LOOP3_ASSERT(di,
                             count,
                             expectedCount,
                             expectedCount == count);
                LOOP3_ASSERT(di,
                             mean,
                             expectedMean,
                             fabs((expectedMean - mean) /
                                  (expectedMean + 1e-10)) < 1e-4);
                LOOP3_ASSERT(di,
                             meanRaw,
                             expectedMean,
                             fabs((expectedMean - meanRaw) /
                                  (expectedMean + 1e-10)) < 1e-4);
                LOOP3_ASSERT(di,
                             variance,
                             expectedVariance,
                             fabs((expectedVariance - variance) /
                                  (expectedVariance + 1e-10)) < 1e-4);
                LOOP3_ASSERT(di,
                             varianceRaw,
                             expectedVariance,
                             fabs((expectedVariance - varianceRaw) /
                                  (expectedVariance + 1e-10)) < 1e-4);
                LOOP3_ASSERT(di,
                             skew,
                             expectedSkew,
                             fabs((expectedSkew - skew) /
                                  (expectedSkew + 1e-10)) < 1e-4);
                LOOP3_ASSERT(di,
                             skewRaw,
                             expectedSkew,
                             fabs((expectedSkew - skewRaw) /
                                  (expectedSkew + 1e-10)) < 1e-4);
                LOOP3_ASSERT(di,
                             kurtosis,
                             expectedKurtosis,
                             fabs((expectedKurtosis - kurtosis) /
                                  (expectedKurtosis + 1e-10)) < 1e-4);
                LOOP3_ASSERT(di,
                             kurtosisRaw,
                             expectedKurtosis,
                             fabs((expectedKurtosis - kurtosisRaw) /
                                  (expectedKurtosis + 1e-10)) < 1e-4);
                LOOP2_ASSERT(di,
                             meanRet,
                             0 == meanRet);
                LOOP2_ASSERT(di,
                             varianceRet,
                             0 == varianceRet);
                LOOP2_ASSERT(di,
                             skewRet,
                             0 == skewRet);
                LOOP2_ASSERT(di,
                             kurtosisRet,
                             0 == kurtosisRet);
            }
        }

        if (verbose) cout << "Testing M3\n";
        {
            for (size_t di = 0; di < NUM_DATA; ++di) {
                ObjS m3;

                const int     LINE   = INPUT[di].d_line;
                const double *VALUES = INPUT[di].d_values;
                const int     COUNT  = INPUT[di].d_count;
                if (COUNT < 3) continue;
                for(int i = 0; i < COUNT; ++i) {
                    m3.add(VALUES[i]);
                    if (veryVerbose) {
                        P_(LINE) P_(i) P(VALUES[i]);
                    }
                }
                const int    expectedCount    = INPUT[di].d_count;
                const double expectedMean     = INPUT[di].d_mean;
                const double expectedVariance = INPUT[di].d_variance;
                const double expectedSkew     = INPUT[di].d_skew;
                const int    count       = m3.count();
                double       mean = 0.0;
                const int    meanRet     = m3.meanIfValid(&mean);
                const double meanRaw     = m3.mean();
                double       variance = 0.0;
                const int    varianceRet = m3.varianceIfValid(&variance);
                const double varianceRaw = m3.variance();
                double       skew = 0.0;
                const int    skewRet     = m3.skewIfValid(&skew);
                const double skewRaw     = m3.skew();

                if (veryVerbose) {
                    T_ P_(LINE) P_(mean) P_(meanRet) P_(meanRaw)
                                                     P(expectedMean);
                    T_ P_(variance) P_(varianceRet) P_(varianceRaw)
                                                    P(expectedVariance);
                    T_ P_(skew) P_(skewRet) P_(skewRaw) P(expectedSkew);
                }

                LOOP3_ASSERT(di,
                             count,
                             expectedCount,
                             expectedCount == count);
                LOOP3_ASSERT(di,
                             mean,
                             expectedMean,
                             fabs((expectedMean - mean) /
                                  (expectedMean + 1e-10)) < 1e-4);
                LOOP3_ASSERT(di,
                             meanRaw,
                             expectedMean,
                             fabs((expectedMean - meanRaw) /
                                  (expectedMean + 1e-10)) < 1e-4);
                LOOP3_ASSERT(di,
                             variance,
                             expectedVariance,
                             fabs((expectedVariance - variance) /
                                  (expectedVariance + 1e-10)) < 1e-4);
                LOOP3_ASSERT(di,
                             varianceRaw,
                             expectedVariance,
                             fabs((expectedVariance - varianceRaw) /
                                  (expectedVariance + 1e-10)) < 1e-4);
                LOOP3_ASSERT(di,
                             skew,
                             expectedSkew,
                             fabs((expectedSkew - skew) /
                                  (expectedSkew + 1e-10)) < 1e-4);
                LOOP3_ASSERT(di,
                             skewRaw,
                             expectedSkew,
                             fabs((expectedSkew - skewRaw) /
                                  (expectedSkew + 1e-10)) < 1e-4);
                LOOP2_ASSERT(di,
                             meanRet,
                             0 == meanRet);
                LOOP2_ASSERT(di,
                             varianceRet,
                             0 == varianceRet);
                LOOP2_ASSERT(di,
                             skewRet,
                             0 == skewRet);
            }
        }

        if (verbose) cout << "Testing M2\n";
        {
            for (size_t di = 0; di < NUM_DATA; ++di) {
                ObjV m2;

                const int     LINE   = INPUT[di].d_line;
                const double *VALUES = INPUT[di].d_values;
                const int     COUNT  = INPUT[di].d_count;
                for(int i = 0; i < COUNT; ++i) {
                    m2.add(VALUES[i]);
                    if (veryVerbose) {
                        P_(LINE) P_(i) P(VALUES[i]);
                    }
                }
                const int    expectedCount    = INPUT[di].d_count;
                const double expectedMean     = INPUT[di].d_mean;
                const double expectedVariance = INPUT[di].d_variance;
                const int    count       = m2.count();
                double       mean = 0.0;
                const int    meanRet     = m2.meanIfValid(&mean);
                const double meanRaw     = m2.mean();
                double       variance = 0.0;
                const int    varianceRet = m2.varianceIfValid(&variance);
                const double varianceRaw = m2.variance();

                if (veryVerbose) {
                    T_ P_(LINE) P_(mean) P_(meanRet) P_(meanRaw)
                                                     P(expectedMean);
                    T_ P_(variance) P_(varianceRet) P_(varianceRaw)
                                                    P(expectedVariance);
                }

                LOOP3_ASSERT(di,
                             count,
                             expectedCount,
                             expectedCount == count);
                LOOP3_ASSERT(di,
                             mean,
                             expectedMean,
                             fabs((expectedMean - mean) /
                                  (expectedMean + 1e-10)) < 1e-4);
                LOOP3_ASSERT(di,
                             meanRaw,
                             expectedMean,
                             fabs((expectedMean - meanRaw) /
                                  (expectedMean + 1e-10)) < 1e-4);
                LOOP3_ASSERT(di,
                             variance,
                             expectedVariance,
                             fabs((expectedVariance - variance) /
                                  (expectedVariance + 1e-10)) < 1e-4);
                LOOP3_ASSERT(di,
                             varianceRaw,
                             expectedVariance,
                             fabs((expectedVariance - varianceRaw) /
                                  (expectedVariance + 1e-10)) < 1e-4);
                LOOP2_ASSERT(di,
                             meanRet,
                             0 == meanRet);
                LOOP2_ASSERT(di,
                             varianceRet,
                             0 == varianceRet);
            }
        }

        if (verbose) cout << "Testing M1\n";
        {
            for (size_t di = 0; di < NUM_DATA; ++di) {
                ObjM m1;

                const int     LINE   = INPUT[di].d_line;
                const double *VALUES = INPUT[di].d_values;
                const int     COUNT  = INPUT[di].d_count;
                for(int i = 0; i < COUNT; ++i) {
                    m1.add(VALUES[i]);
                    if (veryVerbose) {
                        P_(LINE) P_(i) P(VALUES[i]);
                    }
                }
                const int    expectedCount    = INPUT[di].d_count;
                const double expectedMean     = INPUT[di].d_mean;
                const int    count       = m1.count();
                double       mean = 0.0;
                const int    meanRet     = m1.meanIfValid(&mean);
                const double meanRaw     = m1.mean();

                if (veryVerbose) {
                    T_ P_(LINE) P_(mean) P_(meanRet) P_(meanRaw)
                                                     P(expectedMean);
                }

                LOOP3_ASSERT(di,
                             count,
                             expectedCount,
                             expectedCount == count);
                LOOP3_ASSERT(di,
                             mean,
                             expectedMean,
                             fabs((expectedMean - mean) /
                                  (expectedMean + 1e-10)) < 1e-4);
                LOOP3_ASSERT(di,
                             meanRaw,
                             expectedMean,
                             fabs((expectedMean - meanRaw) /
                                  (expectedMean + 1e-10)) < 1e-4);
                LOOP2_ASSERT(di,
                             meanRet,
                             0 == meanRet);
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
        //: 1 Developer test sandbox. (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        double input[] = { 1.0, 2.0, 4.0, 5.0 };

        bdlsta::Moment<bdlsta::MomentLevel::e_M1> m1;
        for(int i = 0; i < 4; ++i) {
            m1.add(input[i]);
            if (veryVerbose) {
                P_(i) P(input[i]);
            }
        }
        ASSERT(4   == m1.count());
        ASSERT(3.0 == m1.mean());
        // The below does not compile, as it should
        //ASSERT(fabs(3.33333 - m2.variance()) < 1e-5);

        bdlsta::Moment<bdlsta::MomentLevel::e_M2> m2;
        for(int i = 0; i < 4; ++i) {
            m2.add(input[i]);
            if (veryVerbose) {
                P_(i) P(input[i]);
            }
        }
        ASSERT(4           == m2.count());
        ASSERT(3.0         == m2.mean());
        ASSERT(fabs(3.33333 - m2.variance()) < 1e-5);

        bdlsta::Moment<bdlsta::MomentLevel::e_M3> m3;
        for(int i = 0; i < 4; ++i) {
            m3.add(input[i]);
            if (veryVerbose) {
                P_(i) P(input[i]);
            }
        }
        ASSERT(4           == m3.count());
        ASSERT(3.0         == m3.mean());
        ASSERT(fabs(3.33333 - m3.variance()) < 1e-5);
        ASSERT(fabs(0.0     - m3.skew())     < 1e-5);
        // The below would not compile since the object was created using
        // 'e_M3':
        //ASSERT(fabs(644.185 - m3.kurtosis()) < 1e-3);

        bdlsta::Moment<bdlsta::MomentLevel::e_M4> m4;
        for(int i = 0; i < 4; ++i) {
            m4.add(input[i]);
            if (veryVerbose) {
                P_(i) P(input[i]);
            }
        }
        ASSERT(4           == m4.count());
        ASSERT(3.0         == m4.mean());
        ASSERT(fabs(3.33333 - m4.variance()) < 1e-5);
        ASSERT(fabs(0.0     - m4.skew())     < 1e-5);
        ASSERT(fabs(-3.3    - m4.kurtosis()) < 1e-3);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
