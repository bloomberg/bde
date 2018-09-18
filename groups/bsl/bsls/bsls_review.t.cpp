// bsls_review.t.cpp                                                  -*-C++-*-
#include <bsls_review.h>

#include <bsls_asserttestexception.h>
#include <bsls_bsltestutil.h>
#include <bsls_log.h>
#include <bsls_logseverity.h>
#include <bsls_platform.h>
#include <bsls_types.h>

// Include 'cassert' to make sure no macros conflict between 'bsls_review.h'
// and 'cassert'.  This test driver does *not* invoke 'assert(expression)'.
#include <cassert>

#include <cstdio>    // 'fprintf'
#include <cstdlib>   // 'atoi'
#include <cstring>   // 'strcmp'
#include <exception> // 'exception'

#ifdef BSLS_PLATFORM_OS_UNIX
#include <signal.h>
#endif

using namespace BloombergLP;
using namespace std;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides a number of MACROs that are instantiated based on
// build options.  We will therefore have to make our test configure itself to
// the various build options.  Also, it is difficult (not feasible) to test
// functions that are supposed to cause the program to abort or to hang, so
// those will have to be tested manually.  We will, however, be able to verify
// that functions that throw exceptions do in fact do so.  Finally, verifying
// that certain combinations of build options cause a compile-time error will
// also have to be done by hand (see negative case numbers).
//
// ----------------------------------------------------------------------------
// [ 6] BSLS_REVIEW_SAFE_IS_ACTIVE
// [ 6] BSLS_REVIEW_IS_ACTIVE
// [ 6] BSLS_REVIEW_OPT_IS_ACTIVE
// [ 2] BSLS_REVIEW_SAFE(X)
// [ 2] BSLS_REVIEW(X)
// [ 2] BSLS_REVIEW_OPT(X)
// [ 2] BSLS_REVIEW_INVOKE(X)
// [ 4] typedef void (*Handler)(const char *, const char *, int);
// [ 1] ReviewViolation::ReviewViolation(...);
// [ 1] const char *ReviewViolation::comment();
// [ 1] int ReviewViolation::count();
// [ 1] const char *ReviewViolation::fileName();
// [ 1] int ReviewViolation::lineNumber();
// [ 1] const char *ReviewViolation::reviewLevel();
// [ 1] static void setFailureHandler(bsls::Review::Handler function);
// [ 1] static void lockReviewAdministration();
// [ 1] static bsls::Review::Handler failureHandler();
// [ 1] static void invokeHandler(const char *t, const char *f, int);
// [ 1] static int updateCount(Count *count);
// [ 4] static void failLog(const ReviewViolation &violation);
// [ 4] static void failAbort(const ReviewViolation &violation);
// [-2] static void failAbort(const ReviewViolation &violation);
// [ 4] static void failSleep(const ReviewViolation &violation);
// [-3] static void failSleep(const ReviewViolation &violation);
// [ 4] static void failThrow(const ReviewViolation &violation);
// [ 5] class bsls::ReviewFailureHandlerGuard
// [ 5] ReviewFailureHandlerGuard::ReviewFailureHandlerGuard(Handler)
// [ 5] ReviewFailureHandlerGuard::~ReviewFailureHandlerGuard()
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] CONCERN: default handler log: content
// [ 8] CONCERN: default handler log: backoff
// [-4] CONCERN: default handler log: limits
//
// [ 9] ASSERT USAGE EXAMPLE: Using Review Macros
// [10] ASSERT USAGE EXAMPLE: Invoking an review handler directly
// [11] ASSERT USAGE EXAMPLE: Using Administration Functions
// [11] ASSERT USAGE EXAMPLE: Installing Prefabricated Review-Handlers
// [12] ASSERT USAGE EXAMPLE: Creating Your Own Review-Handler
// [13] ASSERT USAGE EXAMPLE: Using Scoped Guard
// [14] ASSERT USAGE EXAMPLE: Using 'BDE_BUILD_TARGET_SAFE_2'
// [15] USAGE EXAMPLE: Adding 'BSLS_ASSERT' to an existing function
//
// [ 1] CONCERN: By default, the 'bsls_review::failAbort' is used
// [ 2] CONCERN: REVIEW macros are instantiated properly for build targets
// [ 2] CONCERN: all combinations of BDE_BUILD_TARGETs are allowed
// [ 2] CONCERN: any one review mode overrides all BDE_BUILD_TARGETs
// [ 3] CONCERN: ubiquitously detect multiply-defined review-mode flags
// [ 5] CONCERN: that locking does not stop the handlerGuard from working
// [-1] CONCERN: 'bsls::Review::failAbort' aborts
// [-1] CONCERN: 'bsls::Review::failAbort' prints to 'stderr' not 'stdout'
// [-2] CONCERN: 'bsls::Review::failThrow' aborts in non-exception build
// [-2] CONCERN: 'bsls::Review::failThrow' prints to 'stderr' for NON-EXC
// [-3] CONCERN: 'bsls::Review::failSleep' sleeps forever
// [-3] CONCERN: 'bsls::Review::failSleep' prints to 'stderr' not 'stdout'

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                    GLOBAL CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

bool globalVerbose         = false;
bool globalVeryVerbose     = false;
bool globalVeryVeryVerbose = false;

static bool        globalReviewFiredFlag = false;
static const char *globalText = "";
static const char *globalFile = "";
static const char *globalLevel = "";
static int         globalLine = -1;
static int         globalCount = 0;

// BDE_VERIFY pragma: -FE01 // at this level in the package hierarchy in a test
                            // driver we want to avoid std::exception

#define IS_POWER_OF_TWO(X) (0 == ((X) & ((X) - 1)))

#ifndef BDE_BUILD_TARGET_EXC
static bool globalReturnOnTestAssert = false;
    // This flag is very dangerous, as it will cause the test-driver review
    // handler to simple 'return' by default, exposing any additional function
    // under test to the subsequent undefined behavior.  In general, exception-
    // free builds should avoid executing such tests, rather than set this
    // flag.  However, there is some subset of this test driver that would
    // benefit from being able to invoke this handler in a test mode to be sure
    // that correct behavior occurs in the presence of the various preprocessor
    // checks for exceptions being disabled.  This flag allows for testing such
    // behavior that does not rely on aborting out of the review handler.
#endif

//=============================================================================
//                  GLOBAL HELPER MACROS FOR TESTING
//-----------------------------------------------------------------------------

#if (defined(BSLS_REVIEW_LEVEL_REVIEW_SAFE) ||                               \
     defined(BSLS_REVIEW_LEVEL_REVIEW)      ||                               \
     defined(BSLS_REVIEW_LEVEL_REVIEW_OPT)  ||                               \
     defined(BSLS_REVIEW_LEVEL_NONE) )
    #define IS_BSLS_REVIEW_MODE_FLAG_DEFINED 1
#else
    #define IS_BSLS_REVIEW_MODE_FLAG_DEFINED 0
#endif

#ifdef BDE_BUILD_TARGET_EXC
#define REVIEW_TEST_BEGIN                                    \
        try {

#define REVIEW_TEST_END                                                      \
        } catch (const std::exception& ) {                                   \
            if (verbose) printf( "Exception caught.\n" );                    \
        }
#else
#define REVIEW_TEST_BEGIN
#define REVIEW_TEST_END
#endif

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

struct LogProfile {
    enum { k_TEXT_BUFFER_SIZE = 1024 };

    char                    d_file[k_TEXT_BUFFER_SIZE];
    int                     d_line;
    char                    d_text[k_TEXT_BUFFER_SIZE];
};

struct HandlerLoggingTest {
    static const int          k_max_count = 10;
    static LogProfile         s_profiles[k_max_count];
    static bsls::Types::Int64 s_loggerInvocationCount;

    static void clear();
        // Clear s_profiles and set s_loggerInvocationCount to 0

    static LogProfile &lastProfile();
        // return the last updated profile.

    static void emptyViolationHandler(const bsls::ReviewViolation &violation);
        // Do nothing with the specified 'violation'.

    static void recordingLogMessageHandler(bsls::LogSeverity::Enum  severity,
                                           const char              *file,
                                           int                      line,
                                           const char              *message);
        // Register a test failure if the specified 'severity' is not error,
        // and store the specified 'file', 'line' and 'message' into the
        // corresponding fields of s_profile[s_loggerInvocationCount].  then
        // increment s_loggerInvocationCount

};

LogProfile         HandlerLoggingTest::s_profiles[10];
bsls::Types::Int64 HandlerLoggingTest::s_loggerInvocationCount = 0;

void HandlerLoggingTest::clear()
{
    s_loggerInvocationCount = 0;
    for (int i = 0; i < k_max_count; ++i)
    {
        s_profiles[i].d_file[0] = '\0';
        s_profiles[i].d_line = 0;
        s_profiles[i].d_text[0] = '\0';
    }
}

LogProfile &HandlerLoggingTest::lastProfile()
{
    return s_profiles[s_loggerInvocationCount-1];
}

void HandlerLoggingTest::emptyViolationHandler(
                                        const bsls::ReviewViolation &violation)
{
    (void) violation;
}

void HandlerLoggingTest::recordingLogMessageHandler(
                                             bsls::LogSeverity::Enum  severity,
                                             const char              *file,
                                             int                      line,
                                             const char              *message)
{
    ASSERT(bsls::LogSeverity::e_ERROR == severity);

    if (s_loggerInvocationCount >= k_max_count)
    {
        ASSERT(s_loggerInvocationCount < k_max_count);
        return;                                                       // RETURN
    }

    if (globalVeryVeryVerbose) {
        bsls::Log::stdoutMessageHandler(severity, file, line, message);
    }

    LogProfile & profile = s_profiles[s_loggerInvocationCount];
    strncpy(profile.d_file,
            file,
            LogProfile::k_TEXT_BUFFER_SIZE);

    profile.d_line = line;

    strncpy(profile.d_text,
            message,
            LogProfile::k_TEXT_BUFFER_SIZE);

    ++s_loggerInvocationCount;
}

static void globalReset()
    // Reset all global values used by test handlers.
{
    if (globalVeryVeryVerbose)
        printf( "*** globalReset()\n" );;

    globalReviewFiredFlag = false;
    globalText = "";
    globalFile = "";
    globalLevel = "";
    globalLine = -1;
    globalCount = 0;

    ASSERT( 0 == std::strcmp("", globalText));
    ASSERT( 0 == std::strcmp("", globalFile));
    ASSERT( 0 == std::strcmp("", globalLevel));
    ASSERT(-1 == globalLine);
    ASSERT( 0 == globalCount);
}

//-----------------------------------------------------------------------------

static void testDriverHandler(const bsls::ReviewViolation &violation)
    // Set the 'globalReviewFiredFlag' to 'true' and store the specified
    // 'comment', 'file' name, 'line' number and 'count' from the 'violation'
    // into 'globalText', globalFile', 'globalLevel, 'globalLine', and
    // 'globalCount' respectively.  Then throw an 'std::exception' object
    // provided that 'BDE_BUILD_TARGET_EXC' is defined; otherwise, abort the
    // program.
{
    if (globalVeryVeryVerbose) {
        printf( "*** testDriverHandler: ");
        P_(violation.comment());
        P_(violation.fileName());
        P(violation.lineNumber());
    }

    globalReviewFiredFlag = true;
    globalText = violation.comment();
    globalFile = violation.fileName();
    globalLevel = violation.reviewLevel();
    globalLine = violation.lineNumber();
    globalCount = violation.count();

#ifdef BDE_BUILD_TARGET_EXC
    throw std::exception();
#else
    if (globalReturnOnTestAssert) {
        return;                                                       // RETURN
    }
    std::abort();
#endif
}

//-----------------------------------------------------------------------------

static void testDriverPrint(const bsls::ReviewViolation &violation)
    // Format, in verbose mode, the specified expression 'text', 'file' name,
    // and 'line' number from the specified 'violation' the same way as the
    // 'bsls::Review::failAbort' review-failure handler function might, but on
    // 'cout' instead of 'cerr'.  Then throw an 'std::exception' object
    // provided that 'BDE_BUILD_TARGET_EXC' is defined; otherwise, abort the
    // program.
{
    if (globalVeryVeryVerbose) {
        printf( "*** testDriverPrint: " );
        P_(violation.comment());
        P_(violation.fileName());
        P(violation.lineNumber());
    }

    if (globalVeryVerbose) {
        std::fprintf(stdout,
                     "Review failed: %s, file %s, line %d\n",
                     violation.comment(), violation.fileName(),
                     violation.lineNumber() );

        std::fflush(stdout);
    }

#ifdef BDE_BUILD_TARGET_EXC
    throw std::exception();
#else
    if (globalReturnOnTestAssert) {
        return;                                                       // RETURN
    }
    std::abort();
#endif
}

//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct BadBoy {
    // Bogus 'struct' used for testing: calls 'bsls::Review::failThrow' on
    // destruction to ensure that it does not re-throw with an exception
    // pending (see case -2).

    BadBoy() {
        if (globalVeryVerbose) printf( "BadBoy Created!\n" );
    }

    ~BadBoy() {
        if (globalVeryVerbose) printf( "BadBoy Destroyed!\n" );
        bsls::ReviewViolation violation(
            "'failThrow' handler called from ~BadBoy",
            "f.c",
            9,
            "BADBOY",
            1);
        bsls::Review::failThrow(violation);
     }
};

// Declaration of function that must appear after main in order to test the
// configuration macros.
void TestConfigurationMacros();
    // Contains tests of component macros in many different configurations.

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

// Usage examples don't follow all bde coding standards for clarity.
// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FD01
// BDE_VERIFY pragma: -FD02
// BDE_VERIFY pragma: -FD03
// BDE_VERIFY pragma: -FD06
// BDE_VERIFY pragma: -IND01

///Example 1: Adding 'BSLS_ASSERT' To An Existing Function
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose you have an existing function, already deployed to production, that
// was not written with defensive programming in mind.  In order to increase
// safety, you would like to add 'BSLS_ASSERT' macros to this function that
// match the contract originally written when this function was added earlier.
//
// For example, consider the function 'myFunc' in the class 'FunctionsV1' that
// was implemented like this:
//..
// my_functions.h
// ...

class FunctionsV1 {
    // ...
  public:
    // ...

    static int myFunc(int x, int y);
        // Do something with the specified positive integers 'x' and 'y'.
};

inline int FunctionsV1::myFunc(int x, int y)
{
    int output = 0;
    // ... do stuff with 'x' and 'y'.
    return output;
}
//..
// Notice that there are no checks on 'x' and 'y' within 'myFunc' and no
// assertions to detect use of 'myFunc' outside of its contract.  On the other
// hand, 'myFunc' is part of legacy code that has been in use extensively for
// years or decades, so clearly this is not causing a problem (yet).
//
// Upon reviewing this class you realize that 'myFunc' produces random results
// for values of 'x' or 'y' less than 0.  You, however, do not have enough
// information to conclude that no one is calling it with negative values and
// just using the bad results unknowingly.  There are a number of possibilities
// for how the result of this undefined behavior might be going unnoticed.
//: o The invalid value might be discarded by a bounds check later in the
//:   process.
//: o The invalid value may only result in a small glitch the users have not
//:   noticed or ignored.
//: o The resulting value may actually be valid, but allowing negative input
//:   for 'x' and 'y' may preclude potential future development in ways we do
//:   not want to allow.
// All of these are bad, but adding in checks with 'BSLS_ASSERT' that would
// replace these bad behaviors by process termination would turn silent errors
// into loud errors (potentially worse).  On the other hand, by not adding
// 'BSLS_ASSERT' checks we permit future misuses of this functions, which may
// not be innocuous, to potentially reach production systems.  'BSLS_REVIEW'
// here serves as a bridge, from the current state of 'myFunc' (entirely
// unchecked) to the ideal state of 'myFunc' (where misuse is caught loudly and
// immediately through 'BSLS_ASSERT'), following a path that doesn't risk
// turning an un-noticed or irrelevant error into one that will significantly
// hinder ongoing business.
//
// The solution to this is to *initially* reimplement 'myFunc' using
// 'BSLS_REVIEW' like this:
//..
// my_functions.h
// ...
#include <bsls_review.h>
// ...

class FunctionsV2 {
    // ...
  public:
    // ...

    static int myFunc(int x, int y);
        // Do something with the specified 'x' and 'y'.  The behavior is
        // undefined unless 'x > 0' and 'y > 0'.
};

inline int FunctionsV2::myFunc(int x, int y)
{
    BSLS_REVIEW(x > 0);
    BSLS_REVIEW(y > 0);
    int output = 0;
    // ... do stuff with 'x' and 'y'.
    return output;
}
//..
// Now you can deploy this code to production and then begin reviewing logs.
// The log messages you should look for are those produced by 'bsls::Review's
// default review failure handler and will be similar to:
//..
// ... BSLS_REVIEW Failure: 'x > 0' Please run "/bb/bin/showfunc.tsk ...
//..
// 'showfunc.tsk' is a Bloomberg application that can be used (along with the
// task binary) to convert the reported stack addresses to a more traditional
// stack trace with a function call stack.
//
// It is important to note that 'BSLS_REVIEW' is purely informative, and adding
// a review will not adversely affect behavior, and may in fact alert the
// library author to common client misconceptions about the intended behavior.
//
// For example, let's say actual usage makes it clear that users expect 0 to be
// valid values for the arguments to 'myFunc', and nothing in the
// implementation prevents us from accepting 0 as input and producing the
// answer clients expect.  Instead of changing all the clients, we may instead
// choose to change the function contract (and implemented checks):
//..
// my_functions.h
// ...
#include <bsls_review.h>
// ...

class FunctionsV3 {
    // ...
  public:
    // ...

    static int myFunc(int x, int y);
        // Do something with the specified 'x' and 'y'.  The behavior is
        // undefined unless 'x >= 0' and 'y >= 0'.
};

inline int FunctionsV3::myFunc(int x, int y)
{
    BSLS_REVIEW(x >= 0);
    BSLS_REVIEW(y >= 0);
    int output = 0;
    // ... do stuff with 'x' and 'y'.
    return output;
}
//..
// Finally, at some point, the implementation of 'myFunc' using 'BSLS_REVIEW'
// has been running a suitable amount of time that you are comfortable
// transitioning the use of 'bsls_review' to 'bsls_assert'.  We now use our
// favorite text editor or script to replace "BSLS_REVIEW" by "BSLS_ASSERT":
//..
// my_functions.h
// ...
//#include <bsls_assert.h>
#define BSLS_ASSERT(X) // This usage example references the higher-level
                       // 'bsls_assert' component which we cannot use here, so
                       // we just define the macro as a no-op for local
                       // testing.
// ...

class FunctionsV4 {
    // ...
  public:
    // ...

    static int myFunc(int x, int y);
        // Do something with the specified 'x' and 'y'.  The behavior is
        // undefined unless 'x >= 0' and 'y >= 0'.
};

inline int FunctionsV4::myFunc(int x, int y)
{
    BSLS_ASSERT(x >= 0);
    BSLS_ASSERT(y >= 0);
    int output = 0;
    // ... do stuff with 'x' and 'y'.
    return output;
}
#undef BSLS_ASSERT // Undefine this macro now that the usage example is
                   // finished.
//..
// At this point, any contract violations in the use of 'myFunc' in new code
// will be caught immediately (i.e., in appropriate build modes).

//=============================================================================
//                         ASSERT USAGE EXAMPLES
//-----------------------------------------------------------------------------
///Assert Usage Examples
///--------------
// The following examples illustrate (1) when to use each of the three kinds of
// (BSLS) "REVIEW" macros, (2) when and how to call the 'invokeHandler' method
// directly, (3) how to configure, at runtime, the behavior resulting from an
// review failure using "off-the-shelf" handler methods, (4) how to create your
// own custom review-failure handler function.  (5) proper use of
// 'bsls::ReviewFailureHandlerGuard' to install, temporarily, an
// exception-producing review handler, and (6) how "REVIEW" macros would be
// used in conjunction with portions of the code that are instantiated only
// when 'BDE_BUILD_TARGET_SAFE_2' is defined.
//
///1. Using 'BSLS_REVIEW', 'BSLS_REVIEW_SAFE', and 'BSLS_REVIEW_OPT'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component provides three different variants of (BSLS) "REVIEW" macros.
// This first usage example attempts to illustrate how one might select each of
// the particular variants, based on the runtime cost of the defensive check
// relative to that of the useful work being done, as well as the confidence
// that the defensive check will not actually fail regularly in production.
//
// Use of the 'BSLS_REVIEW_SAFE' macro is often appropriate when the defensive
// check occurs within the body of an inline function.  The 'BSLS_REVIEW_SAFE'
// macro minimizes the impact on runtime performance as it is instantiated only
// when requested (i.e., by building in "safe mode").  For example, consider a
// light-weight point class 'Kpoint' that maintains 'x' and 'y' coordinates in
// the range '[ -1000 ..  1000 ]':
//..
    // my_kpoint.h
    // ...

    class Kpoint {
        short int d_x;
        short int d_y;
      public:
        Kpoint(short int x, short int y);
        // ...
            // The behavior is undefined unless '-1000 <= x <= 1000' and
            // '-1000 <= y <= 1000'.
        // ...
    };

    // ...
//..
// Since the cost of validation here is significant compared with the useful
// work being done, we might choose to implement defensive checks using
// 'BSLS_REVIEW_SAFE' as follows:
//..
    // ...

    inline
    Kpoint::Kpoint(short int x, short int y)
    : d_x(x)
    , d_y(y)
    {
        BSLS_REVIEW_SAFE(-1000 <= x); BSLS_REVIEW_SAFE(x <= 1000);
        BSLS_REVIEW_SAFE(-1000 <= y); BSLS_REVIEW_SAFE(y <= 1000);
    }
//..
// For more substantial (non-'inline') functions, we would be more likely to
// use the 'BSLS_REVIEW' macro because the runtime overhead due to defensive
// checks is likely to be much less significant.  For example, consider a
// hash-table class that allows the client to resize the underlying table:
//..
    // my_hashtable.h
    // ...

    class HashTable {
        // ...
      public:
        // ...

        void resize(double loadFactor);
            // Adjust the size of the underlying hash table to be approximately
            // the current number of elements divided by the specified
            // 'loadFactor'.  The behavior is undefined unless
            // '0 < loadFactor'.
    };
//..
// Since the relative runtime cost of validating the input argument is quite
// small (e.g., less than 10%) compared to the typical work being done, we
// might choose to implement the defensive check using 'BSLS_REVIEW' as
// follows:
//..
    // my_hashtable.cpp
    // ...

    void HashTable::resize(double loadFactor)
    {
        // silence "unused parameter" warning in release builds:
        (void) loadFactor;
        BSLS_REVIEW(0 < loadFactor);

        // ...
    }
//..
// In some cases, the runtime cost of checking is always negligible when
// compared with the runtime cost of performing the useful work; moreover, the
// consequences of continuing in an undefined state for certain applications
// could be catastrophic.  Instead of using 'BSLS_REVIEW' in such cases, we
// might consider using 'BSLS_REVIEW_OPT'.  For example, suppose we have a
// financial application class 'TradingSystem' that performs trades.
//..
    // my_tradingsystem.h
    // ...

    class TradingSystem {
        // ...
      public:
        // ...
//..
// Let's further suppose that there is a particular method 'executeTrade' that
// takes, as a scaling factor, an integer that must be a multiple of 100 or the
// behavior is undefined (and might actually execute a trade):
//..

        void executeTrade(int scalingFactor);
            // Execute the current trade using the specified 'scalingFactor'.
            // The behavior is undefined unless '0 <= scalingFactor' and '100'
            // evenly divides 'scalingFactor'.
        // ...
     };
//..
// Because the cost of the two checks is likely not even measurable compared to
// the overhead of accessing databases and executing the trade, and because the
// consequences of specifying a bad scaling factor are virtually unbounded, we
// might choose to implement these defensive checks using 'BSLS_REVIEW_OPT' as
// follow:
//..
    // my_tradingsystem.cpp
    // ...

    void TradingSystem::executeTrade(int scalingFactor)
    {
        BSLS_REVIEW_OPT(0 <= scalingFactor);
        BSLS_REVIEW_OPT(0 == scalingFactor % 100);

        // ...
    }
//..
// Notice that in each case, the choice of which of the three (BSLS) "REVIEW"
// macros to use is governed primarily by the relative runtime cost compared
// with that of the useful work being done (and only secondarily by the
// potential consequences of continuing execution in an undefined state).
//
///2. When and How to Call the Invoke-Handler Method Directly
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// There may be times when we choose to invoke the currently installed
// review-failure handler directly -- i.e., instead of via one of the three
// (BSLS) "REVIEW" macros provided in this component.  Suppose that we are
// currently in the body of some function 'someFunc' and, for whatever reason,
// feel compelled to invoke the currently installed review-failure handler
// based on some criteria other than the current build mode.  The call might
// look as follows:
//..
    void someFunc(bool a, bool b, bool c)
    {
        bool someCondition = a && b && !c;

        if (someCondition) {
            BSLS_REVIEW_INVOKE("Bad News");
        }
    }
//..
// If presented with invalid arguments, 'someFunc' (above) will produce output
// similar to the following:
//..
//  Review failed: Bad News, file bsls_review.t.cpp, line 607
//  Abort (core dumped)
//..
//
///3. Runtime Configuration of the 'bsls::Review' Facility
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// By default, any review failure will result in the invocation of the
// 'bsls::Review::failAbort' handler function.  We can replace this behavior
// with that of one of the other static failure handler methods supplied in
// 'bsls::Review' as follows.  Let's assume we are at the top of our
// application called 'myMain' (which would typically be 'main'):
//..
    void myMain()
    {
//..
// First observe that the default review-failure handler function is, in fact,
// 'bsls::Review::failLog':
//..
        ASSERT(&bsls::Review::failLog == bsls::Review::failureHandler());
//..
// Next, we install a new review-failure handler function,
// 'bsls::Review::failSleep', from the suite of "off-the-shelf" handlers
// provided as 'static' methods of 'bsls::Review':
//..
        bsls::Review::setFailureHandler(&bsls::Review::failSleep);
//..
// Observe that 'bsls::Review::failSleep' is the new, currently-installed
// review-failure handler:
//..
        ASSERT(&bsls::Review::failSleep == bsls::Review::failureHandler());
//..
// Note that if we were to explicitly invoke the current review-failure handler
// as follows:
//..
//  bsls::ReviewViolation violation("message", "file", 27, "Test", 1);
//  bsls::Review::invokeHandler(violation);  // This will hang!
//..
// the program will hang since 'bsls::Review::failSleep' repeatedly sleeps for
// a period of time within an infinite loop.  Thus, this review-failure handler
// is useful for hanging a process so that a debugger may be attached to it.
//
// We may now decide to disable the 'setFailureHandler' method using the
// 'bsls::Review::lockReviewAdministration()' method to ensure that no one else
// will override our decision globally.  Note, however, that the
// 'bsls::ReviewFailureHandlerGuard' is not affected, and can still be used to
// supplant the currently installed handler (see below):
//..
        bsls::Review::lockReviewAdministration();
//..
// Attempting to change the currently installed handler now will fail:
//..
        bsls::Review::setFailureHandler(&bsls::Review::failAbort);

        ASSERT(&bsls::Review::failAbort != bsls::Review::failureHandler());

        ASSERT(&bsls::Review::failSleep == bsls::Review::failureHandler());
    }
//..
//
///4. Creating a Custom Review Handler
///- - - - - - - - - - - - - - - - - - -
// Sometimes, especially during testing, we may need to write our own custom
// review-failure handler function.  The only requirements are that the
// function have the same prototype (i.e., the same respective parameter and
// return types) as the 'bsls::Review::Handle' 'typedef', and that the function
// should not return (i.e., it must 'abort', 'exit', 'terminate', 'throw', or
// hang).  To illustrate, we will create a 'static' method at file scope that
// conforms to the required structure (notice the explicit use of 'std::printf'
// from '<cstdio>' instead of 'std::cout' from '<iostream>' to avoid
// interaction with the C++ memory allocation layer):
//..
    static bool globalEnableOurPrintingFlag = true;

    static void ourFailureHandler(const bsls::ReviewViolation &violation)
        // Print the expression 'text', 'file' name, and 'count' number from
        // the specified 'violation' to 'stdout' as a comma-separated list,
        // replacing null string-argument values with empty strings (unless
        // printing has been disabled by the 'globalEnableOurPrintingFlag'
        // variable), then unconditionally abort.
    {
        const char *text = violation.comment();
        if (!text) {
            text = "";
        }
        const char *file = violation.fileName();
        if (!file) {
            file = "";
        }
        if (globalEnableOurPrintingFlag) {
            std::printf("%s, %s, %d\n", text, file,
                        violation.lineNumber());
        }
#ifdef BDE_BUILD_TARGET_EXC
        throw std::exception();
#else
        std::abort();
#endif
    }
//..
// Now at the top level of our application:
//..
    void ourMain()
    {
//..
// First, let's observe that we can assign this new function to a function
// pointer of type 'bsls::Review::Handler':
//..
        bsls::Review::Handler f = &ourFailureHandler;
//..
// Now we can install it just as we would any any other handler:
//..
        bsls::Review::setFailureHandler(f);
//..
// Then we can create a violation instance to represent the violation we are
// about to invoke
//..
        bsls::ReviewViolation violation("str1", "str2", 3, "TEST", 1);
//..
// We can now invoke the default handler directly:
//..
        bsls::Review::invokeHandler(violation);
    }
//..
// With the resulting output as follows:
//..
//  str1, str2, 3
//  Abort (core dumped)
//..
//
///5. Using the 'bsls::ReviewFailureHandlerGuard'
///- - - - - - - - - - - - - - - - - - - - - - -
// Sometimes we may want to replace, temporarily (i.e., within some local
// lexical scope), the currently installed review-failure handler function.  In
// particular, we sometimes use the 'bsls::ReviewFailureHandlerGuard' class to
// replace the current handler with one that throws an exception (because we
// know that such an exception is safe in the local context).  Let's start with
// the simple factorial function below, which validates, in "debug mode" (or
// "safe mode"), that its input is non-negative:
//..
    double fact(int n)
        // Return 'n!'.  The behavior is undefined unless '0 <= n'.
    {
        BSLS_REVIEW(0 <= n);

        double result = 1.0;
        while (n > 1) {
            result *= n--;
        }
        return result;
    }
//..
// Now consider the following integer-valued 'extern "C"' C++ function,
// 'wrapperFunc', which can be called from C and FORTRAN, as well as from C++:
//..
    extern "C" int wrapperFunc(bool verboseFlag)
    {
        enum { GOOD = 0, BAD } result = GOOD;
//..
// The purpose of this function is to allow review failures in subroutine calls
// below this function to be handled by throwing an exception, which is then
// caught by the wrapper and reported to the caller as a "bad" status.  Hence,
// when within the runtime scope of this function, we want to install,
// temporarily, the review-failure handler 'bsls::Review::failThrow', which,
// when invoked, causes an 'bsls::AssertTestException' object to be thrown.
// (Note that we are not advocating this approach for "recovery", but rather
// for an orderly shut-down, or perhaps during testing.)  The
// 'bsls::ReviewFailureHandlerGuard' class is provided for just this purpose:
//..
    ASSERT(&bsls::Review::failLog == bsls::Review::failureHandler());

    bsls::ReviewFailureHandlerGuard guard(&bsls::Review::failThrow);

    ASSERT(&bsls::Review::failThrow == bsls::Review::failureHandler());
//..
// Next we open up a 'try' block, and somewhere within the 'try' we
// "accidentally" invoke 'fact' with an out-of-contract value (i.e., '-1'):
//..
    #ifdef BDE_BUILD_TARGET_EXC
        try
    #endif
            {

            // ...

            double d = fact(-1);        // Out-of-contract call to 'fact'.
            (void) d;

            // ...
        }
    #ifdef BDE_BUILD_TARGET_EXC
        catch (const bsls::AssertTestException& e) {
            result = BAD;
            if (verboseFlag) {
                printf( "Internal Error: %s, %s, %d\n",
                        e.expression(),
                        e.filename(),
                        e.lineNumber() );
            }
        }
    #endif
        return result;
    }
//..
// Assuming exceptions are enabled (i.e., 'BDE_BUILD_TARGET_EXC' is defined),
// if an 'bsls::AssertTestException' occurs below this wrapper function, the
// exception will be caught, a message will be printed to 'stdout', e.g.,
//..
//  Internal Error: bsls_review.t.cpp:500: 0 <= n
//..
// and the 'wrapperFunc' function will return a bad status (i.e., 1) to its
// caller.  Note that if exceptions are not enabled, 'bsls::Review::failThrow'
// will behave as 'bsls::Review::failAbort', and dump core immediately:
//..
// Review failed: 0 <= n, file bsls_review.t.cpp, line 500 Abort (core dumped)
//..
// Finally note that the 'bsls::ReviewFailureHandlerGuard' is not thread-aware.
// In particular, a guard that is created in one thread will also affect the
// failure handlers that are used in other threads.  Care should be taken when
// using this guard when more than a single thread is executing.
//
/// 6. Using (BSLS) "REVIEW" Macros in Conjunction w/ 'BDE_BUILD_TARGET_SAFE_2'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Recall that reviews do not affect binary compatibility; however, software
// built with 'BDE_BUILD_TARGET_SAFE_2' defined need not be binary compatible
// with software built otherwise.  In this final example, we look at how we
// might use the (BSLS) "REVIEW" family of macro's in conjunction with code
// that is incorporated (at compile time) only when the
// 'BDE_BUILD_TARGET_SAFE_2' is defined.
//
// As a simple example, let's consider an elided implementation of a
// singly-linked integer list and its iterator.  Whenever
// 'BDE_BUILD_TARGET_SAFE_2' is defined, we want to defend against the
// possibility that a client mistakenly passes a 'ListIter' object into a
// 'List' object method (e.g., 'List::insert') where that 'ListIter' object did
// not originate from the same 'List' object.
//
// We'll start by defining a local helper 'List_Link' 'struct' as follows:
//..
    struct List_Link {
        List_Link *d_next_p;
        int        d_data;
        List_Link(List_Link *next, int data) : d_next_p(next), d_data(data) { }
    };
//..
// Next, we'll define 'ListIter', which always identifies the current position
// in a sequence of links, but whenever 'BDE_BUILD_TARGET_SAFE_2' is defined,
// also maintains a pointer to its parent 'List' object:
//..
    class List;                         // Forward declaration.

    class ListIter {
    #ifdef BDE_BUILD_TARGET_SAFE_2
        List *d_parent_p;               // Exists only in "safe 2 mode."
    #endif
        List_Link **d_current_p;
        friend class List;
        friend bool operator==(const ListIter&, const ListIter&);
      private:
        ListIter(List_Link **current,
                 List *
    #ifdef BDE_BUILD_TARGET_SAFE_2
                       parent           // Not used unless in "safe 2 mode."
    #endif
                )
        : d_current_p(current)
    #ifdef BDE_BUILD_TARGET_SAFE_2
        , d_parent_p(parent)            // Initialize only in "safe 2 mode."
    #endif
        { }
      public:
        ListIter& operator++() { /* ... */ return *this; }
        // ...
    };
    bool operator==(const ListIter& lhs, const ListIter& rhs);
    bool operator!=(const ListIter& lhs, const ListIter& rhs);
//..
// Finally we define the 'List' class itself with most of the operations
// elided; the methods of particular interest here are 'begin' and 'insert':
//..

    class List {
        List_Link *d_head_p;
      public:
        // CREATORS
        List() : d_head_p(0) { }
        List(const List& /* original */) { /* ... */ }
        ~List() { /* ... */ }

        // MANIPULATORS
        List& operator=(const List& /* rhs */) { /* ... */ return *this; }

        //| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
        //v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        ListIter begin()
            // Return an iterator referring to the beginning of this list.
        {
            return ListIter(&d_head_p, this);
        }
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        //| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
        //v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        void insert(const ListIter& position, int data)
            // Insert the specified 'data' value into this list at the
            // specified 'position'.
        {
    #ifdef BDE_BUILD_TARGET_SAFE_2
            BSLS_REVIEW_SAFE(this == position.d_parent_p);  // "safe 2 mode"
    #endif
            *position.d_current_p = new List_Link(*position.d_current_p, data);
        }
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // ACCESSORS
        void print()
        {
            printf( "[" );
            for (List_Link *p = d_head_p; p; p = p->d_next_p) {
                printf(" %d", p->d_data);
            }
            printf(" ]\n");
        }
    };
//..
// Outside of "safe 2 mode", it is possible to pass an iterator object obtained
// from the 'begin' method of one 'List' object into the 'insert' method of
// another, having, perhaps, unexpected results:
//..
    void sillyFunc(bool printFlag)
    {
        List     a;
        ListIter aIt = a.begin();
        a.insert(aIt, 1);
        a.insert(aIt, 2);
        a.insert(aIt, 3);

        if (printFlag) {
            printf( "a = "); a.print();
        }

        List     b;
        ListIter bIt = b.begin();
        a.insert(bIt, 4);       // Oops! Should have been: 'b.insert(bIt, 4);'
        a.insert(bIt, 5);       // Oops!   "     "     "   '    "     "   5  '
        a.insert(bIt, 6);       // Oops!   "     "     "   '    "     "   6  '

        if (printFlag) {
            printf( "a = "); a.print();
            printf( "b = "); b.print();
        }
    }
//..
// In the example above, we have "accidentally" passed the iterator 'bIt'
// obtained from 'List' object 'b' into the 'insert' method for List object
// 'a'.  The resulting undefined behavior (in other than "safe 2 mode") might
// produce output that looks as follows:
//..
//  a = [ 3 2 1 ]
//  a = [ 3 2 1 ]
//  b = [ 6 5 4 ]
//..
// If the same 'sillyFunc' were compiled in "safe 2 mode" (i.e., with
// 'BDE_BUILD_TARGET_SAFE_2' defined) the undefined behavior would be detected
// and the output would, by default look more like the following:
//..
//  a = [ 3 2 1 ]
//  Review failed: this == position.d_parent_p, file my_list.cpp, line 56
//  Abort (core dumped)
//..
// Thereby quickly exposing the misuse by the client.

// End of usage examples
// BDE_VERIFY pragma: pop

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

            globalVerbose =         verbose;
        globalVeryVerbose =     veryVerbose;
    globalVeryVeryVerbose = veryVeryVerbose;

    printf( "TEST %s CASE %d\n", __FILE__, test);

    switch (test) { case 0:  // zero is always the leading case
      case 15: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #1
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with "ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE: Adding 'BSLS_ASSERT' to an existing function
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE #1"
                            "\n================\n");

        if (verbose) printf("\n1. introducing a new bsls_assert.\n");

        ASSERT(&bsls::Review::failLog == bsls::Review::failureHandler());

        bsls::ReviewFailureHandlerGuard guard(&bsls::Review::failThrow);
        ASSERT(&bsls::Review::failThrow == bsls::Review::failureHandler());


        if (verbose) printf("\n2. Running myFunc with valid arguments.\n");
        FunctionsV1::myFunc(1,1);
        FunctionsV2::myFunc(1,1);
        FunctionsV3::myFunc(1,1);
        FunctionsV4::myFunc(1,1);

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // ASSERT USAGE EXAMPLE #6
        //
        // Concerns:
        //: 1 The usage example provided in the bsls_assert header must
        //:   compile, link and run with BSLS_REVIEW instead.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   ASSERT USAGE EXAMPLE: Using 'BDE_BUILD_TARGET_SAFE_2'
        // --------------------------------------------------------------------

        if (verbose) printf("\nASSERT USAGE EXAMPLE #6"
                            "\n=======================\n");

        if (verbose) printf("\n6. Using (BSLS) \"REVIEW\" Macros in "
                            "Conjunction with BDE_BUILD_TARGET_SAFE_2\n");

        // See usage examples section at top of this file.

#ifndef BDE_BUILD_TARGET_SAFE_2
        if (veryVerbose) printf( "\tsafe mode 2 is *not* defined\n" );
        sillyFunc(veryVerbose);
#else
        if (veryVerbose) printf( "\tSAFE MODE 2 *is* defined.\n" );

        // bsls::Review::setFailureHandler(::testDriverPrint);
                                                          // for usage example
        bsls::Review::setFailureHandler(::testDriverHandler);
                                                          // for regression
        globalReset();
        ASSERT(false == globalReviewFiredFlag);
#ifdef BDE_BUILD_TARGET_EXC
        try
        {
#endif  // BDE_BUILD_TARGET_EXC
            sillyFunc(veryVerbose);
            ASSERT(false);
#ifdef BDE_BUILD_TARGET_EXC
        }
        catch(const std::exception &)
        {
            ASSERT(true == globalReviewFiredFlag);
        }
#endif  // BDE_BUILD_TARGET_EXC
#endif  // BDE_BUILD_TARGET_SAFE_2
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // ASSERT USAGE EXAMPLE #5
        //
        // Concerns:
        //: 1 The usage example provided in the bsls_assert header must
        //:   compile, link and run with BSLS_REVIEW instead.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters.
        //
        // Testing:
        //   ASSERT USAGE EXAMPLE: Using Scoped Guard
        // --------------------------------------------------------------------

        if (verbose) printf( "\nASSERT USAGE EXAMPLE #5"
                             "\n=======================\n" );

        if (verbose) printf( "\n5. Using the "
                             "bsls::ReviewFailureHandlerGuard\n" );


        // See usage examples section at top of this file.

        ASSERT(&bsls::Review::failLog == bsls::Review::failureHandler());

#ifndef BDE_BUILD_TARGET_OPT
    #if defined(BDE_BUILD_TARGET_EXC) ||                                      \
        defined(BSLS_REVIEW_ENABLE_TEST_CASE_10)

        if (verbose) printf( "\n*** Note that the following 'Internal "
                             "Error: ... '0 <= n' message is expected:\n" );

        ASSERT(0 != wrapperFunc(verbose));

    #endif
#endif
        ASSERT(&bsls::Review::failLog == bsls::Review::failureHandler());

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // ASSERT USAGE EXAMPLE #4
        //
        // Concerns:
        //: 1 The usage example provided in the bsls_assert header must
        //:   compile, link and run with BSLS_REVIEW instead.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters.
        //
        // Testing:
        //   ASSERT USAGE EXAMPLE: Creating Your Own Review-Handler
        // --------------------------------------------------------------------

        if (verbose) printf( "\nASSERT USAGE EXAMPLE #4"
                             "\n=======================\n" );

#ifndef BDE_BUILD_TARGET_EXC
        if (verbose) {
            printf( "\tTest disabled as exceptions are NOT enabled.\n"
                    "\tCalling the test function would abort.\n" );
        }

#else
        if (verbose) printf("\n4. Creating a Custom Review Handler\n" );


        // See usage examples section at top of this file.

        globalEnableOurPrintingFlag = veryVerbose;

        REVIEW_TEST_BEGIN
        ourMain();
        REVIEW_TEST_END
#endif
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // ASSERT USAGE EXAMPLE #3
        //
        // Concerns:
        //: 1 The usage example provided in the bsls_assert header must
        //:   compile, link and run with BSLS_REVIEW instead.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters.
        //
        // Testing:
        //   ASSERT USAGE EXAMPLE: Using Administration Functions
        //   ASSERT USAGE EXAMPLE: Installing Prefabricated Review-Handlers
        // --------------------------------------------------------------------

        if (verbose) printf( "\nASSERT USAGE EXAMPLE #3"
                             "\n=======================\n" );

        if (verbose) printf( "\n3. Runtime Configuration of the "
                             "bsls::Review Facility\n" );

        // See usage examples section at top of this file.

        myMain();

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // ASSERT USAGE EXAMPLE #2
        //
        // Concerns:
        //: 1 The usage example provided in the bsls_assert header must
        //:   compile, link and run with BSLS_REVIEW instead.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters.
        //
        // Testing:
        //   ASSERT USAGE EXAMPLE: Invoking an review handler directly
        // --------------------------------------------------------------------

        if (verbose) printf( "\nASSERT USAGE EXAMPLE #2"
                             "\n=======================\n" );

        if (verbose) printf( "\n2. When and How to Call the Invoke-Handler "
                             "Method Directly\n" );

        // See usage examples section at top of this file.

        bsls::Review::setFailureHandler(::testDriverPrint);

#ifndef BDE_BUILD_TARGET_EXC
        globalReturnOnTestReview = true;
#endif

        REVIEW_TEST_BEGIN
        someFunc(1, 1, 0);
        REVIEW_TEST_END

#ifndef BDE_BUILD_TARGET_EXC
        globalReturnOnTestReview = false;
#endif

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // ASSERT USAGE EXAMPLE #1
        //
        // Concerns:
        //: 1 The usage example provided in the bsls_assert header must
        //:   compile, link and run with BSLS_REVIEW instead.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters.
        //
        // Testing:
        //   ASSERT USAGE EXAMPLE: Using Review Macros
        // --------------------------------------------------------------------

        if (verbose) printf( "\nASSERT USAGE EXAMPLE #1"
                             "\n=======================\n" );

        if (verbose) printf( "\n1. Using BSLS_REVIEW, BSLS_REVIEW_SAFE, and "
                             "BSLS_REVIEW_OPT\n");

        // See usage examples section at top of this file.

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // DEFAULT HANDLER LOG: BACKOFF
        //
        // Concerns:
        //: 1 The default (failLog) handler should back off exponentially.
        //
        // Plan:
        //: 1 Covered by test case 7 and -4
        //
        // Testing:
        //    CONCERN: default handler log: backoff
        // --------------------------------------------------------------------

        if (verbose) printf( "\nDEFAULT HANDLER LOG: BACKOFF"
                             "\n============================\n" );

        // low values of the backoff skipping are checked by case 7, high
        // values and the periodic repeating at 2^29 are checked by case -4.

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // DEFAULT HANDLER LOG: CONTENT
        //
        // Concerns:
        //: 1 'failLog' should log log a message via 'bsls_log' containing call
        //:   site information.
        //:
        //: 2 The log message should identify the file and line where the
        //:   failed review occurred.
        //:
        //: 3 The log message should be severity 'error'.  // CHECK
        //:
        // Plan:
        //: 1 set the log message handler to recordingLogMessageHandler
        //:
        //: 2 invoke the default log handler in a loop and verify the contents
        //:   of message, verify that the message is skipped on the 3rd
        //:   invocation.
        //
        // Testing:
        //   CONCERN: default handler log: content
        // --------------------------------------------------------------------

        if (verbose) printf( "\nDEFAULT HANDLER LOG: CONTENT"
                             "\n============================\n" );

        bsls::ReviewFailureHandlerGuard guard(bsls::Review::failLog);
        bsls::Log::setLogMessageHandler(
            HandlerLoggingTest::recordingLogMessageHandler);

        int prevSkipped = 0;
        int skipped = 0;
        for (long i = 0; i < 64; ++i) {
            if (veryVeryVerbose) printf( "\nTesting invocation %ld"
                                         " of default handler\n" , i);

            HandlerLoggingTest::clear();
            BloombergLP::bsls::Types::Int64 prev_count =
                HandlerLoggingTest::s_loggerInvocationCount;

            int line = L_ + 1;
            BSLS_REVIEW_INVOKE("OUTPUT TEST");

            if (prev_count == HandlerLoggingTest::s_loggerInvocationCount) {
                if (veryVeryVerbose) {
                    printf( "Skipped logger invocation: " );
                    P_(prev_count);
                    P_(skipped);
                    P(HandlerLoggingTest::s_loggerInvocationCount);
                }

                ++skipped;
                ASSERT( !IS_POWER_OF_TWO( i+1 ) );
            }
            else {
                if (veryVerbose) {
                    printf( "Logger invocation: " );
                    P_(prev_count);
                    P_(skipped);
                    P(HandlerLoggingTest::s_loggerInvocationCount);
                }

                // check things
                ASSERT( IS_POWER_OF_TWO( i+1 ) );

                LogProfile &profile = HandlerLoggingTest::lastProfile();
                LOOP2_ASSERT( line, profile.d_line, line == profile.d_line );

                const char *file = __FILE__;
                LOOP2_ASSERT(file, profile.d_file,
                             0 == std::strcmp(file, profile.d_file));

                if (skipped > 0) {
                    char skipText[32];
                    sprintf(skipText,"skipped: %d", skipped);

                    ASSERT( NULL != std::strstr(profile.d_text, skipText) );

                    if (prevSkipped == (1 << 29)) {
                        LOOP2_ASSERT( prevSkipped, skipped,
                                      skipped == prevSkipped );
                    }
                    else {
                        LOOP2_ASSERT( prevSkipped, skipped,
                                      skipped == prevSkipped*2 + 1 );
                    }
                }
                else {
                    ASSERT( NULL == std::strstr(profile.d_text, "skipped") );
                }

                prevSkipped = skipped;
                skipped = 0;
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // CONFIGURATION MACROS
        //
        // Concerns:
        //: 1 The configuration macros that report on which review facilities
        //:   are available in the current build mode might not report
        //:   correctly in all build modes.
        //
        // Plan:
        //: 1 Subvert the regular include guards to verify that inclusion of
        //:   'bsls_review.h' defines the proper macros when included with
        //:   varying build modes.
        //
        // Testing:
        //   BSLS_REVIEW_SAFE_IS_ACTIVE
        //   BSLS_REVIEW_IS_ACTIVE
        //   BSLS_REVIEW_OPT_IS_ACTIVE
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCONFIGURATION MACROS"
                             "\n====================\n" );

        TestConfigurationMacros();
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // FAILURE HANDLER GUARD
        //
        // Concerns:
        //: 1 That the guard swaps new for current handler at construction.
        //:
        //: 2 Restores the original one at destruction.
        //:
        //: 3 That guards can nest.
        //:
        //: 4 That 'lockReviewAdministration' has no effect on guard.
        //:
        //: 5 This applies equally to both the review and backoff handlers.
        //
        // Plan:
        //: 1 Create a guard, passing it the 'testDriverHandler' handler, and
        //:   verify, using 'failureHandler', that this new handler was
        //:   installed.  Then lock the administration, and repeat in nested
        //:   fashion with the 'failSleep' handler.  Verify restoration on the
        //:   way out.
        //
        // Testing:
        //   class bsls::ReviewFailureHandlerGuard
        //   ReviewFailureHandlerGuard::ReviewFailureHandlerGuard(Handler)
        //   ReviewFailureHandlerGuard::~ReviewFailureHandlerGuard()
        //   CONCERN: that locking does not stop the handlerGuard from working
        // --------------------------------------------------------------------

        if (verbose) printf( "\nFAILURE HANDLER GUARD"
                             "\n=====================\n" );

        if (verbose) printf( "\nVerify initial review failure handler.\n" );

        ASSERT(bsls::Review::failLog == bsls::Review::failureHandler());

        if (verbose) printf( "\nCreate guard with 'testDriverHandler' "
                             "handler.\n" );
        {
            bsls::ReviewFailureHandlerGuard guard(::testDriverHandler);

            if (verbose) printf( "\nVerify new review handler.\n" );

            ASSERT(::testDriverHandler == bsls::Review::failureHandler());

            if (verbose) printf( "\nLock administration.\n" );

            bsls::Review::lockReviewAdministration();

            if (verbose) printf( "\nRe-verify new review handler.\n" );

            ASSERT(testDriverHandler == bsls::Review::failureHandler());

            if (verbose) printf( "\nCreate second guard with 'failSleep' "
                                 "handler.\n");

            {
                bsls::ReviewFailureHandlerGuard guard(bsls::Review::failSleep);

                if (verbose) printf( "\nVerify newer review handler.\n" );

                ASSERT(bsls::Review::failSleep ==
                                               bsls::Review::failureHandler());

                if (verbose) printf( "\nDestroy guard created with "
                                     "'::failSleep' handler.\n" );
            }

            if (verbose) printf( "\nVerify new review handler.\n" );

            ASSERT(::testDriverHandler == bsls::Review::failureHandler());

            if (verbose) printf( "\nDestroy guard created with "
                                 "'::testDriverHandler' handler.\n" );
        }

        if (verbose) printf( "\nVerify initial review handler.\n" );

        ASSERT(bsls::Review::failLog == bsls::Review::failureHandler());


      } break;
      case 4: {
        // --------------------------------------------------------------------
        // REVIEW FAILURE HANDLERS
        //
        // Concerns:
        //: 1 That each of the review failure handlers provided herein behaves
        //:   as advertised and (at least) matches the signature of the
        //:   'bsls::Review::Handler' 'typedef'
        //
        // Plan:
        //: 1 Verify each handler's behavior.  Unfortunately, we cannot test
        //:   functions that abort except by hand (see negative test cases).
        //:
        //: 2 Assign each handler function to a pointer of type 'Handler'.
        //
        // Testing:
        //   typedef void (*Handler)(const char *, const char *, int);
        //   static void failLog(const ReviewViolation &violation);
        //   static void failAbort(const ReviewViolation &violation);
        //   static void failThrow(const ReviewViolation &violation);
        //   static void failSleep(const ReviewViolation &violation);
        // --------------------------------------------------------------------

        if (verbose) printf( "\nREVIEW FAILURE HANDLERS"
                             "\n=======================\n" );

        if (verbose) printf( "\nTesting 'void failLog(const char *t, "
                             "const char *f, int line);'\n" );
        {
            bsls::Review::Handler f = bsls::Review::failLog;
            (void) f;

            if (veryVerbose) {
                printf( "\t(Logging behavior must be tested by "
                        "other cases.)\n" );
            }
        }

        if (verbose) printf( "\nTesting 'void failAbort(const char *t, "
                             "const char *f, int line);'\n" );
        {
            bsls::Review::Handler f = bsls::Review::failAbort;
            (void) f;

            if (veryVerbose) {
                printf( "\t(Aborting behavior must be tested by hand.)\n" );
            }
        }

        if (verbose) printf( "\nTesting 'void failThrow(const char *t, "
                             "const char *f, int line);'\n" );
        {

            bsls::Review::Handler f = bsls::Review::failThrow;

#ifdef BDE_BUILD_TARGET_EXC
            const char *text = "Test text";
            const char *file = "bsls_review.t.cpp";
            int         line = 101;

            if (veryVerbose) {
                printf( "\tExceptions ARE enabled.\n" );
            }

            try {
                bsls::ReviewViolation violation(text, file, line, "TEST", 1);
                f(violation);
            }
            catch (bsls::AssertTestException) {
                if (veryVerbose) printf( "\tException Text Succeeded!\n" );
            }
            catch (...) {
                ASSERT("Through wrong exception!" && 0);
            }
#else
            if (veryVerbose) {
                printf( "\tExceptions are NOT enabled.\n" );
            }
#endif
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // INCOMPATIBLE BUILD TARGETS
        //
        // Concerns:
        //: 1 Any component including 'bsls_review.h' that has multiple
        //:   review-mode flags defined should fail to compile and provide a
        //:   useful diagnostic.
        //
        // Plan:
        //: 1 Repeat the reviews at runtime, and fail if any two macros
        //:   incompatible macros are present.
        //:
        //: 2 (Manually) observe that the test is in the bsls_review.h file.
        //
        // Testing:
        //   CONCERN: ubiquitously detect multiply-defined review-mode flags
        // --------------------------------------------------------------------

        if (verbose) printf( "\nINCOMPATIBLE BUILD TARGETS"
                             "\n==========================\n" );

        if (verbose) printf( "\nExtract defined-ness of each target.\n" );

#ifdef BSLS_REVIEW_LEVEL_REVIEW_SAFE
        bool a = 1;
#else
        bool a = 0;
#endif

#ifdef BSLS_REVIEW_LEVEL_REVIEW
        bool b = 1;
#else
        bool b = 0;
#endif

#ifdef BSLS_REVIEW_LEVEL_REVIEW_OPT
        bool c = 1;
#else
        bool c = 0;
#endif

#ifdef BSLS_REVIEW_LEVEL_NONE
        bool d = 1;
#else
        bool d = 0;
#endif

        if (veryVerbose) printf( "\tVerify all combinations redundantly.\n" );

        LOOP2_ASSERT(a, b, !(a&b));
        LOOP2_ASSERT(a, c, !(a&c));
        LOOP2_ASSERT(a, d, !(a&d));

        LOOP2_ASSERT(b, a, !(b&a));
        LOOP2_ASSERT(b, c, !(b&c));
        LOOP2_ASSERT(b, d, !(b&d));

        LOOP2_ASSERT(c, a, !(c&a));
        LOOP2_ASSERT(c, b, !(c&b));
        LOOP2_ASSERT(c, d, !(c&d));

        LOOP2_ASSERT(d, a, !(d&a));
        LOOP2_ASSERT(d, b, !(d&b));
        LOOP2_ASSERT(d, c, !(d&c));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // (BSLS) "REVIEW"-MACRO TEST
        //
        // Concerns:
        //: 1 Each review macro instantiates only when the appropriate build
        //:   mode is set.
        //:
        //: 2 When instantiated, each macro fires only on "0" valued
        //:   expressions
        //:
        //: 3 When a macro fires, the correct text, line, and file are
        //:   transmitted to the current review-failure handler.
        //:
        //: 4 That the expression text that is printed is exactly what is in
        //:   the parentheses of the macro.
        //
        // Plan:
        //: 1 We must not try to change build targets (or review modes), but
        //:   just observe them to see which review macros should be
        //:   instantiated.
        //:
        //: 2 When enabled, we need to try each of the macros on each of the
        //:   (four) kinds of expression text arguments to make sure that it
        //:   fires only on 'false' and '(void*)(0)' and not 'true' or
        //:   '(void*)(1)'.
        //:
        //: 3 In each case for 2.  (above) that fires, we will observe that the
        //:   expression text, file name, and line number are correct.
        //:
        //: 4 Make sure that we vary the text in the expression (and include
        //:   embedded whitespace (we don't care about leading or trailing
        //:   whitespace).
        //
        // Testing:
        //   BSLS_REVIEW_SAFE(X)
        //   BSLS_REVIEW(X)
        //   BSLS_REVIEW_OPT(X)
        //   BSLS_REVIEW_INVOKE(X)
        //   CONCERN: REVIEW macros are instantiated properly for build targets
        //   CONCERN: all combinations of BDE_BUILD_TARGETs are allowed
        //   CONCERN: any one review mode overrides all BDE_BUILD_TARGETs
        // --------------------------------------------------------------------

        if (verbose) printf( "\n(BSLS) \"REVIEW\"-MACRO TEST"
                             "\n==========================\n" );

        if (verbose) printf( "\nInstall 'testDriverHandler' "
                             "review-handler.\n" );

        bsls::Review::setFailureHandler(&testDriverHandler);
        ASSERT(::testDriverHandler == bsls::Review::failureHandler());

        if (veryVerbose) printf( "\tSet up all but line numbers now. \n" );

        const void *p      = 0;
        const char *text   = "p";
        const char *file   = __FILE__;
        const char *level = bsls::Review::k_LEVEL_SAFE;
        int         line   = -1;
        int         count  = 1; // always first review call for all of these
                                 // invocations.

        const char *expr = "false == true";

#ifndef BDE_BUILD_TARGET_EXC
        globalReturnOnTestReview = true;
#endif

        if (verbose) {
            printf( "\nCurrent build-mode settings:\n" );

#ifdef BDE_BUILD_TARGET_SAFE_2
            printf( "\t1 == BDE_BUILD_TARGET_SAFE_2\n" );
#else
            printf( "\t0 == BDE_BUILD_TARGET_SAFE_2\n" );
#endif

#ifdef BDE_BUILD_TARGET_SAFE
            printf( "\t1 == BDE_BUILD_TARGET_SAFE\n" );
#else
            printf( "\t0 == BDE_BUILD_TARGET_SAFE\n" );
#endif

#ifdef BDE_BUILD_TARGET_DBG
            printf( "\t1 == BDE_BUILD_TARGET_DBG\n" );
#else
            printf( "\t0 == BDE_BUILD_TARGET_DBG\n" );
#endif

#ifdef BSLS_REVIEW_LEVEL_REVIEW_SAFE
            printf( "\t1 == BSLS_REVIEW_LEVEL_REVIEW_SAFE\n" );
#else
            printf( "\t0 == BSLS_REVIEW_LEVEL_REVIEW_SAFE\n" );
#endif

#ifdef BSLS_REVIEW_LEVEL_REVIEW
            printf( "\t1 == BSLS_REVIEW_LEVEL_REVIEW\n" );
#else
            printf( "\t0 == BSLS_REVIEW_LEVEL_REVIEW\n" );
#endif

#ifdef BSLS_REVIEW_LEVEL_REVIEW_OPT
            printf( "\t1 == BSLS_REVIEW_LEVEL_REVIEW_OPT\n" );
#else
            printf( "\t0 == BSLS_REVIEW_LEVEL_REVIEW_OPT\n" );
#endif

#ifdef BSLS_REVIEW_LEVEL_NONE
            printf( "\t1 == BSLS_REVIEW_LEVEL_NONE\n" );
#else
            printf( "\t0 == BSLS_REVIEW_LEVEL_NONE\n" );
#endif

#if IS_BSLS_REVIEW_MODE_FLAG_DEFINED
            printf( "\t1 == IS_BSLS_REVIEW_MODE_FLAG_DEFINED\n" );
#else
            printf( "\t0 == IS_BSLS_REVIEW_MODE_FLAG_DEFINED\n" );
#endif

#if BSLS_NO_REVIEW_MACROS_DEFINED
            printf( "\t1 == BSLS_NO_REVIEW_MACROS_DEFINED\n" );
#else
            printf( "\t0 == BSLS_NO_REVIEW_MACROS_DEFINED\n" );
#endif
        }

        //_____________________________________________________________________
        //            BSLS_REVIEW_SAFE, BSLS_REVIEW, BSLS_REVIEW_OPT
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if defined(BSLS_REVIEW_LEVEL_REVIEW_SAFE)                                   \
 || !IS_BSLS_REVIEW_MODE_FLAG_DEFINED && (                                   \
        defined(BDE_BUILD_TARGET_SAFE_2) ||                                  \
        defined(BDE_BUILD_TARGET_SAFE)   )

        if (verbose) printf( "\nEnabled: REVIEW_SAFE, REVIEW, REVIEW_OPT\n" );

        if (veryVerbose) printf( "\tCheck for integer expression.\n" );

        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_SAFE(0);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);

        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW     (0);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);

        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (0);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);

        if (veryVerbose) printf( "\tCheck for pointer expression.\n" );

        globalReset();
        level = bsls::Review::k_LEVEL_SAFE;
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_SAFE(p);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(text, globalText,   0 == std::strcmp(text, globalText));
        LOOP2_ASSERT(file, globalFile,   0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        globalReset();
        level = bsls::Review::k_LEVEL_REVIEW;
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW     (p);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(text, globalText,    0 == std::strcmp(text, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        globalReset();
        level = bsls::Review::k_LEVEL_OPT;
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (p);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(text, globalText,    0 == std::strcmp(text, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        if (veryVerbose) printf( "\tCheck for expression with spaces.\n" );

        globalReset();
        level = bsls::Review::k_LEVEL_SAFE;
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_SAFE(false == true);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(expr, globalText,    0 == std::strcmp(expr, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        globalReset();
        level = bsls::Review::k_LEVEL_REVIEW;
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW     (false == true);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(expr, globalText,    0 == std::strcmp(expr, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        globalReset();
        level = bsls::Review::k_LEVEL_OPT;
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (false == true);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(expr, globalText,    0 == std::strcmp(expr, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);
#endif

        //_____________________________________________________________________
        //                    BSLS_REVIEW, BSLS_REVIEW_OPT
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if defined(BSLS_REVIEW_LEVEL_REVIEW)                                        \
 || !IS_BSLS_REVIEW_MODE_FLAG_DEFINED &&                                     \
        !defined(BDE_BUILD_TARGET_OPT) &&                                    \
        !defined(BDE_BUILD_TARGET_SAFE) &&                                   \
        !defined(BDE_BUILD_TARGET_SAFE_2)

        if (verbose) printf( "\nEnabled: REVIEW, REVIEW_OPT\n" );

        if (veryVerbose) printf( "\tCheck for integer expression.\n" );

        globalReset(); BSLS_REVIEW_SAFE(0); ASSERT(0 == globalReviewFiredFlag);

        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW     (0);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);

        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (0);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);

        if (veryVerbose) printf( "\tCheck for pointer expression.\n" );

        globalReset(); BSLS_REVIEW_SAFE(p); ASSERT(0 == globalReviewFiredFlag);

        globalReset();
        level = bsls::Review::k_LEVEL_REVIEW;
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW     (p);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(text, globalText,    0 == std::strcmp(text, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        globalReset();
        level = bsls::Review::k_LEVEL_OPT;
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (p);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(text, globalText,    0 == std::strcmp(text, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        if (veryVerbose) printf( "\tCheck for expression with spaces.\n" );

        globalReset(); BSLS_REVIEW_SAFE(false == true);
        ASSERT(0 == globalReviewFiredFlag);

        globalReset();
        level = bsls::Review::k_LEVEL_REVIEW;
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW     (false == true);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(expr, globalText,    0 == std::strcmp(expr, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        globalReset();
        level = bsls::Review::k_LEVEL_OPT;
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (false == true);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(expr, globalText,    0 == std::strcmp(expr, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);
#endif

        //_____________________________________________________________________
        //                         BSLS_REVIEW_OPT
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if defined(BSLS_REVIEW_LEVEL_REVIEW_OPT)                                    \
 || !IS_BSLS_REVIEW_MODE_FLAG_DEFINED &&                                     \
        defined(BDE_BUILD_TARGET_OPT) &&                                     \
        !defined(BDE_BUILD_TARGET_SAFE) &&                                   \
        !defined(BDE_BUILD_TARGET_SAFE_2)

        if (verbose) printf( "\nEnabled: REVIEW_OPT\n" );

        if (veryVerbose) printf( "\tCheck for integer expression.\n" );

        globalReset(); BSLS_REVIEW_SAFE(0); ASSERT(0 == globalReviewFiredFlag);
        globalReset(); BSLS_REVIEW     (0); ASSERT(0 == globalReviewFiredFlag);

        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (0);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);

        if (veryVerbose) printf( "\tCheck for pointer expression.\n" );

        globalReset(); BSLS_REVIEW_SAFE(p); ASSERT(0 == globalReviewFiredFlag);
        globalReset(); BSLS_REVIEW     (p); ASSERT(0 == globalReviewFiredFlag);

        globalReset();
        level = bsls::Review::k_LEVEL_OPT;
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (p);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(text, globalText,    0 == std::strcmp(text, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        if (veryVerbose) printf( "\tCheck for expression with spaces.\n" );

        globalReset(); BSLS_REVIEW_SAFE(false == true);
        ASSERT(0 == globalReviewFiredFlag);

        globalReset(); BSLS_REVIEW     (false == true);
        ASSERT(0 == globalReviewFiredFlag);

        globalReset();
        level = bsls::Review::k_LEVEL_OPT;
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (false == true);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(expr, globalText, 0 == std::strcmp(expr, globalText));
        LOOP2_ASSERT(file, globalFile, 0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);
#endif

        //_____________________________________________________________________
        //                  *** None Instantiate ***
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if defined(BSLS_REVIEW_LEVEL_NONE)                             \

        if (verbose) printf( "\nEnabled: (* Nothing *)\n" );

        if (veryVerbose) printf( "\tCheck for integer expression.\n" );

        globalReset(); BSLS_REVIEW_SAFE(0); ASSERT(0 == globalReviewFiredFlag);
        globalReset(); BSLS_REVIEW     (0); ASSERT(0 == globalReviewFiredFlag);
        globalReset(); BSLS_REVIEW_OPT (0); ASSERT(0 == globalReviewFiredFlag);

        if (veryVerbose) printf( "\tCheck for integer expression.\n" );

        globalReset(); BSLS_REVIEW_SAFE(p); ASSERT(0 == globalReviewFiredFlag);

        globalReset(); BSLS_REVIEW     (p); ASSERT(0 == globalReviewFiredFlag);

        globalReset(); BSLS_REVIEW_OPT (p); ASSERT(0 == globalReviewFiredFlag);

        if (veryVerbose) printf( "\tCheck for expression with spaces.\n" );

        globalReset(); BSLS_REVIEW_SAFE(false == true);
        ASSERT(0 == globalReviewFiredFlag);

        globalReset(); BSLS_REVIEW     (false == true);
        ASSERT(0 == globalReviewFiredFlag);

        globalReset(); BSLS_REVIEW_OPT (false == true);
        ASSERT(0 == globalReviewFiredFlag);
#endif

        //_____________________________________________________________________
        //            *** BSLS_REVIEW_INVOKE (always instantiate) ***
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if (verbose) printf( "\nChecking BSLS_INVOKE\n" );

        globalReset();
        count = 1;
        level = bsls::Review::k_LEVEL_INVOKE;
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_INVOKE(text);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(text, globalText,    0 == std::strcmp(text, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);


      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 Want to observe the basic operation of this component in "default
        //:   mode" (i.e., with no build flags specified).
        //
        // Plan:
        //: 1 Call 'setReviewHandler' to install the 'testDriverHandler'
        //:   "review" function in order to observe that the installed function
        //:   was called using the 'invokeHandler' method -- and, contingently,
        //:   the 'BSLS_REVIEW_OPT(X)' macro -- with various arguments.
        //
        // Testing:
        //   BREATHING TEST
        //   CONCERN: By default, the 'bsls::Review::falLog' is used.
        //   ReviewViolation::ReviewViolation(...);
        //   const char *ReviewViolation::comment();
        //   int ReviewViolation::count();
        //   const char *ReviewViolation::fileName();
        //   int ReviewViolation::lineNumber();
        //   const char *ReviewViolation::reviewLevel();
        //   static void setFailureHandler(bsls::Review::Handler function);
        //   static bsls::Review::Handler failureHandler();
        //   static void invokeHandler(const char *t, const char *f, int);
        //   static int updateCount(Count *count);
        //   static void lockReviewAdministration();
        //   CONCERN: By default, the 'bsls_review::failAbort' is used
        // --------------------------------------------------------------------

        if (verbose) printf( "\nBREATHING TEST"
                             "\n==============\n" );

        if (verbose) printf( "\nVerify that the correct review callback is "
                             "installed by default.\n" );


        ASSERT(bsls::Review::failLog == bsls::Review::failureHandler());

        if (verbose) printf( "\nVerify that we can install a new review "
                             "callback.\n" );

        bsls::Review::setFailureHandler(&testDriverHandler);
        ASSERT(::testDriverHandler == bsls::Review::failureHandler());

        if (verbose) printf( "\nVerify that 'invokeHandler' properly "
                             "transmits its arguments.\n" );

        globalReset();
        ASSERT(false        == globalReviewFiredFlag);


        if (verbose) printf( "\nVerify that 'updateCount' properly"
                             "increments a count.\n" );
        {
            bsls::Review::Count count = {17};

            bsls::Review::updateCount(&count);
            ASSERT( 18 == bsls::AtomicOperations::getInt(&count) );
        }


        if (verbose) printf( "\nVerify that 'updateCount' rolls over"
                             "properly.\n" );
        {
            bsls::Review::Count count = {(1 << 30) - 1 };

            bsls::Review::updateCount(&count);
            ASSERT( ( (1<<30) - (1<<29) ) ==
                                      bsls::AtomicOperations::getInt(&count) );
        }

        if (verbose) printf( "\nVerify that a ReviewViolation can be "
                             "constructed and has the right values." );

        bsls::ReviewViolation violation("ExPrEsSiOn", "FiLe", -12345678,
                                        "TEST", 17);

        if (verbose) printf( "\nVerify ReviewViolation accessors." );

        ASSERT(        0 == std::strcmp("ExPrEsSiOn", violation.comment()));
        ASSERT(        0 == std::strcmp("FiLe", violation.fileName()));
        ASSERT(-12345678 == violation.lineNumber());
        ASSERT(        0 == std::strcmp("TEST", violation.reviewLevel()));
        ASSERT(       17 == violation.count());

#ifndef BDE_BUILD_TARGET_EXC
        globalReturnOnTestReview = true;
#endif

        REVIEW_TEST_BEGIN
        bsls::Review::invokeHandler(violation);
        REVIEW_TEST_END

        ASSERT(     true == globalReviewFiredFlag);
        ASSERT(        0 == std::strcmp("ExPrEsSiOn", globalText));
        ASSERT(        0 == std::strcmp("FiLe",       globalFile));
        ASSERT(-12345678 == globalLine);
        ASSERT(       17 == globalCount);

#ifndef BDE_BUILD_TARGET_EXC
        globalReturnOnTestReview = false;
#endif

        if (verbose) printf( "\nVerify that 'lockReviewAdministration' blocks "
                             "callback changes.\n" );

        bsls::Review::lockReviewAdministration();

        bsls::Review::setFailureHandler(&bsls::Review::failAbort);
        ASSERT(::testDriverHandler == bsls::Review::failureHandler());

#ifdef BSLS_REVIEW_LEVEL_NONE
        if (verbose) printf( "\n'BSLS_REVIEW_LEVEL_NONE' is defined; exit "
                             "breathing test.\n" );
        break;
#endif
        if (verbose) printf( "\nVerify that 'BSLS_REVIEW_OPT' doesn't fire "
                             "for '!0' expressions.\n" );

        if (veryVerbose) printf( "\tInteger-valued expression\n" );
        {
            globalReset();
            ASSERT(false == globalReviewFiredFlag);

            BSLS_REVIEW_OPT(true == true);
            ASSERT(false == globalReviewFiredFlag);
        }

        if (veryVerbose) printf( "\tPointer-valued expression\n" );
        {
            globalReset();

            ASSERT(false == globalReviewFiredFlag);

            BSLS_REVIEW_OPT((void *)(1));
            ASSERT(false == globalReviewFiredFlag);
        }

        if (verbose) printf( "\nVerify that 'BSLS_REVIEW_OPT' does fire for "
                             "'0' expressions.\n" );

#ifndef BDE_BUILD_TARGET_EXC
            globalReturnOnTestReview = true;
#endif

        if (veryVerbose) printf( "\tInteger-valued expression\n" );
        {
            globalReset();
            ASSERT(false == globalReviewFiredFlag);

            const char *const text  = "true == false";
            const char *const file  = __FILE__;
            int               line  = -1;
            const char *const level = bsls::Review::k_LEVEL_OPT;
            int               count = 1;

            line = L_ + 2;
            REVIEW_TEST_BEGIN
            BSLS_REVIEW_OPT(true == false);
            REVIEW_TEST_END

            ASSERT(true == globalReviewFiredFlag);
            LOOP2_ASSERT(text, globalText, 0 == std::strcmp(text, globalText));
            LOOP2_ASSERT(file, globalFile, 0 == std::strcmp(file, globalFile));
            LOOP2_ASSERT(level, globalLevel,
                         0 == std::strcmp(level, globalLevel));
            LOOP2_ASSERT(line, globalLine, line == globalLine);
            LOOP2_ASSERT(count, globalCount, count == globalCount);
        }

        if (veryVerbose) printf( "\tPointer-valued expression\n" );
        {
            globalReset();
            ASSERT(false == globalReviewFiredFlag);

            const char *const text  = "(void *)(0)";
            const char *const file  = __FILE__;
            int               line  = -1;
            const char *const level = bsls::Review::k_LEVEL_OPT;
            int               count = 1;

            line = L_ + 2;
            REVIEW_TEST_BEGIN
            BSLS_REVIEW_OPT((void *)(0));
            REVIEW_TEST_END

            ASSERT(true == globalReviewFiredFlag);
            LOOP2_ASSERT(text, globalText, 0 == std::strcmp(text, globalText));
            LOOP2_ASSERT(file, globalFile, 0 == std::strcmp(file, globalFile));
            LOOP2_ASSERT(line, globalLine, line == globalLine);
            LOOP2_ASSERT(level, globalLevel,
                         0 == std::strcmp(level, globalLevel));
            LOOP2_ASSERT(count, globalCount, count == globalCount);
        }

        if (veryVerbose) printf( "\tMultiple review failures\n" );
        for (int c = 1; c <= 10; ++c)
        {
            globalReset();
            ASSERT(false == globalReviewFiredFlag);

            const char *const text  = "true == false";
            const char *const file  = __FILE__;
            int               line  = -1;
            const char *const level = bsls::Review::k_LEVEL_OPT;
            int               count = c;

            line = L_ + 2;
            REVIEW_TEST_BEGIN
            BSLS_REVIEW_OPT(true == false);
            REVIEW_TEST_END

            ASSERT(true == globalReviewFiredFlag);
            LOOP2_ASSERT(text, globalText, 0 == std::strcmp(text, globalText));
            LOOP2_ASSERT(file, globalFile, 0 == std::strcmp(file, globalFile));
            LOOP2_ASSERT(line, globalLine, line == globalLine);
            LOOP2_ASSERT(level, globalLevel,
                         0 == std::strcmp(level, globalLevel));
            LOOP2_ASSERT(count, globalCount, count == globalCount);
        }

#ifndef BDE_BUILD_TARGET_EXC
            globalReturnOnTestReview = false;
#endif

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // CALL FAIL ABORT HANDLER
        //
        // Concerns:
        //: 1 That it does abort the program.
        //:
        //: 2 That it prints a message to 'stderr'.
        //
        // Plan:
        //: 1 Call 'bsls::Review::failAbort' after blocking the signal.
        //
        // Testing:
        //   CONCERN: 'bsls::Review::failAbort' aborts
        //   CONCERN: 'bsls::Review::failAbort' prints to 'stderr' not 'stdout'
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCALL FAIL ABORT HANDLER"
                             "\n=======================\n" );

#ifdef BSLS_PLATFORM_OS_UNIX
        sigset_t newset;
        sigaddset(&newset, SIGABRT);

    #if defined(BDE_BUILD_TARGET_MT)
        pthread_sigmask(SIG_BLOCK, &newset, 0);
    #else
        sigprocmask(SIG_BLOCK, &newset, 0);
    #endif

#endif
        fprintf( stderr,  "\nTHE FOLLOWING SHOULD PRINT ON STDERR:\n"
                "Review failed: 0 != 0, file myfile.cpp, line 123\n" );

        bsls::ReviewViolation violation("0 != 0", "myfile.cpp", 123, "TST", 1);
        bsls::Review::failAbort(violation);

        ASSERT(0 && "Should not be reached");
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // CALL FAIL THROW HANDLER
        //
        // Concerns:
        //: 1 That it does *not* throw for an exception build when there is an
        //:   exception pending.
        //:
        //: 2 That it behaves as failAbort for non-exception builds.
        //
        // Plan:
        //: 1 Call bsls::Review::failThrow from within the destructor of a test
        //:   object on the stack after a throw.
        //:
        //: 1 Call 'bsls::Review::failAbort' after blocking the signal.
        //
        // Testing:
        //   static void failAbort(const ReviewViolation &violation);
        //   CONCERN: 'bsls::Review::failThrow' aborts in non-exception build
        //   CONCERN: 'bsls::Review::failThrow' prints to 'stderr' for NON-EXC
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCALL FAIL THROW HANDLER"
                             "\n=======================\n" );

#if BDE_BUILD_TARGET_EXC

        printf( "\nEXCEPTION BUILD\n" );

        fprintf( stderr, "\nTHE FOLLOWING SHOULD PRINT ON STDERR:\n"
                "BSLS_REVIEW: An uncaught exception is pending;"
                " cannot throw 'bsls_reviewtestexception'.\n" );
        fprintf( stderr, "review failed: 'failThrow' handler called "
                 "from ~BadBoy, file f.c, line 9\n" );

        try {
            BadBoy bad;       // calls 'bsls::Review::failThrow' on destruction

            if (veryVerbose) printf( "About to throw \"stuff\"\n" );

            throw "stuff";
        }
        catch (...) {
            ASSERT("We should not have caught this exception." && 0);
        }

        ASSERT("We should not have made it to here either." && 0);
#else
        printf( "\nNON-EXCEPTION BUILD\n" );

  #ifdef BSLS_PLATFORM_OS_UNIX
        sigset_t newset;
        sigaddset(&newset, SIGABRT);

    #if defined(BDE_BUILD_TARGET_MT)
        pthread_sigmask(SIG_BLOCK, &newset, 0);
    #else
        sigprocmask(SIG_BLOCK, &newset, 0);
    #endif

  #endif
        fprintf( stderr,  "THE FOLLOWING SHOULD PRINT ON STDERR:\n"
                "Review failed: 0 != 0, file myfile.cpp, line 123\n" );

        bsls::ReviewViolation violation("0 != 0", "myfile.cpp", 123, "TST", 1);
        bsls::Review::failAbort(violation);

        ASSERT(0 && "Should not be reached");
#endif
      } break;
      case -3: {
        // --------------------------------------------------------------------
        // CALL FAIL SLEEP HANDLER
        //
        // Concerns:
        //: 1 That it does sleep forever.
        //:
        //: 2 That it prints a message to 'stderr'.
        //
        // Plan:
        //: 1 Call 'bsls::Review::failSleep'.  Then observe that a diagnostic
        //:   is printed to 'stderr' and the program hangs.
        //
        // Testing:
        //   static void failSleep(const ReviewViolation &violation);
        //   CONCERN: 'bsls::Review::failSleep' sleeps forever
        //   CONCERN: 'bsls::Review::failSleep' prints to 'stderr' not 'stdout'
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCALL FAIL SLEEP HANDLER"
                             "\n=======================\n" );

        fprintf( stderr, "THE FOLLOWING SHOULD PRINT ON STDERR (BEFORE "
                 "HANGING):\n"
                 "Review failed: 0 != 0, file myfile.cpp, line 123\n" );

        bsls::ReviewViolation violation("0 != 0", "myfile.cpp", 123, "TST", 1);
        bsls::Review::failSleep(violation);

        ASSERT(0 && "Should not be reached");
      } break;
      case -4: {
        // --------------------------------------------------------------------
        // DEFAULT HANDLER LOG: LIMITS
        //
        // Concerns:
        //: 1 Log messages should stabilize at a period of 2^29.
        //
        // Plan:
        //: 1 Invoke a review with the default ('bsls::Review::failLog')
        //:   handler repeatedly, checking that skipped count of 2^29 repeats
        //:
        //: 2 This is the same as test case 7 with MUCH higher total count.
        //
        // Testing:
        //   CONCERN: default handler log: limits
        // --------------------------------------------------------------------

        if (verbose) printf( "\nDEFAULT HANDLER LOG: LIMITS"
                             "\n===========================\n" );

        bsls::ReviewFailureHandlerGuard guard(bsls::Review::failLog);
        bsls::Log::setLogMessageHandler(
            HandlerLoggingTest::recordingLogMessageHandler);

        int prevSkipped = 0;
        int skipped = 0;
        for (long long i = 0; i < (1ll << 32); ++i) {
            if (veryVeryVerbose) printf( "\nTesting invocation %lld"
                                         " of default handler\n" , i);

            HandlerLoggingTest::clear();
            BloombergLP::bsls::Types::Int64 prev_count =
                HandlerLoggingTest::s_loggerInvocationCount;

            int line = L_ + 1;
            BSLS_REVIEW_INVOKE("OUTPUT TEST");

            if (prev_count == HandlerLoggingTest::s_loggerInvocationCount) {
                if (veryVeryVerbose) {
                    printf( "Skipped logger invocation: " );
                    P_(prev_count);
                    P_(skipped);
                    P(HandlerLoggingTest::s_loggerInvocationCount);
                }

                ++skipped;
                LOOP1_ASSERT( i, !IS_POWER_OF_TWO( i+1 ) );
            }
            else {
                if (veryVerbose) {
                    printf( "Logger invocation: " );
                    P_(prev_count);
                    P_(skipped);
                    P(HandlerLoggingTest::s_loggerInvocationCount);
                }

                // check things
                if (skipped == ((1 << 29) -1)) {
                    // the repeating 2^29 period means future invocations are
                    // not at powers of 2, but the skipped check below should
                    // verify that we still have the right period.
                }
                else {
                    LOOP1_ASSERT( i, IS_POWER_OF_TWO( i+1 ) );
                }

                LogProfile &profile = HandlerLoggingTest::lastProfile();
                LOOP2_ASSERT( line, profile.d_line, line == profile.d_line );

                const char *file = __FILE__;
                LOOP2_ASSERT(file, profile.d_file,
                             0 == std::strcmp(file, profile.d_file));

                if (skipped > 0) {
                    char skipText[32];
                    sprintf(skipText,"skipped: %d", skipped);

                    ASSERT( NULL != std::strstr(profile.d_text, skipText) );

                    LOOP2_ASSERT( prevSkipped, skipped,
                                  skipped == prevSkipped*2 + 1 );
                }
                else {
                    ASSERT( NULL == std::strstr(profile.d_text, "skipped") );
                }

                prevSkipped = skipped;
                skipped = 0;
            }
        }

      } break;
      default: {
          fprintf( stderr, "WARNING: CASE `%d` NOT FOUND.\n" , test);
          testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf( stderr, "Error, non-zero test status = %d.\n", testStatus );
    }

    return testStatus;
}


// ----------------------------------------------------------------------------
// The tests for the correct behavior of the configuration macros must come
// *last* in the file, as the nature of the test is to repeatedly redefine the
// effective build mode.  If these tests came any earlier in the test driver
// file, they would have the effect of changing build mode to that of the last
// configuration tested, for any code that followed.
//
// We want to test the following 3 macros:
//   BSLS_REVIEW_SAFE_IS_ACTIVE
//   BSLS_REVIEW_IS_ACTIVE
//   BSLS_REVIEW_OPT_IS_ACTIVE
//
// Their definition is controlled by a definition (or lack thereof) of the
// following macros:
//   BSLS_REVIEW_LEVEL_REVIEW_SAFE
//   BSLS_REVIEW_LEVEL_REVIEW
//   BSLS_REVIEW_LEVEL_REVIEW_OPT
//   BSLS_REVIEW_LEVEL_NONE
//
// The following macros should also have the same effect as the corresponding
// 'BSLS_REVIEW' macros if the review macro has not been set, so we can be sure
// that the default review level always matches the default assert level.
//   BSLS_ASSERT_LEVEL_ASSERT_SAFE
//   BSLS_ASSERT_LEVEL_ASSERT
//   BSLS_ASSERT_LEVEL_ASSERT_OPT
//   BSLS_ASSERT_LEVEL_NONE
//
// The following macros define the build mode, and provide defaults for the
// macros under tests, which might be overridden by the macros above
//   BDE_BUILD_TARGET_SAFE_2
//   BDE_BUILD_TARGET_SAFE
//   BDE_BUILD_TARGET_DBG
//   BDE_BUILD_TARGET_OPT
//
// Our testing strategy is to undefine all these macros, and any implementation
// detail macros in the 'bsls_review.h' header file, and re-include that
// header.  The supporting component 'bsls_review_macroreset' provides a header
// that will undefine all of the public macros from 'bsls_review.h' and prepare
// us to re-include it after changing the above build and assert level macros.
// Note that '#include'ing a header inside a function definition, as we do
// below, will flag an error for any construct that is not supported inside a
// function definition, such as declaring a template or defining a "local"
// function.  consequently, we must provide a deeper "include-guard" inside the
// component header itself to protect the non-macro parts of this component
// against the repeated inclusion.
//
// For each test iteration that '#include <bsls_review.h>', each of the macros
// listed above should be undefined, along with each of the following that are
// also defined within this header:
//   BSLS_REVIEW_SAFE
//   BSLS_REVIEW
//   BSLS_REVIEW_OPT
//   BSLS_REVIEW_REVIEW_IMP
//   BSLS_REVIEW_DISABLED_IMP
//   BSLS_REVIEW_INVOKE
//
// Note that each test contains a certain amount of "boilerplate" code that
// looks like it might be refactored into a common function or two.  This would
// be a mistake, as the "bodies" that looks similar will actually have quite
// different meanings as the macros expand in different ways according to the
// configuration under test.  For example, extracting some of this code into a
// common function passed the expected test result would see that function
// compiled with the default configuration of the test driver, rather than each
// of the 80 configurations under test.
//
// The table below maps each possible build configuration against the available
// review macros, flagging an 'X' when that combination should be supported.
// If a review macro should be enabled, this should be detectable by the
// definition of the corresponding 'BSLS_REVIEW_*_IS_ACTIVE' macro.  Likewise,
// those macros should not be defined unless the configuration is marked.
//
// The configurations that define a BSLS_REVIEW_LEVEL_* macro are also
// duplicated with the corresponding BSLS_ASSERT_LEVEL_* set, expecting the
// same output.
//
//  Expected test results
//    BDE_BUILD mode      review            REVIEW macros
//  SAFE2 SAFE DBG OPT      level              OPT     SAFE
//  ----- ---- --- ---    --------             --- --- ----
//                                              X   X
//                  X                           X
//             X                                X   X
//             X    X                           X
//         X                                    X   X   X
//         X        X                           X   X   X
//         X   X                                X   X   X
//         X   X    X                           X   X   X
//    X                                         X   X   X
//    X             X                           X   X   X
//    X        X                                X   X   X
//    X        X    X                           X   X   X
//    X    X                                    X   X   X
//    X    X        X                           X   X   X
//    X    X   X                                X   X   X
//    X    X   X    X                           X   X   X
//                       LEVEL_NONE
//                  X    LEVEL_NONE
//             X         LEVEL_NONE
//             X    X    LEVEL_NONE
//         X             LEVEL_NONE
//         X        X    LEVEL_NONE
//         X   X         LEVEL_NONE
//         X   X    X    LEVEL_NONE
//    X                  LEVEL_NONE
//    X             X    LEVEL_NONE
//    X        X         LEVEL_NONE
//    X        X    X    LEVEL_NONE
//    X    X             LEVEL_NONE
//    X    X        X    LEVEL_NONE
//    X    X   X         LEVEL_NONE
//    X    X   X    X    LEVEL_NONE
//                       LEVEL_REVIEW_OPT       X
//                  X    LEVEL_REVIEW_OPT       X
//             X         LEVEL_REVIEW_OPT       X
//             X    X    LEVEL_REVIEW_OPT       X
//         X             LEVEL_REVIEW_OPT       X
//         X        X    LEVEL_REVIEW_OPT       X
//         X   X         LEVEL_REVIEW_OPT       X
//         X   X    X    LEVEL_REVIEW_OPT       X
//    X                  LEVEL_REVIEW_OPT       X
//    X             X    LEVEL_REVIEW_OPT       X
//    X        X         LEVEL_REVIEW_OPT       X
//    X        X    X    LEVEL_REVIEW_OPT       X
//    X    X             LEVEL_REVIEW_OPT       X
//    X    X        X    LEVEL_REVIEW_OPT       X
//    X    X   X         LEVEL_REVIEW_OPT       X
//    X    X   X    X    LEVEL_REVIEW_OPT       X
//                       LEVEL_REVIEW           X   X
//                  X    LEVEL_REVIEW           X   X
//             X         LEVEL_REVIEW           X   X
//             X    X    LEVEL_REVIEW           X   X
//         X             LEVEL_REVIEW           X   X
//         X        X    LEVEL_REVIEW           X   X
//         X   X         LEVEL_REVIEW           X   X
//         X   X    X    LEVEL_REVIEW           X   X
//    X                  LEVEL_REVIEW           X   X
//    X             X    LEVEL_REVIEW           X   X
//    X        X         LEVEL_REVIEW           X   X
//    X        X    X    LEVEL_REVIEW           X   X
//    X    X             LEVEL_REVIEW           X   X
//    X    X        X    LEVEL_REVIEW           X   X
//    X    X   X         LEVEL_REVIEW           X   X
//    X    X   X    X    LEVEL_REVIEW           X   X
//                       LEVEL_REVIEW_SAFE      X   X   X
//                  X    LEVEL_REVIEW_SAFE      X   X   X
//             X         LEVEL_REVIEW_SAFE      X   X   X
//             X    X    LEVEL_REVIEW_SAFE      X   X   X
//         X             LEVEL_REVIEW_SAFE      X   X   X
//         X        X    LEVEL_REVIEW_SAFE      X   X   X
//         X   X         LEVEL_REVIEW_SAFE      X   X   X
//         X   X    X    LEVEL_REVIEW_SAFE      X   X   X
//    X                  LEVEL_REVIEW_SAFE      X   X   X
//    X             X    LEVEL_REVIEW_SAFE      X   X   X
//    X        X         LEVEL_REVIEW_SAFE      X   X   X
//    X        X    X    LEVEL_REVIEW_SAFE      X   X   X
//    X    X             LEVEL_REVIEW_SAFE      X   X   X
//    X    X        X    LEVEL_REVIEW_SAFE      X   X   X
//    X    X   X         LEVEL_REVIEW_SAFE      X   X   X
//    X    X   X    X    LEVEL_REVIEW_SAFE      X   X   X
//
// Finally, we will test that the review level when explicitly set takes
// precedence over explicitly setting the assertion level.
//
//  assertion         assertion          REVIEW macros
//    level             level           OPT     SAFE
//  ----------------- ----------------- --- --- ----
//  LEVEL_NONE        LEVEL_NONE
//  LEVEL_ASSERT_OPT  LEVEL_NONE
//  LEVEL_ASSERT      LEVEL_NONE
//  LEVEL_ASSERT_SAFE LEVEL_NONE
//  LEVEL_NONE        LEVEL_REVIEW_OPT   X
//  LEVEL_ASSERT_OPT  LEVEL_REVIEW_OPT   X
//  LEVEL_ASSERT      LEVEL_REVIEW_OPT   X
//  LEVEL_ASSERT_SAFE LEVEL_REVIEW_OPT   X
//  LEVEL_NONE        LEVEL_REVIEW       X   X
//  LEVEL_ASSERT_OPT  LEVEL_REVIEW       X   X
//  LEVEL_ASSERT      LEVEL_REVIEW       X   X
//  LEVEL_ASSERT_SAFE LEVEL_REVIEW       X   X
//  LEVEL_NONE        LEVEL_REVIEW_SAFE  X   X   X
//  LEVEL_ASSERT_OPT  LEVEL_REVIEW_SAFE  X   X   X
//  LEVEL_ASSERT      LEVEL_REVIEW_SAFE  X   X   X
//  LEVEL_ASSERT_SAFE LEVEL_REVIEW_SAFE  X   X   X

namespace
{

#if defined(BDE_BUILD_TARGET_EXC)
struct ReviewFailed {
    // This struct contains a static function suitable for registration as a
    // review handler, and provides a distinct "empty" type that may be thrown
    // from the handler and caught within the test cases below, in order to
    // confirm if the appropriate 'BSLS_REVIEW_*' macros are enabled or not.
    static void failMacroTest(const bsls::ReviewViolation &) {
        throw ReviewFailed();
    }
};
#else
    // Without exception support, we cannot fail a review-test by throwing an
    // exception.  The most practical solution is to simply not compile those
    // tests, so we do not supply an 'ReviewFailed' alternative, to be sure to
    // catch any compile-time use of this structure in exception-free builds.
#endif
}  // close unnamed namespace

void TestConfigurationMacros()
{

    if (globalVerbose) printf( "\nCONFIGURATION MACROS"
                               "\n====================\n" );

#if !defined(BDE_BUILD_TARGET_EXC)
    if (globalVerbose)
        printf( "\nThis case is not run as it "
                "relies on exception support.\n" );
#else
    if (globalVerbose) printf( "\nWe need to write a running commentary\n" );

    bsls::Review::setFailureHandler(&ReviewFailed::failMacroTest);

//========================== (NO BUILD FLAGS SET) ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

    // (THIS LINE INTENTIONALLY LEFT BLANK)

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }


//=================================== OPT ===================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }

//=================================== DBG ===================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//================================= DBG OPT =================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//================================== SAFE ===================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//================================ SAFE OPT =================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//================================ SAFE DBG =================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//============================== SAFE DBG OPT ===============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//================================= SAFE_2 ==================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//=============================== SAFE_2 OPT ================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BDE_BUILD_TARGET_SAFE_2

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//=============================== SAFE_2 DBG ================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_SAFE_2

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//============================= SAFE_2 DBG OPT ==============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//=============================== SAFE_2 SAFE ===============================//


// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//============================= SAFE_2 SAFE OPT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//============================= SAFE_2 SAFE DBG =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//=========================== SAFE_2 SAFE DBG OPT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//- - - - - - - - - - - - - - - - LEVEL_NONE - - - - - - - - - - - - - - - - //

//=============================== LEVEL_NONE ================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//============================= OPT LEVEL_NONE ==============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//============================= DBG LEVEL_NONE ==============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//=========================== DBG OPT LEVEL_NONE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//============================= SAFE LEVEL_NONE =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//=========================== SAFE OPT LEVEL_NONE ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//=========================== SAFE DBG LEVEL_NONE ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//========================= SAFE DBG OPT LEVEL_NONE =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//============================ SAFE_2 LEVEL_NONE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//========================== SAFE_2 OPT LEVEL_NONE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//========================== SAFE_2 DBG LEVEL_NONE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//======================== SAFE_2 DBG OPT LEVEL_NONE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//========================= SAFE_2 SAFE LEVEL_NONE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//======================= SAFE_2 SAFE OPT LEVEL_NONE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//======================= SAFE_2 SAFE DBG LEVEL_NONE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//===================== SAFE_2 SAFE DBG OPT LEVEL_NONE ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//- - - - - - - - - - - - - -  LEVEL_REVIEW_OPT - - - - - - - - - - - - - - -//

//============================ LEVEL_REVIEW_OPT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================== OPT LEVEL_REVIEW_OPT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================== DBG LEVEL_REVIEW_OPT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================== DBG OPT LEVEL_REVIEW_OPT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================== SAFE LEVEL_REVIEW_OPT ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================== SAFE OPT LEVEL_REVIEW_OPT ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================== SAFE DBG LEVEL_REVIEW_OPT ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//====================== SAFE DBG OPT LEVEL_REVIEW_OPT ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================= SAFE_2 LEVEL_REVIEW_OPT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================= SAFE_2 OPT LEVEL_REVIEW_OPT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================= SAFE_2 DBG LEVEL_REVIEW_OPT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//===================== SAFE_2 DBG OPT LEVEL_REVIEW_OPT =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//====================== SAFE_2 SAFE LEVEL_REVIEW_OPT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//==================== SAFE_2 SAFE OPT LEVEL_REVIEW_OPT =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//==================== SAFE_2 SAFE DBG LEVEL_REVIEW_OPT =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//================== SAFE_2 SAFE DBG OPT LEVEL_REVIEW_OPT ===================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//- - - - - - - - - - - - - - -  LEVEL_REVIEW - - - - - - - - - - - - - - - -//

//============================== LEVEL_REVIEW ===============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//============================ OPT LEVEL_REVIEW =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//============================ DBG LEVEL_REVIEW =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================== DBG OPT LEVEL_REVIEW ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//============================ SAFE LEVEL_REVIEW ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================== SAFE OPT LEVEL_REVIEW ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================== SAFE DBG LEVEL_REVIEW ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================== SAFE DBG OPT LEVEL_REVIEW ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//=========================== SAFE_2 LEVEL_REVIEW ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================= SAFE_2 OPT LEVEL_REVIEW =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================= SAFE_2 DBG LEVEL_REVIEW =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================= SAFE_2 DBG OPT LEVEL_REVIEW =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================== SAFE_2 SAFE LEVEL_REVIEW =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//====================== SAFE_2 SAFE OPT LEVEL_REVIEW =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//====================== SAFE_2 SAFE DBG LEVEL_REVIEW =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//==================== SAFE_2 SAFE DBG OPT LEVEL_REVIEW =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//- - - - - - - - - - - - - -  LEVEL_REVIEW_SAFE - - - - - - - - - - - - - - //

//============================ LEVEL_REVIEW_SAFE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================== OPT LEVEL_REVIEW_SAFE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================== DBG LEVEL_REVIEW_SAFE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================== DBG OPT LEVEL_REVIEW_SAFE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================= SAFE LEVEL_REVIEW_SAFE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================= SAFE OPT LEVEL_REVIEW_SAFE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================= SAFE DBG LEVEL_REVIEW_SAFE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//===================== SAFE DBG OPT LEVEL_REVIEW_SAFE ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================== SAFE_2 LEVEL_REVIEW_SAFE =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//====================== SAFE_2 OPT LEVEL_REVIEW_SAFE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//====================== SAFE_2 DBG LEVEL_REVIEW_SAFE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//==================== SAFE_2 DBG OPT LEVEL_REVIEW_SAFE =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//====================== SAFE_2 SAFE LEVEL_REVIEW_SAFE ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//==================== SAFE_2 SAFE OPT LEVEL_REVIEW_SAFE ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//==================== SAFE_2 SAFE DBG LEVEL_REVIEW_SAFE ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//================== SAFE_2 SAFE DBG OPT LEVEL_REVIEW_SAFE ==================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//- - - - - - - - - - - - - -  ASSERT_LEVEL_NONE - - - - - - - - - - - - - - //

//============================ ASSERT_LEVEL_NONE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//========================== OPT ASSERT_LEVEL_NONE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//========================== DBG ASSERT_LEVEL_NONE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//======================== DBG OPT ASSERT_LEVEL_NONE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//========================= SAFE ASSERT_LEVEL_NONE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//======================= SAFE OPT ASSERT_LEVEL_NONE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//======================= SAFE DBG ASSERT_LEVEL_NONE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//===================== SAFE DBG OPT ASSERT_LEVEL_NONE ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//======================== SAFE_2 ASSERT_LEVEL_NONE =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//====================== SAFE_2 OPT ASSERT_LEVEL_NONE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//====================== SAFE_2 DBG ASSERT_LEVEL_NONE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//==================== SAFE_2 DBG OPT ASSERT_LEVEL_NONE =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//====================== SAFE_2 SAFE ASSERT_LEVEL_NONE ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//==================== SAFE_2 SAFE OPT ASSERT_LEVEL_NONE ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//==================== SAFE_2 SAFE DBG ASSERT_LEVEL_NONE ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//================== SAFE_2 SAFE DBG OPT ASSERT_LEVEL_NONE ==================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }


//- - - - - - - - - - - - - -  LEVEL_ASSERT_OPT - - - - - - - - - - - - - - -//

//============================ LEVEL_ASSERT_OPT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================== OPT LEVEL_ASSERT_OPT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================== DBG LEVEL_ASSERT_OPT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================== DBG OPT LEVEL_ASSERT_OPT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================== SAFE LEVEL_ASSERT_OPT ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================== SAFE OPT LEVEL_ASSERT_OPT ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================== SAFE DBG LEVEL_ASSERT_OPT ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//====================== SAFE DBG OPT LEVEL_ASSERT_OPT ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================= SAFE_2 LEVEL_ASSERT_OPT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================= SAFE_2 OPT LEVEL_ASSERT_OPT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================= SAFE_2 DBG LEVEL_ASSERT_OPT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//===================== SAFE_2 DBG OPT LEVEL_ASSERT_OPT =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//====================== SAFE_2 SAFE LEVEL_ASSERT_OPT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//==================== SAFE_2 SAFE OPT LEVEL_ASSERT_OPT =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//==================== SAFE_2 SAFE DBG LEVEL_ASSERT_OPT =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//================== SAFE_2 SAFE DBG OPT LEVEL_ASSERT_OPT ===================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//- - - - - - - - - - - - - - -  LEVEL_ASSERT - - - - - - - - - - - - - - - -//

//============================== LEVEL_ASSERT ===============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//============================ OPT LEVEL_ASSERT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//============================ DBG LEVEL_ASSERT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================== DBG OPT LEVEL_ASSERT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//============================ SAFE LEVEL_ASSERT ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================== SAFE OPT LEVEL_ASSERT ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================== SAFE DBG LEVEL_ASSERT ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================== SAFE DBG OPT LEVEL_ASSERT ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//=========================== SAFE_2 LEVEL_ASSERT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================= SAFE_2 OPT LEVEL_ASSERT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================= SAFE_2 DBG LEVEL_ASSERT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================= SAFE_2 DBG OPT LEVEL_ASSERT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================== SAFE_2 SAFE LEVEL_ASSERT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//====================== SAFE_2 SAFE OPT LEVEL_ASSERT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//====================== SAFE_2 SAFE DBG LEVEL_ASSERT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//==================== SAFE_2 SAFE DBG OPT LEVEL_ASSERT =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//- - - - - - - - - - - - - -  LEVEL_ASSERT_SAFE - - - - - - - - - - - - - - //

//============================ LEVEL_ASSERT_SAFE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================== OPT LEVEL_ASSERT_SAFE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================== DBG LEVEL_ASSERT_SAFE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================== DBG OPT LEVEL_ASSERT_SAFE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//========================= SAFE LEVEL_ASSERT_SAFE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================= SAFE OPT LEVEL_ASSERT_SAFE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================= SAFE DBG LEVEL_ASSERT_SAFE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//===================== SAFE DBG OPT LEVEL_ASSERT_SAFE ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//======================== SAFE_2 LEVEL_ASSERT_SAFE =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//====================== SAFE_2 OPT LEVEL_ASSERT_SAFE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//====================== SAFE_2 DBG LEVEL_ASSERT_SAFE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//==================== SAFE_2 DBG OPT LEVEL_ASSERT_SAFE =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//====================== SAFE_2 SAFE LEVEL_ASSERT_SAFE ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//==================== SAFE_2 SAFE OPT LEVEL_ASSERT_SAFE ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//==================== SAFE_2 SAFE DBG LEVEL_ASSERT_SAFE ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }


//================== SAFE_2 SAFE DBG OPT LEVEL_ASSERT_SAFE ==================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_review header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false"); ASSERT(false); }
    catch(ReviewFailed)              { ASSERT(true); }

//- - - - - - - - - - ASSERT_LEVEL and REVIEW_LEVEL_NONE - - - - - - - - - - //

//=================== ASSERT_LEVEL_NONE REVIEW_LEVEL_NONE ===================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//================ ASSERT_LEVEL_ASSERT_OPT REVIEW_LEVEL_NONE ================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//================== ASSERT_LEVEL_ASSERT REVIEW_LEVEL_NONE ==================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//=============== ASSERT_LEVEL_ASSERT_SAFE REVIEW_LEVEL_NONE ================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//- - - - - - - -  ASSERT_LEVEL and REVIEW_LEVEL_REVIEW_OPT - - - - - - - - -//

//================ ASSERT_LEVEL_NONE REVIEW_LEVEL_REVIEW_OPT ================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_assert header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//============= ASSERT_LEVEL_ASSERT_OPT REVIEW_LEVEL_REVIEW_OPT =============//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_assert header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//=============== ASSERT_LEVEL_ASSERT REVIEW_LEVEL_REVIEW_OPT ===============//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_assert header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//============ ASSERT_LEVEL_ASSERT_SAFE REVIEW_LEVEL_REVIEW_OPT =============//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the bsls_assert header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//- - - - - - - - -  ASSERT_LEVEL and REVIEW_LEVEL_REVIEW - - - - - - - - - -//

//================== ASSERT_LEVEL_NONE REVIEW_LEVEL_REVIEW ==================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_assert header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//=============== ASSERT_LEVEL_ASSERT_OPT REVIEW_LEVEL_REVIEW ===============//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_assert header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//================= ASSERT_LEVEL_ASSERT REVIEW_LEVEL_REVIEW =================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_assert header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//============== ASSERT_LEVEL_ASSERT_SAFE REVIEW_LEVEL_REVIEW ===============//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the bsls_assert header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//- - - - - - - -  ASSERT_LEVEL and REVIEW_LEVEL_REVIEW_SAFE - - - - - - - - //

//=============== ASSERT_LEVEL_NONE REVIEW_LEVEL_REVIEW_SAFE ================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_assert header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//============ ASSERT_LEVEL_ASSERT_OPT REVIEW_LEVEL_REVIEW_SAFE =============//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_assert header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//============== ASSERT_LEVEL_ASSERT REVIEW_LEVEL_REVIEW_SAFE ===============//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_assert header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//============ ASSERT_LEVEL_ASSERT_SAFE REVIEW_LEVEL_REVIEW_SAFE ============//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the bsls_assert header.

#include <bsls_review.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false); ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -//
#endif  // defined BDE_BUILD_TARGET_EXC
}
// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
