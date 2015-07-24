// btlmt_channelqueuepool.cpp                                         -*-C++-*-
#include <btlmt_channelqueuepool.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlmt_channelqueuepool_cpp,"$Id$ $CSID$")

#include <btlmt_message.h>
#include <bdlmca_xxxpooledbufferchain.h>
#include <bsls_assert.h>

#include <bdlf_function.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>

namespace BloombergLP {

namespace btlmt {
// PRIVATE MANIPULATORS
void ChannelQueuePool::poolStateCb(int state, int source, int)
{
    Message msg(Message::BTEMT_POOL_STATE);
    PoolMsg& data = msg.poolMsg();
    data.setSourceId(source);
    data.setState((PoolMsg::PoolState)state);
    d_incomingQueue_p->pushBack(msg);
}

void ChannelQueuePool::channelStateCb(int   channelId,
                                            int   sourceId,
                                            int   state,
                                            void *)
{
    Message msg(Message::BTEMT_CHANNEL_STATE);
    ChannelMsg& data = msg.channelMsg();
    data.setChannelId(channelId);
    data.setChannelState((ChannelMsg::ChannelState)state);
    data.setAllocatorId(sourceId);
    d_incomingQueue_p->pushBack(msg);
}

void ChannelQueuePool::dataCb(int           *numConsumed,
                                    int           *minAdditional,
                                    DataMsg  dataMsg,
                                    void          *)
{
    BSLS_ASSERT(numConsumed);
    BSLS_ASSERT(minAdditional);

    *numConsumed = 0;
    int msgSize, numNeeded;

    bdlmca::PooledBufferChain *chain = dataMsg.data();
    int bufferSize   = chain->bufferSize();
    int offset       = 0;
    int bufferIdx    = 0;
    int bufferOffset = 0;

    int lookahead    = (1 < chain->numBuffers())
                     ? bufferSize
                     : chain->length();

    bdlmca::PooledBufferChainFactory *factory = d_channelPool_p->
                                                       incomingBufferFactory();
    /*
    printf("QP dataCb: read %d bytes (%d buffers)\n", chain->length(),
    chain->numBuffers());
    */

    do {
        do {
            msgSize   = -1;
            numNeeded = -1;

            d_userCallback(&msgSize,
                           &numNeeded,
                           chain->buffer(bufferIdx) + bufferOffset,
                           chain->length() - offset);

            BSLS_ASSERT(0 <= numNeeded);
            BSLS_ASSERT(0 <= msgSize);
            BSLS_ASSERT(msgSize <= chain->length() - offset);
            BSLS_ASSERT(msgSize || numNeeded);

            if (msgSize) {
                Message msg(Message::BTEMT_DATA);
                DataMsg& dt = msg.dataMsg();
                dt = dataMsg;
                dt.setUserDataField1(offset);
                dt.setUserDataField2(msgSize);
                d_incomingQueue_p->pushBack(msg);
            }

            *numConsumed += msgSize;  // accumulate numConsumed in case offset
                                      // is reset to 0 by shifting.

            offset += msgSize;
            bufferIdx = offset / bufferSize;
            bufferOffset = offset % bufferSize;

            if (bufferIdx < chain->numBuffers() - 1) {
                lookahead = bufferSize - bufferOffset;
            } else {
                lookahead = chain->length() - offset;
            }

        } while (0 == numNeeded && lookahead >= d_minIncomingMessageSize);

        if (0 == numNeeded && chain->length() - offset >=
                                                    d_minIncomingMessageSize) {
            // We're too close to a buffer boundary, but there is enough data
            // available in subsequent buffers: create shifted chain, and
            // assign it to 'dataMsg' for enqueueing future messages.

            bdlmca::PooledBufferChain *newChain = factory->allocate(0);
            newChain->replace(0, *chain, offset, chain->length() - offset);

            dataMsg.setData(newChain, factory);
            chain        = newChain;
            offset       = 0;
            bufferIdx    = 0;
            bufferOffset = 0;
        }

    } while (0 == numNeeded);

    *minAdditional = numNeeded;
}

void ChannelQueuePool::timerCb(int clockId) {
    Message msg(Message::BTEMT_TIMER);
    TimerMsg& data = msg.timerMsg();
    data.setTimerId(clockId);
    d_incomingQueue_p->pushBack(msg);
}

// CREATORS
ChannelQueuePool::ChannelQueuePool(
        bdlcc::Queue<Message>             *incomingQueue,
        bdlcc::Queue<Message>             *outgoingQueue,
        ParseMessagesCallback                  callback,
        const ChannelPoolConfiguration&  parameters,
        bslma::Allocator                      *basicAllocator)
: d_incomingQueue_p(incomingQueue)
, d_outgoingQueue_p(outgoingQueue)
, d_userCallback(callback)
, d_minIncomingMessageSize(parameters.minIncomingMessageSize())
, d_workTimeout(5.0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_channelCbFunctor
        = ChannelPool::ChannelStateChangeCallback(
                bdlf::MemFnUtil::memFn( &ChannelQueuePool::channelStateCb
                                     , this)
              , basicAllocator);

    d_dataCbFunctor
        = ChannelPool::DataReadCallback(
                bdlf::MemFnUtil::memFn(&ChannelQueuePool::dataCb, this)
              , basicAllocator);

    d_poolCbFunctor
        = ChannelPool::PoolStateChangeCallback(
                bdlf::MemFnUtil::memFn( &ChannelQueuePool::poolStateCb
                                     , this)
              , basicAllocator);

    d_channelPool_p =
        new (*d_allocator_p) ChannelPool(d_channelCbFunctor,
                                               d_dataCbFunctor,
                                               d_poolCbFunctor,
                                               parameters, basicAllocator);
    bsls::AtomicOperations::initInt(&d_runningFlag, 0);
}

ChannelQueuePool::~ChannelQueuePool() {
    d_allocator_p->deleteObjectRaw(d_channelPool_p);
}
}  // close package namespace

extern "C" void *queueProc(void *arg) {
    btlmt::ChannelQueuePool *qp = (btlmt::ChannelQueuePool*)arg;
    return reinterpret_cast<void*>(qp->processOutgoingQueue());
}

namespace btlmt {
// MANIPULATORS
int ChannelQueuePool::start() {
    d_channelPool_p->start();
    bcemt_Attribute attributes;
    attributes.setDetachedState(bcemt_Attribute::BCEMT_CREATE_JOINABLE);
    bsls::AtomicOperations::setInt(&d_runningFlag, 1);
    return bdlmtt::ThreadUtil::create(&d_processorHandle, attributes,
                                    &queueProc, (void*)this);
}

int ChannelQueuePool::stop() {
    d_channelPool_p->stop();
    bsls::AtomicOperations::setInt(&d_runningFlag, 0);
    return bdlmtt::ThreadUtil::join(d_processorHandle);
}

int ChannelQueuePool::processOutgoingQueue() {
    while(bsls::AtomicOperations::getInt(&d_runningFlag)) {
        Message msg(Message::BTEMT_DATA);
        int s = d_outgoingQueue_p->timedPopFront(&msg,
                                      bdlt::CurrentTime::now() + d_workTimeout);

        if (s) {
            continue;
        }

        if (Message::BTEMT_BLOB == msg.type()) {
            const BlobMsg& data = msg.blobMsg();
            if (0 != d_channelPool_p->write(data.channelId(), data)) {
                Message msg(Message::BTEMT_CHANNEL_STATE);
                ChannelMsg& cs = msg.channelMsg();
                cs.setChannelId(data.channelId());
                cs.setChannelState(ChannelMsg::BTEMT_CHANNEL_DOWN);
                cs.setAllocatorId(-1);
                // Then what do we do with msg???
            }
        } else {
            BSLS_ASSERT(Message::BTEMT_DATA == msg.type());
            const DataMsg& data = msg.dataMsg();
            /*
              printf("QP Sending %d bytes to %d\n",
              data.data()->length(), data.channelId());
            */
            if (0 != d_channelPool_p->write(data.channelId(), data)) {
                Message msg(Message::BTEMT_CHANNEL_STATE);
                ChannelMsg& cs = msg.channelMsg();
                cs.setChannelId(data.channelId());
                cs.setChannelState(ChannelMsg::BTEMT_CHANNEL_DOWN);
                cs.setAllocatorId(-1);
                // Then what do we do with msg???
            }
        }
    }
    return 0;
}

int ChannelQueuePool::registerClock(const bsls::TimeInterval& startTime,
                                          const bsls::TimeInterval& period,
                                          int clockId) {
    bdlf::Function<void (*)()> cb(
            bdlf::BindUtil::bindA(
                d_allocator_p
              , &ChannelQueuePool::timerCb
              , this
              , clockId));

    return d_channelPool_p->registerClock(cb, startTime, period, clockId);
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
