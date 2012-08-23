// btes_leakybucket.cpp                                               -*-C++-*-
#include <btes_leakybucket.h>

#include <bdes_bitutil.h>

#include <bsl_algorithm.h>
#include <bsl_climits.h>
#include <bsl_c_math.h>

namespace BloombergLP {

static bsls_Types::Uint64 calculateNumberOfUnitsToDrain(
                                  bsls_Types::Uint64*      nanounitsPtr,
                                  bsls_Types::Uint64       drainRate,
                                  const bdet_TimeInterval& timeInterval)

    // Calculate the number of units that would be drained at the specified
    // 'drainRate' during the specified 'timeInterval', taking fraction of a
    // unit, passed via the specified 'nanounitsPtr' into account and loading
    // the fractional part of calculated number of units into it.
    // The fractional part represented by the number of nanounits. The behavior
    // is undefined unless the whole part of calculated number of units may be
    // represented by 64-bit unsigned integral type.

{
    BSLS_ASSERT((bsls_Types::Uint64)timeInterval.seconds() <= 
                ULLONG_MAX/drainRate);

    bsls_Types::Uint64 G = 1000000000;

    bsls_Types::Uint64 units  = drainRate * timeInterval.seconds();
    units += (drainRate / G) * timeInterval.nanoseconds();

    bsls_Types::Uint64 nanounits = 0;

    // As long as rate is represented by a whole number, the fractional part
    // of number of units to drain comes from fractional part of seconds of
    // the time interval

    if (0 != nanounitsPtr) {

        nanounits = *nanounitsPtr + 
                    (drainRate % G) * timeInterval.nanoseconds();

        *nanounitsPtr = nanounits % G;
    }
    else {

        nanounits = (drainRate % G) * timeInterval.nanoseconds();
    }

    units += nanounits / G;

    return units;
}

                        //-----------------------
                        // class btes_LeakyBucket
                        //-----------------------

// CLASS METHODS

bdet_TimeInterval btes_LeakyBucket::calculateDrainTime(
                                            bsls_Types::Uint64 numOfUnits,
                                            bsls_Types::Uint64 drainRate,
                                            bool ceilFlag)
{
    BSLS_ASSERT_SAFE(drainRate > 0);
    BSLS_ASSERT_SAFE(drainRate > 1 || numOfUnits <= LLONG_MAX);

    const bsls_Types::Uint64 G  = 1000000000;

    bdet_TimeInterval interval(0,0);

    interval.addSeconds(numOfUnits / drainRate);
    bsls_Types::Uint64 remUnits = numOfUnits % drainRate;

    if(true == ceilFlag) {
        interval.addNanoseconds(
                           unsigned(ceil((double(remUnits) * G) / drainRate)));
    }
    else {
        interval.addNanoseconds(
                          unsigned(floor((double(remUnits) * G) / drainRate)));
    }

    return interval;
}

bdet_TimeInterval btes_LeakyBucket::calculateTimeWindow(
                                                 bsls_Types::Uint64 drainRate,
                                                 bsls_Types::Uint64 capacity)
{
    BSLS_ASSERT_SAFE(drainRate > 0);
    BSLS_ASSERT_SAFE(capacity  > 0);
    BSLS_ASSERT_SAFE(drainRate > 1 || capacity <= LLONG_MAX);

    bdet_TimeInterval window = btes_LeakyBucket::calculateDrainTime(capacity,
                                                                    drainRate,
                                                                    true);

    if (0 == window) {
        window.addNanoseconds(1);
    }

    return window;
}


bsls_Types::Uint64 btes_LeakyBucket::calculateCapacity(
                                                 bsls_Types::Uint64 drainRate,
                                           const bdet_TimeInterval& timeWindow)
{
    BSLS_ASSERT_SAFE(drainRate  >  0);
    BSLS_ASSERT_SAFE(timeWindow >  0);

    BSLS_ASSERT_SAFE (1 == drainRate || 
                      timeWindow <= btes_LeakyBucket::calculateDrainTime(
                                                                   ULLONG_MAX,
                                                                   drainRate,
                                                                   false));

    bsls_Types::Uint64 capacity = calculateNumberOfUnitsToDrain(0,
                                                                drainRate,
                                                                timeWindow);

    // Round capacity to 1. It is OK, because it doesn`t affect drain rate.

    return (0 != capacity) ? capacity : 1;
}

// CREATORS

btes_LeakyBucket::btes_LeakyBucket( bsls_Types::Uint64       drainRate,
                                    bsls_Types::Uint64       capacity,
                                    const bdet_TimeInterval& currentTime)
: d_drainRate(drainRate)
, d_capacity(capacity)
, d_unitsReserved(0)
, d_unitsInBucket(0)
, d_nanoUnitsCarry(0)
, d_timestamp(currentTime)
, d_submittedUnits(0)
, d_submittedUnitsAtLastUpdate(0)
, d_statisticsTimestamp(currentTime)
{
    BSLS_ASSERT_SAFE(drainRate > 0);
    BSLS_ASSERT_SAFE(capacity  > 0);

    // Calculate the maximum interval between updates that would not cause
    // overflow of 'bdet_TimeInterval'

    if (drainRate > 1) {
        d_maxUpdateInterval = btes_LeakyBucket::calculateDrainTime(ULLONG_MAX,
                                                                   drainRate,
                                                                   false);
    }
    else{
        d_maxUpdateInterval = bdet_TimeInterval(LLONG_MAX, 999999999);
    }
}

// MANIPULATORS

void btes_LeakyBucket::setRateAndCapacity(bsls_Types::Uint64 newRate,
                                          bsls_Types::Uint64 newCapacity)
{
    BSLS_ASSERT_SAFE(newRate     > 0);
    BSLS_ASSERT_SAFE(newCapacity > 0);

    d_drainRate  = newRate;
    d_capacity   = newCapacity;

    // Calculate the maximum interval between updates that would not cause
    // overflow of 'bdet_TimeInterval'

    if (newRate > 1) {
        d_maxUpdateInterval = btes_LeakyBucket::calculateDrainTime(ULLONG_MAX,
                                                                   newRate,
                                                                   false);
    }
    else{
        d_maxUpdateInterval = bdet_TimeInterval(LLONG_MAX, 999999999);
    }
}

void btes_LeakyBucket::updateState(const bdet_TimeInterval& currentTime)
{

    bdet_TimeInterval delta = currentTime - d_timestamp;
    d_submittedUnitsAtLastUpdate = d_submittedUnits;

    // If delta is greater than the time it takes to drain maximum number of
    // units, representable by 64 bit integral type, set 'unitsInBucket'
    // to zero.

    if (delta > d_maxUpdateInterval) {

        d_timestamp      = currentTime;
        d_unitsInBucket  = 0;
        d_nanoUnitsCarry = 0;

        return;                                                       // RETURN
    }

    if ((delta.seconds() >= 0) && (delta.nanoseconds() >= 0)) {

        bsls_Types::Uint64 units;

        units = calculateNumberOfUnitsToDrain(&d_nanoUnitsCarry,
                                              d_drainRate,
                                              delta);

        if (units < d_unitsInBucket) {
            d_unitsInBucket -= units;
        }
        else {
            d_unitsInBucket = 0;
        }
    }
    else {

        // The time is going backwards (delta < 0). This may happen when clocks
        // are updated.
        // If the specified time precedes 'statisticsTimestamp', adjust
        // statistics timestamp to prevent the statistics collection interval
        // from going negative.

        d_statisticsTimestamp = currentTime;
    }

    d_timestamp                  = currentTime;
}

bool btes_LeakyBucket::wouldOverflow(bsls_Types::Uint64       numOfUnits,
                                     const bdet_TimeInterval& currentTime)
{

    BSLS_ASSERT_SAFE(numOfUnits > 0);

    if (numOfUnits > ULLONG_MAX - d_unitsInBucket - d_unitsReserved) {

        updateState(currentTime);
        
        // 'd_unitsInBucket + d_unitsReserved + numOfUnits' will not be 
        // calculated if the first part of condition evaluates to 'true'.

                                                                      // RETURN

        return (numOfUnits > ULLONG_MAX - d_unitsInBucket - d_unitsReserved ||
                d_unitsInBucket + d_unitsReserved + numOfUnits > d_capacity);
    }

    bsls_Types::Uint64 totalUnits = d_unitsInBucket +
                                    d_unitsReserved +
                                    numOfUnits;

    // If there is already room in the bucket, return false immediately
    // (avoid updating state to improve performance).

    if (totalUnits <= d_capacity) {
        return false;                                                 // RETURN
    }

    // the number of 'unitsInBucket' or 'submittedUnits' can not be increase
    // by 'updateState'. Overflow check is not needed.

    updateState(currentTime);

    totalUnits = d_unitsInBucket + d_unitsReserved + numOfUnits;

    return (totalUnits > d_capacity);
}

bdet_TimeInterval btes_LeakyBucket::calculateTimeToSubmit(
                                          const bdet_TimeInterval& currentTime)
{
    bsls_Types::Uint64 usedUnits = d_unitsInBucket + d_unitsReserved;

    // Return zero time interval immediately if units can be submitted right
    // now.

    if (usedUnits < d_capacity) {
        return bdet_TimeInterval(0,0);                                // RETURN
    }

    updateState(currentTime);

    // Return zero time interval if units can be submitted after updating
    // state

    if (d_unitsInBucket + d_unitsReserved < d_capacity) {
        return bdet_TimeInterval(0,0);                                // RETURN
    }

    bdet_TimeInterval timeToSubmit(0,0);
    bsls_Types::Uint64 backlogUnits;

    // Here 'd_unitsInBucket + d_unitsReserved' is always greater than
    // 'd_capacity'

    backlogUnits = d_unitsInBucket + d_unitsReserved - d_capacity + 1;

    timeToSubmit = btes_LeakyBucket::calculateDrainTime(backlogUnits, 
                                                        d_drainRate,
                                                        true);

    // Assuming 1 nanosecond as 'timeToSubmit' counting resolution
    // if time interval was rounded to zero(in case of high rates) -
    // return 1 nanosecond

    if (timeToSubmit == 0) {
        timeToSubmit.addNanoseconds(1);
    }

    return timeToSubmit;
}

// ACCESSORS

void btes_LeakyBucket::getStatistics(bsls_Types::Uint64* submittedUnits,
                                     bsls_Types::Uint64* unusedUnits) const
{

    BSLS_ASSERT(0 != submittedUnits);
    BSLS_ASSERT(0 != unusedUnits);

    *submittedUnits = d_submittedUnitsAtLastUpdate;

    // The 'monitoredInterval' can not be negative, as 'updateState' method
    // checks, whether specified time precedes 'statisticsTimestamp' and
    // adjusts 'statisticsTimestamp' if required.

    bdet_TimeInterval monitoredInterval = d_timestamp - d_statisticsTimestamp;

    bsls_Types::Uint64 drainedUnits = calculateNumberOfUnitsToDrain(
                                             0,
                                             d_drainRate,
                                             monitoredInterval);

    if (drainedUnits < d_submittedUnitsAtLastUpdate) {
        *unusedUnits = 0;
    }
    else {
        *unusedUnits = drainedUnits - d_submittedUnitsAtLastUpdate;
    }
}

}// closed enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
