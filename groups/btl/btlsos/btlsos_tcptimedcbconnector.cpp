// btlsos_tcptimedcbconnector.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlsos_tcptimedcbconnector.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlsos_tcptimedcbconnector_cpp,"$Id$ $CSID$")

#include <btlsos_tcptimedcbchannel.h>
#include <btlsos_tcpcbchannel.h>
#include <btlso_timereventmanager.h>
#include <btlso_streamsocketfactory.h>
#include <btlso_streamsocket.h>
#include <btlso_eventtype.h>
#include <btlsc_flag.h>

#include <bdlt_currenttime.h>
#include <bsls_timeinterval.h>

#include <bdlf_memfn.h>
#include <bdlf_bind.h>

#include <bslalg_scalardestructionprimitives.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

#ifdef TEST
// These dependencies will cause the test driver to recompile when the concrete
// implementation of the event manager changes.
#include <btlso_tcptimereventmanager.h>
#endif

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>
#include <bsl_iterator.h>
#include <bsl_memory.h>
#include <bsl_vector.h>

// ============================================================================
//                          IMPLEMENTATION DETAILS
// ----------------------------------------------------------------------------
// 1.  Internally, this connector holds a queue of callbacks for allocation
// requests.  The queue contains both timed and non-timed callbacks along with
// any supporting data for a request, such as the timeout value, if any, and
// flags.
//
// 2.  At most two callbacks are registered with the socket event manager:
// timer callbacks and accept callback.  Timer callback is registered only if
// accept callback is registered.  The timer registration ID is cached and a
// NULL-value for the timer ID is used to figure out if timer callback is
// registered.
//
// 3.  Timer callback is registered if and only if the corresponding request is
// for a timed operation (i.e., as a result of 'timedAllocate' or
// 'timedAllocateTimed' operations.
//
// 4.  The request queue keeps the type of the result (i.e., timed or non-timed
// channel) and the request can invoke the callback in a type-safe manner.
//
// 5.  Whenever an accept callback is registered with the socket event manager,
// the 'd_isRegisteredFlag' is set.
//
// 6.  All allocate methods register callbacks with a socket event manager if
// the request queue size is 1 (i.e., when only the current request is cached).
// An allocate method can set the value of 'd_isRegisteredFlag' to 1, but not
// to 0 (i.e., it cannot deregister the accept callback).
//
// 7.
// ============================================================================

namespace BloombergLP {

// ============================================================================
//                             LOCAL DEFINITIONS
// ============================================================================

                     // ===============================
                     // Local typedefs and enumerations
                     // ===============================

enum {
    k_CALLBACK_SIZE      = sizeof(btlsc::TimedCbChannelAllocator::Callback),
    k_TIMEDCALLBACK_SIZE =
                        sizeof (btlsc::TimedCbChannelAllocator::TimedCallback),
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
    e_FAILED_TO_REG      = -4,
    e_CLOSED             = -3,
    e_UNINITIALIZED      = -2,
    e_INVALID            = -1,
    e_CANCELLED          = -1,
    e_SUCCESS            = 0,
    e_TIMEDOUT           = 0,
    e_ENQUEUED           = 1
};

namespace btlsos {

                  // =====================================
                  // class btesos_TcpTimedCbConnector_RReg
                  // =====================================

class TcpTimedCbConnector_Reg {

    // PRIVATE DATA MEMBERS
    union {
        char                                d_callbackArena[k_ARENA_SIZE];
        bsls::AlignmentUtil::MaxAlignedType d_align;  // for alignment
    }                 d_cb;

    int               d_isTimedChannel;
    int               d_isTimedOperation;
    bsls::TimeInterval d_timeout;
    int               d_flags;

  private:
    TcpTimedCbConnector_Reg(const TcpTimedCbConnector_Reg&);
    TcpTimedCbConnector_Reg&
        operator=(const TcpTimedCbConnector_Reg&);
  public:
    // CREATORS
    TcpTimedCbConnector_Reg(
               const bsls::TimeInterval&                               timeout,
               const bsl::function<void(btlsc::TimedCbChannel*, int)>& functor,
               int                                                     flags);
    TcpTimedCbConnector_Reg(
                    const bsl::function<void(btlsc::CbChannel*, int)>& functor,
                    int                                                flags);
    TcpTimedCbConnector_Reg(
               const bsl::function<void(btlsc::TimedCbChannel*, int)>& functor,
               int                                                     flags);
    TcpTimedCbConnector_Reg(
                    const bsls::TimeInterval&                          timeout,
                    const bsl::function<void(btlsc::CbChannel*, int)>& functor,
                    int                                                flags);

    ~TcpTimedCbConnector_Reg();

    // MANIPULATORS
    void invoke(int status);
    void invoke(btlsc::CbChannel *channel, int status);
    void invokeTimed(btlsc::TimedCbChannel *channel, int status);
        // The behavior is undefined unless this registration holds a non-timed
        // callback or unless the specified 'channel' is actually a
        // 'btlsc::TimedCbChannel'.

    // ACCESSORS
    int flags() const;
    int isTimedResult() const;
    int isTimedOperation() const;
    const bsls::TimeInterval& timeout() const { return d_timeout; }
    const bsl::function<void(btlsc::CbChannel*, int)>& callback() const;
    const bsl::function<void(btlsc::TimedCbChannel*, int)>&
                                                         timedCallback() const;
};

// CREATORS
TcpTimedCbConnector_Reg::TcpTimedCbConnector_Reg(
               const bsls::TimeInterval&                               timeout,
               const bsl::function<void(btlsc::TimedCbChannel*, int)>& functor,
               int                                                     flags)
: d_isTimedChannel(1)
, d_isTimedOperation(1)
, d_timeout(timeout)
, d_flags(flags)
{
    new (d_cb.d_callbackArena)
                bsl::function<void(btlsc::TimedCbChannel*, int)>(functor);
}

TcpTimedCbConnector_Reg::TcpTimedCbConnector_Reg(
               const bsl::function<void(btlsc::TimedCbChannel*, int)>& functor,
               int                                                     flags)
: d_isTimedChannel(1)
, d_isTimedOperation(0)
, d_flags(flags)
{
    new (d_cb.d_callbackArena)
                bsl::function<void(btlsc::TimedCbChannel*, int)>(functor);
}

TcpTimedCbConnector_Reg::TcpTimedCbConnector_Reg(
                    const bsl::function<void(btlsc::CbChannel*, int)>& functor,
                    int                                                flags)
: d_isTimedChannel(0)
, d_isTimedOperation(0)
, d_flags(flags)
{
    new (d_cb.d_callbackArena)
            bsl::function<void(btlsc::CbChannel*, int)>(functor);
}

TcpTimedCbConnector_Reg::TcpTimedCbConnector_Reg(
                    const bsls::TimeInterval&                          timeout,
                    const bsl::function<void(btlsc::CbChannel*, int)>& functor,
                    int                                                flags)
: d_isTimedChannel(0)
, d_isTimedOperation(1)
, d_timeout(timeout)
, d_flags(flags)
{
    new (d_cb.d_callbackArena)
            bsl::function<void(btlsc::CbChannel*, int)>(functor);
}

TcpTimedCbConnector_Reg::~TcpTimedCbConnector_Reg()
{
    if (d_isTimedChannel) {
        bsl::function<void(btlsc::TimedCbChannel*, int)> *cb =
            (bsl::function<void(btlsc::TimedCbChannel*, int)> *)
                                                  (void *)d_cb.d_callbackArena;

        bslalg::ScalarDestructionPrimitives::destroy(cb);
    }
    else {
        bsl::function<void(btlsc::TimedCbChannel*, int)> *cb =
             (bsl::function<void(btlsc::TimedCbChannel*, int)>*)
                (void *)d_cb.d_callbackArena;

        bslalg::ScalarDestructionPrimitives::destroy(cb);
    }
}

inline
void TcpTimedCbConnector_Reg::invoke(int status) {
    if (d_isTimedChannel) {
        invokeTimed(NULL, status);
    }
    else {
        invoke(NULL, status);
    }
}

inline
void TcpTimedCbConnector_Reg::invoke(btlsc::CbChannel *channel, int status) {
    BSLS_ASSERT(0 == d_isTimedChannel);
    bsl::function<void(btlsc::CbChannel*, int)> *cb =
             (bsl::function<void(btlsc::CbChannel*, int)>*)
                (void *)d_cb.d_callbackArena;
    (*cb)(channel, status);
}

inline
void TcpTimedCbConnector_Reg::invokeTimed(btlsc::TimedCbChannel *channel,
                                          int                    status)
{
    bsl::function<void(btlsc::TimedCbChannel*, int)> *cb =
        (bsl::function<void(btlsc::TimedCbChannel*, int)>*)
            (void *)d_cb.d_callbackArena;
    (*cb)(channel, status);
}

inline int TcpTimedCbConnector_Reg::isTimedResult() const {
    return d_isTimedChannel;
}

inline int TcpTimedCbConnector_Reg::isTimedOperation() const {
    return d_isTimedOperation;
}

// ACCESSORS
inline int TcpTimedCbConnector_Reg::flags() const {
    return d_flags;
}

inline
const bsl::function<void(btlsc::CbChannel*, int)>&
TcpTimedCbConnector_Reg::callback() const {
    BSLS_ASSERT(0 == d_isTimedChannel);
    return *(bsl::function<void(btlsc::CbChannel*, int)>*)
                (void *) const_cast<char *>(d_cb.d_callbackArena);
}

inline
const bsl::function<void(btlsc::TimedCbChannel*, int)>&
TcpTimedCbConnector_Reg::timedCallback() const {
    BSLS_ASSERT(1 == d_isTimedChannel);
    return *(bsl::function<void(btlsc::TimedCbChannel*, int)>*)
                            (void *) const_cast<char *>(d_cb.d_callbackArena);
}

// ============================================================================
//                           END LOCAL DEFINITIONS
// ============================================================================

                        // -------------------------
                        // class TcpTimedCbConnector
                        // -------------------------

// PRIVATE MANIPULATORS

template <class CALLBACK_TYPE, class CHANNEL>
int TcpTimedCbConnector::initiateTimedConnection(
                                      const CALLBACK_TYPE&      callback,
                                      const bsls::TimeInterval& timeout,
                                      int                       flags,
                                      int                       enqueueRequest)
    // Initiate a non-blocking connection to the peer server, invoke the
    // specified 'callback' if the operation completed immediately, either
    // successfully or not, (and allocate a channel, if needed), or enqueue the
    // request for a connection completion after the specified 'timeout'
    // absolute time.  Return 0 if operation completed successfully, a negative
    // value if an error occurred and a positive value if an operation could
    // not complete immediately (and thus was enqueued for further processing).
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
        callback(NULL, e_UNINITIALIZED);
        return e_UNINITIALIZED;                                       // RETURN
    }

    int s = d_connectingSocket_p->connect(d_peerAddress);

    if ( s == btlso::SocketHandle::e_ERROR_WOULDBLOCK ||
        (s == btlso::SocketHandle::e_ERROR_INTERRUPTED &&
         0 == (flags & btlsc::Flag::k_ASYNC_INTERRUPT)))
    {
        if (enqueueRequest) {
            TcpTimedCbConnector_Reg *cb =
                new (d_callbackPool) TcpTimedCbConnector_Reg(timeout,
                                                           callback, flags);
            d_callbacks.push_front(cb);
        }

        if (0 != d_manager_p->registerSocketEvent(
                                         d_connectingSocket_p->handle(),
                                         btlso::EventType::e_CONNECT,
                                         d_connectFunctor))
        {
            d_factory_p->deallocate(d_connectingSocket_p);
            d_connectingSocket_p = NULL;
            if (enqueueRequest) {
                // If a request was just created, it must be destroyed
                TcpTimedCbConnector_Reg *cb = d_callbacks.front();
                BSLS_ASSERT(cb);
                d_callbackPool.deleteObjectRaw(cb);
                d_callbacks.pop_front();
            }

            callback(0, e_FAILED_TO_REG);
            return e_FAILED_TO_REG;                                   // RETURN
        }
        d_timerId =
            d_manager_p->registerTimer(timeout, d_timeoutFunctor);

        return e_ENQUEUED;                                            // RETURN
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
        return e_SUCCESS;                                             // RETURN
    }
    if (s == btlso::SocketHandle::e_ERROR_INTERRUPTED) {
        BSLS_ASSERT(btlsc::Flag::k_ASYNC_INTERRUPT & flags);
        callback(NULL, 1);
        return e_SUCCESS;                                             // RETURN
    }
    // Hard error occurred
    BSLS_ASSERT(s < 0);
    d_factory_p->deallocate(d_connectingSocket_p);
    d_connectingSocket_p = NULL;
    callback(NULL, s - 2);
    return e_SUCCESS;
}

template <class CALLBACK_TYPE, class CHANNEL>
int TcpTimedCbConnector::initiateConnection(const CALLBACK_TYPE& callback,
                                            int                  flags,
                                            int                  createRequest)
    // Initiate a non-blocking connection to the peer server, invoke the
    // specified 'callback' if operation completed immediately, either
    // successfully or not, (and allocate a channel, if needed), or enqueue the
    // request for a connection completion.  Return 0 if operation completed
    // successfully, a negative value if an error occurred and a positive value
    // if an operation could not complete immediately (and thus was enqueued
    // for further processing).
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
        callback(NULL, e_UNINITIALIZED);
        return e_UNINITIALIZED;                                       // RETURN
    }
    int s = d_connectingSocket_p->connect(d_peerAddress);

    if ( s == btlso::SocketHandle::e_ERROR_WOULDBLOCK ||
        (s == btlso::SocketHandle::e_ERROR_INTERRUPTED &&
         0 == (flags & btlsc::Flag::k_ASYNC_INTERRUPT)))
    {
        if (createRequest) {
            TcpTimedCbConnector_Reg *cb =
                new (d_callbackPool)
                         TcpTimedCbConnector_Reg(callback, flags);
            d_callbacks.push_front(cb);
        }
        if (0 != d_manager_p->registerSocketEvent(
                                         d_connectingSocket_p->handle(),
                                         btlso::EventType::e_CONNECT,
                                         d_connectFunctor)){
            d_factory_p->deallocate(d_connectingSocket_p);
            d_connectingSocket_p = NULL;
            if (createRequest) {
                // If a request was just created, it must be destroyed
                TcpTimedCbConnector_Reg *cb = d_callbacks.front();
                BSLS_ASSERT(cb);
                d_callbackPool.deleteObjectRaw(cb);
                d_callbacks.pop_front();
            }
            callback(0, e_FAILED_TO_REG);
            return e_FAILED_TO_REG;                                   // RETURN
        }

        d_timerId = NULL;
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
        return e_SUCCESS;                                             // RETURN
    }
    if (s == btlso::SocketHandle::e_ERROR_INTERRUPTED) {
        BSLS_ASSERT(btlsc::Flag::k_ASYNC_INTERRUPT & flags);
        callback(NULL, 1);
        return e_SUCCESS;                                             // RETURN
    }
    // Hard error occurred
    BSLS_ASSERT(s < 0);
    callback(NULL, s - 2);
    d_factory_p->deallocate(d_connectingSocket_p);
    d_connectingSocket_p = NULL;
    return e_SUCCESS;
}

void TcpTimedCbConnector::timerCb() {
    BSLS_ASSERT(d_callbacks.size());  // at least one must be registered
    BSLS_ASSERT(d_connectingSocket_p);
    d_currentRequest_p = d_callbacks.back();
    BSLS_ASSERT(d_currentRequest_p);

    d_manager_p->deregisterSocketEvent(d_connectingSocket_p->handle(),
                                       btlso::EventType::e_CONNECT);
    d_factory_p->deallocate(d_connectingSocket_p);
    d_connectingSocket_p = NULL;

    d_currentRequest_p->invoke(e_TIMEDOUT);

    int status = 0;
    while (d_callbacks.size() > 1 && 0 == status) {
        // Deallocate previous request.
        d_callbackPool.deleteObjectRaw(d_currentRequest_p);
        d_callbacks.pop_back();
        d_currentRequest_p = d_callbacks.back();

        if (d_currentRequest_p->isTimedOperation()) {
            if (d_currentRequest_p->isTimedResult()) {

                status = initiateTimedConnection<TimedCallback,
                             TcpTimedCbChannel> (
                                 d_currentRequest_p->timedCallback(),
                                 d_currentRequest_p->timeout(),
                                 d_currentRequest_p->flags(), 0);

            }
            else {
                status = initiateTimedConnection<Callback,
                             TcpCbChannel> (
                                 d_currentRequest_p->callback(),
                                 d_currentRequest_p->timeout(),
                                 d_currentRequest_p->flags(), 0);
            }
        } else {
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
    }
    if (e_ENQUEUED > status) {
        d_callbackPool.deleteObjectRaw(d_currentRequest_p);
        d_callbacks.pop_back();
    }
    d_currentRequest_p = NULL;
}

void TcpTimedCbConnector::connectCb() {
    BSLS_ASSERT(d_callbacks.size());
    BSLS_ASSERT(d_connectingSocket_p);

    // Deregister associated timer, if any.
    if (d_timerId) {
        d_manager_p->deregisterTimer(d_timerId);
        d_timerId = NULL;
    }
    BSLS_ASSERT(NULL == d_timerId);   // internal (in-method) invariant
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
        if (d_currentRequest_p->isTimedOperation()) {
            if (d_currentRequest_p->isTimedResult()) {

                status = initiateTimedConnection<TimedCallback,
                             TcpTimedCbChannel> (
                                 d_currentRequest_p->timedCallback(),
                                 d_currentRequest_p->timeout(),
                                 d_currentRequest_p->flags(), 0);

            }
            else {
                status = initiateTimedConnection<Callback,
                             TcpCbChannel> (
                                 d_currentRequest_p->callback(),
                                 d_currentRequest_p->timeout(),
                                 d_currentRequest_p->flags(), 0);
            }
        } else {
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
    }
    if (e_ENQUEUED > status) {
        d_callbackPool.deleteObjectRaw(d_currentRequest_p);
        d_callbacks.pop_back();
        d_connectingSocket_p = NULL;
    }
    d_currentRequest_p = NULL;
}

void TcpTimedCbConnector::deallocateCb(btlsc::CbChannel *channel) {
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
    return;
}

// CREATORS

TcpTimedCbConnector::TcpTimedCbConnector(
                btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
                btlso::TimerEventManager                       *manager,
                bslma::Allocator                               *basicAllocator)
: d_callbackPool(sizeof(TcpTimedCbConnector_Reg), basicAllocator)
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
            bdlf::MemFnUtil::memFn(&TcpTimedCbConnector::connectCb, this));

    d_timeoutFunctor
        = bsl::function<void()>(
            bsl::allocator_arg_t(),
            bsl::allocator<bsl::function<void()> >(d_allocator_p),
            bdlf::MemFnUtil::memFn(&TcpTimedCbConnector::timerCb, this));
}

TcpTimedCbConnector::TcpTimedCbConnector(
                btlso::StreamSocketFactory<btlso::IPv4Address> *factory,
                btlso::TimerEventManager                       *manager,
                int                                             numChannels,
                bslma::Allocator                               *basicAllocator)
: d_callbackPool(sizeof(TcpTimedCbConnector_Reg), basicAllocator)
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
            bdlf::MemFnUtil::memFn(&TcpTimedCbConnector::connectCb, this));

    d_timeoutFunctor
        = bsl::function<void()>(
            bsl::allocator_arg_t(),
            bsl::allocator<bsl::function<void()> >(d_allocator_p),
            bdlf::MemFnUtil::memFn(&TcpTimedCbConnector::timerCb, this));

    d_channelPool.reserveCapacity(numChannels);
}

TcpTimedCbConnector::~TcpTimedCbConnector()
{
    invalidate();
    cancelAll();

    // Deallocate channels
    while (d_channels.size()) {
        btlsc::CbChannel *ch = d_channels[0];
        BSLS_ASSERT(ch);
        ch->invalidate();
        ch->cancelAll();
        deallocateCb(ch);
    }
}

// MANIPULATORS

int TcpTimedCbConnector::allocate(const Callback& callback, int flags)
{
    if (d_isInvalidFlag) {
        return e_INVALID;                                             // RETURN
    }

    if (d_callbacks.size() == 0) {
        initiateConnection<Callback, TcpTimedCbChannel>
                (callback, flags, 1);
        return 0;                                                     // RETURN

    }
    TcpTimedCbConnector_Reg *cb =
        new (d_callbackPool) TcpTimedCbConnector_Reg(callback, flags);
    d_callbacks.push_front(cb);
    return 0;
}

int TcpTimedCbConnector::allocateTimed(const TimedCallback& timedCallback,
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

    TcpTimedCbConnector_Reg *cb =
        new (d_callbackPool) TcpTimedCbConnector_Reg(timedCallback, flags);
    d_callbacks.push_front(cb);
    return 0;
}

void TcpTimedCbConnector::cancelAll() {
    if (d_currentRequest_p) {
        // A callback is active -- can't destroy current request.
        bsl::deque<TcpTimedCbConnector_Reg *> toBeCancelled(
                                  d_callbacks.begin(),
                                  d_callbacks.begin() + d_callbacks.size() - 1,
                                  d_allocator_p);
        d_callbacks.erase(d_callbacks.begin(),
                          d_callbacks.begin() + d_callbacks.size() - 1);
        BSLS_ASSERT(d_currentRequest_p == d_callbacks.back());
        int numToCancel = static_cast<int>(toBeCancelled.size());
        while (--numToCancel >= 0) {
            TcpTimedCbConnector_Reg *reg = toBeCancelled[numToCancel];
            reg->invoke(-1);
            d_callbackPool.deleteObjectRaw(reg);
        }
    }
    else {
        // This part is reached when 'cancelAll' is invoked not from a
        // callback.

        bsl::deque<TcpTimedCbConnector_Reg *> toBeCancelled(
                                                   d_callbacks, d_allocator_p);
        d_callbacks.clear();
        int numToCancel = static_cast<int>(toBeCancelled.size());
        if (numToCancel) {
            BSLS_ASSERT(d_connectingSocket_p);
            d_manager_p->deregisterSocketEvent(
                                              d_connectingSocket_p->handle(),
                                              btlso::EventType::e_CONNECT);

            if (d_timerId) {
                d_manager_p->deregisterTimer(d_timerId);
                d_timerId = NULL;
            }
            d_factory_p->deallocate(d_connectingSocket_p);
        }

        while (--numToCancel >= 0) {
            TcpTimedCbConnector_Reg *reg = toBeCancelled[numToCancel];
            reg->invoke(-1);
            d_callbackPool.deleteObjectRaw(reg);
        }
    }
}

void TcpTimedCbConnector::deallocate(btlsc::CbChannel *channel)
{
    BSLS_ASSERT(channel);
    channel->invalidate();
    channel->cancelAll();
    bsl::function<void()> cb(bdlf::BindUtil::bind(
                                            &TcpTimedCbConnector::deallocateCb,
                                             this,
                                             channel));
    d_manager_p->registerTimer(bdlt::CurrentTime::now(), cb);
}

int TcpTimedCbConnector::timedAllocate(const Callback&           callback,
                                       const bsls::TimeInterval& timeout,
                                       int                       flags)
{
    if (d_isInvalidFlag) {
        return e_INVALID;                                             // RETURN
    }

    if (d_callbacks.size() == 0) {
        initiateTimedConnection<Callback, TcpTimedCbChannel>
                (callback, timeout, flags, 1);
        return 0;                                                     // RETURN
    }

    TcpTimedCbConnector_Reg *cb =
        new (d_callbackPool)
                     TcpTimedCbConnector_Reg(timeout, callback, flags);
    d_callbacks.push_front(cb);
    return 0;
}

int TcpTimedCbConnector::timedAllocateTimed(
                                       const TimedCallback&      timedCallback,
                                       const bsls::TimeInterval& timeout,
                                       int                       flags)
{
    if (d_isInvalidFlag) {
        return e_INVALID;                                             // RETURN
    }

    if (d_callbacks.size() == 0) {
        BSLS_ASSERT(NULL == d_connectingSocket_p);
        return initiateTimedConnection<TimedCallback, TcpTimedCbChannel>
                                      (timedCallback, timeout, flags, 1) < 0;
                                                                      // RETURN
    }
    TcpTimedCbConnector_Reg *cb =
        new (d_callbackPool)
                   TcpTimedCbConnector_Reg(timeout, timedCallback, flags);
    d_callbacks.push_front(cb);
    return 0;
}

void TcpTimedCbConnector::invalidate() {
    d_isInvalidFlag = 1;
}

// ACCESSORS

int TcpTimedCbConnector::isInvalid() const {
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
