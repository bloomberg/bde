// btlso_defaulteventmanager_pollset.cpp                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_defaulteventmanager_pollset.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_defaulteventmanager_pollset_cpp,"$Id$ $CSID$")

#if defined(BSLS_PLATFORM_OS_AIX)

// The following set of checks mirror those found on AIX in <sys/poll.h>.  When
// we include this file in our headers, we are careful to undefine the 'events'
// and 'revents' macros since they cause unfortunate collisions, but here in
// this file we do want their effect.
#if _XOPEN_SOURCE_EXTENDED == 1 &&                                            \
    defined(_ALL_SOURCE) &&                                                   \
    !defined(__64BIT__) &&                                                    \
    !defined(__HIDE_SVR4_POLLFD_NAMES)
#define events reqevents   // renamed field in struct pollfd
#define revents rtnevents  // renamed field in struct pollfd
#endif

#include <btlso_flags.h>
#include <btlso_socketoptutil.h>
#include <btlso_timemetrics.h>

#include <bslmt_threadutil.h>

#include <bdlt_currenttime.h>
#include <bdlb_bitmaskutil.h>
#include <bdlb_bitutil.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_timeinterval.h>

#include <bsl_vector.h>
#include <bsl_utility.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/pollset.h>
#include <bsl_c_errno.h>

// We use 'int' instead of 'pollset_t' in the header to avoid including
// <sys/pollset.h> there.  Verify that they're the same type.
BSLMF_ASSERT((bsl::is_same<pollset_t, int>::value));

// 'NO_TIMEOUT' and 'INF_TIMEOUT' are defined in <sys/poll.h>

#define BTLSO_EVENTMANAGERIMP_POLL_NO_TIMEOUT  NO_TIMEOUT
#define BTLSO_EVENTMANAGERIMP_POLL_INF_TIMEOUT INF_TIMEOUT

#include <bsl_cstring.h>

namespace BloombergLP {

namespace btlso {

const uint32_t k_POLLIN_EVENTS =
    (1u << EventType::e_READ) | (1u << EventType::e_ACCEPT);

// This struct (initialized to 0 as a static) is used to grow the 'd_signaled'
// working array when needed to avoid UMR-type errors from Purify and similar
// tools.
static struct ::pollfd DEFAULT_POLLFD;

          // --------------------------------------------
          // class DefaultEventManager<Platform::POLLSET>
          // --------------------------------------------

// PRIVATE CLASS METHODS
int DefaultEventManager<Platform::POLLSET>::dispatchCallbacks(
                                                         int numSignaled) const
{
    BSLS_ASSERT(numSignaled <= d_signaled.size());

    int numCallbacks = 0;

    for (int i = 0; i < numSignaled; ++i) {
        const struct ::pollfd& currData = d_signaled[i];

        int eventMask = d_callbacks.getRegisteredEventMask(currData.fd);

        // READ/ACCEPT.

        enum { DEFAULT_MASK = POLLERR | POLLHUP | POLLNVAL };

        if (currData.revents & (POLLIN | DEFAULT_MASK)) {
            if (eventMask & bdlb::BitMaskUtil::eq(EventType::e_READ)) {
                numCallbacks += !d_callbacks.invoke(Event(currData.fd,
                                                          EventType::e_READ));
            } else {
                numCallbacks += !d_callbacks.invoke(Event(currData.fd,
                                                          EventType::e_ACCEPT));
            }
        }

        // WRITE/CONNECT.

        if (currData.revents & (POLLOUT | DEFAULT_MASK)) {
            if (eventMask & bdlb::BitMaskUtil::eq(EventType::e_WRITE)) {
                numCallbacks += !d_callbacks.invoke(Event(currData.fd,
                                                          EventType::e_WRITE));
            } else {
                numCallbacks += !d_callbacks.invoke(
                                      Event(currData.fd, EventType::e_CONNECT));
            }
        }
    }

    return numCallbacks;
}

// CREATORS
DefaultEventManager<Platform::POLLSET>::DefaultEventManager(
                                              TimeMetrics      *timeMetric,
                                              bslma::Allocator *basicAllocator)
: d_ps(::pollset_create(-1))
, d_timeMetric_p(timeMetric)
, d_callbacks(basicAllocator)
, d_signaled(basicAllocator)
{
    BSLS_ASSERT(-1 != d_ps);
}

DefaultEventManager<Platform::POLLSET>::~DefaultEventManager()
{
    int rc = ::pollset_destroy(d_ps);
    BSLS_ASSERT(-1 != rc);
}

// MANIPULATORS
int DefaultEventManager<Platform::POLLSET>::dispatch(
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

            ts.tv_sec  = currTimeout.seconds();
            ts.tv_nsec = currTimeout.nanoseconds();

            // Sleep till it's time.

            if (d_timeMetric_p) {
                d_timeMetric_p->switchTo(TimeMetrics::e_IO_BOUND);
                bslmt::ThreadUtil::microSleep(ts.tv_nsec / 1000, ts.tv_sec);
                d_timeMetric_p->switchTo(TimeMetrics::e_CPU_BOUND);
            }
            else {
                bslmt::ThreadUtil::microSleep(ts.tv_nsec / 1000, ts.tv_sec);
            }

            now = bdlt::CurrentTime::now();
        }
        return 0;                                                     // RETURN
    }

    int numSockets = d_callbacks.numSockets();

    // Note: resize should be invoked here, rather than in 'register' as
    // in previous versions, to avoid resizing while dispatchCallbacks() is
    // iterating through the array.

    d_signaled.resize(numSockets, DEFAULT_POLLFD);

    int numCallbacks = 0;                    // number of callbacks dispatched

    do {
        int relativeTimeout;
        int rfds;                    // number of returned sockets
        int savedErrno = 0;          // saved errno value set by poll
        do {
            if (timeout > now) {
                // Calculate the time remaining for the poll() call.

                bsls::TimeInterval currTimeout(timeout - now);

                // Convert this timeout to a 32 bit value in milliseconds.

                relativeTimeout = currTimeout.seconds() * 1000
                                  + currTimeout.nanoseconds() / 1000000 + 1;
                BSLS_ASSERT(relativeTimeout >= 0);
            }
            else {
                relativeTimeout = BTLSO_EVENTMANAGERIMP_POLL_NO_TIMEOUT;
            }

            if (d_timeMetric_p) {
                d_timeMetric_p->switchTo(TimeMetrics::e_IO_BOUND);

                rfds = ::pollset_poll(d_ps,
                                      &d_signaled[0],
                                      numSockets,
                                      relativeTimeout);

                savedErrno = errno;
                d_timeMetric_p->switchTo(TimeMetrics::e_CPU_BOUND);
            }
            else {
                rfds = ::pollset_poll(d_ps,
                                      &d_signaled[0],
                                      numSockets,
                                      relativeTimeout);
                savedErrno = errno;
            }
            errno = 0;
            now = bdlt::CurrentTime::now();
        } while ((0 > rfds && EINTR == savedErrno)
              && !(btlso::Flags::k_ASYNC_INTERRUPT & flags)
              && now < timeout);

        if (0 >= rfds) {
            if (0 == rfds) {
                // timed out, no events

                return 0;                                             // RETURN
            }
            else if (-1 == rfds && EINTR == savedErrno) {
                // interrupted

                return -1;                                            // RETURN
            }
            else {
                // According to the contract, we are to return any negative
                // number other than -1.  We might as well return '-savedErrno'
                // to aid in debugging, except in the case where
                // '-savedErrno >= -1', in which case it would be mistaken to
                // have another meaning, so in that case, we return -10000.

                return -savedErrno >= -1 ? -10000 : -savedErrno;      // RETURN
            }
        }

        numCallbacks += dispatchCallbacks(rfds);

        now = bdlt::CurrentTime::now();
    } while (0 == numCallbacks && now < timeout);

    return numCallbacks;
}

int DefaultEventManager<Platform::POLLSET>::dispatch(int flags)
{
    if (!numEvents()) {
        return 0;                                                     // RETURN
    }

    int numSockets = d_callbacks.numSockets();
    d_signaled.resize(numSockets, DEFAULT_POLLFD);
    int numCallbacks = 0;                    // number of callbacks dispatched

    while  (0 == numCallbacks) {
        int rfds;                    // number of returned fds
        int savedErrno = 0;          // saved errno value set by 'pollset_poll'
                                     // in case 'now()' writes over errno later
        do {
            if (d_timeMetric_p) {
                d_timeMetric_p->switchTo(TimeMetrics::e_IO_BOUND);

                rfds = ::pollset_poll(d_ps,
                                      &d_signaled[0],
                                      numSockets,
                                      BTLSO_EVENTMANAGERIMP_POLL_INF_TIMEOUT);

                savedErrno = errno;
                d_timeMetric_p->switchTo(TimeMetrics::e_CPU_BOUND);
            }
            else {
                rfds = ::pollset_poll(d_ps,
                                      &d_signaled[0],
                                      numSockets,
                                      BTLSO_EVENTMANAGERIMP_POLL_INF_TIMEOUT);
                savedErrno = errno;
            }
            errno = 0;
        } while ((0 > rfds && EINTR == savedErrno)
              && !(btlso::Flags::k_ASYNC_INTERRUPT & flags));

        if (0 >= rfds) {
            if (0 == rfds) {
                // No events.  We weren't interrupted.  Shouldn't happen. The
                // POLL event manager returns 0 in this case.

                return 0;                                             // RETURN
            }
            else if (-1 == rfds && EINTR == savedErrno) {
                // interrupted

                return -1;                                            // RETURN
            }
            else {
                // According to the contract, we are to return any negative
                // number other than -1.  We might as well return '-savedErrno'
                // to aid in debugging, except in the case where
                // '-savedErrno >= -1', in which case it would be mistaken to
                // have another meaning, so in that case, we return -10000.

                return -savedErrno >= -1 ? -10000 : -savedErrno;      // RETURN
            }
        }

        numCallbacks += dispatchCallbacks(rfds);
    }

    return numCallbacks;
}

int DefaultEventManager<Platform::POLLSET>::registerSocketEvent(
                                    const SocketHandle::Handle&   socketHandle,
                                    const EventType::Type         eventType,
                                    const EventManager::Callback& callback)
{
    Event event(socketHandle, eventType);

    uint32_t newMask = d_callbacks.registerCallback(event, callback);
    if (0 == newMask) {
        // We replaced an existing callback function.
        return 0;                                                     // RETURN
    }

    ::poll_ctl ctl;
    ctl.fd = socketHandle;

    // If there is 1 bit in the event mask, it's a new event (otherwise
    // we would have replaced its callback and returned); then the command is
    // PS_ADD.  Otherwise, we're adding an event and the command is PS_MOD.

    ctl.cmd = 1 == bdlb::BitUtil::numBitsSet(newMask)
        ? PS_ADD
        : PS_MOD;

    // We set the event being registered (POLLIN/POLLOUT) and the library will
    // OR it with any existing event (as needed, for PS_MOD).

    ctl.events = eventType == EventType::e_ACCEPT ||
                 eventType == EventType::e_READ
        ? POLLIN
        : POLLOUT;

    int rc = ::pollset_ctl(d_ps, &ctl, 1);
    if (0 != rc) {
        // On error, remove the event we just registered.
        d_callbacks.remove(event);
        return -1;                                                    // RETURN
    }

    return 0;
}

void DefaultEventManager<Platform::POLLSET>::deregisterSocketEvent(
                                      const SocketHandle::Handle& socketHandle,
                                      EventType::Type             eventType)
{
    Event                 event(socketHandle, eventType);

    bool removed = d_callbacks.remove(event);
    BSLS_ASSERT(removed);

    // Get the mask describing any remaining event (there can be at most 1
    // such event).
    uint32_t newMask = d_callbacks.getRegisteredEventMask(socketHandle);
    BSLS_ASSERT(1 >= bdlb::BitUtil::numBitsSet(newMask));
    BSLS_ASSERT(2 == bdlb::BitUtil::numBitsSet(k_POLLIN_EVENTS));

    // Passing 'PS_DELETE' to 'pollset_ctl' will erase the 'fd' entirely. If
    // there is an event remaining, we need to add it back.

    ::poll_ctl ctl;
    ctl.cmd = PS_DELETE;
    ctl.events = 0;
    ctl.fd = socketHandle;

    int rc = ::pollset_ctl(d_ps, &ctl, 1);
    if (0 != rc) {
        return;                                                       // RETURN
    }

    if (newMask) {
        // still an event on this fd, have to add it back

        ctl.cmd = PS_ADD;

        ctl.events = newMask & k_POLLIN_EVENTS
            ? POLLIN
            : POLLOUT;

        rc = ::pollset_ctl(d_ps, &ctl, 1);
        BSLS_ASSERT(0 == rc);    // if this fails right after the last
                                 // ::pollset_ctl succeeded, that's just weird
    }
}

int DefaultEventManager<Platform::POLLSET>::deregisterSocket(
                                      const SocketHandle::Handle& socketHandle)
{
    int numCallbacks = d_callbacks.removeSocket(socketHandle);

    ::poll_ctl ctl;
    ctl.cmd = PS_DELETE;
    ctl.events = 0;
    ctl.fd = socketHandle;

    int rc = ::pollset_ctl(d_ps, &ctl, 1);
    BSLS_ASSERT(0 == numCallbacks || 0 == rc);

    return numCallbacks;
}

void DefaultEventManager<Platform::POLLSET>::deregisterAll()
{
    d_callbacks.removeAll();

    int rc = ::pollset_destroy(d_ps);
    BSLS_ASSERT(0 == rc);

    d_ps = ::pollset_create(-1);
}

// ACCESSORS
bool DefaultEventManager<Platform::POLLSET>::hasLimitedSocketCapacity() const
{
    return false;
}

int DefaultEventManager<Platform::POLLSET>::numSocketEvents(
                                const SocketHandle::Handle& socketHandle) const
{
    return bdlb::BitUtil::numBitsSet(
                            d_callbacks.getRegisteredEventMask(socketHandle));
}

int DefaultEventManager<Platform::POLLSET>::numEvents() const
{
    return d_callbacks.numCallbacks();
}

int DefaultEventManager<Platform::POLLSET>::isRegistered(
                                   const SocketHandle::Handle& handle,
                                   const EventType::Type       eventType) const
{
    return d_callbacks.contains(Event(handle, eventType));
}

}  // close package namespace

}  // close namespace BloombergLP

#endif // AIX

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
