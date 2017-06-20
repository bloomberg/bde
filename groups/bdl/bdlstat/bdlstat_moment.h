// bdlstat_moment.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLSTAT_MOMENT
#define INCLUDED_BDLSTAT_MOMENT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functions for generating schedules of dates.
//
//@CLASSES:
//  bblb::ScheduleGenerationUtil: namespace for schedule generation functions
//
//@SEE ALSO:
//
//@AUTHOR: Rahul Kumar (rkumar85@bloomberg.net)
//
//@DESCRIPTION: This component provides a 'struct',
// 'bblb::ScheduleGenerationUtil', that serves as a namespace for functions
// that generate a schedule; a set of dates limited to within a closed-interval
// date-range, represented by the specified 'earliest' and 'latest' dates.
// Typically, a schedule generation method can be defined by an algorithm
// specific to the method, an 'example' date, a closed-interval represented by
// an 'earliest' and a 'latest' date, and any other information required to
// determine the interval between successive dates in a schedule (for example,
// the number of days or months between successive dates).  The process of
// computing the dates within a schedule is exemplified using the following
// diagram:
//..
// ___________|__________________|___________________|________
//        'example'          'earliest'          'latest'
//            .___.___.___.___.___|___|___|___|___|___.
//                                ^   ^   ^   ^   ^
// 'interval':.___.               |   |   |   |   |
// 'schedule':                   [d0, d1, d2, d3, d4]
//..
// The schedule generated in the above diagram is '[d0, d1, d2, d3, d4]'.
// Notice that the 'example' date does not have to reside within the closed
// interval (the 'example' date is before the 'earliest' date in this diagram).
//
// More formally, the resulting 'schedule' is the subset of the infinite series
// of dates, defined by all dates separated by an integral multiple of
// intervals from the 'example' date, that reside within the closed-interval
// specified by 'earliest' and 'latest'.
//
// The following section provides a synopsis of the main functions provided in
// this component:
//..
//  'generateFromDayInterval'               Generate a schedule having an
//                                          interval of a fixed number of days.
//
//  'generateFromDayOfMonth'                Generate a schedule having an
//                                          interval of a fixed number of
//                                          months, with each date in the
//                                          schedule occuring on a specific day
//                                          of the month.
//
//  'generateFromBusinessDayOfMonth'        Generate a schedule having an
//                                          interval of a fixed number of
//                                          months, with each date in the
//                                          schedule occuring on a specific
//                                          business day of the month.
//
//  'generateFromDayOfWeekAfterDayOfMonth'  Generate a schedule having an
//                                          interval of a fixed number of
//                                          months, with each date in the
//                                          schedule occuring on a specific day
//                                          of the week on or after a specific
//                                          day of the month.
//
//  'generateFromDayOfWeekInMonth'          Generate a schedule having an
//                                          interval of a fixed number of
//                                          months, with each date in the
//                                          schedule occuring on a specific day
//                                          of the week in a specific week of
//                                          the month.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Generating a Schedule
///- - - - - - - - - - - - - - - -
// Suppose that we want to determine the sequence of dates that are:
//   * integral multiples of 9 months away from July 2007,
//   * on the 23rd day of the month,
//   * and within the closed interval '[02/01/2012, 02/28/2015]'.
//
// First, we define the inputs and output to the schedule generation function:
//..
//  bdlt::Date earliest(2012, 2,  1);
//  bdlt::Date   latest(2015, 2, 28);
//  bdlt::Date  example(2007, 7, 23);
//
//  bsl::vector<bdlt::Date> schedule;
//..
// Now, we invoke the 'generateFromDayOfMonth' routine to obtain the subset of
// dates:
//..
//  bblb::ScheduleGenerationUtil::generateFromDayOfMonth(
//                                                  &schedule,
//                                                  earliest,
//                                                  latest,
//                                                  example.year(),
//                                                  example.month(),
//                                                  9,    // 'intervalInMonths'
//                                                  23);  // 'targetDayOfMonth'
//..
// Finally, we assert that the generated schedule is what we expect:
//..
//  assert(4 == schedule.size());
//  assert(bdlt::Date(2012, 10, 23) == schedule[0]);
//  assert(bdlt::Date(2013,  7, 23) == schedule[1]);
//  assert(bdlt::Date(2014,  4, 23) == schedule[2]);
//  assert(bdlt::Date(2015,  1, 23) == schedule[3]);
//..

#ifndef INCLUDED_BBLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_CMATH
#include <bsl_cmath.h>
#endif

#ifndef INCLUDED_LIMITS
#include <limits>           // 'std::numeric_limits'
#endif

namespace BloombergLP {
namespace bdlstat {

                      // ============
                      // class Moment
                      // ============
const double DBL_NAN  = std::numeric_limits<double>::quiet_NaN();
enum MomentLevel {M1, M2, M3, M4};

template <MomentLevel ML>
struct Moment_Data;

template<>
struct Moment_Data<M1> {
    // Data members for Mean only.
    int    d_count; // Number of entries.
    double d_sum;   // Sum of entries.
    Moment_Data()
    : d_count(0)
    , d_sum(0.0)
    {}
};

template<>
struct Moment_Data<M2> {
    // Data members for Variance and below.
    int    d_count; // Number of entries.
    double d_sum;   // Sum of entries.
    double d_mean;  // Mean of entries.
    double d_M2;    // 2nd moment, for variance.
    Moment_Data()
    : d_count(0)
    , d_sum(0.0)
    , d_mean(0.0)
    , d_M2(0.0)
    {}
};

template<>
struct Moment_Data<M3> {
    // Data members for Skew and below
    int    d_count; // Number of entries.
    double d_sum;   // Sum of entries.
    double d_mean;  // Mean of entries.
    double d_M2;    // 2nd moment, for variance.
    double d_M3;    // 3rd moment, for skew
    Moment_Data()
    : d_count(0)
    , d_sum(0.0)
    , d_mean(0.0)
    , d_M2(0.0)
    , d_M3(0.0)
    {}
};

template<>
struct Moment_Data<M4> {
    // Data members for Kurtosis and below
    int    d_count; // Number of entries.
    double d_sum;   // Sum of entries.
    double d_mean;  // Mean of entries.
    double d_M2;    // 2nd moment, for variance.
    double d_M3;    // 3rd moment, for skew
    double d_M4;    // 4th moment, for kurtosis
    Moment_Data()
    : d_count(0)
    , d_sum(0.0)
    , d_mean(0.0)
    , d_M2(0.0)
    , d_M3(0.0)
    , d_M4(0.0)
    {}
};

template <MomentLevel ML>
class Moment {
  // How to calculate stable skew and kurtosis
  // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Higher-order_statistics
  private:
    struct Moment_Data<ML> d_data;

  public:
    // CREATORS
    Moment();

    // MANIPULATORS
    void add(double value);

    // ACCESSORS
    int getCount() const;
        // Returns the number of elements in the data set. 

    double getMeanRaw() const;
        // Return mean of the data set.  The behavior is undefined unless
        // 'count' >= 1.

    double getMean() const;
        // Return mean of the data set.  The result is 'Nan' unless
        // 'count' >= 1. 

    double getVarianceRaw() const;
        // Return variance of the data set.  The behavior is undefined unless
        // 'count' >= 2.

    double getVariance() const;
        // Return variance of the data set.  The result is 'Nan' unless
        // 'count' >= 2. 

    double getSkewRaw() const;
        // Return kurtosis of the data set.  The behavior is undefined unless
        // 'count' >= 3.

    double getSkew() const;
        // Return kurtosis of the data set.  The result is 'Nan' unless
        // 'count' >= 3. 

    double getKurtosisRaw() const;
        // Return kurtosis of the data set.  The behavior is undefined unless
        // 'count' >= 4.

    double getKurtosis() const;
        // Return kurtosis of the data set.  The result is 'Nan' unless
        // 'count' >= 4.
};

// CREATORS
template <MomentLevel ML>
inline
Moment<ML>::Moment()
{
}

// MANIPULATORS
template<>
inline void Moment<M1>::add(double value)
{
	++d_data.d_count;
	d_data.d_sum += value;
}

template<>
inline void Moment<M2>::add(double value)
{
	// Modified Welford algorithm for variance
	const double delta = value - d_data.d_mean;
	d_data.d_sum += value;
	++d_data.d_count;
	d_data.d_mean = d_data.d_sum / static_cast<double>(d_data.d_count);
	const double delta2 = value - d_data.d_mean;
	d_data.d_M2 += delta * delta2;
}

template<>
inline void Moment<M3>::add(double value)
{
	// Modified Welford algorithm for variance, and similar algorithm for skew.
    const double delta = value - d_data.d_mean;
    const double nm1 = d_data.d_count;
    d_data.d_sum += value;
    ++d_data.d_count;
    const double n = d_data.d_count;
    d_data.d_mean = d_data.d_sum / n;
    const double deltaN = value - d_data.d_mean;
    d_data.d_M2 += delta * deltaN;
	const double delta2 = delta * delta;
	const double delta3 = delta * delta2;
	const double n2 = n * n;
	d_data.d_M3 += delta3 * nm1 * (nm1 - 1.0) / n2 - 3.0 * delta * d_data.d_M2 / n;
}

template<>
inline void Moment<M4>::add(double value)
{
	// Modified Welford algorithm for variance, and similar algorithms for skew
	// and kurtosis.
	const double delta = value - d_data.d_mean;
	const double nm1 = d_data.d_count;
	d_data.d_sum += value;
	++d_data.d_count;
	const double n = d_data.d_count;
	d_data.d_mean = d_data.d_sum / n;
	const double deltaN = value - d_data.d_mean;
	d_data.d_M2 += delta * deltaN;
	const double delta2 = delta * delta;
	const double delta3 = delta * delta2;
	const double delta4 = delta * delta3;
	const double n2 = n * n;
	const double n3 = n * n2;
	d_data.d_M3 += delta3 * nm1 * (nm1 - 1.0) / n2 - 3.0 * delta * d_data.d_M2 / n;
	d_data.d_M4 += delta4 * nm1 * (n2 - 3.0 * n + 3.0) / n3 + 6.0 * delta2 * d_data.d_M2 / n2;
	d_data.d_M4 -= 4.0 * delta * d_data.d_M3 / n;
}

// ACCESSORS
template <MomentLevel ML>
inline int Moment<ML>::getCount() const
{
	return d_data.d_count;
}

template <MomentLevel ML>
inline double Moment<ML>::getMeanRaw() const
{
	return d_data.d_sum / static_cast<double>(d_data.d_count);
}

template <MomentLevel ML>
inline double Moment<ML>::getMean() const
{
	if (d_data.d_count < 1)
		return DBL_NAN;
    return getMeanRaw();
}

template <MomentLevel ML>
inline double Moment<ML>::getVarianceRaw() const
{
    return d_data.d_M2 / (d_data.d_count - 1);
}

template <MomentLevel ML>
inline double Moment<ML>::getVariance() const
{
    if (d_data.d_count < 1)
		return DBL_NAN;
    return getVarianceRaw();
}

template <MomentLevel ML>
inline double Moment<ML>::getSkewRaw() const
{
	return bsl::sqrt(static_cast<double>(d_data.d_count)) * d_data.d_M3 / bsl::pow(d_data.d_M2, 1.5);
}

template <MomentLevel ML>
inline double Moment<ML>::getSkew() const
{
	if (d_data.d_count < 3)
		return DBL_NAN;
	return getSkewRaw();
}

template<>
inline double Moment<M4>::getKurtosis() const
{
	if (d_data.d_count < 2)
		return DBL_NAN;
	return static_cast<double>(d_data.d_count) * d_data.d_M4 / d_data.d_M2 * d_data.d_M2 - 3.0;
	//return getKurtosisRaw(); // ALISDAIR
}

template<>
inline double Moment<M4>::getKurtosisRaw() const
{
	return static_cast<double>(d_data.d_count) * d_data.d_M4 / d_data.d_M2 * d_data.d_M2 - 3.0;
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
