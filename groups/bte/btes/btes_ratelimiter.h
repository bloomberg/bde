// btes_ratelimiter.h                                              -*-C++-*-
#ifndef INCLUDED_BTES_RATELIMITER
#define INCLUDED_BTES_RATELIMITER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Limit peak and sustained consumption rate of a resource.
//
//@CLASSES:
//   btes_RateLimiter: a dual leaky bucket rate controller.
//
//@AUTHOR: Mikhail Kunitskiy (mkunitskiy1)
//
//@SEE ALSO btes_leakybucket
//
//@DESCRIPTION:
// This component provides a mechanism, 'btes_RateLimiter', that enables
// clients to monitor and control the use of a resource such that the peak and
// sustained resource consumption rates do no exceed configured limits. A
// 'btes_RateLimiter' object's limits on resource usage are configured through
// a peak rate and a peak-rate time-window, and a sustained rate and a
// sustained-rate time-window. The peak-rate time-window indicates a (sliding)
// time period over which the aggregate resource usage will not be allowed to 
// exceed the peak-rate, similarly the sustained-rate time-window indicates a 
// sliding time period over which the aggregate resource usage will not be
// allowed to exceed the sustained-rate.
// 'btes_RateLimiter' provides a method 'submit' with which clients report the
// use of a resource, as well as methods (e.g., 'wouldExceedBandwidth') to
// determine whether additional resources use will exceed the configured
// limits. Note that a 'btes_RateLimiter' object does not directly manage any
// resources, determine its own clock times, or manage its own threads; it
// serves to advise clients who themselves use a resource, and therefore the
// limits it helps impose are *approximations* of the rate-limiter's configured
// limit values.

// This component does not provide internal timers. Timing must be handled by
// the client. An initial time stamp is specified at creation or when the
// 'reset' method is invoked, and subsequent times are interpreted using the
// difference from this initial time point. Since 'btes_RateLimiter' cares only
// about the difference between a provided time, and the initial time, the
//  supplied 'bdet_TimeInverval' values may be relative to any arbitrary time
// origin, though clients are encouraged to use the UNIX epoch time (such as
// values returned by 'bdetu_systemtime'). To ensure consistency, all time
// intervals should refer to the same time origin.
//
// For example, suppose we have a rate limiter with a peak rate 'Rp = 2 u/s',
// peak rate time window 'Wp = 2 s', sustained rate 'Rs = 1 u/s', sustained
// rate window 'Ws = 7 s'. The rate limiter *approximates* moving totals 'T1'
// of units consumed at peak rate and 'T2' of units consumed at sustained rate.
// If 'T1' exceeds the limit 'Lp = Rp * Wp', or 'T2' exceeds the limit
// 'Ls = Rs * Ws', the rate limiter responds that no units may be submitted at
// current time, because the specified bandwidth limits will be exceeded.
//
// As illustrated in FIG.1:
//  o At time 't0' we submit 5 units. The submitted units are added to the both
//    moving average counters. The limit 'Lp = 2 * 2 = 4u' is exceeded, and we
//    can not submit any more units at this time, although the limit 'Ls' is
//    not exceeded. It means that while still keeping the average rate below
//    the allowed sustained rate, we are exceeding the allowed peak rate we
//    generate a load spike. 
//  o At time 't0 + 2s' the moving totals are recalculated, 4 units
//    ('Rp * 2 s') are subtracted from the peak rate moving total counter,
//    2 units ('Rs * 2 s') are subtracted from the sustained rate moving total
//    rate counter.  Now both limits are not exceeded, and we are free to
//    submit more units. 
//  o Then, at time 't0 + 2s' we submit 7 units and both limits are exceeded
//    now.
//  o At time 't0 + 4s' the counters are updated in the same way as at
//    't1'. The 'Lp' limit is not exceeded, but the moving total counter for
//    sustained rate still exceeds the 'Ls' limit and no more units may be
//    submitted.
//  o At time 't0 + 6s', the counters are updated again, both are not
//    exceeded now and submitting units is allowed.
//  o Then, at time 't0 + 6s' We submit one unit. The 'Lp' limit is not
//    exceeded, but 'Ls' limit is exceed now.  It means that we have not
//    generated unallowed load spike, but we have exceed the specified
//    sustained rate limit and no more units may be submitted at this time.
//
//..
// FIG. 1:  Peak rate      = 2 u/s, Peak rate window      = 2 s
//          Sustained rate = 1 u/s, Sustained rate window = 7 s
//
//       Submit 5                                       Submit 7
//
//      |   |     |   |        |   |      |   |       |   |     7|~~~|
//    12|   |    6|   |      12|   |     6|   |     12|   |     6|~~~|
//    11|   |    5|~~~|      11|   |     5|   |     11|   |     5|~~~|
//    10|   | Lp-4|~~~|      10|   |  Lp-4|---|     10|   |  Lp-4|~~~|
//     9|   |    3|~~~|       9|   |     3|   |      9|~~~|     3|~~~|
//     8|   |    2|~~~|       8|   |     2|   |      8|~~~|     2|~~~|
//  Ls-7|---|    1|~~~|    Ls-7|---|     1|~~~|   Ls-7|~~~|     1|~~~|
//     6|   |     +- -+       6|   |      +- -+      6|~~~|      +- -+
//     5|~~~|                 5|   |                 5|~~~|
//     4|~~~|                 4|   |                 4|~~~|
//     3|~~~|                 3|~~~|                 3|~~~|
//     2|~~~|                 2|~~~|                 2|~~~|
//     1|~~~|                 1|~~~|                 1|~~~|
//      +- -+                  +- -+                  +- -+
//
// Time:    t0                     t0 + 2s                  t0 + 2s
//
//
//                                                      Submit 1
//
//      |   |     7|   |       |   |     7|   |       |   |     7|   |
//    12|   |     6|   |     12|   |     6|   |     12|   |     6|   |
//    11|   |     5|   |     11|   |     5|   |     11|   |     5|   |
//    10|   |  Lp-4|---|     10|   |  Lp-4|---|     10|   |  Lp-4|---|
//     9|   |     3|~~~|      9|   |     3|   |      9|   |     3|   |
//     8|   |     2|~~~|      8|   |     2|   |      8|   |     2|~~~|
//  Ls-7|~~~|     1|~~~|   Ls-7|---|     1|~~~|   Ls-7|~~~|     1|~~~|
//     6|~~~|      +- -+      6|~~~|      +- -+      6|~~~|      +- -+
//     5|~~~|                 5|~~~|                 5|~~~|
//     4|~~~|                 4|~~~|                 4|~~~|
//     3|~~~|                 3|~~~|                 3|~~~|
//     2|~~~|                 2|~~~|                 2|~~~|
//     1|~~~|                 1|~~~|                 1|~~~|
//      +- -+                  +- -+                  +- -+
//
// Time:    t0 + 4s               t0 + 6s                t0 + 6s
//..
//
///Time Synchronization
///--------------------
// This component does not provide internal timers. Timing must be handled by
// the client. An initial lastUpdateTime is specified at creation or when the
// 'reset' method is invoked, and subsequent times are interpreted using the
// difference from this initial time point. Since 'btes_RateLimiter' cares only
// about the difference between a provided time, and the initial time, the
// supplied 'bdet_TimeInverval' values may be relative to any arbitrary time
// origin, though clients are encouraged to use the UNIX epoch time (such as
// values returned by 'bdetu_systemtime'). To ensure consistency, all time
// intervals should refer to the same time origin.
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Controlling Network Traffic Generation
///-------------------------------------------------
// Imagine that we want to send data over network at average rate of 1024
// bytes/sec and set the load spike limitations, described on FIG.2 :
// The areas 'A1' and 'A2 + B' above the maximum sustained rate 'R1 = 1024 U/s'
// should contain no more than 512 units and the area 'B' above the maximum
// peak rate 'R2 = 2048 U/s' should contain no more than 128 units.
//
//..
// FIG.2
//     ^ Rate (Units per second)
//     |                             _____         .
//     |                            /  B  \        . 
// 2048|---------------------------/-------\--------R2 (Maximum peak rate)
//     |           __             /         \      .
//     |          /  \           /    A2     \     .
//     |         / A1 \         /             \    .
// 1024|--------/------\ ------/---------------\----R1 (Maximum sustained rate)
//     |   __  /        \     /                 \__.
//     |__/  \/          \___/                     .
//     |                                           .
//      --------------------------------------------->
//                                         T (seconds)
//..
// Notice that the rate limiter does not prevent the rate at any instant from
// exceeding either of the maximum peak-rate or the maximum sustained rate, but
// instead prevents the average rate over the peak-rate time-window from
// exceeding maximum peak-rate, and the average rate over the sustained-rate
// time-window from exceeding the maximum sustained-rate.
//
// In this example, we are going to send a fixed amount of data in chunks, an
// use a 'btes_RateLimiter' to prevent our application from overloading its
// network connection.
//..
//  bool sendData(size_t dataSize)
//      // Send a specified 'dataSize' amount of data over the network
//      // return 'true' if data was sent successfully and 'false' otherwise.
//{
//..
// For simplicity, 'sendData' will not actually send any data and will
// always return 'true'.
//..
//   return true;
//}
//..
// First we define the size of data we are going to transmit and a counter
// of data that is actually sent.  We are going to send data by 64 byte
// chunks.
//..
//  bsls_Types::Uint64 bytesSent  = 0;
//  bsls_Types::Uint64 sizeOfData = 10 * 1024; // in bytes
//  bsls_Types::Uint64 chunkSize  = 64;        // in bytes
//..
// Then, we select a sustained-rate time-window, and a peak-rate time window,
// over which the average rate will not exceed the respective maximum. Note
// that the sustained-rate is meant to long-term average rate of resource
// usage, where-as the peak-rate is meant to limit spikes in resource usage,
// so the sustained-rate time-window is typically significantly longer than the
// peak-rate time-window:
//..
//  bsls_Types::Uint64 sustainedRateLimit = 1024;
//  bdet_TimeInterval  sustainedRateWindow(0.5);
//  bsls_Types::Uint64 peakRateLimit = 2048;
//  bdet_TimeInterval  peakRateWindow(0.0625);
//..
// Next, we create a'btes_RateLimiter' object, providing the
// 'sustainedRateLimit', 'peakRate', and respective time-windows defined
// earlier. Note that we provide a starting time stamp that is an interval from
// the UNIX epoch-time, and subsequent time stamps must be from the same
// reference point:
//..
//  bdet_TimeInterval now = bdetu_SystemTime::now();
//  btes_RateLimiter  rateLimiter(sustainedRateLimit,
//                                sustainedRateWindow,
//                                peakRateLimit,
//                                peakRateWindow,
//                                now);
//..
// Now, we build a loop and for each iteration we check whether submitting
// another chunk of data to the rate limiter would cause exceeding the defined
// bandwidth limits.  If not, we can send the data and submit it to the rate
// limiter.  The loop terminates when all the data is sent.  Note that 'submit'
// is invoked only after a successful operation on the resource.
//..
//  while (bytesSent < sizeOfData) {
//      now = bdetu_SystemTime::now();
//      if (!rateLimiter.wouldExceedBandwidth(now)) {
//          if (true == sendData(chunkSize)) {
//              rateLimiter.submit(chunkSize);
//              bytesSent += chunkSize;
//          }
//      }
//..
// Finally, if it is not possible to submit a new chunk of data without
// exceeding bandwidth, we invoke the 'calculateTimeToSubmit' method to
// determine how much time is required to submit a new chunk without causing
// overflow. We round up the number of microseconds in time interval.
//..
//      else {
//          bdet_TimeInterval timeToSubmit =
//                                      rateLimiter.calculateTimeToSubmit(now);
//          bsls_Types::Uint64 uS = timeToSubmit.totalMicroseconds() +
//                                 (timeToSubmit.nanoseconds() % 1000) ? 1 : 0;
//          bcemt_ThreadUtil::microSleep(uS);
//      }
//  }
//..
// Notice that in a multi-threaded application it is appropriate to put the
// thread into the 'sleep' state, in order to avoid busy-waiting.

#ifndef INCLUDED_BTES_LEAKYBUCKET
#include <btes_leakybucket.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

namespace BloombergLP {

                        //=======================
                        // class btes_RateLimiter
                        //=======================

class btes_RateLimiter {
    // This class provides a mechanism for controlling the rate at which a
    // resource is used.
    // The class invariant are:
    // 'sustainedRateLimit()  > 0'
    // 'sustainedRateWindow() > 0'
    // 'peakRateLimit() > 0'
    // 'peakRateWindow() > 0'
    // This class:
    //: o is *exception* *neutral* (agnostic)
    //: o is *const* *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    btes_LeakyBucket d_peakRateBucket;      // 'btes_LeakyBucket' object for
                                            // handling peak load

    btes_LeakyBucket d_sustainedRateBucket; // 'btes_LeakyBucket' object for
                                            // handling sustained load

private:

    // NOT IMPLEMENTED
    btes_RateLimiter& operator=(const btes_RateLimiter&);
    btes_RateLimiter(const btes_RateLimiter&);

public:

    // CREATORS
    btes_RateLimiter();
        // Create a btes_RateLimiter object having peak rate of 10 units per
        // second, peak rate window of 1 second, sustained rate of 1 unit per
        // second, sustained rate window of 10 seconds and using zero as the
        // reference point for time intervals.

    btes_RateLimiter(bsls_Types::Uint64       sustainedRateLimit,
                     const bdet_TimeInterval& sustainedRateWindow,
                     bsls_Types::Uint64       peakRateLimit,
                     const bdet_TimeInterval& peakRateWindow,
                     const bdet_TimeInterval& currentTime);
        // Create a btes_RateLimiter object, having the specified
        // 'sustainedRateLimit' in units per second, the specified
        // 'sustainedRateWindow' size of time window for sustained rate,
        // the specified 'peakRateLimit' in units per second, the specified
        // 'peakRateWindow' size of time window for peak rate and using
        // the specified 'currentTime' as the lastUpdateTime.
        // The behavior is undefined unless '0 < sustainedRateLimit',
        // '0 < sustainedRateWindow', '0 < peakRateLimit',
        // '0 < peakRateWindow', the product of 'sustainedRateLimit' and
        // 'sustainedRateWindow' can be represented by 64-bit unsigned
        // integral type and the product of 'peakRateLimit' and
        // 'peakRateWindow' can be represented by 64-bit unsigned integral
        // type.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)

    // The following destructor is generated by the compiler, except in "SAFE"
    // build modes (e.g., to enable the checking of class invariants).

    ~btes_RateLimiter();
        // Destroy this object.

#endif

    // MANIPULATORS
    void updateState(const bdet_TimeInterval& currentTime);
        // Set the 'lastUpdateTime' of this rate limiter to the specified
        // 'currentTime'.  If the specified 'currentTime' is after
        // 'lastUpdateTime', then recalculate number of units available for
        // consumption, based on the 'peakRate', 'sustainedRate' and the time
        // interval between 'lastUpdateTime' and 'currentTime'.
        // If 'currentTime' is before the value, returned by 
        // 'statisticsCollectionStartTime' method, set the
        // 'statisticsCollectionStartTime' to 'currentTime'.

    void resetStatistics();
        // Reset the statics for the number of units used and the number of
        // units submitted to 0, and set the 'statisticsCollectionStartTime' to
        // the current 'lastUpdateTime'.

    void setRateLimits(bsls_Types::Uint64        newSustainedRateLimit,
                       const bdet_TimeInterval& newSustainedRateWindow,
                       bsls_Types::Uint64       newPeakRateLimit,
                       const bdet_TimeInterval& newPeakRateWindow);
        // Set the sustained rate limit of this rate limiter to the specified
        // 'newSustainedRateLimit' in units per second, the time window size 
        // for the sustained rate to the specified 'newSustainedRateWindow',
        // peak rate limit to the specified 'newPeakRateLimit' and the time
        // window size for the peak rate to the specified
        // 'newPeakRateWindow'. The behavior is undefined unless
        // '0 < sustainedRateLimit', '0 < sustainedRateWindow',
        // '0 < peakRateLimit', '0 < peakRateWindow',
        // the product of 'sustainedRateLimit' and 'sustainedRateWindow'
        // can be represented by 64-bit unsigned integral type and
        // the product of 'peakRateLimit' and 'peakRateWindow'
        // can be represented by 64-bit unsigned integral type.

    void submit(bsls_Types::Uint64 numOfUnits);
        // Add, unconditionally, the specified 'numOfUnits' to this rate
        // limiter.  The behavior is undefined unless the sum of:
        // (1) 'numOfUnits', (2) units previously submitted to this rate
        // limit but not yet used, and (3) 'unitsReserved' can be represented
        // by a 64-bit unsigned integral type.

    void reserve(bsls_Types::Uint64 numOfUnits);
        // Reserve, uncoditionally, the specified 'numOfUnits' for future
        // usage.  The behavior is undefined unless the value
        // 'unitsReserved() + unitsInBucket() + numOfUnits' can be represented
        // by 64-bit integral type.  Note that after this operation number of
        // reserved units may exceed the capacity of the bucket.  Also note
        // that the time interval between the invocations of 'reserve' and
        // 'submitReserved' or 'cancelReserved' should be as short as possible,
        // to avoid long-lasting reservations that may skew results from
        // operations like 'calculateTimeToSubmit'.

    void cancelReserved(bsls_Types::Uint64 numOfUnits);
        // Cancel the reservation of the specified 'numOfUnits' that were
        // previously reserved.  The behavior is undefined unless
        // 'numOfUnits <= unitsReserved()'.

    void submitReserved(bsls_Types::Uint64 numOfUnits);
        // Submit the specified 'numOfUnits' that were previosly reserved.
        // The behavior is undefined unless 'numOfUnits <= unitsReserved'.

    bool wouldExceedBandwidth(const bdet_TimeInterval& currentTime);
        // Update the state of this rate limiter, set 'lastUpdateTime' to the
        // specified 'currentTime' and return 'true' if submitting one more
        // unit at the 'currentTime' would exceed the configured limits.
        // Return 'false' otherwise.

    bdet_TimeInterval calculateTimeToSubmit(
                                         const bdet_TimeInterval& currentTime);
        // Return the estimated time interval, that should pass since the
        // specified 'currentTime', until it would be possible to submit one
        // more unit into this rate limiter, without exceeding the capacity.
        // Return an interval of length 0, if one more unit can be submitted
        // at the specifiled 'currentTime'. Otherwise, set 'lastUpdateTime'
        // to 'currentTime' and return the time interval, that should pass
        // until one more unit can be submitted.  The number of nanoseconds in
        // the time interval is rounded up.  Note that after waiting for the
        // returned interval, a client should typically check 'wouldOverlow'
        // before submitting units, as additional units may have been submitted
        // in the interim.

    void reset(const bdet_TimeInterval& currentTime);
        // Reset the rate limiter to its default-constructed state and set the
        // 'lastUpdateTime' of this rate limiter to the specified 'currentTime'.

    // ACCESSORS
    bsls_Types::Uint64 peakRateLimit() const;
        // Return the peak rate in units per second

    bsls_Types::Uint64 sustainedRateLimit() const;
        // Return sustained rate limit in units per second

    bdet_TimeInterval peakRateWindow() const;
        // Return the time-period over which the average rate of resource usage
        // will be limited to (approximately) the peak rate. Note that this
        // period is generally significantly shorter than
        // 'sustainedRateWindow'.

    bdet_TimeInterval sustainedRateWindow() const;
        // Return the time-period over which the average rate of resource usage
        // will be limited to (approximately) the sustained rate. Note that
        // this period is generally significantly longer than the
        // 'peakRateWindow'.

    bsls_Types::Uint64 unitsReserved() const;
        // Return the number of units that are reserved.

    bdet_TimeInterval lastUpdateTime() const;
        // Return the most recent time, as a time interval, that this rate
        // limiter was updated.  The returned time interval uses the same
        // reference point as the time interval specified during construction
        // or last invocation of the 'reset' method.

    void getStatistics(bsls_Types::Uint64* submittedUnits,
                       bsls_Types::Uint64* unusedUnits) const;
        // Load, into the specified 'submittedUnits' and 'unusedUnits' the
        // numbers of submitted units and unused units respectively.
        // The number of submitted units is the total number of units submitted
        // to this rate-limiter between 'statisticsCollectionStartTime' and
        // 'lastUpdateTime'.  The number of unused units is the number of
        // additional units that could have been submitted, but were not,
        // between 'statisticsCollectionStartTime' and 'lastUpdateTime'.

    bdet_TimeInterval statisticsCollectionStartTime() const;
        // Return the time when the collection of the statistics (as returned
        // by 'getStatistics') started.  The returned time interval uses the
        // same reference point as the time interval specified during
        // construction or last invocation of the 'reset' method.

};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        //-----------------------
                        // class btes_RateLimiter
                        //-----------------------

// CREATORS
inline
btes_RateLimiter::btes_RateLimiter()
{
    setRateLimits(1, bdet_TimeInterval(10), 10, bdet_TimeInterval(1));
    reset(bdet_TimeInterval(0));
}

// MANIPULATORS
inline
void btes_RateLimiter::updateState(const bdet_TimeInterval& currentTime)
{
    d_peakRateBucket.updateState(currentTime);
    d_sustainedRateBucket.updateState(currentTime);
}

inline
void btes_RateLimiter::submit(bsls_Types::Uint64 numOfUnits)
{
    BSLS_ASSERT_SAFE(numOfUnits <= ULLONG_MAX - 
                                   d_sustainedRateBucket.unitsInBucket());

    BSLS_ASSERT_SAFE(unitsReserved() <= ULLONG_MAX -
                                        d_sustainedRateBucket.unitsInBucket()-
                                        numOfUnits);

    // Possibly resundant check. Implemented, unless there is no check,
    // whether 'peakRateLimit() > sustainedRateLimit()'

    BSLS_ASSERT_SAFE(numOfUnits <= ULLONG_MAX - 
                                   d_peakRateBucket.unitsInBucket());

    BSLS_ASSERT_SAFE(unitsReserved() <= ULLONG_MAX -
                                        d_peakRateBucket.unitsInBucket()-
                                        numOfUnits);

    d_peakRateBucket.submit(numOfUnits);
    d_sustainedRateBucket.submit(numOfUnits);
}

inline
void btes_RateLimiter::reserve(bsls_Types::Uint64 numOfUnits)
{
    BSLS_ASSERT_SAFE(numOfUnits <= ULLONG_MAX - 
                                   unitsReserved());

    BSLS_ASSERT_SAFE(d_sustainedRateBucket.unitsInBucket() <= ULLONG_MAX - 
                                                              unitsReserved()-
                                                              numOfUnits);

    // Possibly resundant check. Implemented, unless there is no check,
    // whether 'peakRateLimit() > sustainedRateLimit()'.

    BSLS_ASSERT_SAFE(d_peakRateBucket.unitsInBucket() <= ULLONG_MAX - 
                                                         unitsReserved()-
                                                         numOfUnits);

    d_peakRateBucket.reserve(numOfUnits);
    d_sustainedRateBucket.reserve(numOfUnits);
}

inline
void btes_RateLimiter::submitReserved(bsls_Types::Uint64 numOfUnits)
{
    BSLS_ASSERT_SAFE(numOfUnits <= unitsReserved());

    // There is no need to check, whether 'numOfUnits' causes overflow, because
    // number of units to be reserved was checked by the 'reserve' method and
    // 'numOfUnits <= unitsReserved'.

    d_peakRateBucket.submitReserved(numOfUnits);
    d_sustainedRateBucket.submitReserved(numOfUnits);
}

inline
void btes_RateLimiter::cancelReserved(bsls_Types::Uint64 numOfUnits)
{
    BSLS_ASSERT_SAFE(numOfUnits <= unitsReserved());

    d_peakRateBucket.cancelReserved(numOfUnits);
    d_sustainedRateBucket.cancelReserved(numOfUnits);
}

inline
bool btes_RateLimiter::wouldExceedBandwidth(
                                          const bdet_TimeInterval& currentTime)
{
    return (d_peakRateBucket.wouldOverflow(1,currentTime) ||
            d_sustainedRateBucket.wouldOverflow(1,currentTime));
}

inline
void btes_RateLimiter::resetStatistics()
{
    d_sustainedRateBucket.resetStatistics();
}

inline
void btes_RateLimiter::reset(const bdet_TimeInterval& currentTime)
{
    d_peakRateBucket.reset(currentTime);
    d_sustainedRateBucket.reset(currentTime);
}

// ACCESSORS
inline
void btes_RateLimiter::getStatistics(
                                bsls_Types::Uint64* submittedUnits,
                                bsls_Types::Uint64* unusedUnits) const
{
    BSLS_ASSERT_SAFE(0 != submittedUnits);
    BSLS_ASSERT_SAFE(0 != unusedUnits);

    // The statistics is collected for the sustained rate leaky bucket.

    d_sustainedRateBucket.getStatistics(submittedUnits, unusedUnits);
}

inline
bsls_Types::Uint64 btes_RateLimiter::peakRateLimit() const
{
    return d_peakRateBucket.drainRate();
}

inline
bsls_Types::Uint64 btes_RateLimiter::sustainedRateLimit() const
{
    return d_sustainedRateBucket.drainRate();
}

inline
bdet_TimeInterval btes_RateLimiter::peakRateWindow() const
{
    return btes_LeakyBucket::calculateTimeWindow(d_peakRateBucket.drainRate(),
                                                 d_peakRateBucket.capacity());
}

inline
bdet_TimeInterval btes_RateLimiter::sustainedRateWindow() const
{
    return btes_LeakyBucket::calculateTimeWindow(
                                            d_sustainedRateBucket.drainRate(),
                                            d_sustainedRateBucket.capacity());
}

inline
bdet_TimeInterval btes_RateLimiter::statisticsCollectionStartTime() const
{
    return d_sustainedRateBucket.statisticsCollectionStartTime();
}

inline
bdet_TimeInterval btes_RateLimiter::lastUpdateTime() const
{
    return bsl::max(d_sustainedRateBucket.lastUpdateTime(),
                    d_peakRateBucket.lastUpdateTime());
}

inline
bsls_Types::Uint64 btes_RateLimiter::unitsReserved() const
{
    BSLS_ASSERT_SAFE(d_sustainedRateBucket.unitsReserved() ==
                     d_peakRateBucket.unitsReserved());

    return d_sustainedRateBucket.unitsReserved();
}

}  // closed enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
