// btesos_tcptimedcbconnector.cpp                                     -*-C++-*-
#include <btesos_tcptimedcbconnector.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btesos_tcptimedcbconnector_cpp,"$Id$ $CSID$")

#include <btesos_tcptimedcbchannel.h>
#include <btesos_tcpcbchannel.h>
#include <bteso_timereventmanager.h>
#include <bteso_streamsocketfactory.h>
#include <bteso_streamsocket.h>
#include <bteso_eventtype.h>
#include <btesc_flag.h>

#include <bdetu_systemtime.h>
#include <bdet_timeinterval.h>

#include <bdef_function.h>
#include <bdef_memfn.h>
#include <bdef_bind.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

#ifdef TEST
// These dependencies will cause the test driver to recompile when the concrete
// implementation of the event manager changes.
#include <bteso_tcptimereventmanager.h>
#endif

#include <bsl_algorithm.h>
#include <bsl_vector.h>

// ===========================================================================
// IMPLEMENTATION DETAILS
// ---------------------------------------------------------------------------
// 1. Internally, this connector holds a queue of callbacks for allocation
// requests.  The queue contains both timed and non-timed callbacks along
// with any supporting data for a request, such as the timeout value, if any,
// and flags.
//
// 2. At most two callbacks are registered with the socket event manager:
// timer callbacks and accept callback.  Timer callback is registered only if
// accept callback is registered.  The timer registration ID is cached and
// a NULL-value for the timer ID  is used to figure out if timer callback is
// registered.
//
// 3. Timer callback is registered if and only if the corresponding request
// is for a timed operation (i.e., as a result of 'timedAllocate' or
// 'timedAllocateTimed' operations.
//
// 4. The request queue keeps the type of the result (i.e., timed or non-timed
// channel) and the request can invoke the callback in a type-safe manner.
//
// 5. Whenever an accept callback is registered with the socket event manager,
// the 'd_isRegisteredFlag' is set.
//
// 6. All allocate methods register callbacks with a socket event manager if
// the request queue size is 1 (i.e., when only the current request is
// cached).  An allocate method can set the value of 'd_isRegisteredFlag' to
// 1, but not to 0 (i.e., it cannot deregister the accept callback).
//
// 7.
// ===========================================================================

namespace BloombergLP {

// ============================================================================
//                        LOCAL DEFINITIONS
// ============================================================================

                       // ========================
                       // Local typedefs and enums
                       // ========================

enum {
    CALLBACK_SIZE      = sizeof(btesc_TimedCbChannelAllocator::Callback),
    TIMEDCALLBACK_SIZE = sizeof (btesc_TimedCbChannelAllocator::TimedCallback),
    ARENA_SIZE         = CALLBACK_SIZE < TIMEDCALLBACK_SIZE
                         ? TIMEDCALLBACK_SIZE
                         : CALLBACK_SIZE
};

enum {
    CHANNEL_SIZE       = sizeof(btesos_TcpCbChannel) <
                                               sizeof(btesos_TcpTimedCbChannel)
                         ? sizeof(btesos_TcpTimedCbChannel)
                         : sizeof(btesos_TcpCbChannel)
};

enum {
    FAILED_TO_REG      = -4,
    CLOSED             = -3,
    UNINITIALIZED      = -2,
    INVALID            = -1,
    CANCELLED          = -1,
    SUCCESS            = 0,
    TIMEDOUT           = 0,
    ENQUEUED           = 1
};

                       // =====================================
                       // class btesos_TcpTimedCbConnector_RReg
                       // =====================================

class btesos_TcpTimedCbConnector_Reg {

    // PRIVATE DATA MEMBERS
    union {
        char                               d_callbackArena[ARENA_SIZE];
        bsls_AlignmentUtil::MaxAlignedType d_align;  // for alignment
    }                 d_cb;

    int               d_isTimedChannel;
    int               d_isTimedOperation;
    bdet_TimeInterval d_timeout;
    int               d_flags;

  private:
    btesos_TcpTimedCbConnector_Reg(const btesos_TcpTimedCbConnector_Reg&);
    btesos_TcpTimedCbConnector_Reg&
        operator=(const btesos_TcpTimedCbConnector_Reg&);
  public:
    // CREATORS
    btesos_TcpTimedCbConnector_Reg(const bdet_TimeInterval& timeout,
            const bdef_Function<void (*)(btesc_TimedCbChannel*, int)>& functor,
            int flags);
    btesos_TcpTimedCbConnector_Reg(
            const bdef_Function<void (*)(btesc_CbChannel*, int)>& functor,
            int flags);
    btesos_TcpTimedCbConnector_Reg(
            const bdef_Function<void (*)(btesc_TimedCbChannel*, int)>& functor,
            int flags);
    btesos_TcpTimedCbConnector_Reg(const bdet_TimeInterval& timeout,
            const bdef_Function<void (*)(btesc_CbChannel*, int)>& functor,
            int flags);

    ~btesos_TcpTimedCbConnector_Reg();

    // MANIPULATORS
    void invoke(int status);
    void invoke(btesc_CbChannel *channel, int status);
    void invokeTimed(btesc_TimedCbChannel *channel, int status);
    // The behavior is undefined unless this registration holds
    // a non-timed callback or unless 'channel' is actually a
    // 'btesc_TimedCbChannel'.

    // ACCESSORS
    int flags() const;
    int isTimedResult() const;
    int isTimedOperation() const;
    const bdet_TimeInterval& timeout() const { return d_timeout; }
    const bdef_Function<void (*)(btesc_CbChannel*, int)>& callback() const;
    const bdef_Function<void (*)(btesc_TimedCbChannel*, int)>&
                                                         timedCallback() const;
};

// CREATORS
btesos_TcpTimedCbConnector_Reg::btesos_TcpTimedCbConnector_Reg(
    const bdet_TimeInterval& timeout,
    const bdef_Function<void (*)(btesc_TimedCbChannel*, int)>& functor,
    int flags)
: d_isTimedChannel(1)
, d_isTimedOperation(1)
, d_timeout(timeout)
, d_flags(flags)
{
    new (d_cb.d_callbackArena)
                 bdef_Function<void (*)(btesc_TimedCbChannel*, int)>(functor);
}

btesos_TcpTimedCbConnector_Reg::btesos_TcpTimedCbConnector_Reg(
    const bdef_Function<void (*)(btesc_TimedCbChannel*, int)>& functor,
    int flags)
: d_isTimedChannel(1)
, d_isTimedOperation(0)
, d_flags(flags)
{
    new (d_cb.d_callbackArena)
                  bdef_Function<void (*)(btesc_TimedCbChannel*, int)>(functor);
}

btesos_TcpTimedCbConnector_Reg::btesos_TcpTimedCbConnector_Reg(
    const bdef_Function<void (*)(btesc_CbChannel*, int)>& functor, int flags)
: d_isTimedChannel(0)
, d_isTimedOperation(0)
, d_flags(flags)
{
    new (d_cb.d_callbackArena)
            bdef_Function<void (*)(btesc_CbChannel*, int)>(functor);
}

btesos_TcpTimedCbConnector_Reg::btesos_TcpTimedCbConnector_Reg(
    const bdet_TimeInterval& timeout,
    const bdef_Function<void (*)(btesc_CbChannel*, int)>& functor,
    int flags)
: d_isTimedChannel(0)
, d_isTimedOperation(1)
, d_timeout(timeout)
, d_flags(flags)
{
    new (d_cb.d_callbackArena)
            bdef_Function<void (*)(btesc_CbChannel*, int)>(functor);
}

btesos_TcpTimedCbConnector_Reg::~btesos_TcpTimedCbConnector_Reg()
{
    if (d_isTimedChannel) {
        bdef_Function<void (*)(btesc_TimedCbChannel*, int)> *cb =
            (bdef_Function<void (*)(btesc_TimedCbChannel*, int)> *)
                                                  (void *)d_cb.d_callbackArena;
        cb->~bdef_Function<void (*)(btesc_TimedCbChannel*, int)>();
    }
    else {
        bdef_Function<void (*)(btesc_TimedCbChannel*, int)> *cb =
             (bdef_Function<void (*)(btesc_TimedCbChannel*, int)>*)
                (void *)d_cb.d_callbackArena;
        cb->~bdef_Function<void (*)(btesc_TimedCbChannel*, int)>();
    }
}

inline
void btesos_TcpTimedCbConnector_Reg::invoke(int status) {
    if (d_isTimedChannel) {
        invokeTimed(NULL, status);
    }
    else {
        invoke(NULL, status);
    }
}

inline
void btesos_TcpTimedCbConnector_Reg::invoke(btesc_CbChannel *channel,
                                            int              status) {
    BSLS_ASSERT(0 == d_isTimedChannel);
    bdef_Function<void (*)(btesc_CbChannel*, int)> *cb =
             (bdef_Function<void (*)(btesc_CbChannel*, int)>*)
                (void *)d_cb.d_callbackArena;
    (*cb)(channel, status);
}

inline
void btesos_TcpTimedCbConnector_Reg::invokeTimed(btesc_TimedCbChannel *channel,
                                                 int                   status)
{
    bdef_Function<void (*)(btesc_TimedCbChannel*, int)> *cb =
        (bdef_Function<void (*)(btesc_TimedCbChannel*, int)>*)
            (void *)d_cb.d_callbackArena;
    (*cb)(channel, status);
}

inline int btesos_TcpTimedCbConnector_Reg::isTimedResult() const {
    return d_isTimedChannel;
}

inline int btesos_TcpTimedCbConnector_Reg::isTimedOperation() const {
    return d_isTimedOperation;
}

// ACCESSORS
inline int btesos_TcpTimedCbConnector_Reg::flags() const {
    return d_flags;
}

inline
const bdef_Function<void (*)(btesc_CbChannel*, int)>&
btesos_TcpTimedCbConnector_Reg::callback() const {
    BSLS_ASSERT(0 == d_isTimedChannel);
    return *(bdef_Function<void (*)(btesc_CbChannel*, int)>*)
                (void *)d_cb.d_callbackArena;
}

inline
const bdef_Function<void (*)(btesc_TimedCbChannel*, int)>&
btesos_TcpTimedCbConnector_Reg::timedCallback() const {
    BSLS_ASSERT(1 == d_isTimedChannel);
    return *(bdef_Function<void (*)(btesc_TimedCbChannel*, int)>*)
                                                  (void *)d_cb.d_callbackArena;
}

// ============================================================================
//                        END LOCAL DEFINITIONS
// ============================================================================

                       // --------------------------------
                       // class btesos_TcpTimedCbConnector
                       // --------------------------------

// PRIVATE MANIPULATORS

template <class CALLBACK_TYPE, class CHANNEL>
int btesos_TcpTimedCbConnector::initiateTimedConnection(
                                       const CALLBACK_TYPE&     callback,
                                       const bdet_TimeInterval& timeout,
                                       int                      flags,
                                       int                      enqueueRequest)
    // Initiate a non-blocking connection to the peer server, invoke
    // the specified 'callback' if the operation completed immediately,
    // either successfully or not, (and allocate a channel, if needed),
    // or enqueue the request for a connection completion after
    // the specified 'timeout' absolute time.  Return 0 if operation
    // completed successfully, a negative value if an error occurred and
    // a positive value if an operation could not complete immediately
    // (and thus was enqueued for further processing).
{
    d_connectingSocket_p = d_factory_p->allocate();
    if (NULL == d_connectingSocket_p) {
        callback(NULL, UNINITIALIZED);
        return UNINITIALIZED;
    }

    if (0 != d_connectingSocket_p->
                           setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE))
    {
        d_factory_p->deallocate(d_connectingSocket_p);
        d_connectingSocket_p = NULL;
        callback(NULL, UNINITIALIZED);
        return UNINITIALIZED;
    }

    int s = d_connectingSocket_p->connect(d_peerAddress);

    if ( s == bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK ||
        (s == bteso_SocketHandle::BTESO_ERROR_INTERRUPTED &&
         0 == (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT)))
    {
        if (enqueueRequest) {
            btesos_TcpTimedCbConnector_Reg *cb =
                new (d_callbackPool) btesos_TcpTimedCbConnector_Reg(timeout,
                                                           callback, flags);
            d_callbacks.push_front(cb);
        }

        if (0 != d_manager_p->registerSocketEvent(
                                         d_connectingSocket_p->handle(),
                                         bteso_EventType::BTESO_CONNECT,
                                         d_connectFunctor))
        {
            d_factory_p->deallocate(d_connectingSocket_p);
            d_connectingSocket_p = NULL;
            if (enqueueRequest) {
                // If a request was just created, it must be destroyed
                btesos_TcpTimedCbConnector_Reg *cb = d_callbacks.front();
                BSLS_ASSERT(cb);
                d_callbackPool.deleteObjectRaw(cb);
                d_callbacks.pop_front();
            }

            callback(0, FAILED_TO_REG);
            return FAILED_TO_REG;
        }
        d_timerId =
            d_manager_p->registerTimer(timeout, d_timeoutFunctor);

        return ENQUEUED;
    }

    if (0 == s) {
        // Connection established
        CHANNEL *channel = new (d_channelPool) CHANNEL(d_connectingSocket_p,
                                               d_manager_p,
                                               d_allocator_p);

        bsl::vector<btesc_CbChannel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(), channel);
        d_channels.insert(idx, channel);
        d_connectingSocket_p = NULL;
        callback(channel, 0);
        return SUCCESS;
    }
    if (s == bteso_SocketHandle::BTESO_ERROR_INTERRUPTED) {
        BSLS_ASSERT(btesc_Flag::BTESC_ASYNC_INTERRUPT & flags);
        callback(NULL, 1);
        return SUCCESS;
    }
    // Hard error occurred
    BSLS_ASSERT(s < 0);
    d_factory_p->deallocate(d_connectingSocket_p);
    d_connectingSocket_p = NULL;
    callback(NULL, s - 2);
    return SUCCESS;
}

template <class CALLBACK_TYPE, class CHANNEL>
int btesos_TcpTimedCbConnector::initiateConnection(
                                            const CALLBACK_TYPE& callback,
                                            int                  flags,
                                            int                  createRequest)
    // Initiate a non-blocking connection to the peer server, invoke
    // the specified 'callback' if operation completed immediately,
    // either successfully or not, (and allocate a channel, if needed),
    // or enqueue the request for a connection completion.   Return 0 if
    // operation completed successfully, a negative value if an error
    // occurred and a positive value if an operation could not complete
    // immediately (and thus was enqueued for further processing).
{
    d_connectingSocket_p = d_factory_p->allocate();
    if (NULL == d_connectingSocket_p) {
        callback(NULL, UNINITIALIZED);
        return UNINITIALIZED;
    }

    if (0 != d_connectingSocket_p->
                           setBlockingMode(bteso_Flag::BTESO_NONBLOCKING_MODE))
    {
        d_factory_p->deallocate(d_connectingSocket_p);
        d_connectingSocket_p = NULL;
        callback(NULL, UNINITIALIZED);
        return UNINITIALIZED;
    }
    int s = d_connectingSocket_p->connect(d_peerAddress);

    if ( s == bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK ||
        (s == bteso_SocketHandle::BTESO_ERROR_INTERRUPTED &&
         0 == (flags & btesc_Flag::BTESC_ASYNC_INTERRUPT)))
    {
        if (createRequest) {
            btesos_TcpTimedCbConnector_Reg *cb =
                new (d_callbackPool)
                         btesos_TcpTimedCbConnector_Reg(callback, flags);
            d_callbacks.push_front(cb);
        }
        if (0 != d_manager_p->registerSocketEvent(
                                         d_connectingSocket_p->handle(),
                                         bteso_EventType::BTESO_CONNECT,
                                         d_connectFunctor)){
            d_factory_p->deallocate(d_connectingSocket_p);
            d_connectingSocket_p = NULL;
            if (createRequest) {
                // If a request was just created, it must be destroyed
                btesos_TcpTimedCbConnector_Reg *cb = d_callbacks.front();
                BSLS_ASSERT(cb);
                d_callbackPool.deleteObjectRaw(cb);
                d_callbacks.pop_front();
            }
            callback(0, FAILED_TO_REG);
            return FAILED_TO_REG;
        }

        d_timerId = NULL;
        return 1;
    }

    if (0 == s) {
        // Connection established
        CHANNEL *channel = new (d_channelPool) CHANNEL(d_connectingSocket_p,
                                                       d_manager_p,
                                                       d_allocator_p);

        bsl::vector<btesc_CbChannel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(), channel);
        d_channels.insert(idx, channel);
        d_connectingSocket_p = NULL;
        callback(channel, 0);
        return SUCCESS;
    }
    if (s == bteso_SocketHandle::BTESO_ERROR_INTERRUPTED) {
        BSLS_ASSERT(btesc_Flag::BTESC_ASYNC_INTERRUPT & flags);
        callback(NULL, 1);
        return SUCCESS;
    }
    // Hard error occurred
    BSLS_ASSERT(s < 0);
    callback(NULL, s - 2);
    d_factory_p->deallocate(d_connectingSocket_p);
    d_connectingSocket_p = NULL;
    return SUCCESS;
}

void btesos_TcpTimedCbConnector::timerCb() {
    BSLS_ASSERT(d_callbacks.size());  // at least one must be registered
    BSLS_ASSERT(d_connectingSocket_p);
    d_currentRequest_p = d_callbacks.back();
    BSLS_ASSERT(d_currentRequest_p);

    d_manager_p->deregisterSocketEvent(d_connectingSocket_p->handle(),
                                       bteso_EventType::BTESO_CONNECT);
    d_factory_p->deallocate(d_connectingSocket_p);
    d_connectingSocket_p = NULL;

    d_currentRequest_p->invoke(TIMEDOUT);

    int status = 0;
    while (d_callbacks.size() > 1 && 0 == status) {
        // Deallocate previous request.
        d_callbackPool.deleteObjectRaw(d_currentRequest_p);
        d_callbacks.pop_back();
        d_currentRequest_p = d_callbacks.back();

        if (d_currentRequest_p->isTimedOperation()) {
            if (d_currentRequest_p->isTimedResult()) {

                status = initiateTimedConnection<TimedCallback,
                             btesos_TcpTimedCbChannel> (
                                 d_currentRequest_p->timedCallback(),
                                 d_currentRequest_p->timeout(),
                                 d_currentRequest_p->flags(), 0);

            }
            else {
                status = initiateTimedConnection<Callback,
                             btesos_TcpCbChannel> (
                                 d_currentRequest_p->callback(),
                                 d_currentRequest_p->timeout(),
                                 d_currentRequest_p->flags(), 0);
            }
        } else {
            if (d_currentRequest_p->isTimedResult()) {
                status = initiateConnection<TimedCallback,
                             btesos_TcpTimedCbChannel> (
                                     d_currentRequest_p->timedCallback(),
                                     d_currentRequest_p->flags(), 0);
            }
            else {
                status = initiateConnection<Callback,
                             btesos_TcpCbChannel> (
                                     d_currentRequest_p->callback(),
                                     d_currentRequest_p->flags(), 0);
            }
        }
    }
    if (ENQUEUED > status) {
        d_callbackPool.deleteObjectRaw(d_currentRequest_p);
        d_callbacks.pop_back();
    }
    d_currentRequest_p = NULL;
}

void btesos_TcpTimedCbConnector::connectCb() {
    BSLS_ASSERT(d_callbacks.size());
    BSLS_ASSERT(d_connectingSocket_p);

    // Deregister associated timer, if any.
    if (d_timerId) {
        d_manager_p->deregisterTimer(d_timerId);
        d_timerId = NULL;
    }
    BSLS_ASSERT(NULL == d_timerId);   // internal (in-method) invariant
    d_manager_p->deregisterSocketEvent(d_connectingSocket_p->handle(),
                                       bteso_EventType::BTESO_CONNECT);

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
            btesos_TcpTimedCbChannel *channel =
                new (d_channelPool)
                            btesos_TcpTimedCbChannel(d_connectingSocket_p,
                                                     d_manager_p,
                                                     d_allocator_p);
            bsl::vector<btesc_CbChannel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(), channel);
            d_channels.insert(idx, channel);
            d_connectingSocket_p = NULL;
            d_currentRequest_p->invokeTimed(channel, 0);
        }
        else {
            btesos_TcpCbChannel *channel =
                new (d_channelPool) btesos_TcpCbChannel(d_connectingSocket_p,
                                                d_manager_p,
                                                d_allocator_p);
            bsl::vector<btesc_CbChannel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(), channel);
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
                             btesos_TcpTimedCbChannel> (
                                 d_currentRequest_p->timedCallback(),
                                 d_currentRequest_p->timeout(),
                                 d_currentRequest_p->flags(), 0);

            }
            else {
                status = initiateTimedConnection<Callback,
                             btesos_TcpCbChannel> (
                                 d_currentRequest_p->callback(),
                                 d_currentRequest_p->timeout(),
                                 d_currentRequest_p->flags(), 0);
            }
        } else {
            if (d_currentRequest_p->isTimedResult()) {
                status = initiateConnection<TimedCallback,
                             btesos_TcpTimedCbChannel> (
                                  d_currentRequest_p->timedCallback(),
                                  d_currentRequest_p->flags(), 0);
            }
            else {
                status = initiateConnection<Callback,
                             btesos_TcpCbChannel> (
                                 d_currentRequest_p->callback(),
                                 d_currentRequest_p->flags(), 0);
            }
        }
    }
    if (ENQUEUED > status) {
        d_callbackPool.deleteObjectRaw(d_currentRequest_p);
        d_callbacks.pop_back();
        d_connectingSocket_p = NULL;
    }
    d_currentRequest_p = NULL;
}

void btesos_TcpTimedCbConnector::deallocateCb(btesc_CbChannel *channel) {
    BSLS_ASSERT(channel);
    bteso_StreamSocket<bteso_IPv4Address> *s = NULL;
    btesos_TcpTimedCbChannel *c =
        dynamic_cast<btesos_TcpTimedCbChannel*>(channel);
    if (c) {
        s = c->socket();
    }
    else {
        btesos_TcpCbChannel *c =
            dynamic_cast<btesos_TcpCbChannel*>(channel);
        BSLS_ASSERT(c);
        s = c->socket();
    }
    BSLS_ASSERT(s);
    channel->~btesc_CbChannel();
    d_factory_p->deallocate(s);

    bsl::vector<btesc_CbChannel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(), channel);
    BSLS_ASSERT(idx != d_channels.end() && *idx == channel);
    d_channels.erase(idx);

    d_channelPool.deleteObjectRaw(channel);
    return;
}

// CREATORS

btesos_TcpTimedCbConnector::btesos_TcpTimedCbConnector(
                  bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
                  bteso_TimerEventManager                      *manager,
                  bslma_Allocator                              *basicAllocator)
: d_callbackPool(sizeof(btesos_TcpTimedCbConnector_Reg), basicAllocator)
, d_channelPool(CHANNEL_SIZE, basicAllocator)
, d_channels(basicAllocator)
, d_currentRequest_p(NULL)
, d_manager_p(manager)
, d_factory_p(factory)
, d_connectingSocket_p(NULL)
, d_isInvalidFlag(0)
, d_allocator_p(basicAllocator)
{
    d_connectFunctor
        = bdef_Function<void (*)()>(
            bdef_MemFnUtil::memFn(&btesos_TcpTimedCbConnector::connectCb, this)
          , d_allocator_p);

    d_timeoutFunctor
        = bdef_Function<void (*)()>(
            bdef_MemFnUtil::memFn(&btesos_TcpTimedCbConnector::timerCb, this)
          , d_allocator_p);
}

btesos_TcpTimedCbConnector::btesos_TcpTimedCbConnector(
                  bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
                  bteso_TimerEventManager                      *manager,
                  int                                           numChannels,
                  bslma_Allocator                              *basicAllocator)
: d_callbackPool(sizeof(btesos_TcpTimedCbConnector_Reg), basicAllocator)
, d_channelPool(CHANNEL_SIZE, basicAllocator)
, d_channels(basicAllocator)
, d_currentRequest_p(NULL)
, d_manager_p(manager)
, d_factory_p(factory)
, d_connectingSocket_p(NULL)
, d_isInvalidFlag(0)
, d_allocator_p(basicAllocator)
{
    d_connectFunctor
        = bdef_Function<void (*)()>(
            bdef_MemFnUtil::memFn(&btesos_TcpTimedCbConnector::connectCb, this)
          , d_allocator_p);

    d_timeoutFunctor
        = bdef_Function<void (*)()>(
            bdef_MemFnUtil::memFn(&btesos_TcpTimedCbConnector::timerCb, this)
          , d_allocator_p);

    d_channelPool.reserveCapacity(numChannels);
}

btesos_TcpTimedCbConnector::~btesos_TcpTimedCbConnector()
{
    invalidate();
    cancelAll();

    // Deallocate channels
    while(d_channels.size()) {
        btesc_CbChannel *ch = d_channels[0];
        BSLS_ASSERT(ch);
        ch->invalidate();
        ch->cancelAll();
        deallocateCb(ch);
    }
}

// MANIPULATORS

int btesos_TcpTimedCbConnector::allocate(const Callback& callback,
                                         int             flags)
{
    if (d_isInvalidFlag) {
        return INVALID;
    }

    if (d_callbacks.size() == 0) {
        initiateConnection<Callback, btesos_TcpTimedCbChannel>
                (callback, flags, 1);
        return 0;

    }
    btesos_TcpTimedCbConnector_Reg *cb =
        new (d_callbackPool) btesos_TcpTimedCbConnector_Reg(callback, flags);
    d_callbacks.push_front(cb);
    return 0;
}

int btesos_TcpTimedCbConnector::allocateTimed(const TimedCallback& callback,
                                              int                  flags)
{
    if (d_isInvalidFlag) {
        return INVALID;
    }

    // Implementation note: the request must be pushed onto the queue
    // before the corresponding callback is registered with an
    // event manager.

    if (d_callbacks.size() == 0) {
        initiateConnection<TimedCallback, btesos_TcpTimedCbChannel>
            (callback, flags, 1);
        return 0;
    }

    btesos_TcpTimedCbConnector_Reg *cb =
        new (d_callbackPool) btesos_TcpTimedCbConnector_Reg(callback, flags);
    d_callbacks.push_front(cb);
    return 0;
}

void btesos_TcpTimedCbConnector::cancelAll() {
    if (d_currentRequest_p) {
        // A callback is active -- can't destroy current request.
        bsl::deque<btesos_TcpTimedCbConnector_Reg *> toBeCancelled(
                                  d_callbacks.begin(),
                                  d_callbacks.begin() + d_callbacks.size() - 1,
                                  d_allocator_p);
        d_callbacks.erase(d_callbacks.begin(),
                          d_callbacks.begin() + d_callbacks.size() - 1);
        BSLS_ASSERT(d_currentRequest_p == d_callbacks.back());
        int numToCancel = toBeCancelled.size();
        while (--numToCancel >= 0) {
            btesos_TcpTimedCbConnector_Reg *reg = toBeCancelled[numToCancel];
            reg->invoke(-1);
            d_callbackPool.deleteObjectRaw(reg);
        }
    }
    else {
        // This part is reached when 'cancelAll' is invoked not
        // from a callback.

        bsl::deque<btesos_TcpTimedCbConnector_Reg *> toBeCancelled(
                                                   d_callbacks, d_allocator_p);
        d_callbacks.clear();
        int numToCancel = toBeCancelled.size();
        if (numToCancel) {
            BSLS_ASSERT(d_connectingSocket_p);
            d_manager_p->deregisterSocketEvent(d_connectingSocket_p->handle(),
                                               bteso_EventType::BTESO_CONNECT);

            if (d_timerId) {
                d_manager_p->deregisterTimer(d_timerId);
                d_timerId = NULL;
            }
            d_factory_p->deallocate(d_connectingSocket_p);
        }

        while(--numToCancel >= 0) {
            btesos_TcpTimedCbConnector_Reg *reg = toBeCancelled[numToCancel];
            reg->invoke(-1);
            d_callbackPool.deleteObjectRaw(reg);
        }
    }
}

void btesos_TcpTimedCbConnector::deallocate(btesc_CbChannel *channel)
{
    BSLS_ASSERT(channel);
    channel->invalidate();
    channel->cancelAll();
    bdef_Function<void (*)()> cb(
            bdef_BindUtil::bindA(
                d_allocator_p
              , &btesos_TcpTimedCbConnector::deallocateCb
              , this
              , channel));
    d_manager_p->registerTimer(bdetu_SystemTime::now(), cb);
}

int btesos_TcpTimedCbConnector::timedAllocate(
        const Callback&          callback,
        const bdet_TimeInterval& timeout,
        int                      flags)
{
    if (d_isInvalidFlag) {
        return INVALID;
    }

    if (d_callbacks.size() == 0) {
        initiateTimedConnection<Callback, btesos_TcpTimedCbChannel>
                (callback, timeout, flags, 1);
        return 0;
    }

    btesos_TcpTimedCbConnector_Reg *cb =
        new (d_callbackPool)
                     btesos_TcpTimedCbConnector_Reg(timeout, callback, flags);
    d_callbacks.push_front(cb);
    return 0;
}

int btesos_TcpTimedCbConnector::timedAllocateTimed(
                                             const TimedCallback&     callback,
                                             const bdet_TimeInterval& timeout,
                                             int                      flags)
{
    if (d_isInvalidFlag) {
        return INVALID;
    }

    if (d_callbacks.size() == 0) {
        BSLS_ASSERT(NULL == d_connectingSocket_p);
        return initiateTimedConnection<TimedCallback, btesos_TcpTimedCbChannel>
                                      (callback, timeout, flags, 1) < 0;
    }
    btesos_TcpTimedCbConnector_Reg *cb =
        new (d_callbackPool)
                   btesos_TcpTimedCbConnector_Reg(timeout, callback, flags);
    d_callbacks.push_front(cb);
    return 0;
}

void btesos_TcpTimedCbConnector::invalidate() {
    d_isInvalidFlag = 1;
}

// ACCESSORS

int btesos_TcpTimedCbConnector::isInvalid() const {
    return d_isInvalidFlag;
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
