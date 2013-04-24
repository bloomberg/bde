// btemt_socks5connector.h                                            -*-C++-*-
#ifndef INCLUDED_BTEMT_SOCKS5CONNECTOR
#define INCLUDED_BTEMT_SOCKS5CONNECTOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an asynchronous connector (with timeout) to TCP-based
//  servers using one or more SOCKS5 proxy servers.
//
//@CLASSES:
//  apiso::Socks5Connector: asynchronous TCP connector using SOCKS5
//
//@SEE_ALSO: btemt::Socks5Negotiator
//
//@DESCRIPTION: This component allows connection to a TCP server through one or
// more SOCKS5 proxy servers. Each level of proxy may have more than one proxy
// servers, with each one used in turn in case of failure. It allows a timeout
// to be specified, in which case the connection must occur within the
// specified time interval. The source port for the connection to the socks5
// server is configurable to be within a specified range. The connection
// success/failure notification is done via asynchronous callback from the user
// specified TcpTimerEventManager.
//
///Asynchronous connect
///--------------------
// This connector supports an asynchronous connect mechanism. On a successful
// connection attempt, the user-defined callback is invoked with zero status
// and a 'bteso_StreamSocket<bteso_IPv4Address>'. If a connection attempt does
// not succeed (either due to timeout or other failure), the user-defined
// callback is invoked with a non-zero status. For non-timeout related failures
// a detailed error code is provided in the callback. This detailed error code
// should only be used for diagnostic and troubleshooting purposes.
//
// The proxy servers can be specified as hostnames; and these hostnames are
// resolved at connect time. The motiviation for connect-time resolution is
// that host addresses can change during the potentially long time of the
// lifetime of the object.
//
///Thread Safety
///-------------
// This connector is *thread-enabled* meaning that any operation on the same
// instance can be invoked from any thread.
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
//
// First we define a callback function to process connection status.
//..
//  void connectCb(int                                     status,
//                    bteso_StreamSocket< bteso_IPv4Address> *socket)
//  {
//      if (0 == status) {
//          cout << "connection succeeded" << endl;
//          // Success: conduct I/O operations with 'socket', or use
//          // btemt_SessionPool::import to import it into a session pool.
//          //TODO: signal success to main function
//      } else {
//          cerr << "Connect failed: "
//               << btemt::Socks5Connector::errorToString(status)
//               << endl;
//      }
//  }
//..
// Then we define the level of proxies that should be reachable directory.
//..
//  static void connectThroughProxies()
//  {
//      btemt::Socks5Connector::ProxyLevel corpProxies;
//      corpProxies.push_back(HostPort("proxy1.corp.com", 1080));
//      corpProxies.push_back(HostPort("proxy2.corp.com", 1080));
//..
// Next we add a layer for regional proxies reachable from the corporate
// proxies. Note that .tk stands for Tokelau in the Pacific Ocean.
//..
//      btemt::Socks5Connector::ProxyLevel regionProxies;
//      corpProxies.push_back(HostPort("proxy1.example.tk", 1080));
//      corpProxies.push_back(HostPort("proxy2.example.tk", 1080));
//      bsl::vector<btemt::Socks5Connector::ProxyLevel> socks5Servers;
//      socks5Servers.push_back(corpProxies);
//      socks5Servers.push_back(regionProxies);
//..
// Then we set configuration parameters, including the user name and password
// which will be used in case one of the proxies in the connection path
// requires that type of authentication.
//..
//      SocksConfiguration configuration;
//      configuration.username() = "john.smith";
//      configuration.password() = "pass1";
//..
// Next we construct a 'Socks5Connector' which will be used to connect to one
// or more destinations. We don't care which local port we are bound to, and
// specify the total timeout of 30 seconds.
//..
//      int minSourcePort = 0;
//      int maxSourcePort = 0;
//      int timeoutSeconds = 30;
//      btemt_TcpTimerEventManager timerEventManager;
//      Socks5Connector connector(socks5Servers,
//                                configuration,
//                                minSourcePort,
//                                maxSourcePort,
//                                timeoutSeconds,
//                                &timerEventManager);
//..
// Finally we enable the event manager and attempt to connect to the
// destination. Input, output and eventual closing of the connection will be
// handled from 'connectCb'.
//..
//      timerEventManager.enable();
//      connector.connect(&connectCb, HostPort("destination.example.com", 8194));
//  }

#ifndef INCLUDED_BTEMT_HOSTPORT
#include <btemt_hostport.h>
#endif

#ifndef INCLUDED_BTEMT_SOCKSCONFIGURATION
#include <btemt_socksconfiguration.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BTESO_IPV4ADDRESS
#include <bteso_ipv4address.h>
#endif

#ifndef INCLUDED_BTESO_STREAMSOCKET
#include <bteso_streamsocket.h>
#endif

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif

#ifndef INCLUDED_STRING
#include <string>
#define INCLUDED_STRING
#endif

#ifndef INCLUDED_UTILITY
#include <utility>
#define INCLUDED_UTILITY
#endif

namespace BloombergLP {

class btemt_TcpTimerEventManager;
namespace bslma { class Allocator; }
class bcemt_Mutex;

namespace btemt {

// FORWARD DECLARATIONS
struct ConnectInfo;
struct ConnectRequest;

                        // =====================
                        // class Socks5Connector
                        // =====================

class Socks5Connector {
    // This class supports a connection to the destination TCP server using one
    // or more levels of SOCKS5 proxies to reach the destination. At least one
    // of the proxies in the first level must be reachable from the source;
    // with each successive level providing connectivity to a proxy in the next
    // level, and finally to the destination.

public:
    // TYPES

    enum Status {
        SOCKS5CONNECTOR_SUCCESS = 0,
        SOCKS5CONNECTOR_TIMEOUT,
        SOCKS5CONNECTOR_METHOD_REQUEST_WRITE,
        SOCKS5CONNECTOR_REGISTER_READ,
        SOCKS5CONNECTOR_METHOD_RESPONSE_READ,
        SOCKS5CONNECTOR_UNSUPPORTED_METHOD,
        SOCKS5CONNECTOR_CONNECT_REQUEST_WRITE,
        SOCKS5CONNECTOR_CONNECT_RESPONSE_READ,
        SOCKS5CONNECTOR_CONNECT_SOCKS_SERVER_FAILURE,
        SOCKS5CONNECTOR_CONNECT_CONNECTION_NOT_ALLOWED,
        SOCKS5CONNECTOR_CONNECT_NETWORK_UNREACHABLE,
        SOCKS5CONNECTOR_CONNECT_HOST_UNREACHABLE,
        SOCKS5CONNECTOR_CONNECT_CONNECTION_REFUSED,
        SOCKS5CONNECTOR_CONNECT_TTL_EXPIRED,
        SOCKS5CONNECTOR_CONNECT_COMMAND_NOT_SUPPORTED,
        SOCKS5CONNECTOR_CONNECT_ADDRESS_TYPE_NOT_SUPPORTED,
        SOCKS5CONNECTOR_CONNECT_OTHER
    };

    typedef bdef_Function<void (*)(
            int                                    status,
            bteso_StreamSocket<bteso_IPv4Address> *socket)> ConnectCallback;
        // This type of function will be called when the 'Socks5Connector'
        // object establishes a connection or fails. If the specified 'status'
        // is zero, use the specified 'socket' for communication. Otherwise,
        // process connection failure with one of the non-zero values of
        // 'Status' indicating the cause.

    typedef bsl::vector<HostPort> ProxyLevel;
        // Group of SOCKS5 proxy servers at one level. Proxy connections will
        // be attempted in the order specified in the vector.

private:
    int                              d_minSourcePort;
    int                              d_maxSourcePort;
    int                              d_nextBindPort;
    int                              d_timeoutSeconds;
    std::vector<ProxyLevel>          d_socks5Servers;
    btemt_TcpTimerEventManager      *d_eventManager_p;
    bslma::Allocator                *d_allocator_p;

    // PRIVATE MANIPULATORS
    void tcpConnectImpl(const bcema_SharedPtr<ConnectInfo> &connInfo);

    void processFailureImpl(
            const bcema_SharedPtr<ConnectRequest>   &request,
            const bcema_SharedPtr<ConnectInfo>      &connInfo,
            int                                      status,
            int                                      detailedError,
            bcemt_Mutex                             *mutex_p);
        // Process a failure for the specified 'request', retry if possible
        // otherwise callback the user with failure code 'detailedError'
        // If the specified 'mutex_p' is not null, then it must have been locked
        // by the calling function.

    void timeoutCallback(
            const bcema_SharedPtr<ConnectRequest>   &request,
            const bcema_SharedPtr<ConnectInfo>      &connInfo);
        // process time out for the specified 'request'

    void tcpConnectCallback(
            const bcema_SharedPtr<ConnectInfo>      &connInfo,
            int                                      status,
            int                                      detailedError,
            bteso_SocketHandle::Handle               handle);

    void readMethodResponseCallback(
            const bcema_SharedPtr<ConnectRequest>   &request,
            const bcema_SharedPtr<ConnectInfo>      &connInfo);
        // process response to socks5 method request

    void readConnectResponseCallback(
            const bcema_SharedPtr<ConnectRequest>   &request,
            const bcema_SharedPtr<ConnectInfo>      &connInfo);
        // process response to socks5 connect request

public:
    // CLASS METHODS
    static const char *errorToString(int status);
        // Produce a text error string corresponding to the specified 'status'.

    // CREATORS
    Socks5Connector(const bsl::vector<ProxyLevel>   &socks5Servers,
                    const SocksConfiguration&        configuration,
                    int                              minSourcePort,
                    int                              maxSourcePort,
                    int                              timeoutSeconds,
                    btemt_TcpTimerEventManager      *timerEventManager,
                    bslma::Allocator                *basicAllocator = 0);
        // Create a 'Socks5Connector' object that will use the specified
        // 'socks5Servers' and 'configuration' to connect to a TCP-based
        // server. If 'minSourcePort' and 'maxSourcePort' are both non-zero,
        // bind the source port to a value in this range. Use the specified
        // 'timerEventManager' for event management and timing. If
        // 'timeoutSeconds' is positive, a successful connection must occur
        // within that time interval. If the optionally specified
        // 'basicAllocator' is not 0 use it to allocate memory, otherwise, use
        // the default allocator.

    ~Socks5Connector();

    // MANIPULATORS
    void connect(const ConnectCallback      &callback,
                 const HostPort             &server);
        // Asynchronously connect to the specified 'server'.

};

}  // close package namespace
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
