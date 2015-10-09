// bdlb_tokenizer.t.cpp                                               -*-C++-*-
#include <bdlb_tokenizer.h>

#include <bslim_testutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_string.h>

#include <bsl_cstdlib.h>                  // 'bsl::atoi'
#include <bsl_cstring.h>                  // 'bsl::memcpy', 'bsl::strcmp'
#include <bsl_iostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This component consists of a top-level mechanism, 'Tokenizer', that
// dispenses in-core value-semantic (standard) input iterators, of type
// 'TokenizerIterator', which in turn can be used to sequence over tokens
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
//:   - Tokenizer_Data(const StringRef& softD, const StringRef& hardD);
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
// Primary manipulators and basic accessors tests for 'Tokenizer' class have
// been united because of identity of testing.  Also secondary constructor
// verification has been added to the same test case in order not to duplicate
// machinery and the table of input data.  Similarly tests for
// 'TokenizerIterator' class have been united.
//
// We will need to make sure that  the use of postfix ++ on a 'Tokenizer'
// object fails to compile.  We will also want to make sure that neither
// 'Tokenizer_Data' or 'Tokenizer' is not copy constructable or assignable.
//
//-----------------------------------------------------------------------------
//                      // ----------------------------
//                      // private class Tokenizer_Data
//                      // ----------------------------
// CREATORS
// [ 2] Tokenizer_Data(const StringRef& softD);
// [ 2] Tokenizer_Data(const StringRef& softD, const StringRef& hardD);
// [ 2] ~Tokenizer_Data();
//
// ACCESSORS
// [ 2] int inputType(char character) const;
//
//                        // -----------------------
//                        // class TokenizerIterator
//                        // -----------------------
//
// CREATORS
// [ 7] TokenizerIterator();
// [ 7] TokenizerIterator(const TokenizerIterator& origin);
// [ 7] ~TokenizerIterator();
//
// MANIPULATORS
// [ 8] TokenizerIterator& operator=(const TokenizerIterator& rhs);
// [ 7] TokenizerIterator& operator++();
//
// ACCESSORS
// [ 7] StringRef operator*() const;
//
// FREE OPERATORS
// [ 8] bool operator==(const TokenizerIterator&,const TokenizerIterator&)
// [ 8] bool operator!=(const TokenizerIterator&,const TokenizerIterator&)
// [ 8] const TokenizerIterator operator++(TokenizerIterator& obj, int);
//
//                             // ===============
//                             // class Tokenizer
//                             // ===============
//
// TYPES
// [  ] typedef TokenizerIterator iterator;
//
// CREATORS
// [ 4] Tokenizer(const char *, const StringRef&);
// [ 4] Tokenizer(const StringRef&, const StringRef&);
// [ 4] Tokenizer(const char *, const StringRef&, const StringRef&);
// [ 4] Tokenizer(const StringRef&, const StringRef&, const StringRef&);
// [ 4] ~Tokenizer();
//
// MANIPULATORS
// [ 4] Tokenizer& operator++();
// [ 6] void reset(const char *input);
// [ 6] void reset(const StringRef& input);
//
// ACCESSORS
// [ 5] bool hasPreviousSoft() const;
// [ 5] bool hasTrailingSoft() const;
// [ 5] bool isPreviousHard() const;
// [ 5] bool isTrailingHard() const;
// [ 5] bool isValid() const;
// [ 4] StringRef previousDelimiter() const;
// [ 4] StringRef token() const;
// [ 4] StringRef trailingDelimiter() const;
//
//                        // iterators
// [ 7] iterator begin() const;
// [  ] iterator end() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TEST APPARATUS
// [  ] OPERATOR POSTFIX ++ FOR TOKENIZER DOES NOT COMPILE
// [  ] TOKENIZER IS NOT COPYABLE OR ASSIGNABLE
// [  ] CONSTRUCTOR OF TOKENIZER_DATA HANDLES DUPLICATE CHARACTERS
// [  ] CONSTRUCTOR OF TOKENIZER WARNS IN DEBUG MODE ON DUPLICATE CHARACTERS
// [ 9] USAGE EXAMPLE

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
typedef Obj::iterator      ObjIt;
typedef bslstl::StringRef  StringRef;

// Specification of character types used in this test driver
const char TOKEN_CHARS[]      = "0123";
const char SOFT_DELIM_CHARS[] = "stuv";
const char HARD_DELIM_CHARS[] = "HIJK";

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

bool isValid(const StringRef input,
             const StringRef soft,
             const StringRef hard,
             const StringRef token);
    // Validate the specified 'input', 'soft', 'hard' and 'token' character
    // sets and return 'true' if the supplied character sets are valid and
    // 'false' otherwise.  Valid character sets must confirm to the following
    // rules:
    // o There are no duplicates in the soft, hard and token character sets.
    //
    // o There are no duplicates in the input character set.
    //
    // o All characters in the input string occur in the same relative order as
    //   they do in each of the respective character sets and that they do NOT
    //   skip over any characters in those respective sets.
    //
    // o Only characters from the respective character sets appear in the
    //   input string.
    //
    // All strings that do not confirm the to rules above are invalid.
bool isValid(const StringRef input,
             const StringRef soft,
             const StringRef hard,
             const StringRef token)
{
    int inputLength = static_cast<int>(input.length());
    int softLength  = static_cast<int>(soft.length());
    int hardLength  = static_cast<int>(hard.length());
    int tokenLength = static_cast<int>(token.length());

    // Sets under the test are small, using brute force implementation.

    // Check for duplicates in input.  We need to test for duplicates
    // separately for the cases when soft, hard and token character sets are
    // empty.

    for (int i = 0; i < inputLength; ++i) {
        for (int j = i+1; j < inputLength; ++j) {
            if (input[i] == input[j]) {
                return false;                                         // RETURN
            }
        }
    }

    // Check for duplicates in and across soft, hard and token character sets.

    for (int i = 0; i < softLength; ++i) {
        for (int j = i+1; j < softLength; ++j) {
            if (soft[i] == soft[j]) {
                return false;                                         // RETURN
            }
        }
        for (int j = 0; j < hardLength; ++j) {
            if (soft[i] == hard[j]) {
                return false;                                         // RETURN
            }
        }
        for (int j = 0; j < tokenLength; ++j) {
            if (soft[i] == token[j]) {
                return false;                                         // RETURN
            }
        }
    }

    for (int i = 0; i < hardLength; ++i) {
        for (int j = i+1; j < hardLength; ++j) {
            if (hard[i] == hard[j]) {
                return false;                                         // RETURN
            }
        }
        for (int j = 0; j < tokenLength; ++j) {
            if (hard[i] == token[j]) {
                return false;                                         // RETURN
            }
        }
    }

    for (int i = 0; i < tokenLength; ++i) {
        for (int j = i+1; j < tokenLength; ++j) {
            if (token[i] == token[j]) {
                return false;                                         // RETURN
            }
        }
    }

    // Check that all characters in the input string occur in the same relative
    // order as they do in each of the respective character sets and that they
    // do NOT skip over any characters in those respective sets.

    int softIndex  = 0;
    int hardIndex  = 0;
    int tokenIndex = 0;
    for (int i = 0; i < inputLength; ++i) {
        if (softIndex < softLength &&
            input[i] == soft[softIndex]) {
            ++softIndex;
        } else if (hardIndex < hardLength &&
                   input[i] == hard[hardIndex]) {
            ++hardIndex;
        } else if (tokenIndex < tokenLength &&
                   input[i] == token[tokenIndex]) {
            ++tokenIndex;
        } else {
            return false;                                             // RETURN
        }
    }
    return true;
}

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
      case 9: {
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
        if (verbose) cout << "TODO" << endl;
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // 'TokenizerIterator' OPERATORS
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 Assigning doesn't modify the value of the source object.
        //:
        //: 3 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //: 4 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 5 Comparison is symmetric.
        //:
        //: 6 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 7 The post-increment operator changes the value of the object to
        //:   refer to the next element in the list.
        //:
        //: 8 The value returned is the value of the object prior to the
        //:   operator call.
        //:
        //: 9 The signature and return type of operators are standard.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a sets of input for
        //:   tokenizer construction.
        //:
        //: 2 For each row 'R' in the table of P-1:
        //:
        //:   1 Create a tokenizer and acquire a TokenizerIterator.
        //:
        //:   2 Create an empty TokenizerIterator and assign previous one to
        //:     it.
        //:
        //:   3 Verify integrity of the source object.  (C-2)
        //:
        //:   4 Verify new state of tested iterator.  (C-1,9)
        //:
        //:   5 Assign tested iterator to itself and verify it's integrity.
        //:     (C-3,9)
        //:
        //: 3 For each row 'R' in the table of P-1:
        //:
        //:   1 Create a tokenizer and acquire a TokenizerIterator.
        //:
        //:   2 Create a copy of iterator.
        //:
        //:   3 Move both iterators along the input string and verify their
        //:     comparison results.  (C-4..6,9)
        //:
        //: 4 For each row 'R' in the table of P-1:
        //:
        //:   1 Create a tokenizer and acquire a TokenizerIterator (model).
        //:
        //:   2 Create a copy of model (tested iterator).
        //:
        //:   3 Move model iterator with pre-increment operator and tested
        //:     iterator with post-increment operator.  Verify returned value
        //:     of post-increment operator and compare both operators result.
        //:     (C-7..9)
        //
        // Testing:
        //   TokenizerIterator& operator=(const TokenizerIterator& rhs);
        //   bool operator==(const TokenizerIterator&,const TokenizerIterator&)
        //   bool operator!=(const TokenizerIterator&,const TokenizerIterator&)
        //   const TokenizerIterator operator++(TokenizerIterator& obj, int);
        // --------------------------------------------------------------------

        if (verbose)
                    cout << endl
                         << "'TokenizerIterator' OPERATORS" << endl
                         << "=============================" << endl;

        if (verbose) cout << "\nTesting TokenizerIterator operators." << endl;

        const int NUM_ITERATIONS = 5;

        static const struct {
            int         d_line;                    // line number
            const char *d_input_p;                 // input
            const char *d_tokens[NUM_ITERATIONS];  // values array
        } DATA[] = {
            //LINE  INPUT           PREV
            //                      DELIM
            //----  -------------   ------
            { L_,   "",            {      }},
            { L_,   "s",           {      }},
            { L_,   "H",           {""    }},
            { L_,   "0",           {"0"   }},

            { L_,   "0s1H2tI3Ju",  {"0",
                                    "1",
                                    "2",
                                    "3"   }},

            { L_,   "s0H1tI2Ju3",  {"0",
                                    "1",
                                    "2",
                                    "3"   }},

            { L_,   "s0tH12Iu3J",  {"0",
                                    "12",
                                    "3"   }},
        };
        enum { DATA_LEN = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\tTesting 'operator='." << endl;
        {
            for (int ti = 0; ti < DATA_LEN; ++ti) {
                const int    LINE     = DATA[ti].d_line;
                const char  *INPUT    = DATA[ti].d_input_p;

                if (veryVerbose) {
                    T_ P_(LINE) P(INPUT)
                }

                ASSERTV(LINE, isValid(INPUT,
                                      SOFT_DELIM_CHARS,
                                      HARD_DELIM_CHARS,
                                      TOKEN_CHARS));

                Obj          mT(INPUT, SOFT_DELIM_CHARS, HARD_DELIM_CHARS);
                ObjIt        mIt = mT.begin();
                const ObjIt& It  = mIt;

                int i = 0;
                do {
                    // To test the immutability of the source object we need to
                    // store it's current state.  As we can't call accessor for
                    // invalid iterators, we need to check it's validity first.

                    StringRef currentToken;
                    if (DATA[ti].d_tokens[i]) {
                        currentToken = *It;
                    }

                    ObjIt        eMIt;                 // experimental iterator
                    const ObjIt& eIt  = eMIt;
                    eMIt = It;

                    // We need to test 'operator=' even with invalid iterator
                    // passed as a parameter, because it is allowed by
                    // component contract.  But we can't call accessor for such
                    // iterators, so we need to stop current iteration in that
                    // case.

                    if (!DATA[ti].d_tokens[i]) {
                        break;
                    }

                    const StringRef TOKEN              = DATA[ti].d_tokens[i];
                    const StringRef MODEL_TOKEN        = *It;
                    const StringRef EXPERIMENTAL_TOKEN = *eIt;

                    // Testing the immutability of the source object.

                    ASSERTV(LINE,
                            i,
                            MODEL_TOKEN,
                            currentToken,
                            MODEL_TOKEN == currentToken);

                    if (veryVerbose) {
                        T_ T_ P_(i) P_(MODEL_TOKEN) P(EXPERIMENTAL_TOKEN)
                    }

                    ASSERTV(LINE,
                            i,
                            TOKEN,
                            MODEL_TOKEN,
                            TOKEN == MODEL_TOKEN);
                    ASSERTV(LINE,
                            i,
                            MODEL_TOKEN,
                            EXPERIMENTAL_TOKEN,
                            MODEL_TOKEN == EXPERIMENTAL_TOKEN);

                    // Testing assigning an object to itself.

                    eMIt = eMIt;
                    currentToken = *eIt;

                    ASSERTV(LINE,
                            i,
                            EXPERIMENTAL_TOKEN,
                            currentToken,
                            EXPERIMENTAL_TOKEN == currentToken);

                    ++mIt;
                    ++i;
                } while (true);
            }
        }

        if (verbose) cout << "\tTesting 'operator==' and 'operator!='."
                          << endl;
        {
            for (int ti = 0; ti < DATA_LEN; ++ti) {
                const int    LINE     = DATA[ti].d_line;
                const char  *INPUT    = DATA[ti].d_input_p;

                if (veryVerbose) {
                    T_ P_(LINE) P(INPUT)
                }

                ASSERTV(LINE, isValid(INPUT,
                                      SOFT_DELIM_CHARS,
                                      HARD_DELIM_CHARS,
                                      TOKEN_CHARS));

                Obj          mT(INPUT, SOFT_DELIM_CHARS, HARD_DELIM_CHARS);
                ObjIt        mIt = mT.begin();
                const ObjIt& It  = mIt;
                ObjIt        eMIt(It);  // experimental iterator
                const ObjIt& eIt  = eMIt;

                int i = 0;
                do {
                    bool areEqual    = (It == eIt);
                    bool areNotEqual = (It != eIt);

                    if (veryVerbose) {
                        T_ T_ P_(i) P_(areEqual) P(areNotEqual)
                    }
                    ASSERTV(LINE, i, true  == areEqual);
                    ASSERTV(LINE, i, false == areNotEqual);

                    // Testing comparison symmetry

                    areEqual    = (eIt == It);
                    areNotEqual = (eIt != It);

                    ASSERTV(LINE, i, true  == areEqual);
                    ASSERTV(LINE, i, false == areNotEqual);

                    // We need to test 'operator==' and 'operator!=' even with
                    // invalid iterators passed as a parameter, because it is
                    // allowed by component contract.  But we can't move
                    // forward such iterators, so we need to stop current
                    // iteration in that case.

                    if (!DATA[ti].d_tokens[i]) {
                        break;
                    }

                    ++mIt;

                    areEqual    = (It == eIt);
                    areNotEqual = (It != eIt);

                    ASSERTV(LINE, i, false == areEqual);
                    ASSERTV(LINE, i, true  == areNotEqual);

                    ++eMIt;
                    ++i;
                } while (true);
            }
        }

        if (verbose) cout << "\tTesting post-increment operator" << endl;
        {
            for (int ti = 0; ti < DATA_LEN; ++ti) {
                const int    LINE     = DATA[ti].d_line;
                const char  *INPUT    = DATA[ti].d_input_p;

                if (veryVerbose) {
                    T_ P_(LINE) P(INPUT)
                }

                ASSERTV(LINE, isValid(INPUT,
                                      SOFT_DELIM_CHARS,
                                      HARD_DELIM_CHARS,
                                      TOKEN_CHARS));

                Obj          mT(INPUT, SOFT_DELIM_CHARS, HARD_DELIM_CHARS);
                ObjIt        mIt = mT.begin();
                const ObjIt& It  = mIt;
                ObjIt        eMIt(It);  // experimental iterator
                const ObjIt& eIt  = eMIt;

                int i = 0;
                do {
                    if (!DATA[ti].d_tokens[i]) {
                        break;
                    }

                    // Testing return value.

                    ObjIt        mTempIt(eMIt++);
                    const ObjIt &tempIt  = mTempIt;
                    bool         arePrevEqual = (It == tempIt);

                    // Testing iterator shift.

                    ++mIt;
                    bool areEqual = (It == eIt);

                    if (veryVerbose) {

                        T_ T_ P_(i) P_(arePrevEqual) P(areEqual)
                    }

                    ASSERTV(LINE, i, true == arePrevEqual);
                    ASSERTV(LINE, i, true == areEqual);

                    ++i;
                } while (true);
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // 'TokenizerIterator' PRIMARY MANIPULATORS AND BASIC ACCESSOR
        //
        // Concerns:
        //: 1 All (including internal) relevant states can be reached with
        //:   primary manipulators.
        //:
        //: 2 TokenizerIterator correctly performs a traverse along the input
        //:   string supplied at the 'Tokenizer' construction.
        //:
        //: 3 The pre-increment operator changes the value of the object to
        //:    refer to the next token sequence in the input string.
        //:
        //: 4 Indirection operator provide an access to the tokens sequence,
        //:   iterator pointing to.
        //:
        //: 5 'TokenizerIterator' object can be destroyed.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a sets of input for
        //:   tokenizer construction.
        //:
        //: 2 For each row 'R' in the table of P-1 create a tokenizer and
        //:   acquire TokenizerIterator.  Iterate through the input string and
        //:   verify token sequence, returned by the indirection operator.
        //:   Failing to supply a token implies that the iterator has become
        //:   invalid after the internal iteration loop exits.  (C-1..4)
        //:
        //: 3 Allow 'TokenizerIterator' object leave the scope.  (C-5)
        //
        // Testing:
        //   iterator begin() const;
        //   TokenizerIterator(const TokenizerIterator& origin);
        //   ~TokenizerIterator();
        //   TokenizerIterator& operator++();
        //   StringRef operator*() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
      << "'TokenizerIterator' PRIMARY MANIPULATORS AND BASIC ACCESSOR" << endl
      << "===========================================================" << endl;

        if (verbose) cout <<
                  "\nTesting TokenizerIterator ctor, copy ctor and operator++."
                          << endl;
        {
            enum { MAX_ITER = 4 };  // Maximum iterations

            static const struct {
                int         d_line;                       // line number
                const char *d_stringData_p[3 + MAX_ITER]; // input + expected
            } DATA[] = {
                //________________Expected Parse of INPUT__________________
                //L#  INPUT   TOK0    TOK1   TOK2  TOK3
                //--  ------  ------  ------ ----- ----

                //L#  INPUT   TOK0    TOK1   TOK2  TOK3
                //--  ------  ------  ------ ----- ----
                {L_, {"",                              } }, // Depth 0

                //L#  INPUT   TOK0    TOK1   TOK2  TOK3
                //--  ------  ------  ------ ----- ----
                {L_, {"s",                             } }, // Depth 1
                {L_, {"H",    "",                      } },
                {L_, {"0",    "0",                     } },

                //L#  INPUT   TOK0    TOK1   TOK2  TOK3
                //--  ------  ------  ------ ----- ----
                {L_, {"st",                            } }, // Depth 2
                {L_, {"sH",   "",                      } },
                {L_, {"s0",   "0",                     } },
                //--  ------  ------  ------ ----- ----
                {L_, {"Hs",   "",                      } },
                {L_, {"HI",   "",     "",              } },
                {L_, {"H0",   "",     "0",             } },
                //--  ------  ------  ------ ----- ----
                {L_, {"0s",   "0",                     } },
                {L_, {"0H",   "0",                     } },
                {L_, {"01",   "01",                    } },

                //L#  INPUT   TOK0    TOK1   TOK2  TOK3
                //--  ------  ------  ------ ----- ----
                {L_, {"stu",                           } }, // Depth 3
                {L_, {"stH",  "",                      } },
                {L_, {"st0",  "0",                     } },
                //--  ------  ------  ------ ----- ----
                {L_, {"sHt",  "",                      } },
                {L_, {"sHI",  "",     "",              } },
                {L_, {"sH0",  "",     "0",             } },
                //--  ------  ------  ------ ----- ----
                {L_, {"s0t",  "0",                     } },
                {L_, {"s0H",  "0",                     } },
                {L_, {"s01",  "01",                    } },

                //--  ------  ------  ------ ----- ----
                {L_, {"Hst",  "",                      } },
                {L_, {"HsI",  "",     "",              } },
                {L_, {"Hs0",  "",     "0",             } },
                //--  ------  ------  ------ ----- ----
                {L_, {"HIs",  "",     "",              } },
                {L_, {"HIJ",  "",     "",    "",       } },
                {L_, {"HI0",  "",     "",    "0",      } },
                //--  ------  ------  ------ ----- ----
                {L_, {"H0s",  "",     "0",             } },
                {L_, {"H0I",  "",     "0",             } },
                {L_, {"H01",  "",     "01",            } },

                //--  ------  ------  ------ ----- ----
                {L_, {"0st",  "0",                     } },
                {L_, {"0sH",  "0",                     } },
                {L_, {"0s1",  "0",    "1",             } },
                //--  ------  ------  ------ ----- ----
                {L_, {"0Hs",  "0",                     } },
                {L_, {"0HI",  "0",    "",              } },
                {L_, {"0H1",  "0",    "1",             } },
                //--  ------  ------  ------ ----- ----
                {L_, {"01s",  "01",                    } },
                {L_, {"01H",  "01",                    } },
                {L_, {"012",  "012",                   } },

                //L#  INPUT   TOK0    TOK1   TOK2  TOK3
                //-- -------- ------  ------ ----- ----
                {L_, {"stuv",                          } }, // Depth 4
                {L_, {"stuH", "",                      } },
                {L_, {"stu0", "0",                     } },
                //--  ------  ------  ------ ----- ----
                {L_, {"stHu", "",                      } },
                {L_, {"stHI", "",     "",              } },
                {L_, {"stH0", "",     "0",             } },
                //--  ------  ------  ------ ----- ----
                {L_, {"st0u", "0",                     } },
                {L_, {"st0H", "0",                     } },
                {L_, {"st01", "01",                    } },

                //--  ------  ------  ------ ----- ----
                {L_, {"sHtu", "",                      } },
                {L_, {"sHtI", "",     "",              } },
                {L_, {"sHt0", "",     "0",             } },
                //--  ------  ------  ------ ----- ----
                {L_, {"sHIt", "",     "",              } },
                {L_, {"sHIJ", "",     "",    "",       } },
                {L_, {"sHI0", "",     "",    "0",      } },
                //--  ------  ------  ------ ----- ----
                {L_, {"sH0t", "",     "0",             } },
                {L_, {"sH0I", "",     "0",             } },
                {L_, {"sH01", "",     "01",            } },

                //--  ------  ------  ------ ----- ----
                {L_, {"s0tu", "0",                     } },
                {L_, {"s0tH", "0",                     } },
                {L_, {"s0t1", "0",    "1",             } },
                //--  ------  ------  ------ ----- ----
                {L_, {"s0Ht", "0",                     } },
                {L_, {"s0HI", "0",    "",              } },
                {L_, {"s0H1", "0",    "1",             } },
                //--  ------  ------  ------ ----- ----
                {L_, {"s01t", "01",                    } },
                {L_, {"s01H", "01",                    } },
                {L_, {"s012", "012",                   } },

                // Depth 4-H: TOK0    TOK1   TOK2  TOK3
                //-- -------- ------  ------ ----- ----
                {L_, {"Hstu", "",                      } },
                {L_, {"HstI", "",     "",              } },
                {L_, {"Hst0", "",     "0",             } },
                //--  ------  ------  ------ ----- ----
                {L_, {"HsIt", "",     "",              } },
                {L_, {"HsIJ", "",     "",    "",       } },
                {L_, {"HsI0", "",     "",    "0",      } },
                //--  ------  ------  ------ ----- ----
                {L_, {"Hs0t", "",     "0",             } },
                {L_, {"Hs0I", "",     "0",             } },
                {L_, {"Hs01", "",     "01",            } },

                //--  ------  ------  ------ ----- ----
                {L_, {"HIst", "",     "",              } },
                {L_, {"HIsJ", "",     "",    "",       } },
                {L_, {"HIs0", "",     "",    "0",      } },
                //--  ------  ------  ------ ----- ----
                {L_, {"HIJs", "",     "",    "",       } },
                {L_, {"HIJK", "",     "",    "",   "", } },
                {L_, {"HIJ0", "",     "",    "",   "0",} },
                //--  ------  ------  ------ ----- ----
                {L_, {"HI0s", "",     "",    "0",      } },
                {L_, {"HI0J", "",     "",    "0",      } },
                {L_, {"HI01", "",     "",    "01",     } },

                //--  ------  ------  ------ ----- ----
                {L_, {"H0st", "",     "0",             } },
                {L_, {"H0sI", "",     "0",             } },
                {L_, {"H0s1", "",     "0",   "1",      } },
                //--  ------  ------  ------ ----- ----
                {L_, {"H0Is", "",     "0",             } },
                {L_, {"H0IJ", "",     "0",   "",       } },
                {L_, {"H0I1", "",     "0",   "1",      } },
                //--  ------  ------  ------ ----- ----
                {L_, {"H01s", "",     "01",            } },
                {L_, {"H01I", "",     "01",            } },
                {L_, {"H012", "",     "012",           } },

                // Depth 4-0: TOK0    TOK1   TOK2  TOK3
                //-- -------- ------  ------ ----- ----
                {L_, {"0stu", "0",                     } },
                {L_, {"0stH", "0",                     } },
                {L_, {"0st1", "0",    "1",             } },
                //--  ------  ------  ------ ----- ----
                {L_, {"0sHt", "0",                     } },
                {L_, {"0sHI", "0",    "",              } },
                {L_, {"0sH1", "0",    "1",             } },
                //--  ------  ------  ------ ----- ----
                {L_, {"0s1t", "0",    "1",             } },
                {L_, {"0s1H", "0",    "1",             } },
                {L_, {"0s12", "0",    "12",            } },

                //--  ------  ------  ------ ----- ----
                {L_, {"0Hst", "0",                     } },
                {L_, {"0HsI", "0",    "",              } },
                {L_, {"0Hs1", "0",    "1",             } },
                //--  ------  ------  ------ ----- ----
                {L_, {"0HIs", "0",    "",              } },
                {L_, {"0HIJ", "0",    "",    "",       } },
                {L_, {"0HI1", "0",    "",    "1",      } },
                //--  ------  ------  ------ ----- ----
                {L_, {"0Hst", "0",                     } },
                {L_, {"0HsI", "0",    "",              } },
                {L_, {"0Hs1", "0",    "1",             } },

                //--  ------  ------  ------ ----- ----
                {L_, {"01st", "01",                    } },
                {L_, {"01sH", "01",                    } },
                {L_, {"01s2", "01",   "2",             } },
                //--  ------  ------  ------ ----- ----
                {L_, {"01Hs", "01",                    } },
                {L_, {"01HI", "01",   "",              } },
                {L_, {"01H2", "01",   "2",             } },
                //--  ------  ------  ------ ----- ----
                {L_, {"012s", "012",                   } },
                {L_, {"012H", "012",                   } },
                {L_, {"0123", "0123",                  } },
            };  // DATA

            enum { DATA_LEN = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < DATA_LEN; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const char        *INPUT    = DATA[ti].d_stringData_p[0];
                const char *const *EXPECTED = DATA[ti].d_stringData_p + 1;

                if (veryVerbose) {
                    T_ P_(LINE) P(INPUT)
                }

                // Validate the input string and tokenizer parameters.

                bool VALID = isValid(INPUT,
                                     SOFT_DELIM_CHARS,
                                     HARD_DELIM_CHARS,
                                     TOKEN_CHARS);
                ASSERTV(LINE, INPUT, true == VALID);

                Obj          mT(INPUT,
                                StringRef(SOFT_DELIM_CHARS),
                                StringRef(HARD_DELIM_CHARS));
                ObjIt        mIt = mT.begin();
                const ObjIt& It  = mIt;

                // Initially 'cursor' is the address of the first token string.

                for (const char * const *cursor = EXPECTED;
                                        *cursor;
                                        ++cursor) {

                    // Extract iteration number, N; used in error reporting.

                    const long int N = cursor - EXPECTED;  // Nth token

                    // Expected token at this iteration, N, of
                    // TokenizerIterator op++:

                    const char *EXP_TOKEN = *cursor;  // current token

                    ObjIt        mCopyIt(mIt);  // copy of initial object
                    const ObjIt& CopyIt  = mCopyIt;

                    const StringRef RET_TOKEN = *It;
                    const StringRef RET_COPY_TOKEN = *CopyIt;

                    if (veryVerbose) {
                        T_ T_ P_(N) P_(EXP_TOKEN) P(RET_TOKEN)
                    }

                    ASSERTV(LINE,
                            N,
                            EXP_TOKEN,
                            RET_TOKEN,
                            EXP_TOKEN == RET_TOKEN);
                    ASSERTV(LINE,
                            N,
                            EXP_TOKEN,
                            RET_COPY_TOKEN,
                            EXP_TOKEN == RET_COPY_TOKEN);

                    ++mIt;

                }  // for current token in input row
            }  // for each row in table
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'reset' METHOD
        //
        // Concerns:
        //: 1 'reset' method re-directs 'Tokenizer' object to new input.
        //:
        //: 2 'reset' method place 'Tokenizer' object to just-constructed
        //:   state.
        //:
        //: 3 'reset' method preserve delimiter sets, supplied at construction.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a sets of input for
        //:   construction and resetting.
        //:
        //: 2 For each row 'R' in the table of P-1 verify that after resetting
        //:   underlying input sequence has been changed and state of the
        //:   object is as if it has been just constructed with new input.
        //:   (C-1..2)
        //:
        //: 3 Set single character as a delimiter and all other characters as
        //:   an input string for object construction.  Reset obtained object
        //:   with the same character set, arranged in the opposite order.
        //:   Verify that delimiter hasn't been changed.  (C-3)
        //:
        //: 4 Verify that defensive checks are addressed.  (C-4)
        //
        // Testing:
        //   void reset(const char *input);
        //   void reset(const StringRef& input);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'reset' METHOD" << endl
                          << "======================" << endl;

        if (verbose) cout << "\nTesting 'reset' method" << endl;

        if (verbose) cout << "\tTesting input string resetting" << endl;
        {
            static const struct {
                int         d_line;            // line number
                const char *d_ctorInput;       // input for constructor
                const char *d_ctorValues[3];   // initial object state
                const char *d_resetInput;      // input for reset method
                const char *d_resetValues[3];  // object state after reset
            } DATA[] = {
            //L#  CTOR   LEADER  TOKEN  DELIM    RESET  LEADER  TOKEN  DELIM
            //    INPUT                          INPUT
            //--  -----  ------  -----  -----    -----  ------  -----  -----
            {L_,  "",    {""                 },  "",    {""                 }},
            {L_,  "",    {""                 },  "s",   {"s"                }},
            {L_,  "",    {""                 },  "H",   {"",    "",    "H"  }},
            {L_,  "",    {""                 },  "0",   {"",    "0",   ""   }},

            {L_,  "s",   {"s"                },  "",    {""                 }},
            {L_,  "s",   {"s"                },  "st",  {"st"               }},
            {L_,  "s",   {"s"                },  "H",   {"",    "",    "H"  }},
            {L_,  "s",   {"s"                },  "0",   {"",    "0",   ""   }},

            {L_,  "H",   {"",    "",    "H"  },  "",    {""                 }},
            {L_,  "H",   {"",    "",    "H"  },  "s",   {"s"                }},
            {L_,  "H",   {"",    "",    "H"  },  "Hs",  {"",    "",    "Hs" }},
            {L_,  "H",   {"",    "",    "H"  },  "0",   {"",    "0",   ""   }},

            {L_,  "0",   {"",    "0",   ""   },  "",    {""                 }},
            {L_,  "0",   {"",    "0",   ""   },  "s",   {"s"                }},
            {L_,  "0",   {"",    "0",   ""   },  "H",   {"",    "",    "H"  }},
            {L_,  "0",   {"",    "0",   ""   },  "01",  {"",    "01",  ""   }},
            };  // DATA

            enum { DATA_LEN = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < DATA_LEN; ++i) {
                const int   LINE         = DATA[i].d_line;
                const char *CTOR_INPUT   = DATA[i].d_ctorInput;
                const char *CTOR_LEADER  = DATA[i].d_ctorValues[0];
                const char *CTOR_TOKEN   = DATA[i].d_ctorValues[1];
                const char *CTOR_DELIM   = DATA[i].d_ctorValues[2];
                const char *RESET_INPUT  = DATA[i].d_resetInput;
                const char *RESET_LEADER = DATA[i].d_resetValues[0];
                const char *RESET_TOKEN  = DATA[i].d_resetValues[1];
                const char *RESET_DELIM  = DATA[i].d_resetValues[2];

                if (veryVerbose) {
                    T_ T_ P_(LINE) P_(CTOR_INPUT) P(RESET_INPUT)
                }

                bool CTOR_INPUT_VALID = isValid(CTOR_INPUT,
                                                SOFT_DELIM_CHARS,
                                                HARD_DELIM_CHARS,
                                                TOKEN_CHARS);
                ASSERTV(LINE, CTOR_INPUT, true == CTOR_INPUT_VALID);

                bool RESET_INPUT_VALID = isValid(RESET_INPUT,
                                                 SOFT_DELIM_CHARS,
                                                 HARD_DELIM_CHARS,
                                                 TOKEN_CHARS);
                ASSERTV(LINE, RESET_INPUT, true == RESET_INPUT_VALID);

                Obj        mT(CTOR_INPUT, SOFT_DELIM_CHARS, HARD_DELIM_CHARS);
                const Obj& T = mT;

                if (CTOR_TOKEN) {
                    ASSERTV(LINE, true        == T.isValid());
                    ASSERTV(LINE, CTOR_LEADER == T.previousDelimiter());
                    ASSERTV(LINE, CTOR_TOKEN  == T.token());
                    ASSERTV(LINE, CTOR_DELIM  == T.trailingDelimiter());
                } else {
                    ASSERTV(LINE, false       == T.isValid());
                    ASSERTV(LINE, CTOR_LEADER == T.previousDelimiter());
                }

                mT.reset(RESET_INPUT);

                if (RESET_TOKEN) {
                    ASSERTV(LINE, true         == T.isValid());
                    ASSERTV(LINE, RESET_LEADER == T.previousDelimiter());
                    ASSERTV(LINE, RESET_TOKEN  == T.token());
                    ASSERTV(LINE, RESET_DELIM  == T.trailingDelimiter());
                } else {
                    ASSERTV(LINE, false        == T.isValid());
                    ASSERTV(LINE, RESET_LEADER == T.previousDelimiter());
                }
            }
        }

        if (verbose) cout << "\tTesting delimiters preservation" << endl;
        {
            char ctorInput[255];
            char resetInput[255];
            char delim[1] = { static_cast<char>(0) };

            for (int i = 1; i < 256; ++i) {
                ctorInput[i-1]      = static_cast<char>(i);
                resetInput[256-i-1] = static_cast<char>(i);
            }

            Obj        mTSoft(ctorInput, StringRef(delim, 1), (StringRef()));
            const Obj& TSoft = mTSoft;
            Obj        mTHard(ctorInput, (StringRef()), StringRef(delim, 1));
            const Obj& THard = mTHard;

            ASSERT(ctorInput == TSoft.token());
            ASSERT(ctorInput == THard.token());

            mTSoft.reset(resetInput);
            mTHard.reset(resetInput);

            ASSERT(resetInput == TSoft.token());
            ASSERT(resetInput == THard.token());
        }

        if (verbose) cout << "\tNegative Testing."<< endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj        mT("", "", "");

            ASSERT_SAFE_FAIL(mT.reset(static_cast<const char*>(0)));
            ASSERT_SAFE_FAIL(mT.reset((StringRef())));
            ASSERT_SAFE_PASS(mT.reset(""));
            ASSERT_SAFE_PASS(mT.reset(StringRef("")));
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // NON-BASIC ACCESSORS
        //   Verify the non-basic accessors functionality.
        //
        // Concerns:
        //: 1 Accessors correctly reflect current state of 'Tokenizer' object.
        //:
        //: 2 Embedded null character is correctly handled by the accessors.
        //
        // Plan:
        //: 1 Take 'Tokenizer' object through different states and verify
        //:   accessors responses.  (C-1)
        //:
        //: 2 Add embedded null character to soft delimiter, hard delimiter
        //:   and token sets respectively and verify accessors responses.
        //:   (C-2)
        //
        // Testing:
        //   bool isValid() const;
        //   bool hasTrailingSoft() const;
        //   bool hasPreviousSoft() const;
        //   bool isTrailingHard() const;
        //   bool isPreviousHard() const;
        //
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "NON-BASIC ACCESSORS" << endl
                          << "===================" << endl;

        if (verbose) cout << "\nTesting accessors." << endl;

        if (verbose) cout << "\tTesting 'isValid'." << endl;
        {
            Obj        invalidMT("", SOFT_DELIM_CHARS, HARD_DELIM_CHARS);
            const Obj& invalidT = invalidMT;

            ASSERT(false == invalidT.isValid());

            Obj        validMT("sH0", SOFT_DELIM_CHARS, HARD_DELIM_CHARS);
            const Obj& validT = validMT;

            ASSERT(true  == validT.isValid());
        }

        if (verbose) cout << "\tTesting 'hasPreviousSoft'/'isPreviousHard'."
                          << endl;
        {
            const int  NUM_ITERATIONS = 6;

            static const struct {
                int         d_line;                    // line number
                const char *d_input;                   // input
                const struct {
                    const char *d_delim;    // previous delimiter
                    bool        d_hasSoft;  // has soft character
                    bool        d_isHard;   // has hard character
                }           d_values[NUM_ITERATIONS];  // values array
            } DATA[] = {
                //LINE  INPUT           PREV    HAS     IS
                //                      DELIM   SOFT    HARD
                //----  -------------   ------  -----   -----
                { L_,   "s",           {{"s",   true,   false }}},

                { L_,   "H",           {{"",    false,  false },
                                        {"H",   false,  true  }}},

                { L_,   "0",           {{"",    false,  false },
                                        {"",    false,  false }}},

                { L_,   "0s1H2tI3Ju",  {{"",    false,  false },
                                        {"s",   true,   false },
                                        {"H",   false,  true  },
                                        {"tI",  true,   true  },
                                        {"Ju",  true,   true  }}},

                { L_,   "s0H1tI2Ju3",  {{"s",   true,   false },
                                        {"H",   false,  true  },
                                        {"tI",  true,   true  },
                                        {"Ju",  true,   true  },
                                        {"",    false,  false  }}},

                { L_,   "s0tH1Iu2J3K", {{"s",   true,   false },
                                        {"tH",  true,   true  },
                                        {"Iu",  true,   true  },
                                        {"J",   false,  true  },
                                        {"K",   false,  true  }}},

                { L_,   "s0tH1Iu2v3",  {{"s",   true,   false },
                                        {"tH",  true,   true  },
                                        {"Iu",  true,   true  },
                                        {"v",   true,   false },
                                        {"",    false,  false }}},

                { L_,   "s0Ht1I2uJ3K", {{"s",   true,   false },
                                        {"Ht",  true,   true  },
                                        {"I",   false,  true  },
                                        {"uJ",  true,   true  },
                                        {"K",   false,  true  }}},
            };

            enum { DATA_LEN = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < DATA_LEN; ++ti) {
                const int   LINE  = DATA[ti].d_line;
                const char *INPUT = DATA[ti].d_input;

                if (veryVerbose) {
                    T_ P_(LINE) P(INPUT)
                }

                ASSERTV(LINE, isValid(INPUT,
                                      SOFT_DELIM_CHARS,
                                      HARD_DELIM_CHARS,
                                      TOKEN_CHARS));

                Obj        mT(INPUT, SOFT_DELIM_CHARS, HARD_DELIM_CHARS);
                const Obj& T = mT;

                for(int i = 0; DATA[ti].d_values[i].d_delim; ++i) {
                    const StringRef DELIM    = DATA[ti].d_values[i].d_delim;
                    const bool      HAS_SOFT = DATA[ti].d_values[i].d_hasSoft;
                    const bool      IS_HARD  = DATA[ti].d_values[i].d_isHard;

                    ASSERTV(LINE, DELIM, DELIM    == T.previousDelimiter());
                    ASSERTV(LINE, DELIM, HAS_SOFT == T.hasPreviousSoft());
                    ASSERTV(LINE, DELIM, IS_HARD  == T.isPreviousHard());

                    if (DATA[ti].d_values[i+1].d_delim) {
                        ++mT;
                    }
                }
            }
        }

        if (verbose) cout << "\tTesting 'hasTrailingSoft'/'isTrailingHard'."
                          << endl;
        {
            const int  NUM_ITERATIONS = 6;

            static const struct {
                int         d_line;                    // line number
                const char *d_input;                   // input
                const struct {
                    const char *d_delim;    // trailing delimiter
                    bool        d_hasSoft;  // has soft character
                    bool        d_isHard;   // has hard character
                }           d_values[NUM_ITERATIONS];  // values array
            } DATA[] = {
                //LINE  INPUT           DELIM   HAS     IS
                //                              SOFT    HARD
                //----  -------------   -----   ------  -----
                { L_,   "0",           {{"",    false,  false }}},
                { L_,   "0s",          {{"s",   true,   false }}},
                { L_,   "0H",          {{"H",   false,  true  }}},
                { L_,   "0sH",         {{"sH",  true,   true  }}},
                { L_,   "0Hs",         {{"Hs",  true,   true  }}},

                // Ad-hoc tests
                { L_,   "H0s1tI2Ju3",  {{"H",   false,  true  },
                                        {"s",   true,   false },
                                        {"tI",  true,   true  },
                                        {"Ju",  true,   true  },
                                        {"",    false,  false }}},

                { L_,   "0s1tH2Iu3J",  {{"s",   true,   false },
                                        {"tH",  true,   true  },
                                        {"Iu",  true,   true  },
                                        {"J",   false,  true  }}},

                { L_,   "0sH1It2J3u",  {{"sH",  true,   true  },
                                        {"It",  true,   true  },
                                        {"J",   false,  true  },
                                        {"u",   true,   false }}},


                { L_,   "Hs0I1t2uJ3K", {{"Hs",  true,   true  },
                                        {"I",   false,  true  },
                                        {"t",   true,   false },
                                        {"uJ",  true,   true  },
                                        {"K",   false,  true  }}},
            };

            enum { DATA_LEN = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < DATA_LEN; ++ti) {
                const int   LINE  = DATA[ti].d_line;
                const char *INPUT = DATA[ti].d_input;

                if (veryVerbose) {
                    T_ P_(LINE) P(INPUT)
                }

                ASSERTV(LINE, isValid(INPUT,
                                      SOFT_DELIM_CHARS,
                                      HARD_DELIM_CHARS,
                                      TOKEN_CHARS));

                Obj        mT(INPUT, SOFT_DELIM_CHARS, HARD_DELIM_CHARS);
                const Obj& T = mT;

                for (int i = 0; DATA[ti].d_values[i].d_delim; ++i) {
                    const StringRef DELIM    = DATA[ti].d_values[i].d_delim;
                    const bool      HAS_SOFT = DATA[ti].d_values[i].d_hasSoft;
                    const bool      IS_HARD  = DATA[ti].d_values[i].d_isHard;

                    ASSERTV(LINE, DELIM, DELIM    == T.trailingDelimiter());
                    ASSERTV(LINE, DELIM, HAS_SOFT == T.hasTrailingSoft());
                    ASSERTV(LINE, DELIM, IS_HARD  == T.isTrailingHard());

                    ++mT;
                }
            }
        }

        if (verbose) cout << "\tTesting inputs having embedded null character."
                          << endl;
        {
            const char      INPUT_DATA[] = {'\0', '0', '\0'};
            const StringRef INPUT(INPUT_DATA, 3);
            const StringRef NULL_STRING_REF("\0", 1);
            {
                // soft delimiter with embedded null

                Obj        mT(INPUT, NULL_STRING_REF, "");
                const Obj& T = mT;
                ASSERT(NULL_STRING_REF == T.previousDelimiter());
                ASSERT("0"             == T.token());
                ASSERT(NULL_STRING_REF == T.trailingDelimiter());
                ASSERT(true            == T.hasPreviousSoft());
                ASSERT(false           == T.isPreviousHard());
                ASSERT(true            == T.hasTrailingSoft());
                ASSERT(false           == T.isTrailingHard());
            }

            {
                // hard delimiter with embedded null

                Obj        mT(INPUT, "", NULL_STRING_REF);
                const Obj& T = mT;
                ASSERT(""              == T.previousDelimiter());
                ASSERT(""              == T.token());
                ASSERT(NULL_STRING_REF == T.trailingDelimiter());
                ASSERT(false           == T.hasPreviousSoft());
                ASSERT(false           == T.isPreviousHard());
                ASSERT(false           == T.hasTrailingSoft());
                ASSERT(true            == T.isTrailingHard());
            }

            {
                // token with embedded null

                Obj        mT(INPUT, "", "");
                const Obj& T = mT;
                ASSERT(""              == T.previousDelimiter());
                ASSERT(INPUT           == T.token());
                ASSERT(""              == T.trailingDelimiter());
                ASSERT(false           == T.hasPreviousSoft());
                ASSERT(false           == T.isPreviousHard());
                ASSERT(false           == T.hasTrailingSoft());
                ASSERT(false           == T.isTrailingHard());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS AND BASIC ACCESSORS
        //   This test will verify that the primary manipulators are working as
        //   expected.  As basic accessors should be tested exactly the same
        //   way, these two tests have been united.  So we test that the basic
        //   accessors are working as expected also.
        //
        // Concerns:
        //: 1 All (including internal) relevant states can be reached with
        //:   primary manipulators.
        //:
        //: 2 Multiple distinct characters of the same type are handled
        //:   correctly.
        //:
        //: 3 Repeated characters are handled correctly.
        //:
        //: 4 The null character is handled the same way as any other
        //:   character.
        //:
        //: 5 Non-ASCII characters are handled the same way as ASCII ones.
        //:
        //: 6 Inputs requiring many iterations succeed.
        //:
        //: 7 Inputs having large tokens/delimiters succeed.
        //:
        //: 8 QoI: asserted precondition violations are detected when enabled.
        //:
        //: 9 Accessors return references to expected character sequences.
        //:
        //:10 'Tokenizer' object can be destroyed.
        //
        // Plan:
        //: 1 Using the table-driven technique, apply depth-ordered enumeration
        //:   on the length of the input string to parse all unique inputs (in
        //:   lexicographic order) up to a "depth" of 4 (note that we have
        //:   hard-coded "stuv" to be set of soft delimiter characters, and
        //:   "HIJK" to be the set of hard ones, leaving the digit characters
        //:   "0123" to be used as unique token characters).  The input string
        //:   as well as the sequence of expected "parsed" strings will be
        //:   provided -- each on a single row of the table.  Failing to supply
        //:   a token (followed by its trailing delimiter) implies that the
        //:   iterator has become invalid (which is tested) after the internal
        //:   iteration loop exits.  Verify each state of 'Tokenizer' object
        //:   with basic accessors.  (C-1..2, 9)
        //:
        //: 2 Allow 'Tokenizer' object to leave the scope.  (C-10)
        //:
        //: 3 Additional add-hoc tests are provided to address remaining
        //:   concerns.  (C-3..7)
        //:
        //: 4 Verify that defensive checks are addressed.  (C-8)
        //
        // Testing:
        //   Tokenizer(const char *, const StringRef&, const StringRef&);
        //   Tokenizer(const StringRef&, const StringRef&, const StringRef&);
        //   Tokenizer(const char *, const StringRef&);
        //   Tokenizer(const StringRef&, const StringRef&);
        //   Tokenizer& operator++();
        //   StringRef trailingDelimiter() const;
        //   StringRef previousDelimiter() const;
        //   StringRef token() const;
        //   ~Tokenizer();
        // --------------------------------------------------------------------

        if (verbose) cout
                         << endl
                         << "PRIMARY MANIPULATORS AND BASIC ACCESSORS" << endl
                         << "========================================" << endl;

        if (verbose) cout <<
                "\nTesting all input combinations to a depth of four." << endl;
        {
            enum { MAX_ITER = 4 };  // Maximum iterations

            static const struct {
                int         d_line;                         // line number
                const char *d_stringData_p[3 + 2*MAX_ITER]; // input + expected
            } DATA[] = {
  //            _________________Expected Parse of INPUT___________________
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
  {L_, {"0Hs",  "",     "0",    "Hs"                                       } },
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
  {L_, {"0stH", "",     "0",    "stH"                                      } },
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

            for (int ti = 0; ti < DATA_LEN; ++ti) {
                const int          LINE     = DATA[ti].d_line;
                const char        *INPUT    = DATA[ti].d_stringData_p[0];
                const char *const *EXPECTED = DATA[ti].d_stringData_p + 1;

                if (veryVerbose) {
                    T_ P_(LINE) P(INPUT)
                }

                // Validate the input string and tokenizer parameters.

                bool VALID = isValid(INPUT,
                                     SOFT_DELIM_CHARS,
                                     HARD_DELIM_CHARS,
                                     TOKEN_CHARS);
                ASSERTV(LINE, INPUT, true == VALID);

                Obj        mT3(INPUT,
                               StringRef(SOFT_DELIM_CHARS),
                               StringRef(HARD_DELIM_CHARS));

                Obj        mT2(INPUT,
                               StringRef(SOFT_DELIM_CHARS));

                for (int i = 0; i < 2; ++i) {
                    Obj       &mT = (!(i % 2)) ? mT3 : mT2;
                    const Obj &T = mT;

                    // We need to check input appropriateness for
                    // two-parameters constructor.

                    if (i % 2) {
                        if (strlen(INPUT) != strcspn(INPUT,
                                                     HARD_DELIM_CHARS)) {
                            continue;  // input contains hard delimiters
                        }
                    }

                    // Initially 'cursor' is the address of the first token
                    // string.

                    for (const char * const *cursor = EXPECTED+1;
                                            *cursor;
                                            cursor += 2) {

                        ASSERTV(LINE, T.isValid());

                        // Extract iteration number, N; used in error
                        // reporting.

                        const long int N = (cursor - EXPECTED) / 2;

                        // Expected values of Tokenizer's op++ at (this)
                        // iteration:

                        const char *EXP_PREV  = cursor[-1];  // previous delim
                        const char *EXP_TOKEN = cursor[ 0];  // current token
                        const char *EXP_POST  = cursor[+1];  // trailing delim

                        // Shorten for better error messages.

                        const StringRef RET_PREV  = T.previousDelimiter();
                        const StringRef RET_TOKEN = T.token();
                        const StringRef RET_POST  = T.trailingDelimiter();

                        if (!(i % 2)) {
                            // Printing result for three-parameters constructor

                            if (veryVerbose) {
                                T_ T_ P(N)
                                T_ T_ P_(EXP_PREV)  P(RET_PREV)
                                T_ T_ P_(EXP_TOKEN) P(RET_TOKEN)
                                T_ T_ P_(EXP_POST)  P(RET_POST)
                            }
                        } else {
                            // Printing result for two-parameters constructor

                            if (veryVerbose) {
                                T_ T_ T_ T_ T_ T_ P(N)
                                T_ T_ T_ T_ T_ T_ P_(EXP_PREV)  P(RET_PREV)
                                T_ T_ T_ T_ T_ T_ P_(EXP_TOKEN) P(RET_TOKEN)
                                T_ T_ T_ T_ T_ T_ P_(EXP_POST)  P(RET_POST)
                            }
                        }

                        ASSERTV(LINE,
                                N,
                                EXP_PREV,
                                RET_PREV,
                                EXP_PREV  == RET_PREV);
                        ASSERTV(LINE,
                                N,
                                EXP_TOKEN,
                                RET_TOKEN,
                                EXP_TOKEN == RET_TOKEN);
                        ASSERTV(LINE,
                                N,
                                EXP_POST,
                                RET_POST,
                                EXP_POST  == RET_POST);

                        ++mT;

                    }  // for current token in input row

                }  // for each constructor

            }  // for each row in table
        }

        if (verbose) cout <<
                            "\nAd hoc testing of various character properties."
                          << endl;
        {
            if (verbose) cout << "\tTesting repeated characters." << endl;
            {
                char      softDelim[3] = {"ss"};
                char      token[3]     = {"00"};
                char      hardDelim[2] = {"H"};
                char      input[11]    = {"ss00H00ss"};

                Obj        mT(input,
                              StringRef(softDelim),
                              StringRef(hardDelim));
                const Obj& T = mT;

                ASSERT(softDelim == T.previousDelimiter());
                ASSERT(token     == T.token());
                ASSERT(hardDelim == T.trailingDelimiter());

                ++mT;

                ASSERT(hardDelim == T.previousDelimiter());
                ASSERT(token     == T.token());
                ASSERT(softDelim == T.trailingDelimiter());
            }

            if (verbose) cout << "\tTesting embedded null character." << endl;
            {
                const char      INPUT_DATA[] = {'\0', '0', '\0'};
                const StringRef INPUT(INPUT_DATA, 3);
                const StringRef NULL_STRING_REF("\0", 1);
                {
                    // soft delimiter with embedded null

                    Obj        mT(INPUT, NULL_STRING_REF, "");
                    const Obj& T = mT;
                    ASSERT(NULL_STRING_REF == T.previousDelimiter());
                    ASSERT("0"             == T.token());
                    ASSERT(NULL_STRING_REF == T.trailingDelimiter());
                }

                {
                    // hard delimiter with embedded null

                    Obj        mT(INPUT, "", NULL_STRING_REF);
                    const Obj& T = mT;
                    ASSERT(""              == T.previousDelimiter());
                    ASSERT(""              == T.token());
                    ASSERT(NULL_STRING_REF == T.trailingDelimiter());
                }

                {
                    // token with embedded null

                    Obj        mT(INPUT, "", "");
                    const Obj& T = mT;
                    ASSERT(""              == T.previousDelimiter());
                    ASSERT(INPUT           == T.token());
                    ASSERT(""              == T.trailingDelimiter());
                }
            }

            if (verbose) cout << "\tTesting Non-ASCII characters." << endl;
            {
                {
                    // Testing Non-ASCII characters as soft delimiters.

                    char delim[129];
                    char input[130];

                    for (int i = 0; i < 128; ++i) {
                        delim[i] = static_cast<char>(i + 128);
                        input[i] = static_cast<char>(i + 128);
                    }

                    delim[128] = 0;    // end of delim string
                    input[128] = '0';  // token symbol
                    input[129] = 0;    // end of input string

                    Obj        mT(input, StringRef(delim), StringRef(""));
                    const Obj& T = mT;

                    ASSERT(StringRef(delim) == T.previousDelimiter());
                    ASSERT("0"              == T.token());
                    ASSERT(""               == T.trailingDelimiter());
                }

                {
                    // Testing Non-ASCII characters as hard delimiters.

                    char input[129];

                    for (int i = 0; i < 128; ++i) {
                        input[i] = static_cast<char>(i + 128);
                    }

                    input[128] = 0;    // end of input string

                    Obj        mT(input, StringRef(""), StringRef(input));
                    const Obj& T = mT;

                    for (int i = 0; i < 128; ++i) {
                        const StringRef EXP_DELIM(input + i, 1);

                        if (veryVerbose) {
                            T_ P(i)
                        }

                        ASSERTV(i, EXP_DELIM == T.trailingDelimiter());
                        ASSERTV(i, ""        == T.token());

                        ++mT;
                    }
                }

                {
                    // Testing Non-ASCII characters as tokens.

                    char input[129];

                    for (int i = 0; i < 128; ++i) {
                        input[i] = static_cast<char>(i + 128);
                    }

                    input[128] = 0;    // end of input string

                    Obj        mT(input, StringRef(""), StringRef(""));
                    const Obj& T = mT;

                    ASSERT(""               == T.previousDelimiter());
                    ASSERT(StringRef(input) == T.token());
                    ASSERT(""               == T.trailingDelimiter());
                }
            }
        }

        if (verbose) cout <<
                  "\nAd hoc 'stress' testing for iterations and size." << endl;
        {
            if (verbose) cout << "\tTesting inputs requiring many iterations."
                              << endl;
            {
                const int   INPUT_SIZE = 1000;
                char        softInput[INPUT_SIZE+1];
                char        hardInput[INPUT_SIZE+1];

                // softInput string: "s0t1u2v3s0t1u ... 2v3s0t1u2"
                // hardInput string: "0H1I2J3K0H1I2 ... J3K0H1I2J"

                for (int i = 0; i < INPUT_SIZE; i+=2) {
                    const int INDEX = (i/2)%4;
                    softInput[i]   = SOFT_DELIM_CHARS[INDEX];
                    softInput[i+1] =      TOKEN_CHARS[INDEX];
                    hardInput[i]   =      TOKEN_CHARS[INDEX];
                    hardInput[i+1] = HARD_DELIM_CHARS[INDEX];
                }
                softInput[INPUT_SIZE] = 0;
                hardInput[INPUT_SIZE] = 0;

                Obj        softMT(softInput,
                                  StringRef(SOFT_DELIM_CHARS),
                                  StringRef(""));
                const Obj& softT = softMT;

                Obj        hardMT(hardInput,
                                  StringRef(""),
                                  StringRef(HARD_DELIM_CHARS));
                const Obj& hardT = hardMT;

                for (int i = 0; i < INPUT_SIZE; i+=2) {
                    const int       INDEX      = (i/2)%4;
                    const StringRef SOFT_PREV  = softT.previousDelimiter();
                    const StringRef SOFT_TOKEN = softT.token();
                    const StringRef HARD_DELIM = hardT.trailingDelimiter();
                    const StringRef HARD_TOKEN = hardT.token();

                    const StringRef SOFT_PREV_EXP  =
                                        StringRef(SOFT_DELIM_CHARS + INDEX, 1);
                    const StringRef SOFT_TOKEN_EXP =
                                        StringRef(     TOKEN_CHARS + INDEX, 1);
                    const StringRef HARD_DELIM_EXP =
                                        StringRef(HARD_DELIM_CHARS + INDEX, 1);
                    const StringRef HARD_TOKEN_EXP =
                                        StringRef(     TOKEN_CHARS + INDEX, 1);

                    if (veryVerbose) {
                        T_ P_(i) P_(INDEX) P(INDEX)
                        T_ T_ P_(SOFT_PREV_EXP)  P(SOFT_PREV_EXP)
                        T_ T_ P_(SOFT_TOKEN_EXP) P(SOFT_TOKEN)
                        T_ T_ P_(HARD_DELIM_EXP) P(HARD_DELIM)
                        T_ T_ P_(HARD_TOKEN_EXP) P(HARD_TOKEN)
                    }


                    ASSERTV(i, INDEX, SOFT_PREV_EXP  == SOFT_PREV);
                    ASSERTV(i, INDEX, SOFT_TOKEN_EXP == SOFT_TOKEN);
                    ASSERTV(i, INDEX, HARD_DELIM_EXP == HARD_DELIM);
                    ASSERTV(i, INDEX, HARD_TOKEN_EXP == HARD_TOKEN);

                    ++softMT;
                    ++hardMT;
                }
            }

            if (verbose) cout <<
                             "\tTesting inputs having large tokens/delimeters."
                              << endl;
            {
                const int BUF_SIZE = 1000;
                char      softDelim[BUF_SIZE+1];  // "sss...sss"
                char      token[BUF_SIZE+1];      // "000...000"
                char      hardDelim[2];           // "H"
                char      input[(4*BUF_SIZE)+2];  // "s...s0...0H0...0s...s

                for (int i = 0; i < BUF_SIZE; ++i) {
                    softDelim[i]                = 's';
                    token[i]                    = '0';

                    input[i]                    = 's';
                    input[BUF_SIZE + i]         = '0';
                    input[(2*BUF_SIZE) + i + 1] = '0';
                    input[(3*BUF_SIZE) + i + 1] = 's';
                }
                softDelim[BUF_SIZE]   = 0;
                token[BUF_SIZE]       = 0;
                hardDelim[0]          = 'H';
                hardDelim[1]          = 0;
                input[(2*BUF_SIZE)]   = 'H';
                input[(4*BUF_SIZE)+1] = 0;

                Obj        mT(input,
                              StringRef(softDelim),
                              StringRef(hardDelim));
                const Obj& T = mT;

                ASSERT(softDelim == T.previousDelimiter());
                ASSERT(token     == T.token());
                ASSERT(hardDelim == T.trailingDelimiter());

                ++mT;

                ASSERT(hardDelim == T.previousDelimiter());
                ASSERT(token     == T.token());
                ASSERT(softDelim == T.trailingDelimiter());
            }
        }

        if (verbose) cout << "\nNegative Testing."<< endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (verbose) cout << "\tTesting null input." << endl;

            ASSERT_SAFE_FAIL(Obj(static_cast<const char*>(0), "", ""));
            ASSERT_SAFE_FAIL(Obj((StringRef()), "", ""));
            ASSERT_SAFE_PASS(Obj("", "", ""));
            ASSERT_SAFE_PASS(Obj(StringRef(""), (StringRef())));
            ASSERT_SAFE_PASS(Obj(StringRef(""), (StringRef()), (StringRef())));

            if (verbose) cout << "\tTesting iterating from an invalid state"
                              << endl;

            ASSERT_SAFE_FAIL(++Obj("", "", ""));
            ASSERT_SAFE_PASS(++Obj("0", "", ""));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //   Verify the auxiliary function used in the test driver. Note that
        //   the tested function is not part of the component and used only to
        //   check test inputs correctness.
        //
        // Concerns:
        //: 1 There are no duplicates in the soft, hard and token character
        //:  sets.
        //:
        //: 2 There are no duplicates in the input character set.
        //:
        //: 3 All characters in the input string occur in the same relative
        //:   order as they do in each of the respective character sets and
        //:   that they do NOT skip over any characters in those respective
        //:   sets.
        //:
        //: 4 Only characters from the respective character sets appear in the
        //:   input string.
        //
        // Plan:
        //: 1 Using the table-driven technique, test function on various input
        //:   strings containing both valid and invalid character sequences.
        //:   (C-1..4)
        //
        // Testing:
        //   TEST APPARATUS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;
        if (verbose) cout << "\nChecking input verification." << endl;
        {

            static const struct {
                int         d_line;         // line number
                const char *d_soft;         // list of soft delimiters
                const char *d_hard;         // list of hard delimiters
                const char *d_token;        // list of tokens
                const char *d_input;        // input string
                bool        d_isValid;      // expected tokenizer validity
            } DATA[] = {
                //LINE  SOFT    HARD    TOKEN   INPUT   VALID
                //----  ------  ------  ------  ------  -----
                // Depth-ordered test.
                // Depth 0.
                { L_,   "",     "",     "",     "",     true  },
                // Depth 1.
                { L_,   "s",    "",     "",     "",     true  },
                { L_,   "",     "H",    "",     "",     true  },
                { L_,   "",     "",     "0",    "",     true  },
                { L_,   "",     "",     "",     "a",    false },
                // Depth 2.
                { L_,   "ss",   "",     "",     "",     false },
                { L_,   "st",   "",     "",     "",     true  },
                { L_,   "s",    "s",    "",     "",     false },
                { L_,   "s",    "H",    "",     "",     true  },
                { L_,   "s",    "",     "s",    "",     false },
                { L_,   "s",    "",     "0",    "",     true  },
                { L_,   "s",    "",     "",     "s",    true  },
                { L_,   "s",    "",     "",     "a",    false },
                { L_,   "",     "HH",   "",     "",     false },
                { L_,   "",     "HI",   "",     "",     true  },
                { L_,   "",     "H",    "H",    "",     false },
                { L_,   "",     "H",    "0",    "",     true  },
                { L_,   "",     "H",    "",     "H",    true  },
                { L_,   "",     "H",    "",     "a",    false },
                { L_,   "",     "",     "00",   "",     false },
                { L_,   "",     "",     "01",   "",     true  },
                { L_,   "",     "",     "0",    "0",    true  },
                { L_,   "",     "",     "0",    "a",    false },
                // Depth 3.
                { L_,   "sss",  "",     "",     "",     false },
                { L_,   "sst",  "",     "",     "",     false },
                { L_,   "sts",  "",     "",     "",     false },
                { L_,   "stt",  "",     "",     "",     false },
                { L_,   "stu",  "",     "",     "",     true  },
                { L_,   "ss",   "s",    "",     "",     false },
                { L_,   "ss",   "H",    "",     "",     false },
                { L_,   "st",   "s",    "",     "",     false },
                { L_,   "st",   "t",    "",     "",     false },
                { L_,   "st",   "H",    "",     "",     true  },
                { L_,   "s",    "ss",   "",     "",     false },
                { L_,   "s",    "sH",   "",     "",     false },
                { L_,   "s",    "Hs",   "",     "",     false },
                { L_,   "s",    "HH",   "",     "",     false },
                { L_,   "s",    "HI",   "",     "",     true  },
                { L_,   "s",    "H",    "s",    "",     false },
                { L_,   "s",    "H",    "H",    "",     false },
                { L_,   "s",    "H",    "0",    "",     true  },
                { L_,   "s",    "H",    "",     "s",    true  },
                { L_,   "s",    "H",    "",     "H",    true  },
                { L_,   "s",    "H",    "",     "a",    false },
                { L_,   "s",    "",     "ss",   "",     false },
                { L_,   "s",    "",     "s0",   "",     false },
                { L_,   "s",    "",     "0s",   "",     false },
                { L_,   "s",    "",     "00",   "",     false },
                { L_,   "s",    "",     "01",   "",     true  },
                { L_,   "s",    "",     "0",    "s",    true  },
                { L_,   "s",    "",     "0",    "0",    true  },
                { L_,   "s",    "",     "0",    "a",    false },
                { L_,   "",     "HH",   "H",    "",     false },
                { L_,   "",     "HH",   "0",    "",     false },
                { L_,   "",     "HI",   "H",    "",     false },
                { L_,   "",     "HI",   "I",    "",     false },
                { L_,   "",     "HI",   "0",    "",     true  },
                { L_,   "",     "HI",   "",     "H",    true  },
                { L_,   "",     "HI",   "",     "I",    false },
                { L_,   "",     "HI",   "",     "a",    false },
                { L_,   "",     "H",    "H",    "H",    false },
                { L_,   "",     "H",    "H",    "a",    false },
                { L_,   "",     "H",    "0",    "H",    true  },
                { L_,   "",     "H",    "0",    "0",    true  },
                { L_,   "",     "H",    "0",    "a",    false },
                { L_,   "",     "H",    "",     "HH",   false },
                { L_,   "",     "H",    "",     "Ha",   false },
                { L_,   "",     "H",    "",     "aH",   false },
                { L_,   "",     "",     "00",   "0",    false },
                { L_,   "",     "",     "00",   "a",    false },
                { L_,   "",     "",     "01",   "0",    true  },
                { L_,   "",     "",     "01",   "1",    false },
                { L_,   "",     "",     "01",   "a",    false },
                { L_,   "",     "",     "0",    "00",   false },
                { L_,   "",     "",     "0",    "0a",   false },
                { L_,   "",     "",     "0",    "a0",   false },
                { L_,   "",     "",     "0",    "aa",   false },
                // Depth-ordered test with single character in each input type
                // Depth 0.
                { L_,   "s",    "H",    "0",    "",     true  },
                // Depth 1.
                { L_,   "s",    "H",    "0",    "s",    true  },
                { L_,   "s",    "H",    "0",    "H",    true  },
                { L_,   "s",    "H",    "0",    "0",    true  },
                { L_,   "s",    "H",    "0",    "a",    false },
                // Depth 2.
                { L_,   "s",    "H",    "0",    "ss",   false },
                { L_,   "s",    "H",    "0",    "sH",   true  },
                { L_,   "s",    "H",    "0",    "s0",   true  },
                { L_,   "s",    "H",    "0",    "sa",   false },
                { L_,   "s",    "H",    "0",    "Hs",   true  },
                { L_,   "s",    "H",    "0",    "HH",   false },
                { L_,   "s",    "H",    "0",    "H0",   true  },
                { L_,   "s",    "H",    "0",    "Ha",   false },
                { L_,   "s",    "H",    "0",    "0s",   true  },
                { L_,   "s",    "H",    "0",    "0H",   true  },
                { L_,   "s",    "H",    "0",    "00",   false },
                { L_,   "s",    "H",    "0",    "0a",   false },
                { L_,   "s",    "H",    "0",    "as",   false },
                { L_,   "s",    "H",    "0",    "aH",   false },
                { L_,   "s",    "H",    "0",    "a0",   false },
                { L_,   "s",    "H",    "0",    "aa",   false },
                // Depth 3.
                { L_,   "s",    "H",    "0",    "sss",  false },
                { L_,   "s",    "H",    "0",    "ssH",  false },
                { L_,   "s",    "H",    "0",    "ss0",  false },
                { L_,   "s",    "H",    "0",    "ssa",  false },
                { L_,   "s",    "H",    "0",    "sHs",  false },
                { L_,   "s",    "H",    "0",    "sHH",  false },
                { L_,   "s",    "H",    "0",    "sH0",  true  },
                { L_,   "s",    "H",    "0",    "sHa",  false },
                { L_,   "s",    "H",    "0",    "s0s",  false },
                { L_,   "s",    "H",    "0",    "s0H",  true  },
                { L_,   "s",    "H",    "0",    "s00",  false },
                { L_,   "s",    "H",    "0",    "s0a",  false },
                { L_,   "s",    "H",    "0",    "sas",  false },
                { L_,   "s",    "H",    "0",    "saH",  false },
                { L_,   "s",    "H",    "0",    "sa0",  false },
                { L_,   "s",    "H",    "0",    "saa",  false },
                { L_,   "s",    "H",    "0",    "Hss",  false },
                { L_,   "s",    "H",    "0",    "HsH",  false },
                { L_,   "s",    "H",    "0",    "Hs0",  true  },
                { L_,   "s",    "H",    "0",    "Hsa",  false },
                { L_,   "s",    "H",    "0",    "HHs",  false },
                { L_,   "s",    "H",    "0",    "HHH",  false },
                { L_,   "s",    "H",    "0",    "HH0",  false },
                { L_,   "s",    "H",    "0",    "HHa",  false },
                { L_,   "s",    "H",    "0",    "H0s",  true  },
                { L_,   "s",    "H",    "0",    "H0H",  false },
                { L_,   "s",    "H",    "0",    "H00",  false },
                { L_,   "s",    "H",    "0",    "H0a",  false },
                { L_,   "s",    "H",    "0",    "Has",  false },
                { L_,   "s",    "H",    "0",    "HaH",  false },
                { L_,   "s",    "H",    "0",    "Ha0",  false },
                { L_,   "s",    "H",    "0",    "Haa",  false },
                { L_,   "s",    "H",    "0",    "0ss",  false },
                { L_,   "s",    "H",    "0",    "0sH",  true  },
                { L_,   "s",    "H",    "0",    "0s0",  false },
                { L_,   "s",    "H",    "0",    "0sa",  false },
                { L_,   "s",    "H",    "0",    "0Hs",  true  },
                { L_,   "s",    "H",    "0",    "0HH",  false },
                { L_,   "s",    "H",    "0",    "0H0",  false },
                { L_,   "s",    "H",    "0",    "0Ha",  false },
                { L_,   "s",    "H",    "0",    "00s",  false },
                { L_,   "s",    "H",    "0",    "00H",  false },
                { L_,   "s",    "H",    "0",    "000",  false },
                { L_,   "s",    "H",    "0",    "00a",  false },
                { L_,   "s",    "H",    "0",    "0as",  false },
                { L_,   "s",    "H",    "0",    "0aH",  false },
                { L_,   "s",    "H",    "0",    "0a0",  false },
                { L_,   "s",    "H",    "0",    "0aa",  false },
                { L_,   "s",    "H",    "0",    "ass",  false },
                { L_,   "s",    "H",    "0",    "asH",  false },
                { L_,   "s",    "H",    "0",    "as0",  false },
                { L_,   "s",    "H",    "0",    "asa",  false },
                { L_,   "s",    "H",    "0",    "aHs",  false },
                { L_,   "s",    "H",    "0",    "aHH",  false },
                { L_,   "s",    "H",    "0",    "aH0",  false },
                { L_,   "s",    "H",    "0",    "aHa",  false },
                { L_,   "s",    "H",    "0",    "a0s",  false },
                { L_,   "s",    "H",    "0",    "a0H",  false },
                { L_,   "s",    "H",    "0",    "a00",  false },
                { L_,   "s",    "H",    "0",    "a0a",  false },
                { L_,   "s",    "H",    "0",    "aas",  false },
                { L_,   "s",    "H",    "0",    "aaH",  false },
                { L_,   "s",    "H",    "0",    "aa0",  false },
                { L_,   "s",    "H",    "0",    "aaa",  false },
                // Depth-ordered test with double characters in each input type
                // Depth 0.
                { L_,   "st",   "HI",   "01",   "",     true  },
                // Depth 1.
                { L_,   "st",   "HI",   "01",   "s",    true  },
                { L_,   "st",   "HI",   "01",   "t",    false },
                { L_,   "st",   "HI",   "01",   "H",    true  },
                { L_,   "st",   "HI",   "01",   "I",    false },
                { L_,   "st",   "HI",   "01",   "0",    true  },
                { L_,   "st",   "HI",   "01",   "1",    false },
                { L_,   "st",   "HI",   "01",   "a",    false },
                // Depth 2.
                { L_,   "st",   "HI",   "01",   "ss",   false },
                { L_,   "st",   "HI",   "01",   "st",   true  },
                { L_,   "st",   "HI",   "01",   "sH",   true  },
                { L_,   "st",   "HI",   "01",   "sI",   false },
                { L_,   "st",   "HI",   "01",   "s0",   true  },
                { L_,   "st",   "HI",   "01",   "s1",   false },
                { L_,   "st",   "HI",   "01",   "ts",   false },
                { L_,   "st",   "HI",   "01",   "tt",   false },
                { L_,   "st",   "HI",   "01",   "tH",   false },
                { L_,   "st",   "HI",   "01",   "tI",   false },
                { L_,   "st",   "HI",   "01",   "t0",   false },
                { L_,   "st",   "HI",   "01",   "t1",   false },
                { L_,   "st",   "HI",   "01",   "Hs",   true  },
                { L_,   "st",   "HI",   "01",   "Ht",   false },
                { L_,   "st",   "HI",   "01",   "HH",   false },
                { L_,   "st",   "HI",   "01",   "HI",   true  },
                { L_,   "st",   "HI",   "01",   "H0",   true  },
                { L_,   "st",   "HI",   "01",   "H1",   false },
                { L_,   "st",   "HI",   "01",   "Is",   false },
                { L_,   "st",   "HI",   "01",   "It",   false },
                { L_,   "st",   "HI",   "01",   "IH",   false },
                { L_,   "st",   "HI",   "01",   "II",   false },
                { L_,   "st",   "HI",   "01",   "I0",   false },
                { L_,   "st",   "HI",   "01",   "I1",   false },
                { L_,   "st",   "HI",   "01",   "0s",   true  },
                { L_,   "st",   "HI",   "01",   "0t",   false },
                { L_,   "st",   "HI",   "01",   "0H",   true  },
                { L_,   "st",   "HI",   "01",   "0I",   false },
                { L_,   "st",   "HI",   "01",   "00",   false },
                { L_,   "st",   "HI",   "01",   "01",   true  },
                { L_,   "st",   "HI",   "01",   "1s",   false },
                { L_,   "st",   "HI",   "01",   "1t",   false },
                { L_,   "st",   "HI",   "01",   "1H",   false },
                { L_,   "st",   "HI",   "01",   "1I",   false },
                { L_,   "st",   "HI",   "01",   "10",   false },
                { L_,   "st",   "HI",   "01",   "11",   false },

                // Ad-hoc alternative testing.  The test table uses previous
                // patterns to elide already tested combinations.  Uses
                // white-box testing strategy ( known implementation ).

                // Testing soft delimiter validity
                { L_,   "s",    "",     "",     "",     true  },
                { L_,   "ss",   "",     "",     "",     false },
                { L_,   "st",   "",     "",     "",     true  },
                { L_,   "sts",  "",     "",     "",     false },
                { L_,   "stu",  "",     "",     "",     true  },
                { L_,   "stus", "",     "",     "",     false },
                { L_,   "stuv", "",     "",     "",     true  },

                //Testing hard delimiter validity
                { L_,   "",     "H",    "",     "",     true  },
                { L_,   "",     "HH",   "",     "",     false },
                { L_,   "",     "HI",   "",     "",     true  },
                { L_,   "",     "HIH",  "",     "",     false },
                { L_,   "",     "HIJ",  "",     "",     true  },
                { L_,   "",     "HIJH", "",     "",     false },
                { L_,   "",     "HIJK", "",     "",     true  },

                // Testing token set validity
                { L_,   "",     "",     "0",    "",     true  },
                { L_,   "",     "",     "00",   "",     false },
                { L_,   "",     "",     "01",   "",     true  },
                { L_,   "",     "",     "010",  "",     false },
                { L_,   "",     "",     "012",  "",     true  },
                { L_,   "",     "",     "0120", "",     false },
                { L_,   "",     "",     "0123", "",     true  },

                // Testing input (duplicates only) validity
                { L_,   "",     "",     "",     "aa",   false },
                { L_,   "",     "",     "",     "aba",  false },
                { L_,   "",     "",     "",     "abca", false },
                { L_,   "",     "",     "",     "abcb", false },

                // Testing duplicates in delimiters and token sets
                { L_,   "s",    "s",    "",     "",     false },
                { L_,   "s",    "",     "s",    "",     false },
                { L_,   "",     "s",    "s",    "",     false },
                { L_,   "s",    "s",    "s",    "",     false },
                { L_,   "s",    "H",    "s",    "",     false },
                { L_,   "s",    "H",    "H",    "",     false },
                { L_,   "H",    "s",    "H",    "",     false },
                { L_,   "st",   "HI",   "s",    "",     false },
                { L_,   "st",   "HI",   "t",    "",     false },
                { L_,   "st",   "HI",   "H",    "",     false },
                { L_,   "st",   "HI",   "I",    "",     false },
                { L_,   "st",   "s",    "01",   "",     false },
                { L_,   "st",   "t",    "01",   "",     false },
                { L_,   "st",   "0",    "01",   "",     false },
                { L_,   "st",   "1",    "01",   "",     false },
                { L_,   "H",    "HI",   "01",   "",     false },
                { L_,   "I",    "HI",   "01",   "",     false },
                { L_,   "0",    "HI",   "01",   "",     false },
                { L_,   "1",    "HI",   "01",   "",     false },

                // Testing validity of the input up to depth 4.  Self-testing
                // pattern roll up.  The short pattern that returns 'false' is
                // eliminated from patterns of longer depth.  For example, 't'
                // is first shortest 'false' pattern that tests that symbol 't'
                // from the soft delimiter set cannot appear as a first soft
                // delimiter.  All longer patterns that have first soft
                // delimiter 't' are eliminated.  Some elided patterns are left
                // in the table for illustration purposes and marked with "*
                // (pattern)".
                { L_,   "",     "",     "",     "",     true  }, // Depth 0
                { L_,   "stuv", "HIJK", "0123", "",     true  },
                { L_,   "stuv", "HIJK", "0123", "s",    true  }, // Depth 1
                { L_,   "stuv", "HIJK", "0123", "t",    false },
                { L_,   "stuv", "HIJK", "0123", "u",    false },
                { L_,   "stuv", "HIJK", "0123", "v",    false },
                { L_,   "stuv", "HIJK", "0123", "H",    true  },
                { L_,   "stuv", "HIJK", "0123", "I",    false },
                { L_,   "stuv", "HIJK", "0123", "J",    false },
                { L_,   "stuv", "HIJK", "0123", "K",    false },
                { L_,   "stuv", "HIJK", "0123", "0",    true  },
                { L_,   "stuv", "HIJK", "0123", "1",    false },
                { L_,   "stuv", "HIJK", "0123", "2",    false },
                { L_,   "stuv", "HIJK", "0123", "3",    false },
                { L_,   "stuv", "HIJK", "0123", "a",    false },
                { L_,   "stuv", "HIJK", "0123", "st",   true  }, // Depth 2
                { L_,   "stuv", "HIJK", "0123", "su",   false },
                { L_,   "stuv", "HIJK", "0123", "sv",   false },
                { L_,   "stuv", "HIJK", "0123", "sH",   true  },
                { L_,   "stuv", "HIJK", "0123", "sI",   false }, // * ("I")
                { L_,   "stuv", "HIJK", "0123", "s0",   true  },
                { L_,   "stuv", "HIJK", "0123", "s2",   false }, // * ("2")
                { L_,   "stuv", "HIJK", "0123", "Hs",   true  },
                { L_,   "stuv", "HIJK", "0123", "HI",   true  },
                { L_,   "stuv", "HIJK", "0123", "HJ",   false },
                { L_,   "stuv", "HIJK", "0123", "HK",   false },
                { L_,   "stuv", "HIJK", "0123", "H0",   true  },
                { L_,   "stuv", "HIJK", "0123", "0s",   true  },
                { L_,   "stuv", "HIJK", "0123", "0H",   true  },
                { L_,   "stuv", "HIJK", "0123", "01",   true  },
                { L_,   "stuv", "HIJK", "0123", "02",   false },
                { L_,   "stuv", "HIJK", "0123", "03",   false },
                { L_,   "stuv", "HIJK", "0123", "stu",  true  }, // Depth 3
                { L_,   "stuv", "HIJK", "0123", "stv",  false },
                { L_,   "stuv", "HIJK", "0123", "stH",  true  },
                { L_,   "stuv", "HIJK", "0123", "st0",  true  },
                { L_,   "stuv", "HIJK", "0123", "sHt",  true  },
                { L_,   "stuv", "HIJK", "0123", "sHu",  false }, // * ("su")
                { L_,   "stuv", "HIJK", "0123", "sHv",  false }, // * ("sv")
                { L_,   "stuv", "HIJK", "0123", "sHI",  true  },
                { L_,   "stuv", "HIJK", "0123", "sH0",  true  },
                { L_,   "stuv", "HIJK", "0123", "Hst",  true  },
                { L_,   "stuv", "HIJK", "0123", "Hs0",  true  },
                { L_,   "stuv", "HIJK", "0123", "H0s",  true  },
                { L_,   "stuv", "HIJK", "0123", "H0I",  true  },
                { L_,   "stuv", "HIJK", "0123", "H0J",  false }, // * ("HJ")
                { L_,   "stuv", "HIJK", "0123", "H0K",  false }, // * ("HK")
                { L_,   "stuv", "HIJK", "0123", "H01",  true  },
                { L_,   "stuv", "HIJK", "0123", "0st",  true  },
                { L_,   "stuv", "HIJK", "0123", "0sH",  true  },
                { L_,   "stuv", "HIJK", "0123", "0Hs",  true  },
                { L_,   "stuv", "HIJK", "0123", "0HI",  true  },
                { L_,   "stuv", "HIJK", "0123", "0HK",  false }, // * ("HK")
                { L_,   "stuv", "HIJK", "0123", "0H1",  true  },
                { L_,   "stuv", "HIJK", "0123", "0H2",  false }, // * ("02")
                { L_,   "stuv", "HIJK", "0123", "0H3",  false }, // * ("03")
                { L_,   "stuv", "HIJK", "0123", "01s",  true  },
                { L_,   "stuv", "HIJK", "0123", "01H",  true  },
                { L_,   "stuv", "HIJK", "0123", "012",  true  },

                //L_,   "stuv", "HIJK", "0123", "sH0a", false }, // template
                { L_,   "stuv", "HIJK", "0123", "stuv", true  }, // Depth 4
                { L_,   "stuv", "HIJK", "0123", "stuH", true  },
                { L_,   "stuv", "HIJK", "0123", "stu0", true  },
                { L_,   "stuv", "HIJK", "0123", "stHu", true  },
                { L_,   "stuv", "HIJK", "0123", "stHv", false }, // * ("stv")
                { L_,   "stuv", "HIJK", "0123", "stH0", true  },
                { L_,   "stuv", "HIJK", "0123", "sHtu", true  },
                { L_,   "stuv", "HIJK", "0123", "sHtv", false }, // * ("stv")
                { L_,   "stuv", "HIJK", "0123", "sHt2", false }, // * ("2")
                { L_,   "stuv", "HIJK", "0123", "sHt0", true  },
                { L_,   "stuv", "HIJK", "0123", "sHt1", false }, // * ("1")
                { L_,   "stuv", "HIJK", "0123", "sHIt", true  },
                { L_,   "stuv", "HIJK", "0123", "sHIJ", true  },
                { L_,   "stuv", "HIJK", "0123", "sHI0", true  },
                { L_,   "stuv", "HIJK", "0123", "sH0t", true  },
                { L_,   "stuv", "HIJK", "0123", "sH0I", true  },
                { L_,   "stuv", "HIJK", "0123", "sH01", true  },
                { L_,   "stuv", "HIJK", "0123", "s0tu", true  },
                { L_,   "stuv", "HIJK", "0123", "s0tH", true  },
                { L_,   "stuv", "HIJK", "0123", "s0Ht", true  },
                { L_,   "stuv", "HIJK", "0123", "s0HI", true  },
                { L_,   "stuv", "HIJK", "0123", "s0H1", true  },
                { L_,   "stuv", "HIJK", "0123", "s01t", true  },
                { L_,   "stuv", "HIJK", "0123", "s01H", true  },
                { L_,   "stuv", "HIJK", "0123", "s012", true  },
                { L_,   "stuv", "HIJK", "0123", "saI0", false }, // * ("a")
                { L_,   "stuv", "HIJK", "0123", "Hstu", true  },
                { L_,   "stuv", "HIJK", "0123", "Hst0", true  },
                { L_,   "stuv", "HIJK", "0123", "HsIt", true  },
                { L_,   "stuv", "HIJK", "0123", "HsI0", true  },
                { L_,   "stuv", "HIJK", "0123", "HsI1", false }, // * ("1")
                { L_,   "stuv", "HIJK", "0123", "HsIa", false }, // * ("a")
                { L_,   "stuv", "HIJK", "0123", "HIst", true  },
                { L_,   "stuv", "HIJK", "0123", "HIs0", true  },
                { L_,   "stuv", "HIJK", "0123", "HI0s", true  },
                { L_,   "stuv", "HIJK", "0123", "HI0J", true  },
                { L_,   "stuv", "HIJK", "0123", "HIJs", true  },
                { L_,   "stuv", "HIJK", "0123", "HIJK", true  },
                { L_,   "stuv", "HIJK", "0123", "HIJ0", true  },
                { L_,   "stuv", "HIJK", "0123", "HI0s", true  },
                { L_,   "stuv", "HIJK", "0123", "HI0J", true  },
                { L_,   "stuv", "HIJK", "0123", "HI01", true  },
                { L_,   "stuv", "HIJK", "0123", "H0st", true  },
                { L_,   "stuv", "HIJK", "0123", "H0s0", false }, // * ("aa")
                { L_,   "stuv", "HIJK", "0123", "H0s1", true  },
                { L_,   "stuv", "HIJK", "0123", "H01s", true  },
                { L_,   "stuv", "HIJK", "0123", "H012", true  },
                { L_,   "stuv", "HIJK", "0123", "0stu", true  },
                { L_,   "stuv", "HIJK", "0123", "0stH", true  },
                { L_,   "stuv", "HIJK", "0123", "0st1", true  },
                { L_,   "stuv", "HIJK", "0123", "0st2", false }, // * ("02")
                { L_,   "stuv", "HIJK", "0123", "0sHt", true  },
                { L_,   "stuv", "HIJK", "0123", "0sHI", true  },
                { L_,   "stuv", "HIJK", "0123", "0sH1", true  },
                { L_,   "stuv", "HIJK", "0123", "0s1t", true  },
                { L_,   "stuv", "HIJK", "0123", "0s1H", true  },
                { L_,   "stuv", "HIJK", "0123", "0s12", true  },
                { L_,   "stuv", "HIJK", "0123", "0Hst", true  },
                { L_,   "stuv", "HIJK", "0123", "0HsI", true  },
                { L_,   "stuv", "HIJK", "0123", "0Hs1", true  },
                { L_,   "stuv", "HIJK", "0123", "0HIs", true  },
                { L_,   "stuv", "HIJK", "0123", "0HIJ", true  },
                { L_,   "stuv", "HIJK", "0123", "0HI1", true  },
                { L_,   "stuv", "HIJK", "0123", "0H1s", true  },
                { L_,   "stuv", "HIJK", "0123", "0H12", true  },
                { L_,   "stuv", "HIJK", "0123", "0Ha1", false }, // * ("a")
                { L_,   "stuv", "HIJK", "0123", "012s", true  },
                { L_,   "stuv", "HIJK", "0123", "012t", false }, // * ("t")
                { L_,   "stuv", "HIJK", "0123", "012H", true  },
                { L_,   "stuv", "HIJK", "0123", "0123", true  },
            };
            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;
            for (size_t i = 0; i < DATA_LEN; ++i ) {
                const int       LINE      = DATA[i].d_line;
                const StringRef INPUT     = StringRef(DATA[i].d_input);
                const StringRef SOFT      = StringRef(DATA[i].d_soft);
                const StringRef HARD      = StringRef(DATA[i].d_hard);
                const StringRef TOKEN     = StringRef(DATA[i].d_token);
                const bool      EXP_VALID = DATA[i].d_isValid;

                if (veryVerbose) {
                    T_ P_(LINE) P_(INPUT) P_(SOFT) P_(HARD) P_(TOKEN)
                }

                bool VALID = isValid(INPUT, SOFT, HARD, TOKEN);
                ASSERTV(LINE, SOFT, HARD, TOKEN, INPUT, EXP_VALID == VALID);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'Tokenizer_Data'
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
        //   ~Tokenizer_Data();
        //   int inputType(char character) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'Tokenizer_Data'" << endl
                          << "========================" << endl;

        // Copy of the input character to input type mapping.

        enum InputType {
            TOK = 0,  // token character
            SFT = 1,  // soft delimiter character
            HRD = 2,  // hard delimiter character
        };

        typedef bdlb::Tokenizer_Data ObjData;

        if (verbose) cout << "\nTesting Tokenizer_Data(const StringRef&)."
                          << endl;
        {
            if (veryVerbose) cout <<
                      "\tTesting default constructed StringRef as a delimiter."
                                  << endl;
            {
                ObjData        mD((StringRef()));
                const ObjData& D = mD;

                for (int i = 0; i < 256; ++i) {
                    const char CH = static_cast<char>(i);
                    const int  INPUT_TYPE = D.inputType(CH);

                    if (veryVerbose) {
                        T_ P_(i) P(INPUT_TYPE)
                    }

                    ASSERTV(i, TOK == INPUT_TYPE);
                }
            }

            if (veryVerbose) cout <<
                                    "\tTesting empty StringRef as a delimiter."
                                  << endl;
            {
                ObjData        mD(StringRef(""));
                const ObjData& D = mD;

                for (int i = 0; i < 256; ++i) {
                    const char CH = static_cast<char>(i);
                    const int  INPUT_TYPE = D.inputType(CH);

                    if (veryVerbose) {
                        T_ P_(i) P(INPUT_TYPE)
                    }

                    ASSERTV(i, TOK == INPUT_TYPE);
                }
            }

            if (veryVerbose) cout <<
                                   "\tTesting single character as a delimiter."
                                  << endl;
            {
                for (int i = 0; i < 256; ++i) {
                    char delim[1];
                    delim[0] = static_cast<char>(i);

                    ObjData        mD(StringRef(delim, 1));
                    const ObjData& D = mD;

                    for (int j = 0; j < 256; j++) {
                        const char CH = static_cast<char>(j);
                        const int  INPUT_TYPE = D.inputType(CH);

                        if (veryVerbose) {
                            T_ P_(i) P(INPUT_TYPE)
                        }

                        if (i != j) {
                            ASSERTV(i, j, TOK == INPUT_TYPE);
                        } else {
                            ASSERTV(i, j, SFT == INPUT_TYPE);
                        }
                    }
                }
            }

            if (veryVerbose) cout <<
                                  "\tTesting duplicates in a delimiter string."
                                  << endl;
            {
                for (int i = 0; i < 256; ++i) {
                    char delim[2];
                    delim[0] = static_cast<char>(i);
                    delim[1] = static_cast<char>(i);

                    ObjData        mD(StringRef(delim, 2));
                    const ObjData& D = mD;

                    for (int j = 0; j < 256; j++) {
                        const char CH = static_cast<char>(j);
                        const int  INPUT_TYPE = D.inputType(CH);

                        if (veryVerbose) {
                            T_ P_(i) P(INPUT_TYPE)
                        }

                        if (i != j) {
                            ASSERTV(i, j, TOK == INPUT_TYPE);
                        } else {
                            ASSERTV(i, j, SFT == INPUT_TYPE);
                        }
                    }
                }
            }

            if (veryVerbose) cout <<
                                "\tTesting multiple characters as a delimiter."
                                  << endl;
            {
                char delim[256];
                for (int i = 0; i < 256; ++i) {
                    for (int j = 0; j < i; ++j) {
                        delim[j] = static_cast<char>(j);
                    }

                    ObjData        mD(StringRef(delim, i));
                    const ObjData& D = mD;

                    for (int j = 0; j < 256; ++j) {
                        const char CH = static_cast<char>(j);
                        const int  INPUT_TYPE = D.inputType(CH);

                        if (veryVerbose) {
                            T_ P_(i) P(INPUT_TYPE)
                        }

                        if (j >= i) {
                            ASSERTV(i, j, TOK == INPUT_TYPE);
                        } else {
                            ASSERTV(i, j, SFT == INPUT_TYPE);
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting Tokenizer_Data(const StringRef&, "
                          << "const StringRef&)."
                          << endl;
        {
            if (veryVerbose) cout <<
                       "\tTesting default constructed StringRef as delimiters."
                                  << endl;
            {
                ObjData        mD((StringRef()), (StringRef()));
                const ObjData& D = mD;

                for (int i = 0; i < 256; ++i) {
                    const char CH = static_cast<char>(i);
                    const int  INPUT_TYPE = D.inputType(CH);

                    if (veryVerbose) {
                        T_ P_(i) P(INPUT_TYPE)
                    }

                    ASSERTV(i, TOK == INPUT_TYPE);
                }
            }

            if (veryVerbose) cout << "\tTesting empty StringRef as delimiters."
                                  << endl;
            {
                ObjData        mD(StringRef(""), StringRef(""));
                const ObjData& D = mD;

                for (int i = 0; i < 256; ++i) {
                    const char CH = static_cast<char>(i);
                    const int  INPUT_TYPE = D.inputType(CH);

                    if (veryVerbose) {
                        T_ P_(i) P(INPUT_TYPE)
                    }

                    ASSERTV(i, TOK == INPUT_TYPE);
                }
            }

            if (veryVerbose) cout <<
                              "\tTesting single character as a soft delimiter."
                                  << endl;
            {
                for (int i = 0; i < 256; ++i) {
                    char delim[1];
                    delim[0] = static_cast<char>(i);

                    ObjData        mD(StringRef(delim, 1), (StringRef()));
                    const ObjData& D = mD;

                    for (int j = 0; j < 256; j++) {
                        const char CH = static_cast<char>(j);
                        const int  INPUT_TYPE = D.inputType(CH);

                        if (veryVerbose) {
                            T_ P_(i) P(INPUT_TYPE)
                        }

                        if (i != j) {
                            ASSERTV(i, j, TOK == INPUT_TYPE);
                        } else {
                            ASSERTV(i, j, SFT == INPUT_TYPE);
                        }
                    }
                }
            }

            if (veryVerbose) cout <<
                              "\tTesting single character as a hard delimiter."
                                  << endl;
            {
                for (int i = 0; i < 256; ++i) {
                    char delim[1];
                    delim[0] = static_cast<char>(i);

                    ObjData        mD(StringRef(""), StringRef(delim, 1));
                    const ObjData& D = mD;

                    for (int j = 0; j < 256; j++) {
                        const char CH = static_cast<char>(j);
                        const int  INPUT_TYPE = D.inputType(CH);

                        if (veryVerbose) {
                            T_ P_(i) P(INPUT_TYPE)
                        }

                        if (i != j) {
                            ASSERTV(i, j, TOK == INPUT_TYPE);
                        } else {
                            ASSERTV(i, j, HRD == INPUT_TYPE);
                        }
                    }
                }
            }

            if (veryVerbose) cout <<
                             "\tTesting duplicates in a hard delimiter string."
                                  << endl;
            {
                for (int i = 0; i < 256; ++i) {
                    char delim[2];
                    delim[0] = static_cast<char>(i);
                    delim[1] = static_cast<char>(i);

                    ObjData        mD(StringRef(""), StringRef(delim, 2));
                    const ObjData& D = mD;

                    for (int j = 0; j < 256; j++) {
                        const char CH = static_cast<char>(j);
                        const int  INPUT_TYPE = D.inputType(CH);

                        if (veryVerbose) {
                            T_ P_(i) P(INPUT_TYPE)
                        }

                        if (i != j) {
                            ASSERTV(i, j, TOK == INPUT_TYPE);
                        } else {
                            ASSERTV(i, j, HRD == INPUT_TYPE);
                        }
                    }
                }
            }

            if (veryVerbose) cout <<
                           "\tTesting multiple characters as hard delimiter."
                                  << endl;
            {
                char delim[256];
                for (int i = 0; i < 256; ++i) {
                    for (int j = 0; j < i; ++j) {
                        delim[j] = static_cast<char>(j);
                    }

                    ObjData        mD(StringRef(""), StringRef(delim, i));
                    const ObjData& D = mD;

                    for (int j = 0; j < 256; ++j) {
                        const char CH = static_cast<char>(j);
                        const int  INPUT_TYPE = D.inputType(CH);

                        if (veryVerbose) {
                            T_ P_(i) P(INPUT_TYPE)
                        }

                        if (j >= i) {
                            ASSERTV(i, j, TOK == INPUT_TYPE);
                        } else {
                            ASSERTV(i, j, HRD == INPUT_TYPE);
                        }
                    }
                }
            }

            if (veryVerbose) cout <<
                   "\tTesting hard delimiter precedence for single character."
                                  << endl;
            {
                for (int i = 0; i < 256; ++i) {
                    char delim[1];
                    delim[0] = static_cast<char>(i);

                    ObjData        mD(StringRef(delim, 1),
                                      StringRef(delim, 1));
                    const ObjData& D = mD;

                    for (int j = 0; j < 256; j++) {
                        const char CH = static_cast<char>(j);
                        const int  INPUT_TYPE = D.inputType(CH);

                        if (veryVerbose) {
                            T_ P_(i) P(INPUT_TYPE)
                        }

                        if (i != j) {
                            ASSERTV(i, j, TOK == INPUT_TYPE);
                        } else {
                            ASSERTV(i, j, HRD == INPUT_TYPE);
                        }
                    }
                }
            }

            if (veryVerbose) cout <<
                 "\tTesting hard delimiter precedence for multiple characters."
                                  << endl;
            {
                char delim[256];
                for (int i = 0; i < 256; ++i) {
                    for (int j = 0; j < i; ++j) {
                        delim[j] = static_cast<char>(j);
                    }

                    ObjData        mD(StringRef(delim, i),
                                      StringRef(delim, i));
                    const ObjData& D = mD;

                    for (int j = 0; j < 256; ++j) {
                        const char CH = static_cast<char>(j);
                        const int  INPUT_TYPE = D.inputType(CH);

                        if (veryVerbose) {
                            T_ P_(i) P(INPUT_TYPE)
                        }

                        if (j >= i) {
                            ASSERTV(i, j, TOK == INPUT_TYPE);
                        } else {
                            ASSERTV(i, j, HRD == INPUT_TYPE);
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
                if (veryVeryVerbose) {
                    cout << "|\t"
                         << '"' << tokenizer.token() << '"'
                         << "\t"
                         << '"' << tokenizer.trailingDelimiter() << '"'
                         << "\tSoft?: "
                         << (tokenizer.hasTrailingSoft() ? "T":"F")
                         << "\tPSoft?: "
                         << (tokenizer.hasPreviousSoft() ? "T":"F")
                         << "\tHard?: "
                         << (tokenizer.isTrailingHard() ? "T":"F")
                         << "\tPHard?: "
                         << (tokenizer.isPreviousHard() ? "T":"F")
                         << endl;
                }
                ++tokenizer;
            }
        }

        if (verbose) cout << "\n'bdlb::Tokenizer' with StringRef."  << endl;
        {
            Obj tokenizer(StringRef(",,Hello, world,,,"),
                          StringRef(" "),
                          StringRef(","));

            while (tokenizer.isValid()) {
                if (veryVeryVerbose) {
                    cout << "|\t"
                         << '"' << tokenizer.token() << '"'
                         << "\t"
                         << '"' << tokenizer.trailingDelimiter() << '"'
                         << "\tSoft?: "
                         << (tokenizer.hasTrailingSoft() ? "T":"F")
                         << "\tPSoft?: "
                         << (tokenizer.hasPreviousSoft() ? "T":"F")
                         << "\tHard?: "
                         << (tokenizer.isTrailingHard() ? "T":"F")
                         << "\tPHard?: "
                         << (tokenizer.isPreviousHard() ? "T":"F")
                         << endl;
                }
                ++tokenizer;
            }
        }

        if (verbose) cout << "\n'bdlb::Tokenizer' with StringRef."  << endl;
        {
            Obj tokenizer(StringRef("   I've : been a : :bad   boy!"),
                          StringRef(" "),
                          StringRef(":/"));

            for (; tokenizer.isValid(); ++tokenizer) {
                if (veryVeryVerbose) {
                    cout << "|\t"
                         << '"' << tokenizer.token() << '"'
                         << "\t"
                         << '"' << tokenizer.trailingDelimiter() << '"'
                         << "\tSoft?: "
                         << (tokenizer.hasTrailingSoft() ? "T":"F")
                         << "\tPSoft?: "
                         << (tokenizer.hasPreviousSoft() ? "T":"F")
                         << "\tHard?: "
                         << (tokenizer.isTrailingHard() ? "T":"F")
                         << "\tPHard?: "
                         << (tokenizer.isPreviousHard() ? "T":"F")
                         << endl;
                }
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
                if (veryVeryVerbose) cout << "|\t"
                                          << '"' << *it << '"'
                                          << endl;

            }
            ++tokenizer;
            ++tokenizer;

            for (Obj::iterator it=tokenizer.begin(), end = tokenizer.end();
                               it != end;
                               ++it) {
                if (veryVeryVerbose) {
                    cout << "|\t"
                         << '"' << *it << '"'
                         << endl;
                }
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
                if (veryVeryVerbose) cout << "|\t"
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
