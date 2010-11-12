// bsls_logicerror.t.cpp                  -*-C++-*-

#include <bsls_logicerror.h>

#include <iostream>
#include <string>
#include <cstdlib>

using namespace BloombergLP;
using std::cout;
using std::endl;
using std::flush;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// 'bsls_LogicError' is just a 'typedef' from 'std::logic_error'.  Its testing
// is done in 'bsl+apache' package.  Furthermore, this component is deprecated.
// Therefore, minimal testing is done in this package.
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        std::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << std::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { std::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { std::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { std::cout << #I << ": " << I << "\t" << #J << ": " << J \
                          << "\t" << #K << ": " << K << "\n";           \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { std::cout << #I << ": " << I << "\t" << #J << ": " << J \
                          << "\t" << #K << ": " << K << "\t" << #L << ": " \
                          << L << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
    if (!(X)) { std::cout << #I << ": " << I << "\t" << #J << ": " << J    \
                          << "\t" << #K << ": " << K << "\t" << #L << ": " \
                          << L << "\t" << #M << ": " << M << "\n";         \
                aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
    if (!(X)) { std::cout << #I << ": " << I << "\t" << #J << ": " << J     \
                          << "\t" << #K << ": " << K << "\t" << #L << ": "  \
                          << L << "\t" << #M << ": " << M << "\t" << #N     \
                          << ": " << N << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) std::cout << #X " = " << (X) << std::endl; // Print ID and value.
#define Q(X) std::cout << "<| " #X " |>" << std::endl;  // Quote ID literally.
#define P_(X) std::cout << #X " = " << (X) << ", " << flush; // P(X) w/o '\n'
#define T_ std::cout << "\t" << flush;             // Print a tab (w/o newline)
#define L_ __LINE__                                // current Line number


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    std::cout << "TEST " << __FILE__ << " CASE " << test << std::endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   That a 'bsls_LogicError' can be thrown as an exception.
        //
        // Plan:
        //   Throw it.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) std::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << std::endl;

#ifdef BDE_BUILD_TARGET_EXC
        std::string result;

        try {
            throw bsls_LogicError("woof");
        }
        catch (const bsls_LogicError& e) {
            result = e.what();
        }
        catch (...) {
            ASSERT(0);
        }

        ASSERT(result == "woof");
#endif  // BDE_BUILD_TARGET_EXC
      } break;
      default: {
         std::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << std::endl;
         testStatus = -1;
      }
    }

    if (testStatus > 0) {
        std::cerr << "Error, non-zero test status = " << testStatus << "."
                  << std::endl;
    }


    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
