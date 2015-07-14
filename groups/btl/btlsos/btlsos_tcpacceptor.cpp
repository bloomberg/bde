// btlsos_tcpacceptor.cpp    -*-C++-*-
#include <btlsos_tcpacceptor.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlsos_tcpacceptor_cpp,"$Id$ $CSID$")

#include <btlsos_tcptimedchannel.h>
#include <btlsos_tcpchannel.h>
#include <btlso_streamsocketfactory.h>
#include <btlso_streamsocket.h>
#include <btlsc_flag.h>

#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>

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
    ARENA_SIZE = sizeof(btlsos::TcpChannel) < sizeof(btlsos::TcpTimedChannel)
                 ? sizeof(btlsos::TcpTimedChannel)
                 : sizeof(btlsos::TcpChannel)
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
                 btlso::StreamSocket<btlso::IPv4Address> *socket,
                 bdlma::Pool *pool)
{
    BSLS_ASSERT(socket);
    BSLS_ASSERT(pool);
    BSLS_ASSERT(status);

    // Bring the listening socket into blocking mode.
    int rc = socket->setBlockingMode(bteso_Flag::BTESO_BLOCKING_MODE);
    BSLS_ASSERT(0 == rc);

    btlso::IPv4Address peer;
    btlso::StreamSocket<btlso::IPv4Address> *acceptedConnection = 0;
    while(1) {
        int s = socket->accept(&acceptedConnection, &peer);

        if (acceptedConnection) { break; }

        if (btlso::SocketHandle::BTESO_ERROR_INTERRUPTED != s) {
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

namespace btlsos {
// ============================================================================
//                        END LOCAL DEFINITIONS
// ============================================================================

                          // ------------------------
                          // class TcpAcceptor
                          // ------------------------

// CREATORS

TcpAcceptor::TcpAcceptor(
        btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
        bslma::Allocator                             *basicAllocator)
: d_pool(ARENA_SIZE, basicAllocator)
, d_channels(basicAllocator)
, d_factory_p(factory)
, d_serverSocket_p(0)
, d_isInvalidFlag(0)
{

    BSLS_ASSERT(d_factory_p);
}

TcpAcceptor::TcpAcceptor(
        btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
        int                                           initialCapacity,
        bslma::Allocator                             *basicAllocator)
: d_pool(ARENA_SIZE,
         bsls::BlockGrowth::BSLS_CONSTANT,
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

TcpAcceptor::~TcpAcceptor() {
    // INVARIANTS: If listening socket is open, it is in a non-blocking mode.

    invalidate();
    BSLS_ASSERT(d_factory_p);

    if (d_serverSocket_p) {
        close();
    }
    // Deallocate channels
    while (d_channels.size()) {
        btlsc::Channel *ch = d_channels[d_channels.size()-1];
        BSLS_ASSERT(ch);
        ch->invalidate();
        deallocate(ch);
    }
    BSLS_ASSERT(0 == d_channels.size());
}

// MANIPULATORS

void TcpAcceptor::deallocate(btlsc::Channel *channel) {
    BSLS_ASSERT(channel);
    char *arena = (char *) channel;
    TcpTimedChannel *c =
        dynamic_cast<TcpTimedChannel*>(channel);
    btlso::StreamSocket<btlso::IPv4Address> *s = NULL;

    if (c) {
        s = c->socket();
    }
    else {
        TcpChannel *c =
            dynamic_cast<TcpChannel*>(channel);
        BSLS_ASSERT(c);
        s = c->socket();
    }
    BSLS_ASSERT(s);

    d_factory_p->deallocate(s);
    channel->~Channel();

    bsl::vector<btlsc::Channel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(), channel);
    BSLS_ASSERT(idx != d_channels.end() && *idx == channel);
    d_channels.erase(idx);

    d_pool.deallocate(arena);
    return ;
}

btlsc::Channel *TcpAcceptor::allocate(int *status, int flags)
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

    TcpChannel *channel = BloombergLP::allocate<TcpChannel> (
                                                         status,
                                                         flags,
                                                         d_serverSocket_p,
                                                         &d_pool);

    if (channel) {
        bsl::vector<btlsc::Channel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(),
                                static_cast<btlsc::Channel*>(channel));
        d_channels.insert(idx, channel);
    }
    return channel;
}

btlsc::TimedChannel *TcpAcceptor::allocateTimed(int *status,
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

    btlsc::TimedChannel *channel =
        BloombergLP::allocate<TcpTimedChannel>(status, flags,
                                                      d_serverSocket_p,
                                                      &d_pool);
    if (channel) {
        bsl::vector<btlsc::Channel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(),
                                static_cast<btlsc::Channel*>(channel));
        d_channels.insert(idx, channel);
    }
    return channel;
}

int TcpAcceptor::close() {
    BSLS_ASSERT(NULL != d_serverSocket_p);
    BSLS_ASSERT(d_serverAddress.portNumber());  // Address is valid.

    d_factory_p->deallocate(d_serverSocket_p);
    d_serverSocket_p = NULL;
    d_serverAddress = btlso::IPv4Address();
    return SUCCESS;
}

int TcpAcceptor::open(const btlso::IPv4Address& endpoint,
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
                                       btlso::SocketOptUtil::BTESO_SOCKETLEVEL,
                                       btlso::SocketOptUtil::BTESO_REUSEADDRESS,
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

int TcpAcceptor::setOption(int level, int option, int value)
{
    BSLS_ASSERT(!d_isInvalidFlag);
    BSLS_ASSERT(d_serverSocket_p);

    return d_serverSocket_p->setOption(level, option, value);
}

// ACCESSORS

int TcpAcceptor::getOption(int *result, int level, int option) const
{
    BSLS_ASSERT(!d_isInvalidFlag);
    BSLS_ASSERT(d_serverSocket_p);

    return d_serverSocket_p->socketOption(result, level, option);
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
