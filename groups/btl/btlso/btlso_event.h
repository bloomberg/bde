// btlso_event.h                                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_EVENT
#define INCLUDED_BTLSO_EVENT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide component to describe an event on a socket.
//
//@CLASSES:
//  btlso::Event: representation of an event on a socket
//  btlso::EventHash: namespace for a hash function
//
//@DESCRIPTION: This component provides an unconstrained (value-semantic)
// attribute class, 'btlso::Event', which represents an event on a particular
// socket.  The two attributes are the socket handle and the event type.  The
// recognized events are 'btlso::EventType::e_ACCEPT',
// 'btlso::EventType::e_CONNECT', 'btlso::EventType::e_READ', and
// 'btlso::EventType::e_WRITE'.  Accessors and manipulators are provided for
// the socket handle and the event type.  In order to facilitate storing
// 'btlso::Event' objects into associative containers, a hash functor,
// 'btlso::EventHash', is also provided.
//
///Attributes
///----------
//..
//  Name    Type                 Constraints
//  ------  ----                 -----------
//  handle  SocketHandle::Handle none
//  type    EventType::Type      none
//..
//: o 'handle': The socket handle on which the event is registered.
//:
//: o 'type': The event type corresponding to the event registered.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing an Event Manager
/// - - - - - - - - - - - - - - - - - - - -
// First, we define a class, 'my_EventManager', that allows clients to register
// an asynchronous callback that is invoked when a certain event on a socket
// handle is complete.  As an example, objects of this class would allow users
// to register a read callback that is invoked when there is data available on
// a socket.
//
// The definition of the 'my_EventManager' class is provided below with
// non-relevant portions elided for brevity:
//..
//  class my_EventManager {
//      // This class allows clients to register asynchronous callbacks that
//      // are invoked when certain events fire on a socket handle.
//
//    public:
//      // TYPES
//      typedef bsl::function<void()> Callback;
//
//    private:
//      typedef bsl::unordered_map<btlso::Event,
//                                 Callback,
//                                 btlso::EventHash> CallbacksMap;
//
//      // DATA
//      CallbacksMap d_callbacks;
//
//      . . .
//
//    public:
//      // CREATORS
//      my_EventManager(bslma::Allocator *basicAllocator = 0);
//          // Create a 'my_EventManager' object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      ~my_EventManager();
//
//      // MANIPULATORS
//      int registerEvent(btlso::SocketHandle::Handle handle,
//                        btlso::EventType::Type      eventType,
//                        Callback&                   callback);
//          // Register the specified 'callback' to be invoked when the
//          // specified 'eventType' fires on the specified 'handle'.  Return
//          // 0 on success and non-zero value otherwise.
//
//      . . .
//  };
//..
// Then, we provide the function definitions for the 'my_EventManager' class:
//..
//  // CREATORS
//  my_EventManager::my_EventManager(bslma::Allocator *basicAllocator)
//  : d_callbacks(basicAllocator)
//  {
//  }
//
//  my_EventManager::~my_EventManager()
//  {
//  }
//
//  // MANIPULATORS
//  int my_EventManager::registerEvent(btlso::SocketHandle::Handle handle,
//                                     btlso::EventType::Type      eventType,
//                                     Callback&                   callback)
//  {
//..
// Next, we create a 'btlso_Event' object
//..
//      btlso::Event event(handle, eventType);
//..
// Then, we confirm if a callback is already registered for 'event'.  If so we
// simply update the callback:
//..
//      CallbacksMap::iterator iter = d_callbacks.find(event);
//      if (d_callbacks.end() != iter) {
//          iter->second = callback;
//          return 0;
//      }
//..
// Next, we add the event to the callbacks map:
//..
//      bool insertedEvent = d_callbacks.insert(
//                                     bsl::make_pair(event, callback)).second;
//      assert(insertedEvent);
//
//      return 0;
//  }
//..
// Now, we specify a read callback:
//..
//  void readCallback(btlso::SocketHandle::Handle handle)
//      // Process data read from the specified 'handle'.
//  {
//      // . . .
//  }
//..
// Finally, we construct a 'my_EventManager' object and register an event:
//..
//      my_EventManager             manager;
//
//      btlso::SocketHandle::Handle handle;
//
//      // Open the socket 'handle'
//
//      // . . .
//
//      btlso::EventType::Type    eventType = btlso::EventType::e_READ;
//      my_EventManager::Callback callback  = bdlf::BindUtil::bind(
//                                                               &readCallback,
//                                                               handle);
//
//      const int rc = manager.registerEvent(handle, eventType, callback);
//      assert(0 == rc);
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

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

namespace BloombergLP {

namespace btlso {

                             // ===========
                             // class Event
                             // ===========

class Event {
    // This attribute class represents an event on a socket.  The recognized
    // events are 'btlso::EventType::e_ACCEPT', 'btlso::EventType::e_CONNECT',
    // 'btlso::EventType::e_READ', and 'btlso::EventType::e_WRITE'.  Accessors
    // and manipulators are provided for the socket handle and the event type.

    // DATA
    SocketHandle::Handle d_handle;
    EventType::Type      d_type;

    // FRIENDS
    friend bool operator==(const Event& lhs, const Event& rhs);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Event, bsl::is_trivially_copyable)

    // CREATORS
    Event(const SocketHandle::Handle& handle, const EventType::Type type);
        // Construct an 'Event' object corresponding to the specified event
        // 'type' on the specified socket 'handle'.

    Event(const Event& original);
        // Create an 'Event' object having the same value as the specified
        // 'original' object.

    ~Event();
        // Destroy this event object.

    // MANIPULATORS
    void setHandle(const SocketHandle::Handle& handle);
        // Reset the handle stored by this object to the specified 'handle'.
        // The type remains unchanged.

    void setType(EventType::Type type);
        // Reset the type stored by this object to the specified 'type'.  The
        // socket handle remains unchanged.

    // ACCESSORS
    SocketHandle::Handle handle() const;
        // Get the socket handle of this event.

    EventType::Type type() const;
        // Get the type of this event.
};

// FREE OPERATORS
bool operator==(const Event& lhs, const Event& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' events have the same
    // value, and 'false' otherwise.  Two events have the same value if and
    // only if they have the same socket handle and event type field values.

bool operator!=(const Event& lhs, const Event& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' events do not have the
    // same value, and 'false' otherwise.  Two events have the same value if
    // and only if they have the same socket handle and event type field
    // values.

                           // ===============
                           // class EventHash
                           // ===============

struct EventHash {
    // This 'struct' provides a functor that computes a hash value for a
    // 'Event' object, and is suitable for use with 'bsl::unordered_map' or
    // other unordered associative containers.

    // ACCESSORS
    bsl::size_t operator()(const Event& event) const;
        // Return the hash value for the specified 'event'.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                             // -----------
                             // class Event
                             // -----------

// CREATORS
inline
Event::Event(const Event& original)
: d_handle(original.d_handle)
, d_type(original.d_type)
{
}

inline
Event::Event(const SocketHandle::Handle& handle, const EventType::Type type)
: d_handle(handle)
, d_type(type)
{
}

inline
Event::~Event()
{
}

// MANIPULATORS
inline
void Event::setHandle(const SocketHandle::Handle& handle)
{
    d_handle = handle;
}

inline
void Event::setType(const EventType::Type type)
{
    d_type = type;
}

// ACCESSORS
inline
SocketHandle::Handle Event::handle() const
{
    return d_handle;
}

inline
EventType::Type Event::type() const
{
    return d_type;
}

}  // close package namespace

// FREE OPERATORS
inline
bool btlso::operator==(const Event& lhs, const Event& rhs)
{
    return lhs.d_handle == rhs.d_handle && lhs.d_type == rhs.d_type;
}

inline
bool btlso::operator!=(const Event& lhs, const Event& rhs)
{
    return !(lhs == rhs);
}

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
