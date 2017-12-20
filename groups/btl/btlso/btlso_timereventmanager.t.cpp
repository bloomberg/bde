// btlso_timereventmanager.t.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlso_timereventmanager.h>

#include <btlso_event.h>

#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>

#include <bsl_cstdlib.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_unordered_map.h>
#include <bsl_utility.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// We are testing a pure protocol class.  We need to verify that a
// concrete derived class compiles and links and that the example compiles
// and works as advertised.
//-----------------------------------------------------------------------------
// [ 1] ~btlso::TimerEventManager()
// [ 1] int registerSocketEvent(handle, event, callback);
// [ 1] void *registerTimer(expiryTime, callback);
// [ 1] int rescheduleTimer(timerId, expiryTime);
// [ 1] void deregisterSocketEvent(handle, event);
// [ 1] void deregisterSocket(const btlso::SocketHandle::Handle& handle);
// [ 1] void deregisterAllSocketEvents();
// [ 1] void deregisterTimer(const void *handle);
// [ 1] void deregisterAllTimers();
// [ 1] void deregisterAll();
// [ 1] bool hasLimitedSocketCapacity() const;
// [ 1] int  isRegistered(const Handle& handle, const Type event);
// [ 1] void numTimers();
// [ 1] void numEvents();
// [ 1] void numSocketEvents(const btlso::SocketHandle::Handle& handle);
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
// [ 2] USAGE TEST - Make sure usage example compiles and works as advertised.
//=============================================================================

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing a timed socket multiplexer
///- - - - - - - - - - - - - - - - - - - - - - - - -
class my_TimerInfo {
    // This is a helper class that stores information about the timer
    // associated with a socket/event pair (an instance of 'btlso::Event').

    friend bool operator==(const my_TimerInfo& lhs, const my_TimerInfo& rhs);
        // This class supports only in-core value semantics.

  private:
    bsls::TimeInterval  d_expiryTime; // current expiry time of the timer
    bsls::TimeInterval  d_period;     // period of the (recurrent) timer
    void               *d_id;         // unique timer identifier

  public:
    // CREATORS
    my_TimerInfo();
        // Create a 'my_TimerInfo' having default values for 'expiryTime',
        // 'period', and 'id'.

    my_TimerInfo(const bsls::TimeInterval&  expiryTime,
                 const bsls::TimeInterval&  period,
                 void                      *id);
        // Create a 'my_TimerInfo' containing the specified 'expiryTime',
        // 'period', and 'id'.

    ~my_TimerInfo();
        // Destroy this object.

    // MANIPULATORS
    void setExpiryTime(const bsls::TimeInterval& expiryTime);
        // Set 'expiryTime'.

    void setPeriod(const bsls::TimeInterval& period);
        // Set 'period'.

    void setId(void *id);
        // Set 'id'.

    // ACCESSORS
    const bsls::TimeInterval& expiryTime() const;
        // Return a reference to the timer's expiry time.

    const bsls::TimeInterval& period() const;
        // Return a reference to the timer's period.

    const void *id() const;
        // Return the timer's ID.
};

bool operator==(const my_TimerInfo& lhs, const my_TimerInfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' timer information objects
    // have the same value, and 'false' otherwise.  Two timer information
    // objects have the same value if they have the same respective values of
    // expiry time, period, and ID.

my_TimerInfo::my_TimerInfo()
{
}

my_TimerInfo::my_TimerInfo(const bsls::TimeInterval&  expiryTime,
                           const bsls::TimeInterval&  period,
                           void                      *id)
: d_expiryTime(expiryTime)
, d_period(period)
, d_id(id)
{
}

my_TimerInfo::~my_TimerInfo()
{
}

void my_TimerInfo::setExpiryTime(const bsls::TimeInterval& expiryTime)
{
    d_expiryTime = expiryTime;
}

void my_TimerInfo::setPeriod(const bsls::TimeInterval& period)
{
    d_period = period;
}

void my_TimerInfo::setId(void *id)
{
    d_id = id;
}

const bsls::TimeInterval& my_TimerInfo::expiryTime() const
{
    return d_expiryTime;
}

const bsls::TimeInterval& my_TimerInfo::period() const
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
    // 'bsl::function<void(my_TimedSocketMultiplexer::CallbackCode)>'
    // functor to be registered via the 'registerTimedSocketEvent' method.
    // This functor is invoked with an argument of
    // 'my_TimedSocketMultiplexer::SOCKET_EVENT' if the socket event occurs
    // before the timeout interval or with an argument of
    // 'my_TimedSocketMultiplexer::TIMEOUT' when the timeout occurs before an
    // occurrence of the specified socket event.  Each time the callback is
    // invoked, the timer is rescheduled to expire after the specified time
    // period.

  public:

    // TYPES
    enum CallbackCode {
        // Enumerations used to indicate the reason the user callback
        // functor is being invoked.

        e_SOCKET_EVENT = 0,  // The specified socket event has occurred.
        e_TIMEOUT      = 1   // The timer has expired before the specified
                             // socket event occurred.
    };

  private:
    typedef bsl::unordered_map<btlso::Event, my_TimerInfo, btlso::EventHash>
                                             EventTimeMap;

    EventTimeMap                             d_eventTimeMap;

    btlso::TimerEventManager                *d_manager_p;

  private:

    // Private methods 'eventCb' and 'timerCb' are internal callback member
    // functions registered with 'btlso::TimerEventManager'.

    void eventCb(const btlso::Event&                       socketEvent,
                 const bsl::function<void(CallbackCode)>&  userCb,
                 const btlso::TimerEventManager::Callback& internalCb);
        // Callback registered with 'btlso::TimerEventManager', which is
        // invoked to indicate the occurrence of the specified socket event
        // 'socketEvent'.  This method cancels the current timer and registers
        // a new timer to expire after the specified period from the current
        // time along with the internal timer callback functor, 'internalCb',
        // to be invoked when the timer expires.  This method then invokes the
        // user specified callback 'userCb' with the argument
        // 'my_TimedSocketMultiplexer::e_SOCKET_EVENT'.

    void timerCb(const btlso::Event&                       socketEvent,
                 const bsl::function<void(CallbackCode)>&  userCb,
                 const btlso::TimerEventManager::Callback& internalCb);
        // Callback registered with 'btlso::TimerEventManager', which is
        // invoked to indicate the expiry of the timer associated with the
        // specified socket event 'socketEvent'.  This method registers a new
        // timer to expire after the specified time period measured from the
        // previous timer's expiry time along with the internal timer callback
        // functor 'internalCb' to be invoked when the new timer expires.  This
        // method then invokes the specified callback 'userCb' with the
        // argument user 'my_TimedSocketMultiplexer::e_TIMEOUT'.

  public:
    // CREATORS
    my_TimedSocketMultiplexer(btlso::TimerEventManager *manager_p);
        // Create an empty multiplexer object.

    // MANIPULATORS
    int registerTimedSocketEvent(
                              const btlso::SocketHandle::Handle&       handle,
                              btlso::EventType::Type                   event,
                              const bsls::TimeInterval&                period,
                              const bsl::function<void(CallbackCode)>& userCb);
        // Register the specified 'userCb' functor to be invoked whenever the
        // specified 'event' occurs on the specified 'handle' or when 'event'
        // has not occurred within the specified 'period' of time.  Return 0 on
        // successful registration, and a nonzero value otherwise.

    int deregisterTimedSocketEvent(const btlso::SocketHandle::Handle& handle,
                                   btlso::EventType::Type             event);
        // Deregister the callback associated with the specified 'handle' and
        // 'event'.  Return 0 on successful deregistration and a nonzero value
        // otherwise.
};

void my_TimedSocketMultiplexer::eventCb(
                         const btlso::Event&                       socketEvent,
                         const bsl::function<void(CallbackCode)>&  userCb,
                         const btlso::TimerEventManager::Callback& internalCb)
{
    // Retrieve the timer information associated with 'socketEvent'.

    EventTimeMap::iterator socketEventIt = d_eventTimeMap.find(socketEvent);
    ASSERT(d_eventTimeMap.end() != socketEventIt);
    my_TimerInfo *timerInfo = &socketEventIt->second;

    // Deregister the current timer callback.

    d_manager_p->deregisterTimer(timerInfo->id());

    // Set the new timeout value.

    timerInfo->setExpiryTime(bdlt::CurrentTime::now() + timerInfo->period());

    // Register a new timer callback to fire at this time.

    timerInfo->setId(d_manager_p->registerTimer(timerInfo->expiryTime(),
                                                internalCb));

    // Invoke userCb with an argument of 'e_SOCKET_EVENT' to indicate that
    // 'socketEvent' has occurred.

    userCb(e_SOCKET_EVENT);
}

void my_TimedSocketMultiplexer::timerCb(
                         const btlso::Event&                       socketEvent,
                         const bsl::function<void(CallbackCode)>&  userCb,
                         const btlso::TimerEventManager::Callback& internalCb)
{
    // Retrieve the timer information associated with 'socketEvent' and set
    // the new expiry time.

    EventTimeMap::iterator socketEventIt = d_eventTimeMap.find(socketEvent);
    ASSERT(d_eventTimeMap.end() != socketEventIt);
    my_TimerInfo *timerInfo = &socketEventIt->second;
    timerInfo->setExpiryTime(timerInfo->expiryTime() + timerInfo->period());

    // Register a new timer callback to fire at that time.

    timerInfo->setId(d_manager_p->registerTimer(timerInfo->expiryTime(),
                                               internalCb));

    // Invoke user callback functor with an argument of 'e_TIMEOUT' to
    // indicate that a timeout has occurred before 'socketEvent'.

    userCb(e_TIMEOUT);
}

my_TimedSocketMultiplexer::my_TimedSocketMultiplexer(
                                           btlso::TimerEventManager *manager_p)
: d_eventTimeMap()
, d_manager_p(manager_p)
{
}

int my_TimedSocketMultiplexer::registerTimedSocketEvent(
                               const btlso::SocketHandle::Handle&       handle,
                               btlso::EventType::Type                   event,
                               const bsls::TimeInterval&                period,
                               const bsl::function<void(CallbackCode)>& userCb)
{
    btlso::Event socketEvent(handle, event);
    bsls::TimeInterval expiryTime = bdlt::CurrentTime::now() + period;

    // Create a timer callback.

    btlso::TimerEventManager::Callback myTimerCb;
    myTimerCb = bdlf::BindUtil::bind(
             bdlf::MemFnUtil::memFn(&my_TimedSocketMultiplexer::timerCb, this),
             socketEvent,
             userCb,
             myTimerCb);

    // Create an event callback.

    btlso::TimerEventManager::Callback myEventCb(bdlf::BindUtil::bind(
             bdlf::MemFnUtil::memFn(&my_TimedSocketMultiplexer::eventCb, this),
             socketEvent,
             userCb,
             myTimerCb));

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
                                     const btlso::SocketHandle::Handle& handle,
                                     btlso::EventType::Type             event)
{

    // Retrieve timer information for this event.

    btlso::Event socketEvent(handle, event);
    EventTimeMap::iterator socketEventIt = d_eventTimeMap.find(socketEvent);
    my_TimerInfo *timerInfo = &socketEventIt->second;
    if (d_eventTimeMap.end() != socketEventIt) {
        return -1;                                                    // RETURN
    }

    // Deregister this socket event.

    d_manager_p->deregisterSocketEvent(handle, event);

    // Deregister timer

    d_manager_p->deregisterTimer(timerInfo->id());

    // Remove timer information for this event from the map.

    d_eventTimeMap.erase(socketEventIt);

    return 0;
}

//=============================================================================
//                      CONCRETE DERIVED TYPE
//-----------------------------------------------------------------------------

class my_TimerEventManager : public btlso::TimerEventManager {
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

    int registerSocketEvent(const btlso::SocketHandle::Handle&,
                            btlso::EventType::Type            ,
                            const Callback&                   )
        { *d_fun = 3; return -1; }

    void *registerTimer(const bsls::TimeInterval&, const Callback&)
        { *d_fun = 4; return 0; }

    void deregisterSocketEvent(const btlso::SocketHandle::Handle& ,
                               btlso::EventType::Type             )
        { *d_fun = 5; }

    void deregisterSocket(const btlso::SocketHandle::Handle&)
        { *d_fun = 6; }

    void deregisterAllSocketEvents()
        { *d_fun = 7; }

    void deregisterTimer(const void *)
        { *d_fun = 8; }

    void deregisterAllTimers()
        { *d_fun = 9; }

    void deregisterAll()
        { *d_fun = 10; }

    int numTimers() const
        { *d_fun = 11; return 0; }

    int numSocketEvents(const btlso::SocketHandle::Handle&) const
        { *d_fun = 12; return 0; }

    int numEvents() const
        { *d_fun = 13; return 0; }

    int isRegistered(const btlso::SocketHandle::Handle&,
                     btlso::EventType::Type            ) const
        { *d_fun = 14; return 0; }

    int rescheduleTimer(const void                *,
                        const bsls::TimeInterval&  )
        { *d_fun = 15; return 0; }

    bool hasLimitedSocketCapacity() const
        { *d_fun = 16; return true; }
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

      } break;
      case 1: {
        // -----------------------------------------------------------------
        // PROTOCOL TEST:
        //   We need to make sure that a subclass of the
        //   'btlso::TimerEventManager' class compiles and links when
        //   all virtual functions are defined.
        //
        // Testing:
        //   ~btlso::TimerEventManager()
        //   int registerSocketEvent(handle, event, callback);
        //   void *registerTimer(expiryTime, callback);
        //   int rescheduleTimer(timerId, expiryTime);
        //   int deregisterSocketEvent(handle, event);
        //   int deregisterSocket(const btlso::SocketHandle::Handle& handle);
        //   void deregisterAllSocketEvents();
        //   int deregisterTimer(const void *handle);
        //   int deregisterAllTimers();
        //   void deregisterAll();
        //   bool hasLimitedSocketCapacity() const;
        //   bool isRegistered() const;
        //   int numTimers();
        //   int numEvents();
        //   int numSocketEvents(const btlso::SocketHandle::Handle& handle);
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;
        int                                 i(0);
        btlso::TimerEventManager           *m = new my_TimerEventManager(&i);
        btlso::TimerEventManager&           t = *m;
        btlso::SocketHandle::Handle         h;
        btlso::EventType::Type              e = btlso::EventType::Type(1);
        btlso::TimerEventManager::Callback  cb;
        bsls::TimeInterval                  ti;
        void                               *tmr;

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
