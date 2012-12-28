// btes_leakybucket.cpp                                               -*-C++-*-
#include <btes_leakybucket.h>

#include <bsl_climits.h>
#include <bsl_c_math.h>

namespace BloombergLP {

namespace {

bsls_Types::Uint64 calculateNumberOfUnitsToDrain(
                     bsls_Types::Uint64*      fractionalUnitDrainedInNanoUnits,
                     bsls_Types::Uint64       drainRate,
                     const bdet_TimeInterval& timeInterval)
    // Return the integral sum of the number of units that would be drained at
    // the specified 'drainRate' during the specified 'timeInterval' plus the
    // specified 'fractionalUnitDrainedInNanoUnits', representing a fractional
    // unit (between 0.0 and 1.0).  Furthermore, load, into the specified
    // 'fractionalUnitDrainedInNanoUnits', the fractional unit (between 0.0 and
    // 1.0) left over from that summation.  The behavior is undefined unless
    // '0 <= fractionalUnitDrainedInNanoUnits < 1000000000'.  Note that
    // 'fractionalUnitDrainedInNanoUnits' is represented in nano-units to avoid
    // using a floating point representation.

{
    const bsls_Types::Uint64 NANOUNITS_PER_UNIT = 1000000000;

    BSLS_ASSERT((bsls_Types::Uint64)timeInterval.seconds() <=
                ULLONG_MAX/drainRate);
    BSLS_ASSERT(0 != fractionalUnitDrainedInNanoUnits);
    BSLS_ASSERT(*fractionalUnitDrainedInNanoUnits < NANOUNITS_PER_UNIT);

    bsls_Types::Uint64 units  = drainRate * timeInterval.seconds();
    units += (drainRate / NANOUNITS_PER_UNIT) * timeInterval.nanoseconds();

    bsls_Types::Uint64 nanounits = 0;

    // As long as rate is represented by a whole number, the fractional part
    // of number of units to drain comes from fractional part of seconds of
    // the time interval

    nanounits = *fractionalUnitDrainedInNanoUnits +
        (drainRate % NANOUNITS_PER_UNIT) * timeInterval.nanoseconds();

    *fractionalUnitDrainedInNanoUnits = nanounits % NANOUNITS_PER_UNIT;

    units += nanounits / NANOUNITS_PER_UNIT;

    return units;
}

}  // close unnamed namespace

                        //-----------------------
                        // class btes_LeakyBucket
                        //-----------------------

// CLASS METHODS
bdet_TimeInterval btes_LeakyBucket::calculateDrainTime(
                                                  bsls_Types::Uint64 numUnits,
                                                  bsls_Types::Uint64 drainRate,
                                                  bool               ceilFlag)
{
    BSLS_ASSERT_SAFE(drainRate > 0);
    BSLS_ASSERT_SAFE(drainRate > 1 || numUnits <= LLONG_MAX);

    const bsls_Types::Uint64 NANOUNITS_PER_UNIT  = 1000000000;

    bdet_TimeInterval interval(0,0);

    interval.addSeconds(numUnits / drainRate);
    bsls_Types::Uint64 remUnits = numUnits % drainRate;

    if(true == ceilFlag) {
        interval.addNanoseconds(
        unsigned(ceil((double(remUnits) * NANOUNITS_PER_UNIT) / drainRate)));
    }
    else {
        interval.addNanoseconds(
        unsigned(floor((double(remUnits) * NANOUNITS_PER_UNIT) / drainRate)));
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
                                           bsls_Types::Uint64       drainRate,
                                           const bdet_TimeInterval& timeWindow)
{
    BSLS_ASSERT_SAFE(drainRate  >  0);
    BSLS_ASSERT_SAFE(timeWindow >  0);

    BSLS_ASSERT_SAFE (1 == drainRate ||
                      timeWindow <= btes_LeakyBucket::calculateDrainTime(
                                                                   ULLONG_MAX,
                                                                   drainRate,
                                                                   false));

    bsls_Types::Uint64 fractionalUnitsInNanoUnits = 0;

    bsls_Types::Uint64 capacity = calculateNumberOfUnitsToDrain(
                                                   &fractionalUnitsInNanoUnits,
                                                   drainRate,
                                                   timeWindow);

    // Round the returned capacity to 1, which is okay, because it does not
    // affect the drain rate.

    return (0 != capacity) ? capacity : 1;
}

// CREATORS
btes_LeakyBucket::btes_LeakyBucket()
: d_drainRate(1)
, d_capacity(1)
, d_unitsReserved(0)
, d_unitsInBucket(0)
, d_fractionalUnitDrainednNanoUnits(0)
, d_lastUpdateTime(0)
, d_statSubmittedUnits(0)
, d_statSubmittedUnitsAtLastUpdate(0)
, d_statisticsCollectionStartTime(0)
{
}

btes_LeakyBucket::btes_LeakyBucket(bsls_Types::Uint64       drainRate,
                                   bsls_Types::Uint64       capacity,
                                   const bdet_TimeInterval& currentTime)
: d_drainRate(drainRate)
, d_capacity(capacity)
, d_unitsReserved(0)
, d_unitsInBucket(0)
, d_fractionalUnitDrainednNanoUnits(0)
, d_lastUpdateTime(currentTime)
, d_statSubmittedUnits(0)
, d_statSubmittedUnitsAtLastUpdate(0)
, d_statisticsCollectionStartTime(currentTime)
{
    BSLS_ASSERT_SAFE(drainRate > 0);
    BSLS_ASSERT_SAFE(capacity  > 0);

    // Calculate the maximum interval between updates that would not cause the
    // number of units drained to overflow an unsigned 64-bit integral type.

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

    // Calculate the maximum interval between updates that would not cause the
    // number of units drained to overflow an unsigned 64-bit integral type.

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

    bdet_TimeInterval delta = currentTime - d_lastUpdateTime;
    d_statSubmittedUnitsAtLastUpdate = d_statSubmittedUnits;

    // If delta is greater than the time it takes to drain the maximum number
    // of units representable by 64 bit integral type, then reset
    // 'unitsInBucket'.

    if (delta > d_maxUpdateInterval) {
        d_lastUpdateTime                  = currentTime;
        d_unitsInBucket                   = 0;
        d_fractionalUnitDrainednNanoUnits = 0;
        return;                                                       // RETURN
    }

    if ((delta.seconds() >= 0) && (delta.nanoseconds() >= 0)) {
        bsls_Types::Uint64 units;
        units = calculateNumberOfUnitsToDrain(
                                            &d_fractionalUnitDrainednNanoUnits,
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
        // The delta maybe negative when the system clocks are updated.  If the
        // specified 'currentTime' precedes 'statisticsCollectionStartTime',
        // adjust it to prevent the statistics collection interval from going
        // negative.

        if (currentTime < d_statisticsCollectionStartTime) {
            d_statisticsCollectionStartTime = currentTime;
        }
    }

    d_lastUpdateTime = currentTime;
}

bool btes_LeakyBucket::wouldOverflow(bsls_Types::Uint64       numUnits,
                                     const bdet_TimeInterval& currentTime)
{

    BSLS_ASSERT_SAFE(numUnits > 0);

    updateState(currentTime);

    if (numUnits > ULLONG_MAX - d_unitsInBucket - d_unitsReserved ||
        d_unitsInBucket + d_unitsReserved + numUnits > d_capacity) {

        return true;                                                  // RETURN
    }
    return false;
}

bdet_TimeInterval btes_LeakyBucket::calculateTimeToSubmit(
                                          const bdet_TimeInterval& currentTime)
{
    bsls_Types::Uint64 usedUnits = d_unitsInBucket + d_unitsReserved;

    // Return 0-length time interval if units can be submitted right now.

    if (usedUnits < d_capacity) {
        return bdet_TimeInterval(0,0);                                // RETURN
    }

    updateState(currentTime);

    // Return 0-length time interval if units can be submitted after the state
    // has been updated.

    if (d_unitsInBucket + d_unitsReserved < d_capacity) {
        return bdet_TimeInterval(0,0);                                // RETURN
    }

    bdet_TimeInterval timeToSubmit(0,0);
    bsls_Types::Uint64 backlogUnits;

    // From here, 'd_unitsInBucket + d_unitsReserved' is always greater than
    // 'd_capacity'

    backlogUnits = d_unitsInBucket + d_unitsReserved - d_capacity + 1;

    timeToSubmit = btes_LeakyBucket::calculateDrainTime(backlogUnits,
                                                        d_drainRate,
                                                        true);

    // Return 1 nanosecond if the time interval was rounded to zero (in cases
    // of high drain rates).

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

    *submittedUnits  = d_statSubmittedUnitsAtLastUpdate;
    bsls_Types::Uint64 fractionalUnits = 0;

    // 'monitoredInterval' can not be negative, as the 'updateState' method
    // ensures that 'd_lastUpdateTime' always precedes
    // 'statisticsCollectionStartTime'.

    bdet_TimeInterval monitoredInterval = d_lastUpdateTime -
                                          d_statisticsCollectionStartTime;

    bsls_Types::Uint64 drainedUnits = calculateNumberOfUnitsToDrain(
                                                            &fractionalUnits,
                                                            d_drainRate,
                                                            monitoredInterval);

    if (drainedUnits < d_statSubmittedUnitsAtLastUpdate) {
        *unusedUnits = 0;
    }
    else {
        *unusedUnits = drainedUnits - d_statSubmittedUnitsAtLastUpdate;
    }
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
