// btlso_eventcallbackregistry.cpp                                    -*-C++-*-
#include <btlso_eventcallbackregistry.h>

#include <bslmf_assert.h>

#include <bdlb_bitmaskutil.h>
#include <bdlf_bind.h>

#include <bsls_assert.h>
#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_eventcallbackregistry_cpp,"$Id$ $CSID$")

namespace BloombergLP {

namespace btlso {

// We use the integer values of EventType::Type as bit indexes in an unsigned
// 32-bit value.  Thus, all must be greater than or equal to 0 and less than
// or equal to 32.

BSLMF_ASSERT(0 <= EventType::e_ACCEPT && EventType::e_ACCEPT <= 32);
BSLMF_ASSERT(0 <= EventType::e_CONNECT && EventType::e_CONNECT <= 32);
BSLMF_ASSERT(0 <= EventType::e_READ && EventType::e_READ <= 32);
BSLMF_ASSERT(0 <= EventType::e_WRITE && EventType::e_WRITE <= 32);

                      // ===========================
                      // class EventCallbackRegistry
                      // ===========================

// CREATORS
EventCallbackRegistry::EventCallbackRegistry(bslma::Allocator *basicAllocator)
: d_callbacks(basicAllocator)
, d_size(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

// MANIPULATORS
int EventCallbackRegistry::invoke(const Event& event) const
{
    CallbackMap::const_iterator it = d_callbacks.find(event.handle());
    if (d_callbacks.end() == it) {
        return 1;                                                     // RETURN
    }

    for (EventCallbackVector::const_iterator jt = it->second.begin();
         jt != it->second.end(); ++jt) {
        if (jt->first == event.type()) {
            // Keep a copy of the shared pointer in scope while the callback is
            // executing so the one in the map can be removed or replaced.

            bsl::shared_ptr<EventManager::Callback> callback = jt->second;
            (*callback)();
            return 0;                                                 // RETURN
        }
    }

    return 1;
}

uint32_t EventCallbackRegistry::registerCallback(
                                const Event&                  event,
                                const EventManager::Callback& callback)
{
    // Create a shared-ptr managing a copy of the callback.
    bsl::shared_ptr<EventManager::Callback> managedCallback;
    managedCallback.createInplace(d_allocator_p, bsl::allocator_arg_t(),
                                  d_allocator_p, callback);

    CallbackMap::iterator it = d_callbacks.find(event.handle());
    if (d_callbacks.end() == it) {
        // This is a new callback for this socket.
        it = d_callbacks.insert(bsl::make_pair(event.handle(),
                                               EventCallbackVector())).first;
    }

    uint32_t existingMask = 0;
    for (EventCallbackVector::iterator jt = it->second.begin();
         jt != it->second.end();
         ++jt) {
        existingMask |= bdlb::BitMaskUtil::eq(jt->first);
        if (event.type() == jt->first) {
            // Replace the previous callback
            jt->second.swap(managedCallback);
            return 0;                                                 // RETURN
        }
    }

    // This is a new callback for this event.
    it->second.emplace_back(event.type(), managedCallback);
    ++d_size;
    return existingMask | bdlb::BitMaskUtil::eq(event.type());
}

bool EventCallbackRegistry::remove(const Event& event)
{
    CallbackMap::iterator it = d_callbacks.find(event.handle());
    if (d_callbacks.end() == it) {
        return false;                                                 // RETURN
    }

    EventCallbackVector::iterator jt;
    for (jt = it->second.begin();
         jt != it->second.end() && jt->first != event.type();
         ++jt) ;

    if (it->second.end() == jt) {
        return false;                                                 // RETURN
    }

    if (it->second.end() == it->second.erase(jt)) {
        // Last event for this socket removed
        d_callbacks.erase(it);
    }
    --d_size;
    return true;
}

void EventCallbackRegistry::removeAll()
{
    d_callbacks.clear();
    d_size = 0;
}

int EventCallbackRegistry::removeSocket(const SocketHandle::Handle& socket)
{
    CallbackMap::iterator it = d_callbacks.find(socket);
    if (d_callbacks.end() == it) {
        return 0;                                                     // RETURN
    }

    int numRemoved = static_cast<int>(it->second.size());
    d_callbacks.erase(it);
    d_size -= numRemoved;
    return numRemoved;
}

// ACCESSORS
bool EventCallbackRegistry::contains(const Event& event) const
{
    CallbackMap::const_iterator it = d_callbacks.find(event.handle());
    if (d_callbacks.end() == it) {
        return false;                                                 // RETURN
    }

    EventCallbackVector::const_iterator jt;
    for (jt = it->second.begin();
         jt != it->second.end() && jt->first != event.type();
         ++jt) ;

    return it->second.end() != jt;
}

uint32_t EventCallbackRegistry::getRegisteredEventMask(
                                   const SocketHandle::Handle& socket) const
{
    uint32_t mask = 0;
    CallbackMap::const_iterator it = d_callbacks.find(socket);
    if (d_callbacks.end() != it) {
        for (EventCallbackVector::const_iterator jt = it->second.begin();
             jt != it->second.end();
             ++jt) {
            mask |= bdlb::BitMaskUtil::eq(jt->first);
        }
    }

    return mask;
}

}  // close package namespace

}  // close enterprise namespace

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
