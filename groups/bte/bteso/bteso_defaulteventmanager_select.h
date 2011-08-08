// bteso_defaulteventmanager_select.h                                 -*-C++-*-
#ifndef INCLUDED_BTESO_DEFAULTEVENTMANAGER_SELECT
#define INCLUDED_BTESO_DEFAULTEVENTMANAGER_SELECT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a 'select'-based socket-event multiplexer.
//
//@AUTHOR: Andrei Basov (abasov), Rohan Bhindwale (rbhindwa)
//
//@CLASSES:
//   bteso_DefaultEventManager<bteso_Platform::SELECT>:
//                                      'select'-based socket-event multiplexer
//
//@SEE_ALSO: bteso_eventmanager bteso_defaulteventmanager bteso_timemetrics
//
//@DESCRIPTION: This component provides a class,
// 'bteso_DefaultEventManager<bteso_Platform::SELECT>', that implements the
// 'bteso_EventManager' protocol using the 'select' system call available on
// platforms conforming to POSIX.1g.
//
// Generally speaking, a platform imposes a limit on the number of socket
// handles that can be registered with 'select'.  This limit, which is a
// compile time constant (and, therefore, cannot be increased at run time), can
// be as low as 64 (e.g., Windows) or as high as 1024 (e.g., Solaris).
// However, it is often required to handle more than this maximum number of
// simultaneous connections.  This component does not provide a solution to
// this problem but provides an accessor function, 'canRegisterSockets', that
// allows clients to identify if this event manager is at the socket
// registration limit.  If that is the case then clients can create more
// objects of this class for registering their sockets.
//
///Thread-safety
///-------------
// The 'select'-based multiplexers provided by this component depend on a
// 'bdema'-style allocator to supply memory.  The allocator is installed
// at creation and is NOT managed by the multiplexers.  If the allocator is not
// *thread* *safe* (*thread* *enabled*) then the instances of this
// component using that allocator will consequently not be *thread* *safe*
// (*thread* *enabled*).  Moreover, if two distinct instances of a multiplexer
// share the same allocator, thread-safety is guaranteed if and only if the
// allocator is *thread* *enabled*.  If no allocator is provided at creation,
// the default allocator, which is *thread* *enabled*, is used.
// Otherwise, the following is guaranteed:
//..
//   o A single instance of this event manager is *not* *thread* *safe* with
//     respect to operations invoked on the same instance from different
//     threads: such operations are *not* guaranteed to work correctly.
//   o An instance of this event manager *is* *thread* *safe* with respect to
//     operations invoked on *different* *instances* from different threads:
//     there will be no side-effects from such operations (which, generally
//     speaking, means that there is no 'static' data).
//   o Distinct instances of this event manager are *thread* *enabled* meaning
//     that operations invoked on distinct instances from different threads can
//     proceed in parallel.
//   o This event manager is not *async-safe*, meaning that one or more
//     functions cannot be invoked safely from a signal handler.
//..
///Performance
///-----------
// Given that S is the number of socket events registered, the two classes of
// this component each provide the following (typical) complexity guarantees:
//..
//  +=======================================================================+
//  |        FUNCTION          | EXPECTED COMPLEXITY | WORST CASE COMPLEXITY|
//  +-----------------------------------------------------------------------+
//  | dispatch                 |        O(S)         |       O(S^2)         |
//  +-----------------------------------------------------------------------+
//  | registerSocketEvent      |        O(1)         |        O(S)          |
//  +-----------------------------------------------------------------------+
//  | deregisterSocketEvent    |        O(1)         |        O(S)          |
//  +-----------------------------------------------------------------------+
//  | deregisterSocket         |        O(1)         |        O(S)          |
//  +-----------------------------------------------------------------------+
//  | deregisterAll            |        O(S)         |        O(S)          |
//  +-----------------------------------------------------------------------+
//  | numSocketEvents          |        O(1)         |        O(S)          |
//  +-----------------------------------------------------------------------+
//  | numEvents                |        O(1)         |        O(1)          |
//  +-----------------------------------------------------------------------+
//  | isRegistered             |        O(1)         |        O(S)          |
//  +=======================================================================+
//..
//
///USAGE EXAMPLE
///-------------
// For the comprehensive usage example, see 'bteso_defaulteventmanager_poll'
// component.

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_PLATFORM
#include <bteso_platform.h>
#endif

#ifdef BTESO_PLATFORM__WIN_SOCKETS
  #ifndef INCLUDED_WINSOCK2
    #include <winsock2.h>
    #define INCLUDED_WINSOCK2
  #endif
#endif

#ifdef BTESO_PLATFORM__BSD_SOCKETS
  #ifndef INCLUDED_SYS_SELECT
    #include <sys/select.h>
    #define INCLUDED_SYS_SELECT
  #endif
#endif

#ifndef INCLUDED_BTESO_DEFAULTEVENTMANAGERIMPL
#include <bteso_defaulteventmanagerimpl.h>
#endif

#ifndef INCLUDED_BTESO_EVENTMANAGER
#include <bteso_eventmanager.h>
#endif

#ifndef INCLUDED_BTESO_EVENT
#include <bteso_event.h>
#endif

#ifndef INCLUDED_BCEMA_POOLALLOCATOR
#include <bcema_poolallocator.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSL_HASH_MAP
#include <bsl_hash_map.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bteso_TimeMetrics;
class bslma_Allocator;

            // =======================================================
            // class bteso_DefaultEventManager<bteso_Platform::SELECT>
            // =======================================================

template <>
class bteso_DefaultEventManager<bteso_Platform::SELECT>
                                                  : public bteso_EventManager {
    // This class implements the 'bteso_EventManager' protocol to provide an
    // event manager that uses the 'select' system call.

  public:
    enum {
        BTESO_MAX_NUM_HANDLES = FD_SETSIZE  // maximum number of socket handles
                                            // that can be registered with an
                                            // event manager (of this type)
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , MAX_NUM_HANDLES = BTESO_MAX_NUM_HANDLES
#endif
    };

  private:
    typedef bsl::hash_map<bteso_Event,
                          bteso_EventManager::Callback,
                          bteso_EventHash>              EventMap;

    bcema_PoolAllocator d_eventsAllocator;   // event map allocator

    bsl::hash_map<bteso_Event, bteso_EventManager::Callback, bteso_EventHash>
                        d_events;     // socket events and associated callbacks

    fd_set              d_readSet;    // set of descriptors monitored for
                                      // incoming data

    int                 d_numRead;    // number of sockets in the read set

    fd_set              d_writeSet;   // set of descriptors monitored for
                                      // outgoing data

    int                 d_numWrite;   // number of sockets in the write set

    fd_set              d_exceptSet;  // set of descriptors monitored for
                                      // exceptions

    int                 d_maxFd;      // maximum number of socket descriptors

    bteso_TimeMetrics  *d_timeMetric; // time metrics given to this object

    bsl::vector<EventMap::iterator> d_signaledRead;
    bsl::vector<EventMap::iterator> d_signaledWrite;
                                      // temporary arrays used by dispatch

    // PRIVATE ACCESSORS
    bool checkInternalInvariants();
        // Verify that every socket handle that is registered in the
        // specified 'events' is set in the appropriate set (e.g., either
        // 'readSet' or 'writeSet' depending on whether or not this is a
        // READ or WRITE event).  Return 'true' on success, and 'false'
        // otherwise.

    int canBeRegistered(const bteso_SocketHandle::Handle& handle);
        // Return 1 if the specified 'handle' can be registered with this
        // 'select'-based event manager and 0 otherwise.

    int dispatchCallbacks(int           numEvents,
                          const fd_set& readSet,
                          const fd_set& writeSet,
                          const fd_set& exceptSet);
        // Dispatch the specified 'numEvents' callbacks from the specified
        // 'readSet', 'writeSet', and 'exceptSet' file descriptor sets that
        // were signalled as ready.

  public:
    // CREATORS
    explicit
    bteso_DefaultEventManager(bslma_Allocator   *basicAllocator = 0);
    explicit
    bteso_DefaultEventManager(bteso_TimeMetrics *timeMetric,
                              bslma_Allocator   *basicAllocator = 0);
        // Create a 'select'-based event manager.  Optionally specify a
        // 'timeMetric' to report time spent in CPU-bound and IO-bound
        // operations.  If 'timeMetric' is not specified or is 0, these metrics
        // are not reported.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~bteso_DefaultEventManager();
        // Destroy this object.  Note that the registered callbacks are NOT
        // invoked.

    // MANIPULATORS
    int dispatch(int flags);
        // For each pending socket event, invoke the corresponding callback
        // registered with this event manager.  If no event is pending, wait
        // until either (1) at least one event occurs (in which case the
        // corresponding callback(s) is invoked) or (2) provided that the
        // specified 'flags' contains 'bteso_Flag::BTESO_ASYNC_INTERRUPT', an
        // underlying system call is interrupted by a signal.  Return the
        // number of dispatched callbacks on success, and a negative value
        // otherwise; -1 is reserved to indicate that an underlying system
        // call was interrupted.  When such an interruption occurs this method
        // will return (-1) if 'flags' contains
        // 'bteso_Flag::BTESO_ASYNC_INTERRUPT' and otherwise will automatically
        // restart (i.e., reissue the identical system call).  Note that all
        // callbacks are invoked in the same thread that invokes 'dispatch',
        // and the order of invocation, relative to the order of registration,
        // is unspecified.  Also note that -1 is never returned if 'option' is
        // set to 'bteso_Flag::BTESO_ASYNC_INTERRUPT'.

    int dispatch(const bdet_TimeInterval& timeout, int flags);
        // For each pending socket event, invoke the corresponding callback
        // registered with this event manager.  If no event is pending, wait
        // until either (1) at least one event occurs (in which case the
        // corresponding callback(s) is invoked), (2) the specified absolute
        // 'timeout' is reached, or (3) provided that the specified 'flags'
        // contains 'bteso_Flag::BTESO_ASYNC_INTERRUPT', an underlying system
        // call is interrupted by a signal.  Return the number of dispatched
        // callbacks on success, 0 if 'timeout' is reached, and a negative
        // value otherwise; -1 is reserved to indicate that an underlying
        // system call was interrupted.  When such an interruption occurs this
        // method will return -1 if 'flags' contains
        // 'bteso_Flag::BTESO_ASYNC_INTERRUPT', and otherwise will
        // automatically restart (i.e., reissue the identical system call).
        // Note that all callbacks are invoked in the same thread that invokes
        // 'dispatch', and the order of invocation, relative to the order of
        // registration, is unspecified.  Also note that -1 is never returned
        // if 'flags' contains 'bteso_Flag::BTESO_ASYNC_INTERRUPT'.

     int registerSocketEvent(const bteso_SocketHandle::Handle&   handle,
                             const bteso_EventType::Type         event,
                             const bteso_EventManager::Callback& callback);
        // Register with this event manager the specified 'callback' to be
        // invoked when the specified 'event' occurs on the specified socket
        // 'handle'.  Each socket event registration stays in effect until it
        // is subsequently deregistered; the callback is invoked each time the
        // corresponding event is detected.  'bteso_EventType::BTESO_READ' and
        // 'bteso_EventType::BTESO_WRITE' are the only events that can be
        // registered simultaneously for a socket.  If a registration attempt
        // is made for an event that is already registered, the callback
        // associated with this event will be overwritten with the new one.
        // Simultaneous registration of incompatible events for the same socket
        // 'handle' will result in undefined behavior.  Return 0 on success and
        // a non-zero value otherwise.

    void deregisterSocketEvent(const bteso_SocketHandle::Handle& handle,
                               bteso_EventType::Type             event);
        // Deregister from this event manager the callback associated with the
        // specified 'event' on the specified 'handle' so that said callback
        // will not be invoked should 'event' occur.

    int deregisterSocket(const bteso_SocketHandle::Handle& handle);
        // Deregister from this event manager all events associated with the
        // specified socket 'handle'.  Return the number of deregistered
        // callbacks.

    void deregisterAll();
        // Deregister from this event manager all events on every socket
        // handle.

    // ACCESSORS
    bool canRegisterSockets() const;
        // Return 'true' if this event manager can register additional sockets,
        // and 'false' otherwise.

    int isRegistered(const bteso_SocketHandle::Handle& handle,
                     const bteso_EventType::Type       event) const;
        // Return 1 if the specified 'event' is registered with this event
        // manager for the specified socket 'handle' and 0 otherwise.

    int numEvents() const;
        // Return the total number of all socket events currently registered
        // with this event manager.

    int numSocketEvents(const bteso_SocketHandle::Handle& handle) const;
        // Return the number of socket events currently registered with this
        // event manager for the specified 'handle'.
};

//-----------------------------------------------------------------------------
//                      INLINE FUNCTIONS' DEFINITIONS
//-----------------------------------------------------------------------------

           // =======================================================
           // class bteso_DefaultEventManager<bteso_Platform::SELECT>
           // =======================================================

// ACCESSORS
inline
int bteso_DefaultEventManager<bteso_Platform::SELECT>::numEvents() const
{
    return static_cast<int>(d_events.size());
}

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
