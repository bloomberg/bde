// btls5_networkconnector.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLS5_NETWORKCONNECTOR
#define INCLUDED_BTLS5_NETWORKCONNECTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism to connect through SOCKS5 proxies.
//
//@CLASSES:
//  btls5::NetworkConnector: mechanism to connect via SOCKS5 hosts
//
//@SEE_ALSO: btls5_negotiator
//
//@DESCRIPTION: This component provides a mechanism class,
// 'btls5::NetworkConnector', that establishes connections through proxy hosts
// using the SOCKS5 protocol.  The connections are established asynchronously,
// with status reported to a client-supplied callback.  Each connection attempt
// is identified by a 'ConnectionAttemptHandle', which can be used to start the
// attempt as well as cancel it.  The client code should call
// 'makeConnectionAttemptHandle', which returns a 'ConnectionAttemptHandle',
// followed by 'startConnectionAttempt' to initiate the connection attempt.
//
// After initiating the attempt the client can wait until the attempt is
// concluded and the callback is invoked, or cancel the attempt by calling
// 'cancelConnectionAttempt'.  Note that the callback may still be invoked
// after 'cancelConnectionAttempt' is called.
//
// Note that on MS Windows, 'btlso::SocketImpUtil::startup' must be called to
// initialize the socket environment before calling
// 'btls5::NetworkConnector::startConnectionAttempt'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Connect to a Server Through Two Proxy Levels
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to connect to a server reachable through two levels of
// proxies: first through one of our corporate SOCKS5 servers, and then through
// one of the regional SOCKS5 servers.
//
// First, we define a callback function to process connection status, and if
// successful, perform useful work and finally deallocate the socket.  After
// the work is done (or an error is reported) we signal the main thread with
// the status; this also signifies that we no longer need the stream factory
// passed to us:
//..
//  void connectCb(
//              int                                             status,
//              btlso::StreamSocket< btlso::IPv4Address>       *socket,
//              btlso::StreamSocketFactory<btlso::IPv4Address> *socketFactory,
//              const btls5::DetailedStatus&                    detailedStatus,
//              bslmt::Mutex                                   *stateLock,
//              bslmt::Condition                               *stateChanged,
//              volatile int                                   *state)
//  {
//      if (0 == status) {
//          // Success: conduct I/O operations with 'socket' ... and deallocate
//
//          socketFactory->deallocate(socket);
//      } else {
//          cout << "Connect failed " << status << ": " << detailedStatus
//               << endl;
//      }
//      bslmt::LockGuard<bslmt::Mutex> lock(stateLock);
//      *state = status ? -1 : 1; // 1 for success, -1 for failure
//      stateChanged->signal();
//  }
//..
// Then, we define the function that will set up the proxy network description
// and attempt to connect to a remote host.  The first level of proxies should
// be reachable directly:
//..
//  static int connectThroughProxies(const btlso::Endpoint& corpProxy1,
//                                   const btlso::Endpoint& corpProxy2)
//  {
//      btls5::NetworkDescription proxies;
//      proxies.addProxy(0, corpProxy1);
//      proxies.addProxy(0, corpProxy2);
//..
// Next, we add a level for regional proxies reachable from the corporate
// proxies.  Note that '.tk' stands for Tokelau in the Pacific Ocean:
//..
//      proxies.addProxy(1, btlso::Endpoint("proxy1.example.tk", 1080));
//      proxies.addProxy(1, btlso::Endpoint("proxy2.example.tk", 1080));
//..
// Then, we set the username and password, which will be used in case one of
// the proxies in the connection path requires that type of authentication:
//..
//      btls5::Credentials credentials("John.smith", "pass1");
//      btls5::NetworkDescriptionUtil::setAllCredentials(&proxies,
//                                                       credentials);
//..
// Now, we construct a 'btls5::NetworkConnector' that will be used to connect
// to one or more destinations:
//..
//      btlso::InetStreamSocketFactory<btlso::IPv4Address> factory;
//      btlmt::TcpTimerEventManager eventManager;
//      eventManager.enable();
//      btls5::NetworkConnector connector(proxies, &factory, &eventManager);
//..
// Finally, we attempt to connect to the destination.  Input, output, and
// eventual closing of the connection will be handled from 'connectCb', which
// will signal by changing 'state', with the access protected by a mutex and
// condition variable:
//..
//      const bsls::TimeInterval proxyTimeout(5.0);
//      const bsls::TimeInterval totalTimeout(30.0);
//      bslmt::Mutex     stateLock;
//      bslmt::Condition stateChanged;
//      volatile int     state = 0; // value > 0 is success and < 0 is error
//      using namespace bdlf::PlaceHolders;
//      btls5::NetworkConnector::ConnectionAttemptHandle attempt =
//         connector.makeConnectionAttemptHandle(bdlf::BindUtil::bind(
//                                                            connectCb,
//                                                            _1, _2, _3, _4,
//                                                            &stateLock,
//                                                            &stateChanged,
//                                                            &state),
//                                               proxyTimeout,
//                                               totalTimeout,
//                                               btlso::Endpoint(
//                                                   "destination.example.com",
//                                                   8194));
//      connector.startConnectionAttempt(attempt);
//      bslmt::LockGuard<bslmt::Mutex> lock(&stateLock);
//      while (!state) {
//          stateChanged.wait(&stateLock);
//      }
//      return state;
//  }
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLS5_DETAILEDSTATUS
#include <btls5_detailedstatus.h>
#endif

#ifndef INCLUDED_BTLS5_NETWORKDESCRIPTION
#include <btls5_networkdescription.h>
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

#ifndef INCLUDED_BTLSO_STREAMSOCKETFACTORY
#include <btlso_streamsocketfactory.h>
#endif

#ifndef INCLUDED_BDLT_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
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

namespace btls5 {

                           // ======================
                           // class NetworkConnector
                           // ======================

class NetworkConnector {
    // This class supports a connection to a destination TCP server using one
    // or more levels of SOCKS5 proxies to reach the destination.  At least one
    // of the proxies in the first level must be reachable from the source;
    // with each successive level providing connectivity to a proxy in the next
    // level, and finally to the destination.  If one of the SOCKS5 servers
    // requires username/password authentication, and the credentials were
    // supplied at construction, then they will be used to attempt
    // authentication.  If a SOCKS5 server requires a different authentication
    // method, or it requires username/password and none were supplied, the
    // negotiation will fail with 'status == e_AUTHENTICATION'.
    //
    // A 'NetworkConnector' object allows multiple concurrent connection
    // attempts.  A connection attempt is initiated by calling
    // 'makeConnectionAttemptHandle' to obtain a 'ConnectionAttemptHandle' that
    // is subsequently passed to 'startConnectionAttempt'.  The
    // 'ConnectionAttemptHandle' object can also be used to cancel the
    // connection attempt.

    // PRIVATE TYPES
    class Connector;
        // persistent state of the SOCKS5 network connector

    class ConnectionAttempt;
        // state of a connection attempt

  public:
    // TYPES
    enum ConnectionStatus {
        e_SUCCESS = 0,    // connection successfully established
        e_TIMEOUT,        // connection attempt timed out
        e_AUTHENTICATION, // no acceptable authentication methods
        e_CANCEL,         // connection was canceled
        e_ERROR           // any other error
    };

    typedef bsl::function<void(
               NetworkConnector::ConnectionStatus              status,
               btlso::StreamSocket<btlso::IPv4Address>        *socket,
               btlso::StreamSocketFactory<btlso::IPv4Address> *socketFactory,
               const DetailedStatus&                           detailedStatus)>
                                                       ConnectionStateCallback;
        // A callback of this type is invoked when the 'NetworkConnector'
        // object establishes a connection or fails.  If the specified 'status'
        // is zero, use the specified 'socket' for communication, and the
        // specified 'socketFactory' for eventual deallocation.  Otherwise,
        // process connection failure as described by the specified
        // 'detailedStatus'.

    typedef bsl::shared_ptr<ConnectionAttempt> ConnectionAttemptHandle;
        // A 'ConnectionAttemptHandle' object can be used to start a connection
        // attempt, as well as cancel it.

  private:
    // DATA
    bsl::shared_ptr<Connector> d_connector; // persistent state

  private:
    // NOT IMPLEMENTED
    NetworkConnector(const NetworkConnector&);
    NetworkConnector& operator=(const NetworkConnector&);

  public:
    // CREATORS
    NetworkConnector(
           const NetworkDescription&                       socks5Servers,
           btlso::StreamSocketFactory<btlso::IPv4Address> *socketFactory,
           btlmt::TcpTimerEventManager                    *eventManager,
           bslma::Allocator                               *basicAllocator = 0);
    NetworkConnector(
           const NetworkDescription&                       socks5Servers,
           btlso::StreamSocketFactory<btlso::IPv4Address> *socketFactory,
           btlmt::TcpTimerEventManager                    *eventManager,
           int                                             minSourcePort,
           int                                             maxSourcePort,
           bslma::Allocator                               *basicAllocator = 0);
        // Create a 'NetworkConnector' object that will use the specified
        // 'socks5Servers' to connect to a TCP server, allocating and
        // deallocating sockets using the specified 'socketFactory'.  Use the
        // specified 'eventManager' for timeout and socket operations.
        // Optionally specify 'minSourcePort' and 'maxSourcePort' to bind the
        // source port to a value in this range.  If 'minSourcePort' and
        // 'maxSourcePort' are not specified, the system will assign the source
        // port.  Optionally specify a 'basicAllocator' used to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.  The behavior is undefined unless
        // 'NetworkDescriptionUtil::isWellFormed(socks5Servers)' is 'true',
        // and, for the second signature, '1 <= minSourcePort' and
        // 'minSourcePort <= maxSourcePort' and 'maxSourcePort <= 65535'.

    //! ~NetworkConnector() = default;
        // Destroy this object.  Connection attempts in progress will continue,
        // and established connections are not closed.

    // MANIPULATORS
    ConnectionAttemptHandle makeConnectionAttemptHandle(
                                   const ConnectionStateCallback& callback,
                                   const bsls::TimeInterval&      proxyTimeout,
                                   const bsls::TimeInterval&      totalTimeout,
                                   const btlso::Endpoint&         server);
        // Return a 'ConnectionAttemptHandle' object that can be used to
        // asynchronously connect to the specified 'server'; the specified
        // 'callback' will be invoked with connection status.  If the specified
        // 'proxyTimeout' is non-zero, a successful connection attempt to each
        // proxy must occur within that time; otherwise individual proxy
        // connections will not time out.  If the specified 'totalTimeout' is
        // non-zero, a successful connection must occur within that time;
        // otherwise the connection attempt will not time out.  Note that the
        // handle can be used to start the connection attempt by calling
        // 'startConnectionAttempt' and cancel the attempt by calling
        // 'cancelConnectionAttempt'.

    void startConnectionAttempt(const ConnectionAttemptHandle& handle);
        // Start a connection attempt for the specified 'handle'.  If the
        // source port range was specified at construction and a port in that
        // range cannot be bound, use a system-assigned source port.  The
        // behavior is undefined unless the event manager supplied at
        // construction is enabled.

    void startConnectionAttemptStrict(const ConnectionAttemptHandle& handle);
        // Start a connection attempt for the specified 'handle'.  If the
        // source port range was specified at construction and a port in that
        // range cannot be bound, fail the connection attempt.  The behavior is
        // undefined unless the event manager supplied at construction is
        // enabled.

    void cancelConnectionAttempt(const ConnectionAttemptHandle& handle);
        // Cancel the connection attempt for the specified 'handle'.  Further
        // invocation of the associated callback is disabled, but this function
        // does *not* synchronize with callback invocation (i.e., an invocation
        // initiated before the call to this function may execute after the
        // return from this function).

    // ACCESSORS
    const NetworkDescription& socks5Servers() const;
        // Return a reference providing non-modifiable access to the internal
        // copy of the network description that was suplied at construction.
};

}  // close package namespace

// TRAITS
namespace bslma {

template<>
struct UsesBslmaAllocator<btls5::NetworkConnector> : bsl::true_type {
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
