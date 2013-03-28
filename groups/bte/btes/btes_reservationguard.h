// btes_reservationguard.h                                            -*-C++-*-
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
// reserve and release units from a rate controlling object.  The rate
// controlling object can be of any type (typically either a 'btes_RateLimiter'
// or 'btes_LeakyBucket') that provides the following methods:
//..
//  void reserve(bsls_Types::Uint64 numOfUnits);
//  void submitReserved(bsls_Types::Uint64 numOfUnits);
//  void cancelReserved(bsls_Types::Uint64 numOfUnits);
//..
// Use 'btes_ReservationGuard' to ensure that reserved units will be correctly
// returned to a rate controlling object in a programming scope.  Note that
// 'btes_ReservationGuard' does not assume ownership of the rate controlling
// object.
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Guarding units reservation in operations with btes_LeakyBucket
///-------------------------------------------------------------------------
// Suppose that we are limiting the rate of network traffic generation using a
// 'btes_LeakyBucket' object.  We send data buffer over a network interface
// using the 'mySendData' function:
//..
//  bsls_Types::Uint64 mySendData(size_t dataSize);
//      // Send a specified 'dataSize' amount of data over the network.  Return
//      // the amount of data actually sent.  Throw an exception if a network
//      // failure is detected.
//..
// Notice that the 'mySendData' function may throw an exception; therefore, we
// should wait until 'mySendData' returns before indicating the amount of data
// sent to the leaky bucket.
//
// Further suppose that multiple threads are sending network data and sharing
// the same leaky bucket.  If every thread simply checks for overflowing of the
// leaky bucket, send data, and then submit to the leaky bucket, then the rate
// of data usage may exceed the limits imposed by the leaky bucket due to race
// conditions.  We can avoid the this issue by reserving the amount of data
// immediately after checking whether the leaky bucket has overflown and submit
// the reserved amount after the data has been sent.  However, this process
// could lead to the loss of the reserved units (effectively decreasing the
// leaky bucket's capacity) if 'mySendData' throws an exception.
// 'btes_ReservationGuard' is designed to resolve this issue.
//
// First, we define the size of each data chunk and the total size of the data
// to send:
//..
//  const bsls_Types::Uint64 CHUNK_SIZE = 256;
//  bsls_Types::Uint64 bytesSent        = 0;
//  bsls_Types::Uint64 totalSize        = 10 * 1024; // in bytes
//..
// Then, we create a 'btes_LeakyBucket' object to limit the rate of data
// transmission:
//..
//  bsls_Types::Uint64 rate     = 512;
//  bsls_Types::Uint64 capacity = 1536;
//  bdet_TimeInterval  now       = bdetu_SystemTime::now();
//  btes_LeakyBucket   bucket(rate, capacity, now);
//..
// Next, we send the chunks of data using a loop.  For each iteration, we check
// whether submitting another byte would cause the leaky bucket to overflow:
//..
//  while (bytesSent < totalSize) {
//
//      now = bdetu_SystemTime::now();
//      if (!bucket.wouldOverflow(now)) {
//..
// Now, if the leaky bucket would not overflow, we create a
// 'btes_ReservationGuard' object to reserve the amount of data to be sent:
//..
//          btes_ReservationGuard<btes_LeakyBucket> guard(&bucket,
//                                                        CHUNK_SIZE);
//..
// Then, we use the 'mySendData' function to send the data chunk over the
// network.  After the data had been sent, we submit the amount of reserved
// data that was actually sent:
//..
//          bsls_Types::Uint64 result;
//          result = mySendData(CHUNK_SIZE);
//          bytesSent += result;
//          guard.submitReserved(result);
//..
// Note that we do not have manually cancel any remaining units reserved by the
// 'btes_ReservationGuard' object either because 'mySendData' threw an
// exception, or the data was only partially sent, because when the guard
// object goes out of scope, all remaining reserved units will be automatically
// cancelled.
//..
//      }
//..
// Finally, if submitting another byte will cause the leaky bucket to overflow,
// then we wait until the submission will be allowed by waiting for an amount
// time returned by the 'calculateTimeToSubmit' method:
//..
//      else {
//
//          bdet_TimeInterval timeToSubmit = bucket.calculateTimeToSubmit(now);
//          bsls_Types::Uint64 uS = timeToSubmit.totalMicroseconds() +
//                                 (timeToSubmit.nanoseconds() % 1000) ? 1 : 0;
//          bcemt_ThreadUtil::microSleep(uS);
//      }
//  }
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERTTEST
#include <bsls_asserttest.h>
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
    // units in a rate controlling object.
    //
    // This class:
    //: o is *exception* *neutral* (agnostic)
    //: o is *const* *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    TYPE                  *d_rateController_p;  // Pointer to the rate
                                                // controlling object in which
                                                // the units are reserved.

    bsls_Types::Uint64     d_unitsReserved;     // Number of units reserved by
                                                // this object.

  private:
    // NOT IMPLEMENTED
    btes_ReservationGuard();
    btes_ReservationGuard& operator =(const btes_ReservationGuard<TYPE>&);
    btes_ReservationGuard(const btes_ReservationGuard<TYPE>&);

  public:
    // CREATORS
    btes_ReservationGuard(TYPE* rateController, bsls_Types::Uint64 numUnits);
        // Create a 'btes_ReservationGuard' object guarding the specified
        // 'rateController' and reserving the specified 'numUnits'.

    ~btes_ReservationGuard();
        // Destroy this object.  Invoke the 'cancelReserved' method for the
        // remaining remaining units reserved by this proctor.

    // MANIPULATORS
    void submitReserved(bsls_Types::Uint64 numUnits);
        // Submit the specified 'numUnits' from the reserve units guarded by
        // this object.  After this operation, the number of reserved units
        // guarded by this object will be reduced by 'numUnits'.  The behavior
        // is undefined unless 'numUnits <= unitsReserved()'.

    void cancelReserved(bsls_Types::Uint64 numUnits);
        // Cancel the specified 'numUnits' from the reserve units guarded by
        // this object.  Subtract the 'numUnits' from 'unitsReserved' and
        // invoke the 'cancelReserved' method on the guarded object for
        // 'numUnits'.  After this operation, the number of reserved units
        // guarded by this object will be reduced by 'numUnits'.  The behavior
        // is undefined unless 'numUnits <= unitsReserved()'.

    // ACCESSORS
    bsls_Types::Uint64 unitsReserved() const;
        // Return the number of units reserved by this object.

    TYPE *ptr() const;
        // Return a pointer to the rate controlling object used by this object.
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
btes_ReservationGuard<TYPE>::btes_ReservationGuard(
                                             TYPE*              rateController,
                                             bsls_Types::Uint64 numUnits)
{
    BSLS_ASSERT_SAFE(0 != rateController);

    d_rateController_p = rateController;
    d_unitsReserved    = numUnits;

    d_rateController_p->reserve(numUnits);
}

template <class TYPE>
inline
btes_ReservationGuard<TYPE>::~btes_ReservationGuard()
{
    d_rateController_p->cancelReserved(d_unitsReserved);
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
    return d_rateController_p;
}

// MANIPULATORS
template <class TYPE>
inline
void btes_ReservationGuard<TYPE>::cancelReserved(bsls_Types::Uint64 numUnits)
{
    BSLS_ASSERT_SAFE(numUnits <= d_unitsReserved);

    d_rateController_p->cancelReserved(numUnits);
    d_unitsReserved -= numUnits;
}

template <class TYPE>
inline
void btes_ReservationGuard<TYPE>::submitReserved(bsls_Types::Uint64 numUnits)
{
    BSLS_ASSERT_SAFE(numUnits <= d_unitsReserved);

    d_rateController_p->submitReserved(numUnits);
    d_unitsReserved -= numUnits;
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
