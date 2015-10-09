// btls5_networkconnector.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btls5_networkconnector.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btls5_networkconnector_cpp, "$Id$ $CSID$")

#include <btls5_negotiator.h>
#include <btls5_networkdescriptionutil.h>  // for testing only
#include <btls5_testserver.h>              // for testing only

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bdlf_bind.h>
#include <bdlt_currenttime.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_platform.h>
#include <btlmt_sessionpool.h>
#include <btlso_inetstreamsocketfactory.h>
#include <btlso_ioutil.h>
#include <btlso_resolveutil.h>
#include <btlso_socketoptutil.h>

#include <bsl_cerrno.h>
#include <bsl_cstddef.h>
#include <bsl_cstring.h>
#include <bsl_memory.h>
#include <bsl_new.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

// 'btls5::NetworkConnector' implements asynchronous connection establishments.
// Because of that, callbacks related to IO and timeout events can be invoked
// after an associated connection has been canceled, or, indeed, after the
// 'btls5::NetworkConnector' object has been destroyed.  The state referenced
// by the callbacks is allocated, with its lifetime controlled by
// 'bsl::shared_ptr'.
//
// Two classes are used to maintain state: a 'Connector' for object variables
// such as the SOCKS5 network description, and a 'ConnectionAttempt' for the
// state specific to one connection attempt.

#define BTLS5_NETWORKCONNECTOR_RETRIES 0  // number of times to retry from the
                                          // beginning of proxy network

namespace BloombergLP {

namespace btls5 {

                              // ===============
                              // class Connector
                              // ===============

class NetworkConnector::Connector {
    // This mechanism class represents the state associated with a
    // 'NetworkConnector' object.  A 'NetworkConnector::Connector' may outlive
    // the destruction of the associated 'NetworkConnector' since there may
    // still be callbacks in place after the latter's destruction.

  public:
    // DATA
    int                                             d_minSourcePort;
    int                                             d_maxSourcePort;
        // if not 0, originating sockets will be bound to a port in this range

    const NetworkDescription                        d_socks5Servers;
        // the network of proxy hosts

    btlso::StreamSocketFactory<btlso::IPv4Address> *d_socketFactory_p;
        // factory used to allocate sockets, not owned

    btlmt::TcpTimerEventManager                    *d_eventManager_p;
        // event manager, not owned

    bslma::Allocator                               *d_allocator_p;
        // memory allocator, not owned

  private:
    // NOT IMPLEMENTED
    Connector(const Connector&);                                    // = delete
    Connector& operator=(const Connector&);                         // = delete

  public:
    // CREATORS
    Connector(const NetworkDescription&                       socks5Servers,
              btlso::StreamSocketFactory<btlso::IPv4Address> *socketFactory,
              btlmt::TcpTimerEventManager                    *eventManager,
              int                                             minSourcePort,
              int                                             maxSourcePort,
              bslma::Allocator                               *basicAllocator);
        // Create a 'Connector' object for connecting through a proxy network
        // described by the specified 'socks5Servers', allocating (and
        // deallocating) sockets using the specified 'socketFactory', and
        // scheduling events with the specified 'eventManager'.  If the
        // specified 'minSourcePort' and 'maxSourcePort' are both 0 let the
        // operating system assign a port for the local endpoint; otherwise try
        // to bind a port in the range from 'minSourcePort' to 'maxSourcePort',
        // inclusive for the local endpoint of connections established through
        // this object.  Use the specified 'basicAllocator' to supply memory.
        // The behavior is undefined unless 'minSourcePort' and 'maxSourcePort'
        // are both 0, or '1 <= minSourcePort' and
        // 'minSourcePort <= maxSourcePort' and 'maxSourcePort <= 65535'.
};


                              // ---------------
                              // class Connector
                              // ---------------

// CREATORS
btls5::NetworkConnector::Connector::Connector(
                const btls5::NetworkDescription&                socks5Servers,
                btlso::StreamSocketFactory<btlso::IPv4Address> *socketFactory,
                btlmt:: TcpTimerEventManager                   *eventManager,
                int                                             minSourcePort,
                int                                             maxSourcePort,
                bslma::Allocator                               *basicAllocator)
: d_minSourcePort(minSourcePort)
, d_maxSourcePort(maxSourcePort)
, d_socks5Servers(socks5Servers, basicAllocator)
, d_socketFactory_p(socketFactory)
, d_eventManager_p(eventManager)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT(eventManager);
    BSLS_ASSERT(socketFactory);
    BSLS_ASSERT((0 == minSourcePort && 0 == maxSourcePort)
             || (1 <= minSourcePort
                && minSourcePort <= maxSourcePort && maxSourcePort <= 65535));
    BSLS_ASSERT(basicAllocator);
}

                          // =======================
                          // class ConnectionAttempt
                          // =======================

class NetworkConnector::ConnectionAttempt {
    // This class describes the state of a SOCKS connection attempt.  The
    // object lifetime is managed by using a
    // 'bsl::shared_ptr<ConnectionAttempt>' in callback registration.  An
    // 'ConnectionAttempt' object is constructed by
    // 'NetworkConnector::makeConnectionAttemptHandle' and persists until all
    // callbacks into which such object is bound are destroyed.  An
    // 'ConnectionAttempt' object is in one of two states: it is created in the
    // normal state indicated by 'd_terminating == 0', and it enters a
    // terminating state when 'd_terminating != 0'.
    //
    // The creation of a 'ConnectionAttempt' object initiates an asynchronous
    // attempt to establish a connection to the destination through a network
    // of SOCKS5-compliant proxies defined in 'd_connector'.  The proxy network
    // is composed of one or more levels, with level 0 proxies directly
    // reachable, and all level N proxies reachable from any level N-1 proxy.
    // We try to connect to each proxy in level N (starting with 0); when one
    // succeeds it is used to negotiate a connection to a proxy in level N+1
    // (or the destination, if N is the last level).  On error, we try
    // connecting to the next proxy in level N+1 through the previously
    // successful proxy path.  Note that on error, the socket must be closed
    // and the path needs to be reestablished from the start.  When we have
    // tried every proxy in level N+1, we advance to the next proxy in level N
    // and start trying to connect to the proxies (starting from 0) in level
    // N+1.  We try from the beginning in level N+1 because it is possible that
    // due to connectivity restrictions our new path may be able to connect to
    // proxies in level N+1 that we failed to connect to previously.  The
    // variables 'd_level' and 'd_indices' is used to keep track of the
    // connection path being tried.
    //
    // This type is 'thread-aware' and a single 'ConnectionAttempt' object may
    // be modified from a client thread as well as from the event scheduler
    // thread.  Two locking mechanisms are used to keep the object state
    // consistent:
    //: 1 Atomic member 'd_terminating' ensures that only one termination is
    //:   processed on an object; once this variable is set (changed from the
    //:   initial value of 0) no more 'terminate' calls will proceed on this
    //:   object.
    //:
    //: 2 The access to 'd_socket_p' is protected by 'd_socketLock' since it
    //:   can otherwise be closed (deallocated) while another function tries to
    //:   use it.  Since it is a pointer, 0 is used to indicate socket absence.
    //:   In addition, 'd_socketLock' is also used to serialize access to
    //:   'd_proxyTimer' since it may be set (registered) in a thread different
    //:   from the callback which uses it.


  public:
    // DATA
    const ConnectionStateCallback  d_callback;      // client callback

    bsls::TimeInterval             d_proxyTimeout;  // step timeout

    bsls::TimeInterval             d_totalTimeout;  // total attempt timeout

    btlso::Endpoint                d_server;        // destination address

    btlso::SocketHandle::Handle    d_handle;        // OS-level socket

    bsl::shared_ptr<Connector>     d_connector;     // persistent state of the
                                                    // connector associated
                                                    // with this attempt

    void                          *d_timer;         // total expiration timer
                                                    // id

    void                          *d_proxyTimer;    // per-proxy expiration
                                                    // timer id

    bsls::AtomicInt                d_terminating;   // if set, this attempt is
                                                    // being terminated

    bsl::size_t                    d_level;         // proxy level being tried

    bsl::vector<bsl::size_t>       d_indices;
        // sequence of 'd_connector->d_Socks5Servers.numLevels()' indices
        // indicating the next SOCKS5 server to try connecting to

    btlso::StreamSocket<btlso::IPv4Address>
                                  *d_socket_p;      // socket for the
                                                    // connection to the
                                                    // first-level proxy, owned

    bslmt::Mutex                   d_socketLock;    // serialize 'd_socket_p'
                                                    // access

    int                            d_numRetries;    // number of tries left for
                                                    // this attempt

    Negotiator                     d_negotiator;    // SOCKS5 negotiator

    Negotiator::NegotiationHandle  d_negotiation;   // Negotiation currently in
                                                    // progress.

    bool                           d_sourceStrict;  // if 'true', failure to
                                                    // bind the source port as
                                                    // specified is fatal

    bslma::Allocator              *d_allocator_p;   // memory allocator, not
                                                    // owned

  private:
    // NOT IMPLEMENTED
    ConnectionAttempt(const ConnectionAttempt&);                    // = delete
    ConnectionAttempt& operator=(const ConnectionAttempt&);         // = delete

  public:
    // CREATORS
    ConnectionAttempt(const ConnectionStateCallback&     callback,
                      const bsls::TimeInterval&          proxyTimeout,
                      const bsls::TimeInterval&          totalTimeout,
                      const btlso::Endpoint&             server,
                      const bsl::shared_ptr<Connector>&  connector,
                      bslma::Allocator                  *basicAllocator);
        // Create a 'ConnectionAttempt' object associated with the specified
        // 'connector' to connect to the specified 'server' and asynchronously
        // invoke the specified 'callback'.  Use the specified 'basicAllocator'
        // to supply memory.  If the specified 'proxyTimeout' is not the
        // default value, each proxy connection attempt must succeed within
        // that period.  If the specified 'totalTimeout' is not the default
        // value, the entire connection attempt must succeed within this time
        // period.

    //! ~ConnectionAttempt() = default;
        // Destroy this object.  Do not deallocate  or close 'd_socket_p'.
        // Connection attempts already in progress may continue, and their
        // callbacks be invoked after this object is destroyed.
};

namespace {

// forward declarations for file-scope functions

static void tcpConnect(
              const btls5::NetworkConnector::ConnectionAttemptHandle& attempt);
    // Establish a connection to the first-level proxy host in the specified
    // 'attempt'.  The results will be delivered, possibly from another thread,
    // by invoking 'connectTcpCb'.

static void socksConnect(
              const btls5::NetworkConnector::ConnectionAttemptHandle& attempt);
    // Establish a TCP connection using a SOCKS5 proxy per the specified
    // 'attempt'.  Use 'attempt->d_socket_p' for communication with the proxy,
    // and deliver the result, possibly from a different thread, to
    // 'socksConnectCb'.

// definitions for file-scope functions

static void terminate(
        const btls5::NetworkConnector::ConnectionAttemptHandle& attempt,
        btls5::NetworkConnector::ConnectionStatus               status,
        const btls5::DetailedStatus&                            detailedStatus)
    // Terminate the specified 'attempt' with the specified 'status' and
    // 'detailedStatus'.
{
    if (attempt->d_terminating.testAndSwap(0, 1)) {

        // this attempt is already being terminated

        return;                                                       // RETURN
    }
    if (attempt->d_negotiation.get()) {
        attempt->d_negotiator.cancelNegotiation(attempt->d_negotiation);
        attempt->d_negotiation.reset();
    }

    if (attempt->d_timer) {
        attempt->d_connector->d_eventManager_p->deregisterTimer(
                                                             attempt->d_timer);
    }
    if (btls5::NetworkConnector::e_SUCCESS == status) {
        bslmt::LockGuard<bslmt::Mutex> guard(&attempt->d_socketLock);
        attempt->d_callback(status,
                            attempt->d_socket_p,
                            attempt->d_connector->d_socketFactory_p,
                            detailedStatus);
    } else {
        {
            bslmt::LockGuard<bslmt::Mutex> guard(&attempt->d_socketLock);
            if (attempt->d_socket_p) {
                attempt->d_connector->d_socketFactory_p->deallocate(
                                                          attempt->d_socket_p);
                attempt->d_socket_p = 0;
            }
        }
        if (btls5::NetworkConnector::e_TIMEOUT == status
         || btls5::NetworkConnector::e_CANCEL  == status
         || !attempt->d_numRetries) {
            attempt->d_callback(status,
                                0,
                                attempt->d_connector->d_socketFactory_p,
                                detailedStatus);
        }
        else {
            // try again, from the beginning

            for (bsl::size_t li = 0;
                 li < attempt->d_connector->d_socks5Servers.numLevels();
                 ++li) {
                attempt->d_indices[li] = 0;
            }
            --attempt->d_numRetries;
            attempt->d_terminating = 0;
            tcpConnect(attempt);
        }
    }
}

static void socksConnectCb(
               btls5::NetworkConnector::ConnectionAttemptHandle attempt,
               btls5::Negotiator::NegotiationStatus             result,
               const btls5::DetailedStatus&                     detailedStatus)
    // Process the specified 'result' of a SOCKS5 negotiation for the specified
    // 'attempt' with the specified 'detailedStatus'.
{
    if (attempt->d_terminating) {
        // callbacks may be invoked asynchronously post-termination

        return;                                                       // RETURN
    }
    attempt->d_negotiation.reset();  // negotiation has concluded

    bsl::size_t level = attempt->d_level;

    if (btls5::Negotiator::e_SUCCESS == result) {
        ++level;

        if (level == attempt->d_connector->d_socks5Servers.numLevels()) {
            btls5::DetailedStatus status("Success");
            terminate(attempt,
                      btls5::NetworkConnector::e_SUCCESS,
                      status);
        } else {
            ++attempt->d_level;

            // for all subsequent levels start from proxy 0

            for (bsl::size_t li = level + 1;
                 li < attempt->d_connector->d_socks5Servers.numLevels();
                 ++li) {
                attempt->d_indices[li] = 0;
            }
            socksConnect(attempt);
        }
    }
    else {
        const bsl::size_t index = attempt->d_indices[level];
            // last failed proxy

        const btls5::ProxyDescription& proxy =
                attempt->d_connector->d_socks5Servers.beginLevel(level)[index];

        // See if we have just tried the last proxy at 'level'.

        while (++attempt->d_indices[level] ==
                     attempt->d_connector->d_socks5Servers.numProxies(level)) {
            if (!level) {
                btls5::DetailedStatus e(detailedStatus);
                e.setAddress(proxy.address());
                terminate(attempt, btls5::NetworkConnector::e_ERROR, e);
                return;                                               // RETURN
            }
            --level;  // try a lower (closer) proxy level
        }

        // More proxies left in this level: close socket, increment index and
        // restart connection.

        for (bsl::size_t li = level + 1;
             li < attempt->d_connector->d_socks5Servers.numLevels();
             ++li) {
            attempt->d_indices[li] = 0;
        }

        {
            bslmt::LockGuard<bslmt::Mutex> guard(&attempt->d_socketLock);
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

static void socksConnect(
               const btls5::NetworkConnector::ConnectionAttemptHandle& attempt)
    // Invoke the SOCKS5 negotiator for the specified 'attempt'.
{
    const bsl::size_t level = attempt->d_level;          // proxy level to try
    const bsl::size_t index = attempt->d_indices[level]; // specific proxy
    const btls5::ProxyDescription& proxy =
                attempt->d_connector->d_socks5Servers.beginLevel(level)[index];

    const bsl::size_t      nextLevel = level + 1;
    const btlso::Endpoint *destination;
    if (nextLevel == attempt->d_connector->d_socks5Servers.numLevels()) {
        destination = &attempt->d_server;
    } else {
        const bsl::size_t nextIndex = attempt->d_indices[nextLevel];

        destination = &attempt->d_connector->d_socks5Servers.
                                    beginLevel(nextLevel)[nextIndex].address();
    }

    using namespace bdlf::PlaceHolders;
    btls5::Negotiator::NegotiationStateCallback cb =
                                           bdlf::BindUtil::bind(socksConnectCb,
                                                                attempt,
                                                                _1,
                                                                _2);

    int rc;
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&attempt->d_socketLock);
        if (attempt->d_socket_p) {
            if (proxy.credentials().username().length()) {
                attempt->d_negotiation =
                     attempt->d_negotiator.makeNegotiationHandle(
                                                       attempt->d_socket_p,
                                                       *destination,
                                                       cb,
                                                       attempt->d_proxyTimeout,
                                                       proxy.credentials());
            } else {
                attempt->d_negotiation =
                     attempt->d_negotiator.makeNegotiationHandle(
                                                      attempt->d_socket_p,
                                                      *destination,
                                                      cb,
                                                      attempt->d_proxyTimeout);
            }
            rc = attempt->d_negotiator.startNegotiation(
                                                       attempt->d_negotiation);
        } else {  // socket has disconnected, e.g. after cancel
            rc = -1;
        }
    }
    if (rc) {
        attempt->d_negotiation.reset();
        socksConnectCb(attempt,
                       btls5::Negotiator::e_ERROR,
                       btls5::DetailedStatus("Unable to negotiate.",
                                             proxy.address()));
    }
}

static void tcpConnectCb(
            btls5::NetworkConnector::ConnectionAttemptHandle connectionAttempt,
            bool                                             hasTimedOut)
    // Process the result of a connection attempt to a first-level proxy in the
    // specified 'connectionAttempt'.  If 'hasTimedOut' is 'false' the TCP
    // connection succeeded or failed, otherwise the connection has timed out.
{
    if (connectionAttempt->d_terminating) {
        // callbacks may be invoked asynchronously post-termination

        return;                                                       // RETURN
    }

    btls5::NetworkConnector::ConnectionAttemptHandle attempt(
                                                            connectionAttempt);
        // copy shared ptr because 'deregisterSocket' removes the reference

    const bsl::size_t index = attempt->d_indices[0];
        // current first-level proxy index

    int rc;
    {
        // This callback may be invoked from the event manager thread before
        // the timeout event is scheduled by the client thread.  Therefore,
        // lock to insure the proxy connection timer is registered.

        bslmt::LockGuard<bslmt::Mutex> guard(&attempt->d_socketLock);

        void *& timer = attempt->d_proxyTimer;
        if (timer) {
            attempt->d_connector->d_eventManager_p->deregisterTimer(timer);
            timer = 0;
        }

        if (attempt->d_socket_p) {
            const btlso::SocketHandle::Handle handle =
                                                 attempt->d_socket_p->handle();
            attempt->d_connector->d_eventManager_p->deregisterSocket(handle);
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
        if (index < attempt->d_connector->d_socks5Servers.numProxies(0) - 1) {
            ++attempt->d_indices[0];  // try the next proximate proxy
            tcpConnect(attempt);
        } else {
            bsl::ostringstream connectionError;
            connectionError
                << "Unable to connect to any proximate proxies"
                << ", last error " << rc
                << ", errno " << errno << ": " << strerror(errno);
            btls5::DetailedStatus error(
                            connectionError.str(),
                            attempt->d_connector->d_socks5Servers.beginLevel(0)
                                                            [index].address());
            terminate(attempt, btls5::NetworkConnector::e_ERROR, error);
        }
    } else {

        // start negotiation on socket

        attempt->d_level = 0;
        socksConnect(connectionAttempt);
    }
}

static btlso::StreamSocket<btlso::IPv4Address> *makeSocket(
             btlso::StreamSocketFactory<btlso::IPv4Address> *socketFactory,
             btls5::DetailedStatus                          *error,
             bool                                            sourceStrict,
             int                                             minSourcePort = 0,
             int                                             maxSourcePort = 0)
    // Allocate a socket suitable for SOCKS5 negotiation using the specified
    // 'socketFactory'.  Return the socket if successful, otherwise return 0
    // and load information into the specified 'error'.  Optionally specify
    // 'minSourcePort' and 'maxSourcePort' to bind the socket to a port in this
    // range.  If 'minSourcePort' and 'maxSourcePort' are 0, let the system
    // select the port; otherwise, try to bind the source port within the range
    // '[minSourcePort .. maxSourcePort]', treating failure as fatal if the
    // specified 'sourceStrict' is 'true'.  The behavior is undefined unless
    // '0 == minSourcePort && 0 == maxSourcePort' or
    // '1 <= minSourcePort <= maxSourcePort <= 65535'.
{
    BSLS_ASSERT((0 == minSourcePort && 0 == maxSourcePort)
        || (1 <= minSourcePort && minSourcePort <= maxSourcePort
                                                   && maxSourcePort <= 65535));

    btlso::StreamSocket<btlso::IPv4Address> *socket =
                                                     socketFactory->allocate();
    if (!socket) {
        error->setDescription("Unable to allocate a socket.");
        return 0;                                                     // RETURN
    }
    btlso::StreamSocketFactoryAutoDeallocateGuard<btlso::IPv4Address>
                                            socketGuard(socket, socketFactory);

    int rc = 0;  // return code for socket operations

#ifndef BSLS_PLATFORM_OS_WINDOWS
    // REUSEADDR on Windows has a different meaning than on UNIX: it allows
    // more than one process to bind to the same port simultaneously, which is
    // not what we want.

    rc = socket->setOption(btlso::SocketOptUtil::k_SOCKETLEVEL,
                           btlso::SocketOptUtil::k_REUSEADDRESS,
                           0);
    if (rc) {
        error->setDescription("Unable to set REUSEADDR option.");
        return 0;                                                     // RETURN
    }
#endif

    if (minSourcePort > 0) {
        int port = minSourcePort;
        for (; port <= maxSourcePort; ++port) {
            btlso::IPv4Address srcAddress(btlso::IPv4Address::k_ANY_ADDRESS,
                                          port);
            rc = socket->bind(srcAddress);
            if (!rc) {
                break;  // bound successfully
            }
        }
        if (port > maxSourcePort && sourceStrict) {
            bsl::ostringstream description;
            description << "Unable to bind source port between "
                        << minSourcePort << " and " << maxSourcePort;
            error->setDescription(description.str());
            return 0;                                                 // RETURN
        }
    }
    rc = socket->setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE);
    if (rc) {
        error->setDescription("Unable to set socket mode to non-blocking.");
        return 0;                                                     // RETURN
    }

    btlso::SocketOptUtil::LingerData lingerData;
    lingerData.l_onoff  = 1;  // enable lingering
    lingerData.l_linger = 0;  // 0 seconds
    rc = socket->setLingerOption(lingerData);
    if (rc) {
        error->setDescription("Unable to set linger option.");
        return 0;                                                     // RETURN
    }

    rc = socket->setOption(btlso::SocketOptUtil::k_SOCKETLEVEL,
                           btlso::SocketOptUtil::k_KEEPALIVE,
                           1);
    if (rc) {
        error->setDescription("Unable to set KEEPALIVE option.");
        return 0;                                                     // RETURN
    }

    rc = socket->setOption(btlso::SocketOptUtil::k_TCPLEVEL,
                           btlso::SocketOptUtil::k_TCPNODELAY,
                           1);
    if (rc) {
        error->setDescription("Unable to set TCPNODELAY option.");
        return 0;                                                     // RETURN
    }
    socketGuard.release();
    return socket;
}

static void tcpConnect(
               const btls5::NetworkConnector::ConnectionAttemptHandle& attempt)
{
    btls5::DetailedStatus error("TCP connect to first-level proxy.");

    btls5::NetworkDescription::ProxyIterator it =
                           attempt->d_connector->d_socks5Servers.beginLevel(0);
    it += attempt->d_indices[0];
    const btls5::NetworkDescription::ProxyIterator end =
                             attempt->d_connector->d_socks5Servers.endLevel(0);
    for (; end != it; ++it, ++attempt->d_indices[0]) {
        const btlso::Endpoint& destination = it->address();
        btlso::IPv4Address server;

        if (btlso::IPv4Address::isValidAddress(
                                             destination.hostname().c_str())) {

            // if 'hostname' just an IP address use it

            server.setIpAddress(destination.hostname().c_str());
        } else {

            // otherwise look up the address

            int errorCode;
            if (btlso::ResolveUtil::getAddress(&server,
                                               destination.hostname().c_str(),
                                               &errorCode)) {
                bsl::ostringstream description;
                description << "Unable to resolve " << destination.hostname()
                            << ", error code "      << errorCode;
                error.setDescription(description.str());
                continue;  // not fatal because there may be more servers
            }
        }
        int rc;
        server.setPortNumber(destination.port());

        // Since we can be either in client or event manager thread, lock
        // socket-related operations to prevent races with event
        // manager-invoked callbacks accessing 'd_socket_p' and 'd_proxyTimer'.

        bslmt::LockGuard<bslmt::Mutex> guard(&attempt->d_socketLock);
        attempt->d_socket_p = makeSocket(
                                       attempt->d_connector->d_socketFactory_p,
                                       &error,
                                       attempt->d_sourceStrict,
                                       attempt->d_connector->d_minSourcePort,
                                       attempt->d_connector->d_maxSourcePort);
        if (!attempt->d_socket_p) {
            continue;  // try again if more servers
        }
        rc = attempt->d_socket_p->connect(server);
        if (!rc) {
            // on immediate success call 'tcpConnectCb' in the event manager
            // thread to avoid nested locks

            attempt->d_proxyTimer = 0;
            const bool hasTimedOut = false;
            attempt->d_connector->d_eventManager_p->execute(
                                            bdlf::BindUtil::bind(tcpConnectCb,
                                                                 attempt,
                                                                 hasTimedOut));
            break;
        } else if (btlso::SocketHandle::e_ERROR_WOULDBLOCK == rc) {
            btlso::EventManager::Callback cb = bdlf::BindUtil::bind(
                                                                  tcpConnectCb,
                                                                  attempt,
                                                                  false);
            if (attempt->d_socket_p) {
                rc = attempt->d_connector->d_eventManager_p->
                             registerSocketEvent(attempt->d_socket_p->handle(),
                                                 btlso::EventType::e_CONNECT,
                                                 cb);
            }

            if (bsls::TimeInterval() != attempt->d_proxyTimeout) {
                bsls::TimeInterval expiration = bdlt::CurrentTime::now()
                                                     + attempt->d_proxyTimeout;
                btlso::EventManager::Callback cb = bdlf::BindUtil::bind(
                                                                  tcpConnectCb,
                                                                  attempt,
                                                                  true);
                attempt->d_proxyTimer =
                         attempt->d_connector->d_eventManager_p->registerTimer(
                                                                    expiration,
                                                                    cb);
            }
            break;  // now wait for connection callback
        } else {
            bsl::ostringstream description;
            description << "Unable to connect: " << rc;
            error.setDescription(description.str());
            error.setAddress(destination);
            continue; // do not terminate because there may be more servers
        }
    }
    if (end == it) {
        terminate(attempt, btls5::NetworkConnector::e_ERROR, error);
        return;                                                       // RETURN
    }
}

static void timeoutAttempt(
            btls5::NetworkConnector::ConnectionAttemptHandle connectionAttempt)
    // Process a timeout for the specified 'connectionAttempt'.
{
    btls5::DetailedStatus error("Connection attempt timed out.");
    terminate(connectionAttempt, btls5::NetworkConnector::e_TIMEOUT, error);
}

}  // close unnamed namespace

                          // -----------------------
                          // class ConnectionAttempt
                          // -----------------------

// CREATORS
btls5::NetworkConnector::ConnectionAttempt::ConnectionAttempt(
                             const ConnectionStateCallback&     callback,
                             const bsls::TimeInterval&          proxyTimeout,
                             const bsls::TimeInterval&          totalTimeout,
                             const btlso::Endpoint&             server,
                             const bsl::shared_ptr<Connector>&  connector,
                             bslma::Allocator                  *basicAllocator)
: d_callback(bsl::allocator_arg_t(),
             bsl::allocator<ConnectionStateCallback>(basicAllocator),
             callback)
, d_proxyTimeout(proxyTimeout)
, d_totalTimeout(totalTimeout)
, d_server(server, basicAllocator)
, d_connector(connector)
, d_timer(0)
, d_proxyTimer(0)
, d_level(0)
, d_indices(connector->d_socks5Servers.numLevels(), 0, basicAllocator)
, d_socket_p(0)
, d_numRetries(BTLS5_NETWORKCONNECTOR_RETRIES)
, d_negotiator(d_connector->d_eventManager_p, basicAllocator)
, d_negotiation()
, d_sourceStrict(false)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT(basicAllocator);
}

                           // ----------------------
                           // class NetworkConnector
                           // ----------------------

// CREATORS
NetworkConnector::NetworkConnector(
                const NetworkDescription&                       socks5Servers,
                btlso::StreamSocketFactory<btlso::IPv4Address> *socketFactory,
                btlmt::TcpTimerEventManager                    *eventManager,
                int                                             minSourcePort,
                int                                             maxSourcePort,
                bslma::Allocator                               *basicAllocator)
{
    BSLS_ASSERT(NetworkDescriptionUtil::isWellFormed(socks5Servers));
        // 'isWellFormed' is relatively much cheaper than multiple asynchronous
        // IO operations (including memory allocation, system calls, and
        // network latencies) involved in traversing a proxy network.

    basicAllocator = bslma::Default::allocator(basicAllocator);
    d_connector.createInplace(basicAllocator,
                              socks5Servers,
                              socketFactory,
                              eventManager,
                              minSourcePort,
                              maxSourcePort,
                              basicAllocator);
}

NetworkConnector::NetworkConnector(
                const NetworkDescription&                       socks5Servers,
                btlso::StreamSocketFactory<btlso::IPv4Address> *socketFactory,
                btlmt::TcpTimerEventManager                    *eventManager,
                bslma::Allocator                               *basicAllocator)
{
    BSLS_ASSERT(NetworkDescriptionUtil::isWellFormed(socks5Servers));
        // 'isWellFormed' is relatively much cheaper than multiple asynchronous
        // IO operations (including memory allocation, system calls, and
        // network latencies) involved in traversing a proxy network.

    basicAllocator = bslma::Default::allocator(basicAllocator);
    d_connector.createInplace(basicAllocator,
                              socks5Servers,
                              socketFactory,
                              eventManager,
                              0,
                              0,
                              basicAllocator);
}

// MANIPULATORS
NetworkConnector::ConnectionAttemptHandle
NetworkConnector::makeConnectionAttemptHandle(
                                   const ConnectionStateCallback& callback,
                                   const bsls::TimeInterval&      proxyTimeout,
                                   const bsls::TimeInterval&      totalTimeout,
                                   const btlso::Endpoint&         server)
{
    ConnectionAttemptHandle attempt;
    attempt.createInplace(d_connector->d_allocator_p,
                          callback,
                          proxyTimeout,
                          totalTimeout,
                          server,
                          d_connector,
                          d_connector->d_allocator_p);
    return attempt;
}

void NetworkConnector::startConnectionAttempt(
                                         const ConnectionAttemptHandle& handle)
{
    BSLS_ASSERT(d_connector->d_eventManager_p->isEnabled());

    if (bsls::TimeInterval() != handle->d_totalTimeout) {
        bsls::TimeInterval expiration =
                             bdlt::CurrentTime::now() + handle->d_totalTimeout;
        btlso::EventManager::Callback cb = bdlf::BindUtil::bind(timeoutAttempt,
                                                                handle);
        handle->d_timer = handle->d_connector->d_eventManager_p->registerTimer(
                                                                    expiration,
                                                                    cb);
    }
    tcpConnect(handle);
}

void NetworkConnector::startConnectionAttemptStrict(
                                         const ConnectionAttemptHandle& handle)
{
    handle->d_sourceStrict = true;
    startConnectionAttempt(handle);
}

void NetworkConnector::cancelConnectionAttempt(
                                         const ConnectionAttemptHandle& handle)
{
    terminate(handle, NetworkConnector::e_CANCEL, DetailedStatus("Canceled"));
}

// ACCESSORS
const NetworkDescription&
NetworkConnector::socks5Servers() const
{
    return d_connector->d_socks5Servers;
}

}  // close package namespace

}  // close enterprise namespace

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
