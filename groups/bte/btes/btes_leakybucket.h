// btes_leakybucket.h                                                 -*-C++-*-
#ifndef INCLUDED_BTES_LEAKYBUCKET
#define INCLUDED_BTES_LEAKYBUCKET

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Control the consumption rate of a resource.
//
//@CLASSES:
//   btes_LeakyBucket: a leaky bucket rate controller
//
//@AUTHOR: Anvar Murtazin (amurtazin), Mikhail Kunitskiy (mkunitskiy1)
//
//@SEE_ALSO: btes_ratelimiter
//
//@DESCRIPTION This component provides a mechanism, 'btes_LeakyBucket', to
// monitor whether the *capacity* of a resource is consumed at a particular
// *drain* *rate*
//
// This name for this component comes from an analogy of a bucket with
// a hole: the maximum rate at which the bucket will let water out is a
// constant, derived from the size of the hole, and does not depend on the rate
// at which water is poured into the bucket. If more water is continually
// poured into the bucket than is drained, eventually the bucket will overflow.
//
// The drain rate, in the context of this component, is the theoretical rate at
// which units are consumed (drained) from the bucket by its associated
// resource, and it is expressed in 'units/s'. 'unit' is a generic unit of
// measurement (e.g., bytes, n.o. messages, packets, liters, clock cycles,
// etc.).
//
// The capacity, in the context of this component, is the maximum number of
// units that a 'btes_LeakyBucket' can hold before it "overflows", meaning a
// call 'wouldOverflow' would return 'true' (note that a clients may submit
// units to a leaky bucket past its capacity). The capacity also determines
// the minimum amount of time required to empty a leaky-bucket at its
// configured drain rate.
//
// For example, suppose we have an empty bucket with a drain rate 'd = 5 u/s'
// and capacity 'c = 5 u'.  As illustrated in FIG. 1,  at time 't0' we submit 5
// units to the bucket.  At time 't0 + 4s' the bucket drained 4 units. Then, at
// time 't0 + 4s' we add 2 more. Note that if we added again 5 units we would
// overflow the capacity of the bucket. At time 't0 + 10' we are sure that the
// bucket is empty (because it was not overflown before), so we can add again
// 5 units without exceeding capacity.
//..
// FIG. 1:  Capacity = 5 units, Rate = 1 unit / second
//
//    Submit 5                     Submit 2
//
//    7|     |      7|     |       7|     |      7|     |
//    6|     |      6|     |       6|     |      6|     |
// c--5|~~~~~|   c--5|-----|    c--5|-----|   c--5|-----|
//    4|~~~~~|      4|     |       4|     |      4|     |
//    3|~~~~~|      3|     |       3|~~~~~|      3|     |
//    2|~~~~~|      2|     |       2|~~~~~|      2|     |
//    1|~~~~~|      1|~~~~~|       1|~~~~~|      1|     |
//     +-- --+       +-- --+        +-- --+       +-- --+
//
// Time: t0          t0 + 4         t0 + 4        t0 + 10
//..
// FIG. 2 illustrates the case in which at time 't0 + 4' we add 6 more units,
// exceeding the capacity of the bucket.  At time 't0 + 10' we can see that 6
// units have been drained, as we do not add any more units.
//..
// FIG. 2: Capacity = 5 units, Rate = 1 unit / second
//
//    Submit 5                     Submit 6
//
//    7|     |      7|     |       7|~~~~~|      7|     |
//    6|     |      6|     |       6|~~~~~|      6|     |
// c--5|~~~~~|   c--5|-----|    c--5|~~~~~|   c--5|-----|
//    4|~~~~~|      4|     |       4|~~~~~|      4|     |
//    3|~~~~~|      3|     |       3|~~~~~|      3|     |
//    2|~~~~~|      2|     |       2|~~~~~|      2|     |
//    1|~~~~~|      1|~~~~~|       1|~~~~~|      1|~~~~~|
//     +-- --+       +-- --+        +-- --+       +-- --+
//
// Time: t0          t0 + 4         t0 + 4        t0 + 10
//..
// Note that units are added to a 'btes_LeakyBucket' object by invoking the
// 'submit' method, and should be added only after the associated resource
// is actually used. At any point, the leaky bucket can be queried whether
// submitting a specified number of units would exceed the capacity of the
// leaky bucket, via the 'wouldOverflow' method.
//
// This component alsoprovides the capability of *reserving* units.
// The reserved units are excluded from the bucket capacity and are not 
// drained when the bucket state is updated.  Reserved units may be later 
// submitted to the leaky bucket or the reservation may be canceled.
// When reserved units are either submitted or canceled (using 'submitReserved'
// or 'cancelReserved', respectively), those units are no longer excluded from
// the capacity, and, if the reserved units were submitted, the units are
// added to the number of used units, which are drained from the bucket over
// time.
//
// In the example of FIG. 3 at time 't0' we reserve 4 units.  At time 
// 't0 + 5' none the reserved units are drained from the leaky bucket,
// then, at time 't0 + 6' we submit 3 of those units which will be drained
// from the bucket over time. At t0 + 9 we observe that all but the remaining
// reserved unit have been drained from the bucket. Finally, at t0 + 10, the
// reservation for the remaining unit is canceled.
//..
// FIG. 3: Capacity = 5 units, Rate = 1 unit / second
//
//    Reserve 3                 Submit 3                  Cancel 1
//                            from reserve              from reserve
//
//    7|     |     7|     |     7|     |     7|     |     7|     |
//    6|     |     6|     |     6|     |     6|     |     6|     |
// c--5|-----|  c--5|-----|  c--5|-----|  c--5|-----|  c--5|-----|
//    4|#####|     4|#####|     4|~~~~~|     4|     |     4|     |
//    3|#####|     3|#####|     3|~~~~~|     3|     |     3|     |
//    2|#####|     2|#####|     2|~~~~~|     2|     |     2|     |
//    1|#####|     1|#####|     1|#####|     1|#####|     1|     |
//     +-- --+      +-- --+      +-- --+      +-- --+      +-- --+
//
// Time: t0         t0 + 5       t0 + 6       t0 + 9       t0 + 10
//..
///Modeling a Network Connection
///-----------------------------
// One of the intended use cases of the 'btes_LeakyBucket' is limiting the rate
// at which data is written on a network. In this case, the drain rate of the
// bucket is corresponding to the *ideal* maximum rate that the client wishes
// to *enforce* on their outgoing connection.
// Clients may choose to provide a value related to the physical limitations of
// their network, or any other arbitrary limit.  The capacity of the bucket
// does map directly to the analogy of a leaky-bucket as 'btes_leakybucket'
// does not actually manage the actual resource being modeled. Instead, the
// capacity of the bucket relates to the size of the time period over which
// the actual rate will not exceed the configured drain rate of the bucket
// (see 'approximations' section below).
//
///Approximations
///--------------
// The 'btes_LeakyBucket' provides an analogy to a leaky bucket, but as a 
// 'btes_leakybucket' does not manage any resources, there are several 
// approximations in this model:
//  1 Units are submitted istantaneously to the leaky bucket, whereas the
//    resource consumes them over time, depending on the nature and speed
//    of the resource, at an unspecified rate.
//  2 The model simulates the drain of units from the buckets by removing units
//    at the specified drain rate, even though the resource effectively
//    consumes units at different rate. This guarantees that a resource's usage
//    does not exceed the configured drain-rate when amortized over some
//    configured period of time (determined by the capacity of the bucket),
//    but does not prevent resource usage from spiking above the configured
//    drain rate for smaller periods.
//  3 Draining units at the drain rate allows for the approximation of a
//    *sliding* *window*: as enough units are drained one can imagine the
//    window sliding to a point in time that excludes previously submitted
//    units.  The size of the window can be derived from the capacity of the
//    bucket and the drain rate.  The 'calculateTimeWindow' class method
//    convienently performs that calculation.
//
///Time Synchronization
///--------------------
// This component does not provide internal timers.  Timing must be handled
// by the client. The calculation of the current number of units in the leaky
// bucket is based on the lastUpdateTimes provided by the client.
// An initial lastUpdateTime is specified at creation or when the 'reset' method is
// invoked, and subsequent times are interpreted using the difference from this
// initial time point. Since 'btes_LeakyBucket' cares only about the difference
// between a provided time, and the initial time, the supplied
// 'bdet_TimeInverval' values may be relative to any arbitrary time origin,
// though clients are encouraged to use the UNIX epoch time (such as values
// returned by 'bdetu_systemtime'). To ensure consistency, all time intervals
// should refer to the same time origin.
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Controlling Network Traffic Generation
///-------------------------------------------------
// In some systems data is processed faster than it is consumed by I/O
// interfaces.  This circumstance could lead to data loss due to the overflow
// of the buffers where the data is queued before being processed.  On other
// systems, generic resources are shared, and their counsumption might need to
// be managed in order to guarantee quality-of-service QOS.
//
// Imagine the case of a network interface able to transfer only 1024 byte/s
// and assume that an application wants to transmit 1Mb of data over that
// network in 20 different 256-bytes data chunks.  Given the low speed of the
// network, we want to ensure that the communication uses, on average, less
// than 50% of the available bandwidth, or 512 byte/s.  In this way other
// clients can comfortably transmit and receive data on the same interface.
//
// Also, assume that we have a function 'sendData', that transmits data
// contained in a buffer over that network interface:
//..
//  bool sendData(const unsigned char *buffer, size_t dataSize);
//      // Send the specified 'dataSize' through the network interface, and
//      // return true if data was sent successfully, and false otherwise.
//..
// First, we create a 'btes_LeakyBucket' object having a drain rate of 512
// byte/s, a capacity of 2560 bytes, and the time origin set to the current
// time (as an interval from UNIX 'epoch'):
//..
//  bsls_Types::Uint64 rate     = 512;  // bytes/second
//  bsls_Types::Uint64 capacity = 2560; // bytes
//  bdet_TimeInterval  now      = bdetu_SystemTime::now();
//
//  btes_LeakyBucket   bucket(rate, capacity, now);
//
//..
// Note that, to ensure consistency, all the time intervals further specified,
// will have the same time origin as 'now'.
//
// Next, we define the size of each data chunk, and the total size of the data
// to transmit:
//..
//  unsigned int       chunkSize  = 256;             // in bytes
//  bsls_Types::Uint64 totalSize  = 20 * chunkSize;  // in bytes
//  bsls_Types::Uint64 dataSent   = 0;               // in bytes
//..
// Now, we build a loop and for each iteration we check whether submitting
// another chunk of data to the bucket would cause overflow.  If not, we can
// send the data and submit it to the bucket.  The loop terminates when all the
// data is sent.  Note that 'submit' is invoked only after a successful
// operation on the resource.
//..
//  while (dataSent < totalSize) {
//      now = bdetu_SystemTime::now();
//      if (!bucket.wouldOverflow(1, now)) {
//          if (true == sendData(256)) {
//              bucket.submit(256);
//              dataSent += 256;
//          }
//      }
//..
// Finally, if it is not possible to submit a new chunk of data without
// overflowing the bucket, we invoke the 'calculateTimeToSubmit' method to
// determine how much time is required to submit a new chunk without causing
// overflow. We round up the number of microseconds in time interval.
//..
//      else {
//          bdet_TimeInterval timeToSubmit = bucket.calculateTimeToSubmit(now);
//          bsls_Types::Uint64 uS = timeToSubmit.totalMicroseconds() +
//                                 (timeToSubmit.nanoseconds() % 1000) ? 1 : 0;
//          bcemt_ThreadUtil::microSleep(uS);
//      }
//  }
//..
// Notice that in a multi-threaded application it is appropriate to put the
// thread into the 'sleep' state, in order to avoid busy-waiting.

#ifndef INCLUDED_BDETU_SYSTEMTIME
#include <bdetu_systemtime.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

                        //=======================
                        // class btes_LeakyBucket
                        //=======================

class btes_LeakyBucket {
    // This mechanism is an implementation of the leaky bucket algorithm.
    // The class invariants are:
    // 'capacity() > 0',
    // 'drainRate() > 0'.
    // This class:
    //: o is *exception* *neutral* (agnostic)
    //: o is *const* *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    bsls_Types::Uint64 d_drainRate;            // drain rate in units per
                                               // second

    bsls_Types::Uint64 d_capacity;             // the bucket capacity in
                                               // units

    bsls_Types::Uint64 d_unitsReserved;        // reserved units

    bsls_Types::Uint64 d_unitsInBucket;        // number of units currently in
                                               // the bucket

    bsls_Types::Uint64 d_fractionalUnitDrainednNanoUnits;
                                               // fractional number of units
                                               // that is carried from the
                                               // last drain operation

    bdet_TimeInterval  d_lastUpdateTime;       // time of last drain,
                                               // represented by time interval
                                               // relative to arbitrary
                                               // reference point

    bdet_TimeInterval d_maxUpdateInterval;     // maximum interval between
                                               // updates.

    bsls_Types::Uint64 d_statSubmittedUnits;   // counter of units,
                                               // submitted since last
                                               // reset

    bsls_Types::Uint64 d_statSubmittedUnitsAtLastUpdate; 
                                               // value of submitted unit
                                               // counter saved during
                                               // last 'lastUpdateTime'
                                               // call

    bdet_TimeInterval  d_statisticsCollectionStartTime;
                                               // reference point for
                                               // statistics counter of
                                               // submitted units, as time
                                               // interval, relative to
                                               // arbitrary reference point

private:
    // NOT IMPLEMENTED
    btes_LeakyBucket& operator=(const btes_LeakyBucket&);
    btes_LeakyBucket(const btes_LeakyBucket&);

public:

    // CLASS METHODS

    static bdet_TimeInterval calculateDrainTime(bsls_Types::Uint64 numOfUnits,
                                                bsls_Types::Uint64 drainRate,
                                                bool               ceilFlag);
        // Return the time interval that is required to drain the specified
        // 'numOfUnits' at the specified 'drainRate', round up the number
        // of nanoseconds in the time interval if the specified 'ceilFlag' is
        // set to 'true', otherwise, round down the number of nanoseconds.
        // The behavior is undefined unless 'drainRate > 0' and the number of
        // seconds in the calculated interval may be represented by a 64-bit
        // integral type.

    static bdet_TimeInterval calculateTimeWindow(bsls_Types::Uint64 drainRate,
                                                 bsls_Types::Uint64 capacity);
        // Return the time interval over which a leaky bucket
        // *approximates* a moving-total of submitted units, as the
        // rounded-down ratio between the specified 'capacity' and the
        // specified 'drainRate'.  If the rounded ratio is 0, return a time
        // interval of 1 nanosecond.  The behavior is undefined unless
        // 'capacity > 0', 'drainRate > 0' and 'capacity / drainRate' can be
        // represented with 64-bit signed integer type.

    static bsls_Types::Uint64 calculateCapacity(
                                          bsls_Types::Uint64       drainRate,
                                          const bdet_TimeInterval& timeWindow);
        // Return the capacity of a leaky bucket as the rounded-down product
        // of the specified 'drainRate' by the specified 'timeWindow'.  If the
        // result evaluates to 0, return 1. The behavior is undefined unless
        // 'drainRate > 0', 'timeWindow > 0' and 'drainRate' and 'timeWindow'
        // product can be represented by a 64-bit unsigned integral type.

    // CREATORS
    btes_LeakyBucket();
        // Create a 'btes_LeakyBucket' object having a drain rate of 1 unit per
        // second, and a capacity of 1 unit, using zero as the reference point
        // for time intervals and statistics calculation and such that
        // 'unitsInBucket() == 0'.

    btes_LeakyBucket(bsls_Types::Uint64       drainRate,
                     bsls_Types::Uint64       capacity,
                     const bdet_TimeInterval& currentTime);
        // Create a 'btes_LeakyBucket' object having the specified 'drainRate'
        // in units per second and the specified 'capacity' in units,
        // using specified 'currentTime' as the *initial* lastUpdateTime, and such
        // that 'unitsInBucket() == 0'.  The behaviour is undefined unless
        // 'drainRate > 0' and 'capacity > 0'.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)

    // The following destructor is generated by the compiler, except in "SAFE"
    // build modes (e.g., to enable the checking of class invariants).

    ~btes_LeakyBucket();
        // Destroy this object.

#endif

    // MANIPULATORS
    void updateState(const bdet_TimeInterval& currentTime);
        // Set the 'lastUpdateTime' of this leaky bucket to the specified
        // 'currentTime'.  If the specified 'currentTime' is after
        // 'lastUpdateTime', then update 'unitsInBucket' based on the number of
        // drained units between 'lastUpdateTime' and 'currentTime'.
        // If 'currentTime' is before the value, returned by 
        // 'statisticsCollectionStartTime' method, set the
        // 'statisticsCollectionStartTime' to 'currentTime'.

    void resetStatistics();
        // Reset the statics for the number of units used and the number of
        // units submitted to 0, and set the 'statisticsCollectionStartTime'
        // to the current 'lastUpdateTimeStamp'.

    void setRateAndCapacity(bsls_Types::Uint64 newRate,
                            bsls_Types::Uint64 newCapacity);
        // Set the draining rate of this leaky bucket to the specified
        // 'newRate'(in units per second) and the capacity to the specified
        // 'newCapacity'.  The behavior is undefined unless 'newRate > 0' and
        // 'newCapacity > 0'.

    void submit(bsls_Types::Uint64 numOfUnits);
        // Add, unconditionally, the specified 'numOfUnits' to this leaky
        // bucket.  The behavior is undefined unless the value
        // 'unitsReserved() + unitsInBucket() + numOfUnits' can be
        // represented by 64-bit integral type.  Note that this operation
        // might overflow the 'capacity' of the bucket.

     void reserve(bsls_Types::Uint64 numOfUnits);
        // Reserve, uncoditionally, the specified 'numOfUnits' for future
        // usage.  The behavior is undefined unless the value
        // 'unitsReserved() + unitsInBucket() + numOfUnits' can be represented
        // by 64-bit integral type.  Note that after this operation number of
        // reserved units may exceed the capacity of the bucket.  Also note
        // that the time interval between the invocations of 'reserve' and
        // 'submitReserved' or 'cancelReserved' should be as short as possible,
        // otherwise it would affect precision of calculating time interval by
        // 'calculateTimeToSubmit'.

    void cancelReserved(bsls_Types::Uint64 numOfUnits);
        // Cancel the reservation of the specified 'numOfUnits' that were
        // previously reserved.  The behavior is undefined unless
        // 'numOfUnits <= unitsReserved()'.

    void submitReserved(bsls_Types::Uint64 numOfUnits);
        // Submit the specified 'numOfUnits' that were previosly reserved.
        // The behavior is undefined unless 'numOfUnits <= unitsReserved()'.

    bool wouldOverflow(bsls_Types::Uint64       numOfUnits,
                       const bdet_TimeInterval& currentTime);
        // Update 'unitsInBucket' based on the number of drained units between
        // 'lastUpdateTime' and 'currentTime', and then set 'lastUpdateTime' to
        // the specified 'currentTime' and return 'true' if adding the
        // specified 'numOfUnits' into this leaky bucket at the specified
        // 'currentTime' would exceed its capacity, and return 'false'
        // otherwise.  The behavior is undefined unless 'numOfUnits > 0'.

    bdet_TimeInterval calculateTimeToSubmit(
                                         const bdet_TimeInterval& currentTime);
        // Return the estimated time interval, that should pass since the
        // specified 'currentTime', until it would be possible to submit one
        // more unit into this leaky bucket, without exceeding the capacity.
        // Return an interval of length 0, if one more unit can be submitted
        // at the specifiled 'currentTime'. Otherwise, update the lastUpdateTime of
        // this leaky bucket to 'currentTime' and return the time interval,
        // that should pass until one more unit can be submitted.
        // The number of nanoseconds in the time interval is rounded up.
        // Note that after waiting for the returned interval, a client should
        // typically check 'wouldOverlow' before submitting units, as
        // additional units may have been submitted in the interim.

    void reset(const bdet_TimeInterval& currentTime);
        // Reset the 'unitsInBucket', 'unitsReserved', as well as the
        // 'submittedUnits' and 'unusedUnits' statistics to 0, and set
        // 'lastUpdateTime' and 'statisticCollectionStartTime' to
        // 'currentTime'.

    // ACCESSORS
    bsls_Types::Uint64 drainRate() const;
        // Return the drain rate in units per second.

    bsls_Types::Uint64 capacity() const;
        // Return the size of this leaky bucket in units.

    bsls_Types::Uint64 unitsInBucket() const;
        // Return the number of units that are currently in the bucket.

    bsls_Types::Uint64 unitsReserved() const;
        // Return the number of units that are reserved.

    bdet_TimeInterval lastUpdateTime() const;
        // Return the most recent time, as a time interval that this leaky
        // bucket was updated.  The returned time interval uses the same
        // reference point as the time interval specified during construction
        // or last invocation of the 'reset' method.

    void getStatistics(bsls_Types::Uint64* submittedUnits,
                       bsls_Types::Uint64* unusedUnits) const;
        // Load the specified 'submittedUnits' and 'unusedUnits' with the
        // numbers of submitted units and unused units respectively.
        // The number of used units is the total number of units submitted
        // to this bucket between 'statisticsCollectionStartTime' and 
        // 'lastUpdateTime'.  The number of unused units is the number of units
        // that could have been submited, but were not,
        // 'statisticsCollectionStartTime' and 'lastUpdateTime'.

    bdet_TimeInterval statisticsCollectionStartTime() const;
        // Return the time, as a time interval, when the collection of the
        // statistics (as returned by 'getStatistics') started.  The returned
        // time interval uses the same reference point as the time interval
        // specified during construction or last invocation of the 'reset'
        // method.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        //-----------------------
                        // class btes_LeakyBucket
                        //-----------------------

// CREATORS
inline
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

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)

inline
btes_LeakyBucket::~btes_LeakyBucket()
{
    BSLS_ASSERT_SAFE(d_drainRate >  0);
    BSLS_ASSERT_SAFE(d_capacity  >  0);
}

#endif

// MANIPULATORS
inline
void btes_LeakyBucket::resetStatistics()
{
    d_statisticsCollectionStartTime  = d_lastUpdateTime;
    d_statSubmittedUnits             = 0;
    d_statSubmittedUnitsAtLastUpdate = 0;
}

inline
void btes_LeakyBucket::reset(const bdet_TimeInterval& currentTime)
{
    d_lastUpdateTime     = currentTime;
    d_unitsInBucket = 0;
    d_unitsReserved = 0;
    resetStatistics();
}

inline
void btes_LeakyBucket::submit(bsls_Types::Uint64 numOfUnits)
{

    // Checking, whether adding 'numOfUnits' causes overflow of unsigned 64-bit
    // integer.

    BSLS_ASSERT_SAFE(numOfUnits <= ULLONG_MAX - d_unitsInBucket);

    BSLS_ASSERT_SAFE(
                d_unitsReserved <= ULLONG_MAX - d_unitsInBucket - numOfUnits);

    d_unitsInBucket  += numOfUnits;
    d_statSubmittedUnits += numOfUnits;
}

inline
void btes_LeakyBucket::reserve(bsls_Types::Uint64 numOfUnits)
{

    // Checking, whether submitting 'numOfUnits' causes overflow of unsigned
    // 64-bit integral type.

    BSLS_ASSERT_SAFE(numOfUnits <= ULLONG_MAX - d_unitsReserved);

    BSLS_ASSERT_SAFE(
                d_unitsInBucket <= ULLONG_MAX - d_unitsReserved - numOfUnits);

    d_unitsReserved += numOfUnits;
}

inline
void btes_LeakyBucket::submitReserved(bsls_Types::Uint64 numOfUnits)
{
    BSLS_ASSERT_SAFE(numOfUnits <= d_unitsReserved);

    // There is no need to check, whether 'numOfUnits' causes overflow, because
    // number of units to be reserved was checked by the 'reserve' method and
    // 'numOfUnits <= unitsReserved'.

    d_unitsReserved -= numOfUnits;

    submit(numOfUnits);
}

inline
void btes_LeakyBucket::cancelReserved(bsls_Types::Uint64 numOfUnits)
{
    BSLS_ASSERT_SAFE(numOfUnits <= d_unitsReserved);

    if (numOfUnits > d_unitsReserved) {
        d_unitsReserved = 0;
    }
    else {
        d_unitsReserved -= numOfUnits;
    }
}

// ACCESSORS
inline
bsls_Types::Uint64 btes_LeakyBucket::drainRate() const
{
    return d_drainRate;
}

inline
bsls_Types::Uint64 btes_LeakyBucket::capacity() const
{
    return d_capacity;
}

inline
bsls_Types::Uint64 btes_LeakyBucket::unitsInBucket() const
{
    return d_unitsInBucket;
}

inline
bsls_Types::Uint64 btes_LeakyBucket::unitsReserved() const
{
    return d_unitsReserved;
}

inline
bdet_TimeInterval btes_LeakyBucket::lastUpdateTime() const
{
    return d_lastUpdateTime;
}

inline
bdet_TimeInterval btes_LeakyBucket::statisticsCollectionStartTime() const
{
    return d_statisticsCollectionStartTime;
}

}  // closed enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007-2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
