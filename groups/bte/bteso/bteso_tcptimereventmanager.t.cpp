// bteso_tcptimereventmanager.t.cpp                                   -*-C++-*-
#include <bteso_tcptimereventmanager.h>
#include <bteso_socketimputil.h>
#include <bteso_flag.h>
#include <bcemt_thread.h>
#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bsls_platform.h>
#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>
#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdef_memfn.h>

#include <bsl_c_stdlib.h>     // atoi()
#include <bsl_iostream.h>
using namespace BloombergLP;
using namespace bsl;  // automatically added by script

#ifdef BSLS_PLATFORM__OS_UNIX
#include <bsl_c_signal.h>
#endif

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//    [TBD - Overview of the test]
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] bteso_TcpTimerEventManager(bslma_Allocator *basicAllocator = 0);
// [ 2] bteso_TcpTimerEventManager(Hint infreqRegHint, bslma_Allocator *ba =0);
// [ 2] bteso_TcpTimerEventManager(bteso_EventManager*, bslma_Allocator *ba=0);
// [ 2] ~bteso_TcpTimerEventManager();

// MANIPULATORS
// [ 6] int dispatch(int flags);
// [ 3] int registerSocketEvent(const Handle&, Type, const Callback&);
// [ 4] void *registerTimer(const bdet_TimeInterval&, const Callback&);
// [ 4] int rescheduleTimer(const void *handle, const bdet_TimeInterval&);
// [ 3] void deregisterSocketEvent(const Handle& handle, Type event);
// [ 3] void deregisterSocket(const bteso_SocketHandle::Handle& handle);
// [ 3] void deregisterAllSocketEvents();
// [ 4] void deregisterTimer(const void *handle);
// [ 4] void deregisterAllTimers();
// [ 3] void deregisterAll();
// [ 2] bteso_TimeMetrics *timeMetrics();

// ACCESSORS
// [ 3] int isRegistered(const Handle& handle, Type eventType) const;
// [ 3] int numEvents() const;
// [ 4] int numTimers() const;
// [ 3] int numSocketEvents(const Handle& handle) const;
// [ 2] const bteso_EventManager *socketEventManager() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 9] USAGE EXAMPLE
//=============================================================================

//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bteso_TcpTimerEventManager Obj;

class my_CommandMediator {
    // This class provides a mechanism to invoke a user-installed command
    // from the user-installed socket event manager's 'dispatch' method.
    // Internally, it creates a socket pair, and monitors one of the pair's
    // socket handles for incoming data.  The 'invoke' method will write a
    // byte into the other handle.  On the next call to 'dispatch' method,
    // read event will be signaled, and an internal callback will be invoked.
    // This callback, in turn, will read a byte, and invoke the user's command.

    bteso_SocketHandle::Handle  d_client;   // socket handle for "control data"
    bteso_SocketHandle::Handle  d_server;   // socket handle to monitor
    bteso_TcpTimerEventManager *d_manager_p;// targeted event manager
    const char                  d_byte;     // control byte
    bdef_Function<void (*)()>   d_command;  // user command.
  private:
    void readCb();
        // Read exactly one byte from 'd_server' socket, and, if read
        // successfully and correctly, invoke the callback currently
        // installed.

  public:
    // CREATORS
    my_CommandMediator(bteso_TcpTimerEventManager *manager,
                       bdef_Function<void (*)()> command,
                       bslma_Allocator *basicAllocator = 0);
        // Create a mediator attached to the specified 'manager' and
        // the specified 'command', which will be invoked from 'manager''s
        // 'dispatch' method.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, global operators 'new'
        // and 'delete' are used.  The behavior is undefined if 'manager' is 0.

    ~my_CommandMediator();
        // Destroy this object.

    // MANIPULATORS
    int operator()();
        // Initiate an invocation of the associated command on the
        // the next invocation of 'dispatch' method of installed timer
        // event manager.   Return 0 on success and a non-zero value
        // otherwise.
};

// Private member functions

void my_CommandMediator::readCb() {
    char data;
    ASSERT(1 == bteso_SocketImpUtil::read(&data, d_server, sizeof data));
    ASSERT(data == d_byte);
    d_command();
}

// CREATORS
inline
my_CommandMediator::my_CommandMediator(bteso_TcpTimerEventManager *manager,
                                       bdef_Function<void (*)()>   command,
                                       bslma_Allocator     *basicAllocator)
: d_manager_p(manager)
, d_byte(0xAF)
, d_command(command)
{
    bteso_SocketHandle::Handle handles[2];
    ASSERT(0 == bteso_SocketImpUtil::socketPair<bteso_IPv4Address>
           (handles, bteso_SocketImpUtil::BTESO_SOCKET_STREAM));

    d_client = handles[0];
    d_server = handles[1];

    bdef_Function<void (*)()> functor(
     bdef_MemFnUtil::memFn(&my_CommandMediator::readCb, this), basicAllocator);
    d_manager_p->registerSocketEvent(d_server, bteso_EventType::BTESO_READ,
                                     functor);
}

inline
my_CommandMediator::~my_CommandMediator() {
    d_manager_p->deregisterSocketEvent(d_server, bteso_EventType::BTESO_READ);
    ASSERT(0 == d_manager_p->numSocketEvents(d_server));
    bteso_SocketImpUtil::close(d_client);
    bteso_SocketImpUtil::close(d_server);
}

// MANIPULATORS
inline
int my_CommandMediator::operator()() {
    return
        bteso_SocketImpUtil::write(d_client, &d_byte, sizeof(char))
        != sizeof(char);

}

// ---------------------------------------------------------------------------

static void set(char *result)
    // Load into the specified 'result' value 1.  The behavior is undefined if
    // 'result' is 0.
{
    ASSERT(result);
    *result = 1;
}

// ===========================================================================

class TestSocketPair {
    bteso_SocketHandle::Handle  d_client;
    bteso_SocketHandle::Handle  d_server;
    bteso_TcpTimerEventManager *d_manager_p;
    void                       *d_timerId;
    char                        d_readCbInvoked;
    char                        d_timerCbInvoked;

  private:
    void readCb();
    void timerCb();

  public:
    // CREATORS
    TestSocketPair(bteso_TcpTimerEventManager *manager);
    ~TestSocketPair();

    // MANIPULATORS
    void reset();

    // ACCESSORS
    bteso_SocketHandle::Handle client() const;

    char readCbInvokedFlag() const;
    bteso_SocketHandle::Handle server() const;

    char timerCbInvokedFlag() const;
};

inline
TestSocketPair::TestSocketPair(bteso_TcpTimerEventManager *manager)
: d_manager_p(manager)
, d_timerId(0)
, d_readCbInvoked(0)
, d_timerCbInvoked(0)
{
    bteso_SocketHandle::Handle handles[2];
    ASSERT(0 == bteso_SocketImpUtil::socketPair<bteso_IPv4Address>
           (handles, bteso_SocketImpUtil::BTESO_SOCKET_STREAM));

    d_client = handles[0];
    d_server = handles[1];

    bdef_Function<void (*)()> functor(
            bdef_MemFnUtil::memFn(&TestSocketPair::timerCb, this));
    d_timerId = d_manager_p->registerTimer(bdetu_SystemTime::now() + 0.01,
                                           functor);

    functor = bdef_MemFnUtil::memFn(&TestSocketPair::readCb, this);
    d_manager_p->registerSocketEvent(d_server, bteso_EventType::BTESO_READ,
                                     functor);
}

inline
TestSocketPair::~TestSocketPair() {
    d_manager_p->deregisterSocket(d_server);
    bteso_SocketImpUtil::close(d_client);
    bteso_SocketImpUtil::close(d_server);

}

void TestSocketPair::readCb() {
    d_manager_p->deregisterTimer(d_timerId);
    d_timerId = NULL;
    d_readCbInvoked = 1;
}

void TestSocketPair::timerCb() {
    d_timerCbInvoked = 1;
}

// ACCESSORS
inline
bteso_SocketHandle::Handle TestSocketPair::client() const {
    return d_client;
}

inline
char TestSocketPair::readCbInvokedFlag() const {
    return d_readCbInvoked;
}

inline
bteso_SocketHandle::Handle TestSocketPair::server() const {
    return d_server;
}

inline
char TestSocketPair::timerCbInvokedFlag() const {
    return d_timerCbInvoked;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

static
void timerCallback(bdet_TimeInterval *regTime, int *isInvoked)
    // Verify that the current time is at least the time value stored
    // at the specified 'regTime' and store 1 into the specified
    // 'isInvoked' flag (verifying that it is not 1 before).
{
    ASSERT(regTime); ASSERT(isInvoked); ASSERT(*isInvoked != 1);
    *isInvoked = 1;
    ASSERT(bdetu_SystemTime::now() >= *regTime);
}

bcemt_ThreadUtil::Handle mainThread;

#ifdef BSLS_PLATFORM__OS_UNIX
extern "C" void *interruptThread(void* arg) {
    const int SLEEP_INTERVAL = 500000; // in microseconds
    bcemt_ThreadUtil::microSleep(SLEEP_INTERVAL, 0);
    pthread_kill((pthread_t)mainThread, SIGUSR1);
    return arg;
}

extern "C" void signalHandler(int sigNumber) {
    ASSERT(sigNumber == SIGUSR1);
    signal(SIGUSR1, signalHandler);
}
#endif

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma_TestAllocator testAllocator(veryVeryVerbose);
    ASSERT(0 == bteso_SocketImpUtil::startup());

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
            // ----------------------------------------------------------------
            // TESTING USAGE EXAMPLE
            //   The usage example provided in the component header file must
            //   compile, link, and run on all platforms as shown.
            //
            // Plan:
            //   Incorporate usage example from header into driver, remove
            //   leading comment characters, and replace 'assert' with
            //   'ASSERT'.
            //
            // Testing:
            //   USAGE EXAMPLE
            // ----------------------------------------------------------------
            ASSERT(0 == bteso_SocketImpUtil::startup());
            bslma_TestAllocator testAllocator(veryVeryVerbose);

            if (verbose) cout << "\nTesting Usage Example"
                              << "\n=====================" << endl;

            {
                bteso_TcpTimerEventManager mX(&testAllocator);
                const bteso_TcpTimerEventManager& X = mX;

                char isInvokedFlag = 0;
                bdef_Function<void (*)()> command(
                        bdef_BindUtil::bind(&set, &isInvokedFlag));

                my_CommandMediator mediator(&mX, command, &testAllocator);
                ASSERT(1 == X.numEvents());
                ASSERT(0 == X.numTimers());
                ASSERT(0 == mediator());

                ASSERT(1 == mX.dispatch(0));
                ASSERT(1 == isInvokedFlag);
            }
            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == bteso_SocketImpUtil::cleanup());
      } break;
      case 8: {
            // ----------------------------------------------------------------
            // TESTING USAGE EXAMPLE
            //   The usage example provided in the component header file must
            //   compile, link, and run on all platforms as shown.
            //
            // Plan:
            //   Incorporate usage example from header into driver, remove
            //   leading comment characters, and replace 'assert' with
            //   'ASSERT'.
            //
            // Testing:
            //   USAGE EXAMPLE
            // ----------------------------------------------------------------
            ASSERT(0 == bteso_SocketImpUtil::startup());
            bslma_TestAllocator testAllocator(veryVeryVerbose);

            if (verbose) cout << "\nTesting Usage Example"
                              << "\n=====================" << endl;

            {
                bteso_TcpTimerEventManager mX(&testAllocator);
                const bteso_TcpTimerEventManager& X = mX;

                TestSocketPair sp(&mX);
                ASSERT(2 == X.numEvents());
                ASSERT(1 == X.numTimers());
                const char controlByte = 0xAB;
                ASSERT(1 == bteso_SocketImpUtil::write(sp.client(),
                                                       &controlByte,
                                                       sizeof(char)));
                ASSERT(1 == mX.dispatch(0));
                ASSERT(0 == sp.timerCbInvokedFlag());
                ASSERT(1 == sp.readCbInvokedFlag());

            }
            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == bteso_SocketImpUtil::cleanup());
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'dispatch' WITH INTERRUPTS
        // Concerns:
        //   o Dispatch behaves as expected when interrupted
        // Plan:
        //   Create a table of timer offsets and interrupt flags.  On each
        //   iteration, create an auxiliary thread that will interrupt
        //   main thread (after a hard-coded timeout).  Verify that a timer
        //   callback is invoked if expected and dispatch's return value
        //   is correct.  This test is executed only in UNIX.
        // Testing:
        //   dispatch
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING 'dispatch' with asynchornous interrupts." << endl
            << "================================================" << endl;

#ifdef BSLS_PLATFORM__OS_UNIX
        mainThread = bcemt_ThreadUtil::self();
        struct {
            double d_offset;
            int d_flag;
            int d_expDispatchReturn;
            int d_expIsInvoked;
        } DATA[] = {
            { 1.5, 0,                                  1, 1 },
            { 1.5, bteso_Flag::BTESO_ASYNC_INTERRUPT, -1, 0 }
        };
        signal(SIGUSR1, signalHandler);

        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        for (int i = 0; i < NUM_DATA; ++i) {
            bslma_TestAllocator testAllocator;
            Obj mX(&testAllocator);

            int isInvoked = 0;
            bdet_TimeInterval now = bdetu_SystemTime::now();
            bdet_TimeInterval offset = now +
                bdet_TimeInterval(DATA[i].d_offset);
            bdef_Function<void (*)()> functor(
                    bdef_BindUtil::bind(&timerCallback, &offset, &isInvoked));
            mX.registerTimer(offset, functor);

            bcemt_ThreadUtil::Handle h;
            bcemt_ThreadUtil::create(&h, interruptThread, NULL);

            int rc = mX.dispatch(DATA[i].d_flag);
            if (veryVerbose) {
                Q("STATE AFTER DISPATCH");
                P_(DATA[i].d_flag);
                P_(DATA[i].d_expDispatchReturn);
                P(rc);
                P_(DATA[i].d_expIsInvoked);
                P(isInvoked);
            }
            bcemt_ThreadUtil::join(h);
        }
#endif

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'dispatch' and 'deregisterTimer'
        // Concerns:
        //   o Deregistered timers wont be invoked
        //   o Accessors report state as expected
        // Plan:
        //   Create a table of timer offsets in seconds from some fixed point.
        //   Register timers with appropriate time values and verify that
        //   the callbacks are invoked as expected.
        // Testing:
        //   deregisterTimer
        //   numTimers
        //   numEvents
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING 'dispatch' AND 'deregisterTimer'" << endl
            << "========================================" << endl;

        enum { NUM_OFFSETS = 10 };
        struct {
            int d_offsets[NUM_OFFSETS];  // offsets for timers from now
                                         // (in seconds)
            int d_numOffsets;
            int d_deregisterBits[NUM_OFFSETS];
                                         // which timers to deregister
                                         // before dispatch
            int d_expIsInvoked[NUM_OFFSETS];
                                         // expected flags for each timer
            double d_dispatchOffset;     // minimum difference between
                                         // time after dispatch and before
        } DATA[] = {
            // One timer
            { { 0 },   1,  { 0 }, { 1 },  0  },
            { { 0 },   1,  { 1 }, { 0 },  0  },

            // Two timers

            { {  0,  0  },  2,  { 0, 0 }, { 1, 1 }, 0  },
            { {  0,  0  },  2,  { 0, 1 }, { 1, 0 }, 0  },
            { {  0,  0  },  2,  { 1, 0 }, { 0, 1 }, 0  },
            { {  0,  0  },  2,  { 1, 0 }, { 0, 1 }, 0  },

            { {  0,  1  },  2,  { 0, 0 }, { 1, 0 }, 0  },
            { {  0,  1  },  2,  { 0, 1 }, { 1, 0 }, 0  },
            { {  0,  1  },  2,  { 1, 0 }, { 0, 1 }, 1  },
            { {  0,  1  },  2,  { 1, 1 }, { 0, 0 }, 0  },

            { {  0, -1  },  2,  { 0, 0 }, { 1, 1 }, 0  },
            { {  0, -1  },  2,  { 0, 1 }, { 1, 0 }, 0  },
            { {  0, -1  },  2,  { 1, 0 }, { 0, 1 }, 0  },
            { {  0, -1  },  2,  { 1, 1 }, { 0, 0 }, 0  },

            { {  1, -1  },  2,  { 0, 0 }, { 0, 1 }, 0  },
            { {  1, -1  },  2,  { 0, 1 }, { 1, 0 }, 1  },
            { {  1, -1  },  2,  { 1, 0 }, { 0, 1 }, 0  },
            { {  1, -1  },  2,  { 1, 1 }, { 0, 0 }, 0  },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        const bdet_TimeInterval tolerance(0.25);

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        for (int i = 0; i < NUM_DATA; ++i) {
            void *timerIds[NUM_OFFSETS];
            bdet_TimeInterval timeValues[NUM_OFFSETS];
            int invocationFlags[NUM_OFFSETS];
            bdet_TimeInterval now = bdetu_SystemTime::now();

            Obj mX(&testAllocator);
            const Obj& X = mX;

            for (int j = 0; j < DATA[i].d_numOffsets; ++j) {
                invocationFlags[j] = 0;
                timeValues[j] = now +
                    bdet_TimeInterval(DATA[i].d_offsets[j]);
                bdef_Function<void (*)()> functor(
                       bdef_BindUtil::bind( &timerCallback
                                          , &timeValues[j]
                                          , &invocationFlags[j]));
                timerIds[j] = mX.registerTimer(timeValues[j], functor);
            }
            LOOP_ASSERT(i, DATA[i].d_numOffsets == X.numTimers());
            LOOP_ASSERT(i, DATA[i].d_numOffsets == X.numEvents());

            int numDeregistered = 0;
            for (int j = 0; j < DATA[i].d_numOffsets; ++j) {
                if (DATA[i].d_deregisterBits[j]) {
                    mX.deregisterTimer(timerIds[j]);
                    ++numDeregistered;
                }
            }

            int rc = mX.dispatch(0);
            bdet_TimeInterval now1 = bdetu_SystemTime::now();
            bdet_TimeInterval delta = now1 - now;
            bdet_TimeInterval expected =
                bdet_TimeInterval(DATA[i].d_dispatchOffset);

            LOOP_ASSERT(i, delta >= expected);
            LOOP_ASSERT(i, delta - expected < tolerance);
            if (veryVerbose) {
                P_(now); P(now1);
                P_(delta); P(expected);
            }

            int expNumInvoked = 0;
            for (int j = 0; j < DATA[i].d_numOffsets; ++j) {
                LOOP2_ASSERT(i, j, DATA[i].d_expIsInvoked[j] ==
                             invocationFlags[j]);
                expNumInvoked += DATA[i].d_expIsInvoked[j];
            }
            LOOP_ASSERT(i, expNumInvoked == rc);
            LOOP_ASSERT(i,
                        DATA[i].d_numOffsets - expNumInvoked - numDeregistered
                        == X.numTimers());

            LOOP_ASSERT(i,
                        DATA[i].d_numOffsets - expNumInvoked - numDeregistered
                        == X.numEvents());
            if (veryVerbose) {
                P(rc);
                P(expNumInvoked);
            }
            mX.deregisterAllTimers();
        }
      } break;
      case 5: {
// TBD FIX ME
#if !defined(BSLS_PLATFORM__OS_AIX) && !defined(BSLS_PLATFORM__OS_SOLARIS)
        // --------------------------------------------------------------------
        // TESTING 'dispatch' and 'registerTimer'
        // Concerns:
        //   o A registered timer can be invoked
        //   o Multiple timers registered for the same time will be dispatched
        //   o A timer registered before 'now' will be dispatched
        //   o Dispatch will block for the minimum required time
        //   o Deregistered timers wont be invoked
        //   o Accessors report state as expected
        // Plan:
        //   Create a table of timer offsets in seconds from some fixed point.
        //   Register timers with appropriate time values and verify that
        //   the callbacks are invoked as expected.
        // Testing:
        //   registerTimer
        //   deregisterTimer
        //   deregisterAllTimers
        //   numTimers
        //   numEvents
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING 'dispatch' AND 'registerTimer'" << endl
            << "======================================" << endl;

        enum { NUM_OFFSETS = 10 };
        struct {
            int d_offsets[NUM_OFFSETS];  // offsets for timers from now
                                         // (in seconds)
            int d_numOffsets;

            int d_expIsInvoked[NUM_OFFSETS];
                                         // expected flags for each timer
            double d_dispatchOffset;     // minimum difference between
                                         // time after dispatch and before
        } DATA[] = {
            // One timer
            { { 0 },  1,  { 1 },  0    },
            // Two timers
            { {  0,  0  },  2, { 1, 1 }, 0  },
            { {  0,  1  },  2, { 1, 0 }, 0  },
            { {  0, -1  },  2, { 1, 1 }, 0  },
            { {  1,  0  },  2, { 0, 1 }, 0  },
            { { -1,  0  },  2, { 1, 1 }, 0  },
            { { -1, -1  },  2, { 1, 1 }, 0  },
            { {  1,  1  },  2, { 1, 1 }, 1  },
            { {  2,  2  },  2, { 1, 1 }, 2  },
            { {  1,  2  },  2, { 1, 0 }, 1  },
            { {  2, -1  },  2, { 0, 1 }, 0  },

            // Three timers
            { {  0,  0,  0 },  3, { 1, 1, 1 }, 0 },
            { {  0,  1,  0 },  3, { 1, 0, 1 }, 0 },
            { {  0, -1,  0 },  3, { 1, 1, 1 }, 0 },
            { {  0,  0,  1 },  3, { 1, 1, 0 }, 0 },
            { {  0,  0, -1 },  3, { 1, 1, 1 }, 0 },

            { {  1,  0,  0 },  3, { 0, 1, 1 }, 0 },
            { {  1,  1,  0 },  3, { 0, 0, 1 }, 0 },
            { {  1, -1,  0 },  3, { 0, 1, 1 }, 0 },
            { {  1,  0,  1 },  3, { 0, 1, 0 }, 0 },
            { {  1,  0, -1 },  3, { 0, 1, 1 }, 0 },

            { { -1,  0,  0 },  3, { 1, 1, 1 }, 0 },
            { { -1,  1,  0 },  3, { 1, 0, 1 }, 0 },
            { { -1, -1,  0 },  3, { 1, 1, 1 }, 0 },
            { { -1,  0,  1 },  3, { 1, 1, 0 }, 0 },
            { { -1,  0, -1 },  3, { 1, 1, 1 }, 0 },

            { {  1,  1,  1 },  3, { 1, 1, 1 }, 1 },
            { {  1,  2,  1 },  3, { 1, 0, 1 }, 1 },
            { {  1,  1,  2 },  3, { 1, 1, 0 }, 1 },
            { {  1,  2,  2 },  3, { 1, 0, 0 }, 1 },

            { {  2,  1,  1 },  3, { 0, 1, 1 }, 1 },
            { {  2,  2,  1 },  3, { 0, 0, 1 }, 1 },
            { {  2,  1,  2 },  3, { 0, 1, 0 }, 1 },
            { {  2,  2,  2 },  3, { 1, 1, 1 }, 2 },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        #if   defined(BSLS_PLATFORM__OS_UNIX) \
           && !defined(BSLS_PLATFORM__OS_CYGWIN)
        const bdet_TimeInterval tolerance(0.02);
        #else
        const bdet_TimeInterval tolerance(0.1);
        #endif
        bslma_TestAllocator testAllocator(veryVeryVerbose);

        for (int i = 0; i < NUM_DATA; ++i) {
            bdet_TimeInterval timeValues[NUM_OFFSETS];
            int invocationFlags[NUM_OFFSETS];
            bdet_TimeInterval now = bdetu_SystemTime::now();

            Obj mX(&testAllocator);
            const Obj& X = mX;

            for (int j = 0; j < DATA[i].d_numOffsets; ++j) {
                invocationFlags[j] = 0;
                timeValues[j] = now +
                    bdet_TimeInterval(DATA[i].d_offsets[j]);
                bdef_Function<void (*)()> functor(
                        bdef_BindUtil::bind( &timerCallback
                                           , &timeValues[j]
                                           , &invocationFlags[j]));
                mX.registerTimer(timeValues[j], functor);
            }
            LOOP_ASSERT(i, DATA[i].d_numOffsets == X.numTimers());
            LOOP_ASSERT(i, DATA[i].d_numOffsets == X.numEvents());
            int rc = mX.dispatch(0);
            bdet_TimeInterval now1 = bdetu_SystemTime::now();
            bdet_TimeInterval delta = now1 - now;
            bdet_TimeInterval expected =
                bdet_TimeInterval(DATA[i].d_dispatchOffset);

            LOOP_ASSERT(i, delta >= expected);
            LOOP_ASSERT(i, delta - expected < tolerance);
            if (veryVerbose) {
                P_(now); P(now1);
                P_(delta); P(expected);
            }

            int expNumInvoked = 0;
            for (int j = 0; j < DATA[i].d_numOffsets; ++j) {
                LOOP2_ASSERT(i, j, DATA[i].d_expIsInvoked[j] ==
                             invocationFlags[j]);
                expNumInvoked += !!DATA[i].d_expIsInvoked[j];
            }
            LOOP_ASSERT(i, expNumInvoked == rc);
            LOOP_ASSERT(i,
                        DATA[i].d_numOffsets - expNumInvoked == X.numTimers());

            LOOP_ASSERT(i,
                        DATA[i].d_numOffsets - expNumInvoked == X.numEvents());
            if (veryVerbose) {
                P(rc);
                P(expNumInvoked);
            }
            mX.deregisterAllTimers();
        }
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING TIMER-RELATED FUNCTIONALITY: PART 1
        // Concerns:
        //   o A timer can be registered
        //   o A registered timer can be rescheduled
        //   o A registered timer can be deregistered
        //   o Multiple timers can be registered for the same time
        //   o Accessors report state as expected
        // Plan:
        //   Register and deregister timers for same and different time
        //   values.  Use accessors to examine state.
        // Testing:
        //   registerTimer
        //   rescheduleTimer
        //   deregisterTimer
        //   deregisterAllTimers
        //   numTimers
        //   numEvents
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING TIMER-RELATED FUNCTIONALITY." << endl
            << "====================================" << endl;

        const int OFFSETS[] = { 0, -1, +2, -2, +10, -100 };
        const int NUM_OFFSETS = sizeof OFFSETS / sizeof *OFFSETS;
        enum { MAX_NUM_SAME = 10 };

        if (verbose)
            cout << "\tTesting 'registerTimer' method." << endl;
        {
            bslma_TestAllocator testAllocator(veryVeryVerbose);

            void *timerIds[NUM_OFFSETS * MAX_NUM_SAME];
            bdet_TimeInterval now = bdetu_SystemTime::now();

            Obj mX(&testAllocator);
            const Obj& X = mX;
            for (int i = 0; i < MAX_NUM_SAME; ++i) {
                for (int j = 0; j < NUM_OFFSETS; ++j) {
                    timerIds[NUM_OFFSETS * i + j] =
                        mX.registerTimer(now +
                            bdet_TimeInterval(OFFSETS[j], 0),
                                         bdef_Function<void (*)()>());
                    LOOP2_ASSERT(i, j, timerIds[NUM_OFFSETS * i + j]);
                    LOOP2_ASSERT(i, j, NUM_OFFSETS * i + j + 1 ==
                                 X.numTimers());
                    LOOP2_ASSERT(i, j, NUM_OFFSETS * i + j + 1 ==
                                 X.numEvents());
                }
            }
        }

        if (verbose)
            cout << "\tTesting 'rescheduleTimer' method." << endl;
        {
            bslma_TestAllocator testAllocator(veryVeryVerbose);

            void *timerIds[NUM_OFFSETS  * (MAX_NUM_SAME + 1)];
            bdet_TimeInterval now = bdetu_SystemTime::now();

            enum { NUM_ATTEMPTS = 2 };
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                Obj mX(&testAllocator);
                const Obj& X = mX;
                for (int j = 0; j <= i; ++j) {
                    for (int k = 0; k < NUM_OFFSETS; ++k) {
                        int idx = NUM_OFFSETS * j + k;
                        timerIds[idx] =
                            mX.registerTimer(now +
                                bdet_TimeInterval(OFFSETS[k], 0),
                                             bdef_Function<void (*)()>());
                        LOOP3_ASSERT(i, j, k, timerIds[idx]);
                        LOOP3_ASSERT(i, j, k, idx + 1 == X.numTimers());
                        LOOP3_ASSERT(i, j, k, idx + 1 == X.numEvents());
                    }
                }

                const int numTimers = NUM_OFFSETS * (i + 1);
                now                 = bdetu_SystemTime::now();

                for (int j = 0; j <= i; ++j) {
                    for (int k = 0; k < NUM_OFFSETS; ++k) {
                        int idx = NUM_OFFSETS * j + k;
                        int rc = mX.rescheduleTimer(timerIds[idx],
                                       now + bdet_TimeInterval(OFFSETS[k], 0));
                        LOOP3_ASSERT(i, j, k, !rc);
                        LOOP3_ASSERT(i, j, k, numTimers == X.numTimers());
                        LOOP3_ASSERT(i, j, k, numTimers == X.numEvents());
                    }
                }
            }
        }

        if (verbose)
            cout << "\tTesting 'deregisterAll' method." << endl;
        {
            bslma_TestAllocator testAllocator(veryVeryVerbose);

            void *timerIds[NUM_OFFSETS  * (MAX_NUM_SAME + 1)];
            bdet_TimeInterval now = bdetu_SystemTime::now();

            enum { NUM_ATTEMPTS = 10 };
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                Obj mX(&testAllocator);
                const Obj& X = mX;
                for (int j = 0; j <= i; ++j) {
                    for (int k = 0; k < NUM_OFFSETS; ++k) {
                        int idx = NUM_OFFSETS * j + k;
                        timerIds[idx] =
                            mX.registerTimer(now +
                                bdet_TimeInterval(OFFSETS[k], 0),
                                             bdef_Function<void (*)()>());
                        LOOP3_ASSERT(i, j, k, timerIds[idx]);
                        LOOP3_ASSERT(i, j, k, idx + 1 == X.numTimers());
                        LOOP3_ASSERT(i, j, k, idx + 1 == X.numEvents());
                    }
                }

                mX.deregisterAll();
                LOOP_ASSERT(i, 0 == X.numTimers());
                LOOP_ASSERT(i, 0 == X.numEvents());
            }
        }

        if (verbose)
            cout << "\tTesting 'deregisterTimer' method." << endl;
        {
            bslma_TestAllocator testAllocator(veryVeryVerbose);

            void *timerIds[NUM_OFFSETS  * (MAX_NUM_SAME + 1)];
            bdet_TimeInterval now = bdetu_SystemTime::now();

            enum { NUM_ATTEMPTS = 2 };
            for (int i = 0; i < NUM_ATTEMPTS; ++i) {
                Obj mX(&testAllocator);
                const Obj& X = mX;
                for (int j = 0; j <= i; ++j) {
                    for (int k = 0; k < NUM_OFFSETS; ++k) {
                        int idx = NUM_OFFSETS * j + k;
                        timerIds[idx] =
                            mX.registerTimer(now +
                                bdet_TimeInterval(OFFSETS[k], 0),
                                             bdef_Function<void (*)()>());
                        LOOP3_ASSERT(i, j, k, timerIds[idx]);
                        LOOP3_ASSERT(i, j, k, idx + 1 == X.numTimers());
                        LOOP3_ASSERT(i, j, k, idx + 1 == X.numEvents());
                    }
                }

                int maxIndex = NUM_OFFSETS * (i + 1);
                while (--maxIndex >= 0) {
                    mX.deregisterTimer(timerIds[maxIndex]);
                    LOOP_ASSERT(i, maxIndex == X.numTimers());
                    LOOP_ASSERT(i, maxIndex == X.numEvents());
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING SOCKET-RELATED FUNCTIONALITY: PART 1
        // Concerns:
        //   The registration events are properly forwarded to an underlying
        //   event manager.
        // Plan:
        //   For each socket registration and deregistration operation,
        //   perform an operation through the timer event manager and
        //   verify the state of an underlying socket event manager
        // Testing:
        //   registerSocketEvent()
        //   deregisterSocketEvent()
        //   deregisterSocket()
        //   deregisterAllSocketEvents()
        //   deregisterAll()
        //   isRegistered()
        //   numSocketEvents()
        //   numEvents()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING SOCKET-RELATED FUNCTIONALITY." << endl
            << "=====================================" << endl;

        bteso_SocketHandle::Handle TEST_HANDLES[]
            = { 1, 2, 3, 4, 5 };
        const int NUM_HANDLES =
            sizeof TEST_HANDLES / sizeof *TEST_HANDLES ;

        bteso_EventType::Type TEST_EVENTS[] = {
            bteso_EventType::BTESO_READ,
            bteso_EventType::BTESO_WRITE,
            bteso_EventType::BTESO_ACCEPT,
            bteso_EventType::BTESO_CONNECT,
        };
        const int NUM_EVENTS =
            sizeof TEST_EVENTS / sizeof *TEST_EVENTS ;

        if (verbose)
            cout << "\tTesting 'registerSocketEvent'" << endl;
        {
            bslma_TestAllocator testAllocator(veryVeryVerbose);
            Obj mX(&testAllocator);
            const Obj& X = mX;
            const bteso_EventManager *em = X.socketEventManager();

            for (int i = 0; i < NUM_HANDLES; ++i) {
                Obj::Callback nullCb;
                LOOP_ASSERT(i,
                            0 == mX.registerSocketEvent(TEST_HANDLES[i],
                                TEST_EVENTS[i % NUM_EVENTS],  nullCb));
                LOOP_ASSERT(i, i + 1 == em->numEvents());
                LOOP_ASSERT(i, 1 == em->isRegistered(TEST_HANDLES[i],
                                TEST_EVENTS[i % NUM_EVENTS]));
                LOOP_ASSERT(i, 1 == em->numSocketEvents(TEST_HANDLES[i]));
            }
        }
        if (verbose)
            cout << "\tTesting 'deregisterSocketEvent'" << endl;
        {
            bslma_TestAllocator testAllocator(veryVeryVerbose);
            Obj mX(&testAllocator);
            const Obj& X = mX;
            const bteso_EventManager *em = X.socketEventManager();

            for (int i = 0; i < NUM_HANDLES; ++i) {
                Obj::Callback nullCb;
                LOOP_ASSERT(i,
                            0 == mX.registerSocketEvent(TEST_HANDLES[i],
                                TEST_EVENTS[i % NUM_EVENTS],  nullCb));
                LOOP_ASSERT(i, 1 == em->numEvents());
                LOOP_ASSERT(i, 1 == em->isRegistered(TEST_HANDLES[i],
                                TEST_EVENTS[i % NUM_EVENTS]));
                LOOP_ASSERT(i, 1 == em->numSocketEvents(TEST_HANDLES[i]));

                mX.deregisterSocketEvent(TEST_HANDLES[i],
                                         TEST_EVENTS[i % NUM_EVENTS]);

                LOOP_ASSERT(i, 0 == em->numEvents());
                LOOP_ASSERT(i, 0 == em->isRegistered(TEST_HANDLES[i],
                                TEST_EVENTS[i % NUM_EVENTS]));
                LOOP_ASSERT(i, 0 == em->numSocketEvents(TEST_HANDLES[i]));
            }
        }
        if (verbose)
            cout << "\tTesting 'deregisterSocket'" << endl;
        {
            bslma_TestAllocator testAllocator(veryVeryVerbose);
            Obj mX(&testAllocator);
            const Obj& X = mX;
            const bteso_EventManager *em = X.socketEventManager();

            for (int i = 0; i < NUM_HANDLES; ++i) {
                Obj::Callback nullCb;
                LOOP_ASSERT(i,
                            0 == mX.registerSocketEvent(TEST_HANDLES[i],
                                TEST_EVENTS[i % NUM_EVENTS],  nullCb));
                LOOP_ASSERT(i, 1 == em->numEvents());
                LOOP_ASSERT(i, 1 == em->isRegistered(TEST_HANDLES[i],
                                TEST_EVENTS[i % NUM_EVENTS]));
                LOOP_ASSERT(i, 1 == em->numSocketEvents(TEST_HANDLES[i]));

                mX.deregisterSocket(TEST_HANDLES[i]);

                LOOP_ASSERT(i, 0 == em->numEvents());
                LOOP_ASSERT(i, 0 == em->isRegistered(TEST_HANDLES[i],
                                TEST_EVENTS[i % NUM_EVENTS]));
                LOOP_ASSERT(i, 0 == em->numSocketEvents(TEST_HANDLES[i]));
            }
        }

        if (verbose)
            cout << "\tTesting 'deregisterAllSocketEvents'" << endl;
        {
            bslma_TestAllocator testAllocator(veryVeryVerbose);
            Obj mX(&testAllocator);
            const Obj& X = mX;
            const bteso_EventManager *em = X.socketEventManager();

            for (int i = 0; i < NUM_HANDLES; ++i) {
                for (int j = 0; j <= i; ++j) {
                    Obj::Callback nullCb;
                    LOOP2_ASSERT(i, j,
                                 0 == mX.registerSocketEvent(TEST_HANDLES[j],
                                      TEST_EVENTS[j % NUM_EVENTS],  nullCb));
                    LOOP2_ASSERT(i, j, j + 1 == em->numEvents());
                    LOOP2_ASSERT(i, j, 1 == em->isRegistered(TEST_HANDLES[j],
                                                TEST_EVENTS[j % NUM_EVENTS]));
                    LOOP2_ASSERT(i, j,
                                 1 == em->numSocketEvents(TEST_HANDLES[j]));
                }

                mX.deregisterAllSocketEvents();

                LOOP_ASSERT(i, 0 == em->numEvents());
                for (int j = 0; j <= i; ++j) {
                    LOOP2_ASSERT(i, j, 0 == em->isRegistered(TEST_HANDLES[j],
                                      TEST_EVENTS[j % NUM_EVENTS]));
                    LOOP2_ASSERT(i, j, 0 == em->numSocketEvents(
                                      TEST_HANDLES[j]));
                }
            }
        }
        if (verbose)
            cout << "\tTesting 'deregisterAll'" << endl;
        {
            bslma_TestAllocator testAllocator(veryVeryVerbose);
            Obj mX(&testAllocator);
            const Obj& X = mX;
            const bteso_EventManager *em = X.socketEventManager();

            for (int i = 0; i < NUM_HANDLES; ++i) {
                for (int j = 0; j <= i; ++j) {
                    Obj::Callback nullCb;
                    LOOP2_ASSERT(i, j,
                                 0 == mX.registerSocketEvent(TEST_HANDLES[j],
                                      TEST_EVENTS[j % NUM_EVENTS],  nullCb));
                    LOOP2_ASSERT(i, j, j + 1 == em->numEvents());
                    LOOP2_ASSERT(i, j, 1 == em->isRegistered(TEST_HANDLES[j],
                                                TEST_EVENTS[j % NUM_EVENTS]));
                    LOOP2_ASSERT(i, j,
                                 1 == em->numSocketEvents(TEST_HANDLES[j]));
                }

                mX.deregisterAll();

                LOOP_ASSERT(i, 0 == em->numEvents());
                for (int j = 0; j <= i; ++j) {
                    LOOP2_ASSERT(i, j, 0 == em->isRegistered(TEST_HANDLES[j],
                                      TEST_EVENTS[j % NUM_EVENTS]));
                    LOOP2_ASSERT(i, j, 0 == em->numSocketEvents(
                                      TEST_HANDLES[j]));
                }
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS AND BASIC ACCESSORS
        // Concerns:
        //   Verify that constructors work as expected by creating various
        //   instances using different constructors and using basic accessors
        //   to check the state.
        // Plan:
        //   Particularly, install external socket event
        //   manager and memory allocator and
        // Testing:
        //   bteso_TcpTimerEventManager(bslma_Allocator * = 0);
        //   bteso_TcpTimerEventManager(Hint , bslma_Allocator *);
        //   bteso_TcpTimerEventManager(bteso_EventManager *,
        //                              bslma_Allocator *);
        //   ~bteso_TcpTimerEventManager()
        //   socketEventManager()
        //   timeMetrics()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING CONSTRUCTORS AND BASIC ACCESSORS." << endl
            << "=========================================" << endl;

        if (verbose)
            cout << "\tTesting default constructor." << endl;

        {
            bslma_TestAllocator testAllocator;
            Obj mX(&testAllocator); const Obj& X = mX;
            ASSERT(0 != testAllocator.numAllocations());
            const bteso_EventManager *eventManager = X.socketEventManager();
            ASSERT(eventManager); ASSERT(0 == eventManager->numEvents());
            ASSERT(0 == X.numEvents()); ASSERT(0 == X.numTimers());
            bteso_TimeMetrics *metrics = mX.timeMetrics();
            ASSERT(metrics);
            ASSERT(bteso_TimeMetrics::BTESO_MIN_NUM_CATEGORIES
                   == metrics->numCategories());
            ASSERT(bteso_TimeMetrics::BTESO_CPU_BOUND ==
                   metrics->currentCategory());
        }

        if (verbose)
            cout << "\tTesting constructor with a hint." << endl;

        {
            {
            bslma_TestAllocator testAllocator;
            Obj mX(bteso_TcpTimerEventManager::BTESO_NO_HINT,
                   &testAllocator); const Obj& X = mX;

            ASSERT(0 != testAllocator.numAllocations());
            const bteso_EventManager *eventManager = X.socketEventManager();
            ASSERT(eventManager); ASSERT(0 == eventManager->numEvents());
            ASSERT(0 == X.numEvents()); ASSERT(0 == X.numTimers());
            bteso_TimeMetrics *metrics = mX.timeMetrics();
            ASSERT(metrics);
            ASSERT(bteso_TimeMetrics::BTESO_MIN_NUM_CATEGORIES
                   == metrics->numCategories());
            ASSERT(bteso_TimeMetrics::BTESO_CPU_BOUND ==
                   metrics->currentCategory());
            }

            {
            Obj mX(bteso_TcpTimerEventManager::BTESO_INFREQUENT_REGISTRATION,
                   &testAllocator); const Obj& X = mX;

            ASSERT(0 != testAllocator.numAllocations());
            const bteso_EventManager *eventManager = X.socketEventManager();
            ASSERT(eventManager); ASSERT(0 == eventManager->numEvents());
            ASSERT(0 == X.numEvents()); ASSERT(0 == X.numTimers());
            bteso_TimeMetrics *metrics = mX.timeMetrics();
            ASSERT(metrics);
            ASSERT(bteso_TimeMetrics::BTESO_MIN_NUM_CATEGORIES
                   == metrics->numCategories());
            ASSERT(bteso_TimeMetrics::BTESO_CPU_BOUND ==
                   metrics->currentCategory());
            }
        }

        if (verbose)
            cout << "\tTesting constructor with user-installed event manager."
                 << endl;
        {
            bslma_TestAllocator testAllocator;
            bteso_EventManager *myEventManager
                = (bteso_EventManager*)0xAB;

            Obj mX(myEventManager, &testAllocator); const Obj& X = mX;
            ASSERT(0 != testAllocator.numAllocations());
            const bteso_EventManager *eventManager = X.socketEventManager();
            ASSERT(0xAB == (long)eventManager);

            ASSERT(0 == X.numTimers());
            bteso_TimeMetrics *metrics = mX.timeMetrics();
            ASSERT(metrics);
            ASSERT(bteso_TimeMetrics::BTESO_MIN_NUM_CATEGORIES
                   == metrics->numCategories());
            ASSERT(bteso_TimeMetrics::BTESO_CPU_BOUND ==
                   metrics->currentCategory());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Create an object under test and exercise basic functionality.
        //   This test case doesn't really test anything exhaustively.
        // Testing:
        //  BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bslma_TestAllocator testAllocator;

        Obj mX(&testAllocator);
        const Obj& X = mX;

        ASSERT(0 == X.numEvents());      ASSERT(0 == X.numTimers());
        ASSERT(0 != X.socketEventManager());
        if (verbose)
            cout << "\tDispatching events." << endl;
        ASSERT(0 == mX.dispatch(0));

        if (verbose)
            cout << "\tRegistering a timer." << endl;

        bdet_TimeInterval now = bdetu_SystemTime::now();
        int isInvoked = 0;
        bdef_Function<void (*)()> timer(
                bdef_BindUtil::bind(&timerCallback, &now, &isInvoked));
        void *timerId = mX.registerTimer(now, timer);
        if (veryVerbose) {
            P(now);
            P(timerId);
            P(bdetu_SystemTime::now());
        }

        ASSERT(1 == X.numEvents());      ASSERT(1 == X.numTimers());
        if (verbose)
            cout << "\tDispatching callbacks." << endl;
        ASSERT(1 == mX.dispatch(0));     ASSERT(1 == isInvoked);
        ASSERT(0 == X.numEvents());      ASSERT(0 == X.numTimers());

        ASSERT(0 == testAllocator.numMismatches());
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }
    ASSERT(0 == bteso_SocketImpUtil::cleanup());
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
