// balb_pipetaskmanager.h                                             -*-C++-*-
#ifndef INCLUDED_BALB_PIPETASKMANAGER
#define INCLUDED_BALB_PIPETASKMANAGER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a pipe-based mechanism to process task control messages.
//
//@CLASSES:
//  balb::PipeTaskManager: message-to-handler dispatcher
//
//@SEE_ALSO: balb_controlmanager, balb_pipecontrolchannel
//
//@DESCRIPTION: This component provides a mechanism, 'balb::PipeTaskManager',
// that listens on a named pipe for messages that are typically used to
// influence the behavior of a (running) task.
//
// For example, a 'balb::PipeTaskManager' might be configured to listen on a
// well known named pipe (e.g., 'myapplication.ctrl'), for the control messages
// starting with:
//: o "EXIT",
//: o "RESTART",
//: o "LOG", and
//: o "HELP".
//
// The use of imperative verbs for the first field of a message is a common
// practice.  The first field is called the message "prefix".  On receipt of a
// message with a known prefix, a previously registered handler functor is
// invoked with two arguments:
//: 1 the prefix value, and
//: 2 an 'bsl::istream' from which the rest of the message (if any) can be
//:   read.
// Thus, we have a mechanism by which a running task can be sent commands and,
// optionally, arguments for those commands.
//
// Once the relationship between 'prefix' and handler has been specified, the
// 'start' method is used to create the named pipe (or re-open an existing
// named pipe) and a thread created to listen for messages.
//
// A human user on a console might then use a command line application to send
// control messages to the 'myapplication.ctrl' pipe to configure the behavior
// of the running task.  In the example above, the handler for the 'LOG' prefix
// expects additional parameters.  Thus:
//..
//  echo "LOG VERBOSITY 4" > $SOCKDIR/myapplication.ctrl
//..
// changes the logging verbosity of the task to level "4".  See
// {'bdls_pipeutil'} for functions that can be invoked from C++ code to send
// messages to a named pipe.
//
///Configuring the 'balbl::PipeTaskManager'
/// - - - - - - - - - - - - - - - - - - - -
// A default constructed 'balbl::PipeTaskManager' has no registered handlers.
// Users can use the exposed 'balb::ControlManager', to register different
// control message prefixes (typically "verbs") to dispatch received messages
// to an appropriate functor.
//
// Alternatively, one can construct a 'balb::PipeTaskManager' using a
// separately created and configured a 'balb::ControlManager' object.  Doing so
// allows that single 'balb::ControlManager' to be shared among several
// 'balb::PipeTaskManagrer' objects, each listening on a different named pipe.
//
///Thread Safety
///-------------
// This component is thread-safe but not thread-enabled, meaning that multiple
// threads may safely use their own instances of 'balb::PipeTaskManager', but
// may not manipulate the same instance of 'balb::PipeTaskManager'
// simultaneously.  Note that the contained 'balb::ControlManager' object is
// available via both 'const' and non='const' references and that object *is*
// safe for multiple threads.
//
///Requirements for the Named Pipe
///-------------------------------
// The 'balb::PipeTaskManger' objects waits for messages from a named pipe
// provided to the 'start' method.  The argument to 'start' -- mapped to all
// lower case, if needed -- determines the basename of the named pipe.  The
// directory of that named pipe depends on the platform and environment
// variables.
//
//: o On Windows, that directory is: "\\.\pipe\".
//: o On Unix systems, that directory is determined by
//:   o the 'SOCKDIR' environment variable, if set; otherwise,
//:   o the 'TMPDIR' environment variable, if set; otherwise,
//:   o the current directory.
//
// See the 'makeCanonicalName' overloads in 'bdls_pipeutil' for details.
//
// Moreover, the 'start' method must be able to *freshly* create a named pipe.
// In general, 'start' will fail if a named pipe of the calculated canonical
// name already exists.  On Unix, if that named pipe is not in use (not open
// for reading), the 'start' attempts to remove and re-create that named pipe.
//
// On Unix systems, named pipes are created having the permission '0666' (read
// and write for user, group, and other) limited by the current 'umask' value
// of the process.
//
// On successful completion of 'start', the (full) pathname of the created
// named pipe is provided by the 'pipeName' accessor.  The full path name must
// be passed to sending processes so they can open that named pipe and write
// control messages.
//
///Message Requirements
///--------------------
// Each message consists of a sequence of fields separated by blanks and/or
// tabs and terminated by a newline ('\n') character.  The terminating newline
// is not passed to the message handler.
//
// The first field is called the message "prefix" and is used to find a
// previously registered handler for the message.  The handler lookup is case
// insensitive.  Empty messages (newline only) and messages for which no
// handler can be found are silently ignored.
//
// Note that this facility provides a one-way flow of information from the
// writer to a named pipe to the registered message handler.  There is no
// mechanism here for validating message (e.g., a given prefix has required
// additional fields) or returning status.  Many applications provide output by
// writing to the console or to a log.
//
///Pipe Atomicity
/// - - - - - - -
// Users that expect multiple concurrent writers to a single pipe must be aware
// that the message content might be corrupted (interleaved) unless:
//
//: 1 Each message is written to the pipe in a single 'write' system call.
//: 2 The length of each message is less than 'PIPE_BUF' (the limit for
//:   guaranteed atomicity).
//
// The value 'PIPE_BUF' depends on the platform:
//..
//   +------------------------------+------------------+
//   | Platform                     | PIPE_BUF (bytes) |
//   +------------------------------+------------------+
//   | POSIX (minimum requirement)) |    512           |
//   | IBM                          | 32,768           |
//   | SUN                          | 32,768           |
//   | Linux                        | 65,536           |
//   | Windows                      | 65,536           |
//   +------------------------------+------------------+
//..
// Also note that Linux allows the 'PIPE_BUF' size to be changed via the
// 'fcntl' system call.
//
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
//  This process responds to the following messages:
//      EXIT no arguments
//          Terminate the application.
//      HELP
//          Display this message
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
//  static bsls::AtomicBool done(false);
//  static bsls::AtomicInt  progress(0);
//  static bsls::AtomicInt  myLoggingManagerLevel(0);
//..
// Then, we define helper functions 'myLoggingManagerGet' and
// 'myLoggingManagerSet' so that the handler for "LOG" messages can delegate
// processing the "GET" and "SET" subcommands.  The other defined messages have
// minimal syntax so use of a delegation pattern is overkill in those cases.
//..
//  void myLoggingManagerGet()
//      // Print the current log level to the console.
//  {
//      bsl::cout << "LOG LEVEL IS NOW"    << ": "
//                << myLoggingManagerLevel << bsl::endl;
//  }
//
//  void myLoggingManagerSet(bsl::istream& message)
//      // Set the log level to the value obtained from the specified
//      // 'message' and print that value to the console.
//  {
//      int newLogLevel;
//      message >> newLogLevel; // Cannot stream to an 'bsls::AtomicInt'.
//
//      myLoggingManagerLevel = newLogLevel;
//
//      bsl::cout << "LOG LEVEL SET TO"    << ": "
//                << myLoggingManagerLevel << bsl::endl;
//  }
//..
// Next, define handler functions for the "EXIT", "RESTART", and "LOG"
// messages.
//..
//  void onExit(const bsl::string_view& , bsl::istream& )
//      // Handle a "EXIT" message.
//  {
//      bsl::cout << "onExit" << bsl::endl;
//      done = true;
//  }
//
//  void onRestart(const bsl::string_view& , bsl::istream& )
//      // Handle a "RESTART" message.
//  {
//      bsl::cout << "onRestart" << bsl::endl;
//      progress = 0;
//  }
//
//  void onLog(const bsl::string_view& , bsl::istream& message)
//      // Handle a "LOG" message supporting sub command "GET" and "SET".  If
//      // the subcommand is "SET" the new log level is obtained from the
//      // specified 'message'.
//  {
//      bsl::cout << "onLog" << bsl::endl;
//
//      bsl::string subCommand;
//      message >> subCommand;
//          // See the registration of the 'onLog' handler below for the
//          // details of the supported sub commands and their arguments.
//
//      if ("GET" == subCommand) {
//          myLoggingManagerGet();
//      }
//      else if ("SET" == subCommand) {
//          myLoggingManagerSet(message);
//      }
//      else {
//          bsl::cout << "onLog"              << ": "
//                    << "unknown subcommand" << ": "
//                    << subCommand           << bsl::endl;
//      }
//  }
//..
// Notice that no handler is yet defined for the "HELP" message.  That
// functionally can be provided using methods of the contained
// 'balb::ControlManager' object.  See below.
//
// Then, create a 'balb::PipeTaskManger' object and register the above
// functions as handlers:
//..
//  int myApplication1()
//      // Run Application1 and return status;
//  {
//      balb::PipeTaskManager taskManager;
//
//      int rc;
//
//      rc = taskManager.controlManager().registerHandler(
//                                                "EXIT",
//                                                "no arguments",
//                                                "Terminate the application.",
//                                                onExit);
//      assert(0 == rc);
//      rc = taskManager.controlManager().registerHandler(
//                                                "RESTART",
//                                                "no arguments",
//                                                "Restart the application.",
//                                                onRestart);
//      assert(0 == rc);
//      rc = taskManager.controlManager().registerHandler(
//                                                "LOG",
//                                                "<GET|SET <level> >",
//                                                "Get/set verbosity level.",
//                                                onLog);
//      assert(0 == rc);
//..
// and add an additional handler that provides a list of the registered
// messages and the syntax for using them:
//..
//      rc = taskManager.controlManager().registerUsageHandler(bsl::cout);
//      assert(0 == rc);
//..
// Next, if we are on a Unix system, we confirm that our named pipes will be
// created in the directory named by the 'TMPDIR' environment variable:
//..
//  #if   defined(BSLS_PLATFORM_OS_UNIX)
//      rc = unsetenv("SOCKDIR"); // 'SOCKDIR' has precedence over 'TMPDIR'.
//      assert(0 == rc);
//
//      const char *expectedDirectory = getenv("TMPDIR");
//  #elif defined(BSLS_PLATFORM_OS_WINDOWS)
//      const char *expectedDirectory = "\\\\.\\pipe\\";
//  #else
//      #error "Unexpected platform."
//  #endif
//..
// Then, start listening for incoming messages at pipe having the name based on
// on the name "MyApplication.CTRL".
//..
//      rc = taskManager.start("MyApplication.CTRL");
//      assert(0 == rc);
//..
// Next, for expository purposes, confirm that a pipe of that name exists in
// the expected directory and is open for reading.
//..
//      const bsl::string_view pipeName = taskManager.pipeName();
//
//      assert(bdls::PathUtil::isAbsolute      (pipeName));
//  #ifdef BSLS_PLATFORM_OS_UNIX
//      assert(bdls::PipeUtil::isOpenForReading(pipeName));
//  #endif
//
//      bsl::string canonicalDirname, canonicalLeafName;
//
//      bdls::PathUtil::getDirname(&canonicalDirname,  pipeName);
//      bdls::PathUtil::getLeaf   (&canonicalLeafName, pipeName);
//
//      assert(0 == bsl::strcmp(expectedDirectory, canonicalDirname.c_str()));
//      assert("myapplication.ctrl" == canonicalLeafName);
//..
// Notice that given 'baseName' has been canonically converted to lowercase.
//
// Now, our application can continue doing useful work while the background
// thread monitors the named pipe for incoming messages:
//..
//      while (!done) {
//          // Do useful work while background thread responds to incoming
//          // commands from the named pipe.
//      }
//
//      return 0;
//  }
//..
// Finally, in some other programming context, say 'mySender', a context in
// another process that has been passed the value of 'pipeName', control
// messages can be sent to 'myApplication1' above.
//..
//  void mySender(const bsl::string& pipeName)
//      // Write control messages into the pipe named by the specified
//      // 'pipeName'.
//  {
//      int rc;
//      rc = bdls::PipeUtil::send(pipeName, "LoG GET\n");
//      assert(0 == rc);
//      rc = bdls::PipeUtil::send(pipeName, "Log SET 4\n");
//      assert(0 == rc);
//      rc = bdls::PipeUtil::send(pipeName, "log GET\n");
//      assert(0 == rc);
//      rc = bdls::PipeUtil::send(pipeName, "\n");       // empty
//      assert(0 == rc);
//      rc = bdls::PipeUtil::send(pipeName, "RESET\n");  // invalid
//      assert(0 == rc);
//      rc = bdls::PipeUtil::send(pipeName, "RESTART\n");
//      assert(0 == rc);
//      rc = bdls::PipeUtil::send(pipeName, "EXIT\n");
//      assert(0 == rc);
//  }
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

#include <balscm_version.h>

#include <balb_controlmanager.h>
#include <balb_pipecontrolchannel.h>

#include <bslma_allocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>

#include <bsl_memory.h>   // 'bsl::shared_ptr' 'bsl::allocate_shared'
#include <bsl_string.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace balb {

                        // =====================
                        // class PipeTaskManager
                        // =====================

class PipeTaskManager {
    // This class provides a mechanism route messages received on a named pipe
    // to the registered handler (functor).

    // DATA
    bslma::Allocator                *d_allocator_p;       // allocator (held)
    PipeControlChannel              *d_controlChannel_p;  // message IPC mech.
    bsl::shared_ptr<ControlManager>  d_controlManager_p;  // callback registry

  private:
    // NOT IMPLEMENTED
    PipeTaskManager(const PipeTaskManager& );             // = delete
    PipeTaskManager& operator=(const PipeTaskManager& );  // = delete

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(PipeTaskManager,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit PipeTaskManager(bslma::Allocator *basicAllocator = 0);
        // Create a task manager having no message handlers.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Message handlers can be supplied using the return value of
        // the 'controlManager' method.  If that return value is used to create
        // other 'PipeTaskManager' objects, this object (the owner of the
        // internal 'ControlManager' must outlive those other objects.  The
        // 'start' method must be called successfully before messages will be
        // received (on a separate thread created by 'start').  Note that the
        // name of the message pipe is supplied as an argument to 'start'.

    explicit
    PipeTaskManager(bsl::shared_ptr<ControlManager>&  controlManager,
                    bslma::Allocator                 *basicAllocator = 0);
        // Create a task manager that uses the handlers of the specified shared
        // 'controlManager'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The 'start' method must be called
        // successfully before messages will be received (on a separate thread
        // created by 'start').  Note that the name of the message pipe is
        // supplied as an argument to 'start'.  Also note that the handlers of
        // 'controlManager' can be manipulated via the return value of the
        // 'controlManager' method.  Finally note that the allocator of
        // 'controlManger' need not equal 'basicAllocator'.

    ~PipeTaskManager();
        // Shutdown message handling, and release the shared reference to the
        // 'controlManager' (destroying the 'ConstrolManager' and its message
        // handlers if this is the last shared reference), and destroy this
        // object.

    // MANIPULATORS
    balb::ControlManager& controlManager();
        // Return a non-'const' reference to the 'ControlManager' object of
        // this 'PipeTaskManager'.

// BDE_VERIFY pragma: -FABC01 //  not in alphanumeric order

    int start(const bsl::string_view& pipeBasename);
        // Create a named pipe using the specified 'pipeBasename' (not a
        // pathname), and execute the task manager event processor in a
        // background thread.  Return 0 on success, and a non-zero value
        // otherwise.  See {Requirements for the Named Pipe} for expectations
        // for 'pipeBasename'.  After a successful return, calls to 'start'
        // fail until 'stop' has been called (successfully); afterwards,
        // 'start' can be called again with the same or some other
        // 'pipeBasename'.  Note that the 'pipeName()' method provides the
        // (full) pathname to the created named pipe.

    void shutdown();
        // Stop processing incoming messages by the background thread.  If a
        // handler is executing at the time of invocation, that handler is
        // allowed to complete.  This method does not block the caller, and the
        // background thread persists until it is joined by a call to 'stop'.
        // Note that 'shutdown' and 'stop' are typically called in succession.

    int stop();
        // Block until the background processing thread has shutdown (which
        // must be initiated by a separate call to 'shutdown') then join the
        // background thread, remove the named pipe, and return.  Return 0 on
        // success, and a non-zero value otherwise.  If 'shutdown' has not been
        // called, this method will block indefinitely until another thread
        // calls 'shutdown'.  Once 'stop' has returned, 'start' can be called
        // again with the same or a different named pipe.  Note that frequently
        // user code will call 'shutdown' and then immediately call 'stop' on a
        // 'TaskManager' object.

// BDE_VERIFY pragma: +FABC01 //  not in alphanumeric order

    // ACCESSORS
    const balb::ControlManager& controlManager() const;
        // Return a 'const' reference to the 'ControlManager' object of this
        // 'PipeTaskManager'.

    const bsl::string& pipeName() const;
        // Return the path of the named pipe used by the implementation.  The
        // behavior is undefined unless the task manager has been started.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator in effect at construction is used.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // ---------------------
                         // class PipeTaskManager
                         // ---------------------

// MANIPULATORS
inline
balb::ControlManager& PipeTaskManager::controlManager()
{
    return *d_controlManager_p;
}

inline
void PipeTaskManager::shutdown()
{
    d_controlChannel_p->shutdown();
}

inline
int PipeTaskManager::stop()
{
    d_controlChannel_p->stop();
    return 0;
}

// ACCESSORS
inline
const balb::ControlManager& PipeTaskManager::controlManager() const
{
    return *d_controlManager_p;
}

inline
const bsl::string& PipeTaskManager::pipeName() const
{
    BSLS_ASSERT(d_controlChannel_p);

    return d_controlChannel_p->pipeName();
}

                                  // Aspects

inline
bslma::Allocator *PipeTaskManager::allocator() const
{
    return d_allocator_p;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
