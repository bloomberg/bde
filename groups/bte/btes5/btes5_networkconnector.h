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
//   btes5_NetworkConnector
//
//@SEE ALSO:
//
//@DESCRIPTION: This component provides ...
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
//  void connectCb(btes5_NetworkConnector::Status                status,
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
//                                   const bteso_Endpoint& corpProxy2)
//  {
//      btemt::btes5_NetworkDescription proxies;
//      proxies.addProxy(0, corpProxy1);
//      proxies.addProxy(0, corpProxy2);
//      proxies.addProxy(1, bteso_Endpoint("proxy1.example.tk", 1080));
//      proxies.addProxy(1, bteso_Endpoint("proxy2.example.tk", 1080));
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
//      connector.connect(connectCb,
//                        timeout,
//                        bteso_Endpoint("destination.example.com", 8194));
//  }

#ifndef INCLUDED_BTES5_CREDENTIALSPROVIDER
#include <btes5_credentialsprovider.h>
#endif

#ifndef INCLUDED_BTES5_NETWORKDESCRIPTION
#include <btes5_networkdescription.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BTESO_ENDPOINT
#include <bteso_endpoint.h>
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
    // 'status == BTES5_AUTHENTICATION'.

public:
    // TYPES

    enum Status {
        BTES5_SUCCESS = 0,    // connection successfully established
        BTES5_TIMEOUT,        // connection attempt timed out
        BTES5_AUTHENTICATION, // no acceptable authentication methods
        BTES5_ERROR           // any other error
    };

    typedef bdef_Function<void (*)(
            Status                                        status,
            bteso_StreamSocket<bteso_IPv4Address>        *socket,
            bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
            btes5_DetailedError&                          error)>
        ConnectCallback;
        // A callback of this type is invoked when the 'btes5_NetworkConnector'
        // object establishes a connection or fails. If the specified 'status'
        // is zero, use the specified 'socket' for communication, and the
        // specified 'socketFactory' for eventual deallocation. Otherwise,
        // process connection failure as described by the specified 'error'.

private:
    // DATA
    int                                           d_minSourcePort;
    int                                           d_maxSourcePort;
    btes5_NetworkDescription                      d_socks5Servers;
    bteso_StreamSocketFactory<bteso_IPv4Address> *d_socketFactory_p; // held
    btemt_TcpTimerEventManager                   *d_eventManager;    // held
    bslma::Allocator                             *d_allocator_p;     // held

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
        // Destroy this object.

    // MANIPULATORS
    void connect(const ConnectCallback&   callback,
                 const bdet_TimeInterval& timeout,
                 const bteso_Endpoint&    server);
        // Asynchronously connect to the specified 'server'; the specified
        // 'callback' will be invoked with connection status. If the specified
        // 'timeout' is not empty, a successful connection must occur within
        // that time.

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
