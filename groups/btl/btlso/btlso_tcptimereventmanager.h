// btlso_tcptimereventmanager.h                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_TCPTIMEREVENTMANAGER
#define INCLUDED_BTLSO_TCPTIMEREVENTMANAGER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a multiplexer of events on sockets and timers.
//
//@CLASSES:
//  btlso::TcpTimerEventManager: socket-event and timer multiplexer
//
//@SEE_ALSO: btlso_eventmanager, btlso_timereventmanager
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'btlso_timereventmanager' protocol for socket handles.  An interested party
// can register a callback to be invoked whenever a particular event happens on
// a particular socket, or when a timer expires.  Socket events have
// "permanent" semantics, i.e., once registered, a callback is invoked whenever
// appropriate events happen until it is explicitly deregistered.  Timers have
// "one-time" semantics -- a timer callback is invoked at most once.  A
// registered callback can only be invoked from the 'dispatch' method.
// Registering the same socket handle and the same event with two different
// instances of this event managers may result in undefined behavior of an
// application.
//
// This event manager can optimize its performance based on a hint provided at
// construction.  Generally speaking, a particular usage falls into two
// categories: whether or not socket events are frequently registered.  On some
// platforms, a significant performance improvement can be achieved if the
// registrations are infrequent.  For this situation, the currently installed
// hint should be provided to this event manager for optimal performance.
//
// When callbacks are being dispatched (through the 'dispatch' method) priority
// is given to callbacks associated with socket events.  The timer- related
// callbacks are invoked only after all socket callbacks are invoked.  If two
// timer callbacks are registered with the same time, they're invoked in the
// order of registration (i.e., this timer event manager is stable with respect
// to timers).
//
///Thread Safety
///-------------
// This event manager is *thread* *safe*, meaning that any operation can be
// called on *distinct* *instances* from different threads without any
// side-effects (which, generally speaking, means that there is no 'static'
// data), but not *thread* *enabled* (i.e., two threads cannot safely call
// methods on the *same* *instance* without external synchronization).  This
// event manager is not *async-safe*, meaning that one or more of its functions
// cannot be invoked safely from a signal handler.
//
///Performance
///-----------
// This component will minimize the number of system calls for the set of
// callbacks.  A significant performance improvement can be achieved if an
// appropriate hint is provided.  Providing a false hint (e.g., indicating
// infrequent registration when the reverse is true) may result in significant
// performance degradation.
//
// Given that T is the number of timers registered and S is the number of
// socket events registered, the following complexity is guaranteed by this
// component:
//
//..
//  +=======================================================================+
//  |        FUNCTION          | EXPECTED COMPLEXITY | WORST CASE COMPLEXITY|
//  +-----------------------------------------------------------------------+
//  | dispatch                 |    O(S + log[T])    |    O(S^2 + log[T])   |
//  +-----------------------------------------------------------------------+
//  | registerTimer            |      O(log[T])      |      O(log[T])       |
//  +-----------------------------------------------------------------------+
//  | rescheduleTimer          |      O(log[T])      |      O(log[T])       |
//  +-----------------------------------------------------------------------+
//  | deregisterTimer          |        O(T)         |        O(T)          |
//  +-----------------------------------------------------------------------+
//  | registerSocketEvent      |        O(1)         |        O(S)          |
//  +-----------------------------------------------------------------------+
//  | deregisterSocketEvent    |        O(1)         |        O(S)          |
//  +-----------------------------------------------------------------------+
//  | isRegistered             |        O(1)         |        O(S)          |
//  +-----------------------------------------------------------------------+
//  | deregisterSocket         |        O(1)         |        O(S)          |
//  +-----------------------------------------------------------------------+
//  | deregisterAllSocketEvents|        O(S)         |        O(S)          |
//  +-----------------------------------------------------------------------+
//  | deregisterAllTimers      |        O(T)         |        O(T)          |
//  +-----------------------------------------------------------------------+
//  | deregisterAll            |       O(S+T)        |       O(S+T)         |
//  +-----------------------------------------------------------------------+
//  | numEvents                |        O(1)         |        O(1)          |
//  +-----------------------------------------------------------------------+
//  | numTimers                |        O(1)         |        O(1)          |
//  +-----------------------------------------------------------------------+
//  | numSocketEvents          |        O(1)         |        O(S)          |
//  +=======================================================================+
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing a Command Mediator
/// - - - - - - - - - - - - - - - - - - - - -
// The following usage example shows the implementation of a command mediator.
// Given that this event manager is not thread enabled, a workaround is
// required if we are to use this manager in a multithreaded program.  Let's
// assume that the memory allocator is either thread enabled or the design of
// the program is such that no concurrent allocations occur.  The problem then
// becomes to find or create a mechanism that will allow multiple threads to
// register/deregister callbacks with the same instance of this event manager.
// This mechanism then would delegate the request to the thread that is running
// the 'dispatch' method.  Further analysis of the problem leads to a very
// simple solution.  Clearly, the dispatcher thread needs to be activated.
// However, it might be blocked on a polling call (e.g., 'select') and there
// may be no activity on the set of monitored sockets.  There are a few
// alternatives available on UNIX systems; Windows turns out to be the most
// restrictive platform and therefore the only solution that works across all
// platforms is to create a local pair of connected sockets and register one
// socket with the event manager.  Whenever the invocation is requested, a byte
// is written into the other socket, and the dispatcher thread will invoke the
// (read) callback on the monitored socket from the pair, which, in turn will
// read the byte and invoke user-installed callback.  This is the basic
// structure for the mechanism, which we will call 'my_CommandMediator'.  The
// implementation that follows is self-explanatory.
//..
//  class my_CommandMediator {
//      // This class provides a mechanism to invoke a user-installed command
//      // from the user-installed socket event manager's 'dispatch' method.
//      // Internally, it creates a socket pair, and monitors one of the pair's
//      // socket handles for incoming data.  The 'invoke' method will write a
//      // byte into the other handle.  On the next call to the 'dispatch'
//      // method, read event will be signaled, and an internal callback will
//      // be invoked.  This callback, in turn, will read a byte, and invoke
//      // the user's command.
//
//      btlso::SocketHandle::Handle  d_client;   // socket handle for "control
//                                               // data"
//      btlso::SocketHandle::Handle  d_server;   // socket handle to monitor
//      btlso::TcpTimerEventManager *d_manager_p;// targeted event manager
//      const char                   d_byte;     // control byte
//      bsl::function<void()>        d_command;  // user command.
//
//    private:
//      void readCb();
//          // Read exactly one byte from the 'd_server' socket, and, if read
//          // successfully and correctly, invoke the callback currently
//          // installed.
//
//    public:
//      // CREATORS
//      my_CommandMediator(btlso::TcpTimerEventManager *manager,
//                         bsl::function<void()>        command,
//                         bslma::Allocator            *basicAllocator = 0);
//          // Create a mediator attached to the specified 'manager' and
//          // the specified 'command', which will be invoked from 'manager''s
//          // 'dispatch' method.  Optionally specify a 'basicAllocator' used
//          // to supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.  The behavior is undefined
//          // if 'manager' is 0.
//
//      ~my_CommandMediator();
//          // Destroy this object.
//
//      // MANIPULATORS
//      int operator()();
//          // Initiate an invocation of the associated command on the
//          // next invocation of the 'dispatch' method of the installed timer
//          // event manager.   Return 0 on success and a non-zero value
//          // otherwise.
//  };
//
//  // Private member functions
//  void my_CommandMediator::readCb() {
//      char data;
//      ASSERT(1 == btlso::SocketImpUtil::read(&data, d_server, sizeof data));
//      ASSERT(data == d_byte);
//      d_command();
//  }
//
//  // CREATORS
//  inline
//  my_CommandMediator::my_CommandMediator(
//                                 btlso::TcpTimerEventManager *manager,
//                                 bsl::function<void()>        command,
//                                 bslma::Allocator            *basicAllocator)
//  : d_manager_p(manager)
//  , d_byte(0xAF)
//  , d_command(command)
//  {
//      btlso::SocketHandle::Handle handles[2];
//      ASSERT(0 == btlso::SocketImpUtil::socketPair<btlso::IPv4Address>
//                        (handles, btlso::SocketImpUtil::k_SOCKET_STREAM));
//
//      d_client = handles[0];
//      d_server = handles[1];
//
//      bsl::function<void()> functor(
//              bdlf::MemFnUtil::memFn(&my_CommandMediator::readCb, this),
//              basicAllocator);
//      d_manager_p->registerSocketEvent(d_server, btlso::EventType::e_READ,
//                                       functor);
//  }
//
//  inline
//  my_CommandMediator::~my_CommandMediator() {
//      d_manager_p->deregisterSocketEvent(d_server,
//                                         btlso::EventType::e_READ);
//      ASSERT(0 == d_manager_p->numSocketEvents(d_server));
//      btlso::SocketImpUtil::close(d_client);
//      btlso::SocketImpUtil::close(d_server);
//  }
//
//  // MANIPULATORS
//  inline
//  int my_CommandMediator::operator()() {
//      return
//          btlso::SocketImpUtil::write(d_client, &d_byte, sizeof(char))
//          != sizeof(char);
//
//  }
//..
//
// Please note that the mediator implementation presented in this usage example
// cannot be considered complete.  Particularly, there is no data passing
// between the mediator thread and dispatcher thread.  In a real-world
// application, being able to pass data is required, and, therefore, additional
// thread-enabled mechanisms (e.g., a thread-enabled queue) are needed.

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_TIMEREVENTMANAGER
#include <btlso_timereventmanager.h>
#endif

#ifndef INCLUDED_BTLSO_EVENTMANAGER
#include <btlso_eventmanager.h>
#endif

#ifndef INCLUDED_BTLSO_EVENTTYPE
#include <btlso_eventtype.h>
#endif

#ifndef INCLUDED_BTLSO_TIMEMETRICS
#include <btlso_timemetrics.h>
#endif

#ifndef INCLUDED_BDLCC_TIMEQUEUE
#include <bdlcc_timequeue.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace btlso {

                   // ==========================
                   // class TcpTimerEventManager
                   // ==========================

class TcpTimerEventManager : public TimerEventManager
{
    // This class provides a concrete implementation of a timer event manager
    // for sockets.  The supported event types for sockets are ACCEPT, CONNECT,
    // READ, and WRITE, and only READ and WRITE can be registered
    // simultaneously on a single socket.  Timers are stable (i.e., the
    // relative order of registrations is maintained).  A (registered) callback
    // can only be invoked from the 'dispatch' method.  The performance of
    // 'dispatch' can be optimized based on user hints.

  public:
    enum Hint {
        e_NO_HINT,                 // the registrations may be frequent
        e_INFREQUENT_REGISTRATION  // the (de)registrations will be infrequent


    };

  private:
    bdlcc::TimeQueue<bsl::function<void()> >
                        d_timers;          // registered timers

    EventManager       *d_manager_p;       // socket event manager

    int                 d_isManagedFlag;   // indicates whether or not the
                                           // event manager is managed.

    bslma::Allocator   *d_allocator_p;     // allocator used to supply memory

    TimeMetrics         d_metrics;         // workload counter

  private:
    TcpTimerEventManager(const TcpTimerEventManager&);
    TcpTimerEventManager& operator=(const TcpTimerEventManager&);

  public:
    // CREATORS
    TcpTimerEventManager(bslma::Allocator *basicAllocator = 0);
        // Create an event manager with timer support optimized for frequent
        // registrations ('e_NO_HINT').  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    TcpTimerEventManager(Hint              hint,
                         bslma::Allocator *basicAllocator = 0);
        // Create an event manager with timer support optimized for the
        // registration frequency as hinted by 'hint'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    TcpTimerEventManager(EventManager     *manager,
                         bslma::Allocator *basicAllocator = 0);
        // Create a timer event manager that uses the specified 'manager' for
        // monitoring socket events.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  The behavior is undefined
        // unless 'manager' is not 0.  Note that 'manager' must remain valid
        // (i.e., not destroyed) for the lifetime of this (newly-created) timer
        // event manager and when this object is destroyed the object addressed
        // by 'manager' is not destroyed.

    ~TcpTimerEventManager();
        // Destroy this event manager without invoking registered callbacks.

    // MANIPULATORS
    int dispatch(int flags);
        // For each socket event pending on this event manager, and for each
        // expired timer , invoke the corresponding callback registered with
        // this timer event manager.  If no event is pending, wait until either
        // (1) at least one event occurs (in which case the corresponding
        // callback(s) is invoked), (2) a timer expires or (3) provided that
        // the specified 'flags' contains 'bteso_Flag::k_ASYNC_INTERRUPT', an
        // underlying system call is interrupted by a signal.  If no socket and
        // no timer is registered with this event manager, this call will
        // return (with 0) immediately.  Return the number of dispatched
        // callbacks on success, and a negative value otherwise; -1 is reserved
        // to indicate that an underlying system call was interrupted.  When
        // such an interruption occurs this method will return -1 if 'flags'
        // contains 'bteso_Flag::k_ASYNC_INTERRUPT' and otherwise will
        // automatically restart (i.e., reissue the identical system call).
        // Note that the order of invocation, relative to the order of
        // registration, is unspecified and that -1 is never returned if
        // 'flags' does not contain 'bteso_Flag::k_ASYNC_INTERRUPT'.  Calling
        // this method from a callback invoked through this method will result
        // in undefined behavior.

    int registerSocketEvent(const SocketHandle::Handle& handle,
                            EventType::Type             event,
                            const Callback&             callBack);
        // Register with this event manager the specified 'cb' functor to be
        // invoked whenever the specified 'event' occurs on the socket
        // specified by 'handle'.  Return 0 on success and a negative number on
        // error.  No two different socket events can have callbacks registered
        // with the same socket handle other than read and write.  Any
        // invocation of this method that would cause this to occur will result
        // in an error and the callback will not be registered.  The callback
        // is recurring (i.e., it remains registered until it is explicitly
        // deregistered).

    void *registerTimer(const bsls::TimeInterval& timeout,
                        const Callback&           callBack);
        // Register with this event manager the specified 'cb' functor to be
        // invoked when the absolute time of the specified 'timeout' is reached
        // or exceeded.  Return a timer handle which can be used to deregister
        // this timer before expiration.  Note that specifying a 'timeout'
        // prior to the current time will result in the associated 'cb' being
        // executed on the next invocation of 'dispatch'.  Note also that the
        // callback is not recurring (i.e., after being invoked it is
        // deregistered automatically).

    int rescheduleTimer(const void                *timerId,
                        const bsls::TimeInterval&  expiryTime);
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

    void deregisterSocketEvent(const SocketHandle::Handle& handle,
                               EventType::Type             event);
        // Deregister from this event manager the callback associated with the
        // specified 'event' on the specified 'handle' so that said callback
        // will not be invoked if the 'event' occurs.  The behavior is
        // undefined unless there is a callback registered for 'event' on the
        // socket 'handle'.

    void deregisterSocket(const SocketHandle::Handle& handle);
        // Deregister from this event manager all callbacks associated with any
        // event on the specified 'handle' such that no callback will be
        // invoked if an event occurs on the 'handle'.

    void deregisterAllSocketEvents();
        // Deregister from this event manager all callbacks associated with any
        // event on any socket handle so that no callbacks are invoked when any
        // event occurs on any handle.

    void deregisterTimer(const void *handle);
        // Deregister from this event manager the timer callback associated
        // with the specified timer 'handle' (returned when the timer callback
        // was registered) so that no callback will be invoked at the appointed
        // time.  The behavior is undefined unless 'handle' was not obtained
        // from this event manager through 'registerTimer' method.

    void deregisterAllTimers();
        // Deregister from this event manager all timer callbacks currently
        // registered.  The number of timers (as reported by 'numTimers') is 0
        // after this method is completed.

    void deregisterAll();
        // Deregister from this event manager all callbacks currently
        // registered.  Note that the behavior is equivalent to the sequence of
        //..
        //  deregisterAllTimers();
        //  deregisterAllSocketEvents();
        //..

    TimeMetrics *timeMetrics();
        // Return the address of modifiable metrics that reflect the workload
        // of this event manager as a percentage of time spent in CPU bound
        // operations vs. total time (see 'btlso_timemetrics').

    // ACCESSORS
    bool hasLimitedSocketCapacity() const;
        // Return 'true' if this event manager has a limited socket capacity,
        // and 'false' otherwise.

    int isRegistered(const SocketHandle::Handle& handle,
                     EventType::Type             eventType) const;
        // Return 1 if a callback is registered with this event manager to be
        // invoked when a socket event of the specified 'eventType' occurs on
        // the specified socket 'handle', and 0 otherwise.

    int numEvents() const;
        // Return the total number of callbacks registered with this event
        // manager for either timers or sockets.

    int numTimers() const;
        // Return the number of timers that are currently registered with this
        // event manager.

    int numSocketEvents(const SocketHandle::Handle& handle) const;
        // Return the number of callbacks registered with this event manager,
        // associated with the specified socket 'handle'.

    const EventManager *socketEventManager() const;
        // Return the non-modifiable event manager used for monitoring for
        // socket events.
};

//-----------------------------------------------------------------------------
//                      INLINE FUNCTION DEFINITIONS
//-----------------------------------------------------------------------------

                   // --------------------------
                   // class TcpTimerEventManager
                   // --------------------------

// ACCESSORS
inline
bool TcpTimerEventManager::hasLimitedSocketCapacity() const
{
    return d_manager_p->hasLimitedSocketCapacity();
}

inline
TimeMetrics *TcpTimerEventManager::timeMetrics()
{
    return &d_metrics;
}

inline const EventManager *
TcpTimerEventManager::socketEventManager() const
{
    return d_manager_p;
}
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
