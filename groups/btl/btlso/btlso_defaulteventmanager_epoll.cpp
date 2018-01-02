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

#include <btlso_eventmanagertester.h>   // for testing only
#include <btlso_flags.h>
#include <btlso_socketimputil.h>
#include <btlso_socketoptutil.h>        // for testing only
#include <btlso_timemetrics.h>

#include <bdlb_bitutil.h>
#include <bdlb_bitmaskutil.h>
#include <bdlf_bind.h>
#include <bdlt_currenttime.h>

#include <bsls_timeinterval.h>
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

            if (flags & btlso::Flags::k_ASYNC_INTERRUPT) {
                // We're allowing async interrupts.

                return -1;                                            // RETURN
            }
        }
        now = bdlt::CurrentTime::now();
    }
    return 0;
}

const uint32_t k_POLLIN_EVENTS =
    (1u << EventType::e_READ)  | (1u << EventType::e_ACCEPT);
const uint32_t k_POLLOUT_EVENTS =
    (1u << EventType::e_WRITE) | (1u << EventType::e_CONNECT);

static
struct epoll_event makeEvent(uint32_t eventMask, int fd)
{
    // Return an epoll_event structure with a mask having EPOLLIN set if READ
    // or ACCEPT is set in 'eventMask', and EPOLLOUT set if WRITE or CONNECT
    // is set in 'eventMask'. Assert that if multiple events are registered,
    // they are READ and WRITE.
    //
    // The 'data' portion of the event is a 64-bit-wide union.  One of its
    // fields is "fd", an int, but we also want to store the original
    // 'eventMask'.  Store the fd in the lower 32 bits of the 'data' union
    // and the 'eventMask' in the upper 32 bits.  (Thus, 'data.fd' does *not*
    // contain the fd.)

    int pollinEvents =
        bdlb::BitUtil::numBitsSet(eventMask & k_POLLIN_EVENTS);
    int polloutEvents =
        bdlb::BitUtil::numBitsSet(eventMask & k_POLLOUT_EVENTS);
    BSLS_ASSERT(2 == bdlb::BitUtil::numBitsSet(k_POLLIN_EVENTS));
    BSLS_ASSERT(2 == bdlb::BitUtil::numBitsSet(k_POLLOUT_EVENTS));
    BSLS_ASSERT(2 > pollinEvents);
    BSLS_ASSERT(2 > polloutEvents);
    BSLS_ASSERT(!(pollinEvents && polloutEvents) ||
                (eventMask & bdlb::BitMaskUtil::eq(EventType::e_READ) &&
                 eventMask & bdlb::BitMaskUtil::eq(EventType::e_WRITE)));

    struct epoll_event result;
    result.events = (EPOLLIN * pollinEvents) | (EPOLLOUT * polloutEvents);
    result.data.u64 = (uint64_t)eventMask << 32 | fd;
    return result;
}

struct RemoveVisitor {
    int d_epollFd;

    RemoveVisitor(int epollFd)
    : d_epollFd(epollFd)
    {
    }

    void operator()(const SocketHandle::Handle& handle) {
        struct epoll_event event = { 0, { 0 } };
        int ret = epoll_ctl(d_epollFd, EPOLL_CTL_DEL, handle, &event);
        // epoll removes closed file descriptors automatically.

        BSLS_ASSERT(0 == ret || ENOENT == errno || EBADF == errno);
    }
};

}  // close unnamed namespace

           // ------------------------------------------
           // class DefaultEventManager<Platform::EPOLL>
           // ------------------------------------------

typedef btlso::DefaultEventManager<btlso::Platform::EPOLL> EventManagerName;
    // Alias for brevity.

// PRIVATE MANIPULATORS
int EventManagerName::dispatchCallbacks(int numReady)
{
    int numCallbacks = 0;

    for (int i = 0; i < numReady; ++i) {
        const struct ::epoll_event *curEvent = &d_signaled[i];

        BSLS_ASSERT(curEvent);
        BSLS_ASSERT(curEvent->events);

        uint32_t eventMask = (uint32_t)(curEvent->data.u64 >> 32);
        int fd = (int)(curEvent->data.u64 & 0xFFFFFFFF);

        // Read/Accept.

        if (curEvent->events & (EPOLLIN | EPOLLERR | EPOLLHUP)) {
            if (eventMask & bdlb::BitMaskUtil::eq(EventType::e_READ)) {
                numCallbacks += !d_callbacks.invoke(Event(fd,
                                                          EventType::e_READ));
            } else {
                numCallbacks += !d_callbacks.invoke(Event(fd,
                                                          EventType::e_ACCEPT));
            }
        }

        // Write/Connect.

        if (curEvent->events & (EPOLLOUT | EPOLLERR | EPOLLHUP)) {
            if (eventMask & bdlb::BitMaskUtil::eq(EventType::e_WRITE)) {
                numCallbacks += !d_callbacks.invoke(Event(fd,
                                                          EventType::e_WRITE));
            } else {
                numCallbacks += !d_callbacks.invoke(
                                               Event(fd, EventType::e_CONNECT));
            }
        }
    }

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
                              (0 != (btlso::Flags::k_ASYNC_INTERRUPT & flags));

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

            d_signaled.resize(d_callbacks.numSockets());
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
                                      static_cast<int>(d_signaled.size()),
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

        BSLS_ASSERT(numReady <= static_cast<int>(d_signaled.size()));
        numCallbacks += dispatchCallbacks(numReady);
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
, d_timeMetric_p(timeMetric)
, d_callbacks(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
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
    (void)rc; BSLS_ASSERT(0 == rc);
}

// MANIPULATORS
void EventManagerName::deregisterAll()
{
    RemoveVisitor visitor(d_epollFd);
    d_callbacks.visitSockets(&visitor);
    d_callbacks.removeAll();
}

void EventManagerName::deregisterSocketEvent(
                                     const btlso::SocketHandle::Handle& handle,
                                     btlso::EventType::Type             event)
{
    Event handleEvent(handle, event);

    if (!d_callbacks.remove(handleEvent)) {
        return;                                                       // RETURN
    }

    uint32_t newMask = d_callbacks.getRegisteredEventMask(handle);
    if (0 == newMask) {
        // There are no more events to monitor for this handle.  Remove it from
        // epoll.

        struct epoll_event epollEvent = { 0, { 0 } };
        int ret = epoll_ctl(d_epollFd, EPOLL_CTL_DEL, handle, &epollEvent);

        // epoll removes closed file descriptors automatically.

        (void) ret; BSLS_ASSERT(0 == ret || ENOENT == errno || EBADF == errno);
        return;                                                       // RETURN
    }

    // We're still interested in another event for this fd.  Send the new mask
    // to epoll.
    struct epoll_event epollEvent = makeEvent(newMask, handle);

    int ret = epoll_ctl(d_epollFd, EPOLL_CTL_MOD, handle, &epollEvent);
    (void)ret; BSLS_ASSERT(0 == ret || ENOENT == errno || EBADF == errno);
}

int EventManagerName::deregisterSocket(
                                     const btlso::SocketHandle::Handle& handle)
{
    int numEvents = d_callbacks.removeSocket(handle);
    struct epoll_event epollEvent = { 0, { 0 } };
    int ret = epoll_ctl(d_epollFd, EPOLL_CTL_DEL, handle, &epollEvent);

    // epoll removes closed file descriptors automatically.

    (void)ret; BSLS_ASSERT(0 == ret || ENOENT == errno || EBADF == errno);

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
    Event handleEvent(handle, event);

    uint32_t eventMask = d_callbacks.registerCallback(handleEvent, callback);
    if (0 == eventMask) {
        // Event was already registered; we simply changed the callback
        return 0;                                                     // RETURN
    }

    // Otherwise, get the new epoll mask for the socket.
    struct epoll_event epollEvent = makeEvent(eventMask, handle);

    // If there is 1 event registered now, it was the first one for this socket
    // (otherwise, we would have replaced the callback and returned already).
    // In that case, the command is EPOLL_CTL_ADD. Otherwise, we are
    // adding a second event and the command is EPOLL_CTL_MOD.
    int epollCmd = 1 == bdlb::BitUtil::numBitsSet(eventMask)
        ? EPOLL_CTL_ADD
        : EPOLL_CTL_MOD;

    const int ret = epoll_ctl(d_epollFd, epollCmd, handle, &epollEvent);
    BSLS_ASSERT(0 == ret || ENOENT == errno || EBADF == errno);

    if (0 == ret) {
        return 0;                                                     // RETURN
    }

    // Otherwise, there's been an error; deregister the event.
    d_callbacks.remove(handleEvent);
    return -1;
}

// ACCESSORS
int EventManagerName::numSocketEvents(
                               const btlso::SocketHandle::Handle& handle) const
{
    return bdlb::BitUtil::numBitsSet(
                                  d_callbacks.getRegisteredEventMask(handle));
}

int EventManagerName::numEvents() const
{
    return d_callbacks.numCallbacks();
}

int EventManagerName::isRegistered(
                                const btlso::SocketHandle::Handle& handle,
                                const btlso::EventType::Type       event) const
{
    return d_callbacks.contains(Event(handle, event));
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
