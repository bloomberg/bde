// bteso_defaulteventmanager_epoll.h                                  -*-C++-*-
#ifndef INCLUDED_BTESO_DEFAULTEVENTMANAGER_EPOLL
#define INCLUDED_BTESO_DEFAULTEVENTMANAGER_EPOLL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide socket multiplexer implementation using Linux 'epoll'.
//
//@CLASSES:
//  bteso_DefaultEventManager<bteso_Platform::EPOLL>: 'epoll'-based multiplexer
//
//@SEE_ALSO: bteso_eventmanager bteso_defaulteventmanager bteso_timemetrics
//
//@SEE_ALSO: bteso_tcptimereventmanager  bteso_eventmanagertest
//
//@AUTHOR: Guillaume Morin (gmorin1)
//
//@DESCRIPTION: This component provides an implementation of an event manager
// that uses the Linux 'epoll' system calls to monitor for socket events and
// adheres to the 'bteso_EventManager' protocol.  In particular, this protocol
// supports the registration of level-triggered socket events, along with an
// associated 'bdef_Function' callback functor, which is invoked when the
// corresponding socket event occurs.
//
// Registering a socket event requires specifying a socket handle and the type
// of event to monitor on the indicated socket.  Socket event registrations
// stay in effect until they are subsequently deregistered; the associated
// callback is invoked each time the specified socket event occurs provided
// that appropriate method (i.e., 'dispatch') is called.  Once deregistered,
// the callback will no longer be invoked.
//
///Availability
///------------
// The 'epoll' systems calls (and consequently this specialized component) is
// currently supported only on Linux.  Direct use of this library component on
// *any* platform may result in non-portable software.
//
///Component Diagram
///-----------------
// This specialized component is one of the specializations of the
// 'bteso_defaulteventmanager' component; the other components are shown
// (schematically) on the following diagram:
//..
//                         _____bteso_defaulteventmanager_____
//                 _______/    |           |         |        \_________
//                 *_poll   *_epoll     *_select  *_devpoll    *_pollset
//..
///Thread-safety
///-------------
// This component depends on a 'bslma_Allocator' instance to supply memory.  If
// the allocator is not thread enabled then the instances of this component
// that use the same allocator instance will consequently not be thread safe
// Otherwise, this component provides the following guarantees.
//
// Accessing an instance of the event manager provided by this component from
// different threads may result in undefined behavior.  Accessing distinct
// instances from different threads is safe.  Distinct instances of the event
// manager provided by this component are *thread* *enabled* meaning that
// operations invoked on distinct instances from different threads can proceed
// concurrently.  The event manager is not *async-safe*, meaning that one or
// more functions cannot be invoked safely from a signal handler.
//
///Performance
///-----------
// Given that S is the number of socket events registered, this component
// provides the following complexity guarantees: TBD XXX
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
///Metrics
///-------
// The event manager provided by this component can use external (i.e.,
// user-installed) time metrics (see 'bteso_timemetrics' component) to record
// times spend in IO-bound and CPU-bound operations using the category IDs
// defined in 'bteso_TimeMetrics'.
//
///USAGE EXAMPLE
///-------------
// The following snippets of code illustrate how to use this event manager with
// a non-blocking socket.  First, create a 'bteso_TimeMetrics' object and a
// 'bteso_DefaultEventManager<bteso_Platform::EPOLL>' object; also create a
// (locally-connected) socket pair:
//..
//  bteso_TimeMetrics timeMetric(bteso_TimeMetrics::BTESO_MIN_NUM_CATEGORIES,
//                               bteso_TimeMetrics::BTESO_CPU_BOUND);
//  bteso_DefaultEventManager<bteso_Platform::EPOLL> mX(&timeMetric);
//
//  bteso_SocketHandle::Handle socket[2];
//
//  int rc = bteso_SocketImpUtil::socketPair<bteso_IPv4Address>(
//                           socket, bteso_SocketImpUtil::BTESO_SOCKET_STREAM);
//
//  assert(0 == rc);
//..
// Second, register three socket events, two for 'socket[0]', and one for
// 'socket[1]', and verify the state by using accessor functions.  The callback
// installed as part of each registration operation will either read or write
// the pre-determined (specified on a case-by-case basis) number of bytes:
//..
//  int numBytes = 5;
//  bteso_EventManager::Callback readCb(
//          bdef_BindUtil::bind(&genericCb, bteso_EventType::BTESO_READ,
//                              socket[0], numBytes, &mX));
//  mX.registerSocketEvent(socket[0], bteso_EventType::BTESO_READ, readCb);
//
//  numBytes = 25;
//  bteso_EventManager::Callback writeCb1(
//          bdef_BindUtil::bind(&genericCb, bteso_EventType::BTESO_WRITE,
//                              socket[0], numBytes, &mX));
//  mX.registerSocketEvent(socket[0], bteso_EventType::BTESO_WRITE, writeCb1);
//
//  numBytes = 15;
//  bteso_EventManager::Callback writeCb2(
//          bdef_BindUtil::bind(&genericCb, bteso_EventType::BTESO_WRITE,
//                              socket[1], numBytes, &mX));
//  mX.registerSocketEvent(socket[1], bteso_EventType::BTESO_WRITE, writeCb2);
//
//
//  assert(3 == mX.numEvents());
//  assert(2 == mX.numSocketEvents(socket[0]));
//  assert(1 == mX.numSocketEvents(socket[1]));
//  assert(1 == mX.isRegistered(socket[0], bteso_EventType::BTESO_READ));
//  assert(0 == mX.isRegistered(socket[1], bteso_EventType::BTESO_READ));
//  assert(1 == mX.isRegistered(socket[0], bteso_EventType::BTESO_WRITE));
//  assert(1 == mX.isRegistered(socket[1], bteso_EventType::BTESO_WRITE));
//..
// Next, we try to execute the requests by calling the 'dispatch' function with
// a timeout (5 seconds from now) requirement and verify the result.  The two
// write requests should be executed since both ends are writable If we don't
// have a timeout requirement, a different version of 'dispatch' (in which no
// timeout is specified) can also be called.
//..
//  bteso_EventManager::InterruptOpt opt =
//                                      bteso_EventManager::NON_INTERRUPTIBLE;
//  bdet_TimeInterval deadline(bdetu_SystemTime::now());
//  deadline += 5;    // timeout 5 seconds from now.
//  rc = mX.dispatch(deadline, opt);   assert(2 == rc);
//..
// Now we try to remove the write request of 'socket[0]' from the event manager
// by calling 'deregisterSocketEvent()' and verify the state:
//..
//  mX.deregisterSocketEvent(socket[0], bteso_EventType::BTESO_WRITE);
//  assert(2 == mX.numEvents());
//  assert(1 == mX.numSocketEvents(socket[0]));
//  assert(1 == mX.numSocketEvents(socket[1]));
//  assert(1 == mX.isRegistered(socket[0], bteso_EventType::BTESO_READ));
//  assert(0 == mX.isRegistered(socket[1], bteso_EventType::BTESO_READ));
//  assert(0 == mX.isRegistered(socket[0], bteso_EventType::BTESO_WRITE));
//  assert(1 == mX.isRegistered(socket[1], bteso_EventType::BTESO_WRITE));
//..
// We then try to remove all registrations associated with 'socket[1]' by
// calling 'deregisterSocket()' and verify the state:
//..
//  assert(1 == mX.deregisterSocket(socket[1]));
//  assert(1 == mX.numEvents());
//  assert(1 == mX.numSocketEvents(socket[0]));
//  assert(0 == mX.numSocketEvents(socket[1]));
//  assert(1 == mX.isRegistered(socket[0], bteso_EventType::BTESO_READ));
//  assert(0 == mX.isRegistered(socket[1], bteso_EventType::BTESO_READ));
//  assert(0 == mX.isRegistered(socket[0], bteso_EventType::BTESO_WRITE));
//  assert(0 == mX.isRegistered(socket[1], bteso_EventType::BTESO_WRITE));
//..
// Finally, we remove all registrations by calling 'deregisterAll()' and verify
// the state again:
//..
//  mX.deregisterAll();
//  assert(0 == mX.numEvents());
//  assert(0 == mX.numSocketEvents(socket[0]));
//  assert(0 == mX.numSocketEvents(socket[1]));
//  assert(0 == mX.isRegistered(socket[0], bteso_EventType::BTESO_READ));
//  assert(0 == mX.isRegistered(socket[0], bteso_EventType::BTESO_READ));
//  assert(0 == mX.isRegistered(socket[0], bteso_EventType::BTESO_WRITE));
//  assert(0 == mX.isRegistered(socket[1], bteso_EventType::BTESO_WRITE));
//..
// The following snippets of code show what a 'genericCb' may look like:
//..
//  static void
//  genericCb(bteso_EventType::Type event, bteso_SocketHandle::Handle socket,
//            int bytes, bteso_EventManager *mX)
//  {
//      // User specified callback function that is invoked when a socket
//      // event is detected.
//
//      enum {
//          MAX_READ_SIZE  = 8192,   // The numbers are for illustrative
//          MAX_WRITE_SIZE = 70000   // purposes only.
//      };
//
//      switch (event) {
//        case bteso_EventType::BTESO_READ: {
//            assert(0 < bytes);
//            char buffer[MAX_READ_SIZE];
//
//            int rc = bteso_SocketImpUtil::read(buffer, socket, bytes, 0);
//            assert(0 < rc);
//        } break;
//        case bteso_EventType::BTESO_WRITE: {
//            char wBuffer[MAX_WRITE_SIZE];
//            assert(0 < bytes);
//            assert(MAX_WRITE_SIZE >= bytes);
//            memset(wBuffer,'4', bytes);
//            int rc = bteso_SocketImpUtil::write(socket, &wBuffer, bytes, 0);
//            assert(0 < rc);
//        } break;
//        // ...
//        default: {
//             ASSERT("Invalid event code" && 0);
//        } break;
//      }
//  }
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_DEFAULTEVENTMANAGERIMPL
#include <bteso_defaulteventmanagerimpl.h>
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

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#include <bslalg_typetraitbitwisecopyable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_HASH_MAP
#include <bsl_hash_map.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

#if defined(BSLS_PLATFORM__OS_LINUX)

#ifndef INCLUDED_SYS_EPOLL
#include <sys/epoll.h>
#define INCLUDED_SYS_EPOLL
#endif

namespace BloombergLP {

template <>
struct bslalg_TypeTraits<struct ::epoll_event> :
                                        public bslalg_TypeTraitBitwiseCopyable
{
};

class bdet_TimeInterval;
class bteso_TimeMetrics;

           // ======================================================
           // class bteso_DefaultEventManager<bteso_Platform::EPOLL>
           // ======================================================
template <>
class bteso_DefaultEventManager<bteso_Platform::EPOLL>
                                                    : public bteso_EventManager
{
  private:
    struct HandleEvents {
        bool                         d_isValid;
        bteso_EventManager::Callback d_readCallback;
        bteso_EventManager::Callback d_writeCallback;
        bteso_EventType::Type        d_readEventType;
        bteso_EventType::Type        d_writeEventType;
        int                          d_mask;

        BSLALG_DECLARE_NESTED_TRAITS(HandleEvents,
                                     bslalg_TypeTraitBitwiseMoveable);
    };

    typedef bsl::hash_map<int, HandleEvents>               EventMap;

    int                          d_epollFd;      // epoll fd

    bsl::vector<struct ::epoll_event>
                                 d_signaled;     // array of 'epoll_event'
                                                 // structures indicating
                                                 // pending IO operations

    bool                         d_isInvokingCb; // is the manager invoking
                                                 // callbacks

    bteso_TimeMetrics           *d_timeMetric_p; // metrics to use for
                                                 // reporting percent-busy
                                                 // statistics

    EventMap                     d_events;       // map of socket handles
                                                 // to associated events

    bsl::vector<EventMap::iterator>
                                 d_entriesBeingRemoved;
                                                 // if we're in a user cb, we
                                                 // will not update the map
                                                 // right away but keep the
                                                 // list what needs to be
                                                 // removed here

    int                          d_numEvents;    // number of registered events

    // PRIVATE MANIPULATORS
    int dispatchCallbacks(const bsl::vector<struct ::epoll_event>& signaled,
                          int                                      numReady);

    int dispatchImp(int flags, const bdet_TimeInterval *timeout = 0);

  private:
    // NOT IMPLEMENTED
    bteso_DefaultEventManager(const bteso_DefaultEventManager&);
    bteso_DefaultEventManager& operator=(const bteso_DefaultEventManager&);

  public:
    // PUBLIC CLASS METHODS
    static bool isSupported();
        // Return true if the current kernel supports this event manager.

    // CREATORS
    explicit
    bteso_DefaultEventManager(bteso_TimeMetrics *timeMetric     = 0,
                              bslma_Allocator   *basicAllocator = 0);
        // Create a 'epoll'-based event manager.  Optionally specify a
        // 'timeMetric' to report time spent in CPU-bound and IO-bound
        // operations.  If 'timeMetric' is not specified or is 0, these metrics
        // are not reported.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~bteso_DefaultEventManager();
        // Destroy this object.  Note that the registered callbacks are NOT
        // invoked.

    // MANIPULATORS
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
        // method will return (-1) if 'flags' contains
        // 'bteso_Flag::BTESO_ASYNC_INTERRUPT', and otherwise will
        // automatically restart (i.e., reissue the identical system call).
        // Note that all callbacks are invoked in the same thread that invokes
        // 'dispatch', and the order of invocation, relative to the order of
        // registration, is unspecified.  Also note that -1 is never returned
        // unless 'flags' contains 'bteso_Flag::BTESO_ASYNC_INTERRUPT'.

    int dispatch(int flags);
        // For each pending socket event, invoke the corresponding callback
        // registered with this event manager.  If no event is pending, wait
        // until either (1) at least one event occurs (in which case the
        // corresponding callback(s) is invoked) or (2) provided that the
        // specified 'flags' contains 'bteso_Flag::BTESO_ASYNC_INTERRUPT', an
        // underlying system call is interrupted by a signal.  Return the
        // number of dispatched callbacks on success, and a negative value
        // otherwise; -1 is reserved to indicate that an underlying system call
        // was interrupted.  When such an interruption occurs this method will
        // return (-1) if 'flags' contains 'bteso_Flag::BTESO_ASYNC_INTERRUPT'
        // and otherwise will automatically restart (i.e., reissue the
        // identical system call).  Note that all callbacks are invoked in the
        // same thread that invokes 'dispatch', and the order of invocation,
        // relative to the order of registration, is unspecified.  Also note
        // that -1 is never returned unless 'option' is set to
        // 'bteso_Flag::BTESO_ASYNC_INTERRUPT'.

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
        // 'handle' will result in undefined behavior.  Return 0 in success and
        // a non-zero value, which is the same as native error code, on error.

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

//-----------------------------------------------------------------------------
//                      INLINE FUNCTIONS' DEFINITIONS
//-----------------------------------------------------------------------------

           // ======================================================
           // class bteso_DefaultEventManager<bteso_Platform::EPOLL>
           // ======================================================

// ACCESSORS
inline
bool bteso_DefaultEventManager<bteso_Platform::EPOLL>::
                                               hasLimitedSocketCapacity() const
{
    return false;
}

}  // close namespace BloombergLP

#endif // BSLS_PLATFORM__OS_LINUX

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
