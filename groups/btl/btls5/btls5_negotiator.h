// btls5_negotiator.h                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLS5_NEGOTIATOR
#define INCLUDED_BTLS5_NEGOTIATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism for SOCKS5 client-side handshake.
//
//@CLASSES:
//  btls5::Negotiator: negotiator for SOCKS5 client-side handshake
//
//@SEE_ALSO: btls5_networkconnector
//
//@DESCRIPTION: This component defines a mechanism, 'btls5::Negotiator', that
// asynchronously negotiates a connection to a destination host via a SOCKS5
// proxy.  Multiple negotiations can be conducted during the lifetime of a
// 'btls5::Negotiator', each one identified by a 'NegotiationHandle' object.
// The steps to negotiate a connection to a destination host is typically:
//: 1 Create a 'NegotiationHandle' by calling 'makeNegotiationHandle'.
//: 2 Start negotiation on the handle by calling 'startNegotiaion'.
//: 3 Optionally cancel the negotiation by calling 'cancelNegotiation'.
// The results of the negotiation are passed to a 'NegotiationStateCallback'
// specified by the client.  If successful, the TCP connection will be suitable
// for communication with the destination host.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Negotiation With Predefined Credentials
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// The following code snippets demonstrate how to use a 'btls5::Negotiator' to
// negotiate a SOCKS5 client-side handshake.
//
// First, we define the callback function that will receive the results of the
// negotiation.  It will signal the main thread by setting 'state', which in
// turn is protected by a mutex and a condition variable:
//..
//  void socks5Callback(btls5::Negotiator::NegotiationStatus  result,
//                      const btls5::DetailedStatus&          error,
//                      int                                  *state,
//                      bslmt::Mutex                         *stateLock,
//                      bslmt::Condition                     *stateChanged)
//  {
//      bslmt::LockGuard<bslmt::Mutex> lock(stateLock);
//      if (btls5::Negotiator::e_SUCCESS == result) {
//          *state = 0;
//      } else {
//          // report negotiation failure ...
//          *state = -1;
//      }
//      stateChanged->signal();
//  }
//..
// Next, we define the function that will invoke the negotiator on the
// previously connected socket, using predefined username and password for
// authentication:
//..
//  int negotiate(btlso::StreamSocket<btlso::IPv4Address> *socket,
//                const btlso::Endpoint&                   destination)
//  {
//      btls5::Credentials credentials("john.smith", "PassWord123");
//..
// Then, we declare the variable for communicating the response, with a mutex
// and a condition variable to protect access to it from different threads:
//..
//      bslmt::Mutex     stateLock;
//      bslmt::Condition stateChanged;
//      int              state = 1;  // 'state == 1' means negotiation is
//                                   // still in progress.
//..
// Next, we create an event manager and a 'btls5::Negotiator' and start
// negotiation:
//..
//      btlmt::TcpTimerEventManager eventManager;
//      int rc = eventManager.enable();
//      assert(0 == rc);
//      btls5::Negotiator negotiator(&eventManager);
//
//      using namespace bdlf::PlaceHolders;
//      btls5::Negotiator::NegotiationHandle
//          handle = negotiator.makeNegotiationHandle(
//                                         socket,
//                                         destination,
//                                         bdlf::BindUtil::bind(socks5Callback,
//                                                              _1,
//                                                              _2,
//                                                              &state,
//                                                              &stateLock,
//                                                              &stateChanged),
//                                         bsls::TimeInterval(),
//                                         credentials);
//      negotiator.startNegotiation(handle);
//..
// Now, we wait until the negotiation ends and 'socks5Callback' updates the
// 'state' variable:
//..
//      bslmt::LockGuard<bslmt::Mutex> lock(&stateLock);
//      while (1 == state) {
//          stateChanged.wait(&stateLock);
//      }
//..
// Finally, we return the status of SOCKS5 negotiation.  If '0 == state',
// 'socket' can be used to communicate with 'destination' through the proxy:
//..
//      return state;
//  }
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLS5_CREDENTIALS
#include <btls5_credentials.h>
#endif

#ifndef INCLUDED_BTLS5_DETAILEDSTATUS
#include <btls5_detailedstatus.h>
#endif

#ifndef INCLUDED_BTLMT_TCPTIMEREVENTMANAGER
#include <btlmt_tcptimereventmanager.h>
#endif

#ifndef INCLUDED_BTLSO_ENDPOINT
#include <btlso_endpoint.h>
#endif

#ifndef INCLUDED_BTLSO_IPV4ADDRESS
#include <btlso_ipv4address.h>
#endif

#ifndef INCLUDED_BTLSO_STREAMSOCKET
#include <btlso_streamsocket.h>
#endif

#ifndef INCLUDED_BDLT_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace btls5 {

class Negotiator_Negotiation;

                              // ================
                              // class Negotiator
                              // ================

class Negotiator {
    // This mechanism class negotiates connections to TCP destinations using
    // SOCKS5.  This class is 'const' thread-safe; an object can be negotiating
    // more than one connection at a time.
    //
    // If the SOCKS5 server requires username/password authentication and the
    // credentials were supplied ahead of time, then the 'Negotiator' object
    // will use them to attempt authentication.  If the SOCKS5 server requires
    // a different authentication method, or it requires username/password and
    // none were supplied, the negotiation will fail with the
    // 'e_AUTHENTICATION' status.
    //
    // A 'Negotiator' object allows multiple concurrent negotiations.  A
    // negotiation is initiated by calling 'makeNegotiationHandle' to obtain a
    // 'NegotiationHandle' that is subsequently passed to 'startNegotiation'.
    // The 'NegotiationHandle' object can be used to cancel the negotiation.

  public:

    // PUBLIC TYPES
    enum NegotiationStatus {
        e_SUCCESS        =  0,
        e_AUTHENTICATION = -1,  // no acceptable authentication methods
        e_ERROR          = -2   // any other error
    };

    typedef bsl::function<void(Negotiator::NegotiationStatus,
                               const DetailedStatus&)>
                                                      NegotiationStateCallback;
        // A callback of this type is invoked when a SOCKS5 negotiation is
        // complete.  If the specified 'status' is 0 the connection has been
        // established and the value of the specified 'detailedStatus' is
        // undefined; otherwise 'status' indicates the failure type and
        // 'detailedStatus' contains a detailed failure description.

    typedef bsl::shared_ptr<Negotiator_Negotiation> NegotiationHandle;
        // A 'NegotiationHandle' object is used to start a negotiation.  It can
        // also be used to cancel a negotiation in progress.

  private:
    // DATA
    btlmt::TcpTimerEventManager *d_eventManager_p;  // socket event manager,
                                                    // not owned

    bslma::Allocator            *d_allocator_p;     // memory allocator, not
                                                    // owned

    // NOT IMPLEMENTED
    Negotiator();
    Negotiator(const Negotiator&);
    Negotiator& operator=(const Negotiator&);

  public:
    // CREATORS
    explicit Negotiator(btlmt::TcpTimerEventManager *eventManager,
                        bslma::Allocator            *basicAllocator = 0);
        // Create a 'Negotiator' that can negotiate client-side SOCKS5
        // handshakes using the specified 'eventManager'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    //! ~Negotiator() = default;
        // Destroy this object.  Negotiations in progress are not canceled, and
        // callbacks supplied to 'makeNegotiationHandle' may be invoked after
        // this object is destroyed.

    // MANIPULATORS
    NegotiationHandle makeNegotiationHandle(
                          btlso::StreamSocket<btlso::IPv4Address> *socket,
                          const btlso::Endpoint&                   destination,
                          NegotiationStateCallback                 callback);
    NegotiationHandle makeNegotiationHandle(
                          btlso::StreamSocket<btlso::IPv4Address> *socket,
                          const btlso::Endpoint&                   destination,
                          NegotiationStateCallback                 callback,
                          const bsls::TimeInterval&                timeout);
    NegotiationHandle makeNegotiationHandle(
                         btlso::StreamSocket<btlso::IPv4Address> *socket,
                         const btlso::Endpoint&                   destination,
                         NegotiationStateCallback                 callback,
                         const Credentials&                       credentials);
    NegotiationHandle makeNegotiationHandle(
                         btlso::StreamSocket<btlso::IPv4Address> *socket,
                         const btlso::Endpoint&                   destination,
                         NegotiationStateCallback                 callback,
                         const bsls::TimeInterval&                timeout,
                         const Credentials&                       credentials);
        // Return a 'NegotiationHandle' object that can be used to
        // asynchronously negotiate on the specified 'socket' connecting to the
        // specified 'destination'.  Invoke the specified 'callback' when
        // negotiation is finished.  Optionally specify a 'timeout' defining
        // the maximum time within which to conclude the negotiation.  If
        // 'timeout' is not specified the negotiation will not time out.
        // Optionally specify 'credentials' to authenticate the SOCKS5
        // connection.  If 'credentials' are not specified, the negotiation
        // with a proxy requiring authentication will fail.  The handle can be
        // used to start the negotiation by calling 'startNegotiation' and
        // cancelled by calling 'cancelNegotiation'.

    int startNegotiation(const NegotiationHandle& handle);
        // Start negotiation for the specified 'handle'.  Return 0 if
        // negotiation is started, and a non-zero value on immediate error.
        // The behavior is undefined unless the event manager supplied at
        // construction is enabled, 'handle' was returned by a call to
        // 'makeNegotiationhandle', and this method has not already been called
        // with 'handle'.  Note that a return code of 0 does not indicate the
        // negotiation has concluded successfully.

    void cancelNegotiation(const NegotiationHandle& handle);
        // Cancel negotiation for the specified 'handle'.  Further invocation
        // of the associated callback is disabled, but this function does *not*
        // synchronize with callback invocation (i.e., an invocation initiated
        // before the call to this function may continue to execute after the
        // return from this function).  The behavior is undefined unless
        // 'handle' was returned by a call to 'makeNegotiationHandle' on this
        // object, and this method has not already been called with 'handle'.
};

}  // close package namespace

// TRAITS
namespace bslma {

template<>
struct UsesBslmaAllocator<btls5::Negotiator> : bsl::true_type {
};

}  // close namespace bslma

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
