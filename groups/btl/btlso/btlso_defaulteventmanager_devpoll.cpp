// btlso_defaulteventmanager_devpoll.cpp                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_defaulteventmanager_devpoll.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_defaulteventmanager_devpoll_cpp,"$Id$ $CSID$")

#include <btlso_timemetrics.h>
#include <btlso_flags.h>

#include <bsls_timeinterval.h>
#include <bdlb_bitmaskutil.h>
#include <bdlb_bitutil.h>
#include <bdlt_currenttime.h>
#include <bsls_assert.h>

#if defined(BSLS_PLATFORM_OS_SOLARIS) || defined(BSLS_PLATFORM_OS_HPUX)
#include <sys/devpoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <bsl_algorithm.h>
#include <bsl_c_errno.h>
#include <bsl_cstring.h>
#include <bsl_limits.h>
#include <bsl_utility.h>

namespace BloombergLP {

namespace btlso {

namespace {
    // unnamed namespace for private resources

enum {
    MIN_IOCTL_TIMEOUT_MS = 333  // force spinning of devpoll every 333ms,
                                // otherwise a bug on Solaris may cause
                                // missing events if passing a longer timeout
                                // to ioctl(fd, DP_POLL, ...)
};

const int k_POLLFD_SIZE = static_cast<int>(sizeof(struct ::pollfd));

// This object is used to initialize and grow the working arrays passed to
// ioctl.  This is not strictly necessary but prevents frequent warnings about
// uninitialized memory reads in Purify and similar tools without meaningful
// cost.
static struct ::pollfd DEFAULT_POLLFD; // initialized to 0 as a static

struct PollRemoveVisitor {
    // Visit a set of sockets and populate an array of pollfd to deregister
    // those sockets.

    struct ::pollfd *d_pollfdArray;
    int              d_index;

    PollRemoveVisitor(struct ::pollfd *pollfdArray) {
        d_pollfdArray = pollfdArray;
        d_index = -1;
    }

    void operator()(int fd) {
        int i = ++d_index;
        d_pollfdArray[i].fd = fd;
        d_pollfdArray[i].events = POLLREMOVE;
        d_pollfdArray[i].revents = 0;
    }
};

static
short convertMask(uint32_t eventMask)
// Return a mask with POLLIN set if READ or ACCEPT is set in 'eventMask',
// and with POLLOUT set if WRITE or CONNECT is set in 'eventMask'.
// Assert that if multiple events are registered, they are READ and WRITE.
{

    int pollinEvents =
        bdlb::BitUtil::numBitsSet(eventMask & EventType::k_INBOUND_EVENTS);
    int polloutEvents =
        bdlb::BitUtil::numBitsSet(eventMask & EventType::k_OUTBOUND_EVENTS);
    BSLS_ASSERT(2 > pollinEvents);
    BSLS_ASSERT(2 > polloutEvents);
    BSLS_ASSERT(!(pollinEvents && polloutEvents) ||
                (eventMask & bdlb::BitMaskUtil::eq(EventType::e_READ) &&
                 eventMask & bdlb::BitMaskUtil::eq(EventType::e_WRITE)));

    return static_cast<short>(  (POLLIN  * static_cast<short>(pollinEvents))
                              | (POLLOUT * static_cast<short>(polloutEvents)));
}

static
inline int dispatchCallbacks(bsl::vector<struct ::pollfd>&  signaled,
                             int                            rfds,
                             EventCallbackRegistry         *callbacks)
{
    int numCallbacks = 0;

    for (int i = 0; i < rfds && i < static_cast<int>(signaled.size()); ++i) {
        const struct ::pollfd& currData = signaled[i];

        BSLS_ASSERT(currData.revents);

        int eventMask = callbacks->getRegisteredEventMask(currData.fd);

        // Read/Accept.

        if (currData.revents & POLLIN) {
            if (eventMask & bdlb::BitMaskUtil::eq(EventType::e_READ)) {
                numCallbacks += !callbacks->invoke(Event(currData.fd,
                                                         EventType::e_READ));
            } else {
                numCallbacks += !callbacks->invoke(Event(currData.fd,
                                                         EventType::e_ACCEPT));
            }
        }

        // Write/Connect.

        if (currData.revents & POLLOUT) {
            if (eventMask & bdlb::BitMaskUtil::eq(EventType::e_WRITE)) {
                numCallbacks += !callbacks->invoke(Event(currData.fd,
                                                         EventType::e_WRITE));
            } else {
                numCallbacks += !callbacks->invoke(Event(currData.fd,
                                                         EventType::e_CONNECT));
            }
        }
    }
    return numCallbacks;
}

} // close unnamed namespace

         // --------------------------------------------
         // class DefaultEventManager<Platform::DEVPOLL>
         // --------------------------------------------

// CREATORS
DefaultEventManager<Platform::DEVPOLL>::DefaultEventManager(
                                              TimeMetrics      *timeMetric,
                                              bslma::Allocator *basicAllocator)
: d_callbacks(basicAllocator)
, d_timeMetric_p(timeMetric)
, d_signaled(basicAllocator)
, d_dpFd(open("/dev/poll", O_RDWR))
{

}

DefaultEventManager<Platform::DEVPOLL>::~DefaultEventManager()
{
    int rc = close(d_dpFd);
    BSLS_ASSERT(0 == rc);
}

// MANIPULATORS
int DefaultEventManager<Platform::DEVPOLL>::dispatch(
                                             const bsls::TimeInterval& timeout,
                                             int                       flags)
{
    bsls::TimeInterval now(bdlt::CurrentTime::now());

    if (!numEvents()) {
        if (timeout <= now) {
            return 0;                                                 // RETURN
        }
        while (timeout > now) {
            bsls::TimeInterval currTimeout(timeout - now);
            struct timespec    ts;
            ts.tv_sec = static_cast<time_t>(
                bsl::min(static_cast<bsls::Types::Int64>(
                             bsl::numeric_limits<time_t>::max()),
                         currTimeout.seconds()));
            ts.tv_nsec = currTimeout.nanoseconds();

            // Sleep till it's time.

            int savedErrno;
            int rc;
            if (d_timeMetric_p) {
                d_timeMetric_p->switchTo(TimeMetrics::e_IO_BOUND);
                rc = nanosleep(&ts, 0);
                savedErrno = errno;
                d_timeMetric_p->switchTo(TimeMetrics::e_CPU_BOUND);
            }
            else {
                rc = nanosleep(&ts, 0);
                savedErrno = errno;
            }

            errno = 0;
            if (0 > rc) {
                if (EINTR == savedErrno) {
                    if (flags & btlso::Flags::k_ASYNC_INTERRUPT) {
                        return -1;                                    // RETURN
                    }
                }
                else {
                    return -2;                                        // RETURN
                }
            }
            now = bdlt::CurrentTime::now();
        }
        return 0;                                                     // RETURN
    }

    int ncbs = 0;                    // number of callbacks dispatched

    do {
        int rfds;                    // number of returned sockets
        int savedErrno = 0;          // saved errno value set by poll
        do {
            d_signaled.resize(d_callbacks.numSockets(), DEFAULT_POLLFD);

            struct dvpoll dopoll;
            dopoll.dp_nfds = d_signaled.size();
            dopoll.dp_fds  = &d_signaled.front();

            if (timeout < now) {
                // The ioctl() call should return immediately.

                dopoll.dp_timeout = 0;
            }
            else {
                // Calculate the time remaining for the ioctl() call.

                bsls::TimeInterval curr_timeout(timeout - now);

                // Convert this timeout to a 32 bit value in milliseconds.

                dopoll.dp_timeout = static_cast<int>(
                    bsl::min(static_cast<bsls::Types::Int64>(
                                 bsl::numeric_limits<int>::max()),
                             curr_timeout.seconds() * 1000 +
                                 curr_timeout.nanoseconds() / 1000000 + 1));
            }
            dopoll.dp_timeout = bsl::min(
                                       dopoll.dp_timeout,
                                       static_cast<int>(MIN_IOCTL_TIMEOUT_MS));

            if (d_timeMetric_p) {
                d_timeMetric_p->switchTo(TimeMetrics::e_IO_BOUND);
                rfds = ioctl(d_dpFd, DP_POLL, &dopoll);
                savedErrno = errno;
                d_timeMetric_p->switchTo(TimeMetrics::e_CPU_BOUND);
            }
            else {
                rfds = ioctl(d_dpFd, DP_POLL, &dopoll);
                savedErrno = errno;
            }
            errno = 0;
            now = bdlt::CurrentTime::now();
        } while ((0 > rfds && EINTR == savedErrno)
              && !(btlso::Flags::k_ASYNC_INTERRUPT & flags)
              && now < timeout);

        if (0 >= rfds) {
            return rfds
                   ? -1 == rfds && EINTR == savedErrno
                     ? -1
                     : -2
                   : 0;                                               // RETURN
        }

        ncbs += dispatchCallbacks(d_signaled, rfds, &d_callbacks);
        now = bdlt::CurrentTime::now();
    } while (0 == ncbs && now < timeout);

    return ncbs;
}

int DefaultEventManager<Platform::DEVPOLL>::dispatch(int flags)
{
    if (!numEvents()) {
        return 0;                                                     // RETURN
    }

    int ncbs = 0;                    // number of callbacks dispatched
    while (0 == ncbs) {
        int rfds;                    // number of returned fds
        int savedErrno = 0;          // saved errno value set by 'poll'

        d_signaled.resize(d_callbacks.numSockets(), DEFAULT_POLLFD);

        struct dvpoll dopoll;
        dopoll.dp_nfds    = d_signaled.size();
        dopoll.dp_fds     = &d_signaled.front();
        dopoll.dp_timeout = MIN_IOCTL_TIMEOUT_MS;

        do {
            if (d_timeMetric_p) {
                d_timeMetric_p->switchTo(TimeMetrics::e_IO_BOUND);
                rfds = ioctl(d_dpFd, DP_POLL, &dopoll);
                savedErrno = errno;
                d_timeMetric_p->switchTo(TimeMetrics::e_CPU_BOUND);
            }
            else {
                rfds = ioctl(d_dpFd, DP_POLL, &dopoll);
                savedErrno = errno;
            }
            errno = 0;
        } while ((0 > rfds && EINTR == savedErrno)
              && !(btlso::Flags::k_ASYNC_INTERRUPT & flags));

        if (0 >= rfds) {
            return rfds
                   ? -1 == rfds && EINTR == savedErrno
                     ? -1
                     : -2
                   : 0;                                               // RETURN
        }

        ncbs += dispatchCallbacks(d_signaled, rfds, &d_callbacks);
    }
    return ncbs;
}

int DefaultEventManager<Platform::DEVPOLL>::registerSocketEvent(
                                        const SocketHandle::Handle&   handle,
                                        const EventType::Type         event,
                                        const EventManager::Callback& callback)
{
    Event handleEvent(handle, event);

    uint32_t newMask = d_callbacks.registerCallback(handleEvent, callback);
    if (0 == newMask) {
        // We replaced an existing callback function.
        return 0;                                                     // RETURN
    }

    // Prepare a ::pollfd object to write to /dev/poll

    struct ::pollfd pfd;
    pfd.fd = handle;
    pfd.revents = 0;      // just to satisfy purify

    // Calculate the new event mask
    pfd.events = convertMask(newMask);

    // Write the new event mask for this fd to /dev/poll.
    ssize_t rc = write(d_dpFd, &pfd, k_POLLFD_SIZE);
    if (k_POLLFD_SIZE != rc) {
        // Unregister the event we added earlier.
        d_callbacks.remove(handleEvent);
        return errno;                                                 // RETURN
    }

    return 0;
}

void DefaultEventManager<Platform::DEVPOLL>::deregisterSocketEvent(
                                            const SocketHandle::Handle& handle,
                                            EventType::Type             event)
{
    Event handleEvent(handle, event);

    bool removed = d_callbacks.remove(handleEvent);
    BSLS_ASSERT(removed);

    // Retrieve the new event mask
    int eventmask = convertMask(d_callbacks.getRegisteredEventMask(handle));

    // Prepare a ::pollfd object to write to /dev/poll
    // First, we need to remove this socket handle from the set.
    // The write it out with a new mask, if applicable.

    struct ::pollfd pfd;
    pfd.fd      = handle;
    pfd.events  = POLLREMOVE;
    pfd.revents = 0;      // just to satisfy purify

    ssize_t rc = write(d_dpFd, &pfd, k_POLLFD_SIZE);
    BSLS_ASSERT(k_POLLFD_SIZE == rc);

    if (eventmask) {
        // Write the new event mask for this fd to /dev/poll.

        pfd.events = static_cast<short>(eventmask);
        ssize_t rc = write(d_dpFd, &pfd, k_POLLFD_SIZE);
        BSLS_ASSERT(k_POLLFD_SIZE == rc);
    }
}

int DefaultEventManager<Platform::DEVPOLL>::deregisterSocket(
                                            const SocketHandle::Handle& handle)
{
    int eventmask = d_callbacks.getRegisteredEventMask(handle);
    if (0 == eventmask) {
        // No registered events, nothing to do.
        return 0;                                                     // RETURN
    }

    struct ::pollfd req;
    req.fd      = handle;
    req.events  = POLLREMOVE;
    req.revents = 0;

    ssize_t rc = ::write(d_dpFd, &req, k_POLLFD_SIZE);
    BSLS_ASSERT(k_POLLFD_SIZE == rc);

    return d_callbacks.removeSocket(handle);
}

void DefaultEventManager<Platform::DEVPOLL>::deregisterAll()
{
    bsl::vector<struct ::pollfd> removed(d_callbacks.numSockets(),
                                         DEFAULT_POLLFD);

    if (!removed.empty()) {
        PollRemoveVisitor visitor(&removed.front());
        d_callbacks.visitSockets(&visitor);

        int pollfdSize = removed.size() * k_POLLFD_SIZE;
        ssize_t rc = write(d_dpFd, &removed.front(), pollfdSize);

        BSLS_ASSERT(pollfdSize == rc);
    }

    d_callbacks.removeAll();
}

// ACCESSORS
int DefaultEventManager<Platform::DEVPOLL>::numSocketEvents(
                                      const SocketHandle::Handle& handle) const
{
    return bdlb::BitUtil::numBitsSet(
                                  d_callbacks.getRegisteredEventMask(handle));
}

int DefaultEventManager<Platform::DEVPOLL>::numEvents() const
{
    return d_callbacks.numCallbacks();
}

int DefaultEventManager<Platform::DEVPOLL>::isRegistered(
                                       const SocketHandle::Handle& handle,
                                       const EventType::Type       event) const
{
    return d_callbacks.contains(Event(handle, event));
}

}  // close package namespace

}  // close namespace BloombergLP

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
