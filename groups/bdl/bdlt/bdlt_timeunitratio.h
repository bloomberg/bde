// bdlt_timeunitratio.h                                               -*-C++-*-
#ifndef INCLUDED_BDLT_TIMEUNITRATIO
#define INCLUDED_BDLT_TIMEUNITRATIO

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide constants characterizing ratios between common time units.
//
//@CLASSES:
//  bdlt::TimeUnitRatio: namespace for common time-unit-ratio constants
//
//@DESCRIPTION: This component provides a utility 'struct',
// 'bdlt::TimeUnitRatio', that defines a namespace for constants that
// characterize the ratios between commonly used time units.  Ratios are
// provided for combinations of all time units from a nanosecond up through a
// day.
//
// Each constant defined in this namespace has at least two forms, a full form
// and an abbreviated form following the patterns:
//..
//  bdlt::TimeUnitRatio::k_SMALLUNITS_PER_LARGEUNIT
//  bdlt::TimeUnitRatio::k_SU_PER_LU
//..
// Where 'SMALLUNIT' and 'LARGEUNIT' are members of the following group:
//..
//  NANOSECOND
//  MICROSECOND
//  MILLISECOND
//  SECOND
//  MINUTE
//  HOUR
//  DAY
//..
// and 'SU' and 'LU' are abbreviations in the following group:
//..
//  NS // abbreviation for 'NANOSECOND'
//  US // abbreviation for 'MICROSECOND'
//  MS // abbreviation for 'MILLISECOND'
//  S  // abbreviation for 'SECOND'
//  M  // abbreviation for 'MINUTE'
//  H  // abbreviation for 'HOUR'
//  D  // abbreviation for 'DAY'
//..
// Note that 'SMALLUNIT' ('SU') is always a smaller unit than 'LARGEUNIT'
// ('LU').  Thus, the (whole) number of microseconds in an hour is
// characterized by the constants:
//..
//  bdlt::TimeUnitRatio::k_MICROSECONDS_PER_HOUR
//  bdlt::TimeUnitRatio::k_US_PER_H'
//..
// Additionally, ratios that can be represented by a 32-bit integer also have
// two corresponding 32-bit constants, having the same names as the constants
// described above, but incorporate a '_32' suffix.  Thus, the number of
// milliseconds per minute is characterized by four constants:
//..
//  bdlt::TimeUnitRatio::k_MILLISECONDS_PER_MINUTE
//  bdlt::TimeUnitRatio::k_MS_PER_M
//  bdlt::TimeUnitRatio::k_MILLISECONDS_PER_MINUTE_32
//  bdlt::TimeUnitRatio::k_MS_PER_M_32
//..
// By providing both 64- and 32-bit versions of the constants, this component
// allows the programmer to minimize the use of casts in time unit
// calculations.
//
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
//  bsls::Types::Int64 interval = 62003004005;  // nanoseconds since event
//..
// Then, we extract the minutes part from the total, using the constant
// 'bdlt::TimeUnitRatio::k_NS_PER_M':
//..
//  bsls::Types::Int64 minutesPart =
//                                  interval / bdlt::TimeUnitRatio::k_NS_PER_M;
//..
// Next, we calculate the remaining nanoseconds using the same constant:
//..
//  bsls::Types::Int64 remainder = interval % bdlt::TimeUnitRatio::k_NS_PER_M;
//..
// Then, we extract the seconds part from the remainder, using the constant
// 'bdlt::TimeUnitRatio::k_NS_PER_S':
//..
//  bsls::Types::Int64 secondsPart =
//                                 remainder / bdlt::TimeUnitRatio::k_NS_PER_S;
//..
// Next, we calculate the remaining nanoseconds using the same constant:
//..
//  remainder %= bdlt::TimeUnitRatio::k_NS_PER_S;
//..
// Then, we extract the milliseconds part from the remainder, using the
// constant 'bdlt::TimeUnitRatio::k_NS_PER_MS':
//..
//  bsls::Types::Int64 millisecondsPart =
//                                remainder / bdlt::TimeUnitRatio::k_NS_PER_MS;
//..
// Next, we calculate the remaining nanoseconds using the same constant:
//..
//  remainder %= bdlt::TimeUnitRatio::k_NS_PER_MS;
//..
// Then, we extract the microseconds part from the remainder, using the
// constant 'bdlt::TimeUnitRatio::k_NS_PER_US':
//..
//  bsls::Types::Int64 microsecondsPart =
//                                remainder / bdlt::TimeUnitRatio::k_NS_PER_US;
//..
// Next, we calculate the remaining nanoseconds using the same constant:
//..
//  remainder %= bdlt::TimeUnitRatio::k_NS_PER_US;
//..
// Now, we extract the nanoseconds part, which is exactly the remainder we
// already have:
//..
//  bsls::Types::Int64 nanosecondsPart = remainder;
//..
// Finally, we confirm that the parts we have extracted all have the correct
// values:
//..
//  assert(1 == minutesPart);
//  assert(2 == secondsPart);
//  assert(3 == millisecondsPart);
//  assert(4 == microsecondsPart);
//  assert(5 == nanosecondsPart);
//..
// Note that in practice, the number of nanoseconds since the event would be
// provided by some system utility, and not a constant as was shown here for
// purposes of exposition.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {
namespace bdlt {

                            // ====================
                            // struct TimeUnitRatio
                            // ====================

struct TimeUnitRatio {
    // This 'struct' provides a namespace for constants characterizing ratios
    // between commonly-used time units.

    // CLASS DATA

                             // Fundamental ratios

    static const bsls::Types::Int64 k_NANOSECONDS_PER_MICROSECOND  = 1000;
    static const bsls::Types::Int64 k_MICROSECONDS_PER_MILLISECOND = 1000;
    static const bsls::Types::Int64 k_MILLISECONDS_PER_SECOND      = 1000;
    static const bsls::Types::Int64 k_SECONDS_PER_MINUTE           = 60;
    static const bsls::Types::Int64 k_MINUTES_PER_HOUR             = 60;
    static const bsls::Types::Int64 k_HOURS_PER_DAY                = 24;

                               // Derived ratios

    static const bsls::Types::Int64 k_NANOSECONDS_PER_MILLISECOND =
                k_NANOSECONDS_PER_MICROSECOND * k_MICROSECONDS_PER_MILLISECOND;

    static const bsls::Types::Int64 k_NANOSECONDS_PER_SECOND      =
                k_NANOSECONDS_PER_MILLISECOND * k_MILLISECONDS_PER_SECOND;

    static const bsls::Types::Int64 k_NANOSECONDS_PER_MINUTE      =
                k_NANOSECONDS_PER_SECOND * k_SECONDS_PER_MINUTE;

    static const bsls::Types::Int64 k_NANOSECONDS_PER_HOUR        =
                k_NANOSECONDS_PER_MINUTE * k_MINUTES_PER_HOUR;

    static const bsls::Types::Int64 k_NANOSECONDS_PER_DAY         =
                k_NANOSECONDS_PER_HOUR * k_HOURS_PER_DAY;


    static const bsls::Types::Int64 k_MICROSECONDS_PER_SECOND     =
                k_MICROSECONDS_PER_MILLISECOND * k_MILLISECONDS_PER_SECOND;

    static const bsls::Types::Int64 k_MICROSECONDS_PER_MINUTE     =
                k_MICROSECONDS_PER_SECOND * k_SECONDS_PER_MINUTE;

    static const bsls::Types::Int64 k_MICROSECONDS_PER_HOUR       =
                k_MICROSECONDS_PER_MINUTE * k_MINUTES_PER_HOUR;

    static const bsls::Types::Int64 k_MICROSECONDS_PER_DAY        =
                k_MICROSECONDS_PER_HOUR * k_HOURS_PER_DAY;


    static const bsls::Types::Int64 k_MILLISECONDS_PER_MINUTE     =
                k_MILLISECONDS_PER_SECOND * k_SECONDS_PER_MINUTE;

    static const bsls::Types::Int64 k_MILLISECONDS_PER_HOUR       =
                k_MILLISECONDS_PER_MINUTE * k_MINUTES_PER_HOUR;

    static const bsls::Types::Int64 k_MILLISECONDS_PER_DAY        =
                k_MILLISECONDS_PER_HOUR * k_HOURS_PER_DAY;


    static const bsls::Types::Int64 k_SECONDS_PER_HOUR            =
                k_MINUTES_PER_HOUR * k_SECONDS_PER_MINUTE;

    static const bsls::Types::Int64 k_SECONDS_PER_DAY             =
                k_HOURS_PER_DAY * k_SECONDS_PER_HOUR;

    static const bsls::Types::Int64 k_MINUTES_PER_DAY             =
                k_HOURS_PER_DAY * k_MINUTES_PER_HOUR;

                               // Abbreviations

    static const bsls::Types::Int64 k_NS_PER_US =
                                                 k_NANOSECONDS_PER_MICROSECOND;
    static const bsls::Types::Int64 k_NS_PER_MS =
                                                 k_NANOSECONDS_PER_MILLISECOND;
    static const bsls::Types::Int64 k_NS_PER_S  = k_NANOSECONDS_PER_SECOND;
    static const bsls::Types::Int64 k_NS_PER_M  = k_NANOSECONDS_PER_MINUTE;
    static const bsls::Types::Int64 k_NS_PER_H  = k_NANOSECONDS_PER_HOUR;
    static const bsls::Types::Int64 k_NS_PER_D  = k_NANOSECONDS_PER_DAY;

    static const bsls::Types::Int64 k_US_PER_MS =
                                                k_MICROSECONDS_PER_MILLISECOND;
    static const bsls::Types::Int64 k_US_PER_S  = k_MICROSECONDS_PER_SECOND;
    static const bsls::Types::Int64 k_US_PER_M  = k_MICROSECONDS_PER_MINUTE;
    static const bsls::Types::Int64 k_US_PER_H  = k_MICROSECONDS_PER_HOUR;
    static const bsls::Types::Int64 k_US_PER_D  = k_MICROSECONDS_PER_DAY;

    static const bsls::Types::Int64 k_MS_PER_S  = k_MILLISECONDS_PER_SECOND;
    static const bsls::Types::Int64 k_MS_PER_M  = k_MILLISECONDS_PER_MINUTE;
    static const bsls::Types::Int64 k_MS_PER_H  = k_MILLISECONDS_PER_HOUR;
    static const bsls::Types::Int64 k_MS_PER_D  = k_MILLISECONDS_PER_DAY;

    static const bsls::Types::Int64 k_S_PER_M   = k_SECONDS_PER_MINUTE;
    static const bsls::Types::Int64 k_S_PER_H   = k_SECONDS_PER_HOUR;
    static const bsls::Types::Int64 k_S_PER_D   = k_SECONDS_PER_DAY;

    static const bsls::Types::Int64 k_M_PER_H   = k_MINUTES_PER_HOUR;
    static const bsls::Types::Int64 k_M_PER_D   = k_MINUTES_PER_DAY;

    static const bsls::Types::Int64 k_H_PER_D   = k_HOURS_PER_DAY;

                             // 32-Bit Variations

    static const int k_NANOSECONDS_PER_MICROSECOND_32  =
                                                k_NANOSECONDS_PER_MICROSECOND;
    static const int k_NANOSECONDS_PER_MILLISECOND_32  =
                                                k_NANOSECONDS_PER_MILLISECOND;
    static const int k_NANOSECONDS_PER_SECOND_32       =
                                                k_NANOSECONDS_PER_SECOND;

    static const int k_MICROSECONDS_PER_MILLISECOND_32 =
                                                k_MICROSECONDS_PER_MILLISECOND;
    static const int k_MICROSECONDS_PER_SECOND_32      =
                                                k_MICROSECONDS_PER_SECOND;
    static const int k_MICROSECONDS_PER_MINUTE_32      =
                                                k_MICROSECONDS_PER_MINUTE;

    static const int k_MILLISECONDS_PER_SECOND_32 = k_MILLISECONDS_PER_SECOND;
    static const int k_MILLISECONDS_PER_MINUTE_32 = k_MILLISECONDS_PER_MINUTE;
    static const int k_MILLISECONDS_PER_HOUR_32   = k_MILLISECONDS_PER_HOUR;
    static const int k_MILLISECONDS_PER_DAY_32    = k_MILLISECONDS_PER_DAY;

    static const int k_SECONDS_PER_MINUTE_32      = k_SECONDS_PER_MINUTE;
    static const int k_SECONDS_PER_HOUR_32        = k_SECONDS_PER_HOUR;
    static const int k_SECONDS_PER_DAY_32         = k_SECONDS_PER_DAY;

    static const int k_MINUTES_PER_HOUR_32        = k_MINUTES_PER_HOUR;
    static const int k_MINUTES_PER_DAY_32         = k_MINUTES_PER_DAY;

    static const int k_HOURS_PER_DAY_32 = k_HOURS_PER_DAY;

                       // Abbreviated 32-Bit Variations

    static const int k_NS_PER_US_32 = k_NANOSECONDS_PER_MICROSECOND;
    static const int k_NS_PER_MS_32 = k_NANOSECONDS_PER_MILLISECOND;
    static const int k_NS_PER_S_32  = k_NANOSECONDS_PER_SECOND;

    static const int k_US_PER_MS_32 = k_MICROSECONDS_PER_MILLISECOND;
    static const int k_US_PER_S_32  = k_MICROSECONDS_PER_SECOND;
    static const int k_US_PER_M_32  = k_MICROSECONDS_PER_MINUTE;

    static const int k_MS_PER_S_32  = k_MILLISECONDS_PER_SECOND;
    static const int k_MS_PER_M_32  = k_MILLISECONDS_PER_MINUTE;
    static const int k_MS_PER_H_32  = k_MILLISECONDS_PER_HOUR;
    static const int k_MS_PER_D_32  = k_MILLISECONDS_PER_DAY;

    static const int  k_S_PER_M_32  = k_SECONDS_PER_MINUTE;
    static const int  k_S_PER_H_32  = k_SECONDS_PER_HOUR;
    static const int  k_S_PER_D_32  = k_SECONDS_PER_DAY;

    static const int  k_M_PER_H_32  = k_MINUTES_PER_HOUR;
    static const int  k_M_PER_D_32  = k_MINUTES_PER_DAY;

    static const int  k_H_PER_D_32  = k_HOURS_PER_DAY;
};

}  // close package namespace
}  // close enterprise namespace

#endif

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
