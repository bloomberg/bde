#ifndef BTES_RESERVATIONGUARD
#define BTES_RESERVATIONGUARD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a generic proctor for rate controlling objects.
//
//@CLASSES:
//  btes_ReservationGuard: a guard for reserving resources from rate limiters.
//
//@SEE_ALSO: btes_leakybucket, btes_ratelimiter
//
//@AUTHOR: Mikhail Kunitskiy (mkunitskiy1)
//
//@DESCRIPTION: This component provides generic proctor to automatically
// reserve and release units indicating resource consumption in an external
// rate controlling object.  The rate controlling object can be of any type
// (e.g., 'btes_RateLimiter' or 'btes_LeakyBucket') that provides the following
// methods:
//..
//  void reserve(bsls_Types::Uint64 numOfUnits);
//  void submitReserved(bsls_Types::Uint64 numOfUnits);
//  void cancelReserved(bsls_Types::Uint64 numOfUnits);
//..
// Note that 'btes_ReservationGuard' does not assume ownership of the external
// rate controlling object.  Also note that objects of this guard type can not
// be constructed with a null pointer to the guarded object.
//
///Thread Safety
///-------------
// 'btes_reservationguard' is *const* *thread-safe*.
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
// Use this component to ensure that in the event of an exception or exit from
// any point in a given scope the reserved units, that are remaining unused
// will be correctly returned to the object, controlling the consumption of a
// resource.
//
///Example 1: Guarding units reservation in operations with btes_LeakyBucket
///-------------------------------------------------------------------------
// Imagine the case in which we are trying to limit network bandwidth usage
// and control traffic generation, using a 'btes_LeakyBucket' object.
// Suppose that we have a function 'sendData', that transmits data contained
// in a buffer over said network interface. In certain case this function may
// be able to send only a part of required data block. It may also generate
// exceptions:
//..
//  bsls_Types::Uint64 mySendData(size_t dataSize);
//      // Try to send a specified 'dataSize' amount of data over the network
//      // return the amount of data, that was actually sent.
//      // Note that this function throws an exception, if a network failure
//      // is detected.
//..
// First we define the size of each data chunk, and the total size of the data
// to transmit:
//..
//  const bsls_Types::Uint64 CHUNK_SIZE = 256;
//  bsls_Types::Uint64 bytesSent        = 0;
//  bsls_Types::Uint64 sizeOfData       = 10 * 1024; // in bytes
//..
// Next, we create a 'btes_LeakyBucket' object having the required parameters:
//..
//  bsls_Types::Uint64 rate     = 512;
//  bsls_Types::Uint64 capacity = 1536;
//..
//..
//  bdet_TimeInterval now = bdetu_SystemTime::now();
//  btes_LeakyBucket  bucket(rate, capacity, now);
//..
//  Then we define a loop in which we test:
//..
//  while (bytesSent < sizeOfData) {
//
//      now = bdetu_SystemTime::now();
//..
// Now, for each iteration we check whether submitting another chunk into the
// bucket would cause overflow:
//..
//      if (!bucket.wouldOverflow(CHUNK_SIZE,now)) {
//..
// If sending data is allowed, we reserve units in the leaky bucket for one
// data chunk.  We create a 'btes_ReservationGuard' object, specifying the
// 'btes_LeakyBucket' object, in which we want to reserve units and the
// number of units, we want to reserve:
//..
//          btes_ReservationGuard<btes_LeakyBucket> guard(&bucket,
//                                                        CHUNK_SIZE);
//..
// Next, we are trying to send data chunk over the network and submit the
// number of units corresponding to the amount of data that was actually sent:
//..
//          bsls_Types::Uint64 result;
//          result = mySendData(CHUNK_SIZE);
//          bytesSent += result;
//          guard.submitReserved(result);
//..
// We do not care about the units that possibly remained unused, because they
// will be cancelled automatically, when 'guard' goes out of scope and is
// destroyed. Note that if 'mySendData' throws an exception, all units will be
// also returned to the 'leakyBucket' object.
//..
//      }
//..
// In case submitting the data chunk would cause overflow, we invoke
// the 'calculateTimeToSubmit' method to determine how much time we need to
// wait before submitting the data chunk without overflowing the bucket.
// We round up the number of microseconds in time interval:
//..
//      else {
//
//          bdet_TimeInterval timeToSubmit = bucket.calculateTimeToSubmit(now);
//          bsls_Types::Uint64 uS = timeToSubmit.totalMicroseconds() +
//                                 (timeToSubmit.nanoseconds() % 1000) ? 1 : 0;
//          bcemt_ThreadUtil::microSleep(uS);
//      }
//  }

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BTES_LEAKYBUCKET
#include <btes_leakybucket.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

                        //============================
                        // class btes_ReservationGuard
                        //============================

template<class TYPE>
class btes_ReservationGuard {
    // This class template implements a proctor for reserving and cancelling
    // units, representing resource consumption in the rate controlling
    // objects.
    //
    // This class:
    //: o is *exception* *neutral* (agnostic)
    //: o is *const* *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    TYPE                  *d_reserve_p;    // Pointer to the object, units are
                                        // reserved in.

    bsls_Types::Uint64 d_unitsReserved; // Number of units reserved by this
                                        // 'btes_ReservationGuard' instance.

    // NOT IMPLEMENTED
    btes_ReservationGuard();
    btes_ReservationGuard& operator =(const btes_ReservationGuard<TYPE>&);
    btes_ReservationGuard(const btes_ReservationGuard<TYPE>&);

    public:

    // CREATORS
    btes_ReservationGuard(TYPE* rateLimiter, bsls_Types::Uint64 numOfUnits);
        // Create a 'btes_ReservationGuard' object, guarding the specified
        // 'rateLimiter' and reserving the specified 'numOfUnits'.

    ~btes_ReservationGuard();
        // Destroy this object. Invoke the 'cancelReserved' method for the
        // remaining 'unitsReserved' on the object under management by this
        // proctor.

    // MANIPULATORS
    void submitReserved(bsls_Types::Uint64 numOfUnits);
        // Submit the specified 'numOfUnits' from the reserve of the guarded
        // object.  Subtract the 'numOfUnits' from the number of
        // 'unitsReserved' and invoke the 'submitReserved' method on the
        // guarded object for 'numOfUnits'.  The behavior is undefined unless
        // 'numOfUnits <= unitsReserved()'.

    void cancelReserved(bsls_Types::Uint64 numOfUnits);
        // Cancel the specified 'numOfUnits' from the reserve of the guarded
        // object.  Subtract the 'numOfUnits' from 'unitsReserved' and invoke
        // the 'cancelReserved' method on the guarded object for
        // 'numOfUnits'.  The behavior is undefined unless
        // 'numOfUnits <= unitsReserved()'.

    // ACCESSORS
    bsls_Types::Uint64 unitsReserved() const;
        // Return the number of units that are currently reserved by this
        // 'btes_ReservationGuard' instance.

    TYPE *ptr() const;
        // Return pointer to the guarded object
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        //----------------------------
                        // class btes_ReservationGuard
                        //----------------------------

// CREATORS
template <class TYPE>
inline
btes_ReservationGuard<TYPE>::btes_ReservationGuard(TYPE*                 reserve,
                                                bsls_Types::Uint64 numOfUnits)
{
    BSLS_ASSERT_SAFE(0 != reserve);

    d_reserve_p     = reserve;
    d_unitsReserved = numOfUnits;

    d_reserve_p->reserve(numOfUnits);
}

template <class TYPE>
inline
btes_ReservationGuard<TYPE>::~btes_ReservationGuard()
{
    d_reserve_p->cancelReserved(d_unitsReserved);
}

// ACCESSORS
template <class TYPE>
inline
bsls_Types::Uint64 btes_ReservationGuard<TYPE>::unitsReserved() const
{
    return d_unitsReserved;
}

template <class TYPE>
inline
TYPE *btes_ReservationGuard<TYPE>::ptr() const
{
    return d_reserve_p;
}

// MANIPULATORS
template <class TYPE>
inline
void btes_ReservationGuard<TYPE>::cancelReserved(bsls_Types::Uint64 numOfUnits)
{
    BSLS_ASSERT_SAFE(numOfUnits <= d_unitsReserved);

    d_reserve_p->cancelReserved(numOfUnits);
    d_unitsReserved -= numOfUnits;
}

template <class TYPE>
inline
void btes_ReservationGuard<TYPE>::submitReserved(bsls_Types::Uint64 numOfUnits)
{
    BSLS_ASSERT_SAFE(numOfUnits <= d_unitsReserved);

    d_reserve_p->submitReserved(numOfUnits);
    d_unitsReserved -= numOfUnits;
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
