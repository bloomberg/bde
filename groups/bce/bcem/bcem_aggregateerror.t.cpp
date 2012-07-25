// bcem_aggregateerror.t.cpp                                          -*-C++-*-

#include <bcem_aggregateerror.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"
#define T_() cout << '\t' << flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bcem_AggregateError Obj;
typedef bcem_ErrorCode      Error;

void doSomething(Obj* error)
{
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // TESTING usage example
        //
        // Test that the usage example compiles
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << bsl::endl << "TESTING Usage Example"
                               << bsl::endl << "====================="
                               << bsl::endl;

        bcem_AggregateError error;
        doSomething(&error);

        if (0 != error.code()) {
            bsl::cout << "Error: " << error << bsl::endl;
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING output
        //
        // Test that print output is as expected.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl << "TESTING output"
                               << bsl::endl << "=============="
                               << bsl::endl;

        Obj mX;

        bsl::ostringstream ss;
        ss << mX;

        LOOP_ASSERT(ss.str(),
                    "[ description = \"\" code = SUCCESS ]" == ss.str());

        mX.code() = Error::BCEM_BAD_ARRAYINDEX;
        mX.description() = "Out of bounds";

        ss.str("");
        ss << mX;

        LOOP_ASSERT(ss.str(),
                    "[ description = \"Out of bounds\" "
                    "code = BAD_ARRAYINDEX ]" == ss.str());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Attribute test
        //
        // Test all the accessors and manipulators.
        // --------------------------------------------------------------------

          if (verbose) bsl::cout << bsl::endl << "Attribute Test"
                                 << bsl::endl << "=============="
                                 << bsl::endl;

          Obj mX;

          Obj mY = mX;
          const Obj& Y = mY;
          const Obj& X = mX;

          ASSERT(Y.description().empty());
          ASSERT(Error::BCEM_SUCCESS == X.code());

          mX.code() = Error::BCEM_NON_CONFORMANT;
          ASSERT(Error::BCEM_NON_CONFORMANT == mX.code());
          ASSERT(Error::BCEM_NON_CONFORMANT == X.code());
          mY = mX;
          ASSERT(Error::BCEM_NON_CONFORMANT == Y.code());

          mX.description() = "plugh";
          ASSERT("plugh" == mX.description());
          ASSERT("plugh" == X.description());
          mY = mX;
          ASSERT("plugh" == Y.description());

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

          if (verbose) bsl::cout << bsl::endl << "Breathing Test"
                                 << bsl::endl << "=============="
                                 << bsl::endl;

          Obj mX; const Obj& X = mX;
          ASSERT(X.description().empty());
          ASSERT(Error::BCEM_SUCCESS == X.code());

          const string expDesc = "Pick one field";
          Obj mY(Error::BCEM_AMBIGUOUS_ANON, expDesc.c_str());
          const Obj& Y = mY;
          ASSERT(expDesc == Y.description());
          ASSERT(Error::BCEM_AMBIGUOUS_ANON == Y.code());

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
