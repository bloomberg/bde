// btlmt_channelpoolchannel.cpp                                       -*-C++-*-
#include <btlmt_channelpoolchannel.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <btlmt_channelpool.h>

#include <bdlmca_blob.h>
#include <bdlmca_blobutil.h>
#include <bdlmca_xxxpooledbufferchain.h>
#include <bdlmca_pooledblobbufferfactory.h>
#include <bdlma_concurrentpoolallocator.h>
#include <bdlqq_lockguard.h>

#include <bdlma_bufferedsequentialallocator.h>

#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>

#include <bdlf_function.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>

#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

namespace BloombergLP {

namespace btlmt {
                       // ------------------------------
                       // class ChannelPoolChannel
                       // ------------------------------

// PRIVATE MANIPULATORS
template <typename BTEMT_CALLBACK>
int ChannelPoolChannel::addReadQueueEntry(
                                             int                      numBytes,
                                             const BTEMT_CALLBACK&    callback,
                                             const bsls::TimeInterval& timeOut)
{
    if (d_closed) {
        return -6;
    }

    BSLS_ASSERT(0 < numBytes);

    d_readQueue.push_back(ReadQueueEntry());
    ReadQueueEntry& entry = d_readQueue.back();

    entry.d_numBytesNeeded = numBytes;
    entry.d_timeOut        = timeOut;
    entry.d_timeOutTimerId = 0;
    entry.d_progress       = AsyncChannel::BTEMT_SUCCESS;
    entry.d_readCallback   = callback;

    if (0 != timeOut.totalMicroseconds()) {
        registerTimeoutAndUpdateClockId(timeOut);
        entry.d_timeOutTimerId = d_nextClockId;
    }

    if (1 == d_readQueue.size()) {
        d_channelPool_p->enableRead(d_channelId);
    }
    return 0;
}

void ChannelPoolChannel::registerTimeoutAndUpdateClockId(
                                              const bsls::TimeInterval& timeOut)
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
    bdlf::Function<void (*)()> timeoutCallback(
           bdlf::BindUtil::bind(
             bdlf::MemFnUtil::memFn(&ChannelPoolChannel::timeoutCb, this),
             entryIter));

    while (1 == d_channelPool_p->registerClock(timeoutCallback,
                                               timeOut,
                                               bsls::TimeInterval(0),
                                               ++d_nextClockId,
                                               d_channelId)) {
        d_nextClockId += 0x01000000;
    }
}

void ChannelPoolChannel::removeTopReadEntry(bool invokeCallback)
{
    ReadQueueEntry&                  entry            = d_readQueue.front();
    int                              timerId          = entry.d_timeOutTimerId;
    int                              cancellationCode = entry.d_progress;
    ReadQueueEntry::ReadCb           callback         = entry.d_readCallback;

    d_readQueue.pop_front();

    bdlqq::LockGuardUnlock<bdlqq::Mutex> guard(&d_mutex);

    d_channelPool_p->deregisterClock(timerId);

    int dummy1, dummy2;
    if (invokeCallback) {
        if (callback.is<BlobBasedReadCallback>()) {
            bdlmca::Blob dummyBlob;
            callback.the<BlobBasedReadCallback>()(cancellationCode,
                                                  &dummy1,
                                                  &dummyBlob,
                                                  0);

        }
        else {
            BSLS_ASSERT_SAFE(callback.is<ReadCallback>());

            callback.the<ReadCallback>()(cancellationCode,
                                         &dummy1,
                                         &dummy2,
                                         DataMsg());
        }
    }
}

// CREATORS
ChannelPoolChannel::ChannelPoolChannel(
                           int                             channelId,
                           ChannelPool              *channelPool,
                           bdlmca::PooledBufferChainFactory *bufferChainFactory,
                           bdlmca::BlobBufferFactory        *blobBufferFactory,
                           bdlma::ConcurrentPoolAllocator            *spAllocator,
                           bslma::Allocator               *allocator,
                           bool                            useBlobForDataReads)
: d_pooledBufferChainPendingData()
, d_useBlobForDataReads(useBlobForDataReads)
, d_mutex()
, d_callbackInProgress(false)
, d_closed(false)
, d_readQueue(allocator)
, d_bufferChainFactory_p(bufferChainFactory, 
                         0,
                         &bslma::ManagedPtrUtil::noOpDeleter)
, d_blobBufferFactory_p(blobBufferFactory, 
                        0,
                        &bslma::ManagedPtrUtil::noOpDeleter)
, d_spAllocator_p(spAllocator)
, d_channelPool_p(channelPool)
, d_nextClockId(channelId + 0x00800000)
, d_channelId(channelId)
, d_peerAddress()
, d_localAddress()
, d_allocator_p(bslma::Default::allocator(allocator))
{
    BSLS_ASSERT(0 != channelPool);
    BSLS_ASSERT(0 != bufferChainFactory);
    BSLS_ASSERT(0 != blobBufferFactory);
    BSLS_ASSERT(0 != spAllocator);

    // Cache these addresses since the ChannelPool channel can have
    // disappeared when we get SESSION_DOWN.
    d_channelPool_p->getLocalAddress(&d_localAddress, d_channelId);
    d_channelPool_p->getPeerAddress(&d_peerAddress, d_channelId);
}

ChannelPoolChannel::~ChannelPoolChannel()
{
    // Cancel future callbacks, but do not invoke them if this channel is
    // closed.

    cancelRead();
}

// MANIPULATORS
int ChannelPoolChannel::read(int numBytes, const ReadCallback& callback)
{
    bdlqq::LockGuard<bdlqq::Mutex> lock(&d_mutex);

    return addReadQueueEntry(numBytes, callback, bsls::TimeInterval(0));
}

int ChannelPoolChannel::read(int                          numBytes,
                                   const BlobBasedReadCallback& callback)
{
    bdlqq::LockGuard<bdlqq::Mutex> lock(&d_mutex);

    return addReadQueueEntry(numBytes, callback, bsls::TimeInterval(0));
}

int ChannelPoolChannel::timedRead(int                      numBytes,
                                        bsls::TimeInterval const& timeOut,
                                        const ReadCallback&      callback)
{
    bdlqq::LockGuard<bdlqq::Mutex> lock(&d_mutex);

    return addReadQueueEntry(numBytes, callback, timeOut);
}

int ChannelPoolChannel::timedRead(int                          numBytes,
                                        bsls::TimeInterval const&     timeOut,
                                        const BlobBasedReadCallback& callback)
{
    bdlqq::LockGuard<bdlqq::Mutex> lock(&d_mutex);

    return addReadQueueEntry(numBytes, callback, timeOut);
}

int ChannelPoolChannel::write(const bdlmca::Blob& blob,
                                    int               highWaterMark)
{
    return d_channelPool_p->write(d_channelId, blob, highWaterMark);
}

int ChannelPoolChannel::write(const BlobMsg& blob,
                                    int                  highWaterMark)
{
    return d_channelPool_p->write(d_channelId, blob, highWaterMark);
}

int ChannelPoolChannel::write(const DataMsg& data,
                                    BlobMsg       *msg)
{
    return d_channelPool_p->write(d_channelId, data, INT_MAX, msg);
}

int ChannelPoolChannel::write(const DataMsg& data,
                                    int                  highWaterMark,
                                    BlobMsg       *msg)
{
    return d_channelPool_p->write(d_channelId, data, highWaterMark, msg);
}

void ChannelPoolChannel::timeoutCb(ReadQueue::iterator entryIter)
{
    bdlqq::LockGuard<bdlqq::Mutex> lock(&d_mutex);

    // TBD FIX ME
    // It seems that at this point, 'd_callbackInProgress' must be false.  We
    // need to investigate and remove the if statement if this is true.

    if (d_callbackInProgress && entryIter == d_readQueue.begin()) {
        // We are invoking this callback, it will either succeed (consume
        // everything and be removed from the queue) or need more bytes (and
        // re-enter the loop), so we cannot remove it, but we can mark it as
        // canceled.  It will ensure that it is not called again in case more
        // bytes are needed.

        entryIter->d_progress = AsyncChannel::BTEMT_TIMEOUT;
    }
    else {
        // Remove this callback from the queue.

        int dummy1 = 0;
        int dummy2 = 0;
        if (entryIter->d_readCallback.is<BlobBasedReadCallback>()) {
            BlobBasedReadCallback callback =
                        entryIter->d_readCallback.the<BlobBasedReadCallback>();
            d_readQueue.erase(entryIter);
            bdlqq::LockGuardUnlock<bdlqq::Mutex> unlockGuard(&d_mutex);
            bdlmca::Blob emptyBlob;
            callback(AsyncChannel::BTEMT_TIMEOUT,
                     &dummy1,
                     &emptyBlob,
                     d_channelId);
        }
        else {
            BSLS_ASSERT_SAFE(entryIter->d_readCallback.is<ReadCallback>());

            ReadCallback callback =
                                 entryIter->d_readCallback.the<ReadCallback>();
            d_readQueue.erase(entryIter);
            bdlqq::LockGuardUnlock<bdlqq::Mutex> unlockGuard(&d_mutex);
            callback(AsyncChannel::BTEMT_TIMEOUT,
                     &dummy1,
                     &dummy2,
                     DataMsg());
        }
    }
}

void ChannelPoolChannel::dataCb(int                  *numConsumed,
                                      int                  *numNeeded,
                                      const DataMsg&  msg)
{
    bdlmca::PooledBufferChain *chain             = msg.data();
    int                      numBytesAvailable = chain->length();

    *numNeeded        = 1;
    int totalConsumed = 0;

    const DataMsg *currentMsg = &msg;

    // We're accessing 'd_pooledBufferChainPendingData' before acquiring the
    // lock because only this method accesses it *and* only the manager thread
    // calls this method.

    bdlmca::PooledBufferChain *pendingDataChain =
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

    bdlqq::LockGuard<bdlqq::Mutex> lock(&d_mutex);
    d_callbackInProgress = true;
    while (!d_closed && d_readQueue.size() &&
           (d_readQueue.front().d_numBytesNeeded <= numBytesAvailable ||
            d_readQueue.front().d_progress)) {
        // Note: 'd_closed' may be set by a read callback within the loop, in
        // which case do not process further callbacks and exit the loop.

        ReadQueueEntry& entry = d_readQueue.front();

        if (AsyncChannel::BTEMT_SUCCESS != entry.d_progress) {
            removeTopReadEntry(true);
            continue;
        }

        int nConsumed = 0;
        int nNeeded   = 0;

        if (entry.d_readCallback.is<BlobBasedReadCallback>()) {
            const BlobBasedReadCallback& callback =
                             entry.d_readCallback.the<BlobBasedReadCallback>();

            bdlmca::Blob blob(d_blobBufferFactory_p.ptr());
            MessageUtil::assignData(&blob,
                                          *currentMsg,
                                          currentMsg->data()->length());
            nConsumed = blob.length();
            {
                bdlqq::LockGuardUnlock<bdlqq::Mutex> guard(&d_mutex);
                callback(BTEMT_SUCCESS, &nNeeded, &blob, d_channelId);
                nConsumed -= blob.length();
            }
        }
        else {
            BSLS_ASSERT_SAFE(entry.d_readCallback.is<ReadCallback>());

            const ReadCallback& callback =
                                      entry.d_readCallback.the<ReadCallback>();
            {
                bdlqq::LockGuardUnlock<bdlqq::Mutex> guard(&d_mutex);
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

                DataMsg newData;
                bdlmca::PooledBufferChain *newChain =
                    d_bufferChainFactory_p->allocate(numBytesAvailable);
                newChain->replace(0, *currentMsg->data(), nConsumed,
                                  numBytesAvailable);
                newData.setData(newChain, d_bufferChainFactory_p.ptr(),
                                d_spAllocator_p);
                d_pooledBufferChainPendingData = newData;
                currentMsg = &d_pooledBufferChainPendingData;
            }
            else if (currentMsg != &msg) {
                currentMsg->sharedData().reset();
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
        d_pooledBufferChainPendingData.sharedData().reset();
        *numConsumed = 0;
    }
    else {
        // Everything was consumed by this callback, what is not already
        // consumed by the user readCallback is in
        // 'd_pooledBufferChainPendingData'.

        *numConsumed = chain->length();
    }
}

void ChannelPoolChannel::blobBasedDataCb(int *numNeeded, bdlmca::Blob *msg)
{
    *numNeeded            = 1;
    int numBytesAvailable = msg->length();

    bdlqq::LockGuard<bdlqq::Mutex> lock(&d_mutex);
    d_callbackInProgress = true;

    while (!d_closed
        && d_readQueue.size()
        && (d_readQueue.front().d_numBytesNeeded <= numBytesAvailable
         || d_readQueue.front().d_progress)) {
        // Note: 'd_closed' may be set by a read callback within the loop, in
        // which case do not process further callbacks and exit the loop.

        ReadQueueEntry& entry = d_readQueue.front();

        if (AsyncChannel::BTEMT_SUCCESS != entry.d_progress) {
            removeTopReadEntry(true);
            continue;
        }

        int numConsumed = 0;
        int nNeeded     = 0;

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                           entry.d_readCallback.is<BlobBasedReadCallback>())) {
            const BlobBasedReadCallback& callback =
                             entry.d_readCallback.the<BlobBasedReadCallback>();
            numBytesAvailable = msg->length();

            {
                bdlqq::LockGuardUnlock<bdlqq::Mutex> guard(&d_mutex);
                callback(BTEMT_SUCCESS, &nNeeded, msg, d_channelId);
                numConsumed = numBytesAvailable - msg->length();
            }
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            BSLS_ASSERT_SAFE(entry.d_readCallback.is<ReadCallback>());

            const ReadCallback& callback =
                                      entry.d_readCallback.the<ReadCallback>();

            DataMsg dataMsg;
            MessageUtil::assignData(&dataMsg,
                                          *msg,
                                          msg->length(),
                                          d_bufferChainFactory_p.ptr(),
                                          d_spAllocator_p);

            dataMsg.setChannelId(d_channelId);
            {
                bdlqq::LockGuardUnlock<bdlqq::Mutex> guard(&d_mutex);
                callback(BTEMT_SUCCESS, &numConsumed, &nNeeded, dataMsg);
            }
            dataMsg.sharedData().reset();
            bdlmca::BlobUtil::erase(msg, 0, numConsumed);
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

void ChannelPoolChannel::cancelRead()
{
    const int NUM_ENTRIES = 16;
    const int SIZE        = NUM_ENTRIES * sizeof(ReadQueueEntry);

    char BUFFER[SIZE];
    bdlma::BufferedSequentialAllocator bufferAllocator(BUFFER, SIZE);

    ReadQueue cancelQueue(&bufferAllocator);

    {
        bdlqq::LockGuard<bdlqq::Mutex> lock(&d_mutex);

        if (!d_readQueue.size()) return;

        ReadQueue::iterator it=d_readQueue.begin();

        if (d_callbackInProgress) {
            it->d_progress = AsyncChannel::BTEMT_CANCELED;
            ++it;
        }

        if (!d_closed) {
            cancelQueue.insert(cancelQueue.end(), it, d_readQueue.end());
        }

        for (ReadQueue::iterator it2 = it; it2 != d_readQueue.end(); ++it2) {
            if (bsls::TimeInterval(0) != it2->d_timeOut) {
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

        bsls::TimeInterval now = bdlt::CurrentTime::now();
        int dummy1 = 0, dummy2 = 0;
        if (d_useBlobForDataReads) {
            bdlmca::Blob dummyBlob;
            for (ReadQueue::iterator it = cancelQueue.begin();
                                               it != cancelQueue.end(); ++it) {
                BSLS_ASSERT_SAFE(
                               it->d_readCallback.is<BlobBasedReadCallback>());

                bdlf::Function<void (*)()> cancelNotifyCallback(
                        bdlf::BindUtil::bind(
                               it->d_readCallback.the<BlobBasedReadCallback>(),
                               AsyncChannel::BTEMT_CANCELED,
                               &dummy1, &dummyBlob, d_channelId));

                int rc;
                while (1 == (rc = d_channelPool_p->registerClock(
                                                        cancelNotifyCallback,
                                                        now,
                                                        bsls::TimeInterval(0),
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
            DataMsg dummyMsg;
            for (ReadQueue::iterator it = cancelQueue.begin();
                                               it != cancelQueue.end(); ++it) {
                BSLS_ASSERT_SAFE(it->d_readCallback.is<ReadCallback>());

                bdlf::Function<void (*)()> cancelNotifyCallback(
                    bdlf::BindUtil::bind(it->d_readCallback.the<ReadCallback>(),
                                        AsyncChannel::BTEMT_CANCELED,
                                        &dummy1, &dummy2, dummyMsg));

                int rc;
                while (1 == (rc = d_channelPool_p->registerClock(
                                                        cancelNotifyCallback,
                                                        now,
                                                        bsls::TimeInterval(0),
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

void ChannelPoolChannel::close()
{
    {
        bdlqq::LockGuard<bdlqq::Mutex> lock(&d_mutex);

        if (!d_closed) {
            d_closed = true;
            d_channelPool_p->shutdown(d_channelId,
                                      ChannelPool::BTEMT_IMMEDIATE);

            if (!d_readQueue.size()) return;

            ReadQueue::iterator it=d_readQueue.begin();

            if (d_callbackInProgress) {
                it->d_progress = AsyncChannel::BTEMT_CLOSED;
                ++it;
            }

            for (ReadQueue::iterator it2 = it;
                                             it2 != d_readQueue.end(); ++it2) {
                if (bsls::TimeInterval(0) != it2->d_timeOut) {
                    d_channelPool_p->deregisterClock(it2->d_timeOutTimerId);
                }
            }

            d_readQueue.erase(it, d_readQueue.end());
        }
    }
}

btlso::IPv4Address ChannelPoolChannel::localAddress() const
{
    return d_localAddress;
}

btlso::IPv4Address ChannelPoolChannel::peerAddress() const
{
    return d_peerAddress;
}

int ChannelPoolChannel::setSocketOption(int option, int level, int value)
{
    return d_channelPool_p->setSocketOption(option, level, value, d_channelId);
}
}  // close package namespace

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
