// bteso_timereventmanager.h  -*-C++-*-
#ifndef INCLUDED_BTESO_TIMEREVENTMANAGER
#define INCLUDED_BTESO_TIMEREVENTMANAGER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for multiplexing sockets events and timers.
//
//@CLASSES:
//   bteso_TimerEventManager: socket event/timer multiplexer protocol
//
//@AUTHOR: Cheenu Srinivasan (csriniva)
//
//@DESCRIPTION: The 'bteso_TimerEventManager' component provides the interface
// for an event manager that supports the registration of both timers and
// socket events, each along with an associated 'bdef_Function' callback
// functor, which is invoked when the corresponding timer expires or the socket
// event occurs.
//
// Timers are single shot and are registered by specifying the (earliest) time
// (as a 'bdet_TimeInterval') at which the timer callback should be invoked.  A
// unique ID for the timer is returned when the registration succeeds.  Timers
// can be deregistered at any time before the timer expires by using this ID.
// When a timer expires, the associated callback is invoked and this timer is
// automatically deregistered.
//
// Socket event registrations specify a socket handle and the type of event to
// monitor on the specified socket.  Socket event registrations stay in effect
// until they are subsequently deregistered; the callback is invoked each time
// the specified socket event occurs.  Note that 'bteso_EventType::BTESO_READ'
// and 'bteso_EventType::BTESO_WRITE' are the only events that can be
// registered simultaneously for a socket.  Otherwise only a single socket
// event can be registered for a particular socket.
//
///Usage
///-----
// The following example illustrates the implementation of periodic timed
// socket events using the (separate) timer and socket event registration
// mechanisms provided by 'bteso_TimerEventManager', such that a user-specified
// callback is invoked with different codes depending on whether the socket
// event occurs before the specified timeout or not.
//
// The interface for such functionality is provided by the class
// 'my_TimedSocketMultiplexer', which registers each specified socket event and
// an associated timer with an instance of 'bteso_TimerEventManager'.
//
// If the socket event occurs before the corresponding timer expires,
// 'bteso_TimerEventManager' invokes the socket event callback.  Within this
// callback 'my_TimedSocketMultiplexer' cancels the current timer and registers
// a new timer to expire after the specified period in order to wait for the
// next occurrence of the same socket event.
//
// If the timer expires before the socket event occurs
// 'bteso_TimerEventManager' invokes the timer callback.  Within this callback
// 'my_TimedSocketMultiplexer' registers a new timer to expire after the
// specified period, thus periodically rescheduling the timer until the socket
// event is seen (this behavior is for illustration only and real application
// code may handle such timeouts differently).
//
// 'my_TimedSocketMultiplexer' uses the helper class 'my_TimerInfo' to store
// the timer information associated with a socket/event pair (represented as a
// 'bteso_Event') and internally maintains a mapping of 'bteso_Event' instances
// to 'my_TimerInfo' instances (in a 'bsl::hash_map').
//
//..
// class my_TimerInfo {
//     // This is a helper class that stores information about the timer
//     // associated with a socket/event pair (an instance of 'bteso_Event').
//
//     friend bool operator==(const my_TimerInfo& lhs,
//                                                    const my_TimerInfo& rhs);
//         // This class supports only in-core value semantics.
//
//   private:
//     bdet_TimeInterval  d_expiryTime; // current expiry time of the timer
//     bdet_TimeInterval  d_period;     // period of the (recurrent) timer
//     void              *d_id;         // unique timer identifier
//
//   public:
//     // CREATORS
//     my_TimerInfo();
//         // Create a default 'my_TimerInfo' instance.  Note that such a
//         // default constructor might be required, e.g., for values stored in
//         // 'bsl::hash_map'.
//
//     my_TimerInfo(const bdet_TimeInterval&  expiryTime,
//                  const bdet_TimeInterval&  period,
//                  void                     *id);
//         // Create a 'my_TimerInfo' containing the specified 'expiryTime',
//         // 'period', and 'id'.
//
//     ~my_TimerInfo();
//         // Destroy this object.
//
//     // MANIPULATORS
//     void setExpiryTime(const bdet_TimeInterval& expiryTime);
//         // Set the specified 'expiryTime'.
//
//     void setPeriod(const bdet_TimeInterval& period);
//         // Set the specified 'period'.
//
//     void setId(void *id);
//         // Set the specified 'id'.
//
//     // ACCESSORS
//     const bdet_TimeInterval& expiryTime() const;
//         // Return a reference to the timer's expiry time.
//
//     const bdet_TimeInterval& period() const;
//         // Return a reference to the timer's period.
//
//     const void *id() const;
//         // Return the timer's ID.
// };
//
// bool operator==(const my_TimerInfo& lhs, const my_TimerInfo& rhs);
//     // Return 'true' if the specified 'lhs' and 'rhs' timer information
//     // objects have the same value, and 'false' otherwise.  Two timer
//     // information objects have the same value if they have the same
//     // respective values of expiry time, period, and ID.
//
// my_TimerInfo::my_TimerInfo()
// {
// }
//
// my_TimerInfo::my_TimerInfo(
//         const bdet_TimeInterval&  expiryTime,
//         const bdet_TimeInterval&  period,
//         void                     *id)
// : d_expiryTime(expiryTime)
// , d_period(period)
// , d_id(id)
// {
// }
//
// my_TimerInfo::~my_TimerInfo()
// {
// }
//
// void my_TimerInfo::setExpiryTime(const bdet_TimeInterval& expiryTime)
// {
//     d_expiryTime = expiryTime;
// }
//
// void my_TimerInfo::setPeriod(const bdet_TimeInterval& period)
// {
//     d_period = period;
// }
//
// void my_TimerInfo::setId(void *id)
// {
//     d_id = id;
// }
//
// const bdet_TimeInterval& my_TimerInfo::expiryTime() const
// {
//     return d_expiryTime;
// }
//
// const bdet_TimeInterval& my_TimerInfo::period() const
// {
//     return d_period;
// }
//
// const void *my_TimerInfo::id() const
// {
//     return d_id;
// }
//
// bool operator==(const my_TimerInfo& lhs, const my_TimerInfo& rhs)
// {
//     return lhs.expiryTime() == rhs.expiryTime()
//            && lhs.period() == rhs.period()
//            && lhs.id() == rhs.id();
// }
//
// class my_TimedSocketMultiplexer {
//     // This class implements a subset of a socket event multiplexer that
//     // supports the registration of timed socket events and associated
//     // callbacks.  Specifically, this class allows a user specified
//     // 'bdef_Function<void (*)(my_TimedSocketMultiplexer::CallbackCode)>'
//     // functor to be registered via the 'registerTimedSocketEvent' method.
//     // This functor is invoked with an argument of
//     // 'my_TimedSocketMultiplexer::SOCKET_EVENT' if the socket event
//     // occurs before the timeout interval or with an argument of
//     // 'my_TimedSocketMultiplexer::TIMEOUT' when the timeout occurs
//     // before an occurrence of the specified socket event.  Each time the
//     // callback is invoked, the timer is rescheduled to expire
//     // after the specified time period.
//
//   public:
//
//     // TYPES
//     enum CallbackCode {
//         // Enumerations used to indicate the reason the user callback
//         // functor is being invoked.
//         SOCKET_EVENT = 0,  // The specified socket event has occurred.
//         TIMEOUT      = 1   // The timer has expired before the specified
//                            // socket event occurred.
//     };
//
//   private:
//     typedef bsl::hash_map<bteso_Event, my_TimerInfo, bteso_EventHash>
//                                             EventTimeMap;
//
//     EventTimeMap                            d_eventTimeMap;
//
//     bteso_TimerEventManager                *d_manager_p;
//
//   private:
//     // Private methods 'eventCb' and 'timerCb' are internal callback member
//     // functions registered with 'bteso_TimerEventManager'.
//
//     void eventCb(
//             const bteso_Event&                             socketEvent,
//             const bdef_Function<void (*)(CallbackCode)>&   userCb,
//             const bteso_TimerEventManager::Callback&       internalCb);
//         // Cancel the current timer and register a new timer to expire
//         // after the specified period from the current time along with the
//         // internal timer callback functor 'internalCb' to be invoked when
//         // the timer expires.  Invoke the user specified callback 'userCb'
//         // with the argument 'my_TimedSocketMultiplexer::SOCKET_EVENT'.
//         // Note that this method is registered as a callback with
//         // 'bteso_TimerEventManager' and is invoked to indicate the
//         // occurrence of the specified socket event 'socketEvent'.
//
//     void timerCb(
//             const bteso_Event&                             socketEvent,
//             const bdef_Function<void (*)(CallbackCode)>&   userCb,
//             const bteso_TimerEventManager::Callback&       internalCb);
//         // Register a new timer to expire after the specified time period
//         // measured from the previous timer's expiry time along with
//         // the internal timer callback functor 'internalCb' to be
//         // invoked when the new timer expires.  Invoke the user
//         // specified callback 'userCb' with the argument
//         // 'my_TimedSocketMultiplexer::TIMEOUT'.
//         // Note that 'timerCb' is registered as a callback with
//         // 'bteso_TimerEventManager' and is invoked to indicate the
//         // expiry of the timer associated with the specified socket event
//         // 'socketEvent'.
//
//   public:
//     // CREATORS
//     my_TimedSocketMultiplexer(bteso_TimerEventManager *manager_p);
//         // Create an empty multiplexer object.
//
//     // MANIPULATORS
//     int registerTimedSocketEvent(
//             const bteso_SocketHandle::Handle&            handle,
//             bteso_EventType::Type                        event,
//             const bdet_TimeInterval&                     period,
//             const bdef_Function<void (*)(CallbackCode)>& userCb);
//         // Register the specified 'userCb' functor to be invoked whenever
//         // the specified 'event' occurs on the specified 'handle' or when
//         // 'event' has not occurred within the specified 'period' of time.
//         // Return 0 on successful registration, and a nonzero value
//         // otherwise.
//
//     int deregisterTimedSocketEvent(
//             const bteso_SocketHandle::Handle& handle,
//             bteso_EventType::Type             event);
//         // Deregister the callback associated with the specified 'handle'
//         // and 'event'.  Return 0 on successful deregistration and a
//         // nonzero value otherwise.
// };
//
// void my_TimedSocketMultiplexer::eventCb(
//         const bteso_Event&                             socketEvent,
//         const bdef_Function<void (*)(CallbackCode)>&   userCb,
//         const bteso_TimerEventManager::Callback&       internalCb)
// {
//     // Retrieve the timer information associated with 'socketEvent'.
//     EventTimeMap::iterator socketEventIt = d_eventTimeMap.find(socketEvent);
//     ASSERT(d_eventTimeMap.end() != socketEventIt);
//     my_TimerInfo *timerInfo = &socketEventIt->second;
//
//     // Deregister the current timer callback.
//     d_manager_p->deregisterTimer(timerInfo->id());
//
//     // Set the new timeout value.
//     timerInfo->setExpiryTime(bdetu_SystemTime::now() + timerInfo->period());
//
//     // Register a new timer callback to fire at this time.
//     timerInfo->setId(
//            d_manager_p->registerTimer(timerInfo->expiryTime(), internalCb));
//
//     // Invoke userCb with an argument of 'SOCKET_EVENT' to indicate that
//     // 'socketEvent' has occurred.
//     userCb(SOCKET_EVENT);
// }
//
// void my_TimedSocketMultiplexer::timerCb(
//         const bteso_Event&                             socketEvent,
//         const bdef_Function<void (*)(CallbackCode)>&   userCb,
//         const bteso_TimerEventManager::Callback&       internalCb)
// {
//     // Retrieve the timer information associated with 'socketEvent' and set
//     // the new expiry time.
//     EventTimeMap::iterator socketEventIt = d_eventTimeMap.find(socketEvent);
//     ASSERT(d_eventTimeMap.end() != socketEventIt);
//     my_TimerInfo *timerInfo = &socketEventIt->second;
//     timerInfo->setExpiryTime(timerInfo->expiryTime() + timerInfo->period());
//
//     // Register a new timer callback to fire at that time.
//     timerInfo->setId(
//            d_manager_p->registerTimer(timerInfo->expiryTime(), internalCb));
//
//     // Invoke user callback functor with an argument of 'TIMEOUT' to
//     // indicate that a timeout has occurred before 'socketEvent'.
//     userCb(TIMEOUT);
// }
//
// my_TimedSocketMultiplexer::my_TimedSocketMultiplexer(
//         bteso_TimerEventManager *manager_p)
// : d_eventTimeMap()
// , d_manager_p(manager_p)
// {
// }
//
// int my_TimedSocketMultiplexer::registerTimedSocketEvent(
//         const bteso_SocketHandle::Handle&            handle,
//         bteso_EventType::Type                        event,
//         const bdet_TimeInterval&                     period,
//         const bdef_Function<void (*)(CallbackCode)>& userCb)
// {
//     bteso_Event socketEvent(handle, event);
//     bdet_TimeInterval expiryTime = bdetu_SystemTime::now() + period;
//
//     // Create a timer callback.
//     bteso_TimerEventManager::Callback myTimerCb(
//         bdef_BindUtil::bind(&timerCb, this, socketEvent, userCb, myTimerCb);
//
//     // Create an event callback.
//     bteso_TimerEventManager::Callback myEventCb(
//         bdef_BindUtil::bind(&eventCb, this, socketEvent, userCb, myTimerCb);
//
//     // Register the event callback.
//     d_manager_p->registerSocketEvent(handle, event, myEventCb);
//
//     // Register the timer callback.
//     void *timerHandle = d_manager_p->registerTimer(expiryTime, myTimerCb);
//
//     // Save the timer information associated with this event in the map.
//     my_TimerInfo timerInfo(expiryTime, period, timerHandle);
//     d_eventTimeMap.insert(bsl::make_pair(socketEvent, timerInfo));
//
//     return 0;
// }
//
// int my_TimedSocketMultiplexer::deregisterTimedSocketEvent(
//         const bteso_SocketHandle::Handle& handle,
//         bteso_EventType::Type             event)
// {
//
//     // Retrieve timer information for this event.
//     bteso_Event socketEvent(handle, event);
//     EventTimeMap::iterator socketEventIt = d_eventTimeMap.find(socketEvent);
//     my_TimerInfo *timerInfo = &socketEventIt->second;
//     if (d_eventTimeMap.end() != socketEventIt) {
//         return -1;
//     }
//
//     // Deregister this socket event.
//     d_manager_p->deregisterSocketEvent(handle, event);
//
//     // Deregister timer
//     d_manager_p->deregisterTimer(timerInfo->id());
//
//     // Remove timer information for this event from the map.
//     d_eventTimeMap.erase(socketEventIt);
//
//     return 0;
// }
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_SOCKETHANDLE
#include <bteso_sockethandle.h>
#endif

#ifndef INCLUDED_BTESO_EVENTTYPE
#include <bteso_eventtype.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

namespace BloombergLP {

class bdet_TimeInterval;

class bteso_TimerEventManager {
    // This class provides the interface for an event manager that supports the
    // registration of both timers and socket events.

  public:
    // TYPES
    typedef bdef_Function<void (*)()> Callback;

    // CREATORS
    virtual ~bteso_TimerEventManager();
        // Destroy this 'bteso_TimerEventManager' object.

    // MANIPULATORS
    virtual int registerSocketEvent(
                        const bteso_SocketHandle::Handle&        handle,
                        bteso_EventType::Type              event,
                        const bteso_TimerEventManager::Callback& callback) = 0;
        // Register the occurrence of the specified 'event' on the specified
        // socket 'handle' such that the specified 'callback' functor is
        // invoked when this event occurs.  Return 0 on success and a negative
        // value otherwise.  Socket event registrations stay in effect until
        // they are subsequently deregistered; the callback is invoked each
        // time the specified 'event' is seen.  Typically,
        // 'bteso_EventType::BTESO_READ' and 'bteso_EventType::BTESO_WRITE' are
        // the only events that can be registered simultaneously for a socket.
        // Simultaneously registering for incompatible events for the same
        // socket 'handle' will result in undefined behavior.  If a
        // registration attempt is made for an event that is already
        // registered, the callback associated with this event will be
        // overwritten with the new one.

    virtual void *registerTimer(
                        const bdet_TimeInterval&                 expiryTime,
                        const bteso_TimerEventManager::Callback& callback) = 0;
        // Register a timer such that the specified 'callback' functor will be
        // invoked after the specified 'expiryTime' is reached.  Return a
        // unique identifier for the registered timer.  Note that a timer can
        // be deregistered at any time before it expires by passing this
        // identifier to the 'deregisterTimer' method.  Specifying an
        // 'expiryTime' earlier than the current time
        // ('bdetu_SystemTime::now()') will result in the associated 'callback'
        // being invoked the first time that the callbacks are dispatched.

    virtual int rescheduleTimer(const void               *timerId,
                                const bdet_TimeInterval&  expiryTime) = 0;
        // Reschedule the timer indicated by the specified 'timerId' such that
        // the callback function supplied to 'registerTimer' will be invoked
        // after the specified 'expiryTime' is reached.  Return 0 on success,
        // and a negative value otherwise.  'expiryTime' is expressed as the
        // absolute time from 00:00:00 UTC, January 1, 1970 (the epoch time
        // defined by 'bdetu_epoch').  The behavior is undefined unless
        // 'timerId' is a timer id returned from 'registerTimer', and has not
        // subsequently been deregistered.  Note that if 'expiryTime' is
        // earlier than the current time ('bdetu_SystemTime::now()') the
        // associated callback will be invoked the first time that the
        // callbacks are dispatched.

    virtual void deregisterSocketEvent(
                                  const bteso_SocketHandle::Handle& handle,
                                  bteso_EventType::Type             event) = 0;
        // Deregister the occurrence of the specified 'event' on the specified
        // socket 'handle'.  The behavior is undefined unless there is a
        // callback registered for the 'event' on the socket 'handle'.

    virtual void deregisterSocket(
                const bteso_SocketHandle::Handle& handle) = 0;
        // Deregister all events associated with the specified socket 'handle'.

    virtual void deregisterAllSocketEvents() = 0;
        // Deregister all events on every socket handle.

    virtual void deregisterTimer(const void *timerId) = 0;
        // Deregister the timer identified by 'timerId' (returned when the
        // timer was registered using the 'registerTimer' method).

    virtual void deregisterAllTimers() = 0;
        // Deregister all timers that are currently registered.

    virtual void deregisterAll() = 0;
        // Deregister all timers and socket events that are currently
        // registered.

    // ACCESSORS
    bool canRegisterSockets() const = 0;
        // Return 'true' if this event manager can register additional sockets,
        // and 'false' otherwise.  Note that if 'canRegisterSockets' is
        // 'false' then a subsequent call to register an event (without an
        // intervening call to deregister an event) will result in undefined
        // behavior.

    bool hasLimitedSocketCapacity() const = 0;
        // Return 'true' if this event manager has a limited socket capacity,
        // and 'false' otherwise.  Note that if 'hasLimitedSocketCapacity' is
        // 'true' then 'canRegisterSockets' may either return 'true' or
        // 'false' depending on whether the socket capacity of this event
        // manager has been reached, but if 'hasLimitedSocketCapacity' is
        // 'false' then 'canRegisterSockets' is (always) 'true'.

    virtual int numTimers() const = 0;
        // Return the number of timers that are currently registered.

    virtual int numSocketEvents(
                const bteso_SocketHandle::Handle& handle) const = 0;
        // Return the number of socket events currently registered for the
        // specified 'handle'.

    virtual int numEvents() const = 0;
        // Return the total number of timers and socket events currently
        // registered for.

    virtual int isRegistered(
                 const bteso_SocketHandle::Handle&  handle,
                 bteso_EventType::Type              event) const = 0;
        // Return 1 if the specified 'event' is registered for the specified
        // socket 'handle' and 0 otherwise.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
