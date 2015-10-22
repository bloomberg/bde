// bdlt_delegatingdateimputil.h                                       -*-C++-*-
#ifndef INCLUDED_BDLT_DELEGATINGDATEIMPUTIL
#define INCLUDED_BDLT_DELEGATINGDATEIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide low-level support functions for date-value manipulation.
//
//@INTERNAL_DEPRECATED: Do *NOT* use.
//
//@CLASSES:
//  bdlt::DelegatingDateImpUtil: low-level date-related stateless functions
//
//@SEE_ALSO: bdlt_posixdateimputil, bdlt_serialdateimputil
//
//@DESCRIPTION: This component provides a utility 'struct',
// 'bdlt::DelegatingDateImpUtil', that serves as a "switch" between the two
// underlying utilities 'bdlt::SerialDateImpUtil' (whose functionality is
// consistent with the proleptic Gregorian calendar) and
// 'bdlt::PosixDateImpUtil' (which is consistent with the POSIX 'cal' command).
// 'bdlt::DelegatingDateImpUtil' has a two-state calendar mode that is in
// either POSIX mode or proleptic Gregorian mode.  When the calendar mode in
// effect is POSIX (proleptic Gregorian), functions in
// 'bdlt::DelegatingDateImpUtil' delegate to corresponding functions in
// 'bdlt::PosixDateImpUtil' ('bdlt::SerialDateImpUtil').

// The calendar mode is set by the 'disableProlepticGregorianMode' and
// 'enableProlepticGregorianMode' functions, and queried by the
// 'isProlepticGregorianMode' function (which returns 'true' for proleptic
// Gregorian and 'false' for POSIX).  Expected usage is that the calendar mode
// will be established *once*, early in 'main', and not changed throughout the
// lifetime of a process.
//
// This component is meant for *internal* *use* *only*.  Do *NOT* use unless
// you know what you are doing.
//
///Usage
///-----
// Since this component is meant for *internal* *use* *only* , there is no
// usage example.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_POSIXDATEIMPUTIL
#include <bdlt_posixdateimputil.h>
#endif

#ifndef INCLUDED_BDLT_SERIALDATEIMPUTIL
#include <bdlt_serialdateimputil.h>
#endif

namespace BloombergLP {
namespace bdlt {

                           // ============================
                           // struct DelegatingDateImpUtil
                           // ============================

struct DelegatingDateImpUtil {
    // Each function defined in this utility 'struct', with the exception of
    // the "Calendar Mode" functions, delegates to the corresponding function
    // in 'bdlt::SerialDateImpUtil' or 'bdlt::PosixDateImpUtil' according to
    // the value returned by 'isProlepticGregorianMode'.

  private:
    // CLASS DATA
    static bool s_prolepticGregorianModeFlag;  // 'true' if proleptic
                                               // Gregorian, 'false' if POSIX

  public:
    // CLASS METHODS
    static bool isLeapYear(int year);
        // Return 'true' if the specified 'year' is a leap year, and 'false'
        // otherwise.  The behavior is undefined unless '1 <= year <= 9999'.

    static int lastDayOfMonth(int year, int month);
        // Return the last day of the specified 'month' in the specified
        // 'year'.  The behavior is undefined unless '1 <= year <= 9999' and
        // '1 <= month <= 12'.  Note that the value returned will be in the
        // range '[28 .. 31]'.

    static int numLeapYears(int year1, int year2);
        // Return the number of leap years occurring between the specified
        // 'year1' and 'year2', inclusive.  The behavior is undefined unless
        // '1 <= year1 <= 9999', '1 <= year2 <= 9999', and 'year1 <= year2'.

                        // Is Valid Date

    static bool isValidSerial(int serialDay);
        // Return 'true' if the specified 'serialDay' represents a valid date
        // value, and 'false' otherwise.  Note that valid date values, which
        // depend on the calendar mode that is in effect, are in the range
        // '[1 .. 3652059]' (proleptic Gregorian mode) or '[1 .. 3652061]'
        // (POSIX mode).  See the component-level documentation of
        // 'bdlt_posixdateimputil' and 'bdlt_serialdateimputil' for details.

    static bool isValidYearDay(int year, int dayOfYear);
        // Return 'true' if the specified 'year' and 'dayOfYear' represent a
        // valid date value, and 'false' otherwise.  Note that valid date
        // values, which depend on the calendar mode that is in effect, are in
        // the range '[0001/001 .. 9999/366]'.  See the component-level
        // documentation of 'bdlt_posixdateimputil' and
        // 'bdlt_serialdateimputil' for details.

    static bool isValidYearMonthDay(int year, int month, int day);
        // Return 'true' if the specified 'year', 'month', and 'day' represent
        // a valid date value, and 'false' otherwise.  Note that valid date
        // values, which depend on the calendar mode that is in effect, are in
        // the range '[0001/01/01 .. 9999/12/31]'.  See the component-level
        // documentation of 'bdlt_posixdateimputil' and
        // 'bdlt_serialdateimputil' for details.

    static bool isValidYearMonthDayNoCache(int year, int month, int day);
        // Return 'true' if the specified 'year', 'month', and 'day' represent
        // a valid date value,  and 'false' otherwise.  Note that valid date
        // values, which depend on the calendar mode that is in effect, are in
        // the range '[0001/01/01 .. 9999/12/31]'.  See the component-level
        // documentation of 'bdlt_posixdateimputil' and
        // 'bdlt_serialdateimputil' for details.  Also note that this function
        // is guaranteed not to use any date-cache optimizations.

                        // To Serial Date (s)

    static int ydToSerial(int year, int dayOfYear);
        // Return the serial date representation of the date value indicated
        // by the specified 'year' and 'dayOfYear'.  The behavior is undefined
        // unless 'isValidYearDay(year, dayOfYear)' returns 'true'.

    static int ymdToSerial(int year, int month, int day);
        // Return the serial date representation of the date value indicated by
        // the specified 'year', 'month', and 'day'.  The behavior is undefined
        // unless 'isValidYearMonthDay(year, month, day)' returns 'true'.

    static int ymdToSerialNoCache(int year, int month, int day);
        // Return the serial date representation of the date value indicated
        // by the specified 'year', 'month', and 'day'.  The behavior is
        // undefined unless 'isValidYearMonthDay(year, month, day)' returns
        // 'true'.  Note that this function is guaranteed not to use any
        // date-cache optimizations.

                        // To Day-Of-Year Date (yd)

    static int serialToDayOfYear(int serialDay);
        // Return the day of the year of the date value indicated by the
        // specified 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.

    static void serialToYd(int *year, int *dayOfYear, int serialDay);
        // Load, into the specified 'year' and 'dayOfYear', the year-day
        // representation of the date value indicated by the specified
        // 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.

    static int ymdToDayOfYear(int year, int month, int day);
        // Return the day of the year of the date value indicated by the
        // specified 'year', 'month', and 'day'.  The behavior is undefined
        // unless 'isValidYearMonthDay(year, month, day)' returns 'true'.

                        // To Calendar Date (ymd)

    static int serialToDay(int serialDay);
        // Return the day (of the month) of the date value indicated by the
        // specified 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.

    static int serialToDayNoCache(int serialDay);
        // Return the day (of the month) of the date value indicated by the
        // specified 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.  Note that this function
        // is guaranteed not to use any date-cache optimizations.

    static int serialToMonth(int serialDay);
        // Return the month of the date value indicated by the specified
        // 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.

    static int serialToMonthNoCache(int serialDay);
        // Return the month of the date value indicated by the specified
        // 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.  Note that this function
        // is guaranteed not to use any date-cache optimizations.

    static int serialToYear(int serialDay);
        // Return the year of the date value indicated by the specified
        // 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.

    static int serialToYearNoCache(int serialDay);
        // Return the year of the date value indicated by the specified
        // 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.  Note that this function
        // is guaranteed not to use any date-cache optimizations.

    static void serialToYmd(int *year, int *month, int *day, int serialDay);
        // Load, into the specified 'year', 'month', and 'day', the date value
        // indicated by the specified 'serialDay'.  The behavior is undefined
        // unless 'isValidSerial(serialDay)' returns 'true'.

    static void serialToYmdNoCache(int *year,
                                   int *month,
                                   int *day,
                                   int  serialDay);
        // Load, into the specified 'year', 'month', and 'day', the date value
        // indicated by the specified 'serialDay'.  The behavior is undefined
        // unless 'isValidSerial(serialDay)' returns 'true'.  Note that this
        // function is guaranteed not to use any date-cache-optimizations.

    static int ydToDay(int year, int dayOfYear);
        // Return the day (of the month) of the date value indicated by the
        // specified 'year' and 'dayOfYear'.  The behavior is undefined unless
        // 'isValidYearDay(year, dayOfYear)' returns 'true'.

    static void ydToMd(int *month, int *day, int year, int dayOfYear);
        // Load, into the specified 'month' and 'day', the (partial) calendar
        // date representation of the date value indicated by the specified
        // 'year' and 'dayOfYear'.  The behavior is undefined unless
        // 'isValidYearDay(year, dayOfYear)' returns 'true'.

    static int ydToMonth(int year, int dayOfYear);
        // Return the month of the date value indicated by the specified 'year'
        // and 'dayOfYear'.  The behavior is undefined unless
        // 'isValidYearDay(year, dayOfYear)' returns 'true'.

                        // To Day of Week '[SUN = 1, MON .. SAT]'

    static int serialToDayOfWeek(int serialDay);
        // Return, as an integer (with '1 = SUN', '2 = MON', ..., '7 = SAT'),
        // the day (of the week) of the date value indicated by the specified
        // 'serialDay'.  The behavior is undefined unless
        // 'isValidSerial(serialDay)' returns 'true'.

    static int ydToDayOfWeek(int year, int dayOfYear);
        // Return, as an integer (with '1 = SUN', '2 = MON', ..., '7 = SAT'),
        // the day (of the week) of the date value indicated by the specified
        // 'year' and 'dayOfYear'.  The behavior is undefined unless
        // 'isValidYearDay(year, dayOfYear)' returns 'true'.

    static int ymdToDayOfWeek(int year, int month, int day);
        // Return, as an integer (with '1 = SUN', '2 = MON', ..., '7 = SAT'),
        // the day (of the week) of the date value indicated by the specified
        // 'year', 'month', and 'day'.  The behavior is undefined unless
        // 'isValidYearMonthDay(year, month, day)' returns 'true'.

                        // Calendar Mode

    static void disableProlepticGregorianMode();
        // Configure this utility to be in POSIX mode.  This function has no
        // effect if this utility is already in POSIX mode.  Do *not* call this
        // method unless you know what you are doing.  The behavior is
        // undefined if the task has started multiple threads.

    static void enableProlepticGregorianMode();
        // Configure this utility to be in proleptic Gregorian mode.  This
        // function has no effect if this utility is already in proleptic
        // Gregorian mode.  Do *not* call this method unless you know what you
        // are doing.  The behavior is undefined if the task has started
        // multiple threads.

    static bool isProlepticGregorianMode();
        // Return 'true' if this utility is configured to be in proleptic
        // Gregorian mode, and 'false' otherwise.  Note that a return value of
        // 'false' implies that this utility is in POSIX mode.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                           // ----------------------------
                           // struct DelegatingDateImpUtil
                           // ----------------------------

// CLASS METHODS
inline
bool DelegatingDateImpUtil::isLeapYear(int year)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::isLeapYear(year)
           : PosixDateImpUtil::isLeapYear(year);
}

inline
int DelegatingDateImpUtil::lastDayOfMonth(int year, int month)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::lastDayOfMonth(year, month)
           : PosixDateImpUtil::lastDayOfMonth(year, month);
}

inline
int DelegatingDateImpUtil::numLeapYears(int year1, int year2)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::numLeapYears(year1, year2)
           : PosixDateImpUtil::numLeapYears(year1, year2);
}

                        // Is Valid Date

inline
bool DelegatingDateImpUtil::isValidSerial(int serialDay)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::isValidSerial(serialDay)
           : PosixDateImpUtil::isValidSerial(serialDay);
}

inline
bool DelegatingDateImpUtil::isValidYearDay(int year, int dayOfYear)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::isValidYearDay(year, dayOfYear)
           : PosixDateImpUtil::isValidYearDay(year, dayOfYear);
}

inline
bool DelegatingDateImpUtil::isValidYearMonthDay(int year, int month, int day)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::isValidYearMonthDay(year, month, day)
           : PosixDateImpUtil::isValidYearMonthDay(year, month, day);
}

inline
bool DelegatingDateImpUtil::isValidYearMonthDayNoCache(int year,
                                                       int month,
                                                       int day)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::isValidYearMonthDayNoCache(year, month, day)
           : PosixDateImpUtil::isValidYearMonthDayNoCache(year, month, day);
}

                        // To Serial Date (s)

inline
int DelegatingDateImpUtil::ydToSerial(int year, int dayOfYear)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::ydToSerial(year, dayOfYear)
           : PosixDateImpUtil::ydToSerial(year, dayOfYear);
}

inline
int DelegatingDateImpUtil::ymdToSerial(int year, int month, int day)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::ymdToSerial(year, month, day)
           : PosixDateImpUtil::ymdToSerial(year, month, day);
}

inline
int DelegatingDateImpUtil::ymdToSerialNoCache(int year, int month, int day)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::ymdToSerialNoCache(year, month, day)
           : PosixDateImpUtil::ymdToSerialNoCache(year, month, day);
}

                        // To Day-Of-Year Date (yd)

inline
int DelegatingDateImpUtil::serialToDayOfYear(int serialDay)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::serialToDayOfYear(serialDay)
           : PosixDateImpUtil::serialToDayOfYear(serialDay);
}

inline
void DelegatingDateImpUtil::serialToYd(int *year,
                                       int *dayOfYear,
                                       int  serialDay)
{
    isProlepticGregorianMode()
    ? SerialDateImpUtil::serialToYd(year, dayOfYear, serialDay)
    : PosixDateImpUtil::serialToYd(year, dayOfYear, serialDay);
}

inline
int DelegatingDateImpUtil::ymdToDayOfYear(int year, int month, int day)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::ymdToDayOfYear(year, month, day)
           : PosixDateImpUtil::ymdToDayOfYear(year, month, day);
}

                        // To Calendar Date (ymd)

inline
int DelegatingDateImpUtil::serialToDay(int serialDay)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::serialToDay(serialDay)
           : PosixDateImpUtil::serialToDay(serialDay);
}

inline
int DelegatingDateImpUtil::serialToDayNoCache(int serialDay)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::serialToDayNoCache(serialDay)
           : PosixDateImpUtil::serialToDayNoCache(serialDay);
}

inline
int DelegatingDateImpUtil::serialToMonth(int serialDay)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::serialToMonth(serialDay)
           : PosixDateImpUtil::serialToMonth(serialDay);
}

inline
int DelegatingDateImpUtil::serialToMonthNoCache(int serialDay)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::serialToMonthNoCache(serialDay)
           : PosixDateImpUtil::serialToMonthNoCache(serialDay);
}

inline
int DelegatingDateImpUtil::serialToYear(int serialDay)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::serialToYear(serialDay)
           : PosixDateImpUtil::serialToYear(serialDay);
}

inline
int DelegatingDateImpUtil::serialToYearNoCache(int serialDay)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::serialToYearNoCache(serialDay)
           : PosixDateImpUtil::serialToYearNoCache(serialDay);
}

inline
void DelegatingDateImpUtil::serialToYmd(int *year,
                                        int *month,
                                        int *day,
                                        int  serialDay)
{
    isProlepticGregorianMode()
    ? SerialDateImpUtil::serialToYmd(year, month, day, serialDay)
    : PosixDateImpUtil::serialToYmd(year, month, day, serialDay);
}

inline
void DelegatingDateImpUtil::serialToYmdNoCache(int *year,
                                               int *month,
                                               int *day,
                                               int  serialDay)
{
    isProlepticGregorianMode()
    ? SerialDateImpUtil::serialToYmdNoCache(year, month, day, serialDay)
    : PosixDateImpUtil::serialToYmdNoCache(year, month, day, serialDay);
}

inline
int DelegatingDateImpUtil::ydToDay(int year, int dayOfYear)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::ydToDay(year, dayOfYear)
           : PosixDateImpUtil::ydToDay(year, dayOfYear);
}

inline
void DelegatingDateImpUtil::ydToMd(int *month,
                                   int *day,
                                   int  year,
                                   int  dayOfYear)
{
    isProlepticGregorianMode()
    ? SerialDateImpUtil::ydToMd(month, day, year, dayOfYear)
    : PosixDateImpUtil::ydToMd(month, day, year, dayOfYear);
}

inline
int DelegatingDateImpUtil::ydToMonth(int year, int dayOfYear)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::ydToMonth(year, dayOfYear)
           : PosixDateImpUtil::ydToMonth(year, dayOfYear);
}

                        // To Day of Week '[SUN = 1, MON .. SAT]'

inline
int DelegatingDateImpUtil::serialToDayOfWeek(int serialDay)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::serialToDayOfWeek(serialDay)
           : PosixDateImpUtil::serialToDayOfWeek(serialDay);
}

inline
int DelegatingDateImpUtil::ydToDayOfWeek(int year, int dayOfYear)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::ydToDayOfWeek(year, dayOfYear)
           : PosixDateImpUtil::ydToDayOfWeek(year, dayOfYear);
}

inline
int DelegatingDateImpUtil::ymdToDayOfWeek(int year, int month, int day)
{
    return isProlepticGregorianMode()
           ? SerialDateImpUtil::ymdToDayOfWeek(year, month, day)
           : PosixDateImpUtil::ymdToDayOfWeek(year, month, day);
}

                        // Calendar Mode

inline
bool DelegatingDateImpUtil::isProlepticGregorianMode()
{
    return s_prolepticGregorianModeFlag;
}

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
