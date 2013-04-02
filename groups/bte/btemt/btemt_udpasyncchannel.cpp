// btemt_udpasyncchannel.cpp               -*-C++-*-
#include <btemt_udpasyncchannel.h>

#include <btemt_blobutil.h>

#include <btemt_message.h>
#include <bteso_eventtype.h>
#include <bteso_socketimputil.h>
#include <bteso_socketoptutil.h>
#include <bcema_blob.h>
#include <bcema_blobutil.h>
#include <bdef_bind.h>
#include <bcemt_lockguard.h>
#include <btemt_log.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace btemt {

namespace {

const int BUFFER_SIZE = 4 * 1024;

}  // close anonymous namespace

                        // ---------------------
                        // class UdpAsyncChannel
                        // ---------------------

// PRIVATE MANIPULATORS
void UdpAsyncChannel::socketReadCb(bcema_SharedPtr<bool> isClosed)
{
    BTEMT_LOG_SET_CATEGORY("btemt.UdpAsyncChannel");

    if (*isClosed) {
        return;
    }

    char buffer[BUFFER_SIZE];
    bteso_IPv4Address fromAddress;
    int errCode = 0;
    int numBytesRead = bteso_SocketImpUtil::readFrom<bteso_IPv4Address>(
                                                                &fromAddress,
                                                                buffer,
                                                                d_socketHandle,
                                                                BUFFER_SIZE,
                                                                &errCode);

    bcema_Blob blobData(d_blobBufferFactory_p);
    if (numBytesRead < 0) {
        BTEMT_LOG_WARN 
            << "error reading from " << d_peerAddress
            << "(" << bteso_SocketImpUtil_Util::mapErrorCode(errCode) << ")"
            << BTEMT_LOG_END;
    } else {
        BSLS_ASSERT(numBytesRead >= 0);
        bcema_BlobUtil::append(&blobData, buffer, numBytesRead);
    }

    bcemt_LockGuard<bcemt_Mutex> lockGuard(&d_mutex);

    if (*d_closed_sp) {
        return;
    }

    BSLS_ASSERT(d_readQueue.size() != 0);

    ReadResult progress;
    BlobBasedReadCallback readCallback;
    removeTopReadEntry(&progress, &readCallback);

    if (errCode) {
        progress = btemt_AsyncChannel::BTEMT_FAILURE;
    }

    if (progress != btemt_AsyncChannel::BTEMT_SUCCESS) {
        lockGuard.release()->unlock();
        int dummyNumNeeded   = 0;
        bcema_Blob dummyBlob;
        readCallback(progress, &dummyNumNeeded, &dummyBlob, 0);
        return;
    }

    if (d_readQueue.size() == 0) {
        d_eventManager_p->deregisterSocket(d_socketHandle);
    }

    lockGuard.release()->unlock();

    int nNeeded   = 0;
    readCallback(BTEMT_SUCCESS, &nNeeded, &blobData, 0);
    BSLS_ASSERT(nNeeded == 0);
}

void UdpAsyncChannel::timeoutCb(bcema_SharedPtr<bool> isClosed, ReadQueue::iterator it)
{
    if (*isClosed) {
        return;
    }

    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);

    if (d_callbackInProgress && it == d_readQueue.begin()) {
        it->d_progress = btemt_AsyncChannel::BTEMT_TIMEOUT;
    } else {

        BlobBasedReadCallback callback = it->d_callback;
        d_readQueue.erase(it);
        lock.release()->unlock();
        int dummyNumNeeded  = 0;
        bcema_Blob dummyBlob;
        callback(btemt_AsyncChannel::BTEMT_TIMEOUT,
                 &dummyNumNeeded,
                 &dummyBlob,
                 0);
    }
}
int UdpAsyncChannel::addReadQueueEntry(
                     const btemt_AsyncChannel::BlobBasedReadCallback& callback,
                     const bdet_TimeInterval&                         timeout)
{
    BTEMT_LOG_SET_CATEGORY("btemt.UdpAsyncChannel");

    if (*d_closed_sp) {
        return -2;
    }

    d_readQueue.push_back(ReadQueueEntry());
    ReadQueueEntry& entry = d_readQueue.back();

    entry.d_callback = callback;
    entry.d_progress = btemt_AsyncChannel::BTEMT_SUCCESS;
    entry.d_timeout  = timeout;
    entry.d_timerId = 0;

    if (timeout.totalMicroseconds() != 0) {
        ReadQueue::iterator it = d_readQueue.end();
        --it;
        entry.d_timerId = d_eventManager_p->registerTimer(
                               bdetu_SystemTime::now() + timeout,
                               bdef_BindUtil::bind(&UdpAsyncChannel::timeoutCb,
                                                   this, d_closed_sp, it));
    }

    if (d_eventManager_p->numSocketEvents(d_socketHandle)) {
        BSLS_ASSERT(d_eventManager_p->numSocketEvents(d_socketHandle) == 1);
        return 0;
    }

    int rc = d_eventManager_p->registerSocketEvent(
                          d_socketHandle,
                          bteso_EventType::BTESO_READ,
                          bdef_BindUtil::bind(&UdpAsyncChannel::socketReadCb, 
                                              this, d_closed_sp));
    if (rc) {
        BTEMT_LOG_ERROR
            << "unable to register READ socket event"
            << BTEMT_LOG_END;
    }

    return rc;
}

void UdpAsyncChannel::removeTopReadEntry(ReadResult            *progress,
                                         BlobBasedReadCallback *callback)
{
    ReadQueueEntry& entry          = d_readQueue.front();
    if (progress) {
        *progress = entry.d_progress;
    }

    if (callback) {
        *callback = entry.d_callback;
    }

    d_eventManager_p->deregisterTimer(entry.d_timerId);

    d_readQueue.pop_front();
}

bool UdpAsyncChannel::openSocket()
{
    BTEMT_LOG_SET_CATEGORY("btemt.UdpAsyncChannel");

    int errCode = 0;
    if (bteso_SocketImpUtil::open<bteso_IPv4Address>(
                                    &d_socketHandle,
                                    bteso_SocketImpUtil::BTESO_SOCKET_DATAGRAM,
                                    &errCode) != 0)
    {
        BTEMT_LOG_ERROR 
            << "Unable to open socket" 
            << "(" << bteso_SocketImpUtil_Util::mapErrorCode(errCode) << ")"
            << " from local address (" << d_localAddress
            << ") to peer address (" << d_peerAddress << ")"
            << BTEMT_LOG_END;

        *d_closed_sp = true;
        return false;
    }
    return true;
}

bool UdpAsyncChannel::bindSocket()
{
    BTEMT_LOG_SET_CATEGORY("btemt.UdpAsyncChannel");
    int errCode = 0;
    if (bteso_SocketImpUtil::bind(d_socketHandle, 
                                  d_localAddress, 
                                  &errCode)  != 0) {
        BTEMT_LOG_ERROR 
            << "Unable to bind to "  << d_localAddress
            << "(" << bteso_SocketImpUtil_Util::mapErrorCode(errCode) << ")"
            << " from local address (" << d_localAddress
            << ") to peer address (" << d_peerAddress << ")"
            << BTEMT_LOG_END;
        
        d_wasBindError = true;
        close();
        return false;
    }
    return true;

}

bool UdpAsyncChannel::connectSocket()
{
    BTEMT_LOG_SET_CATEGORY("btemt.UdpAsyncChannel");

    int errCode = 0;
    if (bteso_SocketImpUtil::connect(d_socketHandle, 
                                     d_peerAddress, 
                                     &errCode)  != 0) {
        BTEMT_LOG_ERROR 
            << "Unable to connect" 
            << "(" << bteso_SocketImpUtil_Util::mapErrorCode(errCode) << ")"
            << " from local address (" << d_localAddress
            << ") to peer address (" << d_peerAddress << ")"
            << BTEMT_LOG_END;
        
        close();
        return false;
    }
    else if (bteso_SocketImpUtil::getLocalAddress<bteso_IPv4Address>(
                                                               &d_localAddress,
                                                               d_socketHandle,
                                                               &errCode) != 0) 
    {
        BTEMT_LOG_ERROR 
            << "Unable to get local address" 
            << "(" << bteso_SocketImpUtil_Util::mapErrorCode(errCode) << ")"
            << " from local address (" << d_localAddress
            << ") to peer address (" << d_peerAddress << ")"
            << BTEMT_LOG_END;
        
        close();
        return false;
    }

    BTEMT_LOG_DEBUG
        << "Success connecting UDP socket "
        << " from local address (" << d_localAddress
        << ") to peer address (" << d_peerAddress << ")"
        << BTEMT_LOG_END;
    return true;
}

// CREATORS
UdpAsyncChannel::UdpAsyncChannel(bteso_TimerEventManager   *eventManager,
                                 const bteso_IPv4Address&   peerAddress, 
                                 bcema_BlobBufferFactory   *blobBufferFactory,
                                 bslma::Allocator           *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
, d_localAddress()
, d_peerAddress(peerAddress)
, d_blobBufferFactory_p(blobBufferFactory)
, d_socketHandle()
, d_eventManager_p(eventManager)
, d_mutex()
, d_readQueue(allocator)
, d_callbackInProgress(false)
, d_closed_sp()
{
    BTEMT_LOG_SET_CATEGORY("btemt.UdpAsyncChannel");

    d_closed_sp.createInplace(d_allocator_p, false);
    if (!openSocket() || !connectSocket()) {
        BTEMT_LOG_ERROR << "Unable to establish udp socket" << BTEMT_LOG_END;
    }
}

UdpAsyncChannel::UdpAsyncChannel(bteso_TimerEventManager   *eventManager,
                                 const bteso_IPv4Address&   localAddress,
                                 const bteso_IPv4Address&   peerAddress,
                                 bcema_BlobBufferFactory   *blobBufferFactory,
                                 bslma::Allocator           *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
, d_localAddress(localAddress)
, d_peerAddress(peerAddress)
, d_blobBufferFactory_p(blobBufferFactory)
, d_socketHandle()
, d_eventManager_p(eventManager)
, d_mutex()
, d_readQueue(allocator)
, d_callbackInProgress(false)
, d_wasBindError(false)
, d_closed_sp()
{
    BTEMT_LOG_SET_CATEGORY("btemt.UdpAsyncChannel");

    d_closed_sp.createInplace(d_allocator_p, false);
    if (!openSocket() || !bindSocket() || !connectSocket())
    {
        BTEMT_LOG_ERROR << "Unable to establish UDP socket" << BTEMT_LOG_END;
    }
}

UdpAsyncChannel::~UdpAsyncChannel()
{
    cancelRead();
    close();
}

// MANIPULATORS
int UdpAsyncChannel::read(int                 numBytes, 
                          const ReadCallback& readCallback)
{
    BSLS_ASSERT(!"Not implemented, use the Blob based read instead");
    return -1;
}

int UdpAsyncChannel::read(int                          numBytes,
                          const BlobBasedReadCallback& readCallback)
{
    return timedRead(numBytes, bdet_TimeInterval(0), readCallback);
}

int UdpAsyncChannel::timedRead(int                      numBytes,
                               const bdet_TimeInterval& timeout,
                               const ReadCallback&      readCallback)
{
    BSLS_ASSERT(!"Not implemented, use the Blob based read instead");
    return -1;
}

int UdpAsyncChannel::timedRead(int                          numBytes,
                               const bdet_TimeInterval&     timeout,
                               const BlobBasedReadCallback& readCallback)
{
    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);
    return addReadQueueEntry(readCallback, timeout);
}

int UdpAsyncChannel::write(const bcema_Blob& blob,
                           int               highWaterMark)
{
    BTEMT_LOG_SET_CATEGORY("btemt.UdpAsyncChannel");

    char buffer[BUFFER_SIZE];

    const char *data = btemt::BlobUtil::getData(buffer, blob.length(), blob);
    int errCode = 0;
    int numBytesWritten 
        = bteso_SocketImpUtil::writeTo(d_socketHandle, 
                                       d_peerAddress,
                                       data, 
                                       blob.length(), 
                                       &errCode);
    if (numBytesWritten < 0) {
        BTEMT_LOG_WARN
            << "error on write to " << d_peerAddress 
            << "(" << bteso_SocketImpUtil_Util::mapErrorCode(errCode) <<")"
            << BTEMT_LOG_END;
        return -1;
    }

    return 0;
}

int UdpAsyncChannel::write(const btemt_BlobMsg& blob,
                           int                  highWaterMark)
{
    BSLS_ASSERT(!"Not implemented, use the Blob based write instead");
    return -1;
}

int UdpAsyncChannel::write(const btemt_DataMsg&  data,
                           btemt_BlobMsg        *msg)
{
    BSLS_ASSERT(!"Not implemented, use the Blob based write instead");
    return -1;
}

int UdpAsyncChannel::write(const btemt_DataMsg&  data,
                           int                   highWaterMark,
                           btemt_BlobMsg        *msg)
{
    BSLS_ASSERT(!"Not implemented, use the Blob based write instead");
    return -1;
}

int UdpAsyncChannel::setSocketOption(int option, int level, int value)
{
    return bteso_SocketOptUtil::setOption(d_socketHandle, 
                                          level, 
                                          option, 
                                          value);
}

void UdpAsyncChannel::cancelRead()
{
    ReadQueue cancelQueue;
    {
        bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);

        d_eventManager_p->deregisterSocket(d_socketHandle);

        if (!d_readQueue.size()) {
            return;
        }

        ReadQueue::iterator it = d_readQueue.begin();
        if (d_callbackInProgress) {
            it->d_progress = btemt_AsyncChannel::BTEMT_CANCELED;
            it++;
        }

        if (!*d_closed_sp) {
            cancelQueue.insert(cancelQueue.end(), it, d_readQueue.end());
        }

        for (ReadQueue::iterator it2 = it; it2 != d_readQueue.end(); ++it2) {
            if (bdet_TimeInterval(0) != it2->d_timeout) {
                d_eventManager_p->deregisterTimer(it2->d_timerId);
            }
        }

        d_readQueue.erase(it, d_readQueue.end());
    }

    if (!*d_closed_sp) {
        int dummyNumNeeded  = 0;
        bcema_Blob dummyBlob;
        for (ReadQueue::iterator it  = cancelQueue.begin();
                                 it != cancelQueue.end();
                               ++it)
        {
            it->d_callback(btemt_AsyncChannel::BTEMT_CANCELED,
                           &dummyNumNeeded,
                           &dummyBlob,
                           0);
        }
    }
}

void UdpAsyncChannel::close()
{
    BTEMT_LOG_SET_CATEGORY("btemt.UdpAsyncChannel");

    bcemt_LockGuard<bcemt_Mutex> lock(&d_mutex);

    if (*d_closed_sp) {
        return;
    }

    *d_closed_sp = true;

    d_eventManager_p->deregisterSocket(d_socketHandle);

    int errCode = 0;
    if (bteso_SocketImpUtil::close(d_socketHandle, &errCode) != 0) {
        BTEMT_LOG_WARN 
            << "Error closing socket" 
            << "(" << bteso_SocketImpUtil_Util::mapErrorCode(errCode) << ")"
            << " with local address (" << d_localAddress
            << ") and peer address (" << d_peerAddress << ")"
            << BTEMT_LOG_END;
    }

    if (!d_readQueue.size()) {
        return;
    }

    ReadQueue::iterator it = d_readQueue.begin();
    if (d_callbackInProgress) {
        it->d_progress = btemt_AsyncChannel::BTEMT_CLOSED;
        it++;
    }

    for (ReadQueue::iterator it2 = it; it2 != d_readQueue.end(); ++it2) {
        if (bdet_TimeInterval(0) != it2->d_timeout) {
            d_eventManager_p->deregisterTimer(it2->d_timerId);
        }
    }
    d_readQueue.erase(it, d_readQueue.end());
}

// ACCESSORS
bool UdpAsyncChannel::isClosed() const
{
    return *d_closed_sp;
}

bool UdpAsyncChannel::wasBindError() const
{
    return d_wasBindError;
}

bteso_IPv4Address UdpAsyncChannel::localAddress() const
{
    return d_localAddress;
}

bteso_IPv4Address UdpAsyncChannel::peerAddress() const
{
    return d_peerAddress;
}

}  // close package namespace

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
