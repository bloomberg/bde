// baltzo_localtimevalidity.t.cpp                                     -*-C++-*-

#include <baltzo_localtimevalidity.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_platform.h>

#include <bsl_cstdlib.h>       // `atoi`
#include <bsl_cstring.h>       // `strcmp`, `memcmp`, `memcpy`
#include <bsl_ios.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test implements a single enumeration having sequential
// enumerator values that start from 0.
//
// We will therefore follow our standard 3-step approach to testing enumeration
// types, with certain test cases omitted:
//  - [ 4] -- BDEX streaming is not (yet) implemented for this type.
//
// Global Concerns:
//  - No methods or free operators allocate memory.
//
// Global Assumptions:
//  - All CLASS METHODS and the `<<` free operator are `const` thread-safe.
// ----------------------------------------------------------------------------
// TYPES
// [ 1] enum Enum { ... };
//
// CLASS METHODS
// [ 2] ostream& print(ostream& s, Enum val, int level = 0, int sPL = 4);
// [ 1] const char *toAscii(baltzo::LocalTimeValidity::Enum val);
//
// FREE OPERATORS
// [ 3] operator<<(ostream& s, baltzo::LocalTimeValidity::Enum val);
// ----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE

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
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // `P(X)` without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef baltzo::LocalTimeValidity::Enum Enum;
typedef baltzo::LocalTimeValidity       Obj;

// ============================================================================
//                       GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

const int NUM_ENUMERATORS = 3;

#define UNKNOWN_FORMAT "(* UNKNOWN *)"

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator defaultAllocator(veryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    bslma::TestAllocator globalAllocator(veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // 1. The usage example provided in the component header file must
        //    compile, link, and run as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, replace
        //    leading comment characters with spaces, replace `assert` with
        //    `ASSERT`, and insert `if (veryVerbose)` before all output
        //    operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// `baltzo::LocalTimeValidity` usage.
//
// First, we create a variable `value` of type
// `baltzo::LocalTimeValidity::Enum` and initialize it with the enumerator
// value `baltzo::LocalTimeValidity::e_VALID_AMBIGUOUS`:
// ```
   baltzo::LocalTimeValidity::Enum value =
                                  baltzo::LocalTimeValidity::e_VALID_AMBIGUOUS;
// ```
// Next, we store a pointer to its ASCII representation in a variable
// `asciiValue` of type `const char *`:
// ```
   const char *asciiValue = baltzo::LocalTimeValidity::toAscii(value);
   ASSERT(0 == bsl::strcmp(asciiValue, "VALID_AMBIGUOUS"));
// ```
// Finally, we print `value` to `bsl::cout`.
// ```
if (veryVerbose)
   bsl::cout << value << bsl::endl;
// ```
// This statement produces the following output on `stdout`:
// ```
// VALID_AMBIGUOUS
// ```

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMING
        //   `bdex` streaming is not yet implemented for this enumeration.
        //
        // Concerns:
        //   Not applicable.
        //
        // Plan:
        //   Not applicable.
        //
        // Testing:
        //   Not applicable.
        // --------------------------------------------------------------------

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (`<<`) OPERATOR
        //
        // Concerns:
        // 1. The `<<` operator writes the output to the specified stream.
        //
        // 2. The `<<` operator writes the string representation of each
        //    enumerator in the intended format.
        //
        // 3. The `<<` operator writes a distinguished string when passed an
        //    out-of-band value.
        //
        // 4. The output produced by `stream << value` is the same as that
        //    produced by `Obj::print(stream, value, 0, -1)`.
        //
        // 5. There is no output when the stream is invalid.
        //
        // 6. The `<<` operator has the expected signature.
        //
        // 7. The `<<` operator returns the supplied `ostream`.
        //
        // Plan:
        // 1. Verify that the `<<` operator produces the expected results for
        //    each enumerator.  (C-1..2)
        //
        // 2. Verify that the `<<` operator writes a distinguished string when
        //    passed an out-of-band value.  (C-3)
        //
        // 3. Verify that `stream << value` writes the same output as
        //    `Obj::print(stream, value, 0, -1)`.  (C-4)
        //
        // 4. Verify that the address of the returned `stream` is the same as
        //    the supplied `stream`.  (C-7)
        //
        // 5. Verify that there is no output when the stream is invalid.  (C-5)
        //
        // 6. Take the address of the `<<` (free) operator and use the result
        //    to initialize a variable of the appropriate type.  (C-6)
        //
        // Testing:
        //   operator<<(ostream& s, baltzo::LocalTimeValidity::Enum val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING OUTPUT (`<<`) OPERATOR" << endl
                                  << "==============================" << endl;

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

        static const struct {
            int         d_lineNum;  // source line number
            Enum        d_value;    // enumerator value
            const char *d_exp;      // expected result
        } DATA[] = {
            //line       enumerator value              expected result
            //----    ----------------------          -----------------
            { L_,     Obj::e_VALID_UNIQUE,            "VALID_UNIQUE"     },
            { L_,     Obj::e_VALID_AMBIGUOUS,         "VALID_AMBIGUOUS"  },
            { L_,     Obj::e_INVALID,                 "INVALID"          },

#ifndef BDE_BUILD_TARGET_UBSAN
            { L_,     (Enum)NUM_ENUMERATORS,          UNKNOWN_FORMAT     },
            { L_,     (Enum)-1,                       UNKNOWN_FORMAT     },
            { L_,     (Enum)-5,                       UNKNOWN_FORMAT     },
            { L_,     (Enum)99,                       UNKNOWN_FORMAT     },
#endif
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

        if (verbose) cout << "\nTesting `<<` operator." << endl;

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ASSERT(Obj:: e_VALID_UNIQUE    == Obj::e_VALID_UNIQUE);
        ASSERT(Obj:: e_VALID_AMBIGUOUS == Obj::e_VALID_AMBIGUOUS);
        ASSERT(Obj:: e_INVALID         == Obj::e_INVALID);
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE  = DATA[ti].d_lineNum;
            const Enum   VALUE = DATA[ti].d_value;
            const string EXP   = DATA[ti].d_exp;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }
            if (veryVerbose) cout << "EXPECTED FORMAT: " << EXP << endl;

            bslma::TestAllocator scratch("scratch", veryVeryVerbose);
            ostringstream os(&scratch);

            LOOP_ASSERT(LINE, &os == &(os << VALUE));

            if (veryVerbose) cout << "  ACTUAL FORMAT: " << os.str() << endl;

            LOOP4_ASSERT(LINE, ti, EXP, os.str(), EXP == os.str());

            if (veryVerbose) cout << "\tVerify that the output matches that of"
                                  << " print with level = 0 and spl = -1."
                                  << endl;
            {
                ostringstream out(&scratch);

                Obj::print(out, VALUE, 0, -1);

                LOOP4_ASSERT(LINE, ti, os.str(), out.str(),
                             os.str() == out.str());
            }
        }

        if (verbose) cout << "\tNothing is written to a bad stream." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE  = DATA[ti].d_lineNum;
            const Enum   VALUE = DATA[ti].d_value;
            const string EXP   = DATA[ti].d_exp;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }

            bslma::TestAllocator scratch("scratch", veryVeryVerbose);
            ostringstream os(&scratch);   os.setstate(ios::failbit);

            LOOP_ASSERT(LINE, &os == &(os << VALUE));

            LOOP3_ASSERT(LINE, ti, os.str(), os.str().empty());
        }

        if (verbose) cout << "\nVerify `<<` operator signature." << endl;

        {
            using namespace baltzo;
            typedef bsl::ostream& (*FuncPtr)(bsl::ostream&, Enum);

            const FuncPtr FP = &operator<<;

            (void)FP;   // quash potential compiler warning
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `print`
        //
        // Concerns:
        // 1. The `print` method writes the output to the specified stream.
        //
        // 2. The `print` method writes the string representation of each
        //    enumerator in the intended format taking the `level` and
        //    `spacesPerLevel` parameter values into account.
        //
        // 3. The optional `level` and `spacesPerLevel` parameters have the
        //    correct default values.
        //
        // 4. The `print` method writes a distinguished string when passed an
        //    out-of-band value.
        //
        // 5. There is no output when the stream is invalid.
        //
        // 6. The `print` method has the expected signature.
        //
        // 7. The `print` method returns the supplied `ostream`.
        //
        // Plan:
        // 1. Verify that the `print` method produces the expected results for
        //    each enumerator.  (C-1..3)
        //
        // 2. Verify that the `print` method writes a distinguished string when
        //    passed an out-of-band value.  (C-4)
        //
        // 3. Verify that the address of the returned `stream` is the same as
        //    the supplied `stream`.  (C-7)
        //
        // 4. Verify that there is no output when the stream is invalid.  (C-5)
        //
        // 5. Take the address of the `print` (class) method and use the
        //    result to initialize a variable of the appropriate type.  (C-6)
        //
        // Testing:
        //   ostream& print(ostream& s, Enum val, int level = 0, int sPL = 4);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING `print`" << endl
                                  << "===============" << endl;

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

        static const struct {
            int         d_lineNum;  // source line number
            int         d_level;    // level
            int         d_spl;      // spaces per level
            Enum        d_value;    // enumerator value
            const char *d_exp;      // expected result
        } DATA[] = {
#define NL "\n"
            //line level spl    enumerator value         expected result
            //---- ----- --- ----------------------     --------------------
            { L_,    0,   4, Obj::e_VALID_UNIQUE,       "VALID_UNIQUE" NL    },
            { L_,    0,   4, Obj::e_VALID_AMBIGUOUS,    "VALID_AMBIGUOUS" NL },
            { L_,    0,   4, Obj::e_INVALID,            "INVALID" NL         },

#ifndef BDE_BUILD_TARGET_UBSAN
            { L_,    0,   4, (Enum)NUM_ENUMERATORS,     UNKNOWN_FORMAT NL    },
            { L_,    0,   4, (Enum)-1,                  UNKNOWN_FORMAT NL    },
            { L_,    0,   4, (Enum)-5,                  UNKNOWN_FORMAT NL    },
            { L_,    0,   4, (Enum)99,                  UNKNOWN_FORMAT NL    },
#endif

            { L_,    0,  -1, Obj::e_VALID_UNIQUE,       "VALID_UNIQUE"       },
            { L_,    0,   0, Obj::e_VALID_UNIQUE,       "VALID_UNIQUE" NL    },
            { L_,    0,   2, Obj::e_VALID_UNIQUE,       "VALID_UNIQUE" NL    },
            { L_,    1,   1, Obj::e_VALID_UNIQUE,       " VALID_UNIQUE" NL   },
            { L_,    1,   2, Obj::e_VALID_UNIQUE,       "  VALID_UNIQUE" NL  },
            { L_,   -1,   2, Obj::e_VALID_UNIQUE,       "VALID_UNIQUE" NL    },
            { L_,   -2,   1, Obj::e_VALID_UNIQUE,       "VALID_UNIQUE" NL    },
            { L_,    2,   1, Obj::e_VALID_UNIQUE,       "  VALID_UNIQUE" NL  },
            { L_,    1,   3, Obj::e_VALID_UNIQUE,       "   VALID_UNIQUE" NL },
#undef NL
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

        if (verbose) cout << "\nTesting `print`." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE  = DATA[ti].d_lineNum;
            const int    LEVEL = DATA[ti].d_level;
            const int    SPL   = DATA[ti].d_spl;
            const Enum   VALUE = DATA[ti].d_value;
            const string EXP   = DATA[ti].d_exp;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }
            if (veryVerbose) cout << "EXPECTED FORMAT: " << EXP << endl;

            bslma::TestAllocator scratch("scratch", veryVeryVerbose);
            ostringstream os(&scratch);

            LOOP_ASSERT(LINE, &os == &Obj::print(os, VALUE, LEVEL, SPL));
            LOOP4_ASSERT(LINE, ti, EXP, os.str(), EXP == os.str());

            if (veryVerbose) cout << "  ACTUAL FORMAT: " << os.str() << endl;

            if (0 == LEVEL && 4 == SPL) {
                if (veryVerbose)
                    cout << "\tRepeat for `print` default arguments." << endl;

                bslma::TestAllocator scratch("scratch", veryVeryVerbose);
                ostringstream os(&scratch);

                LOOP_ASSERT(LINE, &os == &Obj::print(os, VALUE));
                LOOP4_ASSERT(LINE, ti, EXP, os.str(), EXP == os.str());

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
            const string EXP   = DATA[ti].d_exp;

            if (veryVerbose) { T_; P_(ti); P_(LEVEL); P_(SPL); P(VALUE); }

            bslma::TestAllocator scratch("scratch", veryVeryVerbose);
            ostringstream os(&scratch);   os.setstate(ios::failbit);

            LOOP_ASSERT(LINE, &os == &Obj::print(os, VALUE));
            LOOP3_ASSERT(LINE, ti, os.str(), os.str().empty());
        }

        if (verbose) cout << "\nVerify `print` signature." << endl;

        {
            typedef bsl::ostream& (*FuncPtr)(bsl::ostream&, Enum, int, int);

            const FuncPtr FP = &Obj::print;

            (void)FP;   // quash potential compiler warning
        }
      } break;
      case 1: {
        // -------------------------------------------------------------------
        // TESTING `enum` AND `toAscii`
        //
        // Concerns:
        // 1. The enumerator values are sequential, starting from 0.
        //
        // 2. The `toAscii` method returns the expected string representation
        //    for each enumerator.
        //
        // 3. The `toAscii` method returns a distinguished string when passed
        //    an out-of-band value.
        //
        // 4. The string returned by `toAscii` is non-modifiable.
        //
        // 5. The `toAscii` method has the expected signature.
        //
        // Plan:
        // 1. Verify that the enumerator values are sequential, starting from
        //    0.  (C-1)
        //
        // 2. Verify that the `toAscii` method returns the expected string
        //    representation for each enumerator.  (C-2)
        //
        // 3. Verify that the `toAscii` method returns a distinguished string
        //    when passed an out-of-band value.  (C-3)
        //
        // 4. Take the address of the `toAscii` (class) method and use the
        //    result to initialize a variable of the appropriate type.
        //    (C-4..5)
        //
        // Testing:
        //   enum Enum { ... };
        //   const char *toAscii(baltzo::LocalTimeValidity::Enum val);
        // -------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING `enum` AND `toAscii`" << endl
                                  << "============================" << endl;

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

        static const struct {
            int         d_lineNum;  // source line number
            Enum        d_value;    // enumerator value
            const char *d_exp;      // expected result
        } DATA[] = {
            // line         enumerator value            expected result
            // ----    ---------------------------     -----------------
            {  L_,     Obj::e_VALID_UNIQUE,            "VALID_UNIQUE"     },
            {  L_,     Obj::e_VALID_AMBIGUOUS,         "VALID_AMBIGUOUS"  },
            {  L_,     Obj::e_INVALID,                 "INVALID"          },

#ifndef BDE_BUILD_TARGET_UBSAN
            {  L_,     (Enum)NUM_ENUMERATORS,          UNKNOWN_FORMAT     },
            {  L_,     (Enum)-1,                       UNKNOWN_FORMAT     },
            {  L_,     (Enum)-5,                       UNKNOWN_FORMAT     },
            {  L_,     (Enum)99,                       UNKNOWN_FORMAT     }
#endif
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

        if (verbose) cout << "\nVerify enumerator values are sequential."
                          << endl;

        for (int ti = 0; ti < NUM_ENUMERATORS; ++ti) {
            const Enum VALUE = DATA[ti].d_value;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }

            LOOP_ASSERT(ti, ti == VALUE);
        }

        if (verbose) cout << "\nTesting `toAscii`." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;
            const Enum  VALUE = DATA[ti].d_value;
            const char *EXP   = DATA[ti].d_exp;

            const char *result = Obj::toAscii(VALUE);

            if (veryVerbose) { T_; P_(ti); P_(VALUE); P_(EXP); P(result); }

            LOOP2_ASSERT(LINE, ti, strlen(EXP) == strlen(result));
            LOOP2_ASSERT(LINE, ti,           0 == strcmp(EXP, result));
        }

        if (verbose) cout << "\nVerify `toAscii` signature." << endl;

        {
            typedef const char *(*FuncPtr)(Enum);

            const FuncPtr FP = &Obj::toAscii;

            (void)FP;   // quash potential compiler warning
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
