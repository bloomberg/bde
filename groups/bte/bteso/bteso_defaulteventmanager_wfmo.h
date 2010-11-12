// bteso_defaulteventmanager_wfmo.h      -*-C++-*-
#ifndef INCLUDED_BTESO_DEFAULTEVENTMANAGER_WFMO
#define INCLUDED_BTESO_DEFAULTEVENTMANAGER_WFMO

//@PURPOSE: Provide a 'wfmo'-based socket-event multiplexer
//
//@AUTHOR: Andrei Basov (abasov)
//
//@CLASSES:
//  bteso_DefaultEventManager_WfmoRaw:
//                               'wfmo'-based multiplexer w. limited capacity
//  bteso_DefaultEventManager<bteso_Platform::WFMO>:
//                'wfmo'-based socket-event multiplexer w. unlimited capacity
//
//
//@SEE_ALSO: bteso_eventmanager bteso_defaulteventmanager
//
//@DESCRIPTION: This component provides socket-event multiplexers implemented
// using 'wfmo' system call available on platforms conforming to POSIX.1g.
// Generally speaking, a platform imposes a limit on the number of socket
// handles that can be registered with 'wfmo'.  This limit can be as low
// as 64 (e.g., Windows) or as high as 1024 (e.g., Solaris).  In any case,
// this limit is a compile time constant (and, therefore, cannot be increased
// at run time).  However, it is often required to handle more than this
// maximum number of simultaneous connections.  One approach is to create
// another thread and associated with another set of socket handles.  Another
// is to spin around multiple sets of socket handles with pre-determined
// spin period.  Thereby, two multiplexers are provided by this component:
// 'bteso_DefaultEventManager_WfmoRaw' is a capacity-limited 'wfmo'-based
// multiplexer, and 'bteso_DefaultEventManager<bteso_Platform::WFMO>' is
// a spinning event manager with unlimited capacity.  A threaded strategy
// can be easily implemented with raw event managers.  The spinning
// multiplixer adheres to 'bteso_EventManager' protocol whereas the raw event
// manager doesn't.
//
///Thread-safety
///-------------
// The 'wfmo'-based multiplexers provided by this component depend on a
// 'bdema'-style allocator to supply memory.  The allocator is installed
// at creation and is NOT managed by the multiplexers.  If the allocator is not
// *thread* *safe* (or *thread* *enabled*) then the instances of this
// component using that allocator will consequently not be *thread* *safe*
// (*thread* *enabled*).  Moreover, if two distinct instances of a multiplexer
// share the same allocator, thread-safety is guaranteed if and only if the
// allocator is *thread* *enabled*.  If no allocator is provided at creation,
// the default allocator, which is *thread* *enabled*, is used.
// Otherwise, the following is guaranteed:
//..
//   o A single instance of this event manager is *not* *thread* *safe*,
//     meaning that an operation invoked on the same instance from different
//     threads is *not* guaranteed to work correctly.
//   o Distinct instances of this event manager *are* *thread* *safe* meaning
//     that any operation can be invoked on *distinct* *instances* from
//     different threads without any side-effects (which, generally speaking,
//     means that there is no 'static' data).
//   o Distinct instances of this event manager are *thread* *enabled* meaning
//     that operations invoked on distinct instances from different threads can
//     proceed in parallel.
//   o This event manager is not *async-safe*, meaning that one or more
//     functions cannot be invoked safely from a signal handler.
//..
///Performance
///-----------
// Given that S is the number of socket events registered, this component
// provides the following (typical) complexity guarantees:
//..
//  +=======================================================================+
//  |        FUNCTION          | EXPECTED COMPLEXITY | WORST CASE COMPLEXETY|
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
// WARNING: The performance may decrease *significantly* if the number of
// registered sockets is more than the number of socket handles supported by
// an underlying system (given by
// 'bteso_DefaultEventManager_WfmoRaw::MAX_NUM_HANDLES'), and, depending
// on an application requirements and design, a threaded approach may be
// more appropriate.
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

#ifndef INCLUDED_BTESO_DEFAULTEVENTMANAGERIMPL
#include <bteso_defaulteventmanagerimpl.h>
#endif

#ifndef INCLUDED_BTESO_SPINNINGEVENTMANAGER
#include <bteso_spinningeventmanager.h>
#endif

#ifndef INCLUDED_BTESO_EVENTMANAGER
#include <bteso_eventmanager.h>
#endif

#ifndef INCLUDED_BTESO_EVENT
#include <bteso_event.h>
#endif

#ifndef INCLUDED_BCEF_VFUNC0
#include <bcef_vfunc0.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BDEC2_MAP
#include <bdec2_map.h>
#endif

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif

namespace BloombergLP {

class bteso_TimeMetrics;
class bdema_Allocator;

#ifdef BTESO_PLATFORM__WIN_SOCKETS

                  // ============================================
                  // class bteso_DefaultEventManager_WfmoRaw_HandleHash
                  // ============================================
struct bteso_DefaultEventManager_WfmoRaw_HandleHash {
    // This struct defines (overloaded) hash methods in order to use
    // 'bteso_SocketHandle::Handle's and 'HANDLE's as keys in a
    // 'bdec2_Map'.
    static int hash(const bteso_SocketHandle::Handle& h, int size);
    static int hash(const HANDLE& h, int size);
};

                  // ============================================
                  // class bteso_DefaultEventManager_WfmoRaw_HandleInfo
                  // ============================================
class bteso_DefaultEventManager_WfmoRaw_HandleInfo {
    // This class stores event and index information for a 'HANDLE'.
  private:
    bteso_SocketHandle::Handle d_handle;
        // Socket handle this object corresponds to.
    long                       d_mask;
        // Bits encoding events subscribed for.
    int                        d_handleIndex;
        // Index into array of 'HANDLE's for the 'HANDLE' used as the key for
        // this instance of 'bteso_DefaultEventManager_WfmoRaw_HandleInfo'.

  public:
    // CREATORS
    bteso_DefaultEventManager_WfmoRaw_HandleInfo();
        // Construct a default instance of this object.

    bteso_DefaultEventManager_WfmoRaw_HandleInfo(
        const bteso_SocketHandle::Handle& handle,
        long                              mask,
        int                               handleIndex);
        // Construct an instance of this object from the specified components.

    bteso_DefaultEventManager_WfmoRaw_HandleInfo(
        const bteso_DefaultEventManager_WfmoRaw_HandleInfo& other);
        // Construct an instance of this object from another instance.

    // MANIPULATORS
    inline
    bteso_DefaultEventManager_WfmoRaw_HandleInfo&
            operator=(const bteso_DefaultEventManager_WfmoRaw_HandleInfo& rhs);
        // Assign to this state object the value of the specified 'rhs' object
        // and return a reference to this modifiable object.

    void setHandle(const bteso_SocketHandle::Handle& handle);
        // Set the 'handle'.

    void setMask(long mask);
        // Set the event mask for this object to the specified 'mask'.

    void setHandleIndex(int handleIndex);
        // Set the 'handleIndex'.

    // ACCESSORS.
    const bteso_SocketHandle::Handle& handle() const;
        // Return a const reference to 'handle'.

    long mask() const;
        // Return 'mask'.

    int handleIndex() const;
        // Return 'handleIndex'.
};

// Specialize 'btemf_IsBitWiseCopyable' for
// 'bteso_DefaultEventManager_WfmoRaw_HandleInfo's in order to be
// able to store 'HANDLE's in a 'std::vector' efficiently.
template <>
struct bdemf_IsBitwiseCopyable<bteso_DefaultEventManager_WfmoRaw_HandleInfo> {
    enum { VALUE = 1 };
};
                   // =======================================
                   // class bteso_DefaultEventManager_WfmoRaw
                   // =======================================

class bteso_DefaultEventManager_WfmoRaw {
    // This class provides a raw 'wfmo'-based socket event multiplexer.
    // This multiplexer has a limitied capacity -- not more than
    // 'MAX_NUM_HANDLES' (see below) different socket handles can be
    // simultaneously registered with this event manager.  Note that
    // though this class doesn't implement bteso_EventManager protocol, its
    // interface is a strict superset of 'bteso_EventManager' interface.
  public:
    enum {
        MAX_NUM_HANDLES = 64     // maximum number of socket handles that can
                                 // be registered with an event manager (of
                                 // this type)
                                 // Note that this limit is hardcoded on
                                 // Windows.
    };
  private:
    bdec2_Map<bteso_Event, bteso_EventManager::Callback, bteso_EventHash>
                         d_callbacks; // socket events and associated callbacks

    WSAEVENT             d_handles[MAX_NUM_HANDLES];

    std::vector<bteso_DefaultEventManager_WfmoRaw_HandleInfo>
                         d_sockets;

    bdec2_Map<bteso_SocketHandle::Handle, int,
              bteso_DefaultEventManager_WfmoRaw_HandleHash>
                         d_socketIndex;

    bteso_TimeMetrics   *d_timeMetric_p; // time metrics given to this object

    std::vector<int>     d_signaled;     // temporary array used by dispatch

    bdema_Allocator     *d_allocator_p;

  private:
    // not implemented
    bteso_DefaultEventManager_WfmoRaw(
                                   const bteso_DefaultEventManager_WfmoRaw&);
    bteso_DefaultEventManager_WfmoRaw&
                         operator=(const bteso_DefaultEventManager_WfmoRaw&);
  public:
    // CLASS METHODS
    static int canBeRegistered(const bteso_SocketHandle::Handle& handle);
        // Return 1 if the specified 'handle' can be registered with a
        // 'select'-based event manager and 0 otherwise.

    // CREATORS
    bteso_DefaultEventManager_WfmoRaw(bteso_TimeMetrics *timeMetric     = 0,
                                      bdema_Allocator   *basicAllocator = 0);
        // Create a 'wfmo'-based event manager.  Optionally specify a
        // 'timeMetric' to report time spent in CPU-bound and IO-bound
        // operations.  If 'timeMetric' is not specified or is 0, these metrics
        // are not reported.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~bteso_DefaultEventManager_WfmoRaw();
        // Destroy this event manager.  Note that the registered callbacks are
        // NOT invoked.

    // MANIPULATORS
    int dispatch(const bdet_TimeInterval&   timeout,
                 int                        flags);
        // For each pending socket event, invoke the corresponding callback
        // registered with this event manager.  If no event is pending, wait
        // until either (1) at least one event occurs (in which case the
        // corresponding callback(s) is invoked), (2) the specified 'timeout'
        // is exceeded, or (3) provided that the specified 'flags' includes
        // 'bteso_Flag::ASYNC_INTERRUPT', an underlying system call is
        // interrupted by a signal.  Return the number of dispatched callbacks
        // on success, 0 on timeout, and a negative value otherwise; -1 is
        // reserved to indicate that an underlying system call was interrupted.
        // When such an interruption occurs this method will return (-1) if
        // 'flags' includes 'bteso_Flag::ASYNC_INTERRUPT' and otherwise
        // will automatically restart (i.e., reissue the identical system
        // call).  Note that all callbacks are invoked in the same thread that
        // invokes 'dispatch', and the order of invocation, relative to the
        // order of registration, is unspecified.  Also note that -1 is never
        // returned if 'flags' includes
        // 'bteso_Flag::ASYNC_INTERRUPT'.  The time values are
        // supported with the resolution of 10 ms, and, all time values
        // provided to the multiplexer are rounded up.

    int dispatch(int flags);
        // For each pending socket event, invoke the corresponding callback
        // registered with this event manager.  If no event is pending, wait
        // until either (1) at least one event occurs (in which case the
        // corresponding callback(s) is invoked) or (2) provided that the
        // specified 'flags' includes 'bteso_Flag::ASYNC_INTERRUPT',
        // an underlying system call is interrupted by a signal.  Return the
        // number of dispatched callbacks on success, and a negative value
        // otherwise; -1 is reserved to indicate that an underlying system
        // call was interrupted.  When such an interruption occurs this method
        // will return (-1) if 'flags' includes
        // 'bteso_Flag::ASYNC_INTERRUPT' and otherwise will automatically
        // restart (i.e., reissue the identical system call).  Note that all
        // callbacks are invoked in the same thread that invokes 'dispatch',
        // and the order of invocation, relative to the order of registration,
        // is unspecified.  Also note that -1 is never returned if 'flags'
        // includes 'bteso_Flag::ASYNC_INTERRUPT'.

    int registerSocketEvent(const bteso_SocketHandle::Handle&   handle,
                            const bteso_EventType::Type         event,
                            const bteso_EventManager::Callback& callback);

        // Register with this event manager the specified 'callback' to be
        // invoked when the specified 'event' occurs on the specified socket
        // 'handle'.  Each socket event registration stays in effect until it
        // is subsequently deregistered; the callback is invoked each time
        // the corresponding event is detected.  'bteso_EventType::READ' and
        // 'bteso_EventType::WRITE' are the only events that can be registered
        // simulataneously for a socket.  If a registration attempt is made for
        // an event that is already registered, the callback associated with
        // this event will be overwritten with the new one.  Return 0 on
        // success and a non-zero value otherwise.  The behavior is
        // undefined unless 'handle' can be registered with this type of event
        // manager (as reported by 'canBeRegistered' method) and unless the
        // number of sockets registered with this event manager (as reported
        // by 'numSockets') doesn't exceed 'MAX_NUM_HANDLES'.  Simultaneous
        // registration of incompatible events for the same socket 'handle'
        // will also result in undefined behavior.  Note that the callback is
        // recurring (i.e.,  it remains registered until it is explicitly
        // deregistered).

    void moveSocket(bteso_DefaultEventManager_WfmoRaw *manager,
                    const bteso_SocketHandle::Handle&  handle);
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
        // this association may be violated by an invokation of any
        // manipulator on this event manager.
};

            // =======================================================
            // class bteso_DefaultEventManager<bteso_Platform::WFMO>
            // =======================================================

template<>
class bteso_DefaultEventManager<bteso_Platform::WFMO>
    : public bteso_EventManager
{
  public:
    enum {
        SPIN_PERIOD_MS  = 10,     // 10 ms to spin
        ONE_MILLISECOND = 1000000 // in nanoseconds
    };

  private:
    bteso_SpinningEventManager<bteso_DefaultEventManager_WfmoRaw>
          d_impl;

  public:
    // CREATORS
    bteso_DefaultEventManager(bteso_TimeMetrics *timeMetric     = 0,
                              bdema_Allocator   *basicAllocator = 0);
        // Create a 'wfmo'-based event manager.  Optionally specify a
        // 'timeMetric' to report time spent in CPU-bound and IO-bound
        // operations.  If 'timeMetric' is not specified or is 0, these metrics
        // are not reported.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~bteso_DefaultEventManager();
        // Destroy this object.  Note that the registered callbacks are NOT
        // invoked.

    // MANIPULATORS
    int dispatch(const bdet_TimeInterval&   timeout,
                 int                        flags);
        // For each pending socket event, invoke the corresponding callback
        // registered with this event manager.  If no event is pending, wait
        // until either (1) at least one event occurs (in which case the
        // corresponding callback(s) is invoked), (2) the specified 'timeout'
        // is exceeded, or (3) provided that the specified 'flags' includes
        // 'bteso_Flag::ASYNC_INTERRUPT', an underlying system call is
        // interrupted by a signal.  Return the number of dispatched callbacks
        // on success, 0 on timeout, and a negative value otherwise; -1 is
        // reserved to indicate that an underlying system call was interrupted.
        // When such an interruption occurs this method will return (-1) if
        // 'flags' includes 'bteso_Flag::ASYNC_INTERRUPT' and otherwise
        // will automatically restart (i.e., reissue the identical system
        // call).  Note that all callbacks are invoked in the same thread that
        // invokes 'dispatch', and the order of invocation, relative to the
        // order of registration, is unspecified.  Also note that -1 is never
        // returned if 'flags' includes
        // 'bteso_Flag::ASYNC_INTERRUPT'.  The time values are
        // supported with the resolution of 10 ms, and, all time values
        // provided to the multiplexer are rounded up.

    int dispatch(int flags);
        // For each pending socket event, invoke the corresponding callback
        // registered with this event manager.  If no event is pending, wait
        // until either (1) at least one event occurs (in which case the
        // corresponding callback(s) is invoked) or (2) provided that the
        // specified 'flags' includes 'bteso_Flag::ASYNC_INTERRUPT',
        // an underlying system call is interrupted by a signal.  Return the
        // number of dispatched callbacks on success, and a negative value
        // otherwise; -1 is reserved to indicate that an underlying system
        // call was interrupted.  When such an interruption occurs this method
        // will return (-1) if 'flags' includes
        // 'bteso_Flag::ASYNC_INTERRUPT' and otherwise will automatically
        // restart (i.e., reissue the identical system call).  Note that all
        // callbacks are invoked in the same thread that invokes 'dispatch',
        // and the order of invocation, relative to the order of registration,
        // is unspecified.  Also note that -1 is never returned if 'flags'
        // includes 'bteso_Flag::ASYNC_INTERRUPT'.

     int registerSocketEvent(const bteso_SocketHandle::Handle&   handle,
                             const bteso_EventType::Type         event,
                             const bteso_EventManager::Callback& callback);
        // Register with this event manager the specified 'callback' to be
        // invoked when the specified 'event' occurs on the specified socket
        // 'handle'.  Each socket event registration stays in effect until it
        // is subsequently deregistered; the callback is invoked each time
        // the corresponding event is detected.  'bteso_EventType::READ' and
        // 'bteso_EventType::WRITE' are the only events that can be registered
        // simulataneously for a socket.  If a registration attempt is made for
        // an event that is already registered, the callback associated with
        // this event will be overwritten with the new one.  Simultaneous
        // registration of incompatible events for the same socket 'handle'
        // will result in undefined behavior.
        // Return 0 on success and a non-zero value otherwise.

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

                   // ==========================================
                   // class bteso_DefaultEventManager_HandleHash
                   // ==========================================
inline int bteso_DefaultEventManager_WfmoRaw_HandleHash::hash(
    const bteso_SocketHandle::Handle& h, int size)
{
    return bdeimp_IntHash::hash((int)h, size);
}

inline int bteso_DefaultEventManager_WfmoRaw_HandleHash::hash(
    const HANDLE& h, int size)
{
    return bdeimp_IntHash::hash((int)h, size);
}

                   // ==========================================
                   // class bteso_DefaultEventManager_HandleInfo
                   // ==========================================

inline bteso_DefaultEventManager_WfmoRaw_HandleInfo::
        bteso_DefaultEventManager_WfmoRaw_HandleInfo()
{
}

inline
bteso_DefaultEventManager_WfmoRaw_HandleInfo::
    bteso_DefaultEventManager_WfmoRaw_HandleInfo(
        const bteso_SocketHandle::Handle& handle,
        long                              mask,
        int                               handleIndex)
: d_handle(handle)
, d_mask(mask)
, d_handleIndex(handleIndex)
{
}

inline
bteso_DefaultEventManager_WfmoRaw_HandleInfo::
    bteso_DefaultEventManager_WfmoRaw_HandleInfo(
        const bteso_DefaultEventManager_WfmoRaw_HandleInfo& other)
: d_handle(other.d_handle)
, d_mask(other.d_mask)
, d_handleIndex(other.d_handleIndex)
{
}

bteso_DefaultEventManager_WfmoRaw_HandleInfo&
bteso_DefaultEventManager_WfmoRaw_HandleInfo::operator=(
        const bteso_DefaultEventManager_WfmoRaw_HandleInfo& rhs)
{
        if (this != &rhs) {
                d_mask = rhs.d_mask;
                d_handle = rhs.d_handle;
                d_handleIndex = rhs.d_handleIndex;
        }
        return *this;
}

inline
void bteso_DefaultEventManager_WfmoRaw_HandleInfo::setHandle(
    const bteso_SocketHandle::Handle& handle)
{
    d_handle = handle;
}

inline
void bteso_DefaultEventManager_WfmoRaw_HandleInfo::setMask(long mask)
{
    d_mask = mask;
}

inline
void bteso_DefaultEventManager_WfmoRaw_HandleInfo::setHandleIndex(int index)
{
    d_handleIndex = index;
}

inline
const bteso_SocketHandle::Handle&
bteso_DefaultEventManager_WfmoRaw_HandleInfo::handle() const
{
    return d_handle;
}

inline
long bteso_DefaultEventManager_WfmoRaw_HandleInfo::mask() const
{
    return d_mask;
}

inline
int bteso_DefaultEventManager_WfmoRaw_HandleInfo::handleIndex() const
{
    return d_handleIndex;
}


                   // =======================================
                   // class bteso_DefaultEventManager_WfmoRaw
                   // =======================================

inline
int bteso_DefaultEventManager_WfmoRaw::canBeRegistered(
        const bteso_SocketHandle::Handle& handle)
{
    return 1;
}

inline
int bteso_DefaultEventManager_WfmoRaw::numEvents() const
{
    return d_callbacks.length();
}

inline
const bteso_Event& bteso_DefaultEventManager_WfmoRaw::event(int index) const
{
    bdec2_MapIter<bteso_Event, bteso_EventManager::Callback, bteso_EventHash>
                it(d_callbacks);
    int i = 0;
    for (int i = 0; i < index; ++i) {
        ++it;
    }
    return it.key();
}

inline
const bteso_EventManager::Callback&
bteso_DefaultEventManager_WfmoRaw::callback(int index) const
{
    bdec2_MapIter<bteso_Event, bteso_EventManager::Callback, bteso_EventHash>
                it(d_callbacks);
    int i = 0;
    for (int i = 0; i < index; ++i) {
        ++it;
    }
    return it.value();
}

inline int bteso_DefaultEventManager_WfmoRaw::numSockets() const {
  return d_sockets.size();
}

#endif // BTESO_PLATFORM__WIN_SOCKETS

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
