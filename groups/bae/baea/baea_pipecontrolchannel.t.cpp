// baea_pipecontrolchannel.t.cpp   -*-C++-*-
#include <baea_pipecontrolchannel.h>

#include <bael_defaultobserver.h>
#include <bael_log.h>
#include <bael_loggermanager.h>
#include <bael_loggermanagerconfiguration.h>
#include <bael_severity.h>

#include <bcemt_barrier.h>
#include <bcemt_threadgroup.h>
#include <bces_atomictypes.h>

#include <bdef_bind.h>
#include <bdesu_pipeutil.h>

#include <bsls_assert.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_fstream.h>

#ifdef BSLS_PLATFORM__OS_UNIX
#include <bsl_c_signal.h>
#include <bsl_c_stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test is a mechanism that reads data from a named pipe
// and invokes a user-supplied callback function with that data.  This test
// driver verifies the implementation of by ensuring the component may be
// started, stopped, and shutdown, and also by ensuring that any data written
// to the named pipe is supplied as an argument to the user-defined callback
// function.
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] explicit
//      baea_PipeControlChannel(const ControlCallback&  callback,
//                              bslma_Allocator        *basicAllocator = 0);
//
// [ 4] ~baea_PipeControlChannel();
//
// MANIPULATORS
// [ 2] int start(const bsl::string& pipeName);
// [ 2] void shutdown();
// [ 2] void stop();
//
// ACCESSORS
// [ 1] const bsl::string& pipeName() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TESTING CONCERN: THE COMPONENT MAY BE RESTARTED
// [ 3] TESTING CONCERN: 'shutdown' IS IDEMPOTENT
// [ 4] TESTING CONCERN: THE DESTRUCTOR INVOKES 'shutdown'
// [ 5] TESTING CONCERN: DATA SENT BY A CLIENT IS READ FROM THE PIPE
// [ 6] TESTING CONCERN: CONCURRENT WRITES
// [ 7] TESTING USAGE EXAMPLE

//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100)  ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\t" \
                    << #M << ": " << M << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number.
#define T_()  cout << '\t' << flush;          // Print tab w/o newline.

//=============================================================================
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

extern "C"
{
void noopHandler (int)
{
}
}

namespace {

const char LOG_CATEGORY[] = "BAEA.PCC.TEST";

                       // ===================
                       // class ControlServer
                       // ===================

class ControlServer {

    // DATA
    baea_PipeControlChannel  d_channel;
    bsl::vector<bsl::string> d_messages;

    // PRIVATE MANIPULATORS
    void onMessage(const bdeut_StringRef& message)
    {
        if ("EXIT" != message) {
            d_messages.push_back(message);
        }
        else {
            shutdown();
        }
    }

  public:
    // CREATORS
    explicit ControlServer(bslma_Allocator *basicAllocator = 0)
        : d_channel(bdef_BindUtil::bind(&ControlServer::onMessage,
                                        this,
                                        bdef_PlaceHolders::_1),
                    basicAllocator)
        , d_messages(basicAllocator)
    {}

    // MANIPULATORS
    int start(const bdeut_StringRef &pipeName) {
        return d_channel.start(pipeName);
    }

    void shutdown() {
        d_channel.shutdown();
    }

    void stop() {
        d_channel.stop();
    }

    // ACCESSORS
    int numMessages() const {
        return d_messages.size();
    }

    const bsl::string& message(int index) const {
        return d_messages[index];
    }
};

static void threadSend(const bsl::string& pipeName,
                       const bsl::string& message,
                       int                numIterations)
{
    for (int i = 0; i < numIterations; ++i) {
        bdesu_PipeUtil::send(pipeName.c_str(), message);
    }
}

void noop(const bdeut_StringRef&)
{
}

void verifyPayload(const bsl::string&      expected,
                   const bdeut_StringRef&  found,
                   bcemt_Barrier          *barrier)
{
    // Verify that the specified 'found' payload has the same value as the
    // specified 'expected' payload.

    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);

    BAEL_LOG_TRACE << "Control callback received '" << found << "'"
                   << BAEL_LOG_END;

    ASSERT(expected == found);
    barrier->wait();
}

void loadData(bsl::string *result, int length)
{
    const char DATA[] = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
                        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
    enum { DATA_SIZE = sizeof DATA - 1 };

    int capacity = length;
    result->clear();
    result->reserve(capacity);
    do {
        int nbytes = (capacity >= DATA_SIZE) ? DATA_SIZE : capacity;
        result->append(DATA, nbytes);
        capacity -= nbytes;
    } while (capacity > 0);
}

}  // close unnamed namespace

extern "C"
void onSigPipe(int) {
    BAEL_LOG_SET_CATEGORY(LOG_CATEGORY);
    BAEL_LOG_WARN << "SIGPIPE received" << BAEL_LOG_END;
}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = (argc > 1) ? bsl::atoi(argv[1]) : 1;

    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

#ifdef BSLS_PLATFORM__OS_UNIX
    ofstream devnull;
    if (9 == test) {
        devnull.open("/dev/null");
    }

    bael_DefaultObserver observer(9 == test ? devnull
                                            : test < 0 ? bsl::cerr
                                                       : bsl::cout);
#else
    bael_DefaultObserver observer(test < 0 ? bsl::cerr : bsl::cout);
#endif
    bael_LoggerManagerConfiguration configuration;
    bael_LoggerManager::initSingleton(&observer, configuration);

    if (test != -10) {
        bael_Severity::Level passthrough = bael_Severity::BAEL_OFF;
        if (verbose) passthrough = bael_Severity::BAEL_WARN;
        if (veryVerbose) passthrough = bael_Severity::BAEL_INFO;
        if (veryVeryVerbose) passthrough = bael_Severity::BAEL_TRACE;

        bael_LoggerManager::singleton().setDefaultThresholdLevels(
                                                      bael_Severity::BAEL_OFF,
                                                      passthrough,
                                                      bael_Severity::BAEL_OFF,
                                                      bael_Severity::BAEL_OFF);
    }

    if (test >= 0) {
        cout << "TEST " << __FILE__ << " CASE " << test << endl;
    }

#ifndef BSLS_PLATFORM__OS_WINDOWS
    sigset(SIGPIPE, onSigPipe);
#endif

    switch (test) { case 0:
      case -10: {
        // --------------------------------------------------------------------
        // Child process for case 10
        // --------------------------------------------------------------------
#ifdef BSLS_PLATFORM__OS_WINDOWS
        cout << "Skipping case -10 on windows..." << endl;
#else
        if (argc != 5 ||
            (argv[2][0] != 'Y' &&
             argv[2][0] != 'Z' &&
             argv[2][0] != 'A'))
        {
            cout << "Case -10 is for internal use only.  Go away!" << endl;
            exit(99);
        }
        veryVeryVerbose = argv[2][0] == 'A';
        verbose = veryVeryVerbose || argv[2][0] == 'Z';

        bael_Severity::Level passthrough = bael_Severity::BAEL_OFF;
        if (verbose) passthrough = bael_Severity::BAEL_WARN;
        if (veryVeryVerbose) passthrough = bael_Severity::BAEL_TRACE;

        bael_LoggerManager::singleton().setDefaultThresholdLevels(
                                                      bael_Severity::BAEL_OFF,
                                                      passthrough,
                                                      bael_Severity::BAEL_OFF,
                                                      bael_Severity::BAEL_OFF);

        cout << "P" << getpid() << "\n" << flush;

        signal(SIGINT, &noopHandler);

        if (verbose) {
            cerr << "Case 10 client process starting" << endl;
        }
        unlink(argv[3]);

        bcemt_Barrier barrier(2);
        baea_PipeControlChannel channel(bdef_BindUtil::bind(
                                                         &verifyPayload,
                                                         bsl::string(argv[4]),
                                                         bdef_PlaceHolders::_1,
                                                         &barrier));

        int rc = channel.start(argv[3]);
        ASSERT(0 == rc);

        sigset_t intset;
        sigemptyset(&intset);
        sigaddset(&intset, SIGINT);
        pthread_sigmask(SIG_BLOCK, &intset, 0);

        barrier.wait();

        if (verbose) {
            cerr << "Case 10 client process stopping" << endl;
        }

        channel.shutdown();
        channel.stop();
#endif
      } break;
      case 10: {
#ifdef BSLS_PLATFORM__OS_WINDOWS
        cout << "Skipping case 10 on windows..." << endl;
#else
        if (verbose) {
            cout << "EINTR test" << endl
                 << "==========" << endl;
        }

        bsl::string pipeName;

        ASSERT(0 == bdesu_PipeUtil::makeCanonicalName
               (&pipeName, "ctrl.pcctest10"));

        char buffer[512];
        char verb = veryVeryVerbose ? 'A' :
                          verbose ? 'Z' : 'Y';

        snprintf(buffer, 512, "%s -10 %c %s QUICKBROWNFOX_CASE10",
                 argv[0], verb, pipeName.c_str());

        FILE* childFD = popen(buffer, "r");
        BSLS_ASSERT(0 != childFD); // test invariant

        do {
            fscanf(childFD, "%s", buffer);
        } while (buffer[0] == '\n');

        int childPid = 0;

        if (buffer[0] == 'P') {
            childPid = atoi(&buffer[1]);
        }
        if (0 == childPid) {
            cout << "Communication error with child process!!" << endl;
            cout << "...(received \"" << buffer << "\"" << endl;
            exit(9);
        }
        else if (verbose) {
            cout << "Child pid: " << childPid << endl;
        }

        bcemt_ThreadUtil::microSleep(10000);
        kill(childPid, SIGINT);
        bcemt_ThreadUtil::microSleep(100000);

        if (verbose) {
            cout << "Sending QUICKBROWNFOX_CASE10 to " << pipeName
                 << endl;
        }

        ASSERT(0 == bdesu_PipeUtil::send(pipeName, "QUICKBROWNFOX_CASE10\n"));
        ASSERT(0 == pclose(childFD));

#endif
      } break;

      case -9: {
        enum { NUM_CLIENTS = 2, NUM_ITERATIONS = 500 };

        if (argc != 4 ||
            (argv[2][0] != 'Y' &&
             argv[2][0] != 'Z'))
        {
            cout << "Case -9 is for internal use only.  Go away!" << endl;
            exit(99);
        }
        verbose = argv[2][0] == 'Z';

        if (verbose) {
            cerr << "Case 9 client process starting" << endl;
        }

#ifdef BSLS_PLATFORM__OS_UNIX
        unlink(argv[3]);
#endif

        const char MESSAGE[]
             = "Hello, world! The sick cat couldn't jump over even a lazy dog";
        bcemt_Barrier barrier(2);

        bsl::string message(MESSAGE);
        baea_PipeControlChannel channel(bdef_BindUtil::bind(
                                             &verifyPayload,
                                             message,
                                             bdef_PlaceHolders::_1,
                                             &barrier));

        ASSERT(0 == channel.start(argv[3]));

        if (verbose) {
            cerr << "\tCase 9 client: channel started, sending A..."
                 << endl;
        }
        cout << "A\n" << flush;

        bcemt_ThreadGroup threadGroup;
        threadGroup.addThreads(bdef_BindUtil::bind(
                                                   &threadSend,
                                                   bsl::string(argv[3]),
                                                   message + "\n",
                                                   (int)NUM_ITERATIONS),
                               NUM_CLIENTS);

        for (int i = 0; i < NUM_CLIENTS * NUM_ITERATIONS; ++i) {
            barrier.wait();
        }

        threadGroup.joinAll();

        if (verbose) {
            cerr << "\tCase 9 client: sleeping..." << endl;
        }

        bcemt_ThreadUtil::sleep(bdet_TimeInterval(2.0));

        if (verbose) {
            cerr << "\tCase 9 client: sending B and shutting down..." << endl;
        }

        cout << "B\n" << flush;

        channel.shutdown();
        channel.stop();

        bcemt_ThreadUtil::sleep(bdet_TimeInterval(1.0));

        break;
      }
      case 9: {
        // --------------------------------------------------------------------
        // TESTING PIPE-IN-USE SAFETY
        //
        // Concern: That if a process opens a PipeControlChannel, another one
        // cannot open the same pipe or interfere with its operation.
        //
        //---------------------------------------------------------------------
#ifdef BSLS_PLATFORM__OS_WINDOWS
        cout << "Skipping case 9 on windows..." << endl;
#else
        if (verbose) {
            cout << "Pipe-in-use safety test" << endl
                 << "=======================" << endl;
        }

        bsl::string pipeName;

        ASSERT(0 == bdesu_PipeUtil::makeCanonicalName
               (&pipeName, "ctrl.safttest9"));

        char buffer[512];
        snprintf (buffer, 512, "%s -9 %c %s", argv[0],
                  verbose? 'Z' : 'Y', pipeName.c_str());
        FILE* childFD = popen(buffer, "r");
        BSLS_ASSERT(0 != childFD); // test invariant
        do {
            fgets(buffer, 3, childFD);
        } while (buffer[0] == '\n');

        if (buffer[0] != 'A') {
            cout << "Communication error with child process!!" << endl;
            cout << "...(received \"" << buffer << "\"" << endl;
            exit(9);
        }
        ControlServer server;

        int rc;
        for (int i = 0; i < 5; ++i) {
            rc = server.start(pipeName);
            LOOP_ASSERT(i, 0 != rc);
            bcemt_ThreadUtil::microSleep(100);
        }

        do {
            fgets(buffer, 50, childFD);
        } while (buffer[0] == '\n');
        if (buffer[0] != 'B') {
            cout << "Communication error with child process!!" << endl;
            cout << "...(received \"" << buffer << "\"" << endl;
            exit(9);
        }
        pclose(childFD);
#endif
      } break;

      case -8: {
        if (argc != 4 ||
            (argv[2][0] != 'Y' &&
             argv[2][0] != 'Z'))
        {
            cout << "Case -8 is for internal use only.  Go away!" << endl;
            exit(99);
        }
        verbose = argv[2][0] == 'Z';

        if (verbose) {
            cerr << "Case 8 client process starting" << endl;
        }

#ifdef BSLS_PLATFORM__OS_UNIX
        unlink(argv[3]);
#endif

        baea_PipeControlChannel *channel = new baea_PipeControlChannel(&noop);
        ASSERT(0 == channel->start(argv[3]));

        if (verbose) {
            cerr << "\tCase 8 client: channel started, seg faulting..."
                 << endl;
        }
        cout << "A\n" << flush;
        *(int*)10 = 10;

        ASSERT(!"unreachable");
        break;
      }
      case 8: {
        // --------------------------------------------------------------------
        // TESTING CRASH RECOVERY
        //
        // Concern: That if a process opens a PipeControlChannel and
        // subsequently crashes, another process can then open the
        // same named pipe.
        //---------------------------------------------------------------------
#ifdef BSLS_PLATFORM__OS_WINDOWS
        cout << "Skipping case 8 on windows..." << endl;
#else
        if (verbose) {
            cout << "Crash recovery test: \"Bus Error\" is OK" << endl
                 << "======================================" << endl;
        }

        bsl::string pipeName;

        ASSERT(0 == bdesu_PipeUtil::makeCanonicalName
               (&pipeName, "ctrl.restarttest8"));

        char buffer[512];
        snprintf (buffer, 512, "%s -8 %c %s", argv[0],
                  verbose? 'Z' : 'Y', pipeName.c_str());
        FILE* childFD = popen(buffer, "r");
        BSLS_ASSERT(0 != childFD); // test invariant
        do {
            fgets(buffer, 3, childFD);
        } while (buffer[0] == '\n');
        if (buffer[0] != 'A') {
            cout << "Communication error with child process!!" << endl;
            cout << "...(received \"" << buffer << "\"" << endl;
            exit(9);
        }
        bcemt_ThreadUtil::sleep(bdet_TimeInterval(1.0));

        ControlServer server;

        int rc = server.start(pipeName);
        ASSERT(0 == rc && "Failed to start pipe control channel");

        if (0 == rc) {
            const char MSG0[]  = "this is the first message";
            const char MSG1[]  = "this is the second message";

            rc = bdesu_PipeUtil::send(pipeName, bsl::string(MSG0) + "\n");
            ASSERT(0 == rc);
            rc = bdesu_PipeUtil::send(pipeName, bsl::string(MSG1) + "\n");
            ASSERT(0 == rc);
            rc = bdesu_PipeUtil::send(pipeName, "EXIT\n");
            ASSERT(0 == rc);

            server.stop();  // blocks until shutdown

            ASSERT(2 == server.numMessages());
            ASSERT(bsl::string(MSG0) == server.message(0));
            ASSERT(bsl::string(MSG1) == server.message(1));
        }
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver.  Additionally, replace all calls to 'BSLS_ASSERT' in
        //   the usage example with calls to 'ASSERT'.  This now becomes the
        //   source, which is then "copied" back to the header file by
        //   reversing the above process.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) {
            cout << "Usage Example" << endl
                 << "=============" << endl;
        }

        bslma_TestAllocator ta(veryVeryVeryVerbose);
        {
            bsl::string pipeName;

            ControlServer server(&ta);

            ASSERT(0 == bdesu_PipeUtil::makeCanonicalName
                   (&pipeName, "ctrl.baea.pcctest7"));
#ifdef BSLS_PLATFORM__OS_UNIX
            unlink(pipeName.c_str());
#endif

            int rc = server.start(pipeName);
            ASSERT(0 == rc && "Failed to start pipe control channel");

            const char MSG0[]  = "this is the first message";
            const char MSG1[]  = "this is the second message";

            rc = bdesu_PipeUtil::send(pipeName, bsl::string(MSG0) + "\n");
            ASSERT(0 == rc);
            rc = bdesu_PipeUtil::send(pipeName, bsl::string(MSG1) + "\n");
            ASSERT(0 == rc);
            rc = bdesu_PipeUtil::send(pipeName, "EXIT\n");
            ASSERT(0 == rc);

            server.stop();  // blocks until shutdown

            ASSERT(2 == server.numMessages());
            ASSERT(bsl::string(MSG0) == server.message(0));
            ASSERT(bsl::string(MSG1) == server.message(1));

        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: CONCURRENT WRITES
        //
        // Concerns:
        //   When multiple clients concurrent write to the pipe each write
        //   occurs "atomically", i.e., the data in a message is not
        //   interleaved with the data from any other message.
        //
        // Plan:
        //    Create a pipe control channel and start the channel processing
        //    a named pipe.  Start a group of threads that write a payload to
        //    the named pipe, and verify the pipe control channel's control
        //    callback is invoked the expected number of times with an argument
        //    having the same value as the payload written to the pipe.
        //
        // Testing:
        //   Concurrent writes
        // --------------------------------------------------------------------
#ifdef BSLS_PLATFORM__OS_WINDOWS
        cout << "Skipping case 6 on windows..." << endl;
#else
        if (verbose) {
            cout << "TESTING CONCERN: CONCURRENT WRITES" << endl
                 << "==================================" << endl;
        }

        bslma_TestAllocator ta(veryVeryVeryVerbose);
        {
            bsl::string pipeName;

            ASSERT(0 == bdesu_PipeUtil::makeCanonicalName
                   (&pipeName, "ctrl.baea.pcctest6"));
#ifdef BSLS_PLATFORM__OS_UNIX
            unlink(pipeName.c_str());
#endif
            enum { NUM_CLIENTS = 10, NUM_ITERATIONS = 100 };

            int DATA[] = {
                1, 32, 256
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int iteration = 0; iteration < NUM_DATA; ++iteration) {
                int messageLength = DATA[iteration];

                bsl::string message;
                loadData(&message, messageLength);

                bcemt_Barrier  barrier(2);
                bces_AtomicInt numWrites = 0;

                baea_PipeControlChannel channel(bdef_BindUtil::bind(
                                                         &verifyPayload,
                                                         message,
                                                         bdef_PlaceHolders::_1,
                                                         &barrier),
                                                 &ta);

                int rc = channel.start(pipeName);
                ASSERT(0 == rc);

                bcemt_ThreadGroup threadGroup;
                threadGroup.addThreads(bdef_BindUtil::bind(
                                                        &threadSend,
                                                        bsl::string(pipeName),
                                                        message + "\n",
                                                        (int)NUM_ITERATIONS),
                                       NUM_CLIENTS);

                for (int i = 0; i < NUM_CLIENTS * NUM_ITERATIONS; ++i) {
                    barrier.wait();
                }

                threadGroup.joinAll();

                channel.shutdown();
                channel.stop();
            }
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: DATA SENT BY A CLIENT IS READ FROM THE PIPE
        //
        // Concerns:
        //   Data sent through the named pipe by a client is read by the object
        //   and specified as the argument to the control callback.
        //
        // Plan:
        //    Create a pipe control channel and start the channel processing
        //    a named pipe.  Write a payload to the named pipe, and verify the
        //    pipe control channel's control callback is invoked with an
        //    argument having the same value as the payload written to the
        //    pipe.
        //
        // Testing:
        //   Reading data from the named pipe
        // --------------------------------------------------------------------
#ifdef BSLS_PLATFORM__OS_WINDOWS
        cout << "Skipping case 5 on windows..." << endl;
#else
        if (verbose) {
            cout << "TESTING CONCERN: READING DATA FROM NAMED PIPE" << endl
                 << "=============================================" << endl;
        }

        bslma_TestAllocator ta(veryVeryVeryVerbose);
        {
            bsl::string pipeName;

            ASSERT(0 == bdesu_PipeUtil::makeCanonicalName
                   (&pipeName, "ctrl.baea.pcctest5"));
#ifdef BSLS_PLATFORM__OS_UNIX
            unlink(pipeName.c_str());
#endif

            const char MESSAGE[]  = "Hello, world!";

            bcemt_Barrier barrier(2);

            baea_PipeControlChannel channel(bdef_BindUtil::bind(
                                                         &verifyPayload,
                                                         bsl::string(MESSAGE),
                                                         bdef_PlaceHolders::_1,
                                                         &barrier),
                                             &ta);

            int rc = channel.start(pipeName);
            ASSERT(0 == rc);

            bdesu_PipeUtil::send(pipeName, bsl::string(MESSAGE) + "\n");
            barrier.wait();

            channel.shutdown();
            channel.stop();
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: THE DESTRUCTOR INVOKES 'shutdown'
        //
        // Concerns:
        //   Destroying the object shuts down the object and joins the
        //   background thread.
        //
        // Plan:
        //   Create a pipe control channel and start processing a name pipe.
        //   Do no explicitly shut down the pipe control channel.  Destroy the
        //   object, and verify the thread destroying the object is eventually
        //   unblocked.
        //
        // Testing:
        //   ~PipeControlChannel()
        // --------------------------------------------------------------------
        if (verbose) {
            cout << "TESTING CONCERN: DESTRUCTOR INVOKES 'shutdown'" << endl
                 << "=============================================" << endl;
        }

        bslma_TestAllocator ta(veryVeryVeryVerbose);
        {
            bsl::string pipeName;

            ASSERT(0 == bdesu_PipeUtil::makeCanonicalName
                   (&pipeName, "ctrl.baea.pcctest4"));
#ifdef BSLS_PLATFORM__OS_UNIX
            unlink(pipeName.c_str());
#endif

            {
            baea_PipeControlChannel channel(&noop, &ta);

            int rc = channel.start(pipeName);
            ASSERT(0 == rc);
            }

            ASSERT("The calling thread is unblocked.");
        }
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: 'shutdown' IS IDEMPOTENT
        //
        // Concerns:
        //   The 'shutdown' function is idempotent.
        //
        // Plan:
        //   Start the pipe control channel then explicitly call shutdown and
        //   then stop, validating the result.  Explicitly call shutdown and
        //   then stop again.
        //
        // Testing:
        //   shutdown
        // --------------------------------------------------------------------
        if (verbose) {
            cout << "TESTING CONCERN: 'shutdown' IS IDEMPOTENT" << endl
                 << "=========================================" << endl;
        }

        bslma_TestAllocator ta(veryVeryVeryVerbose);
        {
#ifdef BSLS_PLATFORM__OS_WINDOWS
            const char PIPE_NAME[] = "\\\\.\\pipe\\ctrl.baea.pcctest3";
#else
            const char PIPE_NAME[] = "/tmp/ctrl.baea.pcctest3";
            unlink(PIPE_NAME);
#endif

            baea_PipeControlChannel channel(&noop, &ta);

            int rc = channel.start(PIPE_NAME);
            ASSERT(0 == rc);

            channel.shutdown();
            channel.shutdown();
            channel.stop();

            channel.shutdown();
            channel.shutdown();
            channel.stop();
        }
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: THE COMPONENT MAY BE RESTARTED
        //
        // Concerns:
        //   The pipe control channel may be restarted after it has been
        //   shutdown and stopped.
        //
        // Plan:
        //   Create a pipe control channel.  Start, shutdown, and stop the
        //   object.  Restart the pipe control channel with a different name
        //   and verify the result.  Stop the pipe control channel, then
        //   restart the object with the same name as the previous iteration
        //   and verify the result.
        //
        // Testing:
        //   run, shutdown, stop
        // --------------------------------------------------------------------
        if (verbose) {
            cout << "TESTING CONCERN: The Component May Be Restarted" << endl
                 << "===============================================" << endl;
        }

        bslma_TestAllocator ta(veryVeryVeryVerbose);
        {
            int rc;

#ifdef BSLS_PLATFORM__OS_WINDOWS
            const char PIPE_NAME1[] = "\\\\.\\pipe\\ctrl.pcctest2-1";
            const char PIPE_NAME2[] = "\\\\.\\pipe\\ctrl.pcctest2-2";
#else
            const char PIPE_NAME1[] = "/tmp/ctrl.baea.pcctest2-1";
            const char PIPE_NAME2[] = "/tmp/ctrl.baea.pcctest2-2";
            unlink(PIPE_NAME1);
            unlink(PIPE_NAME2);
#endif

            baea_PipeControlChannel channel(&noop, &ta);

            rc = channel.start(PIPE_NAME1);
            ASSERT(0 == rc);

            ASSERT(PIPE_NAME1 == channel.pipeName());

            channel.shutdown();
            channel.stop();

            rc = channel.start(PIPE_NAME2);
            ASSERT(0 == rc);

            ASSERT(PIPE_NAME2 == channel.pipeName());

            channel.shutdown();
            channel.stop();

            rc = channel.start(PIPE_NAME2);
            ASSERT(0 == rc);

            ASSERT(PIPE_NAME2 == channel.pipeName());

            channel.shutdown();
            channel.stop();
        }
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise default implementation of task manager component by
        //   calling primary manipulators and accessors and verifying their
        //   return codes.
        //
        // Plan:
        //   Create a pipe control channel, ensure the object reports the pipe
        //   name correctly, start, shutdown, and stop the object.  Verify
        //   each function's return code.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) {
            cout << "Breathing Test" << endl
                 << "==============" << endl;
        }

        bslma_TestAllocator ta(veryVeryVeryVerbose);
        {
#ifdef BSLS_PLATFORM__OS_WINDOWS
            const char PIPE_NAME[] = "\\\\.\\pipe\\ctrl.baea.pcctest1";
#else
            const char PIPE_NAME[] = "/tmp/ctrl.baea.pcctest1";
            unlink(PIPE_NAME);
#endif

            baea_PipeControlChannel channel(&noop, &ta);

            int rc = channel.start(PIPE_NAME);
            ASSERT(0 == rc);

            ASSERT(PIPE_NAME == channel.pipeName());

            channel.shutdown();
            channel.stop();
        }
        ASSERT(0 == ta.numBytesInUse());
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
