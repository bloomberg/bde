// bteso_timereventmanager.t.cpp      -*-C++-*-

#include <bteso_timereventmanager.h>

#include <bteso_event.h>

#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>
#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_memfn.h>

#include <bsl_cstdlib.h>
#include <bsl_hash_map.h>
#include <bsl_iostream.h>
#include <bsl_utility.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//==========================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------

//==========================================================================
//                              TEST PLAN
//--------------------------------------------------------------------------
//                              OVERVIEW
// We are testing a pure protocol class.  We need to verify that a
// concrete derived class compiles and links and that the example compiles
// and works as advertised.
//--------------------------------------------------------------------------
// [ 1] ~bteso_TimerEventManager()
// [ 1] int registerSocketEvent(
//                const bteso_SocketHandle::Handle& handle,
//                bteso_EventType::Type             event,
//                const Callback&                   callback);
// [ 1] void *registerTimer(
//                const bdet_TimeInterval& expiryTime,
//                const Callback&          callback);
// [ 1] void deregisterSocketEvent(
//                const bteso_SocketHandle::Handle& handle,
//                bteso_EventType::Type             event);
// [ 1] void deregisterSocket(const bteso_SocketHandle::Handle& handle);
// [ 1] void deregisterAllSocketEvents();
// [ 1] void deregisterTimer(const void *handle);
// [ 1] void deregisterAllTimers();
// [ 1] void deregisterAll();
// [ 1] bool hasLimitedSocketCapacity() const;
// [ 1] int  isRegistered(const Handle& handle, const Type event);
// [ 1] void numTimers();
// [ 1] void numSocketEvents(const bteso_SocketHandle::Handle& handle);
//--------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
// [ 2] USAGE TEST - Make sure usage example compiles and works as advertised.
//==========================================================================

//==========================================================================
//                      USAGE EXAMPLE
//--------------------------------------------------------------------------

class my_TimerInfo {
    // This is a helper class that stores information about the timer
    // associated with a socket/event pair (an instance of 'bteso_Event').

    friend bool operator==(const my_TimerInfo& lhs, const my_TimerInfo& rhs);
        // This class supports only in-core value semantics.

  private:
    bdet_TimeInterval  d_expiryTime; // current expiry time of the timer
    bdet_TimeInterval  d_period;     // period of the (recurrent) timer
    void              *d_id;         // unique timer identifier

  public:
    // CREATORS
    my_TimerInfo();
        // Create a 'my_TimerInfo' having default values for 'expiryTime',
        // 'period', and 'id'.

    my_TimerInfo(const bdet_TimeInterval&  expiryTime,
                 const bdet_TimeInterval&  period,
                 void                     *id);
        // Create a 'my_TimerInfo' containing the specified 'expiryTime',
        // 'period', and 'id'.

    ~my_TimerInfo();
        // Destroy this object.

    // MANIPULATORS
    void setExpiryTime(const bdet_TimeInterval& expiryTime);
        // Set 'expiryTime'.

    void setPeriod(const bdet_TimeInterval& period);
        // Set 'period'.

    void setId(void *id);
        // Set 'id'.

    // ACCESSORS
    const bdet_TimeInterval& expiryTime() const;
        // Return a reference to the timer's expiry time.

    const bdet_TimeInterval& period() const;
        // Return a reference to the timer's period.

    const void *id() const;
        // Return the timer's ID.
};

bool operator==(const my_TimerInfo& lhs, const my_TimerInfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' timer information
    // objects have the same value, and 'false' otherwise.  Two timer
    // information objects have the same value if they have the same
    // respective values of expiry time, period, and ID.

my_TimerInfo::my_TimerInfo()
{
}

my_TimerInfo::my_TimerInfo(
        const bdet_TimeInterval&  expiryTime,
        const bdet_TimeInterval&  period,
        void                     *id)
: d_expiryTime(expiryTime)
, d_period(period)
, d_id(id)
{
}

my_TimerInfo::~my_TimerInfo()
{
}

void my_TimerInfo::setExpiryTime(const bdet_TimeInterval& expiryTime)
{
    d_expiryTime = expiryTime;
}

void my_TimerInfo::setPeriod(const bdet_TimeInterval& period)
{
    d_period = period;
}

void my_TimerInfo::setId(void *id)
{
    d_id = id;
}

const bdet_TimeInterval& my_TimerInfo::expiryTime() const
{
    return d_expiryTime;
}

const bdet_TimeInterval& my_TimerInfo::period() const
{
    return d_period;
}

const void *my_TimerInfo::id() const
{
    return d_id;
}

bool operator==(const my_TimerInfo& lhs, const my_TimerInfo& rhs)
{
    return lhs.expiryTime() == rhs.expiryTime()
           && lhs.period() == rhs.period()
           && lhs.id() == rhs.id();
}

class my_TimedSocketMultiplexer {
    // This class implements a subset of a socket event multiplexer that
    // supports the registration of timed socket events and associated
    // callbacks.  Specifically, this class allows a user specified
    // 'bdef_Function<void (*)(my_TimedSocketMultiplexer::CallbackCode)>'
    // functor to be registered via the 'registerTimedSocketEvent' method.
    // This functor is invoked with an argument of
    // 'my_TimedSocketMultiplexer::SOCKET_EVENT' if the socket event
    // occurs before the timeout interval or with an argument of
    // 'my_TimedSocketMultiplexer::TIMEOUT' when the timeout occurs
    // before an occurrence of the specified socket event.  Each time the
    // callback is invoked, the timer is rescheduled to expire
    // after the specified time period.

  public:

    // TYPES
    enum CallbackCode {
        // Enumerations used to indicate the reason the user callback
        // functor is being invoked.
        SOCKET_EVENT = 0,  // The specified socket event has occurred.
        TIMEOUT      = 1   // The timer has expired before the specified
                           // socket event occurred.
    };

  private:
    typedef bsl::hash_map<bteso_Event, my_TimerInfo, bteso_EventHash>
                                            EventTimeMap;

    EventTimeMap                            d_eventTimeMap;

    bteso_TimerEventManager                *d_manager_p;

  private:

    // Private methods 'eventCb' and 'timerCb' are internal callback member
    // functions registered with 'bteso_TimerEventManager'.

    void eventCb(
            const bteso_Event&                             socketEvent,
            const bdef_Function<void (*)(CallbackCode)>&   userCb,
            const bteso_TimerEventManager::Callback&       internalCb);
        // Callback registered with 'bteso_TimerEventManager', which is
        // invoked to indicate the occurrence of the specified socket event
        // 'socketEvent'.  This method cancels the current timer and
        // registers a new timer to expire after the specified period from
        // the current time along with the internal timer callback functor,
        // 'internalCb', to be invoked when the timer expires.  This method
        // then invokes the user specified callback 'userCb' with the
        // argument 'my_TimedSocketMultiplexer::SOCKET_EVENT'.

    void timerCb(
            const bteso_Event&                             socketEvent,
            const bdef_Function<void (*)(CallbackCode)>&   userCb,
            const bteso_TimerEventManager::Callback&       internalCb);
        // Callback registered with 'bteso_TimerEventManager', which is
        // invoked to indicate the expiry of the timer associated with the
        // specified socket event 'socketEvent'.  This method registers
        // a new timer to expire after the specified time period
        // measured from the previous timer's expiry time along with
        // the internal timer callback functor 'internalCb' to be
        // invoked when the new timer expires.  This method then invokes the
        // specified callback 'userCb' with the argument
        // user 'my_TimedSocketMultiplexer::TIMEOUT'.

  public:
    // CREATORS
    my_TimedSocketMultiplexer(bteso_TimerEventManager *manager_p);
        // Create an empty multiplexer object.

    // MANIPULATORS
    int registerTimedSocketEvent(
            const bteso_SocketHandle::Handle&               handle,
            bteso_EventType::Type                           event,
            const bdet_TimeInterval&                        period,
            const bdef_Function<void (*)(CallbackCode)>&    userCb);
        // Register the specified 'userCb' functor to be invoked whenever
        // the specified 'event' occurs on the specified 'handle' or when
        // 'event' has not occurred within the specified 'period' of time.
        // Return 0 on successful registration, and a nonzero value
        // otherwise.

    int deregisterTimedSocketEvent(
            const bteso_SocketHandle::Handle& handle,
            bteso_EventType::Type             event);
        // Deregister the callback associated with the specified 'handle'
        // and 'event'.  Return 0 on successful deregistration and a
        // nonzero value otherwise.
};

void my_TimedSocketMultiplexer::eventCb(
        const bteso_Event&                             socketEvent,
        const bdef_Function<void (*)(CallbackCode)>&   userCb,
        const bteso_TimerEventManager::Callback&       internalCb)
{
    // Retrieve the timer information associated with 'socketEvent'.
    EventTimeMap::iterator socketEventIt = d_eventTimeMap.find(socketEvent);
    ASSERT(d_eventTimeMap.end() != socketEventIt);
    my_TimerInfo *timerInfo = &socketEventIt->second;

    // Deregister the current timer callback.
    d_manager_p->deregisterTimer(timerInfo->id());

    // Set the new timeout value.
    timerInfo->setExpiryTime(bdetu_SystemTime::now() + timerInfo->period());

    // Register a new timer callback to fire at this time.
    timerInfo->setId(
            d_manager_p->registerTimer(timerInfo->expiryTime(), internalCb));

    // Invoke userCb with an argument of 'SOCKET_EVENT' to indicate that
    // 'socketEvent' has occurred.
    userCb(SOCKET_EVENT);
}

void my_TimedSocketMultiplexer::timerCb(
        const bteso_Event&                             socketEvent,
        const bdef_Function<void (*)(CallbackCode)>&   userCb,
        const bteso_TimerEventManager::Callback&       internalCb)
{
    // Retrieve the timer information associated with 'socketEvent' and set
    // the new expiry time.
    EventTimeMap::iterator socketEventIt = d_eventTimeMap.find(socketEvent);
    ASSERT(d_eventTimeMap.end() != socketEventIt);
    my_TimerInfo *timerInfo = &socketEventIt->second;
    timerInfo->setExpiryTime(timerInfo->expiryTime() + timerInfo->period());

    // Register a new timer callback to fire at that time.
   timerInfo->setId(
            d_manager_p->registerTimer(timerInfo->expiryTime(), internalCb));

    // Invoke user callback functor with an argument of 'TIMEOUT' to
    // indicate that a timeout has occurred before 'socketEvent'.
    userCb(TIMEOUT);
}

my_TimedSocketMultiplexer::my_TimedSocketMultiplexer(
        bteso_TimerEventManager *manager_p)
: d_eventTimeMap()
, d_manager_p(manager_p)
{
}

int my_TimedSocketMultiplexer::registerTimedSocketEvent(
        const bteso_SocketHandle::Handle&               handle,
        bteso_EventType::Type                           event,
        const bdet_TimeInterval&                        period,
        const bdef_Function<void (*)(CallbackCode)>&    userCb)
{
    bteso_Event socketEvent(handle, event);
    bdet_TimeInterval expiryTime = bdetu_SystemTime::now() + period;

    // Create a timer callback.
    bteso_TimerEventManager::Callback myTimerCb;
    myTimerCb =
           bdef_BindUtil::bind(
               bdef_MemFnUtil::memFn(&my_TimedSocketMultiplexer::timerCb, this)
             , socketEvent
             , userCb
             , myTimerCb);

    // Create an event callback.
    bteso_TimerEventManager::Callback myEventCb(
           bdef_BindUtil::bind(
               bdef_MemFnUtil::memFn(&my_TimedSocketMultiplexer::eventCb, this)
             , socketEvent
             , userCb
             , myTimerCb));

    // Register the event callback.
    d_manager_p->registerSocketEvent(handle, event, myEventCb);

    // Register the timer callback.
    void *timerHandle = d_manager_p->registerTimer(expiryTime, myTimerCb);

    // Save the timer information associated with this event in the map.
    my_TimerInfo timerInfo(expiryTime, period, timerHandle);
    d_eventTimeMap.insert(bsl::make_pair(socketEvent, timerInfo));

    return 0;
}

int my_TimedSocketMultiplexer::deregisterTimedSocketEvent(
        const bteso_SocketHandle::Handle& handle,
        bteso_EventType::Type             event)
{

    // Retrieve timer information for this event.
    bteso_Event socketEvent(handle, event);
    EventTimeMap::iterator socketEventIt = d_eventTimeMap.find(socketEvent);
    my_TimerInfo *timerInfo = &socketEventIt->second;
    if (d_eventTimeMap.end() != socketEventIt) {
        return -1;
    }

    // Deregister this socket event.
    d_manager_p->deregisterSocketEvent(handle, event);

    // Deregister timer
    d_manager_p->deregisterTimer(timerInfo->id());

    // Remove timer information for this event from the map.
    d_eventTimeMap.erase(socketEventIt);

    return 0;
}

//==========================================================================
//                      CONCRETE DERIVED TYPE
//--------------------------------------------------------------------------

class my_TimerEventManager : public bteso_TimerEventManager {
  // Test class used to verify protocol.

    int *d_fun; // Code describing last called function:
                //  1: ~my_TimerEventManager
                //  3: registerSocketEvent
                //  4: registerTimer
                //  5: deregisterSocketEvent
                //  6: deregisterSocket
                //  7: deregisterAllSocketEvents
                //  8: deregisterTimer
                //  9: deregisterAllTimers
                // 10: deregisterAll
                // 11: numTimers
                // 12: numSocketEvents
                // 13: numEvents
                // 14: isRegistered
                // 15: rescheduleTimer
                // 16: hasLimitedSocketCapacity

  public:
    my_TimerEventManager(int *fun) : d_fun(fun) { }
    ~my_TimerEventManager()
        { *d_fun = 1; }

    int registerSocketEvent(
                const bteso_SocketHandle::Handle& handle,
                bteso_EventType::Type       event,
                const Callback&                   callback)
        { *d_fun = 3; return -1; }

    void *registerTimer(
                const bdet_TimeInterval& expiryTime,
                const Callback&          callback)
        { *d_fun = 4; return 0; }

    void deregisterSocketEvent(
                const bteso_SocketHandle::Handle& handle,
                bteso_EventType::Type       event)
        { *d_fun = 5; }

    void deregisterSocket(const bteso_SocketHandle::Handle& handle)
        { *d_fun = 6; }

    void deregisterAllSocketEvents()
        { *d_fun = 7; }

    void deregisterTimer(const void *handle)
        { *d_fun = 8; }

    void deregisterAllTimers()
        { *d_fun = 9; }

    void deregisterAll()
        { *d_fun = 10; }

    int numTimers() const
        { *d_fun = 11; return 0; }

    int numSocketEvents(const bteso_SocketHandle::Handle& handle) const
        { *d_fun = 12; return 0; }

    int numEvents() const
        { *d_fun = 13; return 0; }

    int isRegistered(
                const bteso_SocketHandle::Handle& handle,
                bteso_EventType::Type             event) const
        { *d_fun = 14; return 0; }

    int rescheduleTimer(const void               *timerId,
                        const bdet_TimeInterval&  expiryTime)
        { *d_fun = 15; return 0; }

    bool hasLimitedSocketCapacity() const
        { *d_fun = 16; return true; }
};

//==========================================================================
//                      MAIN PROGRAM
//--------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // -----------------------------------------------------------------
        // USAGE TEST:
        //   This test is really just to make sure the syntax is correct.
        // Testing:
        //   USAGE TEST - Make sure usage example compiles and works as
        //   advertised.
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        // TODO
      } break;
      case 1: {
        // -----------------------------------------------------------------
        // PROTOCOL TEST:
        //   We need to make sure that a subclass of the
        //   'bteso_TimerEventManager' class compiles and links when
        //   all virtual functions are defined.
        // Testing:
        //   ~bteso_TimerEventManager()
        //   int registerSocketEvent(
        //                const bteso_SocketHandle::Handle& handle,
        //                bteso_EventType::Type             event,
        //                const Callback&                   callback);
        //   void *registerTimer(
        //                const bdet_TimeInterval& expiryTime,
        //                const Callback&          callback);
        //   int rescheduleTimer(const void               *timerId,
        //                       const bdet_TimeInterval&  expiryTime);
        //   int deregisterSocketEvent(
        //                const bteso_SocketHandle::Handle& handle,
        //                bteso_EventType::Type             event);
        //   int deregisterSocket(const bteso_SocketHandle::Handle& handle);
        //   void deregisterAllSocketEvents();
        //   int deregisterTimer(const void *handle);
        //   int deregisterAllTimers();
        //   void deregisterAll();
        //   bool hasLimitedSocketCapacity() const;
        //   bool isRegistered() const;
        //   int numTimers();
        //   int numSocketEvents(const bteso_SocketHandle::Handle& handle);
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;
        int                                i(0);
        bteso_TimerEventManager           *m = new my_TimerEventManager(&i);
        bteso_TimerEventManager&           t = *m;
        bteso_SocketHandle::Handle         h;
        bteso_EventType::Type              e = bteso_EventType::Type(1);
        bteso_TimerEventManager::Callback  cb;
        bdet_TimeInterval                  ti;
        void                              *tmr;

        if (verbose) cout << "\nTesting register functions." << endl;

        t.registerSocketEvent(h, e, cb);
        ASSERT(3 == i);

        tmr = t.registerTimer(ti, cb);
        ASSERT(4 == i);

        if (verbose) cout << "\nTesting deregister functions." << endl;

        t.deregisterSocketEvent(h, e);
        ASSERT(5 == i);

        t.deregisterSocket(h);
        ASSERT(6 == i);

        t.deregisterAllSocketEvents();
        ASSERT(7 == i);

        t.deregisterTimer(tmr);
        ASSERT(8 == i);

        t.deregisterAllTimers();
        ASSERT(9 == i);

        t.deregisterAll();
        ASSERT(10 == i);

        if (verbose) cout << "\nTesting accessor functions." << endl;

        t.numTimers();
        ASSERT(11 == i);

        t.numSocketEvents(h);
        ASSERT(12 == i);

        t.numEvents();
        ASSERT(13 == i);

        t.isRegistered(h, e);
        ASSERT(14 == i);

        t.rescheduleTimer(tmr, ti);
        ASSERT(15 == i);

        t.hasLimitedSocketCapacity();
        ASSERT(16 == i);

        delete m;
        ASSERT(1 == i);

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
