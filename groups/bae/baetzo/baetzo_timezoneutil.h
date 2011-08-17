// baetzo_timezoneutil.h                                              -*-C++-*-
#ifndef INCLUDED_BAETZO_TIMEZONEUTIL
#define INCLUDED_BAETZO_TIMEZONEUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide utilities for converting times among different time zones.
//
//@CLASSES:
//  baetzo_TimeZoneUtil: utilities for converting local time values
//
//@SEE_ALSO: baet_localdatetime, baetzo_zoneinfo, baetzo_defaultzoneinfocache
//
//@AUTHOR: Stefano Pacifico (spacifico1), Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a namespace, 'baetzo_TimeZoneUtil',
// containing utility functions for converting time values to and from their
// corresponding local time representations in (possibly) different time zones.
// The primary methods provided include:
//: o 'convertLocalToLocalTime' and 'convertUtcToLocalTime', for converting a
//:   time to the corresponding local-time value in some time zone;
//: o 'convertLocalToUtc', for converting a local-time value into the
//:   corresponding UTC time value;
//: o 'initLocalTime', for initializing a local-time value.
// Additionally, the 'loadLocalTimePeriod' and 'loadLocalTimePeriodForUtc'
// methods enable clients to obtain information about a time value, such as
// whether the provided time is a daylight-saving time value.  Finally note
// that, all of the functions in this utility component make use of a
// process-wide cache of time-zone information (see
// 'baetzo_defaultzoneinfocache').
//
///Valid, Ambiguous, and Invalid Local-Time Values
///-----------------------------------------------
// There are intervals around each daylight-saving time transition where a
// 'bdet_Datetime' object holding a local time may not describe a valid or
// unique clock time in the local time zone (see 'baetzo_localtimevalidity').
// When interpreting a local-time value represented using a 'bdet_Datetime'
// object w.r.t. a given time zone, there are three possible scenarios:
//
//: 1 The local time is *valid* and *unique*: The local time representation is
//:   valid, and unique within the time zone (the most likely scenario).  For
//:   example, in New York at "Aug 31, 2010 12:00AM", DST was in effect and the
//:   local-time offset from UTC was -4 hours.
//:
//: 2 The local time is *valid*, but *ambiguous*: The local time representation
//:   is valid, but could correctly be interpreted as either of two possible
//:   times, as may happen around a daylight-saving time transition where the
//:   local-time offset from UTC increases (e.g., in the United States local
//:   time "falls back" by an hour in the fall).  Thus, a local time within
//:   such a transition period occurs twice, and is ambiguous without
//:   additional information.  For example, in New York, daylight-saving time
//:   was in effect until "Nov 7, 2010 2:00AM" when clocks were set back by an
//:   hour; therefore, the time "Nov 7, 2010 1:30AM" occurred twice, and that
//:   description (as represented in a 'bdet_Datetime' object) could refer to
//:   either of those two times.
//:
//: 3 The local time is *invalid*: The local-time representation doesn't
//:   correspond to a valid time within the given time zone, as may happen
//:   around a daylight-saving time transition where the offset from UTC
//:   decreases (e.g., in the United States local time "springs forward" by an
//:   hour in the spring).  Thus, local times that are skipped during such a
//:   transition are invalid.  For example, in New York, DST was in effect
//:   starting "Mar 14, 2010 2:00AM" when clocks are set forward an hour;
//:   therefore, the local time "Mar 14, 2010 2:30AM" never occurs.
//
// Note that the functions provided in this component guarantee a graceful
// handling of all three scenarios.  Ambiguity and invalidity, when they arise,
// are resolved according to a user-supplied daylight-saving time policy that
// describes how to interpret the input time values (see 'baetzo_dstpolicy').
//
///Daylight-Saving Time (DST) Policies and Disambiguation
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// The 'baetzo_TimeZoneUtil' methods that take, as input, a 'bdet_Datetime'
// object representing a local time (i.e., a local-time value without a UTC
// offset) also accept an optional 'baetzo_DstPolicy::Enum'.  This (optional)
// argument policy allows clients to specify how they would like the operation
// to interpret the input value (as such a value may be ambiguous or invalid
// within the indicated time zone -- see above).  Clients are, however,
// encouraged to use the default policy, 'BAETZO_UNSPECIFIED', unless there is
// some specific reason they require a different option.
//
// Three enumerated 'baetzo_DstPolicy' values are supported:
//
//: 1 BAETZO_UNSPECIFIED (default)
//:   o The client does not explicitly indicate whether the associated input
//:     time represents a daylight-saving time value, and the operation will
//:     determine how to interpret the time solely based on the input itself.
//:     If the input value is *valid* and *unique*, the operation will use its
//:     (unique) corresponding UTC value.  If the input is either *ambiguous*
//:     or *invalid*, the operation will use the later of two potential
//:     interpretations of the input (determined, e.g., by applying the
//:     standard and daylight-saving time UTC offsets to the input).  For
//:     *invalid* times, this choice reflects the assumption that the user most
//:     likely forgot to adjust their clock.  For *ambiguous* times, this
//:     choice is arbitrary (but is consistent with common implementations of
//:     the C standard library).
//:
//: 2 BAETZO_STANDARD
//:   o Indicates that the operation should treat the associated input time
//:     value as a standard time, using the UTC value computed by applying the
//:     standard-time UTC offset.  Note that the standard-time UTC offset is
//:     used even when the input time value is (unambiguously) *not* a standard
//:     time, which would result in a UTC time that does not correspond to a
//:     standard time within the time zone.
//:
//: 3 BAETZO_DST
//:   o Indicates that the operation should treat the associated input time
//:     value as a daylight-saving time, using the UTC value computed by
//:     applying the daylight-saving time UTC offset.  Note that the
//:     daylight-saving-time UTC offset is used even when the input time value
//:     is (unambiguously) *not* a daylight-saving time, which would result in
//:     a UTC time that does not correspond to a daylight-saving time within
//:     the time zone.
//
// Note that these policies are intended to reflect the behavior of the C
// standard library function 'mktime' and its interpretation of the 'tm_isdst'
// value of the supplied 'tm' structure.  The behavior for the "unspecified"
// policy, however, is not strictly defined by either the ISO or POSIX
// standards, and varies among implementations.
//
///Result of 'convertLocalToUtc' for Various 'baetzo_DstPolicy' Values
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following table summarizes the effect that the different
// 'baetzo_DstPolicy' values have on a call to 'convertLocalToUtc' for several
// possible time values in New York.  Note that standard local time in New York
// is UTC-5:00, and daylight-saving local time there is UTC-4:00.
//..
//     Result format: UTC 'bdet_Time' & corresponding local 'bdet_TimeTz'.
// ,--------------------------------------------------------------------------.
// | Input in New York  |              'baetzo_DstPolicy::Enum'               |
// |     Local Time     |-----------------------------------------------------|
// |   (bdet_Datetime)  |  *_UNSPECIFIED  |   *_STANDARD    |   *_DST         |
// |==========================================================================|
// | Jan  1, 2010 01:30 |  06:30:00 UTC   |  06:30:00 UTC   |  05:30:00 UTC   |
// |   (standard time)  | (01:30:00-5:00) | (01:30:00-5:00) | (00:30:00-5:00) |
// |                    |                 |       [1]       |      [2]        |
// |--------------------|-----------------------------------------------------|
// | Mar 14, 2010 02:30 |  07:30:00 UTC   |  07:30:00 UTC   |  06:30:00 UTC   |
// |     (invalid)      | (03:30:00-4:00) | (03:30:00-4:00) | (01:30:00-5:00) |
// |                    |      [3]        |       [4]       |      [5]        |
// |--------------------|-----------------------------------------------------|
// | Apr  1, 2010 01:30 |  05:30:00 UTC   |  06:30:00 UTC   |  05:30:00 UTC   |
// | (daylight-saving)  | (01:30:00-4:00) | (02:30:00-4:00) | (01:30:00-4:00) |
// |                    |                 |       [6]       |      [7]        |
// |--------------------|-----------------------------------------------------|
// | Nov  7, 2010 01:30 |  06:30:00 UTC   |  06:30:00 UTC   |  05:30:00  UTC  |
// |    (ambiguous)     | (01:30:00-5:00) | (01:30:00-5:00) | (01:30:00-4:00) |
// |                    |      [8]        |                 |                 |
// `--------------------------------------------------------------------------'
//..
//
//: 1 "Jan 1, 2010 01:30" is unambiguously a standard time value.  The
//:   result is simply the corresponding UTC time "Jan 1, 2010 05:30 UTC".
//:
//: 2 "Jan 1, 2010 01:30" is unambiguously a standard time value, so the
//:   supplied policy, 'BAETZO_DST', contradicts the actual occurrence of
//:   daylight-saving time in New York.  The input time is adjusted by the
//:   UTC offset for daylight-saving time in New York (-4:00) resulting in a
//:   UTC time 05:30.  Note that the result, "Jan 1, 2010 05:30 UTC",
//:   corresponds to the New York time "Jan 1, 2010 00:30:00-5:00" (a
//:   standard time).
//:
//: 3 "Mar 14, 2010 02:30" is not a valid local time in New York (a correctly
//:   administered clock would have been set ahead an hour at 2:00AM).  The
//:   operation will use the later of two potential values, determined by
//:   applying the standard and daylight-saving time UTC offsets to the input
//:   (07:30 UTC and 06:30 UTC, respectively).  Note that the selection of
//:   the later time reflects an assumption that the user forgot to adjust
//:   the clock.
//:
//: 4 The input time is adjusted by the UTC offset for standard time in New
//:   York (-5:00) resulting in the UTC time 07:30.  Note that
//:   "Mar 14, 2010 07:30 UTC" corresponds to the New York time
//:   "Mar 14, 2010 03:30-4:00" (a daylight-saving time).
//:
//: 5 The input time is adjusted by the UTC offset for daylight-saving time in
//:   New York (-4:00) resulting in the UTC time 06:30.  Note that
//:   "Mar 14, 2010 06:30 UTC" corresponds to the New York time
//:   "Mar 14, 2010 01:30-5:00" (a standard time).
//:
//: 6 "Apr  1, 2010 01:30" is unambiguously a daylight-saving time value, so
//:   the supplied policy 'BAETZO_STANDARD' contradicts the actual occurrence
//:   of daylight-saving time in New York.  The input time is adjusted by the
//:   UTC offset for standard time in New York (-5:00) resulting in a UTC time
//:   06:30.  Note that "Apr 1, 2010 06:30 UTC" corresponds to the New York
//:   time "Apr 1, 2010 02:30:00-4:00" (a daylight-saving time).
//:
//: 7 "Apr  1, 2010 01:30" is unambiguously a daylight-saving time value.  The
//:   result is simply the corresponding UTC time "Apr 1, 2010 06:30 UTC".
//:
//: 8 "Nov  7, 2010 01:30" is a valid, but ambiguous, local time in New York
//:   (clocks are set back by an hour at 2:00AM, so 1:30AM occurs twice).
//:   The operation will use the later of two potential values determined by
//:   applying the standard and daylight-saving time UTC offsets to the input
//:   (06:30 UTC and 05:30 UTC, respectively).  Note that the selection of the
//:   later time is arbitrary, but is consistent with common implementations
//:   of the C standard library.
//
///Usage
///-----
// The following usage examples demonstrate how to use various functions
// provided by 'baetzo_TimeZoneUtil' to perform conversions on various time
// representations.
//
///Example 1: Converting a UTC time to a Local Time
/// - - - - - - - - - - - - - - - - - - - - - - - -
// In this usage example, we illustrate how to convert a UTC time to its
// corresponding local time in a given time zone.  We start by creating a
// 'bdet_Datetime' object holding the UTC time "July 31, 2010 15:00:00":
//..
//  bdet_Datetime utcTime(2010, 7, 31, 15, 0, 0);
//..
//  Then, we create a 'baet_LocalDatetime' object to hold the result of the
//  conversion operation:
//..
//  baet_LocalDatetime newYorkTime;
//..
// Now, we call the 'convertUtcToLocalTime' function in 'baetzo_TimeZoneUtil':
//..
//  int status = baetzo_TimeZoneUtil::convertUtcToLocalTime(&newYorkTime,
//                                                          "America/New_York",
//                                                          utcTime);
//  if (0 != status) {
//      // A non-zero 'status' indicates there was an error in the conversion
//      // (e.g., the time zone id was not valid or the environment has not
//      // been correctly configured).
//
//      return 1;                                                     // RETURN
//  }
//..
// Finally, we observe that the result in 'newYorkTime' is
// "July 31, 2010 11:00:00" and that the offset from UTC applied was -4 hours:
//..
//  const bdet_Datetime test = newYorkTime.datetimeTz().localDatetime();
//  assert(2010 == test.year());  assert(11 == test.hour());
//  assert(   7 == test.month()); assert( 0 == test.minute());
//  assert(  31 == test.day());   assert( 0 == test.second());
//
//  assert( -4 * 60 == newYorkTime.datetimeTz().offset());
//..
//
///Example 2: Converting a Local Time in One Time Zone to Another Time Zone
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate how to convert a local time in a given time
// zone directly to its corresponding local time in another time zone.  In
// particular, we want to convert the time "July 31, 2010 15:00:00" in New
// York to its corresponding time in Rome, Italy.
//
// First, we create a 'bdet_Datetime' object representing the time
// "July 31, 2010 15:00:00" in New York:
//..
//  bdet_Datetime newYorkTime(2010, 7, 31, 15, 0, 0);
//..
// Now, let's apply the conversion operation to obtain a 'baet_LocalDatetime'
// object representing the corresponding local time in Italy:
//..
//  baet_LocalDatetime romeTime;
//  int status = baetzo_TimeZoneUtil::convertLocalToLocalTime(
//                                                        &romeTime,
//                                                        "Europe/Rome",
//                                                        newYorkTime,
//                                                        "America/New_York");
//  if (0 != status) {
//      // A non-zero 'status' indicates there was an error in the conversion
//      // (e.g., the time zone id was not valid or the environment has not
//      // been correctly configured).
//
//      return 1;                                                     // RETURN
//  }
//..
// Notice that we did not specify the optional 'dstPolicy' argument
// to 'convertLocalToLocalTime'.  The default value should be appropriate for
// most users.
//
// Finally, we verify that the value of 'romeTime' is "July 31, 2010 21:00:00",
// which is the time in Italy corresponding to "July 31, 2010 15:00:00" in New
// York:
//..
//  const bdet_Datetime test = romeTime.datetimeTz().localDatetime();
//  assert(2010 == test.year());  assert(21 == test.hour());
//  assert(   7 == test.month()); assert( 0 == test.minute());
//  assert(  31 == test.day());   assert( 0 == test.second());
//
//  assert( 2 * 60 == romeTime.datetimeTz().offset());
//..
//
///Example 3: Initializing a local time
/// - - - - - - - - - - - - - - - - - -
// In this example we illustrate how to create a 'baet_LocalDatetime' from a
// 'bdet_Datetime', which may not represent a unique (or valid) clock time.
//
// First, we create a 'bdet_Datetime' object for the New York local time
// "Jul 31, 2010 15:00:00".  Note that this local date-time occurs during a DST
// transition and is an invalid date-time.
//..
//  bdet_Datetime uniqueTime(2010, 7, 31, 15, 0, 0);
//..
// Then, we call 'initLocalTime', which returns a 'baet_LocalDatetime' object.
// 'initLocalTime' also optionally returns 'baetzo_LocalTimeValidity::Enum',
// indicating whether the provided input was a valid and unique clock time.
// Note that invalid or ambiguous times are resolved using the optionally
// provided 'baetzo_DstPolicy::Enum' (see the section
// 'Daylight-Savings Time (DST) Policies and Disambiguation'):
//..
//  baet_LocalDatetime             localTime;
//  baetzo_LocalTimeValidity::Enum validity;
//  int status = baetzo_TimeZoneUtil::initLocalTime(&localTime,
//                                                  &validity,
//                                                  uniqueTime,
//                                                  "America/New_York");
//  if (0 != status) {
//      return 1;
//  }
//..
// Now, we verify the value of 'localTime' is "Jul 31, 2010 15:00:00" with an
// offset of -4:00 from GMT, in the time zone "America/New_York".
//..
//  const bdet_Datetime invalidTest = localTime.datetimeTz().localDatetime();
//  assert(2010 == invalidTest.year());  assert(15 == invalidTest.hour());
//  assert(   7 == invalidTest.month()); assert( 0 == invalidTest.minute());
//  assert(  31 == invalidTest.day());   assert( 0 == invalidTest.second());
//
//  assert( -4 * 60 == localTime.datetimeTz().offset());
//  assert("America/New_York" == localTime.timeZoneId());
//..
// In addition, the time provided represents a unique and valid clock time in
// New York (because it does not fall near a daylight-savings time
// transition):
//..
//  assert(baetzo_LocalTimeValidity::BAETZO_VALID_UNIQUE == validity);
//..
// By contrast, if we call 'initLocalTime' for a time value that falls during a
// during a daylight-savings time transition, the returned
// 'baetzo_LocalTimeValidity::Enum' will indicate if the supplied time either
// does not represent a valid clock time in the time zone (as may occur when
// clocks are set forward), or does not represent a unique clock time (as may
// occur when clocks are set back).
//
// For example, suppose we call 'initLocalTime' for "Mar 14, 2010 02:30";
// this clock time does not occurs in New York, as clocks are set forward by an
// hour at 2am local time:
//..
//  bdet_Datetime invalidTime(2010, 3, 14, 2, 30, 0);
//  status = baetzo_TimeZoneUtil::initLocalTime(&localTime,
//                                              &validity,
//                                              invalidTime,
//                                             "America/New_York");
//  if (0 != status) {
//      return 1;
//  }
//..
// Now, we verify the value of 'localTime' represents a valid and unique time
// of "Mar 14, 2010 03:30:00-04:00" in the "America/New_York" time zone.
//..
//  const bdet_Datetime test = localTime.datetimeTz().localDatetime();
//  assert(2010 == test.year());  assert( 3 == test.hour());
//  assert(   3 == test.month()); assert(30 == test.minute());
//  assert(  14 == test.day());   assert( 0 == test.second());
//
//  assert("America/New_York" == localTime.timeZoneId());
//  assert( -4 * 60 == localTime.datetimeTz().offset());
//..
// Finally, we verify that the validity status returned for 'invalidTime' is
// 'BAETZO_INVALID':
//..
//  assert(baetzo_LocalTimeValidity::BAETZO_INVALID == validity);
//..
//
///Example 4: Obtaining Information about a Time Value
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate how to obtain additional information about a
// local time in a given time zone using the 'loadLocalTimePeriod' method.
// Using 'loadLocalTimePeriod' a client can determine, for a point in time,
// the attributes that characterize local time in a given time zone (e.g.,
// the offset from UTC, whether it is daylight-saving time) as well as the
// interval over which those attributes apply (see 'baetzo_localtimeperiod').
//
// First, we create a 'baet_LocalDatetime' object for the New York local time
// "Jul 31, 2010 15:00:00-04:00".  Note that this 'baet_LocalDatetime' may also
// be created as in example 3.
//..
//  bdet_DatetimeTz localTimeTz(bdet_Datetime(2010, 7, 31, 15, 0, 0), -4 * 60);
//  baet_LocalDatetime localTime(localTimeTz, "America/New_York");
//..
// Then, we call 'loadLocalTimePeriod', which returns a
// 'baetzo_LocalTimePeriod' object that is loaded with attributes
// characterizing local time in New York on "Mar 14, 2010 03:30:00", and the
// interval over which those attributes are in effect.
//..
//  baetzo_LocalTimePeriod period;
//  int status = baetzo_TimeZoneUtil::loadLocalTimePeriod(&period, localTime);
//  if (0 != status) {
//      // A non-zero 'status' indicates there was an error in the conversion
//      // (e.g., the time zone id was not valid or the environment has not
//      // been correctly configured).
//
//      return 1;                                                     // RETURN
//  }
//..
// Now we examine the returned properties.  "Mar 14, 2010 03:30:00" is during
// daylight-savings time, which is -4:00 GMT, and the type of local time is
// sometimes abbreviated "EDT" for "Eastern Daylight Time".  "Eastern
// Daylight Time" is in effect from "Mar 14, 2010 7am UTC" to "Nov 7, 2010 6am
// UTC".  Note that the abbreviation provided ("EDT") is not canonical or
// localized.  In general the provided abbreviations should not be displayed
// to users (they are inteded for development and debugging only):
//..
//  assert(true         == period.descriptor().dstInEffectFlag());
//  assert(-4 * 60 * 60 == period.descriptor().utcOffsetInSeconds());
//  assert("EDT"        == period.descriptor().description());
//  assert(bdet_Datetime(2010,  3, 14, 7, 0, 0) == period.utcStartTime());
//  assert(bdet_Datetime(2010, 11,  7, 6, 0, 0) == period.utcEndTime());
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAETZO_DEFAULTZONEINFOCACHE
#include <baetzo_defaultzoneinfocache.h>
#endif

#ifndef INCLUDED_BAETZO_DSTPOLICY
#include <baetzo_dstpolicy.h>
#endif

#ifndef INCLUDED_BAETZO_LOCALTIMEVALIDITY
#include <baetzo_localtimevalidity.h>
#endif

#ifndef INCLUDED_BAETZO_TIMEZONEUTILIMP
#include <baetzo_timezoneutilimp.h>
#endif

#ifndef INCLUDED_BAET_LOCALDATETIME
#include <baet_localdatetime.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class bdet_Datetime;
class bdet_DatetimeTz;
class bdet_DatetimeInterval;

class baetzo_LocalTimePeriod;
class baetzo_ZoneinfoCache;

                        // =========================
                        // class baetzo_TimeZoneUtil
                        // =========================

struct baetzo_TimeZoneUtil {
    // This 'struct' provides a namespace for utility functions that convert
    // time values to, from, and between, their corresponding local time
    // representations in (possibly) different time zones.

    // CLASS METHODS
    static int convertUtcToLocalTime(baet_LocalDatetime   *result,
                                     const char           *targetTimeZoneId,
                                     const bdet_Datetime&  utcTime);
    static int convertUtcToLocalTime(bdet_DatetimeTz      *result,
                                     const char           *targetTimeZoneId,
                                     const bdet_Datetime&  utcTime);
        // Load, into the specified 'result', the local date-time value (in the
        // time zone indicated by the specified 'targetTimeZoneId')
        // corresponding to the specified 'utcTime'.  The offset from UTC of
        // the time zone is rounded down to minute precision.  Return 0 on
        // success, and a non-zero value with no effect otherwise.  A return
        // value of 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that
        // 'targetTimeZoneId' was not recognized.

    static int convertLocalToLocalTime(
                                   baet_LocalDatetime        *result,
                                   const char                *targetTimeZoneId,
                                   const baet_LocalDatetime&  srcTime);
    static int convertLocalToLocalTime(
                                   baet_LocalDatetime        *result,
                                   const char                *targetTimeZoneId,
                                   const bdet_DatetimeTz&     srcTime);
    static int convertLocalToLocalTime(
                                   bdet_DatetimeTz           *result,
                                   const char                *targetTimeZoneId,
                                   const baet_LocalDatetime&  srcTime);
    static int convertLocalToLocalTime(
                                   bdet_DatetimeTz           *result,
                                   const char                *targetTimeZoneId,
                                   const bdet_DatetimeTz&     srcTime);
        // Load, into the specified 'result', the local date-time value (in the
        // time zone indicated by the specified 'targetTimeZoneId')
        // corresponding to the local time indicated by the specified
        // 'srcTime'.  The offset from UTC of both time zones is rounded down
        // to minute precision.  Return 0 on success, and a non-zero value with
        // no effect otherwise.   A return value of
        // 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that
        // 'targetTimeZoneId' was not recognized.

    static int convertLocalToLocalTime(
                             baet_LocalDatetime     *result,
                             const char             *targetTimeZoneId,
                             const bdet_Datetime&    srcTime,
                             const char             *srcTimeZoneId,
                             baetzo_DstPolicy::Enum  dstPolicy =
                                         baetzo_DstPolicy::BAETZO_UNSPECIFIED);
    static int convertLocalToLocalTime(
                             bdet_DatetimeTz        *result,
                             const char             *targetTimeZoneId,
                             const bdet_Datetime&    srcTime,
                             const char             *srcTimeZoneId,
                             baetzo_DstPolicy::Enum  dstPolicy =
                                         baetzo_DstPolicy::BAETZO_UNSPECIFIED);
        // Load, into the specified 'result', the local date-time value (in the
        // time zone indicated by the specified 'targetTimeZoneId')
        // corresponding to the local time indicated by the specified 'srcTime'
        // (in the time zone indicated by the specified 'srcTimeZoneId').
        // Optionally specify a 'dstPolicy' indicating whether or not 'srcTime'
        // represents a daylight-saving time value.  If no 'dstPolicy' is
        // specified and 'srcTime' is a unique and valid time in the source
        // time zone, then perform the conversion using that uniquely described
        // time; if no 'dstPolicy' is specified and 'srcTime' is either
        // ambiguous or invalid, then use the later of the two possible
        // interpretations of 'srcTime'.  The offset from UTC of both time
        // zones is rounded down to minute precision.  Return 0 on success, and
        // a non-zero value with no effect otherwise.  A return value of
        // 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that either
        // 'targetTimeZoneId' or 'srcTimeZoneId' was not recognized.

    static int initLocalTime(bdet_DatetimeTz        *result,
                             const bdet_Datetime&    localTime,
                             const char             *timeZoneId,
                             baetzo_DstPolicy::Enum  dstPolicy =
                                         baetzo_DstPolicy::BAETZO_UNSPECIFIED);
    static int initLocalTime(baet_LocalDatetime     *result,
                             const bdet_Datetime&    localTime,
                             const char             *timeZoneId,
                             baetzo_DstPolicy::Enum  dstPolicy =
                                         baetzo_DstPolicy::BAETZO_UNSPECIFIED);
    static int initLocalTime(bdet_DatetimeTz                *result,
                             baetzo_LocalTimeValidity::Enum *resultValidity,
                             const bdet_Datetime&            localTime,
                             const char                     *timeZoneId,
                             baetzo_DstPolicy::Enum          dstPolicy =
                                         baetzo_DstPolicy::BAETZO_UNSPECIFIED);
    static int initLocalTime(baet_LocalDatetime             *result,
                             baetzo_LocalTimeValidity::Enum *resultValidity,
                             const bdet_Datetime&            localTime,
                             const char                     *timeZoneId,
                             baetzo_DstPolicy::Enum          dstPolicy =
                                         baetzo_DstPolicy::BAETZO_UNSPECIFIED);
        // Load, into the specified 'result', the local date-time value --
        // including the local date, time, and resolved UTC offset -- indicated
        // by the specified 'localTime' in the time zone indicated by the
        // specified 'timeZoneId'.  Optionally specify 'resultValidity' in
        // which to load the validity of 'localTime' as being unique, ambiguous
        // but valid, or invalid.  Optionally specify a 'dstPolicy' indicating
        // whether or not 'localTime' represents a daylight-saving time value.
        // If no 'dstPolicy' is specified and 'localTime' is a unique and valid
        // time in the source time zone, then perform the conversion using that
        // uniquely described time; if no 'dstPolicy' is specified and
        // 'localTime' is either ambiguous or invalid, then use the later of
        // the two possible interpretations of 'localTime'.  The offset from
        // UTC of the time zone is rounded down to minute precision.  Return 0
        // on success, and a non-zero value with no effect otherwise.  A return
        // value of 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that
        // 'timeZoneId' was not recognized.

    static int convertLocalToUtc(bdet_Datetime          *result,
                                 const bdet_Datetime&    localTime,
                                 const char             *timeZoneId,
                                 baetzo_DstPolicy::Enum  dstPolicy =
                                         baetzo_DstPolicy::BAETZO_UNSPECIFIED);
    static int convertLocalToUtc(baet_LocalDatetime     *result,
                                 const bdet_Datetime&    localTime,
                                 const char             *timeZoneId,
                                 baetzo_DstPolicy::Enum  dstPolicy =
                                         baetzo_DstPolicy::BAETZO_UNSPECIFIED);
        // Load, into the specified 'result', the UTC time value that
        // corresponds to the specified 'localTime' in the time zone indicated
        // by the specified 'timeZoneId'.  Optionally specify a 'dstPolicy'
        // indicating whether or not 'localTime' represents a daylight-saving
        // time value.  If no 'dstPolicy' is specified and 'localTime'
        // is a unique and valid time in the source time zone, then perform
        // the conversion using that uniquely described time; if no
        // 'dstPolicy' is specified and 'localTime' is either ambiguous or
        // invalid, then use the later of the two possible interpretations of
        // 'localTime'.  The offset from UTC of the time zone is rounded down
        // to minute precision.  Return 0 on success, and a non-zero value with
        // no effect otherwise.  A return value of
        // 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that
        // 'timeZoneId' was not recognized.

    static int loadLocalTimePeriod(baetzo_LocalTimePeriod    *result,
                                   const baet_LocalDatetime&  localTime);
        // Load, into the specified 'result', attributes characterizing the
        // specified 'localTime' (i.e., the offset from UTC, whether
        // daylight-saving time is in effect and the description of the time
        // zone), as well as the time interval over which those attributes
        // apply.  Return 0 on success, and a non-zero value with no effect
        // otherwise.  A return value of
        // 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that
        // 'localTime.timeZoneId()' was not recognized.

    static int loadLocalTimePeriod(baetzo_LocalTimePeriod *result,
                                   const bdet_DatetimeTz&  localTime,
                                   const char             *timeZoneId);
        // Load, into the specified 'result', attributes characterizing the
        // specified 'localTime' in the time time zone indicated by the
        // specified 'timeZoneid' (i.e., the offset from UTC, whether
        // daylight-saving time is in effect and the description of the time
        // zone), as well as the time interval over which those attributes
        // apply.  Return 0 on success, and a non-zero value with no effect
        // otherwise.  A return value of
        // 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that
        // 'timeZoneId' was not recognized.

    static int loadLocalTimePeriodForUtc(baetzo_LocalTimePeriod *result,
                                         const char             *timeZoneId,
                                         const bdet_Datetime&    utcTime);
        // Load, into the specified 'result', attributes characterizing local
        // time at the specified 'utcTime' in the time zone indicated by the
        // specified 'timeZoneId' (i.e., the offset from UTC, whether
        // daylight-saving time is in effect and the description of the time
        // zone), as well as the time interval over which those attributes
        // apply.  Return 0 on success, and a non-zero value with no effect
        // otherwise.  A return value of
        // 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that
        // 'timeZoneId' was not recognized.

    static int addInterval(baet_LocalDatetime           *result,
                           const baet_LocalDatetime&     originalTime,
                           const bdet_DatetimeInterval&  interval);
        // Load, into the specified 'result', the local time value resulting
        // from adding the specified 'interval' to the specified 'originalTime'
        // (in the time zone indicated by its 'timeZoneId' field).  Return 0 on
        // success, and a non-zero value with no effect otherwise.  A return
        // value of 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that
        // 'timeZoneId' was not recognized.

    static int validateLocalTime(bool                   *result,
                                 const bdet_DatetimeTz&  localTime,
                                 const char             *timeZoneId);
        // Load, into the specified 'result', 'true' if the offset from UTC of
        // the specified 'localTime' (i.e., 'localTime.offset()') is consistent
        // with the actual local time offset, as indicated by time zone data,
        // at the UTC time 'localTime.gmtDatetime()' in the time zone indicated
        // by the specified 'timeZoneId', and 'false' otherwise.  Return 0 on
        // success, and a non-zero value with 'false' loaded into 'result'
        // otherwise.  A return value of
        // 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that
        // 'timeZoneId' is not recognized.  Note that this operation verifies
        // that the properties of the provided local time are consistent with
        // the time zone data.

    static int validateLocalTime(bool                      *result,
                                 const baet_LocalDatetime&  localTime);
        // Load, into the specified 'result', 'true' if the time zone
        // identifier of the specified 'localTime' (i.e.,
        // 'localTime.timeZoneId()') is a valid identifier, and the offset from
        // UTC of 'localTime' (i.e., 'localTime.datetimeTz().offset()') is
        // consistent with the actual local time offset, as indicated by time
        // zone data, at the UTC time 'localTime.dateTimeTz().gmtDatetime()' in
        // the time zone inidicated by 'localTime.timeZoneId()', and 'false'
        // otherwise.  Return 0 on success, and a non-zero value with 'false'
        // loaded into 'result' otherwise.  A return value of
        // 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that
        // 'timeZoneId' is not recognized.  Note that this operation verifies
        // that the properties of the provided local time are consistent with
        // the time zone data.

};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -------------------------
                        // class baetzo_TimeZoneUtil
                        // -------------------------

// CLASS METHODS
inline
int baetzo_TimeZoneUtil::convertUtcToLocalTime(
                                       bdet_DatetimeTz      *result,
                                       const char           *targetTimeZoneId,
                                       const bdet_Datetime&  utcTime)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(targetTimeZoneId);

    return baetzo_TimeZoneUtilImp::convertUtcToLocalTime(
                                  result,
                                  targetTimeZoneId,
                                  utcTime,
                                  baetzo_DefaultZoneinfoCache::defaultCache());
}

inline
int baetzo_TimeZoneUtil::convertLocalToLocalTime(
                                  baet_LocalDatetime        *result,
                                  const char                *targetTimeZoneId,
                                  const baet_LocalDatetime&  srcTime)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(targetTimeZoneId);

    return convertUtcToLocalTime(result,
                                 targetTimeZoneId,
                                 srcTime.datetimeTz().gmtDatetime());
}

inline
int baetzo_TimeZoneUtil::convertLocalToLocalTime(
                                  baet_LocalDatetime     *result,
                                  const char             *targetTimeZoneId,
                                  const bdet_DatetimeTz&  srcTime)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(targetTimeZoneId);

    return convertUtcToLocalTime(result,
                                 targetTimeZoneId,
                                 srcTime.gmtDatetime());
}

inline
int baetzo_TimeZoneUtil::convertLocalToLocalTime(
                                   bdet_DatetimeTz           *result,
                                   const char                *targetTimeZoneId,
                                   const baet_LocalDatetime&  srcTime)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(targetTimeZoneId);

    return convertUtcToLocalTime(result,
                                 targetTimeZoneId,
                                 srcTime.datetimeTz().gmtDatetime());
}

inline
int baetzo_TimeZoneUtil::convertLocalToLocalTime(
                                      bdet_DatetimeTz        *result,
                                      const char             *targetTimeZoneId,
                                      const bdet_DatetimeTz&  srcTime)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(targetTimeZoneId);

    return convertUtcToLocalTime(result,
                                 targetTimeZoneId,
                                 srcTime.gmtDatetime());
}

inline
int baetzo_TimeZoneUtil::initLocalTime(
                                bdet_DatetimeTz                *result,
                                baetzo_LocalTimeValidity::Enum *resultValidity,
                                const bdet_Datetime&            localTime,
                                const char                     *timeZoneId,
                                baetzo_DstPolicy::Enum          dstPolicy)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(resultValidity);
    BSLS_ASSERT_SAFE(timeZoneId);

    return baetzo_TimeZoneUtilImp::initLocalTime(
                                  result,
                                  resultValidity,
                                  localTime,
                                  timeZoneId,
                                  dstPolicy,
                                  baetzo_DefaultZoneinfoCache::defaultCache());
}

inline
int baetzo_TimeZoneUtil::initLocalTime(bdet_DatetimeTz         *result,
                                       const bdet_Datetime&     localTime,
                                       const char              *timeZoneId,
                                       baetzo_DstPolicy::Enum   dstPolicy)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(timeZoneId);

    baetzo_LocalTimeValidity::Enum validityStatus;
    return initLocalTime(result,
                         &validityStatus,
                         localTime,
                         timeZoneId,
                         dstPolicy);
}

inline
int baetzo_TimeZoneUtil::loadLocalTimePeriod(
                                          baetzo_LocalTimePeriod    *result,
                                          const baet_LocalDatetime&  localTime)
{
    BSLS_ASSERT_SAFE(result);

    return loadLocalTimePeriod(result,
                               localTime.datetimeTz(),
                               localTime.timeZoneId().c_str());
}

inline
int baetzo_TimeZoneUtil::loadLocalTimePeriod(
                                           baetzo_LocalTimePeriod *result,
                                           const bdet_DatetimeTz&  localTime,
                                           const char             *timeZoneId)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(timeZoneId);

    return loadLocalTimePeriodForUtc(result,
                                     timeZoneId,
                                     localTime.gmtDatetime());
}

inline
int baetzo_TimeZoneUtil::validateLocalTime(
                                          bool                      *result,
                                          const baet_LocalDatetime&  localTime)
{
    BSLS_ASSERT_SAFE(result);

    return validateLocalTime(result,
                             localTime.datetimeTz(),
                             localTime.timeZoneId().c_str());
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
