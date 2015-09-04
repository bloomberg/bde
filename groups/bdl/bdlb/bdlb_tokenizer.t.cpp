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
//*[ 3] Tokenizer_Data(const StringRef& softDelimiters);
//*[ 2] Tokenizer_Data(const bslstl::StringRef& sd, const StringRef& hd);
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
//            _____________________Expected Parse of INPUT__________________
//L# INPUT    LEADER TOKEN0 DELIM0 TOKEN1 DELIM1 TOKEN2 DELIM2 TOKEN3 DELIM3
//-- ------   ------ ------ ------ ------ ------ ------ ------ ------ ------
{L_, { "HI",   "",    "",    "H",    "",   "I"                             } },
{L_, { "H0",   "",    "",    "H",    "0",  ""                              } },
{L_, { "sH0",  "s",   "",    "H",    "0",  ""                              } },
{L_, { "s0t1", "s",   "0",   "t",    "1",  ""                              } },
{L_, { "HIJK", "",    "",    "H",    "",   "I",    "",   "J",    "",   "K" } },
//--------------v
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

                    const int N = (cursor - EXPECTED) / 2;  // Nth token

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

        if (verbose) cout <<
                  "\nNeagtive tests: repeated delimiters; null input." << endl;
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
                         it.isValid();
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
