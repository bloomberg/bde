// baltzo_timezoneutilimp.cpp                                         -*-C++-*-
#include <baltzo_timezoneutilimp.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_timezoneutilimp_cpp,"$Id$ $CSID$")

#include <baltzo_defaultzoneinfocache.h>
#include <baltzo_errorcode.h>
#include <baltzo_localtimedescriptor.h>
#include <baltzo_localtimeperiod.h>
#include <baltzo_testloader.h>                // for testing
#include <baltzo_zoneinfo.h>
#include <baltzo_zoneinfocache.h>
#include <baltzo_zoneinfoutil.h>

#include <ball_log.h>

#include <bdlt_epochutil.h>

#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetimeinterval.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_ostream.h>

namespace BloombergLP {

static const char LOG_CATEGORY[] = "BALTZO.TIMEZONEUTILIMP";

// STATIC HELPER FUNCTIONS
static
int lookupTimeZone(const baltzo::Zoneinfo **timeZone,
                   const char              *timeZoneId,
                   baltzo::ZoneinfoCache   *cache)
    // Load, into the specified 'timeZone', the address of the time zone
    // information having the specified 'timeZoneId' from the specified
    // 'cache'.  Return 0 on success, and a non-zero value otherwise.  A return
    // status of 'baltzo::ErrorCode::k_UNSUPPORTED_ID' indicates that
    // 'timeZoneId' is not recognized.
{
    BSLS_ASSERT(timeZone);
    BSLS_ASSERT(timeZoneId);
    BSLS_ASSERT(cache);

    int rc = 0;
    *timeZone = cache->getZoneinfo(&rc, timeZoneId);
    BSLS_ASSERT((0 == rc && 0 != *timeZone) || (0 != rc && 0 == *timeZone));

    if (0 == *timeZone) {
        BALL_LOG_SET_CATEGORY(LOG_CATEGORY);
        BALL_LOG_INFO << "No data found for time zone '" << timeZoneId
                      << "' (rc = " << rc << ")." << BALL_LOG_END;
    }

    return rc;
}

static
baltzo::Zoneinfo::TransitionConstIterator findTransitionWithDstFlag(
                     const bool                                       dstFlag,
                     const baltzo::Zoneinfo::TransitionConstIterator& start,
                     const baltzo::Zoneinfo&                          timeZone)
    // Return an iterator referring to a transition in the specified
    // 'timeZone', having a local-time descriptor with the specified 'dstFlag'.
    // If existing, first examine the transition after the specified 'start',
    // then the two transitions preceding 'start' and eventually search from
    // the last transition of 'timeZone' backwards.  Return
    // 'timeZone.endTransition()' if 'timeZone' does not contain any transition
    // having a local-time descriptor with 'dstFlag'.  The behavior is
    // undefined unless 'start' is a valid iterator in 'timeZone'.
{
    BSLS_ASSERT(timeZone.endTransitions() != start);
    BSLS_ASSERT(timeZone.numTransitions() != 0);

    // Access one after.

    baltzo::Zoneinfo::TransitionConstIterator probeIterator = start;

    probeIterator++;
    if (timeZone.endTransitions() != probeIterator
        && probeIterator->descriptor().dstInEffectFlag() == dstFlag) {
        return probeIterator;                                         // RETURN
    }

   // Access the two previous ones.
    probeIterator = start;

    for (int i = 0; i < 2; ++i) {
        if (timeZone.beginTransitions() != probeIterator) {
            probeIterator--;
        }
        if (probeIterator->descriptor().dstInEffectFlag() == dstFlag) {
            return probeIterator;                                     // RETURN
        }
    }

    // Search from the end.

    baltzo::Zoneinfo::TransitionConstIterator backIterator =
                                                     timeZone.endTransitions();
    do {
        --backIterator;
        const baltzo::LocalTimeDescriptor& desc = backIterator->descriptor();
        if (dstFlag == desc.dstInEffectFlag()) {
            return backIterator;                                      // RETURN
        }
    } while (backIterator != timeZone.beginTransitions());

    return timeZone.endTransitions();
}


static
void selectUtcOffset(
               int                                              *utcOffsetSec,
               const baltzo::Zoneinfo::TransitionConstIterator&  iter1,
               const baltzo::Zoneinfo::TransitionConstIterator&  iter2,
               const baltzo::Zoneinfo&                           timeZone,
               bool                                              selectDstFlag)
{
    // Load into the specified 'utcOffsetSec', a UTC offset defined by a
    // local-time descriptor in the specified 'timeZone' whose daylight-saving
    // time (DST) property matches the one indicated by the specified
    // 'selectDstFlag', if it exists, showing preference to the local-time
    // descriptors associated with the two specified potential candidate
    // transitions referred to by 'iter1' and 'iter2'; load into
    // 'utcOffsetSec' the UTC offset indicated by the local-time descriptor
    // associated with the 'baltzo::Transition' object referred to by 'iter2'
    // if one matching 'selectDstFlag' is not found.  This operation will use
    // the local-time descriptor associated with the transition referred to by
    // 'iter1' if its DST property matches 'selectDstFlag', and, if 'iter1'
    // does not match, then the local-time descriptor of 'iter2' will be used,
    // if matching, otherwise the sequence of transitions will be searched for
    // a transition whose local-time descriptor has the correct DST property;
    // if no transition is present that holds a local-time descriptor matching
    // 'selectDstFlag', return 'iter2 .  The behavior is undefined unless
    // 'iter1' and 'iter2' are valid iterators into 'timeZone'.

    BSLS_ASSERT(utcOffsetSec);

    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);
    if (iter2 != iter1
        && selectDstFlag == iter2->descriptor().dstInEffectFlag()
        && selectDstFlag == iter1->descriptor().dstInEffectFlag()) {

            // If 'iter1' and 'iter2' are different transitions and both match
            // 'selectDstFlag', then there is an ambiguous selection.

            BALL_LOG_WARN << "The choice of a "
                          <<  (selectDstFlag ? "DST" : "STANDARD")
                          << " local-time is an ambiguous selection for "
                          << "local time types: "
                          << iter1->descriptor() << " and "
                          << iter2->descriptor()
                          << BALL_LOG_END;

        // The daylight-saving time property of 'iter2' matches the specified
        // 'dstPolicy'.

        *utcOffsetSec = iter2->descriptor().utcOffsetInSeconds();
        return;                                                       // RETURN
    }

    if (selectDstFlag == iter1->descriptor().dstInEffectFlag()) {
        *utcOffsetSec = iter1->descriptor().utcOffsetInSeconds();
        return;                                                       // RETURN
    }

    if (selectDstFlag == iter2->descriptor().dstInEffectFlag()) {
        // The daylight-saving time property of 'iter2' matches the specified
        // 'dstPolicy'.

        *utcOffsetSec = iter2->descriptor().utcOffsetInSeconds();
        return;                                                       // RETURN
    }

    // 'iter2' is by construction later than 'iter1' if they are different.

    baltzo::Zoneinfo::TransitionConstIterator candidateTransition =
                     findTransitionWithDstFlag(selectDstFlag, iter2, timeZone);

    if (timeZone.endTransitions() != candidateTransition) {
        *utcOffsetSec = candidateTransition->descriptor().utcOffsetInSeconds();
        return;                                                       // RETURN
    }

    // The supplied 'dstPolicy' makes no sense, select the latter of the two
    // possible values.

    BALL_LOG_WARN << "The choice of a " << (selectDstFlag ? "STANDARD" : "DST")
                  << " local-time does not match any time type "
                  << "in the provided time zone" << timeZone
                  << BALL_LOG_END;

    *utcOffsetSec = iter2->descriptor().utcOffsetInSeconds();
}

                           // ---------------------
                           // class TimeZoneUtilImp
                           // ---------------------

// CLASS METHODS
int baltzo::TimeZoneUtilImp::convertUtcToLocalTime(
                                       bdlt::DatetimeTz      *result,
                                       const char            *resultTimeZoneId,
                                       const bdlt::Datetime&  utcTime,
                                       ZoneinfoCache         *cache)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(resultTimeZoneId);
    BSLS_ASSERT(cache);

    const Zoneinfo *timeZone;
    const int rc = lookupTimeZone(&timeZone, resultTimeZoneId, cache);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    Zoneinfo::TransitionConstIterator it;
    ZoneinfoUtil::convertUtcToLocalTime(result,
                                               &it,
                                               utcTime,
                                               *timeZone);
    return 0;
}

int baltzo::TimeZoneUtilImp::initLocalTime(
                                       bdlt::DatetimeTz        *result,
                                       LocalTimeValidity::Enum *resultValidity,
                                       const bdlt::Datetime&    localTime,
                                       const char              *timeZoneId,
                                       DstPolicy::Enum          dstPolicy,
                                       ZoneinfoCache           *cache)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(resultValidity);
    BSLS_ASSERT(timeZoneId);
    BSLS_ASSERT(cache);

    const Zoneinfo *timeZone;
    const int rc = lookupTimeZone(&timeZone, timeZoneId, cache);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    Zoneinfo::TransitionConstIterator iter;
    resolveLocalTime(result,
                     resultValidity,
                     &iter,
                     localTime,
                     dstPolicy,
                     *timeZone);
    return 0;
}

int baltzo::TimeZoneUtilImp::loadLocalTimePeriodForUtc(
                                             LocalTimePeriod       *result,
                                             const char            *timeZoneId,
                                             const bdlt::Datetime&  utcTime,
                                             ZoneinfoCache         *cache)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timeZoneId);
    BSLS_ASSERT(cache);

    const Zoneinfo *timeZone;
    int rc = lookupTimeZone(&timeZone, timeZoneId, cache);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    bdlt::DatetimeTz                          localTime;

    Zoneinfo::TransitionConstIterator iter =
                                   timeZone->findTransitionForUtcTime(utcTime);

    createLocalTimePeriod(result, iter, *timeZone);
    return 0;
}

void baltzo::TimeZoneUtilImp::resolveLocalTime(
                             bdlt::DatetimeTz                  *result,
                             LocalTimeValidity::Enum           *resultValidity,
                             Zoneinfo::TransitionConstIterator *transitionIter,
                             const bdlt::Datetime&              localTime,
                             DstPolicy::Enum                    dstPolicy,
                             const Zoneinfo&                    timeZone)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(resultValidity);
    BSLS_ASSERT(transitionIter);
    BSLS_ASSERT_SAFE(ZoneinfoUtil::isWellFormed(timeZone));

    BALL_LOG_SET_CATEGORY(LOG_CATEGORY);

    typedef LocalTimeValidity Validity;

    // First, determine the transitions that could conceivably apply to
    // 'localTime', and determine whether 'localTime' is valid and unique,
    // valid but ambiguous, or invalid.

    Zoneinfo::TransitionConstIterator iter1, iter2;
    ZoneinfoUtil::loadRelevantTransitions(&iter1,
                                                 &iter2,
                                                 resultValidity,
                                                 localTime,
                                                 timeZone);

    const int utcOffset1 = iter1->descriptor().utcOffsetInSeconds();
    const int utcOffset2 = iter2->descriptor().utcOffsetInSeconds();

    // Next resolve the UTC offset for 'localTime' based on 'dstPolicy' and
    // the relevant transitions.

    int utcOffsetInSeconds;
    if (dstPolicy != DstPolicy::e_UNSPECIFIED) {
        // If 'dstPolicy' is DST or STANDARD, select the UTC offset from a
        // local time descriptor with a matching daylight-saving time
        // property.

        const bool isDstOff = dstPolicy == DstPolicy::e_DST;
        selectUtcOffset(&utcOffsetInSeconds, iter1, iter2, timeZone, isDstOff);
    }
    else {
        // 'dstPolicy' is UNSPECIFIED.  Select the UTC offset from the later
        // relevant transition if the local time is ambiguous or the earlier if
        // invalid.  Note that for valid and unique local times, the returned
        // iterators are equal so this choice is irrelevant.

        BSLS_ASSERT(*resultValidity != Validity::e_VALID_UNIQUE
                 || utcOffset1 == utcOffset2);

         utcOffsetInSeconds = *resultValidity == Validity::e_INVALID
                            ? utcOffset1 : utcOffset2;
    }

    // Use the resolved UTC offset to create the resolved UTC value for
    // 'localTime'

    const int utcOffsetInMinutes = utcOffsetInSeconds / 60;
    bdlt::Datetime resolvedUtcTime = localTime;
    resolvedUtcTime.addMinutes(-utcOffsetInMinutes);

    // Assign 'transitionIter' to the transition, from the two relevant
    // transitions determined earlier, describing the properties of local time
    // at 'resolvedUtcTime'.

    const bdlt::EpochUtil::TimeT64 resolvedUtcTimeT =
                            Zoneinfo::convertToTimeT64(resolvedUtcTime);
    *transitionIter = resolvedUtcTimeT < iter2->utcTime()
                    ? iter1
                    : iter2;

    // Finally, set 'result' to the local time in the indicated time zone
    // corresponding to 'resolvedUtcTime'.  Note that the corresponding local
    // time value for 'resolvedUtcTime' may be different than 'localTime'.

    const int resultOffsetInMinutes =
                     (*transitionIter)->descriptor().utcOffsetInSeconds() / 60;

    BALL_LOG_TRACE << "[ Input    = "      << localTime
                   << "  Validity = "      << *resultValidity
                   << "  DstPolicy = "     <<  dstPolicy
                   << "  AppliedOffset = " << resultOffsetInMinutes  << " ]"
                   << BALL_LOG_END;

    bdlt::Datetime resultTime = resolvedUtcTime;
    resultTime.addMinutes(resultOffsetInMinutes);
    result->setDatetimeTz(resultTime, resultOffsetInMinutes);
}

void baltzo::TimeZoneUtilImp::createLocalTimePeriod(
                          LocalTimePeriod                          *result,
                          const Zoneinfo::TransitionConstIterator&  transition,
                          const Zoneinfo&                           timeZone)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(transition != timeZone.endTransitions());
    BSLS_ASSERT_SAFE(ZoneinfoUtil::isWellFormed(timeZone));

    result->setDescriptor(transition->descriptor());

    // The transition times in 'timeZone' are guaranteed to be in the range
    // representable by 'bdlt::Datetime'.

    bdlt::Datetime utcStartTime(1, 1, 1, 0, 0, 0);
    Zoneinfo::convertFromTimeT64(
                              &utcStartTime, transition->utcTime());

    Zoneinfo::TransitionConstIterator next = transition;
    ++next;

    // 'utcEndTime' must account for the special case that the time falls
    // after the last transition.

    bdlt::Datetime nextUtcTime;
    Zoneinfo::convertFromTimeT64(&nextUtcTime, next->utcTime());

    bdlt::Datetime utcEndTime = (next == timeZone.endTransitions())
       ? bdlt::Datetime(9999, 12, 31, 23, 59, 59, 999)
       : nextUtcTime;

    result->setDescriptor(transition->descriptor());
    result->setUtcStartAndEndTime(utcStartTime, utcEndTime);
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
