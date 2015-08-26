// ball_userfieldtype.t.cpp                                           -*-C++-*-
#include <ball_userfieldtype.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test provides an enumeration that lists the months of
// the year and assigns them sequential values that start at 1.  Additionally,
// the component defines a constant to represent the number of months in a
// year.
//
// We will therefore follow our standard 3-step approach to testing enumeration
// types.
//
// Global Concerns:
//: o No methods or free operators allocate memory.
//
// Global Assumptions:
//: o All CLASS METHODS and the '<<' free operator are 'const' thread-safe.
// ----------------------------------------------------------------------------
// TYPES
// [ 1] enum Enum { ... };
//
// CLASS METHODS
// [ 2] ostream& print(ostream& s, Enum val, int level = 0, int sPL = 4);
// [ 1] const char *toAscii(UserFieldType::Enum val);
//
// FREE OPERATORS
// [ 3] operator<<(ostream& s, UserFieldType::Enum val);
//
// ----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::UserFieldType Obj;
typedef Obj::Enum           Enum;

// ============================================================================
//                       GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

const int NUM_ENUMERATORS = 5;

#define UNKNOWN_FORMAT "(* UNKNOWN *)"

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator defaultAllocator(veryVeryVerbose);
    bslma::Default::setDefaultAllocatorRaw(&defaultAllocator);

    bslma::TestAllocator globalAllocator(veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'ball::UserFieldType' usage.
//
// First, we create a variable 'value' of type 'ball::UserFieldType::Enum' and
// initialize it to the value 'ball::UserFieldType::e_STRING':
//..
    ball::UserFieldType::Enum value = ball::UserFieldType::e_STRING;
//..
// Next, we store a pointer to its ASCII representation in a variable
// 'asciiValue' of type 'const char *':
//..
    const char *asciiValue = ball::UserFieldType::toAscii(value);
    ASSERT(0 == bsl::strcmp(asciiValue, "STRING"));
//..
// Finally, we print the value to 'bsl::cout':
//..
if (veryVerbose)
    bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  STRING
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT ('<<') OPERATOR
        //
        // Concerns:
        //: 1 The '<<' operator writes the output to the specified stream.
        //:
        //: 2 The '<<' operator writes the string representation of each
        //:   enumerator in the intended format.
        //:
        //: 3 The '<<' operator writes a distinguished string when passed an
        //:   out-of-band value.
        //:
        //: 4 The output produced by 'stream << value' is the same as that
        //:   produced by 'Obj::print(stream, value, 0, -1)'.
        //:
        //: 5 There is no output when the stream is invalid.
        //:
        //: 6 The '<<' operator has the expected signature.
        //:
        //: 7 The '<<' operator returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Verify that the '<<' operator produces the expected results for
        //:   each enumerator.  (C-1..2)
        //:
        //: 2 Verify that the '<<' operator writes a distinguished string when
        //:   passed an out-of-band value.  (C-3)
        //:
        //: 3 Verify that 'stream << value' writes the same output as
        //:   'Obj::print(stream, value, 0, -1)'.  (C-4)
        //:
        //: 4 Verify that the address of the returned 'stream' is the same as
        //:   the supplied 'stream'.  (C-7)
        //:
        //: 5 Verify that there is no output when the stream is invalid.  (C-5)
        //:
        //: 6 Take the address of the '<<' (free) operator and use the result
        //:   to initialize a variable of the appropriate type.  (C-6)
        //
        // Testing:
        //   operator<<(ostream& s, UserFieldType::Enum val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING OUTPUT ('<<') OPERATOR" << endl
                                  << "==============================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            Enum        d_value;    // enumerator value
            const char *d_exp_p;    // expected result
        } DATA[] = {
            // line   enumerator value                       expected output
            // ----   ----------------                       ---------------
            {  L_,    Obj::e_VOID,                           "VOID"         },
            {  L_,    Obj::e_INT64,                          "INT64"         },
            {  L_,    Obj::e_DOUBLE,                         "DOUBLE"        },
            {  L_,    Obj::e_STRING,                         "STRING"        },
            {  L_,    Obj::e_DATETIMETZ,                     "DATETIMETZ"    },

            {  L_,    static_cast<Enum>(-1),                  UNKNOWN_FORMAT },
            {  L_,    static_cast<Enum>(NUM_ENUMERATORS),     UNKNOWN_FORMAT },

            {  L_,    static_cast<Enum>(-5),                  UNKNOWN_FORMAT },
            {  L_,    static_cast<Enum>(99),                  UNKNOWN_FORMAT }
        };

        const int   NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting '<<' operator." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE  = DATA[ti].d_lineNum;
            const Enum   VALUE = DATA[ti].d_value;
            const string EXP   = DATA[ti].d_exp_p;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }
            if (veryVerbose) cout << "EXPECTED FORMAT: " << EXP << endl;

            bslma::TestAllocator scratch("scratch", veryVeryVerbose);
            ostringstream os(&scratch);

            ASSERTV(LINE, &os == &(os << VALUE));

            if (veryVerbose) cout << "  ACTUAL FORMAT: " << os.str() << endl;

            ASSERTV(LINE, ti, EXP, os.str(), EXP == os.str());

            if (veryVerbose) cout << "\tVerify that the output matches that of"
                                  << " print with level = 0 and spl = -1."
                                  << endl;
            {
                ostringstream out(&scratch);

                Obj::print(out, VALUE, 0, -1);

                ASSERTV(LINE, ti, os.str(), out.str(), os.str() == out.str());
            }
        }

        if (verbose) cout << "\tNothing is written to a bad stream." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE  = DATA[ti].d_lineNum;
            const Enum   VALUE = DATA[ti].d_value;
            const string EXP   = DATA[ti].d_exp_p;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }

            bslma::TestAllocator scratch("scratch", veryVeryVerbose);
            ostringstream os(&scratch);   os.setstate(ios::failbit);

            ASSERTV(LINE, &os == &(os << VALUE));

            ASSERTV(LINE, ti, os.str(), os.str().empty());
        }

        if (verbose) cout << "\nVerify '<<' operator signature." << endl;

        {
            using namespace ball;

            typedef bsl::ostream& (*FuncPtr)(bsl::ostream&, Enum);

            const FuncPtr FP = &operator<<;

            (void)FP;   // quash potential compiler warning
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'print'
        //
        // Concerns:
        //: 1 The 'print' method writes the output to the specified stream.
        //:
        //: 2 The 'print' method writes the string representation of each
        //:   enumerator in the intended format taking the 'level' and
        //:   'spacesPerLevel' parameter values into account.
        //:
        //: 3 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values.
        //:
        //: 4 The 'print' method writes a distinguished string when passed an
        //:   out-of-band value.
        //:
        //: 5 There is no output when the stream is invalid.
        //:
        //: 6 The 'print' method has the expected signature.
        //:
        //: 7 The 'print' method returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Verify that the 'print' method produces the expected results for
        //:   each enumerator.  (C-1..3)
        //:
        //: 2 Verify that the 'print' method writes a distinguished string when
        //:   passed an out-of-band value.  (C-4)
        //:
        //: 3 Verify that the address of the returned 'stream' is the same as
        //:   the supplied 'stream'.  (C-7)
        //:
        //: 4 Verify that there is no output when the stream is invalid.  (C-5)
        //:
        //: 5 Take the address of the 'print' (class) method and use the
        //:   result to initialize a variable of the appropriate type.  (C-6)
        //
        // Testing:
        //   ostream& print(ostream& s, Enum val, int level = 0, int sPL = 4);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'print'" << endl
                                  << "===============" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            int         d_level;    // level
            int         d_spl;      // spaces per level
            Enum        d_value;    // enumerator value
            const char *d_exp_p;    // expected result
        } DATA[] = {
#define NL "\n"
            //line level spl    enumerator value          expected result
            //---- ----- --- ----------------------       -----------------
            { L_,    0,   4, Obj::e_VOID,                  "VOID" NL         },
            { L_,    0,   4, Obj::e_INT64,                "INT64" NL         },
            { L_,    0,   4, Obj::e_DOUBLE,               "DOUBLE" NL        },
            { L_,    0,   4, Obj::e_STRING,               "STRING" NL        },
            { L_,    0,   4, Obj::e_DATETIMETZ,           "DATETIMETZ" NL    },

            { L_,    0,   4, static_cast<Enum>(NUM_ENUMERATORS + 1),
                                                          UNKNOWN_FORMAT NL  },
            { L_,    0,   4, static_cast<Enum>(-1),       UNKNOWN_FORMAT NL  },
            { L_,    0,   4, static_cast<Enum>(-5),       UNKNOWN_FORMAT NL  },
            { L_,    0,   4, static_cast<Enum>(99),       UNKNOWN_FORMAT NL  },

            { L_,    0,  -1, Obj::e_INT64,                "INT64"            },
            { L_,    0,   0, Obj::e_STRING,               "STRING" NL        },
            { L_,    0,   2, Obj::e_DATETIMETZ,           "DATETIMETZ" NL    },
            { L_,    1,   1, Obj::e_INT64,                " INT64" NL        },
            { L_,    1,   2, Obj::e_INT64,                "  INT64" NL       },
            { L_,   -1,   2, Obj::e_INT64,                "INT64" NL         },
            { L_,   -2,   1, Obj::e_INT64,                "INT64" NL         },
            { L_,    2,   1, Obj::e_STRING,               "  STRING" NL      },
            { L_,    1,   3, Obj::e_STRING,               "   STRING" NL     },
#undef NL
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting 'print'." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE  = DATA[ti].d_lineNum;
            const int    LEVEL = DATA[ti].d_level;
            const int    SPL   = DATA[ti].d_spl;
            const Enum   VALUE = DATA[ti].d_value;
            const string EXP   = DATA[ti].d_exp_p;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }
            if (veryVerbose) cout << "EXPECTED FORMAT: " << EXP << endl;

            bslma::TestAllocator scratch("scratch", veryVeryVerbose);
            ostringstream os(&scratch);

            ASSERTV(LINE, &os == &Obj::print(os, VALUE, LEVEL, SPL));
            ASSERTV(LINE, ti, EXP, os.str(), EXP == os.str());

            if (veryVerbose) cout << "  ACTUAL FORMAT: " << os.str() << endl;

            if (0 == LEVEL && 4 == SPL) {
                if (veryVerbose)
                    cout << "\tRepeat for 'print' default arguments." << endl;

                bslma::TestAllocator scratch("scratch", veryVeryVerbose);
                ostringstream os(&scratch);

                ASSERTV(LINE, &os == &Obj::print(os, VALUE));
                ASSERTV(LINE, ti, EXP, os.str(), EXP == os.str());

                if (veryVerbose) cout << "  ACTUAL FORMAT: " << os.str()
                                      << endl;
            }
        }

        if (verbose) cout << "\tNothing is written to a bad stream." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE  = DATA[ti].d_lineNum;
            const int    LEVEL = DATA[ti].d_level;
            const int    SPL   = DATA[ti].d_spl;
            const Enum   VALUE = DATA[ti].d_value;
            const string EXP   = DATA[ti].d_exp_p;

            if (veryVerbose) { T_; P_(ti); P_(LEVEL); P_(SPL); P(VALUE); }

            bslma::TestAllocator scratch("scratch", veryVeryVerbose);
            ostringstream os(&scratch);   os.setstate(ios::failbit);

            ASSERTV(LINE, &os == &Obj::print(os, VALUE));
            ASSERTV(LINE, ti, os.str(), os.str().empty());
        }

        if (verbose) cout << "\nVerify 'print' signature." << endl;

        {
            typedef bsl::ostream& (*FuncPtr)(bsl::ostream&, Enum, int, int);

            const FuncPtr FP = &Obj::print;

            (void)FP;   // quash potential compiler warning
        }
      } break;
      case 1: {
        // -------------------------------------------------------------------
        // TESTING 'enum' AND 'toAscii'
        //
        // Concerns:
        //: 1 The enumerator values are sequential, starting from 1.
        //:
        //: 2 The 'toAscii' method returns the expected string representation
        //:   for each enumerator.
        //:
        //: 3 The 'toAscii' method returns a distinguished string when passed
        //:   an out-of-band value.
        //:
        //: 4 The string returned by 'toAscii' is non-modifiable.
        //:
        //: 5 The 'toAscii' method has the expected signature.
        //:
        //
        // Plan:
        //: 1 Verify that the enumerator values are sequential, starting from
        //:   1.  (C-1)
        //:
        //: 2 Verify that the 'toAscii' method returns the expected string
        //:   representation for each enumerator.  (C-2)
        //:
        //: 3 Verify that the 'toAscii' method returns a distinguished string
        //:   when passed an out-of-band value.  (C-3)
        //:
        //: 4 Take the address of the 'toAscii' (class) method and use the
        //:   result to initialize a variable of the appropriate type.
        //:   (C-4..5)
        //:
        //
        // Testing:
        //   enum Enum { ... };
        //   const char *toAscii(UserFieldType::Enum val);
        // -------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'enum' AND 'toAscii'" << endl
                                  << "============================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            Enum        d_value;    // enumerator value
            const char *d_exp_p;    // expected result
        } DATA[] = {
            // line   enumerator value                       expected output
            // ----   ----------------                       ---------------
            {  L_,    Obj::e_VOID,                            "VOID"         },
            {  L_,    Obj::e_INT64,                          "INT64"         },
            {  L_,    Obj::e_DOUBLE,                         "DOUBLE"        },
            {  L_,    Obj::e_STRING,                         "STRING"        },
            {  L_,    Obj::e_DATETIMETZ,                     "DATETIMETZ"    },

            {  L_,    static_cast<Enum>(-1),                  UNKNOWN_FORMAT },
            {  L_,    static_cast<Enum>(NUM_ENUMERATORS),     UNKNOWN_FORMAT },

            {  L_,    static_cast<Enum>(-5),                  UNKNOWN_FORMAT },
            {  L_,    static_cast<Enum>(99),                  UNKNOWN_FORMAT }
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nVerify enumerator values are sequential."
                          << endl;

        for (int ti = 0; ti < NUM_ENUMERATORS; ++ti) {
            const Enum VALUE = DATA[ti].d_value;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }

            ASSERTV(ti, VALUE, ti == VALUE);
        }

        if (verbose) cout << "\nTesting 'toAscii'." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;
            const Enum  VALUE = DATA[ti].d_value;
            const char *EXP   = DATA[ti].d_exp_p;

            const char *result = Obj::toAscii(VALUE);

            if (veryVerbose) { T_; P_(ti); P_(VALUE); P_(EXP); P(result); }

            ASSERTV(LINE, ti, strlen(EXP) == strlen(result));
            ASSERTV(LINE, ti,           0 == strcmp(EXP, result));
        }

        if (verbose) cout << "\nVerify 'toAscii' signature." << endl;

        {
            typedef const char *(*FuncPtr)(Enum);

            const FuncPtr FP = &Obj::toAscii;

            (void) FP;   // quash potential compiler warning
        }

      } break;
      default: {
          cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
          testStatus = -1;
      }
    }

    ASSERT(0 == defaultAllocator.numBlocksTotal());
    ASSERT(0 ==  globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
