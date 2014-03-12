// btes5_networkconnector.cpp                                         -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_socks5connector_cpp, "$Id$ $CSID$")

#include <btes5_negotiator.h>
#include <btes5_networkconnector.h>
#include <btes5_testserver.h> // for testing only

#include <bcema_sharedptr.h>
#include <bdef_bind.h>
#include <bdetu_systemtime.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_platform.h>
#include <btemt_sessionpool.h> // for testing only
#include <btemt_tcptimereventmanager.h>
#include <bteso_inetstreamsocketfactory.h>
#include <bteso_ioutil.h>
#include <bteso_resolveutil.h>
#include <bteso_socketimputil.h>
#include <bteso_socketoptutil.h>

#include <bsl_sstream.h>

// 'btes5_NetworkConnector' implements asynchronous connection establishments.
// Because of that, callbacks related to IO and timeout events can be invoked
// after an associated connection has been cancelled, or, indeed, after the
// 'btes5_NetworkConnector' object has been destroyed.  The state referenced by
// the callbacks is allocated, with its lifetime controlled by
// 'bcema_SharedPtr'.
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
        // factory used to allocate sockets, not owned

    btemt_TcpTimerEventManager *d_eventManager_p;  // event manager, not owned

    btes5_CredentialsProvider *d_provider_p;
        // credentials provider, not owned

    btes5_Negotiator d_negotiator; // SOCKS5 negotiator

    bslma::Allocator *d_allocator_p;  // memory allocator, not owned

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
, d_socks5Servers(socks5Servers, allocator)
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

                            // ==============
                            // struct Attempt
                            // ==============

struct btes5_NetworkConnector::Attempt {
    // Objects describing the state of a SOCKS connection attempt. The object
    // lifetime is managed by using an 'bcema_SharedPtr<Attempt>' in callback
    // registration. An 'Attempt' object is constructed by
    // 'btes5_NetworkConnector::connect()' and persists until all callbacks
    // into which such object is bound are destroyed. An 'Attempt' object is in
    // one of two states: it's created in the normal state  indicated by
    // 'd_terminating == 0', and it enters a terminating state when
    // 'd_terminating != 0'.
    //
    // The creation of an 'Attempt' object initiates an asynchronous attempt to
    // establish a connection to the destination through a network of
    // SOCKS5-compliant proxies defined in 'd_connector'. The proxy network is
    // composed of one or more levels, with level 0 proxies directly reachable,
    // and all level N proxies reachable from any level N-1 proxy.  We try to
    // connect to each proxy in level N (starting with 0); when one succeeds
    // it's used to negotiate a connection to a proxy in level N+1 (or the
    // destination, if N is the last level). On error, we try connecting to the
    // next proxy in level N+1 through the previously successful proxy path.
    // Note that on error, the socket must be closed and the path needs to be
    // reestablished from the start.  When we have tried every proxy in level
    // N+1, we advance to the next proxy in level N and start trying to connect
    // to the proxies (starting from 0) in level N+1. We try from the beginning
    // in level N+1 because it's possible that due to connectivity restrictions
    // our new path may be able to connect to proxies in level N+1 that were
    // previously not reachable.  The variables 'd_level' and 'd_indices' is
    // used to keep track of the connection path being tried.
    //
    // This type is 'thread-aware' and a single 'Attempt' object may be
    // modified from a client thread as well as from the event scheduler
    // thread.  Two locking mechanisms are used to maintain the object state
    // consistent:
    //: 1 Atomic member 'd_teminating' ensures that only one termination is
    //:   processed on an object; once this variable is set (changed from the
    //:   initial value of 0) no more 'terminate' calls will proceed on this
    //:   object.
    //:
    //: 2 The access to 'd_socket_p' is protected by 'd_socketLock' since it
    //:   can otherwise be closed (deallocated) while another function tries to
    //:   use it.  Since it's a pointer, 0 is used to indicate socket absence.

    // DATA
    const ConnectionStateCallback   d_callback;      // client callback
    const bdet_TimeInterval         d_proxyTimeout;  // step timeout
    const bdet_TimeInterval         d_totalTimeout;  // total attempt timeout
    const bteso_Endpoint            d_server;        // destination address
    bteso_SocketHandle::Handle      d_handle;        // OS-level socket

    bcema_SharedPtr<Connector>      d_connector;
        // persistent state of the connector associated with this attempt

    void           *d_timer;       // total expiration timer id
    void           *d_proxyTimer;  // per-proxy expiration timer id
    bsls::AtomicInt d_terminating; // attempt being terminated

    bsl::size_t d_level; // proxy level being tried

    bsl::vector<bsl::size_t>        d_indices;
        // sequence of 'd_connector->d_Socks5Servers.levelCount()' indices
        // indicating the next SOCKS5 server to try connecting to

    bteso_StreamSocket<bteso_IPv4Address> *d_socket_p;
        // socket for the connection to the first-level proxy, owned

    bcemt_Mutex d_socketLock;  // serialize 'd_socket_p' access

    int         d_tries;       // number of tries left for this attempt

    bslma::Allocator                      *d_allocator_p;
        // memory allocator, not owned

    // CREATORS
    Attempt(const ConnectionStateCallback&  callback,
            const bdet_TimeInterval&        proxyTimeout,
            const bdet_TimeInterval&        totalTimeout,
            const bteso_Endpoint&           destination,
            bcema_SharedPtr<Connector>&     connector,
            bslma::Allocator               *allocator);
        // Create an 'Attempt' object associated with the specified 'connector'
        // to connect to the specified 'destination' and asynchronously invoke
        // the specified 'callback', using the specified 'allocator' to supply
        // memory.  If the specified 'proxyTimeout' is not empty, each proxy
        // connection attempt must succeed within that period. If the specified
        // 'totalTimeout' is not empty, the entire connection attempt must
        // succeed within this time period.

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
    const btes5_NetworkConnector::AttemptHandle& attempt,
    btes5_NetworkConnector::ConnectionStatus     status,
    const btes5_DetailedError&                   error)
    // Terminate the specified 'attempt' with the specified 'status'
    // and 'error'.
{
    if (attempt->d_terminating.testAndSwap(0, 1)) {
        return; // this attempt is already being terminated
    }
    attempt->d_connector->d_negotiator.cancelNegotiation();

    if (attempt->d_timer) {
        attempt->d_connector->d_eventManager_p->deregisterTimer(
                                                         attempt->d_timer);
    }
    if (btes5_NetworkConnector::e_SUCCESS == status) {
        bcemt_LockGuard<bcemt_Mutex> guard(&attempt->d_socketLock);
        attempt->d_callback(status,
                            attempt->d_socket_p,
                            attempt->d_connector->d_socketFactory_p,
                            error);
    } else {
        {
            bcemt_LockGuard<bcemt_Mutex> guard(&attempt->d_socketLock);
            if (attempt->d_socket_p) {
                attempt->d_connector->d_socketFactory_p
                                             ->deallocate(attempt->d_socket_p);
                attempt->d_socket_p = 0;
            }
        }
        if (btes5_NetworkConnector::e_TIMEOUT == status
                || !attempt->d_tries) {
            attempt->d_callback(status,
                                0,
                                attempt->d_connector->d_socketFactory_p,
                                error);
        }
        else {
            // try again, from the beginning

            for (bsl::size_t l = 0;
                 l < attempt->d_connector->d_socks5Servers.levelCount();
                 ++l) {
                    attempt->d_indices[l] = 0;
            }
            --attempt->d_tries;
            attempt->d_terminating = 0;
            tcpConnect(attempt);
        }
    }
}

static void socksConnectCb(
    btes5_NetworkConnector::AttemptHandle         attempt,
    btes5_Negotiator::NegotiationStatus           result,
    const btes5_DetailedError&                    error)
    // Process the specified 'result' of a SOCKS5 negotiation for the specified
    // 'attempt' with the specified 'error'.
{
    bsl::size_t level = attempt->d_level;
    if (btes5_Negotiator::e_SUCCESS == result) {
        ++level;

        if (level == attempt->d_connector->d_socks5Servers.levelCount()) {
            btes5_DetailedError error("Success");
            terminate(attempt,
                      btes5_NetworkConnector::e_SUCCESS,
                      error);
        } else {
            ++attempt->d_level;

            // for all subsequent levels start from proxy 0

            for (bsl::size_t l = level + 1;
                 l < attempt->d_connector->d_socks5Servers.levelCount();
                 ++l) {
                    attempt->d_indices[l] = 0;
            }
            socksConnect(attempt);
        }
    }
    else {
        const bsl::size_t index = attempt->d_indices[level]; // failed proxy
        const btes5_ProxyDescription& proxy
            = attempt->d_connector ->d_socks5Servers.beginLevel(level)[index];

        while (++attempt->d_indices[level]
                == attempt->d_connector->d_socks5Servers.numProxies(level)) {
            if (!level) {
                btes5_DetailedError e(error);
                e.setAddress(proxy.address());
                terminate(attempt, btes5_NetworkConnector::e_ERROR, e);
                return;                                               // RETURN
            }
            --level; // try a lower (closer) proxy level
        }

        // More proxies left in this level: close socket, increment index
        // and restart connection

        for (bsl::size_t l = level + 1;
             l < attempt->d_connector->d_socks5Servers.levelCount();
             ++l) {
                attempt->d_indices[l] = 0;
        }

        {
            bcemt_LockGuard<bcemt_Mutex> guard(&attempt->d_socketLock);
            if (attempt->d_socket_p) {
                attempt->d_connector->d_socketFactory_p->deallocate(
                                                      attempt->d_socket_p);
                attempt->d_socket_p = 0;
            }
        }
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
    {
        bcemt_LockGuard<bcemt_Mutex> guard(&attempt->d_socketLock);
        if (proxy.credentials().isSet()) {
            rc = attempt->d_connector
                ->d_negotiator.negotiate(attempt->d_socket_p,
                                         *destination,
                                         cb,
                                         attempt->d_proxyTimeout,
                                         proxy.credentials());
        } else if (attempt->d_connector->d_provider_p) {
            rc = attempt->d_connector->d_negotiator.negotiate(
                                           attempt->d_socket_p,
                                           *destination,
                                           cb,
                                           attempt->d_proxyTimeout,
                                           attempt->d_connector->d_provider_p);
        } else {
            rc = attempt->d_connector
                ->d_negotiator.negotiate(attempt->d_socket_p,
                                         *destination,
                                         cb,
                                         attempt->d_proxyTimeout);
        }
    }
    if (rc) {
        socksConnectCb(attempt,
                       btes5_Negotiator::e_ERROR,
                       btes5_DetailedError("Unable to negotiate",
                                           proxy.address()));
    }
}

static void connectTcpCb(
                       btes5_NetworkConnector::AttemptHandle connectionAttempt,
                       bool                                  timeout)
    // Process the result of a connection attempt to a first-level proxy in the
    // specified 'connectionAttempt'.  If the specified 'timeout' is 'true' the
    // TCP connection timed out.
{
    btes5_NetworkConnector::AttemptHandle attempt(connectionAttempt);
        // copy shared ptr because deregisterSocket removes the reference

    if (!timeout && bdet_TimeInterval() != attempt->d_proxyTimeout) {
        attempt->d_connector->d_eventManager_p
            ->deregisterTimer(attempt->d_proxyTimer);
    }

    const bsl::size_t index = attempt->d_indices[0]; // current proxy index

    int rc;
    {
        bcemt_LockGuard<bcemt_Mutex> guard(&attempt->d_socketLock);
        if (attempt->d_socket_p) {
            attempt->d_connector->d_eventManager_p->deregisterSocket(
                                                attempt->d_socket_p->handle());
            if (hasTimedOut) {
                rc = -1;
            } else if (attempt->d_socket_p->connectionStatus()) {
                rc = -2;
            } else {
                rc = 0;
            }
            if (rc) {
                attempt->d_connector->d_socketFactory_p->deallocate(
                                                          attempt->d_socket_p);
                attempt->d_socket_p = 0;
            }
        } else {
            rc = -3;  // the socket was already closed
        }
    }
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
    } else {

        // start negotiation on socket

        attempt->d_level = 0;
        socksConnect(connectionAttempt);
    }
}

static bteso_StreamSocket<bteso_IPv4Address> *makeSocket(
               bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
               btes5_DetailedError                          *error,
               int                                           minSourcePort = 0,
               int                                           maxSourcePort = 0)
    // Allocate a socket suitable for SOCKS5 negotiation using the specified
    // 'socketFactory'. Return the socket if successful, otherwise return 0 and
    // load information into the specified 'error'. Optionally specify
    // 'minSourcePort' and 'maxSourcePort' to bind the socket to a port in this
    // range. The behavior is undefined unless
    // '0 == minSourcePort && 0 == maxSourcePort' or
    // '1 <= minSourcePort <= maxSourcePort <= 65535'.
{
    bteso_StreamSocket<bteso_IPv4Address> *socket = socketFactory->allocate();
    if (!socket) {
        error->setDescription("Unable to allocate a socket");
        return 0;                                                     // RETURN
    }
    bteso_StreamSocketFactoryAutoDeallocateGuard<bteso_IPv4Address>
        socketGuard(socket, socketFactory);

    int rc = 0; // return code for socket operations

#ifndef BSLS_PLATFORM_OS_WINDOWS
    // REUSEADDR on Windows has a different meaning than on UNIX: it allows
    // more than one process to bind to the same port simultaneously, which is
    // not what we want

    rc = socket->setOption(bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                           bteso_SocketOptUtil::BTESO_REUSEADDRESS,
                           0);
    if (rc) {
        error->setDescription("Unable to set REUSEADDR option");
        return 0;                                                     // RETURN
    }
#endif

    if (minSourcePort > 0) {
        const int begin = minSourcePort;
        const int end = maxSourcePort + 1;
        int port = begin;
        for (; end != port; ++port) {
            bteso_IPv4Address
                srcAddress(bteso_IPv4Address::BTESO_ANY_ADDRESS, port);
            rc = socket->bind(srcAddress);
            if (!rc) {
                break; // bound successfully
            }
        }
        if (end == port) {
            bsl::ostringstream description;
            description << "Unable to bind source address to a port between "
                        << begin << " and " << end;
            error->setDescription(description.str());
            return 0;                                                 // RETURN
        }
    }
    rc = socket->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    if (rc) {
        error->setDescription("Unable to set socket mode to non-blocking");
        return 0;                                                     // RETURN
    }

    bteso_SocketOptUtil::LingerData lingerData;
    lingerData.l_onoff = 1;     // enable lingering
    lingerData.l_linger = 0;    // 0 seconds
    rc = socket->setLingerOption(lingerData);
    if (rc) {
        error->setDescription("Unable to set linger option");
        return 0;                                                     // RETURN
    }

    rc = socket->setOption(bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                           bteso_SocketOptUtil::BTESO_KEEPALIVE,
                           1);
    if (rc) {
        error->setDescription("Unable to set KEEPALIVE option");
        return 0;                                                     // RETURN
    }

    rc = socket->setOption(bteso_SocketOptUtil::BTESO_TCPLEVEL,
                           bteso_SocketOptUtil::BTESO_TCPNODELAY,
                           1);
    if (rc) {
        error->setDescription("Unable to set TCPNODELAY option");
        return 0;                                                     // RETURN
    }
    socketGuard.release();
    return socket;
}

static void tcpConnect(const btes5_NetworkConnector::AttemptHandle& attempt)
{
    btes5_DetailedError error("TCP connect");

    btes5_NetworkDescription::ProxyIterator
        it = attempt->d_connector->d_socks5Servers.beginLevel(0);
    it += attempt->d_indices[0];
    const btes5_NetworkDescription::ProxyIterator
        end = attempt->d_connector->d_socks5Servers.endLevel(0);
    for (; end != it; ++it, ++attempt->d_indices[0]) {
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
        int rc;
        server.setPortNumber(destination.port());
        {
            bcemt_LockGuard<bcemt_Mutex> guard(&attempt->d_socketLock);
            attempt->d_socket_p = makeSocket(
                                       attempt->d_connector->d_socketFactory_p,
                                       &error,
                                       attempt->d_connector->d_minSourcePort,
                                       attempt->d_connector->d_maxSourcePort);
            if (!attempt->d_socket_p) {
                continue; // try again if more servers
            }
            rc = attempt->d_socket_p->connect(server);
        }
        if (!rc) {
            connectTcpCb(attempt, false); // immediate success
            break;
        } else if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == rc) {
            bteso_EventManager::Callback
                cb = bdef_BindUtil::bind(connectTcpCb, attempt, false);
            {
                bcemt_LockGuard<bcemt_Mutex> guard(&attempt->d_socketLock);
                if (attempt->d_socket_p) {
                    rc = attempt->d_connector->d_eventManager_p
                        ->registerSocketEvent(attempt->d_socket_p->handle(),
                                              bteso_EventType::BTESO_CONNECT,
                                              cb);
                }
            }

            if (bdet_TimeInterval() != attempt->d_proxyTimeout) {
                bdet_TimeInterval expiration = bdetu_SystemTime::now()
                    + attempt->d_proxyTimeout;
                bteso_EventManager::Callback
                    cb = bdef_BindUtil::bind(connectTcpCb, attempt, true);
                attempt->d_proxyTimer
                    = attempt->d_connector->d_eventManager_p->registerTimer(
                                                                    expiration,
                                                                    cb);
            }
            break; // now wait for connection callback
        } else {
            bsl::ostringstream description;
            description << "Unable to connect: " << rc;
            error.setDescription(description.str());
            error.setAddress(destination);
            continue; // do not terminate because there may be more servers
        }
    }
    if (end == it) {
        terminate(attempt, btes5_NetworkConnector::e_ERROR, error);
        return;                                                       // RETURN
    }
}

static void timeoutAttempt(
                btes5_NetworkConnector::AttemptHandle connectionAttempt)
    // Process a timeout for the specified 'connectionAttempt'.
{
    btes5_DetailedError error("Connection attempt timed out",
                              connectionAttempt->d_connector->d_allocator_p);
    terminate(connectionAttempt, btes5_NetworkConnector::e_TIMEOUT, error);
}

}  // close unnamed namespace


                               // --------------
                               // struct Attempt
                               // --------------

// CREATORS
btes5_NetworkConnector::Attempt::Attempt(
    const ConnectionStateCallback&  callback,
    const bdet_TimeInterval&        proxyTimeout,
    const bdet_TimeInterval&        totalTimeout,
    const bteso_Endpoint&           server,
    bcema_SharedPtr<Connector>&     connector,
    bslma::Allocator               *allocator)
: d_callback(callback, allocator)
, d_proxyTimeout(proxyTimeout)
, d_totalTimeout(totalTimeout)
, d_server(server, allocator)
, d_connector(connector)
, d_timer(0)
, d_level(0)
, d_indices(connector->d_socks5Servers.levelCount(), 0, allocator)
, d_socket_p(0)
, d_tries(2)
, d_allocator_p(allocator)
{
}

btes5_NetworkConnector::Attempt::~Attempt()
{
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
                                                allocator),
                     allocator);
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
                                                allocator),
                     allocator);
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
                                                allocator),
                     allocator);
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
                                                allocator),
                     allocator);
}

btes5_NetworkConnector::~btes5_NetworkConnector()
{
}

// MANIPULATORS
btes5_NetworkConnector::AttemptHandle
btes5_NetworkConnector::makeAttemptHandle(
                                   const ConnectionStateCallback& callback,
                                   const bdet_TimeInterval&       proxyTimeout,
                                   const bdet_TimeInterval&       totalTimeout,
                                   const bteso_Endpoint&          server)
{
    // verify at least one level, and each proxy level has at least one proxy

    const bsl::size_t levels = d_connector->d_socks5Servers.levelCount();
    BSLS_ASSERT(levels > 0);
    for (bsl::size_t i = 0; i < levels; ++i) {
        BSLS_ASSERT(d_connector->d_socks5Servers.numProxies(i));
    }

    AttemptHandle attempt(new (*d_connector->d_allocator_p)
                              Attempt(callback,
                                      proxyTimeout,
                                      totalTimeout,
                                      server,
                                      d_connector,
                                      d_connector->d_allocator_p),
                          d_connector->d_allocator_p);
    return attempt;
}

void btes5_NetworkConnector::startAttempt(AttemptHandle& connectionAttempt)
{
    BSLS_ASSERT(d_connector->d_eventManager_p->isEnabled());

    if (bdet_TimeInterval() != connectionAttempt->d_totalTimeout) {
        bdet_TimeInterval expiration
            = bdetu_SystemTime::now() + connectionAttempt->d_totalTimeout;
        bteso_EventManager::Callback
           cb = bdef_BindUtil::bind(timeoutAttempt, connectionAttempt);
        connectionAttempt->d_timer = connectionAttempt->d_connector
           ->d_eventManager_p->registerTimer(expiration, cb);
    }
    tcpConnect(connectionAttempt);
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
