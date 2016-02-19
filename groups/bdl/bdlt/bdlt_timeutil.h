// bdlt_timeutil.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLT_TIMEUTIL
#define INCLUDED_BDLT_TIMEUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide common non-primitive operations on 'bdlt::Time'.
//
//@CLASSES:
//  bdlt::TimeUtil: namespace for static functions operating on 'bdlt::Time'
//
//@SEE_ALSO: bdlt_time
//
//@DESCRIPTION: This component provides non-primitive operations on
// 'bdlt::Time' objects.  In particular, the 'bdlt::TimeUtil' namespace defined
// in this component provides conversions among 'bdlt::Time' values and their
// corresponding non-negative integral values (e.g., 'convertFromHHMM',
// 'convertToHHMMSSmmm'), and methods to validate such integral values (e.g.,
// 'isValidHHMMSS') before passing them to the corresponding "convertFrom"
// method.
//
///Usage
///------
// Following are examples illustrating basic use of this component.
//
///Example 1
///- - - - -
// First, we demonstrate how to use 'bdlt::TimeUtil' to
// convert from an integer representation of time in "HHMMSSmmm" format to a
// 'bdlt::Time'.  Our first time will be around 3:45 pm.
//..
//  //      format: HHMMSSmmm
//  int timeValue = 154502789;
//
//  bdlt::Time result = bdlt::TimeUtil::convertFromHHMMSSmmm(timeValue);
//
//  bsl::cout << result << bsl::endl;
//..
// The code above produces the following on 'stdout':
//..
//  15:45:02.789
//..
// Then, we demonstrate a different time, 3:32:24.832 am.  Note that we do not
// lead the integer value with '0':
//..
//  //  format: HHMMSSmmm
//  timeValue =  33224832;      // Do not start with leading '0' as that would
//                              // make the value octal and incorrect.
//
//  result = bdlt::TimeUtil::convertFromHHMMSSmmm(timeValue);
//
//  bsl::cout << result << bsl::endl;
//..
// The code above produces the following on 'stdout':
//..
//  03:32:24.832
//..
// Now, we demonstrate how 'bdlt::TimeUtil' provides methods that can be used
// to validate integral time values before passing them to the various
// "convert" methods.  For example:
//..
//  assert( bdlt::TimeUtil::isValidHHMMSSmmm(timeValue));
//..
// Finally, we demonstrate catching an invalid time value, 12:61:02.789 pm:
//..
//  //         format: HHMMSSmmm
//  int badTimeValue = 126102789;
//
//  assert(!bdlt::TimeUtil::isValidHHMMSSmmm(badTimeValue));
//..
//
///Example 2
///- - - - -
// The following snippet of code demonstrates how to use 'bdlt::TimeUtil' to
// convert from a 'bdlt::Time' to an integer representation of time in "HHMM",
// "HHMMSS", and "HHMMSSmmm" formats:
//..
//  bdlt::Time time(12, 45, 2, 789);
//  int        timeHHMM      = bdlt::TimeUtil::convertToHHMM(time);
//  int        timeHHMMSS    = bdlt::TimeUtil::convertToHHMMSS(time);
//  int        timeHHMMSSmmm = bdlt::TimeUtil::convertToHHMMSSmmm(time);
//
//  bsl::cout << "Time in HHMM:      " << timeHHMM      << bsl::endl;
//  bsl::cout << "Time in HHMMSS:    " << timeHHMMSS    << bsl::endl;
//  bsl::cout << "Time in HHMMSSmmm: " << timeHHMMSSmmm << bsl::endl;
//..
// The code above produces the following on 'stdout':
//..
//  Time in HHMM:      1245
//  Time in HHMMSS:    124502
//  Time in HHMMSSmmm: 124502789
//..
// Note that the millisecond and/or second fields of 'bdlt::Time' are ignored
// depending on the conversion method that is called.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_TIME
#include <bdlt_time.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bdlt {

                                // ===============
                                // struct TimeUtil
                                // ===============

struct TimeUtil {
    // This 'struct' provides a namespace for common non-primitive procedures
    // that operate on 'Time' objects.  These methods are alias-safe and
    // exception-neutral.

  private:
    // PRIVATE TYPES
    enum {
        k_HHMMSSMMM_HH_FACTOR = 10000000,
        k_HHMMSSMMM_MM_FACTOR =   100000,
        k_HHMMSSMMM_SS_FACTOR =     1000,

        k_HHMMSS_HH_FACTOR    =    10000,
        k_HHMMSS_MM_FACTOR    =      100,

        k_HHMM_HH_FACTOR      =      100
    };

  public:
    // CLASS METHODS
    static Time convertFromHHMM(int timeValue);
        // Return the 'bdlt::Time' value corresponding to the specified
        // 'timeValue', where 'timeValue' is a non-negative integer that, when
        // expressed in decimal notation, contains exactly four digits
        // (counting leading zeros, if any): two digits for the hour and two
        // digits for the minute.  For example, 309 is converted to
        // 'Time(3, 9)' (03:09:00.000).  More formally, 'timeValue' is
        // interpreted as:
        //..
        //  hour * 100 + minute
        //..
        // The behavior is undefined unless 'timeValue' represents a valid time
        // in the allowable range for 'bdlt::Time'
        // (00:00:00.000 - 23:59:00.000, and 24:00:00.000).

    static Time convertFromHHMMSS(int timeValue);
        // Return the 'bdlt::Time' value corresponding to the specified
        // 'timeValue', where 'timeValue' is a non-negative integer that, when
        // expressed in decimal notation, contains exactly six digits (counting
        // leading zeros, if any): two digits for the hour, two digits for the
        // minute, and two digits for the second.  For example, 30907 is
        // converted to 'Time(3, 9, 7)' (03:09:07.000).  More formally,
        // 'timeValue' is interpreted as:
        //..
        //  hour * 10000 + minute * 100 + second
        //..
        // The behavior is undefined unless 'timeValue' represents a valid time
        // in the allowable range for 'bdlt::Time'
        // (00:00:00.000 - 23:59:59.000, and 24:00:00.000).

    static Time convertFromHHMMSSmmm(int timeValue);
        // Return the 'bdlt::Time' value corresponding to the specified
        // 'timeValue', where 'timeValue' is a non-negative integer that, when
        // expressed in decimal notation, contains exactly nine digits
        // (counting leading zeros, if any): two digits for the hour, two
        // digits for the minute, two digits for the second, and three digits
        // for the millisecond.  For example, 30907056 is converted to
        // 'Time(3, 9, 7, 56)' (03:09:07.056).  More formally, 'timeValue' is
        // interpreted as:
        //..
        //  hour * 10000000 + minute * 100000 + second * 1000 + millisecond
        //..
        // The behavior is undefined unless 'timeValue' represents a valid time
        // in the allowable range for 'bdlt::Time'
        // (00:00:00.000 - 23:59:59.999, and 24:00:00.000).

    static int convertToHHMM(const Time& value);
        // Return the non-negative integer representing the same time as the
        // specified 'value' that, when expressed in decimal notation, contains
        // exactly four digits (counting leading zeros, if any): two digits for
        // the hour and two digits for the minute.  For example,
        // 'Time(3, 9, sec, ms)', where '0 <= sec < 60' and '0 <= ms < 1000',
        // is converted to 309.  More formally, this method returns:
        //..
        //  value.hour() * 100 + value.minute()
        //..

    static int convertToHHMMSS(const Time& value);
        // Return the non-negative integer representing the same time as the
        // specified 'value' that, when expressed in decimal notation, contains
        // exactly six digits (counting leading zeros, if any): two digits for
        // the hour, two digits for the minute, and two digits for the second.
        // For example, 'Time(3, 9, 7, ms)', where '0 <= ms < 1000', is
        // converted to 30907.  More formally, this method returns:
        //..
        //  value.hour() * 10000 + value.minute() * 100 + value.second()
        //..

    static int convertToHHMMSSmmm(const Time& value);
        // Return the non-negative integer representing the same time as the
        // specified 'value' that, when expressed in decimal notation, contains
        // exactly nine digits (counting leading zeros, if any): two digits for
        // the hour, two digits for the minute, two digits for the second, and
        // three digits for the millisecond.  For example, 'Time(3, 9, 7, 56)'
        // is converted to 30907056.  More formally, this method returns:
        //..
        //  value.hour() * 10000000 + value.minute() * 100000
        //                          + value.second() * 1000
        //                          + value.millisecond()
        //..

    static bool isValidHHMM(int timeValue);
        // Return 'true' if the specified 'timeValue' is a non-negative integer
        // that represents a valid four-digit time value suitable for passing
        // to 'convertFromHHMM', and 'false' otherwise.  'timeValue' is a valid
        // four-digit time value if, when expressed in decimal notation, it
        // contains exactly four digits (counting leading zeros, if any): two
        // digits for the hour and two digits for the minute, where either
        // '0 <= hour < 24' and '0 <= minute < 60', or '2400 == timeValue'.

    static bool isValidHHMMSS(int timeValue);
        // Return 'true' if the specified 'timeValue' is a non-negative integer
        // that represents a valid six-digit time value suitable for passing to
        // 'convertFromHHMMSS', and 'false' otherwise.  'timeValue' is a valid
        // six-digit time value if, when expressed in decimal notation, it
        // contains exactly six digits (counting leading zeros, if any): two
        // digits for the hour, two digits for the minute, and two digits for
        // the second, where either '0 <= hour < 24', '0 <= minute < 60', and
        // '0 <= second < 60', or '240000 == timeValue'.

    static bool isValidHHMMSSmmm(int timeValue);
        // Return 'true' if the specified 'timeValue' is a non-negative integer
        // that represents a valid nine-digit time value suitable for passing
        // to 'convertFromHHMMSSmmm', and 'false' otherwise.  'timeValue' is a
        // valid nine-digit time value if, when expressed in decimal notation,
        // it contains exactly nine digits (counting leading zeros, if any):
        // two digits for the hour, two digits for the minute, two digits for
        // the second, and three digits for the millisecond, where either
        // '0 <= hour < 24', '0 <= minute < 60', '0 <= second < 60', and
        // '0 <= millisecond < 1000', or '240000000 == timeValue'.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                               // ---------------
                               // struct TimeUtil
                               // ---------------

                              // -----------------
                              // Level-0 Functions
                              // -----------------

// CLASS METHODS
inline
bool TimeUtil::isValidHHMM(int timeValue)
{
    return Time::isValid(timeValue / k_HHMM_HH_FACTOR,
                         timeValue % k_HHMM_HH_FACTOR);
}

inline
bool TimeUtil::isValidHHMMSS(int timeValue)
{
    return Time::isValid(timeValue / k_HHMMSS_HH_FACTOR,
                        (timeValue % k_HHMMSS_HH_FACTOR) / k_HHMMSS_MM_FACTOR,
                         timeValue % k_HHMMSS_MM_FACTOR);
}

inline
bool TimeUtil::isValidHHMMSSmmm(int timeValue)
{
    return Time::isValid(
                    timeValue / k_HHMMSSMMM_HH_FACTOR,
                   (timeValue % k_HHMMSSMMM_HH_FACTOR) / k_HHMMSSMMM_MM_FACTOR,
                   (timeValue % k_HHMMSSMMM_MM_FACTOR) / k_HHMMSSMMM_SS_FACTOR,
                    timeValue % k_HHMMSSMMM_SS_FACTOR);
}

                            // -------------------
                            // All Other Functions
                            // -------------------

inline
Time TimeUtil::convertFromHHMM(int timeValue)
{
    BSLS_ASSERT_SAFE(TimeUtil::isValidHHMM(timeValue));

    return Time(timeValue / k_HHMM_HH_FACTOR,
                timeValue % k_HHMM_HH_FACTOR);
}

inline
Time TimeUtil::convertFromHHMMSS(int timeValue)
{
    BSLS_ASSERT_SAFE(TimeUtil::isValidHHMMSS(timeValue));

    return Time(timeValue / k_HHMMSS_HH_FACTOR,
               (timeValue % k_HHMMSS_HH_FACTOR) / k_HHMMSS_MM_FACTOR,
                timeValue % k_HHMMSS_MM_FACTOR);
}

inline
Time TimeUtil::convertFromHHMMSSmmm(int timeValue)
{
    BSLS_ASSERT_SAFE(TimeUtil::isValidHHMMSSmmm(timeValue));

    return Time(timeValue / k_HHMMSSMMM_HH_FACTOR,
               (timeValue % k_HHMMSSMMM_HH_FACTOR) / k_HHMMSSMMM_MM_FACTOR,
               (timeValue % k_HHMMSSMMM_MM_FACTOR) / k_HHMMSSMMM_SS_FACTOR,
                timeValue % k_HHMMSSMMM_SS_FACTOR);
}

inline
int TimeUtil::convertToHHMM(const Time& value)
{
    return value.hour() * k_HHMM_HH_FACTOR + value.minute();
}

inline
int TimeUtil::convertToHHMMSS(const Time& value)
{
    return value.hour()   * k_HHMMSS_HH_FACTOR
         + value.minute() * k_HHMMSS_MM_FACTOR
         + value.second();
}

inline
int TimeUtil::convertToHHMMSSmmm(const Time& value)
{
    return value.hour()   * k_HHMMSSMMM_HH_FACTOR
         + value.minute() * k_HHMMSSMMM_MM_FACTOR
         + value.second() * k_HHMMSSMMM_SS_FACTOR
         + value.millisecond();
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
