// bsls_review.t.cpp                                                  -*-C++-*-
#include <bsls_review.h>

#include <bsls_asserttestexception.h>
#include <bsls_bsltestutil.h>
#include <bsls_log.h>
#include <bsls_logseverity.h>
#include <bsls_platform.h>
#include <bsls_types.h>

// Include `cassert` to make sure no macros conflict between `bsls_review.h`
// and `cassert`.  This test driver does *not* invoke `assert(expression)`.
#include <cassert>

#include <cstdio>    // `fprintf`
#include <cstdlib>   // `atoi`
#include <cstring>   // `strcmp`
#include <exception> // `exception`
#include <iostream>  // `ostream`, `cerr` for usage examples

#ifdef BSLS_PLATFORM_OS_UNIX
#include <signal.h>
#endif

#ifdef BSLS_PLATFORM_PRAGMA_GCC_DIAGNOSTIC_GCC
#pragma GCC diagnostic ignored "-Wcatch-value"
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
// [ 1] static void setViolationHandler(ViolationHandler function);
// [ 1] static void lockReviewAdministration();
// [ 1] static bsls::Review::ViolationHandler violationHandler();
// [ 1] static void invokeHandler(const bsls::ReviewViolation&);
// [ 1] static int updateCount(Count *count);
// [ 4] static void failByLog(const ReviewViolation& violation);
// [ 4] static void failByAbort(const ReviewViolation& violation);
// [-2] static void failByAbort(const ReviewViolation& violation);
// [ 4] static void failBySleep(const ReviewViolation& violation);
// [-3] static void failBySleep(const ReviewViolation& violation);
// [ 4] static void failByThrow(const ReviewViolation& violation);
// [ 5] class bsls::ReviewFailureHandlerGuard
// [ 5] ReviewFailureHandlerGuard::ReviewFailureHandlerGuard(Handler)
// [ 5] ReviewFailureHandlerGuard::~ReviewFailureHandlerGuard()
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] CONCERN: default handler log: content
// [ 8] CONCERN: default handler log: backoff
// [-4] CONCERN: default handler log: limits
//
// [12] ASSERT USAGE EXAMPLE: Using Review Macros
// [13] ASSERT USAGE EXAMPLE: Invoking an review handler directly
// [14] ASSERT USAGE EXAMPLE: Using Administration Functions
// [14] ASSERT USAGE EXAMPLE: Installing Prefabricated Review-Handlers
// [15] ASSERT USAGE EXAMPLE: Creating Your Own Review-Handler
// [16] ASSERT USAGE EXAMPLE: Using Scoped Guard
// [17] ASSERT USAGE EXAMPLE: Using `BDE_BUILD_TARGET_SAFE_2`
// [18] ASSERT USAGE EXAMPLE: Conditional Compilation
// [19] ASSERT USAGE EXAMPLE: Conditional Compilation of Support Functions
// [20] ASSERT USAGE EXAMPLE: Conditional Compilation of Support Code
// [21] USAGE EXAMPLE: Adding `BSLS_ASSERT` to an existing function
//
// [ 1] CONCERN: By default, the `bsls_review::failByAbort` is used
// [ 2] CONCERN: REVIEW macros are instantiated properly for build targets
// [ 2] CONCERN: all combinations of BDE_BUILD_TARGETs are allowed
// [ 2] CONCERN: any one review mode overrides all BDE_BUILD_TARGETs
// [ 3] CONCERN: ubiquitously detect multiply-defined review-mode flags
// [ 5] CONCERN: that locking does not stop the handlerGuard from working
// [-1] CONCERN: `bsls::Review::failByAbort` aborts
// [-1] CONCERN: `bsls::Review::failByAbort` prints to `stderr`
// [-2] CONCERN: `bsls::Review::failByThrow` aborts in non-exception build
// [-2] CONCERN: `bsls::Review::failByThrow` prints to `stderr` w/o EXC
// [-3] CONCERN: `bsls::Review::failBySleep` sleeps forever
// [-3] CONCERN: `bsls::Review::failBySleep` prints to `stderr`
// [ 9] CONCERN: `BSLS_ASSERTIMPUTIL_FILE` interaction
// [10] CONCERN: `constexpr` interaction
// [11] CONCERN: Dynamic assertion messages

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

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

static bool verbose         = false;
static bool veryVerbose     = false;
static bool veryVeryVerbose = false;

static bool        globalReviewFiredFlag = false;
static const char *globalText = "";
static const char *globalFile = "";
static const char *globalLevel = "";
static int         globalLine = -1;
static int         globalCount = 0;

// When using language contracts the contract level is not available to the
// violation handler, so it will not be set.
#ifdef BSLS_ASSERT_USE_CONTRACTS
static bool checkLevels = false;
#else
static bool checkLevels = true;
#endif

// BDE_VERIFY pragma: -FE01 // at this level in the package hierarchy in a test
                            // driver we want to avoid std::exception

#define IS_POWER_OF_TWO(X) (0 == ((X) & ((X) - 1)))

#ifndef BDE_BUILD_TARGET_EXC
static bool globalReturnOnTestReview = false;
    // This flag is very dangerous, as it will cause the test-driver review
    // handler to simple `return` by default, exposing any additional function
    // under test to the subsequent undefined behavior.  In general, exception-
    // free builds should avoid executing such tests, rather than set this
    // flag.  However, there is some subset of this test driver that would
    // benefit from being able to invoke this handler in a test mode to be sure
    // that correct behavior occurs in the presence of the various preprocessor
    // checks for exceptions being disabled.  This flag allows for testing such
    // behavior that does not rely on aborting out of the review handler.
#endif

                         // -------------------------
                         // Language Contract Support
                         // -------------------------
// Because this component is levelized below `bsls_assert` a bespoke violation
// handler is needed for this test driver in order to hook into language
// contracts properly.

#ifdef BSLS_ASSERT_USE_CONTRACTS

void handle_contract_violation(const std::contract_violation& violation)
    // Call `bsls::Review::invokeLanguageContractHandler` with the specified
    // `violation`.  Note that this is the replacable function that GCC will
    // look for when handling a contract violation.
{
    BloombergLP::bsls::Review::invokeLanguageContractHandler(violation);
}

#endif

//=============================================================================
//                  GLOBAL HELPER MACROS FOR TESTING
//-----------------------------------------------------------------------------

#if (defined(BSLS_REVIEW_LEVEL_REVIEW_SAFE) ||                                \
     defined(BSLS_REVIEW_LEVEL_REVIEW)      ||                                \
     defined(BSLS_REVIEW_LEVEL_REVIEW_OPT)  ||                                \
     defined(BSLS_REVIEW_LEVEL_NONE) )
    #define IS_BSLS_REVIEW_MODE_FLAG_DEFINED 1
#else
    #define IS_BSLS_REVIEW_MODE_FLAG_DEFINED 0
#endif

#ifdef BDE_BUILD_TARGET_EXC
#define REVIEW_TEST_BEGIN                                    \
        try {

#define REVIEW_TEST_END                                                       \
        } catch (const std::exception&) {                                     \
            if (verbose) printf( "Exception caught.\n" );                     \
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

    /// Clear s_profiles and set s_loggerInvocationCount to 0
    static void clear();

    /// return the last updated profile.
    static LogProfile &lastProfile();

    /// Do nothing with the specified `violation`.
    static void emptyViolationHandler(const bsls::ReviewViolation& violation);

    /// Register a test failure if the specified `severity` is not error,
    /// and store the specified `file`, `line` and `message` into the
    /// corresponding fields of s_profile[s_loggerInvocationCount].  then
    /// increment s_loggerInvocationCount
    static void recordingLogMessageHandler(bsls::LogSeverity::Enum  severity,
                                           const char              *file,
                                           int                      line,
                                           const char              *message);

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
                                        const bsls::ReviewViolation& violation)
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

    if (veryVeryVerbose) {
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

/// Reset all global values used by test handlers.
static void globalReset()
{
    if (veryVeryVerbose)
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

/// Set the `globalReviewFiredFlag` to `true` and store the specified
/// `comment`, `file` name, `line` number and `count` from the `violation`
/// into `globalText`, globalFile', `globalLevel, `globalLine', and
/// `globalCount` respectively.  Then throw an `std::exception` object
/// provided that `BDE_BUILD_TARGET_EXC` is defined; otherwise, abort the
/// program.
static void testDriverHandler(const bsls::ReviewViolation& violation)
{
    if (veryVeryVerbose) {
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
    if (globalReturnOnTestReview) {
        return;                                                       // RETURN
    }
    std::abort();
#endif
}

//-----------------------------------------------------------------------------

/// Format, in verbose mode, the specified expression `text`, `file` name,
/// and `line` number from the specified `violation` the same way as the
/// `bsls::Review::failByAbort` review-failure handler function might, but
/// on `cout` instead of `cerr`.  Then throw an `std::exception` object
/// provided that `BDE_BUILD_TARGET_EXC` is defined; otherwise, abort the
/// program.
static void testDriverPrint(const bsls::ReviewViolation& violation)
{
    if (veryVeryVerbose) {
        printf( "*** testDriverPrint: " );
        P_(violation.comment());
        P_(violation.fileName());
        P(violation.lineNumber());
    }

    if (veryVerbose) {
        std::fprintf(stdout,
                     "Review failed: %s, file %s, line %d\n",
                     violation.comment(), violation.fileName(),
                     violation.lineNumber() );

        std::fflush(stdout);
    }

#ifdef BDE_BUILD_TARGET_EXC
    throw std::exception();
#else
    if (globalReturnOnTestReview) {
        return;                                                       // RETURN
    }
    std::abort();
#endif
}

//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

/// Bogus `struct` used for testing: calls `bsls::Review::failByThrow` on
/// destruction to ensure that it does not re-throw with an exception
/// pending (see case -2).
struct BadBoy {

    BadBoy() {
        if (veryVerbose) printf( "BadBoy Created!\n" );
    }

    ~BadBoy() BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false) {
        if (veryVerbose) printf( "BadBoy Destroyed!\n" );
        bsls::ReviewViolation violation(
            "`failByThrow` handler called from ~BadBoy",
            "f.c",
            9,
            "BADBOY",
            1);
        bsls::Review::failByThrow(violation);
     }
};

// Declaration of function that must appear after main in order to test the
// configuration macros.

/// Contains tests of component macros in many different configurations.
void TestConfigurationMacros();

/// Return the specified `x`, the behavior is undefined if `x < 0`.
struct ConstexprFunctions {
    BSLS_KEYWORD_CONSTEXPR_CPP17
    static int narrowConstexprFunction(int x)
    {
#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
        BSLS_REVIEW_OPT(x >= 0);
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
        BSLS_REVIEW(x >= 0);
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
        BSLS_REVIEW_SAFE(x >= 0);
#endif

        if (x < 0) {
            BSLS_REVIEW_INVOKE("x < 0");
        }
        return x;
    }
};

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

#define BSLS_ASSERT(X) // Usage examples below refer to the higher level
                       // component `bsls_assert`, which cannot actually be
                       // referenced here, so we include a dummy definition for
                       // testing.

///Usage
///-----
//
namespace usage_example_review_1 {
///Example 1: Adding `BSLS_ASSERT` To An Existing Function
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose you have an existing function, already deployed to production, that
// was not written with defensive programming in mind.  In order to increase
// robustness, you would like to add `BSLS_ASSERT` macros to this function that
// match the contract originally written when this function was initially
// released.
//
// For example, consider the function `myFunc` in the class `FunctionsV1` that
// was implemented like this:
// ```
// my_functions.h
// ...

/// ...
class FunctionsV1 {
  public:
    // ...

    /// Do something with the specified positive integers `x` and `y`.
    static int myFunc(int x, int y);
};

inline int FunctionsV1::myFunc(int x, int y)
{
    (void)x;
    (void)y;
    int output = 0;
    // ... do stuff with `x` and `y`.
    return output;
}
// ```
// Notice that there are no checks on `x` and `y` within `myFunc` and no
// assertions to detect use of `myFunc` outside of its contract.  On the other
// hand, `myFunc` is part of legacy code that has been in use extensively for
// years or decades, so clearly this is not causing a problem (yet).
//
// Upon reviewing this class you realize that `myFunc` produces random results
// for values of `x` or `y` less than 0.  You, however, do not have enough
// information to conclude that no one is calling it with negative values and
// just using the bad results unknowingly.  There are a number of possibilities
// for how the result of this undefined behavior might be going unnoticed.
//  - The invalid value might be discarded by a bounds check later in the
//    process.
//  - The invalid value may only result in a small glitch the users have not
//    noticed or ignored.
//  - The resulting value may actually be valid, but allowing negative input
//    for `x` and `y` may preclude potential future development in ways we do
//    not want to allow.
// All of these are bad, but adding in checks with `BSLS_ASSERT` that would
// replace these bad behaviors by process termination would turn silent errors
// into loud errors (potentially worse).  On the other hand, by not adding
// `BSLS_ASSERT` checks we permit future misuses of this function, which may
// not be innocuous, to potentially reach production systems.  `BSLS_REVIEW`
// here serves as a bridge, from the current state of `myFunc` (entirely
// unchecked) to the ideal state of `myFunc` (where misuse is caught loudly and
// immediately through `BSLS_ASSERT`), following a path that doesn't risk
// turning an un-noticed or irrelevant error into one that will significantly
// hinder ongoing business.
//
// The solution to this is to *initially* reimplement `myFunc` using
// `BSLS_REVIEW` like this:
// ```
// my_functions.h
// ...
#include <bsls_review.h>
// ...

/// ...
class FunctionsV2 {
  public:
    // ...

    /// Do something with the specified `x` and `y`.  The behavior is
    /// undefined unless `x > 0` and `y > 0`.
    static int myFunc(int x, int y);
};

inline int FunctionsV2::myFunc(int x, int y)
{
    (void)x;
    (void)y;
    BSLS_REVIEW(x > 0);
    BSLS_REVIEW(y > 0);
    int output = 0;
    // ... do stuff with `x` and `y`.
    return output;
}
// ```
// Now you can deploy this code to production and then begin reviewing logs.
// The log messages you should look for are those produced by `bsls::Review`s
// default review failure handler and will be similar to:
// ```
// ERROR myfunction.h:17 BSLS_REVIEW failure (myfunction.h:17 level:R-DBG):
//                                'x > 0' Please run "/bb/bin/showfunc.tsk ...
// ```
// `showfunc.tsk` is a Bloomberg application that can be used (along with the
// task binary) to convert the reported stack addresses to a more traditional
// stack trace with a function call stack.
//
// It is important to note that `BSLS_REVIEW` is purely informative, and adding
// a review will not adversely affect behavior, and may in fact alert the
// library author to common client misconceptions about the intended behavior.
//
// For example, let's say actual usage makes it clear that users expect 0 to be
// valid values for the arguments to `myFunc`, and nothing in the
// implementation prevents us from accepting 0 as input and producing the
// answer clients expect.  Instead of changing all the clients, we may instead
// choose to change the function contract (and implemented checks):
// ```
// my_functions.h
// ...
//  #include <bsls_review.h>
// ...

/// ...
class FunctionsV3 {
  public:
    // ...

    /// Do something with the specified `x` and `y`.  The behavior is
    /// undefined unless `x >= 0` and `y >= 0`.
    static int myFunc(int x, int y);
};

inline int FunctionsV3::myFunc(int x, int y)
{
    (void)x;
    (void)y;
    BSLS_REVIEW(x >= 0);
    BSLS_REVIEW(y >= 0);
    int output = 0;
    // ... do stuff with `x` and `y`.
    return output;
}
// ```
// Finally, at some point, the implementation of `myFunc` using `BSLS_REVIEW`
// has been running a suitable amount of time that you are comfortable
// transitioning the use of `bsls_review` to `bsls_assert`.  We now use our
// favorite text editor or script to replace "BSLS_REVIEW" with "BSLS_ASSERT":
// ```
// my_functions.h
// ...
//  #include <bsls_assert.h>
// ...

/// ...
class FunctionsV4 {
  public:
    // ...

    /// Do something with the specified `x` and `y`.  The behavior is
    /// undefined unless `x >= 0` and `y >= 0`.
    static int myFunc(int x, int y);
};

inline int FunctionsV4::myFunc(int x, int y)
{
    (void)x;
    (void)y;
    BSLS_ASSERT(x >= 0);
    BSLS_ASSERT(y >= 0);
    int output = 0;
    // ... do stuff with `x` and `y`.
    return output;
}
// ```
// At this point, any contract violations in the use of `myFunc` in new code
// will be caught immediately (i.e., in appropriate build modes).
}  // close namespace usage_example_review_1

#undef BSLS_ASSERT // This macro is no longer needed outside the usage example.

//=============================================================================
//                         ASSERT USAGE EXAMPLES
//-----------------------------------------------------------------------------
// All usage of `BSLS_ASSERT` should be valid with the corresponding
// `BSLS_REVIEW` macros, so all of the usage from `bsls_assert.t.cpp` and the
// corresponding test cases below are replicated.
//
///Usage - from `bsls_assert.t.cpp`
///--------------------------------
// The following examples illustrate (1) when to use each of the three kinds of
// (BSLS) "REVIEW" macros, (2) when and how to call the `invokeHandler` method
// directly, (3) how to configure, at runtime, the behavior resulting from an
// reviewion failure using "off-the-shelf" handler methods, (4) how to create
// your own custom reviewion-failure handler function, (5) proper use of
// `bsls::ReviewFailureHandlerGuard` to install, temporarily, an
// exception-producing review handler, (6) how "REVIEW" macros would be used in
// conjunction with portions of the source code (affecting binary
// compatibility) that are incorporated only when `BDE_BUILD_TARGET_SAFE_2` is
// defined, and (7) how reviewion predicates (e.g.,
// `BSLS_REVIEW_SAFE_IS_ACTIVE`) are used to conditionally compile additional
// (redundant) defensive source code (not affecting binary compatibility)
// precisely when the corresponding (BSLS) "REVIEW" macro (e.g.,
// `BSLS_REVIEW_SAFE`) is active.
//
namespace usage_example_assert_1 {
///Example 1: Using `BSLS_REVIEW`, `BSLS_REVIEW_SAFE`, and `BSLS_REVIEW_OPT`
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component provides three different variants of (BSLS) "REVIEW" macros.
// This first usage example illustrates how one might select each of the
// particular variants, based on the runtime cost of the defensive check
// relative to that of the useful work being done.
//
// Use of the `BSLS_REVIEW_SAFE` macro is often appropriate when the defensive
// check occurs within the body of an `inline` function.  The
// `BSLS_REVIEW_SAFE` macro minimizes the impact on runtime performance as it
// is instantiated only when requested (i.e., by building in "safe mode").  For
// example, consider a light-weight point class `Kpoint` that maintains `x` and
// `y` coordinates in the range `[-1000 .. 1000]`:
// ```
// my_kpoint.h
// ...

class Kpoint {
    short int d_x;
    short int d_y;
  public:
    /// ...
    /// The behavior is undefined unless `-1000 <= x <= 1000` and
    /// `-1000 <= y <= 1000`.
    Kpoint(short int x, short int y);
    // ...
};

// ...
// ```
// Since the cost of validation here is significant compared with the useful
// work being done, we might choose to implement defensive checks using
// `BSLS_REVIEW_SAFE` as follows:
// ```
// ...

inline
Kpoint::Kpoint(short int x, short int y)
: d_x(x)
, d_y(y)
{
    (void)d_x;
    (void)d_y;
    BSLS_REVIEW_SAFE(-1000 <= x); BSLS_REVIEW_SAFE(x <= 1000);
    BSLS_REVIEW_SAFE(-1000 <= y); BSLS_REVIEW_SAFE(y <= 1000);
}
// ```
// For more substantial (non-`inline`) functions, we would be more likely to
// use the `BSLS_REVIEW` macro because the runtime overhead due to defensive
// checks is likely to be much less significant.  For example, consider a
// hash-table class that allows the client to resize the underlying table:
// ```
// my_hashtable.h
// ...

/// ...
class HashTable {
  public:
    // ...

    /// Adjust the size of the underlying hash table to be approximately the
    /// current number of elements divided by the specified `loadFactor`.
    /// The behavior is undefined unless `0 < loadFactor`.
    void resize(double loadFactor);
};
// ```
// Since the relative runtime cost of validating the input argument is quite
// small (e.g., less than 10%) compared to the typical work being done, we
// might choose to implement the defensive check using `BSLS_REVIEW` as
// follows:
// ```
// my_hashtable.cpp
// ...

void HashTable::resize(double loadFactor)
{
    (void)loadFactor;
    BSLS_REVIEW(0 < loadFactor);

    // ...
}
// ```
// In some cases, the runtime cost of checking is always negligible when
// compared with the runtime cost of performing the useful work; moreover, the
// consequences of continuing in an undefined state for certain applications
// could be catastrophic.  Instead of using `BSLS_REVIEW` in such cases, we
// might consider using `BSLS_REVIEW_OPT`.  For example, suppose we have a
// financial application class `TradingSystem` that performs trades:
// ```
// my_tradingsystem.h
// ...

/// ...
class TradingSystem {
  public:
    // ...
// ```
// Further suppose that there is a particular method `executeTrade` that takes,
// as a scaling factor, an integer that must be a multiple of 100 or the
// behavior is undefined (and might actually execute a trade):
// ```

    /// Execute the current trade using the specified `scalingFactor`.  The
    /// behavior is undefined unless `0 <= scalingFactor` and `100` evenly
    /// divides `scalingFactor`.
    void executeTrade(int scalingFactor);
    // ...
};
// ```
// Because the cost of the two checks is likely not even measurable compared to
// the overhead of accessing databases and executing the trade, and because the
// consequences of specifying a bad scaling factor are virtually unbounded, we
// might choose to implement these defensive checks using `BSLS_REVIEW_OPT` as
// follows:
// ```
// my_tradingsystem.cpp
// ...

void TradingSystem::executeTrade(int scalingFactor)
{
    (void)scalingFactor;
    BSLS_REVIEW_OPT(0 <= scalingFactor);
    BSLS_REVIEW_OPT(0 == scalingFactor % 100);

    // ...
}
// ```
// Notice that in each case, the choice of which of the three (BSLS) "REVIEW"
// macros to use is governed primarily by the relative runtime cost compared
// with that of the useful work being done (and only secondarily by the
// potential consequences of continuing execution in an undefined state).
//
}  // close namespace usage_example_assert_1
namespace usage_example_assert_2 {
///Example 2: When and How to Call the `invokeHandler` Method Directly
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// There *may* be times (but this is yet to be demonstrated) when we might
// reasonably choose to unconditionally invoke the currently installed
// assertion-failure handler directly -- i.e., instead of via one of the three
// (BSLS) "REVIEW" macros provided in this component.  Suppose that we are
// currently in the body of some function `someFunc` and, for whatever reason,
// feel compelled to invoke the currently installed assertion-failure handler
// based on some criteria other than the current build mode.
// `BSLS_REVIEW_INVOKE` is provided for this purpose.  The call might look as
// follows:
// ```
void someFunc(bool a, bool b, bool c)
{
    bool someCondition = a && b && !c;

    if (someCondition) {
        BSLS_REVIEW_INVOKE("Bad News");
    }
}
// ```
// If presented with invalid arguments, `someFunc` (above) will produce output
// similar to the following:
// ```
//  Assertion failed: Bad News, file bsls_assert.t.cpp, line 609
//  Abort (core dumped)
// ```
// If a piece of code needs to be guaranteed to not return, the additional
// macro `BSLS_REVIEW_INVOKE_NORETURN` is also available.  It behaves the same
// way as `BSLS_REVIEW_INVOKE`, but if the installed handler *does* return
// `failByAbort` will be immediately called.  On supported platforms it is
// marked appropriately to not return to support compiler requirements and
// static analysis tools.
//
}  // close namespace usage_example_assert_2
namespace usage_example_assert_3 {
///Example 3: Runtime Configuration of the `bsls::Review` Facility
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// By default, any review failure will result in the invocation of the
// `bsls::Review::failByLog` handler function.  We can replace this behavior
// with that of one of the other static failure handler methods supplied in
// `bsls::Review` as follows.  Let's assume we are at the top of our
// application called `myMain` (which would typically be `main`):
// ```
void myMain()
{
// ```
// First observe that the default assertion-failure handler function is, in
// fact, `bsls::Review::failByLog`:
// ```
assert(&bsls::Review::failByLog == bsls::Review::violationHandler());
// ```
// Next, we install a new assertion-failure handler function,
// `bsls::Review::failBySleep`, from the suite of "off-the-shelf" handlers
// provided as `static` methods of `bsls::Review`:
// ```
bsls::Review::setViolationHandler(&bsls::Review::failBySleep);
// ```
// Observe that `bsls::Review::failBySleep` is the new, currently-installed
// assertion-failure handler:
// ```
assert(&bsls::Review::failBySleep == bsls::Review::violationHandler());
// ```
// Note that if we were to explicitly invoke the current assertion-failure
// handler as follows:
// ```
//  BSLS_REVIEW_INVOKE("message");  // This will hang!
// ```
// the program will hang since `bsls::Review::failBySleep` repeatedly sleeps
// for a period of time within an infinite loop.  Thus, this assertion-failure
// handler is useful for hanging a process so that a debugger may be attached
// to it.
//
// We may now decide to disable the `setViolationHandler` method using the
// `bsls::Review::lockReviewAdministration()` method to ensure that no one else
// will override our decision globally.  Note, however, that the
// `bsls::ReviewFailureHandlerGuard` is not affected, and can still be used to
// supplant the currently installed handler (see below):
// ```
bsls::Review::lockReviewAdministration();
// ```
// Attempting to change the currently installed handler now will fail:
// ```
    bsls::Review::setViolationHandler(&bsls::Review::failByAbort);

    assert(&bsls::Review::failByAbort != bsls::Review::violationHandler());

    assert(&bsls::Review::failBySleep == bsls::Review::violationHandler());
}
// ```
//
}  // close namespace usage_example_assert_3
namespace usage_example_assert_4 {
///Example 4: Creating a Custom Assertion Handler
/// - - - - - - - - - - - - - - - - - - - - - - -
// Sometimes, especially during testing, we may need to write our own custom
// assertion-failure handler function.  The only requirements are that the
// function have the same prototype (i.e., the same respective parameter and
// return types) as the `bsls::Review::Handle` `typedef`, and that the function
// should not return (i.e., it must `abort`, `exit`, `terminate`, `throw`, or
// hang).  To illustrate, we will create a `static` method at file scope that
// conforms to the required structure (notice the explicit use of `std::printf`
// from `<cstdio>` instead of `std::cout` from `<iostream>` to avoid
// interaction with the C++ memory allocation layer):
// ```
static bool globalEnableOurPrintingFlag = true;

/// Print the expression `comment`, `file` name, and `line` number from the
/// specified `violation` to `stdout` as a comma-separated list, replacing
/// null string-argument values with empty strings (unless printing has been
/// disabled by the `globalEnableOurPrintingFlag` variable), then
/// unconditionally abort.
static
void ourFailureHandler(const bsls::ReviewViolation& violation)
{
    const char *comment = violation.comment();
    if (!comment) {
        comment = "";
    }
    const char *file = violation.fileName();
    if (!file) {
        file = "";
    }
    int line = violation.lineNumber();
    if (globalEnableOurPrintingFlag) {
        std::printf("%s, %s, %d\n", comment, file, line);
    }
    std::abort();
}
// ```
// At the top level of our application we have the following:
// ```
void ourMain()
{
// ```
// First, let's observe that we can assign this new function to a function
// pointer of type `bsls::Review::Handler`:
// ```
bsls::Review::ViolationHandler f = &ourFailureHandler;
(void)f;
// ```
// Now we can install it just as we would any other handler:
// ```
bsls::Review::setViolationHandler(&ourFailureHandler);
// ```
// We can now invoke the default handler directly:
// ```
//  BSLS_REVIEW_INVOKE("str1");
}
// ```
// With the resulting output something like as follows:
// ```
//  str1, my_file.cpp, 17
//  Abort (core dumped)
// ```
//
}  // close namespace usage_example_assert_4
namespace usage_example_assert_5 {
///Example 5: Using the `bsls::ReviewFailureHandlerGuard`
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Sometimes we may want to replace, temporarily (i.e., within some local
// lexical scope), the currently installed assertion-failure handler function.
// In particular, we sometimes use the `bsls::ReviewFailureHandlerGuard` class
// to replace the current handler with one that throws an exception (because we
// know that such an exception is safe in the local context).  Let's start with
// the simple factorial function below, which validates, in "debug mode" (or
// "safe mode"), that its input is non-negative:
// ```

/// Return `n!`.  The behavior is undefined unless `0 <= n`.
double fact(int n)
{
    BSLS_REVIEW(0 <= n);

    double result = 1.0;
    while (n > 1) {
        result *= n--;
    }
    return result;
}
// ```
// Now consider the following integer-valued `extern "C"` C++ function,
// `wrapperFunc`, which can be called from C and FORTRAN, as well as from C++:
// ```
extern "C" int wrapperFunc(bool verboseFlag)
{
    enum { GOOD = 0, BAD } result = GOOD;  (void) verboseFlag;
// ```
// The purpose of this function is to allow assertion failures in subroutine
// calls below this function to be handled by throwing an exception, which is
// then caught by the wrapper and reported to the caller as a "bad" status.
// Hence, when within the runtime scope of this function, we want to install,
// temporarily, the assertion-failure handler `bsls::Review::failByThrow`,
// which, when invoked, causes an `bsls::AssertTestException` object to be
// thrown.  (Note that we are not advocating this approach for "recovery", but
// rather for an orderly shut-down, or perhaps during testing.)  The
// `bsls::ReviewFailureHandlerGuard` class is provided for just this purpose:
// ```
    assert(&bsls::Review::failByLog == bsls::Review::violationHandler());

    bsls::ReviewFailureHandlerGuard guard(&bsls::Review::failByThrow);

    assert(&bsls::Review::failByThrow == bsls::Review::violationHandler());
// ```
// Next we open up a `try` block, and somewhere within the `try` we
// "accidentally" invoke `fact` with an out-of-contract value (i.e., `-1`):
// ```
#ifdef BDE_BUILD_TARGET_EXC
    try
#endif
        {

        // ...

        double d = fact(-1);        // Out-of-contract call to `fact`.
        (void)d;

        // ...
    }
#ifdef BDE_BUILD_TARGET_EXC
    catch (const bsls::AssertTestException& e) {
        result = BAD;
        if (verboseFlag) {
            std::printf( "Internal Error: %s, %s, %d\n",
                         e.expression(),
                         e.filename(),
                         e.lineNumber() );
        }
    }
#endif
    return result;
}
// ```
// Assuming exceptions are enabled (i.e., `BDE_BUILD_TARGET_EXC` is defined),
// if an `bsls::AssertTestException` occurs below this wrapper function, the
// exception will be caught, a message will be printed to `stdout`, e.g.,
// ```
//  Internal Error: bsls_assert.t.cpp:500: 0 <= n
// ```
// and the `wrapperFunc` function will return a bad status (i.e., 1) to its
// caller.  Note that if exceptions are not enabled,
// `bsls::Review::failByThrow` will behave as `bsls::Review::failByAbort`, and
// dump core immediately:
// ```
//  Assertion failed: 0 <= n, file bsls_assert.t.cpp, line 500
//  Abort (core dumped)
// ```
// Finally note that the `bsls::ReviewFailureHandlerGuard` is not thread-aware.
// In particular, a guard that is created in one thread will also affect the
// failure handlers that are used in other threads.  Care should be taken when
// using this guard when more than a single thread is executing.
//
}  // close namespace usage_example_assert_5
namespace usage_example_assert_6 {
///Example 6: Using (BSLS) "REVIEW" Macros Along With `BDE_BUILD_TARGET_SAFE_2`
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Recall that assertions do not affect binary compatibility; however, software
// built with `BDE_BUILD_TARGET_SAFE_2` defined need not be binary compatible
// with software built otherwise.  In this example, we look at how we might use
// the (BSLS) "REVIEW" family of macros in conjunction with code that is
// incorporated (at compile time) only when `BDE_BUILD_TARGET_SAFE_2` is
// defined.
//
// As a simple example, let's consider an elided implementation of a
// singly-linked integer list and its iterator.  Whenever
// `BDE_BUILD_TARGET_SAFE_2` is defined, we want to defend against the
// possibility that a client mistakenly passes a `ListIter` object into a
// `List` object method (e.g., `List::insert`) where that `ListIter` object did
// not originate from the same `List` object.
//
// We'll start by defining a local helper `List_Link` `struct` as follows:
// ```
struct List_Link {
    List_Link *d_next_p;
    int        d_data;
    List_Link(List_Link *next, int data) : d_next_p(next), d_data(data) { }
};
// ```
// Next, we'll define `ListIter`, which always identifies the current position
// in a sequence of links, but whenever `BDE_BUILD_TARGET_SAFE_2` is defined,
// also maintains a pointer to its parent `List` object:
// ```
class List;                         // Forward declaration.

class ListIter {
#ifdef BDE_BUILD_TARGET_SAFE_2
    List *d_parent_p;               // Exists only in "safe 2 mode".
#endif
    List_Link **d_current_p;
    friend class List;
    friend bool operator==(const ListIter&, const ListIter&);
  private:
    ListIter(List_Link **current,
             List *
#ifdef BDE_BUILD_TARGET_SAFE_2
                   parent           // Not used unless in "safe 2 mode".
#endif
            )
    : d_current_p(current)
#ifdef BDE_BUILD_TARGET_SAFE_2
    , d_parent_p(parent)            // Initialize only in "safe 2 mode".
#endif
    { }
  public:
    ListIter& operator++() { /* ... */ return *this; }
    // ...
};
bool operator==(const ListIter& lhs, const ListIter& rhs);
bool operator!=(const ListIter& lhs, const ListIter& rhs);
// ```
// Finally we define the `List` class itself with most of the operations
// elided; the methods of particular interest here are `begin` and `insert`:
// ```

class List {
    List_Link *d_head_p;
  public:
    // CREATORS
    List() : d_head_p(0) { }
    List(const List&) { /* ... */ }
    ~List() { /* ... */ }

    // MANIPULATORS
    List& operator=(const List&) { /* ... */ return *this; }

    //| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
    //v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    /// Return an iterator referring to the beginning of this list.
    ListIter begin()
    {
        return ListIter(&d_head_p, this);
    }
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    //| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
    //v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    /// Insert the specified `data` value into this list at the specified
    /// `position`.
    void insert(const ListIter& position, int data)
    {
#ifdef BDE_BUILD_TARGET_SAFE_2
        BSLS_REVIEW_SAFE(this == position.d_parent_p);  // "safe 2 mode"
#endif
        *position.d_current_p = new List_Link(*position.d_current_p, data);
    }
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


    // ACCESSORS

    /// Output the contents of this list to `stdout`.
    void print()
    {
        printf( "[" );
        for (List_Link *p = d_head_p; p; p = p->d_next_p) {
            printf( " %d", p->d_data );
        }
        printf(" ]\n");
    }
};
// ```
// Outside of "safe 2 mode", it is possible to pass an iterator object obtained
// from the `begin` method of one `List` object into the `insert` method of
// another, having, perhaps, unexpected results:
// ```
void sillyFunc(bool printFlag)
{
    List a;
    ListIter aIt = a.begin();
    a.insert(aIt, 1);
    a.insert(aIt, 2);
    a.insert(aIt, 3);

    if (printFlag) {
        std::printf( "a = "); a.print();
    }

    List b;
    ListIter bIt = b.begin();
    a.insert(bIt, 4);       // Oops!  Should have been: `b.insert(bIt, 4);`
    a.insert(bIt, 5);       // Oops!    "     "     "   `    "     "   5  `
    a.insert(bIt, 6);       // Oops!    "     "     "   `    "     "   6  `

    if (printFlag) {
        std::printf( "a = "); a.print();
        std::printf( "b = "); b.print();
    }
}
// ```
// In the example above, we have "accidentally" passed the iterator `bIt`
// obtained from `List` object `b` into the `insert` method for `List` object
// `a`.  The resulting undefined behavior (in other than "safe 2 mode") might
// produce output that looks as follows:
// ```
//  a = [ 3 2 1 ]
//  a = [ 3 2 1 ]
//  b = [ 6 5 4 ]
// ```
// If the same `sillyFunc` were compiled in "safe 2 mode" (i.e., with
// `BDE_BUILD_TARGET_SAFE_2` defined) the undefined behavior would be detected
// and the output would, by default, look more like the following:
// ```
//  a = [ 3 2 1 ]
//  Assertion failed: this == position.d_parent_p, file my_list.cpp, line 56
//  Abort (core dumped)
// ```
// thereby quickly exposing the misuse by the client.
//
}  // close namespace usage_example_assert_6
namespace usage_example_assert_7 {
///Example 7: Conditional Compilation Associated with Enabled Assertion Levels
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In cases where we want to tie code, other than just an assertion, to a
// specific level of enabled assertions, we will want to use the corresponding
// intermediate predicate that enables that level of assertions:
//
//  - For `BSLS_REVIEW_SAFE`, use `BSLS_REVIEW_SAFE_IS_ACTIVE`.
//
//  - For `BSLS_REVIEW`, use `BSLS_REVIEW_IS_ACTIVE`.
//
//  - For `BSLS_REVIEW_OPT`, use `BSLS_REVIEW_OPT_IS_ACTIVE`.
//
// Suppose that we have a class such as `MyDate` (below) that, except for
// checking its invariants, would have a trivial destructor.  By not declaring
// a destructor at all, we may realize performance advantages, but then we lose
// the ability to validate our invariants in "debug" or "safe" mode.  What we
// want to do is to declare (and later define) the destructor in precisely
// those build modes for which we would want to assert invariants.
//
// An elided class `MyDate`, which is based on a serial-date implementation, is
// provided for reference:
// ```

/// This class implements a value-semantic "date" type representing valid
/// date values in the range `[ 0001Jan01 .. 9999Dec31 ]`.
class MyDate {

    // DATA
    int d_serialDate;  // sequential representation within a valid range

  public:
     // CLASS METHODS

     // ...

     // CREATORS

     /// Create a `MyDate` object having the value `0001Jan01`.
     MyDate();

     // ...

     /// Create a `MyDate` object having the same value as the specified
     /// `original` object.
     MyDate(const MyDate& original);

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
     ~MyDate();
         // Destroy this object.  Note that in some build modes the destructor
         // generated by the compiler is trivial.
#endif

    // ...
};

// ...

// ========================================================================
//                  INLINE FUNCTION DEFINITIONS
// ========================================================================

// ...

// CREATORS
inline
MyDate::MyDate()
: d_serialDate(1)  // 0001Jan01
{
}

inline
MyDate::MyDate(const MyDate& original)
: d_serialDate(original.d_serialDate)
{
}

// ...

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
inline
MyDate::~MyDate()
{
    BSLS_REVIEW_SAFE(1 <= d_serialDate);             // 0001Jan01
    BSLS_REVIEW_SAFE(     d_serialDate <= 3652061);  // 9999Dec31
}
#endif

// ...
// ```
// In practice, however, we would probably implement an `isValidSerialDate`
// method in a lower-level utility class, e.g., `MyDateImpUtil`, leading to
// code that is more fine-grained, modular, and hierarchically reusable:
// ```

/// Return `true` if the specified `d_date` represents a valid date
/// value, and `false` otherwise.
struct MyDateImpUtil {
    static bool isValidSerialDate(int d_date);
};

inline
bool MyDateImpUtil::isValidSerialDate(int d_date)
{
    return 1 <= d_date && d_date <= 3652061;
}
// ```
// Like other aspects of `BSLS_REVIEW_SAFE`, the example above violates the
// one-definition rule for mixed-mode builds.  Note that all code conditionally
// compiled based on `BSLS_REVIEW_SAFE_IS_ACTIVE`, `BSLS_REVIEW_IS_ACTIVE`, and
// `BSLS_REVIEW_OPT_IS_ACTIVE` should be binary compatible for mixed-mode
// builds.  If the conditionally-compiled code would not be binary compatible,
// use `BDE_BUILD_TARGET_SAFE_2` instead.
//
// WARNING - In practice, declaring a destructor in some build modes but not
// others has led to subtle and difficult-to-diagnose failures.  DON'T DO IT!
//
// Finally, in very rare cases, we may want to put in (redundant) defensive
// code (in the spirit of `BSLS_REVIEW_OPT`) that is not part of the
// component-level contract, yet (1) is known to have negligible runtime cost
// and (2) is deemed to be so important as to be necessary even for optimized
// builds.
//
// For example, consider again the `MyDate` class above that now also declares
// a non-`inline` `print` method to format the current date value in some
// human-readable, but otherwise unspecified format:
// ```
// xyza_mydate.h
// ...
namespace usage_example_assert_7_b {
/// ...
class MyDate {

    // DATA
    int d_serialDate;  // sequential representation within a valid range

  public:
    // ...
    // ACCESSORS
    // ...

    /// Write the value of this object to the specified output `stream` in
    /// some human-readable format, and return a reference to `stream`.
    /// Optionally specify ...
    std::ostream& print(std::ostream& stream, ...) const;

    // ...

};
// ```
// Successfully writing bad data is among the most insidious of bugs, because a
// latent error can persist and not be discovered until long after the program
// terminates.  Writing the value of a corrupted `MyDate` object in a
// *machine-readable* (binary) format is an error so serious as to warrant
// invoking
// ```
void testFunction(int d_serialDate) {
    (void)d_serialDate;
    BSLS_REVIEW_OPT(MyDateImpUtil::isValidSerialDate(d_serialDate));
}
// ```
// each time we attempt the output operation; however, printing the value in a
// human-readable format intended primarily for debugging purposes is another
// matter.  In anything other than a safe build (which in this case would
// enforce essentially all method preconditions), it would be unfortunate if a
// developer, knowing that there was a problem involving the use of `MyDate`,
// inserted print statements to identify that problem, only to have the `print`
// method itself ruthlessly invoke the assert handler, likely terminating the
// process).  Moreover, it may also be unsafe even to attempt to format the
// value of a `MyDate` object whose `d_serialDate` value violates its
// invariants (e.g., due to a static table lookup).  In such cases we may, as
// sympathetic library developers, choose to implement different undefined
// (undocumented) redundant defensive behaviors, depending on the desired level
// of assertions:
// ```
// xyza_mydate.cpp
// ...
// #include <xyza_mydateimputil.h>
// ...

std::ostream& MyDate::print(std::ostream& stream, ...) const
{
    // BSLS_REVIEW(/* any *argument* preconditions for this function */);

    // Handle case where the invariants have been violated.

    (void)d_serialDate;
#ifdef BSLS_REVIEW_OPT_IS_ACTIVE
    // Note that if `BSLS_REVIEW_LEVEL_NONE` has been set, this code -- along
    // with all `BSLS_REVIEW_OPT` macros -- will not instantiate, enabling us
    // to verify that the combined runtime overhead of all such (redundant)
    // defensive code is at worst negligible, if not unmeasurable.

    if (!MyDateImpUtil::isValidSerialDate(d_serialDate)) {

        // Our invariant is corrupted.

#ifdef BSLS_REVIEW_IS_ACTIVE
        // Providing debugging information in this mode would be useful.

        std::cerr << "\nxyza::MyDate: Invalid internal serial date value "
                  << d_serialDate << '.' << std::endl;

#endif // BSLS_REVIEW_IS_ACTIVE

        // In safe mode, each of the `MyClass` methods fully guards its
        // preconditions: There is simply no easy way to get here!

        BSLS_REVIEW_SAFE("Probable rogue memory overwrite!" && 0);

        // If we get here, we're corrupted, but not in safe mode!

        return stream << "(* Invalid `MyDate` State "
                      << d_serialDate
                      << " *)" << std::flush;                         // RETURN

    }

#endif // BSLS_REVIEW_OPT_IS_ACTIVE

    // If we get here in a production build, this object is "sane": Do whatever
    // this `print` method would normally do, assuming that no method
    // preconditions or object invariants are violated.

    // ...  <*** Your (Normal-Case) Formatting Code Here! ***>

    return stream;
}
// ```
}  // close namespace usage_example_assert_7_b
}  // close namespace usage_example_assert_7
///Example 8: Conditional Compilation of Support Functions
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Occasionally a function may exist only to support a specific set of
// assertions.  Often this can happen when a large expression that captures a
// complicated precondition wants to be refactored into a distinct location to
// ease understanding of it.  When this happens the function might still remain
// as a private implementation detail of the class.
//
// When the only assertion macros that use the function are disabled this can
// lead to a compiler warning about a function being unused, and the
// corresponding code bloat from having the function available might be an
// overhead that is not desired.
//
// In order to totally remove the function when it is not needed, the `IS_USED`
// suffixed macros can be used to guard the declaration and definition of the
// function.  Suppose we have a `class` with a function having a complex
// precondition, and that precondition check is both private and only needed
// when the assertions that use it are enabled.  In that case, we can guard the
// definitions and declarations against even being compiled like this:
// ```
namespace usage_example_assert_8 {
/// ...
class ComplexObject {
#if defined(BSLS_REVIEW_SAFE_IS_USED)
    bool isPurplish() const;
        // Return `true` if the current state of this object fits within the
        // complex requirements of being sufficiently purple, false otherwise.
#endif
    // ...
public:
    // MANIPULATORS
 void doSomethingPurpley();
        // Do something purpley.  The behavior is undefined unless this object
        // is currently purplish (contact customer support to know the current
        // threshholds for purplishness).
};

#if defined(BSLS_REVIEW_SAFE_IS_USED)
bool ComplexObject::isPurplish() const
{
    // The real implementation would encode the complex logic of needing to
    // determine if this object feels purplish at the moment.
    return true;
}
#endif

void ComplexObject::doSomethingPurpley()
{
    BSLS_REVIEW_SAFE(isPurplish());
}
}  // close namespace usage_example_assert_8
// ```
// Now, the `ComplexObject::isPurplish` function will only exist in a subset of
// builds:
//  - When `BSLS_REVIEW_SAFE` assertions are enabled in assert or review mode,
//    the function will be compiled and invoked.
//  - When `BSLS_REVIEW_VALIDATE_DISABLED_MACROS` is defined the function will
//    be compiled.  This will make sure that a future change does not
//    invalidate the implementation of `isPurplish()` even though it is not
//    used.
//  - When `BSLS_REVIEW_SAFE` assertions are assumed the function will be
//    compiled and might be invoked, or at least have its implementation
//    inspected by the compiler to improve code generation.
//
///Example 9: Conditional Compilation of Support Code
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Sometimes there is code that needs to run in a function before an assertion
// to gather information needed only by that assertion.  Often this can be
// capturing input values or other system state before it is modified and
// verifying at the end of a function that the values are changed (or not)
// appropriately.
//
// When the corresponding assertion macro is not active in assert or review
// mode the supporting code should not be executed at all.  Importantly,
// because the capturing of additional information is an extra cost, the
// assertion itself does not lend itself to being assumed.
//
// Suppose we have a function that wishes to swap the values of its input:
// ```
namespace usage_example_assert_9_a {
/// Exchange the values of the specified `lhs` and `rhs`.
struct MySwapper {
    template <class T>
    static void swap(T& lhs, T& rhs)
    {
        T tmp = lhs;
        lhs = rhs;
        rhs = tmp;
    }
};
}  // close namespace usage_example_assert_9_a
// ```
// This works great as a simple `swap` implementation, but we would like to
// assert in safe mode that it is doing the correct thing.  In order to do that
// we need to capture the initial values of our inputs before doing anything
// else, and we want to do this only when the respective assertions are
// enabled.  Here we would guard our code and our assertions in a check that
// `BSLS_REVIEW_SAFE_IS_ACTIVE` is defined, like this:
// ```
namespace usage_example_assert_9_b {
/// Exchange the values of the specified `lhs` and `rhs`.
struct MySwapper {
    template <class T>
    static void swap(T& lhs, T& rhs)
    {
#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
        T origLhs(lhs);
        T origRhs(rhs);
#endif
        T tmp = lhs;
        lhs = rhs;
        rhs = tmp;
#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
        BSLS_REVIEW_SAFE(rhs == origLhs);
        BSLS_REVIEW_SAFE(lhs == origRhs);
#endif
    }
};
}  // close namespace usage_example_assert_9_b
// ```

// End of usage examples
// BDE_VERIFY pragma: pop

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

void test_case_21() {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #1
        //
        // Concerns:
        // 1. The usage example provided in the component header file must
        //    compile, link, and run on all platforms as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into driver, remove leading
        //    comment characters, and replace `assert` with "ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE: Adding `BSLS_ASSERT` to an existing function
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE #1"
                            "\n================\n");

        if (verbose) printf("\n1. introducing a new bsls_assert.\n");

        ASSERT(&bsls::Review::failByLog == bsls::Review::violationHandler());

        bsls::ReviewFailureHandlerGuard guard(&bsls::Review::failByThrow);
        ASSERT(&bsls::Review::failByThrow == bsls::Review::violationHandler());


        if (verbose) printf("\n2. Running myFunc with valid arguments.\n");
        usage_example_review_1::FunctionsV1::myFunc(1,1);
        usage_example_review_1::FunctionsV2::myFunc(1,1);
        usage_example_review_1::FunctionsV3::myFunc(1,1);
        usage_example_review_1::FunctionsV4::myFunc(1,1);
}

void test_case_20() {
        // --------------------------------------------------------------------
        // ASSERT USAGE EXAMPLE #9
        //
        // Concerns:
        // 1. The usage example provided in the bsls_assert header must
        //    compile, link and run with BSLS_REVIEW instead.
        //
        // Plan:
        // 1. Incorporate usage example from header into driver, remove leading
        //    comment characters, and replace `assert` with `ASSERT`.
        //
        // Testing:
        //   USAGE EXAMPLE: Conditional Compilation of Support Code
        // --------------------------------------------------------------------

        if (verbose) printf( "\nUSAGE EXAMPLE #8"
                             "\n================\n" );

        if (verbose) printf( "\n9. Conditional Compilation of Code\n");

        // See usage examples section at top of this file.
        int i = 17;
        int j = 19;
        usage_example_assert_9_a::MySwapper::swap(i,j);
        ASSERT(i == 19 && j == 17);
        usage_example_assert_9_b::MySwapper::swap(i,j);
        ASSERT(i == 17 && j == 19);

}

void test_case_19() {
        // --------------------------------------------------------------------
        // ASSERT USAGE EXAMPLE #8
        //
        // Concerns:
        // 1. The usage example provided in the bsls_assert header must
        //    compile, link and run with BSLS_REVIEW instead.
        //
        // Plan:
        // 1. Incorporate usage example from header into driver, remove leading
        //    comment characters, and replace `assert` with `ASSERT`.
        //
        // Testing:
        //   USAGE EXAMPLE: Conditional Compilation of Support Functions
        // --------------------------------------------------------------------

        if (verbose) printf( "\nUSAGE EXAMPLE #8"
                             "\n================\n" );

        if (verbose) printf( "\n8. Conditional Compilation of Functions\n");

        // See usage examples section at top of this file.
        usage_example_assert_8::ComplexObject o;
        o.doSomethingPurpley();
}

void test_case_18() {
        // --------------------------------------------------------------------
        // ASSERT USAGE EXAMPLE #7
        //
        // Concerns:
        // 1. The usage example provided in the bsls_assert header must
        //    compile, link and run with BSLS_REVIEW instead.
        //
        // Plan:
        // 1. Incorporate usage example from header into driver, remove leading
        //    comment characters, and replace `assert` with `ASSERT`.
        //
        // Testing:
        //   ASSERT USAGE EXAMPLE: Conditional Compilation
        // --------------------------------------------------------------------

        if (verbose) printf( "\nASSERT USAGE EXAMPLE #7"
                             "\n=======================\n" );

        if (verbose) printf( "\n7. Conditional Compilation Associated with "
                             "Enabled Assertion Levels\n");

        // See usage examples section at top of this file.

        ASSERT(usage_example_assert_7::MyDateImpUtil::isValidSerialDate(1));
}

void test_case_17() {
        // --------------------------------------------------------------------
        // ASSERT USAGE EXAMPLE #6
        //
        // Concerns:
        // 1. The usage example provided in the bsls_assert header must
        //    compile, link and run with BSLS_REVIEW instead.
        //
        // Plan:
        // 1. Incorporate usage example from header into driver, remove leading
        //    comment characters, and replace `assert` with `ASSERT`.
        //
        // Testing:
        //   ASSERT USAGE EXAMPLE: Using `BDE_BUILD_TARGET_SAFE_2`
        // --------------------------------------------------------------------

        if (verbose) printf("\nASSERT USAGE EXAMPLE #6"
                            "\n=======================\n");

        if (verbose) printf("\n6. Using (BSLS) \"REVIEW\" Macros in "
                            "Conjunction with BDE_BUILD_TARGET_SAFE_2\n");

        // See usage examples section at top of this file.

#if defined(BDE_BUILD_TARGET_ASAN)
        // This usage example demonstrates assertions on runtime failure that
        // are not compatible with asan reporting those same issues as errors.
        if (verbose) puts("\tThis test is not compatible with asan");
#elif !defined(BDE_BUILD_TARGET_SAFE_2)
        if (veryVerbose) printf( "\tsafe mode 2 is *not* defined\n" );
        usage_example_assert_6::sillyFunc(veryVerbose);
#else
        if (veryVerbose) printf( "\tSAFE MODE 2 *is* defined.\n" );

        // bsls::Review::setViolationHandler(::testDriverPrint);
                                                          // for usage example
        bsls::Review::setViolationHandler(::testDriverHandler);
                                                          // for regression
        globalReset();
        ASSERT(false == globalReviewFiredFlag);
# ifdef BDE_BUILD_TARGET_EXC
        try
        {
# endif // BDE_BUILD_TARGET_EXC
            usage_example_assert_6::sillyFunc(veryVerbose);
            ASSERT(false);
# ifdef BDE_BUILD_TARGET_EXC
        }
        catch(const std::exception &)
        {
            ASSERT(true == globalReviewFiredFlag);
        }
# endif // BDE_BUILD_TARGET_EXC
#endif  // BDE_BUILD_TARGET_SAFE_2
}

void test_case_16() {
        // --------------------------------------------------------------------
        // ASSERT USAGE EXAMPLE #5
        //
        // Concerns:
        // 1. The usage example provided in the bsls_assert header must
        //    compile, link and run with BSLS_REVIEW instead.
        //
        // Plan:
        // 1. Incorporate usage example from header into driver, remove leading
        //    comment characters.
        //
        // Testing:
        //   ASSERT USAGE EXAMPLE: Using Scoped Guard
        // --------------------------------------------------------------------

        if (verbose) printf( "\nASSERT USAGE EXAMPLE #5"
                             "\n=======================\n" );

#ifndef BDE_BUILD_TARGET_EXC
        if (verbose) {
            printf( "\tTest disabled as exceptions are NOT enabled.\n"
                    "\tCalling the test function would abort.\n" );
        }
#else
        if (verbose) printf( "\n5. Using the "
                             "bsls::ReviewFailureHandlerGuard\n" );


        // See usage examples section at top of this file.

        ASSERT(&bsls::Review::failByLog == bsls::Review::violationHandler());

#ifndef BDE_BUILD_TARGET_OPT
    #if defined(BSLS_REVIEW_IS_ACTIVE) ||                                     \
        defined(BSLS_REVIEW_ENABLE_TEST_CASE_16)

        if (verbose) printf( "\n*** Note that the following 'Internal "
                             "Error: ... `0 <= n` message is expected:\n" );

        ASSERT(0 != usage_example_assert_5::wrapperFunc(verbose));

    #endif
#endif
#endif
        ASSERT(&bsls::Review::failByLog == bsls::Review::violationHandler());
}

void test_case_15() {
        // --------------------------------------------------------------------
        // ASSERT USAGE EXAMPLE #4
        //
        // Concerns:
        // 1. The usage example provided in the bsls_assert header must
        //    compile, link and run with BSLS_REVIEW instead.
        //
        // Plan:
        // 1. Incorporate usage example from header into driver, remove leading
        //    comment characters.
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

        usage_example_assert_4::globalEnableOurPrintingFlag = veryVerbose;

        REVIEW_TEST_BEGIN
        usage_example_assert_4::ourMain();
        REVIEW_TEST_END
#endif
}

void test_case_14() {
        // --------------------------------------------------------------------
        // ASSERT USAGE EXAMPLE #3
        //
        // Concerns:
        // 1. The usage example provided in the bsls_assert header must
        //    compile, link and run with BSLS_REVIEW instead.
        //
        // Plan:
        // 1. Incorporate usage example from header into driver, remove leading
        //    comment characters.
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

        usage_example_assert_3::myMain();
}

void test_case_13() {
        // --------------------------------------------------------------------
        // ASSERT USAGE EXAMPLE #2
        //
        // Concerns:
        // 1. The usage example provided in the bsls_assert header must
        //    compile, link and run with BSLS_REVIEW instead.
        //
        // Plan:
        // 1. Incorporate usage example from header into driver, remove leading
        //    comment characters.
        //
        // Testing:
        //   ASSERT USAGE EXAMPLE: Invoking an review handler directly
        // --------------------------------------------------------------------

        if (verbose) printf( "\nASSERT USAGE EXAMPLE #2"
                             "\n=======================\n" );

        if (verbose) printf( "\n2. When and How to Call the Invoke-Handler "
                             "Method Directly\n" );

        // See usage examples section at top of this file.

        bsls::Review::setViolationHandler(::testDriverPrint);

#ifndef BDE_BUILD_TARGET_EXC
        globalReturnOnTestReview = true;
#endif

        REVIEW_TEST_BEGIN
        usage_example_assert_2::someFunc(1, 1, 0);
        REVIEW_TEST_END

#ifndef BDE_BUILD_TARGET_EXC
        globalReturnOnTestReview = false;
#endif
}

void test_case_12() {
        // --------------------------------------------------------------------
        // ASSERT USAGE EXAMPLE #1
        //
        // Concerns:
        // 1. The usage example provided in the bsls_assert header must
        //    compile, link and run with BSLS_REVIEW instead.
        //
        // Plan:
        // 1. Incorporate usage example from header into driver, remove leading
        //    comment characters.
        //
        // Testing:
        //   ASSERT USAGE EXAMPLE: Using Review Macros
        // --------------------------------------------------------------------

        if (verbose) printf( "\nASSERT USAGE EXAMPLE #1"
                             "\n=======================\n" );

        if (verbose) printf( "\n1. Using BSLS_REVIEW, BSLS_REVIEW_SAFE, and "
                             "BSLS_REVIEW_OPT\n");

        // See usage examples section at top of this file.
}

namespace {

class StringResetter {
public: 
    StringResetter(char*       buffer,
                   std::size_t bufferSize,
                   const char* newValue)
    : d_buffer(buffer)
    , d_bufferSize(bufferSize)
    , d_newValue(newValue)
    {}
    
    ~StringResetter()
    {
        std::strncpy(d_buffer, d_newValue, d_bufferSize);
    }

    const char* buffer() const
    { return d_buffer; }

private:
    char*       d_buffer;
    std::size_t d_bufferSize;
    const char *d_newValue;

};

void testCase11Handler(const bsls::ReviewViolation& violation)
{
    ASSERTV(violation.comment(), 0 == strcmp(violation.comment(), "InitValue"));

#ifdef BDE_BUILD_TARGET_EXC
    // Test that throwing will produce an exception that still has the same
    // contents even if they are changed or freed as the stack unwinds.
    bsls::Review::failByThrow(violation);
#endif
}

}

void test_case_11() {
        // --------------------------------------------------------------------
        // Dynamic assertion messages
        //
        // Concerns:
        // 1. The string passed to `BSLS_REVIEW_INVOKE` might have a lifetime
        //    that ends after the complete expression used as an argument to
        //    the macro, which must be after invoking the violation handler
        //    (DRQS 181156417}.
        //
        // 2. An `AssertTestException` constructed from a violation object must
        //    not contain references to strings that might be freed before that
        //    exception is caught {DRQS 180431036}.
        //
        // Plan:
        // 1. Use an expression that changes the contents of the string when
        //    the temporary it returns is destroyed, verify that the violation
        //    handler is invoked with the unchanged string.
        //
        // 2. Throw a `bsls::AssertTestException` created from the violation so
        //    we can verify that its expression also remains correct as the
        //    stack is unwound.
        //
        // Testing:
        //   CONCERN: Dynamic assertion messages
        // --------------------------------------------------------------------

        if (verbose) printf( "\nDynamic Assertion Messages"
                             "\n==========================\n" );

        bsls::ReviewFailureHandlerGuard guard(&testCase11Handler);

        if (veryVerbose) printf( "\tTesting BSLS_REVIEW_INVOKE\n" );        
        {
            char buffer[32] = "InitValue";
#ifdef BDE_BUILD_TARGET_EXC
            try {
#endif                
                BSLS_REVIEW_INVOKE(
                    StringResetter(buffer,
                                   sizeof(buffer)-1,
                                   "ResetValue").buffer() );
#ifdef BDE_BUILD_TARGET_EXC
            } catch (const bsls::AssertTestException& ex) {
                // The below checks address {DRQS 180431036}
                ASSERT(buffer != ex.expression());
                ASSERTV(ex.expression(),
                        0 == strcmp(ex.expression(), "InitValue"));
            }
#endif            
        }
}

void test_case_10() {
        // --------------------------------------------------------------------
        // `CONSTEXPR` USE
        //
        // Concerns:
        // 1. The `BSLS_REVIEW` macros should compile when used in a function
        //    marked with `BSLS_KEYWORD_CONSTEXPR_CPP14`.
        //
        // Plan:
        // 1. Functions defined earlier must compile and be executable in
        //    contract at compile time.
        //
        // Testing:
        //   CONCERN: `constexpr` interaction
        // --------------------------------------------------------------------

        if (verbose) printf( "\n'CONSTEXPR' USE"
                             "\n===============\n" );

        BSLS_KEYWORD_CONSTEXPR_CPP17 int value =
                               ConstexprFunctions::narrowConstexprFunction(17);
        ASSERT(value == 17);

        int rtValue = ConstexprFunctions::narrowConstexprFunction(34);
        ASSERT(rtValue == 34);

#ifdef BDE_BUILD_TARGET_EXC
        bsls::ReviewFailureHandlerGuard guard(bsls::Review::failByThrow);

        try {
            int badValue = ConstexprFunctions::narrowConstexprFunction(-1);
            ASSERT(false);
            ASSERT(badValue == -1);
        } catch (...) {
            if (verbose) printf("Expected failure caught.\n");
        }

#else
        if (verbose) printf( "\nNON-EXCEPTION BUILD\n" );
#endif
}

void test_case_9() {
        // --------------------------------------------------------------------
        // FILE NAME OVERRIDE
        //
        // Concerns:
        // 1. The file name logged can be overridden with the
        //    `BSLS_ASSERTIMPUTIL_FILE` macro.
        //
        // Plan:
        // 1. Only test the macros that are enabled in the current build level.
        //
        // 2. Test that the macros by default log the current `__FILE__`.
        //
        // 3. Test that the `BSLS_ASSERTIMPUTIL_FILE` can be changed and all
        //    enabled macros log the new value.
        //
        // 4. Test that reverting back to `BSLS_ASSERTIMPUTIL_DEFAULTFILE`
        //    returns to the original behavior.
        //
        // Testing:
        //   CONCERN: `BSLS_ASSERTIMPUTIL_FILE` interaction
        // --------------------------------------------------------------------

        if (verbose) printf( "\nFILE NAME OVERRIDE"
                             "\n==================\n" );

        if (verbose) printf( "\nInstall `testDriverHandler` "
                             "review-handler.\n" );

        bsls::Review::setViolationHandler(&testDriverHandler);
        ASSERT(::testDriverHandler == bsls::Review::violationHandler());

        if (veryVerbose) printf( "\tSet up expected file names. \n" );

        const char *file = __FILE__;
        const char *altf = "injected_file.t.cpp";

#ifndef BDE_BUILD_TARGET_EXC
        globalReturnOnTestReview = true;
#endif

        if (verbose) printf(
            "\tVerify that the correct file is logged by default.\n");

#ifdef BSLS_REVIEW_SAFE_IS_ACTIVE
        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_SAFE(false);
        REVIEW_TEST_END
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
#endif

#ifdef BSLS_REVIEW_IS_ACTIVE
        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW(false);
        REVIEW_TEST_END
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
#endif

#ifdef BSLS_REVIEW_OPT_IS_ACTIVE
        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT(false);
        REVIEW_TEST_END
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
#endif

        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_INVOKE("false");
        REVIEW_TEST_END
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));

        if (veryVerbose) printf(
            "\tRedefine `BSLS_ASSERTIMPUTIL_FILE`.\n");

#undef BSLS_ASSERTIMPUTIL_FILE
#define BSLS_ASSERTIMPUTIL_FILE altf

        if (veryVerbose) printf(
            "\tVerify that the alternate file text is logged by "
            "enabled macros.\n");

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_SAFE(false);
        REVIEW_TEST_END
        LOOP2_ASSERT(altf, globalFile,    0 == std::strcmp(altf, globalFile));
#endif

#ifdef BSLS_ASSERT_IS_ACTIVE
        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW(false);
        REVIEW_TEST_END
        LOOP2_ASSERT(altf, globalFile,    0 == std::strcmp(altf, globalFile));
#endif

#ifdef BSLS_ASSERT_OPT_IS_ACTIVE
        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT(false);
        REVIEW_TEST_END
        LOOP2_ASSERT(altf, globalFile,    0 == std::strcmp(altf, globalFile));
#endif

        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_INVOKE("false");
        REVIEW_TEST_END
        LOOP2_ASSERT(altf, globalFile,    0 == std::strcmp(altf, globalFile));

        if (veryVerbose) printf(
            "\tRevert `BSLS_ASSERTIMPUTIL_FILE` to "
            "`BSLS_ASSERTIMPUTIL_DEFAULTFILE`.\n");

#undef BSLS_ASSERTIMPUTIL_FILE
#define BSLS_ASSERTIMPUTIL_FILE BSLS_ASSERTIMPUTIL_DEFAULTFILE

        if (verbose) printf(
            "\tVerify that the correct file is logged again.\n");

#ifdef BSLS_REVIEW_SAFE_IS_ACTIVE
        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_SAFE(false);
        REVIEW_TEST_END
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
#endif

#ifdef BSLS_REVIEW_IS_ACTIVE
        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW(false);
        REVIEW_TEST_END
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
#endif

#ifdef BSLS_REVIEW_OPT_IS_ACTIVE
        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT(false);
        REVIEW_TEST_END
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
#endif

        globalReset();
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_INVOKE("false");
        REVIEW_TEST_END
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
}

void test_case_8() {
        // --------------------------------------------------------------------
        // DEFAULT HANDLER LOG: BACKOFF
        //
        // Concerns:
        // 1. The default (failByLog) handler should back off exponentially.
        //
        // Plan:
        // 1. Covered by test case 7 and -4
        //
        // Testing:
        //    CONCERN: default handler log: backoff
        // --------------------------------------------------------------------

        if (verbose) printf( "\nDEFAULT HANDLER LOG: BACKOFF"
                             "\n============================\n" );

        // low values of the backoff skipping are checked by case 7, high
        // values and the periodic repeating at 2^29 are checked by case -4.
}

void test_case_7() {
        // --------------------------------------------------------------------
        // DEFAULT HANDLER LOG: CONTENT
        //
        // Concerns:
        // 1. `failByLog` should log log a message via `bsls_log` containing
        //    call site information.
        //
        // 2. The log message should identify the file and line where the
        //    failed review occurred.
        //
        // 3. The log message should be severity `error`.  // CHECK
        //
        // Plan:
        // 1. set the log message handler to recordingLogMessageHandler
        //
        // 2. invoke the default log handler in a loop and verify the contents
        //    of message, verify that the message is skipped on the 3rd
        //    invocation.
        //
        // Testing:
        //   CONCERN: default handler log: content
        // --------------------------------------------------------------------

        if (verbose) printf( "\nDEFAULT HANDLER LOG: CONTENT"
                             "\n============================\n" );

        bsls::ReviewFailureHandlerGuard guard(bsls::Review::failByLog);
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
                ASSERTV( line, profile.d_line, line == profile.d_line );

                const char *file = __FILE__;

                ASSERTV(file, profile.d_file,
                        0 == std::strcmp(file, profile.d_file));

                char shorttext[32];
                snprintf(shorttext,
                         sizeof shorttext,
                         "(bsls_review.t.cpp:%d",
                         line);
                ASSERTV(shorttext, profile.d_text,
                        NULL != std::strstr(profile.d_text,shorttext));

                if (skipped > 0) {
                    char skipText[32];
                    snprintf(skipText, sizeof skipText, "skipped:%d", skipped);

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
}

void test_case_6() {
        // --------------------------------------------------------------------
        // CONFIGURATION MACROS
        //
        // Concerns:
        // 1. The configuration macros that report on which review facilities
        //    are available in the current build mode might not report
        //    correctly in all build modes.
        //
        // Plan:
        // 1. Subvert the regular include guards to verify that inclusion of
        //    `bsls_review.h` defines the proper macros when included with
        //    varying build modes.
        //
        // Testing:
        //   BSLS_REVIEW_SAFE_IS_ACTIVE
        //   BSLS_REVIEW_IS_ACTIVE
        //   BSLS_REVIEW_OPT_IS_ACTIVE
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCONFIGURATION MACROS"
                             "\n====================\n" );

        TestConfigurationMacros();
}

void test_case_5() {
        // --------------------------------------------------------------------
        // FAILURE HANDLER GUARD
        //
        // Concerns:
        // 1. That the guard swaps new for current handler at construction.
        //
        // 2. Restores the original one at destruction.
        //
        // 3. That guards can nest.
        //
        // 4. That `lockReviewAdministration` has no effect on guard.
        //
        // 5. This applies equally to both the review and backoff handlers.
        //
        // Plan:
        // 1. Create a guard, passing it the `testDriverHandler` handler, and
        //    verify, using `failureHandler`, that this new handler was
        //    installed.  Then lock the administration, and repeat in nested
        //    fashion with the `failBySleep` handler.  Verify restoration on
        //    the way out.
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

        ASSERT(bsls::Review::failByLog == bsls::Review::violationHandler());

        if (verbose) printf( "\nCreate guard with `testDriverHandler` "
                             "handler.\n" );
        {
            bsls::ReviewFailureHandlerGuard guard(::testDriverHandler);

            if (verbose) printf( "\nVerify new review handler.\n" );

            ASSERT(::testDriverHandler == bsls::Review::violationHandler());

            if (verbose) printf( "\nLock administration.\n" );

            bsls::Review::lockReviewAdministration();

            if (verbose) printf( "\nRe-verify new review handler.\n" );

            ASSERT(testDriverHandler == bsls::Review::violationHandler());

            if (verbose) printf( "\nCreate second guard with `failBySleep` "
                                 "handler.\n");

            {
                bsls::ReviewFailureHandlerGuard guard(
                                                    bsls::Review::failBySleep);

                if (verbose) printf( "\nVerify newer review handler.\n" );

                ASSERT(bsls::Review::failBySleep ==
                                             bsls::Review::violationHandler());

                if (verbose) printf( "\nDestroy guard created with "
                                     "`::failBySleep` handler.\n" );
            }

            if (verbose) printf( "\nVerify new review handler.\n" );

            ASSERT(::testDriverHandler == bsls::Review::violationHandler());

            if (verbose) printf( "\nDestroy guard created with "
                                 "`::testDriverHandler` handler.\n" );
        }

        if (verbose) printf( "\nVerify initial review handler.\n" );

        ASSERT(bsls::Review::failByLog == bsls::Review::violationHandler());
}

void test_case_4() {
        // --------------------------------------------------------------------
        // REVIEW FAILURE HANDLERS
        //
        // Concerns:
        // 1. That each of the review failure handlers provided herein behaves
        //    as advertised and (at least) matches the signature of the
        //    `bsls::Review::ViolationHandler` `typedef`
        //
        // Plan:
        // 1. Verify each handler's behavior.  Unfortunately, we cannot test
        //    functions that abort except by hand (see negative test cases).
        //
        // 2. Assign each handler function to a pointer of type `Handler`.
        //
        // Testing:
        //   typedef void (*Handler)(const char *, const char *, int);
        //   static void failByLog(const ReviewViolation& violation);
        //   static void failByAbort(const ReviewViolation& violation);
        //   static void failByThrow(const ReviewViolation& violation);
        //   static void failBySleep(const ReviewViolation& violation);
        // --------------------------------------------------------------------

        if (verbose) printf( "\nREVIEW FAILURE HANDLERS"
                             "\n=======================\n" );

        if (verbose) printf( "\nTesting 'void failByLog(const char *t, "
                             "const char *f, int line);'\n" );
        {
            bsls::Review::ViolationHandler f = bsls::Review::failByLog;
            (void) f;

            if (veryVerbose) {
                printf( "\t(Logging behavior must be tested by "
                        "other cases.)\n" );
            }
        }

        if (verbose) printf( "\nTesting 'void failByAbort(const char *t, "
                             "const char *f, int line);'\n" );
        {
            bsls::Review::ViolationHandler f = bsls::Review::failByAbort;
            (void) f;

            if (veryVerbose) {
                printf( "\t(Aborting behavior must be tested by hand.)\n" );
            }
        }

        if (verbose) printf( "\nTesting 'void failByThrow(const char *t, "
                             "const char *f, int line);'\n" );
        {
            bsls::Review::ViolationHandler f = bsls::Review::failByThrow;
            (void)f;

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
}

void test_case_3() {
        // --------------------------------------------------------------------
        // INCOMPATIBLE BUILD TARGETS
        //
        // Concerns:
        // 1. Any component including `bsls_review.h` that has multiple
        //    review-mode flags defined should fail to compile and provide a
        //    useful diagnostic.
        //
        // Plan:
        // 1. Repeat the reviews at runtime, and fail if any two macros
        //    incompatible macros are present.
        //
        // 2. (Manually) observe that the test is in the bsls_review.h file.
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
}

void test_case_2() {
        // --------------------------------------------------------------------
        // (BSLS) "REVIEW"-MACRO TEST
        //
        // Concerns:
        // 1. Each review macro instantiates only when the appropriate build
        //    mode is set.
        //
        // 2. When instantiated, each macro fires only on "0" valued
        //    expressions
        //
        // 3. When a macro fires, the correct text, line, and file are
        //    transmitted to the current review-failure handler.
        //
        // 4. That the expression text that is printed is exactly what is in
        //    the parentheses of the macro.
        //
        // Plan:
        // 1. We must not try to change build targets (or review modes), but
        //    just observe them to see which review macros should be
        //    instantiated.
        //
        // 2. When enabled, we need to try each of the macros on each of the
        //    (four) kinds of expression text arguments to make sure that it
        //    fires only on `false` and `(void*)(0)` and not `true` or
        //    `(void*)(1)`.
        //
        // 3. In each case for 2.  (above) that fires, we will observe that the
        //    expression text, file name, and line number are correct.
        //
        // 4. Make sure that we vary the text in the expression (and include
        //    embedded whitespace (we don't care about leading or trailing
        //    whitespace).
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

        if (verbose) printf( "\nInstall `testDriverHandler` "
                             "review-handler.\n" );

        bsls::Review::setViolationHandler(&testDriverHandler);
        ASSERT(::testDriverHandler == bsls::Review::violationHandler());

        if (veryVerbose) printf( "\tSet up all but line numbers now. \n" );

        const void *p     = 0;
        const char *istr  = "0";
        const char *pstr  = "p";
        const char *estr  = "false == true";
        const char *file  = __FILE__;
        const char *level = bsls::Review::k_LEVEL_SAFE;
        int         line  = -1;
        int         count = 1; // always first review call for all of these
                                 // invocations.

        (void)p;
        (void)istr;
        (void)estr;

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

#if defined(BSLS_REVIEW_LEVEL_REVIEW_SAFE)                                    \
 || !IS_BSLS_REVIEW_MODE_FLAG_DEFINED && (                                    \
        defined(BDE_BUILD_TARGET_SAFE_2) ||                                   \
        defined(BDE_BUILD_TARGET_SAFE)   )

        if (verbose) printf( "\nEnabled: REVIEW_SAFE, REVIEW, REVIEW_OPT\n" );

        if (veryVerbose) printf( "\tCheck for integer expression.\n" );

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_SAFE : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_SAFE(0);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(istr,  globalText,  0 == std::strcmp(istr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_REVIEW : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW     (0);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(istr,  globalText,  0 == std::strcmp(istr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_OPT : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (0);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(istr,  globalText,  0 == std::strcmp(istr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        if (veryVerbose) printf( "\tCheck for pointer expression.\n" );

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_SAFE : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_SAFE(p);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(pstr,  globalText,  0 == std::strcmp(pstr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_REVIEW : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW     (p);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(pstr,  globalText,  0 == std::strcmp(pstr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_OPT : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (p);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(pstr,  globalText,  0 == std::strcmp(pstr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        if (veryVerbose) printf( "\tCheck for expression with spaces.\n" );

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_SAFE : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_SAFE(false == true);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(estr,  globalText,  0 == std::strcmp(estr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_REVIEW : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW     (false == true);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(estr,  globalText,  0 == std::strcmp(estr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_OPT : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (false == true);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(estr,  globalText,  0 == std::strcmp(estr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);
#endif

        //_____________________________________________________________________
        //                    BSLS_REVIEW, BSLS_REVIEW_OPT
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if defined(BSLS_REVIEW_LEVEL_REVIEW)                                         \
 || !IS_BSLS_REVIEW_MODE_FLAG_DEFINED &&                                      \
        !defined(BDE_BUILD_TARGET_OPT) &&                                     \
        !defined(BDE_BUILD_TARGET_SAFE) &&                                    \
        !defined(BDE_BUILD_TARGET_SAFE_2)

        if (verbose) printf( "\nEnabled: REVIEW, REVIEW_OPT\n" );

        if (veryVerbose) printf( "\tCheck for integer expression.\n" );

        globalReset(); BSLS_REVIEW_SAFE(0); ASSERT(0 == globalReviewFiredFlag);

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_REVIEW : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW     (0);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(istr,  globalText,  0 == std::strcmp(istr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_OPT : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (0);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(istr,  globalText,  0 == std::strcmp(istr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        if (veryVerbose) printf( "\tCheck for pointer expression.\n" );

        globalReset(); BSLS_REVIEW_SAFE(p); ASSERT(0 == globalReviewFiredFlag);

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_REVIEW : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW     (p);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(pstr,  globalText,  0 == std::strcmp(pstr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_OPT : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (p);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(pstr,  globalText,  0 == std::strcmp(pstr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        if (veryVerbose) printf( "\tCheck for expression with spaces.\n" );

        globalReset(); BSLS_REVIEW_SAFE(false == true);
        ASSERT(0 == globalReviewFiredFlag);

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_REVIEW : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW     (false == true);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(estr,  globalText,  0 == std::strcmp(estr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_OPT : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (false == true);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(estr,  globalText,  0 == std::strcmp(estr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);
#endif

        //_____________________________________________________________________
        //                         BSLS_REVIEW_OPT
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if defined(BSLS_REVIEW_LEVEL_REVIEW_OPT)                                     \
 || !IS_BSLS_REVIEW_MODE_FLAG_DEFINED &&                                      \
        defined(BDE_BUILD_TARGET_OPT) &&                                      \
        !defined(BDE_BUILD_TARGET_SAFE) &&                                    \
        !defined(BDE_BUILD_TARGET_SAFE_2)

        if (verbose) printf( "\nEnabled: REVIEW_OPT\n" );

        if (veryVerbose) printf( "\tCheck for integer expression.\n" );

        globalReset(); BSLS_REVIEW_SAFE(0); ASSERT(0 == globalReviewFiredFlag);
        globalReset(); BSLS_REVIEW     (0); ASSERT(0 == globalReviewFiredFlag);

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_OPT : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (0);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(istr,  globalText,  0 == std::strcmp(istr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        if (veryVerbose) printf( "\tCheck for pointer expression.\n" );

        globalReset(); BSLS_REVIEW_SAFE(p); ASSERT(0 == globalReviewFiredFlag);
        globalReset(); BSLS_REVIEW     (p); ASSERT(0 == globalReviewFiredFlag);

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_OPT : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (p);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(pstr,  globalText,  0 == std::strcmp(pstr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);

        if (veryVerbose) printf( "\tCheck for expression with spaces.\n" );

        globalReset(); BSLS_REVIEW_SAFE(false == true);
        ASSERT(0 == globalReviewFiredFlag);

        globalReset(); BSLS_REVIEW     (false == true);
        ASSERT(0 == globalReviewFiredFlag);

        globalReset();
        level = checkLevels ? bsls::Review::k_LEVEL_OPT : "";
        line = L_ + 2;
        REVIEW_TEST_BEGIN
        BSLS_REVIEW_OPT (false == true);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(estr,  globalText,  0 == std::strcmp(estr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
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
        BSLS_REVIEW_INVOKE(pstr);
        REVIEW_TEST_END
        ASSERT(1 == globalReviewFiredFlag);
        LOOP2_ASSERT(pstr,  globalText,  0 == std::strcmp(pstr, globalText));
        LOOP2_ASSERT(file,  globalFile,  0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine,  line == globalLine);
        LOOP2_ASSERT(count, globalCount, count == globalCount);
}

void test_case_1() {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. Want to observe the basic operation of this component in "default
        //    mode" (i.e., with no build flags specified).
        //
        // Plan:
        // 1. Call `setReviewHandler` to install the `testDriverHandler`
        //    "review" function in order to observe that the installed function
        //    was called using the `invokeHandler` method -- and, contingently,
        //    the `BSLS_REVIEW_OPT(X)` macro -- with various arguments.
        //
        // Testing:
        //   BREATHING TEST
        //   CONCERN: By default, the `bsls::Review::falLog` is used.
        //   ReviewViolation::ReviewViolation(...);
        //   const char *ReviewViolation::comment();
        //   int ReviewViolation::count();
        //   const char *ReviewViolation::fileName();
        //   int ReviewViolation::lineNumber();
        //   const char *ReviewViolation::reviewLevel();
        //   static void setViolationHandler(ViolationHandler function);
        //   static bsls::Review::ViolationHandler violationHandler();
        //   static void invokeHandler(const bsls::ReviewViolation&);
        //   static int updateCount(Count *count);
        //   static void lockReviewAdministration();
        //   CONCERN: By default, the `bsls_review::failByAbort` is used
        // --------------------------------------------------------------------

        if (verbose) printf( "\nBREATHING TEST"
                             "\n==============\n" );

        if (verbose) printf( "\nVerify that the correct review callback is "
                             "installed by default.\n" );


        ASSERT(bsls::Review::failByLog == bsls::Review::violationHandler());

        if (verbose) printf( "\nVerify that we can install a new review "
                             "callback.\n" );

        bsls::Review::setViolationHandler(&testDriverHandler);
        ASSERT(::testDriverHandler == bsls::Review::violationHandler());

        if (verbose) printf( "\nVerify that `invokeHandler` properly "
                             "transmits its arguments.\n" );

        globalReset();
        ASSERT(false        == globalReviewFiredFlag);


        if (verbose) printf( "\nVerify that `updateCount` properly"
                             "increments a count.\n" );
        {
            bsls::Review::Count count = {17};

            bsls::Review::updateCount(&count);
            ASSERT( 18 == bsls::AtomicOperations::getInt(&count) );
        }


        if (verbose) printf( "\nVerify that `updateCount` rolls over"
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

        if (verbose) printf( "\nVerify that `lockReviewAdministration` blocks "
                             "callback changes.\n" );

        bsls::Review::lockReviewAdministration();

        bsls::Review::setViolationHandler(&bsls::Review::failByAbort);
        ASSERT(::testDriverHandler == bsls::Review::violationHandler());

#ifdef BSLS_REVIEW_LEVEL_NONE
        if (verbose) printf( "\n'BSLS_REVIEW_LEVEL_NONE' is defined; exit "
                             "breathing test.\n" );
        return;
#endif
        if (verbose) printf( "\nVerify that `BSLS_REVIEW_OPT` doesn't fire "
                             "for `!0` expressions.\n" );

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

        if (verbose) printf( "\nVerify that `BSLS_REVIEW_OPT` does fire for "
                             "`0` expressions.\n" );

#ifndef BDE_BUILD_TARGET_EXC
            globalReturnOnTestReview = true;
#endif

        if (veryVerbose) printf( "\tInteger-valued expression\n" );
        {
            globalReset();
            ASSERT(false == globalReviewFiredFlag);

            const char *text  = "true == false";
            const char *file  = __FILE__;
            int         line  = -1;
            const char *level = checkLevels ? bsls::Review::k_LEVEL_OPT : "";
            int         count = 1;

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

            const char *text  = "(void *)(0)";
            const char *file  = __FILE__;
            int         line  = -1;
            const char *level = checkLevels ? bsls::Review::k_LEVEL_OPT : "";
            int         count = 1;

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

            const char *text  = "true == false";
            const char *file  = __FILE__;
            int         line  = -1;
            const char *level = checkLevels ? bsls::Review::k_LEVEL_OPT : "";
            int         count = c;

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
}

void test_case_m1() {
        // --------------------------------------------------------------------
        // CALL FAIL ABORT HANDLER
        //
        // Concerns:
        // 1. That it does abort the program.
        //
        // 2. That it prints a message to `stderr`.
        //
        // Plan:
        // 1. Call `bsls::Review::failByAbort` after blocking the signal.
        //
        // Testing:
        //   CONCERN: `bsls::Review::failByAbort` aborts
        //   CONCERN: `bsls::Review::failByAbort` prints to `stderr`
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
        bsls::Review::failByAbort(violation);

        ASSERT(0 && "Should not be reached");
}

void test_case_m2() {
        // --------------------------------------------------------------------
        // CALL FAIL THROW HANDLER
        //
        // Concerns:
        // 1. That it does *not* throw for an exception build when there is an
        //    exception pending.
        //
        // 2. That it behaves as `failByAbort` for non-exception builds.
        //
        // Plan:
        // 1. Call `bsls::Review::failByThrow` from within the destructor of a
        //    test object on the stack after a throw.
        //
        // 2. Call `bsls::Review::failByAbort` after blocking the signal.
        //
        // Testing:
        //   static void failByAbort(const ReviewViolation& violation);
        //   CONCERN: `bsls::Review::failByThrow` aborts in non-exception build
        //   CONCERN: `bsls::Review::failByThrow` prints to `stderr` w/o EXC
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCALL FAIL THROW HANDLER"
                             "\n=======================\n" );

#ifdef BDE_BUILD_TARGET_EXC
        printf( "\nEXCEPTION BUILD\n" );

        fprintf( stderr, "\nTHE FOLLOWING SHOULD PRINT ON STDERR:\n"
                "BSLS_REVIEW: An uncaught exception is pending;"
                " cannot throw `bsls_asserttestexception`.\n" );
        fprintf( stderr, "review failed: `failByThrow` handler called "
                 "from ~BadBoy, file f.c, line 9\n" );

        try {
            BadBoy bad;     // calls `bsls::Review::failByThrow` on destruction

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
        bsls::Review::failByAbort(violation);

        ASSERT(0 && "Should not be reached");
#endif
}

void test_case_m3() {
        // --------------------------------------------------------------------
        // CALL FAIL SLEEP HANDLER
        //
        // Concerns:
        // 1. That it does sleep forever.
        //
        // 2. That it prints a message to `stderr`.
        //
        // Plan:
        // 1. Call `bsls::Review::failBySleep`.  Then observe that a diagnostic
        //    is printed to `stderr` and the program hangs.
        //
        // Testing:
        //   static void failBySleep(const ReviewViolation& violation);
        //   CONCERN: `bsls::Review::failBySleep` sleeps forever
        //   CONCERN: `bsls::Review::failBySleep` prints to `stderr`
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCALL FAIL SLEEP HANDLER"
                             "\n=======================\n" );

        fprintf( stderr, "THE FOLLOWING SHOULD PRINT ON STDERR (BEFORE "
                 "HANGING):\n"
                 "Review failed: 0 != 0, file myfile.cpp, line 123\n" );

        bsls::ReviewViolation violation("0 != 0", "myfile.cpp", 123, "TST", 1);
        bsls::Review::failBySleep(violation);

        ASSERT(0 && "Should not be reached");
}

void test_case_m4() {
        // --------------------------------------------------------------------
        // DEFAULT HANDLER LOG: LIMITS
        //
        // Concerns:
        // 1. Log messages should stabilize at a period of 2^29.
        //
        // Plan:
        // 1. Invoke a review with the default (`bsls::Review::failByLog`)
        //    handler repeatedly, checking that skipped count of 2^29 repeats
        //
        // 2. This is the same as test case 7 with MUCH higher total count.
        //
        // Testing:
        //   CONCERN: default handler log: limits
        // --------------------------------------------------------------------

        if (verbose) printf( "\nDEFAULT HANDLER LOG: LIMITS"
                             "\n===========================\n" );

        bsls::ReviewFailureHandlerGuard guard(bsls::Review::failByLog);
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
                    snprintf(skipText, sizeof skipText, "skipped:%d", skipped);

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
}

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
                 verbose = argc > 2;
             veryVerbose = argc > 3;
         veryVeryVerbose = argc > 4;

    printf( "TEST %s CASE %d\n", __FILE__, test);

    switch (test) {
    case 0:  // zero is always the leading case
    case 20: test_case_20(); break;
    case 19: test_case_19(); break;
    case 18: test_case_18(); break;
    case 17: test_case_17(); break;
    case 16: test_case_16(); break;
    case 15: test_case_15(); break;
    case 14: test_case_14(); break;
    case 13: test_case_13(); break;
    case 12: test_case_12(); break;
    case 11: test_case_11(); break;
    case 10: test_case_10(); break;
    case 9: test_case_9(); break;
    case 8: test_case_8(); break;
    case 7: test_case_7(); break;
    case 6: test_case_6(); break;
    case 5: test_case_5(); break;
    case 4: test_case_4(); break;
    case 3: test_case_3(); break;
    case 2: test_case_2(); break;
    case 1: test_case_1(); break;
    case -1: test_case_m1(); break;
    case -2: test_case_m2(); break;
    case -3: test_case_m3(); break;
    case -4: test_case_m4(); break;
    default: {
        fprintf( stderr, "WARNING: CASE `%d' NOT FOUND.\n" , test);
        testStatus = -1;
    } break;
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
// We want to test the following 6 macros:
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
// `BSLS_REVIEW` macros if the review macro has not been set, so we can be sure
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
// detail macros in the `bsls_review.h` header file, and re-include that
// header.  The supporting component `bsls_review_macroreset` provides a header
// that will undefine all of the public macros from `bsls_review.h` and prepare
// us to re-include it after changing the above build and assert level macros.
// Note that `#include`ing a header inside a function definition, as we do
// below, will flag an error for any construct that is not supported inside a
// function definition, such as declaring a template or defining a "local"
// function.  consequently, we must provide a deeper "include-guard" inside the
// component header itself to protect the non-macro parts of this component
// against the repeated inclusion.
//
// For each test iteration that `#include <bsls_review.h>`, each of the macros
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
// review macros, flagging an `X` when that combination should be supported.
// If a review macro should be enabled, this should be detectable by the
// definition of the corresponding `BSLS_REVIEW_*_IS_ACTIVE` macro.  Likewise,
// those macros should not be defined unless the configuration is marked.
//
// The configurations that define a BSLS_REVIEW_LEVEL_* macro are also
// duplicated with the corresponding BSLS_ASSERT_LEVEL_* set, expecting the
// same output.
//
//  Expected test results
//    BDE_BUILD mode       review             REVIEW macros
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
//  assertion          review          REVIEW macros
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
/// This struct contains a static function suitable for registration as a
/// review handler, and provides a distinct "empty" type that may be thrown
/// from the handler and caught within the test cases below, in order to
/// confirm if the appropriate `BSLS_REVIEW_*` macros are enabled or not.
struct ReviewFailed {
    static void failMacroTest(const bsls::ReviewViolation&) {
        throw ReviewFailed();
    }
};
#else
    // Without exception support, we cannot fail a review-test by throwing an
    // exception.  The most practical solution is to simply not compile those
    // tests, so we do not supply an `ReviewFailed` alternative, to be sure to
    // catch any compile-time use of this structure in exception-free builds.
#endif
}  // close unnamed namespace

void TestConfigurationMacros()
{

    if (verbose) printf( "\nCONFIGURATION MACROS"
                               "\n====================\n" );

#if !defined(BDE_BUILD_TARGET_EXC)
    if (verbose)
        printf( "\nThis case is not run as it "
                "relies on exception support.\n" );
#else
    if (verbose) printf( "\nWe need to write a running commentary\n" );

    bsls::Review::setViolationHandler(&ReviewFailed::failMacroTest);
//--------------------------------------------------------------------GENERATOR
// The following script generates tests that are included in this function for
// most/all of the various build configurations that we want to verify.
// ```
//  #!/usr/bin/env python
//
//  table1 = """
//    BDE_BUILD mode       review             REVIEW macros
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
//  """
//
//  table2 = """
//  assertion             review          REVIEW macros
//    level                level           OPT     SAFE
//  -----------------    ----------------- --- --- ----
//  LEVEL_ASSUME_OPT     LEVEL_NONE
//  LEVEL_ASSUME_ASSERT  LEVEL_NONE
//  LEVEL_ASSUME_SAFE    LEVEL_NONE
//  LEVEL_NONE           LEVEL_NONE
//  LEVEL_ASSERT_OPT     LEVEL_NONE
//  LEVEL_ASSERT         LEVEL_NONE
//  LEVEL_ASSERT_SAFE    LEVEL_NONE
//  LEVEL_NONE|ASSUME*   LEVEL_REVIEW_OPT   X
//  LEVEL_ASSERT_OPT     LEVEL_REVIEW_OPT   X
//  LEVEL_ASSERT         LEVEL_REVIEW_OPT   X
//  LEVEL_ASSERT_SAFE    LEVEL_REVIEW_OPT   X
//  LEVEL_NONE|ASSUME*   LEVEL_REVIEW       X   X
//  LEVEL_ASSERT_OPT     LEVEL_REVIEW       X   X
//  LEVEL_ASSERT         LEVEL_REVIEW       X   X
//  LEVEL_ASSERT_SAFE    LEVEL_REVIEW       X   X
//  LEVEL_NONE|ASSUME*   LEVEL_REVIEW_SAFE  X   X   X
//  LEVEL_ASSERT_OPT     LEVEL_REVIEW_SAFE  X   X   X
//  LEVEL_ASSERT         LEVEL_REVIEW_SAFE  X   X   X
//  LEVEL_ASSERT_SAFE    LEVEL_REVIEW_SAFE  X   X   X
//  """
// ```
//  def printtitle(t):
//      e1 = (73 - len(t)) / 2
//      e2 = 73 - len(t) - e1
//      t1 = "//%s %s %s//" % ("="*e1,t,"="*e2,)
//      print(t1)
//
//  def printcheckreview(rtype,exp):
//      if exp != "X":
//          print("""#if defined(BSLS_REVIEW%s_IS_ACTIVE)
//  #error BSLS_REVIEW%s_IS_ACTIVE should not be defined
//  #endif
//  """ % (rtype,rtype,))
//      else:
//          print("""#if !defined(BSLS_REVIEW%s_IS_ACTIVE)
//  #error BSLS_REVIEW%s_IS_ACTIVE should be defined
//  #endif
//  """ % (rtype,rtype,))
//
//  def printcheckfailures(failing, expected):
//      args = (failing,)
//      if expected == "X":
//          args = args + ( "(false);", "(true); ",)
//      else:
//          args = args + ( "(true); ", "(false);")
//          print("""    try { %-29s ASSERT%s }
//      catch(ReviewFailed)               { ASSERT%s }
//  """ % args)
//
//  def printtest( flags, expected):
//      if not flags:
//          printtitle("(NO BUILD FLAGS SET)")
//      else:
//          showflags = " ".join( [ x.replace("BDE_BUILD_TARGET_","")
//                                   .replace("BSLS_ASSERT_","")
//                                   .replace("BSLS_REVIEW_","")
//                                  for x in flags ] )
//          printtitle(showflags)
//
//      print("""
//  // [1] Reset all configuration macros
//
//  #undef INCLUDED_BSLS_REVIEW_MACRORESET
//  #include <bsls_review_macroreset.h>
//
//  // [2] Define the macros for this test case.
//  """)
//      if not flags:
//          print("    // (THIS LINE INTENTIONALLY LEFT BLANK)")
//      else:
//          for f in flags:
//              print("#define %s" % (f,))
//
//      print("""
//  // [3] Re-include the `bsls_review.h` header.
//
//  #include <bsls_review.h>
//
//  // [4] Test the values of the 3 `IS_ACTIVE` macros.
//  """)
//
//      printcheckreview("_OPT",expected[0])
//      printcheckreview("",expected[1])
//      printcheckreview("_SAFE",expected[2])
//
//      print("// [5] Test that the public review macros have the expected " +
//            "effect.\n")
//
//      printcheckfailures("BSLS_REVIEW_OPT(false);", expected[0])
//      printcheckfailures("BSLS_REVIEW(false);", expected[1])
//      printcheckfailures("BSLS_REVIEW_SAFE(false);", expected[2])
//      printcheckfailures("BSLS_REVIEW_INVOKE(\"false\");", "X")
//
//  for n,line in enumerate(table1.split("\n")):
//      if n <= 3 or not line.strip(): continue
//
//      flags = []
//      if line[2] == "X":
//          flags.append("BDE_BUILD_TARGET_SAFE_2")
//      if line[7] == "X":
//          flags.append("BDE_BUILD_TARGET_SAFE")
//      if line[11] == "X":
//          flags.append("BDE_BUILD_TARGET_DBG")
//      if line[16] == "X":
//          flags.append("BDE_BUILD_TARGET_OPT")
//      if line[21:41].strip():
//          flags.append("BSLS_REVIEW_%s" % (line[21:41].strip(),))
//      expected = (line[44:45], line[48:49], line[52:53],)
//      printtest(tuple(flags),expected)
//
//  for n,line in enumerate(table2.split("\n")):
//      if n <= 3 or not line.strip(): continue
//
//      alevels = line[0:20].strip()
//      rlevel = line[21:39].strip()
//      if "ASSUME*" in alevels:
//          alevels = ("LEVEL_NONE", "LEVEL_ASSUME_OPT",
//                     "LEVEL_ASSUME_ASSERT", "LEVEL_ASSUME_SAFE",)
//      else:
//          alevels = (alevels,)
//
//      for alevel in alevels:
//          flags = ( "BSLS_ASSERT_%s" % (alevel,),
//                    "BSLS_REVIEW_%s" % (rlevel,) )
//          expected = (line[40:41], line[44:45], line[48:49], )
//          printtest(flags, expected)
// ```
//----------------------------------------------------------------END GENERATOR

//--------------------------------------------------------------------GENERATED
//========================== (NO BUILD FLAGS SET) ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

    // (THIS LINE INTENTIONALLY LEFT BLANK)

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//=================================== OPT ===================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//=================================== DBG ===================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//================================= DBG OPT =================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//================================== SAFE ===================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//================================ SAFE OPT =================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//================================ SAFE DBG =================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//============================== SAFE DBG OPT ===============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//================================= SAFE_2 ==================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//=============================== SAFE_2 OPT ================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//=============================== SAFE_2 DBG ================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//============================= SAFE_2 DBG OPT ==============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//=============================== SAFE_2 SAFE ===============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//============================= SAFE_2 SAFE OPT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//============================= SAFE_2 SAFE DBG =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//=========================== SAFE_2 SAFE DBG OPT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//=============================== LEVEL_NONE ================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//============================= OPT LEVEL_NONE ==============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//============================= DBG LEVEL_NONE ==============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//=========================== DBG OPT LEVEL_NONE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//============================= SAFE LEVEL_NONE =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//=========================== SAFE OPT LEVEL_NONE ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//=========================== SAFE DBG LEVEL_NONE ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//========================= SAFE DBG OPT LEVEL_NONE =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//============================ SAFE_2 LEVEL_NONE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//========================== SAFE_2 OPT LEVEL_NONE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//========================== SAFE_2 DBG LEVEL_NONE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//======================== SAFE_2 DBG OPT LEVEL_NONE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//========================= SAFE_2 SAFE LEVEL_NONE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//======================= SAFE_2 SAFE OPT LEVEL_NONE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//======================= SAFE_2 SAFE DBG LEVEL_NONE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

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

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//============================ LEVEL_REVIEW_OPT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//========================== OPT LEVEL_REVIEW_OPT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//========================== DBG LEVEL_REVIEW_OPT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//======================== DBG OPT LEVEL_REVIEW_OPT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//========================== SAFE LEVEL_REVIEW_OPT ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//======================== SAFE OPT LEVEL_REVIEW_OPT ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//======================== SAFE DBG LEVEL_REVIEW_OPT ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//====================== SAFE DBG OPT LEVEL_REVIEW_OPT ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//========================= SAFE_2 LEVEL_REVIEW_OPT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//======================= SAFE_2 OPT LEVEL_REVIEW_OPT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//======================= SAFE_2 DBG LEVEL_REVIEW_OPT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//===================== SAFE_2 DBG OPT LEVEL_REVIEW_OPT =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//====================== SAFE_2 SAFE LEVEL_REVIEW_OPT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//==================== SAFE_2 SAFE OPT LEVEL_REVIEW_OPT =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//==================== SAFE_2 SAFE DBG LEVEL_REVIEW_OPT =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

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

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//============================== LEVEL_REVIEW ===============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//============================ OPT LEVEL_REVIEW =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//============================ DBG LEVEL_REVIEW =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//========================== DBG OPT LEVEL_REVIEW ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//============================ SAFE LEVEL_REVIEW ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//========================== SAFE OPT LEVEL_REVIEW ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//========================== SAFE DBG LEVEL_REVIEW ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//======================== SAFE DBG OPT LEVEL_REVIEW ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//=========================== SAFE_2 LEVEL_REVIEW ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//========================= SAFE_2 OPT LEVEL_REVIEW =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//========================= SAFE_2 DBG LEVEL_REVIEW =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//======================= SAFE_2 DBG OPT LEVEL_REVIEW =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//======================== SAFE_2 SAFE LEVEL_REVIEW =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//====================== SAFE_2 SAFE OPT LEVEL_REVIEW =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//====================== SAFE_2 SAFE DBG LEVEL_REVIEW =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

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

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//============================ LEVEL_REVIEW_SAFE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//========================== OPT LEVEL_REVIEW_SAFE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//========================== DBG LEVEL_REVIEW_SAFE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//======================== DBG OPT LEVEL_REVIEW_SAFE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//========================= SAFE LEVEL_REVIEW_SAFE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//======================= SAFE OPT LEVEL_REVIEW_SAFE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//======================= SAFE DBG LEVEL_REVIEW_SAFE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//===================== SAFE DBG OPT LEVEL_REVIEW_SAFE ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//======================== SAFE_2 LEVEL_REVIEW_SAFE =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//====================== SAFE_2 OPT LEVEL_REVIEW_SAFE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//====================== SAFE_2 DBG LEVEL_REVIEW_SAFE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//==================== SAFE_2 DBG OPT LEVEL_REVIEW_SAFE =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//====================== SAFE_2 SAFE LEVEL_REVIEW_SAFE ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//==================== SAFE_2 SAFE OPT LEVEL_REVIEW_SAFE ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//==================== SAFE_2 SAFE DBG LEVEL_REVIEW_SAFE ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

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

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//======================= LEVEL_ASSUME_OPT LEVEL_NONE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//===================== LEVEL_ASSUME_ASSERT LEVEL_NONE ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_ASSERT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//====================== LEVEL_ASSUME_SAFE LEVEL_NONE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_SAFE
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//========================== LEVEL_NONE LEVEL_NONE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//======================= LEVEL_ASSERT_OPT LEVEL_NONE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//========================= LEVEL_ASSERT LEVEL_NONE =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//====================== LEVEL_ASSERT_SAFE LEVEL_NONE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//======================= LEVEL_NONE LEVEL_REVIEW_OPT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//==================== LEVEL_ASSUME_OPT LEVEL_REVIEW_OPT ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//================== LEVEL_ASSUME_ASSERT LEVEL_REVIEW_OPT ===================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//=================== LEVEL_ASSUME_SAFE LEVEL_REVIEW_OPT ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//==================== LEVEL_ASSERT_OPT LEVEL_REVIEW_OPT ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//====================== LEVEL_ASSERT LEVEL_REVIEW_OPT ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//=================== LEVEL_ASSERT_SAFE LEVEL_REVIEW_OPT ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//========================= LEVEL_NONE LEVEL_REVIEW =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//====================== LEVEL_ASSUME_OPT LEVEL_REVIEW ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//==================== LEVEL_ASSUME_ASSERT LEVEL_REVIEW =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//===================== LEVEL_ASSUME_SAFE LEVEL_REVIEW ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//====================== LEVEL_ASSERT_OPT LEVEL_REVIEW ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//======================== LEVEL_ASSERT LEVEL_REVIEW ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//===================== LEVEL_ASSERT_SAFE LEVEL_REVIEW ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public review macros have the expected effect.

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

//====================== LEVEL_NONE LEVEL_REVIEW_SAFE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//=================== LEVEL_ASSUME_OPT LEVEL_REVIEW_SAFE ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//================== LEVEL_ASSUME_ASSERT LEVEL_REVIEW_SAFE ==================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//=================== LEVEL_ASSUME_SAFE LEVEL_REVIEW_SAFE ===================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//=================== LEVEL_ASSERT_OPT LEVEL_REVIEW_SAFE ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//===================== LEVEL_ASSERT LEVEL_REVIEW_SAFE ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.

//=================== LEVEL_ASSERT_SAFE LEVEL_REVIEW_SAFE ===================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_REVIEW_MACRORESET
#include <bsls_review_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the `bsls_review.h` header.

#include <bsls_review.h>

// [4] Test the values of the 3 `IS_ACTIVE` macros.

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public review macros have the expected effect.
//----------------------------------------------------------------END GENERATED
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
