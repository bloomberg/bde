// bteso_defaulteventmanager_select.cpp -*-C++-*-
#include <bteso_defaulteventmanager_select.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_defaulteventmanager_select_cpp,"$Id$ $CSID$")

#ifdef BTE_FOR_TESTING_ONLY
// These dependencies need to be here for the the bde_build.pl script to
// generate the proper makefiles, but do not need to be compiled into the
// component's .o file.  The symbol BTE_FOR_TESTING_ONLY should remain
// undefined, and is here only because '#if 0' is optimized away by the
// bde_build.pl script.

#include <bteso_eventmanagertester.h>          // for testing only
#include <bteso_socketimputil.h>               // for testing only
#endif

#include <bteso_timemetrics.h>
#include <bteso_flag.h>

#include <bcemt_thread.h>

#include <bdetu_systemtime.h>

#include <bsls_assert.h>
#include <bsl_cstring.h>

using bsl::memset;

// EINTR does not have any exact equivalent on Windows, except WSAEINTR which
// indicates the call has been interrrupted with WSACancelBlockingCall().
// Also, Microsoft's errno.h contains an erroneous definition of EINTR which
// actually means ERROR_TOO_MANY_OPEN_FILES. Fixing EINTR definition here:

#ifdef BSLS_PLATFORM__OS_WINDOWS
#ifdef EINTR
#undef EINTR
#endif
#define EINTR WSAEINTR
#endif

namespace BloombergLP {

static bool compareFdSets(const fd_set& lhs, const fd_set& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' file descriptor sets are
    // equal, and 'false' otherwise.
{
#ifdef BTESO_PLATFORM__BSD_SOCKETS
    return !bsl::memcmp(&lhs, &rhs, sizeof(fd_set));                 // RETURN
#endif
#ifdef BTESO_PLATFORM__WIN_SOCKETS
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

bool
bteso_DefaultEventManager<bteso_Platform::SELECT>::checkInternalInvariants()
{
    EventMap::const_iterator it(d_events.begin()), end(d_events.end());
    fd_set readControl, writeControl, exceptControl;
    FD_ZERO(&readControl);
    FD_ZERO(&writeControl);
    FD_ZERO(&exceptControl);

    for (; it != end; ++it) {
        const bteso_Event& event = it->first;
        if (bteso_EventType::BTESO_READ == event.type()
         || bteso_EventType::BTESO_ACCEPT == event.type()) {
            FD_SET(event.handle(), &readControl);
        }
        else {
            FD_SET(event.handle(), &writeControl);
#ifdef BTESO_PLATFORM__WIN_SOCKETS
            FD_SET(event.handle(), &exceptControl);
#endif
        }
    }

    return compareFdSets(d_readSet, readControl)
        && compareFdSets(d_writeSet, writeControl)
        && compareFdSets(d_exceptSet, exceptControl);
}

static inline void copySet(fd_set *dst, const fd_set& src)
     // Assign to the specified 'dst' file descriptor set the value of the
     // specified 'src' file descriptor set.
{
#ifdef BTESO_PLATFORM__WIN_SOCKETS
    dst->fd_count = src.fd_count;
    bsl::memcpy(&dst->fd_array,
                &src.fd_array,
                dst->fd_count * sizeof(dst->fd_array[0]));
#else
    *dst = src;
#endif
}

static void convert(struct timeval           *result,
                    const bdet_TimeInterval&  value)
    // TBD: Do
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

inline
int bteso_DefaultEventManager<bteso_Platform::SELECT>::canBeRegistered(
                                      const bteso_SocketHandle::Handle& handle)
{
#ifdef BTESO_PLATFORM__BSD_SOCKETS
    return handle < FD_SETSIZE;
#endif
#ifdef BTESO_PLATFORM__WIN_SOCKETS
    return bsl::max(d_numRead, d_numWrite) < BTESO_MAX_NUM_HANDLES;
#endif
}

int bteso_DefaultEventManager<bteso_Platform::SELECT>::dispatchCallbacks(
                                                       int           numEvents,
                                                       const fd_set& readSet,
                                                       const fd_set& writeSet,
                                                       const fd_set& exceptSet)
{
    // Iterate through all the events to find out which file descriptors have
    // been set and 
    EventMap::iterator it(d_events.begin()), end(d_events.end());

    for (; numEvents > 0 && it != end; ++it) {
        const bteso_Event& event = it->first;
        if (bteso_EventType::BTESO_READ == event.type()
         || bteso_EventType::BTESO_ACCEPT == event.type()) {
            if (FD_ISSET(event.handle(), &readSet)) {
                d_signaledRead.push_back(it);
                --numEvents;
            }
        }
        else {
#ifdef BTESO_PLATFORM__WIN_SOCKETS
            if (FD_ISSET(event.handle(), &writeSet)
             || FD_ISSET(event.handle(), &exceptSet)) {
#else
            if (FD_ISSET(event.handle(), &writeSet)) {
#endif
                d_signaledWrite.push_back(it);
                --numEvents;
            }
        }
    }

    int numDispatched = 0;
    int numRead       = d_signaledRead.size();

    for (int i = 0; i < numRead; ++i) {
        EventMap::iterator callbackIt = d_signaledRead[i];
        if (d_events.end() != callbackIt) {
            ++numDispatched;
            (callbackIt->second)();
        }
    }

    int numWrite = d_signaledWrite.size();

    for (int i = 0; i < numWrite; ++i) {
        EventMap::iterator callbackIt = d_signaledWrite[i];
        if (d_events.end() != callbackIt) {
            ++numDispatched;
            (callbackIt->second)();
        }
    }

    // maintain internal invariants
    d_signaledRead.clear();
    d_signaledWrite.clear();
    return numDispatched;
}

            // =======================================================
            // class bteso_DefaultEventManager<bteso_Platform::SELECT>
            // =======================================================

// CREATORS
bteso_DefaultEventManager<bteso_Platform::SELECT>::bteso_DefaultEventManager(
                                               bslma_Allocator *basicAllocator)
: d_eventsAllocator(basicAllocator)
, d_events(&d_eventsAllocator)
, d_numRead(0)
, d_numWrite(0)
, d_maxFd(0)
, d_timeMetric(0)
, d_signaledRead(basicAllocator)
, d_signaledWrite(basicAllocator)
{
    FD_ZERO(&d_readSet);
    FD_ZERO(&d_writeSet);
    FD_ZERO(&d_exceptSet);

    d_signaledRead.reserve(BTESO_MAX_NUM_HANDLES);
    d_signaledWrite.reserve(BTESO_MAX_NUM_HANDLES);
}

bteso_DefaultEventManager<bteso_Platform::SELECT>::bteso_DefaultEventManager(
                                             bteso_TimeMetrics *timeMetric,
                                             bslma_Allocator   *basicAllocator)
: d_eventsAllocator(basicAllocator)
, d_events(&d_eventsAllocator)
, d_numRead(0)
, d_numWrite(0)
, d_maxFd(0)
, d_timeMetric(timeMetric)
, d_signaledRead(basicAllocator)
, d_signaledWrite(basicAllocator)
{
    FD_ZERO(&d_readSet);
    FD_ZERO(&d_writeSet);
    FD_ZERO(&d_exceptSet);

    d_signaledRead.reserve(BTESO_MAX_NUM_HANDLES);
    d_signaledWrite.reserve(BTESO_MAX_NUM_HANDLES);
}

bteso_DefaultEventManager<bteso_Platform::SELECT>::~bteso_DefaultEventManager()
{
    BSLS_ASSERT(checkInternalInvariants());
    BSLS_ASSERT(0 == d_signaledRead.size());
    BSLS_ASSERT(0 == d_signaledWrite.size());
    d_events.clear();
}

// MANIPULATORS
int bteso_DefaultEventManager<bteso_Platform::SELECT>::dispatch(int flags)
{
    if (0 == d_events.size()) {
        return 0;
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
            d_timeMetric->switchTo(bteso_TimeMetrics::BTESO_IO_BOUND);
            ret = ::select(d_maxFd, &readSet, &writeSet, &exceptSet, NULL);
            savedErrno = errno;
            d_timeMetric->switchTo(bteso_TimeMetrics::BTESO_CPU_BOUND);
        }
        else {
            ret = ::select(d_maxFd, &readSet, &writeSet, &exceptSet, NULL);
            savedErrno = errno;
        }
    } while (ret < 0
          && EINTR == savedErrno
          && !(flags & bteso_Flag::BTESO_ASYNC_INTERRUPT));

    if (ret < 0) {
        return EINTR == savedErrno ? -1 : ret;
    }

    BSLS_ASSERT(0 < ret);
    return dispatchCallbacks(ret, readSet, writeSet, exceptSet);
}

int bteso_DefaultEventManager<bteso_Platform::SELECT>::dispatch(
                                              const bdet_TimeInterval& timeout,
                                              int                      flags)
{
    int ret;
    fd_set readSet, writeSet, exceptSet;
    copySet(&readSet, d_readSet);
    copySet(&writeSet, d_writeSet);
    copySet(&exceptSet, d_exceptSet);

#ifdef BTESO_PLATFORM__WIN_SOCKETS
    enum {
       RESOLUTION_MS = 10,   // in milliseconds
       MILLISECOND   = 1000, // in microseconds
       INTERVAL      = RESOLUTION_MS * MILLISECOND,
       ADJUSTMENT    = INTERVAL  - 1
    };

    struct timeval tv;
    bdet_TimeInterval delta = timeout - bdetu_SystemTime::now();
    convert(&tv, delta);

    tv.tv_usec = (tv.tv_usec + ADJUSTMENT) / INTERVAL * INTERVAL;
    BSLS_ASSERT(0 == tv.tv_usec % INTERVAL);

    if (0 == d_events.size()) {
        DWORD timeoutMS = tv.tv_sec * 1000 + tv.tv_usec / 1000 + 1;
        if (d_timeMetric) {
            d_timeMetric->switchTo(bteso_TimeMetrics::BTESO_IO_BOUND);
            ::Sleep(timeoutMS);
            d_timeMetric->switchTo(bteso_TimeMetrics::BTESO_CPU_BOUND);
        }
        else {
            ::Sleep(timeoutMS);
        }
        ret = 0;
    }
    else {
        if (d_timeMetric) {
            d_timeMetric->switchTo(bteso_TimeMetrics::BTESO_IO_BOUND);
            ret = ::select(d_maxFd, &readSet, &writeSet, &exceptSet, &tv);
            d_timeMetric->switchTo(bteso_TimeMetrics::BTESO_CPU_BOUND);
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
        bdet_TimeInterval delta = timeout - bdetu_SystemTime::now();
        convert(&tv, delta);
        errno = 0;
        if (d_timeMetric) {
            d_timeMetric->switchTo(bteso_TimeMetrics::BTESO_IO_BOUND);
            ret = ::select(d_maxFd, &readSet, &writeSet, &exceptSet, &tv);
            savedErrno = errno;
            d_timeMetric->switchTo(bteso_TimeMetrics::BTESO_CPU_BOUND);
        }
        else {
            ret = ::select(d_maxFd, &readSet, &writeSet, &exceptSet, &tv);
            savedErrno = errno;
        }
    } while (ret < 0 && EINTR == savedErrno
          && !(flags & bteso_Flag::BTESO_ASYNC_INTERRUPT)
             #ifdef BSLS_PLATFORM__OS_LINUX
             // Linux select() returns at one tick *preceding* the expiration
             // of the timeout.  Since code usually expects that select() will
             // sleep a bit more than the timeout (i.e select() will return at
             // the tick following the expiration of the timeout), we will need
             // to sleep a bit more.  For more information, see
             // http://www.uwsg.iu.edu/hypermail/linux/kernel/0402.2/1636.html

          || (ret == 0 && (timeout - bdetu_SystemTime::now()) > 0)
             #endif
             );

    if (ret <= 0) {
        return savedErrno == EINTR ? -1 : ret;
    }
#endif
    BSLS_ASSERT(ret > 0);

    return dispatchCallbacks(ret, readSet, writeSet, exceptSet);
}

int bteso_DefaultEventManager<bteso_Platform::SELECT>::registerSocketEvent(
                                 const bteso_SocketHandle::Handle&   handle,
                                 const bteso_EventType::Type         eventType,
                                 const bteso_EventManager::Callback& callback)
{
    BSLS_ASSERT(canBeRegistered(handle));

    bteso_Event ev(handle, eventType);
    d_events[ev] = callback;

    if (eventType == bteso_EventType::BTESO_READ
     || eventType == bteso_EventType::BTESO_ACCEPT) {
        if (!FD_ISSET(handle, &d_readSet)) {
            FD_SET(handle, &d_readSet);
            ++d_numRead;
        }
    }
    else {
        if (!FD_ISSET(handle, &d_writeSet)) {
            FD_SET(handle, &d_writeSet);
#ifdef BTESO_PLATFORM__WIN_SOCKETS
            FD_SET(handle, &d_exceptSet);
#endif
            ++d_numWrite;
        }
    }
#ifdef BTESO_PLATFORM__BSD_SOCKETS
    if (handle >= d_maxFd) {
        d_maxFd = handle + 1;
    }
#endif
    return 0;
}

void bteso_DefaultEventManager<bteso_Platform::SELECT>::deregisterSocketEvent(
                                      const bteso_SocketHandle::Handle& handle,
                                      const bteso_EventType::Type       event)
{
    bteso_Event ev(handle, event);

    if (1 == d_events.erase(ev)) {
        if (event == bteso_EventType::BTESO_READ
         || event == bteso_EventType::BTESO_ACCEPT) {
            FD_CLR(handle, &d_readSet);
            --d_numRead;
        }
        else {
            FD_CLR(handle, &d_writeSet);
#ifdef BTESO_PLATFORM__WIN_SOCKETS
            FD_CLR(handle, &d_exceptSet);
#endif
            --d_numWrite;
        }
    }
}

int bteso_DefaultEventManager<bteso_Platform::SELECT>::deregisterSocket(
                                      const bteso_SocketHandle::Handle& handle)
{
#ifdef BTESO_PLATFORM__BSD_SOCKETS
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
    if (isRegistered(handle, bteso_EventType::BTESO_READ)) {
        deregisterSocketEvent(handle, bteso_EventType::BTESO_READ);
        ++ncbs;
    }
    if (isRegistered(handle, bteso_EventType::BTESO_WRITE)) {
        deregisterSocketEvent(handle, bteso_EventType::BTESO_WRITE);
        ++ncbs;
    }
    if (isRegistered(handle, bteso_EventType::BTESO_ACCEPT)) {
        deregisterSocketEvent(handle, bteso_EventType::BTESO_ACCEPT);
        ++ncbs;
    }
    if (isRegistered(handle, bteso_EventType::BTESO_CONNECT)) {
        deregisterSocketEvent(handle, bteso_EventType::BTESO_CONNECT);
        ++ncbs;
    }
    return ncbs;
}

void bteso_DefaultEventManager<bteso_Platform::SELECT>::deregisterAll()
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
bool bteso_DefaultEventManager<bteso_Platform::SELECT>::canRegisterSockets()
                                                                          const
{
#ifdef BTESO_PLATFORM__WIN_SOCKETS
    return bsl::max(d_numRead, d_numWrite) < BTESO_MAX_NUM_HANDLES;
#else
    return d_maxFd < BTESO_MAX_NUM_HANDLES;
#endif
}

int bteso_DefaultEventManager<bteso_Platform::SELECT>::isRegistered(
                                 const bteso_SocketHandle::Handle& handle,
                                 const bteso_EventType::Type       event) const
{
    return d_events.end() != d_events.find(bteso_Event(handle, event));
}

int bteso_DefaultEventManager<bteso_Platform::SELECT>::numSocketEvents (
                                const bteso_SocketHandle::Handle& handle) const
{
    return   isRegistered(handle, bteso_EventType::BTESO_READ)
           + isRegistered(handle, bteso_EventType::BTESO_WRITE)
           + isRegistered(handle, bteso_EventType::BTESO_ACCEPT)
           + isRegistered(handle, bteso_EventType::BTESO_CONNECT);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
