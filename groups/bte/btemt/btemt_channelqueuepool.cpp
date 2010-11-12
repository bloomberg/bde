// btemt_channelqueuepool.cpp                                         -*-C++-*-
#include <btemt_channelqueuepool.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_channelqueuepool_cpp,"$Id$ $CSID$")

#include <btemt_message.h>
#include <bcema_pooledbufferchain.h>
#include <bsls_assert.h>

#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_memfn.h>

namespace BloombergLP {

// PRIVATE MANIPULATORS
void btemt_ChannelQueuePool::poolStateCb(int state, int source, int)
{
    btemt_Message msg(btemt_Message::BTEMT_POOL_STATE);
    btemt_PoolMsg& data = msg.poolMsg();
    data.setSourceId(source);
    data.setState((btemt_PoolMsg::PoolState)state);
    d_incomingQueue_p->pushBack(msg);
}

void btemt_ChannelQueuePool::channelStateCb(int   channelId,
                                            int   sourceId,
                                            int   state,
                                            void *)
{
    btemt_Message msg(btemt_Message::BTEMT_CHANNEL_STATE);
    btemt_ChannelMsg& data = msg.channelMsg();
    data.setChannelId(channelId);
    data.setChannelState((btemt_ChannelMsg::ChannelState)state);
    data.setAllocatorId(sourceId);
    d_incomingQueue_p->pushBack(msg);
}

void btemt_ChannelQueuePool::dataCb(int           *numConsumed,
                                    int           *minAdditional,
                                    btemt_DataMsg  dataMsg,
                                    void          *)
{
    BSLS_ASSERT(numConsumed);
    BSLS_ASSERT(minAdditional);

    *numConsumed = 0;
    int msgSize, numNeeded;

    bcema_PooledBufferChain *chain = dataMsg.data();
    int bufferSize   = chain->bufferSize();
    int offset       = 0;
    int bufferIdx    = 0;
    int bufferOffset = 0;

    int lookahead    = (1 < chain->numBuffers())
                     ? bufferSize
                     : chain->length();

    bcema_PooledBufferChainFactory *factory = d_channelPool_p->
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
                btemt_Message msg(btemt_Message::BTEMT_DATA);
                btemt_DataMsg& dt = msg.dataMsg();
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

            bcema_PooledBufferChain *newChain = factory->allocate(0);
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

void btemt_ChannelQueuePool::timerCb(int clockId) {
    btemt_Message msg(btemt_Message::BTEMT_TIMER);
    btemt_TimerMsg& data = msg.timerMsg();
    data.setTimerId(clockId);
    d_incomingQueue_p->pushBack(msg);
}

// CREATORS
btemt_ChannelQueuePool::btemt_ChannelQueuePool(
        bcec_Queue<btemt_Message>             *incomingQueue,
        bcec_Queue<btemt_Message>             *outgoingQueue,
        ParseMessagesCallback                  callback,
        const btemt_ChannelPoolConfiguration&  parameters,
        bslma_Allocator                       *basicAllocator)
: d_incomingQueue_p(incomingQueue)
, d_outgoingQueue_p(outgoingQueue)
, d_userCallback(callback)
, d_minIncomingMessageSize(parameters.minIncomingMessageSize())
, d_workTimeout(5.0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    d_channelCbFunctor
        = btemt_ChannelPool::ChannelStateChangeCallback(
                bdef_MemFnUtil::memFn( &btemt_ChannelQueuePool::channelStateCb
                                     , this)
              , basicAllocator);

    d_dataCbFunctor
        = btemt_ChannelPool::DataReadCallback(
                bdef_MemFnUtil::memFn(&btemt_ChannelQueuePool::dataCb, this)
              , basicAllocator);

    d_poolCbFunctor
        = btemt_ChannelPool::PoolStateChangeCallback(
                bdef_MemFnUtil::memFn( &btemt_ChannelQueuePool::poolStateCb
                                     , this)
              , basicAllocator);

    d_channelPool_p =
        new (*d_allocator_p) btemt_ChannelPool(d_channelCbFunctor,
                                               d_dataCbFunctor,
                                               d_poolCbFunctor,
                                               parameters, basicAllocator);
    bces_AtomicUtil::initInt(&d_runningFlag, 0);
}

btemt_ChannelQueuePool::~btemt_ChannelQueuePool() {
    d_allocator_p->deleteObjectRaw(d_channelPool_p);
}

extern "C" void *queueProc(void *arg) {
    btemt_ChannelQueuePool *qp = (btemt_ChannelQueuePool*)arg;
    return (void*)qp->processOutgoingQueue();
}

// MANIPULATORS
int btemt_ChannelQueuePool::start() {
    d_channelPool_p->start();
    bcemt_Attribute attributes;
    attributes.setDetachedState(bcemt_Attribute::BCEMT_CREATE_JOINABLE);
    bces_AtomicUtil::setInt(&d_runningFlag, 1);
    return bcemt_ThreadUtil::create(&d_processorHandle, attributes,
                                    &queueProc, (void*)this);
}

int btemt_ChannelQueuePool::stop() {
    d_channelPool_p->stop();
    bces_AtomicUtil::setInt(&d_runningFlag, 0);
    return bcemt_ThreadUtil::join(d_processorHandle);
}

int btemt_ChannelQueuePool::processOutgoingQueue() {
    while(bces_AtomicUtil::getInt(d_runningFlag)) {
        btemt_Message msg(btemt_Message::BTEMT_DATA);
        int s = d_outgoingQueue_p->timedPopFront(&msg,
                                      bdetu_SystemTime::now() + d_workTimeout);

        if (s) {
            continue;
        }

        if (btemt_Message::BTEMT_BLOB == msg.type()) {
            const btemt_BlobMsg& data = msg.blobMsg();
            if (0 != d_channelPool_p->write(data.channelId(), data)) {
                btemt_Message msg(btemt_Message::BTEMT_CHANNEL_STATE);
                btemt_ChannelMsg& cs = msg.channelMsg();
                cs.setChannelId(data.channelId());
                cs.setChannelState(btemt_ChannelMsg::BTEMT_CHANNEL_DOWN);
                cs.setAllocatorId(-1);
                // Then what do we do with msg???
            }
        } else {
            BSLS_ASSERT(btemt_Message::BTEMT_DATA == msg.type());
            const btemt_DataMsg& data = msg.dataMsg();
            /*
              printf("QP Sending %d bytes to %d\n",
              data.data()->length(), data.channelId());
            */
            if (0 != d_channelPool_p->write(data.channelId(), data)) {
                btemt_Message msg(btemt_Message::BTEMT_CHANNEL_STATE);
                btemt_ChannelMsg& cs = msg.channelMsg();
                cs.setChannelId(data.channelId());
                cs.setChannelState(btemt_ChannelMsg::BTEMT_CHANNEL_DOWN);
                cs.setAllocatorId(-1);
                // Then what do we do with msg???
            }
        }
    }
    return 0;
}

int btemt_ChannelQueuePool::registerClock(const bdet_TimeInterval& startTime,
                                          const bdet_TimeInterval& period,
                                          int clockId) {
    bdef_Function<void (*)()> cb(
            bdef_BindUtil::bindA(
                d_allocator_p
              , &btemt_ChannelQueuePool::timerCb
              , this
              , clockId));

    return d_channelPool_p->registerClock(cb, startTime, period, clockId);
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
