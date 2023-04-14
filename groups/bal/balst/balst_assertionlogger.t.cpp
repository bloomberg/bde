// balst_assertionlogger.t.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


// Try to force assertion in string to trigger, for the usage example.
#undef  BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE_2
#undef  BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_SAFE
#undef  BDE_BUILD_TARGET_OPT
#define BDE_BUILD_TARGET_OPT

#include <balst_assertionlogger.h>

#include <ball_loggermanager.h>
#include <ball_testobserver.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>

#include <bsl_string.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcpy()

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// We will use a 'balst::TestObserver' to see whether the
// 'balst::AssertionLogger' system reports assertion failures at various
// severity levels.
//-----------------------------------------------------------------------------
//CLASS METHODS
// [ 1] void assertionFailureHandler(*text, *file, int line);
// [ 1] void getLogSeverityCallback(LogSeverityCallback*cb, void **cl);
// [ 1] void setLogSeverityCallback(LogSeverityCallback cb, void *cl);
// [ 1] void setDefaultLogSeverity(ball::Severity::Level severity);
// [ 1] ball::Severity::Level defaultLogSeverity();
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

#define C_(X)   << #X << ": " << X << '\t'
#define A_(X,S) { if (!(X)) { cout S << endl; aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef balst::AssertionLogger Obj;

// ============================================================================
//                           GLOBAL HELPER CLASSES
// ----------------------------------------------------------------------------

template <ball::Severity::Level Level>
class SeverityCB
    // Simple callback class to test severity callback.
{
    // PRIVATE ACCESSORS
    ball::Severity::Level level() const;
        // Return Level.

  public:
    // CLASS METHODS
    static ball::Severity::Level callback(void *severityCB,
                                          const char *,
                                          const char *,
                                          int);
        // Indirect to the specified 'severityCB' class object, ignoring the
        // available text, file, and line information.
};

template <ball::Severity::Level Level>
ball::Severity::Level SeverityCB<Level>::level() const
{
    return Level;
}

template <ball::Severity::Level Level>
ball::Severity::Level SeverityCB<Level>::callback(void *severityCB,
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

struct NumReturnReports {
    // This 'struct' anticipates the number of reports that will be generated
    // from 'BSLS_ASSERT_OPT' complaining about it having been returned from.
    // 'BSLS_ASSERT_OPT' will publish two reports to ball every time it returns
    // when the count of returns (including the current return) was a power of
    // 2.

    int d_numReturns;
    int d_numPublished;

    NumReturnReports()
    : d_numReturns(0)
    , d_numPublished(0)
        // Create a 'ReturnNumReports' object.
    {}

    void operator++()
        // Update 'd_numPublished', if appropriate, and increment
        // 'd_numReturns'.
    {
        ++d_numReturns;
        if (0 == (d_numReturns & (d_numReturns - 1))) {
            d_numPublished += 2;
        }
    }

    operator int()
        // Return the expected number of return reports published.
    {
        return d_numPublished;
    }
};

// ============================================================================
//                    HELPER FUNCTIONS FOR USAGE EXAMPLE
// ============================================================================

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
// 'balst::AssertionLogger::failTrace' assertion-failure callback to replace
// the default callback, which aborts the task, with one that will log the
// failure and the call-stack at which it occurred.
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
# if defined(BSLS_PLATFORM_OS_LINUX) && defined(BSLS_PLATFORM_CMP_GNU)

    struct tm datetime = { 0, 0, 0, 11, 8, 113, 3, 0, 1, 0, 0 };

#else

    struct tm datetime = { 0, 0, 0, 11, 8, 113, 3, 0, 1 };

#endif

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
    bsl::string s;
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
                              balst::AssertionLogger::assertionFailureHandler);
    big_important_highly_visible_subsystem();
}
//..
// Finally, we activate the logging handler and find logged errors similar to
// the following, indicating bugs that must be fixed.  The hexadecimal stack
// trace can be merged against the executable program to determine the location
// of the error.
//..
    // [ [ ... balst_assertionlogger.cpp 55 Assertion.Failure 32
    // Assertion failed: (*this)[this->d_length] == CHAR_TYPE(),
    // file .../bslstl_string.h, line 3407
    // For stack trace, run 'showfunc.tsk <your_program_binary>
    //    0x805d831 0x804f8cc 0x804e3f9 ...
    // ]  { } ]
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;    (void) verbose;
    bool         veryVerbose = argc > 3;    (void) veryVerbose;
    bool     veryVeryVerbose = argc > 4;    (void) veryVeryVerbose;
    bool veryVeryVeryVerbose = argc > 5;    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // Change the handler return policy not to abort.
    {
        // Enable assertions to return (in violation of policy) for testing
        // purposes only.

        char *key = const_cast<char*>(
                  bsls::Assert::k_permitOutOfPolicyReturningAssertionBuildKey);
        strcpy(key, "bsls-PermitOutOfPolicyReturn");

        bsls::Assert::permitOutOfPolicyReturningFailureHandler();
    }

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

        ball::TestObserver               to(&cout);
        ball::LoggerManagerConfiguration c;
        ball::LoggerManagerScopedGuard   lmsg(&to, c);

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
        //: 1 Verify that 'balst::AssertionLogger' respects the severity level
        //:   at which it is asked to report assertions.
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
        //: 3 Set up a callback that returns 'e_FATAL' and verify that a record
        //:   is logged on assertion failure.  Set up a callback that returns
        //:   'e_WARN' and verify that a record is not logged on assertion
        //:   failure. (C-3)
        //:
        //: 4 Set the 'ball::LoggerManager' pass threshold to 255 (meaning
        //:   all), set the defualt log severity to 'e_OFF', trigger an
        //:   assertion, and verify that a record is not logged.  (C-4)
        //
        // Testing:
        // [ 1] void assertionFailureHandler(*text, *file, int line);
        // [ 1] void getLogSeverityCallback(LogSeverityCallback*cb, void **cl);
        // [ 1] void setLogSeverityCallback(LogSeverityCallback cb, void *cl);
        // [ 1] void setDefaultLogSeverity(ball::Severity::Level severity);
        // [ 1] ball::Severity::Level defaultLogSeverity();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ASSERT TRACING" << endl
                          << "==============" << endl;

        bsl::ostringstream               o;
        ball::TestObserver               to(&o);
        ball::LoggerManagerConfiguration c;
        ball::LoggerManagerScopedGuard   lmsg(&to, c);
        bsls::AssertFailureHandlerGuard  guard(
                              balst::AssertionLogger::assertionFailureHandler);

        NumReturnReports numReturnReports;

        ASSERTV(to.numPublishedRecords(), 0 == to.numPublishedRecords());

        if (veryVerbose) { T_ cout << "Severity OFF" << endl; }
        balst::AssertionLogger::setDefaultLogSeverity(ball::Severity::e_OFF);
        { AlwaysAssert(); ++numReturnReports; }
        ASSERTV(balst::AssertionLogger::defaultLogSeverity(),
                                 ball::Severity::e_OFF ==
                                 balst::AssertionLogger::defaultLogSeverity());
        ASSERTV(to.numPublishedRecords(), numReturnReports,
                             0 + numReturnReports == to.numPublishedRecords());

        if (veryVerbose) { T_ cout << "Severity FATAL" << endl; }
        balst::AssertionLogger::setDefaultLogSeverity(
                                                   ball::Severity::e_FATAL);
        { AlwaysAssert(); ++numReturnReports; }
        ASSERTV(balst::AssertionLogger::defaultLogSeverity(),
                                 ball::Severity::e_FATAL ==
                                 balst::AssertionLogger::defaultLogSeverity());
        ASSERTV(to.numPublishedRecords(), numReturnReports,
                             1 + numReturnReports == to.numPublishedRecords());

        if (veryVerbose) { T_ cout << "Severity ERROR" << endl; }
        balst::AssertionLogger::setDefaultLogSeverity(
                                                   ball::Severity::e_ERROR);
        { AlwaysAssert(); ++numReturnReports; }
        ASSERTV(balst::AssertionLogger::defaultLogSeverity(),
                                 ball::Severity::e_ERROR ==
                                 balst::AssertionLogger::defaultLogSeverity());
        ASSERTV(to.numPublishedRecords(), numReturnReports,
                             2 + numReturnReports == to.numPublishedRecords());
        o << to.lastPublishedRecord();
        if (veryVeryVerbose) { P(o.str()) }
        LOOP_ASSERT(o.str(), string::npos != o.str().find(" 0x"));
        o.str(string());
        ASSERT(string::npos == o.str().find(" 0x"));

        if (veryVerbose) { T_ cout << "Severity WARN" << endl; }
        balst::AssertionLogger::setDefaultLogSeverity(
                                                    ball::Severity::e_WARN);
        { AlwaysAssert(); ++numReturnReports; }
        ASSERTV(balst::AssertionLogger::defaultLogSeverity(),
                                 ball::Severity::e_WARN ==
                                 balst::AssertionLogger::defaultLogSeverity());
        ASSERTV(to.numPublishedRecords(), numReturnReports,
                             2 + numReturnReports == to.numPublishedRecords());

        if (veryVerbose) { T_ cout << "Severity TRACE" << endl; }
        balst::AssertionLogger::setDefaultLogSeverity(
                                                   ball::Severity::e_TRACE);
        { AlwaysAssert(); ++numReturnReports; }
        ASSERTV(balst::AssertionLogger::defaultLogSeverity(),
                                 ball::Severity::e_TRACE ==
                                 balst::AssertionLogger::defaultLogSeverity());
        ASSERTV(to.numPublishedRecords(), numReturnReports,
                             2 + numReturnReports == to.numPublishedRecords());

        SeverityCB<ball::Severity::e_FATAL>        fatalCb;
        SeverityCB<ball::Severity::e_WARN>         warnCb;
        balst::AssertionLogger::LogSeverityCallback   cb;
        void                                        *cl;

        if (veryVerbose) { T_ cout << "Using severity callback" << endl; }
        balst::AssertionLogger::setLogSeverityCallback(fatalCb.callback, &cb);
        balst::AssertionLogger::getLogSeverityCallback(&cb, &cl);
        ASSERT(fatalCb.callback == cb);
        ASSERT(&cb              == cl);
        { AlwaysAssert(); ++numReturnReports; }
        ASSERTV(balst::AssertionLogger::defaultLogSeverity(),
                                 ball::Severity::e_TRACE ==
                                 balst::AssertionLogger::defaultLogSeverity());
        ASSERTV(to.numPublishedRecords(), numReturnReports,
                             3 + numReturnReports == to.numPublishedRecords());
        o << to.lastPublishedRecord();
        if (veryVeryVerbose) { P(o.str()) }
        ASSERTV(o.str(), string::npos != o.str().find(" 0x"));
        o.str(string());
        ASSERT(string::npos == o.str().find(" 0x"));

        balst::AssertionLogger::setLogSeverityCallback(warnCb.callback, &cb);
        balst::AssertionLogger::getLogSeverityCallback(&cb, &cl);
        ASSERT(warnCb.callback == cb);
        ASSERT(&cb             == cl);
        { AlwaysAssert(); ++numReturnReports; }
        ASSERTV(balst::AssertionLogger::defaultLogSeverity(),
                                 ball::Severity::e_TRACE ==
                                 balst::AssertionLogger::defaultLogSeverity());
        ASSERTV(to.numPublishedRecords(), numReturnReports,
                             3 + numReturnReports == to.numPublishedRecords());

        balst::AssertionLogger::setLogSeverityCallback(0, 0);
        balst::AssertionLogger::getLogSeverityCallback(&cb, &cl);
        ASSERT(0 == cb);
        ASSERT(0 == cl);

        if (veryVerbose) { T_ cout << "e_OFF disables tracing" << endl; }
        ball::LoggerManager::singleton().setDefaultThresholdLevels(
                                                                 0, 255, 0, 0);
        balst::AssertionLogger::setDefaultLogSeverity(
                                                     ball::Severity::e_OFF);
        { AlwaysAssert(); ++numReturnReports; }
        ASSERTV(balst::AssertionLogger::defaultLogSeverity(),
                                 ball::Severity::e_OFF ==
                                 balst::AssertionLogger::defaultLogSeverity());
        ASSERTV(to.numPublishedRecords(), numReturnReports,
                             3 + numReturnReports == to.numPublishedRecords());
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
