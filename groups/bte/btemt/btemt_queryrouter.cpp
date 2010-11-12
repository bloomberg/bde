 // btemt_queryrouter.cpp                  -*-C++-*-
#include <btemt_queryrouter.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_queryrouter_cpp,"$Id$ $CSID$")

#include <btemt_channelpoolconfiguration.h>
#include <btemt_queryxferutil.h>

#include <bcema_sharedptr.h>
#include <bcema_pooledbufferchain.h>

#include <bdef_function.h>
#include <bdef_memfn.h>

#include <bdem_list.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_platformutil.h>

#include <bteso_ipv4address.h>

#include <bsl_vector.h>
#include <bsl_map.h>

namespace {

inline
int getProcessorId(int channelId)
// Convert a btemt_QueryRouter-managed 'channelId' to a
// btemt_QueryRouter-managed processorId.  This processorId here is
// different from the btemt_QueryProcessor's property:
// btemt_QueryProcessor::d_processorId.  This processorId is known to the
// btemt_QueryRouter and its user.
{
    return channelId;
}

inline
int getChannelId(int processorId)
// Convert to a btemt_QueryRouter-managed channelId from a
// btemt_QueryRouter-managed processorId.  This processorId here is different
// from the btemt_QueryProcessor's property:
// btemt_QueryProcessor::d_processorId.  This processorId is known to the
// btemt_QueryRouter and its user
{
    return processorId;
}

}  // close unnamed namespace

namespace BloombergLP {

                        // =======================
                        // class btemt_QueryRouter
                        // =======================

                        // ========
                        // CREATORS
                        // ========
btemt_QueryRouter::btemt_QueryRouter(
    const btemt_ChannelPoolConfiguration& config,
    const bdef_Function<void (*)(const bcema_SharedPtr<btemt_QueryResponse>&)>&
        responseFunctor,
    const bdef_Function<void (*)(btemt_Message::MessageType, int,
                                 int, const bteso_IPv4Address&, void *)>&
        eventFunctor,
    bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_channelAllocatorId(0)
, d_channelAllocators(bsl::less<int>(), d_allocator_p)
, d_factory(config.maxOutgoingMessageSize(), d_allocator_p)
, d_responseFunctor(responseFunctor)
, d_eventFunctor(eventFunctor)
{
    bdef_Function<void (*)(int, int, int, void*)> channelStateFunctor(
            bdef_MemFnUtil::memFn(&btemt_QueryRouter::channelCb, this)
          , d_allocator_p);

    bdef_Function<void (*)(int, int, int)> poolStateFunctor(
            bdef_MemFnUtil::memFn(&btemt_QueryRouter::poolCb, this)
          , d_allocator_p);

    bdef_Function<void (*)(int *, int *, const btemt_DataMsg&, void *)>
        dataFunctor( bdef_MemFnUtil::memFn(&btemt_QueryRouter::dataCb, this)
                   , d_allocator_p);

    d_channelPool_p =
        new (*d_allocator_p) btemt_ChannelPool(channelStateFunctor,
                                               dataFunctor,
                                               poolStateFunctor,
                                               config,
                                               d_allocator_p);
}

btemt_QueryRouter::~btemt_QueryRouter()
{
    stop();
    d_allocator_p->deleteObjectRaw(d_channelPool_p);
}

                        // =========
                        // CALLBACKS
                        // =========
void btemt_QueryRouter::poolCb(int state, int, int)
{
    d_eventFunctor(btemt_Message::BTEMT_POOL_STATE, state,
                -1, bteso_IPv4Address(), 0);
    // the last three arguments are all dummy values
}

void btemt_QueryRouter::channelCb(int channelId, int, int state, void *)
{
    bteso_IPv4Address peer;
    d_channelPool_p->getPeerAddress(&peer, channelId);
    d_eventFunctor(btemt_Message::BTEMT_CHANNEL_STATE, state,
                   getProcessorId(channelId), peer, 0);
}

void btemt_QueryRouter::dataCb(int                  *consumed,
                               int                  *needed,
                               const btemt_DataMsg&  data,
                               void *)
{
    btemt_QueryXferUtil::parseQueryResponse(d_responseFunctor,
                                            consumed, needed,
                                            data, d_allocator_p);
}

                        // ============
                        // MANIPULATORS
                        // ============
int btemt_QueryRouter::query(const btemt_Query& query,
                             bsls_PlatformUtil::Int64 queryId,
                             int processorId)
{
    int channelId = getChannelId(processorId);
    bcema_PooledBufferChain *chain = d_factory.allocate(0);
    btemt_DataMsg serialized(chain, &d_factory, channelId, d_allocator_p);
    btemt_QueryXferUtil::serializeQuery(&serialized, query, queryId);
    return d_channelPool_p->write(channelId, serialized);
}

                        // =========
                        // ACCESSORS
                        // =========
void btemt_QueryRouter::getProcessorAddress(bteso_IPv4Address *address,
                                            int processorId) const
{
    d_channelPool_p->getPeerAddress(address,
                                    getChannelId(processorId));
}

                        // ==================
                        // CHANNEL MANAGEMENT
                        // ==================
int btemt_QueryRouter::connect(const bteso_IPv4Address& address,
                               const bdet_TimeInterval& timeout)
{
    int sourceId = d_channelAllocatorId++;
//    d_channelAllocatorsLock.lock();
//    d_channelAllocators.insert(bsl::make_pair(sourceId, address));
//    d_channelAllocatorsLock.unlock();

    int rc =  d_channelPool_p->connect(address, 1, timeout,
                                       sourceId);

//    if (rc) {
//        d_channelAllocatorsLock.lock();
//        d_channelAllocators.erase(d_channelAllocators.find(sourceId));
//        d_channelAllocatorsLock.unlock();
//    }
    return rc;
}

int btemt_QueryRouter::listen(const bteso_IPv4Address& address,
                              int queueLength)
{
    int sourceId = d_channelAllocatorId++;
//    d_channelAllocatorsLock.lock();
//    d_channelAllocators.insert(bsl::make_pair(sourceId, address));
//    d_channelAllocatorsLock.unlock();

    int rc = d_channelPool_p->listen(address, queueLength, sourceId);

//    if (rc) {
//        d_channelAllocatorsLock.lock();
//        d_channelAllocators.erase(d_channelAllocators.find(sourceId));
//        d_channelAllocatorsLock.unlock();
//    }
    return rc;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
