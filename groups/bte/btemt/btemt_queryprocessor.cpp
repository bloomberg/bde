// btemt_queryprocessor.cpp                                           -*-C++-*-
#include <btemt_queryprocessor.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_queryprocessor_cpp,"$Id$ $CSID$")

#include <btemt_queryxferutil.h>
#include <btemt_message.h>
#include <btemt_channelpool.h>
#include <btemt_channelpoolconfiguration.h>

#include <bcema_sharedptr.h>
#include <bcep_threadpool.h>

#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_memfn.h>
#include <bdef_placeholder.h>

#include <bdet_timeinterval.h>

#include <bslma_rawdeleterproctor.h>
#include <bsls_assert.h>

#include <bsl_cstdio.h>

namespace BloombergLP {

using bdef_PlaceHolders::_1;
using bdef_PlaceHolders::_2;
using bdef_PlaceHolders::_3;
using bdef_PlaceHolders::_4;

static btemt_ChannelPoolConfiguration
   createChannelPoolConfiguration(const btemt_QueryProcessorConfiguration&
                                  configuration)
   {
       btemt_ChannelPoolConfiguration result;
       result.setIncomingMessageSizes(4, 4,
                                      configuration.incomingMessageSize());

       result.setOutgoingMessageSizes(4, 4,
                                      configuration.outgoingMessageSize());

       result.setMaxConnections(configuration.maxConnections());

       result.setMaxThreads(configuration.maxIoThreads());

       result.setMaxWriteCache(configuration.maxWriteCache());

       result.setReadTimeout(configuration.readTimeout());

       result.setMetricsInterval(configuration.metricsInterval());
       return result;
   }

bsl::ostream& operator<<(bsl::ostream& stream,
                         const btemt_QueryProcessorEvent& rhs)
{
    switch(rhs.eventType()) {
    case btemt_QueryProcessorEvent::BTEMT_CONNECT_FAILED: {
        stream << "(CONNECT_FAILED,, ";
    } break;
    case btemt_QueryProcessorEvent::BTEMT_PARSE_ERROR: {
        stream << "(PARSE_ERROR, ";
    } break;
    case btemt_QueryProcessorEvent::BTEMT_DISPATCHER_UP: {
        stream << "(DISPATCHER_UP, ";
    } break;
    case btemt_QueryProcessorEvent::BTEMT_DISPATCHER_DOWN: {
        stream << "(DISPATCHER_DOWN, ";
    } break;
    case btemt_QueryProcessorEvent::BTEMT_QUERY_DISCARDED: {
        stream << "(QUERY_DISCARDED, ";
    } break;
    case btemt_QueryProcessorEvent::BTEMT_QUERY_SUBMITTED: {
        stream << "(QUERY_SUBMITTED, ";
    } break;
    case btemt_QueryProcessorEvent::BTEMT_UNMATCHED_RESPONSE: {
        stream << "(DISPATCHER_DOWN, ";
    } break;
    case btemt_QueryProcessorEvent::BTEMT_UNKNOWN: {
        stream << "(UNKNOWN, ";
    } break;
    default: {
        BSLS_ASSERT("Unhandled message type" && 0);
    }
    }

    stream << rhs.processorId() << ", "
           << rhs.processorAddress() << ", "
           << rhs.userData() << ")" << bsl::flush;

    return stream;
}

void btemt_QueryProcessor::poolCb(int type, int sourceId, int)
{
    switch (type) {
      case btemt_PoolMsg::BTEMT_ACCEPT_TIMEOUT: {
          bsl::printf("ACCEPT_TIMEOUT on %d\n", sourceId);
      } break;
      case btemt_PoolMsg::BTEMT_ERROR_ACCEPTING: {
          bsl::printf("ERROR_ACCEPTING to %d\n", sourceId);
      } break;
      case btemt_PoolMsg::BTEMT_ERROR_CONNECTING: {
          bsl::printf("ERROR_CONNECTING to %d\n", sourceId);
          d_channelAllocatorsLock.lock();
          btemt_QueryProcessorEvent event;
          event.setProcessorId(sourceId);
          event.setProcessorAddress(d_channelAllocators[sourceId]);
          event.setEventType(btemt_QueryProcessorEvent::BTEMT_CONNECT_FAILED);
          d_channelAllocatorsLock.unlock();
          d_eventFunctor(event);
      } break;
      case btemt_PoolMsg::BTEMT_CHANNEL_LIMIT: {
          bsl::printf(
                   "CHANNEL_LIMIT reached. Current number of channels is %d\n",
                   d_channelPool_p->numChannels());
      } break;
      case btemt_PoolMsg::BTEMT_CAPACITY_LIMIT: {
          bsl::printf("CAPACITY_LIMIT reached.");
      } break;
    }
}

void btemt_QueryProcessor::channelCb(int channel, int, int type, void *)
{
    switch (type) {
      case btemt_ChannelMsg::BTEMT_READ_TIMEOUT: {
          bsl::printf("READ_TIMEOUT on %d\n", channel);
      } break;
      case btemt_ChannelMsg::BTEMT_SEND_BUFFER_FULL: {
          bsl::printf("SEND_BUFFER_FULL to %d\n", channel);
      } break;
      case btemt_ChannelMsg::BTEMT_CHANNEL_DOWN: {
          bsl::printf("CHANNEL_DOWN on %d\n", channel);
          d_channelPool_p->shutdown(channel,
                                    btemt_ChannelPool::BTEMT_IMMEDIATE);
          btemt_QueryProcessorEvent event;
          event.setProcessorId(channel);
          bteso_IPv4Address address;
          d_channelPool_p->getPeerAddress(&address, channel);
          event.setProcessorAddress(address);
          event.setEventType(btemt_QueryProcessorEvent::BTEMT_DISPATCHER_DOWN);
          d_eventFunctor(event);
      } break;
      case btemt_ChannelMsg::BTEMT_CHANNEL_UP: {
          bsl::printf("CHANNEL_UP on %d\n", channel);
          btemt_QueryProcessorEvent event;
          event.setProcessorId(channel);
          bteso_IPv4Address address;
          d_channelPool_p->getPeerAddress(&address, channel);
          event.setProcessorAddress(address);
          event.setEventType(btemt_QueryProcessorEvent::BTEMT_DISPATCHER_UP);
          d_eventFunctor(event);
      } break;
      case btemt_ChannelMsg::BTEMT_MESSAGE_DISCARDED: {
          bsl::printf("MESSAGE_DISCARDED on %d\n", channel);
          btemt_QueryProcessorEvent event;
          event.setProcessorId(channel);
          bteso_IPv4Address address;
          d_channelPool_p->getPeerAddress(&address, channel);
          event.setProcessorAddress(address);
          event.setEventType(btemt_QueryProcessorEvent::BTEMT_QUERY_DISCARDED);
          d_eventFunctor(event);
      } break;
    }
}

void btemt_QueryProcessor::dataCb(int                  *consumed,
                                  int                  *needed,
                                  const btemt_DataMsg&  data,
                                  void *)
{
    bdef_Function<void (*)(const bcema_SharedPtr<btemt_QueryRequest>&)>
        processQueryRequestFunctor(
                bdef_BindUtil::bindA(
                    d_allocator_p
                  , bdef_MemFnUtil::memFn(
                          &btemt_QueryProcessor::processQueryRequestCb
                        , this)
                  , _1
                  , data.channelId()));

    btemt_QueryXferUtil::parseQueryRequest(processQueryRequestFunctor,
                                           consumed, needed, data,
                                           d_allocator_p);
        // Each parsed packet of btemt_QueryRequest will be processed by
        // processQueryRequestFunctor
}

void btemt_QueryProcessor::processQueryRequestCb(
    bcema_SharedPtr<btemt_QueryRequest> queryRequest, int channelId)
{
    bdef_Function<void (*)()> procFunctor(
            bdef_BindUtil::bindA( d_allocator_p
                                , bdef_MemFnUtil::memFn(
                                    &btemt_QueryProcessor::serverFunctorWrapCb
                                  , this)
                                , queryRequest
                                , channelId));

    d_threadPool_p->enqueueJob(procFunctor);
}

void btemt_QueryProcessor::serverFunctorWrapCb(
    bcema_SharedPtr<btemt_QueryRequest> queryRequest, int channelId)
{
    d_serverFunctor(queryRequest.ptr(),
                    bdef_BindUtil::bind(
                        &btemt_QueryProcessor::writeQueryResultCb, this,
                        _1, channelId, queryRequest->queryId()));
}

void btemt_QueryProcessor::writeQueryResultCb(
    btemt_QueryResult *queryResult,
    int channelId,
    const bsls_PlatformUtil::Int64& queryId)
{
    btemt_DataMsg msg(d_outFactory.allocate(0),
                      &d_outFactory,
                      channelId);

    btemt_QueryXferUtil::serializeQueryResult(
                                           &msg,
                                           *queryResult,
                                           queryId,
                                           btemt_QueryResponse::BTEMT_SUCCESS);

    if (d_channelPool_p->write(channelId, msg)) {
        bsl::printf("Can't send response to %d\n", channelId);
        btemt_QueryProcessorEvent event;
        event.setProcessorId(channelId);
        bteso_IPv4Address address;
        d_channelPool_p->getPeerAddress(&address, channelId);
        event.setProcessorAddress(address);
        event.setEventType(btemt_QueryProcessorEvent::BTEMT_QUERY_DISCARDED);
        d_eventFunctor(event);
    }
}

// CREATORS
btemt_QueryProcessor::btemt_QueryProcessor(
            const btemt_QueryProcessorConfiguration&        configuration,
            const bdef_Function<void (*)(const btemt_QueryProcessorEvent&)>&
                eventFunctor,
            const bdef_Function<void (*)(
                btemt_QueryRequest*,
                bdef_Function<void (*)(btemt_QueryResult*)>)>&
                serverFunctor,
            bslma_Allocator *basicAllocator)
: d_threadPool_p(NULL)
, d_externalThreadPoolFlag(0)
, d_channelPool_p(NULL)
, d_outFactory(configuration.outgoingMessageSize(), basicAllocator)
, d_eventFunctor(eventFunctor, basicAllocator)
, d_serverFunctor(serverFunctor, basicAllocator)
, d_channelAllocatorId(0)
, d_channelAllocators(bsl::less<int>(), basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    bdef_Function<void (*)(int, int, int)> poolEventFunctor(
            bdef_MemFnUtil::memFn(&btemt_QueryProcessor::poolCb, this)
          , d_allocator_p);

    bdef_Function<void (*)(int, int, int, void*)> channelEventFunctor(
            bdef_MemFnUtil::memFn(&btemt_QueryProcessor::channelCb, this)
          , d_allocator_p);

    bdef_Function<void (*)(int*, int*, const btemt_DataMsg&, void*)>
        dataEventFunctor(
                bdef_MemFnUtil::memFn(&btemt_QueryProcessor::dataCb, this)
              , d_allocator_p);

    d_threadPool_p = new (*d_allocator_p)
        bcep_ThreadPool(bcemt_Attribute(),
                        configuration.minProcessingThreads(),
                        configuration.maxProcessingThreads(),
                        configuration.idleTimeout());

    bslma_RawDeleterProctor<bcep_ThreadPool, bslma_Allocator>
        deleter(d_threadPool_p, d_allocator_p);

    d_channelPool_p = new (*d_allocator_p)
        btemt_ChannelPool(channelEventFunctor,
                          dataEventFunctor,
                          poolEventFunctor,
                          createChannelPoolConfiguration(configuration),
                          basicAllocator);
    deleter.release();
    BSLS_ASSERT(d_threadPool_p);
    BSLS_ASSERT(d_channelPool_p);
}

btemt_QueryProcessor::btemt_QueryProcessor(
            const btemt_QueryProcessorConfiguration&        configuration,
            bcep_ThreadPool                                *procPool,
            const bdef_Function<void (*)(const btemt_QueryProcessorEvent&)>&
                eventFunctor,
            const bdef_Function<void (*)(
                btemt_QueryRequest*,
                bdef_Function<void (*)(btemt_QueryResult*)>)>&
                serverFunctor,
            bslma_Allocator *basicAllocator)
: d_threadPool_p(procPool)
, d_externalThreadPoolFlag(1)
, d_channelPool_p(NULL)
, d_outFactory(configuration.outgoingMessageSize(), basicAllocator)
, d_eventFunctor(eventFunctor, basicAllocator)
, d_serverFunctor(serverFunctor, basicAllocator)
, d_channelAllocatorId(0)
, d_channelAllocators(bsl::less<int>(), basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    bdef_Function<void (*)(int, int, int)> poolEventFunctor(
            bdef_MemFnUtil::memFn(&btemt_QueryProcessor::poolCb, this)
          , d_allocator_p);

    bdef_Function<void (*)(int, int, int, void*)> channelEventFunctor(
            bdef_MemFnUtil::memFn(&btemt_QueryProcessor::channelCb, this)
          , d_allocator_p);

    bdef_Function<void (*)(int*, int*, const btemt_DataMsg&, void*)>
        dataEventFunctor(
                bdef_MemFnUtil::memFn(&btemt_QueryProcessor::dataCb, this)
              , d_allocator_p);

    d_channelPool_p = new (*d_allocator_p)
        btemt_ChannelPool(channelEventFunctor,
                          dataEventFunctor,
                          poolEventFunctor,
                          createChannelPoolConfiguration(configuration),
                          basicAllocator);
    BSLS_ASSERT(d_threadPool_p);
    BSLS_ASSERT(d_channelPool_p);

}

btemt_QueryProcessor::~btemt_QueryProcessor() {
    this->stop();
    if (d_externalThreadPoolFlag) {
        d_allocator_p->deleteObjectRaw(d_threadPool_p);
    }
    d_allocator_p->deleteObjectRaw(d_channelPool_p);
}

// MANIPULATORS
// Threads management
void btemt_QueryProcessor::start() {
    d_channelPool_p->start();
    d_threadPool_p->start();
}

void btemt_QueryProcessor::stop() {
    d_threadPool_p->stop();
    d_channelPool_p->stop();
}

// Channel management
int btemt_QueryProcessor::listen(const bteso_IPv4Address& address,
                                 int queueLength)
{
    int sourceId = d_channelAllocatorId++;
    d_channelAllocatorsLock.lock();
    d_channelAllocators.insert(bsl::make_pair(sourceId, address));
    d_channelAllocatorsLock.unlock();

    int rc = d_channelPool_p->listen(address, queueLength, sourceId);

    if (rc) {
        d_channelAllocatorsLock.lock();
        d_channelAllocators.erase(d_channelAllocators.find(sourceId));
        d_channelAllocatorsLock.unlock();
    }
    return rc;
}

int btemt_QueryProcessor::connect(const bteso_IPv4Address& address,
                                  const bdet_TimeInterval& timeout)
{

    int sourceId = d_channelAllocatorId++;
    d_channelAllocatorsLock.lock();
    d_channelAllocators.insert(bsl::make_pair(sourceId, address));
    d_channelAllocatorsLock.unlock();

    int rc =  d_channelPool_p->connect(address, 1, timeout,
                                       sourceId);

    if (rc) {
        d_channelAllocatorsLock.lock();
        d_channelAllocators.erase(d_channelAllocators.find(sourceId));
        d_channelAllocatorsLock.unlock();
    }
    return rc;
}

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
