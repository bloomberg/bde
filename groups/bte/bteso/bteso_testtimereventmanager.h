// bteso_testtimersocketeventmanager.h-*-C++-*-
#ifndef INCLUDED_BTESO_TESTTIMEREVENTMANAGER
#define INCLUDED_BTESO_TESTTIMEREVENTMANAGER

//@PURPOSE:
//
//@AUTHOR: Andrei Basov (abasov)
//
//@CLASSES:
//  bteso_TestTimerEventManager:
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

#ifndef INCLUDED_BTESO_TIMEREVENTMANAGER
#include <bteso_timereventmanager.h>
#endif

#ifndef INCLUDED_BTESO_EVENT
#include <bteso_event.h>
#endif

#ifndef INCLUDED_BTESO_PLATFORM
#include <bteso_platform.h>
#endif

#ifndef INCLUDED_BCEF_VFUNC0
#include <bcef_vfunc0.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BDET_STRING
#include <bdet_string.h>
#endif

#ifndef INCLUDED_BDEC2_MAP
#include <bdec2_map.h>
#endif

#ifndef INCLUDED_BDEC_PTRPRIORITYQUEUE
#include <bdec_ptrpriorityqueue.h>
#endif

#ifdef BTESO_PLATFORM__WIN_SOCKETS
#define FD_SETSITE 1024
#include <winsock2.h>
#endif
#ifdef BTESO_PLATFORM__BSD_SOCKETS
#include <sys/select.h>
#endif

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

namespace BloombergLP {

struct bteso_TestTimer {
    bdet_TimeInterval                    d_timeout;
    bteso_TimerEventManager::Callback    d_callback;
    bteso_TestTimer(const bdet_TimeInterval& timeout,
                    const bcef_Vfunc0& callback);
};

                   // =======================================
                   // class bteso_TestTimerEventManager
                   // =======================================

class bteso_TestTimerEventManager : public bteso_TimerEventManager
{
  // [TBD - Class description]
    bdet_String  d_script;     //
    int          d_cursor;     // Current position within a script.

    fd_set       d_readSet;
    fd_set       d_writeSet;

    bdec2_Map<bteso_Event, Callback, bteso_EventHash>
                                          d_events;

    bdec_PtrPriorityQueue<bteso_TestTimer> d_timers;

  public:
    // CREATORS
    bteso_TestTimerEventManager(const char *script);

    ~bteso_TestTimerEventManager();

    // MANIPULATORS
    int dispatch(InterruptOpt value);
        // Dispatch the registered callbacks whose associated events occur or
        // whose time is reached.  Return the number of outstanding callbacks
        // on success or a negative number on error.  If the specified
        // 'interruptFlag' is set to 1, the dispatch loop will return on any
        // asynchronous event.  If it is set to 0, the dispatch loop will
        // attempt to restart itself on asynchronous events.  All other values
        // for the 'interruptFlag' are invalid and will cause an error.

    int registerSocketEvent(const bteso_SocketHandle::Handle& handle,
                            const bteso_EventType::Type event,
                            const Callback&                   cb);
        // Register the specified 'cb' functor to be invoked whenever the
        // specified 'event' occurs on the socket specified by 'handle'.
        // Return 0 on success and a negative number on error.  No two
        // different socket events can be have callbacks registered with the
        // same socket handle other than read and write.  Any invocation of
        // this method that would cause this to occur will result in an error
        // and the callback will not be registered and -1 will be returned.
        // Note that the callback is recurring (i.e. it remains registered even
        // after it is invoked, until it is explicitly deregistered).

    void* registerTimer(const bdet_TimeInterval& timeout,
                        const Callback&          cb);
        // Register the specified 'cb' functor to be invoked when the
        // absolute time of the specified 'timeout' is reached or exceeded.
        // Return a 'void*' which is needed to deregister the callback.  Note
        // that specifying a 'timeout' previous to the current time will result
        // in the associated 'cb' being executed the first time that dispatch
        // is called.  Note also that the callback is not recurring (i.e. after
        // being invoked it is deregistered automatically).

    int deregisterSocketEvent(
            const bteso_SocketHandle::Handle& handle,
            const bteso_EventType::Type event);
        // Deregister the callback associated with the specified 'event' on the
        // specified 'handle' so that said callback will not be invoked if the
        // 'event' occurs.  Return 0 on successful removal and a negative value
        // on error.  If the specified 'handle' does not have a callback
        // associated with its 'event', -1 is returned.

    int deregisterSocket(const bteso_SocketHandle::Handle& handle)
       ;
        // Deregister all callbacks associated with any event on the specified
        // 'handle' such that no callback will be invoked if the any event
        // occurs on the specified 'handle'.  Return 0 on successful removal
        // and a negative value on error.  If the specified 'handle' does not
        // have a callback associated with any of its events, return -1.

    void deregisterAllSocketEvents();
        // Deregister all callbacks associated with any event on any socket
        // handle so that no callbacks are invoked when any event occurs on any
        // handle.  Return 0 on sucessful removal and a negative value on
        // error.  If there are no socket events to be deregistered, return -1.

    int deregisterTimer(const void *timedCb);
        // Deregister the callback associated with the specified 'timedCb'
        // (returned when the timer callback was registered) so that no
        // callback will not be invoked at the appointed time.  Return 0 on
        // successful removal and a negative value on error.  If the specified
        // 'timerCb' is not registered, return -1.

    int deregisterAllTimers();
        // Deregister all callbacks associated with any timer IDs so that no
        // callbacks are invoked when any time is reached.  Return 0 on success
        // and a negative value on error.  If there are no timer events to
        // deregister, return -1.

    void deregisterAll();
        // Deregister all callbacks associated with any event on any socket
        // handle as well as any callbacks associated with any timer IDs so
        // that no callbacks are invoked when any event occurs on any handle or
        // any time is reached.  Return 0 on success and a negative value on
        // error.  If there are no events to deregister, return -1.

    // ACCESSORS
    int numTimers() const;
        // Return the number of timers that are currently registered.

    int numSocketEvents(
                const bteso_SocketHandle::Handle& handle) const;


};

//-----------------------------------------------------------------------------
//                      INLINE FUNCTIONS' DEFINITIONS
//-----------------------------------------------------------------------------

inline bteso_TestTimer::bteso_TestTimer(const bdet_TimeInterval& timeout,
                                        const bcef_Vfunc0& callback)
: d_timeout(timeout)
, d_callback(callback)
{
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
