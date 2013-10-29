// baesu_asserttrace.h                                                -*-C++-*-
#ifndef INCLUDED_BAESU_ASSERTTRACE
#define INCLUDED_BAESU_ASSERTTRACE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide controllable logging handler for assertion failures.
//
//@CLASSES:
//   baesu_AssertTrace: mechanism class for logging assertion failures
//
//@SEE_ALSO:
//   bsls_assert
//
//@AUTHOR: Hyman Rosen (hrosen4)
//
//@DESCRIPTION: This component defines an assert handler which will log stack
// traces of failed assertions via BAEL logging and then return to the caller,
// allowing program execution to continue.  This logging may by turned on or
// off throughout the execution of the program (and not just at initialization
// time).  The intent is to allow previously disabled assertions to be enabled
// and to allow new ones to be added without causing immediate crashes in
// programs that have failures, on the premise that "legacy" problems that
// "work anyway" should not be "penalized", and so that they can be fixed in a
// leisurely fashion.
//
// Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: String Buffer Overflow
///- - - - - - - - - - - - - - - - -
// There exists a good deal of legacy code which passes the internal buffers of
// string objects to methods which fill them with data.  Occasionally, those
// methods overrun the buffers, causing the program to abort with a fatal error
// when the string destructor detects that this has happened.  Suppose we would
// like to avoid these crashes, because we believe that the overflow is only of
// the null terminating byte and is otherwise harmless.  We would also,
// however, like to know that this is happening, and where.  We can use
// 'baesu_AssertTrace' for this purpose.
//
// First, we write a dubious date routine:
//..
//    void ascdate(char *buf, const struct tm *date)
//        // Format the specified 'date' into the specified 'buf' as
//        // "Weekday, Month N", e.g., "Monday, May 6".
//    {
//        static const char *const months[] =
//        {   "January", "February", "March", "April", "May", "June", "July",
//            "August", "September", "October", "November", "December"       };
//        static const char *const days[] =
//        {   "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
//            "Saturday"                                                     };
//        static const char *const digits[] =
//        {   "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"               };
//        *buf = 0;
//        strcat(buf, days[date->tm_wday]);
//        strcat(buf, ", ");
//        strcat(buf, months[date->tm_mon]);
//        strcat(buf, " ");
//        if (date->tm_mday >= 10) {
//            strcat(buf, digits[date->tm_mday / 10]);
//        }
//        strcat(buf, digits[date->tm_mday % 10]);
//    }
//..
// Then, we write the buggy code which will cause the problem.
//..
//    void bogus()
//        // "Try to remember a time in ..."
//    {
//        struct tm date = { 0, 0, 0, 11, 8, 113, 3, 0, 1 };
//        bsl::string datef(22, ' ');  // Surely this string is long enough...
//        ascdate(const_cast<char *>(datef.c_str()), &date);
//    }
//..
// Next, we embed this code deep in the heart of a large system, compile it
// with optimization, and have it run, apparently successfully, for several
// years.
//..
//    void big_important_highly_visible_subsystem()
//        // If this crashes, you're fired!
//    {
//        // lots of code...
//        bogus();
//        // lots more code...
//    }
//..
// Now, someone comes along and insists that all assertions must be turned on
// due to heightened auditing requirements.  He is prevailed upon to agree that
// logging the assertions is preferable to crashing.
//
// Finally, we activate this logging handler and upon checking the logs, one
// day we see the error of our ways.
//..
//    void protect_the_subsystem()
//        // Protect your job, too!
//    {
//        bsls::AssertFailureHandlerGuard guard(baesu_AssertTrace::failTrace);
//        big_important_highly_visible_subsystem();
//    }
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_SEVERITY
#include <bael_severity.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

namespace BloombergLP {
                          // ========================
                          // struct baesu_AssertTrace
                          // ========================

class baesu_AssertTrace {
  public:
    // PUBLIC TYPES
    typedef bael_Severity::Level (*LevelCB)(
                             void       *closure,
                             const char *text,
                             const char *file,
                             int         line);  // Severity callback type.

  private:
    // CLASS DATA
    static bsls::AtomicOperations::AtomicTypes::Pointer
                        s_callback;  // Severity callback.
    static bsls::AtomicOperations::AtomicTypes::Pointer
                        s_closure;   // Closure for callback.
    static bsls::AtomicOperations::AtomicTypes::Int
                        s_severity;  // Logging severity level.

  public:
    // CLASS METHODS
    static void failTrace(const char *text, const char *file, int line);
        // Report the assertion failure and a stack trace via BAEL logging and
        // return.  If a severity callback has been set, it is invoked with the
        // closure and the specified 'text', 'file', and 'line' to get the
        // severity level, otherwise the static severity level is used.  Note
        // that this assertion handler violates the usual policy that assertion
        // handlers should not return to their callers.

    static void getLevelCB(LevelCB *callback, void **closure);
        // Store the previously specified 'callback' and 'closure' (or null)
        // into the parameters.

    static void setLevelCB(LevelCB callback, void *closure);
        // Set the specified 'callback' and 'closure' to be invoked when an
        // assertion trace needs to be reported.  The 'closure' is passed back
        // as the first argument to the 'callback', followed by the assertion
        // text, file, and line.

    static void setSeverity(bael_Severity::Level severity);
        // Set the severity level at which assertion tracing is logged to the
        // specified 'severity'.  This level is used when no callback is set.

    static bael_Severity::Level severity();
        // Get the severity level at which assertion tracing is logged.
};

}  // close enterprise namespace
#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
