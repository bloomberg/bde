// btemt_sessionpool.cpp                                              -*-C++-*-
#include <btemt_sessionpool.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_sessionpool_cpp,"$Id$ $CSID$")

#include <btemt_channelpool.h>
#include <btemt_channelpoolchannel.h>
#include <btemt_session.h>

#include <bteso_socketoptions.h>

#include <bcemt_thread.h>
#include <bcemt_lockguard.h>

#include <bdef_bind.h>
#include <bdef_function.h>
#include <bdef_memfn.h>

#include <bdema_bufferedsequentialallocator.h>

#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bslalg_typetraits.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

namespace BloombergLP {

                      // ===============================
                      // class btemt_SessionPool__Handle
                      // ===============================

struct btemt_SessionPool__Handle {
    // This opaque handle is used privately in this implementation.

    // PRIVATE TYPES
    enum Type {
        LISTENER = 1,
        REGULAR_SESSION = 2,
        CONNECT_SESSION = 3,
        IMPORTED_SESSION = 4,
        INVALID_SESSION = 5,
        ABORTED_CONNECT_SESSION = 6
    };

    // DATA
    bcemt_Mutex                             d_mutex;
    btemt_SessionPool::SessionStateCallback d_sessionStateCB;
    int                                     d_numAttemptsRemaining;
    int                                     d_handleId;
    int                                     d_type;
    void                                   *d_userData_p;
    btemt_ChannelPoolChannel               *d_channel_p;
    btemt_Session                          *d_session_p;
    btemt_SessionFactory                   *d_sessionFactory_p;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(btemt_SessionPool__Handle,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    btemt_SessionPool__Handle(bslma_Allocator *basicAllocator = 0)
    : d_sessionStateCB(basicAllocator)
    {
    }
};

                   // --------------------------------------
                   // class btemt_SessionPoolSessionIterator
                   // --------------------------------------

btemt_SessionPoolSessionIterator::btemt_SessionPoolSessionIterator(
                                                btemt_SessionPool *sessionPool)
: d_iterator(sessionPool->d_handles)
{
    if(d_iterator) {
        d_current_p = &d_current;
        bsl::pair<int, bcema_SharedPtr<btemt_SessionPool__Handle> >
                                                            curr(d_iterator());
        d_current.second = curr.second->d_session_p;
        d_current.first = curr.first;
        if (!d_current.second) {
            operator ++();
        }
    }
    else {
        d_current_p = 0;
    }
}

void btemt_SessionPoolSessionIterator::operator++()
{
    if (d_current_p) {
        while (true) {
            ++d_iterator;
            if (!d_iterator) {
                d_current_p = 0;
                break;
            }
            bsl::pair<int, bcema_SharedPtr<btemt_SessionPool__Handle> >
                                                            curr(d_iterator());
            d_current.second = curr.second->d_session_p;
            if (!d_current.second) {
                continue;
            }
            d_current.first = curr.first;
            break;
        }
    }
}

typedef bcema_SharedPtr<btemt_SessionPool__Handle> HandlePtr;

                          // -----------------------
                          // class btemt_SessionPool
                          // -----------------------

// PRIVATE MANIPULATORS
void btemt_SessionPool::channelStateCb(int   channelId,
                                       int   sourceId,
                                       int   state,
                                       void *userData)
{
    using namespace bdef_PlaceHolders;

    switch(state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
          if (0 == userData) {
              break;
          }

          btemt_SessionPool__Handle *handlePtr =
                                         (btemt_SessionPool__Handle*) userData;
          HandlePtr handle;

          if (d_handles.find(handlePtr->d_handleId, &handle)) {
              return;
          }

          int handleId = handle->d_handleId;
          {
              bcemt_LockGuard<bcemt_Mutex> lock(&handle->d_mutex);

              if (handle->d_session_p) {
                  int handleId = handle->d_handleId;

                  handle->d_handleId = 0; // Zero out the handleId to indicate
                                          // that SESSION_DOWN has been invoked

                  lock.release()->unlock();

                  handle->d_session_p->stop();
                  handle->d_sessionStateCB(SESSION_DOWN, handleId,
                                           handle->d_session_p,
                                           handle->d_userData_p);
              }
          }
          d_handles.remove(handleId);
      } break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
          HandlePtr handle;
          if (d_handles.find(sourceId, &handle)) {
              // Handle not found, don't know this source

              d_channelPool_p->shutdown(channelId,
                                        btemt_ChannelPool::BTEMT_IMMEDIATE);
              return;
          }
          if (btemt_SessionPool__Handle::LISTENER == handle->d_type) {
              // This connection originate from a listener socket,
              // create a new handle for the new channel

              HandlePtr newHandle(
                              new (*d_allocator_p) btemt_SessionPool__Handle(),
                              bdef_MemFnUtil::memFn(
                                             &btemt_SessionPool::handleDeleter,
                                             this),
                              d_allocator_p);

              newHandle->d_type = btemt_SessionPool__Handle::REGULAR_SESSION;
              newHandle->d_sessionStateCB = handle->d_sessionStateCB;
              newHandle->d_userData_p = handle->d_userData_p;
              newHandle->d_channel_p  = 0;
              newHandle->d_session_p  = 0;
              newHandle->d_sessionFactory_p = handle->d_sessionFactory_p;

              newHandle->d_handleId = d_handles.add(newHandle);
              handle.swap(newHandle);
          }

          // It is important to set the channel context before any calls to the
          // session state callback can be made.  Otherwise if a session
          // becomes up and dies right away, it is possible that the
          // CHANNEL_DOWN will be received with a NULL userData and thus will
          // ignored.

          bcemt_LockGuard<bcemt_Mutex> lock(&handle->d_mutex);
          if (btemt_SessionPool__Handle::ABORTED_CONNECT_SESSION ==
                                                              handle->d_type) {
              // We raced against 'closeHandle()'.

              d_channelPool_p->shutdown(channelId,
                                        btemt_ChannelPool::BTEMT_IMMEDIATE);
              return;
          }
          d_channelPool_p->setChannelContext(channelId, handle.ptr());

          handle->d_channel_p = new (*d_allocator_p)
                                btemt_ChannelPoolChannel(channelId,
                                                        d_channelPool_p,
                                                        &d_bufferChainFactory,
                                                        &d_spAllocator,
                                                        d_allocator_p,
                                                        &d_blobBufferFactory,
                                                        d_useBlobForDataReads);

          lock.release()->unlock();

          // We're binding the 'handleId' instead of the shared pointer so if
          // the channel goes down between the call to 'allocate' and the
          // callback,  'handleDeleter' is invoked and invokes sessionStateCb.
          // Note that in this case, we send 'CONNECT_ABORTED'.  We might want
          // to have a specific event for this.

          handle->d_sessionFactory_p->allocate(
                   handle->d_channel_p,
                   bdef_BindUtil::bind(&btemt_SessionPool::sessionAllocationCb,
                                       this, _1, _2, handle->d_handleId));
      } break;
      case btemt_ChannelPool::BTEMT_WRITE_CACHE_LOWWAT: {
          if (0 == userData) {
              break;
          }
          btemt_SessionPool__Handle *handle =
                                         (btemt_SessionPool__Handle*) userData;
          if (handle->d_session_p) {
              handle->d_sessionStateCB(WRITE_CACHE_LOWWAT, handle->d_handleId,
                                       handle->d_session_p,
                                       handle->d_userData_p);
          }
      } break;
      case btemt_ChannelPool::BTEMT_WRITE_CACHE_HIWAT: {
          if (0 == userData) {
              break;
          }
          btemt_SessionPool__Handle *handle =
              (btemt_SessionPool__Handle*)userData;
          if (handle->d_session_p) {
              handle->d_sessionStateCB(WRITE_CACHE_HIWAT,
                                       handle->d_handleId,
                                       handle->d_session_p,
                                       handle->d_userData_p);
          }
      } break;
    }
}

void btemt_SessionPool::connectAbortTimerCb(
                     const bcema_SharedPtr<btemt_SessionPool__Handle>& handle)
{
    d_channelPool_p->deregisterClock(handle->d_handleId);
    do {
        BSLS_ASSERT(handle.numReferences() >= 1);
    } while (1 != handle.numReferences());
}

void btemt_SessionPool::pooledBufferChainBasedReadCb(
                                             int                  *numConsumed,
                                             int                  *numNeeded,
                                             const btemt_DataMsg&  data,
                                             void                 *userData)
{
    HandlePtr spHandle; // Make sure that we hold a shared pointer
                        // until the callback is complete


    btemt_SessionPool__Handle *handle = (btemt_SessionPool__Handle*) userData;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!handle)
        || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!handle->d_channel_p)
        || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                 d_handles.find(handle->d_handleId, &spHandle))
        || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                       handle->d_channel_p->channelId() != data.channelId())) {

        d_channelPool_p->shutdown(data.channelId(),
                                  btemt_ChannelPool::BTEMT_IMMEDIATE);
        *numConsumed = 0;
        *numNeeded   = 1;
        return;
    }

    handle->d_channel_p->dataCb(numConsumed, numNeeded, data);
}

void btemt_SessionPool::blobBasedReadCb(int        *numNeeded,
                                        bcema_Blob *data,
                                        int         channelId,
                                        void       *userData)
{
    HandlePtr spHandle; // Make sure that we hold a shared pointer
                        // until the callback is complete


    btemt_SessionPool__Handle *handle = (btemt_SessionPool__Handle*) userData;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!handle)
        || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!handle->d_channel_p)
        || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                 d_handles.find(handle->d_handleId, &spHandle))
        || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                              handle->d_channel_p->channelId() != channelId)) {

        d_channelPool_p->shutdown(channelId,
                                  btemt_ChannelPool::BTEMT_IMMEDIATE);
        *numNeeded   = 1;
        return;
    }

    handle->d_channel_p->blobBasedDataCb(numNeeded, data);
}

void btemt_SessionPool::handleDeleter(btemt_SessionPool__Handle *handle)
{
    if (0 != handle->d_handleId ) {

        if (btemt_SessionPool__Handle::LISTENER == handle->d_type) {
            d_channelPool_p->close(handle->d_handleId);
        }
        else if (handle->d_session_p) {
            handle->d_session_p->stop();
            handle->d_sessionStateCB(SESSION_DOWN,
                                     handle->d_handleId,
                                     handle->d_session_p,
                                     handle->d_userData_p);
            handle->d_handleId = 0;
        }
        else if (btemt_SessionPool__Handle::CONNECT_SESSION ==
                                                              handle->d_type
              || btemt_SessionPool__Handle::ABORTED_CONNECT_SESSION ==
                                                              handle->d_type) {
            handle->d_sessionStateCB(CONNECT_ABORTED, handle->d_handleId, 0,
                                     handle->d_userData_p);
            d_poolStateCB(CONNECT_ABORTED, 0, handle->d_userData_p);
        }
    }

    if (handle->d_channel_p) {
        handle->d_channel_p->cancelRead();
        handle->d_channel_p->close();
    }

    if (handle->d_session_p) {
        --d_numSessions;
        handle->d_sessionFactory_p->deallocate(handle->d_session_p);
    }

    if (handle->d_channel_p) {
        d_allocator_p->deleteObjectRaw(handle->d_channel_p);
    }

    handle->d_session_p = 0;
    handle->d_channel_p = 0;

    d_allocator_p->deleteObjectRaw(handle);
}

void btemt_SessionPool::poolStateCb(int state, int source, int)
{
    switch(state) {
      case btemt_PoolMsg::BTEMT_ERROR_ACCEPTING: {
        HandlePtr handle;
        if (d_handles.find(source, &handle)) {
            return;
        }

        // ACCEPT_FAILED is forwarded to both callbacks.  So we can move away
        // from the poolStateCb.

        handle->d_sessionStateCB(ACCEPT_FAILED,
                                 handle->d_handleId,
                                 0,
                                 handle->d_userData_p);
        d_poolStateCB(ACCEPT_FAILED, source, handle->d_userData_p);
      } break;

      case btemt_PoolMsg::BTEMT_ERROR_CONNECTING: {
        HandlePtr handle;
        if (d_handles.find(source, &handle)) {
            return;
        }
        bcemt_LockGuard<bcemt_Mutex> lock(&handle->d_mutex);
        if (btemt_SessionPool__Handle::ABORTED_CONNECT_SESSION ==
                                                              handle->d_type) {
            return;
        }

        if (!--handle->d_numAttemptsRemaining) {
            handle->d_type = btemt_SessionPool__Handle::INVALID_SESSION;
            lock.release()->unlock();
            handle->d_sessionStateCB(CONNECT_FAILED, handle->d_handleId, 0,
                                     handle->d_userData_p);

            d_poolStateCB(CONNECT_FAILED, source, handle->d_userData_p);
            d_handles.remove(source);
        }
        else {
            lock.release()->unlock();

            // Note that it is important to keep a reference to the handle so
            // any of this callback will be invoked after the abort message if
            // closeHandle was called for this pending connect.

            handle->d_sessionStateCB(CONNECT_ATTEMPT_FAILED,
                                     handle->d_handleId, 0,
                                     handle->d_userData_p);
            d_poolStateCB(CONNECT_ATTEMPT_FAILED, source,
                          handle->d_userData_p);
        }
      } break;

      case btemt_PoolMsg::BTEMT_CHANNEL_LIMIT: {
        d_poolStateCB(SESSION_LIMIT_REACHED, 0, 0);
      } break;
    }
}

void btemt_SessionPool::sessionAllocationCb(int             result,
                                            btemt_Session  *session,
                                            int             handleId)
{
    HandlePtr handle;
    if (d_handles.find(handleId, &handle)) {
        // The handle already went away.  For example if CHANNEL_DOWN is
        // received before the call to this callback and the call to
        // 'allocate'.

        return;
    }

    if (result) {
        handle->d_sessionStateCB(
                                 SESSION_ALLOC_FAILED,
                                 handle->d_handleId,
                                 0,
                                 handle->d_userData_p);

        // Failed to allocate session

        handle->d_channel_p->close();
        return;
    }

    // Start the session

    if (session->start()) {
        handle->d_sessionStateCB(SESSION_STARTUP_FAILED, handle->d_handleId,
                                 session, handle->d_userData_p);
        handle->d_sessionFactory_p->deallocate(session);

        // Session failed to start, shutdown the channel.

        if (handle->d_channel_p) {
            handle->d_channel_p->close();
        }
        return;
    }

    ++d_numSessions;
    handle->d_session_p = session;
    handle->d_sessionStateCB(SESSION_UP, handle->d_handleId, session,
                             handle->d_userData_p);
}

// CREATORS
btemt_SessionPool::btemt_SessionPool(
                      const btemt_ChannelPoolConfiguration&  config,
                      SessionPoolStateCallback const&        poolStateCallback,
                      bslma_Allocator                       *allocator)
: d_handles(allocator)
, d_config(config)
, d_channelPool_p(0)
, d_poolStateCB(poolStateCallback, allocator)
, d_spAllocator(allocator)
, d_bufferChainFactory(config.maxIncomingMessageSize(), allocator)
, d_blobBufferFactory(config.maxIncomingMessageSize(), allocator)
, d_useBlobForDataReads(false)
, d_numSessions(0)
, d_allocator_p(bslma_Default::allocator(allocator))
{
    btemt_ChannelPoolConfiguration defaultValues;
    if (d_config.readTimeout() == defaultValues.readTimeout()) {
        // If the supplied 'config' has the default read-timeout value, then
        // disable the channel pool configuration's read timeout  Note that
        // the channel pool's read timeout events are ignored by session pool
        // - so they provide no benefit and should be disabled by default
        // (DRQS 16796796).
        d_config.setReadTimeout(0.0);
    }
}

btemt_SessionPool::btemt_SessionPool(
                    const btemt_ChannelPoolConfiguration&  config,
                    SessionPoolStateCallback const&        poolStateCallback,
                    bool                                   useBlobForDataReads,
                    bslma_Allocator                       *allocator)
: d_handles(allocator)
, d_config(config)
, d_channelPool_p(0)
, d_poolStateCB(poolStateCallback, allocator)
, d_spAllocator(allocator)
, d_bufferChainFactory(config.maxIncomingMessageSize(), allocator)
, d_blobBufferFactory(config.maxIncomingMessageSize(), allocator)
, d_useBlobForDataReads(useBlobForDataReads)
, d_numSessions(0)
, d_allocator_p(bslma_Default::allocator(allocator))
{
    btemt_ChannelPoolConfiguration defaultValues;
    if (d_config.readTimeout() == defaultValues.readTimeout()) {
        // If the supplied 'config' has the default read-timeout value, then
        // disable the channel pool configuration's read timeout  Note that
        // the channel pool's read timeout events are ignored by session pool
        // - so they provide no benefit and should be disabled by default
        // (DRQS 16796796).
        d_config.setReadTimeout(0.0);
    }
}

btemt_SessionPool::~btemt_SessionPool()
{
    stop();
    if (d_channelPool_p) {
        d_allocator_p->deleteObjectRaw(d_channelPool_p);
    }
}

// MANIPULATORS
int btemt_SessionPool::start()
{
    BSLS_ASSERT(!d_channelPool_p);

    btemt_ChannelPool::ChannelStateChangeCallback channelStateFunctor(
               bdef_MemFnUtil::memFn(&btemt_SessionPool::channelStateCb, this),
               d_allocator_p);

    btemt_ChannelPool::PoolStateChangeCallback poolStateFunctor(
                  bdef_MemFnUtil::memFn(&btemt_SessionPool::poolStateCb, this),
                  d_allocator_p);

    if (d_useBlobForDataReads) {
        btemt_ChannelPool::BlobBasedReadCallback dataFunctor =
                     bdef_MemFnUtil::memFn(&btemt_SessionPool::blobBasedReadCb,
                                           this);

        d_channelPool_p = new (*d_allocator_p)
                                         btemt_ChannelPool(channelStateFunctor,
                                                           dataFunctor,
                                                           poolStateFunctor,
                                                           d_config,
                                                           d_allocator_p);
    }
    else {
        bdef_Function<void (*)(int *, int*, const btemt_DataMsg&, void*)>
            dataFunctor(
                    bdef_MemFnUtil::memFn(
                              &btemt_SessionPool::pooledBufferChainBasedReadCb,
                              this),
                    d_allocator_p);

        d_channelPool_p = new (*d_allocator_p)
                                         btemt_ChannelPool(channelStateFunctor,
                                                           dataFunctor,
                                                           poolStateFunctor,
                                                           d_config,
                                                           d_allocator_p);
    }
    return d_channelPool_p->start();
}

int btemt_SessionPool::stop()
{
    int ret = 0;
    if (d_channelPool_p) {
        ret = d_channelPool_p->stop();
    }

    const int NUM_HANDLES = 32;
    const int SIZE        = NUM_HANDLES * sizeof(HandlePtr);

    char BUFFER[SIZE];
    bdema_BufferedSequentialAllocator bufferAllocator(BUFFER, SIZE);

    bsl::vector<HandlePtr> handles(&bufferAllocator);
    {
        bcec_ObjectCatalogIter<HandlePtr> itr(d_handles);

        // Move the handles to a temporary vector to avoid a potential
        // deadlock if when destroying the handle, we invoke the session
        // down event and the client then makes a call into back into
        // the session pool which requires adding/removing an item from
        // d_handles.

        handles.reserve(d_handles.length());

        while (itr) {
            handles.push_back(itr().second);
            ++itr;
        }
    }

    d_handles.removeAll();

    return ret;
}

int btemt_SessionPool::closeHandle(int handleId)
{
    HandlePtr handle;
    if (d_handles.find(handleId, &handle)) {
        return -1;
    }

    if(btemt_SessionPool__Handle::LISTENER == handle->d_type) {
        d_channelPool_p->close(handle->d_handleId);
        d_handles.remove(handle->d_handleId);

        // Set handleId to 0 so the deleter does not try to close() again and
        // race against the event manager thread.

        handle->d_handleId = 0;
    }
    else {
        bcemt_LockGuard<bcemt_Mutex> lock(&handle->d_mutex);
        if (handle->d_channel_p) {
            // Let channel down cleanup.

            handle->d_channel_p->close();
        }
        else {
            // No channel yet, waiting for a connect.  Simply removing the
            // handler.
            // The session might have become invalid at this point.  In this
            // case, remove will fail but still hold a reference to the handle
            // and we want to make sure that the deleter will not be run in
            // this thread.  At this point, it is not completely necessary but
            // in the future we might want to call a callback in
            // 'handleDeleter' when the session became invalid.

            if (handle->d_type == btemt_SessionPool__Handle::CONNECT_SESSION) {

                handle->d_type =
                            btemt_SessionPool__Handle::ABORTED_CONNECT_SESSION;
            }

            lock.release()->unlock();
            d_handles.remove(handleId);

            // The handle needs to be destroyed in a channel pool thread.  We
            // will register a clock for this.  But the clock thread can race
            // with this thread.
            // To avoid this problem , we will check the number of references
            // of the handle before letting the clock finish.  Since the bcef
            // functors are reference-counted (so copies of the functor do not
            // increase the ref count) , we need to make sure that the functor
            // we're about to create will go out of scope before the clock
            // finishes (which will destroy 'fctor' and the handle).  To
            // achieve this here, we are holding another reference ('handle')
            // to the handle and will go out of scope *after* 'fctor'.  That
            // way, the only copy of 'fctor' that will exist will be in the
            // timer queue.  We then can rely on the reference count of the
            // shared pointer.

            bdef_Function<void (*)()> fctor(
                    bdef_BindUtil::bindA(
                        d_allocator_p,
                        bdef_MemFnUtil::memFn(
                                       &btemt_SessionPool::connectAbortTimerCb,
                                       this),
                        handle));

            int ret = d_channelPool_p->registerClock(
                                    fctor,
                                    bdetu_SystemTime::now(),
                                    bdet_TimeInterval(0,
                                                      1 * 1000 * 1000), // 1 ms
                                    handleId);
            BSLS_ASSERT(0 == ret);
        }
    }
    return 0;
}

int btemt_SessionPool::connect(
                int                                            *handleBuffer,
                const btemt_SessionPool::SessionStateCallback&  cb,
                const char                                     *hostname,
                int                                             port,
                int                                             numAttempts,
                const bdet_TimeInterval&                        interval,
                btemt_SessionFactory                           *factory,
                void                                           *userData,
                ConnectResolutionMode                           resolutionMode,
                const bteso_SocketOptions                      *socketOptions,
                const bteso_IPv4Address                        *localAddress)
{
    BSLS_ASSERT(d_channelPool_p);

    if (0 == d_channelPool_p->numThreads()) {
        // Going down.

        return -1;
    }

    int handleId;
    {
        HandlePtr handle(new(*d_allocator_p) btemt_SessionPool__Handle(),
                       bdef_MemFnUtil::memFn(&btemt_SessionPool::handleDeleter,
                                             this),
                         d_allocator_p);
        handle->d_type = btemt_SessionPool__Handle::CONNECT_SESSION;
        handle->d_sessionStateCB = cb;
        handle->d_session_p = 0;
        handle->d_channel_p = 0;
        handle->d_numAttemptsRemaining = numAttempts;
        handle->d_userData_p = userData;
        handle->d_sessionFactory_p = factory;
        handle->d_handleId = d_handles.add(handle);
        handleId = handle->d_handleId;
    }
    *handleBuffer = handleId;

    btemt_ChannelPool::ConnectResolutionMode cpResolutionMode;
    if (resolutionMode == RESOLVE_AT_EACH_ATTEMPT) {
        cpResolutionMode = btemt_ChannelPool::BTEMT_RESOLVE_AT_EACH_ATTEMPT;
    } else {
        BSLS_ASSERT(resolutionMode == RESOLVE_ONCE);
        cpResolutionMode = btemt_ChannelPool::BTEMT_RESOLVE_ONCE;
    }

    int ret = d_channelPool_p->connect(hostname,
                                       port,
                                       numAttempts,
                                       interval,
                                       handleId,
                                       cpResolutionMode,
                                       false,
                                       btemt_ChannelPool::BTEMT_CLOSE_BOTH,
                                       socketOptions,
                                       localAddress);
    if (ret) {
        HandlePtr handle;
        int rc = d_handles.remove(handleId, &handle);
        BSLS_ASSERT(0 == rc);
        handle->d_handleId = 0; // Do not call back anybody
        return ret;
    }
    return 0;
}

int btemt_SessionPool::connect(
                 int                                            *handleBuffer,
                 const btemt_SessionPool::SessionStateCallback&  cb,
                 bteso_IPv4Address const&                        endpoint,
                 int                                             numAttempts,
                 const bdet_TimeInterval&                        interval,
                 btemt_SessionFactory                           *factory,
                 void                                           *userData,
                 const bteso_SocketOptions                      *socketOptions,
                 const bteso_IPv4Address                        *localAddress)
{
    BSLS_ASSERT(d_channelPool_p);

    if (0 == d_channelPool_p->numThreads()) {
        // Going down.

        return -1;
    }

    int handleId;
    {
        HandlePtr handle(new (*d_allocator_p) btemt_SessionPool__Handle(),
                       bdef_MemFnUtil::memFn(&btemt_SessionPool::handleDeleter,
                                             this),
                       d_allocator_p);

        handle->d_type = btemt_SessionPool__Handle::CONNECT_SESSION;
        handle->d_sessionStateCB = cb;
        handle->d_session_p = 0;
        handle->d_channel_p = 0;
        handle->d_numAttemptsRemaining = numAttempts;
        handle->d_userData_p = userData;
        handle->d_sessionFactory_p = factory;
        handle->d_handleId = d_handles.add(handle);
        handleId = handle->d_handleId;
    }
    *handleBuffer = handleId;

    int ret = d_channelPool_p->connect(endpoint,
                                       numAttempts,
                                       interval,
                                       handleId,
                                       false,
                                       btemt_ChannelPool::BTEMT_CLOSE_BOTH,
                                       socketOptions,
                                       localAddress);
    if (ret) {
        HandlePtr handle;
        d_handles.remove(handleId, &handle);
        handle->d_handleId = 0; // Do not call back anybody
        return ret;
    }
    return 0;
}

int btemt_SessionPool::import(int *handleBuffer,
                  const btemt_SessionPool::SessionStateCallback& cb,
                  bteso_StreamSocket<bteso_IPv4Address>        *streamSocket,
                  bteso_StreamSocketFactory<bteso_IPv4Address> *socketFactory,
                  btemt_SessionFactory                         *sessionFactory,
                  void                                         *userData)
{
    BSLS_ASSERT(d_channelPool_p);

    HandlePtr handle(new (*d_allocator_p) btemt_SessionPool__Handle(),
                     bdef_MemFnUtil::memFn(&btemt_SessionPool::handleDeleter,
                                           this),
                     d_allocator_p);

    handle->d_type = btemt_SessionPool__Handle::IMPORTED_SESSION;
    handle->d_sessionStateCB = cb;
    handle->d_session_p  = 0;
    handle->d_channel_p  = 0;
    handle->d_userData_p = userData;
    handle->d_sessionFactory_p = sessionFactory;
    handle->d_handleId = d_handles.add(handle);
    *handleBuffer = handle->d_handleId;

    int ret = d_channelPool_p->import(streamSocket,
                                      socketFactory,
                                      handle->d_handleId,
                                      false);
    if (ret) {
        d_handles.remove(handle->d_handleId);
        return ret;
    }
    return 0;
}

int btemt_SessionPool::listen(
                 int                                            *handleBuffer,
                 const btemt_SessionPool::SessionStateCallback&  cb,
                 int                                             portNumber,
                 int                                             backlog,
                 btemt_SessionFactory                           *factory,
                 void                                           *userData,
                 const bteso_SocketOptions                      *socketOptions)
{
    bteso_IPv4Address endpoint;
    endpoint.setPortNumber(portNumber);
    return listen(handleBuffer,
                  cb,
                  endpoint,
                  backlog,
                  1,
                  factory,
                  userData,
                  socketOptions);
}

int btemt_SessionPool::listen(
                 int                                            *handleBuffer,
                 const btemt_SessionPool::SessionStateCallback&  cb,
                 int                                             portNumber,
                 int                                             backlog,
                 int                                             ,
                 btemt_SessionFactory                           *factory,
                 void                                           *userData,
                 const bteso_SocketOptions                      *socketOptions)
{
    bteso_IPv4Address endpoint;
    endpoint.setPortNumber(portNumber);
    return listen(handleBuffer,
                  cb,
                  endpoint,
                  backlog,
                  1,
                  factory,
                  userData,
                  socketOptions);
}

int btemt_SessionPool::listen(
                 int                                            *handleBuffer,
                 const btemt_SessionPool::SessionStateCallback&  cb,
                 const bteso_IPv4Address&                        endpoint,
                 int                                             backlog,
                 btemt_SessionFactory                           *factory,
                 void                                           *userData,
                 const bteso_SocketOptions                      *socketOptions)
{
    return listen(handleBuffer,
                  cb,
                  endpoint,
                  backlog,
                  1,
                  factory,
                  userData,
                  socketOptions);
}

int btemt_SessionPool::listen(
                 int                                            *handleBuffer,
                 const btemt_SessionPool::SessionStateCallback&  cb,
                 const bteso_IPv4Address&                        endpoint,
                 int                                             backlog,
                 int                                             reuseAddress,
                 btemt_SessionFactory                           *factory,
                 void                                           *userData,
                 const bteso_SocketOptions                      *socketOptions)
{
    BSLS_ASSERT(d_channelPool_p);

    HandlePtr handle(new (*d_allocator_p) btemt_SessionPool__Handle(),
                     bdef_MemFnUtil::memFn(&btemt_SessionPool::handleDeleter,
                                           this),
                     d_allocator_p);

    handle->d_type = btemt_SessionPool__Handle::LISTENER;
    handle->d_sessionStateCB = cb;
    handle->d_session_p  = 0;
    handle->d_channel_p  = 0;
    handle->d_userData_p = userData;
    handle->d_sessionFactory_p = factory;
    handle->d_handleId = d_handles.add(handle);
    *handleBuffer = handle->d_handleId;

    int ret = d_channelPool_p->listen(endpoint,
                                      backlog,
                                      handle->d_handleId,
                                      reuseAddress,
                                      false,
                                      socketOptions);

    if (ret) {
        d_handles.remove(handle->d_handleId);
        return ret;
    }
    return 0;
}

// ACCESSORS
int btemt_SessionPool::portNumber(int handle) const
{
    const bteso_IPv4Address *address = d_channelPool_p->serverAddress(handle);
    if (address) {
        return address->portNumber();
    }
    return -1;
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
