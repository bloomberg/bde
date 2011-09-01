// bdes_assert.t.cpp                                                  -*-C++-*-
#include <bdes_assert.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>
#include <bsls_assert.h>
#include <bsls_asserttestexception.h>

#include <bsl_cstdlib.h>  // 'strcmp'
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The DEPRECATED component is now implemented in 'bsls_assert'.  The test
// strategy here is to confirm (or not) that the elements of this component
// have been correctly mapped to the corresponding elements in the successor
// component.  Forwarding is implemented using 'typedef' for classes, and by
// '#define' for macros.
//
// ----------------------------------------------------------------------------
// [ 2] bdes_Assert
// [ 2] bdes_AssertFailureHandlerGuard
// [ 1] BDE_ASSERT_CPP(X)
// [ 1] BDE_ASSERT_H(X)

// ============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
// ----------------------------------------------------------------------------
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

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // CORRECTNESS OF TYPEDEFS
        //
        // Concerns:
        //: 1 Has each type defined in this component been 'typedef'ed to
        //:   the expected successor type?
        //
        // Plan:
        //: 1 Do compile-time comparison of the types using 'BSLMF_ASSERT' and
        //:   'bslmf_IsSame'.
        //
        // Testing:
        //   bdes_Assert
        //   bdes_AssertFailureHandlerGuard
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CORRECTNESS OF TYPEDEFS" << endl
                          << "=======================" << endl;

        if (veryVerbose) cout << "Correctness of 'bdes_Assert'" << endl;

        BSLMF_ASSERT(1 == (bslmf_IsSame<bdes_Assert, bsls_Assert>::VALUE));

        if (veryVerbose) cout
                  << "Correctness of 'bdes_AssertFailureHandlerGuard'" << endl;

        BSLMF_ASSERT(1 ==
                        (bslmf_IsSame<bdes_AssertFailureHandlerGuard,
                                      bsls_AssertFailureHandlerGuard>::VALUE));

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // REDEFINITION OF MACROS
        //
        // Concerns:
        //: 1 The 'BDE_ASSERT_H' macro is disabled (i.e., expands to nothing)
        //:   unless the 'BDE_BUILD_TARGET_SAFE' compile-time flag is defined.
        //:
        //: 2 The 'BDE_ASSERT_CPP' macros is disabled (i.e., expands to
        //:   nothing) unless the 'BDE_BUILD_TARGET_SAFE' or the
        //:   'BDE_BUILD_TARGET_DBG' compile-time flags are defined.
        //:
        //: 3 When enabled, the 'BDE_ASSERT_H' macro has the same behavior as
        //:   the 'BSLS_ASSERT_SAFE' macro.
        //:
        //: 4 When enabled, the 'BDE_ASSERT_CPP' macro has the same behavior as
        //:   the 'BSLS_ASSERT' macro.
        //
        // Plan:
        //: 1 In build-modes where the macros of interest should be disabled
        //:   create expressions that will fail to compile or fail to run
        //:   unless those macros expand to nothing.
        //:
        //: 2 In a build-mode where the 'BDE_ASSERT_H' macro is enabled:
        //:   1 Define a conveniently observable behavior for the successor
        //:     macro, 'BSLS_ASSERT_SAFE'.
        //:   2 See if that behavior occurs when 'BDE_ASSERT_H' fails, and
        //:     doesn't occur when 'BDE_ASSERT_H' passes.
        //
        //: 3 In a build-mode where the 'BDE_ASSERT_CPP' macro is enabled:
        //:   1 Define a conveniently observable behavior for the successor
        //:     macro, 'BSLS_ASSERT'.
        //:   2 See if that behavior occurs when 'BDE_ASSERT_CPP' fails, and
        //:     doesn't occur when 'BDE_ASSERT_CPP' passes.
        //
        // Testing:
        //   BDE_ASSERT_CPP(X)
        //   BDE_ASSERT_H(X)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "REDEFINITION OF MACROS" << endl
                          << "----------------------" << endl;

#if BSL_LEGACY == 0
#if !defined(BDE_BUILD_TARGET_SAFE)

        if (veryVerbose) cout << "Confirm 'BDE_ASSERT_H' disabled" << endl;

        ASSERT(strcmp("hello", "hello" BDE_ASSERT_H(X)));

#if !defined(BDE_BUILD_TARGET_DBG)

        if (veryVerbose) cout << "Confirm 'BDE_ASSERT_CPP' disabled" << endl;

        ASSERT(strcmp("hello", "hello" BDE_ASSERT_CPP(X)));

#endif // !defined(BDE_BUILD_TARGET_DBG)
#endif // !defined(BDE_BUILD_TARGET_SAFE)
#endif // BSL_LEGACY == 0

#if defined(BDE_BUILD_TARGET_EXC)
        bsls_Assert::setFailureHandler(bsls_Assert::failThrow);

        if (veryVerbose) cout << "Confirm 'BDE_ASSERT_H' behavior" << endl;
        {
            bool handlerCalledViaHWithTrue;
            try {
                BDE_ASSERT_H(true);
                handlerCalledViaHWithTrue = false;
            } catch (bsls_AssertTestException) {
                handlerCalledViaHWithTrue = true;
            }
            if (veryVeryVerbose) { T_ P(handlerCalledViaHWithTrue) }

            bool handlerCalledViaHWithFalse;
            try {
                BDE_ASSERT_H(false);
                handlerCalledViaHWithFalse = false;
            } catch (bsls_AssertTestException) {
                handlerCalledViaHWithFalse = true;
            }
            if (veryVeryVerbose) { T_ P(handlerCalledViaHWithFalse) }

            bool handlerCalledViaSafeWithTrue;
            try {
                BSLS_ASSERT_SAFE(true);
                handlerCalledViaSafeWithTrue = false;

            } catch (bsls_AssertTestException) {
                handlerCalledViaSafeWithTrue = true;
            }
            if (veryVeryVerbose) { T_ P(handlerCalledViaSafeWithTrue) }

            bool handlerCalledViaSafeWithFalse;
            try {
                BSLS_ASSERT_SAFE(false);
                handlerCalledViaSafeWithFalse = false;
            } catch (bsls_AssertTestException) {
                handlerCalledViaSafeWithFalse = true;
            }
            if (veryVeryVerbose) { T_ P(handlerCalledViaSafeWithFalse) }

            LOOP2_ASSERT(handlerCalledViaHWithTrue,
                         handlerCalledViaSafeWithTrue,
                         handlerCalledViaHWithTrue
                      == handlerCalledViaSafeWithTrue);

            LOOP2_ASSERT(handlerCalledViaHWithFalse,
                         handlerCalledViaSafeWithFalse,
                         handlerCalledViaHWithFalse
                      == handlerCalledViaSafeWithFalse);
        }

        if (veryVerbose) cout << "Confirm 'BDE_ASSERT_CPP' behavior" << endl;
        {
            bool handlerCalledViaCppWithTrue;
            try {
                BDE_ASSERT_H(true);
                handlerCalledViaCppWithTrue = false;
            } catch (bsls_AssertTestException) {
                handlerCalledViaCppWithTrue = true;
            }
            if (veryVeryVerbose) { T_ P(handlerCalledViaCppWithTrue) }

            bool handlerCalledViaCppWithFalse;
            try {
                BDE_ASSERT_H(false);
                handlerCalledViaCppWithFalse = false;
            } catch (bsls_AssertTestException) {
                handlerCalledViaCppWithFalse = true;
            }
            if (veryVeryVerbose) { T_ P(handlerCalledViaCppWithFalse) }

            bool handlerCalledVia_WithTrue;
            try {
                BSLS_ASSERT_SAFE(true);
                handlerCalledVia_WithTrue = false;
            } catch (bsls_AssertTestException) {
                handlerCalledVia_WithTrue = true;
            }
            if (veryVeryVerbose) { T_ P(handlerCalledVia_WithTrue) }

            bool handlerCalledVia_WithFalse;
            try {
                BSLS_ASSERT_SAFE(false);
                handlerCalledVia_WithFalse = false;
            } catch (bsls_AssertTestException) {
                handlerCalledVia_WithFalse = true;
            }
            if (veryVeryVerbose) { T_ P(handlerCalledVia_WithFalse) }

            LOOP2_ASSERT(handlerCalledViaCppWithTrue,
                         handlerCalledVia_WithTrue,
                         handlerCalledViaCppWithTrue
                      == handlerCalledVia_WithTrue);

            LOOP2_ASSERT(handlerCalledViaCppWithFalse,
                         handlerCalledVia_WithFalse,
                         handlerCalledViaCppWithFalse
                      == handlerCalledVia_WithFalse);
        }
#endif // if defined(BDE_BUILD_TARGET_EXC)

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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
