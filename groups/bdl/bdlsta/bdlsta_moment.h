// bdlsta_moment.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLSTA_MOMENT
#define INCLUDED_BDLSTA_MOMENT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

// BDE_VERIFY pragma: -LL01 // Link is just too long

//@PURPOSE: Online algorithm for mean, variance, skew, and kurtosis.
//
//@CLASSES:
//  bdlsta::Moment: online calculation of mean, variance, skew, and kurtosis
//
//@DESCRIPTION: This component provides a mechanism, 'bdlsta::Moment', that
// provides online calculation of basic statistics: mean, variance, skew, and
// kurtosis while maintaining accuracy.  Online algorithms process the data in
// one pass, while keeping good accuracy.  The online algorithms used are
// Welford for variance, and the stable skew and kurtosis algorithms taken
// from:
// https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Higher-order_statistics
//
// The implementation uses template specialization so the user can choose the
// statistics necessary, and not calculate or allocate memory for those
// statistics that are not needed.
//
// The template parameter is a value from the provided enum and having the
// following interpretation:
//..
//  M1 - mean
//  M2 - variance+mean
//  M3 - skew+variance+mean
//  M4 - kurtosis+skew+variance+mean
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Calculating skew, variance, and mean
///- - - - - - - - - - - - - - - -
// This example shows how to accumulate a set of values, and calculate the
// skew, variance, and kurtosis.
//
// First, we create example input and instantiate the appropriate mechanism:
//..
//  double input[] = { 1.0, 2.0, 4.0, 5.0 };
//
//  bdlsta::Moment<bdlsta::MomentLevel::e_M3> m3;
//..
// Then, we invoke the 'add' routine to accumulate the data:
//..
//  for(int i = 0; i < 4; ++i) {
//      m3.add(input[i]);
//  }
//..
// Finally, we assert that the mean, variance, and skew are what we expect:
//..
//  ASSERT(4   == m3.count());
//  ASSERT(3.0 == m3.mean());
//  ASSERT(1e-5 > fabs(3.33333 - m3.variance()));
//  ASSERT(1e-5 > fabs(0.0     - m3.skew()));
//..

// BDE_VERIFY pragma: +LL01

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
namespace bdlsta {

struct MomentLevel {
    // TYPES
    enum Enum {
        // Enumeration of moment level of data desired.

        e_M1, // mean
        e_M2, // variance+mean
        e_M3, // skew+variance+mean
        e_M4  // kurtosis+skew+variance+mean
    };
};

                      // ==========================
                      // private struct Moment_Data
                      // ==========================

template <MomentLevel::Enum ML>
struct Moment_Data;

template<>
struct Moment_Data<MomentLevel::e_M1> {
    // Data members for Mean only.

    // PUBLIC DATA
    int    d_count; // Number of entries.
    double d_sum;   // Sum of entries.

    // CREATORS
    Moment_Data();
        // Constructor initializes all members to zero.
};

template<>
struct Moment_Data<MomentLevel::e_M2> {
    // Data members for Variance and below.

    // PUBLIC DATA
    int    d_count; // Number of entries.
    double d_sum;   // Sum of entries.
    double d_mean;  // Mean of entries.
    double d_M2;    // 2nd moment, for variance.

    // CREATORS
    Moment_Data();
        // Constructor initializes all members to zero.
};

template<>
struct Moment_Data<MomentLevel::e_M3> {
    // Data members for Skew and below

    // PUBLIC DATA
    int    d_count; // Number of entries.
    double d_sum;   // Sum of entries.
    double d_mean;  // Mean of entries.
    double d_M2;    // 2nd moment, for variance.
    double d_M3;    // 3rd moment, for skew

    // CREATORS
    Moment_Data();
        // Constructor initializes all members to zero.
};

template<>
struct Moment_Data<MomentLevel::e_M4> {
    // Data members for Kurtosis and below

    // PUBLIC DATA
    int    d_count; // Number of entries.
    double d_sum;   // Sum of entries.
    double d_mean;  // Mean of entries.
    double d_M2;    // 2nd moment, for variance.
    double d_M3;    // 3rd moment, for skew
    double d_M4;    // 4th moment, for kurtosis

    // CREATORS
    Moment_Data();
        // Constructor initializes all members to zero.
};

                            // ============
                            // class Moment
                            // ============

// BDE_VERIFY pragma: -LL01 // Link is just too long

template <MomentLevel::Enum ML>
class Moment {
    // This class provides efficient and accurate online algorithms for
    // calculating mean, variance, skew, and kurtosis.  The class provides
    // template specializations, so that no unnecessary data members will be
    // kept or unnecessary calculations done.  The online algorithms used are
    // Welford for variance, and the stable M3 and M4 are taken from:
    // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Higher-order_statistics
    //
    // The formula for sample skewness is taken from:
    // http://www.macroption.com/skewness-formula/
    //
    // The formula for sample excess kurtosis is taken from:
    // http://www.macroption.com/kurtosis-formula/

    // BDE_VERIFY pragma: +LL01

  private:
    // PRIVATE TYPES
    typedef struct Moment_Data<ML> Moment_Data_t;

    // DATA
    Moment_Data_t d_data;

  public:
    // CONSTANTS
    enum {
        e_SUCCESS         = 0,
        e_INADEQUATE_DATA = -1
    };

    // MANIPULATORS
    void add(double value);
        // Add the specified 'value' to the data set.

    // ACCESSORS
    int count() const;
        // Returns the number of elements in the data set.

    double kurtosis() const;
        // Return the kurtosis of the data set.  The behavior is undefined
        // unless '4 <= count' and the variance is not zero.

    int kurtosisIfValid(double *result) const;
        // Load into the specified 'result', the kurtosis of the data set.
        // Return 0 on success, and a non-zero value otherwise.  Specifically,
        // 'e_INADEQUATE_DATA' is returned if '4 > count' or the variance is
        // zero.

    double mean() const;
        // Return the mean of the data set.  The behavior is undefined unless
        // '1 <= count'.

    int meanIfValid(double *result) const;
        // Load into the specified 'result', the mean of the data set.  Return
        // 0 on success, and a non-zero value otherwise.  Specifically,
        // 'e_INADEQUATE_DATA' is returned if '1 > count'.

    double skew() const;
        // Return skew of the data set.  The behavior is undefined unless
        // '3 <= count' or the variance is zero.

    int skewIfValid(double *result) const;
        // Load into the specified 'result', the skew of the data set.  Return
        // 0 on success, and a non-zero value otherwise.  Specifically,
        // 'e_INADEQUATE_DATA' is returned if '3 > count' or the variance is
        // zero.

    double variance() const;
        // Return the variance of the data set.  The behavior is undefined
        // unless '2 <= count'.

    int varianceIfValid(double *result) const;
        // Load into the specified 'result', the variance of the data set.
        // Return 0 on success, and a non-zero value otherwise.  Specifically,
        // 'e_INADEQUATE_DATA' is returned if '2 > count'.
};

// ============================================================================
//                               INLINE DEFINITIONS
// ============================================================================

                        // --------------------------
                        // struct bdlsta::Moment_Data
                        // --------------------------

// CREATORS
Moment_Data<MomentLevel::e_M1>::Moment_Data()
: d_count(0)
, d_sum(0.0)
{
}

Moment_Data<MomentLevel::e_M2>::Moment_Data()
: d_count(0)
, d_sum(0.0)
, d_mean(0.0)
, d_M2(0.0)
{
}

Moment_Data<MomentLevel::e_M3>::Moment_Data()
: d_count(0)
, d_sum(0.0)
, d_mean(0.0)
, d_M2(0.0)
, d_M3(0.0)
{
}

Moment_Data<MomentLevel::e_M4>::Moment_Data()
: d_count(0)
, d_sum(0.0)
, d_mean(0.0)
, d_M2(0.0)
, d_M3(0.0)
, d_M4(0.0)
{
}

                        // --------------------
                        // class bdlsta::Moment
                        // --------------------

// MANIPULATORS
template<>
inline
void Moment<MomentLevel::e_M1>::add(double value)
{
    ++d_data.d_count;
    d_data.d_sum += value;
}

template<>
inline
void Moment<MomentLevel::e_M2>::add(double value)
{
    // Modified Welford algorithm for variance.
    const double delta = value - d_data.d_mean;
    d_data.d_sum += value;
    ++d_data.d_count;
    d_data.d_mean = d_data.d_sum / static_cast<double>(d_data.d_count);
    const double delta2 = value - d_data.d_mean;
    d_data.d_M2 += delta * delta2;
}

template<>
inline
void Moment<MomentLevel::e_M3>::add(double value)
{
    // Modified Welford algorithm for variance, and similar algorithm for skew.
    const double delta = value - d_data.d_mean;
    const double nm1 = d_data.d_count;
    d_data.d_sum += value;
    ++d_data.d_count;
    const double n = d_data.d_count;
    const double deltaN = delta / n;
    d_data.d_mean = d_data.d_sum / n;
    const double term1 = delta * deltaN * nm1;
    d_data.d_M3 += term1 * deltaN * (n - 2.0) - 3.0 * deltaN * d_data.d_M2;
    d_data.d_M2 += term1;
}

template<>
inline
void Moment<MomentLevel::e_M4>::add(double value)
{
    // Modified Welford algorithm for variance, and similar algorithms for skew
    // and kurtosis.
    const double delta = value - d_data.d_mean;
    const double nm1 = d_data.d_count;
    d_data.d_sum += value;
    ++d_data.d_count;
    const double n = d_data.d_count;
    const double n2 = n * n;
    const double deltaN = delta / n;
    d_data.d_mean = d_data.d_sum / n;
    const double term1 = delta * deltaN * nm1;
    const double deltaN2 = deltaN * deltaN;
    d_data.d_M4 += term1 * deltaN2 * (n2 - 3.0 * n + 3.0) +
                   6 * deltaN2 * d_data.d_M2 - 4.0 * deltaN * d_data.d_M3;
    d_data.d_M3 += term1 * deltaN * (n - 2.0) - 3.0 * deltaN * d_data.d_M2;
    d_data.d_M2 += term1;
}

// ACCESSORS
template <MomentLevel::Enum ML>
inline
int Moment<ML>::count() const
{
    return d_data.d_count;
}

template<>
inline
double Moment<MomentLevel::e_M4>::kurtosis() const
{
    BSLS_ASSERT_SAFE(4 <= d_data.d_count && 0.0 != d_data.d_M2);

    const double n    = static_cast<double>(d_data.d_count);
    const double n1   = (n - 1.0);
    const double n2n3 = (n - 2.0) * (n - 3.0);
    return n * (n + 1.0) * n1 / n2n3 * d_data.d_M4 / d_data.d_M2 / d_data.d_M2
           - 3.0 * n1 * n1 / n2n3;
}

template<>
inline
int Moment<MomentLevel::e_M4>::kurtosisIfValid(double *result) const
{
    if (4 > d_data.d_count || 0.0 == d_data.d_M2) {
        return e_INADEQUATE_DATA;                                     // RETURN
    }
    *result = kurtosis();
    return 0;
}

template <MomentLevel::Enum ML>
inline
double Moment<ML>::mean() const
{
    BSLS_ASSERT_SAFE(1 <= d_data.d_count);

    return d_data.d_sum / static_cast<double>(d_data.d_count);
}

template <MomentLevel::Enum ML>
inline
int Moment<ML>::meanIfValid(double *result) const
{
    if (1 > d_data.d_count) {
        return e_INADEQUATE_DATA;                                     // RETURN
    }
    *result = mean();
    return 0;
}

template <MomentLevel::Enum ML>
inline
double Moment<ML>::skew() const
{
    BSLS_ASSERT_SAFE(3 <= d_data.d_count && 0.0 != d_data.d_M2);

    const double n = static_cast<double>(d_data.d_count);
    return bsl::sqrt(n - 1.0) * n / (n- 2.0) * d_data.d_M3
                                                  / bsl::pow(d_data.d_M2, 1.5);
}

template <MomentLevel::Enum ML>
inline int Moment<ML>::skewIfValid(double *result) const
{
    if (3 > d_data.d_count || 0.0 == d_data.d_M2) {
        return e_INADEQUATE_DATA;                                     // RETURN
    }
    *result = skew();
    return 0;
}

template <MomentLevel::Enum ML>
inline
double Moment<ML>::variance() const
{
    BSLS_ASSERT_SAFE(2 <= d_data.d_count);

    return d_data.d_M2 / (d_data.d_count - 1);
}

template <MomentLevel::Enum ML>
inline
int Moment<ML>::varianceIfValid(double *result) const
{
    if (2 > d_data.d_count) {
        return e_INADEQUATE_DATA;                                     // RETURN
    }
    *result = variance();
    return 0;
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
