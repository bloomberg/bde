// bsls_assert.t.cpp                                                  -*-C++-*-
#include <bsls_assert.h>

#include <bsls_asserttestexception.h>
#include <bsls_bsltestutil.h>
#include <bsls_keyword.h>
#include <bsls_log.h>
#include <bsls_logseverity.h>
#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_types.h>

// Include 'cassert' to make sure no macros conflict between 'bsls_assert.h'
// and 'cassert'.  This test driver does *not* invoke 'assert(expression)'.
#include <cassert>

#include <cstdio>    // 'fprintf'
#include <cstdlib>   // 'atoi'
#include <cstring>   // 'strcmp', 'strcpy'
#include <exception> // 'exception'

#ifdef BSLS_PLATFORM_OS_UNIX
#include <signal.h>
#endif

#ifdef BSLS_ASSERT_NORETURN
#error BSLS_ASSERT_NORETURN must be a macro scoped locally to this file
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN)
#   define BSLS_ASSERT_NORETURN [[noreturn]]
#elif defined(BSLS_PLATFORM_CMP_MSVC)
#   define BSLS_ASSERT_NORETURN __declspec(noreturn)
#else
#   define BSLS_ASSERT_NORETURN
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
// [ 7] BSLS_ASSERT_SAFE_IS_ACTIVE
// [ 7] BSLS_ASSERT_IS_ACTIVE
// [ 7] BSLS_ASSERT_OPT_IS_ACTIVE
// [ 2] BSLS_ASSERT_SAFE(X)
// [ 2] BSLS_ASSERT(X)
// [ 2] BSLS_ASSERT_OPT(X)
// [ 2] BSLS_ASSERT_INVOKE(X)
// [ 4] typedef void (*ViolationHandler)(const AssertViolation&);
// [ 1] AssertViolation::AssertViolation(...);
// [ 1] const char *AssertViolation::comment();
// [ 1] const char *AssertViolation::fileName();
// [ 1] int AssertViolation::lineNumber();
// [ 1] const char *AssertViolation::assertLevel();
// [ 1] void setViolationHandler(ViolationHandler function);
// [ 1] void lockAssertAdministration();
// [ 1] bsls::Assert::ViolationHandler violationHandler();
// [ 1] void invokeHandler(const char *t, const char *f, int);
// [ 1] void invokeHandler(const AssertViolation&);
// [ 4] void failAbort(const char *, const char *, int);
// [-2] void failAbort(const char *, const char *, int);
// [ 4] void failByAbort(const AssertViolation& violation);
// [-1] void failByAbort(const AssertViolation& violation);
// [ 4] void failSleep(const char *, const char *, int);
// [-6] void failSleep(const char *, const char *, int);
// [ 4] void failBySleep(const AssertViolation& violation);
// [-5] void failBySleep(const AssertViolation& violation);
// [ 4] void failThrow(const char *, const char *, int);
// [-4] void failThrow(const char *, const char *, int);
// [ 4] void failByThrow(const AssertViolation& violation);
// [-3] void failByThrow(const AssertViolation& violation);
// [ 5] void permitOutOfPolicyReturningFailureHandler();
// [-8] void permitOutOfPolicyReturningFailureHandler();
// [ 5] static k_permitOutOfPolicyReturningAssertionBuildKey
// [ 5] bool abortUponReturningAssertionFailureHandler();
// [-8] bool abortUponReturningAssertionFailureHandler();
// [ 6] class bsls::AssertFailureHandlerGuard
// [ 6] AssertFailureHandlerGuard(Handler)
// [ 6] ~AssertFailureHandlerGuard()
// [10] typedef void (*Handler)(const char *, const char *, int);
// [10] void setFailureHandler(Handler function);
// [10] bsls::Assert::Handler failureHandler();
// [10] AssertFailureHandlerGuard(Handler)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] CONCERN: Returning handler log: content
// [ 9] CONCERN: Returning handler log: backoff
// [-7] CONCERN: Returning handler log: limits
//
// [11] USAGE EXAMPLE: Using Assert Macros
// [12] USAGE EXAMPLE: Invoking an assert handler directly
// [13] USAGE EXAMPLE: Using Administration Functions
// [13] USAGE EXAMPLE: Installing Prefabricated Assert-Handlers
// [14] USAGE EXAMPLE: Creating Your Own Assert-Handler
// [15] USAGE EXAMPLE: Using Scoped Guard
// [16] USAGE EXAMPLE: Using "ASSERT" with 'BDE_BUILD_TARGET_SAFE_2'
//
// [ 1] CONCERN: By default, the 'bsls::Assert::failByAbort' is used.
// [ 2] CONCERN: ASSERT macros are instantiated properly for build targets
// [ 2] CONCERN: all combinations of BDE_BUILD_TARGETs are allowed
// [ 2] CONCERN: any one assert mode overrides all BDE_BUILD_TARGETs
// [ 3] CONCERN: ubiquitously detect multiply-defined assertion-mode flags
// [ 6] CONCERN: that locking does not stop the handlerGuard from working
// [-1] CONCERN: 'bsls::Assert::failByAbort' aborts
// [-1] CONCERN: 'bsls::Assert::failByAbort' prints to 'stderr'
// [-2] CONCERN: 'bsls::Assert::failAbort' aborts
// [-2] CONCERN: 'bsls::Assert::failAbort' prints to 'stderr'
// [-3] CONCERN: 'bsls::Assert::failByThrow' aborts in non-exception build
// [-3] CONCERN: 'bsls::Assert::failByThrow' prints to 'stderr' w/o EXC
// [-4] CONCERN: 'bsls::Assert::failThrow' aborts in non-exception build
// [-4] CONCERN: 'bsls::Assert::failThrow' prints to 'stderr' w/o EXC
// [-5] CONCERN: 'bsls::Assert::failBySleep' sleeps forever
// [-5] CONCERN: 'bsls::Assert::failBySleep' prints to 'stderr'
// [-6] CONCERN: 'bsls::Assert::failSleep' sleeps forever
// [-6] CONCERN: 'bsls::Assert::failSleep' prints to 'stderr'

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

static bool        globalAssertFiredFlag = false;
static bool        globalLegacyAssertFiredFlag = false;
static const char *globalText = "";
static const char *globalFile = "";
static int         globalLine = -1;
static const char *globalLevel = "";

// BDE_VERIFY pragma: -FE01 // at this level in the package hierarchy in a test
                            // driver we want to avoid std::exception

#ifndef BDE_BUILD_TARGET_EXC
static bool globalReturnOnTestAssert = false;
    // This flag is very dangerous, as it will cause the test-driver assertion
    // handler to simple 'return' by default, exposing any additional function
    // under test to the subsequent undefined behavior.  In general, exception-
    // free builds should avoid executing such tests, rather than set this
    // flag.  However, there is some subset of this test driver that would
    // benefit from being able to invoke this handler in a test mode to be sure
    // that correct behavior occurs in the presence of the various preprocessor
    // checks for exceptions being disabled.  This flag allows for testing such
    // behavior that does not rely on aborting out of the assert handler.
#endif

//=============================================================================
//                  GLOBAL HELPER MACROS FOR TESTING
//-----------------------------------------------------------------------------

#if (defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE) ||                               \
     defined(BSLS_ASSERT_LEVEL_ASSERT)      ||                               \
     defined(BSLS_ASSERT_LEVEL_ASSERT_OPT)  ||                               \
     defined(BSLS_ASSERT_LEVEL_NONE) )
    #define IS_BSLS_ASSERT_MODE_FLAG_DEFINED 1
#else
    #define IS_BSLS_ASSERT_MODE_FLAG_DEFINED 0
#endif

#ifdef BDE_BUILD_TARGET_EXC
#define ASSERTION_TEST_BEGIN                                    \
        try {

#define ASSERTION_TEST_END                                                   \
        } catch (const std::exception&) {                                    \
            if (verbose) printf( "\nException caught." );                    \
        }
#else
#define ASSERTION_TEST_BEGIN
#define ASSERTION_TEST_END
#endif

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#if    !defined(BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER)               \
    || !defined(BSLS_PLATFORM_CMP_MSVC)

// 'invokeHandler' cannot return if we are building on Windows with the
// '...ENABLE_NORETURN...' flag turned on.

struct LogProfile {
    enum { k_TEXT_BUFFER_SIZE = 1024 };

    char                    d_file[k_TEXT_BUFFER_SIZE];
    int                     d_line;
    char                    d_text[k_TEXT_BUFFER_SIZE];
};

struct HandlerReturnTest {
    static LogProfile         s_firstProfile;
    static LogProfile         s_secondProfile;
    static bsls::Types::Int64 s_handlerInvocationCount;
    static bsls::Types::Int64 s_loggerInvocationCount;

    static void clear();
        // Clear 's_firstProfile' and 's_secondProfile', and set
        // 's_handlerInvocationCount' and 's_loggerInvocationCount' to 0.

    static void emptyViolationHandler(const char *text,
                                      const char *file,
                                      int         line);
        // Do nothing with the specified 'text', 'file', and 'line'.

    static void countingViolationHandler(const char *text,
                                         const char *file,
                                         int         line);
        // Increment the 's_handlerInvocationCount' counter, do nothing with
        // the specified 'text', 'file', and 'line'.

    static void recordingLogMessageHandler(bsls::LogSeverity::Enum  severity,
                                           const char              *file,
                                           int                      line,
                                           const char              *message);
        // Register a test failure if the specified 'severity' is not fatal,
        // and store the specified 'file', 'line', and 'message' into the
        // corresponding fields of either 's_firstProfile' (on the first
        // invocation of this function) or 's_secondProfile' (on the second
        // invocation of this function).  Additionally, register a test failure
        // if this function is called more than twice without an intervening
        // call to 'clear'.

    static void countingLogMessageHandler(bsls::LogSeverity::Enum  severity,
                                          const char              *file,
                                          int                      line,
                                          const char              *message);
        // Increment the 's_loggerInvocationCount' counter.  If
        // 'globalVeryVeryVerbose' is 'true' then log the specified 'severity',
        // 'file', 'line', and 'message';
};

LogProfile         HandlerReturnTest::s_firstProfile;
LogProfile         HandlerReturnTest::s_secondProfile;
bsls::Types::Int64 HandlerReturnTest::s_handlerInvocationCount = 0;
bsls::Types::Int64 HandlerReturnTest::s_loggerInvocationCount = 0;

void HandlerReturnTest::clear()
{
    s_firstProfile.d_file[0] = '\0';
    s_firstProfile.d_line = 0;
    s_firstProfile.d_text[0] = '\0';

    s_secondProfile.d_file[0] = '\0';
    s_secondProfile.d_line = 0;
    s_secondProfile.d_text[0] = '\0';

    s_handlerInvocationCount = 0;
    s_loggerInvocationCount = 0;
}

void HandlerReturnTest::emptyViolationHandler(const char *text,
                                              const char *file,
                                              int         line)
{
    (void) text;
    (void) file;
    (void) line;
}

void HandlerReturnTest::countingViolationHandler(const char *text,
                                                 const char *file,
                                                 int         line)
{
    (void) text;
    (void) file;
    (void) line;

    ++s_handlerInvocationCount;
}

void HandlerReturnTest::recordingLogMessageHandler(
                                             bsls::LogSeverity::Enum  severity,
                                             const char              *file,
                                             int                      line,
                                             const char              *message)
{
    ASSERT(bsls::LogSeverity::e_ERROR == severity);
    ASSERT(s_loggerInvocationCount < 2);

    if (globalVeryVeryVerbose) {
        bsls::Log::stdoutMessageHandler(severity, file, line, message);
    }

    if (0 == s_loggerInvocationCount) {
        strncpy(s_firstProfile.d_file,
                file,
                LogProfile::k_TEXT_BUFFER_SIZE);

        s_firstProfile.d_line = line;

        strncpy(s_firstProfile.d_text,
                message,
                LogProfile::k_TEXT_BUFFER_SIZE);
    }
    else {
        strncpy(s_secondProfile.d_file,
                file,
                LogProfile::k_TEXT_BUFFER_SIZE);

        s_secondProfile.d_line = line;

        strncpy(s_secondProfile.d_text,
                message,
                LogProfile::k_TEXT_BUFFER_SIZE);
    }

    ++s_loggerInvocationCount;
}

void HandlerReturnTest::countingLogMessageHandler(
                                             bsls::LogSeverity::Enum  severity,
                                             const char              *file,
                                             int                      line,
                                             const char              *message)
{
    ASSERT(bsls::LogSeverity::e_ERROR == severity);

    ++s_loggerInvocationCount;

    if (globalVeryVeryVerbose) {
        bsls::Log::stdoutMessageHandler(severity, file, line, message);
    }
}

#endif

static void globalReset()
{
    if (globalVeryVeryVerbose)
        printf( "*** globalReset()\n" );

    globalAssertFiredFlag = false;
    globalLegacyAssertFiredFlag = false;
    globalText = "";
    globalFile = "";
    globalLine = -1;
    globalLevel = "";

    ASSERT( !globalAssertFiredFlag );
    ASSERT( !globalLegacyAssertFiredFlag );
    ASSERT( 0 == std::strcmp("", globalText));
    ASSERT( 0 == std::strcmp("", globalFile));
    ASSERT(-1 == globalLine);
    ASSERT( 0 == std::strcmp("", globalLevel));
}

//-----------------------------------------------------------------------------

BSLS_ASSERT_NORETURN
static void testDriverHandler(const bsls::AssertViolation& violation)
    // Set the 'globalAssertFiredFlag' to 'true', the
    // 'globalLegacyAssertFiredFlag' to 'false', and store the expression
    // 'text', 'file' name, 'line' number, and 'assertLlevel' values from the
    // specified 'violation' in 'globalText', 'globalFile', 'globalLine', and
    // 'globalLevel' respectively.  Then throw an 'std::exception' object
    // provided that 'BDE_BUILD_TARGET_EXC' is defined; otherwise, abort the
    // program.
{
    if (globalVeryVeryVerbose) {
        printf( "*** testDriverHandler: ");
        P_(violation.comment());
        P_(violation.fileName());
        P_(violation.lineNumber());
        P(violation.assertLevel());
    }

    globalAssertFiredFlag = true;
    globalLegacyAssertFiredFlag = false;
    globalText = violation.comment();
    globalFile = violation.fileName();
    globalLine = violation.lineNumber();
    globalLevel = violation.assertLevel();

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

BSLS_ASSERT_NORETURN
static void legacyTestDriverHandler(const char *text,
                                    const char *file,
                                    int         line)
    // Set the 'globalAssertFiredFlag' to 'true', the
    // 'globalLegacyAssertFiredFlag' to 'true', the 'globalLevel' to '""', and
    // store the specified expression 'text', 'file' name, and 'line' number in
    // 'globalText', 'globalFile', and 'globalLine'.  Then throw an
    // 'std::exception' object provided that 'BDE_BUILD_TARGET_EXC' is defined;
    // otherwise, abort the program.
{
    if (globalVeryVeryVerbose) {
        printf( "*** legacyTestDriverHandler: "); P_(text) P_(file) P(line)
    }

    globalAssertFiredFlag = true;
    globalLegacyAssertFiredFlag = true;
    globalText = text;
    globalFile = file;
    globalLine = line;
    globalLevel = "";

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

static void returningHandler(const bsls::AssertViolation& violation)
    // Print the 'comment', 'file', 'line' and 'assertLevel' from the specified
    // 'violation' to standard output if 'globalVeryVeryVerbose' is non-zero
    // and return.  NOTE THAT this handler is against Bloomberg default policy
    // and under normal circumstances such a handler will result in termination
    // of the program anyway.
{
    if (globalVeryVeryVerbose) {
        printf( "*** returningHandler: ");
        P_(violation.comment());
        P_(violation.fileName());
        P_(violation.lineNumber());
        P(violation.assertLevel());
    }
}

//-----------------------------------------------------------------------------

BSLS_ASSERT_NORETURN
static void testDriverPrint(const bsls::AssertViolation& violation)
    // Format, in verbose mode, the expression 'comment', 'file' name, 'line'
    // number, and 'assertLevel' from the specified 'violation' in the same way
    // as the 'bsls::Assert::failByAbort' assertion-failure handler function
    // might, but on 'cout' instead of 'cerr'.  Then throw an 'std::exception'
    // object provided that 'BDE_BUILD_TARGET_EXC' is defined; otherwise, abort
    // the program.

{
    if (globalVeryVeryVerbose) {
        printf( "*** testDriverPrint: ");
        P_(violation.comment());
        P_(violation.fileName());
        P_(violation.lineNumber());
        P(violation.assertLevel());
    }

    if (globalVeryVerbose) {
        std::fprintf(stdout,
                     "Assertion failed: %s, file %s, line %d, level: %s\n",
                     violation.comment(),
                     violation.fileName(),
                     violation.lineNumber(),
                     violation.assertLevel() );

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
    // Bogus 'struct' used for testing: calls 'bsls::Assert::failByThrow' on
    // destruction to ensure that it does not re-throw with an exception
    // pending (see case -2).

    BadBoy() {
        if (globalVeryVerbose) printf( "BadBoy Created!\n" );
    }

    ~BadBoy() BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false) {
        if (globalVeryVerbose) printf( "BadBoy Destroyed!\n" );
        bsls::Assert::failByThrow(bsls::AssertViolation(
                                   "'failByThrow' handler called from ~BadBoy",
                                   "f.c",
                                   9,"L"));
     }
};

// Declaration of function that must appear after main in order to test the
// configuration macros.
void TestConfigurationMacros();


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

///Usage Examples
///--------------
// The following examples illustrate (1) when to use each of the three kinds of
// (BSLS) "ASSERT" macros, (2) when and how to call the 'invokeHandler' method
// directly, (3) how to configure, at runtime, the behavior resulting from an
// assertion failure using "off-the-shelf" handler methods, (4) how to create
// your own custom assertion-failure handler function.  (5) proper use of
// 'bsls::AssertFailureHandlerGuard' to install, temporarily, an
// exception-producing assert handler, and (6) how "ASSERT" macros would be
// used in conjunction with portions of the code that are instantiated only
// when 'BDE_BUILD_TARGET_SAFE_2' is defined.
//
///1. Using 'BSLS_ASSERT', 'BSLS_ASSERT_SAFE', and 'BSLS_ASSERT_OPT'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component provides three different variants of (BSLS) "ASSERT" macros.
// This first usage example attempts to illustrate how one might select each of
// the particular variants, based on the runtime cost of the defensive check
// relative to that of the useful work being done.
//
// Use of the 'BSLS_ASSERT_SAFE' macro is often appropriate when the defensive
// check occurs within the body of an inline function.  The 'BSLS_ASSERT_SAFE'
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
// 'BSLS_ASSERT_SAFE' as follows:
//..
    // ...

    inline
    Kpoint::Kpoint(short int x, short int y)
    : d_x(x)
    , d_y(y)
    {
        BSLS_ASSERT_SAFE(-1000 <= x); BSLS_ASSERT_SAFE(x <= 1000);
        BSLS_ASSERT_SAFE(-1000 <= y); BSLS_ASSERT_SAFE(y <= 1000);
    }
//..
// For more substantial (non-'inline') functions, we would be more likely to
// use the 'BSLS_ASSERT' macro because the runtime overhead due to defensive
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
// might choose to implement the defensive check using 'BSLS_ASSERT' as
// follows:
//..
    // my_hashtable.cpp
    // ...

    void HashTable::resize(double loadFactor)
    {
        // silence "unused parameter" warning in release builds:
        (void) loadFactor;
        BSLS_ASSERT(0 < loadFactor);

        // ...
    }
//..
// In some cases, the runtime cost of checking is always negligible when
// compared with the runtime cost of performing the useful work; moreover, the
// consequences of continuing in an undefined state for certain applications
// could be catastrophic.  Instead of using 'BSLS_ASSERT' in such cases, we
// might consider using 'BSLS_ASSERT_OPT'.  For example, suppose we have a
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
// might choose to implement these defensive checks using 'BSLS_ASSERT_OPT' as
// follow:
//..
    // my_tradingsystem.cpp
    // ...

    void TradingSystem::executeTrade(int scalingFactor)
    {
        BSLS_ASSERT_OPT(0 <= scalingFactor);
        BSLS_ASSERT_OPT(0 == scalingFactor % 100);

        // ...
    }
//..
// Notice that in each case, the choice of which of the three (BSLS) "ASSERT"
// macros to use is governed primarily by the relative runtime cost compared
// with that of the useful work being done (and only secondarily by the
// potential consequences of continuing execution in an undefined state).
//
///2. When and How to Call the Invoke-Handler Method Directly
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// There may be times when we choose to invoke the currently installed
// assertion-failure handler directly -- i.e., instead of via one of the three
// (BSLS) "ASSERT" macros provided in this component.  Suppose that we are
// currently in the body of some function 'someFunc' and, for whatever reason,
// feel compelled to invoke the currently installed assertion-failure handler
// based on some criteria other than the current build mode.  The call might
// look as follows:
//..
    void someFunc(bool a, bool b, bool c)
    {
        bool someCondition = a && b && !c;

        if (someCondition) {
            bsls::Assert::invokeHandler("Bad News", __FILE__, __LINE__);
        }
    }
//..
// If presented with invalid arguments, 'someFunc' (above) will produce output
// similar to the following:
//..
//  Assertion failed: Bad News, file bsls_assert.t.cpp, line 365
//  Abort (core dumped)
//..
//
///3. Runtime Configuration of the 'bsls::Assert' Facility
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// By default, any assertion failure will result in the invocation of the
// 'bsls::Assert::failByAbort' handler function.  We can replace this behavior
// with that of one of the other static failure handler methods supplied in
// 'bsls::Assert' as follows.  Let's assume we are at the top of our
// application called 'myMain' (which would typically be 'main'):
//..
    void myMain()
    {
//..
// First observe that the default assertion-failure handler function is, in
// fact, 'bsls::Assert::failByAbort':
//..
        ASSERT(&bsls::Assert::failByAbort == bsls::Assert::violationHandler());
//..
// Next, we install a new assertion-failure handler function,
// 'bsls::Assert::failBySleep', from the suite of "off-the-shelf" handlers
// provided as 'static' methods of 'bsls::Assert':
//..
        bsls::Assert::setViolationHandler(&bsls::Assert::failBySleep);
//..
// Observe that 'bsls::Assert::failBySleep' is the new, currently-installed
// assertion-failure handler:
//..
        ASSERT(&bsls::Assert::failBySleep == bsls::Assert::violationHandler());
//..
// Note that if we were to explicitly invoke the current assertion-failure
// handler as follows:
//..
//  bsls::Assert::invokeHandler("message", "file", 27);  // This will hang!
//..
// the program will hang since 'bsls::Assert::failBySleep' repeatedly sleeps
// for a period of time within an infinite loop.  Thus, this assertion-failure
// handler is useful for hanging a process so that a debugger may be attached
// to it.
//
// We may now decide to disable the 'setViolationHandler' method using the
// 'bsls::Assert::lockAssertAdministration()' method to ensure that no one else
// will override our decision globally.  Note, however, that the
// 'bsls::AssertFailureHandlerGuard' is not affected, and can still be used to
// supplant the currently installed handler (see below):
//..
        bsls::Assert::lockAssertAdministration();
//..
// Attempting to change the currently installed handler now will fail:
//..
        bsls::Assert::setViolationHandler(&bsls::Assert::failByAbort);

        ASSERT(&bsls::Assert::failByAbort != bsls::Assert::violationHandler());

        ASSERT(&bsls::Assert::failBySleep == bsls::Assert::violationHandler());
    }
//..
//
///4. Creating a Custom Assertion Handler
///- - - - - - - - - - - - - - - - - - -
// Sometimes, especially during testing, we may need to write our own custom
// assertion-failure handler function.  The only requirements are that the
// function have the same prototype (i.e., the same respective parameter and
// return types) as the 'bsls::Assert::Handle' 'typedef', and that the function
// should not return (i.e., it must 'abort', 'exit', 'terminate', 'throw', or
// hang).  To illustrate, we will create a 'static' method at file scope that
// conforms to the required structure (notice the explicit use of 'std::printf'
// from '<cstdio>' instead of 'std::cout' from '<iostream>' to avoid
// interaction with the C++ memory allocation layer):
//..
    static bool globalEnableOurPrintingFlag = true;

    BSLS_ASSERT_NORETURN
    static void ourFailureHandler(const bsls::AssertViolation& violation)
        // Print the specified expression 'text', 'file' name, and 'line'
        // number to 'stdout' as a comma-separated list, replacing null
        // string-argument values with empty strings (unless printing has been
        // disabled by the 'globalEnableOurPrintingFlag' variable), then
        // unconditionally abort.
    {
        const char *text = violation.comment();
        if (!text) {
            text = "";
        }
        const char *file = violation.fileName();
        if (!file) {
            file = "";
        }
        int line = violation.lineNumber();
        if (globalEnableOurPrintingFlag) {
            std::printf("%s, %s, %d\n", text, file, line);
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
// pointer of type 'bsls::Assert::ViolationHandler':
//..
        bsls::Assert::ViolationHandler f = &ourFailureHandler;
//..
// Now we can install it just as we would any any other handler:
//..
        bsls::Assert::setViolationHandler(f);
//..
// We can now invoke the default handler directly:
//..
        BSLS_ASSERT_INVOKE("str1");
    }
//..
// With the resulting output as follows:
//..
//  str1, my_file.cpp, 17
//  Abort (core dumped)
//..
//
///5. Using the 'bsls::AssertFailureHandlerGuard'
///- - - - - - - - - - - - - - - - - - - - - - -
// Sometimes we may want to replace, temporarily (i.e., within some local
// lexical scope), the currently installed assertion-failure handler function.
// In particular, we sometimes use the 'bsls::AssertFailureHandlerGuard' class
// to replace the current handler with one that throws an exception (because we
// know that such an exception is safe in the local context).  Let's start with
// the simple factorial function below, which validates, in "debug mode" (or
// "safe mode"), that its input is non-negative:
//..
    double fact(int n)
        // Return 'n!'.  The behavior is undefined unless '0 <= n'.
    {
        BSLS_ASSERT(0 <= n);

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
// The purpose of this function is to allow assertion failures in subroutine
// calls below this function to be handled by throwing an exception, which is
// then caught by the wrapper and reported to the caller as a "bad" status.
// Hence, when within the runtime scope of this function, we want to install,
// temporarily, the assertion-failure handler 'bsls::Assert::failByThrow',
// which, when invoked, causes an 'bsls::AssertTestException' object to be
// thrown.  (Note that we are not advocating this approach for "recovery", but
// rather for an orderly shut-down, or perhaps during testing.)  The
// 'bsls::AssertFailureHandlerGuard' class is provided for just this purpose:
//..
    ASSERT(&bsls::Assert::failByAbort == bsls::Assert::violationHandler());

    bsls::AssertFailureHandlerGuard guard(&bsls::Assert::failByThrow);

    ASSERT(&bsls::Assert::failByThrow == bsls::Assert::violationHandler());
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
                std::printf( "Internal Error: %s, %s, %d\n",
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
//  Internal Error: bsls_assert.t.cpp:500: 0 <= n
//..
// and the 'wrapperFunc' function will return a bad status (i.e., 1) to its
// caller.  Note that if exceptions are not enabled,
// 'bsls::Assert::failByThrow' will behave as 'bsls::Assert::failByAbort', and
// dump core immediately:
//..
// Assertion failed: 0 <= n, file bsls_assert.t.cpp, line 500 Abort (core
// dumped)
//..
// Finally note that the 'bsls::AssertFailureHandlerGuard' is not thread-aware.
// In particular, a guard that is created in one thread will also affect the
// failure handlers that are used in other threads.  Care should be taken when
// using this guard when more than a single thread is executing.
//
/// 6. Using (BSLS) "ASSERT" Macros in Conjunction w/ 'BDE_BUILD_TARGET_SAFE_2'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Recall that assertions do not affect binary compatibility; however, software
// built with 'BDE_BUILD_TARGET_SAFE_2' defined need not be binary compatible
// with software built otherwise.  In this final example, we look at how we
// might use the (BSLS) "ASSERT" family of macro's in conjunction with code
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
            BSLS_ASSERT_SAFE(this == position.d_parent_p);  // "safe 2 mode"
    #endif
            *position.d_current_p = new List_Link(*position.d_current_p, data);
        }
        //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // ACCESSORS
        void print()
            // output the contents of this list to stdout.
        {
            printf( "[" );
            for (List_Link *p = d_head_p; p; p = p->d_next_p) {
                printf( " %d", p->d_data );
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
            std::printf( "a = "); a.print();
        }

        List     b;
        ListIter bIt = b.begin();
        a.insert(bIt, 4);       // Oops! Should have been: 'b.insert(bIt, 4);'
        a.insert(bIt, 5);       // Oops!   "     "     "   '    "     "   5  '
        a.insert(bIt, 6);       // Oops!   "     "     "   '    "     "   6  '

        if (printFlag) {
            std::printf( "a = "); a.print();
            std::printf( "b = "); b.print();
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
//  Assertion failed: this == position.d_parent_p, file my_list.cpp, line 56
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

    // Change the handler return policy not to abort, except for manual tests
    // and tests that deal directly with the policy check.
    if (test > 0 && test != 5)
    {
        // Enable assertions to return (in violation of policy) for testing
        // purposes only.

        char *key = const_cast<char*>(
            bsls::Assert::k_permitOutOfPolicyReturningAssertionBuildKey);
        strcpy(key, "bsls-PermitOutOfPolicyReturn");

        bsls::Assert::permitOutOfPolicyReturningFailureHandler();
    }

    switch (test) { case 0:  // zero is always the leading case
      case 16: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #6
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE: Using "ASSERT" with 'BDE_BUILD_TARGET_SAFE_2'
        // --------------------------------------------------------------------

        if (verbose) printf( "\nUSAGE EXAMPLE #6"
                             "\n================\n" );

        if (verbose) printf( "\n6. Using (BSLS) \"ASSERT\" Macros in "
                            "Conjunction with BDE_BUILD_TARGET_SAFE_2\n" );

        // See usage examples section at top of this file.

#ifndef BDE_BUILD_TARGET_SAFE_2
        if (veryVerbose) printf( "\tsafe mode 2 is *not* defined\n" );
        sillyFunc(veryVerbose);
#else
        if (veryVerbose) printf( "\tSAFE MODE 2 *is* defined.\n" );

        // bsls::Assert::setViolationHandler(::testDriverPrint);
                                                          // for usage example
        bsls::Assert::setViolationHandler(::testDriverHandler);
                                                          // for regression
        globalReset();
        ASSERT(false == globalAssertFiredFlag);
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
            ASSERT(true == globalAssertFiredFlag);
        }
#endif  // BDE_BUILD_TARGET_EXC
#endif  // BDE_BUILD_TARGET_SAFE_2
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #5
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE: Using Scoped Guard
        // --------------------------------------------------------------------

        if (verbose) printf( "\nUSAGE EXAMPLE #5"
                             "\n================\n" );

        if (verbose) printf(
                      "\n5. Using the bsls::AssertFailureHandlerGuard\n" );

        // See usage examples section at top of this file.

        ASSERT(&bsls::Assert::failByAbort == bsls::Assert::violationHandler());

#ifndef BDE_BUILD_TARGET_OPT
    #if defined(BDE_BUILD_TARGET_EXC) ||                                      \
        defined(BSLS_ASSERT_ENABLE_TEST_CASE_10)

        if (verbose) printf(
                "\n*** Note that the following 'Internal Error: ... 0 <= n' "
                "message is expected:\n\n" );

        ASSERT(0 != wrapperFunc(verbose));

    #endif
#endif
        ASSERT(&bsls::Assert::failByAbort == bsls::Assert::violationHandler());

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #4
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE: Creating Your Own Assert-Handler
        // --------------------------------------------------------------------

        if (verbose) printf( "\nUSAGE EXAMPLE #4"
                             "\n================\n" );

#ifndef BDE_BUILD_TARGET_EXC
        if (verbose) {
            printf(
               "\tTest disabled as exceptions are NOT enabled.\n"
              "\tCalling the test function would abort.\n" );
        }

#else
        if (verbose) printf(
                            "\n4. Creating a Custom Assertion Handler\n" );

        // See usage examples section at top of this file.

        globalEnableOurPrintingFlag = veryVerbose;

        ASSERTION_TEST_BEGIN
        ourMain();
        ASSERTION_TEST_END
#endif
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #3
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE: Using Administration Functions
        //   USAGE EXAMPLE: Installing Prefabricated Assert-Handlers
        // --------------------------------------------------------------------

        if (verbose) printf( "\nUSAGE EXAMPLE #3"
                             "\n================\n" );

        if (verbose) printf(
                "\n3. Runtime Configuration of the bsls::Assert Facility\n" );

        // See usage examples section at top of this file.

        myMain();

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #2
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE: Invoking an assert handler directly
        // --------------------------------------------------------------------

        if (verbose) printf( "\nUSAGE EXAMPLE #2"
                             "\n================\n" );

        if (verbose) printf(
            "\n2. When and How to Call the Invoke-Handler Method Directly\n" );

        // See usage examples section at top of this file.

        bsls::Assert::setViolationHandler(::testDriverPrint);

#ifndef BDE_BUILD_TARGET_EXC
        globalReturnOnTestAssert = true;
#endif

        ASSERTION_TEST_BEGIN
        someFunc(1, 1, 0);
        ASSERTION_TEST_END

#ifndef BDE_BUILD_TARGET_EXC
        globalReturnOnTestAssert = false;
#endif

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #1
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE: Using Assert Macros
        // --------------------------------------------------------------------

        if (verbose) printf( "\nUSAGE EXAMPLE #1"
                             "\n================\n" );

        if (verbose) printf( "\n1. Using BSLS_ASSERT, BSLS_ASSERT_SAFE, and "
                             "BSLS_ASSERT_OPT\n" );

        // See usage examples section at top of this file.

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // LEGACY FAILURE HANDLER
        //
        // Concerns:
        //: 1 Failure handlers with the 'legacy' signature should continue to
        //:   work properly and use the new overloads for that signature.
        //
        // Plan:
        //: 1 Set the failure handler to 'legacyTestDriverHandler'
        //:
        //: 2 Verify that the 'failureHandler' function returns the correct
        //:   expected function pointer, and that 'failureHandler' now returns
        //:   a different value (pointing to a 'private' function that we
        //:   cannot name.)
        //:
        //: 3 Directly invoke the handler using both overloads of
        //:   'invokeHandler' in a manner paralleling the breathing test for
        //:   the newer functions.
        //:
        //: 4 Verify that a handler guard created with a 'Handler' still works
        //:   as expected.
        //
        // Testing:
        //   typedef void (*Handler)(const char *, const char *, int);
        //   void setFailureHandler(Handler function);
        //   bsls::Assert::Handler failureHandler();
        //   AssertFailureHandlerGuard(Handler)
        // --------------------------------------------------------------------

        if (verbose) printf( "\nLEGACY FAILURE HANDLER"
                             "\n======================\n" );

        if (verbose) printf(
                "\nVerify that the correct assert callback is installed "
                "by default.\n" );

        ASSERT(bsls::Assert::failByAbort == bsls::Assert::violationHandler());

        bsls::Assert::setFailureHandler(&legacyTestDriverHandler);

        // This should now be the 'private' method 'bsls::Assert::legacyFail',
        // so we just verify that the value has changed.
        ASSERT(bsls::Assert::failByAbort != bsls::Assert::violationHandler());

        // Store the pointer to this 'private' function for future comparisons.
        bsls::Assert::Handler legacyFailPtr = bsls::Assert::failureHandler();

        ASSERT(::legacyTestDriverHandler ==
                                         bsls::Assert::failureHandler());

        if (verbose) printf(
                "\nVerify that the 'invokeHandler' properly transmits "
                "its arguments to a 'Handler'.\n" );
        {

            globalReset();
            ASSERT(false == globalAssertFiredFlag);
            ASSERT(false == globalLegacyAssertFiredFlag);

#ifndef BDE_BUILD_TARGET_EXC
            globalReturnOnTestAssert = true;
#endif

            bsls::AssertViolation violation("ExPrEsSiOn",
                                            "FiLe",
                                            12345678,
                                            "LeVeL");
            ASSERTION_TEST_BEGIN
                bsls::Assert::invokeHandler(violation);
            ASSERTION_TEST_END

            ASSERT(    true == globalAssertFiredFlag);
            ASSERT(    true == globalLegacyAssertFiredFlag);
            ASSERT(       0 == std::strcmp("ExPrEsSiOn", globalText));
            ASSERT(       0 == std::strcmp("FiLe",       globalFile));
            ASSERT(       0 == std::strcmp("",           globalLevel));
            ASSERT(12345678 == globalLine);

#ifndef BDE_BUILD_TARGET_EXC
            globalReturnOnTestAssert = false;
#endif
        }

        if (verbose) printf(
                "\nVerify that the legacy 'invokeHandler' properly transmits "
                "its arguments to a 'Handler'.\n" );
        {

            globalReset();
            ASSERT(false == globalAssertFiredFlag);
            ASSERT(false == globalLegacyAssertFiredFlag);

#ifndef BDE_BUILD_TARGET_EXC
            globalReturnOnTestAssert = true;
#endif

            ASSERTION_TEST_BEGIN
                bsls::Assert::invokeHandler("ExPrEsSiOn", "FiLe", 12345678);
            ASSERTION_TEST_END

            ASSERT(    true == globalAssertFiredFlag);
            ASSERT(    true == globalLegacyAssertFiredFlag);
            ASSERT(       0 == std::strcmp("ExPrEsSiOn", globalText));
            ASSERT(       0 == std::strcmp("FiLe",       globalFile));
            ASSERT(       0 == std::strcmp("",           globalLevel));
            ASSERT(12345678 == globalLine);

#ifndef BDE_BUILD_TARGET_EXC
            globalReturnOnTestAssert = false;
#endif
        }

        if (verbose) printf("\nRestoring the handler to the default.\n");

        bsls::Assert::setViolationHandler(&bsls::Assert::failByAbort);
        ASSERT(&bsls::Assert::failByAbort == bsls::Assert::violationHandler());
        ASSERT(NULL == bsls::Assert::failureHandler());

        {
            bsls::AssertFailureHandlerGuard guard(::legacyTestDriverHandler);

            ASSERT(legacyFailPtr == bsls::Assert::failureHandler());
            ASSERT(::legacyTestDriverHandler ==
                                               bsls::Assert::failureHandler());

            {
                bsls::AssertFailureHandlerGuard guard2(::testDriverHandler);

                ASSERT(::testDriverHandler ==
                                             bsls::Assert::violationHandler());
                ASSERT(NULL == bsls::Assert::failureHandler());
            }

            if (verbose) printf("\nVerifying that a legacy handler gets "
                                "properly restored.\n");

            ASSERT(legacyFailPtr == bsls::Assert::failureHandler());
            ASSERT(::legacyTestDriverHandler ==
                                         bsls::Assert::failureHandler());
        }

        ASSERT(&bsls::Assert::failByAbort == bsls::Assert::violationHandler());

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // RETURNING HANDLER LOG: BACKOFF
        //
        // Concerns:
        //: 1 Log messages should back off exponentially.
        //
        // Plan:
        //: 1 In build configurations that allow handlers to return, install a
        //:   violation handler that increments a counter and returns, and an
        //:   instrumented log callback that increments a counter.
        //:
        //: 2 Call 'invokeHandler' repeatedly, and confirm that messages are
        //:   emitted at the appropriate rate.  (C-1)
        //
        // Testing:
        //   CONCERN: Returning handler log: backoff
        // --------------------------------------------------------------------

        if (verbose) printf( "\nRETURNING HANDLER LOG: BACKOFF"
                             "\n==============================\n" );

#if        !defined(BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER)           \
        || !defined(BSLS_PLATFORM_CMP_MSVC)

        // 'invokeHandler' cannot return if we are building on Windows with the
        // '...ENABLE_NORETURN...' flag turned on.

        typedef HandlerReturnTest Test;

        bsls::AssertFailureHandlerGuard guard(Test::countingViolationHandler);
        bsls::Log::setLogMessageHandler(Test::countingLogMessageHandler);

        bsls::Types::Int64 iterations = 0;
        for (int triggerCount = 1; triggerCount < 7; ++triggerCount) {
            while (Test::s_loggerInvocationCount < 2 * triggerCount) {
                bsls::Assert::invokeHandler("maxAllowed < value",
                                            __FILE__,
                                            __LINE__);

                ++iterations;
            }

            if (veryVerbose) {
                P_(iterations)
                P_(triggerCount)
                P_(Test::s_handlerInvocationCount)
                P(Test::s_loggerInvocationCount)
            }

            ASSERT(Test::s_loggerInvocationCount == 2 * triggerCount);
            ASSERT(iterations == Test::s_handlerInvocationCount);
            ASSERT(1 << (triggerCount - 1) == iterations);
        }
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // RETURNING HANDLER LOG: CONTENT
        //
        // Concerns:
        //: 1 'invokeHandler' should log a message via 'bsls_log' if the
        //:   currently-installed handler returns.
        //:
        //: 2 The log message should identify the file and line where the
        //:   failed assertion occurred.
        //:
        //: 3 The log message should be severity 'fatal'.
        //
        // Plan:
        //: 1 In build configurations that allow handlers to return, install a
        //:   violation handler that does nothing but return, and an
        //:   instrumented log callback that captures log messages instead of
        //:   printing them.
        //:
        //: 2 Call 'invokeHandler' twice, and confirm that messages are emitted
        //:   with the appropriate (distinct) content.  (C-1..3)
        //
        // Testing:
        //   CONCERN: Returning handler log: content
        // --------------------------------------------------------------------

        if (verbose) printf( "\nRETURNING HANDLER LOG: CONTENT"
                             "\n==============================\n" );

#if        !defined(BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER)           \
        || !defined(BSLS_PLATFORM_CMP_MSVC)

        // 'invokeHandler' cannot return if we are building on Windows with the
        // '...ENABLE_NORETURN...' flag turned on.

        static const struct {
            int  d_tableLine;
            char d_file[LogProfile::k_TEXT_BUFFER_SIZE];
            int  d_line;
            char d_text[LogProfile::k_TEXT_BUFFER_SIZE];
            char d_expected[LogProfile::k_TEXT_BUFFER_SIZE];
        } DATA[] = {
            //LN FILE       L  TEXT        EXPECTED
            //-- --------- --  ----------- --------
            {L_, "",       99, "",         "BSLS_ASSERT failure: ''"         },
            {L_, "FB",      2, "MB",       "BSLS_ASSERT failure: 'MB'"       },
            {L_, "FileA",   1, "messageA", "BSLS_ASSERT failure: 'messageA'" },
            {L_, "foo.cpp",42, "0 != ptr", "BSLS_ASSERT failure: '0 != ptr'" },
        };
        enum { NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

        typedef HandlerReturnTest Test;

        bsls::AssertFailureHandlerGuard guard(Test::emptyViolationHandler);
        bsls::Log::setLogMessageHandler(Test::recordingLogMessageHandler);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE          = DATA[ti].d_tableLine;
            const char *ASSERT_FILE   = DATA[ti].d_file;
            const int   ASSERT_LINE   = DATA[ti].d_line;
            const char *ASSERT_TEXT   = DATA[ti].d_text;
            const char *EXPECTED_TEXT = DATA[ti].d_expected;

            if (veryVerbose) {
                T_ P_(ti)
                   P_(LINE)
                   P_(ASSERT_FILE)
                   P_(ASSERT_LINE)
                   P(ASSERT_TEXT)
            }

            Test::clear();

            ASSERT(0 == strcmp("", Test::s_firstProfile.d_file));
            ASSERT(0 ==            Test::s_firstProfile.d_line);
            ASSERT(0 == strcmp("", Test::s_firstProfile.d_text));

            ASSERT(0 == strcmp("", Test::s_secondProfile.d_file));
            ASSERT(0 ==            Test::s_secondProfile.d_line);
            ASSERT(0 == strcmp("", Test::s_secondProfile.d_text));

            while (0 == Test::s_loggerInvocationCount) {
                // Keep on invoking the handler until the logger is called.

                bsls::Assert::invokeHandler(ASSERT_TEXT,
                                            ASSERT_FILE,
                                            ASSERT_LINE);
            }

            ASSERT(0 == strcmp(ASSERT_FILE,   Test::s_firstProfile.d_file));
            ASSERT(            ASSERT_LINE == Test::s_firstProfile.d_line);
            ASSERT(0 == strcmp(EXPECTED_TEXT, Test::s_firstProfile.d_text));

            ASSERT(0 != strcmp("", Test::s_secondProfile.d_file));
            ASSERT(          0 !=  Test::s_secondProfile.d_line);
            ASSERT(ASSERT_LINE !=  Test::s_secondProfile.d_line);
            ASSERT(0 != strstr(    Test::s_secondProfile.d_text,
                                   "Bad 'bsls_assert' configuration:"));
        }
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // CONFIGURATION MACROS
        //
        // Concerns:
        //: 1 The configuration macros that report on which assert facilities
        //:   are available in the current build mode might not report
        //:   correctly in all build modes.
        //
        // Plan:
        //: 1 Subvert the regular include guards to verify that inclusion of
        //:   'bsls_assert.h' defines the proper macros when included with
        //:   varying build modes.
        //
        // Testing:
        //   BSLS_ASSERT_SAFE_IS_ACTIVE
        //   BSLS_ASSERT_IS_ACTIVE
        //   BSLS_ASSERT_OPT_IS_ACTIVE
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCONFIGURATION MACROS"
                             "\n====================\n" );

        TestConfigurationMacros();
      } break;
      case 6: {
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
        //: 4 That 'lockAssertAdministration' has no effect on guard.
        //
        // Plan:
        //: 1 Create a guard, passing it the 'testDriverHandler' handler, and
        //:   verify, using 'failureHandler', that this new handler was
        //:   installed.  Then lock the administration, and repeat in nested
        //:   fashion with the 'failBySleep' handler.  Verify restoration on
        //:   the way out.
        //
        // Testing:
        //   class bsls::AssertFailureHandlerGuard
        //   AssertFailureHandlerGuard(Handler)
        //   ~AssertFailureHandlerGuard()
        //   CONCERN: that locking does not stop the handlerGuard from working
        // --------------------------------------------------------------------

        if (verbose) printf( "\nFAILURE HANDLER GUARD"
                             "\n=====================\n" );

        if (verbose) printf( "\nVerify initial assert handler.\n" );

        ASSERT(bsls::Assert::failByAbort == bsls::Assert::violationHandler());

        if (verbose) printf( "\nCreate guard with 'testDriverHandler' "
                             "handler.\n" );

        {
            bsls::AssertFailureHandlerGuard guard(::testDriverHandler);

            if (verbose) printf( "\nVerify new assert handler.\n" );

            ASSERT(::testDriverHandler == bsls::Assert::violationHandler());

            if (verbose) printf( "\nLock administration.\n" );

            bsls::Assert::lockAssertAdministration();

            if (verbose) printf( "\nRe-verify new assert handler.\n" );

            ASSERT(testDriverHandler == bsls::Assert::violationHandler());

            if (verbose) printf(
                     "\nCreate second guard with 'failBySleep' handler.\n" );
            {
                bsls::AssertFailureHandlerGuard guard(
                                                    bsls::Assert::failBySleep);

                if (verbose) printf( "\nVerify newer assert handler.\n" );

                ASSERT(bsls::Assert::failBySleep ==
                                             bsls::Assert::violationHandler());

                if (verbose) printf(
                   "\nDestroy guard created with '::failBySleep' handler.\n" );
            }

            if (verbose) printf( "\nVerify new assert handler.\n" );

            ASSERT(::testDriverHandler == bsls::Assert::violationHandler());

            if (verbose) printf(
                    "\nDestroy guard created with '::testDriverHandler' "
                    "handler.\n" );
        }

        if (verbose) printf( "\nVerify initial assert handler.\n" );

        ASSERT(bsls::Assert::failByAbort == bsls::Assert::violationHandler());

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // ASSERTION HANDLER RETURN POLICY
        //
        // Concerns:
        //: 1 'abortUponReturningAssertionFailureHandler' returns 'true', with
        //:   default settings/build.
        //:
        //: 2 If 'permitOutOfPolicyReturningFailureHandler' is *not* called but
        //:   'k_permitOutOfPolicyReturningAssertionBuildKey' is set to
        //:   "bsls-PermitOutOfPolicyReturn"
        //:   'abortUponReturningAssertionFailureHandler' returns 'true'.
        //:
        //: 3 If 'permitOutOfPolicyReturningFailureHandler' is called but
        //:   'k_permitOutOfPolicyReturningAssertionBuildKey' is not set to
        //:   "bsls-PermitOutOfPolicyReturn"
        //:   'abortUponReturningAssertionFailureHandler' returns 'true'.
        //:
        //: 4 If 'permitOutOfPolicyReturningFailureHandler' is called and
        //:   'k_permitOutOfPolicyReturningAssertionBuildKey' is set to
        //:   "bsls-PermitOutOfPolicyReturn" and the provided assertion failure
        //:   handler does not return, the program is not aborted.
        //
        // Plan:
        //: 1 We cannot test behavior that aborts except by hand (see negative
        //:   test cases) therefore we use the
        //:   'abortUponReturningAssertionFailureHandler' method to verify that
        //:   the default behavior would be termination.  (C-1..3)
        //:
        //: 2 Set 'k_permitOutOfPolicyReturningAssertionBuildKey' to
        //:   "bsls-PermitOutOfPolicyReturn" (use const cast) and call
        //:   'permitOutOfPolicyReturningFailureHandler'.  Use
        //:   'returningHandler' assertion handler and fire an assert.  (C-4)
        //
        // Testing:
        //   void permitOutOfPolicyReturningFailureHandler();
        //   static k_permitOutOfPolicyReturningAssertionBuildKey
        //   bool abortUponReturningAssertionFailureHandler();
        // --------------------------------------------------------------------

        if (verbose) printf( "\nASSERTION HANDLER RETURN POLICY"
                             "\n===============================\n" );

        if (verbose) printf( "\nTesting default behavior.\n" );
        {
            ASSERT(bsls::Assert::abortUponReturningAssertionFailureHandler()
                                                                      == true);
        }

        if (verbose) printf( "\nOnly AssertionBuildKey is set.\n" );
        {

            // Half enable assertions to return (in violation of policy) for
            // testing purposes only.
            char *key = const_cast<char*>(
                  bsls::Assert::k_permitOutOfPolicyReturningAssertionBuildKey);
            strcpy(key, "bsls-PermitOutOfPolicyReturn");

            ASSERT(bsls::Assert::abortUponReturningAssertionFailureHandler()
                                                                      == true);

            // Disable assertions to return.
            strcpy(key, "No");

        }

        if (verbose) {
            printf( "\nOnly permitOutOfPolicyReturningFailureHandler"
                    "called.\n");
        }

        {
            bsls::Assert::permitOutOfPolicyReturningFailureHandler();
            ASSERT(bsls::Assert::abortUponReturningAssertionFailureHandler()
                                                                      == true);
        }

        if (verbose) printf("\nTesting exceptional behavior\n");
        {
            // Set up the exceptional behavior

            // Enable assertions to return (in violation of policy) for testing
            // purposes only.

            char *key = const_cast<char*>(
                  bsls::Assert::k_permitOutOfPolicyReturningAssertionBuildKey);
            strcpy(key, "bsls-PermitOutOfPolicyReturn");

            bsls::Assert::permitOutOfPolicyReturningFailureHandler();

            // Set up the misbehaving handler

            bsls::Assert::setViolationHandler(returningHandler);

            // Fire an assertion

            BSLS_ASSERT(!"This is an intentional assert");
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // ASSERTION FAILURE HANDLERS
        //
        // Concerns:
        //: 1 That each of the assertion failure handlers provided herein
        //:   behaves as advertised and (at least) matches the signature of the
        //:   'bsls::Assert::ViolationHandler' or 'bsls::Assert::Handler'
        //:   'typedef's.
        //
        // Plan:
        //: 1 Verify each handler's behavior.  Unfortunately, we cannot test
        //:   functions that abort except by hand (see negative test cases).
        //:
        //: 2 Assign each handler function to a pointer of type 'Handler'.
        //
        // Testing:
        //   typedef void (*ViolationHandler)(const AssertViolation&);
        //   void failAbort(const char *, const char *, int);
        //   void failByAbort(const AssertViolation& violation);
        //   void failSleep(const char *, const char *, int);
        //   void failBySleep(const AssertViolation& violation);
        //   void failThrow(const char *, const char *, int);
        //   void failByThrow(const AssertViolation& violation);
        // --------------------------------------------------------------------

        if (verbose) printf( "\nASSERTION FAILURE HANDLERS"
                             "\n==========================\n" );

        if (verbose) printf( "\nTesting 'void failAbort("
                             "const char *, const char *, int);'\n" );
        {
            bsls::Assert::Handler f = bsls::Assert::failAbort;
            (void) f;

            if (veryVerbose) {
                printf( "\t(Aborting behavior must be tested by hand.)\n" );
            }
        }

        if (verbose) printf( "\nTesting 'void failByAbort("
                             "const AssertViolation& violation);'\n" );
        {
            bsls::Assert::ViolationHandler f = bsls::Assert::failByAbort;
            (void) f;

            if (veryVerbose) {
                printf( "\t(Aborting behavior must be tested by hand.)\n" );
            }
        }

        if (verbose) printf( "\nTesting 'void failSleep("
                             "const char *, const hcar *, int);'\n" );
        {
            bsls::Assert::Handler f = bsls::Assert::failSleep;
            (void) f;

            if (veryVerbose) {
                printf( "\t(Sleeping behavior must be tested by hand.)\n" );
            }
        }

        if (verbose) printf( "\nTesting 'void failBySleep("
                             "const AssertViolation& violation);'\n" );
        {
            bsls::Assert::ViolationHandler f = bsls::Assert::failBySleep;
            (void) f;

            if (veryVerbose) {
                printf( "\t(Sleeping behavior must be tested by hand.)\n" );
            }
        }

        if (verbose) printf( "\nTesting 'void failThrow("
                             "const char *, const char *, int);'\n" );
        {

            bsls::Assert::Handler f = bsls::Assert::failThrow;
            (void) f;

#ifdef BDE_BUILD_TARGET_EXC
            const char *text = "Test text";
            const char *file = "bsls_assert.t.cpp";
            int         line = 101;

            if (veryVerbose) {
                printf( "\tExceptions ARE enabled.\n" );
            }

            try {
                f(text,file,line);
            }
            catch (bsls::AssertTestException) {
                if (veryVerbose) printf( "\tException Text Succeeded!\n" );
            }
            catch (...) {
                ASSERT("Threw wrong exception!" && 0);
            }
#else
            if (veryVerbose) {
                printf( "\tExceptions are NOT enabled.\n" );
            }
#endif
        }

        if (verbose) printf( "\nTesting 'void failByThrow("
                             "const AssertViolation& violation);'\n" );
        {

            bsls::Assert::ViolationHandler f = bsls::Assert::failByThrow;
            (void) f;

#ifdef BDE_BUILD_TARGET_EXC
            const char *text = "Test text";
            const char *file = "bsls_assert.t.cpp";
            int         line = 101;

            if (veryVerbose) {
                printf( "\tExceptions ARE enabled.\n" );
            }

            try {
                f(bsls::AssertViolation(text, file, line, "L"));
            }
            catch (bsls::AssertTestException) {
                if (veryVerbose) printf( "\tException Text Succeeded!\n" );
            }
            catch (...) {
                ASSERT("Threw wrong exception!" && 0);
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
        //: 1 Any component including 'bsls_assert.h' that has multiple
        //:   assertion-mode flags defined should fail to compile and provide a
        //:   useful diagnostic.
        //
        // Plan:
        //: 1 Repeat the assertions at runtime, and fail if any two macros
        //:   incompatible macros are present.
        //:
        //: 2 (Manually) observe that the test is in the bsls_assert.h file.
        //
        // Testing:
        //   CONCERN: ubiquitously detect multiply-defined assertion-mode flags
        // --------------------------------------------------------------------

        if (verbose) printf( "\nINCOMPATIBLE BUILD TARGETS"
                             "\n==========================\n" );

        if (verbose) printf( "\nExtract defined-ness of each target.\n" );

#ifdef BSLS_ASSERT_LEVEL_ASSERT_SAFE
        bool a = 1;
#else
        bool a = 0;
#endif

#ifdef BSLS_ASSERT_LEVEL_ASSERT
        bool b = 1;
#else
        bool b = 0;
#endif

#ifdef BSLS_ASSERT_LEVEL_ASSERT_OPT
        bool c = 1;
#else
        bool c = 0;
#endif

#ifdef BSLS_ASSERT_LEVEL_NONE
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
        // (BSLS) "ASSERT"-MACRO TEST
        //
        // Concerns:
        //: 1 Each assert macro instantiates only when the appropriate build
        //:   mode is set.
        //:
        //: 2 When instantiated, each macro fires only on "0" valued
        //:   expressions
        //:
        //: 3 When a macro fires, the correct text, line, and file are
        //:   transmitted to the current assertion-failure handler.
        //:
        //: 4 That the expression text that is printed is exactly what is in
        //:   the parentheses of the macro.
        //
        // Plan:
        //: 1 We must not try to change build targets (or assert modes), but
        //:   just observe them to see which assert macros should be
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
        //   BSLS_ASSERT_SAFE(X)
        //   BSLS_ASSERT(X)
        //   BSLS_ASSERT_OPT(X)
        //   BSLS_ASSERT_INVOKE(X)
        //   CONCERN: ASSERT macros are instantiated properly for build targets
        //   CONCERN: all combinations of BDE_BUILD_TARGETs are allowed
        //   CONCERN: any one assert mode overrides all BDE_BUILD_TARGETs
        // --------------------------------------------------------------------

        if (verbose) printf( "\n(BSLS) \"ASSERT\"-MACRO TEST"
                             "\n==========================\n" );

        if (verbose) printf( "\nInstall 'testDriverHandler' "
                             "assertion-handler.\n" );

        // Change the handler return policy not to abort.
        {
            // Enable assertions to return (in violation of policy) for testing
            // purposes only.

            char *key = const_cast<char*>(
                  bsls::Assert::k_permitOutOfPolicyReturningAssertionBuildKey);
            strcpy(key, "bsls-PermitOutOfPolicyReturn");

            bsls::Assert::permitOutOfPolicyReturningFailureHandler();
        }

        bsls::Assert::setViolationHandler(&testDriverHandler);
        ASSERT(::testDriverHandler == bsls::Assert::violationHandler());

        if (veryVerbose) printf( "\tSet up all but line numbers now. \n" );

        const void *p    = 0;
        const char *text = "p";
        const char *file = __FILE__;
        int         line;                    // initialized each time

        const char *expr = "false == true";

#ifndef BDE_BUILD_TARGET_EXC
        globalReturnOnTestAssert = true;
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

#ifdef BSLS_ASSERT_LEVEL_ASSERT_SAFE
            printf( "\t1 == BSLS_ASSERT_LEVEL_ASSERT_SAFE\n" );
#else
            printf( "\t0 == BSLS_ASSERT_LEVEL_ASSERT_SAFE\n" );
#endif

#ifdef BSLS_ASSERT_LEVEL_ASSERT
            printf( "\t1 == BSLS_ASSERT_LEVEL_ASSERT\n" );
#else
            printf( "\t0 == BSLS_ASSERT_LEVEL_ASSERT\n" );
#endif

#ifdef BSLS_ASSERT_LEVEL_ASSERT_OPT
            printf( "\t1 == BSLS_ASSERT_LEVEL_ASSERT_OPT\n" );
#else
            printf( "\t0 == BSLS_ASSERT_LEVEL_ASSERT_OPT\n" );
#endif

#ifdef BSLS_ASSERT_LEVEL_NONE
            printf( "\t1 == BSLS_ASSERT_LEVEL_NONE\n" );
#else
            printf( "\t0 == BSLS_ASSERT_LEVEL_NONE\n" );
#endif

#if IS_BSLS_ASSERT_MODE_FLAG_DEFINED
            printf( "\t1 == IS_BSLS_ASSERT_MODE_FLAG_DEFINED\n" );
#else
            printf( "\t0 == IS_BSLS_ASSERT_MODE_FLAG_DEFINED\n" );
#endif

#if BSLS_NO_ASSERTION_MACROS_DEFINED
            printf( "\t1 == BSLS_NO_ASSERTION_MACROS_DEFINED\n" );
#else
            printf( "\t0 == BSLS_NO_ASSERTION_MACROS_DEFINED\n" );
#endif
        }

        //_____________________________________________________________________
        //            BSLS_ASSERT_SAFE, BSLS_ASSERT, BSLS_ASSERT_OPT
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE)                                   \
 || !IS_BSLS_ASSERT_MODE_FLAG_DEFINED && (                                   \
        defined(BDE_BUILD_TARGET_SAFE_2) ||                                  \
        defined(BDE_BUILD_TARGET_SAFE)   )

        if (verbose) printf(
                          "\nEnabled: ASSERT_SAFE, ASSERT, ASSERT_OPT\n" );

        if (veryVerbose) printf( "\tCheck for integer expression.\n" );

        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_SAFE(0);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);

        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT     (0);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);

        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (0);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);

        if (veryVerbose) printf( "\tCheck for pointer expression.\n" );

        globalReset();
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_SAFE(p);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(text, globalText,    0 == std::strcmp(text, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(line, globalLine, line == globalLine);

        globalReset();
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT     (p);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(text, globalText,    0 == std::strcmp(text, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(line, globalLine, line == globalLine);

        globalReset();
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (p);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(text, globalText,    0 == std::strcmp(text, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(line, globalLine, line == globalLine);

        if (veryVerbose) printf( "\tCheck for expression with spaces.\n" );

        globalReset();
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_SAFE(false == true);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(expr, globalText,    0 == std::strcmp(expr, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(line, globalLine, line == globalLine);

        globalReset();
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT     (false == true);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(expr, globalText,    0 == std::strcmp(expr, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(line, globalLine, line == globalLine);

        globalReset();
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (false == true);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(expr, globalText,    0 == std::strcmp(expr, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
#endif

        //_____________________________________________________________________
        //                    BSLS_ASSERT, BSLS_ASSERT_OPT
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if defined(BSLS_ASSERT_LEVEL_ASSERT)                                        \
 || !IS_BSLS_ASSERT_MODE_FLAG_DEFINED &&                                     \
        !defined(BDE_BUILD_TARGET_OPT) &&                                    \
        !defined(BDE_BUILD_TARGET_SAFE) &&                                   \
        !defined(BDE_BUILD_TARGET_SAFE_2)

        if (verbose) printf( "\nEnabled: ASSERT, ASSERT_OPT\n" );

        if (veryVerbose) printf( "\tCheck for integer expression.\n" );

        globalReset(); BSLS_ASSERT_SAFE(0); ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT     (0);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);

        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (0);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);

        if (veryVerbose) printf( "\tCheck for pointer expression.\n" );

        globalReset(); BSLS_ASSERT_SAFE(p); ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT     (p);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(text, globalText,    0 == std::strcmp(text, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(line, globalLine, line == globalLine);

        globalReset();
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (p);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(text, globalText,    0 == std::strcmp(text, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(line, globalLine, line == globalLine);

        if (veryVerbose) printf( "\tCheck for expression with spaces.\n" );

        globalReset(); BSLS_ASSERT_SAFE(false == true);
        ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT     (false == true);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(expr, globalText,    0 == std::strcmp(expr, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(line, globalLine, line == globalLine);

        globalReset();
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (false == true);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(expr, globalText,    0 == std::strcmp(expr, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
#endif

        //_____________________________________________________________________
        //                         BSLS_ASSERT_OPT
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if defined(BSLS_ASSERT_LEVEL_ASSERT_OPT)                                    \
 || !IS_BSLS_ASSERT_MODE_FLAG_DEFINED &&                                     \
        defined(BDE_BUILD_TARGET_OPT) &&                                     \
        !defined(BDE_BUILD_TARGET_SAFE) &&                                   \
        !defined(BDE_BUILD_TARGET_SAFE_2)

        if (verbose) printf( "\nEnabled: ASSERT_OPT\n" );

        if (veryVerbose) printf( "\tCheck for integer expression.\n" );

        globalReset(); BSLS_ASSERT_SAFE(0); ASSERT(0 == globalAssertFiredFlag);
        globalReset(); BSLS_ASSERT     (0); ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (0);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);

        if (veryVerbose) printf( "\tCheck for pointer expression.\n" );

        globalReset(); BSLS_ASSERT_SAFE(p); ASSERT(0 == globalAssertFiredFlag);
        globalReset(); BSLS_ASSERT     (p); ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (p);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(text, globalText,    0 == std::strcmp(text, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(line, globalLine, line == globalLine);

        if (veryVerbose) printf( "\tCheck for expression with spaces.\n" );

        globalReset(); BSLS_ASSERT_SAFE(false == true);
        ASSERT(0 == globalAssertFiredFlag);

        globalReset(); BSLS_ASSERT     (false == true);
        ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (false == true);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(expr, globalText, 0 == std::strcmp(expr, globalText));
        LOOP2_ASSERT(file, globalFile, 0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
#endif

        //_____________________________________________________________________
        //                  *** None Instantiate ***
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if defined(BSLS_ASSERT_LEVEL_NONE)                             \

        if (verbose) printf( "\nEnabled: (* Nothing *)\n" );

        if (veryVerbose) printf( "\tCheck for integer expression.\n" );

        globalReset(); BSLS_ASSERT_SAFE(0); ASSERT(0 == globalAssertFiredFlag);
        globalReset(); BSLS_ASSERT     (0); ASSERT(0 == globalAssertFiredFlag);
        globalReset(); BSLS_ASSERT_OPT (0); ASSERT(0 == globalAssertFiredFlag);

        if (veryVerbose) printf( "\tCheck for integer expression.\n" );

        globalReset(); BSLS_ASSERT_SAFE(p); ASSERT(0 == globalAssertFiredFlag);

        globalReset(); BSLS_ASSERT     (p); ASSERT(0 == globalAssertFiredFlag);

        globalReset(); BSLS_ASSERT_OPT (p); ASSERT(0 == globalAssertFiredFlag);

        if (veryVerbose) printf( "\tCheck for expression with spaces.\n" );

        globalReset(); BSLS_ASSERT_SAFE(false == true);
        ASSERT(0 == globalAssertFiredFlag);

        globalReset(); BSLS_ASSERT     (false == true);
        ASSERT(0 == globalAssertFiredFlag);

        globalReset(); BSLS_ASSERT_OPT (false == true);
        ASSERT(0 == globalAssertFiredFlag);
#endif

        //_____________________________________________________________________
        //                  *** BSLS_ASSERT_INVOKE (always instantiate) ***
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if (verbose) printf( "\nChecking BSLS_INVOKE\n" );

        globalReset();
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_INVOKE(text);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(text, globalText,    0 == std::strcmp(text, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(line, globalLine, line == globalLine);


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
        //: 1 Call 'setAssertHandler' to install the 'testDriverHandler'
        //:   "assert" function in order to observe that the installed function
        //:   was called using the 'invokeHandler' method -- and, contingently,
        //:   the 'BSLS_ASSERT_OPT(X)' macro -- with various arguments.
        //
        // Testing:
        //   BREATHING TEST
        //   CONCERN: By default, the 'bsls::Assert::failByAbort' is used.
        //   AssertViolation::AssertViolation(...);
        //   const char *AssertViolation::comment();
        //   const char *AssertViolation::fileName();
        //   int AssertViolation::lineNumber();
        //   const char *AssertViolation::assertLevel();
        //   void setViolationHandler(ViolationHandler function);
        //   bsls::Assert::ViolationHandler violationHandler();
        //   void invokeHandler(const char *t, const char *f, int);
        //   void invokeHandler(const AssertViolation&);
        //   void lockAssertAdministration();
        // --------------------------------------------------------------------

        if (verbose) printf( "\nBREATHING TEST"
                             "\n==============\n" );

        if (verbose) printf(
                "\nVerify that the correct assert callback is installed "
                "by default.\n" );

        ASSERT(bsls::Assert::failByAbort == bsls::Assert::violationHandler());

        if (verbose) printf(
                     "\nVerify that we can install a new assert callback.\n" );

        bsls::Assert::setViolationHandler(&testDriverHandler);
        ASSERT(::testDriverHandler == bsls::Assert::violationHandler());

        if (verbose) printf(
                "\nVerify that the 'invokeHandler' properly transmits "
                "its arguments.\n" );
        {

            globalReset();
            ASSERT(false        == globalAssertFiredFlag);
            ASSERT(false        == globalLegacyAssertFiredFlag);

#ifndef BDE_BUILD_TARGET_EXC
            globalReturnOnTestAssert = true;
#endif

            bsls::AssertViolation violation("ExPrEsSiOn",
                                            "FiLe",
                                            12345678,
                                            "LeVeL");
            ASSERTION_TEST_BEGIN
                bsls::Assert::invokeHandler(violation);
            ASSERTION_TEST_END

            ASSERT(    true == globalAssertFiredFlag);
            ASSERT(   false == globalLegacyAssertFiredFlag);
            ASSERT(       0 == std::strcmp("ExPrEsSiOn", globalText));
            ASSERT(       0 == std::strcmp("FiLe",       globalFile));
            ASSERT(       0 == std::strcmp("LeVeL",      globalLevel));
            ASSERT(12345678 == globalLine);

#ifndef BDE_BUILD_TARGET_EXC
            globalReturnOnTestAssert = false;
#endif
        }

        if (verbose) printf(
                "\nVerify that the legacy 'invokeHandler' properly transmits "
                "its arguments.\n" );
        {

            globalReset();
            ASSERT(false        == globalAssertFiredFlag);
            ASSERT(false        == globalLegacyAssertFiredFlag);

#ifndef BDE_BUILD_TARGET_EXC
            globalReturnOnTestAssert = true;
#endif

            ASSERTION_TEST_BEGIN
                bsls::Assert::invokeHandler("ExPrEsSiOn", "FiLe", 12345678);
            ASSERTION_TEST_END

            ASSERT(    true == globalAssertFiredFlag);
            ASSERT(   false == globalLegacyAssertFiredFlag);
            ASSERT(       0 == std::strcmp("ExPrEsSiOn", globalText));
            ASSERT(       0 == std::strcmp("FiLe",       globalFile));
            ASSERT(       0 == std::strcmp("INV",        globalLevel));
            ASSERT(12345678 == globalLine);

#ifndef BDE_BUILD_TARGET_EXC
            globalReturnOnTestAssert = false;
#endif
        }

        if (verbose) printf(
                "\nVerify that 'lockAssertAdministration' blocks callback "
                "changes.\n" );

        bsls::Assert::lockAssertAdministration();

        bsls::Assert::setViolationHandler(&bsls::Assert::failByAbort);
        ASSERT(::testDriverHandler == bsls::Assert::violationHandler());

#ifdef BSLS_ASSERT_LEVEL_NONE
        if (verbose) printf(
            "\n'BSLS_ASSERT_LEVEL_NONE' is defined; exit breathing test.\n" );
        break;
#endif
        if (verbose) printf(
                "\nVerify that 'BSLS_ASSERT_OPT' doesn't fire for '!0' "
                "expressions.\n" );

        if (veryVerbose) printf( "\tInteger-valued expression\n" );
        {
            globalReset();
            ASSERT(false == globalAssertFiredFlag);

            BSLS_ASSERT_OPT(true == true);
            ASSERT(false == globalAssertFiredFlag);
        }

        if (veryVerbose) printf( "\tPointer-valued expression\n" );
        {
            globalReset();

            ASSERT(false == globalAssertFiredFlag);

            BSLS_ASSERT_OPT((void *)(1));
            ASSERT(false == globalAssertFiredFlag);
        }

        if (verbose) printf(
                "\nVerify that 'BSLS_ASSERT_OPT' does fire for '0' "
                "expressions.\n" );

#ifndef BDE_BUILD_TARGET_EXC
            globalReturnOnTestAssert = true;
#endif

        if (veryVerbose) printf( "\tInteger-valued expression\n" );
        {
            globalReset();
            ASSERT(false == globalAssertFiredFlag);

            const char *const text = "true == false";
            const char *const file = __FILE__;
            int               line = -1;

            line = L_ + 2;
            ASSERTION_TEST_BEGIN
            BSLS_ASSERT_OPT(true == false);
            ASSERTION_TEST_END

            ASSERT(true == globalAssertFiredFlag);
            LOOP2_ASSERT(text, globalText, 0 == std::strcmp(text, globalText));
            LOOP2_ASSERT(file, globalFile, 0 == std::strcmp(file, globalFile));
            LOOP2_ASSERT(line, globalLine, line == globalLine);
        }

        if (veryVerbose) printf( "\tPointer-valued expression\n" );
        {
            globalReset();
            ASSERT(false == globalAssertFiredFlag);

            const char *const text = "(void *)(0)";
            const char *const file = __FILE__;
            int               line = -1;

            line = L_ + 2;
            ASSERTION_TEST_BEGIN
            BSLS_ASSERT_OPT((void *)(0));
            ASSERTION_TEST_END

            ASSERT(true == globalAssertFiredFlag);
            LOOP2_ASSERT(text, globalText, 0 == std::strcmp(text, globalText));
            LOOP2_ASSERT(file, globalFile, 0 == std::strcmp(file, globalFile));
            LOOP2_ASSERT(line, globalLine, line == globalLine);
        }

#ifndef BDE_BUILD_TARGET_EXC
            globalReturnOnTestAssert = false;
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
        //: 1 Call 'bsls::Assert::failByAbort' after blocking the signal.
        //
        // Testing:
        //   void failByAbort(const AssertViolation& violation);
        //   CONCERN: 'bsls::Assert::failByAbort' aborts
        //   CONCERN: 'bsls::Assert::failByAbort' prints to 'stderr'
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
        fprintf( stderr, "THE FOLLOWING SHOULD PRINT ON STDERR:\n"
                "Assertion failed: 0 != 0, file myfile.cpp, line 123\n" );

        bsls::Assert::failByAbort(bsls::AssertViolation("0 != 0", "myfile.cpp",
                                                      123, "L"));

        ASSERT(0 && "Should not be reached");
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // CALL DEPRECATED FAIL ABORT HANDLER
        //
        // Concerns:
        //: 1 That it does abort the program.
        //:
        //: 2 That it prints a message to 'stderr'.
        //
        // Plan:
        //: 1 Call 'bsls::Assert::failAbort' after blocking the signal.
        //
        // Testing:
        //   void failAbort(const char *, const char *, int);
        //   CONCERN: 'bsls::Assert::failAbort' aborts
        //   CONCERN: 'bsls::Assert::failAbort' prints to 'stderr'
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCALL DEPRECATED FAIL ABORT HANDLER"
                             "\n==================================\n" );

#ifdef BSLS_PLATFORM_OS_UNIX
        sigset_t newset;
        sigaddset(&newset, SIGABRT);

    #if defined(BDE_BUILD_TARGET_MT)
        pthread_sigmask(SIG_BLOCK, &newset, 0);
    #else
        sigprocmask(SIG_BLOCK, &newset, 0);
    #endif

#endif
        fprintf( stderr, "THE FOLLOWING SHOULD PRINT ON STDERR:\n"
                "Assertion failed: 0 != 0, file myfile.cpp, line 123\n" );

        bsls::Assert::failAbort("0 != 0", "myfile.cpp", 123);

        ASSERT(0 && "Should not be reached");
      } break;
      case -3: {
        // --------------------------------------------------------------------
        // CALL FAIL THROW HANDLER
        //
        // Concerns:
        //: 1 That it does *not* throw for an exception build when there is an
        //:   exception pending.
        //:
        //: 2 That it behaves as 'failByAbort' for non-exception builds.
        //
        // Plan:
        //: 1 Call 'bsls::Assert::failByThrow' from within the destructor of a
        //:   test object on the stack after a throw.
        //:
        //: 2 Call 'bsls::Assert::failByAbort' after blocking the signal.
        //
        // Testing:
        //   void failByThrow(const AssertViolation& violation);
        //   CONCERN: 'bsls::Assert::failByThrow' aborts in non-exception build
        //   CONCERN: 'bsls::Assert::failByThrow' prints to 'stderr' w/o EXC
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCALL FAIL THROW HANDLER"
                             "\n=======================\n" );

#if BDE_BUILD_TARGET_EXC
        printf( "\nEXCEPTION BUILD\n" );

        fprintf(stderr, "\nTHE FOLLOWING SHOULD PRINT ON STDERR:\n"
                "BSLS_ASSERT: An uncaught exception is pending;"
                " cannot throw 'bsls_asserttestexception'.\n" );
        fprintf(stderr, "assertion failed: 'failByThrow' handler called from "
                "~BadBoy, file f.c, line 9" );


        try {
            BadBoy bad;     // calls 'bsls::Assert::failByThrow' on destruction

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
                "Assertion failed: 0 != 0, file myfile.cpp, line 123\n" );

        bsls::Assert::failByAbort(bsls::AssertViolation("0 != 0", "myfile.cpp",
                                                      123,"L"));

        ASSERT(0 && "Should not be reached");
#endif
      } break;
      case -4: {
        // --------------------------------------------------------------------
        // CALL DEPRECATED FAIL THROW HANDLER
        //
        // Concerns:
        //: 1 That it does *not* throw for an exception build when there is an
        //:   exception pending.
        //:
        //: 2 That it behaves as 'failAbort' for non-exception builds.
        //
        // Plan:
        //: 1 Call 'bsls::Assert::failThrow' from within the destructor of a
        //:   test object on the stack after a throw.
        //:
        //: 2 Call 'bsls::Assert::failAbort' after blocking the signal.
        //
        // Testing:
        //   void failThrow(const char *, const char *, int);
        //   CONCERN: 'bsls::Assert::failThrow' aborts in non-exception build
        //   CONCERN: 'bsls::Assert::failThrow' prints to 'stderr' w/o EXC
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCALL DEPRECATED FAIL THROW HANDLER"
                             "\n==================================\n" );

#if BDE_BUILD_TARGET_EXC
        printf( "\nEXCEPTION BUILD\n" );

        fprintf(stderr, "\nTHE FOLLOWING SHOULD PRINT ON STDERR:\n"
                "BSLS_ASSERT: An uncaught exception is pending;"
                " cannot throw 'bsls_asserttestexception'.\n" );
        fprintf(stderr, "assertion failed: 'failThrow' handler called from "
                "~BadBoy, file f.c, line 9" );


        try {
            BadBoy bad;       // calls 'bsls::Assert::failThrow' on destruction

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
                "Assertion failed: 0 != 0, file myfile.cpp, line 123\n" );

        bsls::Assert::failAbort("0 != 0", "myfile.cpp", 123);

        ASSERT(0 && "Should not be reached");
#endif
      } break;
      case -5: {
        // --------------------------------------------------------------------
        // CALL FAIL SLEEP HANDLER
        //
        // Concerns:
        //: 1 That it does sleep forever.
        //:
        //: 2 That it prints a message to 'stderr'.
        //
        // Plan:
        //: 1 Call 'bsls::Assert::failBySleep'.  Then observe that a diagnostic
        //:   is printed to 'stderr' and the program hangs.
        //
        // Testing:
        //   void failBySleep(const AssertViolation& violation);
        //   CONCERN: 'bsls::Assert::failBySleep' sleeps forever
        //   CONCERN: 'bsls::Assert::failBySleep' prints to 'stderr'
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCALL FAIL SLEEP HANDLER"
                             "\n=======================\n" );

        fprintf( stderr, "THE FOLLOWING SHOULD PRINT ON STDERR"
                 "(BEFORE HANGING):\n"
                 "Assertion failed: 0 != 0, file myfile.cpp, line 123\n" );

        bsls::Assert::failBySleep(bsls::AssertViolation("0 != 0", "myfile.cpp",
                                                      123, "L"));

        ASSERT(0 && "Should not be reached");
      } break;
      case -6: {
        // --------------------------------------------------------------------
        // CALL DEPRECATED FAIL SLEEP HANDLER
        //
        // Concerns:
        //: 1 That it does sleep forever.
        //:
        //: 2 That it prints a message to 'stderr'.
        //
        // Plan:
        //: 1 Call 'bsls::Assert::failSleep'.  Then observe that a diagnostic
        //:   is printed to 'stderr' and the program hangs.
        //
        // Testing:
        //   void failSleep(const char *, const char *, int);
        //   CONCERN: 'bsls::Assert::failSleep' sleeps forever
        //   CONCERN: 'bsls::Assert::failSleep' prints to 'stderr'
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCALL DEPRECATED FAIL SLEEP HANDLER"
                             "\n==================================\n" );

        fprintf( stderr, "THE FOLLOWING SHOULD PRINT ON STDERR"
                 "(BEFORE HANGING):\n"
                 "Assertion failed: 0 != 0, file myfile.cpp, line 123\n" );

        bsls::Assert::failSleep("0 != 0", "myfile.cpp", 123);

        ASSERT(0 && "Should not be reached");
      } break;
      case -7: {
        // --------------------------------------------------------------------
        // RETURNING HANDLER LOG: LIMITS
        //
        // Concerns:
        //: 1 Log messages should stabilize at a period of 2^29.
        //
        // Plan:
        //: 1 In build configurations that allow handlers to return, install a
        //:   violation handler that increments a counter and returns, and an
        //:   instrumented log callback that captures log messages instead of
        //:   printing them.
        //:
        //: 2 Call 'invokeHandler' 2^29 times.  Observe that the logger is
        //:   called once every 2^29 times after that.  (C-1)
        //
        // Testing:
        //   CONCERN: Returning handler log: limits
        // --------------------------------------------------------------------

        if (verbose) printf( "\nRETURNING HANDLER LOG: LIMITS"
                             "\n=============================\n" );

#if        !defined(BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER)           \
        || !defined(BSLS_PLATFORM_CMP_MSVC)

        // 'invokeHandler' cannot return if we are building on Windows with the
        // '...ENABLE_NORETURN...' flag turned on.

        typedef HandlerReturnTest Test;

        bsls::AssertFailureHandlerGuard guard(Test::countingViolationHandler);
        bsls::Log::setLogMessageHandler(Test::countingLogMessageHandler);

        bsls::Types::Int64 iterations = 0;
        for (int triggerCount = 1; triggerCount <= 30; ++triggerCount) {
            while (Test::s_loggerInvocationCount < 2 * triggerCount) {
                bsls::Assert::invokeHandler("maxAllowed < value",
                                            __FILE__,
                                            __LINE__);

                ++iterations;
            }

            if (veryVerbose) {
                P_(iterations)
                P_(triggerCount)
                P_(Test::s_handlerInvocationCount)
                P(Test::s_loggerInvocationCount)
            }

            ASSERT(Test::s_loggerInvocationCount == 2 * triggerCount);
            ASSERT(iterations == Test::s_handlerInvocationCount);
            ASSERT(1 << (triggerCount - 1) == iterations);
        }

        for (int i = 0; i < 3; ++i) {
            iterations = 0;
            bsls::Types::Int64 lastCount = Test::s_loggerInvocationCount;

            while (Test::s_loggerInvocationCount == lastCount) {
                bsls::Assert::invokeHandler("maxAllowed < value",
                                            __FILE__,
                                            __LINE__);

                ++iterations;

                if (veryVerbose && 0 == (iterations & (iterations - 1))) {
                    P_(i)
                    P_(iterations)
                    P_(Test::s_handlerInvocationCount)
                    P(Test::s_loggerInvocationCount)
                }

            }
            LOOP2_ASSERT(lastCount, Test::s_loggerInvocationCount,
                         lastCount + 2 == Test::s_loggerInvocationCount);
            LOOP2_ASSERT(iterations, (1 << 29), iterations == (1 << 29));
        }
#endif
      } break;
      case -8: {
        // --------------------------------------------------------------------
        // RETURNING HANDLER ABORTS
        //
        // Concerns:
        //: 1 With default settings/build, if the provided assertion failure
        //:    handlers does not return, the program is aborted.
        //
        // Plan:
        //: 1 Verify the default behavior by firing an assertion followed by a
        //:   printout that should not be seen.
        //
        // Testing:
        //   void permitOutOfPolicyReturningFailureHandler();
        //   bool abortUponReturningAssertionFailureHandler();
        // --------------------------------------------------------------------

        if (verbose) printf( "\nRETURNING HANDLER ABORTS"
                             "\n========================\n" );

        // Set up the misbehaving handler

        bsls::Assert::setViolationHandler(returningHandler);

        // Fire an assertion

        BSLS_ASSERT(!"This is an intentional assert");

        printf( "This message should not be seen.\n" );

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
//   BSLS_ASSERT_SAFE_IS_ACTIVE
//   BSLS_ASSERT_IS_ACTIVE
//   BSLS_ASSERT_OPT_IS_ACTIVE
//
// Their definition is controlled by a definition (or lack thereof) of the
// following macros:
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
// detail macros in the 'bsls_assert.h' header file, and re-include that
// header.  The supporting component 'bsls_assert_macrotest' provides a header
// that will undefine all of the public macros from both 'bsls_assert.h' and
// 'bsls_review.h', then prepare us to re-include it after changing the above
// build and assert level macros.  Note that '#include'ing a header inside a
// function definition, as we do below, will flag an error for any construct
// that is not supported inside a function definition, such as declaring a
// template or defining a "local" function.  consequently, we must provide a
// deeper "include-guard" inside the component header itself to protect the
// non-macro parts of this component against the repeated inclusion.
//
// Because 'bsls_assert.h' includes 'bsls_review.h' and the configuration of
// 'BSLS_REVIEW' impacts how asserts will behave, we also force 'bsls_review.h'
// to get re-included (and undefine its macros just as the 'bsls_review' test
// driver does) so that we confirm both components work properly in tandem.
//
// For each test iteration that '#include <bsls_assert.h>', each of the macros
// listed above should be undefined, along with each of the following that are
// also defined within this header and the review header:
//   BSLS_ASSERT_SAFE
//   BSLS_ASSERT
//   BSLS_ASSERT_OPT
//   BSLS_ASSERT_ASSERT_IMP
//   BSLS_ASSERT_DISABLED_IMP
//   BSLS_ASSERT_INVOKE
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
// assert macros, flagging an 'X' when that combination should be supported.
// If an assert macro should be enabled, this should be detectable by the
// definition of the corresponding 'BSLS_ASSERT_*_IS_ACTIVE' macro.  Likewise,
// those macros should not be defined unless the configuration is marked.
//
//  Expected test results
//    BDE_BUILD mode      assertion            ASSERT macros
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
//                       LEVEL_ASSERT_OPT       X
//                  X    LEVEL_ASSERT_OPT       X
//             X         LEVEL_ASSERT_OPT       X
//             X    X    LEVEL_ASSERT_OPT       X
//         X             LEVEL_ASSERT_OPT       X
//         X        X    LEVEL_ASSERT_OPT       X
//         X   X         LEVEL_ASSERT_OPT       X
//         X   X    X    LEVEL_ASSERT_OPT       X
//    X                  LEVEL_ASSERT_OPT       X
//    X             X    LEVEL_ASSERT_OPT       X
//    X        X         LEVEL_ASSERT_OPT       X
//    X        X    X    LEVEL_ASSERT_OPT       X
//    X    X             LEVEL_ASSERT_OPT       X
//    X    X        X    LEVEL_ASSERT_OPT       X
//    X    X   X         LEVEL_ASSERT_OPT       X
//    X    X   X    X    LEVEL_ASSERT_OPT       X
//                       LEVEL_ASSERT           X   X
//                  X    LEVEL_ASSERT           X   X
//             X         LEVEL_ASSERT           X   X
//             X    X    LEVEL_ASSERT           X   X
//         X             LEVEL_ASSERT           X   X
//         X        X    LEVEL_ASSERT           X   X
//         X   X         LEVEL_ASSERT           X   X
//         X   X    X    LEVEL_ASSERT           X   X
//    X                  LEVEL_ASSERT           X   X
//    X             X    LEVEL_ASSERT           X   X
//    X        X         LEVEL_ASSERT           X   X
//    X        X    X    LEVEL_ASSERT           X   X
//    X    X             LEVEL_ASSERT           X   X
//    X    X        X    LEVEL_ASSERT           X   X
//    X    X   X         LEVEL_ASSERT           X   X
//    X    X   X    X    LEVEL_ASSERT           X   X
//                       LEVEL_ASSERT_SAFE      X   X   X
//                  X    LEVEL_ASSERT_SAFE      X   X   X
//             X         LEVEL_ASSERT_SAFE      X   X   X
//             X    X    LEVEL_ASSERT_SAFE      X   X   X
//         X             LEVEL_ASSERT_SAFE      X   X   X
//         X        X    LEVEL_ASSERT_SAFE      X   X   X
//         X   X         LEVEL_ASSERT_SAFE      X   X   X
//         X   X    X    LEVEL_ASSERT_SAFE      X   X   X
//    X                  LEVEL_ASSERT_SAFE      X   X   X
//    X             X    LEVEL_ASSERT_SAFE      X   X   X
//    X        X         LEVEL_ASSERT_SAFE      X   X   X
//    X        X    X    LEVEL_ASSERT_SAFE      X   X   X
//    X    X             LEVEL_ASSERT_SAFE      X   X   X
//    X    X        X    LEVEL_ASSERT_SAFE      X   X   X
//    X    X   X         LEVEL_ASSERT_SAFE      X   X   X
//    X    X   X    X    LEVEL_ASSERT_SAFE      X   X   X
//
// Finally we will test the (hopefully orthogonal) effects of setting the
// review level and the assert level explicitly.  This should turn any asserts
// that are above the assert level into reviews, according to the following
// table:
//
//  assertion         assertion          ASSERT macros
//    level             level           OPT     SAFE
//  ----------------- ----------------- --- --- ----
//  LEVEL_NONE        LEVEL_NONE
//  LEVEL_ASSERT_OPT  LEVEL_NONE
//  LEVEL_ASSERT      LEVEL_NONE
//  LEVEL_ASSERT_SAFE LEVEL_NONE
//  LEVEL_NONE        LEVEL_REVIEW_OPT   R
//  LEVEL_ASSERT_OPT  LEVEL_REVIEW_OPT   X
//  LEVEL_ASSERT      LEVEL_REVIEW_OPT   X   X
//  LEVEL_ASSERT_SAFE LEVEL_REVIEW_OPT   X   X   X
//  LEVEL_NONE        LEVEL_REVIEW       R   R
//  LEVEL_ASSERT_OPT  LEVEL_REVIEW       X   R
//  LEVEL_ASSERT      LEVEL_REVIEW       X   X
//  LEVEL_ASSERT_SAFE LEVEL_REVIEW       X   X   X
//  LEVEL_NONE        LEVEL_REVIEW_SAFE  R   R   R
//  LEVEL_ASSERT_OPT  LEVEL_REVIEW_SAFE  X   R   R
//  LEVEL_ASSERT      LEVEL_REVIEW_SAFE  X   X   R
//  LEVEL_ASSERT_SAFE LEVEL_REVIEW_SAFE  X   X   X

namespace
{

#if defined(BDE_BUILD_TARGET_EXC)
struct AssertFailed {
    // This struct contains static functions suitable for registration as an
    // assert handler, and provides a distinct "empty" type that may be thrown
    // from the handler and caught within the test cases below, in order to
    // confirm if the appropriate 'BSLS_ASSERT_*' macros are enabled properly
    // or not.
    BSLS_ASSERT_NORETURN
    static void failMacroTest(const bsls::AssertViolation&) {
        throw AssertFailed();
    }
};
struct ReviewFailed {
    // This struct contains static functions suitable for registration as a
    // review handler, and provides a distinct "empty" type that may be thrown
    // from the handler and caught within the test cases below, in order to
    // confirm if the appropriate 'BSLS_ASSERT_*' macros are enabled properly
    // or not.
    //
    // Note that, without an explicit review level set an ASSERT macro should
    // never trigger a review failure, so the tests for cases not involving
    // review levels let this exception fall out and lead to a test failure.
    static void failReviewMacroTest(const bsls::ReviewViolation &) {
        throw ReviewFailed();
    }
};
#else
    // Without exception support, we cannot fail an assert-test by throwing an
    // exception.  The most practical solution is to simply not compile those
    // tests, so we do not supply an 'AssertFailed' alternative, to be sure to
    // catch any compile-time use of this structure in exception-free builds.
#endif
}  // close unnamed namespace

#undef BSLS_ASSERT_NORETURN

void TestConfigurationMacros()
{

    if (globalVerbose) printf( "\nCONFIGURATION MACROS"
                               "\n====================\n" );

#if !defined(BDE_BUILD_TARGET_EXC)
    if (globalVerbose)
        printf( "\nThis case is not run as it relies "
                "on exception support.\n" );
#else
    if (globalVerbose) printf( "\nWe need to write a running commentary\n" );

    bsls::Assert::setViolationHandler(&AssertFailed::failMacroTest);
    bsls::Review::setViolationHandler(&ReviewFailed::failReviewMacroTest);

//========================== (NO BUILD FLAGS SET) ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

    // (THIS LINE INTENTIONALLY LEFT BLANK)

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false);  }
    catch(AssertFailed)          { ASSERT(true); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//=================================== OPT ===================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//=================================== DBG ===================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//================================= DBG OPT =================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//================================== SAFE ===================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//================================ SAFE OPT =================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//================================ SAFE DBG =================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//============================== SAFE DBG OPT ===============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//================================= SAFE_2 ==================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//=============================== SAFE_2 OPT ================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BDE_BUILD_TARGET_SAFE_2

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//=============================== SAFE_2 DBG ================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_SAFE_2

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//============================= SAFE_2 DBG OPT ==============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//=============================== SAFE_2 SAFE ===============================//


// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//============================= SAFE_2 SAFE OPT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//============================= SAFE_2 SAFE DBG =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//=========================== SAFE_2 SAFE DBG OPT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//- - - - - - - - - - - - - - - - LEVEL_NONE - - - - - - - - - - - - - - - - //

//=============================== LEVEL_NONE ================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }


//============================= OPT LEVEL_NONE ==============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }


//============================= DBG LEVEL_NONE ==============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }


//=========================== DBG OPT LEVEL_NONE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }


//============================= SAFE LEVEL_NONE =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }


//=========================== SAFE OPT LEVEL_NONE ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }


//=========================== SAFE DBG LEVEL_NONE ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }


//========================= SAFE DBG OPT LEVEL_NONE =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }


//============================ SAFE_2 LEVEL_NONE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }


//========================== SAFE_2 OPT LEVEL_NONE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }


//========================== SAFE_2 DBG LEVEL_NONE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }


//======================== SAFE_2 DBG OPT LEVEL_NONE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }


//========================= SAFE_2 SAFE LEVEL_NONE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }


//======================= SAFE_2 SAFE OPT LEVEL_NONE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }


//======================= SAFE_2 SAFE DBG LEVEL_NONE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }


//===================== SAFE_2 SAFE DBG OPT LEVEL_NONE ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }


//- - - - - - - - - - - - - -  LEVEL_ASSERT_OPT - - - - - - - - - - - - - - -//

//============================ LEVEL_ASSERT_OPT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//========================== OPT LEVEL_ASSERT_OPT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//========================== DBG LEVEL_ASSERT_OPT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//======================== DBG OPT LEVEL_ASSERT_OPT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//========================== SAFE LEVEL_ASSERT_OPT ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//======================== SAFE OPT LEVEL_ASSERT_OPT ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//======================== SAFE DBG LEVEL_ASSERT_OPT ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//====================== SAFE DBG OPT LEVEL_ASSERT_OPT ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//========================= SAFE_2 LEVEL_ASSERT_OPT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//======================= SAFE_2 OPT LEVEL_ASSERT_OPT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//======================= SAFE_2 DBG LEVEL_ASSERT_OPT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//===================== SAFE_2 DBG OPT LEVEL_ASSERT_OPT =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//====================== SAFE_2 SAFE LEVEL_ASSERT_OPT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//==================== SAFE_2 SAFE OPT LEVEL_ASSERT_OPT =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//==================== SAFE_2 SAFE DBG LEVEL_ASSERT_OPT =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//================== SAFE_2 SAFE DBG OPT LEVEL_ASSERT_OPT ===================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//- - - - - - - - - - - - - - -  LEVEL_ASSERT - - - - - - - - - - - - - - - -//

//============================== LEVEL_ASSERT ===============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//============================ OPT LEVEL_ASSERT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//============================ DBG LEVEL_ASSERT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//========================== DBG OPT LEVEL_ASSERT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//============================ SAFE LEVEL_ASSERT ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//========================== SAFE OPT LEVEL_ASSERT ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//========================== SAFE DBG LEVEL_ASSERT ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//======================== SAFE DBG OPT LEVEL_ASSERT ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//=========================== SAFE_2 LEVEL_ASSERT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//========================= SAFE_2 OPT LEVEL_ASSERT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//========================= SAFE_2 DBG LEVEL_ASSERT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//======================= SAFE_2 DBG OPT LEVEL_ASSERT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//======================== SAFE_2 SAFE LEVEL_ASSERT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//====================== SAFE_2 SAFE OPT LEVEL_ASSERT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//====================== SAFE_2 SAFE DBG LEVEL_ASSERT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//==================== SAFE_2 SAFE DBG OPT LEVEL_ASSERT =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//- - - - - - - - - - - - - -  LEVEL_ASSERT_SAFE - - - - - - - - - - - - - - //

//============================ LEVEL_ASSERT_SAFE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//========================== OPT LEVEL_ASSERT_SAFE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//========================== DBG LEVEL_ASSERT_SAFE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//======================== DBG OPT LEVEL_ASSERT_SAFE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//========================= SAFE LEVEL_ASSERT_SAFE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//======================= SAFE OPT LEVEL_ASSERT_SAFE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//======================= SAFE DBG LEVEL_ASSERT_SAFE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//===================== SAFE DBG OPT LEVEL_ASSERT_SAFE ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//======================== SAFE_2 LEVEL_ASSERT_SAFE =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//====================== SAFE_2 OPT LEVEL_ASSERT_SAFE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//====================== SAFE_2 DBG LEVEL_ASSERT_SAFE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//==================== SAFE_2 DBG OPT LEVEL_ASSERT_SAFE =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//====================== SAFE_2 SAFE LEVEL_ASSERT_SAFE ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//==================== SAFE_2 SAFE OPT LEVEL_ASSERT_SAFE ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//==================== SAFE_2 SAFE DBG LEVEL_ASSERT_SAFE ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//================== SAFE_2 SAFE DBG OPT LEVEL_ASSERT_SAFE ==================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 3 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }


//- - - - - - - - - - ASSERT_LEVEL and REVIEW_LEVEL_NONE - - - - - - - - - - //

//=================== ASSERT_LEVEL_NONE REVIEW_LEVEL_NONE ===================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 6 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false);  }

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false);  }
    catch(ReviewFailed)               { ASSERT(true);  }

//================ ASSERT_LEVEL_ASSERT_OPT REVIEW_LEVEL_NONE ================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 6 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false);  }

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false);  }
    catch(ReviewFailed)               { ASSERT(true);  }

//================== ASSERT_LEVEL_ASSERT REVIEW_LEVEL_NONE ==================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 6 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false);  }

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false);  }
    catch(ReviewFailed)               { ASSERT(true);  }

//=============== ASSERT_LEVEL_ASSERT_SAFE REVIEW_LEVEL_NONE ================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 6 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false);  }
    catch(AssertFailed)          { ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false);  }

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false);  }
    catch(ReviewFailed)               { ASSERT(true);  }

//- - - - - - - -  ASSERT_LEVEL and REVIEW_LEVEL_REVIEW_OPT - - - - - - - - -//

//================ ASSERT_LEVEL_NONE REVIEW_LEVEL_REVIEW_OPT ================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 6 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false);  }

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false);  }
    catch(ReviewFailed)               { ASSERT(true);  }

//============= ASSERT_LEVEL_ASSERT_OPT REVIEW_LEVEL_REVIEW_OPT =============//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 6 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false);  }

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false);  }
    catch(ReviewFailed)               { ASSERT(true);  }

//=============== ASSERT_LEVEL_ASSERT REVIEW_LEVEL_REVIEW_OPT ===============//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 6 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false);  }

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false);  }
    catch(ReviewFailed)               { ASSERT(true);  }

//============ ASSERT_LEVEL_ASSERT_SAFE REVIEW_LEVEL_REVIEW_OPT =============//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 6 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false);  }
    catch(AssertFailed)          { ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false);  }

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(true); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false);  }
    catch(ReviewFailed)               { ASSERT(true);  }

//- - - - - - - - -  ASSERT_LEVEL and REVIEW_LEVEL_REVIEW - - - - - - - - - -//

//================== ASSERT_LEVEL_NONE REVIEW_LEVEL_REVIEW ==================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 6 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false);  }

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false);  }
    catch(ReviewFailed)               { ASSERT(true);  }

//=============== ASSERT_LEVEL_ASSERT_OPT REVIEW_LEVEL_REVIEW ===============//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 6 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false);  }

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false);  }
    catch(ReviewFailed)               { ASSERT(true);  }

//================= ASSERT_LEVEL_ASSERT REVIEW_LEVEL_REVIEW =================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 6 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false);  }

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false);  }
    catch(ReviewFailed)               { ASSERT(true);  }

//============== ASSERT_LEVEL_ASSERT_SAFE REVIEW_LEVEL_REVIEW ===============//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 6 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false);  }
    catch(AssertFailed)          { ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false);  }

    try { BSLS_REVIEW_SAFE(false); ASSERT(true);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false);  }
    catch(ReviewFailed)               { ASSERT(true);  }

//- - - - - - - -  ASSERT_LEVEL and REVIEW_LEVEL_REVIEW_SAFE - - - - - - - - //

//=============== ASSERT_LEVEL_NONE REVIEW_LEVEL_REVIEW_SAFE ================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 6 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false);  }

    try { BSLS_REVIEW_SAFE(false); ASSERT(false);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false);  }
    catch(ReviewFailed)               { ASSERT(true);  }

//============ ASSERT_LEVEL_ASSERT_OPT REVIEW_LEVEL_REVIEW_SAFE =============//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 6 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false);  }

    try { BSLS_REVIEW_SAFE(false); ASSERT(false);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false);  }
    catch(ReviewFailed)               { ASSERT(true);  }

//============== ASSERT_LEVEL_ASSERT REVIEW_LEVEL_REVIEW_SAFE ===============//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 6 'IS_ACTIVE' macros.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false);  }

    try { BSLS_REVIEW_SAFE(false); ASSERT(false);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false);  }
    catch(ReviewFailed)               { ASSERT(true);  }

//============ ASSERT_LEVEL_ASSERT_SAFE REVIEW_LEVEL_REVIEW_SAFE ============//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the values of the 6 'IS_ACTIVE' macros.

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_SAFE(false); ASSERT(false);  }
    catch(AssertFailed)          { ASSERT(true); }
    catch(ReviewFailed)          { ASSERT(false); }

    try { BSLS_ASSERT(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(true);  }
    catch(ReviewFailed)          { ASSERT(false);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false);  }

    try { BSLS_REVIEW_SAFE(false); ASSERT(false);  }
    catch(AssertFailed)          { ASSERT(false); }
    catch(ReviewFailed)          { ASSERT(true); }

    try { BSLS_REVIEW(false);      ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_OPT(false);  ASSERT(false); }
    catch(AssertFailed)          { ASSERT(false);  }
    catch(ReviewFailed)          { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false);  }
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
