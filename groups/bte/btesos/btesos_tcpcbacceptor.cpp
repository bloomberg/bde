// btesos_tcpcbacceptor.cpp  -*-C++-*-
#include <btesos_tcpcbacceptor.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btesos_tcpcbacceptor_cpp,"$Id$ $CSID$")

#include <btesos_tcptimedcbchannel.h>
#include <btesos_tcpcbchannel.h>
#include <bteso_timereventmanager.h>
#include <bteso_streamsocketfactory.h>
#include <bteso_streamsocket.h>
#include <btesc_flag.h>

#include <bdetu_systemtime.h>
#include <bdef_function.h>
#include <bdef_memfn.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_blockgrowth.h>

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
// 1. Internally, this acceptor holds a queue of callbacks for allocation
// requests.  The queue contains non-timed callbacks along with any
// supporting data for a request, if any.
//
// 2. The request queue keeps the type of the result (i.e., timed or non-timed
// channel) and the request can invoke the callback in a type-safe manner.
//
// 3. All allocate methods register callbacks with a socket event manager if
// the request queue size is 1 (i.e., when only the current request is
// cached).
//
// 4. The 'deallocate' method does not deallocate a channel directly, it
// rather installs the deallocate callback to be invoked on the next
// invocation of the 'dispatch' method of the event manager.  The
// deallocate callback will actually destroy the resources allocates for a
// channel.
// ===========================================================================

namespace BloombergLP {

// ============================================================================
//                        LOCAL DEFINITIONS
// ============================================================================

                       // ========================
                       // Local typedefs and enums
                       // ========================

enum {
    CALLBACK_SIZE      = sizeof(btesc_CbChannelAllocator::Callback),
    TIMEDCALLBACK_SIZE = sizeof (btesc_CbChannelAllocator::TimedCallback),
    ARENA_SIZE         = CALLBACK_SIZE < TIMEDCALLBACK_SIZE
                         ? TIMEDCALLBACK_SIZE : CALLBACK_SIZE
};

enum {
    CHANNEL_SIZE =
        sizeof(btesos_TcpCbChannel) < sizeof(btesos_TcpTimedCbChannel)
        ? sizeof(btesos_TcpTimedCbChannel)
        : sizeof(btesos_TcpCbChannel)
};

enum {
    CLOSED        = -3,
    UNINITIALIZED = -2,
    INVALID       = -1,
    CANCELLED     = -1,
    SUCCESS       = 0
};

                    // ==============================
                    // class btesos_TcpCbAcceptor_Reg
                    // ==============================

class btesos_TcpCbAcceptor_Reg {
    // This class stores a callback, and allows to invoke it.

    union {
        char                               d_callbackArena[ARENA_SIZE];
        bsls_AlignmentUtil::MaxAlignedType d_align;  // for alignment
    } d_cb;                                      // callback storage arena

    int d_isTimedChannel;  // true if the channel is timed
    int d_flags;           // associated flags

  private:
    // Not implemented.
    btesos_TcpCbAcceptor_Reg(const btesos_TcpCbAcceptor_Reg&);
    btesos_TcpCbAcceptor_Reg&
        operator=(const btesos_TcpCbAcceptor_Reg&);
  public:
    // CREATORS
    btesos_TcpCbAcceptor_Reg(
            const bdet_TimeInterval& timeout,
            const bdef_Function<void (*)(btesc_TimedCbChannel*, int)>& functor,
            int flags);
    btesos_TcpCbAcceptor_Reg(
            const bdef_Function<void (*)(btesc_CbChannel*, int)>& functor,
            int flags);
    btesos_TcpCbAcceptor_Reg(
            const bdef_Function<void (*)(btesc_TimedCbChannel*, int)>& functor,
            int flags);
    btesos_TcpCbAcceptor_Reg(
            const bdet_TimeInterval& timeout,
            const bdef_Function<void (*)(btesc_CbChannel*, int)>& functor,
            int flags);
        // Create a callback from a specified functor 'func' with specified
        // 'flags', and an optionally specified 'timeout' for the callback
        // execution.

    ~btesos_TcpCbAcceptor_Reg();
        // Destroy my object.

    // MANIPULATORS
    void invoke(int status);
    void invoke(btesc_CbChannel *channel, int status);
    void invokeTimed(btesc_TimedCbChannel *channel, int status);
        // Invoke the callback functor contained in this request passing to it
        // an optionally specified 'channel' value for the channel address and
        // the specified 'status'.  If 'channel' is not specified, a NULL value
        // is used.

    // ACCESSORS
    int flags() const;
        // Access the 'flags' with which this object was created.

    int isTimedResult() const;
        // Return non-zero if the underlying channel is timed, 0 otherwise.
};

// CREATORS
btesos_TcpCbAcceptor_Reg::btesos_TcpCbAcceptor_Reg(
    const bdef_Function<void (*)(btesc_TimedCbChannel*, int)>& functor,
    int flags)
: d_isTimedChannel(1)
, d_flags(flags)
{
    new (d_cb.d_callbackArena)
                  bdef_Function<void (*)(btesc_TimedCbChannel*, int)>(functor);
}

btesos_TcpCbAcceptor_Reg::btesos_TcpCbAcceptor_Reg(
    const bdef_Function<void (*)(btesc_CbChannel*, int)>& functor,
    int flags)
: d_isTimedChannel(0)
, d_flags(flags)
{
    new (d_cb.d_callbackArena)
                       bdef_Function<void (*)(btesc_CbChannel*, int)>(functor);
}

btesos_TcpCbAcceptor_Reg::~btesos_TcpCbAcceptor_Reg()
{
    if (d_isTimedChannel) {
        bdef_Function<void (*)(btesc_TimedCbChannel*, int)> *cb =
            (bdef_Function<void (*)(btesc_TimedCbChannel*, int)> *)
                (void *) d_cb.d_callbackArena;

        bslalg_ScalarDestructionPrimitives::destroy(cb);
    }
    else {
        bdef_Function<void (*)(btesc_CbChannel*, int)> *cb =
            (bdef_Function<void (*)(btesc_CbChannel*, int)> *)
                (void *) d_cb.d_callbackArena;

        bslalg_ScalarDestructionPrimitives::destroy(cb);
    }
}

// MANIPULATORS
inline
void btesos_TcpCbAcceptor_Reg::invoke(int status)
{
    if (d_isTimedChannel) {
        this->invokeTimed(NULL, status);
    }
    else {
        this->invoke(NULL, status);
    }
}

inline
void btesos_TcpCbAcceptor_Reg::invoke(btesc_CbChannel *channel,
                                           int              status)
{
    BSLS_ASSERT(0 == d_isTimedChannel);
    bdef_Function<void (*)(btesc_CbChannel*, int)> *cb =
        (bdef_Function<void (*)(btesc_CbChannel*, int)> *)
            (void *) d_cb.d_callbackArena;
    (*cb)(channel, status);
}

inline
void btesos_TcpCbAcceptor_Reg::invokeTimed(btesc_TimedCbChannel *channel,
                                           int                   status)
{
    BSLS_ASSERT(1 == d_isTimedChannel);
    bdef_Function<void (*)(btesc_TimedCbChannel*, int)> *cb =
        (bdef_Function<void (*)(btesc_TimedCbChannel*, int)> *)
            (void *) d_cb.d_callbackArena;
    (*cb)(channel, status);
}

// ACCESSORS
inline
int btesos_TcpCbAcceptor_Reg::flags() const
{
    return d_flags;
}

inline
int btesos_TcpCbAcceptor_Reg::isTimedResult() const
{
    return d_isTimedChannel;
}

// ============================================================================
//                        END OF LOCAL DEFINITIONS
// ============================================================================

                          // --------------------------
                          // class btesos_TcpCbAcceptor
                          // --------------------------

//PRIVATE MANIPULATORS

void btesos_TcpCbAcceptor::acceptCb() {
    BSLS_ASSERT(d_callbacks.size()); // At least one must be registered.

    // Accept the socket connection prior to getting the current request.

    bteso_StreamSocket<bteso_IPv4Address> *connection;
    int status = d_serverSocket_p->accept(&connection);

    d_currentRequest_p = d_callbacks.back();
    BSLS_ASSERT(d_currentRequest_p);

    if (!status) {    // A new connection is accepted.
        BSLS_ASSERT(connection);

        if (d_currentRequest_p->isTimedResult()) {
            btesos_TcpTimedCbChannel *result =
                   new (d_channelPool) btesos_TcpTimedCbChannel(connection,
                                                                d_manager_p,
                                                                d_allocator_p);
            bsl::vector<btesc_CbChannel*>::iterator idx =
                bsl::lower_bound(d_channels.begin(), d_channels.end(),
                                 static_cast<btesc_CbChannel*>(result));
            d_channels.insert(idx, result);
            d_currentRequest_p->invokeTimed(result, 0);
        }
        else {
            btesos_TcpCbChannel *result =
                new (d_channelPool)
                     btesos_TcpCbChannel(connection,
                                         d_manager_p,
                                         d_allocator_p);
            bsl::vector<btesc_CbChannel*>::iterator idx =
                bsl::lower_bound(d_channels.begin(), d_channels.end(),
                                 static_cast<btesc_CbChannel*>(result));
            d_channels.insert(idx, result);
            d_currentRequest_p->invoke(result, 0);
        }
    }
    else {  // Existing connection - find out what happened
        if (status == bteso_SocketHandle::BTESO_ERROR_INTERRUPTED &&
            d_currentRequest_p->flags() & btesc_Flag::BTESC_ASYNC_INTERRUPT) {
            d_currentRequest_p->invoke(btesc_Flag::BTESC_ASYNC_INTERRUPT);
        }
        else {
            if (status != bteso_SocketHandle::BTESO_ERROR_WOULDBLOCK) {
                d_currentRequest_p->invoke(status);
            }
            else {
                return; // EWOULDBLOCK -- ignored.
            }
        }
    }

    d_callbacks.pop_back();
    d_callbackPool.deleteObjectRaw(d_currentRequest_p);

    if (d_callbacks.size()) {
        d_currentRequest_p = d_callbacks.back();
    }
    else {
        if (d_serverSocket_p) {
            // If 'close' was called in the invoked callback, the listening
            // socket was deallocated, and 'd_serverSocket_p' is NULL.

            d_manager_p->deregisterSocketEvent(d_serverSocket_p->handle(),
                                               bteso_EventType::BTESO_ACCEPT);
        }
    }

    d_currentRequest_p = NULL; // internal class invariant
}

void btesos_TcpCbAcceptor::deallocateCb(btesc_CbChannel *channel)
{
    BSLS_ASSERT(channel);

    btesos_TcpTimedCbChannel *c =
        dynamic_cast<btesos_TcpTimedCbChannel*>(channel);
    bteso_StreamSocket<bteso_IPv4Address> *s = NULL;

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
    channel->~btesc_CbChannel();  // This will cancel all pending requests.
    d_factory_p->deallocate(s);

    bsl::vector<btesc_CbChannel*>::iterator idx =
               bsl::lower_bound(d_channels.begin(), d_channels.end(), channel);
    BSLS_ASSERT(idx != d_channels.end() && *idx == channel);
    d_channels.erase(idx);
    d_channelPool.deleteObject(channel);
}

// CREATORS

btesos_TcpCbAcceptor::btesos_TcpCbAcceptor(
        bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
        bteso_TimerEventManager                      *manager,
        bslma_Allocator                              *basicAllocator)
: d_callbackPool(sizeof(btesos_TcpCbAcceptor_Reg), basicAllocator)
, d_channelPool(CHANNEL_SIZE, basicAllocator)
, d_callbacks(basicAllocator)
, d_channels(basicAllocator)
, d_manager_p(manager)
, d_factory_p(factory)
, d_serverSocket_p(NULL)
, d_isInvalidFlag(0)
, d_currentRequest_p(NULL)
, d_allocator_p(basicAllocator)
{
    d_acceptFunctor
        = bdef_Function<void (*)()>(
                bdef_MemFnUtil::memFn(&btesos_TcpCbAcceptor::acceptCb, this),
                d_allocator_p);
}

btesos_TcpCbAcceptor::btesos_TcpCbAcceptor(
        bteso_StreamSocketFactory<bteso_IPv4Address> *factory,
        bteso_TimerEventManager                      *manager,
        int                                           numElements,
        bslma_Allocator                              *basicAllocator)
: d_callbackPool(sizeof(btesos_TcpCbAcceptor_Reg), basicAllocator)
, d_channelPool(CHANNEL_SIZE,
                bsls_BlockGrowth::BSLS_CONSTANT,
                numElements,
                basicAllocator)
, d_callbacks(basicAllocator)
, d_channels(numElements, (btesc_CbChannel *)0, basicAllocator)
, d_manager_p(manager)
, d_factory_p(factory)
, d_serverSocket_p(NULL)
, d_isInvalidFlag(0)
, d_currentRequest_p(NULL)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT(0 < numElements);
    d_acceptFunctor
        = bdef_Function<void (*)()>(
                bdef_MemFnUtil::memFn(&btesos_TcpCbAcceptor::acceptCb, this),
                d_allocator_p);
}

btesos_TcpCbAcceptor::~btesos_TcpCbAcceptor()
{
    invalidate();
    cancelAll();
    if (d_serverSocket_p) {
        close();
    }
    // Deallocate channels
    while(d_channels.size()) {
        btesc_CbChannel *ch = d_channels[0];
        BSLS_ASSERT(ch);
        ch->invalidate();
        deallocateCb(ch);
    }
}

// MANIPULATORS

int btesos_TcpCbAcceptor::allocate(const Callback& callback,
                                   int             flags)
{
    if (d_isInvalidFlag) {
        return INVALID;
    }
    if (NULL == d_serverSocket_p) {
        callback(NULL, UNINITIALIZED);
        return UNINITIALIZED;
    }

    btesos_TcpCbAcceptor_Reg *cb =
        new (d_callbackPool) btesos_TcpCbAcceptor_Reg(callback, flags);

    // For safety, we push the callback before registering the socket event,
    // although when this component is used by 'btemt_channelpool' (as
    // intended), the event ('d_acceptFunctor') should not be called until
    // *after* this function returns since both this function and the event are
    // processed in the I/O thread of the 'btemt_ChannelPool' object.

    d_callbacks.push_front(cb);
    if (1 == d_callbacks.size()) {
        if (0 != d_manager_p->registerSocketEvent(d_serverSocket_p->handle(),
                                                 bteso_EventType::BTESO_ACCEPT,
                                                 d_acceptFunctor)) {
            d_callbacks.pop_back();
            cb->invoke(CANCELLED);
            d_callbackPool.deleteObjectRaw(cb);
            return INVALID;
        }
    }
    return SUCCESS;
}

int btesos_TcpCbAcceptor::allocateTimed(const TimedCallback& callback,
                                        int                  flags)
{
    if (d_isInvalidFlag) {
        return INVALID;
    }
    if (NULL == d_serverSocket_p) {
        callback(NULL, UNINITIALIZED);
        return UNINITIALIZED;
    }

    btesos_TcpCbAcceptor_Reg *cb =
        new (d_callbackPool) btesos_TcpCbAcceptor_Reg(callback, flags);

    // For safety, we push the callback before registering the socket event,
    // although when this component is used by 'btemt_channelpool' (as
    // intended), the event ('d_acceptFunctor') should not be called until
    // *after* this function returns since both this function and the event are
    // processed in the I/O thread of the 'btemt_ChannelPool' object.

    d_callbacks.push_front(cb);
    if (1 == d_callbacks.size()) {
        if (0 != d_manager_p->registerSocketEvent(d_serverSocket_p->handle(),
                                         bteso_EventType::BTESO_ACCEPT,
                                         d_acceptFunctor)) {
            d_callbacks.pop_back();
            cb->invoke(CANCELLED);
            d_callbackPool.deleteObjectRaw(cb);
            return INVALID;
        }
    }
    return SUCCESS;
}

void btesos_TcpCbAcceptor::cancelAll()
{
    if (d_currentRequest_p) {
        // A callback is active -- can't destroy current request.
        bsl::deque<btesos_TcpCbAcceptor_Reg *> toBeCancelled(d_allocator_p);
        toBeCancelled.insert(toBeCancelled.begin(), d_callbacks.begin(),
                             d_callbacks.begin() + d_callbacks.size() - 1);

        d_callbacks.erase(d_callbacks.begin(),
                          d_callbacks.begin() + d_callbacks.size() - 1);
        BSLS_ASSERT(d_currentRequest_p == d_callbacks.back());

        int numToCancel = toBeCancelled.size();
        while (--numToCancel >= 0) {
            btesos_TcpCbAcceptor_Reg *reg = toBeCancelled[numToCancel];
            BSLS_ASSERT(reg);
            reg->invoke(-1);
            d_callbackPool.deleteObjectRaw(reg);
        }
        BSLS_ASSERT(d_currentRequest_p == d_callbacks.back());
    }
    else {
        bsl::deque<btesos_TcpCbAcceptor_Reg *>
                                     toBeCancelled(d_callbacks, d_allocator_p);
        d_callbacks.clear();
        int numToCancel = toBeCancelled.size();
        if (numToCancel) {
            d_manager_p->deregisterSocketEvent(d_serverSocket_p->handle(),
                                               bteso_EventType::BTESO_ACCEPT);
        }

        while (--numToCancel >= 0) {
            btesos_TcpCbAcceptor_Reg *reg = toBeCancelled[numToCancel];
            BSLS_ASSERT(reg);
            reg->invoke(-1);
            d_callbackPool.deleteObjectRaw(reg);
        }
    }
}

int btesos_TcpCbAcceptor::close()
{
    enum { SUCCESS = 0 };
    BSLS_ASSERT(NULL != d_serverSocket_p);
    BSLS_ASSERT(d_serverAddress.portNumber());  // Address is valid.
    cancelAll();
    d_factory_p->deallocate(d_serverSocket_p);
    d_serverSocket_p = NULL;
    d_serverAddress = bteso_IPv4Address();
    return SUCCESS;
}

void btesos_TcpCbAcceptor::deallocate(btesc_CbChannel *channel)
{
    BSLS_ASSERT(channel);
    channel->invalidate();
}

void btesos_TcpCbAcceptor::invalidate()
{
    d_isInvalidFlag = 1;
}

int btesos_TcpCbAcceptor::open(const bteso_IPv4Address& endpoint,
                                    int                 queueSize,
                                    int                 reuseAddress)
{
    BSLS_ASSERT(0 < queueSize);
    BSLS_ASSERT(NULL == d_serverSocket_p);

    enum {
        REUSEADDRESS_FAILED  = -6,
        ALLOCATION_FAILED    = -5,
        BIND_FAILED          = -4,
        LISTEN_FAILED        = -3,
        LOCALINFO_FAILED     = -2,
        BLOCKMODE_FAILED     = -1,
        SUCCESS              =  0
    };

    d_serverSocket_p = d_factory_p->allocate();
    if (!d_serverSocket_p) {
        return ALLOCATION_FAILED;
    }

    if (0 !=
           d_serverSocket_p->setOption(bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
                                       bteso_SocketOptUtil::BTESO_REUSEADDRESS,
                                       !!reuseAddress))
    {
        d_factory_p->deallocate(d_serverSocket_p);
        return REUSEADDRESS_FAILED;
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
#ifndef BTESO_PLATFORM__WIN_SOCKETS
    // Windows has a bug -- setting listening socket to non-blocking
    // mode will force subsequent 'accept' calls to return
    // WSAEWOULDBLOCK *even when connection is present*.

    if (0 != d_serverSocket_p->setBlockingMode(
                                         bteso_Flag::BTESO_NONBLOCKING_MODE)) {
        d_factory_p->deallocate(d_serverSocket_p);
        d_serverSocket_p = NULL;
        return BLOCKMODE_FAILED;
    }
#endif
    return SUCCESS;
}

int btesos_TcpCbAcceptor::setOption(int level, int option, int value)
{
    BSLS_ASSERT(d_serverSocket_p);
    return d_serverSocket_p->setOption(level, option, value);
}

// ACCESSORS

int btesos_TcpCbAcceptor::getOption(int *result, int level, int option) const
{
    BSLS_ASSERT(d_serverSocket_p);
    return d_serverSocket_p->socketOption(result, option, level);
}

int btesos_TcpCbAcceptor::isInvalid() const
{
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
