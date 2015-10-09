// btlso_defaulteventmanager.h                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_DEFAULTEVENTMANAGER
#define INCLUDED_BTLSO_DEFAULTEVENTMANAGER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an interface for default event manager.
//
//@CLASSES:
//  btlso::DefaultEventManager: event manager interface
//
//@SEE_ALSO: btlso_eventmanager, btlso_timereventmanager
//
//@DESCRIPTION: This component provides a default socket event multiplexer that
// adheres to 'btlso::EventManager' protocol.  In a case where more than one
// multiplexing mechanism is available for a certain platform, all mechanisms
// (to the best of our knowledge) are implemented and a particular one can be
// chosen explicitly by the user (see usage example).  The following socket
// event manager specializations (presented along with the polling mechanisms
// and applicable platforms) are available through this component (note that,
// for shortness, we will omit 'btlso::DefaultEventManager' in the
// specialization column):
//..
//  +========================================================================+
//  |      SPECIALIZATION        |   SYSTEM MECHANISM    |     PLATFORM      |
//  +------------------------------------------------------------------------+
//  | <btlso::Platform::SELECT>  |        select         | Windows, Solaris, |
//  |                            |                       | AIX, Linux        |
//  +------------------------------------------------------------------------+
//  | <btlso::Platform::DEVPOLL> |        /dev/poll      |      Solaris*     |
//  +------------------------------------------------------------------------+
//  | <btlso::Platform::EPOLL>   |         epoll         |       Linux*      |
//  +------------------------------------------------------------------------+
//  | <btlso::Platform::POLL>    |          poll         | Solaris, AIX*,    |
//  |                            |                       | Linux             |
//  +========================================================================+
//  * indicates the default specialization for a platform.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Default Event Manager
///- - - - - - - - - - - - - - - - - - - - - -
// In the following usage example we show how to create an instance of a
// default event manager.  First, we need to include this file (shown here for
// completeness):
//..
//  #include <btlso_defaulteventmanager.h>
//..
// Second, create a 'btlso::TimeMetrics' to give to the event manager:
//..
//  btlso::TimeMetrics metrics(btlso::TimeMetrics::e_MIN_NUM_CATEGORIES,
//                             btlso::TimeMetrics::e_CPU_BOUND);
//..
// Now, create a default event manager that uses this 'metrics':
//..
//  typedef btlso::Platform::DEFAULT_POLLING_MECHANISM PollMechanism;
//  btlso::DefaultEventManager<PollMechanism> eventManager(&metrics);
//..
// Note that the time metrics is optional.  Using the same component, we can
// create an event manager that uses a particular mechanism (for example,
// '/dev/poll') as follows:
//..
//  btlso::DefaultEventManager<btlso::Platform::DEVPOLL> fastEventManager;
//..
// Note that '/dev/poll' is available only on Solaris and this instantiation
// fails (at compile time) on other platforms.

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_DEFAULTEVENTMANAGER_DEVPOLL
#include <btlso_defaulteventmanager_devpoll.h>
#endif

#ifndef INCLUDED_BTLSO_DEFAULTEVENTMANAGER_EPOLL
#include <btlso_defaulteventmanager_epoll.h>
#endif

#ifndef INCLUDED_BTLSO_DEFAULTEVENTMANAGER_POLL
#include <btlso_defaulteventmanager_poll.h>
#endif

#ifndef INCLUDED_BTLSO_DEFAULTEVENTMANAGER_POLLSET
#include <btlso_defaulteventmanager_pollset.h>
#endif

#ifndef INCLUDED_BTLSO_DEFAULTEVENTMANAGER_SELECT
#include <btlso_defaulteventmanager_select.h>
#endif

#ifndef INCLUDED_BTLSO_EVENTMANAGER
#include <btlso_eventmanager.h>
#endif

#ifndef INCLUDED_BTLSO_EVENTTYPE
#include <btlso_eventtype.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#include <btlso_sockethandle.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace bsls { class TimeInterval; }

namespace btlso {

class TimeMetrics;

                     // =========================
                     // class DefaultEventManager
                     // =========================

template <class POLLING_MECHANISM>
class DefaultEventManager : public EventManager {
    // This class standardizes the interface for the default event manager for
    // a given platform.  The generic class (this one) is NOT implemented.  All
    // specialized event managers must adhere to this interface.
    //
    // Note that 'POLLING_MECHANISM' is specified in the forward declaration in
    // 'btlso_defaulteventmanagerimpl.h' to default to
    // 'Platform::DEFAULT_POLLING_MECHANISM'.

  public:
    // CREATORS
    explicit
    DefaultEventManager(TimeMetrics      *timeMetric     = 0,
                        bslma::Allocator *basicAllocator = 0);
        // Create a 'poll'-based event manager.  Optionally specify a
        // 'timeMetric' to report time spent in CPU-bound and IO-bound
        // operations.  If 'timeMetric' is not specified or is 0, these metrics
        // are not reported.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~DefaultEventManager();
        // Destroy this object.  Note that the registered callbacks are NOT
        // invoked.

    // MANIPULATORS
    int dispatch(int flags);
        // For each socket event pending on this event manager, invoke the
        // corresponding callback registered with this event manager.  If no
        // event is pending, wait until either (1) at least one event occurs
        // (in which case the corresponding callback(s) is invoked) or (2)
        // provided that the specified 'flags' contains
        // 'bteso_Flag::k_ASYNC_INTERRUPT', an underlying system call is
        // interrupted by a signal.  Return the number of dispatched callbacks
        // on success, and a negative value otherwise; -1 is reserved to
        // indicate that an underlying system call was interrupted.  When such
        // an interruption occurs this method will return -1 if 'flags'
        // contains 'bteso_Flag::k_ASYNC_INTERRUPT' and otherwise will
        // automatically restart (i.e., reissue the identical system call).
        // Note that the order of invocation, relative to the order of
        // registration, is unspecified and that -1 is never returned if
        // 'flags' does not contain 'bteso_Flag::k_ASYNC_INTERRUPT'.  Also note
        // that the behavior of this method may be undefined if the number of
        // registered sockets is 0.

    int dispatch(const bsls::TimeInterval& timeout, int flags);
        // For each socket event pending on this event manager, invoke the
        // corresponding callback registered with this event manager.  If no
        // event is pending, wait until either (1) at least one event occurs
        // (in which case the corresponding callback(s) are invoked), (2) the
        // specified absolute 'timeout' interval is reached, or (3) provided
        // that the specified 'flags' contains 'bteso_Flag::k_ASYNC_INTERRUPT',
        // an underlying system call is interrupted by a signal.  Return the
        // number of dispatched callbacks on success, 0 if 'timeout' is
        // reached, and a negative value otherwise; -1 is reserved to indicate
        // that an underlying system call was interrupted.  When such an
        // interruption occurs this method will return -1 if 'flags' contains
        // 'bteso_Flag::k_ASYNC_INTERRUPT' and otherwise will automatically
        // restart (i.e., reissue the identical system call).  Note that the
        // order of invocation, relative to the order of registration, is
        // unspecified and that -1 is never returned if 'flags' does not
        // contain 'bteso_Flag::k_ASYNC_INTERRUPT'.  Also note that the
        // behavior of this method may be undefined if the number of registered
        // sockets is 0.

    int registerSocketEvent(const SocketHandle::Handle&   handle,
                            const EventType::Type         event,
                            const EventManager::Callback& callback);
        // Register with this event manager the occurrence of the specified
        // 'event' on the specified socket 'handle' such that the specified
        // 'callback' functor is invoked when 'event' occurs.  Return 0 on
        // success and a non-zero value otherwise.  Socket event registrations
        // stay in effect until they are subsequently deregistered; the
        // callback is invoked each time 'event' is seen.  Typically,
        // 'EventType::e_READ' and 'EventType::e_WRITE' are the only events
        // that can be registered simultaneously for a socket.  Simultaneously
        // registering for incompatible events for the same socket 'handle' may
        // result in undefined behavior.  If a registration attempt is made for
        // an event that is already registered, the callback associated with
        // this event will be overwritten with the new one.

    void deregisterSocketEvent(const SocketHandle::Handle& handle,
                               EventType::Type             event);
        // Deregister from this event manager the callback associated with the
        // specified 'event' on the specified 'handle' so that said callback
        // will not be invoked should 'event' occur.

    int deregisterSocket(const SocketHandle::Handle& handle);
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

    int isRegistered(const SocketHandle::Handle& handle,
                     const EventType::Type       event) const;
        // Return 1 if the specified 'event' is registered with this event
        // manager for the specified socket 'handle' and 0 otherwise.

    int numEvents() const;
        // Return the total number of all socket events currently registered
        // with this event manager.

    int numSocketEvents(const SocketHandle::Handle& handle) const;
        // Return the number of socket events currently registered with this
        // event manager for the specified 'handle'.
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
