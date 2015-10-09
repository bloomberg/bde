// btls_ratelimiter.h                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLS_RATELIMITER
#define INCLUDED_BTLS_RATELIMITER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism to limit peak and sustained consumption rates.
//
//@CLASSES:
//   btls::RateLimiter: mechanism to monitor resource consumption rates
//
//@SEE_ALSO: btls_leakybucket
//
//@DESCRIPTION: This component provides a mechanism, 'btls::RateLimiter', that
// enables clients to monitor and control the use of a resource such that the
// peak consumption rate and the sustained consumption rate do not exceed their
// respective configured limits.
//
// The limits on resource consumption rates of a 'btls::RateLimiter' object are
// configured using a specified peak rate (measured in 'units/s') along with
// its time-window, and a specified sustained rate (measured in 'units/s')
// along with its time-window.  The peak-rate time-window indicates a sliding
// time period over which the average consumption rate shall not exceed the
// peak-rate; similarly, the sustained-rate time-window indicates a sliding
// time period over which the average consumption rate shall not exceed the
// sustained rate.  'unit' is a generic unit of measurement (e.g., bytes,
// megabytes, number of messages, packets, liters, clock cycles, etc.).
//
///Internal Model
///--------------
// Internally, a rate limiter (currently) models resource usage using two
// corresponding 'btls::LeakyBucket' objects, one for limiting peak resource
// usage and one for limiting sustained resource usage.  Each leaky bucket
// provides an approximation for a moving total, where the configured time
// window corresponds to the period of the moving total, and that time window
// multiplied by the corresponding rate indicates the sum that the moving total
// may not exceed (i.e., the capacity of the leaky bucket).  As the units are
// submitted to a rate limiter, they are added to both the peak and sustained
// rate moving-totals, and then removed over time at the corresponding
// configured rate.
//
// Figure 1 illustrates the behavior of a rate limiter during a typical usage
// scenario using moving-totals:
//..
// Fig. 1:
//
//  Rp (peak rate)                  = 2 units/s
//  Wp (peak-rate time-window)      = 2 s
//  Rs (sustained rate)             = 1 units/s
//  Ws (sustained-rate time-window) = 7 s
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
//                                                      Submit 2
//
//      |   |     7|   |       |   |     7|   |       |   |     7|   |
//    12|   |     6|   |     12|   |     6|   |     12|   |     6|   |
//    11|   |     5|   |     11|   |     5|   |     11|   |     5|   |
//    10|   |  Lp-4|---|     10|   |  Lp-4|---|     10|   |  Lp-4|---|
//     9|   |     3|~~~|      9|   |     3|   |      9|   |     3|~~~|
//     8|   |     2|~~~|      8|   |     2|   |      8|   |     2|~~~|
//  Ls-7|~~~|     1|~~~|   Ls-7|---|     1|~~~|   Ls-7|~~~|     1|~~~|
//     6|~~~|      +- -+      6|---|      +- -+      6|~~~|      +- -+
//     5|~~~|                 5|~~~|                 5|~~~|
//     4|~~~|                 4|~~~|                 4|~~~|
//     3|~~~|                 3|~~~|                 3|~~~|
//     2|~~~|                 2|~~~|                 2|~~~|
//     1|~~~|                 1|~~~|                 1|~~~|
//      +- -+                  +- -+                  +- -+
//
// Time:    t0 + 4s               t0 + 6s                t0 + 6s
//..
// Suppose we have a rate limiter with a peak rate of 'Rp = 2 units/s', a
// peak-rate time-window of 'Wp = 2 s', a sustained rate of 'Rs = 1 units/s',
// and a sustained-rate time-window of 'Ws = 7 s'.
//
// This rate limiter maintains a moving-total having a capacity
// 'Lp = Rp * Wp = 4 units' that controls the peak rate and another
// moving-total having a capacity 'Ls = Rs * Ws = 7 units' that controls the
// sustained rate.
//
// Figure 1 shows the following sequence of events:
//: o At time 't0s', we submit 5 units.  The submitted units are added to the
//:   both moving-totals, and as a result the 'Lp' is exceeded, which means
//:   that the average consumption rate over the peak-rate time-window has
//:   exceeded the peak rate.  Note that we can not submit any more units at
//:   this time even though 'Ls' is not exceeded (the average consumption rate
//:   over the sustained-rate time-windows has not exceeded the sustained
//:   rate).
//:
//: o At time 't0 + 2s' the number of units contained moving-totals are
//:   recalculated.  As a result, 4 units ('Rp * 2 s') are subtracted from the
//:   peak rate moving-total, and 2 units ('Rs * 2 s') are subtracted from the
//:   sustained rate moving-total.  Now, capacities of both moving-totals are
//:   no longer exceeded, so we are free to submit more units.  We submit 7
//:   units, causing both 'Lp' and 'Ls' to be exceeded.
//:
//: o At time 't0 + 4s', the moving-totals are again updated.  The 'Lp' limit
//:   is no longer exceeded.  The number of units held by the moving-total
//:   tracking sustained rate matches the moving-total's capacity, and this
//:   boundary condition imply and no units can be submitted, because
//:   submitting any amount of units would cause 'Ls' to be exceeded.
//:
//: o At time 't0 + 6s', the moving-totals are again updated.  Both 'Lp' and
//:   'Ls' are no longer exceeded.  We submit 2 units.  The 'Lp' limit is not
//:   exceeded, but 'Ls' limit is exceeded.
//
///Monitoring Resource Usage
///-------------------------
// A 'btls::LeakyBucket' provides methods to both submit units and reserve
// units for future submission.  Submitting a unit indicates that it has been
// consumed by the entity being modeled, and it is added to the moving-totals
// tracking both peak and sustained resource usage.
//
// Reserving a unit guarantees that available capacity will be reserved so that
// unit can be submitted in the future without exceeding the configured limits.
// Reserved units may be later submitted using the 'submitReserved' method or
// canceled using the 'cancelReserved' method.  Reserved units permanently
// reside in the two moving-totals of consumed units, resulting in the
// reduction in the effective capacities of the moving-totals, until the
// reserved units are canceled or submitted.  Reserving units effectively
// shortens the time-window during which the average sustained and peak rate
// are enforced.  Therefore, the time interval between reserving units and
// submitting or canceling them should be kept as short as possible.  For a
// practical example of using reserved units, please see
// 'btls_reservationguard'.
//
// The recommended usage of a rate limiter is to first check whether 1 unit can
// be added without exceeding the rate limiter's configured limits, and if so,
// consume the desired amount of the resource.  Afterwards, submit the amount
// of consumed resource to the rate limiter.
//
// Whether submitting more units would exceed the configured limits can be
// determined using the 'wouldExceedBandwidth' method.  The estimated amount of
// time to wait before 1 more unit will be allowed to be submitted can be
// determined using the 'calculateTimeToSubmit' method.
//
///Time Synchronization
///--------------------
// rate limiter does not utilize an internal timer, so timing must be handled
// manually.  Clients can specify an initial time interval for a rate limiter
// object at construction or using the 'reset' method.  Whenever the state of a
// rate limiter object needs to be updated, clients must invoke the
// 'updateState' method specifying the current time interval.  Since rate
// limiter cares only about the elapsed time (not absolute time), the specified
// time intervals may be relative to any arbitrary time origin, though all of
// them must refer to the same origin.  For the sake of consistency, clients
// are encouraged to use the unix epoch time (such as the values returned by
// 'bdlt::CurrentTime::now').
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Controlling Network Traffic Generation
///-------------------------------------------------
// Suppose that we want to send data over a network interface with the load
// spike limitations explained below:
//
//: o The long term average rate of resource usage (i.e., the sustained rate)
//:   should not exceed 1024 bytes/s ('Rs').
//:
//: o The period over which to monitor the long term average rate (i.e., the
//:   sustained-rate time-window) should be 0.5s ('Wp').
//:
//: o The peak resource usage (i.e., the peak rate) should not exceed 2048
//:   bytes/s ('Rp').
//:
//: o The period over which to monitor the peak resource usage should be
//:   0.0625s (Wp).
//
// This is shown in Figure 2 below.
//..
// Fig. 2:
//
//     ^ Rate (Units per second)
//     |                             _____         .
//     |                            /  B  \        .
// 2048|---------------------------/-------\--------Rp (Maximum peak rate)
//     |           __             /         \      .
//     |          /  \           /    A2     \     .
//     |         / A1 \         /             \    .
// 1024|--------/------\ ------/---------------\----Rs (Maximum sustained rate)
//     |   __  /        \     /                 \__.
//     |__/  \/          \___/                     .
//     |                                           .
//      --------------------------------------------->
//                                         T (seconds)
//..
// Notice that we can understand the limitations imposed by the rate-limiter
// graphically as the maximum area above the respective lines, 'Rp' and 'Rs',
// that the usage curve to allowed to achieve.  In the example above:
//
//  o The area above the sustained rate 'Rs' (e.g., 'A1' or 'A2+B') should
//    contain no more than 512 bytes (Rs * Ws).
//
//  o The area above the peak rate 'Rp' should contain no more than 128 bytes
//    (Rp * Wp).
//
// Further suppose that we have a function, 'sendData', that transmits a
// specified amount of data over that network:
//..
//  bool sendData(size_t dataSize)
//      // Send a specified 'dataSize' amount of data over the network.  Return
//      // 'true' if data was sent successfully and 'false' otherwise.
//  {
//      (void)(dataSize);
//      // For simplicity, 'sendData' will not actually send any data and will
//      // always return 'true'.
//      return true;
//  }
//..
// First, we create a 'btls::RateLimiter' object having a sustained rate of
// 1024 bytes/s, a sustained-rate time-window of 0.5s (512 bytes / 1024
// bytes/s), a peak-rate of 2048 bytes/s, and a peak-rate time-window of
// 0.0625s (128 bytes / 2048 bytes/s):
//..
//  bsls::Types::Uint64 sustainedRateLimit = 1024;
//  bsls::TimeInterval  sustainedRateWindow(0.5);
//  bsls::Types::Uint64 peakRateLimit = 2048;
//  bsls::TimeInterval  peakRateWindow(0.0625);
//  bsls::TimeInterval  now = bdlt::CurrentTime::now();
//
//  btls::RateLimiter  rateLimiter(sustainedRateLimit,
//                                 sustainedRateWindow,
//                                 peakRateLimit,
//                                 peakRateWindow,
//                                 now);
//..
// Note that the rate limiter does not prevent the rate at any instant from
// exceeding either the peak-rate or the sustained rate; instead, it prevents
// the average rate over the peak-rate time-window from exceeding maximum
// peak-rate and the average rate over the sustained-rate time-window from
// exceeding the maximum sustained-rate.
//
// Then, we define the size of data to be send, the size of each data chunk,
// and a counter of data actually sent:
//..
//  bsls::Types::Uint64 sizeOfData = 10 * 1024; // in bytes
//  bsls::Types::Uint64 chunkSize  = 64;        // in bytes
//  bsls::Types::Uint64 bytesSent  = 0;
//..
// Now, we send the chunks of data using a loop.  For each iteration, we check
// whether submitting another byte would exceed the rate limiter's bandwidth
// limits.  If not, we send an additional chunk of data and submit the number
// of bytes sent to the leaky bucket.  Note that 'submit' is invoked only after
// the data has been sent.
//..
//  while (bytesSent < sizeOfData) {
//      now = bdlt::CurrentTime::now();
//      if (!rateLimiter.wouldExceedBandwidth(now)) {
//          if (true == sendData(chunkSize)) {
//              rateLimiter.submit(chunkSize);
//              bytesSent += chunkSize;
//          }
//      }
//..
// Finally, if submitting another byte will cause the rate limiter to exceed
// its bandwidth limits, then we wait until the submission will be allowed by
// waiting for an amount time returned by the 'calculateTimeToSubmit' method:
//..
//      else {
//          bsls::TimeInterval timeToSubmit =
//                                      rateLimiter.calculateTimeToSubmit(now);
//          bsls::Types::Uint64 uS = timeToSubmit.totalMicroseconds() +
//                                 (timeToSubmit.nanoseconds() % 1000) ? 1 : 0;
//          bslmt::ThreadUtil::microSleep(static_cast<int>(uS));
//      }
//  }
//..
// Notice that we wait by putting the thread into a sleep state instead of
// using busy-waiting to better optimize for multi-threaded applications.

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLS_LEAKYBUCKET
#include <btls_leakybucket.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_C_LIMITS
#include <bsl_c_limits.h>
#endif

namespace BloombergLP {
namespace btls {

                            // =================
                            // class RateLimiter
                            // =================

class RateLimiter {
    // This mechanism implements a rate limiter that allows clients to monitor
    // and control the usage of a resource such that the rate of consumption
    // stays within configured limits.  The behavior of a rate limiter is
    // determined by four properties: the sustained rate (in units/s), the
    // sustained-rate time-window (in seconds), the peak rate (in units/s), and
    // the peak-rate time-window (in seconds).  All of these properties can be
    // specified at construction or using the 'setRateLimits' method.
    //
    // Units can be indicated to a rate limiter as consumed by either
    // submitting them using the 'submit' method.  Units can be marked as
    // reserved, which effectively shorten the sustained-rate time-window and
    // the peak-rate time-window, by using the 'reserve' method.
    //
    // Whether submitting 1 more unit would exceed the configured limits can be
    // determined using the 'wouldExceedBandwidth' method.  The estimated
    // amount of time to wait before 1 more unit will be allowed to be
    // submitted can be determined using the 'calculateTimeToSubmit' method.
    //
    // The state of a rate limiter must be updated manually using the
    // 'updateState' method supplying the current time interval.  The time
    // intervals supplied should all refer to the same time origin.
    //
    // A rate limiter keeps some statistics, including the number of submitted
    // units, that can be accessed using the 'getStatistics' and reset using
    // the 'resetStatistics' method.
    //
    // This class:
    //: o is *exception* *neutral* (agnostic)
    //: o is *const* *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    LeakyBucket d_peakRateBucket;       // 'LeakyBucket' object for handling
                                        // peak load

    LeakyBucket d_sustainedRateBucket;  // 'LeakyBucket' object for handling
                                        // sustained load

  private:
    // NOT IMPLEMENTED
    RateLimiter& operator=(const RateLimiter&);
    RateLimiter(const RateLimiter&);

  public:
    // CREATORS
    RateLimiter(bsls::Types::Uint64       sustainedRateLimit,
                const bsls::TimeInterval& sustainedRateWindow,
                bsls::Types::Uint64       peakRateLimit,
                const bsls::TimeInterval& peakRateWindow,
                const bsls::TimeInterval& currentTime);
        // Create a RateLimiter object, having the specified
        // 'sustainedRateLimit', the specified 'sustainedRateWindow', the
        // specified 'peakRateLimit', the specified 'peakRateWindow', and using
        // the specified 'currentTime' as the initial 'lastUpdateTime'.  The
        // behavior is undefined unless '0 < sustainedRateLimit',
        // '0 < sustainedRateWindow', '0 < peakRateLimit',
        // '0 < peakRateWindow', the product of 'sustainedRateLimit' and
        // 'sustainedRateWindow' can be represented by 64-bit unsigned integral
        // type, and the product of 'peakRateLimit' and 'peakRateWindow' can be
        // represented by 64-bit unsigned integral type.

    ~RateLimiter();
        // Destroy this object.

    // MANIPULATORS
    void updateState(const bsls::TimeInterval& currentTime);
        // Set the 'lastUpdateTime' of this rate limiter to the specified
        // 'currentTime'.  If 'currentTime' is after 'lastUpdateTime', then
        // recalculate number of units available for consumption based on the
        // 'peakRate', 'sustainedRate' and the time interval between
        // 'lastUpdateTime' and 'currentTime'.  If 'currentTime' is before
        // 'statisticsCollectionStartTime', set it' to 'currentTime'.

    void resetStatistics();
        // Reset the statics collected for this rate limiter by setting the
        // number of units used and the number of units submitted to 0, and set
        // the 'statisticsCollectionStartTime' to the 'lastUpdateTime' of this
        // leaky bucket.

    void setRateLimits(bsls::Types::Uint64       sustainedRateLimit,
                       const bsls::TimeInterval& sustainedRateWindow,
                       bsls::Types::Uint64       peakRateLimit,
                       const bsls::TimeInterval& peakRateWindow);
        // Set the sustained rate of this rate limiter to the specified
        // 'sustainedRateLimit', the sustained-rate time-window to the
        // specified 'sustainedRateWindow', the peak rate to the specified
        // 'peakRateLimit' and the peak-rate time-window to the specified
        // 'peakRateWindow'.  The behavior is undefined unless
        // '0 < sustainedRateLimit', '0 < sustainedRateWindow',
        // '0 < peakRateLimit', '0 < peakRateWindow', the product of
        // 'sustainedRateLimit' and 'sustainedRateWindow' can be represented by
        // 64-bit unsigned integral type, and the product of 'peakRateLimit'
        // and 'peakRateWindow' can be represented by 64-bit unsigned integral
        // type.

    void submit(bsls::Types::Uint64 numUnits);
        // Submit the specified 'numUnits' to this rate limiter.  The behavior
        // is undefined unless the sum of 'numUnits', unused units previously
        // submitted to this rate limiter, and 'unitsReserved' can be
        // represented by a 64-bit unsigned integral type.

    void reserve(bsls::Types::Uint64 numUnits);
        // Reserve the specified 'numUnits' for future use by this rate
        // limiter.  The behavior is undefined unless the sum of 'numUnits',
        // unused units previously submitted to this rate limiter, and
        // 'unitsReserved' can be represented by a 64-bit unsigned integral
        // type.

    void cancelReserved(bsls::Types::Uint64 numUnits);
        // Cancel the specified 'numUnits' that were previously reserved.  The
        // behavior is undefined unless 'numUnits <= unitsReserved()'.

    void submitReserved(bsls::Types::Uint64 numUnits);
        // Submit the specified 'numUnits' that were previously reserved.  The
        // behavior is undefined unless 'numUnits <= unitsReserved()'.

    bool wouldExceedBandwidth(const bsls::TimeInterval& currentTime);
        // Update the state of this rate limiter to the specified
        // 'currentTime'.  Return 'true' if submitting 1 unit at the
        // 'currentTime' would exceed the configured limits, and false
        // otherwise.

    bsls::TimeInterval calculateTimeToSubmit(
                                        const bsls::TimeInterval& currentTime);
        // Update the state of this rate limiter to the specified
        // 'currentTime'.  Return the estimated time interval that should pass
        // from 'currentTime' until 1 more unit can be submitted to this rate
        // limiter without exceeding its configured limits.  The number of
        // nanoseconds in the returned time interval is rounded up.  Note that
        // a time interval of 0 is returned if 1 or more units can be submitted
        // at 'currentTime'.  Also note that after waiting for the returned
        // time interval, clients should typically check again using this
        // method, because additional units may have been submitted in the
        // interim.

    void reset(const bsls::TimeInterval& currentTime);
        // Reset the statistics counter for this rate limiter to 0, and set the
        // 'lastUpdateTime' of this rate limiter to the specified
        // 'currentTime'.

    // ACCESSORS
    bsls::Types::Uint64 peakRateLimit() const;
        // Return the peak rate of this rate limiter.

    bsls::TimeInterval peakRateWindow() const;
        // Return the peak-rate time-period of this rate limiter.  Note that
        // this period is generally significantly shorter than
        // 'sustainedRateWindow'.

    bsls::Types::Uint64 sustainedRateLimit() const;
        // Return the sustained rate of this rate limiter.

    bsls::TimeInterval sustainedRateWindow() const;
        // Return the sustained-rate time-period of this rate limiter.  Note
        // that this period is generally significantly longer than the
        // 'peakRateWindow'.

    bsls::Types::Uint64 unitsReserved() const;
        // Return the number of reserved units for this rate limiter.

    bsls::TimeInterval lastUpdateTime() const;
        // Return the time when this rate limiter was last updated.

    void getStatistics(bsls::Types::Uint64* submittedUnits,
                       bsls::Types::Uint64* unusedUnits) const;
        // Load, into the specified 'submittedUnits' and the specified
        // 'unusedUnits' respectively, the numbers of submitted units and the
        // number of unused units for this rate limiter from the
        // 'statisticsCollectionStartTime' to the 'lastUpdateTime'.  The number
        // of unused units is the difference between the number of units that
        // could have been consumed at the sustained rate and the number of
        // units actually submitted for the time period.

    bsls::TimeInterval statisticsCollectionStartTime() const;
        // Return the time interval when the collection of the statistics (as
        // returned by 'getStatistics') started.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                            // -----------------
                            // class RateLimiter
                            // -----------------

// MANIPULATORS
inline
void RateLimiter::updateState(const bsls::TimeInterval& currentTime)
{
    d_peakRateBucket.updateState(currentTime);
    d_sustainedRateBucket.updateState(currentTime);
}

inline
void RateLimiter::submit(bsls::Types::Uint64 numUnits)
{
    BSLS_ASSERT_SAFE(numUnits <=
                           ULLONG_MAX - d_sustainedRateBucket.unitsInBucket());

    BSLS_ASSERT_SAFE(unitsReserved() <=
                 ULLONG_MAX - d_sustainedRateBucket.unitsInBucket()- numUnits);

    BSLS_ASSERT_SAFE(numUnits <=
                                ULLONG_MAX - d_peakRateBucket.unitsInBucket());

    BSLS_ASSERT_SAFE(unitsReserved() <=
                      ULLONG_MAX - d_peakRateBucket.unitsInBucket()- numUnits);

    d_peakRateBucket.submit(numUnits);
    d_sustainedRateBucket.submit(numUnits);
}

inline
void RateLimiter::reserve(bsls::Types::Uint64 numUnits)
{
    BSLS_ASSERT_SAFE(numUnits <= ULLONG_MAX - unitsReserved());

    BSLS_ASSERT_SAFE(d_sustainedRateBucket.unitsInBucket() <=
                                       ULLONG_MAX - unitsReserved()- numUnits);

    BSLS_ASSERT_SAFE(d_peakRateBucket.unitsInBucket() <=
                                       ULLONG_MAX - unitsReserved()- numUnits);

    d_peakRateBucket.reserve(numUnits);
    d_sustainedRateBucket.reserve(numUnits);
}

inline
void RateLimiter::submitReserved(bsls::Types::Uint64 numUnits)
{
    BSLS_ASSERT(numUnits <= unitsReserved());

    // There is no need to check whether 'numUnits' causes overflow because the
    // reserved units was already checked by the 'reserve' method.

    d_peakRateBucket.submitReserved(numUnits);
    d_sustainedRateBucket.submitReserved(numUnits);
}

inline
void RateLimiter::cancelReserved(bsls::Types::Uint64 numUnits)
{
    BSLS_ASSERT(numUnits <= unitsReserved());

    d_peakRateBucket.cancelReserved(numUnits);
    d_sustainedRateBucket.cancelReserved(numUnits);
}

inline
bool RateLimiter::wouldExceedBandwidth(const bsls::TimeInterval& currentTime)
{
    return (d_peakRateBucket.wouldOverflow(currentTime) ||
                             d_sustainedRateBucket.wouldOverflow(currentTime));
}

inline
void RateLimiter::resetStatistics()
{
    d_sustainedRateBucket.resetStatistics();
}

inline
void RateLimiter::reset(const bsls::TimeInterval& currentTime)
{
    d_peakRateBucket.reset(currentTime);
    d_sustainedRateBucket.reset(currentTime);
}

// ACCESSORS
inline
void RateLimiter::getStatistics(bsls::Types::Uint64* submittedUnits,
                                bsls::Types::Uint64* unusedUnits) const
{
    BSLS_ASSERT_SAFE(0 != submittedUnits);
    BSLS_ASSERT_SAFE(0 != unusedUnits);

    // The statistics is collected from the sustained rate leaky bucket.

    d_sustainedRateBucket.getStatistics(submittedUnits, unusedUnits);
}

inline
bsls::Types::Uint64 RateLimiter::peakRateLimit() const
{
    return d_peakRateBucket.drainRate();
}

inline
bsls::Types::Uint64 RateLimiter::sustainedRateLimit() const
{
    return d_sustainedRateBucket.drainRate();
}

inline
bsls::TimeInterval RateLimiter::peakRateWindow() const
{
    return LeakyBucket::calculateTimeWindow(d_peakRateBucket.drainRate(),
                                                 d_peakRateBucket.capacity());
}

inline
bsls::TimeInterval RateLimiter::sustainedRateWindow() const
{
    return LeakyBucket::calculateTimeWindow(
                                             d_sustainedRateBucket.drainRate(),
                                             d_sustainedRateBucket.capacity());
}

inline
bsls::TimeInterval RateLimiter::statisticsCollectionStartTime() const
{
    return d_sustainedRateBucket.statisticsCollectionStartTime();
}

inline
bsls::TimeInterval RateLimiter::lastUpdateTime() const
{
    return bsl::max(d_sustainedRateBucket.lastUpdateTime(),
                    d_peakRateBucket.lastUpdateTime());
}

inline
bsls::Types::Uint64 RateLimiter::unitsReserved() const
{
    BSLS_ASSERT_SAFE(d_sustainedRateBucket.unitsReserved() ==
                                             d_peakRateBucket.unitsReserved());

    return d_sustainedRateBucket.unitsReserved();
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
