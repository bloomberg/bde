// bdetu_datetime.h                                                   -*-C++-*-
#ifndef INCLUDED_BDETU_DATETIME
#define INCLUDED_BDETU_DATETIME

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide common non-primitive operations on 'bdet_Datetime'.
//
//@CLASSES:
//   bdetu_Datetime: non-primitive pure procedures on 'bdet_Datetime'
//
//@AUTHOR: John Lakos (jlakos)
//
//@SEE_ALSO: bdet_datetime, bdet_datetimeinterval, bdet_epoch
//
//@DESCRIPTION: This component provides non-primitive operations on
// 'bdet_Datetime' objects.  In particular, 'bdetu_Datetime' supplies
// conversions of universal time to and from the C-standard 'typedef' 'time_t'
// (DEPRECATED, see 'bdetu_Epoch') and 'struct' 'tm' representations.
//
///SYNOPSIS
///--------
// This utility component provides the following (static) methods:
//..
//   int convertToTimeT(bsl::time_t *result, const bdet_Datetime& datetime);
//   void convertFromTimeT(bdet_Datetime *result, bsl::time_t time);
//     // DEPRECATED
//
//   void convertToTm(tm *result, const bdet_Datetime& datetime);
//   int convertFromTm(bdet_Datetime *result, const tm& timeStruct);
//..
///USAGE EXAMPLE: Converting To and From Local Time
///------------------------------------------------
// Conversion from system time (universally referenced to Coordinated
// Universal Time (UTC)) to date and time in the geographical local time
// zone ("locale") is known to be a complex problem.  Among the issues are:
//..
//     (1) The geographical boundaries that constitute a timezone are both
//         complex and mutable.
//
//     (2) A timezone is not just a fixed offset from UTC, but rather an offset
//         that *may* vary periodically; Daylight Saving Time (DST) is commonly
//         (but not universally) observed in many locales.  Moreover, the rules
//         that govern DST can and do change at the discretion of local
//         governments.
//
//     (3) The very meaning of "local timezone" itself becomes problematic in
//         a distributed computing environment.  We can rely on an OS to know
//         its *own* local time, but that value may be of little use to the
//         running client code, since the locale of the CPU might be the result
//         of an arbitrary selection made by some global router efficiently
//         balancing the load across a worldwide network.
//..
// Adjusting for *regular* definitions of DST in different locales is easy;
// (past) historical variations are slightly more cumbersome to account for,
// but are not truly difficult; predicting the future is known to be a hard
// problem.  Among the known regular variations are the following conventions
// and historical variations.
//
// For countries in the European Union (EU), DST (since 1996) begins and ends
// at 1AM UTC, starting on the last Sunday in March and ending on the last
// Sunday in October.  In these countries, local time jumps ahead one hour in
// the spring and jumps back one hour in the fall, at 1AM UTC, whatever the
// local time might be.
//
// In the United States, for those locales that observe it, DST (since 1986)
// begins and ends at 2AM *local* *time*, starting on the first Sunday in April
// and also ending on the last Sunday in October.  In the United States, local
// time "springs forward" from 1:59AM to 3AM in the spring and "falls back"
// from 1:59AM to 1AM in the fall.
//
// For Australia and several other countries in the southern hemisphere, the
// strategies for calculating DST are similar, but swapped -- i.e., DST is in
// effect in December, but not in June.  Many other variations exist, including
// non-integral hour offsets.
//
// Note that prior to 1986 (Public Law-99-359), DST in the US began with the
// last Sunday in April, not the first.  Also note that due to the energy
// crisis, DST was observed year round in 1973, for 10 months in 1974, and for
// 8 months in 1975.
//
// Limiting ourselves to the locales and rules given explicitly above,
// converting from UTC to local time is a non-trivial but in principle
// straightforward function of both the locale and the time itself -- the
// conversion is only problematic when the locale of the end user is not that
// of the computer.
//
// Unfortunately, conversion from local time to UTC in general requires an
// explicit qualification indicating whether or not DST is in effect.
// Specifically, the change *to* DST creates each year an hour-long interval of
// (unqualified) local-time *representations* that are invalid, i.e., a
// syntactically valid "time" that does not correspond to any UTC value.
// Similarly, the change *from* DST creates a two-hour-long period where each
// (unqualified) value for the local time maps onto two distinct UTC values,
// separated by one hour.  Adding the appropriate DST qualifier removes this
// representational ambiguity, but this information is often simply not
// available.
//
// As part of this example, we will create (in 'my_TimeUtil') a function
//..
//   static
//   int convertGmtToLocalTime(bdet_Datetime                *result,
//                             bsl::time_t                   universalTime,
//                             const bdet_DatetimeInterval&  timezoneOffset,
//                             my_TimeUtil::DstMode          mode);
//..
// that converts from UTC expressed as a 'bsl::time_t' to a local
// 'bdet_Datetime'.  The standard local-time offest from UTC ('timezoneOffset')
// as well as the mode ('DstMode') of daylight saving time (e.g., 'NONE', 'EU',
// or 'US') are also supplied.  The following specific examples illustrate the
// expected result with initial universal times corresponding to 12 noon, June
// 30, 2005 and 12 noon, December 30, 2005 UTC, respectively:
//..
//                                          __________________________________
//                                          |  Local Times Corresponding To  |
//                          Offset   DST*   |  Initial Universal Time (UTC)  |
// Geographical Location   From UTC  Mode   2005/6/30 12:00   2005/12/30 12:00
// ---------------------   --------  ----   ---------------   ----------------
// Chatham Is New Zealand    +12:45  NONE   2005/6/31  0:45   2005/12/31  0:45
// Auckland, New Zealand     +12:00  (TBD)  2005/6/31  0:00   2005/12/31  1:00*
// Fiji, Pacific             +12:00  NONE   2005/6/31  0:00   2005/12/31  0:00
// Norfolk Is, Pacific       +11:30  NONE   2005/6/30 23:30   2005/12/30 23:30
// Sidney, Australia         +10:00  (TBD)  2005/6/30 22:00   2005/12/30 23:00*
// Adelaide, Australia        +9:30  (TBD)  2005/6/30 23:30   2005/12/31 00:30*
// Perth, Australia           +8:00  NONE   2005/6/30 20:00   2005/12/30 20:00
// Kabul, Afghanistan         +4:30  NONE   2005/6/30 16:30   2005/12/30 16:30
// Cairo, Egypt               +2:00  NONE   2005/6/30 14:00   2005/12/30 14:00
// London, United Kingdom      0:00    EU   2005/6/30 12:00   2005/12/30 12:00
// New York, USA              -5:00    US   2005/6/30  8:00*  2005/12/30  7:00
// Virgin Islands, USA        -6:00  NONE   2005/6/30  8:00   2005/12/30  8:00
// California, USA            -8:00    US   2005/6/30  5:00*  2005/12/30  4:00
// Hawaii, USA               -10:00  NONE   2005/6/30  2:00*  2005/12/30  2:00
// Midway Is, Pacific        -11:00  NONE   2005/6/30  1:00*  2005/12/30  1:00
//
// Ref: 'www.timezoneconverter.com' and 'www.swissinfo.net/cgi/worldtime'
//..
// (See also the 'IC' command on the Bloomberg Professional Service.)
//
// If we wanted to hard-code local time for New York City (which is 5 hours
// ahead and observes DST), it might look like this:
//..
//  int toNycLocalTime(bdet_Datetime *result, bsl::time_t universalTime)
//  {
//      return my_TimeUtil::convertGmtToLocalTime(result,
//                                                universalTime,
//                                                bdet_DatetimeInterval(0, -5),
//                                                my_TimeUtil::US);
//  }
//  bdet_Datetime result;
//
//  int dst1 = toNycLocalTime(&result, bdet_Datetime(2005,  6, 30, 12));
//  assert(1 == dst1); assert(bdet_Datetime(2005,  6, 30,  8) == result);
//
//  int dst2 = toNycLocalTime(&result, bdet_Datetime(2005, 12, 30, 12));
//  assert(0 == dst2); assert(bdet_Datetime(2005, 12, 30,  7) == result);
//..
// In a real application, however, we would want to obtain the appropriate
// settings from data indicating the current user preference:
//..
//  my_TimeUtil::DstMode getMyLocalPreference(bdet_DatetimeInterval *);
//      // Implementation not shown.
//
//  int toLocalTime(bdet_Datetime *result, bsl::time_t universalTime)
//  {
//      bdet_DatetimeInterval tz;
//      my_TimeUtil::DstMode dm = getMyLocalPreference(&tz);
//
//      // Assume dm and tz are valid.
//
//      return my_TimeUtil::convertGmtToLocalTime(result, universalTime, tz,
//                                                                       dm);
//  }
//..
// For simplicity, we will initially assume that the valid range for our
// 'toLocalTime' function is any UTC after midnight, January 1, 1996 through
// January 19, 2038 at 3:14:07AM (corresponding to the maximum value of
// 'bsl::time_t' currently available on all platforms).  Initially, we will
// support only three DST modes: NONE, EU, and US; however, the 'toLocalTime'
// function is easily extensible to wider time periods, as well as additional
// DST modes.
//
// First we define a 'struct', called 'my_TimeUtil', that will act as a
// namespace for the hierarchical suite of functions needed to implement
// 'toLocalTime':
//..
//  struct my_TimeUtil {
//      // This 'struct' provides a namespace for functionality related to
//      // converting universal time to local time.
//
//
//      enum DstMode {
//          // Defines the mode in which Daylight Saving Time is observed.
//
//          NONE,       // not observed
//          EU,         // as observed by all countries in the European Union
//          US          // as observed (where observed) in the United States
//      };
//
//      static
//      int weekdayInMonth(int                 year,
//                         int                 month,
//                         bdet_DayOfWeek::Day dayOfWeek,
//                         int                 ordinal);
//          // Return the day of the specified 'month' of the specified
//          // 'year' corresponding to the specified 'ordinal' value of the
//          // specified 'dayOfWeek' in that month, or 0 if no such day exists.
//          // Absolute values for 'ordinal' begin with 1.  E.g.,
//          //..
//          //   weekdayInMonth(11, 2004, bdet_DayOfWeek::THURSDAY, 4)
//          //..
//          // requests the day in November, 2004 of the fourth Thursday
//          // (which is 25).  Making 'ordinal' negative indicates that its
//          // *absolute* *value* references a day counting from the *last*
//          // 'dayOfWeek' of the month rather than the first.  E.g.,
//          //..
//          //   weekdayInMonth(10, 2004, bdet_DayOfWeek::SUNDAY, -1)
//          //..
//          // requests the date of the last Sunday in October, 2004 (which
//          // is 31).  The behavior is undefined unless 1 <= 'year' <= 9999,
//          // 1 <= 'month' <= 12, -5 <= 'ordinal' <= 5, and 'ordinal' != 0.
//
//      static
//      bool isDaylightSavingTime(const bdet_Datetime&         universalTime,
//                                const bdet_DatetimeInterval& timezoneOffset,
//                                my_TimeUtil::DstMode         mode);
//          // Return 'true' if the specified 'universalTime', (referenced to
//          // UTC), if converted to local time as indicated by the specified
//          // 'timezoneOffset' and 'mode' of observing Daylight Saving
//          // Time, would represent a Daylight Saving Time (DST) value in
//          // the local time zone, and 'false' otherwise.  'timezoneOffset'
//          // represents the relative time difference between UTC and
//          // standard local time.  For example, Eastern Standard Time
//          // (EST) is consistently UTC - 5 hours.  That is, if it is noon
//          // in Greenwich (UTC), in New York it is 7AM EST (in the winter)
//          // and 8AM EDT (in the summer).  The behavior is undefined unless
//          // -12:00 <= 'timezoneOffset' <= 13:00.
//
//      static
//      int convertGmtToLocalTime(bdet_Datetime               *result,
//                                const bdet_Datetime&         universalTime,
//                                const bdet_DatetimeInterval& timezoneOffset,
//                                my_TimeUtil::DstMode         mode);
//      static
//      int convertGmtToLocalTime(bdet_Datetime               *result,
//                                bsl::time_t                  universalTime,
//                                const bdet_DatetimeInterval& timezoneOffset,
//                                my_TimeUtil::DstMode         mode);
//          // Convert the specified 'universalTime' (referenced to UTC)
//          // to the desired local time indicated by the specified
//          // 'timezoneOffset' and 'mode' of observing Daylight Saving
//          // Time.  Return 1 if 'result' is in Daylight Saving Time, and
//          // 0 if in Standard Time.  'timezoneOffset' represents the
//          // relative time difference between UTC and standard local time
//          // (see 'isDaylightSavingTime' for examples).  The behavior
//          // is undefined unless 1996/01/01_00:00:00 <= 'universalTime',
//          // 'universalTime' <= 2038/01/19_03:17:04, and
//          // -12:00 <= 'timezoneOffset' <= 12:00.
//  };
//..
// For completeness, we also provide the implementations of these functions.
//..
//  int my_TimeUtil::weekdayInMonth(int                 year,
//                                  int                 month,
//                                  bdet_DayOfWeek::Day dayOfWeek,
//                                  int                 ordinal)
//  {
//      assert(1 <= year);                  assert(year <= 9999);
//      assert(1 <= month);                 assert(month <= 12);
//      assert(-5 <= ordinal);              assert(ordinal <= 5);
//      assert(0 != ordinal);
//
//      enum { NONE = 0 }; // Returned when a requested fifth weekday does
//                         // not exist in the specified month and year.
//
//      int eom = bdeimp_DateUtil::lastDayOfMonth(year, month);
//      if (ordinal > 0) {                          // from start of month
//          int dow = bdeimp_DateUtil::ymd2weekday(year, month, 1);
//          int diff = (7 + dayOfWeek - dow) % 7;
//          int dom = 1 + diff; // First weekday in month is [1 .. 7].
//                                 assert(1 <= dom); assert(dom <= 7);
//          dom += 7 * (ordinal - 1);
//          return dom <= eom ? dom : NONE;
//      }
//      else {                                      // from end of month
//          int dow = bdeimp_DateUtil::ymd2weekday(year, month, eom);
//          int diff = (7 + dow - dayOfWeek) % 7;
//          int dom = eom - diff; // Last weekday in month is [22 .. 31].
//                                  assert(22 <= dom); assert(dom <= 31);
//          dom -= 7 * (-ordinal - 1);
//          return 1 <= dom ? dom : NONE;
//      }
//
//      assert("Programming Error: Unreachable!" && 0);
//  }
//
//  int my_TimeUtil::isDaylightSavingTime(
//                                 const bdet_Datetime&         universalTime,
//                                 const bdet_DatetimeInterval& timezoneOffset,
//                                 my_TimeUtil::DstMode         mode)
//  {
//      assert(bdet_Datetime(1996, 1, 1) <= universalTime);   // for now
//      assert(universalTime <= bdet_Datetime(2038, 1, 19, 3, 14, 7));
//      assert(bdet_DatetimeInterval(-12, 0) < timezoneOffset);
//      assert(timezoneOffset < bdet_DatetimeInterval(+13, 0));
//
//      enum {
//          FIRST   = +1,   // first <weekday> of the month
//          LAST    = -1,   // last <weekday> of the month
//          MARCH   = 3,    // 3rd month of the year
//          APRIL   = 4,    // 4th month of the year
//          OCTOBER = 10    // 10th month of the year
//      };
//
//      switch (mode) {
//        case NONE: {
//          return 0;
//        }; break;
//        case EU: {
//          const int m = universalTime.month(); // reference is UTC
//          if (MARCH < m && m < OCTOBER) {
//              return 1;
//          }
//          if (MARCH > m || m > OCTOBER) {
//              return 0;
//          }
//
//          int y, x, d;  // Getting all 3 is faster than getting just two.
//          universalTime.date().getYearMonthDay(&y, &x, &d);
//          assert(m == x);
//          int dom = weekdayInMonth(y, m, bdet_DayOfWeek::SUNDAY, LAST);
//
//          if (MARCH == m) {
//              if (d < dom) {
//                  return 0;
//              }
//              if (d > dom) {
//                  return 1;
//              }
//              return universalTime.hour() >= 1; // changes at 1AM UTC
//          }
//          else { assert(OCTOBER == m);
//              if (d < dom) {
//                  return 1;
//              }
//              if (d > dom) {
//                  return 0;
//              }
//              return universalTime.hour() < 1;  // changes at 1AM UTC
//          }
//        }; break;
//        case US: {
//          const bdet_Datetime& localStandardTime(universalTime +
//                                                 timezoneOffset);
//
//          const int m = localStandardTime.month(); // reference is local
//          if (APRIL < m && m < OCTOBER) {
//              return 1;
//          }
//          if (APRIL > m || m > OCTOBER) {
//              return 0;
//          }
//
//          int y, x, d;  // Getting all 3 is faster than getting just two.
//          localStandardTime.date().getYearMonthDay(&y, &x, &d);
//          assert(m == x);
//
//          if (APRIL == m) {
//              int dom = weekdayInMonth(y, m, bdet_DayOfWeek::SUNDAY, FIRST);
//              if (d < dom) {
//                  return 0;
//              }
//              if (d > dom) {
//                  return 1;
//              }
//              return localStandardTime.hour() >= 2; // 2AM local std time
//          }
//          else { assert(OCTOBER == m);
//              int dom = weekdayInMonth(y, m, bdet_DayOfWeek::SUNDAY, LAST);
//              if (d < dom) {
//                  return 1;
//              }
//              if (d > dom) {
//                  return 0;
//              }
//              return localStandardTime.hour() < 2;  // 2AM local std time
//          }
//        }; break;
//      }
//
//      assert("Unrecognized Enumeration Value!" && 0);
//  }
//
//  int my_TimeUtil::convertGmtToLocalTime(
//                          bdet_Datetime                *result,
//                          const bdet_Datetime&          universalTime,
//                          const bdet_DatetimeInterval&  timezoneOffset,
//                          my_TimeUtil::DstMode          mode)
//  {
//      assert(result);
//      int dst = isDaylightSavingTime(universalTime, timezoneOffset, mode);
//      bdet_DatetimeInterval tmp = timezoneOffset;
//      tmp.addHours(dst);
//      *result = universalTime + tmp;
//      return dst;
//  }
//
//  int my_TimeUtil::convertGmtToLocalTime(
//                          bdet_Datetime                *result,
//                          bsl::time_t                        universalTime,
//                          const bdet_DatetimeInterval&  timezoneOffset,
//                          my_TimeUtil::DstMode          mode)
//  {
//      assert(result);
//      assert(0 <= universalTime);
//      bdet_Datetime tmp;
//      status = bdetu_Datetime::convertFromTimeT(&tmp, universalTime);
//      return convertGmtToLocalTime(result, tmp, timezoneOffset, mode);
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDETU_EPOCH
#include <bdetu_epoch.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>    // @DEPRECATED
#endif

#ifndef INCLUDED_BSL_CTIME
#include <bsl_ctime.h>            // 'time_t'
#endif

namespace BloombergLP {

                            // =====================
                            // struct bdetu_Datetime
                            // =====================

struct bdetu_Datetime {
    // This 'struct' provides a namespace for a suite of non-primitive pure
    // procedures operating on objects of type 'bdet_Datetime'.  These
    // methods are alias-safe, thread-safe, and exception-neutral.

  public:
    // CLASS METHODS
    static int convertToTimeT(bsl::time_t          *result,
                              const bdet_Datetime&  datetime);
        // Load into the specified 'result' the value of the specified
        // 'datetime'.  Return 0 on success, and a non-zero value with no
        // effect on 'result' if 'datetime' cannot be represented as a
        // 'bsl::time_t'.  Note that 'datetime' is assumed to be referenced to
        // Coordinated Universal Time (UTC), and that this function is
        // thread-safe on all platforms.
        //
        // DEPRECATED: use 'bdetu_Epoch::convertToTimeT()' instead.

    static void convertFromTimeT(bdet_Datetime *result, bsl::time_t time);
        // Load into the specified 'result' the value of the specified 'time'.
        // The behavior is undefined if 'bsl::time_t' is negative or represents
        // a time beyond the valid range for a 'bdet_Datetime'.  Note that
        // 'result' will be referenced to Coordinated Universal Time (UTC),
        // and that this function is thread-safe on all platforms.
        //
        // DEPRECATED: use 'bdetu_Epoch::convertFromTimeT()' instead.

    static void convertToTm(bsl::tm *result, const bdet_Datetime& datetime);
        // Load into the specified 'result' the value of the specified
        // 'datetime'.  Each field in 'result' is set to its proper value
        // except 'tm_isdst', which is set to '-1' to indicate that no
        // information on daylight savings time is available.  A time value of
        // 24:00:00:00 will be converted to 0:00:00.  Note that time zones are
        // irrelevant for this conversion.

    static int convertFromTm(bdet_Datetime *result, const bsl::tm& timeStruct);
        // Load into the specified 'result' the value of the specified
        // 'timeStruct'.  Return 0 on success, and a non-zero value with no
        // effect on 'result' if 'timeStruct' is invalid or otherwise cannot be
        // represented as a 'bdet_Datetime'.  Values in fields 'tm_wday',
        // 'tm_yday', and 'tm_isdst' are ignored.  The time 24:00:00 will be
        // recognized, but leap seconds (i.e., values in 'tm_sec' of 60 or 61)
        // will cause the conversion to fail.  Note that time zones are
        // irrelevant for this conversion.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    static const bdet_Datetime& epoch();
        // Return a reference to the non-modifiable epoch time of midnight on
        // January 1, 1970.  Note that this value exists before any code is
        // executed and will continue to exist, unchanged, until the program
        // exits.
        //
        // DEPRECATED: use 'bdetu_Epoch::epoch()' instead.
#endif // BDE_OMIT_INTERNAL_DEPRECATED
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                            // ---------------------
                            // struct bdetu_Datetime
                            // ---------------------

// CLASS METHODS
inline
int bdetu_Datetime::convertToTimeT(bsl::time_t          *result,
                                   const bdet_Datetime&  datetime)
{
    return bdetu_Epoch::convertToTimeT(result, datetime);
}

inline
void bdetu_Datetime::convertFromTimeT(bdet_Datetime *result, bsl::time_t time)
{
    bdetu_Epoch::convertFromTimeT(result, time);
}

inline
void bdetu_Datetime::convertToTm(bsl::tm              *result,
                                 const bdet_Datetime&  datetime)
{
    BSLS_ASSERT_SAFE(result);

    result->tm_sec   = datetime.second();
    result->tm_min   = datetime.minute();
    const int hour   = datetime.hour();
    result->tm_hour  = 24 == hour ? 0 : hour;        // convert to 0 hour;
    result->tm_mday  = datetime.day();
    result->tm_mon   = datetime.month() - 1;
    result->tm_year  = datetime.year() - 1900;
    result->tm_wday  = datetime.date().dayOfWeek() - 1;
    result->tm_yday  = datetime.date().dayOfYear() - 1;
    result->tm_isdst = -1;  // This information is unavailable.
}

inline
int bdetu_Datetime::convertFromTm(bdet_Datetime  *result,
                                  const bsl::tm&  timeStruct)
{
    BSLS_ASSERT_SAFE(result);

    return result->setDatetimeIfValid(timeStruct.tm_year + 1900,
                                      timeStruct.tm_mon + 1,
                                      timeStruct.tm_mday,
                                      timeStruct.tm_hour,
                                      timeStruct.tm_min,
                                      timeStruct.tm_sec);   // msec = 0
}


#ifndef BDE_OMIT_INTERNAL_DEPRECATED
inline
const bdet_Datetime& bdetu_Datetime::epoch()
{
    return bdetu_Epoch::epoch();
}
#endif // BDE_OMIT_INTERNAL_DEPRECATED

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
