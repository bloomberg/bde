// bteso_defaulteventmanager.h    -*-C++-*-
#ifndef INCLUDED_BTESO_DEFAULTEVENTMANAGER
#define INCLUDED_BTESO_DEFAULTEVENTMANAGER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an interface for default event manager.
//
//@CLASSES:
//  bteso_DefaultEventManager<TYPE>: event manager interface
//  bteso_EventMgr: namespace for the default event manager type
//
//@SEE_ALSO: bteso_eventmanager, bteso_defaulteventmanager, bteso_timemetrics,
//           bteso_defaulteventmanager_poll,  bteso_defaulteventmanager_select,
//           bteso_defaulteventmanager_devpoll, bteso_defaulteventmanager_wfmo,
//           bteso_tcptimereventmanager,  bteso_eventmanagertest
//
//@AUTHOR: Andrei Basov (abasov)
//
//@DESCRIPTION: This component provides a default socket event multiplexer that
// adheres to 'bteso_EventManager' protocol.  In a case where more than one
// multiplexing mechanism is available for a certain platform, all mechanisms
// (to the best of our knowledge) are implemented and a particular one can be
// chosen explicitly by the user (see usage example).  The following socket
// event manager specializations (presented along with the polling mechanisms
// and applicable platforms) are available through this component (note that,
// for shortness, we will omit 'bteso_DefaultEventManager' in the
// specialization column):
//..
//  +=======================================================================+
//  |      SPECIALIZATION       |   SYSTEM MECHANISM    |     PLATFORM      |
//  +-----------------------------------------------------------------------+
//  | <bteso_Platform::SELECT>  |        select         | Windows, Solaris, |
//  |                           |                       | AIX, Linux        |
//  +-----------------------------------------------------------------------+
//  | <bteso_Platform::WFMO>    | WaitForMultipleEvents |      Windows*     |
//  +-----------------------------------------------------------------------+
//  | <bteso_Platform::DEVPOLL> |        /dev/poll      |      Solaris*     |
//  +-----------------------------------------------------------------------+
//  | <bteso_Platform::EPOLL>   |         epoll         |       Linux*      |
//  +-----------------------------------------------------------------------+
//  | <bteso_Platform::POLL>    |          poll         | Solaris, AIX*,    |
//  |                           |                       | Linux             |
//  +=======================================================================+
//  * indicates the default specialization for a platform.
//..
//
///USAGE EXAMPLE
///-------------
// In the following usage example we show how to create an instance of a
// default event manager.  First, we need to include this file (shown here for
// completeness):
//..
//  #include <bteso_defaulteventmanager.h>
//..
// Second, create a 'bteso_TimeMetrics' to give to the event manager:
//..
//  bteso_TimeMetrics metrics(bteso_TimeMetrics::BTESO_MIN_NUM_CATEGORIES,
//                            bteso_TimeMetrics::BTESO_CPU_BOUND);
//..
// Now, create a default event manager (using 'bteso_EventMgr') that uses this
// 'metrics':
//..
//  bteso_EventMgr::TYPE eventManager(&metrics);
//..
// Note that the time metrics is optional.  Using the same component, we can
// create an event manager that uses a particular mechanism (for example,
// '/dev/poll') as follows:
//..
//  bteso_DefaultEventManager<bteso_Platform::DEVPOLL> fastEventManager;
//..
// Note that '/dev/poll' is available only on Solaris and this instantiation
// fails (at compile time) on other platforms.

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_DEFAULTEVENTMANAGER_DEVPOLL
#include <bteso_defaulteventmanager_devpoll.h>
#endif

#ifndef INCLUDED_BTESO_DEFAULTEVENTMANAGER_EPOLL
#include <bteso_defaulteventmanager_epoll.h>
#endif

#ifndef INCLUDED_BTESO_DEFAULTEVENTMANAGER_POLL
#include <bteso_defaulteventmanager_poll.h>
#endif

#ifndef INCLUDED_BTESO_DEFAULTEVENTMANAGER_POLLSET
#include <bteso_defaulteventmanager_pollset.h>
#endif

#ifndef INCLUDED_BTESO_DEFAULTEVENTMANAGER_SELECT
#include <bteso_defaulteventmanager_select.h>
#endif

#ifndef INCLUDED_BTESO_EVENT
#include <bteso_event.h>
#endif

#ifndef INCLUDED_BTESO_EVENTMANAGER
#include <bteso_eventmanager.h>
#endif

#ifndef INCLUDED_BTESO_EVENTTYPE
#include <bteso_eventtype.h>
#endif

#ifndef INCLUDED_BTESO_PLATFORM
#include <bteso_platform.h>
#endif

#ifndef INCLUDED_BTESO_SOCKETHANDLE
#include <bteso_sockethandle.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

class bdet_TimeInterval;
class bteso_TimeMetrics;

                            // =====================
                            // struct bteso_EventMgr
                            // =====================

struct bteso_EventMgr {
    // Provides a type definition for the default event manager for a given
    // platform.

    // This class is DEPRECATED: instead of 'bteso_EventMgr::TYPE', use
    // 'bteso_DefaultEventManager<>' directly.

    typedef bteso_DefaultEventManager<> TYPE;
};

                     // ===============================
                     // class bteso_DefaultEventManager
                     // ===============================

template <class POLLING_MECHANISM>
class bteso_DefaultEventManager : public bteso_EventManager {
    // This class standardizes the interface for the default event manager for
    // a given platform.  The generic class (this one) is NOT implemented.  All
    // specialized event managers must adhere to this interface.
    //
    // Note that 'POLLING_MECHANISM' is specified in the forward declaration in
    // 'bteso_defaulteventmanagerimpl.h' to default to
    // 'bteso_Platform::DEFAULT_POLLING_MECHANISM'.

  public:
    // CREATORS
    explicit
    bteso_DefaultEventManager(bteso_TimeMetrics *timeMetric     = 0,
                              bslma_Allocator   *basicAllocator = 0);
        // Create a 'poll'-based event manager.  Optionally specify a
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
        // For each socket event pending on this event manager, invoke the
        // corresponding callback registered with this event manager.  If no
        // event is pending, wait until either (1) at least one event occurs
        // (in which case the corresponding callback(s) is invoked) or (2)
        // provided that the specified 'flags' contains
        // 'bteso_Flag::BTESO_ASYNC_INTERRUPT', an underlying system call is
        // interrupted by a signal.  Return the number of dispatched callbacks
        // on success, and a negative value otherwise; -1 is reserved to
        // indicate that an underlying system call was interrupted.  When such
        // an interruption occurs this method will return -1 if 'flags'
        // contains 'bteso_Flag::BTESO_ASYNC_INTERRUPT' and otherwise will
        // automatically restart (i.e., reissue the identical system call).
        // Note that the order of invocation, relative to the order of
        // registration, is unspecified and that -1 is never returned if
        // 'flags' does not contain 'bteso_Flag::BTESO_ASYNC_INTERRUPT'.  Also
        // note that the behavior of this method may be undefined if the number
        // of registered sockets is 0.

    int dispatch(const bdet_TimeInterval& timeout, int flags);
        // For each socket event pending on this event manager, invoke the
        // corresponding callback registered with this event manager.  If no
        // event is pending, wait until either (1) at least one event occurs
        // (in which case the corresponding callback(s) are invoked), (2) the
        // specified absolute 'timeout' interval is reached, or (3) provided
        // that the specified 'flags' contains
        // 'bteso_Flag::BTESO_ASYNC_INTERRUPT', an underlying system call is
        // interrupted by a signal.  Return the number of dispatched callbacks
        // on success, 0 if 'timeout' is reached, and a negative value
        // otherwise; -1 is reserved to indicate that an underlying system call
        // was interrupted.  When such an interruption occurs this method will
        // return -1 if 'flags' contains 'bteso_Flag::BTESO_ASYNC_INTERRUPT'
        // and otherwise will automatically restart (i.e., reissue the
        // identical system call).  Note that the order of invocation, relative
        // to the order of registration, is unspecified and that -1 is never
        // returned if 'flags' does not contain
        // 'bteso_Flag::BTESO_ASYNC_INTERRUPT'.  Also note that the behavior of
        // this method may be undefined if the number of registered sockets is
        // 0.

    int registerSocketEvent(const bteso_SocketHandle::Handle&   handle,
                            const bteso_EventType::Type         event,
                            const bteso_EventManager::Callback& callback);
        // Register with this event manager the occurrence of the specified
        // 'event' on the specified socket 'handle' such that the specified
        // 'callback' functor is invoked when 'event' occurs.  Return 0 on
        // success and a non-zero value otherwise.  Socket event registrations
        // stay in effect until they are subsequently deregistered; the
        // callback is invoked each time the specified 'event' is seen.
        // Typically, 'bteso_EventType::BTESO_READ' and
        // 'bteso_EventType::BTESO_WRITE' are the only events that can be
        // registered simultaneously for a socket.  Simultaneously registering
        // for incompatible events for the same socket 'handle' may result in
        // undefined behavior.  If a registration attempt is made for an event
        // that is already registered, the callback associated with this event
        // will be overwritten with the new one.

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
    bool hasLimitedSocketCapacity() const;
        // Return 'true' if this event manager has a limited socket capacity,
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

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
