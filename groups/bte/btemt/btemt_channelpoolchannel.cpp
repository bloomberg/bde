// btemt_channelpoolchannel.cpp    -*-C++-*-
#include <btemt_channelpoolchannel.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <btemt_channelpool.h>

#include <bcema_blob.h>
#include <bcema_blobutil.h>
#include <bcema_pooledbufferchain.h>
#include <bcema_poolallocator.h>
#include <bcemt_lockguard.h>

#include <bdema_bufferedsequentialallocator.h>

#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_memfn.h>

#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

namespace BloombergLP {

                       // ------------------------------
                       // class btemt_ChannelPoolChannel
                       // ------------------------------

// PRIVATE MANIPULATORS
template <typename CALLBCK>
void btemt_ChannelPoolChannel::assignCallback(
                                    ReadQueueEntry::ReadDataCallback& cbEntry,
                                    const CALLBCK&                    callback,
                                    const bslmf_MetaInt<BTEMT_BLOB_BASED>&)
{
    bsls_ObjectBuffer<BlobBasedReadCallback>& callbackBuffer =
                                                         cbEntry.d_blobBasedCb;
    new (callbackBuffer.buffer()) BlobBasedReadCallback();
    callbackBuffer.object() = callback;
}

template <typename CALLBCK>
void btemt_ChannelPoolChannel::assignCallback(
                                    ReadQueueEntry::ReadDataCallback& cbEntry,
                                    const CALLBCK&                    callback,
                                    const bslmf_MetaInt<BTEMT_DATAMSG_BASED>&)
{
    bsls_ObjectBuffer<ReadCallback>& callbackBuffer =
                                            cbEntry.d_pooledBufferChainBasedCb;
    new (callbackBuffer.buffer()) ReadCallback();
    callbackBuffer.object() = callback;
}

template <typename CALLBCK, int CALLBACK_TYPE>
int btemt_ChannelPoolChannel::addReadQueueEntry(
                                             int                      numBytes,
                                             const CALLBCK&          callback,
                                             const bdet_TimeInterval& timeOut)
{
    if (d_closed) {
        return -2;
    }

    BSLS_ASSERT(0 < numBytes);

    d_readQueue.push_back(ReadQueueEntry());
    ReadQueueEntry& entry = d_readQueue.back();

    entry.d_numBytesNeeded = numBytes;
    entry.d_timeOut        = timeOut;
    entry.d_timeOutTimerId = 0;
    entry.d_progress       = btemt_AsyncChannel::BTEMT_SUCCESS;
    entry.d_callbackType   = (CallbackType) CALLBACK_TYPE;

    assignCallback(entry.d_readCallback,
                   callback,
                   bslmf_MetaInt<CALLBACK_TYPE>());

    if (0 != timeOut.totalMicroseconds()) {
        registerTimeoutAndUpdateClockId(timeOut);
        entry.d_timeOutTimerId = d_nextClockId;
    }

    if (1 == d_readQueue.size()) {
        d_channelPool_p->enableRead(d_channelId);
    }
    return 0;
}

void btemt_ChannelPoolChannel::registerTimeoutAndUpdateClockId(
                                              const bdet_TimeInterval& timeOut)
{
    // This interface stinks: we have to guess a clockId that will not compete
    // with other clients of the channelpool!  Instead, channelpool should
    // return us a unique clockId.  We make an educated first guess (set to
    // 'channelId + 0x00800000' in the ctor so as not to conflict with the
    // handles used as clockId in the session pool) and increment it if it
    // happens to be a duplicate.  With current usage in session pool, a
    // single iteration through the loop (i.e., no loop at all) should be
    // guaranteed.  Note that the loop increments by 0x01000001 which is prime
    // to 2^32, and so it will run 2^32 times before it tries the same clockId.

    ReadQueue::iterator entryIter = d_readQueue.end();
    --entryIter;
    bdef_Function<void (*)()> timeoutCallback(
           bdef_BindUtil::bind(
             bdef_MemFnUtil::memFn(&btemt_ChannelPoolChannel::timeoutCb, this),
             entryIter));

    int ret;
    while (1 == (ret = d_channelPool_p->registerClock(timeoutCallback,
                                                      timeOut,
                                                      bdet_TimeInterval(0),
                                                      ++d_nextClockId,
                                                      d_channelId))) {
        d_nextClockId += 0x01000000;
    }
}

void btemt_ChannelPoolChannel::removeTopReadEntry(bool invokeCallback)
{
    ReadQueueEntry&                  entry            = d_readQueue.front();
    int                              timerId          = entry.d_timeOutTimerId;
    int                              cancellationCode = entry.d_progress;
    ReadQueueEntry::ReadDataCallback callbackObj      = entry.d_readCallback;
    CallbackType                     callbackType     = entry.d_callbackType;

    d_readQueue.pop_front();

    bcemt_LockGuardUnlock<bcemt_Mutex> guard(&d_mutex);

    d_channelPool_p->deregisterClock(timerId);

    int dummy1, dummy2;
    if (invokeCallback) {
        if (BTEMT_BLOB_BASED == callbackType) {
            bcema_Blob dummyBlob;
            callbackObj.d_blobBasedCb.object()(cancellationCode,
                                               &dummy1,
                                               &dummyBlob,
                                               0);
        }
        else {
            callbackObj.d_pooledBufferChainBasedCb.object()(cancellationCode,
                                                            &dummy1,
                                                            &dummy2,
                                                            btemt_DataMsg());
        }
    }
}

// CREATORS
btemt_ChannelPoolChannel::btemt_ChannelPoolChannel(
                                 int                             channelId,
                                 btemt_ChannelPool              *channelPool,
                                 bcema_PooledBufferChainFactory *bufferFactory,
                                 bcema_PoolAllocator            *spAllocator,
                                 bslma_Allocator                *allocator)
: d_pooledBufferChainPendingData()
, d_useBlobForDataReads(false)
, d_mutex()
, d_callbackInProgress(false)
, d_closed(false)
, d_readQueue(allocator)
, d_bufferChainFactory_p(bufferFactory)
, d_blobBufferFactory_p(0)
, d_spAllocator_p(spAllocator)
, d_channelPool_p(channelPool)
, d_nextClockId(channelId + 0x00800000)
, d_channelId(channelId)
, d_peerAddress()
, d_localAddress()
, d_allocator_p(bslma_Default::allocator(allocator))
{
    // Cache these addresses since the btemt_ChannelPool channel can have
    // disappeared when we get SESSION_DOWN.

    d_channelPool_p->getLocalAddress(&d_localAddress, d_channelId);
    d_channelPool_p->getPeerAddress(&d_peerAddress, d_channelId);
}

btemt_ChannelPoolChannel::btemt_ChannelPoolChannel(
                             int                             channelId,
                             btemt_ChannelPool              *channelPool,
                             bcema_PooledBlobBufferFactory  *blobBufferFactory,
                             bcema_PoolAllocator            *spAllocator,
                             bslma_Allocator                *allocator)
: d_pooledBufferChainPendingData()
, d_useBlobForDataReads(true)
, d_mutex()
, d_callbackInProgress(false)
, d_closed(false)
, d_readQueue(allocator)
, d_bufferChainFactory_p(0)
, d_blobBufferFactory_p(blobBufferFactory)
, d_spAllocator_p(spAllocator)
, d_channelPool_p(channelPool)
, d_nextClockId(channelId + 0x00800000)
, d_channelId(channelId)
, d_peerAddress()
, d_localAddress()
, d_allocator_p(bslma_Default::allocator(allocator))
{
    // Cache these addresses since the btemt_ChannelPool channel can have
    // disappeared when we get SESSION_DOWN.

    d_channelPool_p->getLocalAddress(&d_localAddress, d_channelId);
    d_channelPool_p->getPeerAddress(&d_peerAddress, d_channelId);
}

btemt_ChannelPoolChannel::~btemt_ChannelPoolChannel()
{
    // Cancel future callbacks, but do not invoke them if this channel is
    // closed.

    cancelRead();
    if (d_useBlobForDataReads) {
        d_allocator_p->deleteObject(d_bufferChainFactory_p);
    }
    else {
        d_allocator_p->deleteObject(d_blobBufferFactory_p);
    }
}

// MANIPULATORS
int btemt_ChannelPoolChannel::read(int           numBytes,
                                   const ReadCallback& callback)
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);

    return addReadQueueEntry<ReadCallback, BTEMT_DATAMSG_BASED>(
                                                         numBytes,
                                                         callback,
                                                         bdet_TimeInterval(0));
}

int btemt_ChannelPoolChannel::read(int                    numBytes,
                                   const BlobBasedReadCallback& callback)
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);

    return addReadQueueEntry<BlobBasedReadCallback, BTEMT_BLOB_BASED>(
                                                         numBytes,
                                                         callback,
                                                         bdet_TimeInterval(0));
}

int btemt_ChannelPoolChannel::timedRead(int                      numBytes,
                                        bdet_TimeInterval const& timeOut,
                                        const ReadCallback&      callback)
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);

    return addReadQueueEntry<ReadCallback, BTEMT_DATAMSG_BASED>(numBytes,
                                                          callback,
                                                          timeOut);
}

int btemt_ChannelPoolChannel::timedRead(int                          numBytes,
                                        bdet_TimeInterval const&     timeOut,
                                        const BlobBasedReadCallback& callback)
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);

    return addReadQueueEntry<BlobBasedReadCallback, BTEMT_BLOB_BASED>(numBytes,
                                                                callback,
                                                                timeOut);
}

int btemt_ChannelPoolChannel::write(const bcema_Blob& blob,
                                    int               highWaterMark)
{
    return d_channelPool_p->write(d_channelId, blob, highWaterMark);
}

int btemt_ChannelPoolChannel::write(const btemt_BlobMsg& blob,
                                    int                  highWaterMark)
{
    return d_channelPool_p->write(d_channelId, blob, highWaterMark);
}

int btemt_ChannelPoolChannel::write(const btemt_DataMsg& data,
                                    btemt_BlobMsg       *msg)
{
    return d_channelPool_p->write(d_channelId, data, INT_MAX, msg);
}

int btemt_ChannelPoolChannel::write(const btemt_DataMsg& data,
                                    int                  highWaterMark,
                                    btemt_BlobMsg       *msg)
{
    return d_channelPool_p->write(d_channelId, data, highWaterMark, msg);
}

void btemt_ChannelPoolChannel::timeoutCb(ReadQueue::iterator entryIter)
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);

    // TBD FIX ME
    // It seems that at this point, 'd_callbackInProgress' must be false.  We
    // need to investigate and remove the if statement if this is true.

    if (d_callbackInProgress && entryIter == d_readQueue.begin()) {
        // We are invoking this callback, it will either succeed (consume
        // everything and be removed from the queue) or need more bytes (and
        // re-enter the loop), so we cannot remove it, but we can mark it as
        // canceled.  It will ensure that it is not called again in case more
        // bytes are needed.

        entryIter->d_progress = btemt_AsyncChannel::BTEMT_TIMEOUT;
    }
    else {
        // Remove this callback from the queue.

        int dummy1 = 0;
        int dummy2 = 0;
        if (BTEMT_BLOB_BASED == entryIter->d_callbackType) {
            BlobBasedReadCallback callback =
                              entryIter->d_readCallback.d_blobBasedCb.object();
            d_readQueue.erase(entryIter);
            bcemt_LockGuardUnlock<bcemt_Mutex> unlockGuard(&d_mutex);
            bcema_Blob emptyBlob;
            callback(btemt_AsyncChannel::BTEMT_TIMEOUT,
                     &dummy1,
                     &emptyBlob,
                     d_channelId);
        }
        else {
            ReadCallback callback =
                 entryIter->d_readCallback.d_pooledBufferChainBasedCb.object();
            d_readQueue.erase(entryIter);
            bcemt_LockGuardUnlock<bcemt_Mutex> unlockGuard(&d_mutex);
            callback(btemt_AsyncChannel::BTEMT_TIMEOUT,
                     &dummy1,
                     &dummy2,
                     btemt_DataMsg());
        }
    }
}

void btemt_ChannelPoolChannel::dataCb(int                  *numConsumed,
                                      int                  *numNeeded,
                                      const btemt_DataMsg&  msg)
{
    bcema_PooledBufferChain *chain             = msg.data();
    int                      numBytesAvailable = chain->length();

    *numNeeded        = 1;
    int totalConsumed = 0;

    const btemt_DataMsg *currentMsg = &msg;

    // We're accessing 'd_pooledBufferChainPendingData' before acquiring the
    // lock because only this method accesses it *and* only the manager thread
    // calls this method.

    bcema_PooledBufferChain *pendingDataChain =
                                         d_pooledBufferChainPendingData.data();

    if (pendingDataChain) {
        // If there is pending data, then we either don't have an
        // active reader, or we don't have enough data available for them
        // yet.  In either case consume the whole message and append
        // it to the pending data.

        pendingDataChain->replace(pendingDataChain->length(), *chain, 0,
                                  numBytesAvailable);
        numBytesAvailable = pendingDataChain->length();
        currentMsg = &d_pooledBufferChainPendingData;
    }

    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    d_callbackInProgress = true;
    while (!d_closed && d_readQueue.size() &&
           (d_readQueue.front().d_numBytesNeeded <= numBytesAvailable ||
            d_readQueue.front().d_progress)) {
        // Note: 'd_closed' may be set by a read callback within the loop, in
        // which case do not process further callbacks and exit the loop.

        ReadQueueEntry& entry = d_readQueue.front();

        if (btemt_AsyncChannel::BTEMT_SUCCESS != entry.d_progress) {
            removeTopReadEntry(true);
            continue;
        }

        CallbackType callbackType = entry.d_callbackType;
        int          nConsumed    = 0;
        int          nNeeded      = 0;

        if (BTEMT_BLOB_BASED == callbackType) {
            BlobBasedReadCallback callback =
                                   entry.d_readCallback.d_blobBasedCb.object();

            if (!d_blobBufferFactory_p) {
                d_blobBufferFactory_p = new (*d_allocator_p)
                                            bcema_PooledBlobBufferFactory(
                                         d_bufferChainFactory_p->bufferSize());
            }

            bcema_Blob blob(d_blobBufferFactory_p);
            btemt_MessageUtil::assignData(&blob,
                                          *currentMsg,
                                          currentMsg->data()->length());
            nConsumed = blob.length();
            {
                bcemt_LockGuardUnlock<bcemt_Mutex> guard(&d_mutex);
                callback(BTEMT_SUCCESS, &nNeeded, &blob, d_channelId);
                nConsumed -= blob.length();
            }
        }
        else {
            ReadCallback callback =
                      entry.d_readCallback.d_pooledBufferChainBasedCb.object();
            {
                bcemt_LockGuardUnlock<bcemt_Mutex> guard(&d_mutex);
                callback(BTEMT_SUCCESS, &nConsumed, &nNeeded, *currentMsg);
            }
        }
        BSLS_ASSERT(0 <= nConsumed && nConsumed <= numBytesAvailable);
        BSLS_ASSERT(0 <= nNeeded);

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(0 != nConsumed)) {
            numBytesAvailable -= nConsumed;
            totalConsumed += nConsumed;

            if (numBytesAvailable) {
                // If there is any data unconsumed, then move it to
                // a new buffer chain (would be done by channel pool, but we
                // prefer to cache data in 'd_pooledBufferChainPendingData'
                // rather than in the channel pool current message; both are
                // same efficiency).

                btemt_DataMsg newData;
                bcema_PooledBufferChain *newChain =
                    d_bufferChainFactory_p->allocate(numBytesAvailable);
                newChain->replace(0, *currentMsg->data(), nConsumed,
                                  numBytesAvailable);
                newData.setData(newChain, d_bufferChainFactory_p,
                                d_spAllocator_p);
                d_pooledBufferChainPendingData = newData;
                currentMsg = &d_pooledBufferChainPendingData;
            }
            else if (currentMsg != &msg) {
                currentMsg->sharedData().clear();
            }
        }

        if (nNeeded) {
            entry.d_numBytesNeeded = nNeeded;
            if (nNeeded <= numBytesAvailable) {
                continue;
            }
            *numNeeded = nNeeded - numBytesAvailable;
        }
        else {
            // Read operation complete for read queue first message, once
            // 'nNeeded == 0'.

            // TBD: Previously timerId was registered only if 0 == size.  That
            // seems wrong.  reconfirm.
            removeTopReadEntry(false);
            if (!d_readQueue.size()) {
                d_channelPool_p->disableRead(d_channelId);
            }
        }
    }
    d_callbackInProgress = false;
    lock.release()->unlock();

    // Touching 'd_pooledBufferChainPendingData' only starting from here so no
    // lock is needed.

    if (!pendingDataChain
      && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == totalConsumed)) {
        // Nothing was consumed and nothing was pending originally, let channel
        // pool keep reading more.

        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        d_pooledBufferChainPendingData.sharedData().clear();
        *numConsumed = 0;
    }
    else {
        // Everything was consumed by this callback, what is not already
        // consumed by the user readCallback is in
        // 'd_pooledBufferChainPendingData'.

        *numConsumed = chain->length();
    }
}

void btemt_ChannelPoolChannel::blobBasedDataCb(int *numNeeded, bcema_Blob *msg)
{
    // We're accessing 'd_blobPendingData' before acquiring the lock because
    // only this method accesses it *and* only the manager thread calls this
    // method.

    bcema_Blob *currentBlob = msg;

    *numNeeded            = 1;
    int numBytesAvailable = currentBlob->length();

    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    d_callbackInProgress = true;

    while (!d_closed
        && d_readQueue.size()
        && (d_readQueue.front().d_numBytesNeeded <= numBytesAvailable
         || d_readQueue.front().d_progress)) {
        // Note: 'd_closed' may be set by a read callback within the loop, in
        // which case do not process further callbacks and exit the loop.

        ReadQueueEntry& entry = d_readQueue.front();

        if (btemt_AsyncChannel::BTEMT_SUCCESS != entry.d_progress) {
            removeTopReadEntry(true);
            continue;
        }

        CallbackType callbackType = entry.d_callbackType;
        int          numConsumed  = 0;
        int          nNeeded      = 0;

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                           BTEMT_BLOB_BASED == callbackType)) {
            BlobBasedReadCallback callback =
                                   entry.d_readCallback.d_blobBasedCb.object();
            numBytesAvailable = currentBlob->length();

            {
                bcemt_LockGuardUnlock<bcemt_Mutex> guard(&d_mutex);
                callback(BTEMT_SUCCESS, &nNeeded, currentBlob, d_channelId);
                numConsumed = numBytesAvailable - currentBlob->length();
            }
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            ReadCallback callback =
                      entry.d_readCallback.d_pooledBufferChainBasedCb.object();

            if (!d_bufferChainFactory_p) {
                d_bufferChainFactory_p = new (*d_allocator_p)
                                             bcema_PooledBufferChainFactory(
                                           d_blobBufferFactory_p->bufferSize(),
                                           d_allocator_p);
            }

            btemt_DataMsg dataMsg;
            btemt_MessageUtil::assignData(&dataMsg,
                                          *currentBlob,
                                          currentBlob->length(),
                                          d_bufferChainFactory_p,
                                          d_spAllocator_p);
            dataMsg.setChannelId(d_channelId);
            {
                bcemt_LockGuardUnlock<bcemt_Mutex> guard(&d_mutex);
                callback(BTEMT_SUCCESS, &numConsumed, &nNeeded, dataMsg);
            }
            dataMsg.sharedData().clear();
            bcema_BlobUtil::erase(currentBlob, 0, numConsumed);
        }

        BSLS_ASSERT(0 <= nNeeded);
        BSLS_ASSERT(0 <= numConsumed);

        numBytesAvailable -= numConsumed;

        if (nNeeded) {
            entry.d_numBytesNeeded = nNeeded;
            if (nNeeded <= numBytesAvailable) {
                continue;
            }

            *numNeeded = nNeeded - numBytesAvailable;
        }
        else {
            removeTopReadEntry(false);
            if (!d_readQueue.size()) {
                d_channelPool_p->disableRead(d_channelId);
            }
        }
    }
    d_callbackInProgress = false;
    lock.release()->unlock();
}

void btemt_ChannelPoolChannel::cancelRead()
{
    const int NUM_ENTRIES = 16;
    const int SIZE        = NUM_ENTRIES * sizeof(ReadQueueEntry);

    char BUFFER[SIZE];
    bdema_BufferedSequentialAllocator bufferAllocator(BUFFER, SIZE);

    ReadQueue cancelQueue(&bufferAllocator);

    {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);

        if (!d_readQueue.size()) return;

        ReadQueue::iterator it=d_readQueue.begin();

        if (d_callbackInProgress) {
            it->d_progress = btemt_AsyncChannel::BTEMT_CANCELED;
            ++it;
        }

        if (!d_closed) {
            cancelQueue.insert(cancelQueue.end(), it, d_readQueue.end());
        }

        for (ReadQueue::iterator it2 = it; it2 != d_readQueue.end(); ++it2) {
            if (bdet_TimeInterval(0) != it2->d_timeOut) {
                d_channelPool_p->deregisterClock(it2->d_timeOutTimerId);
            }
        }

        d_readQueue.erase(it, d_readQueue.end());
    }

    if (!d_closed) {
        // We do not invoke callbacks on a closed channel, because to be
        // correct those callbacks should only be involved in the dispatcher
        // thread of the channel pool's manager corresponding to this channel,
        // but this object might be being destroyed and no further callbacks
        // referencing this channel should be invoked.

        bdet_TimeInterval now = bdetu_SystemTime::now();
        int dummy1 = 0, dummy2 = 0;
        if (d_useBlobForDataReads) {
            bcema_Blob dummyBlob;
            for (ReadQueue::iterator it = cancelQueue.begin();
                                               it != cancelQueue.end(); ++it) {
                bdef_Function<void (*)()> cancelNotifyCallback(
                        bdef_BindUtil::bind(
                                    it->d_readCallback.d_blobBasedCb.object(),
                                    btemt_AsyncChannel::BTEMT_CANCELED,
                                    &dummy1, &dummyBlob, d_channelId));

                int rc;
                while (1 == (rc = d_channelPool_p->registerClock(
                                                        cancelNotifyCallback,
                                                        now,
                                                        bdet_TimeInterval(0),
                                                        ++d_nextClockId,
                                                        d_channelId))) {
                    d_nextClockId += 0x01000000;
                }
                if (rc) {
                    // The channel was already cleaned up in channel pool
                    // because the connection was closed at the other end.
                    // We assume that this is the event manager's dispatcher
                    // thread and it is safe to invoke the cancel notify
                    // callback from this thread.

                    cancelNotifyCallback();
                }
            }
        }
        else {
            btemt_DataMsg dummyMsg;
            for (ReadQueue::iterator it = cancelQueue.begin();
                                               it != cancelQueue.end(); ++it) {
                bdef_Function<void (*)()> cancelNotifyCallback(
                    bdef_BindUtil::bind(
                        it->d_readCallback.d_pooledBufferChainBasedCb.object(),
                        btemt_AsyncChannel::BTEMT_CANCELED,
                        &dummy1, &dummy2, dummyMsg));

                int rc;
                while (1 == (rc = d_channelPool_p->registerClock(
                                                        cancelNotifyCallback,
                                                        now,
                                                        bdet_TimeInterval(0),
                                                        ++d_nextClockId,
                                                        d_channelId))) {
                    d_nextClockId += 0x01000000;
                }
                if (rc) {
                    // The channel was already cleaned up in channel pool
                    // because the connection was closed at the other end.
                    // We assume that this is the event manager's dispatcher
                    // thread and it is safe to invoke the cancel notify
                    // callback from this thread.

                    cancelNotifyCallback();
                }
            }
        }
    }
}

void btemt_ChannelPoolChannel::close()
{
    {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);

        if (!d_closed) {
            d_closed = true;
            d_channelPool_p->shutdown(d_channelId,
                                      btemt_ChannelPool::BTEMT_IMMEDIATE);

            if (!d_readQueue.size()) return;

            ReadQueue::iterator it=d_readQueue.begin();

            if (d_callbackInProgress) {
                it->d_progress = btemt_AsyncChannel::BTEMT_CLOSED;
                ++it;
            }

            for (ReadQueue::iterator it2 = it;
                                             it2 != d_readQueue.end(); ++it2) {
                if (bdet_TimeInterval(0) != it2->d_timeOut) {
                    d_channelPool_p->deregisterClock(it2->d_timeOutTimerId);
                }
            }

            d_readQueue.erase(it, d_readQueue.end());
        }
    }
}

bteso_IPv4Address btemt_ChannelPoolChannel::localAddress() const
{
    return d_localAddress;
}

bteso_IPv4Address btemt_ChannelPoolChannel::peerAddress() const
{
    return d_peerAddress;
}

int btemt_ChannelPoolChannel::setSocketOption(int option, int level, int value)
{
    return d_channelPool_p->setSocketOption(option, level, value, d_channelId);
}

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
