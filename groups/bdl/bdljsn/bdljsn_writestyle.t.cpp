// bdljsn_writestyle.t.cpp                                            -*-C++-*-

#include <bdljsn_writestyle.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// Standard enumeration test plan.
// ----------------------------------------------------------------------------
// TYPES
// [ 1] enum Enum { ... };
//
// CLASS METHODS
// [ 3] ostream& print(ostream&, WriteStyle::Enum, int, int)
// [ 2] static const char *toAscii(WriteStyle::Enum val);
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

// ============================================================================
//                       GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdljsn::WriteStyle Obj;
typedef Obj::Enum          Enum;

#define UNKNOWN_STYLE "(* UNKNOWN *)"

enum { ABOVE_ENUM_RANGE = Obj::e_COMPACT + 1 };

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------


static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT' and writes
        //:   to 'bsl::cout' with writes to an in-memory stream.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING USAGE EXAMPLE" << endl
                          << "=====================" << endl;

///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// 'bdljsn::WriteStyle'.
//
// First, we create a variable 'value' of type 'bdljsn::WriteStyle::Enum' and
// initialize it with the enumerator value 'bdljsn::WriteStyle::e_PRETTY':
//..
    bdljsn::WriteStyle::Enum value = bdljsn::WriteStyle::e_PRETTY;
//..
// Now, we store the address of its ASCII representation in a pointer variable,
// 'asciiValue', of type 'const char *':
//..
    const char *asciiValue = bdljsn::WriteStyle::toAscii(value);
    ASSERT(0 == bsl::strcmp(asciiValue, "PRETTY"));
//..
// Finally, we print 'value' to 'bsl::cout'.
//..
//  bsl::cout << value << bsl::endl;
    bslma::TestAllocator tmpDfltAlloc;
    ostringstream        oss(&tmpDfltAlloc);
    oss << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  PRETTY
//..
    ASSERT(0 == bsl::strcmp(oss.str().c_str(), "PRETTY\n"));
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'print'
        //
        // Concerns:
        //: 1 The 'print' method writes the output to the specified stream.
        //
        //: 2 The 'print' method writes the string representation of each
        //:   enumerator in the intended format.
        //
        //: 3 The 'print' method writes a distinguished string when passed an
        //:   out-of-band value.
        //
        //: 4 There is no output when the stream is invalid.
        //
        //: 5 The 'print' method has the expected signature.
        //
        //: 6 That 'print' returns a reference to the 'ostream' passed to it.
        //
        // Plan:
        //: 1 Verify that the 'print' method produces the expected results for
        //:   each enumerator, and that 'print' returns a reference to the
        //:   'ostream' passed to it.  (C-1 .. 2, 7)
        //
        //: 2 Verify that the 'print' method writes a distinguished string when
        //:   passed an out-of-band value.  (C-3)
        //
        //: 3 Verify that there is no output when the stream is invalid.  (C-4)
        //
        //: 4 Take the address of the 'print' (class) method and use the
        //:   result to initialize a variable of the appropriate type.  (C-5)
        //
        // Testing:
        //   ostream& print(ostream&, WriteStyle::Enum, int, int)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'print'" << endl
                          << "===============" << endl;

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -SP01
        static const struct {
            int         d_lineNum;  // source line number
            int         d_level;    // level
            int         d_spl;      // spaces per level
            int         d_value;    // enumerator value
            const char *d_exp_p;    // expected result
        } DATA[] = {
#define NL "\n"
            //LINE  LEVEL  SPL    ENUMERATOR VALUE        EXPECTED RESULT
            //----  -----  ---  ----------------------   ------------------
            { L_,     0,    4,  Obj::e_PRETTY,           "PRETTY" NL        },
            { L_,     0,    4,  Obj::e_ONELINE,          "ONELINE" NL       },
            { L_,     0,    4,  Obj::e_COMPACT,          "COMPACT" NL       },

#if !defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
            { L_,     0,    4,  ABOVE_ENUM_RANGE,        UNKNOWN_STYLE NL   },
            { L_,     0,    4,  -1,                      UNKNOWN_STYLE NL   },
            { L_,     0,    4,  -5,                      UNKNOWN_STYLE NL   },
            { L_,     0,    4,  99,                      UNKNOWN_STYLE NL   },
#endif

            { L_,     0,   -1,  Obj::e_PRETTY,           "PRETTY"           },
            { L_,     0,    0,  Obj::e_PRETTY,           "PRETTY" NL        },
            { L_,     0,    2,  Obj::e_PRETTY,           "PRETTY" NL        },
            { L_,     1,    1,  Obj::e_PRETTY,           " PRETTY" NL       },

            { L_,     1,    2,  Obj::e_PRETTY,           "  PRETTY" NL      },
            { L_,     1,    2,  Obj::e_ONELINE,          "  ONELINE" NL     },
            { L_,     1,    2,  Obj::e_COMPACT,          "  COMPACT" NL     },

            { L_,    -1,    2,  Obj::e_PRETTY,           "PRETTY" NL        },
            { L_,    -2,    1,  Obj::e_PRETTY,           "PRETTY" NL        },
            { L_,     2,    1,  Obj::e_PRETTY,           "  PRETTY" NL      },
            { L_,     1,    3,  Obj::e_PRETTY,           "   PRETTY" NL     },
#undef NL
        };
// BDE_VERIFY pragma: pop

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting 'print'." << endl;

        bslma::TestAllocator tmpDfltAlloc;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;
            const int   LEVEL = DATA[ti].d_level;
            const int   SPL   = DATA[ti].d_spl;
            const Enum  VALUE = static_cast<Enum>(DATA[ti].d_value);
            const char *EXP   = DATA[ti].d_exp_p;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }
            if (veryVerbose) cout << "EXPECTED FORMAT: " << EXP << endl;

            ostringstream oss(&tmpDfltAlloc);
            Obj::print(oss, VALUE, LEVEL, SPL);

            {
                bslma::DefaultAllocatorGuard allocatorGuard(&tmpDfltAlloc);

                const bsl::string& str = oss.str();

                if (veryVerbose) cout << "  ACTUAL FORMAT: " << str << endl;

                ASSERTV(LINE, EXP == str);
            }

            if (0 == LEVEL && 4 == SPL) {
                if (veryVerbose)
                    cout << "\tRepeat for 'print' default arguments." << endl;

                oss.str("");
                ostream *po = &Obj::print(oss, VALUE);
                ASSERT(&oss == po);

                bslma::DefaultAllocatorGuard allocatorGuard(&tmpDfltAlloc);

                const bsl::string& str2 = oss.str();

                if (veryVerbose) cout << "  ACTUAL FORMAT: " << str2;

                ASSERTV(LINE, EXP, str2, EXP == str2);
            }
        }

        if (verbose) cout << "\tNothing is written to a bad stream." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;
            const int   LEVEL = DATA[ti].d_level;
            const int   SPL   = DATA[ti].d_spl;
            const Enum  VALUE = static_cast<Enum>(DATA[ti].d_value);

            if (veryVerbose) { T_; P_(ti); P(VALUE); }

            ostringstream oss(&tmpDfltAlloc);
            oss.setstate(ios::badbit);
            Obj::print(oss, VALUE, LEVEL, SPL);

            bslma::DefaultAllocatorGuard allocatorGuard(&tmpDfltAlloc);

            ASSERTV(LINE, ti, "" == oss.str());
        }

        if (verbose) cout << "\nVerify 'print' signature." << endl;

        {
            typedef bsl::ostream& (*FuncPtr)(bsl::ostream&, Enum, int, int);

            const FuncPtr FP = &Obj::print;
            (void) FP;
        }

      } break;
      case 2: {
        // -------------------------------------------------------------------
        // TESTING 'toAscii'
        //
        // Concerns:
        //: 1 The 'toString' method returns the expected string representation
        //:   for each enumerator.
        //
        // Plan:
        //: 1 Test 'toString' with different enumerator values and checking
        //:   against the expected string representation.
        //
        // Testing:
        //   static const char *toAscii(WriteStyle::Enum val);
        // -------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'toAscii'"
                          << endl << "================="
                          << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                Enum        d_value;    // enumerator value
                const char *d_exp_p;    // expected result
            } DATA[] = {
                // line         enumerator value        expected result
                // ----    -----------------------      -----------------
                {  L_,     Obj::e_COMPACT,              "COMPACT"         },
                {  L_,     Obj::e_ONELINE,              "ONELINE"         },
                {  L_,     Obj::e_PRETTY,               "PRETTY"          },
                {  L_,     (Enum) -1,                   UNKNOWN_STYLE     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_lineNum;
                const Enum  VALUE = DATA[ti].d_value;
                const char *EXP   = DATA[ti].d_exp_p;

                const char *result = Obj::toAscii(VALUE);

                if (veryVerbose) { T_; P_(ti); P_(VALUE); P_(EXP); P(result); }

                LOOP2_ASSERT(LINE, ti, strlen(EXP) == strlen(result));
                LOOP2_ASSERT(LINE, ti,           0 == strcmp(EXP, result));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create a 'WriteStyle::Enum' object and assign it the valid
        //:   enumerator values.
        //
        // Testing:
        //   enum Enum { ... };
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "BASIC BREATHING TEST\n"
                      << "====================" << bsl::endl;
        }

        Enum e = Obj::e_PRETTY;
        ASSERTV(Obj::e_PRETTY, e, Obj::e_PRETTY == e);

        e = Obj::e_ONELINE;
        ASSERTV(Obj::e_ONELINE, e, Obj::e_ONELINE == e);

        e = Obj::e_COMPACT;
        ASSERTV(Obj::e_COMPACT, e, Obj::e_COMPACT == e);
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
