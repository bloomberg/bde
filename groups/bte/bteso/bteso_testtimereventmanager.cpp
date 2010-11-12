// bteso_testtimereventmanager.cpp -*-C++-*-

#include <bteso_testtimereventmanager.h>

#include <bteso_platform.h>
#include <bdetu_systemtime.h>
#include <bdes_assert.h>

#include <iostream>

namespace BloombergLP {

inline
int operator<(const bteso_TestTimer& lhs, const bteso_TestTimer& rhs) {
    return lhs.d_timeout < rhs.d_timeout;
}


bteso_TestTimerEventManager::bteso_TestTimerEventManager(
        const char *script)
    : d_script(script)
    , d_cursor(0)
{
    FD_ZERO(&d_readSet);
    FD_ZERO(&d_writeSet);
}

bteso_TestTimerEventManager::~bteso_TestTimerEventManager() {
    d_timers.removeAll();
    d_events.removeAll();
}

// MANIPULATORS
int bteso_TestTimerEventManager::dispatch(InterruptOpt value) {
    std::cout << "Dispatch: " << d_timers.length() << " timers, "
         << d_events.length() << " sockets." << std::endl;
    int ret;
    bteso_TestTimer *t;
    fd_set readSet, writeSet;
    readSet = d_readSet;
    writeSet = d_writeSet;
    if (d_timers.length()) {
        t = d_timers.top();
        BDE_ASSERT_CPP(t);
        struct timeval timeout;
        bdet_TimeInterval delta = t->d_timeout - bdetu_SystemTime::now();
        timeout.tv_sec = delta.seconds() > 0 ? delta.seconds() : 0;
        timeout.tv_usec = delta.nanoseconds() / 1000; // microseconds
        if (timeout.tv_usec < 0) {
            timeout.tv_usec = 0;
        }
        BDE_ASSERT_CPP(timeout.tv_usec >= 0);
        BDE_ASSERT_CPP(timeout.tv_sec >= 0);
#ifdef BTESO_PLATFORM__WIN_SOCKETS
        if (0 == d_events.length()) {
            DWORD timeoutMS = timeout.tv_sec * 1000 + timeout.tv_usec / 1000;
            ::Sleep(timeoutMS);
            ret = 0;
        }
        else {
            ret = ::select(0, &readSet, &writeSet, NULL, &timeout);
        }
#else
        ret = ::select(0, &readSet, &writeSet, NULL, &timeout);
#endif
    }
    else {
        ret = ::select(0, &readSet, &writeSet, NULL, 0);
    }

    if (ret > 0) {
        // ret - the number of sockets signaled
        bdec2_Map<bteso_Event, bcef_Vfunc0, bteso_EventHash>
            events(d_events);
        bdec2_MapManip<bteso_Event, bcef_Vfunc0, bteso_EventHash>
            manip(&events);
        while(manip) {     // TBD - could be nice to have && ret
            const bteso_Event &sev = manip.key();
            if (bteso_EventType::READ == sev.type() ||
                bteso_EventType::ACCEPT == sev.type())
            {
                // TBD - we can optimize this  if
                // we cache the address of the set
                // in the socket event structure
                if (FD_ISSET(sev.handle(), &readSet)) {
                    manip.value()();
                    // This can invoke a registration, and, in turn,
                    // violate the manipulator.
                }
                manip.advance();
            }
            else {
                if (FD_ISSET(sev.handle(), &writeSet)) {
                    manip.value()();
                }
                manip.advance();
            }
        }
        return d_timers.length() + d_events.length();
    }

    if (0 == ret) {
        // Timeout occured.  TBD: invoke multiple.
        d_timers.extractTop();
        t->d_callback();
        delete t;
        return d_timers.length() + d_events.length();
    }
    return ret; // ERROR OCCURED
}

int bteso_TestTimerEventManager::registerSocketEvent(
        const bteso_SocketHandle::Handle& handle,
        const bteso_EventType::Type eventType,
        const Callback&                   cb)
{
    std::cout << "Registering socket event " << eventType << std::endl;
    bteso_Event ev(handle, eventType);
    d_events.add(ev, cb);
    if (eventType == bteso_EventType::READ
        || eventType == bteso_EventType::ACCEPT) {
        FD_SET(handle, &d_readSet);
    }
    else {
        FD_SET(handle, &d_writeSet);
    }
    return 0;
}

void* bteso_TestTimerEventManager::registerTimer(
        const bdet_TimeInterval& timeout,
        const Callback&          cb)
{
    std::cout << "Registering timer for cb at " << timeout << std::endl;
    bteso_TestTimer *p = new bteso_TestTimer (timeout, cb);
    d_timers.add(p);
    return p;
}

int bteso_TestTimerEventManager::deregisterSocketEvent(
        const bteso_SocketHandle::Handle& handle,
        const bteso_EventType::Type event)
{
    std::cout << "Deregistering socket event: " << event << std::endl;
    bteso_Event ev(handle, event);
    if (0 == d_events.remove(ev)) {
        return -1;
    }
    if (event == bteso_EventType::READ || event == bteso_EventType::ACCEPT) {
        FD_CLR(handle, &d_readSet);
    }
    else {
        FD_CLR(handle, &d_writeSet);
    }
    return 0;
}


int bteso_TestTimerEventManager::deregisterSocket(
        const bteso_SocketHandle::Handle& handle)
{
    deregisterSocketEvent(handle, bteso_EventType::READ);
    deregisterSocketEvent(handle, bteso_EventType::WRITE);
    deregisterSocketEvent(handle, bteso_EventType::ACCEPT);
    deregisterSocketEvent(handle, bteso_EventType::CONNECT);
    return 0;
}


void bteso_TestTimerEventManager::deregisterAllSocketEvents() {
    d_events.removeAll();
    FD_ZERO(&d_readSet);
    FD_ZERO(&d_writeSet);
}


int bteso_TestTimerEventManager::deregisterTimer(const void *id) {
    std::cout << "Deregistering timer...";
    bdec_PtrPriorityQueueManip<bteso_TestTimer> manip(&d_timers);
    bteso_TestTimer *p = (bteso_TestTimer*)id;
    while (manip) {
        if (manip() == p) {
            delete p;
            manip.remove();
            std::cout << "Done!" << std::endl;
            return 0;
        }
        manip.advance();
    }
    std::cout << "NOT FOUND." << std::endl;
    return -1;
}

int bteso_TestTimerEventManager::deregisterAllTimers() {
    bdec_PtrPriorityQueueManip<bteso_TestTimer> manip(&d_timers);
    while (manip) {
        delete manip();
        manip.remove();
    }

    d_timers.removeAll();
    return 0;
}


void bteso_TestTimerEventManager::deregisterAll() {
    deregisterAllSocketEvents();
    deregisterAllTimers();
}

// ACCESSORS
int bteso_TestTimerEventManager::numSocketEvents(
        const bteso_SocketHandle::Handle& handle) const {
    return 0;
}

int bteso_TestTimerEventManager::numTimers() const {
    return d_timers.length();
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
