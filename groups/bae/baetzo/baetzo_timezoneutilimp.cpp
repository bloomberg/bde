// baetzo_timezoneutilimp.cpp                                         -*-C++-*-
#include <baetzo_timezoneutilimp.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_timezoneutilimp_cpp,"$Id$ $CSID$")

#include <baetzo_defaultzoneinfocache.h>
#include <baetzo_errorcode.h>
#include <baetzo_localtimedescriptor.h>
#include <baetzo_localtimeperiod.h>
#include <baetzo_testloader.h>                // for testing
#include <baetzo_zoneinfo.h>
#include <baetzo_zoneinfocache.h>
#include <baetzo_zoneinfoutil.h>

#include <bael_log.h>

#include <bdetu_epoch.h>

#include <bdet_datetime.h>
#include <bdet_datetimetz.h>
#include <bdet_datetimeinterval.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_types.h>

namespace BloombergLP {

static const char LOG_CATEGORY[] = "BAETZO.TIMEZONEUTILIMP";

// STATIC HELPER FUNCTIONS
static
int lookupTimeZone(const baetzo_Zoneinfo **timeZone,
                   const char             *timeZoneId,
                   baetzo_ZoneinfoCache   *cache)
    // Load, into the specified 'timeZone', the address of the time zone
    // information having the specified 'timeZoneId' from the specified
    // 'cache'.  Return 0 on success, and a non-zero value otherwise.  A return
    // status of 'baetzo_ErrorCode::BAETZO_UNSUPPORTED_ID' indicates that
    // 'timeZoneId' is not recognized.
{
    BSLS_ASSERT(timeZone);
    BSLS_ASSERT(timeZoneId);
    BSLS_ASSERT(cache);

    int rc = 0;
    *timeZone = cache->getZoneinfo(&rc, timeZoneId);
    BSLS_ASSERT((0 == rc && 0 != *timeZone) || (0 != rc && 0 == *timeZone));

    if (0 == *timeZone) {
        BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
        BAEL_LOG_INFO << "No data found for time zone '" << timeZoneId
                      << "' (rc = " << rc << ")." << BAEL_LOG_END;
    }

    return rc;
}

static
baetzo_Zoneinfo::TransitionConstIterator findTransitionWithDstFlag(
                      const bool                                      dstFlag,
                      const baetzo_Zoneinfo::TransitionConstIterator& start,
                      const baetzo_Zoneinfo&                          timeZone)
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

    baetzo_Zoneinfo::TransitionConstIterator probeIterator = start;

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

    baetzo_Zoneinfo::TransitionConstIterator backIterator =
                                                     timeZone.endTransitions();
    do {
        --backIterator;
        const baetzo_LocalTimeDescriptor& desc = backIterator->descriptor();
        if (dstFlag == desc.dstInEffectFlag()) {
            return backIterator;                                      // RETURN
        }
    } while (backIterator != timeZone.beginTransitions());

    return timeZone.endTransitions();
}


static
void selectUtcOffset(
                int                                             *utcOffsetSec,
                const baetzo_Zoneinfo::TransitionConstIterator&  iter1,
                const baetzo_Zoneinfo::TransitionConstIterator&  iter2,
                const baetzo_Zoneinfo&                           timeZone,
                bool                                             selectDstFlag)
{
    // Load into the specified 'utcOffsetSec', a UTC offset defined by a
    // local-time descriptor in the specified 'timeZone' whose daylight-saving
    // time (DST) property matches the one indicated by the specified
    // 'selectDstFlag', if it exists, showing preference to the local-time
    // descriptors associated with the two specified potential candidate
    // transitions referred to by 'iter1' and 'iter2'; load into
    // 'utcOffsetSec' the UTC offset indicated by the local-time descriptor
    // associated with the 'baetzo_Transition' object referred to by 'iter2'
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

    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    if (iter2 != iter1
        && selectDstFlag == iter2->descriptor().dstInEffectFlag()
        && selectDstFlag == iter1->descriptor().dstInEffectFlag()) {

            // If 'iter1' and 'iter2' are different transitions and both match
            // 'selectDstFlag', then there is an ambiguous selection.

            BAEL_LOG_WARN << "The choice of a "
                          <<  (selectDstFlag ? "DST" : "STANDARD")
                          << " local-time is an ambiguous selection for "
                          << "local time types: "
                          << iter1->descriptor() << " and "
                          << iter2->descriptor()
                          << BAEL_LOG_END;

        // The daylight-savings time property of 'iter2' matches the specified
        // 'dstPolicy'.

        *utcOffsetSec = iter2->descriptor().utcOffsetInSeconds();
        return;                                                       // RETURN
    }

    if (selectDstFlag == iter1->descriptor().dstInEffectFlag()) {
        *utcOffsetSec = iter1->descriptor().utcOffsetInSeconds();
        return;                                                       // RETURN
    }

    if (selectDstFlag == iter2->descriptor().dstInEffectFlag()) {
        // The daylight-savings time property of 'iter2' matches the specified
        // 'dstPolicy'.

        *utcOffsetSec = iter2->descriptor().utcOffsetInSeconds();
        return;                                                       // RETURN
    }


    typedef baetzo_LocalTimeDescriptor Descriptor;


    // 'iter2' is by construction later than 'iter1' if they are different.

    baetzo_Zoneinfo::TransitionConstIterator candidateTransition =
                     findTransitionWithDstFlag(selectDstFlag, iter2, timeZone);

    if (timeZone.endTransitions() != candidateTransition) {
        *utcOffsetSec = candidateTransition->descriptor().utcOffsetInSeconds();
        return;                                                       // RETURN
    }

    // The supplied 'dstPolicy' makes no sense, select the latter of the two
    // possible values.

    BAEL_LOG_WARN << "The choice of a " << (selectDstFlag ? "STANDARD" : "DST")
                  << " local-time does not match any time type "
                  << "in the provided time zone" << timeZone
                  << BAEL_LOG_END;

    *utcOffsetSec = iter2->descriptor().utcOffsetInSeconds();
}

                        // ----------------------------
                        // class baetzo_TimeZoneUtilImp
                        // ----------------------------

// CLASS METHODS
int baetzo_TimeZoneUtilImp::convertUtcToLocalTime(
                                        bdet_DatetimeTz      *result,
                                        const char           *resultTimeZoneId,
                                        const bdet_Datetime&  utcTime,
                                        baetzo_ZoneinfoCache *cache)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(resultTimeZoneId);
    BSLS_ASSERT(cache);

    const baetzo_Zoneinfo *timeZone;
    const int rc = lookupTimeZone(&timeZone, resultTimeZoneId, cache);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    baetzo_Zoneinfo::TransitionConstIterator it;
    baetzo_ZoneinfoUtil::convertUtcToLocalTime(result,
                                               &it,
                                               utcTime,
                                               *timeZone);
    return 0;
}

int baetzo_TimeZoneUtilImp::initLocalTime(
                                bdet_DatetimeTz                *result,
                                baetzo_LocalTimeValidity::Enum *resultValidity,
                                const bdet_Datetime&            localTime,
                                const char                     *timeZoneId,
                                baetzo_DstPolicy::Enum          dstPolicy,
                                baetzo_ZoneinfoCache           *cache)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(resultValidity);
    BSLS_ASSERT(timeZoneId);
    BSLS_ASSERT(cache);

    const baetzo_Zoneinfo *timeZone;
    const int rc = lookupTimeZone(&timeZone, timeZoneId, cache);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    baetzo_Zoneinfo::TransitionConstIterator iter;
    resolveLocalTime(result,
                     resultValidity,
                     &iter,
                     localTime,
                     dstPolicy,
                     *timeZone);
    return 0;
}

int baetzo_TimeZoneUtilImp::loadLocalTimePeriodForUtc(
                                          baetzo_LocalTimePeriod *result,
                                          const char             *timeZoneId,
                                          const bdet_Datetime&    utcTime,
                                          baetzo_ZoneinfoCache   *cache)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(timeZoneId);
    BSLS_ASSERT(cache);

    const baetzo_Zoneinfo *timeZone;
    int rc = lookupTimeZone(&timeZone, timeZoneId, cache);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    bdet_DatetimeTz                          localTime;

    baetzo_Zoneinfo::TransitionConstIterator iter =
                                   timeZone->findTransitionForUtcTime(utcTime);

    createLocalTimePeriod(result, iter, *timeZone);
    return 0;
}

void baetzo_TimeZoneUtilImp::resolveLocalTime(
                     bdet_DatetimeTz                           *result,
                     baetzo_LocalTimeValidity::Enum            *resultValidity,
                     baetzo_Zoneinfo::TransitionConstIterator  *transitionIter,
                     const bdet_Datetime&                       localTime,
                     baetzo_DstPolicy::Enum                     dstPolicy,
                     const baetzo_Zoneinfo&                     timeZone)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(resultValidity);
    BSLS_ASSERT(transitionIter);
    BSLS_ASSERT_SAFE(baetzo_ZoneinfoUtil::isWellFormed(timeZone));

    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    typedef baetzo_LocalTimeValidity Validity;

    // First, determine the transitions that could conceivably apply to
    // 'localTime', and determine whether 'localTime' is valid and unique,
    // valid but ambiguous, or invalid.

    baetzo_Zoneinfo::TransitionConstIterator iter1, iter2;
    baetzo_ZoneinfoUtil::loadRelevantTransitions(&iter1,
                                                 &iter2,
                                                 resultValidity,
                                                 localTime,
                                                 timeZone);

    const int utcOffset1 = iter1->descriptor().utcOffsetInSeconds();
    const int utcOffset2 = iter2->descriptor().utcOffsetInSeconds();

    // Next resolve the UTC offset for 'localTime' based on 'dstPolicy' and
    // the relevant transitions.

    int utcOffsetInSeconds;
    if (dstPolicy != baetzo_DstPolicy::BAETZO_UNSPECIFIED) {
        // If 'dstPolicy' is DST or STANDARD, select the UTC offset from a
        // local time descriptor with a matching daylight-savings time
        // property.

        const bool isDstOff = dstPolicy == baetzo_DstPolicy::BAETZO_DST;
        selectUtcOffset(&utcOffsetInSeconds, iter1, iter2, timeZone, isDstOff);
    }
    else {
        // 'dstPolicy' is UNSPECIFIED.  Select the UTC offset from the
        // later relevant transition if the local time is ambiguous or the
        // earlier if invalid.  Note that for valid and unique local times, the
        // returned iterators are equal so this choice is irrelevant.

        BSLS_ASSERT(*resultValidity != Validity::BAETZO_VALID_UNIQUE
                 || utcOffset1 == utcOffset2);

         utcOffsetInSeconds = *resultValidity == Validity::BAETZO_INVALID
                            ? utcOffset1 : utcOffset2;
    }

    // Use the resolved UTC offset to create the resolved UTC value for
    // 'localTime'

    const int utcOffsetInMinutes = utcOffsetInSeconds / 60;
    bdet_Datetime resolvedUtcTime = localTime;
    resolvedUtcTime.addMinutes(-utcOffsetInMinutes);

    // Assign 'transitionIter' to the transition, from the two relevant
    // transitions determined earlier, describing the properties of local time
    // at 'resolvedUtcTime'.

    const bdetu_Epoch::TimeT64 resolvedUtcTimeT =
                                bdetu_Epoch::convertToTimeT64(resolvedUtcTime);
    *transitionIter = resolvedUtcTimeT < iter2->utcTime()
                    ? iter1
                    : iter2;

    // Finally, set 'result' to the local time in the indicated time zone
    // corresponding to 'resolvedUtcTime'.  Note that the corresponding local
    // time value for 'resolvedUtcTime' may be different than 'localTime'.

    const int resultOffsetInMinutes =
                     (*transitionIter)->descriptor().utcOffsetInSeconds() / 60;

    BAEL_LOG_TRACE << "[ Input    = "      << localTime
                   << "  Validity = "      << *resultValidity
                   << "  DstPolicy = "     <<  dstPolicy
                   << "  AppliedOffset = " << resultOffsetInMinutes  << " ]"
                   << BAEL_LOG_END;

    bdet_Datetime resultTime = resolvedUtcTime;
    resultTime.addMinutes(resultOffsetInMinutes);
    result->setDatetimeTz(resultTime, resultOffsetInMinutes);
}

void baetzo_TimeZoneUtilImp::createLocalTimePeriod(
               baetzo_LocalTimePeriod                          *result,
               const baetzo_Zoneinfo::TransitionConstIterator&  transition,
               const baetzo_Zoneinfo&                           timeZone)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(transition != timeZone.endTransitions());
    BSLS_ASSERT_SAFE(baetzo_ZoneinfoUtil::isWellFormed(timeZone));

    result->setDescriptor(transition->descriptor());

    // The transition times in 'timeZone' are guaranteed to be in the range
    // representable by 'bdet_Datetime'.

    bdet_Datetime utcStartTime(1, 1, 1, 0, 0, 0);
    bdetu_Epoch::convertFromTimeT64(
                              &utcStartTime, transition->utcTime());

    baetzo_Zoneinfo::TransitionConstIterator next = transition;
    ++next;

    // 'utcEndTime' must account for the special case that the time falls
    // after the last transition.

    bdet_Datetime utcEndTime = (next == timeZone.endTransitions())
       ? bdet_Datetime(9999, 12, 31, 23, 59, 59, 999)
       : bdetu_Epoch::convertFromTimeT64(next->utcTime());

    result->setDescriptor(transition->descriptor());
    result->setUtcStartAndEndTime(utcStartTime, utcEndTime);
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
