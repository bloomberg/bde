// btes5_networkconnector.cpp                                         -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_socks5connector_cpp, "$Id$ $CSID$")

#include <btes5_networkconnector.h>
#include <btes5_testserver.h> // for testing only

#include <bcema_sharedptr.h>
#include <bdef_bind.h>
#include <bsls_atomic.h>
#include <bsls_platform.h>
#include <bsl_sstream.h>
#include <btemt_sessionpool.h> // for testing only
#include <btemt_tcptimereventmanager.h>
#include <bteso_inetstreamsocketfactory.h>
#include <bteso_ioutil.h>
#include <bteso_resolveutil.h>
#include <bteso_socketimputil.h>
#include <bteso_socketoptutil.h>

// 'btes5_NetworkConnector' implements asynchronous connection establishments.
// Because of that, callbacks related to IO and timeout events can be invoked
// after an associated connection has been cancelled, or, indeed, after the
// 'btes5_NetworkConnector' object has been destroyed. To avoid crashihng, the
// state referenced by the callbacks is allocated, with its lifetime controlled
// by bcema_SharedPtr.
//
// Two structures are used to maintain state: a 'Connector' for object
// variables such as the SOCKS5 network description, and an 'Attempt' for the
// state specific to one connection attempt.

namespace BloombergLP {

                              // ================
                              // struct Connector
                              // ================
struct btes5_NetworkConnector::Connector {
    // DATA
    int                                           d_minSourcePort;
    int                                           d_maxSourcePort;
        // if not 0, originating sockets will be bound to a port in this range

    btes5_NetworkDescription                      d_socks5Servers;
        // the network of proxy hosts

    bteso_StreamSocketFactory<bteso_IPv4Address> *d_socketFactory_p;
        // factory used to allocate sockets (not owned)

    btemt_TcpTimerEventManager *d_eventManager_p; // event manager, not owned

    btes5_CredentialsProvider *d_provider_p; // credentials provider, not owned

    bslma::Allocator *d_allocator_p; // memory allocator, not owned

    // CREATORS
    Connector(const btes5_NetworkDescription&               socks5Servers,
              bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
              btemt_TcpTimerEventManager                   *eventManager,
              int                                           minSourcePort,
              int                                           maxSourcePort,
              btes5_CredentialsProvider                    *provider,
              bslma::Allocator                             *allocator);
};

                              // ----------------
                              // struct Connector
                              // ----------------
// CREATORS
btes5_NetworkConnector::Connector::Connector(
                   const btes5_NetworkDescription&               socks5Servers,
                   bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
                   btemt_TcpTimerEventManager                   *eventManager,
                   int                                           minSourcePort,
                   int                                           maxSourcePort,
                   btes5_CredentialsProvider                    *provider,
                   bslma::Allocator                             *allocator)
: d_minSourcePort(minSourcePort)
, d_maxSourcePort(maxSourcePort)
, d_socks5Servers(socks5Servers)
, d_socketFactory_p(socketFactory)
, d_eventManager_p(eventManager)
, d_provider_p(provider)
, d_allocator_p(allocator)
{
    BSLS_ASSERT(eventManager);
    BSLS_ASSERT(socketFactory);
    d_eventManager_p->enable();
}

                            // ====================
                            // struct Attempt
                            // ====================
struct btes5_NetworkConnector::Attempt {
    // Objects describing the state of a SOCKS connection attempt. The object
    // lifetime is managed by using an 'bcema_SharedPtr<Attempt>' in callback
    // registration. An 'Attempt' object is constructed by
    // 'btes5_NetworkConnector::connect()' and persists until all callbacks
    // into which such object is bound are destroyed. An 'Attempt' object is in
    // one of two states: it's created in the normal state  indicated by
    // 'd_terminating == 0', and it enters a terminating state when
    // 'd_terminating != 0'.

    // TYPES
    typedef bcema_SharedPtr<Attempt> Context;
        // A shared pointer of this type is bound into the callback functors
        // for object lifetime management.

    // DATA
    const ConnectionStateCallback   d_callback;    // client callback
    const bdet_TimeInterval         d_timeout;     // expiration time
    const bteso_Endpoint            d_server;      // destination address
    bteso_SocketHandle::Handle      d_handle;      // OS-level socket

    bcema_SharedPtr<Connector>      d_connector;
        // persistent state of the connector associated with this attempt

    void                           *d_timerId_p;   // timer id, not owned
    bsls::AtomicInt                 d_terminating; // attempt being terminated

    bsl::vector<bsl::size_t>        d_indices;
        // sequence of 'd_connector->d_Socks5Servers.levelCount()' indices
        // indicating the next SOCKS5 server to try connecting to

    bteso_StreamSocket<bteso_IPv4Address> *d_socket_p;
        // socket for the connection to the first-level proxy, owned

    bslma::Allocator                      *d_allocator_p;
        // memory allocator, not owned

    // CREATORS
    Attempt(const ConnectionStateCallback&  callback,
            const bdet_TimeInterval&        timeout,
            const bteso_Endpoint&           destination,
            bcema_SharedPtr<Connector>&     connector,
            bslma::Allocator               *allocator);
        // Create an 'Attempt' object associated with the specified 'connector'
        // to connect to the specified 'destination' and asynchrounously invoke
        // the specified 'callback', using the specified 'allocator' to supply
        // memory.

    ~Attempt();
        // Destroy this object. Do not deallocate  or close 'd_socket_p'.

};

namespace {

static void terminate(
    const btes5_NetworkConnector::AttemptHandle& connectionAttempt,
    btes5_NetworkConnector::ConnectionStatus     status,
    const btes5_DetailedError&                   error)
    // Terminate the specified 'connectionAttempt' with the specified 'status'
    // and 'error'.
{
    if (connectionAttempt->d_terminating.testAndSwap(1, 1)) {
        return; // this attempt is already being terminated
    }
    if (btes5_NetworkConnector::e_SUCCESS == status) {
        connectionAttempt->d_callback(
                             status,
                             connectionAttempt->d_socket_p,
                             connectionAttempt->d_connector->d_socketFactory_p,
                             error);
    } else {
        if (connectionAttempt->d_socket_p) {
           connectionAttempt->d_connector->d_socketFactory_p
            ->deallocate(connectionAttempt->d_socket_p);
        }
        connectionAttempt->d_callback(
                             status,
                             connectionAttempt->d_socket_p,
                             connectionAttempt->d_connector->d_socketFactory_p,
                             error);
    }
    // TODO: cancel any negotiation in progress
}

static void tcpConnect(
    btes5_NetworkConnector::AttemptHandle& connectionAttempt,
    bsl::size_t index);
    // Establish a connection to the first-level proxy host in the specified
    // 'connectionAttempt' at or after the specified 'index'.

static void connectCallback(
    const btes5_NetworkConnector::AttemptHandle& connectionAttempt,
    bsl::size_t                                  index)
    // Process a connection event for a first-level proxy in the specified
    // 'connectionAttempt' identified by the specified 'index'.
{
    // TODO: check for terminating attempt

    btes5_NetworkConnector::AttemptHandle attempt(connectionAttempt);
        // copy shared ptr because deregisterSocket removes the reference

    attempt->d_connector->d_eventManager_p->deregisterSocket(
                                                attempt->d_socket_p->handle());
    int rc = attempt->d_socket_p->connectionStatus();
    if (rc) {
        if (index < attempt->d_connector->d_socks5Servers.numProxies(0)) {
            tcpConnect(attempt, index + 1);
        } else {
            btes5_DetailedError error(
                "Unable to connect to any proximate proxies",
                attempt->d_connector->d_socks5Servers.beginLevel(0)
                    [index].address());
            terminate(attempt, btes5_NetworkConnector::e_ERROR, error);
        }
    }
    // TODO: start negotiation on socket
}

static void tcpConnect(
    btes5_NetworkConnector::AttemptHandle& connectionAttempt,
    bsl::size_t index)
{
    btes5_DetailedError error("TCP connect");;

    connectionAttempt->d_socket_p
        = connectionAttempt->d_connector->d_socketFactory_p->allocate();
    bteso_StreamSocket<bteso_IPv4Address>
        *socket = connectionAttempt->d_socket_p;
    int rc = 0; // return code for socket operations

    // Note: to be compatible with bbcomm - only do this on unix
#ifndef BSLS_PLATFORM_OS_WINDOWS
    rc = socket->setOption(bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                           bteso_SocketOptUtil::BTESO_REUSEADDRESS,
                           0);
    if (rc) {
        error.setDescription("Unable to set REUSEADDR option");
        terminate(connectionAttempt, btes5_NetworkConnector::e_ERROR, error);
        return;                                                       // RETURN
    }
#endif

    if (connectionAttempt->d_connector->d_minSourcePort > 0) {
        const int begin = connectionAttempt->d_connector->d_minSourcePort;
        const int end = connectionAttempt->d_connector->d_maxSourcePort + 1;
        int port = begin;
        for (; end != port; port++) {
            bteso_IPv4Address
                srcAddress(bteso_IPv4Address::BTESO_ANY_ADDRESS, port);
            rc = socket->bind(srcAddress);
            if (!rc) {
                break;
            }
        }
        if (end == port) {
            bsl::ostringstream description;
            description << "Unable to bind source address a port between "
                        << begin << " " << end;
            terminate(connectionAttempt,
                      btes5_NetworkConnector::e_ERROR,
                      error);
            return;                                                   // RETURN
        }
    }
    rc = socket->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    if (rc) {
        error.setDescription("Unable to set socket mode to non-blocking");
        terminate(connectionAttempt, btes5_NetworkConnector::e_ERROR, error);
        return;                                                       // RETURN
    }

    bteso_SocketOptUtil::LingerData lingerData;
    lingerData.l_onoff = 1;     // enable lingering
    lingerData.l_linger = 0;    // 0 seconds
    rc = socket->setLingerOption(lingerData);
    if (rc) {
        error.setDescription("Unable to set linger option");
        terminate(connectionAttempt, btes5_NetworkConnector::e_ERROR, error);
        return;                                                       // RETURN
    }

    rc = socket->setOption( bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                            bteso_SocketOptUtil::BTESO_KEEPALIVE,
                            1);
    if (rc) {
        error.setDescription("Unable to set KEEPALIVE option");
        terminate(connectionAttempt, btes5_NetworkConnector::e_ERROR, error);
        return;                                                       // RETURN
    }

    rc = socket->setOption(bteso_SocketOptUtil::BTESO_TCPLEVEL,
                            bteso_SocketOptUtil::BTESO_TCPNODELAY,
                            1);
    if (rc) {
        error.setDescription("Unable to set TCPNODELAY option");
        terminate(connectionAttempt, btes5_NetworkConnector::e_ERROR, error);
        return;                                                       // RETURN
    }

    btes5_NetworkDescription::ProxyIterator
        it = connectionAttempt->d_connector->d_socks5Servers.beginLevel(0);
    it += index;
    const btes5_NetworkDescription::ProxyIterator
        end = connectionAttempt->d_connector->d_socks5Servers.endLevel(0);
    for (; end != it; it++, index++) {
        const bteso_Endpoint& destination = it->address();
        bteso_IPv4Address server;

        if (bteso_IPv4Address::isValid(destination.hostname().c_str()) == 0) {
            // if 'hostname' just an ip address use it
            server.setIpAddress(destination.hostname().c_str());
        } else {
            // otherwise look up the address
            int errorCode;
            int ret
                = bteso_ResolveUtil::getAddress(&server,
                                                destination.hostname().c_str(),
                                                &errorCode);
            if (ret != 0) {
                bsl::ostringstream description;
                description << "Unable to resolve " << destination.hostname()
                            << " error code " << errorCode;
                error.setDescription(description.str());
                continue;
            }
        }
        server.setPortNumber(destination.port());
        rc = socket->connect(server);
        if (!rc) {
            connectCallback(connectionAttempt, index);
        } else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            bteso_EventManager::Callback
                cb = bdef_BindUtil::bind(connectCallback,
                                         connectionAttempt,
                                         index);
            rc = connectionAttempt->d_connector
              ->d_eventManager_p->registerSocketEvent(socket->handle(),
                                                      bteso_EventType::CONNECT,
                                                      cb);
        } else {
            error.setDescription("Unable to conect");
            continue; // do not terminate because there may be more servers
        }
    }
    if (end == it) {
        terminate(connectionAttempt, btes5_NetworkConnector::e_ERROR, error);
        return;                                                       // RETURN
    }
}

}  // close unnamed namespace


                               // --------------
                               // struct Attempt
                               // --------------
// CREATORS
btes5_NetworkConnector::Attempt::Attempt(
    const ConnectionStateCallback&  callback,
    const bdet_TimeInterval&        timeout,
    const bteso_Endpoint&           server,
    bcema_SharedPtr<Connector>&     connector,
    bslma::Allocator               *allocator)
: d_callback(callback, allocator)
, d_timeout(timeout)
, d_server(server, allocator)
, d_connector(connector)
, d_timerId_p(0)
, d_indices(connector->d_socks5Servers.levelCount(), 0, allocator)
, d_socket_p(0)
, d_allocator_p(allocator)
{
}

btes5_NetworkConnector::Attempt::~Attempt()
{
}

static void timeoutAttempt(
                const btes5_NetworkConnector::AttemptHandle& connectionAttempt)
    // Process a timeout for the specified 'connectionAttempt'.
{
    btes5_DetailedError error("Connection attempt timed out",
                              connectionAttempt->d_connector->d_allocator_p);
    terminate(connectionAttempt, btes5_NetworkConnector::e_TIMEOUT, error);
}

                        // ----------------------------
                        // class btes5_NetworkConnector
                        // ----------------------------
// CREATORS
btes5_NetworkConnector::btes5_NetworkConnector(
                   const btes5_NetworkDescription&               socks5Servers,
                   bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
                   btemt_TcpTimerEventManager                   *eventManager,
                   int                                           minSourcePort,
                   int                                           maxSourcePort,
                   btes5_CredentialsProvider                    *provider,
                   bslma::Allocator                             *allocator)
{
    allocator = bslma::Default::allocator(allocator);
    d_connector.load(new (*allocator) Connector(socks5Servers,
                                                socketFactory,
                                                eventManager,
                                                minSourcePort,
                                                maxSourcePort,
                                                provider,
                                                allocator));
}

btes5_NetworkConnector::btes5_NetworkConnector(
                   const btes5_NetworkDescription&               socks5Servers,
                   bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
                   btemt_TcpTimerEventManager                   *eventManager,
                   btes5_CredentialsProvider                    *provider,
                   bslma::Allocator                             *allocator)
{
    allocator = bslma::Default::allocator(allocator);
    d_connector.load(new (*allocator) Connector(socks5Servers,
                                                socketFactory,
                                                eventManager,
                                                0,
                                                0,
                                                provider,
                                                allocator));
}

btes5_NetworkConnector::btes5_NetworkConnector(
                   const btes5_NetworkDescription&               socks5Servers,
                   bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
                   btemt_TcpTimerEventManager                   *eventManager,
                   int                                           minSourcePort,
                   int                                           maxSourcePort,
                   bslma::Allocator                             *allocator)
{
    allocator = bslma::Default::allocator(allocator);
    d_connector.load(new (*allocator) Connector(socks5Servers,
                                                socketFactory,
                                                eventManager,
                                                minSourcePort,
                                                maxSourcePort,
                                                0,
                                                allocator));
}

btes5_NetworkConnector::btes5_NetworkConnector(
                   const btes5_NetworkDescription&               socks5Servers,
                   bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
                   btemt_TcpTimerEventManager                   *eventManager,
                   bslma::Allocator                             *allocator)
{
    allocator = bslma::Default::allocator(allocator);
    d_connector.load(new (*allocator) Connector(socks5Servers,
                                                socketFactory,
                                                eventManager,
                                                0,
                                                0,
                                                0,
                                                allocator));
}

btes5_NetworkConnector::~btes5_NetworkConnector()
{
}

// MANIPULATORS
btes5_NetworkConnector::AttemptHandle
btes5_NetworkConnector::makeAttemptHandle(
                                    const ConnectionStateCallback& callback,
                                    const bdet_TimeInterval&       timeout,
                                    const bteso_Endpoint&          server)
{
    // check at least one level, and each proxy level has at least one proxy

    const bsl::size_t levels = d_connector->d_socks5Servers.levelCount();
    BSLS_ASSERT(levels > 0);
    for (bsl::size_t i = 0; i < levels; i++) {
        BSLS_ASSERT(d_connector->d_socks5Servers.numProxies(i));
    }

    AttemptHandle attempt(new (*d_connector->d_allocator_p)
                              Attempt(callback,
                                      timeout,
                                      server,
                                      d_connector,
                                      d_connector->d_allocator_p));
    return attempt;
}

void btes5_NetworkConnector::startAttempt(AttemptHandle& connectionAttempt)
{
    bteso_EventManager::Callback
        cb = bdef_BindUtil::bind(timeoutAttempt, connectionAttempt);
    connectionAttempt->d_timerId_p = connectionAttempt->d_connector
        ->d_eventManager_p->registerTimer(connectionAttempt->d_timeout, cb);

    tcpConnect(connectionAttempt, 0);

/*** TODO: put real logic here
OK set timeout
OK create a vector of indices [levelCount], starting with 0.
connect(level=0, vector):

OK Connect:
OK  
OK call real-connector
OK  if index < level0.size
OK     connect(level0[index], connect0-callback)
OK connect0-callback(status)
OK  if fail index++ and call real-connector
 if succeed call negotiator(socket, level, order, negotiatorCb)

connect(level, indices)
 // theoretically, we have at least one more proxy to try
 if (0 == level) tcp-connect(d_proxies[0].indices[0], negotiatorCb)
 else negotiate

negotiatorCb(socket, level, order
 if success
  if last level SUCCEED
  else
   set indices[level+1..levelCount] = 0
   call negotiator(socket, level+1, 0, negotiatorCb)
 else
  if failure is bad password FAIL
  indices[level]++
  set indices[level+1..levelCount] = 0
  while (indices[level] == d_proxies[level].size()) {
   if (level == 0) FAIL
   set indices[level..levelCount] = 0
   indices[--level]++
  }
  connect(level=0, indices)

***/
    btes5_DetailedError error("not implemented yet");
    connectionAttempt->d_callback(e_ERROR,
             0,
             connectionAttempt->d_connector->d_socketFactory_p,
             error);
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
