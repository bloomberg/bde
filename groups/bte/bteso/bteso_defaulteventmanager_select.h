// bteso_defaulteventmanager_select.h                                 -*-C++-*-
#ifndef INCLUDED_BTESO_DEFAULTEVENTMANAGER_SELECT
#define INCLUDED_BTESO_DEFAULTEVENTMANAGER_SELECT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a 'select'-based socket-event multiplexer.
//
//@AUTHOR: Andrei Basov (abasov)
//
//@CLASSES:
//  bteso_DefaultEventManager_SelectRaw: 'select'-based multiplexer
//  bteso_DefaultEventManager<bteso_Platform::SELECT>:
//             'select'-based socket-event multiplexer with unlimited capacity
//
//@SEE_ALSO: bteso_eventmanager bteso_defaulteventmanager
//
//@DESCRIPTION: This component provides socket-event multiplexers implemented
// using the 'select' system call available on platforms conforming to
// POSIX.1g.
//
// Generally speaking, a platform imposes a limit on the number of socket
// handles that can be registered with 'select'.  This limit, which is a
// compile time constant (and, therefore, cannot be increased at run time), can
// be as low as 64 (e.g., Windows) or as high as 1024 (e.g., Solaris).
// However, it is often required to handle more than this maximum number of
// simultaneous connections.  This component does not provide a solution to
// this problem but provides an accessor function, 'canRegisterSocket', that
// allows clients to identify if this event manager is at the socket
// registeration limit.
//
// This component provides two multiplexers.  The
// 'bteso_DefaultEventManager_SelectRaw' provides the implementation for
// the 'select' based event manager and should not be used directly by
// clients.  The 'bteso_DefaultEventManager<bteso_Platform::SELECT>' event
// manager uses 'bteso_DefaultEventManager_SelectRaw' and should be used by
// clients.
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

                   // =========================================
                   // class bteso_DefaultEventManager_SelectRaw
                   // =========================================

class bteso_DefaultEventManager_SelectRaw {
    // This class provides a raw 'select'-based socket event multiplexer.
    // This multiplexer has a limitied capacity -- not more than
    // 'MAX_NUM_HANDLES' (see below) different socket handles can be
    // simultaneously registered with this event manager.  Note that
    // though this class doesn't implement bteso_EventManager protocol, its
    // interface is a strict superset of the 'bteso_EventManager' interface.

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
    bsl::hash_map<bteso_Event, bteso_EventManager::Callback, bteso_EventHash>
                 d_events;            // socket events and associated callbacks

    fd_set       d_readSet;           // set of descriptors monitored for
                                      // incoming data

    int          d_numRead;           // number of sockets in the read set

    fd_set       d_writeSet;          // set of descriptors monitored for
                                      // outgoing data

    int          d_numWrite;          // number of sockets in the write set

    fd_set       d_exceptSet;         // set of descriptors monitored for
                                      // exceptions

    int          d_maxFd;             // maximum number of socket descriptors

    bteso_TimeMetrics *d_timeMetric;  // time metrics given to this object

    bsl::vector<bteso_Event> d_signaledRead;
    bsl::vector<bteso_Event> d_signaledWrite;
                                      // temporary arrays used by dispatch

  private:
    // NOT IMPLEMENTED
    bteso_DefaultEventManager_SelectRaw(
                                   const bteso_DefaultEventManager_SelectRaw&);
    bteso_DefaultEventManager_SelectRaw&
                         operator=(const bteso_DefaultEventManager_SelectRaw&);
  public:
    // CLASS METHODS
    static int compareFdSets(const fd_set& lsh, const fd_set& rhs);
        // Return 0 if the specified socket-handle sets 'lhs' and 'rhs' contain
        // the same socket handles (independently of order) and a non-zero
        // value otherwise.

    int canBeRegistered(const bteso_SocketHandle::Handle& handle);
        // Return 1 if the specified 'handle' can be registered with this
        // 'select'-based event manager and 0 otherwise.

    // CREATORS
    explicit
    bteso_DefaultEventManager_SelectRaw(bteso_TimeMetrics *timeMetric     = 0,
                                        bslma_Allocator   *basicAllocator = 0);
        // Create a 'select'-based event manager.  Optionally specify a
        // 'timeMetric' to report time spent in CPU-bound and IO-bound
        // operations.  If 'timeMetric' is not specified or is 0, these metrics
        // are not reported.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~bteso_DefaultEventManager_SelectRaw();
        // Destroy this event manager.  Note that any registered callbacks are
        // NOT invoked.

    // MANIPULATORS
    int dispatch(const bdet_TimeInterval& timeout, int flags);
        // For each pending socket event, invoke the corresponding callback
        // registered with this event manager.  If no event is pending, wait
        // until either (1) at least one event occurs (in which case the
        // corresponding callback(s) is invoked), (2) the specified (absolute)
        // 'timeout' is exceeded, or (3) provided that the specified 'flags'
        // contains 'bteso_Flag::BTESO_ASYNC_INTERRUPT', an underlying system
        // call is interrupted by a signal.  Return the number of dispatched
        // callbacks on success, 0 on timeout, and a negative value otherwise;
        // -1 is reserved to indicate that an underlying system call was
        // interrupted.  When such an interruption occurs this method will
        // return (-1) if 'flags' contains 'bteso_Flag::BTESO_ASYNC_INTERRUPT',
        // and otherwise will automatically restart (i.e., reissue the
        // identical system call).  Note that all callbacks are invoked in the
        // same thread that invokes 'dispatch', and the order of invocation,
        // relative to the order of registration, is unspecified.  Also note
        // that -1 is never returned if 'flags' contains
        // 'bteso_Flag::BTESO_ASYNC_INTERRUPT'.

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

    int registerSocketEvent(const bteso_SocketHandle::Handle&   handle,
                            const bteso_EventType::Type         event,
                            const bteso_EventManager::Callback& callback);
        // Register with this event manager the specified 'callback' to be
        // invoked when the specified 'event' occurs on the specified socket
        // 'handle'.  Each socket event registration stays in effect until it
        // is subsequently deregistered; the callback is invoked each time
        // the corresponding event is detected.  'bteso_EventType::BTESO_READ'
        // and 'bteso_EventType::BTESO_WRITE' are the only events that can be
        // registered simultaneously for a socket.  If a registration attempt
        // is made for an event that is already registered, the callback
        // associated with this event will be overwritten with the new one.
        //  Return 0 on success and a non-zero value otherwise.  The behavior
        // is undefined unless 'handle' can be registered with this type of
        // event manager (as reported by 'canBeRegistered' method) and unless
        // the number of sockets registered with this event manager (as
        // reported by 'numSockets') doesn't exceed 'MAX_NUM_HANDLES'.
        // Simultaneous registration of incompatible events for the same socket
        // 'handle' will also result in undefined behavior.  Note that the
        // callback is recurring (i.e., it remains registered until it is
        // explicitly deregistered).

    void moveSocket(bteso_DefaultEventManager_SelectRaw *manager,
                    const bteso_SocketHandle::Handle&    handle);
        // Move all socket events along with associated callbacks corresponding
        // to the specified socket 'handle' from the specified event 'manager'
        // into this event manager.  The behavior is undefined if 'manager'
        // is 0 or unless 'handle' is registered with 'manager' (as
        // reported by a positive return status of 'numSocketEvents').

    void deregisterAll();
        // Deregister from this event manager all events on every socket
        // handle.

    int deregisterSocket(const bteso_SocketHandle::Handle& handle);
        // Deregister from this event manager all events associated with the
        // specified socket 'handle'.  Return the number of deregistered
        // callbacks.

    void deregisterSocketEvent(const bteso_SocketHandle::Handle& handle,
                               const bteso_EventType::Type       event);
        // Deregister from this event manager the callback associated with the
        // specified 'event' on the specified 'handle' so that said callback
        // will not be invoked should 'event' occur.

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

    int numSockets() const;
        // Return the number of different socket handles registered with
        // this event manager.

    int numSocketEvents(const bteso_SocketHandle::Handle& handle) const;
        // Return the number of socket events currently registered with this
        // event manager for the specified 'handle'.

    const bteso_Event& event(int index) const;
        // Return the event associated with the specified 'index'.  The
        // behavior is undefined unless 0 <= index < numEvents().  Note that
        // this association may be violated by an invocation of any
        // manipulator on this event manager.

    const bteso_EventManager::Callback& callback(int index) const;
        // Return a callback associated with the specified 'index'.  The
        // behavior is undefined unless 0 <= index < numEvents().  Note that
        // this association may be violated by an invocation of any
        // manipulator on this event manager.

    int canRegister(const bteso_SocketHandle::Handle& handle);
        // Return 1 if the specified 'handle' can be registered with this
        // event, manager and 0 otherwise.
};

            // =======================================================
            // class bteso_DefaultEventManager<bteso_Platform::SELECT>
            // =======================================================

template<>
class bteso_DefaultEventManager<bteso_Platform::SELECT>
    : public bteso_EventManager {
    // This class implements the 'bteso_EventManager' protocol to provide an
    // event manager that uses the 'select' system call but does not have the
    // (platform-specific) limited registration capacity associated with
    // 'select'.  This registration-capacity limitation is avoided by using
    // a time-multiplexing ("spin") technique.

  private:
    bteso_DefaultEventManager_SelectRaw d_impl;

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
    int dispatch(const bdet_TimeInterval&  timeout, int flags);
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

                   // =========================================
                   // class bteso_DefaultEventManager_SelectRaw
                   // =========================================
// ACCESSORS
inline
int bteso_DefaultEventManager_SelectRaw::numEvents() const
{
    return static_cast<int>(d_events.size());
}

inline
const bteso_Event& bteso_DefaultEventManager_SelectRaw::event(int index) const
{
    bsl::hash_map<bteso_Event,
                  bteso_EventManager::Callback,
                  bteso_EventHash>::const_iterator  callbackIt =
                                                              d_events.begin();
    for (int i = 0; i < index; ++i) {
        ++callbackIt;
    }
    return callbackIt->first;
}

inline int bteso_DefaultEventManager_SelectRaw::numSockets() const {
    return d_numRead > d_numWrite ? d_numRead : d_numWrite;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005, 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
