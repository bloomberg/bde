// bslx_typecode.t.cpp                                                -*-C++-*-

#include <bslx_typecode.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bslx;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test implements a single enumeration having sequential
// enumerator values.
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
// [ 3] ostream& print(ostream& s, Enum val, int level = 0, int sPL = 4);
// [ 1] const char *toAscii(TypeCode::Enum val);
//
// FREE OPERATORS
// [ 2] operator<<(ostream& s, TypeCode::Enum val);
// ----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
// ----------------------------------------------------------------------------

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
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef TypeCode::Enum Enum;
typedef TypeCode       Obj;

// ============================================================================
//                       GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

const int NUM_ENUMERATORS = 19;

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
    bslma::Default::setDefaultAllocatorRaw(&defaultAllocator);

    bslma::TestAllocator globalAllocator(veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
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
// The following snippets of code provide a simple illustration of using
// 'bslx::TypeCode'.
//
// First, we create a variable 'value' of type 'bslx::TypeCode::Enum' and
// initialize it with the enumerator value 'bslx::TypeCode::e_INT32':
//..
    bslx::TypeCode::Enum value = bslx::TypeCode::e_INT32;
//..
// Now, we store the address of its ASCII representation in a pointer variable,
// 'asciiValue', of type 'const char *':
//..
    const char *asciiValue = bslx::TypeCode::toAscii(value);
    ASSERT(0 == bsl::strcmp(asciiValue, "INT32"));
//..
// Finally, we print 'value' to 'bsl::cout':
//..
if (veryVerbose)
    bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  INT32
//..

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'print'
        //
        // Concerns:
        //: 1 The 'print' method writes the output to the specified stream.
        //:
        //: 2 The 'print' method writes the string representation of each
        //:   enumerator in the intended format.
        //:
        //: 3 The 'print' method writes a distinguished string when passed an
        //:   out-of-band value.
        //:
        //: 4 There is no output when the stream is invalid.
        //:
        //: 5 The 'print' method has the expected signature.
        //
        // Plan:
        //: 1 Verify that the 'print' method produces the expected results for
        //:   each enumerator.  (C-1..2)
        //:
        //: 2 Verify that the 'print' method writes a distinguished string when
        //:   passed an out-of-band value.  (C-3)
        //:
        //: 3 Verify that there is no output when the stream is invalid.  (C-4)
        //:
        //: 4 Take the address of the 'print' (class) method and use the
        //:   result to initialize a variable of the appropriate type.  (C-5)
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
            const char *d_exp;      // expected result
        } DATA[] = {
#define NL "\n"
            //line  level  spl    enumerator value        expected result
            //----  -----  ---  ----------------------    -----------------
            { L_,     0,    4,  Obj::e_INT8,              "INT8" NL          },
            { L_,     0,    4,  Obj::e_UINT8,             "UINT8" NL         },
            { L_,     0,    4,  Obj::e_INT16,             "INT16" NL         },
            { L_,     0,    4,  Obj::e_UINT16,            "UINT16" NL        },
            { L_,     0,    4,  Obj::e_INT24,             "INT24" NL         },
            { L_,     0,    4,  Obj::e_UINT24,            "UINT24" NL        },
            { L_,     0,    4,  Obj::e_INT32,             "INT32" NL         },
            { L_,     0,    4,  Obj::e_UINT32,            "UINT32" NL        },
            { L_,     0,    4,  Obj::e_INT40,             "INT40" NL         },
            { L_,     0,    4,  Obj::e_UINT40,            "UINT40" NL        },
            { L_,     0,    4,  Obj::e_INT48,             "INT48" NL         },
            { L_,     0,    4,  Obj::e_UINT48,            "UINT48" NL        },
            { L_,     0,    4,  Obj::e_INT56,             "INT56" NL         },
            { L_,     0,    4,  Obj::e_UINT56,            "UINT56" NL        },
            { L_,     0,    4,  Obj::e_INT64,             "INT64" NL         },
            { L_,     0,    4,  Obj::e_UINT64,            "UINT64" NL        },
            { L_,     0,    4,  Obj::e_FLOAT32,           "FLOAT32" NL       },
            { L_,     0,    4,  Obj::e_FLOAT64,           "FLOAT64" NL       },
            { L_,     0,    4,  Obj::e_INVALID,           "INVALID" NL       },

            { L_,     0,    4,  (Enum)NUM_ENUMERATORS,    UNKNOWN_FORMAT NL  },
            { L_,     0,    4,  (Enum)99,                 UNKNOWN_FORMAT NL  },

            { L_,     0,   -1,  Obj::e_INT32,             "INT32"            },
            { L_,     0,    0,  Obj::e_INT32,             "INT32" NL         },
            { L_,     0,    2,  Obj::e_INT32,             "INT32" NL         },
            { L_,     1,    1,  Obj::e_INT32,             " INT32" NL        },
            { L_,     1,    2,  Obj::e_INT32,             "  INT32" NL       },
            { L_,    -1,    2,  Obj::e_INT32,             "INT32" NL         },
            { L_,    -2,    1,  Obj::e_INT32,             "INT32" NL         },
            { L_,     2,    1,  Obj::e_INT32,             "  INT32" NL       },
            { L_,     1,    3,  Obj::e_INT32,             "   INT32" NL      },

#undef NL
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        const int   SIZE = 128;         // big enough to hold output string
        const char  XX   = static_cast<char>(0xFF); // value of an unset 'char'

        char        mCtrl[SIZE];  memset(mCtrl, XX, SIZE);
        const char *CTRL = mCtrl;

        if (verbose) cout << "\nTesting 'print'." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;
            const int   LEVEL = DATA[ti].d_level;
            const int   SPL   = DATA[ti].d_spl;
            const Enum  VALUE = DATA[ti].d_value;
            const char *EXP   = DATA[ti].d_exp;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }
            if (veryVerbose) cout << "EXPECTED FORMAT: " << EXP << endl;

            bslma::TestAllocator allocator;

            ostringstream out(bsl::string(CTRL, SIZE, &allocator), &allocator);
            Obj::print(out, VALUE, LEVEL, SPL) << ends;

            bsl::string buffer(&allocator);
            {
                bslma::DefaultAllocatorGuard allocatorGuard(&allocator);

                buffer = out.str();
            }
            const char *RESULT = buffer.c_str();

            if (veryVerbose) cout << "  ACTUAL FORMAT: " << RESULT << endl;

            const int SZ = static_cast<int>(strlen(EXP)) + 1;
            LOOP2_ASSERT(LINE, ti, XX == RESULT[SIZE - 1]);
                                                          // Check for overrun.
            LOOP2_ASSERT(LINE, ti,  0 == memcmp(RESULT, EXP, SZ));
            LOOP2_ASSERT(LINE, ti,  0 == memcmp(RESULT + SZ,
                                                CTRL + SZ,
                                                SIZE - SZ));

            if (0 == LEVEL && 4 == SPL) {
                if (veryVerbose)
                    cout << "\tRepeat for 'print' default arguments." << endl;

                bslma::TestAllocator allocator;

                ostringstream out(bsl::string(CTRL, SIZE, &allocator),
                                  &allocator);
                Obj::print(out, VALUE) << ends;

                bsl::string buffer(&allocator);
                {
                    bslma::DefaultAllocatorGuard allocatorGuard(&allocator);

                    buffer = out.str();
                }
                const char *RESULT = buffer.c_str();

                if (veryVerbose) cout << "  ACTUAL FORMAT: " << RESULT << endl;

                const int SZ = static_cast<int>(strlen(EXP)) + 1;
                LOOP2_ASSERT(LINE, ti, XX == RESULT[SIZE - 1]);
                                                          // Check for overrun.
                LOOP2_ASSERT(LINE, ti,  0 == memcmp(RESULT, EXP, SZ));
                LOOP2_ASSERT(LINE, ti,  0 == memcmp(RESULT + SZ,
                                                    CTRL + SZ,
                                                    SIZE - SZ));
            }
        }

        if (verbose) cout << "\tNothing is written to a bad stream." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;
            const int   LEVEL = DATA[ti].d_level;
            const int   SPL   = DATA[ti].d_spl;
            const Enum  VALUE = DATA[ti].d_value;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }

            bslma::TestAllocator allocator;

            ostringstream out(bsl::string(CTRL, SIZE, &allocator), &allocator);
            out.setstate(ios::badbit);
            Obj::print(out, VALUE, LEVEL, SPL);

            bsl::string buffer(&allocator);
            {
                bslma::DefaultAllocatorGuard allocatorGuard(&allocator);

                buffer = out.str();
            }
            const char *RESULT = buffer.c_str();

            LOOP2_ASSERT(LINE, ti, 0 == memcmp(RESULT, CTRL, SIZE));
        }

        if (verbose) cout << "\nVerify 'print' signature." << endl;

        {
            typedef bsl::ostream& (*FuncPtr)(bsl::ostream&, Enum, int, int);

            const FuncPtr FP = &Obj::print;  (void)FP;
        }

      } break;
      case 2: {
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
        //: 4 Verify that there is no output when the stream is invalid.  (C-5)
        //:
        //: 5 Take the address of the '<<' (free) operator and use the result
        //:   to initialize a variable of the appropriate type.  (C-6)
        //
        // Testing:
        //   operator<<(ostream& s, TypeCode::Enum val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING OUTPUT ('<<') OPERATOR" << endl
                                  << "==============================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            Enum        d_value;    // enumerator value
            const char *d_exp;      // expected result
        } DATA[] = {
            // line         enumerator value        expected result
            // ----    -----------------------      -----------------
            {  L_,     Obj::e_INT8,                 "INT8"            },
            {  L_,     Obj::e_UINT8,                "UINT8"           },
            {  L_,     Obj::e_INT16,                "INT16"           },
            {  L_,     Obj::e_UINT16,               "UINT16"          },
            {  L_,     Obj::e_INT24,                "INT24"           },
            {  L_,     Obj::e_UINT24,               "UINT24"          },
            {  L_,     Obj::e_INT32,                "INT32"           },
            {  L_,     Obj::e_UINT32,               "UINT32"          },
            {  L_,     Obj::e_INT40,                "INT40"           },
            {  L_,     Obj::e_UINT40,               "UINT40"          },
            {  L_,     Obj::e_INT48,                "INT48"           },
            {  L_,     Obj::e_UINT48,               "UINT48"          },
            {  L_,     Obj::e_INT56,                "INT56"           },
            {  L_,     Obj::e_UINT56,               "UINT56"          },
            {  L_,     Obj::e_INT64,                "INT64"           },
            {  L_,     Obj::e_UINT64,               "UINT64"          },
            {  L_,     Obj::e_FLOAT32,              "FLOAT32"         },
            {  L_,     Obj::e_FLOAT64,              "FLOAT64"         },
            {  L_,     Obj::e_INVALID,              "INVALID"         },

            {  L_,     (Enum)NUM_ENUMERATORS,       UNKNOWN_FORMAT    },
            {  L_,     (Enum)99,                    UNKNOWN_FORMAT    }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        const int   SIZE = 128;         // big enough to hold output string
        const char  XX   = static_cast<char>(0xFF); // value of an unset 'char'

        char        mCtrl[SIZE];  memset(mCtrl, XX, SIZE);
        const char *CTRL = mCtrl;

        if (verbose) cout << "\nTesting '<<' operator." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;
            const Enum  VALUE = DATA[ti].d_value;
            const char *EXP   = DATA[ti].d_exp;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }
            if (veryVerbose) cout << "EXPECTED FORMAT: " << EXP << endl;

            bslma::TestAllocator allocator;

            ostringstream out(bsl::string(CTRL, SIZE, &allocator), &allocator);
            out << VALUE << ends;

            bsl::string buffer(&allocator);
            {
                bslma::DefaultAllocatorGuard allocatorGuard(&allocator);

                buffer = out.str();
            }
            const char *RESULT = buffer.c_str();

            if (veryVerbose) cout << "  ACTUAL FORMAT: " << RESULT << endl;

            const int SZ = static_cast<int>(strlen(EXP)) + 1;
            LOOP2_ASSERT(LINE, ti, SZ  < SIZE);           // Buffer is large
                                                          // enough.
            LOOP2_ASSERT(LINE, ti, XX == RESULT[SIZE - 1]);
                                                          // Check for overrun.
            LOOP2_ASSERT(LINE, ti,  0 == memcmp(RESULT, EXP, SZ));
            LOOP2_ASSERT(LINE, ti,  0 == memcmp(RESULT + SZ,
                                                CTRL + SZ,
                                                SIZE - SZ));
        }

        if (verbose) cout << "\tNothing is written to a bad stream." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;
            const Enum  VALUE = DATA[ti].d_value;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }

            bslma::TestAllocator allocator;

            ostringstream out(bsl::string(CTRL, SIZE, &allocator), &allocator);
            out.setstate(ios::badbit);
            out << VALUE << ends;

            bsl::string buffer(&allocator);
            {
                bslma::DefaultAllocatorGuard allocatorGuard(&allocator);

                buffer = out.str();
            }
            const char *RESULT = buffer.c_str();

            LOOP2_ASSERT(LINE, ti, 0 == memcmp(RESULT, CTRL, SIZE));
        }

        if (verbose) cout << "\nVerify '<<' operator signature." << endl;

        {
            typedef bsl::ostream& (*FuncPtr)(bsl::ostream&, Enum);

            const FuncPtr FP = &operator<<;  (void)FP;
        }

      } break;
      case 1: {
        // -------------------------------------------------------------------
        // TESTING 'enum' AND 'toAscii'
        //
        // Concerns:
        //: 1 The enumerator values are sequential, starting from 0.
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
        //
        // Plan:
        //: 1 Verify that the enumerator values are sequential, starting from
        //:   0.  (C-1)
        //:
        //: 2 Verify that the 'toAscii' method returns the expected string
        //:   representation for each enumerator.  (C-2)
        //:
        //: 3 Verify that the 'toAscii' method returns a distinguished string
        //:   when passed an out-of-band value.  (C-3)
        //:
        //: 4 Take the address of the 'toAscii' (class) method and use the
        //:   result to initialize a variable of the appropriate type.
        //:   (C-4, C-5)
        //
        // Testing:
        //   enum Enum { ... };
        //   const char *toAscii(TypeCode::Enum val);
        // -------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'enum' AND 'toAscii'" << endl
                                  << "============================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            Enum        d_value;    // enumerator value
            const char *d_exp;      // expected result
        } DATA[] = {
            // line         enumerator value        expected result
            // ----    -----------------------      -----------------
            {  L_,     Obj::e_INT8,                 "INT8"            },
            {  L_,     Obj::e_UINT8,                "UINT8"           },
            {  L_,     Obj::e_INT16,                "INT16"           },
            {  L_,     Obj::e_UINT16,               "UINT16"          },
            {  L_,     Obj::e_INT24,                "INT24"           },
            {  L_,     Obj::e_UINT24,               "UINT24"          },
            {  L_,     Obj::e_INT32,                "INT32"           },
            {  L_,     Obj::e_UINT32,               "UINT32"          },
            {  L_,     Obj::e_INT40,                "INT40"           },
            {  L_,     Obj::e_UINT40,               "UINT40"          },
            {  L_,     Obj::e_INT48,                "INT48"           },
            {  L_,     Obj::e_UINT48,               "UINT48"          },
            {  L_,     Obj::e_INT56,                "INT56"           },
            {  L_,     Obj::e_UINT56,               "UINT56"          },
            {  L_,     Obj::e_INT64,                "INT64"           },
            {  L_,     Obj::e_UINT64,               "UINT64"          },
            {  L_,     Obj::e_FLOAT32,              "FLOAT32"         },
            {  L_,     Obj::e_FLOAT64,              "FLOAT64"         },
            {  L_,     Obj::e_INVALID,              "INVALID"         },

            {  L_,     (Enum)NUM_ENUMERATORS,       UNKNOWN_FORMAT    },
            {  L_,     (Enum)99,                    UNKNOWN_FORMAT    }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nVerify enumerator values are sequential."
                          << endl;

        for (int ti = 0; ti < NUM_ENUMERATORS; ++ti) {
            const Enum VALUE = DATA[ti].d_value;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }

            LOOP_ASSERT(ti, ti + DATA[0].d_value == VALUE);
        }

        if (verbose) cout << "\nTesting 'toAscii'." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;
            const Enum  VALUE = DATA[ti].d_value;
            const char *EXP   = DATA[ti].d_exp;

            const char *result = Obj::toAscii(VALUE);

            if (veryVerbose) { T_; P_(ti); P_(VALUE); P_(EXP); P(result); }

            LOOP2_ASSERT(LINE, ti, strlen(EXP) == strlen(result));
            LOOP2_ASSERT(LINE, ti,           0 == strcmp(EXP, result));
        }

        if (verbose) cout << "\nVerify 'toAscii' signature." << endl;

        {
            typedef const char *(*FuncPtr)(Enum);

            const FuncPtr FP = &Obj::toAscii;  (void)FP;
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
// Copyright 2014 Bloomberg Finance L.P.
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
