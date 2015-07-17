// btlmt_message.cpp                                                  -*-C++-*-
#include <btlmt_message.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlmt_message_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace btlmt {
                             // -------------------
                             // class BlobMsg
                             // -------------------

// CREATORS
BlobMsg::BlobMsg(bdlmca::Blob                *blob,
                             int                        channelId,
                             bslma::Allocator          *basicAllocator)
{
    new (d_impl.d_data.d_arena) Handle(blob, basicAllocator);
    d_impl.d_channelId = channelId;
}

BlobMsg::BlobMsg(const BlobMsg& original)
{
    Handle *h = (Handle *)(void *)original.d_impl.d_data.d_arena;
    new (d_impl.d_data.d_arena) Handle(*h);
    d_impl.d_channelId = original.d_impl.d_channelId;
    d_impl.d_dataLength = original.d_impl.d_dataLength;
    d_impl.d_bufferLength = original.d_impl.d_bufferLength;
}

// MANIPULATORS
BlobMsg& BlobMsg::operator=(const BlobMsg& rhs)
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
}  // close package namespace

                           // ----------------------
                           // class btlmt::ChannelMsg
                           // ----------------------

// FREE OPERATORS
bool btlmt::operator==(const ChannelMsg& lhs,
                const ChannelMsg& rhs)
{
    return lhs.channelId() == rhs.channelId()
        && lhs.allocatorId() == rhs.allocatorId()
        && lhs.channelState() == rhs.channelState();
}

namespace btlmt {
                             // -------------------
                             // class DataMsg
                             // -------------------

DataMsg::DataMsg(const DataMsg& original)
{
    Handle *h = (Handle *)(void *)original.d_impl.d_data.d_arena;
    new (d_impl.d_data.d_arena) Handle(*h);
    d_impl.d_channelId = original.d_impl.d_channelId;
    d_impl.d_dataLength = original.d_impl.d_dataLength;
    d_impl.d_bufferLength = original.d_impl.d_bufferLength;
}

// MANIPULATORS
DataMsg& DataMsg::operator=(const DataMsg& rhs)
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
                             // class UserMsg
                             // -------------------

// CREATORS
UserMsg::UserMsg()
{
    d_impl.d_bufferLength = BTEMT_INVALID_INT_DATA;
    d_impl.d_channelId = BTEMT_INVALID_MESSAGE_TYPE;
    d_impl.d_dataLength = BTEMT_OPAQUE_VALUE;
    d_impl.d_data.d_opaque = (void*)BTEMT_INVALID_VOID_DATA;
}

UserMsg::UserMsg(const UserMsg& original)
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

UserMsg::~UserMsg()
{
    if (BTEMT_OPAQUE_VALUE != d_impl.d_dataLength) {
        Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
        h->~Handle();
    }
}

// MANIPULATORS
UserMsg& UserMsg::operator=(const UserMsg& rhs)
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

void UserMsg::
         setManagedData(bdlmca::PooledBufferChain                *chain,
                        bdlma::Deleter<bdlmca::PooledBufferChain> *deleter,
                        bslma::Allocator                       *basicAllocator)
{
    if (BTEMT_MANAGED == d_impl.d_dataLength) {
        Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
        h->reset(chain, deleter, basicAllocator);
    }
    else {
        d_impl.d_dataLength = BTEMT_MANAGED;
        new (d_impl.d_data.d_arena) Handle(chain, deleter, basicAllocator);
    }
}

void UserMsg::setVoidPtrData(void *value)
{
    if (BTEMT_MANAGED == d_impl.d_dataLength) {
        Handle *h = (Handle *)(void *)d_impl.d_data.d_arena;
        h->~Handle();
    }
    d_impl.d_data.d_opaque = value;
}

                             // -------------------
                             // class Message
                             // -------------------

// CREATORS
Message::Message(MessageType type)
: d_type(type)
{
    switch(d_type) {
      case BTEMT_BLOB: {
          new ((char*)&d_data) DataMsg();
      } break;
      case BTEMT_CHANNEL_STATE: {
          new ((char*)&d_data) ChannelMsg();
      } break;
      case BTEMT_DATA: {
          new ((char*)&d_data) DataMsg();
      } break;
      case BTEMT_POOL_STATE: {
          new ((char*)&d_data) PoolMsg();
      } break;
      case BTEMT_TIMER: {
          new ((char*)&d_data) TimerMsg();
      } break;
      case BTEMT_USER_DATA: {
          new ((char*)&d_data) UserMsg();
      } break;
    }
}

Message::Message(const Message& original)
: d_type(original.d_type)
{
    switch(d_type) {
      case BTEMT_BLOB: {
          new ((char*)&d_data) BlobMsg(original.blobMsg());
      } break;
      case BTEMT_CHANNEL_STATE: {
          new ((char*)&d_data) ChannelMsg(original.channelMsg());
      } break;
      case BTEMT_DATA: {
          new ((char*)&d_data) DataMsg(original.dataMsg());
      } break;
      case BTEMT_POOL_STATE: {
          new ((char*)&d_data) PoolMsg(original.poolMsg());
      } break;
      case BTEMT_TIMER: {
          new ((char*)&d_data) TimerMsg(original.timerMsg());
      } break;
      case BTEMT_USER_DATA: {
          new ((char*)&d_data) UserMsg(original.userMsg());
      } break;
    }
}

Message::~Message()
{
    switch(d_type) {
      case BTEMT_BLOB: {
          BlobMsg& msg = blobMsg();
          msg.~BlobMsg();
      } break;
      case BTEMT_CHANNEL_STATE: {
          ChannelMsg& msg = channelMsg();
          msg.~ChannelMsg();
      } break;
      case BTEMT_DATA: {
          DataMsg& msg = dataMsg();
          msg.~DataMsg();
      } break;
      case BTEMT_POOL_STATE: {
          PoolMsg& msg = poolMsg();
          msg.~PoolMsg();
      } break;
      case BTEMT_TIMER: {
          TimerMsg& msg = timerMsg();
          msg.~TimerMsg();
      } break;
      case BTEMT_USER_DATA: {
          UserMsg& msg = userMsg();
          msg.~UserMsg();
      } break;
    }
}

// MANIPULATORS
Message& Message::operator=(const Message& rhs)
{
    if (this != &rhs) {
        if (d_type == rhs.d_type) {
            switch(d_type) {
              case BTEMT_BLOB: {
                BlobMsg& msg = blobMsg();
                const BlobMsg& original = rhs.blobMsg();
                msg = original;
              } break;
              case BTEMT_CHANNEL_STATE: {
                ChannelMsg& msg = channelMsg();
                const ChannelMsg& original = rhs.channelMsg();
                msg = original;
              } break;
              case BTEMT_DATA: {
                DataMsg& msg = dataMsg();
                const DataMsg& original = rhs.dataMsg();
                msg = original;
              } break;
              case BTEMT_POOL_STATE: {
                PoolMsg& msg = poolMsg();
                const PoolMsg& original = rhs.poolMsg();
                msg = original;
              } break;
              case BTEMT_TIMER: {
                TimerMsg& msg = timerMsg();
                const TimerMsg& original = rhs.timerMsg();
                msg = original;
              } break;
              case BTEMT_USER_DATA: {
                UserMsg& msg = userMsg();
                const UserMsg& original = rhs.userMsg();
                msg = original;
              } break;
            }
        }
        else {
            this->~Message();
            new (this) Message(rhs);
        }
    }
    return *this;
}
}  // close package namespace

bsl::ostream& btlmt::operator<<(bsl::ostream& stream, const ChannelMsg& msg)
{
    switch(msg.event()) {
    case ChannelMsg::BTEMT_CHANNEL_DOWN: {
        stream << "(CHANNEL_DOWN, ";
    } break;
    case ChannelMsg::BTEMT_CHANNEL_UP: {
        stream << "(CHANNEL_UP, ";
    } break;
    case ChannelMsg::BTEMT_READ_TIMEOUT: {
        stream << "(READ_TIMEOUT, ";
    } break;
    case ChannelMsg::BTEMT_SEND_BUFFER_FULL: {
        stream << "(SEND_BUFFER_FULL, ";
    } break;
    case ChannelMsg::BTEMT_MESSAGE_DISCARDED: {
        stream << "(MESSAGE_DISCARDED, ";
    } break;
    case ChannelMsg::BTEMT_AUTO_READ_ENABLED: {
        stream << "(AUTO_READ_ENABLED, ";
    } break;
    case ChannelMsg::BTEMT_AUTO_READ_DISABLED: {
        stream << "(AUTO_READ_DISABLED, ";
    } break;
    case ChannelMsg::BTEMT_WRITE_CACHE_LOWWAT: {
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

bsl::ostream& btlmt::operator<<(bsl::ostream& stream, const PoolMsg& msg) {
    switch(msg.event()) {
    case PoolMsg::BTEMT_ACCEPT_TIMEOUT: {
        stream << "(ACCEPT_TIMEOUT, ";
    } break;
    case PoolMsg::BTEMT_ERROR_ACCEPTING: {
        stream << "(ERROR_ACCEPTING, ";
    } break;
    case PoolMsg::BTEMT_ERROR_CONNECTING: {
        stream << "(ERROR_CONNECTING, ";
    } break;
    case PoolMsg::BTEMT_CHANNEL_LIMIT: {
        stream << "(CHANNEL_LIMIT, ";
    } break;
    case PoolMsg::BTEMT_CAPACITY_LIMIT: {
        stream << "(CAPACITY_LIMIT, ";
    } break;
    default: {
        BSLS_ASSERT("Unhandled message type" && 0);
    }
    }

    stream << msg.sourceId() << ")" << bsl::flush;

    return stream;
}

bsl::ostream& btlmt::operator<<(bsl::ostream& stream, const TimerMsg& msg) {
    stream << "(TIMER ID = " << msg.timerId() << ")" << bsl::flush;

    return stream;
}

namespace btlmt {
void  MessageUtil::assignData(bdlmca::Blob           *blob,
                                    const DataMsg&  dataMsg,
                                    int                   numBytes)
{
    if (0 == numBytes) {
        return;                                                       // RETURN
    }

    bdlmca::PooledBufferChain *chain    = dataMsg.data();
    const int                chainLen = chain->length();

    BSLS_ASSERT(numBytes <= chainLen);

    blob->setLength(numBytes);

    int srcBufSize   = chain->bufferSize();
    int dstBufIdx    = 0, srcBufIdx    = 0;
    int dstBufOffset = 0, srcBufOffset = 0;
    int remaining    = numBytes;

    while (remaining > 0) {
        const bdlmca::BlobBuffer&  dstBuffer      = blob->buffer(dstBufIdx);
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

void MessageUtil::assignData(DataMsg                  *dataMsg,
                                   const bdlmca::Blob&               blob,
                                   int                             numBytes,
                                   bdlmca::PooledBufferChainFactory *factory,
                                   bslma::Allocator               *spAllocator)
{
    if (0 == numBytes) {
        return;                                                       // RETURN
    }

    BSLS_ASSERT(numBytes <= blob.length());

    bdlmca::PooledBufferChain *newChain =
                                    factory->allocate(numBytes);

    BSLS_ASSERT(newChain);

    int numRemaining = numBytes;
    int offset       = 0;

    for (int i = 0; numRemaining > 0; ++i) {
        const bdlmca::BlobBuffer&  buffer         = blob.buffer(i);
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
}  // close package namespace

}  // end namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
