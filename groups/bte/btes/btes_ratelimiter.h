// btes_ratelimiter.h                                              -*-C++-*-
#ifndef INCLUDED_BTES_RATELIMITER
#define INCLUDED_BTES_RATELIMITER

//@PURPOSE: Limit peak and sustained consumption rate of a resource.
//
//@CLASSES:
//   btes_RateLimiter: a dual leaky bucket rate controller.
//
//@AUTHOR: Mikhail Kunitskiy (mkunitskiy1)
//
//@SEE ALSO btes_leakybucket
//
//@DESCRIPTION: this component provides a mechanism, 'btes_RateLimiter', to
// monitor and control that maximum peak and sustained resource consumption
// rate does not exceed the specified limits over the specified sliding time
// windows.
// Peak and sustained rate along with peak rate time window and sutained rate
// time window respectively define the characteristics of load spikes that are
// allowed.
// Rate limiter monitors, how many units are submitted over a period of time
// and how many units are processed and can be queried, whether submission of
// additional units, representing load generation, would lead to exceeding
// the specified resource consumtion rate limits.
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
//    the allowed sustained rate, we are exceeding the allowed peak rate and
//    generate a load spike. 
//  o At time 't1 = t0 + 2s' the moving totals are recalculated(A), 4 units
//    ('Rp * 2 s') are subtracted from the peak rate moving total counter,
//    2 units ('Rs * 2 s') are subtracted from the sustained rate moving total
//    rate counter.  Now both limits are not exceeded, and we are free to
//    submit more units.   We submit 7 units and both limits are exceeded(B).
//  o At time 't2 = t1 + 2s' the counters are updated in the same way as at
//    't1'. The 'Lp' limit is not exceeded, but the moving total counter for
//    sustained rate still exceeds the 'Ls' limit and no more units may be
//    submitted.
//  o At time 't3 = t2 + 2', the counters are updated again, both are not
//    exceeded now and submitting units is allowed(A).  We submit one unit(B).
//    The 'Lp' limit is not exceeded, but 'Ls' limit is exceed now.  It means
//    that we have not generated unallowed load spike, but we have exceed the
//    specified sustained rate limit and no more units may be submitted at this
//    time.
//
//..
// FIG. 1:  Peak rate      = 2 u/s, Peak rate window      = 2 s
//          Sustained rate = 1 u/s, Sustained rate window = 7 s
//
//       Submit 5                                       Submit 7
//
//      |   |     |   |        |   |      |   |       |   |      |   |
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
//                                     A                     B    
//                           \_________________________________________/
// Time:  t0                                t1 = t0 + 2s
//
//
//                                                      Submit 1
//
//      |   |      |   |       |   |      |   |       |   |      |   |
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
//                                      A                     B
//                          \____________________________________________/
// Time:   t2 = t1 + 2s                      t3 = t2 + 2s
//..
//
///Time Synchronization
///--------------------
// This component does not provide internal timers.  Timing must be handled
// by the client. The calculation of the current number of units in the leaky
// bucket is based on the timestamps (external timing) provided by the client.
// An initial timestamp is specified at creation or when the 'reset' method is
// invoked.  The timestamps are represented as time intervals from an arbitrary
// time origin point (e.g., UNIX epoch).  To ensure consistency, all time
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
//     |                             _____
//     |                            /  B  \
// 2048|---------------------------/-------\--------R2 (Maximum peak rate)
//     |           __             /         \
//     |          /  \           /    A2     \
//     |         / A1 \         /             \
// 1024|--------/------\ ------/---------------\----R1 (Maximum sustained rate)
//     |   __  /        \     /                 \___
//     |__/  \/          \___/
//     |
//      --------------------------------------------->
//                                         T (seconds)
//..
// We are going to send certain amount of data by chunks, and in order to
// prevent exceeding allowed bandwidth limits, we need to control traffic
// generation rate of our application.
//
// Also, assume that we have a function 'sendData', that transmits data
// contained in a buffer over said network interface:
//..
//  bool mySendData(size_t dataSize)
//      Send a specified 'dataSize' amount of data over the network
//      return 'true' if data was sent successfully and 'false' otherwise.
//{
//..
// For simplicity, 'mySendData' will not actually send any data and will
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
// We assume that one unit represents one byte and define sustained rate and
// time window for the sustained rate.  We limit sustained rate to 1024
// units/second and area above this rate to 512 units(see FIG.2).
// The sustained rate time window may be calculated from the following
// equation:
// Rate * T = N
// where 'Rate' is sustained rate,
// 'T' is time window to be calculated,
// 'N' is maximum size of area above the sustained rate.
// T = 512 / 1024 = 0.5 seconds.
//..
//  bsls_Types::Uint64 sustainedRateLimit = 1024;
//  bdet_TimeInterval  sustainedRateWindow(0.5);
//..
// We define maximum peak rate and time window for the peak rate.
// As mentioned above, we limit maximum peak rate to 2048 units/second and
// limit area above the maximum peak rate to 128 units(see FIG.2).
// We calculate the time window size for peak rate using the same equation, as
// for the sustained rate
// T = 128 / 2048 = 0.0625 seconds.
//..
//  bsls_Types::Uint64 peakRateLimit = 2048;
//  bdet_TimeInterval  peakRateWindow(0.0625);
//..
// Then we create the btes_RateLimiter object, specifying 'sustainedRateLimit',
// 'peakRateLimit' and calculated time windows for sustained and peak rates.
// Note that at creation we specify the time reference point as an interval
// from UNIX epoch.  Time intervals specified for all further
// 'wouldExceedBandwidth()', 'updateState()', 'calculateTimeToSubmit()'
// calls are calculated from the same reference point (UNIX epoch).
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
//          if (true == mySendData(chunkSize)) {
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
    // This mechanism is an implementation of the dual leaky bucket rate
    // controlling algorithm.
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
                                            // handling short burst load

    btes_LeakyBucket d_sustainedRateBucket; // 'btes_LeakyBucket' object for
                                            // handling sustained load

    // NOT IMPLEMENTED
    btes_LeakyBucket& operator=(const btes_LeakyBucket&);

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
        // 'sustainedRateLimit' in units per second, specified
        // 'sustainedRateWindow' size of time window for sustained rate,
        // specified 'peakRateLimit' in units per second,
        // 'peakRateWindow' size of time window for peak rate and using
        // specified 'currentTime' as the timestamp.
        // The behavior is undefined unless '0 < sustainedRateLimit',
        // '0 < sustainedRateWindow', '0 < peakRateLimit',
        // '0 < peakRateWindow',
        // the product of 'sustainedRateLimit' and 'sustainedRateWindow'
        // can be represented by 64-bit unsigned integral type,
        // the product of 'peakRateLimit' and 'peakRateWindow'
        // can be represented by 64-bit unsigned integral type.

    //! btes_RateLimiter(const btes_RateLimiter& original) = default;
        // Create a 'btes_RateLimiter' object having the parameters and state
        // of the specified 'original' object.  Note that this method's
        // definition is compiler generated.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)

    // The following destructor is generated by the compiler, except in "SAFE"
    // build modes (e.g., to enable the checking of class invariants).

    ~btes_RateLimiter();
        // Destroy this object.

#endif

    // MANIPULATORS
    void updateState(const bdet_TimeInterval& currentTime);
        // Recalculate the number of units available for consumption at the
        // specified 'currentTime', if 'currentTime' is later than the value,
        // returned by the 'timestamp' method.  Set the 'timestamp' of
        // this rate limiter to 'currentTime'.  If 'currentTime' is before the
        // value, returned by 'statisticsTimestamp' method, set the 
        // 'statisticsTimestamp' of this rate limiter to 'currentTime'.

    void resetStatistics();
        // Reset the used units statistics counter and time interval of
        // collecting statistics.  Set the reference point for the interval
        // of collecting statistics to 'timestamp'.

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
        // can be represented by 64-bit unsigned integral type,
        // the product of 'peakRateLimit' and 'peakRateWindow'
        // can be represented by 64-bit unsigned integral type.

    void submit(bsls_Types::Uint64 numOfUnits);
        // Add, unconditionally, the specified 'numOfUnits' to this rate
        // limiter.  The behavior is undefined unless the sum of units
        // submitted to the rate limiter and not processed, units to submit and
        // units reserved can be represented by 64-bit unsigned integral type.

    void reserve(bsls_Types::Uint64 numOfUnits);
        // Reserve, uncoditionally, the specified 'numOfUnits' for future
        // usage. The behavior is undefined unless the sum of units
        // submitted to the rate limiter and not consumed, numder of units to
        // reserve to reserve and the number of units reserved can
        // be represented by 64-bit unsigned integral type. Note that the time
        // interval between invocation of 'reserve' and 'submitReserved' or
        // 'cancelReserved' should be as short as possible, otherwise it would
        // make significant error in time interval calculation by
        // 'calculateTimeToSubmit'.

    void cancelReserved(bsls_Types::Uint64 numOfUnits);
        // Cancel the reservation of the specified 'numOfUnits' that were
        // previously reserved.  If more units are cancelled than reserved, set
        // the number of reserved units to 0.  The behavior is undefined unless
        // 'numOfUnits <= unitsReserved()'.

    void submitReserved(bsls_Types::Uint64 numOfUnits);
        // Submit the specified 'numOfUnits' that were previosly reserved.
        // The behavior is undefined unless the total number of units
        // submitted to the rate limiter and not consumed, and units to submit
        // can be represented by 64-bit unsigned integral type and
        // 'numOfUnits <= unitsReserved()'.

    bool wouldExceedBandwidth(const bdet_TimeInterval& currentTime);
        // Check whether submitting any more units at the specified
        // 'currentTime' would lead to exceeding the defined bandwidth limits.
        // Return false if submitting at one more unit to the rate limiter
        // will not lead to exceeding the defined bandwidth limits.
        // Otherwise, update state of the rate limiter, set 'timestamp' to
        // 'currentTime' and return false if submitting one more unit will
        // not lead to exceeding the bandwidth limits.  Return true otherwise.

    bdet_TimeInterval calculateTimeToSubmit(
                                         const bdet_TimeInterval& currentTime);
        // Return the estimated time interval, that should pass since the
        // specified 'currentTime', until it would be possible to submit one
        // more unit into this rate limiter, without exceeding the capacity.
        // Return an interval of length 0, if one more unit can be submitted
        // at the specifiled 'currentTime'. Otherwise, update the timestamp of
        // this rate limiter to 'currentTime' and return the time interval,
        // that should pass until one more unit can be submitted.
        // The number of nanoseconds in the time interval is rounded up.
        // Note that after waiting for the specified interval
        // 'wouldExceedBandwidth' check should be performed again.

    void reset(const bdet_TimeInterval& currentTime);
        // Reset the rate limiter to its default-constructed state and set the
        // 'timestamp' of this rate limiter to the specified 'currentTime'.
        // Note that this function should be used if the rate limiter was
        // created long before starting to use it.

    // ACCESSORS
    bsls_Types::Uint64 peakRateLimit() const;
        // Return the peak rate in units per second

    bsls_Types::Uint64 sustainedRateLimit() const;
        // Return sustained rate limit in units per second

    bdet_TimeInterval peakRateWindow() const;
        // Return the time-period over which the rate limiter *approximates*
        // computing a moving-total of submitted units for limiting peak rate.

    bdet_TimeInterval sustainedRateWindow() const;
        // Return the time-period over which the rate limiter *approximates*
        // computing a moving-total of submitted units for limiting sustained
        // rate.

    bsls_Types::Uint64 unitsReserved() const;
        // Return the number of units that are reserved.

    bdet_TimeInterval timestamp() const;
        // Return the timestamp of this object, as a time interval,
        // describing the moment in time the rate limiter was last updated.
        // The returned time interval uses the same reference point as the time
        // interval specified during construction or last invocation of the
        // 'reset' method.

    void getStatistics(bsls_Types::Uint64* submittedUnits,
                       bsls_Types::Uint64* unusedUnits) const;
        // Load, into the specified 'submittedUnits' and 'unusedUnits' the
        // numbers of submitted units and unused units respectively.
        // The number of submitter units is the total number of units submitted
        // since the time returned by the 'statisticsTimestamp' method till
        // the time returned by the 'timestamp' method.  The number of unused
        // units is the number of additional units that could potentially have
        // been submitted without exceeding the sustained rate since the time,
        // returned by the 'statisticsTimestamp' method till the time, returned
        // by the 'timestamp' method.

    bdet_TimeInterval statisticsTimestamp() const;
        // Return the statistics timestamp, as a time interval, indicating the
        // start time of the statistics collection for this rate limiter.
        // The returned time interval uses the same reference point as the time
        // interval specified during construction or last invocation of the
        // 'reset' method.

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
bdet_TimeInterval btes_RateLimiter::statisticsTimestamp() const
{
    return d_sustainedRateBucket.statisticsTimestamp();
}

inline
bdet_TimeInterval btes_RateLimiter::timestamp() const
{
    // The 'timestamp' of rate limiter is the 'timestamp' of the leaky bucket,
    // that was updated last.

    return bsl::max(d_sustainedRateBucket.timestamp(),
                    d_peakRateBucket.timestamp());
}

inline
bsls_Types::Uint64 btes_RateLimiter::unitsReserved() const
{
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
