// baetzo_timezoneutilimp.h                                           -*-C++-*-
#ifndef INCLUDED_BAETZO_TIMEZONEUTILIMP
#define INCLUDED_BAETZO_TIMEZONEUTILIMP

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Implement utilities for converting times between time zones.
//
//@CLASSES:
//  baetzo_TimeZoneUtilImp: implementation utilities for converting times
//
//@SEE_ALSO: baet_localdatetime, baetzo_zoneinfo,
//           baetzo_defaultzoneinfocache
//
//@AUTHOR: Stefano Pacifico (spacifico1), Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a namespace, 'baetzo_TimeZoneUtilImp',
// containing a set of utility functions for converting time values to and
// from, their corresponding local time representations in (possibly) different
// time zones.  The primary methods provided include: 'convertUtcToLocalTime'
// for converting time values to their corresponding local-time values in some
// time zone; 'convertLocalToUtc', for converting a local-time value into the
// corresponding UTC time value; and 'initLocalTime' for initializing a
// local-time value.  Additionally the 'loadLocalTimeForUtc' method enable
// clients to obtain information about a time value, such as whether the
// provided time is a daylight-savings time value.
//
///Usage
///-----
// The following examples demonstrate how to use a 'baetzo_TimeZoneUtilImp' to
// perform common operations on time values:
//
///Prologue: Initializing an Example 'baetzo_ZoneinfoCache' Object.
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Before using the methods provided by 'baetzo_TimeZoneUtilImp' we must first
// define a 'baetzo_ZoneinfoCache' object containing information about various
// time zones.  For the purposes of this example, we will define a sample
// cache containing only data for New York loaded through a
// 'baetzo_TestLoader' object.  Note that, in general, clients should use data
// from an external data source (see 'baetzo_datafileloader').
//
// First, we create a Zoneinfo object for New York, and populate 'newYork' with
// a correct time zone identifier:
//..
//  baetzo_Zoneinfo newYork;
//  newYork.setIdentifier("America/New_York");
//..
// Next, we create two local-time descriptors, one for standard time and one
// for daylight-saving time:
//..
//  baetzo_LocalTimeDescriptor est(-18000, false, "EST");
//  baetzo_LocalTimeDescriptor edt(-14400, true,  "EDT");
//..
// Then, we set the initial descriptor for 'newYork' to Eastern Standard
// Time.  Note that such an initial transition is required for a
// 'baetzo_Zoneinfo' object to be considered Well-Defined (see
// 'baetzo_zoneinfoutil')
//..
//  const bsls_Epoch::TimeT64 firstTransitionTime =
//                       bdetu_Epoch::convertToTimeT64(bdet_Datetime(1, 1, 1));
//
//  newYork.addTransition(firstTransitionTime, est);
//..
// Next, we create a series of transitions between these local-time descriptors
// for the years 2007-2011.  Note that the United States transitions to
// daylight savings time on the second Sunday in March, at 2am local time
// (07:00 UTC), and transitions back to standard time on the first Sunday in
// November at 2am local time (06:00 UTC), resulting in an even number of
// transitions:
//..
//  static const bdet_Datetime TRANSITION_TIMES[] = {
//      bdet_Datetime(2007,  3, 11, 7),
//      bdet_Datetime(2007, 11,  4, 6),
//      bdet_Datetime(2008,  3,  9, 7),
//      bdet_Datetime(2008, 11,  2, 6),
//      bdet_Datetime(2009,  3,  8, 7),
//      bdet_Datetime(2009, 11,  1, 6),
//      bdet_Datetime(2010,  3, 14, 7),
//      bdet_Datetime(2010, 11,  7, 6),
//      bdet_Datetime(2011,  3, 13, 7),
//      bdet_Datetime(2011, 11,  6, 6),
//  };
//  const int NUM_TRANSITION_TIMES =
//                          sizeof TRANSITION_TIMES / sizeof *TRANSITION_TIMES;
//  assert(0 == NUM_TRANSITION_TIMES % 2);
//
//  for (int i = 0; i < NUM_TRANSITION_TIMES; i += 2) {
//
//      const bsls_Epoch::TimeT64 edtTransitionTime =
//                          bdetu_Epoch::convertToTimeT64(TRANSITION_TIMES[i]);
//      newYork.addTransition(edtTransitionTime, edt);
//
//      const bsls_Epoch::TimeT64 estTransitionTime =
//                      bdetu_Epoch::convertToTimeT64(TRANSITION_TIMES[i + 1]);
//      newYork.addTransition(estTransitionTime, est);
//  }
//..
// Next, we verify that the time zone information we have created is
// considered well-defined (as discussed above):
//..
//  assert(true == baetzo_ZoneinfoUtil::isWellFormed(newYork));
//..
// Finally, we create a 'baetzo_TestLoader' object, provide it the description
// of 'newYork', and use it to initialize a 'baetzo_ZoneinfoCache' object:
//..
//  baetzo_TestLoader loader;
//  loader.setTimeZone(newYork);
//  baetzo_ZoneinfoCache cache(&loader);
//..
//
///Example 1: Converting from a UTC Time to a Local Time.
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we demonstrate how to convert a UTC time to the
// corresponding local time using the 'convertUtcToLocalTime' class method.
//
// We start by creating a 'bdet_Datetime' representing the UTC time
// "Dec 12, 2010 15:00":
//..
//  bdet_Datetime utcTime(2010, 12, 12, 15, 0, 0);
//..
// Now, we call 'convertUtcToLocalTime' and supply as input 'utcTime', the
// time zone identifier for New York ("America/New_York"), and the cache of
// time zone information created in the prologue:
//..
//  bdet_DatetimeTz localNYTime;
//  baetzo_TimeZoneUtilImp::convertUtcToLocalTime(&localNYTime,
//                                                "America/New_York",
//                                                utcTime,
//                                                &cache);
//..
// Finally we verify that 'localNYTime' is "Dec 12, 2010 10:00+5:00", the time
// in New York corresponding to the UTC time "Dec 12, 2010 15:00".
//..
//  assert(utcTime                         == localNYTime.gmtDatetime());
//  assert(bdet_Datetime(2010, 12, 12, 10) == localNYTime.localDatetime());
//  assert(-5 * 60                         == localNYTime.offset());
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAETZO_DSTPOLICY
#include <baetzo_dstpolicy.h>
#endif

#ifndef INCLUDED_BAETZO_LOCALTIMEVALIDITY
#include <baetzo_localtimevalidity.h>
#endif

#ifndef INCLUDED_BAETZO_ZONEINFO
#include <baetzo_zoneinfo.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class bdet_Datetime;
class bdet_DatetimeTz;

class baetzo_LocalTimePeriod;
class baetzo_ZoneinfoCache;

                     // ============================
                     // class baetzo_TimeZoneUtilImp
                     // ============================

struct baetzo_TimeZoneUtilImp {
    // This 'struct' provides a namespace for utility functions that convert
    // time values to, and from, local time.

    // CLASS METHODS
    static int convertUtcToLocalTime(bdet_DatetimeTz      *result,
                                     const char           *resultTimeZoneId,
                                     const bdet_Datetime&  utcTime,
                                     baetzo_ZoneinfoCache *cache);
        // Load, into the specified 'result', the local date-time value, in the
        // time zone indicated by the specified 'resultTimeZoneId',
        // corresponding to the specified 'utcTime', using time zone
        // information supplied by the specified 'cache'.  Return 0 on success,
        // and a non-zero value otherwise.  A return status of
        // 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that
        // 'resultTimeZoneId' is not recognized.

    static void createLocalTimePeriod(
                   baetzo_LocalTimePeriod                          *result,
                   const baetzo_Zoneinfo::TransitionConstIterator&  transition,
                   const baetzo_Zoneinfo&                           timeZone);
        // Load, into the specified 'result', attributes characterizing local
        // time indicated by the specified 'transition' in the specified
        // 'timeZone'.  The behavior is undefined unless
        // 'baetzo_ZoneinfoUtil::isWellFormed(timeZone)' is 'true' and
        // 'transition' is a valid, non-ending, iterator into the sequence of
        // transitions described by 'timeZone'.

    static int initLocalTime(bdet_DatetimeTz                *result,
                             baetzo_LocalTimeValidity::Enum *resultValidity,
                             const bdet_Datetime&            localTime,
                             const char                     *timeZoneId,
                             baetzo_DstPolicy::Enum          dstPolicy,
                             baetzo_ZoneinfoCache           *cache);
        // Load, into the specified 'result', the local date-time value --
        // including the local date, time, and resolved UTC offset -- indicated
        // by the specified 'localTime' in the time zone indicated by the
        // specified 'timeZoneId', using the specified 'dstPolicy' to interpret
        // whether or not 'localTime' represents a daylight-saving time value,
        // and using time zone information supplied by the specified 'cache'.
        // Load, into the specified 'resultValidity' the value indicating the
        // whether 'localTime' is unique, ambiguous but valid, or invalid.
        // Return 0 on success, and a non-zero value otherwise.  A return
        // status of 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that
        // 'timeZoneId' is not recognized.

    static int loadLocalTimePeriodForUtc(baetzo_LocalTimePeriod *result,
                                         const char             *timeZoneId,
                                         const bdet_Datetime&    utcTime,
                                         baetzo_ZoneinfoCache   *cache);
        // Load, into the specified 'result', attributes characterizing local
        // time at the specified 'utcTime' in the time zone indicated by the
        // specified 'timeZoneId' (e.g., the offset from UTC, whether it is
        // daylight-saving time), as well as the time interval over which those
        // attributes apply, using time zone information supplied by the
        // specified 'cache'.  Return 0 on success, and a non-zero value
        // otherwise.  A return status of
        // 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that
        // 'timeZoneId' is not recognized.

    static void resolveLocalTime(
                     bdet_DatetimeTz                           *result,
                     baetzo_LocalTimeValidity::Enum            *resultValidity,
                     baetzo_Zoneinfo::TransitionConstIterator  *transitionIter,
                     const bdet_Datetime&                       localTime,
                     baetzo_DstPolicy::Enum                     dstPolicy,
                     const baetzo_Zoneinfo&                     timeZone);
        // Load, into the specified 'result', the local time and UTC
        // offset of the specified 'localTime' in the specified 'timeZone',
        // using the specified 'dstPolicy' to interpret whether or not
        // 'localTime' represents a daylight-saving time value; load into the
        // specified 'resultValidity' a indication of whether 'localTime' is
        // valid and unique, valid but ambiguous, or invalid; load into the
        // specified 'transitionIter' an iterator pointing to the transition
        // that characterizes the attributes of 'localTime'.  The behavior is
        // undefined unless 'baetzo_ZoneinfoUtil::isWellFormed(timeZone)' is
        // 'true'.

};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

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
