// bteso_defaulteventmanager_wfmo.cpp -*-C++-*-

#include <bteso_defaulteventmanager_wfmo.h>

#ifdef BTE_FOR_TESTING_ONLY
// These dependencies need to be here for the the bde_build.pl script to
// generate the proper makefiles, but do not need to be compiled into the
// component's .o file.  The symbol BTE_FOR_TESTING_ONLY should remain
// undefined, and is here only because '#if 0' is optimized away by the
// bde_build.pl script.

#include <bteso_socketimputil.h>         // for testing only
#include <bteso_eventmanagertester.h>    // for testing only
#endif

#include <bteso_flag.h>
#include <bteso_timemetrics.h>

#include <bcemt_thread.h>
#include <bdetu_systemtime.h>
#include <bdes_assert.h>

namespace BloombergLP {

#if defined(BTESO_PLATFORM__WIN_SOCKETS)

typedef bteso_DefaultEventManager_WfmoRaw_HandleInfo HandleInfo;

                             // ------------------
                             // FILE-SCOPE STATICS
                             // ------------------
inline int dispatchCallbacks(
            const WSAEVENT&                                       eventHandle,
            const bteso_DefaultEventManager_WfmoRaw_HandleInfo& state,
                        bdec2_Map<bteso_Event,
                                  bteso_EventManager::Callback,
                                          bteso_EventHash> *callbacks)
{
    BDE_ASSERT_CPP(callbacks);
        typedef bteso_EventManager::Callback Callback;

    const bteso_SocketHandle::Handle& socketHandle = state.handle();

    WSANETWORKEVENTS wsaNetworkEvents;
    BDE_ASSERT_CPP(0 ==
           WSAEnumNetworkEvents(socketHandle, eventHandle, &wsaNetworkEvents));

    int ncbs = 0;

    if (wsaNetworkEvents.lNetworkEvents & FD_READ) {
        Callback *cb = callbacks->lookup(
                             bteso_Event(socketHandle, bteso_EventType::READ));
        if (cb) {
            ++ncbs;
            (*cb)();
        }
    }
    if (wsaNetworkEvents.lNetworkEvents & FD_WRITE) {
        // std::cout << "WRITE is ready on " << socketHandle << std::endl;
        Callback *cb = (Callback*)0;
        if ((cb = callbacks->lookup(
                    bteso_Event(socketHandle, bteso_EventType::WRITE)))
            || (cb = callbacks->lookup(
                    bteso_Event(socketHandle, bteso_EventType::CONNECT))))
                {
            ++ncbs;
            (*cb)();
            return ncbs;
        }
    }
    if (wsaNetworkEvents.lNetworkEvents & FD_ACCEPT) {
        Callback *cb = callbacks->lookup(
                           bteso_Event(socketHandle, bteso_EventType::ACCEPT));
        if (cb) {
            ++ncbs;
            (*cb)();
        }
    }
    return ncbs;
}


                    // =======================================
                    // class bteso_DefaultEventManager_WfmoRaw
                    // =======================================

bteso_DefaultEventManager_WfmoRaw::bteso_DefaultEventManager_WfmoRaw(
        bteso_TimeMetrics *timeMetric,
        bdema_Allocator   *basicAllocator)
: d_allocator_p(bdema_Default::allocator(basicAllocator))
//, d_handles(basicAllocator)
, d_sockets(basicAllocator)
, d_socketIndex(basicAllocator)
, d_callbacks(basicAllocator)
, d_timeMetric_p(timeMetric)
, d_signaled(basicAllocator)
{
    BDE_ASSERT_CPP(d_allocator_p);
}

bteso_DefaultEventManager_WfmoRaw::~bteso_DefaultEventManager_WfmoRaw()
{
        int numHandles = d_sockets.size();
        BDE_ASSERT_CPP(d_sockets.size() == numHandles);
        for (int i = 0; i < numHandles; i++) {
        DWORD rc = WSACloseEvent(d_handles[i]);
                BDE_ASSERT_CPP(rc);
    }
}

                             // ------------
                             // MANIPULATORS
                             // ------------

int bteso_DefaultEventManager_WfmoRaw::dispatch(
        const bdet_TimeInterval&   deadline,
        int                        flags)
{
    bdet_TimeInterval delta = deadline - bdetu_SystemTime::now();
    DWORD sleepTime = delta.seconds() * 1000 +
                                       delta.nanoseconds() / 1000000;
    if (0 == numEvents()) {
        DWORD retVal;
        if (d_timeMetric_p) {
            d_timeMetric_p->switchTo(bteso_TimeMetrics::IO_BOUND);
            retVal = ::SleepEx(sleepTime, flags & bteso_Flag::ASYNC_INTERRUPT);
            d_timeMetric_p->switchTo(bteso_TimeMetrics::CPU_BOUND);
        }
        else {
            retVal = ::SleepEx(sleepTime, flags & bteso_Flag::ASYNC_INTERRUPT);
        }
        BDE_ASSERT_CPP(0 == retVal || WAIT_IO_COMPLETION == retVal);
        return retVal ? -1 : 0;
    }

    // Return code.
    DWORD dwRet;
    DWORD nWait = d_sockets.size();
/*
        std::cout << "Sleeping for " << sleepTime << std::endl;
        std::cout << "Monitoring " << nWait << " handles." << std::endl;
        for (int i = 0; i < nWait; ++i) {
             printf("(%x, %d, %d) ",
                    d_handles[i],
                    d_sockets[i].handle(),
                    d_sockets[i].mask());
        }
        puts("");
*/

    if (d_timeMetric_p) {
        d_timeMetric_p->switchTo(bteso_TimeMetrics::IO_BOUND);
        dwRet =
           WSAWaitForMultipleEvents(nWait, d_handles, FALSE, sleepTime, FALSE);
        d_timeMetric_p->switchTo(bteso_TimeMetrics::CPU_BOUND);
    }
    else {
        dwRet =
           WSAWaitForMultipleEvents(nWait, d_handles, FALSE, sleepTime, FALSE);
    }
//        std::cout << "WFMO returned " << dwRet << std::endl;
    if (WSA_WAIT_FAILED == dwRet) {
        return -2;
    }
    if(WSA_WAIT_TIMEOUT == dwRet) {
        return 0;
    }
    if (WAIT_IO_COMPLETION == dwRet) {
        return -1;
    }

    dwRet -= WSA_WAIT_EVENT_0;
    d_signaled.clear();
    d_signaled.reserve(d_sockets.size());
//  std::cout << dwRet << " is signaled" << std::endl;
    d_signaled.push_back(dwRet);
    for (int i = dwRet + 1; i < nWait; ++i) {
        dwRet = WSAWaitForMultipleEvents(1, &d_handles[i], TRUE, 0, FALSE);
//                std::cout << "WFME returned " << dwRet << std::endl;
        if (dwRet != WSA_WAIT_TIMEOUT && dwRet != WSA_WAIT_FAILED) {
            BDE_ASSERT_CPP(WSA_WAIT_EVENT_0 == dwRet);
            d_signaled.push_back(i);
        }
    }
    int numSignaled = d_signaled.size();
//        printf("%d CALLBACK CANDIDATES\n", numSignaled);
        // Number of dispatched callbacks.
    int nDispatched = 0;
    for (int i = 0; i < numSignaled; ++i) {
        // std::cout << "\t" << d_handles[d_signaled[i]] << " signaled."
        //           << std::endl;
        nDispatched += dispatchCallbacks(d_handles[d_signaled[i]],
                                         d_sockets[d_signaled[i]],
                                         &d_callbacks);
        //        std::cout << nDispatched << std::endl;
    }
        /*
        nWait = d_sockets.size();
        for (int i = 0; i < nWait; ++i) {
                int rc = WSAEventSelect(d_sockets[i].handle(), d_handles[i],
                        d_sockets[i].mask());
                BDE_ASSERT_CPP(0 == rc);
        }
        */
    return nDispatched;
}

int bteso_DefaultEventManager_WfmoRaw::
                                 dispatch(int flags)
{
    if (0 == numEvents()) {
        return 0;
    }
    // Return code.
    DWORD dwRet;

    // Number of dispatched callbacks.
    int nDispatched = 0;
    DWORD sleepTime = INFINITE;

        DWORD nWait = d_sockets.size();
/*
        std::cout << "Monitoring " << nWait << " handles." << std::endl;
        for (int i = 0; i < nWait; ++i) {
            printf("(%d, %d) ", d_sockets[i].handle(), d_sockets[i].mask());
        }
        puts("");
*/
        if (d_timeMetric_p) {
            d_timeMetric_p->switchTo(bteso_TimeMetrics::IO_BOUND);
            dwRet = ::WSAWaitForMultipleEvents(
                                     nWait,
                                     d_handles,
                                     FALSE,
                                     sleepTime,
                                     (flags & bteso_Flag::ASYNC_INTERRUPT));
            d_timeMetric_p->switchTo(bteso_TimeMetrics::CPU_BOUND);
        }
        else {
            dwRet = ::WSAWaitForMultipleEvents(
                                     nWait,
                                     d_handles,
                                     FALSE,
                                     sleepTime,
                                     (flags & bteso_Flag::ASYNC_INTERRUPT));
        }

        if (dwRet == WSA_WAIT_FAILED) {
                return -2;
        }
        if (dwRet == WAIT_IO_COMPLETION) {
                return -1;
        }

        BDE_ASSERT_CPP(WSA_WAIT_TIMEOUT != dwRet);
        dwRet -= WSA_WAIT_EVENT_0;
        d_signaled.clear();
        d_signaled.reserve(d_sockets.size());
        d_signaled.push_back(dwRet);
        for (int i = dwRet + 1; i < nWait; ++i) {
            dwRet = WSAWaitForMultipleEvents(1, &d_handles[i], TRUE, 0, FALSE);
            if (dwRet != WSA_WAIT_TIMEOUT && dwRet != WSA_WAIT_FAILED) {
                 BDE_ASSERT_CPP(WSA_WAIT_EVENT_0 == dwRet);
                 d_signaled.push_back(i);
            }
        }
        int numSignaled = d_signaled.size();
        //std::cout << _LINE_ << ": " << numSignaled << " events signaled."
        //          << std::endl;
        for (int i = 0; i < numSignaled; ++i) {
            //std::cout << d_handles[d_signaled[i]] << " signaled."
            //          << std::endl;
            nDispatched += dispatchCallbacks(d_handles[d_signaled[i]],
                                             d_sockets[d_signaled[i]],
                                             &d_callbacks);
        }
        // std::cout << nDispatched << " callbacks dispatched." << std::endl;
        nWait = d_sockets.size();
        for (int i = 0; i < nWait; ++i) {
                WSAResetEvent(d_handles[i]);
        }
        return nDispatched;
}

int bteso_DefaultEventManager_WfmoRaw::registerSocketEvent(
            const bteso_SocketHandle::Handle&              handle,
            const bteso_EventType::Type                    event,
            const bteso_EventManager::Callback&            callback)
{
    bteso_Event handleEvent(handle, event);
    bteso_EventManager::Callback *savedCb = d_callbacks.lookup(handleEvent);
    if (savedCb) {
        *savedCb = callback;
        return 0;
    }

    // Map the socket to the corresponding (system) 'WSAEVENT'.
    const int *index = d_socketIndex.lookup(handle);
    HandleInfo *handleState = NULL;
    HandleInfo newState;
    WSAEVENT systemEvent = WSA_INVALID_EVENT;
    if (!index) {
        // A new system handle needs to be created
        systemEvent = ::WSACreateEvent();
        // std::cout << "Created a new event " << systemEvent << std::endl;
        handleState = &newState;
        newState.setHandle(handle);
        newState.setHandleIndex(d_sockets.size());
        newState.setMask(0);
        d_handles[d_sockets.size()] = systemEvent;
    }
    else {
        handleState = &d_sockets[*index];
        systemEvent = d_handles[*index];
    }

    BDE_ASSERT_CPP(WSA_INVALID_EVENT != systemEvent);
    long mask = handleState->mask();

    switch (event) {
        // No other event can be registered simultaneously with ACCEPT.
      case bteso_EventType::ACCEPT: {
        BDE_ASSERT_CPP(0 == mask);
        mask |= FD_ACCEPT;
      }  break;

        // Only WRITE can be registered simultaneously with READ.
      case bteso_EventType::READ: {
        BDE_ASSERT_CPP(0 == (mask & ~FD_WRITE));
        mask |= FD_READ;
      }  break;

        // No other event can be registered simultaneously with CONNECT.
      case bteso_EventType::CONNECT: {
        BDE_ASSERT_CPP(0 == mask);
        mask |= FD_WRITE;
      }  break;

        // Only READ can be registered simultaneously with WRITE.
      case bteso_EventType::WRITE: {
        BDE_ASSERT_CPP(0 == (mask & ~FD_READ));
        mask |= FD_WRITE;
      }  break;

      default:
        BDE_ASSERT_CPP("Must be unreachable -- unknown event type" && 0);
        return -1;
    }

    handleState->setMask(mask);
    int rc = ::WSAEventSelect(handle, systemEvent, mask);
    BDE_ASSERT_CPP(0 == rc);

    if (!index) {
        d_sockets.push_back(newState);
        d_socketIndex.add(handle, newState.handleIndex());
    }
    savedCb = d_callbacks.add(handleEvent, callback);
    BDE_ASSERT_CPP(savedCb);
    return 0;
}

void bteso_DefaultEventManager_WfmoRaw::moveSocket(
                bteso_DefaultEventManager_WfmoRaw *manager,
                const bteso_SocketHandle::Handle& handle)
{
    BDE_ASSERT_CPP(manager);

    const int *index = manager->d_socketIndex.lookup(handle);
    BDE_ASSERT_CPP(index);

    d_handles[d_sockets.size()] = manager->d_handles[*index];
    d_sockets.push_back(manager->d_sockets[*index]);
    d_socketIndex.add(handle, d_sockets.size() - 1);
    d_sockets[*index].setHandleIndex(d_sockets.size() - 1);

    int numHandles = manager->d_sockets.size();
    if (*index != numHandles - 1) {
        manager->d_handles[*index] = manager->d_handles[numHandles - 1];
        manager->d_sockets[*index] = manager->d_sockets[numHandles - 1];
        manager->d_sockets[*index].setHandleIndex(*index);
        manager->d_socketIndex.set(manager->d_sockets[*index].handle(),
                                   *index);
    }
    manager->d_socketIndex.remove(handle);
    //manager->d_handles.setLength(numHandles - 1);
    manager->d_sockets.resize(numHandles - 1);
    // Move the callbacks
    long mask = d_sockets[*index].mask();
    if (mask & FD_READ) {
        bteso_Event ev(handle, bteso_EventType::READ);
        bteso_EventManager::Callback *cb = manager->d_callbacks.lookup(ev);
        BDE_ASSERT_CPP(cb);
        d_callbacks.add(ev, *cb);
        manager->d_callbacks.remove(ev);
    }
    if (mask & FD_WRITE) {
        bteso_Event ev(handle, bteso_EventType::WRITE);
        bteso_EventManager::Callback *cb = manager->d_callbacks.lookup(ev);
        if (cb) {
            d_callbacks.add(ev, *cb);
            manager->d_callbacks.remove(ev);
        }
        else {
            bteso_Event evc(handle, bteso_EventType::CONNECT);
            cb = manager->d_callbacks.lookup(evc);
            BDE_ASSERT_CPP(cb);
            d_callbacks.add(ev, *cb);
            manager->d_callbacks.remove(evc);
        }
        return;
    }
    if (mask & FD_ACCEPT) {
        bteso_Event ev(handle, bteso_EventType::ACCEPT);
        bteso_EventManager::Callback *cb = manager->d_callbacks.lookup(ev);
        BDE_ASSERT_CPP(cb);
        d_callbacks.add(ev, *cb);
        manager->d_callbacks.remove(ev);
    }
}

void bteso_DefaultEventManager_WfmoRaw::deregisterSocketEvent(
            const bteso_SocketHandle::Handle& handle,
            const bteso_EventType::Type       event)
{
    // std::cout << "DeRegistering " << handle << std::endl;
    // Determine from d_callbacks if the event is currently registered.
    bteso_Event  handleEvent(handle, event);
    bteso_EventManager::Callback *cb = d_callbacks.lookup(handleEvent);
    BDE_ASSERT_CPP(cb);

    // Translate the type of event.
    long fdevent = 0;
    switch (event) {
      case bteso_EventType::ACCEPT:
        fdevent = FD_ACCEPT;
        break;
      case bteso_EventType::READ:
        fdevent = FD_READ;
        break;
      case bteso_EventType::CONNECT:
      case bteso_EventType::WRITE:
        fdevent = FD_WRITE;
        break;
      default:
        BDE_ASSERT_CPP(0);
        return;
    }
    const int *index = d_socketIndex.lookup(handle);
    BDE_ASSERT_CPP(index);
    WSAEVENT systemEvent = d_handles[*index];
    HandleInfo& handleState = d_sockets[*index];

    // Ensure that the bit for the specified event is set.
    long mask = handleState.mask();
    BDE_ASSERT_CPP(mask & fdevent);

    // Create new mask by clearing the bit.
    mask &= ~fdevent;

    int rc;
    if (mask) {
        rc = WSAEventSelect(handle, systemEvent, mask);
        BDE_ASSERT_CPP(SOCKET_ERROR != rc);
        handleState.setMask(mask);
    }
    else {
        // Deregister this file descriptor altogether.
                CloseHandle(systemEvent);
                int numHandles = d_sockets.size();
                if (*index != numHandles - 1) {
                        d_handles[*index] = d_handles[numHandles - 1];
                        d_sockets[*index] = d_sockets[numHandles - 1];
                        d_sockets[*index].setHandleIndex(*index);
                        d_socketIndex.set(d_sockets[*index].handle(),
                                *index);
                }
                d_socketIndex.remove(handle);
                //d_handles.setLength(numHandles - 1);
                d_sockets.resize(numHandles - 1);
    }
    // Remove the callback for this handle/event.
    rc = d_callbacks.remove(handleEvent);
    BDE_ASSERT_CPP(1 == rc);
}

int bteso_DefaultEventManager_WfmoRaw::deregisterSocket(
        const bteso_SocketHandle::Handle& handle)
{
    const int *index = d_socketIndex.lookup(handle);
    int result = 0;
    if (!index) {
        return 0;
    }
    long mask = d_sockets[*index].mask();

    CloseHandle(d_handles[*index]);

    int numHandles = d_sockets.size();
    if (*index != numHandles - 1) {
        d_handles[*index] = d_handles[numHandles - 1];
        d_sockets[*index] = d_sockets[numHandles - 1];
        d_sockets[*index].setHandleIndex(*index);
        d_socketIndex.set(d_sockets[*index].handle(), *index);
    }
    d_socketIndex.remove(handle);
    // d_handles.setLength(numHandles - 1);
    d_sockets.resize(numHandles - 1);

    if (mask & FD_READ) {
        bteso_Event ev(handle, bteso_EventType::READ);
        bteso_EventManager::Callback *cb = d_callbacks.lookup(ev);
        if (cb) {
            d_callbacks.remove(ev);
            ++result;
        }
    }
    if (mask & FD_WRITE) {
        bteso_Event ev(handle, bteso_EventType::WRITE);
        bteso_EventManager::Callback *cb = d_callbacks.lookup(ev);
        if (cb) {
            d_callbacks.remove(ev);
            ++result;
        }
        else {
            bteso_Event evc(handle, bteso_EventType::CONNECT);
            cb = d_callbacks.lookup(evc);
            BDE_ASSERT_CPP(cb);
            d_callbacks.remove(evc);
            ++result;
        }
        return result;
    }
    else {
        if (mask & FD_ACCEPT) {
          bteso_Event ev(handle, bteso_EventType::ACCEPT);
          bteso_EventManager::Callback *cb = d_callbacks.lookup(ev);
          BDE_ASSERT_CPP(cb);
          d_callbacks.remove(ev);
          ++result;
        }
    }
    return result;
}

void bteso_DefaultEventManager_WfmoRaw::deregisterAll()
{
    int numHandles = d_sockets.size();
    for (int i = 0; i < numHandles; ++i) {
        CloseHandle(d_handles[i]);
    }

    // d_handles.removeAll();
    d_sockets.clear();
    d_socketIndex.removeAll();
    d_callbacks.removeAll();
}

                             // ---------
                             // ACCESSORS
                             // ---------

int bteso_DefaultEventManager_WfmoRaw::numSocketEvents(
        const bteso_SocketHandle::Handle& socketHandle) const
{
    const int *index = d_socketIndex.lookup(socketHandle);
    if (!index) {
        return 0;
    }

    long mask = d_sockets[*index].mask();
    return 1 + (mask == (FD_READ | FD_WRITE));
}

int bteso_DefaultEventManager_WfmoRaw::isRegistered(
    const bteso_SocketHandle::Handle&        handle,
    const bteso_EventType::Type              event) const
{
    return 0 != d_callbacks.lookup(bteso_Event(handle, event));
}

            // =======================================================
            // class bteso_DefaultEventManager<bteso_Platform::WFMO>
            // =======================================================

typedef bteso_DefaultEventManager<bteso_Platform::WFMO> EventManagerName;

                             // --------
                             // CREATORS
                             // --------

EventManagerName::bteso_DefaultEventManager(bteso_TimeMetrics *timeMetric,
                                            bdema_Allocator   *basicAllocator)
: d_impl(bdet_TimeInterval(0, SPIN_PERIOD_MS * ONE_MILLISECOND),
                           timeMetric, basicAllocator)
{

}

EventManagerName::~bteso_DefaultEventManager() {

}

// MANIPULATORS
int EventManagerName::dispatch(int flags)
{
    return d_impl.dispatch(flags);
}

int EventManagerName::dispatch(const bdet_TimeInterval&   timeout,
                               int                        flags)
{
    return d_impl.dispatch(timeout, flags);
}

int EventManagerName::registerSocketEvent(
        const bteso_SocketHandle::Handle&   handle,
        const bteso_EventType::Type         eventType,
        const bteso_EventManager::Callback& callback)
{
    return d_impl.registerSocketEvent(handle, eventType, callback);
}

void EventManagerName::deregisterSocketEvent(
        const bteso_SocketHandle::Handle& handle,
        const bteso_EventType::Type       event)
{
    d_impl.deregisterSocketEvent(handle, event);
}


int EventManagerName::deregisterSocket(
                const bteso_SocketHandle::Handle& handle)
{
    return d_impl.deregisterSocket(handle);
}


void EventManagerName::deregisterAll() {
    d_impl.deregisterAll();
}

// ACCESSORS
int EventManagerName::isRegistered(
    const bteso_SocketHandle::Handle& handle,
    const bteso_EventType::Type       event) const
{
    return d_impl.isRegistered(handle, event);

}

int EventManagerName::numEvents() const
{
    return d_impl.numEvents();
}

int EventManagerName::numSocketEvents (
        const bteso_SocketHandle::Handle& handle) const
{
    return d_impl.numSocketEvents(handle);
}

#endif // BTESO_PLATFORM__WIN_SOCKETS
}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
