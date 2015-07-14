// btls_ratelimiter.cpp                                               -*-C++-*-
#include <btls_ratelimiter.h>

namespace BloombergLP {

namespace btls {
                        //-----------------------
                        // class RateLimiter
                        //-----------------------

// CREATORS
RateLimiter::RateLimiter(
                                  bsls::Types::Uint64      sustainedRateLimit,
                                  const bsls::TimeInterval& sustainedRateWindow,
                                  bsls::Types::Uint64      peakRateLimit,
                                  const bsls::TimeInterval& peakRateWindow,
                                  const bsls::TimeInterval& currentTime)
: d_peakRateBucket(1, 1, currentTime)
, d_sustainedRateBucket(1, 1, currentTime)
{
    setRateLimits(sustainedRateLimit,
                  sustainedRateWindow,
                  peakRateLimit,
                  peakRateWindow);
}

RateLimiter::~RateLimiter()
{
    BSLS_ASSERT_SAFE(sustainedRateLimit() > 0);
    BSLS_ASSERT_SAFE(peakRateLimit()      > 0);

    BSLS_ASSERT_SAFE(sustainedRateWindow() > bsls::TimeInterval(0));
    BSLS_ASSERT_SAFE(peakRateWindow()      > bsls::TimeInterval(0));

    BSLS_ASSERT_SAFE(peakRateLimit()  == 1 ||
        peakRateWindow() <= LeakyBucket::calculateDrainTime(
                                                               ULLONG_MAX,
                                                               peakRateLimit(),
                                                               true));

    BSLS_ASSERT_SAFE(sustainedRateLimit()  == 1 ||
        sustainedRateWindow() <= LeakyBucket::calculateDrainTime(
                                                          ULLONG_MAX,
                                                          sustainedRateLimit(),
                                                          true));
}

// MANIPULATORS

void RateLimiter::setRateLimits(
                               bsls::Types::Uint64      sustainedRateLimit,
                               const bsls::TimeInterval& sustainedRateWindow,
                               bsls::Types::Uint64      peakRateLimit,
                               const bsls::TimeInterval& peakRateWindow)
{
    BSLS_ASSERT(sustainedRateLimit > 0);
    BSLS_ASSERT(peakRateLimit      > 0);

    BSLS_ASSERT(sustainedRateWindow > bsls::TimeInterval(0));
    BSLS_ASSERT(peakRateWindow      > bsls::TimeInterval(0));

    BSLS_ASSERT(peakRateLimit  == 1 ||
        peakRateWindow <= LeakyBucket::calculateDrainTime(ULLONG_MAX,
                                                               peakRateLimit,
                                                               true));

    BSLS_ASSERT(sustainedRateLimit  == 1 ||
        sustainedRateWindow <= LeakyBucket::calculateDrainTime(
                                                            ULLONG_MAX,
                                                            sustainedRateLimit,
                                                            true));

    bsls::Types::Uint64 capacity = LeakyBucket::calculateCapacity(
                                                          sustainedRateLimit,
                                                          sustainedRateWindow);

    d_sustainedRateBucket.setRateAndCapacity(sustainedRateLimit, capacity);

    capacity = LeakyBucket::calculateCapacity(peakRateLimit,
                                                   peakRateWindow);


    d_peakRateBucket.setRateAndCapacity(peakRateLimit, capacity);

}



bsls::TimeInterval RateLimiter::calculateTimeToSubmit(
                                         const bsls::TimeInterval& currentTime)
{
    bsls::TimeInterval timeToSubmitPeak =
                           d_peakRateBucket.calculateTimeToSubmit(currentTime);

    bsls::TimeInterval timeToSubmitSustained =
                      d_sustainedRateBucket.calculateTimeToSubmit(currentTime);

    return bsl::max(timeToSubmitPeak,timeToSubmitSustained);
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
