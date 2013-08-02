// btes5_negotiator.h                                                 -*-C++-*-
#ifndef INCLUDED_BTES5_NEGOTIATOR
#define INCLUDED_BTES5_NEGOTIATOR

//@PURPOSE:  Provide SOCKS5 client-side handshake negotiation
//
//@CLASSES:
//   btes5_Negotiator: negotiator to facilitate SOCKS5 client-side handshake
//
//@SEE ALSO: btes5_networkconnector
//
//@DESCRIPTION: This component defines a mechanism for negotiating a
// SOCKS5 client-side handshake.
//
///Usage Example
///-------------
///Example 1: Connection With Predefined Credentials
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The following code snippets demonstrate how to use a 'btes5_Negotiator' to
// negotiate a SOCKS5 client-side handshake.  First we will declare and define
// the callback function.
//..
//  void negotiateCb(btes5_Negotiator::NegotiationStatus  result,
//                   btes5_DetailedError                 *error)
//  {
//      assert(result == btes5_Negotiator::SUCCESS);
//  }
//..
// Next we define the function that will invoke the negotiator on the
// previously connected socket, using predefined name and password for
// authentication.
//..
//  void negotiate(bteso_StreamSocket<bteso_IPv4Address> *socket,
//                 const bteso_Endpoint&                  destination)
//  {
//      btes5_Credentials credentials("john.smith", "PassWord123");
//..
// Finally we will create a 'btes5_Negotiator' and start negotiation.
//..
//      btes5_Negotiator negotiator;
//      negotiator.negotiate(socket, destination, credentials, &negotiateCb);
//..
//
///Example 2: Connection With Dynamically Acquired Credentials
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The username and password are not available ahead of time; instead, the
// negotiator will use the provided object to acquire them, possibly by
// prompting the user. We'll assume 'negotiateCb' is already defined.
//
// First, we define a class derived from 'btes5_CredentialsProvider' which will
// provide username and password.
//..
//  class MyCredentialsProvider : public btes5_CredentialsProvider {
//    public:
//      virtual void acquireCredentials(CredentialsCallback callback);
//      virtual void cancelAcquiringCredentials();
//  };
//
//  void MyCredentialsProvider::acquireCredentials(
//                                         CredentialsCallback callback)
//  {
//      // normally we might prompt the user for username and password, but
//      // here we use hard-coded values.
//      callback("jane.doe", "PassWord456");
//  }
//  void cancelAcquiringCredentials()
//  {
//      // we would normally cancel the user prompting process, if one is in
//      // progress; but here it's a no-op.
//  }
//..
// Next we define the function that will invoke the negotiator on the
// previously connected socket, using a 'MyCredentialsProvider' object to
// acquire credentials.
//..
//  void negotiate2(bteso_StreamSocket<bteso_IPv4Address> *socket,
//                  const bteso_Endpoint&                  destination)
//  {
//      MyCredentialsProvider provider;
//..
// Finally we will create a 'btes5_Negotiator' and start negotiation.
//..
//      btes5_Negotiator negotiator2;
//      negotiator2.negotiate(socket, destination, &negotiateCb, &provider);
//..

#ifndef INCLUDED_BTES5_CREDENTIALSPROVIDER
#include <btes5_credentialsprovider.h>
#endif

#ifndef INCLUDED_BTES5_DETAILEDERROR
#include <btes5_detailederror.h>
#endif

#ifndef INCLUDED_BTES5_USERCREDENTIALS
#include <btes5_credentials.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BTESO_ENDPOINT
#include <bteso_endpoint.h>
#endif

#ifndef INCLUDED_BTESO_IPV4ADDRESS
#include <bteso_ipv4address.h>
#endif

#ifndef INCLUDED_BTESO_STREAMSOCKET
#include <bteso_streamsocket.h>
#endif

#ifndef INCLUDED_BTESO_TIMEREVENTMANAGER
#include <bteso_timereventmanager.h>
#endif

namespace BloombergLP {

                        // ======================
                        // class btes5_Negotiator
                        // ======================
class btes5_Negotiator {
    // Class to connect to a TCP destination using a SOCKS5 connection. This
    // class is const thread-safe; an object can only be negotiating one
    // connection at a time. However, multiple connections can be negotiated
    // over the lifetime of a single 'btes5_Negotiator' object. If the SOCKS5
    // server requires username/password authentication, and the credentials
    // were supplied ahead of time, or a credentials provider was specified,
    // then the 'btes5_Negotiator' object will use them to attempt
    // authentication. If the SOCKS5 server requires a different authentication
    // method, or it requires username/password and none were supplied, the
    // negotiation will fail with 'status == e_AUTHENTICATION'.

  public:
    // PUBLIC TYPES
    enum NegotiationStatus {
        e_SUCCESS = 0,
        e_AUTHENTICATION, // no acceptable authentication methods
        e_ERROR           // any other error
    };
    typedef bdef_Function<void(*)(btes5_Negotiator::NegotiationStatus status,
                                  const btes5_DetailedError&          error)>
        NegotiationStateCallback;
        // The callback of this type is invoked when a SOCKS5 negotiation is
        // complete. If the specified 'status' is 0 the connection has been
        // established, otherwise 'status' indicates the failure type and the
        // specified 'error' contains detailed failure description.

  private:
    // DATA
    bteso_TimerEventManager *d_eventManager_p; // socket event manager, held
    bslma::Allocator        *d_allocator_p;    // memory allocator, held

    // NOT IMPLEMENTED
    btes5_Negotiator();
    btes5_Negotiator(const btes5_Negotiator&);
    btes5_Negotiator& operator=(const btes5_Negotiator&);

  public:
    // CREATORS
    explicit btes5_Negotiator(bteso_TimerEventManager  *eventManager,
                              bslma::Allocator         *allocator = 0);
        // Create a 'btes5_Negotiator' that can negotiate a client-side SOCKS5
        // handshake using the specified 'eventManager'. If the optionally
        // specified 'allocator' is not zero use it to allocate memory,
        // otherwise use the default allocator.

    ~btes5_Negotiator();
        // Destroy this object.

    // MANIPULATORS
    int negotiate(bteso_StreamSocket<bteso_IPv4Address> *socket,
                  const bteso_Endpoint&                  destination,
                  NegotiationStateCallback               callback,
                  const bdet_TimeInterval&               timeout);
    int negotiate(bteso_StreamSocket<bteso_IPv4Address> *socket,
                  const bteso_Endpoint&                  destination,
                  NegotiationStateCallback               callback,
                  const bdet_TimeInterval&               timeout,
                  const btes5_Credentials&               credentials);
        // Start SOCKS5 client-side negotiation on the specified 'socket' to
        // connect to the specified 'destination' using the optionally
        // specified 'credentials' to authenticate the SOCKS5 connection.  If
        // the specified 'timeout' is not empty, complete the negotiation
        // within this time period or time out.  Invoke the specified
        // 'callback' when negotiation is finished. Return 0 on successful
        // start, and a non-zero value on immediate failure. Note that 'socket'
        // will not be closed on error.

    int negotiate(bteso_StreamSocket<bteso_IPv4Address> *socket,
                  const bteso_Endpoint&                  destination,
                  NegotiationStateCallback               callback,
                  const bdet_TimeInterval&               timeout,
                  btes5_CredentialsProvider             *provider);
        // Start SOCKS5 client-side negotiation on the specified 'socket' to
        // connect to the specified 'destination' using the specified
        // 'provider' to acquire credentials for authentication.  If the
        // specified 'timeout' is not empty, complete the negotiation within
        // this time period or time out. Invoke the specified 'callback' when
        // negotiation is finished. Return 0 on successful start, and a
        // non-zero value on immediate failure. Note that 'socket' will not be
        // closed on error.

    // TODO: do we need a cancel interface?

};

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
