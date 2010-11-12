// btemt_queryprocessor.h                                             -*-C++-*-
#ifndef INCLUDED_BTEMT_QUERYPROCESSOR
#define INCLUDED_BTEMT_QUERYPROCESSOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a generic query processor
//
//@AUTHOR: Andrei Basov (abasov)
//
//@CLASSES: btemt_QueryProcessorEvent: a container for various events
//@CLASSES: btemt_QueryProcessor: generic query processor
//
//@SEE_ALSO: btemt_QueryDispatcher btemt_QueryRequest btemt_QueryResult
//
//@DESCRIPTION: This class provides a generic query processor that receives
// queries taking the form of btemt_QueryRequest.
//
///Usage Example
///-------------
// In this usage example, a query processor is created which receives
// a query containing a list of integers and replies with the sum of these
// integers.  Basically, we have to define two functors, one for
// processor events (such as DISPATCHER_UP, etc.) and one that actually
// processes the query:
//..
//  void eventCallback(const btemt_QueryProcessorEvent& controlEvent)
//  {
//      bsl::cout << "controlEvent = " << controlEvent << bsl::endl;
//  }
//
//  void processingCallback(const btemt_Query& query,
//      const bdef_Function<void (*)(btemt_QueryResponse)>& replyFunctor)
//  {
//      btemt_QueryResponse response;
//      response.setQueryId(query.queryId());
//
//      bdem_List& result = response.result();
//      int sum = 0;
//      int length = query.query().length();
//      for (int i = 0; i < length; ++i) {
//          sum += query.query().theInt(i);
//      }
//      result.appendInt(sum);
//      response.setSequenceNumber(1);
//      response.setStatus(btemt_QueryResponse::BTEMT_SUCCESS);
//      replyFunctor(response);
//  }
//..
// Now, in the 'main' function, create a configuration for the processor
// create a processor and start it:
//..
//  btemt_QueryProcessorConfiguration cpc;
//  cpc.setIncomingMessageSize(1024);
//  cpc.setOutgoingMessageSize(1024);
//  cpc.setMaxConnections(10000);
//  cpc.setMaxIoThreads(4);
//  cpc.setMaxWriteCache(1024*1024);
//  cpc.setReadTimeout(100);
//  cpc.setMetricsInterval(30.0);
//
//  cpc.setProcessingThreads(3, 5);
//  cpc.setIdleTimeout(60);
//
//  bsl::cout << "Configuration = " << cpc << bsl::endl;
//  bdef_Function<void (*)(const btemt_QueryProcessorEvent&)> eventFunctor(
//                                                              eventCallback);
//  bdef_Function<void (*)(btemt_QueryRequest *,
//                bdef_Function<void (*)(btemt_QueryResult *)>)>
//        serverFunctor(processingCallback);
//
//  btemt_QueryProcessor processor(cpc, eventFunctor,
//                                 serverFunctor, &testAllocator);
//
//  enum { PORT_NUMBER = 15243 };
//  bteso_IPv4Address address;
//  address.setPortNumber(PORT_NUMBER);
//  bsl::cout << "Address = " << address << bsl::endl;
//  processor.start();
//  if (0 != processor.listen(address, 10)) {
//      bsl::cout << "Unable to establish a server on " << address
//                << bsl::endl;
//  }
//  else {
//      while(1) {
//          bcemt_ThreadUtil::microSleep(0, 10);
//      }
//  }
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTEMT_QUERYPROCESSORCONFIGURATION
#include <btemt_queryprocessorconfiguration.h>
#endif

#ifndef INCLUDED_BTEMT_QUERY
#include <btemt_query.h>
#endif

#ifndef INCLUDED_BTESO_IPV4ADDRESS
#include <bteso_ipv4address.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BCEMA_POOLEDBUFFERCHAIN
#include <bcema_pooledbufferchain.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

namespace BloombergLP {

class btemt_DataMsg;
class btemt_ChannelPool;
class bcep_ThreadPool;
class bslma_Allocator;

                        // ===============================
                        // class btemt_QueryProcessorEvent
                        // ===============================

class btemt_QueryProcessorEvent {
  public:
    enum Type {
        BTEMT_CONNECT_FAILED,
        BTEMT_PARSE_ERROR,
        BTEMT_DISPATCHER_UP,
        BTEMT_DISPATCHER_DOWN,
        BTEMT_QUERY_DISCARDED,
        BTEMT_QUERY_SUBMITTED,
        BTEMT_UNMATCHED_RESPONSE,
        BTEMT_UNKNOWN
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , CONNECT_FAILED     = BTEMT_CONNECT_FAILED
      , PARSE_ERROR        = BTEMT_PARSE_ERROR
      , DISPATCHER_UP      = BTEMT_DISPATCHER_UP
      , DISPATCHER_DOWN    = BTEMT_DISPATCHER_DOWN
      , QUERY_DISCARDED    = BTEMT_QUERY_DISCARDED
      , QUERY_SUBMITTED    = BTEMT_QUERY_SUBMITTED
      , UNMATCHED_RESPONSE = BTEMT_UNMATCHED_RESPONSE
      , UNKNOWN            = BTEMT_UNKNOWN
#endif
    };
  private:
    int               d_processorId;
    bteso_IPv4Address d_processorAddress;
    int               d_userData;
    Type              d_eventType;
  public:
    // CREATORS
    btemt_QueryProcessorEvent();

    btemt_QueryProcessorEvent(const btemt_QueryProcessorEvent& original);

    ~btemt_QueryProcessorEvent();

    // MANIPULATORS
    btemt_QueryProcessorEvent& operator=(const btemt_QueryProcessorEvent& rhs);

    void setEventType(Type eventType);

    void setProcessorId(int processorId);

    void setProcessorAddress(const bteso_IPv4Address& address);

    void setUserData(int userData);

    // ACCESSORS
    Type eventType() const;

    int processorId() const;

    const bteso_IPv4Address& processorAddress() const;

    int userData() const;
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& s,
                         const btemt_QueryProcessorEvent& rhs);

                        // ==========================
                        // class btemt_QueryProcessor
                        // ==========================

class btemt_QueryProcessor {
    bcep_ThreadPool       *d_threadPool_p;

    int                    d_externalThreadPoolFlag;

    btemt_ChannelPool     *d_channelPool_p;
    bcema_PooledBufferChainFactory
                           d_outFactory;

    bdef_Function<void (*)(const btemt_QueryProcessorEvent&)>
                           d_eventFunctor;

    bdef_Function<void (*)(btemt_QueryRequest*,
                           bdef_Function<void (*)(btemt_QueryResult*)>)>
                           d_serverFunctor;
                               // Cached processing functor supplied by server.
                               // The first argument is a pointer to the
                               // btemt_QueryRequest packet parsed from the
                               // channelpool for the server to process; the
                               // second argument is the functor with which
                               // the resulting btemt_QueryResult packet is
                               // written back to the channelpool.

    bces_AtomicInt         d_channelAllocatorId;

    bsl::map<int, bteso_IPv4Address>
                           d_channelAllocators;
    bcemt_Mutex            d_channelAllocatorsLock;
    bslma_Allocator       *d_allocator_p;

    void poolCb(int, int, int); // just print out
       // Invoke 'processorEventFunctor' as appropriate:
       //   btemt_PoolMsg::BTEMT_ERROR_CONNECTING -> CONNECT_FAILED
       // Log (print out) everything else

    void channelCb(int type, int channel, int source,
                   void *context);
       // Invoke 'processorEventFunctor' as appropriate:
       //   btemt_ChannelMsg::BTEMT_CHANNEL_UP -> DISPATCHER_UP
       //   btemt_ChannelMsg::BTEMT_CHANNEL_DOWN -> DISPATCHER_DOWN
       //   btemt_ChannelMsg::BTEMT_MESSAGE_DISCARDED -> QUERY_DISCARDED
       // Log (print out) everything else

    void dataCb(int *consumed, int *needed,
                const btemt_DataMsg& data,
                void *context);
       // Parse the data and invoke processing CB into a channel pool.

    void processQueryRequestCb(
        bcema_SharedPtr<btemt_QueryRequest> queryRequest,
        int channelId);
        // Process the packet 'queryRequest' parsed from the channel with
        // 'channelId', which is an id associated with the btemt_DataMsg from
        // the channel.  This will invoke the cached processing functor
        // provided by user.

    void serverFunctorWrapCb(
        bcema_SharedPtr<btemt_QueryRequest> queryRequest, int channelId);
        // Wrap around the cached d_serverFunctor so that the a normal pointer
        // to the btemt_QueryRequest packet is supplied to the d_serverFunctor
        // when the thread pool executes it.

    void writeQueryResultCb(btemt_QueryResult *result,
                            int channelId,
                            const bsls_PlatformUtil::Int64& queryId);
        // Write the 'result' appended with 'queryId' and a
        // btemt_QueryResponse::BTEMT_SUCCESS status to the channel associated
        // with 'channelId'.  This is invoked from within the cached processing
        // functor provided by user.

  private:
    // not implemented
    btemt_QueryProcessor(const btemt_QueryProcessor&);
    btemt_QueryProcessor& operator=(const btemt_QueryProcessor&);
  public:
    // CREATORS
    btemt_QueryProcessor(
            const btemt_QueryProcessorConfiguration&        configuaration,
            const bdef_Function<void (*)(const btemt_QueryProcessorEvent&)>&
                eventFunctor,
            const bdef_Function<void (*)(
                    btemt_QueryRequest*,
                    bdef_Function<void (*)(btemt_QueryResult*)>)>&
                serverFunctor,
            bslma_Allocator *basicAllocator = 0);
        // Create a query processor with the specified 'configuration' and
        // invoke the specified 'eventFunctor' whenever an "interesting" event
        // occurs and invoke the specified 'serverFunctor' to process queries.
        // The 'serverFunctor' will report the result in a form of a
        // 'btemt_QueryResponse' through the second functor parameter
        // 'serverFunctor' must the query ID appropriately in response.  All
        // other fields are ignored.

    btemt_QueryProcessor(
            const btemt_QueryProcessorConfiguration&        configuaration,
            bcep_ThreadPool                                *procPool,
            const bdef_Function<void (*)(const btemt_QueryProcessorEvent&)>&
                eventFunctor,
            const bdef_Function<void (*)(
                btemt_QueryRequest*,
                bdef_Function<void (*)(btemt_QueryResult*)>)>&
                serverFunctor,
            bslma_Allocator *basicAllocator = 0);
        // Create a query processor with the specified 'configuration' that
        // uses the specified 'procPool' thread pool for processing queries and
        // invokes the specified 'eventFunctor' whenever an "interesting" event
        // occurs and invoke the specified 'serverFunctor' to process queries.
        // The 'serverFunctor' will report the result in a form of a
        // 'btemt_QueryResponse' through the second functor parameter
        // 'serverFunctor' must the query ID appropriately in response.  All
        // other fields are ignored.  Note that the thread-pool parameters of
        // the 'configuration' are ignored.

    ~btemt_QueryProcessor();

    // MANIPULATORS
    // Threads management
    void start();
       // 'start' the channel pool; 'start' the thread pool.

    void stop();
       // 'stop' the channel pool; 'drain' the thread pool.

    // Channel management
    int listen(const bteso_IPv4Address& address, int queueLength);
       // Calls btemt_channelpool::listen.  Return 0 on success and a non-zero
       // value otherwise.

    int connect(const bteso_IPv4Address& address,
                const bdet_TimeInterval& timeout);
       // Asynchronously, establish a channel with the peer process at the
       // specified 'address' with the specified 'timeout' (absolute time).
       // Return 0 on success and a non-zero value otherwise.  If connection
       // cannot be established, 'eventFunctor' is called.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ===============================
                        // class btemt_QueryProcessorEvent
                        // ===============================
// CREATORS
inline
btemt_QueryProcessorEvent::btemt_QueryProcessorEvent()
: d_processorId       (-1)
, d_userData          (0)
, d_eventType         (BTEMT_UNKNOWN)
{
}

inline
btemt_QueryProcessorEvent::btemt_QueryProcessorEvent(
    const btemt_QueryProcessorEvent& original)
: d_processorId       (original.d_processorId)
, d_processorAddress  (original.d_processorAddress)
, d_userData          (original.d_userData)
, d_eventType         (original.d_eventType)
{
}

inline
btemt_QueryProcessorEvent::~btemt_QueryProcessorEvent()
{
}

// MANIPULATORS
inline
btemt_QueryProcessorEvent& btemt_QueryProcessorEvent::operator=(
    const btemt_QueryProcessorEvent& rhs)
{
    d_eventType         = rhs.d_eventType;
    d_processorId       = rhs.d_processorId;
    d_processorAddress  = rhs.d_processorAddress;
    d_userData          = rhs.d_userData;
    return *this;
}

inline
void btemt_QueryProcessorEvent::setEventType(Type eventType)
{
    d_eventType = eventType;
}

inline
void btemt_QueryProcessorEvent::setProcessorId(int processorId)
{
    d_processorId = processorId;
}

inline
void btemt_QueryProcessorEvent::setProcessorAddress(
    const bteso_IPv4Address& address)
{
    d_processorAddress  = address;
}

inline
void btemt_QueryProcessorEvent::setUserData(int userData)
{
    d_userData  = userData;
}

// ACCESSORS
inline
btemt_QueryProcessorEvent::Type btemt_QueryProcessorEvent::eventType() const
{
    return d_eventType;
}

inline
int btemt_QueryProcessorEvent::processorId() const
{
    return d_processorId;
}

inline
const bteso_IPv4Address& btemt_QueryProcessorEvent::processorAddress() const
{
    return d_processorAddress;
}

inline
int btemt_QueryProcessorEvent::userData() const
{
    return d_userData;
}

} // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
