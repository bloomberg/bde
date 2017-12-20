// btlso_eventmanager.h                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_EVENTMANAGER
#define INCLUDED_BTLSO_EVENTMANAGER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Define a protocol for socket-event multiplexer.
//
//@CLASSES:
//  btlso::EventManager: socket-event multiplexer protocol
//
//@SEE_ALSO: btlso_defaulteventmanager, btlso_tcptimereventmanager,
//           bteso_eventmanagertest
//
//@DESCRIPTION: This component defines a protocol for a socket-event
// multiplexer.  Generally speaking, a socket-event multiplexer allows
// registering a callback to be invoked when a particular event occurs on a
// particular socket.  (The user is required to provide a socket handle, an
// event, and a callback for registration.)  For 'btlso::EventManager', a
// callback is represented by a 'bsl::function' functor (which may be
// associated with a free function or a class method).  Registrations stay in
// effect until they are subsequently deregistered, and the callback is invoked
// each time the associated event occurs.
//
// The user of 'btlso::EventManager' controls when registered callbacks can be
// invoked; the callbacks are invoked from within one of the 'dispatch' methods
// (see below) and cannot be invoked otherwise.  (Particularly, this means that
// an implementation with one internal thread that invokes callbacks
// constitutes a violation of this contract.)
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Monitoring a Set of Sockets
/// - - - - - - - - - - - - - - - - - - -
// Let's suppose that we would like to monitor a set of sockets, obtained
// through some other means, for incoming data and invoke a given function for
// each when data is detected.  (For simplicity, we assume that the function to
// be invoked takes only a socket handle.)  This can be accomplished by using
// 'btlso::EventManager' in the following way:
//..
//  int monitorSet(btlso::SocketHandle::Handle      *sockets,
//                 int                               numSockets,
//                 void (*function)(btlso::SocketHandle::Handle),
//                 btlso::EventManager              *manager)
//      // Invoke the specified 'function' when incoming data is detected on a
//      // subset of the specified 'sockets' of the specified 'numSockets'
//      // length.  Use the specified 'manager' for monitoring.  Return a
//      // positive number of callbacks invoked on success and a non-positive
//      // value otherwise.  The behavior is undefined unless '0 < numSockets'.
//  {
//      assert(sockets);
//      assert(manager);
//      assert(0 < numSockets);
//
//      // Create a callback associated with 'function' for each socket and
//      // register this callback to be invoked when associated socket is
//      // ready for reading.
//
//      for (int i = 0; i < numSockets; ++i) {
//          bsl::function<void()> callback(bdlf::BindUtil::bind(
//                                                                function,
//                                                                sockets[i]));
//
//          const int rc = manager->registerSocketEvent(
//                                                    sockets[i],
//                                                    btlso::EventType::e_READ,
//                                                    callback);
//
//          if (rc) {
//
//              // For cleanliness, when a registration fails, we will cancel
//              // all previous registrations.
//
//              while(--i >= 0) {
//                  manager->deregisterSocket(sockets[i]);
//              }
//
//              return -1;
//          }
//      }
//      return manager->dispatch();
//  }
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

                   // ==================
                   // class EventManager
                   // ==================

class EventManager {
    // This class defines a protocol for a socket-event multiplexer.  The
    // callback are registered permanently (i.e., invoked repeatedly as events
    // occur until explicitly deregistered).  The invocation is done in the
    // user thread at user's discretion.

  public:
    // TYPES
    typedef bsl::function<void()> Callback;
        // Alias for the type used as the registered callback for this event
        // manager.

    // CREATORS
    virtual ~EventManager();
        // Destroy this event manager.

    // MANIPULATORS
    virtual int dispatch(int flags = 0) = 0;
        // For each socket event pending on this event manager, invoke the
        // corresponding callback registered with this event manager.  If no
        // event is pending, wait until either (1) at least one event occurs
        // (in which case the corresponding callback(s) is invoked) or (2)
        // provided that the optionally-specified 'flags' contains
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

    virtual int dispatch(const bsls::TimeInterval& timeout, int flags = 0) = 0;
        // For each socket event pending on this event manager, invoke the
        // corresponding callback registered with this event manager.  If no
        // event is pending, wait until either (1) at least one event occurs
        // (in which case the corresponding callback(s) is invoked), (2) the
        // specified relative 'timeout' interval is exceeded, or (3) provided
        // that the specified 'flags' contains 'bteso_Flag::k_ASYNC_INTERRUPT',
        // an underlying system call is interrupted by a signal.  Return the
        // number of dispatched callbacks on success, 0 on timeout, and a
        // negative value otherwise; -1 is reserved to indicate that an
        // underlying system call was interrupted.  When such an interruption
        // occurs this method will return -1 if 'flags' contains
        // 'bteso_Flag::k_ASYNC_INTERRUPT' and otherwise will automatically
        // restart (i.e., reissue the identical system call).  Note that the
        // order of invocation, relative to the order of registration, is
        // unspecified and that -1 is never returned if 'flags' does not
        // contain 'bteso_Flag::k_ASYNC_INTERRUPT'.  Also note that the
        // behavior of this method may be undefined if the number of registered
        // sockets is 0.

    virtual int registerSocketEvent(
                             const SocketHandle::Handle&   handle,
                             const EventType::Type         event,
                             const EventManager::Callback& callback) = 0;
        // Register with this event manager the occurrence of the specified
        // 'event' on the specified socket 'handle' such that the specified
        // 'callback' functor is invoked when 'event' occurs.  Return 0 on
        // success and a non-zero value otherwise.  Socket event registrations
        // stay in effect until they are subsequently deregistered; the
        // callback is invoked each time the specified 'event' is seen.
        // Typically, 'EventType::e_READ' and 'EventType::e_WRITE' are the only
        // events that can be registered simultaneously for a socket.
        // Simultaneously registering for incompatible events for the same
        // socket 'handle' may result in undefined behavior.  If a registration
        // attempt is made for an event that is already registered, the
        // callback associated with this event will be overwritten with the new
        // one.

    virtual void deregisterSocketEvent(const SocketHandle::Handle& handle,
                                       EventType::Type             event) = 0;
        // Deregister from this event manager the callback associated with the
        // specified 'event' on the specified 'handle' so that said callback
        // will not be invoked should 'event' occur.  The behavior is undefined
        // unless there is a callback registered for 'event' on the socket
        // 'handle'.

    virtual int deregisterSocket(const SocketHandle::Handle& handle) = 0;
        // Deregister from this event manager all events associated with the
        // specified socket 'handle'.  Return the number of deregistered
        // callbacks.

    virtual void deregisterAll() = 0;
        // Deregister from this event manager all events on every socket
        // handle.

    // ACCESSORS
    virtual bool hasLimitedSocketCapacity() const = 0;
        // Return 'true' if this event manager has a limited socket capacity,
        // and 'false' otherwise.

    virtual int isRegistered(const SocketHandle::Handle& handle,
                             const EventType::Type       event) const = 0;
        // Return 1 if the specified 'event' is registered with this event
        // manager for the specified socket 'handle' and 0 otherwise.

    virtual int numEvents() const = 0;
        // Return the total number of all socket events currently registered
        // with this event manager.

    virtual int numSocketEvents(const SocketHandle::Handle& handle) const = 0;
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
