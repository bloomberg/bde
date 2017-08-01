// bdlstat_moment.t.cpp                                               -*-C++-*-
#include <bdlstat_moment.h>

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
// [ 2] getCount()
// [ 2] getMean()
// [ 2] getMeanRaw()
// [ 2] getVariance()
// [ 2] getVarianceRaw()
// [ 2] getSkew()
// [ 2] getSkewRaw()
// [ 2] getKurtosis()
// [ 2] getKurtosisRaw()
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

typedef bdlstat::Moment<bdlstat::MomentLevel::e_M1> ObjM;
typedef bdlstat::Moment<bdlstat::MomentLevel::e_M2> ObjV;
typedef bdlstat::Moment<bdlstat::MomentLevel::e_M3> ObjS;
typedef bdlstat::Moment<bdlstat::MomentLevel::e_M4> ObjK;

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
    double input[] = {1.0, 2.0, 4.0, 5.0};

    bdlstat::Moment<bdlstat::MomentLevel::e_M3> m3;
//..
// Then, we invoke the 'add' routine to accumulate the data:
//..
    for(int i = 0; i < 4; ++i) {
        m3.add(input[i]);
    }
//..
// Finally, we assert that the mean, variance, and skew are what we expect:
//..
    ASSERT(4   == m3.getCount());
    ASSERT(3.0 == m3.getMean());
    ASSERT(1e-5 > fabs(3.33333 - m3.getVariance()));
    ASSERT(1e-5 > fabs(0.0     - m3.getSkew()));
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING EDGE CASES
        //
        // Concerns:
        //: 1 'getMean' returns 'Nan' when no data is fed.
        //:
        //: 2 'getVariance' returns 'Nan' with less than 2 data values.
        //:
        //: 3 'getSkew' returns 'Nan' with less than 3 data values.
        //:
        //: 4 'getKurtosis' returns 'Nan' with less than 4 data values.
        //
        // Plan:
        //: 1 Verify the 'getMean' with no data returns 'Nan'.  (C-1)
        //:
        //: 2 Verify the 'getVariance' with 1 data value returns 'Nan'. (C-2)
        //:
        //: 3 Verify the 'getSkew' with 2 data values returns 'Nan'. (C-2)
        //:
        //: 4 Verify the 'getKurtosis' with 3 data values returns 'Nan'. (C-4)
        //
        // Testing:
        //   EDGE CASES
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING EDGE CASES" << endl
                          << "==================" << endl;

        {
            bdlstat::Moment<bdlstat::MomentLevel::e_M1> m1;
            ASSERT(0 == m1.getCount());
            ASSERT(bdlb::Float::isNan(m1.getMean()));
        }

        {
            bdlstat::Moment<bdlstat::MomentLevel::e_M2> m2;
            m2.add(1.0);
            ASSERT(1 == m2.getCount());
            ASSERT(bdlb::Float::isNan(m2.getVariance()));
        }

        {
            bdlstat::Moment<bdlstat::MomentLevel::e_M3> m3;
            m3.add(1.0);
            m3.add(2.0);
            ASSERT(2 == m3.getCount());
            ASSERT(bdlb::Float::isNan(m3.getSkew()));
        }

        {
            bdlstat::Moment<bdlstat::MomentLevel::e_M4> m4;
            m4.add(1.0);
            m4.add(2.0);
            m4.add(4.0);
            ASSERT(3 == m4.getCount());
            ASSERT(bdlb::Float::isNan(m4.getKurtosis()));
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
        //: 1 All accessors return the expected values after 'bdlstat::Moment'
        //:   loaded with a series of values.
        //:
        //: 2 Different specializations produce the same set of values.
        //
        // Plan:
        //: 1 Using the table method, create
        //    'bdlstat::Moment<MomentLevel::e_M4>', use
        //:   'bdlstat::Moment<MomentLevel::e_M4>::add' to load data, and check
        //:   the values of mean, variance, skew, and kurtosis.  (C-1)
        //:
        //: 2 Verify that we get the same values from the 'raw' methods.  (C-1)
        //:
        //: 3 Load the same set of data into
        //:   'bdlstat::Moment<MomentLevel::e_M3>',
        //:   'bdlstat::Moment<MomentLevel::e_M2>',
        //:   'bdlstat::Moment<MomentLevel::e_M1>', and verify that we get the
        //:   same values.  (C-2)
        //
        // Testing:
        //   Moment()
        //   add(double value)
        //   getCount()
        //   getMean()
        //   getMeanRaw()
        //   getVariance()
        //   getVarianceRaw()
        //   getSkew()
        //   getSkewRaw()
        //   getKurtosis()
        //   getKurtosisRaw()
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "PRIMARY MANIPULATORS AND BASIC ACCESSORS" << endl
                 << "========================================" << endl;

        static const struct {
            int    d_line;
            double d_values[7];
            double d_mean;
            double d_variance;
            double d_skew;
            double d_kurtosis;
        } INPUT[] = {
            //LN  val1   val2   val3   val4   val5   val6   val7
            //--  -----  -----  -----  -----  -----  -----  -----
            //mean      variance  skew      kurtosis
            //--------  --------  --------  --------
            { L_, 1.0  , 2.0   , 4.0  , 12.0 , 20.0 , 25.0 , 30.0,
              13.4286  ,137.952, 0.30677  , -1.86809                        },
            { L_,30.0  , 25.0  , 20.0 , 12.0 , 4.0  , 2.0  , 1.0,
              13.4286  ,137.952, 0.30677  , -1.86809                        },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  , 1.0  , 2.0  , 3.0,
              1429.43  ,2.616E6, 0.674989 , -1.14935                        },
            { L_, 1.0  , 2.0   , 3.0  , 4e3  , 1e3  , 2e3  , 3e3,
              1429.43  ,2.616E6, 0.674989 , -1.14935                        },
            { L_,-123.0,-55.0  , 4.0  , 0.0  , 100.0, 24.0 , 24.0,
              -3.71429 ,4870.9 ,-0.46229  , 1.01756                         },
            { L_, 1.0  ,  2.0  , 3.0  , 4.0  , 5.0  ,  6.0 , 7.0 ,
                   4.0 ,4.6667  ,0.0       , -1.2                            },
            { L_, 1e3  , 2e3   , 3e3  , 4e3  , 1e-3 , 2e-3 , 3e-3,
             1428.57  ,2.619E6, 0.67409   , -1.15101                        },
            { L_, 1e-3 , 2e-3  , 3e-3 , 4e3  , 1e3  , 2e3  , 3e3 ,
             1428.57  ,2.619E6, 0.67409   , -1.15101                        },
        };

        const size_t NUM_DATA = sizeof INPUT / sizeof *INPUT;

        if (verbose) cout << "Testing M4\n";
        {
            for (size_t di = 0; di < NUM_DATA; ++di) {
                ObjK m4;

                const int     LINE   = INPUT[di].d_line;
                const double *VALUES = INPUT[di].d_values;
                for(int i = 0; i < 7; ++i) {
                    m4.add(VALUES[i]);
                    if (veryVerbose) {
                        P_(LINE) P_(i) P(VALUES[i]);
                    }
                }
                const double expectedMean     = INPUT[di].d_mean;
                const double expectedVariance = INPUT[di].d_variance;
                const double expectedSkew     = INPUT[di].d_skew;
                const double expectedKurtosis = INPUT[di].d_kurtosis;
                const int    count       = m4.getCount();
                const double mean        = m4.getMean();
                const double meanRaw     = m4.getMeanRaw();
                const double variance    = m4.getVariance();
                const double varianceRaw = m4.getVarianceRaw();
                const double skew        = m4.getSkew();
                const double skewRaw     = m4.getSkewRaw();
                const double kurtosis    = m4.getKurtosis();
                const double kurtosisRaw = m4.getKurtosisRaw();

                if (veryVerbose) {
                    T_ P_(LINE) P_(mean) P_(meanRaw) P(expectedMean);
                    T_ P_(variance) P_(varianceRaw) P(expectedVariance);
                    T_ P_(skew) P_(skewRaw) P(expectedSkew);
                    T_ P_(kurtosis) P_(kurtosisRaw) P(expectedKurtosis);
                }

                LOOP3_ASSERT(di,
                             count,
                             7,
                             7 == count);
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
            }
        }

        if (verbose) cout << "Testing M3\n";
        {
            for (size_t di = 0; di < NUM_DATA; ++di) {
                ObjS m3;

                const int     LINE   = INPUT[di].d_line;
                const double *VALUES = INPUT[di].d_values;
                for(int i = 0; i < 7; ++i) {
                    m3.add(VALUES[i]);
                    if (veryVerbose) {
                        P_(LINE) P_(i) P(VALUES[i]);
                    }
                }
                const double expectedMean     = INPUT[di].d_mean;
                const double expectedVariance = INPUT[di].d_variance;
                const double expectedSkew     = INPUT[di].d_skew;
                const int    count       = m3.getCount();
                const double mean        = m3.getMean();
                const double meanRaw     = m3.getMeanRaw();
                const double variance    = m3.getVariance();
                const double varianceRaw = m3.getVarianceRaw();
                const double skew        = m3.getSkew();
                const double skewRaw     = m3.getSkewRaw();

                if (veryVerbose) {
                    T_ P_(LINE) P_(mean) P_(meanRaw) P(expectedMean);
                    T_ P_(variance) P_(varianceRaw) P(expectedVariance);
                    T_ P_(skew) P_(skewRaw) P(expectedSkew);
                }

                LOOP3_ASSERT(di,
                             count,
                             7,
                             7 == count);
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
            }
        }

        if (verbose) cout << "Testing M2\n";
        {
            for (size_t di = 0; di < NUM_DATA; ++di) {
                ObjV m2;

                const int     LINE   = INPUT[di].d_line;
                const double *VALUES = INPUT[di].d_values;
                for(int i = 0; i < 7; ++i) {
                    m2.add(VALUES[i]);
                    if (veryVerbose) {
                        P_(LINE) P_(i) P(VALUES[i]);
                    }
                }
                const double expectedMean     = INPUT[di].d_mean;
                const double expectedVariance = INPUT[di].d_variance;
                const int    count       = m2.getCount();
                const double mean        = m2.getMean();
                const double meanRaw     = m2.getMeanRaw();
                const double variance    = m2.getVariance();
                const double varianceRaw = m2.getVarianceRaw();

                if (veryVerbose) {
                    T_ P_(LINE) P_(mean) P_(meanRaw) P(expectedMean);
                    T_ P_(variance) P_(varianceRaw) P(expectedVariance);
                }

                LOOP3_ASSERT(di,
                             count,
                             7,
                             7 == count);
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
            }
        }

        if (verbose) cout << "Testing M1\n";
        {
            for (size_t di = 0; di < NUM_DATA; ++di) {
                ObjM m1;

                const int     LINE   = INPUT[di].d_line;
                const double *VALUES = INPUT[di].d_values;
                for(int i = 0; i < 7; ++i) {
                    m1.add(VALUES[i]);
                    if (veryVerbose) {
                        P_(LINE) P_(i) P(VALUES[i]);
                    }
                }
                const double expectedMean     = INPUT[di].d_mean;
                const int    count       = m1.getCount();
                const double mean        = m1.getMean();
                const double meanRaw     = m1.getMeanRaw();

                if (veryVerbose) {
                    T_ P_(LINE) P_(mean) P_(meanRaw) P(expectedMean);
                }

                LOOP3_ASSERT(di,
                             count,
                             7,
                             7 == count);
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

        double input[] = {1.0, 2.0, 4.0, 5.0};

        bdlstat::Moment<bdlstat::MomentLevel::e_M1> m1;
        for(int i = 0; i < 4; ++i) {
            m1.add(input[i]);
            if (veryVerbose) {
                P_(i) P(input[i]);
            }
        }
        ASSERT(4 == m1.getCount());
        ASSERT(3.0 == m1.getMean());
        // The below does not compile, as it should
        //ASSERT(fabs(3.33333  - m2.getVariance()) < 1e-5);

        bdlstat::Moment<bdlstat::MomentLevel::e_M2> m2;
        for(int i = 0; i < 4; ++i) {
            m2.add(input[i]);
            if (veryVerbose) {
                P_(i) P(input[i]);
            }
        }
        ASSERT(4 == m2.getCount());
        ASSERT(3.0 == m2.getMean());
        ASSERT(fabs(3.33333  - m2.getVariance()) < 1e-5);

        bdlstat::Moment<bdlstat::MomentLevel::e_M3> m3;
        for(int i = 0; i < 4; ++i) {
            m3.add(input[i]);
            if (veryVerbose) {
                P_(i) P(input[i]);
            }
        }
        ASSERT(4 == m3.getCount());
        ASSERT(3.0 == m3.getMean());
        ASSERT(fabs(3.33333 - m3.getVariance()) < 1e-5);
        ASSERT(fabs(0.0     - m3.getSkew())     < 1e-5);
        // The below does not compile, as it should
        //ASSERT(fabs(644.185  - m3.getKurtosis()) < 1e-3);

        bdlstat::Moment<bdlstat::MomentLevel::e_M4> m4;
        for(int i = 0; i < 4; ++i) {
            m4.add(input[i]);
            if (veryVerbose) {
                P_(i) P(input[i]);
            }
        }
        ASSERT(4 == m4.getCount());
        ASSERT(3.0 == m4.getMean());
        ASSERT(fabs(3.33333 - m4.getVariance()) < 1e-5);
        ASSERT(fabs(0.0     - m4.getSkew())     < 1e-5);
        ASSERT(fabs(-3.3    - m4.getKurtosis()) < 1e-3);
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
// Copyright 2016 Bloomberg Finance L.P.
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
