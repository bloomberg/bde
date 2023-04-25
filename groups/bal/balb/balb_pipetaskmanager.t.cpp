// balb_pipetaskmanager.t.cpp                                         -*-C++-*-
#include <balb_pipetaskmanager.h>

#include <bdlb_guidutil.h>
#include <bdlb_string.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bdls_filesystemutil.h>
#include <bdls_pathutil.h>
#include <bdls_pipeutil.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmt_threadutil.h>

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>

#include <bsl_cstddef.h>   // 'bsl::size_t'
#include <bsl_cstdlib.h>   // 'bsl::atoi'
#include <bsl_iostream.h>
#include <bsl_memory.h>    // 'bsl::shared_ptr', 'bsl::(make|allocate)_shared'
#include <bsl_ostream.h>   // 'operator<<'
#include <bsl_sstream.h>   // 'bsl::ostringstring'
#include <bsl_streambuf.h> // 'bsl::streambuf'
#include <bsl_string.h>
#include <bsl_string_view.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
    #include <windows.h>
#else
    #include <fcntl.h>   // 'open',  'O_WRONLY'
    #include <unistd.h>  // 'write', 'close'

    #include <stdio.h>   // 'perror'
    #include <errno.h>
    #include <stdlib.h>  // 'unsetenv'
#endif

using namespace BloombergLP;
using namespace BloombergLP::balb;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This component is implemented 'balb::PipeTaskManager' (PTM) by combining two
// existing classes: 'balb::ControlManager and 'balb::PipeControlChannel'.
// Accordingly, most of the tests here address determine whether or not those
// other classes are configured as expected and are forwarded the correct
// information.
//
// Of necessity, some tests here actually write messages to named pipes and
// observe that the handlers perform their expected actions.  However, since
// reading of the named pipe and invocation of the message handlers is done by
// a background thread, it is challenging to know when the handler has
// completed so one can check for the expected results.  In some cases, the
// main thread simply sleeps (implicit race condition); in other cases, an
// entire scenario is run and the cumulative results are checked once the PTM
// has been stopped.  Even the later case has a possible race condition, as the
// 'stop' method does not guarantee that messages in the pipe have been
// handled.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] explicit PipeTaskManager(*bA = 0);
// [ 5] explicit PipeTaskManager(shared_ptr<ControlManager>& cm, *bA = 0);
// [ 2] ~PipeTaskManager();
//
// MANIPULATORS
// [ 2] balb::ControlManager& controlManager();
//
// [ 3] int start(const bsl::string_view& pipeBasename);
// [ 3] void shutdown();
// [ 3] int stop();
//
// ACCESSORS
// [ 2] const balb::ControlManager& controlManager() const;
// [ 3] const bsl::string& pipeName() const;
//
// [ 2] bslma::Allocator *allocator() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] CONCERN: Writing to the named pipe invokes intended callbacks.
// [ 6] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define ASSERT_EQ(X,Y) ASSERTV(X, Y, X == Y)
#define ASSERT_NE(X,Y) ASSERTV(X, Y, X != Y)
#define ASSERT_LT(X,Y) ASSERTV(X, Y, X <  Y)
#define ASSERT_LE(X,Y) ASSERTV(X, Y, X <= Y)
#define ASSERT_GT(X,Y) ASSERTV(X, Y, X >  Y)
#define ASSERT_GE(X,Y) ASSERTV(X, Y, X >= Y)

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef PipeTaskManager Obj;
typedef ControlManager  CM;

// ============================================================================
//                   HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

#ifdef BSLS_PLATFORM_OS_WINDOWS
void mswrite(HANDLE handle, const char *message, int length)
    // Write to the specified 'handle' the specified 'message' having the
    // specified 'length'.
{
    DWORD dummy;
    bool  result = WriteFile(handle, message, length, &dummy, 0);
    ASSERT(result);
    if (!result) {
        char errorBuffer[128];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                      0,
                      GetLastError(),
                      LANG_SYSTEM_DEFAULT,
                      errorBuffer,
                      sizeof(errorBuffer),
                      0);
        bsl::cout << "Error was \"" << errorBuffer << "\"" << bsl::endl;
    }
    FlushFileBuffers(handle);
}
#endif

void noop(const bsl::string_view& , bsl::istream& )
    // Fail the test.  Note that in the absence of errors this callback is
    // never invoked.
{
    ASSERT(!"called");
}

void rc1cb(const bsl::string_view& , bsl::istream&, int *rc)
    // Assign '1' to the specified '*rc'.
{
    *rc = 1;
}

void rc2cb(const bsl::string_view& , bsl::istream&, int *rc)
    // Assign '2' to the specified '*rc'.
{
    *rc = 2;
}

void sleep2shutdown(Obj *obj)
    // Wait two seconds and then shutdown the specified 'obj'.
{
    bslmt::ThreadUtil::microSleep(0, 2);
    obj->shutdown();
}

void vec1cb(const bsl::string_view& , bsl::istream& , bsl::vector<int> *vec)
    // Append to the specified 'vec' an element having the value '1'.
{
    vec->push_back(1);
}

void vec2cb(const bsl::string_view& , bsl::istream&, bsl::vector<int> *vec)
    // Append to the specified 'vec' an element having the value '2'.
{
    vec->push_back(2);
}

void onExit(const bsl::string_view& , bsl::istream&, Obj *obj)
    // Invoke the 'shutdown' method of the specified 'obj'.
{
    obj->shutdown();
}

// ============================================================================
//                               USAGE EXAMPLES
// ----------------------------------------------------------------------------

namespace Usage {

// BDE_VERIFY pragma: -FABC01 //  not in alphanumeric order

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// Suppose one is creating an application that allows for dynamically changing
// its logging verbosity level, resetting to its initial state, to shutdown
// cleanly, and the listing a description of supported messages.
//
// The 'balb::PipeTaskManager' class can be used to provide support for
// messages that are sent via a named pipe and have the syntax shown below:
//..
//  Supported messages:
//      EXIT no arguments
//          Terminate the application.
//      HELP no arguments
//          List registered messages.
//      LOG <GET|SET <level> >
//          Get/set verbosity level.
//      RESTART no arguments
//          Restart the application.
//..
// Note that the above description corresponds to the output produced by our
// application in response to a "HELP" message.
//
// First, define several global, atomic variables that will be used to exchange
// information between the thread that monitors the named pipe and the other
// threads of the application.
//..
    static bsls::AtomicBool done(false);
    static bsls::AtomicInt  progress(0);
    static bsls::AtomicInt  myLoggingManagerLevel(0);
//..
// Then, we define helper functions 'myLoggingManagerGet' and
// 'myLoggingManagerSet' so that the handler for "LOG" messages can delegate
// processing the "GET" and "SET" subcommands.  The other defined messages have
// minimal syntax so use of a delegation pattern is overkill in those cases.
//..
    void myLoggingManagerGet()
        // Print the current log level to the console.
    {
        bsl::cout << "LOG LEVEL IS NOW"    << ": "
                  << myLoggingManagerLevel << bsl::endl;
    }

    void myLoggingManagerSet(bsl::istream& message)
        // Set the log level to the value obtained from the specified
        // 'message' and print that value to the console.
    {
        int newLogLevel;
        message >> newLogLevel; // Cannot stream to an 'bsls::AtomicInt'.

        myLoggingManagerLevel = newLogLevel;

        bsl::cout << "LOG LEVEL SET TO"    << ": "
                  << myLoggingManagerLevel << bsl::endl;
    }
//..
// Next, define handler functions for the "EXIT", "RESTART", and "LOG"
// messages.
//..
    void onExit(const bsl::string_view& , bsl::istream& )
        // Handle a "EXIT" message.
    {
        bsl::cout << "onExit" << bsl::endl;
        done = true;
    }

    void onRestart(const bsl::string_view& , bsl::istream& )
        // Handle a "RESTART" message.
    {
        bsl::cout << "onRestart" << bsl::endl;
        progress = 0;
    }

    void onLog(const bsl::string_view& , bsl::istream& message)
        // Handle a "LOG" message supporting sub command "GET" and "SET".  If
        // the subcommand is "SET" the new log level is obtained from the
        // specified 'message'.
    {
        bsl::cout << "onLog" << bsl::endl;

        bsl::string subCommand;
        message >> subCommand;
            // See the registration of the 'onLog' handler below for the
            // details of the supported sub commands and their arguments.

        if ("GET" == subCommand) {
            myLoggingManagerGet();
        }
        else if ("SET" == subCommand) {
            myLoggingManagerSet(message);
        }
        else {
            bsl::cout << "onLog"              << ": "
                      << "unknown subcommand" << ": "
                      << subCommand           << bsl::endl;
        }
    }
//..
// Notice that no handler is yet defined for the "HELP" message.  That
// functionally can be provided using methods of the contained
// 'balb::ControlManager' object.  See below.
//
// Then, create a 'balb::PipeTaskManger' object and register the above
// functions as handlers:
//..
    int myApplication1()
        // Run Application1 and return status;
    {
        balb::PipeTaskManager taskManager;

        int rc;

        rc = taskManager.controlManager().registerHandler(
                                                  "EXIT",
                                                  "no arguments",
                                                  "Terminate the application.",
                                                  onExit);
        ASSERT(0 == rc);
        rc = taskManager.controlManager().registerHandler(
                                                  "RESTART",
                                                  "no arguments",
                                                  "Restart the application.",
                                                  onRestart);
        ASSERT(0 == rc);
        rc = taskManager.controlManager().registerHandler(
                                                  "LOG",
                                                  "<GET|SET <level> >",
                                                  "Get/set verbosity level.",
                                                  onLog);
        ASSERT(0 == rc);
//..
// and add an additional handler that provides a list of the registered
// messages and the syntax for using them:
//..
        rc = taskManager.controlManager().registerUsageHandler(bsl::cout);
        ASSERT(0 == rc);
//..
// Next, if we are on a Unix system, we confirm that our named pipes will be
// created in the directory named by the 'TMPDIR' environment variable:
//..
    #if   defined(BSLS_PLATFORM_OS_UNIX)
        rc = unsetenv("SOCKDIR"); // 'SOCKDIR' has precedence over 'TMPDIR'.
        ASSERT(0 == rc);

        const char *expectedDirectory = getenv("TMPDIR");
    #elif defined(BSLS_PLATFORM_OS_WINDOWS)
        const char *expectedDirectory = "\\\\.\\pipe\\";
    #else
        #error "Unexpected platform."
    #endif
//..
// Then, start listening for incoming messages at pipe having the name based on
// on the name "MyApplication.CTRL".
//..
        rc = taskManager.start("MyApplication.CTRL");
        ASSERT(0 == rc);
//..
// Next, for expository purposes, confirm that a pipe of that name exists in
// the expected directory and is open for reading.
//..
        const bsl::string_view pipeName = taskManager.pipeName();

        ASSERT(bdls::PathUtil::isAbsolute      (pipeName));
    #ifdef BSLS_PLATFORM_OS_UNIX
        ASSERT(bdls::PipeUtil::isOpenForReading(pipeName));
    #endif

        bsl::string canonicalDirname, canonicalLeafName;

        bdls::PathUtil::getDirname(&canonicalDirname,  pipeName);
        bdls::PathUtil::getLeaf   (&canonicalLeafName, pipeName);

        ASSERT(0 == bsl::strcmp(expectedDirectory, canonicalDirname.c_str()));
        ASSERT("myapplication.ctrl" == canonicalLeafName);
//..
// Notice that given 'baseName' has been canonically converted to lowercase.
//
// Now, our application can continue doing useful work while the background
// thread monitors the named pipe for incoming messages:
//..
        while (!done) {
            // Do useful work while background thread responds to incoming
            // commands from the named pipe.
        }

        return 0;
    }
//..
// Finally, in some other programming context, say 'mySender', a context in
// another process that has been passed the value of 'pipeName', control
// messages can be sent to 'myApplication1' above.
//..
    void mySender(const bsl::string& pipeName)
        // Write control messages into the pipe named by the specified
        // 'pipeName'.
    {
        int rc;
        rc = bdls::PipeUtil::send(pipeName, "LoG GET\n");
        ASSERT(0 == rc);
        rc = bdls::PipeUtil::send(pipeName, "Log SET 4\n");
        ASSERT(0 == rc);
        rc = bdls::PipeUtil::send(pipeName, "log GET\n");
        ASSERT(0 == rc);
        rc = bdls::PipeUtil::send(pipeName, "\n");       // empty
        ASSERT(0 == rc);
        rc = bdls::PipeUtil::send(pipeName, "RESET\n");  // invalid
        ASSERT(0 == rc);
        rc = bdls::PipeUtil::send(pipeName, "RESTART\n");
        ASSERT(0 == rc);
        rc = bdls::PipeUtil::send(pipeName, "EXIT\n");
        ASSERT(0 == rc);
    }
//..
// Notice that:
//
//: o Each message must be terminated by a newline character.
//:
//: o Although each registered message prefix was all capital letters, the
//:   prefix field in the sent message is case insensitive -- "LoG", "Log", and
//:   "log" all invoke the intended handler.  If we wanted case insensitivity
//:   for the subcommands "GET" and "SET" we would change of implementation of
//:   'onLog' accordingly.
//:
//: o The empty message and the unregistered "RESET" message are silently
//:   ignored.  The console output (see below) shows no indication that these
//:   were sent.
//
// The console log of our application shows the response for each received
// control message.  In general, these messages will be interleaved with the
// output of the "useful work" done in the 'for' loop of 'myApplicaton1.
//..
//  onLog
//  LOG LEVEL IS NOW: 0
//  onLog
//  LOG LEVEL SET TO: 4
//  onLog
//  LOG LEVEL IS NOW: 4
//  onRestart
//  onExit
//..

// BDE_VERIFY pragma: +FABC01 //  not in alphanumeric order

}  // close namespace Usage

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5; (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "USAGE EXAMPLE" << endl
                 << "=============" << endl;
        }

        bsl:: ostringstream ossOut;
        bsl:: ostringstream ossErr;

        bsl::streambuf *outStreamBuf = cout.rdbuf();
        bsl::streambuf *errStreamBuf = cerr.rdbuf();

        if (!verbose) {
            cout.rdbuf(ossOut.rdbuf());  // Redirect 'cout'
            cerr.rdbuf(ossOut.rdbuf());  // Redirect 'cerr'
        }

        Usage::done = true; // avoid infinite loop in {Example 1}.
        ASSERT(0 == Usage::myApplication1());                           // TEST

        if (!verbose) {
            cout.rdbuf(outStreamBuf);  // Restore 'cout'
            cerr.rdbuf(errStreamBuf);  // Restore 'cerr'
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // SHARED 'balb::ControlManager'
        //
        // Concerns:
        //: 1 A 'PipeTaskManager' constructed using a shared 'ControlManager'
        //:   as single allocation -- just one block for the owned
        //:   'PipeControlChannel' object -- from the intended allocator.
        //:
        //: 2 The shared 'ControlManager' can use an different allocator than
        //:   that by the 'PipeTaskmanager'.
        //
        // Plan:
        //: 1 Use the "footprint" idiom to confirm the intended installation
        //:   allocator is installed when 'basicAllocator' is supplied, not
        //:   supplied, and explicitly supplied as '0'.  In each case,
        //:   use a shared 'ControlManager' that uses a different allocator
        //:   and confirm its installation using the 'allocator' method of
        //:   'ControlManager'.  (C-1..2)
        //
        // Testing:
        //   explicit PipeTaskManager(shared_ptr<ControlManager>& cm, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "SHARED 'balb::ControlManager'" << endl
                 << "=============================" << endl;
        }

        if (veryVerbose) cout << "Test constructor taking shared CM." << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;

            if (veryVerbose) {
                P(CONFIG);
            }

            bslma::TestAllocator ca("controlManager", veryVeryVeryVerbose);
            bsl::shared_ptr<CM>  sharedCm = bsl::allocate_shared<CM>(&ca);

            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr          = 0;
            bslma::TestAllocator *objAllocatorPtr = 0;

            switch (CONFIG) {
              case 'a': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj(sharedCm);                        // TEST
              } break;
              case 'b': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj(sharedCm, 0);                     // TEST
              } break;
              case 'c': {
                objAllocatorPtr = &sa;
                objPtr = new (fa) Obj(sharedCm, objAllocatorPtr);       // TEST
              } break;
              default: {
                BSLS_ASSERT_OPT(!"Bad allocator config.");
              } break;
            }
            ASSERT(objPtr);
            ASSERT(objAllocatorPtr);

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

            ASSERTV(CONFIG, &oa == X.allocator());
            ASSERTV(CONFIG, &ca == X.controlManager().allocator());

            ASSERTV(CONFIG, 1 ==  oa.numBlocksTotal()); // 'PipeControlChannel'
            ASSERTV(CONFIG, 0 == noa.numBlocksTotal());

            fa.deleteObject(objPtr);

            ASSERTV(CONFIG, 0 <   oa.numBlocksTotal());
            ASSERTV(CONFIG, 0 ==  oa.numBlocksInUse());
            ASSERTV(CONFIG, 0 == noa.numBlocksTotal());
        }

        if (veryVerbose) {
            cout << "Test multiple PTMs sharing a CM." << endl;
        }
        {
            using namespace bdlf::PlaceHolders;

            int rc = 3;

            CM::ControlHandler x1 = bdlf::BindUtil::bind(&rc1cb,
                                                         _1,
                                                         _2,
                                                         &rc);

            CM::ControlHandler x2 = bdlf::BindUtil::bind(&rc2cb,
                                                         _1,
                                                         _2,
                                                         &rc);

            const bsl::string PIPE_NAME1 = bdlb::GuidUtil::guidToString(
                                                   bdlb::GuidUtil::generate());
            const bsl::string PIPE_NAME2 = bdlb::GuidUtil::guidToString(
                                                   bdlb::GuidUtil::generate());
            ASSERT(PIPE_NAME1 != PIPE_NAME2);

            bsl::shared_ptr<CM> cmSharedPtr = bsl::make_shared<CM>();
            {
                Obj obj1(cmSharedPtr);
                Obj obj2(cmSharedPtr);

                ASSERT_EQ(0, obj1.controlManager().registerHandler("MESSAGE1",
                                                                   "",
                                                                   "",
                                                                   x1));
                ASSERT_EQ(0, obj2.controlManager().registerHandler("MESSAGE2",
                                                                   "",
                                                                   "",
                                                                   x2));

                // Both PTM's share can now respond to the same messages,
                // irrespective of which PTM registered the message.

                ASSERT(0 == obj1.start(PIPE_NAME1));
                ASSERT(0 == obj2.start(PIPE_NAME2));

                int rcSend;

                rcSend = bdls::PipeUtil::send(obj1.pipeName(), "MESSAGE1\n");
                bslmt::ThreadUtil::microSleep(0, 2);
                ASSERTV(rcSend, 0 == rcSend);
                ASSERTV(rc,     1 == rc);

                rcSend = bdls::PipeUtil::send(obj1.pipeName(), "MESSAGE2\n");
                bslmt::ThreadUtil::microSleep(0, 2);
                ASSERTV(rcSend, 0 == rcSend);
                ASSERTV(rc,     2 == rc);

                rcSend = bdls::PipeUtil::send(obj2.pipeName(), "MESSAGE1\n");
                bslmt::ThreadUtil::microSleep(0, 2);
                ASSERTV(rcSend, 0 == rcSend);
                ASSERTV(rc,     1 == rc);

                rcSend = bdls::PipeUtil::send(obj2.pipeName(), "MESSAGE2\n");
                bslmt::ThreadUtil::microSleep(0, 2);
                ASSERTV(rcSend, 0 == rcSend);
                ASSERTV(rc,     2 == rc);
            }

            const char EXPECTED_USAGE[] =              "\n"
                                          "    MESSAGE1 \n"
                                          "    MESSAGE2 \n";

            bsl::ostringstream oss;
            cmSharedPtr->printUsage(oss, "");

            ASSERTV(EXPECTED_USAGE,   oss.str(),
                    EXPECTED_USAGE == oss.str());
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CONCERN: WRITING TO THE PIPE INVOKES CALLBACKS
        //
        // Concerns:
        //: 1 Writing to the named pipe invokes registered callbacks
        //
        // Plan:
        //: 1 Start a task manager.
        //:
        //: 2 Write to the pipe using the name obtained from 'pipeName', and
        //:   verify that the associated callback is invoked.
        //
        // Testing:
        //   CONCERN: Writing to the named pipe invokes intended callbacks.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "CONCERN: WRITING TO THE PIPE INVOKES CALLBACKS" << endl
                 << "==============================================" << endl;
        }

        bslma::TestAllocator ta("supplied");

        {
            const bsl::string PIPE_NAME = bdlb::GuidUtil::guidToString(
                                                   bdlb::GuidUtil::generate());

            Obj mX(&ta); const Obj& X = mX;

            using namespace bdlf::PlaceHolders;

            bsl::vector<int> vec;

        //-^
        CM::ControlHandler x1 = bdlf::BindUtil::bind(&vec1cb, _1, _2, &vec);
        CM::ControlHandler x2 = bdlf::BindUtil::bind(&vec2cb, _1, _2, &vec);
        CM::ControlHandler xx = bdlf::BindUtil::bind(&onExit, _1, _2, &mX);

        ASSERT_EQ(0, mX.controlManager().registerHandler("XXX", "", "", xx));
        ASSERT_EQ(0, mX.controlManager().registerHandler("ONE", "", "", x1));
        ASSERT_EQ(0, mX.start(PIPE_NAME));
        ASSERT_EQ(0, mX.controlManager().registerHandler("TWO", "", "", x2));
        //-v

#ifdef BSLS_PLATFORM_OS_WINDOWS
            HANDLE pipe = CreateFile(X.pipeName().c_str(),
                                     GENERIC_WRITE,
                                     0,
                                     NULL,
                                     OPEN_EXISTING,
                                     0,
                                     NULL);
            ASSERT_NE(INVALID_HANDLE_VALUE, pipe);
            mswrite(pipe, "TWO\n", sizeof "TWO\n" - 1);
            mswrite(pipe, "ONE\n", sizeof "ONE\n" - 1);
            mswrite(pipe, "TWO\n", sizeof "TWO\n" - 1);
            mswrite(pipe, "XXX\n", sizeof "XXX\n" - 1);
            CloseHandle(pipe);
#else
            int pipe = open(X.pipeName().c_str(), O_WRONLY);
            ASSERT_NE(-1, pipe);
            write(pipe, "TWO\n", sizeof "TWO\n" - 1);
            write(pipe, "ONE\n", sizeof "ONE\n" - 1);
            write(pipe, "TWO\n", sizeof "TWO\n" - 1);
            write(pipe, "XXX\n", sizeof "XXX\n" - 1);
            close(pipe);
#endif

            ASSERT_EQ(0, mX.stop());

            ASSERT_EQ(3, vec.size());
            ASSERT_EQ(2, vec[0]);
            ASSERT_EQ(1, vec[1]);
            ASSERT_EQ(2, vec[2]);
        }

        ASSERT_LE(0, ta.numAllocations());
        ASSERT_EQ(0, ta.numBytesInUse());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING START, SHUTDOWN, AND STOP
        //
        // Concerns:
        //: 1 'shutdown and 'stop' are idempotent.
        //:
        //: 2 Pipe names are normalized to unique names.
        //:
        //: 3 'start' fails if the manager is already started.
        //:
        //: 4 Once started, 'start' fails unless 'stop' is called.
        //:
        //: 5 Multiple managers can be started simultaneously.
        //:
        //: 6 'stop' blocks until 'shutdown' is called.
        //;
        //: 7 'stop' removes the named pipe.
        //:
        //: 8 A stopped PTM can be restarted.
        //
        // Plan:
        //: 1 Create and start a single manager.
        //:
        //: 2 Verify that once started, the manager cannot be started again
        //:   using the same pipe name, or one with an alternate spelling until
        //:   it has been stopped.
        //:
        //: 3 Create and start multiple managers with different pipe names.
        //:
        //: 4 Verify that they can be stopped independently.
        //:
        //: 5 Verify that once a manager is started, calling 'stop' blocks
        //:   until 'shutdown' is called.
        //:
        //: 6 Verify that 'start' creates a named pipe and open its and that
        //:   'stop' removes that named pipe.
        //
        // Testing:
        //   int start(const bsl::string_view& pipeBasename);
        //   void shutdown();
        //   int stop();
        //   const bsl::string& pipeName() const;
        // --------------------------------------------------------------------

        if (verbose)  {
            cout << endl
                 << "TESTING START, SHUTDOWN, AND STOP" << endl
                 << "=================================" << endl;
        }

        bslma::TestAllocator ta("supplied");

        {
            {
                const bsl::string PIPE = bdlb::GuidUtil::guidToString(
                                                   bdlb::GuidUtil::generate());

                bsl::string lcpn(PIPE);
                bdlb::String::toLower(&lcpn);

                bsl::string ucpn(PIPE);
                bdlb::String::toUpper(&ucpn);

                if (veryVeryVerbose) {
                    P(PIPE)
                    P(ucpn)
                    P(lcpn)
                }

                ASSERT_NE(lcpn, ucpn);

                Obj mX(&ta); const Obj& X = mX;

                ASSERT_EQ(0, mX.stop());
                ASSERT_EQ(0, mX.stop());

                mX.shutdown();
                ASSERT_EQ(0, mX.stop());

                mX.shutdown();
                mX.shutdown();
                ASSERT_EQ(0, mX.stop());

                ASSERT_EQ(0, mX.start(PIPE));

                if (veryVeryVerbose) {
                    Q(Verify that 'PIPE' is used in the actual pipe name)
                }

                ASSERT_NE(bsl::string::npos, X.pipeName().find(lcpn));

                ASSERT_NE(0, mX.start(PIPE));
                ASSERT_NE(0, mX.start(lcpn));
                ASSERT_NE(0, mX.start(ucpn));
                mX.shutdown();
                ASSERT_EQ(0, mX.stop());

                ASSERT_EQ(0, mX.start(PIPE));
                ASSERT_NE(0, mX.start(PIPE));
                mX.shutdown();
                ASSERT_NE(0, mX.start(PIPE));
                ASSERT_EQ(0, mX.stop());
                ASSERT_EQ(0, mX.stop());
            }

            {
                const bsl::string PIPE_NAME1 = bdlb::GuidUtil::guidToString(
                                                   bdlb::GuidUtil::generate());
                const bsl::string PIPE_NAME2 = bdlb::GuidUtil::guidToString(
                                                   bdlb::GuidUtil::generate());

                Obj mX1(&ta);
                Obj mX2(&ta);

                ASSERT_EQ(0, mX1.start(PIPE_NAME1));  // started
                ASSERT_NE(0, mX1.start(PIPE_NAME1));  // cannot restart
                ASSERT_NE(0, mX1.start(PIPE_NAME2));  // cannot restart
#ifdef BSLS_PLATFORM_OS_UNIX
                ASSERT_NE(0, mX2.start(PIPE_NAME1));  // other PTM cannot use
#endif

                ASSERT_EQ(0, mX2.start(PIPE_NAME2));
#ifdef BSLS_PLATFORM_OS_UNIX
                ASSERT_NE(0, mX2.start(PIPE_NAME2));
#endif
                ASSERT_NE(0, mX2.start(PIPE_NAME1));

                mX1.shutdown();
                ASSERT_EQ(0, mX1.stop());

                ASSERT_NE(0, mX2.start(PIPE_NAME2));
                ASSERT_EQ(0, mX1.start(PIPE_NAME1));

                // Do not stop managers, but call the destructor
            }

            {
                const bsl::string PIPE_NAME = bdlb::GuidUtil::guidToString(
                                                   bdlb::GuidUtil::generate());

                Obj mX(&ta); const Obj& X = mX;

                ASSERT_EQ(0, mX.start(PIPE_NAME));
                const bsl::string& pipeName = X.pipeName();

#ifdef BSLS_PLATFORM_OS_UNIX
                ASSERT(bdls::PipeUtil::isOpenForReading(pipeName));
#endif

                mX.shutdown();
                mX.stop();

                ASSERT(!bdls::FilesystemUtil::exists(pipeName));        // TEST
            }

            {
                const bsl::string PIPE_NAME1 = bdlb::GuidUtil::guidToString(
                                                   bdlb::GuidUtil::generate());
                const bsl::string PIPE_NAME2 = bdlb::GuidUtil::guidToString(
                                                   bdlb::GuidUtil::generate());

                Obj mX(&ta); const Obj& X = mX;

                ASSERT_EQ(0, mX.start(PIPE_NAME1));

                mX.shutdown();
                ASSERT_EQ(0, mX.stop());

                // Restart for some other named pipe.
                ASSERT_EQ(0, mX.start(PIPE_NAME2));                     // TEST
#ifdef BSLS_PLATFORM_OS_UNIX
                ASSERT(bdls::PipeUtil::isOpenForReading(X.pipeName()));
#endif
            }

            {
                const bsl::string PIPE_NAME = bdlb::GuidUtil::guidToString(
                                                   bdlb::GuidUtil::generate());

                Obj mX(&ta);

                ASSERT_EQ(0, mX.start(PIPE_NAME));

                bsls::Stopwatch sw;

                bslmt::ThreadUtil::Handle handle;
                ASSERT_EQ(0, bslmt::ThreadUtil::create(&handle,
                                                       bdlf::BindUtil::bind(
                                                               &sleep2shutdown,
                                                               &mX)));
                sw.start();
                ASSERT_EQ(0, mX.stop());
                sw.stop();

                ASSERT_LE(1.0, sw.elapsedTime());
                ASSERT_EQ(0,   bslmt::ThreadUtil::join(handle));
            }
        }

        ASSERT_LE(0, ta.numAllocations());
        ASSERT_EQ(0, ta.numBytesInUse());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CONSTRUCTOR, BASIC ACCESSORS, AND DESTRUCTOR
        //
        // Concerns:
        //: 1 A default constructed 'PipeTaskManager' makes two allocations --
        //:   one for the owned 'ControlManager' and one for the owned
        //:   'PipeControlChannel' objects -- from the intended allocator.
        //:
        //: 2 A default constructed 'PipeTaskManger' has a 'ControlManager'
        //:   with no registered messages and configured with the same
        //:   allocator as the 'PipeTaskManager'
        //:
        //: 3 The tested accessors are all 'const' qualified.
        //:
        //: 4 The 'controlManager' manipulator method provides a non-'const'
        //:   reference to the owned 'ControlManager'.
        //
        // Plan:
        //: 1 Use the "footprint" idiom to confirm the intended installation
        //:   allocator is installed when 'basicAllocator' is supplied, not
        //:   supplied, and explicitly supplied as '0'.  (C-1)
        //:
        //: 2 Confirm the 'const'-ness of accessors by invoking them on
        //:   'const'-qualified objects.  (C-3)
        //
        //: 3 Confirm that 'printUsage' shows the output expected of an empty
        //:   'ControlManager'.  (C-2)
        //:
        //: 4 Confirm that in a non-'const' context, the 'controlManager'
        //:   method provides a reference that allows invocation of the
        //:   'registerHandler' and 'deregisterHandler' methods of the owned
        //:   'ControlManager'.  (C-4).
        //:
        //:   o Incidentally, the usage scenario of the 'ControlManager' tests
        //:     several features of that class such as:
        //:     o Redefinition of the handler for a registered prefix.
        //:     o Case insensitivity of the prefix in messages.
        //
        // Testing:
        //   explicit PipeTaskManager(*bA = 0);
        //   ~PipeTaskManager();
        //   balb::ControlManager& controlManager();
        //   const balb::ControlManager& controlManager() const;
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
             << "DEFAULT CONSTRUCTOR, BASIC ACCESSORS, AND DESTRUCTOR" << endl
             << "====================================================" << endl;
        }

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;

            if (veryVerbose) {
                P(CONFIG);
            }

            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr          = 0;
            bslma::TestAllocator *objAllocatorPtr = 0;

            switch (CONFIG) {
              case 'a': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj();                                // TEST
              } break;
              case 'b': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj(0);                               // TEST
              } break;
              case 'c': {
                objAllocatorPtr = &sa;
                objPtr = new (fa) Obj(objAllocatorPtr);
              } break;
              default: {
                BSLS_ASSERT_OPT(!"Bad allocator config.");
              } break;
            }

            ASSERT(objPtr);
            ASSERT(objAllocatorPtr);

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

            ASSERTV(CONFIG, &oa == X.allocator());                      // TEST
            ASSERTV(CONFIG, &oa == X.controlManager().allocator());     // TEST

            ASSERTV(CONFIG, 2 ==  oa.numBlocksTotal());
                              // owned 'ControlManager' and PipeControlChannel'
            ASSERTV(CONFIG, 0 == noa.numBlocksTotal());

            bslma::TestAllocator outputAlloc("output", veryVeryVeryVerbose);
            bsl::ostringstream   oss(&outputAlloc);

            // String returned from 'oss' must not upset the counts of default
            // allocator usage.

            const char PREAMBLE[] = "is empty";
            const char IS_EMPTY[] = "is empty\n";

            const bsl::size_t PREAMBLE_MSGSIZE = sizeof PREAMBLE
                                               - 1  // terminating '\0'
                                               + 1; // appended    '\n'
            ASSERT(PREAMBLE_MSGSIZE <= bsl::string().capacity());

            X.controlManager().printUsage(oss, PREAMBLE);
            ASSERT(IS_EMPTY == oss.str());

            using namespace bdlf::PlaceHolders;

            int rc = 3;

            CM::ControlHandler x1 = bdlf::BindUtil::bind(&rc1cb,
                                                         _1,
                                                         _2,
                                                         &rc);

            CM::ControlHandler x2 = bdlf::BindUtil::bind(&rc2cb,
                                                         _1,
                                                         _2,
                                                         &rc);

            ASSERT_EQ(3, rc);
            ASSERT_NE(0, mX.controlManager().dispatchMessage("MESSAGE"));
            ASSERT_EQ(3, rc);

            ASSERT_EQ(0, mX.controlManager().registerHandler("MESSAGE",
                                                             "",
                                                             "",
                                                             x1));
            ASSERT_EQ(0, mX.controlManager().dispatchMessage("MESSAGE"));
            ASSERT_EQ(1, rc);

            ASSERT_EQ(0, mX.controlManager().dispatchMessage("message"));
            ASSERT_EQ(1, rc);

            ASSERT_LE(0, mX.controlManager().registerHandler("MESSAGE",
                                                             "",
                                                             "",
                                                             x2));
            ASSERT_EQ(0, mX.controlManager().dispatchMessage("MESSAGE"));
            ASSERT_EQ(2, rc);
            ASSERT_EQ(0, mX.controlManager().dispatchMessage("message"));
            ASSERT_EQ(2, rc);

            ASSERT_LE(0, mX.controlManager().registerHandler("mEsSaGe",
                                                             "",
                                                             "",
                                                             x1));
            ASSERT_EQ(0, mX.controlManager().dispatchMessage("MESSAGE"));
            ASSERT_EQ(1, rc);
            ASSERT_EQ(0, mX.controlManager().dispatchMessage("message"));
            ASSERT_EQ(1, rc);

    //------^
    ASSERT_EQ(0, mX.controlManager().deregisterHandler("message"));
    ASSERT_NE(0, mX.controlManager().dispatchMessage  ("mEsSaGe"));
    ASSERT_NE(0, mX.controlManager().deregisterHandler("MESSAGE"));
    ASSERT_NE(0, mX.controlManager().dispatchMessage  ("MeSsAgE"));

    ASSERT_EQ(0, mX.controlManager().registerHandler("A",  "",  "", &noop));
    ASSERT_EQ(0, mX.controlManager().registerHandler("B", "1",  "", &noop));
    ASSERT_EQ(0, mX.controlManager().registerHandler("C",  "", "2", &noop));
    ASSERT_EQ(0, mX.controlManager().registerHandler("D", "3", "4", &noop));

    ASSERT_LE(0, mX.controlManager().registerHandler("A",  "",  "", &noop));
    ASSERT_LE(0, mX.controlManager().registerHandler("B",  "",  "", &noop));
    ASSERT_LE(0, mX.controlManager().registerHandler("C",  "",  "", &noop));
    ASSERT_LE(0, mX.controlManager().registerHandler("D",  "",  "", &noop));

    ASSERT_EQ(0, mX.controlManager().deregisterHandler("A"));
    ASSERT_EQ(0, mX.controlManager().deregisterHandler("B"));
    ASSERT_EQ(0, mX.controlManager().deregisterHandler("C"));
    ASSERT_EQ(0, mX.controlManager().deregisterHandler("D"));
    //------v

            fa.deleteObject(objPtr);                                    // TEST

            ASSERTV(CONFIG, 0 <   oa.numBlocksTotal());
            ASSERTV(CONFIG, 0 ==  oa.numBlocksInUse());
            ASSERTV(CONFIG, 0 == noa.numBlocksTotal());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create and start a 'balb::PipeTaskManager' instance.
        //:
        //: 2 In the main thread, exercise various 'balb::PipeTaskManager'
        //:   manipulators and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) {
            cout << bsl::endl
                 << "BREATHING TEST" << endl
                 << "==============" << endl;
        }

        const bsl::string PIPE_NAME = bdlb::GuidUtil::guidToString(
                                                   bdlb::GuidUtil::generate());

        if (veryVerbose) {
            P(PIPE_NAME);
        }

        bslma::TestAllocator ta("supplied");

        {
            Obj mX(&ta);

            if (veryVerbose) cout << "register some handlers" << endl;

        //-^
        ASSERT_EQ(0, mX.controlManager().registerHandler("1", "", "", &noop));
        ASSERT_EQ(0, mX.controlManager().registerHandler("2", "", "", &noop));
        ASSERT_EQ(0, mX.controlManager().registerHandler("3", "", "", &noop));
        //-v

            if (veryVerbose) cout << "de-register those handlers" << endl;

            ASSERT_EQ(0, mX.controlManager().deregisterHandler("1"));
            ASSERT_EQ(0, mX.controlManager().deregisterHandler("2"));
            ASSERT_EQ(0, mX.controlManager().deregisterHandler("3"));

            if (veryVerbose) cout << "start" << endl;

            ASSERT_EQ(0, mX.start(PIPE_NAME));

            if (veryVerbose) cout << "shutdown" << endl;

            mX.shutdown();

            if (veryVerbose) cout << "stop" << endl;

            ASSERT_EQ(0, mX.stop());
        }

        ASSERT_LE(0, ta.numAllocations());
        ASSERT_EQ(0, ta.numBytesInUse());

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
