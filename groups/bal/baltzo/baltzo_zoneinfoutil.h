// baltzo_zoneinfoutil.h                                              -*-C++-*-
#ifndef INCLUDED_BALTZO_ZONEINFOUTIL
#define INCLUDED_BALTZO_ZONEINFOUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utility operations on 'baltzo::Zoneinfo' objects.
//
//@CLASSES:
//  baltzo::ZoneinfoUtil: utility for operations on a 'baltzo::Zoneinfo' object
//
//@SEE_ALSO: baltzo_zoneinfo, baltzo_localtimevalidity
//
//@DESCRIPTION: This component provides a suite of pure functions that operate
// on objects of type 'baltzo::Zoneinfo'.  A 'baltzo::Zoneinfo' is a value
// semantic-type providing information about a time zone, mirroring the
// information found in the Zoneinfo Database, a public-domain distribution of
// time zone data (see 'baltzo_zoneinfo' for more information).  The primary
// functions provided by 'baltzo::ZoneinfoUtil' are 'convertUtcToLocalTime' and
// 'loadRelevantTransitions': 'convertUtcToLocalTime' converts a UTC time into
// its corresponding local time in the time zone described by the supplied
// 'baltzo::Zoneinfo' object; 'loadRelevantTransitions' returns the transition,
// from the supplied 'baltzo::Zoneinfo' object's list of transitions that
// describes the attributes of local time in effect at supplied local time;
// returning two possible transitions in instances where the supplied local
// time is either invalid or ambiguous (see 'baltzo_localtimevalidity').  Note
// that the time supplied as input to 'convertUtcToLocalTime' is a *UTC* time,
// whereas the time supplied as input to 'loadRelevantTransitions' is a *local*
// time.
//
///Determining Relevant Transitions with 'loadRelevantTransitions'
///---------------------------------------------------------------
// The function 'loadRelevantTransitions' is used to find the transition in a
// 'baltzo::Zoneinfo' object that describes the properties of a client supplied
// local time value.  In instances where the client supplied local time is
// either ambiguous or invalid 'loadRelevantTransitions' returns an alternative
// transition that refers to a second set of properties that could be used to
// describe the supplied local time.  To understand why multiple transitions
// might be needed, consider the impact of daylight-saving time transitions on
// local time in New York:
//..
// Figure 1: Mapping between UTC Time and New York Local Time
//
//    EST - Eastern Standard Time (UTC-5:00)
//    EDT - Eastern Daylight Time (UTC-4:00)
//
//                                        ,-invalid times    ,- ambiguous times
//                                    T2 /  T3           T4 /   T5
//                                    @-----O            @------O
//
//               T1      EST          T2                 T4        EST
// New York Time @--------------------O                  @------------------>
//                                    |     T3    EDT    |      T5
//                                    |     @------------|------O
//                                   /   __/            /    __/
//                                 /  __/              /  __/
//                                /__/                /__/
//      UTC Time +---------------+-------------------+---------------------->
//               |               |                   |
//               Transition 1    Transition 2        Transition 3
//               (to EST)        (to EDT)            (to EST)
//
//..
// In New York, clocks are set forward an hour in the spring, creating the
// discontinuity between T2 and T3 in the diagram.  A New York local time value
// in the range [T2, T3) is considered invalid, because a correctly set clock
// in New York would never display that value.  Similarly, clocks are set back
// an hour in the fall, creating the discontinuity between T4 and T5 in the
// diagram.  A New York local time value in the range [T4, T5) is considered
// ambiguous, as local times in that range occur twice.  For either invalid or
// ambiguous times, 'loadRelevantTransitions' will return two distinct
// iterators referring to the adjacent transitions holding the two descriptions
// that might be applied to that local time.
//
// For example, consider the returned validity and transitions for the
// following New York local times:
//..
//  New York Local Time  Validity     1st Transition      2nd Transition
//  -------------------  -----------  ------------------  ------------------
//  Jan  1, 2010 2:30am  *_UNIQUE     Nov  1, 2009 (EST)  Nov  1, 2009 (EST)
//  Mar 14, 2010 2:30am  *_INVALID    Nov  1, 2009 (EST)  Mar 14, 2010 (EDT)
//  Nov  7, 2010 1:30am  *_AMBIGUOUS  Mar 14, 2010 (EDT)  Nov  7, 2010 (EST)
//..
//
///Well-Formed Time Zone Information
///---------------------------------
// The primary operations provided by this component require the supplied
// Zoneinfo value meet certain constraints that are not enforced by the
// 'baltzo::Zoneinfo' type itself.  A Zoneinfo meeting these constraints is
// considered *well-formed*, and 'baltzo::ZoneinfoUtil::isWellFormed' will
// return 'true' for such a value.  Specifically, a 'baltzo::Zoneinfo' object
// is considered well-formed only if *all* of the following are true:
//
//: 1 The Zoneinfo provides at least one transition.
//:
//: 2 The first transition in the Zoneinfo is at the first representable
//:   'bdlt::Datetime' value, "Jan 01, 0001 00:00" -- i.e.,
//:   'bdlt::Datetime(1, 1, 1)'.
//:
//: 3 There is no transition in the ordered sequence of transitions described
//:   by the Zoneinfo where local clock time is adjusted (either forwards or
//:   backwards) introducing a period of invalid or ambiguous local times,
//:   where that range of invalid or ambiguous local times overlaps with the
//:   range of invalid or ambiguous local times introduced by subsequent
//:   transition.
//
// Note that 'baltzo::ZoneinfoUtil::isWellFormed' has linear complexity with
// respect to the number of transitions that the Zoneinfo value defines.
//
///Overlapping Transitions
///- - - - - - - - - - - -
// In order to better understand the 3rd constraint (above) on a well-formed
// Zoneinfo object, first, notice that Figure 1 above (showing local time in
// New York) illustrates a well-formed sequence of transitions.  Both
// Transition 2 (to EDT) and Transition 3 (to EST) introduce a range of
// ambiguous or invalid times (ambiguous when clocks are adjusted backwards,
// invalid when clocks are adjusted forward), but those two ranges of ambiguous
// and invalid local times do not overlap.
//
// However, a Zoneinfo object is not well-formed if two transitions occur so
// close together that the respective ranges of invalid or ambiguous times that
// those transitions introduce, overlap with one and other, as illustrated in
// Figure 2.
//..
//  Figure 2:  Overlapping Transitions
//
//  Standard Time (STD):        UTC+00:00
//  Daylight-Saving Time (DST): UTC+01:00
//
//  Transition 1 (to DST): At 00:00 UTC (00:00 local)
//  Transition 2 (to STD): At 00:30 UTC (01:30 local)
//
//
//                                   (00:30)    STD
//                    STD   (00:00)    @--------------------------
//    Local Time @------------O        |           DST
//                            |        | (01:00) @------O (01:30)
//                            |        |      __/     _/
//                             \        \  __/     __/
//                              \      __\/     __/
//                               \ ___/   \ __/
//      UTC Time +---------------+---------+----------------------------
//                          Transition 1   Transition 2
//                           (01:00 UTC)   (01:30 UTC)
//..
// Notice that between [ 00:30 .. 01:00 ] local time, the range of invalid
// times introduced by Transition 1, overlaps with the range of ambiguous times
// introduced by Transition 2.  The above time zone would therefore *not* be
// well-formed.
//
///Usage
///-----
// The following examples demonstrate how to use a 'ZoneinfoUtil' to perform
// common operations on time values using a Zoneinfo description of a time
// zone.
//
///Prologue: Initializing a 'baltzo::Zoneinfo' Object
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// We start by creating a Zoneinfo time zone description for New York, which we
// will use in subsequent examples.  Note that, in practice, clients should
// obtain time zone information from a data source (see
// 'baltzo_zoneinfocache').
//
// First we create a Zoneinfo object for New York, and populate 'newYork' with
// the correct time zone identifier:
//..
//  baltzo::Zoneinfo newYork;
//  newYork.setIdentifier("America/New_York");
//..
// Next we create two local-time descriptors, one for standard time and one for
// daylight-saving time:
//..
//  baltzo::LocalTimeDescriptor est(-18000, false, "EST");
//  baltzo::LocalTimeDescriptor edt(-14400, true,  "EDT");
//..
// Then we set the initial descriptor for 'newYork' to Eastern Standard Time.
// Note that such an initial transition is required for a 'baltzo::Zoneinfo'
// object to be considered Well-Formed (see 'isWellFormed'):
//..
//  newYork.addTransition(bdlt::EpochUtil::convertToTimeT64(
//                                                    bdlt::Datetime(1, 1, 1)),
//                        est);
//..
// Next we create a series of transitions between these local-time descriptors
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
//      newYork.addTransition(bdlt::EpochUtil::convertToTimeT64(
//                                                        TRANSITION_TIMES[i]),
//                            edt);
//      newYork.addTransition(bdlt::EpochUtil::convertToTimeT64(
//                                                    TRANSITION_TIMES[i + 1]),
//                            est);
//  }
//..
// Finally we verify that the time zone information we've created is considered
// well-formed (as discussed above):
//..
//  assert(true == baltzo::ZoneinfoUtil::isWellFormed(newYork));
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
// Now, we call 'convertUtcToLocalTime' and supply as input both 'utcTime' and
// the Zoneinfo description for 'newYork' (which we initialized in the prologue
// above):
//..
//  bdlt::DatetimeTz                          localNYTime;
//  baltzo::Zoneinfo::TransitionConstIterator iterator;
//  baltzo::ZoneinfoUtil::convertUtcToLocalTime(&localNYTime,
//                                              &iterator,
//                                              utcTime,
//                                              newYork);
//..
// Then we verify that 'localNYTime' is "Dec 12, 2010 10:00+5:00", the time in
// New York corresponding to the UTC time "Dec 12, 2010 15:00".
//..
//  assert(utcTime                          == localNYTime.utcDatetime());
//  assert(bdlt::Datetime(2010, 12, 12, 10) == localNYTime.localDatetime());
//  assert(-5 * 60                          == localNYTime.offset());
//..
// Finally, we verify that the returned 'iterator' refers to the local-time
// transition immediately before 'utcTime', and that that transition refers to
// a local-time descriptor characterizing standard-time in New York:
//..
//  baltzo::Zoneinfo::TransitionConstIterator transitionIter     = iterator;
//  baltzo::Zoneinfo::TransitionConstIterator nextTransitionIter = ++iterator;
//
//  const bdlt::EpochUtil::TimeT64 utcTimeT =
//                                   bdlt::EpochUtil::converToTimeT64(utcTime);
//  assert(utcTimeT >= transitionIter->transition());
//  assert(utcTimeT <  nextTransitionIter->transition());
//
//  assert(false        == transitionIter->descriptor().dstInEffectFlag());
//  assert(-5 * 60 * 60 == transitionIter->descriptor().utcOffsetInSeconds());
//  assert("EST"        == transitionIter->descriptor().description());
//..
//
///Example 2: Determining the Type of a Local Time
///- - - - - - - - - - - - - - - - - - - - - - - -
// In this next example we use 'loadRelevantTransitions' to determine the
// local-time descriptor (see 'baltzo_localtimedescriptor') that applies to a
// local time value, represented using a 'bdlt::Datetime' object.
//
// We start by defining a 'bdlt::Datetime' object for "Jan 1, 2011 12:00" in
// New York:
//..
//  bdlt::Datetime nyLocalTime(2011, 1, 1, 12);
//..
// Then, we call 'loadRelevantTransitions', and supply, as input, both
// 'nyLocalTime' and the Zoneinfo description for 'newYork' (which we
// initialized in the prologue above):
//..
//  baltzo::LocalTimeValidity::Enum           validity;
//  baltzo::Zoneinfo::TransitionConstIterator firstTransition;
//  baltzo::Zoneinfo::TransitionConstIterator secondTransition;
//  baltzo::ZoneinfoUtil::loadRelevantTransitions(&firstTransition,
//                                                &secondTransition,
//                                                &validity,
//                                                nyLocalTime,
//                                                newYork);
//..
// "Jan 1, 2011 12:00" in New York, is not near a daylight-saving time
// transition, so it uniquely describes a valid time (in New York) which falls
// during Eastern Standard Time, and whose local time offset from UTC is -5:00.
// Because "Jan 1, 2011 12:00" is both a valid and unique local time, the
// returned validity will be 'baltzo::LocalTimeValidity::e_VALID_UNIQUE' and
// the two returned transition iterators will be equal:
//..
//  assert(baltzo::LocalTimeValidity::e_VALID_UNIQUE == validity);
//  assert(firstTransition == secondTransition);
//
//  assert(false    == firstTransition->descriptor().dstInEffectFlag());
//  assert(-5*60*60 == firstTransition->descriptor().utcOffsetInSeconds());
//  assert("EST"    == firstTransition->descriptor().description());
//..
// Next, we create a second 'bdlt::Datetime' object to represent "Nov 7, 2010
// 1:30" in New York.  Note that the clock time "Nov 7, 2010 1:30" occurred
// twice in New York, as clocks were set back by an hour an instant before the
// local clock would have reached "Nov 7, 2010 02:00 EDT", and it is therefore
// ambiguous which of those two values that local time is meant to refer.
//..
//  bdlt::Datetime ambiguousLocalTime(2010, 11, 7, 1, 30);
//..
// Now, we call 'loadRelevantTransitions', this time supplying
// 'ambiguousLocalTime':
//..
//  baltzo::ZoneinfoUtil::loadRelevantTransitions(&firstTransition,
//                                                &secondTransition,
//                                                &validity,
//                                                ambiguousLocalTime,
//                                                newYork);
//..
// Finally we observe that the local time was ambiguous and that the returned
// transitions are distinct:
//..
//  assert(baltzo::LocalTimeValidity::e_VALID_AMBIGUOUS == validity);
//  assert(firstTransition != secondTransition);
//..
// Because 'ambiguousLocalTime' may refer to either the standard or the
// daylight-saving time value "Nov 7, 2010 01:30", the returned validity will
// be 'e_VALID_AMBIGUOUS', and the 'first' and 'second' iterators will differ.
// 'first' will refer to a description of the local time before the transition
// (daylight-saving time) and 'second' will refer to a description of local
// time after the transition (standard-time):
//..
//  assert(true      == firstTransition->descriptor().dstInEffectFlag());
//  assert(-4*60*60  == firstTransition->descriptor().utcOffsetInSeconds());
//  assert("EDT"     == firstTransition->descriptor().description());
//
//  assert(false     == secondTransition->descriptor().dstInEffectFlag());
//  assert(-5*60*60  == secondTransition->descriptor().utcOffsetInSeconds());
//  assert("EST"     == secondTransition->descriptor().description());
//..
// Note that the two transitions returned are adjacent:
//..
//  ++firstTransition;
//  assert(firstTransition == secondTransition);
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALTZO_LOCALTIMEVALIDITY
#include <baltzo_localtimevalidity.h>
#endif

#ifndef INCLUDED_BALTZO_ZONEINFO
#include <baltzo_zoneinfo.h>
#endif

#ifndef INCLUDED_BDLT_DATETIME
#include <bdlt_datetime.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMETZ
#include <bdlt_datetimetz.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace baltzo {
                             // ==================
                             // class ZoneinfoUtil
                             // ==================

struct ZoneinfoUtil {
    // This 'struct' provides a namespace for utility operations using a
    // 'Zoneinfo' object.

    // CLASS METHODS
    static void convertUtcToLocalTime(
                           bdlt::DatetimeTz                  *resultTime,
                           Zoneinfo::TransitionConstIterator *resultTransition,
                           const bdlt::Datetime&              utcTime,
                           const Zoneinfo&                    timeZone);
        // Load, into the specified 'resultTime', the local date-time value, in
        // the specified 'timeZone', corresponding to the specified 'utcTime',
        // and load, into the specified 'resultTransition', an iterator
        // referring to the first transition in 'timeZone' whose
        // transition-time is before 'utcTime'.  The behavior is undefined
        // unless 'isWellFormed(timeZone)' returns 'true'.

    static void loadRelevantTransitions(
                     Zoneinfo::TransitionConstIterator *firstResultTransition,
                     Zoneinfo::TransitionConstIterator *secondResultTransition,
                     LocalTimeValidity::Enum           *resultValidity,
                     const bdlt::Datetime&              localTime,
                     const Zoneinfo&                    timeZone);
        // Load, into the specified 'firstResultTransition', an iterator
        // referring to the transition describing the characteristics of local
        // time in effect at the specified 'localTime' in the specified
        // 'timeZone'; in instances where 'localTime' is not both a unique and
        // valid local time in 'timeZone', load, into the specified
        // 'secondResultTransition', an iterator referring to a subsequent
        // transition describing the alternative characteristics of local time
        // that could also apply to 'localTime'; finally load, into the
        // specified 'resultValidity', the validity of 'localTime' as being
        // unique, ambiguous but valid, or invalid.  If 'resultValidity' is
        // 'e_VALID_UNIQUE', then 'firstResultTransition' and
        // 'secondResultTransition' will be loaded with the same transition,
        // otherwise the returned transitions will be distinct with
        // 'secondResultTransition' referring to the transition immediately
        // after 'firstResultTransition'.  The behavior is undefined unless
        // 'isWellFormed(timeZone)' is 'true', and
        // 'firstResultTransition != secondResultTransition'.

    static bool isWellFormed(const Zoneinfo& timeZone);
        // Return 'true' if the specified 'timeZone' is a well-formed Zoneinfo
        // object (which can be used by other methods on this utility), and
        // 'false' otherwise.  For a Zoneinfo to be considered well-formed
        // *all* of the following must be true:
        //
        //: 1 'timeZone.numTransitions() > 0'
        //:
        //: 2 The first transition in 'timeZone' is at the first representable
        //:   'bdlt::Datetime' value, "Jan 01, 0001 00 00.000" -- i.e.,
        //:   'bdlt::Datetime(1, 1, 1)'.
        //:
        //: 3 There is no transition in the ordered sequence of transitions
        //:   described by 'timeZone' where the local clock time is adjusted
        //:   (either forwards or backwards) introducing a period of invalid or
        //:   ambiguous local times, where that range of invalid or ambiguous
        //:   local times overlaps with a range of invalid or or ambiguous
        //:   local times introduced by the subsequent transition (see
        //:   component documentation for an illustration).
        //
        // Note that this method has linear worst-case time complexity with
        // respect to 'timeZone.numTransitions()'.
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
