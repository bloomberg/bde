// btemt_message.cpp             -*-C++-*-
#include <btemt_message.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_message_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                             // -------------------
                             // class btemt_BlobMsg
                             // -------------------

// CREATORS
btemt_BlobMsg::btemt_BlobMsg(bcema_Blob                *blob,
                             int                        channelId,
                             bslma_Allocator           *basicAllocator)
{
    new (d_impl.d_data.d_arena) Handle(blob, basicAllocator);
    d_impl.d_channelId = channelId;
}

btemt_BlobMsg::btemt_BlobMsg(const btemt_BlobMsg& original)
{
    Handle *h = (Handle *)(void *)original.d_impl.d_data.d_arena;
    new (d_impl.d_data.d_arena) Handle(*h);
    d_impl.d_channelId = original.d_impl.d_channelId;
    d_impl.d_dataLength = original.d_impl.d_dataLength;
    d_impl.d_bufferLength = original.d_impl.d_bufferLength;
}

// MANIPULATORS
btemt_BlobMsg& btemt_BlobMsg::operator=(const btemt_BlobMsg& rhs)
{
    if (this != &rhs) {
        Handle *src = (Handle *)(void *)rhs.d_impl.d_data.d_arena;
        Handle *dst = (Handle *)(void *)d_impl.d_data.d_arena;
        *dst = *src;
        d_impl.d_channelId = rhs.d_impl.d_channelId;
        d_impl.d_dataLength = rhs.d_impl.d_dataLength;
        d_impl.d_bufferLength = rhs.d_impl.d_bufferLength;
    }
    return *this;
}

                           // ----------------------
                           // class btemt_ChannelMsg
                           // ----------------------

// FREE OPERATORS
bool operator==(const btemt_ChannelMsg& lhs,
                const btemt_ChannelMsg& rhs)
{
    return lhs.channelId() == rhs.channelId()
        && lhs.allocatorId() == rhs.allocatorId()
        && lhs.channelState() == rhs.channelState();
}

                             // -------------------
                             // class btemt_DataMsg
                             // -------------------

btemt_DataMsg::btemt_DataMsg(const btemt_DataMsg& original)
{
    Handle *h = (Handle *)(void *)original.d_impl.d_data.d_arena;
    new (d_impl.d_data.d_arena) Handle(*h);
    d_impl.d_channelId = original.d_impl.d_channelId;
    d_impl.d_dataLength = original.d_impl.d_dataLength;
    d_impl.d_bufferLength = original.d_impl.d_bufferLength;
}

// MANIPULATORS
btemt_DataMsg& btemt_DataMsg::operator=(const btemt_DataMsg& rhs)
{
    if (this != &rhs) {
        Handle *src = (Handle *)(void *)rhs.d_impl.d_data.d_arena;
        Handle *dst = (Handle *)(void *)d_impl.d_data.d_arena;
        *dst = *src;
        d_impl.d_channelId = rhs.d_impl.d_channelId;
        d_impl.d_dataLength = rhs.d_impl.d_dataLength;
        d_impl.d_bufferLength = rhs.d_impl.d_bufferLength;
    }
    return *this;
}

                             // -------------------
                             // class btemt_UserMsg
                             // -------------------

// CREATORS
btemt_UserMsg::btemt_UserMsg()
{
    d_impl.d_bufferLength = BTEMT_INVALID_INT_DATA;
    d_impl.d_channelId = BTEMT_INVALID_MESSAGE_TYPE;
    d_impl.d_dataLength = BTEMT_OPAQUE_VALUE;
    d_impl.d_data.d_opaque = (void*)BTEMT_INVALID_VOID_DATA;
}

btemt_UserMsg::btemt_UserMsg(const btemt_UserMsg& original)
{
    d_impl.d_bufferLength = original.d_impl.d_bufferLength;
    d_impl.d_channelId = original.d_impl.d_channelId;
    d_impl.d_dataLength = original.d_impl.d_dataLength;
    if (BTEMT_OPAQUE_VALUE == d_impl.d_dataLength) {
        d_impl.d_data.d_opaque = original.d_impl.d_data.d_opaque;
    }
    else {
        Handle *src = (Handle *)(void *)original.d_impl.d_data.d_arena;
        new (d_impl.d_data.d_arena) Handle(*src);
    }
}

btemt_UserMsg::~btemt_UserMsg()
{
    if (BTEMT_OPAQUE_VALUE != d_impl.d_dataLength) {
        Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
        h->~Handle();
    }
}

// MANIPULATORS
btemt_UserMsg& btemt_UserMsg::operator=(const btemt_UserMsg& rhs)
{
    if (this != &rhs) {
        d_impl.d_bufferLength = rhs.d_impl.d_bufferLength;
        d_impl.d_channelId = rhs.d_impl.d_channelId;
        if (BTEMT_OPAQUE_VALUE == d_impl.d_dataLength) {
            if (BTEMT_OPAQUE_VALUE == rhs.d_impl.d_dataLength) {
                // BTEMT_OPAQUE <= BTEMT_OPAQUE
                d_impl.d_data.d_opaque = rhs.d_impl.d_data.d_opaque;
            }
            else {
                // BTEMT_OPAQUE <= BTEMT_MANAGED
                Handle *src = (Handle *)(void *)rhs.d_impl.d_data.d_arena;
                new (d_impl.d_data.d_arena) Handle(*src);
            }
        }
        else {
            if (BTEMT_OPAQUE_VALUE == rhs.d_impl.d_dataLength) {
                // BTEMT_MANAGED <= BTEMT_OPAQUE
                Handle *src = (Handle *)(void *)d_impl.d_data.d_arena;
                src->~Handle();
                d_impl.d_data.d_opaque = rhs.d_impl.d_data.d_opaque;
            }
            else {
                // BTEMT_MANAGED <= BTEMT_MANAGED
                Handle *src = (Handle *)(void *)rhs.d_impl.d_data.d_arena;
                Handle *dst = (Handle *)(void *)d_impl.d_data.d_arena;
                *dst = *src;
            }
        }
        d_impl.d_dataLength = rhs.d_impl.d_dataLength;
    }
    return *this;
}

void btemt_UserMsg::
         setManagedData(bcema_PooledBufferChain                *chain,
                        bcema_Deleter<bcema_PooledBufferChain> *deleter,
                        bslma_Allocator                        *basicAllocator)
{
    if (BTEMT_MANAGED == d_impl.d_dataLength) {
        Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
        h->load(chain, deleter, basicAllocator);
    }
    else {
        d_impl.d_dataLength = BTEMT_MANAGED;
        new (d_impl.d_data.d_arena) Handle(chain, deleter, basicAllocator);
    }
}

void btemt_UserMsg::setVoidPtrData(void *value)
{
    if (BTEMT_MANAGED == d_impl.d_dataLength) {
        Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
        h->~Handle();
    }
    d_impl.d_data.d_opaque = value;
}

                             // -------------------
                             // class btemt_Message
                             // -------------------

// CREATORS
btemt_Message::btemt_Message(MessageType type)
: d_type(type)
{
    switch(d_type) {
      case BTEMT_BLOB: {
          new ((char*)&d_data) btemt_DataMsg();
      } break;
      case BTEMT_CHANNEL_STATE: {
          new ((char*)&d_data) btemt_ChannelMsg();
      } break;
      case BTEMT_DATA: {
          new ((char*)&d_data) btemt_DataMsg();
      } break;
      case BTEMT_POOL_STATE: {
          new ((char*)&d_data) btemt_PoolMsg();
      } break;
      case BTEMT_TIMER: {
          new ((char*)&d_data) btemt_TimerMsg();
      } break;
      case BTEMT_USER_DATA: {
          new ((char*)&d_data) btemt_UserMsg();
      } break;
    }
}

btemt_Message::btemt_Message(const btemt_Message& original)
: d_type(original.d_type)
{
    switch(d_type) {
      case BTEMT_BLOB: {
          new ((char*)&d_data) btemt_BlobMsg(original.blobMsg());
      } break;
      case BTEMT_CHANNEL_STATE: {
          new ((char*)&d_data) btemt_ChannelMsg(original.channelMsg());
      } break;
      case BTEMT_DATA: {
          new ((char*)&d_data) btemt_DataMsg(original.dataMsg());
      } break;
      case BTEMT_POOL_STATE: {
          new ((char*)&d_data) btemt_PoolMsg(original.poolMsg());
      } break;
      case BTEMT_TIMER: {
          new ((char*)&d_data) btemt_TimerMsg(original.timerMsg());
      } break;
      case BTEMT_USER_DATA: {
          new ((char*)&d_data) btemt_UserMsg(original.userMsg());
      } break;
    }
}

btemt_Message::~btemt_Message()
{
    switch(d_type) {
      case BTEMT_BLOB: {
          btemt_BlobMsg& msg = blobMsg();
          msg.~btemt_BlobMsg();
      } break;
      case BTEMT_CHANNEL_STATE: {
          btemt_ChannelMsg& msg = channelMsg();
          msg.~btemt_ChannelMsg();
      } break;
      case BTEMT_DATA: {
          btemt_DataMsg& msg = dataMsg();
          msg.~btemt_DataMsg();
      } break;
      case BTEMT_POOL_STATE: {
          btemt_PoolMsg& msg = poolMsg();
          msg.~btemt_PoolMsg();
      } break;
      case BTEMT_TIMER: {
          btemt_TimerMsg& msg = timerMsg();
          msg.~btemt_TimerMsg();
      } break;
      case BTEMT_USER_DATA: {
          btemt_UserMsg& msg = userMsg();
          msg.~btemt_UserMsg();
      } break;
    }
}

// MANIPULATORS
btemt_Message& btemt_Message::operator=(const btemt_Message& rhs)
{
    if (this != &rhs) {
        if (d_type == rhs.d_type) {
            switch(d_type) {
              case BTEMT_BLOB: {
                btemt_BlobMsg& msg = blobMsg();
                const btemt_BlobMsg& original = rhs.blobMsg();
                msg = original;
              } break;
              case BTEMT_CHANNEL_STATE: {
                btemt_ChannelMsg& msg = channelMsg();
                const btemt_ChannelMsg& original = rhs.channelMsg();
                msg = original;
              } break;
              case BTEMT_DATA: {
                btemt_DataMsg& msg = dataMsg();
                const btemt_DataMsg& original = rhs.dataMsg();
                msg = original;
              } break;
              case BTEMT_POOL_STATE: {
                btemt_PoolMsg& msg = poolMsg();
                const btemt_PoolMsg& original = rhs.poolMsg();
                msg = original;
              } break;
              case BTEMT_TIMER: {
                btemt_TimerMsg& msg = timerMsg();
                const btemt_TimerMsg& original = rhs.timerMsg();
                msg = original;
              } break;
              case BTEMT_USER_DATA: {
                btemt_UserMsg& msg = userMsg();
                const btemt_UserMsg& original = rhs.userMsg();
                msg = original;
              } break;
            }
        }
        else {
            this->~btemt_Message();
            new (this) btemt_Message(rhs);
        }
    }
    return *this;
}

bsl::ostream& operator<<(bsl::ostream& stream, const btemt_ChannelMsg& msg)
{
    switch(msg.event()) {
    case btemt_ChannelMsg::BTEMT_CHANNEL_DOWN: {
        stream << "(CHANNEL_DOWN, ";
    } break;
    case btemt_ChannelMsg::BTEMT_CHANNEL_UP: {
        stream << "(CHANNEL_UP, ";
    } break;
    case btemt_ChannelMsg::BTEMT_READ_TIMEOUT: {
        stream << "(READ_TIMEOUT, ";
    } break;
    case btemt_ChannelMsg::BTEMT_SEND_BUFFER_FULL: {
        stream << "(SEND_BUFFER_FULL, ";
    } break;
    case btemt_ChannelMsg::BTEMT_MESSAGE_DISCARDED: {
        stream << "(MESSAGE_DISCARDED, ";
    } break;
    case btemt_ChannelMsg::BTEMT_AUTO_READ_ENABLED: {
        stream << "(AUTO_READ_ENABLED, ";
    } break;
    case btemt_ChannelMsg::BTEMT_AUTO_READ_DISABLED: {
        stream << "(AUTO_READ_DISABLED, ";
    } break;
    case btemt_ChannelMsg::BTEMT_WRITE_CACHE_LOWWAT: {
        stream << "(WRITE_CACHE_LOWWAT, ";
    }
    default: {
        BSLS_ASSERT("Unhandled message type" && 0);
    }
    }

    stream << msg.channelId() << ", " << msg.allocatorId()
           << ")" << bsl::flush;
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const btemt_PoolMsg& msg) {
    switch(msg.event()) {
    case btemt_PoolMsg::BTEMT_ACCEPT_TIMEOUT: {
        stream << "(ACCEPT_TIMEOUT, ";
    } break;
    case btemt_PoolMsg::BTEMT_ERROR_ACCEPTING: {
        stream << "(ERROR_ACCEPTING, ";
    } break;
    case btemt_PoolMsg::BTEMT_ERROR_CONNECTING: {
        stream << "(ERROR_CONNECTING, ";
    } break;
    case btemt_PoolMsg::BTEMT_CHANNEL_LIMIT: {
        stream << "(CHANNEL_LIMIT, ";
    } break;
    case btemt_PoolMsg::BTEMT_CAPACITY_LIMIT: {
        stream << "(CAPACITY_LIMIT, ";
    } break;
    default: {
        BSLS_ASSERT("Unhandled message type" && 0);
    }
    }

    stream << msg.sourceId() << ")" << bsl::flush;

    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const btemt_TimerMsg& msg) {
    stream << "(TIMER ID = " << msg.timerId() << ")" << bsl::flush;

    return stream;
}

void  btemt_MessageUtil::assignData(bcema_Blob           *blob,
                                    const btemt_DataMsg&  dataMsg,
                                    int                   numBytes)
{
    if (0 == numBytes) {
        return;                                                       // RETURN
    }

    bcema_PooledBufferChain *chain    = dataMsg.data();
    const int                chainLen = chain->length();

    BSLS_ASSERT(numBytes <= chainLen);

    blob->setLength(numBytes);

    int srcBufSize   = chain->bufferSize();
    int dstBufIdx    = 0, srcBufIdx    = 0;
    int dstBufOffset = 0, srcBufOffset = 0;
    int remaining    = numBytes;

    while (remaining > 0) {
        const bcema_BlobBuffer&  dstBuffer      = blob->buffer(dstBufIdx);
        const int                dstBufSize     = dstBuffer.size();
        char                    *dstData        = dstBuffer.data();
        const int                dstRemBufSize  = dstBufSize - dstBufOffset;
        const int                srcRemBufSize  = srcBufSize - srcBufOffset;
        const int                numBytesToCopy = dstRemBufSize > srcRemBufSize
                                               ? srcRemBufSize : dstRemBufSize;
        bsl::memcpy(dstData + dstBufOffset,
                    chain->buffer(srcBufIdx) + srcBufOffset,
                    numBytesToCopy);

        remaining -= numBytesToCopy;
        srcBufOffset = srcBufOffset + numBytesToCopy;
        if (srcBufOffset == srcBufSize) {
            srcBufOffset = 0;
            ++srcBufIdx;
        }

        dstBufOffset = dstBufOffset + numBytesToCopy;
        if (dstBufOffset == dstBufSize) {
            dstBufOffset = 0;
            ++dstBufIdx;
        }
    }
}

void  btemt_MessageUtil::assignData(btemt_DataMsg     *dataMsg,
                                    const bcema_Blob&  blob,
                                    int                numBytes,
                                    bcema_PooledBufferChainFactory *factory,
                                    bslma_Allocator    *spAllocator)
{
    if (0 == numBytes) {
        return;                                                       // RETURN
    }

    BSLS_ASSERT(numBytes <= blob.length());

    bcema_PooledBufferChain *newChain =
                                    factory->allocate(numBytes);

    BSLS_ASSERT(newChain);

    int numRemaining = numBytes;
    int offset       = 0;

    for (int i = 0; numRemaining > 0; ++i) {
        const bcema_BlobBuffer&  buffer         = blob.buffer(i);
        const int                bufSize        = buffer.size();
        const char              *bufData        = buffer.data();
        const int                numBytesToCopy = numRemaining > bufSize
                                                ? bufSize : numRemaining;

        newChain->replace(offset, bufData, numBytesToCopy);
        numRemaining -= numBytesToCopy;
        offset       += numBytesToCopy;
    }

    dataMsg->setData(newChain, factory, spAllocator);
}

}  // end namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
