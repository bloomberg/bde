// bsls_exceptionutil.t.cpp                                           -*-C++-*-

#include <bsls_exceptionutil.h>
#include <bsls_platform.h>
#include <bsls_bsltestutil.h>

#include <setjmp.h>
#include <signal.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// 'bsls_exceptionutil' provides macros that define 'try', 'catch', and
// 'throw' statements in exception enabled builds, but define, respectively,
// unconditionally executed if-statements, calls to the assert handler, and
// unconditionally non-executed else-statements, in non-exception enabled
// builds.  Because this component defines many elements that depend on the
// build mode, the tests performed vary with the build mode, and the
// test-driver must be run in both exception and non-exception build modes in
// order to test the full range of functionality for the component.
//
// For exception enabled builds, we do not attempt to exhaustively test
// language behavior, but simply verify 'BSLS_TRY', 'BSLS_CATCH',
// 'BSLS_THROW', and 'BSLS_RETHROW' generally behave like their respective
// language constructs.  These macros can be tested independently by, e.g.,
// testing 'BSLS_TRY', with direct use of 'throw' and 'catch'.
//
// For non-exception builds, 'BSLS_TRY' and 'BSLS_CATCH' either define
// executed, or non-executed, blocks of code, and are fairly straightforward
// to test.  'BSLS_THROW' and 'BSLS_RETHROW' invoke the 'bsls_assert'
// assert-handler, unfortunately as this is in a non-exception build we cannot
// throw an exception from the assert handler, and
// 'bsls::Assert::invokeHandler' is marked 'noreturn' so we cannot simply
// define an assert-handler that returns.  Instead, this component defines an
// assert-handler function that records its arguments and aborts, and also
// defines 'BEGIN_ABORT_TEST' and 'END_ABORT_TEST_AND_ASSERT' macros that use
// set-jump and long-jump to recover from a potential abort, and then verify
// whether an abort occurred.
//-----------------------------------------------------------------------------
// [ 3] BSLS_TRY
// [ 4] BSLS_THROW
// [ 5] BSLS_CATCH
// [ 6] BSLS_RETHROW
// [ 7] BSLS_NOTHROW_SPEC
// [ 7] BSLS_EXCEPTION_SPEC
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
// [ 2] TESTING: ABORT HANDLING MACROS

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

//-----------------------------------------------------------------------------

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;

// We define 'BDE_BUILD_TARGET_EXC_ACTUAL' to hold the original
// exception-enable build mode flag to allow, to the extent possible, testing
// multiple build configurations in a given test run.
#define BDE_BUILD_TARGET_EXC_ACTUAL BDE_BUILD_TARGET_EXC

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

enum Action { ACTION_NONE, ACTION_THROW, ACTION_ABORT, ACTION_ERROR };

class TestExceptionClass { };

// The following global variables are managed by the assertHandler function.
const char *g_assertMessage;
const char *g_assertFile;
int         g_assertLine;

void resetAssertHandler()
{
    g_assertMessage = 0;
    g_assertFile    = 0;
    g_assertLine    = -1;
}

void assertHandler(const char *message, const char *file, int line)
    // Call 'abort' and record the specified 'message' to 'g_assertMessage',
    // the specified 'file' to 'g_assertFile', and the specified 'line' to
    // 'g_line'.  Note that 'Assert::invokeHandler' (which will be configured
    // by this test driver to call this function) is marked 'noreturn', so
    // this function cannot return; throwing an exception is also not an
    // option as this function is called by this test-driver in non-exception
    // builds.
{
    g_assertMessage = message;
    g_assertFile    = file;
    g_assertLine    = line;
    abort();
}


#ifdef BSLS_PLATFORM_OS_WINDOWS
typedef jmp_buf JumpBuffer;
#define setJump(X) setjmp((X))
#else
typedef sigjmp_buf JumpBuffer;
#define setJump(X) sigsetjmp((X), 1)
#endif

// The following global variables are managed by the 'abortSignalHandler'
// defined below.

static JumpBuffer    g_jumpBuffer;
static volatile bool g_inTest = false;

extern "C" {

void abortSignalHandler(int /* x */)
   // If the global variable 'g_inTest' is 'true' then long jump to the global
   // jump buffer 'g_jumpBuffer', otherwise this function has no effect (and,
   // if the function were called to handle an abort signal, the task will
   // terminate).  Note that this function is designed to be installed as a
   // signal handler for the 'SIGABRT' (abort) signal, and that the resulting
   // set-jump return code can be used to determine whether this handler was
   // called.
{
    if (g_inTest) {
#ifdef BSLS_PLATFORM_OS_WINDOWS
        longjmp(g_jumpBuffer, 1);
#else
        siglongjmp(g_jumpBuffer, 1);
#endif
    }
    else {
        BSLS_ASSERT(false);
    }
}

}

void installAbortHandler()
   // Install a abort-signal handler and mark 'g_inTest' as 'true'.

{
    // Set the abort signal handler to the test handler.
    signal(SIGABRT, abortSignalHandler);

    // set the global test flag (used by the signal handler).
    g_inTest = true;
}

void removeAbortHandler() {
    signal(SIGABRT, SIG_DFL);
    g_inTest = false;
}

#define BEGIN_ABORT_TEST {                                                    \
    volatile bool _abortOccurred = false;                                     \
    installAbortHandler();                                                    \
    _abortOccurred = setJump(g_jumpBuffer);                                   \
    if (!_abortOccurred) {


static const bool ABORT_OCCURRED    = true;
static const bool NO_ABORT_OCCURRED = false;

#define END_ABORT_TEST_AND_ASSERT(EXPECTED_ABORT)                             \
    }                                                                         \
    removeAbortHandler();                                                     \
    ASSERTV(EXPECTED_ABORT, _abortOccurred, EXPECTED_ABORT == _abortOccurred);\
  }


//=============================================================================
//                           CLASSES FOR TESTING
//-----------------------------------------------------------------------------

void noThrowFunction() BSLS_NOTHROW_SPEC
{
}

void exceptionSpecFunction() BSLS_EXCEPTION_SPEC((TestExceptionClass))
{
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Example 1: Using 'bsls_exceptionutil' to Implement A Memory Allocator
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to define an implementation of a standard-defined
// 'vector' template.  Unfortunately, the C++ standard requires that 'vector'
// provide an 'at' method that throws an 'out_of_range' exception
// if the supplied index is not in the valid range of elements in the vector.
// In this example we show using 'BSLS_THROW' so that such an implementation
// will compile in both exception enabled an non-exception enabled builds.
// Note that apart from memory allocation, and where required by the C++
// standard, types defined in the BDE libraries do not throw exceptions, and
// are typically "exception neutral" (see {'bsldoc_glossary'), meaning they
// behave reasonably in the presence of injected exceptions, but do not
// themselves throw any exceptions.
//
// First we open a namespace 'myStd' and define an 'out_of_range' exception
// that the 'at' method will throw (note that in practice, 'out_of_range'
// would inherit from 'logic_error')':
//..
    namespace myStd {

    class out_of_range  // ...
    {
       // ...
    };
//..
// Next, we declare the 'vector' template and its template parameters (note
// that the majority of the implementation is elided, for clarity):
//..
    template <class VALUE, class ALLOCATOR /* ... */>
    class vector {
        // DATA
        VALUE *d_begin_p;
        VALUE *d_end_p;
        // ...

      public:

        typedef typename ALLOCATOR::size_type size_type;

        //...
//..
// Then, we define the 'at' method, which is required to throw an
// 'out_of_range' exception.
//..
        const VALUE& at(size_type index) const
        {
            if (d_begin_p + index < d_end_p) {
                return d_begin_p[index];                              // RETURN
            }
//..
// Now, we use 'BSLS_THROW' the t
//..
            BSLS_THROW(out_of_range(/* ... */));
        }
//..
// Finally, we complete the (mostly elided) 'vector' implementation:
//..
        // ...

    };

    }  // close namespace myStd
//..

struct DummyAllocator {
    typedef int size_type;
};

///Example 2: Using 'bsls_exceptionutil' to Throw and Catch Exceptions
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates the macros defined in the
// 'bsls_exceptionutil' component to both throw and catch exceptions in a way
// that will allow the code to compile in non-exception enabled builds.
//
// First, we define a couple of example exception classes (note that we cannot
// use 'bsl::exception' in this example, as this component is defined below
//'bsl_exception.h'):
//..
    class my_ExClass1
    {
    };
//
    class my_ExClass2
    {
    };
//..
// Then, we define a function that never throws an exception, and use the
// 'BSLS_NOTHROW_SPEC' to ensure the no-throw exception specification will be
// present in exception enabled builds, and elided in non-exception enabled
// builds:
//..
    int noThrowFunc() BSLS_NOTHROW_SPEC
    {
        return -1;
    }
//..
// Next, we define a function that might throw 'my_ExClass1' or 'my_ExClass2',
// and we use the 'BSLS_EXCEPTION_SPEC' to ensure the exception specification
// will be present in exception enabled builds, and elided in non-exception
// builds:
//..
    int mightThrowFunc(int i) BSLS_EXCEPTION_SPEC((my_ExClass1, my_ExClass2))
    {
        switch (i) {
          case 0: break;
          case 1: BSLS_THROW(my_ExClass1());
          case 2: BSLS_THROW(my_ExClass2());
        }
        return i;
    }
//..
// Then, we start the definition of a 'testMain' function:
//..
    int testMain()
    {
//..
// Next, we use the 'BDE_BUILD_TARGET_EXC' exception build flag to determine,
// at compile time, whether to initialize 'ITERATIONS' to 3 (for exception
// enabled builds) or 1 (for non-exception enabled builds).  The different
// values of the 'ITERATOR' ensure the subsequent for-loop calls
// 'mightThrowFunc' in a way that generates exceptions for only exception
// enabled builds:
//..
    #ifdef BDE_BUILD_TARGET_EXC
        const int ITERATIONS = 3;
    #else
        const int ITERATIONS = 1;
    #endif
//
        for (int i = 0; i < ITERATIONS; ++i) {
//..
// Then, we use a pair of nested 'try' blocks constructed using
// 'BSLS_TRY', so that the code will compile whether or not exceptions are
// enabled (note that the curly brace placement is identical to normal
// 'try' and 'catch' constructs):
//..
            int caught = -1;
            BSLS_TRY {
//
                BSLS_TRY {
                    noThrowFunc();
                    mightThrowFunc(i);
//..
// Notice that this example is careful to call 'mightThrowFunc' in a way that
// it will not throw in non-exception builds.  Although the use 'BSLS_TRY',
// 'BSLS_THROW', and 'BSLS_CATCH' ensures the code *compiles* in both
// exception, and non-exception enabled builds, attempting to 'BSLS_THROW' an
// exception in a non- exception enabled build will invoke the assert handler
// and will typically abort the task.
//..
                    caught = 0; // Got here if no throw
                }
//..
// Next, we use 'BSLS_CATCH' to defined blocks for handling exceptions that may
// have been thrown from the preceding 'BSLS_TRY':
//..
                BSLS_CATCH(my_ExClass1) {
                    caught = 1;
                }
                BSLS_CATCH(...) {
//..
// Here, within the catch-all handler, we use the 'BSLS_RETHROW' macro to
// re-throw the exception to the outer 'try' block:
//..
                    BSLS_RETHROW;
                } // end inner try-catch
            }
            BSLS_CATCH(my_ExClass2) {
                caught = 2;
            }
            BSLS_CATCH(...) {
                ASSERT("Should not get here" && 0);
            } // end outer try-catch
//
            if (0 != caught) {
                if (verbose)
                printf("Caught exception my_ExClass: %d\n", caught);
            }
            else {
                if (verbose)
                printf("Caught no exceptions: %d\n", caught);
            }
            ASSERT(i == caught);
//
        } // end for (i)
//
        return 0;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        testMain();
        myStd::vector<int, DummyAllocator> obj;
        (void) obj;

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // BSLS_EXCEPTION_SPEC and BSLS_NOTHROW_SPEC
        //   Ensure BSLS_EXCEPTION_SPEC and BSLS_NOTHROW_SPEC build
        //
        // Concerns:
        //   1 'BSLS_NOTHROW_SPEC' and 'BSLS_EXCEPTION_SPEC' compile in
        //      multiple build modes.
        //
        // Plan:
        //   1 Define functions using the 'BSLS_NOTHROW_SPEC' and
        //     'BSLS_EXCEPTION_SEC' macros and verify the compile in both
        //     exception and non-exception enabled builds.
        //
        // Testing:
        //   BSLS_EXCEPTION_SPEC
        //   BSLS_NOTHROW_SPEC
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BSLS_EXCEPTION_SPEC & BSLS_NOTHROW_SPEC"
                            "\n==============================================="
                            "\n");

        noThrowFunction();
        exceptionSpecFunction();
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // BSLS_RETHROW
        //   Ensure basic properties of BSLS_RETHROW
        //
        // Concerns:
        //  1 In exception enabled builds BSLS_RETHROW will define a 'throw'
        //    statement.
        //
        //  2 In non-exception enabled builds BSLS_RETHROW will call the
        //    currently installed assertion handler.
        //
        // Plan:
        //   1 In exception build modes, create a 'try' block and 'throw'
        //     an exception and verify it is caught by a corresponding
        //     'catch' block, the call 'BSLS_RETHROW', and verify the same
        //     exception is caught in a corresponding 'catch' block.  (C-1).
        //
        //   2 In exception enabled builds, install the 'assertHandler'
        //     defined in this test driver (that records its arguments, and
        //     calls 'abort'), then call 'BSLS_RETHROW' using the
        //     'BEGIN_ABORT_TEST' and 'END_ABORT_TEST_AND_ASSERT'.  Verify an
        //     abort occurred, and that the assert-handler was invoked with the
        //     expected values. (C-2).
        //
        // Testing:
        //   BSLS_RETHROW
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BSLS_RETHROW"
                            "\n====================\n");

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) {
            printf("Verify BSLS_RETHROW rethrows the exception in exc mode\n");
        }
        {
            bool executedTest    = false;
            bool caughtException = false;
            try {
                try {
                    throw TestExceptionClass();
                    ASSERT(false);
                }
                catch (const TestExceptionClass&) {
                    executedTest = true;
                    BSLS_RETHROW;
                    ASSERT(false);
                }
            }
            catch(const TestExceptionClass) {
                caughtException = true;
            }
            ASSERT(executedTest);
            ASSERT(caughtException);
        }
#else // !BDE_BUILD_TARGET_EXC
        if (verbose) {
            printf("Verify BSLS_RETHROW calls assert in non-exc\n");
        }
        {
            bsls::Assert::setFailureHandler(assertHandler);
            BEGIN_ABORT_TEST {
                BSLS_RETHROW;
            } END_ABORT_TEST_AND_ASSERT(ABORT_OCCURRED);
            ASSERT(0 != g_assertMessage);
            ASSERT(0 != g_assertFile);
            ASSERT(0 <  g_assertLine);

            ASSERT(0 != strstr(g_assertMessage, "Tried to re-throw "));
            ASSERT(0 != strstr(g_assertFile, "bsls_exceptionutil.t.cpp"));
            if (veryVerbose) {
                P_(g_assertMessage);
                P_(g_assertFile);
                P(g_assertLine);
            }
        }
#endif
     } break;
      case 5: {
        // --------------------------------------------------------------------
        // BSLS_CATCH
        //   Ensure basic properties of BSLS_CATCH
        //
        // Concerns:
        //  1 In exception enabled builds BSLS_CATCH will define a 'catch'
        //    statement.
        //
        //  2 In non-exception enabled builds BSLS_CATCH will not generate a
        //    catch statement, and the subsequent block of code will not
        //    execute.
        //
        // Plan:
        //   1 In all build modes, create a 'catch' block and verify that it
        //     is not entered.  (C-1, C-2).
        //
        //   2 In exception enabled builds, create a 'try' block and 'throw'
        //     and exception, verify it is caught in the corresponding
        //     'BSLS_CATCH' block.  (C-1)
        //
        // Testing:
        //   BSLS_CATCH
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BSLS_CATCH"
                            "\n==================\n");

        if (verbose) {
            printf("Verify BSLS_CATCH does not execute without a 'throw'\n");
        }
        {
            BSLS_TRY {
            }
            BSLS_CATCH(...) {
                ASSERT(false);
            }
        }
#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) {
            printf("Verify BSLS_THROW throws an exception in exc builds\n");
        }
        {
            bool executedTest    = false;
            bool caughtException = false;
            try {
                executedTest = true;
                throw TestExceptionClass();
                ASSERT(false);
            }
            BSLS_CATCH (const TestExceptionClass&) {
                caughtException = true;
            }
            BSLS_CATCH (...) {
                ASSERT(false);
            }
            ASSERT(executedTest);
            ASSERT(caughtException);
        }
#endif
     } break;
      case 4: {
        // --------------------------------------------------------------------
        // BSLS_THROW
        //   Ensure basic properties of BSLS_THROW
        //
        // Concerns:
        //  1 In exception enabled builds BSLS_THROW will define a 'throw'
        //    statement.
        //
        //  2 In non-exception enabled builds BSLS_THROW will call the
        //    currently installed assertion handler.
        //
        // Plan:
        //   1 In exception build modes, create a 'try' block and 'BSLS_THROW'
        //     an exception and verify it is caught by a corresponding
        //     'catch' block.  (C-1).
        //
        //   2 In exception enabled builds, install the 'assertHandler'
        //     defined in this test driver (that records its arguments, and
        //     calls 'abort'), then call 'BSLS_THROW' using the
        //     'BEGIN_ABORT_TEST' and 'END_ABORT_TEST_AND_ASSERT'.  Verify an
        //     abort occurred, and that the assert-handler was invoked with the
        //     expected values. (C-2).
        //
        // Testing:
        //   BSLS_THROW
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BSLS_THROW"
                            "\n==================\n");

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) {
            printf("Verify BSLS_THROW throws an exception in exc builds\n");
        }
        {
            bool executedTest    = false;
            bool caughtException = false;
            try {
                executedTest = true;
                BSLS_THROW(TestExceptionClass());
                ASSERT(false);
            }
            catch (const TestExceptionClass&) {
                caughtException = true;
            }
            catch (...) {
                ASSERT(false);
            }
            ASSERT(executedTest);
            ASSERT(caughtException);
        }
#else // !BDE_BUILD_TARGET_EXC
        if (verbose) {
            printf("Verify BSLS_THROW calls the assert-handler in non-exc\n");
        }
        {
            bsls::Assert::setFailureHandler(assertHandler);
            BEGIN_ABORT_TEST {
                BSLS_THROW(TestExceptionClass());
            } END_ABORT_TEST_AND_ASSERT(ABORT_OCCURRED);
            ASSERT(0 != g_assertMessage);
            ASSERT(0 != g_assertFile);
            ASSERT(0 <  g_assertLine);

            ASSERT(0 != strstr(g_assertMessage, "Tried to throw "));
            ASSERT(0 != strstr(g_assertFile, "bsls_exceptionutil.t.cpp"));
            if (veryVerbose) {
                P_(g_assertMessage);
                P_(g_assertFile);
                P(g_assertLine);
            }
        }

#endif
     } break;
      case 3: {
        // --------------------------------------------------------------------
        // BSLS_TRY
        //   Ensure basic properties of BSLS_TRY
        //
        // Concerns:
        //  1 In exception enabled builds, BSLS_TRY will define a 'try'
        //    statement.
        //
        //  2 In non-exception enabled builds BSLS_TRY will not generate a
        //    'try' statement, and the subsequent block of code will
        //     execute unconditionally.
        //
        // Plan:
        //   1 In all build modes, create a BSLS_TRY block and verify the code
        //     inside is executed. (C-1, C-2)
        //
        //   2 In exception enabled builds, create a BSLS_TRY block in which
        //     a 'throw' is used to throw an exception, verify code after the
        //     'throw' is not executed, and that it is caught in a
        //     corresponding 'catch' block.
        //
        // Testing:
        //   BSLS_TRY
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BSLS_TRY"
                            "\n================\n");


        if (verbose) {
            printf("Verify BSLS_TRY blocks always execute unconditionally\n");
        }
        {
            bool executedTest = false;
            BSLS_TRY {
                executedTest = true;
            }
#ifdef BDE_BUILD_TARGET_EXC
            catch (...) { ASSERT(false); }
#endif
            ASSERT(executedTest);
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) {
            printf("Verify BSLS_TRY is replaced by 'try' in exc builds\n");
        }
        {
            bool executedTest    = false;
            bool caughtException = false;
            BSLS_TRY {
                executedTest = true;
                throw TestExceptionClass();
                ASSERT(false);
            }
            catch (const TestExceptionClass&) {
                caughtException = true;
            }
            catch (...) {
                ASSERT(false);
            }
            ASSERT(executedTest);
            ASSERT(caughtException);
        }
#endif
     } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING: ABORT HANDLING MACROS
        //
        // Concerns:
        //   1 That 'BEGIN_ABORT_TEST' and 'END_ABORT_TEST_AND_ASSERT'
        //     will correctly indicate whether a 'abort' occurred in the
        //     lexical scope (without terminating the task).
        //
        //   2 Test that 'BEGIN_ABORT_TEST' and 'END_ABORT_TEST_AND_ASSERT'
        //     when called multiple times to recover from an abort do not have
        //     surprising side effects.
        //
        //   3 That 'BEGIN_ABORT_TEST' and 'END_ABORT_TEST_AND_ASSERT'
        //     form a valid lexical scope.
        //
        // Plan:
        //   1 Call 'BEGIN_ABORT_TEST' and 'END_ABORT_TEST_AND_ASSERT' without
        //     an 'abort' present, and verify the indicate no-abort occurred.
        //     (C-1)
        //
        //   2 Call 'BEGIN_ABORT_TEST' and 'END_ABORT_TEST_AND_ASSERT' with
        //     an 'abort', and verify they indicate an abort occurred.
        //     (C-1)
        //
        //   3 Call 'BEGIN_ABORT_TEST' and 'END_ABORT_TEST_AND_ASSERT' again
        //     with an 'abort', and verify they indicate an abort occurred
        //     (C-2)
        //
        //   4 Define local variables and verify they're scoped correctly with
        //     'BEGIN_ABORT_TEST' and 'END_ABORT_TEST_AND_ASSERT' blocks.
        //     (C-3)
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ABORT HANDLING MACROS"
                            "\n============================\n");

        if (verbose) {
            printf("Verify non-aborting test\n");
        }
        {
            bool executedTest = false;
            BEGIN_ABORT_TEST {
                executedTest = true;
            }
            END_ABORT_TEST_AND_ASSERT(NO_ABORT_OCCURRED);
            ASSERT(executedTest);
        }

        if (verbose) {
            printf("Verify aborting test\n");
        }
        {
            volatile bool executedTest = false;
            BEGIN_ABORT_TEST {
                executedTest = true;
                abort();
                ASSERT(false);
            }
            END_ABORT_TEST_AND_ASSERT(ABORT_OCCURRED);
            ASSERT(executedTest);
        }

        // Note that the use of 'volatile' prevents (for the time being)
        // reordering the assignment to 'executedTest' in ways that cause the
        // test to fail on opt-AIX builds.

        if (verbose) {
            printf("Verify second aborting test\n");
        }
        {
            volatile bool executedTest = false;
            BEGIN_ABORT_TEST {
                executedTest = true;
                abort();
                ASSERT(false);
            }
            END_ABORT_TEST_AND_ASSERT(ABORT_OCCURRED);
            ASSERT(executedTest);
        }

        if (verbose) {
            printf("Verify lexical scope\n");
        }
        {
            int duplicate = 0;
            BEGIN_ABORT_TEST
                int duplicate = 1;
                ASSERT(1 == duplicate);
            END_ABORT_TEST_AND_ASSERT(NO_ABORT_OCCURRED);
            ASSERT(0 == duplicate);
        }
     } break;
     case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");


        if (verbose) {
            printf("Verify basic compilation of try/catch and throw blocks\n");
        }
        {
            int status = 0;

            BSLS_TRY {
                status = 1;
                ASSERT(true);
                if (status == 2) {
                    ASSERT(false);
                    BSLS_THROW(1);
                }
            }
            BSLS_CATCH(...) {
                ASSERT(false);
                status = 2;
                BSLS_RETHROW;
            }
            ASSERT(1 == status);
        }

        if (verbose) {
            printf("Verify basic try/catch in exception enabled builds\n");
        }
#ifdef BDE_BUILD_TARGET_EXC
        {
            int status = 0;

            BSLS_TRY {
                BSLS_TRY {
                    BSLS_THROW(1);
                    ASSERT(false);
                }
                BSLS_CATCH(int value) {
                    ASSERT(1 == value);
                    status = 1;
                    BSLS_RETHROW;
                    ASSERT(false);
                }
            }
            BSLS_CATCH(int value) {
                ASSERT(1 == value);
                ASSERT(1 == status);
                status = 2;
            }
            ASSERT(2 == status);
        }
#endif // BDE_BUILD_TARGET_EXC
      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n",
                     testStatus);
    }

    return testStatus;
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
