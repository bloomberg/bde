// btemt_querydispatcher.h   -*-C++-*-
#ifndef INCLUDED_BTEMT_QUERYDISPATCHER
#define INCLUDED_BTEMT_QUERYDISPATCHER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide 'bdem_List'-based query dispatcher
//
//@CLASSES:
//    btemt_QueryDispatcher:
//    btemt_QueryDispatcherEvent:
//
//@AUTHOR: Andrei Basov (abasov)
//
//@SEE_ALSO:
//    btemt_QueryDispatcherConfiguration
//    btemt_QueryRouter
//    btemt_QueryResponse
//    btemt_QueryProcessor
//
//@DESCRIPTION:
// This component provides 'btemt_QueryDispatcher', which maintains connections
// to a pool of homogeneous 'btemt_QueryProcessor's.  User can send a
// 'btemt_Query' using this component and expect one or more
// 'btemt_QueryResponse's with a timeout.  This component uses 'connect' method
// to connect to a processor and adds it into the managed channel pool.
// Together with the query, user supplies 'timedQuery' or 'query' method with a
// 'bdef_Function<void (*)(int *, btemt_QueryResponse *)>'-type functor,
// which the dispatcher calls asynchronously after a response is received (or
// timed out) for the query.  The first argument of the supplied functor is an
// output user uses to indicate whether the response is the final one in
// possibly a sequence of responses to the query.
//
///Usage
///-----
// TBD: Will make one out of the test driver

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BCEC_OBJECTCATALOG
#include <bcec_objectcatalog.h>
#endif

#ifndef INCLUDED_BCECS_ROUNDROBIN
#include <bcecs_roundrobin.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BCEP_THREADPOOL
#include <bcep_threadpool.h>
#endif

#ifndef INCLUDED_BCEP_TIMEREVENTSCHEDULER
#include <bcep_timereventscheduler.h>
#endif

#ifndef INCLUDED_BTEMT_CHANNELPOOLCONFIGURATION
#include <btemt_channelpoolconfiguration.h>
#endif

#ifndef INCLUDED_BTESO_IPV4ADDRESS
#include <bteso_ipv4address.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BTEMT_QUERY
#include <btemt_query.h>
#endif

#ifndef INCLUDED_BTEMT_QUERYROUTER
#include <btemt_queryrouter.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_SET
#include <bsl_set.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

class btemt_Message;
class btemt_QueryDispatcherConfiguration;

class btemt_QueryDispatcherEvent {
  public:
    enum Type {
        BTEMT_CONNECT_FAILED,
        BTEMT_PARSE_ERROR,
        BTEMT_PROCESSOR_UP,
        BTEMT_PROCESSOR_DOWN,
        BTEMT_QUERY_DISCARDED,
        BTEMT_QUERY_SUBMITTED,
        BTEMT_UNMATCHED_RESPONSE,
        BTEMT_UNKNOWN
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , CONNECT_FAILED     = BTEMT_CONNECT_FAILED
      , PARSE_ERROR        = BTEMT_PARSE_ERROR
      , PROCESSOR_UP       = BTEMT_PROCESSOR_UP
      , PROCESSOR_DOWN     = BTEMT_PROCESSOR_DOWN
      , QUERY_DISCARDED    = BTEMT_QUERY_DISCARDED
      , QUERY_SUBMITTED    = BTEMT_QUERY_SUBMITTED
      , UNMATCHED_RESPONSE = BTEMT_UNMATCHED_RESPONSE
      , UNKNOWN            = BTEMT_UNKNOWN
#endif
    };
  private:
    int               d_processorId;
    bteso_IPv4Address d_processorAddress;
    void             *d_userData;
    Type              d_eventType;
  public:
    // CREATORS
    btemt_QueryDispatcherEvent(int processorId = 0, void *userData = 0,
                               Type eventType = BTEMT_UNKNOWN)
        : d_processorId(processorId)
        , d_userData(userData)
        , d_eventType(eventType)
    { }

    btemt_QueryDispatcherEvent(const btemt_QueryDispatcherEvent& rhs)
        : d_processorId(rhs.d_processorId)
        , d_processorAddress(rhs.d_processorAddress)
        , d_userData(rhs.d_userData)
        , d_eventType(rhs.d_eventType)
    { }

    ~btemt_QueryDispatcherEvent() { }

    // MANIPULATORS
    btemt_QueryDispatcherEvent& operator=(
        const btemt_QueryDispatcherEvent& rhs)
    {
        if (&rhs != this) {
            d_processorId= rhs.d_processorId;
            d_processorAddress = rhs.d_processorAddress;
            d_userData = rhs.d_userData;
            d_eventType = rhs.d_eventType;
        }
        return (*this);
    }

    void setProcessorId(int processorId)
    {
        d_processorId = processorId;
    }

    void setProcessorAddress(const bteso_IPv4Address& processorAddress)
    {
        d_processorAddress = processorAddress;
    }

    void setUserData(void *userData)
    {
        d_userData = userData;
    }

    void setEventType(Type eventType)
    {
        d_eventType = eventType;
    }

    // ACCESSORS
    int processorId() const
    {
        return d_processorId;
    }

    const bteso_IPv4Address& processorAddress() const
    {
        return d_processorAddress;
    }

    void *userData() const
    {
        return d_userData;
    }

    Type eventType() const
    {
        return d_eventType;
    }
};

// FREE OPERATORS
bool operator==(const btemt_QueryDispatcherEvent& lhs,
                const btemt_QueryDispatcherEvent& rhs);

bool operator!=(const btemt_QueryDispatcherEvent& lhs,
                const btemt_QueryDispatcherEvent& rhs);

bsl::ostream& operator<<(bsl::ostream& os,
                         const btemt_QueryDispatcherEvent& rhs);

class btemt_QueryDispatcher {
    typedef bsls_PlatformUtil::Int64 Int64;

    bslma_Allocator         *d_allocator_p;
    bcep_ThreadPool          d_threadPool;
    btemt_QueryRouter       *d_queryRouter_p;// manages channel pool

    bcecs_RoundRobin<int>    d_channels;     // selects a channel in
                                             // round-robin fashion
    bdef_Function<void (*)(const btemt_QueryDispatcherEvent&)>
                             d_dispatcherEventFunctor;
                                             // A cached dispatcher event
                                             // functor supplied at
                                             // construction
    enum Id { BTEMT_UNSET_ID = -1,
              BTEMT_INIT_ID = 0,
              BTEMT_NULL_CLOCK = 0     // Gino will provide this later in
                                 // bcep_timereventscheduler
    };
    struct Entry;
    friend struct btemt_QueryDispatcher::Entry;

    bces_AtomicInt           d_atomicId;     // provides for unresponded
                                             // query's unique id.  It's
                                             // initialized to INIT_ID

    bces_AtomicInt           d_queryCount;   // total number of queries that
                                             // are requested

    bces_AtomicInt           d_successCount; // number  of queries that have
                                             // successfully received their
                                             // last responses

    bces_AtomicInt           d_cancelCount;  // number of queries that have
                                             // been successfully canceled

    bces_AtomicInt           d_timeoutCount; // number of queries that have
                                             // at least one response that
                                             // times out
    bces_AtomicInt64         d_sumElapsedResponseTime;
                                             // Summation of all successful
                                             // queries' response time since
                                             // dispatcher was constructed.
                                             // One response time is defined as
                                             // the lapse between when the
                                             // query was first enqueued and
                                             // when the last response was
                                             // received.  In microseconds
    struct Entry {
        // An entry record for a single unresponded query in the
        // d_queryCatalog.  It references a callback that enqueues into the
        // threadpool the processing of a btemt_QueryResponse upon receiving
        // it.  It also keeps the response timeout and the time of the query
        // submission.

        bcemt_Mutex                      d_mutex;
                                             // protect this Entry record.
        int                              d_uniqueId;
                                             // uniquely identifies one Entry.
                                             // This forms a 64-bit integer
                                             // with the handle from
                                             // d_queryCatalog.  This 64-bit
                                             // integer is used as the queryId.
        int                              d_clockId;
                                             // a timer handle
        bdet_TimeInterval                d_timeout;
                                             // relative timeout interval
                                             // before one response is
                                             // received.
        bdet_TimeInterval                d_startTime;
                                             // query submission absolute time

        bdef_Function<void (*)(const bcema_SharedPtr<btemt_QueryResponse>&)>
                                         d_enqueueV1Functor;
                                             // Enqueue into threadpool the
                                             // processing callback for a
                                             // received response.  The
                                             // callback wraps around the user
                                             // functor supplied at query
                                             // submission.
        void                            *d_category;
                                             // category for this query
        // CREATORS
        Entry()
            : d_uniqueId(BTEMT_UNSET_ID),
              d_clockId(BTEMT_NULL_CLOCK),
              d_category(0)
            {}
        Entry(int uniqueId,
              int clockId,
              const bdet_TimeInterval& timeout,
              const bdet_TimeInterval& startTime,
              void *category)
            : d_uniqueId(uniqueId), d_clockId(clockId), d_timeout(timeout),
              d_startTime(startTime), d_category(category)
            {}
        ~Entry() {}
      private:
        // DISABLED
        Entry(const Entry&);
        Entry& operator=(const Entry&);
    };

    bcec_ObjectCatalog<bcema_SharedPtr<Entry> >
                             d_queryCatalog; // contains all queries
                                             // waiting for responses.  The
                                             // handle from this registry is
                                             // used as half of a query's
                                             // queryId.  dispatcherV1Cb() and
                                             // cancel() remove items from
                                             // this registry and query() and
                                             // its variants add into it.
    bsl::set<bsl::pair<void *, Int64> >
                             d_queryMap;     // maps category
                                             // of type void* into multiple
                                             // queryId's.  dispatcherV1Cb()
                                             // and cancel() remove items from
                                             // this registry and query() and
                                             // its variants add into it.

    bcemt_Mutex              d_mapLock;      // guards access to d_queryMap.

    bcep_TimerEventScheduler d_timerScheduler;
                                             // schedule timer events.

  private:
    // CALLBACKS
    void timeoutCb(Int64 queryId);
        // Scheduled with a clock for the specified 'queryId', this callback
        // checks for if a response has been received for this queryId since
        // last time a clock was set for it.  If the response is not yet
        // received, a TIMEOUT response is sent to the functor-ified
        // dispatcherV1Cb in the threadpool.

    void postFunctorCb(
        bcema_SharedPtr<btemt_QueryResponse> response,
        const bdef_Function<void (*)(
           const bcema_SharedPtr<btemt_QueryResponse>&)>& dispatcherV1Functor);
        // A functor based on this callback takes the specified
        // 'dispatcherV1Functor' and sends it to the threadpool for execution
        // with the specified 'response' as the functor argument

    void responseCb(bcema_SharedPtr<btemt_QueryResponse> response);
        // A functor based on this callback is passed to the member query
        // router when it's constructed.  The query router calls the functor
        // when after it parses the specified 'response' out of a received
        // message.  This callback will reset the timeout if there is one (see
        // timeoutCb), and pass the execution of user supplied functor to the
        // threadpool (see postFunctorCb and dispatcherV1Cb)

    void dispatcherV1Cb(
        bcema_SharedPtr<btemt_QueryResponse> response,
        const bdef_Function<void (*)(int *, btemt_QueryResponse*)>& functor);
        // In postFunctorCb, a functor based on this callback is given to the
        // threadpool.  This callback uses the user supplied functor 'functor'
        // to analyze the specified 'response' received by the router and
        // depending on the user functor output (whether it's the final
        // response) or the response timeout status, this callback decides
        // whether to remove the queryId from the registry.

    void eventCb(btemt_Message::MessageType type, int state,
                 int processorId,
                 const bteso_IPv4Address& processorAddress,
                 void *userData);
        // A functor based on this callback is passed to the router as the
        // router manages the channel pool.  This callback will invoke the
        // cached d_dispatcherEventFunctor over a btemt_QueryDispatcherEvent
        // translated from the specified 'state' of the specified 'type'.

    // MANIPULATORS
    int registerTimer(const bdef_Function<void(*)()>& callback,
                      const bdet_TimeInterval&        startTime,
                      int                            *handle);
        // Register the 'callback' to be invoked at 'startTime' (absolute
        // time), and fill the specified '*handle'.  Return 0 on success and a
        // non-zero value otherwise.

    int reregisterTimer(int                      handle,
                        const bdet_TimeInterval& startTime);
        // Re-register the timer identified by the 'handle' as returned from a
        // previous call to 'registerTimer' to be invoked at 'startTime'.
        // Return 0 on success, and a non-zero value if the 'handle' is
        // invalid, or the timer event has just been dispatched, or the timer
        // event is about to be dispatched soon and cannot be canceled.

    int deregisterTimer(int handle);
        // Deregister/cancel the timer having the specified 'handle'.  Return 0
        // on success, and a non-zero value if the 'handle' is invalid, or the
        // timer event has just been dispatched, or the timer event is about to
        // be dispatched soon and cannot be canceled.

    // DISABLED
    btemt_QueryDispatcher(const btemt_QueryDispatcher&);
    btemt_QueryDispatcher& operator=(const btemt_QueryDispatcher&);
  public:
    // CREATORS
    btemt_QueryDispatcher(
        const btemt_QueryDispatcherConfiguration& config,
        const bcemt_Attribute& threadAttributes,
        const bdef_Function<void (*)(const btemt_QueryDispatcherEvent&)>&
            dispatcherEventFunctor,
        bslma_Allocator *basicAllocator = 0);
       // The channel pool and thread pool are created and configured according
       // to 'config'.  'dispatcherEventFunctor' is cached and is invoked
       // every time a channel event occurs:
       //   btemt_ChannelMsg::BTEMT_CHANNEL_UP -> PROCESSOR_UP
       //   btemt_ChannelMsg::BTEMT_CHANNEL_DOWN -> PROCESSOR_DOWN
       //   btemt_ChannelMsg::BTEMT_MESSAGE_DISCARDED -> QUERY_DISCARDED

    ~btemt_QueryDispatcher();
       // Destroy this object

    // MANIPULATORS
    Int64 query(btemt_Query *query,
                const bdef_Function<void (*)(int *, btemt_QueryResponse*)>&
                    functor);

    Int64 timedQuery(
        btemt_Query *query,
        const bdef_Function<void (*)(int *, btemt_QueryResponse*)>&
            functor,
        const bdet_TimeInterval& timeout);
        // Send the specified 'query' to a processor selected by the round
        // robin.  Invoke the user supplied 'functor' once a response is
        // available or 'timeout' (relative time) is reached.  The timeout
        // value, if specified, is reset after every 'functor''s invocation.
        // The first argument of 'functor' is an output parameter to indicate
        // whether or not a final response was received (as determined by user,
        // who must load a non-zero value to indicate a final response).  The
        // second argument of 'functor' is a pointer to the received
        // btemt_QueryResponse packet.  The 'functor' may steal the managed
        // object in that packet.  When the query was successfully submitted,
        // the cached d_dispatcherEventFunctor is synchronously executed with
        // btemt_QueryDispatcherEvent::BTEMT_QUERY_SUBMITTED.  Return a
        // non-zero query ID on success and 0, otherwise.  'query' may lose its
        // managed object after this call.

    Int64 query(btemt_Query *query, int processorId,
                const bdef_Function<void (*)(int *, btemt_QueryResponse*)>&
                    functor);

    Int64 timedQuery(
        btemt_Query *query, int processorId,
        const bdef_Function<void (*)(int *, btemt_QueryResponse*)>&
            functor,
        const bdet_TimeInterval& timeout);
        // Send the specified 'query' to the processor of the specified
        // 'processorId'.  Invoke the user supplied 'functor' once a response
        // is available or 'timeout' (relative time) is reached.  The timeout
        // value, if specified, is reset after every 'functor''s invocation.
        // The first argument of 'functor' is an output parameter to indicate
        // whether or not a final response was received (as determined by user,
        // who must load a non-zero value to indicate a final response).  The
        // second argument of 'functor' is a pointer to the received
        // btemt_QueryResponse packet.  The 'functor' may steal the managed
        // object in that packet.  When the query was successfully submitted,
        // the cached d_dispatcherEventFunctor is synchronously executed with
        // btemt_QueryDispatcherEvent::BTEMT_QUERY_SUBMITTED.  Return a
        // non-zero query ID on success and 0, otherwise.  'query' may lose its
        // managed object after this call.

    void cancel(Int64 queryId);
        // Cancel a query with the specified 'queryId'.  Do nothing if
        // 'queryId' doesn't exist.  It's possible a final response is received
        // or a response has timed out for a queryId after user issues a cancel
        // but before cancel completes.  TBD: current implementation is slow

    void cancel(void *category);
        // Cancel all queries associated with the specified 'category' and
        // awaiting responses.  Do nothing if 'category' is 0.
        // It's possible a final response is received or a response has timed
        // out for a queryId after user issues a cancel but before cancel
        // completes.

    // THREAD MANAGEMENT
    int start();
       // 'start' the query router; 'start' the thread pool; 'start' the timer
       // scheduler.  Return 0 on success, and a non-zero value otherwise.

    void stop();
       // 'stop' the query router; 'drain' the thread pool.  'stop' the timer
       // scheduler.

    // CHANNEL MANAGEMENT

    int listen(const bteso_IPv4Address& address, int queueLength = 10);
        // Calls btemt_QueryRouter::listen
        // Return 0 on success and a non-zero value otherwise

    int connect(const bteso_IPv4Address& address,
                const bdet_TimeInterval& timeout);
       // Asynchronously, establish a channel with the peer process at the
       // specified 'address' with the specified 'timeout' (absolute time).
       // Return 0 on success and a non-zero value otherwise.  If connection
       // cannot be established, 'dispatcherEventFunctor' is called.

    // ACCESSORS
    void printStatus(bsl::ostream& os) const; // for testing only
        // Print out current status of dispatcher: thread and channel pool
        // configurations, and query response counts, etc.

    int queryCount() const;
        // Return the total number of queries received by the dispatcher since
        // construction.

    int successCount() const;
        // Return the number of successful queries since the construction of
        // the dispatcher.  One successful query means one or more responses
        // have been received and the user supplied functor has determined the
        // final status.

    int cancelCount() const;
        // Return the number of successfully canceled queries since the
        // construction of the dispatcher.

    int timeoutCount() const;
        // Return the number of timed-out queries since the construction of the
        // dispatcher by the time of this call.  One timed-out query means at
        // least one response for this query has timed out.

    int numProcessors() const;
        // Return the number of currently connected processors.
};

// INLINE PRIVATE MANIPULATORS
inline
int btemt_QueryDispatcher::reregisterTimer(int                      handle,
                                           const bdet_TimeInterval& startTime)
{
    return d_timerScheduler.rescheduleEvent(handle,
                                            startTime);
}

inline
int btemt_QueryDispatcher::deregisterTimer(int handle)
{
    return d_timerScheduler.cancelEvent(handle);
}

// INLINE CHANNEL MANAGEMENT
inline
int btemt_QueryDispatcher::listen(const bteso_IPv4Address& address,
                                  int queueLength)
{
    return d_queryRouter_p->listen(address, queueLength);
}

// INLINE ACCESSORS
inline
int btemt_QueryDispatcher::queryCount() const
{
    return d_queryCount;
}

inline
int btemt_QueryDispatcher::successCount() const
{
    return d_successCount;
}

inline
int btemt_QueryDispatcher::cancelCount() const
{
    return d_cancelCount;
}

inline
int btemt_QueryDispatcher::timeoutCount() const
{
    return d_timeoutCount;
}

inline
int btemt_QueryDispatcher::numProcessors() const
{
    return d_channels.numObjects();
}

} // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
