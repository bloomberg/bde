// btes_leakybucket.h                                                 -*-C++-*-
#ifndef INCLUDED_BTES_LEAKYBUCKET
#define INCLUDED_BTES_LEAKYBUCKET

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
// monitor and control that the specified *capacity* of a given resource is
// consumed at a specified *drain* *rate*.
//
// The component is named after the analogy of a bucket with
// a hole: the maximum rate at which the bucket will let water out from the
// hole does not depend on the rate at which the water is poured in the bucket.
// If more water is poured than it is drained, eventually the bucket will
// overflow.
//
// The drain rate is the theoretical rate at which units are consumed (drained)
// from the bucket by its associated resource, and it is expressed in
// 'units/s'. 'unit' is a generic unit of measurement (e.g., bytes, n.o.
// messages, packets, liters, clock cycles, etc.). The capacity is the maximum
// number of units that the bucket can consume completely at the drain rate,
// and it determines the minimum amount of time required to empty the bucket,
// without exceeding the drain rate.
//
// For example, suppose we have an empty bucket with a drain rate 'd = 5 u/s'
// and capacity 'c = 5 u'.  As illustrated in FIG. 1,  at time 't0' we submit 5
// units to the bucket.  At time 't1 = t0 + 4s' the bucket drained 4 units, and
// we add 2 more. Note that if we added again 5 units we would overflow the
// capacity of the bucket. At time 't2 = t1 + 6' we are sure that the bucket is
// empty (because it was not overflown before), so we can add again 5 units
// without exceeding capacity.
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
// Time: t0        t1 = t0 + 4    t1'= t0 + 4   t2 = t1 + 6
//..
// FIG. 2 illustrates the case in which at time 't1' we add 6 more units,
// exceeding the capacity of the bucket.  At time 't2' we can see that 6 units
// have been drained, as we do not add any more units.
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
// Time: t0        t1 = t0 + 4    t1'= t0 + 4    t2 = t1 + 6
//..
// Note that units are added invoking the 'submit' method, and should be added
// only after the associated resource is actually used. At any point, the leaky
// bucket can be queried whether submitting a specified number of units would
// exceed the capacity of the leaky bucket, via the 'wouldOverflow' method.
//
// This component also offers provides the capability of *reserving* units.
// The reserved units are excluded from the bucket capacity and are not 
// drained when the bucket state is updated.  Reserved units may be later 
// submitted to the leaky bucket or the reservation may be canceled.
// When reserved units are submitted to the leaky bucket, they are excluded 
// from the reserve and added to the number of used units, that may be drained
// from the bucket during the update.  In case of cancelling the reserved 
// units, the units are excluded from the reserve and are not added to the
// number of used units.
//
// In the example of FIG. 3 at time 't0' we reserve 3 units.  At time 
// 't1 = t0 + 5' none the reserved units are drained from the leaky bucket(A)
// and we submit 3 units from the reserve(B).
// At time 't2 = t1 + 2' we can see that 2 units have been drained, 1 unit
// remains in bucket and one - in the reserve(A) and we cancel one reserved
// unit(B). It is taken out of the reserve without submitting it into the
// leaky bucket.
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
//    2|#####|     2|#####|     2|~~~~~|     2|~~~~~|     2|     |
//    1|#####|     1|#####|     1|#####|     1|#####|     1|~~~~~|
//     +-- --+      +-- --+      +-- --+      +-- --+      +-- --+
//                     A            B            A            B
//                \______________________/   \____________________/
// Time: t0              t1 = t0 + 5                t2 = t1 + 2
//..
///Modeling a Network Connection
///-----------------------------
// One of the intended use cases of the 'btes_LeakyBucket' is controlling
// the rate at which traffic is generated on a network to limit the use
// bandwidth .  In this case the drain rate is intended as the *ideal*
// maximum rate that the client wishes to *enforce* on their connection.
// Clients may choose to provide a value related to the physical limitations of
// their network, or any other arbitrary limit.  The capacity of the bucket,
// instead, maps onto the concept of buffering.  It describes the size of
// buffer, which is instantaneously filled with data to be transmitted.  The
// data will be actually sent over time, according to the speed of the medium
// used.
//
///Approximations
///--------------
// Note that this models entail the following approximations:
//: 1 Units are submitted istantaneously to the leaky bucket, whereas the
//:   resource consumes them over time, depending on the nature and speed
//:   of the resource, at an unspecified rate.
//: 2 The model simulates the drain of units from the buckets by removing units
//    at the specified drain rate, even though the resource effectively
//    consumes units at different rate. This guarantees that the specified
//    drain rate is never exceeded by the resource.
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
//
///-------------------------------------------------
// In some systems data is processed faster than it is consumed by I/O
// interfaces.  This circumstance could lead to data loss due to the overflow
// of the buffers where the data is queued before being processed.  On other
// systems, generic resources are shared, and their counsumption might need to
// be managed in order to guarantee QOS.
//
// Imagine the case of a network interface able to transfer only 1024 byte/s
// and assume that an application wants to transmit 1Mb of data over that
// network in 20 different 256-bytes data chunks.  Given the low speed of the
// network, we want to ensure that the communication uses, on average, less
// than 50% of the available bandwidth, or 512 byte/s.  In this way other
// clients can comfortably transmit and receive data on the same interface.
//
// Also, assume that we have a function 'sendData', that transmits data
// contained in a buffer over said network interface:
//..
//  bool sendData(const unsigned char *buffer, size_t dataSize);
//      Send the specified 'dataSize' through the network interface, and
//      return true if data was sent successfully, and false otherwise.
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
//  bsls_Types::Uint64 chunkSize  = 256;             // in bytes
//  bsls_Types::Uint64 totalSize  = 20 * chunkSize;  // in bytes
//..
// Now, we build a loop and for each iteration we check whether submitting
// another chunk of data to the bucket would cause overflow.  If not, we can
// send the data and submit it to the bucket.  The loop terminates when all the
// data is sent.  Note that 'submit' is invoked only after a successful
// operation on the resource.
//..
//  while (bytesSent < sizeOfData) {
//      now = bdetu_SystemTime::now();
//      if (!bucket.wouldOverflow(1, now)) {
//          if (true == mySendData(256)) {
//              bucket.submit(256);
//              bytesSent += 256;
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
//                                  (timeToSubmit.nanoseconds() % 1000) ? 1 : 0;
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

    bsls_Types::Uint64 d_nanoUnitsCarry;       // fractional number of units
                                               // carried from the last drain
                                               // operation

    bdet_TimeInterval  d_timestamp;            // time of last drain,
                                               // represented by time interval
                                               // relative to arbitrary
                                               // reference point

    bdet_TimeInterval d_maxUpdateInterval;     // maximum interval between
                                               // updates.

    bsls_Types::Uint64 d_submittedUnits;       // counter of units,
                                               // submitted since last
                                               // reset

    bsls_Types::Uint64 d_submittedUnitsAtLastUpdate; // value of submitted unit
                                                     // counter saved during
                                                     // last 'updateState' call

    bdet_TimeInterval  d_statisticsTimestamp;  // reference point for
                                               // statistics counter of
                                               // submitted units,
                                               // as time interval, relative to
                                               // arbitrary reference point

private:
    // NOT IMPLEMENTED
    btes_LeakyBucket& operator=(const btes_LeakyBucket&);

public:

    // CLASS METHODS

    static bdet_TimeInterval calculateDrainTime(bsls_Types::Uint64 numOfUnits,
                                                bsls_Types::Uint64 drainRate,
                                                bool ceilFlag);
        // Return the time interval, that is required to drain the specified
        // 'numOfUnits' at the specified 'drainRate', round up the number
        // of nanoseconds in the time interval if the specified 'ceilFlag' is
        // set to 'true', otherwise, round down the number of nanoseconds.
        // The behavior is undefined unless 'drainRate > 0' and the number of
        // seconds in the calculated interval may be represented by 64-bit
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
        // result evaluates to 0, return 1.
        // The behavior is undefined unless 'drainRate > 0', 'timeWindow > 0'
        // and 'drainRate' and 'timeWindow' product can be represented by
        // 64-bit unsigned integral type.

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
        // using specified 'currentTime' as the timestamp, and such that
        // 'unitsInBucket() == 0'.  The behaviour is undefined unless
        // 'drainRate > 0' and 'capacity > 0'.

    // The following copy CTOR is generated by the compiler, to enable
    // 'btes_LeakyBucket' usage with STL containers.

    //! btes_LeakyBucket(const btes_LeakyBucket& original) = default;
        // Create a 'btes_LeakyBucket' object having the parameters and state
        // of the specified 'original' object.  Note that this method's
        // definition is compiler generated.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)

    // The following destructor is generated by the compiler, except in "SAFE"
    // build modes (e.g., to enable the checking of class invariants).

    ~btes_LeakyBucket();
        // Destroy this object.

#endif

    // MANIPULATORS
    void updateState(const bdet_TimeInterval& currentTime);
        // Recalculate the number of units in this leaky bucket at the
        // specified  'currentTime', if 'currentTime' is later than the value
        // returned by the 'timestamp' method.  Set the 'timestamp' of this
        // leaky bucket to 'currentTime'.  If 'currentTime' is before the
        // value, returned by 'statisticsTimestamp' method, set the 
        // 'statisticsTimestamp' of this leaky bucket to 'currentTime'.

    void resetStatistics();
        // Reset the used units statistics counter and time interval of
        // collecting statistics.  Set the reference point for the interval
        // of collecting statistics to 'timestamp'.

    void setRateAndCapacity(bsls_Types::Uint64 newRate,
                            bsls_Types::Uint64 newCapacity);
        // Set the draining rate of this leaky bucket to the specified
        // 'newRate'(in units per second) and the capacity to the specified
        // 'newCapacity'.
        // The behavior is undefined unless 'newRate > 0' and 
        // 'newCapacity > 0'.

    void submit(bsls_Types::Uint64 numOfUnits);
        // Add, unconditionally, the specified 'numOfUnits' to this leaky
        // bucket.  Note that this operation might overflow the capacity of the
        // bucket.  The behavior is undefined unless the value
        // 'unitsReserved() + unitsInBucket() + numOfUnits()' can be
        // represented by 64-bit integral type.

     void reserve(bsls_Types::Uint64 numOfUnits);
        // Reserve, uncoditionally, the specified 'numOfUnits' for future
        // usage.  Note that after this operation number of reserved units may
        // exceed the capacity of the bucket.  The behavior is undefined unless
        // the value 'unitsReserved() + unitsInBucket() + numOfUnits()' can be
        // represented by 64-bit integral type.  Note that the time interval
        // between the invocations of 'reserve' and 'submitReserved' or
        // 'cancelReserved' should be as short as possible, otherwise it would
        // affect precision of calculating time interval by
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
        // Check whether adding the specified 'numOfUnits' into the bucket
        // at the specified 'currentTime' would exceed its capacity.
        // Return false if there is enough capacity for 'numOfUnits'.
        // If there is not enough capacity, drain the bucket, update the
        // timestamp of this leaky bucket to 'currentTime', and return false
        // if there is enough capacity for 'numOfUnits' at this point.
        // Return true otherwise. If 'currentTime' precedes 'lastUpdate', only
        // set the 'timestamp' of this leaky bucket to 'currentTime'.
        // The behavior is undefined unless 'numOfUnits > 0'.

    bdet_TimeInterval calculateTimeToSubmit(
                                         const bdet_TimeInterval& currentTime);
        // Return the estimated time interval, that should pass since the
        // specified 'currentTime', until it would be possible to submit one
        // more unit into this leaky bucket, without exceeding the capacity.
        // Return an interval of length 0, if one more unit can be submitted
        // at the specifiled 'currentTime'. Otherwise, update the timestamp of
        // this leaky bucket to 'currentTime' and return the time interval,
        // that should pass until one more unit can be submitted.
        // The number of nanoseconds in the time interval is rounded up.
        // Note that after waiting for the specified interval 'wouldOverflow'
        // check should be performed again, because since unit reservation is
        // provided the method returns an estimated interval.

    void reset(const bdet_TimeInterval& currentTime);
        // Reset the leaky bucket to its default-constructed state and set the
        // 'timestamp' of this 'btes_LeakyBucket' object to the specified
        // 'currentTime'.  Note that this function should be used if the bucket
        // was created long before starting to use it.

    // ACCESSORS
    bsls_Types::Uint64 drainRate() const;
        // Return the drain rate in units per second.

    bsls_Types::Uint64 capacity() const;
        // Return the size of this leaky bucket in units.

    bsls_Types::Uint64 unitsInBucket() const;
        // Return the number of units that are currently in the bucket.

    bsls_Types::Uint64 unitsReserved() const;
        // Return the number of units that are reserved.

    bdet_TimeInterval timestamp() const;
        // Return the timestamp of this leaky bucket, as a time interval,
        // describing the moment in time the bucket was last updated.  The
        // returned time interval uses the same reference point as the time
        // interval specified during construction or last invocation of the
        // 'reset' method.

    void getStatistics(bsls_Types::Uint64* submittedUnits,
                       bsls_Types::Uint64* unusedUnits) const;
        // Load the specified 'submittedUnits' and 'unusedUnits' with the
        // numbers of submitted units and unused units respectively.
        // The number of used units is the total number of units submitted
        // since the time returned by the 'statisticsTimestamp' method till
        // the time returned by the 'timestamp' method.  The number of unused
        // units is the number of units that potentially could have been 
        // submitted but were not since the time, returned by the 
        // 'statisticsTimestamp' method till the time, returned by the 
        // 'timestamp' method.

    bdet_TimeInterval statisticsTimestamp() const;
        // Return the statistics timestamp, as a time interval, indicating the
        // start time of the statistics collection for this leaky bucket.
        // The returned time interval uses the same reference point as the time
        // interval specified during construction or last invocation of the
        // 'reset' method.
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
, d_nanoUnitsCarry(0)
, d_timestamp(0)
, d_submittedUnits(0)
, d_submittedUnitsAtLastUpdate(0)
, d_statisticsTimestamp(0)
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
    d_statisticsTimestamp        = d_timestamp;
    d_submittedUnits             = 0;
    d_submittedUnitsAtLastUpdate = 0;
}

inline
void btes_LeakyBucket::reset(const bdet_TimeInterval& currentTime)
{
    d_timestamp     = currentTime;
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
    d_submittedUnits += numOfUnits;
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
bdet_TimeInterval btes_LeakyBucket::timestamp() const
{
    return d_timestamp;
}

inline
bdet_TimeInterval btes_LeakyBucket::statisticsTimestamp() const
{
    return d_statisticsTimestamp;
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
