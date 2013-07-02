// btes_leakybucket.cpp                                               -*-C++-*-
#include <btes_leakybucket.h>

#include <bsl_c_math.h>

namespace BloombergLP {

namespace {

bsls::Types::Uint64 calculateNumberOfUnitsToDrain(
                     bsls::Types::Uint64*     fractionalUnitDrainedInNanoUnits,
                     bsls::Types::Uint64      drainRate,
                     const bdet_TimeInterval& timeInterval)
    // Return the number of units that would be drained from a leaky bucket
    // over the specified 'timeInterval' at the specified 'drainRate', plus the
    // specified 'fractionalUnitDrainedInNanoUnits', representing a fractional
    // remainder from a previous call to 'calculateNumberOfUnitsToDrain'.  Load
    // into 'fractionalUnitDrainedInNanoUnits' the fractional remainder
    // (between 0.0 and 1.0, represented in nano-units) from this caculation.
    // The behavior is undefined unless
    // '0 <= *fractionalUnitDrainedInNanoUnits < 1000000000' (i.e., it
    // represents a value between 0 and 1 unit) and
    // 'timeInterval.seconds() * drainRate <= ULLONG_MAX'.  Note that
    // 'fractionalUnitDrainedInNanoUnits' is represented in nano-units to avoid
    // using a floating point representation.

{
    const bsls::Types::Uint64 NANOUNITS_PER_UNIT = 1000000000;

    BSLS_ASSERT(static_cast<bsls::Types::Uint64>(timeInterval.seconds()) <=
                                                       ULLONG_MAX / drainRate);
    BSLS_ASSERT(0 != fractionalUnitDrainedInNanoUnits);
    BSLS_ASSERT(*fractionalUnitDrainedInNanoUnits < NANOUNITS_PER_UNIT);

    bsls::Types::Uint64 units  = drainRate * timeInterval.seconds();
    units += (drainRate / NANOUNITS_PER_UNIT) * timeInterval.nanoseconds();

    bsls::Types::Uint64 nanounits = 0;

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
                                                 bsls::Types::Uint64 numUnits,
                                                 bsls::Types::Uint64 drainRate,
                                                 bool                ceilFlag)
{
    BSLS_ASSERT(drainRate > 1 || numUnits <= LLONG_MAX);

    const bsls::Types::Uint64 NANOUNITS_PER_UNIT = 1000000000;

    bdet_TimeInterval interval(0,0);

    interval.addSeconds(numUnits / drainRate);
    bsls::Types::Uint64 remUnits = numUnits % drainRate;

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
                                                 bsls::Types::Uint64 drainRate,
                                                 bsls::Types::Uint64 capacity)
{

    BSLS_ASSERT(drainRate > 0);
    BSLS_ASSERT(drainRate > 1 || capacity <= LLONG_MAX);

    bdet_TimeInterval window = btes_LeakyBucket::calculateDrainTime(capacity,
                                                                    drainRate,
                                                                    true);

    if (0 == window) {
        window.addNanoseconds(1);
    }

    return window;
}

bsls::Types::Uint64 btes_LeakyBucket::calculateCapacity(
                                           bsls::Types::Uint64      drainRate,
                                           const bdet_TimeInterval& timeWindow)
{
    BSLS_ASSERT(1 == drainRate ||
                timeWindow <= btes_LeakyBucket::calculateDrainTime(ULLONG_MAX,
                                                                   drainRate,
                                                                   false));

    bsls::Types::Uint64 fractionalUnitsInNanoUnits = 0;

    bsls::Types::Uint64 capacity = calculateNumberOfUnitsToDrain(
                                                   &fractionalUnitsInNanoUnits,
                                                   drainRate,
                                                   timeWindow);

    // Round the returned capacity to 1, which is okay, because it does not
    // affect the drain rate.

    return (0 != capacity) ? capacity : 1;
}

// CREATORS
btes_LeakyBucket::btes_LeakyBucket(bsls::Types::Uint64       drainRate,
                                   bsls::Types::Uint64       capacity,
                                   const bdet_TimeInterval&  currentTime)
: d_drainRate(drainRate)
, d_capacity(capacity)
, d_unitsReserved(0)
, d_unitsInBucket(0)
, d_fractionalUnitDrainedInNanoUnits(0)
, d_lastUpdateTime(currentTime)
, d_statSubmittedUnits(0)
, d_statSubmittedUnitsAtLastUpdate(0)
, d_statisticsCollectionStartTime(currentTime)
{
    BSLS_ASSERT_OPT(0 < d_drainRate);
    BSLS_ASSERT_OPT(0 < d_capacity);

    // Calculate the maximum interval between updates that would not cause the
    // number of units drained to overflow an unsigned 64-bit integral type.

    if (drainRate == 1) {

        // 'd_maxUpdateInterval' is a signed 64-bit integral type that can't
        // represent 'ULLONG_MAX' number of seconds, so we set
        // 'd_maxUpdateInterval' to the maximum representable value when
        // 'drainRate' is 1.

        d_maxUpdateInterval = bdet_TimeInterval(LLONG_MAX, 999999999);
    }
    else {
        d_maxUpdateInterval = btes_LeakyBucket::calculateDrainTime(ULLONG_MAX,
                                                                   drainRate,
                                                                   false);
    }
}

// MANIPULATORS
bdet_TimeInterval btes_LeakyBucket::calculateTimeToSubmit(
                                          const bdet_TimeInterval& currentTime)
{
    bsls::Types::Uint64 usedUnits = d_unitsInBucket + d_unitsReserved;

    // Return 0-length time interval if units can be submitted right now.

    if (usedUnits < d_capacity) {
        return bdet_TimeInterval(0, 0);                               // RETURN
    }

    updateState(currentTime);

    // Return 0-length time interval if units can be submitted after the state
    // has been updated.

    if (d_unitsInBucket + d_unitsReserved < d_capacity) {
        return bdet_TimeInterval(0, 0);                               // RETURN
    }

    bdet_TimeInterval timeToSubmit(0,0);
    bsls::Types::Uint64 backlogUnits;

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

void btes_LeakyBucket::setRateAndCapacity(bsls::Types::Uint64 newRate,
                                          bsls::Types::Uint64 newCapacity)
{
    BSLS_ASSERT_SAFE(0 < newRate);
    BSLS_ASSERT_SAFE(0 < newCapacity);

    d_drainRate  = newRate;
    d_capacity   = newCapacity;

    // Calculate the maximum interval between updates that would not cause the
    // number of units drained to overflow an unsigned 64-bit integral type.


    if (newRate == 1) {
        d_maxUpdateInterval = bdet_TimeInterval(LLONG_MAX, 999999999);
    }
    else {
        d_maxUpdateInterval = btes_LeakyBucket::calculateDrainTime(ULLONG_MAX,
                                                                   newRate,
                                                                   false);
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
        d_lastUpdateTime                   = currentTime;
        d_unitsInBucket                    = 0;
        d_fractionalUnitDrainedInNanoUnits = 0;
        return;                                                       // RETURN
    }

    if (delta >= bdet_TimeInterval(0, 0)) {
        bsls::Types::Uint64 units;
        units = calculateNumberOfUnitsToDrain(
                                           &d_fractionalUnitDrainedInNanoUnits,
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

bool btes_LeakyBucket::wouldOverflow(const bdet_TimeInterval& currentTime)
{
    updateState(currentTime);

    if (1 > ULLONG_MAX - d_unitsInBucket - d_unitsReserved ||
        d_unitsInBucket + d_unitsReserved + 1 > d_capacity) {

        return true;                                                  // RETURN
    }
    return false;
}

// ACCESSORS
void btes_LeakyBucket::getStatistics(bsls::Types::Uint64* submittedUnits,
                                     bsls::Types::Uint64* unusedUnits) const
{

    BSLS_ASSERT(0 != submittedUnits);
    BSLS_ASSERT(0 != unusedUnits);

    *submittedUnits  = d_statSubmittedUnitsAtLastUpdate;
    bsls::Types::Uint64 fractionalUnits = 0;

    // 'monitoredInterval' can not be negative, as the 'updateState' method
    // ensures that 'd_lastUpdateTime' always precedes
    // 'statisticsCollectionStartTime'.

    bdet_TimeInterval monitoredInterval = d_lastUpdateTime -
                                          d_statisticsCollectionStartTime;

    bsls::Types::Uint64 drainedUnits = calculateNumberOfUnitsToDrain(
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
