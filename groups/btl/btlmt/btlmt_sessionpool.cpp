// btlmt_sessionpool.cpp                                              -*-C++-*-
#include <btlmt_sessionpool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlmt_sessionpool_cpp,"$Id$ $CSID$")

#include <btlmt_channelpool.h>
#include <btlmt_channelpoolchannel.h>
#include <btlmt_session.h>

#include <btlso_socketoptions.h>

#include <bdlmca_pooledblobbufferfactory.h>
#include <bdlmtt_xxxthread.h>
#include <bdlmtt_lockguard.h>

#include <bdlf_bind.h>
#include <bdlf_function.h>
#include <bdlf_memfn.h>

#include <bdlma_bufferedsequentialallocator.h>

#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>

#include <bslalg_typetraits.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

namespace BloombergLP {

namespace btlmt {
                      // ===============================
                      // class SessionPool_Handle
                      // ===============================

struct SessionPool_Handle {
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
    bdlmtt::Mutex                             d_mutex;
    SessionPool::SessionStateCallback d_sessionStateCB;
    int                                     d_numAttemptsRemaining;
    int                                     d_handleId;
    int                                     d_type;
    void                                   *d_userData_p;
    ChannelPoolChannel               *d_channel_p;
    Session                          *d_session_p;
    SessionFactory                   *d_sessionFactory_p;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(SessionPool_Handle,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    SessionPool_Handle(bslma::Allocator *basicAllocator = 0)
    : d_sessionStateCB(basicAllocator)
    {
    }
};

                   // --------------------------------------
                   // class SessionPoolSessionIterator
                   // --------------------------------------

SessionPoolSessionIterator::SessionPoolSessionIterator(
                                                SessionPool *sessionPool)
: d_iterator(sessionPool->d_handles)
{
    if(d_iterator) {
        d_current_p = &d_current;
        bsl::pair<int, bsl::shared_ptr<SessionPool_Handle> >
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

void SessionPoolSessionIterator::operator++()
{
    if (d_current_p) {
        while (true) {
            ++d_iterator;
            if (!d_iterator) {
                d_current_p = 0;
                break;
            }
            bsl::pair<int, bsl::shared_ptr<SessionPool_Handle> >
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
}  // close package namespace

typedef bsl::shared_ptr<btlmt::SessionPool_Handle> HandlePtr;

static btlmt::ChannelPool::ConnectResolutionMode mapResolutionMode(
                               btlmt::SessionPool::ConnectResolutionMode mode)
{
    if (mode == btlmt::SessionPool::RESOLVE_AT_EACH_ATTEMPT) {
        return btlmt::ChannelPool::BTEMT_RESOLVE_AT_EACH_ATTEMPT;
    }

    BSLS_ASSERT(mode == btlmt::SessionPool::RESOLVE_ONCE);
    return btlmt::ChannelPool::BTEMT_RESOLVE_ONCE;
}

namespace btlmt {
                          // -----------------------
                          // class SessionPool
                          // -----------------------

// PRIVATE MANIPULATORS
void SessionPool::channelStateCb(int   channelId,
                                       int   sourceId,
                                       int   state,
                                       void *userData)
{
    using namespace bdlf::PlaceHolders;

    switch(state) {
      case ChannelPool::BTEMT_CHANNEL_DOWN: {
          if (0 == userData) {
              break;
          }

          SessionPool_Handle *handlePtr =
                                         (SessionPool_Handle*) userData;
          HandlePtr handle;

          if (d_handles.find(handlePtr->d_handleId, &handle)) {
              return;                                                 // RETURN
          }

          int handleId = handle->d_handleId;
          {
              bdlmtt::LockGuard<bdlmtt::Mutex> lock(&handle->d_mutex);

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

      case ChannelPool::BTEMT_CHANNEL_UP: {
          HandlePtr handle;
          if (d_handles.find(sourceId, &handle)) {
              // Handle not found, don't know this source

              d_channelPool_p->shutdown(channelId,
                                        ChannelPool::BTEMT_IMMEDIATE);
              return;
          }
          if (SessionPool_Handle::LISTENER == handle->d_type) {
              // This connection originate from a listener socket,
              // create a new handle for the new channel

              HandlePtr newHandle(
                              new (*d_allocator_p) SessionPool_Handle(),
                              bdlf::MemFnUtil::memFn(
                                             &SessionPool::handleDeleter,
                                             this),
                              d_allocator_p);

              newHandle->d_type = SessionPool_Handle::REGULAR_SESSION;
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

          bdlmtt::LockGuard<bdlmtt::Mutex> lock(&handle->d_mutex);
          if (SessionPool_Handle::ABORTED_CONNECT_SESSION ==
                                                              handle->d_type) {
              // We raced against 'closeHandle()'.

              d_channelPool_p->shutdown(channelId,
                                        ChannelPool::BTEMT_IMMEDIATE);
              return;                                                 // RETURN
          }

          d_channelPool_p->setChannelContext(channelId, handle.get());

          handle->d_channel_p = new (*d_allocator_p)
                                ChannelPoolChannel(
                                                   channelId,
                                                   d_channelPool_p,
                                                   &d_bufferChainFactory,
                                                   d_blobBufferFactory.ptr(),
                                                   &d_spAllocator,
                                                   d_allocator_p,
                                                   d_useBlobForDataReads);

          lock.release()->unlock();

          // We're binding the 'handleId' instead of the shared pointer so if
          // the channel goes down between the call to 'allocate' and the
          // callback,  'handleDeleter' is invoked and invokes sessionStateCb.
          // Note that in this case, we send 'CONNECT_ABORTED'.  We might want
          // to have a specific event for this.

          bsl::shared_ptr<AsyncChannel> channel_sp(handle,
                                                         handle->d_channel_p);
          handle->d_sessionFactory_p->allocate(
                   channel_sp,
                   bdlf::BindUtil::bind(&SessionPool::sessionAllocationCb,
                                       this, _1, _2, handle->d_handleId));
      } break;
      case ChannelPool::BTEMT_WRITE_CACHE_LOWWAT: {
          if (0 == userData) {
              break;
          }
          SessionPool_Handle *handle =
                                         (SessionPool_Handle*) userData;
          if (handle->d_session_p) {
              handle->d_sessionStateCB(WRITE_CACHE_LOWWAT, handle->d_handleId,
                                       handle->d_session_p,
                                       handle->d_userData_p);
          }
      } break;
      case ChannelPool::BTEMT_WRITE_CACHE_HIWAT: {
          if (0 == userData) {
              break;
          }
          SessionPool_Handle *handle =
              (SessionPool_Handle*)userData;
          if (handle->d_session_p) {
              handle->d_sessionStateCB(WRITE_CACHE_HIWAT,
                                       handle->d_handleId,
                                       handle->d_session_p,
                                       handle->d_userData_p);
          }
      } break;
    }
}

void SessionPool::connectAbortTimerCb(
                      const bsl::shared_ptr<SessionPool_Handle>& handle,
                      int                                              clockId)
{
    d_channelPool_p->deregisterClock(clockId);
    do {
        BSLS_ASSERT(handle.use_count() >= 1);
    } while (1 != handle.use_count());
}

void SessionPool::pooledBufferChainBasedReadCb(
                                             int                  *numConsumed,
                                             int                  *numNeeded,
                                             const DataMsg&  data,
                                             void                 *userData)
{
    HandlePtr spHandle; // Make sure that we hold a shared pointer
                        // until the callback is complete


    SessionPool_Handle *handle = (SessionPool_Handle*) userData;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!handle)
        || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!handle->d_channel_p)
        || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                 d_handles.find(handle->d_handleId, &spHandle))
        || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                       handle->d_channel_p->channelId() != data.channelId())) {

        d_channelPool_p->shutdown(data.channelId(),
                                  ChannelPool::BTEMT_IMMEDIATE);
        *numConsumed = 0;
        *numNeeded   = 1;
        return;
    }

    handle->d_channel_p->dataCb(numConsumed, numNeeded, data);
}

void SessionPool::blobBasedReadCb(int        *numNeeded,
                                        bdlmca::Blob *data,
                                        int         channelId,
                                        void       *userData)
{
    HandlePtr spHandle; // Make sure that we hold a shared pointer
                        // until the callback is complete


    SessionPool_Handle *handle = (SessionPool_Handle*) userData;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!handle)
        || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!handle->d_channel_p)
        || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                 d_handles.find(handle->d_handleId, &spHandle))
        || BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                              handle->d_channel_p->channelId() != channelId)) {

        d_channelPool_p->shutdown(channelId,
                                  ChannelPool::BTEMT_IMMEDIATE);
        *numNeeded   = 1;
        return;
    }

    handle->d_channel_p->blobBasedDataCb(numNeeded, data);
}

void SessionPool::terminateSession(SessionPool_Handle *handle)
{
    if (handle->d_session_p) {
        handle->d_session_p->stop();
        handle->d_sessionStateCB(SESSION_DOWN,
                                 handle->d_handleId,
                                 handle->d_session_p,
                                 handle->d_userData_p);
        handle->d_handleId = 0;
    }
}

void SessionPool::handleDeleter(SessionPool_Handle *handle)
{
    if (0 != handle->d_handleId ) {

        if (SessionPool_Handle::LISTENER == handle->d_type) {
            d_channelPool_p->close(handle->d_handleId);
        }
        else if (handle->d_session_p) {
            terminateSession(handle);
        }
        else if (SessionPool_Handle::CONNECT_SESSION ==
                                                              handle->d_type
              || SessionPool_Handle::ABORTED_CONNECT_SESSION ==
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

void SessionPool::poolStateCb(int state, int source, int)
{
    switch(state) {
      case PoolMsg::BTEMT_ERROR_ACCEPTING: {
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

      case PoolMsg::BTEMT_ERROR_BINDING_CLIENT_ADDR:      // FALL THROUGH
      case PoolMsg::BTEMT_ERROR_SETTING_OPTIONS:          // FALL THROUGH
      case PoolMsg::BTEMT_ERROR_CONNECTING: {
        HandlePtr handle;
        if (d_handles.find(source, &handle)) {
            return;
        }
        bdlmtt::LockGuard<bdlmtt::Mutex> lock(&handle->d_mutex);
        if (SessionPool_Handle::ABORTED_CONNECT_SESSION ==
                                                              handle->d_type) {
            return;
        }

        if (!--handle->d_numAttemptsRemaining) {
            handle->d_type = SessionPool_Handle::INVALID_SESSION;
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

      case PoolMsg::BTEMT_CHANNEL_LIMIT: {
        d_poolStateCB(SESSION_LIMIT_REACHED, 0, 0);
      } break;
    }
}

void SessionPool::init() 
{
    ChannelPoolConfiguration defaultValues;
    if (d_config.readTimeout() == defaultValues.readTimeout()) {
        // If the supplied 'config' has the default read-timeout value, then
        // disable the channel pool configuration's read timeout  Note that
        // the channel pool's read timeout events are ignored by session pool
        // - so they provide no benefit and should be disabled by default
        // (DRQS 16796796).

        d_config.setReadTimeout(0.0);
    }

    if (!d_blobBufferFactory) {
        d_blobBufferFactory.load(
                     new (*d_allocator_p) bdlmca::PooledBlobBufferFactory(
                                             d_config.maxIncomingMessageSize(),
                                             d_allocator_p), 
                     d_allocator_p);
    }

}

int SessionPool::makeConnectHandle(
               const SessionPool::SessionStateCallback&  cb,
               int                                             numAttempts,
               void                                           *userData,
               SessionFactory                           *factory) {
    HandlePtr handle(new (*d_allocator_p) SessionPool_Handle(),
                     bdlf::MemFnUtil::memFn(&SessionPool::handleDeleter,
                                           this),
                     d_allocator_p);

    handle->d_type = SessionPool_Handle::CONNECT_SESSION;
    handle->d_sessionStateCB = cb;
    handle->d_session_p = 0;
    handle->d_channel_p = 0;
    handle->d_numAttemptsRemaining = numAttempts;
    handle->d_userData_p = userData;
    handle->d_sessionFactory_p = factory;
    return (handle->d_handleId = d_handles.add(handle));
}

void SessionPool::sessionAllocationCb(int             result,
                                            Session  *session,
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
SessionPool::SessionPool(
                      const ChannelPoolConfiguration&  config,
                      SessionPoolStateCallback const&        poolStateCallback,
                      bslma::Allocator                      *allocator)
: d_handles(allocator)
, d_config(config)
, d_channelPool_p(0)
, d_poolStateCB(poolStateCallback, allocator)
, d_spAllocator(allocator)
, d_bufferChainFactory(config.maxIncomingMessageSize(), allocator)
, d_blobBufferFactory()
, d_useBlobForDataReads(false)
, d_numSessions(0)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    init();
}

SessionPool::SessionPool(
                    const ChannelPoolConfiguration&  config,
                    SessionPoolStateCallback const&        poolStateCallback,
                    bool                                   useBlobForDataReads,
                    bslma::Allocator                      *allocator)
: d_handles(allocator)
, d_config(config)
, d_channelPool_p(0)
, d_poolStateCB(poolStateCallback, allocator)
, d_spAllocator(allocator)
, d_bufferChainFactory(config.maxIncomingMessageSize(), allocator)
, d_blobBufferFactory()
, d_useBlobForDataReads(useBlobForDataReads)
, d_numSessions(0)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    init();
}

SessionPool::SessionPool(
              bdlmca::BlobBufferFactory                   *blobBufferFactory, 
              const ChannelPoolConfiguration&      config,
              const SessionPoolStateCallback&            poolStateCallback,
              bslma::Allocator                          *allocator)
: d_handles(allocator)
, d_config(config)
, d_channelPool_p(0)
, d_poolStateCB(poolStateCallback, allocator)
, d_spAllocator(allocator)
, d_bufferChainFactory(config.maxIncomingMessageSize(), allocator) //unused
, d_blobBufferFactory(blobBufferFactory,
                      0,
                      &bslma::ManagedPtrUtil::noOpDeleter)
, d_useBlobForDataReads(true)
, d_numSessions(0)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    init();
}

SessionPool::~SessionPool()
{
    stop();
    if (d_channelPool_p) {
        d_allocator_p->deleteObjectRaw(d_channelPool_p);
    }
}

// MANIPULATORS
int SessionPool::start()
{
    if (d_channelPool_p) {

        // Channel pool was previously created and then 'stop'ed.  Restart the
        // object by calling 'start' again.

        return d_channelPool_p->start();                              // RETURN
    }

    ChannelPool::ChannelStateChangeCallback channelStateFunctor(
        bdlf::MemFnUtil::memFn(&SessionPool::channelStateCb, this),
        d_allocator_p);

    ChannelPool::PoolStateChangeCallback poolStateFunctor(
        bdlf::MemFnUtil::memFn(&SessionPool::poolStateCb, this),
        d_allocator_p);
    
    if (d_useBlobForDataReads) {
        ChannelPool::BlobBasedReadCallback dataFunctor =
            bdlf::MemFnUtil::memFn(&SessionPool::blobBasedReadCb,
                                  this);
        
        d_channelPool_p = new (*d_allocator_p)
            ChannelPool(d_blobBufferFactory.ptr(),
                              channelStateFunctor,
                              dataFunctor,
                              poolStateFunctor,
                              d_config,
                              d_allocator_p);
    }
    else {
        bdlf::Function<void (*)(int *, int*, const DataMsg&, void*)>
            dataFunctor(bdlf::MemFnUtil::memFn(
                              &SessionPool::pooledBufferChainBasedReadCb,
                              this),
                        d_allocator_p);
        
        d_channelPool_p = new (*d_allocator_p)
            ChannelPool(channelStateFunctor,
                              dataFunctor,
                              poolStateFunctor,
                              d_config,
                              d_allocator_p);
    }
    return d_channelPool_p->start();
}

int SessionPool::stop()
{
    int ret = 0;
    if (d_channelPool_p) {
        ret = d_channelPool_p->stop();
    }

    const int NUM_HANDLES = 32;
    const int SIZE        = NUM_HANDLES * sizeof(HandlePtr);

    char BUFFER[SIZE];
    bdlma::BufferedSequentialAllocator bufferAllocator(BUFFER, SIZE);

    bsl::vector<HandlePtr> handles(&bufferAllocator);
    {
        bdlcc::ObjectCatalogIter<HandlePtr> itr(d_handles);

        // Move the handles to a temporary vector to avoid a potential deadlock
        // if, when destroying the handle, we invoke the session down event and
        // the client then makes a call back into the session pool which
        // requires adding/removing an item from 'd_handles'.

        handles.reserve(d_handles.length());

        while (itr) {
            handles.push_back(itr().second);
            ++itr;
        }
    }

    d_handles.removeAll();

    typedef bsl::vector<HandlePtr>::const_iterator Iter;
    for (Iter it = handles.begin(); it != handles.end(); ++it) {
        terminateSession(it->get());
    }

    return ret;
}

int SessionPool::stopAndRemoveAllSessions()
{
    int ret = 0;
    if (d_channelPool_p) {
        ret = d_channelPool_p->stopAndRemoveAllChannels();
    }

    const int NUM_HANDLES = 32;
    const int SIZE        = NUM_HANDLES * sizeof(HandlePtr);

    char BUFFER[SIZE];
    bdlma::BufferedSequentialAllocator bufferAllocator(BUFFER, SIZE);

    bsl::vector<HandlePtr> handles(&bufferAllocator);
    {
        bdlcc::ObjectCatalogIter<HandlePtr> itr(d_handles);

        // Move the handles to a temporary vector to avoid a potential deadlock
        // if, when destroying the handle, we invoke the session down event and
        // the client then makes a call back into the session pool which
        // requires adding/removing an item from 'd_handles'.

        handles.reserve(d_handles.length());

        while (itr) {
            handles.push_back(itr().second);
            ++itr;
        }
    }

    d_handles.removeAll();

    return ret;
}

int SessionPool::closeHandle(int handleId)
{
    HandlePtr handle;
    if (d_handles.find(handleId, &handle)) {
        return -1;
    }

    if(SessionPool_Handle::LISTENER == handle->d_type) {
        d_channelPool_p->close(handle->d_handleId);
        d_handles.remove(handle->d_handleId);

        // Set handleId to 0 so the deleter does not try to close() again and
        // race against the event manager thread.

        handle->d_handleId = 0;
    }
    else {
        bdlmtt::LockGuard<bdlmtt::Mutex> lock(&handle->d_mutex);
        if (handle->d_channel_p) {
            // Let channel down cleanup.

            handle->d_channel_p->close();
        }
        else {
            // No channel yet, waiting for a connect.  Simply remove the
            // handle.

            if (handle->d_type == SessionPool_Handle::CONNECT_SESSION) {

                handle->d_type =
                            SessionPool_Handle::ABORTED_CONNECT_SESSION;
            }

            // The handle needs to be destroyed in a channel pool thread.  We
            // will register a clock for this.  But the clock thread can race
            // with this thread.  To avoid this problem , we will check the
            // number of references of the handle in the 'connectAbortTimerCb'
            // before letting the clock finish.

            lock.release()->unlock();

            // Channel pool requires us to guess a unique 'clockId' to register
            // events.  So we make an educated first guess (set to 'handleId')
            // so as not to conflict with the clocks registered by other
            // handles.  But if that fails we will increment the 'clockId' by
            // '0x01000000' on each attempt.  The successful 'clockId' is
            // bound to the passed functor so that the callback can be
            // deregistered later.

            int clockId = handleId;
            int ret;
            do {
                bdlf::Function<void (*)()> fctor(
                    bdlf::BindUtil::bindA(
                        d_allocator_p,
                        bdlf::MemFnUtil::memFn(
                                       &SessionPool::connectAbortTimerCb,
                                       this),
                        handle,
                        clockId));

                ret = d_channelPool_p->registerClock(
                                    fctor,
                                    bdlt::CurrentTime::now(),
                                    bsls::TimeInterval(0,
                                                      1 * 1000 * 1000), // 1 ms
                                    clockId);

                clockId += 0x01000000;
            } while (0 != ret);
            BSLS_ASSERT(0 == ret);

            d_handles.remove(handleId);
        }
    }
    return 0;
}

int SessionPool::connect(
                int                                            *handleBuffer,
                const SessionPool::SessionStateCallback&  cb,
                const char                                     *hostname,
                int                                             port,
                int                                             numAttempts,
                const bsls::TimeInterval&                        interval,
                SessionFactory                           *factory,
                void                                           *userData,
                ConnectResolutionMode                           resolutionMode,
                const btlso::SocketOptions                      *socketOptions,
                const btlso::IPv4Address                        *localAddress)
{
    BSLS_ASSERT(d_channelPool_p);

    if (0 == d_channelPool_p->numThreads()) {
        // Going down.

        return -1;
    }

    int handleId = makeConnectHandle(cb, numAttempts, userData, factory);
    *handleBuffer = handleId;

    int ret = d_channelPool_p->connect(hostname,
                                       port,
                                       numAttempts,
                                       interval,
                                       handleId,
                                       mapResolutionMode(resolutionMode),
                                       false,
                                       ChannelPool::BTEMT_CLOSE_BOTH,
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

int SessionPool::connect(
                 int                                            *handleBuffer,
                 const SessionPool::SessionStateCallback&  cb,
                 btlso::IPv4Address const&                        endpoint,
                 int                                             numAttempts,
                 const bsls::TimeInterval&                        interval,
                 SessionFactory                           *factory,
                 void                                           *userData,
                 const btlso::SocketOptions                      *socketOptions,
                 const btlso::IPv4Address                        *localAddress)
{
    BSLS_ASSERT(d_channelPool_p);

    if (0 == d_channelPool_p->numThreads()) {
        // Going down.

        return -1;
    }

    int handleId = makeConnectHandle(cb, numAttempts, userData, factory);
    *handleBuffer = handleId;

    int ret = d_channelPool_p->connect(endpoint,
                                       numAttempts,
                                       interval,
                                       handleId,
                                       false,
                                       ChannelPool::BTEMT_CLOSE_BOTH,
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

int SessionPool::connect(
                int                                            *handleBuffer,
                const SessionPool::SessionStateCallback&  cb,
                const char                                     *hostname,
                int                                             port,
                int                                             numAttempts,
                const bsls::TimeInterval&                        interval,
                bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                                               *socket,
                SessionFactory                           *factory,
                void                                           *userData,
                ConnectResolutionMode                           resolutionMode)
{
    BSLS_ASSERT(d_channelPool_p);

    if (0 == d_channelPool_p->numThreads()) {
        // Going down.

        return -1;
    }

    int handleId = makeConnectHandle(cb, numAttempts, userData, factory);
    *handleBuffer = handleId;

    int ret = d_channelPool_p->connect(hostname,
                                       port,
                                       numAttempts,
                                       interval,
                                       handleId,
                                       socket,
                                       mapResolutionMode(resolutionMode),
                                       false,
                                       ChannelPool::BTEMT_CLOSE_BOTH);
    if (ret) {
        HandlePtr handle;
        int rc = d_handles.remove(handleId, &handle);
        BSLS_ASSERT(0 == rc);
        handle->d_handleId = 0; // Do not call back anybody
        return ret;
    }
    return 0;
}

int SessionPool::connect(
                 int                                            *handleBuffer,
                 const SessionPool::SessionStateCallback&  cb,
                 btlso::IPv4Address const&                        endpoint,
                 int                                             numAttempts,
                 const bsls::TimeInterval&                        interval,
                 bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                                                *socket,
                 SessionFactory                           *factory,
                 void                                           *userData)
{
    BSLS_ASSERT(d_channelPool_p);

    if (0 == d_channelPool_p->numThreads()) {
        // Going down.

        return -1;
    }

    int handleId = makeConnectHandle(cb, numAttempts, userData, factory);
    *handleBuffer = handleId;

    int ret = d_channelPool_p->connect(endpoint,
                                       numAttempts,
                                       interval,
                                       handleId,
                                       socket,
                                       false,
                                       ChannelPool::BTEMT_CLOSE_BOTH);
    if (ret) {
        HandlePtr handle;
        d_handles.remove(handleId, &handle);
        handle->d_handleId = 0; // Do not call back anybody
        return ret;
    }
    return 0;
}

int SessionPool::import(int *handleBuffer,
                  const SessionPool::SessionStateCallback& cb,
                  bslma::ManagedPtr<btlso::StreamSocket<btlso::IPv4Address> >
                                                               *streamSocket,
                  SessionFactory                         *sessionFactory,
                  void                                         *userData)
{
    BSLS_ASSERT(d_channelPool_p);

    HandlePtr handle(new (*d_allocator_p) SessionPool_Handle(),
                     bdlf::MemFnUtil::memFn(&SessionPool::handleDeleter,
                                           this),
                     d_allocator_p);

    handle->d_type = SessionPool_Handle::IMPORTED_SESSION;
    handle->d_sessionStateCB = cb;
    handle->d_session_p  = 0;
    handle->d_channel_p  = 0;
    handle->d_userData_p = userData;
    handle->d_sessionFactory_p = sessionFactory;
    handle->d_handleId = d_handles.add(handle);
    *handleBuffer = handle->d_handleId;

    int ret = d_channelPool_p->import(streamSocket,
                                      handle->d_handleId,
                                      false);
    if (ret) {
        d_handles.remove(handle->d_handleId);
        return ret;
    }
    return 0;
}

int SessionPool::listen(
                 int                                            *handleBuffer,
                 const SessionPool::SessionStateCallback&  cb,
                 int                                             portNumber,
                 int                                             backlog,
                 SessionFactory                           *factory,
                 void                                           *userData,
                 const btlso::SocketOptions                      *socketOptions)
{
    btlso::IPv4Address endpoint;
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

int SessionPool::listen(
                 int                                            *handleBuffer,
                 const SessionPool::SessionStateCallback&  cb,
                 int                                             portNumber,
                 int                                             backlog,
                 int                                             reuseAddress,
                 SessionFactory                           *factory,
                 void                                           *userData,
                 const btlso::SocketOptions                      *socketOptions)
{
    btlso::IPv4Address endpoint;
    endpoint.setPortNumber(portNumber);
    return listen(handleBuffer,
                  cb,
                  endpoint,
                  backlog,
                  reuseAddress,
                  factory,
                  userData,
                  socketOptions);
}

int SessionPool::listen(
                 int                                            *handleBuffer,
                 const SessionPool::SessionStateCallback&  cb,
                 const btlso::IPv4Address&                        endpoint,
                 int                                             backlog,
                 SessionFactory                           *factory,
                 void                                           *userData,
                 const btlso::SocketOptions                      *socketOptions)
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

int SessionPool::listen(
                 int                                            *handleBuffer,
                 const SessionPool::SessionStateCallback&  cb,
                 const btlso::IPv4Address&                        endpoint,
                 int                                             backlog,
                 int                                             reuseAddress,
                 SessionFactory                           *factory,
                 void                                           *userData,
                 const btlso::SocketOptions                      *socketOptions)
{
    BSLS_ASSERT(d_channelPool_p);

    HandlePtr handle(new (*d_allocator_p) SessionPool_Handle(),
                     bdlf::MemFnUtil::memFn(&SessionPool::handleDeleter,
                                           this),
                     d_allocator_p);

    handle->d_type = SessionPool_Handle::LISTENER;
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

int SessionPool::setWriteCacheWatermarks(int handleId,
                                               int lowWatermark,
                                               int hiWatermark)
{
    BSLS_ASSERT(0 <= lowWatermark);
    BSLS_ASSERT(lowWatermark <= hiWatermark);

    HandlePtr handle;
    if (d_handles.find(handleId, &handle)) {
        return -1;
    }

    ChannelPoolChannel *channelPtr = handle->d_channel_p;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!channelPtr)) {
        return -1;
    }
    else {
        return d_channelPool_p->setWriteCacheWatermarks(
                                                       channelPtr->channelId(),
                                                       lowWatermark,
                                                       hiWatermark);
    }
}

// ACCESSORS
int SessionPool::portNumber(int handle) const
{
    const btlso::IPv4Address *address = d_channelPool_p->serverAddress(handle);
    if (address) {
        return address->portNumber();
    }
    return -1;
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
