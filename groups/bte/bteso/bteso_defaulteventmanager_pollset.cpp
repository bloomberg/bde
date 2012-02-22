// bteso_defaulteventmanager_pollset.cpp                              -*-C++-*-
#include <bteso_defaulteventmanager_pollset.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_defaulteventmanager_pollset_cpp,"$Id$ $CSID$")

#if defined(BSLS_PLATFORM__OS_AIX)

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

#include <bcemt_threadutil.h>

#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bslmf_assert.h>
#include <bsls_assert.h>

#include <bsl_vector.h>
#include <bsl_utility.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <bsl_c_errno.h>

#ifdef BSLS_PLATFORM__OS_AIX
//    #include <sys/timers.h>       // timespec

    // 'NO_TIMEOUT' and 'INF_TIMEOUT' are defined in <sys/poll.h>

    #define BTESO_EVENTMANAGERIMP_POLL_NO_TIMEOUT  NO_TIMEOUT
    #define BTESO_EVENTMANAGERIMP_POLL_INF_TIMEOUT INF_TIMEOUT
#endif

#include <bsl_cstring.h>

namespace BloombergLP {

                             // ---------------
                             // PRIVATE METHODS
                             // ---------------

int bteso_DefaultEventManager<bteso_Platform::POLLSET>::dispatchCallbacks(
                                                         int numSignaled) const
    // using bitwise '|'s between bools rather than '||' for speed
{
    int numCallbacks = 0;

    const CallbackMap::const_iterator cbEnd = d_callbacks.end();
    CallbackMap::const_iterator cbIt;

    for (int i = 0; i < numSignaled; ++i) {
        const struct ::pollfd& currData = d_signaled[i];

        // READ/ACCEPT.

        enum { DEFAULT_MASK = POLLERR | POLLHUP | POLLNVAL };
        if (currData.revents & (POLLIN | DEFAULT_MASK)) {
            if      (cbEnd != (cbIt = d_callbacks.find(
                    bteso_Event(currData.fd, bteso_EventType::BTESO_READ)))) {
                (cbIt->second)();
                ++numCallbacks;
            }
            else if (cbEnd != (cbIt = d_callbacks.find(
                    bteso_Event(currData.fd, bteso_EventType::BTESO_ACCEPT)))){
                (cbIt->second)();
                ++numCallbacks;
            }
        }

        // WRITE/CONNECT.

        if (currData.revents & (POLLOUT | DEFAULT_MASK)) {

            if      (cbEnd != (cbIt = d_callbacks.find(
                   bteso_Event(currData.fd, bteso_EventType::BTESO_WRITE)))) {
                (cbIt->second)();
                ++numCallbacks;
            }
            else if (cbEnd != (cbIt = d_callbacks.find(
                   bteso_Event(currData.fd, bteso_EventType::BTESO_CONNECT)))){
                (cbIt->second)();
                ++numCallbacks;
            }
        }
    }

    return numCallbacks;
}
                             // --------
                             // CREATORS
                             // --------

bteso_DefaultEventManager<bteso_Platform::POLLSET>::
bteso_DefaultEventManager(bteso_TimeMetrics *timeMetric,
                          bslma_Allocator   *basicAllocator)
: d_ps(::pollset_create(-1))
, d_fdCount(0)
, d_callbacks(basicAllocator)
, d_timeMetric_p(timeMetric)
, d_signaled(basicAllocator)
{
}

bteso_DefaultEventManager<bteso_Platform::POLLSET>::
~bteso_DefaultEventManager()
{
}

                             // ------------
                             // MANIPULATORS
                             // ------------

int bteso_DefaultEventManager<bteso_Platform::POLLSET>::
                                     dispatch(const bdet_TimeInterval& timeout,
                                              int                      flags)
{
    bdet_TimeInterval now(bdetu_SystemTime::now());
    errno = 0;             // note that on all Unix, 'errno' is thread-specific

    if (!numEvents()) {
        if (timeout <= now) {
            return 0;                                                 // RETURN
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
                // rfds = ::poll(&d_pollFds.front(), d_pollFds.size(),
                //               relativeTimeout);

                rfds = ::pollset_poll(d_ps, &d_signaled[0], d_fdCount,
                                      relativeTimeout);
                savedErrno = errno;
                d_timeMetric_p->switchTo(bteso_TimeMetrics::BTESO_CPU_BOUND);
            }
            else {
                // rfds = ::poll(&d_pollFds.front(), d_pollFds.size(),
                //               relativeTimeout);

                rfds = ::pollset_poll(d_ps, &d_signaled[0], d_fdCount,
                                      relativeTimeout);
                savedErrno = errno;
            }
            errno = 0;
            now = bdetu_SystemTime::now();
        } while ((0 > rfds && EINTR == savedErrno)
                && !(bteso_Flag::BTESO_ASYNC_INTERRUPT & flags)
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

        now = bdetu_SystemTime::now();
    } while (0 == numCallbacks && now < timeout);

    return numCallbacks;
}

int bteso_DefaultEventManager<bteso_Platform::POLLSET>::dispatch(int flags)
{
    if (!numEvents()) {
        return 0;                                                     // RETURN
    }

    int numCallbacks = 0;                    // number of callbacks dispatched
    errno = 0;             // note that on all Unix, 'errno' is thread-specific

    while  (0 == numCallbacks) {
        int rfds;                    // number of returned fds
        int savedErrno = 0;          // saved errno value set by 'poll'
        do {
            if (d_timeMetric_p) {
                d_timeMetric_p->switchTo(bteso_TimeMetrics::BTESO_IO_BOUND);
                rfds = ::pollset_poll(d_ps, &d_signaled[0], d_fdCount,
                                       BTESO_EVENTMANAGERIMP_POLL_INF_TIMEOUT);
                savedErrno = errno;
                d_timeMetric_p->switchTo(bteso_TimeMetrics::BTESO_CPU_BOUND);
            }
            else {
                rfds = ::pollset_poll(d_ps, &d_signaled[0], d_fdCount,
                                       BTESO_EVENTMANAGERIMP_POLL_INF_TIMEOUT);
                savedErrno = errno;
            }
            errno = 0;
        } while (
            (0 > rfds && EINTR == savedErrno)
                 && !(bteso_Flag::BTESO_ASYNC_INTERRUPT & flags));

        if (0 >= rfds) {
            return 0 == rfds
                   ? 0
                   : -1 == rfds && EINTR == savedErrno
                     ? -1
                     : savedErrno < 2
                       ? -10000
                       : -savedErrno;                                 // RETURN
        }

        numCallbacks += dispatchCallbacks(rfds);
    }

    return numCallbacks;
}

int bteso_DefaultEventManager<bteso_Platform::POLLSET>::registerSocketEvent(
                              const bteso_SocketHandle::Handle&   socketHandle,
                              const bteso_EventType::Type         eventType,
                              const bteso_EventManager::Callback& callback)
{
    int rc;

    // Determine from 'd_callbacks' if the event has already been registered.

    bteso_Event event(socketHandle, eventType);
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
    ::pollfd queryPollfd;
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
        d_signaled.resize(d_fdCount);
    }

    switch (eventType) {
      case bteso_EventType::BTESO_ACCEPT: {
        // No other event type can be registered simultaneously with ACCEPT.

        BSLS_ASSERT(0 == eventMask);
        eventMask |= POLLIN;
      } break;
      case bteso_EventType::BTESO_READ: {
        // Only WRITE can be registered simultaneously with READ.

        BSLS_ASSERT(0 == (eventMask & ~POLLOUT));
        BSLS_ASSERT_SAFE(!eventMask || !d_callbacks.count(
                   bteso_Event(socketHandle, bteso_EventType::BTESO_CONNECT)));
        eventMask |= POLLIN;
      } break;
      case bteso_EventType::BTESO_CONNECT: {
        // No other event type can be registered simultaneously with CONNECT.

        BSLS_ASSERT(0 == eventMask);
        eventMask |= POLLOUT;
      } break;
      case bteso_EventType::BTESO_WRITE: {
        // Only READ can be registered simultaneously with WRITE.

        BSLS_ASSERT(0 == (eventMask & ~POLLIN));
        BSLS_ASSERT_SAFE(!eventMask || !d_callbacks.count(
                    bteso_Event(socketHandle, bteso_EventType::BTESO_ACCEPT)));
        eventMask |= POLLOUT;

        //#ifdef BDES_PLATFORM__OS_FREEBSD
        //bteso_SocketOptUtil::setOption(socketHandle,
        //                            bteso_SocketOptUtil::BTESO_SOCKETLEVEL,
        //                            bteso_SocketOptUtil::BTESO_SENDLOWATER,
        //                            1);
        //#endif

      } break;
      default: {
        BSLS_ASSERT("Invalid event type (must be unreachable by design)" && 0);
        return -1;                                                    // RETURN
      } break;
    }

    ctl.events = eventMask;
    rc = ::pollset_ctl(d_ps, &ctl, 1);
    BSLS_ASSERT_OPT(0 == rc && "rc = ::pollset_ctl(d_ps, &ctl, 1);");

    return 0;
}

void bteso_DefaultEventManager<bteso_Platform::POLLSET>::deregisterSocketEvent(
                                const bteso_SocketHandle::Handle& socketHandle,
                                bteso_EventType::Type             eventType)
{
    int rc;

    // Determine from d_callbacks if the event is currently registered.

    bteso_Event event(socketHandle, eventType);
    CallbackMap::iterator cbIt = d_callbacks.find(event);
    BSLS_ASSERT(d_callbacks.end() != cbIt);
    d_callbacks.erase(cbIt);

    ::pollfd queryPollfd;
    queryPollfd.fd = socketHandle;
    rc = ::pollset_query(d_ps, &queryPollfd);
    BSLS_ASSERT(1 == rc);

    // Translate the type of event.

    switch (eventType) {
      case bteso_EventType::BTESO_ACCEPT:
      case bteso_EventType::BTESO_READ: {
        queryPollfd.events &= ~POLLIN;
      } break;
      case bteso_EventType::BTESO_CONNECT:
      case bteso_EventType::BTESO_WRITE: {
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
    BSLS_ASSERT(0 == rc);

    if (queryPollfd.events) {
        // still some events on this fd, have to add it back

        ctl.cmd = PS_ADD;
        ctl.events = queryPollfd.events;

        rc = ::pollset_ctl(d_ps, &ctl, 1);
        BSLS_ASSERT(0 == rc);
    }
    else {
        --d_fdCount;

        // d_signaled.resize(d_fdCount);  -- no point in this, we just need
        // to make sure d_signaled.size() >= d_fdCount
    }
}

int bteso_DefaultEventManager<bteso_Platform::POLLSET>::
               deregisterSocket(const bteso_SocketHandle::Handle& socketHandle)
{
    int rc;

    ::pollfd queryPollfd;
    queryPollfd.fd = socketHandle;
    rc = ::pollset_query(d_ps, &queryPollfd);
    if (0 == rc) {
        return 0;                                                     // RETURN
    }

    int numCallbacks = 0;

    if (queryPollfd.events & POLLIN) {
        if (d_callbacks.count(bteso_Event(
                                   socketHandle, bteso_EventType::BTESO_READ)))
        {
            deregisterSocketEvent(socketHandle, bteso_EventType::BTESO_READ);
            ++numCallbacks;
        }
        else {
            BSLS_ASSERT_SAFE(0 == (queryPollfd.events & POLLOUT));

            deregisterSocketEvent(socketHandle, bteso_EventType::BTESO_ACCEPT);
            ++numCallbacks;
        }
    }
    else {
        BSLS_ASSERT_SAFE(!d_callbacks.count(bteso_Event(
                                  socketHandle, bteso_EventType::BTESO_READ)));
    }

    if (queryPollfd.events & POLLOUT) {
        if (d_callbacks.count(bteso_Event(
                                socketHandle, bteso_EventType::BTESO_WRITE))) {
            deregisterSocketEvent(socketHandle, bteso_EventType::BTESO_WRITE);
            ++numCallbacks;
        }
        else {
            BSLS_ASSERT_SAFE(0 == (queryPollfd.events & POLLIN));

            deregisterSocketEvent(socketHandle,bteso_EventType::BTESO_CONNECT);
            ++numCallbacks;
        }
    }

    BSLS_ASSERT(numCallbacks >= 0);
    BSLS_ASSERT(numCallbacks <= 2);

    return numCallbacks;
}

void bteso_DefaultEventManager<bteso_Platform::POLLSET>::deregisterAll()
{
    d_callbacks.clear();
    int rc = ::pollset_destroy(d_ps);
    BSLS_ASSERT(0 == rc);
    d_ps = ::pollset_create(-1);
}

                             // ---------
                             // ACCESSORS
                             // ---------

bool bteso_DefaultEventManager<bteso_Platform::POLLSET>::
                                               hasLimitedSocketCapacity() const
{
    return false;
}

int bteso_DefaultEventManager<bteso_Platform::POLLSET>::numSocketEvents(
                          const bteso_SocketHandle::Handle& socketHandle) const
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

int bteso_DefaultEventManager<bteso_Platform::POLLSET>::numEvents() const
{
    return d_callbacks.size();
}

int bteso_DefaultEventManager<bteso_Platform::POLLSET>::
              isRegistered(const bteso_SocketHandle::Handle& handle,
                           const bteso_EventType::Type       eventType) const
{
    return 1 == d_callbacks.count(bteso_Event(handle, eventType));
}

}  // close namespace BloombergLP

#endif // AIX

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
