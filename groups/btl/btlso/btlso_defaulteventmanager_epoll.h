// btlso_defaulteventmanager_epoll.h                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_DEFAULTEVENTMANAGER_EPOLL
#define INCLUDED_BTLSO_DEFAULTEVENTMANAGER_EPOLL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide socket multiplexer implementation using Linux 'epoll'.
//
//@CLASSES:
//  btlso::DefaultEventManager<btlso::Platform::EPOLL>: 'epoll' multiplexer
//
//@SEE_ALSO: btlso_eventmanager btlso_defaulteventmanager btlso_timemetrics
//
//@DESCRIPTION: This component provides an implementation of an event manager,
// 'btlso::DefaultEventManager<btlso::Platform::EPOLL>', that uses the Linux
// 'epoll' system calls to monitor for socket events and adheres to the
// 'btlso::EventManager' protocol.  In particular, this protocol supports the
// registration of level-triggered socket events, along with an associated
// 'bsl::function' callback functor, which is invoked when the corresponding
// socket event occurs.
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
// 'btlso_defaulteventmanager' component; the other components are shown
// (schematically) on the following diagram:
//..
//                         _____bteso_defaulteventmanager_____
//                 _______/    |           |         |        \_________
//                 *_poll   *_epoll     *_select  *_devpoll    *_pollset
//..
//
///Thread Safety
///-------------
// This component depends on a 'bslma::Allocator' instance to supply memory.
// If the allocator is not thread enabled then the instances of this component
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
// provides the following complexity guarantees:
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
///Metrics
///-------
// The event manager provided by this component can use external (i.e.,
// user-installed) time metrics (see 'btlso_timemetrics' component) to record
// times spend in IO-bound and CPU-bound operations using the category IDs
// defined in 'btlso::TimeMetrics'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using an Event Manager
///- - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use this event manager with
// a non-blocking socket.  First, create a 'btlso::TimeMetrics' object and a
// 'btlso::DefaultEventManager<btlso::Platform::EPOLL>' object; also create a
// (locally-connected) socket pair:
//..
//  btlso::TimeMetrics timeMetric(btlso::TimeMetrics::e_MIN_NUM_CATEGORIES,
//                                btlso::TimeMetrics::e_CPU_BOUND);
//
//  btlso::DefaultEventManager<btlso::Platform::EPOLL> mX(&timeMetric);
//
//  btlso::SocketHandle::Handle socket[2];
//
//  int rc = btlso::SocketImpUtil::socketPair<btlso::IPv4Address>(
//                                      socket,
//                                      btlso::SocketImpUtil::k_SOCKET_STREAM);
//
//  assert(0 == rc);
//..
// Second, register three socket events, two for 'socket[0]', and one for
// 'socket[1]', and verify the state by using accessor functions.  The callback
// installed as part of each registration operation will either read or write
// the pre-determined (specified on a case-by-case basis) number of bytes:
//..
//  int numBytes = 5;
//  btlso::EventManager::Callback readCb(
//                               bdlf::BindUtil::bind(&genericCb,
//                                                    btlso::EventType::e_READ,
//                                                    socket[0],
//                                                    numBytes,
//                                                    &mX));
//  mX.registerSocketEvent(socket[0], btlso::EventType::e_READ, readCb);
//
//  numBytes = 25;
//  btlso::EventManager::Callback writeCb1(
//                              bdlf::BindUtil::bind(&genericCb,
//                                                   btlso::EventType::e_WRITE,
//                                                   socket[0],
//                                                   numBytes,
//                                                   &mX));
//  mX.registerSocketEvent(socket[0], btlso::EventType::e_WRITE, writeCb1);
//
//  numBytes = 15;
//  btlso::EventManager::Callback writeCb2(
//                              bdlf::BindUtil::bind(&genericCb,
//                                                   btlso::EventType::e_WRITE,
//                                                   socket[1],
//                                                   numBytes,
//                                                   &mX));
//  mX.registerSocketEvent(socket[1], btlso::EventType::e_WRITE, writeCb2);
//
//  assert(3 == mX.numEvents());
//  assert(2 == mX.numSocketEvents(socket[0]));
//  assert(1 == mX.numSocketEvents(socket[1]));
//  assert(1 == mX.isRegistered(socket[0], btlso::EventType::e_READ));
//  assert(0 == mX.isRegistered(socket[1], btlso::EventType::e_READ));
//  assert(1 == mX.isRegistered(socket[0], btlso::EventType::e_WRITE));
//  assert(1 == mX.isRegistered(socket[1], btlso::EventType::e_WRITE));
//..
// Next, we try to execute the requests by calling the 'dispatch' function with
// a timeout (5 seconds from now) requirement and verify the result.  The two
// write requests should be executed since both ends are writable If we don't
// have a timeout requirement, a different version of 'dispatch' (in which no
// timeout is specified) can also be called.
//..
//  int                flags = 0;
//  bsls::TimeInterval deadline(bdlt::CurrentTime::now());
//
//  deadline += 5;    // timeout 5 seconds from now.
//  rc = mX.dispatch(deadline, flags);   assert(2 == rc);
//..
// Now we try to remove the write request of 'socket[0]' from the event manager
// by calling 'deregisterSocketEvent()' and verify the state:
//..
//  mX.deregisterSocketEvent(socket[0], btlso::EventType::e_WRITE);
//  assert(2 == mX.numEvents());
//  assert(1 == mX.numSocketEvents(socket[0]));
//  assert(1 == mX.numSocketEvents(socket[1]));
//  assert(1 == mX.isRegistered(socket[0], btlso::EventType::e_READ));
//  assert(0 == mX.isRegistered(socket[1], btlso::EventType::e_READ));
//  assert(0 == mX.isRegistered(socket[0], btlso::EventType::e_WRITE));
//  assert(1 == mX.isRegistered(socket[1], btlso::EventType::e_WRITE));
//..
// We then try to remove all registrations associated with 'socket[1]' by
// calling 'deregisterSocket()' and verify the state:
//..
//  assert(1 == mX.deregisterSocket(socket[1]));
//  assert(1 == mX.numEvents());
//  assert(1 == mX.numSocketEvents(socket[0]));
//  assert(0 == mX.numSocketEvents(socket[1]));
//  assert(1 == mX.isRegistered(socket[0], btlso::EventType::e_READ));
//  assert(0 == mX.isRegistered(socket[1], btlso::EventType::e_READ));
//  assert(0 == mX.isRegistered(socket[0], btlso::EventType::e_WRITE));
//  assert(0 == mX.isRegistered(socket[1], btlso::EventType::e_WRITE));
//..
// Finally, we remove all registrations by calling 'deregisterAll()' and verify
// the state again:
//..
//  mX.deregisterAll();
//  assert(0 == mX.numEvents());
//  assert(0 == mX.numSocketEvents(socket[0]));
//  assert(0 == mX.numSocketEvents(socket[1]));
//  assert(0 == mX.isRegistered(socket[0], btlso::EventType::e_READ));
//  assert(0 == mX.isRegistered(socket[0], btlso::EventType::e_READ));
//  assert(0 == mX.isRegistered(socket[1], btlso::EventType::e_WRITE));
//  assert(0 == mX.isRegistered(socket[0], btlso::EventType::e_WRITE));
//..
// The following snippets of code show what a 'genericCb' may look like:
//..
//  static void genericCb(btlso::EventType::Type       event,
//                        btlso::SocketHandle::Handle  socket,
//                        int                          bytes,
//                        btlso::EventManager         *mX)
//  {
//      // User specified callback function that is invoked when a socket
//      // event is detected.
//
//      enum {
//          k_MAX_READ_SIZE  =  8192,   // The numbers are for illustrative
//          k_MAX_WRITE_SIZE = 70000    // purposes only.
//      };
//
//      switch (event) {
//        case btlso::EventType::e_READ: {
//            assert(0 < bytes);
//
//            char buffer[k_MAX_READ_SIZE];
//
//            int rc = btlso::SocketImpUtil::read(buffer, socket, bytes, 0);
//            assert(0 < rc);
//        } break;
//        case btlso::EventType::e_WRITE: {
//            char wBuffer[k_MAX_WRITE_SIZE];
//            assert(0 < bytes);
//            assert(k_MAX_WRITE_SIZE >= bytes);
//
//            memset(wBuffer,'4', bytes);
//            int rc = btlso::SocketImpUtil::write(socket, &wBuffer, bytes, 0);
//            assert(0 < rc);
//        } break;
//        // ...
//        default: {
//             ASSERT("Invalid event code" && 0);
//        } break;
//      }
//  }
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_DEFAULTEVENTMANAGERIMPL
#include <btlso_defaulteventmanagerimpl.h>
#endif

#ifndef INCLUDED_BTLSO_EVENT
#include <btlso_event.h>
#endif

#ifndef INCLUDED_BTLSO_EVENTMANAGER
#include <btlso_eventmanager.h>
#endif

#ifndef INCLUDED_BTLSO_EVENTTYPE
#include <btlso_eventtype.h>
#endif

#ifndef INCLUDED_BTLSO_PLATFORM
#include <btlso_platform.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#include <btlso_sockethandle.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_UNORDERED_MAP
#include <bsl_unordered_map.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#if defined(BSLS_PLATFORM_OS_LINUX)

#ifndef INCLUDED_SYS_EPOLL
#include <sys/epoll.h>
#define INCLUDED_SYS_EPOLL
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace bsls { class TimeInterval; }

namespace btlso {

class TimeMetrics;

           // ==========================================
           // class DefaultEventManager<Platform::EPOLL>
           // ==========================================

template <>
class DefaultEventManager<Platform::EPOLL> : public EventManager
{
  private:
    struct HandleEvents {
        bool                   d_isValid;
        EventManager::Callback d_readCallback;
        EventManager::Callback d_writeCallback;
        EventType::Type        d_readEventType;
        EventType::Type        d_writeEventType;
        int                    d_mask;

        BSLMF_NESTED_TRAIT_DECLARATION(HandleEvents, bslmf::IsBitwiseMoveable);
    };

    typedef bsl::unordered_map<int, HandleEvents> EventMap;

    int                                d_epollFd;// epoll fd

    bsl::vector<struct ::epoll_event>  d_signaled;
                                                 // array of 'epoll_event'
                                                 // structures indicating
                                                 // pending IO operations

    bool                               d_isInvokingCb;
                                                 // is the manager invoking
                                                 // callbacks

    TimeMetrics                       *d_timeMetric_p;
                                                 // metrics to use for
                                                 // reporting percent-busy
                                                 // statistics

    EventMap                           d_events; // map of socket handles to
                                                 // associated events

    bsl::vector<EventMap::iterator>    d_entriesBeingRemoved;
                                                 // if we're in a user cb, we
                                                 // will not update the map
                                                 // right away but keep the
                                                 // list what needs to be
                                                 // removed here

    int                                d_numEvents;
                                                 // number of registered events

    // PRIVATE MANIPULATORS
    int dispatchCallbacks(const bsl::vector<struct ::epoll_event>& signaled,
                          int                                      numReady);
        // Invoke the callbacks in the specified 'signaled' vector of events
        // with the specified 'numReady' number of ready callbacks.  Return the
        // number of callbacks that were invoked.

    int dispatchImp(int flags, const bsls::TimeInterval *timeout = 0);
        // For each pending socket event, invoke the corresponding callback
        // registered with this event manager.

  private:
    // NOT IMPLEMENTED
    DefaultEventManager(const DefaultEventManager&);
    DefaultEventManager& operator=(const DefaultEventManager&);

  public:
    // PUBLIC CLASS METHODS
    static bool isSupported();
        // Return true if the current kernel supports this event manager.

    // CREATORS
    explicit
    DefaultEventManager(TimeMetrics      *timeMetric     = 0,
                        bslma::Allocator *basicAllocator = 0);
        // Create a 'epoll'-based event manager.  Optionally specify a
        // 'timeMetric' to report time spent in CPU-bound and IO-bound
        // operations.  If 'timeMetric' is not specified or is 0, these metrics
        // are not reported.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~DefaultEventManager();
        // Destroy this object.  Note that the registered callbacks are NOT
        // invoked.

    // MANIPULATORS
    int dispatch(const bsls::TimeInterval& timeout, int flags);
        // For each pending socket event, invoke the corresponding callback
        // registered with this event manager.  If no event is pending, wait
        // until either (1) at least one event occurs (in which case the
        // corresponding callback(s) is invoked), (2) the specified absolute
        // 'timeout' is reached, or (3) provided that the specified 'flags'
        // contains 'bteso_Flag::k_ASYNC_INTERRUPT', an underlying system call
        // is interrupted by a signal.  Return the number of dispatched
        // callbacks on success, 0 if 'timeout' is reached, and a negative
        // value otherwise; -1 is reserved to indicate that an underlying
        // system call was interrupted.  When such an interruption occurs this
        // method will return (-1) if 'flags' contains
        // 'bteso_Flag::k_ASYNC_INTERRUPT', and otherwise will automatically
        // restart (i.e., reissue the identical system call).  Note that all
        // callbacks are invoked in the same thread that invokes 'dispatch',
        // and the order of invocation, relative to the order of registration,
        // is unspecified.  Also note that -1 is never returned unless 'flags'
        // contains 'bteso_Flag::k_ASYNC_INTERRUPT'.

    int dispatch(int flags);
        // For each pending socket event, invoke the corresponding callback
        // registered with this event manager.  If no event is pending, wait
        // until either (1) at least one event occurs (in which case the
        // corresponding callback(s) is invoked) or (2) provided that the
        // specified 'flags' contains 'bteso_Flag::k_ASYNC_INTERRUPT', an
        // underlying system call is interrupted by a signal.  Return the
        // number of dispatched callbacks on success, and a negative value
        // otherwise; -1 is reserved to indicate that an underlying system call
        // was interrupted.  When such an interruption occurs this method will
        // return (-1) if 'flags' contains 'bteso_Flag::k_ASYNC_INTERRUPT' and
        // otherwise will automatically restart (i.e., reissue the identical
        // system call).  Note that all callbacks are invoked in the same
        // thread that invokes 'dispatch', and the order of invocation,
        // relative to the order of registration, is unspecified.  Also note
        // that -1 is never returned unless 'option' is set to
        // 'bteso_Flag::k_ASYNC_INTERRUPT'.

    int registerSocketEvent(const SocketHandle::Handle&   handle,
                            const EventType::Type         event,
                            const EventManager::Callback& callback);
        // Register with this event manager the specified 'callback' to be
        // invoked when the specified 'event' occurs on the specified socket
        // 'handle'.  Each socket event registration stays in effect until it
        // is subsequently deregistered; the callback is invoked each time the
        // corresponding event is detected.  'EventType::e_READ' and
        // 'EventType::e_WRITE' are the only events that can be registered
        // simultaneously for a socket.  If a registration attempt is made for
        // an event that is already registered, the callback associated with
        // this event will be overwritten with the new one.  Simultaneous
        // registration of incompatible events for the same socket 'handle'
        // will result in undefined behavior.  Return 0 in success and a
        // non-zero value, which is the same as native error code, on error.

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

//-----------------------------------------------------------------------------
//                      INLINE FUNCTION DEFINITIONS
//-----------------------------------------------------------------------------

           // ------------------------------------------
           // class DefaultEventManager<Platform::EPOLL>
           // ------------------------------------------

// ACCESSORS
inline
bool DefaultEventManager<Platform::EPOLL>::hasLimitedSocketCapacity() const
{
    return false;
}

}  // close package namespace

}  // close enterprise namespace

namespace bsl {

template <> struct is_trivially_copyable<epoll_event> : true_type {};

}  // close namespace bsl

#endif // BSLS_PLATFORM_OS_LINUX

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
