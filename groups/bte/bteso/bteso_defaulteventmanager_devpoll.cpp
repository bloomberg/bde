// bteso_defaulteventmanager_devpoll.cpp                              -*-C++-*-
#include <bteso_defaulteventmanager_devpoll.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_defaulteventmanager_devpoll_cpp,"$Id$ $CSID$")

#ifdef BTE_FOR_TESTING_ONLY
// These dependencies need to be here for the the bde_build.pl script to
// generate the proper makefiles, but do not need to be compiled into the
// component's .o file.  The symbol BTE_FOR_TESTING_ONLY should remain
// undefined, and is here only because '#if 0' is optimized away by the
// bde_build.pl script.

#include <bteso_eventmanagertester.h>           // for testing only
#include <bteso_socketimputil.h>                // for testing only
#include <bteso_socketoptutil.h>                // for testing only
#endif

#include <bteso_timemetrics.h>
#include <bteso_flag.h>

#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>
#include <bsls_assert.h>

#if defined(BSLS_PLATFORM__OS_SOLARIS)
#include <sys/devpoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <bsl_c_errno.h>
#include <bsl_cstring.h>
#include <bsl_utility.h>

namespace BloombergLP {

namespace {
    // unnamed namespace for private resources

    enum {
        MIN_IOCTL_TIMEOUT_MS = 333  // force spinning of devpoll every 333ms,
                                    // otherwise a bug on Solaris may cause
                                    // missing events if passing a longer
                                    // timeout to ioctl(fd, DP_POLL, ...)
    };

typedef bsl::hash_map<bteso_Event,
                      bteso_EventManager::Callback,
                      bteso_EventHash>                     CallbackMap;
typedef bsl::hash_map<int, int>                            EventmaskMap;

#if 0  // not used
static void printPollFds(const pollfd *data, int length) {
    BSLS_ASSERT(data);
    BSLS_ASSERT(0 <= length);
    for (int i = 0; i < length; ++i) {
        const pollfd& c = data[i];
        bsl::cout << "(" << c.fd << ", " << c.events << ", " << c.revents
                  << ") ";
    }
    bsl::cout << '\n';
}
#endif

static
inline int dispatchCallbacks(
    bsl::vector<struct ::pollfd>&                signaled,
    int                                          rfds,
    CallbackMap                                 *callbacks)
{
    int numCallbacks = 0;

    for (int i = 0; i < rfds; ++i) {
        const struct ::pollfd *currData = &signaled[i];
        BSLS_ASSERT(currData);
        BSLS_ASSERT(currData->revents);

        // Read/Accept.

        if (currData->revents & POLLIN) {
            CallbackMap::iterator cbit, cbend = callbacks->end();
            if (cbend != (cbit = callbacks->find(
                  bteso_Event(currData->fd, bteso_EventType::BTESO_READ)))
             || cbend != (cbit = callbacks->find(
                  bteso_Event(currData->fd, bteso_EventType::BTESO_ACCEPT)))) {
                cbit->second.operator()();
                ++numCallbacks;
                if ((bsl::size_t)i >= signaled.size()) {
                    // If deregisterAll was called in the callback (see DRQS
                    // 10105162), d_signaled is cleared and the loop must be
                    // exited.

                    break;
                }
            }
        }

        // The invocation of the previous callback may have reallocated
        // d_signaled.  Therefore, the same address cannot be used and must be
        // acquired again.

        currData = &signaled[i];

        // Write/Connect.

        if (currData->revents & POLLOUT) {
            CallbackMap::iterator  cbit, cbend = callbacks->end();
            if (cbend != (cbit = callbacks->find(
                 bteso_Event(currData->fd, bteso_EventType::BTESO_WRITE)))
             || cbend != (cbit = callbacks->find(
                 bteso_Event(currData->fd, bteso_EventType::BTESO_CONNECT)))) {
                cbit->second.operator()();
                ++numCallbacks;
                if ((bsl::size_t)i >= signaled.size()) {
                    // Same as above.

                    break;
                }
            }
        }
    }
    return numCallbacks;
}

} // close unnamed namespace

         // ---------------------------------------------------------
         // class bteso_DefaultEventManager<bteso_Platform::DEV_POLL>
         // ---------------------------------------------------------

// CREATORS
bteso_DefaultEventManager<bteso_Platform::DEVPOLL>::bteso_DefaultEventManager(
                                            bteso_TimeMetrics *timeMetric,
                                            bslma_Allocator   *basicAllocator)
: d_callbacks(basicAllocator)
, d_timeMetric_p(timeMetric)
, d_signaled(basicAllocator)
, d_eventmasks(basicAllocator)
, d_dpFd(open("/dev/poll", O_RDWR))
{

}

bteso_DefaultEventManager<bteso_Platform::DEVPOLL>::
                                                   ~bteso_DefaultEventManager()
{
    int rc = close(d_dpFd);
    BSLS_ASSERT(0 == rc);
}

// MANIPULATORS
int bteso_DefaultEventManager<bteso_Platform::DEVPOLL>::dispatch(
                               const bdet_TimeInterval& timeout,
                               int                      flags)
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
            int savedErrno;
            int rc;
            if (d_timeMetric_p) {
                d_timeMetric_p->switchTo(bteso_TimeMetrics::BTESO_IO_BOUND);
                rc = nanosleep(&ts, 0);
                savedErrno = errno;
                d_timeMetric_p->switchTo(bteso_TimeMetrics::BTESO_CPU_BOUND);
            }
            else {
                rc = nanosleep(&ts, 0);
                savedErrno = errno;
            }

            errno = 0;
            if (0 > rc) {
                if (EINTR == savedErrno) {
                    if (flags & bteso_Flag::BTESO_ASYNC_INTERRUPT) {
                        return -1;
                    }
                }
                else {
                    return -2;
                }
            }
            now = bdetu_SystemTime::now();
        }
        return 0;
    }

    int ncbs = 0;                    // number of callbacks dispatched

    do {
        int rfds;                    // number of returned sockets
        int savedErrno = 0;          // saved errno value set by poll
        do {
            int oldlength = d_signaled.size();
            d_signaled.resize(d_eventmasks.size());

            // This was added to fix a (very frequent) Purify UMR.  The cost is
            // insignificant.
            if (oldlength < (int) d_eventmasks.size()) {
                bsl::memset(&d_signaled[oldlength], 0,
                  sizeof(struct ::pollfd)*(d_eventmasks.size() - oldlength));
            }

            struct dvpoll dopoll;
            dopoll.dp_nfds = d_signaled.size();
            dopoll.dp_fds = &d_signaled.front();

            if (timeout < now) {
                // The ioctl() call should return immediately.
                dopoll.dp_timeout = 0;
            }
            else {
                // Calculate the time remaining for the ioctl() call.
                bdet_TimeInterval curr_timeout(timeout - now);
                // Convert this timeout to a 32 bit value in milliseconds.
                dopoll.dp_timeout =
                    curr_timeout.seconds() * 1000
                    + curr_timeout.nanoseconds()/1000000 + 1;
            }
            dopoll.dp_timeout = bsl::min(dopoll.dp_timeout,
                                         (int)MIN_IOCTL_TIMEOUT_MS);

            if (d_timeMetric_p) {
                d_timeMetric_p->switchTo(bteso_TimeMetrics::BTESO_IO_BOUND);
                rfds = ioctl(d_dpFd, DP_POLL, &dopoll);
                savedErrno = errno;
                d_timeMetric_p->switchTo(bteso_TimeMetrics::BTESO_CPU_BOUND);
            }
            else {
                rfds = ioctl(d_dpFd, DP_POLL, &dopoll);
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
                     : -2
                   : 0;
        }

        ncbs += dispatchCallbacks(d_signaled, rfds, &d_callbacks);
        now = bdetu_SystemTime::now();
    } while (0 == ncbs && now < timeout);

    return ncbs;
}

int bteso_DefaultEventManager<bteso_Platform::DEVPOLL>::dispatch(int flags)
{
    if (!numEvents()) {
        return 0;
    }

    int ncbs = 0;                    // number of callbacks dispatched
    while  (0 == ncbs) {
        int rfds;                    // number of returned fds
        int savedErrno = 0;          // saved errno value set by 'poll'

        int oldlength = d_signaled.size();
        d_signaled.resize(d_eventmasks.size());

        // This was added to fix a (very frequent) Purify UMR.  The cost is
        // insignificant.

        if (oldlength < (int) d_eventmasks.size()) {
            bsl::memset(&d_signaled[oldlength], 0,
                  sizeof(struct ::pollfd)*(d_eventmasks.size() - oldlength));
        }

        struct dvpoll dopoll;
        dopoll.dp_nfds = d_signaled.size();
        dopoll.dp_fds = &d_signaled.front();
        dopoll.dp_timeout = MIN_IOCTL_TIMEOUT_MS;

        do {
            if (d_timeMetric_p) {
                d_timeMetric_p->switchTo(bteso_TimeMetrics::BTESO_IO_BOUND);
                rfds = ioctl(d_dpFd, DP_POLL, &dopoll);
                savedErrno = errno;
                d_timeMetric_p->switchTo(bteso_TimeMetrics::BTESO_CPU_BOUND);
            }
            else {
                rfds = ioctl(d_dpFd, DP_POLL, &dopoll);
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
                     : -2
                   : 0;
        }

        ncbs += dispatchCallbacks(d_signaled, rfds, &d_callbacks);
    }
    return ncbs;
}

int bteso_DefaultEventManager<bteso_Platform::DEVPOLL>::registerSocketEvent(
                                  const bteso_SocketHandle::Handle&   handle,
                                  const bteso_EventType::Type         event,
                                  const bteso_EventManager::Callback& callback)
{
    // If 'event' is already registered overwrite callback.

    bteso_Event handleEvent(handle, event);
    CallbackMap::iterator cbIt = d_callbacks.find(handleEvent);
    if (d_callbacks.end() != cbIt) {
        cbIt->second = callback;
        return 0;
    }

    // If not, retrieve the handle from d_eventmasks.

    EventmaskMap::iterator eventmaskIt = d_eventmasks.find(handle);
    int  eventmask = (d_eventmasks.end() != eventmaskIt)
                   ? eventmaskIt->second
                   : 0;

    // Prepare a ::pollfd object to write to /dev/poll

    struct ::pollfd pfd;
    pfd.fd = handle;
    pfd.revents = 0;      // just to satisfy purify

    switch (event) {
        // No other event can be registered simultaneously with ACCEPT.
      case bteso_EventType::BTESO_ACCEPT: {
        BSLS_ASSERT(0 == eventmask);
        pfd.events = (eventmask |= POLLIN);
      } break;

        // No other event can be registered simultaneously with CONNECT.
      case bteso_EventType::BTESO_CONNECT: {
        BSLS_ASSERT(0 == eventmask);
        pfd.events = (eventmask |= POLLOUT);
      } break;

        // Only WRITE can be registered simultaneously with READ.
      case bteso_EventType::BTESO_READ: {
        BSLS_ASSERT(0 == (eventmask & ~POLLOUT));
        pfd.events = (eventmask |= POLLIN);
      } break;

        // Only READ can be registered simultaneously with WRITE.
      case bteso_EventType::BTESO_WRITE: {
        BSLS_ASSERT(0 == (eventmask & ~POLLIN));
        pfd.events = (eventmask |= POLLOUT);
      } break;

      default:
        BSLS_ASSERT("Invalid event (must be unreachable)" && 0);
        return -1;
    }

    // Write the new event mask for this fd to /dev/poll.

    int rc = write(d_dpFd, &pfd, sizeof(struct ::pollfd));
    if (rc != sizeof(struct ::pollfd)) {
        return errno;
    }

    if (d_eventmasks.end() == eventmaskIt) {
        // This socket handle is not registered yet.
        // Add the mapping from this handle to its eventmask.

        bool insertedMask = d_eventmasks.insert(
                                     bsl::make_pair(handle, eventmask)).second;
        BSLS_ASSERT(insertedMask);
    }
    else {
        // Update the contents of d_eventmasks.

        eventmaskIt->second = eventmask;
    }

    // Register the handle/event with d_callbacks.

    bool insertedEvent = d_callbacks.insert(
                                 bsl::make_pair(handleEvent, callback)).second;
    BSLS_ASSERT(insertedEvent);
    return 0;
}

void bteso_DefaultEventManager<bteso_Platform::DEVPOLL>::deregisterSocketEvent(
                                      const bteso_SocketHandle::Handle& handle,
                                      bteso_EventType::Type             event)
{
    // Determine from d_callbacks if the event is currently registered.

    bteso_Event  handleEvent(handle, event);
    CallbackMap::iterator cbIt = d_callbacks.find(handleEvent);
    BSLS_ASSERT(d_callbacks.end() != cbIt);

    // Translate the type of event.

    int pollevent = 0;
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

    // Retrieve the handle from d_eventmask.

    EventmaskMap::iterator eventmaskIt = d_eventmasks.find(handle);
    BSLS_ASSERT(d_eventmasks.end() != eventmaskIt &&
                                            (eventmaskIt->second & pollevent));

    // Clear the corresponding event bit to get the new eventmask.

    int eventmask = (eventmaskIt->second & ~pollevent);

    // Prepare a ::pollfd object to write to /dev/poll
    // First, we need to remove this socket handle from the set.
    // The write it out with a new mask, if applicable.

    struct ::pollfd pfd;
    pfd.fd = handle;
    pfd.events = POLLREMOVE;
    pfd.revents = 0;      // just to satisfy purify
    int rc = write(d_dpFd, &pfd, sizeof(struct ::pollfd));
    BSLS_ASSERT(sizeof(struct ::pollfd) == rc);

    if (eventmask) {
        // Write the new event mask for this fd to /dev/poll.

        pfd.events = eventmask;
        int rc = write(d_dpFd, &pfd, sizeof(struct ::pollfd));
        BSLS_ASSERT(sizeof(struct ::pollfd) == rc);
    }

    // Remove entry from d_callbacks.  Update event mask in 'd_eventmask'.

    rc = d_callbacks.erase(handleEvent);
    BSLS_ASSERT(1 == rc);

    // Delete the socket handle from 'd_eventmasks' if 'eventmask' is 0.

    if (0 == eventmask) {
        rc = d_eventmasks.erase(handle);
        BSLS_ASSERT(1 == rc);
    }
    else {
        eventmaskIt->second = eventmask;
    }
}

int bteso_DefaultEventManager<bteso_Platform::DEVPOLL>::deregisterSocket(
                                      const bteso_SocketHandle::Handle& handle)
{
    EventmaskMap::iterator eventmaskIt = d_eventmasks.find(handle);
    if (d_eventmasks.end() == eventmaskIt) {
        return 0;
    }

    struct ::pollfd req;
    req.fd = handle;
    req.events = POLLREMOVE;
    req.revents = 0;
    int rc = ::write(d_dpFd, &req, sizeof(struct ::pollfd));
    BSLS_ASSERT(sizeof(struct ::pollfd) == rc);

    int result = 0;
    if (eventmaskIt->second & POLLIN) {
        if (eventmaskIt->second & POLLOUT) {
            result += d_callbacks.erase(bteso_Event(
                                                 handle,
                                                 bteso_EventType::BTESO_READ));
            result += d_callbacks.erase(bteso_Event(
                                                handle,
                                                bteso_EventType::BTESO_WRITE));
        }
        else {
            // Either ACCEPT or READ.
            // Either CONNECT or WRITE.

            result += d_callbacks.erase(bteso_Event(
                                               handle,
                                               bteso_EventType::BTESO_ACCEPT));
            result += d_callbacks.erase(bteso_Event(
                                                 handle,
                                                 bteso_EventType::BTESO_READ));
        }
    }
    else {
        BSLS_ASSERT(eventmaskIt->second & POLLOUT);
        // Either CONNECT or WRITE.

        result += d_callbacks.erase(bteso_Event(
                                              handle,
                                              bteso_EventType::BTESO_CONNECT));
        result += d_callbacks.erase(bteso_Event(
                                                handle,
                                                bteso_EventType::BTESO_WRITE));
    }
    BSLS_ASSERT(0 < result);
    BSLS_ASSERT(2 >= result);
    d_eventmasks.erase(eventmaskIt);
    return result;
}

void bteso_DefaultEventManager<bteso_Platform::DEVPOLL>::deregisterAll() {

    int i = 0;
    d_signaled.resize(d_eventmasks.size());
    EventmaskMap::iterator endIt = d_eventmasks.end();
    for (EventmaskMap::iterator it = d_eventmasks.begin(); it != endIt; ++it) {
        d_signaled[i].fd = it->first;
        d_signaled[i].events = POLLREMOVE;
        d_signaled[i].revents = 0;
        ++i;
    }

    if (!d_signaled.empty()) {
        int rc = write(d_dpFd, &d_signaled.front(),
                       i * sizeof(struct ::pollfd));
        BSLS_ASSERT((int)(sizeof(struct ::pollfd) * i) == rc);
    }

    d_callbacks.clear();
    d_eventmasks.clear();
    d_signaled.clear();
}

                             // ---------
                             // ACCESSORS
                             // ---------

int bteso_DefaultEventManager<bteso_Platform::DEVPOLL>::numSocketEvents(
        const bteso_SocketHandle::Handle& handle) const
{
    EventmaskMap::const_iterator eventmaskIt = d_eventmasks.find(handle);
    return d_eventmasks.end() != eventmaskIt
         ? (1 + (eventmaskIt->second == (POLLIN | POLLOUT)))
         : 0;
}

int bteso_DefaultEventManager<bteso_Platform::DEVPOLL>::numEvents() const
{
    return d_callbacks.size();
}

int bteso_DefaultEventManager<bteso_Platform::DEVPOLL>::isRegistered(
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
