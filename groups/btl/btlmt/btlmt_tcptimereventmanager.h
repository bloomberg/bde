// btlmt_tcptimereventmanager.h                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLMT_TCPTIMEREVENTMANAGER
#define INCLUDED_BTLMT_TCPTIMEREVENTMANAGER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a thread-enabled multiplexor of socket events and timers.
//
//@CLASSES:
//  btlmt::TcpTimerEventManager: thread-enabled socket event/timer multiplexor
//
//@SEE_ALSO: btlmt_channelpool bdlmt_threadpool bslmt_threadutil
//
//@DESCRIPTION: This component provides a concrete implementation of a
// thread-enabled multiplexer for socket events and timers.  An interested
// party can register a callback to be invoked whenever a particular event
// occurs on a particular socket, or when a timer expires.  Socket events have
// a permanent semantics (i.e., once registered, a callback is invoked until it
// is explicitly deregistered).  Timers have "one-time" semantics (i.e., a
// timer callback is invoked at most once).  Registering the same socket handle
// and the same event with different instances of this event manager may result
// in undefined behavior.  The timers are unstable with respect to
// registration; that is, if two timer callbacks are registered with the same
// time, they maybe invoked in an order different from the order of
// registration.  Socket events take priority over timers; if a socket event
// and a timer occur at the same time, a socket event callback is invoked
// first.
//
// All registered callbacks are invoked from an internal thread managed by
// 'this' event manager if its underlying thread is enabled (see below).  If
// the thread is disabled, the queries and registrations are processed (in a
// thread-safe fashion) but no callbacks are invoked.  On UNIX platforms all
// signals are disabled for this thread.
//
// An event manager can optimize its performance based on a hint provided at
// construction.  Generally speaking, a particular usage falls into two
// categories: socket events are registered: 1)frequently, and 2)infrequently.
// On some platforms, a significant performance improvement can be achieved if
// the registrations are infrequent.  For this situation, the appropriate hint
// should be provided to this event manager at construction for optimal
// performance.
//
///Thread Safety
///-------------
// This event manager is *thread* *safe*, i.e., operations can be invoked
// simultaneously on a single instance of this event manager from multiple
// threads without undesirable side-effects.
//
// To the extent that this component uses local synchronization primitives
// internally to protect access to different pieces of private data, and to
// maximize the ability of different threads to operate concurrently on a
// single instance of this component, it is *thread* *enabled*.
//
// This component is not *async-safe*, i.e., one or more methods cannot be
// invoked safely from a signal handler.
//
///Performance
///-----------
// This component minimizes the number of system calls for the set of
// callbacks.  A significant performance improvement can be achieved if an
// appropriate registration frequency "hint" is provided.  Supplying a false
// hint (e.g., indicating infrequent registration when the reverse is true) may
// result in significant performance degradation.
//
// Given that T is the number of timers registered and S is the number of
// socket events registered, the following complexity is guaranteed by this
// component:
//..
//  NOTE: The following table is a place holder and needs to be rethought.
//  +========================================================================+
//  |        FUNCTION          | EXPECTED COMPLEXITY | WORST CASE COMPLEXITY |
//  +------------------------------------------------------------------------+
//  | registerTimer            |      O(log[T])      |      O(log[T])        |
//  +------------------------------------------------------------------------+
//  | deregisterTimer          |        O(T)         |        O(T)           |
//  +------------------------------------------------------------------------+
//  | registerSocketEvent      |        O(1)         |        O(S)           |
//  +------------------------------------------------------------------------+
//  | deregisterSocketEvent    |        O(1)         |        O(S)           |
//  +------------------------------------------------------------------------+
//  | isRegistered             |        O(1)         |        O(S)           |
//  +------------------------------------------------------------------------+
//  | deregisterSocket         |        O(1)         |        O(S)           |
//  +------------------------------------------------------------------------+
//  | deregisterAllSocketEvents|        O(S)         |        O(S)           |
//  +------------------------------------------------------------------------+
//  | deregisterAllTimers      |        O(T)         |        O(T)           |
//  +------------------------------------------------------------------------+
//  | deregisterAll            |       O(S+T)        |       O(S+T)          |
//  +------------------------------------------------------------------------+
//  | numEvents                |        O(1)         |        O(1)           |
//  +------------------------------------------------------------------------+
//  | numTimers                |        O(1)         |        O(1)           |
//  +------------------------------------------------------------------------+
//  | numSocketEvents          |        O(1)         |        O(S)           |
//  +========================================================================+
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// In the following usage example, we demonstrate how to use timer
// functionality provided by 'btlmt::TcpTimerEventManager'.  For simplicity, we
// simulate enqueueing a job to a queue periodically.  Let's assume that a job
// is represented by an integer.  First, let's implement a function that
// enqueues a job to a given queue.  This function will also re-register the
// "next" timer with the event manager since timers have "one-time" semantics.
//..
//  static void producer(bdlcc::Queue<int>           *workQueue,
//                       btlmt::TcpTimerEventManager *manager,
//                       bsls::TimeInterval           nextTime)
//      // Enqueue a work item onto the specified 'workQueue' and register
//      // this function with the specified 'manager' to be invoked after
//      // the specified 'nextTime' absolute time interval.
//  {
//      assert(workQueue);
//      assert(manager);
//
//      enum { TIME_OFFSET = 50 };   // invoke timer every 50 milliseconds
//
//      int item = bdlt::CurrentTime::now().nanoseconds() / 1000;
//      workQueue->pushBack(item);
//
//      bsls::TimeInterval nextNextTime(nextTime);
//      nextNextTime.addMilliseconds(TIME_OFFSET);
//      bsl::function<void()> callback(bdlf::BindUtil::bind(&producer,
//                                                          workQueue,
//                                                          manager,
//                                                          nextNextTime));
//
//      void *timerId = manager->registerTimer(nextTime, callback);
//      assert(timerId);
//  }
//..
// Second, implement the main function.  In main, create a work queue and
// a timer event manager, register with the event manager an initial timer
// with its callback and begin monitoring the work queue:
//..
//  int main() {
//      enum {
//          TIME_OFFSET         = 50, // milliseconds
//          INITIAL_TIME_OFFSET = 10  // milliseconds
//      };
//
//      bdlcc::Queue<int> workQueue(&testAllocator);;
//      btlmt::TcpTimerEventManager manager(&testAllocator);;
//
//      bsls::TimeInterval now = bdlt::CurrentTime::now();
//      now.addMilliseconds(INITIAL_TIME_OFFSET);
//      bsls::TimeInterval nextTime(now);
//      nextTime.addMilliseconds(TIME_OFFSET);
//
//      bsl::function<void()> callback(bdlf::BindUtil::bind(&producer,
//                                                          &workQueue,
//                                                          &manager,
//                                                          nextNextTime));
//
//      void *timerId = manager.registerTimer(now, callback);
//      assert(timerId);
//      int rc = manager.enable();
//      assert(0 == rc);
//      while(1) {
//          // Monitor 'workQueue' here
//          // ...
//      }
//      return 0;
//  }
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#include <btlso_sockethandle.h>
#endif

#ifndef INCLUDED_BTLSO_EVENTTYPE
#include <btlso_eventtype.h>
#endif

#ifndef INCLUDED_BTLSO_IPV4ADDRESS
#include <btlso_ipv4address.h>
#endif

#ifndef INCLUDED_BTLSO_IOUTIL
#include <btlso_ioutil.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETIMPUTIL
#include <btlso_socketimputil.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETOPTUTIL
#include <btlso_socketoptutil.h>
#endif

#ifndef INCLUDED_BTLSO_EVENTMANAGER
#include <btlso_eventmanager.h>
#endif

#ifndef INCLUDED_BTLSO_TIMEREVENTMANAGER
#include <btlso_timereventmanager.h>
#endif

#ifndef INCLUDED_BTLSO_TIMEMETRICS
#include <btlso_timemetrics.h>
#endif

#ifndef INCLUDED_BDLCC_QUEUE
#include <bdlcc_queue.h>
#endif

#ifndef INCLUDED_BDLCC_TIMEQUEUE
#include <bdlcc_timequeue.h>
#endif

#ifndef INCLUDED_BDLMA_CONCURRENTPOOL
#include <bdlma_concurrentpool.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLMT_THREADATTRIBUTES
#include <bslmt_threadattributes.h>
#endif

#ifndef INCLUDED_BSLMT_THREADUTIL
#include <bslmt_threadutil.h>
#endif

#ifndef INCLUDED_BSLMT_RWMUTEX
#include <bslmt_rwmutex.h>
#endif

#ifndef INCLUDED_BSLMT_LOCKGUARD
#include <bslmt_lockguard.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BDLT_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDPTR
#include <bslma_managedptr.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace btlmt {

class TcpTimerEventManager_Request;

              // =========================================
              // class TcpTimerEventManager_ControlChannel
              // =========================================

class TcpTimerEventManager_ControlChannel {
    // This class is an implementation detail of 'TcpTimerEventManager'.
    // Do not use.

    // DATA
#if defined(BTLSO_PLATFORM_WIN_SOCKETS) && defined(BSLS_PLATFORM_CPU_64_BIT)
    bsls::AtomicInt64 d_fds[2];              // connected socket pair
#else
    bsls::AtomicInt   d_fds[2];              // connected socket pair
#endif

    const char        d_byte;                // signal byte

    int               d_numServerReads;      // read operations
    int               d_numServerBytesRead;  // total number of bytes read
    bsls::AtomicInt   d_numPendingRequests;  // number of pending requests

    // NOT IMPLEMENTED
    TcpTimerEventManager_ControlChannel(
                                   const TcpTimerEventManager_ControlChannel&);
    TcpTimerEventManager_ControlChannel& operator=(
                                   const TcpTimerEventManager_ControlChannel&);

  public:
    // CREATORS
    TcpTimerEventManager_ControlChannel();
        // Create an instance of this component by instantiating a connected
        // pair of sockets.

    ~TcpTimerEventManager_ControlChannel();
        // Close the internal sockets and destroy this object.

    // MANIPULATORS
    int clientWrite(bool forceWrite = false);
        // Write the control byte into the client handle.  If the specified
        // 'forceWrite' is 'true' write to the client handle even if a previous
        // write is outstanding.  By default, 'forceWrite' is 'false' and the
        // control byte is not written to the client handle if a previous write
        // is outstanding.  Return the number of bytes written on success, and
        // a negative value otherwise.  Note that 'forceWrite' can be used when
        // a write might be lost because the control channel had closed and
        // ensures that the control byte is unconditionally written to the
        // client handle.

    int close();
        // Shutdown this control channel and close the connection between the
        // 'serverFd' and 'clientFd' managed by this object.  Return 0 on
        // success and a non-zero value otherwise.

    int open();
        // Open this control channel and establish a connection between the
        // 'serverFd' and 'clientFd' managed by this object.  Return 0 on
        // success and a non-zero value otherwise.

    int serverRead();
        // Read as many bytes as possible from the server buffer without
        // blocking.  Return the number of bytes read on success, and a
        // negative value otherwise.

    // ACCESSORS
    btlso::SocketHandle::Handle clientFd();
        // Return a handle to the client socket.

    btlso::SocketHandle::Handle serverFd();
        // Return a handle to the server socket.
};

                       // ==========================
                       // class TcpTimerEventManager
                       // ==========================

class TcpTimerEventManager : public btlso::TimerEventManager {
    // This class provides a thread-enabled implementation of an event manager
    // for socket events and timers.  The supported event types for sockets are
    // ACCEPT, CONNECT, READ, and WRITE; only READ and WRITE can be registered
    // simultaneously for a single socket.  Timers are stable (i.e., the
    // relative order of registrations is maintained).  Callbacks are invoked
    // from dedicated threads, created internally for this purpose by this
    // component.

    // PRIVATE TYPES
    enum State {
        e_ENABLED  = 0,  // dispatching thread is running
        e_DISABLED = 1   // dispatching thread is not running
    };

    typedef TcpTimerEventManager_ControlChannel ControlChannel;
        // Channel for sending messages from external threads operating on this
        // component and its internal dispatch thread.

    // DATA
    mutable bdlma::ConcurrentPool  d_requestPool;     // memory pool for
                                                      // operations

    mutable bdlcc::Queue<TcpTimerEventManager_Request*>
                                   d_requestQueue;    // queue of requests to
                                                      // dispatcher thread

    mutable bslmt::ThreadUtil::Handle
                                   d_dispatcher;      // dispatcher thread
                                                      // handle

    volatile State                 d_state;           // the state of the
                                                      // dispatcher thread

    bsls::AtomicInt                d_terminateThread; // signals end of
                                                      // dispatcher

    mutable bslmt::RWMutex         d_stateLock;       // protects access to the
                                                      // state changes via
                                                      // 'enable' and 'disable'

    bsl::function<void()>          d_dispatchThreadEntryPoint;
                                                      // functor containing the
                                                      // dispatch thread's
                                                      // entry point

    btlso::EventManager           *d_manager_p;       // socket event manager

    int                            d_isManagedFlag;   // whether or not event
                                                      // manager is internal or
                                                      // external

    bsl::vector<bsl::function<void()> >
                                  *d_executeQueue_p;  // queue of executed
                                                      // timers (pointer, to
                                                      // be swappable in
                                                      // dispatcher thread
                                                      // loop)

    mutable bslmt::Mutex           d_executeQueueLock;
                                                      // protects access to the
                                                      // execute queue

    bdlcc::TimeQueue<bsl::function<void()> >
                                   d_timerQueue;      // queue of registered
                                                      // timers

    mutable bslma::ManagedPtr<ControlChannel>
                                   d_controlChannel_p;// channel for sending
                                                      // control bytes from
                                                      // external threads
                                                      // operating on this
                                                      // manager to unlock its
                                                      // internal 'dispatch'
                                                      // thread after they
                                                      // post a message for
                                                      // the 'dispatch' thread
                                                      // to process

    mutable btlso::TimeMetrics     d_metrics;         // cached metrics

    const bool                     d_collectMetrics;  // whether to update
                                                      // 'd_metrics'

    bsls::AtomicInt                d_numTotalSocketEvents;
                                                      // the total number of
                                                      // all socket events
                                                      // registered (excluding
                                                      // registered events of
                                                      // 'd_controlChannel_p')

    bsls::AtomicInt                d_numControlChannelReinitializations;
                                                      // number of times the
                                                      // control channel was
                                                      // reinitialized

    bslma::Allocator              *d_allocator_p;     // memory allocator
                                                      // (held, not owned)

    // FRIENDS
    friend struct TcpTimerEventManager_TestUtil;

    // NOT IMPLEMENTED
    TcpTimerEventManager(const TcpTimerEventManager&);
    TcpTimerEventManager& operator=(const TcpTimerEventManager&);

    // PRIVATE MANIPULATORS
    void initialize();
        // Initialize this event manager.

    void dispatchThreadEntryPoint();
        // Entry point for the dispatch thread.

    void controlCb();
        // Internal callback method to process control information received on
        // 'd_controlChannel_p.serverFd()'.

    int initiateControlChannelRead();
        // Initiate a request to the internal control channel's 'serverFd' to
        // make it operational and to start waiting for subsequent writes.
        // Return 0 on success and a non-zero value otherwise.

    int reinitializeControlChannel();
        // Reinitialize this event manager's internal control channel.  Return
        // 0 on success and a non-zero value otherwise.

  public:
    // CREATORS
    explicit TcpTimerEventManager(bslma::Allocator *basicAllocator = 0);
    explicit TcpTimerEventManager(bool              collectTimeMetrics,
                                  bslma::Allocator *basicAllocator = 0);
    TcpTimerEventManager(bool              collectTimeMetrics,
                         bool              poolTimerMemory,
                         bslma::Allocator *basicAllocator = 0);
        // Create an event manager.  Optionally specify 'collectTimeMetrics'
        // indicating whether this event manager should collect timing metrics.
        // If 'collectTimeMetrics' is unspecified or 'true' then the event
        // manager will provide a categorization of the time it spends
        // processing data via 'timeMetrics()', and if 'collectTimeMetrics' is
        // 'false' the value of 'timeMetrics()' is unspecified.  Optionally
        // specify 'poolTimerMemory' indicating whether the memory used for
        // internal timers should be pooled.  If 'poolTimerMemory' is
        // unspecified then the memory used for allocating timers will not be
        // pooled.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // 'basicAllocator' refers to a *thread* *safe* allocator.  Note that
        // the dispatcher thread is NOT started by this method (i.e., it must
        // be started explicitly).

    TcpTimerEventManager(btlso::EventManager *rawEventManager,
                         bslma::Allocator    *basicAllocator = 0);
        // Create an event manager with timer support that uses the specified
        // 'rawEventManager' to monitor for socket events.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  'basicAllocator'
        // must refer to a *thread* *safe* allocator, and the behavior is
        // undefined, otherwise.  The value of the event manager's
        // 'timeMetrics()' object will be unspecified, clients interested in
        // those metrics must use the 'btlso::TimeMetrics' object provided to
        // 'rawEventManager' on its construction.  Note that the dispatcher
        // thread is NOT started by this method (and, therefore, must be
        // started explicitly).

    virtual ~TcpTimerEventManager();
        // Terminate the dispatcher thread, if it is running, and destroy this
        // event manager without invoking any registered callback.

    // MANIPULATORS
    virtual void deregisterAllSocketEvents();
        // Deregister all callbacks associated with any event on any socket
        // handle so that no callbacks are not invoked when any event occurs on
        // any handle.

    virtual void deregisterAllTimers();
        // Remove all timer callbacks currently registered.  The number of
        // timers (as reported by 'numTimers') method is 0 after this method is
        // completed.

    virtual void deregisterAll();
        // Deregister all callbacks currently registered.  Note that the
        // behavior is equivalent to the sequence:
        //..
        //  deregisterAllTimers();
        //  deregisterAllSocketEvents();
        //..

    virtual void deregisterSocketEvent(
                                     const btlso::SocketHandle::Handle& handle,
                                     btlso::EventType::Type             event);
        // Deregister the callback associated with the specified 'event' on the
        // specified 'handle' so that said callback will not be invoked if the
        // 'event' occurs.

    virtual void deregisterSocket(const btlso::SocketHandle::Handle& handle);
        // Deregister all callbacks associated with any event on the specified
        // 'handle' such that no callback will be invoked if an event occurs on
        // the 'handle'.

    virtual void deregisterTimer(const void *timerId);
        // Deregister the callback associated with the specified 'timerId'
        // (returned when the timer callback was registered) so that the
        // callback will not be invoked at the appointed time.

    int disable();
        // Destroy the internal thread responsible for monitoring sockets and
        // dispatching timer and socket callbacks.  Return 0 on success and a
        // non-zero value otherwise.  If this event manager is not already
        // enabled, as reported by 'isEnabled' method, this method returns
        // immediately with 0.  Otherwise, it will block until the internal
        // thread exits.  Note that 'disable' will always fail if invoked,
        // through a callback, in the internal thread.

    int enable();
    int enable(const bslmt::ThreadAttributes& attribute);
        // Create the internal thread responsible for monitoring sockets and
        // dispatching timer and socket callbacks, optionally setting the
        // 'attribute' of the new thread to the specified 'attribute'.  Return
        // 0 on success and a non-zero value otherwise.  If this event manager
        // is already enabled, as reported by 'isEnabled' method, this method
        // returns immediately with 0.  Otherwise, it will block until the
        // internal thread is started or failure occurs.

    void execute(const bsl::function<void()>& functor);
        // Execute the specified 'functor' in the internal thread, if it is
        // started.

    void clearExecuteQueue();
        // Clear the functors enqueued to the execute queue.

    virtual int registerSocketEvent(
                                   const btlso::SocketHandle::Handle&   handle,
                                   btlso::EventType::Type               event,
                                   const btlso::EventManager::Callback& cb);
        // Register the specified 'cb' functor to be invoked whenever the
        // specified 'event' occurs on the socket specified by 'handle'.
        // Return 0 on success and a negative number on error.  No two
        // different socket events can have callbacks registered with the same
        // socket handle other than read and write.  Any invocation of this
        // method that would cause this to occur will result in an error and
        // the callback will not be registered.  The callback is recurring
        // (i.e., it remains registered until it is explicitly deregistered).
        // Note that the callback will be invoked only from the internal thread
        // and that the callback may be invoked before this method returns.

    virtual void *registerTimer(const bsls::TimeInterval&            timeout,
                                const btlso::EventManager::Callback& cb);
        // Register the specified 'cb' functor to be invoked when the absolute
        // time of the specified 'timeout' is reached or exceeded.  Return a
        // 'void*' registration id which can be used to deregister this timer
        // before expiration.  Specifying a 'timeout' previous to the current
        // time will result in the associated 'cb' being executed almost
        // immediately.  Note also that the callback is not recurring (i.e.,
        // after being invoked it is deregistered automatically).  Note that
        // the callback will be invoked only from the internal thread and that
        // the callback may be invoked before this method returns.

    int rescheduleTimer(const void                *timerId,
                        const bsls::TimeInterval&  timeout);
        // Reschedule the timer indicated by the specified 'timerId' such that
        // the callback function supplied to 'registerTimer' will be invoked
        // after the specified 'expiryTime' is reached.  Return 0 on success,
        // and a negative value otherwise.  'expiryTime' is expressed as the
        // absolute time from 00:00:00 UTC, January 1, 1970 (the epoch time
        // defined by 'bdetu_epoch').  The behavior is undefined unless
        // 'timerId' is a timer id returned from 'registerTimer', and has not
        // subsequently been deregistered.  Note that if 'expiryTime' is
        // earlier than the current time ('bdlt::CurrentTime::now()') the
        // associated callback will be invoked the first time that the
        // callbacks are dispatched.

    // ACCESSORS
    virtual bool hasLimitedSocketCapacity() const;
        // Return 'true' if this event manager has a limited socket capacity,
        // and 'false' otherwise.

    virtual int isRegistered(const btlso::SocketHandle::Handle& handle,
                             btlso::EventType::Type             event) const;
        // Return 1 if a callback is registered to be invoked when a socket
        // event of the specified 'event' occurs on the specified socket
        // 'handle', and 0 otherwise.

    virtual int numEvents() const;
        // Return the total number of callbacks registered with this event
        // manager for either timers or sockets.

    virtual int numTimers() const;
        // Return the number of timers that are currently registered.

    virtual int numSocketEvents(
                              const btlso::SocketHandle::Handle& handle) const;
        // Return the number of callbacks registered for the specified socket
        // 'handle'.

    int numTotalSocketEvents() const;
        // Return the number of socket callbacks registered with this event
        // manager.

    btlso::TimeMetrics *timeMetrics() const;
        // Return the address of the modifiable time metrics object that
        // provides a breakdown of the time this event manager has spent
        // processing data.  Note that the metrics managed by the returned
        // object have unspecified values unless 'hasTimeMetrics' returns
        // 'true'.

    bslmt::ThreadUtil::Handle dispatcherThreadHandle() const;
        // Return the thread handle of the dispatcher thread of this object.

    int isEnabled() const;
        // Return 1 if the dispatch thread is created/running and 0 otherwise.

    bool hasTimeMetrics() const;
        // Return 'true' if the object returned by 'timeMetrics()' contains a
        // valid value, and 'false' otherwise.  This value will be 'false' if
        // either the (optional) 'collectTimeMetrics' value supplied at
        // construction was 'false', or if a 'rawEventManager' was provided at
        // construction.  Note that if the value is 'false', the value of the
        // object returned by 'timeMetrics()' is not specified.
};

                       // ===================================
                       // class TcpTimerEventManager_TestUtil
                       // ===================================

struct TcpTimerEventManager_TestUtil {
    // This 'struct' provides access to the internal control channel of a
    // 'TcpTimerEventManager' object and is used for testing only.  This
    // 'struct' should *NOT* be used directly to gain access to the control
    // channel.

    static const TcpTimerEventManager_ControlChannel *getControlChannel(
                                          const TcpTimerEventManager& manager);
        // Return a pointer providing non-modifiable access to the control
        // channel object held by the specified 'manager' or 0 if the control
        // channel has not been initialized.
};

//-----------------------------------------------------------------------------
//                      INLINE FUNCTIONS DEFINITIONS
//-----------------------------------------------------------------------------

               // -----------------------------------------
               // class TcpTimerEventManager_ControlChannel
               // -----------------------------------------

// MANIPULATORS
inline
btlso::SocketHandle::Handle TcpTimerEventManager_ControlChannel::clientFd()
{
    return static_cast<btlso::SocketHandle::Handle>(d_fds[0].loadRelaxed());
}

inline
btlso::SocketHandle::Handle TcpTimerEventManager_ControlChannel::serverFd()
{
    return static_cast<btlso::SocketHandle::Handle>(d_fds[1].loadRelaxed());
}

                        // --------------------------
                        // class TcpTimerEventManager
                        // --------------------------

// MANIPULATORS
inline
int TcpTimerEventManager::enable()
{
    return enable(bslmt::ThreadAttributes());
}

// ACCESSORS
inline
bool TcpTimerEventManager::hasLimitedSocketCapacity() const
{
    return d_manager_p->hasLimitedSocketCapacity();
}

inline
btlso::TimeMetrics *TcpTimerEventManager::timeMetrics() const
{
    return &d_metrics;
}

inline
bslmt::ThreadUtil::Handle TcpTimerEventManager::dispatcherThreadHandle() const
{
    return d_dispatcher;
}

inline
bool TcpTimerEventManager::hasTimeMetrics() const
{
    return d_collectMetrics;
}

                        // -----------------------------------
                        // class TcpTimerEventManager_TestUtil
                        // -----------------------------------

inline
const TcpTimerEventManager_ControlChannel *
TcpTimerEventManager_TestUtil::getControlChannel(
                                           const TcpTimerEventManager& manager)
{
    return manager.d_controlChannel_p.ptr();
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
