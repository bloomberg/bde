// bsls_assert.t.cpp                                                  -*-C++-*-
#include <bsls_assert.h>

#include <bsls_asserttestexception.h>
#include <bsls_platform.h>

// Include 'cassert' to make sure no macros conflict between 'bsls_assert.h'
// and 'cassert'.  This test driver does *not* invoke 'assert(expression)'.
#include <cassert>

#include <cstdio>    // 'fprintf'
#include <cstdlib>   // 'atoi'
#include <cstring>   // 'strcmp'
#include <exception> // 'exception'
#include <iostream>  // 'cout'

#ifdef BSLS_PLATFORM_OS_UNIX
#include <signal.h>
#endif

// Note that a portable syntax for 'noreturn' will be available once we have
// access to conforming C++0x compilers.
//# define BSLS_ASSERT_NORETURN [[noreturn]]

#if defined(BSLS_PLATFORM_CMP_MSVC)
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
// Global Concerns:
//: o TBD
//
// Global Assumptions:
//: o TBD
// ----------------------------------------------------------------------------
// [ 6] BSLS_ASSERT_SAFE_IS_ACTIVE
// [ 6] BSLS_ASSERT_IS_ACTIVE
// [ 6] BSLS_ASSERT_OPT_IS_ACTIVE
// [ 2] BSLS_ASSERT_SAFE(X)
// [ 2] BSLS_ASSERT(X)
// [ 2] BSLS_ASSERT_OPT(X)
// [ 4] typedef void (*Handler)(const char *, const char *, int);
// [ 1] static void setFailureHandler(bsls::Assert::Handler function);
// [ 1] static void lockAssertAdministration();
// [ 1] static bsls::Assert::Handler failureHandler();
// [ 1] static void invokeHandler(const char *t, const char *f, int);
// [ 4] static void failAbort(const char *t, const char *f, int line);
// [-3] static void failSleep(const char *t, const char *f, int line);
// [ 4] static void failThrow(const char *t, const char *f, int line);
// [ 5] class bsls::AssertFailureHandlerGuard
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
//
// [ 7] USAGE EXAMPLE: Using Assert Macros
// [ 8] USAGE EXAMPLE: Invoking an assert handler directly
// [ 9] USAGE EXAMPLE: Using Administration Functions
// [ 9] USAGE EXAMPLE: Installing Prefabricated Assert-Handlers
// [10] USAGE EXAMPLE: Creating Your Own Assert-Handler
// [11] USAGE EXAMPLE: Using Scoped Guard
// [12] USAGE EXAMPLE: Using "ASSERT" with 'BDE_BUILD_TARGET_SAFE_2'
//
// [ 1] CONCERN: By default, the 'bsls_assert::failAbort' is used
// [ 2] CONCERN: ASSERT macros are instantiated properly for build targets
// [ 2] CONCERN: all combinations of BDE_BUILD_TARGETs are allowed
// [ 2] CONCERN: any one assert mode overrides all BDE_BUILD_TARGETs
// [ 3] CONCERN: ubiquitously detect multiply-defined assertion-mode flags
// [ 5] CONCERN: that locking does not stop the handlerGuard from working
// [-1] CONCERN: 'bsls::Assert::failAbort' aborts
// [-1] CONCERN: 'bsls::Assert::failAbort' prints to 'stderr' not 'stdout'
// [-2] CONCERN: 'bsls::Assert::failThrow' aborts in non-exception build
// [-2] CONCERN: 'bsls::Assert::failThrow' prints to 'stderr' for NON-EXC
// [-3] CONCERN: 'bsls::Assert::failSleep' sleeps forever
// [-3] CONCERN: 'bsls::Assert::failSleep' prints to 'stderr' not 'stdout'
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline
#define L_ __LINE__                           // current Line number

//=============================================================================
//                    GLOBAL CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

bool globalVerbose         = false;
bool globalVeryVerbose     = false;
bool globalVeryVeryVerbose = false;

static bool globalAssertFiredFlag = false;
static const char *globalText = "";
static const char *globalFile = "";
static int globalLine = -1;

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

#if (defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE) ||                  \
     defined(BSLS_ASSERT_LEVEL_ASSERT)      ||                  \
     defined(BSLS_ASSERT_LEVEL_ASSERT_OPT)  ||                  \
     defined(BSLS_ASSERT_LEVEL_NONE) )
    #define IS_BSLS_ASSERT_MODE_FLAG_DEFINED 1
#else
    #define IS_BSLS_ASSERT_MODE_FLAG_DEFINED 0
#endif

#ifdef BDE_BUILD_TARGET_EXC
#define ASSERTION_TEST_BEGIN                                    \
        try {

#define ASSERTION_TEST_END                                      \
        } catch (std::exception& ) {                            \
            if (verbose) cout << "\nException caught." << endl; \
        }
#else
#define ASSERTION_TEST_BEGIN
#define ASSERTION_TEST_END
#endif

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static void globalReset()
{
    if (globalVeryVeryVerbose)
        cout << "*** globalReset()" << endl;

    globalAssertFiredFlag = false;
    globalText = "";
    globalFile = "";
    globalLine = -1;

    ASSERT( 0 == std::strcmp("", globalText));
    ASSERT( 0 == std::strcmp("", globalFile));
    ASSERT(-1 == globalLine);
}

//-----------------------------------------------------------------------------

BSLS_ASSERT_NORETURN
static void testDriverHandler(const char *text, const char *file, int line)
    // Set the 'globalAssertFiredFlag' to 'true' and store the specified
    // expression 'text', 'file' name, and 'line' number values in
    // 'globalText', globalFile', and 'globalLine', respectively.  Then throw
    // an 'std::exception' object provided that 'BDE_BUILD_TARGET_EXC' is
    // defined; otherwise, abort the program.
{
    if (globalVeryVeryVerbose) {
        cout << "*** testDriverHandler: "; P_(text) P_(file) P(line)
    }

    globalAssertFiredFlag = true;
    globalText = text;
    globalFile = file;
    globalLine = line;

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
static void testDriverPrint(const char *text, const char *file, int line)
    // Format, in verbose mode, the specified expression 'text', 'file' name,
    // and 'line' number the same way as the 'bsls::Assert::failAbort'
    // assertion-failure handler function might, but on 'cout' instead of
    // 'cerr'.  Then throw an 'std::exception' object provided that
    // 'BDE_BUILD_TARGET_EXC' is defined; otherwise, abort the program.

{
    if (globalVeryVeryVerbose) {
        cout << "*** testDriverPrint: "; P_(text) P_(file) P(line)
    }

    if (globalVeryVerbose) {
        std::fprintf(stdout,
                     "Assertion failed: %s, file %s, line %d\n",
                     text, file, line);

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
    // Bogus 'struct' used for testing: calls 'bsls::Assert::failThrow' on
    // destruction to ensure that it does not rethrow with an exception
    // pending (see case -2).

    BadBoy() {
        if (globalVeryVerbose) cout << "BadBoy Created!" << endl;
    }

    ~BadBoy() {
        if (globalVeryVerbose) cout << "BadBoy Destroyed!" << endl;
        bsls::Assert::failThrow("'failThrow' handler called from ~BadBoy",
                                "f.c",
                                9);
     }
};

// Declaration of function that must appear after main in order to test the
// configuration macros.
void TestConfigurationMacros();


//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------
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
// This first usage example attempts to illustrate how one might select each
// of the particular variants, based on the runtime cost of the defensive check
// relative to that of the useful work being done.
//
// Use of the 'BSLS_ASSERT_SAFE' macro is often appropriate when the defensive
// check occurs within the body of an inline function.  The 'BSLS_ASSERT_SAFE'
// macro minimizes the impact on runtime performance as it is instantiated only
// when requested (i.e., by building in "safe mode").  For example, consider
// a light-weight point class 'Kpoint' that maintains 'x' and 'y' coordinates
// in the range '[ -1000 .. 1000 ]':
//..
    // my_kpoint.h
    // ...

    class Kpoint {
        short int d_x;
        short int d_y;
      public:
        Kpoint(short int x, short int y);
            // ...
            // The behavior is undefined unless '-1000 <= x <= 1000'
            // and '-1000 <= y <= 1000'.
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
            // Adjust the size of the underlying hash table to be
            // approximately the current number of elements divided
            // by the specified 'loadFactor'.  The behavior is undefined
            // unless '0 < loadFactor'.
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
// takes, as a scaling factor, an integer that must be a multiple of 100 or
// the behavior is undefined (and might actually execute a trade):
//..

        void executeTrade(int scalingFactor);
            // Execute the current trade using the specified 'scalingFactor'.
            // The behavior is undefined unless '0 <= scalingFactor' and
            // '100' evenly divides 'scalingFactor'.
        // ...
     };
//..
// Because the cost of the two checks is likely not even measurable compared
// to the overhead of accessing databases and executing the trade, and because
// the consequences of specifying a bad scaling factor are virtually
// unbounded, we might choose to implement these defensive checks using
// 'BSLS_ASSERT_OPT' as follow:
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
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// By default, any assertion failure will result in the invocation of the
// 'bsls::Assert::failAbort' handler function.  We can replace this behavior
// with that of one of the other static failure handler methods supplied in
// 'bsls::Assert' as follows.  Let's assume we are at the top of our
// application called 'myMain' (which would typically be 'main'):
//..
    void myMain()
    {
//..
// First observe that the default assertion-failure handler function is, in
// fact, 'bsls::Assert::failAbort':
//..
        ASSERT(&bsls::Assert::failAbort == bsls::Assert::failureHandler());
//..
// Next, we install a new assertion-failure handler function,
// 'bsls::Assert::failSleep', from the suite of "off-the-shelf" handlers
// provided as 'static' methods of 'bsls::Assert':
//..
        bsls::Assert::setFailureHandler(&bsls::Assert::failSleep);
//..
// Observe that 'bsls::Assert::failSleep' is the new, currently-installed
// assertion-failure handler:
//..
        ASSERT(&bsls::Assert::failSleep == bsls::Assert::failureHandler());
//..
// Note that if we were to explicitly invoke the current assertion-failure
// handler as follows:
//..
//  bsls::Assert::invokeHandler("message", "file", 27);  // This will hang!
//..
// the program will hang since 'bsls::Assert::failSleep' repeatedly sleeps for
// a period of time within an infinite loop.  Thus, this assertion-failure
// handler is useful for hanging a process so that a debugger may be attached
// to it.
//
// We may now decide to disable the 'setFailureHandler' method using the
// 'bsls::Assert::lockAssertAdministration()' method to ensure that no one else
// will override our decision globally.  Note, however, that the
// 'bsls::AssertFailureHandlerGuard' is not affected, and can still be used to
// supplant the currently installed handler (see below):
//..
        bsls::Assert::lockAssertAdministration();
//..
// Attempting to change the currently installed handler now will fail:
//..
        bsls::Assert::setFailureHandler(&bsls::Assert::failAbort);

        ASSERT(&bsls::Assert::failAbort != bsls::Assert::failureHandler());

        ASSERT(&bsls::Assert::failSleep == bsls::Assert::failureHandler());
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
    static void ourFailureHandler(const char *text, const char *file, int line)
        // Print the specified expression 'text', 'file' name, and 'line'
        // number to 'stdout' as a comma-separated list, replacing null
        // string-argument values with empty strings (unless printing has been
        // disabled by the 'globalEnableOurPrintingFlag' variable), then
        // unconditionally abort.
    {
        if (!text) {
            text = "";
        }
        if (!file) {
            file = "";
        }
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
// pointer of type 'bsls::Assert::Handler':
//..
        bsls::Assert::Handler f = &ourFailureHandler;
//..
// Now we can install it just as we would any any other handler:
//..
        bsls::Assert::setFailureHandler(f);
//..
// We can now invoke the default handler directly:
//..
        bsls::Assert::invokeHandler("str1", "str2", 3);
    }
//..
// With the resulting output as follows:
//..
//  str1, str2, 3
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
// temporarily, the assertion-failure handler 'bsls::Assert::failThrow', which,
// when invoked, causes an 'bsls::AssertTestException' object to be thrown.
// (Note that we are not advocating this approach for "recovery", but rather
// for an orderly shut-down, or perhaps during testing.)  The
// 'bsls::AssertFailureHandlerGuard' class is provided for just this purpose:
//..
    ASSERT(&bsls::Assert::failAbort == bsls::Assert::failureHandler());

    bsls::AssertFailureHandlerGuard guard(&bsls::Assert::failThrow);

    ASSERT(&bsls::Assert::failThrow == bsls::Assert::failureHandler());
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
                std::cout << "Internal Error: "
                          << e.expression() << ", "
                          << e.filename()   << ", "
                          << e.lineNumber() << std::endl;
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
// caller.  Note that if exceptions are not enabled, 'bsls::Assert::failThrow'
// will behave as 'bsls::Assert::failAbort', and dump core immediately:
//..
// Assertion failed: 0 <= n, file bsls_assert.t.cpp, line 500
// Abort (core dumped)
//..
// Finally note that the 'bsls::AssertFailureHandlerGuard' is not thread-aware.
// In particular, a guard that is created in one thread will also affect the
// failure handlers that are used in other threads.  Care should be taken when
// using this guard when more than a single thread is executing.
//
/// 6. Using (BSLS) "ASSERT" Macros in Conjunction w/ 'BDE_BUILD_TARGET_SAFE_2'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Recall that assertions do not affect binary compatibility; however,
// software built with 'BDE_BUILD_TARGET_SAFE_2' defined need not be binary
// compatible with software built otherwise.  In this final example, we look
// at how we might use the (BSLS) "ASSERT" family of macro's in conjunction
// with code that is incorporated (at compile time) only when the
// 'BDE_BUILD_TARGET_SAFE_2' is defined.
//
// As a simple example, let's consider an elided implementation of a
// singly-linked integer list and its iterator.  Whenever
// 'BDE_BUILD_TARGET_SAFE_2' is defined, we want to defend against the
// possibility that a client mistakenly passes a 'ListIter' object into a
// 'List' object method (e.g., 'List::insert') where that 'ListIter' object
// did not originate from the same 'List' object.
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
// in a sequence of links, but whenever 'BDE_BUILD_TARGET_SAFE_2' is
// defined, also maintains a pointer to its parent 'List' object:
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
        void print(std::ostream& stream)
        {
            stream << '[';
            for (List_Link *p = d_head_p; p; p = p->d_next_p) {
                stream << ' ' << p->d_data;
            }
            stream << " ]" << std::endl;
        }
    };
//..
// Outside of "safe 2 mode", it is possible to pass an iterator object
// obtained from the 'begin' method of one 'List' object into the 'insert'
// method of another, having, perhaps, unexpected results:
//..
    void sillyFunc(bool printFlag)
    {
        List a;
        ListIter aIt = a.begin();
        a.insert(aIt, 1);
        a.insert(aIt, 2);
        a.insert(aIt, 3);

        if (printFlag) {
            std::cout << "a = "; a.print(cout);
        }

        List b;
        ListIter bIt = b.begin();
        a.insert(bIt, 4);       // Oops! Should have been: 'b.insert(bIt, 4);'
        a.insert(bIt, 5);       // Oops!   "     "     "   '    "     "   5  '
        a.insert(bIt, 6);       // Oops!   "     "     "   '    "     "   6  '

        if (printFlag) {
            std::cout << "a = "; a.print(cout);
            std::cout << "b = "; b.print(cout);
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
// 'BDE_BUILD_TARGET_SAFE_2' defined) the undefined behavior would be
// detected and the output would, by default look more like the following:
//..
//  a = [ 3 2 1 ]
//  Assertion failed: this == position.d_parent_p, file my_list.cpp, line 56
//  Abort (core dumped)
//..
// Thereby quickly exposing the misuse by the client.

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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;
    switch (test) { case 0:  // zero is always the leading case
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #6:
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE: Using "ASSERT" with 'BDE_BUILD_TARGET_SAFE_2'
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE #6" << endl
                          << "================" << endl;

        if (verbose) cout << "\n6. Using (BSLS) \"ASSERT\" Macros in "
                            "Conjunction with BDE_BUILD_TARGET_SAFE_2" << endl;

        // See usage examples section at top of this file.

#ifndef BDE_BUILD_TARGET_SAFE_2
        if (veryVerbose) cout << "\tsafe mode 2 is *not* defined" << endl;
        sillyFunc(veryVerbose);
#else
        if (veryVerbose) cout << "\tSAFE MODE 2 *is* defined." << endl;

        // bsls::Assert::setFailureHandler(::testDriverPrint);
                                                          // for usage example
        bsls::Assert::setFailureHandler(::testDriverHandler);
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
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #5:
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE: Using Scoped Guard
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE #5" << endl
                          << "================" << endl;

        if (verbose) cout <<
                      "\n5. Using the bsls::AssertFailureHandlerGuard" << endl;

        // See usage examples section at top of this file.

        ASSERT(&bsls::Assert::failAbort == bsls::Assert::failureHandler());

#ifndef BDE_BUILD_TARGET_OPT
    #if defined(BDE_BUILD_TARGET_EXC) ||                                      \
        defined(BSLS_ASSERT_ENABLE_TEST_CASE_10)

        if (verbose) cout <<
                "\n*** Note that the following 'Internal Error: ... 0 <= n' "
                "message is expected:\n" << endl;

        ASSERT(0 != wrapperFunc(verbose));

    #endif
#endif
        ASSERT(&bsls::Assert::failAbort == bsls::Assert::failureHandler());

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #4:
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE: Creating Your Own Assert-Handler
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE #4" << endl
                          << "================" << endl;

#ifndef BDE_BUILD_TARGET_EXC
        if (verbose) {
            cout <<
               "\tTest disabled as exceptions are NOT enabled.\n"
              "\tCalling the test funciton would abort." << endl;
        }

#else
        if (verbose) cout <<
                            "\n4. Creating a Custom Assertion Handler" << endl;

        // See usage examples section at top of this file.

        globalEnableOurPrintingFlag = veryVerbose;

        ASSERTION_TEST_BEGIN
        ourMain();
        ASSERTION_TEST_END
#endif
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #3:
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE: Using Administration Functions
        //   USAGE EXAMPLE: Installing Prefabricated Assert-Handlers
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE #3" << endl
                          << "================" << endl;

        if (verbose) cout <<
              "\n3. Runtime Configuration of the bsls::Assert Facility"<< endl;

        // See usage examples section at top of this file.

        myMain();

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #2:
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE: Invoking an assert handler directly
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE #2" << endl
                          << "================" << endl;

        if (verbose) cout <<
         "\n2. When and How to Call the Invoke-Handler Method Directly"<< endl;

        // See usage examples section at top of this file.

        bsls::Assert::setFailureHandler(::testDriverPrint);

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
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE #1:
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE: Using Assert Macros
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE #1" << endl
                          << "================" << endl;

        if (verbose) cout << "\n1. Using BSLS_ASSERT, BSLS_ASSERT_SAFE, and "
                             "BSLS_ASSERT_OPT" << endl;

        // See usage examples section at top of this file.

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // CONFIGURATION MACROS:
        //
        // Concerns:
        //   The configuration macros that report on which assert facilities
        //   are available in the current build mode might not report correctly
        //   in all build modes.
        //
        // Plan:
        //   Subvert the regular .
        //
        // Testing:
        //   BSLS_ASSERT_SAFE_IS_ACTIVE
        //   BSLS_ASSERT_IS_ACTIVE
        //   BSLS_ASSERT_OPT_IS_ACTIVE
        // --------------------------------------------------------------------

        TestConfigurationMacros();
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // FAILURE HANDLER GUARD
        //
        // Concerns:
        //   1. That the guard swaps new for current handler at construction.
        //   2. Restores the original one at destruction.
        //   3. That guards can nest.
        //   4. That 'lockAssertAdministration' has no effect on guard.
        //
        // Plan:
        // Create a guard, passing it the 'testDriverHandler' handler, and
        // verify, using 'failureHandler', that this new handler was installed.
        // Then lock the administration, and repeat in nested fashion with the
        // 'failSleep' handler.  Verify restoration on the way out.
        //
        // Testing:
        //   class bsls::AssertFailureHandlerGuard
        //   CONCERN: that locking does not stop the handlerGuard from working
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "FAILURE HANDLER GUARD" << endl
                          << "=====================" << endl;

        if (verbose) cout << "\nVerify initial assert handler." << endl;

        ASSERT(bsls::Assert::failAbort == bsls::Assert::failureHandler());

        if (verbose) cout << "\nCreate guard with 'testDriverHandler' handler."
                                                                       << endl;
        {
            bsls::AssertFailureHandlerGuard guard(::testDriverHandler);

            if (verbose) cout << "\nVerify new assert handler." << endl;

            ASSERT(::testDriverHandler == bsls::Assert::failureHandler());

            if (verbose) cout << "\nLock administration." << endl;

            bsls::Assert::lockAssertAdministration();

            if (verbose) cout << "\nRe-verify new assert handler." << endl;

            ASSERT(testDriverHandler == bsls::Assert::failureHandler());

            if (verbose) cout <<
                     "\nCreate second guard with 'failSleep' handler." << endl;
            {
                bsls::AssertFailureHandlerGuard guard(bsls::Assert::failSleep);

                if (verbose) cout << "\nVerify newer assert handler." << endl;

                ASSERT(bsls::Assert::failSleep ==
                                               bsls::Assert::failureHandler());

                if (verbose) cout <<
                 "\nDestroy guard created with '::failSleep' handler." << endl;
            }

            if (verbose) cout << "\nVerify new assert handler." << endl;

            ASSERT(::testDriverHandler == bsls::Assert::failureHandler());

            if (verbose) cout <<
                  "\nDestroy guard created with '::testDriverHandler' handler."
                                                                       << endl;
        }

        if (verbose) cout << "\nVerify initial assert handler." << endl;

        ASSERT(bsls::Assert::failAbort == bsls::Assert::failureHandler());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // ASSERTION FAILURE HANDLERS
        //
        // Concerns:
        //   1. That each of the assertion failure handlers provided herein
        //      behaves as advertized and (at least) matches the signature
        //      of the 'bsls::Assert::Handler' 'typedef'
        //
        // Plan:
        //   1. Verify each handler's behavior.  Unfortunately, we cannot
        //      test functions that abort except by hand (see negative test
        //      cases).
        //   2. Assign each handler function to a pointer of type 'Handler'.
        //
        // Testing:
        //   typedef void (*Handler)(const char *, const char *, int);
        //   static void failAbort(const char *t, const char *f, int line);
        //   static void failThrow(const char *t, const char *f, int line);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ASSERTION FAILURE HANDLERS" << endl
                          << "==========================" << endl;

        if (verbose) cout << "\nTesting 'void failAbort(const char *t, "
                             "const char *f, int line);'" << endl;
        {
            bsls::Assert::Handler f = bsls::Assert::failAbort;
            (void) f;

            if (veryVerbose) {
                cout << "\t(Aborting behavior must be tested by hand.)" <<
                                                                          endl;
            }
        }

        if (verbose) cout << "\nTesting 'void failThrow(const char *t, "
                             "const char *f, int line);'" << endl;
        {

            bsls::Assert::Handler f = bsls::Assert::failThrow;

#ifdef BDE_BUILD_TARGET_EXC
            const char *text = "Test text";
            const char *file = "bsls_assert.t.cpp";
            int   line = 101;

            if (veryVerbose) {
                cout << "\tExceptions ARE enabled." << endl;
            }

            try {
                f(text, file, line);
            }
            catch (bsls::AssertTestException) {
                if (veryVerbose) cout << "\tException Text Succeeded!" << endl;
            }
            catch (...) {
                ASSERT("Through wrong exception!" && 0);
            }
#else
            if (veryVerbose) {
                cout << "\tExceptions are NOT enabled." << endl;
            }
#endif
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // INCOMPATIBLE BUILD TARGETS
        //
        // Concerns:
        //   1. Any component including 'bsls_assert.h' that has multiple
        //        assertion-mode flags defined should fail to compile
        //        and provide a useful diagnostic.
        //
        // Plan:
        //   1. Repeat the assertions at runtime, and fail if any two macros
        //        incompatible macros are present.
        //   2. (Manually) observe that the test is in the bsls_assert.h file.
        //
        // Testing:
        //   CONCERN: ubiquitously detect multiply-defined assertion-mode flags
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INCOMPATIBLE BUILD TARGETS" << endl
                          << "==========================" << endl;

        if (verbose) cout << "\nExtract defined-ness of each target." << endl;

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

        if (veryVerbose) cout << "\tVerify all combinations redundantly."
                              << endl;

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
        //   1. Each assert macro instantiates only when the appropriate build
        //        mode is set.
        //   2. When instantiated, each macro fires only on "0" valued
        //        expressions
        //   3. When a macro fires, the correct text, line, and file are
        //        transmitted to the current assertion-failure handler.
        //   4. That the expression text that is printed is exactly what is in
        //        the parentheses of the macro.
        //
        // Plan:
        //   1. We must not try to change build targets (or assert modes),
        //        but just observe them two see which assert macros should
        //        be instantiated.
        //   2. When enabled, we need to try each of the macros on each of the
        //      (four) kinds of expression text arguments to make sure that it
        //      fires only on 'false' and '(void *)(0)' and not 'true' or
        //      (void *)(1).
        //   3. In each case for 2. (above) that fires, we will observe that
        //        the expression text, file name, and line number are correct.
        //   4. Make sure that we vary the text in the expression (and include
        //        embedded whitespace (we don't care about leading or trailing
        //        whitespace).
        //
        // Testing:
        //   BSLS_ASSERT_SAFE(X)
        //   BSLS_ASSERT(X)
        //   BSLS_ASSERT_OPT(X)
        //   CONCERN: ASSERT macros are instantiated properly for build targets
        //   CONCERN: all combinations of BDE_BUILD_TARGETs are allowed
        //   CONCERN: any one assert mode overrides all BDE_BUILD_TARGETs
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ASSERT-MACRO TEST" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nInstall 'testDriverHandler' assertion-handler."
                                                                       << endl;

        bsls::Assert::setFailureHandler(&testDriverHandler);
        ASSERT(::testDriverHandler == bsls::Assert::failureHandler());

        if (veryVerbose) cout << "\tSet up all but line numbers now. " << endl;

        const void *p    = 0;
        const char *text = "p";
        const char *file = __FILE__;
        int         line;                    // initialized each time

        const char *expr = "false == true";

#ifndef BDE_BUILD_TARGET_EXC
        globalReturnOnTestAssert = true;
#endif

        if (verbose) {
            cout << "\nCurrent build-mode settings:" << endl;

#ifdef BDE_BUILD_TARGET_SAFE_2
            cout << "\t1 == BDE_BUILD_TARGET_SAFE_2" << endl;
#else
            cout << "\t0 == BDE_BUILD_TARGET_SAFE_2" << endl;
#endif

#ifdef BDE_BUILD_TARGET_SAFE
            cout << '\t' << "\t1 == BDE_BUILD_TARGET_SAFE" << endl;
#else
            cout << '\t' << "\t0 == BDE_BUILD_TARGET_SAFE" << endl;
#endif

#ifdef BDE_BUILD_TARGET_DBG
            cout << "\t1 == BDE_BUILD_TARGET_DBG" << endl;
#else
            cout << "\t0 == BDE_BUILD_TARGET_DBG" << endl;
#endif

#ifdef BSLS_ASSERT_LEVEL_ASSERT_SAFE
            cout << "\t1 == BSLS_ASSERT_LEVEL_ASSERT_SAFE" << endl;
#else
            cout << "\t0 == BSLS_ASSERT_LEVEL_ASSERT_SAFE" << endl;
#endif

#ifdef BSLS_ASSERT_LEVEL_ASSERT
            cout << "\t1 == BSLS_ASSERT_LEVEL_ASSERT" << endl;
#else
            cout << "\t0 == BSLS_ASSERT_LEVEL_ASSERT" << endl;
#endif

#ifdef BSLS_ASSERT_LEVEL_ASSERT_OPT
            cout << "\t1 == BSLS_ASSERT_LEVEL_ASSERT_OPT" << endl;
#else
            cout << "\t0 == BSLS_ASSERT_LEVEL_ASSERT_OPT" << endl;
#endif

#ifdef BSLS_ASSERT_LEVEL_NONE
            cout << "\t1 == BSLS_ASSERT_LEVEL_NONE" << endl;
#else
            cout << "\t0 == BSLS_ASSERT_LEVEL_NONE" << endl;
#endif

#if IS_BSLS_ASSERT_MODE_FLAG_DEFINED
            cout << "\t1 == IS_BSLS_ASSERT_MODE_FLAG_DEFINED" << endl;
#else
            cout << "\t0 == IS_BSLS_ASSERT_MODE_FLAG_DEFINED" << endl;
#endif

#if BSLS_NO_ASSERTION_MACROS_DEFINED
            cout << "\t1 == BSLS_NO_ASSERTION_MACROS_DEFINED" << endl;
#else
            cout << "\t0 == BSLS_NO_ASSERTION_MACROS_DEFINED" << endl;
#endif
        }

        //_____________________________________________________________________
        //            BSLS_ASSERT_SAFE, BSLS_ASSERT, BSLS_ASSERT_OPT
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE)                              \
 || !IS_BSLS_ASSERT_MODE_FLAG_DEFINED && (                              \
        defined(BDE_BUILD_TARGET_SAFE_2) ||                             \
        defined(BDE_BUILD_TARGET_SAFE)   )

        if (verbose) cout <<
                          "\nEnabled: ASSERT_SAFE, ASSERT, ASSERT_OPT" << endl;

        if (veryVerbose) cout << "\tCheck for integer expression." << endl;

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

        if (veryVerbose) cout << "\tCheck for pointer expression." << endl;

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

        if (veryVerbose) cout << "\tCheck for expression with spaces." << endl;

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

#if defined(BSLS_ASSERT_LEVEL_ASSERT)                                   \
 || !IS_BSLS_ASSERT_MODE_FLAG_DEFINED &&                                \
        !defined(BDE_BUILD_TARGET_OPT) &&                               \
        !defined(BDE_BUILD_TARGET_SAFE) &&                              \
        !defined(BDE_BUILD_TARGET_SAFE_2)

        if (verbose) cout << "\nEnabled: ASSERT, ASSERT_OPT" << endl;

        if (veryVerbose) cout << "\tCheck for integer expression." << endl;

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

        if (veryVerbose) cout << "\tCheck for pointer expression." << endl;

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

        if (veryVerbose) cout << "\tCheck for expression with spaces." << endl;

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

#if defined(BSLS_ASSERT_LEVEL_ASSERT_OPT)                               \
 || !IS_BSLS_ASSERT_MODE_FLAG_DEFINED &&                                \
        defined(BDE_BUILD_TARGET_OPT) &&                                \
        !defined(BDE_BUILD_TARGET_SAFE) &&                              \
        !defined(BDE_BUILD_TARGET_SAFE_2)

        if (verbose) cout << "\nEnabled: ASSERT_OPT" << endl;

        if (veryVerbose) cout << "\tCheck for integer expression." << endl;

        globalReset(); BSLS_ASSERT_SAFE(0); ASSERT(0 == globalAssertFiredFlag);
        globalReset(); BSLS_ASSERT     (0); ASSERT(0 == globalAssertFiredFlag);

        globalReset();
        ASSERTION_TEST_BEGIN
        BSLS_ASSERT_OPT (0);
        ASSERTION_TEST_END
        ASSERT(1 == globalAssertFiredFlag);

        if (veryVerbose) cout << "\tCheck for pointer expression." << endl;

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

        if (veryVerbose) cout << "\tCheck for expression with spaces." << endl;

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

        if (verbose) cout << "\nEnabled: (* Nothing *)" << endl;

        if (veryVerbose) cout << "\tCheck for integer expression." << endl;

        globalReset(); BSLS_ASSERT_SAFE(0); ASSERT(0 == globalAssertFiredFlag);
        globalReset(); BSLS_ASSERT     (0); ASSERT(0 == globalAssertFiredFlag);
        globalReset(); BSLS_ASSERT_OPT (0); ASSERT(0 == globalAssertFiredFlag);

        if (veryVerbose) cout << "\tCheck for integer expression." << endl;

        globalReset(); BSLS_ASSERT_SAFE(p); ASSERT(0 == globalAssertFiredFlag);

        globalReset(); BSLS_ASSERT     (p); ASSERT(0 == globalAssertFiredFlag);

        globalReset(); BSLS_ASSERT_OPT (p); ASSERT(0 == globalAssertFiredFlag);

        if (veryVerbose) cout << "\tCheck for expression with spaces." << endl;

        globalReset(); BSLS_ASSERT_SAFE(false == true);
        ASSERT(0 == globalAssertFiredFlag);

        globalReset(); BSLS_ASSERT     (false == true);
        ASSERT(0 == globalAssertFiredFlag);

        globalReset(); BSLS_ASSERT_OPT (false == true);
        ASSERT(0 == globalAssertFiredFlag);
#endif

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Want to observe the basic operation of this component in "default
        //   mode" (i.e., with no build flags specified).
        //
        // Plan:
        //   Call 'setAssertHandler' to install the 'testDriverHandler'
        //   "assert" function in order to observe that the installed function
        //   was called using the 'invokeHandler' method -- and, contingently,
        //   the 'BSLS_ASSERT_OPT(X)' macro -- with various arguments.
        //
        // Testing:
        //   BREATHING TEST
        //   CONCERN: By default, the 'bde_assert::failAbort' is used.
        //   static void setFailureHandler(bsls::Assert::Handler function);
        //   static bsls::Assert::Handler failureHandler();
        //   static void invokeHandler(const char *t, const char *f, int);
        //   static void lockAssertAdministration();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout <<
           "\nVerify that the correct assert callback is installed by default."
                                                                       << endl;

        ASSERT(bsls::Assert::failAbort == bsls::Assert::failureHandler());

        if (verbose) cout <<
           "\nVerify that we can install a new assert callback." << endl;

        bsls::Assert::setFailureHandler(&testDriverHandler);
        ASSERT(::testDriverHandler == bsls::Assert::failureHandler());

        if (verbose) cout <<
           "\nVerify that 'invokeHandler' properly transmits its arguments."
                                                                       << endl;

        globalReset();
        ASSERT(false        == globalAssertFiredFlag);

#ifndef BDE_BUILD_TARGET_EXC
        globalReturnOnTestAssert = true;
#endif

        ASSERTION_TEST_BEGIN
        bsls::Assert::invokeHandler("ExPrEsSiOn", "FiLe", -12345678);
        ASSERTION_TEST_END

        ASSERT(     true == globalAssertFiredFlag);
        ASSERT(        0 == std::strcmp("ExPrEsSiOn", globalText));
        ASSERT(        0 == std::strcmp("FiLe",       globalFile));
        ASSERT(-12345678 == globalLine);

#ifndef BDE_BUILD_TARGET_EXC
        globalReturnOnTestAssert = false;
#endif

        if (verbose) cout <<
           "\nVerify that 'lockAssertAdminisration' blocks callback changes."
                                                                       << endl;

        bsls::Assert::lockAssertAdministration();

        bsls::Assert::setFailureHandler(&bsls::Assert::failAbort);
        ASSERT(::testDriverHandler == bsls::Assert::failureHandler());

#ifdef BSLS_ASSERT_LEVEL_NONE
        if (verbose) cout <<
          "\n'BSLS_ASSERT_LEVEL_NONE' is defined; exit breathing test."
                                                                       << endl;
        break;
#endif
        if (verbose) cout <<
          "\nVerify that 'BSLS_ASSERT_OPT' doesn't fire for '!0' expressions."
                                                                       << endl;

        if (veryVerbose) cout << "\tInteger-valued expression" << endl;
        {
            globalReset();
            ASSERT(false == globalAssertFiredFlag);

            BSLS_ASSERT_OPT(true == true);
            ASSERT(false == globalAssertFiredFlag);
        }

        if (veryVerbose) cout << "\tPointer-valued expression" << endl;
        {
            globalReset();

            ASSERT(false == globalAssertFiredFlag);

            BSLS_ASSERT_OPT((void *)(1));
            ASSERT(false == globalAssertFiredFlag);
        }

        if (verbose) cout <<
           "\nVerify that 'BSLS_ASSERT_OPT' does fire for '0' expressions."
                                                                       << endl;

#ifndef BDE_BUILD_TARGET_EXC
            globalReturnOnTestAssert = true;
#endif

        if (veryVerbose) cout << "\tInteger-valued expression" << endl;
        {
            globalReset();
            ASSERT(false == globalAssertFiredFlag);

            const char *const text = "true == false";
            const char *const file = __FILE__;
                  int         line = -1;

            line = L_ + 2;
            ASSERTION_TEST_BEGIN
            BSLS_ASSERT_OPT(true == false);
            ASSERTION_TEST_END

            ASSERT(true == globalAssertFiredFlag);
            LOOP2_ASSERT(text, globalText, 0 == std::strcmp(text, globalText));
            LOOP2_ASSERT(file, globalFile, 0 == std::strcmp(file, globalFile));
            LOOP2_ASSERT(line, globalLine, line == globalLine);
        }

        if (veryVerbose) cout << "\tPointer-valued expression" << endl;
        {
            globalReset();
            ASSERT(false == globalAssertFiredFlag);

            const char *const text = "(void *)(0)";
            const char *const file = __FILE__;
                  int         line = -1;

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
        //   1. That it does abort the program.
        //   2. That it prints a message to 'stderr'.
        //
        // Plan:
        //   Call 'bsls::Assert::failAbort' after blocking the signal.
        //
        // Testing:
        //   CONCERN: 'bsls::Assert::failAbort' aborts
        //   CONCERN: 'bsls::Assert::failAbort' prints to 'cerr' not 'cout'
        // --------------------------------------------------------------------

        cout << endl << "Manual Testing 'bsls::Assert::failAbort'" << endl
                     << "========================================" << endl;

#ifdef BSLS_PLATFORM_OS_UNIX
        sigset_t newset;
        sigaddset(&newset, SIGABRT);

    #if defined(BDE_BUILD_TARGET_MT)
        pthread_sigmask(SIG_BLOCK, &newset, 0);
    #else
        sigprocmask(SIG_BLOCK, &newset, 0);
    #endif

#endif
        cerr << "THE FOLLOWING SHOULD PRINT ON STDERR:\n"
                "Assertion failed: 0 != 0, file myfile.cpp, line 123" << endl;

        bsls::Assert::failAbort("0 != 0", "myfile.cpp", 123);

        ASSERT(0 && "Should not be reached");
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // CALL FAIL THROW HANDLER
        //
        // Concerns:
        //   1a) That it does *not* throw for an exception build when there
        //        is an exception pending.
        //
        //   1b) That it behaves as failAbort for non-exception builds.
        //
        // Plan:
        //   1a) Call bsls::Assert::failThrow from within the dtor of a
        //       test object on the stack after a throw.
        //
        //   1b) Call 'bsls::Assert::failAbort' after blocking the signal.
        //
        // Testing:
        //   CONCERN: 'bsls::Assert::failAbort' aborts
        //   CONCERN: 'bsls::Assert::failAbort' prints to 'cerr' not 'cout'
        // --------------------------------------------------------------------

        cout << endl << "Manual Testing 'bsls::Assert::failThrow'" << endl
                     << "========================================" << endl;

#if BDE_BUILD_TARGET_EXC

        cout << "\nEXCEPTION BUILD" << endl;

        cerr << "\nTHE FOLLOWING SHOULD PRINT ON STDERR:\n"
                "BSLS_ASSERTION ERROR: An uncaught exception is pending;"
                " cannot throw 'bsls_asserttestexception'." << endl;
        cerr <<
  "assertion failed: 'failThrow' handler called from ~BadBoy, file f.c, line 9"
             << endl;

        try {
            BadBoy bad;       // calls 'bsls::Assert::failThrow' on destruction

            if (veryVerbose) cout << "About to throw \"stuff\"" << endl;

            throw "stuff";
        }
        catch (...) {
            ASSERT("We should not have caught this exception." && 0);
        }

        ASSERT("We should not have made it to here either." && 0);
#else
        cout << "\nNON-EXCEPTION BUILD" << endl;

  #ifdef BSLS_PLATFORM_OS_UNIX
        sigset_t newset;
        sigaddset(&newset, SIGABRT);

    #if defined(BDE_BUILD_TARGET_MT)
        pthread_sigmask(SIG_BLOCK, &newset, 0);
    #else
        sigprocmask(SIG_BLOCK, &newset, 0);
    #endif

  #endif
        cerr << "THE FOLLOWING SHOULD PRINT ON STDERR:\n"
                "Assertion failed: 0 != 0, file myfile.cpp, line 123" << endl;

        bsls::Assert::failAbort("0 != 0", "myfile.cpp", 123);

        ASSERT(0 && "Should not be reached");
#endif
      } break;
      case -3: {
        // --------------------------------------------------------------------
        // CALL FAIL SLEEP HANDLER
        //
        // Concerns:
        //   1. That it does sleep forever.
        //   2. That it prints a message to 'stderr'.
        //
        // Plan:
        //   Call 'bsls::Assert::failSleep'.  Then observe that a diagnostic is
        //   printed to 'stderr' and the program hangs.
        //
        // Testing:
        //   CONCERN: 'bsls::Assert::failSleep' sleeps forever
        //   CONCERN: 'bsls::Assert::failSleep' prints to 'cerr' not 'cout'
        // --------------------------------------------------------------------

        cout << endl << "Manual Testing 'bsls::Assert::failSleep'" << endl
                     << "========================================" << endl;

        cerr << "THE FOLLOWING SHOULD PRINT ON STDERR (BEFORE HANGING):\n"
                "Assertion failed: 0 != 0, file myfile.cpp, line 123" << endl;

        bsls::Assert::failSleep("0 != 0", "myfile.cpp", 123);

        ASSERT(0 && "Should not be reached");
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
// header.  This means that, most unusually, we will '#undef' the header-guard
// in order to support repeated inclusion.  Note that '#include'ing a header
// inside a function definition, as we do below, will flag an error for any
// construct that is not supported inside a function definition, such as
// declaring a template or defining a "local" function.  consequently, we must
// provide a deeper "include-guard" inside the component header itself to
// protect the non-macro parts of this component against the repeated
// inclusion.
//
// For each test iteration that '#include <bsls_assert.h>', each of the macros
// listed above should be undefined, along with each of the following that are
// also defined within this header:
//   BSLS_ASSERT_SAFE
//   BSLS_ASSERT
//   BSLS_ASSERT_OPT
//   BSLS_ASSERT_ASSERT
//
// Note that each test contains a certain amount of "boilerplate" code that
// looks like it might be refactored into a common function or two.  This would
// be a mistake, as the "bodies" that looks similar will actually have quite
// meanings as the macros expand in different ways according to the
// configuration under test.  For example, extracting some of this code into
// a common function passed the expected test result would see that function
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

namespace
{

#if defined(BDE_BUILD_TARGET_EXC)
struct AssertFailed {
    // This struct contains a static function suitable for registration as an
    // assert handler, and provides a distinct "empty" type that may be thrown
    // from the handler and caught within the test cases below, in order to
    // confirm if the appropriate 'BSLS_ASSERT_*' macros are enabled or not.
    BSLS_ASSERT_NORETURN
    static void failMacroTest(const char *, const char *, int) {
        throw AssertFailed();
    }
};
#else
    // Without exception support, we cannot fail an assert-test by throwing
    // an exception.  The most practical solution is to simply not compile
    // those tests, so we do not supply an 'AssertFailed' alternative, to be
    // sure to catch any compile-time use of this structure in exception-free
    // builds.
#endif
}  // close unnamed namespace

#undef BSLS_ASSERT_NORETURN

void TestConfigurationMacros()
{

    if (globalVerbose) cout << endl
                            << "CONFIGURATION MACROS" << endl
                            << "====================" << endl;

#if !defined(BDE_BUILD_TARGET_EXC)
    if (globalVerbose)
        cout << "\nThis case is not run as it relies on exception support."
             << endl;
#else
    if (globalVerbose) cout << "\nWe need to write a running commentary"
                            << endl;

    bsls::Assert::setFailureHandler(&AssertFailed::failMacroTest);

//===================== (NO BUILD FLAGS SET) ===============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

    // (THIS LINE INTENTIONALLY LEFT BLANK)

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== OPT ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== DBG ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== DBG OPT ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//========================== SAFE ==========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE OPT ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE DBG ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE DBG OPT ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//========================== SAFE_2 ==========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 OPT ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BDE_BUILD_TARGET_SAFE_2

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 DBG ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_SAFE_2

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 DBG OPT ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//========================== SAFE_2 SAFE ==========================//


// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 SAFE OPT ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 SAFE DBG ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 SAFE DBG OPT ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//- - - - - - - - - - - LEVEL_NONE - - - - - - - - - - - - - - - - - - - - -//

//===================== LEVEL_NONE ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== OPT LEVEL_NONE ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== DBG LEVEL_NONE ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== DBG OPT LEVEL_NONE ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE LEVEL_NONE ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE OPT LEVEL_NONE ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE DBG LEVEL_NONE ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE DBG OPT LEVEL_NONE ============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 LEVEL_NONE ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 OPT LEVEL_NONE ==============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 DBG LEVEL_NONE ==============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 DBG OPT LEVEL_NONE ==========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//======================= SAFE_2 SAFE LEVEL_NONE ===========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//=================== SAFE_2 SAFE OPT LEVEL_NONE ===========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//==================== SAFE_2 SAFE DBG LEVEL_NONE ==========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//=================== SAFE_2 SAFE DBG OPT LEVEL_NONE =======================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_NONE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//- - - - - - - - - - - LEVEL_ASSERT_OPT - - - - - - - - - - - - - - - -//

//===================== LEVEL_ASSERT_OPT ===============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== OPT LEVEL_ASSERT_OPT ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== DBG LEVEL_ASSERT_OPT ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== DBG OPT LEVEL_ASSERT_OPT ============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE LEVEL_ASSERT_OPT ===============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE OPT LEVEL_ASSERT_OPT ===========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE DBG LEVEL_ASSERT_OPT ===========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE DBG OPT LEVEL_ASSERT_OPT =======================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 LEVEL_ASSERT_OPT =============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 OPT LEVEL_ASSERT_OPT =========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 DBG LEVEL_ASSERT_OPT =========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 DBG OPT LEVEL_ASSERT_OPT =====================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 SAFE LEVEL_ASSERT_OPT ========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 SAFE OPT LEVEL_ASSERT_OPT ====================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 SAFE DBG LEVEL_ASSERT_OPT ====================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//==================== SAFE_2 SAFE DBG OPT LEVEL_ASSERT_OPT =================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_OPT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//- - - - - - - - - - - LEVEL_ASSERT - - - - - - - - - - - - - - - -//

//===================== LEVEL_ASSERT ===============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== OPT LEVEL_ASSERT ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== DBG LEVEL_ASSERT ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== DBG OPT LEVEL_ASSERT ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE LEVEL_ASSERT ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE OPT LEVEL_ASSERT ===============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE DBG LEVEL_ASSERT ===============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE DBG OPT LEVEL_ASSERT ===========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 LEVEL_ASSERT ================================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 OPT LEVEL_ASSERT =============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 DBG LEVEL_ASSERT =============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 DBG OPT LEVEL_ASSERT =========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 SAFE LEVEL_ASSERT ============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 SAFE OPT LEVEL_ASSERT ========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 SAFE DBG LEVEL_ASSERT ========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//==================== SAFE_2 SAFE DBG OPT LEVEL_ASSERT =====================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//- - - - - - - - - - - LEVEL_ASSERT_SAFE - - - - - - - - - - - - - - - -//

//===================== LEVEL_ASSERT_SAFE ===============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== OPT LEVEL_ASSERT_SAFE ===============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== DBG LEVEL_ASSERT_SAFE ===============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== DBG OPT LEVEL_ASSERT_SAFE ===========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE LEVEL_ASSERT_SAFE ==============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE OPT LEVEL_ASSERT_SAFE ==========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE DBG LEVEL_ASSERT_SAFE ==========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE DBG OPT LEVEL_ASSERT_SAFE ======================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 LEVEL_ASSERT_SAFE ============================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 OPT LEVEL_ASSERT_SAFE ========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 DBG LEVEL_ASSERT_SAFE ========================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 DBG OPT LEVEL_ASSERT_SAFE ====================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 SAFE LEVEL_ASSERT_SAFE =======================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//==================== SAFE_2 SAFE OPT LEVEL_ASSERT_SAFE ====================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//===================== SAFE_2 SAFE DBG LEVEL_ASSERT_SAFE ===================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//==================== SAFE_2 SAFE DBG OPT LEVEL_ASSERT_SAFE ================//

// [1] Reset all configuration macros

#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE

// [2] Define the macros for this test case.

#define BDE_BUILD_TARGET_SAFE_2
#define BDE_BUILD_TARGET_SAFE
#define BDE_BUILD_TARGET_DBG
#define BDE_BUILD_TARGET_OPT
#define BSLS_ASSERT_LEVEL_ASSERT_SAFE

// [3] Re-include the bsls_assert header.

#undef INCLUDED_BSLS_ASSERT
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


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -//
#endif  // defined BDE_BUILD_TARGET_EXC
}
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
