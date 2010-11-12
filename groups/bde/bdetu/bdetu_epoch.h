// bdetu_epoch.h                                                      -*-C++-*-
#ifndef INCLUDED_BDETU_EPOCH
#define INCLUDED_BDETU_EPOCH

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Conversion between absolute/relative time with respect to epoch.
//
//@CLASSES:
//   bdetu_Epoch: non-primitive pure procedures on epoch-related conversions
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@SEE_ALSO: bdet_datetime, bdet_datetimeinterval, bdet_timeinterval
//
//@DESCRIPTION: This component provides non-primitive conversions between two
// different concepts of time.  Clients can convert between absolute time
// ('bdet_Datetime') and relative time (such as 'bsl::time_t',
// 'bdet_TimeInterval', and 'bdet_DatetimeInterval') with respect to the
// Unix standard "epoch" (1970/01/01_00:00:00.000, henceforth, simply referred
// to as "the epoch").  Also provided is a fast, thread-safe method, 'epoch',
// for access to a preinstantiated 'bdet_Datetime' object whose (constant)
// value is that of the epoch.
//
// Due to different resolutions, conversions between absolute/relative time are
// possibly lossy when converting from a type with higher resolution to one
// with lower resolution.  The value of the type with higher resolution will be
// truncated (not rounded).  The following table lists the resolution of the
// types involved in this component:
//..
//  Type                      Reference             Resolution
//  ----                      ---------             ----------
//  bsl::time_t               relative              seconds
//  bdet_Datetime             absolute (GMT)        milliseconds
//  bdet_DatetimeInterval     relative              milliseconds
//  bdet_TimeInterval         relative              nanoseconds
//..
///Thread-Safety
///-------------
// It is safe to invoke any function defined in this component in two or more
// separate threads simultaneously, provided no other thread is simultaneously
// modifying the argument passed by reference to a non-modifiable user-defined
// type (such as 'bdet_TimeInterval' or 'bdet_DateTime').
//
///Usage
///-----
///Example 1: Converting from 'bsl::time_t' to 'bdet_Datetime'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippet of code demonstrates how to use 'bdetu_Epoch' to
// convert from a (relative) 'bsl::time_t' (with respect to the epoch) to an
// (absolute) 'bdet_Datetime':
//..
//  bsl::time_t   myStdTime = 946684800;  // Midnight, January 1, 2000
//  bdet_Datetime datetime;
//
//  bdetu_Epoch::convertFromTimeT(&datetime, myStdTime);  // conversion never
//                                                        // fails
//
//  bsl::cout << datetime << bsl::endl;
//..
// The code above produces the following on 'stdout':
//..
//  01JAN2000_00:00:00.000
//..
// For the convenience of the user, this component also provides a version of
// the function that returns the converted 'bdet_Datetime' by value:
//..
//  bsl::time_t myStdTime = 946684800;  // Midnight, January 1, 2000
//
//  bsl::cout << bdetu_Epoch::convertFromTimeT(myStdTime) << bsl::endl;
//..
// The code above produces the following on 'stdout':
//..
//  01JAN2000_00:00:00.000
//..
// Note that this convenience comes with a cost: Clients will sacrifice
// efficiency, as the object is constructed first within the function, before
// being returned by value.
//
///Example 2: Converting from 'bdet_Datetime' to 'bsl::time_t'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following demonstrates how to convert from an (absolute) 'bdet_Datetime'
// to a (relative) 'bsl::time_t' (with respect to the epoch).  The conversion
// will fail and return a non-zero status code with no changes to the
// 'bsl::time_t' passed in unless 'datetime' can be represented as
// 'bsl::time_t' and 'datetime >= epoch()':
//..
//  bdet_Datetime datetime(2000, 1, 1, 0, 0, 0, 0);
//  bsl::time_t   myStdTime;
//
//  int status = bdetu_Epoch::convertToTimeT(&myStdTime, datetime);
//
//  if (0 != status) {
//      bsl::cout << "Conversion Failure" << bsl::endl;
//  }
//  else {
//      bsl::cout << myStdTime << bsl::endl;
//  }
//..
// The code above produces the following on 'stdout':
//..
//  946684800
//..
// Again, a version that returns the converted (relative) 'bsl::time_t' by
// value is provided.  Usage is similar to example 1.  In addition to
// sacrificing efficiency, this version does not perform any error checking.
// The behavior is undefined if the conversion fails.  If error checking is
// desired, use the version that returns a status code.
//
///Example 3: Converting from 'bdet_TimeInterval' to 'bdet_Datetime'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following demonstrates how to convert from a (relative)
// 'bdet_TimeInterval' (with respect to the epoch) to an (absolute)
// 'bdet_Datetime':
//..
//  bdet_TimeInterval timeInterval(946684800, 900000000);  // January 1st, 2000
//                                                         // 900 milliseconds
//
//  bdet_Datetime datetime;
//
//  bdetu_Epoch::convertFromTimeInterval(&datetime, timeInterval);
//
//  bsl::cout << datetime << bsl::endl;
//..
// The code above produces the following on 'stdout':
//..
//  01JAN2000_00:00:00.900
//..
// The conversion from 'bdet_TimeInterval' to 'bdet_Datetime' is lossy as the
// resolution of 'bdet_TimeInterval' is greater than that of 'bdet_Datetime'.
// The following table illustrates the truncation of resolution beyond
// microseconds after conversion:
//..
//  Input (nanoseconds)     Output (milliseconds)
//  -------------------     ---------------------
//  123456789               123
//  987654321               987
//  000000001               000
//  000999999               000
//  999999999               999
//..
// Also provided is a version of the function that returns the converted
// absolute 'bdet_Datetime' by value.  Again, usage is similar to that of
// example 1.
//
///Example 4: Converting from 'bdet_Datetime' to 'bdet_TimeInterval'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following demonstrates how to convert from an (absolute) 'bdet_Datetime'
// to a (relative) 'bdet_TimeInterval' (with respect to the epoch):
//..
//  bdet_Datetime     datetime(2000, 1, 1, 0, 0, 0, 900);
//  bdet_TimeInterval timeInterval;
//
//  int status = bdetu_Epoch::convertToTimeInterval(&timeInterval, datetime);
//      // Works when 'datetime >= epoch()'.
//
//  if (0 != status) {
//      bsl::cout << "Conversion Failure" << bsl::endl;
//  }
//  else {
//      bsl::cout << timeInterval << bsl::endl;
//  }
//..
// The code above produces the following on 'stdout':
//..
//  (946684800, 900000000)
//..
// A version that returns the converted relative 'bdet_TimeInterval' by value
// is provided.  Usage is similar to example 1.
//
///Example 5: Converting from 'bdet_DatetimeInterval' to 'bdet_Datetime'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following demonstrates how to convert from a (relative)
// 'bdet_DatetimeInterval' (with respect to the epoch) to an (absolute)
// 'bdet_Datetime':
//..
//  bdet_DatetimeInterval datetimeInterval(0, 0, 0, 0, 946684800999LL);
//      // January 1st, 2000, 999 milliseconds
//
//  bdet_Datetime datetime;
//
//  bdetu_Epoch::convertFromDatetimeInterval(&datetime, datetimeInterval);
//
//  bsl::cout << datetime << bsl::endl;
//..
// The code above produces the following on 'stdout':
//..
//  01JAN2000_00:00:00.999
//..
// A version that returns the converted (relative) 'bdet_Datetime' by value is
// provided.  Usage is similar to example 1.
//
///Example 6: Converting from 'bdet_Datetime' to 'bdet_DatetimeInterval'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following demonstrates how to convert from an (absolute) 'bdet_Datetime'
// (with respect to the epoch) to a (relative) 'bdet_DatetimeInterval':
//..
//  bdet_Datetime         datetime(2000, 1, 1, 0, 0, 0, 999);
//  bdet_DatetimeInterval datetimeInterval;
//
//  bdetu_Epoch::convertToDatetimeInterval(&datetimeInterval, datetime);
//      // Works when 'datetime >= epoch()'.
//
//  if (0 != status) {
//      bsl::cout << "Conversion Failure" << bsl::endl;
//  }
//  else {
//      bsl::cout << datetimeInterval << bsl::endl;
//  }
//..
// The code above produces the following on 'stdout':
//..
//  +10957_00:00:00.999
//..
// Again, for the convenience of the user, a version that returns the
// converted relative 'bdet_DatetimeInterval' by value is provided.  Usage is
// similar to example 1.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BDET_DATETIMEINTERVAL
#include <bdet_datetimeinterval.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BDETU_UNSET
#include <bdetu_unset.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>        // 'Int64', 'Uint64'
#endif

#ifndef INCLUDED_BSL_CTIME
#include <bsl_ctime.h>                // 'bsl::time_t'
#endif

namespace BloombergLP {

                            // ==================
                            // struct bdetu_Epoch
                            // ==================

struct bdetu_Epoch {
    // This 'struct' provides a namespace for a suite of non-primitive pure
    // procedures providing conversions between (absolute) 'bdet_Datetime'
    // values and corresponding (relative) time intervals with respect to the
    // Unix standard epoch time, returned by the 'epoch' method.  These methods
    // are alias-safe, thread-safe, and exception-neutral.

  private:
    // CLASS DATA
    static const bdet_Datetime *s_epoch_p;  // pointer for conversions

  public:
    // CLASS METHODS
    static const bdet_Datetime& epoch();
        // Return a reference to the non-modifiable epoch time: midnight on
        // January 1, 1970.  Note that this value exists before any code is
        // executed and will continue to exist, unchanged, until the program
        // exits.

    static int convertToTimeT(bsl::time_t          *result,
                              const bdet_Datetime&  datetime);
        // Load into the specified 'result' the (relative) time converted to
        // 'bsl::time_t' format, computed as the difference between the
        // specified (absolute) 'datetime' and the epoch.  Return 0 on success,
        // and a non-zero value (with no effect on 'result') if 'datetime'
        // cannot be represented in 'bsl::time_t' format or
        // 'datetime < epoch()'.  Note that 'datetime' is assumed to use
        // Greenwich Mean Time (GMT) as a reference.

    static bsl::time_t convertToTimeT(const bdet_Datetime& datetime);
        // Return, in 'bsl::time_t' format, the (relative) time computed as the
        // difference between the specified (absolute) 'datetime' and the
        // epoch.  The behavior is undefined unless 'epoch() <= datetime' and
        // the converted 'datetime' can be represented in 'bsl::time_t' format.
        // Note that 'datetime' is assumed to use Greenwich Mean Time (GMT) as
        // a reference.  Also note that if error detection is desired, the
        // overloaded version that loads the converted 'datetime' into a
        // supplied 'bsl::time_t' object should be used.

    static void convertFromTimeT(bdet_Datetime *result, bsl::time_t time);
        // Load into the specified 'result' the (absolute) datetime converted
        // to 'bdet_Datetime', computed as the sum of the specified (relative)
        // 'time' and the epoch.  The behavior is undefined unless '0 <= time'.
        // Note that the conversion will always succeed, and that 'result' will
        // use Greenwich Mean Time (GMT) as a reference.

    static bdet_Datetime convertFromTimeT(bsl::time_t time);
        // Return, as 'bdet_Datetime', the (absolute) datetime computed as the
        // sum of the specified (relative) 'time' and the epoch.  The behavior
        // is undefined unless '0 <= time'.  Note that the returned value will
        // use Greenwich Mean Time (GMT) as a reference.

    static int convertToTimeInterval(bdet_TimeInterval    *result,
                                     const bdet_Datetime&  datetime);
        // Load into the specified 'result' the (relative) time converted to
        // 'bdet_TimeInterval', computed as the difference between the
        // specified (absolute) 'datetime' and the epoch.  Return 0 on success,
        // and a non-zero value (with no effect on 'result') if
        // 'datetime < epoch()'.

    static bdet_TimeInterval convertToTimeInterval(
                                                const bdet_Datetime& datetime);
        // Return, as 'bdet_TimeInterval', the (relative) time computed as the
        // difference between the specified (absolute) 'datetime' and the
        // epoch.  The behavior is undefined unless 'epoch() <= datetime'.
        // Note that if error detection is desired, the overloaded version that
        // loads the converted 'datetime' into a supplied 'bdet_TimeInterval'
        // object should be used.

    static void convertFromTimeInterval(
                                       bdet_Datetime            *result,
                                       const bdet_TimeInterval&  timeInterval);
        // Load into the specified 'result' the (absolute) datetime converted
        // to 'bdet_Datetime', computed as the sum of the specified (relative)
        // 'timeInterval' and the epoch.  The behavior is undefined unless
        // '0 <= timeInterval' and the conversion result can be represented as
        // a 'bdet_Datetime'.  Note that the conversion is potentially lossy as
        // the resolution of 'bdet_TimeInterval' is greater than that of
        // 'bdet_Datetime'.

    static bdet_Datetime convertFromTimeInterval(
                                        const bdet_TimeInterval& timeInterval);
        // Return, as 'bdet_Datetime', the (absolute) datetime computed as the
        // sum of the specified (relative) 'timeInterval' and the epoch.  The
        // behavior is undefined unless '0 <= timeInterval' and the conversion
        // result can be represented as a 'bdet_Datetime'.  Note that the
        // conversion is potentially lossy as the resolution of
        // 'bdet_TimeInterval' is greater than that of 'bdet_Datetime'.

    static int convertToDatetimeInterval(bdet_DatetimeInterval *result,
                                         const bdet_Datetime&   datetime);
        // Load into the specified 'result' the (relative) time converted to
        // 'bdet_DatetimeInterval', computed as the difference between the
        // specified (absolute) 'datetime' and the epoch.  Return 0 on success,
        // and a non-zero value (with no effect on 'result') if
        // 'datetime < epoch()'.

    static bdet_DatetimeInterval convertToDatetimeInterval(
                                                const bdet_Datetime& datetime);
        // Return, as 'bdet_DatetimeInterval', the (relative) time computed as
        // the difference between the specified (absolute) 'datetime' and the
        // epoch.  The behavior is undefined unless 'epoch() <= datetime'.
        // Note that if error detection is desired, the overloaded version that
        // loads the converted 'datetime' into a supplied
        // 'bdet_DatetimeInterval' object should be used.

    static void convertFromDatetimeInterval(
                               bdet_Datetime                *result,
                               const bdet_DatetimeInterval&  datetimeInterval);
        // Load into the specified 'result' the (absolute) datetime converted
        // to 'bdet_Datetime', computed as the sum of the specified (relative)
        // 'datetimeInterval' and the epoch.  The behavior is undefined unless
        // '0 <= datetimeInterval' and the conversion result can be represented
        // as a 'bdet_Datetime'.

    static bdet_Datetime convertFromDatetimeInterval(
                                const bdet_DatetimeInterval& datetimeInterval);
        // Return, as 'bdet_Datetime', the (absolute) datetime computed as the
        // sum of the specified (relative) 'datetimeInterval' and the epoch.
        // The behavior is undefined unless '0 <= datetimeInterval' and the
        // conversion result can be represented as a 'bdet_Datetime'.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                            // ------------------
                            // struct bdetu_Epoch
                            // ------------------

// CLASS METHODS
inline
const bdet_Datetime& bdetu_Epoch::epoch()
{
    return *s_epoch_p;
}

inline
int bdetu_Epoch::convertToTimeT(bsl::time_t          *result,
                                const bdet_Datetime&  datetime)
{
    BSLS_ASSERT_SAFE(result);

    // Avoid doing '<' comparison of 'datetime' until we're sure the time
    // portion is not unset.

    if (bdetu_Unset<bdet_Time>::unsetValue() != datetime.time()
     && datetime < *s_epoch_p) {
        return 1;                                                     // RETURN
    }

    bsls_PlatformUtil::Int64 seconds =
                                   ((datetime - *s_epoch_p).totalMilliseconds()
                                              - datetime.millisecond()) / 1000;
    if (bsls_PlatformUtil::Uint64(seconds) > 0x7FFFFFFFULL) {       // 2^31 - 1
        return 1;                                                   // OVERFLOW
    }

    *result = (bsl::time_t)seconds;
    return 0;
}

inline
bsl::time_t bdetu_Epoch::convertToTimeT(const bdet_Datetime& datetime)
{
    return (bsl::time_t) (((datetime - *s_epoch_p).totalMilliseconds()
                                             - datetime.millisecond()) / 1000);
}

inline
void bdetu_Epoch::convertFromTimeT(bdet_Datetime *result, bsl::time_t time)
{
    BSLS_ASSERT_SAFE(result);

    *result = *s_epoch_p;
    result->addSeconds(time);
}

inline
bdet_Datetime bdetu_Epoch::convertFromTimeT(bsl::time_t time)
{
    bdet_Datetime datetime(*s_epoch_p);
    datetime.addSeconds(time);
    return datetime;
}

inline
int bdetu_Epoch::convertToTimeInterval(bdet_TimeInterval    *result,
                                       const bdet_Datetime&  datetime)
{
    BSLS_ASSERT_SAFE(result);

    // Avoid doing '<' comparison of 'datetime' until we're sure the time
    // portion is not unset.

    if (bdetu_Unset<bdet_Time>::unsetValue() != datetime.time()
     && datetime < *s_epoch_p) {
        return 1;                                                     // RETURN
    }
    result->setInterval((datetime - *s_epoch_p).totalSeconds(),
                        (datetime - *s_epoch_p).milliseconds() * 1000000);
    return 0;
}

inline
bdet_TimeInterval bdetu_Epoch::convertToTimeInterval(
                                                 const bdet_Datetime& datetime)
{
    return bdet_TimeInterval(
                           (datetime - *s_epoch_p).totalSeconds(),
                           (datetime - *s_epoch_p).milliseconds() * 1000000);
}

inline
void bdetu_Epoch::convertFromTimeInterval(
                                        bdet_Datetime            *result,
                                        const bdet_TimeInterval&  timeInterval)
{
    BSLS_ASSERT_SAFE(result);

    *result = *s_epoch_p;
    result->addSeconds(timeInterval.seconds());
    result->addMilliseconds(timeInterval.nanoseconds() / 1000000);
}

inline
bdet_Datetime bdetu_Epoch::convertFromTimeInterval(
                                         const bdet_TimeInterval& timeInterval)
{
    bdet_Datetime datetime(*s_epoch_p);
    datetime.addSeconds(timeInterval.seconds());
    datetime.addMilliseconds(timeInterval.nanoseconds() / 1000000);
    return datetime;
}

inline
int bdetu_Epoch::convertToDatetimeInterval(bdet_DatetimeInterval *result,
                                           const bdet_Datetime&   datetime)
{
    BSLS_ASSERT_SAFE(result);

    // Avoid doing '<' comparison of 'datetime' until we're sure the time
    // portion is not unset.

    if (bdetu_Unset<bdet_Time>::unsetValue() != datetime.time()
     && datetime < *s_epoch_p) {
        return 1;                                                     // RETURN
    }
    *result = datetime - *s_epoch_p;
    return 0;
}

inline
bdet_DatetimeInterval bdetu_Epoch::convertToDatetimeInterval(
                                                 const bdet_Datetime& datetime)
{
    return datetime - *s_epoch_p;
}

inline
void bdetu_Epoch::convertFromDatetimeInterval(
                                bdet_Datetime                *result,
                                const bdet_DatetimeInterval&  datetimeInterval)
{
    BSLS_ASSERT_SAFE(result);

    *result = *s_epoch_p;
    result->addMilliseconds(datetimeInterval.totalMilliseconds());
}

inline
bdet_Datetime bdetu_Epoch::convertFromDatetimeInterval(
                                 const bdet_DatetimeInterval& datetimeInterval)
{
    return *s_epoch_p + datetimeInterval;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
