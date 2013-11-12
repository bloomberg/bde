// baesu_asserttrace.t.cpp                                            -*-C++-*-
#include <baesu_asserttrace.h>

#include <bael_loggermanager.h>
#include <bael_testobserver.h>
#include <bsl_string.h>
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
//CLASS METHODS
// [ 1] void assertionFailureHandler(*text, *file, int line);
// [ 1] void getLogSeverityCallback(LogSeverityCallback*cb, void **cl);
// [ 1] void setLogSeverityCallback(LogSeverityCallback cb, void *cl);
// [ 1] void setDefaultLogSeverity(bael_Severity::Level severity);
// [ 1] bael_Severity::Level defaultLogSeverity();
//--------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

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

// ============================================================================
//                            GLOBAL HELPER CLASSES
// ----------------------------------------------------------------------------

template <bael_Severity::Level Level>
class SeverityCB
    // Simple callback class to test severity callback.
{
    // PRIVATE ACCESSORS
    bael_Severity::Level level() const;
        // Return Level.

  public:
    // CLASS METHODS
    static bael_Severity::Level callback(void *severityCB,
                                         const char *,
                                         const char *,
                                         int);
        // Indirect to the specified 'severityCB' class object, ignoring the
        // available text, file, and line information.
};

template <bael_Severity::Level Level>
bael_Severity::Level SeverityCB<Level>::level() const
{
    return Level;
}

template <bael_Severity::Level Level>
bael_Severity::Level SeverityCB<Level>::callback(void *severityCB,
                                                 const char *,
                                                 const char *,
                                                 int)
{
    return static_cast<SeverityCB *>(severityCB)->level();
}

struct AlwaysAssert
    // A class whose destructor deliberately fails an assertion.
{
    ~AlwaysAssert();
        // Complain!
};

AlwaysAssert::~AlwaysAssert()
{
    BSLS_ASSERT_OPT(false);
}

//=============================================================================
//                    HELPER FUNCTIONS FOR USAGE EXAMPLE
//=============================================================================

// Usage
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
// 'bdesu_AssertTrace::failTrace' assertion-failure callback to replace the
// default callback, which aborts the task, with one that will log the failure
// and the call-stack at which it occurred.
//
// First, we write a dubious legacy date routine:
//..
void getDayAndDate(char *buf, const struct tm *datetime)
    // Format the specified 'datetime' into the specified 'buf' as
    // "Weekday, Month N", e.g., "Monday, May 6".
{
    static const char *const months[] = {
        "January", "February", "March", "April", "May", "June", "July",
        "August", "September", "October", "November", "December"       };
    static const char *const days[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
        "Saturday"                                                     };
    static const char *const digits[] = {
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"               };
    *buf = 0;
    strcat(buf, days[datetime->tm_wday]);
    strcat(buf, ", ");
    strcat(buf, months[datetime->tm_mon]);
    strcat(buf, " ");
    if (datetime->tm_mday >= 10) {
        strcat(buf, digits[datetime->tm_mday / 10]);
    }
    strcat(buf, digits[datetime->tm_mday % 10]);
}
//..
// Then, we write a buggy function that will inadvertently over-write the null
// terminator in a 'bsl::string':
//..
void getLateSummerDate(bsl::string *date)
    // "Try to remember a time in ..."
{
    struct tm datetime = { 0, 0, 0, 11, 8, 113, 3, 0, 1 };
    date->resize(22);  // Surely this is long enough...
    getDayAndDate(const_cast<char *>(date->c_str()), &datetime);
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
    bsl:;string s;
    getLateSummerDate(&s);
    // lots more code...
}
//..
// Now, someone comes along and insists that all assertions must be turned on
// due to heightened auditing requirements.  He is prevailed upon to agree that
// logging the assertions is preferable to crashing.  We do so:
//..
void protect_the_subsystem()
    // Protect your job, too!
{
    bsls::AssertFailureHandlerGuard guard(
                                 baesu_AssertTrace::assertionFailureHandler);
    big_important_highly_visible_subsystem();
}
//..
// Finally, we activate the logging handler and find logged errors similar to
// the following, indicating bugs that must be fixed:
//..
// Assertion failed: (*this)[this->d_length] == CHAR_TYPE()...
// (0): BloombergLP::baesu_StackTracePrintUtil::printStackTrace...
// (1): BloombergLP::baesu_AssertTrace::failTrace...
// (2): bsl::basic_string<...>::~basic_string...
// (3): big_important_highly_visible_subsystem()...
// (4): protect_the_subsystem()...
// (5): main...
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
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //:
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        bael_TestObserver               to(cout);
        bael_LoggerManagerConfiguration c;
        bael_LoggerManagerScopedGuard   lmsg(&to, c);

        to.setVerbose(veryVerbose);
        protect_the_subsystem();

        if (veryVerbose) {
            cout << to.lastPublishedRecord() << endl;
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // ASSERT TRACING
        //
        // Concerns:
        //: 1 Verify that 'baesu_AssertTrace' respects the severity level at
        //:   which it is asked to report assertions.
        //:
        //: 2 Verify that the log message contains a stack trace.
        //:
        //: 3 Verify that an established severity callback is used to obtain
        //:   the severity level to be used.
        //:
        //: 4 Verify that no logging (attempt) occurs for severity OFF.
        //
        // Plan:
        //: 1 Set the severity level for reporting to various levels, some
        //:   which report by default and some which are silenced.  Verify that
        //:   when assertions are triggered, they are reported or not as the
        //:   severity specifies. (C-1)
        //:
        //: 2 When a log record is produced, examine it to verify that it
        //:   contains a method name expected to be in the stack trace. (C-2)
        //:
        //: 3 Set up a callback that returns 'BAEL_FATAL' and verify that a
        //:   record is logged on assertion failure.  Set up a callback that
        //:   returns 'BAEL_WARN' and verify that a record is not logged on
        //:   assertion failure. (C-3)
        //:
        //: 4 Set the 'bael_LoggerManager' pass threshold to 255 (meaning all),
        //:   set the defualt log severity to 'BAEL_OFF', trigger an assertion,
        //:   and verify that a record is not logged. (C-4)
        //
        // Testing:
        // [ 1] void assertionFailureHandler(*text, *file, int line);
        // [ 1] void getLogSeverityCallback(LogSeverityCallback*cb, void **cl);
        // [ 1] void setLogSeverityCallback(LogSeverityCallback cb, void *cl);
        // [ 1] void setDefaultLogSeverity(bael_Severity::Level severity);
        // [ 1] bael_Severity::Level defaultLogSeverity();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ASSERT TRACING" << endl
                          << "==============" << endl;

        bsl::ostringstream              o;
        bael_TestObserver               to(o);
        bael_LoggerManagerConfiguration c;
        bael_LoggerManagerScopedGuard   lmsg(&to, c);
        bsls::AssertFailureHandlerGuard guard(
                                   baesu_AssertTrace::assertionFailureHandler);

        LOOP_ASSERT(to.numPublishedRecords(), 0 == to.numPublishedRecords());

        if (veryVerbose) { T_ cout << "Severity OFF" << endl; }
        baesu_AssertTrace::setDefaultLogSeverity(bael_Severity::BAEL_OFF);
        { AlwaysAssert(); }
        LOOP_ASSERT(baesu_AssertTrace::defaultLogSeverity(),
                                      bael_Severity::BAEL_OFF ==
                                      baesu_AssertTrace::defaultLogSeverity());
        LOOP_ASSERT(to.numPublishedRecords(), 0 == to.numPublishedRecords());

        if (veryVerbose) { T_ cout << "Severity FATAL" << endl; }
        baesu_AssertTrace::setDefaultLogSeverity(bael_Severity::BAEL_FATAL);
        { AlwaysAssert(); }
        LOOP_ASSERT(baesu_AssertTrace::defaultLogSeverity(),
                                      bael_Severity::BAEL_FATAL ==
                                      baesu_AssertTrace::defaultLogSeverity());
        LOOP_ASSERT(to.numPublishedRecords(), 1 == to.numPublishedRecords());
        o << to.lastPublishedRecord();
        ASSERT(string::npos != o.str().find("AlwaysAssert"));
        o.str(string());
        ASSERT(string::npos == o.str().find("AlwaysAssert"));

        if (veryVerbose) { T_ cout << "Severity ERROR" << endl; }
        baesu_AssertTrace::setDefaultLogSeverity(bael_Severity::BAEL_ERROR);
        { AlwaysAssert(); }
        LOOP_ASSERT(baesu_AssertTrace::defaultLogSeverity(),
                                      bael_Severity::BAEL_ERROR ==
                                      baesu_AssertTrace::defaultLogSeverity());
        LOOP_ASSERT(to.numPublishedRecords(), 2 == to.numPublishedRecords());
        o << to.lastPublishedRecord();
        ASSERT(string::npos != o.str().find("AlwaysAssert"));
        o.str(string());
        ASSERT(string::npos == o.str().find("AlwaysAssert"));

        if (veryVerbose) { T_ cout << "Severity WARN" << endl; }
        baesu_AssertTrace::setDefaultLogSeverity(bael_Severity::BAEL_WARN);
        { AlwaysAssert(); }
        LOOP_ASSERT(baesu_AssertTrace::defaultLogSeverity(),
                                      bael_Severity::BAEL_WARN ==
                                      baesu_AssertTrace::defaultLogSeverity());
        LOOP_ASSERT(to.numPublishedRecords(), 2 == to.numPublishedRecords());

        if (veryVerbose) { T_ cout << "Severity TRACE" << endl; }
        baesu_AssertTrace::setDefaultLogSeverity(bael_Severity::BAEL_TRACE);
        { AlwaysAssert(); }
        LOOP_ASSERT(baesu_AssertTrace::defaultLogSeverity(),
                                      bael_Severity::BAEL_TRACE ==
                                      baesu_AssertTrace::defaultLogSeverity());
        LOOP_ASSERT(to.numPublishedRecords(), 2 == to.numPublishedRecords());

        SeverityCB<bael_Severity::BAEL_FATAL> fatalCb;
        SeverityCB<bael_Severity::BAEL_WARN>  warnCb;
        baesu_AssertTrace::LogSeverityCallback  cb;
        void                                   *cl;

        if (veryVerbose) { T_ cout << "Using severity callback" << endl; }
        baesu_AssertTrace::setLogSeverityCallback(fatalCb.callback, &cb);
        baesu_AssertTrace::getLogSeverityCallback(&cb, &cl);
        ASSERT(fatalCb.callback == cb);
        ASSERT(&cb              == cl);
        { AlwaysAssert(); }
        LOOP_ASSERT(baesu_AssertTrace::defaultLogSeverity(),
                                      bael_Severity::BAEL_TRACE ==
                                      baesu_AssertTrace::defaultLogSeverity());
        LOOP_ASSERT(to.numPublishedRecords(), 3 == to.numPublishedRecords());
        o << to.lastPublishedRecord();
        ASSERT(string::npos != o.str().find("AlwaysAssert"));
        o.str(string());
        ASSERT(string::npos == o.str().find("AlwaysAssert"));

        baesu_AssertTrace::setLogSeverityCallback(warnCb.callback, &cb);
        baesu_AssertTrace::getLogSeverityCallback(&cb, &cl);
        ASSERT(warnCb.callback == cb);
        ASSERT(&cb             == cl);
        { AlwaysAssert(); }
        LOOP_ASSERT(baesu_AssertTrace::defaultLogSeverity(),
                                      bael_Severity::BAEL_TRACE ==
                                      baesu_AssertTrace::defaultLogSeverity());
        LOOP_ASSERT(to.numPublishedRecords(), 3 == to.numPublishedRecords());

        baesu_AssertTrace::setLogSeverityCallback(0, 0);
        baesu_AssertTrace::getLogSeverityCallback(&cb, &cl);
        ASSERT(0 == cb);
        ASSERT(0 == cl);

        if (veryVerbose) { T_ cout << "BAEL_OFF disables tracing" << endl; }
        bael_LoggerManager::singleton().setDefaultThresholdLevels(
                                                                 0, 255, 0, 0);
        baesu_AssertTrace::setDefaultLogSeverity(bael_Severity::BAEL_OFF);
        { AlwaysAssert(); }
        LOOP_ASSERT(baesu_AssertTrace::defaultLogSeverity(),
                                      bael_Severity::BAEL_OFF ==
                                      baesu_AssertTrace::defaultLogSeverity());
        LOOP_ASSERT(to.numPublishedRecords(), 3 == to.numPublishedRecords());
      } break;
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
