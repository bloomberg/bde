// btls_reservationguard.h                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLS_RESERVATIONGUARD
#define INCLUDED_BTLS_RESERVATIONGUARD

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a generic proctor for rate controlling objects.
//
//@CLASSES:
//  btls::ReservationGuard: a guard for reserving resources from rate limiters.
//
//@SEE_ALSO: btls_leakybucket, btls_ratelimiter
//
//@DESCRIPTION: This component provides generic proctor to automatically
// reserve and release units from a rate controlling object.  The rate
// controlling object can be of any type (typically either a
// 'btls::RateLimiter' or 'btls::LeakyBucket') that provides the following
// methods:
//..
//  void reserve(bsls::Types::Uint64 numOfUnits);
//  void submitReserved(bsls::Types::Uint64 numOfUnits);
//  void cancelReserved(bsls::Types::Uint64 numOfUnits);
//..
// Use 'btls::ReservationGuard' to ensure that reserved units will be correctly
// returned to a rate controlling object in a programming scope.  Note that
// 'btls::ReservationGuard' does not assume ownership of the rate controlling
// object.
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Guarding units reservation in operations with btls::LeakyBucket
///--------------------------------------------------------------------------
// Suppose that we are limiting the rate of network traffic generation using a
// 'btls::LeakyBucket' object.  We send data buffer over a network interface
// using the 'mySendData' function:
//..
//  static bsls::Types::Uint64 mySendData(size_t dataSize)
//      // Send a specified 'dataSize' amount of data over the network.  Return
//      // the amount of data actually sent.  Throw an exception if a network
//      // failure is detected.
//  {
//      // In our example we don`t deal with actual data sending, so we assume
//      // that the function has sent certain amount of data (3/4 of the
//      // requested amount) successfully.
//      return (dataSize * 3) >> 2;
//  }
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
// 'btls::ReservationGuard' is designed to resolve this issue.
//
// First, we define the size of each data chunk and the total size of the data
// to send:
//..
//  const unsigned int  CHUNK_SIZE = 256;
//  bsls::Types::Uint64 bytesSent  = 0;
//  bsls::Types::Uint64 totalSize  = 10 * 1024; // in bytes
//..
// Then, we create a 'btls::LeakyBucket' object to limit the rate of data
// transmission:
//..
//  bsls::Types::Uint64 rate     = 512;
//  bsls::Types::Uint64 capacity = 1536;
//  bsls::TimeInterval  now      = bdlt::CurrentTime::now();
//  btls::LeakyBucket   bucket(rate, capacity, now);
//..
// Next, we send the chunks of data using a loop.  For each iteration, we check
// whether submitting another byte would cause the leaky bucket to overflow:
//..
//  while (bytesSent < totalSize) {
//      now = bdlt::CurrentTime::now();
//      if (!bucket.wouldOverflow(now)) {
//..
// Now, if the leaky bucket would not overflow, we create a
// 'btls::ReservationGuard' object to reserve the amount of data to be sent:
//..
//          btls::ReservationGuard<btls::LeakyBucket> guard(&bucket,
//                                                          CHUNK_SIZE);
//..
// Then, we use the 'mySendData' function to send the data chunk over the
// network.  After the data had been sent, we submit the amount of reserved
// data that was actually sent:
//..
//          bsls::Types::Uint64 result;
//          result = mySendData(CHUNK_SIZE);
//          bytesSent += result;
//          guard.submitReserved(result);
//..
// Note that we do not have manually cancel any remaining units reserved by the
// 'btls::ReservationGuard' object either because 'mySendData' threw an
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
//          bsls::TimeInterval timeToSubmit =
//                                           bucket.calculateTimeToSubmit(now);
//          bsls::Types::Uint64 uS = timeToSubmit.totalMicroseconds() +
//                                 (timeToSubmit.nanoseconds() % 1000) ? 1 : 0;
//          bslmt::ThreadUtil::microSleep(static_cast<int>(uS));
//      }
//  }
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {
namespace btls {

                          // ======================
                          // class ReservationGuard
                          // ======================

template<class TYPE>
class ReservationGuard {
    // This class template implements a proctor for reserving and cancelling
    // units in a rate controlling object.
    //
    // This class:
    //: o is *exception* *neutral* (agnostic)
    //: o is *const* *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    TYPE                *d_rateController_p;  // Pointer to the rate
                                              // controlling object in which
                                              // the units are reserved.

    bsls::Types::Uint64  d_unitsReserved;    // Number of units reserved by
                                             // this object.

  private:
    // NOT IMPLEMENTED
    ReservationGuard();
    ReservationGuard& operator =(const ReservationGuard<TYPE>&);
    ReservationGuard(const ReservationGuard<TYPE>&);

  public:
    // CREATORS
    ReservationGuard(TYPE* rateController, bsls::Types::Uint64 numUnits);
        // Create a 'ReservationGuard' object guarding the specified
        // 'rateController' and reserving the specified 'numUnits'.

    ~ReservationGuard();
        // Destroy this object.  Invoke the 'cancelReserved' method for the
        // remaining remaining units reserved by this proctor.

    // MANIPULATORS
    void submitReserved(bsls::Types::Uint64 numUnits);
        // Submit the specified 'numUnits' from the reserve units guarded by
        // this object.  After this operation, the number of reserved units
        // guarded by this object will be reduced by 'numUnits'.  The behavior
        // is undefined unless 'numUnits <= unitsReserved()'.

    void cancelReserved(bsls::Types::Uint64 numUnits);
        // Cancel the specified 'numUnits' from the reserve units guarded by
        // this object.  Subtract the 'numUnits' from 'unitsReserved' and
        // invoke the 'cancelReserved' method on the guarded object for
        // 'numUnits'.  After this operation, the number of reserved units
        // guarded by this object will be reduced by 'numUnits'.  The behavior
        // is undefined unless 'numUnits <= unitsReserved()'.

    // ACCESSORS
    bsls::Types::Uint64 unitsReserved() const;
        // Return the number of units reserved by this object.

    TYPE *ptr() const;
        // Return a pointer to the rate controlling object used by this object.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                          // ----------------------
                          // class ReservationGuard
                          // ----------------------

// CREATORS
template <class TYPE>
inline
ReservationGuard<TYPE>::ReservationGuard(TYPE                *rateController,
                                         bsls::Types::Uint64  numUnits)
{
    BSLS_ASSERT_SAFE(0 != rateController);

    d_rateController_p = rateController;
    d_unitsReserved    = numUnits;

    d_rateController_p->reserve(numUnits);
}

template <class TYPE>
inline
ReservationGuard<TYPE>::~ReservationGuard()
{
    d_rateController_p->cancelReserved(d_unitsReserved);
}

// ACCESSORS
template <class TYPE>
inline
bsls::Types::Uint64 ReservationGuard<TYPE>::unitsReserved() const
{
    return d_unitsReserved;
}

template <class TYPE>
inline
TYPE *ReservationGuard<TYPE>::ptr() const
{
    return d_rateController_p;
}

// MANIPULATORS
template <class TYPE>
inline
void ReservationGuard<TYPE>::cancelReserved(bsls::Types::Uint64 numUnits)
{
    BSLS_ASSERT_SAFE(numUnits <= d_unitsReserved);

    d_rateController_p->cancelReserved(numUnits);
    d_unitsReserved -= numUnits;
}

template <class TYPE>
inline
void ReservationGuard<TYPE>::submitReserved(bsls::Types::Uint64 numUnits)
{
    BSLS_ASSERT_SAFE(numUnits <= d_unitsReserved);

    d_rateController_p->submitReserved(numUnits);
    d_unitsReserved -= numUnits;
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
