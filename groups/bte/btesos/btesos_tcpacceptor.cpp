// btesos_tcpacceptor.cpp    -*-C++-*-
#include <btesos_tcpacceptor.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btesos_tcpacceptor_cpp,"$Id$ $CSID$")

#include <btesos_tcptimedchannel.h>
#include <btesos_tcpchannel.h>
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
    BLOCKING_MODE    = bteso_Flag::BTESO_BLOCKING_MODE,
    NONBLOCKING_MODE = bteso_Flag::BTESO_NONBLOCKING_MODE
};

enum {
    ARENA_SIZE = sizeof(btesos_TcpChannel) < sizeof(btesos_TcpTimedChannel)
                 ? sizeof(btesos_TcpTimedChannel)
                 : sizeof(btesos_TcpChannel)
};

enum {
    INVALID       = -4,
    FAILED        = -3,
    UNINITIALIZED = -2,
    CANCELLED     = -1,
    SUCCESS       =  0
};

                       // =======================
                       // local function allocate
                       // =======================

template <class RESULT>
inline
RESULT *allocate(int *status, int flags,
                 bteso_StreamSocket<bteso_IPv4Address> *socket,
                 bdema_Pool *pool)
{
    BSLS_ASSERT(socket);
    BSLS_ASSERT(pool);
    BSLS_ASSERT(status);

    // Bring the listening socket into blocking mode.
    int rc = socket->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    bteso_IPv4Address peer;
    bteso_StreamSocket<bteso_IPv4Address> *acceptedConnection = 0;
    while(1) {
        int s = socket->accept(&acceptedConnection, &peer);

        if (acceptedConnection) { break; }

        if (bteso_SocketHandle::BTESO_ERROR_INTERRUPTED != s) {
            *status = FAILED;
            socket->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);
            return NULL;
        }

        if (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT) {
            *status = 1;  // Any positive number satisfies the contract.
            socket->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);

            return NULL;
        }
    }

    socket->setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE);

    RESULT *channel =
        new (*pool) RESULT(acceptedConnection);

    return channel;
}

// ============================================================================
//                        END LOCAL DEFINITIONS
// ============================================================================

                          // ------------------------
                          // class btesos_TcpAcceptor
                          // ------------------------

// CREATORS

btesos_TcpAcceptor::btesos_TcpAcceptor(
        bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
        bslma_Allocator                              *basicAllocator)
: d_pool(ARENA_SIZE, basicAllocator)
, d_channels(basicAllocator)
, d_factory_p(factory)
, d_serverSocket_p(0)
, d_isInvalidFlag(0)
{

    BSLS_ASSERT(d_factory_p);
}

btesos_TcpAcceptor::btesos_TcpAcceptor(
        bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
        int                                           initialCapacity,
        bslma_Allocator                              *basicAllocator)
: d_pool(ARENA_SIZE,
         bsls_BlockGrowth::BSLS_CONSTANT,
         initialCapacity,
         basicAllocator)
, d_channels(basicAllocator)
, d_factory_p(factory)
, d_serverSocket_p(0)
, d_isInvalidFlag(0)
{
    BSLS_ASSERT(d_factory_p);
    BSLS_ASSERT(0 < initialCapacity);
}

btesos_TcpAcceptor::~btesos_TcpAcceptor() {
    // INVARIANTS: If listening socket is open, it is in a non-blocking mode.

    invalidate();
    BSLS_ASSERT(d_factory_p);

    if (d_serverSocket_p) {
        close();
    }
    // Deallocate channels
    while (d_channels.size()) {
        btesc_Channel *ch = d_channels[d_channels.size()-1];
        BSLS_ASSERT(ch);
        ch->invalidate();
        deallocate(ch);
    }
    BSLS_ASSERT(0 == d_channels.size());
}

// MANIPULATORS

void btesos_TcpAcceptor::deallocate(btesc_Channel *channel) {
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

btesc_Channel *btesos_TcpAcceptor::allocate(int *status, int flags)
{
    BSLS_ASSERT(status);

    if (d_isInvalidFlag || NULL == d_serverSocket_p) {
        if (NULL == d_serverSocket_p) {
            *status = UNINITIALIZED;
        }
        else {
            *status = INVALID;
        }
        return NULL;
    }

    btesos_TcpChannel *channel = BloombergLP::allocate<btesos_TcpChannel> (
                                                         status,
                                                         flags,
                                                         d_serverSocket_p,
                                                         &d_pool);

    if (channel) {
        bsl::vector<btesc_Channel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(),
                                static_cast<btesc_Channel*>(channel));
        d_channels.insert(idx, channel);
    }
    return channel;
}

btesc_TimedChannel *btesos_TcpAcceptor::allocateTimed(int *status,
                                                      int flags)
{
    BSLS_ASSERT(status);
    if (d_isInvalidFlag || NULL == d_serverSocket_p) {
        if (NULL == d_serverSocket_p) {
            *status = UNINITIALIZED;
        }
        else {
            *status = INVALID;
        }
        return NULL;
    }

    btesc_TimedChannel *channel =
        BloombergLP::allocate<btesos_TcpTimedChannel>(status, flags,
                                                      d_serverSocket_p,
                                                      &d_pool);
    if (channel) {
        bsl::vector<btesc_Channel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(),
                                static_cast<btesc_Channel*>(channel));
        d_channels.insert(idx, channel);
    }
    return channel;
}

int btesos_TcpAcceptor::close() {
    BSLS_ASSERT(NULL != d_serverSocket_p);
    BSLS_ASSERT(d_serverAddress.portNumber());  // Address is valid.

    d_factory_p->deallocate(d_serverSocket_p);
    d_serverSocket_p = NULL;
    d_serverAddress = bteso_IPv4Address();
    return SUCCESS;
}

int btesos_TcpAcceptor::open(const bteso_IPv4Address& endpoint,
                                  int queueSize, int reuseAddressFlag) {
    BSLS_ASSERT(0 < queueSize);
    BSLS_ASSERT(NULL == d_serverSocket_p);

    enum {
        CANT_SET_OPTIONS     = -7,
        INVALID_ACCEPTOR     = -6,
        ALLOCATION_FAILED    = -5,
        BIND_FAILED          = -4,
        LISTEN_FAILED        = -3,
        LOCALINFO_FAILED     = -2,
        BLOCKMODE_FAILED     = -1
    };

    if (d_isInvalidFlag) {
        return INVALID_ACCEPTOR;
    }
    d_serverSocket_p = d_factory_p->allocate();
    if (!d_serverSocket_p) {
        return ALLOCATION_FAILED;
    }

    if (reuseAddressFlag) {
        if (0 != d_serverSocket_p->setOption(
                                       bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                       bteso_SocketOptUtil::BTESO_REUSEADDRESS,
                                       reuseAddressFlag)) {
            d_factory_p->deallocate(d_serverSocket_p);
            d_serverSocket_p = NULL;
            return CANT_SET_OPTIONS;
        }
    }

    if (0 != d_serverSocket_p->bind(endpoint)) {
        d_factory_p->deallocate(d_serverSocket_p);
        d_serverSocket_p = NULL;
        return BIND_FAILED;
    }

    if (0 != d_serverSocket_p->localAddress(&d_serverAddress)) {
        d_factory_p->deallocate(d_serverSocket_p);
        d_serverSocket_p = NULL;
        return BIND_FAILED;
    }
    BSLS_ASSERT(d_serverAddress.portNumber());

    if (0 != d_serverSocket_p->listen(queueSize)) {
        d_factory_p->deallocate(d_serverSocket_p);
        d_serverSocket_p = NULL;
        return LISTEN_FAILED;
    }

    if (0 != d_serverSocket_p->setBlockingMode(
                                         bteso_Flag::BTESO_NONBLOCKING_MODE)) {
        d_factory_p->deallocate(d_serverSocket_p);
        d_serverSocket_p = NULL;
        return BLOCKMODE_FAILED;
    }
    return SUCCESS;
}

int btesos_TcpAcceptor::setOption(int level, int option, int value)
{
    BSLS_ASSERT(!d_isInvalidFlag);
    BSLS_ASSERT(d_serverSocket_p);

    return d_serverSocket_p->setOption(level, option, value);
}

// ACCESSORS

int btesos_TcpAcceptor::getOption(int *result, int level, int option) const
{
    BSLS_ASSERT(!d_isInvalidFlag);
    BSLS_ASSERT(d_serverSocket_p);

    return d_serverSocket_p->socketOption(result, level, option);
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
