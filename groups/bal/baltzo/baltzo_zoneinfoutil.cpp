// baltzo_zoneinfoutil.cpp                                            -*-C++-*-
#include <baltzo_zoneinfoutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_zoneinfoutil_cpp,"$Id$ $CSID$")

#include <baltzo_localtimedescriptor.h>
#include <baltzo_zoneinfo.h>

#include <ball_log.h>

#include <bdlt_epochutil.h>

#include <bdlt_datetimeinterval.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

#include <bsls_assert.h>
#include <bsls_types.h>

namespace BloombergLP {

static const char LOG_CATEGORY[] = "BALTZO.ZONEINFOUTIL";

void baltzo::ZoneinfoUtil::convertUtcToLocalTime(
                           bdlt::DatetimeTz                  *resultTime,
                           Zoneinfo::TransitionConstIterator *resultTransition,
                           const bdlt::Datetime&              utcTime,
                           const Zoneinfo&                    timeZone)
{
    BSLS_ASSERT(resultTime);
    BSLS_ASSERT(resultTransition);
    BSLS_ASSERT_SAFE(isWellFormed(timeZone));

    Zoneinfo::TransitionConstIterator it =
                                    timeZone.findTransitionForUtcTime(utcTime);

    BSLS_ASSERT(it != timeZone.endTransitions());

    *resultTransition = it;
    const int offset = it->descriptor().utcOffsetInSeconds();
    const int offsetInMinutes = offset / 60;

    bdlt::Datetime temp(utcTime);
    temp.addMinutes(offsetInMinutes);

    resultTime->setDatetimeTz(temp, offsetInMinutes);
}

void baltzo::ZoneinfoUtil::loadRelevantTransitions(
                     Zoneinfo::TransitionConstIterator *firstResultTransition,
                     Zoneinfo::TransitionConstIterator *secondResultTransition,
                     LocalTimeValidity::Enum           *resultValidity,
                     const bdlt::Datetime&              localTime,
                     const Zoneinfo&                    timeZone)
{
    BSLS_ASSERT(firstResultTransition);
    BSLS_ASSERT(secondResultTransition);
    BSLS_ASSERT(firstResultTransition != secondResultTransition);
    BSLS_ASSERT(resultValidity);
    BSLS_ASSERT_SAFE(isWellFormed(timeZone));

    // Implementation Note:  This operations begins by using 'localTime' as an
    // initial approximation for its corresponding UTC time (this approximation
    // is represented by {*} in the figure below).  'convertUtcToLocalTime' is
    // invoked on this approximation, and the returned iterator is used to
    // determine three potential local-time descriptors that could apply to
    // 'localTime' (Previous, Current, and Next in the figure below).   The
    // transitions between those 3 local-time descriptors define four
    // important local-time values -- T1, T1', T2, and T2' (in the figure
    // below).
    //
    // Note that this diagram is analogous to that shown for New York in the
    // component documentation, except that we have yet to determine whether
    // the two highlighted ranges of local time, [T1, T1') and [T2, T2'],
    // refer to ambiguous times or invalid times.
    //..
    // Figure: Mapping between UTC Time and Local Time
    //
    //                                              either ambiguous or
    //                                       ,----- invalid local times
    //                                      /
    //                                     /___________________
    //                                    /                    |
    //                              @--------O             @--------O
    //
    //                              T1       T1'           T2       T2'
    // Local Time +-----------------O  -  -  @-------------O  -  -  @--------
    //                             /     ___/             /     ___/
    //                            /  ___/                /  ___/
    //                           /__/                   /__/
    // UTC Time   +-------------+---------{*}----------+---------------------
    //            | Previous    | Current              | Next
    //            | Descriptor  | Descriptor           | Descriptor
    //            |-------------|----------------------|----------------------
    //            |             |                      |
    //            Previous      Current                Next
    //            Transition    Transition             Transition
    //..
    //
    // If 'localTime' does *not* fall between either T1 and T1' or T2 and T2',
    // then a single local-time descriptor uniquely applies to 'localTime',
    // and this method assigns to both '*firstResultTransition' and
    // '*secondResultTransition' an iterator referring to the transition for
    // that local-time descriptor, and assigns 'resultValidity'
    // 'e_VALID_UNIQUE'.
    //
    // If 'localTime' falls in either the range [T1, T1') or [T2 and T2'), then
    // either of the two adjacent local-time descriptors might apply to the
    // 'localTime'.  In that case, '*firstResultTransition' is assigned an
    // iterator referring to the earlier transition, and
    // '*secondResultTransition' is assigned an iterator referring to the
    // latter transition.  'resultValidity' is assigned 'e_VALID_AMBIGUOUS', if
    // clocks were set back (i.e., the UTC offset decreased at the transition),
    // and assigned 'e_INVALID' if clocks were set forward (i.e., the UTC
    // offset increased at the transition).  Note that we do not need to test
    // whether clocks remained the same at a transition, because in that
    // instance, the corresponding highlighted range -- e.g., [T1, T1') --
    // degenerates to an empty range, which therefore cannot include
    // 'localTime'.

    typedef LocalTimeValidity                 Validity;
    typedef Zoneinfo::TransitionConstIterator TransitionConstIter;

    bdlt::EpochUtil::TimeT64 localTimeT =
                                  Zoneinfo::convertToTimeT64(localTime);
    const bdlt::Datetime& utcTimeApproximation = localTime;

    bdlt::DatetimeTz     dummy;
    TransitionConstIter currentTransition;
    convertUtcToLocalTime(&dummy,
                          &currentTransition,
                          utcTimeApproximation,
                          timeZone);

    // The following assert is to ensure changes to the behavior of
    // 'utcToLocalTime' are caught.

    BSLS_ASSERT(currentTransition != timeZone.endTransitions());

    bdlt::EpochUtil::TimeT64 currentTimeT = currentTransition->utcTime();
    const int currentOffset =
                         currentTransition->descriptor().utcOffsetInSeconds();

    if (timeZone.beginTransitions() != currentTransition) {
        // Test whether 'localTime' falls either before T1, or in the range
        // between T1 and T1' (in the figure above).

        TransitionConstIter prevTransition = currentTransition;
        --prevTransition;

        const int prevOffset =
                             prevTransition->descriptor().utcOffsetInSeconds();

        bdlt::EpochUtil::TimeT64 T1      = currentTimeT +
                                       bsl::min(prevOffset, currentOffset);
        bdlt::EpochUtil::TimeT64 T1Prime = currentTimeT +
                                       bsl::max(prevOffset, currentOffset);
        if (localTimeT < T1) {
            *resultValidity         = Validity::e_VALID_UNIQUE;
            *firstResultTransition  = prevTransition;
            *secondResultTransition = prevTransition;
            return;                                                   // RETURN
        }
        else if (localTimeT < T1Prime) {
            // If 'prevOffset < currentOffset' then local time was adjusted
            // forwards at 'currentTransition', so intervening times between
            // T1 and T1' are invalid, otherwise local time was adjusted
            // backward, and intervening times are ambiguous.  Note that
            // 'currentOffset' cannot equal 'nextOffset', otherwise the
            // previous condition 'localTimeT < T1' would have been true.

            BSLS_ASSERT_SAFE(prevOffset != currentOffset);

            *resultValidity         = prevOffset < currentOffset
                                    ? Validity::e_INVALID
                                    : Validity::e_VALID_AMBIGUOUS;
            *firstResultTransition  = prevTransition;
            *secondResultTransition = currentTransition;
            return;                                                   // RETURN
        }
    }

    TransitionConstIter nextTransition = currentTransition;
    ++nextTransition;
    if (timeZone.endTransitions() != nextTransition) {
        // Test whether 'localTime' falls after T2', or in the range between
        // T2 and T2' (in the figure above).

        bdlt::EpochUtil::TimeT64 nextTimeT = nextTransition->utcTime();
        const int nextOffset =
                            nextTransition->descriptor().utcOffsetInSeconds();

        bdlt::EpochUtil::TimeT64 T2      = nextTimeT +
                                    bsl::min(currentOffset, nextOffset);
        bdlt::EpochUtil::TimeT64 T2Prime = nextTimeT +
                                    bsl::max(currentOffset, nextOffset);

        if (localTimeT >= T2Prime) {
            *resultValidity         = Validity::e_VALID_UNIQUE;
            *firstResultTransition  = nextTransition;
            *secondResultTransition = nextTransition;
            return;                                                   // RETURN
        }
        else if (localTimeT >= T2) {
            // If 'currentOffset < nextOffset' then local time was adjusted
            // forwards at 'currentTransition', so intervening times between
            // T1 and T1' are invalid, otherwise local time was adjusted
            // backward, and intervening times are ambiguous.  Note that
            // 'currentOffset' cannot equal 'nextOffset', otherwise the
            // previous condition 'localTimeT >= T2Prime' would have been true.

            BSLS_ASSERT_SAFE(currentOffset != nextOffset);
            *resultValidity         = currentOffset < nextOffset
                                    ? Validity::e_INVALID
                                    : Validity::e_VALID_AMBIGUOUS;
            *firstResultTransition  = currentTransition;
            *secondResultTransition = nextTransition;
            return;                                                   // RETURN
        }
    }

    // Since all other cases have been tested, 'localTime' must fall between
    // T1' and T2 (in the figure above).

    *resultValidity         = Validity::e_VALID_UNIQUE;
    *firstResultTransition  = currentTransition;
    *secondResultTransition = currentTransition;
}

bool baltzo::ZoneinfoUtil::isWellFormed(const Zoneinfo& timeZone)
{
    // This method tests the logical inverse of each constraint indicated in
    // the component documentation, and returns 'false' if any constraint is
    // not satisfied.

    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    // Constraint 1: 'timeZone.numTransitions() > 0'.

    if (timeZone.numTransitions() == 0) {
        BALL_LOG_WARN << "Time zone '" << timeZone.identifier() << "' "
                      << "constains no transitions." << BALL_LOG_END;
        return false;                                                 // RETURN
    }

    // Constraint 2: The first transition in 'timeZone' is at the first
    // representable 'bdlt::Datetime' value -- 'bdlt::Datetime(1, 1, 1)'.

    bdlt::Datetime firstDatetime(1, 1, 1);
    bdlt::EpochUtil::TimeT64 firstDatetimeT =
                            Zoneinfo::convertToTimeT64(firstDatetime);

    if (firstDatetimeT != timeZone.beginTransitions()->utcTime()) {
        BALL_LOG_WARN << "Time zone '" << timeZone.identifier() << "' does "
                      << "not contain an initial transition at 1/1/1."
                      << BALL_LOG_END;
        return false;                                                 // RETURN
    }

    // Constraint 3: There is no transition, in the ordered sequence of
    // transitions described by 'timeZone', where the local clock time is
    // adjusted, either forwards or backwards, introducing a period of invalid
    // or ambiguous local times, respectively, and where that range of local
    // times overlaps with the range of invalid or ambiguous local times
    // introduced by subsequent transition (if any).

    BSLS_ASSERT(0 < timeZone.numTransitions());
    Zoneinfo::TransitionConstIterator it = timeZone.beginTransitions();
    int previousOffset = it->descriptor().utcOffsetInSeconds();

    // Create a representation of local time immediately before and after the
    // previous transition ('prePreviousTransition' and
    // 'postPreviousTransition' respectively).

    bdlt::EpochUtil::TimeT64 prePreviousTransition  = it->utcTime() +
                                                                previousOffset;
    bdlt::EpochUtil::TimeT64 postPreviousTransition = prePreviousTransition;
    ++it;
    while (it != timeZone.endTransitions()) {
        int currentOffset = it->descriptor().utcOffsetInSeconds();
        bdlt::EpochUtil::TimeT64 utc = it->utcTime();

        // Create a representation of local time immediately before and after
        // the current transition ('preCurrentTransition' and
        // 'postCurrentTransition' respectively).

        bdlt::EpochUtil::TimeT64 preCurrentTransition  = utc + previousOffset;
        bdlt::EpochUtil::TimeT64 postCurrentTransition = utc + currentOffset;

        // Next, we test if the local time representation either immediately
        // before or after the previous transition, falls after the local time
        // immediately before or after the current transition.
        //
        // Note that we are guaranteed that 'postPreviousTransition' is an
        // earlier time representation than 'preCurrentTransition' since they
        // both have the same offset from UTC, and we are guaranteed that UTC
        // times for transitions are monotonically increasing.

        BSLS_ASSERT(!(postPreviousTransition >= preCurrentTransition));

        if (prePreviousTransition  >= preCurrentTransition  ||
            prePreviousTransition  >= postCurrentTransition ||
            postPreviousTransition >= postCurrentTransition) {
            BALL_LOG_WARN << "Time zone '" << timeZone.identifier() << "' "
                          << "contains transitions with overlapping ranges "
                          << "of invalid or ambiguous times."
                          << BALL_LOG_END;
            return false;                                             // RETURN
        }

        previousOffset         = currentOffset;
        prePreviousTransition  = preCurrentTransition;
        postPreviousTransition = postCurrentTransition;
        ++it;
    }
    return true;
}

}  // close enterprise namespace

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
