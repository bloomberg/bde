// btlso_eventcallbackregistry.h                                      -*-C++-*-
#ifndef INCLUDED_BTLSO_EVENTCALLBACKREGISTRY
#define INCLUDED_BTLSO_EVENTCALLBACKREGISTRY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: A mapping of Events to callback functions with deferred removal
//
//@CLASSES:
//  btlso::EventCallbackRegistry: stores callback functions for Events
//
//@SEE_ALSO: btlso_eventmanager
//
//@DESCRIPTION: This component provides a container associating
// 'EventManager::Callback' instances with 'Event' values.  Given an 'Event',
// the associated 'Callback' can be invoked via 'EventCallbackRegistry'; any
// attempts to remove a callback while it is executing will defer the
// destruction until the callback completes.  Thus this component provides
// support for registering 'Event' callbacks that can deregister (and/or
// re-register) themselves.
//
// This type also maintains, for each socket handle with registered events,
// a bitmask of all registered events.  This bitmask is formed by using the
// integer values of EventType::Type as bit indexes.
//
///Thread Safety
///-------------
// The class provided by this component depends on a 'bslma::Allocator'
// instance to supply memory.  If the allocator is not thread enabled then the
// instances of this component that use the same allocator instance will
// consequently not be thread safe.  Otherwise, this component provides the
// following guarantees:
//
// 'EventCallbackRegistry' is not *thread-enabled* (it is not safe to invoke
// methods from different threads on the same instance).
// 'EventCallbackRegistry' is *'const'* *thread-safe*, meaning that its
// accessors may be invoked concurrently from different threads, but it is not
// safe to access or modify an 'EventCallbackRegistry' instance while another
// thread modifies the same object.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Self-Deregistering Callback
/// - - - - - - - - - - - - - - - - - - -
// 'EventCallbackRegistry' is intended to simplify the implementation of socket
// event manager components by handling bookkeeping.  Those components, in
// turn, will provide callback-based interfaces to their callers.  In this
// example we use 'EventCallbackRegistry' to manage a callback for a 'READ'
// event that deregisters itself after consuming a certain number of bytes.
//
// First, we define the callback to be invoked.  Note that details of error
// handling are elided from this example.
//..
//  void readBytes(const SocketHandle::Handle&  socket,
//                 int                          bytesToRead,
//                 char                        *buffer,
//                 int                         *bytesRead,
//                 EventCallbackRegistry       *registry) {
//      // Read up to 'bytesToRead' bytes from the specified 'socket' into the
//      // specified 'buffer', updating the specified 'bytesRead' counter to
//      // store the total number of bytes.  Once 'bytesToRead' bytes have
//      // been read into 'buffer', deregister the 'READ' callback from the
//      // specified 'registry' for 'socket'.
//
//      int maxRead = bytesToRead - *bytesRead;
//      int numRead = SocketImpUtil::read(buffer + *bytesRead, socket,
//                                        maxRead, 0);
//      *bytesRead += numRead;
//      if (*bytesRead == bytesToRead) {
//          registry->remove(Event(socket, EventType::e_READ));
//      }
//
//      // At this point, we can confirm that the registry reports that there
//      // is no callback registered, though in fact the callback object
//      // remains in scope.
//
//      assert(!registry->contains(Event(socket, EventType::e_READ)));
//  }
//..
// Next, we create an event callback registry and register this callback for
// a socket.  Note that the details of creating 'socket' (an object of type
// SocketHandle::Handle) are elided from this example.
//..
//  EventCallbackRegistry registry;
//  char data[NUM_BYTES];
//  int numRead = 0;
//
//  registry.registerCallback(Event(socket, EventType::e_READ),
//                            bdlf::BindUtil::bind(&readBytes, socket,
//                                                 NUM_BYTES, data,
//                                                 &numRead, &registry));
//..
// Finally, we write data to the socket, the details of which are elided from
// this example; and we invoke the read callback via the registry.  The
// bound functor object will be destroyed only after the callback completes
// (thus ensuring any bound arguments remain valid for the duration of the
// callback).
//..
//  int rc = registry.invoke(Event(socket, EventType::e_READ));
//  assert(0 == rc);
//  assert(NUM_BYTES == numRead);
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_EVENT
#include <btlso_event.h>
#endif

#ifndef INCLUDED_BTLSO_EVENTMANAGER
#include <btlso_eventmanager.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#include <btlso_sockethandle.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSL_CSTDINT
#include <bsl_cstdint.h>
#endif

#ifndef INCLUDED_BSL_UNORDERED_MAP
#include <bsl_unordered_map.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

#ifndef INCLUDED_BSL_UTITLITY
#include <bsl_utility.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace btlso {

                      // ===========================
                      // class EventCallbackRegistry
                      // ===========================

class EventCallbackRegistry {
    // This class implements a map of socket events to corresponding callback
    // functions.  Callbacks are invoked via the registry, allowing for
    // callbacks to deregister and reregister themselves.

    // TYPES
    typedef bsl::vector<bsl::pair<EventType::Type,
                                  bsl::shared_ptr<EventManager::Callback> > >
       EventCallbackVector;

    typedef bsl::unordered_map<SocketHandle::Handle, EventCallbackVector>
       CallbackMap;

    // DATA
    CallbackMap        d_callbacks;    // container of registered socket events
                                       // and associated callbacks

    int                d_size;         // total number of callbacks

    bslma::Allocator  *d_allocator_p;  // allocator (held, not owned)
  private:
    // NOT IMPLEMENTED
    EventCallbackRegistry(const EventCallbackRegistry&);
    EventCallbackRegistry& operator=(const EventCallbackRegistry&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(btlso::EventCallbackRegistry,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit
    EventCallbackRegistry(bslma::Allocator *basicAllocator = 0);
        // Create a mapping of 'Event' objects to callback functions.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    //! ~EventCallbackRegistry() = default;

    // MANIPULATORS
    uint32_t registerCallback(const Event&                  event,
                              const EventManager::Callback& callback);
        // Store the specified 'callback' to be invoked for the specified
        // 'event'.  Return the new bitmask for 'event.handle()' if a new
        // callback was registered, or 0 if an existing callback was replaced
        // (in which case, if the existing callback is executing, it is
        // deregistered but not destroyed until it completes).

    bool remove(const Event& event);
        // Remove the callback for the specified 'event'.  Return 'true' if
        // a callback was removed, and 'false' if no callback was registered
        // for 'event'.  If the callback is currently being invoked, it is
        // deregistered but not destroyed until it completes.

    void removeAll();
        // Remove all registered callbacks.  If 'invoke' is currently being
        // executed on this object, the callbacks are deregistered but are not
        // destroyed until 'invoke' completes.

    int removeSocket(const SocketHandle::Handle& socket);
        // Remove any registered events for the specified 'socket'.
        // Return the number of events removed.

    // ACCESSORS
    bool contains(const Event& event) const;
        // Return 'true' if this registry contains a callback for the specified
        // 'event', and 'false' otherwise.

    uint32_t getRegisteredEventMask(const SocketHandle::Handle& socket) const;
        // Return the bitmask of events registered for the specified 'socket'.
        // In the returned mask, the bit at the index of the 'btls::EventType'
        // value corresponding to each registered event type is set (for
        // example, the bit at the 'btlso::EventType::e_READ' index is set if a
        // read event is registered).

    int invoke(const Event& event) const;
        // Invoke the callback for the specified 'event'.  Return 0 on success,
        // and a non-zero value if no callback is registered for 'event'.

    int numSockets() const;
        // Return the number of sockets for which events are registered.

    int numCallbacks() const;
        // Return the number of registered callbacks.

    template<typename VISITOR>
    void visitEvents(VISITOR *visitor) const;
        // Invoke the specified 'visitor' on each registered event. 'VISITOR'
        // shall be a functor type that can be invoked as if it had the
        // following signature: 'void operator()(const Event&)'.

    template<typename VISITOR>
    void visitSockets(VISITOR *visitor) const;
        // Invoke the specified 'visitor' on each socket with any registered
        // events.  'VISITOR' shall be a functor type that can be invoked as if
        // it had the following signature:
        // 'void operator()(const SocketHandle::Handle&)'.
};

//-----------------------------------------------------------------------------
//                        INLINE FUNCTION DEFINITIONS
//-----------------------------------------------------------------------------

                     // ---------------------------
                     // class EventCallbackRegistry
                     // ---------------------------

// ACCESSORS
inline
int EventCallbackRegistry::numSockets() const
{
    return static_cast<int>(d_callbacks.size());
}

inline
int EventCallbackRegistry::numCallbacks() const
{
    return d_size;
}

template<typename VISITOR>
inline void EventCallbackRegistry::visitEvents(VISITOR *visitor) const
{
    for (CallbackMap::const_iterator handleIt = d_callbacks.begin();
         handleIt != d_callbacks.end();
         ++handleIt) {
        for (EventCallbackVector::const_iterator cbIt =
                 handleIt->second.begin();
             cbIt != handleIt->second.end();
             ++cbIt) {
            (*visitor)(Event(handleIt->first, cbIt->first));
        }
    }
}

template<typename VISITOR>
inline void EventCallbackRegistry::visitSockets(VISITOR *visitor) const
{
    for (CallbackMap::const_iterator it = d_callbacks.begin();
         it != d_callbacks.end();
         ++it) {
        (*visitor)(it->first);
    }
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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


