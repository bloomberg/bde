// btesos_tcptimedconnector.cpp    -*-C++-*-
#include <btesos_tcptimedconnector.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btesos_tcptimedconnector_cpp,"$Id$ $CSID$")

#include <btesos_tcpchannel.h>
#include <btesos_tcptimedchannel.h>
#include <bteso_streamsocketfactory.h>
#include <bteso_streamsocket.h>
#include <btesc_flag.h>

#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bsls_assert.h>
#include <bsls_blockgrowth.h>

#include <bsl_algorithm.h>
#include <bsl_vector.h>

namespace BloombergLP {

// ============================================================================
//                        LOCAL DEFINITIONS
// ============================================================================

                       // ========================
                       // Local typedefs and enums
                       // ========================

enum {
    ARENA_SIZE    = sizeof(btesos_TcpChannel) < sizeof(btesos_TcpTimedChannel)
                    ? sizeof(btesos_TcpTimedChannel)
                    : sizeof(btesos_TcpChannel) };

enum {
    INVALID       = -4,
    FAILED        = -3,
    PEER_UNSET    = -2,
    CANCELLED     = -1,
    SUCCESS       =  0
};

template <class RESULT>
inline
RESULT *allocate(int                                          *status,
                 int                                           flags,
                 const bteso_IPv4Address&                      peerAddress,
                 bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
                 bdema_Pool                                   *pool)
{
    BSLS_ASSERT(factory); BSLS_ASSERT(pool); BSLS_ASSERT(status);

    bteso_StreamSocket<bteso_IPv4Address> *socket_p = 0;
    socket_p = factory->allocate();
    if (!socket_p) {
        return NULL;
    }
    int rc = socket_p->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);

    BSLS_ASSERT(0 == rc);
    while(1) {
        int s = socket_p->connect(peerAddress);

        if (0 == s) break;
        if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED != s) {
            *status = FAILED; // Any negative number satisfies the contract.
            factory->deallocate(socket_p);
            return NULL;
        }
        if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {
            *status = 1;  // Any positive number satisfies the contract.
            factory->deallocate(socket_p);
            return NULL;
        }
    }
    RESULT *channel = new (*pool) RESULT(socket_p);
    return channel;
}

template <class RESULT>
inline
RESULT *timedAllocate(int                                         *status,
                     int                                           flags,
                     const bteso_IPv4Address&                      peerAddress,
                     bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
                     bdema_Pool                                   *pool,
                     const bdet_TimeInterval&                      timeout)
{
    BSLS_ASSERT(factory); BSLS_ASSERT(pool); BSLS_ASSERT(status);

    bteso_StreamSocket<bteso_IPv4Address> *socket = factory->allocate();
    if (!socket) {
        return NULL;
    }

    int rc = socket->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    int s = socket->connect(peerAddress);

    if (bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK == s) {
        do {
            errno = 0;
            s = socket->waitForConnect(timeout);

            if (0 == s) {
                s = socket->connectionStatus();
                if (s == 0) {
                    break; // Connection has been established immediately.
                }
            }
            if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED == s) {
                ;                 // 3/1/2004: To be updated after
                                  // 'bteso_inetstreamsocketfactory' in effect.
            }
            else if (s == bteso_SocketHandle::BTESO_ERROR_TIMEDOUT) {
                *status = 0;
                factory->deallocate(socket);
                return NULL;
            }
            else if (s && bteso_SocketHandle::BTESO_ERROR_INTERRUPTED != s) {
                *status = FAILED;
                factory->deallocate(socket);
                return NULL;
            }
            if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {
                *status = 1;  // Any positive number satisfies the contract.
                factory->deallocate(socket);
                return NULL;
            }

        } while (bdetu_SystemTime::now() <= timeout);
    }
    else if (0 != s) {
        if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED != s) {
            *status = FAILED;
            factory->deallocate(socket);
            return NULL;
        }
        if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {
            *status = 1;  // Any positive number satisfies the contract.
            factory->deallocate(socket);
            return NULL;
        }
    }

    RESULT *channel = new (*pool) RESULT(socket);
    return channel;
}

                       // ================================
                       // class btesos_TcpTimedCbConnector
                       // ================================

// CREATORS
btesos_TcpTimedConnector::btesos_TcpTimedConnector(
                 bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
                 bslma_Allocator                              *basicAllocator)
: d_pool(ARENA_SIZE, basicAllocator)
, d_channels(basicAllocator)
, d_factory_p(factory)
, d_isInvalidFlag(0)
{
    BSLS_ASSERT(d_factory_p);
}

btesos_TcpTimedConnector::btesos_TcpTimedConnector(
                 bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
                 int                                           numElements,
                 bslma_Allocator                              *basicAllocator)
: d_pool(ARENA_SIZE,
         bsls_BlockGrowth::BSLS_CONSTANT,
         numElements,
         basicAllocator)
, d_channels(basicAllocator)
, d_factory_p(factory)
, d_isInvalidFlag(0)
{
    BSLS_ASSERT(d_factory_p);
    BSLS_ASSERT(0 <= numElements);
}

btesos_TcpTimedConnector::~btesos_TcpTimedConnector()
{
    BSLS_ASSERT(d_factory_p);

    // Deallocate channels.
    while(d_channels.size()) {
        btesc_Channel *ch = d_channels[0];
        BSLS_ASSERT(ch);
        ch->invalidate();
        deallocate(ch);
    }
}

// MANIPULATORS
btesc_Channel *btesos_TcpTimedConnector::allocate(int *status, int flags)
{
    BSLS_ASSERT(status);

    if (d_isInvalidFlag || bteso_IPv4Address::BTESO_ANY_PORT ==
                                                  d_peerAddress.portNumber()) {
        if (bteso_IPv4Address::BTESO_ANY_PORT == d_peerAddress.portNumber()) {
            *status = PEER_UNSET;
        }
        else {
            *status = INVALID;
        }
        return NULL;
    }

    btesc_Channel *channel = BloombergLP::allocate<btesos_TcpChannel>(
                                                          status,
                                                          flags,
                                                          d_peerAddress,
                                                          d_factory_p,
                                                          &d_pool);
    if (channel) {
        bsl::vector<btesc_Channel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(), channel);
        d_channels.insert(idx, channel);
    }
    return channel;
}

btesc_TimedChannel *btesos_TcpTimedConnector::allocateTimed(int *status,
                                                            int  flags)
{
    BSLS_ASSERT(status);

    if (d_isInvalidFlag || bteso_IPv4Address::BTESO_ANY_PORT ==
                                                  d_peerAddress.portNumber()) {
        if (bteso_IPv4Address::BTESO_ANY_PORT == d_peerAddress.portNumber()) {
            *status = PEER_UNSET;
        }
        else {
            *status = INVALID;
        }
        return NULL;
    }

    btesc_TimedChannel *channel =
        BloombergLP::allocate<btesos_TcpTimedChannel>(status,
                                                      flags,
                                                      d_peerAddress,
                                                      d_factory_p,
                                                      &d_pool);
    if (channel) {
        bsl::vector<btesc_Channel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(),
                                static_cast<btesc_Channel*>(channel));
        d_channels.insert(idx, channel);
    }
    return channel;
}

btesc_Channel *btesos_TcpTimedConnector::timedAllocate(
                                            int                      *status,
                                            const bdet_TimeInterval&  timeout,
                                            int                       flags)
{
    BSLS_ASSERT(status);

    if (d_isInvalidFlag || bteso_IPv4Address::BTESO_ANY_PORT ==
                                                  d_peerAddress.portNumber()) {
        if (bteso_IPv4Address::BTESO_ANY_PORT == d_peerAddress.portNumber()) {
            *status = PEER_UNSET;
        }
        else {
            *status = INVALID;
        }
        return NULL;
    }

    btesc_Channel *channel =
        BloombergLP::timedAllocate<btesos_TcpChannel>(status,
                                                      flags,
                                                      d_peerAddress,
                                                      d_factory_p,
                                                      &d_pool,
                                                      timeout);
    if (channel) {
        bsl::vector<btesc_Channel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(), channel);
        d_channels.insert(idx, channel);
    }
    return channel;
}

btesc_TimedChannel *btesos_TcpTimedConnector::timedAllocateTimed(
                                           int                      *status,
                                           const bdet_TimeInterval&  timeout,
                                           int                       flags)
{
    BSLS_ASSERT(status);

    if (d_isInvalidFlag || bteso_IPv4Address::BTESO_ANY_PORT ==
                                                  d_peerAddress.portNumber()) {
        if (bteso_IPv4Address::BTESO_ANY_PORT == d_peerAddress.portNumber()) {
            *status = PEER_UNSET;
        }
        else {
            *status = INVALID;
        }
        return NULL;
    }

    btesc_TimedChannel *channel =
        BloombergLP::timedAllocate<btesos_TcpTimedChannel>(status,
                                                           flags,
                                                           d_peerAddress,
                                                           d_factory_p,
                                                           &d_pool,
                                                           timeout);
    if (channel) {
        bsl::vector<btesc_Channel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(),
                                static_cast<btesc_Channel*>(channel));
        d_channels.insert(idx, channel);
    }
    return channel;
}

void btesos_TcpTimedConnector::deallocate(btesc_Channel *channel)
{
    BSLS_ASSERT(channel);
    char *arena = (char *) channel;
    btesos_TcpTimedChannel *c =
        dynamic_cast<btesos_TcpTimedChannel*>(channel);
    bteso_StreamSocket<bteso_IPv4Address> *s = NULL;

    if (c) {
        s = c->socket();
    }
    else {
        btesos_TcpChannel *c =
            dynamic_cast<btesos_TcpChannel*>(channel);
        BSLS_ASSERT(c);
        s = c->socket();
    }
    BSLS_ASSERT(s);

    d_factory_p->deallocate(s);
    channel->~btesc_Channel();

    bsl::vector<btesc_Channel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(), channel);
    BSLS_ASSERT(idx != d_channels.end() && *idx == channel);
    d_channels.erase(idx);

    d_pool.deallocate(arena);
    return ;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
