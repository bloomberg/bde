// bsls_exceptionutil.t.cpp                  -*-C++-*-

#include <bsls_exceptionutil.h>
#include <bsls_platform.h>

#include <cstdio>
#include <cstdlib>

#if BSLS_PLATFORM__OS_UNIX
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#else
typedef int pid_t;
#endif

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//

//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        std::printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) std::printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ std::printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

enum Action { ACTION_NONE, ACTION_THROW, ACTION_ABORT, ACTION_ERROR };

class TestExceptionClass { };

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----

// Define a few exception classes:
//..
    class my_ExClass1
    {
    };

    class my_ExClass2
    {
    };
//..
// Define a function that never throws an exception:
//..
    int noThrowFunc() BSLS_NOTHROW_SPEC
    {
        return -1;
    }
//..
// Define a function that might throw 'my_ExClass1' or 'my_ExClass2':
//..
    int doThrowSome(int i) BSLS_EXCEPTION_SPEC((my_ExClass1, my_ExClass2))
    {
        switch (i) {
          case 0: break;
          case 1: BSLS_THROW(my_ExClass1());
          case 2: BSLS_THROW(my_ExClass2());
        }
        return i;
    }
//..
// In the main program, loop three times if exceptions are enabled, but only
// once if exceptions are disabled.  The only conditional compilation is for
// the loop counter:
//..
    int usageExample()
    {
    #ifdef BDE_BUILD_TARGET_EXC
        const int ITERATIONS = 3;
    #else
        const int ITERATIONS = 1;
    #endif

        for (int i = 0; i < ITERATIONS; ++i) {
//..
// The loop contains a pair of nested 'try' blocks constructed using the
// macros so that it will compile and run whether or not exceptions are
// enabled.  Note that the curly brace placement is identical to normal 'try'
// and 'catch' constructs.  The outer 'try' block catches 'my_ExClass2':
//..
            int caught = -1;
            BSLS_TRY {
//..
// The inner 'try' block catches 'my_ExClass1' and also has a "catch-all"
// handler:
//..

                BSLS_TRY {
                    noThrowFunc();
                    doThrowSome(i);

                    caught = 0; // Got here if no throw
                }
                BSLS_CATCH(my_ExClass1) {
                    caught = 1;
                }
                BSLS_CATCH(...) {
//..
// Within the catch-all handler, use the 'BSLS_RETHROW' macro to re-throw the
// exception to the outer 'try' block:
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

            if (verbose) {
                if (0 != caught) {
                    std::printf("Caught exception my_ExClass%d\n", caught);
                }
                else {
                    std::printf("Caught no exceptions\n");
                }
            }
            ASSERT(i == caught);

        } // end for (i)

        return 0;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    std::setbuf(stdout, 0);    // Use unbuffered output

    std::printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) std::printf("\nUSAGE EXAMPLE"
                                 "\n=============\n");

        usageExample();

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // TEST THROW
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) std::printf("\nTESTING BSLS_THROW"
                                 "\n==================\n");

        Action throwAction = ACTION_NONE;

#ifdef BDE_BUILD_TARGET_EXC

        // If exceptions are enabled, expect 'BSLS_THROW' to do a real 'throw'.
        Action expectedAction = ACTION_THROW;
        BSLS_TRY {
            if (verbose) std::printf("About to throw...\n");
            BSLS_THROW(TestExceptionClass());
            throwAction = ACTION_NONE; // Should not get here.
        }
        BSLS_CATCH (TestExceptionClass) {
            if (verbose) std::printf("Caught TestExceptionClass\n");
            throwAction = ACTION_THROW;
        }
        BSLS_CATCH (...) {
            if (verbose) std::printf("Caught unexpected exception\n");
            throwAction = ACTION_ERROR;
        }

        ASSERT(expectedAction == throwAction);

#elif defined BSLS_PLATFORM__OS_UNIX
        // If exceptions are disabled on Unix, for a process and expect
        // 'BSLS_THROW' to abort.
        Action expectedAction = ACTION_ABORT;
        if (verbose) std::printf("Forking child process\n");
        pid_t childPid = fork();
        ASSERT((pid_t) -1 != childPid);

        if (0 == childPid) {
            // In the child process, we should never see the expected action.
            // However, if we do, it means that 'BSLS_THROW' did nothing.
            expectedAction = ACTION_NONE;
        }
        else if ((pid_t) -1 == childPid) {
            break; // Failed to fork child.
        }

        if (0 == childPid) {
            BSLS_TRY {
                if (verbose) std::printf("About to throw...\n");
                BSLS_THROW(TestExceptionClass());
                throwAction = ACTION_NONE; // Should not get here.
            }
            BSLS_CATCH (TestExceptionClass) {
                if (verbose) std::printf("Caught TestExceptionClass\n");
                throwAction = ACTION_THROW;
            }
            BSLS_CATCH (...) {
                if (verbose) std::printf("Caught unexpected exception\n");
                throwAction = ACTION_ERROR;
            }
        }
        else {
            // This branch is only executed for non-exception unix builds.
            // Parent waits for child to abort or return.
            int childStat = 0;
            int waitRet = wait(&childStat);

            if (verbose) std::printf("Child returned with status %d\n",
                                     childStat);

            ASSERT(childPid == waitRet);

            // If child exited with a signal, assume 'BSLS_THROW' aborted.
            // Otherwise, if child exited with a zero status, assume that
            // 'BSLS_THROW' did nothing.  Otherwise, assume some other error.
            throwAction = (WIFSIGNALED(childStat) ? ACTION_ABORT :
                           (WIFEXITED(childStat) &&
                            0 == WEXITSTATUS(childStat)) ?
                           ACTION_NONE : ACTION_ERROR);
        }

        ASSERT(expectedAction == throwAction);

#endif // UNIX

      } break;

      default: {
        std::fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        std::fprintf(stderr, "Error, non-zero test status = %d.\n",
                     testStatus);
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
