// btemt_socks5udpchannel.t.cpp     -*-C++-*-
#include <btemt_socks5udpchannel.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h> // for testing only
#include <bslma_default.h>               // for testing only
#include <bsls_assert.h>

#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;
using namespace BloombergLP::btemt;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// ...
//
//-----------------------------------------------------------------------------
// [1] constructors
// [2] assign and reset
// [3] operators for equality and inequality
// [4] operators less/greater than
// [5] comparators
// [6] output (printing)
// [7] conversion
// [8] ....
//-----------------------------------------------------------------------------
// [0] BREATHING TEST
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }


//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
          << J << "\t" << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                          << "\t" << #K << ": " << K << "\t" << #L << ": " << L \
                          << "\n"; aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                    SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl;           // Print identifier and value
#define Q(X) cout << "<! " #X " |>" << endl;            // Quote identifier literally
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) wihtout '\n'
#define L_ __LINE__
#define T_ cout << "\t" << flush;                       // Print tab w/o newline

//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void assertHandler(const char *text, const char *file, int line)
{
    LOOP3_ASSERT(text, file, line, false);
}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bslma_TestAllocator ta(veryVeryVerbose);
    bslma_Default::setDefaultAllocatorRaw(&ta);
    bsls_Assert::setFailureHandler(assertHandler);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // TESTING '<<TODO>>'
        //
        // Concerns:
        //
        //
        // Plan:
        //
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING '<<TODO>>'" << endl
                                  << "==================" << endl;

        // . . .

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PRIMARY ACCESSORS
        //
        // Concerns:
        //
        //
        // Plan:
        //
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PRIMARY ACCESSORS" << endl
                                  << "=================" << endl;

        // . . .

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING APPARATUS
        //
        // Concerns:
        //
        //
        // Plan:
        //
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING APPARATUS" << endl
                                  << "=================" << endl;

        // . . .

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //
        // Concerns:
        //
        //
        // Plan:
        //
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PRIMARY MANIPULATORS" << endl
                                  << "====================" << endl;

        // . . .

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   This test exercises basic functionality but tests nothing.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        // . . .

      } break;
      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        return -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
