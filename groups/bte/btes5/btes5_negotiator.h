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
// [!WARNING!] This component should only be used by API at this time.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Connection With Predefined Credentials
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The following code snippets demonstrate how to use a 'btes5_Negotiator' to
// negotiate a SOCKS5 client-side handshake.  First we will declare and define
// the callback function. It will signal the main thread by setting 'state',
// which in turn is protected by a mutex and a condition variable.
//..
//  void socks5Callback(btes5_Negotiator::NegotiationStatus result,
//                      btes5_DetailedError                 error,
//                      bcemt_Mutex                         *stateLock,
//                      bcemt_Condition                     *stateChanged,
//                      volatile int                        *state)
//  {
//      bcemt_LockGuard<bcemt_Mutex> lock(stateLock);
//      if (result == btes5_Negotiator::e_SUCCESS) {
//          *state = 1;
//      } else {
//          // report negotiation failure ...
//          *state = -1;
//      }
//      stateChanged->signal();
//  }
//..
// Next we define the function that will invoke the negotiator on the
// previously connected socket, using predefined name and password for
// authentication.
//..
//  int negotiate(bteso_StreamSocket<bteso_IPv4Address> *socket,
//                const bteso_Endpoint&                  destination)
//  {
//      btes5_Credentials credentials("john.smith", "PassWord123");
//..
// Next, we declare the variable for communicating the response, with a mutex
// and a condition variable to protect access to it from different threads.
//..
//      bcemt_Mutex     stateLock;
//      bcemt_Condition stateChanged;
//      volatile int    state = 0; // value > 0 indicates success, < 0 is error
//..
// Then we will create an event manager and a 'btes5_Negotiator' and start
// negotiation.
//..
//      btemt_TcpTimerEventManager eventManager;
//      assert(0 == eventManager.enable());
//      btes5_Negotiator negotiator(&eventManager);
//      using namespace bdef_PlaceHolders;
//      negotiator.negotiate(socket,
//                           destination,
//                           bdef_BindUtil::bind(socks5Callback,
//                                               _1,
//                                               _2,
//                                               &stateLock,
//                                               &stateChanged,
//                                               &state),
//                           bdet_TimeInterval(),
//                           credentials);
//..
// Next, we block until the negotiation ends and 'socks5Callback' updates
// the 'state' variable.
//..
//      bcemt_LockGuard<bcemt_Mutex> lock(&stateLock);
//      while (!state) {
//          stateChanged.wait(&stateLock);
//      }
//..
// Finally, we return the status of SOCKS5 negotiation.  If '0 < state',
// 'socket' can be used to communicate with 'destination' through the proxy.
//..
//      return state;
//  }
//..
//
///Example 2: Connection With Dynamically Acquired Credentials
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The username and password are not available ahead of time; instead, the
// negotiator will use the provided object to acquire them, possibly by
// prompting the user. We'll assume 'socks5Callback' is already defined.
//
// First, we define a class derived from 'btes5_CredentialsProvider' which will
// provide username and password.
//..
//  class MyCredentialsProvider : public btes5_CredentialsProvider {
//    public:
//      virtual void acquireCredentials(
//          const bteso_Endpoint&                                  proxy,
//          btes5_CredentialsProvider::SuppliedCredentialsCallback callback);
//      virtual void cancelAcquiringCredentials();
//  //
//  };
//  void MyCredentialsProvider::acquireCredentials(
//          const bteso_Endpoint&                                  proxy,
//          btes5_CredentialsProvider::SuppliedCredentialsCallback callback)
//  {
//      // normally we might prompt the user for username and password, but
//      // here we use hard-coded values.
//      callback(0, "User", "Password");
//  }
//  void MyCredentialsProvider::cancelAcquiringCredentials()
//  {
//      // we would normally cancel the user prompting process, if one is in
//      // progress; but here it's a no-op.
//  }
//  int negotiateWithAcquiredCredentials(
//      bteso_StreamSocket<bteso_IPv4Address> *socket,
//      const bteso_Endpoint&                  destination)
//  {
//..
// Finally we will create a 'btes5_Negotiator' and start negotiation, passing
// the address of a credentials provider object.
//..
//      MyCredentialsProvider credentialsProvider;
//      bcemt_Mutex     stateLock;
//      bcemt_Condition stateChanged;
//      volatile int    state = 0; // value > 0 indicates success, < 0 is error
//      btemt_TcpTimerEventManager eventManager;
//      assert(0 == eventManager.enable());
//      btes5_Negotiator negotiator(
//          reinterpret_cast<bteso_TimerEventManager*>(&eventManager));
//      using namespace bdef_PlaceHolders;
//      negotiator.negotiate(socket,
//                           destination,
//                           bdef_BindUtil::bind(socks5Callback,
//                                               _1,
//                                               _2,
//                                               &stateLock,
//                                               &stateChanged,
//                                               &state),
//                           bdet_TimeInterval(),
//                           &credentialsProvider);
//      bcemt_LockGuard<bcemt_Mutex> lock(&stateLock);
//      while (!state) {
//          stateChanged.wait(&stateLock);
//      }
//      return state;
//  }
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

#ifndef INCLUDED_BCEMA_WEAKPTR
#include <bcema_weakptr.h>
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

struct btes5_Negotiation_Imp;

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
        e_ERROR = -1      // any other error
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
    bteso_TimerEventManager    *d_eventManager_p; // socket event manager, held
    bcema_WeakPtr<btes5_Negotiation_Imp>  d_negotiation;    // negotiation in progress
    bslma::Allocator           *d_allocator_p;    // memory allocator, held

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

    void cancelNegotiation();
        // Cancel the negotiation in progress.  If there is no negotiation in
        // progress, this call has no effect.  No callback will be invoked as a
        // result of this call. Note that callbacks already in progress when
        // this method is called may proceed and invoke the 'callback' supplied
        // to 'negotiate'.

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
