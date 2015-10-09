// btlso_defaulteventmanager_select.h                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_DEFAULTEVENTMANAGER_SELECT
#define INCLUDED_BTLSO_DEFAULTEVENTMANAGER_SELECT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a 'select'-based socket-event multiplexer.
//
//@CLASSES:
//  btlso::DefaultEventManager<btlso::Platform::SELECT>: 'select' multiplexer
//
//@SEE_ALSO: btlso_eventmanager btlso_defaulteventmanager btlso_timemetrics
//
//@DESCRIPTION: This component provides a class,
// 'btlso::DefaultEventManager<btlso::Platform::SELECT>', that implements the
// 'btlso::EventManager' protocol using the 'select' system call available on
// platforms conforming to POSIX.1g.
//
// Generally speaking, a platform imposes a limit on the number of socket
// handles that can be registered with 'select'.  This limit, which is a
// compile time constant (and, therefore, cannot be increased at run time), can
// be as low as 64 (e.g., Windows) or as high as 1024 (e.g., Solaris).
// However, it is often required to handle more than this maximum number of
// simultaneous connections.  This component does not provide a solution to
// this problem but provides an accessor function, 'canRegisterSockets', that
// allows clients to identify if this event manager is at the socket
// registration limit.  If that is the case then clients can create more
// objects of this class for registering their sockets.
//
///Component Diagram
///-----------------
// This specialized component is one of the specializations of the
// 'btlso_defaulteventmanager' component; the other components are shown
// (schematically) on the following diagram:
//..
//                          _bteso_defaulteventmanager_
//                 _______/     |       |        |     \________
//                 *_poll  *_pollset *_select *_devpoll  *_epoll
//
//..
//
///Thread Safety
///-------------
// The 'select'-based multiplexers provided by this component depend on a
// 'bdema'-style allocator to supply memory.  The allocator is installed at
// creation and is NOT managed by the multiplexers.  If the allocator is not
// *thread* *safe* (*thread* *enabled*) then the instances of this component
// using that allocator will consequently not be *thread* *safe* (*thread*
// *enabled*).  Moreover, if two distinct instances of a multiplexer share the
// same allocator, thread-safety is guaranteed if and only if the allocator is
// *thread* *enabled*.  If no allocator is provided at creation, the default
// allocator, which is *thread* *enabled*, is used.  Otherwise, the following
// is guaranteed:
//
//: o A single instance of this event manager is *not* *thread* *safe* with
//:   respect to operations invoked on the same instance from different
//:   threads: such operations are *not* guaranteed to work correctly.
//:
//: o An instance of this event manager *is* *thread* *safe* with respect to
//:   operations invoked on *different* *instances* from different threads:
//:   there will be no side-effects from such operations (which, generally
//:   speaking, means that there is no 'static' data).
//:
//: o Distinct instances of this event manager are *thread* *enabled* meaning
//:   that operations invoked on distinct instances from different threads can
//:   proceed in parallel.
//:
//: o This event manager is not *async-safe*, meaning that one or more
//:   functions cannot be invoked safely from a signal handler.
//
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
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using an Event Manager
///- - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use this event manager with
// a non-blocking socket.  First, create a 'btlso::TimeMetrics' object and a
// 'btlso::DefaultEventManager<btlso::Platform::SELECT>' object; also create a
// (locally-connected) socket pair:
//..
//  btlso::TimeMetrics timeMetric(btlso::TimeMetrics::e_MIN_NUM_CATEGORIES,
//                                btlso::TimeMetrics::e_CPU_BOUND);
//
//  btlso::DefaultEventManager<btlso::Platform::SELECT> mX(&timeMetric);
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
//  assert(0 == mX.isRegistered(socket[0], btlso::EventType::e_WRITE));
//  assert(0 == mX.isRegistered(socket[1], btlso::EventType::e_WRITE));
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
//          k_MAX_READ_SIZE  = 8192,   // The numbers are for illustrative
//          k_MAX_WRITE_SIZE = 70000   // purposes only.
//      };
//
//      switch (event) {
//        case btlso::EventType::e_READ: {
//            assert(0 < bytes);
//            char buffer[k_MAX_READ_SIZE];
//
//            int rc = btlso::SocketImpUtil::read(buffer, socket, bytes, 0);
//            assert(0 < rc);
//        } break;
//        case btlso::EventType::e_WRITE: {
//            char wBuffer[k_MAX_WRITE_SIZE];
//            assert(0 < bytes);
//            assert(k_MAX_WRITE_SIZE >= bytes);
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

#ifndef INCLUDED_BTLSO_PLATFORM
#include <btlso_platform.h>
#endif

#ifndef INCLUDED_BDLMA_CONCURRENTPOOLALLOCATOR
#include <bdlma_concurrentpoolallocator.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSL_UNORDERED_MAP
#include <bsl_unordered_map.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifdef BTLSO_PLATFORM_WIN_SOCKETS
  #ifndef INCLUDED_WINSOCK2
    #include <winsock2.h>
    #define INCLUDED_WINSOCK2
  #endif
#endif

#ifdef BTLSO_PLATFORM_BSD_SOCKETS
  #ifndef INCLUDED_SYS_SELECT
    #include <sys/select.h>
    #define INCLUDED_SYS_SELECT
  #endif
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace btlso {

class TimeMetrics;

            // ===========================================
            // class DefaultEventManager<Platform::SELECT>
            // ===========================================

template <>
class DefaultEventManager<Platform::SELECT> : public EventManager {
    // This class implements the 'EventManager' protocol to provide an event
    // manager that uses the 'select' system call.

  public:
    enum {
        k_MAX_NUM_HANDLES = FD_SETSIZE  // maximum number of socket handles
                                        // that can be registered with an
                                        // event manager (of this type)

    };

  private:
    typedef bsl::unordered_map<Event, EventManager::Callback, EventHash>
                                                                      EventMap;

    // Due to the initialization dependency between 'd_eventsAllocator' and
    // 'd_events' their declaration order should always be as follows.

    bdlma::ConcurrentPoolAllocator  d_eventsAllocator;  // event map allocator

    EventMap                        d_events;           // socket events and
                                                        // associated callbacks

    fd_set                          d_readSet;          // set of descriptors
                                                        // monitored for
                                                        // incoming data

    int                             d_numRead;          // number of sockets
                                                        // in the read set

    fd_set                          d_writeSet;         // set of descriptors
                                                        // monitored for
                                                        // outgoing data

    int                             d_numWrite;         // number of sockets
                                                        // in the write set

    fd_set                          d_exceptSet;        // set of descriptors
                                                        // monitored for
                                                        // exceptions

    int                             d_maxFd;            // maximum number of
                                                        // socket descriptors

    TimeMetrics                    *d_timeMetric;       // time metrics given
                                                        // to this object

    mutable bsl::vector<Event>      d_signaledReads;

    mutable bsl::vector<Event>      d_signaledWrites;   // temporary arrays
                                                        // used to dispatch
                                                        // callbacks

    // PRIVATE ACCESSORS
    bool checkInternalInvariants() const;
        // Verify that every socket handle that is registered in the specified
        // 'events' is set in the appropriate set (e.g., either 'readSet' or
        // 'writeSet' depending on whether or not this is a READ or WRITE
        // event).  Return 'true' on success, and 'false' otherwise.

    int dispatchCallbacks(int           numEvents,
                          const fd_set& readSet,
                          const fd_set& writeSet,
                          const fd_set& exceptSet) const;
        // Dispatch the specified 'numEvents' callbacks from the specified
        // 'readSet', 'writeSet', and 'exceptSet' file descriptor sets that
        // were signalled as ready.

  public:
    // CREATORS
    explicit DefaultEventManager(bslma::Allocator *basicAllocator = 0);
    explicit DefaultEventManager(TimeMetrics      *timeMetric,
                                 bslma::Allocator *basicAllocator = 0);
        // Create a 'select'-based event manager.  Optionally specify a
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
        // that -1 is never returned if 'option' is not set to
        // 'bteso_Flag::k_ASYNC_INTERRUPT'.

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
        // method will return -1 if 'flags' contains
        // 'bteso_Flag::k_ASYNC_INTERRUPT', and otherwise will automatically
        // restart (i.e., reissue the identical system call).  Note that all
        // callbacks are invoked in the same thread that invokes 'dispatch',
        // and the order of invocation, relative to the order of registration,
        // is unspecified.  Also note that -1 is never returned unless 'flags'
        // contains 'bteso_Flag::k_ASYNC_INTERRUPT'.

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
        // will result in undefined behavior.  Return 0 on success and a
        // non-zero value otherwise.  The behavior is undefined unless
        // 'canRegisterSockets()' is 'true'.

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
    bool canRegisterSockets() const;
        // Return 'true' if this event manager can register additional sockets,
        // and 'false' otherwise.  Note that if 'canRegisterSockets' is 'false'
        // then a subsequent call to register an event (without an intervening
        // call to deregister an event) will result in undefined behavior.

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
//                      INLINE FUNCTIONS DEFINITIONS
//-----------------------------------------------------------------------------

           // -------------------------------------------
           // class DefaultEventManager<Platform::SELECT>
           // -------------------------------------------

// ACCESSORS
inline
int DefaultEventManager<Platform::SELECT>::numEvents() const
{
    return static_cast<int>(d_events.size());
}

inline
bool DefaultEventManager<Platform::SELECT>::hasLimitedSocketCapacity() const
{
    return true;
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
