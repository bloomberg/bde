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

#include <btlso_flag.h>
#include <btlso_socketoptutil.h>
#include <btlso_timemetrics.h>

#include <bslmt_threadutil.h>

#include <bdlt_currenttime.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_timeinterval.h>

#include <bsl_vector.h>
#include <bsl_utility.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <bsl_c_errno.h>

// 'NO_TIMEOUT' and 'INF_TIMEOUT' are defined in <sys/poll.h>

#define BTLSO_EVENTMANAGERIMP_POLL_NO_TIMEOUT  NO_TIMEOUT
#define BTLSO_EVENTMANAGERIMP_POLL_INF_TIMEOUT INF_TIMEOUT

#include <bsl_cstring.h>

namespace BloombergLP {

namespace btlso {

          // --------------------------------------------
          // class DefaultEventManager<Platform::POLLSET>
          // --------------------------------------------

// PRIVATE CLASS METHODS
int DefaultEventManager<Platform::POLLSET>::dispatchCallbacks(
                                                         int numSignaled) const
{
    BSLS_ASSERT(numSignaled <= d_fdCount);

    int numCallbacks = 0;

    const CallbackMap::const_iterator cbEnd = d_callbacks.end();
    CallbackMap::const_iterator       cbIt;

    for (int i = 0; i < numSignaled; ++i) {
        const struct ::pollfd& currData = d_signaled[i];

        // READ/ACCEPT.

        enum { DEFAULT_MASK = POLLERR | POLLHUP | POLLNVAL };

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                 currData.revents & (POLLIN | DEFAULT_MASK))) {

            if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                     cbEnd != (cbIt = d_callbacks.find(
                                                 Event(currData.fd,
                                                       EventType::e_READ))))) {
                (cbIt->second)();
                ++numCallbacks;
            }
            else {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                if (cbEnd != (cbIt = d_callbacks.find(
                                                Event(currData.fd,
                                                      EventType::e_ACCEPT)))) {
                    (cbIt->second)();
                    ++numCallbacks;
                }
            }
        }

        // WRITE/CONNECT.

        if (currData.revents & (POLLOUT | DEFAULT_MASK)) {

            if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                     cbEnd != (cbIt = d_callbacks.find(
                                                Event(currData.fd,
                                                      EventType::e_WRITE))))) {
                (cbIt->second)();
                ++numCallbacks;
            }
            else {
                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

                if (cbEnd != (cbIt = d_callbacks.find(
                                               Event(currData.fd,
                                                     EventType::e_CONNECT)))) {
                    (cbIt->second)();
                    ++numCallbacks;
                }
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
, d_fdCount(0)
, d_callbacks(basicAllocator)
, d_timeMetric_p(timeMetric)
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
                                      d_fdCount,
                                      relativeTimeout);

                savedErrno = errno;
                d_timeMetric_p->switchTo(TimeMetrics::e_CPU_BOUND);
            }
            else {
                rfds = ::pollset_poll(d_ps,
                                      &d_signaled[0],
                                      d_fdCount,
                                      relativeTimeout);
                savedErrno = errno;
            }
            errno = 0;
            now = bdlt::CurrentTime::now();
        } while ((0 > rfds && EINTR == savedErrno)
              && !(btlso::Flag::k_ASYNC_INTERRUPT & flags)
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
                                      d_fdCount,
                                      BTLSO_EVENTMANAGERIMP_POLL_INF_TIMEOUT);

                savedErrno = errno;
                d_timeMetric_p->switchTo(TimeMetrics::e_CPU_BOUND);
            }
            else {
                rfds = ::pollset_poll(d_ps,
                                      &d_signaled[0],
                                      d_fdCount,
                                      BTLSO_EVENTMANAGERIMP_POLL_INF_TIMEOUT);
                savedErrno = errno;
            }
            errno = 0;
        } while ((0 > rfds && EINTR == savedErrno)
              && !(btlso::Flag::k_ASYNC_INTERRUPT & flags));

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
    int rc;

    // Determine from 'd_callbacks' if the event has already been registered.

    Event event(socketHandle, eventType);
    CallbackMap::iterator cbIt = d_callbacks.find(event);
    if (d_callbacks.end() != cbIt) {
        cbIt->second = callback;
        return 0;                                                     // RETURN
    }
    else {
        // Register the handle/eventType with d_callbacks.

        bool insertedEvent = d_callbacks.insert(
                                       bsl::make_pair(event, callback)).second;
        BSLS_ASSERT(insertedEvent);
    }

    ::poll_ctl ctl;
    ctl.fd = socketHandle;

    unsigned short eventMask;
    ::pollfd       queryPollfd;

    queryPollfd.fd = socketHandle;
    rc = ::pollset_query(d_ps, &queryPollfd);

    if (1 == rc) {
        // the file descriptor is already registered

        eventMask = queryPollfd.events;
        ctl.cmd = PS_MOD;
    }
    else {
        // new file descriptor

        BSLS_ASSERT(0 == rc);

        eventMask = 0;
        ctl.cmd = PS_ADD;

        ++d_fdCount;
        if (d_fdCount > d_signaled.size()) {
            d_signaled.resize(d_fdCount);
        }
    }

    switch (eventType) {
      case EventType::e_ACCEPT: {
        // No other event type can be registered simultaneously with ACCEPT.

        BSLS_ASSERT(0 == eventMask);
        eventMask |= POLLIN;
      } break;
      case EventType::e_READ: {
        // Only WRITE can be registered simultaneously with READ.

        BSLS_ASSERT(0 == (eventMask & ~POLLOUT));
        BSLS_ASSERT_SAFE(!eventMask
                      || !d_callbacks.count(Event(socketHandle,
                                                  EventType::e_CONNECT)));
        eventMask |= POLLIN;
      } break;
      case EventType::e_CONNECT: {
        // No other event type can be registered simultaneously with CONNECT.

        BSLS_ASSERT(0 == eventMask);
        eventMask |= POLLOUT;
      } break;
      case EventType::e_WRITE: {
        // Only READ can be registered simultaneously with WRITE.

        BSLS_ASSERT(0 == (eventMask & ~POLLIN));
        BSLS_ASSERT_SAFE(!eventMask
                      || !d_callbacks.count(Event(socketHandle,
                                                  EventType::e_ACCEPT)));
        eventMask |= POLLOUT;
      } break;
      default: {
        BSLS_ASSERT("Invalid event type (must be unreachable by design)" && 0);
        return -1;                                                    // RETURN
      } break;
    }

    ctl.events = eventMask;
    rc = ::pollset_ctl(d_ps, &ctl, 1);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    return 0;
}

void DefaultEventManager<Platform::POLLSET>::deregisterSocketEvent(
                                      const SocketHandle::Handle& socketHandle,
                                      EventType::Type             eventType)
{
    int rc;

    // Determine from d_callbacks if the event is currently registered.

    Event                 event(socketHandle, eventType);
    CallbackMap::iterator cbIt = d_callbacks.find(event);

    BSLS_ASSERT(d_callbacks.end() != cbIt);

    d_callbacks.erase(cbIt);

    ::pollfd queryPollfd;
    queryPollfd.fd = socketHandle;

    rc = ::pollset_query(d_ps, &queryPollfd);

    if (1 != rc) {
        // perhaps the fd was closed.

        return;
    }

    // Translate the type of event.

    switch (eventType) {
      case EventType::e_ACCEPT:                                 // FALL THROUGH
      case EventType::e_READ: {
        queryPollfd.events &= ~POLLIN;
      } break;
      case EventType::e_CONNECT:                                // FALL THROUGH
      case EventType::e_WRITE: {
        queryPollfd.events &= ~POLLOUT;
      } break;
      default: {
        BSLS_ASSERT("Invalid eventType (must be unreachable)" && 0);
        return;                                                       // RETURN
      }
    }

    // no way to clear bits in an entry with 'pollset_ctl', we have to delete
    // the fd and if bits are still set, add it back again.

    ::poll_ctl ctl;
    ctl.cmd = PS_DELETE;
    ctl.events = 0;
    ctl.fd = socketHandle;

    rc = ::pollset_ctl(d_ps, &ctl, 1);
    if (0 != rc) {
        return;
    }

    if (queryPollfd.events) {
        // still some events on this fd, have to add it back

        ctl.cmd = PS_ADD;
        ctl.events = queryPollfd.events;

        rc = ::pollset_ctl(d_ps, &ctl, 1);
        BSLS_ASSERT(0 == rc);    // if this fails right after the last
                                 // ::pollset_ctl succeeded, that's just weird
    }
    else {
        --d_fdCount;

        // It is important not to do 'd_signaled.resize(d_fdCount)', because
        // we might be within a dispatch event, and the last element(s) of
        // 'd_signaled' may contain valid events which must be processed by
        // this call to 'dispatch'.
    }
}

int DefaultEventManager<Platform::POLLSET>::deregisterSocket(
                                      const SocketHandle::Handle& socketHandle)
{
    int numCallbacks = 0;

    Event event(socketHandle, EventType::e_ACCEPT);

    numCallbacks += d_callbacks.erase(event);

    event.setType(EventType::e_CONNECT);

    numCallbacks += d_callbacks.erase(event);

    event.setType(EventType::e_READ);

    numCallbacks += d_callbacks.erase(event);

    event.setType(EventType::e_WRITE);

    numCallbacks += d_callbacks.erase(event);

    BSLS_ASSERT((unsigned) numCallbacks <= 2);

    ::poll_ctl ctl;
    ctl.cmd = PS_DELETE;
    ctl.events = 0;
    ctl.fd = socketHandle;

    int rc = ::pollset_ctl(d_ps, &ctl, 1);
    BSLS_ASSERT(0 == numCallbacks || 0 == rc);

    if (0 == rc) {
        --d_fdCount;

        // It is important not to do 'd_signaled.resize(d_fdCount)', because
        // we might be within a dispatch event, and the last element(s) of
        // 'd_signaled' may contain valid events which must be processed by
        // this call to 'dispatch'.
    }

    return numCallbacks;
}

void DefaultEventManager<Platform::POLLSET>::deregisterAll()
{
    d_callbacks.clear();

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
    ::pollfd queryPollfd;
    queryPollfd.fd = socketHandle;

    int rc = ::pollset_query(d_ps, &queryPollfd);
    if (0 == rc) {
        return 0;                                                     // RETURN
    }

    int numInputEvents  = !!(queryPollfd.events & POLLIN);
    int numOutputEvents = !!(queryPollfd.events & POLLOUT);

    return numInputEvents + numOutputEvents;
}

int DefaultEventManager<Platform::POLLSET>::numEvents() const
{
    return d_callbacks.size();
}

int DefaultEventManager<Platform::POLLSET>::isRegistered(
                                   const SocketHandle::Handle& handle,
                                   const EventType::Type       eventType) const
{
    return 1 == d_callbacks.count(Event(handle, eventType));
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
