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
//@DESCRIPTION: This component provides a set of macros, along with a
// namespace, 'bsls::Log', which contains a suite of utility functions for
// logging in low-level code.  The macros and functions in this component
// provide a consistent interface for logging across different platforms
// through the use of a global cross-platform log message handler function.
// Users can customize the logging behavior by providing their own log message
// handler function.  Note that this component is intended to be used only when
// a more fully-featured logger is not available.
//
///Motivation
///----------
// Using the functionality of this component instead of writing messages
// directly to 'stderr' has the following advantages:
//
//: o Users have the freedom to customize the default logging behavior.
//:     - A user may want all logs to be automatically redirected to a file or
//:       may want to add some custom formatting or handling.  These abilities
//:       are not available if the output stream is predetermined.
//:
//: o The logging mechanism behaves correctly on all platforms by default.
//:     - Some platforms have particular restrictions on when the standard
//:       output streams can be used.  For example, Windows applications
//:       running in non-console mode do not have a concept of 'stdout' or
//:       'stderr'; writing directly to either of these streams is known to
//:       hang the process when no console is attached.
//
///Functionality
///-------------
// This section describes the functionality provided by this component in more
// detail.
//
///Log Message Handler
///- - - - - - - - - -
// The 'bsls::Log' class provides two 'static' methods, 'logMessageHandler' and
// 'setLogMessageHandler', which can be used, respectively, to retrieve and set
// the globally installed log message handler through which all log messages
// are written.  All log message handlers must follow the signature and
// contract requirements of the 'bsls::Log::LogMessageHandler' 'typedef'.
//
// The log message handler 'bsls::Log::platformDefaultMessageHandler' is
// installed by default as the global handler.  This handler writes all log
// messages to 'stderr', except in Windows non-console mode where the
// destination of the 'OutputDebugString' function is used.
//
// In addition to the default handler, the log message handlers
// 'bsls::Log::stdoutMessageHandler' and 'bsls::Log::stderrMessageHandler' are
// provided as a set of simple handlers that write log messages to 'stdout' and
// 'stderr', respectively.
//
///Writing Log Messages
/// - - - - - - - - - -
// There are four ways to invoke the currently installed log message handler,
// depending on the specific behavior required: The macro 'BSLS_LOG' allows a
// formatted message to be written using a 'printf'-style format string.  The
// macro 'BSLS_LOG_SIMPLE' allows a simple, non-formatted string to be written.
// Both of the macros automatically use the file name and line number of the
// point that the macro was invoked.  The 'static' methods
// 'bsls::Log::logFormatted' and 'bsls::Log::logMessage' provide the same
// functionality as 'BSLS_LOG' and 'BSLS_LOG_SIMPLE, respectively, except that
// these two methods allow a file name and line number to be passed in as
// parameters.  This is described in table form as follows:
//..
//  ===========================================================================
//  |                           Ways to Write Log Messages                    |
//  ===========================================================================
//  | File Name & Line #  |    Formatted Message    |       Simple String     |
//  |=====================|=========================|=========================|
//  |      Automatic      |       BSLS_LOG          |     BSLS_LOG_SIMPLE     |
//  |---------------------|-------------------------|-------------------------|
//  |       Manual        | bsls::Log::logFormatted |   bsls::Log::logMessage |
//  ===========================================================================
//..
//
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Logging Formatted Messages
///- - - - - - - - - - - - - - - - - - -
// Suppose that we want to write a log message when the preconditions of a
// function are not met.
//
// First, we begin to define a function, 'add', which will return the sum of
// two positive integer values:
//..
//  // myapp.cpp
//
//  unsigned int add(int a, int b)
//      // Return the sum of the specified 'a' and the specified 'b'.  The
//      // behavior is undefined unless 'a' and 'b' are not negative.
//  {
//..
//
// Then we check the precondition of the function, and use the 'BSLS_LOG' macro
// to write a log message if one of the input parameters is less than 0:
//..
//      if(a < 0 || b < 0) {
//          BSLS_LOG("Error: Invalid input combination (%d, %d).", a, b);
//          return 0;                                                 // RETURN
//      }
//
//      return static_cast<unsigned int>(a) + static_cast<unsigned int>(b);
//  }
//..
//
// The user might then use the 'add' function as follows:
//..
//  printf("%d", add(3, -100));
//..
//
// Assuming the default log message handler is currently installed, the
// following line would be printed to 'stderr' or to the Windows debugger:
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
//  void handleError(int code)
//      // Log the error message associated with the specified 'code'. The
//      // behavior is undefined unless 'code' is in the range [0 .. 3].
//  {
//      BSLS_ASSERT(static_cast<unsigned int>(code)
//                  < sizeof(errorStrings)/sizeof(errorStrings[0]));
//..
//
// In the case that we receive a valid error code, we would want to log the
// string associated with this code.  We use the macro 'BSLS_LOG_SIMPLE' to
// ensure that the true strings are logged and are not interpreted as format
// strings:
//..
//      BSLS_LOG_SIMPLE(errorStrings[code]);
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
//  myapp.cpp:14 Please use '%2f' for a slash character in a URI.
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
//  void handleErrorFlexible(const char *file, int line, int code)
//      // Log the error message associated with the specified 'code', using
//      // the specified 'file' and the specified 'line' as the source location
//      // for the error.  The behavior is undefined unless 'file' is a
//      // null-terminated string, 'line' is not negative, and 'code' is in the
//      // range [0 .. 3].
//  {
//      BSLS_ASSERT(file);
//      BSLS_ASSERT(line >= 0);
//      BSLS_ASSERT(code >= 0);
//      BSLS_ASSERT(static_cast<unsigned int>(code)
//                  < (sizeof(errorStringsNew)/sizeof(errorStringsNew[0])));
//..
//
// We can bypass the macros by calling the function 'bsls::Log::logMessage'
// directly, allowing us to pass in the given file name and line number:
//..
//      bsls::Log::logMessage(file, line, errorStringsNew[code]);
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
#define BSLS_LOG(...)                                                         \
        (BloombergLP::bsls::Log::logFormatted(__FILE__, __LINE__, __VA_ARGS__))
    // Write, to the currently installed log message handler, the formatted
    // string that would result from applying the 'printf'-style formatting
    // rules to the specified '...', using the first parameter as the format
    // string and any further parameters as the expected substitutions.  The
    // file name and line number of the point of expansion of the macro are
    // automatically used as the file name and line number for the log. The
    // behavior is undefined unless the first parameter of '...' is a valid
    // 'printf'-style format string, and all substitutions needed by the format
    // string are in the subsequent elements of '...'.

#define BSLS_LOG_SIMPLE(msg)                                                  \
                (BloombergLP::bsls::Log::logMessage(__FILE__, __LINE__, (msg)))
    // Write the specified 'msg' to the currently installed log message
    // handler, with the file name and line number of the point of expansion of
    // the macro automatically used as the file name and line number of the
    // log.  The behavior is undefined unless 'msg' is a null-terminated
    // string.

namespace BloombergLP {
namespace bsls {

                         // =========
                         // class Log
                         // =========

class Log {
    // This class serves as a namespace containing a suite of utility functions
    // that allow low-level code to write log messages to a configurable,
    // globally controlled destination.

  public:
    // TYPES
    typedef void (*LogMessageHandler)(const char *file,
                                      int         line,
                                      const char *message);
        // The 'LogMessageHandler' 'typedef' represents a type of function that
        // handles log messages in an unspecified way (e.g. by writing the log
        // message to an output stream or to a file).  Because they can be
        // called concurrently from multiple threads, log message handlers
        // should be thread-safe.  While installed, handlers must exhibit only
        // defined behavior if the specified 'file' is a null-terminated
        // string, the specified 'line' is not negative, and the specified
        // 'message' is a null-terminated string.

  private:
    // CLASS DATA
    static bsls::AtomicOperations::AtomicTypes::Pointer
                                        s_logMessageHandler; // the currently
                                                             // installed log
                                                             // message handler
                                                             // (not owned)

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
        // Write, to a platform-specific destination, a string (henceforth
        // labeled 'final_string') composed as the sequential concatenation of
        // the following components: (a) the specified 'file' name, (b) a colon
        // character, (c) the single canonical decimal representation of the
        // specified 'line' containing the minimal number of zero digits, (d) a
        // space character, (e) the specified 'message', and (f) a new line.
        // On *non-Windows* systems, write the aforementioned string
        // 'final_string' to the 'stderr' output stream.  On *Windows* systems:
        // If the current process is running in *console* *mode*, write the
        // string 'final_string' to the 'stderr' output stream.  If the current
        // process is running in *non-console* *mode*, write the string
        // 'final_string' to the Windows process debugger, followed by a new
        // line. The behavior is undefined unless 'file' is a null-terminated
        // string, 'line' is not negative, and 'message' is a null-terminated
        // string.  Note that this function is used as the default log message
        // handler.

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
