 // btemt_querydispatcher.cpp                  -*-C++-*-
#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_querydispatcher_cpp,"$Id$ $CSID$")

#include <bsls_platform.h>

// TBD: Determine whether or not this is a deliberately non-standard include
// order?  This was introduced in "btemt_querydispatcher.cpp@@/main/bb/dev/14"
#ifdef BSLS_PLATFORM__OS_LINUX
#define __STDC_LIMIT_MACROS 1
#endif

#include <btemt_querydispatcher.h>
#include <btemt_querydispatcherconfiguration.h>
#include <btemt_channelpool.h>
#include <btemt_message.h>

#include <bcema_sharedptr.h>

#include <bcemt_thread.h>
#include <bces_atomictypes.h>
#include <bces_atomicutil.h>
#include <bcemt_lockguard.h>

#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_memfn.h>
#include <bdef_placeholder.h>

#include <bsls_platformutil.h>
#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_set.h>
#include <bsl_utility.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_climits.h>
#include <bsl_cstdio.h>

#ifdef BSLS_PLATFORM__OS_HPUX
#include <stdint.h>  // INT64_MIN
#endif

namespace {

using namespace BloombergLP;

void splitQueryId(int *handle, int *uniqueId,
                  const bsls_PlatformUtil::Int64& queryId)
{
    typedef bsls_PlatformUtil::Int64 Int64;

    if (handle) *handle = (int)(queryId >> 32); // signed shift
    if (uniqueId) *uniqueId = (int)(queryId & (Int64)0x00000000FFFFFFFF);
}

bsls_PlatformUtil::Int64 makeQueryId(int handle, int uniqueId)
{
    typedef bsls_PlatformUtil::Int64 Int64;

    Int64 queryId = handle;
    queryId <<= 32;
    queryId |= (Int64)uniqueId & (Int64)0x00000000FFFFFFFF;
    return queryId;
}

} // close anonymous namespace

namespace BloombergLP {

using bdef_PlaceHolders::_1;

                        // ================================
                        // class btemt_QueryDispatcherEvent
                        // ================================

bool operator==(const btemt_QueryDispatcherEvent& lhs,
                const btemt_QueryDispatcherEvent& rhs)
{
    return
        lhs.processorId() == rhs.processorId() &&
        lhs.processorAddress() == rhs.processorAddress() &&
        lhs.userData() == rhs.userData() &&
        lhs.eventType() == rhs.eventType();
}

bool operator!=(const btemt_QueryDispatcherEvent& lhs,
                const btemt_QueryDispatcherEvent& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& operator<<(bsl::ostream& os,
                         const btemt_QueryDispatcherEvent& rhs)
{
    os << "{processorId = " << rhs.processorId()
       << ", address =  " << rhs.processorAddress()
       << ", user data = " << rhs.userData()
       << ", type = ";
    switch (rhs.eventType()) {
      case btemt_QueryDispatcherEvent::BTEMT_CONNECT_FAILED:
        os << "CONNECT_FAILED";
        break;
      case btemt_QueryDispatcherEvent::BTEMT_PARSE_ERROR:
        os << "PARSE_ERROR";
        break;
      case btemt_QueryDispatcherEvent::BTEMT_PROCESSOR_UP:
        os << "PROCESSOR_UP";
        break;
      case btemt_QueryDispatcherEvent::BTEMT_PROCESSOR_DOWN:
        os << "PROCESSOR_DOWN";
        break;
      case btemt_QueryDispatcherEvent::BTEMT_QUERY_DISCARDED:
        os << "QUERY_DISCARDED";
        break;
      case btemt_QueryDispatcherEvent::BTEMT_QUERY_SUBMITTED:
        os << "QUERY_SUBMITTED";
        break;
      case btemt_QueryDispatcherEvent::BTEMT_UNMATCHED_RESPONSE:
        os << "UNMATCHED_RESPONSE";
        break;
      default: {
          os << "UNKNOWN_TYPE";
      }
    }
    os << "}";
    return os;
}

                        // ===========================
                        // class btemt_QueryDispatcher
                        // ===========================

enum { PSEUDO_INFINITE_PERIOD = 1000000 }; // an infinite period in seconds

static btemt_ChannelPoolConfiguration
createChannelPoolConfiguration(const btemt_QueryDispatcherConfiguration&
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

                        // ========
                        // CREATORS
                        // ========
btemt_QueryDispatcher::btemt_QueryDispatcher(
    const btemt_QueryDispatcherConfiguration& config,
    const bcemt_Attribute& threadAttributes,
    const bdef_Function<void (*)(const btemt_QueryDispatcherEvent&)>&
        dispatcherEventFunctor,
    bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
, d_threadPool(threadAttributes,
               config.minProcessingThreads(), config.maxProcessingThreads(),
               (int)config.idleTimeout(),
               d_allocator_p)
, d_queryRouter_p(NULL)
, d_channels(d_allocator_p)
, d_dispatcherEventFunctor(dispatcherEventFunctor, basicAllocator)
//, d_atomicClock(INIT_ID)
, d_atomicId(BTEMT_INIT_ID)
, d_queryCount(0)
, d_successCount(0)
, d_cancelCount(0)
, d_timeoutCount(0)
, d_sumElapsedResponseTime(0LL)
, d_queryCatalog(d_allocator_p)
, d_queryMap(bsl::less<bsl::pair<void *, Int64> >(), d_allocator_p)
{
    bdef_Function<void (*)(const bcema_SharedPtr<btemt_QueryResponse>&)>
        responseFunctor(
                bdef_MemFnUtil::memFn(&btemt_QueryDispatcher::responseCb, this)
              , d_allocator_p);

    bdef_Function<void (*)( btemt_Message::MessageType
                          , int
                          , int
                          , const bteso_IPv4Address&
                          , void *)>
        eventFunctor(
                bdef_MemFnUtil::memFn(&btemt_QueryDispatcher::eventCb, this)
              , d_allocator_p);

    d_queryRouter_p = new (*d_allocator_p)
        btemt_QueryRouter(createChannelPoolConfiguration(config),
                          responseFunctor,
                          eventFunctor,
                          basicAllocator);
}

btemt_QueryDispatcher::~btemt_QueryDispatcher()
{
    d_allocator_p->deleteObjectRaw(d_queryRouter_p);
}

                        // =========
                        // CALLBACKS
                        // =========
void btemt_QueryDispatcher::timeoutCb(bsls_PlatformUtil::Int64 queryId)
{
//    bsl::printf("timeoutCb queryId = %lld\n", queryId);
    bcema_SharedPtr<Entry> entry;
    int handle, uniqueId;
    splitQueryId(&handle, &uniqueId, queryId);

    if (d_queryCatalog.find(handle, &entry)) {
        // This queryId is canceled or all responses for this queryId
        // have been processed and removed.
        return;
    }
    bcemt_LockGuard<bcemt_Mutex> guard(&entry->d_mutex);
    if (uniqueId != entry->d_uniqueId) {
        // either the found entry is not for this query, or this thread is
        // too late finding the entry for this query.  (another thread
        // found it first and reset it to BTEMT_UNSET_ID)
        return;
    }

    // a response is not yet received since clock was last set
    bcema_SharedPtr<btemt_QueryResponse> timeoutResponse;
    timeoutResponse.createInplace(d_allocator_p);
    // no need to call timeoutResponse->setQueryResponse()/setSequenceNum()
    timeoutResponse->setQueryId(queryId);
    timeoutResponse->setStatus(btemt_QueryResponse::BTEMT_TIMEOUT);

    entry->d_enqueueV1Functor(timeoutResponse);
        // Rather than removing it by this channel pool thread, enqueue a
        // functor to the threadpool.  The enqueued functor will remove the
        // entry and deregister any associated clock
}

void btemt_QueryDispatcher::postFunctorCb(
    bcema_SharedPtr<btemt_QueryResponse> response,
    const bdef_Function<void (*)(const bcema_SharedPtr<btemt_QueryResponse>&)>&
        dispatcherV1Functor)
{
//    bsl::printf("postFunctorCb queryId = %lld\n", response->queryId());

    bdef_Function<void (*)()> myOwnV0Functor(
            bdef_BindUtil::bindA( d_allocator_p
                                , dispatcherV1Functor
                                , response));

    d_threadPool.enqueueJob(myOwnV0Functor);
}

void btemt_QueryDispatcher::responseCb(
    bcema_SharedPtr<btemt_QueryResponse> response)
{
    Int64 queryId = response->queryId();
    bcema_SharedPtr<Entry> entry;
    int handle, uniqueId;
    splitQueryId(&handle, &uniqueId, queryId);

//    bsl::printf("responseCb 1: queryId = %lld, h = %d, u = %d\n",
//                queryId, handle, uniqueId);

    if (d_queryCatalog.find(handle, &entry)) {
        // either the queryId was never there or canceled,
        // or response was received after the queryId was removed upon timeout
        return;
    }

//    bsl::printf("responseCb 2: queryId = %lld, h = %d, u = %d\n",
//                queryId, handle, uniqueId);

    bcemt_LockGuard<bcemt_Mutex> guard(&entry->d_mutex);
    if (uniqueId != entry->d_uniqueId) {
        // either the found entry is not for this query, or this thread is
        // too late finding the entry for this query.  (another thread
        // found it first and reset it to BTEMT_UNSET_ID)
        return;
    }
//    bsl::printf("responseCb 3: queryId = %lld\n", response->queryId());
    entry->d_enqueueV1Functor(response);
}

void btemt_QueryDispatcher::dispatcherV1Cb(
    bcema_SharedPtr<btemt_QueryResponse> response,
    const bdef_Function<void (*)(int *, btemt_QueryResponse*)>& functor)
{
    Int64 queryId = response->queryId();
    int handle, uniqueId;
    splitQueryId(&handle, &uniqueId, queryId);
//    bsl::printf("dispatcherV1Cb handle = %d, uniqueId = %d\n",
//                handle, uniqueId);
    bcema_SharedPtr<Entry> entry;

    if (d_queryCatalog.find(handle, &entry)) {
        return;
    }
    bcemt_LockGuard<bcemt_Mutex> guard(&entry->d_mutex);

    if (uniqueId != entry->d_uniqueId) {
        return;
    }

    int isFinalResponse;
    // TBD: a decision here is that user functor will examine the response
    // even if it is a TIMEOUT generated response.  User functor can
    // be skipped for TIMEOUT if it's decided so
    functor(&isFinalResponse, response.ptr());

    if (isFinalResponse ||
        btemt_QueryResponse::BTEMT_TIMEOUT == response->status()) {

        BSLS_ASSERT(0 == d_queryCatalog.remove(handle, &entry));

        entry->d_uniqueId = BTEMT_UNSET_ID;

        if (BTEMT_NULL_CLOCK != entry->d_clockId) {
            deregisterTimer(entry->d_clockId);
            // No need to check for return value.  If deregister fails, the
            // dispatcherV1Cb invoked at the next timeout will find this entry
            // has been removed.
        } // else { no clock was registered for this queryId }
        if (isFinalResponse) {
            bdet_TimeInterval elapsedTime =
                bdetu_SystemTime::now() - entry->d_startTime;
            // convert elapsedTime(sec.nanosec) into microseconds
            bsls_PlatformUtil::Int64 microSeconds =
                elapsedTime.seconds() * 1000000 +
                elapsedTime.nanoseconds() / 1000;
            d_sumElapsedResponseTime += microSeconds;
            ++d_successCount;
//            bsl::printf("dispatcherV1Cb final queryId = %lld\n", queryId);
        }
        else { // timeout
            ++d_timeoutCount;
//            bsl::printf("dispatcherV1Cb timeout\n");
        }

        if (0 != entry->d_category) {
            d_mapLock.lock();
            d_queryMap.erase(
                bsl::make_pair<void*, Int64>(entry->d_category, queryId));
            d_mapLock.unlock();
        }
    }
    else { // !isFinalResponse && !TIMEOUT
        if (BTEMT_NULL_CLOCK != entry->d_clockId) {
            reregisterTimer(entry->d_clockId,
                            bdetu_SystemTime::now() + entry->d_timeout);
            // Reuse original timeoutFunctor passing in original queryId.
            // No need to check return value.
            // if it fails because the timer has just been dispatched or about
            // to be dispatched, this query will end up with a timeout
        }
    }
}

void btemt_QueryDispatcher::eventCb(btemt_Message::MessageType type,
                                    int state,
                                    int processorId,
                                    const bteso_IPv4Address& processorAddress,
                                    void *userData)
{
    btemt_QueryDispatcherEvent event(processorId, userData);
    event.setProcessorAddress(processorAddress);
    switch (type) {
      case btemt_Message::BTEMT_POOL_STATE: {
          switch (state) {
            case btemt_PoolMsg::BTEMT_ERROR_CONNECTING: {
                event.setEventType(
                        btemt_QueryDispatcherEvent::BTEMT_CONNECT_FAILED);
                d_dispatcherEventFunctor(event);
            } break;
            default:
              break; // ("eventCb received btemt_PoolMsg=%d",state);
          }
      } break;
      case btemt_Message::BTEMT_CHANNEL_STATE: {
          switch (state) {
            case btemt_ChannelMsg::BTEMT_CHANNEL_DOWN: {
                event.setEventType(
                    btemt_QueryDispatcherEvent::BTEMT_PROCESSOR_DOWN);
                d_channels.remove(processorId);
                d_dispatcherEventFunctor(event);
            } break;
            case btemt_ChannelMsg::BTEMT_CHANNEL_UP: {
                event.setEventType(
                    btemt_QueryDispatcherEvent::BTEMT_PROCESSOR_UP);
                d_channels.add(processorId);
                d_dispatcherEventFunctor(event);
            } break;
            case btemt_ChannelMsg::BTEMT_MESSAGE_DISCARDED: {
                event.setEventType(
                    btemt_QueryDispatcherEvent::BTEMT_QUERY_DISCARDED);
                d_dispatcherEventFunctor(event);
            } break;
            default:
              break; //("eventCb received btemt_ChannelMsg=%d",state);
          }
      } break;
      default: {
          ;// do nothing
      }
    }
}

// PRIVATE MANIPULATORS
int btemt_QueryDispatcher::registerTimer(
    const bdef_Function<void(*)()>& callback,
    const bdet_TimeInterval&        startTime,
    int                            *handle)
{
    int myHandle = d_timerScheduler.scheduleEvent(startTime, callback);
    if (myHandle) {
        *handle = myHandle;
        return 0;
    }
    else {
        return -1;
    }
}

// MANIPULATORS
bsls_PlatformUtil::Int64 btemt_QueryDispatcher::query(
    btemt_Query *query,
    const bdef_Function<void (*)(int *, btemt_QueryResponse*)>& functor)
{
    return timedQuery(query, functor, bdet_TimeInterval(0));
}

bsls_PlatformUtil::Int64 btemt_QueryDispatcher::query(
    btemt_Query *query, int processorId,
    const bdef_Function<void (*)(int *, btemt_QueryResponse*)>& functor)
{
    return timedQuery(query, processorId, functor, bdet_TimeInterval(0));
}

bsls_PlatformUtil::Int64 btemt_QueryDispatcher::timedQuery(
    btemt_Query *query,
    const bdef_Function<void (*)(int *, btemt_QueryResponse*)>& functor,
    const bdet_TimeInterval& timeout)
{
    int processorId;
    if (!d_channels.next(&processorId)) {
        return 0;
    }
    return timedQuery(query, processorId, functor, timeout);
}

bsls_PlatformUtil::Int64 btemt_QueryDispatcher::timedQuery(
    btemt_Query *query, int processorId,
    const bdef_Function<void (*)(int *, btemt_QueryResponse*)>& functor,
    const bdet_TimeInterval& timeout)
{
    void *category = query->category();
    btemt_Query myQuery(*query); // steal the managed ptr from 'query'

    ++d_queryCount; // Increment count even if query fails to register

    bcema_SharedPtr<Entry> entry;
    entry.createInplace(d_allocator_p,
                        ++d_atomicId, // d_uniqueId
                        BTEMT_NULL_CLOCK, // d_clockId
                        timeout, // d_timeout
                        bdetu_SystemTime::now(), // d_startTime
                        category); // d_category

    // make a dispatcherV1Functor out of the functor
    bdef_Function<void (*)(const bcema_SharedPtr<btemt_QueryResponse>&)>
        dispatcherV1Functor(
                bdef_BindUtil::bindA(
                  d_allocator_p
                , bdef_MemFnUtil::memFn( &btemt_QueryDispatcher::dispatcherV1Cb
                                       , this)
                , _1
                , functor));

    entry->d_enqueueV1Functor =
        bdef_BindUtil::bindA(
                d_allocator_p
              , bdef_MemFnUtil::memFn( &btemt_QueryDispatcher::postFunctorCb
                                     , this)
              , _1
              , dispatcherV1Functor);

    int handle = d_queryCatalog.add(entry);
    if (!handle) {
        return 0; // failure
    }

    Int64 queryId = makeQueryId(handle, entry->d_uniqueId);

    if (category) {
        d_mapLock.lock();
        d_queryMap.insert(bsl::make_pair<void*, Int64>(category,
                                                       queryId));
        d_mapLock.unlock();
    }

    if (timeout > 0) {
        int status =
            registerTimer(bdef_BindUtil::bind(
                              &btemt_QueryDispatcher::timeoutCb,
                              this, queryId),
                          entry->d_startTime + timeout,
                          &entry->d_clockId);
        if (status) {
            bsl::stringstream ss;
            ss << myQuery;
            bsl::fprintf(stderr,
                         "failed to register timer for query: %s",
                         ss.str().c_str());
            return 0;
        }
    }

    if (0 == d_queryRouter_p->query(myQuery, queryId, processorId)) {
        btemt_QueryDispatcherEvent
                      event(processorId,
                            0,
                            btemt_QueryDispatcherEvent::BTEMT_QUERY_SUBMITTED);
        bteso_IPv4Address processorAddress;
        d_queryRouter_p->getProcessorAddress(&processorAddress, processorId);
        event.setProcessorAddress(processorAddress);
        d_dispatcherEventFunctor(event);

//        bsl::printf("timedQuery 2: queryId = %lld, h = %d, u = %d\n",
//                    queryId, handle, (int)entry->d_uniqueId);

        return queryId;
    }
    else {
        // Do nothing because dispatcherV1Cb() removes it after timeout
        return 0;
    }
}

void btemt_QueryDispatcher::cancel(bsls_PlatformUtil::Int64 queryId)
{
    int handle, uniqueId;
    splitQueryId(&handle, &uniqueId, queryId);

    bcema_SharedPtr<Entry> entry;

    if (d_queryCatalog.find(handle, &entry)) {
        return;
    }
    bcemt_LockGuard<bcemt_Mutex> guard(&entry->d_mutex);

    if (uniqueId != entry->d_uniqueId) {
        return;
    }

    BSLS_ASSERT(0 == d_queryCatalog.remove(handle, &entry));
    entry->d_uniqueId = BTEMT_UNSET_ID;

    if (BTEMT_NULL_CLOCK != entry->d_clockId) {
        deregisterTimer(entry->d_clockId);
        // No need to check return value.  If it fails, the timeout will find
        // the entry has been removed.
    } // else { no clock was registered for this queryId }
    ++d_cancelCount;
//    bsl::printf("Canceled queryId = %lld.\n", queryId); // TBD: remove this

    if (0 != entry->d_category) {
        d_mapLock.lock();
        d_queryMap.erase(
            bsl::make_pair<void*, Int64>(entry->d_category, queryId));
        d_mapLock.unlock();
    }
}

void btemt_QueryDispatcher::cancel(void *category)
{
    if (0 == category) { return; }

    bsl::vector<Int64> queryIds;
    d_mapLock.lock();
    bsl::set<bsl::pair<void*, Int64> >::iterator itr =
        d_queryMap.lower_bound(bsl::make_pair<void*, Int64>(category,
#if !defined(BSLS_PLATFORM__CMP_MSVC) \
 && !defined(BDES_PLATFORM__OS_FREEBSD)
                                                            INT64_MIN));
#else
                                                            LLONG_MIN));
#endif
    for (; itr != d_queryMap.end(); ++itr) {
        if (itr->first == category) {
            queryIds.push_back(itr->second);
        }
    }
    typedef bsl::vector<Int64>::size_type size_type;
    for (size_type i = 0; i < queryIds.size(); ++i) {
        d_queryMap.erase(bsl::make_pair<void*, Int64>(category, queryIds[i]));
    }
    d_mapLock.unlock();

    int count = 0;
    for (size_type i = 0; i < queryIds.size(); ++i) {
        bcema_SharedPtr<Entry> entry;
        int handle, uniqueId;
        splitQueryId(&handle, &uniqueId, queryIds[i]);

        if (d_queryCatalog.find(handle, &entry)) {
            continue;
        }
        bcemt_LockGuard<bcemt_Mutex> guard(&entry->d_mutex);
        if (uniqueId != entry->d_uniqueId) {
            continue;
        }
        BSLS_ASSERT(0 == d_queryCatalog.remove(handle));
        if (BTEMT_NULL_CLOCK != entry->d_clockId) {
            deregisterTimer(entry->d_clockId);
            // No need to check return value.
        }
        ++count;
    }
    d_cancelCount += count;
//    bsl::printf("Canceled %d queries with category = %d\n",
//                count, (int)category); // TBD: delete this
}

                        // =================================
                        // CHANNEL AND THREADPOOL MANAGEMENT
                        // =================================
int btemt_QueryDispatcher::start()
{
    d_threadPool.start();
    return d_queryRouter_p->start() || d_timerScheduler.start();
}

void btemt_QueryDispatcher::stop()
{
    d_timerScheduler.stop();
    d_queryRouter_p->stop();
    d_threadPool.stop();
}

int btemt_QueryDispatcher::connect(const bteso_IPv4Address& address,
                                   const bdet_TimeInterval& timeout)
{
    int status = d_queryRouter_p->connect(address, timeout);
    if (status) {
        d_dispatcherEventFunctor(
                             btemt_QueryDispatcherEvent::BTEMT_CONNECT_FAILED);
    }
    return status;
}

                        // =========
                        // ACCESSORS
                        // =========
void btemt_QueryDispatcher::printStatus(bsl::ostream& os) const
{
    bsls_PlatformUtil::Int64 avgResponseTime = successCount() ?
        (bsls_PlatformUtil::Int64)d_sumElapsedResponseTime / successCount()
        : 0;
    os << "{\n numbers of mininum and maximum threads in thread pool:"
       << "(" << d_threadPool.minThreads()
       << ", " << d_threadPool.maxThreads() << ")"
       << "\n number of threads in channel pool: "
       << d_queryRouter_p->channelPool()->numThreads()

       << "\n\n number of connected processors: " << numProcessors()
       << "\n number of channels in channel pool: "
       << d_queryRouter_p->channelPool()->numChannels()
       << "\n number of query-registry entries: "
       << d_queryCatalog.length()
       << "\n number of queryMap entries: "
       << d_queryMap.size()
       << "\n number of active threads in thread pool:"
       << d_threadPool.numActiveThreads()
       << "\n number of waiting threads in thread pool:"
       << d_threadPool.numWaitingThreads()
       << "\n total received query count: "
       << queryCount()
       << "\n successful response count: "
       << successCount()
       << "\n canceled query count: "
       << cancelCount()
       << "\n timed-out  response count: "
       << timeoutCount()
       << "\n average response time: "
       << avgResponseTime << " usec"
       << "\n}"
       << bsl::endl;
}

} // Close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
