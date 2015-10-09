// btlsos_tcpconnector.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlsos_tcpconnector.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlsos_tcpconnector_cpp,"$Id$ $CSID$")

#include <btlsos_tcpchannel.h>
#include <btlsos_tcptimedchannel.h>
#include <btlso_streamsocketfactory.h>
#include <btlso_streamsocket.h>
#include <btlsc_flag.h>

#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>

#include <bsls_assert.h>
#include <bsls_blockgrowth.h>

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>
#include <bsl_vector.h>

namespace BloombergLP {

// ============================================================================
//                             LOCAL DEFINITIONS
// ============================================================================

                     // ===============================
                     // Local typedefs and enumerations
                     // ===============================

enum {
    k_ARENA_SIZE = sizeof(btlsos::TcpChannel) < sizeof(btlsos::TcpTimedChannel)
                 ? sizeof(btlsos::TcpTimedChannel)
                 : sizeof(btlsos::TcpChannel)
};

enum {
    e_INVALID       = -4,
    e_FAILED        = -3,
    e_PEER_UNSET    = -2,
    e_CANCELLED     = -1,
    e_SUCCESS       =  0
};

                         // =======================
                         // local function allocate
                         // =======================

template <class RESULT>
inline
    RESULT *allocate(
                   int                                            *status,
                   int                                             flags,
                   const btlso::IPv4Address&                       peerAddress,
                   btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
                   bdlma::Pool                                    *pool)
{
    BSLS_ASSERT(factory);
    BSLS_ASSERT(pool);
    BSLS_ASSERT(status);

    btlso::StreamSocket<btlso::IPv4Address> *socket_p = 0;
    socket_p = factory->allocate();
    if (!socket_p) {
        return NULL;                                                  // RETURN
    }
    int rc = socket_p->setBlockingMode(btlso::Flag::e_BLOCKING_MODE);

    BSLS_ASSERT(0 == rc);
    while (1) {
        int s = socket_p->connect(peerAddress);

        if (0 == s) break;
        if (btlso::SocketHandle::e_ERROR_INTERRUPTED != s) {
            *status = e_FAILED; // Any negative number satisfies the contract.
            factory->deallocate(socket_p);
            return NULL;                                              // RETURN
        }
        if (flags & btlsc::Flag::k_ASYNC_INTERRUPT) {
            *status = 1;  // Any positive number satisfies the contract.
            factory->deallocate(socket_p);
            return NULL;                                              // RETURN
        }
    }
    RESULT *channel = new (*pool) RESULT(socket_p);
    return channel;
}

namespace btlsos {

// ============================================================================
//                             LOCAL DEFINITIONS
// ============================================================================

                            // ------------------
                            // class TcpConnector
                            // ------------------

// CREATORS
TcpConnector::TcpConnector(
                btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
                bslma::Allocator                               *basicAllocator)
: d_pool(k_ARENA_SIZE, basicAllocator)
, d_channels(basicAllocator)
, d_factory_p(factory)
, d_isInvalidFlag(0)
{
    BSLS_ASSERT(d_factory_p);
}

TcpConnector::TcpConnector(
                btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
                int                                             numElements,
                bslma::Allocator                               *basicAllocator)
: d_pool(k_ARENA_SIZE,
         bsls::BlockGrowth::BSLS_CONSTANT,
         numElements,
         basicAllocator)
, d_channels(basicAllocator)
, d_factory_p(factory)
, d_isInvalidFlag(0)
{
    BSLS_ASSERT(d_factory_p);
    BSLS_ASSERT(0 <= numElements);
}

TcpConnector::~TcpConnector()
{
    BSLS_ASSERT(d_factory_p);

    // Deallocate channels.
    while (d_channels.size()) {
        btlsc::Channel *ch = d_channels[0];
        BSLS_ASSERT(ch);
        ch->invalidate();
        deallocate(ch);
    }
}

// MANIPULATORS
btlsc::Channel *TcpConnector::allocate(int *status, int flags)
{
    BSLS_ASSERT(status);

    if (d_isInvalidFlag ||  btlso::IPv4Address::k_ANY_PORT ==
        d_peerAddress.portNumber())
    {
        if (btlso::IPv4Address::k_ANY_PORT == d_peerAddress.portNumber()) {
            *status = e_PEER_UNSET;
        }
        else {
            *status = e_INVALID;
        }
        return NULL;                                                  // RETURN
    }

    btlsc::Channel *channel = BloombergLP::allocate<TcpChannel>(
                                                          status,
                                                          flags,
                                                          d_peerAddress,
                                                          d_factory_p,
                                                          &d_pool);
    if (channel) {
        bsl::vector<btlsc::Channel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(), channel);
        d_channels.insert(idx, channel);
    }
    return channel;
}

btlsc::TimedChannel *TcpConnector::allocateTimed(int *status, int flags)
{
    BSLS_ASSERT(status);

    if (d_isInvalidFlag ||  btlso::IPv4Address::k_ANY_PORT ==
                                               d_peerAddress.portNumber())
    {
        if (btlso::IPv4Address::k_ANY_PORT == d_peerAddress.portNumber()) {
            *status = e_PEER_UNSET;
        }
        else {
            *status = e_INVALID;
        }
        return NULL;                                                  // RETURN
    }

    btlsc::TimedChannel *channel =
        BloombergLP::allocate<TcpTimedChannel>(status,
                                                      flags,
                                                      d_peerAddress,
                                                      d_factory_p,
                                                      &d_pool);
    if (channel) {
        bsl::vector<btlsc::Channel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(),
                                static_cast<btlsc::Channel*>(channel));
        d_channels.insert(idx, channel);
    }
    return channel;
}

void TcpConnector::deallocate(btlsc::Channel *channel)
{
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

    channel->invalidate();
    d_factory_p->deallocate(s);

    bsl::vector<btlsc::Channel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(), channel);
    BSLS_ASSERT(idx != d_channels.end() && *idx == channel);
    d_channels.erase(idx);

    d_pool.deallocate(arena);
    return ;
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
