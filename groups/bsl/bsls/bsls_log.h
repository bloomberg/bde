// bsls_log.h                                                         -*-C++-*-
#ifndef INCLUDED_BSLS_LOG
#define INCLUDED_BSLS_LOG

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a common location for low-level code to write logs.
//
//@CLASSES:
//  bsls::Log: utilities for low-level logging
//
//@DESCRIPTION: This component provides a namespace, 'bsls::Log', which
// contains a suite of utility functions for logging in low-level code.  The
// component also provides a set of macros that further simplify the low-level
// logging process.  This facility is intended to be used only when a more
// fully-featured logger is not available.
//
///Motivation
///----------
// This component provides a mechanism for installing and invoking a global log
// message handler function through a unified interface.  This interface is
// intended as a replacement for instances in low-level code in which a log
// message is written directly to an output stream such as 'stderr'.
//
// Outputting directly to a hard-coded stream is problematic in many ways.
// First, hard-coding the stream does not give users any freedom to control log
// messages.  A user may want all logs to be automatically redirected to a file
// or may want to add some custom formatting or handling.  These freedoms are
// not available if the output stream is predetermined.  Second, Windows
// applications running in non-console mode do not have a concept of 'stdout'
// or 'stderr'.  Writing directly to either of these streams is known to hang
// the process on certain Windows systems when no console is attached.  Third,
// a better overall logging system would encourage a greater number of useful
// logs to be written where they were originally omitted, possibly revealing
// previously unknown issues in code.
//
///Functionality
///-------------
// This section describes the full functionality provided by this component.
//
///Log Message Handlers
/// - - - - - - - - - -
// The class 'bsls::Log' provides a mechanism for installing and invoking a
// global log message handler function through a simple interface.
//
// Log message handler functions must have a signature that conforms to the
// 'bsls::Log::LogMessageHandler' 'typedef', which corresponds to a
// 'void'-valued function taking as input three parameters: the string name of
// the file from which the log was written, the line number at which the log
// was written, and the log message string.
//
///Invocation of the Installed Log Message Handler
///- - - - - - - - - - - - - - - - - - - - - - - -
// The dispatch method 'bsls::Log::logMessage' is provided as the most direct,
// safe, and efficient means of invoking the currently installed log message
// handler with the provided file name, line number, and message string.  It
// can be used as in the following expression:
//..
//  bsls::Log::logMessage(__FILE__, __LINE__, "Function 'f' is deprecated.");
//..
//
// The method 'bsls::Log::logFormatted' takes a format string and a set of
// variadic arguments instead of a raw message string.  This method will format
// the format string with the variadic argument substitutions according to the
// 'printf'-style rules and will subsequently invoke the currently installed
// log message handler with the provided file name and line number, along with
// the newly formatted string.  *Note:* Passing an arbitrary string as the
// format string is dangerous and can lead to undefined behavior.  The method
// may be used as in the following example:
//..
//  int x = 3;
//  const char *s = "Hello World";
//  bsls::Log::logFormatted(__FILE__, __LINE__, "Invalid String %d: %s", x, s);
//..
//
// The macro 'BSLS_LOG_SIMPLE' is provided.  This macro behaves exactly like
// 'bsls::Log::logMessage' except that the file name and line number parameters
// are automatically filled in with '__FILE__' and '__LINE__', respectively.
// Therefore, the 'BSLS_LOG_SIMPLE' macro takes a single parameter, which is
// the expected log message string:
//..
//  BSLS_LOG_SIMPLE("Error: Double initialization of singleton.");
//..
//
// Finally, the macro 'BSLS_LOG' is provided.  This macro behaves exactly like
// 'bsls::Log::logFormatted' except that the file name and line number
// parameters are automatically filled in with '__FILE__' and '__LINE__',
// respectively.  The 'BSLS_LOG' macro takes the format string as the first
// parameter, with the expected variadic substitutions as the subsequent
// parameters.  The 'BSLS_LOG' macro may be used as in the following example:
//..
//  int user = 5;
//  int admin = 7;
//  BSLS_LOG("User level %d does not have admin rights (%d).", user, admin);
//..
//
///Installation of Log Message Handlers
/// - - - - - - - - - - - - - - - - - -
// The currently installed log message handler function may be replaced by
// using the 'static' method 'bsls::Log::setLogMessageHandler' which takes, as
// a parameter, a single function pointer of type
// 'bsls::Log::LogMessageHandler'.
//
// Installing a log message handler is usually not necessary.  The default
// handler that is installed is usually the best handler available in a given
// moment.  Additionally, this component is designed to allow higher-level
// logging facilities to install their own low-level log message handlers that
// redirect all low-level logs to the higher-level facility.  Installing one's
// own handler may interfere with this behavior.
//
// As an example of a situation in which a user might want to install a log
// message handler function manually, suppose that the user wants to completely
// silence all low-level log messages.  This can be done by defining a log
// message handler that ignores all input messages, and later installing it:
//..
//  void ignoreMessage(const char *file, int line, const char *message)
//      // Ignore the specified 'file', the specified 'line', and the specified
//      // 'message'.  The behavior is undefined unless 'file' is a
//      // null-terminated string, 'line' is not negative, and 'message' is a
//      // null-terminated string.
//  {
//      BSLS_ASSERT(file);
//      BSLS_ASSERT(line >= 0);
//      BSLS_ASSERT(message);
//      return;
//  }
//..
//
// We may then install the handler:
//..
//  bsls::Log::setLogMessageHandler(&ignoreMessage);
//..
//
// If we attempt to write a low-level log, it will now be ignored:
//..
//  BSLS_LOG_SIMPLE("This message will be ignored");
//..
//
// Note that log message handlers may be invoked from different threads at the
// same time.  Ensure that handlers themselves are thread-safe if the program
// may use multiple threads.
//
///Standard Log Message Handlers
///- - - - - - - - - - - - - - -
// The 'static' methods 'bsls::Log::stdoutMessageHandler' and
// 'bsls::Log::stderrMessageHandler' are provided as a set of simple standard
// log message handlers.  'bsls::Log::stdoutMessageHandler' writes, in the
// following sequence, the file name, a colon character, the line number, a
// space character, the log message string, and a newline to the 'stdout'
// output stream.  'bsls::stderrMessageHandler' writes a string to the 'stderr'
// output stream with the same format as the string written by
// 'stdoutMessageHandler'.
//
// Suppose that the user wants to redirect all low-level logs to 'stdout'.
// This can be done by installing the standard handler
// 'bsls::Log::stdoutMessageHandler' as the log message handler:
//..
//  bsls::Log::setLogMessageHandler(&bsls::Log::stdoutMessageHandler);
//..
//
// Now, writing a low-level log message will result in a log printed to
// 'stdout':
//..
//  int x = 3;
//  BSLS_LOG("This message will be printed to stdout (x=%d)", x);
//..
//
// If the above logging call were to have occurred on line 7 of a file called
// 'myFile.cpp' , the following line would be printed to 'stdout', including a
// single newline:
//..
//  myfile.cpp:7 This message will be printed to stdout (x=3)
//..
//
// In addition to the simple handlers, the 'static' method
// 'bsls::Log::platformDefaultMessageHandler' is provided.  This method has
// platform-specific behavior.  Under non-Windows systems, this handler will
// simply ensure that all log messages are output to 'stderr', in the same
// format as the standard handler 'bsls::Log::stderrMessageHandler'.  On
// Windows, the method will check if the current process is running in console
// mode or non-console mode.  If the process is running in console mode, the
// method will behave exactly as in the non-Windows case: logs will be output
// to 'stderr', followed by a new line.  If the current process is running in
// non-console mode, messages will be written to the Windows debugger, in an
// format equivalent to the string written by the standard handler
// 'bsls::Log::stderrMessageHandler'.  In table form, this is described as
// follows:
//..
//  ========================================================================
//                                               Destination of
//    Platform      Console Mode    bsls::Log::platformDefaultMessageHandler
//  ------------    ------------    ----------------------------------------
//  Non-Windows     Undefined                       'stderr'
//  Windows         Console                         'stderr'
//  Windows         Non-Console                 Windows Debugger
//  ========================================================================
//..
//
///Default Log Message Handler
///- - - - - - - - - - - - - -
// The user does not need to do any set-up to write logs.  By default, the log
// message handler 'bsls::Log::platformDefaultMessageHandler' will be
// immediately available as the global log message handler.
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Logging Formatted Messages
///- - - - - - - - - - - - - - - - - - -
// Suppose that we are writing a function with certain input requirements.  If
// these requirements are not met, we may want to write a log alerting the
// client to the invalid input.
//
// First, we define a function, 'add', which will return the sum of two
// positive integer values:
//..
//  // myapp.cpp
//
//  unsigned int add(int a, int b)
//      // Return the sum of the specified 'a' and the specified 'b'.  The
//      // behavior is undefined unless 'a' and 'b' are not negative.
//  {
//      unsigned int res;
//      if(a >= 0 && b >= 0) {
//          res = static_cast<unsigned int>(a) + static_cast<unsigned int>(b);
//      } else {
//..
//
// If any of the input is invalid, we will want to alert the user to the error.
// We will use the macro 'BSLS_LOG' and will format the erroneous input values
// into the log message:
//..
//          BSLS_LOG("Error: Invalid input combination (%d, %d).", a, b);
//          res = 0;
//      }
//
//      return res;
//  }
//..
// The user might then call the 'add' function as follows:
//..
//  unsigned int x = add(3, -100);
//..
//
// Assuming the default log message handler is the currently installed handler,
// the following line would be printed to 'stderr' or to the Windows debugger:
//..
//  myapp.cpp:8 Error: Invalid input combination (3, -100).
//..
//
// !Note:! Never pass an arbitrary string as the format string to 'BSLS_LOG'.
// If the string happens to contain 'printf'-style format specifiers but the
// expected substitutions are not present, it will lead to undefined behavior.
//
///Example 2: Logging Formatless Messages
/// - - - - - - - - - - - - - - - - - - -
// Suppose we want to log a string that is not meant to be a 'printf'-style
// format string.
//
// First, we define a global association of error codes with error strings:
//..
//  // myapp.cpp
//
//  static const char *errorStrings[4] = {
//      "Invalid input value 'a'.",
//      "Invalid input value 'b'.",
//      "Percentages add up to more than 100%.",
//      "Please use '%2f' for a slash character in a URI."
//  };
//..
// Notice that the fourth string has a sequence that could be misinterpreted as
// a 'printf'-style format specifier.
//
// Next, we define a function that handles error codes and logs an error based
// on the error code:
//..
//  void handleError(size_t code)
//      // Log the error message associated with the specified 'code'. The
//      // behavior is undefined unless 'code' is in the range [0 .. 3].
//  {
//      BSLS_ASSERT(code < sizeof(errorStringsNew)/sizeof(errorStringsNew[0]));
//..
//
// In the case that we receive a valid error code, we would want to log the
// string associated with this code.  We use the macro 'BSLS_LOG_SIMPLE' to
// ensure that the true strings are logged and are not interpreted as format
// strings:
//..
//      BSLS_LOG_SIMPLE(errorStringsNew[code]);
//  }
//..
//
// A user may attempt to use error code '3':
//..
//  handleError(3);
//..
//
// Assuming the default log message handler is the currently installed handler,
// the following line would be printed to 'stderr' or to the Windows debugger:
//..
//  myapp.cpp:12 Please use '%2f' for a slash character in a URI.
//..
//
///Example 3: Using a Different File Name or Line Number
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to define an error handling function that will use the
// logger to print an error message, except that we want to use the file name
// and line number of the code that *called* the error handling function
// instead of the file name and line number of the code *in* the error handling
// function.
//
// First, we will mimic Example 2 by defining a global array of error strings:
//..
//  // mylib.cpp
//
//  static const char *errorStringsNew[4] = {
//      "Bad input attempt.",
//      "Invalid types.",
//      "Invalid username.",
//      "Invalid password."
//  };
//..
//
// Next, we will define a function that takes in a file name and line number
// along with the error code:
//..
//  void handleErrorFlexible(const char *file, int line, size_t code)
//      // Log the error message associated with the specified 'code', using
//      // the specified 'file' and the specified 'line' as the source location
//      // for the error.  The behavior is undefined unless 'file' is a
//      // null-terminated string, 'line' is not negative, and 'code' is in the
//      // range [0 .. 3].
//  {
//      BSLS_ASSERT(file);
//      BSLS_ASSERT(line >= 0);
//      BSLS_ASSERT(code < sizeof(errorStrings)/sizeof(errorStrings[0]));
//..
//
// We can bypass the macros by calling the function 'bsls::Log::logMessage'
// directly, allowing us to pass in the given file name and line number:
//..
//      bsls::Log::logMessage(file, line, errorStrings[code]);
//  }
//..
//
// A user in a different file may now specify the original source of an error:
//..
//  handleErrorFlexible(__FILE__, __LINE__, 2);
//..
//
// If this line of code were placed on line 5 of the file 'otherapp.cpp', the
// following line would be printed to 'stderr' or to the Windows debugger:
//..
//  otherapp.cpp:5 Invalid username.
//..
//
// Users may wrap their error function in a macro to automatically fill in the
// file name and line number parameters, as done by the 'BSLS_LOG*' macros.

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

                         // ==========================
                         // BSLS_LOG Macro Definitions
                         // ==========================
#define BSLS_LOG(...) \
        (BloombergLP::bsls::Log::logFormatted(__FILE__, __LINE__, __VA_ARGS__))
    // The 'BSLS_LOG' macro accepts a 'printf'-style format string as the first
    // parameter.  All subsequent parameters are the variadic substitutions
    // expected by the 'printf'-style format string.  This macro will invoke
    // the 'static' method 'bsls::Log::logFormatted' with the file name and
    // line number of the point of expansion of the macro as the first two
    // parameters, with the format string and variadic arguments as the
    // subsequent parameters.  Note that use of this macro with an arbitrary
    // format string is dangerous.  To log an arbitrary string that is not
    // intended as a 'printf'-style format string, use the 'BSLS_LOG_SIMPLE'
    // macro.

#define BSLS_LOG_SIMPLE(msg) \
                (BloombergLP::bsls::Log::logMessage(__FILE__, __LINE__, (msg)))
    // Write the specified null-terminated string 'message' to the currently
    // installed log message handler function, with the file name and line
    // number parameters automatically input as the file name and line number
    // of the point of expansion of the macro.

namespace BloombergLP {
namespace bsls {

                         // =========
                         // class Log
                         // =========

class Log {
    // This class serves as a namespace containing a suite of utility functions
    // that allow low-level code to write log messages to a globally-controlled
    // destination, which is configurable as well by the utility functions in
    // this class.

  public:
    // TYPES
    typedef void (*LogMessageHandler)(const char *file,
                                      int         line,
                                      const char *message);
        // 'LogMessageHandler' represents a pointer to a function that handles
        // log messages in any desirable way, such as outputting to a stream or
        // a file.  Log message handlers are liable to be called concurrently
        // from different threads, so they should be thread-safe.

  private:
    // CLASS DATA
    static bsls::AtomicOperations::AtomicTypes::Pointer s_logMessageHandler;
        // 's_logMessageHandler' represents the currently installed log message
        // handler.

  public:
    // CLASS METHODS

                         // Administrative Methods

    static Log::LogMessageHandler logMessageHandler();
        // Return the address of the currently installed log message handler.

    static void setLogMessageHandler(Log::LogMessageHandler handler);
        // Install the specified 'handler' as the current log message handler.


                         // Dispatcher Methods

    static void logFormatted(const char *file,
                             int         line,
                             const char *format,
                             ...);
        // Invoke the currently installed log message handler with the
        // specified 'file' as the first parameter and the specified 'line' as
        // the second parameter.  The third parameter to the handler is defined
        // as the formatted string written to the sufficiently large buffer
        // 'buf' by the following C function call: 'sprintf(buf, fmt, ...)',
        // where 'fmt' corresponds to the specified 'format', and '...'
        // represents the specified variadic arguments as if they were passed
        // directly to the hypothetical 'sprintf' function call.  The behavior
        // is undefined unless: (a) file is a null-terminated C-style string,
        // (b) 'line' is not negative, (c) 'format' is a null-terminated
        // C-style string, and (d) the number and types of variadic arguments
        // passed to this function are as expected by their 'printf'-style
        // substitutions in the string 'format'.

    static void logMessage(const char *file, int line, const char *message);
        // Invoke the currently installed log message handler with the
        // specified 'file' as the first parameter, the specified 'line' as the
        // second parameter, and the specified 'message' as the third
        // parameter.  The behavior is undefined unless 'file' is a
        // null-terminated C-style string, 'line' is not negative, and
        // 'message' is a null-terminated C-style string.

                         // Standard Log Message Handlers

    static void platformDefaultMessageHandler(const char *file,
                                              int         line,
                                              const char *message);
        // (Default Handler) Write, to a platform-specific destination, a
        // string (henceforth labeled 'final_string') composed as the
        // sequential concatenation of the following components: (a) the
        // specified 'file' name, (b) a colon character, (c) the single
        // canonical decimal representation of the specified 'line' containing
        // the minimal number of zero digits, (d) a space character, (e) the
        // specified 'message', and (f) a new line.  On *non-Windows* systems,
        // write the aforementioned string 'final_string' to the 'stderr'
        // output stream.  On *Windows* systems: If the current process is
        // running in *console* *mode*, write the string 'final_string' to the
        // 'stderr' output stream.  If the current process is running in
        // *non-console* *mode*, write the string 'message' to the Windows
        // process debugger, followed by a new line.  The behavior is undefined
        // unless 'file' is a null-terminated C-style string, 'line' is not
        // negative, and 'message' is a null-terminated C-style string.

    static void stderrMessageHandler(const char *file,
                                     int         line,
                                     const char *message);
        // Write, to the 'stderr' output stream, a string composed as the
        // sequential concatenation of the following components: (a) the
        // specified 'file' name, (b) a colon character, (c) the single
        // canonical decimal representation of the specified 'line' containing
        // the minimal number of zero digits, (d) a space character, (e) the
        // specified 'message', and (f) a new line.  The behavior is undefined
        // unless 'file' is a null-terminated C-style string, 'line' is not
        // negative, and 'message' is a null-terminated C-style string.

    static void stdoutMessageHandler(const char *file,
                                     int         line,
                                     const char *message);
        // Write, to the 'stdout' output stream, a string composed as the
        // sequential concatenation of the following components: (a) the
        // specified 'file' name, (b) a colon character, (c) the single
        // canonical decimal representation of the specified 'line' containing
        // the minimal number of zero digits, (d) a space character, (e) the
        // specified 'message', and (f) a new line.  The behavior is undefined
        // unless 'file' is a null-terminated C-style string, 'line' is not
        // negative, and 'message' is a null-terminated C-style string.

};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                         // =========
                         // class Log
                         // =========

// CLASS METHODS
                         // Administrative Methods
inline
Log::LogMessageHandler Log::logMessageHandler()
{
    return reinterpret_cast<LogMessageHandler>(
        bsls::AtomicOperations::getPtrAcquire(&s_logMessageHandler));
}


inline
void Log::setLogMessageHandler(Log::LogMessageHandler handler)
{
    BSLS_ASSERT_OPT(handler);

    bsls::AtomicOperations::setPtrRelease(&s_logMessageHandler,
                                          reinterpret_cast<void*>(handler));
}

                         // Dispatcher Method
inline
void Log::logMessage(const char *file, int line, const char *message)
{
    BSLS_ASSERT_OPT(file);
    BSLS_ASSERT(line >= 0);
    BSLS_ASSERT_OPT(message);

    return (logMessageHandler()) (file, line, message);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
