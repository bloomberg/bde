// bteso_defaulteventmanager_poll.cpp                                 -*-C++-*-
#include <bteso_defaulteventmanager_poll.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_defaulteventmanager_poll_cpp,"$Id$ $CSID$")

#ifdef BTE_FOR_TESTING_ONLY
// These dependencies need to be here for the the bde_build.pl script to
// generate the proper makefiles, but do not need to be compiled into the
// component's .o file.  The symbol BTE_FOR_TESTING_ONLY should remain
// undefined, and is here only because '#if 0' is optimized away by the
// bde_build.pl script.

#include <bteso_eventmanagertester.h>           // for testing only
#include <bteso_ioutil.h>                       // for testing only
#include <bteso_socketimputil.h>                // for testing only
#include <bteso_socketoptutil.h>                // for testing only
#endif

#include <bteso_flag.h>
#include <bteso_socketoptutil.h>
#include <bteso_timemetrics.h>

#include <bcemt_thread.h>
#include <bsls_assert.h>
#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bsls_assert.h>

#include <bsl_vector.h>
#include <bsl_utility.h>

#if defined(BSLS_PLATFORM__OS_SOLARIS)    \
    || defined(BSLS_PLATFORM__OS_LINUX)   \
    || defined(BDES_PLATFORM__OS_FREEBSD) \
    || defined(BSLS_PLATFORM__OS_AIX)     \
    || defined(BSLS_PLATFORM__OS_HPUX)    \
    || defined(BSLS_PLATFORM__OS_CYGWIN)

#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <bsl_c_errno.h>

#ifdef BSLS_PLATFORM__OS_AIX
    #include <sys/timers.h>       // timespec
    #define BTESO_EVENTMANAGERIMP_POLL_NO_TIMEOUT  NO_TIMEOUT
    #define BTESO_EVENTMANAGERIMP_POLL_INF_TIMEOUT INF_TIMEOUT
#elif defined(BSLS_PLATFORM__OS_LINUX) || defined(BSLS_PLATFORM__OS_CYGWIN)
    #define BTESO_EVENTMANAGERIMP_POLL_NO_TIMEOUT  0
    #define BTESO_EVENTMANAGERIMP_POLL_INF_TIMEOUT -1
#else
    #ifdef BSLS_PLATFORM__OS_SOLARIS
    #include <sys/stropts.h>      // INFTIM
    #endif
    #define BTESO_EVENTMANAGERIMP_POLL_NO_TIMEOUT  0
    #define BTESO_EVENTMANAGERIMP_POLL_INF_TIMEOUT INFTIM
#endif

#include <bsl_cstring.h>

namespace BloombergLP {

                             // ------------------
                             // FILE-SCOPE STATICS
                             // ------------------

typedef bsl::hash_map<bteso_Event,
                      bteso_EventManager::Callback,
                      bteso_EventHash>              CallbackMap;
typedef bsl::hash_map<int,int>                      IndexMap;

static inline int dispatchCallbacks(bsl::vector<struct ::pollfd>  *tmp,
                                    bsl::vector<struct ::pollfd>&  pollFds,
                                    CallbackMap                   *callbacks)
{
    BSLS_ASSERT(tmp);
    BSLS_ASSERT(0 == tmp->size()); // Internal invariant

    int numCallbacks = 0;

    // Implementation note: We need to make a local copy of signaled the
    // sockets because an invoked callback can potentially modify 'pollFds'
    // array thereby corrupting the iteration order.  In the worst case, a
    // full copy of 'pollFds' is made.

    int numSockets = pollFds.size();
    for (int i = 0; i < numSockets; ++i) {
        const struct ::pollfd& data = pollFds[i];
        if (0 != data.revents) {
            tmp->push_back(data);
        }
    }

    numSockets = tmp->size();
    for (int i = 0; i < numSockets; ++i) {
        const struct ::pollfd& currData = (*tmp)[i];

        // READ/ACCEPT.

        enum { DEFAULT_MASK = POLLERR | POLLHUP | POLLNVAL };
        if (currData.revents & (POLLIN | DEFAULT_MASK)) {
            CallbackMap::iterator  callbackIt, cbEnd = callbacks->end();
            if (cbEnd != (callbackIt = callbacks->find(
                   bteso_Event(currData.fd, bteso_EventType::BTESO_READ)))
             || cbEnd != (callbackIt = callbacks->find(
                   bteso_Event(currData.fd, bteso_EventType::BTESO_ACCEPT)))) {
                (callbackIt->second)();
                ++numCallbacks;
            }
        }

        // WRITE/CONNECT.

        if (currData.revents & (POLLOUT | DEFAULT_MASK)) {
            CallbackMap::iterator  callbackIt, cbEnd = callbacks->end();
            if (cbEnd != (callbackIt = callbacks->find(
                  bteso_Event(currData.fd, bteso_EventType::BTESO_WRITE)))
             || cbEnd != (callbackIt = callbacks->find(
                  bteso_Event(currData.fd, bteso_EventType::BTESO_CONNECT)))) {
                (callbackIt->second)();
                ++numCallbacks;
            }
        }
    }
    tmp->clear();
    return numCallbacks;
}

                             // --------
                             // CREATORS
                             // --------

bteso_DefaultEventManager<bteso_Platform::POLL>::bteso_DefaultEventManager(
                                            bteso_TimeMetrics *timeMetric,
                                            bslma_Allocator   *basicAllocator)
: d_pollFds(basicAllocator)
, d_callbacks(basicAllocator)
, d_index(basicAllocator)
, d_timeMetric_p(timeMetric)
, d_signaled(basicAllocator)
{
}

bteso_DefaultEventManager<bteso_Platform::POLL>::~bteso_DefaultEventManager()
{
    BSLS_ASSERT(0 == d_signaled.size());  // internal invariant
}

                             // ------------
                             // MANIPULATORS
                             // ------------

int bteso_DefaultEventManager<bteso_Platform::POLL>::dispatch(
                               const bdet_TimeInterval&         timeout,
                               int flags)
{
    bdet_TimeInterval now(bdetu_SystemTime::now());

    if (!numEvents()) {
        if (timeout <= now) {
            return 0;
        }
        while (timeout > now) {
            bdet_TimeInterval currTimeout(timeout - now);
            struct timespec ts;
            ts.tv_sec = currTimeout.seconds();
            ts.tv_nsec = currTimeout.nanoseconds();

            // Sleep till it's time.
            if (d_timeMetric_p) {
                d_timeMetric_p->switchTo(bteso_TimeMetrics::BTESO_IO_BOUND);
                bcemt_ThreadUtil::microSleep(ts.tv_nsec / 1000, ts.tv_sec);
                d_timeMetric_p->switchTo(bteso_TimeMetrics::BTESO_CPU_BOUND);
            }
            else {
                bcemt_ThreadUtil::microSleep(ts.tv_nsec / 1000, ts.tv_sec);
            }

            now = bdetu_SystemTime::now();
        }
        return 0;
    }

    int numCallbacks = 0;                    // number of callbacks dispatched

    do {
        int relativeTimeout;
        int rfds;                    // number of returned sockets
        int savedErrno = 0;          // saved errno value set by poll
        do {

            if (timeout > now) {
                // Calculate the time remaining for the poll() call.
                bdet_TimeInterval currTimeout(timeout - now);

                // Convert this timeout to a 32 bit value in milliseconds.
                relativeTimeout = currTimeout.seconds() * 1000
                          + currTimeout.nanoseconds() / 1000000 + 1;
                BSLS_ASSERT(relativeTimeout >= 0);
            }
            else {
                relativeTimeout = BTESO_EVENTMANAGERIMP_POLL_NO_TIMEOUT;
            }

            if (d_timeMetric_p) {
                d_timeMetric_p->switchTo(bteso_TimeMetrics::BTESO_IO_BOUND);
                rfds = ::poll(&d_pollFds.front(), d_pollFds.size(),
                              relativeTimeout);
                savedErrno = errno;
                d_timeMetric_p->switchTo(bteso_TimeMetrics::BTESO_CPU_BOUND);
            }
            else {
                rfds = ::poll(&d_pollFds.front(), d_pollFds.size(),
                              relativeTimeout);
                savedErrno = errno;
            }
            errno = 0;
            now = bdetu_SystemTime::now();
        } while (
            (0 > rfds && EINTR == savedErrno)
            && !(bteso_Flag::BTESO_ASYNC_INTERRUPT & flags)
            && now < timeout);

        if (0 >= rfds) {
            return rfds
                   ? -1 == rfds && EINTR == savedErrno
                     ? -1
                     : -savedErrno
                   : 0;
        }

        numCallbacks += dispatchCallbacks(&d_signaled,
                                           d_pollFds,
                                          &d_callbacks);
        now = bdetu_SystemTime::now();
    } while (0 == numCallbacks && now < timeout);

    return numCallbacks;
}

int bteso_DefaultEventManager<bteso_Platform::POLL>::dispatch(int flags)
{
    if (!numEvents()) {
        return 0;
    }

    int numCallbacks = 0;                    // number of callbacks dispatched

    while  (0 == numCallbacks) {
        int rfds;                    // number of returned fds
        int savedErrno = 0;          // saved errno value set by 'poll'
        do {
            if (d_timeMetric_p) {
                d_timeMetric_p->switchTo(bteso_TimeMetrics::BTESO_IO_BOUND);
                rfds = ::poll(&d_pollFds.front(), d_pollFds.size(),
                              BTESO_EVENTMANAGERIMP_POLL_INF_TIMEOUT);
                savedErrno = errno;
                d_timeMetric_p->switchTo(bteso_TimeMetrics::BTESO_CPU_BOUND);
            }
            else {
                rfds = ::poll(&d_pollFds.front(), d_pollFds.size(),
                              BTESO_EVENTMANAGERIMP_POLL_INF_TIMEOUT);
                savedErrno = errno;
            }
            errno = 0;
        } while (
            (0 > rfds && EINTR == savedErrno)
                 && !(bteso_Flag::BTESO_ASYNC_INTERRUPT & flags));

        if (0 >= rfds) {
            return rfds
                   ? -1 == rfds && EINTR == savedErrno
                     ? -1
                     : -savedErrno
                   : 0;
        }

        numCallbacks += dispatchCallbacks(&d_signaled,
                                           d_pollFds,
                                          &d_callbacks);
    }
    return numCallbacks;
}

int bteso_DefaultEventManager<bteso_Platform::POLL>::registerSocketEvent(
            const bteso_SocketHandle::Handle&              socketHandle,
            const bteso_EventType::Type                    event,
            const bteso_EventManager::Callback&            callback)
{
    // Determine from 'd_callbacks' if the event has already been registered.
    bteso_Event handleEvent(socketHandle, event);
    CallbackMap::iterator cbIt = d_callbacks.find(handleEvent);
    if (d_callbacks.end() != cbIt) {
        cbIt->second = callback;
        return 0;
    }

    // Map the socket handle to the corresponding index into 'd_index'.
    IndexMap::iterator indexIt = d_index.find(socketHandle);
    short eventmask = (d_index.end() != indexIt)
                    ? d_pollFds[indexIt->second].events
                    : 0;

    switch (event) {
        // No other event can be registered simultaneously with ACCEPT.
      case bteso_EventType::BTESO_ACCEPT:
        BSLS_ASSERT(0 == eventmask);
        eventmask |= POLLIN;
        break;

        // Only WRITE can be registered simultaneously with READ.
      case bteso_EventType::BTESO_READ:
        BSLS_ASSERT(0 == (eventmask & ~POLLOUT));
        eventmask |= POLLIN;
        break;

        // No other event can be registered simultaneously with CONNECT.
      case bteso_EventType::BTESO_CONNECT:
        BSLS_ASSERT(0 == eventmask);
        eventmask |= POLLOUT;
        break;

        // Only READ can be registered simultaneously with WRITE.
      case bteso_EventType::BTESO_WRITE:
        BSLS_ASSERT(0 == (eventmask & ~POLLIN));
        eventmask |= POLLOUT;
//#ifdef BDES_PLATFORM__OS_FREEBSD
//        bteso_SocketOptUtil::setOption(socketHandle,
//                                      bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
//                                      bteso_SocketOptUtil::BTESO_SENDLOWATER,
//                                      1);
//#endif
        break;

      default:
        BSLS_ASSERT("Invalid event (must be unreachable by design)" && 0);
        return -1;
    }

    if (d_index.end() != indexIt) {
        d_pollFds[indexIt->second].events = eventmask;
    }
    else {
        int index(d_pollFds.size());

        struct ::pollfd currfd;
        bsl::memset(&currfd, 0, sizeof(currfd)); // make purify/ZeroFault happy
        currfd.fd = socketHandle;
        currfd.events = eventmask;
        d_pollFds.push_back(currfd);

        bool insertedIndex = d_index.insert(
                                   bsl::make_pair(socketHandle, index)).second;
        BSLS_ASSERT(insertedIndex);
    }

    // Register the handle/event with d_callbacks.
    bool insertedEvent = d_callbacks.insert(
                                 bsl::make_pair(handleEvent, callback)).second;
    BSLS_ASSERT(insertedEvent);
    return 0;
}

void bteso_DefaultEventManager<bteso_Platform::POLL>::deregisterSocketEvent(
            const bteso_SocketHandle::Handle& socketHandle,
            bteso_EventType::Type             event)
{
    // Determine from d_callbacks if the event is currently registered.
    bteso_Event  handleEvent(socketHandle, event);
    CallbackMap::iterator cbIt = d_callbacks.find(handleEvent);
    BSLS_ASSERT(d_callbacks.end() != cbIt);

    // Translate the type of event.
    short pollevent(0);
    switch (event) {
      case bteso_EventType::BTESO_ACCEPT:
      case bteso_EventType::BTESO_READ:
        pollevent = POLLIN;
        break;
      case bteso_EventType::BTESO_CONNECT:
      case bteso_EventType::BTESO_WRITE:
        pollevent = POLLOUT;
        break;
      default:
        BSLS_ASSERT("Invalid event (must be unreachable)" && 0);
        return;
    }

    // Retrieve the '::pollfd' corresponding to 'socketHandle'.
    IndexMap::iterator indexIt = d_index.find(socketHandle);
    BSLS_ASSERT(d_index.end() != indexIt);

    int  index = indexIt->second;

    // Retrieve '::pollfd' from 'index'.
    struct ::pollfd& currfd = d_pollFds[index];

    // Ensure that the bit for the specified event is set.
    short eventmask = currfd.events;
    BSLS_ASSERT(eventmask & pollevent);

    // Create new eventmask by clearing the bit.
    eventmask &= ~pollevent;

    int rc;
    if (eventmask) {
        currfd.events = eventmask;
    }
    else {
        // Remove this fd from d_pollFds.
        int lastIndex = d_pollFds.size() - 1;
        if (lastIndex != index) {

            // Retrieve index information for this last entry.
            IndexMap::iterator lastIndexIt = d_index.find(
                                                      d_pollFds[lastIndex].fd);
            BSLS_ASSERT(d_index.end() != lastIndexIt &&
                                             lastIndexIt->second == lastIndex);

            // Move information from the last handle to the current one.
            currfd = d_pollFds[lastIndex];

            // Update the index information for this fd.
            lastIndexIt->second = index;
        }
        // Shorten the array.
        d_pollFds.resize(d_pollFds.size() - 1);

        // Delete index information.
        rc = d_index.erase(socketHandle);
        BSLS_ASSERT(1 == rc);
    }
    // Remove the callback for this handle/event.
    rc = d_callbacks.erase(handleEvent);
    BSLS_ASSERT(1 == rc);
}

int bteso_DefaultEventManager<bteso_Platform::POLL>::deregisterSocket(
        const bteso_SocketHandle::Handle& socketHandle)
{
    int numCallbacks = 0;
    if (d_callbacks.end() != d_callbacks.find(bteso_Event(
                            socketHandle, bteso_EventType::BTESO_READ)))
    {
        deregisterSocketEvent(socketHandle, bteso_EventType::BTESO_READ);
        ++numCallbacks;
    }
    if (d_callbacks.end() != d_callbacks.find(bteso_Event(
                            socketHandle, bteso_EventType::BTESO_WRITE)))
    {
        deregisterSocketEvent(socketHandle, bteso_EventType::BTESO_WRITE);
        return ++numCallbacks;
    }
    if (d_callbacks.end() != d_callbacks.find(bteso_Event(
                            socketHandle, bteso_EventType::BTESO_ACCEPT)))
    {
        deregisterSocketEvent(socketHandle, bteso_EventType::BTESO_ACCEPT);
        return ++numCallbacks;
    }
    if (d_callbacks.end() != d_callbacks.find(bteso_Event(
                            socketHandle, bteso_EventType::BTESO_CONNECT)))
    {
        deregisterSocketEvent(socketHandle, bteso_EventType::BTESO_CONNECT);
        return ++numCallbacks;
    }
    return numCallbacks;
}

void bteso_DefaultEventManager<bteso_Platform::POLL>::deregisterAll()
{
    d_pollFds.clear();
    d_callbacks.clear();
    d_index.clear();
}

                             // ---------
                             // ACCESSORS
                             // ---------

int bteso_DefaultEventManager<bteso_Platform::POLL>::numSocketEvents(
        const bteso_SocketHandle::Handle& socketHandle) const
{
    IndexMap::const_iterator indexIt = d_index.find(socketHandle);
    return d_index.end() != indexIt
           ? 1 + ((POLLIN | POLLOUT) == d_pollFds[indexIt->second].events)
           : 0;
}

int bteso_DefaultEventManager<bteso_Platform::POLL>::numEvents() const
{
    return d_callbacks.size();
}

int bteso_DefaultEventManager<bteso_Platform::POLL>::isRegistered(
        const bteso_SocketHandle::Handle& handle,
        const bteso_EventType::Type       event) const
{
    return d_callbacks.end() != d_callbacks.find(bteso_Event(handle, event));
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
