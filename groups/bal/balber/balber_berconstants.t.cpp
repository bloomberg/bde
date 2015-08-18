// balber_berconstants.t.cpp                                          -*-C++-*-

#include <balber_berconstants.h>

#include <bsl_sstream.h>
#include <bsl_iostream.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//
//

// ----------------------------------------------------------------------------

// ============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
// ----------------------------------------------------------------------------
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

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl; // Print ID and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote ID literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << flush; // P(X) w/o '\n'
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << flush;             // Print a tab (w/o newline)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static bool         verbose = false;
static bool     veryVerbose = false;
static bool veryVeryVerbose = false;

// ============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
// ----------------------------------------------------------------------------

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? bsl::atoi(argv[1]) : 0;
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
            const balber::BerConstants::TagClass X
                                           = balber::BerConstants::e_UNIVERSAL;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "UNIVERSAL" == ss.str());
        }
        {
            const balber::BerConstants::TagClass X
                                         = balber::BerConstants::e_APPLICATION;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "APPLICATION" == ss.str());
        }
        {
            const balber::BerConstants::TagClass X
                                    = balber::BerConstants::e_CONTEXT_SPECIFIC;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "CONTEXT_SPECIFIC" == ss.str());
        }
        {
            const balber::BerConstants::TagClass X
                                             = balber::BerConstants::e_PRIVATE;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "PRIVATE" == ss.str());
        }

        if (verbose) bsl::cout << "\nTesting TagType." << bsl::endl;
        {
            const balber::BerConstants::TagType X
                                           = balber::BerConstants::e_PRIMITIVE;

            bsl::stringstream ss;

            ss << X;

            LOOP_ASSERT(ss.str(), "PRIMITIVE" == ss.str());
        }
        {
            const balber::BerConstants::TagType X
                                         = balber::BerConstants::e_CONSTRUCTED;

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

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
