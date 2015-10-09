// btlso_timereventmanager.h                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_TIMEREVENTMANAGER
#define INCLUDED_BTLSO_TIMEREVENTMANAGER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for multiplexing sockets events and timers.
//
//@CLASSES:
//  btlso::TimerEventManager: socket event/timer multiplexer protocol
//
//@DESCRIPTION: The 'btlso::TimerEventManager' class provides the interface
// for an event manager that supports the registration of both timers and
// socket events, each along with an associated 'bsl::function' callback
// functor, which is invoked when the corresponding timer expires or the socket
// event occurs.
//
// Timers are single shot and are registered by specifying the (earliest) time
// (as a 'bsls::TimeInterval') at which the timer callback should be invoked.
// A unique ID for the timer is returned when the registration succeeds.
// Timers can be deregistered at any time before the timer expires by using
// this ID.  When a timer expires, the associated callback is invoked and this
// timer is automatically deregistered.
//
// Socket event registrations specify a socket handle and the type of event to
// monitor on the specified socket.  Socket event registrations stay in effect
// until they are subsequently deregistered; the callback is invoked each time
// the specified socket event occurs.  Note that 'btlso::EventType::e_READ' and
// 'btlso::EventType::e_WRITE' are the only events that can be registered
// simultaneously for a socket.  Otherwise only a single socket event can be
// registered for a particular socket.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing a Timed Socket Multiplexer
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example illustrates the implementation of periodic timed
// socket events using the (separate) timer and socket event registration
// mechanisms provided by 'btlso::TimerEventManager', such that a
// user-specified callback is invoked with different codes depending on whether
// the socket event occurs before the specified timeout or not.
//
// The interface for such functionality is provided by the class
// 'my_TimedSocketMultiplexer', which registers each specified socket event and
// an associated timer with an instance of 'btlso::TimerEventManager'.
//
// If the socket event occurs before the corresponding timer expires,
// 'btlso::TimerEventManager' invokes the socket event callback.  Within this
// callback 'my_TimedSocketMultiplexer' cancels the current timer and registers
// a new timer to expire after the specified period in order to wait for the
// next occurrence of the same socket event.
//
// If the timer expires before the socket event occurs
// 'btlso::TimerEventManager' invokes the timer callback.  Within this callback
// 'my_TimedSocketMultiplexer' registers a new timer to expire after the
// specified period, thus periodically rescheduling the timer until the socket
// event is seen (this behavior is for illustration only and real application
// code may handle such timeouts differently).
//
// 'my_TimedSocketMultiplexer' uses the helper class 'my_TimerInfo' to store
// the timer information associated with a socket/event pair (represented as a
// 'btlso::Event') and internally maintains a mapping of 'btlso::Event'
// instances to 'my_TimerInfo' instances (in a 'bsl::unordered_map').
//..
// class my_TimerInfo {
//     // This is a helper class that stores information about the timer
//     // associated with a socket/event pair (an instance of 'btlso::Event').
//
//     friend bool operator==(const my_TimerInfo& lhs,
//                            const my_TimerInfo& rhs);
//         // This class supports only in-core value semantics.
//
//   private:
//     bsls::TimeInterval  d_expiryTime; // current expiry time of the timer
//     bsls::TimeInterval  d_period;     // period of the (recurrent) timer
//     void               *d_id;         // unique timer identifier
//
//   public:
//     // CREATORS
//     my_TimerInfo();
//         // Create a default 'my_TimerInfo' instance.  Note that such a
//         // default constructor might be required, e.g., for values stored in
//         // 'bsl::unordered_map'.
//
//     my_TimerInfo(const bsls::TimeInterval&  expiryTime,
//                  const bsls::TimeInterval&  period,
//                  void                      *id);
//         // Create a 'my_TimerInfo' containing the specified 'expiryTime',
//         // 'period', and 'id'.
//
//     ~my_TimerInfo();
//         // Destroy this object.
//
//     // MANIPULATORS
//     void setExpiryTime(const bsls::TimeInterval& expiryTime);
//         // Set the specified 'expiryTime'.
//
//     void setPeriod(const bsls::TimeInterval& period);
//         // Set the specified 'period'.
//
//     void setId(void *id);
//         // Set the specified 'id'.
//
//     // ACCESSORS
//     const bsls::TimeInterval& expiryTime() const;
//         // Return a reference to the timer's expiry time.
//
//     const bsls::TimeInterval& period() const;
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
// my_TimerInfo::my_TimerInfo(const bsls::TimeInterval&  expiryTime,
//                            const bsls::TimeInterval&  period,
//                            void                      *id)
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
// void my_TimerInfo::setExpiryTime(const bsls::TimeInterval& expiryTime)
// {
//     d_expiryTime = expiryTime;
// }
//
// void my_TimerInfo::setPeriod(const bsls::TimeInterval& period)
// {
//     d_period = period;
// }
//
// void my_TimerInfo::setId(void *id)
// {
//     d_id = id;
// }
//
// const bsls::TimeInterval& my_TimerInfo::expiryTime() const
// {
//     return d_expiryTime;
// }
//
// const bsls::TimeInterval& my_TimerInfo::period() const
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
//            && lhs.period()  == rhs.period()
//            && lhs.id()      == rhs.id();
// }
//
// class my_TimedSocketMultiplexer {
//     // This class implements a subset of a socket event multiplexer that
//     // supports the registration of timed socket events and associated
//     // callbacks.  Specifically, this class allows a user specified
//     // 'bsl::function<void(my_TimedSocketMultiplexer::CallbackCode)>'
//     // functor to be registered via the 'registerTimedSocketEvent' method.
//     // This functor is invoked with an argument of
//     // 'my_TimedSocketMultiplexer::e_SOCKET_EVENT' if the socket event
//     // occurs before the timeout interval or with an argument of
//     // 'my_TimedSocketMultiplexer::e_TIMEOUT' when the timeout occurs
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
//
//         e_SOCKET_EVENT = 0,  // The specified socket event has occurred.
//         e_TIMEOUT      = 1   // The timer has expired before the specified
//                              // socket event occurred.
//     };
//
//   private:
//     typedef bsl::unordered_map<btlso::Event, my_TimerInfo, btlso::EventHash>
//                                             EventTimeMap;
//
//     EventTimeMap                            d_eventTimeMap;
//
//     btlso::TimerEventManager               *d_manager_p;
//
//   private:
//     // Private methods 'eventCb' and 'timerCb' are internal callback member
//     // functions registered with 'btlso::TimerEventManager'.
//
//     void eventCb(const btlso::Event&                       socketEvent,
//                  const bsl::function<void(CallbackCode)>&  userCb,
//                  const btlso::TimerEventManager::Callback& internalCb);
//         // Cancel the current timer and register a new timer to expire
//         // after the specified period from the current time along with the
//         // internal timer callback functor 'internalCb' to be invoked when
//         // the timer expires.  Invoke the user specified callback 'userCb'
//         // with the argument 'my_TimedSocketMultiplexer::e_SOCKET_EVENT'.
//         // Note that this method is registered as a callback with
//         // 'btlso::TimerEventManager' and is invoked to indicate the
//         // occurrence of the specified socket event 'socketEvent'.
//
//     void timerCb(const btlso::Event&                       socketEvent,
//                  const bsl::function<void(CallbackCode)>&  userCb,
//                  const btlso::TimerEventManager::Callback& internalCb);
//         // Register a new timer to expire after the specified time period
//         // measured from the previous timer's expiry time along with
//         // the internal timer callback functor 'internalCb' to be
//         // invoked when the new timer expires.  Invoke the user
//         // specified callback 'userCb' with the argument
//         // 'my_TimedSocketMultiplexer::e_TIMEOUT'.  Note that 'timerCb' is
//         // registered as a callback with 'btlso::TimerEventManager' and is
//         // invoked to indicate the expiry of the timer associated with the
//         // specified socket event 'socketEvent'.
//
//   public:
//     // CREATORS
//     my_TimedSocketMultiplexer(btlso::TimerEventManager *manager_p);
//         // Create an empty multiplexer object.
//
//     // MANIPULATORS
//     int registerTimedSocketEvent(
//                       const btlso::SocketHandle::Handle&       handle,
//                       btlso::EventType::Type                   event,
//                       const bsls::TimeInterval&                period,
//                       const bsl::function<void(CallbackCode)>& userCb);
//         // Register the specified 'userCb' functor to be invoked whenever
//         // the specified 'event' occurs on the specified 'handle' or when
//         // 'event' has not occurred within the specified 'period' of time.
//         // Return 0 on successful registration, and a nonzero value
//         // otherwise.
//
//     int deregisterTimedSocketEvent(const SocketHandle::Handle& handle,
//                                    EventType::Type             event);
//         // Deregister the callback associated with the specified 'handle'
//         // and 'event'.  Return 0 on successful deregistration and a
//         // nonzero value otherwise.
// };
//
// void my_TimedSocketMultiplexer::eventCb(
//                   const btlso::Event&                       socketEvent,
//                   const bsl::function<void(CallbackCode)>&  userCb,
//                   const btlso::TimerEventManager::Callback& internalCb)
// {
//     // Retrieve the timer information associated with 'socketEvent'.
//
//     EventTimeMap::iterator socketEventIt = d_eventTimeMap.find(socketEvent);
//     ASSERT(d_eventTimeMap.end() != socketEventIt);
//     my_TimerInfo *timerInfo = &socketEventIt->second;
//
//     // Deregister the current timer callback.
//
//     d_manager_p->deregisterTimer(timerInfo->id());
//
//     // Set the new timeout value.
//
//     timerInfo->setExpiryTime(bdlt::CurrentTime::now()
//                                                      + timerInfo->period());
//
//     // Register a new timer callback to fire at this time.
//
//     timerInfo->setId(
//            d_manager_p->registerTimer(timerInfo->expiryTime(), internalCb));
//
//     // Invoke userCb with an argument of 'e_SOCKET_EVENT' to indicate that
//     // 'socketEvent' has occurred.
//
//     userCb(e_SOCKET_EVENT);
// }
//
// void my_TimedSocketMultiplexer::timerCb(
//                   const btlso::Event&                       socketEvent,
//                   const bsl::function<void(CallbackCode)>&  userCb,
//                   const btlso::TimerEventManager::Callback& internalCb)
// {
//     // Retrieve the timer information associated with 'socketEvent' and set
//     // the new expiry time.
//
//     EventTimeMap::iterator socketEventIt = d_eventTimeMap.find(socketEvent);
//     ASSERT(d_eventTimeMap.end() != socketEventIt);
//     my_TimerInfo *timerInfo = &socketEventIt->second;
//     timerInfo->setExpiryTime(timerInfo->expiryTime() + timerInfo->period());
//
//     // Register a new timer callback to fire at that time.
//
//     timerInfo->setId(d_manager_p->registerTimer(timerInfo->expiryTime(),
//                                                 internalCb));
//
//     // Invoke user callback functor with an argument of 'e_TIMEOUT' to
//     // indicate that a timeout has occurred before 'socketEvent'.
//
//     userCb(e_TIMEOUT);
// }
//
// my_TimedSocketMultiplexer::my_TimedSocketMultiplexer(
//                                         btlso::TimerEventManager *manager_p)
// : d_eventTimeMap()
// , d_manager_p(manager_p)
// {
// }
//
// int my_TimedSocketMultiplexer::registerTimedSocketEvent(
//                        const btlso::SocketHandle::Handle&       handle,
//                        btlso::EventType::Type                   event,
//                        const bsls::TimeInterval&                period,
//                        const bsl::function<void(CallbackCode)>& userCb)
// {
//     btlso::Event socketEvent(handle, event);
//     bsls::TimeInterval expiryTime = bdlt::CurrentTime::now() + period;
//
//     // Create a timer callback.
//
//  btlso::TimerEventManager::Callback myTimerCb;
//  myTimerCb = bdlf::BindUtil::bind(
//           bdlf::MemFnUtil::memFn(&my_TimedSocketMultiplexer::timerCb, this),
//           socketEvent,
//           userCb,
//           myTimerCb);
//
//     // Create an event callback.
//
//    btlso::TimerEventManager::Callback myEventCb(bdlf::BindUtil::bind(
//           bdlf::MemFnUtil::memFn(&my_TimedSocketMultiplexer::eventCb, this),
//           socketEvent,
//           userCb,
//           myTimerCb));
//
//     // Register the event callback.
//
//     d_manager_p->registerSocketEvent(handle, event, myEventCb);
//
//     // Register the timer callback.
//
//     void *timerHandle = d_manager_p->registerTimer(expiryTime, myTimerCb);
//
//     // Save the timer information associated with this event in the map.
//
//     my_TimerInfo timerInfo(expiryTime, period, timerHandle);
//     d_eventTimeMap.insert(bsl::make_pair(socketEvent, timerInfo));
//
//     return 0;
// }
//
// int my_TimedSocketMultiplexer::deregisterTimedSocketEvent(
//                                   const btlso::SocketHandle::Handle& handle,
//                                   btlso::EventType::Type             event)
// {
//
//     // Retrieve timer information for this event.
//
//     btlso::Event socketEvent(handle, event);
//     EventTimeMap::iterator socketEventIt = d_eventTimeMap.find(socketEvent);
//     my_TimerInfo *timerInfo = &socketEventIt->second;
//     if (d_eventTimeMap.end() != socketEventIt) {
//         return -1;                                                 // RETURN
//     }
//
//     // Deregister this socket event.
//
//     d_manager_p->deregisterSocketEvent(handle, event);
//
//     // Deregister timer
//
//     d_manager_p->deregisterTimer(timerInfo->id());
//
//     // Remove timer information for this event from the map.
//
//     d_eventTimeMap.erase(socketEventIt);
//
//     return 0;
// }
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#include <btlso_sockethandle.h>
#endif

#ifndef INCLUDED_BTLSO_EVENTTYPE
#include <btlso_eventtype.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

namespace BloombergLP {

namespace bsls { class TimeInterval; }

namespace btlso {

                   // =======================
                   // class TimerEventManager
                   // =======================

class TimerEventManager {
    // This class provides the interface for an event manager that supports the
    // registration of both timers and socket events.

  public:
    // TYPES
    typedef bsl::function<void()> Callback;

    // CREATORS
    virtual ~TimerEventManager();
        // Destroy this 'TimerEventManager' object.

    // MANIPULATORS
    virtual int registerSocketEvent(
                              const SocketHandle::Handle&        handle,
                              EventType::Type                    event,
                              const TimerEventManager::Callback& callback) = 0;
        // Register the occurrence of the specified 'event' on the specified
        // socket 'handle' such that the specified 'callback' functor is
        // invoked when this event occurs.  Return 0 on success and a negative
        // value otherwise.  Socket event registrations stay in effect until
        // they are subsequently deregistered; the callback is invoked each
        // time the specified 'event' is seen.  Typically, 'EventType::e_READ'
        // and 'EventType::e_WRITE' are the only events that can be registered
        // simultaneously for a socket.  Simultaneously registering for
        // incompatible events for the same socket 'handle' will result in
        // undefined behavior.  If a registration attempt is made for an event
        // that is already registered, the callback associated with this event
        // will be overwritten with the new one.

    virtual void *registerTimer(
                              const bsls::TimeInterval&          expiryTime,
                              const TimerEventManager::Callback& callback) = 0;
        // Register a timer such that the specified 'callback' functor will be
        // invoked after the specified 'expiryTime' is reached.  Return a
        // unique identifier for the registered timer.  Note that a timer can
        // be deregistered at any time before it expires by passing this
        // identifier to the 'deregisterTimer' method.  Specifying an
        // 'expiryTime' earlier than the current time
        // ('bdlt::CurrentTime::now()') will result in the associated
        // 'callback' being invoked the first time that the callbacks are
        // dispatched.

    virtual int rescheduleTimer(const void                *timerId,
                                const bsls::TimeInterval&  expiryTime) = 0;
        // Reschedule the timer indicated by the specified 'timerId' such that
        // the callback function supplied to 'registerTimer' will be invoked
        // after the specified 'expiryTime' is reached.  Return 0 on success,
        // and a negative value otherwise.  'expiryTime' is expressed as the
        // absolute time from 00:00:00 UTC, January 1, 1970 (the epoch time
        // defined by 'bdetu_epoch').  The behavior is undefined unless
        // 'timerId' is a timer id returned from 'registerTimer', and has not
        // subsequently been deregistered.  Note that if 'expiryTime' is
        // earlier than the current time ('bdlt::CurrentTime::now()') the
        // associated callback will be invoked the first time that the
        // callbacks are dispatched.

    virtual void deregisterSocketEvent(const SocketHandle::Handle& handle,
                                       EventType::Type             event) = 0;
        // Deregister the occurrence of the specified 'event' on the specified
        // socket 'handle'.  The behavior is undefined unless there is a
        // callback registered for the 'event' on the socket 'handle'.

    virtual void deregisterSocket(const SocketHandle::Handle& handle) = 0;
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
    virtual bool hasLimitedSocketCapacity() const = 0;
        // Return 'true' if this event manager has a limited socket capacity,
        // and 'false' otherwise.

    virtual int numTimers() const = 0;
        // Return the number of timers that are currently registered.

    virtual int numSocketEvents(const SocketHandle::Handle& handle) const = 0;
        // Return the number of socket events currently registered for the
        // specified 'handle'.

    virtual int numEvents() const = 0;
        // Return the total number of timers and socket events that are
        // currently registered.

    virtual int isRegistered(const SocketHandle::Handle&  handle,
                             EventType::Type              event) const = 0;
        // Return 1 if the specified 'event' is registered for the specified
        // socket 'handle' and 0 otherwise.
};

}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
