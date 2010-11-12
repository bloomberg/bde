// bdes_assert.t.cpp                                                  -*-C++-*-
#include <bdes_assert.h>

#include <bsls_platform.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_stdexcept.h>

#ifdef BSLS_PLATFORM__OS_UNIX
#include <bsl_c_signal.h>
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

#if BSL_LEGACY == 0

#define BDE_ASSERT_H(X)    BSLS_ASSERT(X)
#define BDE_ASSERT_CPP(X)  BSLS_ASSERT_OPT(X)

typedef bsls_Assert                    bdes_Assert;
typedef bsls_AssertFailureHandlerGuard bdes_AssertFailureHandlerGuard;

#else

#ifdef BDE_ASSERT_H
#undef BDE_ASSERT_H
#endif
#ifdef BDE_ASSERT_CPP
#undef BDE_ASSERT_CPP
#endif
#define BDE_ASSERT_H(X)    BSLS_ASSERT(X)
#define BDE_ASSERT_CPP(X)  BSLS_ASSERT_OPT(X)

#endif

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

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
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdes_Assert                      Util;
typedef bdes_AssertFailureHandlerGuard   Guard;

int globalVerbose     = 0;
int globalVeryVerbose = 0;

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

static
void failSafe(const char *text, const char *file, int line)
    // Print an appropriate error message to 'stdout' containing the specified
    // 'text', 'file', and 'line' information.
{
    if (globalVerbose) {
        bsl::cout << "Oh Darn! " << "( " << text << " ) failed in "
                  << file << " at line " << line << "!" << endl;
    }
}

static
void badFunction(const char *text, const char *file, int line)
    // Unconditionally 'ASSERT(0)'.
{
    ASSERT(0);
}

static
void testCPPFunction(int x, char id)
    // 'ASSERT' that the specified 'id' equals 'C', and 'BDE_ASSERT_CPP' that
    // 0 is less than the specified 'x'.  The behavior is contingent on build
    // flags if '0 >= x'.
{
    ASSERT('C' == id);
    if (globalVeryVerbose) bsl::cout << "In 'testCPPFunction'." << bsl::endl;

    BDE_ASSERT_CPP(0 < x);

    if (Util::failThrow == Util::failureHandler()) {
        if (globalVerbose && !(0 < x)) {
            bsl::cout << "Oh Darn! " << "!(0 < x) failed!" << bsl::endl;
        }
    }
}

static
void testHFunction(int x, char id)
    // 'ASSERT' that the specified 'id' equals 'C', and 'BDE_ASSERT_H' that 0
    // is less than the specified 'x'.  The behavior is contingent on build
{
    ASSERT('H' == id);
    if (globalVeryVerbose) bsl::cout << "In 'testHFunction'." << bsl::endl;

    BDE_ASSERT_H(0 < x);

    if (Util::failThrow == Util::failureHandler()) {
        if (globalVerbose && !(0 < x)) {
            bsl::cout << "Oh Darn! " << "!(0 < x) failed!" << bsl::endl;
        }
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    globalVerbose     = verbose;
    globalVeryVerbose = veryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'lockAssertAdministration'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   void lockAssertAdministration();
        //   class bdes_AssertFailureHandlerGuard
        //   CONCERN: that locking does not stop the handlerGuard from working
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                           << "TESTING 'lockAssertAdministration'" << endl
                           << "==================================" << endl;

        if (verbose) cout << "Testing 'lockAssertAdministration'" << endl;

        Util::setFailureHandler(::failSafe);
        ASSERT(::failSafe == Util::failureHandler());

        Util::lockAssertAdministration();
        ASSERT(::failSafe == Util::failureHandler());

        Util::setFailureHandler(::badFunction);
        ASSERT(::failSafe == Util::failureHandler());

        {
            Guard guard(::badFunction);
            ASSERT(::badFunction == Util::failureHandler());
        }

        ASSERT(::failSafe == Util::failureHandler());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BREATHING II -- 'BDE_ASSERT_H' MACRO
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BDE_ASSERT_H(X)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST -- BDE_ASSERT_H" << endl
                          << "==============================" << endl;

        Util::setFailureHandler(Util::failThrow);
        ASSERT(Util::failThrow == Util::failureHandler());

#ifdef BDE_BUILD_TARGET_EXC
        enum { NO_FLAG = 0, YES_FLAG = 1, X_FLAG = 2 };
        int assertionFailedFlag;

        assertionFailedFlag = X_FLAG;
        try {
            testHFunction( 1, 'H');
            assertionFailedFlag = NO_FLAG;
        }
        catch (bsl::logic_error) {
            assertionFailedFlag = YES_FLAG;
        }
        ASSERT(NO_FLAG == assertionFailedFlag);

#ifdef BDE_BUILD_TARGET_DBG
        assertionFailedFlag = X_FLAG;
        try {
            testHFunction( 0, 'H');
            assertionFailedFlag = NO_FLAG;
        }
        catch (bsl::logic_error) {
            assertionFailedFlag = YES_FLAG;
        }
        ASSERT(YES_FLAG == assertionFailedFlag);

        assertionFailedFlag = X_FLAG;
        try {
            testHFunction(-1, 'H');
            assertionFailedFlag = NO_FLAG;
        }
        catch (bsl::logic_error) {
            assertionFailedFlag = YES_FLAG;
        }
        ASSERT(YES_FLAG == assertionFailedFlag);
#endif
#endif

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING I -- 'BDE_ASSERT_CPP' MACRO
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BDE_ASSERT_CPP(X)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST -- BDE_ASSERT_CPP" << endl
                          << "================================" << endl;

        Util::setFailureHandler(Util::failThrow);
        ASSERT(Util::failThrow == Util::failureHandler());

#ifdef BDE_BUILD_TARGET_EXC
        enum { NO_FLAG = 0, YES_FLAG = 1, X_FLAG = 2 };
        int assertionFailedFlag;

        assertionFailedFlag = X_FLAG;
        try {
            testCPPFunction( 1, 'C');
            assertionFailedFlag = NO_FLAG;
        }
        catch (bsl::logic_error) {
            assertionFailedFlag = YES_FLAG;
        }
        ASSERT(NO_FLAG == assertionFailedFlag);

        assertionFailedFlag = X_FLAG;
        try {
            testCPPFunction( 0, 'C');
            assertionFailedFlag = NO_FLAG;
        }
        catch (bsl::logic_error) {
            assertionFailedFlag = YES_FLAG;
        }
        ASSERT(YES_FLAG == assertionFailedFlag);

        assertionFailedFlag = X_FLAG;
        try {
            testCPPFunction(-1, 'C');
            assertionFailedFlag = NO_FLAG;
        }
        catch (bsl::logic_error) {
            assertionFailedFlag = YES_FLAG;
        }
        ASSERT(YES_FLAG == assertionFailedFlag);
#endif

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // abort() with SIGABRT block
        //
        // Concerns:
        // 1. abort() terminates the program
        //
        // Plan: call BDE_ASSERT_CPP(0) after blocking the signal
        //
        // Testing:
        //   bsl::abort
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                           << "Testing abort" << endl
                           << "=============" << endl;

#ifdef BSLS_PLATFORM__OS_UNIX
        sigset_t newset;
        sigaddset(&newset, SIGABRT);

        #if defined(BDE_BUILD_TARGET_MT)
            pthread_sigmask(SIG_BLOCK, &newset, 0);
        #else
            sigprocmask(SIG_BLOCK, &newset, 0);
        #endif
#endif
        BDE_ASSERT_CPP(0);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
