// btlso_defaulteventmanager_poll.cpp                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_defaulteventmanager_poll.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_defaulteventmanager_poll_cpp,"$Id$ $CSID$")

#include <btlso_flags.h>
#include <btlso_socketoptutil.h>
#include <btlso_timemetrics.h>

#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_timeinterval.h>
#include <bdlb_bitmaskutil.h>
#include <bdlb_bitutil.h>
#include <bdlt_currenttime.h>

#include <bsls_assert.h>

#include <bsl_vector.h>
#include <bsl_utility.h>

#if defined(BSLS_PLATFORM_OS_SOLARIS)    \
 || defined(BSLS_PLATFORM_OS_LINUX)      \
 || defined(BSLS_PLATFORM_OS_FREEBSD)    \
 || defined(BSLS_PLATFORM_OS_AIX)        \
 || defined(BSLS_PLATFORM_OS_HPUX)       \
 || defined(BSLS_PLATFORM_OS_CYGWIN)     \
 || defined(BSLS_PLATFORM_OS_DARWIN)

#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <bsl_c_errno.h>

#ifdef BSLS_PLATFORM_OS_AIX
    #include <sys/timers.h>       // timespec
    #define BTLSO_DEFAULTEVENTMANAGER_POLL_NO_TIMEOUT  NO_TIMEOUT
    #define BTLSO_DEFAULTEVENTMANAGER_POLL_INF_TIMEOUT INF_TIMEOUT
    // The following set of checks mirror those found on AIX in <sys/poll.h>.
    // When we include this file in our headers, we are careful to undefine the
    // 'events' and 'revents' macros since they cause unfortunate collisions,
    // but here in this file we do want their effect.
    #if _XOPEN_SOURCE_EXTENDED == 1 &&                                        \
        defined(_ALL_SOURCE) &&                                               \
        !defined(__64BIT__) &&                                                \
        !defined(__HIDE_SVR4_POLLFD_NAMES)
        #define events reqevents   // renamed field in struct pollfd
        #define revents rtnevents  // renamed field in struct pollfd
    #endif
#elif defined(BSLS_PLATFORM_OS_LINUX)  ||  \
      defined(BSLS_PLATFORM_OS_CYGWIN) ||  \
      defined(BSLS_PLATFORM_OS_DARWIN)
    #define BTLSO_DEFAULTEVENTMANAGER_POLL_NO_TIMEOUT  0
    #define BTLSO_DEFAULTEVENTMANAGER_POLL_INF_TIMEOUT -1
#else
    #ifdef BSLS_PLATFORM_OS_SOLARIS
    #include <sys/stropts.h>      // INFTIM
    #endif
    #define BTLSO_DEFAULTEVENTMANAGER_POLL_NO_TIMEOUT  0
    #define BTLSO_DEFAULTEVENTMANAGER_POLL_INF_TIMEOUT INFTIM
#endif

#include <bsl_cstring.h>

namespace BloombergLP {

namespace btlso {

static
short convertMask(uint32_t eventMask)
{
    // Return a mask with POLLIN set if READ or ACCEPT is set in 'eventMask',
    // and with POLLOUT set if WRITE or CONNECT is set in 'eventMask'.
    // Assert that if multiple events are registered, they are READ and WRITE.

    int pollinEvents = bdlb::BitUtil::numBitsSet(
                                       eventMask & EventType::k_INBOUND_EVENTS);
    int polloutEvents = bdlb::BitUtil::numBitsSet(
                                       eventMask & EventType::k_OUTBOUND_EVENTS);
    BSLS_ASSERT(2 > pollinEvents);
    BSLS_ASSERT(2 > polloutEvents);
    BSLS_ASSERT(!(pollinEvents && polloutEvents) ||
                (eventMask & bdlb::BitMaskUtil::eq(EventType::e_READ) &&
                 eventMask & bdlb::BitMaskUtil::eq(EventType::e_WRITE)));

    return static_cast<short>((POLLIN * pollinEvents) |
                              (POLLOUT * polloutEvents));
}

           // -----------------------------------------
           // class DefaultEventManager<Platform::POLL>
           // -----------------------------------------

// PRIVATE MANIPULATORS
int DefaultEventManager<Platform::POLL>::dispatchCallbacks()
{
    BSLS_ASSERT(0 == d_signaledFds.size()); // internal invariant

    int numCallbacks = 0;

    // Implementation note: We need to copy the signaled sockets because an
    // invoked callback can potentially modify 'd_pollFds' thereby corrupting
    // the iteration order. We copy each to a working array along with the
    // mask for all registered events for that socket.

    enum { DEFAULT_MASK = POLLERR | POLLHUP | POLLNVAL };

    int numFds = static_cast<int>(d_pollFds.size());
    for (int i = 0; i < numFds; ++i) {
        const struct ::pollfd data = d_pollFds[i];

        if (0 == data.revents) {
            continue;
        }

        IndexMap::const_iterator it = d_index.find(data.fd);
        BSLS_ASSERT(it != d_index.end());
        uint32_t eventMask = it->second.second;

        // Copy fds for READ/ACCEPT events.

        uint32_t registeredRead = eventMask & EventType::k_INBOUND_EVENTS;
        if (0 != registeredRead &&
            data.revents & (POLLIN | DEFAULT_MASK)) {
            d_signaledFds.push_back(bsl::make_pair(data.fd, registeredRead));
        }

        // Copy fds for WRITE/CONNECT events.

        uint32_t registeredWrite = eventMask & EventType::k_OUTBOUND_EVENTS;
        if (0 != registeredWrite &&
            data.revents & (POLLOUT | DEFAULT_MASK)) {
            d_signaledFds.push_back(bsl::make_pair(data.fd, registeredWrite));
        }
    }

    // Then iterate through fds with registered events and invoke the
    // appropriate callback.

    for (SignaledArray::iterator it = d_signaledFds.begin();
         it != d_signaledFds.end(); ++it) {
        uint32_t eventMask = it->second;

        if (eventMask & bdlb::BitMaskUtil::eq(EventType::e_READ)) {
            numCallbacks += !d_callbacks.invoke(Event(it->first,
                                                      EventType::e_READ));
        } else if (eventMask &
                   bdlb::BitMaskUtil::eq(EventType::e_ACCEPT)) {
            numCallbacks += !d_callbacks.invoke(Event(it->first,
                                                      EventType::e_ACCEPT));
        }

        if (eventMask & bdlb::BitMaskUtil::eq(EventType::e_WRITE)) {
            numCallbacks += !d_callbacks.invoke(Event(it->first,
                                                      EventType::e_WRITE));
        } else if (eventMask &
                   bdlb::BitMaskUtil::eq(EventType::e_CONNECT)) {
            numCallbacks += !d_callbacks.invoke(Event(it->first,
                                                      EventType::e_CONNECT));
        }
    }

    d_signaledFds.clear();
    return numCallbacks;
}

// CREATORS
DefaultEventManager<Platform::POLL>::DefaultEventManager(
                                              TimeMetrics      *timeMetric,
                                              bslma::Allocator *basicAllocator)
: d_pollFds(basicAllocator)
, d_callbacks(basicAllocator)
, d_index(basicAllocator)
, d_timeMetric_p(timeMetric)
, d_signaledFds(basicAllocator)
{
}

DefaultEventManager<Platform::POLL>::~DefaultEventManager()
{
    BSLS_ASSERT(0 == d_signaledFds.size()); // internal invariant
}

// MANIPULATORS
int DefaultEventManager<Platform::POLL>::dispatch(
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

            ts.tv_sec  = static_cast<int>(currTimeout.seconds());
            ts.tv_nsec = currTimeout.nanoseconds();

            // Sleep till it's time.

            if (d_timeMetric_p) {
                d_timeMetric_p->switchTo(TimeMetrics::e_IO_BOUND);
                bslmt::ThreadUtil::microSleep(
                                           static_cast<int>(ts.tv_nsec / 1000),
                                           static_cast<int>(ts.tv_sec));
                d_timeMetric_p->switchTo(TimeMetrics::e_CPU_BOUND);
            }
            else {
                bslmt::ThreadUtil::microSleep(
                                           static_cast<int>(ts.tv_nsec / 1000),
                                           static_cast<int>(ts.tv_sec));
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

                relativeTimeout =
                        static_cast<int>(currTimeout.seconds()) * 1000
                                     + currTimeout.nanoseconds() / 1000000 + 1;
                BSLS_ASSERT(relativeTimeout >= 0);
            }
            else {
                relativeTimeout = BTLSO_DEFAULTEVENTMANAGER_POLL_NO_TIMEOUT;
            }

            if (d_timeMetric_p) {
                d_timeMetric_p->switchTo(TimeMetrics::e_IO_BOUND);

                rfds = ::poll(&d_pollFds.front(),
                              d_pollFds.size(),
                              relativeTimeout);

                savedErrno = errno;
                d_timeMetric_p->switchTo(TimeMetrics::e_CPU_BOUND);
            }
            else {
                rfds = ::poll(&d_pollFds.front(),
                              d_pollFds.size(),
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

        numCallbacks += dispatchCallbacks();
        now = bdlt::CurrentTime::now();
    } while (0 == numCallbacks && now < timeout);

    return numCallbacks;
}

int DefaultEventManager<Platform::POLL>::dispatch(int flags)
{
    if (!numEvents()) {
        return 0;                                                     // RETURN
    }

    int numCallbacks = 0;                    // number of callbacks dispatched

    while  (0 == numCallbacks) {
        int rfds;                    // number of returned fds
        int savedErrno = 0;          // saved errno value set by 'poll'
        do {
            if (d_timeMetric_p) {
                d_timeMetric_p->switchTo(TimeMetrics::e_IO_BOUND);

                rfds = ::poll(&d_pollFds.front(),
                              d_pollFds.size(),
                              BTLSO_DEFAULTEVENTMANAGER_POLL_INF_TIMEOUT);

                savedErrno = errno;
                d_timeMetric_p->switchTo(TimeMetrics::e_CPU_BOUND);
            }
            else {
                rfds = ::poll(&d_pollFds.front(),
                              d_pollFds.size(),
                              BTLSO_DEFAULTEVENTMANAGER_POLL_INF_TIMEOUT);
                savedErrno = errno;
            }
            errno = 0;
        } while ((0 > rfds && EINTR == savedErrno)
              && !(btlso::Flags::k_ASYNC_INTERRUPT & flags));

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

        numCallbacks += dispatchCallbacks();
    }
    return numCallbacks;
}

int DefaultEventManager<Platform::POLL>::registerSocketEvent(
                                    const SocketHandle::Handle&   socketHandle,
                                    const EventType::Type         event,
                                    const EventManager::Callback& callback)
{
    Event handleEvent(socketHandle, event);

    uint32_t newMask = d_callbacks.registerCallback(handleEvent, callback);
    if (0 == newMask) {
        // We replaced an existing callback function.
        return 0;                                                     // RETURN
    }

    // Map the socket handle to the corresponding index into 'd_index'.

    IndexMap::iterator indexIt = d_index.find(socketHandle);

    short eventmask = convertMask(newMask);
    if (d_index.end() != indexIt) {
        // We added an event for this fd.  Update the event mask.
        d_pollFds[indexIt->second.first].events = eventmask;
        indexIt->second.second = newMask;
    }
    else {
        // New event for this fd. Its position will be at the end of d_pollFds.
        int index = static_cast<int>(d_pollFds.size());

        struct ::pollfd currfd;
        bsl::memset(&currfd, 0, sizeof(currfd)); // make purify/ZeroFault happy
        currfd.fd = socketHandle;
        currfd.events = eventmask;
        d_pollFds.push_back(currfd);

        bsl::pair<int, uint32_t> indexAndMask = bsl::make_pair(index, newMask);
        bool insertedIndex = d_index.insert(
                                          bsl::make_pair(socketHandle,
                                                         indexAndMask)).second;
        (void)insertedIndex; BSLS_ASSERT(insertedIndex);
    }
    return 0;
}

void DefaultEventManager<Platform::POLL>::removeFdAtIndex(int index)
{
    int fd = d_pollFds[index].fd;

    int lastIndex = static_cast<int>(d_pollFds.size()) - 1;
    if (lastIndex != index) {

        // This fd is not at the end of d_pollFds.  Overwrite it with the one
        // that is so that we can trivially erase the last element.

        IndexMap::iterator lastIndexIt = d_index.find(d_pollFds[lastIndex].fd);
        BSLS_ASSERT(d_index.end() != lastIndexIt
                    && lastIndexIt->second.first == lastIndex);

        // Move information from the last handle to the current one.

        d_pollFds[index] = d_pollFds[lastIndex];

        // Update the index information for the fd we moved.

        lastIndexIt->second.first = index;
    }

    // Shorten the array.

    d_pollFds.resize(d_pollFds.size() - 1);

    // Delete index information.

    int rc = static_cast<int>(d_index.erase(fd));
    BSLS_ASSERT(1 == rc);
}

void DefaultEventManager<Platform::POLL>::deregisterSocketEvent(
                                      const SocketHandle::Handle& socketHandle,
                                      EventType::Type             event)
{
    Event  handleEvent(socketHandle, event);

    bool removed = d_callbacks.remove(handleEvent);
    BSLS_ASSERT(removed);

    // Translate the type of event.
    uint32_t newMask = d_callbacks.getRegisteredEventMask(socketHandle);
    short eventmask = convertMask(newMask);

    // Retrieve the '::pollfd' corresponding to 'socketHandle'.

    IndexMap::iterator indexIt = d_index.find(socketHandle);
    BSLS_ASSERT(d_index.end() != indexIt);

    int index = indexIt->second.first;

    // Retrieve '::pollfd' from 'index'.

    struct ::pollfd& currfd = d_pollFds[index];

    if (eventmask) {
        currfd.events = eventmask;
        indexIt->second.second = newMask;
    }
    else {
        // Remove this fd from d_pollFds.
        removeFdAtIndex(index);
    }
}

int DefaultEventManager<Platform::POLL>::deregisterSocket(
                                      const SocketHandle::Handle& socketHandle)
{
    int numCallbacks = d_callbacks.removeSocket(socketHandle);
    if (0 == numCallbacks) {
        // No registered events, nothing to do.
        return 0;                                                     // RETURN
    }

    IndexMap::iterator indexIt = d_index.find(socketHandle);
    BSLS_ASSERT(d_index.end() != indexIt);

    int  index = indexIt->second.first;
    removeFdAtIndex(index);
    return numCallbacks;
}

void DefaultEventManager<Platform::POLL>::deregisterAll()
{
    d_pollFds.clear();
    d_index.clear();
    d_callbacks.removeAll();
}

// ACCESSORS
int DefaultEventManager<Platform::POLL>::numSocketEvents(
                                const SocketHandle::Handle& socketHandle) const
{
    return bdlb::BitUtil::numBitsSet(
                            d_callbacks.getRegisteredEventMask(socketHandle));
}

int DefaultEventManager<Platform::POLL>::numEvents() const
{
    return d_callbacks.numCallbacks();
}

int DefaultEventManager<Platform::POLL>::isRegistered(
                                       const SocketHandle::Handle& handle,
                                       const EventType::Type       event) const
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
