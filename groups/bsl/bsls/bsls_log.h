// bsls_log.h                                                         -*-C++-*-
#ifndef INCLUDED_BSLS_LOG
#define INCLUDED_BSLS_LOG

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for low-level logging functions.
//
//@CLASSES:
//  bsls::Log: namespace for low-level logging functions
//
//@MACROS:
// BSLS_LOG: write log message using 'printf'-style format specification
// BSLS_LOG_SIMPLE: write log message as an unformatted string
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
//:
//:   A user may want all logs to be automatically redirected to a file or may
//:   want to add some custom formatting or handling.  These abilities are not
//:   available if the output stream is predetermined.
//:
//: o The logging mechanism behaves correctly on all platforms by default.
//:
//:   Some platforms have particular restrictions on the use of the standard
//:   output streams.  For example, Windows applications running in non-console
//:   mode do not have a concept of 'stdout' or 'stderr'; writing directly to
//:   either of these streams is known to hang the process when no console is
//:   attached.
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
// macro 'BSLS_LOG_SIMPLE' allows a simple, unformatted string to be written.
// Both of the macros automatically use the file name and line number of the
// point that the macro was invoked.  The 'static' methods
// 'bsls::Log::logFormattedMessage' and 'bsls::Log::logMessage' provide the
// same functionality as 'BSLS_LOG' and 'BSLS_LOG_SIMPLE, respectively, except
// that these two methods allow a file name and line number to be passed in as
// parameters.  This is described in table form as follows:
//..
//  .=========================================================================.
//  |      Mechanism                 |  Formatted   |  Automatic File & Line  |
//  |=========================================================================|
//  | BSLS_LOG                       |     YES      |          YES            |
//  |--------------------------------|--------------|-------------------------|
//  | BSLS_LOG_SIMPLE                |     NO       |          YES            |
//  |--------------------------------|--------------|-------------------------|
//  | bsls::Log::logFormattedMessage |     YES      |          NO             |
//  |--------------------------------|--------------|-------------------------|
//  | bsls::Log::logMessage          |     NO       |          NO             |
//  `-------------------------------------------------------------------------'
//..
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Logging Formatted Messages
///- - - - - - - - - - - - - - - - - - -
// Suppose that we want to write a formatted log message using 'printf'-style
// format specifiers when the preconditions of a function are not met.  The
// 'BSLS_LOG' macro can be used for this purpose.
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
// Now, we check the precondition of the function, and use the 'BSLS_LOG' macro
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
// Next, we may erroneously call the 'add' function with a negative argument:
//..
//  unsigned int x = add(3, -100);
//..
// Finally, assuming the default log message handler is currently installed, we
// observe the following output printed to 'stderr' or to the Windows debugger:
//..
//  myapp.cpp:8 Error: Invalid input combination (3, -100).
//..
// Note that an arbitrary string should never be passed to 'BSLS_LOG' as the
// format string.  If the string happens to contain 'printf'-style format
// specifiers but the expected substitutions are not present, it will lead to
// undefined behavior.

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef INCLUDED_BSLS_POINTERCASTUTIL
#include <bsls_pointercastutil.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

                         // ==========================
                         // BSLS_LOG Macro Definitions
                         // ==========================

#define BSLS_LOG(...)                                                         \
 (BloombergLP::bsls::Log::logFormattedMessage(__FILE__, __LINE__, __VA_ARGS__))
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
    // log.

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
        // must be thread-safe.  While installed, handlers must exhibit only
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
    static void logFormattedMessage(const char *file,
                                    int         line,
                                    const char *format,
                                    ...);
        // Invoke the currently installed log message handler with the
        // specified 'file', the specified 'line', and a message string
        // created by calling 'sprintf' on the specified 'format' with the
        // specified variadic arguments.  The behavior is undefined unless
        // '0 <= line' and 'format' is a valid 'sprintf' format specification
        // for the supplied variadic arguments.

    static void logMessage(const char *file, int line, const char *message);
        // Invoke the currently installed log message handler with the
        // specified 'file' as the first parameter, the specified 'line' as the
        // second parameter, and the specified 'message' as the third
        // parameter.  The behavior is undefined unless '0 <= line'.

    static Log::LogMessageHandler logMessageHandler();
        // Return the address of the currently installed log message handler.

    static void platformDefaultMessageHandler(const char *file,
                                              int         line,
                                              const char *message);
        // Write, to a platform-specific destination, a string composed of the
        // specified 'file' name, the specified 'line' number, and the
        // specified 'message'.  On *non*-Windows systems, write the log record
        // to the 'stderr' output stream.  On *Windows* systems: If the current
        // process is running in *console* *mode*, write the log record to the
        // 'stderr' output stream.  If the current process is running in
        // *non-console* *mode*, write the log record to the Windows process
        // debugger.  The behavior is undefined unless '0 <= line'.  Note that
        // this function is used as the default log message handler.

    static void setLogMessageHandler(Log::LogMessageHandler handler);
        // Install the specified 'handler' as the current log message handler.

    static void stderrMessageHandler(const char *file,
                                     int         line,
                                     const char *message);
        // Write, to the 'stderr' output stream, a string composed of the
        // specified 'file' name, the specified 'line' number, and the
        // specified 'message'.  The behavior is undefined unless '0 <= line'.

    static void stdoutMessageHandler(const char *file,
                                     int         line,
                                     const char *message);
        // Write, to the 'stdout' output stream, a string composed of the
        // specified 'file' name, the specified 'line' number, and the
        // specified 'message'.  The behavior is undefined unless '0 <= line'.
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                         // =========
                         // class Log
                         // =========

// CLASS METHODS
inline
void Log::logMessage(const char *file, int line, const char *message)
{
    BSLS_ASSERT_SAFE(file);
    BSLS_ASSERT_SAFE(line >= 0);
    BSLS_ASSERT_SAFE(message);

    return (logMessageHandler()) (file, line, message);
}

inline
Log::LogMessageHandler Log::logMessageHandler()
{
    return reinterpret_cast<LogMessageHandler>(
        bsls::AtomicOperations::getPtrAcquire(&s_logMessageHandler));
}

inline
void Log::setLogMessageHandler(Log::LogMessageHandler handler)
{
    BSLS_ASSERT_SAFE(handler);
    bsls::AtomicOperations::setPtrRelease(
        &s_logMessageHandler, PointerCastUtil::cast<void *>(handler));
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
