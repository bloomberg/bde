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
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This component consists of a top-level mechanism, 'Tokenizer', that
// dispenses in-core value-semantic (standard) input iterators, of type
// 'InputIterator', which in turn can be used to sequence over tokens
// characterized by "soft" and "hard" delimiter characters maintained in a
// shared private class, 'Tokenizer_Data'.  The 'Tokenizer' mechanism, though
// not value-semantic, can also be used to sequence of the tokens a given
// input, supplied at construction, additionally providing access to the
// previous and current trailing delimiters.
//..
//  +--------------------------------------+
//  |   ,--------------.                   |
//  |  ( Tokenizer_Data )                  |
//  |   `--------------'\                  |
//  |          |     ,---*-------------.   |
//  |          |    ( TokenizerIterator )  |
//  |          |    /`-----------------'   |
//  |     ,----*---o.                      |
//  |    ( Tokenizer )                     |
//  |     `---------'                      |
//  +--------------------------------------+
//  bdlb_tokenizer
//..
// The plan will be to test the (component private) 'Tokenizer_Data' first,
// followed by 'Tokenizer' itself, and finally the 'TokenizerIterator', as
// these iterators are created (and returned by value) only from a valid
// 'Tokenizer' object.
//
///Tokenizer_Data
///--------------
//: o Primary Manipulators:
//:   - Tokenizer_Data(const bslstl::StringRef& sd, const StringRef& hd);
//: o Basic Accessors:
//:   - int inputType(char character) const;
//
///Tokenizer
///---------
//: o Primary Manipulators:
//:   - Tokenizer(const StringRef& i, const StringRef& s, const StringRef& h);
//:   - Tokenizer(const char *i, const StringRef& s, const StringRef& h);
//:   - Tokenizer& operator++();
//: o Basic Accessors:
//:   - bool isValid() const;
//:   - StringRef previousDelimiter() const;
//:   - StringRef token() const;
//:   - StringRef trailingDelimiter() const;
//
///TokenizerIterator
///-----------------
//: o Primary Manipulators:
//:   - Tokenizer::begin() const;
//:   - Tokenizer::end() const;
//:   - TokenizerIterator();
//:   - TokenizerIterator& operator++();
//: o Basic Accessors:
//:   - StringRef operator*() const;
//:   - bool operator==(const TokenizerIterator&, const TokenizerIterator&);
//
// Keep in mind that the inertial contract for the 'Tokenizer_Data' states that
// if the same character is supplied as both a 'soft' or 'hard' delimiter, it
// is considered 'hard' by the 'Tokenizer_Data::inputType' method, yet
// supplying that same character to both delimiter sets of a 'Tokenizer' would
// be considered (library) undefined and must be checked (in the appreciate
// build mode) to ensure that such user are detected (and similarly for
// repeated characters in the same delimiter set).
//
// We will need to make sure that  the use of postfix ++ on a 'Tokenizer'
// object fails to compile.  We will also want to make sure that neither
// 'Tokenizer_Data' or 'Tokenizer' is not copy constructable or assignable.
//
//-----------------------------------------------------------------------------
//                      // ----------------------------
//                      // private class Tokenizer_Data
//                      // ----------------------------
// CREATERS
//*[ 3] Tokenizer_Data(const StringRef& softD);
//*[ 2] Tokenizer_Data(const bslstl::StringRef& softD, const StringRef& hardD);
//*[ 2] ~Tokenizer_Data()
//
// ACCESSORS
//*[ 2] int inputType(char character) const;
//
//                        // -----------------------
//                        // class TokenizerIterator
//                        // -----------------------
//
// CREATORS
// [  ] TokenizerIterator();
// [  ] TokenizerIterator(const TokenizerIterator& other);
// [  ] ~Tokenizer();
//
// MANIPULATORS
// [  ] TokenizerIterator& operator=(const TokenizerIterator& rhs);
// [  ] TokenizerIterator& operator++();
//
// ACCESSORS
// [  ] StringRef operator*() const;
//
// FREE OPERATORS
// [  ] bool operator==(const TokenizerIterator&, const TokenizerIterator& );
// [  ] bool operator!=(const TokenizerIterator&, const TokenizerIterator& );
// [  ] const TokenizerIterator operator++(TokenizerIterator& object, int);
//
//                             // ===============
//                             // class Tokenizer
//                             // ===============
//
// TYPES
// [  ] typedef TokenizerIterator iterator;
//
// CREATORS
// [  ] Tokenizer(const char *input, const StringRef& soft);
// [  ] Tokenizer(const StringRef& input, const StringRef& soft);
//*[ 5] Tokenizer(const char *i, const StringRef& s, const StringRef& h);
//*[ 4] Tokenizer(const StringRef&, const StringRef&, const StringRef&);
//*[ 5] ~Tokenizer();
//
// MANIPULATORS
//*[ 5] Tokenizer& operator++();
// [  ] void reset(const char *input);
// [  ] void reset(const bslstl::StringRef& input);
//
// ACCESSORS
// [  ] bool hasPreviousSoft() const;
// [  ] bool hasTrailingSoft() const;
// [  ] bool isPreviousHard() const;
// [  ] bool isTrailingHard() const;
//*[ 6] bool isValid () const;
//*[ 7] StringRef previousDelimiter() const;
//*[ 7] StringRef token() const;
//*[ 7] StringRef trailingDelimiter() const;
//
//                        // iterators
// [  ] iterator begin() const;
// [  ] iterator end() const;
//
// FREE OPERATORS
// [  ] const Tokenizer operator++(Tokenizer& object, int);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] OPERATOR POSTFIX ++ FOR TOKENIZER DOES NOT COMPILE
// [  ] TOKENIZER IS NOT COPIABLE OR ASSIGNABLE
// [  ] CONSTRUCTOR OF TOKENIZER_DATA HANDLES DUPLICATE CHARACTERS
// [  ] CONSTRUCTOR OF TOKENIZER WARNS IN DEBUG MODE ON DUPLICATE CHARACTERS
// [  ] USAGE EXAMPLE #1
// [  ] USAGE EXAMPLE #2
// [  ] USAGE EXAMPLE #3
// [  ] USAGE EXAMPLE #4
// [  ] USAGE EXAMPLE #5

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
                    { L_,   "#",    ".",   "#",   true,   false },
                    { L_,   "T",    ".",   "#",   true,   false },
                    { L_,   "##",   ".",   "#",   true,   true  },
                    { L_,   ".#",   ".",   "#",   true,   false },
                    { L_,   "#.",   ".",   "#",   true,   false },
                    { L_,   ".T",   ".",   "#",   true,   false },
                    { L_,   "#T",   ".",   "#",   true,   true  },
                    { L_,   "T.",   ".",   "#",   true,   false },
                    { L_,   "T#",   ".",   "#",   true,   false },
                    { L_,   "TT",   ".",   "#",   true,   false },
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

                ASSERTV(LINE, VALID_CTOR == T.isValid());

                ++mT;

                ASSERTV(LINE, VALID_INCR == T.isValid());
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
             { L_,  "##",   ".",  "#",  "",    "#",  "",    "",   "#",  "#"  },
             { L_,  "#T",   ".",  "#",  "",    "#",  "",    "T",  "",   "#"  },
             { L_,  ".##",  ".",  "#",  "",    "#",  ".",   "",   "#",  "#"  },
             { L_,  ".#T",  ".",  "#",  "",    "#",  ".",   "T",  "",   "#"  },
             { L_,  "#.#",  ".",  "#",  "",    "#.", "",    "",   "#",  "#." },
             { L_,  "#.T",  ".",  "#",  "",    "#.", "",    "T",  "",   "#." },
             { L_,  "T.T",  ".",  "#",  "T",   ".",  "",    "T",  "",   "."  },
             { L_,  "T##",  ".",  "#",  "T",   "#",  "",    "",   "#",  "#"  },
             { L_,  "T#T",  ".",  "#",  "T",   "#",  "",    "T",  "",   "#"  },
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

                ASSERTV(LINE, INPUT, DELIM_CTOR      == T.trailingDelimiter());
                ASSERTV(LINE, INPUT, PREV_DELIM_CTOR == T.previousDelimiter());
                ASSERTV(LINE, INPUT, TOKEN_CTOR      == T.token());

                ++mT;

                ASSERTV(LINE, INPUT, DELIM_INCR      == T.trailingDelimiter());
                ASSERTV(LINE, INPUT, PREV_DELIM_INCR == T.previousDelimiter());
                ASSERTV(LINE, INPUT, TOKEN_INCR      == T.token());
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
              { L_,   ".##",  ".",   "#",  true,    false,   false,   true   },
              { L_,   ".#T",  ".",   "#",  true,    false,   false,   true   },
              { L_,   "#.#",  ".",   "#",  false,   false,   true,    true   },
              { L_,   "#.T",  ".",   "#",  false,   false,   true,    true   },
              { L_,   "T.#",  ".",   "#",  false,   false,   true,    true   },
              { L_,   "T.T",  ".",   "#",  false,   false,   true,    false  },
              { L_,   "T##",  ".",   "#",  false,   false,   false,   true   },
              { L_,   "T#T",  ".",   "#",  false,   false,   false,   true   },
              { L_,   "TT#",  ".",   "#",  false,   false,   false,   true   },
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
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //   Bring the object to every state relevant for thorough testing.
        //
        // Concerns:
        //:  1 We cover all (including internal) relevant states.
        //:  2 Multiple distinct characters of the same type work.
        //:  3 Repeated characters behave the same as above.
        //:  4 The null character behave the same as any other character.
        //:  5 Non-ASCII characters behave the same as ASCII ones.
        //:  6 Inputs requiring many iterations succeed.
        //:  7 Inputs having large tokens/delimiters succeed.
        //:  8 Supplying a null input is detected (DEBUG).
        //:  9 Non-unique delimiter characters are detected (DEBUG).
        //: 10 Iterating from an invalid state (DEBUG).
        //
        // Plan:
        //: 1 Using the table-driven technique, apply depth-ordered enumeration
        //:   on the length of the input string to parse all unique inputs (in
        //:   lexicographic order) up to a "depth" of 4 (note that for this
        //:   first test, we have hard-coded "stuv" to be set of soft delimiter
        //:   characters, and "HIJK" to be the set of hard ones, leaving the
        //:   digit characters "1234" to be used as unique token characters).
        //:   The input string as well as the sequence of expected "parsed"
        //:   strings will be provided -- each on a single row of the table.
        //:   Failing to supply a token (followed by its trailing delimiter)
        //:   implies that the iterator has become invalid (which is tested)
        //:   after the internal iteration loop exits. [C 1..2]
        //:
        //: 2 Additional add-hoc tests are provided to address remaining
        //:   concerns. [C 3..7]
        //: 3 Finally defensive checks are addressed. [C 8..10]
        //
        // Testing:
        //   Tokenizer(const char *i, const StringRef& sd, const StringRef& hd)
        //   Tokenizer(const StringRef&, const StringRef&, const StringRef&)
        //   Tokenizer::operator++()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;
        if (verbose) cout <<
                "\nTesting all input combinations to a depth of four." << endl;
        {
            enum { MAX_ITER = 4 };  // Maximum iterations

            static const struct {
                int         d_line;                         // line number
                const char *d_stringData_p[3 + 2*MAX_ITER]; // input + expected
            } DATA[] = {
 //           ________________Expected Parse of INPUT__________________
  //L#  INPUT   LEADER  TOK0    DEL0   TOK1   DEL1   TOK2  DEL2  TOK3  DEL3
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----

  // Depth 0:   LEADER  TOK0    DEL0   TOK1   DEL1   TOK2  DEL2  TOK3  DEL3
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"",     ""                                                         } },

  // Depth 1:   LEADER  TOK0    DEL0   TOK1   DEL1   TOK2  DEL2  TOK3  DEL3
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"s",    "s"                                                        } },
  {L_, {"H",    "",     "",     "H"                                        } },
  {L_, {"0",    "",     "0",    ""                                         } },

  // Depth 2:   LEADER  TOK0    DEL0   TOK1   DEL1   TOK2  DEL2  TOK3  DEL3
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"st",   "st"                                                       } },
  {L_, {"sH",   "s",    "",     "H"                                        } },
  {L_, {"s0",   "s",    "0",    ""                                         } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"Hs",   "",     "",     "Hs"                                       } },
  {L_, {"HI",   "",     "",     "H",   "",    "I"                          } },
  {L_, {"H0",   "",     "",     "H",   "0",   ""                           } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"0s",   "",     "0",    "s"                                        } },
  {L_, {"0H",   "",     "0",    "H"                                        } },
  {L_, {"01",   "",     "01",   ""                                         } },

  // Depth 3:   LEADER  TOK0    DEL0   TOK1   DEL1   TOK2  DEL2  TOK3  DEL3
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"stu",  "stu"                                                      } },
  {L_, {"stH",  "st",   "",     "H"                                        } },
  {L_, {"st0",  "st",   "0",    ""                                         } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"sHt",  "s",    "",     "Ht"                                       } },
  {L_, {"sHI",  "s",    "",     "H",   "",    "I"                          } },
  {L_, {"sH0",  "s",    "",     "H",   "0",   ""                           } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"s0t",  "s",    "0",    "t"                                        } },
  {L_, {"s0H",  "s",    "0",    "H"                                        } },
  {L_, {"s01",  "s",    "01",   ""                                         } },

  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"Hst",  "",     "",     "Hst"                                      } },
  {L_, {"HsI",  "",     "",     "Hs",  "",    "I"                          } },
  {L_, {"Hs0",  "",     "",     "Hs",  "0",   ""                           } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"HIs",  "",     "",     "H",   "",    "Is"                         } },
  {L_, {"HIJ",  "",     "",     "H",   "",    "I",   "",   "J"             } },
  {L_, {"HI0",  "",     "",     "H",   "",    "I",   "0",  ""              } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"H0s",  "",     "",     "H",   "0",   "s"                          } },
  {L_, {"H0I",  "",     "",     "H",   "0",   "I"                          } },
  {L_, {"H01",  "",     "",     "H",   "01",  ""                           } },

  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"0st",  "",     "0",    "st"                                       } },
  {L_, {"0sH",  "",     "0",    "sH"                                       } },
  {L_, {"0s1",  "",     "0",    "s",   "1",   ""                           } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"0Is",  "",     "0",    "Is"                                       } },
  {L_, {"0HI",  "",     "0",    "H",   "",    "I"                          } },
  {L_, {"0H1",  "",     "0",    "H",   "1",   ""                           } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"01s",  "",     "01",   "s"                                        } },
  {L_, {"01H",  "",     "01",   "H"                                        } },
  {L_, {"012",  "",     "012",  ""                                         } },

  // Depth 4-s: LEADER  TOK0    DEL0   TOK1   DEL1   TOK2  DEL2  TOK3  DEL3
  //-- -------- ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"stuv", "stuv"                                                     } },
  {L_, {"stuH", "stu",  "",     "H"                                        } },
  {L_, {"stu0", "stu",  "0",    ""                                         } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"stHu", "st",   "",     "Hu"                                       } },
  {L_, {"stHI", "st",   "",     "H",   "",    "I"                          } },
  {L_, {"stH0", "st",   "",     "H",   "0",   ""                           } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"st0u", "st",   "0",    "u"                                        } },
  {L_, {"st0H", "st",   "0",    "H"                                        } },
  {L_, {"st01", "st",   "01",   ""                                         } },

  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"sHtu", "s",    "",     "Htu"                                      } },
  {L_, {"sHtI", "s",    "",     "Ht",  "",    "I"                          } },
  {L_, {"sHt0", "s",    "",     "Ht",  "0",   ""                           } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"sHIt", "s",    "",     "H",   "",    "It"                         } },
  {L_, {"sHIJ", "s",    "",     "H",   "",    "I",   "",   "J"             } },
  {L_, {"sHI0", "s",    "",     "H",   "",    "I",   "0",  ""              } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"sH0t", "s",    "",     "H",   "0",   "t"                          } },
  {L_, {"sH0I", "s",    "",     "H",   "0",   "I"                          } },
  {L_, {"sH01", "s",    "",     "H",   "01",  ""                           } },

  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"s0tu", "s",    "0",    "tu"                                       } },
  {L_, {"s0tH", "s",    "0",    "tH"                                       } },
  {L_, {"s0t1", "s",    "0",    "t",   "1",   ""                           } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"s0Ht", "s",    "0",    "Ht"                                       } },
  {L_, {"s0HI", "s",    "0",    "H",   "",    "I"                          } },
  {L_, {"s0H1", "s",    "0",    "H",   "1",   ""                           } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"s01t", "s",    "01",   "t"                                        } },
  {L_, {"s01H", "s",    "01",   "H"                                        } },
  {L_, {"s012", "s",    "012",  ""                                         } },

  // Depth 4-H: LEADER  TOK0    DEL0   TOK1   DEL1   TOK2  DEL2  TOK3  DEL3
  //-- -------- ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"Hstu", "",     "",     "Hstu"                                     } },
  {L_, {"HstI", "",     "",     "Hst", "",    "I"                          } },
  {L_, {"Hst0", "",     "",     "Hst", "0",   ""                           } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"HsIt", "",     "",     "Hs",  "",    "It"                         } },
  {L_, {"HsIJ", "",     "",     "Hs",  "",    "I",   "",   "J"             } },
  {L_, {"HsI0", "",     "",     "Hs",  "",    "I",   "0",  ""              } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"Hs0t", "",     "",     "Hs",  "0",   "t"                          } },
  {L_, {"Hs0I", "",     "",     "Hs",  "0",   "I"                          } },
  {L_, {"Hs01", "",     "",     "Hs",  "01",  ""                           } },

  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"HIst", "",     "",     "H",   "",    "Ist"                        } },
  {L_, {"HIsJ", "",     "",     "H",   "",    "Is",  "",   "J"             } },
  {L_, {"HIs0", "",     "",     "H",   "",    "Is",  "0",  ""              } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"HIJs", "",     "",     "H",   "",    "I",   "",   "Js",           } },
  {L_, {"HIJK", "",     "",     "H",   "",    "I",   "",   "J",  "",   "K" } },
  {L_, {"HIJ0", "",     "",     "H",   "",    "I",   "",   "J",  "0",  ""  } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"HI0s", "",     "",     "H",   "",    "I",   "0",  "s"             } },
  {L_, {"HI0J", "",     "",     "H",   "",    "I",   "0",  "J"             } },
  {L_, {"HI01", "",     "",     "H",   "",    "I",   "01", ""              } },

  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"H0st", "",     "",     "H",   "0",   "st"                         } },
  {L_, {"H0sI", "",     "",     "H",   "0",   "sI"                         } },
  {L_, {"H0s1", "",     "",     "H",   "0",   "s",   "1",  ""              } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"H0Is", "",     "",     "H",   "0",   "Is"                         } },
  {L_, {"H0IJ", "",     "",     "H",   "0",   "I",   "",   "J"             } },
  {L_, {"H0I1", "",     "",     "H",   "0",   "I",   "1",  ""              } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"H01s", "",     "",     "H",   "01",  "s"                          } },
  {L_, {"H01I", "",     "",     "H",   "01",  "I"                          } },
  {L_, {"H012", "",     "",     "H",   "012", ""                           } },

  // Depth 4-0: LEADER  TOK0    DEL0   TOK1   DEL1   TOK2  DEL2  TOK3  DEL3
  //-- -------- ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"0stu", "",     "0",    "stu"                                      } },
  {L_, {"0stI", "",     "0",    "stI"                                      } },
  {L_, {"0st1", "",     "0",    "st",  "1",   ""                           } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"0sHt", "",     "0",    "sHt"                                      } },
  {L_, {"0sHI", "",     "0",    "sH",  "",    "I"                          } },
  {L_, {"0sH1", "",     "0",    "sH",  "1",   ""                           } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"0s1t", "",     "0",    "s",   "1",   "t"                          } },
  {L_, {"0s1H", "",     "0",    "s",   "1",   "H"                          } },
  {L_, {"0s12", "",     "0",    "s",   "12",  ""                           } },

  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"0Hst", "",     "0",    "Hst"                                      } },
  {L_, {"0HsI", "",     "0",    "Hs",  "",    "I"                          } },
  {L_, {"0Hs1", "",     "0",    "Hs",  "1",   ""                           } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"0HIs", "",     "0",    "H",   "",    "Is"                         } },
  {L_, {"0HIJ", "",     "0",    "H",   "",    "I",   "",   "J"             } },
  {L_, {"0HI1", "",     "0",    "H",   "",    "I",   "1",  ""              } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"0Hst", "",     "0",    "Hst"                                      } },
  {L_, {"0HsI", "",     "0",    "Hs",  "",    "I"                          } },
  {L_, {"0Hs1", "",     "0",    "Hs",  "1",   ""                           } },

  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"01st", "",     "01",   "st"                                       } },
  {L_, {"01sH", "",     "01",   "sH"                                       } },
  {L_, {"01s2", "",     "01",   "s",   "2",   ""                           } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"01Hs", "",     "01",   "Hs"                                       } },
  {L_, {"01HI", "",     "01",   "H",   "",    "I"                          } },
  {L_, {"01H2", "",     "01",   "H",   "2",   ""                           } },
  //--  ------  ------  ------  ------ ------ ------ ----- ----- ----  ----
  {L_, {"012s", "",     "012",  "s"                                        } },
  {L_, {"012H", "",     "012",  "H"                                        } },
  {L_, {"0123", "",     "0123", ""                                         } },
            };  // DATA

            enum { DATA_LEN = sizeof DATA / sizeof *DATA };

            // Specification of character types used in this test.

            const char SOFT_DELIM_CHARS[] = "stuv";
            const char HARD_DELIM_CHARS[] = "HIJK";
            const char TOKEN_CHARS[]      = "0123";

            // Used inside the loop to ensure the input is composed
            // of only the valid characters above.

            const char VALID_CHARS[] = "stuvHIJK0123";

            for (int ti = 0; ti < DATA_LEN; ++ti) {
                const int    LINE     = DATA[ti].d_line;
                const char  *INPUT    = DATA[ti].d_stringData_p[0];
                const char *const *EXPECTED = DATA[ti].d_stringData_p + 1;

                // Make sure that each character in the input string is unique
                // and among the characters in the three strings listed above.

                const size_t INPUT_LENGTH = strlen(INPUT);

                bool uniqueCharsInInputFlag = true;

                for (size_t i = 0; i < INPUT_LENGTH; ++i) {
                    for (size_t j = i + 1; j < INPUT_LENGTH; ++j) {
                        if (INPUT[i] == INPUT[j]) {
                            uniqueCharsInInputFlag = false;
                            ASSERTV(LINE, i, j, INPUT, uniqueCharsInInputFlag);
                        }
                    }
                }

                bool allCharsInInputAreValidFlag =
                                   strlen(INPUT) == strspn(INPUT, VALID_CHARS);

                ASSERTV(LINE, INPUT, VALID_CHARS, allCharsInInputAreValidFlag);

                if (!uniqueCharsInInputFlag || !allCharsInInputAreValidFlag) {
                    continue;  // The input for this row of the table is bad.
                }

                // Make sure that the characters in the input string occur in
                // the same relative order as they do in each of the respective
                // character sets defined above, and that they do NOT skip over
                // any characters in those respective sets.

#if 0
                //TBD: PLEASE WRITE THIS CODE HERE (AND VERIFY THAT *EVERY*
                     ASPECT WORKS);  IF IT FAILS, ASSERT AND CONTINUE
                     (AS ABOVE).  Please either provide explicit tests in the
                     test driver for these and the above tests (implemented as
                     functions, and tested in an earlier test case) or make
                     "damn" sure (by deliberately breaking the input that all
                     such defects are always reported.  ITS ON YOU! Keep in
                     mind that test apparatus is a valid entry (below the line)
                     and I think all of these qualify.  I encourage you to test
                     all such test apparatus in the new test case 3, and to make
                     this one test case 4.  :)
#endif

                Obj        mT(INPUT,
                              StringRef(SOFT_DELIM_CHARS),
                              StringRef(HARD_DELIM_CHARS));
                const Obj& T = mT;

                // Initially 'cursor' is the address of the first token string.
                for (const char * const *cursor = EXPECTED+1;
                                        *cursor;
                                        cursor += 2) {

                    ASSERTV(LINE, T.isValid());  // Table: tokanizer's valid!

                    // Extract iteration number, N; use in error reporting.

                    const long int N = (cursor - EXPECTED) / 2;  // Nth token

            //------^
            // Expected values at (this) iteration, N, of Tokenizer's op++:

            const char *EXP_PREV  = cursor[-1];  // initially the "leader"
            const char *EXP_TOKEN = cursor[ 0];  // current token
            const char *EXP_POST  = cursor[+1];  // current (trailing) delim.

            // Shorten for better error messages.

            const StringRef RET_PREV  = T.previousDelimiter();
            const StringRef RET_TOKEN = T.token();
            const StringRef RET_POST  = T.trailingDelimiter();

            ASSERTV(LINE, N, EXP_PREV,  RET_PREV,   EXP_PREV  == RET_PREV);
            ASSERTV(LINE, N, EXP_TOKEN, RET_TOKEN,  EXP_TOKEN == RET_TOKEN);
            ASSERTV(LINE, N, EXP_POST,  RET_POST,   EXP_POST  == RET_POST);
            //------v
                    ++mT;

                }  // for current token in input row

                ASSERTV(LINE, !T.isValid());

            }  // for each row in table
        }

        if (verbose) cout <<
                   "\nAd hoc testing of various character properties." << endl;
        {
            // 3. Repeated characters behaves the same as above.
            // 4. The null character behave the same as any other character.
            // 5. Non-ASCII characters behave the same as ASCII ones.
        }

        if (verbose) cout <<
                  "\nAd hoc 'stress' testing for iterations and size." << endl;
        {
            // 6. Inputs requiring many iterations work as expected.
            // 7. Inputs having large tokens/delimeters work as expected.
        }

        if (verbose) cout << "\nNegative tests: null input, iterating in "
                          << "invalid state."
                          << endl;
        {
            //  8. Supplying a null input is detected (DEBUG).
            //  9. Non-unique delimiter characters are detected (DEBUG).
            // 10. Iterating from an invalid state (DEBUG).
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_FAIL(Obj(static_cast<const char*>(0), "", ""));
            ASSERT_SAFE_FAIL(Obj(StringRef(), "", ""));
            ASSERT_SAFE_PASS(Obj("", "", ""));
            ASSERT_SAFE_PASS(Obj(StringRef(""), StringRef()));
            ASSERT_SAFE_PASS(Obj(StringRef(""), StringRef(), StringRef()));
        }
      } break;

      case 2: {
        // --------------------------------------------------------------------
        // Tokenizer_Data test
        //   Ensure that types of characters are specified correctly by
        //   'Tokenizer_Data' constructors and accessor returns them properly.
        //
        // Concerns:
        //: 1 Any character can be specified as either token, soft delimiter or
        //:   hard delimiter input type.
        //:
        //: 2 Both constructors correctly map soft and hard delimiters sets,
        //:   supplied at construction, to corresponding input types.
        //:
        //: 3 'inputType' method returns the actual input type of the specified
        //:    character.
        //
        // Plan:
        //: 1 Create a 'Tokenizer_Data' object, using one argument constructor
        //:   for soft delimiter set categories of "empty", one character,
        //:   multiple characters and all characters.  Verify that all
        //:   characters from the supplied set are mapped to a soft delimiter
        //:   input type and all other characters are mapped to a token input
        //:   type.  (C-1..3)
        //:
        //: 2 Create a 'Tokenizer_Data' object, using two argument constructor
        //:   for cross product of soft and hard delimiter sets categories of
        //:   "empty", one character, multiple characters and all characters.
        //:   Verify that:
        //:
        //:   1 All characters, not presented in the soft and hard delimiter
        //:     sets, are mapped to the token input type.  (C-2..3)
        //:
        //:   2 Characters, presented only in one set (soft or hard delimiter)
        //:     are mapped to the soft and hard delimiter input types
        //:     respectively.  (C-2..3)
        //:
        //:   3 Characters, presented in both sets simultaneously, are mapped
        //:     to the hard delimiter input type (hard delimiter  precedence
        //:     over soft).  (C-2..3)
        //:
        //
        // Testing:
        //   Tokenizer_Data(const StringRef& softD);
        //   Tokenizer_Data(const StringRef& softD, const StringRef& hardD);
        //   int inputType(char character);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Tokenizer_Data" << endl
                          << "======================" << endl;

        // Copy of the input character to input type mapping.
        enum InputType {
            TOK = 0,  // token character
            SFT = 1,  // soft delimiter character
            HRD = 2,  // hard delimiter character
        };

        typedef bdlb::Tokenizer_Data Obj;

        if (verbose) cout << "\nTesting Tokenizer_Data(const StringRef&)."
                          << endl;
        {
            if (veryVerbose) cout << "\tEmpty soft delimiter test." << endl;
            {
                StringRef  nS;
                // Constructing with Obj mD(StringRef()); fails due to vexing
                // parse issue.
                Obj        mD(nS);
                const Obj& D = mD;

                for (int i = 0; i < 256; ++i) {
                    char ch = static_cast<char>(i);
                    ASSERTV(i, TOK == D.inputType(ch));
                }
            }
            {
                Obj        mD(StringRef(""));
                const Obj& D = mD;

                for (int i = 0; i < 256; ++i) {
                    char ch = static_cast<char>(i);
                    ASSERTV(i, TOK == D.inputType(ch));
                }
            }

            if (veryVerbose) cout << "\tSingle delimiter test." << endl;
            {
                for (int i = 0; i < 256; ++i) {
                    char delim[1];
                    delim[0] = static_cast<char>(i);

                    Obj        mD(StringRef(delim, 1));
                    const Obj& D = mD;

                    for (int j = 0; j < 256; j++) {
                        char ch = static_cast<char>(j);
                        if (i != j) {
                            ASSERTV(i, j, TOK == D.inputType(ch));
                        } else {
                            ASSERTV(i, j, SFT == D.inputType(ch));
                        }
                    }
                }
            }
            if (veryVerbose) cout <<"\tDuplicate delimiter test." << endl;
            {
                for (int i = 0; i < 256; ++i) {
                    char delim[2];
                    delim[0] = static_cast<char>(i);
                    delim[1] = static_cast<char>(i);

                    Obj        mD(StringRef(delim, 2));
                    const Obj& D = mD;

                    for (int j = 0; j < 256; j++) {
                        char ch = static_cast<char>(j);
                        if (i != j) {
                            ASSERTV(i, j, TOK == D.inputType(ch));
                        } else {
                            ASSERTV(i, j, SFT == D.inputType(ch));
                        }
                    }
                }
            }

            if (veryVerbose) cout << "\tMultiple delimiter test." << endl;
            {
                char delim[256];
                for (int i = 0; i < 256; ++i) {
                    for (int j = 0; j < i; ++j) {
                        delim[j] = static_cast<char>(j);
                    }

                    Obj        mD(StringRef(delim, i));
                    const Obj& D = mD;

                    for (int j = 0; j < 256; ++j) {
                        char ch = static_cast<char>(j);
                        if (j >= i) {
                            ASSERTV(i, j, TOK == D.inputType(ch));
                        } else {
                            ASSERTV(i, j, SFT == D.inputType(ch));
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting Tokenizer_Data(const StringRef&, "
                          << "const StringRef&)."
                          << endl;
        {
            if (veryVerbose) cout << "\tEmpty delimiter test." << endl;
            {
                StringRef  nS;
                Obj        mD(nS, nS);
                const Obj& D = mD;

                for (int i = 0; i < 256; ++i) {
                    char ch = static_cast<char>(i);
                    ASSERTV(i, TOK == D.inputType(ch));
                }
            }
            {
                Obj        mD(StringRef(""), StringRef(""));
                const Obj& D = mD;

                for (int i = 0; i < 256; ++i) {
                    char ch = static_cast<char>(i);
                    ASSERTV(i, TOK == D.inputType(ch));
                }
            }

            if (veryVerbose) cout << "\tSingle delimiter test." << endl;
            {
                for (int i = 0; i < 256; ++i) {
                    char delim[1];
                    delim[0] = static_cast<char>(i);

                    Obj        mD(StringRef(delim, 1), StringRef());
                    const Obj& D = mD;

                    for (int j = 0; j < 256; j++) {
                        char ch = static_cast<char>(j);
                        if (i != j) {
                            ASSERTV(i, j, TOK == D.inputType(ch));
                        } else {
                            ASSERTV(i, j, SFT == D.inputType(ch));
                        }
                    }
                }
            }
            {
                for (int i = 0; i < 256; ++i) {
                    char delim[1];
                    delim[0] = static_cast<char>(i);

                    Obj        mD(StringRef(""), StringRef(delim, 1));
                    const Obj& D = mD;

                    for (int j = 0; j < 256; j++) {
                        char ch = static_cast<char>(j);
                        if (i != j) {
                            ASSERTV(i, j, TOK == D.inputType(ch));
                        } else {
                            ASSERTV(i, j, HRD == D.inputType(ch));
                        }
                    }
                }
            }

            if (veryVerbose) cout << "\tDuplicate delimiter test." << endl;
            {
                for (int i = 0; i < 256; ++i) {
                    char delim[2];
                    delim[0] = static_cast<char>(i);
                    delim[1] = static_cast<char>(i);

                    Obj        mD(StringRef(""), StringRef(delim, 2));
                    const Obj& D = mD;

                    for (int j = 0; j < 256; j++) {
                        char ch = static_cast<char>(j);
                        if (i != j) {
                            ASSERTV(i, j, TOK == D.inputType(ch));
                        } else {
                            ASSERTV(i, j, HRD == D.inputType(ch));
                        }
                    }
                }
            }

            if (veryVerbose) cout << "\tMultiple delimiter test." << endl;
            {
                char delim[256];
                for (int i = 0; i < 256; ++i) {
                    for (int j = 0; j < i; ++j) {
                        delim[j] = static_cast<char>(j);
                    }

                    Obj        mD(StringRef(""), StringRef(delim, i));
                    const Obj& D = mD;

                    for (int j = 0; j < 256; ++j) {
                        char ch = static_cast<char>(j);
                        if (j >= i) {
                            ASSERTV(i, j, TOK == D.inputType(ch));
                        } else {
                            ASSERTV(i, j, HRD == D.inputType(ch));
                        }
                    }
                }
            }

            if (veryVerbose) cout
                << "\tHard delimiter precedence test." << endl;
            {
                for (int i = 0; i < 256; ++i) {
                    char delim[1];
                    delim[0] = static_cast<char>(i);

                    Obj        mD(StringRef(delim, 1), StringRef(delim, 1));
                    const Obj& D = mD;

                    for (int j = 0; j < 256; j++) {
                        char ch = static_cast<char>(j);
                        if (i != j) {
                            ASSERTV(i, j, TOK == D.inputType(ch));
                        } else {
                            ASSERTV(i, j, HRD == D.inputType(ch));
                        }
                    }
                }
            }
            {
                char delim[256];
                for (int i = 0; i < 256; ++i) {
                    for (int j = 0; j < i; ++j) {
                        delim[j] = static_cast<char>(j);
                    }

                    Obj        mD(StringRef(delim, i), StringRef(delim, i));
                    const Obj& D = mD;

                    for (int j = 0; j < 256; ++j) {
                        char ch = static_cast<char>(j);
                        if (j >= i) {
                            ASSERTV(i, j, TOK == D.inputType(ch));
                        } else {
                            ASSERTV(i, j, HRD == D.inputType(ch));
                        }
                    }
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

            while (tokenizer.isValid()) {
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

            while (tokenizer.isValid()) {
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

            for (; tokenizer.isValid(); ++tokenizer) {
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

            for (Obj::iterator it=tokenizer.begin(), end = tokenizer.end();
                               it != end;
                               ++it) {
                cout << "|\t"
                     << '"' << *it << '"'
                     << endl;

            }
            ++tokenizer;
            ++tokenizer;

            for (Obj::iterator it=tokenizer.begin(), end = tokenizer.end();
                               it != end;
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

            for (Obj::iterator it=tokenizer.begin(), end = tokenizer.end();
                               it != end;
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
                for (Obj::iterator it  = tokenizer.begin(),
                                   end = tokenizer.end();
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
                for (Obj it(INPUT, StringRef(" ")); it.isValid(); ++it) {
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
                         it.isValid();
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
