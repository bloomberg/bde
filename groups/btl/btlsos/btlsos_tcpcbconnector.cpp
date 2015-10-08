// btlsos_tcpcbconnector.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlsos_tcpcbconnector.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlsos_tcpcbconnector_cpp,"$Id$ $CSID$")

#include <btlsos_tcptimedcbchannel.h>
#include <btlsos_tcpcbchannel.h>
#include <btlso_timereventmanager.h>
#include <btlso_streamsocketfactory.h>
#include <btlso_streamsocket.h>
#include <btlso_eventtype.h>
#include <btlsc_flag.h>

#include <bdlt_currenttime.h>

#include <bdlf_memfn.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_timeinterval.h>

#include <bslalg_scalardestructionprimitives.h>

#include <bsl_cstddef.h>
#include <bsl_functional.h>
#include <bsl_iterator.h>
#include <bsl_memory.h>

#ifdef TEST
// These dependencies will cause the test driver to recompile when the concrete
// implementation of the event manager changes.
#include <btlso_tcptimereventmanager.h>
#endif

#include <bsl_algorithm.h>
#include <bsl_vector.h>

// ============================================================================
//                          IMPLEMENTATION DETAILS
// ----------------------------------------------------------------------------
// 1.  Internally, this connector holds a queue of callbacks for allocation
// requests.  The queue contains both timed and non-timed callbacks along with
// any supporting data for a request, such as the timeout value, if any, and
// flags.
//
// 2.  The request queue keeps the type of the result (i.e., timed or non-timed
// channel) and the request can invoke the callback in a type-safe manner.
//
// 3.  Whenever an accept callback is registered with the socket event manager,
// the 'd_isRegisteredFlag' is set.
//
// 4.  All allocate methods register callbacks with a socket event manager if
// the request queue size is 1 (i.e., when only the current request is cached).
// An allocate method can set the value of 'd_isRegisteredFlag' to 1, but not
// to 0 (i.e., it cannot deregister the accept callback).
//
// ============================================================================

namespace BloombergLP {

// ============================================================================
//                             LOCAL DEFINITIONS
// ============================================================================

                     // ===============================
                     // Local typedefs and enumerations
                     // ===============================

enum {
    k_CALLBACK_SIZE      = sizeof(btlsc::CbChannelAllocator::Callback),
    k_TIMEDCALLBACK_SIZE = sizeof (btlsc::CbChannelAllocator::TimedCallback),
    k_ARENA_SIZE         = k_CALLBACK_SIZE < k_TIMEDCALLBACK_SIZE
                         ? k_TIMEDCALLBACK_SIZE
                         : k_CALLBACK_SIZE
};

enum {
    k_CHANNEL_SIZE       = sizeof(btlsos::TcpCbChannel) <
                                              sizeof(btlsos::TcpTimedCbChannel)
                         ? sizeof(btlsos::TcpTimedCbChannel)
                         : sizeof(btlsos::TcpCbChannel)
};

enum {
    e_CLOSED             = -3,
    e_UNINITIALIZED      = -2,
    e_INVALID            = -1,
    e_CANCELLED          = -1,
    e_SUCCESS            = 0,
    e_TIMEDOUT           = 0
};

namespace btlsos {

                     // ================================
                     // class btesos_TcpCbConnector_RReg
                     // ================================

class TcpCbConnector_Reg {

    // PRIVATE DATA MEMBERS
    union {
        char                                d_callbackArena[k_ARENA_SIZE];
        bsls::AlignmentUtil::MaxAlignedType d_align;  // for alignment
    }                     d_cb;

    int                   d_isTimedChannel;
    int                   d_flags;

private:
    TcpCbConnector_Reg(const TcpCbConnector_Reg&);
    TcpCbConnector_Reg&
        operator=(const TcpCbConnector_Reg&);
public:
    // CREATORS
    TcpCbConnector_Reg(
                    const bsl::function<void(btlsc::CbChannel*, int)>& functor,
                    int                                                flags);
    TcpCbConnector_Reg(
               const bsl::function<void(btlsc::TimedCbChannel*, int)>& functor,
               int                                                     flags);

    ~TcpCbConnector_Reg();

    // MANIPULATORS
    void invoke(int status);
    void invoke(btlsc::CbChannel *channel, int status);
    void invokeTimed(btlsc::TimedCbChannel *channel, int status);
        // The behavior is undefined unless this registration holds a non-timed
        // callback or unless the specified 'channel' is actually a
        // 'btlsc::TimedCbChannel'.

    int flags() const;
    int isTimedResult() const;
    const bsl::function<void(btlsc::CbChannel*, int)>& callback() const;
    const bsl::function<void(btlsc::TimedCbChannel*, int)>&
                                                         timedCallback() const;
};

// CREATORS
TcpCbConnector_Reg::TcpCbConnector_Reg(
               const bsl::function<void(btlsc::TimedCbChannel*, int)>& functor,
               int                                                     flags)
: d_isTimedChannel(1)
, d_flags(flags)
{
    new (d_cb.d_callbackArena)
                bsl::function<void(btlsc::TimedCbChannel*, int)>(functor);
}

TcpCbConnector_Reg::TcpCbConnector_Reg(
                    const bsl::function<void(btlsc::CbChannel*, int)>& functor,
                    int                                                flags)
: d_isTimedChannel(0)
, d_flags(flags)
{
    new (d_cb.d_callbackArena)
                     bsl::function<void(btlsc::CbChannel*, int)>(functor);
}

TcpCbConnector_Reg::~TcpCbConnector_Reg()
{
    if (d_isTimedChannel) {
        bsl::function<void(btlsc::TimedCbChannel*, int)> *cb =
            (bsl::function<void(btlsc::TimedCbChannel*, int)> *)
                (void *) d_cb.d_callbackArena;

        bslalg::ScalarDestructionPrimitives::destroy(cb);
    }
    else {
        bsl::function<void(btlsc::CbChannel*, int)> *cb =
             (bsl::function<void(btlsc::CbChannel*, int)> *)
                 (void *) d_cb.d_callbackArena;

        bslalg::ScalarDestructionPrimitives::destroy(cb);
    }
}

// MANIPULATORS
inline
void TcpCbConnector_Reg::invoke(int status)
{
    if (d_isTimedChannel) {
        invokeTimed(NULL, status);
    }
    else {
        invoke(NULL, status);
    }
}

inline
void TcpCbConnector_Reg::invoke(btlsc::CbChannel *channel, int status)
{
    BSLS_ASSERT(0 == d_isTimedChannel);
    bsl::function<void(btlsc::CbChannel*, int)> *cb =
             (bsl::function<void(btlsc::CbChannel*, int)> *)
                (void *) d_cb.d_callbackArena;
    (*cb)(channel, status);
}

inline
void TcpCbConnector_Reg::invokeTimed(btlsc::TimedCbChannel *channel,
                                     int                    status)
{
    bsl::function<void(btlsc::TimedCbChannel*, int)> *cb =
        (bsl::function<void(btlsc::TimedCbChannel*, int)> *)
            (void *) d_cb.d_callbackArena;
    (*cb)(channel, status);
}

inline int TcpCbConnector_Reg::isTimedResult() const
{
    return d_isTimedChannel;
}

// ACCESSORS
inline int TcpCbConnector_Reg::flags() const
{
    return d_flags;
}

inline
const bsl::function<void(btlsc::CbChannel*, int)>&
TcpCbConnector_Reg::callback() const
{
    BSLS_ASSERT(0 == d_isTimedChannel);
    return *(bsl::function<void(btlsc::CbChannel*, int)> *)
                (void *) const_cast<char *>(d_cb.d_callbackArena);
}

inline
const bsl::function<void(btlsc::TimedCbChannel*, int)>&
TcpCbConnector_Reg::timedCallback() const
{
    BSLS_ASSERT(1 == d_isTimedChannel);
    return *(bsl::function<void(btlsc::TimedCbChannel*, int)>*)
                            (void *) const_cast<char *>(d_cb.d_callbackArena);
}

// ============================================================================
//                           END LOCAL DEFINITIONS
// ============================================================================

                           // --------------------
                           // class TcpCbConnector
                           // --------------------

// PRIVATE MANIPULATORS

template <class CALLBACK_TYPE, class CHANNEL>
int TcpCbConnector::initiateConnection(const CALLBACK_TYPE& callback,
                                       int                  flags,
                                       int                  createRequest)
{
    d_connectingSocket_p = d_factory_p->allocate();
    if (NULL == d_connectingSocket_p) {
        callback(NULL, e_UNINITIALIZED);
        return e_UNINITIALIZED;                                       // RETURN
    }

    if (0 != d_connectingSocket_p->
                           setBlockingMode(btlso::Flag::e_NONBLOCKING_MODE))
    {
        d_factory_p->deallocate(d_connectingSocket_p);
        d_connectingSocket_p = NULL;
        callback(NULL, -2);
        return 1;                                                     // RETURN
    }
    int s = d_connectingSocket_p->connect(d_peerAddress);

    if ( s == btlso::SocketHandle::e_ERROR_WOULDBLOCK ||
        (s == btlso::SocketHandle::e_ERROR_INTERRUPTED &&
         0 == (flags & btlsc::Flag::k_ASYNC_INTERRUPT)))
    {
        if (createRequest) {
            TcpCbConnector_Reg *cb =
                new (d_callbackPool)
                         TcpCbConnector_Reg(callback, flags);
            d_callbacks.push_front(cb);
        }
        if (0 != d_manager_p->registerSocketEvent(
                                         d_connectingSocket_p->handle(),
                                         btlso::EventType::e_CONNECT,
                                         d_connectFunctor)){
            callback(0, e_CANCELLED);
            TcpCbConnector_Reg *cb = d_callbacks.back();
            d_callbackPool.deleteObjectRaw(cb);
            d_callbacks.pop_back();
        }
        return 1;                                                     // RETURN
    }

    if (0 == s) {
        // Connection established
        CHANNEL *channel = new (d_channelPool) CHANNEL(d_connectingSocket_p,
                                                       d_manager_p,
                                                       d_allocator_p);
        bsl::vector<btlsc::CbChannel*>::iterator idx =
            bsl::lower_bound(d_channels.begin(), d_channels.end(),
                             static_cast<btlsc::CbChannel*>(channel));
        d_channels.insert(idx, channel);
        d_connectingSocket_p = NULL;
        callback(channel, 0);
        return 0;                                                     // RETURN
    }
    if (s == btlso::SocketHandle::e_ERROR_INTERRUPTED) {
        BSLS_ASSERT(btlsc::Flag::k_ASYNC_INTERRUPT & flags);
        callback(NULL, 1);
        return 0;                                                     // RETURN
    }
    // Hard error occurred
    BSLS_ASSERT(s < 0);
    callback(NULL, s - 2);
    d_factory_p->deallocate(d_connectingSocket_p);
    d_connectingSocket_p = NULL;
    return 0;
}

void TcpCbConnector::connectCb()
{
    BSLS_ASSERT(d_callbacks.size());
    BSLS_ASSERT(d_connectingSocket_p);

    d_manager_p->deregisterSocketEvent(d_connectingSocket_p->handle(),
                                       btlso::EventType::e_CONNECT);

    int s = d_connectingSocket_p->connectionStatus();
    d_currentRequest_p = d_callbacks.back();
    BSLS_ASSERT(d_currentRequest_p);
    if (s) {
        BSLS_ASSERT(0 > s);
        d_factory_p->deallocate(d_connectingSocket_p);
        d_connectingSocket_p = NULL;
        d_currentRequest_p->invoke(s - 1);  // -1 is reserved to indicate
                                            // dequeueing.
    }
    else {
        if (d_currentRequest_p->isTimedResult()) {
            TcpTimedCbChannel *channel =
                new (d_channelPool)
                            TcpTimedCbChannel(d_connectingSocket_p,
                                                     d_manager_p,
                                                     d_allocator_p);
            bsl::vector<btlsc::CbChannel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(),
                                static_cast<btlsc::CbChannel*>(channel));
            d_channels.insert(idx, channel);
            d_connectingSocket_p = NULL;
            d_currentRequest_p->invokeTimed(channel, 0);
        }
        else {
            TcpCbChannel *channel =
                new (d_channelPool) TcpCbChannel(d_connectingSocket_p,
                                                d_manager_p,
                                                d_allocator_p);
            bsl::vector<btlsc::CbChannel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(),
                                static_cast<btlsc::CbChannel*>(channel));
            d_channels.insert(idx, channel);
            d_connectingSocket_p = NULL;
            d_currentRequest_p->invoke(channel, 0);
        }
    }

    int status = 0;
    while (d_callbacks.size() > 1 && 0 == status) {
        d_callbackPool.deleteObjectRaw(d_currentRequest_p); // Deallocate
                                                            // previous
                                                            // request.
        d_callbacks.pop_back();
        d_currentRequest_p = d_callbacks.back();

        if (d_currentRequest_p->isTimedResult()) {
            status = initiateConnection<TimedCallback,
                             TcpTimedCbChannel> (
                                  d_currentRequest_p->timedCallback(),
                                  d_currentRequest_p->flags(), 0);
        }
        else {
            status = initiateConnection<Callback,
                             TcpCbChannel> (
                                 d_currentRequest_p->callback(),
                                 d_currentRequest_p->flags(), 0);
        }
    }
    if (0 == status) {
        d_callbackPool.deleteObjectRaw(d_currentRequest_p);
        d_callbacks.pop_back();
        d_connectingSocket_p = NULL;
    }
    d_currentRequest_p = NULL;
}

void TcpCbConnector::deallocateCb(btlsc::CbChannel *channel) {
    BSLS_ASSERT(channel);
    btlso::StreamSocket<btlso::IPv4Address> *s = NULL;
    TcpTimedCbChannel *c =
        dynamic_cast<TcpTimedCbChannel*>(channel);
    if (c) {
        s = c->socket();
    }
    else {
        TcpCbChannel *c =
            dynamic_cast<TcpCbChannel*>(channel);
        BSLS_ASSERT(c);
        s = c->socket();
    }
    BSLS_ASSERT(s);
    channel->invalidate();
    channel->cancelAll();
    d_factory_p->deallocate(s);

    bsl::vector<btlsc::CbChannel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(), channel);
    BSLS_ASSERT(idx != d_channels.end() && *idx == channel);

    d_channels.erase(idx);
    d_channelPool.deleteObjectRaw(channel);
}

// CREATORS

TcpCbConnector::TcpCbConnector(
                btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
                btlso::TimerEventManager                       *manager,
                bslma::Allocator                               *basicAllocator)
: d_callbackPool(sizeof(TcpCbConnector_Reg), basicAllocator)
, d_channelPool(k_CHANNEL_SIZE, basicAllocator)
, d_channels(basicAllocator)
, d_currentRequest_p(NULL)
, d_manager_p(manager)
, d_factory_p(factory)
, d_connectingSocket_p(NULL)
, d_isInvalidFlag(0)
, d_allocator_p(basicAllocator)
{
    d_connectFunctor
        = bsl::function<void()>(
                bsl::allocator_arg_t(),
                bsl::allocator<bsl::function<void()> >(d_allocator_p),
                bdlf::MemFnUtil::memFn(&TcpCbConnector::connectCb, this));
}

TcpCbConnector::TcpCbConnector(
                btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
                btlso::TimerEventManager                       *manager,
                int                                             numChannels,
                bslma::Allocator                               *basicAllocator)
: d_callbackPool(sizeof(TcpCbConnector_Reg), basicAllocator)
, d_channelPool(k_CHANNEL_SIZE, basicAllocator)
, d_channels(basicAllocator)
, d_currentRequest_p(NULL)
, d_manager_p(manager)
, d_factory_p(factory)
, d_connectingSocket_p(NULL)
, d_isInvalidFlag(0)
, d_allocator_p(basicAllocator)
{
    d_connectFunctor
        = bsl::function<void()>(
                bsl::allocator_arg_t(),
                bsl::allocator<bsl::function<void()> >(d_allocator_p),
                bdlf::MemFnUtil::memFn(&TcpCbConnector::connectCb, this));

    d_channelPool.reserveCapacity(numChannels);
}

TcpCbConnector::~TcpCbConnector()
{
    invalidate();
    cancelAll();

    // Deallocate channels
    while (d_channels.size()) {
        btlsc::CbChannel *ch = d_channels[d_channels.size() - 1];
        BSLS_ASSERT(ch);
        ch->invalidate();
        ch->cancelAll();
        deallocateCb(ch);
    }
}

// MANIPULATORS

int TcpCbConnector::allocate(const Callback& callback, int flags)
{
    if (d_isInvalidFlag) {
        return e_INVALID;                                             // RETURN
    }

    if (d_callbacks.size() == 0) {
        initiateConnection<Callback, TcpTimedCbChannel>
                (callback, flags, 1);
        return 0;                                                     // RETURN

    }
    TcpCbConnector_Reg *cb =
        new (d_callbackPool) TcpCbConnector_Reg(callback, flags);
    d_callbacks.push_front(cb);
    return 0;
}

int TcpCbConnector::allocateTimed(const TimedCallback& timedCallback,
                                  int                  flags)
{
    if (d_isInvalidFlag) {
        return e_INVALID;                                             // RETURN
    }

    // Implementation note: the request must be pushed onto the queue before
    // the corresponding callback is registered with an event manager.

    if (d_callbacks.size() == 0) {
        initiateConnection<TimedCallback, TcpTimedCbChannel>
            (timedCallback, flags, 1);
        return 0;                                                     // RETURN
    }

    TcpCbConnector_Reg *cb =
        new (d_callbackPool) TcpCbConnector_Reg(timedCallback, flags);
    d_callbacks.push_front(cb);
    return 0;
}

void TcpCbConnector::cancelAll()
{
    if (d_currentRequest_p) {
        // A callback is active -- can't destroy current request.
        bsl::deque<TcpCbConnector_Reg *> toBeCancelled(
                    d_callbacks.begin(),
                    d_callbacks.begin() + d_callbacks.size() - 1,
                    d_allocator_p);
        d_callbacks.erase(d_callbacks.begin(),
                          d_callbacks.begin() + d_callbacks.size() - 1);
        BSLS_ASSERT(d_currentRequest_p == d_callbacks.back());
        int numToCancel = static_cast<int>(toBeCancelled.size());
        while (--numToCancel >= 0) {
            TcpCbConnector_Reg *reg = toBeCancelled[numToCancel];
            reg->invoke(-1);
            d_callbackPool.deleteObjectRaw(reg);
        }
    }
    else {
        // This part is reached when 'cancelAll' is invoked not from a
        // callback.

        bsl::deque<TcpCbConnector_Reg *>
                                     toBeCancelled(d_callbacks, d_allocator_p);
        d_callbacks.clear();
        int numToCancel = static_cast<int>(toBeCancelled.size());
        if (numToCancel) {
            d_manager_p->deregisterSocketEvent(
                                              d_connectingSocket_p->handle(),
                                              btlso::EventType::e_CONNECT);

            d_factory_p->deallocate(d_connectingSocket_p);
        }

        while (--numToCancel >= 0) {
            TcpCbConnector_Reg *reg = toBeCancelled[numToCancel];
            reg->invoke(-1);
            d_callbackPool.deleteObjectRaw(reg);
        }
    }
}

void TcpCbConnector::deallocate(btlsc::CbChannel *channel)
{
    BSLS_ASSERT(channel);
    channel->invalidate();
    channel->cancelAll();
}

// ACCESSORS
void TcpCbConnector::invalidate() {
    d_isInvalidFlag = 1;
}

int TcpCbConnector::isInvalid() const {
    return d_isInvalidFlag;
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
