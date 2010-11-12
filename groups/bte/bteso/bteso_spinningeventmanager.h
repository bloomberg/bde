// bteso_spinningeventmanager.h  -*-C++-*-
#ifndef INCLUDED_BTESO_SPINNINGEVENTMANAGER
#define INCLUDED_BTESO_SPINNINGEVENTMANAGER

//@PURPOSE: Provide a generic event manager that spins multiple managers.
//
//@AUTHOR: Andrei Basov (abasov)
//
//@CLASSES:
//  bteso_SpinningEventManager:  a spinning event manager
//
//@SEE_ALSO:
//
//@DESCRIPTION:
//
///Thread-safety
///-------------
//
///Performance
///-----------
//
///USAGE EXAMPLE
///=============
//..
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_EVENTMANAGER
#include <bteso_eventmanager.h>
#endif

#ifndef INCLUDED_BTESO_EVENT
#include <bteso_event.h>
#endif

#ifndef INCLUDED_BTESO_TIMEMETRICS
#include <bteso_timemetrics.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BCEF_VFUNC0
#include <bcef_vfunc0.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BDEC2_MAP
#include <bdec2_map.h>
#endif

#ifndef INCLUDED_BDEMA_POOL
#include <bdema_pool.h>
#endif

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif

namespace BloombergLP {

                      // ================================
                      // class bteso_SpinningEventManager
                      // ================================

template <class RAW_EVENTMANAGER>
        class bteso_SpinningEventManager : public bteso_EventManager {
    // [TBD - Class description]
  private:
    std::vector<RAW_EVENTMANAGER*> d_pollers;

    bdec2_Map<bteso_SocketHandle::Handle, int, bdeimp_IntHash>
                         d_indices;

    bdema_Pool           d_pool;

    bteso_TimeMetrics   *d_metrics_p;

    bdema_Allocator     *d_allocator_p;

    bdet_TimeInterval    d_spinPeriod;

    // Not implemented
    bteso_SpinningEventManager(const bteso_SpinningEventManager&);
    bteso_SpinningEventManager&
        operator=(const bteso_SpinningEventManager&);
  public:
    // CREATORS
    bteso_SpinningEventManager(const bdet_TimeInterval&   spinPeriod,
                               bteso_TimeMetrics *timeMetric     = 0,
                               bdema_Allocator   *basicAllocator = 0);
        // Create an event manager that simulates a capacity-unlimited
        // socket-event multiplexer by managing many instances of
        // 'RAW_EVENTMANAGER''s and looping with the specified 'spinPeriod'
        // through the set of managed event managers.  Optionally specify a
        // 'timeMetric' to report time spent in CPU-bound and IO-bound
        // operations.  If 'timeMetric' is not specified or is 0, these metrics
        // are not reported.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~bteso_SpinningEventManager();
        // Destroy this object.  Note that the registered callbacks are NOT
        // invoked.

    // MANIPULATORS
    int dispatch(const bdet_TimeInterval& timeout, int flags);


    int dispatch(int flags);


    int registerSocketEvent(const bteso_SocketHandle::Handle&   handle,
                             const bteso_EventType::Type         event,
                             const bteso_EventManager::Callback& callback);

        // Register with this event manager the specified 'callback' to be
        // invoked when the specified 'event' occurs on the specified socket
        // 'handle'.  Each socket event registration stays in effect until it
        // is subsequently deregistered; the callback is invoked each time
        // the corresponding event is detected.  'bteso_EventType::READ' and
        // 'bteso_EventType::WRITE' are the only events that can be registered
        // simulataneously for a socket.  If a registration attempt is made for
        // an event that is already registered, the callback associated with
        // this event will be overwritten with the new one.  Simultaneous
        // registration of incompatible events for the same socket 'handle'
        // will result in undefined behavior.  The behavior is also undefined
        // unless the the capacity of this event manager is not reached
        // (i.e., MAX_NUM_HANDLES < numSockets()) or unless
        // 'handle' is already registered with this event manager (i.e.,
        // numSocketEvents(handle) > 0).  Note that the callback is recurring
        // (i.e., it remains registered until it is explicitly deregistered).


    void deregisterAll();
        // Deregister from this event manager all events on every socket
        // handle.

    int deregisterSocket(const bteso_SocketHandle::Handle& handle);
        // Deregister from this event manager all events associated with the
        // specified socket 'handle'.  Return the number of deregistered
        // callbacks.

    void deregisterSocketEvent(const bteso_SocketHandle::Handle& handle,
                               bteso_EventType::Type       event);
        // Deregister from this event manager the callback associated with the
        // specified 'event' on the specified 'handle' so that said callback
        // will not be invoked should 'event' occur.

    // ACCESSORS
    int isRegistered(const bteso_SocketHandle::Handle& handle,
                     const bteso_EventType::Type       event) const;
        // Return 1 if the specified 'event' is registered with this event
        // manager for the specified socket 'handle' and 0 otherwise.

    int numEventManagers() const;
        // Return the number of raw event managers currently employed by
        // this event manager.

    int numEvents() const;
        // Return the total number of all socket events currently registered
        // with this event manager.

    int numSockets() const;
            // Return the number of different socket handles registered with
            // this event manager.

    int numSocketEvents(const bteso_SocketHandle::Handle& handle) const;
        // Return the number of socket events currently registered with this
        // event manager for the specified 'handle'.

    const bteso_EventManager::Callback& callback(int index) const;
             // Return a callback associated with the specified 'index'.  The
        // behavior is undefined unless 0 <= index < numEvents().  Note that
            // this association may be violated by an invokation of any
            // manipulator on this event manager.

    const bteso_Event& event(int index) const;
            // Return the event associated with the specified 'index'.  The
        // behavior is undefined unless 0 <= index < numEvents().  Note that
            // this association may be violated by an invocation of any
            // manipulator on this event manager.
};

//-----------------------------------------------------------------------------
//                      INLINE FUNCTIONS' DEFINITIONS
//-----------------------------------------------------------------------------

                             // --------
                             // CREATORS
                             // --------

template <class RAW_EVENTMANAGER>
inline
bteso_SpinningEventManager<RAW_EVENTMANAGER>::bteso_SpinningEventManager(
      const bdet_TimeInterval& spinPeriod,
      bteso_TimeMetrics       *timeMetric,
      bdema_Allocator     *basicAllocator)
: d_pollers(basicAllocator)
, d_indices(basicAllocator)
, d_pool(sizeof(RAW_EVENTMANAGER), basicAllocator)
, d_metrics_p(timeMetric)
, d_allocator_p(basicAllocator)
, d_spinPeriod(spinPeriod)
{
        RAW_EVENTMANAGER *poller
        = new (d_pool) RAW_EVENTMANAGER(d_metrics_p, d_allocator_p);
    d_pollers.push_back(poller);
}

template<class RAW_EVENTMANAGER>
inline
bteso_SpinningEventManager<RAW_EVENTMANAGER>::~bteso_SpinningEventManager() {
        int numPollers = d_pollers.size();
        for (int i = 0; i < numPollers; ++i) {
                d_pool.deleteObject(d_pollers[i]);
        }
}

// MANIPULATORS
template <class RAW_EVENTMANAGER>
inline
int bteso_SpinningEventManager<RAW_EVENTMANAGER>::dispatch(int flags)
{
        if (d_pollers.size() == 1) {
                return d_pollers[0]->dispatch(flags);
        }

        int numDispatched = 0;
        int numPollers = d_pollers.size();
        bdet_TimeInterval zero(0, 0);
        while(1) {
                for (int i = 0; i < numPollers; ++i) {
                        numDispatched += d_pollers[i]->dispatch(zero, flags);
                }
                if (numDispatched) {
                        return numDispatched;
                }
                bcemt_ThreadUtil::sleep(d_spinPeriod);
        }
}


template <class RAW_EVENTMANAGER>
inline
int bteso_SpinningEventManager<RAW_EVENTMANAGER>::dispatch(
        const bdet_TimeInterval& timeout, int flags)
{
        if (d_pollers.size() == 1) {
                return d_pollers[0]->dispatch(timeout, flags);
        }

        int numDispatched = 0;
        int numPollers = d_pollers.size();
        bdet_TimeInterval now = bdetu_SystemTime::now();
        bdet_TimeInterval zero(0,0);
        do {
            for (int i = 0; i < numPollers; ++i) {
                numDispatched += d_pollers[i]->dispatch(zero, flags);
            }
            if (numDispatched) {
                return numDispatched;
            }
            bcemt_ThreadUtil::sleep(d_spinPeriod);
            now += d_spinPeriod;
        } while(timeout <= now);
        return 0;  // TIMED OUT
}

template <class RAW_EVENTMANAGER>
inline
int bteso_SpinningEventManager<RAW_EVENTMANAGER>::registerSocketEvent(
        const bteso_SocketHandle::Handle&   handle,
        const bteso_EventType::Type         eventType,
        const bteso_EventManager::Callback& callback)
{
    if (!RAW_EVENTMANAGER::canBeRegistered(handle)) {
        return -1;
    }
    int *associatedIndex = d_indices.lookup(handle);

    int pollerIndex;
    RAW_EVENTMANAGER *poller;
    if (associatedIndex) {
        pollerIndex = *associatedIndex;
        poller = d_pollers[pollerIndex];
        return poller->registerSocketEvent(handle, eventType, callback);
    }
    else {
        pollerIndex = d_pollers.size()  - 1;
        poller = d_pollers[pollerIndex];
        if (poller->numSockets() == RAW_EVENTMANAGER::MAX_NUM_HANDLES) {
            poller = new (d_pool) RAW_EVENTMANAGER(d_metrics_p, d_allocator_p);
            d_pollers.push_back(poller);
            ++pollerIndex;
        }
        d_indices.add(handle, pollerIndex);
        return poller->registerSocketEvent(handle, eventType, callback);
    }
}

template <class RAW_EVENTMANAGER>
inline
void bteso_SpinningEventManager<RAW_EVENTMANAGER>::deregisterSocketEvent(
        const bteso_SocketHandle::Handle& handle,
        bteso_EventType::Type       event)
{
    int *pollerIndex = d_indices.lookup(handle);
    int idx = *pollerIndex;                  // safe for future reference

    int lastPollerIndex = d_pollers.size() - 1;

    RAW_EVENTMANAGER *poller = d_pollers[idx];
    poller->deregisterSocketEvent(handle, event);
    if (0 == poller->numSocketEvents(handle)) {
        d_indices.remove(handle);
    }
    if (idx != lastPollerIndex &&
        poller->numSockets() < RAW_EVENTMANAGER::MAX_NUM_HANDLES)
    {
        // Shift a socket from the last poller to this one.
        RAW_EVENTMANAGER *last = d_pollers[lastPollerIndex];
        bteso_SocketHandle::Handle h = last->event(0).handle();
        poller->moveSocket(last, h);
        d_indices.add(h, idx);
        poller = last;
    }
    if (0 == poller->numEvents() && poller != d_pollers[0]) {
        d_pool.deleteObject(poller);
        d_pollers.erase(d_pollers.end()-1);
    }
}

template <class RAW_EVENTMANAGER>
inline
int bteso_SpinningEventManager<RAW_EVENTMANAGER>::deregisterSocket(
                const bteso_SocketHandle::Handle& handle)
{
    int *index = d_indices.lookup(handle);
    if (!index) {
        return 0;
    }

    RAW_EVENTMANAGER *poller = d_pollers[*index];
    int result = poller->deregisterSocket(handle);
    d_indices.remove(handle);

    if (*index != d_pollers.size() - 1 &&
        poller->numSockets() < RAW_EVENTMANAGER::MAX_NUM_HANDLES)
    {
        // Shift a socket from the last poller to this one.

        RAW_EVENTMANAGER *last = d_pollers[d_pollers.size() - 1];
        const bteso_Event& anEvent = last->event(0);
        poller->moveSocket(last, anEvent.handle());
        d_indices.add(anEvent.handle(), *index);
        poller = last;
    }

    if (0 == poller->numSockets() && *index != 0) {
        d_pool.deleteObject(poller);
        d_pollers.erase(d_pollers.end()-1);
    }
    return result;
}

template <class RAW_EVENTMANAGER>
inline
void bteso_SpinningEventManager<RAW_EVENTMANAGER>::deregisterAll() {
        d_indices.removeAll();
        int numPollers = d_pollers.size();

        for (int i = 1; i < numPollers; ++i) {
                d_pollers[i]->deregisterAll();
                d_pool.deleteObject(d_pollers[i]);
        }
        d_pollers.resize(1);

        d_pollers[0]->deregisterAll();
}

// ACCESSORS
template <class RAW_EVENTMANAGER>
inline
int bteso_SpinningEventManager<RAW_EVENTMANAGER>::isRegistered(
    const bteso_SocketHandle::Handle& handle,
    const bteso_EventType::Type       event) const
{
        const int *index = d_indices.lookup(handle);
        if (!index) {
                return 0;
        }
        RAW_EVENTMANAGER *poller = d_pollers[*index];
        return poller->isRegistered(handle, event);

}

template <class RAW_EVENTMANAGER>
inline
int bteso_SpinningEventManager<RAW_EVENTMANAGER>::numEventManagers() const
{
    return d_pollers.size();
}


template <class RAW_EVENTMANAGER>
inline
int bteso_SpinningEventManager<RAW_EVENTMANAGER>::numEvents() const
{
        int result = 0;
        int numPollers = d_pollers.size();
        for (int i = 0; i < numPollers; ++i) {
                result += d_pollers[i]->numEvents();
        }
        return result;
}

template <class RAW_EVENTMANAGER>
inline
int bteso_SpinningEventManager<RAW_EVENTMANAGER>::numSocketEvents (
        const bteso_SocketHandle::Handle& handle) const
{
        const int *index = d_indices.lookup(handle);
        if (!index) {
                return 0;
        }
        RAW_EVENTMANAGER *poller = d_pollers[*index];
        return poller->numSocketEvents(handle);
}


} // close namespace BloombergLP
#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
