// baesu_asserttrace.t.cpp                                            -*-C++-*-
#include <baesu_asserttrace.h>

#include <bael_loggermanager.h>
#include <bael_defaultobserver.h>
#include <bael_testobserver.h>
#include <bsls_assert.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_cstdlib.h>     // atoi()

using namespace BloombergLP;
using namespace bsl;

//==========================================================================
//                             TEST PLAN
//--------------------------------------------------------------------------
//                             Overview
//                             --------
// We will use a 'baesu_TestObserver' to see whether the 'baesu_AssertTrace'
// system reports assertion failures at various severity levels.
//--------------------------------------------------------------------------

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define C_(X)   << #X << ": " << X << '\t'
#define A_(X,S) { if (!(X)) { cout S << endl; aSsErT(1, #X, __LINE__); } }
#define LOOP_ASSERT(I,X)              A_(X,C_(I))
#define LOOP2_ASSERT(I,J,X)           A_(X,C_(I)C_(J))
#define LOOP3_ASSERT(I,J,K,X)         A_(X,C_(I)C_(J)C_(K))
#define LOOP4_ASSERT(I,J,K,L,X)       A_(X,C_(I)C_(J)C_(K)C_(L))
#define LOOP5_ASSERT(I,J,K,L,M,X)     A_(X,C_(I)C_(J)C_(K)C_(L)C_(M))
#define LOOP6_ASSERT(I,J,K,L,M,N,X)   A_(X,C_(I)C_(J)C_(K)C_(L)C_(M)C_(N))
#define LOOP7_ASSERT(I,J,K,L,M,N,O,X) A_(X,C_(I)C_(J)C_(K)C_(L)C_(M)C_(N)C_(O))

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef baesu_AssertTrace Obj;

//=============================================================================
//                    HELPER FUNCTIONS FOR USAGE EXAMPLE
//=============================================================================

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
    void ascdate(char *buf, const struct tm *date)
        // Format the specified 'date' into the specified 'buf' as
        // "Weekday, Month N", e.g., "Monday, May 6".
    {
        static const char *const months[] =
        {   "January", "February", "March", "April", "May", "June", "July",
            "August", "September", "October", "November", "December"       };
        static const char *const days[] =
        {   "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
            "Saturday"                                                     };
        static const char *const digits[] =
        {   "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"               };
        *buf = 0;
        strcat(buf, days[date->tm_wday]);
        strcat(buf, ", ");
        strcat(buf, months[date->tm_mon]);
        strcat(buf, " ");
        if (date->tm_mday >= 10) {
            strcat(buf, digits[date->tm_mday / 10]);
        }
        strcat(buf, digits[date->tm_mday % 10]);
    }
//..
// Then, we write the buggy code which will cause the problem.
//..
    std::string bogus()
        // "Try to remember a time in ..."
    {
        struct tm date = { 0, 0, 0, 11, 8, 113, 3, 0, 1 };
        bsl::string datef(22, ' ');  // Surely this string is long enough...
        ascdate(const_cast<char *>(datef.c_str()), &date);
        return datef;
    }
//..
// Next, we embed this code deep in the heart of a large system, compile it
// with optimization, and have it run, apparently successfully, for several
// years.
//..
    void big_important_highly_visible_subsystem()
        // If this crashes, you're fired!
    {
        // lots of code...
        bogus();
        // lots more code...
    }
//..
// Now, someone comes along and insists that all assertions must be turned on
// due to heightened auditing requirements.  He is prevailed upon to agree that
// logging the assertions is preferable to crashing.
//
// Finally, we activate this logging handler and upon checking the logs, one
// day we see the error of our ways.
//..
    void protect_the_subsystem()
        // Protect your job, too!
    {
        bsls::AssertFailureHandlerGuard guard(baesu_AssertTrace::failTrace);
        big_important_highly_visible_subsystem();
    }
//..

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //:
        //: 2 Verify that 'baesu_AssertTrace' respects the severity level at
        //:   which it is asked to report assertions.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //:
        //: 2 Set the severity level for reporting to various levels, some
        //:   which report by default and some which are silenced.  Verify that
        //:   when assertions are triggered, they are reported or not as the
        //:   severity specifies.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        bsl::ostringstream o;
        bael_TestObserver to(o);
        to.setVerbose(true);
        bael_LoggerManagerConfiguration c;
        bael_LoggerManagerScopedGuard lmsg(&to, c);

        ASSERT(to.numPublishedRecords() == 0);

        baesu_AssertTrace::setSeverity(bael_Severity::BAEL_FATAL);
        protect_the_subsystem();
        ASSERT(baesu_AssertTrace::severity() == bael_Severity::BAEL_FATAL);
        ASSERT(to.numPublishedRecords() == 1);

        baesu_AssertTrace::setSeverity(bael_Severity::BAEL_ERROR);
        protect_the_subsystem();
        ASSERT(baesu_AssertTrace::severity() == bael_Severity::BAEL_ERROR);
        ASSERT(to.numPublishedRecords() == 2);

        baesu_AssertTrace::setSeverity(bael_Severity::BAEL_WARN);
        protect_the_subsystem();
        ASSERT(baesu_AssertTrace::severity() == bael_Severity::BAEL_WARN);
        ASSERT(to.numPublishedRecords() == 2);

        baesu_AssertTrace::setSeverity(bael_Severity::BAEL_TRACE);
        protect_the_subsystem();
        ASSERT(baesu_AssertTrace::severity() == bael_Severity::BAEL_TRACE);
        ASSERT(to.numPublishedRecords() == 2);
      }  break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
