// balst_assertionlogger.h                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALST_ASSERTIONLOGGER
#define INCLUDED_BALST_ASSERTIONLOGGER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide configurable logging handler for assertion failures.
//
//@CLASSES:
//  balst::AssertionLogger: mechanism class for logging assertion failures
//
//@SEE_ALSO: bsls_assert
//
//@DESCRIPTION: This component defines an assert handling mechanism that logs
// the function call stack at the point of the assertion (using the BAEL
// logging framework), and then returns to the caller allowing program
// execution to continue.  This component provides methods to enable or disable
// logging, which can be called throughout the execution of the program.  These
// methods allow running tasks to adjust the run-time cost of generating
// assertion failure information in a production environment.  The intent of
// providing a non-terminating assertion handler is to allow previously
// disabled assertions to be enabled (and new assertions to be added) without
// causing crashes in production applications that have such failures.  The
// premise is that the bugs uncovered in "legacy" code by newly enabled
// assertions may be benign, and more damage may be done by terminating the
// task in a production environment than by allowing execution to continue and
// the bug to be addressed in a more orderly fashion.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: String Buffer Overflow
///- - - - - - - - - - - - - - - - -
// Suppose we are responsible for maintaining a large amount of legacy code
// that frequently passes internal buffers of string objects to methods that
// manipulate those buffers's contents.  We expect that these methods
// occasionally overrun their supplied buffer, so we introduce an assertion to
// the destructor of string that will alert us to such a buffer overrun.
// Unfortunately, our legacy code does not have effective unit tests, and we
// want to avoid causing crashes in production applications, since we expect
// that frequently the overflow in "working" legacy code is only overwriting
// the null terminating byte and is otherwise harmless.  We can use the
// 'bdesu::AssertionLogger::failTrace' assertion-failure callback to replace
// the default callback, which aborts the task, with one that will log the
// failure and the call-stack at which it occurred.
//
// First, we write a dubious legacy date routine:
//..
//  void getDayAndDate(char *buf, const struct tm *datetime)
//      // Format the specified 'datetime' into the specified 'buf' as
//      // "Weekday, Month N", e.g., "Monday, May 6".
//  {
//      static const char *const months[] = {
//          "January", "February", "March", "April", "May", "June", "July",
//          "August", "September", "October", "November", "December"       };
//      static const char *const days[] = {
//          "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
//          "Saturday"                                                     };
//      static const char *const digits[] = {
//          "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"               };
//      *buf = 0;
//      strcat(buf, days[datetime->tm_wday]);
//      strcat(buf, ", ");
//      strcat(buf, months[datetime->tm_mon]);
//      strcat(buf, " ");
//      if (datetime->tm_mday >= 10) {
//          strcat(buf, digits[datetime->tm_mday / 10]);
//      }
//      strcat(buf, digits[datetime->tm_mday % 10]);
//  }
//..
// Then, we write a buggy function that will inadvertently over-write the null
// terminator in a 'bsl::string':
//..
//  void getLateSummerDate(bsl::string *date)
//      // "Try to remember a time in ..."
//  {
//      struct tm datetime = { 0, 0, 0, 11, 8, 113, 3, 0, 1 };
//      date->resize(22);  // Surely this is long enough...
//      getDayAndDate(const_cast<char *>(date->c_str()), &datetime);
//  }
//..
// Next, we embed this code deep in the heart of a large system, compile it
// with optimization, and have it run, apparently successfully, for several
// years.
//..
//  void big_important_highly_visible_subsystem()
//      // If this crashes, you're fired!
//  {
//      // lots of code...
//      bsl::string s;
//      getLateSummerDate(&s);
//      // lots more code...
//  }
//..
// Now, someone comes along and insists that all assertions must be turned on
// due to heightened auditing requirements.  He is prevailed upon to agree that
// logging the assertions is preferable to crashing.  We do so:
//..
//  void protect_the_subsystem()
//      // Protect your job, too!
//  {
//      bsls::AssertFailureHandlerGuard guard(
//                            balst::AssertionLogger::assertionFailureHandler);
//      big_important_highly_visible_subsystem();
//  }
//..
// Finally, we activate the logging handler and find logged errors similar to
// the following, indicating bugs that must be fixed.  The hexadecimal stack
// trace can be merged against the executable program to determine the location
// of the error.
//..
//  // [ [ ... balst_assertionlogger.cpp 55 Assertion.Failure 32
//  // Assertion failed: (*this)[this->d_length] == CHAR_TYPE(),
//  // file .../bslstl_string.h, line 3407
//  // For stack trace, run 'showfunc.tsk <your_program_binary>
//  //    0x805d831 0x804f8cc 0x804e3f9 ...
//  // ]  { } ]
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_SEVERITY
#include <ball_severity.h>
#endif

namespace BloombergLP {

namespace balst {                        // ============================
                        // struct AssertionLogger
                        // ============================

class AssertionLogger {
    // This mechanism class implements an assertion failure handler that logs a
    // stack trace when triggered, and unlike other such handlers, returns to
    // its caller.  The class provides the option of setting the severity level
    // at which (BAEL) logging occurs, as well as the ability to obtain the
    // severity via a callback function invoked on each failure.  The class
    // acts as a "static singleton," keeping a single state in atomic static
    // variables.

  public:
    // PUBLIC TYPES
    typedef ball::Severity::Level (*LogSeverityCallback)(void       *closure,
                                                        const char *text,
                                                        const char *file,
                                                        int         line);
        // Severity callback type.  If a callback function is set, it is
        // invoked on each assertion failure and passed the closure that was
        // set along with the callback, the text of the assertion failure, and
        // the source file and line number where the assertion was triggered.
        // The function should return a severity level at which the assertion
        // failure is to be logged.  If it returns 0 ('BAEL_OFF') no logging
        // at all will be done.

    // CLASS METHODS
    static void assertionFailureHandler(const char *text,
                                        const char *file,
                                        int         line);
        // Report the sepcified 'text', 'file', and 'line' of an assertion
        // failure, and a stack trace pinpointing where the failure occurred,
        // via BAEL logging and return.  If a severity callback has been set,
        // it is invoked with the closure and 'text', 'file', and 'line' to get
        // the severity level, otherwise the static severity level is used.  If
        // the severity level is 0 ('BAEL_OFF') then the assertion is not
        // logged at all.  Note that this assertion handler violates the usual
        // policy that assertion handlers should not return to their callers.

    static void getLogSeverityCallback(LogSeverityCallback *callback,
                                       void **closure);
        // Load the specified 'callback' with a pointer to the function that
        // will be invoked to determine the severity level at which to report
        // an assertion failure (when an assertion failure occurs), and load
        // the specified 'closure' with the closure pointer which will be
        // passed to the callback function.  Note that these values are the
        // ones set by the most recent call to 'setLogSeverityCallback', and
        // will be null if there has been no such call.

    static void setLogSeverityCallback(LogSeverityCallback callback,
                                       void *closure);
        // Set, to the specified 'callback', the function that will be invoked
        // to determine the severity at which to log assertion failures, and
        // set the specified 'closure' to be the first argument passed to that
        // function when it is invoked (when an assertion failure occurs).  If
        // 'callback' is null, the value of 'severity()' is used instead.  Note
        // that a typical pattern for 'callback' and 'closure' is to pass a
        // static class method as the 'callback' and a pointer to a particular
        // object as the 'closure'.

    static void setDefaultLogSeverity(ball::Severity::Level severity);
        // Set, to the specified 'severity', the default severity level at
        // which assertion failures are logged.  This value is used if a log
        // severity callback has not been supplied (by calling
        // 'setLogSeverityCallback').  The default severity level is initially
        // 'ball::Severity::BAEL_FATAL'.

    static ball::Severity::Level defaultLogSeverity();
        // Get the default severity level at which assertion tracing is logged.
        // This value is used if a log severity callback has not been supplied
        // (by calling 'setLogSeverityCallback').  The default severity level
        // is initially 'ball::Severity::BAEL_FATAL'.
};
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
