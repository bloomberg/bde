// btlso_eventmanagertester.t.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlso_eventmanagertester.h>

#include <btlso_eventmanager.h>
#include <btlso_ioutil.h>
#include <btlso_socketimputil.h>

#include <bslmt_threadutil.h>

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bsls_platform.h>

#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>

using namespace bsl;  // automatically added by script

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The 'btlso::EventManagerTester' component provides an implementation to
// test each event manager by using the same test data and same test script
// interpreting function gg().  It's the most important thing to make sure the
// test script interpreting function gg() works fine.  This test driver
// is mainly to verify the above concern.  To prevent the cyclic dependency
// problem b/w this component and those event managers to be tested by using
// this component, a 'HelperEventManager' class which also inherits from
// 'btlso_eventmanager' as other 'real' event managers is created inside
// this driver.  Instances of 'HelperEventManager' can thus help us go
// through the gg() to simulate the test for a 'real' event manager.
//
//-----------------------------------------------------------------------------
// MANIPULATORS
// [ 7] int testDispatch();
// [ 6] int testRegisterSocketEvent();
// [ 5] int testDeregisterSocketEvent();
// [ 4] int testDeregisterSocket();
// [ 3] int testDeregisterAll();
// ACCESSORS
// [ 2] int testAccessor();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE

//=============================================================================
//          CENTRAL TABLE OF RESULTS OF TESTS OF 'testDispatchPerforamnce'
//                              ON ALL PLATFORMS
//=============================================================================
//
// This test is a compilation of tables at the -1 test cases of
// 'bteso_defaulteventmanager_*.t.cpp'.
//
// (note 'select' has a severe limit on the # of sockets it can listen to,
//  so it has to be treated differently.  Also, Windows has a limitation on
//  the total # of sockets).
//
// This set of test data is OBSOLETE, the tests were redone (see below).  This
// data is only kept around to show that passing a timeout to the dispatcher
// does not seem to significantly degrade performance (in the case of AIX it
// resulted in a slight speedup, but that may just be spurious).
//
// R|N: R: signifies that callback actually read data from sockets (1 byte
//         per socket)
//      N: signifies that callbacks are just a nop function
//
// Linux: ---------------------------------------------------------------------
//   SocketPairs FracBusy TimeOut R|N Platform microSeconds EventManager
//      5000        0        0     R    Linux        20        epoll
//      5000        0        0     R    Linux      3200        poll
//       500        0        0     R    Linux       300        select
//
//      5000        0       0.1    R    Linux        23        epoll
//      5000        0       0.1    R    Linux      2200        poll
//       500        0       0.1    R    Linux       390        select
//
//      5000       0.5       0     R    Linux     11000        epoll
//      5000       0.5       0     R    Linux     12000        poll
//       500       0.5       0     R    Linux      1100        select
//
//      5000       0.5       0     N    Linux      2300        epoll
//      5000       0.5       0     N    Linux      3675        poll
//       500       0.5       0     N    Linux       550        select
//
// Solaris: -------------------------------------------------------------------
//   SocketPairs FracBusy TimeOut R|N Platform microSeconds EventManager
//      5000        0        0     R   Solaris       50        devpoll
//      5000        0        0     R   Solaris      750        poll
//       500        0        0     R   Solaris      430        select
//
//      5000        0       0.1    R   Solaris       52        devpoll
//      5000        0       0.1    R   Solaris      690        poll
//       500        0       0.1    R   Solaris      450        select
//
//      5000       0.5       0     R   Solaris     56000       devpoll
//      5000       0.5       0     R   Solaris     58000       poll
//       500       0.5       0     R   Solaris      5600       select
//
//      5000       0.5       0     N   Solaris     13500       devpoll
//      5000       0.5       0     N   Solaris     24000       poll
//       500       0.5       0     N   Solaris      2250       select
//
// HPUX: ----------------------------------------------------------------------
//   SocketPairs FracBusy TimeOut R|N Platform microSeconds EventManager
//      5000        0        0     R     HPUX        10        devpoll
//      5000        0        0     R     HPUX      8200        poll
//       500        0        0     R     HPUX       148        select
//
//      5000        0       0.1    R     HPUX        13        devpoll
//      5000        0       0.1    R     HPUX      8900        poll
//       500        0       0.1    R     HPUX       143        select
//
//      5000       0.5       0     R     HPUX     49000        devpoll
//      5000       0.5       0     R     HPUX     55000        poll
//       500       0.5       0     R     HPUX      1100        select
//
//      5000       0.5       0     N     HPUX     10500        devpoll
//      5000       0.5       0     N     HPUX     23000        poll
//       500       0.5       0     N     HPUX       400        select
//
// AIX: -----------------------------------------------------------------------
//   SocketPairs FracBusy TimeOut R|N Platform microSeconds EventManager
//      5000        0        0     R     AIX       8400        poll
//      5000        0        0     R     AIX         25        pollset
//      5000        0        0     R     AIX       6600        select
//
//      5000        0       0.1    R     AIX       5800        poll
//      5000        0       0.1    R     AIX         30        pollset
//      5000        0       0.1    R     AIX       4100        select
//
//      5000       0.5       0     R     AIX      17000        poll
//      5000       0.5       0     R     AIX      12500        pollset
//      5000       0.5       0     R     AIX      16500        select
//
//      5000       0.5       0     N     AIX       7500        poll
//      5000       0.5       0     N     AIX       4600        pollset
//      5000       0.5       0     N     AIX       7800        select
//
// Windows: -------------------------------------------------------------------
//   SocketPairs FracBusy TimeOut R|N Platform microSeconds EventManager
//
//       'bdlt::CurrentTime::now()' has a resolution of 1/60th of a second
//       on Windows, so it just reports 0 microseconds everywhere.  It is
//       not worth redoing the test to get meaningful results on Windows,
//       since the only choice of event manager there is 'select.
//
//=============================================================================

//=============================================================================
//                              2nd pass results
//                              ================
//
//
// SocketsTotal: # of sockets being listened to
//
// # Busy: of the sockets being listened to, the # with data (1 byte each)
//         ready for reading
//
// R|N: R: signifies that callback actually read data from sockets (1 byte
//         per socket)
//      N: signifies that callbacks are just a nop function
//
// Microseconds: time to call the 'dispatch' function, in microseconds.
//
// Benchmark of Windows is not done here, since only 'select' works there,
// there is no decision of a default event manager to be made.
//
// ----------------------------------------------------------------------------
// Table of AIX Dispatcher Results, 2nd Pass:
//
//                                  ----------- Microseconds ---------
// SocketsTotal   # Busy    R|N         Poll      Pollset       Select
// ------------   ------    ---     --------     --------     --------
//       12            1     R         38.55        27.81        49.13
//       12            6     R         49.89        37.79        59.42
//       12            6     N         29.58        25.38        40.56
//
//     5000            1     R       5092.62        18.99      5989.99
//     5000         2500     R      13228.7      12034.6      15593.9
//     5000         2500     N       4973.89      3316.96      6724.12
//
//    20000            1     R      27978.8         20.81     32116.5
//    20000        10000     R      69348.3      59668.2      86618.5
//    20000        10000     N      27031.1      17862.9      42468.9
//
// ----------------------------------------------------------------------------
// Table of HP_UX Dispatcher Results, 2nd Pass:
//
//                                  ---- Microseconds ----
// SocketsTotal   # Busy    R|N         Poll     /dev/poll
// ------------   ------    ---     --------     ---------
//       12            1     R             9             8
//       12            6     R            24            21
//       12            6     N             9             7
//
//     5000            1     R          7169             7
//     5000         2500     R         19960         13468
//     5000         2500     N          9736          2365
//
//    20000            1     R         44575             8
//    20000        10000     R        104580         88438
//    20000        10000     N         47988         17161
//
// Note: times given are for the 10th poll.  The first time the set of
// descriptors is polled, there is an addition 3 ms lag when using 'poll', and
// 8-30 ms lag when using '/dev/poll', depending how many ports are being
// listed to.  This is a one time event.
//
//
// ----------------------------------------------------------------------------
// Table of Linux Dispatcher Results, 2nd Pass:
//
//                                  ---- Microseconds ----
// SocketsTotal   # Busy    R|N         Poll         EPoll
// ------------   ------    ---     --------     ---------
//       12            1     R         30.1          21.32
//       12            6     R         46.9          37.44
//       12            6     N         24.2          15.77
//
//     5000            1     R       3124.35         22.78
//     5000         2500     R       8427.31       7355.13
//     5000         2500     N       3215.38       1951.78
//
//    20000            1     R      12586.4          23.5
//    20000        10000     R      36049.7       32431
//    20000        10000     N      13151.7       11217.8
//
//
// ----------------------------------------------------------------------------
// Table of Solaris Dispatcher Results, 2nd Pass:
//
//                                  ---- Microseconds ----
// SocketsTotal   # Busy    R|N         Poll     /dev/poll
// ------------   ------    ---     --------     ---------
//       12            1     R            57            42
//       12            6     R           128            90
//       12            6     N            45            48
//
//      5000           1     R           599            66
//      5000        2500     R         45395         39890
//      5000        2500     N         16744         16911
//
//     20000           1     R          2380            89
//     20000       10000     R        183668        169349
//     20000       10000     N         73020         57642
//
// Note: times given are for the 10th poll.  The first time the set of
// descriptors is polled, there is an addition 33-120 ms lag when using 'poll',
// and 22-93 ms lag when using '/dev/poll', depending how many ports are being
// listed to.  This is a one time event.
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Registration time tests: test case -2 in bteso_defaulteventmanager_*.t.cpp
// Note we don't show results for 'select' on unix platforms other than AIX --
// though 'select' works on other Unix platforms, it cannot handle enough
// sockets to be a contender.
//
//                         ----- Microseconds to Register all Sockets -----
//                                            Event Manager
// Platform NumSockets      devpoll     epoll      poll   pollset    select
// -------- ----------     --------  --------  --------  --------  --------
//     AIX      5000                            21562.8   40285.9   20559.9
//     AIX     20000                            95107.5  168850     87524.4
//
//    HPUX      5000        40080               13111
//    HPUX     20000       219252               49592
//
//   Linux      5000                  14072      8116
//   Linux     20000                  58735     34352
//
// Solaris      5000       117983               49223
// Solaris     20000       471784              219840
// ----------------------------------------------------------------------------

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

typedef btlso::EventManagerTestPair TestPair;

enum {
    MAX_SCRIPT = 50,
    MAX_PORT   = 50,
    BUF_LEN    = 8192
};

enum {
    SUCCESS = 0,
    FAIL    = -1
};

//=============================================================================
//                             HELPER CLASS
//-----------------------------------------------------------------------------
class HelperEventManager : public btlso::EventManager {
    // This helper class provides a dummy event manager to create an instance
    // of btlso::EventManager, which is used to test
    // 'btlso::EventManagerTester'.

  public:
    enum {
        UNSET                 = -1, // The value is unset.
        DISPATCH              =  1, // Call dispatch() w/o timeout requirement.
        DISPATCHTIMEOUT       =  2, // Call dispatch() w/ timeout requirement.
        REGISTERSOCKETEVENT   =  3, // Call registerSocketEvent().
        DEREGISTERSOCKETEVENT =  4, // Call deregisterSocketEvent().
        DEREGISTERSOCKET      =  5, // Call deregisterSocket().
        DEREGISTERALL         =  6, // Call deregisterALL().
        ISREGISTERED          =  7, // Call isRegistered().
        NUMEVENTS             =  8, // Call numEvents().
        NUMSOCKETEVENTS       =  9  // Call numSocketEvents().
    };

    struct OperationDetails {
       int                        d_functionCode;// the function called
       btlso::SocketHandle::Handle d_handle;      // socket handle to worked on
       btlso::EventType::Type      d_event;       // READ/WRITE/ACCEPT/CONNECT
       int                        d_timeout;     // timeout in milliseconds
    };

  private:
    mutable bsl::vector<OperationDetails> d_parameters;

    // not implemented
    HelperEventManager(const HelperEventManager&);
    HelperEventManager& operator==(const HelperEventManager&);

  public:
    // CREATORS
    HelperEventManager(bslma::Allocator *basicAllocator = 0);
        // Create this 'HelperEventManager' object.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator'
        // is 0, global operators 'new' and 'delete' are used.

    ~HelperEventManager();
        // Destroy this object.

    // MANIPULATORS
    virtual int dispatch(const bsls::TimeInterval& timeout,
                         int                      flags);
        // Dummy dispatch: record this operation details such as parameters
        // which is used by this dispatch function.  Return 1 for simulating
        // one dispatched event.

    virtual int dispatch(int flags);
        // Dummy dispatch: record this operation details such as parameters
        // which is used by this dispatch function.  Return 1 for simulating
        // one dispatched event.

    virtual int registerSocketEvent(
                            const btlso::SocketHandle::Handle&   handle,
                            const btlso::EventType::Type         event,
                            const btlso::EventManager::Callback& callback);
        // Dummy function: record this operation details such as parameters
        // which is used by this function.  Return 0 on success and nonzero
        // otherwise.

    virtual void deregisterSocketEvent(
                               const btlso::SocketHandle::Handle& handle,
                               btlso::EventType::Type             event);
        // Dummy function: record this operation details such as parameters
        // which is used by this function.

    virtual int deregisterSocket(const btlso::SocketHandle::Handle& handle);
        // Dummy function: record this operation details such as parameters
        // which is used by this function.

    virtual void deregisterAll();
        // Dummy function: record this operation details such as parameters
        // which is used by this function.

    void clearOpDetails();
        // Remove all parameter data saved for the previous function calls.

    // ACCESSORS
    virtual bool hasLimitedSocketCapacity() const;
        // Return 'true' if this event manager has limited socket capacity, and
        // 'false' otherwise.

    virtual int isRegistered(const btlso::SocketHandle::Handle& handle,
                             const btlso::EventType::Type       event) const;
        // Dummy function: record this operation details such as parameters
        // which is used by this function.

    virtual int numEvents() const;
        // Dummy function: record this operation details such as parameters
        // which is used by this function.

    virtual int numSocketEvents(
                              const btlso::SocketHandle::Handle& handle) const;
        // Dummy function: record this operation details such as parameters
        // which is used by this function.

    const bsl::vector<OperationDetails>& opDetails() const;
        // Return the array of functions called and parameters used to call
        // functions under test.
};

// CREATORS
HelperEventManager::HelperEventManager(bslma::Allocator *basicAllocator)
: d_parameters(basicAllocator)
{
}

HelperEventManager::~HelperEventManager()
{
}

// MANIPULATORS
int HelperEventManager::dispatch(const bsls::TimeInterval& deadline,
                                 int)
{
    OperationDetails info;
    info.d_functionCode = DISPATCHTIMEOUT;
    info.d_handle = UNSET;
    bsls::TimeInterval timeLimit = deadline - bdlt::CurrentTime::now();
    info.d_timeout =
             (int) (timeLimit.seconds() * 1E3 + timeLimit.nanoseconds() / 1E6);
    d_parameters.push_back(info);
    return 1;
}

int HelperEventManager::dispatch(int)
{
    OperationDetails info;
    info.d_functionCode = DISPATCH;
    info.d_handle = UNSET;
    info.d_timeout = UNSET;
    d_parameters.push_back(info);
    return 1;
}

int HelperEventManager::registerSocketEvent(
                  const btlso::SocketHandle::Handle&   socketHandle,
                  const btlso::EventType::Type         event,
                  const btlso::EventManager::Callback& callback)
{
    OperationDetails info;
    info.d_handle = socketHandle;
    info.d_functionCode = REGISTERSOCKETEVENT;
    info.d_event = event;
    info.d_timeout = UNSET;
    d_parameters.push_back(info);

    callback.operator()();

    return 0;
}

void HelperEventManager::deregisterSocketEvent(
            const btlso::SocketHandle::Handle& socketHandle,
            const btlso::EventType::Type       event)
{
    OperationDetails info;
    info.d_handle = socketHandle;
    info.d_event = event;
    info.d_functionCode = DEREGISTERSOCKETEVENT;
    info.d_timeout = UNSET;
    d_parameters.push_back(info);
}

int HelperEventManager::deregisterSocket(
        const btlso::SocketHandle::Handle& socketHandle)
{
    OperationDetails info;
    info.d_handle = socketHandle;
    info.d_functionCode = DEREGISTERSOCKET;
    info.d_timeout = UNSET;
    d_parameters.push_back(info);
    return 1;
}

void HelperEventManager::deregisterAll()
{
    OperationDetails info;
    info.d_handle = UNSET;
    info.d_functionCode = DEREGISTERALL;
    info.d_timeout = UNSET;
    d_parameters.push_back(info);
}

void HelperEventManager::clearOpDetails()
{
    d_parameters.clear();
}

const bsl::vector<HelperEventManager::OperationDetails>&
                                       HelperEventManager::opDetails() const
{
    return d_parameters;
}

// ACCESSORS
bool HelperEventManager::hasLimitedSocketCapacity() const
{
    return false;
}

int HelperEventManager::numSocketEvents(
                        const btlso::SocketHandle::Handle& socketHandle) const
{
    OperationDetails info;
    info.d_handle = socketHandle;
    info.d_functionCode = NUMSOCKETEVENTS;
    info.d_timeout = UNSET;
    d_parameters.push_back(info);
    return 0;
}

int HelperEventManager::numEvents() const
{
    OperationDetails info;
    info.d_handle = UNSET;
    info.d_functionCode = NUMEVENTS;
    info.d_timeout = UNSET;
    d_parameters.push_back(info);
    return 0;
}

int HelperEventManager::isRegistered(
                        const btlso::SocketHandle::Handle& handle,
                        const btlso::EventType::Type       event) const
{
    OperationDetails info;
    info.d_handle = handle;
    info.d_functionCode = ISREGISTERED;
    info.d_timeout = UNSET;
    info.d_event = event;
    d_parameters.push_back(info);
    return 1;
}
static
int buildOpDetails(
             const char*                                        script,
             TestPair                                          *fds,
             bsl::vector<HelperEventManager::OperationDetails> *opDetails)
    // Parse the script and save operation details such as the functions
    // called, parameters passed to that function,  into the specified array
    // 'opDetails'.  Return "SUCCESS", on success and FAIL otherwise.
{
    ASSERT(script);

    int  fd(-1), nt(0);
    int  rc(FAIL);
    char buf[BUF_LEN], c[2];
    c[0] = -1;

    while ('\0' != *script) {
      while (' ' == *script || '{' == *script || '}' == *script ||
             ';' == *script || '\t' == *script) {
          ++script;
      }
      switch (script[0]) {
        case 'T': {         // Commands such as "T5; T3,0" come here.
          rc = sscanf(script, "T%u,%u", (unsigned *)&nt, (unsigned *)&fd);
          HelperEventManager::OperationDetails info;
          info.d_timeout = HelperEventManager::UNSET;
          if (1 == rc) {
              info.d_handle = HelperEventManager::UNSET;
              info.d_functionCode = HelperEventManager::NUMEVENTS;
              opDetails->push_back(info);
          }
          else if (2 == rc) {
              info.d_handle = fds[fd].observedFd();
              info.d_functionCode = HelperEventManager::NUMSOCKETEVENTS;
              opDetails->push_back(info);
          }
          else {
              return FAIL;                                            // RETURN
          }
        } break;
        case '+': {
          int bytes;

          HelperEventManager::OperationDetails info;
          info.d_functionCode = HelperEventManager::REGISTERSOCKETEVENT;

          info.d_timeout = HelperEventManager::UNSET;

          // Read the <fd> field.
          rc = sscanf(script + 1, "%d", &fd);
          if (1 != rc) {
              return FAIL;                                            // RETURN
          }
          info.d_handle = fds[fd].observedFd();

          char d = script[2];
          switch (d) {
            case 'r':
            case 'w':
              rc = sscanf(script + 3, "%d", &bytes);
              if (1 == rc && 0 >= bytes) {
                  return FAIL;                                        // RETURN
              }
              if (1 != rc) {
                  bytes = -1;
              }
              break;
              case 'a':
              case 'c':
                break;
              default: {
                return FAIL;                                          // RETURN
              }
          }
          switch (d) {
            case 'r':
              info.d_event = btlso::EventType::e_READ;
              break;

            case 'w':
              info.d_event = btlso::EventType::e_WRITE;
              break;

            case 'a':
              info.d_event = btlso::EventType::e_ACCEPT;
              break;

            case 'c':
              info.d_event = btlso::EventType::e_CONNECT;
              break;
            default:
              return FAIL;                                            // RETURN
          }
          opDetails->push_back(info);
        } break;
        case '-': { // Commands such as "-1w; -0r; -a; -2" come here.
          rc = sscanf(script, "-%u%[rwac]", (unsigned *)&fd, &c[0]);

          HelperEventManager::OperationDetails info;
          info.d_timeout = HelperEventManager::UNSET;

          if (1 > rc) {
              rc = sscanf(script, "-%c", &c[0]);
              if (1 != rc || 'a' != c[0]) {
                  return FAIL;                                        // RETURN
              }
              info.d_functionCode = HelperEventManager::DEREGISTERALL;
              info.d_handle = HelperEventManager::UNSET;
              opDetails->push_back(info);
              break;
          }

          if (1 == rc) {
              info.d_functionCode = HelperEventManager::DEREGISTERSOCKET;
              info.d_handle = fds[fd].observedFd();
              opDetails->push_back(info);
              break;
          }
          // Start for "rc == 2".
          info.d_functionCode = HelperEventManager::DEREGISTERSOCKETEVENT;
          info.d_handle = fds[fd].observedFd();
          switch (c[0]) {
            case 'r': {
                info.d_event = btlso::EventType::e_READ;
            } break;
            case 'w': {
                info.d_event = btlso::EventType::e_WRITE;
            } break;
            case 'a': {
                info.d_event = btlso::EventType::e_ACCEPT;
            } break;
            case 'c': {
                info.d_event = btlso::EventType::e_CONNECT;
            } break;
            default:
                return FAIL;                                          // RETURN
          }
          opDetails->push_back(info);
      } break;
      case 'D': {
          int msecs(0), rc(-1), nbytes(0);
          char ch;
          // int flags = 0;  // not used
          bsls::TimeInterval deadline(bdlt::CurrentTime::now());
          enum { SLEEP_TIME = 200000 };
          bslmt::ThreadUtil::microSleep(SLEEP_TIME);

          HelperEventManager::OperationDetails info;
          info.d_handle = HelperEventManager::UNSET;
          info.d_timeout = HelperEventManager::UNSET;

          if (3 == sscanf(script, "D%c%d,%d%n", &ch, &msecs, &rc, &nbytes)) {
              if (0 > msecs || 0 > rc) {
                  return FAIL;                                        // RETURN
              }
              info.d_timeout = msecs;
              info.d_functionCode = HelperEventManager::DISPATCHTIMEOUT;
              switch (ch) {
                case 'n': {
                    // flags = 0;
                } break;
                case 'i': {
                    // flags = 0;
                } break;
                default:
                    return FAIL;                                      // RETURN
              }
          }
          else if (2 == sscanf(script, "D%c,%u%n", &ch, (unsigned *)&rc,
                                                                    &nbytes)) {
              if (0 > rc) {
                  return FAIL;                                        // RETURN
              }
              switch (ch) {
                case 'n': {
                    // flags = 0;
                } break;
                case 'i': {
                    // flags = 0;
                } break;
                default:
                    return FAIL;                                      // RETURN
              }
              info.d_functionCode = HelperEventManager::DISPATCH;
          }
          else {
              return FAIL;                                            // RETURN
          }
          opDetails->push_back(info);

      } break;
      case 'E': {
          rc = sscanf(script, "E%d%[acrw]", &fd, buf);

          if (rc == 2) {
              if (strchr(buf, 'a')) {
                  HelperEventManager::OperationDetails info;
                  info.d_functionCode = HelperEventManager::ISREGISTERED;
                  info.d_handle = fds[fd].observedFd();
                  info.d_event = btlso::EventType::e_ACCEPT;
                  info.d_timeout = HelperEventManager::UNSET;
                  opDetails->push_back(info);
              }
              if (strchr(buf, 'c')) {
                  HelperEventManager::OperationDetails info;
                  info.d_functionCode = HelperEventManager::ISREGISTERED;
                  info.d_handle = fds[fd].observedFd();
                  info.d_event = btlso::EventType::e_CONNECT;
                  info.d_timeout = HelperEventManager::UNSET;
                  opDetails->push_back(info);
              }
              if (strchr(buf, 'r')) {
                  HelperEventManager::OperationDetails info;
                  info.d_functionCode = HelperEventManager::ISREGISTERED;
                  info.d_handle = fds[fd].observedFd();
                  info.d_event = btlso::EventType::e_READ;
                  info.d_timeout = HelperEventManager::UNSET;
                  opDetails->push_back(info);
              }
              if (strchr(buf, 'w')) {
                  HelperEventManager::OperationDetails info;
                  info.d_functionCode = HelperEventManager::ISREGISTERED;
                  info.d_handle = fds[fd].observedFd();
                  info.d_event = btlso::EventType::e_WRITE;
                  info.d_timeout = HelperEventManager::UNSET;
                  opDetails->push_back(info);
              }
          }
          else {
              HelperEventManager::OperationDetails info;
              info.d_functionCode = HelperEventManager::NUMSOCKETEVENTS;
              info.d_handle = fds[fd].observedFd();
              info.d_timeout = HelperEventManager::UNSET;
              opDetails->push_back(info);
          }
      } break;
      case 'R':
      case 'W':
      case 'S':
        break;

      default:
          return FAIL;                                                // RETURN
      }
      while (' ' != *script && '{' != *script && '}' != *script &&
             ';' != *script && '\t' != *script) {
          // Go to next command.
          if ('\0' == *script) {
              return SUCCESS;                                         // RETURN
          }
          ++script;
      }
   }
   return SUCCESS;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << endl;

    int errCode = 0;
    btlso::SocketImpUtil::startup(&errCode);
    ASSERT(0 == errCode);
    bslma::TestAllocator testAllocator(veryVeryVerbose);
    testAllocator.setNoAbort(1);

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
        // ------------------------------------------------------------------
        // TESTING SOCKET START-UP
        //
        // Concern:
        //   That newly created socket does correct i/o.  HPUX sockets are
        //   observed to take ~ 20 ms to get started properly.
        //
        // Plan:
        //   Write to a socket IMMEDIATELY after creating it.  Verify that
        //   the data is eventually received correctly.
        // ------------------------------------------------------------------

        const char *testData =
                        "There are more things in heaven and earth, Horatio,\n"
                        "than are dreamt of in your philosophy.\n";
        const int testLen = bsl::strlen(testData);

        bsls::TimeInterval start = bdlt::CurrentTime::now();

        btlso::EventManagerTestPair mX(veryVerbose);
        int rc = btlso::IoUtil::setBlockingMode(mX.observedFd(),
                                               btlso::IoUtil::e_BLOCKING);
        ASSERT(0 == rc);

        rc = btlso::SocketImpUtil::write(mX.controlFd(), testData, testLen);
        ASSERT(testLen == rc);
        char readBuf[1000];
        rc = btlso::SocketImpUtil::read(readBuf, mX.observedFd(), testLen);
        bsls::TimeInterval finish = bdlt::CurrentTime::now();
        LOOP2_ASSERT(testLen, rc, testLen == rc);
        ASSERT(0 == bsl::memcmp(readBuf, testData, testLen));

        double elapsed = (finish - start).totalSecondsAsDouble();
        LOOP_ASSERT(elapsed, elapsed <= 0.40);

        // verify that now that the socket's woken up, it's quite fast

        bsl::memset(readBuf, 0, testLen);
        rc = btlso::SocketImpUtil::write(mX.controlFd(), testData, testLen);
        ASSERT(testLen == rc);
        rc = btlso::SocketImpUtil::read(readBuf, mX.observedFd(), testLen);
        bsls::TimeInterval finish2 = bdlt::CurrentTime::now();
        LOOP2_ASSERT(testLen, rc, testLen == rc);
        ASSERT(0 == bsl::memcmp(readBuf, testData, testLen));

        double elapsed2 = (finish2 - finish).totalSecondsAsDouble();
        LOOP2_ASSERT(elapsed, elapsed2, elapsed2 <= 0.0001);

        if (verbose) {
            P_(elapsed) P(elapsed2);
        }
      } break;
      case 11: {
        // ------------------------------------------------------------------
        // TESTING btlso::EventManagerTestPair
        // Concerns:
        //   o a pair can be created with different verbose modes
        //   o accessors work as expected
        //   o able to set/get buffer sizes and watermarks
        // Plan:
        //   Create a socket pair and address the concerns
        // Testing:
        //   btlso::EventManagerTestPair
        // ------------------------------------------------------------------
        if (verbose) cout << "TESTING bteso_EventManagerTestPair." << endl
                          << "===================================" << endl;
        btlso::EventManagerTestPair mX(veryVerbose);
        const btlso::EventManagerTestPair& X = mX;
        ASSERT(mX.isValid());
#if defined(BTLSO_PLATFORM_BSD_SOCKETS)
        ASSERT(0 < X.observedFd());         ASSERT(0 < X.controlFd());
#endif
#if defined(BTLSO_PLATFORM_WIN_SOCKETS)
        ASSERT(NULL != X.observedFd());     ASSERT(NULL != X.controlFd());
#endif
      } break;
      case 10: {
        // ------------------------------------------------------------------
        // USAGE EXAMPLE:
        //   Test building the operation details which will be used to verify
        //   the right functions and parameters passed for each call.
        // Plan:
        //   Create a set of scripts which include all kinds of commands,
        //   invoke the corresponding function to build and verify the array
        //   to save all the information.
        // ------------------------------------------------------------------
        if (verbose) cout << "Test building operation details." << endl
                          << "================================" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
              {L_, 0, "-a"},
              {L_, 0, "T0"},
              {L_, 0, "-1; -0"},
              {L_, 0, "-1r;-0w"},
              {L_, 0, "T0; T0,1"},
              {L_, 0, "Di,1; Dn,1"},
              {L_, 0, "Di100,1; Dn260,1"},
              {L_, 0, "E0rwac; E2ac;  E1wa"},
              {L_, 0, "Di,1; Dn,1;  Di150,1; Dn400,1"},
              {L_, 0, "T0; +0w21; E2rw; W1,20; +1r11; -0; -1r; -2w"},
              {L_, 0, "+1w60; W1,60; +0r12,{+2r10,{+1r16};+0r8}; +2r15"},
              {L_, 0, "W1,70; +2w64,{+1r10,{+0w30};+2r25}; W0,60; +0r30;"
                      "+1w35"},
            };

            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {
                HelperEventManager mX(&testAllocator);

                enum { NUM_PAIRS = 3 };
                TestPair socketPairs[NUM_PAIRS];

                for (int j = 0; j < NUM_PAIRS; ++j) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                }
                bsl::vector<HelperEventManager::OperationDetails>
                                              opDetails(&testAllocator);
                buildOpDetails(SCRIPTS[i].d_script, socketPairs, &opDetails);
                if (verbose) {
                    int len = opDetails.size();
                    for (int j = 0; j < len; ++j) {
                        cout << opDetails[j].d_functionCode << "; "
                             << opDetails[j].d_handle << "; "
                             << opDetails[j].d_timeout << "; "
                             << opDetails[j].d_event << endl;
                    }
                    cout << endl;
                }
            }
        }
      } break;
      case 9: {
        // ------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // ------------------------------------------------------------------
        if (verbose) bsl::cout << endl
                               << "USAGE EXAMPLE" << endl
                               << "=============" << endl;

        // The following snippets of code illustrate how to use this utility
        // component for a 'standard' test.
        // First create a btlso::TimeMetrics object 'tm' and an event manager
        // object under test 'mX'.

        // bteso_EventManagerImp_Poll mX();

        // Then call a function to be tested in the event manager.
        // Notice that only one example is given here, the same usage for
        // other functions.

        // int ctrlFlag = 0;
        // if (veryVeryVerbose) {
        //    ctrlFlag |= btlso::EventManagerTester::k_VERY_VERY_VERBOSE;
        //}
        //else if (veryVerbose) {
        //    ctrlFlag |= btlso::EventManagerTester::k_VERY_VERBOSE;
        //}
        //else if (verbose) {
        //    ctrlFlag |= btlso::EventManagerTester::k_VERBOSE;
        //}
        //btlso::EventManagerTester::testRegisterSocketEvent(&mX, ctrlFlag);
        // The following snippets of code illustrate how to use this utility
        // component to perform a 'customized' test.
        // First an array of sockets that will be used
        // in the test script is given as 'fds' and connection(s) have been
        // created.

        typedef btlso::EventManagerTestPair SocketPair;
        enum { NUM_PAIRS = 4 }; // a number at your choice
        TestPair socketPairs[NUM_PAIRS];

        for (int j = 0; j < NUM_PAIRS; j++) {
            socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
            socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
        }
        // Secondly, construct the test script which is a "customized" one
        // 'script' especially for the event manager under test and the event
        // manager object 'mX2'.

        const char *script =
                      "+1w12,{+0w10,{+0w16};+2w25};+0w30;+0w35";
        const int LINE = __LINE__;

        // Then create an object 'mX2' of the event manager under test.

         HelperEventManager mX2;
        // Finally, invoke gg() of this component by passing arguments
        // created above.
        int ctrlFlag = 0;
        if (veryVeryVerbose) {
                ctrlFlag |= btlso::EventManagerTester::k_VERY_VERY_VERBOSE;
        }
        else if (veryVerbose) {
            ctrlFlag |= btlso::EventManagerTester::k_VERY_VERBOSE;
        }
        else if (verbose) {
            ctrlFlag |= btlso::EventManagerTester::k_VERBOSE;
        }
        int fails = btlso::EventManagerTester::gg(&mX2, socketPairs,
                                                 script, ctrlFlag);
        LOOP_ASSERT(LINE, 0 == fails);

      } break;
      case 8: {
        // ------------------------------------------------------------------
        // TESTING 'sleep'
        //
        // Plan:
        //   Issue a 'sleep' command and verify that an appropriate amount of
        //   time passed.
        // ------------------------------------------------------------------

        if (verbose) bsl::cout << "Testing 'gg' for sleep\n"
                                  "======================\n";

        double start   = bdlt::CurrentTime::now().totalSecondsAsDouble();
        int fails = btlso::EventManagerTester::gg(0, 0, "S100; S150", 0);
        double elapsed = bdlt::CurrentTime::now().totalSecondsAsDouble() -
                                                                         start;
        ASSERT(0 == fails);
        ASSERT(elapsed >= 0.25);
      } break;
      case 7: {
        // ------------------------------------------------------------------
        // TESTING 'testDispatch':
        //   To simulate the 'testDispatch()' in this component.
        //
        // Plan:
        //   Create a set of scripts to go through every branch in gg() where
        //   'testDispatch()' does.
        //
        // Testing:
        //   int testDispatch();
        // ------------------------------------------------------------------

        int fails = 0;

        if (verbose) bsl::cout << endl
                          << "Testing 'gg' for dispatching a request" << endl
                          << "======================================" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
               {L_, 0, "Di,1; Dn,1"},
            };
            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {
                HelperEventManager mX(&testAllocator);

                const int LINE =  SCRIPTS[i].d_line;

                enum { NUM_PAIRS = 2 };
                TestPair socketPairs[NUM_PAIRS];

                for (int j = 0; j < NUM_PAIRS; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                }
                int ctrlFlag = 0;
                fails = btlso::EventManagerTester::gg(&mX, socketPairs,
                                                     SCRIPTS[i].d_script,
                                                     ctrlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                bsl::vector<HelperEventManager::OperationDetails>
                                                opDetails(&testAllocator);
                int rc = buildOpDetails(SCRIPTS[i].d_script,
                                        socketPairs,
                                        &opDetails);

                LOOP_ASSERT(i, SUCCESS == rc);
                const bsl::vector<HelperEventManager::OperationDetails>&
                    details = mX.opDetails();
                int len = opDetails.size();
                LOOP_ASSERT(i, len == (int) details.size());

                for (int j = 0; j < len; ++j) {
                    if (veryVerbose) {
                        cout << opDetails[j].d_functionCode << "; "
                             << opDetails[j].d_handle << "; "
                             << opDetails[j].d_timeout << "; "
                             << opDetails[j].d_event << endl;
                    }
                    LOOP_ASSERT(j, opDetails[j].d_functionCode ==
                                           details[j].d_functionCode);
                    LOOP_ASSERT(j, opDetails[j].d_timeout ==
                                           details[j].d_timeout);
                    LOOP_ASSERT(j, opDetails[j].d_handle ==
                                           details[j].d_handle);
                }
                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }

        if (verbose) bsl::cout << endl
            << "Testing 'gg' for dispatching a request with a timeout value\n"
            << "===========================================================\n";
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
               {L_, 0, "Di100,1; Dn260,1"},
            };

            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {
                HelperEventManager mX(&testAllocator);

                const int LINE =  SCRIPTS[i].d_line;

                enum { NUM_PAIRS = 2 };
                TestPair socketPairs[NUM_PAIRS];

                for (int j = 0; j < NUM_PAIRS; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                }
                int ctrlFlag = 0;
                fails = btlso::EventManagerTester::gg(&mX, socketPairs,
                                                     SCRIPTS[i].d_script,
                                                     ctrlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                bsl::vector<HelperEventManager::OperationDetails>
                                                opDetails(&testAllocator);
                int rc = buildOpDetails(SCRIPTS[i].d_script,
                                        socketPairs,
                                        &opDetails);

                LOOP_ASSERT(i, SUCCESS == rc);
                const bsl::vector<HelperEventManager::OperationDetails>&
                                                  details = mX.opDetails();
                int len = opDetails.size();
                LOOP_ASSERT(i, len == (int) details.size());

                for (int j = 0; j < len; ++j) {
                    if (veryVerbose) {
                        cout << opDetails[j].d_functionCode << "; "
                             << opDetails[j].d_handle << "; "
                             << opDetails[j].d_timeout << "; "
                             << opDetails[j].d_event << endl;
                    }
                    LOOP_ASSERT(j, opDetails[j].d_functionCode ==
                                           details[j].d_functionCode);
                    // The comparison of time won't work most times because
                    // the absolute time value recovered usually doesn't
                    // equal to its originally passed value.
                    LOOP_ASSERT(j, opDetails[j].d_timeout > 0);
                    LOOP_ASSERT(j, details[j].d_timeout > 0);

                    LOOP_ASSERT(j, opDetails[j].d_handle ==
                                                details[j].d_handle);
                }
                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }
      } break;
      case 6: {
        // ------------------------------------------------------------------
        // TEST testRegisterSocketEvent:
        //   To simulate the 'testRegisterSocketEvent()' in this component.
        //
        // Plan:
        //   Create a set of scripts to go through every branch in gg() where
        //   'testRegisterSocketEvent()' does.
        //
        // Testing:
        //   int testRegisterSocketEvent();
        // ------------------------------------------------------------------
        int fails = 0;

        if (verbose) bsl::cout << endl
               << "Testing gg() for registering socket events" << endl
               << "==========================================" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
          {
            {L_, 0, "+1w12; W1,30; +1r8; +1w20; +1w6"},
            {L_, 0, "+1w32,{+0w10; W1,50; +1r25}; +0w30"},
            {L_, 0, "+1w12,{+0w10,{+0w16};+2w25};+0w30;+0w35"},
            {L_, 0, "+1w60; W1,60; +1r12,{+1r10,{+1r16};+1r8}; +1r14"},
            {L_, 0, "W1,70; +1w64,{+1r10,{+0w30};+1r25}; W0,60; +0r30; +0w35"},
          };

            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {
                HelperEventManager mX(&testAllocator);

                const int LINE =  SCRIPTS[i].d_line;

                enum { NUM_PAIRS = 3 };
                TestPair socketPairs[NUM_PAIRS];

                for (int j = 0; j < NUM_PAIRS; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                }
                int ctrlFlag = 0;
                fails = btlso::EventManagerTester::gg(&mX, socketPairs,
                                                     SCRIPTS[i].d_script,
                                                     ctrlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                bsl::vector<HelperEventManager::OperationDetails>
                                                  opDetails(&testAllocator);
                int rc = buildOpDetails(SCRIPTS[i].d_script,
                                        socketPairs,
                                        &opDetails);

                LOOP_ASSERT(i, SUCCESS == rc);
                const bsl::vector<HelperEventManager::OperationDetails>&
                    details = mX.opDetails();
                int len = opDetails.size();
                LOOP_ASSERT(i, len == (int) details.size());

                for (int j = 0; j < len; ++j) {
                    if (veryVerbose) {
                        cout << opDetails[j].d_functionCode << "; "
                             << opDetails[j].d_handle << "; "
                             << opDetails[j].d_timeout << "; "
                             << opDetails[j].d_event << endl;
                    }
                    LOOP_ASSERT(j, opDetails[j].d_functionCode ==
                                           details[j].d_functionCode);
                    LOOP_ASSERT(j, opDetails[j].d_timeout ==
                                           details[j].d_timeout);
                    LOOP_ASSERT(j, opDetails[j].d_handle ==
                                           details[j].d_handle);
                    if (HelperEventManager::ISREGISTERED ==
                                               opDetails[j].d_functionCode ||
                        HelperEventManager::REGISTERSOCKETEVENT ==
                                               opDetails[j].d_functionCode ||
                        HelperEventManager::DEREGISTERSOCKETEVENT ==
                                               opDetails[j].d_functionCode) {
                        LOOP_ASSERT(j, opDetails[j].d_event ==
                                                 details[j].d_event);
                    }
                }

                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }
      } break;
      case 5: {
        // ------------------------------------------------------------------
        // TEST 'testDeregisterSocketEvent()':
        //   To simulate the 'testDeregisterSocketEvent()' in this component.
        //
        // Plan:
        //   Create a set of scripts to go through every branch in gg() where
        //   'testDeregisterSocketEvent()' does.
        //
        // Testing:
        //   int testDeregisterSocketEvent();
        // ------------------------------------------------------------------
        int fails = 0;

        if (verbose) bsl::cout << endl
            << "Testing 'gg' for deregistering socket svents" << endl
            << "============================================" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
               {L_, 0, "-1r;-0w"},
            };

            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {
                HelperEventManager mX(&testAllocator);

                const int LINE =  SCRIPTS[i].d_line;

                enum { NUM_PAIRS = 2 };
                TestPair socketPairs[NUM_PAIRS];

                for (int j = 0; j < NUM_PAIRS; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                }
                int ctrlFlag = 0;
                fails = btlso::EventManagerTester::gg(&mX, socketPairs,
                                                     SCRIPTS[i].d_script,
                                                     ctrlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                bsl::vector<HelperEventManager::OperationDetails>
                                                   opDetails(&testAllocator);
                buildOpDetails(SCRIPTS[i].d_script, socketPairs, &opDetails);
                const bsl::vector<HelperEventManager::OperationDetails>&
                    details = mX.opDetails();
                int len = opDetails.size();
                LOOP_ASSERT(i, len == (int) details.size());

                for (int j = 0; j < len; ++j) {
                    if (veryVerbose) {
                        cout << opDetails[j].d_functionCode << "; "
                             << opDetails[j].d_handle << "; "
                             << opDetails[j].d_timeout << "; "
                             << opDetails[j].d_event << endl;
                    }
                    LOOP_ASSERT(j, opDetails[j].d_functionCode ==
                                           details[j].d_functionCode);
                    LOOP_ASSERT(j, opDetails[j].d_timeout ==
                                           details[j].d_timeout);
                    LOOP_ASSERT(j, opDetails[j].d_handle ==
                                           details[j].d_handle);
                    if (HelperEventManager::ISREGISTERED ==
                                               opDetails[j].d_functionCode ||
                        HelperEventManager::REGISTERSOCKETEVENT ==
                                               opDetails[j].d_functionCode ||
                        HelperEventManager::DEREGISTERSOCKETEVENT ==
                                               opDetails[j].d_functionCode) {
                        LOOP_ASSERT(j, opDetails[j].d_event ==
                                                 details[j].d_event);
                    }
                }

                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }
      } break;
      case 4: {
        // ------------------------------------------------------------------
        // TEST 'testDeregisterSocket()':
        //   To simulate the 'testDeregisterSocket()' in this component.
        //
        // Plan:
        //   Create a set of scripts to go through every branch in gg() where
        //   'testDeregisterSocket()' does.
        //
        // Testing:
        //   int testDeregisterSocket();
        // ------------------------------------------------------------------
        int fails = 0;

        if (verbose) bsl::cout << endl
            << "Testing 'gg' for deregistering events of a socket" << endl
            << "=================================================" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
               {L_, 0, "-1; -0"},
            };

            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {
                HelperEventManager mX(&testAllocator);

                const int LINE =  SCRIPTS[i].d_line;

                enum { NUM_PAIRS = 2 };
                TestPair socketPairs[NUM_PAIRS];

                for (int j = 0; j < NUM_PAIRS; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                }
                int ctrlFlag = 0;
                fails = btlso::EventManagerTester::gg(&mX, socketPairs,
                                                     SCRIPTS[i].d_script,
                                                     ctrlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                bsl::vector<HelperEventManager::OperationDetails>
                                                 opDetails(&testAllocator);
                buildOpDetails(SCRIPTS[i].d_script, socketPairs, &opDetails);
                const bsl::vector<HelperEventManager::OperationDetails>&
                    details = mX.opDetails();
                int len = opDetails.size();
                LOOP_ASSERT(i, len == (int) details.size());

                for (int j = 0; j < len; ++j) {
                    if (veryVerbose) {
                        cout << opDetails[j].d_functionCode << "; "
                             << opDetails[j].d_handle << "; "
                             << opDetails[j].d_timeout << "; "
                             << opDetails[j].d_event << endl;
                    }
                    LOOP_ASSERT(j, opDetails[j].d_functionCode ==
                                           details[j].d_functionCode);
                    LOOP_ASSERT(j, opDetails[j].d_timeout ==
                                           details[j].d_timeout);
                    LOOP_ASSERT(j, opDetails[j].d_handle ==
                                           details[j].d_handle);
                }
                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }
      } break;
      case 3: {
        // ------------------------------------------------------------------
        // TEST 'testDeregisterAll':
        //   To simulate the 'testDeregisterAll()' in this component.
        //
        // Plan:
        //   Create a set of scripts to go through every branch in gg() where
        //   'testDeregisterAll()' does.
        //
        // Testing:
        //   int testDeregisterAll();
        // ------------------------------------------------------------------
        int fails = 0;

        if (verbose) bsl::cout << endl
                   << "Testing 'gg' for deregistering all events" << endl
                   << "=========================================" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
               {L_, 0, "-a"},
            };

            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {
                HelperEventManager mX(&testAllocator);

                const int LINE =  SCRIPTS[i].d_line;

                enum { NUM_PAIRS = 2 };
                TestPair socketPairs[NUM_PAIRS];

                for (int j = 0; j < NUM_PAIRS; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                }
                int ctrlFlag = 0;
                fails = btlso::EventManagerTester::gg(&mX, socketPairs,
                                                     SCRIPTS[i].d_script,
                                                     ctrlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                bsl::vector<HelperEventManager::OperationDetails>
                                                 opDetails(&testAllocator);
                buildOpDetails(SCRIPTS[i].d_script, socketPairs, &opDetails);
                const bsl::vector<HelperEventManager::OperationDetails>&
                    details = mX.opDetails();
                int len = opDetails.size();
                LOOP_ASSERT(i, len == (int) details.size());

                for (int j = 0; j < len; ++j) {
                    if (veryVerbose) {
                        cout << opDetails[j].d_functionCode << "; "
                             << opDetails[j].d_handle << "; "
                             << opDetails[j].d_timeout << "; "
                             << opDetails[j].d_event << endl;
                    }
                    LOOP_ASSERT(j, opDetails[j].d_functionCode ==
                                           details[j].d_functionCode);
                    LOOP_ASSERT(j, opDetails[j].d_timeout ==
                                           details[j].d_timeout);
                    LOOP_ASSERT(j, opDetails[j].d_handle ==
                                           details[j].d_handle);
                }
                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }
      } break;
      case 2: {
        // ------------------------------------------------------------------
        // TEST 'testAccessor':
        //   To simulate the 'testAccessor()' in this component.
        //
        // Plan:
        //   Create a set of scripts to go through every branch in gg() where
        //   'testAccessor()' does.
        //
        // Testing:
        //   int testAccessor();
        // ------------------------------------------------------------------
        int fails = 0;
        if (verbose) bsl::cout << endl
                          << "Testing 'gg' for accessor functions" << endl
                          << "===================================" << endl;
        {
            struct {
                int         d_line;
                int         d_fails;  // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
               {L_, 0, "E0rwac"},
               {L_, 0, "T0"},
               {L_, 0, "T0; T0,1"},
            };

            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {
                HelperEventManager mX(&testAllocator);

                const int LINE =  SCRIPTS[i].d_line;

                enum { NUM_PAIRS = 2 };
                TestPair socketPairs[NUM_PAIRS];

                for (int j = 0; j < NUM_PAIRS; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                }
                int ctrlFlag = 0;
                fails = btlso::EventManagerTester::gg(&mX, socketPairs,
                                                     SCRIPTS[i].d_script,
                                                     ctrlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                bsl::vector<HelperEventManager::OperationDetails>
                                                  opDetails(&testAllocator);
                buildOpDetails(SCRIPTS[i].d_script, socketPairs, &opDetails);
                const bsl::vector<HelperEventManager::OperationDetails>&
                    details = mX.opDetails();
                int len = opDetails.size();
                LOOP_ASSERT(i, len == (int) details.size());

                for (int j = 0; j < len; ++j) {
                    if (veryVerbose) {
                        cout << opDetails[j].d_functionCode << "; "
                             << opDetails[j].d_handle << "; "
                             << opDetails[j].d_timeout << "; "
                             << opDetails[j].d_event << endl;
                    }
                    LOOP_ASSERT(j, opDetails[j].d_functionCode ==
                                           details[j].d_functionCode);
                    LOOP_ASSERT(j, opDetails[j].d_timeout ==
                                           details[j].d_timeout);
                    LOOP_ASSERT(j, opDetails[j].d_handle ==
                                           details[j].d_handle);
                    if (HelperEventManager::ISREGISTERED ==
                                               opDetails[j].d_functionCode ||
                        HelperEventManager::REGISTERSOCKETEVENT ==
                                               opDetails[j].d_functionCode ||
                        HelperEventManager::DEREGISTERSOCKETEVENT ==
                                               opDetails[j].d_functionCode) {
                        LOOP_ASSERT(j, opDetails[j].d_event ==
                                                 details[j].d_event);
                    }
                }
                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }
      } break;
      case 1: {
        // ------------------------------------------------------------------
        // BREATHING TEST:
        //   Simulate to go through the gg() as a a test for a 'real' event
        //   manager does.
        //
        // Plan:
        //   Create a set of scripts, call gg() separately and verify the
        //   result.
        // ------------------------------------------------------------------
        if (verbose) bsl::cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;
        {
            int fails = 0;
            struct {
                int         d_line;
                int         d_fails;    // number of failures in this script
                const char *d_script;
            } SCRIPTS[] =
            {
               {L_, 0, "T0; E0r; E0rwa; E1caw; E0rwac"},
#if defined(BSLS_PLATFORM_OS_HPUX)
               {L_, 0, "W0,30; S40; R0,24"},
#else
               {L_, 0, "W0,30; S1; R0,24"},
#endif
               {L_, 0, "Di,1; Dn,1;  Di150,1; Dn400,1"},
#if defined(BSLS_PLATFORM_OS_HPUX)
               {L_, 0, "T0; +0w21; W1,20; S40; +1r11"},
#else
               {L_, 0, "T0; +0w21; W1,20; S1; +1r11"},
#endif
            };

            const int NUM_SCRIPTS = sizeof SCRIPTS / sizeof *SCRIPTS;

            for (int i = 0; i < NUM_SCRIPTS; ++i) {
                HelperEventManager mX(&testAllocator);

                const int LINE =  SCRIPTS[i].d_line;

                enum { NUM_PAIRS = 2 };
                TestPair socketPairs[NUM_PAIRS];

                for (int j = 0; j < NUM_PAIRS; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                }
                int ctrlFlag = 0;
                fails = btlso::EventManagerTester::gg(&mX, socketPairs,
                                                     SCRIPTS[i].d_script,
                                                     ctrlFlag);
                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                bsl::vector<HelperEventManager::OperationDetails>
                                                 opDetails(&testAllocator);
                buildOpDetails(SCRIPTS[i].d_script, socketPairs, &opDetails);
                const bsl::vector<HelperEventManager::OperationDetails>&
                    details = mX.opDetails();
                int len = opDetails.size();
                LOOP_ASSERT(i, len == (int) details.size());

                for (int j = 0; j < len; ++j) {
                    if (veryVerbose) {
                        cout << opDetails[j].d_functionCode << "; "
                             << opDetails[j].d_handle << "; "
                             << opDetails[j].d_timeout << "; "
                             << opDetails[j].d_event << endl;
                    }
                    LOOP_ASSERT(j, opDetails[j].d_functionCode ==
                                           details[j].d_functionCode);
                    if (HelperEventManager::DISPATCHTIMEOUT ==
                                             opDetails[j].d_functionCode) {
                        // This is the "real" time value.
                        LOOP_ASSERT(j, opDetails[j].d_timeout > 0);
                        LOOP_ASSERT(j, details[j].d_timeout > 0);
                    }
                    else {
                        // This time value should be "UNSET".
                        LOOP_ASSERT(j, opDetails[j].d_timeout ==
                                                    details[j].d_timeout);
                    }
                    LOOP_ASSERT(j, opDetails[j].d_handle ==
                                           details[j].d_handle);

                    if (HelperEventManager::ISREGISTERED ==
                                               opDetails[j].d_functionCode ||
                        HelperEventManager::REGISTERSOCKETEVENT ==
                                               opDetails[j].d_functionCode ||
                        HelperEventManager::DEREGISTERSOCKETEVENT ==
                                               opDetails[j].d_functionCode) {
                        LOOP_ASSERT(j, opDetails[j].d_event ==
                                                 details[j].d_event);
                    }
                }

                if (veryVerbose) {
                    P_(LINE);   P(fails);
                }
            }
        }
      } break;

      case -1: {
        // -----------------------------------------------------------------
        // Interactive gg test shell
        // -----------------------------------------------------------------

        while (1) {
            char script[1000];
            cout << "Enter script: " << flush;
            cin.getline(script, 1000);

            if (cin.eof() && '\0' == script[0]) {
                cout << endl;
                break;
            }
            if (0 == bsl::strncmp(script, "quit", 4)) {
                break;
            }

            int i = 0;
            for (; i < 4; ++i) {
                HelperEventManager mX(&testAllocator);

                const int NUM_PAIR = 4;
                btlso::EventManagerTestPair socketPairs[NUM_PAIR];

                for (int j = 0; j < NUM_PAIR; j++) {
                    socketPairs[j].setObservedBufferOptions(BUF_LEN, 1);
                    socketPairs[j].setControlBufferOptions(BUF_LEN, 1);
                }

                int ctrlFlag = 0;
                int fails = btlso::EventManagerTester::gg(&mX, socketPairs,
                                                         script,
                                                         ctrlFlag);
                if (fails) {
                    break;
                }
            }
            if (4 == i) {
                cout << "Success!\n";
            }
        }
      } break;

      default: {
          cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
          testStatus = -1;
      }
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status: " << testStatus << "." << endl;
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
