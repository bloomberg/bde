// btlso_defaulteventmanager_epoll.cpp                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_defaulteventmanager_epoll.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_defaulteventmanager_epoll_cpp,"$Id$ $CSID$")

#if defined(BSLS_PLATFORM_OS_LINUX)

#include <btlso_flag.h>
#include <btlso_socketimputil.h>

#include <btlso_timemetrics.h>

#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_functional.h>
#include <bsl_utility.h>

#include <bsl_cstdio.h>
#include <bsl_c_errno.h>

namespace BloombergLP {

namespace btlso {

namespace {

int sleep(int                       *resultErrno,
          const bsls::TimeInterval&  timeout,
          int                        flags,
          btlso::TimeMetrics        *metrics)
{
    bsls::TimeInterval now(bdlt::CurrentTime::now());

    while (timeout > now) {
        bsls::TimeInterval currTimeout(timeout - now);
        struct timespec    ts;

        ts.tv_sec  = static_cast<time_t>(currTimeout.seconds());
        ts.tv_nsec = static_cast<long>(currTimeout.nanoseconds());

        // Sleep till it's time.

        int savedErrno;
        int rc;
        if (metrics) {
            metrics->switchTo(btlso::TimeMetrics::e_IO_BOUND);
            rc = nanosleep(&ts, 0);
            savedErrno = errno;
            metrics->switchTo(btlso::TimeMetrics::e_CPU_BOUND);
        }
        else {
            rc = nanosleep(&ts, 0);
            savedErrno = errno;
        }

        errno = 0;
        *resultErrno = savedErrno;
        if (0 > rc) {
            BSLS_ASSERT(savedErrno == EINTR);

            if (flags & btlso::Flag::k_ASYNC_INTERRUPT) {
                // We're allowing async interrupts.

                return -1;                                            // RETURN
            }
        }
        now = bdlt::CurrentTime::now();
    }
    return 0;
}

int translateEventToMask(btlso::EventType::Type event)
{
    switch (event) {
      case btlso::EventType::e_ACCEPT:                          // FALL THROUGH
      case btlso::EventType::e_READ: {
        return EPOLLIN;                                               // RETURN
      } break;
      case btlso::EventType::e_CONNECT:                         // FALL THROUGH
      case btlso::EventType::e_WRITE: {
        return EPOLLOUT;                                              // RETURN
      } break;
      default: {
        BSLS_ASSERT("Invalid event (must be unreachable)" && 0);

        return -1;                                                    // RETURN
      } break;
    }
}

}  // close unnamed namespace

           // ------------------------------------------
           // class DefaultEventManager<Platform::EPOLL>
           // ------------------------------------------

typedef btlso::DefaultEventManager<btlso::Platform::EPOLL> EventManagerName;
    // Alias for brevity.

// PRIVATE MANIPULATORS
int EventManagerName::dispatchCallbacks(
                             const bsl::vector<struct ::epoll_event>& signaled,
                             int                                      numReady)
{
    int numCallbacks = 0;

    // Letting people know that we're executing user-callbacks.

    d_isInvokingCb = (0 != numReady);

    for (int i = 0; i < numReady; ++i) {
        const struct ::epoll_event *curEvent = &signaled[i];

        BSLS_ASSERT(curEvent);
        BSLS_ASSERT(curEvent->events);

        EventMap::value_type *info = (EventMap::value_type *)
                                                            curEvent->data.ptr;
        HandleEvents *events = &info->second;

        // If true, this means that a callback executed during this
        // 'dispatchCallbacks' run removed this handle.

        if (!events->d_isValid) {
            BSLS_ASSERT(!d_entriesBeingRemoved.empty());
            BSLS_ASSERT(0 != i);
            continue;
        }

        // Read/Accept.

        if (curEvent->events & (EPOLLIN | EPOLLERR | EPOLLHUP)
         && events->d_readCallback) {
            events->d_readCallback.operator()();
            ++numCallbacks;

            // Need to recheck the valid bit, the previous callback could have
            // un-registered it.

            if (!events->d_isValid) {
                BSLS_ASSERT(!d_entriesBeingRemoved.empty());
                continue;
            }
        }

        // Write/Connect.

        if (curEvent->events & (EPOLLOUT | EPOLLERR | EPOLLHUP)
         && events->d_writeCallback) {
            info->second.d_writeCallback.operator()();
            ++numCallbacks;
        }
    }

    d_isInvokingCb = false;

    // We're going to remove from the map any event that was de-registered
    // during a callback.  Not removing them when we're in the for loop allows
    // up to use directly the pointer returned in the epoll data.

    bsl::vector<EventMap::iterator>::const_iterator it;
    for (it = d_entriesBeingRemoved.begin();
         d_entriesBeingRemoved.end() != it;
         ++it) {
        if ((*it)->second.d_isValid) {
            // Item was first removed, then registered again.  We do not have
            // anything to do.

            continue;
        }
        d_events.erase(*it);
    }
    d_entriesBeingRemoved.clear();
    return numCallbacks;
}

int EventManagerName::dispatchImp(int                       flags,
                                  const bsls::TimeInterval *timeout)
{
    bsls::TimeInterval now;
    if (timeout) {
        now = bdlt::CurrentTime::now();
    }
    int numCallbacks = 0;                    // number of callbacks dispatched
    const bool allowAsyncInterrupts =
                               (0 != (btlso::Flag::k_ASYNC_INTERRUPT & flags));

    do {
        int numReady;                // number of returned sockets
        int savedErrno = 0;          // saved errno value set by poll
        while(1) {
            int epollTimeout = -1;
            if (timeout) {
                if (*timeout < now) {
                    // The epoll_wait should return immediately.

                    epollTimeout = 0;
                }
                else {
                    // Calculate the time remaining in ms

                    bsls::TimeInterval curr_timeout(*timeout - now);
                    bsls::Types::Int64 totalMs =
                                              curr_timeout.totalMilliseconds();
                    BSLS_ASSERT(totalMs < INT_MAX);

                    // totalMs is rounded down

                    epollTimeout = static_cast<int>(totalMs + 1);
                }
            }

            d_signaled.resize(d_events.size());
            if (d_signaled.empty()) {
                // No fds to wait for.  We'll just sleep if there is a timeout.

                if (!timeout || 0 == epollTimeout) {
                    numReady = 0;
                    break;
                }
                numReady = sleep(&savedErrno, *timeout, flags, d_timeMetric_p);
            }
            else {
                if (d_timeMetric_p) {
                    d_timeMetric_p->switchTo(btlso::TimeMetrics::e_IO_BOUND);
                }

                numReady = epoll_wait(d_epollFd,
                                      &d_signaled.front(),
                                      d_signaled.size(),
                                      epollTimeout);

                BSLS_ASSERT(-1 != numReady || EINTR == errno);
                savedErrno = errno;
                if (d_timeMetric_p) {
                    d_timeMetric_p->switchTo(btlso::TimeMetrics::e_CPU_BOUND);
                }
            }
            errno = 0;
            if (numReady > 0
             || (numReady < 0
              && EINTR == savedErrno
              && allowAsyncInterrupts)) {
                // Either a fd is ready or we've been interrupted and the user
                // wants to know.

                break;
            }
            if (timeout) {
                now = bdlt::CurrentTime::now();
                if (now >= *timeout) {
                    // We reached the timeout.

                    break;
                }
            }
        }

        if (0 >= numReady) {
            return numReady
                   ? -1 == numReady && EINTR == savedErrno
                     ? -1
                     : -2
                   : 0;                                               // RETURN
        }
        numCallbacks += dispatchCallbacks(d_signaled, numReady);
        if (timeout) {
            now = bdlt::CurrentTime::now();
        }
    } while (0 == numCallbacks && (0 == timeout || now < *timeout));

    return numCallbacks;
}

// PUBLIC CLASS METHODS
bool EventManagerName::isSupported()
{
    int fd = epoll_create(128);
    if (-1 == fd) {
        return false;                                                 // RETURN
    }
    close(fd);
    return true;
}

// CREATORS
EventManagerName::DefaultEventManager(btlso::TimeMetrics *timeMetric,
                                      bslma::Allocator   *basicAllocator)
: d_epollFd(-1)
, d_signaled(basicAllocator)
, d_isInvokingCb(false)
, d_timeMetric_p(timeMetric)
, d_events(128, bsl::hash<int>(), bsl::equal_to<int>(), basicAllocator)
, d_entriesBeingRemoved(basicAllocator)
, d_numEvents(0)
{
    d_epollFd = epoll_create(128);
    if (-1 == d_epollFd) {
        bsl::perror("epoll_create returned ");
        BSLS_ASSERT_OPT("epoll_create() failed" && 0);
    }
}

EventManagerName::~DefaultEventManager()
{
    int rc = close(d_epollFd);
    BSLS_ASSERT(0 == rc);
}

// MANIPULATORS
void EventManagerName::deregisterAll()
{
    d_numEvents = 0;
    d_entriesBeingRemoved.reserve(d_events.size());
    for (EventMap::iterator it = d_events.begin();
         d_events.end() != it;
         ++it) {

        if (!it->second.d_isValid) {
            continue;
        }
        struct epoll_event event = { 0, { 0 } };
        int ret = epoll_ctl(d_epollFd, EPOLL_CTL_DEL, it->first, &event);

        // epoll removes closed file descriptors automatically.

        BSLS_ASSERT(0 == ret || ENOENT == errno || EBADF == errno);

        it->second.d_isValid = false;
        it->second.d_mask = 0;
        it->second.d_writeCallback = btlso::EventManager::Callback();
        it->second.d_readCallback = btlso::EventManager::Callback();
        d_entriesBeingRemoved.push_back(it);
    }

    // If we're in a user-specified callback, we'll clean up in
    // 'dispatchCallbacks'.  That keeps every pointer in events data inside
    // 'd_signaled' valid.

    if (!d_isInvokingCb) {
        d_events.clear();
        d_signaled.clear();
        d_entriesBeingRemoved.clear();
    }
}

void EventManagerName::deregisterSocketEvent(
                                     const btlso::SocketHandle::Handle& handle,
                                     btlso::EventType::Type             event)
{
    EventMap::iterator it = d_events.find(handle);

    if (d_events.end() == it || !it->second.d_isValid) {
        // Should really be an assert.

        return;                                                       // RETURN
    }
    HandleEvents *regEvents = &it->second;

    // Reset callbacks.

    if (btlso::EventType::e_READ == event
     || btlso::EventType::e_ACCEPT == event) {

        if (!(regEvents->d_readCallback
         && event == regEvents->d_readEventType)) {
            return ;                                                  // RETURN
        }
        regEvents->d_readCallback = btlso::EventManager::Callback();
    }
    else {
        if (!(regEvents->d_writeCallback
         && event == regEvents->d_writeEventType)) {
            return;                                                   // RETURN
        }
        regEvents->d_writeCallback = btlso::EventManager::Callback();
    }
    --d_numEvents;

    int pollEvent = translateEventToMask(event);
    BSLS_ASSERT(regEvents->d_mask & pollEvent);

    // Clear the corresponding event bit to get the new event mask.

    const int newMask = (regEvents->d_mask ^ pollEvent);
    if (0 == newMask) {
        // There is no more event to monitor for this handle.  Remove it from
        // epoll.

        struct epoll_event epollEvent = { 0, { 0 } };
        int ret = epoll_ctl(d_epollFd, EPOLL_CTL_DEL, handle, &epollEvent);

        // epoll removes closed file descriptors automatically.

        BSLS_ASSERT(0 == ret || ENOENT == errno || EBADF == errno);

        if (d_isInvokingCb) {
            // This method has been invoked in a user-callback from
            // 'dispatchCallbacks'.  We can't remove the HandleEvent from
            // d_events, otherwise it would potentially invalidate the data
            // member of an epoll_event in 'd_signaled'.  We'll just add it to
            // 'd_entriesBeingRemoved' and 'dispatchCallbacks' will clean it up
            // when it is done.  For now, we'll just mark this as invalid and
            // reset the state of the event.

            it->second.d_isValid = false;
            it->second.d_mask = 0;
            d_entriesBeingRemoved.push_back(it);
        }
        else {
            d_events.erase(it);
        }
        return;                                                       // RETURN
    }

    // We're still interested in another event for this fd.  Let's just remove
    // 'event' from the set we're monitoring with epoll.

    regEvents->d_mask = newMask;

    struct epoll_event epollEvent = { 0, { 0 } };
    epollEvent.events = newMask;
    epollEvent.data.ptr = (void *) &*it;

    int ret = epoll_ctl(d_epollFd, EPOLL_CTL_MOD, handle, &epollEvent);
    BSLS_ASSERT(0 == ret || ENOENT == errno || EBADF == errno);
}

int EventManagerName::deregisterSocket(
                                     const btlso::SocketHandle::Handle& handle)
{
    EventMap::iterator it = d_events.find(handle);
    if (d_events.end() == it || !it->second.d_isValid) {
        return 0;                                                     // RETURN
    }

    int numEvents = it->second.d_readCallback ? 1 : 0;
    numEvents += it->second.d_writeCallback ? 1 : 0;
    BSLS_ASSERT(numEvents);

    struct epoll_event epollEvent = { 0, { 0 } };
    int ret = epoll_ctl(d_epollFd, EPOLL_CTL_DEL, handle, &epollEvent);

    // epoll removes closed file descriptors automatically.

    BSLS_ASSERT(0 == ret || ENOENT == errno || EBADF == errno);

    if (d_isInvokingCb) {
        // This method has been invoked in a user-callback from
        // 'dispatchCallbacks'.  We can't remove the HandleEvent from d_events,
        // otherwise it would potentially invalidate the data member of an
        // epoll_event in 'd_signaled'.  We'll just add it to
        // 'd_entriesBeingRemoved' and 'dispatchCallbacks' will clean it up
        // when it is done.  For now, we'll just mark this as invalid and reset
        // the state of the event.

        it->second.d_isValid = false;
        it->second.d_mask = 0;
        it->second.d_writeCallback = btlso::EventManager::Callback();
        it->second.d_readCallback = btlso::EventManager::Callback();
        d_entriesBeingRemoved.push_back(it);
    }
    else {
        d_events.erase(it);
    }
    d_numEvents -= numEvents;

    return numEvents;
}

int EventManagerName::dispatch(const bsls::TimeInterval& timeout,
                               int                       flags)
{
    if (0 == numEvents()) {
        int dummy;
        return sleep(&dummy, timeout, flags, d_timeMetric_p);         // RETURN
    }
    return dispatchImp(flags, &timeout);
}

int EventManagerName::dispatch(int flags)
{
    if (0 == numEvents()) {
        return 0;                                                     // RETURN
    }
    return dispatchImp(flags, 0);
}

int EventManagerName::registerSocketEvent(
                                 const btlso::SocketHandle::Handle&   handle,
                                 const btlso::EventType::Type         event,
                                 const btlso::EventManager::Callback& callback)
{
    EventMap::iterator it = d_events.find(handle);

    if (d_events.end() == it) {
        bsl::pair<EventMap::iterator, bool> ret = d_events.insert(
                                       bsl::make_pair(handle, HandleEvents()));
        BSLS_ASSERT(ret.second);
        it = ret.first;

        // Set initial state.

        it->second.d_mask = 0;
        it->second.d_isValid = true;
    }

    // Register the callback and event type.

    HandleEvents                  *regEvents = &it->second;
    btlso::EventManager::Callback *modifiedCallback = 0;

    if (btlso::EventType::e_READ == event
     || btlso::EventType::e_ACCEPT == event) {

        BSLS_ASSERT(!it->second.d_isValid
                 || !regEvents->d_readCallback
                 || event == regEvents->d_readEventType);

        regEvents->d_readCallback  = callback;
        regEvents->d_readEventType = event;
        modifiedCallback = &regEvents->d_readCallback;
    }
    else {
        BSLS_ASSERT(!it->second.d_isValid
                 || !regEvents->d_writeCallback
                 || event == regEvents->d_writeEventType);

        regEvents->d_writeCallback  = callback;
        regEvents->d_writeEventType = event;
        modifiedCallback = &regEvents->d_writeCallback;
    }

    const int newMask = regEvents->d_mask | translateEventToMask(event);
    BSLS_ASSERT(0 != newMask);
    bool wasRevalidated = false;
    if (!it->second.d_isValid) {
        // We are being called from an user-specified callback in
        // 'dispatchCallbacks'.  This handle was deregistered during a
        // previous callback and is being registered again.  We have to
        // revalidate it.

        it->second.d_isValid = true;
        wasRevalidated = true;
        BSLS_ASSERT(0 == it->second.d_mask);
        BSLS_ASSERT(d_isInvokingCb);
    }
    else if (newMask == regEvents->d_mask) {
        // We just updated the callback.

        return 0;                                                     // RETURN
    }
    ++d_numEvents;

    // Assert that if two events are registered at the same, they can
    // only READ and WRITE.

    BSLS_ASSERT(0 == (newMask & (newMask - 1)) // only 1 bit set
             || (btlso::EventType::e_READ == regEvents->d_readEventType
              && btlso::EventType::e_WRITE == regEvents->d_writeEventType));

    struct epoll_event epollEvent = { 0, { 0 } };
    epollEvent.events = newMask;
    epollEvent.data.ptr = (void *) &*it;

    int epollCmd = EPOLL_CTL_MOD;
    if (0 == regEvents->d_mask) {
        // This socket handle is not registered with epoll.

        epollCmd = EPOLL_CTL_ADD;
    }
    const int oldMask = regEvents->d_mask;
    regEvents->d_mask = newMask;

    const int ret = epoll_ctl(d_epollFd, epollCmd, handle, &epollEvent);
    BSLS_ASSERT(0 == ret || ENOENT == errno || EBADF == errno);

    if (0 == ret) {

        if (wasRevalidated) {
            // We successfully re-registered an entry of this socket handle
            // that is still in 'd_entriesBeingRemoved'.  Remove it from
            // 'd_entriesBeingRemoved'.

            BSLS_ASSERT(d_isInvokingCb);
            bsl::vector<EventMap::iterator>::iterator v_it;
            for (v_it  = d_entriesBeingRemoved.begin();
                 v_it != d_entriesBeingRemoved.end()  ; ++v_it) {

                if (*v_it == it) {
                    d_entriesBeingRemoved.erase(v_it);
                    break;
                }
            }
        }
        return 0;                                                     // RETURN
    }
    regEvents->d_mask = oldMask;
    *modifiedCallback = btlso::EventManager::Callback();
    --d_numEvents;
    if (oldMask) {
        // There are other events registered at this point.  Leave the entry.

        return -1;                                                    // RETURN
    }

    if (wasRevalidated) {
        BSLS_ASSERT(d_isInvokingCb);
        it->second.d_isValid = false;
        return -2;                                                    // RETURN
    }

    // This was a completely new entry, nobody could have seen it.  We can
    // simply remove it.

    d_events.erase(it);
    return -3;
}

// ACCESSORS
int EventManagerName::numSocketEvents(
                               const btlso::SocketHandle::Handle& handle) const
{
    EventMap::const_iterator it = d_events.find(handle);
    if (d_events.end() == it || !it->second.d_isValid) {
        return 0;                                                     // RETURN
    }
    const int numEvents = it->second.d_readCallback ? 1 : 0;
    return numEvents + (it->second.d_writeCallback ? 1 : 0);
}

int EventManagerName::numEvents() const
{
    return d_numEvents;
}

int EventManagerName::isRegistered(
                                const btlso::SocketHandle::Handle& handle,
                                const btlso::EventType::Type       event) const
{
    EventMap::const_iterator it = d_events.find(handle);

    if (d_events.end() == it || !it->second.d_isValid) {
        return 0;                                                     // RETURN
    }

    const HandleEvents& regEvents = it->second;

    if (regEvents.d_readCallback
     && event == regEvents.d_readEventType) {
        return 1;                                                     // RETURN
    }

    if (regEvents.d_writeCallback
     && event == regEvents.d_writeEventType) {
        return 1;                                                     // RETURN
    }

    return 0;
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
