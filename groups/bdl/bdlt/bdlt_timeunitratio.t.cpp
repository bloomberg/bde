// bdlt_timeunitratio.t.cpp                                           -*-C++-*-
#include <bdlt_timeunitratio.h>

#include <bslim_testutil.h>

#include <bsl_iostream.h>

#include <bsl_cstdlib.h>     // 'atoi'

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TESTPLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test implements a single utility 'struct', that defines
// a set of constants inside an 'enum'.  There is no executable code.
// Therefore the component can be tested in a single test case that simply
// checks the values defined in the 'enum'.
// ----------------------------------------------------------------------------
// [ 2] const Int64 TimeUnitRatio::k_NANOSECONDS_PER_MICROSECOND
// [ 2] const Int64 TimeUnitRatio::k_MICROSECONDS_PER_MILLISECOND
// [ 2] const Int64 TimeUnitRatio::k_MILLISECONDS_PER_SECOND
// [ 2] const Int64 TimeUnitRatio::k_SECONDS_PER_MINUTE
// [ 2] const Int64 TimeUnitRatio::k_MINUTES_PER_HOUR
// [ 2] const Int64 TimeUnitRatio::k_HOURS_PER_DAY
// [ 2] const Int64 TimeUnitRatio::k_NANOSECONDS_PER_MILLISECOND
// [ 2] const Int64 TimeUnitRatio::k_NANOSECONDS_PER_SECOND
// [ 2] const Int64 TimeUnitRatio::k_NANOSECONDS_PER_MINUTE
// [ 2] const Int64 TimeUnitRatio::k_NANOSECONDS_PER_HOUR
// [ 2] const Int64 TimeUnitRatio::k_NANOSECONDS_PER_DAY
// [ 2] const Int64 TimeUnitRatio::k_MICROSECONDS_PER_SECOND
// [ 2] const Int64 TimeUnitRatio::k_MICROSECONDS_PER_MINUTE
// [ 2] const Int64 TimeUnitRatio::k_MICROSECONDS_PER_HOUR
// [ 2] const Int64 TimeUnitRatio::k_MICROSECONDS_PER_DAY
// [ 2] const Int64 TimeUnitRatio::k_MILLISECONDS_PER_MINUTE
// [ 2] const Int64 TimeUnitRatio::k_MILLISECONDS_PER_HOUR
// [ 2] const Int64 TimeUnitRatio::k_MILLISECONDS_PER_DAY
// [ 2] const Int64 TimeUnitRatio::k_SECONDS_PER_HOUR
// [ 2] const Int64 TimeUnitRatio::k_SECONDS_PER_DAY
// [ 2] const Int64 TimeUnitRatio::k_MINUTES_PER_DAY
// [ 2] const Int64 TimeUnitRatio::k_NS_PER_US
// [ 2] const Int64 TimeUnitRatio::k_NS_PER_MS
// [ 2] const Int64 TimeUnitRatio::k_NS_PER_S
// [ 2] const Int64 TimeUnitRatio::k_NS_PER_M
// [ 2] const Int64 TimeUnitRatio::k_NS_PER_H
// [ 2] const Int64 TimeUnitRatio::k_NS_PER_D
// [ 2] const Int64 TimeUnitRatio::k_US_PER_MS
// [ 2] const Int64 TimeUnitRatio::k_US_PER_S
// [ 2] const Int64 TimeUnitRatio::k_US_PER_M
// [ 2] const Int64 TimeUnitRatio::k_US_PER_H
// [ 2] const Int64 TimeUnitRatio::k_US_PER_D
// [ 2] const Int64 TimeUnitRatio::k_MS_PER_S
// [ 2] const Int64 TimeUnitRatio::k_MS_PER_M
// [ 2] const Int64 TimeUnitRatio::k_MS_PER_H
// [ 2] const Int64 TimeUnitRatio::k_MS_PER_D
// [ 2] const Int64 TimeUnitRatio::k_S_PER_M
// [ 2] const Int64 TimeUnitRatio::k_S_PER_H
// [ 2] const Int64 TimeUnitRatio::k_S_PER_D
// [ 2] const Int64 TimeUnitRatio::k_M_PER_H
// [ 2] const Int64 TimeUnitRatio::k_M_PER_D
// [ 2] const Int64 TimeUnitRatio::k_H_PER_D
// [ 2] const int   TimeUnitRatio::k_NANOSECONDS_PER_MICROSECOND_32
// [ 2] const int   TimeUnitRatio::k_NANOSECONDS_PER_MILLISECOND_32
// [ 2] const int   TimeUnitRatio::k_NANOSECONDS_PER_SECOND_32
// [ 2] const int   TimeUnitRatio::k_MICROSECONDS_PER_MILLISECOND_32
// [ 2] const int   TimeUnitRatio::k_MICROSECONDS_PER_SECOND_32
// [ 2] const int   TimeUnitRatio::k_MICROSECONDS_PER_MINUTE_32
// [ 2] const int   TimeUnitRatio::k_MILLISECONDS_PER_SECOND_32
// [ 2] const int   TimeUnitRatio::k_MILLISECONDS_PER_MINUTE_32
// [ 2] const int   TimeUnitRatio::k_MILLISECONDS_PER_HOUR_32
// [ 2] const int   TimeUnitRatio::k_MILLISECONDS_PER_DAY_32
// [ 2] const int   TimeUnitRatio::k_SECONDS_PER_MINUTE_32
// [ 2] const int   TimeUnitRatio::k_SECONDS_PER_HOUR_32
// [ 2] const int   TimeUnitRatio::k_SECONDS_PER_DAY_32
// [ 2] const int   TimeUnitRatio::k_MINUTES_PER_HOUR_32
// [ 2] const int   TimeUnitRatio::k_MINUTES_PER_DAY_32
// [ 2] const int   TimeUnitRatio::k_HOURS_PER_DAY_32
// [ 2] const int   TimeUnitRatio::k_NS_PER_US_32
// [ 2] const int   TimeUnitRatio::k_NS_PER_MS_32
// [ 2] const int   TimeUnitRatio::k_NS_PER_S_32
// [ 2] const int   TimeUnitRatio::k_US_PER_MS_32
// [ 2] const int   TimeUnitRatio::k_US_PER_S_32
// [ 2] const int   TimeUnitRatio::k_US_PER_M_32
// [ 2] const int   TimeUnitRatio::k_MS_PER_S_32
// [ 2] const int   TimeUnitRatio::k_MS_PER_M_32
// [ 2] const int   TimeUnitRatio::k_MS_PER_H_32
// [ 2] const int   TimeUnitRatio::k_MS_PER_D_32
// [ 2] const int   TimeUnitRatio::k_S_PER_M_32
// [ 2] const int   TimeUnitRatio::k_S_PER_H_32
// [ 2] const int   TimeUnitRatio::k_S_PER_D_32
// [ 2] const int   TimeUnitRatio::k_M_PER_H_32
// [ 2] const int   TimeUnitRatio::k_M_PER_D_32
// [ 2] const int   TimeUnitRatio::k_H_PER_D_32
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 1] TEST APPARATUS: 'calculateRatio'

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlt::TimeUnitRatio Util;
typedef bsls::Types::Int64 Int64;


                 // Reference Values for Known Time Intervals

// Copied from NIST Stopwatch and Timer Calibrations guide (2009), page 30.
// http://tf.boulder.nist.gov/general/pdf/2281.pdf

Int64 k_SOURCE_SECS_PER_HOUR   = 3600;
Int64 k_SOURCE_SECS_PER_DAY    = 86400;

// Copied from NIST The Mechatronics Handbook, 2001,
// Chapter 17 "Fundamentals of Time and Frequency", page 5.
// http://tf.nist.gov/general/pdf/1498.pdf

Int64 k_SOURCE_USECS_PER_DAY = 86400000000LL;

// Logical Derivations

Int64 k_SOURCE_NSECS_PER_SEC = 1000000000;
    // nano- indicates 10^-9:  ^123456789

Int64 k_SOURCE_USECS_PER_SEC = 1000000;
    // micro- indicates 10^-6: ^123456

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

bsls::Types::Int64 ratios[] = {
    // Each value in 'ratios' represents the multiplier between a unit of time,
    // and the unit that follows it in the progression from nanosecond to day.

          1000,        1000,        1000,       60,        60,        24,
};

enum TimeIntervalDesignators {
    // Each value in 'TimeIntervalDesignators' represents the offset of into
    // 'ratios' at which the ratio between a given time unit and the next
    // larger time unit occurs.  Note that there is no entry in 'ratios'
    // corresponding to days ('k_D'), because there is no larger time unit.

    k_NS,        k_US,        k_MS,        k_S,       k_M,       k_H,       k_D
};

bsls::Types::Int64 calculateRatio(int numeratorID, int denominatorID)
    // Calculate the ratio between the numerator designated by the specified
    // 'numeratorID' and the denominator designated by the specified
    // 'denominatorID'.  The behavior is undefined unless 'numeratorID' and
    // 'denominatorID' are values that appear in
    // 'enum TimeIntervalDesignators', and 'numeratorID <= denominatorID'.
{
    ASSERT(k_NS <= numeratorID         );
    ASSERT(        numeratorID   <= k_D);

    ASSERT(k_NS <= denominatorID       );
    ASSERT(        denominatorID <= k_D);

    ASSERT(numeratorID <= denominatorID);

    // Walk through the ratios table, accumulating steps between each interval.

    bsls::Types::Int64 ratio = 1;
    for (int i = numeratorID; i < denominatorID; ++i) {
        ratio *= ratios[i];
    }

    return ratio;
}

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void)         veryVerbose;
    (void)     veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Breaking a Time Interval Into Component Parts
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Components that deal with time often need to convert between various units.
// Each component that needs to perform such conversions could derive the
// constants needed locally, but doing so would result in an inconsistent
// vocabulary across components, and multiple opportunities for bugs.
// 'bdlt::TimeUnitRatio' achieves the desired consistency and avoids bugs by
// providing a single location for the constants used in such conversions.
//
// Suppose we have a time interval described as an integral number of
// nanoseconds, and we need to break it down into its constituent second,
// millisecond, microsecond, and nanosecond parts.
//
// First, we define a variable representing the number of nanoseconds that have
// elapsed since a particular event:
//..
    bsls::Types::Int64 interval = 62003004005LL;  // nanoseconds since event
//..
// Then, we extract the minutes part from the total, using the constant
// 'bdlt::TimeUnitRatio::k_NS_PER_M':
//..
    bsls::Types::Int64 minutesPart =
                                    interval / bdlt::TimeUnitRatio::k_NS_PER_M;
//..
// Next, we calculate the remaining nanoseconds using the same constant:
//..
    bsls::Types::Int64 remainder = interval % bdlt::TimeUnitRatio::k_NS_PER_M;
//..
// Then, we extract the seconds part from the remainder, using the constant
// 'bdlt::TimeUnitRatio::k_NS_PER_S':
//..
    bsls::Types::Int64 secondsPart =
                                   remainder / bdlt::TimeUnitRatio::k_NS_PER_S;
//..
// Next, we calculate the remaining nanoseconds using the same constant:
//..
    remainder %= bdlt::TimeUnitRatio::k_NS_PER_S;
//..
// Then, we extract the milliseconds part from the remainder, using the
// constant 'bdlt::TimeUnitRatio::k_NS_PER_MS':
//..
    bsls::Types::Int64 millisecondsPart =
                                  remainder / bdlt::TimeUnitRatio::k_NS_PER_MS;
//..
// Next, we calculate the remaining nanoseconds using the same constant:
//..
    remainder %= bdlt::TimeUnitRatio::k_NS_PER_MS;
//..
// Then, we extract the microseconds part from the remainder, using the
// constant 'bdlt::TimeUnitRatio::k_NS_PER_US':
//..
    bsls::Types::Int64 microsecondsPart =
                                  remainder / bdlt::TimeUnitRatio::k_NS_PER_US;
//..
// Next, we calculate the remaining nanoseconds using the same constant:
//..
    remainder %= bdlt::TimeUnitRatio::k_NS_PER_US;
//..
// Now, we extract the nanoseconds part, which is exactly the remainder we
// already have:
//..
    bsls::Types::Int64 nanosecondsPart = remainder;
//..
// Finally, we confirm that the parts we have extracted all have the correct
// values:
//..
    ASSERT(1 == minutesPart);
    ASSERT(2 == secondsPart);
    ASSERT(3 == millisecondsPart);
    ASSERT(4 == microsecondsPart);
    ASSERT(5 == nanosecondsPart);
//..
// Note that in practice, the number of nanoseconds since the event would be
// provided by some system utility, and not a constant as was shown here for
// purposes of exposition.
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'TimeUnitRatio' VALUES
        //
        // Concerns:
        //: 1 That each value defined in 'TimeUnitRatio' is correct.
        //
        // Plan:
        //: 1 Using brute force, compare each explicit value defined in
        //:   'TimeUnitRatio' to a value calculated using the 'calculateRatio'
        //:   helper function.  (C-1)
        //
        // Testing:
        //   const Int64 TimeUnitRatio::k_NANOSECONDS_PER_MICROSECOND
        //   const Int64 TimeUnitRatio::k_MICROSECONDS_PER_MILLISECOND
        //   const Int64 TimeUnitRatio::k_MILLISECONDS_PER_SECOND
        //   const Int64 TimeUnitRatio::k_SECONDS_PER_MINUTE
        //   const Int64 TimeUnitRatio::k_MINUTES_PER_HOUR
        //   const Int64 TimeUnitRatio::k_HOURS_PER_DAY
        //   const Int64 TimeUnitRatio::k_NANOSECONDS_PER_MILLISECOND
        //   const Int64 TimeUnitRatio::k_NANOSECONDS_PER_SECOND
        //   const Int64 TimeUnitRatio::k_NANOSECONDS_PER_MINUTE
        //   const Int64 TimeUnitRatio::k_NANOSECONDS_PER_HOUR
        //   const Int64 TimeUnitRatio::k_NANOSECONDS_PER_DAY
        //   const Int64 TimeUnitRatio::k_MICROSECONDS_PER_SECOND
        //   const Int64 TimeUnitRatio::k_MICROSECONDS_PER_MINUTE
        //   const Int64 TimeUnitRatio::k_MICROSECONDS_PER_HOUR
        //   const Int64 TimeUnitRatio::k_MICROSECONDS_PER_DAY
        //   const Int64 TimeUnitRatio::k_MILLISECONDS_PER_MINUTE
        //   const Int64 TimeUnitRatio::k_MILLISECONDS_PER_HOUR
        //   const Int64 TimeUnitRatio::k_MILLISECONDS_PER_DAY
        //   const Int64 TimeUnitRatio::k_SECONDS_PER_HOUR
        //   const Int64 TimeUnitRatio::k_SECONDS_PER_DAY
        //   const Int64 TimeUnitRatio::k_MINUTES_PER_DAY
        //   const Int64 TimeUnitRatio::k_NS_PER_US
        //   const Int64 TimeUnitRatio::k_NS_PER_MS
        //   const Int64 TimeUnitRatio::k_NS_PER_S
        //   const Int64 TimeUnitRatio::k_NS_PER_M
        //   const Int64 TimeUnitRatio::k_NS_PER_H
        //   const Int64 TimeUnitRatio::k_NS_PER_D
        //   const Int64 TimeUnitRatio::k_US_PER_MS
        //   const Int64 TimeUnitRatio::k_US_PER_S
        //   const Int64 TimeUnitRatio::k_US_PER_M
        //   const Int64 TimeUnitRatio::k_US_PER_H
        //   const Int64 TimeUnitRatio::k_US_PER_D
        //   const Int64 TimeUnitRatio::k_MS_PER_S
        //   const Int64 TimeUnitRatio::k_MS_PER_M
        //   const Int64 TimeUnitRatio::k_MS_PER_H
        //   const Int64 TimeUnitRatio::k_MS_PER_D
        //   const Int64 TimeUnitRatio::k_S_PER_M
        //   const Int64 TimeUnitRatio::k_S_PER_H
        //   const Int64 TimeUnitRatio::k_S_PER_D
        //   const Int64 TimeUnitRatio::k_M_PER_H
        //   const Int64 TimeUnitRatio::k_M_PER_D
        //   const Int64 TimeUnitRatio::k_H_PER_D
        //   const int   TimeUnitRatio::k_NANOSECONDS_PER_MICROSECOND_32
        //   const int   TimeUnitRatio::k_NANOSECONDS_PER_MILLISECOND_32
        //   const int   TimeUnitRatio::k_NANOSECONDS_PER_SECOND_32
        //   const int   TimeUnitRatio::k_MICROSECONDS_PER_MILLISECOND_32
        //   const int   TimeUnitRatio::k_MICROSECONDS_PER_SECOND_32
        //   const int   TimeUnitRatio::k_MICROSECONDS_PER_MINUTE_32
        //   const int   TimeUnitRatio::k_MILLISECONDS_PER_SECOND_32
        //   const int   TimeUnitRatio::k_MILLISECONDS_PER_MINUTE_32
        //   const int   TimeUnitRatio::k_MILLISECONDS_PER_HOUR_32
        //   const int   TimeUnitRatio::k_MILLISECONDS_PER_DAY_32
        //   const int   TimeUnitRatio::k_SECONDS_PER_MINUTE_32
        //   const int   TimeUnitRatio::k_SECONDS_PER_HOUR_32
        //   const int   TimeUnitRatio::k_SECONDS_PER_DAY_32
        //   const int   TimeUnitRatio::k_MINUTES_PER_HOUR_32
        //   const int   TimeUnitRatio::k_MINUTES_PER_DAY_32
        //   const int   TimeUnitRatio::k_HOURS_PER_DAY_32
        //   const int   TimeUnitRatio::k_NS_PER_US_32
        //   const int   TimeUnitRatio::k_NS_PER_MS_32
        //   const int   TimeUnitRatio::k_NS_PER_S_32
        //   const int   TimeUnitRatio::k_US_PER_MS_32
        //   const int   TimeUnitRatio::k_US_PER_S_32
        //   const int   TimeUnitRatio::k_US_PER_M_32
        //   const int   TimeUnitRatio::k_MS_PER_S_32
        //   const int   TimeUnitRatio::k_MS_PER_M_32
        //   const int   TimeUnitRatio::k_MS_PER_H_32
        //   const int   TimeUnitRatio::k_MS_PER_D_32
        //   const int   TimeUnitRatio::k_S_PER_M_32
        //   const int   TimeUnitRatio::k_S_PER_H_32
        //   const int   TimeUnitRatio::k_S_PER_D_32
        //   const int   TimeUnitRatio::k_M_PER_H_32
        //   const int   TimeUnitRatio::k_M_PER_D_32
        //   const int   TimeUnitRatio::k_H_PER_D_32
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'TimeUnitRatio' VALUES" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nCompare constants to expected values." << endl;

                             // Fundamental Ratios

        {
            Int64 expected = calculateRatio(k_NS,           k_US);
            Int64 result   =          Util::k_NANOSECONDS_PER_MICROSECOND;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = calculateRatio(k_US,            k_MS);
            Int64 result   =          Util::k_MICROSECONDS_PER_MILLISECOND;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = calculateRatio(k_MS,            k_S);
            Int64 result   =          Util::k_MILLISECONDS_PER_SECOND;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = calculateRatio(k_S,        k_M);
            Int64 result   =          Util::k_SECONDS_PER_MINUTE;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = calculateRatio(k_M,        k_H);
            Int64 result   =          Util::k_MINUTES_PER_HOUR;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = calculateRatio(k_H,      k_D);
            Int64 result   =          Util::k_HOURS_PER_DAY;
            ASSERTV(expected, result, expected == result);
        }


                               // Derived Ratios

        {
            Int64 expected = calculateRatio(k_NS,           k_MS);
            Int64 result   =          Util::k_NANOSECONDS_PER_MILLISECOND;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = calculateRatio(k_NS,           k_S);
            Int64 result   =          Util::k_NANOSECONDS_PER_SECOND;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = calculateRatio(k_NS,           k_M);
            Int64 result   =          Util::k_NANOSECONDS_PER_MINUTE;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = calculateRatio(k_NS,           k_H);
            Int64 result   =          Util::k_NANOSECONDS_PER_HOUR;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = calculateRatio(k_NS,           k_D);
            Int64 result   =          Util::k_NANOSECONDS_PER_DAY;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = calculateRatio(k_US,            k_S);
            Int64 result   =          Util::k_MICROSECONDS_PER_SECOND;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = calculateRatio(k_US,            k_M);
            Int64 result   =          Util::k_MICROSECONDS_PER_MINUTE;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = calculateRatio(k_US,  k_H);
            Int64 result   =          Util::k_MICROSECONDS_PER_HOUR;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = calculateRatio(k_US,            k_D);
            Int64 result   =          Util::k_MICROSECONDS_PER_DAY;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = calculateRatio(k_MS,            k_M);
            Int64 result   =          Util::k_MILLISECONDS_PER_MINUTE;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = calculateRatio(k_MS,            k_H);
            Int64 result   =          Util::k_MILLISECONDS_PER_HOUR;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = calculateRatio(k_MS,            k_D);
            Int64 result   =          Util::k_MILLISECONDS_PER_DAY;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = calculateRatio(k_S,        k_H);
            Int64 result   =          Util::k_SECONDS_PER_HOUR;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = calculateRatio(k_S,        k_D);
            Int64 result   =          Util::k_SECONDS_PER_DAY;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = calculateRatio(k_M,        k_D);
            Int64 result   =          Util::k_MINUTES_PER_DAY;
            ASSERTV(expected, result, expected == result);
        }


        if (verbose) cout << "\nCompare aliases to base constants." << endl;

                               // Abbreviations

        {
            Int64 expected = Util::k_NANOSECONDS_PER_MICROSECOND;
            Int64 result   = Util::k_NS_PER_US;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_NANOSECONDS_PER_MILLISECOND;
            Int64 result   = Util::k_NS_PER_MS;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_NANOSECONDS_PER_SECOND;
            Int64 result   = Util::k_NS_PER_S;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_NANOSECONDS_PER_MINUTE;
            Int64 result   = Util::k_NS_PER_M;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_NANOSECONDS_PER_HOUR;
            Int64 result   = Util::k_NS_PER_H;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_NANOSECONDS_PER_DAY;
            Int64 result   = Util::k_NS_PER_D;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = Util::k_MICROSECONDS_PER_MILLISECOND;
            Int64 result   = Util::k_US_PER_MS;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MICROSECONDS_PER_SECOND;
            Int64 result   = Util::k_US_PER_S;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MICROSECONDS_PER_MINUTE;
            Int64 result   = Util::k_US_PER_M;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MICROSECONDS_PER_HOUR;
            Int64 result   = Util::k_US_PER_H;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MICROSECONDS_PER_DAY;
            Int64 result   = Util::k_US_PER_D;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = Util::k_MILLISECONDS_PER_SECOND;
            Int64 result   = Util::k_MS_PER_S;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MILLISECONDS_PER_MINUTE;
            Int64 result   = Util::k_MS_PER_M;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MILLISECONDS_PER_HOUR;
            Int64 result   = Util::k_MS_PER_H;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MILLISECONDS_PER_DAY;
            Int64 result   = Util::k_MS_PER_D;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = Util::k_SECONDS_PER_MINUTE;
            Int64 result   = Util::k_S_PER_M;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_SECONDS_PER_HOUR;
            Int64 result   = Util::k_S_PER_H;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_SECONDS_PER_DAY;
            Int64 result   = Util::k_S_PER_D;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = Util::k_MINUTES_PER_HOUR;
            Int64 result   = Util::k_M_PER_H;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MINUTES_PER_DAY;
            Int64 result   = Util::k_M_PER_D;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = Util::k_HOURS_PER_DAY;
            Int64 result   = Util::k_H_PER_D;
            ASSERTV(expected, result, expected == result);
        }


                             // 32-Bit Variations

        {
            Int64 expected = Util::k_NANOSECONDS_PER_MICROSECOND;
            Int64 result   = Util::k_NANOSECONDS_PER_MICROSECOND_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_NANOSECONDS_PER_MILLISECOND;
            Int64 result   = Util::k_NANOSECONDS_PER_MILLISECOND_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_NANOSECONDS_PER_SECOND;
            Int64 result   = Util::k_NANOSECONDS_PER_SECOND_32;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = Util::k_MICROSECONDS_PER_MILLISECOND;
            Int64 result   = Util::k_MICROSECONDS_PER_MILLISECOND_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MICROSECONDS_PER_SECOND;
            Int64 result   = Util::k_MICROSECONDS_PER_SECOND_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MICROSECONDS_PER_MINUTE;
            Int64 result   = Util::k_MICROSECONDS_PER_MINUTE_32;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = Util::k_MILLISECONDS_PER_SECOND;
            Int64 result   = Util::k_MILLISECONDS_PER_SECOND_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MILLISECONDS_PER_MINUTE;
            Int64 result   = Util::k_MILLISECONDS_PER_MINUTE_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MILLISECONDS_PER_HOUR;
            Int64 result   = Util::k_MILLISECONDS_PER_HOUR_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MILLISECONDS_PER_DAY;
            Int64 result   = Util::k_MILLISECONDS_PER_DAY_32;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = Util::k_SECONDS_PER_MINUTE;
            Int64 result   = Util::k_SECONDS_PER_MINUTE_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_SECONDS_PER_HOUR;
            Int64 result   = Util::k_SECONDS_PER_HOUR_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_SECONDS_PER_DAY;
            Int64 result   = Util::k_SECONDS_PER_DAY_32;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = Util::k_MINUTES_PER_HOUR;
            Int64 result   = Util::k_MINUTES_PER_HOUR_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MINUTES_PER_DAY;
            Int64 result   = Util::k_MINUTES_PER_DAY_32;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = Util::k_HOURS_PER_DAY;
            Int64 result   = Util::k_HOURS_PER_DAY_32;
            ASSERTV(expected, result, expected == result);
        }


                       // Abbreviated 32-Bit Variations

        {
            Int64 expected = Util::k_NANOSECONDS_PER_MICROSECOND;
            Int64 result   = Util::k_NS_PER_US_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_NANOSECONDS_PER_MILLISECOND;
            Int64 result   = Util::k_NS_PER_MS_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_NANOSECONDS_PER_SECOND;
            Int64 result   = Util::k_NS_PER_S_32;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = Util::k_MICROSECONDS_PER_MILLISECOND;
            Int64 result   = Util::k_US_PER_MS_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MICROSECONDS_PER_SECOND;
            Int64 result   = Util::k_US_PER_S_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MICROSECONDS_PER_MINUTE;
            Int64 result   = Util::k_US_PER_M_32;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = Util::k_MILLISECONDS_PER_SECOND;
            Int64 result   = Util::k_MS_PER_S_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MILLISECONDS_PER_MINUTE;
            Int64 result   = Util::k_MS_PER_M_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MILLISECONDS_PER_HOUR;
            Int64 result   = Util::k_MS_PER_H_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MILLISECONDS_PER_DAY;
            Int64 result   = Util::k_MS_PER_D_32;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = Util::k_SECONDS_PER_MINUTE;
            Int64 result   = Util::k_S_PER_M_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_SECONDS_PER_HOUR;
            Int64 result   = Util::k_S_PER_H_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_SECONDS_PER_DAY;
            Int64 result   = Util::k_S_PER_D_32;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = Util::k_MINUTES_PER_HOUR;
            Int64 result   = Util::k_M_PER_H_32;
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = Util::k_MINUTES_PER_DAY;
            Int64 result   = Util::k_M_PER_D_32;
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = Util::k_HOURS_PER_DAY;
            Int64 result   = Util::k_H_PER_D_32;
            ASSERTV(expected, result, expected == result);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING ORACLE
        //
        // Concerns:
        //: 1 That each single-step ratio between neighboring units is correct.
        //:
        //: 2 That the ratio between a unit and itself is '1'.
        //:
        //: 3 That ratios are accumulated across the entire range of units.
        //
        // Plan:
        //: 1 Using brute force, check the calculated ratio of each unit with
        //:   itself and with the next larger unit in the progression from
        //:   nanosecond to day.  (C-1,2)
        //:
        //: 2 Compare accumulated ratios with documented or empirically-derived
        //:   values for larger time ratios.  (C-3)
        //
        // Testing:
        //   TEST APPARATUS: 'calculateRatio'
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ORACLE" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nCheck internal consistency of oracle." << endl;

        {
            Int64 expected = 1;
            Int64 result   = calculateRatio(k_NS, k_NS);
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = 1;
            Int64 result   = calculateRatio(k_US, k_US);
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = 1;
            Int64 result   = calculateRatio(k_MS, k_MS);
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = 1;
            Int64 result   = calculateRatio(k_S,  k_S);
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = 1;
            Int64 result   = calculateRatio(k_M,  k_M);
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = 1;
            Int64 result   = calculateRatio(k_H,  k_H);
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = 1;
            Int64 result   = calculateRatio(k_D,  k_D);
            ASSERTV(expected, result, expected == result);
        }


        {
            Int64 expected = ratios[k_NS];
            Int64 result   = calculateRatio(k_NS, k_US);
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = ratios[k_US];
            Int64 result   = calculateRatio(k_US, k_MS);
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = ratios[k_MS];
            Int64 result   = calculateRatio(k_MS, k_S);
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = ratios[k_S];
            Int64 result   = calculateRatio(k_S,  k_M);
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = ratios[k_M];
            Int64 result   = calculateRatio(k_M,  k_H);
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected = ratios[k_H];
            Int64 result   = calculateRatio(k_H,  k_D);
            ASSERTV(expected, result, expected == result);
        }


        if (verbose) cout << "\nCompare oracle to reference values." << endl;

        {
            Int64 expected =         k_SOURCE_NSECS_PER_SEC;
            Int64 result   = calculateRatio(k_NS,     k_S);
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected =         k_SOURCE_USECS_PER_SEC;
            Int64 result   = calculateRatio(k_US,     k_S);
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected =         k_SOURCE_SECS_PER_HOUR;
            Int64 result   = calculateRatio(k_S,     k_H);
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected =         k_SOURCE_SECS_PER_DAY;
            Int64 result   = calculateRatio(k_S,     k_D);
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected =         k_SOURCE_USECS_PER_DAY;
            Int64 result   = calculateRatio(k_US,     k_D);
            ASSERTV(expected, result, expected == result);
        }

        {
            Int64 expected =         k_SOURCE_NSECS_PER_SEC
                                             * k_SOURCE_SECS_PER_DAY;
            Int64 result   = calculateRatio(k_NS,              k_D);
            ASSERTV(expected, result, expected == result);
        }
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
// Copyright 2014 Bloomberg Finance L.P.
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
