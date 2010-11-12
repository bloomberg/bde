// bteso_eventmanager.h  -*-C++-*-
#ifndef INCLUDED_BTESO_EVENTMANAGER
#define INCLUDED_BTESO_EVENTMANAGER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Define a protocol for socket-event multiplexer.
//
//@CLASSES: bteso_EventManager: socket-event multiplexer protocol
//
//@SEE_ALSO: bteso_defaulteventmanager bteso_tcptimereventmanager
//
//@SEE_ALSO: bteso_eventmanagertest
//
//@AUTHOR: Cheenu Srinivasan (csriniva)
//
//@DESCRIPTION: This component defines a protocol for a socket-event
// multiplexer.  Generally speaking, a socket-event multiplexer allows
// registering a callback to be invoked when a particular event occurs on a
// particular socket.  (The user is required to provide a socket handle, an
// event, and a callback for registration.)  For 'bteso_EventManager', a
// callback is represented by a 'bdef_Function' functor (which may be
// associated with a free function or a class method).  Registrations stay in
// effect until they are subsequently deregistered, and the callback is invoked
// each time the associated event occurs.
//
// The user of 'bteso_EventManager' controls when registered callbacks can be
// invoked; the callbacks are invoked from within one of the 'dispatch' methods
// (see below) and cannot be invoked otherwise.  (Particularly, this means that
// an implementation with one internal thread that invokes callbacks
// constitutes a violation of this contract.)
//
///Usage
///-----
// Let's suppose that we would like to monitor a set of sockets, obtained
// through some other means, for incoming data and invoke a given function for
// each when data is detected.  (For simplicity, we assume that the function to
// be invoked takes only a socket handle.)  This can be accomplished by using
// 'bteso_EventManager' in the following way:
//..
//  int monitorSet(bteso_SocketHandle::Handle      *sockets,
//                 int                              numSockets,
//                 void (*function)(bteso_SocketHandle::Handle),
//                 bteso_EventManager              *manager)
//      // Invoke the specified 'function' when incoming data is detected on
//      // a subset of the specified 'sockets' of the specified 'numSockets'
//      // length.  Use the specified 'manager' for monitoring.
//      // Return a positive number of callbacks invoked on success and a
//      // non-positive value otherwise.  The behavior is undefined unless
//      // 0 < numSockets.
//  {
//      assert(sockets);
//      assert(manager);
//      assert(0 < numSockets);
//
//      // Create a callback associated with 'function' for each socket and
//      // register this callback to be invoked when associated socket is
//      // ready for reading.
//      for (int i = 0; i < numSockets; ++i) {
//          bdef_Function<void (*)()> callback(
//              bdef_BindUtil::bind(function, sockets[i]));
//
//          if (manager->registerSocketEvent(sockets[i],
//                                           bteso_EventType::BTESO_READ,
//                                           callback))
//          {
//              // For cleanliness, when a registration fails, we will cancel
//              // all previous registrations.
//              while(--i >= 0) {
//                  manager->deregisterSocket(sockets[i]);
//              }
//              return -1;
//          }
//      }
//      return manager->dispatch();
//  }
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

class bteso_EventManager {
    // This class defines a protocol for a socket-event multiplexer.  The
    // callback are registered permanently (i.e., invoked repeatedly as events
    // occur until explicitly deregistered).  The invocation is done in the
    // user thread at user's discretion.

  public:
    // TYPES
    typedef bdef_Function<void (*)()> Callback;
        // Alias for the type used as the registered callback for this event
        // manager.

    // CREATORS
    virtual ~bteso_EventManager();
        // Destroy this event manager.

    // MANIPULATORS
    virtual int dispatch(int flags = 0) = 0;
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

    virtual int dispatch(const bdet_TimeInterval& timeout, int flags = 0) = 0;
        // For each socket event pending on this event manager, invoke the
        // corresponding callback registered with this event manager.  If no
        // event is pending, wait until either (1) at least one event occurs
        // (in which case the corresponding callback(s) is invoked), (2) the
        // specified relative 'timeout' interval is exceeded, or (3) provided
        // that the specified 'flags' contains
        // 'bteso_Flag::BTESO_ASYNC_INTERRUPT', an underlying system call is
        // interrupted by a signal.  Return the number of dispatched callbacks
        // on success, 0 on timeout, and a negative value otherwise; -1 is
        // reserved to indicate that an underlying system call was interrupted.
        // When such an interruption occurs this method will return -1 if
        // 'flags' contains 'bteso_Flag::BTESO_ASYNC_INTERRUPT' and otherwise
        // will automatically restart (i.e., reissue the identical system
        // call).  Note that the order of invocation, relative to the order of
        // registration, is unspecified and that -1 is never returned if
        // 'flags' does not contain 'bteso_Flag::BTESO_ASYNC_INTERRUPT'.  Also
        // note that the behavior of this method may be undefined if the number
        // of registered sockets is 0.

    virtual int registerSocketEvent(
                             const bteso_SocketHandle::Handle&   handle,
                             const bteso_EventType::Type         event,
                             const bteso_EventManager::Callback& callback) = 0;
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

    virtual void deregisterSocketEvent(
                                  const bteso_SocketHandle::Handle& handle,
                                  bteso_EventType::Type             event) = 0;
        // Deregister from this event manager the callback associated with the
        // specified 'event' on the specified 'handle' so that said callback
        // will not be invoked should 'event' occur.  The behavior is undefined
        // unless there is a callback registered for 'event' on the socket
        // 'handle'.

    virtual int deregisterSocket(
                 const bteso_SocketHandle::Handle& handle) = 0;
        // Deregister from this event manager all events associated with the
        // specified socket 'handle'.  Return the number of deregistered
        // callbacks.

    virtual void deregisterAll() = 0;
        // Deregister from this event manager all events on every socket
        // handle.

    // ACCESSORS
    virtual int isRegistered(const bteso_SocketHandle::Handle& handle,
                             const bteso_EventType::Type       event)
                                                                     const = 0;
        // Return 1 if the specified 'event' is registered with this event
        // manager for the specified socket 'handle' and 0 otherwise.

    virtual int numEvents() const = 0;
        // Return the total number of all socket events currently registered
        // with this event manager.

    virtual int numSocketEvents(const bteso_SocketHandle::Handle& handle)
                                                                     const = 0;
        // Return the number of socket events currently registered with this
        // event manager for the specified 'handle'.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003, 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
