// baexml_encodingstyle.t.cpp   -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_encodingstyle_t_cpp,"$Id$ $CSID$")

#include <baexml_encodingstyle.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

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
#define T_ cout << "\t" << flush;             // Print tab w/o newline.

// =========================================================================
//                       GLOBAL CONSTANTS FOR TESTING
// -------------------------------------------------------------------------

#define UNKNOWN_FORMAT "(* UNKNOWN *)"

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------


static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // -------------------------------------------------------------------
        // TESTING 'toString' and 'fromString'
        //
        // Concerns:
        //: 1 The 'toString' method returns the expected string representation
        //:   for each enumerator.
        //: 2 The 'fromString' method returns the right enumeration value.
        //
        // Plan:
        //: 1 Test 'toString' with different enumerator values and checking
        //:   against the expected string representation.
        //: 2 Test 'fromString' with different string values and checking
        //:   against the expected enumerator value.
        //
        // Testing:
        //   const char *toString(Value val);
        //   int fromString(Value *result, const char *str, int len);
        // -------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'toString' and 'fromString'"
                          << endl << "==================================="
                          << endl;

        typedef baexml_EncodingStyle::Value Enum;
        typedef baexml_EncodingStyle        Obj;

        if (verbose) cout << "\nTesting 'toString'." << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                Enum        d_value;    // enumerator value
                const char *d_exp;      // expected result
            } DATA[] = {
                // line         enumerator value        expected result
                // ----    -----------------------      -----------------
                {  L_,     Obj::BAEXML_COMPACT,         "COMPACT"         },
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                {  L_,     Obj::COMPACT,                "COMPACT"         },
#endif
                {  L_,     Obj::BAEXML_PRETTY,          "PRETTY"          },
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                {  L_,     Obj::PRETTY,                 "PRETTY"          },
#endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_lineNum;
                const Enum  VALUE = DATA[ti].d_value;
                const char *EXP   = DATA[ti].d_exp;

                const char *result = Obj::toString(VALUE);

                if (veryVerbose) { T_; P_(ti); P_(VALUE); P_(EXP); P(result); }

                LOOP2_ASSERT(LINE, ti, strlen(EXP) == strlen(result));
                LOOP2_ASSERT(LINE, ti,           0 == strcmp(EXP, result));
            }
        }

        if (verbose) cout << "\nTesting 'fromString'." << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                Enum        d_expValue; // expected enumerator value
                const char *d_inputStr; // input string
            } DATA[] = {
                // line         enumerator value        expected result
                // ----    -----------------------      -----------------
                {  L_,     Obj::BAEXML_COMPACT,         "COMPACT"         },
                {  L_,     Obj::BAEXML_COMPACT,         "compact"         },
                {  L_,     Obj::BAEXML_COMPACT,         "BAEXML_COMPACT"  },
                {  L_,     Obj::BAEXML_COMPACT,         "baexml_compact"  },
                {  L_,     Obj::BAEXML_PRETTY,          "PRETTY"          },
                {  L_,     Obj::BAEXML_PRETTY,          "pretty"          },
                {  L_,     Obj::BAEXML_PRETTY,          "BAEXML_PRETTY"   },
                {  L_,     Obj::BAEXML_PRETTY,          "baexml_pretty"   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE      = DATA[ti].d_lineNum;
                const Enum  EXP_VALUE = DATA[ti].d_expValue;
                const char *STR       = DATA[ti].d_inputStr;

                Enum value;
                int rc = Obj::fromString(&value, STR, strlen(STR));

                if (veryVerbose) { T_; P_(ti); P_(value); P_(STR); P(rc); }

                LOOP_ASSERT(LINE, !rc);
                LOOP_ASSERT(LINE, EXP_VALUE == value);
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Basic Attribute Test:
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "Basic Attribute Test" << bsl::endl
                      << "====================" << bsl::endl;
        }

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

// GENERATED BY BLP_BAS_CODEGEN_3.5.0 Tue May  4 08:22:33 2010
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
