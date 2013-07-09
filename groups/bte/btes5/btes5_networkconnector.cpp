// btes5_networkconnector.cpp                                         -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_socks5connector_cpp, "$Id$ $CSID$")

#include <btes5_networkconnector.h>
#include <btes5_negotiator.h>
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

/* TODO: Pseudo-code
set timeout
create a vector of indices [levelCount], starting with 0.
connect(level=0, vector):

Connect:

call real-connector
 if index < level0.size
    connect(level0[index], connect0-callback)
connect0-callback(status)
 if fail index++ and call real-connector
 if succeed call negotiator(socket, level, order, socksConnectCb)

connect(level, indices)
 // theoretically, we have at least one more proxy to try
 if (0 == level) tcp-connect(d_proxies[0].indices[0], socksConnectCb)
 else negotiate

socksConnectCb(socket, level, order
 if success
  if last level SUCCEED
  else
   set indices[level+1..levelCount] = 0
   call negotiator(socket, level+1, 0, socksConnectCb)
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

    btes5_Negotiator d_negotiator; // SOCKS5 negotiator

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
, d_negotiator(eventManager, allocator)
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
    // TODO: describe d_indices

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

    bsl::size_t d_level; // proxy level being tried

    bsl::vector<bsl::size_t>        d_indices;
        // sequence of 'd_connector->d_Socks5Servers.levelCount()' indices
        // indicating the next SOCKS5 server to try connecting to

    bteso_StreamSocket<bteso_IPv4Address> *d_socket_p;
        // socket for the connection to the first-level proxy, owned
        // TODO: protect wrt deallocation in 'terminate'.

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

// forward declarations for file-scope functions

static void tcpConnect(const btes5_NetworkConnector::AttemptHandle& attempt);
    // Establish a connection to the first-level proxy host in the specified
    // 'attempt'. The results will be delivered, possibly from another thread,
    // by invoking 'ConnectTcpCb'.

static void socksConnect(const btes5_NetworkConnector::AttemptHandle& attempt);
    // Establish a TCP connection using a SOCKS5 proxy per the specified
    // 'attempt'. Use 'attempt->d_socket_p' for communication with the proxy,
    // and deliver the result, possibly from a different thread, to
    // 'socksConnectCb'.

// definitions for file-scope functions

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

static void socksConnectCb(
    const btes5_NetworkConnector::AttemptHandle&  attempt,
    btes5_Negotiator::NegotiationStatus           result,
    const btes5_DetailedError&                    error)
    // Process the specified 'result' of a SOCKS5 negotiation for the specified
    // 'attempt' with the specified 'error'.
{
    bsl::size_t level = attempt->d_level;
    if (btes5_Negotiator::e_SUCCESS == result) {
        level++;

        if (level == attempt->d_connector->d_socks5Servers.levelCount()) {
            btes5_DetailedError error("Success");
            terminate(attempt,
                      btes5_NetworkConnector::e_SUCCESS,
                      error);
        } else {
            attempt->d_level++;

            // for all subsequent levels start from proxy 0

            for (bsl::size_t l = level + 1;
                 l < attempt->d_connector->d_socks5Servers.levelCount();
                 l++) {
                    attempt->d_indices[l] = 0;
            }
            socksConnect(attempt);
        }
    } else {
        // TODO: special handling for password failure?
        while (++attempt->d_indices[level]
                == attempt->d_connector->d_socks5Servers.numProxies(level)) {
            if (!level) {
                terminate(attempt, btes5_NetworkConnector::e_ERROR, error);
                return;                                               // RETURN
            }
            level--; // try a lower (closer) proxy level
        }

        // More proxies left in this level: close socket, increment index
        // and restart connection

        for (bsl::size_t l = level + 1;
             l < attempt->d_connector->d_socks5Servers.levelCount();
             l++) {
                attempt->d_indices[l] = 0;
        }
        // TODO: protect d_socket_p against termination?
        attempt->d_connector->d_socketFactory_p->deallocate(
                                                      attempt->d_socket_p);
        attempt->d_level = level;
        tcpConnect(attempt);
    }
}

static void socksConnect(const btes5_NetworkConnector::AttemptHandle& attempt)
    // Invoke the SOCKS5 negotiator for the specified 'attempt'.
{
    const bsl::size_t level = attempt->d_level;          // proxy level to try
    const bsl::size_t index = attempt->d_indices[level]; // specific proxy
    const btes5_ProxyDescription& proxy
        = attempt->d_connector->d_socks5Servers.beginLevel(level)[index];

    const bsl::size_t nextLevel = level + 1;
    const bteso_Endpoint *destination;
    if (nextLevel == attempt->d_connector->d_socks5Servers.levelCount()) {
        destination = &attempt->d_server;
    } else {
        const bsl::size_t nextIndex = attempt->d_indices[nextLevel];

        destination = &attempt->d_connector->d_socks5Servers
            .beginLevel(nextLevel)[nextIndex].address();
    }

    using namespace bdef_PlaceHolders;
    btes5_Negotiator::NegotiationStateCallback
        cb = bdef_BindUtil::bind(socksConnectCb, attempt, _1, _2);

    int rc;
    if (proxy.credentials().isSet()) {
        rc = attempt->d_connector->d_negotiator.negotiate(attempt->d_socket_p,
                                                          *destination,
                                                          cb,
                                                          proxy.credentials());
    } else if (attempt->d_connector->d_provider_p) {
        rc = attempt->d_connector->d_negotiator.negotiate(
                                           attempt->d_socket_p,
                                           *destination,
                                           cb,
                                           attempt->d_connector->d_provider_p);
    } else {
        rc = attempt->d_connector->d_negotiator.negotiate(attempt->d_socket_p,
                                                          *destination,
                                                          cb);
    }
    if (rc) {
        socksConnectCb(attempt,
                       btes5_Negotiator::e_ERROR,
                       btes5_DetailedError("Unable to negotiate",
                                           proxy.address()));
    }
}

static void connectTcpCb(
    const btes5_NetworkConnector::AttemptHandle& connectionAttempt)
    // Process the result of a connection attempt to a first-level proxy in the
    // specified 'connectionAttempt'.
{
    // TODO: check for terminating attempt

    btes5_NetworkConnector::AttemptHandle attempt(connectionAttempt);
        // copy shared ptr because deregisterSocket removes the reference

    const bsl::size_t index = attempt->d_indices[0]; // current proxy index
    attempt->d_connector->d_eventManager_p->deregisterSocket(
                                                attempt->d_socket_p->handle());
    int rc = attempt->d_socket_p->connectionStatus();
    if (rc) {
        if (index < attempt->d_connector->d_socks5Servers.numProxies(0)) {
            attempt->d_indices[0]++; // try the next proximate proxy
            tcpConnect(attempt);
        } else {
            btes5_DetailedError error(
                "Unable to connect to any proximate proxies",
                attempt->d_connector->d_socks5Servers.beginLevel(0)
                    [index].address());
            terminate(attempt, btes5_NetworkConnector::e_ERROR, error);
        }
    }

    // start negotiation on socket

    attempt->d_level = 0;
    socksConnect(connectionAttempt);
}

static void tcpConnect(const btes5_NetworkConnector::AttemptHandle& attempt)
{
    btes5_DetailedError error("TCP connect");;

    bteso_StreamSocket<bteso_IPv4Address> *socket
        = attempt->d_connector->d_socketFactory_p->allocate();
    if (!socket) {
        // socket allocation failed, retrying immediately isn't likely to work
        // so return control to the client

        error.setDescription("Unable to allocate a socket");
        terminate(attempt, btes5_NetworkConnector::e_ERROR, error);
        return;                                                       // RETURN
    }
    attempt->d_socket_p = socket;

    int rc = 0; // return code for socket operations

    // Note: to be compatible with bbcomm - only do this on unix
#ifndef BSLS_PLATFORM_OS_WINDOWS
    rc = socket->setOption(bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                           bteso_SocketOptUtil::BTESO_REUSEADDRESS,
                           0);
    if (rc) {
        error.setDescription("Unable to set REUSEADDR option");
        terminate(attempt, btes5_NetworkConnector::e_ERROR, error);
        return;                                                       // RETURN
    }
#endif

    if (attempt->d_connector->d_minSourcePort > 0) {
        const int begin = attempt->d_connector->d_minSourcePort;
        const int end = attempt->d_connector->d_maxSourcePort + 1;
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
            terminate(attempt,
                      btes5_NetworkConnector::e_ERROR,
                      error);
            return;                                                   // RETURN
        }
    }
    rc = socket->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    if (rc) {
        error.setDescription("Unable to set socket mode to non-blocking");
        terminate(attempt, btes5_NetworkConnector::e_ERROR, error);
        return;                                                       // RETURN
    }

    bteso_SocketOptUtil::LingerData lingerData;
    lingerData.l_onoff = 1;     // enable lingering
    lingerData.l_linger = 0;    // 0 seconds
    rc = socket->setLingerOption(lingerData);
    if (rc) {
        error.setDescription("Unable to set linger option");
        terminate(attempt, btes5_NetworkConnector::e_ERROR, error);
        return;                                                       // RETURN
    }

    rc = socket->setOption( bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                            bteso_SocketOptUtil::BTESO_KEEPALIVE,
                            1);
    if (rc) {
        error.setDescription("Unable to set KEEPALIVE option");
        terminate(attempt, btes5_NetworkConnector::e_ERROR, error);
        return;                                                       // RETURN
    }

    rc = socket->setOption(bteso_SocketOptUtil::BTESO_TCPLEVEL,
                            bteso_SocketOptUtil::BTESO_TCPNODELAY,
                            1);
    if (rc) {
        error.setDescription("Unable to set TCPNODELAY option");
        terminate(attempt, btes5_NetworkConnector::e_ERROR, error);
        return;                                                       // RETURN
    }

    btes5_NetworkDescription::ProxyIterator
        it = attempt->d_connector->d_socks5Servers.beginLevel(0);
    it += attempt->d_indices[0];
    const btes5_NetworkDescription::ProxyIterator
        end = attempt->d_connector->d_socks5Servers.endLevel(0);
    for (; end != it; it++, attempt->d_indices[0]++) {
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
                continue;  // not fatal because there may be more servers
            }
        }
        server.setPortNumber(destination.port());
        rc = socket->connect(server);
        if (!rc) {
            connectTcpCb(attempt); // immediate success
        } else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            bteso_EventManager::Callback
                cb = bdef_BindUtil::bind(connectTcpCb, attempt);
            rc = attempt->d_connector
              ->d_eventManager_p->registerSocketEvent(socket->handle(),
                                                      bteso_EventType::CONNECT,
                                                      cb);
        } else {
            error.setDescription("Unable to conect");
            continue; // do not terminate because there may be more servers
        }
    }
    if (end == it) {
        terminate(attempt, btes5_NetworkConnector::e_ERROR, error);
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
, d_level(0)
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

    tcpConnect(connectionAttempt);
    return;
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
