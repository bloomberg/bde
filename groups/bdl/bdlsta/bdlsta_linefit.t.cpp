// bdlsta_linefit.t.cpp                                               -*-C++-*-
#include <bdlsta_linefit.h>

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
// The component under test consists of an object accumulating data points
// (X,Y) and supporting calculation of a least squares line fit, as well as
// mean and variance.  The methods under test will be directly verified through
// the use of tabulated test vectors.  Furthermore, detection of all
// preconditions (i.e., negative tests) is done directly in appropriate build
// modes.
// ----------------------------------------------------------------------------
// [ 2] LineFit()
// [ 2] add(double xValue, yValue)
// [ 2] int count()
// [ 2] void fit(double *alpha, double *beta)
// [ 3] int fitIfValid(double *alpha, double *beta)
// [ 2] double xMean()
// [ 3] int xMeanIfValid(double *)
// [ 2] double yMean()
// [ 3] int yMeanIfValid(double *)
// [ 2] double variance()
// [ 3] int varianceIfValid(double *)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] EDGE CASES
// [ 5] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
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
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlsta::LineFit Obj;

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
      case 5: {
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
///Example 1: Calculating line fit, variance, and mean
///- - - - - - - - - - - - - - - -
// This example shows how to accumulate a set of values, and calculate the
// line fit parameters, variance and mean.
//
// First, we create example input and instantiate the appropriate mechanism:
//..
  double          inputX[] = { 1.0, 2.0, 4.0, 5.0 };
  double          inputY[] = { 1.0, 2.0, 4.0, 4.5 };
  bdlsta::LineFit lineFit;
//..
// Then, we invoke the 'add' routine to accumulate the data:
//..
  for(int i = 0; i < 4; ++i) {
      lineFit.add(inputX[i], inputY[i]);
  }
//..
// Finally, we assert that the alpha, beta, variance, and mean are what we
// expect:
//..
  double alpha = 0.0, beta = 0.0;
  ASSERT(4    == lineFit.count());
  ASSERT(3.0  == lineFit.xMean());
  ASSERT(1e-3 >  fabs(2.875   - lineFit.yMean()));
  ASSERT(1e-3 >  fabs(3.33333 - lineFit.variance()));
  ASSERT(0    == lineFit.fitIfValid(&alpha, &beta));
  ASSERT(1e-3 >  fabs(0.175 - alpha));
  ASSERT(1e-3 >  fabs(0.9   - beta ));
//..
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING EDGE CASES
        //
        // Concerns:
        //: 1 'xMeanIfValid' returns '-1' when no data is supplied.
        //:
        //: 2 'yMeanIfValid' returns '-1' when no data is supplied.
        //:
        //: 3 'varianceIfValid' returns '-1' with less than 2 data values.
        //:
        //: 4 'fit' returns '-1' with less than 2 data values or all X's are
        //:   identical.
        //:
        //
        // Plan:
        //: 1 Verify that 'xMeanIfValid' with no data returns '-1'.  (C-1)
        //:
        //: 2 Verify that 'yMeanIfValid' with no data returns '-1'.  (C-1)
        //:
        //: 3 Verify that 'varianceIfValid' with 1 data value returns '-1'.
        //:   (C-3)
        //:
        //: 4 Verify that 'fit' with 1 data value returns '-1'.  (C-4)
        //:
        //: 5 Verify that 'fit' with 2 identical X's returns '-1'.  (C-4)
        //:
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
            bdlsta::LineFit lf;
            ASSERT(0 == lf.count());
            double result;
            ASSERT(-1 == lf.xMeanIfValid(&result));
            ASSERT_SAFE_FAIL(lf.xMean());
            ASSERT(-1 == lf.yMeanIfValid(&result));
            ASSERT_SAFE_FAIL(lf.yMean());
        }

        {
            bdlsta::LineFit lf;
            lf.add(1.0, 2.0);
            ASSERT(1 == lf.count());
            double result;
            ASSERT(-1 == lf.varianceIfValid(&result));
            ASSERT_SAFE_FAIL(lf.variance());
            double alpha, beta;
            ASSERT(-1 == lf.fitIfValid(&alpha, &beta));
            lf.add(1.0, 3.0);
            ASSERT(-1 == lf.fitIfValid(&alpha, &beta));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // OTHER ACCESSORS
        //   This test will verify that the other accessors are working as
        //   expected.
        //
        // Concerns:
        //: 1 All accessors return the expected values after 'bdlsta::LineFit'
        //:   loaded with a series of values.
        //
        // Plan:
        //: 1 Using the table method, create 'bdlsta::LineFit', use
        //:   'bdlsta::LineFit::add' to load data, and check the values of
        //:   fitIfValid, xMeanIfValid, yMeanIfValid, and varianceIfValid.
        //:   (C-1)
        //
        // Testing:
        //   int fitIfValid(double *alpha, double *beta)
        //   int xMeanIfValid(double *)
        //   int yMeanIfValid(double *)
        //   int varianceIfValid(double *)
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "OTHER ACCESSORS" << endl
                 << "===============" << endl;

        static const struct {
            int    d_line;
            double d_xValues[7];
            int    d_count;
            double d_yValues[7];
            double d_xMean;
            double d_yMean;
            double d_variance;
            double d_alpha;
            double d_beta;
        } INPUT[] = {
            //LN  xVal1  xVal2  xVal3  xVal4  xVal5  xVal6  xVal7
            //--  -----  -----  -----  -----  -----  -----  -----
            //cnt yVal1  yVal2  yVal3  yVal4  yVal5  yVal6  yVal7
            //--- -----  -----  -----  -----  -----  -----  -----
            //xMean     yMean     variance  alpha    beta
            //--------  --------  --------  -------- --------
            { L_, 1e3  , 2e3   , 0.0  , 0.0  , 0.0  , 0.0  , 0.0,
              2 , 2.1e3, 4e3   , 0.0  , 0.0  , 0.0  , 0.0  , 0.0,
              1500.0   ,3050.0 , 5E5    , 200.0  , 1.9                      },
            { L_, 1.0  , 2.0   , 0.0  , 0.0  , 0.0  , 0.0  , 0.0,
              2 ,-1.1  ,-2.0   , 0.0  , 0.0  , 0.0  , 0.0  , 0.0,
              1.5      ,-1.55   , 0.5    , -0.2    , -0.9                   },
            { L_, 1e3  , 2e3   , 3e3  , 0.0  , 0.0  , 0.0  , 0.0,
              3 , 2.1e3, 4e3   , 6.1e3, 0.0  , 0.0  , 0.0  , 0.0,
              2000     ,4066.67, 1E6    , 66.6667, 2.0                      },
            { L_, 1.0  , 2.0   , 3.0  , 0.0  , 0.0  , 0.0  , 0.0,
              3 ,-1.1  ,-2.0   ,-3.2  , 0.0  , 0.0  , 0.0  , 0.0,
              2.0      ,-2.1    , 1.0    , 0.0     , -1.05                  },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  , 0.0  , 0.0  , 0.0,
              4 , 2.1e3, 4e3   , 6.1e3, 8e3  , 0.0  , 0.0  , 0.0,
              2500.0   ,5050.0 , 1.667E6, 100    , 1.98                     },
            { L_, 1.0  , 2.0   , 3.0  , 4e3  , 0.0  , 0.0  , 0.0,
              4 ,-1.1  ,-2.0   ,-3.2  ,-4.1e3, 0.0  , 0.0  , 0.0,
              1001.5   ,-1026.58, 3.996E6, -0.05002, -1.02499               },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  , 1.0  , 0.0  , 0.0,
              5 , 2.1e3, 4e3   , 6.1e3, 8e3  , 1.2  , 0.0  , 0.0,
              2000.2   ,4040.24, 2.499E6, 39.528 , 2.00016                  },
            { L_, 1.0  , 2.0   , 3.0  , 4e3  , 1e3  , 0.0  , 0.0,
              5 ,-1.1  ,-2.0   ,-3.2  ,-4.1e3,-1e3  , 0.0  , 0.0,
              1001.2   ,-1021.26, 2.997E6, 4.95999 , -1.02499               },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  , 1.0  , 2.0  , 0.0,
              6 , 2.1e3, 4e3   , 6.1e3, 8e3  , 1.2  , 2.1  , 0.0,
              1667.17  ,3367.22, 2.665E6, 23.9911, 2.00533                  },
            { L_, 1.0  , 2.0   , 3.0  , 4e3  , 1e3  , 2e3  , 0.0,
              6 ,-1.1  ,-2.0   ,-3.2  ,-4.1e3,-1e3  ,-2.2e3, 0.0,
              1167.67  ,-1217.72, 2.564E6, -9.12032, -1.03505               },
            { L_, 1.0  , 2.0   , 4.0  , 12.0 , 20.0 , 25.0 , 30.0,
              7 , 1.2  , 2.3   , 4.0  , 12.0 , 20.9 , 25.5 , 29.0,
              13.4286  ,13.5571, 137.952, 0.303555, 0.986969                },
            { L_,30.0  , 25.0  , 20.0 , 12.0 , 4.0  , 2.0  , 1.0,
              7 ,31.0  , 25.5  , 20.3 , 12.4 , 4.0  , 2.1  , 1.3,
              13.4286  ,13.8   , 137.952, 0.0650328, 1.02282                },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  , 1.0  , 2.0  , 3.0,
              7 , 2.1e3, 4e3   , 6.1e3, 8e3  , 1.2  , 2.1  , 3.0,
              1429.43  ,2886.61, 2.616E6, 16.6248, 2.00779                  },
            { L_, 1.0  , 2.0   , 3.0  , 4e3  , 1e3  , 2e3  , 3e3,
              7 ,-1.1  ,-2.0   ,-3.2  ,-4.1e3,-1e3  ,-2.2e3,-3e3,
              1429.43  ,-1472.33, 2.616E6, -9.13903, -1.02362               },
            { L_,-123.0,-55.0  , 4.0  , 0.0  , 100.0, 24.0 , 24.0,
              7 ,-123.0,-55.0  , 4.0  , 0.0  , 100.0, 24.0 , 24.0,
              -3.71429 ,-3.71429, 4870.9, 0.0, 1.0                          },
            { L_, 1.0  ,  2.0  , 3.0  , 4.0  , 5.0  ,  6.0 , 7.0 ,
              7 , 1.1  ,  2.0  , 3.2  , 4.4  , 5.0  ,  6.5 , 7.1 ,
                   4.0 ,4.18571, 4.6667, 0.071428, 1.02857                  },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  , 1e-3 , 2e-3 , 3e-3,
              7 , 3.2e3, 6e3   , 9.1e3, 12e3 , 3e-3 , 6e-3 , 9e-3,
             1428.57  ,4328.57 , 2.619E6, 36.3636, 3.00455                  },
            { L_, 1e-3 , 2e-3  , 3e-3 , 4e3  , 1e3  , 2e3  , 3e3 ,
              7 , 1e-3 , 2e-3  , 3e-3 , 4.2e3, 1e3  , 2.2e3, 3.1e3,
             1428.57  ,1500.0  , 2.619E6, -8.18182e-05, 1.05                },
        };

        const size_t NUM_DATA = sizeof INPUT / sizeof *INPUT;

        {
            for (size_t di = 0; di < NUM_DATA; ++di) {
                Obj lf;

                const int     LINE    = INPUT[di].d_line;
                const int     COUNT   = INPUT[di].d_count;
                const double *XVALUES = INPUT[di].d_xValues;
                const double *YVALUES = INPUT[di].d_yValues;
                for(int i = 0; i < COUNT; ++i) {
                    lf.add(XVALUES[i], YVALUES[i]);
                    if (veryVerbose) {
                        P_(LINE) P_(i) P_(XVALUES[i]) P(YVALUES[i]);
                    }
                }
                const int    expectedCount    = INPUT[di].d_count;
                const double expectedXMean    = INPUT[di].d_xMean;
                const double expectedYMean    = INPUT[di].d_yMean;
                const double expectedVariance = INPUT[di].d_variance;
                const double expectedAlpha    = INPUT[di].d_alpha;
                const double expectedBeta     = INPUT[di].d_beta;
                const int    count       = lf.count();
                double       xMean = 0.0;
                const int    xMeanRet    = lf.xMeanIfValid(&xMean);
                double       yMean = 0.0;
                const int    yMeanRet    = lf.yMeanIfValid(&yMean);
                double       variance = 0.0;
                const int    varianceRet = lf.varianceIfValid(&variance);
                double       alpha = 0.0, beta = 0.0;
                const int    lineFitRet  = lf.fitIfValid(&alpha, &beta);

                if (veryVerbose) {
                    T_ P_(LINE) P_(xMean) P_(xMeanRet) P(expectedXMean);
                    T_ P_(LINE) P_(yMean) P_(yMeanRet) P(expectedYMean);
                    T_ P_(variance) P_(varianceRet) P(expectedVariance);
                }

                LOOP3_ASSERT(di,
                             count,
                             expectedCount,
                             expectedCount == count);
                LOOP3_ASSERT(di,
                             xMean,
                             expectedXMean,
                             fabs((expectedXMean - xMean) /
                                  (expectedXMean + 1e-10)) < 1e-3);
                LOOP3_ASSERT(di,
                             yMean,
                             expectedYMean,
                             fabs((expectedYMean - yMean) /
                                  (expectedYMean + 1e-10)) < 1e-3);
                LOOP3_ASSERT(di,
                             variance,
                             expectedVariance,
                             fabs((expectedVariance - variance) /
                                  (expectedVariance + 1e-10)) < 1e-3);
                LOOP3_ASSERT(di,
                             alpha,
                             expectedAlpha,
                             fabs((expectedAlpha - alpha) /
                                  (expectedAlpha + 1e-10)) < 1e-3);
                LOOP3_ASSERT(di,
                             beta,
                             expectedBeta,
                             fabs((expectedBeta - beta) /
                                  (expectedBeta + 1e-10)) < 1e-3);
                LOOP2_ASSERT(di,
                             xMeanRet,
                             0 == xMeanRet);
                LOOP2_ASSERT(di,
                             yMeanRet,
                             0 == yMeanRet);
                LOOP2_ASSERT(di,
                             varianceRet,
                             0 == varianceRet);
                LOOP2_ASSERT(di,
                             lineFitRet,
                             0 == lineFitRet);
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS AND BASIC ACCESSORS
        //   This test will verify that the primary manipulators are working as
        //   expected.  As basic accessors should be tested exactly the same
        //   way, these two tests have been united.  So we test that the basic
        //   accessors are working as expected also.
        //
        // Concerns:
        //: 1 All accessors return the expected values after 'bdlsta::LineFit'
        //:   loaded with a series of values.
        //:
        // Plan:
        //: 1 Using the table method, create 'bdlsta::LineFit', use
        //:   'bdlsta::LineFit::add' to load data, and check the values of
        //:   xMean, yMean, and variance.  (C-1)
        //
        // Testing:
        //   LineFit()
        //   add(double xValue, yValue)
        //   int count()
        //   double xMean()
        //   double yMean()
        //   double variance()
        //   void fit(double *alpha, double *beta)
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "PRIMARY MANIPULATORS AND BASIC ACCESSORS" << endl
                 << "========================================" << endl;

        static const struct {
            int    d_line;
            double d_xValues[7];
            int    d_count;
            double d_yValues[7];
            double d_xMean;
            double d_yMean;
            double d_variance;
            double d_alpha;
            double d_beta;
        } INPUT[] = {
            //LN  xVal1  xVal2  xVal3  xVal4  xVal5  xVal6  xVal7
            //--  -----  -----  -----  -----  -----  -----  -----
            //cnt yVal1  yVal2  yVal3  yVal4  yVal5  yVal6  yVal7
            //--- -----  -----  -----  -----  -----  -----  -----
            //xMean     yMean     variance  alpha    beta
            //--------  --------  --------  -------- --------
            { L_, 1e3  , 2e3   , 0.0  , 0.0  , 0.0  , 0.0  , 0.0,
              2 , 2.1e3, 4e3   , 0.0  , 0.0  , 0.0  , 0.0  , 0.0,
              1500.0   ,3050.0 , 5E5    , 200.0  , 1.9                      },
            { L_, 1.0  , 2.0   , 0.0  , 0.0  , 0.0  , 0.0  , 0.0,
              2 ,-1.1  ,-2.0   , 0.0  , 0.0  , 0.0  , 0.0  , 0.0,
              1.5      ,-1.55   , 0.5    , -0.2    , -0.9                   },
            { L_, 1e3  , 2e3   , 3e3  , 0.0  , 0.0  , 0.0  , 0.0,
              3 , 2.1e3, 4e3   , 6.1e3, 0.0  , 0.0  , 0.0  , 0.0,
              2000     ,4066.67, 1E6    , 66.6667, 2.0                      },
            { L_, 1.0  , 2.0   , 3.0  , 0.0  , 0.0  , 0.0  , 0.0,
              3 ,-1.1  ,-2.0   ,-3.2  , 0.0  , 0.0  , 0.0  , 0.0,
              2.0      ,-2.1    , 1.0    , 0.0     , -1.05                  },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  , 0.0  , 0.0  , 0.0,
              4 , 2.1e3, 4e3   , 6.1e3, 8e3  , 0.0  , 0.0  , 0.0,
              2500.0   ,5050.0 , 1.667E6, 100    , 1.98                     },
            { L_, 1.0  , 2.0   , 3.0  , 4e3  , 0.0  , 0.0  , 0.0,
              4 ,-1.1  ,-2.0   ,-3.2  ,-4.1e3, 0.0  , 0.0  , 0.0,
              1001.5   ,-1026.58, 3.996E6, -0.05002, -1.02499               },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  , 1.0  , 0.0  , 0.0,
              5 , 2.1e3, 4e3   , 6.1e3, 8e3  , 1.2  , 0.0  , 0.0,
              2000.2   ,4040.24, 2.499E6, 39.528 , 2.00016                  },
            { L_, 1.0  , 2.0   , 3.0  , 4e3  , 1e3  , 0.0  , 0.0,
              5 ,-1.1  ,-2.0   ,-3.2  ,-4.1e3,-1e3  , 0.0  , 0.0,
              1001.2   ,-1021.26, 2.997E6, 4.95999 , -1.02499               },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  , 1.0  , 2.0  , 0.0,
              6 , 2.1e3, 4e3   , 6.1e3, 8e3  , 1.2  , 2.1  , 0.0,
              1667.17  ,3367.22, 2.665E6, 23.9911, 2.00533                  },
            { L_, 1.0  , 2.0   , 3.0  , 4e3  , 1e3  , 2e3  , 0.0,
              6 ,-1.1  ,-2.0   ,-3.2  ,-4.1e3,-1e3  ,-2.2e3, 0.0,
              1167.67  ,-1217.72, 2.564E6, -9.12032, -1.03505               },
            { L_, 1.0  , 2.0   , 4.0  , 12.0 , 20.0 , 25.0 , 30.0,
              7 , 1.2  , 2.3   , 4.0  , 12.0 , 20.9 , 25.5 , 29.0,
              13.4286  ,13.5571, 137.952, 0.303555, 0.986969                },
            { L_,30.0  , 25.0  , 20.0 , 12.0 , 4.0  , 2.0  , 1.0,
              7 ,31.0  , 25.5  , 20.3 , 12.4 , 4.0  , 2.1  , 1.3,
              13.4286  ,13.8   , 137.952, 0.0650328, 1.02282                },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  , 1.0  , 2.0  , 3.0,
              7 , 2.1e3, 4e3   , 6.1e3, 8e3  , 1.2  , 2.1  , 3.0,
              1429.43  ,2886.61, 2.616E6, 16.6248, 2.00779                  },
            { L_, 1.0  , 2.0   , 3.0  , 4e3  , 1e3  , 2e3  , 3e3,
              7 ,-1.1  ,-2.0   ,-3.2  ,-4.1e3,-1e3  ,-2.2e3,-3e3,
              1429.43  ,-1472.33, 2.616E6, -9.13903, -1.02362               },
            { L_,-123.0,-55.0  , 4.0  , 0.0  , 100.0, 24.0 , 24.0,
              7 ,-123.0,-55.0  , 4.0  , 0.0  , 100.0, 24.0 , 24.0,
              -3.71429 ,-3.71429, 4870.9, 0.0, 1.0                          },
            { L_, 1.0  ,  2.0  , 3.0  , 4.0  , 5.0  ,  6.0 , 7.0 ,
              7 , 1.1  ,  2.0  , 3.2  , 4.4  , 5.0  ,  6.5 , 7.1 ,
                   4.0 ,4.18571, 4.6667, 0.071428, 1.02857                  },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  , 1e-3 , 2e-3 , 3e-3,
              7 , 3.2e3, 6e3   , 9.1e3, 12e3 , 3e-3 , 6e-3 , 9e-3,
             1428.57  ,4328.57 , 2.619E6, 36.3636, 3.00455                  },
            { L_, 1e-3 , 2e-3  , 3e-3 , 4e3  , 1e3  , 2e3  , 3e3 ,
              7 , 1e-3 , 2e-3  , 3e-3 , 4.2e3, 1e3  , 2.2e3, 3.1e3,
             1428.57  ,1500.0  , 2.619E6, -8.18182e-05, 1.05                },
        };

        const size_t NUM_DATA = sizeof INPUT / sizeof *INPUT;

        {
            for (size_t di = 0; di < NUM_DATA; ++di) {
                Obj lf;

                const int     LINE    = INPUT[di].d_line;
                const int     COUNT   = INPUT[di].d_count;
                const double *XVALUES = INPUT[di].d_xValues;
                const double *YVALUES = INPUT[di].d_yValues;
                for(int i = 0; i < COUNT; ++i) {
                    lf.add(XVALUES[i], YVALUES[i]);
                    if (veryVerbose) {
                        P_(LINE) P_(i) P_(XVALUES[i]) P(YVALUES[i]);
                    }
                }
                const int    expectedCount    = INPUT[di].d_count;
                const double expectedXMean    = INPUT[di].d_xMean;
                const double expectedYMean    = INPUT[di].d_yMean;
                const double expectedVariance = INPUT[di].d_variance;
                const double expectedAlpha    = INPUT[di].d_alpha;
                const double expectedBeta     = INPUT[di].d_beta;
                const int    count       = lf.count();
                const double xMeanRaw    = lf.xMean();
                const double yMeanRaw    = lf.yMean();
                const double varianceRaw = lf.variance();
                double       alpha = 0.0, beta = 0.0;
                lf.fit(&alpha, &beta);

                if (veryVerbose) {
                    T_ P_(LINE) P(expectedCount);
                    T_ P_(LINE) P_(xMeanRaw)    P(expectedXMean);
                    T_ P_(LINE) P_(yMeanRaw)    P(expectedYMean);
                    T_ P_(LINE) P_(varianceRaw) P(expectedVariance);
                }

                LOOP3_ASSERT(di,
                             count,
                             expectedCount,
                             expectedCount == count);
                LOOP3_ASSERT(di,
                             xMeanRaw,
                             expectedXMean,
                             fabs((expectedXMean - xMeanRaw) /
                                  (expectedXMean + 1e-10)) < 1e-3);
                LOOP3_ASSERT(di,
                             yMeanRaw,
                             expectedYMean,
                             fabs((expectedYMean - yMeanRaw) /
                                  (expectedYMean + 1e-10)) < 1e-3);
                LOOP3_ASSERT(di,
                             varianceRaw,
                             expectedVariance,
                             fabs((expectedVariance - varianceRaw) /
                                  (expectedVariance + 1e-10)) < 1e-3);
                LOOP3_ASSERT(di,
                             alpha,
                             expectedAlpha,
                             fabs((expectedAlpha - alpha) /
                                  (expectedAlpha + 1e-10)) < 1e-3);
                LOOP3_ASSERT(di,
                             beta,
                             expectedBeta,
                             fabs((expectedBeta - beta) /
                                  (expectedBeta + 1e-10)) < 1e-3);
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

        double          inputX[] = { 1.0, 2.0, 4.0, 5.0 };
        double          inputY[] = { 1.0, 2.0, 4.0, 4.5 };
        bdlsta::LineFit lineFit;

        for(int i = 0; i < 4; ++i) {
            lineFit.add(inputX[i], inputY[i]);
            if (veryVerbose) {
                P_(i) P_(inputX[i]) P(inputY[i]);
            }
        }

        double alpha = 0.0, beta = 0.0;
        ASSERT(4 == lineFit.count());
        ASSERT(3.0 == lineFit.xMean());
        ASSERT(fabs(2.875 - lineFit.yMean()) < 1e-3);
        ASSERT(fabs(3.33333  - lineFit.variance()) < 1e-3);
        ASSERT(0 == lineFit.fitIfValid(&alpha, &beta));
        ASSERT(fabs(0.175 - alpha)     < 1e-3);
        ASSERT(fabs(0.9 - beta )     < 1e-3);
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
