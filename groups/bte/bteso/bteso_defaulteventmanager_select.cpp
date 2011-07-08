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

typedef bsl::hash_map<bteso_Event,
                      bteso_EventManager::Callback,
                      bteso_EventHash>              EventMap;

static inline int internalInvariant(fd_set&          readSet,
                                    fd_set&          writeSet,
                                    fd_set&          exceptSet,
                                    const EventMap&  events)
    // Verify that every socket handle that is registered in the
    // specified 'events' is set in the appropriate set (e.g., either
    // 'readSet' or 'writeSet' depending on whether or not this is a
    // READ or WRITE event) and return 0 on success and a non-zero value
    // otherwise.
{
    EventMap::const_iterator iter(events.begin()), end(events.end());
    fd_set readControl, writeControl;
    FD_ZERO(&readControl);
    FD_ZERO(&writeControl);
    fd_set exceptControl;
    FD_ZERO(&exceptControl);

    while(end != iter) {
        const bteso_Event& event = iter->first;
        if (bteso_EventType::BTESO_READ == event.type() ||
                bteso_EventType::BTESO_ACCEPT == event.type())
        {
            FD_SET(event.handle(), &readControl);
        }
        else {
            FD_SET(event.handle(), &writeControl);
#ifdef BTESO_PLATFORM__WIN_SOCKETS
            FD_SET(event.handle(), &exceptControl);
#endif
        }
        ++iter;
    }
    return
        bteso_DefaultEventManager_SelectRaw::compareFdSets(readSet,
                                                           readControl) ||
        bteso_DefaultEventManager_SelectRaw::compareFdSets(writeSet,
                                                           writeControl) ||
        bteso_DefaultEventManager_SelectRaw::compareFdSets(exceptSet,
                                                           exceptControl);
}

static inline
void copySet(fd_set *dst, const fd_set& src)
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

static inline
void convert(struct timeval *result, const bdet_TimeInterval& value) {
    BSLS_ASSERT(result);

    result->tv_sec = value.seconds() > 0 ? (bsl::time_t)value.seconds()
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

                    // =========================================
                    // class bteso_DefaultEventManager_SelectRaw
                    // =========================================

int
bteso_DefaultEventManager_SelectRaw::compareFdSets(const fd_set& lhs,
                                                   const fd_set& rhs)
{
#ifdef BTESO_PLATFORM__BSD_SOCKETS
    return !!bsl::memcmp(&lhs, &rhs, sizeof(fd_set));
#endif
#ifdef BTESO_PLATFORM__WIN_SOCKETS
    if (lhs.fd_count != rhs.fd_count) {
        return 1;
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
            return 1;
        }
    }
    return 0;
#endif
}

int bteso_DefaultEventManager_SelectRaw::canBeRegistered(
       const bteso_SocketHandle::Handle& handle)
{
#ifdef BTESO_PLATFORM__BSD_SOCKETS
    return handle < FD_SETSIZE;
#endif
#ifdef BTESO_PLATFORM__WIN_SOCKETS
    return bsl::max(d_numRead, d_numWrite) < BTESO_MAX_NUM_HANDLES;
#endif
}

bteso_DefaultEventManager_SelectRaw::bteso_DefaultEventManager_SelectRaw(
        bteso_TimeMetrics *timeMetric,
        bslma_Allocator   *basicAllocator)
: d_events(basicAllocator)
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
    BSLS_ASSERT(0 == d_signaledRead.size());
    BSLS_ASSERT(0 == d_signaledWrite.size());

    d_signaledRead.reserve(BTESO_MAX_NUM_HANDLES);
    d_signaledWrite.reserve(BTESO_MAX_NUM_HANDLES);
}

bteso_DefaultEventManager_SelectRaw::~bteso_DefaultEventManager_SelectRaw()
{
    BSLS_ASSERT(0 == internalInvariant(d_readSet,
                                          d_writeSet,
                                          d_exceptSet,
                                          d_events));
    BSLS_ASSERT(0 == d_signaledRead.size());
    BSLS_ASSERT(0 == d_signaledWrite.size());
    d_events.clear();
}

// MANIPULATORS
int bteso_DefaultEventManager_SelectRaw::dispatch(int flags)
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
    } while (ret < 0 && savedErrno == EINTR &&
             !(flags & bteso_Flag::BTESO_ASYNC_INTERRUPT));

    if (ret < 0) {
        return savedErrno == EINTR ? -1 : ret;
    }
    BSLS_ASSERT(0 < ret);

    // Implementation note:
    // 'ret' contains the number of socket-events signaled.  We mark up
    // which are signaled and invoke the associated callbacks verifying
    // before each invocation, that a callback is still registered.

    EventMap::iterator it(d_events.begin()), end(d_events.end());

    while (0 < ret && end != it) {
        const bteso_Event &sev = it->first;
        if (bteso_EventType::BTESO_READ == sev.type() ||
            bteso_EventType::BTESO_ACCEPT == sev.type())
        {
            if (FD_ISSET(sev.handle(), &readSet)) {
                d_signaledRead.push_back(sev);
                --ret;
            }
        }
        else {
#ifdef BTESO_PLATFORM__WIN_SOCKETS
            if (FD_ISSET(sev.handle(), &writeSet) ||
                FD_ISSET(sev.handle(), &exceptSet))
#else
            if (FD_ISSET(sev.handle(), &writeSet))
#endif
            {
                d_signaledWrite.push_back(sev);
                --ret;
            }
        }
        ++it;
    }

    int numDispatched = 0;
    int numRead = d_signaledRead.size();

    for (int i = 0; i < numRead; ++i) {
        EventMap::iterator callbackIt = d_events.find(d_signaledRead[i]);
        if (d_events.end() != callbackIt) {
            ++numDispatched;
            (callbackIt->second)();
        }
    }

    int numWrite = d_signaledWrite.size();

    for (int i = 0; i < numWrite; ++i) {
        EventMap::iterator callbackIt = d_events.find(d_signaledWrite[i]);
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

int bteso_DefaultEventManager_SelectRaw::dispatch(
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
    } while (ret < 0 && savedErrno == EINTR &&
             !(flags & bteso_Flag::BTESO_ASYNC_INTERRUPT)
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

    EventMap::iterator it(d_events.begin()), end(d_events.end());

    while (ret > 0 && end != it) {
        const bteso_Event &sev = it->first;

        if (bteso_EventType::BTESO_READ == sev.type() ||
            bteso_EventType::BTESO_ACCEPT == sev.type())
        {
            if (FD_ISSET(sev.handle(), &readSet)) {
                d_signaledRead.push_back(sev);
                --ret;
            }
        }
        else {
#ifdef BTESO_PLATFORM__WIN_SOCKETS
            if (FD_ISSET(sev.handle(), &writeSet) ||
                FD_ISSET(sev.handle(), &exceptSet))
#else
            if (FD_ISSET(sev.handle(), &writeSet))
#endif
            {
                d_signaledWrite.push_back(sev);
                --ret;
            }
        }
        ++it;
    }

    int numDispatched = 0;
    int numRead = d_signaledRead.size();
    for (int i = 0; i < numRead; ++i) {
        EventMap::iterator callbackIt = d_events.find(d_signaledRead[i]);
        if (d_events.end() != callbackIt) {
            ++numDispatched;
            (callbackIt->second)();
        }
    }

    int numWrite = d_signaledWrite.size();
    for (int i = 0; i < numWrite; ++i) {
        EventMap::iterator callbackIt = d_events.find(d_signaledWrite[i]);
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

int bteso_DefaultEventManager_SelectRaw::registerSocketEvent(
        const bteso_SocketHandle::Handle&    handle,
        const bteso_EventType::Type          eventType,
        const bteso_EventManager::Callback&  cb)
{
    BSLS_ASSERT(
                 bteso_DefaultEventManager_SelectRaw::canBeRegistered(handle));

    bteso_Event ev(handle, eventType);
    d_events[ev] = cb;

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

void bteso_DefaultEventManager_SelectRaw::moveSocket(
                bteso_DefaultEventManager_SelectRaw *manager,
                const bteso_SocketHandle::Handle& handle)
{
    BSLS_ASSERT(manager);
    int numRegistered = manager->numSocketEvents(handle);
    BSLS_ASSERT(0 < numRegistered);
    if (FD_ISSET(handle, &manager->d_readSet)) {
        FD_CLR(handle, &manager->d_readSet);
        FD_SET(handle, &d_readSet);
        --manager->d_numRead;
        bteso_Event ev(handle, bteso_EventType::BTESO_READ);
        EventMap::iterator callbackIt = manager->d_events.find(ev);
        EventMap::iterator cbEnd = manager->d_events.end();
        if (cbEnd == callbackIt) {
            ev.setType(bteso_EventType::BTESO_ACCEPT);
            callbackIt = manager->d_events.find(ev);
            BSLS_ASSERT(cbEnd != callbackIt);
            d_events.insert(*callbackIt);
            manager->d_events.erase(callbackIt);
            return; // No other events can be registered
        }
        d_events.insert(*callbackIt);
        manager->d_events.erase(callbackIt);
    }
    if (FD_ISSET(handle, &manager->d_writeSet)) {
        FD_CLR(handle, &manager->d_writeSet);
        FD_SET(handle, &d_writeSet);
#ifdef BTESO_PLATFORM__WIN_SOCKETS
        FD_CLR(handle, &manager->d_exceptSet);
        FD_SET(handle, &d_exceptSet);
#endif
        --manager->d_numWrite;
        bteso_Event ev(handle, bteso_EventType::BTESO_WRITE);
        EventMap::iterator callbackIt = manager->d_events.find(ev);
        EventMap::iterator cbEnd = manager->d_events.end();
        if (cbEnd == callbackIt) {
            ev.setType(bteso_EventType::BTESO_CONNECT);
            callbackIt = manager->d_events.find(ev);
            BSLS_ASSERT(cbEnd != callbackIt);
        }
        d_events.insert(*callbackIt);
        manager->d_events.erase(callbackIt);
    }
}

void bteso_DefaultEventManager_SelectRaw::deregisterSocketEvent(
        const bteso_SocketHandle::Handle& handle,
        const bteso_EventType::Type       event)
{
    bteso_Event ev(handle, event);

    if (1 == d_events.erase(ev)) {
        if (event == bteso_EventType::BTESO_READ
         || event == bteso_EventType::BTESO_ACCEPT)
        {
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

int bteso_DefaultEventManager_SelectRaw::deregisterSocket(
    const bteso_SocketHandle::Handle& handle)
{
#ifdef BTESO_PLATFORM__BSD_SOCKETS
    if (handle == d_maxFd - 1) {
        d_maxFd = 0;
        EventMap::iterator it(d_events.begin()), end(d_events.end());
        for (; end != it; ++it) {
            if (it->first.handle() > d_maxFd) {
                d_maxFd = it->first.handle();
            }
        }
        ++d_maxFd;
    }
#endif
    int ncbs(0);
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

void bteso_DefaultEventManager_SelectRaw::deregisterAll() {
    d_events.clear();
    FD_ZERO(&d_readSet);
    FD_ZERO(&d_writeSet);
    FD_ZERO(&d_exceptSet);
    d_maxFd = 0;
    d_numRead = 0;
    d_numWrite = 0;
}

// ACCESSORS
bool bteso_DefaultEventManager_SelectRaw::canRegisterSockets() const
{
#ifdef BTESO_PLATFORM__WIN_SOCKETS
    return bsl::max(d_numRead, d_numWrite) < BTESO_MAX_NUM_HANDLES;
#else
    return d_maxFd < BTESO_MAX_NUM_HANDLES;
#endif
}

const bteso_EventManager::Callback&
bteso_DefaultEventManager_SelectRaw::callback(int index) const
{
    bsl::hash_map<bteso_Event,
                  bteso_EventManager::Callback,
                  bteso_EventHash>::const_iterator  callbackIt =
                                                              d_events.begin();
    for (int i = 0; i < index; ++i) {
        ++callbackIt;
    }
    return callbackIt->second;
}

int bteso_DefaultEventManager_SelectRaw::isRegistered(
    const bteso_SocketHandle::Handle& handle,
    const bteso_EventType::Type       event) const
{
    return d_events.end() != d_events.find(bteso_Event(handle, event));
}

int bteso_DefaultEventManager_SelectRaw::numSocketEvents (
        const bteso_SocketHandle::Handle& handle) const
{
    return   isRegistered(handle, bteso_EventType::BTESO_READ)
           + isRegistered(handle, bteso_EventType::BTESO_WRITE)
           + isRegistered(handle, bteso_EventType::BTESO_ACCEPT)
           + isRegistered(handle, bteso_EventType::BTESO_CONNECT);
}

            // =======================================================
            // class bteso_DefaultEventManager<bteso_Platform::SELECT>
            // =======================================================

                             // --------
                             // CREATORS
                             // --------

bteso_DefaultEventManager<bteso_Platform::SELECT>::bteso_DefaultEventManager(
                                            bslma_Allocator   *basicAllocator)
: d_impl(0, basicAllocator)
{
}

bteso_DefaultEventManager<bteso_Platform::SELECT>::bteso_DefaultEventManager(
                                            bteso_TimeMetrics *timeMetric,
                                            bslma_Allocator   *basicAllocator)
: d_impl(timeMetric, basicAllocator)
{
}

bteso_DefaultEventManager<bteso_Platform::SELECT>::~bteso_DefaultEventManager()
{
}

// MANIPULATORS
int bteso_DefaultEventManager<bteso_Platform::SELECT>::dispatch(int flags)
{
    return d_impl.dispatch(flags);
}

int bteso_DefaultEventManager<bteso_Platform::SELECT>::dispatch(
                               const bdet_TimeInterval& timeout,
                               int flags)
{
    return d_impl.dispatch(timeout, flags);
}

int bteso_DefaultEventManager<bteso_Platform::SELECT>::registerSocketEvent(
        const bteso_SocketHandle::Handle&   handle,
        const bteso_EventType::Type         eventType,
        const bteso_EventManager::Callback& callback)
{
    return d_impl.registerSocketEvent(handle, eventType, callback);
}

void bteso_DefaultEventManager<bteso_Platform::SELECT>::deregisterSocketEvent(
        const bteso_SocketHandle::Handle& handle,
        const bteso_EventType::Type       event)
{
    d_impl.deregisterSocketEvent(handle, event);
}

int bteso_DefaultEventManager<bteso_Platform::SELECT>::deregisterSocket(
                const bteso_SocketHandle::Handle& handle)
{
    return d_impl.deregisterSocket(handle);
}

void bteso_DefaultEventManager<bteso_Platform::SELECT>::deregisterAll() {
    d_impl.deregisterAll();
}

// ACCESSORS
bool bteso_DefaultEventManager<bteso_Platform::SELECT>::canRegisterSockets()
                                                                          const
{
    return d_impl.canRegisterSockets();
}

int bteso_DefaultEventManager<bteso_Platform::SELECT>::isRegistered(
    const bteso_SocketHandle::Handle& handle,
    const bteso_EventType::Type       event) const
{
    return d_impl.isRegistered(handle, event);
}

int bteso_DefaultEventManager<bteso_Platform::SELECT>::numEvents() const
{
    return d_impl.numEvents();
}

int bteso_DefaultEventManager<bteso_Platform::SELECT>::numSocketEvents (
        const bteso_SocketHandle::Handle& handle) const
{
    return d_impl.numSocketEvents(handle);
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
