// bdlmt_timereventscheduler.h                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLMT_TIMEREVENTSCHEDULER
#define INCLUDED_BDLMT_TIMEREVENTSCHEDULER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-safe recurring and non-recurring event scheduler.
//
//@CLASSES:
//  bdlmt::TimerEventScheduler: thread-safe event scheduler
//
//@SEE_ALSO: bdlmt_eventscheduler, bdlcc_timequeue
//
//@DESCRIPTION: This component provides a thread-safe event scheduler,
// 'bdlmt::TimerEventScheduler'.  It provides methods to schedule and cancel
// recurring and non-recurring events.  (A recurring event is also referred to
// as a clock).  The callbacks are processed by a separate thread (called the
// dispatcher thread).  By default the callbacks are executed in the dispatcher
// thread, but this behavior can be altered by providing a dispatcher functor
// at the creation time (see the section "The dispatcher thread and the
// dispatcher functor").  Use this component for implementing timeouts,
// deferred executions, calendars and reminders, and recurring tasks, among
// other time-bound behaviors.
//
///Comparison to 'bdlmt::EventScheduler'
///- - - - - - - - - - - - - - - - - - -
// This class has been made mostly obsolete by the newer
// 'bdlmt_eventscheduler', which addresses two main disadvantages of this
// component: 1) 'bdlmt_timereventscheduler' can only manage a finite number of
// events -- this limit is in the millions, but 'bdlmt_eventscheduler' has no
// such limit; and 2) accessing the queue of a 'bdlmt::TimerEventScheduler' is
// inefficient when there is a large number of managed events (since adding or
// removing an event involves a linear search); 'bdlmt_eventscheduler' has a
// more sophisticated queue which can be accessed in constant or worst-case
// log(n) time.  The advantage this component provides over
// 'bdlmt_eventscheduler' is that it provides light-weight handles to events in
// the queue, while 'bdlmt_eventscheduler' provides more heavy-weight
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
// The 'bdlmt::TimerEventScheduler' class is both *fully thread-safe* (i.e.,
// all non-creator methods can correctly execute concurrently), and is
// *thread-enabled* (i.e., the classes does not function correctly in a
// non-multi-threading environment).  See 'bsldoc_glossary' for complete
// definitions of *fully thread-safe* and *thread-enabled*.
//
///Supported Clock-Types
///---------------------
// The component 'bsls::SystemClockType' supplies the enumeration indicating
// the system clock on which times supplied to other methods should be based.
// If the clock type indicated at construction is
// 'bsls::SystemClockType::e_REALTIME', time should be expressed as an absolute
// offset since 00:00:00 UTC, January 1, 1970 (which matches the epoch used in
// 'bdlt::CurrentTime::now(bsls::SystemClockType::e_REALTIME)'.  If the clock
// type indicated at construction is 'bsls::SystemClockType::e_MONOTONIC', time
// should be expressed as an absolute offset since the epoch of this clock
// (which matches the epoch used in
// 'bdlt::CurrentTime::now(bsls::SystemClockType::e_MONOTONIC)'.
//
///Usage
///-----
// The following example shows how to use a 'bdlmt::TimerEventScheduler' to
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
//         bdlmt::TimerEventScheduler::Handle d_timerId; // handle for timeout
//                                                     // event
//
//         my_Session *d_session_p;                    // session for this
//                                                     // connection
//     };
//
//     bsl::vector<Connection*>     d_connections; // maintained connections
//     bdlmt::TimerEventScheduler     d_scheduler;   // timeout event scheduler
//     bsls::TimeInterval            d_ioTimeout;   // time out
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
//     my_Server(const bsls::TimeInterval&  ioTimeout,
//               bslma::Allocator          *allocator = 0);
//         // Construct a 'my_Server' object with a timeout value of
//         // 'ioTimeout' seconds.  Optionally specify a 'allocator' used to
//         // supply memory.  If 'allocator' is 0, the currently installed
//         // default allocator is used.
//
//     ~my_Server();
//         // Perform the required clean-up and destroy this object.
// };
//
// my_Server::my_Server(const bsls::TimeInterval&  ioTimeout,
//                      bslma::Allocator          *allocator)
// : d_connections(allocator)
// , d_scheduler(allocator)
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
//        bdlt::CurrentTime::now() + d_ioTimeout,
//        bdlf::BindUtil::bind(&my_Server::closeConnection, this, connection));
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
//        bdlt::CurrentTime::now() + d_ioTimeout,
//        bdlf::BindUtil::bind(&my_Server::closeConnection, this, connection));
// }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLCC_OBJECTCATALOG
#include <bdlcc_objectcatalog.h>
#endif

#ifndef INCLUDED_BDLCC_TIMEQUEUE
#include <bdlcc_timequeue.h>
#endif

#ifndef INCLUDED_BDLMA_CONCURRENTPOOL
#include <bdlma_concurrentpool.h>
#endif

#ifndef INCLUDED_BSLMT_THREADATTRIBUTES
#include <bslmt_threadattributes.h>
#endif

#ifndef INCLUDED_BSLMT_CONDITION
#include <bslmt_condition.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLMT_THREADUTIL
#include <bslmt_threadutil.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLS_SYSTEMCLOCKTYPE
#include <bsls_systemclocktype.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_MEMORY
#include <bsl_memory.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace bdlmt {

struct TimerEventSchedulerDispatcher;

                         // =========================
                         // class TimerEventScheduler
                         // =========================

class TimerEventScheduler {
    // This class provides a thread-safe event scheduler.  'scheduleEvent'
    // schedules a non-recurring event, returning a handle of type
    // 'TimerEventScheduler::Handle', which can be used to cancel the scheduled
    // event by invoking 'cancelEvent'.  Similarly, 'startClock' schedules a
    // recurring event, returning a handle of type
    // 'TimerEventScheduler::Handle', which can be used to cancel the clock by
    // invoking 'cancelClock'.  'cancelAllEvents' cancels all the registered
    // events and 'cancelAllClocks' cancels all the registered clocks.  The
    // callbacks are processed by a separate thread (called dispatcher thread).
    // By default the callbacks are executed in the dispatcher thread, but this
    // behavior can be altered by providing a dispatcher functor at the
    // creation time (see the section "The dispatcher thread and the dispatcher
    // functor").  'start' must be invoked to start dispatching the callbacks.
    // 'stop' stops the dispatching of the callbacks without removing the
    // pending events.

  private:
    // PRIVATE TYPES
    struct ClockData {
        // This structure encapsulates the data associated with a clock.

        bsl::function<void()>     d_callback;          // associated callback

        bsls::TimeInterval        d_periodicInterval;  // associated periodic
                                                       // interval

        volatile bool             d_isCancelled;       // tracks if the
                                                       // associated clock has
                                                       // been cancelled

        int                       d_handle;            // handle for clock
                                                       // callback

        // TRAITS
        BSLALG_DECLARE_NESTED_TRAITS(ClockData,
                                     bslalg::TypeTraitUsesBslmaAllocator);

        // CREATORS
        ClockData(const bsl::function<void()>&  callback,
                  const bsls::TimeInterval&     interval,
                  bslma::Allocator             *basicAllocator = 0)
        : d_callback(bsl::allocator_arg_t(),
                     bsl::allocator<bsl::function<void()> >(basicAllocator),
                     callback)
        , d_periodicInterval(interval)
        , d_isCancelled(false)
        , d_handle(0)
        {
        }

        ClockData(const ClockData&  original,
                  bslma::Allocator *basicAllocator = 0)
        : d_callback(bsl::allocator_arg_t(),
                     bsl::allocator<bsl::function<void()> >(basicAllocator),
                     original.d_callback)
        , d_periodicInterval(original.d_periodicInterval)
        , d_isCancelled(original.d_isCancelled)
        , d_handle(original.d_handle)
        {
        }
    };

    typedef bsl::shared_ptr<ClockData>                   ClockDataPtr;
    typedef bdlcc::TimeQueue<ClockDataPtr>               ClockTimeQueue;
    typedef bdlcc::TimeQueueItem<bsl::function<void()> > EventItem;
    typedef bdlcc::TimeQueue<bsl::function<void()> >     EventTimeQueue;

  public:
    // TYPES
    typedef int Handle;
        // Defines a type alias for a handle that identifies a scheduled clock
        // or event.

    typedef bsl::function<void(const bsl::function<void()>&)> Dispatcher;
        // Defines a type alias for the dispatcher functor type.

    typedef bdlcc::TimeQueue<bsl::function<void()> >::Key     EventKey;
        // Defines a type alias for a user-supplied key for identifying events.

    // CONSTANTS
    enum {
        e_INVALID_HANDLE = -1  // value of an invalid event or clock handle
    };

  private:
    // DATA
    bslma::Allocator *d_allocator_p;        // memory allocator (held)

    bsls::SystemClockType::Enum
                      d_clockType;          // clock type used

    bdlma::ConcurrentPool
                      d_clockDataAllocator; // pool for 'ClockData' objects

    EventTimeQueue    d_eventTimeQueue;     // time queue for non recurring
                                            // events

    ClockTimeQueue    d_clockTimeQueue;     // time queue for clock events

    bdlcc::ObjectCatalog<ClockDataPtr>
                      d_clocks;             // catalog of clocks

    bslmt::Mutex      d_mutex;              // mutex used to control access to
                                            // this timer event scheduler

    bslmt::Condition  d_condition;          // condition variable used to
                                            // control access to this timer
                                            // event scheduler

    bslmt::ThreadUtil::Handle
                      d_dispatcherThread;   // handle of the dispatcher thread

    Dispatcher        d_dispatcherFunctor;  // functor used to dispatch events

    volatile int      d_running;            // indicates if the timer event
                                            // scheduler is running

    volatile int      d_iterations;         // dispatcher cycle iteration
                                            // number

    bsl::vector<EventItem>
                      d_pendingEventItems;  // array of pending event callbacks

    int               d_currentEventIndex;  // index (in the array
                                            // 'd_pendingEventItems') of the
                                            // current event callback being
                                            // processed by dispatcher thread

    bsls::AtomicInt   d_numEvents;          // the number of events currently
                                            // registered and/or pending
                                            // dispatch (current callback is
                                            // NOT counted)

    bsls::AtomicInt   d_numClocks;          // number of clocks currently
                                            // registered

    // NOT IMPLEMENTED
    TimerEventScheduler(const TimerEventScheduler& original);
    TimerEventScheduler& operator=(const TimerEventScheduler& rhs);

    // FRIENDS
    friend struct TimerEventSchedulerDispatcher;

  private:
    // PRIVATE MANIPULATORS
    void yieldToDispatcher();
        // Repeatedly wake up dispatcher thread until it noticeably starts
        // running.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(TimerEventScheduler,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit TimerEventScheduler(bslma::Allocator *basicAllocator = 0);
        // Construct an event scheduler using the default dispatcher functor
        // (see the "The dispatcher thread and the dispatcher functor" section
        // in component-level doc) and use the realtime clock epoch for all
        // time intervals (see {Supported Clock-Types} in the component
        // documentation).  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    explicit TimerEventScheduler(
                              bsls::SystemClockType::Enum  clockType,
                              bslma::Allocator            *basicAllocator = 0);
        // Construct an event scheduler using the default dispatcher functor
        // (see the "The dispatcher thread and the dispatcher functor" section
        // in component-level doc) and use the specified 'clockType' to
        // indicate the epoch used for all time intervals (see {Supported
        // Clock-Types} in the component documentation).  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit TimerEventScheduler(const Dispatcher&  dispatcherFunctor,
                                 bslma::Allocator  *basicAllocator = 0);
        // Construct an event scheduler using the specified 'dispatcherFunctor'
        // (see "The dispatcher thread and the dispatcher functor" section in
        // component-level doc) and use the realtime clock epoch for all time
        // intervals (see {Supported Clock-Types} in the component
        // documentation).  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    explicit TimerEventScheduler(
                              const Dispatcher&            dispatcherFunctor,
                              bsls::SystemClockType::Enum  clockType,
                              bslma::Allocator            *basicAllocator = 0);
        // Construct an event scheduler using the specified 'dispatcherFunctor'
        // (see "The dispatcher thread and the dispatcher functor" section in
        // component-level doc) and use the specified 'clockType' to indicate
        // the epoch used for all time intervals (see {Supported Clock-Types}
        // in the component documentation).  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    TimerEventScheduler(int               numEvents,
                        int               numClocks,
                        bslma::Allocator *basicAllocator = 0);
        // Construct a timer event scheduler using the default dispatcher
        // functor (see the "The dispatcher thread and the dispatcher functor"
        // section in component level doc) that has the capability to
        // concurrently schedule *at* *least* the specified 'numEvents' and
        // 'numClocks' and use the realtime clock epoch for all time intervals
        // (see {Supported Clock-Types} in the component documentation).
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '0 <= numEvents < 2**24' and
        // '0 <= numClocks < 2**24'.

    TimerEventScheduler(int                          numEvents,
                        int                          numClocks,
                        bsls::SystemClockType::Enum  clockType,
                        bslma::Allocator            *basicAllocator = 0);
        // Construct a timer event scheduler using the default dispatcher
        // functor (see the "The dispatcher thread and the dispatcher functor"
        // section in component level doc) that has the capability to
        // concurrently schedule *at* *least* the specified 'numEvents' and
        // 'numClocks' and use the specified 'clockType' to indicate the epoch
        // used for all time intervals (see {Supported Clock-Types} in the
        // component documentation).  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  The behavior is undefined
        // unless '0 <= numEvents < 2**24' and '0 <= numClocks < 2**24'.

    TimerEventScheduler(int                numEvents,
                        int                numClocks,
                        const Dispatcher&  dispatcherFunctor,
                        bslma::Allocator  *basicAllocator = 0);
        // Construct a timer event scheduler using the specified
        // 'dispatcherFunctor' (see "The dispatcher thread and the dispatcher
        // functor" section in component level doc) that has the capability to
        // concurrently schedule *at* *least* the specified 'numEvents' and
        // 'numClocks' and use the realtime clock epoch for all time intervals
        // (see {Supported Clock-Types} in the component documentation).
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '0 <= numEvents < 2**24' and
        // '0 <= numClocks < 2**24'.

    TimerEventScheduler(int                          numEvents,
                        int                          numClocks,
                        const Dispatcher&            dispatcherFunctor,
                        bsls::SystemClockType::Enum  clockType,
                        bslma::Allocator            *basicAllocator = 0);
        // Construct a timer event scheduler using the specified
        // 'dispatcherFunctor' (see "The dispatcher thread and the dispatcher
        // functor" section in component level doc) that has the capability to
        // concurrently schedule *at* *least* the specified 'numEvents' and
        // 'numClocks' and use the specified 'clockType' to indicate the epoch
        // used for all time intervals (see {Supported Clock-Types} in the
        // component documentation).  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  The behavior is undefined
        // unless '0 <= numEvents < 2**24' and '0 <= numClocks < 2**24'.

    ~TimerEventScheduler();
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

    int start(const bslmt::ThreadAttributes& threadAttributes);
        // Start dispatching events on this scheduler, using the specified
        // 'threadAttributes' for the dispatcher thread, except the DETACHED
        // attribute will always be overridden to be joinable.  Return 0 on
        // success, and a non-zero result otherwise.  If this scheduler has
        // already started then this invocation has no effect and 0 is
        // returned.  This scheduler can be stopped by invoking 'stop'.  Note
        // that any event whose time has already passed is pending and will be
        // dispatched immediately.

    void stop();
        // Stop dispatching events on this scheduler, but do not remove any
        // pending event.  Block until all pending jobs are finished, then
        // terminate the dispatcher thread and return.  If this scheduler is
        // already stopped then this invocation has no effect.  The behavior is
        // undefined if this function is called by a job enqueued to the
        // scheduler that is to be stopped.  This scheduler can be restarted by
        // invoking 'start'.

    Handle scheduleEvent(const bsls::TimeInterval&    time,
                         const bsl::function<void()>& callback,
                         const EventKey&              key = EventKey(0));
        // Schedule the specified 'callback' to be dispatched at the specified
        // 'time' and return a handle that can be used to cancel the 'callback'
        // (by invoking 'cancelEvent').  Optionally specify 'key' to uniquely
        // identify the event.  The 'time' is an absolute time represented as
        // an interval from some epoch, which is detemined by the clock
        // indicated at construction (see {'Supported Clock-Types'} in the
        // component documentation).

    int rescheduleEvent(Handle                    handle,
                        const bsls::TimeInterval& newTime,
                        bool                      wait = false);
    int rescheduleEvent(Handle                    handle,
                        const EventKey&           key,
                        const bsls::TimeInterval& newTime,
                        bool                      wait = false);

        // Reschedule the event having the specified 'handle' at the specified
        // 'newTime'.  Optionally use the specified 'key' to uniquely identify
        // the event.  If the optionally specified 'wait' is true, then ensure
        // that the event having the specified 'handle' (if it is valid) is
        // either successfully rescheduled or dispatched before the call
        // returns.  Return 0 on successful reschedule, and a non-zero value if
        // the 'handle' is invalid *or* if the event has already been
        // dispatched *or* if the event has not yet been dispatched but will
        // soon be dispatched.  If this method is being invoked from the
        // dispatcher thread then the 'wait' is ignored to avoid deadlock.  The
        // 'newTime' is an absolute time represented as an interval from some
        // epoch, which is detemined by the clock indicated at construction
        // (see {'Supported Clock-Types'} in the component documentation).

    int cancelEvent(Handle          handle,
                    bool            wait = false);
    int cancelEvent(Handle          handle,
                    const EventKey& key,
                    bool            wait = false);
        // Cancel the event having the specified 'handle'.  Optionally use the
        // specified 'key' to uniquely identify the event.  If the optionally
        // specified 'wait' is true, then ensure that the dispatcher thread has
        // resumed execution before returning.  Return 0 on successful
        // cancellation, and a non-zero value if the 'handle' is invalid *or*
        // if it is too late to cancel the event.  If this method is being
        // invoked from the dispatcher thread then the 'wait' is ignored to
        // avoid deadlock.

    void cancelAllEvents(bool wait = false);
        // Cancel all the events.  If the optionally specified 'wait' is true,
        // then ensure any event still in this scheduler is either cancelled or
        // has been dispatched before this call returns.  If this method is
        // being invoked from the dispatcher thread then the 'wait' is ignored
        // to avoid deadlock.

    Handle startClock(
               const bsls::TimeInterval&    interval,
               const bsl::function<void()>& callback,
               const bsls::TimeInterval&    startTime = bsls::TimeInterval(0));
        // Schedule a recurring event that invokes the specified 'callback' at
        // every specified 'interval', starting at the optionally specified
        // 'startTime'.  Return an identifier that can be use to cancel the
        // clock (by invoking 'cancelClock').  If no start time is specified,
        // it is assumed to be the 'interval' time from now.  The 'startTime'
        // is an absolute time represented as an interval from some epoch,
        // which is detemined by the clock indicated at construction (see
        // {'Supported Clock-Types'} in the component documentation).

    int cancelClock(Handle handle, bool wait = false);
        // Cancel the clock having the specified 'handle'.  If the optionally
        // specified 'wait' is true, then ensure that any scheduled event for
        // the clock having 'handle' is either cancelled or has been dispatched
        // before this call returns.  Return 0 on success, and a non-zero value
        // if the 'handle' is invalid.  If this method is being invoked from
        // the dispatcher thread, then the 'wait' is ignored to avoid deadlock.

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

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                            // -------------------
                            // TimerEventScheduler
                            // -------------------

// MANIPULATORS
inline
int TimerEventScheduler::cancelEvent(TimerEventScheduler::Handle handle,
                                     bool                        wait)
{
    return cancelEvent(handle, EventKey(0), wait);
}

inline
int TimerEventScheduler::rescheduleEvent(TimerEventScheduler::Handle handle,
                                         const bsls::TimeInterval&   newTime,
                                         bool                        wait)
{
    return rescheduleEvent(handle, EventKey(0), newTime, wait);
}

// ACCESSORS
inline
int TimerEventScheduler::numClocks() const
{
    return d_numClocks;
}

inline
int TimerEventScheduler::numEvents() const
{
    return d_numEvents;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
