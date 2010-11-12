// bdem_berconstants.t.cpp                  -*-C++-*-

#include <bdem_berconstants.h>

#include <bsl_sstream.h>
#include <bsl_iostream.h>
#include <bsl_cstdlib.h>

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
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\n";           \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J    \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\n";         \
               aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J     \
                         << "\t" << #K << ": " << K << "\t" << #L << ": "  \
                         << L << "\t" << #M << ": " << M << "\t" << #N     \
                         << ": " << N << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl; // Print ID and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote ID literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << flush; // P(X) w/o '\n'
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Output (<<) Operator"
                               << "\n============================"
                               << bsl::endl;

        if (verbose) bsl::cout << "\nTesting TagClass." << bsl::endl;
        {
            const bdem_BerConstants::TagClass X
                                           = bdem_BerConstants::BDEM_UNIVERSAL;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "UNIVERSAL" == ss.str());
        }
        {
            const bdem_BerConstants::TagClass X
                                         = bdem_BerConstants::BDEM_APPLICATION;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "APPLICATION" == ss.str());
        }
        {
            const bdem_BerConstants::TagClass X
                                    = bdem_BerConstants::BDEM_CONTEXT_SPECIFIC;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "CONTEXT_SPECIFIC" == ss.str());
        }
        {
            const bdem_BerConstants::TagClass X
                                             = bdem_BerConstants::BDEM_PRIVATE;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "PRIVATE" == ss.str());
        }

        if (verbose) bsl::cout << "\nTesting TagType." << bsl::endl;
        {
            const bdem_BerConstants::TagType X
                                           = bdem_BerConstants::BDEM_PRIMITIVE;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "PRIMITIVE" == ss.str());
        }
        {
            const bdem_BerConstants::TagType X
                                         = bdem_BerConstants::BDEM_CONSTRUCTED;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "CONSTRUCTED" == ss.str());
        }

        if (verbose) bsl::cout << "\nEnd of test." << bsl::endl;
      } break;

      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
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
