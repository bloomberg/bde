// bdldfp_bufferbuf.t.cpp                                             -*-C++-*-
#include <bdldfp_bufferbuf.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::flush;
using bsl::endl;
using bsl::atoi;

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define C_(X)   << #X << ": " << X << '\t'
#define A_(X,S) { if (!(X)) { cout S << endl; aSsErT(1, #X, __LINE__); } }
#define LOOP_ASSERT(I,X)            A_(X,C_(I))
#define LOOP2_ASSERT(I,J,X)         A_(X,C_(I)C_(J))
#define LOOP3_ASSERT(I,J,K,X)       A_(X,C_(I)C_(J)C_(K))
#define LOOP4_ASSERT(I,J,K,L,X)     A_(X,C_(I)C_(J)C_(K)C_(L))
#define LOOP5_ASSERT(I,J,K,L,M,X)   A_(X,C_(I)C_(J)C_(K)C_(L)C_(M))
#define LOOP6_ASSERT(I,J,K,L,M,N,X) A_(X,C_(I)C_(J)C_(K)C_(L)C_(M)C_(N))

#define Q   BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P   BDLS_TESTUTIL_P   // Print identifier and value.
#define P_  BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BDLS_TESTUTIL_L_  // current Line number

int main(int argc, char* argv[])
{
    int                test = argc > 1 ? atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;  // always the last

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // TESTING
        //   Breathing test
        //
        // Concerns:
        //:  1 BufferBuf object functions in a reasonable fashion.
        //:  2 The class is sufficiently functional to enable comprehensive
        //:    testing in subsequent test cases.
        //
        // Plan:
        //:  1 Try all operations see if basics work
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << bsl::endl
                               << "BREATHING TEST" << bsl::endl
                               << "==============" << bsl::endl;
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
