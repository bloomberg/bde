// bdljsn_jsontype.t.cpp                                              -*-C++-*-
#include <bdljsn_jsontype.h>

#include <bdlsb_fixedmemoutstreambuf.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_platform.h>
#include <bsls_review.h>

#include <bsl_cstdlib.h>  // 'bsl::atoi'
#include <bsl_cstring.h>  // 'bsl::strcmp'
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test implements a single enumeration having sequential
// enumerator values that start from 0.
//
// We will therefore follow our standard 3-step approach to testing enumeration
// types, with certain test cases omitted:
//: o [ 4] -- BDEX streaming is not (yet) implemented for this type.
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
// [ 3] ostream& print(ostream& s, Enum val, int level = 0, int sPL = 4);
// [ 1] const char *toAscii(bdljsn::JsonType::Enum val);
//
// FREE OPERATORS
// [ 2] operator<<(ostream& s, bdljsn::JsonType::Enum val);
// ----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE

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
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdljsn::JsonType::Enum Enum;
typedef bdljsn::JsonType       Obj;

// ============================================================================
//                       GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

const int NUM_ENUMERATORS = 6;

#define UNKNOWN_FORMAT "(* UNKNOWN *)"

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int test = argc > 1 ? atoi(argv[1]) : 0;

    BSLA_MAYBE_UNUSED const bool             verbose = argc > 2;
    BSLA_MAYBE_UNUSED const bool         veryVerbose = argc > 3;
    BSLA_MAYBE_UNUSED const bool     veryVeryVerbose = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << bsl::endl
                      << "USAGE EXAMPLE" << bsl::endl
                      << "=============" << bsl::endl;
        }

        bsl::streambuf *outStreamBuf = cout.rdbuf();
        bsl::streambuf *errStreamBuf = cerr.rdbuf();

        bsl::ostringstream ossOut;
        bsl::ostringstream ossErr;

        cout.rdbuf(ossOut.rdbuf());  // Redirect 'cout'.
        cerr.rdbuf(ossErr.rdbuf());  // Redirect 'cerr'.

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// 'bdljsn::JsonType'.
//
// First, we create a variable 'value' of type 'bdljsn::JsonType::Enum' and
// initialize it with the enumerator value 'bdljsn::JsonType::e_STRING':
//..
    bdljsn::JsonType::Enum value = bdljsn::JsonType::e_STRING;
//..
// Now, we store the address of its ASCII representation in a pointer variable,
// 'asciiValue', of type 'const char *':
//..
    const char *asciiValue = bdljsn::JsonType::toAscii(value);
    ASSERT(0 == bsl::strcmp(asciiValue, "STRING"));
//..
// Finally, we print 'value' to 'bsl::cout'.
//..
    bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  STRING
//..

        cout.rdbuf(outStreamBuf);    // Restore 'cout'.
        cerr.rdbuf(errStreamBuf);    // Restore 'cerr'.

        ASSERTV(ossOut.str(), "STRING\n" == ossOut.str());
        ASSERTV(ossErr.str(), ""         == ossErr.str());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING
        //   'bdex' streaming is not yet implemented for this enumeration.
        //
        // Concerns:
        //: 1 Not applicable.
        //
        // Plan:
        //: 1 Not applicable.
        //
        // Testing:
        //   Not applicable.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'bdex' STREAMING" << endl
                                  << "========================" << endl;

        if (veryVerbose) cout  << "Not implemented" << endl;

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'print'
        //
        // Concerns:
        //: 1 The 'print' method writes the output to the specified stream.
        //:   (P-1)
        //: 2 The 'print' method writes the string representation of each
        //:   enumerator in the intended format.  (P-1)
        //: 3 The 'print' method writes a distinguished string when passed an
        //:   out-of-band value.  (P-2)
        //: 4 There is no output when the stream is invalid.  (P-3)
        //: 5 The 'print' method has the expected signature.  (P-4)
        //
        // Plan:
        //: 1 Verify that the 'print' method produces the expected results for
        //:   each enumerator.  (C-1, C-2)
        //: 2 Verify that the 'print' method writes a distinguished string when
        //:   passed an out-of-band value.  (C-3)
        //: 3 Verify that there is no output when the stream is invalid.  (C-4)
        //: 4 Take the address of the 'print' (class) method and use the
        //:   result to initialize a variable of the appropriate type.  (C-5)
        //
        // Testing:
        //   ostream& print(ostream& s, Enum val, int level = 0, int sPL = 4);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'print'" << endl
                                  << "===============" << endl;

#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
        static const struct {
            int         d_lineNum;  // source line number
            int         d_level;    // level
            int         d_spl;      // spaces per level
            Enum        d_value;    // enumerator value
            const char *d_exp_p;    // expected result
        } DATA[] = {
#define NL "\n"
            //line  level  spl  enumerator value        expected result
            //----  -----  ---  ----------------------  ----------------
            {  L_,    0,    4,  Obj::e_OBJECT,          "OBJECT"       NL },
            {  L_,    0,    4,  Obj::e_ARRAY,           "ARRAY"        NL },
            {  L_,    0,    4,  Obj::e_STRING,          "STRING"       NL },
            {  L_,    0,    4,  Obj::e_NUMBER,          "NUMBER"       NL },
            {  L_,    0,    4,  Obj::e_BOOLEAN,         "BOOLEAN"      NL },
            {  L_,    0,    4,  Obj::e_NULL,            "NULL"         NL },

            {  L_,    0,    4,  (Enum)NUM_ENUMERATORS,  UNKNOWN_FORMAT NL },
            {  L_,    0,    4,  (Enum)-1,               UNKNOWN_FORMAT NL },
            {  L_,    0,    4,  (Enum)-5,               UNKNOWN_FORMAT NL },
            {  L_,    0,    4,  (Enum)99,               UNKNOWN_FORMAT NL },

            { L_,     0,   -1,  Obj::e_OBJECT,          "OBJECT"          },
            { L_,     0,    0,  Obj::e_OBJECT,          "OBJECT"       NL },
            { L_,     0,    2,  Obj::e_OBJECT,          "OBJECT"       NL },
            { L_,     1,    1,  Obj::e_OBJECT,          " OBJECT"      NL },
            { L_,     1,    2,  Obj::e_OBJECT,          "  OBJECT"     NL },
            { L_,    -1,    2,  Obj::e_OBJECT,          "OBJECT"       NL },
            { L_,    -2,    1,  Obj::e_OBJECT,          "OBJECT"       NL },
            { L_,     2,    1,  Obj::e_OBJECT,          "  OBJECT"     NL },
            { L_,     1,    3,  Obj::e_OBJECT,          "   OBJECT"    NL },
#undef NL
        };
#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic pop
#endif
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int   SIZE = 128;         // big enough to hold output string
        const char  XX   = static_cast<char>(0xFF);  // value of unset 'char'
        char        buf[SIZE];                       // output buffer

        char        mCtrl[SIZE];  memset(mCtrl, XX, SIZE);
        const char *CTRL = mCtrl;

        if (verbose) cout << "\nTesting 'print'." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;
            const int   LEVEL = DATA[ti].d_level;
            const int   SPL   = DATA[ti].d_spl;
            const Enum  VALUE = DATA[ti].d_value;
            const char *EXP   = DATA[ti].d_exp_p;

            memcpy(buf, CTRL, SIZE);  // Preset 'buf' to unset 'char' values.

            if (veryVerbose) { T_; P_(ti); P(VALUE); }
            if (veryVerbose) cout << "EXPECTED FORMAT: " << EXP << endl;

            bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
            bsl::ostream                out(&obuf);
            Obj::print(out, VALUE, LEVEL, SPL) << ends;

            if (veryVerbose) cout << "  ACTUAL FORMAT: " << buf << endl;

            const bsl::size_t SZ = strlen(EXP) + 1;
            LOOP2_ASSERT(LINE, ti, SZ  < SIZE);           // Buffer is large
                                                          // enough.
            LOOP2_ASSERT(LINE, ti, XX == buf[SIZE - 1]);  // Check for overrun.
            LOOP2_ASSERT(LINE, ti,  0 == memcmp(buf, EXP, SZ));
            LOOP2_ASSERT(LINE, ti,  0 == memcmp(buf + SZ,
                                                CTRL + SZ, SIZE - SZ));

            if (0 == LEVEL && 4 == SPL) {
                if (veryVerbose)
                    cout << "\tRepeat for 'print' default arguments." << endl;

                memcpy(buf, CTRL, SIZE);  // Preset 'buf' to unset 'char'
                                          // values.

                bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
                bsl::ostream                out(&obuf);
                Obj::print(out, VALUE) << ends;

                if (veryVerbose) cout << "  ACTUAL FORMAT: " << buf << endl;

                LOOP2_ASSERT(LINE, ti, XX == buf[SIZE - 1]);  // Check for
                                                              // overrun.
                LOOP2_ASSERT(LINE, ti,  0 == memcmp(buf, EXP, SZ));
                LOOP2_ASSERT(LINE, ti,  0 == memcmp(buf + SZ,
                                                    CTRL + SZ, SIZE - SZ));
            }
        }

        if (verbose) cout << "\tNothing is written to a bad stream." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE  = DATA[ti].d_lineNum;
            const int  LEVEL = DATA[ti].d_level;
            const int  SPL   = DATA[ti].d_spl;
            const Enum VALUE = DATA[ti].d_value;

            BSLA_MAYBE_UNUSED const char *EXP = DATA[ti].d_exp_p;

            memcpy(buf, CTRL, SIZE);  // Preset 'buf' to unset 'char' values.

            if (veryVerbose) { T_; P_(ti); P(VALUE); }

            bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
            bsl::ostream                out(&obuf);
            out.setstate(ios::badbit);
            Obj::print(out, VALUE, LEVEL, SPL);

            LOOP2_ASSERT(LINE, ti, 0 == memcmp(buf, CTRL, SIZE));
        }

        if (verbose) cout << "\nVerify 'print' signature." << endl;

        {
            typedef bsl::ostream& (*FuncPtr)(bsl::ostream&, Enum, int, int);

            BSLA_MAYBE_UNUSED const FuncPtr FP = &Obj::print;
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING '<<' OPERATOR
        //
        // Concerns:
        //: 1 The '<<' operator writes the output to the specified stream.
        //:   (P-1)
        //: 2 The '<<' operator writes the string representation of each
        //:   enumerator in the intended format.  (P-1)
        //: 3 The '<<' operator writes a distinguished string when passed an
        //:   out-of-band value.  (P-2)
        //: 4 The output produced by 'stream << value' is the same as that
        //:   produced by 'Obj::print(stream, value, 0, -1)'.  (P-3)
        //: 5 There is no output when the stream is invalid.  (P-4)
        //: 6 The '<<' operator has the expected signature.  (P-5)
        //
        // Plan:
        //: 1 Verify that the '<<' operator produces the expected results for
        //:   each enumerator.  (C-1, C-2)
        //: 2 Verify that the '<<' operator writes a distinguished string when
        //:   passed an out-of-band value.  (C-3)
        //: 3 Verify that 'stream << value' writes the same output as
        //:   'Obj::print(stream, value, 0, -1)'.  (C-4)
        //: 4 Verify that there is no output when the stream is invalid.  (C-5)
        //: 5 Take the address of the '<<' (free) operator and use the result
        //:   to initialize a variable of the appropriate type.  (C-6)
        //
        // Testing:
        //   operator<<(ostream& s, bdljsn::JsonType::Enum val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING '<<' OPERATOR" << endl
                                  << "=====================" << endl;

#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
        static const struct {
            int         d_lineNum;  // source line number
            Enum        d_value;    // enumerator value
            const char *d_exp_p;    // expected result
        } DATA[] = {
            // line         enumerator value        expected result
            // ----    -----------------------      -----------------
            {  L_,     Obj::e_OBJECT,               "OBJECT"          },
            {  L_,     Obj::e_ARRAY,                "ARRAY"           },
            {  L_,     Obj::e_STRING,               "STRING"          },
            {  L_,     Obj::e_NUMBER,               "NUMBER"          },
            {  L_,     Obj::e_BOOLEAN,              "BOOLEAN"         },
            {  L_,     Obj::e_NULL,                 "NULL"            },

            {  L_,     (Enum)NUM_ENUMERATORS,       UNKNOWN_FORMAT    },
            {  L_,     (Enum)-1,                    UNKNOWN_FORMAT    },
            {  L_,     (Enum)-5,                    UNKNOWN_FORMAT    },
            {  L_,     (Enum)99,                    UNKNOWN_FORMAT    }
        };
#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic pop
#endif
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int   SIZE = 128;         // big enough to hold output string
        const char  XX   = static_cast<char>(0xFF);  // value of unset 'char'
        char        buf[SIZE];                       // output buffer

        char        mCtrl[SIZE];  memset(mCtrl, XX, SIZE);
        const char *CTRL = mCtrl;

        if (verbose) cout << "\nTesting '<<' operator." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;
            const Enum  VALUE = DATA[ti].d_value;
            const char *EXP   = DATA[ti].d_exp_p;

            memcpy(buf, CTRL, SIZE);  // Preset 'buf' to unset 'char' values.

            if (veryVerbose) { T_; P_(ti); P(VALUE); }
            if (veryVerbose) cout << "EXPECTED FORMAT: " << EXP << endl;

            bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
            bsl::ostream                out(&obuf);
            out << VALUE << ends;

            if (veryVerbose) cout << "  ACTUAL FORMAT: " << buf << endl;

            const bsl::size_t SZ = strlen(EXP) + 1;
            LOOP2_ASSERT(LINE, ti, SZ  < SIZE);           // Buffer is large
                                                          // enough.
            LOOP2_ASSERT(LINE, ti, XX == buf[SIZE - 1]);  // Check for overrun.
            LOOP2_ASSERT(LINE, ti,  0 == memcmp(buf, EXP, SZ));
            LOOP2_ASSERT(LINE, ti,  0 == memcmp(buf + SZ,
                                                CTRL + SZ, SIZE - SZ));
        }

        if (verbose) cout << "\tNothing is written to a bad stream." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE  = DATA[ti].d_lineNum;
            const Enum VALUE = DATA[ti].d_value;

            memcpy(buf, CTRL, SIZE);  // Preset 'buf' to unset 'char' values.

            if (veryVerbose) { T_; P_(ti); P(VALUE); }

            bdlsb::FixedMemOutStreamBuf obuf(buf, sizeof buf);
            bsl::ostream                out(&obuf);
            out.setstate(ios::badbit);
            out << VALUE;

            LOOP2_ASSERT(LINE, ti, 0 == memcmp(buf, CTRL, SIZE));
        }

        if (verbose) cout << "\nVerify '<<' operator signature." << endl;

        {
            using namespace bdljsn;
            typedef bsl::ostream& (*FuncPtr)(bsl::ostream&, Enum);

            BSLA_MAYBE_UNUSED const FuncPtr FP = &operator<<;
        }
      } break;
      case 1: {
        // -------------------------------------------------------------------
        // TESTING 'enum' AND 'toAscii'
        //
        // Concerns:
        //: 1 The enumerator values are sequential, starting from 0.  (P-1)
        //: 2 The 'toAscii' method returns the expected string representation
        //:   for each enumerator.  (P-2)
        //: 3 The 'toAscii' method returns a distinguished string when passed
        //:   an out-of-band value.  (P-3)
        //: 4 The string returned by 'toAscii' is non-modifiable.  (P-4)
        //: 5 The 'toAscii' method has the expected signature.  (P-4)
        //
        // Plan:
        //: 1 Verify that the enumerator values are sequential, starting from
        //:   0.  (C-1)
        //: 2 Verify that the 'toAscii' method returns the expected string
        //:   representation for each enumerator.  (C-2)
        //: 3 Verify that the 'toAscii' method returns a distinguished string
        //:   when passed an out-of-band value.  (C-3)
        //: 4 Take the address of the 'toAscii' (class) method and use the
        //:   result to initialize a variable of the appropriate type.
        //:   (C-4, C-5)
        //
        // Testing:
        //   enum Enum { ... };
        //   const char *toAscii(bdljsn::JsonType::Enum val);
        // -------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'enum' AND 'toAscii'" << endl
                                  << "============================" << endl;

#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
        static const struct {
            int         d_lineNum;  // source line number
            Enum        d_value;    // enumerator value
            const char *d_exp_p;    // expected result
        } DATA[] = {
            // line         enumerator value        expected result
            // ----    -----------------------      -----------------
            {  L_,     Obj::e_OBJECT,               "OBJECT"          },
            {  L_,     Obj::e_ARRAY,                "ARRAY"           },
            {  L_,     Obj::e_STRING,               "STRING"          },
            {  L_,     Obj::e_NUMBER,               "NUMBER"          },
            {  L_,     Obj::e_BOOLEAN,              "BOOLEAN"         },
            {  L_,     Obj::e_NULL,                 "NULL"            },

            {  L_,     (Enum)NUM_ENUMERATORS,       UNKNOWN_FORMAT    },
            {  L_,     (Enum)-1,                    UNKNOWN_FORMAT    },
            {  L_,     (Enum)-5,                    UNKNOWN_FORMAT    },
            {  L_,     (Enum)99,                    UNKNOWN_FORMAT    }
        };
#ifdef BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic pop
#endif
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nVerify enumerator values are sequential."
                          << endl;

        for (int ti = 0; ti < NUM_ENUMERATORS; ++ti) {
            const Enum VALUE = DATA[ti].d_value;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }

            LOOP_ASSERT(ti, ti == VALUE);
        }

        if (verbose) cout << "\nTesting 'toAscii'." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;
            const Enum  VALUE = DATA[ti].d_value;
            const char *EXP   = DATA[ti].d_exp_p;

            const char *result = Obj::toAscii(VALUE);

            if (veryVerbose) { T_; P_(ti); P_(VALUE); P_(EXP); P(result); }

            LOOP2_ASSERT(LINE, ti, strlen(EXP) == strlen(result));
            LOOP2_ASSERT(LINE, ti,           0 == strcmp(EXP, result));
        }

        if (verbose) cout << "\nVerify 'toAscii' signature." << endl;

        {
            typedef const char *(*FuncPtr)(Enum);

            BSLA_MAYBE_UNUSED const FuncPtr FP = &Obj::toAscii;
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      } break;
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = "
                  << testStatus
                  << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
