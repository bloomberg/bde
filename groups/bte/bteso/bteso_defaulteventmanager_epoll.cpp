// bteso_defaulteventmanager_epoll.cpp    -*-C++-*-
#include <bteso_defaulteventmanager_epoll.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_defaulteventmanager_epoll_cpp,"$Id$ $CSID$")

#if defined(BSLS_PLATFORM__OS_LINUX)

#include <bteso_eventmanagertester.h>           // for testing only
#include <bteso_flag.h>
#include <bteso_socketimputil.h>                // for testing only
#include <bteso_socketoptutil.h>                // for testing only
#include <bteso_timemetrics.h>

#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bsls_assert.h>
#include <bsls_platformutil.h>

#include <bsl_iostream.h>
#include <bsl_utility.h>

#include <bsl_cstdio.h>
#include <bsl_c_errno.h>

namespace BloombergLP {

namespace {

int sleep(int                     *resultErrno,
          const bdet_TimeInterval& timeout,
          int                      flags,
          bteso_TimeMetrics        *metrics)
{
    bdet_TimeInterval now(bdetu_SystemTime::now());
    while (timeout > now) {
        bdet_TimeInterval currTimeout(timeout - now);
        struct timespec ts;
        ts.tv_sec = currTimeout.seconds();
        ts.tv_nsec = currTimeout.nanoseconds();

        // Sleep till it's time.

        int savedErrno;
        int rc;
        if (metrics) {
            metrics->switchTo(bteso_TimeMetrics::BTESO_IO_BOUND);
            rc = nanosleep(&ts, 0);
            savedErrno = errno;
            metrics->switchTo(bteso_TimeMetrics::BTESO_CPU_BOUND);
        }
        else {
            rc = nanosleep(&ts, 0);
            savedErrno = errno;
        }

        errno = 0;
        *resultErrno = savedErrno;
        if (0 > rc) {
            BSLS_ASSERT(savedErrno == EINTR);
            if (flags & bteso_Flag::BTESO_ASYNC_INTERRUPT) {
                // We're allowing async interrupts.

                return -1;
            }
        }
        now = bdetu_SystemTime::now();
    }
    return 0;
}

int translateEventToMask(bteso_EventType::Type event)
{
    switch (event) {
      case bteso_EventType::BTESO_ACCEPT:
      case bteso_EventType::BTESO_READ:
        return EPOLLIN;
      case bteso_EventType::BTESO_CONNECT:
      case bteso_EventType::BTESO_WRITE:
        return EPOLLOUT;
      default:
        BSLS_ASSERT("Invalid event (must be unreachable)" && 0);
        return -1;
    }
}

} // close unnamed namespace

           // ------------------------------------------------------
           // class bteso_DefaultEventManager<bteso_Platform::EPOLL>
           // ------------------------------------------------------

typedef bteso_DefaultEventManager<bteso_Platform::EPOLL> EventManagerName;
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
                                    d_entriesBeingRemoved.end() != it; ++it) {
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

int EventManagerName::dispatchImp(int                      flags,
                                  const bdet_TimeInterval *timeout)
{
    bdet_TimeInterval now;
    if (timeout) {
        now = bdetu_SystemTime::now();
    }
    int numCallbacks = 0;                    // number of callbacks dispatched
    const bool allowAsyncInterrupts =
                            (0 != (bteso_Flag::BTESO_ASYNC_INTERRUPT & flags));
    do {
        int numReady;                    // number of returned sockets
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

                    bdet_TimeInterval curr_timeout(*timeout - now);
                    bsls_PlatformUtil::Int64 totalMs =
                                             curr_timeout.totalMilliseconds();
                    BSLS_ASSERT(totalMs < INT_MAX);
                    epollTimeout = totalMs + 1; // totalMs is rounded down
                }
            }

            d_signaled.resize(d_events.size());
            if (d_signaled.empty()) {
                // No fds to wait for.  We'll just sleep if there is a timeout.

                if (!timeout || 0 == epollTimeout) {
                    numReady = 0;
                    // XXX not sure if we should return -2 here.
                    break;
                }
                numReady = sleep(&savedErrno, *timeout, flags, d_timeMetric_p);
            }
            else {
                if (d_timeMetric_p) {
                    d_timeMetric_p->switchTo(
                                            bteso_TimeMetrics::BTESO_IO_BOUND);
                }
                numReady = epoll_wait(d_epollFd, &d_signaled.front(),
                                      d_signaled.size(), epollTimeout);
                BSLS_ASSERT(-1 != numReady || EINTR == errno);
                savedErrno = errno;
                if (d_timeMetric_p) {
                    d_timeMetric_p->switchTo(
                                           bteso_TimeMetrics::BTESO_CPU_BOUND);
                }
            }
            errno = 0;
            if (numReady > 0 ||
               (numReady < 0 && EINTR == savedErrno && allowAsyncInterrupts)) {
                // Either a fd is ready or we've been interrupted and the user
                // wants to know.

                break;
            }
            if (timeout) {
                now = bdetu_SystemTime::now();
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
                   : 0;
        }
        numCallbacks += dispatchCallbacks(d_signaled, numReady);
        if (timeout) {
            now = bdetu_SystemTime::now();
        }
    } while (0 == numCallbacks && (0 == timeout || now < *timeout));

    return numCallbacks;
}

// PUBLIC CLASS METHODS
bool
EventManagerName::isSupported()
{
    int fd = epoll_create(128);
    if (-1 == fd) {
        return false;
    }
    close(fd);
    return true;
}

// CREATORS
EventManagerName::bteso_DefaultEventManager(bteso_TimeMetrics *timeMetric,
                                            bslma_Allocator   *basicAllocator)
: d_epollFd(-1)
, d_signaled(basicAllocator)
, d_isInvokingCb(false)
, d_timeMetric_p(timeMetric)
, d_events(128, basicAllocator)
, d_entriesBeingRemoved(basicAllocator)
, d_numEvents(0)
{
    d_epollFd = epoll_create(128);
    if (-1 == d_epollFd) {
        bsl::perror("epoll_create returned ");
        BSLS_ASSERT_OPT("epoll_create() failed" && 0);
    }
}

EventManagerName::~bteso_DefaultEventManager()
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
                                                  d_events.end() != it; ++it) {
        if (!it->second.d_isValid) {
            continue;
        }
        struct epoll_event event = {0,{0}};
        int ret = epoll_ctl(d_epollFd, EPOLL_CTL_DEL, it->first, &event);

        // epoll removes closed file descriptors automatically.

        BSLS_ASSERT(0 == ret || ENOENT == errno || EBADF == errno);

        it->second.d_isValid = false;
        it->second.d_mask = 0;
        it->second.d_writeCallback = bteso_EventManager::Callback();
        it->second.d_readCallback = bteso_EventManager::Callback();
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
                                      const bteso_SocketHandle::Handle& handle,
                                      bteso_EventType::Type             event)
{
    EventMap::iterator it = d_events.find(handle);

    // TBD
    // The asserts commented below should be there but right now it is
    // difficult to use this manager through btemt_TcpTimerEventManager if you
    // do not keep the socket state very carefully.  Indeed, when you call
    // registerSocketEvent on btemt_TcpTimerEventManager, you never know if it
    // succeeded so you might try to deregister the event later.  For this
    // reason, we relaxed 3 asserts.  They should be re-enabled when the
    // btemt_TcpTimerEventManager is fixed.
    //
    //BSLS_ASSERT(d_events.end() != it);

    if (d_events.end() == it || !it->second.d_isValid) {
        // Should really be an assert.

        return;
    }
    HandleEvents *regEvents = &it->second;

    // Reset callbacks.

    if (bteso_EventType::BTESO_READ == event
     || bteso_EventType::BTESO_ACCEPT == event) {
        //BSLS_ASSERT(regEvents->d_readCallback
        //            && event == regEvents->d_readEventType);

        if (!(regEvents->d_readCallback
              && event == regEvents->d_readEventType)) {
            return ;
        }
        regEvents->d_readCallback = bteso_EventManager::Callback();
    }
    else {
        //BSLS_ASSERT(regEvents->d_writeCallback
        //            && event == regEvents->d_writeEventType);

        if (!(regEvents->d_writeCallback
              && event == regEvents->d_writeEventType)) {
            return;
        }
        regEvents->d_writeCallback = bteso_EventManager::Callback();
    }
    --d_numEvents;

    int pollEvent = translateEventToMask(event);
    BSLS_ASSERT(regEvents->d_mask & pollEvent);

    // Clear the corresponding event bit to get the new event mask.

    const int newMask = (regEvents->d_mask ^ pollEvent);
    if (0 == newMask) {
        // There is no more event to monitor for this handle.  Remove it from
        // epoll.

        struct epoll_event epollEvent = {0, {0}};
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
        return;
    }

    // We're still interested in another event for this fd.  Let's just remove
    // 'event' from the set we're monitoring with epoll.

    regEvents->d_mask = newMask;
    struct epoll_event epollEvent;
    epollEvent.events = newMask;
    epollEvent.data.ptr = (void *) &*it;
    int ret = epoll_ctl(d_epollFd, EPOLL_CTL_MOD, handle, &epollEvent);
    BSLS_ASSERT(0 == ret || ENOENT == errno || EBADF == errno);
}

int EventManagerName::deregisterSocket(
                                      const bteso_SocketHandle::Handle& handle)
{
    EventMap::iterator it = d_events.find(handle);
    if (d_events.end() == it || !it->second.d_isValid) {
        return 0;
    }
    int numEvents = it->second.d_readCallback ? 1 : 0;
    numEvents += it->second.d_writeCallback ? 1 : 0;
    BSLS_ASSERT(numEvents);

    struct epoll_event epollEvent = {0,{0}};
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
        it->second.d_writeCallback = bteso_EventManager::Callback();
        it->second.d_readCallback = bteso_EventManager::Callback();
        d_entriesBeingRemoved.push_back(it);
    }
    else {
        d_events.erase(it);
    }
    d_numEvents -= numEvents;

    return numEvents;
}

int EventManagerName::dispatch(const bdet_TimeInterval& timeout,
                               int                      flags)
{
    if (0 == numEvents()) {
        int dummy;
        return sleep(&dummy, timeout, flags, d_timeMetric_p);
    }
    return dispatchImp(flags, &timeout);
}

int EventManagerName::dispatch(int flags)
{
    if (0 == numEvents()) {
        return 0;
    }
    return dispatchImp(flags, 0);
}

int EventManagerName::registerSocketEvent(
                                  const bteso_SocketHandle::Handle&   handle,
                                  const bteso_EventType::Type         event,
                                  const bteso_EventManager::Callback& callback)
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

    HandleEvents *regEvents = &it->second;
    bteso_EventManager::Callback *modifiedCallback = 0;
    if (bteso_EventType::BTESO_READ == event
     || bteso_EventType::BTESO_ACCEPT == event) {
        BSLS_ASSERT(!it->second.d_isValid
                    || !regEvents->d_readCallback
                    || event == regEvents->d_readEventType);
        regEvents->d_readCallback = callback;
        regEvents->d_readEventType = event;
        modifiedCallback = &regEvents->d_readCallback;
    }
    else {
        BSLS_ASSERT(!it->second.d_isValid
                    || !regEvents->d_writeCallback
                    || event == regEvents->d_writeEventType);
        regEvents->d_writeCallback = callback;
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

        return 0;
    }
    ++d_numEvents;

    // Assert that if two events are registered at the same, they can
    // only READ and WRITE.

    BSLS_ASSERT(0 == (newMask & (newMask - 1)) // only 1 bit set
             || (bteso_EventType::BTESO_READ == regEvents->d_readEventType
              && bteso_EventType::BTESO_WRITE == regEvents->d_writeEventType));

    struct epoll_event epollEvent;
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
            // We successfully re-registered an entry of this socakt handle
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
        return 0;
    }
    regEvents->d_mask = oldMask;
    *modifiedCallback = bteso_EventManager::Callback();
    --d_numEvents;
    if (oldMask) {
        // There are other events registered at this point.  Leave the entry.
        return -1;
    }

    if (wasRevalidated) {
        BSLS_ASSERT(d_isInvokingCb);
        it->second.d_isValid = false;
        return -2;
    }

    // This was a completely new entry, nobody could have seen it.  We can
    // simply remove it.

    d_events.erase(it);
    return -3;
}

// ACCESSORS
int EventManagerName::numSocketEvents(
                                const bteso_SocketHandle::Handle& handle) const
{
    EventMap::const_iterator it = d_events.find(handle);
    if (d_events.end() == it || !it->second.d_isValid) {
        return 0;
    }
    const int numEvents = it->second.d_readCallback ? 1 : 0;
    return numEvents + (it->second.d_writeCallback ? 1 : 0);
}

int EventManagerName::numEvents() const
{
    return d_numEvents;
}

int EventManagerName::isRegistered(
                                 const bteso_SocketHandle::Handle& handle,
                                 const bteso_EventType::Type       event) const
{
    EventMap::const_iterator it = d_events.find(handle);
    if (d_events.end() == it || !it->second.d_isValid) {
        return 0;
    }
    const HandleEvents& regEvents = it->second;
    if (regEvents.d_readCallback
     && event == regEvents.d_readEventType) {
        return 1;
    }
    if (regEvents.d_writeCallback
     && event == regEvents.d_writeEventType) {
        return 1;
    }
    return 0;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
