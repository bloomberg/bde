// bdlstat_linefit.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLSTAT_LINEFIT
#define INCLUDED_BDLSTAT_LINEFIT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

// BDE_VERIFY pragma: -LL01 // Link is just too long
//@PURPOSE: Online algorithm for computing the least squares regression line.
//
//@CLASSES:
//  bdlstat::LineFit: online calculation of least squares regression line
//
//@AUTHOR: Ofer Imanuel (oimanuel@bloomberg.net)
//
//@DESCRIPTION: This component provides a mechanism, 'bdlstat::LineFit', that
// provides online calculation of the least squares line fit.  Online
// algorithms process the data in one pass, while maintaining accuracy.  The
// online algorithm used is developed in the implementation notes.  It is
// similar to Welford online algorithm for computing variance.  The formulae
// for line fit are taken from:
// https://en.wikipedia.org/wiki/Simple_linear_regression#Fitting_the_regression_line
//
// Note that the behavior is undefined if there are less than 2 data points, or
// if all the X's (dependent variable) are the same.
// BDE_VERIFY pragma: +LL01
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Calculating line fit, variance, and mean
///- - - - - - - - - - - - - - - -
// This example shows how to accumulate a set of values, and calculate the
// line fit parameters, variance, and mean.
//
// First, we create example input and instantiate the appropriate mechanism:
//..
//  double inputX[] = {1.0, 2.0, 4.0, 5.0};
//  double inputY[] = {1.0, 2.0, 4.0, 4.5};
//  bdlstat::LineFit lineFit;
//..
// Then, we invoke the 'add' routine to accumulate the data:
//..
//  for(int i = 0; i < 4; ++i) {
//      lineFit.add(inputX[i], inputY[i]);
//  }
//..
// Finally, we assert that the alpha, beta, variance, and mean are what we
// expect:
//..
//  double alpha, beta;
//  ASSERT(4    == lineFit.count());
//  ASSERT(3.0  == lineFit.xMean());
//  ASSERT(1e-3 >  fabs(2.875    - lineFit.yMean()));
//  ASSERT(1e-3 >  fabs(3.33333  - lineFit.variance()));
//  ASSERT(0    == lineFit.getLineFit(&alpha, &beta));
//  ASSERT(1e-3 >  fabs(0.175 - alpha));
//  ASSERT(1e-3 >  fabs(0.9   - beta ));
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_CMATH
#include <bsl_cmath.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bdlstat {

                            // =============
                            // class LineFit
                            // =============

class LineFit {
    // This class provides efficient an online algorithm for calculating linear
    // square line fit.  The class also calculates the mean for the X's and
    // Y's, and variance for the X's, all byproducts of calculating the line
    // fit.  The online algorithm is detailed in the implementation notes.
  private:
    // DATA
    int    d_count; // Number of data points.
    double d_xMean; // Mean of X's.
    double d_xSum;  // Sum of X's.
    double d_ySum;  // Sum of Y's.
    double d_M2;    // 2nd moment
    double d_xySum; // Sum of Xi*Yi

  public:
    // CREATORS
    LineFit();
        // Create an empty 'LineFit' object.

    // MANIPULATORS
    void add(double xValue, double yValue);
        // Add the specified 'xValue', 'yValue' point to the data set.

    // ACCESSORS
    int count() const;
        // Returns the number of elements in the data set.

    int getLineFit(double *alpha, double *beta) const;
        // Calculate line fit coefficients T=A+B*X, and populate the specified
        // 'alpha' (intercept) and 'beta' (slope).  Return 0 on success, and -1
        // otherwise.  Calculation fails if '2 > count' or all X's are
        // identical.

    int varianceIfValid(double *result) const;
        // Load into the specified 'result, the variance of the data set X's.
        // Return 0 for success, or -1 if '2 > count'.

    double variance() const;
        // Return variance of the data set X's.  The behavior is undefined
        // unless '2 <= count'.

    int xMeanIfValid(double *result) const;
        // Load into the specified 'result, the mean of the data set X's.
        // Return 0 for success, or -1 if '1 > count'.

    double xMean() const;
        // Return mean of the data set Y's.  The behavior is undefined unless
        // '1 <= count'.

    int yMeanIfValid(double *result) const;
        // Load into the specified 'result, the mean of the data set X's.
        // Return 0 for success, or -1 if '1 > count'.

    double yMean() const;
        // Return mean of the data set Y's.  The behavior is undefined unless
        // '1 <= count'.
};

// ============================================================================
//                               INLINE DEFINITIONS
// ============================================================================

                        // ----------------------
                        // class bdlstat::LineFit
                        // ----------------------

// CREATORS
LineFit::LineFit()
: d_count(0)
, d_xMean(0.0)
, d_xSum(0.0)
, d_ySum(0.0)
, d_M2(0.0)
, d_xySum(0.0)
{
}

// MANIPULATORS
inline
void LineFit::add(double xValue, double yValue)
{
    const double delta = xValue - d_xMean;
    ++d_count;
    d_xSum += xValue;
    d_ySum += yValue;
    d_xMean = d_xSum / static_cast<double>(d_count);
    const double delta2 = xValue - d_xMean;
    d_M2 += delta * delta2;
    d_xySum += xValue * yValue;
}

// ACCESSORS
inline
int LineFit::count() const
{
    return d_count;
}

inline
int LineFit::getLineFit(double *alpha, double *beta) const
{
    if (2 > d_count || 0.0 == d_M2) {
        return -1;                                                    // RETURN
    }
    const double n = static_cast<double>(d_count);
    double tmpBeta = (d_xySum - d_xSum * d_ySum / n) / d_M2;
    *beta = tmpBeta;
    *alpha = (d_ySum - d_xSum * tmpBeta) / n;
    return 0;
}

inline
int LineFit::varianceIfValid(double *result) const
{
    if (2 > d_count) {
        return -1;                                                    // RETURN
    }
    *result = variance();
    return 0;
}

inline
double LineFit::variance() const
{
    BSLS_ASSERT_SAFE(2 <= d_data.d_count);
    return d_M2 / (d_count - 1);
}

inline
int LineFit::xMeanIfValid(double *result) const
{
    if (1 > d_count) {
        return -1;                                                    // RETURN
    }
    *result = xMean();
    return 0;
}

inline
double LineFit::xMean() const
{
    BSLS_ASSERT_SAFE(1 <= d_data.d_count);
    return d_xSum / static_cast<double>(d_count);
}

inline
int LineFit::yMeanIfValid(double *result) const
{
    if (1 > d_count) {
        return -1;                                                    // RETURN
    }
    *result = yMean();
    return 0;
}

inline
double LineFit::yMean() const
{
    BSLS_ASSERT_SAFE(1 <= d_data.d_count);
    return d_ySum / static_cast<double>(d_count);
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
