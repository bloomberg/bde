// bsls_assert.t.cpp                                                  -*-C++-*-
#include <bsls_assert.h>

#include <bsls_asserttestexception.h>
#include <bsls_bsltestutil.h>
#include <bsls_log.h>
#include <bsls_logseverity.h>
#include <bsls_unspecifiedbool.h>
#include <bsls_types.h>

// Include 'cassert' to make sure no macros conflict between 'bsls_assert.h'
// and 'cassert'.  This test driver does *not* invoke 'assert(expression)'.
#include <cassert>

#ifndef BDE_BUILD_TARGET_EXC
#include <csetjmp>   // 'setjmp' 'longjmp'
#endif
#include <cstdio>    // 'fprintf'
#include <cstdlib>   // 'atoi'
#include <cstring>   // 'strcmp', 'strcpy'
#include <exception> // 'exception'
#include <iostream>  // 'ostream', 'cerr' for usage examples

#ifdef BSLS_PLATFORM_OS_UNIX
#include <signal.h>
#endif

#ifndef BDE_BUILD_TARGET_EXC
#if defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)                      \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT)

#include <type_traits>

static_assert(std::is_trivially_destructible<
                          BloombergLP::bsls::AssertViolation>::value,
                                            "Can use with 'setjmp'/'longjmp'");
#endif
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
// [ 7] BSLS_ASSERT_IS_ACTIVE
// [ 7] BSLS_ASSERT_IS_ASSUME
// [ 7] BSLS_ASSERT_IS_REVIEW
// [ 7] BSLS_ASSERT_OPT_IS_ACTIVE
// [ 7] BSLS_ASSERT_OPT_IS_ASSUME
// [ 7] BSLS_ASSERT_OPT_IS_REVIEW
// [ 7] BSLS_ASSERT_SAFE_IS_ACTIVE
// [ 7] BSLS_ASSERT_SAFE_IS_ASSUME
// [ 7] BSLS_ASSERT_SAFE_IS_REVIEW
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
// [13] USAGE EXAMPLE: Using Assert Macros
// [14] USAGE EXAMPLE: Invoking an assert handler directly
// [15] USAGE EXAMPLE: Using Administration Functions
// [15] USAGE EXAMPLE: Installing Prefabricated Assert-Handlers
// [16] USAGE EXAMPLE: Creating Your Own Assert-Handler
// [17] USAGE EXAMPLE: Using Scoped Guard
// [18] USAGE EXAMPLE: Using "ASSERT" with 'BDE_BUILD_TARGET_SAFE_2'
// [19] USAGE EXAMPLE: Conditional Compilation
// [20] USAGE EXAMPLE: Conditional Compilation of Support Functions
// [21] USAGE EXAMPLE: Conditional Compilation of Support Code
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
// [11] CONCERN: 'BSLS_ASSERTIMPUTIL_FILE' interaction
// [12] CONCERN: 'constexpr' interaction

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

#define STRINGIFY2(...) "" #__VA_ARGS__
#define STRINGIFY(a) STRINGIFY2(a)

//=============================================================================
//                    GLOBAL CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static bool verbose         = false;
static bool veryVerbose     = false;
static bool veryVeryVerbose = false;

static bool        globalAssertFiredFlag = false;
static bool        globalReviewFiredFlag = false;
static bool        globalLegacyAssertFiredFlag = false;
static const char *globalText = "";
static const char *globalFile = "";
static int         globalLine = -1;
static const char *globalLevel = "";

// When using language contracts the contract level is not available to the
// violation handler, so it will not be set.
#ifdef BSLS_ASSERT_USE_CONTRACTS
static bool checkLevels = false;
#else
static bool checkLevels = true;
#endif

// BDE_VERIFY pragma: -FE01 // at this level in the package hierarchy in a test
                            // driver we want to avoid std::exception

#ifndef BDE_BUILD_TARGET_EXC
static std::jmp_buf jmpContext;  // for the 'ASSERTION_TEST_BEGIN' macro
#endif

//=============================================================================
//                  GLOBAL HELPER MACROS FOR TESTING
//-----------------------------------------------------------------------------

#if (defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE)   ||                              \
     defined(BSLS_ASSERT_LEVEL_ASSERT)        ||                              \
     defined(BSLS_ASSERT_LEVEL_ASSERT_OPT)    ||                              \
     defined(BSLS_ASSERT_LEVEL_NONE)          ||                              \
     defined(BSLS_ASSERT_LEVEL_ASSUME_OPT)    ||                              \
     defined(BSLS_ASSERT_LEVEL_ASSUME_ASSERT) ||                              \
     defined(BSLS_ASSERT_LEVEL_ASSUME_SAFE))
    #define IS_BSLS_ASSERT_MODE_FLAG_DEFINED 1
#else
    #define IS_BSLS_ASSERT_MODE_FLAG_DEFINED 0
#endif

#ifdef BDE_BUILD_TARGET_EXC
#define ASSERTION_TEST_BEGIN                                                  \
        try {

#define ASSERTION_TEST_END                                                    \
        } catch (const std::exception&) {                                     \
            if (veryVerbose) printf( "\nException caught." );                 \
        }
#else
#define ASSERTION_TEST_BEGIN                                                  \
        if (0 == setjmp(jmpContext)) {                                        \
            if (veryVerbose) printf( "\nJump context saved." );
#define ASSERTION_TEST_END                                                    \
            if (veryVerbose) printf( "\nJump context restored." );            \
        }
#endif

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#if !defined(BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER)

// 'invokeHandler' cannot return if we are building with the
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
        // 'veryVeryVerbose' is 'true' then log the specified 'severity',
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
    ASSERTV(severity, bsls::LogSeverity::e_FATAL == severity);
    ASSERT(s_loggerInvocationCount < 2);

    if (veryVeryVerbose) {
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
    ASSERTV(severity, bsls::LogSeverity::e_FATAL == severity);

    ++s_loggerInvocationCount;

    if (veryVeryVerbose) {
        bsls::Log::stdoutMessageHandler(severity, file, line, message);
    }
}

#endif

static void globalReset()
{
    if (veryVeryVerbose)
        printf( "*** globalReset()\n" );

    globalAssertFiredFlag = false;
    globalReviewFiredFlag = false;
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

BSLS_ANNOTATION_NORETURN
static void testDriverHandler(const bsls::AssertViolation& violation)
    // Set the 'globalAssertFiredFlag' to 'true', the 'globalReviewFiredFlag'
    // to 'false', the 'globalLegacyAssertFiredFlag' to 'false', and store the
    // expression 'text', 'file' name, 'line' number, and 'assertLlevel' values
    // from the specified 'violation' in 'globalText', 'globalFile',
    // 'globalLine', and 'globalLevel' respectively.  Then throw an
    // 'std::exception' object provided that 'BDE_BUILD_TARGET_EXC' is defined;
    // otherwise, abort the program.
{
    if (veryVeryVerbose) {
        printf( "*** testDriverHandler: ");
        P_(violation.comment());
        P_(violation.fileName());
        P_(violation.lineNumber());
        P(violation.assertLevel());
    }

    globalAssertFiredFlag = true;
    globalReviewFiredFlag = false;
    globalLegacyAssertFiredFlag = false;
    globalText = violation.comment();
    globalFile = violation.fileName();
    globalLine = violation.lineNumber();
    globalLevel = violation.assertLevel();

#ifdef BDE_BUILD_TARGET_EXC
    throw std::exception();
#else
    std::longjmp(jmpContext, 1);
#endif
}

BSLS_ANNOTATION_NORETURN
static void testDriverReviewHandler(const bsls::ReviewViolation& violation)
    // Set the 'globalAssertFiredFlag' to 'true', the 'globalReviewFiredFlag'
    // to 'true', 'globalLegacyAssertFiredFlag' to 'false', and store the
    // expression 'text', 'file' name, 'line' number, and 'assertLlevel' values
    // from the specified 'violation' in 'globalText', 'globalFile',
    // 'globalLine', and 'globalLevel' respectively.  Then throw an
    // 'std::exception' object provided that 'BDE_BUILD_TARGET_EXC' is defined;
    // otherwise, abort the program.
{
    if (veryVeryVerbose) {
        printf( "*** testReviewDriverHandler: ");
        P_(violation.comment());
        P_(violation.fileName());
        P_(violation.lineNumber());
        P(violation.reviewLevel());
    }

    globalAssertFiredFlag = true;
    globalReviewFiredFlag = true;
    globalLegacyAssertFiredFlag = false;
    globalText = violation.comment();
    globalFile = violation.fileName();
    globalLine = violation.lineNumber();
    globalLevel = violation.reviewLevel();

#ifdef BDE_BUILD_TARGET_EXC
    throw std::exception();
#else
    std::longjmp(jmpContext, 1);
#endif
}

//-----------------------------------------------------------------------------

BSLS_ANNOTATION_NORETURN
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
    if (veryVeryVerbose) {
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
    std::longjmp(jmpContext, 1);
#endif
}

//-----------------------------------------------------------------------------

static void returningHandler(const bsls::AssertViolation& violation)
    // Print the 'comment', 'file', 'line' and 'assertLevel' from the specified
    // 'violation' to standard output if 'veryVeryVerbose' is non-zero and
    // return.  NOTE THAT this handler is against Bloomberg default policy and
    // under normal circumstances such a handler will result in termination of
    // the program anyway.
{
    if (veryVeryVerbose) {
        printf( "*** returningHandler: ");
        P_(violation.comment());
        P_(violation.fileName());
        P_(violation.lineNumber());
        P(violation.assertLevel());
    }

    globalAssertFiredFlag = true;
    globalReviewFiredFlag = false;
    globalLegacyAssertFiredFlag = false;
    globalText = violation.comment();
    globalFile = violation.fileName();
    globalLine = violation.lineNumber();
    globalLevel = violation.assertLevel();
}

//-----------------------------------------------------------------------------

BSLS_ANNOTATION_NORETURN
static void testDriverPrint(const bsls::AssertViolation& violation)
    // Format, in verbose mode, the expression 'comment', 'file' name, 'line'
    // number, and 'assertLevel' from the specified 'violation' in the same way
    // as the 'bsls::Assert::failByAbort' assertion-failure handler function
    // might, but on 'cout' instead of 'cerr'.  Then throw an 'std::exception'
    // object provided that 'BDE_BUILD_TARGET_EXC' is defined; otherwise, abort
    // the program.

{
    if (veryVeryVerbose) {
        printf( "*** testDriverPrint: ");
        P_(violation.comment());
        P_(violation.fileName());
        P_(violation.lineNumber());
        P(violation.assertLevel());
    }

    if (veryVerbose) {
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
    std::longjmp(jmpContext, 1);
#endif
}

//-----------------------------------------------------------------------------

struct ExplicitBool
{
  bool d_value;

  ExplicitBool(bool value)
  : d_value(value)
  {
  }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
  explicit
  operator bool() const {
    return d_value;
  }
#else
  typedef bsls::UnspecifiedBool<ExplicitBool>::BoolType BoolType;

  operator BoolType() const {
    return bsls::UnspecifiedBool<ExplicitBool>::makeValue(d_value);
  }
#endif

};

//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct BadBoy {
    // Bogus 'struct' used for testing: calls 'bsls::Assert::failByThrow' on
    // destruction to ensure that it does not re-throw with an exception
    // pending (see case -2).

    BadBoy() {
        if (veryVerbose) printf( "BadBoy Created!\n" );
    }

    ~BadBoy() BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false) {
        if (veryVerbose) printf( "BadBoy Destroyed!\n" );
        bsls::Assert::failByThrow(bsls::AssertViolation(
                                   "'failByThrow' handler called from ~BadBoy",
                                   "f.c",
                                   9,"L"));
     }
};

// Declaration of function that must appear after main in order to test the
// configuration macros.
void TestConfigurationMacros();

struct ConstexprFunctions {

    BSLS_KEYWORD_CONSTEXPR_CPP17
    static int narrowConstexprFunction(int x)
        // Return the specified 'x', the behavior is undefined if 'x < 0'.
    {

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
        BSLS_ASSERT_OPT(x >= 0);
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
        BSLS_ASSERT(x >= 0);
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
        BSLS_ASSERT_SAFE(x >= 0);
#endif

        if (x < 0) {
            BSLS_ASSERT_INVOKE("x < 0");
            BSLS_ASSERT_INVOKE_NORETURN("x < 0");
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

///Usage
///-----
// The following examples illustrate (1) when to use each of the three kinds of
// (BSLS) "ASSERT" macros, (2) when and how to call the 'invokeHandler' method
// directly, (3) how to configure, at runtime, the behavior resulting from an
// assertion failure using "off-the-shelf" handler methods, (4) how to create
// your own custom assertion-failure handler function, (5) proper use of
// 'bsls::AssertFailureHandlerGuard' to install, temporarily, an
// exception-producing assert handler, (6) how "ASSERT" macros would be used in
// conjunction with portions of the source code (affecting binary
// compatibility) that are incorporated only when 'BDE_BUILD_TARGET_SAFE_2' is
// defined, and (7) how assertion predicates (e.g.,
// 'BSLS_ASSERT_SAFE_IS_ACTIVE') are used to conditionally compile additional
// (redundant) defensive source code (not affecting binary compatibility)
// precisely when the corresponding (BSLS) "ASSERT" macro (e.g.,
// 'BSLS_ASSERT_SAFE') is active.
//
namespace usage_example_assert_1 {
///Example 1: Using 'BSLS_ASSERT', 'BSLS_ASSERT_SAFE', and 'BSLS_ASSERT_OPT'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component provides three different variants of (BSLS) "ASSERT" macros.
// This first usage example illustrates how one might select each of the
// particular variants, based on the runtime cost of the defensive check
// relative to that of the useful work being done.
//
// Use of the 'BSLS_ASSERT_SAFE' macro is often appropriate when the defensive
// check occurs within the body of an 'inline' function.  The
// 'BSLS_ASSERT_SAFE' macro minimizes the impact on runtime performance as it
// is instantiated only when requested (i.e., by building in "safe mode").  For
// example, consider a light-weight point class 'Kpoint' that maintains 'x' and
// 'y' coordinates in the range '[-1000 .. 1000]':
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
    (void)d_x;
    (void)d_y;
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
    (void)loadFactor;
    BSLS_ASSERT(0 < loadFactor);

    // ...
}
//..
// In some cases, the runtime cost of checking is always negligible when
// compared with the runtime cost of performing the useful work; moreover, the
// consequences of continuing in an undefined state for certain applications
// could be catastrophic.  Instead of using 'BSLS_ASSERT' in such cases, we
// might consider using 'BSLS_ASSERT_OPT'.  For example, suppose we have a
// financial application class 'TradingSystem' that performs trades:
//..
// my_tradingsystem.h
// ...

class TradingSystem {
    // ...
  public:
    // ...
//..
// Further suppose that there is a particular method 'executeTrade' that takes,
// as a scaling factor, an integer that must be a multiple of 100 or the
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
// follows:
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
}  // close namespace usage_example_assert_1
namespace usage_example_assert_2 {
///Example 2: When and How to Call the 'invokeHandler' Method Directly
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// There *may* be times (but this is yet to be demonstrated) when we might
// reasonably choose to unconditionally invoke the currently installed
// assertion-failure handler directly -- i.e., instead of via one of the three
// (BSLS) "ASSERT" macros provided in this component.  Suppose that we are
// currently in the body of some function 'someFunc' and, for whatever reason,
// feel compelled to invoke the currently installed assertion-failure handler
// based on some criteria other than the current build mode.
// 'BSLS_ASSERT_INVOKE' is provided for this purpose.  The call might look as
// follows:
//..
void someFunc(bool a, bool b, bool c)
{
    bool someCondition = a && b && !c;

    if (someCondition) {
        BSLS_ASSERT_INVOKE("Bad News");
    }
}
//..
// If presented with invalid arguments, 'someFunc' (above) will produce output
// similar to the following:
//..
//  Assertion failed: Bad News, file bsls_assert.t.cpp, line 609
//  Abort (core dumped)
//..
// If a piece of code needs to be guaranteed to not return, the additional
// macro 'BSLS_ASSERT_INVOKE_NORETURN' is also available.  It behaves the same
// way as 'BSLS_ASSERT_INVOKE', but if the installed handler *does* return
// 'failByAbort' will be immediately called.  On supported platforms it is
// marked appropriately to not return to support compiler requirements and
// static analysis tools.
//
}  // close namespace usage_example_assert_2
namespace usage_example_assert_3 {
///Example 3: Runtime Configuration of the 'bsls::Assert' Facility
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
assert(&bsls::Assert::failByAbort == bsls::Assert::violationHandler());
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
assert(&bsls::Assert::failBySleep == bsls::Assert::violationHandler());
//..
// Note that if we were to explicitly invoke the current assertion-failure
// handler as follows:
//..
//  BSLS_ASSERT_INVOKE("message");  // This will hang!
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

    assert(&bsls::Assert::failByAbort != bsls::Assert::violationHandler());

    assert(&bsls::Assert::failBySleep == bsls::Assert::violationHandler());
}
//..
//
}  // close namespace usage_example_assert_3
namespace usage_example_assert_4 {
///Example 4: Creating a Custom Assertion Handler
/// - - - - - - - - - - - - - - - - - - - - - - -
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

static
void ourFailureHandler(const bsls::AssertViolation& violation)
    // Print the expression 'comment', 'file' name, and 'line' number from
    // the specified 'violation' to 'stdout' as a comma-separated list,
    // replacing null string-argument values with empty strings (unless
    // printing has been disabled by the 'globalEnableOurPrintingFlag'
    // variable), then unconditionally abort.
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
//..
// At the top level of our application we have the following:
//..
void ourMain()
{
//..
// First, let's observe that we can assign this new function to a function
// pointer of type 'bsls::Assert::Handler':
//..
bsls::Assert::ViolationHandler f = &ourFailureHandler;
(void)f;
//..
// Now we can install it just as we would any other handler:
//..
bsls::Assert::setViolationHandler(&ourFailureHandler);
//..
// We can now invoke the default handler directly:
//..
//  BSLS_ASSERT_INVOKE("str1");
}
//..
// With the resulting output something like as follows:
//..
//  str1, my_file.cpp, 17
//  Abort (core dumped)
//..
//
}  // close namespace usage_example_assert_4
namespace usage_example_assert_5 {
///Example 5: Using the 'bsls::AssertFailureHandlerGuard'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
    enum { GOOD = 0, BAD } result = GOOD; (void) verboseFlag;
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
    assert(&bsls::Assert::failByAbort == bsls::Assert::violationHandler());

    bsls::AssertFailureHandlerGuard guard(&bsls::Assert::failByThrow);

    assert(&bsls::Assert::failByThrow == bsls::Assert::violationHandler());
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
//  Assertion failed: 0 <= n, file bsls_assert.t.cpp, line 500
//  Abort (core dumped)
//..
// Finally note that the 'bsls::AssertFailureHandlerGuard' is not thread-aware.
// In particular, a guard that is created in one thread will also affect the
// failure handlers that are used in other threads.  Care should be taken when
// using this guard when more than a single thread is executing.
//
}  // close namespace usage_example_assert_5
namespace usage_example_assert_6 {
///Example 6: Using (BSLS) "ASSERT" Macros Along With 'BDE_BUILD_TARGET_SAFE_2'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Recall that assertions do not affect binary compatibility; however, software
// built with 'BDE_BUILD_TARGET_SAFE_2' defined need not be binary compatible
// with software built otherwise.  In this example, we look at how we might use
// the (BSLS) "ASSERT" family of macros in conjunction with code that is
// incorporated (at compile time) only when 'BDE_BUILD_TARGET_SAFE_2' is
// defined.
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
    List_Link **d_current_p;
#ifdef BDE_BUILD_TARGET_SAFE_2
    List *d_parent_p;               // Exists only in "safe 2 mode".
#endif
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
//..
// Finally we define the 'List' class itself with most of the operations
// elided; the methods of particular interest here are 'begin' and 'insert':
//..

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
        // Output the contents of this list to 'stdout'.
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
    a.insert(bIt, 4);       // Oops!  Should have been: 'b.insert(bIt, 4);'
    a.insert(bIt, 5);       // Oops!    "     "     "   '    "     "   5  '
    a.insert(bIt, 6);       // Oops!    "     "     "   '    "     "   6  '

    if (printFlag) {
        std::printf( "a = "); a.print();
        std::printf( "b = "); b.print();
    }
}
//..
// In the example above, we have "accidentally" passed the iterator 'bIt'
// obtained from 'List' object 'b' into the 'insert' method for 'List' object
// 'a'.  The resulting undefined behavior (in other than "safe 2 mode") might
// produce output that looks as follows:
//..
//  a = [ 3 2 1 ]
//  a = [ 3 2 1 ]
//  b = [ 6 5 4 ]
//..
// If the same 'sillyFunc' were compiled in "safe 2 mode" (i.e., with
// 'BDE_BUILD_TARGET_SAFE_2' defined) the undefined behavior would be detected
// and the output would, by default, look more like the following:
//..
//  a = [ 3 2 1 ]
//  FATAL my_list.cpp:56 Assertion failed: this == position.d_parent_p
//  Abort (core dumped)
//..
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
//: o For 'BSLS_ASSERT_SAFE', use 'BSLS_ASSERT_SAFE_IS_ACTIVE'.
//:
//: o For 'BSLS_ASSERT', use 'BSLS_ASSERT_IS_ACTIVE'.
//:
//: o For 'BSLS_ASSERT_OPT', use 'BSLS_ASSERT_OPT_IS_ACTIVE'.
//
// Suppose that we have a class such as 'MyDate' (below) that, except for
// checking its invariants, would have a trivial destructor.  By not declaring
// a destructor at all, we may realize performance advantages, but then we lose
// the ability to validate our invariants in "debug" or "safe" mode.  What we
// want to do is to declare (and later define) the destructor in precisely
// those build modes for which we would want to assert invariants.
//
// An elided class 'MyDate', which is based on a serial-date implementation, is
// provided for reference:
//..
class MyDate {
    // This class implements a value-semantic "date" type representing
    // valid date values in the range '[ 0001Jan01 .. 9999Dec31 ]'.

    // DATA
    int d_serialDate;  // sequential representation within a valid range

  public:
     // CLASS METHODS

     // ...

     // CREATORS
     MyDate();
         // Create a 'MyDate' object having the value '0001Jan01'.

     // ...

     MyDate(const MyDate& original);
         // Create a 'MyDate' object having the same value as the specified
         // 'original' object.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
     ~MyDate();
         // Destroy this object.  Note that in some build modes the
         // destructor generated by the compiler is trivial.
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
    (void)d_serialDate;
}

inline
MyDate::MyDate(const MyDate& original)
: d_serialDate(original.d_serialDate)
{
}

// ...

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
inline
MyDate::~MyDate()
{
    BSLS_ASSERT_SAFE(1 <= d_serialDate);             // 0001Jan01
    BSLS_ASSERT_SAFE(     d_serialDate <= 3652061);  // 9999Dec31
}
#endif

// ...
//..
// In practice, however, we would probably implement an 'isValidSerialDate'
// method in a lower-level utility class, e.g., 'MyDateImpUtil', leading to
// code that is more fine-grained, modular, and hierarchically reusable:
//..
struct MyDateImpUtil {
    static bool isValidSerialDate(int d_date);
        // Return 'true' if the specified 'd_date' represents a valid date
        // value, and 'false' otherwise.
};

inline
bool MyDateImpUtil::isValidSerialDate(int d_date)
{
    return 1 <= d_date && d_date <= 3652061;
}
//..
// Like other aspects of 'BSLS_ASSERT_SAFE', the example above violates the
// one-definition rule for mixed-mode builds.  Note that all code conditionally
// compiled based on 'BSLS_ASSERT_SAFE_IS_ACTIVE', 'BSLS_ASSERT_IS_ACTIVE', and
// 'BSLS_ASSERT_OPT_IS_ACTIVE' should be binary compatible for mixed-mode
// builds.  If the conditionally-compiled code would not be binary compatible,
// use 'BDE_BUILD_TARGET_SAFE_2' instead.
//
// WARNING - In practice, declaring a destructor in some build modes but not
// others has led to subtle and difficult-to-diagnose failures.  DON'T DO IT!
//
// Finally, in very rare cases, we may want to put in (redundant) defensive
// code (in the spirit of 'BSLS_ASSERT_OPT') that is not part of the
// component-level contract, yet (1) is known to have negligible runtime cost
// and (2) is deemed to be so important as to be necessary even for optimized
// builds.
//
// For example, consider again the 'MyDate' class above that now also declares
// a non-'inline' 'print' method to format the current date value in some
// human-readable, but otherwise unspecified format:
//..
// xyza_mydate.h
// ...
namespace usage_example_assert_7_b {
class MyDate {
    // ...

    // DATA
    int d_serialDate;  // sequential representation within a valid range

  public:
    // ...
    // ACCESSORS
    // ...

    std::ostream& print(std::ostream& stream, ...) const;
        // Write the value of this object to the specified output 'stream'
        // in some human-readable format, and return a reference to
        // 'stream'.  Optionally specify ...

    // ...

};
//..
// Successfully writing bad data is among the most insidious of bugs, because a
// latent error can persist and not be discovered until long after the program
// terminates.  Writing the value of a corrupted 'MyDate' object in a
// *machine-readable* (binary) format is an error so serious as to warrant
// invoking
//..
void testFunction(int d_serialDate) {
    BSLS_ASSERT_OPT(MyDateImpUtil::isValidSerialDate(d_serialDate));
}
//..
// each time we attempt the output operation; however, printing the value in a
// human-readable format intended primarily for debugging purposes is another
// matter.  In anything other than a safe build (which in this case would
// enforce essentially all method preconditions), it would be unfortunate if a
// developer, knowing that there was a problem involving the use of 'MyDate',
// inserted print statements to identify that problem, only to have the 'print'
// method itself ruthlessly invoke the assert handler, likely terminating the
// process).  Moreover, it may also be unsafe even to attempt to format the
// value of a 'MyDate' object whose 'd_serialDate' value violates its
// invariants (e.g., due to a static table lookup).  In such cases we may, as
// sympathetic library developers, choose to implement different undefined
// (undocumented) redundant defensive behaviors, depending on the desired level
// of assertions:
//..
// xyza_mydate.cpp
// ...
// #include <xyza_mydateimputil.h>
// ...

std::ostream& MyDate::print(std::ostream& stream, ...) const
{
    // BSLS_ASSERT(/* any *argument* preconditions for this function */);

    // Handle case where the invariants have been violated.

    (void)d_serialDate;
#ifdef BSLS_ASSERT_OPT_IS_ACTIVE
    // Note that if 'BSLS_ASSERT_LEVEL_NONE' has been set, this code --
    // along with all 'BSLS_ASSERT_OPT' macros -- will not instantiate,
    // enabling us to verify that the combined runtime overhead of all such
    // (redundant) defensive code is at worst negligible, if not
    // unmeasurable.

    if (!MyDateImpUtil::isValidSerialDate(d_serialDate)) {

        // Our invariant is corrupted.

#ifdef BSLS_ASSERT_IS_ACTIVE
        // Providing debugging information in this mode would be useful.

        std::cerr << "\nxyza::MyDate: Invalid internal serial date value "
                  << d_serialDate << '.' << std::endl;

#endif // BSLS_ASSERT_IS_ACTIVE

        // In safe mode, each of the 'MyClass' methods fully guards its
        // preconditions: There is simply no easy way to get here!

        BSLS_ASSERT_SAFE("Probable rogue memory overwrite!" && 0);

        // If we get here, we're corrupted, but not in safe mode!

        return stream << "(* Invalid 'MyDate' State "
                      << d_serialDate
                      << " *)" << std::flush;                     // RETURN

    }
#endif // BSLS_ASSERT_OPT_IS_ACTIVE

    // If we get here in a production build, this object is "sane": Do
    // whatever this 'print' method would normally do, assuming that no
    // method preconditions or object invariants are violated.

    // ...  <*** Your (Normal-Case) Formatting Code Here! ***>

    return stream;
}
//..
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
// In order to totally remove the function when it is not needed, the 'IS_USED'
// suffixed macros can be used to guard the declaration and definition of the
// function.  Suppose we have a 'class' with a function having a complex
// precondition, and that precondition check is both private and only needed
// when the assertions that use it are enabled.  In that case, we can guard the
// definitions and declarations against even being compiled like this:
//..
namespace usage_example_assert_8 {
class ComplexObject {
    // ...
#if defined(BSLS_ASSERT_SAFE_IS_USED)
    bool isPurplish() const;
        // Return 'true' if the current state of this object fits within
        // the complex requirements of being sufficiently purple, false
        // otherwise.
#endif
    // ...
public:
    // MANIPULATORS
 void doSomethingPurpley();
        // Do something purpley.  The behavior is undefined unless this
        // object is currently purplish (contact customer support to know
        // the current threshholds for purplishness).
};

#if defined(BSLS_ASSERT_SAFE_IS_USED)
bool ComplexObject::isPurplish() const
{
    // The real implementation would encode the complex logic of needing to
    // determine if this object feels purplish at the moment.
    return true;
}
#endif

void ComplexObject::doSomethingPurpley()
{
    BSLS_ASSERT_SAFE(isPurplish());
}
}  // close namespace usage_example_assert_8
//..
// Now, the 'ComplexObject::isPurplish' function will only exist in a subset of
// builds:
//: o When 'BSLS_ASSERT_SAFE' assertions are enabled in assert or review mode,
//:   the function will be compiled and invoked.
//: o When 'BSLS_ASSERT_VALIDATE_DISABLED_MACROS' is defined the function will
//:   be compiled.  This will make sure that a future change does not
//:   invalidate the implementation of 'isPurplish()' even though it is not
//:   used.
//: o When 'BSLS_ASSERT_SAFE' assertions are assumed the function will be
//:   compiled and might be invoked, or at least have its implementation
//:   inspected by the compiler to improve code generation.
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
//..
namespace usage_example_assert_9_a {
struct MySwapper {
    template <class T>
    static void swap(T& lhs, T& rhs)
        // Exchange the values of the specified 'lhs' and 'rhs'.
    {
        T tmp = lhs;
        lhs = rhs;
        rhs = tmp;
    }
};
}  // close namespace usage_example_assert_9_a
//..
// This works great as a simple 'swap' implementation, but we would like to
// assert in safe mode that it is doing the correct thing.  In order to do that
// we need to capture the initial values of our inputs before doing anything
// else, and we want to do this only when the respective assertions are
// enabled.  Here we would guard our code and our assertions in a check that
// 'BSLS_ASSERT_SAFE_IS_ACTIVE' is defined, like this:
//..
namespace usage_example_assert_9_b {
struct MySwapper {
    template <class T>
    static void swap(T& lhs, T& rhs)
        // Exchange the values of the specified 'lhs' and 'rhs'.
    {
#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
        T origLhs(lhs);
        T origRhs(rhs);
#endif
        T tmp = lhs;
        lhs = rhs;
        rhs = tmp;
#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
        BSLS_ASSERT_SAFE(rhs == origLhs);
        BSLS_ASSERT_SAFE(lhs == origRhs);
#endif
    }
};
}  // close namespace usage_example_assert_9_b
//..

// End of usage examples
// BDE_VERIFY pragma: pop

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

void test_case_21() {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #9
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
        //   USAGE EXAMPLE: Conditional Compilation of Support Code
        // --------------------------------------------------------------------

        if (verbose) printf( "\nUSAGE EXAMPLE #9"
                             "\n================\n" );

        if (verbose) printf( "\n9. Conditional Compilation of Code Supporting "
                             "Assertions\n");

        // See usage examples section at top of this file.
        int i = 17;
        int j = 19;
        usage_example_assert_9_a::MySwapper::swap(i,j);
        ASSERT(i == 19 && j == 17);
        usage_example_assert_9_b::MySwapper::swap(i,j);
        ASSERT(i == 17 && j == 19);

}

void test_case_20() {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #8
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
        //   USAGE EXAMPLE: Conditional Compilation of Support Functions
        // --------------------------------------------------------------------

        if (verbose) printf( "\nUSAGE EXAMPLE #8"
                             "\n================\n" );

        if (verbose) printf( "\n8. Conditional Compilation of Functions "
                             "Supporting Assertions\n");

        // See usage examples section at top of this file.
        usage_example_assert_8::ComplexObject o;
        o.doSomethingPurpley();
}

void test_case_19() {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #7
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
        //   USAGE EXAMPLE: Conditional Compilation
        // --------------------------------------------------------------------

        if (verbose) printf( "\nUSAGE EXAMPLE #7"
                             "\n================\n" );

        if (verbose) printf( "\n7. Conditional Compilation Associated with "
                             "Enabled Assertion Levels\n");

        // See usage examples section at top of this file.

        ASSERT(usage_example_assert_7::MyDateImpUtil::isValidSerialDate(1));
}

void test_case_18() {
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
        usage_example_assert_6::sillyFunc(veryVerbose);
#else
        if (veryVerbose) printf( "\tSAFE MODE 2 *is* defined.\n" );

        bsls::Assert::setViolationHandler(::testDriverPrint);
                                                          // for usage example
        bsls::Assert::setViolationHandler(::testDriverHandler);
                                                          // for regression
        globalReset();
        ASSERT(false == globalAssertFiredFlag);
#ifdef BDE_BUILD_TARGET_EXC
        try
        {
#endif  // BDE_BUILD_TARGET_EXC
            usage_example_assert_6::sillyFunc(veryVerbose);
            ASSERT(false);
#ifdef BDE_BUILD_TARGET_EXC
        }
        catch(const std::exception &)
        {
            ASSERT(true == globalAssertFiredFlag);
        }
#endif  // BDE_BUILD_TARGET_EXC
#endif  // BDE_BUILD_TARGET_SAFE_2
}

void test_case_17() {
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

#ifndef BDE_BUILD_TARGET_EXC
        if (verbose) {
            printf(
               "\tTest disabled as exceptions are NOT enabled.\n"
               "\tCalling the test function would abort.\n" );
        }
#else
        if (verbose) printf(
                      "\n5. Using the bsls::AssertFailureHandlerGuard\n" );

        // See usage examples section at top of this file.

        ASSERT(&bsls::Assert::failByAbort == bsls::Assert::violationHandler());

#ifndef BDE_BUILD_TARGET_OPT
    #if defined(BSLS_ASSERT_IS_ACTIVE) ||                                     \
        defined(BSLS_ASSERT_ENABLE_TEST_CASE_10)

        if (verbose) printf(
                "\n*** Note that the following 'Internal Error: ... 0 <= n' "
                "message is expected:\n\n" );

        ASSERT(0 != usage_example_assert_5::wrapperFunc(verbose));

    #endif
#endif
#endif
        ASSERT(&bsls::Assert::failByAbort == bsls::Assert::violationHandler());

}

void test_case_16() {
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

        usage_example_assert_4::globalEnableOurPrintingFlag = veryVerbose;

        ASSERTION_TEST_BEGIN
        usage_example_assert_4::ourMain();
        ASSERTION_TEST_END
#endif
}

void test_case_15() {
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

        usage_example_assert_3::myMain();

}

void test_case_14() {
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

        ASSERTION_TEST_BEGIN
        usage_example_assert_2::someFunc(1, 1, 0);
        ASSERTION_TEST_END
}

void test_case_13() {
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

}

void test_case_12() {
        // --------------------------------------------------------------------
        // 'CONSTEXPR' USE
        //
        // Concerns:
        //: 1 The 'BSLS_ASSERT' macros should compile when used in a function
        //:   marked with 'BSLS_KEYWORD_CONSTEXPR_CPP17'.
        //
        // Plan:
        //: 1 Functions defined earlier must compile and be executable in
        //:   contract at compile time.
        //
        // Testing:
        //   CONCERN: 'constexpr' interaction
        // --------------------------------------------------------------------

        if (verbose) printf( "\n'CONSTEXPR' USE"
                             "\n===============\n" );

        BSLS_KEYWORD_CONSTEXPR_CPP17 int value =
                               ConstexprFunctions::narrowConstexprFunction(17);
        ASSERT(value == 17);

        int rtValue = ConstexprFunctions::narrowConstexprFunction(34);
        ASSERT(rtValue == 34);

#ifdef BDE_BUILD_TARGET_EXC
        bsls::AssertFailureHandlerGuard guard(bsls::Assert::failByThrow);

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

void test_case_11() {
        // --------------------------------------------------------------------
        // FILE NAME OVERRIDE
        //
        // Concerns:
        //: 1 The file name logged can be overridden with the
        //:   'BSLS_ASSERTIMPUTIL_FILE' macro.
        //
        // Plan:
        //: 1 Only test the macros that are enabled in the current build level.
        //:
        //: 2 Test that the macros by default log the current '__FILE__'.
        //:
        //: 3 Test that the 'BSLS_ASSERTIMPUTIL_FILE' can be changed and all
        //:   enabled macros log the new value.
        //:
        //: 4 Test that reverting back to 'BSLS_ASSERTIMPUTIL_DEFAULTFILE'
        //:   returns to the original behavior.
        //
        // Testing:
        //   CONCERN: 'BSLS_ASSERTIMPUTIL_FILE' interaction
        // --------------------------------------------------------------------

        if (verbose) printf( "\nFILE NAME OVERRIDE"
                             "\n==================\n" );

        if (verbose) printf( "\nInstall 'testDriverHandler' "
                             "assertion-handler.\n" );

        bsls::Assert::setViolationHandler(&testDriverHandler);
        ASSERT(::testDriverHandler == bsls::Assert::violationHandler());

        // Set the review handler as well in case the review level is higher
        // than the assert level.
        bsls::Review::setViolationHandler(&testDriverReviewHandler);
        ASSERT(::testDriverReviewHandler == bsls::Review::violationHandler());

        if (veryVerbose) printf( "\tSet up expected file names. \n" );

        const char *file = __FILE__;
        const char *altf = "injected_file.t.cpp";

        if (verbose) printf(
            "\tVerify that the correct file is logged by default.\n");

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_SAFE(false);
        ASSERTION_TEST_END
        ASSERT(globalAssertFiredFlag);
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
#endif

#ifdef BSLS_ASSERT_IS_ACTIVE
        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT(false);
        ASSERTION_TEST_END
        ASSERT(globalAssertFiredFlag);
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
#endif

#ifdef BSLS_ASSERT_OPT_IS_ACTIVE
        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT(false);
        ASSERTION_TEST_END
        ASSERT(globalAssertFiredFlag);
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
#endif

        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_INVOKE("false");
        ASSERTION_TEST_END
        ASSERT(globalAssertFiredFlag);
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));

#ifdef BSLS_ASSERT_USE_CONTRACTS
        // file overriding not supported with language-contracts
        (void)file;
        (void)altf;
#else

        if (veryVerbose) printf(
            "\tRedefine 'BSLS_ASSERTIMPUTIL_FILE'.\n");

#undef BSLS_ASSERTIMPUTIL_FILE
#define BSLS_ASSERTIMPUTIL_FILE altf

        if (veryVerbose) printf(
            "\tVerify that the alternate file text is logged by "
            "enabled macros.\n");

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_SAFE(false);
        ASSERTION_TEST_END
        ASSERT(globalAssertFiredFlag);
        LOOP2_ASSERT(altf, globalFile,    0 == std::strcmp(altf, globalFile));
#endif

#ifdef BSLS_ASSERT_IS_ACTIVE
        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT(false);
        ASSERTION_TEST_END
        ASSERT(globalAssertFiredFlag);
        LOOP2_ASSERT(altf, globalFile,    0 == std::strcmp(altf, globalFile));
#endif

#ifdef BSLS_ASSERT_OPT_IS_ACTIVE
        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT(false);
        ASSERTION_TEST_END
        ASSERT(globalAssertFiredFlag);
        LOOP2_ASSERT(altf, globalFile,    0 == std::strcmp(altf, globalFile));
#endif

        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_INVOKE("false");
        ASSERTION_TEST_END
        LOOP2_ASSERT(altf, globalFile,    0 == std::strcmp(altf, globalFile));

        if (veryVerbose) printf(
            "\tRevert 'BSLS_ASSERTIMPUTIL_FILE' to "
            "'BSLS_ASSERTIMPUTIL_DEFAULTFILE'.\n");

#undef BSLS_ASSERTIMPUTIL_FILE
#define BSLS_ASSERTIMPUTIL_FILE BSLS_ASSERTIMPUTIL_DEFAULTFILE

        if (verbose) printf(
            "\tVerify that the correct file is logged again.\n");

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_SAFE(false);
        ASSERTION_TEST_END
        ASSERT(globalAssertFiredFlag);
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
#endif

#ifdef BSLS_ASSERT_IS_ACTIVE
        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT(false);
        ASSERTION_TEST_END
        ASSERT(globalAssertFiredFlag);
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
#endif

#ifdef BSLS_ASSERT_OPT_IS_ACTIVE
        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT(false);
        ASSERTION_TEST_END
        ASSERT(globalAssertFiredFlag);
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
#endif

        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_INVOKE("false");
        ASSERTION_TEST_END
        ASSERT(globalAssertFiredFlag);
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
#endif // BSLS_ASSERT_USE_CONTRACTS

}

void test_case_10() {
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
        }

        if (verbose) printf(
                "\nVerify that the legacy 'invokeHandler' properly transmits "
                "its arguments to a 'Handler'.\n" );
        {

            globalReset();
            ASSERT(false == globalAssertFiredFlag);
            ASSERT(false == globalLegacyAssertFiredFlag);

            ASSERTION_TEST_BEGIN
                bsls::Assert::invokeHandler("ExPrEsSiOn", "FiLe", 12345678);
            ASSERTION_TEST_END

            ASSERT(    true == globalAssertFiredFlag);
            ASSERT(    true == globalLegacyAssertFiredFlag);
            ASSERT(       0 == std::strcmp("ExPrEsSiOn", globalText));
            ASSERT(       0 == std::strcmp("FiLe",       globalFile));
            ASSERT(       0 == std::strcmp("",           globalLevel));
            ASSERT(12345678 == globalLine);
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

}

void test_case_9() {
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

#if !defined(BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER)
        // 'invokeHandler' cannot return if we are building with the
        // '...ENABLE_NORETURN...' flag turned on.  If we do not have it on
        // then we still need to disable the policy preventing returning.

        // Change the handler return policy not to abort.
        {
            // Enable assertions to return (in violation of policy) for testing
            // purposes only.

            char *key = const_cast<char*>(
                  bsls::Assert::k_permitOutOfPolicyReturningAssertionBuildKey);
            strcpy(key, "bsls-PermitOutOfPolicyReturn");

            bsls::Assert::permitOutOfPolicyReturningFailureHandler();
        }

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
#else
        if (veryVerbose) printf(
             "\nSKIP: "
             "'BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER' is defined.\n");

#endif
}

void test_case_8() {
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

#if !defined(BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER)

        // 'invokeHandler' cannot return if we are building with the
        // '...ENABLE_NORETURN...' flag turned on.  If we do not have it on
        // then we still need to disable the policy preventing returning.

        // Change the handler return policy not to abort.
        {
            // Enable assertions to return (in violation of policy) for testing
            // purposes only.

            char *key = const_cast<char*>(
                  bsls::Assert::k_permitOutOfPolicyReturningAssertionBuildKey);
            strcpy(key, "bsls-PermitOutOfPolicyReturn");

            bsls::Assert::permitOutOfPolicyReturningFailureHandler();
        }

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
#else
        if (veryVerbose) printf(
             "\nSKIP: "
             "'BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER' is defined.\n");
#endif
}

void test_case_7() {
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
        //   BSLS_ASSERT_IS_ACTIVE
        //   BSLS_ASSERT_IS_ASSUME
        //   BSLS_ASSERT_IS_REVIEW
        //   BSLS_ASSERT_OPT_IS_ACTIVE
        //   BSLS_ASSERT_OPT_IS_ASSUME
        //   BSLS_ASSERT_OPT_IS_REVIEW
        //   BSLS_ASSERT_SAFE_IS_ACTIVE
        //   BSLS_ASSERT_SAFE_IS_ASSUME
        //   BSLS_ASSERT_SAFE_IS_REVIEW
        // --------------------------------------------------------------------

        if (verbose) printf( "\nCONFIGURATION MACROS"
                             "\n====================\n" );

        TestConfigurationMacros();
}

void test_case_6() {
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

}

void test_case_5() {
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

#if !defined(BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER)
        {
            bsls::Assert::permitOutOfPolicyReturningFailureHandler();
            ASSERT(bsls::Assert::abortUponReturningAssertionFailureHandler()
                                                                      == true);
        }

        // 'invokeHandler' cannot return if we are building with the
        // '...ENABLE_NORETURN...' flag turned on.

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
            globalReset();
            BSLS_ASSERT_INVOKE("This is an intentional assert");

            ASSERT(    true == globalAssertFiredFlag);
        }
#else
        if (veryVerbose) printf(
             "\nSKIP: "
             "'BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER' is defined.\n");
#endif
}

void test_case_4() {
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
            catch (const bsls::AssertTestException&) {
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
            catch (const bsls::AssertTestException&) {
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
}

void test_case_3() {
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

}

void test_case_2() {
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
        //:   fires only on expressions that contextually conver to false.
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

#if !defined(BDE_BUILD_TARGET_EXC)
#if defined(BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER)
        // With exceptions not enabled and a '[[noreturn]]' 'invokeHandler'
        // there is no way to test triggered assertions.

        if (veryVerbose) printf(
             "\nSKIP: "
             "'BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER' is defined.\n");

        return;
#else
        // Change the handler return policy not to abort
        {
            // Enable assertions to return (in violation of policy) for testing
            // purposes only.

            char *key = const_cast<char*>(
                  bsls::Assert::k_permitOutOfPolicyReturningAssertionBuildKey);
            strcpy(key, "bsls-PermitOutOfPolicyReturn");

            bsls::Assert::permitOutOfPolicyReturningFailureHandler();
        }
#endif
#endif

        bsls::Assert::setViolationHandler(&testDriverHandler);
        ASSERT(::testDriverHandler == bsls::Assert::violationHandler());

        if (veryVerbose) printf( "\tSet up all but line numbers now. \n" );

        const void *p     = 0;

        const char *istr  = "0";
        const char *pstr  = "p";
        const char *estr  = "false == true";
        const char *tstr  = "false == true ? true : false";
        const char *bstr  = "ExplicitBool(false)";
        const char *file  = __FILE__;
        const char *level = bsls::Assert::k_LEVEL_SAFE;
        int         line  = -1;

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

#if defined(__cpp_contracts)
            printf( "\t" STRINGIFY(__cpp_contracts) " == __cpp_contracts\n" );
#else
            printf( "\tundefined __cpp_contracts\n" );
#endif

#if defined(__cpp_contracts_literal_semantics)
            printf( "\t" STRINGIFY(__cpp_contracts_literal_semantics)
                                   " == __cpp_contracts_literal_semantics\n" );
#else
            printf( "\tundefined __cpp_contracts_literal_semantics\n" );
#endif

        }

        //_____________________________________________________________________
        //            BSLS_ASSERT_SAFE, BSLS_ASSERT, BSLS_ASSERT_OPT
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE)                                    \
 || !IS_BSLS_ASSERT_MODE_FLAG_DEFINED && (                                    \
        defined(BDE_BUILD_TARGET_SAFE_2) ||                                   \
        defined(BDE_BUILD_TARGET_SAFE)   )

        if (verbose) printf(
                          "\nEnabled: ASSERT_SAFE, ASSERT, ASSERT_OPT\n" );

        if (veryVerbose) printf( "\tCheck for integer expression.\n" );

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_SAFE : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_SAFE(0);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(istr,  globalText,    0 == std::strcmp(istr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_ASSERT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT     (0);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(istr,  globalText,    0 == std::strcmp(istr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_OPT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (0);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(istr,  globalText,    0 == std::strcmp(istr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        if (veryVerbose) printf( "\tCheck for pointer expression.\n" );

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_SAFE : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_SAFE(p);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(pstr,  globalText,    0 == std::strcmp(pstr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_ASSERT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT     (p);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(pstr,  globalText,    0 == std::strcmp(pstr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_OPT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (p);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(pstr,  globalText,    0 == std::strcmp(pstr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        if (veryVerbose) printf( "\tCheck for expression with spaces.\n" );

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_SAFE : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_SAFE(false == true);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(estr,  globalText,    0 == std::strcmp(estr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_ASSERT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT     (false == true);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(estr,  globalText,    0 == std::strcmp(estr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_OPT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (false == true);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(estr,  globalText,    0 == std::strcmp(estr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        if (veryVerbose) printf( "\tCheck for expression with ternary.\n" );

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_SAFE : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_SAFE(false == true ? true : false);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(tstr,  globalText,    0 == std::strcmp(tstr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_ASSERT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT     (false == true ? true : false);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(tstr,  globalText,    0 == std::strcmp(tstr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_OPT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (false == true ? true : false);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(tstr,  globalText,    0 == std::strcmp(tstr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        if (veryVerbose) printf( "\tCheck for bool conversion.\n" );

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_SAFE : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_SAFE(ExplicitBool(false));
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(bstr,  globalText,    0 == std::strcmp(bstr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_ASSERT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT     (ExplicitBool(false));
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(bstr,  globalText,    0 == std::strcmp(bstr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_OPT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (ExplicitBool(false));
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(bstr,  globalText,    0 == std::strcmp(bstr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);
#endif

        //_____________________________________________________________________
        //                    BSLS_ASSERT, BSLS_ASSERT_OPT
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if defined(BSLS_ASSERT_LEVEL_ASSERT)                                         \
 || !IS_BSLS_ASSERT_MODE_FLAG_DEFINED &&                                      \
        !defined(BDE_BUILD_TARGET_OPT) &&                                     \
        !defined(BDE_BUILD_TARGET_SAFE) &&                                    \
        !defined(BDE_BUILD_TARGET_SAFE_2)

        if (verbose) printf( "\nEnabled: ASSERT, ASSERT_OPT\n" );

        if (veryVerbose) printf( "\tCheck for integer expression.\n" );

        globalReset(); BSLS_ASSERT_SAFE(0); ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_ASSERT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT     (0);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(istr,  globalText,    0 == std::strcmp(istr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_OPT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (0);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(istr,  globalText,    0 == std::strcmp(istr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        if (veryVerbose) printf( "\tCheck for pointer expression.\n" );

        globalReset(); BSLS_ASSERT_SAFE(p); ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_ASSERT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT     (p);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(pstr,  globalText,    0 == std::strcmp(pstr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_OPT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (p);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(pstr,  globalText,    0 == std::strcmp(pstr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        if (veryVerbose) printf( "\tCheck for expression with spaces.\n" );

        globalReset(); BSLS_ASSERT_SAFE(false == true);
        ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_ASSERT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT     (false == true);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(estr,  globalText,    0 == std::strcmp(estr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_OPT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (false == true);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(estr,  globalText,    0 == std::strcmp(estr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        if (veryVerbose) printf( "\tCheck for expression with ternary.\n" );

        globalReset(); BSLS_ASSERT_SAFE(false == true ? true : false);
        ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_ASSERT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT     (false == true ? true : false);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(tstr,  globalText,    0 == std::strcmp(tstr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_OPT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (false == true ? true : false);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(tstr,  globalText,    0 == std::strcmp(tstr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        if (veryVerbose) printf( "\tCheck for bool conversion.\n" );

        globalReset(); BSLS_ASSERT_SAFE(ExplicitBool(false));
        ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_ASSERT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT     (ExplicitBool(false));
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(bstr,  globalText,    0 == std::strcmp(bstr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_OPT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (ExplicitBool(false));
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(bstr,  globalText,    0 == std::strcmp(bstr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

#endif

        //_____________________________________________________________________
        //                         BSLS_ASSERT_OPT
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if defined(BSLS_ASSERT_LEVEL_ASSERT_OPT)                                     \
 || !IS_BSLS_ASSERT_MODE_FLAG_DEFINED &&                                      \
        defined(BDE_BUILD_TARGET_OPT) &&                                      \
        !defined(BDE_BUILD_TARGET_SAFE) &&                                    \
        !defined(BDE_BUILD_TARGET_SAFE_2)

        if (verbose) printf( "\nEnabled: ASSERT_OPT\n" );

        if (veryVerbose) printf( "\tCheck for integer expression.\n" );

        globalReset(); BSLS_ASSERT_SAFE(0); ASSERT(0 == globalAssertFiredFlag);
        globalReset(); BSLS_ASSERT     (0); ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_OPT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (0);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(istr,  globalText,    0 == std::strcmp(istr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        if (veryVerbose) printf( "\tCheck for pointer expression.\n" );

        globalReset(); BSLS_ASSERT_SAFE(p); ASSERT(0 == globalAssertFiredFlag);
        globalReset(); BSLS_ASSERT     (p); ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_OPT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (p);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(pstr,  globalText,    0 == std::strcmp(pstr, globalText));
        LOOP2_ASSERT(file,  globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        if (veryVerbose) printf( "\tCheck for expression with spaces.\n" );

        globalReset(); BSLS_ASSERT_SAFE(false == true);
        ASSERT(0 == globalAssertFiredFlag);

        globalReset(); BSLS_ASSERT     (false == true);
        ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_OPT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (false == true);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(estr,  globalText, 0 == std::strcmp(estr, globalText));
        LOOP2_ASSERT(file,  globalFile, 0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        if (veryVerbose) printf( "\tCheck for expression with ternary.\n" );

        globalReset(); BSLS_ASSERT_SAFE(false == true ? true : false);
        ASSERT(0 == globalAssertFiredFlag);

        globalReset(); BSLS_ASSERT     (false == true ? true : false);
        ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_OPT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (false == true ? true : false);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(tstr,  globalText, 0 == std::strcmp(tstr, globalText));
        LOOP2_ASSERT(file,  globalFile, 0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

        if (veryVerbose) printf( "\tCheck for bool conversion.\n" );

        globalReset(); BSLS_ASSERT_SAFE(ExplicitBool(false));
        ASSERT(0 == globalAssertFiredFlag);

        globalReset(); BSLS_ASSERT     (ExplicitBool(false));
        ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        level = checkLevels ? bsls::Assert::k_LEVEL_OPT : "";
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (ExplicitBool(false));
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(bstr,  globalText, 0 == std::strcmp(bstr, globalText));
        LOOP2_ASSERT(file,  globalFile, 0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(level, globalLevel, 0 == std::strcmp(level, globalLevel));
        LOOP2_ASSERT(line,  globalLine, line == globalLine);

#endif

        //_____________________________________________________________________
        //                  *** None Instantiate ***
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if defined(BSLS_ASSERT_LEVEL_NONE)
        (void)istr;
        (void)estr;
        (void)tstr;
        (void)bstr;

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

        if (veryVerbose) printf( "\tCheck for expression with ternary.\n" );

        globalReset(); BSLS_ASSERT_SAFE(false == true ? true : false);
        ASSERT(0 == globalAssertFiredFlag);

        globalReset(); BSLS_ASSERT     (false == true ? true : false);
        ASSERT(0 == globalAssertFiredFlag);

        globalReset(); BSLS_ASSERT_OPT (false == true ? true : false);
        ASSERT(0 == globalAssertFiredFlag);

        if (veryVerbose) printf( "\tCheck for bool conversion.\n" );

        globalReset(); BSLS_ASSERT_SAFE(ExplicitBool(false));
        ASSERT(0 == globalAssertFiredFlag);

        globalReset(); BSLS_ASSERT     (ExplicitBool(false));
        ASSERT(0 == globalAssertFiredFlag);

        globalReset(); BSLS_ASSERT_OPT (ExplicitBool(false));
        ASSERT(0 == globalAssertFiredFlag);

#endif

        //_____________________________________________________________________
        //                  *** Assumptions ***
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if defined(BSLS_ASSERT_LEVEL_ASSUME_OPT)    ||                               \
    defined(BSLS_ASSERT_LEVEL_ASSUME_ASSERT) ||                               \
    defined(BSLS_ASSERT_LEVEL_ASSUME_SAFE)
        (void)istr;
        (void)estr;
        (void)tstr;
        (void)bstr;
        (void)p;
#endif

        //_____________________________________________________________________
        //           *** BSLS_ASSERT_INVOKE (always instantiate) ***
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if (veryVerbose) printf( "\tChecking BSLS_INVOKE\n" );

        globalReset();
        line = L_ + 2;
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_INVOKE(pstr);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);
        LOOP2_ASSERT(pstr, globalText,    0 == std::strcmp(pstr, globalText));
        LOOP2_ASSERT(file, globalFile,    0 == std::strcmp(file, globalFile));
        LOOP2_ASSERT(line, globalLine, line == globalLine);
}

void test_case_1() {
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
        }

        if (verbose) printf(
                "\nVerify that the legacy 'invokeHandler' properly transmits "
                "its arguments.\n" );
        {

            globalReset();
            ASSERT(false        == globalAssertFiredFlag);
            ASSERT(false        == globalLegacyAssertFiredFlag);

            ASSERTION_TEST_BEGIN
                bsls::Assert::invokeHandler("ExPrEsSiOn", "FiLe", 12345678);
            ASSERTION_TEST_END

            ASSERT(    true == globalAssertFiredFlag);
            ASSERT(   false == globalLegacyAssertFiredFlag);
            ASSERT(       0 == std::strcmp("ExPrEsSiOn", globalText));
            ASSERT(       0 == std::strcmp("FiLe",       globalFile));
            ASSERT(       0 == std::strcmp("INV",        globalLevel));
            ASSERT(12345678 == globalLine);
        }

        if (verbose) printf(
                "\nVerify that 'lockAssertAdministration' blocks callback "
                "changes.\n" );

        bsls::Assert::lockAssertAdministration();

        bsls::Assert::setViolationHandler(&bsls::Assert::failByAbort);
        ASSERT(::testDriverHandler == bsls::Assert::violationHandler());

#ifndef BSLS_ASSERT_OPT_IS_ACTIVE
        if (verbose) printf(
            "\n'BSLS_ASSERT_OPT_IS_ACTIVE' is not defined;"
            " exit breathing test.\n" );
        return;
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
}

void test_case_m1() {
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
}

void test_case_m2() {
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
}

void test_case_m3() {
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

#ifdef BDE_BUILD_TARGET_EXC
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
}

void test_case_m4() {
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

#ifdef BDE_BUILD_TARGET_EXC
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
}

void test_case_m5() {
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
}

void test_case_m6() {
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
}

void test_case_m7() {
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

#if !defined(BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER)
        // 'invokeHandler' cannot return if we are building with the
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
#else
        if (veryVerbose) printf(
             "\nSKIP: "
             "'BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER' is defined.\n");
#endif
}

void test_case_m8() {
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

#if defined(BSLS_ASSERT_IS_ACTIVE)
        BSLS_ASSERT(!"This is an intentional assert");
#endif

        printf( "This message should not be seen.\n" );
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
    case 21: test_case_21(); break;
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
    case -5: test_case_m5(); break;
    case -6: test_case_m6(); break;
    case -7: test_case_m7(); break;
    case -8: test_case_m8(); break;
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
//    BDE_BUILD mode      assertion            ASSERT macros REVIEW macros
//  SAFE2 SAFE DBG OPT      level              OPT     SAFE  OPT     SAFE
//  ----- ---- --- ---    --------             --- --- ----  --- --- ----
//                                              X   X         R   R
//                  X                           X             R
//             X                                X   X         R   R
//             X    X                           X             R
//         X                                    X   X   X     R   R   R
//         X        X                           X   X   X     R   R   R
//         X   X                                X   X   X     R   R   R
//         X   X    X                           X   X   X     R   R   R
//    X                                         X   X   X     R   R   R
//    X             X                           X   X   X     R   R   R
//    X        X                                X   X   X     R   R   R
//    X        X    X                           X   X   X     R   R   R
//    X    X                                    X   X   X     R   R   R
//    X    X        X                           X   X   X     R   R   R
//    X    X   X                                X   X   X     R   R   R
//    X    X   X    X                           X   X   X     R   R   R
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
//                       LEVEL_ASSERT_OPT       X             R
//                  X    LEVEL_ASSERT_OPT       X             R
//             X         LEVEL_ASSERT_OPT       X             R
//             X    X    LEVEL_ASSERT_OPT       X             R
//         X             LEVEL_ASSERT_OPT       X             R
//         X        X    LEVEL_ASSERT_OPT       X             R
//         X   X         LEVEL_ASSERT_OPT       X             R
//         X   X    X    LEVEL_ASSERT_OPT       X             R
//    X                  LEVEL_ASSERT_OPT       X             R
//    X             X    LEVEL_ASSERT_OPT       X             R
//    X        X         LEVEL_ASSERT_OPT       X             R
//    X        X    X    LEVEL_ASSERT_OPT       X             R
//    X    X             LEVEL_ASSERT_OPT       X             R
//    X    X        X    LEVEL_ASSERT_OPT       X             R
//    X    X   X         LEVEL_ASSERT_OPT       X             R
//    X    X   X    X    LEVEL_ASSERT_OPT       X             R
//                       LEVEL_ASSERT           X   X         R   R
//                  X    LEVEL_ASSERT           X   X         R   R
//             X         LEVEL_ASSERT           X   X         R   R
//             X    X    LEVEL_ASSERT           X   X         R   R
//         X             LEVEL_ASSERT           X   X         R   R
//         X        X    LEVEL_ASSERT           X   X         R   R
//         X   X         LEVEL_ASSERT           X   X         R   R
//         X   X    X    LEVEL_ASSERT           X   X         R   R
//    X                  LEVEL_ASSERT           X   X         R   R
//    X             X    LEVEL_ASSERT           X   X         R   R
//    X        X         LEVEL_ASSERT           X   X         R   R
//    X        X    X    LEVEL_ASSERT           X   X         R   R
//    X    X             LEVEL_ASSERT           X   X         R   R
//    X    X        X    LEVEL_ASSERT           X   X         R   R
//    X    X   X         LEVEL_ASSERT           X   X         R   R
//    X    X   X    X    LEVEL_ASSERT           X   X         R   R
//                       LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//                  X    LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//             X         LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//             X    X    LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//         X             LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//         X        X    LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//         X   X         LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//         X   X    X    LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//    X                  LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//    X             X    LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//    X        X         LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//    X        X    X    LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//    X    X             LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//    X    X        X    LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//    X    X   X         LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//    X    X   X    X    LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//
// Finally we will test the (hopefully orthogonal) effects of setting the
// review level and the assert level explicitly.  This should turn any asserts
// that are above the assert level into reviews, according to the following
// table:
//
//  assertion             review           ASSERT macros REVIEW macros
//    level                level           OPT     SAFE  OPT     SAFE
//  -----------------    ----------------- --- --- ----  --- --- ----
//  LEVEL_ASSUME_OPT     LEVEL_NONE         A
//  LEVEL_ASSUME_ASSERT  LEVEL_NONE         A   A
//  LEVEL_ASSUME_SAFE    LEVEL_NONE         A   A   A
//  LEVEL_NONE           LEVEL_NONE
//  LEVEL_ASSERT_OPT     LEVEL_NONE         X
//  LEVEL_ASSERT         LEVEL_NONE         X   X
//  LEVEL_ASSERT_SAFE    LEVEL_NONE         X   X   X
//  LEVEL_NONE|ASSUME*   LEVEL_REVIEW_OPT   R             R
//  LEVEL_ASSERT_OPT     LEVEL_REVIEW_OPT   X             R
//  LEVEL_ASSERT         LEVEL_REVIEW_OPT   X   X         R
//  LEVEL_ASSERT_SAFE    LEVEL_REVIEW_OPT   X   X   X     R
//  LEVEL_NONE|ASSUME*   LEVEL_REVIEW       R   R         R   R
//  LEVEL_ASSERT_OPT     LEVEL_REVIEW       X   R         R   R
//  LEVEL_ASSERT         LEVEL_REVIEW       X   X         R   R
//  LEVEL_ASSERT_SAFE    LEVEL_REVIEW       X   X   X     R   R
//  LEVEL_NONE|ASSUME*   LEVEL_REVIEW_SAFE  R   R   R     R   R   R
//  LEVEL_ASSERT_OPT     LEVEL_REVIEW_SAFE  X   R   R     R   R   R
//  LEVEL_ASSERT         LEVEL_REVIEW_SAFE  X   X   R     R   R   R
//  LEVEL_ASSERT_SAFE    LEVEL_REVIEW_SAFE  X   X   X     R   R   R

namespace
{

#if defined(BDE_BUILD_TARGET_EXC)
struct AssertFailed {
    // This struct contains static functions suitable for registration as an
    // assert handler, and provides a distinct "empty" type that may be thrown
    // from the handler and caught within the test cases below, in order to
    // confirm if the appropriate 'BSLS_ASSERT_*' macros are enabled properly
    // or not.
    BSLS_ANNOTATION_NORETURN
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

void TestConfigurationMacros()
{

    if (verbose) printf( "\nCONFIGURATION MACROS"
                               "\n====================\n" );

#if !defined(BDE_BUILD_TARGET_EXC)
    if (verbose)
        printf( "\nThis case is not run as it relies "
                "on exception support.\n" );
#else
    if (verbose) printf( "\nWe need to write a running commentary\n" );

    bsls::Assert::setViolationHandler(&AssertFailed::failMacroTest);
    bsls::Review::setViolationHandler(&ReviewFailed::failReviewMacroTest);

//--------------------------------------------------------------------GENERATOR
// The following script generates tests that are included in this function for
// most/all of the various build configurations that we want to verify.
//..
//  #!/usr/bin/env python
//
//  table1 = """
//    BDE_BUILD mode      assertion            ASSERT macros REVIEW macros
//  SAFE2 SAFE DBG OPT      level              OPT     SAFE  OPT     SAFE
//  ----- ---- --- ---    --------             --- --- ----  --- --- ----
//                                              X   X         R   R
//                  X                           X             R
//             X                                X   X         R   R
//             X    X                           X             R
//         X                                    X   X   X     R   R   R
//         X        X                           X   X   X     R   R   R
//         X   X                                X   X   X     R   R   R
//         X   X    X                           X   X   X     R   R   R
//    X                                         X   X   X     R   R   R
//    X             X                           X   X   X     R   R   R
//    X        X                                X   X   X     R   R   R
//    X        X    X                           X   X   X     R   R   R
//    X    X                                    X   X   X     R   R   R
//    X    X        X                           X   X   X     R   R   R
//    X    X   X                                X   X   X     R   R   R
//    X    X   X    X                           X   X   X     R   R   R
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
//                       LEVEL_ASSERT_OPT       X             R
//                  X    LEVEL_ASSERT_OPT       X             R
//             X         LEVEL_ASSERT_OPT       X             R
//             X    X    LEVEL_ASSERT_OPT       X             R
//         X             LEVEL_ASSERT_OPT       X             R
//         X        X    LEVEL_ASSERT_OPT       X             R
//         X   X         LEVEL_ASSERT_OPT       X             R
//         X   X    X    LEVEL_ASSERT_OPT       X             R
//    X                  LEVEL_ASSERT_OPT       X             R
//    X             X    LEVEL_ASSERT_OPT       X             R
//    X        X         LEVEL_ASSERT_OPT       X             R
//    X        X    X    LEVEL_ASSERT_OPT       X             R
//    X    X             LEVEL_ASSERT_OPT       X             R
//    X    X        X    LEVEL_ASSERT_OPT       X             R
//    X    X   X         LEVEL_ASSERT_OPT       X             R
//    X    X   X    X    LEVEL_ASSERT_OPT       X             R
//                       LEVEL_ASSERT           X   X         R   R
//                  X    LEVEL_ASSERT           X   X         R   R
//             X         LEVEL_ASSERT           X   X         R   R
//             X    X    LEVEL_ASSERT           X   X         R   R
//         X             LEVEL_ASSERT           X   X         R   R
//         X        X    LEVEL_ASSERT           X   X         R   R
//         X   X         LEVEL_ASSERT           X   X         R   R
//         X   X    X    LEVEL_ASSERT           X   X         R   R
//    X                  LEVEL_ASSERT           X   X         R   R
//    X             X    LEVEL_ASSERT           X   X         R   R
//    X        X         LEVEL_ASSERT           X   X         R   R
//    X        X    X    LEVEL_ASSERT           X   X         R   R
//    X    X             LEVEL_ASSERT           X   X         R   R
//    X    X        X    LEVEL_ASSERT           X   X         R   R
//    X    X   X         LEVEL_ASSERT           X   X         R   R
//    X    X   X    X    LEVEL_ASSERT           X   X         R   R
//                       LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//                  X    LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//             X         LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//             X    X    LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//         X             LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//         X        X    LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//         X   X         LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//         X   X    X    LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//    X                  LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//    X             X    LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//    X        X         LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//    X        X    X    LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//    X    X             LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//    X    X        X    LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//    X    X   X         LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//    X    X   X    X    LEVEL_ASSERT_SAFE      X   X   X     R   R   R
//  """
//
//  table2 = """
//  assertion             review           ASSERT macros REVIEW macros
//    level                level           OPT     SAFE  OPT     SAFE
//  -----------------    ----------------- --- --- ----  --- --- ----
//  LEVEL_ASSUME_OPT     LEVEL_NONE         A
//  LEVEL_ASSUME_ASSERT  LEVEL_NONE         A   A
//  LEVEL_ASSUME_SAFE    LEVEL_NONE         A   A   A
//  LEVEL_NONE           LEVEL_NONE
//  LEVEL_ASSERT_OPT     LEVEL_NONE         X
//  LEVEL_ASSERT         LEVEL_NONE         X   X
//  LEVEL_ASSERT_SAFE    LEVEL_NONE         X   X   X
//  LEVEL_NONE|ASSUME*   LEVEL_REVIEW_OPT   R             R
//  LEVEL_ASSERT_OPT     LEVEL_REVIEW_OPT   X             R
//  LEVEL_ASSERT         LEVEL_REVIEW_OPT   X   X         R
//  LEVEL_ASSERT_SAFE    LEVEL_REVIEW_OPT   X   X   X     R
//  LEVEL_NONE|ASSUME*   LEVEL_REVIEW       R   R         R   R
//  LEVEL_ASSERT_OPT     LEVEL_REVIEW       X   R         R   R
//  LEVEL_ASSERT         LEVEL_REVIEW       X   X         R   R
//  LEVEL_ASSERT_SAFE    LEVEL_REVIEW       X   X   X     R   R
//  LEVEL_NONE|ASSUME*   LEVEL_REVIEW_SAFE  R   R   R     R   R   R
//  LEVEL_ASSERT_OPT     LEVEL_REVIEW_SAFE  X   R   R     R   R   R
//  LEVEL_ASSERT         LEVEL_REVIEW_SAFE  X   X   R     R   R   R
//  LEVEL_ASSERT_SAFE    LEVEL_REVIEW_SAFE  X   X   X     R   R   R
//  """
//
//  def printtitle(t):
//      e1 = (73 - len(t)) / 2
//      e2 = 73 - len(t) - e1
//      t1 = "//%s %s %s//" % ("="*e1,t,"="*e2,)
//      print(t1)
//
//  def printcheckassert(atype,exp):
//      if exp == "X":
//          print("""#if !defined(BSLS_ASSERT%s_IS_ACTIVE)
//  #error BSLS_ASSERT%s_IS_ACTIVE should be defined
//  #endif
//
//  #if defined(BSLS_ASSERT%s_IS_REVIEW)
//  #error BSLS_ASSERT%s_IS_REVIEW should not be defined
//  #endif
//
//  #if defined(BSLS_ASSERT%s_IS_ASSUMED)
//  #error BSLS_ASSERT%s_IS_ASSUMED should not be defined
//  #endif
//  """ % (atype,atype,atype,atype,atype,atype,))
//      elif exp == "R":
//          print("""#if defined(BSLS_ASSERT%s_IS_ACTIVE)
//  #error BSLS_ASSERT%s_IS_ACTIVE should not be defined
//  #endif
//
//  #if !defined(BSLS_ASSERT%s_IS_REVIEW)
//  #error BSLS_ASSERT%s_IS_REVIEW should be defined
//  #endif
//
//  #if defined(BSLS_ASSERT%s_IS_ASSUMED)
//  #error BSLS_ASSERT%s_IS_ASSUMED should not be defined
//  #endif
//  """ % (atype,atype,atype,atype,atype,atype,))
//      elif exp == 'A':
//          print("""#if defined(BSLS_ASSERT%s_IS_ACTIVE)
//  #error BSLS_ASSERT%s_IS_ACTIVE should not be defined
//  #endif
//
//  #if defined(BSLS_ASSERT%s_IS_REVIEW)
//  #error BSLS_ASSERT%s_IS_REVIEW should not be defined
//  #endif
//
//  #if !defined(BSLS_ASSERT%s_IS_ASSUMED)
//  #error BSLS_ASSERT%s_IS_ASSUMED should be defined
//  #endif
//  """ % (atype,atype,atype,atype,atype,atype,))
//      else:
//          print("""#if defined(BSLS_ASSERT%s_IS_ACTIVE)
//  #error BSLS_ASSERT%s_IS_ACTIVE should not be defined
//  #endif
//
//  #if defined(BSLS_ASSERT%s_IS_REVIEW)
//  #error BSLS_ASSERT%s_IS_REVIEW should not be defined
//  #endif
//
//  #if defined(BSLS_ASSERT%s_IS_ASSUMED)
//  #error BSLS_ASSERT%s_IS_ASSUMED should not be defined
//  #endif
//  """ % (atype,atype,atype,atype,atype,atype,))
//
//  def printcheckreview(rtype,exp):
//      if exp != "R":
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
//      if expected == "A":
//          print("""    // %s is undefined behavior
//  """ % (failing,))
//      else:
//          args = (failing,)
//          if expected == "X":
//              args = args + ( "(false);", "(true); ", "(false);",)
//          elif expected == "R":
//              args = args + ( "(false);", "(false);", "(true); ",)
//          else:
//              args = args + ( "(true); ", "(false);", "(false);",)
//          print("""    try { %-29s ASSERT%s }
//      catch(AssertFailed)               { ASSERT%s }
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
//  #undef INCLUDED_BSLS_ASSERT_MACRORESET
//  #include <bsls_assert_macroreset.h>
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
//  // [3] Re-include the 'bsls_assert.h' header.
//
//  #include <bsls_assert.h>
//
//  // [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros
//  """)
//
//      printcheckassert("_OPT",expected[0])
//      printcheckassert("",expected[1])
//      printcheckassert("_SAFE",expected[2])
//      printcheckreview("_OPT",expected[3])
//      printcheckreview("",expected[4])
//      printcheckreview("_SAFE",expected[5])
//
//      print("// [5] Test that the public assert and review macros " +
//            "have the expected effect.\n")
//
//      printcheckfailures("BSLS_ASSERT_OPT(false);", expected[0])
//      printcheckfailures("BSLS_ASSERT(false);", expected[1])
//      printcheckfailures("BSLS_ASSERT_SAFE(false);", expected[2])
//      printcheckfailures("BSLS_ASSERT_INVOKE(\"false\");", "X")
//      printcheckfailures("BSLS_REVIEW_OPT(false);", expected[3])
//      printcheckfailures("BSLS_REVIEW(false);", expected[4])
//      printcheckfailures("BSLS_REVIEW_SAFE(false);", expected[5])
//      printcheckfailures("BSLS_REVIEW_INVOKE(\"false\");", "R")
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
//          flags.append("BSLS_ASSERT_%s" % (line[21:41].strip(),))
//      expected = (line[44:45], line[48:49], line[52:53], line[58:59],
//                  line[62:63], line[66:67],)
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
//          expected = (line[40:41], line[44:45], line[48:49], line[54:55],
//                      line[58:59], line[62:63],)
//          printtest(flags, expected)
//..
//----------------------------------------------------------------END GENERATOR


//--------------------------------------------------------------------GENERATED
//========================== (NO BUILD FLAGS SET) ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

    // (THIS LINE INTENTIONALLY LEFT BLANK)

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//=================================== OPT ===================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//=================================== DBG ===================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//================================= DBG OPT =================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//================================== SAFE ===================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//================================ SAFE OPT =================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//================================ SAFE DBG =================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//================================= SAFE_2 ==================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//=============================== SAFE_2 OPT ================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//=============================== SAFE_2 DBG ================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//=============================== SAFE_2 SAFE ===============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//=============================== LEVEL_NONE ================================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//============================= OPT LEVEL_NONE ==============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//============================= DBG LEVEL_NONE ==============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//============================= SAFE LEVEL_NONE =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//============================ SAFE_2 LEVEL_NONE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//======================== SAFE_2 DBG OPT LEVEL_NONE ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//============================ LEVEL_ASSERT_OPT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//========================== OPT LEVEL_ASSERT_OPT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//========================== DBG LEVEL_ASSERT_OPT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//========================== SAFE LEVEL_ASSERT_OPT ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//========================= SAFE_2 LEVEL_ASSERT_OPT =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//============================== LEVEL_ASSERT ===============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//============================ OPT LEVEL_ASSERT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//============================ DBG LEVEL_ASSERT =============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//============================ SAFE LEVEL_ASSERT ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//=========================== SAFE_2 LEVEL_ASSERT ===========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//============================ LEVEL_ASSERT_SAFE ============================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//========================== OPT LEVEL_ASSERT_SAFE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//========================== DBG LEVEL_ASSERT_SAFE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//========================= SAFE LEVEL_ASSERT_SAFE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//======================== SAFE_2 LEVEL_ASSERT_SAFE =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

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

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//======================= LEVEL_ASSUME_OPT LEVEL_NONE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    // BSLS_ASSERT_OPT(false); is undefined behavior

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//===================== LEVEL_ASSUME_ASSERT LEVEL_NONE ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_ASSERT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    // BSLS_ASSERT_OPT(false); is undefined behavior

    // BSLS_ASSERT(false); is undefined behavior

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//====================== LEVEL_ASSUME_SAFE LEVEL_NONE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_SAFE
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    // BSLS_ASSERT_OPT(false); is undefined behavior

    // BSLS_ASSERT(false); is undefined behavior

    // BSLS_ASSERT_SAFE(false); is undefined behavior

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//========================== LEVEL_NONE LEVEL_NONE ==========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//======================= LEVEL_ASSERT_OPT LEVEL_NONE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//========================= LEVEL_ASSERT LEVEL_NONE =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//====================== LEVEL_ASSERT_SAFE LEVEL_NONE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_REVIEW_LEVEL_NONE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//======================= LEVEL_NONE LEVEL_REVIEW_OPT =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//==================== LEVEL_ASSUME_OPT LEVEL_REVIEW_OPT ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//================== LEVEL_ASSUME_ASSERT LEVEL_REVIEW_OPT ===================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//=================== LEVEL_ASSUME_SAFE LEVEL_REVIEW_OPT ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//==================== LEVEL_ASSERT_OPT LEVEL_REVIEW_OPT ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//====================== LEVEL_ASSERT LEVEL_REVIEW_OPT ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//=================== LEVEL_ASSERT_SAFE LEVEL_REVIEW_OPT ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_OPT

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//========================= LEVEL_NONE LEVEL_REVIEW =========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//====================== LEVEL_ASSUME_OPT LEVEL_REVIEW ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//==================== LEVEL_ASSUME_ASSERT LEVEL_REVIEW =====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//===================== LEVEL_ASSUME_SAFE LEVEL_REVIEW ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//====================== LEVEL_ASSERT_OPT LEVEL_REVIEW ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//======================== LEVEL_ASSERT LEVEL_REVIEW ========================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//===================== LEVEL_ASSERT_SAFE LEVEL_REVIEW ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should not be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(true);  }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//====================== LEVEL_NONE LEVEL_REVIEW_SAFE =======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//=================== LEVEL_ASSUME_OPT LEVEL_REVIEW_SAFE ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//================== LEVEL_ASSUME_ASSERT LEVEL_REVIEW_SAFE ==================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//=================== LEVEL_ASSUME_SAFE LEVEL_REVIEW_SAFE ===================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSUME_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//=================== LEVEL_ASSERT_OPT LEVEL_REVIEW_SAFE ====================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//===================== LEVEL_ASSERT LEVEL_REVIEW_SAFE ======================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

//=================== LEVEL_ASSERT_SAFE LEVEL_REVIEW_SAFE ===================//

// [1] Reset all configuration macros

#undef INCLUDED_BSLS_ASSERT_MACRORESET
#include <bsls_assert_macroreset.h>

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE
#define BSLS_REVIEW_LEVEL_REVIEW_SAFE

// [3] Re-include the 'bsls_assert.h' header.

#include <bsls_assert.h>

// [4] Test the 'IS_ACTIVE', 'IS_REVIEW', and 'IS_ASSUMED' macros

#if !defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE should be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW should not be defined
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED should not be defined
#endif

#if !defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE should be defined
#endif

#if !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE should be defined
#endif

// [5] Test that the public assert and review macros have the expected effect.

    try { BSLS_ASSERT_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_ASSERT_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(true);  }
    catch(ReviewFailed)               { ASSERT(false); }

    try { BSLS_REVIEW_OPT(false);       ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW(false);           ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_SAFE(false);      ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }

    try { BSLS_REVIEW_INVOKE("false");  ASSERT(false); }
    catch(AssertFailed)               { ASSERT(false); }
    catch(ReviewFailed)               { ASSERT(true);  }
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
