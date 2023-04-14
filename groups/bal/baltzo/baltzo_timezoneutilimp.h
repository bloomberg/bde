// baltzo_timezoneutilimp.h                                           -*-C++-*-
#ifndef INCLUDED_BALTZO_TIMEZONEUTILIMP
#define INCLUDED_BALTZO_TIMEZONEUTILIMP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Implement utilities for converting times between time zones.
//
//@CLASSES:
//  baltzo::TimeZoneUtilImp: implementation utilities for converting times
//
//@SEE_ALSO: baltzo_localdatetime, baltzo_zoneinfo,
//           baltzo_defaultzoneinfocache
//
//@DESCRIPTION: This component provides a namespace, 'baltzo::TimeZoneUtilImp',
// containing a set of utility functions for converting time values to and
// from, their corresponding local time representations in (possibly) different
// time zones.  The primary methods provided include: 'convertUtcToLocalTime'
// for converting time values to their corresponding local-time values in some
// time zone; 'convertLocalToUtc', for converting a local-time value into the
// corresponding UTC time value; and 'initLocalTime' for initializing a
// local-time value.  Additionally the 'loadLocalTimeForUtc' method enable
// clients to obtain information about a time value, such as whether the
// provided time is a daylight-saving time value.
//
///Usage
///-----
// The following examples demonstrate how to use a 'baltzo::TimeZoneUtilImp' to
// perform common operations on time values:
//
///Prologue: Initializing an Example 'baltzo::ZoneinfoCache' Object
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Before using the methods provided by 'baltzo::TimeZoneUtilImp' we must first
// define a 'baltzo::ZoneinfoCache' object containing information about various
// time zones.  For the purposes of this example, we will define a sample cache
// containing only data for New York loaded through a 'baltzo::TestLoader'
// object.  Note that, in general, clients should use data from an external
// data source (see 'baltzo_datafileloader').
//
// First, we create a Zoneinfo object for New York, and populate 'newYork' with
// a correct time zone identifier:
//..
//  baltzo::Zoneinfo newYork;
//  newYork.setIdentifier("America/New_York");
//..
// Next, we create two local-time descriptors, one for standard time and one
// for daylight-saving time:
//..
//  baltzo::LocalTimeDescriptor est(-18000, false, "EST");
//  baltzo::LocalTimeDescriptor edt(-14400, true,  "EDT");
//..
// Then, we set the initial descriptor for 'newYork' to Eastern Standard Time.
// Note that such an initial transition is required for a 'baltzo::Zoneinfo'
// object to be considered Well-Defined (see 'baltzo_zoneinfoutil')
//..
//  const bsls::Epoch::TimeT64 firstTransitionTime =
//                  bdlt::EpochUtil::convertToTimeT64(bdlt::Datetime(1, 1, 1));
//
//  newYork.addTransition(firstTransitionTime, est);
//..
// Next, we create a series of transitions between these local-time descriptors
// for the years 2007-2011.  Note that the United States transitions to
// daylight saving time on the second Sunday in March, at 2am local time (07:00
// UTC), and transitions back to standard time on the first Sunday in November
// at 2am local time (06:00 UTC), resulting in an even number of transitions:
//..
//  static const bdlt::Datetime TRANSITION_TIMES[] = {
//      bdlt::Datetime(2007,  3, 11, 7),
//      bdlt::Datetime(2007, 11,  4, 6),
//      bdlt::Datetime(2008,  3,  9, 7),
//      bdlt::Datetime(2008, 11,  2, 6),
//      bdlt::Datetime(2009,  3,  8, 7),
//      bdlt::Datetime(2009, 11,  1, 6),
//      bdlt::Datetime(2010,  3, 14, 7),
//      bdlt::Datetime(2010, 11,  7, 6),
//      bdlt::Datetime(2011,  3, 13, 7),
//      bdlt::Datetime(2011, 11,  6, 6),
//  };
//  const int NUM_TRANSITION_TIMES =
//                          sizeof TRANSITION_TIMES / sizeof *TRANSITION_TIMES;
//  assert(0 == NUM_TRANSITION_TIMES % 2);
//
//  for (int i = 0; i < NUM_TRANSITION_TIMES; i += 2) {
//
//      const bsls::Epoch::TimeT64 edtTransitionTime =
//                      bdlt::EpochUtil::convertToTimeT64(TRANSITION_TIMES[i]);
//      newYork.addTransition(edtTransitionTime, edt);
//
//      const bsls::Epoch::TimeT64 estTransitionTime =
//                  bdlt::EpochUtil::convertToTimeT64(TRANSITION_TIMES[i + 1]);
//      newYork.addTransition(estTransitionTime, est);
//  }
//..
// Next, we verify that the time zone information we have created is considered
// well-defined (as discussed above):
//..
//  assert(true == baltzo::ZoneinfoUtil::isWellFormed(newYork));
//..
// Finally, we create a 'baltzo::TestLoader' object, provide it the description
// of 'newYork', and use it to initialize a 'baltzo::ZoneinfoCache' object:
//..
//  baltzo::TestLoader loader;
//  loader.setTimeZone(newYork);
//  baltzo::ZoneinfoCache cache(&loader);
//..
//
///Example 1: Converting from a UTC Time to a Local Time
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we demonstrate how to convert a UTC time to the
// corresponding local time using the 'convertUtcToLocalTime' class method.
//
// We start by creating a 'bdlt::Datetime' representing the UTC time "Dec 12,
// 2010 15:00":
//..
//  bdlt::Datetime utcTime(2010, 12, 12, 15, 0, 0);
//..
// Now, we call 'convertUtcToLocalTime' and supply as input 'utcTime', the time
// zone identifier for New York ("America/New_York"), and the cache of time
// zone information created in the prologue:
//..
//  bdlt::DatetimeTz localNYTime;
//  baltzo::TimeZoneUtilImp::convertUtcToLocalTime(&localNYTime,
//                                                "America/New_York",
//                                                utcTime,
//                                                &cache);
//..
// Finally we verify that 'localNYTime' is "Dec 12, 2010 10:00+5:00", the time
// in New York corresponding to the UTC time "Dec 12, 2010 15:00".
//..
//  assert(utcTime                         == localNYTime.utcDatetime());
//  assert(bdlt::Datetime(2010, 12, 12, 10) == localNYTime.localDatetime());
//  assert(-5 * 60                         == localNYTime.offset());
//..

#include <balscm_version.h>

#include <baltzo_dstpolicy.h>
#include <baltzo_localtimevalidity.h>
#include <baltzo_zoneinfo.h>

#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>

#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace baltzo {

class LocalTimePeriod;
class ZoneinfoCache;

                           // =====================
                           // class TimeZoneUtilImp
                           // =====================

struct TimeZoneUtilImp {
    // This 'struct' provides a namespace for utility functions that convert
    // time values to, and from, local time.

    // CLASS METHODS
    static int convertUtcToLocalTime(bdlt::DatetimeTz      *result,
                                     const char            *resultTimeZoneId,
                                     const bdlt::Datetime&  utcTime,
                                     ZoneinfoCache         *cache);
        // Load, into the specified 'result', the local date-time value, in the
        // time zone indicated by the specified 'resultTimeZoneId',
        // corresponding to the specified 'utcTime', using time zone
        // information supplied by the specified 'cache'.  Return 0 on success,
        // and a non-zero value otherwise.  A return status of
        // 'ErrorCode::k_UNSUPPORTED_ID' indicates that 'resultTimeZoneId' is
        // not recognized, and a return status of 'ErrorCode::k_OUT_OF_RANGE'
        // indicates that an out of range value of 'result' would have
        // occurred.

    static void createLocalTimePeriod(
                          LocalTimePeriod                          *result,
                          const Zoneinfo::TransitionConstIterator&  transition,
                          const Zoneinfo&                           timeZone);
        // Load, into the specified 'result', attributes characterizing local
        // time indicated by the specified 'transition' in the specified
        // 'timeZone'.  The behavior is undefined unless
        // 'ZoneinfoUtil::isWellFormed(timeZone)' is 'true' and 'transition' is
        // a valid, non-ending, iterator into the sequence of transitions
        // described by 'timeZone'.

    static int initLocalTime(bdlt::DatetimeTz         *result,
                             LocalTimeValidity::Enum  *resultValidity,
                             const bdlt::Datetime&     localTime,
                             const char               *timeZoneId,
                             DstPolicy::Enum           dstPolicy,
                             ZoneinfoCache            *cache);
        // Load, into the specified 'result', the local date-time value --
        // including the local date, time, and resolved UTC offset -- indicated
        // by the specified 'localTime' in the time zone indicated by the
        // specified 'timeZoneId', using the specified 'dstPolicy' to interpret
        // whether or not 'localTime' represents a daylight-saving time value,
        // and using time zone information supplied by the specified 'cache'.
        // Load, into the specified 'resultValidity' the value indicating the
        // whether 'localTime' is unique, ambiguous but valid, or invalid.
        // Return 0 on success, and a non-zero value otherwise.  A return
        // status of 'ErrorCode::k_UNSUPPORTED_ID' indicates that 'timeZoneId'
        // is not recognized.

    static int loadLocalTimePeriodForUtc(LocalTimePeriod       *result,
                                         const char            *timeZoneId,
                                         const bdlt::Datetime&  utcTime,
                                         ZoneinfoCache         *cache);
        // Load, into the specified 'result', attributes characterizing local
        // time at the specified 'utcTime' in the time zone indicated by the
        // specified 'timeZoneId' (e.g., the offset from UTC, whether it is
        // daylight-saving time), as well as the time interval over which those
        // attributes apply, using time zone information supplied by the
        // specified 'cache'.  Return 0 on success, and a non-zero value
        // otherwise.  A return status of 'ErrorCode::k_UNSUPPORTED_ID'
        // indicates that 'timeZoneId' is not recognized.

    static void resolveLocalTime(
                             bdlt::DatetimeTz                  *result,
                             LocalTimeValidity::Enum           *resultValidity,
                             Zoneinfo::TransitionConstIterator *transitionIter,
                             const bdlt::Datetime&              localTime,
                             DstPolicy::Enum                    dstPolicy,
                             const Zoneinfo&                    timeZone);
        // Load, into the specified 'result', the local time and UTC offset of
        // the specified 'localTime' in the specified 'timeZone', using the
        // specified 'dstPolicy' to interpret whether or not 'localTime'
        // represents a daylight-saving time value; load into the specified
        // 'resultValidity' an indication of whether 'localTime' is valid and
        // unique, valid but ambiguous, or invalid; load into the specified
        // 'transitionIter' an iterator pointing to the transition that
        // characterizes the attributes of 'localTime'.  The behavior is
        // undefined unless 'ZoneinfoUtil::isWellFormed(timeZone)' is 'true'.

};

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
