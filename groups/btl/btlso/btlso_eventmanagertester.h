// btlso_eventmanagertester.h                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSO_EVENTMANAGERTESTER
#define INCLUDED_BTLSO_EVENTMANAGERTESTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide programmable and preset test apparatus for event managers.
//
//@CLASSES:
//  btlso::EventManagerTester: namespace for a set of testing utilities
//  btlso::EventManagerTestPair: a pair of locally-connected TCP/IPv4 sockets
//
//@SEE_ALSO:  btlso_eventmanager btlso_defaulteventmanager
//
//@DESCRIPTION: This component provides a test apparatus, suitable for use in
// the *test* *drivers* of implementations derived from 'btlso::EventManager'.
// The apparatus performs both pre-defined and user-programmable tests (i.e.,
// sequences of method calls on the object under test), and thus can perform
// both black-box and white-box testing of event managers.  Methods in this
// component are sufficient to test each virtual method in the
// 'btlso::EventManager' protocol.
//
// The test apparatus consists of the 'btlso::EventManagerTestPair' class,
// which defines a pair of connected sockets to facilitate the testing, a set
// of utility methods to exercise the methods of the event manager under test,
// and a custom language specification that is used as input for the
// "programmable" 'gg' test method.
//
///Synopsis
///--------
// The following test methods are available for testing concrete event managers
// derived from 'btlso::EventManager'.
//..
//        Pre-set Test Methods
//        --------------------
//        testAccessors
//        testDeregisterAll
//        testDeregisterSocket
//        testDeregisterSocketEvent
//        testDispatch
//        testRegisterSocketEvent
//
//        Programmable Test Methods
//        -------------------------
//        gg
//
//        Performance Test Methods
//        ------------------------
//        testDispatchPerformance
//        testRegisterPerformance
//..
// Each pre-set test method performs a set of operations designed to test
// the black-box contract of the corresponding method(s) under test.  The 'gg'
// method takes user input date (in the form of a 'const' 'char*' test script
// "sentence") using a custom "script" language, and is designed to allow
// flexible white-box testing of a particular event manager implementation.
// The script language definition and documentation are given in the "Language
// Specification" section below.  The performance tests have built-in timing
// functions, and report elapsed-time information to an 'ostream' passed in as
// an argument.
//
///Test Apparatus Diagram
///----------------------
// The following diagram indicates the schematic structure of the event manager
// test apparatus.
//..
//         ------
//        | Data |
//         ------
//            |
//            |
//            |
//            V
//     ----------------------------------
//    | TestGenerator | [B1] [B2]...[Bm] | (btlso::EventManagerTester)
//     ----------------------------------
//                | | | | |   Control sockets
//                +-+-++-+
//                    |
//                    |     (Connected Socket Pairs)
//                    =======================================
//                                                          |
//                                                          |
//                                                      +-+-+-+-+
//                                     Observed sockets | | | | |
//                                                   ---------------
//                                                  | Event Manager |
//                                                   ---------------
//
// Data                  : operation information sent to the 'TestGenerator'
//
// TestGenerator         : the execution part in the
//                         'btlso::EventManagerTester' to perform programmable
//                         (customized) test for the event manager based on
//                         data passed in.
//
// [Bm]                  : a control button in the 'btlso::EventManagerTester'
//                         to  perform a pre-set ("canned") test for a function
//                         of the event manager, without the need of any data
//                         to be passed from the outside
//
// Connected Socket Pairs: connections where the event manager works at one
//                         end and the 'btlso::EventManagerTester' verifies
//                         results at the other end
//
// Event Manager         : the event manager under test
//..
//
///Abbreviated Grammar
///-------------------
// The following characters are used to compose expressions within the rules of
// the Language Specification defined in the next section.
//..
//  < >         Delimits a symbol used within an Expression in a rule.  A
//              symbol is a sequence of one or more uppercase letters within
//              this delimiter, e.g., <FD> is a symbol.
//
// ( )          Delimits an expression within a statement.
//
// |            Logical OR.  Indicates that at least one of its operands
//              (expressions) must be present.  May be used to form a compound
//              expression, e.g., ( <A> | <B> | <C> ) would mean that exactly
//              one of the symbols <A>, <B>, and <C> must be present in the
//              expression being defined.
//
// ?            Indicates an optional expression: the expression to the left of
//              the ? must appear zero times or one time.
//
// ;            The semicolon appears as written, and is used to concatenate
//              the execution of otherwise-independent commands.
//
// , { }        These characters are *not* special to this grammar, and must
//              appear as written in the expression being defined.  The ',' is
//              used to separate parts in a command, while the '{' and '}'
//              to indicate a portion of the script that is to be executed in
//              the callback function that is registered with the event manager
//              under test.
//
// a r w c      These characters must appear as written, and represent the
//              four events as enumerated in 'btlso::EventType', i.e.,
//              'ACCEPT', 'READ', 'WRITE', and 'CONNECT', respectively.
//
// T D E R W    These seven characters must appear as written, and represent
// + -          commands as described in the rules defined in the "Language
//              Specification" section, below.
//..
//
///Language Specification
///----------------------
// The Test Language defines a set of possible input strings that translate
// into a sequence of operations (a "sentence") on an objects under test.  The
// script and the test object, referred to as 'mX', are passed to the 'gg'
// method.  The supporting array of socket pairs, referred to as 'fds', must
// also be passed to the 'gg' method (and, thus, managed externally).
//
// The following ten rules define possible sentences.  Various symbols used in
// each rule are defined as needed.
//..
//  1. T<NUM>
//         Verify the number of events currently registered in the event
//         manager 'mX'.
//
//         <NUM> : the number of expected registered events
//
//  2. +<FD><RW_EVENT>(<N>)?({<SCRIPT>})?  |
//     +<FD><AC_EVENT>({<SCRIPT>})?
//         Register a socket event,
//                <FD> : an index into the array 'fds' of socket pairs
//         <RW_EVENT> : ( r | w )
//                       'r' => btlso::EventType::e_READ
//                       'w' => btlso::EventType::e_WRITE
//         <AC_EVENT> : ( a | c )
//                       'a' => btlso::EventType::e_ACCEPT
//                       'c' => btlso::EventType::e_CONNECT
//                 <N> : the optional number of bytes for 'r'ead or 'w'rite
//                       operation on the observed socket ('fds[fd]').
//            <SCRIPT> : the optional test script to be executed by the
//                       registered callback; <SCRIPT> cannot contain 'D'
//                       (dispatch) commands
//
//  3. '-'<FD><EVENT>
//     Deregister an <EVENT> event associated with the observed socket in the
//     socket pair fds[<FD>],
//               <FD>:  the index into the array 'fds' of socket pairs
//           <EVENT> := 'a'|'c'|'r'|'w' :
//                      'a' => btlso::EventType::e_ACCEPT
//                      'c' => btlso::EventType::e_CONNECT
//                      'r' => btlso::EventType::e_READ
//                      'w' => btlso::EventType::e_WRITE
//
//  4. '-'<FD>
//     Deregister all events associated with the observed socket in the
//     socket pairs fds[<FD>],
//             <FD>: the index into the array of socket pairs
//
//  5. '-''a'
//     Deregister all events registered in the event manager,
//
//  6. 'D'<I>,<RC>
//     Dispatch events without specifying a timeout value,
//           <I> = 'n' | 'i' : 'n' => flags = 0
//                             'i' => flags = bteso_Flag::k_ASYNC_INTERRUPT
//           <RC> : the expected return value from the 'dispatch' method.
//
//  7. 'D'<I><MILLISECOND>,<RC>
//     Dispatch events with the specified 'millisecond' timeout value,
//     where <I> = 'n' | 'i' :
//                             'n' => flags = 0
//                             'i' => flags = bteso_Flag::k_ASYNC_INTERRUPT
//           <MILLISECOND>   : a relative timeout in milliseconds
//                             (negative and zero values are also allowed)
//           <RC>            : the expected return value of the dispatch
//
//  8. 'E'<FD><EVENT>
//     Verify the number of registered events associated with the observed
//     socket in the socket pair fds[<FD>],
//           <FD>    :  the index into the array 'fds' of socket pairs
//
//           <EVENT> := 'a'|'c'|'r'|'w'|'rw'|'wr' :
//                'a' => btlso::EventType::e_ACCEPT
//                'c' => btlso::EventType::e_CONNECT
//                'r' => btlso::EventType::e_READ
//                'w' => btlso::EventType::e_WRITE
//                'rw' = 'r' and 'w'
//                'wr' = 'r' and 'w'
//  9. 'R'<FD>,<N>
//     Read <N> bytes from the observed socket of socket pair fds[<FD>],
//           <FD>: the index into the array of socket pairs
//           <N> : the number of bytes to be read from the socket
//
// 10. 'W'<FD>,<N>
//     Write <N> bytes into the control socket of socket pair fds[<FD>],
//           <FD>: the index into the array of socket pairs
//           <N> : the number of bytes to be written into the control socket
//
// 11. 'S'<T>
//     Sleep for <T> milliseconds
//           <T> : the number of milliseconds to sleep for.  Bear in mind the
//                 minimum resolution of sleeping on many platforms is 10
//                 milliseconds.
//..
// The following examples interpret a given script in the order from left to
// right to configure a list of I/O request commands for an event manager test
// according to the custom language defined above.  By parsing a request script
// that is based on this language, each I/O request command, which is separated
// with a ';' from other commands, will be executed and the result can then be
// verified.
//
// Note that any command or sequence of commands that is enclosed in a pair of
// '{' and '}' are to be executed in the user-installed callback function.
// This kind of command could be any one of the above except the dispatch
// command 'D'.
//
// Script examples based on the above language are as follows, 'fds' is the
// array of connected socket pairs, one of which is the control socket and
// another one is the observed socket.  Only the observed socket in a socket
// pair can register events.
//..
// "T3; T1,2"
//    -- Check if the total number of socket events is 3; the observed socket
//       of fds[1] registered 2 events.
// "+1w2; +3r4"
//    -- Register an event of "write to the observed socket of fds[1] 2 bytes";
//       then register an event of "read from the observed socket of fds[3] 4
//       bytes".
// "-1w; -0"
//    -- Deregister the write event for the observed socket of fds[1]; then
//       deregister all events for the observed socket of fds[0].
// "Dn,1"
//    -- Call dispatch on the event manager under test with flags set to 0;
//       there should be 1 event to be dispatched.
// "Di100,2"
//    -- Call dispatch on the event manager under test with flags set to
//       bteso_Flag::k_ASYNC_INTERRUPT, the timeout value for the dispatch
//       is 100 milliseconds, there are 2 events to be dispatched.
// "E1a"
//    -- Verify that an "ACCEPT" event has been registered for the observed
//       socket of fds[1].
// "E2rw"
//    -- Verify that a read and a write event have been registered for the
//       observed socket of fds[2].
// "R0,15"
//    -- Read from the observed socket of fds[0] 15 bytes.
//
// "W1,32"
//    -- Write to the control socket of fds[1] 32 bytes.
//
// "+1c{+1w20}"
//    -- Register a connect event on 'fds[1]' and, when this event occurs,
//       in the callback, register 'fds[1]' for writing and write 20 bytes to
//       it, when ready.
//
// "+1w20{+0r18}"
//    -- Register 'fds[1]' for a write event; when this event occurs, in
//       the callback, write 20 bytes and register for read event on 'fds[0]'.
//       Read 18 bytes when read event occurs on 'fds[0]'.
//..
//
///Thread Safety
///-------------
// The thread-safety of 'btlso::EventManagerTester' depends on the event
// manager to be tested.  If the event manager is not thread safe (thread
// enabled) then the 'btlso::EventManagerTester' will consequently not be
// thread safe (thread enabled).  When operations are invoked on a function of
// the 'btlso::EventManagerTester' with the same instance of an event manager
// under test from different threads, the 'btlso::EventManagerTester' is *not*
// *thread* *safe*.  When operations are invoked on a function of the
// 'btlso::EventManagerTester' with *distinct* *instances* of a *thread* *safe*
// (thread enabled) event manager under test from different threads, the
// 'btlso::EventManagerTester' *is* *thread* *safe* (thread enabled).  This
// 'btlso::EventManagerTester' is not *async-safe*, meaning that one or more
// functions cannot be invoked safely from a signal handler.
//
///Performance Testing
///-------------------
// This component enables users to test the performance of the 'dispatch' and
// 'registerSocketEvent' methods of an event manager.  The performance is
// measured by the time taken to finish a call to the function under test.  The
// overall performance of a function may be tested by varying input parameters
// such as the number of connections, socket handles, etc.
//
// The 'registerSocketEvent' method performance test proceeds as follows:
//
//: 1 In an outer loop over the requested number of socket pairs:
//:   1 Create a socket pair.
//:   2 In an inner loop over the requested number of measurements:
//:     1 Record the current time ('bdlt::CurrentTime::now()').
//:     2 Call the 'registerSocketEvent' method.
//:     3 Calculate and record the elapsed time.
//:   3 Write the average elapsed time to the passed-in 'ostream'.
//: 2 Clean up and free resources.
//
// The 'dispatch' method performance test proceeds as follows:
//
//: 1 In an outer loop over the requested number of socket pairs:
//:   1 Create a socket pair.
//:   2 Register a 'READ' event for the "observed" socket in the pair.
//:   3 In an inner loop over the requested number of measurements:
//:     1 Record the current time ('bdlt::CurrentTime::now()').
//:     2 Call the 'dispatch' method.
//:     3 Calculate and record the elapsed time.
//:   4 Write the average elapsed time to the passed-in 'ostream'.
//: 2 Clean up and free resources.
//
// The implementation ensures that only one 'READ' event is dispatched at a
// time by writing only one byte to the "control" socket, whose counterpart,
// the "observed" socket's 'READ' event is to be dispatched.
//
///Control Flags for Test Methods
///------------------------------
// Each test method accepts a 'flags' argument whose value is created by
// bitwise-ORing the following set of enumerations.
//..
//      Enumerator
//      ----------
//      k_VERBOSE
//      k_VERY_VERBOSE
//      k_VERY_VERY_VERBOSE
//      k_ABORT
//      k_DRY_RUN
//..
// The three 'k_VERBOSE*' flags indicate increasing amounts of diagnostic
// printout during the test method operation.  The 'ABORT' flag indicates that
// the test method should terminate after the first occurrence of a test error.
// The 'DRY_RUN' flag indicates that no event manager tests should be
// performed, but that any input test script will be echoed to 'stdout'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Testing 'registerSocketEvent' method
///- - - - - - - - - - - - - - - - - - - - - - - -
// In this usage example, block-box test for 'registerSocketEvent' is performed
// for an event manager class, 'my_FastEventManager'.  First, create an object
// under test:
//..
//  my_FastEventManager mX;
//..
// Second, set the bit flags as needed to control the execution and call the
// appropriate utility function to perform the test:
//..
//  int ctrlFlag = 0;
//  ctrlFlag |= btlso::EventManagerTester::k_VERY_VERBOSE;
//  ctrlFlag |= btlso::EventManagerTester::k_ABORT;
//  btlso::EventManagerTester::testRegisterSocketEvent(&mX, ctrlFlag);
//..
//
///Example 2: Testing Using Test Specification
///- - - - - - - - - - - - - - - - - - - - - -
// Because different event managers may be implemented differently, it's
// important to make sure the test covers all possible situations based on the
// specific implementation for the event manager.  The following snippets of
// code illustrate how to use this utility component to perform a white-box
// (customized) test specifically for the event manager being tested.  First,
// create an array of four 'btlso::EventManagerTestPair' objects to be used in
// the test script.  Each of these objects is a pair of connected sockets as
// shown in the "Test Apparatus Diagram" section.
//..
//  enum { NUM_PAIRS = 4 };  // arbitrary number for illustration
//  btlso::EventManagerTestPair  socketPairs[NUM_PAIRS];
//..
// Second, define a white-box (customized) test script, designed based on
// white-box knowledge of the implementation under test:
//..
//  const char *script = "+0w8{-0w}; +0r1; +3r20; +2r14; +3w20; -a; T0";
//  const int LINE = __LINE__;
//..
// Then create an instance, 'mX1', of the event manager under test, set the
// bit flags to control execution, and invoke 'gg()' to exercise the test
// script:
//..
//  my_FastEventManager mX1;
//  int ctrlFlag = 0;
//  ctrlFlag |= btlso::EventManagerTester::k_VERBOSE;
//  ctrlFlag |= btlso::EventManagerTester::k_ABORT;    // abort on failure
//  int failures = btlso::EventManagerTester::gg(&mX1,
//                                               socketPairs,
//                                               LINE,
//                                               script, ctrlFlag);
//  assert(0 == failures);
//..

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BTLSO_SOCKETHANDLE
#include <btlso_sockethandle.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace bsls { class TimeInterval; }

namespace btlso {

class EventManager;

                        // ==========================
                        // class EventManagerTestPair
                        // ==========================

class EventManagerTestPair {
    // This class provides a pair of connected sockets to facilitate testing.
    // One socket can be used to generate control events, while another can be
    // used to observe the results.  By default, the sockets are non-blocking
    // with the Nagle algorithm turned off.

    SocketHandle::Handle d_fds[2];      // sockets' descriptors

    int                  d_validFlag;   // flag indicating whether the socket
                                        // pair is valid

    int                  d_verboseFlag; // whether to log the states to
                                        // 'stdout'
    // NOT IMPLEMENTED
    EventManagerTestPair(const EventManagerTestPair& original);
    EventManagerTestPair operator=(const EventManagerTestPair& rhs);

  public:
    // CREATORS
    EventManagerTestPair(int verboseFlag = 0);
        // Create a pair of locally-connected TCP/IPv4 sockets.  Set each
        // socket to non-blocking mode and turn off the Nagle algorithm (i.e.,
        // set the 'TCP_NODELAY' flag to 1).  Optionally specify 'verboseFlag'.
        // If 'verboseFlag' is not 0, the detailed information about underlying
        // calls is printed to 'stdout' along with additional debugging
        // information.  By default, i.e., when 'verboseFlag' is 0, no messages
        // are displayed.

    ~EventManagerTestPair();
        // Close the managed sockets and destroy the object.

    // MANIPULATORS
    int setControlBufferOptions(int bufferSize, int watermark);
        // Set the send and receive buffer sizes and watermarks on the control
        // socket handle (as reported by the 'controlFd' method) to the
        // specified 'bufferSize' and 'watermark' values respectively.  Return
        // 0 on success, and a non-zero value otherwise.

    int setObservedBufferOptions(int bufferSize, int watermark);
        // Set the send and receive buffer sizes and watermarks on the observed
        // socket handle (as reported by the 'observedFd' method) to the
        // specified 'bufferSize' and 'watermark' values respectively.  Return
        // 0 on success, and a non-zero value otherwise.

    // ACCESSORS
    SocketHandle::Handle controlFd() const;
        // Return the handle for the control socket.  Note that this handle may
        // be used to control the connection pipe.

    int getControlBufferOptions(int *sndBufferSize,
                                int *rcvBufferSize,
                                int *sndLowat,
                                int *rcvLowat) const;
        // Load the control socket's send and receive buffer sizes and
        // watermarks into the specified 'sndBufferSize', 'rcvBufferSize',
        // 'sndLowat' and 'rcvLowat' respectively.  Return 0, on success, and a
        // negative value otherwise.  The result is undefined if any of
        // 'sndBufferSize', 'rcvBufferSize', 'sndLowat' or 'rcvLowat' is 0.

    int getObservedBufferOptions(int *sndBufferSize,
                                 int *rcvBufferSize,
                                 int *sndLowat,
                                 int *rcvLowat) const;
        // Load the observed socket's send and receive buffer sizes and
        // watermarks into the specified 'sndBufferSize', 'rcvBufferSize',
        // 'sndLowat' and 'rcvLowat' respectively.  Return 0, on success, and a
        // negative value otherwise.  The result is undefined if any of
        // 'sndBufferSize', 'rcvBufferSize', 'sndLowat' or 'rcvLowat' is 0.

    bool isValid() const;
        // Return 'true' if this socket pair is valid, and 'false' otherwise.

    SocketHandle::Handle observedFd() const;
        // Return the handle for the observed socket.  Note that this handle
        // may be used to observe test results.
};

                        // ========================
                        // class EventManagerTester
                        // ========================

struct EventManagerTester {
    // This class provides in one place the implementation to test different
    // event managers conveniently.  By providing a generic test script
    // interpreting function, gg(), every function in an event manager can be
    // tested.

    enum {
        k_VERBOSE           = 1,  // If this flag is on, print the name of
                                  // functions under test.

        k_VERY_VERBOSE      = 2,  // If this flag is on, print the test script
                                  // to be executed and execution results after
                                  // executing each command of the test script.

        k_VERY_VERY_VERBOSE = 4,  // If this flag is on, print all values after
                                  // executing each command of a test script.

        k_ABORT             = 8,  // If this flag is on, the test will be
                                  // aborted upon an error.

        k_DRY_RUN           = 16  // If this flag is on, the test will not
                                  // execute the test script, but only parse
                                  // the test script to the standard output.

    };

    static int gg(EventManager         *eventManager,
                  EventManagerTestPair *fds,
                  const char           *script,
                  int                   flags);
        // Execute the specified 'script' to test the specified 'eventManager',
        // using the specified 'fds' array of connected socket pair and the
        // specified 'flags' to control execution.  Return the number of
        // failures detected.  The behavior is undefined unless 'fds' refers to
        // the sufficient number of sockets pairs as required by 'script'.
        // Note that if the 'ABORT' bit is set in 'flags', a detected failure
        // will force the test to abort.

    static int testAccessors(EventManager *eventManager, int flags);
        // Exercise a pre-defined ("canned") test of the accessor methods
        // 'numSocketEvents', 'numEvents' and 'isRegistered' of the specified
        // 'eventManager' using the specified 'flags' to control execution.
        // Return the number of failures detected.  Note that if the 'ABORT'
        // bit is set in 'flags', a detected failure will force the test to
        // abort.

    static int testDeregisterAll(EventManager *eventManager, int flags);
        // Exercise a pre-defined ("canned") test of the 'deregisterAll' method
        // of the specified 'eventManager' using the specified 'flags' to
        // control execution.  Return the number of failures detected.  Note
        // that if the 'ABORT' bit is set in 'flags', a detected failure will
        // force the test to abort.

    static int testDeregisterSocket(EventManager *eventManager,
                                    int           flags);
        // Exercise a pre-defined ("canned") test of the 'deregisterSocket'
        // method of the specified 'eventManager' using the specified 'flags'
        // to control execution.  Return the number of failures detected.  Note
        // that if the 'ABORT' bit flag is specified in 'flags', a detected
        // failure will force the test to abort.

    static int testDeregisterSocketEvent(EventManager *eventManager,
                                         int           flags);
        // Exercise a pre-defined ("canned") test of the
        // 'deregisterSocketEvent' method of the specified 'eventManager' using
        // the specified 'flags' to control execution.  Return the number of
        // failures detected.  Note that if the 'ABORT' bit flag is specified
        // in 'flags', a detected failure will force the test to abort.

    static int testDispatch(EventManager *eventManager, int flags);
        // Exercise a pre-defined ("canned") test of the 'dispatch' methods of
        // the specified 'eventManager' using the specified 'flags' to control
        // execution.  Return the number of failures detected.  Note that if
        // the 'ABORT' bit is set in 'flags', a detected failure will force the
        // test to abort.

    static int testRegisterSocketEvent(EventManager *eventManager,
                                       int           flags);
        // Exercise a pre-defined ("canned") test of the 'registerSocketEvent'
        // method of the specified 'eventManager' using the specified 'flags'
        // to control execution.  Return the number of failures detected.  Note
        // that if the 'ABORT' bit is set in 'flags', a detected failure will
        // force the test to abort.

    static int testDispatchPerformance(EventManager *mX,
                                       const char   *pollingMechName,
                                       int           flags);
        // Test the performance of the 'mX->dispatch' function.  Specify
        // 'pollingMechName' which reflects the name of the type of event
        // manager used.  4 paramters are read interactively from 'cin':
        // 'numPairs', the number of socket pairs listened to, a double
        // specifying the fraction of those socket pairs that have data waiting
        // (a value of 0.0 means that one socket has data waiting), a double
        // which is the seconds to timeout (a value of 0.0 means that no
        // timeout is specified), and whether reads on those sockets with data
        // waiting are to be timed as part of the performance test.  Note that
        // those sockets that have data waiting will have one byte of data
        // apiece.  A file name will be created that reflects the values of
        // 'pollingMechName' and the parameters interactively specified, and
        // that file will contain the results, which will be the test run with
        // values of 'numPairs' ranging from 'numPairs - 9' to 'numPairs', and
        // the microseconds taken per dispatch, one value per line.  Specify
        // verbosity in 'flags' with verbosity flags defined in this class.
        // Return 0 on success and a non-zero value if any failures occurred.

    static int testRegisterPerformance(EventManager *mX, int flags);
        // Test the performance of the 'registerSocketEvent' method of the
        // specified eventManager '*mX', prompting the user interactively for
        // int 'numSockets' and double 'fractionRegistered', registering
        // 'fractionRegistered * numSockets' prior to registering one socket
        // and timing that registration.  Repeat the experiment 10 times,
        // reporting the # of microseconds taken to 'cout'.  Or verbosity bits
        // together in 'flags.  Return 0 on success and a non-zero value
        // otherwise.
};

//-----------------------------------------------------------------------------
//                      INLINE FUNCTION DEFINITIONS
//-----------------------------------------------------------------------------

                        // ----------------------------
                        // class EventManagerTesterPair
                        // ----------------------------

inline
SocketHandle::Handle EventManagerTestPair::controlFd() const
{
    return d_fds[1];
}

inline
bool EventManagerTestPair::isValid() const
{
    return d_validFlag >= 0;
}

inline
SocketHandle::Handle EventManagerTestPair::observedFd() const
{
    return d_fds[0];
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
