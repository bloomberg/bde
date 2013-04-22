// btes_ratelimiter.cpp                                               -*-C++-*-
#include <btes_ratelimiter.h>

namespace BloombergLP {

                        //-----------------------
                        // class btes_RateLimiter
                        //-----------------------

// CREATORS
btes_RateLimiter::btes_RateLimiter(
                                  bsls::Types::Uint64      sustainedRateLimit,
                                  const bdet_TimeInterval& sustainedRateWindow,
                                  bsls::Types::Uint64      peakRateLimit,
                                  const bdet_TimeInterval& peakRateWindow,
                                  const bdet_TimeInterval& currentTime)
: d_peakRateBucket(1, 1, currentTime)
, d_sustainedRateBucket(1, 1, currentTime)
{
    setRateLimits(sustainedRateLimit,
                  sustainedRateWindow,
                  peakRateLimit,
                  peakRateWindow);
}

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)

btes_RateLimiter::~btes_RateLimiter()
{
    BSLS_ASSERT_SAFE(sustainedRateLimit() > 0);
    BSLS_ASSERT_SAFE(peakRateLimit()      > 0);

    BSLS_ASSERT_SAFE(sustainedRateWindow() > bdet_TimeInterval(0));
    BSLS_ASSERT_SAFE(peakRateWindow()      > bdet_TimeInterval(0));

    BSLS_ASSERT_SAFE(peakRateLimit()  == 1 ||
        peakRateWindow() <= btes_LeakyBucket::calculateDrainTime(
                                                               ULLONG_MAX,
                                                               peakRateLimit(),
                                                               true));

    BSLS_ASSERT_SAFE(sustainedRateLimit()  == 1 ||
        sustainedRateWindow() <= btes_LeakyBucket::calculateDrainTime(
                                                          ULLONG_MAX,
                                                          sustainedRateLimit(),
                                                          true));
}

#endif

// MANIPULATORS

void btes_RateLimiter::setRateLimits(
                               bsls::Types::Uint64      sustainedRateLimit,
                               const bdet_TimeInterval& sustainedRateWindow,
                               bsls::Types::Uint64      peakRateLimit,
                               const bdet_TimeInterval& peakRateWindow)
{
    BSLS_ASSERT(sustainedRateLimit > 0);
    BSLS_ASSERT(peakRateLimit      > 0);

    BSLS_ASSERT(sustainedRateWindow > bdet_TimeInterval(0));
    BSLS_ASSERT(peakRateWindow      > bdet_TimeInterval(0));

    BSLS_ASSERT(peakRateLimit  == 1 ||
        peakRateWindow <= btes_LeakyBucket::calculateDrainTime(ULLONG_MAX,
                                                               peakRateLimit,
                                                               true));

    BSLS_ASSERT(sustainedRateLimit  == 1 ||
        sustainedRateWindow <= btes_LeakyBucket::calculateDrainTime(
                                                            ULLONG_MAX,
                                                            sustainedRateLimit,
                                                            true));

    bsls::Types::Uint64 capacity = btes_LeakyBucket::calculateCapacity(
                                                          sustainedRateLimit,
                                                          sustainedRateWindow);

    d_sustainedRateBucket.setRateAndCapacity(sustainedRateLimit, capacity);

    capacity = btes_LeakyBucket::calculateCapacity(peakRateLimit,
                                                   peakRateWindow);


    d_peakRateBucket.setRateAndCapacity(peakRateLimit, capacity);

}



bdet_TimeInterval btes_RateLimiter::calculateTimeToSubmit(
                                         const bdet_TimeInterval& currentTime)
{
    bdet_TimeInterval timeToSubmitPeak =
                           d_peakRateBucket.calculateTimeToSubmit(currentTime);

    bdet_TimeInterval timeToSubmitSustained =
                      d_sustainedRateBucket.calculateTimeToSubmit(currentTime);

    return bsl::max(timeToSubmitPeak,timeToSubmitSustained);
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
