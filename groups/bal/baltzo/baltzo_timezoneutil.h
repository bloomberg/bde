// baltzo_timezoneutil.h                                              -*-C++-*-
#ifndef INCLUDED_BALTZO_TIMEZONEUTIL
#define INCLUDED_BALTZO_TIMEZONEUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities for converting times among different time zones.
//
//@CLASSES:
//  baltzo::TimeZoneUtil: utilities for converting local time values
//
//@SEE_ALSO: baltzo_localdatetime, baltzo_zoneinfo, baltzo_defaultzoneinfocache
//
//@DESCRIPTION: This component provides a namespace, 'baltzo::TimeZoneUtil',
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
// {'baltzo_defaultzoneinfocache'}).
//
///Valid, Ambiguous, and Invalid Local-Time Values
///-----------------------------------------------
// There are intervals around each daylight-saving time transition where a
// 'bdlt::Datetime' object holding a local time may not describe a valid or
// unique clock time in the local time zone (see {'baltzo_localtimevalidity'}).
// When interpreting a local-time value represented using a 'bdlt::Datetime'
// object with respect to a given time zone, there are three possible
// scenarios:
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
//:   description (as represented in a 'bdlt::Datetime' object) could refer to
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
// describes how to interpret the input time values (see {'baltzo_dstpolicy'}).
//
///Daylight-Saving Time (DST) Policies and Disambiguation
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The 'baltzo::TimeZoneUtil' methods that take, as input, a 'bdlt::Datetime'
// object representing a local time (i.e., a local-time value without a UTC
// offset) also accept an optional 'baltzo::DstPolicy::Enum'.  This (optional)
// argument policy allows clients to specify how they would like the operation
// to interpret the input value (as such a value may be ambiguous or invalid
// within the indicated time zone -- see above).  Clients are, however,
// encouraged to use the default policy, 'e_UNSPECIFIED', unless there is some
// specific reason they require a different option.
//
// Three enumerated 'baltzo::DstPolicy' values are supported:
//
//: 1 'e_UNSPECIFIED' (default)
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
//: 2 'e_STANDARD'
//:   o Indicates that the operation should treat the associated input time
//:     value as a standard time, using the UTC value computed by applying the
//:     standard-time UTC offset.  Note that the standard-time UTC offset is
//:     used even when the input time value is (unambiguously) *not* a standard
//:     time, which would result in a UTC time that does not correspond to a
//:     standard time within the time zone.
//:
//: 3 'e_DST'
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
///Result of 'convertLocalToUtc' for Various 'baltzo::DstPolicy' Values
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following table summarizes the effect that the different
// 'baltzo::DstPolicy' values have on a call to 'convertLocalToUtc' for several
// possible time values in New York.  Note that standard local time in New York
// is UTC-5:00, and daylight-saving local time there is UTC-4:00.
//..
//     Result format: UTC 'bdlt::Time' & corresponding local 'bdlt::TimeTz'.
// ,--------------------------------------------------------------------------.
// | Input in New York  |              'baltzo::DstPolicy::Enum'              |
// |     Local Time     |-----------------------------------------------------|
// |   (bdlt::Datetime)  |  *_UNSPECIFIED  |   *_STANDARD    |   *_DST        |
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
//: 1 "Jan 1, 2010 01:30" is unambiguously a standard time value.  The result
//:   is simply the corresponding UTC time "Jan 1, 2010 06:30 UTC".
//:
//: 2 "Jan 1, 2010 01:30" is unambiguously a standard time value, so the
//:   supplied policy, 'e_DST', contradicts the actual occurrence of
//:   daylight-saving time in New York.  The input time is adjusted by the UTC
//:   offset for daylight-saving time in New York (-4:00) resulting in a UTC
//:   time 05:30.  Note that the result, "Jan 1, 2010 05:30 UTC", corresponds
//:   to the New York time "Jan 1, 2010 00:30:00-5:00" (a standard time).
//:
//: 3 "Mar 14, 2010 02:30" is not a valid local time in New York (a correctly
//:   administered clock would have been set ahead an hour at 2:00AM).  The
//:   operation will use the later of two potential values, determined by
//:   applying the standard and daylight-saving time UTC offsets to the input
//:   (07:30 UTC and 06:30 UTC, respectively).  Note that the selection of the
//:   later time reflects an assumption that the user forgot to adjust the
//:   clock.
//:
//: 4 The input time is adjusted by the UTC offset for standard time in New
//:   York (-5:00) resulting in the UTC time 07:30.  Note that "Mar 14, 2010
//:   07:30 UTC" corresponds to the New York time "Mar 14, 2010 03:30-4:00" (a
//:   daylight-saving time).
//:
//: 5 The input time is adjusted by the UTC offset for daylight-saving time in
//:   New York (-4:00) resulting in the UTC time 06:30.  Note that "Mar 14,
//:   2010 06:30 UTC" corresponds to the New York time "Mar 14, 2010
//:   01:30-5:00" (a standard time).
//:
//: 6 "Apr 1, 2010 01:30" is unambiguously a daylight-saving time value, so the
//:   supplied policy 'e_STANDARD' contradicts the actual occurrence of
//:   daylight-saving time in New York.  The input time is adjusted by the UTC
//:   offset for standard time in New York (-5:00) resulting in a UTC time
//:   06:30.  Note that "Apr 1, 2010 06:30 UTC" corresponds to the New York
//:   time "Apr 1, 2010 02:30:00-4:00" (a daylight-saving time).
//:
//: 7 "Apr 1, 2010 01:30" is unambiguously a daylight-saving time value.  The
//:   result is simply the corresponding UTC time "Apr 1, 2010 06:30 UTC".
//:
//: 8 "Nov 7, 2010 01:30" is a valid, but ambiguous, local time in New York
//:   (clocks are set back by an hour at 2:00AM, so 1:30AM occurs twice).  The
//:   operation will use the later of two potential values determined by
//:   applying the standard and daylight-saving time UTC offsets to the input
//:   (06:30 UTC and 05:30 UTC, respectively).  Note that the selection of the
//:   later time is arbitrary, but is consistent with common implementations of
//:   the C standard library.
//
///Thread Safety
///-------------
// The functions provided by 'baltzo::TimeZoneUtil' are *thread-safe*, meaning
// they can be safely executed concurrently.
//
///Usage
///-----
// The following usage examples demonstrate how to use various functions
// provided by 'baltzo::TimeZoneUtil' to perform conversions on various time
// representations.
//
///Example 1: Converting a UTC time to a Local Time
/// - - - - - - - - - - - - - - - - - - - - - - - -
// In this usage example, we illustrate how to convert a UTC time to its
// corresponding local time in a given time zone.  We start by creating a
// 'bdlt::Datetime' object holding the UTC time "July 31, 2010 15:00:00":
//..
//  bdlt::Datetime utcTime(2010, 7, 31, 15, 0, 0);
//..
// Then, we create a 'baltzo::LocalDatetime' object to hold the result of the
// conversion operation:
//..
//  baltzo::LocalDatetime newYorkTime;
//..
// Now, we call the 'convertUtcToLocalTime' function in 'baltzo::TimeZoneUtil':
//..
//  int status = baltzo::TimeZoneUtil::convertUtcToLocalTime(&newYorkTime,
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
// Finally, we observe that the result in 'newYorkTime' is "July 31, 2010
// 11:00:00" and that the offset from UTC applied was -4 hours:
//..
//  const bdlt::Datetime test = newYorkTime.datetimeTz().localDatetime();
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
// particular, we want to convert the time "July 31, 2010 15:00:00" in New York
// to its corresponding time in Rome, Italy.
//
// First, we create a 'bdlt::Datetime' object representing the time "July 31,
// 2010 15:00:00" in New York:
//..
//  bdlt::Datetime newYorkTime(2010, 7, 31, 15, 0, 0);
//..
// Now, let's apply the conversion operation to obtain a
// 'baltzo::LocalDatetime' object representing the corresponding local time in
// Italy:
//..
//  baltzo::LocalDatetime romeTime;
//  int status = baltzo::TimeZoneUtil::convertLocalToLocalTime(
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
// Notice that we did not specify the optional 'dstPolicy' argument to
// 'convertLocalToLocalTime'.  The default value should be appropriate for most
// users.
//
// Finally, we verify that the value of 'romeTime' is "July 31, 2010 21:00:00",
// which is the time in Italy corresponding to "July 31, 2010 15:00:00" in New
// York:
//..
//  const bdlt::Datetime test = romeTime.datetimeTz().localDatetime();
//  assert(2010 == test.year());  assert(21 == test.hour());
//  assert(   7 == test.month()); assert( 0 == test.minute());
//  assert(  31 == test.day());   assert( 0 == test.second());
//
//  assert( 2 * 60 == romeTime.datetimeTz().offset());
//..
//
///Example 3: Initializing a Local Time
/// - - - - - - - - - - - - - - - - - -
// In this example we illustrate how to create a 'baltzo::LocalDatetime' from a
// 'bdlt::Datetime', which may not represent a unique (or valid) clock time.
//
// First, we create a 'bdlt::Datetime' object for the New York local time "Jul
// 31, 2010 15:00:00".  Note that this local date-time occurs during a DST
// transition and is an invalid date-time.
//..
//  bdlt::Datetime uniqueTime(2010, 7, 31, 15, 0, 0);
//..
// Then, we call 'initLocalTime', which returns a 'baltzo::LocalDatetime'
// object.  'initLocalTime' also optionally returns
// 'baltzo::LocalTimeValidity::Enum', indicating whether the provided input was
// a valid and unique clock time.  Note that invalid or ambiguous times are
// resolved using the optionally provided 'baltzo::DstPolicy::Enum' (see the
// section {Daylight-Saving Time (DST) Policies and Disambiguation}):
//..
//  baltzo::LocalDatetime             localTime;
//  baltzo::LocalTimeValidity::Enum validity;
//  int status = baltzo::TimeZoneUtil::initLocalTime(&localTime,
//                                                  &validity,
//                                                  uniqueTime,
//                                                  "America/New_York");
//  if (0 != status) {
//      return 1;
//  }
//..
// Now, we verify the value of 'localTime' is "Jul 31, 2010 15:00:00" with an
// offset of -4:00 from UTC, in the time zone "America/New_York".
//..
//  const bdlt::Datetime invalidTest = localTime.datetimeTz().localDatetime();
//  assert(2010 == invalidTest.year());  assert(15 == invalidTest.hour());
//  assert(   7 == invalidTest.month()); assert( 0 == invalidTest.minute());
//  assert(  31 == invalidTest.day());   assert( 0 == invalidTest.second());
//
//  assert( -4 * 60 == localTime.datetimeTz().offset());
//  assert("America/New_York" == localTime.timeZoneId());
//..
// In addition, the time provided represents a unique and valid clock time in
// New York (because it does not fall near a daylight-saving time transition):
//..
//  assert(baltzo::LocalTimeValidity::e_VALID_UNIQUE == validity);
//..
// By contrast, if we call 'initLocalTime' for a time value that falls during a
// during a daylight-saving time transition, the returned
// 'baltzo::LocalTimeValidity::Enum' will indicate if the supplied time either
// does not represent a valid clock time in the time zone (as may occur when
// clocks are set forward), or does not represent a unique clock time (as may
// occur when clocks are set back).
//
// For example, suppose we call 'initLocalTime' for "Mar 14, 2010 02:30"; this
// clock time does not occurs in New York, as clocks are set forward by an hour
// at 2am local time:
//..
//  bdlt::Datetime invalidTime(2010, 3, 14, 2, 30, 0);
//  status = baltzo::TimeZoneUtil::initLocalTime(&localTime,
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
//  const bdlt::Datetime test = localTime.datetimeTz().localDatetime();
//  assert(2010 == test.year());  assert( 3 == test.hour());
//  assert(   3 == test.month()); assert(30 == test.minute());
//  assert(  14 == test.day());   assert( 0 == test.second());
//
//  assert("America/New_York" == localTime.timeZoneId());
//  assert( -4 * 60 == localTime.datetimeTz().offset());
//..
// Finally, we verify that the validity status returned for 'invalidTime' is
// 'e_INVALID':
//..
//  assert(baltzo::LocalTimeValidity::e_INVALID == validity);
//..
//
///Example 4: Obtaining Information About a Time Value
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate how to obtain additional information about a
// local time in a given time zone using the 'loadLocalTimePeriod' method.
// Using 'loadLocalTimePeriod' a client can determine, for a point in time, the
// attributes that characterize local time in a given time zone (e.g., the
// offset from UTC, whether it is daylight-saving time) as well as the interval
// over which those attributes apply (see {'baltzo_localtimeperiod'}).
//
// First, we create a 'baltzo::LocalDatetime' object for the New York local
// time "Jul 31, 2010 15:00:00-04:00".  Note that this 'baltzo::LocalDatetime'
// may also be created as in example 3.
//..
//  bdlt::DatetimeTz localTimeTz(bdlt::Datetime(2010, 7, 31, 15, 0, 0),
//                               -4 * 60);
//  baltzo::LocalDatetime localTime(localTimeTz, "America/New_York");
//..
// Then, we call 'loadLocalTimePeriod', which returns a
// 'baltzo::LocalTimePeriod' object that is loaded with attributes
// characterizing local time in New York on "Mar 14, 2010 03:30:00", and the
// interval over which those attributes are in effect.
//..
//  baltzo::LocalTimePeriod period;
//  int status = baltzo::TimeZoneUtil::loadLocalTimePeriod(&period, localTime);
//  if (0 != status) {
//      // A non-zero 'status' indicates there was an error in the conversion
//      // (e.g., the time zone id was not valid or the environment has not
//      // been correctly configured).
//
//      return 1;                                                     // RETURN
//  }
//..
// Now we examine the returned properties.  "Mar 14, 2010 03:30:00" is during
// daylight-saving time, which is -4:00 UTC, and the type of local time is
// sometimes abbreviated "EDT" for "Eastern Daylight Time".  "Eastern Daylight
// Time" is in effect from "Mar 14, 2010 7am UTC" to "Nov 7, 2010 6am UTC".
// Note that the abbreviation provided ("EDT") is not canonical or localized.
// In general the provided abbreviations should not be displayed to users (they
// are intended for development and debugging only):
//..
//  assert(true         == period.descriptor().dstInEffectFlag());
//  assert(-4 * 60 * 60 == period.descriptor().utcOffsetInSeconds());
//  assert("EDT"        == period.descriptor().description());
//  assert(bdlt::Datetime(2010,  3, 14, 7, 0, 0) == period.utcStartTime());
//  assert(bdlt::Datetime(2010, 11,  7, 6, 0, 0) == period.utcEndTime());
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALTZO_DEFAULTZONEINFOCACHE
#include <baltzo_defaultzoneinfocache.h>
#endif

#ifndef INCLUDED_BALTZO_DSTPOLICY
#include <baltzo_dstpolicy.h>
#endif

#ifndef INCLUDED_BALTZO_LOCALTIMEVALIDITY
#include <baltzo_localtimevalidity.h>
#endif

#ifndef INCLUDED_BALTZO_TIMEZONEUTILIMP
#include <baltzo_timezoneutilimp.h>
#endif

#ifndef INCLUDED_BALTZO_LOCALDATETIME
#include <baltzo_localdatetime.h>
#endif

#ifndef INCLUDED_BDLT_DATETIME
#include <bdlt_datetime.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMETZ
#include <bdlt_datetimetz.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace baltzo {

class LocalTimePeriod;

class ZoneinfoCache;

                            // ===================
                            // struct TimeZoneUtil
                            // ===================

struct TimeZoneUtil {
    // This 'struct' provides a namespace for utility functions that convert
    // time values to, from, and between, their corresponding local time
    // representations in (possibly) different time zones.
    //
    // These utility functions are:
    //: o *alias-safe*
    //: o *exception-neutral* (agnostic)
    //: o *thread-safe*
    // For terminology see {'bsldoc_glossary'}.


    // CLASS METHODS
    static int convertUtcToLocalTime(LocalDatetime         *result,
                                     const char            *targetTimeZoneId,
                                     const bdlt::Datetime&  utcTime);
    static int convertUtcToLocalTime(bdlt::DatetimeTz      *result,
                                     const char            *targetTimeZoneId,
                                     const bdlt::Datetime&  utcTime);
        // Load, into the specified 'result', the local date-time value (in the
        // time zone indicated by the specified 'targetTimeZoneId')
        // corresponding to the specified 'utcTime'.  The offset from UTC of
        // the time zone is rounded down to minute precision.  Return 0 on
        // success, and a non-zero value with no effect otherwise.  A return
        // value of 'ErrorCode::k_UNSUPPORTED_ID' indicates that
        // 'targetTimeZoneId' was not recognized.

    static int convertLocalToLocalTime(LocalDatetime        *result,
                                       const char           *targetTimeZoneId,
                                       const LocalDatetime&  srcTime);
    static int convertLocalToLocalTime(
                                     LocalDatetime           *result,
                                     const char              *targetTimeZoneId,
                                     const bdlt::DatetimeTz&  srcTime);
    static int convertLocalToLocalTime(bdlt::DatetimeTz     *result,
                                       const char           *targetTimeZoneId,
                                       const LocalDatetime&  srcTime);
    static int convertLocalToLocalTime(
                                     bdlt::DatetimeTz        *result,
                                     const char              *targetTimeZoneId,
                                     const bdlt::DatetimeTz&  srcTime);
        // Load, into the specified 'result', the local date-time value (in the
        // time zone indicated by the specified 'targetTimeZoneId')
        // corresponding to the local time indicated by the specified
        // 'srcTime'.  The offset from UTC of both time zones is rounded down
        // to minute precision.  Return 0 on success, and a non-zero value with
        // no effect otherwise.  A return value of
        // 'ErrorCode::k_UNSUPPORTED_ID' indicates that 'targetTimeZoneId' was
        // not recognized.

    static int convertLocalToLocalTime(LocalDatetime         *result,
                                       const char            *targetTimeZoneId,
                                       const bdlt::Datetime&  srcTime,
                                       const char            *srcTimeZoneId,
                                       DstPolicy::Enum        dstPolicy =
                                                     DstPolicy::e_UNSPECIFIED);
    static int convertLocalToLocalTime(bdlt::DatetimeTz      *result,
                                       const char            *targetTimeZoneId,
                                       const bdlt::Datetime&  srcTime,
                                       const char            *srcTimeZoneId,
                                       DstPolicy::Enum        dstPolicy =
                                                     DstPolicy::e_UNSPECIFIED);
        // Load, into the specified 'result', the local date-time value (in the
        // time zone indicated by the specified 'targetTimeZoneId')
        // corresponding to the local time indicated by the specified 'srcTime'
        // (in the time zone indicated by the specified 'srcTimeZoneId').
        // Optionally specify a 'dstPolicy' indicating whether or not 'srcTime'
        // represents a daylight-saving time value.  If 'dstPolicy' is
        // unspecified and 'srcTime' is a unique and valid time in the source
        // time zone, then perform the conversion using that uniquely described
        // time; if 'dstPolicy' is unspecified and 'srcTime' is either
        // ambiguous or invalid, then use the later of the two possible
        // interpretations of 'srcTime'.  The offset from UTC of both time
        // zones is rounded down to minute precision.  Return 0 on success, and
        // a non-zero value with no effect otherwise.  A return value of
        // 'ErrorCode::k_UNSUPPORTED_ID' indicates that either
        // 'targetTimeZoneId' or 'srcTimeZoneId' was not recognized.

    static int initLocalTime(bdlt::DatetimeTz      *result,
                             const bdlt::Datetime&  localTime,
                             const char            *timeZoneId,
                             DstPolicy::Enum        dstPolicy =
                                                     DstPolicy::e_UNSPECIFIED);
    static int initLocalTime(LocalDatetime         *result,
                             const bdlt::Datetime&  localTime,
                             const char            *timeZoneId,
                             DstPolicy::Enum        dstPolicy =
                                                     DstPolicy::e_UNSPECIFIED);
    static int initLocalTime(bdlt::DatetimeTz        *result,
                             LocalTimeValidity::Enum *resultValidity,
                             const bdlt::Datetime&    localTime,
                             const char              *timeZoneId,
                             DstPolicy::Enum          dstPolicy =
                                                     DstPolicy::e_UNSPECIFIED);
    static int initLocalTime(LocalDatetime           *result,
                             LocalTimeValidity::Enum *resultValidity,
                             const bdlt::Datetime&    localTime,
                             const char              *timeZoneId,
                             DstPolicy::Enum          dstPolicy =
                                                     DstPolicy::e_UNSPECIFIED);
        // Load, into the specified 'result', the local date-time value --
        // including the local date, time, and resolved UTC offset -- indicated
        // by the specified 'localTime' in the time zone indicated by the
        // specified 'timeZoneId'.  Optionally specify 'resultValidity' in
        // which to load the validity of 'localTime' as being unique, ambiguous
        // but valid, or invalid.  Optionally specify a 'dstPolicy' indicating
        // whether or not 'localTime' represents a daylight-saving time value.
        // If 'dstPolicy' is unspecified and 'localTime' is a unique and valid
        // time in the source time zone, then perform the conversion using that
        // uniquely described time; if 'dstPolicy' is unspecified and
        // 'localTime' is either ambiguous or invalid, then use the later of
        // the two possible interpretations of 'localTime'.  The offset from
        // UTC of the time zone is rounded down to minute precision.  Return 0
        // on success, and a non-zero value with no effect otherwise.  A return
        // value of 'ErrorCode::k_UNSUPPORTED_ID' indicates that 'timeZoneId'
        // was not recognized.

    static int convertLocalToUtc(bdlt::Datetime        *result,
                                 const bdlt::Datetime&  localTime,
                                 const char            *timeZoneId,
                                 DstPolicy::Enum        dstPolicy =
                                                     DstPolicy::e_UNSPECIFIED);
    static int convertLocalToUtc(LocalDatetime         *result,
                                 const bdlt::Datetime&  localTime,
                                 const char            *timeZoneId,
                                 DstPolicy::Enum        dstPolicy =
                                                     DstPolicy::e_UNSPECIFIED);
        // Load, into the specified 'result', the UTC time value that
        // corresponds to the specified 'localTime' in the time zone indicated
        // by the specified 'timeZoneId'.  Optionally specify a 'dstPolicy'
        // indicating whether or not 'localTime' represents a daylight-saving
        // time value.  If 'dstPolicy' is unspecified and 'localTime' is a
        // unique and valid time in the source time zone, then perform the
        // conversion using that uniquely described time; if 'dstPolicy' is
        // unspecified and 'localTime' is either ambiguous or invalid, then use
        // the later of the two possible interpretations of 'localTime'.  The
        // offset from UTC of the time zone is rounded down to minute
        // precision.  Return 0 on success, and a non-zero value with no effect
        // otherwise.  A return value of 'ErrorCode::k_UNSUPPORTED_ID'
        // indicates that 'timeZoneId' was not recognized.

    static int loadLocalTimePeriod(LocalTimePeriod      *result,
                                   const LocalDatetime&  localTime);
        // Load, into the specified 'result', attributes characterizing the
        // specified 'localTime' (i.e., the offset from UTC, whether
        // daylight-saving time is in effect and the description of the time
        // zone), as well as the time interval over which those attributes
        // apply.  Return 0 on success, and a non-zero value with no effect
        // otherwise.  A return value of 'ErrorCode::k_UNSUPPORTED_ID'
        // indicates that 'localTime.timeZoneId()' was not recognized.

    static int loadLocalTimePeriod(LocalTimePeriod         *result,
                                   const bdlt::DatetimeTz&  localTime,
                                   const char              *timeZoneId);
        // Load, into the specified 'result', attributes characterizing the
        // specified 'localTime' in the time zone indicated by the specified
        // 'timeZoneId' (i.e., the offset from UTC, whether daylight-saving
        // time is in effect and the description of the time zone), as well as
        // the time interval over which those attributes apply.  Return 0 on
        // success, and a non-zero value with no effect otherwise.  A return
        // value of 'ErrorCode::k_UNSUPPORTED_ID' indicates that 'timeZoneId'
        // was not recognized.

    static int loadLocalTimePeriodForUtc(LocalTimePeriod       *result,
                                         const char            *timeZoneId,
                                         const bdlt::Datetime&  utcTime);
        // Load, into the specified 'result', attributes characterizing local
        // time at the specified 'utcTime' in the time zone indicated by the
        // specified 'timeZoneId' (i.e., the offset from UTC, whether
        // daylight-saving time is in effect and the description of the time
        // zone), as well as the time interval over which those attributes
        // apply.  Return 0 on success, and a non-zero value with no effect
        // otherwise.  A return value of 'ErrorCode::k_UNSUPPORTED_ID'
        // indicates that 'timeZoneId' was not recognized.


    static int addInterval(LocalDatetime             *result,
                           const LocalDatetime&       originalTime,
                           const bsls::TimeInterval&  interval);
        // Load, into the specified 'result', the local time value that is the
        // specified 'interval' in the future of the specified 'originalTime'
        // (in the time zone 'originalTime.timeZoneId()').  Return 0 on
        // success, and a non-zero value with no effect otherwise.  A return
        // value of 'ErrorCode::k_UNSUPPORTED_ID' indicates that 'timeZoneId'
        // was not recognized.  The resulting local-time is equivalent to
        // adding 'interval' to 'originalTime.datetimeTz().utcDatetime()' and
        // converting the result into the local time of
        // 'originalTime.timeZoneId()'.

    static int validateLocalTime(bool                    *result,
                                 const bdlt::DatetimeTz&  localTime,
                                 const char              *timeZoneId);
        // Load, into the specified 'result', 'true' if the offset from UTC of
        // the specified 'localTime' (i.e., 'localTime.offset()') is consistent
        // with the actual local time offset, as indicated by time zone data,
        // at the UTC time 'localTime.utcDatetime()' in the time zone indicated
        // by the specified 'timeZoneId', and 'false' otherwise.  Return 0 on
        // success, and a non-zero value with 'false' loaded into 'result'
        // otherwise.  A return value of 'ErrorCode::k_UNSUPPORTED_ID'
        // indicates that 'timeZoneId' is not recognized.  Note that this
        // operation verifies that the properties of the provided local time
        // are consistent with the time zone data.

    static int validateLocalTime(bool *result, const LocalDatetime& localTime);
        // Load, into the specified 'result', 'true' if the time zone
        // identifier of the specified 'localTime' (i.e.,
        // 'localTime.timeZoneId()') is a valid identifier, and the offset from
        // UTC of 'localTime' (i.e., 'localTime.datetimeTz().offset()') is
        // consistent with the actual local time offset, as indicated by time
        // zone data, at the UTC time 'localTime.datetimeTz().utcDatetime()' in
        // the time zone indicated by 'localTime.timeZoneId()', and 'false'
        // otherwise.  Return 0 on success, and a non-zero value with 'false'
        // loaded into 'result' otherwise.  A return value of
        // 'ErrorCode::k_UNSUPPORTED_ID' indicates that 'timeZoneId' is not
        // recognized.  Note that this operation verifies that the properties
        // of the provided local time are consistent with the time zone data.
};

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                            // -------------------
                            // struct TimeZoneUtil
                            // -------------------

// CLASS METHODS
inline
int baltzo::TimeZoneUtil::convertUtcToLocalTime(
                                       bdlt::DatetimeTz      *result,
                                       const char            *targetTimeZoneId,
                                       const bdlt::Datetime&  utcTime)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(targetTimeZoneId);

    return TimeZoneUtilImp::convertUtcToLocalTime(
                                         result,
                                         targetTimeZoneId,
                                         utcTime,
                                         DefaultZoneinfoCache::defaultCache());
}

inline
int baltzo::TimeZoneUtil::convertLocalToLocalTime(
                                        LocalDatetime        *result,
                                        const char           *targetTimeZoneId,
                                        const LocalDatetime&  srcTime)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(targetTimeZoneId);

    return convertUtcToLocalTime(result,
                                 targetTimeZoneId,
                                 srcTime.datetimeTz().utcDatetime());
}

inline
int baltzo::TimeZoneUtil::convertLocalToLocalTime(
                                     LocalDatetime           *result,
                                     const char              *targetTimeZoneId,
                                     const bdlt::DatetimeTz&  srcTime)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(targetTimeZoneId);

    return convertUtcToLocalTime(result,
                                 targetTimeZoneId,
                                 srcTime.utcDatetime());
}

inline
int baltzo::TimeZoneUtil::convertLocalToLocalTime(
                                bdlt::DatetimeTz             *result,
                                const char                   *targetTimeZoneId,
                                const baltzo::LocalDatetime&  srcTime)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(targetTimeZoneId);

    return convertUtcToLocalTime(result,
                                 targetTimeZoneId,
                                 srcTime.datetimeTz().utcDatetime());
}

inline
int baltzo::TimeZoneUtil::convertLocalToLocalTime(
                                     bdlt::DatetimeTz        *result,
                                     const char              *targetTimeZoneId,
                                     const bdlt::DatetimeTz&  srcTime)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(targetTimeZoneId);

    return convertUtcToLocalTime(result,
                                 targetTimeZoneId,
                                 srcTime.utcDatetime());
}

inline
int baltzo::TimeZoneUtil::initLocalTime(
                                       bdlt::DatetimeTz        *result,
                                       LocalTimeValidity::Enum *resultValidity,
                                       const bdlt::Datetime&    localTime,
                                       const char              *timeZoneId,
                                       DstPolicy::Enum          dstPolicy)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(resultValidity);
    BSLS_ASSERT_SAFE(timeZoneId);

    return TimeZoneUtilImp::initLocalTime(
                                         result,
                                         resultValidity,
                                         localTime,
                                         timeZoneId,
                                         dstPolicy,
                                         DefaultZoneinfoCache::defaultCache());
}

inline
int baltzo::TimeZoneUtil::initLocalTime(bdlt::DatetimeTz     *result,
                                       const bdlt::Datetime&  localTime,
                                       const char            *timeZoneId,
                                       DstPolicy::Enum        dstPolicy)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(timeZoneId);

    LocalTimeValidity::Enum validityStatus;
    return initLocalTime(result,
                         &validityStatus,
                         localTime,
                         timeZoneId,
                         dstPolicy);
}

inline
int baltzo::TimeZoneUtil::loadLocalTimePeriod(LocalTimePeriod      *result,
                                              const LocalDatetime&  localTime)
{
    BSLS_ASSERT_SAFE(result);

    return loadLocalTimePeriod(result,
                               localTime.datetimeTz(),
                               localTime.timeZoneId().c_str());
}

inline
int baltzo::TimeZoneUtil::loadLocalTimePeriod(
                                           LocalTimePeriod         *result,
                                           const bdlt::DatetimeTz&  localTime,
                                           const char              *timeZoneId)
{
    BSLS_ASSERT_SAFE(result);
    BSLS_ASSERT_SAFE(timeZoneId);

    return loadLocalTimePeriodForUtc(result,
                                     timeZoneId,
                                     localTime.utcDatetime());
}

inline
int baltzo::TimeZoneUtil::validateLocalTime(bool                 *result,
                                            const LocalDatetime&  localTime)
{
    BSLS_ASSERT_SAFE(result);

    return validateLocalTime(result,
                             localTime.datetimeTz(),
                             localTime.timeZoneId().c_str());
}

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
