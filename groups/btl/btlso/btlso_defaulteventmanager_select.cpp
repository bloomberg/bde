// btlso_defaulteventmanager_select.cpp                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlso_defaulteventmanager_select.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_defaulteventmanager_select_cpp,"$Id$ $CSID$")

#include <btlso_timemetrics.h>
#include <btlso_flag.h>

#include <bdlt_currenttime.h>

#include <bsls_assert.h>
#include <bsl_algorithm.h>
#include <bsl_cerrno.h>
#include <bsl_cstring.h>

// EINTR does not have any exact equivalent on Windows, except WSAEINTR which
// indicates the call has been interrrupted with WSACancelBlockingCall().
// Also, Microsoft's errno.h contains an erroneous definition of EINTR which
// actually means ERROR_TOO_MANY_OPEN_FILES. Fixing EINTR definition here:

#ifdef BSLS_PLATFORM_OS_WINDOWS
#ifdef EINTR
#undef EINTR
#endif
#define EINTR WSAEINTR
#endif

using bsl::memset;          // 'FD_ZERO' macro calls 'memset' with no namespace

namespace BloombergLP {

static bool compareFdSets(const fd_set& lhs, const fd_set& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' file descriptor sets are
    // equal, and 'false' otherwise.
{
#ifdef BTLSO_PLATFORM_BSD_SOCKETS
    return !bsl::memcmp(&lhs, &rhs, sizeof(fd_set));                  // RETURN
#endif
#ifdef BTLSO_PLATFORM_WIN_SOCKETS
    if (lhs.fd_count != rhs.fd_count) {
        return false;                                                 // RETURN
    }
    int numSockets = lhs.fd_count;
    for (int i = 0; i < numSockets; ++i) {
        SOCKET s = lhs.fd_array[i];
        int j;
        for (j = 0; j < numSockets; ++j) {
            if (rhs.fd_array[j] == s) {
                break;
            }
        }

        if (j == numSockets) {

            // A socket handle, s, is in 'lhs' and not in 'rhs'.

            return false;                                             // RETURN
        }
    }
    return true;
#endif
}

static inline void copySet(fd_set *dst, const fd_set& src)
     // Assign to the specified 'dst' file descriptor set the value of the
     // specified 'src' file descriptor set.
{
#ifdef BTLSO_PLATFORM_WIN_SOCKETS
    dst->fd_count = src.fd_count;
    bsl::memcpy(&dst->fd_array,
                &src.fd_array,
                dst->fd_count * sizeof(dst->fd_array[0]));
#else
    *dst = src;
#endif
}

static void convert(struct timeval *result, const bsls::TimeInterval& value)
    // Convert the specified 'value' of 'bsls::TimeInterval' to a 'timeval',
    // and load into the specified 'result'.
{
    BSLS_ASSERT(result);

    result->tv_sec = value.seconds() > 0
                     ? (bsl::time_t)value.seconds()
                     : 0;
    result->tv_usec = value.nanoseconds() / 1000; // microseconds
    result->tv_usec += value.nanoseconds() % 1000 > 0;

    if (result->tv_usec < 0) {
        result->tv_usec = 0;
    }
    if (result->tv_sec < 0) {
        result->tv_sec = 0;
    }
    BSLS_ASSERT(result->tv_usec >= 0);
    BSLS_ASSERT(result->tv_sec >= 0);
}

namespace btlso {

           // -------------------------------------------
           // class DefaultEventManager<Platform::SELECT>
           // -------------------------------------------

bool DefaultEventManager<Platform::SELECT>::checkInternalInvariants() const
{
    EventMap::const_iterator it(d_events.begin()), end(d_events.end());
    fd_set readControl, writeControl, exceptControl;
    FD_ZERO(&readControl);
    FD_ZERO(&writeControl);
    FD_ZERO(&exceptControl);

    for (; it != end; ++it) {
        const Event& event = it->first;
        if (EventType::e_READ == event.type()
         || EventType::e_ACCEPT == event.type()) {
            FD_SET(event.handle(), &readControl);
        }
        else {
            FD_SET(event.handle(), &writeControl);
#ifdef BTLSO_PLATFORM_WIN_SOCKETS
            FD_SET(event.handle(), &exceptControl);
#endif
        }
    }

    return compareFdSets(d_readSet, readControl)
        && compareFdSets(d_writeSet, writeControl)
        && compareFdSets(d_exceptSet, exceptControl);
}

int DefaultEventManager<Platform::SELECT>::dispatchCallbacks(
                                                 int           numEvents,
                                                 const fd_set& readSet,
                                                 const fd_set& writeSet,
                                                 const fd_set& exceptSet) const
{
    // Iterate through all the events to find out which file descriptors have
    // been set and invoke their respective callbacks.

    EventMap::const_iterator it(d_events.begin()), end(d_events.end());

    for (; numEvents > 0 && it != end; ++it) {
        const Event& event = it->first;
        if (EventType::e_READ == event.type()
         || EventType::e_ACCEPT == event.type()) {
            if (FD_ISSET(event.handle(), &readSet)) {
                d_signaledReads.push_back(event);
                --numEvents;
            }
        }
        else {
#ifdef BTLSO_PLATFORM_WIN_SOCKETS
            if (FD_ISSET(event.handle(), &writeSet)
             || FD_ISSET(event.handle(), &exceptSet)) {
#else
            (void) exceptSet;    // silence unused warning

            if (FD_ISSET(event.handle(), &writeSet)) {
#endif
                d_signaledWrites.push_back(event);
                --numEvents;
            }
        }
    }

    int numDispatched = 0;
    int numReads      = d_signaledReads.size();

    for (int i = 0; i < numReads; ++i) {
        EventMap::const_iterator callbackIt =
                                             d_events.find(d_signaledReads[i]);
        if (d_events.end() != callbackIt) {
            ++numDispatched;
            (callbackIt->second)();
        }
    }

    int numWrites = d_signaledWrites.size();

    for (int i = 0; i < numWrites; ++i) {
        EventMap::const_iterator callbackIt =
                                            d_events.find(d_signaledWrites[i]);
        if (d_events.end() != callbackIt) {
            ++numDispatched;
            (callbackIt->second)();
        }
    }

    d_signaledReads.clear();
    d_signaledWrites.clear();

    return numDispatched;
}

// CREATORS
DefaultEventManager<Platform::SELECT>::DefaultEventManager(
                                              bslma::Allocator *basicAllocator)
: d_eventsAllocator(basicAllocator)
, d_events(&d_eventsAllocator)
, d_numRead(0)
, d_numWrite(0)
, d_maxFd(0)
, d_timeMetric(0)
, d_signaledReads(basicAllocator)
, d_signaledWrites(basicAllocator)
{
    FD_ZERO(&d_readSet);
    FD_ZERO(&d_writeSet);
    FD_ZERO(&d_exceptSet);

    d_signaledReads.reserve(k_MAX_NUM_HANDLES);
    d_signaledWrites.reserve(k_MAX_NUM_HANDLES);
}

DefaultEventManager<Platform::SELECT>::DefaultEventManager(
                                              TimeMetrics      *timeMetric,
                                              bslma::Allocator *basicAllocator)
: d_eventsAllocator(basicAllocator)
, d_events(&d_eventsAllocator)
, d_numRead(0)
, d_numWrite(0)
, d_maxFd(0)
, d_timeMetric(timeMetric)
, d_signaledReads(basicAllocator)
, d_signaledWrites(basicAllocator)
{
    FD_ZERO(&d_readSet);
    FD_ZERO(&d_writeSet);
    FD_ZERO(&d_exceptSet);

    d_signaledReads.reserve(k_MAX_NUM_HANDLES);
    d_signaledWrites.reserve(k_MAX_NUM_HANDLES);
}

DefaultEventManager<Platform::SELECT>::~DefaultEventManager()
{
    BSLS_ASSERT(checkInternalInvariants());
    BSLS_ASSERT(0 == d_signaledReads.size());
    BSLS_ASSERT(0 == d_signaledWrites.size());

    d_events.clear();
}

// MANIPULATORS
int DefaultEventManager<Platform::SELECT>::dispatch(int flags)
{
    if (0 == d_events.size()) {
        return 0;                                                     // RETURN
    }

    fd_set readSet, writeSet, exceptSet;
    copySet(&readSet, d_readSet);
    copySet(&writeSet, d_writeSet);
    copySet(&exceptSet, d_exceptSet);

    int ret;
    int savedErrno;

    do {
        errno = 0;
        if (d_timeMetric) {
            d_timeMetric->switchTo(TimeMetrics::e_IO_BOUND);

            ret = ::select(d_maxFd, &readSet, &writeSet, &exceptSet, NULL);

            savedErrno = errno;
            d_timeMetric->switchTo(TimeMetrics::e_CPU_BOUND);
        }
        else {
            ret = ::select(d_maxFd, &readSet, &writeSet, &exceptSet, NULL);

            savedErrno = errno;
        }
    } while (ret < 0
          && EINTR == savedErrno
          && !(flags & btlso::Flag::k_ASYNC_INTERRUPT));

    if (ret < 0) {
        return EINTR == savedErrno ? -1 : ret;                        // RETURN
    }

    BSLS_ASSERT(0 < ret);
    return dispatchCallbacks(ret, readSet, writeSet, exceptSet);
}

int DefaultEventManager<Platform::SELECT>::dispatch(
                                             const bsls::TimeInterval& timeout,
                                             int                       flags)
{
    int ret;
    fd_set readSet, writeSet, exceptSet;
    copySet(&readSet, d_readSet);
    copySet(&writeSet, d_writeSet);
    copySet(&exceptSet, d_exceptSet);

#ifdef BTLSO_PLATFORM_WIN_SOCKETS
    enum {
       RESOLUTION_MS = 10,   // in milliseconds
       MILLISECOND   = 1000, // in microseconds
       INTERVAL      = RESOLUTION_MS * MILLISECOND,
       ADJUSTMENT    = INTERVAL  - 1
    };

    struct timeval tv;
    bsls::TimeInterval delta = timeout - bdlt::CurrentTime::now();
    convert(&tv, delta);

    tv.tv_usec = (tv.tv_usec + ADJUSTMENT) / INTERVAL * INTERVAL;
    BSLS_ASSERT(0 == tv.tv_usec % INTERVAL);

    if (0 == d_events.size()) {
        DWORD timeoutMS = tv.tv_sec * 1000 + tv.tv_usec / 1000 + 1;
        if (d_timeMetric) {
            d_timeMetric->switchTo(TimeMetrics::e_IO_BOUND);
            ::Sleep(timeoutMS);
            d_timeMetric->switchTo(TimeMetrics::e_CPU_BOUND);
        }
        else {
            ::Sleep(timeoutMS);
        }
        ret = 0;
    }
    else {
        if (d_timeMetric) {
            d_timeMetric->switchTo(TimeMetrics::e_IO_BOUND);

            ret = ::select(d_maxFd, &readSet, &writeSet, &exceptSet, &tv);

            d_timeMetric->switchTo(TimeMetrics::e_CPU_BOUND);
        }
        else {
            ret = ::select(d_maxFd, &readSet, &writeSet, &exceptSet, &tv);
        }
    }
    if (ret <= 0) {
        return ret;
    }
#else
    int savedErrno = 0;
    BSLS_ASSERT(savedErrno != EINTR);
    do {
        struct timeval tv;
        bsls::TimeInterval delta = timeout - bdlt::CurrentTime::now();
        convert(&tv, delta);
        errno = 0;
        if (d_timeMetric) {
            d_timeMetric->switchTo(TimeMetrics::e_IO_BOUND);

            ret = ::select(d_maxFd, &readSet, &writeSet, &exceptSet, &tv);

           savedErrno = errno;
            d_timeMetric->switchTo(TimeMetrics::e_CPU_BOUND);
        }
        else {
            ret = ::select(d_maxFd, &readSet, &writeSet, &exceptSet, &tv);
            savedErrno = errno;
        }
    } while ((ret < 0
          && EINTR == savedErrno
          && !(flags & btlso::Flag::k_ASYNC_INTERRUPT))
             #ifdef BSLS_PLATFORM_OS_LINUX
             // Linux select() returns at one tick *preceding* the expiration
             // of the timeout.  Since code usually expects that select() will
             // sleep a bit more than the timeout (i.e select() will return at
             // the tick following the expiration of the timeout), we will need
             // to sleep a bit more.  For more information, see
             // http://www.uwsg.iu.edu/hypermail/linux/kernel/0402.2/1636.html

          || (ret == 0 && (timeout - bdlt::CurrentTime::now()) > 0)
             #endif
             );

    if (ret <= 0) {
        return savedErrno == EINTR ? -1 : ret;                        // RETURN
    }
#endif
    BSLS_ASSERT(ret > 0);

    return dispatchCallbacks(ret, readSet, writeSet, exceptSet);
}

int DefaultEventManager<Platform::SELECT>::registerSocketEvent(
                                       const SocketHandle::Handle&   handle,
                                       const EventType::Type         eventType,
                                       const EventManager::Callback& callback)
{
#ifdef BTLSO_PLATFORM_BSD_SOCKETS
    BSLS_ASSERT(handle < k_MAX_NUM_HANDLES);
#else
    BSLS_ASSERT(canRegisterSockets());
#endif

    Event ev(handle, eventType);
    d_events[ev] = callback;

    if (eventType == EventType::e_READ || eventType == EventType::e_ACCEPT) {
        if (!FD_ISSET(handle, &d_readSet)) {
            FD_SET(handle, &d_readSet);
            ++d_numRead;
        }
    }
    else {
        if (!FD_ISSET(handle, &d_writeSet)) {
            FD_SET(handle, &d_writeSet);
#ifdef BTLSO_PLATFORM_WIN_SOCKETS
            FD_SET(handle, &d_exceptSet);
#endif
            ++d_numWrite;
        }
    }
#ifdef BTLSO_PLATFORM_BSD_SOCKETS
    if (handle >= d_maxFd) {
        d_maxFd = handle + 1;
    }
#endif

    return 0;
}

void DefaultEventManager<Platform::SELECT>::deregisterSocketEvent(
                                            const SocketHandle::Handle& handle,
                                            EventType::Type             event)
{
    Event ev(handle, event);

    if (1 == d_events.erase(ev)) {
        if (event == EventType::e_READ || event == EventType::e_ACCEPT) {
            FD_CLR(handle, &d_readSet);
            --d_numRead;
        }
        else {
            FD_CLR(handle, &d_writeSet);
#ifdef BTLSO_PLATFORM_WIN_SOCKETS
            FD_CLR(handle, &d_exceptSet);
#endif
            --d_numWrite;
        }
    }
}

int DefaultEventManager<Platform::SELECT>::deregisterSocket(
                                            const SocketHandle::Handle& handle)
{
#ifdef BTLSO_PLATFORM_BSD_SOCKETS
    if (handle == d_maxFd - 1) {
        d_maxFd = 0;
        EventMap::iterator it(d_events.begin()), end(d_events.end());
        for (; it != end; ++it) {
            if (it->first.handle() > d_maxFd) {
                d_maxFd = it->first.handle();
            }
        }
        ++d_maxFd;
    }
#endif
    int ncbs = 0;

    if (isRegistered(handle, EventType::e_READ)) {
        deregisterSocketEvent(handle, EventType::e_READ);
        ++ncbs;
    }

    if (isRegistered(handle, EventType::e_WRITE)) {
        deregisterSocketEvent(handle, EventType::e_WRITE);
        ++ncbs;
    }

    if (isRegistered(handle, EventType::e_ACCEPT)) {
        deregisterSocketEvent(handle, EventType::e_ACCEPT);
        ++ncbs;
    }

    if (isRegistered(handle, EventType::e_CONNECT)) {
        deregisterSocketEvent(handle, EventType::e_CONNECT);
        ++ncbs;
    }
    return ncbs;
}

void DefaultEventManager<Platform::SELECT>::deregisterAll()
{
    d_events.clear();
    FD_ZERO(&d_readSet);
    FD_ZERO(&d_writeSet);
    FD_ZERO(&d_exceptSet);
    d_maxFd = 0;
    d_numRead = 0;
    d_numWrite = 0;
}

// ACCESSORS
bool DefaultEventManager<Platform::SELECT>::canRegisterSockets() const
{
#ifdef BTLSO_PLATFORM_WIN_SOCKETS
    return bsl::max(d_numRead, d_numWrite) < k_MAX_NUM_HANDLES;
#else
    return d_maxFd < k_MAX_NUM_HANDLES;
#endif
}

int DefaultEventManager<Platform::SELECT>::isRegistered(
                                       const SocketHandle::Handle& handle,
                                       const EventType::Type       event) const
{
    return d_events.end() != d_events.find(Event(handle, event));
}

int DefaultEventManager<Platform::SELECT>::numSocketEvents (
                                      const SocketHandle::Handle& handle) const
{
    return   isRegistered(handle, EventType::e_READ)
           + isRegistered(handle, EventType::e_WRITE)
           + isRegistered(handle, EventType::e_ACCEPT)
           + isRegistered(handle, EventType::e_CONNECT);
}

}  // close package namespace
}  // close enterprise namespace

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
