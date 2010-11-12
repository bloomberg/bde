// btemt_queryrouter.h                  -*-C++-*-
#ifndef INCLUDED_BTEMT_QUERYROUTER
#define INCLUDED_BTEMT_QUERYROUTER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a router that maintains connections to multiple processors
// and is able to route queries to selected processor and
//
//@CLASSES:
//
//@SEE_ALSO:
//
//@AUTHOR: Xiheng Xu (xxu)
//
//@DESCRIPTION:
//
///Usage
///-----
//

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTEMT_CHANNELPOOL
#include <btemt_channelpool.h>
#endif

#ifndef INCLUDED_BTEMT_MESSAGE
#include <btemt_message.h>
#endif

#ifndef INCLUDED_BTEMT_QUERY
#include <btemt_query.h>
#endif

#ifndef INCLUDED_BCEMA_POOLEDBUFFERCHAIN
#include <bcema_pooledbufferchain.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

namespace BloombergLP {

class bslma_Allocator;
class bteso_IPv4Address;
class bdet_TimeInterval;
class bdem_List;
class btemt_ChannelPoolConfiguration;

                        // =======================
                        // class btemt_QueryRouter
                        // =======================

class btemt_QueryRouter {
    // Class description
    bslma_Allocator                   *d_allocator_p;
                                       // Held, not owned

    btemt_ChannelPool                 *d_channelPool_p;
                                       // Owned

    bces_AtomicInt                     d_channelAllocatorId;
                                       // Provide sourceId's for allocated
                                       // channels

    bsl::map<int, bteso_IPv4Address>   d_channelAllocators;
                                       // A map of channel's sourceId to the
                                       // address of the server this router
                                       // connects to, or the address at which
                                       // this router listens.

    bcemt_Mutex                        d_channelAllocatorsLock;
                                       // A mutex for d_channelAllocators

                                       // Note: this sourceId is insignificant
                                       // at this moment.  The
                                       // d_channelAllocators and
                                       // d_channelAllocatorsLock are disabled
                                       // for now.

    bcema_PooledBufferChainFactory     d_factory;
                                       // Manage the underlying buffer for
                                       // btemt_DataMsg that's being sent to
                                       // the connected servers.

    bdef_Function<void (*)(const bcema_SharedPtr<btemt_QueryResponse>&)>
                                       d_responseFunctor;
                                       // Provided at construction to process
                                       // one btemt_QueryResponse.

    bdef_Function<void (*)(btemt_Message::MessageType, int,
                           int, const bteso_IPv4Address&, void *)>
                                       d_eventFunctor;
                                       // Provided at construction to process a
                                       // channel pool control event in the
                                       // form of a btemt_Message.  The
                                       // arguments are: 1) type of the
                                       // received btemt_Message; 2) state
                                       // indicated by the message of the
                                       // corresponding corresponding type; 3)
                                       // id of the connected processor if the
                                       // message is a btemt_ChannelMsg of the
                                       // channel through which the processor
                                       // is connected; the id is insignificant
                                       // otherwise; 4) IPv4 address of the
                                       // connected processor if the message is
                                       // a btemt_ChannelMsg; the address is
                                       // insignificant otherwise; 5) user data
                                       // in the form of void *.

  private:
    // CALLBACKS FOR CHANNEL POOL
    void poolCb(int state, int dummySourceId, int dummySeverity);
        // Pool state callback.  This is wrapped in the form of a bdef_Function
        // functor for the managed btemt_ChannelPool to process pool event
        // 'state'.  The 'dummySourceId' and the 'dummySeverity' are here so it
        // adheres to the interface of btemt_channelpool component.  They're
        // not used.

    void channelCb(int channelId, int sourceId,
                   int state, void *dummyContext);
        // Channel state callback.  This is wrapped in the form of a
        // bdef_Function functor for the managed btemt_ChannelPool to process
        // channel event 'state' of channel with 'channelId'.  The 'channelId'
        // is generated and managed by the channel pool.  The 'sourceId' is an
        // id set in 'connect()' and 'listen()' indicating the a supplied id
        // (as compared against the channelId generated when a channel is
        // allocated) of a channel when it's allocated by the channel pool.
        // The 'dummySeverity' is here so it adheres to the interface of
        // btemt_channelpool component.  It is not used.

    void dataCb(int *consumed, int *needed,
                const btemt_DataMsg& data, void *dummyContext);
        // Data callback.  This is wrapped in the form of a bdef_Function and
        // is invoked every time data is read from a channel.  The 'data' is
        // parsed into a vector of btemt_QueryResponse''s.  Because the 'data'
        // may contain an incomplete packet, after the callback is completed,
        // '*consumed' contains the length in bytes in 'data' that's
        // completedly consumed(parsed) into btemt_QueryResponse''s, and
        // '*needed' contains the length in bytes needed to parse into one more
        // btemt_QueryResponse.  The 'dummyContext' is here so it adheres to
        // the interface of btemt_channelpool component.  It is used only
        // for the copying out the remaining (i.e., not consumed bytes).  For
        // each parsed btemt_QueryResponse, call the cached d_responseFunctor.

    // DISABLED
    btemt_QueryRouter(const btemt_QueryRouter&); // Not implemented
    btemt_QueryRouter& operator=(const btemt_QueryRouter&); // Not implemented

  public:
    // CLASS METHODS

    // CREATORS
    btemt_QueryRouter(
        const btemt_ChannelPoolConfiguration& config,
        const bdef_Function<void (*)(
            const bcema_SharedPtr<btemt_QueryResponse>&)>& responseFunctor,
        const bdef_Function<void (*)(btemt_Message::MessageType, int,
                                     int, const bteso_IPv4Address&, void *)>&
            eventFunctor,
        bslma_Allocator *basicAllocator = 0);
        // Create a router with the specified 'config' for the managed channel
        // pool, and cache the specified 'responseFunctor' to process a
        // received response from the channel pool, and cache the specified
        // 'eventFunctor' to process a channel or pool event.  The
        // 'eventFunctor''s arguments are: 1) type of the received
        // btemt_Message; 2) state indicated by the message of the
        // corresponding corresponding type; 3) id of the connected processor
        // if the message is a btemt_ChannelMsg of the channel through which
        // the processor is connected.  The id is insignificant otherwise; 4)
        // IPv4 address of the connected processor if the message is a
        // btemt_ChannelMsg; the address is insignificant otherwise; 5) user
        // data in the form of 'void *'.  Optionally specify a 'basicAllocator'
        // to supply memory.  If 'basicAllocator' is omitted, the global new
        // and delete operators are used.

    ~btemt_QueryRouter();
        // Destroy this object.

    // THREAD MANAGEMENT
    int start();
        // Start the channel pool managed by this query router.  Return 0 on
        // success, and a non-zero value otherwise.

    void stop();
        // Stop the channel pool managed by this query router

    // CHANNEL MANAGEMENT
    int connect(const bteso_IPv4Address& address,
                const bdet_TimeInterval& timeout);
        // Connect to server at the specified 'address' with one attempt.  If
        // the specified 'timeout' interval expires or connection fails, return
        // non-0 values.  Return 0 on success.  An id is assigned to each
        // channel thus allocated.

    int listen(const bteso_IPv4Address& address, int queueLength);
        // Listens at the 'address' with a maximum number of pending connection
        // being 'queueLength'.  REUSEADDRESS is enabled for this listening
        // socket.  Return 0 on success and a non-zero value otherwise.

    // MANIPULATORS
    btemt_ChannelPool *channelPool();
        // Return a pointer to the managed channel pool This is TEMPORARY for
        // TESTING ONLY

    int query(const btemt_Query& query, bsls_PlatformUtil::Int64 queryId,
              int processorId);
        // Send the specified 'query' to the channel associated with the
        // specified 'processorId'.  Return 0 on success, non-zero otherwise

    // ACCESSORS
    void getProcessorAddress(bteso_IPv4Address *address,
                             int processorId) const;
        // Load the '*address' for the processor with 'processorId'.  Behavior
        // is undefined if 'processorId' is not known to the router.
};

// FREE OPERATORS

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// THREAD MANAGEMENT
inline
int btemt_QueryRouter::start()
{
    return d_channelPool_p->start();
}

inline
void btemt_QueryRouter::stop()
{
    d_channelPool_p->stop();
}

// MANIPULATORS
inline
btemt_ChannelPool *btemt_QueryRouter::channelPool()
{
    return d_channelPool_p;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
