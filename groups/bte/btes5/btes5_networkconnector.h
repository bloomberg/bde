// btes5_networkconnector.h                                           -*-C++-*-
#ifndef INCLUDED_BTES5_NETWORKCONNECTOR
#define INCLUDED_BTES5_NETWORKCONNECTOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide mechanism to connect to a destination host through one or
// more SOCKS5 proxies.
//
//@CLASSES:
//   btes5_NetworkConnector: establish connections through SOCKS5 hosts
//
//@SEE ALSO: btes5_negotiator
//
//@DESCRIPTION: This component provides a mechanism class,
// 'btes5_NetworkConnector', which establishes connections through proxy hosts
// using the SOCKS5 protocol. The connections are established asynchronously,
// with status reported to client-supplied callback. Each connection attempt is
// identifed by a 'AttempAttempt', which can be used to cancel the attempt.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Connect to a server through two proxy levels
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We want to connect to a server reachable through two levels of proxies:
// first through one of corporate SOCKS5 servers, and then through one of
// regional SOCKS5 servers.
//..
//  void connectCb(btes5_NetworkConnector::ConnectionStatus      status,
//                 bteso_StreamSocket<bteso_IPv4Address>        *socket,
//                 bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
//                 const btes5_DetailedError&                    error)
//  {
//      if (0 == status) {
//          cout << "connection succeeded" << endl;
//          // Success: conduct I/O operations with 'socket'
//          // ... and eventually deallocate it.
//          socketFactory->deallocate(socket);
//      } else {
//          cout << "Connect failed status=" << status
//               << " detail=" << error
//               << endl;
//      }
//  }
//  static int connectThroughProxies(const bteso_Endpoint& corpProxy1,
//                                   const bteso_Endpoint& corpProxy2,
//                                   const bteso_Endpoint& regionProxy1,
//                                   const bteso_Endpoint& regionProxy2)
//  {
//      btes5_NetworkDescription proxies;
//      proxies.addProxy(0, corpProxy1);
//      proxies.addProxy(0, corpProxy2);
//      proxies.addProxy(1, regionProxy1);
//      proxies.addProxy(1, regionProxy2);
//
//      btes5_NetworkDescriptionUtil::setAllCredentials(&proxies,
//                                                      "john.smith",
//                                                      "pass1");
//      btemt_TcpTimerEventManager eventMgr;
//      bteso_InetStreamSocketFactory<bteso_IPv4Address> socketFactory;
//      btes5_NetworkConnector connector(proxies, &socketFactory, &eventMgr);
//
//      bdet_TimeInterval timeout(30.0);
//      using namespace bdef_PlaceHolders;
//      eventMgr.start();
//      const bteso_Endpoint dst("destination.example.com", 8194);
//      btes5_NetworkConnector::AttemptHandle
//          attempt = connector.makeAttemptHandle(connectCb, timeout, dst);
//      attempt.startAttempt();
//  }

#ifndef INCLUDED_BTES5_CREDENTIALSPROVIDER
#include <btes5_credentialsprovider.h>
#endif

#ifndef INCLUDED_BTES5_DETAILEDERROR
#include <btes5_detailederror.h>
#endif

#ifndef INCLUDED_BTES5_NETWORKDESCRIPTION
#include <btes5_networkdescription.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BTESO_ENDPOINT
#include <bteso_endpoint.h>
#endif

#ifndef INCLUDED_BTESO_STREAMSOCKET
#include <bteso_streamsocket.h>
#endif

#ifndef INCLUDED_BTESO_STREAMSOCKETFACTORY
#include <bteso_streamsocketfactory.h>
#endif

#ifndef INCLUDED_BTEMT_TCPTIMEREVENTMANAGER
#include <btemt_tcptimereventmanager.h>
#endif

namespace BloombergLP {

                        // ============================
                        // class btes5_NetworkConnector
                        // ============================
class btes5_NetworkConnector {
    // This class supports a connection to the destination TCP server using one
    // or more levels of SOCKS5 proxies to reach the destination. At least one
    // of the proxies in the first level must be reachable from the source;
    // with each successive level providing connectivity to a proxy in the next
    // level, and finally to the destination. If one of the SOCKS5 servers
    // requires username/password authentication, and the credentials were
    // supplied at construction (statically or through a credentials provider)
    // then they will be used to attempt authentication. If a SOCKS5 server
    // requires a different authentication method, or it requires
    // username/password and none were supplied, the negotiation will fail with
    // 'status == e_AUTHENTICATION'.
    //
    // A 'btes5_NetworkConnector' object allows multiple connection attempts. A
    // connection attempt is initiated by a 'makeAttemptHandle()' call which
    // returns a 'ConnectionHandle', and started by 'startAttempt()'. The
    // 'ConnectionHandle' object can be used to cancel the connection attempt.

    // PRIVATE TYPES
    class Connector; // persistent state of the network connector
    class Attempt;   // state of a connection attempt

public:
    // TYPES

    enum ConnectionStatus {
        e_SUCCESS = 0,    // connection successfully established
        e_TIMEOUT,        // connection attempt timed out
        e_AUTHENTICATION, // no acceptable authentication methods
        e_CANCEL,         // connection was cancelled
        e_ERROR           // any other error
    };

    typedef bdef_Function<void (*)(
            btes5_NetworkConnector::ConnectionStatus      status,
            bteso_StreamSocket<bteso_IPv4Address>        *socket,
            bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
            const btes5_DetailedError&                    error)>
        ConnectionStateCallback;
        // A callback of this type is invoked when the 'btes5_NetworkConnector'
        // object establishes a connection or fails. If the specified 'status'
        // is zero, use the specified 'socket' for communication, and the
        // specified 'socketFactory' for eventual deallocation. Otherwise,
        // process connection failure as described by the specified 'error'.

    typedef bcema_SharedPtr<Attempt> AttemptHandle;
        // A 'AttemptHandle' object can be used to cancel a connection attempt
        // in progress.

private:
    // DATA
    bcema_SharedPtr<Connector> d_connector; // persistent state

    // NOT IMPLEMENTED
    btes5_NetworkConnector(const btes5_NetworkConnector&);
    btes5_NetworkConnector& operator=(const btes5_NetworkConnector&);

public:
    // CREATORS
    btes5_NetworkConnector(
        const btes5_NetworkDescription&               socks5Servers,
        bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
        btemt_TcpTimerEventManager                   *eventManager,
        bslma::Allocator                             *allocator = 0);
    btes5_NetworkConnector(
        const btes5_NetworkDescription&               socks5Servers,
        bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
        btemt_TcpTimerEventManager                   *eventManager,
        int                                           minSourcePort,
        int                                           maxSourcePort,
        bslma::Allocator                             *allocator = 0);
    btes5_NetworkConnector(
        const btes5_NetworkDescription&               socks5Servers,
        bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
        btemt_TcpTimerEventManager                   *eventManager,
        btes5_CredentialsProvider                    *provider,
        bslma::Allocator                             *allocator = 0);
    btes5_NetworkConnector(
        const btes5_NetworkDescription&               socks5Servers,
        bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
        btemt_TcpTimerEventManager                   *eventManager,
        int                                           minSourcePort,
        int                                           maxSourcePort,
        btes5_CredentialsProvider                    *provider,
        bslma::Allocator                             *allocator = 0);
        // Create a 'btes5_NetworkConnector' object that will use the specified
        // 'socks5Servers' to connect to a TCP server. Use the specified
        // 'eventManager' for timeout and socket operations. Optionally specify
        // 'minSourcePort' and 'maxSourcePort' to bind the source port to a
        // value in this range. Optionally specify 'provider' to acquire
        // credentials when required by a proxy server and if they are not
        // predefined in 'socks5Servers'. If the optionally specified
        // 'allocator' is not 0 use it to allocate memory, otherwise, use the
        // default allocator. The behavior is undefined unless 'socks5Servers'
        // has at least one level of proxies, and each level is non-empty.

    ~btes5_NetworkConnector();
        // Destroy this object. Established connections are not closed.

    // MANIPULATORS
    AttemptHandle makeAttemptHandle(const ConnectionStateCallback& callback,
                                    const bdet_TimeInterval&       timeout,
                                    const bteso_Endpoint&          server);
        // Return a 'AttemptHandle' object that can be used to asynchronously
        // connect to the specified 'server'; the specified 'callback' will be
        // invoked with connection status. If the specified 'timeout' is not
        // empty, a successful connection must occur within that time. The
        // handle can be used to start the connection attempt by calling
        // 'startAttempt()' and cancel the attempt by calling
        // 'cancelAttempt()'.

    void startAttempt(AttemptHandle& connectionAttempt);
        // Start the specified 'connectionAttempt'.

    void cancelAttempt(AttemptHandle& connectionAttempt);
        // Cancel the specified 'connectionAttempt'. Further invocation of the
        // associated callback is disabled, but this function does *not*
        // synchronize with callback invocation (i.e. an invocation initiated
        // before the call to this function may execute after the return from
        // this function).

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
