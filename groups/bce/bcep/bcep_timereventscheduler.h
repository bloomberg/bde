// bcep_timereventscheduler.h   -*-C++-*-
#ifndef INCLUDED_BCEP_TIMEREVENTSCHEDULER
#define INCLUDED_BCEP_TIMEREVENTSCHEDULER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a thread-safe recurring and non-recurring event scheduler.
//
//@CLASSES:
//  bcep_TimerEventScheduler: thread-safe event scheduler
//
//@SEE_ALSO: bcep_eventscheduler, bcec_timequeue
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides a thread-safe event scheduler.
// It provides methods to schedule and cancel recurring and non-recurring
// events.  (A recurring event is also referred to as a clock).  The callbacks
// are processed by a separate thread (called the dispatcher thread).
// By default the callbacks are executed in the dispatcher
// thread, but this behavior can be altered by providing a dispatcher
// functor at the creation time (see the section "The dispatcher thread
// and the dispatcher functor").  Use this component for implementing timeouts,
// deferred executions, calendars and reminders, and recurring tasks, among
// other time-bound behaviors.
//
///Comparison to 'bcep_EventScheduler'
///- - - - - - - - - - - - - - - - - -
// This class has been made mostly obsolete by the newer
// 'bcep_eventscheduler', which addresses two main disadvantages of this
// component: 1) 'bcep_timereventscheduler' can only manage a finite number of
// events -- this limit is in the millions, but 'bcep_eventscheduler' has no
// such limit; and 2) accessing the queue of a 'bcep_TimerEventScheduler' is
// inefficient when there is a large number of managed events (since adding or
// removing an event involves a linear search); 'bcep_eventscheduler' has
// a more sophisticated queue which can be accessed in constant or worst-case
// log(n) time.  The advantage this component provides over
// 'bcep_eventscheduler' is that it provides light-weight handles to events
// in the queue, 'bcep_eventmanager' provides more heavy-weight
// reference-counted handles that must be released.
//
///Order of Execution of Events
///----------------------------
// It is intended that recurring and non-recurring events are processed as
// close as possible to their respective time values.  Delays and unfairness in
// thread contention can sometimes delay execution, but this component
// guarantees that (1) events are processed in increasing time order, and (2)
// are never processed sooner than their corresponding time (but could be
// processed arbitrarily long afterward, if the dispatcher thread has not been
// able to gain control in the meantime, due to thread contention or to a long
// event).
//
// Note that it is possible to schedule events in a scheduler that has not been
// started yet.  When starting a scheduler, scheduled events whose times have
// already passed will be dispatched as soon as possible after the start time,
// still in order of their corresponding time.
//
// The only exception to those guarantees are when an event 'e1' at time 'T'
// say, is already pending while another event 'e2' is scheduled at a time <=
// 'T'.  Then the dispatcher will complete the execution of 'e1' before
// dispatching 'e2'.
//
///The Dispatcher Thread and the Dispatcher Functor
///------------------------------------------------
// Between calls to 'start' and 'stop', the scheduler creates a separate thread
// (called the *dispatcher thread*) to process all the callbacks.  The
// dispatcher thread executes the callbacks by passing them to the dispatcher
// functor (optionally specified at creation time).  The default dispatcher
// functor simply invokes the passed callback, effectively executing it in the
// dispatcher thread.  Users can alter this behavior by defining their own
// dispatcher functor (for example in order to use a thread pool or a separate
// thread to run the callbacks).  In that case, the user-supplied functor will
// still be run in the dispatcher thread, different from the scheduler thread.
//
///Thread Safety
///-------------
// The 'bcep_TimerEventScheduler' class is both *fully thread-safe* (i.e., all
// non-creator methods can correctly execute concurrently), and is
// *thread-enabled* (i.e., the classes does not function correctly in a
// non-multi-threading environment).  See 'bsldoc_glossary' for complete
// definitions of *fully thread-safe* and *thread-enabled*.
//
///Usage
///-----
// The following example shows how to use a 'bcep_TimerEventScheduler' to
// implement a timeout mechanism in a server.  'my_Session' maintains several
// connections.  It closes a connection if the data for it does not arrive
// before a timeout (specified at the server creation time).
//
//..
//    class my_Session{
//        // This class encapsulates the data and state associated with a
//        // connection and provides a method 'processData' to process the
//        // incoming data for the connection.
//      public:
//        int processData(void *data, int length);
//            // Process the specified 'data' of the specified 'length'.
//    };
//
//    class my_Server {
//     // This class implements a server maintaining several connections.
//     // A connection is closed if the data for it does not arrive
//     // before a timeout (specified at the server creation time).
//
//     struct Connection {
//         bcep_TimerEventScheduler::Handle d_timerId; // handle for timeout
//                                                     // event
//
//         my_Session *d_session_p;                    // session for this
//                                                     // connection
//     };
//
//     bsl::vector<Connection*>     d_connections; // maintained connections
//     bcep_TimerEventScheduler     d_scheduler;   // timeout event scheduler
//     bdet_TimeInterval            d_ioTimeout;   // time out
//
//     void newConnection(Connection *connection);
//         // Add the specified 'connection' to this server and schedule
//         // the timeout event that closes this connection if the data
//         // for this connection does not arrive before the timeout.
//
//     void closeConnection(Connection *connection);
//         // Close the specified 'connection' and remove it from this server.
//
//     void dataAvailable(Connection *connection, void *data, int length);
//         // Return if the specified 'connection' has already timed-out.
//         // If not, cancel the existing timeout event for the 'connection',
//         // process the specified 'data' of the specified 'length' and
//         // schedule a new timeout event that closes the 'connection' if
//         // the data does not arrive before the timeout.
//
//   public:
//     my_Server(const bdet_TimeInterval& ioTimeout,
//                                             bslma_Allocator *allocator = 0);
//         // Construct a 'my_Server' object with a timeout value of
//         // 'ioTimeout' seconds.  Optionally specify a 'allocator' used to
//         // supply memory.  If 'allocator' is 0, the currently installed
//         // default allocator is used.
//
//     ~my_Server();
//         // Perform the required clean-up and destroy this object.
// };
//
// my_Server::my_Server(const bdet_TimeInterval& ioTimeout,
//                                                     bslma_Allocator *alloc)
// : d_connections(alloc)
// , d_scheduler(alloc)
// , d_ioTimeout(ioTimeout)
// {
//      // logic to start monitoring the arriving connections or data
//
//      d_scheduler.start();
// }
//
// my_Server::~my_Server()
// {
//     // logic to clean up
//
//     d_scheduler.stop();
// }
//
// void my_Server::newConnection(my_Server::Connection *connection)
// {
//     // logic to add 'connection' to the 'd_connections'
//
//     // setup the timeout for data arrival
//     connection->d_timerId = d_scheduler.scheduleEvent(
//      bdetu_SystemTime::now() + d_ioTimeout,
//      bdef_BindUtil::bind(&my_Server::closeConnection, this, connection));
// }
//
// void my_Server::closeConnection(my_Server::Connection *connection)
// {
//     // logic to close the 'connection' and remove it from 'd_ioTimeout'
// }
//
// void my_Server::dataAvailable(my_Server::Connection *connection,
//                               void                  *data,
//                               int                   length)
// {
//     // If connection has already timed out and closed, simply return.
//     if (d_scheduler.cancelEvent(connection->d_timerId)) {
//         return;                                                // RETURN
//     }
//
//     // process the data
//     connection->d_session_p->processData(data, length);
//
//     // setup the timeout for data arrival
//     connection->d_timerId = d_scheduler.scheduleEvent(
//      bdetu_SystemTime::now() + d_ioTimeout,
//      bdef_BindUtil::bind(&my_Server::closeConnection, this, connection));
// }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEC_OBJECTCATALOG
#include <bcec_objectcatalog.h>
#endif

#ifndef INCLUDED_BCEC_TIMEQUEUE
#include <bcec_timequeue.h>
#endif

#ifndef INCLUDED_BCEMA_POOL
#include <bcema_pool.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                           // ==============================
                           // class bcep_TimerEventScheduler
                           // ==============================

class bcep_TimerEventScheduler {
    // This class provides a thread-safe event scheduler.  'scheduleEvent'
    // schedules a non-recurring event, returning a handle of type
    // 'bcep_TimerEventScheduler::Handle', which can be used to cancel the
    // scheduled event by invoking 'cancelEvent'.  Similarly, 'startClock'
    // schedules a recurring event, returning a handle of type
    // 'bcep_TimerEventScheduler::Handle', which can be used to cancel the
    // clock by invoking 'cancelClock'.  'cancelAllEvents' cancels all the
    // registered events and 'cancelAllClocks' cancels all the registered
    // clocks.  The callbacks are processed by a separate thread
    // (called dispatcher thread).  By default the callbacks are executed
    // in the dispatcher thread, but this behavior can be altered by
    // providing a dispatcher functor at the creation time (see the
    // section "The dispatcher thread and the dispatcher functor").
    // 'start' must be invoked to start dispatching the callbacks.  'stop'
    // stops the dispatching of the callbacks without removing the pending
    // events.

  private:
    // PRIVATE TYPES
    struct ClockData {
        // This structure encapsulates the data associated with a clock.

        bdef_Function<void(*)()> d_callback;         // associated callback

        bdet_TimeInterval        d_periodicInterval; // associated periodic
                                                     // interval

        volatile bool            d_isCancelled;      // tracks if the
                                                     // associated clock has
                                                     // been cancelled

        int                      d_handle;           // handle for clock
                                                     // callback

        // TRAITS
        BSLALG_DECLARE_NESTED_TRAITS(ClockData,
                                     bslalg_TypeTraitUsesBslmaAllocator);

        // CREATORS
        ClockData(const bdef_Function<void(*)()>& callback,
                  const bdet_TimeInterval&        interval,
                  bslma_Allocator                *basicAllocator = 0)
        : d_callback(callback, basicAllocator)
        , d_periodicInterval(interval)
        , d_isCancelled(false)
        , d_handle(0)
        {
        }

        ClockData(const ClockData& original,
                  bslma_Allocator *basicAllocator = 0)
        : d_callback(original.d_callback, basicAllocator)
        , d_periodicInterval(original.d_periodicInterval)
        , d_isCancelled(original.d_isCancelled)
        , d_handle(original.d_handle)
        {
        }
    };

    typedef bcema_SharedPtr<ClockData>                    ClockDataPtr;
    typedef bcec_TimeQueue<ClockDataPtr>                  ClockTimeQueue;
    typedef bcec_TimeQueueItem<bdef_Function<void(*)()> > EventItem;
    typedef bcec_TimeQueue<bdef_Function<void(*)()> >     EventTimeQueue;

  public:
    // TYPES
    typedef int Handle;
        // Defines a type alias for a handle that identifies a scheduled clock
        // or event.

    typedef bdef_Function<void(*)(const bdef_Function<void(*)()>&)> Dispatcher;
        // Defines a type alias for the dispatcher functor type.

    typedef bcec_TimeQueue<bdef_Function<void(*)()> >::Key EventKey;
        // Defines a type alias for a user-supplied key for identifying events.

    // CONSTANTS
    enum {
        BCEP_INVALID_HANDLE = -1  // value of an invalid event or clock handle
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , INVALID_HANDLE = BCEP_INVALID_HANDLE
#endif
    };

  private:
    // DATA
    bslma_Allocator *d_allocator_p;        // memory allocator (held)

    bcema_Pool       d_clockDataAllocator; // pool for 'ClockData' objects

    EventTimeQueue   d_eventTimeQueue;     // time queue for non recurring
                                           // events

    ClockTimeQueue   d_clockTimeQueue;     // time queue for clock events

    bcec_ObjectCatalog<ClockDataPtr>
                     d_clocks;             // catalog of clocks

    bcemt_Mutex      d_mutex;              // mutex used to control access to
                                           // this timer event scheduler

    bcemt_Condition  d_condition;          // condition variable used to
                                           // control access to this timer
                                           // event scheduler

    bcemt_ThreadUtil::Handle
                     d_dispatcherThread;   // handle of the dispatcher thread

    Dispatcher       d_dispatcherFunctor;  // functor used to dispatch events

    volatile int     d_running;            // indicates if the timer event
                                           // scheduler is running

    volatile int     d_iterations;         // dispatcher cycle iteration number

    bsl::vector<EventItem>
                     d_pendingEventItems;  // array of pending event callbacks

    int              d_currentEventIndex;  // index (in the array
                                           // 'd_pendingEventItems') of the
                                           // current event callback being
                                           // processed by dispatcher thread

    bces_AtomicInt   d_numEvents;          // the number of events
                                           // currently registered
                                           // and/or pending dispatch
                                           // (current callback is NOT
                                           // counted)

    bces_AtomicInt   d_numClocks;          // number of clocks currently
                                           // registered

    // NOT IMPLEMENTED
    bcep_TimerEventScheduler(const bcep_TimerEventScheduler& original);
    bcep_TimerEventScheduler& operator=(const bcep_TimerEventScheduler& rhs);

    // FRIENDS
    friend struct bcep_TimerEventSchedulerDispatcher;

  private:
    // PRIVATE MANIPULATORS
    void yieldToDispatcher();
        // Repeatedly wake up dispatcher thread until it noticeably starts
        // running.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcep_TimerEventScheduler,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit bcep_TimerEventScheduler(bslma_Allocator *basicAllocator = 0);
        // Construct a timer event scheduler using the default dispatcher
        // functor (see the "The dispatcher thread and the dispatcher functor"
        // section in component level doc).  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit bcep_TimerEventScheduler(const Dispatcher&  dispatcherFunctor,
                                      bslma_Allocator   *basicAllocator = 0);
        // Construct a timer event scheduler using the specified
        // 'dispatcherFunctor' (see "The dispatcher thread and the dispatcher
        // functor" section in component level doc).  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    bcep_TimerEventScheduler(int              numEvents,
                             int              numClocks,
                             bslma_Allocator *basicAllocator = 0);
        // Construct a timer event scheduler using the default dispatcher
        // functor (see the "The dispatcher thread and the dispatcher functor"
        // section in component level doc) that has the capability to
        // concurrently schedule *at* *least* the specified 'numEvents' and
        // 'numClocks'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // '0 <= numEvents < 2**24' and '0 <= numClocks < 2**24'.

    bcep_TimerEventScheduler(int                numEvents,
                             int                numClocks,
                             const Dispatcher&  dispatcherFunctor,
                             bslma_Allocator   *basicAllocator = 0);
        // Construct a timer event scheduler using the specified
        // 'dispatcherFunctor' (see "The dispatcher thread and the dispatcher
        // functor" section in component level doc) that has the capability to
        // concurrently schedule *at* *least* the specified 'numEvents' and
        // 'numClocks'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // '0 <= numEvents < 2**24' and '0 <= numClocks < 2**24'.

    ~bcep_TimerEventScheduler();
        // Stop this scheduler, discard all the unprocessed events and destroy
        // this object.

    // MANIPULATORS
    int start();
        // Start dispatching events on this scheduler.  The dispatcher thread
        // will have default attributes.  Return 0 on success, and a non-zero
        // result otherwise.  If this scheduler has already started then this
        // invocation has no effect and 0 is returned.  This scheduler can be
        // stopped by invoking 'stop'.  Note that any event whose time has
        // already passed is pending and will be dispatched immediately.

    int start(const bcemt_Attribute& threadAttributes);
        // Start dispatching events on this scheduler, using the specified
        // 'threadAttributes' for the dispatcher thread, except the DETACHED
        // attribute will always be overridden to be joinable.  Return 0
        // on success, and a non-zero result otherwise.  If this scheduler has
        // already started then this invocation has no effect and 0 is
        // returned.  This scheduler can be stopped by invoking 'stop'.  Note
        // that any event whose time has already passed is pending and will be
        // dispatched immediately.

    void stop();
        // Stop dispatching events on this scheduler, but do not remove
        // any pending event.  If this scheduler is already stopped then this
        // invocation has no effect.  This scheduler can be restarted
        // by invoking 'start'.

    Handle scheduleEvent(const bdet_TimeInterval&        time,
                         const bdef_Function<void(*)()>& callback,
                         const EventKey&                 key = EventKey(0));
        // Schedule the specified 'callback' to be dispatched at the specified
        // (absolute) 'time' and return a handle that can be used to cancel the
        // 'callback' (by invoking 'cancelEvent').  Optionally use the
        // specified 'key' to uniquely identify the event.

    int rescheduleEvent(Handle                   handle,
                        const bdet_TimeInterval& newTime,
                        bool                     wait = false);
    int rescheduleEvent(Handle                   handle,
                        const EventKey&          key,
                        const bdet_TimeInterval& newTime,
                        bool                     wait = false);

        // Reschedule the event having the specified 'handle' at the specified
        // (absolute) 'newTime'.  Optionally use the specified 'key' to
        // uniquely identify the event.  If the optionally specified 'wait' is
        // true, then ensure that the event having the specified 'handle' (if
        // it is valid) is either successfully rescheduled or dispatched before
        // the call returns.  Return 0 on successful reschedule, and a non-zero
        // value if the 'handle' is invalid *or* if the event has already been
        // dispatched *or* if the event has not yet been dispatched but will
        // soon be dispatched.  If this method is being invoked from the
        // dispatcher thread then the 'wait' is ignored to avoid deadlock.

    int cancelEvent(Handle          handle,
                    bool            wait = false);
    int cancelEvent(Handle          handle,
                    const EventKey& key,
                    bool            wait = false);
        // Cancel the event having the specified 'handle'.  Optionally use the
        // specified 'key' to uniquely identify the event.  If the optionally
        // specified 'wait' is true, then ensure that the dispatcher thread
        // has resumed execution before returning.  Return 0 on successful
        // cancellation, and a non-zero value if the 'handle' is invalid *or*
        // if it is too late to cancel the event.  If this method is being
        // invoked from the dispatcher thread then the 'wait' is ignored
        // to avoid deadlock.

    void cancelAllEvents(bool wait = false);
        // Cancel all the events.  If the optionally specified 'wait' is true,
        // then ensure any event still in this scheduler is either cancelled or
        // has been dispatched before this call returns.  If this method is
        // being invoked from the dispatcher thread then the 'wait' is ignored
        // to avoid deadlock.

    Handle startClock(
             const bdet_TimeInterval&        interval,
             const bdef_Function<void(*)()>& callback,
             const bdet_TimeInterval&        startTime = bdet_TimeInterval(0));
        // Schedule a recurring event that invokes the specified 'callback' at
        // every specified 'interval', starting at the optionally specified
        // (absolute) 'startTime'.  Return an identifier that can be use to
        // cancel the clock (by invoking 'cancelClock').  If no start time is
        // specified, it is assumed to be the 'interval' time from now.

    int cancelClock(Handle handle, bool wait = false);
        // Cancel the clock having the specified 'handle'.  If the optionally
        // specified 'wait' is true, then ensure that any scheduled event for
        // the clock having the specified 'handle' is either cancelled or have
        // been dispatched before this call returns.  Return 0 on success, and
        // a non-zero value if the 'handle' is invalid.  If this method is
        // being invoked from the dispatcher thread, then the 'wait' is ignored
        // to avoid deadlock.

    void cancelAllClocks(bool wait = false);
        // Cancel all clocks.  If the optionally specified 'wait' is true, then
        // ensure that any clock event still in this scheduler is either
        // cancelled or has been dispatched before this call returns.  If this
        // method is being invoked from the dispatcher thread, then the 'wait'
        // is ignored to avoid deadlock.

    // ACCESSORS
    int numClocks() const;
        // Return a *snapshot* of the number of registered clocks with this
        // scheduler.

    int numEvents() const;
        // Return a *snapshot* of the number of pending events and events being
        // dispatched in this scheduler.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // ------------------------
                          // bcep_TimerEventScheduler
                          // ------------------------

// MANIPULATORS
inline
int bcep_TimerEventScheduler::cancelEvent(
        bcep_TimerEventScheduler::Handle handle,
        bool                             wait)
{
    return cancelEvent(handle, EventKey(0), wait);
}

inline
int bcep_TimerEventScheduler::rescheduleEvent(
        bcep_TimerEventScheduler::Handle handle,
        const bdet_TimeInterval&         newTime,
        bool                             wait)
{
    return rescheduleEvent(handle, EventKey(0), newTime, wait);
}

// ACCESSORS
inline
int bcep_TimerEventScheduler::numClocks() const
{
    return d_numClocks;
}

inline
int bcep_TimerEventScheduler::numEvents() const
{
    return d_numEvents;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
