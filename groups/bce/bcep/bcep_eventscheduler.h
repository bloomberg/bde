// bcep_eventscheduler.h                                              -*-C++-*-
#ifndef INCLUDED_BCEP_EVENTSCHEDULER
#define INCLUDED_BCEP_EVENTSCHEDULER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a thread-safe recurring and one-time event scheduler.
//
//@CLASSES:
//    bcep_EventScheduler:                     thread-safe event scheduler
//    bcep_EventSchedulerEventHandle:          managed handle to a scheduled
//                                             event
//    bcep_EventSchedulerRecurringEventHandle: managed handle to a recurring
//                                             scheduled event
//
//
//@AUTHOR: Vlad Kliatchko (vkliatch), David Schumann (dschumann1)
//
//@DESCRIPTION: This component provides a thread-safe event scheduler.
// 'bcep_EventScheduler' implements methods to schedule and cancel recurring
// and one-time events.  All of the callbacks for these events are
// processed by a separate thread (called the dispatcher thread).  By default
// the callbacks are also executed in the dispatcher thread, but that behavior
// can be altered by providing a dispatcher functor at creation time (see
// the section "The Dispatcher Thread and the Dispatcher Functor").
//
// Events may be referred to by 'EventHandle' and 'RecurringEventHandle'
// objects, which clean up after themselves when they go out of scope, or by
// 'Event' and 'RecurringEvent' pointers, which must be released using
// 'releaseEventRaw'.  Such pointers are used in the "Raw" API of this class
// and must be used carefully.
//
///Thread Safety and "Raw" Event Pointers
///--------------------------------------
// 'bcep_EventScheduler' is thread-safe and thread-enabled, meaning that
// multiple threads may use their own instances of the class or use a shared
// instance without further synchronization.  The thread safety and correct
// behavior of the component depend on the correct usage of 'Event' pointers,
// which refer to scheduled events in the "Raw" API of this class.  In
// particular:
//..
//   * Every 'Event*'  and 'RecurringEvent*' populated by 'scheduleEventRaw'
// and 'scheduleRecurringEventRaw' must be released using 'releaseEventRaw.'
//       - Pointers are not released automatically when events are completed.
//       - Pointers are not released automatically when events are canceled.
//       - Events are not canceled when pointers to them are released.
//   * Pointers must not be used after being released.
//   * Pointers must never be shared or duplicated without using
//    'addEventRefRaw' and 'addRecurringEventRefRaw' to get additional
//     references; *each* such added reference must be released separately.
//..
// 'bcep_EventSchedulerEventHandle' and
// 'bcep_EventSchedulerRecurringEventHandle' are *const* *thread-safe*.  It
// is not safe for multiple threads to invoke non-const methods on the same
// EventHandle or RecurringEventHandle object concurrently.
//
///The Dispatcher Thread and the Dispatcher Functor
///------------------------------------------------
// The scheduler creates a single separate thread (called the *dispatcher*
// *thread*) to process all the callbacks.  The dispatcher thread executes
// the callbacks by passing them to the dispatcher functor (optionally
// specified at creation time).  The default dispatcher functor simply
// invokes the passed callback, effectively executing it in the dispatcher
// thread.  Users can alter this behavior by defining their own dispatcher
// functor (for example in order to use a thread pool or a separate thread to
// run the callbacks).  Note that the user-supplied functor will still be run
// in the dispatcher thread.
//
// CAVEAT: Using a dispatcher functor such as the example above (to execute
// the callback in a separate thread) violates the guarantees of
// cancelEventAndWait().  Users who specify a dispatcher functor that
// transfers the event to another thread for execution should not use
// cancelEventAndWait(), and should instead ensure that the lifetime of any
// object bound to an event exceeds the lifetime of the mechanism used by
// the customized dispatcher functor.
//
///Timer Resolution and Order of Execution
///---------------------------------------
// It is intended that recurring and one-time events are processed as
// closely as possible to their respective time values, and that they are
// processed in the order scheduled.  However, this component *guarantees*
// only that events will not be executed before their scheduled time.
// Generally, events that are scheduled more than 1 microsecond apart will be
// executed in the order scheduled; but different behavior may be observed when
// events are submitted after (or shortly before) their scheduled time.
//
// When events are executed in the dispatcher thread and take longer to
// complete than the time between events, the dispatcher can fall behind.  In
// this case, events will be executed in the correct order as soon as the
// dispatcher thread becomes available; once the backlog is worked off,
// events will be executed at or near their scheduled times.
//
///Usage Example 1: Simple Clock
///-----------------------------
// In this example we wish to log some statistics periodically.  We define a
// method to store the value of a variable into an array, and set up a
// scheduler to call that as a recurring event.
//..
//   bces_AtomicInt  g_data;  // Some global data we want to track
//   typedef pair<bdet_Datetime, int> Value;
//
//   void saveData(vector<Value> *array)
//   {
//      array->push_back(Value(bdetu_SystemTime::nowAsDatetimeGMT(), g_data));
//   }
//..
// We allow the scheduler to run for a short time while changing this value
// and observe that the callback is executed:
//..
//   bcep_EventScheduler scheduler;
//   vector<Value> values;
//
//   scheduler.scheduleRecurringEvent(bdet_TimeInterval(1.5),
//                                  bdef_BindUtil::bind(&saveData, &values)));
//   scheduler.start();
//   bdet_Datetime start = bdetu_SystemTime::nowAsDatetimeGMT();
//   while ((bdetu_SystemTime::nowAsDatetimeGMT() -
//                                          start).totalSeconds() < 7) {
//     ++g_data;
//   }
//   scheduler.stop();
//   ASSERT(values.size() >= 4);
//   for (int i = 0; i < values.size(); ++i) {
//     bsl::cout << "At " << values[i].first << " g_data was "
//               << values[i].second << bsl::endl;
//   }
//..
// This will display, e.g.:
//..
//     At 06MAY2008_21:19:17.092 g_data was 816196
//     At 06MAY2008_21:19:18.592 g_data was 1620749
//     At 06MAY2008_21:19:20.092 g_data was 2443358
//     At 06MAY2008_21:19:21.592 g_data was 3267721
//..
//
///Usage Example 2: Server Timeouts
///--------------------------------
// The following example shows how to use a 'bcep_EventScheduler' to
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
//            // Process the specified 'data' of the specified 'length'.  (TBD)
//    };
//
//    class my_Server {
//     // This class implements a server maintaining several connections.
//     // A connection is closed if the data for it does not arrive
//     // before a timeout (specified at the server creation time).
//
//     struct Connection {
//         bcep_EventSchedulerEventHandle d_timerId;   // handle for timeout
//                                                     // event
//
//         my_Session *d_session_p;                    // session for this
//                                                     // connection
//     };
//
//     bsl::vector<Connection*> d_connections; // maintained connections
//     bcep_EventScheduler      d_scheduler;   // timeout event scheduler
//     bdet_TimeInterval        d_ioTimeout;   // time out
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
//               bslma_Allocator *allocator = 0);
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
//                      bslma_Allocator *alloc)
// : d_connections(alloc)
// , d_scheduler(alloc)
// , d_ioTimeout(ioTimeout)
// {
//      // TBD: logic to start monitoring the arriving connections or data
//
//      d_scheduler.start();
// }
//
// my_Server::~my_Server()
// {
//     // TBD: logic to clean up
//
//     d_scheduler.stop();
// }
//
// void my_Server::newConnection(my_Server::Connection *connection)
// {
//     // TBD: logic to add 'connection' to 'd_connections'
//
//     // setup the timeout for data arrival
//     d_scheduler.scheduleEvent(
//         &connection->d_timerId,
//         bdetu_SystemTime::now() + d_ioTimeout,
//         bdef_BindUtil::bind(&my_Server::closeConnection, this, connection));
// }
//
// void my_Server::closeConnection(my_Server::Connection *connection)
// {
//     // TBD: logic to close the 'connection' and remove it from 'd_ioTimeout'
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
//     d_scheduler.scheduleEvent(&connection->d_timerId,
//         bdetu_SystemTime::now() + d_ioTimeout,
//         bdef_BindUtil::bind(&my_Server::closeConnection, this, connection));
// }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEC_SKIPLIST
#include <bcec_skiplist.h>
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

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

namespace BloombergLP {

class bslma_Allocator;
class bcep_EventSchedulerEventHandle;
class bcep_EventSchedulerRecurringEventHandle;

                             // =========================
                             // class bcep_EventScheduler
                             // =========================

class bcep_EventScheduler {
    // This class provides a thread-safe event scheduler that executes
    // callbacks in a separate "dispatcher thread."  'start' must be invoked
    // to start dispatching the callbacks.  'stop' pauses the dispatching of
    // the callbacks without removing the pending events.

  private:
    // PRIVATE TYPES
    typedef bsl::pair<bdef_Function<void(*)()>, bdet_TimeInterval>
                                                           RecurringEventData;

    typedef bcec_SkipList<bsls_PlatformUtil::Int64,
                          RecurringEventData>              RecurringEventQueue;

    typedef bcec_SkipList<bsls_PlatformUtil::Int64,
                          bdef_Function<void(*)()> >       EventQueue;

    // FRIENDS
    friend class bcep_EventSchedulerEventHandle;
    friend class bcep_EventSchedulerRecurringEventHandle;

  public:
    // PUBLIC TYPES
    struct Event {};
    struct RecurringEvent {};
       // Pointers to the opaque structures 'Event' and 'RecurringEvent' are
       // populated by the "Raw" API of 'bcep_EventScheduler'.

    typedef bcep_EventSchedulerEventHandle          EventHandle;

    typedef bcep_EventSchedulerRecurringEventHandle RecurringEventHandle;

    typedef bdef_Function<void(*)(const bdef_Function<void(*)()>&)> Dispatcher;
        // Defines a type alias for the dispatcher functor type.

  private:
    // NOT IMPLEMENTED
    bcep_EventScheduler(const bcep_EventScheduler& original);
    bcep_EventScheduler& operator=(const bcep_EventScheduler& rhs);

  private:
    // PRIVATE DATA
    EventQueue            d_eventQueue;         // events

    RecurringEventQueue   d_recurringQueue;     // recurring events

    Dispatcher            d_dispatcherFunctor;  // dispatch events

    bcemt_ThreadUtil::Handle
                          d_dispatcherThread;   // dispatcher thread handle

    bcemt_Mutex           d_mutex;              // synchronizes access to
                                                // condition variables

    bcemt_Condition       d_queueCondition;     // condition variable used to
                                                // signal when the queues need
                                                // to be checked again (when
                                                // they become non-empty or
                                                // get a new front member)

    bcemt_Condition       d_iterationCondition; // condition variable used to
                                                // signal when the dispatcher
                                                // is ready to enter next
                                                // iteration (synchronizes
                                                // 'wait' methods)

    volatile bool         d_running;            // controls the looping of the
                                                // dispatcher thread

    volatile bool         d_dispatcherAwaited;  // A thread is waiting for
                                                // the dispatcher to complete
                                                // an iteration

    RecurringEventQueue::Pair
                         *d_currentRecurringEvent;
                                                // Raw reference to the
                                                // scheduled event being
                                                // executed
    EventQueue::Pair     *d_currentEvent;
                                                // Raw reference to the
                                                // scheduled recurring event
                                                // being executed

    // PRIVATE MANIPULATORS
    bsls_PlatformUtil::Int64 chooseNextEvent(bsls_PlatformUtil::Int64 *now);
        // Pick either d_currentEvent or d_currentRecurringEvent as the next
        // event to be executed, given that the current time is the specified
        // (absolute) 'now' interval, and return the (absolute) interval of
        // the chosen event.  If both 'd_currentEvent' and
        // 'd_currentRecurringEvent' are valid, release whichever one was
        // not chosen.  If both 'd_currentEvent' and 'd_currentRecurringEvent'
        // are scheduled before 'now', choose 'd_currentEvent'.  The behavior
        // is undefined if neither d_currentEvent nor d_currentRecurringEvent
        // is valid.  Note that the argument and return value of this method
        // are expressed in terms of the number of microseconds elapsed since
        // midnight, January 1, 1970 GMT.  Also note that this method may
        // update the value of 'now' with the current system time if necessary.

    void dispatchEvents();
        // While d_running is true, execute events in the event and
        // recurring event queues at their scheduled times.  Note that this
        // method implements the dispatching thread.

    void releaseCurrentEvents();
        // Release 'd_currentRecurringEvent' and 'd_currentEvent', if they
        // refer to valid events.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcep_EventScheduler,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit bcep_EventScheduler(bslma_Allocator *basicAllocator = 0);
        // Construct an event scheduler using the default dispatcher
        // functor (see the "The dispatcher thread and the dispatcher functor"
        // section in component-level doc).  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit bcep_EventScheduler(const Dispatcher&  dispatcherFunctor,
                                 bslma_Allocator   *basicAllocator = 0);
        // Construct an event scheduler using the specified
        // 'dispatcherFunctor' (see "The dispatcher thread and the dispatcher
        // functor" section in component-level doc).  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~bcep_EventScheduler();
        // Discard all unprocessed events and destroy this object.  The
        // behavior is undefined unless the scheduler is stopped.

    // MANIPULATORS
    void cancelAllEvents();
        // Cancel all recurring and one-time events scheduled in this
        // EventScheduler.

    void cancelAllEventsAndWait();
        // Cancel all recurring and one-time events scheduled in this
        // EventScheduler.  Block until all events have either been cancelled
        // or dispatched before this call returns.  The behavior is undefined
        // if this method is invoked from the dispatcher thread.

    int cancelEvent(const Event          *handle);
    int cancelEvent(const RecurringEvent *handle);
        // Cancel the event having the specified 'handle'.  Return 0 on
        // successful cancellation, and a non-zero value if the 'handle' is
        // invalid *or* if the event has already been dispatched or canceled.

    int cancelEventAndWait(const Event          *handle);
    int cancelEventAndWait(const RecurringEvent *handle);
        // Cancel the event having the specified 'handle'.  Block until
        // the event having the specified 'handle' (if it is valid) is either
        // successfully canceled or dispatched before the call returns.
        // Return 0 on successful cancellation, and a non-zero value if the
        // 'handle' is invalid *or* if the event has already been dispatched
        // or canceled.  The behavior is undefined if this method is invoked
        // from the dispatcher thread.  Note that if the event is being
        // executed when this method is invoked, this method will block until
        // it is completed and then return a nonzero value.

    void releaseEventRaw(Event          *handle);
    void releaseEventRaw(RecurringEvent *handle);
        // Release the specified 'handle'.  Every handle reference added by
        // 'scheduleEventRaw', 'addEventRefRaw', 'scheduleRecurringEventRaw',
        // or 'addRecurringEventRefRaw' must be released using this method to
        // avoid leaking resources.  The behavior is undefined if the value of
        // 'handle' is used for any purpose after being released.

    int rescheduleEvent(const Event              *handle,
                        const bdet_TimeInterval&  newTime);
        // Reschedule the event referred to by the specified 'handle' at
        // the specified (absolute) 'newTime'.  Absolute time is defined as
        // the interval since midnight, Jan 1, 1970, GMT.  Return 0 on
        // successful reschedule, and a non-zero value if the 'handle' is
        // invalid *or* if the event has already been dispatched.

    int rescheduleEventAndWait(const Event              *handle,
                               const bdet_TimeInterval&  newTime);
        // Reschedule the event referred to by the specified 'handle' at
        // the specified (absolute) 'newTime'.  Absolute time is defined as
        // the interval since midnight, Jan 1, 1970, GMT.  Block until the
        // event having the specified 'handle' (if it is valid) is either
        // successfully rescheduled or dispatched before the call returns.
        // Return 0 on successful reschedule, and a non-zero value if the
        // 'handle' is invalid *or* if the event has already been dispatched.
        // The behavior is undefined if this method is invoked from the
        // dispatcher thread.

    void scheduleEvent(const bdet_TimeInterval&         time,
                       const bdef_Function<void(*)()>&  callback);
    void scheduleEvent(EventHandle                     *event,
                       const bdet_TimeInterval&         time,
                       const bdef_Function<void(*)()>&  callback);
        // Schedule the specified 'callback' to be dispatched at the specified
        // (absolute) 'time'.  Absolute time is defined as the interval since
        // midnight, Jan 1, 1970, GMT.  Load into the optionally specified
        // 'event' a handle that can be used to cancel the event (by invoking
        // 'cancelEvent').  Note that 'time' may be in the past, in which case
        // the event will be executed as soon as possible.

    void scheduleEventRaw(Event                           **event,
                          const bdet_TimeInterval&          time,
                          const bdef_Function<void(*)()>&   callback);
        // Schedule the specified 'callback' to be dispatched at the specified
        // (absolute) 'time'.  Absolute time is the defined as the interval
        // since midnight, Jan 1, 1970, GMT.  Load into the specified 'event'
        // pointer a handle that can be used to cancel the event (by invoking
        // 'cancelEvent').  The 'event' pointer must be released by
        // invoking 'releaseEventRaw' when it is no longer needed.

    void scheduleRecurringEvent(
            const bdet_TimeInterval&        interval,
            const bdef_Function<void(*)()>& callback,
            const bdet_TimeInterval&        startTime = bdet_TimeInterval(0));
    void scheduleRecurringEvent(
            RecurringEventHandle            *event,
            const bdet_TimeInterval&         interval,
            const bdef_Function<void(*)()>&  callback,
            const bdet_TimeInterval&         startTime = bdet_TimeInterval(0));
        // Schedule a recurring event that invokes the specified 'callback' at
        // every specified 'interval', with the first event dispatched at the
        // optionally-specified (absolute) 'startTime'.  Absolute time is
        // defined as the interval since midnight, Jan 1, 1970, GMT.  If
        // 'startTime' is not specified, the first event is dispatched at one
        // 'interval' from now.  Load into the optionally-specified 'event' a
        // handle that can be used to cancel the event (by invoking
        // 'cancelEvent').  The behavior is undefined if 'interval' is exactly
        // 0 seconds.

    void scheduleRecurringEventRaw(
           RecurringEvent                  **event,
           const bdet_TimeInterval&          interval,
           const bdef_Function<void(*)()>&   callback,
           const bdet_TimeInterval&          startTime = bdet_TimeInterval(0));
        // Schedule a recurring event that invokes the specified 'callback' at
        // every specified 'interval', with the first event dispatched at the
        // optionally-specified (absolute) 'startTime'.  Absolute time is
        // defined as the interval since midnight, Jan 1, 1970, GMT.  If
        // 'startTime' is not specified, the first event is dispatched at one
        // 'interval' from now.  Load into the specified 'event' pointer a
        // handle that can be used to cancel the event (by invoking
        // 'cancelEvent').  The 'event' pointer must be released by invoking
        // 'releaseEventRaw' when it is no longer needed.  The behavior is
        // undefined if 'interval' is exactly 0 seconds.

    int start();
        // Begin dispatching events on this scheduler.  The dispatcher thread
        // will have default attributes.  Return 0 on success, and a non-zero
        // value otherwise.  If this scheduler is already started then return
        // 0 with no effect.  The scheduler must be stopped by invoking 'stop'
        // before it is destroyed.  Note that any events scheduled in the past
        // will be dispatched immediately upon starting.

    int start(const bcemt_Attribute& threadAttributes);
        // Begin dispatching events on this scheduler, using the specified
        // 'threadAttributes' for the dispatcher thread, except the DETACHED
        // attribute will always be overridden to be joinable.  Return 0 on
        // success, and a non-zero value otherwise.  If this scheduler is
        // already started then return 0 with no effect.  The scheduler must be
        // stopped by invoking 'stop' before it is destroyed.  Note that any
        // events scheduled in the past will be dispatched immediately upon
        // starting.

    void stop();
        // End the dispatching of events on this scheduler (but do not remove
        // any pending events), and wait for any (one) currently executing
        // event to complete.  If the scheduler is already stopped then this
        // method has no effect.  This scheduler can be restarted
        // by invoking 'start'.  The behavior is undefined if this method is
        // invoked from the dispatcher thread.

    // ACCESSORS
    Event *addEventRefRaw(Event *handle) const;
        // Increment the reference count for the event referred to by the
        // specified 'handle' and return 'handle'.  There must be a
        // corresponding call to 'releaseEventRaw' when the reference is no
        // longer needed.

    RecurringEvent *addRecurringEventRefRaw(RecurringEvent *handle) const;
        // Increment the reference count for the recurring event referred to
        // by the specified 'handle' and return 'handle'.  There must be
        // a corresponding call to 'releaseEventRaw' when the reference is no
        // longer needed.

    int numEvents() const;
        // Return the number of pending and executing one-time events in this
        // scheduler.

    int numRecurringEvents() const;
        // Return the number of recurring events registered with this
        // scheduler.
};

                    // ====================================
                    // class bcep_EventSchedulerEventHandle
                    // ====================================

class bcep_EventSchedulerEventHandle
{
    // Objects of this type refer to events in the 'bcep_EventScheduler'
    // API.  They are convertible to 'const Event*' references and may be used
    // in any method which expects them.

    // PRIVATE TYPES
    typedef bcec_SkipList<bsls_PlatformUtil::Int64,
                          bdef_Function<void(*)()> > EventQueue;

    // DATA
    EventQueue::PairHandle  d_handle;

    // FRIENDS
    friend class bcep_EventScheduler;

  public:
    // PUBLIC TYPES
    typedef bcep_EventScheduler::Event Event;

    // CREATORS
    bcep_EventSchedulerEventHandle();
       // Create a new handle object that does not refer to an event.

    bcep_EventSchedulerEventHandle(const bcep_EventSchedulerEventHandle& rhs);
       // Create a new handle object referring to the same event as the
       // specified 'rhs' handle.

    ~bcep_EventSchedulerEventHandle();
       // Destroy this object and release the managed reference, if any.

    // MANIPULATORS
    bcep_EventSchedulerEventHandle& operator=(
                                    const bcep_EventSchedulerEventHandle& rhs);
       // Release this handle's reference, if any; then make this handle
       // refer to the same event as the specified 'rhs' handle.
       // Return a modifiable reference to this handle.

    void release();
       // Release the reference (if any) held by this object.

    // ACCESSORS
    operator const Event*() const;
       // Return a "raw" pointer to the event managed by this handle, or 0
       // if this handle does not manage a reference.
};

                 // =============================================
                 // class bcep_EventSchedulerRecurringEventHandle
                 // =============================================

class bcep_EventSchedulerRecurringEventHandle
{
    // Objects of this type refer to recurring events in the
    // 'bcep_EventScheduler' API.  They are convertible to
    // 'const RecurringEvent*' references and may be used in any method which
    // expects these.

    // PRIVATE TYPES
    typedef bsl::pair<bdef_Function<void(*)()>, bdet_TimeInterval>
                                                     RecurringEventData;
    typedef bcec_SkipList<bsls_PlatformUtil::Int64,
                          RecurringEventData>        RecurringEventQueue;

    // DATA
    RecurringEventQueue::PairHandle  d_handle;

    // FRIENDS
    friend class bcep_EventScheduler;

  public:
    // PUBLIC TYPES
    typedef bcep_EventScheduler::RecurringEvent RecurringEvent;

    // CREATORS
    bcep_EventSchedulerRecurringEventHandle();
       // Create a new handle object.

    bcep_EventSchedulerRecurringEventHandle(
                           const bcep_EventSchedulerRecurringEventHandle& rhs);
       // Create a new handle object referring to the same recurring event
       // as the specified 'rhs' handle.

    ~bcep_EventSchedulerRecurringEventHandle();
       // Destroy this object and release the managed reference, if any.

    // MANIPULATORS
    bcep_EventSchedulerRecurringEventHandle& operator=(
                           const bcep_EventSchedulerRecurringEventHandle& rhs);
       // Release the reference managed by this handle, if any; then make
       // this handle refer to the same recurring event as the specified
       // 'rhs' handle.  Return a modifiable reference to this event handle.

    void release();
       // Release the reference managed by this handle, if any.

    // ACCESSORS
    operator const RecurringEvent*() const;
       // Return a "raw" pointer to the recurring event managed by this
       // handle, or 0 if this handle does not manage a reference.

};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // ------------------------------------
                     // class bcep_EventSchedulerEventHandle
                     // ------------------------------------

// CREATORS
inline
bcep_EventSchedulerEventHandle::bcep_EventSchedulerEventHandle()
{
}

inline
bcep_EventSchedulerEventHandle::bcep_EventSchedulerEventHandle
                                   (const bcep_EventSchedulerEventHandle& rhs)
: d_handle(rhs.d_handle)
{
}

inline
bcep_EventSchedulerEventHandle::~bcep_EventSchedulerEventHandle()
{
}

// MANIPULATORS
inline
bcep_EventSchedulerEventHandle&
bcep_EventSchedulerEventHandle::operator=(
                                     const bcep_EventSchedulerEventHandle& rhs)
{
    d_handle = rhs.d_handle;
    return *this;
}

inline
void bcep_EventSchedulerEventHandle::release()
{
    d_handle.release();
}

// ACCESSORS
// the scoping of "Event" below should not be necessary, but xlc (versions 8
// and 9) requires it
inline
bcep_EventSchedulerEventHandle::
                  operator const bcep_EventSchedulerEventHandle::Event*() const
{
    return (const Event*)((const EventQueue::Pair*)d_handle);
}

                   // ---------------------------------------------
                   // class bcep_EventSchedulerRecurringEventHandle
                   // ---------------------------------------------

// CREATORS
inline
bcep_EventSchedulerRecurringEventHandle::
                                      bcep_EventSchedulerRecurringEventHandle()
{
}

inline
bcep_EventSchedulerRecurringEventHandle::
                                       bcep_EventSchedulerRecurringEventHandle(
                           const bcep_EventSchedulerRecurringEventHandle& rhs)
: d_handle(rhs.d_handle)
{
}

inline
bcep_EventSchedulerRecurringEventHandle::
                                     ~bcep_EventSchedulerRecurringEventHandle()
{
}

// MANIPULATORS
inline
void bcep_EventSchedulerRecurringEventHandle::release()
{
    d_handle.release();
}

inline
bcep_EventSchedulerRecurringEventHandle&
bcep_EventSchedulerRecurringEventHandle::operator=(
                            const bcep_EventSchedulerRecurringEventHandle& rhs)
{
    d_handle = rhs.d_handle;
    return *this;
}

// ACCESSORS
// the scoping of "RecurringEvent" below should not be necessary, but xlc
// (versions 8 and 9) requires it
inline
bcep_EventSchedulerRecurringEventHandle::operator
         const bcep_EventSchedulerRecurringEventHandle::RecurringEvent*() const
{
    return (const RecurringEvent*)((const RecurringEventQueue::Pair*)d_handle);
}

                          // -------------------------
                          // class bcep_EventScheduler
                          // -------------------------

// MANIPULATORS
inline
int bcep_EventScheduler::cancelEvent(const Event *handle)
{
    const EventQueue::Pair *itemPtr =
        reinterpret_cast<const EventQueue::Pair*>(handle);

    return d_eventQueue.remove(itemPtr);
}

inline
int bcep_EventScheduler::cancelEvent(const RecurringEvent *handle)
{
    const RecurringEventQueue::Pair *itemPtr =
        reinterpret_cast<const RecurringEventQueue::Pair*> (handle);

    return d_recurringQueue.remove(itemPtr);
}

inline
void bcep_EventScheduler::scheduleEvent(
                                    const bdet_TimeInterval& time,
                                    const bdef_Function<void(*)()>& callback)
{
    scheduleEventRaw(0, time, callback);
}

inline
void bcep_EventScheduler::releaseEventRaw(Event *handle)
{
    d_eventQueue.releaseReferenceRaw(
                                 reinterpret_cast<EventQueue::Pair*>(handle));
}

inline
void bcep_EventScheduler::releaseEventRaw(RecurringEvent *handle)
{
    d_recurringQueue.releaseReferenceRaw(
                         reinterpret_cast<RecurringEventQueue::Pair*>(handle));
}

inline
void bcep_EventScheduler::scheduleRecurringEvent(
                               const bdet_TimeInterval&        interval,
                               const bdef_Function<void(*)()>& callback,
                               const bdet_TimeInterval&        startTime)
{
    scheduleRecurringEventRaw(0, interval, callback, startTime);
}

// ACCESSORS
inline
bcep_EventScheduler::Event*
bcep_EventScheduler::addEventRefRaw(Event *handle) const
{
    EventQueue::Pair *h = reinterpret_cast<EventQueue::Pair*>(handle);
    return reinterpret_cast<Event*>(d_eventQueue.addPairReferenceRaw(h));
}

inline
bcep_EventScheduler::RecurringEvent*
bcep_EventScheduler::addRecurringEventRefRaw(RecurringEvent *handle) const
{
    RecurringEventQueue::Pair *h =
        reinterpret_cast<RecurringEventQueue::Pair*>(handle);
    return reinterpret_cast<RecurringEvent*>(
                                     d_recurringQueue.addPairReferenceRaw(h));
}

inline
int bcep_EventScheduler::numEvents() const
{
    return d_eventQueue.length();
}

inline
int bcep_EventScheduler::numRecurringEvents() const
{
    return d_recurringQueue.length();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
