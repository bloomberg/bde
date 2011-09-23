// bteso_eventmanagertester.t.cpp  -*-C++-*-

#include <bteso_eventmanagertester.h>

#include <bteso_socketimputil.h>
#include <bteso_eventmanager.h>
#include <bcemt_thread.h>

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bsls_platform.h>

#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_strstream.h>
#include <bsl_vector.h>

using namespace bsl;  // automatically added by script

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The 'bteso_EventManagerTester' component provides an implementation to
// test each event manager by using the same test data and same test script
// interpreting function gg().  It's the most important thing to make sure the
// test script interpreting function gg() works fine.  This test driver
// is mainly to verify the above concern.  To prevent the cyclic dependency
// problem b/w this component and those event managers to be tested by using
// this component, a 'HelperEventManager' class which also inherits from
// 'bteso_eventmanager' as other 'real' event managers is created inside
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
// [ 1] Breathing Test
// [ 8] USAGE Example
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

typedef bteso_EventManagerTestPair TestPair;

enum {
    MAX_SCRIPT = 50,
    MAX_PORT   = 50,
    BUF_LEN    = 8192
};

enum {
    SUCCESS = 0,
    FAIL    = -1
};

//==========================================================================
//                             HELPER CLASS
//--------------------------------------------------------------------------
class HelperEventManager : public bteso_EventManager
{
    // This helper class provides a dummy event manager to create an instance
    // of bteso_EventManager, which is used to test 'bteso_EventManagerTester'.

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
       bteso_SocketHandle::Handle d_handle;      // socket handle to worked on
       bteso_EventType::Type      d_event;       // READ/WRITE/ACCEPT/CONNECT
       int                        d_timeout;     // timeout in milliseconds
    };

private:
    mutable bsl::vector<OperationDetails> d_parameters;

    // not implemented
    HelperEventManager(const HelperEventManager&);
    HelperEventManager& operator==(const HelperEventManager&);

  public:
    // CREATORS
    HelperEventManager(bslma_Allocator *basicAllocator = 0);
        // Create this 'HelperEventManager' object.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator'
        // is 0, global operators 'new' and 'delete' are used.

    ~HelperEventManager();
        // Destroy this object.

    // MANIPULATORS
    virtual int dispatch(const bdet_TimeInterval& timeout,
                         int                      flags);
        // Dummy dispatch: record this operation details such as parameters
        // which is used by this dispatch function.  Return 1 for simulating
        // one dispatched event.

    virtual int dispatch(int flags);
        // Dummy dispatch: record this operation details such as parameters
        // which is used by this dispatch function.  Return 1 for simulating
        // one dispatched event.

    virtual int registerSocketEvent(
                            const bteso_SocketHandle::Handle&   handle,
                            const bteso_EventType::Type         event,
                            const bteso_EventManager::Callback& callback);
        // Dummy function: record this operation details such as parameters
        // which is used by this function.  Return 0 on success and nonzero
        // otherwise.

    virtual void deregisterSocketEvent(
                               const bteso_SocketHandle::Handle& handle,
                               bteso_EventType::Type             event);
        // Dummy function: record this operation details such as parameters
        // which is used by this function.

    virtual int deregisterSocket(const bteso_SocketHandle::Handle& handle);
        // Dummy function: record this operation details such as parameters
        // which is used by this function.

    virtual void deregisterAll();
        // Dummy function: record this operation details such as parameters
        // which is used by this function.

    void clearOpDetails();
        // Remove all parameter data saved for the previous function calls.

    // ACCESSORS
    virtual bool canRegisterSockets() const;
        // Return 'true' if this event manager can register additional sockets,
        // and 'false' otherwise.

    virtual bool hasLimitedSocketCapacity() const;
        // Return 'true' if this event manager has limited socket capacity, and
        // 'false' otherwise.

    virtual int isRegistered(const bteso_SocketHandle::Handle& handle,
                             const bteso_EventType::Type       event) const;
        // Dummy function: record this operation details such as parameters
        // which is used by this function.

    virtual int numEvents() const;
        // Dummy function: record this operation details such as parameters
        // which is used by this function.

    virtual int numSocketEvents(
                              const bteso_SocketHandle::Handle& handle) const;
        // Dummy function: record this operation details such as parameters
        // which is used by this function.

    const bsl::vector<OperationDetails>& opDetails() const;
        // Return the array of functions called and parameters used to call
        // functions under test.
};

HelperEventManager::HelperEventManager(bslma_Allocator *basicAllocator)
: d_parameters(basicAllocator)
{
}

HelperEventManager::~HelperEventManager()
{
}

                             // ------------
                             // MANIPULATORS
                             // ------------

int HelperEventManager::dispatch(const bdet_TimeInterval& deadline,
                                 int                      flags)
{
    OperationDetails info;
    info.d_functionCode = DISPATCHTIMEOUT;
    info.d_handle = UNSET;
    bdet_TimeInterval timeLimit = deadline - bdetu_SystemTime::now();
    info.d_timeout =
             (int) (timeLimit.seconds() * 1E3 + timeLimit.nanoseconds() / 1E6);
    d_parameters.push_back(info);
    return 1;
}

int HelperEventManager::dispatch(int flags)
{
    OperationDetails info;
    info.d_functionCode = DISPATCH;
    info.d_handle = UNSET;
    info.d_timeout = UNSET;
    d_parameters.push_back(info);
    return 1;
}

int HelperEventManager::registerSocketEvent(
                  const bteso_SocketHandle::Handle&   socketHandle,
                  const bteso_EventType::Type         event,
                  const bteso_EventManager::Callback& callback)
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
            const bteso_SocketHandle::Handle& socketHandle,
            const bteso_EventType::Type       event)
{
    OperationDetails info;
    info.d_handle = socketHandle;
    info.d_event = event;
    info.d_functionCode = DEREGISTERSOCKETEVENT;
    info.d_timeout = UNSET;
    d_parameters.push_back(info);
}

int HelperEventManager::deregisterSocket(
        const bteso_SocketHandle::Handle& socketHandle)
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

                             // ---------
                             // ACCESSORS
                             // ---------

bool HelperEventManager::canRegisterSockets() const
{
    return true;
}

bool HelperEventManager::hasLimitedSocketCapacity() const
{
    return false;
}

int HelperEventManager::numSocketEvents(
                        const bteso_SocketHandle::Handle& socketHandle) const
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
                        const bteso_SocketHandle::Handle& handle,
                        const bteso_EventType::Type       event) const
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
    int fd(-1), nt(0);
    int rc(FAIL);
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
              return FAIL;
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
              return FAIL;
          }
          info.d_handle = fds[fd].observedFd();

          char d = script[2];
          switch (d) {
            case 'r':
            case 'w':
              rc = sscanf(script + 3, "%d", &bytes);
              if (1 == rc && 0 >= bytes) {
                  return FAIL;
              }
              if (1 != rc) {
                  bytes = -1;
              }
              break;
              case 'a':
              case 'c':
                break;
              default: {
                return FAIL;
              }
          }
          switch (d) {
            case 'r':
              info.d_event = bteso_EventType::BTESO_READ;
              break;

            case 'w':
              info.d_event = bteso_EventType::BTESO_WRITE;
              break;

            case 'a':
              info.d_event = bteso_EventType::BTESO_ACCEPT;
              break;

            case 'c':
              info.d_event = bteso_EventType::BTESO_CONNECT;
              break;
            default:
              return FAIL;
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
                  return FAIL;
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
                info.d_event = bteso_EventType::BTESO_READ;
            } break;
            case 'w': {
                info.d_event = bteso_EventType::BTESO_WRITE;
            } break;
            case 'a': {
                info.d_event = bteso_EventType::BTESO_ACCEPT;
            } break;
            case 'c': {
                info.d_event = bteso_EventType::BTESO_CONNECT;
            } break;
            default:
                return FAIL;
          }
          opDetails->push_back(info);
      } break;
      case 'D': {
          int msecs(0), rc(-1), nbytes(0);
          char ch;
          // int flags = 0;  // not used
          bdet_TimeInterval deadline(bdetu_SystemTime::now());
          enum { SLEEP_TIME = 200000 };
          bcemt_ThreadUtil::microSleep(SLEEP_TIME);

          HelperEventManager::OperationDetails info;
          info.d_handle = HelperEventManager::UNSET;
          info.d_timeout = HelperEventManager::UNSET;

          if (3 == sscanf(script, "D%c%d,%d%n", &ch, &msecs, &rc, &nbytes)) {
              if (0 > msecs || 0 > rc) {
                  return FAIL;
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
                    return FAIL;
              }
          }
          else if (2 == sscanf(script, "D%c,%u%n", &ch, (unsigned *)&rc,
                                                                    &nbytes)) {
              if (0 > rc) {
                  return FAIL;
              }
              switch (ch) {
                case 'n': {
                    // flags = 0;
                } break;
                case 'i': {
                    // flags = 0;
                } break;
                default:
                    return FAIL;
              }
              info.d_functionCode = HelperEventManager::DISPATCH;
          }
          else {
              return FAIL;
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
                  info.d_event = bteso_EventType::BTESO_ACCEPT;
                  info.d_timeout = HelperEventManager::UNSET;
                  opDetails->push_back(info);
              }
              if (strchr(buf, 'c')) {
                  HelperEventManager::OperationDetails info;
                  info.d_functionCode = HelperEventManager::ISREGISTERED;
                  info.d_handle = fds[fd].observedFd();
                  info.d_event = bteso_EventType::BTESO_CONNECT;
                  info.d_timeout = HelperEventManager::UNSET;
                  opDetails->push_back(info);
              }
              if (strchr(buf, 'r')) {
                  HelperEventManager::OperationDetails info;
                  info.d_functionCode = HelperEventManager::ISREGISTERED;
                  info.d_handle = fds[fd].observedFd();
                  info.d_event = bteso_EventType::BTESO_READ;
                  info.d_timeout = HelperEventManager::UNSET;
                  opDetails->push_back(info);
              }
              if (strchr(buf, 'w')) {
                  HelperEventManager::OperationDetails info;
                  info.d_functionCode = HelperEventManager::ISREGISTERED;
                  info.d_handle = fds[fd].observedFd();
                  info.d_event = bteso_EventType::BTESO_WRITE;
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
        break;

      default:
          return FAIL;
      }
      while (' ' != *script && '{' != *script && '}' != *script &&
             ';' != *script && '\t' != *script) {
          // Go to next command.
          if ('\0' == *script) {
              return SUCCESS;
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
    bteso_SocketImpUtil::startup(&errCode);
    ASSERT(0 == errCode);
    bslma_TestAllocator testAllocator(veryVeryVerbose);
    testAllocator.setNoAbort(1);

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // ------------------------------------------------------------------
        // TESTING bteso_EventManagerTestPair
        // Concerns:
        //   o a pair can be created with different verbose modes
        //   o accessors work as expected
        //   o able to set/get buffer sizes and watermarks
        // Plan:
        //   Create a socket pair and address the concerns
        // Testing:
        //   bteso_EventManagerTestPair
        // ------------------------------------------------------------------
        if (verbose) cout << "TESTING bteso_EventManagerTestPair." << endl
                          << "===================================" << endl;
        bteso_EventManagerTestPair mX(veryVerbose);
        const bteso_EventManagerTestPair& X = mX;
        ASSERT(mX.isValid());
#if defined(BTESO_PLATFORM__BSD_SOCKETS)
        ASSERT(0 < X.observedFd());         ASSERT(0 < X.controlFd());
#endif
#if defined(BTESO_PLATFORM__WIN_SOCKETS)
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
        // First create a bteso_TimeMetrics object 'tm' and an event manager
        // object under test 'mX'.

        // bteso_EventManagerImp_Poll mX();

        // Then call a function to be tested in the event manager.
        // Notice that only one example is given here, the same usage for
        // other functions.

        // int ctrlFlag = 0;
        // if (veryVeryVerbose) {
        //    ctrlFlag |= bteso_EventManagerTester::BTESO_VERY_VERY_VERBOSE;
        //}
        //else if (veryVerbose) {
        //    ctrlFlag |= bteso_EventManagerTester::BTESO_VERY_VERBOSE;
        //}
        //else if (verbose) {
        //    ctrlFlag |= bteso_EventManagerTester::BTESO_VERBOSE;
        //}
        //bteso_EventManagerTester::testRegisterSocketEvent(&mX, ctrlFlag);
        // The following snippets of code illustrate how to use this utility
        // component to perform a 'customized' test.
        // First an array of sockets that will be used
        // in the test script is given as 'fds' and connection(s) have been
        // created.

        typedef bteso_EventManagerTestPair SocketPair;
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
                ctrlFlag |= bteso_EventManagerTester::BTESO_VERY_VERY_VERBOSE;
        }
        else if (veryVerbose) {
            ctrlFlag |= bteso_EventManagerTester::BTESO_VERY_VERBOSE;
        }
        else if (verbose) {
            ctrlFlag |= bteso_EventManagerTester::BTESO_VERBOSE;
        }
        int fails = bteso_EventManagerTester::gg(&mX2, socketPairs,
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

        double start   = bdetu_SystemTime::now().totalSecondsAsDouble();
        int fails = bteso_EventManagerTester::gg(0, 0, "S100; S150", 0);
        double elapsed = bdetu_SystemTime::now().totalSecondsAsDouble() -
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
                fails = bteso_EventManagerTester::gg(&mX, socketPairs,
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
                LOOP_ASSERT(i, len == details.size());

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
                fails = bteso_EventManagerTester::gg(&mX, socketPairs,
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
                LOOP_ASSERT(i, len == details.size());

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
                fails = bteso_EventManagerTester::gg(&mX, socketPairs,
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
                LOOP_ASSERT(i, len == details.size());

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
                fails = bteso_EventManagerTester::gg(&mX, socketPairs,
                                                     SCRIPTS[i].d_script,
                                                     ctrlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                bsl::vector<HelperEventManager::OperationDetails>
                                                   opDetails(&testAllocator);
                buildOpDetails(SCRIPTS[i].d_script, socketPairs, &opDetails);
                const bsl::vector<HelperEventManager::OperationDetails>&
                    details = mX.opDetails();
                int len = opDetails.size();
                LOOP_ASSERT(i, len == details.size());

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
                fails = bteso_EventManagerTester::gg(&mX, socketPairs,
                                                     SCRIPTS[i].d_script,
                                                     ctrlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                bsl::vector<HelperEventManager::OperationDetails>
                                                 opDetails(&testAllocator);
                buildOpDetails(SCRIPTS[i].d_script, socketPairs, &opDetails);
                const bsl::vector<HelperEventManager::OperationDetails>&
                    details = mX.opDetails();
                int len = opDetails.size();
                LOOP_ASSERT(i, len == details.size());

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
                fails = bteso_EventManagerTester::gg(&mX, socketPairs,
                                                     SCRIPTS[i].d_script,
                                                     ctrlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                bsl::vector<HelperEventManager::OperationDetails>
                                                 opDetails(&testAllocator);
                buildOpDetails(SCRIPTS[i].d_script, socketPairs, &opDetails);
                const bsl::vector<HelperEventManager::OperationDetails>&
                    details = mX.opDetails();
                int len = opDetails.size();
                LOOP_ASSERT(i, len == details.size());

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
                fails = bteso_EventManagerTester::gg(&mX, socketPairs,
                                                     SCRIPTS[i].d_script,
                                                     ctrlFlag);

                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                bsl::vector<HelperEventManager::OperationDetails>
                                                  opDetails(&testAllocator);
                buildOpDetails(SCRIPTS[i].d_script, socketPairs, &opDetails);
                const bsl::vector<HelperEventManager::OperationDetails>&
                    details = mX.opDetails();
                int len = opDetails.size();
                LOOP_ASSERT(i, len == details.size());

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
               {L_, 0, "W0,30; R0,24"},
               {L_, 0, "Di,1; Dn,1;  Di150,1; Dn400,1"},
               {L_, 0, "T0; +0w21; W1,20; +1r11"},
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
                fails = bteso_EventManagerTester::gg(&mX, socketPairs,
                                                     SCRIPTS[i].d_script,
                                                     ctrlFlag);
                LOOP_ASSERT(LINE, SCRIPTS[i].d_fails == fails);

                bsl::vector<HelperEventManager::OperationDetails>
                                                 opDetails(&testAllocator);
                buildOpDetails(SCRIPTS[i].d_script, socketPairs, &opDetails);
                const bsl::vector<HelperEventManager::OperationDetails>&
                    details = mX.opDetails();
                int len = opDetails.size();
                LOOP_ASSERT(i, len == details.size());

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
