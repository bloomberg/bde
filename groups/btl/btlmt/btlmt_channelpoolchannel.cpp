// btlmt_channelpoolchannel.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlmt_channelpoolchannel.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <btlmt_channelpool.h>

#include <btlb_blob.h>
#include <btlb_blobutil.h>
#include <btlb_pooledblobbufferfactory.h>
#include <bdlma_concurrentpoolallocator.h>
#include <bslmt_lockguard.h>

#include <bdlma_bufferedsequentialallocator.h>

#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>

#include <bdlf_bind.h>
#include <bdlf_memfn.h>

#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

#include <bsl_functional.h>

namespace BloombergLP {

namespace btlmt {

                       // ------------------------
                       // class ChannelPoolChannel
                       // ------------------------

// PRIVATE MANIPULATORS
int ChannelPoolChannel::addReadQueueEntry(
                                         int                          numBytes,
                                         const BlobBasedReadCallback& callback,
                                         const bsls::TimeInterval&    timeOut)
{
    if (d_closed) {
        return -6;                                                    // RETURN
    }

    BSLS_ASSERT(0 < numBytes);

    d_readQueue.push_back(ReadQueueEntry());
    ReadQueueEntry& entry = d_readQueue.back();

    entry.d_numBytesNeeded = numBytes;
    entry.d_timeOut        = timeOut;
    entry.d_timeOutTimerId = 0;
    entry.d_progress       = AsyncChannel::e_SUCCESS;
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
    // happens to be a duplicate.  With current usage in session pool, a single
    // iteration through the loop (i.e., no loop at all) should be guaranteed.
    // Note that the loop increments by 0x01000001 which is prime to 2^32, and
    // so it will run 2^32 times before it tries the same clockId.

    ReadQueue::iterator entryIter = d_readQueue.end();
    --entryIter;
    bsl::function<void()> timeoutCallback(bdlf::BindUtil::bind(
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
    ReadQueueEntry&        entry            = d_readQueue.front();
    int                    timerId          = entry.d_timeOutTimerId;
    int                    cancellationCode = entry.d_progress;
    BlobBasedReadCallback& callback         = entry.d_readCallback;

    d_readQueue.pop_front();

    bslmt::LockGuardUnlock<bslmt::Mutex> guard(&d_mutex);

    d_channelPool_p->deregisterClock(timerId);

    if (invokeCallback) {
        int        dummy;
        btlb::Blob dummyBlob;

        callback(cancellationCode, &dummy, &dummyBlob, 0);
    }
}

// CREATORS
ChannelPoolChannel::ChannelPoolChannel(
                             int                             channelId,
                             ChannelPool                    *channelPool,
                             btlb::BlobBufferFactory        *blobBufferFactory,
                             bdlma::ConcurrentPoolAllocator *spAllocator,
                             bslma::Allocator               *basicAllocator)
: d_mutex()
, d_callbackInProgress(false)
, d_closed(false)
, d_readQueue(basicAllocator)
, d_blobBufferFactory_p(blobBufferFactory,
                        0,
                        &bslma::ManagedPtrUtil::noOpDeleter)
, d_spAllocator_p(spAllocator)
, d_channelPool_p(channelPool)
, d_nextClockId(channelId + 0x00800000)
, d_channelId(channelId)
, d_peerAddress()
, d_localAddress()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    BSLS_ASSERT(0 != channelPool);
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
int ChannelPoolChannel::read(int                          numBytes,
                             const BlobBasedReadCallback& callback)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    return addReadQueueEntry(numBytes, callback, bsls::TimeInterval(0));
}

int ChannelPoolChannel::timedRead(int                          numBytes,
                                  const bsls::TimeInterval&    timeOut,
                                  const BlobBasedReadCallback& callback)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    return addReadQueueEntry(numBytes, callback, timeOut);
}

int ChannelPoolChannel::write(const btlb::Blob& blob, int highWaterMark)
{
    return d_channelPool_p->write(d_channelId, blob, highWaterMark);
}

void ChannelPoolChannel::timeoutCb(ReadQueue::iterator entryIter)
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    // It seems that at this point, 'd_callbackInProgress' must be false.  We
    // need to investigate and remove the if statement if this is true.

    if (d_callbackInProgress && entryIter == d_readQueue.begin()) {
        // We are invoking this callback, it will either succeed (consume
        // everything and be removed from the queue) or need more bytes (and
        // re-enter the loop), so we cannot remove it, but we can mark it as
        // canceled.  It will ensure that it is not called again in case more
        // bytes are needed.

        entryIter->d_progress = AsyncChannel::e_TIMEOUT;
    }
    else {
        // Remove this callback from the queue.

        BlobBasedReadCallback callback = entryIter->d_readCallback;
        d_readQueue.erase(entryIter);
        {
            bslmt::LockGuardUnlock<bslmt::Mutex> unlockGuard(&d_mutex);

            int          dummy = 0;
            btlb::Blob emptyBlob;

            callback(AsyncChannel::e_TIMEOUT,
                     &dummy,
                     &emptyBlob,
                     d_channelId);
        }
    }
}

void ChannelPoolChannel::cancelRead()
{
    const int NUM_ENTRIES = 16;
    const int SIZE        = NUM_ENTRIES * sizeof(ReadQueueEntry);

    char                               BUFFER[SIZE];
    bdlma::BufferedSequentialAllocator bufferAllocator(BUFFER, SIZE);

    ReadQueue cancelQueue(&bufferAllocator);

    {
        bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

        if (!d_readQueue.size()) {
            return;                                                   // RETURN
        }

        ReadQueue::iterator it = d_readQueue.begin();

        if (d_callbackInProgress) {
            it->d_progress = AsyncChannel::e_CANCELED;
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
        int dummy = 0;
        btlb::Blob dummyBlob;
        for (ReadQueue::iterator it = cancelQueue.begin();
             it != cancelQueue.end(); ++it) {
            bsl::function<void()> cancelNotifyCallback(
                                 bdlf::BindUtil::bind(it->d_readCallback,
                                                      AsyncChannel::e_CANCELED,
                                                      &dummy,
                                                      &dummyBlob,
                                                      d_channelId));

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
                // The channel was already cleaned up in channel pool because
                // the connection was closed at the other end.  We assume that
                // this is the event manager's dispatcher thread and it is safe
                // to invoke the cancel notify callback from this thread.

                cancelNotifyCallback();
            }
        }
    }
}

void ChannelPoolChannel::close()
{
    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);

    if (!d_closed) {
        d_closed = true;
        d_channelPool_p->shutdown(d_channelId, ChannelPool::e_IMMEDIATE);

        if (!d_readQueue.size()) {
            return;                                                   // RETURN
        }

        ReadQueue::iterator it = d_readQueue.begin();

        if (d_callbackInProgress) {
            it->d_progress = AsyncChannel::e_CLOSED;
            ++it;
        }

        for (ReadQueue::iterator it2 = it; it2 != d_readQueue.end(); ++it2) {
            if (bsls::TimeInterval(0) != it2->d_timeOut) {
                d_channelPool_p->deregisterClock(it2->d_timeOutTimerId);
            }
        }

        d_readQueue.erase(it, d_readQueue.end());
    }
}

void ChannelPoolChannel::blobBasedDataCb(int *numNeeded, btlb::Blob *msg)
{
    *numNeeded            = 1;
    int numBytesAvailable = msg->length();

    bslmt::LockGuard<bslmt::Mutex> lock(&d_mutex);
    d_callbackInProgress = true;

    while (!d_closed
        && d_readQueue.size()
        && (d_readQueue.front().d_numBytesNeeded <= numBytesAvailable
         || d_readQueue.front().d_progress)) {
        // Note: 'd_closed' may be set by a read callback within the loop, in
        // which case do not process further callbacks and exit the loop.

        ReadQueueEntry& entry = d_readQueue.front();

        if (AsyncChannel::e_SUCCESS != entry.d_progress) {
            removeTopReadEntry(true);
            continue;
        }

        int numConsumed = 0;
        int nNeeded     = 0;

        const BlobBasedReadCallback& callback = entry.d_readCallback;
        numBytesAvailable = msg->length();

        {
            bslmt::LockGuardUnlock<bslmt::Mutex> guard(&d_mutex);
            callback(e_SUCCESS, &nNeeded, msg, d_channelId);
            numConsumed = numBytesAvailable - msg->length();
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

// ACCESSORS
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
