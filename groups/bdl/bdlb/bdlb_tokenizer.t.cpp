// bdlb_tokenizer.t.cpp                                               -*-C++-*-
#include <bdlb_tokenizer.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bdls_testutil.h>

#include <bdlb_strtokenrefiter.h>         // for performance compare only

#include <bsl_string.h>

#include <bsl_cstdlib.h>                  // 'bsl::atoi()'
#include <bsl_cstring.h>                  // 'bsl::memcpy', 'bsl::strcmp'
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// Primary Manipulators:
//: o
//: o
//
/// Basic Accessors:
//: o
//: o
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2]
// [ 2] ~Tokenizer();
//
// MANIPULATORS
// [ 2] ;
//
// ACCESSORS
// [ 4]
// [ 4]
// [ 4]
// [12]
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3]
// [13] USAGE EXAMPLE

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

#define ASSERT       BDLS_TESTUTIL_ASSERT
#define ASSERTV      BDLS_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BDLS_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BDLS_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BDLS_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BDLS_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BDLS_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BDLS_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BDLS_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BDLS_TESTUTIL_LOOP6_ASSERT

#define Q            BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P            BDLS_TESTUTIL_P   // Print identifier and value.
#define P_           BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BDLS_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                     GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlb::Tokenizer    Obj;
typedef bslstl::StringRef  StringRef;

const char INPUT[] = {  "aaaaaaaaaaaaaaaaaaaaaaaaaaaa "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
};

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 13: {
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
        //:   leading comment characters and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING TokenIterator operator++
        //
        // Concerns:
        //: 1
        //
        // Plan:
        //: 1
        //
        // Testing:
        //   TokenIterator::operator++()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Secondary" << endl
                          << "====================" << endl;
        } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'begin' METHOD
        //
        // Concerns:
        //: 1
        //
        // Plan:
        //: 1
        //
        // Testing:
        //   Tokenizer::begin();
        //   Tokenizer::end();
        //   TokenIterator::operator*();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Secondary" << endl
                          << "====================" << endl;
        } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'reset' METHOD
        //
        // Concerns:
        //: 1
        //
        // Plan:
        //: 1
        //
        // Testing:
        //   Tokenizer::reset(const char *);
        //   Tokenizer::reset(StringRef&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'reset' METHOD" << endl
                          << "======================" << endl;
        } break;
      case 5: {
        // --------------------------------------------------------------------
        // SECONDARY CONSTRUCTOR
        //   Ensure that constructor with two parameters is "wired-up" and
        //   defaults properly.
        //
        // Concerns:
        //: 1
        //
        // Plan:
        //: 1
        //
        // Testing:
        //   Tokenizer(const char *, const StringRef&);
        //   Tokenizer(const StringRef&, const StringRef&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SECONDARY CONSTRUCTOR" << endl
                          << "====================" << endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Verify the basic accessors functionality.
        //
        // Concerns:
        //: 1
        //:
        //
        // Plan:
        //: 1
        //
        // Testing:
        //   Tokenizer::trailingDelimiter();
        //   Tokenizer::hasTrailingSoft();
        //   Tokenizer::hasPreviousSoft();
        //   Tokenizer::isTrailingHard();
        //   Tokenizer::isPreviousHard());
        //   Tokenizer::previousDelimiter();
        //   Tokenizer::token();
        //
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nTesting accessors." << endl;
        {
            if (verbose) cout << "\tTesting boolean conversion operator."
                      << endl;

            static const struct {
                int         d_line;         // line number
                const char *d_input;        // input string
                const char *d_soft;         // list of soft delimiters
                const char *d_hard;         // list of hard delimiters
                bool        d_isValidCtor;  // expected tokenizer validity
                                            // after object construction

                bool        d_isValidIncr;  // expected tokenizer validity
                                            // after increment
                } DATA[] = {
                    //LINE  INPUT   SOFT   HARD   VALID   VALID
                    //                            CTOR    INCR
                    //----  ------  ----   ----   -----   -----
                    { L_,   "",     ".",   "#",   false,  false },
                    { L_,   ".",    ".",   "#",   false,  false },
                    { L_,   "#",    ".",   "#",   true,   false },
                    { L_,   "T",    ".",   "#",   true,   false },
                    { L_,   "..",   ".",   "#",   false,  false },
                    { L_,   "##",   ".",   "#",   true,   true  },
                    { L_,   ".#",   ".",   "#",   true,   false },
                    { L_,   "#.",   ".",   "#",   true,   false },
                    { L_,   ".T",   ".",   "#",   true,   false },
                    { L_,   "#T",   ".",   "#",   true,   true  },
                    { L_,   "T.",   ".",   "#",   true,   false },
                    { L_,   "T#",   ".",   "#",   true,   false },
                    { L_,   "TT",   ".",   "#",   true,   false },
                    { L_,   "...",  ".",   "#",   false,  false },
                    { L_,   "..#",  ".",   "#",   true,   false },
                    { L_,   "..T",  ".",   "#",   true,   false },
                    { L_,   ".#.",  ".",   "#",   true,   false },
                    { L_,   ".##",  ".",   "#",   true,   true  },
                    { L_,   ".#T",  ".",   "#",   true,   true  },
                    { L_,   "#..",  ".",   "#",   true,   false },
                    { L_,   "#.#",  ".",   "#",   true,   true  },
                    { L_,   "#.T",  ".",   "#",   true,   true  },
                    { L_,   "T..",  ".",   "#",   true,   false },
                    { L_,   "T.#",  ".",   "#",   true,   false },
                    { L_,   "T.T",  ".",   "#",   true,   true  },
                    { L_,   "T#.",  ".",   "#",   true,   false },
                    { L_,   "T##",  ".",   "#",   true,   true  },
                    { L_,   "T#T",  ".",   "#",   true,   true  },
                    { L_,   "TT.",  ".",   "#",   true,   false },
                    { L_,   "TT#",  ".",   "#",   true,   false },
                    { L_,   "TTT",  ".",   "#",   true,   false },
                    // Extended cases.
                    //LINE  INPUT   SOFT   HARD   VALID   VALID
                    //                            CTOR    INCR
                    //----  ------  ----   ----   -----   -----
                    { L_,   ".#.#", ".",   "#",   true,   true  },
                    { L_,   ".#.T", ".",   "#",   true,   true  },
                    { L_,   "#.#.", ".",   "#",   true,   true  },
                    { L_,   "T.##", ".",   "#",   true,   true  },
                    { L_,   "T.#T", ".",   "#",   true,   true  },
                    { L_,   "T#.#", ".",   "#",   true,   true  },
                    { L_,   "T#.T", ".",   "#",   true,   true  },
                };   // end table DATA
            enum { DATA_LEN = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < DATA_LEN; ++i) {
                const int        LINE       = DATA[i].d_line;
                const char      *INPUT      = DATA[i].d_input;
                const StringRef  SOFT       = DATA[i].d_soft
                                              ? StringRef(DATA[i].d_soft)
                                              : StringRef();
                const StringRef  HARD       = DATA[i].d_hard
                                              ? StringRef(DATA[i].d_hard)
                                              : StringRef();
                bool             VALID_CTOR = DATA[i].d_isValidCtor;
                bool             VALID_INCR = DATA[i].d_isValidIncr;

                Obj        mT(INPUT, SOFT, HARD);
                const Obj& T = mT;

                ASSERTV(LINE, T, VALID_CTOR == (!!T));

                ++mT;

                ASSERTV(LINE, T, VALID_INCR == (!!T));
            }
        }

        {
            if (verbose) cout << "\tTesting accessors, returning StringRefs."
                              << endl;

            static const struct {
                int         d_line;          // line number
                const char *d_input;         // input string
                const char *d_soft;          // list of soft delimiters
                const char *d_hard;          // list of hard delimiters
                // expected values after construction
                const char *d_tokenCtor;     // expected token
                const char *d_delimCtor;     // expected current delimiter
                const char *d_prevDelimCtor; // expected previous delimiter
                // expected values after increment
                const char *d_tokenIncr;     // expected token
                const char *d_delimIncr;     // expected current delimiter
                const char *d_prevDelimIncr; // expected previous delimiter
            } DATA[] = {
             //LINE INPUT   SOFT  HARD  TOKEN  DELIM PREV   TOKEN DELIM PREV
             //                         CTOR   CTOR  DELIM  INCR  INCR  DELIM
             //                                      CTOR               INCR
             //---- ------  ----  ----  ------ ----- -----  ----- ----- -----
             { L_,  "",     ".",  "#",  "",    "",   "",    "",   "",   ""   },
             { L_,  ".",    ".",  "#",  "",    "",   ".",   "",   "",   ""   },
             { L_,  "#",    ".",  "#",  "",    "#",  "",    "",   "",   "#"  },
             { L_,  "T",    ".",  "#",  "T",   "",   "",    "",   "",   ""   },
             { L_,  "..",   ".",  "#",  "",    "",   "..",  "",   "",   ""   },
             { L_,  "##",   ".",  "#",  "",    "#",  "",    "",   "#",  "#"  },
             { L_,  ".#",   ".",  "#",  "",    "#",  ".",   "",   "",   "#"  },
             { L_,  "#.",   ".",  "#",  "",    "#.", "",    "",   "",   "#." },
             { L_,  ".T",   ".",  "#",  "T",   "",   ".",   "",   "",   ""   },
             { L_,  "#T",   ".",  "#",  "",    "#",  "",    "T",  "",   "#"  },
             { L_,  "T.",   ".",  "#",  "T",   ".",  "",    "",   "",   "."  },
             { L_,  "T#",   ".",  "#",  "T",   "#",  "",    "",   "",   "#"  },
             { L_,  "TT",   ".",  "#",  "TT",  "",   "",    "",   "",   ""   },
             { L_,  "...",  ".",  "#",  "",    "",   "...", "",   "",   ""   },
             { L_,  "..#",  ".",  "#",  "",    "#",  "..",  "",   "",   "#"  },
             { L_,  "..T",  ".",  "#",  "T",   "",   "..",  "",   "",   ""   },
             { L_,  ".#.",  ".",  "#",  "",    "#.", ".",   "",   "",   "#." },
             { L_,  ".##",  ".",  "#",  "",    "#",  ".",   "",   "#",  "#"  },
             { L_,  ".#T",  ".",  "#",  "",    "#",  ".",   "T",  "",   "#"  },
             { L_,  "#..",  ".",  "#",  "",    "#..","",    "",   "",   "#.."},
             { L_,  "#.#",  ".",  "#",  "",    "#.", "",    "",   "#",  "#." },
             { L_,  "#.T",  ".",  "#",  "",    "#.", "",    "T",  "",   "#." },
             { L_,  "T..",  ".",  "#",  "T",   "..", "",    "",   "",   ".." },
             { L_,  "T.#",  ".",  "#",  "T",   ".#", "",    "",   "",   ".#" },
             { L_,  "T.T",  ".",  "#",  "T",   ".",  "",    "T",  "",   "."  },
             { L_,  "T#.",  ".",  "#",  "T",   "#.", "",    "",   "",   "#." },
             { L_,  "T##",  ".",  "#",  "T",   "#",  "",    "",   "#",  "#"  },
             { L_,  "T#T",  ".",  "#",  "T",   "#",  "",    "T",  "",   "#"  },
             { L_,  "TT.",  ".",  "#",  "TT",  ".",  "",    "",   "",   "."  },
             { L_,  "TT#",  ".",  "#",  "TT",  "#",  "",    "",   "",   "#"  },
             { L_,  "TTT",  ".",  "#",  "TTT", "",   "",    "",   "",   ""   },
             // Extended cases.
             //LINE INPUT   SOFT  HARD  TOKEN  DELIM PREV   TOKEN DELIM PREV
             //                         CTOR   CTOR  DELIM  INCR  INCR  DELIM
             //                                      CTOR               INCR
             //---- ------  ----  ----  ------ ----- -----  ----- ----- -----
             { L_,  ".#.#", ".",  "#",  "",    "#.", ".",   "",   "#",  "#." },
             { L_,  ".#.T", ".",  "#",  "",    "#.", ".",   "T",  "",   "#." },
             { L_,  "#.#.", ".",  "#",  "",    "#.", "",    "",   "#.", "#." },
             { L_,  "T.##", ".",  "#",  "T",   ".#", "",    "",   "#",  ".#" },
             { L_,  "T.#T", ".",  "#",  "T",   ".#", "",    "T",  "",   ".#" },
             { L_,  "T#.#", ".",  "#",  "T",   "#.", "",    "",   "#",  "#." },
             { L_,  "T#.T", ".",  "#",  "T",   "#.", "",    "T",  "",   "#." },
            };   // end table DATA

            enum { DATA_LEN = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < DATA_LEN; ++i) {
                const int        LINE            = DATA[i].d_line;
                const char      *INPUT           = DATA[i].d_input;
                const StringRef  SOFT            = StringRef(DATA[i].d_soft);
                const StringRef  HARD            = StringRef(DATA[i].d_hard);
                const StringRef  TOKEN_CTOR      = DATA[i].d_tokenCtor;
                const StringRef  DELIM_CTOR      = DATA[i].d_delimCtor;
                const StringRef  PREV_DELIM_CTOR = DATA[i].d_prevDelimCtor;
                const StringRef  TOKEN_INCR      = DATA[i].d_tokenIncr;
                const StringRef  DELIM_INCR      = DATA[i].d_delimIncr;
                const StringRef  PREV_DELIM_INCR = DATA[i].d_prevDelimIncr;

                Obj        mT(INPUT, SOFT, HARD);
                const Obj& T = mT;

                ASSERTV(LINE, DELIM_CTOR      == T.trailingDelimiter());
                ASSERTV(LINE, PREV_DELIM_CTOR == T.previousDelimiter());
                ASSERTV(LINE, TOKEN_CTOR      == T.token());

                ++mT;

                ASSERTV(LINE, DELIM_INCR      == T.trailingDelimiter());
                ASSERTV(LINE, PREV_DELIM_INCR == T.previousDelimiter());
                ASSERTV(LINE, TOKEN_INCR      == T.token());
            }
        }

        {
            if (verbose) cout <<
                           "\tTesting accessors, returning previous delimiters"
                              << endl;

            static const struct {
                int         d_line;             // line number
                const char *d_input;            // input string
                const char *d_soft;             // list of soft delimiters
                const char *d_hard;             // list of hard delimiters
                bool        d_hasPrevSoftCtor;  // soft character presence
                                                // after construction

                bool        d_isPrevHardCtor;   // hard character presence
                                                // after construction

                bool        d_hasPrevSoftIncr;  // soft character presence
                                                // after increment

                bool        d_isPrevHardIncr;   // hard character presence
                                                // after increment
            } DATA[] = {
              //LINE  INPUT   SOFT   HARD  HAS_PREV IS_PREV  HAS_PREV IS_PREV
              //                           SOFT     HARD     SOFT     HARD
              //                           CTOR     CTOR     INCR     INCR
              //----  ------  ----   ----  -------- -------  -------- -------
              { L_,   "",     ".",   "#",  false,   false,   false,   false  },
              { L_,   ".",    ".",   "#",  true,    false,   false,   false  },
              { L_,   "#",    ".",   "#",  false,   false,   false,   true   },
              { L_,   "T",    ".",   "#",  false,   false,   false,   false  },
              { L_,   "..",   ".",   "#",  true,    false,   false,   false  },
              { L_,   "##",   ".",   "#",  false,   false,   false,   true   },
              { L_,   ".#",   ".",   "#",  true,    false,   false,   true   },
              { L_,   "#.",   ".",   "#",  false,   false,   true,    true   },
              { L_,   ".T",   ".",   "#",  true,    false,   false,   false  },
              { L_,   "#T",   ".",   "#",  false,   false,   false,   true   },
              { L_,   "T.",   ".",   "#",  false,   false,   true,    false  },
              { L_,   "T#",   ".",   "#",  false,   false,   false,   true   },
              { L_,   "TT",   ".",   "#",  false,   false,   false,   false  },
              { L_,   "...",  ".",   "#",  true,    false,   false,   false  },
              { L_,   "..#",  ".",   "#",  true,    false,   false,   true   },
              { L_,   "..T",  ".",   "#",  true,    false,   false,   false  },
              { L_,   ".#.",  ".",   "#",  true,    false,   true,    true   },
              { L_,   ".##",  ".",   "#",  true,    false,   false,   true   },
              { L_,   ".#T",  ".",   "#",  true,    false,   false,   true   },
              { L_,   "#..",  ".",   "#",  false,   false,   true,    true   },
              { L_,   "#.#",  ".",   "#",  false,   false,   true,    true   },
              { L_,   "#.T",  ".",   "#",  false,   false,   true,    true   },
              { L_,   "T..",  ".",   "#",  false,   false,   true,    false  },
              { L_,   "T.#",  ".",   "#",  false,   false,   true,    true   },
              { L_,   "T.T",  ".",   "#",  false,   false,   true,    false  },
              { L_,   "T#.",  ".",   "#",  false,   false,   true,    true   },
              { L_,   "T##",  ".",   "#",  false,   false,   false,   true   },
              { L_,   "T#T",  ".",   "#",  false,   false,   false,   true   },
              { L_,   "TT.",  ".",   "#",  false,   false,   true,    false  },
              { L_,   "TT#",  ".",   "#",  false,   false,   false,   true   },
              { L_,   "TTT",  ".",   "#",  false,   false,   false,   false  },
              // Extended cases.
              //LINE  INPUT   SOFT   HARD  HAS_PREV IS_PREV  HAS_PREV IS_PREV
              //                           SOFT     HARD     SOFT     HARD
              //                           CTOR     CTOR     INCR     INCR
              //----  ------  ----   ----  -------- -------  -------- -------
              { L_,   ".#.#", ".",   "#",  true,    false,   true,    true   },
              { L_,   ".#.T", ".",   "#",  true,    false,   true,    true   },
              { L_,   "#.#.", ".",   "#",  false,   false,   true,    true   },
              { L_,   "T.##", ".",   "#",  false,   false,   true,    true   },
              { L_,   "T.#T", ".",   "#",  false,   false,   true,    true   },
              { L_,   "T#.#", ".",   "#",  false,   false,   true,    true   },
              { L_,   "T#.T", ".",   "#",  false,   false,   true,    true   },
            };   // end table DATA

            enum { DATA_LEN = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < DATA_LEN; ++i) {
                const int        LINE          = DATA[i].d_line;
                const char      *INPUT         = DATA[i].d_input;
                const StringRef  SOFT          = DATA[i].d_soft
                                                 ? StringRef(DATA[i].d_soft)
                                                 : StringRef();
                const StringRef  HARD          = DATA[i].d_hard
                                                 ? StringRef(DATA[i].d_hard)
                                                 : StringRef();
                bool             HAS_PREV_SOFT_CTOR =
                                                     DATA[i].d_hasPrevSoftCtor;
                bool             IS_PREV_HARD_CTOR  = DATA[i].d_isPrevHardCtor;
                bool             HAS_PREV_SOFT_INCR =
                                                     DATA[i].d_hasPrevSoftIncr;
                bool             IS_PREV_HARD_INCR  = DATA[i].d_isPrevHardIncr;

                Obj        mT(INPUT, SOFT, HARD);
                const Obj& T = mT;

                ASSERTV(LINE, HAS_PREV_SOFT_CTOR == T.hasPreviousSoft());
                ASSERTV(LINE, IS_PREV_HARD_CTOR  == T.isPreviousHard());

                ++mT;

                ASSERTV(LINE, HAS_PREV_SOFT_INCR == T.hasPreviousSoft());
                ASSERTV(LINE, IS_PREV_HARD_INCR  == T.isPreviousHard());
            }
        }

        {
            if (verbose) cout <<
                           "\tTesting accessors, returning trailing delimiters"
                              << endl;

            static const struct {
                int         d_line;         // line number
                const char *d_input;        // input string
                const char *d_soft;         // list of soft delimiters
                const char *d_hard;         // list of hard delimiters
                bool        d_hasSoftCtor;  // soft character presence
                                            // after construction

                bool        d_isHardCtor;   // hard character presence
                                            // after construction

                bool        d_hasSoftIncr;  // soft character presence
                                            // after increment

                bool        d_isHardIncr;   // hard character presence
                                           // after increment
            } DATA[] = {
                    //LINE  INPUT   SOFT  HARD  HAS    IS_HARD HAS    IS_HARD
                    //                          SOFT   CTOR    SOFT   INCR
                    //                          CTOR           INCR
                    //----  ------  ----  ----  -----  ------- -----  -------
                    { L_,   "",     ".",  "#",  false, false,  false, false  },
                    { L_,   ".",    ".",  "#",  false, false,  false, false  },
                    { L_,   "#",    ".",  "#",  false, true,   false, false  },
                    { L_,   "T",    ".",  "#",  false, false,  false, false  },
                    { L_,   "..",   ".",  "#",  false, false,  false, false  },
                    { L_,   "##",   ".",  "#",  false, true,   false, true   },
                    { L_,   ".#",   ".",  "#",  false, true,   false, false  },
                    { L_,   "#.",   ".",  "#",  true,  true,   false, false  },
                    { L_,   ".T",   ".",  "#",  false, false,  false, false  },
                    { L_,   "#T",   ".",  "#",  false, true,   false, false  },
                    { L_,   "T.",   ".",  "#",  true,  false,  false, false  },
                    { L_,   "T#",   ".",  "#",  false, true,   false, false  },
                    { L_,   "TT",   ".",  "#",  false, false,  false, false  },
                    { L_,   "...",  ".",  "#",  false, false,  false, false  },
                    { L_,   "..#",  ".",  "#",  false, true,   false, false  },
                    { L_,   "..T",  ".",  "#",  false, false,  false, false  },
                    { L_,   ".#.",  ".",  "#",  true,  true,   false, false  },
                    { L_,   ".##",  ".",  "#",  false, true,   false, true   },
                    { L_,   ".#T",  ".",  "#",  false, true,   false, false  },
                    { L_,   "#..",  ".",  "#",  true,  true,   false, false  },
                    { L_,   "#.#",  ".",  "#",  true,  true,   false, true   },
                    { L_,   "#.T",  ".",  "#",  true,  true,   false, false  },
                    { L_,   "T..",  ".",  "#",  true,  false,  false, false  },
                    { L_,   "T.#",  ".",  "#",  true,  true,   false, false  },
                    { L_,   "T.T",  ".",  "#",  true,  false,  false, false  },
                    { L_,   "T#.",  ".",  "#",  true,  true,   false, false  },
                    { L_,   "T##",  ".",  "#",  false, true,   false, true   },
                    { L_,   "T#T",  ".",  "#",  false, true,   false, false  },
                    { L_,   "TT.",  ".",  "#",  true,  false,  false, false  },
                    { L_,   "TT#",  ".",  "#",  false, true,   false, false  },
                    { L_,   "TTT",  ".",  "#",  false, false,  false, false  },
                    // Extended cases.
                    //LINE  INPUT   SOFT  HARD  HAS    IS_HARD HAS    IS_HARD
                    //                          SOFT   CTOR    SOFT   INCR
                    //                          CTOR           INCR
                    //----  ------  ----  ----  -----  ------- -----  -------
                    { L_,   ".#.#", ".",  "#",  true,  true,   false, true   },
                    { L_,   ".#.T", ".",  "#",  true,  true,   false, false  },
                    { L_,   "#.#.", ".",  "#",  true,  true,   true,  true   },
                    { L_,   "T.##", ".",  "#",  true,  true,   false, true   },
                    { L_,   "T.#T", ".",  "#",  true,  true,   false, false  },
                    { L_,   "T#.#", ".",  "#",  true,  true,   false, true   },
                    { L_,   "T#.T", ".",  "#",  true,  true,   false, false  },
            };   // end table DATA

            enum { DATA_LEN = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < DATA_LEN; ++i) {
                const int        LINE          = DATA[i].d_line;
                const char      *INPUT         = DATA[i].d_input;
                const StringRef  SOFT          = DATA[i].d_soft
                                                 ? StringRef(DATA[i].d_soft)
                                                 : StringRef();
                const StringRef  HARD          = DATA[i].d_hard
                                                 ? StringRef(DATA[i].d_hard)
                                                 : StringRef();
                bool             HAS_SOFT_CTOR = DATA[i].d_hasSoftCtor;
                bool             IS_HARD_CTOR  = DATA[i].d_isHardCtor;
                bool             HAS_SOFT_INCR = DATA[i].d_hasSoftIncr;
                bool             IS_HARD_INCR  = DATA[i].d_isHardIncr;

                Obj        mT(INPUT, SOFT, HARD);
                const Obj& T = mT;

                ASSERTV(LINE, HAS_SOFT_CTOR == T.hasTrailingSoft());
                ASSERTV(LINE, IS_HARD_CTOR  == T.isTrailingHard());

                ++mT;

                ASSERTV(LINE, HAS_SOFT_INCR == T.hasTrailingSoft());
                ASSERTV(LINE, IS_HARD_INCR  == T.isTrailingHard());
            }
        }
      } break;
      case 3: {
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   Tokenizer(const char *, const StringRef&, const StringRef&);
        //   Tokenizer(const StringRef&, const StringRef&, const StringRef&);
        //   Tokenizer::operator++()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nTesting Tokenizer constructors" << endl;
        {
            static const struct {
                int         d_line;         // line number
                const char *d_input;        // input string
                const char *d_soft;         // list of soft delimiters
                const char *d_hard;         // list of hard delimiters
                const char *d_token;        // expected token
                const char *d_delim;        // expected current delimiter
                const char *d_prevDelim;    // expected previous delimiter
            } DATA[] = {
                //LINE  INPUT   SOFT   HARD   TOKEN   DELIM  PREV
                //                                           DELIM
                //----  ------  ----   ----   ------  -----  -----
                { L_,   "",     0,     0,     "",     "",    ""    },
                { L_,   "",     0,     "",    "",     "",    ""    },
                { L_,   "",     "",    0,     "",     "",    ""    },
                { L_,   "",     "",    "",    "",     "",    ""    },
                { L_,   "",     "",    "#",   "",     "",    ""    },
                { L_,   "",     ".",   "",    "",     "",    ""    },
                { L_,   "",     ".",   "#",   "",     "",    ""    },
                { L_,   ".",    ".",   "",    "",     "",    "."   },
                { L_,   ".",    ".",   "#",   "",     "",    "."   },
                { L_,   "#",    "",    "#",   "",     "#",   ""    },
                { L_,   "#",    ".",   "#",   "",     "#",   ""    },
                { L_,   "T",    "",    "",    "T",    "",    ""    },
                { L_,   "T",    "",    "#",   "T",    "",    ""    },
                { L_,   "T",    ".",   "",    "T",    "",    ""    },
                { L_,   "T",    ".",   "#",   "T",    "",    ""    },
                { L_,   "..",   ".",   "#",   "",     "",    ".."  },
                { L_,   "##",   "",    "#",   "",     "#",   ""    },
                { L_,   "##",   ".",   "#",   "",     "#",   ""    },
                { L_,   ".#",   ".",   "#",   "",     "#",   "."   },
                { L_,   "#.",   ".",   "#",   "",     "#.",  ""    },
                { L_,   ".T",   ".",   "",    "T",    "",    "."   },
                { L_,   ".T",   ".",   "#",   "T",    "",    "."   },
                { L_,   "#T",   "",    "#",   "",     "#",   ""    },
                { L_,   "#T",   ".",   "#",   "",     "#",   ""    },
                { L_,   "T.",   ".",   "",    "T",    ".",   ""    },
                { L_,   "T.",   ".",   "#",   "T",    ".",   ""    },
                { L_,   "T#",   "",    "#",   "T",    "#",   ""    },
                { L_,   "T#",   ".",   "#",   "T",    "#",   ""    },
                { L_,   "TT",   "",    "",    "TT",   "",    ""    },
                { L_,   "TT",   "",    "#",   "TT",   "",    ""    },
                { L_,   "TT",   ".",   "",    "TT",   "",    ""    },
                { L_,   "TT",   ".",   "#",   "TT",   "",    ""    },
                //LINE  INPUT   SOFT   HARD   TOKEN   DELIM  PREV
                //                                           DELIM
                //----  ------  ----   ----   ------  -----  -----
                { L_,   "...",  ".",   "#",   "",     "",    "..." },
                { L_,   "..#",  ".",   "#",   "",     "#",   ".."  },
                { L_,   "..T",  ".",   "#",   "T",    "",    ".."  },
                { L_,   ".#.",  ".",   "#",   "",     "#.",  "."   },
                { L_,   ".##",  ".",   "#",   "",     "#",   "."   },
                { L_,   ".#T",  ".",   "#",   "",     "#",   "."   },
                { L_,   "#..",  ".",   "#",   "",     "#..", ""    },
                { L_,   "#.#",  ".",   "#",   "",     "#.",  ""    },
                { L_,   "#.T",  ".",   "#",   "",     "#.",  ""    },
                { L_,   "T..",  ".",   "#",   "T",    "..",  ""    },
                { L_,   "T.#",  ".",   "#",   "T",    ".#",  ""    },
                { L_,   "T.T",  ".",   "#",   "T",    ".",   ""    },
                { L_,   "T#.",  ".",   "#",   "T",    "#.",  ""    },
                { L_,   "T##",  ".",   "#",   "T",    "#",   ""    },
                { L_,   "T#T",  ".",   "#",   "T",    "#",   ""    },
                { L_,   "TT.",  ".",   "#",   "TT",   ".",   ""    },
                { L_,   "TT#",  ".",   "#",   "TT",   "#",   ""    },
                { L_,   "TTT",  ".",   "#",   "TTT",  "",    ""    },

            };   // end table DATA
            enum { DATA_LEN = sizeof DATA / sizeof *DATA };

            if (verbose) cout << "\tTesting 'bdlb::Tokenizer(const char*, "
                              << "const StringRef&, const StringRef&)'."
                              << endl;
            for (int i = 0; i < DATA_LEN; ++i) {
                const int        LINE          = DATA[i].d_line;
                const char      *INPUT         = DATA[i].d_input;
                const StringRef  SOFT          = DATA[i].d_soft
                                                 ? StringRef(DATA[i].d_soft)
                                                 : StringRef();
                const StringRef  HARD          = DATA[i].d_hard
                                                 ? StringRef(DATA[i].d_hard)
                                                 : StringRef();
                const char      *TOKEN         = DATA[i].d_token;
                const char      *DELIM         = DATA[i].d_delim;
                const char      *PREV_DELIM    = DATA[i].d_prevDelim;

                Obj        mT(INPUT, SOFT, HARD);
                const Obj& T = mT;

                ASSERTV(LINE, PREV_DELIM    == T.previousDelimiter());
                ASSERTV(LINE, TOKEN         == T.token());
                ASSERTV(LINE, DELIM         == T.trailingDelimiter());
            }

            if (verbose) cout << "\tTesting 'bdlb::Tokenizer("
                              << "const StringRef&, "
                              << "const StringRef&, "
                              << "const StringRef&)'."
                              << endl;
            for (int i = 0; i < DATA_LEN; ++i) {
                const int        LINE          = DATA[i].d_line;
                const StringRef  INPUT         = StringRef(DATA[i].d_input);
                const StringRef  SOFT          = DATA[i].d_soft
                                                 ? StringRef(DATA[i].d_soft)
                                                 : StringRef();
                const StringRef  HARD          = DATA[i].d_hard
                                                 ? StringRef(DATA[i].d_hard)
                                                 : StringRef();
                const char      *TOKEN         = DATA[i].d_token;
                const char      *DELIM         = DATA[i].d_delim;
                const char      *PREV_DELIM    = DATA[i].d_prevDelim;

                Obj        mT(INPUT, SOFT, HARD);
                const Obj& T = mT;

                ASSERTV(LINE, PREV_DELIM    == T.previousDelimiter());
                ASSERTV(LINE, TOKEN         == T.token());
                ASSERTV(LINE, DELIM         == T.trailingDelimiter());
            }
        }

        if (verbose) cout << "\nTesting Tokenizer::operator++()" << endl;
        {
            static const struct {
                int         d_line;         // line number
                const char *d_input;        // input string
                const char *d_soft;         // list of soft delimiters
                const char *d_hard;         // list of hard delimiters
                const char *d_token;        // expected token
                const char *d_delim;        // expected current delimiter
                const char *d_prevDelim;    // expected previous delimiter
                  // Those fields contain expected values after operator++
                const char *d_token1;       // expected token
                const char *d_delim1;       // expected current delimiter
                const char *d_prevDelim1;   // expected previous delimiter
            } DATA[] = {
                //LINE  INPUT   SOFT   HARD   TOKEN   DELIM  PREV
                //                                           DELIM
                //----  ------  ----   ----   ------  -----  -----
                { L_,   "",     ".",   "#",   "",     "",    "",     "",     "",    ""    },
                { L_,   ".",    ".",   "#",   "",     "",    ".",    "",     "",    ""    },
                { L_,   "#",    ".",   "#",   "",     "#",   "",     "",     "",    "#"   },
                { L_,   "T",    ".",   "#",   "T",    "",    "",     "",     "",    ""    },
                { L_,   "..",   ".",   "#",   "",     "",    "..",   "",     "",    ""    },
                { L_,   "##",   ".",   "#",   "",     "#",   "",     "",     "#",   "#"   },
                { L_,   ".#",   ".",   "#",   "",     "#",   ".",    "",     "",    "#"   },
                { L_,   "#.",   ".",   "#",   "",     "#.",  "",     "",     "",    "#."  },
                { L_,   ".T",   ".",   "#",   "T",    "",    ".",    "",     "",    ""    },
                { L_,   "#T",   ".",   "#",   "",     "#",   "",     "T",    "",    "#"   },
                { L_,   "T.",   ".",   "#",   "T",    ".",   "",     "",     "",    "."   },
                { L_,   "T#",   ".",   "#",   "T",    "#",   "",     "",     "",    "#"   },
                { L_,   "TT",   ".",   "#",   "TT",   "",    "",     "",     "",    ""    },
                //LINE  INPUT   SOFT   HARD   TOKEN   DELIM  PREV
                //                                           DELIM
                //----  ------  ----   ----   ------  -----  -----
                { L_,   "...",  ".",   "#",   "",     "",    "...",  "",     "",    ""    },
                { L_,   "..#",  ".",   "#",   "",     "#",   "..",   "",     "",    "#"   },
                { L_,   "..T",  ".",   "#",   "T",    "",    "..",   "",     "",    ""    },
                { L_,   ".#.",  ".",   "#",   "",     "#.",  ".",    "",     "",    "#."  },
                { L_,   ".##",  ".",   "#",   "",     "#",   ".",    "",     "#",   "#"   },
                { L_,   ".#T",  ".",   "#",   "",     "#",   ".",    "T",    "",    "#"   },
                { L_,   "#..",  ".",   "#",   "",     "#..", "",     "",     "",    "#.." },
                { L_,   "#.#",  ".",   "#",   "",     "#.",  "",     "",     "#",   "#."  },
                { L_,   "#.T",  ".",   "#",   "",     "#.",  "",     "T",    "",    "#."  },
                { L_,   "T..",  ".",   "#",   "T",    "..",  "",     "",     "",    ".."  },
                { L_,   "T.#",  ".",   "#",   "T",    ".#",  "",     "",     "",    ".#"  },
                { L_,   "T.T",  ".",   "#",   "T",    ".",   "",     "T",    "",    "."   },
                { L_,   "T#.",  ".",   "#",   "T",    "#.",  "",     "",     "",    "#."  },
                { L_,   "T##",  ".",   "#",   "T",    "#",   "",     "",     "#",   "#"   },
                { L_,   "T#T",  ".",   "#",   "T",    "#",   "",     "T",    "",    "#"   },
                { L_,   "TT.",  ".",   "#",   "TT",   ".",   "",     "",     "",    "."   },
                { L_,   "TT#",  ".",   "#",   "TT",   "#",   "",     "",     "",    "#"   },
                { L_,   "TTT",  ".",   "#",   "TTT",  "",    "",     "",     "",    ""    },
                // Extended cases.
                //LINE  INPUT   SOFT   HARD   TOKEN   DELIM  PREV
                //                                           DELIM
                //----  ------  ----   ----   ------  -----  -----
                { L_,   ".#.#", ".",   "#",   "",     "#.",  ".",    "",     "#",   "#."  },
                { L_,   ".#.T", ".",   "#",   "",     "#.",  ".",    "T",    "",    "#."  },
                { L_,   "#.#.", ".",   "#",   "",     "#.",  "",     "",     "#.",  "#."  },
                { L_,   "T.##", ".",   "#",   "T",    ".#",  "",     "",     "#",   ".#"  },
                { L_,   "T.#T", ".",   "#",   "T",    ".#",  "",     "T",    "",    ".#"  },
                { L_,   "T#.#", ".",   "#",   "T",    "#.",  "",     "",     "#",   "#."  },
                { L_,   "T#.T", ".",   "#",   "T",    "#.",  "",     "T",    "",    "#."  },
            };   // end table DATA

            enum { DATA_LEN = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < DATA_LEN; ++i) {
                const int        LINE          = DATA[i].d_line;
                const char      *INPUT         = DATA[i].d_input;
                const StringRef  SOFT          = DATA[i].d_soft
                                                 ? StringRef(DATA[i].d_soft)
                                                 : StringRef();
                const StringRef  HARD          = DATA[i].d_hard
                                                 ? StringRef(DATA[i].d_hard)
                                                 : StringRef();
                const char      *TOKEN         = DATA[i].d_token;
                const char      *DELIM         = DATA[i].d_delim;
                const char      *PREV_DELIM    = DATA[i].d_prevDelim;
                // Expected after operator++()
                const char      *TOKEN1         = DATA[i].d_token1;
                const char      *DELIM1         = DATA[i].d_delim1;
                const char      *PREV_DELIM1    = DATA[i].d_prevDelim1;

                Obj        mT(INPUT, SOFT, HARD);
                const Obj& T = mT;

                ASSERTV(LINE, PREV_DELIM    == T.previousDelimiter());
                ASSERTV(LINE, TOKEN         == T.token());
                ASSERTV(LINE, DELIM         == T.trailingDelimiter());

                ++mT;

                ASSERTV(LINE, PREV_DELIM1    == T.previousDelimiter());
                ASSERTV(LINE, TOKEN1         == T.token());
                ASSERTV(LINE, DELIM1         == T.trailingDelimiter());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_FAIL(Obj(static_cast<const char*>(0), "", ""));
            ASSERT_SAFE_FAIL(Obj(StringRef(), "", ""));
            ASSERT_SAFE_PASS(Obj("", "", ""));
            ASSERT_SAFE_PASS(Obj(StringRef(""), StringRef()));
            ASSERT_SAFE_PASS(Obj(StringRef(""), StringRef(), StringRef()));
        }

        if (verbose) cout << "\nDelimiter parameters testing." << endl;
        {
            if (verbose) cout << "\tSingle delimiter testing." << endl;
            {
                for (int i = 0; i < 256; ++i) {
                    char delim;
                    delim = static_cast<char>(i);

                    char input[256];
                    int j = 0;
                    for( int k = 0; k<256; ++k) {
                        if (k != i) {
                            input[j++] = static_cast<char>(k);
                        }
                    }

                    Obj mT1(StringRef(input, 255),
                            StringRef(&delim, 1),
                            StringRef());

                    ASSERT(""                    == mT1.previousDelimiter());
                    ASSERT(StringRef(input, 255) == mT1.token());
                    ASSERT(""                    == mT1.trailingDelimiter());

                    Obj mT2(StringRef(input, 255),
                            StringRef(),
                            StringRef(&delim, 1));

                    ASSERT(""                    == mT2.previousDelimiter());
                    ASSERT(StringRef(input, 255) == mT2.token());
                    ASSERT(""                    == mT2.trailingDelimiter());
                }
            }

            if (verbose) cout << "\tMultiple delimiter testing." << endl;
            {
                char input[256];
                char delim[256];
                for (int i = 1; i < 255; ++i) {
                    for (int j = 0; j < i; ++j) {
                        input[j] = static_cast<char>(j);
                    }
                    for (int j = i; j < 255; ++j) {
                        delim[j - i] = static_cast<char>(j);
                    }

                    Obj mT1(StringRef(input, i),
                            StringRef(delim, 255-i),
                            StringRef());

                    ASSERT(""                  == mT1.previousDelimiter());
                    ASSERT(StringRef(input, i) == mT1.token());
                    ASSERT(""                  == mT1.trailingDelimiter());

                    Obj mT2(StringRef(input, i),
                            StringRef(),
                            StringRef(delim, 255-i));

                    ASSERT(""                  == mT2.previousDelimiter());
                    ASSERT(StringRef(input, i) == mT2.token());
                    ASSERT(""                  == mT2.trailingDelimiter());
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Developer test sandbox. (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;


        if (verbose) cout << "\n'bdlb::Tokenizer' with const char*." << endl;
        {
            Obj tokenizer("Hello, world,,,", " ,");

            while (tokenizer) {
                cout << "|\t"
                     << '"' << tokenizer.token() << '"'
                     << "\t"
                     << '"' << tokenizer.trailingDelimiter() << '"'
                     << "\tSoft?: " << (tokenizer.hasTrailingSoft() ? "T":"F")
                     << "\tPSoft?: " << (tokenizer.hasPreviousSoft() ? "T":"F")
                     << "\tHard?: " << (tokenizer.isTrailingHard() ? "T":"F")
                     << "\tPHard?: " << (tokenizer.isPreviousHard() ? "T":"F")
                     << endl;
                ++tokenizer;
            }
        }

        if (verbose) cout << "\n'bdlb::Tokenizer' with StringRef."  << endl;
        {
            Obj tokenizer(StringRef(",,Hello, world,,,"),
                          StringRef(" "),
                          StringRef(","));

            while (tokenizer) {
                cout << "|\t"
                     << '"' << tokenizer.token() << '"'
                     << "\t"
                     << '"' << tokenizer.trailingDelimiter() << '"'
                     << "\tSoft?: " << (tokenizer.hasTrailingSoft() ? "T":"F")
                     << "\tPSoft?: " << (tokenizer.hasPreviousSoft() ? "T":"F")
                     << "\tHard?: " << (tokenizer.isTrailingHard() ? "T":"F")
                     << "\tPHard?: " << (tokenizer.isPreviousHard() ? "T":"F")
                     << endl;
                ++tokenizer;
            }
        }

        if (verbose) cout << "\n'bdlb::Tokenizer' with StringRef."  << endl;
        {
            Obj tokenizer(StringRef("   I've : been a : :bad   boy!"),
                          StringRef(" "),
                          StringRef(":/"));

            for (; tokenizer; ++tokenizer) {
                cout << "|\t"
                     << '"' << tokenizer.token() << '"'
                     << "\t"
                     << '"' << tokenizer.trailingDelimiter() << '"'
                     << "\tSoft?: " << (tokenizer.hasTrailingSoft() ? "T":"F")
                     << "\tPSoft?: " << (tokenizer.hasPreviousSoft() ? "T":"F")
                     << "\tHard?: " << (tokenizer.isTrailingHard() ? "T":"F")
                     << "\tPHard?: " << (tokenizer.isPreviousHard() ? "T":"F")
                     << endl;
            }
        }

        if (verbose) cout << "\n'bdlb::TokenizerIterator' test." << endl;
        {
            Obj tokenizer(StringRef("   I've : been a : :  bad   boy!   "),
                          StringRef(" "),
                          StringRef(":/"));

            for (Obj::iterator it=tokenizer.begin();
                               it != tokenizer.end();
                               ++it) {
                cout << "|\t"
                     << '"' << *it << '"'
                     << endl;

            }
            ++tokenizer;
            ++tokenizer;

            for (Obj::iterator it=tokenizer.begin();
                               it != tokenizer.end();
                               ++it) {
                cout << "|\t"
                     << '"' << *it << '"'
                     << endl;

            }
        }

        if (verbose) cout << "\n'bdlb::TokenizerIterator' test." << endl;
        {
            Obj tokenizer("   I've : been a : :  bad   boy!   ",
                          StringRef(" "),
                          StringRef(":/"));

            for (Obj::iterator it=tokenizer.begin();
                               it != tokenizer.end();
                               ++it) {
                cout << "|\t"
                     << '"' << *it << '"'
                     << endl;

            }

            Obj::iterator t1=tokenizer.begin();
            Obj::iterator t2=tokenizer.begin();

            ASSERT(t1 == t2);
            ++t1;
            ASSERT(t1 != t2);
            ++t2;
            ASSERT(t1 == t2);
            t1++;
            ASSERT(t1 != t2);
            t2++;
            ASSERT(t1 == t2);

        }


      } break;

      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        // Testing:
        //   PERFORMANCE TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PERFORMANCE TEST" << endl
                          << "================" << endl;


        if (verbose) cout << "\n'bdlb::TokenizerIterator' test." << endl;
        {
            Obj      tokenizer(INPUT, StringRef(" "));
            for (int i = 0; i < 1000000; ++i) {
                Obj::iterator end = tokenizer.end();
                for (Obj::iterator it = tokenizer.begin();
                                   it != end;
                                   ++it) {
                     *it;
                }
            }
        }
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        // Testing:
        //   PERFORMANCE TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PERFORMANCE TEST" << endl
                          << "================" << endl;


        if (verbose) cout << "\n'bdlb::Tokenizer' test." << endl;
        {
            for (int i = 0; i < 1000000; ++i) {
                for (Obj it(INPUT, StringRef(" "));
                         it;
                         ++it) {
                     it.token();
                }
            }
        }
      } break;
      case -3: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        // Testing:
        //   PERFORMANCE TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PERFORMANCE TEST" << endl
                          << "================" << endl;


        if (verbose) cout << "\n'bdlb::Tokenizer' test." << endl;
        {
            for (int i = 0; i < 1000000; ++i) {
                for (Obj it(StringRef(INPUT), StringRef(" "));
                         it;
                         ++it) {
                     it.token();
                }
            }
        }
      } break;
      case -4: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        // Testing:
        //   PERFORMANCE TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PERFORMANCE TEST" << endl
                          << "================" << endl;


        if (verbose) cout << "\n'bdlb::StrTokenRefIter' test." << endl;
        {
            for (int i = 0; i < 1000000; ++i) {
                for (bdlb::StrTokenRefIter it(StringRef(INPUT), " ");
                                   it;
                                   ++it) {
                     it();
                }
            }
        }
      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

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
